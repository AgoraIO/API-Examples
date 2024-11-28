package io.agora.api.example.compose.samples

import android.Manifest
import android.util.Log
import android.view.View
import android.widget.Toast
import androidx.activity.compose.rememberLauncherForActivityResult
import androidx.activity.result.contract.ActivityResultContracts
import androidx.compose.foundation.layout.BoxScope
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.padding
import androidx.compose.material3.AlertDialog
import androidx.compose.material3.Button
import androidx.compose.material3.Text
import androidx.compose.material3.TextButton
import androidx.compose.runtime.Composable
import androidx.compose.runtime.DisposableEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableIntStateOf
import androidx.compose.runtime.mutableStateMapOf
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.rememberCoroutineScope
import androidx.compose.runtime.saveable.rememberSaveable
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.platform.LocalLifecycleOwner
import androidx.compose.ui.platform.LocalSoftwareKeyboardController
import androidx.compose.ui.res.stringResource
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import androidx.core.content.ContentProviderCompat.requireContext
import io.agora.api.example.compose.BuildConfig
import io.agora.api.example.compose.R
import io.agora.api.example.compose.data.SettingPreferences
import io.agora.api.example.compose.ui.common.ChannelNameInput
import io.agora.api.example.compose.ui.common.VideoGrid
import io.agora.api.example.compose.ui.common.VideoStatsInfo
import io.agora.api.example.compose.utils.TokenUtils
import io.agora.rtc2.AgoraMediaRecorder
import io.agora.rtc2.ChannelMediaOptions
import io.agora.rtc2.Constants
import io.agora.rtc2.IMediaRecorderCallback
import io.agora.rtc2.IRtcEngineEventHandler
import io.agora.rtc2.RecorderInfo
import io.agora.rtc2.RecorderStreamInfo
import io.agora.rtc2.RtcEngine
import io.agora.rtc2.RtcEngineConfig
import io.agora.rtc2.video.VideoCanvas
import io.agora.rtc2.video.VideoEncoderConfiguration
import kotlinx.coroutines.launch
import java.io.File

@Composable
fun MediaRecorder() {
    val context = LocalContext.current
    val lifecycleOwner = LocalLifecycleOwner.current
    val keyboard = LocalSoftwareKeyboardController.current
    var isJoined by rememberSaveable { mutableStateOf(false) }
    var channelName by rememberSaveable { mutableStateOf("") }
    var localUid by rememberSaveable { mutableIntStateOf(0) }
    var videoIdList by rememberSaveable { mutableStateOf(listOf<Int>()) }
    val statsMap = remember { mutableStateMapOf(0 to VideoStatsInfo()) }
    val recorders = remember { mutableMapOf<Int, AgoraMediaRecorder>() }
    var recoderResult by rememberSaveable { mutableStateOf("") }

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
                    statsMap[uid] = VideoStatsInfo()
                }

                override fun onLeaveChannel(stats: RtcStats?) {
                    super.onLeaveChannel(stats)
                    isJoined = false
                    videoIdList = emptyList()
                    statsMap.clear()
                }

                override fun onUserJoined(uid: Int, elapsed: Int) {
                    super.onUserJoined(uid, elapsed)
                    videoIdList = videoIdList + uid
                    statsMap[uid] = VideoStatsInfo()
                }

                override fun onUserOffline(uid: Int, reason: Int) {
                    super.onUserOffline(uid, reason)
                    videoIdList = videoIdList - uid
                    statsMap.remove(uid)
                }

                override fun onRtcStats(stats: RtcStats?) {
                    super.onRtcStats(stats)
                    statsMap[localUid]?.copy(rtcStats = stats)?.let {
                        statsMap[localUid] = it
                    }
                }

                override fun onLocalVideoStats(
                    source: Constants.VideoSourceType?,
                    stats: LocalVideoStats?
                ) {
                    super.onLocalVideoStats(source, stats)
                    statsMap[localUid]?.copy(localVideoStats = stats)?.let {
                        statsMap[localUid] = it
                    }
                }

                override fun onLocalAudioStats(stats: LocalAudioStats?) {
                    super.onLocalAudioStats(stats)
                    statsMap[localUid]?.copy(localAudioStats = stats)?.let {
                        statsMap[localUid] = it
                    }
                }

                override fun onRemoteVideoStats(stats: RemoteVideoStats?) {
                    super.onRemoteVideoStats(stats)
                    val uid = stats?.uid ?: return
                    statsMap[uid]?.copy(remoteVideoStats = stats)?.let {
                        statsMap[uid] = it
                    }
                }

                override fun onRemoteAudioStats(stats: RemoteAudioStats?) {
                    super.onRemoteAudioStats(stats)
                    val uid = stats?.uid ?: return
                    statsMap[uid]?.copy(remoteAudioStats = stats)?.let {
                        statsMap[uid] = it
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
    DisposableEffect(lifecycleOwner) {
        onDispose {
            if (isJoined) {
                rtcEngine.stopPreview()
                rtcEngine.leaveChannel()
            }
            RtcEngine.destroy()
        }
    }
    val permissionLauncher =
        rememberLauncherForActivityResult(contract = ActivityResultContracts.RequestMultiplePermissions()) { grantedMap ->
            val allGranted = grantedMap.values.all { it }
            if (allGranted) {
                // Permission is granted
                Toast.makeText(context, "Permission Granted", Toast.LENGTH_LONG).show()
                val mediaOptions = ChannelMediaOptions()
                mediaOptions.channelProfile = Constants.CHANNEL_PROFILE_LIVE_BROADCASTING
                mediaOptions.clientRoleType = Constants.CLIENT_ROLE_BROADCASTER
                TokenUtils.gen(channelName, 0) {
                    rtcEngine.joinChannel(it, channelName, 0, mediaOptions)
                }
            } else {
                // Permission is denied
                Toast.makeText(context, "Permission Denied", Toast.LENGTH_LONG).show()
            }
        }
    val coroutineScope = rememberCoroutineScope()
    MediaRecorderView(
        channelName = channelName,
        isJoined = isJoined,
        statsMap = statsMap,
        onJoinClick = {
            keyboard?.hide()
            channelName = it
            permissionLauncher.launch(
                arrayOf(
                    Manifest.permission.RECORD_AUDIO, Manifest.permission.CAMERA
                )
            )
        },
        onLeaveClick = {
            rtcEngine.stopPreview()
            rtcEngine.leaveChannel()
        },
        videoIdList = videoIdList,
        setupVideo = { view, id, isFirstCreate ->
            if (id == localUid) {
                rtcEngine.setupLocalVideo(VideoCanvas(view, Constants.RENDER_MODE_HIDDEN, id))
                rtcEngine.startPreview()
            } else {
                rtcEngine.setupRemoteVideo(VideoCanvas(view, Constants.RENDER_MODE_HIDDEN, id))
            }
        },
        overlay = { _, id ->
            var isRecording by remember { mutableStateOf(false) }
            Button(
                modifier = Modifier
                    .padding(8.dp)
                    .align(Alignment.BottomEnd),
                onClick = {
                    isRecording = !isRecording
                    if (isRecording) {
                        val storagePath: String =
                            context.externalCacheDir?.absolutePath + File.separator + "media_recorder_" + channelName + "_" + id + ".mp4"
                        val recorder =
                            rtcEngine.createMediaRecorder(RecorderStreamInfo(channelName, id, 0))
                        recorder.setMediaRecorderObserver(object : IMediaRecorderCallback {
                            override fun onRecorderStateChanged(
                                channelId: String?,
                                uid: Int,
                                state: Int,
                                reason: Int
                            ) {
                                Log.d(
                                    "MediaRecorder",
                                    "LocalMediaRecorder -- onRecorderStateChanged channelId=$channelId, uid=$uid, state=$state, reason=$reason"
                                )
                                if (state == AgoraMediaRecorder.RECORDER_STATE_STOP) {
                                    recorders.remove(uid)
                                    recoderResult = storagePath
                                } else if (state == AgoraMediaRecorder.RECORDER_STATE_ERROR && reason == AgoraMediaRecorder.RECORDER_REASON_CONFIG_CHANGED) {
                                    coroutineScope.launch {
                                        isRecording = false
                                        recorders[id]?.let {
                                            it.stopRecording()
                                            it.release()
                                        }
                                    }
                                }
                            }

                            override fun onRecorderInfoUpdated(
                                channelId: String?,
                                uid: Int,
                                info: RecorderInfo?
                            ) {
                                info ?: return
                                Log.d(
                                    "MediaRecorder",
                                    "LocalMediaRecorder -- onRecorderInfoUpdated channelId="
                                            + channelId + ", uid=" + uid + ", fileName=" + info.fileName
                                            + ", durationMs=" + info.durationMs + ", fileSize=" + info.fileSize
                                )
                            }
                        })
                        recorder.startRecording(
                            AgoraMediaRecorder.MediaRecorderConfiguration(
                                storagePath,
                                AgoraMediaRecorder.CONTAINER_MP4,
                                AgoraMediaRecorder.STREAM_TYPE_BOTH,
                                120000,
                                0
                            )
                        )
                        recorders[id] = recorder
                    } else {
                        recorders[id]?.let {
                            it.stopRecording()
                            it.release()
                        }
                    }
                })
            {
                Text(
                    text = if (!isRecording) stringResource(id = R.string.start_recording) else stringResource(
                        id = R.string.stop_recording
                    )
                )
            }
        },
        onCameraSwitchClick = {
            rtcEngine.switchCamera()
        }
    )

    if (recoderResult.isNotBlank()) {
        AlertDialog(
            onDismissRequest = { recoderResult = "" },
            confirmButton = {
                TextButton(onClick = {
                    recoderResult = ""
                }) {
                    Text(stringResource(id = R.string.confirm))
                }
            },
            title = { Text(text = "Recorder Result") },
            text = { Text(text = recoderResult) }
        )
    }
}

@Composable
private fun MediaRecorderView(
    channelName: String,
    isJoined: Boolean,
    onJoinClick: (String) -> Unit,
    onLeaveClick: () -> Unit,
    videoIdList: List<Int>,
    setupVideo: (View, Int, Boolean) -> Unit,
    statsMap: Map<Int, VideoStatsInfo> = emptyMap(),
    overlay: @Composable BoxScope.(index: Int, id: Int) -> Unit? = { _, _ -> },
    onCameraSwitchClick: () -> Unit = { }
) {
    Column {
        VideoGrid(
            modifier = Modifier.weight(1.0f),
            videoIdList = videoIdList,
            setupVideo = setupVideo,
            overlay = overlay
        )
        Button(
            modifier = Modifier
                .padding(16.dp, 8.dp)
                .align(Alignment.End),
            onClick = onCameraSwitchClick,
            enabled = isJoined
        ) {
            Text(text = stringResource(id = R.string.switch_camera))
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
private fun MediaRecorderViewPreview() {
    MediaRecorderView(
        channelName = "",
        isJoined = false,
        onJoinClick = {},
        onLeaveClick = { },
        videoIdList = listOf(0, 1, 2, 3),
        setupVideo = { _, _, _ -> }
    )
}