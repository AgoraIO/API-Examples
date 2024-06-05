package io.agora.api.example.compose.samples

import android.Manifest
import android.media.AudioFormat
import android.media.AudioManager
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
import io.agora.api.example.compose.data.SettingPreferences
import io.agora.api.example.compose.ui.common.AudioGrid
import io.agora.api.example.compose.ui.common.AudioStatsInfo
import io.agora.api.example.compose.ui.common.ChannelNameInput
import io.agora.api.example.compose.utils.AudioPlayer
import io.agora.api.example.compose.utils.TokenUtils
import io.agora.rtc2.ChannelMediaOptions
import io.agora.rtc2.Constants
import io.agora.rtc2.IRtcEngineEventHandler
import io.agora.rtc2.RtcEngine
import io.agora.rtc2.RtcEngineConfig
import java.nio.ByteBuffer

@Composable
fun CustomAudioRender() {
    val context = LocalContext.current
    val lifecycleOwner = LocalLifecycleOwner.current
    val keyboard = LocalSoftwareKeyboardController.current

    val SAMPLE_RATE = 44100
    val SAMPLE_NUM_OF_CHANNEL = 2
    val BITS_PER_SAMPLE = 16
    val SAMPLES = 441
    val BUFFER_SIZE = SAMPLES * BITS_PER_SAMPLE / 8 * SAMPLE_NUM_OF_CHANNEL

    var isJoined by rememberSaveable { mutableStateOf(false) }
    var channelName by rememberSaveable { mutableStateOf("") }
    var localUid by rememberSaveable { mutableIntStateOf(0) }
    var videoIdList by rememberSaveable { mutableStateOf(listOf<Int>()) }
    val statsMap = remember { mutableStateMapOf(0 to AudioStatsInfo()) }
    val audioPlayer = remember {
        AudioPlayer(
            AudioManager.STREAM_MUSIC,
            SAMPLE_RATE,
            SAMPLE_NUM_OF_CHANNEL,
            AudioFormat.ENCODING_PCM_16BIT
        )
    }
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
                    audioPlayer.startPlayer()
                }

                override fun onLeaveChannel(stats: RtcStats?) {
                    super.onLeaveChannel(stats)
                    isJoined = false
                    videoIdList = emptyList()
                    statsMap.clear()
                    audioPlayer.stopPlayer()
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
            setExternalAudioSink(true, SAMPLE_RATE, SAMPLE_NUM_OF_CHANNEL);
        }
    }

    val pullingThread = remember {
        Thread {
            android.os.Process.setThreadPriority(android.os.Process.THREAD_PRIORITY_URGENT_AUDIO);
            while (Thread.currentThread().isInterrupted.not()) {
                val frame = ByteBuffer.allocateDirect(BUFFER_SIZE)
                rtcEngine.pullPlaybackAudioFrame(frame, BUFFER_SIZE)
                val data = ByteArray(frame.remaining())
                frame.get(data, 0, data.size);

                // simple audio filter
                for (i in 0 until data.size) {
                    data[i] = (data[i] + 5).toByte()
                }

                audioPlayer.play(data, 0, BUFFER_SIZE);
            }
        }.apply {
            start()
        }
    }
    DisposableEffect(lifecycleOwner) {
        onDispose {
            pullingThread.interrupt()
            pullingThread.join()

            audioPlayer.stopPlayer()

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

    CustomAudioRenderView(
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
private fun CustomAudioRenderView(
    videoIdList: List<Int>,
    statsMap: Map<Int, AudioStatsInfo> = mapOf(),
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

        Spacer(modifier = Modifier.weight(1.0f))
        ChannelNameInput(
            channelName = channelName,
            isJoined = isJoined,
            onJoinClick = onJoinClick,
            onLeaveClick = onLeaveClick
        )
    }
}