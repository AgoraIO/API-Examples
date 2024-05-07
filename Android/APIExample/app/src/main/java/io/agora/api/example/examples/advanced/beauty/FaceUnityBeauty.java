package io.agora.api.example.examples.advanced.beauty;

import static io.agora.rtc2.video.VideoEncoderConfiguration.STANDARD_BITRATE;

import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.TextureView;
import android.view.View;
import android.view.ViewGroup;
import android.view.ViewParent;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import com.faceunity.core.faceunity.FURenderKit;

import java.lang.reflect.Method;
import java.util.Locale;
import java.util.Random;

import io.agora.api.example.MainApplication;
import io.agora.api.example.R;
import io.agora.api.example.common.BaseFragment;
import io.agora.api.example.common.widget.VideoReportLayout;
import io.agora.api.example.databinding.FragmentBeautyFaceunityBinding;
import io.agora.api.example.utils.TokenUtils;
import io.agora.beautyapi.faceunity.CameraConfig;
import io.agora.beautyapi.faceunity.CaptureMode;
import io.agora.beautyapi.faceunity.Config;
import io.agora.beautyapi.faceunity.FaceUnityBeautyAPI;
import io.agora.beautyapi.faceunity.FaceUnityBeautyAPIKt;
import io.agora.rtc2.ChannelMediaOptions;
import io.agora.rtc2.Constants;
import io.agora.rtc2.IRtcEngineEventHandler;
import io.agora.rtc2.RtcEngine;
import io.agora.rtc2.video.ColorEnhanceOptions;
import io.agora.rtc2.video.VideoCanvas;
import io.agora.rtc2.video.VideoEncoderConfiguration;

/**
 * The type Face unity beauty.
 */
public class FaceUnityBeauty extends BaseFragment {

    private FragmentBeautyFaceunityBinding mBinding;
    private RtcEngine rtcEngine;
    private String channelId;
    private VideoReportLayout mLocalVideoLayout;
    private VideoReportLayout mRemoteVideoLayout;
    private boolean isLocalFull = true;
    private IRtcEngineEventHandler mRtcEngineEventHandler;

    private final FaceUnityBeautyAPI faceUnityBeautyAPI = FaceUnityBeautyAPIKt.createFaceUnityBeautyAPI();

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        mBinding = FragmentBeautyFaceunityBinding.inflate(inflater, container, false);
        return mBinding.getRoot();
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        if (!initFaceUnity()) {
            mBinding.tvIntegrateTip.setVisibility(View.VISIBLE);
            return;
        }

        channelId = getArguments().getString(getString(R.string.key_channel_name));
        initVideoView();
        initRtcEngine();


        faceUnityBeautyAPI.initialize(new Config(requireContext(), rtcEngine, FURenderKit.getInstance(), null, CaptureMode.Agora, 0, false, new CameraConfig()));
        faceUnityBeautyAPI.enable(true);
        joinChannel();
        mBinding.switchVideoEffect.setOnCheckedChangeListener((buttonView, isChecked) -> {
            ColorEnhanceOptions options = new ColorEnhanceOptions();
            options.strengthLevel = 0.5f;
            options.skinProtectLevel = 0.5f;
            rtcEngine.setColorEnhanceOptions(isChecked, options);
        });
    }

    @Override
    public void onDestroyView() {
        super.onDestroyView();
        if (rtcEngine != null) {
            rtcEngine.leaveChannel();
        }
        faceUnityBeautyAPI.release();
        RtcEngine.destroy();
    }

    @Override
    protected void onBackPressed() {
        mBinding.fullVideoContainer.removeAllViews();
        mBinding.smallVideoContainer.removeAllViews();
        super.onBackPressed();
    }

    private void initVideoView() {
        mBinding.cbFaceBeautify.setOnCheckedChangeListener((buttonView, isChecked) -> {
            FaceUnityBeautySDK.INSTANCE.getBeautyConfig().setWhiten(
                    isChecked ? 1.0f : 0.0f
            );
        });
        mBinding.cbMakeup.setOnCheckedChangeListener((buttonView, isChecked) -> {
            if(isChecked){
                FaceUnityBeautySDK.INSTANCE.getBeautyConfig().setMakeUp(
                        new FaceUnityBeautySDK.MakeUpItem(
                                "makeup/diadiatu.bundle",
                                1.0f
                        )
                );
            }else{
                FaceUnityBeautySDK.INSTANCE.getBeautyConfig().setMakeUp(null);
            }
        });
        mBinding.cbSticker.setOnCheckedChangeListener((buttonView, isChecked) -> {
            if (isChecked) {
                FaceUnityBeautySDK.INSTANCE.getBeautyConfig().setSticker("sticker/sdlu.bundle");
            } else {
                FaceUnityBeautySDK.INSTANCE.getBeautyConfig().setSticker(null);
            }
        });
        mBinding.ivCamera.setOnClickListener(v -> {
            rtcEngine.switchCamera();
        });
        mBinding.smallVideoContainer.setOnClickListener(v -> updateVideoLayouts(!FaceUnityBeauty.this.isLocalFull));
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
            rtcEngine.enableExtension("agora_video_filters_clear_vision", "clear_vision", true);

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

    private boolean initFaceUnity() {
        try {
            Class<?> authpack = Class.forName("io.agora.api.example.examples.advanced.beauty.authpack");
            Method aMethod = authpack.getDeclaredMethod("A");
            aMethod.setAccessible(true);
            return true;
        } catch (Exception e) {
            // do nothing
        }
        return false;
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
        faceUnityBeautyAPI.setupLocalVideo(videoView, Constants.RENDER_MODE_HIDDEN);
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
                mRemoteVideoLayout.getChildAt(0).setOnClickListener(v -> updateVideoLayouts(!FaceUnityBeauty.this.isLocalFull));
                ViewParent parent = mRemoteVideoLayout.getParent();
                if (parent instanceof ViewGroup && parent != mBinding.smallVideoContainer) {
                    ((ViewGroup) parent).removeView(mRemoteVideoLayout);
                    mBinding.smallVideoContainer.addView(mRemoteVideoLayout);
                } else if (parent == null) {
                    mBinding.smallVideoContainer.addView(mRemoteVideoLayout);
                }
            }
        } else {
            if (mLocalVideoLayout != null) {
                mLocalVideoLayout.getChildAt(0).setOnClickListener(v -> updateVideoLayouts(!FaceUnityBeauty.this.isLocalFull));
                ViewParent parent = mLocalVideoLayout.getParent();
                if (parent instanceof ViewGroup && parent != mBinding.smallVideoContainer) {
                    ((ViewGroup) parent).removeView(mLocalVideoLayout);
                    mBinding.smallVideoContainer.addView(mLocalVideoLayout);
                } else if (parent == null) {
                    mBinding.smallVideoContainer.addView(mLocalVideoLayout);
                }
            }

            if (mRemoteVideoLayout != null) {
                ViewParent parent = mRemoteVideoLayout.getParent();
                if (parent instanceof ViewGroup && parent != mBinding.fullVideoContainer) {
                    ((ViewGroup) parent).removeView(mRemoteVideoLayout);
                    mBinding.fullVideoContainer.addView(mRemoteVideoLayout);
                } else if (parent == null) {
                    mBinding.fullVideoContainer.addView(mRemoteVideoLayout);
                }
            }
        }
    }

}
