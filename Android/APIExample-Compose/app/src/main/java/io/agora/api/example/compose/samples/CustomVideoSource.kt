package io.agora.api.example.compose.samples

import android.content.Context
import android.graphics.Matrix
import android.os.Handler
import android.os.Looper
import android.util.Log
import android.widget.Toast
import androidx.activity.compose.rememberLauncherForActivityResult
import androidx.activity.result.contract.ActivityResultContracts
import androidx.compose.foundation.layout.Column
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
import io.agora.api.example.compose.BuildConfig
import io.agora.api.example.compose.data.SettingPreferences
import io.agora.api.example.compose.ui.common.ChannelNameInput
import io.agora.api.example.compose.ui.common.DropdownMenuRaw
import io.agora.api.example.compose.ui.common.TwoVideoView
import io.agora.api.example.compose.ui.common.VideoStatsInfo
import io.agora.api.example.compose.utils.TokenUtils
import io.agora.api.example.compose.utils.VideoFileReader
import io.agora.api.example.compose.utils.YuvFboProgram
import io.agora.base.JavaI420Buffer
import io.agora.base.NV12Buffer
import io.agora.base.NV21Buffer
import io.agora.base.TextureBuffer
import io.agora.base.TextureBufferHelper
import io.agora.base.VideoFrame
import io.agora.base.internal.video.YuvConverter
import io.agora.base.internal.video.YuvHelper
import io.agora.rtc2.ChannelMediaOptions
import io.agora.rtc2.Constants
import io.agora.rtc2.IRtcEngineEventHandler
import io.agora.rtc2.RtcEngine
import io.agora.rtc2.RtcEngineConfig
import io.agora.rtc2.gl.EglBaseProvider
import io.agora.rtc2.video.VideoCanvas
import io.agora.rtc2.video.VideoEncoderConfiguration
import java.nio.ByteBuffer

@Composable
fun CustomVideoSource() {
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

    val externalVideoFramePusher = remember {
        ExternalVideoFramePusher(context, rtcEngine)
    }

    DisposableEffect(lifecycleOwner) {
        onDispose {
            externalVideoFramePusher.dispose()
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
                val mediaOptions = ChannelMediaOptions()
                mediaOptions.channelProfile = Constants.CHANNEL_PROFILE_LIVE_BROADCASTING
                mediaOptions.clientRoleType = Constants.CLIENT_ROLE_BROADCASTER
                mediaOptions.autoSubscribeVideo = true
                mediaOptions.autoSubscribeVideo = true
                mediaOptions.publishCustomVideoTrack = true
                TokenUtils.gen(channelName, 0){
                    rtcEngine.joinChannel(it, channelName, 0, mediaOptions)
                }

                externalVideoFramePusher.start()
            } else {
                // Permission is denied
                Toast.makeText(context, "Permission Denied", Toast.LENGTH_LONG).show()
            }
        }

    CustomVideoSourceView(
        rtcEngine = rtcEngine,
        channelName = channelName,
        isJoined = isJoined,
        localUid = localUid,
        remoteUid = remoteUid,
        localLarge = localLarge,
        localStats = localStats,
        remoteStats = remoteStats,
        onSwitch = {
            localLarge = !localLarge
        },
        onPushVideoFrameTypeChanged = {
            externalVideoFramePusher.setPushVideoFrameType(it)
        },
        onJoinClick = {
            channelName = it
            keyboard?.hide()
            permissionLauncher.launch(
                arrayOf(
                    android.Manifest.permission.CAMERA,
                    android.Manifest.permission.RECORD_AUDIO
                )
            )
        },
        onLeaveClick = {
            rtcEngine.leaveChannel()
            externalVideoFramePusher.stop()
        })
}

@Composable
private fun CustomVideoSourceView(
    rtcEngine: RtcEngine? = null,
    channelName: String,
    isJoined: Boolean,
    localUid: Int = 0,
    remoteUid: Int = 0,
    localLarge: Boolean = true,
    localStats: VideoStatsInfo = VideoStatsInfo(),
    remoteStats: VideoStatsInfo = VideoStatsInfo(),
    onSwitch: () -> Unit = {},
    onPushVideoFrameTypeChanged: (ExternalVideoFramePusher.PushVideoFrameType) -> Unit = {},
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
                        Constants.RENDER_MODE_FIT,
                        uid
                    ).apply {
                        mirrorMode = Constants.VIDEO_MIRROR_MODE_DISABLED
                        sourceType = Constants.VIDEO_SOURCE_CUSTOM
                    }
                )
                if (isFirstSetup) {
                    rtcEngine?.startPreview(Constants.VideoSourceType.VIDEO_SOURCE_CUSTOM)
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

        DropdownMenuRaw(
            title = "Push Video Frame Type",
            options = listOf(
                "I420" to ExternalVideoFramePusher.PushVideoFrameType.I420,
                "NV21" to ExternalVideoFramePusher.PushVideoFrameType.NV21,
                "NV12" to ExternalVideoFramePusher.PushVideoFrameType.NV12,
                "Texture2D" to ExternalVideoFramePusher.PushVideoFrameType.Texture2D
            )
        ) { _, option ->
            onPushVideoFrameTypeChanged(option.second)
        }

        ChannelNameInput(
            channelName = channelName,
            isJoined = isJoined,
            onJoinClick = onJoinClick,
            onLeaveClick = onLeaveClick
        )
    }
}

private class ExternalVideoFramePusher(
    private val context: Context,
    private val engine: RtcEngine,
    private val trackId: Int = 0
) : VideoFileReader.OnVideoReadListener {

    enum class PushVideoFrameType {
        I420,
        NV21,
        NV12,
        Texture2D
    }

    private val videoFileReader = VideoFileReader(context, trackId, this)

    private var pushVideoFrameType = PushVideoFrameType.I420

    private val TAG = "ExternalVideoFramePusher"

    private var textureBufferHelper: TextureBufferHelper? = null

    private var yuvFboProgram: YuvFboProgram? = null


    fun start() {
        videoFileReader.start()
    }

    fun stop() {
        videoFileReader.stop()
    }

    fun dispose() {
        videoFileReader.stop()
        textureBufferHelper?.invoke {
            yuvFboProgram?.release()
            yuvFboProgram = null
        }
        textureBufferHelper?.dispose()
        textureBufferHelper = null
    }

    fun setPushVideoFrameType(type: PushVideoFrameType) {
        pushVideoFrameType = type
    }

    override fun onVideoRead(buffer: ByteArray?, width: Int, height: Int) {
        buffer ?: return
        when (pushVideoFrameType) {
            PushVideoFrameType.NV21 -> pushVideoFrameByNV21(
                yuv2nv21(buffer, width, height),
                width,
                height
            )

            PushVideoFrameType.NV12 -> pushVideoFrameByNV12(
                yuv2nv12(buffer, width, height),
                width,
                height
            )

            PushVideoFrameType.Texture2D -> {
                if (textureBufferHelper == null) {
                    textureBufferHelper = TextureBufferHelper.create(
                        "PushExternalVideoFrame",
                        EglBaseProvider.instance().rootEglBase.eglBaseContext
                    )
                }
                textureBufferHelper?.invoke {
                    pushVideoFrameByTexture(
                        yuv2texture(buffer, width, height),
                        VideoFrame.TextureBuffer.Type.RGB,
                        width,
                        height
                    )
                }

            }

            else -> pushVideoFrameByI420(buffer, width, height)
        }
    }

    /**
     * Push video frame by i420.
     *
     * @param yuv    i420 data
     * @param width  width
     * @param height height
     */
    private fun pushVideoFrameByI420(yuv: ByteArray, width: Int, height: Int) {
        val i420Buffer = JavaI420Buffer.allocate(width, height)
        i420Buffer.dataY.put(yuv, 0, i420Buffer.dataY.limit())
        i420Buffer.dataU.put(yuv, i420Buffer.dataY.limit(), i420Buffer.dataU.limit())
        i420Buffer.dataV.put(
            yuv,
            i420Buffer.dataY.limit() + i420Buffer.dataU.limit(),
            i420Buffer.dataV.limit()
        )

        /*
         * Get monotonic time in ms which can be used by capture time,
         * typical scenario is as follows:
         */
        val currentMonotonicTimeInMs = engine.currentMonotonicTimeInMs
        /*
         * Create a video frame to push.
         */
        val videoFrame = VideoFrame(i420Buffer, 0, currentMonotonicTimeInMs * 1000000)

        /*
         * Pushes the external video frame to the app.
         */
        val ret = engine.pushExternalVideoFrameById(videoFrame, trackId)
        i420Buffer.release()
        if (ret != 0) {
            Log.w(TAG, "pushExternalVideoFrame error")
        }
    }

    /**
     * Push video frame by texture id.
     *
     * @param textureId   texture id.
     * @param textureType texture type. rgb or oes.
     * @param width       width.
     * @param height      height.
     */
    private fun pushVideoFrameByTexture(
        textureId: Int,
        textureType: VideoFrame.TextureBuffer.Type,
        width: Int,
        height: Int
    ) {
        val frameBuffer: VideoFrame.Buffer = TextureBuffer(
            EglBaseProvider.getCurrentEglContext()!!,
            width,
            height,
            textureType,
            textureId,
            Matrix(),
            Handler(Looper.myLooper()!!),
            YuvConverter(),
            null
        )

        /*
         * Get monotonic time in ms which can be used by capture time,
         * typical scenario is as follows:
         */
        val currentMonotonicTimeInMs = engine.currentMonotonicTimeInMs
        /*
         * Create a video frame to push.
         */
        val videoFrame = VideoFrame(frameBuffer, 0, currentMonotonicTimeInMs * 1000000)

        /*
         * Pushes the external video frame to the app.
         */
        val ret = engine.pushExternalVideoFrameById(videoFrame, trackId)
        if (ret != 0) {
            Log.w(TAG, "pushExternalVideoFrame error")
        }
    }


    /**
     * Push video frame by nv12.
     *
     * @param nv12   nv12 buffer.
     * @param width  width.
     * @param height height.
     */
    private fun pushVideoFrameByNV12(nv12: ByteBuffer, width: Int, height: Int) {
        val frameBuffer: VideoFrame.Buffer = NV12Buffer(width, height, width, height, nv12, null)

        /*
         * Get monotonic time in ms which can be used by capture time,
         * typical scenario is as follows:
         */
        val currentMonotonicTimeInMs = engine.currentMonotonicTimeInMs
        /*
         * Create a video frame to push.
         */
        val videoFrame = VideoFrame(frameBuffer, 0, currentMonotonicTimeInMs * 1000000)

        /*
         * Pushes the external video frame to the app.
         */
        val ret = engine.pushExternalVideoFrameById(videoFrame, trackId)
        if (ret != 0) {
            Log.w(TAG, "pushExternalVideoFrame error")
        }
    }

    /**
     * Push video frame by nv21.
     *
     * @param nv21   nv21
     * @param width  width
     * @param height height
     */
    private fun pushVideoFrameByNV21(nv21: ByteArray, width: Int, height: Int) {
        val frameBuffer: VideoFrame.Buffer = NV21Buffer(nv21, width, height, null)

        /*
         * Get monotonic time in ms which can be used by capture time,
         * typical scenario is as follows:
         */
        val currentMonotonicTimeInMs: Long = engine.getCurrentMonotonicTimeInMs()
        /*
         * Create a video frame to push.
         */
        val videoFrame = VideoFrame(frameBuffer, 0, currentMonotonicTimeInMs * 1000000)

        /*
         * Pushes the external video frame to the app.
         */
        val ret = engine.pushExternalVideoFrameById(videoFrame, trackId)
        if (ret != 0) {
            Log.w(TAG, "pushExternalVideoFrame error")
        }
    }

    /**
     * Yuv 2 texture id.
     * Run on gl thread.
     *
     * @param yuv    yuv
     * @param width  width
     * @param height heigh
     * @return rgba texture id
     */
    private fun yuv2texture(yuv: ByteArray, width: Int, height: Int): Int {
        if (yuvFboProgram == null) {
            yuvFboProgram = YuvFboProgram()
        }
        return yuvFboProgram?.drawYuv(yuv, width, height) ?: 0
    }

    /**
     * Transform yuv to nv12
     *
     * @param yuv    yuv
     * @param width  width
     * @param height height
     * @return nv12
     */
    private fun yuv2nv12(yuv: ByteArray, width: Int, height: Int): ByteBuffer {
        val srcSizeY = width * height
        val srcY = ByteBuffer.allocateDirect(srcSizeY)
        srcY.put(yuv, 0, srcSizeY)
        val srcStrideU = width / 2
        val srcHeightU = height / 2
        val srcSizeU = srcStrideU * srcHeightU
        val srcU = ByteBuffer.allocateDirect(srcSizeU)
        srcU.put(yuv, srcSizeY, srcSizeU)
        val srcStrideV = width / 2
        val srcHeightV = height / 2
        val srcSizeV = srcStrideV * srcHeightV
        val srcV = ByteBuffer.allocateDirect(srcSizeV)
        srcV.put(yuv, srcSizeY + srcSizeU, srcSizeV)
        val desSize = srcSizeY + srcSizeU + srcSizeV
        val des = ByteBuffer.allocateDirect(desSize)
        YuvHelper.I420ToNV12(
            srcY,
            width,
            srcU,
            srcStrideU,
            srcV,
            srcStrideV,
            des,
            width,
            height
        )
        return des
    }


    /**
     * Transform yuv to nv21.
     *
     * @param yuv    yuv
     * @param width  width
     * @param height height
     * @return nv21
     */
    private fun yuv2nv21(yuv: ByteArray, width: Int, height: Int): ByteArray {
        val srcSizeY = width * height
        val srcY = ByteBuffer.allocateDirect(srcSizeY)
        srcY.put(yuv, 0, srcSizeY)
        val srcStrideU = width / 2
        val srcHeightU = height / 2
        val srcSizeU = srcStrideU * srcHeightU
        val srcU = ByteBuffer.allocateDirect(srcSizeU)
        srcU.put(yuv, srcSizeY, srcSizeU)
        val srcStrideV = width / 2
        val srcHeightV = height / 2
        val srcSizeV = srcStrideV * srcHeightV
        val srcV = ByteBuffer.allocateDirect(srcSizeV)
        srcV.put(yuv, srcSizeY + srcSizeU, srcSizeV)
        val desSize = srcSizeY + srcSizeU + srcSizeV
        val des = ByteBuffer.allocateDirect(desSize)
        YuvHelper.I420ToNV12(
            srcY,
            width,
            srcV,
            srcStrideV,
            srcU,
            srcStrideU,
            des,
            width,
            height
        )
        val nv21 = ByteArray(desSize)
        des.position(0)
        des[nv21]
        return nv21
    }

}

