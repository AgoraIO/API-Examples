package io.agora.api.example.compose.samples

import android.os.Handler
import android.os.Looper
import android.widget.Toast
import androidx.activity.compose.rememberLauncherForActivityResult
import androidx.activity.result.contract.ActivityResultContracts
import androidx.compose.foundation.layout.Column
import androidx.compose.material3.AlertDialog
import androidx.compose.material3.Text
import androidx.compose.material3.TextButton
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
import io.agora.api.example.compose.BuildConfig
import io.agora.api.example.compose.R
import io.agora.api.example.compose.data.SettingPreferences
import io.agora.api.example.compose.ui.common.ChannelNameInput
import io.agora.api.example.compose.ui.common.InputRaw
import io.agora.api.example.compose.ui.common.TwoVideoView
import io.agora.api.example.compose.ui.common.VideoStatsInfo
import io.agora.api.example.compose.utils.TokenUtils
import io.agora.rtc2.ChannelMediaOptions
import io.agora.rtc2.ClientRoleOptions
import io.agora.rtc2.Constants
import io.agora.rtc2.IMetadataObserver
import io.agora.rtc2.IRtcEngineEventHandler
import io.agora.rtc2.RtcEngine
import io.agora.rtc2.RtcEngineConfig
import io.agora.rtc2.video.AgoraMetadata
import io.agora.rtc2.video.VideoCanvas
import io.agora.rtc2.video.VideoEncoderConfiguration

@Composable
fun MediaMetadata() {
    val handler = remember { Handler(Looper.getMainLooper()) }
    val context = LocalContext.current
    val lifecycleOwner = LocalLifecycleOwner.current
    val keyboard = LocalSoftwareKeyboardController.current
    var sendMetadata by rememberSaveable { mutableStateOf("") }
    var isJoined by rememberSaveable { mutableStateOf(false) }
    var localLarge by rememberSaveable { mutableStateOf(true) }
    var channelName by rememberSaveable { mutableStateOf("") }
    var localUid by rememberSaveable { mutableIntStateOf(0) }
    var remoteUid by rememberSaveable { mutableIntStateOf(0) }
    var localStats by remember { mutableStateOf(VideoStatsInfo()) }
    var remoteStats by remember { mutableStateOf(VideoStatsInfo()) }
    var clientRole by remember { mutableIntStateOf(Constants.CLIENT_ROLE_BROADCASTER) }
    var receivedMetadata by rememberSaveable { mutableStateOf("") }

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

                override fun onClientRoleChanged(
                    oldRole: Int,
                    newRole: Int,
                    newRoleOptions: ClientRoleOptions?
                ) {
                    super.onClientRoleChanged(oldRole, newRole, newRoleOptions)
                    clientRole = newRole
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
            registerMediaMetadataObserver(object : IMetadataObserver {
                override fun getMaxMetadataSize() = 1024

                override fun onReadyToSendMetadata(timeStampMs: Long, sourceType: Int): ByteArray {
                    val data = sendMetadata.toByteArray()
                    if (sendMetadata.isNotEmpty()) {
                        sendMetadata = ""
                        handler.post {
                            Toast.makeText(context, "Send Metadata Success", Toast.LENGTH_LONG)
                                .show()
                        }
                    }
                    return data
                }

                override fun onMetadataReceived(metadata: AgoraMetadata) {
                    receivedMetadata = metadata.data?.toString(Charsets.UTF_8) ?: ""
                }

            }, IMetadataObserver.VIDEO_METADATA)
        }
    }
    DisposableEffect(lifecycleOwner) {
        onDispose {
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
                mediaOptions.clientRoleType = clientRole
                TokenUtils.gen(channelName, 0) {
                    rtcEngine.joinChannel(it, channelName, 0, mediaOptions)
                }
            } else {
                // Permission is denied
                Toast.makeText(context, "Permission Denied", Toast.LENGTH_LONG).show()
            }
        }

    MediaMetadataView(
        rtcEngine = rtcEngine,
        channelName = channelName,
        isJoined = isJoined,
        localUid = localUid,
        remoteUid = remoteUid,
        localStats = localStats,
        remoteStats = remoteStats,
        localLarge = localLarge,
        onSwitch = {
            localLarge = !localLarge
        },
        onVideoMetadataSend = {
            sendMetadata = it
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
            rtcEngine.stopPreview()

            rtcEngine.leaveChannel()
        }
    )

    if (receivedMetadata.isNotEmpty()) {
        AlertDialog(
            title = {
                Text("Received Metadata")
            },
            text = {
                Text(receivedMetadata)
            },
            onDismissRequest = {
                receivedMetadata = ""
            }, confirmButton = {
                TextButton(
                    onClick = {
                        receivedMetadata = ""
                    }
                ) {
                    Text("Confirm")
                }
            })
    }
}


@Composable
private fun MediaMetadataView(
    rtcEngine: RtcEngine? = null,
    channelName: String,
    isJoined: Boolean,
    localUid: Int = 0,
    remoteUid: Int = 0,
    localLarge: Boolean = true,
    localStats: VideoStatsInfo = VideoStatsInfo(),
    remoteStats: VideoStatsInfo = VideoStatsInfo(),
    onVideoMetadataSend: (String) -> Unit = { _ -> },
    onSwitch: () -> Unit = {},
    onJoinClick: (String) -> Unit,
    onLeaveClick: () -> Unit
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
            localRender = { view, uid, isFirstSetup ->
                rtcEngine?.setupLocalVideo(
                    VideoCanvas(
                        view,
                        Constants.RENDER_MODE_HIDDEN,
                        uid
                    )
                )
                if (isFirstSetup) {
                    rtcEngine?.startPreview()
                }

            },
            remoteRender = { view, uid, _ ->
                rtcEngine?.setupRemoteVideo(
                    VideoCanvas(
                        view,
                        Constants.RENDER_MODE_HIDDEN,
                        uid
                    )
                )
            }
        )

        InputRaw(
            text = "",
            label = stringResource(id = R.string.metadata_video),
            btnText = stringResource(id = R.string.send),
            enable = isJoined
        ) {
            onVideoMetadataSend(it)
        }

        ChannelNameInput(
            channelName = channelName,
            isJoined = isJoined,
            onJoinClick = onJoinClick,
            onLeaveClick = onLeaveClick
        )
    }
}