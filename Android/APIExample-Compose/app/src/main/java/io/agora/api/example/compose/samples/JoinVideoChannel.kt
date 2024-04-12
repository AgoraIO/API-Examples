package io.agora.api.example.compose.samples

import android.Manifest
import android.view.TextureView
import android.widget.Toast
import androidx.activity.compose.rememberLauncherForActivityResult
import androidx.activity.result.contract.ActivityResultContracts
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxHeight
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.size
import androidx.compose.material3.Button
import androidx.compose.material3.Text
import androidx.compose.material3.TextField
import androidx.compose.runtime.Composable
import androidx.compose.runtime.DisposableEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.saveable.rememberSaveable
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.platform.LocalLifecycleOwner
import androidx.compose.ui.platform.LocalSoftwareKeyboardController
import androidx.compose.ui.unit.Dp
import androidx.compose.ui.unit.dp
import androidx.compose.ui.viewinterop.AndroidView
import io.agora.rtc2.ChannelMediaOptions
import io.agora.rtc2.Constants
import io.agora.rtc2.IRtcEngineEventHandler
import io.agora.rtc2.RtcEngine
import io.agora.rtc2.RtcEngineConfig
import io.agora.rtc2.video.VideoCanvas

@Composable
fun JoinVideoChannel() {
    val context = LocalContext.current
    val lifecycleOwner = LocalLifecycleOwner.current
    val keyboard = LocalSoftwareKeyboardController.current
    var isJoined by rememberSaveable { mutableStateOf(false) }
    var channelName by rememberSaveable { mutableStateOf("") }
    var remoteUidList by rememberSaveable { mutableStateOf(listOf<Int>()) }

    val rtcEngine = remember {
        RtcEngine.create(RtcEngineConfig().apply {
            mContext = context
            mAppId = <=AGORA APP ID=>
            mEventHandler = object : IRtcEngineEventHandler() {
                override fun onJoinChannelSuccess(channel: String?, uid: Int, elapsed: Int) {
                    super.onJoinChannelSuccess(channel, uid, elapsed)
                    isJoined = true
                }

                override fun onLeaveChannel(stats: RtcStats?) {
                    super.onLeaveChannel(stats)
                    isJoined = false
                }

                override fun onUserJoined(uid: Int, elapsed: Int) {
                    super.onUserJoined(uid, elapsed)
                    remoteUidList = remoteUidList + uid
                }

                override fun onUserOffline(uid: Int, reason: Int) {
                    super.onUserOffline(uid, reason)
                    remoteUidList = remoteUidList - uid
                }
            }
        }).apply {
            enableVideo()
        }
    }
    DisposableEffect(lifecycleOwner) {
        onDispose {
            if (isJoined) {
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
                rtcEngine.joinChannel("", channelName, 0, mediaOptions)

            } else {
                // Permission is denied
                Toast.makeText(context, "Permission Denied", Toast.LENGTH_LONG).show()
            }
        }

    Column(Modifier.fillMaxHeight()) {
        Row(Modifier.weight(1.0f)) {
            Box(modifier = Modifier.weight(1.0f)) {
                if (isJoined) {
                    AndroidView(
                        factory = { context ->
                            TextureView(context).apply {
                                rtcEngine.setupLocalVideo(
                                    VideoCanvas(
                                        this,
                                        Constants.RENDER_MODE_HIDDEN,
                                        0
                                    )
                                )
                            }
                        })
                }
            }
            Box(modifier = Modifier.weight(1.0f)) {
                remoteUidList.getOrNull(0)?.let { remoteUid ->
                    AndroidView(
                        factory = { context ->
                            TextureView(context).apply {
                                rtcEngine.setupRemoteVideo(
                                    VideoCanvas(
                                        this,
                                        Constants.RENDER_MODE_HIDDEN,
                                        remoteUid
                                    )
                                )
                            }
                        })
                }
            }
        }
        Row(Modifier.weight(1.0f)) {
            Box(modifier = Modifier.weight(1.0f)) {
                remoteUidList.getOrNull(1)?.let { remoteUid ->
                    AndroidView(
                        factory = { context ->
                            TextureView(context).apply {
                                rtcEngine.setupRemoteVideo(
                                    VideoCanvas(
                                        this,
                                        Constants.RENDER_MODE_HIDDEN,
                                        remoteUid
                                    )
                                )
                            }
                        })
                }
            }
            Box(modifier = Modifier.weight(1.0f)) {
                remoteUidList.getOrNull(2)?.let { remoteUid ->
                    AndroidView(
                        factory = { context ->
                            TextureView(context).apply {
                                rtcEngine.setupRemoteVideo(
                                    VideoCanvas(
                                        this,
                                        Constants.RENDER_MODE_HIDDEN,
                                        remoteUid
                                    )
                                )
                            }
                        })
                }
            }
        }
        Row(
            modifier = Modifier.fillMaxWidth(),
            verticalAlignment = Alignment.CenterVertically
        ) {
            TextField(
                modifier = Modifier.weight(1.0f),
                value = channelName,
                onValueChange = { channelName = it },
                label = { Text("Channel Name") },
                singleLine = true
            )
            Button(
                modifier = Modifier.size(90.dp, Dp.Unspecified),
                onClick = {
                    keyboard?.hide()
                    if (isJoined) {
                        rtcEngine.leaveChannel()
                    } else {
                        permissionLauncher.launch(
                            arrayOf(
                                Manifest.permission.RECORD_AUDIO, Manifest.permission.CAMERA
                            )
                        )
                    }
                }
            ) {
                Text(if (isJoined) "Leave" else "Join")
            }
        }
    }
}