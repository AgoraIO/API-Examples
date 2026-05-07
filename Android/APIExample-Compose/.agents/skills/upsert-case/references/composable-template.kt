package io.agora.api.example.compose.samples

import android.Manifest
import androidx.activity.compose.rememberLauncherForActivityResult
import androidx.activity.result.contract.ActivityResultContracts
import androidx.compose.runtime.Composable
import androidx.compose.runtime.DisposableEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableIntStateOf
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.saveable.rememberSaveable
import androidx.compose.runtime.setValue
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.platform.LocalLifecycleOwner
import androidx.compose.ui.tooling.preview.Preview
import io.agora.api.example.compose.BuildConfig
import io.agora.api.example.compose.data.SettingPreferences
import io.agora.api.example.compose.utils.TokenUtils
import io.agora.rtc2.ChannelMediaOptions
import io.agora.rtc2.IRtcEngineEventHandler
import io.agora.rtc2.RtcEngine
import io.agora.rtc2.RtcEngineConfig
import io.agora.rtc2.RtcStats

@Composable
fun ExampleCase() {
    val context = LocalContext.current
    val lifecycleOwner = LocalLifecycleOwner.current
    var channelName by rememberSaveable { mutableStateOf("") }
    var isJoined by rememberSaveable { mutableStateOf(false) }
    var localUid by rememberSaveable { mutableIntStateOf(0) }

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
                }

                // Add case-specific callbacks here for remote users, stats, or custom media flows.
            }
        })
    }

    DisposableEffect(lifecycleOwner) {
        onDispose {
            if (isJoined) {
                rtcEngine.leaveChannel()
            }
            RtcEngine.destroy()
        }
    }

    val permissionLauncher = rememberLauncherForActivityResult(
        ActivityResultContracts.RequestMultiplePermissions()
    ) { grantedMap ->
        if (grantedMap.values.all { it }) {
            TokenUtils.gen(channelName, 0) { token ->
                rtcEngine.joinChannel(token, channelName, 0, ChannelMediaOptions())
            }
        }
    }
    val requiredPermissions = arrayOf(
        Manifest.permission.RECORD_AUDIO,
        // Add Manifest.permission.CAMERA for video-capable cases.
    )

    ExampleCaseView(
        channelName = channelName,
        isJoined = isJoined,
        onJoinClick = { newChannelName ->
            channelName = newChannelName
            permissionLauncher.launch(requiredPermissions)
        },
        onLeaveClick = {
            rtcEngine.leaveChannel()
        }
    )
}

@Preview
@Composable
private fun ExampleCasePreview() {
    ExampleCaseView(
        channelName = "Channel Name",
        isJoined = false,
        onJoinClick = {},
        onLeaveClick = {}
    )
}

@Composable
private fun ExampleCaseView(
    channelName: String,
    isJoined: Boolean,
    onJoinClick: (String) -> Unit,
    onLeaveClick: () -> Unit
) {
    // Replace with case-specific UI while keeping preview on this private view function.
}
