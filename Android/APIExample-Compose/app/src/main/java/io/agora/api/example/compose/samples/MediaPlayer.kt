package io.agora.api.example.compose.samples

import android.util.Log
import android.view.View
import android.widget.Toast
import androidx.activity.compose.rememberLauncherForActivityResult
import androidx.activity.result.contract.ActivityResultContracts
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.padding
import androidx.compose.material3.Button
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.DisposableEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableFloatStateOf
import androidx.compose.runtime.mutableIntStateOf
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.saveable.rememberSaveable
import androidx.compose.runtime.setValue
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.platform.LocalLifecycleOwner
import androidx.compose.ui.platform.LocalSoftwareKeyboardController
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import io.agora.api.example.compose.BuildConfig
import io.agora.api.example.compose.data.SettingPreferences
import io.agora.api.example.compose.ui.common.ChannelNameInput
import io.agora.api.example.compose.ui.common.InputRaw
import io.agora.api.example.compose.ui.common.SliderRaw
import io.agora.api.example.compose.ui.common.TwoVideoView
import io.agora.api.example.compose.ui.common.VideoStatsInfo
import io.agora.api.example.compose.utils.TokenUtils
import io.agora.mediaplayer.IMediaPlayerObserver
import io.agora.mediaplayer.data.CacheStatistics
import io.agora.mediaplayer.data.PlayerPlaybackStats
import io.agora.mediaplayer.data.PlayerUpdatedInfo
import io.agora.mediaplayer.data.SrcInfo
import io.agora.rtc2.ChannelMediaOptions
import io.agora.rtc2.Constants
import io.agora.rtc2.IRtcEngineEventHandler
import io.agora.rtc2.RtcEngine
import io.agora.rtc2.RtcEngineConfig
import io.agora.rtc2.video.VideoCanvas
import io.agora.rtc2.video.VideoEncoderConfiguration

@Composable
fun MediaPlayer() {
    val context = LocalContext.current
    val lifecycleOwner = LocalLifecycleOwner.current
    val keyboard = LocalSoftwareKeyboardController.current
    var isJoined by rememberSaveable { mutableStateOf(false) }
    var localLarge by rememberSaveable { mutableStateOf(true) }
    var channelName by rememberSaveable { mutableStateOf("") }
    var localUid by rememberSaveable { mutableIntStateOf(0) }
    var remoteUid by rememberSaveable { mutableIntStateOf(0) }
    var localStats by remember { mutableStateOf(VideoStatsInfo()) }
    var remoteStats by remember { mutableStateOf(VideoStatsInfo()) }
    var playerUrl by rememberSaveable { mutableStateOf("https://agora-adc-artifacts.s3.cn-north-1.amazonaws.com.cn/resources/sample.mp4") }
    var playerProgress by rememberSaveable { mutableFloatStateOf(0.0f) }
    var playerReady by rememberSaveable { mutableStateOf(false) }

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
            registerPlayerObserver(object: IMediaPlayerObserver {
                override fun onPlayerStateChanged(
                    state: io.agora.mediaplayer.Constants.MediaPlayerState?,
                    reason: io.agora.mediaplayer.Constants.MediaPlayerReason?
                ) {
                    Log.d("MediaPlayer", "onPlayerStateChanged: $state, $reason")
                    if (state == io.agora.mediaplayer.Constants.MediaPlayerState.PLAYER_STATE_OPEN_COMPLETED) {
                        playerReady = true
                        play()
                    } else if (state == io.agora.mediaplayer.Constants.MediaPlayerState.PLAYER_STATE_STOPPED) {
                        playerReady = false
                        val mediaOptions = ChannelMediaOptions()
                        mediaOptions.clientRoleType = Constants.CLIENT_ROLE_AUDIENCE
                        mediaOptions.publishMediaPlayerVideoTrack = false
                        mediaOptions.publishMediaPlayerAudioTrack = false
                        rtcEngine.updateChannelMediaOptions(mediaOptions)
                    }
                }

                override fun onPositionChanged(positionMs: Long, timestampMs: Long) {
                    playerProgress = positionMs.toFloat() / duration.toFloat()
                    Log.d("MediaPlayer", "onPositionChanged: $positionMs, $timestampMs, $playerProgress")
                }

                override fun onPlayerEvent(
                    eventCode: io.agora.mediaplayer.Constants.MediaPlayerEvent?,
                    elapsedTime: Long,
                    message: String?
                ) {
                    Log.d("MediaPlayer", "onPlayerEvent: $eventCode, $elapsedTime, $message")
                }

                override fun onMetaData(
                    type: io.agora.mediaplayer.Constants.MediaPlayerMetadataType?,
                    data: ByteArray?
                ) {
                    Log.d("MediaPlayer", "onMetaData: $type, $data")
                }

                override fun onPlayBufferUpdated(playCachedBuffer: Long) {
                    Log.d("MediaPlayer", "onPlayBufferUpdated: $playCachedBuffer")
                }

                override fun onPreloadEvent(
                    src: String?,
                    event: io.agora.mediaplayer.Constants.MediaPlayerPreloadEvent?
                ) {
                    Log.d("MediaPlayer", "onPreloadEvent: $src, $event")
                }

                override fun onAgoraCDNTokenWillExpire() {
                    Log.d("MediaPlayer", "onAgoraCDNTokenWillExpire")
                }

                override fun onPlayerSrcInfoChanged(from: SrcInfo?, to: SrcInfo?) {
                    Log.d("MediaPlayer", "onPlayerSrcInfoChanged: $from, $to")
                }

                override fun onPlayerInfoUpdated(info: PlayerUpdatedInfo?) {
                    Log.d("MediaPlayer", "onPlayerInfoUpdated: $info")
                }

                override fun onPlayerCacheStats(stats: CacheStatistics?) {
                    Log.d("MediaPlayer", "onPlayerCacheStats: $stats")
                }

                override fun onPlayerPlaybackStats(stats: PlayerPlaybackStats?) {
                    Log.d("MediaPlayer", "onPlayerPlaybackStats: $stats")
                }

                override fun onAudioVolumeIndication(volume: Int) {
                    Log.d("MediaPlayer", "onAudioVolumeIndication: $volume")
                }
            })
        }
    }


    DisposableEffect(lifecycleOwner) {
        onDispose {
            player?.destroy()
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
                val mediaOptions = ChannelMediaOptions()
                mediaOptions.channelProfile = Constants.CHANNEL_PROFILE_LIVE_BROADCASTING
                mediaOptions.clientRoleType = Constants.CLIENT_ROLE_AUDIENCE
                mediaOptions.autoSubscribeAudio = true
                mediaOptions.autoSubscribeVideo = true
                mediaOptions.publishCameraTrack = false
                mediaOptions.publishMicrophoneTrack = false
                TokenUtils.gen(channelName, 0) {
                    rtcEngine.joinChannel(it, channelName, 0, mediaOptions)
                }
            } else {
                // Permission is denied
                Toast.makeText(context, "Permission Denied", Toast.LENGTH_LONG).show()
            }
        }
    MediaPlayerView(
        channelName = channelName,
        isJoined = isJoined,
        localUid = localUid,
        remoteUid = remoteUid,
        localLarge = localLarge,
        localStats = localStats,
        remoteStats = remoteStats,
        localRender = { view, uid, _ ->
            val videoCanvas = VideoCanvas(view as View, Constants.RENDER_MODE_HIDDEN, uid)
            videoCanvas.sourceType = Constants.VideoSourceType.VIDEO_SOURCE_MEDIA_PLAYER.value
            videoCanvas.mediaPlayerId = player?.mediaPlayerId ?: 0
            rtcEngine.setupLocalVideo(videoCanvas)
        },
        remoteRender = { view, uid, _ ->
            rtcEngine.setupRemoteVideo(VideoCanvas(view as View, Constants.RENDER_MODE_HIDDEN, uid))
        },
        onSwitch = {
            localLarge = !localLarge
        },
        onJoinClick = {
            keyboard?.hide()
            channelName = it
            permissionLauncher.launch(
                arrayOf(
                    android.Manifest.permission.RECORD_AUDIO,
                    android.Manifest.permission.CAMERA
                )
            )
        },
        onLeaveClick = {
            player?.stop()
            rtcEngine.leaveChannel()
        },
        playerUrl = playerUrl,
        playerReady = playerReady,
        playerProgress = playerProgress,
        onOpenClick = {
            keyboard?.hide()
            player?.open(playerUrl,0)
            playerUrl = it
        },
        onPlayClick = {
            player?.play()
        },
        onStopClick = {
            player?.stop()
        },
        onPauseClick = {
            player?.pause()
        },
        onPushClick = {
            val mediaOptions = ChannelMediaOptions()
            mediaOptions.clientRoleType = Constants.CLIENT_ROLE_BROADCASTER
            mediaOptions.publishMediaPlayerVideoTrack = true
            mediaOptions.publishMediaPlayerAudioTrack = true
            mediaOptions.publishMediaPlayerId = player?.mediaPlayerId ?: 0
            rtcEngine.updateChannelMediaOptions(mediaOptions)
        },
        onProgressChange = {
            playerProgress = it
            player?.seek((it * player.duration).toLong())
        }
    )
}

@Composable
private fun MediaPlayerView(
    channelName: String,
    isJoined: Boolean,
    localUid: Int = 0,
    remoteUid: Int = 0,
    playerReady: Boolean = false,
    playerUrl: String = "",
    playerProgress: Float = 0.0f,
    localLarge: Boolean = true,
    localStats: VideoStatsInfo = VideoStatsInfo(),
    remoteStats: VideoStatsInfo = VideoStatsInfo(),
    localRender: (view: Any, uid: Int, isFirstSetup: Boolean) -> Unit = { _, _, _ -> },
    remoteRender: (view: Any, uid: Int, isFirstSetup: Boolean) -> Unit = { _, _, _ -> },
    onSwitch: () -> Unit = {},
    onJoinClick: (String) -> Unit = {},
    onLeaveClick: () -> Unit = {},
    onOpenClick: (String) -> Unit = {},
    onPlayClick: () -> Unit = {},
    onStopClick: () -> Unit = {},
    onPauseClick: () -> Unit = {},
    onPushClick: () -> Unit = {},
    onProgressChange: (Float) -> Unit = {},
) {
    Column {
        TwoVideoView(
            modifier = Modifier.weight(1.0f),
            localUid = localUid,
            remoteUid = remoteUid,
            localStats = localStats,
            remoteStats = remoteStats,
            secondClickable = isJoined && remoteUid != 0,
            localPrimary = localLarge || remoteUid == 0,
            onSecondClick = onSwitch,
            localRender = localRender,
            remoteRender = remoteRender
        )

        Row(modifier = Modifier.padding(8.dp, 0.dp)) {
            Button(
                modifier = Modifier
                    .weight(1.0f)
                    .padding(8.dp),
                enabled = isJoined && playerReady,
                onClick = onPlayClick
            ) {
                Text(text = "播放")
            }
            Button(
                modifier = Modifier
                    .weight(1.0f)
                    .padding(8.dp),
                enabled = isJoined && playerReady,
                onClick = onStopClick
            ) {
                Text(text = "停止")
            }
            Button(
                modifier = Modifier
                    .weight(1.0f)
                    .padding(8.dp),
                enabled = isJoined && playerReady,
                onClick = onPauseClick
            ) {
                Text(text = "暂停")
            }
            Button(
                modifier = Modifier
                    .weight(1.0f)
                    .padding(8.dp),
                enabled = isJoined && playerReady,
                onClick = onPushClick
            ) {
                Text(text = "推送")
            }
        }

        SliderRaw(
            title = "Progress",
            value = playerProgress,
            enable = isJoined && playerReady,
            isDraggingWhileUpdating = true
        ) {
            onProgressChange(it)
        }

        InputRaw(
            text = playerUrl,
            btnText = "Open",
            label = "URL",
            enable = isJoined,
            onBtnClick = onOpenClick
        )

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
private fun MediaPlayerViewPreview() {
    MediaPlayerView(channelName = "", isJoined = false, onJoinClick = {}) {

    }
}