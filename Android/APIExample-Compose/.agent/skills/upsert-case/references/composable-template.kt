package io.agora.api.example.compose.samples

import android.Manifest
import androidx.activity.compose.rememberLauncherForActivityResult
import androidx.activity.result.contract.ActivityResultContracts
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.runtime.Composable
import androidx.compose.runtime.DisposableEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableIntStateOf
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.saveable.rememberSaveable
import androidx.compose.runtime.setValue
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.platform.LocalLifecycleOwner
import androidx.compose.ui.tooling.preview.Preview
import io.agora.api.example.compose.data.SettingPreferences
import io.agora.api.example.compose.ui.common.ChannelNameInput
import io.agora.api.example.compose.utils.AgoraConfig
import io.agora.api.example.compose.utils.TokenUtils
import io.agora.rtc2.ChannelMediaOptions
import io.agora.rtc2.Constants
import io.agora.rtc2.IRtcEngineEventHandler
import io.agora.rtc2.RtcEngine
import io.agora.rtc2.RtcEngineConfig

// For video cases, also import:
// import io.agora.api.example.compose.ui.common.VideoGrid
// import io.agora.rtc2.video.VideoCanvas
// import io.agora.rtc2.video.VideoEncoderConfiguration

/**
 * Demonstrates how to use [describe the feature here].
 *
 * Key APIs used:
 * - RtcEngine.yourApi()
 */
// PUBLIC stateful entry point — no @Preview here
@Composable
fun YourCaseName() {
    val context = LocalContext.current
    val lifecycleOwner = LocalLifecycleOwner.current

    // rememberSaveable: survives rotation — use for channelName, isJoined, uid, videoIdList
    var isJoined by rememberSaveable { mutableStateOf(false) }
    var channelName by rememberSaveable { mutableStateOf("") }
    var localUid by rememberSaveable { mutableIntStateOf(0) }

    // remember: survives recomposition but NOT rotation — use for RtcEngine, collections
    val rtcEngine = remember {
        RtcEngine.create(RtcEngineConfig().apply {
            mAreaCode = SettingPreferences.getArea()   // REQUIRED — do not hardcode
            mContext = context
            mAppId = AgoraConfig.getAppId()
            mEventHandler = object : IRtcEngineEventHandler() {
                // IRtcEngineEventHandler callbacks are safe to mutate Compose state directly —
                // the snapshot system is thread-safe. No runOnUIThread() needed.
                override fun onJoinChannelSuccess(channel: String?, uid: Int, elapsed: Int) {
                    super.onJoinChannelSuccess(channel, uid, elapsed)
                    isJoined = true
                    localUid = uid
                }

                override fun onLeaveChannel(stats: RtcStats?) {
                    super.onLeaveChannel(stats)
                    isJoined = false
                }

                override fun onUserJoined(uid: Int, elapsed: Int) {
                    super.onUserJoined(uid, elapsed)
                    // add uid to videoIdList for video cases
                }

                override fun onUserOffline(uid: Int, reason: Int) {
                    super.onUserOffline(uid, reason)
                    // remove uid from videoIdList for video cases
                }
            }
        }).apply {
            // feature-specific engine setup goes here
            // e.g. enableVideo(); setVideoEncoderConfiguration(...)
        }
    }

    // MUST use lifecycleOwner as key — ensures cleanup fires when screen leaves composition
    DisposableEffect(lifecycleOwner) {
        onDispose {
            if (isJoined) rtcEngine.leaveChannel()
            RtcEngine.destroy()
            // NOTE: Toast/Dialog/AlertDialog MUST be called on main thread.
            // Inside onDispose this is fine. Inside IRtcEngineEventHandler callbacks,
            // wrap with: coroutineScope.launch(Dispatchers.Main) { ... }
        }
    }

    val permissionLauncher = rememberLauncherForActivityResult(
        ActivityResultContracts.RequestMultiplePermissions()
    ) { grantedMap ->
        // Permission callbacks run on main thread — Toast is safe here
        if (grantedMap.values.all { it }) {
            TokenUtils.gen(channelName, 0) { token ->
                val options = ChannelMediaOptions().apply {
                    channelProfile = Constants.CHANNEL_PROFILE_LIVE_BROADCASTING
                    clientRoleType = Constants.CLIENT_ROLE_BROADCASTER
                    publishMicrophoneTrack = true
                    // publishCameraTrack = true  // add for video cases
                }
                rtcEngine.joinChannel(token, channelName, 0, options)
            }
        }
    }

    // Delegate all UI to the private stateless View function
    YourCaseNameView(
        channelName = channelName,
        isJoined = isJoined,
        onJoinClick = { name ->
            channelName = name
            permissionLauncher.launch(
                arrayOf(Manifest.permission.RECORD_AUDIO)
                // add Manifest.permission.CAMERA for video cases
            )
        },
        onLeaveClick = { rtcEngine.leaveChannel() }
    )
}

// @Preview goes here on the PRIVATE stateless function — never on the stateful entry above
@Preview
@Composable
private fun YourCaseNamePreview() {
    YourCaseNameView(
        channelName = "test",
        isJoined = false,
        onJoinClick = {},
        onLeaveClick = {}
    )
}

// PRIVATE stateless View — receives only plain data and lambdas, no engine/state
@Composable
private fun YourCaseNameView(
    channelName: String,
    isJoined: Boolean,
    onJoinClick: (String) -> Unit,
    onLeaveClick: () -> Unit
) {
    Column(Modifier.fillMaxSize()) {
        // feature-specific UI here
        // For video cases: VideoGrid(videoIdList, setupVideo, ...)
        ChannelNameInput(
            channelName = channelName,
            isJoined = isJoined,
            onJoinClick = onJoinClick,
            onLeaveClick = onLeaveClick
        )
    }
}
