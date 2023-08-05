package io.agora.api.example.examples.advanced.videoRender;

import android.graphics.Matrix;
import android.os.Handler;
import android.os.HandlerThread;
import android.util.Log;
import android.view.Surface;

import io.agora.base.TextureBuffer;
import io.agora.base.VideoFrame;
import io.agora.base.internal.ThreadUtils;
import io.agora.base.internal.video.EglBase;
import io.agora.base.internal.video.EglBaseFactory;
import io.agora.base.internal.video.GlRectDrawer;
import io.agora.base.internal.video.RendererCommon;
import io.agora.rtc2.Constants;
import io.agora.rtc2.gl.EglBaseProvider;

public class SurfaceRender {
    private static int index = 0;
    private static final String TAG = "SurfaceRender";

    private final Handler handler;
    private EglBase eglBase;
    private final GlRectDrawer drawer = new GlRectDrawer();
    private final YuvUploader yuvUploader = new YuvUploader();

    private volatile boolean isReleased = false;

    private final Surface surface;

    private int mirrorMode = Constants.VIDEO_MIRROR_MODE_AUTO;
    private int renderMode = Constants.RENDER_MODE_HIDDEN;

    private int viewportX = 0, viewportY = 0, viewportWidth = 0, viewportHeight=0;
    private int surfaceWidth = 0, surfaceHeight = 0;

    public SurfaceRender(Surface surface) {
        index++;
        HandlerThread thread = new HandlerThread("SurfaceRender-" + index);
        thread.start();
        handler = new Handler(thread.getLooper());
        this.surface = surface;
        ThreadUtils.invokeAtFrontUninterruptibly(handler, () -> {
            eglBase = EglBaseFactory.create(EglBaseProvider.instance().getRootEglBase().getEglBaseContext(), EglBase.CONFIG_PIXEL_BUFFER);
            eglBase.createSurface(surface);
            eglBase.makeCurrent();
        });
    }

    public void updateSurfaceSize(int width, int height) {
        this.surfaceWidth = width;
        this.surfaceHeight = height;
    }

    public void setMirrorMode(int mirrorMode) {
        this.mirrorMode = mirrorMode;
    }

    public void setRenderMode(int renderMode){
        this.renderMode = renderMode;
    }

    private void updateViewport(int frameWidth, int frameHeight) {
        if (renderMode == Constants.RENDER_MODE_ADAPTIVE) {
            viewportX = viewportY = 0;
            viewportWidth = surfaceWidth;
            viewportHeight = surfaceHeight;
        } else if (renderMode == Constants.RENDER_MODE_HIDDEN) {
            float frameRadio = frameHeight * 1.0f / frameWidth;
            float surfaceRadio = surfaceHeight * 1.0f / surfaceWidth;
            if(frameRadio == surfaceRadio){
                viewportX = viewportY = 0;
                viewportWidth = surfaceWidth;
                viewportHeight = surfaceHeight;
            }else if(frameRadio > surfaceRadio){
                viewportWidth = surfaceWidth;
                viewportHeight = (int) (viewportWidth * frameRadio + 0.5f);
                viewportX = 0;
                viewportY = (surfaceHeight - viewportHeight  + 1) / 2;
            }else {
                viewportHeight = surfaceHeight;
                viewportWidth = (int) (viewportHeight / frameRadio + 0.5f);
                viewportY = 0;
                viewportX = (surfaceWidth - viewportWidth + 1) / 2;
            }
        } else if (renderMode == Constants.RENDER_MODE_FIT) {
            float frameRadio = frameHeight * 1.0f / frameWidth;
            float surfaceRadio = surfaceHeight * 1.0f / surfaceWidth;
            if(frameRadio == surfaceRadio){
                viewportX = viewportY = 0;
                viewportWidth = surfaceWidth;
                viewportHeight = surfaceHeight;
            }else if(frameRadio > surfaceRadio){
                viewportHeight = surfaceHeight;
                viewportWidth = (int) (viewportHeight / frameRadio + 0.5f);
                viewportY = 0;
                viewportX = (surfaceWidth - viewportWidth + 1) / 2;
            }else {
                viewportWidth = surfaceWidth;
                viewportHeight = (int) (viewportWidth * frameRadio + 0.5f);
                viewportX = 0;
                viewportY = (surfaceHeight  - viewportHeight + 1) / 2;
            }
        }
    }

    public Surface getSurface() {
        return surface;
    }

    public boolean render(VideoFrame videoFrame) {
        if (isReleased) {
            Log.e(TAG, "The surface render has been released!");
            return false;
        }
        int frameWidth = videoFrame.getRotatedWidth();
        int frameHeight = videoFrame.getRotatedHeight();
        int frameRotation = videoFrame.getRotation();
        updateViewport(frameWidth, frameHeight);

        ThreadUtils.invokeAtFrontUninterruptibly(handler, () -> {
            VideoFrame.Buffer buffer = videoFrame.getBuffer();
            boolean mirror = false;
            if(mirrorMode == Constants.VIDEO_MIRROR_MODE_ENABLED){
                mirror = true;
            }else if(mirrorMode == Constants.VIDEO_MIRROR_MODE_AUTO){
                mirror = videoFrame.getSourceType() != VideoFrame.SourceType.kBackCamera;
            }

            if (buffer instanceof TextureBuffer) {
                TextureBuffer texBuffer = (TextureBuffer) buffer;

                float[] matrix = texBuffer.getTransformMatrixArray();
                Matrix transform = RendererCommon.convertMatrixToAndroidGraphicsMatrix(matrix);
                transform.preTranslate(0.5f, 0.5f);
                transform.preRotate(frameRotation, 0.f, 0.f);
                if (mirror) {
                    transform.preScale(-1.f, 1.f);
                }
                transform.preTranslate(-0.5f, -0.5f);
                matrix = RendererCommon.convertMatrixFromAndroidGraphicsMatrix(transform);

                if (texBuffer.getType() == VideoFrame.TextureBuffer.Type.RGB) {
                    drawer.drawRgb(
                            texBuffer.getTextureId(),
                            matrix,
                            frameWidth,
                            frameHeight,
                            viewportX, viewportY, viewportWidth, viewportHeight
                    );
                } else {
                    drawer.drawOes(
                            texBuffer.getTextureId(),
                            matrix,
                            frameWidth,
                            frameHeight,
                            viewportX, viewportY, viewportWidth, viewportHeight
                    );
                }
                eglBase.swapBuffers();

            } else {
                Matrix transform = new Matrix();
                transform.preTranslate(0.5f, 0.5f);
                transform.preRotate(frameRotation, 0.f, 0.f);
                if (mirror) {
                    transform.preScale(-1.f, 1.f);
                }
                transform.preTranslate(-0.5f, -0.5f);
                float[] matrix = RendererCommon.convertMatrixFromAndroidGraphicsMatrix(transform);

                if (buffer instanceof VideoFrame.I420Buffer) {
                    yuvUploader.uploadFromBuffer((VideoFrame.I420Buffer) buffer);
                } else {
                    VideoFrame.I420Buffer i420Buffer = buffer.toI420();
                    yuvUploader.uploadFromBuffer(i420Buffer);
                    i420Buffer.release();
                }

                drawer.drawYuv(
                        yuvUploader.getYuvTextures(),
                        matrix,
                        frameWidth,
                        frameHeight,
                        viewportX, viewportY, viewportWidth, viewportHeight
                );
                eglBase.swapBuffers();
            }
        });
        return true;
    }

    public void release() {
        if (isReleased) {
            Log.e(TAG, "The surface render has been released!");
            return;
        }
        isReleased = true;

        ThreadUtils.invokeAtFrontUninterruptibly(handler, () -> {
            drawer.release();
            yuvUploader.release();
            eglBase.release();
        });
        handler.getLooper().quit();
        try {
            handler.getLooper().getThread().join();
        } catch (InterruptedException e) {
            throw new RuntimeException(e);
        }
    }
}
