package io.agora.api.example.examples.advanced.beauty;

import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.TextureView;
import android.view.View;
import android.view.ViewGroup;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import java.util.Locale;
import java.util.Random;

import io.agora.api.example.R;
import io.agora.api.example.common.BaseFragment;
import io.agora.api.example.common.widget.VideoReportLayout;
import io.agora.api.example.databinding.FragmentBeautyBytedanceBinding;
import io.agora.api.example.utils.TokenUtils;
import io.agora.base.TextureBufferHelper;
import io.agora.base.VideoFrame;
import io.agora.beauty.base.IBeautyByteDance;
import io.agora.rtc2.ChannelMediaOptions;
import io.agora.rtc2.Constants;
import io.agora.rtc2.IRtcEngineEventHandler;
import io.agora.rtc2.RtcEngine;
import io.agora.rtc2.video.IVideoFrameObserver;
import io.agora.rtc2.video.VideoCanvas;

public class ByteDanceBeauty extends BaseFragment {
    private static final String TAG = "SceneTimeBeauty";

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
    }

    @Override
    public void onDestroyView() {
        super.onDestroyView();
        isDestroyed = true;
        if (mTextureBufferHelper != null) {
            mTextureBufferHelper.invoke(() -> {
                iBeautyByteDance.release();
                iBeautyByteDance = null;
                return null;
            });
            mTextureBufferHelper.dispose();
            mTextureBufferHelper = null;
        }
        if (rtcEngine != null) {
            rtcEngine.leaveChannel();
        }
        RtcEngine.destroy();
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
                public boolean onCaptureVideoFrame(int type, VideoFrame videoFrame) {
                    if (isDestroyed) {
                        return false;
                    }
                    VideoFrame.Buffer buffer = videoFrame.getBuffer();
                    if (!(buffer instanceof VideoFrame.TextureBuffer)) {
                        return false;
                    }

                    VideoFrame.TextureBuffer texBuffer = (VideoFrame.TextureBuffer) buffer;

                    if (mTextureBufferHelper == null) {
                        doOnBeautyCreatingBegin();
                        mTextureBufferHelper = TextureBufferHelper.create("STRender", texBuffer.getEglBaseContext());
                        mTextureBufferHelper.invoke(() -> {
                            iBeautyByteDance = IBeautyByteDance.create(getContext());
                            return null;
                        });
                        doOnBeautyCreatingEnd();
                    }

                    int width = texBuffer.getWidth();
                    int height = texBuffer.getHeight();

                    Integer processTexId = mTextureBufferHelper.invoke(() -> iBeautyByteDance.process(
                            texBuffer.getTextureId(),
                            width, height, mFrameRotation
                    ));

                    // drag one frame to avoid reframe when switching camera.
                    if(mFrameRotation != videoFrame.getRotation()){
                        mFrameRotation = videoFrame.getRotation();
                        return false;
                    }

                    VideoFrame.TextureBuffer processBuffer = mTextureBufferHelper.wrapTextureBuffer(
                            width, height, VideoFrame.TextureBuffer.Type.RGB, processTexId,
                            texBuffer.getTransformMatrix());

                    videoFrame.replaceBuffer(processBuffer, mFrameRotation, videoFrame.getTimestampNs());
                    return true;
                }

                @Override
                public boolean onPreEncodeVideoFrame(int type, VideoFrame videoFrame) {
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
        rtcEngine.startPreview();

        updateVideoLayouts(isLocalFull);
    }

    private void updateVideoLayouts(boolean isLocalFull) {
        this.isLocalFull = isLocalFull;
        mBinding.fullVideoContainer.removeAllViews();
        mBinding.smallVideoContainer.removeAllViews();
        if (isLocalFull) {
            if (mLocalVideoLayout != null) {
                mBinding.fullVideoContainer.addView(mLocalVideoLayout);
            }

            if (mRemoteVideoLayout != null) {
                mRemoteVideoLayout.getChildAt(0).setOnClickListener(v -> updateVideoLayouts(!ByteDanceBeauty.this.isLocalFull));
                mBinding.smallVideoContainer.addView(mRemoteVideoLayout);
            }
        } else {
            if (mLocalVideoLayout != null) {
                mLocalVideoLayout.getChildAt(0).setOnClickListener(v -> updateVideoLayouts(!ByteDanceBeauty.this.isLocalFull));
                mBinding.smallVideoContainer.addView(mLocalVideoLayout);
            }
            if (mRemoteVideoLayout != null) {
                mBinding.fullVideoContainer.addView(mRemoteVideoLayout);
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
