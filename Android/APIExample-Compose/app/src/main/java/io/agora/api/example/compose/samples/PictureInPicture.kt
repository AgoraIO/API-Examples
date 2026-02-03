package io.agora.api.example.compose.samples

import android.app.AppOpsManager
import android.app.PictureInPictureParams
import android.content.Context
import android.content.ContextWrapper
import android.content.Intent
import android.graphics.RectF
import android.os.Build
import android.os.Process
import android.util.Log
import android.util.Rational
import android.widget.Toast
import androidx.activity.ComponentActivity
import androidx.activity.compose.rememberLauncherForActivityResult
import androidx.activity.result.contract.ActivityResultContracts
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.material3.Button
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.DisposableEffect
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableIntStateOf
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.toAndroidRectF
import androidx.compose.ui.layout.boundsInWindow
import androidx.compose.ui.layout.onGloballyPositioned
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.platform.LocalLifecycleOwner
import androidx.compose.ui.platform.LocalSoftwareKeyboardController
import androidx.compose.ui.unit.dp
import androidx.lifecycle.DefaultLifecycleObserver
import androidx.lifecycle.LifecycleOwner
import androidx.core.app.PictureInPictureModeChangedInfo
import androidx.core.util.Consumer
import io.agora.api.example.compose.BuildConfig
import io.agora.api.example.compose.R
import io.agora.api.example.compose.data.SettingPreferences
import io.agora.api.example.compose.ui.common.ChannelNameInput
import io.agora.api.example.compose.ui.common.TwoVideoView
import io.agora.api.example.compose.ui.common.TwoVideoViewType
import io.agora.api.example.compose.ui.common.VideoStatsInfo
import io.agora.api.example.compose.utils.TokenUtils
import io.agora.rtc2.ChannelMediaOptions
import io.agora.rtc2.Constants
import io.agora.rtc2.IRtcEngineEventHandler
import io.agora.rtc2.RtcEngine
import io.agora.rtc2.RtcEngineConfig
import io.agora.rtc2.video.VideoCanvas
import io.agora.rtc2.video.VideoEncoderConfiguration

// Global state storage that persists across component recreation
private val globalLocalUid = mutableIntStateOf(0)
private val globalRemoteUid = mutableIntStateOf(0)
private val globalChannelName = mutableStateOf("")
private val globalIsJoined = mutableStateOf(false)
private val isInPipTransition = mutableStateOf(false)
private val isPageLeaving = mutableStateOf(false) // Flag to track if user is truly leaving the page
private var globalCleanupFunction: (() -> Unit)? = null // Global cleanup function

// Helper function to find Activity from Context
private fun Context.findActivity(): ComponentActivity {
    var context = this
    while (context is ContextWrapper) {
        if (context is ComponentActivity) return context
        context = context.baseContext
    }
    throw IllegalStateException("Picture in picture should be called in the context of an Activity")
}

// Correct PiP state management following Android official guidelines
@Composable
private fun rememberIsInPipMode(): Boolean {
    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
        val activity = LocalContext.current.findActivity()
        var pipMode by remember { mutableStateOf(activity.isInPictureInPictureMode) }
        DisposableEffect(activity) {
            val observer = Consumer<PictureInPictureModeChangedInfo> { info ->
                pipMode = info.isInPictureInPictureMode
            }
            activity.addOnPictureInPictureModeChangedListener(observer)
            onDispose { activity.removeOnPictureInPictureModeChangedListener(observer) }
        }
        return pipMode
    } else {
        return false
    }
}

// Public function to clean up global state when user leaves the page
fun cleanupPictureInPictureState() {
    Log.d("PiPDebug", "cleanupPictureInPictureState called")
    globalCleanupFunction?.invoke()
}

@Composable
fun PictureInPicture() {
    val context = LocalContext.current as ComponentActivity
    // Use the correct PiP state management
    val isPipOn = rememberIsInPipMode()

    Log.d("PiPDebug", "PictureInPicture: Current isPipOn = $isPipOn")

    // Function to mark that user is leaving the page
    fun markPageLeaving() {
        isPageLeaving.value = true
        Log.d("PiPDebug", "Marked page as leaving - global state will be cleared on next dispose")
    }

    // Register cleanup function globally
    LaunchedEffect(Unit) {
        globalCleanupFunction = { markPageLeaving() }
    }

    // Add LaunchedEffect to handle PiP mode changes
    LaunchedEffect(isPipOn) {
        Log.d("PiPDebug", "PiP mode changed to: $isPipOn")
        // Mark that we're in a PiP transition
        isInPipTransition.value = true
        // Note: We can't access localUid and rtcEngine here as they're defined later
        // The video setup will be handled in the render callbacks
    }

    // Add DisposableEffect to track lifecycle
    DisposableEffect(Unit) {
        onDispose {
            // Only clear global state when user is truly leaving the page (not during PiP transitions)
            if (isPageLeaving.value) {
                Log.d("PiPDebug", "DisposableEffect: User is leaving page, clearing global state")
                globalLocalUid.intValue = 0
                globalRemoteUid.intValue = 0
                globalChannelName.value = ""
                globalIsJoined.value = false
                isPageLeaving.value = false // Reset flag
            } else {
                Log.d("PiPDebug", "DisposableEffect: Component recreation (PiP transition), preserving global state")
            }
        }
    }
    val lifecycleOwner = LocalLifecycleOwner.current
    val keyboard = LocalSoftwareKeyboardController.current
    // Use global state directly to avoid duplication
    var isJoined by globalIsJoined
    var channelName by globalChannelName
    var localUid by globalLocalUid
    var remoteUid by globalRemoteUid

    var localStats by remember { mutableStateOf(VideoStatsInfo()) }
    var remoteStats by remember { mutableStateOf(VideoStatsInfo()) }
    val videoViewBound = remember { RectF() }

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
                    localStats = VideoStatsInfo()
                    remoteUid = 0
                    remoteStats = VideoStatsInfo()
                }

                override fun onUserJoined(uid: Int, elapsed: Int) {
                    super.onUserJoined(uid, elapsed)
                    remoteUid = uid
                    remoteStats = VideoStatsInfo()
                }

                override fun onUserOffline(uid: Int, reason: Int) {
                    super.onUserOffline(uid, reason)
                    if (remoteUid == uid) {
                        remoteUid = 0
                        remoteStats = VideoStatsInfo()
                    }
                }

                override fun onRtcStats(stats: RtcStats?) {
                    super.onRtcStats(stats)
                    localStats.copy(rtcStats = stats).let {
                        localStats = it
                    }
                }

                override fun onLocalVideoStats(
                    source: Constants.VideoSourceType?,
                    stats: LocalVideoStats?
                ) {
                    super.onLocalVideoStats(source, stats)
                    localStats.copy(localVideoStats = stats).let {
                        localStats = it
                    }
                }

                override fun onLocalAudioStats(stats: LocalAudioStats?) {
                    super.onLocalAudioStats(stats)
                    localStats.copy(localAudioStats = stats).let {
                        localStats = it
                    }
                }

                override fun onRemoteVideoStats(stats: RemoteVideoStats?) {
                    super.onRemoteVideoStats(stats)
                    val uid = stats?.uid ?: return
                    if (remoteUid == uid) {
                        remoteStats.copy(remoteVideoStats = stats).let {
                            remoteStats = it
                        }
                    }
                }

                override fun onRemoteAudioStats(stats: RemoteAudioStats?) {
                    super.onRemoteAudioStats(stats)
                    val uid = stats?.uid ?: return
                    if (remoteUid == uid) {
                        remoteStats.copy(remoteAudioStats = stats).let {
                            remoteStats = it
                        }
                    }
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
    LaunchedEffect(lifecycleOwner) {
        lifecycleOwner.lifecycle.addObserver(object : DefaultLifecycleObserver {
            override fun onDestroy(owner: LifecycleOwner) {
                rtcEngine.stopPreview()
                rtcEngine.leaveChannel()
                RtcEngine.destroy()
            }
        })
    }
    val permissionLauncher =
        rememberLauncherForActivityResult(contract = ActivityResultContracts.RequestMultiplePermissions()) { grantedMap ->
            val allGranted = grantedMap.values.all { it }
            if (allGranted) {
                // Permission is granted
                Toast.makeText(context, R.string.permission_granted, Toast.LENGTH_LONG).show()
                val mediaOptions = ChannelMediaOptions()
                mediaOptions.channelProfile = Constants.CHANNEL_PROFILE_LIVE_BROADCASTING
                mediaOptions.clientRoleType = Constants.CLIENT_ROLE_BROADCASTER
                mediaOptions.autoSubscribeAudio = true
                mediaOptions.autoSubscribeAudio = true
                mediaOptions.publishCameraTrack = true
                mediaOptions.publishMicrophoneTrack = true
                TokenUtils.gen(channelName, 0) {
                    rtcEngine.joinChannel(it, channelName, 0, mediaOptions)
                }
            } else {
                // Permission is denied
                Toast.makeText(context, R.string.permission_denied, Toast.LENGTH_LONG).show()
            }
        }

    val videoView: @Composable () -> Unit = {
        TwoVideoView(
            modifier = Modifier
                .height(350.dp)
                .onGloballyPositioned { layoutCoordinates ->
                    videoViewBound.set(
                        layoutCoordinates
                            .boundsInWindow()
                            .toAndroidRectF()
                    )
                    val boundsInWindow = layoutCoordinates.boundsInWindow()
                    Log.d("PiPDebug", "VideoView distance from top: ${boundsInWindow.top}px")
                },
            type = TwoVideoViewType.Row,
            localUid = localUid,
            remoteUid = remoteUid,
            localStats = localStats,
            remoteStats = remoteStats,
            localRender = { view, id, isFirstSetup ->
                Log.d("PiPDebug", "localRender: view=$view, id=$id, isFirstSetup=$isFirstSetup, isJoined=$isJoined, isPipOn=$isPipOn")
                // Clear previous view first
                rtcEngine.setupLocalVideo(
                    VideoCanvas(
                        null,
                        Constants.RENDER_MODE_HIDDEN,
                        id
                    )
                )
                // Then set up new view
                rtcEngine.setupLocalVideo(
                    VideoCanvas(
                        view,
                        Constants.RENDER_MODE_HIDDEN,
                        id
                    )
                )
                rtcEngine.startPreview()
                Log.d("PiPDebug", "localRender: started preview")
            },
            remoteRender = { view, id, isFirstSetup ->
                Log.d("PiPDebug", "remoteRender: view=$view, id=$id, isFirstSetup=$isFirstSetup, remoteUid=$remoteUid, isPipOn=$isPipOn")
                // Clear previous view first
                rtcEngine.setupRemoteVideo(
                    VideoCanvas(
                        null,
                        Constants.RENDER_MODE_HIDDEN,
                        id
                    )
                )
                // Then set up new view
                rtcEngine.setupRemoteVideo(
                    VideoCanvas(
                        view,
                        Constants.RENDER_MODE_HIDDEN,
                        id
                    )
                )
                Log.d("PiPDebug", "remoteRender: setup completed")
            })
    }

    if (isPipOn) {
        Log.d("PiPDebug", "PictureInPicture： Rendering PiP mode - localUid: $localUid, remoteUid: $remoteUid, " +
                "isJoined: $isJoined")
        // In PiP mode, render only the video content without any scaffold or app bar
        // Use fillMaxSize to ensure video takes full available space in PiP window
        Box(modifier = Modifier.fillMaxSize()) {
            videoView()
        }
    } else {
        Log.d("PiPDebug", "PictureInPicture: Rendering normal mode - full UI")
        // Normal mode with full UI - let Example component handle the scaffold
        Column(modifier = Modifier.fillMaxWidth()) {
            videoView()
            Spacer(modifier = Modifier.weight(1f))

            Button(
                modifier = Modifier.padding(16.dp, 8.dp),
                enabled = isJoined,
                onClick = {
                    if (Build.VERSION.SDK_INT >= 26) {
                        val appOpsManager: AppOpsManager =
                            context.getSystemService(AppOpsManager::class.java)
                        if (appOpsManager.checkOpNoThrow(
                                AppOpsManager.OPSTR_PICTURE_IN_PICTURE,
                                Process.myUid(),
                                context.packageName
                            ) == AppOpsManager.MODE_ALLOWED
                        ) {
                            context.enterPictureInPictureMode(
                                PictureInPictureParams.Builder()
                                    .setAspectRatio(
                                        Rational(
                                            videoViewBound.width().toInt(),
                                            videoViewBound.height().toInt()
                                        )
                                    )
                                    .setActions(emptyList()) // Hide system actions (back button, etc.)
                                    .build()
                            )
                            val homeIntent = Intent(Intent.ACTION_MAIN)
                            homeIntent.addCategory(Intent.CATEGORY_HOME)
                            context.startActivity(homeIntent)
                            // isPipOn is now managed by rememberIsInPipMode(), no need to manually set
                        } else {
                            Toast.makeText(
                                context,
                                "Picture-in-Picture permission is not granted",
                                Toast.LENGTH_SHORT
                            ).show()
                        }
                    } else {
                        Toast.makeText(
                            context,
                            "Picture-in-Picture requires Android 8.0 (API 26) or higher",
                            Toast.LENGTH_SHORT
                        ).show()
                    }
                }
            ) {
                Text(text = "Enter Picture-in-Picture Mode")
            }

            ChannelNameInput(
                channelName = channelName,
                isJoined = isJoined,
                onJoinClick = {
                    channelName = it
                    keyboard?.hide()
                    permissionLauncher.launch(
                        arrayOf(
                            android.Manifest.permission.RECORD_AUDIO,
                            android.Manifest.permission.CAMERA
                        )
                    )
                },
                onLeaveClick = {
                    rtcEngine.stopPreview()
                    rtcEngine.leaveChannel()
                }
            )
        }
    }
}