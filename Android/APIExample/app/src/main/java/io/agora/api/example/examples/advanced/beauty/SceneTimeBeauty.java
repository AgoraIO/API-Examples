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

import com.sensetime.stmobile.model.STMobileMakeupType;

import java.io.File;
import java.io.IOException;
import java.util.Locale;
import java.util.Random;

import io.agora.api.example.MainApplication;
import io.agora.api.example.R;
import io.agora.api.example.common.BaseFragment;
import io.agora.api.example.common.widget.VideoReportLayout;
import io.agora.api.example.databinding.FragmentBeautyScenetimeBinding;
import io.agora.api.example.utils.TokenUtils;
import io.agora.beautyapi.sensetime.BeautyPreset;
import io.agora.beautyapi.sensetime.CaptureMode;
import io.agora.beautyapi.sensetime.Config;
import io.agora.beautyapi.sensetime.SenseTimeBeautyAPI;
import io.agora.beautyapi.sensetime.SenseTimeBeautyAPIKt;
import io.agora.beautyapi.sensetime.utils.STRenderKit;
import io.agora.beautyapi.sensetime.utils.utils.FileUtils;
import io.agora.rtc2.ChannelMediaOptions;
import io.agora.rtc2.Constants;
import io.agora.rtc2.IRtcEngineEventHandler;
import io.agora.rtc2.RtcEngine;
import io.agora.rtc2.video.ColorEnhanceOptions;
import io.agora.rtc2.video.VideoCanvas;
import io.agora.rtc2.video.VideoEncoderConfiguration;

public class SceneTimeBeauty extends BaseFragment {
    private static final String TAG = "SceneTimeBeauty";

    private FragmentBeautyScenetimeBinding mBinding;
    private RtcEngine rtcEngine;
    private String channelId;

    private boolean isFrontCamera = true;

    private VideoReportLayout mLocalVideoLayout;
    private VideoReportLayout mRemoteVideoLayout;
    private boolean isLocalFull = true;
    private IRtcEngineEventHandler mRtcEngineEventHandler;

    private final SenseTimeBeautyAPI senseTimeBeautyAPI = SenseTimeBeautyAPIKt.createSenseTimeBeautyAPI();

    private STRenderKit stRenderKit;

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        mBinding = FragmentBeautyScenetimeBinding.inflate(inflater, container, false);
        return mBinding.getRoot();
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        boolean hasResource = false;
        try {
            hasResource = requireActivity().getAssets().list("beauty_sensetime").length > 1;
        } catch (IOException e) {
            // do nothing
        }
        if (!hasResource) {
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

        stRenderKit = new STRenderKit(requireContext(), "beauty_sensetime");
        senseTimeBeautyAPI.initialize(new Config(
                rtcEngine,
                stRenderKit,
                null,
                CaptureMode.Agora,
                0,
                false
        ));
        senseTimeBeautyAPI.enable(true);
    }


    @Override
    public void onDestroyView() {
        super.onDestroyView();
        if (rtcEngine != null) {
            rtcEngine.leaveChannel();
        }
        senseTimeBeautyAPI.release();
        if (stRenderKit != null) {
            stRenderKit.release();
        }
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
            senseTimeBeautyAPI.setBeautyPreset(isChecked? BeautyPreset.DEFAULT: BeautyPreset.CUSTOM);
        });
        mBinding.cbMakeup.setOnCheckedChangeListener((buttonView, isChecked) -> {
            int type = STMobileMakeupType.ST_MAKEUP_TYPE_LIP;
            if(isChecked){
                String className = "makeup_lip";
                String fileName = "12自然.zip";
                String path = FileUtils.getFilePath(requireContext(), stRenderKit.getResourcePath(className) + File.separator + fileName);
                FileUtils.copyFileIfNeed(requireContext(), fileName, stRenderKit.getResourcePath(className));
                float strength = 1.0f;
                stRenderKit.setMakeupForType(type, path);
                stRenderKit.setMakeupStrength(type, strength);
            }else{
                stRenderKit.removeMakeupByType(type);
            }
        });
        mBinding.cbSticker.setOnCheckedChangeListener((buttonView, isChecked) -> {
            String className = "sticker_face_shape";
            String fileName = "ShangBanLe.zip";
            String path = FileUtils.getFilePath(requireContext(), stRenderKit.getResourcePath(className) + File.separator + fileName);
            FileUtils.copyFileIfNeed(requireContext(), fileName, stRenderKit.getResourcePath(className));
            if(isChecked){
                stRenderKit.changeSticker(path);
            }else{
                stRenderKit.removeSticker(path);
            }

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
                mRemoteVideoLayout.getChildAt(0).setOnClickListener(v -> updateVideoLayouts(!SceneTimeBeauty.this.isLocalFull));
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
                mLocalVideoLayout.getChildAt(0).setOnClickListener(v -> updateVideoLayouts(!SceneTimeBeauty.this.isLocalFull));
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



}
