package io.agora.api.example.compose.samples

import android.content.Context
import android.graphics.Matrix
import android.opengl.GLES20
import android.opengl.GLSurfaceView
import android.util.Log
import android.view.View
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
import io.agora.api.example.compose.ui.common.Switching1v1VideoView
import io.agora.api.example.compose.ui.common.VideoStatsInfo
import io.agora.api.example.compose.utils.GLTextureView
import io.agora.api.example.compose.utils.GLTextureView.Renderer
import io.agora.api.example.compose.utils.YuvUploader
import io.agora.base.TextureBufferHelper
import io.agora.base.VideoFrame
import io.agora.base.VideoFrame.I420Buffer
import io.agora.base.internal.video.EglBase10
import io.agora.base.internal.video.GlRectDrawer
import io.agora.base.internal.video.RendererCommon
import io.agora.rtc2.ChannelMediaOptions
import io.agora.rtc2.Constants
import io.agora.rtc2.IRtcEngineEventHandler
import io.agora.rtc2.RtcEngine
import io.agora.rtc2.RtcEngineConfig
import io.agora.rtc2.video.IVideoFrameObserver
import io.agora.rtc2.video.VideoCanvas
import io.agora.rtc2.video.VideoEncoderConfiguration
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10

@Composable
fun CustomVideoRender() {
    val context = LocalContext.current
    val lifecycleOwner = LocalLifecycleOwner.current
    val keyboard = LocalSoftwareKeyboardController.current
    var isJoined by rememberSaveable { mutableStateOf(false) }
    var channelName by rememberSaveable { mutableStateOf("") }
    var localUid by rememberSaveable { mutableIntStateOf(0) }
    var localStats by remember { mutableStateOf(VideoStatsInfo()) }
    var remoteStats by remember { mutableStateOf(VideoStatsInfo()) }

    val rtcEngine = remember {
        RtcEngine.create(RtcEngineConfig().apply {
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
                    remoteStats = VideoStatsInfo()
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

    val render = remember(rtcEngine) {
        CustomVideoRenderRender(rtcEngine)
    }

    DisposableEffect(lifecycleOwner) {
        onDispose {
            render.dispose()
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
                rtcEngine.joinChannel("", channelName, 0, mediaOptions)
            } else {
                // Permission is denied
                Toast.makeText(context, "Permission Denied", Toast.LENGTH_LONG).show()
            }
        }

    CustomVideoRenderView(
        rtcEngine = rtcEngine,
        channelName = channelName,
        isJoined = isJoined,
        localUid = localUid,
        remoteUid = 1,
        localStats = localStats,
        remoteStats = remoteStats,
        onRemoteViewCreate = {
            GLTextureView(it).apply {
                render.setGLTextureView(this)
            }
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

@Composable
fun CustomVideoRenderView(
    channelName: String,
    isJoined: Boolean,
    rtcEngine: RtcEngine? = null,
    localUid: Int = 0,
    remoteUid: Int = 0,
    localStats: VideoStatsInfo = VideoStatsInfo(),
    remoteStats: VideoStatsInfo = VideoStatsInfo(),
    onRemoteViewCreate: ((context: Context) -> View)? = null,
    onJoinClick: (String) -> Unit,
    onLeaveClick: () -> Unit
) {
    Column {
        Switching1v1VideoView(
            modifier = Modifier.weight(1.0f),
            localUid = localUid,
            remoteUid = remoteUid,
            localStats = localStats,
            remoteStats = remoteStats,
            switchable = false,
            localLarge = true,
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
            remoteCreate = onRemoteViewCreate,
            remoteRender = { _, _, _ -> }
        )


        ChannelNameInput(
            channelName = channelName,
            isJoined = isJoined,
            onJoinClick = onJoinClick,
            onLeaveClick = onLeaveClick
        )
    }
}

class CustomVideoRenderRender(
    private val rtcEngine: RtcEngine
) : IVideoFrameObserver {

    private val TAG = "CustomVideoRenderRender"

    private var glTextureView: GLTextureView? = null

    private var textureBufferHelper: TextureBufferHelper? = null

    private var viewportWidth = 0
    private var viewportHeight = 0

    private val drawer = GlRectDrawer()
    private val yuvUploader: YuvUploader = YuvUploader()
    private val renderMatrix = Matrix()
    private var lastI420Frame: VideoFrame? = null

    private var renderUid = -1
    private var renderUidNoFoundFrame = 0

    private val glRenderer = object : Renderer {
        override fun onSurfaceCreated(gl: GL10?, config: EGLConfig?) {
            Log.d(TAG, "onSurfaceCreated")
            textureBufferHelper = TextureBufferHelper.create(
                "bufferHelper",
                EglBase10.Context(glTextureView!!.getEglContext())
            )
        }

        override fun onSurfaceChanged(gl: GL10?, width: Int, height: Int) {
            Log.d(TAG, "onSurfaceCreated  w: $width  h: $height")
            viewportWidth = width
            viewportHeight = height
        }

        override fun onDrawFrame(gl: GL10?) {
            GLES20.glClearColor(0f /* red */, 0f /* green */, 0f /* blue */, 0f /* alpha */)
            GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT)
            val frame = lastI420Frame ?: return
            Log.d(TAG, "onDrawFrame: " + frame.rotation)
            renderMatrix.reset()
            renderMatrix.preTranslate(0.5f, 0.5f)
            renderMatrix.preScale(1f, -1f) // I420-frames are upside down

            renderMatrix.preRotate(frame.rotation.toFloat())
            renderMatrix.preTranslate(-0.5f, -0.5f)
            try {
                drawer.drawYuv(
                    yuvUploader.yuvTextures,
                    RendererCommon.convertMatrixFromAndroidGraphicsMatrix(renderMatrix),
                    frame.rotatedWidth,
                    frame.rotatedHeight,
                    0,
                    0,
                    viewportWidth,
                    viewportHeight
                )
            } catch (exception: NullPointerException) {
                Log.e(TAG, "skip empty buffer!")
            }
        }
    }

    init {
        rtcEngine.registerVideoFrameObserver(this)
    }

    fun setGLTextureView(textureView: GLTextureView) {
        textureView.preserveEGLContextOnPause = true
        textureView.setEGLContextClientVersion(2)
        textureView.setRenderer(glRenderer)
        textureView.renderMode = GLSurfaceView.RENDERMODE_WHEN_DIRTY
        glTextureView = textureView
    }

    fun dispose() {
        lastI420Frame = null
        renderUid = -1
        renderUidNoFoundFrame = 0
        rtcEngine.registerVideoFrameObserver(null)
        textureBufferHelper?.invoke {
            drawer.release()
            yuvUploader.release()
        }
        textureBufferHelper?.dispose()
        textureBufferHelper = null
    }

    override fun onCaptureVideoFrame(sourceType: Int, videoFrame: VideoFrame?) = false

    override fun onPreEncodeVideoFrame(sourceType: Int, videoFrame: VideoFrame?) = false

    override fun onMediaPlayerVideoFrame(videoFrame: VideoFrame?, mediaPlayerId: Int) = false

    override fun getVideoFrameProcessMode() = IVideoFrameObserver.PROCESS_MODE_READ_ONLY

    override fun getVideoFormatPreference() = IVideoFrameObserver.VIDEO_PIXEL_I420

    override fun getRotationApplied() = false

    override fun getMirrorApplied() = false

    override fun getObservedFramePosition() = IVideoFrameObserver.POSITION_PRE_RENDERER

    override fun onRenderVideoFrame(
        channelId: String?,
        uid: Int,
        videoFrame: VideoFrame?
    ): Boolean {
        if (renderUid == -1) {
            renderUid = uid
        }
        if (renderUid != uid) {
            renderUidNoFoundFrame++
        }
        if (renderUidNoFoundFrame > 4) {
            renderUidNoFoundFrame = 0
            renderUid = -1
        }
        if (glTextureView != null && videoFrame !== lastI420Frame && renderUid == uid) {
            renderUidNoFoundFrame = 0
            Log.d(TAG, "onRenderVideoFrame: " + uid + "   connection: " + channelId + "  buffer: " + videoFrame!!.buffer)
            lastI420Frame = videoFrame
            textureBufferHelper!!.invoke<Void> {
                if (lastI420Frame!!.buffer is I420Buffer) {
                    val i420Buffer = lastI420Frame!!.buffer as I420Buffer
                    GLES20.glPixelStorei(GLES20.GL_UNPACK_ALIGNMENT, 1)
                    yuvUploader.uploadFromBuffer(i420Buffer)
                }
                null
            }
            glTextureView?.requestRender()
        }
        return false
    }

}