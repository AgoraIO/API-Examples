package io.agora.api.example.compose.samples

import android.util.Log
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.padding
import androidx.compose.material3.Button
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.DisposableEffect
import androidx.compose.runtime.getValue
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
import io.agora.api.example.compose.data.SettingPreferences
import io.agora.api.example.compose.ui.common.ChannelNameInput
import io.agora.api.example.compose.ui.common.SliderRaw
import io.agora.rtc2.ChannelMediaOptions
import io.agora.rtc2.Constants
import io.agora.rtc2.IRtcEngineEventHandler
import io.agora.rtc2.RtcEngine
import io.agora.rtc2.RtcEngineConfig
import io.agora.rtc2.audio.AgoraRhythmPlayerConfig
import io.agora.rtc2.video.VideoEncoderConfiguration

private const val URL_UPBEAT = "https://webdemo.agora.io/ding.mp3"
private const val URL_DOWNBEAT = "https://webdemo.agora.io/dang.mp3"

@Composable
fun RhythmPlayer() {
    val context = LocalContext.current
    val lifecycleOwner = LocalLifecycleOwner.current
    val keyboard = LocalSoftwareKeyboardController.current
    var isJoined by rememberSaveable { mutableStateOf(false) }
    var channelName by rememberSaveable { mutableStateOf("") }
    var isPlaying by rememberSaveable { mutableStateOf(false) }

    val rtcEngine = remember {
        RtcEngine.create(RtcEngineConfig().apply {
            mContext = context
            mAppId = BuildConfig.AGORA_APP_ID
            mEventHandler = object : IRtcEngineEventHandler() {
                override fun onJoinChannelSuccess(channel: String?, uid: Int, elapsed: Int) {
                    super.onJoinChannelSuccess(channel, uid, elapsed)
                    isJoined = true
                }

                override fun onLeaveChannel(stats: RtcStats?) {
                    super.onLeaveChannel(stats)
                    isJoined = false
                }

                override fun onRhythmPlayerStateChanged(state: Int, reason: Int) {
                    super.onRhythmPlayerStateChanged(state, reason)
                    Log.d("RhythmPlayer", "state: $state, reason: $reason")
                    isPlaying = state == Constants.RHYTHM_PLAYER_STATE_PLAYING
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
            rtcEngine.leaveChannel()
            RtcEngine.destroy()
        }
    }

    RhythmPlayerView(
        rtcEngine = rtcEngine,
        channelName = channelName,
        isJoined = isJoined,
        isPlaying = isPlaying,
        onPlayStopClick = {
            isPlaying = false
        },
        onJoinClick = {
            keyboard?.hide()
            channelName = it
            val mediaOptions = ChannelMediaOptions()
            mediaOptions.channelProfile = Constants.CHANNEL_PROFILE_LIVE_BROADCASTING
            mediaOptions.clientRoleType = Constants.CLIENT_ROLE_BROADCASTER
            mediaOptions.autoSubscribeAudio = true
            mediaOptions.autoSubscribeVideo = true
            mediaOptions.publishRhythmPlayerTrack = true
            mediaOptions.publishMicrophoneTrack = false
            mediaOptions.publishCameraTrack = false
            rtcEngine.joinChannel("", channelName, 0, mediaOptions)
        },
        onLeaveClick = {
            rtcEngine.leaveChannel()
        })
}

@Composable
fun RhythmPlayerView(
    rtcEngine: RtcEngine? = null,
    channelName: String,
    isJoined: Boolean,
    isPlaying: Boolean,
    onJoinClick: (String) -> Unit,
    onLeaveClick: () -> Unit,
    onPlayStopClick: () -> Unit
) {
    val config = remember { AgoraRhythmPlayerConfig() }
    Column {
        Spacer(modifier = Modifier.weight(1.0f))
        Column(modifier = Modifier.padding(16.dp, 0.dp)) {
            SliderRaw(
                title = "Beats per Minute",
                value = (config.beatsPerMinute - 60) / 300.0f,
                enable = !isPlaying
            ) {
                // 60 ~ 360
                config.beatsPerMinute = 60 + (it * 300).toInt()
            }
            SliderRaw(
                title = "Beats per Measure",
                value = (config.beatsPerMeasure - 1) / 8.0f,
                enable = !isPlaying
            ) {
                // 1 ~ 9
                config.beatsPerMeasure = 1 + (it * 8).toInt()
            }
            Row(modifier = Modifier.padding(8.dp, 0.dp)) {
                Button(
                    modifier = Modifier
                        .weight(1.0f)
                        .padding(8.dp, 0.dp),
                    onClick = {
                        rtcEngine?.startRhythmPlayer(URL_UPBEAT, URL_DOWNBEAT, config)
                    }
                ) {
                    Text(text = "Play")
                }
                Button(
                    modifier = Modifier
                        .weight(1.0f)
                        .padding(8.dp, 0.dp),
                    onClick = {
                        rtcEngine?.stopRhythmPlayer()
                        onPlayStopClick()
                    }
                ) {
                    Text(text = "Stop")
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