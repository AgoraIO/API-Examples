package io.agora.api.example.compose.samples

import android.os.Handler
import android.os.Looper
import android.widget.Toast
import androidx.activity.compose.rememberLauncherForActivityResult
import androidx.activity.result.contract.ActivityResultContracts
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.material3.AlertDialog
import androidx.compose.material3.Button
import androidx.compose.material3.Text
import androidx.compose.material3.TextButton
import androidx.compose.runtime.Composable
import androidx.compose.runtime.DisposableEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableIntStateOf
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
import io.agora.api.example.compose.BuildConfig
import io.agora.api.example.compose.R
import io.agora.api.example.compose.data.SettingPreferences
import io.agora.api.example.compose.ui.common.ChannelNameInput
import io.agora.api.example.compose.ui.common.SwitchRaw
import io.agora.api.example.compose.ui.common.VideoGrid
import io.agora.api.example.compose.utils.TokenUtils
import io.agora.rtc2.ChannelMediaOptions
import io.agora.rtc2.Constants
import io.agora.rtc2.IRtcEngineEventHandler
import io.agora.rtc2.LeaveChannelOptions
import io.agora.rtc2.RtcConnection
import io.agora.rtc2.RtcEngine
import io.agora.rtc2.RtcEngineConfig
import io.agora.rtc2.RtcEngineEx
import io.agora.rtc2.video.VideoCanvas
import io.agora.rtc2.video.VideoEncoderConfiguration
import java.io.File
import kotlin.random.Random

@Composable
fun JoinMultiChannel() {
    val context = LocalContext.current
    val lifecycleOwner = LocalLifecycleOwner.current
    val keyboard = LocalSoftwareKeyboardController.current
    var isJoined by rememberSaveable { mutableStateOf(false) }
    var channelName by rememberSaveable { mutableStateOf("") }
    var localUid by rememberSaveable { mutableIntStateOf(0) }
    var videoIdList by rememberSaveable { mutableStateOf(listOf<Int>()) }
    var exVideoIdList by rememberSaveable { mutableStateOf(listOf<Int>()) }
    var isExJoined by rememberSaveable { mutableStateOf(false) }
    val exConnection = remember { RtcConnection() }
    val mainHandler = remember { Handler(Looper.getMainLooper()) }

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
                }

                override fun onLeaveChannel(stats: RtcStats?) {
                    super.onLeaveChannel(stats)
                    isJoined = false
                    videoIdList = emptyList()
                }

                override fun onUserJoined(uid: Int, elapsed: Int) {
                    super.onUserJoined(uid, elapsed)
                    videoIdList = videoIdList + uid
                }

                override fun onUserOffline(uid: Int, reason: Int) {
                    super.onUserOffline(uid, reason)
                    videoIdList = videoIdList - uid
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
        } as RtcEngineEx
    }

    val exRtcEventHandler = remember {
        object : IRtcEngineEventHandler() {
            override fun onJoinChannelSuccess(channel: String?, uid: Int, elapsed: Int) {
                super.onJoinChannelSuccess(channel, uid, elapsed)
                isExJoined = true
            }

            override fun onLeaveChannel(stats: RtcStats?) {
                super.onLeaveChannel(stats)
                isExJoined = false
                exVideoIdList = emptyList()
            }

            override fun onUserJoined(uid: Int, elapsed: Int) {
                super.onUserJoined(uid, elapsed)
                exVideoIdList += uid
            }

            override fun onUserOffline(uid: Int, reason: Int) {
                super.onUserOffline(uid, reason)
                exVideoIdList -= uid
            }

            override fun onSnapshotTaken(
                uid: Int,
                filePath: String?,
                width: Int,
                height: Int,
                errCode: Int
            ) {
                super.onSnapshotTaken(uid, filePath, width, height, errCode)
                if (errCode == 0) {
                    mainHandler.post {
                        Toast.makeText(
                            context,
                            "SnapshotExTaken path=$filePath",
                            Toast.LENGTH_SHORT
                        ).show()
                    }
                } else {
                    mainHandler.post {
                        Toast.makeText(
                            context,
                            "SnapshotExTaken error=${RtcEngine.getErrorDescription(errCode)}",
                            Toast.LENGTH_SHORT
                        ).show()
                    }
                }
            }

            override fun onLocalAudioStateChanged(state: Int, reason: Int) {
                super.onLocalAudioStateChanged(state, reason)
                if (state == Constants.LOCAL_AUDIO_STREAM_STATE_STOPPED) {
                    mainHandler.post {
                        Toast.makeText(context, "麦克风已关闭", Toast.LENGTH_SHORT).show()
                    }
                }
            }
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
                Toast.makeText(context, "Permission Granted", Toast.LENGTH_LONG).show()
                val options = ChannelMediaOptions()
                options.channelProfile = Constants.CHANNEL_PROFILE_LIVE_BROADCASTING
                options.clientRoleType = Constants.CLIENT_ROLE_AUDIENCE
                options.autoSubscribeVideo = true
                options.autoSubscribeAudio = true
                options.publishMicrophoneTrack = false
                options.publishCameraTrack = false
                TokenUtils.gen(channelName, 0) {
                    rtcEngine.joinChannel(it, channelName, 0, options)
                }

                options.clientRoleType = Constants.CLIENT_ROLE_BROADCASTER
                options.autoSubscribeVideo = true
                options.autoSubscribeAudio = true
                options.publishMicrophoneTrack = true
                options.publishCameraTrack = true
                TokenUtils.gen(exConnection.channelId, exConnection.localUid){
                    rtcEngine.joinChannelEx(it, exConnection, options, exRtcEventHandler)
                }
            } else {
                // Permission is denied
                Toast.makeText(context, "Permission Denied", Toast.LENGTH_LONG).show()
            }
        }

    JoinMultiChannelView(
        rtcEngine = rtcEngine,
        channelName = channelName,
        videoList = videoIdList,
        isJoin = isJoined,
        localUid = localUid,
        isExJoin = isExJoined,
        exVideoList = exVideoIdList,
        exConnection = exConnection,
        onExJoinClick = {
            val options = ChannelMediaOptions()
            options.channelProfile = Constants.CHANNEL_PROFILE_LIVE_BROADCASTING
            options.clientRoleType = Constants.CLIENT_ROLE_BROADCASTER
            options.autoSubscribeVideo = true
            options.autoSubscribeAudio = true
            options.publishMicrophoneTrack = true
            options.publishCameraTrack = true
            TokenUtils.gen(exConnection.channelId, exConnection.localUid){
                rtcEngine.joinChannelEx(it, exConnection, options, exRtcEventHandler)
            }
        },
        onExLeaveClick = {
            val options = LeaveChannelOptions()
            options.stopMicrophoneRecording = it
            rtcEngine.leaveChannelEx(exConnection, options)
        },
        onJoinClick = {
            keyboard?.hide()
            exConnection.channelId = "$it-2"
            exConnection.localUid = Random(System.currentTimeMillis()).nextInt(1000) + 10000
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

@Composable
private fun JoinMultiChannelView(
    rtcEngine: RtcEngineEx?,
    channelName: String,
    videoList: List<Int>,
    isJoin: Boolean,
    localUid: Int,
    isExJoin: Boolean,
    exConnection: RtcConnection?,
    exVideoList: List<Int>,
    onExJoinClick: () -> Unit = {},
    onExLeaveClick: (isStopMicrophoneRecording: Boolean) -> Unit = {},
    onJoinClick: (String) -> Unit = {},
    onLeaveClick: () -> Unit = {},
) {
    var exWillLeave by rememberSaveable { mutableStateOf(false) }
    val context = LocalContext.current

    Column {
        Box(modifier = Modifier.weight(1f)) {
            Column {
                VideoGrid(
                    modifier = Modifier.weight(1f),
                    column = 2,
                    raw = 1,
                    videoIdList = videoList,
                    setupVideo = { view, id, isFirstCreate ->
                        val canvas = VideoCanvas(view, Constants.RENDER_MODE_HIDDEN, id)
                        if (id == localUid) {
                            rtcEngine?.setupLocalVideo(canvas)
                            if (isFirstCreate) {
                                rtcEngine?.startPreview()
                            }
                        } else {
                            rtcEngine?.setupRemoteVideo(canvas)
                        }
                    })
                VideoGrid(
                    modifier = Modifier.weight(1f),
                    column = 1,
                    raw = 1,
                    videoIdList = exVideoList,
                    setupVideo = { view, id, isFirstCreate ->
                        val canvas = VideoCanvas(view, Constants.RENDER_MODE_FIT, id)
                        rtcEngine?.setupRemoteVideoEx(canvas, exConnection)
                    })
            }
            Column(modifier = Modifier.align(Alignment.BottomEnd)) {
                Button(
                    onClick = {
                        val path =
                            context.externalCacheDir?.absolutePath + File.separator + "joinmultiplechannel_snapshot_ex.png"
                        rtcEngine?.takeSnapshotEx(exConnection, exVideoList.first(), path)
                    },
                    enabled = exVideoList.isNotEmpty()
                ) {
                    Text(text = stringResource(id = R.string.snapshot_ex))
                }
                Button(
                    onClick = {
                        if (isExJoin) {
                            exWillLeave = true
                        } else {
                            onExJoinClick()
                        }
                    },
                    enabled = isJoin
                ) {
                    Text(text = if (isExJoin) stringResource(id = R.string.leave_ex) else stringResource(
                        id = R.string.join_ex
                    ))
                }
            }
        }
        ChannelNameInput(
            channelName = channelName,
            isJoined = isJoin,
            onJoinClick = onJoinClick,
            onLeaveClick = onLeaveClick
        )
    }

    if (exWillLeave) {
        var stopMic by rememberSaveable { mutableStateOf(false) }
        AlertDialog(
            title = { Text(stringResource(id = R.string.leave_option)) },
            text = {
                SwitchRaw(title = stringResource(id = R.string.stop_recording_or_not), checked = stopMic) {
                    stopMic = it
                }
            },
            onDismissRequest = { exWillLeave = false },
            confirmButton = {
                TextButton(onClick = {
                    exWillLeave = false
                    onExLeaveClick(stopMic)
                }) {
                    Text(stringResource(id = R.string.confirm))
                }
            },
            dismissButton = {
                TextButton(onClick = { exWillLeave = false }) {
                    Text(stringResource(id = R.string.cancel))
                }
            }
        )
    }
}