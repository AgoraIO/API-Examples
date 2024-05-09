package io.agora.api.example.compose.samples

import android.os.Handler
import android.os.Looper
import android.view.SurfaceView
import android.view.View
import android.widget.Toast
import androidx.activity.compose.rememberLauncherForActivityResult
import androidx.activity.result.contract.ActivityResultContracts
import androidx.compose.foundation.background
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.fillMaxHeight
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.material3.Button
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.DisposableEffect
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.saveable.rememberSaveable
import androidx.compose.runtime.setValue
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.platform.LocalLifecycleOwner
import androidx.compose.ui.res.stringResource
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import io.agora.api.example.compose.BuildConfig
import io.agora.api.example.compose.R
import io.agora.api.example.compose.data.SettingPreferences
import io.agora.api.example.compose.ui.common.VideoCell
import io.agora.rtc2.Constants
import io.agora.rtc2.EchoTestConfiguration
import io.agora.rtc2.IRtcEngineEventHandler
import io.agora.rtc2.RtcEngine
import io.agora.rtc2.RtcEngineConfig
import io.agora.rtc2.internal.LastmileProbeConfig
import io.agora.rtc2.video.VideoEncoderConfiguration
import java.util.Random

private const val ECHO_TEST_INTERVAL_IN_SECONDS = 8

@Composable
fun PreCallTest() {
    val context = LocalContext.current
    val lifecycleOwner = LocalLifecycleOwner.current
    var isNetworkTesting by rememberSaveable { mutableStateOf(false) }
    var isAudioEchoPretesting by rememberSaveable { mutableStateOf(false) }
    var isVideoEchoPretesting by rememberSaveable { mutableStateOf(false) }
    var networkTestResult by rememberSaveable { mutableStateOf("") }
    val handler = remember { Handler(Looper.getMainLooper()) }

    val rtcEngine = remember {
        var engine: RtcEngine? = null
        RtcEngine.create(RtcEngineConfig().apply {
            mAreaCode = SettingPreferences.getArea()
            mContext = context
            mAppId = BuildConfig.AGORA_APP_ID
            mEventHandler = object : IRtcEngineEventHandler() {
                override fun onLastmileQuality(quality: Int) {
                    super.onLastmileQuality(quality)
                    val qualityName = when (quality) {
                        Constants.QUALITY_EXCELLENT -> "Excellent"
                        Constants.QUALITY_GOOD -> "Good"
                        Constants.QUALITY_POOR -> "Poor"
                        Constants.QUALITY_BAD -> "Bad"
                        Constants.QUALITY_VBAD -> "Very Bad"
                        Constants.QUALITY_DOWN -> "Down"
                        else -> "Unsupported"
                    }
                    networkTestResult = "Quality: $qualityName"


                }

                override fun onLastmileProbeResult(result: LastmileProbeResult?) {
                    super.onLastmileProbeResult(result)
                    result ?: return
                    isNetworkTesting = false
                    engine?.stopLastmileProbeTest()
                    val resultInfo = "\nRtt: ${result.rtt}ms" +
                            "\nDownlinkAvailableBandwidth: ${result.downlinkReport.availableBandwidth}Kbps" +
                            "\nDownlinkJitter: ${result.downlinkReport.jitter}ms" +
                            "\nDownlinkLoss: ${result.downlinkReport.packetLossRate}%" +
                            "\nUplinkAvailableBandwidth: ${result.uplinkReport.availableBandwidth}Kbps" +
                            "\nUplinkJitter: ${result.uplinkReport.jitter}ms" +
                            "\nUplinkLoss: ${result.uplinkReport.packetLossRate}%"
                    networkTestResult += resultInfo
                }
            }
        }).apply {
            engine = this
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
    val permissionLauncher =
        rememberLauncherForActivityResult(contract = ActivityResultContracts.RequestMultiplePermissions()) { grantedMap ->
            val allGranted = grantedMap.values.all { it }
            if (allGranted) {
                // Permission is granted
                Toast.makeText(context, "Permission Granted", Toast.LENGTH_LONG).show()
            } else {
                // Permission is denied
                Toast.makeText(context, "Permission Denied", Toast.LENGTH_LONG).show()
            }
        }

    LaunchedEffect(lifecycleOwner) {
        permissionLauncher.launch(
            arrayOf(
                android.Manifest.permission.RECORD_AUDIO,
                android.Manifest.permission.CAMERA
            )
        )
    }
    DisposableEffect(lifecycleOwner) {
        onDispose {
            handler.removeCallbacksAndMessages(null)
            RtcEngine.destroy()
        }
    }

    PreCallTestView(
        isAudioEchoPretesting = isAudioEchoPretesting,
        isVideoEchoPretesting = isVideoEchoPretesting,
        isNetworkTesting = isNetworkTesting,
        networkTestResult = networkTestResult,
        onNetworkTestClick = {
            isNetworkTesting = true
            rtcEngine.startLastmileProbeTest(
                LastmileProbeConfig().apply {
                    probeUplink = true
                    probeDownlink = true
                    expectedUplinkBitrate = 100000
                    expectedDownlinkBitrate = 100000
                }
            )
        },
        onAudioEchoPretestClick = {
            isAudioEchoPretesting = true
            val config = EchoTestConfiguration()
            config.enableVideo = false
            config.enableAudio = true
            config.intervalInSeconds = ECHO_TEST_INTERVAL_IN_SECONDS
            config.channelId = "AudioEchoTest" + (Random().nextInt(1000) + 10000)
            rtcEngine.startEchoTest(config)
            handler.postDelayed({
                isAudioEchoPretesting = false
                rtcEngine.stopEchoTest()
            }, ECHO_TEST_INTERVAL_IN_SECONDS * 2 * 1000L)
        },
        onVideoEchoPretestClick = {
            isVideoEchoPretesting = true
            handler.postDelayed({
                isVideoEchoPretesting = false
                rtcEngine.stopEchoTest()
            }, ECHO_TEST_INTERVAL_IN_SECONDS * 2 * 1000L)
        },
        onVideoEchoViewCreated = {
            val config = EchoTestConfiguration()
            config.enableVideo = true
            config.view = it as? SurfaceView
            config.enableAudio = false
            config.intervalInSeconds = ECHO_TEST_INTERVAL_IN_SECONDS
            config.channelId = "VideoEchoTest" + (Random().nextInt(1000) + 10000)
            rtcEngine.startEchoTest(config)
        }
    )
}

@Composable
private fun PreCallTestView(
    isAudioEchoPretesting: Boolean = false,
    isVideoEchoPretesting: Boolean = false,
    isNetworkTesting: Boolean = false,
    networkTestResult: String = "",
    onNetworkTestClick: () -> Unit = {},
    onAudioEchoPretestClick: () -> Unit = {},
    onVideoEchoPretestClick: () -> Unit = {},
    onVideoEchoViewCreated: (View) -> Unit = { _ -> }
) {
    Column(
        modifier = Modifier
            .fillMaxWidth()
            .fillMaxHeight()
            .padding(16.dp, 8.dp)
    ) {
        Text(
            modifier = Modifier.padding(0.dp, 8.dp),
            text = "Lastmile Network Pretest",
            style = MaterialTheme.typography.titleMedium
        )
        Button(
            onClick = onNetworkTestClick,
            enabled = !isNetworkTesting
        ) {
            Text(if (!isNetworkTesting) "Start Test" else "Test ...")
        }
        Box(
            modifier = Modifier
                .fillMaxWidth()
                .padding(0.dp, 8.dp)
                .height(200.dp)
                .background(Color(0xFFEEEEEE))
        ) {
            LazyColumn(modifier = Modifier.padding(8.dp)) {
                item {
                    Text(
                        text = networkTestResult,
                        style = MaterialTheme.typography.bodyMedium
                    )
                }
            }
        }

        Text(
            modifier = Modifier.padding(0.dp, 8.dp),
            text = "Audio Echo Pretest",
            style = MaterialTheme.typography.titleMedium
        )
        Button(
            onClick = onAudioEchoPretestClick,
            enabled = !isAudioEchoPretesting && !isVideoEchoPretesting
        ) {
            Text(if (!isAudioEchoPretesting) "Start Test" else "Test ...")
        }
        Text(
            modifier = Modifier.padding(0.dp, 8.dp),
            text = stringResource(id = R.string.audio_echo_pretest_tip, ECHO_TEST_INTERVAL_IN_SECONDS),
            style = MaterialTheme.typography.bodySmall
        )

        Text(
            modifier = Modifier.padding(0.dp, 8.dp),
            text = "Video Echo Pretest",
            style = MaterialTheme.typography.titleMedium
        )
        Button(
            onClick = onVideoEchoPretestClick,
            enabled = !isAudioEchoPretesting && !isVideoEchoPretesting
        ) {
            Text(if (!isVideoEchoPretesting) "Start Test" else "Test ...")
        }

        if (isVideoEchoPretesting) {
            VideoCell(
                modifier = Modifier
                    .width(180.dp)
                    .height(180.dp),
                id = 1,
                isLocal = true,
                createView = { SurfaceView(it) },
                setupVideo = { v, _, _ ->
                    onVideoEchoViewCreated(v)
                })
        }
    }
}

@Preview
@Composable
private fun PreCallTestViewPreview() {
    PreCallTestView(
        isVideoEchoPretesting = true
    )
}