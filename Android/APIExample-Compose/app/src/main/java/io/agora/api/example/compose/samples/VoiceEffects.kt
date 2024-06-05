package io.agora.api.example.compose.samples

import android.widget.Toast
import androidx.activity.compose.rememberLauncherForActivityResult
import androidx.activity.result.contract.ActivityResultContracts
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.fillMaxHeight
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.rememberLazyListState
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
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
import androidx.compose.ui.res.stringResource
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import io.agora.api.example.compose.BuildConfig
import io.agora.api.example.compose.R
import io.agora.api.example.compose.data.SettingPreferences
import io.agora.api.example.compose.ui.common.AudioGrid
import io.agora.api.example.compose.ui.common.AudioStatsInfo
import io.agora.api.example.compose.ui.common.ChannelNameInput
import io.agora.api.example.compose.ui.common.DropdownMenuRaw
import io.agora.api.example.compose.utils.TokenUtils
import io.agora.rtc2.ChannelMediaOptions
import io.agora.rtc2.Constants
import io.agora.rtc2.IRtcEngineEventHandler
import io.agora.rtc2.RtcEngine
import io.agora.rtc2.RtcEngineConfig

@Composable
fun VoiceEffects() {
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
            mAreaCode = SettingPreferences.getArea()
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
                TokenUtils.gen(channelName, 0) {
                    rtcEngine.joinChannel(it, channelName, 0, mediaOptions)
                }
            } else {
                // Permission is denied
                Toast.makeText(context, "Permission Denied", Toast.LENGTH_LONG).show()
            }
        }

    VoiceEffectsView(
        videoIdList = videoIdList,
        statsMap = statsMap,
        rtcEngine = rtcEngine,
        channelName = channelName,
        isJoined = isJoined,
        onJoinClick = {
            keyboard?.hide()
            channelName = it
            permissionLauncher.launch(
                arrayOf(
                    android.Manifest.permission.RECORD_AUDIO,
                )
            )
        },
        onLeaveClick = {
            rtcEngine.leaveChannel()
        }
    )
}

@Composable
private fun VoiceEffectsView(
    videoIdList: List<Int>,
    statsMap: Map<Int, AudioStatsInfo> = mapOf(),
    rtcEngine: RtcEngine? = null,
    channelName: String = "",
    isJoined: Boolean = false,
    onJoinClick: (String) -> Unit = {},
    onLeaveClick: () -> Unit = {}
) {

    var audioScenario by rememberSaveable { mutableIntStateOf(Constants.AUDIO_SCENARIO_DEFAULT) }
    val lazyListState = rememberLazyListState(Int.MAX_VALUE)

    Column {
        AudioGrid(
            modifier = Modifier
                .fillMaxWidth()
                .fillMaxHeight(0.3f),
            videoIdList = videoIdList,
            statsMap = statsMap,
            raw = 1,
            column = 2
        )
        LazyColumn(
            modifier = Modifier.weight(1.0f),
            verticalArrangement = Arrangement.Bottom,
            state = lazyListState
        ) {
            item {
                Column {
                    Text(
                        text = "Voice Beautifier / Effects Preset",
                        style = MaterialTheme.typography.titleSmall,
                        modifier = Modifier.padding(16.dp, 8.dp)
                    )
                    DropdownMenuRaw(
                        title = "Chat Beautifier",
                        options = listOf(
                            "Off" to Constants.VOICE_BEAUTIFIER_OFF,
                            "Magnetic" to Constants.CHAT_BEAUTIFIER_MAGNETIC,
                            "Fresh" to Constants.CHAT_BEAUTIFIER_FRESH,
                            "Vitality" to Constants.CHAT_BEAUTIFIER_VITALITY,
                        )
                    ) { _, option ->
                        rtcEngine?.setVoiceBeautifierPreset(option.second)
                    }
                    DropdownMenuRaw(
                        title = "Timbre Transformation",
                        options = listOf(
                            "Off" to Constants.VOICE_BEAUTIFIER_OFF,
                            "Vigorous" to Constants.TIMBRE_TRANSFORMATION_VIGOROUS,
                            "Deep" to Constants.TIMBRE_TRANSFORMATION_DEEP,
                            "Mellow" to Constants.TIMBRE_TRANSFORMATION_MELLOW,
                            "Falsetto" to Constants.TIMBRE_TRANSFORMATION_FALSETTO,
                            "Full" to Constants.TIMBRE_TRANSFORMATION_FULL,
                            "Clear" to Constants.TIMBRE_TRANSFORMATION_CLEAR,
                            "Resounding" to Constants.TIMBRE_TRANSFORMATION_RESOUNDING,
                            "Ringing" to Constants.TIMBRE_TRANSFORMATION_RINGING,
                        )
                    ) { _, option ->
                        rtcEngine?.setVoiceBeautifierPreset(option.second);
                    }

                    DropdownMenuRaw(
                        title = "Voice Changer",
                        options = listOf(
                            "Off" to Constants.VOICE_CONVERSION_OFF,
                            "Neutral" to Constants.VOICE_CHANGER_NEUTRAL,
                            "Sweet" to Constants.VOICE_CHANGER_SWEET,
                            "Solid" to Constants.VOICE_CHANGER_SOLID,
                            "Bass" to Constants.VOICE_CHANGER_BASS,
                            "Cartoon" to Constants.VOICE_CHANGER_CARTOON,
                            "Childlike" to Constants.VOICE_CHANGER_CHILDLIKE,
                            "Phone Operator" to Constants.VOICE_CHANGER_PHONE_OPERATOR,
                            "Monster" to Constants.VOICE_CHANGER_MONSTER,
                            "Transformers" to Constants.VOICE_CHANGER_TRANSFORMERS,
                            "GRoot" to Constants.VOICE_CHANGER_GROOT,
                            "Darth Vader" to Constants.VOICE_CHANGER_DARTH_VADER,
                            "Iron Lady" to Constants.VOICE_CHANGER_IRON_LADY,
                            "Shin Chan" to Constants.VOICE_CHANGER_SHIN_CHAN,
                            "Girlish Man" to Constants.VOICE_CHANGER_GIRLISH_MAN,
                            "Chipmunk" to Constants.VOICE_CHANGER_CHIPMUNK,
                        )
                    ) { _, option ->
                        rtcEngine?.setVoiceConversionPreset(option.second);
                    }

                    DropdownMenuRaw(
                        title = "Style Transformation",
                        options = listOf(
                            "Off" to Constants.AUDIO_EFFECT_OFF,
                            "Popular" to Constants.STYLE_TRANSFORMATION_POPULAR,
                            "RNB" to Constants.STYLE_TRANSFORMATION_RNB,
                        )
                    ) { _, option ->
                        rtcEngine?.setAudioEffectPreset(option.second);
                    }

                    DropdownMenuRaw(
                        title = "Room Acoustics",
                        options = listOf(
                            "Off" to Constants.AUDIO_EFFECT_OFF,
                            "KTV" to Constants.ROOM_ACOUSTICS_KTV,
                            "Vocal Concert" to Constants.ROOM_ACOUSTICS_VOCAL_CONCERT,
                            "Studio" to Constants.ROOM_ACOUSTICS_STUDIO,
                            "Phonograph" to Constants.ROOM_ACOUSTICS_PHONOGRAPH,
                            "Virtual Stereo" to Constants.ROOM_ACOUSTICS_VIRTUAL_STEREO,
                            "Spacial" to Constants.ROOM_ACOUSTICS_SPACIAL,
                            "Ethereal" to Constants.ROOM_ACOUSTICS_ETHEREAL,
                            "3D Voice" to Constants.ROOM_ACOUSTICS_3D_VOICE,
                            "Virtual Surround Sound" to Constants.ROOM_ACOUSTICS_VIRTUAL_SURROUND_SOUND,
                        )
                    ) { _, option ->
                        rtcEngine?.setAudioEffectPreset(option.second);
                    }

                    var showPitchOption by remember { mutableStateOf(false) }
                    var pitchTonicMode by remember { mutableStateOf(1) }
                    var pitchTonicValue by remember { mutableStateOf(1) }
                    DropdownMenuRaw(
                        title = "Pitch Correction",
                        options = listOf(
                            "Off" to Constants.AUDIO_EFFECT_OFF,
                            "Pitch Correction" to Constants.PITCH_CORRECTION,
                        )
                    ) { _, option ->
                        rtcEngine?.setAudioEffectPreset(option.second)
                        showPitchOption = option.second == Constants.PITCH_CORRECTION
                    }
                    if (showPitchOption) {
                        DropdownMenuRaw(
                            title = "  Tonic Mode",
                            options = listOf(
                                "Nature Major" to 1,
                                "Nature Minor" to 2,
                                "Breeze Minor" to 3,
                            )
                        ) { _, option ->
                            pitchTonicMode = option.second
                            rtcEngine?.setAudioEffectParameters(
                                Constants.PITCH_CORRECTION,
                                pitchTonicMode,
                                pitchTonicValue
                            )
                        }
                        DropdownMenuRaw(
                            title = "  Tonic Value",
                            options = listOf(
                                "A Pitch" to 1,
                                "A# Pitch" to 2,
                                "B Pitch" to 3,
                                "C Pitch" to 4,
                                "C# Pitch" to 5,
                                "D Pitch" to 6,
                                "D# Pitch" to 7,
                                "E Pitch" to 8,
                                "F Pitch" to 9,
                                "F# Pitch" to 10,
                                "G Pitch" to 11,
                                "G# Pitch" to 12,
                            )
                        ) { _, option ->
                            pitchTonicValue = option.second
                            rtcEngine?.setAudioEffectParameters(
                                Constants.PITCH_CORRECTION,
                                pitchTonicMode,
                                pitchTonicValue
                            )
                        }
                    }

                    DropdownMenuRaw(
                        title = "AINS Mode",
                        options = listOf(
                            "Off" to 0,
                            stringResource(id = R.string.ains_mode_0) to 0,
                            stringResource(id = R.string.ains_mode_1) to 1,
                            stringResource(id = R.string.ains_mode_2) to 2,
                        )
                    ) { index, option ->
                        rtcEngine?.setAINSMode(index != 0, option.second)
                        showPitchOption = option.second == Constants.PITCH_CORRECTION
                    }
                }
            }
        }

        DropdownMenuRaw(
            title = "Profile",
            options = listOf(
                "Default" to Constants.AUDIO_PROFILE_DEFAULT,
                "Speech Standard" to Constants.AUDIO_PROFILE_SPEECH_STANDARD,
                "Music Standard" to Constants.AUDIO_PROFILE_MUSIC_STANDARD,
                "Music Standard Stereo" to Constants.AUDIO_PROFILE_MUSIC_STANDARD_STEREO,
                "Music High Quality" to Constants.AUDIO_PROFILE_MUSIC_HIGH_QUALITY,
                "Music High Quality Stereo" to Constants.AUDIO_PROFILE_MUSIC_HIGH_QUALITY_STEREO,
            ),
            enable = !isJoined,
            onSelected = { _, option ->
                rtcEngine?.setAudioProfile(option.second, audioScenario)
            }
        )

        DropdownMenuRaw(
            title = "Scenario",
            options = listOf(
                "Default" to Constants.AUDIO_SCENARIO_DEFAULT,
                "Game Streaming" to Constants.AUDIO_SCENARIO_GAME_STREAMING,
                "Chat Room" to Constants.AUDIO_SCENARIO_CHATROOM,
                "Chorus" to Constants.AUDIO_SCENARIO_CHORUS,
                "Meeting" to Constants.AUDIO_SCENARIO_MEETING,
            ),
            onSelected = { _, option ->
                rtcEngine?.setAudioScenario(option.second)
                audioScenario = option.second
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

@Preview
@Composable
private fun VoiceEffectsViewPreview() {
    VoiceEffectsView(videoIdList = listOf(0, 1, 2))
}