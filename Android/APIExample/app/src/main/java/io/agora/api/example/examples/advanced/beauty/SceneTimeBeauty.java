package io.agora.api.example.examples.advanced.beauty;

import static io.agora.rtc2.video.VideoEncoderConfiguration.STANDARD_BITRATE;

import android.graphics.Matrix;
import android.opengl.GLES11Ext;
import android.opengl.GLES20;
import android.os.Bundle;
import android.util.Log;
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
import java.util.concurrent.CountDownLatch;

import io.agora.api.example.MainApplication;
import io.agora.api.example.R;
import io.agora.api.example.common.BaseFragment;
import io.agora.api.example.common.widget.VideoReportLayout;
import io.agora.api.example.databinding.FragmentBeautyScenetimeBinding;
import io.agora.api.example.utils.TokenUtils;
import io.agora.base.TextureBufferHelper;
import io.agora.base.VideoFrame;
import io.agora.base.internal.video.RendererCommon;
import io.agora.base.internal.video.YuvHelper;
import io.agora.beauty.base.IBeautySenseTime;
import io.agora.rtc2.ChannelMediaOptions;
import io.agora.rtc2.Constants;
import io.agora.rtc2.IRtcEngineEventHandler;
import io.agora.rtc2.RtcEngine;
import io.agora.rtc2.gl.EglBaseProvider;
import io.agora.rtc2.video.IVideoFrameObserver;
import io.agora.rtc2.video.VideoCanvas;
import io.agora.rtc2.video.VideoEncoderConfiguration;

public class SceneTimeBeauty extends BaseFragment {
    private static final String TAG = "SceneTimeBeauty";

    private IBeautySenseTime iBeautySenseTime;
    private FragmentBeautyScenetimeBinding mBinding;
    private RtcEngine rtcEngine;
    private String channelId;

    private boolean isFrontCamera = true;

    private VideoReportLayout mLocalVideoLayout;
    private VideoReportLayout mRemoteVideoLayout;
    private boolean isLocalFull = true;
    private IVideoFrameObserver mVideoFrameObserver;
    private IRtcEngineEventHandler mRtcEngineEventHandler;

    // Beauty process require parameters
    private TextureBufferHelper mTextureBufferHelper;
    private CountDownLatch unInitBeautyLatch;
    private ByteBuffer nv21ByteBuffer;
    private byte[] nv21ByteArray;

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        mBinding = FragmentBeautyScenetimeBinding.inflate(inflater, container, false);
        return mBinding.getRoot();
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        if (!IBeautySenseTime.hasIntegrated()) {
            mBinding.tvIntegrateTip.setVisibility(View.VISIBLE);
            return;
        }

        channelId = getArguments().getString(getString(R.string.key_channel_name));
        initVideoView();
        initRtcEngine();
        joinChannel();
    }

    @Override
    public void onDestroyView() {
        super.onDestroyView();
        unInitBeauty();
        if (rtcEngine != null) {
            rtcEngine.leaveChannel();
        }
        RtcEngine.destroy();
    }

    private void initVideoView() {
        mBinding.cbFaceBeautify.setOnCheckedChangeListener((buttonView, isChecked) -> {
            if (iBeautySenseTime == null) {
                return;
            }
            iBeautySenseTime.setFaceBeautifyEnable(isChecked);
        });
        mBinding.cbMakeup.setOnCheckedChangeListener((buttonView, isChecked) -> {
            if (iBeautySenseTime == null) {
                return;
            }
            iBeautySenseTime.setMakeUpEnable(isChecked);
        });
        mBinding.cbSticker.setOnCheckedChangeListener((buttonView, isChecked) -> {
            if (iBeautySenseTime == null) {
                return;
            }
            iBeautySenseTime.setStickerEnable(isChecked);
        });
        mBinding.cbFilter.setOnCheckedChangeListener((buttonView, isChecked) -> {
            if (iBeautySenseTime == null) {
                return;
            }
            iBeautySenseTime.setFilterEnable(isChecked);
        });
        mBinding.ivCamera.setOnClickListener(v -> {
            rtcEngine.switchCamera();
            isFrontCamera = !isFrontCamera;
        });
        mBinding.smallVideoContainer.setOnClickListener(v -> updateVideoLayouts(!SceneTimeBeauty.this.isLocalFull));
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
                public boolean onCaptureVideoFrame(VideoFrame videoFrame) {
                    return processBeauty(videoFrame);
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
                    return true;
                }

                @Override
                public int getObservedFramePosition() {
                    return IVideoFrameObserver.POSITION_POST_CAPTURER;
                }
            };
            rtcEngine.registerVideoFrameObserver(mVideoFrameObserver);
            // Setup video encoding configs
            rtcEngine.setVideoEncoderConfiguration(new VideoEncoderConfiguration(
                    ((MainApplication) getActivity().getApplication()).getGlobalSettings().getVideoEncodingDimensionObject(),
                    VideoEncoderConfiguration.FRAME_RATE.valueOf(((MainApplication) getActivity().getApplication()).getGlobalSettings().getVideoEncodingFrameRate()),
                    STANDARD_BITRATE,
                    VideoEncoderConfiguration.ORIENTATION_MODE.valueOf(((MainApplication) getActivity().getApplication()).getGlobalSettings().getVideoEncodingOrientation())
            ));
            rtcEngine.enableVideo();
            rtcEngine.disableAudio();

        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private void unInitBeauty() {
        unInitBeautyLatch = new CountDownLatch(1);
        try {
            unInitBeautyLatch.wait();
        } catch (InterruptedException e) {
            throw new RuntimeException(e);
        }
        unInitBeautyLatch = null;
    }

    private boolean processBeauty(VideoFrame videoFrame) {
        if (unInitBeautyLatch != null) {
            if (mTextureBufferHelper != null) {
                mTextureBufferHelper.invoke(() -> {
                    iBeautySenseTime.release();
                    iBeautySenseTime = null;
                    return null;
                });
                mTextureBufferHelper.dispose();
                mTextureBufferHelper = null;
            }
            unInitBeautyLatch.countDown();
            return false;
        }
        VideoFrame.Buffer buffer = videoFrame.getBuffer();

        int width = buffer.getWidth();
        int height = buffer.getHeight();


        // Obtain nv21 pixel data
        int nv21Size = (int) (width * height * 3.0f / 2.0f + 0.5f);
        if (nv21ByteBuffer == null || nv21ByteBuffer.capacity() != nv21Size) {
            if (nv21ByteBuffer != null) {
                nv21ByteBuffer.clear();
            }
            nv21ByteBuffer = ByteBuffer.allocateDirect(nv21Size);
            nv21ByteArray = new byte[nv21Size];
        }
        VideoFrame.I420Buffer i420Buffer = buffer.toI420();
        YuvHelper.I420ToNV12(i420Buffer.getDataY(), i420Buffer.getStrideY(),
                i420Buffer.getDataV(), i420Buffer.getStrideV(),
                i420Buffer.getDataU(), i420Buffer.getStrideU(),
                nv21ByteBuffer, width, height);
        nv21ByteBuffer.position(0);
        nv21ByteBuffer.get(nv21ByteArray);
        i420Buffer.release();

        if (mTextureBufferHelper == null) {
            Log.d(TAG, "doOnBeautyCreatingBegin...");
            mTextureBufferHelper = TextureBufferHelper.create("STRender", EglBaseProvider.instance().getRootEglBase().getEglBaseContext());
            mTextureBufferHelper.invoke(() -> {
                iBeautySenseTime = IBeautySenseTime.create(getContext());
                return null;
            });
            Log.d(TAG, "doOnBeautyCreatingEnd.");
            runOnUIThread(() -> {
                mBinding.cbFilter.setChecked(false);
                mBinding.cbFaceBeautify.setChecked(false);
                mBinding.cbSticker.setChecked(false);
                mBinding.cbMakeup.setChecked(false);
            });
        }

        int processTexId;
        if (buffer instanceof VideoFrame.TextureBuffer) {
            VideoFrame.TextureBuffer texBuffer = (VideoFrame.TextureBuffer) buffer;
            int textureFormat = texBuffer.getType() == VideoFrame.TextureBuffer.Type.OES ? GLES11Ext.GL_TEXTURE_EXTERNAL_OES : GLES20.GL_TEXTURE_2D;
            float[] transformMatrix = RendererCommon.convertMatrixFromAndroidGraphicsMatrix(texBuffer.getTransformMatrix());

            processTexId = iBeautySenseTime.process(
                    nv21ByteArray,
                    texBuffer.getTextureId(), textureFormat,
                    width, height, videoFrame.getRotation(), transformMatrix
            );
        } else {
            processTexId = iBeautySenseTime.process(
                    nv21ByteArray,
                    width, height, videoFrame.getRotation()
            );
        }

        VideoFrame.TextureBuffer processBuffer = mTextureBufferHelper.wrapTextureBuffer(
                width, height, VideoFrame.TextureBuffer.Type.RGB, processTexId, new Matrix());
        videoFrame.replaceBuffer(processBuffer, videoFrame.getRotation(), videoFrame.getTimestampNs());
        buffer.release();
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
        rtcEngine.setupLocalVideo(new VideoCanvas(videoView, Constants.RENDER_MODE_HIDDEN));
        mLocalVideoLayout.addView(videoView);

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
                } else {
                    mBinding.fullVideoContainer.addView(mLocalVideoLayout);
                }
            }

            if (mRemoteVideoLayout != null) {
                mRemoteVideoLayout.getChildAt(0).setOnClickListener(v -> updateVideoLayouts(!SceneTimeBeauty.this.isLocalFull));
                ViewParent parent = mRemoteVideoLayout.getParent();
                if (parent instanceof ViewGroup && parent != mBinding.smallVideoContainer) {
                    ((ViewGroup) parent).removeView(mRemoteVideoLayout);
                    mBinding.smallVideoContainer.addView(mRemoteVideoLayout);
                } else {
                    mBinding.smallVideoContainer.addView(mRemoteVideoLayout);
                }
            }
        } else {
            if (mLocalVideoLayout != null) {
                mLocalVideoLayout.getChildAt(0).setOnClickListener(v -> updateVideoLayouts(!SceneTimeBeauty.this.isLocalFull));
                ViewParent parent = mLocalVideoLayout.getParent();
                if (parent instanceof ViewGroup && parent != mBinding.smallVideoContainer) {
                    ((ViewGroup) parent).removeView(mLocalVideoLayout);
                    mBinding.smallVideoContainer.addView(mLocalVideoLayout);
                } else {
                    mBinding.smallVideoContainer.addView(mLocalVideoLayout);
                }
            }

            if (mRemoteVideoLayout != null) {
                ViewParent parent = mRemoteVideoLayout.getParent();
                if (parent instanceof ViewGroup && parent != mBinding.fullVideoContainer) {
                    ((ViewGroup) parent).removeView(mRemoteVideoLayout);
                    mBinding.fullVideoContainer.addView(mRemoteVideoLayout);
                } else {
                    mBinding.fullVideoContainer.addView(mRemoteVideoLayout);
                }
            }
        }
    }

}
