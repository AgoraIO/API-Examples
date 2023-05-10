package io.agora.beauty.sensetime;

import android.content.Context;
import android.graphics.Matrix;
import android.opengl.GLES11Ext;
import android.opengl.GLES20;
import android.util.Size;

import com.ar.effects.EffectsParams;
import com.ar.effects.EffectsRender;
import com.sensetime.effects.STRender;

import java.nio.ByteBuffer;
import java.util.concurrent.Callable;

import io.agora.base.TextureBufferHelper;
import io.agora.base.VideoFrame;
import io.agora.base.internal.video.RendererCommon;
import io.agora.base.internal.video.YuvHelper;
import io.agora.rtc2.gl.EglBaseProvider;
import io.agora.rtc2.video.IVideoFrameObserver;

public class AgoraSenseTimeBeauty implements IVideoFrameObserver {

    private static final boolean useTexture = true;

    private final STRender mSTRenderer;
    private final TextureBufferHelper mTextureBufferHelper;
    private volatile boolean isReleased = false;
    private ByteBuffer nv21ByteBuffer;
    private byte[] nv21ByteArray;


    public AgoraSenseTimeBeauty(Context context) {
        mSTRenderer = new STRender(context.getApplicationContext());
        mTextureBufferHelper = TextureBufferHelper.create("AgoraSenseTimeBeauty", EglBaseProvider.instance().getRootEglBase().getEglBaseContext());
    }

    public EffectsRender getEffectsRender() {
        return mSTRenderer.getEffectsRender();
    }

    public void release() {
        isReleased = true;
        mTextureBufferHelper.invoke((Callable<Void>) () -> {
            mSTRenderer.release();
            return null;
        });
        mTextureBufferHelper.dispose();
    }


    // IVideoFrameObserver implements

    @Override
    public boolean onCaptureVideoFrame(VideoFrame videoFrame) {
        if (isReleased) {
            return false;
        }

        boolean isAcceptFrame = true;
        boolean isFrontCamera = videoFrame.getSourceType() == VideoFrame.SourceType.kFrontCamera;
        VideoFrame.Buffer buffer = videoFrame.getBuffer();

        int retTexId;
        int retImageWidth = videoFrame.getRotatedWidth();
        int retImageHeight = videoFrame.getRotatedHeight();

        if (useTexture && buffer instanceof VideoFrame.TextureBuffer) {
            VideoFrame.TextureBuffer textureBuffer = (VideoFrame.TextureBuffer) buffer;
            Size captureOriginSize = getCaptureOriginSize(textureBuffer);
            retImageWidth = captureOriginSize.getHeight();
            retImageHeight = captureOriginSize.getWidth();
            retTexId = mTextureBufferHelper.invoke(() -> mSTRenderer.preProcess(
                    textureBuffer.getTextureId(),
                    textureBuffer.getType() == VideoFrame.TextureBuffer.Type.OES ? GLES11Ext.GL_TEXTURE_EXTERNAL_OES : GLES20.GL_TEXTURE_2D,
                    captureOriginSize.getWidth(),
                    captureOriginSize.getHeight(),
                    isFrontCamera
            ));
        } else {
            VideoFrame.I420Buffer i420Buffer = buffer.toI420();

            int nv21Size = (int) (i420Buffer.getWidth() * i420Buffer.getHeight() * 3 * 1.0f / 2 + 0.5f);
            if (nv21ByteBuffer == null) {
                nv21ByteBuffer = ByteBuffer.allocateDirect(nv21Size);
            } else if (nv21ByteBuffer.capacity() != nv21Size) {
                nv21ByteBuffer.clear();
                nv21ByteBuffer = ByteBuffer.allocateDirect(nv21Size);
            }
            YuvHelper.I420ToNV12(
                    i420Buffer.getDataY(), i420Buffer.getStrideY(),
                    i420Buffer.getDataV(), i420Buffer.getStrideV(),
                    i420Buffer.getDataU(), i420Buffer.getStrideU(),
                    nv21ByteBuffer,
                    i420Buffer.getWidth(), i420Buffer.getHeight());

            if (nv21ByteArray == null || nv21ByteArray.length != nv21ByteBuffer.capacity()) {
                nv21ByteArray = new byte[nv21ByteBuffer.capacity()];
            }
            nv21ByteBuffer.position(0);
            nv21ByteBuffer.get(nv21ByteArray);
            i420Buffer.release();
            retTexId = mTextureBufferHelper.invoke(() -> mSTRenderer.preProcess(
                    nv21ByteArray,
                    EffectsParams.PixelFormat.PIX_FMT_NV21.ordinal(),
                    buffer.getWidth(),
                    buffer.getHeight(),
                    isFrontCamera
            ));
        }

        VideoFrame.TextureBuffer retBuffer = mTextureBufferHelper.wrapTextureBuffer(
                retImageWidth,
                retImageHeight,
                VideoFrame.TextureBuffer.Type.RGB,
                retTexId,
                new Matrix()
        );
        videoFrame.replaceBuffer(retBuffer, 180, videoFrame.getTimestampNs());
        return isAcceptFrame;
    }

    @Override
    public boolean onPreEncodeVideoFrame(VideoFrame videoFrame) {
        return false;
    }

    @Override
    public boolean onScreenCaptureVideoFrame(VideoFrame videoFrame) {
        return false;
    }

    @Override
    public boolean onPreEncodeScreenVideoFrame(VideoFrame videoFrame) {
        return false;
    }

    @Override
    public boolean onMediaPlayerVideoFrame(VideoFrame videoFrame, int mediaPlayerId) {
        return false;
    }

    @Override
    public boolean onRenderVideoFrame(String channelId, int uid, VideoFrame videoFrame) {
        return false;
    }

    @Override
    public int getVideoFrameProcessMode() {
        return IVideoFrameObserver.PROCESS_MODE_READ_WRITE;
    }

    @Override
    public int getVideoFormatPreference() {
        return IVideoFrameObserver.VIDEO_PIXEL_DEFAULT;
    }

    @Override
    public boolean getRotationApplied() {
        return false;
    }

    @Override
    public boolean getMirrorApplied() {
        return false;
    }

    @Override
    public int getObservedFramePosition() {
        return IVideoFrameObserver.POSITION_POST_CAPTURER;
    }

    private static Size getCaptureOriginSize(VideoFrame.TextureBuffer texBuffer){
        int width = texBuffer.getWidth();
        int height = texBuffer.getHeight();
        Matrix texMatrix = texBuffer.getTransformMatrix();

        // 根据Matrix反算纹理的真实宽高
        Matrix renderMatrix = new Matrix();
        renderMatrix.preTranslate(0.5F, 0.5F);
        renderMatrix.preScale(1.0F, -1.0F);
        renderMatrix.preTranslate(-0.5F, -0.5F);
        Matrix finalMatrix = new Matrix(texMatrix);
        finalMatrix.preConcat(renderMatrix);
        float[] finalGlMatrix = RendererCommon.convertMatrixFromAndroidGraphicsMatrix(finalMatrix);
        int texWidth = (int) (width * 1.0f / finalGlMatrix[0] + 0.5f);
        int texHeight = (int) (height * 1.0f / finalGlMatrix[5] + 0.5f);
        return new Size(texWidth, texHeight);
    }
}
