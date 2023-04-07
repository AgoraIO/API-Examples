package io.agora.api.example.examples.advanced.beauty;

import android.graphics.Matrix;
import android.opengl.GLES11Ext;
import android.opengl.GLES20;
import android.os.Bundle;
import android.util.Log;
import android.util.Size;
import android.view.LayoutInflater;
import android.view.TextureView;
import android.view.View;
import android.view.ViewGroup;
import android.view.ViewParent;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import java.nio.ByteBuffer;
import java.util.Locale;
import java.util.Random;

import io.agora.api.example.R;
import io.agora.api.example.common.BaseFragment;
import io.agora.api.example.common.widget.VideoReportLayout;
import io.agora.api.example.databinding.FragmentBeautyBytedanceBinding;
import io.agora.api.example.utils.TokenUtils;
import io.agora.base.TextureBufferHelper;
import io.agora.base.VideoFrame;
import io.agora.base.internal.video.YuvHelper;
import io.agora.beauty.base.IBeautyByteDance;
import io.agora.rtc2.ChannelMediaOptions;
import io.agora.rtc2.Constants;
import io.agora.rtc2.IRtcEngineEventHandler;
import io.agora.rtc2.RtcEngine;
import io.agora.rtc2.gl.EglBaseProvider;
import io.agora.rtc2.video.ColorEnhanceOptions;
import io.agora.rtc2.video.IVideoFrameObserver;
import io.agora.rtc2.video.VideoCanvas;

public class ByteDanceBeauty extends BaseFragment {
    private static final String TAG = "SceneTimeBeauty";
    private static final Matrix IDENTITY_MATRIX = new Matrix();
    private IBeautyByteDance iBeautyByteDance;
    private FragmentBeautyBytedanceBinding mBinding;
    private RtcEngine rtcEngine;
    private String channelId;
    private boolean isFrontCamera = true;

    private TextureBufferHelper mTextureBufferHelper;

    private VideoReportLayout mLocalVideoLayout;
    private VideoReportLayout mRemoteVideoLayout;
    private boolean isLocalFull = true;
    private IVideoFrameObserver mVideoFrameObserver;
    private IRtcEngineEventHandler mRtcEngineEventHandler;

    private volatile boolean isDestroyed = false;
    private int mFrameRotation;
    private ByteBuffer nv21ByteBuffer;
    private byte[] nv21ByteArray;

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        mBinding = FragmentBeautyBytedanceBinding.inflate(inflater, container, false);
        return mBinding.getRoot();
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        if (!IBeautyByteDance.hasIntegrated()) {
            mBinding.tvIntegrateTip.setVisibility(View.VISIBLE);
            return;
        }

        channelId = getArguments().getString(getString(R.string.key_channel_name));
        initVideoView();
        initRtcEngine();
        joinChannel();
        mBinding.switchVideoEffect.setOnCheckedChangeListener((buttonView, isChecked) ->
        {
            ColorEnhanceOptions options = new ColorEnhanceOptions();
            options.strengthLevel = (float) 0.5f;
            options.skinProtectLevel = (float) 0.5f;
            rtcEngine.setColorEnhanceOptions(isChecked, options);
        });
    }

    @Override
    public void onDestroyView() {
        super.onDestroyView();
        if (rtcEngine != null) {
            rtcEngine.leaveChannel();
        }
        if (mTextureBufferHelper != null) {
            mTextureBufferHelper.invoke(() -> {
                iBeautyByteDance.release();
                iBeautyByteDance = null;
                return null;
            });
            boolean disposeSuccess = false;
            while (!disposeSuccess) {
                try {
                    mTextureBufferHelper.dispose();
                    disposeSuccess = true;
                } catch (Exception e) {
                    try {
                        Thread.sleep(50);
                    } catch (InterruptedException ex) {
                        // do nothing
                    }
                }
            }
            mTextureBufferHelper = null;
        }
        RtcEngine.destroy();
    }

    @Override
    protected void onBackPressed() {
        isDestroyed = true;
        mBinding.fullVideoContainer.removeAllViews();
        mBinding.smallVideoContainer.removeAllViews();
        super.onBackPressed();

    }

    private void initVideoView() {
        mBinding.cbFaceBeautify.setOnCheckedChangeListener((buttonView, isChecked) -> {
            if (iBeautyByteDance == null) {
                return;
            }
            iBeautyByteDance.setFaceBeautifyEnable(isChecked);
        });
        mBinding.cbMakeup.setOnCheckedChangeListener((buttonView, isChecked) -> {
            if (iBeautyByteDance == null) {
                return;
            }
            iBeautyByteDance.setMakeUpEnable(isChecked);
        });
        mBinding.cbSticker.setOnCheckedChangeListener((buttonView, isChecked) -> {
            if (iBeautyByteDance == null) {
                return;
            }
            iBeautyByteDance.setStickerEnable(isChecked);
        });
        mBinding.cbBodyBeauty.setOnCheckedChangeListener((buttonView, isChecked) -> {
            if (iBeautyByteDance == null) {
                return;
            }
            iBeautyByteDance.setBodyBeautifyEnable(isChecked);
        });
        mBinding.ivCamera.setOnClickListener(v -> {
            rtcEngine.switchCamera();
            isFrontCamera = !isFrontCamera;
        });
        mBinding.smallVideoContainer.setOnClickListener(v -> updateVideoLayouts(!ByteDanceBeauty.this.isLocalFull));
    }

    private void initRtcEngine() {
        try {
            mRtcEngineEventHandler = new IRtcEngineEventHandler() {
                @Override
                public void onError(int err) {
                    super.onError(err);
                    showLongToast(String.format(Locale.US, "msg:%s, code:%d", RtcEngine.getErrorDescription(err), err));
                }

                @Override
                public void onJoinChannelSuccess(String channel, int uid, int elapsed) {
                    super.onJoinChannelSuccess(channel, uid, elapsed);
                    mLocalVideoLayout.setReportUid(uid);
                }

                @Override
                public void onUserJoined(int uid, int elapsed) {
                    super.onUserJoined(uid, elapsed);
                    runOnUIThread(() -> {
                        if (mRemoteVideoLayout == null) {
                            mRemoteVideoLayout = new VideoReportLayout(requireContext());
                            mRemoteVideoLayout.setReportUid(uid);
                            TextureView videoView = new TextureView(requireContext());
                            rtcEngine.setupRemoteVideo(new VideoCanvas(videoView, Constants.RENDER_MODE_HIDDEN, uid));
                            mRemoteVideoLayout.addView(videoView);
                            updateVideoLayouts(isLocalFull);
                        }
                    });
                }

                @Override
                public void onUserOffline(int uid, int reason) {
                    super.onUserOffline(uid, reason);
                    runOnUIThread(() -> {
                        if (mRemoteVideoLayout != null && mRemoteVideoLayout.getReportUid() == uid) {
                            mRemoteVideoLayout.removeAllViews();
                            mRemoteVideoLayout = null;
                            updateVideoLayouts(isLocalFull);
                        }
                    });
                }

                @Override
                public void onLocalAudioStats(LocalAudioStats stats) {
                    super.onLocalAudioStats(stats);
                    runOnUIThread(() -> mLocalVideoLayout.setLocalAudioStats(stats));
                }

                @Override
                public void onLocalVideoStats(Constants.VideoSourceType source, LocalVideoStats stats) {
                    super.onLocalVideoStats(source, stats);
                    runOnUIThread(() -> mLocalVideoLayout.setLocalVideoStats(stats));
                }

                @Override
                public void onRemoteAudioStats(RemoteAudioStats stats) {
                    super.onRemoteAudioStats(stats);
                    if (mRemoteVideoLayout != null) {
                        runOnUIThread(() -> mRemoteVideoLayout.setRemoteAudioStats(stats));
                    }
                }

                @Override
                public void onRemoteVideoStats(RemoteVideoStats stats) {
                    super.onRemoteVideoStats(stats);
                    if (mRemoteVideoLayout != null) {
                        runOnUIThread(() -> mRemoteVideoLayout.setRemoteVideoStats(stats));
                    }
                }
            };
            rtcEngine = RtcEngine.create(getContext(), getString(R.string.agora_app_id), mRtcEngineEventHandler);

            if (rtcEngine == null) {
                return;
            }


            mVideoFrameObserver = new IVideoFrameObserver() {
                @Override
                public boolean onCaptureVideoFrame(int sourceType, VideoFrame videoFrame) {
                    return processBeauty(videoFrame);
                }

                @Override
                public boolean onPreEncodeVideoFrame(int sourceType, VideoFrame videoFrame) {
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
            };
            rtcEngine.registerVideoFrameObserver(mVideoFrameObserver);
            rtcEngine.enableVideo();
            rtcEngine.disableAudio();

        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private boolean processBeauty(VideoFrame videoFrame) {
        if (isDestroyed) {
            return false;
        }
        VideoFrame.Buffer buffer = videoFrame.getBuffer();
        if (mTextureBufferHelper == null) {
            doOnBeautyCreatingBegin();
            mTextureBufferHelper = TextureBufferHelper.create("ByteDanceProcess", EglBaseProvider.instance().getRootEglBase().getEglBaseContext());
            mTextureBufferHelper.invoke(() -> {
                iBeautyByteDance = IBeautyByteDance.create(getContext());
                return null;
            });
            doOnBeautyCreatingEnd();
        }

        int width = buffer.getWidth();
        int height = buffer.getHeight();


        int processTexId = -1;
        Matrix transformMatrix = IDENTITY_MATRIX;
        int rotation = videoFrame.getRotation();
        boolean skipFrame = false;
        if (buffer instanceof VideoFrame.TextureBuffer) {
            VideoFrame.TextureBuffer texBuffer = (VideoFrame.TextureBuffer) buffer;
            transformMatrix = texBuffer.getTransformMatrix();
            Size captureOriginSize = VideoCaptureUtils.getCaptureOriginSize(texBuffer);
            processTexId = mTextureBufferHelper.invoke(() -> iBeautyByteDance.process(
                    texBuffer.getTextureId(),
                    texBuffer.getType() == VideoFrame.TextureBuffer.Type.OES ? GLES11Ext.GL_TEXTURE_EXTERNAL_OES : GLES20.GL_TEXTURE_2D,
                    captureOriginSize.getWidth(), captureOriginSize.getHeight(), rotation
            ));
            if (nv21ByteBuffer != null) {
                nv21ByteBuffer.clear();
                nv21ByteBuffer = null;
                skipFrame = true;
            }
        } else {
            // Obtain nv21 pixel data
            int nv21Size = (int) (width * height * 3.0f / 2.0f + 0.5f);
            if (nv21ByteBuffer == null || nv21ByteBuffer.capacity() != nv21Size) {
                if (nv21ByteBuffer != null) {
                    nv21ByteBuffer.clear();
                }
                nv21ByteBuffer = ByteBuffer.allocateDirect(nv21Size);
                nv21ByteArray = new byte[nv21Size];
                skipFrame = true;
            }

            VideoFrame.I420Buffer i420Buffer = buffer.toI420();
            YuvHelper.I420ToNV12(i420Buffer.getDataY(), i420Buffer.getStrideY(),
                    i420Buffer.getDataV(), i420Buffer.getStrideV(),
                    i420Buffer.getDataU(), i420Buffer.getStrideU(),
                    nv21ByteBuffer, width, height);
            nv21ByteBuffer.position(0);
            nv21ByteBuffer.get(nv21ByteArray);
            i420Buffer.release();
            if(mTextureBufferHelper != null){
                processTexId = mTextureBufferHelper.invoke(() -> iBeautyByteDance.process(
                        nv21ByteArray,
                        width, height, rotation
                ));
            }
        }

        // drag one frame to avoid reframe when switching camera.
        if (mFrameRotation != rotation) {
            mFrameRotation = rotation;
            skipFrame = true;
        }

        if(processTexId < 0 || skipFrame){
            return false;
        }
        if(mTextureBufferHelper != null){
            VideoFrame.TextureBuffer processBuffer = mTextureBufferHelper.wrapTextureBuffer(
                    width, height, VideoFrame.TextureBuffer.Type.RGB, processTexId,
                    transformMatrix);
            videoFrame.replaceBuffer(processBuffer, mFrameRotation, videoFrame.getTimestampNs());
        }
        return true;
    }

    private void joinChannel() {
        int uid = new Random(System.currentTimeMillis()).nextInt(1000) + 10000;
        ChannelMediaOptions options = new ChannelMediaOptions();
        options.channelProfile = Constants.CHANNEL_PROFILE_LIVE_BROADCASTING;
        options.clientRoleType = Constants.CLIENT_ROLE_BROADCASTER;
        TokenUtils.gen(requireActivity(), channelId, uid, token -> {
            int ret = rtcEngine.joinChannel(token, channelId, uid, options);
            if (ret != Constants.ERR_OK) {
                showAlert(String.format(Locale.US, "%s\ncode:%d", RtcEngine.getErrorDescription(ret), ret));
            }
        });

        mLocalVideoLayout = new VideoReportLayout(requireContext());
        TextureView videoView = new TextureView(requireContext());
        VideoCanvas local = new VideoCanvas(videoView, Constants.RENDER_MODE_HIDDEN, 0);
        local.mirrorMode = Constants.VIDEO_MIRROR_MODE_DISABLED;
        rtcEngine.setupLocalVideo(local);
        mLocalVideoLayout.addView(videoView);
        rtcEngine.startPreview();

        updateVideoLayouts(isLocalFull);
    }

    private void updateVideoLayouts(boolean isLocalFull) {
        this.isLocalFull = isLocalFull;
        if (isLocalFull) {
            if (mLocalVideoLayout != null) {
                ViewParent parent = mLocalVideoLayout.getParent();
                if (parent instanceof ViewGroup && parent != mBinding.fullVideoContainer) {
                    ((ViewGroup) parent).removeView(mLocalVideoLayout);
                    mBinding.fullVideoContainer.addView(mLocalVideoLayout);
                } else if (parent == null) {
                    mBinding.fullVideoContainer.addView(mLocalVideoLayout);
                }
            }

            if (mRemoteVideoLayout != null) {
                mRemoteVideoLayout.getChildAt(0).setOnClickListener(v -> updateVideoLayouts(!ByteDanceBeauty.this.isLocalFull));
                ViewParent parent = mRemoteVideoLayout.getParent();
                if (parent instanceof ViewGroup && parent != mBinding.smallVideoContainer) {
                    ((ViewGroup) parent).removeView(mRemoteVideoLayout);
                    mBinding.smallVideoContainer.addView(mRemoteVideoLayout);
                } else if(parent == null){
                    mBinding.smallVideoContainer.addView(mRemoteVideoLayout);
                }
            }
        } else {
            if (mLocalVideoLayout != null) {
                mLocalVideoLayout.getChildAt(0).setOnClickListener(v -> updateVideoLayouts(!ByteDanceBeauty.this.isLocalFull));
                ViewParent parent = mLocalVideoLayout.getParent();
                if (parent instanceof ViewGroup && parent != mBinding.smallVideoContainer) {
                    ((ViewGroup) parent).removeView(mLocalVideoLayout);
                    mBinding.smallVideoContainer.addView(mLocalVideoLayout);
                } else if(parent == null){
                    mBinding.smallVideoContainer.addView(mLocalVideoLayout);
                }
            }

            if (mRemoteVideoLayout != null) {
                ViewParent parent = mRemoteVideoLayout.getParent();
                if (parent instanceof ViewGroup && parent != mBinding.fullVideoContainer) {
                    ((ViewGroup) parent).removeView(mRemoteVideoLayout);
                    mBinding.fullVideoContainer.addView(mRemoteVideoLayout);
                } else if(parent == null) {
                    mBinding.fullVideoContainer.addView(mRemoteVideoLayout);
                }
            }
        }
    }

    private void doOnBeautyCreatingBegin() {
        Log.d(TAG, "doOnBeautyCreatingBegin...");
    }

    private void doOnBeautyCreatingEnd() {
        Log.d(TAG, "doOnBeautyCreatingEnd.");
        runOnUIThread(() -> {
            mBinding.cbBodyBeauty.setChecked(false);
            mBinding.cbFaceBeautify.setChecked(false);
            mBinding.cbSticker.setChecked(false);
            mBinding.cbMakeup.setChecked(false);
        });
    }

    private void doOnBeautyReleasingBegin() {
        Log.d(TAG, "doOnBeautyReleasingBegin...");
    }

    private void doOnBeautyReleasingEnd() {
        Log.d(TAG, "doOnBeautyReleasingEnd.");
    }
}
