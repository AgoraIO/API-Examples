package io.agora.api.example.compose.samples

import android.content.ContentResolver
import android.content.ContentValues
import android.content.Context
import android.graphics.Bitmap
import android.graphics.Matrix
import android.net.Uri
import android.os.Build
import android.os.Environment
import android.os.Handler
import android.os.Looper
import android.provider.MediaStore
import android.widget.Toast
import androidx.activity.compose.rememberLauncherForActivityResult
import androidx.activity.result.contract.ActivityResultContracts
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.padding
import androidx.compose.material3.Button
import androidx.compose.material3.Text
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
import androidx.compose.ui.unit.dp
import io.agora.api.example.compose.BuildConfig
import io.agora.api.example.compose.R
import io.agora.api.example.compose.data.SettingPreferences
import io.agora.api.example.compose.ui.common.ChannelNameInput
import io.agora.api.example.compose.ui.common.TwoVideoView
import io.agora.api.example.compose.ui.common.VideoStatsInfo
import io.agora.api.example.compose.utils.TokenUtils
import io.agora.api.example.compose.utils.YUVUtils
import io.agora.base.VideoFrame
import io.agora.base.VideoFrame.I420Buffer
import io.agora.base.internal.video.YuvHelper
import io.agora.rtc2.ChannelMediaOptions
import io.agora.rtc2.ClientRoleOptions
import io.agora.rtc2.Constants
import io.agora.rtc2.IRtcEngineEventHandler
import io.agora.rtc2.RtcEngine
import io.agora.rtc2.RtcEngineConfig
import io.agora.rtc2.video.IVideoFrameObserver
import io.agora.rtc2.video.VideoCanvas
import io.agora.rtc2.video.VideoEncoderConfiguration
import java.io.File
import java.io.OutputStream
import java.nio.ByteBuffer

@Composable
fun OriginVideoData() {
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
    var clientRole by remember { mutableIntStateOf(Constants.CLIENT_ROLE_BROADCASTER) }

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
        }
    }

    val screenshotTaker = remember(context, rtcEngine) {
        OriginVideoDataScreenshotTaker(context, rtcEngine)
    }

    DisposableEffect(lifecycleOwner) {
        onDispose {
            screenshotTaker.dispose()
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
                TokenUtils.gen(channelName, 0){
                    rtcEngine.joinChannel(it, channelName, 0, mediaOptions)
                }
            } else {
                // Permission is denied
                Toast.makeText(context, R.string.permission_denied, Toast.LENGTH_LONG).show()
            }
        }

    OriginVideoDataView(
        rtcEngine = rtcEngine,
        channelName = channelName,
        isJoined = isJoined,
        localUid = localUid,
        remoteUid = remoteUid,
        localStats = localStats,
        remoteStats = remoteStats,
        localLarge = localLarge,
        onSwitch = {
            localLarge = !localLarge
        },
        onScreenshotClick = {
            screenshotTaker.tackSnapshot()
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
        })
}

@Composable
private fun OriginVideoDataView(
    channelName: String,
    isJoined: Boolean,
    rtcEngine: RtcEngine? = null,
    localUid: Int = 0,
    remoteUid: Int = 0,
    localLarge: Boolean = true,
    localStats: VideoStatsInfo = VideoStatsInfo(),
    remoteStats: VideoStatsInfo = VideoStatsInfo(),
    onScreenshotClick: () -> Unit = {},
    onSwitch: () -> Unit = {},
    onJoinClick: (String) -> Unit,
    onLeaveClick: () -> Unit
) {
    Column {
        TwoVideoView(
            modifier = Modifier.weight(1.0f),
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


        Button(
            modifier = Modifier.align(Alignment.End).padding(16.dp, 0.dp),
            onClick = onScreenshotClick
        ) {
            Text(text = stringResource(id = R.string.screenshot))
        }

        ChannelNameInput(
            channelName = channelName,
            isJoined = isJoined,
            onJoinClick = onJoinClick,
            onLeaveClick = onLeaveClick
        )
    }
}

private class OriginVideoDataScreenshotTaker(
    private val context: Context,
    private val rtcEngine: RtcEngine
) : IVideoFrameObserver {

    private var isSnapshot = false
    private var videoNV21Buffer: ByteBuffer? = null
    private var videoNV21: ByteArray = ByteArray(0)
    private val handler = Handler(Looper.getMainLooper())

    init {
        rtcEngine.registerVideoFrameObserver(this)
    }

    fun dispose() {
        rtcEngine.registerVideoFrameObserver(null)
    }

    fun tackSnapshot() {
        isSnapshot = true
    }

    private fun showLongToast(text: String) {
        handler.post {
            Toast.makeText(context, text, Toast.LENGTH_LONG).show()
        }
    }

    private fun saveBitmap2Gallery(bm: Bitmap) {
        val currentTime = System.currentTimeMillis()

        // name the file
        val imageFileName = "IMG_AGORA_$currentTime.jpg"
        val imageFilePath: String
        imageFilePath = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q) {
            Environment.DIRECTORY_PICTURES + File.separator + "Agora" + File.separator
        } else {
            (Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_PICTURES).absolutePath
                    + File.separator + "Agora" + File.separator)
        }

        // write to file
        val outputStream: OutputStream
        val resolver: ContentResolver = context.contentResolver
        val newScreenshot = ContentValues()
        val insert: Uri?
        newScreenshot.put(MediaStore.Images.ImageColumns.DATE_ADDED, currentTime)
        newScreenshot.put(MediaStore.Images.ImageColumns.DISPLAY_NAME, imageFileName)
        newScreenshot.put(MediaStore.Images.ImageColumns.MIME_TYPE, "image/jpg")
        newScreenshot.put(MediaStore.Images.ImageColumns.WIDTH, bm.width)
        newScreenshot.put(MediaStore.Images.ImageColumns.HEIGHT, bm.height)
        try {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q) {
                newScreenshot.put(MediaStore.Images.ImageColumns.RELATIVE_PATH, imageFilePath)
            } else {
                // make sure the path is existed
                val imageFileDir = File(imageFilePath)
                if (!imageFileDir.exists()) {
                    val mkdir = imageFileDir.mkdirs()
                    if (!mkdir) {
                        showLongToast("save failed, error: cannot create folder. Make sure app has the permission.")
                        return
                    }
                }
                newScreenshot.put(
                    MediaStore.Images.ImageColumns.DATA,
                    imageFilePath + imageFileName
                )
                newScreenshot.put(MediaStore.Images.ImageColumns.TITLE, imageFileName)
            }

            // insert a new image
            insert = resolver.insert(MediaStore.Images.Media.EXTERNAL_CONTENT_URI, newScreenshot)
            // write data
            outputStream = resolver.openOutputStream(insert!!)!!
            bm.compress(Bitmap.CompressFormat.PNG, 80, outputStream)
            outputStream.flush()
            outputStream.close()
            newScreenshot.clear()
            newScreenshot.put(MediaStore.Images.ImageColumns.SIZE, File(imageFilePath).length())
            resolver.update(insert, newScreenshot, null, null)
            showLongToast("save success, you can view it in gallery")
        } catch (e: Exception) {
            showLongToast("save failed, error: " + e.message)
            e.printStackTrace()
        }
    }

    override fun onCaptureVideoFrame(sourceType: Int, videoFrame: VideoFrame?): Boolean {
        val buffer = videoFrame!!.buffer

        val needToI420 = buffer !is I420Buffer
        val i420Buffer = if (needToI420) buffer.toI420() else buffer as I420Buffer
        val width = i420Buffer.width
        val height = i420Buffer.height

        val nv21MinSize = ((width * height * 3 + 1) / 2.0f).toInt()
        if (videoNV21Buffer == null || (videoNV21Buffer?.capacity() ?: 0) < nv21MinSize) {
            videoNV21Buffer = ByteBuffer.allocateDirect(nv21MinSize)
            videoNV21 = ByteArray(nv21MinSize)
        }
        YuvHelper.I420ToNV12(
            i420Buffer.dataY, i420Buffer.strideY,
            i420Buffer.dataV, i420Buffer.strideV,
            i420Buffer.dataU, i420Buffer.strideU,
            videoNV21Buffer, width, height
        )
        videoNV21Buffer?.position(0)
        videoNV21Buffer?.get(videoNV21)
        val nv21: ByteArray = videoNV21


        if (isSnapshot) {
            isSnapshot = false
            val bitmap: Bitmap = YUVUtils.nv21ToBitmap(
                context,
                nv21,
                width,
                height
            )
            val matrix = Matrix()
            matrix.setRotate(videoFrame.rotation.toFloat())
            // Rotate around the original position
            val newBitmap = Bitmap.createBitmap(bitmap, 0, 0, width, height, matrix, false)
            // save to file
            saveBitmap2Gallery(newBitmap)
            bitmap.recycle()
        }

        if (needToI420) {
            i420Buffer.release()
        }

        return true
    }

    override fun onPreEncodeVideoFrame(sourceType: Int, videoFrame: VideoFrame?) = false

    override fun onMediaPlayerVideoFrame(videoFrame: VideoFrame?, mediaPlayerId: Int) = false

    override fun getVideoFrameProcessMode() = IVideoFrameObserver.PROCESS_MODE_READ_WRITE

    override fun getVideoFormatPreference() = IVideoFrameObserver.VIDEO_PIXEL_DEFAULT

    override fun getRotationApplied() = false

    override fun getMirrorApplied() = false

    override fun getObservedFramePosition() = IVideoFrameObserver.POSITION_POST_CAPTURER

    override fun onRenderVideoFrame(
        channelId: String?,
        uid: Int,
        videoFrame: VideoFrame?
    ) = false


}