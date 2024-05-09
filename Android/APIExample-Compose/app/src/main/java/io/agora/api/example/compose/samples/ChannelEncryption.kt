package io.agora.api.example.compose.samples

import android.util.Base64
import android.widget.Toast
import androidx.activity.compose.rememberLauncherForActivityResult
import androidx.activity.result.contract.ActivityResultContracts
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.material3.Text
import androidx.compose.material3.TextField
import androidx.compose.material3.TextFieldDefaults
import androidx.compose.runtime.Composable
import androidx.compose.runtime.DisposableEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableIntStateOf
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.saveable.rememberSaveable
import androidx.compose.runtime.setValue
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.RectangleShape
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.platform.LocalLifecycleOwner
import androidx.compose.ui.platform.LocalSoftwareKeyboardController
import androidx.compose.ui.res.stringResource
import androidx.compose.ui.unit.dp
import io.agora.api.example.compose.BuildConfig
import io.agora.api.example.compose.R
import io.agora.api.example.compose.data.SettingPreferences
import io.agora.api.example.compose.ui.common.ChannelNameInput
import io.agora.api.example.compose.ui.common.DropdownMenuRaw
import io.agora.api.example.compose.ui.common.TwoVideoView
import io.agora.api.example.compose.ui.common.VideoStatsInfo
import io.agora.api.example.compose.utils.TokenUtils
import io.agora.rtc2.ChannelMediaOptions
import io.agora.rtc2.ClientRoleOptions
import io.agora.rtc2.Constants
import io.agora.rtc2.IRtcEngineEventHandler
import io.agora.rtc2.RtcEngine
import io.agora.rtc2.RtcEngineConfig
import io.agora.rtc2.internal.EncryptionConfig
import io.agora.rtc2.video.VideoCanvas
import io.agora.rtc2.video.VideoEncoderConfiguration

@Composable
fun ChannelEncryption() {
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
    var clientRole by remember { mutableIntStateOf(Constants.CLIENT_ROLE_BROADCASTER) }
    var encryptionKey by remember { mutableStateOf("") }
    var encryptionMode by remember { mutableStateOf(EncryptionConfig.EncryptionMode.AES_128_XTS) }

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

                val encryptionConfig = EncryptionConfig()
                encryptionConfig.encryptionMode = encryptionMode
                encryptionConfig.encryptionKey = encryptionKey
                System.arraycopy(
                    getKdfSaltFromServer(),
                    0,
                    encryptionConfig.encryptionKdfSalt,
                    0,
                    encryptionConfig.encryptionKdfSalt.size
                )
                rtcEngine.enableEncryption(true, encryptionConfig)

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

    ChannelEncryptionView(
        rtcEngine = rtcEngine,
        channelName = channelName,
        isJoined = isJoined,
        localUid = localUid,
        remoteUid = remoteUid,
        localLarge = localLarge,
        localStats = localStats,
        remoteStats = remoteStats,
        encryptionKey = encryptionKey,
        encryptionMode = encryptionMode,
        onEncryptionKeyChanged = {
            encryptionKey = it
        },
        onEncryptionModeChanged = {
            encryptionMode = it

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
            rtcEngine.stopPreview()
            rtcEngine.leaveChannel()
        }
    )
}

@Composable
private fun ChannelEncryptionView(
    rtcEngine: RtcEngine? = null,
    channelName: String,
    isJoined: Boolean,
    encryptionMode: EncryptionConfig.EncryptionMode,
    encryptionKey: String,
    localUid: Int = 0,
    remoteUid: Int = 0,
    localLarge: Boolean = true,
    localStats: VideoStatsInfo = VideoStatsInfo(),
    remoteStats: VideoStatsInfo = VideoStatsInfo(),
    onEncryptionModeChanged: (EncryptionConfig.EncryptionMode) -> Unit = {},
    onEncryptionKeyChanged: (String) -> Unit = {},
    onSwitch: () -> Unit = {},
    onJoinClick: (channelName: String) -> Unit,
    onLeaveClick: () -> Unit
) {
    Column {
        TwoVideoView(
            modifier = Modifier.weight(1f),
            localUid = localUid,
            remoteUid = remoteUid,
            localPrimary = localLarge,
            onSecondClick = onSwitch,
            localStats = localStats,
            remoteStats = remoteStats,
            localRender = { view, id, isFirstCreate ->
                val canvas = VideoCanvas(view, VideoCanvas.RENDER_MODE_HIDDEN, id)
                rtcEngine?.setupLocalVideo(canvas)
                if (isFirstCreate) {
                    rtcEngine?.startPreview()
                }
            },
            remoteRender = { view, id, _ ->
                val canvas = VideoCanvas(view, VideoCanvas.RENDER_MODE_HIDDEN, id)
                rtcEngine?.setupRemoteVideo(canvas)
            }
        )
        DropdownMenuRaw(
            title = stringResource(id = R.string.encryption_mode),
            options = listOf(
                "AES_128_XTS" to EncryptionConfig.EncryptionMode.AES_128_XTS,
                "AES_128_ECB" to EncryptionConfig.EncryptionMode.AES_128_ECB,
                "AES_256_XTS" to EncryptionConfig.EncryptionMode.AES_256_XTS,
                "SM4_128_ECB" to EncryptionConfig.EncryptionMode.SM4_128_ECB,
                "AES_128_GCM" to EncryptionConfig.EncryptionMode.AES_128_GCM,
                "AES_256_GCM" to EncryptionConfig.EncryptionMode.AES_256_GCM,
                "AES_128_GCM2" to EncryptionConfig.EncryptionMode.AES_128_GCM2,
                "AES_256_GCM2" to EncryptionConfig.EncryptionMode.AES_256_GCM2,
            ),
            enable = !isJoined,
            selectedValue = encryptionMode,
            onSelected = { _, option ->
                onEncryptionModeChanged(option.second)
            }
        )
        TextField(
            modifier = Modifier
                .fillMaxWidth()
                .padding(16.dp, 0.dp),
            value = encryptionKey,
            enabled = !isJoined,
            onValueChange = { onEncryptionKeyChanged(it) },
            label = { Text(stringResource(id = R.string.encryption_key)) },
            singleLine = true,
            shape = RectangleShape,
            colors = TextFieldDefaults.colors(
                focusedContainerColor = Color.Transparent,
                unfocusedContainerColor = Color.Transparent,
                errorContainerColor = Color.Transparent,
                disabledContainerColor = Color.Transparent
            )
        )

        ChannelNameInput(
            channelName = channelName,
            isJoined = isJoined,
            onJoinClick = onJoinClick,
            onLeaveClick = onLeaveClick
        )
    }
}

private fun getKdfSaltFromServer(): ByteArray {
    // Salt string should be the output of the following command:
    // openssl rand -base64 32
    val saltBase64String = "NiIeJ08AbtcQVjvV+oOEvF/4Dz5dy1CIwa805C8J2w0="
    return Base64.decode(saltBase64String, Base64.DEFAULT)
}