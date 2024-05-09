package io.agora.api.example.compose.samples

import android.Manifest
import android.content.Context
import android.os.Build
import android.util.Log
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
import io.agora.api.example.compose.ui.common.SwitchRaw
import io.agora.api.example.compose.utils.TokenUtils
import io.agora.rtc2.ChannelMediaOptions
import io.agora.rtc2.Constants
import io.agora.rtc2.IAudioFrameObserver
import io.agora.rtc2.IRtcEngineEventHandler
import io.agora.rtc2.RtcEngine
import io.agora.rtc2.RtcEngineConfig
import io.agora.rtc2.audio.AudioParams
import java.io.IOException
import java.io.InputStream
import java.nio.ByteBuffer

@Composable
fun OriginAudioData() {
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
    val audioDataRewriter = remember(context, rtcEngine) {
        OriginAudioDataRewriter(context, rtcEngine)
    }

    DisposableEffect(lifecycleOwner) {
        onDispose {
            audioDataRewriter.dispose()
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

    OriginAudioDataView(
        videoIdList = videoIdList,
        statsMap = statsMap,
        channelName = channelName,
        isJoined = isJoined,
        onRewriteOpen = { isOpen ->
            audioDataRewriter.rewritable = isOpen
        },
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
private fun OriginAudioDataView(
    videoIdList: List<Int>,
    statsMap: Map<Int, AudioStatsInfo> = mapOf(),
    channelName: String = "",
    isJoined: Boolean = false,
    onRewriteOpen: (Boolean) -> Unit = {},
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

        SwitchRaw(title = "音频回写", enable = isJoined, onCheckedChange = onRewriteOpen)

        ChannelNameInput(
            channelName = channelName,
            isJoined = isJoined,
            onJoinClick = onJoinClick,
            onLeaveClick = onLeaveClick
        )
    }
}

private class OriginAudioDataRewriter(
    private val context: Context,
    private val rtcEngine: RtcEngine
) : IAudioFrameObserver {
    private val TAG = "OriginAudioDataRewriter"
    private val SAMPLE_RATE = 44100
    private val SAMPLE_NUM_OF_CHANNEL = 1
    private val SAMPLES = 1024
    private val AUDIO_FILE = "output.raw"
    private var inputStream: InputStream? = null


    var rewritable = false

    init {
        rtcEngine.registerAudioFrameObserver(this)
        rtcEngine.setRecordingAudioFrameParameters(
            SAMPLE_RATE,
            SAMPLE_NUM_OF_CHANNEL,
            Constants.RAW_AUDIO_FRAME_OP_MODE_READ_WRITE,
            SAMPLES
        )
        rtcEngine.setPlaybackAudioFrameParameters(
            SAMPLE_RATE,
            SAMPLE_NUM_OF_CHANNEL,
            Constants.RAW_AUDIO_FRAME_OP_MODE_READ_WRITE,
            SAMPLES
        )
        openAudioFile()
    }

    fun dispose() {
        closeAudioFile()
        rtcEngine.registerAudioFrameObserver(null)
    }

    private fun openAudioFile() {
        try {
            inputStream = context.getResources().getAssets()
                .open(AUDIO_FILE)
        } catch (e: IOException) {
            e.printStackTrace()
        }
    }

    private fun closeAudioFile() {
        try {
            inputStream!!.close()
        } catch (e: IOException) {
            e.printStackTrace()
        }
    }

    private fun readBuffer(): ByteArray {
        val byteSize: Int = SAMPLES * SAMPLE_NUM_OF_CHANNEL * 2
        val buffer = ByteArray(byteSize)
        try {
            if ((inputStream?.read(buffer) ?: -1) < 0) {
                inputStream?.reset()
                return readBuffer()
            }
        } catch (e: IOException) {
            e.printStackTrace()
        }
        return buffer
    }

    private fun audioAggregate(origin: ByteArray, buffer: ByteArray): ByteArray {
        val output = ByteArray(buffer.size)
        for (i in origin.indices) {
            output[i] = (origin[i].toLong() / 2 + buffer[i].toLong() / 2).toByte()
            if (i == 2) {
                Log.i(TAG, "origin :" + origin[i].toInt() + " audio: " + buffer[i].toInt())
            }
        }
        return output
    }

    override fun onRecordAudioFrame(
        channelId: String?,
        type: Int,
        samplesPerChannel: Int,
        bytesPerSample: Int,
        channels: Int,
        samplesPerSec: Int,
        byteBuffer: ByteBuffer?,
        renderTimeMs: Long,
        avsync_type: Int
    ): Boolean {
        byteBuffer ?: return true
        if (rewritable) {
            val length: Int = byteBuffer.remaining()
            val buffer: ByteArray = readBuffer()
            val origin = ByteArray(length)
            byteBuffer.get(origin)
            byteBuffer.flip()
            byteBuffer.put(audioAggregate(origin, buffer), 0, byteBuffer.remaining())
            byteBuffer.flip()
        }
        return true
    }

    override fun onPlaybackAudioFrame(
        channelId: String?,
        type: Int,
        samplesPerChannel: Int,
        bytesPerSample: Int,
        channels: Int,
        samplesPerSec: Int,
        buffer: ByteBuffer?,
        renderTimeMs: Long,
        avsync_type: Int
    ) = false

    override fun onMixedAudioFrame(
        channelId: String?,
        type: Int,
        samplesPerChannel: Int,
        bytesPerSample: Int,
        channels: Int,
        samplesPerSec: Int,
        buffer: ByteBuffer?,
        renderTimeMs: Long,
        avsync_type: Int
    ) = false

    override fun onEarMonitoringAudioFrame(
        type: Int,
        samplesPerChannel: Int,
        bytesPerSample: Int,
        channels: Int,
        samplesPerSec: Int,
        buffer: ByteBuffer?,
        renderTimeMs: Long,
        avsync_type: Int
    ) = false

    override fun onPlaybackAudioFrameBeforeMixing(
        channelId: String?,
        uid: Int,
        type: Int,
        samplesPerChannel: Int,
        bytesPerSample: Int,
        channels: Int,
        samplesPerSec: Int,
        buffer: ByteBuffer?,
        renderTimeMs: Long,
        avsync_type: Int,
        rtpTimestamp: Int
    ) = false

    override fun getObservedAudioFramePosition() =
        Constants.AUDIO_ENCODED_FRAME_OBSERVER_POSITION_MIC

    override fun getRecordAudioParams(): AudioParams? = null

    override fun getPlaybackAudioParams(): AudioParams? = null
    override fun getMixedAudioParams(): AudioParams? = null

    override fun getEarMonitoringAudioParams(): AudioParams? = null

}