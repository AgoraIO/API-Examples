package io.agora.api.example.compose.samples

import android.util.Log
import android.widget.Toast
import androidx.activity.compose.rememberLauncherForActivityResult
import androidx.activity.result.contract.ActivityResultContracts
import androidx.annotation.DrawableRes
import androidx.compose.foundation.Image
import androidx.compose.foundation.background
import androidx.compose.foundation.clickable
import androidx.compose.foundation.gestures.detectDragGestures
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxHeight
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.offset
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.width
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.ModalBottomSheet
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.DisposableEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableFloatStateOf
import androidx.compose.runtime.mutableIntStateOf
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.saveable.rememberSaveable
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.alpha
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.input.pointer.pointerInput
import androidx.compose.ui.layout.onGloballyPositioned
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.platform.LocalDensity
import androidx.compose.ui.platform.LocalLifecycleOwner
import androidx.compose.ui.platform.LocalSoftwareKeyboardController
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.res.stringResource
import androidx.compose.ui.unit.Dp
import androidx.compose.ui.unit.IntOffset
import androidx.compose.ui.unit.dp
import io.agora.api.example.compose.BuildConfig
import io.agora.api.example.compose.R
import io.agora.api.example.compose.data.SettingPreferences
import io.agora.api.example.compose.ui.common.ChannelNameInput
import io.agora.api.example.compose.ui.common.SliderRaw
import io.agora.api.example.compose.ui.common.SwitchRaw
import io.agora.api.example.compose.utils.TokenUtils
import io.agora.mediaplayer.data.MediaPlayerSource
import io.agora.rtc2.ChannelMediaOptions
import io.agora.rtc2.Constants
import io.agora.rtc2.IRtcEngineEventHandler
import io.agora.rtc2.RtcEngine
import io.agora.rtc2.RtcEngineConfig
import io.agora.rtc2.SpatialAudioParams
import io.agora.rtc2.video.VideoEncoderConfiguration
import io.agora.spatialaudio.ILocalSpatialAudioEngine
import io.agora.spatialaudio.LocalSpatialAudioConfig
import io.agora.spatialaudio.RemoteVoicePositionInfo
import kotlin.math.roundToInt


private const val AXIS_MAX_DISTANCE = 10.0f

@Composable
fun SpatialSound() {
    val context = LocalContext.current
    val lifecycleOwner = LocalLifecycleOwner.current
    val keyboard = LocalSoftwareKeyboardController.current
    var isJoined by rememberSaveable { mutableStateOf(false) }
    var channelName by rememberSaveable { mutableStateOf("") }
    var localUid by rememberSaveable { mutableIntStateOf(0) }
    var remoteUidList by rememberSaveable { mutableStateOf(listOf<Int>()) }
    val settingCache = remember { mutableMapOf<Int, SettingInfo>() }

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
                    remoteUidList = emptyList()
                }

                override fun onUserJoined(uid: Int, elapsed: Int) {
                    super.onUserJoined(uid, elapsed)
                    remoteUidList += uid
                    settingCache[uid] = SettingInfo(
                        mute = false,
                        voiceBlur = false,
                        airborneSimulation = false,
                        attenuation = 0.5f
                    )
                }

                override fun onUserOffline(uid: Int, reason: Int) {
                    super.onUserOffline(uid, reason)
                    remoteUidList -= uid
                    settingCache.remove(uid)
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
            enableAudio()
        }
    }

    val localSpatial = remember {
        ILocalSpatialAudioEngine.create().apply {
            val localSpatialAudioConfig = LocalSpatialAudioConfig()
            localSpatialAudioConfig.mRtcEngine = rtcEngine
            initialize(localSpatialAudioConfig)

            setMaxAudioRecvCount(2)
            setAudioRecvRange(AXIS_MAX_DISTANCE)
            setDistanceUnit(1f)
        }
    }

    val playerLeft = remember {
        rtcEngine.createMediaPlayer().apply {
            setLoopCount(-1)
            settingCache[mediaPlayerId] = SettingInfo(
                mute = true, voiceBlur = false, airborneSimulation = false, attenuation = 0.5f
            )
        }
    }
    val playerRight = remember {
        rtcEngine.createMediaPlayer().apply {
            setLoopCount(-1)
            settingCache[mediaPlayerId] = SettingInfo(
                mute = true, voiceBlur = false, airborneSimulation = false, attenuation = 0.5f
            )
        }
    }

    DisposableEffect(lifecycleOwner) {
        onDispose {
            playerLeft.destroy()
            playerRight.destroy()
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
                mediaOptions.autoSubscribeAudio = true
                mediaOptions.autoSubscribeVideo = false
                mediaOptions.publishMicrophoneTrack = true
                mediaOptions.publishCameraTrack = false
                TokenUtils.gen(channelName, 0) {
                    rtcEngine.joinChannel(it, channelName, 0, mediaOptions)
                }
            } else {
                // Permission is denied
                Toast.makeText(context, R.string.permission_denied, Toast.LENGTH_LONG).show()
            }
        }

    SpatialSoundView(channelName = channelName,
        isJoined = isJoined,
        remoteUidList = remoteUidList,
        onJoinClick = {
            channelName = it
            keyboard?.hide()
            settingCache[playerLeft.mediaPlayerId] =
                settingCache[playerLeft.mediaPlayerId]?.copy(mute = false)!!
            settingCache[playerRight.mediaPlayerId] =
                settingCache[playerRight.mediaPlayerId]?.copy(mute = false)!!
            playerLeft.openWithMediaSource(MediaPlayerSource().apply {
                url = "https://webdemo.agora.io/audiomixing.mp3"
                isAutoPlay = true
            })
            playerRight.openWithMediaSource(MediaPlayerSource().apply {
                url = "https://webdemo.agora.io/dang.mp3"
                isAutoPlay = true
            })

            permissionLauncher.launch(
                arrayOf(
                    android.Manifest.permission.RECORD_AUDIO,
                )
            )
        },
        onLeaveClick = {
            playerLeft.stop()
            playerRight.stop()
            rtcEngine.leaveChannel()
        },
        onPlayerLeftPositionUpdated = { x, y ->
            localSpatial.updatePlayerPositionInfo(playerLeft.mediaPlayerId,
                RemoteVoicePositionInfo().apply {
                    forward = floatArrayOf(1.0f, 0f, 0f)
                    position = floatArrayOf(x * AXIS_MAX_DISTANCE, y * AXIS_MAX_DISTANCE, 0f)
                })
        },
        onPlayerRightPositionUpdated = { x, y ->
            localSpatial.updatePlayerPositionInfo(playerRight.mediaPlayerId,
                RemoteVoicePositionInfo().apply {
                    forward = floatArrayOf(1.0f, 0f, 0f)
                    position = floatArrayOf(x * AXIS_MAX_DISTANCE, y * AXIS_MAX_DISTANCE, 0f)
                })
        },
        onRemotePositionUpdated = { uid, x, y ->
            localSpatial.updateRemotePosition(uid, RemoteVoicePositionInfo().apply {
                forward = floatArrayOf(1.0f, 0f, 0f)
                position = floatArrayOf(x * AXIS_MAX_DISTANCE, y * AXIS_MAX_DISTANCE, 0f)
            })
        },
        onLocalPositionUpdated = { x, y ->
            localSpatial.updateSelfPosition(
                floatArrayOf(x * AXIS_MAX_DISTANCE, y * AXIS_MAX_DISTANCE, 0f),
                floatArrayOf(1.0f, 0f, 0f),
                floatArrayOf(0f, 1.0f, 0f),
                floatArrayOf(0f, 0f, 1.0f)
            )
        },
        onMute = { settingType, enable, uid ->
            when (settingType) {
                SETTING_TYPE_PLAYER_LEFT -> {
                    settingCache[playerLeft.mediaPlayerId] =
                        settingCache[playerLeft.mediaPlayerId]?.copy(mute = enable)!!
                    if (enable) {
                        playerLeft.pause()
                    } else {
                        playerLeft.resume()
                    }
                }

                SETTING_TYPE_PLAYER_RIGHT -> {
                    settingCache[playerRight.mediaPlayerId] =
                        settingCache[playerRight.mediaPlayerId]?.copy(mute = enable)!!
                    if (enable) {
                        playerRight.pause()
                    } else {
                        playerRight.resume()
                    }
                }

                SETTING_TYPE_REMOTE -> {
                    settingCache[uid]?.copy(mute = enable)?.let {
                        settingCache[uid] = it
                    }
                    localSpatial.muteRemoteAudioStream(uid, enable)
                }
            }
        },
        onVoiceBlur = { settingType, enable, uid ->
            when (settingType) {
                SETTING_TYPE_PLAYER_LEFT -> {
                    settingCache[playerLeft.mediaPlayerId] =
                        settingCache[playerLeft.mediaPlayerId]?.copy(voiceBlur = enable)!!
                    playerLeft.setSpatialAudioParams(SpatialAudioParams().apply {
                        enable_blur = enable
                    })
                }

                SETTING_TYPE_PLAYER_RIGHT -> {
                    settingCache[playerRight.mediaPlayerId] =
                        settingCache[playerRight.mediaPlayerId]?.copy(voiceBlur = enable)!!
                    playerRight.setSpatialAudioParams(SpatialAudioParams().apply {
                        enable_blur = enable
                    })
                }

                SETTING_TYPE_REMOTE -> {
                    settingCache[uid]?.copy(voiceBlur = enable)?.let {
                        settingCache[uid] = it
                    }
                    rtcEngine.setRemoteUserSpatialAudioParams(uid, SpatialAudioParams().apply {
                        enable_blur = enable
                    })
                }
            }
        },
        onAirborneSimulation = { settingType, enable, uid ->
            when (settingType) {
                SETTING_TYPE_PLAYER_LEFT -> {
                    settingCache[playerLeft.mediaPlayerId] =
                        settingCache[playerLeft.mediaPlayerId]?.copy(airborneSimulation = enable)!!
                    playerLeft.setSpatialAudioParams(SpatialAudioParams().apply {
                        enable_air_absorb = enable
                    })
                }

                SETTING_TYPE_PLAYER_RIGHT -> {
                    settingCache[playerRight.mediaPlayerId] =
                        settingCache[playerRight.mediaPlayerId]?.copy(airborneSimulation = enable)!!
                    playerRight.setSpatialAudioParams(SpatialAudioParams().apply {
                        enable_air_absorb = enable
                    })
                }

                SETTING_TYPE_REMOTE -> {
                    settingCache[uid]?.copy(airborneSimulation = enable)?.let {
                        settingCache[uid] = it
                    }
                    rtcEngine.setRemoteUserSpatialAudioParams(uid, SpatialAudioParams().apply {
                        enable_air_absorb = enable
                    })
                }
            }
        },
        onAttenuation = { settingType, value, uid ->
            when (settingType) {
                SETTING_TYPE_PLAYER_LEFT -> {
                    settingCache[playerLeft.mediaPlayerId] =
                        settingCache[playerLeft.mediaPlayerId]?.copy(attenuation = value)!!
                    localSpatial.setRemoteAudioAttenuation(
                        playerLeft.mediaPlayerId, value.toDouble(), false
                    )
                }

                SETTING_TYPE_PLAYER_RIGHT -> {
                    settingCache[playerRight.mediaPlayerId] =
                        settingCache[playerRight.mediaPlayerId]?.copy(attenuation = value)!!
                    localSpatial.setRemoteAudioAttenuation(
                        playerRight.mediaPlayerId, value.toDouble(), false
                    )
                }

                SETTING_TYPE_REMOTE -> {
                    settingCache[uid]?.copy(attenuation = value)?.let {
                        settingCache[uid] = it
                    }
                    localSpatial.setRemoteAudioAttenuation(uid, value.toDouble(), false)
                }
            }
        })
}

@OptIn(ExperimentalMaterial3Api::class)
@Composable
private fun SpatialSoundView(
    channelName: String,
    isJoined: Boolean,
    remoteUidList: List<Int> = emptyList(),
    settingCache: Map<Int, SettingInfo> = emptyMap(),
    onJoinClick: (channelName: String) -> Unit = { _ -> },
    onLeaveClick: () -> Unit = {},
    onMute: (settingType: Int, enable: Boolean, uid: Int) -> Unit = { _, _, _ -> },
    onVoiceBlur: (settingType: Int, enable: Boolean, uid: Int) -> Unit = { _, _, _ -> },
    onAirborneSimulation: (settingType: Int, enable: Boolean, uid: Int) -> Unit = { _, _, _ -> },
    onAttenuation: (settingType: Int, value: Float, uid: Int) -> Unit = { _, _, _ -> },
    onPlayerLeftPositionUpdated: (x: Float, y: Float) -> Unit = { _, _ -> },
    onPlayerRightPositionUpdated: (x: Float, y: Float) -> Unit = { _, _ -> },
    onRemotePositionUpdated: (uid: Int, x: Float, y: Float) -> Unit = { _, _, _ -> },
    onLocalPositionUpdated: (x: Float, y: Float) -> Unit = { _, _ -> },
) {
    var showSetting by rememberSaveable { mutableStateOf(false) }
    var settingUid by rememberSaveable { mutableIntStateOf(0) }
    var settingType by rememberSaveable { mutableIntStateOf(SETTING_TYPE_PLAYER_LEFT) }


    Column {
        Box(modifier = Modifier.weight(1.0f)) {
            DraggableIcon(iconId = R.drawable.ic_speaker,
                iconSize = 60.dp,
                axisX = -0.5f,
                axisY = 0.5f,
                draggable = false,
                clickable = isJoined,
                onClick = {
                    settingUid = 0
                    settingType = SETTING_TYPE_PLAYER_LEFT
                    showSetting = true
                }) { x, y ->
                Log.i("SpatialSoundView", "Speaker1 >> x=$x, y=$y")
                onPlayerLeftPositionUpdated(x, y)
            }
            DraggableIcon(iconId = R.drawable.ic_speaker,
                iconSize = 60.dp,
                axisX = 0.5f,
                axisY = 0.5f,
                draggable = false,
                clickable = isJoined,
                onClick = {
                    settingUid = 1
                    settingType = SETTING_TYPE_PLAYER_RIGHT
                    showSetting = true
                }) { x, y ->
                Log.i("SpatialSoundView", "Speaker2 >> x=$x, y=$y")
                onPlayerRightPositionUpdated(x, y)
            }

            Text(
                modifier = Modifier
                    .align(Alignment.TopCenter)
                    .padding(top = 50.dp),
                text = stringResource(id = R.string.spatial_sound_experience_tip),
                style = MaterialTheme.typography.titleMedium
            )
            DraggableIcon(
                iconId = R.drawable.ic_local,
                iconSize = 50.dp,
                axisX = 0.0f,
                axisY = 0.0f,
                showGuides = false,
                clickable = false,
                draggable = isJoined
            ) { x, y ->
                Log.i("SpatialSoundView", "Local >> x=$x, y=$y")
                onLocalPositionUpdated(x, y)
            }

            remoteUidList.getOrNull(0)?.let {
                DraggableIcon(iconId = R.drawable.ic_remote,
                    iconSize = 60.dp,
                    axisX = -0.5f,
                    axisY = -0.5f,
                    draggable = false,
                    clickable = true,
                    onClick = {
                        settingUid = it
                        settingType = SETTING_TYPE_REMOTE
                        showSetting = true
                    }) { x, y ->
                    Log.i("SpatialSoundView", "Remote1 >> x=$x, y=$y")
                    onRemotePositionUpdated(it, x, y)
                }
            }

            remoteUidList.getOrNull(1)?.let {
                DraggableIcon(iconId = R.drawable.ic_remote,
                    iconSize = 60.dp,
                    axisX = 0.5f,
                    axisY = -0.5f,
                    draggable = false,
                    clickable = true,
                    onClick = {
                        settingUid = it
                        settingType = SETTING_TYPE_REMOTE
                        showSetting = true
                    }) { x, y ->
                    Log.i("SpatialSoundView", "Remote2 >> x=$x, y=$y")
                    onRemotePositionUpdated(it, x, y)
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

    if (showSetting) {
        ModalBottomSheet(
            onDismissRequest = {
                showSetting = false
            }
        ) {
            Column(
                modifier = Modifier
                    .fillMaxWidth()
                    .padding(bottom = 16.dp)
            ) {
                SwitchRaw(title = stringResource(id = R.string.mute_audio), settingCache[settingUid]?.mute ?: false) {
                    onMute(settingType, it, settingUid)
                }
                SwitchRaw(title = "VoiceBlur", settingCache[settingUid]?.voiceBlur ?: false) {
                    onVoiceBlur(settingType, it, settingUid)
                }
                SwitchRaw(
                    title = "Airborne Simulation",
                    settingCache[settingUid]?.airborneSimulation ?: false
                ) {
                    onAirborneSimulation(settingType, it, settingUid)
                }
                SliderRaw(
                    title = "Attenuation",
                    value = settingCache[settingUid]?.attenuation ?: 0.5f
                ) {
                    onAttenuation(settingType, it, settingUid)
                }
            }
        }
    }

}

private const val SETTING_TYPE_PLAYER_LEFT = 0
private const val SETTING_TYPE_PLAYER_RIGHT = 1
private const val SETTING_TYPE_REMOTE = 2

private data class SettingInfo(
    val mute: Boolean,
    val voiceBlur: Boolean,
    val airborneSimulation: Boolean,
    val attenuation: Float
)

@Composable
private fun DraggableIcon(
    @DrawableRes iconId: Int,
    iconSize: Dp,
    axisX: Float,
    axisY: Float,
    draggable: Boolean = true,
    showGuides: Boolean = false,
    clickable: Boolean = true,
    onClick: () -> Unit = { },
    onAxisPositionChanged: (axisX: Float, axisY: Float) -> Unit = { _, _ -> }
) {
    var width by remember { mutableIntStateOf(0) }
    var height by remember { mutableIntStateOf(0) }
    var offsetX by remember { mutableFloatStateOf(0.0f) }
    var offsetY by remember { mutableFloatStateOf(0.0f) }
    val density = LocalDensity.current.density

    Box(modifier = Modifier
        .fillMaxSize()
        .onGloballyPositioned { coordinates ->
            width = coordinates.size.width
            height = coordinates.size.height
            offsetX = width * 1.0f / 2 - (iconSize.value * density) / 2 + width * 1.0f / 2 * axisX
            offsetY = height * 1.0f / 2 - (iconSize.value * density) / 2 - height * 1.0f / 2 * axisY
            onAxisPositionChanged(
                (offsetX - (width * 1.0f / 2 - (iconSize.value * density) / 2)) / (width * 1.0f / 2),
                -1 * (offsetY - (height * 1.0f / 2 - (iconSize.value * density) / 2)) / (height * 1.0f / 2)
            )
        }) {
        if (showGuides) {
            val row = 4
            val col = 4
            Column(modifier = Modifier.fillMaxSize()) {
                for (i in 0 until row) {
                    if (i == 0) {
                        Box(
                            modifier = Modifier
                                .height(1.dp)
                                .fillMaxWidth()
                                .background(Color.Red)
                                .alpha(0.5f)
                        )
                    }
                    Spacer(modifier = Modifier.weight(1.0f))
                    Box(
                        modifier = Modifier
                            .height(1.dp)
                            .fillMaxWidth()
                            .background(Color.Red)
                            .alpha(0.5f)
                    )
                }
            }

            Row(modifier = Modifier.fillMaxSize()) {
                for (i in 0 until col) {
                    if (i == 0) {
                        Box(
                            modifier = Modifier
                                .width(1.dp)
                                .fillMaxHeight()
                                .background(Color.Red)
                                .alpha(0.5f)
                        )
                    }
                    Spacer(modifier = Modifier.weight(1.0f))
                    Box(
                        modifier = Modifier
                            .width(1.dp)
                            .fillMaxHeight()
                            .background(Color.Red)
                            .alpha(0.5f)
                    )
                }
            }
        }

        var modifier = Modifier
            .offset { IntOffset(offsetX.roundToInt(), offsetY.roundToInt()) }
            .size(iconSize)
        if (draggable) {
            modifier = modifier.pointerInput(Unit) {
                detectDragGestures { change, dragAmount ->
                    change.consume()

                    var x = offsetX + dragAmount.x
                    var y = offsetY + dragAmount.y
                    if (x < 0) {
                        x = 0f
                    }
                    if (x > (width - iconSize.toPx())) {
                        x = width - iconSize.toPx()
                    }
                    if (y < 0) {
                        y = 0f
                    }
                    if (y > (height - iconSize.toPx())) {
                        y = height - iconSize.toPx()
                    }
                    offsetX = x
                    offsetY = y

                    onAxisPositionChanged(
                        (offsetX - (width * 1.0f / 2 - (iconSize.value * density) / 2)) / (width * 1.0f / 2),
                        -1 * (offsetY - (height * 1.0f / 2 - (iconSize.value * density) / 2)) / (height * 1.0f / 2)
                    )
                }
            }
        }

        if (clickable) {
            modifier = modifier.clickable {
                onClick()
            }
        }

        Image(
            modifier = modifier, painter = painterResource(id = iconId), contentDescription = null
        )
    }
}
