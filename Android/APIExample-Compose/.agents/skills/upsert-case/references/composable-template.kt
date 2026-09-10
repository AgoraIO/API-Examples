package io.agora.api.example.compose.samples

import android.Manifest
import android.content.Context
import android.os.Handler
import android.os.Looper
import androidx.activity.compose.rememberLauncherForActivityResult
import androidx.activity.result.contract.ActivityResultContracts
import androidx.compose.runtime.Composable
import androidx.compose.runtime.DisposableEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.saveable.rememberSaveable
import androidx.compose.runtime.setValue
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.tooling.preview.Preview
import io.agora.api.example.compose.BuildConfig
import io.agora.api.example.compose.data.SettingPreferences
import io.agora.api.example.compose.utils.AgoraConfig
import io.agora.api.example.compose.utils.TokenUtils
import io.agora.rtc2.ChannelMediaOptions
import io.agora.rtc2.IRtcEngineEventHandler
import io.agora.rtc2.IRtcEngineEventHandler.RtcStats
import io.agora.rtc2.RtcEngine
import io.agora.rtc2.RtcEngineConfig

@Composable
fun ExampleCase() {
    val context = LocalContext.current
    var channelName by rememberSaveable { mutableStateOf("") }
    var isJoined by remember { mutableStateOf(false) }
    var errorMessage by remember { mutableStateOf<String?>(null) }
    val session = remember {
        ExampleCaseSession(onJoined = { isJoined = it }, onError = { errorMessage = it })
    }
    DisposableEffect(session) {
        session.start(context)
        onDispose { session.dispose() }
    }

    val requiredPermissions = arrayOf(
        Manifest.permission.RECORD_AUDIO,
        // Add Manifest.permission.CAMERA for video-capable cases.
    )
    val permissionLauncher = rememberLauncherForActivityResult(
        ActivityResultContracts.RequestMultiplePermissions()
    ) { grantedMap ->
        session.onPermissionResult(requiredPermissions.all { grantedMap[it] == true })
    }

    ExampleCaseView(
        channelName = channelName,
        isJoined = isJoined,
        errorMessage = errorMessage,
        onJoinClick = { newChannelName ->
            channelName = newChannelName
            if (session.beginJoin(newChannelName)) {
                errorMessage = null
                permissionLauncher.launch(requiredPermissions)
            }
        },
        onLeaveClick = { session.leave() }
    )
}

// Owns one engine. Call public methods on main; all asynchronous continuations return to main.
// Keep this class in the case file, with its own name when creating a concrete case.
private class ExampleCaseSession(
    private val onJoined: (Boolean) -> Unit,
    private val onError: (String) -> Unit
) {
    private data class Request(val id: Long, val channel: String, val uid: Int = 0)
    private val mainHandler = Handler(Looper.getMainLooper())
    private var engine: RtcEngine? = null
    private var generation = 0L
    private var engineGeneration = 0L
    private var permissionRequest: Request? = null
    private var activeChannel: String? = null

    fun start(context: Context) {
        check(Looper.myLooper() == Looper.getMainLooper())
        if (engine != null) return
        engineGeneration += 1
        val lifetimeID = engineGeneration
        try {
            engine = RtcEngine.create(RtcEngineConfig().apply {
                mAreaCode = SettingPreferences.getArea()
                mContext = context.applicationContext
                mAppId = BuildConfig.AGORA_APP_ID
                mEventHandler = object : IRtcEngineEventHandler() {
                    override fun onJoinChannelSuccess(channel: String?, uid: Int, elapsed: Int) {
                        mainHandler.post {
                            if (engineGeneration == lifetimeID && engine != null && activeChannel == channel) onJoined(true)
                        }
                    }
                    override fun onLeaveChannel(stats: RtcStats?) {
                        mainHandler.post {
                            if (engineGeneration == lifetimeID && engine != null && activeChannel == null) onJoined(false)
                        }
                    }
                    // Dispatch case-specific callbacks to main and check current ownership too.
                }
            })
        } catch (error: Exception) {
            onError("RTC initialization failed")
        }
    }

    fun beginJoin(channel: String): Boolean {
        check(Looper.myLooper() == Looper.getMainLooper())
        // Only one permission launch may be outstanding: its result has no request identifier.
        if (engine == null || channel.isBlank() || permissionRequest != null || activeChannel != null) return false
        generation += 1
        permissionRequest = Request(generation, channel)
        return true
    }

    fun onPermissionResult(granted: Boolean) {
        check(Looper.myLooper() == Looper.getMainLooper())
        val request = permissionRequest ?: return
        permissionRequest = null
        val expectedEngine = engine ?: return
        if (request.id != generation) return
        if (!granted) {
            onError("Permission denied")
            return
        }
        val tokenRequired = AgoraConfig.getAppCertificate().isNotEmpty()
        // genToken supports projects without an App Certificate as well.
        TokenUtils.genToken(request.channel, request.uid) { token ->
            mainHandler.post {
                if (request.id != generation || engine !== expectedEngine) return@post
                if (tokenRequired && token.isNullOrEmpty()) {
                    onError("Token request failed")
                    return@post
                }
                val options = ChannelMediaOptions().apply {
                    publishMicrophoneTrack = true
                    // Configure video publication/canvases only after camera permission.
                }
                activeChannel = request.channel
                val result = expectedEngine.joinChannel(token, request.channel, request.uid, options)
                if (result != 0) {
                    activeChannel = null
                    onError("joinChannel failed: $result")
                }
            }
        }
    }

    fun leave() {
        check(Looper.myLooper() == Looper.getMainLooper())
        generation += 1
        activeChannel = null
        // Retain the pending permission slot until its obsolete result is consumed.
        engine?.leaveChannel()
        onJoined(false)
    }

    fun dispose() {
        leave() // Invalidate even if permission/Token/join is still pending.
        if (engine != null) {
            // Stop case-owned capture, players, timers and observers here.
            RtcEngine.destroy()
            engine = null
        }
    }
}

@Preview
@Composable
private fun ExampleCasePreview() {
    ExampleCaseView(
        channelName = "Channel Name",
        isJoined = false,
        errorMessage = null,
        onJoinClick = {},
        onLeaveClick = {}
    )
}

@Composable
private fun ExampleCaseView(
    channelName: String,
    isJoined: Boolean,
    errorMessage: String?,
    onJoinClick: (String) -> Unit,
    onLeaveClick: () -> Unit
) {
    // Replace with case-specific UI; display localized error text and keep Preview on this view.
}
