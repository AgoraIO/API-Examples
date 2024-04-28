package io.agora.api.example.compose.samples

import android.Manifest
import android.os.Build
import android.widget.Toast
import androidx.activity.compose.rememberLauncherForActivityResult
import androidx.activity.result.contract.ActivityResultContracts
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
import androidx.compose.ui.unit.dp
import io.agora.api.example.compose.BuildConfig
import io.agora.api.example.compose.ui.common.AudioGrid
import io.agora.api.example.compose.ui.common.AudioStatsInfo
import io.agora.api.example.compose.ui.common.ChannelNameInput
import io.agora.api.example.compose.ui.common.SwitchRaw
import io.agora.api.example.compose.utils.AudioFileReader
import io.agora.rtc2.ChannelMediaOptions
import io.agora.rtc2.Constants
import io.agora.rtc2.IRtcEngineEventHandler
import io.agora.rtc2.RtcEngine
import io.agora.rtc2.RtcEngineConfig
import io.agora.rtc2.audio.AudioTrackConfig

@Composable
fun CustomAudioSource() {
    val context = LocalContext.current
    val lifecycleOwner = LocalLifecycleOwner.current
    val keyboard = LocalSoftwareKeyboardController.current
    var customAudioTrack by rememberSaveable { mutableIntStateOf(-1) }
    var isJoined by rememberSaveable { mutableStateOf(false) }
    var channelName by rememberSaveable { mutableStateOf("") }
    var localUid by rememberSaveable { mutableIntStateOf(0) }
    var videoIdList by rememberSaveable { mutableStateOf(listOf<Int>()) }
    val statsMap = remember { mutableStateMapOf(0 to AudioStatsInfo()) }
    val option =  remember { ChannelMediaOptions() }
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
                    statsMap[uid] = AudioStatsInfo()
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
                    statsMap[uid] = AudioStatsInfo()
                }

                override fun onUserOffline(uid: Int, reason: Int) {
                    super.onUserOffline(uid, reason)
                    videoIdList = videoIdList - uid
                    statsMap.remove(uid)
                }

                override fun onLocalAudioStats(stats: LocalAudioStats?) {
                    super.onLocalAudioStats(stats)
                    statsMap[localUid]?.copy(localAudioStats = stats)?.let {
                        statsMap[localUid] = it
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
            enableAudio()
            val config = AudioTrackConfig()
            config.enableLocalPlayback = false
            customAudioTrack = createCustomAudioTrack(Constants.AudioTrackType.AUDIO_TRACK_MIXABLE, config)
        }
    }
    val audioFilerReader = remember {
        AudioFileReader(context) { buffer, timestamp ->
            if (isJoined && customAudioTrack != -1) {
                rtcEngine.pushExternalAudioFrame(
                    buffer, timestamp,
                    AudioFileReader.SAMPLE_RATE,
                    AudioFileReader.SAMPLE_NUM_OF_CHANNEL,
                    Constants.BytesPerSample.TWO_BYTES_PER_SAMPLE,
                    customAudioTrack
                )
            }
        }
    }

    DisposableEffect(lifecycleOwner) {
        onDispose {
            rtcEngine.destroyCustomAudioTrack(customAudioTrack)
            customAudioTrack = -1

            audioFilerReader.stop()

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
                option.channelProfile = Constants.CHANNEL_PROFILE_LIVE_BROADCASTING
                option.clientRoleType = Constants.CLIENT_ROLE_BROADCASTER
                rtcEngine.joinChannel("", channelName, 0, option)

            } else {
                // Permission is denied
                Toast.makeText(context, "Permission Denied", Toast.LENGTH_LONG).show()
            }
        }
    CustomAudioSourceView(
        videoIdList = videoIdList,
        statsMap = statsMap,
        channelName = channelName,
        isJoined = isJoined,
        onJoinClick = {
            keyboard?.hide()
            channelName = it
            permissionLauncher.launch(
                if (Build.VERSION.SDK_INT < Build.VERSION_CODES.S) {
                    arrayOf(
                        Manifest.permission.RECORD_AUDIO,
                    )
                } else {
                    arrayOf(
                        Manifest.permission.RECORD_AUDIO,
                        Manifest.permission.READ_PHONE_STATE,
                        Manifest.permission.BLUETOOTH_CONNECT,
                    )
                }
            )
        },
        onLeaveClick = {
            rtcEngine.leaveChannel()
        },
        onLocalAudioPublish = { enable ->
            if (enable) {
                audioFilerReader.start()
            } else {
                audioFilerReader.stop()
            }
            option.publishCustomAudioTrack = enable
            option.publishCustomAudioTrackId = customAudioTrack
            rtcEngine.updateChannelMediaOptions(option)
            rtcEngine.enableCustomAudioLocalPlayback(
                customAudioTrack,
                enable
            )
        },
        onMicrophonePublish = {
            option.publishMicrophoneTrack = it
            rtcEngine.updateChannelMediaOptions(option)
        }
    )
}


@Composable
fun CustomAudioSourceView(
    videoIdList: List<Int>,
    statsMap: Map<Int, AudioStatsInfo> = mapOf(),
    channelName: String = "",
    isJoined: Boolean = false,
    onLocalAudioPublish: (Boolean) -> Unit = {},
    onMicrophonePublish: (Boolean) -> Unit = {},
    onJoinClick: (String) -> Unit = {},
    onLeaveClick: () -> Unit = {}
) {
    Column(
        Modifier
            .fillMaxHeight()
            .fillMaxWidth()
    ) {
        AudioGrid(
            modifier = Modifier.height(380.dp),
            videoIdList = videoIdList,
            statsMap = statsMap
        )

        Spacer(modifier = Modifier.weight(1.0f))

        SwitchRaw(title = "发布本地音频", enable = isJoined, onCheckedChange = onLocalAudioPublish)
        SwitchRaw(title = "发布麦克风", enable = isJoined, onCheckedChange = onMicrophonePublish)

        ChannelNameInput(
            channelName = channelName,
            isJoined = isJoined,
            onJoinClick = onJoinClick,
            onLeaveClick = onLeaveClick
        )
    }
}


