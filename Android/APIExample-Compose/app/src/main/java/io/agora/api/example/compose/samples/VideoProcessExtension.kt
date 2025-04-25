package io.agora.api.example.compose.samples

import android.widget.Toast
import androidx.activity.compose.rememberLauncherForActivityResult
import androidx.activity.result.contract.ActivityResultContracts
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.lazy.LazyColumn
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
import androidx.compose.ui.platform.LocalSoftwareKeyboardController
import androidx.compose.ui.res.stringResource
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import io.agora.api.example.compose.BuildConfig
import io.agora.api.example.compose.R
import io.agora.api.example.compose.data.SettingPreferences
import io.agora.api.example.compose.ui.common.ChannelNameInput
import io.agora.api.example.compose.ui.common.RadioGroup
import io.agora.api.example.compose.ui.common.SliderRaw
import io.agora.api.example.compose.ui.common.SwitchRaw
import io.agora.api.example.compose.ui.common.TwoVideoView
import io.agora.api.example.compose.ui.common.TwoVideoViewType
import io.agora.api.example.compose.ui.common.VideoStatsInfo
import io.agora.api.example.compose.utils.FileUtils
import io.agora.api.example.compose.utils.TokenUtils
import io.agora.rtc2.ChannelMediaOptions
import io.agora.rtc2.Constants
import io.agora.rtc2.IRtcEngineEventHandler
import io.agora.rtc2.RtcEngine
import io.agora.rtc2.RtcEngineConfig
import io.agora.rtc2.video.BeautyOptions
import io.agora.rtc2.video.ColorEnhanceOptions
import io.agora.rtc2.video.LowLightEnhanceOptions
import io.agora.rtc2.video.SegmentationProperty
import io.agora.rtc2.video.VideoCanvas
import io.agora.rtc2.video.VideoDenoiserOptions
import io.agora.rtc2.video.VideoEncoderConfiguration
import io.agora.rtc2.video.VirtualBackgroundSource

@Composable
fun VideoProcessExtension() {
    val context = LocalContext.current
    val lifecycleOwner = LocalLifecycleOwner.current
    val keyboard = LocalSoftwareKeyboardController.current
    var isJoined by rememberSaveable { mutableStateOf(false) }
    var channelName by rememberSaveable { mutableStateOf("") }
    var localUid by rememberSaveable { mutableIntStateOf(0) }
    var remoteUid by rememberSaveable { mutableIntStateOf(0) }
    var localStats by remember { mutableStateOf(VideoStatsInfo()) }
    var remoteStats by remember { mutableStateOf(VideoStatsInfo()) }

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
            enableExtension("agora_video_filters_clear_vision", "clear_vision", true)
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
                mediaOptions.clientRoleType = Constants.CLIENT_ROLE_BROADCASTER
                TokenUtils.gen(channelName, 0) {
                    rtcEngine.joinChannel(it, channelName, 0, mediaOptions)
                }
            } else {
                // Permission is denied
                Toast.makeText(context, R.string.permission_denied, Toast.LENGTH_LONG).show()
            }
        }

    VideoProcessExtensionView(
        rtcEngine = rtcEngine,
        channelName = channelName,
        isJoined = isJoined,
        localUid = localUid,
        remoteUid = remoteUid,
        localStats = localStats,
        remoteStats = remoteStats,
        onJoinClick = {
            keyboard?.hide()
            channelName = it
            permissionLauncher.launch(
                arrayOf(
                    android.Manifest.permission.CAMERA,
                    android.Manifest.permission.RECORD_AUDIO
                )
            )
        },
        onLeaveClick = {
            rtcEngine.stopPreview()
            rtcEngine.leaveChannel()
        }
    )
}

@Composable
private fun VideoProcessExtensionView(
    rtcEngine: RtcEngine? = null,
    channelName: String,
    isJoined: Boolean,
    localUid: Int = 0,
    remoteUid: Int = 0,
    localStats: VideoStatsInfo = VideoStatsInfo(),
    remoteStats: VideoStatsInfo = VideoStatsInfo(),
    onJoinClick: (String) -> Unit,
    onLeaveClick: () -> Unit
) {
    val context = LocalContext.current

    Column {
        TwoVideoView(
            modifier = Modifier.height(200.dp),
            type = TwoVideoViewType.Row,
            localUid = localUid,
            remoteUid = remoteUid,
            localStats = localStats,
            remoteStats = remoteStats,
            localRender = { view, id, _ ->
                rtcEngine?.setupLocalVideo(VideoCanvas(view, VideoCanvas.RENDER_MODE_HIDDEN, id))
                rtcEngine?.startPreview()
            },
            remoteRender = { view, id, _ ->
                rtcEngine?.setupRemoteVideo(VideoCanvas(view, VideoCanvas.RENDER_MODE_HIDDEN, id))
            }
        )
        LazyColumn(modifier = Modifier.weight(1f)) {
            item {
                val beautyOptions = remember { BeautyOptions() }
                var isOpen by remember { mutableStateOf(false) }
                Column {
                    SwitchRaw(title = stringResource(id = R.string.beauty_face), checked = isOpen) {
                        isOpen = it
                        rtcEngine?.setBeautyEffectOptions(isOpen, beautyOptions)
                    }
                    Column(modifier = Modifier.padding(16.dp, 0.dp)) {
                        SliderRaw(title = stringResource(id = R.string.beauty_lightening), value = beautyOptions.lighteningLevel) {
                            beautyOptions.lighteningLevel = it
                            rtcEngine?.setBeautyEffectOptions(isOpen, beautyOptions)
                        }
                        SliderRaw(title = stringResource(id = R.string.beauty_redness), value = beautyOptions.rednessLevel) {
                            beautyOptions.rednessLevel = it
                            rtcEngine?.setBeautyEffectOptions(isOpen, beautyOptions)
                        }
                        SliderRaw(title = stringResource(id = R.string.beauty_sharpness), value = beautyOptions.sharpnessLevel) {
                            beautyOptions.sharpnessLevel = it
                            rtcEngine?.setBeautyEffectOptions(isOpen, beautyOptions)
                        }
                        SliderRaw(title = stringResource(id = R.string.beauty_smoothness), value = beautyOptions.smoothnessLevel) {
                            beautyOptions.smoothnessLevel = it
                            rtcEngine?.setBeautyEffectOptions(isOpen, beautyOptions)
                        }
                    }
                }
            }
            item {
                SwitchRaw(title = stringResource(id = R.string.low_light_enhance)) {
                    val options = LowLightEnhanceOptions()
                    options.lowlightEnhanceLevel =
                        LowLightEnhanceOptions.LOW_LIGHT_ENHANCE_LEVEL_FAST
                    options.lowlightEnhanceMode = LowLightEnhanceOptions.LOW_LIGHT_ENHANCE_AUTO
                    rtcEngine?.setLowlightEnhanceOptions(it, options)
                }
            }
            item {
                val colorEnhanceOptions = remember { ColorEnhanceOptions() }
                var isOpen by remember { mutableStateOf(false) }
                Column {
                    SwitchRaw(title = stringResource(id = R.string.color_enhance), checked = isOpen) {
                        isOpen = it
                        rtcEngine?.setColorEnhanceOptions(isOpen, colorEnhanceOptions)
                    }
                    Column(modifier = Modifier.padding(16.dp, 0.dp)) {
                        SliderRaw(title = stringResource(id = R.string.strength), value = colorEnhanceOptions.strengthLevel) {
                            colorEnhanceOptions.strengthLevel = it
                            rtcEngine?.setColorEnhanceOptions(isOpen, colorEnhanceOptions)
                        }
                        SliderRaw(
                            title = stringResource(id = R.string.skin_protect),
                            value = colorEnhanceOptions.skinProtectLevel
                        ) {
                            colorEnhanceOptions.skinProtectLevel = it
                            rtcEngine?.setColorEnhanceOptions(isOpen, colorEnhanceOptions)
                        }
                    }
                }
            }
            item {
                SwitchRaw(title = stringResource(id = R.string.video_denoiser)) {
                    val options = VideoDenoiserOptions()
                    options.denoiserLevel = VideoDenoiserOptions.VIDEO_DENOISER_LEVEL_HIGH_QUALITY
                    options.denoiserMode = VideoDenoiserOptions.VIDEO_DENOISER_AUTO
                    rtcEngine?.setVideoDenoiserOptions(it, options)
                }
            }
            item {
                var isOpen by remember { mutableStateOf(false) }
                val backgroundSource = remember {
                    VirtualBackgroundSource().apply {
                        color = 0x0000EE
                    }
                }
                val segproperty = remember { SegmentationProperty() }

                Column {
                    SwitchRaw(title = stringResource(id = R.string.virtual_background), checked = isOpen) {
                        isOpen = it
                        rtcEngine?.enableVirtualBackground(isOpen, backgroundSource, segproperty)
                    }
                    RadioGroup(
                        options = listOf(
                            stringResource(id = R.string.picture) to VirtualBackgroundSource.BACKGROUND_IMG,
                            stringResource(id = R.string.color) to VirtualBackgroundSource.BACKGROUND_COLOR,
                            stringResource(id = R.string.blur) to VirtualBackgroundSource.BACKGROUND_BLUR,
                            stringResource(id = R.string.video) to VirtualBackgroundSource.BACKGROUND_VIDEO,
                        ),
                        selectedValue = backgroundSource.backgroundSourceType
                    ) { _, option ->
                        backgroundSource.backgroundSourceType = option.second
                        when(option.second) {
                            VirtualBackgroundSource.BACKGROUND_IMG -> {
                                val imagePath = context.externalCacheDir?.path ?: ""
                                val imageName = "agora-logo.png"
                                FileUtils.copyFilesFromAssets(context, imageName, imagePath)
                                backgroundSource.source = "$imagePath/$imageName"
                            }
                            VirtualBackgroundSource.BACKGROUND_COLOR -> {
                                // backgroundSource.color = 0x0000EE
                            }
                            VirtualBackgroundSource.BACKGROUND_BLUR -> {
                                backgroundSource.blurDegree = VirtualBackgroundSource.BLUR_DEGREE_MEDIUM
                            }
                            VirtualBackgroundSource.BACKGROUND_VIDEO -> {
                                backgroundSource.source = "https://agora-adc-artifacts.s3.cn-north-1.amazonaws.com.cn/resources/sample.mp4"
                            }
                        }
                        rtcEngine?.enableVirtualBackground(isOpen, backgroundSource, segproperty)
                    }
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

@Preview
@Composable
private fun VideoProcessExtensionViewPreview() {
    VideoProcessExtensionView(channelName = "", isJoined = true, onJoinClick = {}) {

    }
}