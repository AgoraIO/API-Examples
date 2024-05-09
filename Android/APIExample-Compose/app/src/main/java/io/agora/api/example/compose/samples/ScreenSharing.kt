package io.agora.api.example.compose.samples

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
import androidx.compose.ui.res.stringResource
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import io.agora.api.example.compose.BuildConfig
import io.agora.api.example.compose.R
import io.agora.api.example.compose.data.SettingPreferences
import io.agora.api.example.compose.ui.common.ChannelNameInput
import io.agora.api.example.compose.ui.common.DropdownMenuRaw
import io.agora.api.example.compose.ui.common.SliderRaw
import io.agora.api.example.compose.ui.common.SwitchRaw
import io.agora.api.example.compose.ui.common.TwoVideoView
import io.agora.api.example.compose.ui.common.TwoVideoViewType
import io.agora.api.example.compose.ui.common.VideoStatsInfo
import io.agora.api.example.compose.utils.TokenUtils
import io.agora.rtc2.ChannelMediaOptions
import io.agora.rtc2.Constants
import io.agora.rtc2.IRtcEngineEventHandler
import io.agora.rtc2.RtcEngine
import io.agora.rtc2.RtcEngineConfig
import io.agora.rtc2.ScreenCaptureParameters
import io.agora.rtc2.video.VideoCanvas
import io.agora.rtc2.video.VideoEncoderConfiguration

@Composable
fun ScreenSharing() {
    val context = LocalContext.current
    val lifecycleOwner = LocalLifecycleOwner.current
    val keyboard = LocalSoftwareKeyboardController.current
    var isJoined by rememberSaveable { mutableStateOf(false) }
    var channelName by rememberSaveable { mutableStateOf("") }
    var localUid by rememberSaveable { mutableIntStateOf(0) }
    var remoteUid by rememberSaveable { mutableIntStateOf(0) }
    var localStats by remember { mutableStateOf(VideoStatsInfo()) }
    var remoteStats by remember { mutableStateOf(VideoStatsInfo()) }
    var screenUid by rememberSaveable { mutableIntStateOf(0) }
    var isScreenPreview by rememberSaveable { mutableStateOf(true) }
    var isScreenSharing by rememberSaveable { mutableStateOf(false) }

    val screenCaptureParameters = remember {
        ScreenCaptureParameters()
    }
    val rtcEngine = remember {
        RtcEngine.create(RtcEngineConfig().apply {
            mAreaCode = SettingPreferences.getArea()
            mContext = context
            mAppId = BuildConfig.AGORA_APP_ID
            mEventHandler = object : IRtcEngineEventHandler() {
                override fun onJoinChannelSuccess(channel: String?, uid: Int, elapsed: Int) {
                    super.onJoinChannelSuccess(channel, uid, elapsed)
                    isJoined = true
                    localUid = uid
                    if(isScreenPreview){
                        screenUid = localUid
                    }
                }

                override fun onLeaveChannel(stats: RtcStats?) {
                    super.onLeaveChannel(stats)
                    isJoined = false
                    localUid = 0
                    localStats = VideoStatsInfo()
                    remoteUid = 0
                    remoteStats = VideoStatsInfo()
                    screenUid = 0
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

                override fun onLocalVideoStateChanged(
                    source: Constants.VideoSourceType?,
                    state: Int,
                    reason: Int
                ) {
                    super.onLocalVideoStateChanged(source, state, reason)
                    Log.d("ScreenSharing", "onLocalVideoStateChanged: $source $state $reason")
                    if(source == Constants.VideoSourceType.VIDEO_SOURCE_SCREEN_PRIMARY){
                        if (state == Constants.LOCAL_VIDEO_STREAM_STATE_CAPTURING) {
                            isScreenSharing = true
                        } else if (
                            state == Constants.LOCAL_VIDEO_STREAM_STATE_FAILED
                            || state == Constants.LOCAL_VIDEO_STREAM_STATE_STOPPED
                        ) {
                            isScreenSharing = false
                        }
                    }
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
    DisposableEffect(lifecycleOwner) {
        onDispose {
            rtcEngine.stopScreenCapture()
            rtcEngine.stopPreview(Constants.VideoSourceType.VIDEO_SOURCE_SCREEN_PRIMARY)
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

                rtcEngine.startScreenCapture(screenCaptureParameters)
                val mediaOptions = ChannelMediaOptions()
                mediaOptions.channelProfile = Constants.CHANNEL_PROFILE_LIVE_BROADCASTING
                mediaOptions.clientRoleType = Constants.CLIENT_ROLE_BROADCASTER
                mediaOptions.autoSubscribeAudio = true
                mediaOptions.autoSubscribeVideo = true
                mediaOptions.publishCameraTrack = false
                mediaOptions.publishMicrophoneTrack = false
                mediaOptions.publishScreenCaptureAudio = true
                mediaOptions.publishScreenCaptureVideo = true
                TokenUtils.gen(channelName, 0) {
                    rtcEngine.joinChannel(it, channelName, 0, mediaOptions)
                }
            } else {
                // Permission is denied
                Toast.makeText(context, "Permission Denied", Toast.LENGTH_LONG).show()
            }
        }
    ScreenSharingView(
        channelName = channelName,
        isJoined = isJoined,
        localUid = screenUid,
        remoteUid = remoteUid,
        localStats = localStats,
        remoteStats = remoteStats,
        isScreenPreview = isScreenPreview,
        isScreenSharing = isScreenSharing,
        localRender = { view, id ->
            val videoCanvas = VideoCanvas(view, Constants.RENDER_MODE_FIT, id)
            videoCanvas.sourceType = Constants.VideoSourceType.VIDEO_SOURCE_SCREEN_PRIMARY.value
            videoCanvas.mirrorMode = Constants.VIDEO_MIRROR_MODE_DISABLED
            rtcEngine.setupLocalVideo(videoCanvas)

            rtcEngine.startPreview(Constants.VideoSourceType.VIDEO_SOURCE_SCREEN_PRIMARY)
        },
        remoteRender = { view, id ->
            rtcEngine.setupRemoteVideo(VideoCanvas(view, Constants.RENDER_MODE_HIDDEN, id))
        },
        onJoinClick = {
            if (it.isEmpty()) {
                Toast.makeText(context, "Channel Name is empty", Toast.LENGTH_LONG).show()
                return@ScreenSharingView
            }
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
            rtcEngine.stopScreenCapture()
            rtcEngine.stopPreview(Constants.VideoSourceType.VIDEO_SOURCE_SCREEN_PRIMARY)
            rtcEngine.leaveChannel()
        },
        onScreenPreview = {
            isScreenPreview = it
            if (it) {
                screenUid = localUid
            } else {
                screenUid = 0
                val videoCanvas = VideoCanvas(null, Constants.RENDER_MODE_HIDDEN, localUid)
                videoCanvas.sourceType = Constants.VideoSourceType.VIDEO_SOURCE_SCREEN_PRIMARY.value
                videoCanvas.mirrorMode = Constants.VIDEO_MIRROR_MODE_DISABLED
                rtcEngine.setupLocalVideo(videoCanvas)

                rtcEngine.stopPreview(
                    Constants.VideoSourceType.VIDEO_SOURCE_SCREEN_PRIMARY
                )
            }
        },
        onScreenAudio = {
            screenCaptureParameters.captureAudio = it
            rtcEngine.updateScreenCaptureParameters(screenCaptureParameters)
        },
        onScreenScenario = {
            rtcEngine.setScreenCaptureScenario(it)
        },
        onScreenVolume = {
            screenCaptureParameters.audioCaptureParameters.captureSignalVolume = (it * 100).toInt()
            rtcEngine.updateScreenCaptureParameters(screenCaptureParameters)
        }
    )
}


@Composable
private fun ScreenSharingView(
    channelName: String,
    isJoined: Boolean,
    localUid: Int = 0,
    remoteUid: Int = 0,
    localStats: VideoStatsInfo = VideoStatsInfo(),
    remoteStats: VideoStatsInfo = VideoStatsInfo(),
    localRender: (view: View, id: Int) -> Unit = { _, _-> },
    remoteRender: (view: View, id: Int) -> Unit = { _, _ -> },
    isScreenPreview: Boolean = true,
    isScreenSharing: Boolean = false,
    onJoinClick: (String) -> Unit,
    onLeaveClick: () -> Unit,
    onScreenPreview: (Boolean) -> Unit,
    onScreenAudio: (Boolean) -> Unit,
    onScreenScenario: (Constants.ScreenScenarioType) -> Unit,
    onScreenVolume: (Float) -> Unit,
) {
    Column {
        TwoVideoView(
            modifier = Modifier.height(400.dp),
            type = TwoVideoViewType.Row,
            localUid = localUid,
            remoteUid = remoteUid,
            localStats = localStats,
            remoteStats = remoteStats,
            localRender = { view, id, _ -> localRender(view, id) },
            remoteRender = { view, id, _ -> remoteRender(view, id) },
        )

        Spacer(modifier = Modifier.weight(1.0f))

        SwitchRaw(
            title = stringResource(id = R.string.screen_sharing_local_preview),
            checked = isScreenPreview,
            enable = isJoined && isScreenSharing
        ) {
            onScreenPreview(it)
        }
        SwitchRaw(
            title = stringResource(id = R.string.screen_sharing_audio),
            enable = isJoined && isScreenSharing
        ) {
            onScreenAudio(it)
        }
        DropdownMenuRaw(
            title = "Scenario Type",
            options = listOf(
                "Document" to Constants.ScreenScenarioType.SCREEN_SCENARIO_DOCUMENT,
                "Gaming" to Constants.ScreenScenarioType.SCREEN_SCENARIO_GAMING,
                "Video" to Constants.ScreenScenarioType.SCREEN_SCENARIO_VIDEO,
            ),
            enable = isJoined && isScreenSharing
        ) { _, option ->
            onScreenScenario(option.second)
        }
        SliderRaw(
            title = "Capture Volume",
            value = 1.0f,
            enable = isJoined && isScreenSharing
        ) {
            onScreenVolume(it)
        }
        ChannelNameInput(
            channelName = channelName,
            isJoined = isJoined,
            onJoinClick = onJoinClick,
            onLeaveClick = onLeaveClick
        )
    }
}


@Preview
@Composable
private fun ScreenSharingViewPreview() {
    ScreenSharingView(
        channelName = "test",
        isJoined = false,
        localUid = 1,
        onJoinClick = {},
        onLeaveClick = { },
        onScreenPreview = {},
        onScreenAudio = {},
        onScreenScenario = {}
    ) {

    }
}
