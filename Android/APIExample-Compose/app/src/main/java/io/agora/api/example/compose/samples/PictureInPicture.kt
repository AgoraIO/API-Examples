package io.agora.api.example.compose.samples

import android.app.AppOpsManager
import android.app.PictureInPictureParams
import android.content.Intent
import android.graphics.RectF
import android.os.Build
import android.os.Bundle
import android.os.Process
import android.util.Rational
import android.widget.Toast
import androidx.activity.ComponentActivity
import androidx.activity.compose.rememberLauncherForActivityResult
import androidx.activity.compose.setContent
import androidx.activity.result.contract.ActivityResultContracts
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.WindowInsets
import androidx.compose.foundation.layout.consumeWindowInsets
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.safeDrawing
import androidx.compose.material3.Button
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableIntStateOf
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.saveable.rememberSaveable
import androidx.compose.runtime.setValue
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.toAndroidRectF
import androidx.compose.ui.layout.boundsInWindow
import androidx.compose.ui.layout.onGloballyPositioned
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.platform.LocalLifecycleOwner
import androidx.compose.ui.platform.LocalSoftwareKeyboardController
import androidx.compose.ui.res.stringResource
import androidx.compose.ui.unit.dp
import androidx.lifecycle.DefaultLifecycleObserver
import androidx.lifecycle.Lifecycle
import androidx.lifecycle.LifecycleOwner
import io.agora.api.example.compose.BuildConfig
import io.agora.api.example.compose.R
import io.agora.api.example.compose.data.SettingPreferences
import io.agora.api.example.compose.ui.common.APIExampleScaffold
import io.agora.api.example.compose.ui.common.ChannelNameInput
import io.agora.api.example.compose.ui.common.TwoVideoView
import io.agora.api.example.compose.ui.common.TwoVideoViewType
import io.agora.api.example.compose.ui.common.VideoStatsInfo
import io.agora.api.example.compose.ui.theme.APIExampleComposeTheme
import io.agora.api.example.compose.utils.TokenUtils
import io.agora.rtc2.ChannelMediaOptions
import io.agora.rtc2.Constants
import io.agora.rtc2.IRtcEngineEventHandler
import io.agora.rtc2.RtcEngine
import io.agora.rtc2.RtcEngineConfig
import io.agora.rtc2.video.VideoCanvas
import io.agora.rtc2.video.VideoEncoderConfiguration


@Composable
fun PictureInPictureEntrance(back: () -> Unit) {
    val context = LocalContext.current
    val intent = Intent(context, PictureInPictureActivity::class.java)
    context.startActivity(intent)
    back()
}

@Composable
private fun PictureInPicture() {
    val context = LocalContext.current as ComponentActivity
    var isPipOn by rememberSaveable { mutableStateOf(false) }
    val lifecycleOwner = LocalLifecycleOwner.current
    val keyboard = LocalSoftwareKeyboardController.current
    var isJoined by rememberSaveable { mutableStateOf(false) }
    var channelName by rememberSaveable { mutableStateOf("") }
    var localUid by rememberSaveable { mutableIntStateOf(0) }
    var remoteUid by rememberSaveable { mutableIntStateOf(0) }
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
        context.addOnPictureInPictureModeChangedListener { info ->
            isPipOn = info.isInPictureInPictureMode
            if (lifecycleOwner.lifecycle.currentState < Lifecycle.State.STARTED) {
                context.finish()
            }
        }
        lifecycleOwner.lifecycle.addObserver(object: DefaultLifecycleObserver {
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
                .onGloballyPositioned {
                    videoViewBound.set(
                        it
                            .boundsInWindow()
                            .toAndroidRectF()
                    )
                },
            type = TwoVideoViewType.Row,
            localUid = localUid,
            remoteUid = remoteUid,
            localStats = localStats,
            remoteStats = remoteStats,
            localRender = { view, id, _ ->
                rtcEngine.setupLocalVideo(
                    VideoCanvas(
                        view,
                        Constants.RENDER_MODE_HIDDEN,
                        id
                    )
                )
                rtcEngine.startPreview()
            },
            remoteRender = { view, id, _ ->
                rtcEngine.setupRemoteVideo(
                    VideoCanvas(
                        view,
                        Constants.RENDER_MODE_HIDDEN,
                        id
                    )
                )
            })
    }

    if (isPipOn) {
        videoView()
    } else {
        APIExampleComposeTheme {
            APIExampleScaffold(
                topBarTitle = stringResource(id = R.string.example_pictureinpicture),
                showSettingIcon = false,
                showBackNavigationIcon = true,
                onBackClick = { context.finish() },
            ) { paddingValues ->
                Column(
                    modifier = Modifier
                        .fillMaxSize()
                        .consumeWindowInsets(WindowInsets.safeDrawing)
                        .padding(paddingValues)
                ) {
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
                                            .build()
                                    )
                                    val homeIntent = Intent(Intent.ACTION_MAIN)
                                    homeIntent.addCategory(Intent.CATEGORY_HOME)
                                    context.startActivity(homeIntent)
                                    isPipOn = true
                                }
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
    }

}


class PictureInPictureActivity : ComponentActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContent {
            PictureInPicture()
        }
    }

}