package io.agora.api.example.compose.samples

import android.widget.Toast
import androidx.activity.compose.rememberLauncherForActivityResult
import androidx.activity.result.contract.ActivityResultContracts
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.lazy.LazyColumn
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
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.platform.LocalLifecycleOwner
import androidx.compose.ui.platform.LocalSoftwareKeyboardController
import androidx.compose.ui.res.stringResource
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import io.agora.api.example.compose.BuildConfig
import io.agora.api.example.compose.R
import io.agora.api.example.compose.data.SettingPreferences
import io.agora.api.example.compose.ui.common.AudioGrid
import io.agora.api.example.compose.ui.common.AudioStatsInfo
import io.agora.api.example.compose.ui.common.ChannelNameInput
import io.agora.api.example.compose.ui.common.DropdownMenuRaw
import io.agora.api.example.compose.ui.common.SliderRaw
import io.agora.api.example.compose.utils.TokenUtils
import io.agora.rtc2.ChannelMediaOptions
import io.agora.rtc2.Constants
import io.agora.rtc2.IRtcEngineEventHandler
import io.agora.rtc2.RtcEngine
import io.agora.rtc2.RtcEngineConfig

private const val EFFECT_SOUND_ID = 0
private const val EFFECT_FILE_PATH = "https://webdemo.agora.io/ding.mp3"

@Composable
fun PlayAudioFiles() {
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
            preloadEffect(EFFECT_SOUND_ID, EFFECT_FILE_PATH);
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

    PlayAudioFilesView(videoIdList = videoIdList,
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
                    android.Manifest.permission.CAMERA
                )
            )
        },
        onLeaveClick = {
            rtcEngine.leaveChannel()
        }
    )
}


@Composable
private fun PlayAudioFilesView(
    videoIdList: List<Int>,
    statsMap: Map<Int, AudioStatsInfo> = mapOf(),
    rtcEngine: RtcEngine? = null,
    channelName: String = "",
    isJoined: Boolean = false,
    onJoinClick: (String) -> Unit = {},
    onLeaveClick: () -> Unit = {}
) {
    Column {
        AudioGrid(
            modifier = Modifier.height(160.dp),
            raw = 1,
            videoIdList = videoIdList,
            statsMap = statsMap
        )

        LazyColumn(modifier = Modifier.weight(1f)) {
            item {
                Column {
                    Text(
                        text = "Audio Mixing",
                        style = MaterialTheme.typography.titleSmall,
                        modifier = Modifier.padding(16.dp, 8.dp)
                    )

                    Row(
                        modifier = Modifier
                            .fillMaxWidth()
                            .padding(16.dp, 0.dp)
                    ) {
                        Text(
                            text = stringResource(id = R.string.start),
                            style = MaterialTheme.typography.bodySmall,
                            color = Color.Blue,
                            textAlign = TextAlign.Center,
                            modifier = Modifier
                                .weight(1.0f)
                                .padding(16.dp, 8.dp)
                                .clickable {
                                    rtcEngine?.startAudioMixing(
                                        "/assets/music_1.m4a",
                                        false,
                                        -1,
                                        0
                                    );
                                }
                        )
                        Text(
                            text = stringResource(id = R.string.resume),
                            style = MaterialTheme.typography.bodySmall,
                            color = Color.Blue,
                            textAlign = TextAlign.Center,
                            modifier = Modifier
                                .weight(1.0f)
                                .padding(16.dp, 8.dp)
                                .clickable {
                                    rtcEngine?.resumeAudioMixing()
                                }
                        )
                        Text(
                            text = stringResource(id = R.string.pause),
                            style = MaterialTheme.typography.bodySmall,
                            color = Color.Blue,
                            textAlign = TextAlign.Center,
                            modifier = Modifier
                                .weight(1.0f)
                                .padding(16.dp, 8.dp)
                                .clickable {
                                    rtcEngine?.pauseAudioMixing()
                                }
                        )
                        Text(
                            text = stringResource(id = R.string.stop),
                            style = MaterialTheme.typography.bodySmall,
                            color = Color.Blue,
                            textAlign = TextAlign.Center,
                            modifier = Modifier
                                .weight(1.0f)
                                .padding(16.dp, 8.dp)
                                .clickable {
                                    rtcEngine?.stopAudioMixing()
                                }
                        )
                    }

                    SliderRaw(title = stringResource(id = R.string.audio_mixing_volume), value = 1.0f) {
                        rtcEngine?.adjustAudioMixingVolume((100 * it).toInt())
                    }
                    SliderRaw(title = stringResource(id = R.string.audio_mixing_playout_volume), value = 1.0f) {
                        rtcEngine?.adjustAudioMixingPlayoutVolume((100 * it).toInt())
                    }
                    SliderRaw(title = stringResource(id = R.string.audio_mixing_publish_volume), value = 1.0f) {
                        rtcEngine?.adjustAudioMixingPublishVolume((100 * it).toInt())
                    }
                }
            }
            item {
                Column {
                    Text(
                        text = "Audio Effect",
                        style = MaterialTheme.typography.titleSmall,
                        modifier = Modifier.padding(16.dp, 8.dp)
                    )

                    Row(
                        modifier = Modifier
                            .fillMaxWidth()
                            .padding(16.dp, 0.dp)
                    ) {
                        Text(text = stringResource(id = R.string.start),
                            style = MaterialTheme.typography.bodySmall,
                            color = Color.Blue,
                            textAlign = TextAlign.Center,
                            modifier = Modifier
                                .weight(1.0f)
                                .padding(16.dp, 8.dp)
                                .clickable {
                                    rtcEngine?.playEffect(
                                        EFFECT_SOUND_ID,  // The sound ID of the audio effect file to be played.
                                        EFFECT_FILE_PATH,  // The file path of the audio effect file.
                                        -1,  // The number of playback loops. -1 means an infinite loop.
                                        1.0,  // pitch The pitch of the audio effect. The value ranges between 0.5 and 2. The default value is 1 (no change to the pitch). The lower the value, the lower the pitch.
                                        0.0,  // Sets the spatial position of the effect. 0 means the effect shows ahead.
                                        100.0,  // Sets the volume. The value ranges between 0 and 100. 100 is the original volume.
                                        true // Sets whether to publish the audio effect.
                                    );
                                })
                        Text(text = stringResource(id = R.string.resume),
                            style = MaterialTheme.typography.bodySmall,
                            color = Color.Blue,
                            textAlign = TextAlign.Center,
                            modifier = Modifier
                                .weight(1.0f)
                                .padding(16.dp, 8.dp)
                                .clickable {
                                    rtcEngine?.resumeEffect(EFFECT_SOUND_ID)
                                })
                        Text(text = stringResource(id = R.string.pause),
                            style = MaterialTheme.typography.bodySmall,
                            color = Color.Blue,
                            textAlign = TextAlign.Center,
                            modifier = Modifier
                                .weight(1.0f)
                                .padding(16.dp, 8.dp)
                                .clickable {
                                    rtcEngine?.pauseEffect(EFFECT_SOUND_ID)
                                })
                        Text(text = stringResource(id = R.string.stop),
                            style = MaterialTheme.typography.bodySmall,
                            color = Color.Blue,
                            textAlign = TextAlign.Center,
                            modifier = Modifier
                                .weight(1.0f)
                                .padding(16.dp, 8.dp)
                                .clickable {
                                    rtcEngine?.stopEffect(EFFECT_SOUND_ID)
                                })
                    }

                    SliderRaw(title = stringResource(id = R.string.effects_volume), value = 1.0f) {
                        rtcEngine?.setEffectsVolume((100 * it).toDouble())
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
            )
        ) { _, option ->
            rtcEngine?.setAudioProfile(option.second)
        }
        DropdownMenuRaw(
            title = "Scenario",
            options = listOf(
                "Default" to Constants.AUDIO_SCENARIO_DEFAULT,
                "Game Streaming" to Constants.AUDIO_SCENARIO_GAME_STREAMING,
                "Chatroom" to Constants.AUDIO_SCENARIO_CHATROOM,
                "Chorus" to Constants.AUDIO_SCENARIO_CHORUS,
                "Meeting" to Constants.AUDIO_SCENARIO_MEETING
            )
        ) { _, option ->
            rtcEngine?.setAudioScenario(option.second)
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
private fun PlayAudioFilesViewPreview() {
    PlayAudioFilesView(videoIdList = listOf(1, 2))
}