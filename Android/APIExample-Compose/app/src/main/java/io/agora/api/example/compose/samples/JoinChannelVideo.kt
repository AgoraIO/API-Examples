package io.agora.api.example.compose.samples

import android.Manifest
import android.view.View
import android.widget.Toast
import androidx.activity.compose.rememberLauncherForActivityResult
import androidx.activity.result.contract.ActivityResultContracts
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxHeight
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.runtime.Composable
import androidx.compose.runtime.DisposableEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableIntStateOf
import androidx.compose.runtime.mutableStateMapOf
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
import io.agora.api.example.compose.ui.common.VideoGrid
import io.agora.api.example.compose.ui.common.VideoStatsInfo
import io.agora.rtc2.ChannelMediaOptions
import io.agora.rtc2.Constants
import io.agora.rtc2.IRtcEngineEventHandler
import io.agora.rtc2.RtcEngine
import io.agora.rtc2.RtcEngineConfig
import io.agora.rtc2.video.VideoCanvas
import io.agora.rtc2.video.VideoEncoderConfiguration

const val TAG = "JoinVideoChannel"

@Composable
fun JoinChannelVideo() {
    val context = LocalContext.current
    val lifecycleOwner = LocalLifecycleOwner.current
    val keyboard = LocalSoftwareKeyboardController.current
    var isJoined by rememberSaveable { mutableStateOf(false) }
    var channelName by rememberSaveable { mutableStateOf("") }
    var localUid by rememberSaveable { mutableIntStateOf(0) }
    var videoIdList by rememberSaveable { mutableStateOf(listOf<Int>()) }
    val statsMap  = remember { mutableStateMapOf(0 to VideoStatsInfo()) }

    val rtcEngine = remember {
        RtcEngine.create(RtcEngineConfig().apply {
            mContext = context
            mAppId = BuildConfig.AGORA_APP_ID
            mEventHandler = object : IRtcEngineEventHandler() {
                override fun onJoinChannelSuccess(channel: String?, uid: Int, elapsed: Int) {
                    super.onJoinChannelSuccess(channel, uid, elapsed)
                    isJoined = true
                    localUid = uid
                    videoIdList = videoIdList + uid
                    statsMap[uid] = VideoStatsInfo()
                }

                override fun onLeaveChannel(stats: RtcStats?) {
                    super.onLeaveChannel(stats)
                    isJoined = false
                    videoIdList = emptyList()
                    statsMap.clear()
                }

                override fun onUserJoined(uid: Int, elapsed: Int) {
                    super.onUserJoined(uid, elapsed)
                    videoIdList = videoIdList + uid
                    statsMap[uid] = VideoStatsInfo()
                }

                override fun onUserOffline(uid: Int, reason: Int) {
                    super.onUserOffline(uid, reason)
                    videoIdList = videoIdList - uid
                    statsMap.remove(uid)
                }

                override fun onRtcStats(stats: RtcStats?) {
                    super.onRtcStats(stats)
                    statsMap[localUid]?.copy(rtcStats = stats)?.let {
                        statsMap[localUid] = it
                    }
                }

                override fun onLocalVideoStats(
                    source: Constants.VideoSourceType?,
                    stats: LocalVideoStats?
                ) {
                    super.onLocalVideoStats(source, stats)
                    statsMap[localUid]?.copy(localVideoStats = stats)?.let {
                        statsMap[localUid] = it
                    }
                }

                override fun onLocalAudioStats(stats: LocalAudioStats?) {
                    super.onLocalAudioStats(stats)
                    statsMap[localUid]?.copy(localAudioStats = stats)?.let {
                        statsMap[localUid] = it
                    }
                }

                override fun onRemoteVideoStats(stats: RemoteVideoStats?) {
                    super.onRemoteVideoStats(stats)
                    val uid = stats?.uid ?: return
                    statsMap[uid]?.copy(remoteVideoStats = stats)?.let {
                        statsMap[uid] = it
                    }
                }

                override fun onRemoteAudioStats(stats: RemoteAudioStats?) {
                    super.onRemoteAudioStats(stats)
                    val uid = stats?.uid ?: return
                    statsMap[uid]?.copy(remoteAudioStats = stats)?.let {
                        statsMap[uid] = it
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
            if (isJoined) {
                rtcEngine.leaveChannel()
            }
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
                rtcEngine.joinChannel("", channelName, 0, mediaOptions)

            } else {
                // Permission is denied
                Toast.makeText(context, "Permission Denied", Toast.LENGTH_LONG).show()
            }
        }

    JoinChannelVideoView(
        channelName = channelName,
        isJoined = isJoined,
        onJoinClick = {
            channelName = it
            keyboard?.hide()
            permissionLauncher.launch(
                arrayOf(
                    Manifest.permission.RECORD_AUDIO, Manifest.permission.CAMERA
                )
            )
        },
        onLeaveClick = {
            rtcEngine.leaveChannel()
        },
        videoIdList = videoIdList,
        statsMap = statsMap.toMap(),
        setupVideo = { view, id, _ ->
            val canvas = VideoCanvas(view, Constants.RENDER_MODE_HIDDEN, id)
            if (id == localUid) {
                rtcEngine.setupLocalVideo(canvas)
            } else {
                rtcEngine.setupRemoteVideo(canvas)
            }
        }
    )
}

@Preview
@Composable
fun JoinChannelVideoPreview() {
    JoinChannelVideoView(
        channelName = "Channel Name",
        isJoined = false,
        onJoinClick = {},
        onLeaveClick = {},
        videoIdList = listOf(0, 1, 2, 3),
        setupVideo = { _, _, _ -> }
    )
}

@Composable
fun JoinChannelVideoView(
    channelName: String,
    isJoined: Boolean,
    onJoinClick: (String) -> Unit,
    onLeaveClick: () -> Unit,
    videoIdList: List<Int>,
    setupVideo: (View, Int, Boolean) -> Unit,
    statsMap : Map<Int, VideoStatsInfo> = emptyMap()
) {
    Box {
        Column(
            Modifier
                .fillMaxHeight()
                .fillMaxWidth()
        ) {
            VideoGrid(
                modifier = Modifier.height(600.dp),
                videoIdList = videoIdList,
                setupVideo = setupVideo,
                statsMap = statsMap
            )
            Spacer(modifier = Modifier.weight(1.0f))
            ChannelNameInput(
                channelName = channelName,
                isJoined = isJoined,
                onJoinClick = onJoinClick,
                onLeaveClick = onLeaveClick
            )
        }
    }
}

