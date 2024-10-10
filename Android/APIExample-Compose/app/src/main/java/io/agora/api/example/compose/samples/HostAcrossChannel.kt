package io.agora.api.example.compose.samples

import android.util.Log
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
import androidx.compose.ui.unit.dp
import io.agora.api.example.compose.BuildConfig
import io.agora.api.example.compose.R
import io.agora.api.example.compose.data.SettingPreferences
import io.agora.api.example.compose.ui.common.ChannelNameInput
import io.agora.api.example.compose.ui.common.InputRaw
import io.agora.api.example.compose.ui.common.TwoVideoView
import io.agora.api.example.compose.ui.common.TwoVideoViewType
import io.agora.api.example.compose.ui.common.VideoStatsInfo
import io.agora.api.example.compose.utils.TokenUtils
import io.agora.rtc2.ChannelMediaOptions
import io.agora.rtc2.Constants
import io.agora.rtc2.IRtcEngineEventHandler
import io.agora.rtc2.RtcEngine
import io.agora.rtc2.RtcEngineConfig
import io.agora.rtc2.video.ChannelMediaInfo
import io.agora.rtc2.video.ChannelMediaRelayConfiguration
import io.agora.rtc2.video.VideoCanvas
import io.agora.rtc2.video.VideoEncoderConfiguration

@Composable
fun HostAcrossChannel() {
    val context = LocalContext.current
    val lifecycleOwner = LocalLifecycleOwner.current
    val keyboard = LocalSoftwareKeyboardController.current
    var isJoined by rememberSaveable { mutableStateOf(false) }
    var isAcrossStarted by rememberSaveable { mutableStateOf(false) }
    var isAcrossRunning by rememberSaveable { mutableStateOf(false) }
    var channelName by rememberSaveable { mutableStateOf("") }
    var localUid by rememberSaveable { mutableIntStateOf(0) }
    var remoteUid by rememberSaveable { mutableIntStateOf(0) }
    var localStats by remember { mutableStateOf(VideoStatsInfo()) }
    var remoteStats by remember { mutableStateOf(VideoStatsInfo()) }

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

                override fun onChannelMediaRelayStateChanged(state: Int, code: Int) {
                    super.onChannelMediaRelayStateChanged(state, code)
                    Log.d("onChannelMediaRelayStateChanged", "state: $state, code: $code")
                    when(state){
                        Constants.RELAY_STATE_IDLE -> {
                            isAcrossStarted = false
                            isAcrossRunning = false
                        }
                        Constants.RELAY_STATE_CONNECTING -> {
                            isAcrossStarted = true
                            isAcrossRunning = false
                        }
                        Constants.RELAY_STATE_RUNNING -> {
                            isAcrossStarted = true
                            isAcrossRunning = true
                        }
                        Constants.RELAY_STATE_FAILURE -> {
                            isAcrossStarted = false
                            isAcrossRunning = false
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
            rtcEngine.stopChannelMediaRelay()
            rtcEngine.stopPreview()
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
                mediaOptions.clientRoleType = Constants.CLIENT_ROLE_BROADCASTER
                mediaOptions.autoSubscribeVideo = true
                mediaOptions.autoSubscribeAudio = true
                mediaOptions.publishMicrophoneTrack = true
                mediaOptions.publishCameraTrack = true
                TokenUtils.gen(channelName, 0){
                    rtcEngine.joinChannel(it, channelName, 0, mediaOptions)
                }
            } else {
                // Permission is denied
                Toast.makeText(context, "Permission Denied", Toast.LENGTH_LONG).show()
            }
        }

    HostAcrossChannelView(
        rtcEngine = rtcEngine,
        channelName = channelName,
        isJoined = isJoined,
        localUid = localUid,
        remoteUid = remoteUid,
        localStats = localStats,
        remoteStats = remoteStats,
        isAcrossStarted = isAcrossStarted,
        isAcrossRunning = isAcrossRunning,
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
            rtcEngine.stopChannelMediaRelay()
            rtcEngine.stopPreview()
            rtcEngine.leaveChannel()
        },
        onAcrossClick = { destChannelName, hasStart ->
            if (hasStart) {
                rtcEngine.stopChannelMediaRelay()
            } else {
                val srcChannelInfo = ChannelMediaInfo(channelName, null, localUid)
                val config = ChannelMediaRelayConfiguration()
                config.setSrcChannelInfo(srcChannelInfo)
                val destChannelInfo = ChannelMediaInfo(destChannelName, null, localUid)
                config.setDestChannelInfo(destChannelName, destChannelInfo)
                rtcEngine.startOrUpdateChannelMediaRelay(config)
            }
        },
        onAcrossRunningClick = {
            if (it) {
                rtcEngine.pauseAllChannelMediaRelay()
            } else {
                rtcEngine.resumeAllChannelMediaRelay()
            }
            isAcrossRunning = !it
        }
    )
}

@Composable
private fun HostAcrossChannelView(
    rtcEngine: RtcEngine?,
    channelName: String,
    isJoined: Boolean,
    isAcrossStarted: Boolean,
    isAcrossRunning: Boolean,
    localUid: Int = 0,
    remoteUid: Int = 0,
    localStats: VideoStatsInfo = VideoStatsInfo(),
    remoteStats: VideoStatsInfo = VideoStatsInfo(),
    onJoinClick: (String) -> Unit,
    onLeaveClick: () -> Unit,
    onAcrossClick: (channelName: String, hasStart: Boolean) -> Unit,
    onAcrossRunningClick: (isRunning: Boolean) -> Unit
) {
    Column {
        TwoVideoView(
            modifier = Modifier.height(350.dp),
            type = TwoVideoViewType.Row,
            localUid = localUid,
            remoteUid = remoteUid,
            localStats = localStats,
            remoteStats = remoteStats,
            localRender = { view, id, _ ->
                rtcEngine?.setupLocalVideo(VideoCanvas(view, VideoCanvas.RENDER_MODE_HIDDEN, id))
                rtcEngine?.startPreview()
            },
            remoteRender = { view, id, _ ->
                rtcEngine?.setupRemoteVideo(VideoCanvas(view, VideoCanvas.RENDER_MODE_HIDDEN, id))
            }
        )

        Spacer(modifier = Modifier.weight(1.0f))

        InputRaw(
            text = "",
            label = "Across Channel Name",
            editable = !isAcrossStarted,
            btnText = if (isAcrossStarted) stringResource(id = R.string.exit) else stringResource(id = R.string.join),
            enable = isJoined,
            onBtnClick = {
                onAcrossClick(it, isAcrossStarted)
            },
            secondVisible = true,
            secondBtnText = if (isAcrossRunning) stringResource(id = R.string.pause) else stringResource(
                id = R.string.resume
            ),
            secondEnable = isAcrossStarted,
            onSecondBtnClick = {
                onAcrossRunningClick(isAcrossRunning)
            }
        )
        ChannelNameInput(
            channelName = channelName,
            isJoined = isJoined,
            onJoinClick = onJoinClick,
            onLeaveClick = onLeaveClick
        )
    }

}