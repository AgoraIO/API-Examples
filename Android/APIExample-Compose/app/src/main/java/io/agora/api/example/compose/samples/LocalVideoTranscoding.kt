package io.agora.api.example.compose.samples

import android.graphics.Color
import android.util.Log
import android.view.View
import android.widget.Toast
import androidx.activity.compose.rememberLauncherForActivityResult
import androidx.activity.result.contract.ActivityResultContracts
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.height
import androidx.compose.runtime.Composable
import androidx.compose.runtime.DisposableEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableIntStateOf
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.saveable.rememberSaveable
import androidx.compose.runtime.setValue
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.platform.LocalLifecycleOwner
import androidx.compose.ui.platform.LocalSoftwareKeyboardController
import androidx.compose.ui.unit.dp
import io.agora.api.example.compose.BuildConfig
import io.agora.api.example.compose.data.SettingPreferences
import io.agora.api.example.compose.ui.common.ChannelNameInput
import io.agora.api.example.compose.ui.common.SwitchRaw
import io.agora.api.example.compose.ui.common.TwoVideoView
import io.agora.api.example.compose.ui.common.TwoVideoViewType
import io.agora.api.example.compose.ui.common.VideoStatsInfo
import io.agora.mediaplayer.data.MediaPlayerSource
import io.agora.rtc2.ChannelMediaOptions
import io.agora.rtc2.Constants
import io.agora.rtc2.Constants.VideoSourceType
import io.agora.rtc2.IRtcEngineEventHandler
import io.agora.rtc2.LocalTranscoderConfiguration
import io.agora.rtc2.LocalTranscoderConfiguration.TranscodingVideoStream
import io.agora.rtc2.RtcEngine
import io.agora.rtc2.RtcEngineConfig
import io.agora.rtc2.video.CameraCapturerConfiguration
import io.agora.rtc2.video.SegmentationProperty
import io.agora.rtc2.video.VideoCanvas
import io.agora.rtc2.video.VideoEncoderConfiguration
import io.agora.rtc2.video.VirtualBackgroundSource

@Composable
fun LocalVideoTranscoding() {
    val context = LocalContext.current
    val lifecycleOwner = LocalLifecycleOwner.current
    val keyboard = LocalSoftwareKeyboardController.current
    var isJoined by rememberSaveable { mutableStateOf(false) }
    var channelName by rememberSaveable { mutableStateOf("") }
    var localUid by rememberSaveable { mutableIntStateOf(0) }
    var remoteUid by rememberSaveable { mutableIntStateOf(0) }
    var localStats by remember { mutableStateOf(VideoStatsInfo()) }
    var remoteStats by remember { mutableStateOf(VideoStatsInfo()) }

    val rtcEngine = remember {
        RtcEngine.create(RtcEngineConfig().apply {
            mContext = context
            mAppId = BuildConfig.AGORA_APP_ID
            mEventHandler = object : IRtcEngineEventHandler() {
                override fun onJoinChannelSuccess(channel: String?, uid: Int, elapsed: Int) {
                    super.onJoinChannelSuccess(channel, uid, elapsed)
                    isJoined = true
                    localUid = uid
                }

                override fun onLeaveChannel(stats: RtcStats?) {
                    super.onLeaveChannel(stats)
                    isJoined = false
                    localUid = 0
                    localStats = VideoStatsInfo()
                    remoteUid = 0
                    remoteStats = VideoStatsInfo()
                }

                override fun onUserJoined(uid: Int, elapsed: Int) {
                    super.onUserJoined(uid, elapsed)
                    remoteUid = uid
                    remoteStats = VideoStatsInfo()
                }

                override fun onUserOffline(uid: Int, reason: Int) {
                    super.onUserOffline(uid, reason)
                    if (remoteUid == uid) {
                        remoteUid = 0
                        remoteStats = VideoStatsInfo()
                    }
                }

                override fun onRtcStats(stats: RtcStats?) {
                    super.onRtcStats(stats)
                    localStats.copy(rtcStats = stats).let {
                        localStats = it
                    }
                }

                override fun onLocalVideoStats(
                    source: Constants.VideoSourceType?,
                    stats: LocalVideoStats?
                ) {
                    super.onLocalVideoStats(source, stats)
                    localStats.copy(localVideoStats = stats).let {
                        localStats = it
                    }
                }

                override fun onLocalAudioStats(stats: LocalAudioStats?) {
                    super.onLocalAudioStats(stats)
                    localStats.copy(localAudioStats = stats).let {
                        localStats = it
                    }
                }

                override fun onRemoteVideoStats(stats: RemoteVideoStats?) {
                    super.onRemoteVideoStats(stats)
                    val uid = stats?.uid ?: return
                    if (remoteUid == uid) {
                        remoteStats.copy(remoteVideoStats = stats).let {
                            remoteStats = it
                        }
                    }
                }

                override fun onRemoteAudioStats(stats: RemoteAudioStats?) {
                    super.onRemoteAudioStats(stats)
                    val uid = stats?.uid ?: return
                    if (remoteUid == uid) {
                        remoteStats.copy(remoteAudioStats = stats).let {
                            remoteStats = it
                        }
                    }
                }

                override fun onLocalVideoTranscoderError(
                    stream: TranscodingVideoStream?,
                    error: Int
                ) {
                    super.onLocalVideoTranscoderError(stream, error)
                    Log.e("LocalVideoTranscoder", "error: $error")
                }

            }
        }).apply {
            setVideoEncoderConfiguration(
                VideoEncoderConfiguration(
                    SettingPreferences.getVideoDimensions(),
                    SettingPreferences.getVideoFrameRate(),
                    VideoEncoderConfiguration.STANDARD_BITRATE,
                    SettingPreferences.getOrientationMode()
                )
            )
            enableVideo()
        }
    }
    val player = remember {
        rtcEngine.createMediaPlayer().apply {
            val source = MediaPlayerSource()
            source.url = "https://agora-adc-artifacts.s3.cn-north-1.amazonaws.com.cn/resources/sample.mp4"
            source.isAutoPlay = false
            openWithMediaSource(source)
            adjustPlayoutVolume(0)
        }
    }
    DisposableEffect(lifecycleOwner) {
        onDispose {
            rtcEngine.stopPreview()
            player.stop()
            player.destroy()
            rtcEngine.stopCameraCapture(VideoSourceType.VIDEO_SOURCE_CAMERA_PRIMARY)
            rtcEngine.leaveChannel()
            RtcEngine.destroy()
        }
    }

    val permissionLauncher =
        rememberLauncherForActivityResult(contract = ActivityResultContracts.RequestMultiplePermissions()) { grantedMap ->
            val allGranted = grantedMap.values.all { it }
            if (allGranted) {
                // Permission is granted
                Toast.makeText(context, "Permission Granted", Toast.LENGTH_LONG).show()
                val dimension = SettingPreferences.getVideoDimensions()
                val width = dimension.width
                val height = dimension.height
                val config = LocalTranscoderConfiguration()

                val playerStream = TranscodingVideoStream()
                playerStream.sourceType = VideoSourceType.VIDEO_SOURCE_MEDIA_PLAYER
                playerStream.mediaPlayerId = player.mediaPlayerId
                playerStream.width = width
                playerStream.height = height
                playerStream.zOrder = 1
                config.transcodingVideoStreams.add(playerStream)
                player.play()

                val cameraStream = TranscodingVideoStream()
                cameraStream.sourceType = VideoSourceType.VIDEO_SOURCE_CAMERA_PRIMARY
                cameraStream.width = width / 2
                cameraStream.height = height / 2
                cameraStream.x = 0
                cameraStream.y = height / 2
                cameraStream.zOrder = 2
                cameraStream.mirror = true
                config.transcodingVideoStreams.add(cameraStream)
                rtcEngine.startCameraCapture(
                    VideoSourceType.VIDEO_SOURCE_CAMERA_PRIMARY,
                    CameraCapturerConfiguration(CameraCapturerConfiguration.CAMERA_DIRECTION.CAMERA_FRONT)
                )

                rtcEngine.startLocalVideoTranscoder(config)

                val mediaOptions = ChannelMediaOptions()
                mediaOptions.channelProfile = Constants.CHANNEL_PROFILE_LIVE_BROADCASTING
                mediaOptions.clientRoleType = Constants.CLIENT_ROLE_BROADCASTER
                mediaOptions.autoSubscribeVideo = true
                mediaOptions.autoSubscribeAudio = true
                mediaOptions.publishMicrophoneTrack = false
                mediaOptions.publishCameraTrack = false
                mediaOptions.publishTranscodedVideoTrack = true
                rtcEngine.joinChannel("", channelName, 0, mediaOptions)
            } else {
                // Permission is denied
                Toast.makeText(context, "Permission Denied", Toast.LENGTH_LONG).show()
            }
        }

    LocalVideoTranscodingView(
        channelName = channelName,
        isJoined = isJoined,
        localUid = localUid,
        remoteUid = remoteUid,
        localStats = localStats,
        remoteStats = remoteStats,
        onJoinClick = {
            keyboard?.hide()
            channelName = it
            permissionLauncher.launch(
                arrayOf(
                    android.Manifest.permission.RECORD_AUDIO,
                    android.Manifest.permission.CAMERA,
                )
            )
        },
        onLeaveClick = {
            rtcEngine.stopPreview()
            player.pause()
            rtcEngine.stopCameraCapture(VideoSourceType.VIDEO_SOURCE_CAMERA_PRIMARY)
            rtcEngine.stopLocalVideoTranscoder()
            rtcEngine.leaveChannel()
        },
        localRender = { view, id ->
            val canvas = VideoCanvas(view, Constants.RENDER_MODE_FIT, 0)
            canvas.sourceType = VideoSourceType.VIDEO_SOURCE_TRANSCODED.value
            canvas.mirrorMode = Constants.VIDEO_MIRROR_MODE_DISABLED
            rtcEngine.setupLocalVideo(canvas)
            rtcEngine.startPreview(VideoSourceType.VIDEO_SOURCE_TRANSCODED)
        },
        remoteRender = { view, id ->
            rtcEngine.setupRemoteVideo(VideoCanvas(view, Constants.RENDER_MODE_HIDDEN, id))
        },
        onAlphaBackground = {
            rtcEngine.enableVirtualBackground(
                it,
                VirtualBackgroundSource(
                    VirtualBackgroundSource.BACKGROUND_COLOR,
                    Color.TRANSPARENT, "",
                    VirtualBackgroundSource.BLUR_DEGREE_HIGH
                ),
                SegmentationProperty()
            )
        }
    )
}

@Composable
fun LocalVideoTranscodingView(
    channelName: String,
    isJoined: Boolean,
    localUid: Int = 0,
    remoteUid: Int = 0,
    localStats: VideoStatsInfo = VideoStatsInfo(),
    remoteStats: VideoStatsInfo = VideoStatsInfo(),
    onJoinClick: (String) -> Unit,
    onLeaveClick: () -> Unit,
    localRender: (View, Int) -> Unit,
    remoteRender: (View, Int) -> Unit,
    onAlphaBackground: (enable: Boolean)->Unit
) {
    Column {
        TwoVideoView(
            modifier = Modifier.height(250.dp),
            type = TwoVideoViewType.Row,
            localUid = localUid,
            remoteUid = remoteUid,
            localStats = localStats,
            remoteStats = remoteStats,
            localRender = { view, id, _ -> localRender(view, id) },
            remoteRender = { view, id, _ -> remoteRender(view, id) }
        )
        Spacer(modifier = Modifier.weight(1.0f))
        SwitchRaw(title = "透明背景"){
            onAlphaBackground(it)
        }
        ChannelNameInput(
            channelName = channelName,
            isJoined = isJoined,
            onJoinClick = onJoinClick,
            onLeaveClick = onLeaveClick
        )
    }
}