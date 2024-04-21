package io.agora.api.example.compose.samples

import android.Manifest
import android.os.Build
import android.widget.Toast
import androidx.activity.compose.rememberLauncherForActivityResult
import androidx.activity.result.contract.ActivityResultContracts
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.fillMaxHeight
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.rememberLazyListState
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
import io.agora.api.example.compose.ui.common.AudioGrid
import io.agora.api.example.compose.ui.common.AudioStatsInfo
import io.agora.api.example.compose.ui.common.ChannelNameInput
import io.agora.api.example.compose.ui.common.DropdownMenuRaw
import io.agora.api.example.compose.ui.common.SliderRaw
import io.agora.api.example.compose.ui.common.SwitchRaw
import io.agora.rtc2.ChannelMediaOptions
import io.agora.rtc2.Constants
import io.agora.rtc2.IRtcEngineEventHandler
import io.agora.rtc2.RtcEngine
import io.agora.rtc2.RtcEngineConfig

@Composable
fun JoinChannelAudio() {
    val context = LocalContext.current
    val lifecycleOwner = LocalLifecycleOwner.current
    val keyboard = LocalSoftwareKeyboardController.current
    var isJoined by rememberSaveable { mutableStateOf(false) }
    var channelName by rememberSaveable { mutableStateOf("") }
    var localUid by rememberSaveable { mutableIntStateOf(0) }
    var videoIdList by rememberSaveable { mutableStateOf(listOf<Int>()) }
    val statsMap = remember { mutableStateMapOf(0 to AudioStatsInfo()) }

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
                    videoIdList = videoIdList - localUid
                    statsMap.remove(localUid)
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

    JoinChannelAudioView(
        rtcEngine = rtcEngine,
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
        }
    )
}

@Composable
fun JoinChannelAudioView(
    videoIdList: List<Int>,
    statsMap: Map<Int, AudioStatsInfo> = mapOf(),
    rtcEngine: RtcEngine? = null,
    channelName: String = "",
    isJoined: Boolean = false,
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
        val lazyListState = rememberLazyListState(Int.MAX_VALUE)

        LazyColumn(
            modifier = Modifier.weight(1.0f),
            verticalArrangement = Arrangement.Bottom,
            state = lazyListState
        ) {
            item {
                DropdownMenuRaw(
                    title = "Audio Route",
                    options = listOf(
                        "Speaker" to Constants.AUDIO_ROUTE_SPEAKERPHONE,
                        "Headset" to Constants.AUDIO_ROUTE_HEADSET,
                        "Earphone" to Constants.AUDIO_ROUTE_EARPIECE,
                        "Bluetooth" to Constants.AUDIO_ROUTE_BLUETOOTH_DEVICE_HFP
                    ),
                    selected = 0
                ) { option ->
                    rtcEngine?.setRouteInCommunicationMode(option.second)
                }
            }
            item {
                Column {
                    var inEarOpen by remember { mutableStateOf(false) }
                    SwitchRaw(title = "InEar Monitor", checked = inEarOpen) {
                        inEarOpen = it
                        rtcEngine?.enableInEarMonitoring(it)
                    }
                    if (inEarOpen) {
                        SliderRaw(title = "InEar Monitor Vol") {
                            rtcEngine?.setInEarMonitoringVolume((it * 100).toInt())
                        }
                    }
                }
            }
            item {
                SliderRaw(title = "Playout Vol"){
                    rtcEngine?.adjustPlaybackSignalVolume((it * 100).toInt())
                }
            }
            item {
                SliderRaw(title = "Recording Vol"){
                    rtcEngine?.adjustRecordingSignalVolume((it * 100).toInt())
                }
            }
            item {
                DropdownMenuRaw(
                    title = "Scenario",
                    options = listOf(
                        "Default" to Constants.AUDIO_SCENARIO_DEFAULT,
                        "Game Streaming" to Constants.AUDIO_SCENARIO_GAME_STREAMING,
                        "Chatroom" to Constants.AUDIO_SCENARIO_CHATROOM,
                        "Chorus" to Constants.AUDIO_SCENARIO_CHORUS,
                        "Meeting" to Constants.AUDIO_SCENARIO_MEETING,
                    ),
                    selected = 0
                ) {
                    rtcEngine?.setAudioScenario(it.second)
                }
            }
            item {
                DropdownMenuRaw(
                    title = "Audio Profile",
                    options = listOf(
                        "Default" to Constants.AUDIO_PROFILE_DEFAULT,
                        "Speech Standard" to Constants.AUDIO_PROFILE_SPEECH_STANDARD,
                        "Music Standard" to Constants.AUDIO_PROFILE_MUSIC_STANDARD,
                        "Music Standard Stereo" to Constants.AUDIO_PROFILE_MUSIC_STANDARD_STEREO,
                        "Music High Quality" to Constants.AUDIO_PROFILE_MUSIC_HIGH_QUALITY,
                        "Music High Quality Stereo" to Constants.AUDIO_PROFILE_MUSIC_HIGH_QUALITY_STEREO,
                    ),
                    selected = 0
                ) {
                    rtcEngine?.setAudioScenario(it.second)
                }
            }
            item {
                DropdownMenuRaw(
                    title = "Channel Profile",
                    options = listOf(
                        "Communication" to Constants.CHANNEL_PROFILE_COMMUNICATION,
                        "Live Broadcasting" to Constants.CHANNEL_PROFILE_LIVE_BROADCASTING,
                        "Game" to Constants.CHANNEL_PROFILE_GAME,
                        "Cloud Gaming" to Constants.CHANNEL_PROFILE_CLOUD_GAMING,
                        "Communication 1v1" to Constants.CHANNEL_PROFILE_COMMUNICATION_1v1,
                        "Communication 1v1" to Constants.CHANNEL_PROFILE_COMMUNICATION_1v1,
                    )
                ) {
                    rtcEngine?.setChannelProfile(it.second)
                }
            }
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
fun JoinChannelAudioViewPreview() {
    JoinChannelAudioView(
        listOf(0, 1, 2, 3),
        mapOf(
            0 to AudioStatsInfo(
                localAudioStats = IRtcEngineEventHandler.LocalAudioStats().apply {
                    numChannels = 2
                    sentSampleRate = 48000
                    sentBitrate = 128
                },
            ),
            1 to AudioStatsInfo(),
            2 to AudioStatsInfo(),
            3 to AudioStatsInfo()
        )
    )
}
