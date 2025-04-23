package io.agora.api.example.compose.samples

import android.widget.Toast
import androidx.activity.compose.rememberLauncherForActivityResult
import androidx.activity.result.contract.ActivityResultContracts
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.WindowInsets
import androidx.compose.foundation.layout.fillMaxHeight
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.material3.Button
import androidx.compose.material3.Divider
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.ModalBottomSheet
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
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.platform.LocalLifecycleOwner
import androidx.compose.ui.platform.LocalSoftwareKeyboardController
import androidx.compose.ui.res.stringResource
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.Dp
import androidx.compose.ui.unit.dp
import io.agora.api.example.compose.BuildConfig
import io.agora.api.example.compose.R
import io.agora.api.example.compose.data.SettingPreferences
import io.agora.api.example.compose.ui.common.ChannelNameInput
import io.agora.api.example.compose.ui.common.DropdownMenuRaw
import io.agora.api.example.compose.ui.common.SwitchRaw
import io.agora.api.example.compose.ui.common.TwoVideoView
import io.agora.api.example.compose.ui.common.VideoStatsInfo
import io.agora.api.example.compose.utils.FileUtils
import io.agora.api.example.compose.utils.TokenUtils
import io.agora.rtc2.ChannelMediaOptions
import io.agora.rtc2.ClientRoleOptions
import io.agora.rtc2.Constants
import io.agora.rtc2.IRtcEngineEventHandler
import io.agora.rtc2.RtcEngine
import io.agora.rtc2.RtcEngineConfig
import io.agora.rtc2.SimulcastStreamConfig
import io.agora.rtc2.video.ImageTrackOptions
import io.agora.rtc2.video.VideoCanvas
import io.agora.rtc2.video.VideoEncoderConfiguration
import io.agora.rtc2.video.VideoEncoderConfiguration.AdvanceOptions
import io.agora.rtc2.video.WatermarkOptions

@Composable
fun LiveStreaming() {
    val context = LocalContext.current
    val lifecycleOwner = LocalLifecycleOwner.current
    val keyboard = LocalSoftwareKeyboardController.current
    var isJoined by rememberSaveable { mutableStateOf(false) }
    var localLarge by rememberSaveable { mutableStateOf(true) }
    var channelName by rememberSaveable { mutableStateOf("") }
    var localUid by rememberSaveable { mutableIntStateOf(0) }
    var remoteUid by rememberSaveable { mutableIntStateOf(0) }
    var localStats by remember { mutableStateOf(VideoStatsInfo()) }
    var remoteStats by remember { mutableStateOf(VideoStatsInfo()) }
    var clientRole by remember { mutableStateOf(Constants.CLIENT_ROLE_AUDIENCE) }

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

                override fun onClientRoleChanged(
                    oldRole: Int,
                    newRole: Int,
                    newRoleOptions: ClientRoleOptions?
                ) {
                    super.onClientRoleChanged(oldRole, newRole, newRoleOptions)
                    clientRole = newRole
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
            setDualStreamMode(
                Constants.SimulcastStreamMode.ENABLE_SIMULCAST_STREAM,
                SimulcastStreamConfig(
                    VideoEncoderConfiguration.VideoDimensions(
                        100, 100
                    ), 100, 15
                )
            )
        }
    }
    DisposableEffect(lifecycleOwner) {
        onDispose {
            rtcEngine.stopPreview()
            rtcEngine.leaveChannel()
            RtcEngine.destroy()
        }
    }
    val permissionLauncher =
        rememberLauncherForActivityResult(contract = ActivityResultContracts.RequestMultiplePermissions()) { grantedMap ->
            val allGranted = grantedMap.values.all { it }
            if (allGranted) {
                // Permission is granted
                Toast.makeText(context, R.string.permission_granted, Toast.LENGTH_LONG).show()
                val mediaOptions = ChannelMediaOptions()
                mediaOptions.channelProfile = Constants.CHANNEL_PROFILE_LIVE_BROADCASTING
                mediaOptions.clientRoleType = clientRole
                TokenUtils.gen(channelName, 0) {
                    rtcEngine.joinChannel(it, channelName, 0, mediaOptions)
                }
            } else {
                // Permission is denied
                Toast.makeText(context, R.string.permission_denied, Toast.LENGTH_LONG).show()
            }
        }

    LiveStreamingView(
        rtcEngine = rtcEngine,
        channelName = channelName,
        isJoined = isJoined,
        clientRole = clientRole,
        localUid = localUid,
        remoteUid = remoteUid,
        localStats = localStats,
        remoteStats = remoteStats,
        localLarge = localLarge,
        onSwitch = {
            localLarge = !localLarge
        },
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
            rtcEngine.stopPreview()

            rtcEngine.leaveChannel()
        }
    )
}


@OptIn(ExperimentalMaterial3Api::class)
@Composable
private fun LiveStreamingView(
    rtcEngine: RtcEngine? = null,
    channelName: String,
    isJoined: Boolean,
    clientRole: Int = Constants.CLIENT_ROLE_AUDIENCE,
    localUid: Int = 0,
    remoteUid: Int = 0,
    localLarge: Boolean = true,
    localStats: VideoStatsInfo = VideoStatsInfo(),
    remoteStats: VideoStatsInfo = VideoStatsInfo(),
    onSwitch: () -> Unit = {},
    onJoinClick: (String) -> Unit,
    onLeaveClick: () -> Unit
) {
    var openSettingSheet by rememberSaveable { mutableStateOf(false) }
    val settingsValues = remember { mutableStateMapOf<String, Any>() }

    Box {
        Column {
            Box(modifier = Modifier.weight(1.0f)) {
                TwoVideoView(
                    modifier = Modifier.fillMaxHeight(),
                    localUid = localUid,
                    remoteUid = remoteUid,
                    localStats = localStats,
                    remoteStats = remoteStats,
                    secondClickable = isJoined && remoteUid != 0,
                    localPrimary = localLarge || remoteUid == 0,
                    onSecondClick = onSwitch,
                    localRender = { view, uid, isFirstSetup ->
                        rtcEngine?.setupLocalVideo(
                            VideoCanvas(
                                view,
                                Constants.RENDER_MODE_HIDDEN,
                                uid
                            )
                        )
                        if (isFirstSetup) {
                            rtcEngine?.startPreview()
                        }

                    },
                    remoteRender = { view, uid, _ ->
                        rtcEngine?.setupRemoteVideo(
                            VideoCanvas(
                                view,
                                Constants.RENDER_MODE_HIDDEN,
                                uid
                            )
                        )
                    }
                )

                Column(
                    modifier = Modifier
                        .align(Alignment.BottomStart)
                        .padding(16.dp)
                ) {
                    Row {
                        Button(
                            enabled = isJoined,
                            onClick = {
                                if (clientRole == Constants.CLIENT_ROLE_AUDIENCE) {
                                    rtcEngine?.setClientRole(Constants.CLIENT_ROLE_BROADCASTER)
                                } else {
                                    rtcEngine?.setClientRole(Constants.CLIENT_ROLE_AUDIENCE)
                                }
                            }
                        ) {
                            Text(
                                if (clientRole == Constants.CLIENT_ROLE_AUDIENCE) stringResource(R.string.start_co_hosting)
                                else stringResource(R.string.stop_co_hosting)
                            )
                        }
                    }
                }
            }
            Row(
                modifier = Modifier
                    .fillMaxWidth()
                    .padding(end = 16.dp),
                verticalAlignment = Alignment.CenterVertically
            ) {
                ChannelNameInput(
                    modifier = Modifier.weight(1.0f),
                    channelName = channelName,
                    isJoined = isJoined,
                    onJoinClick = onJoinClick,
                    onLeaveClick = onLeaveClick
                )
                Button(
                    modifier = Modifier.size(120.dp, Dp.Unspecified),
                    onClick = {
                        openSettingSheet = true
                    }
                ) {
                    Text(stringResource(R.string.settings))
                }
            }
        }
        if (openSettingSheet) {
            ModalBottomSheet(
                onDismissRequest = {
                    openSettingSheet = false
                },
                windowInsets = WindowInsets(0)
            ) {
                LiveStreamingSettingView(
                    rtcEngine = rtcEngine,
                    role = clientRole,
                    remoteUid = remoteUid,
                    values = settingsValues,
                    onValueChanged = { key, value ->
                        settingsValues[key] = value
                    }
                )
            }
        }
    }


}

@Preview
@Composable
private fun LiveStreamingViewPreview() {
    Box {
        LiveStreamingView(
            channelName = "Channel Name",
            isJoined = false,
            onJoinClick = {},
            onLeaveClick = {},
        )

//        LiveStreamingSettingView(
//            modifier = Modifier
//                .align(Alignment.BottomCenter)
//                .background(Color.White)
//        )
    }
}


@Composable
private fun LiveStreamingSettingView(
    modifier: Modifier = Modifier,
    rtcEngine: RtcEngine? = null,
    role: Int = Constants.CLIENT_ROLE_AUDIENCE,
    remoteUid: Int = 0,
    values: Map<String, Any> = emptyMap(),
    onValueChanged: (String, Any) -> Unit = { _, _ -> }
) {
    val context = LocalContext.current

    Column(
        modifier = modifier,
        horizontalAlignment = Alignment.CenterHorizontally
    ) {
        if (role == Constants.CLIENT_ROLE_AUDIENCE) {
            SwitchRaw(
                title = stringResource(id = R.string.open_low_latency_live),
                checked = values["living"] as? Boolean ?: false
            ) { enable ->
                onValueChanged("living", enable)
                rtcEngine?.setClientRole(role, ClientRoleOptions().apply {
                    audienceLatencyLevel =
                        if (enable) Constants.AUDIENCE_LATENCY_LEVEL_LOW_LATENCY else Constants.AUDIENCE_LATENCY_LEVEL_ULTRA_LOW_LATENCY
                })
            }
        }

        if (role == Constants.CLIENT_ROLE_BROADCASTER) {
            Divider(modifier = Modifier.padding(horizontal = 16.dp))
            SwitchRaw(
                title = stringResource(id = R.string.watermark),
                checked = values["watermark"] as? Boolean ?: false
            ) { enable ->
                onValueChanged("watermark", enable)
                if (enable) {
                    rtcEngine?.addVideoWatermark(
                        "/assets/agora-logo.png",
                        WatermarkOptions().apply {
                            positionInPortraitMode = WatermarkOptions.Rectangle(50, 50, 100, 100)
                            positionInLandscapeMode = WatermarkOptions.Rectangle(50, 50, 100, 100)
                            visibleInPreview = true
                        })
                } else {
                    rtcEngine?.clearVideoWatermarks()
                }
            }
        }


        Divider(modifier = Modifier.padding(horizontal = 16.dp))
        SwitchRaw(
            title = stringResource(id = R.string.low_stream),
            checked = values["stream"] as? Boolean ?: false
        ) { enable ->
            onValueChanged("stream", enable)
            rtcEngine?.setRemoteDefaultVideoStreamType(if (enable) Constants.VIDEO_STREAM_LOW else Constants.VIDEO_STREAM_HIGH)
            if (remoteUid != 0) {
                rtcEngine?.setRemoteVideoStreamType(
                    remoteUid,
                    if (enable) Constants.VIDEO_STREAM_LOW else Constants.VIDEO_STREAM_HIGH
                )
            }
        }


        Divider(modifier = Modifier.padding(horizontal = 16.dp))
        DropdownMenuRaw(
            title = stringResource(id = R.string.encoder_type),
            options = listOf(
                stringResource(id = R.string.encoder_auto) to VideoEncoderConfiguration.ENCODING_PREFERENCE.PREFER_AUTO,
                stringResource(id = R.string.encoder_hardware) to VideoEncoderConfiguration.ENCODING_PREFERENCE.PREFER_HARDWARE,
                stringResource(id = R.string.encoder_software) to VideoEncoderConfiguration.ENCODING_PREFERENCE.PREFER_SOFTWARE,
            ),
            selected = values["encoder"] as? Int ?: 0
        ) { index, option ->
            onValueChanged("encoder", index)

            val encodingOption = AdvanceOptions()
            encodingOption.encodingPreference = option.second
            rtcEngine?.setVideoEncoderConfiguration(VideoEncoderConfiguration().apply {
                advanceOptions = encodingOption
            })
        }

        Divider(modifier = Modifier.padding(horizontal = 16.dp))
        SwitchRaw(
            title = stringResource(id = R.string.b_frame),
            checked = values["BFrame"] as? Boolean ?: false
        ) { enable ->
            onValueChanged("BFrame", enable)

            val encodingOption = AdvanceOptions()
            encodingOption.compressionPreference =
                if (enable)
                    VideoEncoderConfiguration.COMPRESSION_PREFERENCE.PREFER_QUALITY
                else
                    VideoEncoderConfiguration.COMPRESSION_PREFERENCE.PREFER_LOW_LATENCY
            rtcEngine?.setVideoEncoderConfiguration(VideoEncoderConfiguration().apply {
                advanceOptions = encodingOption
            })
        }


        Divider(modifier = Modifier.padding(horizontal = 16.dp))
        SwitchRaw(
            title = stringResource(id = R.string.video_image),
            checked = values["padding"] as? Boolean ?: false
        ) { enable ->
            onValueChanged("padding", enable)
            val path: String = context.externalCacheDir?.path ?: ""
            val fileName = "bg_blue.png"
            FileUtils.copyFilesFromAssets(context, fileName, path)
            rtcEngine?.enableVideoImageSource(
                enable, ImageTrackOptions(
                    "$path/$fileName",
                    15
                )
            )
        }
    }
}

