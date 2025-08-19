package io.agora.api.example.examples.advanced;

import static io.agora.api.example.common.model.Examples.ADVANCED;
import static io.agora.rtc2.Constants.RENDER_MODE_HIDDEN;
import static io.agora.rtc2.video.VideoEncoderConfiguration.STANDARD_BITRATE;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.Context;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.SurfaceView;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.FrameLayout;
import android.widget.LinearLayout;
import android.widget.RadioGroup;
import android.widget.SeekBar;
import android.widget.Spinner;
import android.widget.Switch;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import io.agora.api.example.BuildConfig;
import io.agora.api.example.MainApplication;
import io.agora.api.example.R;
import io.agora.api.example.annotation.Example;
import io.agora.api.example.common.BaseFragment;
import io.agora.api.example.examples.advanced.beauty.AgoraBeautySDK;
import io.agora.api.example.utils.CommonUtil;
import io.agora.api.example.utils.FileUtils;
import io.agora.api.example.utils.PermissonUtils;
import io.agora.api.example.utils.TokenUtils;
import io.agora.rtc2.ChannelMediaOptions;
import io.agora.rtc2.Constants;
import io.agora.rtc2.IRtcEngineEventHandler;
import io.agora.rtc2.RtcEngine;
import io.agora.rtc2.RtcEngineConfig;
import io.agora.rtc2.proxy.LocalAccessPointConfiguration;
import io.agora.rtc2.video.FaceShapeAreaOptions;
import io.agora.rtc2.video.SegmentationProperty;
import io.agora.rtc2.video.VideoCanvas;
import io.agora.rtc2.video.VideoEncoderConfiguration;
import io.agora.rtc2.video.VirtualBackgroundSource;

/**
 * The type Agora beauty.
 */
@Example(
        index = 27,
        group = ADVANCED,
        name = R.string.item_agora_beauty,
        actionId = R.id.action_mainFragment_agora_beauty,
        tipsId = R.string.agora_beauty
)
public class AgoraBeauty extends BaseFragment implements View.OnClickListener, CompoundButton.OnCheckedChangeListener, SeekBar.OnSeekBarChangeListener, AdapterView.OnItemSelectedListener {
    private static final String TAG = AgoraBeauty.class.getSimpleName();
    private boolean isProgrammaticChange = false;

    private FrameLayout fl_local, fl_remote;
    private LinearLayout controlPanel;
    private Button join, btn_save_beauty, btn_reset_beauty, btn_save_makeup, btn_reset_makeup, btn_save_filter, btn_reset_filter;
    @SuppressLint("UseSwitchCompatOrMaterialCode")
    private Switch shapeBeauty, makeUp, filter, basicBeauty, makeUpFilter, virtualBackground;
    private SeekBar sbLightness, sbRedness, sbSharpness, sbContrastStrength, sbSmoothness, sbEyePouch, sbBrightenEye, sbNasolabialFold, sbWhitenTeeth;
    // Makeup
    private SeekBar sbBrowStrength, sbLashStrength, sbShadowStrength, sbPupilStrength, sbBlushStrength, sbLipStrength;
    private Spinner spinnerFacialStyle, spinnerWocanStyle, spinnerBrowStyle, spinnerLashStyle, spinnerShadowStyle, spinnerPupilStyle, spinnerBlushStyle, spinnerLipStyle;
    private Spinner spinnerBrowColor, spinnerLashColor, spinnerBlushColor, spinnerLipColor;
    // Beauty Shape
    private SeekBar sbFacialStrength, sbWocanStrength, sbShapeBeautifyAreaIntensity, sbShapeBeautifyStyleIntensity,
            sbFaceMakeupStyleIntensity, sbMakeupFilterStrength, sbFilterStyleIntensity;
    private Spinner spinnerShapeBeautyArea, spinnerShapeBeautifyStyle, spinnerFaceMakeupStyle, spinnerFilterStyle;
    private EditText et_channel;
    private RadioGroup contrastType, virtualBgType;
    private RtcEngine engine;
    private int myUid;
    private boolean joined = false;
    private FaceShapeAreaOptions faceShapeAreaOptions = new FaceShapeAreaOptions();

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment_agora_beauty, container, false);
        return view;
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        join = view.findViewById(R.id.btn_join);
        join.setOnClickListener(this);
        et_channel = view.findViewById(R.id.et_channel);
        fl_local = view.findViewById(R.id.fl_local);
        fl_remote = view.findViewById(R.id.fl_remote);
        controlPanel = view.findViewById(R.id.controlPanel);

        // facial reshaping
        shapeBeauty = view.findViewById(R.id.switch_face_shape_beautify);
        shapeBeauty.setOnCheckedChangeListener(this);
        btn_save_beauty = view.findViewById(R.id.btn_save_beauty);
        btn_save_beauty.setOnClickListener(this);
        btn_reset_beauty = view.findViewById(R.id.btn_reset_beauty);
        btn_reset_beauty.setOnClickListener(this);
        spinnerShapeBeautyArea = view.findViewById(R.id.spinner_shape_beauty_area);
        spinnerShapeBeautyArea.setOnItemSelectedListener(this);
        sbShapeBeautifyAreaIntensity = view.findViewById(R.id.sb_shape_beautify_area_intensity);
        sbShapeBeautifyAreaIntensity.setOnSeekBarChangeListener(this);
        spinnerShapeBeautifyStyle = view.findViewById(R.id.spinner_shape_beautify_style);
        spinnerShapeBeautifyStyle.setOnItemSelectedListener(this);
        sbShapeBeautifyStyleIntensity = view.findViewById(R.id.sb_shape_beautify_style_intensity);
        sbShapeBeautifyStyleIntensity.setOnSeekBarChangeListener(this);

        // beauty makeup
        makeUp = view.findViewById(R.id.switch_face_makeup);
        makeUp.setOnCheckedChangeListener(this);
        btn_save_makeup = view.findViewById(R.id.btn_save_makeup);
        btn_save_makeup.setOnClickListener(this);
        btn_reset_makeup = view.findViewById(R.id.btn_reset_makeup);
        btn_reset_makeup.setOnClickListener(this);
        spinnerFaceMakeupStyle = view.findViewById(R.id.spinner_face_makeup_style);
        spinnerFaceMakeupStyle.setOnItemSelectedListener(this);
        sbFaceMakeupStyleIntensity = view.findViewById(R.id.sb_face_makeup_style_intensity);
        sbFaceMakeupStyleIntensity.setOnSeekBarChangeListener(this);
        makeUpFilter = view.findViewById(R.id.switch_makeup_filter);
        makeUpFilter.setOnCheckedChangeListener(this);
        sbMakeupFilterStrength = view.findViewById(R.id.sb_makeup_filter_strength);
        sbMakeupFilterStrength.setOnSeekBarChangeListener(this);

        spinnerFacialStyle = view.findViewById(R.id.spinner_facial_style);
        spinnerFacialStyle.setOnItemSelectedListener(this);
        sbFacialStrength = view.findViewById(R.id.sb_facial_strength);
        sbFacialStrength.setOnSeekBarChangeListener(this);

        spinnerWocanStyle = view.findViewById(R.id.spinner_wocan_style);
        spinnerWocanStyle.setOnItemSelectedListener(this);
        sbWocanStrength = view.findViewById(R.id.sb_wocan_strength);
        sbWocanStrength.setOnSeekBarChangeListener(this);

        spinnerBrowStyle = view.findViewById(R.id.spinner_brow_style);
        spinnerBrowStyle.setOnItemSelectedListener(this);
        spinnerBrowColor = view.findViewById(R.id.spinner_brow_color);
        spinnerBrowColor.setOnItemSelectedListener(this);
        sbBrowStrength = view.findViewById(R.id.sb_brow_strength);
        sbBrowStrength.setOnSeekBarChangeListener(this);

        spinnerLashStyle = view.findViewById(R.id.spinner_lash_style);
        spinnerLashStyle.setOnItemSelectedListener(this);
        spinnerLashColor = view.findViewById(R.id.spinner_lash_color);
        spinnerLashColor.setOnItemSelectedListener(this);
        sbLashStrength = view.findViewById(R.id.sb_lash_strength);
        sbLashStrength.setOnSeekBarChangeListener(this);

        spinnerShadowStyle = view.findViewById(R.id.spinner_shadow_style);
        spinnerShadowStyle.setOnItemSelectedListener(this);
        sbShadowStrength = view.findViewById(R.id.sb_shadow_strength);
        sbShadowStrength.setOnSeekBarChangeListener(this);

        spinnerPupilStyle = view.findViewById(R.id.spinner_pupil_style);
        spinnerPupilStyle.setOnItemSelectedListener(this);
        sbPupilStrength = view.findViewById(R.id.sb_pupil_strength);
        sbPupilStrength.setOnSeekBarChangeListener(this);

        spinnerBlushStyle = view.findViewById(R.id.spinner_blush_style);
        spinnerBlushStyle.setOnItemSelectedListener(this);
        spinnerBlushColor = view.findViewById(R.id.spinner_blush_color);
        spinnerBlushColor.setOnItemSelectedListener(this);
        sbBlushStrength = view.findViewById(R.id.sb_blush_strength);
        sbBlushStrength.setOnSeekBarChangeListener(this);

        spinnerLipStyle = view.findViewById(R.id.spinner_lip_style);
        spinnerLipStyle.setOnItemSelectedListener(this);
        spinnerLipColor = view.findViewById(R.id.spinner_lip_color);
        spinnerLipColor.setOnItemSelectedListener(this);
        sbLipStrength = view.findViewById(R.id.sb_lip_strength);
        sbLipStrength.setOnSeekBarChangeListener(this);

        // filter
        filter = view.findViewById(R.id.switch_filter);
        filter.setOnCheckedChangeListener(this);
        btn_save_filter = view.findViewById(R.id.btn_save_filter);
        btn_save_filter.setOnClickListener(this);
        btn_reset_filter = view.findViewById(R.id.btn_reset_filter);
        btn_reset_filter.setOnClickListener(this);
        spinnerFilterStyle = view.findViewById(R.id.spinner_filter_style);
        spinnerFilterStyle.setOnItemSelectedListener(this);
        sbFilterStyleIntensity = view.findViewById(R.id.sb_filter_strength);
        sbFilterStyleIntensity.setOnSeekBarChangeListener(this);

        // basic beauty
        basicBeauty = view.findViewById(R.id.switch_basic_beautify);
        basicBeauty.setOnCheckedChangeListener(this);
        sbLightness = view.findViewById(R.id.lightening);
        sbLightness.setOnSeekBarChangeListener(this);
        sbRedness = view.findViewById(R.id.redness);
        sbRedness.setOnSeekBarChangeListener(this);
        sbSmoothness = view.findViewById(R.id.smoothness);
        sbSmoothness.setOnSeekBarChangeListener(this);
        sbContrastStrength = view.findViewById(R.id.sb_contrast_strength);
        sbContrastStrength.setOnSeekBarChangeListener(this);
        sbSharpness = view.findViewById(R.id.sharpness);
        sbSharpness.setOnSeekBarChangeListener(this);

        sbEyePouch = view.findViewById(R.id.sb_eye_pouch);
        sbEyePouch.setOnSeekBarChangeListener(this);
        sbBrightenEye = view.findViewById(R.id.sb_brighten_eye);
        sbBrightenEye.setOnSeekBarChangeListener(this);
        sbNasolabialFold = view.findViewById(R.id.sb_nasolabial_fold);
        sbNasolabialFold.setOnSeekBarChangeListener(this);
        sbWhitenTeeth = view.findViewById(R.id.sb_whiten_teeth);
        sbWhitenTeeth.setOnSeekBarChangeListener(this);
        contrastType = view.findViewById(R.id.contrast_type);
        contrastType.setOnCheckedChangeListener((group, checkedId) -> {
            if (!basicBeauty.isChecked()) {
                return;
            }
            if (checkedId == R.id.contrast_low) {
                AgoraBeautySDK.getBeautyConfig().setContrast(0);
            } else if (checkedId == R.id.contrast_high) {
                AgoraBeautySDK.getBeautyConfig().setContrast(2);
            } else if (checkedId == R.id.contrast_normal) {
                AgoraBeautySDK.getBeautyConfig().setContrast(1);
            }


        });

        // Virtual Background
        virtualBackground = view.findViewById(R.id.switch_virtual_background);
        virtualBackground.setOnCheckedChangeListener(this);
        virtualBgType = view.findViewById(R.id.virtual_bg_type);
        virtualBgType.setOnCheckedChangeListener((group, checkedId) -> {
            resetVirtualBackground();
        });
    }

    /**
     * Update virtual background
     */
    private void resetVirtualBackground() {
        if (virtualBackground.isChecked()) {
            int checkedId = virtualBgType.getCheckedRadioButtonId();
            VirtualBackgroundSource backgroundSource = new VirtualBackgroundSource();
            SegmentationProperty segproperty = new SegmentationProperty();
            if (checkedId == R.id.virtual_bg_image) {
                backgroundSource.backgroundSourceType = VirtualBackgroundSource.BACKGROUND_IMG;
                String imagePath = requireContext().getExternalCacheDir().getPath();
                String imageName = "agora-logo.png";
                FileUtils.copyFilesFromAssets(getContext(), imageName, imagePath);
                backgroundSource.source = imagePath + FileUtils.SEPARATOR + imageName;
            } else if (checkedId == R.id.virtual_bg_color) {
                backgroundSource.backgroundSourceType = VirtualBackgroundSource.BACKGROUND_COLOR;
                backgroundSource.color = 0x0000EE;
            } else if (checkedId == R.id.virtual_bg_blur) {
                backgroundSource.backgroundSourceType = VirtualBackgroundSource.BACKGROUND_BLUR;
                backgroundSource.blurDegree = VirtualBackgroundSource.BLUR_DEGREE_MEDIUM;
            } else if (checkedId == R.id.virtual_bg_video) {
                backgroundSource.backgroundSourceType = VirtualBackgroundSource.BACKGROUND_VIDEO;
                backgroundSource.source = "https://agora-adc-artifacts.s3.cn-north-1.amazonaws.com.cn/resources/sample.mp4";
            }
            engine.enableVirtualBackground(true, backgroundSource, segproperty);
        } else {
            engine.enableVirtualBackground(false, null, null);
        }
    }

    @Override
    public void onActivityCreated(@Nullable Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);
        // Check if the context is valid
        Context context = getContext();
        if (context == null) {
            return;
        }
        try {
            RtcEngineConfig config = new RtcEngineConfig();
            /*
             * The context of Android Activity
             */
            config.mContext = context.getApplicationContext();
            /*
             * The App ID issued to you by Agora. See <a href="https://docs.agora.io/en/Agora%20Platform/token#get-an-app-id"> How to get the App ID</a>
             */
            config.mAppId = getString(R.string.agora_app_id);
            /* Sets the channel profile of the Agora RtcEngine.
             CHANNEL_PROFILE_COMMUNICATION(0): (Default) The Communication profile.
             Use this profile in one-on-one calls or group calls, where all users can talk freely.
             CHANNEL_PROFILE_LIVE_BROADCASTING(1): The Live-Broadcast profile. Users in a live-broadcast
             channel have a role as either broadcaster or audience. A broadcaster can both send and receive streams;
             an audience can only receive streams.*/
            config.mChannelProfile = Constants.CHANNEL_PROFILE_LIVE_BROADCASTING;
            /*
             * IRtcEngineEventHandler is an abstract class providing default implementation.
             * The SDK uses this class to report to the app on SDK runtime events.
             */
            config.mEventHandler = iRtcEngineEventHandler;
            config.mAudioScenario = Constants.AudioScenario.getValue(Constants.AudioScenario.DEFAULT);
            config.mAreaCode = ((MainApplication) getActivity().getApplication()).getGlobalSettings().getAreaCode();
            engine = RtcEngine.create(config);
            /*
             * This parameter is for reporting the usages of APIExample to agora background.
             * Generally, it is not necessary for you to set this parameter.
             */
            engine.setParameters("{"
                    + "\"rtc.report_app_scenario\":"
                    + "{"
                    + "\"appScenario\":" + 100 + ","
                    + "\"serviceType\":" + 11 + ","
                    + "\"appVersion\":\"" + RtcEngine.getSdkVersion() + "\""
                    + "}"
                    + "}");
            /* setting the local access point if the private cloud ip was set, otherwise the config will be invalid.*/
            LocalAccessPointConfiguration localAccessPointConfiguration = ((MainApplication) getActivity().getApplication()).getGlobalSettings().getPrivateCloudConfig();
            if (localAccessPointConfiguration != null) {
                // This api can only be used in the private media server scenario, otherwise some problems may occur.
                engine.setLocalAccessPoint(localAccessPointConfiguration);
            }

            //            engine.enableExtension("agora_video_filters_clear_vision", "clear_vision", true);
            //            updateExtensionProperty();
            //            updateFaceShapeBeautyStyleOptions();

            initBeautySDK();
        } catch (Exception e) {
            e.printStackTrace();
            getActivity().onBackPressed();
        }
    }

    private boolean initBeautySDK() {


        Context context = getContext();
        if (context == null) {
            return false;
        }
        return AgoraBeautySDK.initBeautySDK(context, engine);
    }

    // Todo Temporarily use the setFaceShapeAreaOptions method
    private void updateFaceShapeBeautyAreaOptions() {
        if (engine != null) {
            engine.setFaceShapeAreaOptions(faceShapeAreaOptions);
        }
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        /*leaveChannel and Destroy the RtcEngine instance*/
        if (engine != null) {
            engine.leaveChannel();
        }
        AgoraBeautySDK.unInitBeautySDK();
        handler.post(RtcEngine::destroy);
        engine = null;
    }

    private void joinChannel(String channelId) {
        // Check if the context is valid
        Context context = getContext();
        if (context == null) {
            return;
        }

        // Create render view by RtcEngine
        SurfaceView surfaceView = new SurfaceView(context);
        if (fl_local.getChildCount() > 0) {
            fl_local.removeAllViews();
        }
        // Add to the local container
        fl_local.addView(surfaceView, new FrameLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));
        // Setup local video to render your local camera preview
        engine.setupLocalVideo(new VideoCanvas(surfaceView, RENDER_MODE_HIDDEN, 0));
        // Set audio route to microPhone
        engine.setDefaultAudioRoutetoSpeakerphone(true);

        /*In the demo, the default is to enter as the anchor.*/
        engine.setClientRole(Constants.CLIENT_ROLE_BROADCASTER);
        // Enable video module
        engine.enableVideo();
        // Setup video encoding configs
        engine.setVideoEncoderConfiguration(new VideoEncoderConfiguration(
                ((MainApplication) getActivity().getApplication()).getGlobalSettings().getVideoEncodingDimensionObject(),
                VideoEncoderConfiguration.FRAME_RATE.valueOf(((MainApplication) getActivity().getApplication()).getGlobalSettings().getVideoEncodingFrameRate()),
                STANDARD_BITRATE,
                VideoEncoderConfiguration.ORIENTATION_MODE.valueOf(((MainApplication) getActivity().getApplication()).getGlobalSettings().getVideoEncodingOrientation())
        ));

        /*Please configure accessToken in the string_config file.
         * A temporary token generated in Console. A temporary token is valid for 24 hours. For details, see
         *      https://docs.agora.io/en/Agora%20Platform/token?platform=All%20Platforms#get-a-temporary-token
         * A token generated at the server. This applies to scenarios with high-security requirements. For details, see
         *      https://docs.agora.io/en/cloud-recording/token_server_java?platform=Java*/
        TokenUtils.gen(requireContext(), channelId, 0, accessToken -> {
            /* Allows a user to join a channel.
             if you do not specify the uid, we will generate the uid for you*/
            ChannelMediaOptions option = new ChannelMediaOptions();
            option.autoSubscribeAudio = true;
            option.autoSubscribeVideo = true;
            option.publishMicrophoneTrack = true;
            option.publishCameraTrack = true;
            int res = engine.joinChannel(accessToken, channelId, 0, option);
            if (res != 0) {
                // Usually happens with invalid parameters
                // Error code description can be found at:
                // en: https://docs.agora.io/en/Voice/API%20Reference/java/classio_1_1agora_1_1rtc_1_1_i_rtc_engine_event_handler_1_1_error_code.html
                // cn: https://docs.agora.io/cn/Voice/API%20Reference/java/classio_1_1agora_1_1rtc_1_1_i_rtc_engine_event_handler_1_1_error_code.html
                showAlert(RtcEngine.getErrorDescription(Math.abs(res)));
                return;
            }
            // Prevent repeated entry
            join.setEnabled(false);
        });
    }

    @Override
    public void onClick(View v) {
        int viewId = v.getId();
        if (viewId == R.id.btn_join) {
            if (!joined) {
                Activity activity = getActivity();
                if (activity == null) {
                    return;
                }
                CommonUtil.hideInputBoard(activity, et_channel);
                // call when join button hit
                String channelId = et_channel.getText().toString();
                // Check permission
                checkOrRequestPermisson(new PermissonUtils.PermissionResultCallback() {
                    @Override
                    public void onPermissionsResult(boolean allPermissionsGranted, String[] permissions, int[] grantResults) {
                        // Permissions Granted
                        if (allPermissionsGranted) {
                            joinChannel(channelId);
                        }
                    }
                });
            } else {
                joined = false;
                /*After joining a channel, the user must call the leaveChannel method to end the
                 * call before joining another channel. This method returns 0 if the user leaves the
                 * channel and releases all resources related to the call. This method call is
                 * asynchronous, and the user has not exited the channel when the method call returns.
                 * Once the user leaves the channel, the SDK triggers the onLeaveChannel callback.
                 * A successful leaveChannel method call triggers the following callbacks:
                 *      1:The local client: onLeaveChannel.
                 *      2:The remote client: onUserOffline, if the user leaving the channel is in the
                 *          Communication channel, or is a BROADCASTER in the Live Broadcast profile.
                 * @returns 0: Success.
                 *          < 0: Failure.
                 * PS:
                 *      1:If you call the destroy method immediately after calling the leaveChannel
                 *          method, the leaveChannel process interrupts, and the SDK does not trigger
                 *          the onLeaveChannel callback.
                 *      2:If you call the leaveChannel method during CDN live streaming, the SDK
                 *          triggers the removeInjectStreamUrl method.*/
                engine.leaveChannel();
                join.setText(getString(R.string.join));
                controlPanel.setVisibility(View.INVISIBLE);
            }
        } else if (viewId == R.id.btn_save_beauty) {
            AgoraBeautySDK.saveBeautyEffect();
        } else if (viewId == R.id.btn_reset_beauty) {
            AgoraBeautySDK.resetBeautyEffect();
            if (!shapeBeauty.isChecked()) {
                return;
            }
            sbShapeBeautifyStyleIntensity.setProgress(AgoraBeautySDK.getBeautyConfig().getBeautyShapeStrength());
            updateBasicBeautyOption();
        } else if (viewId == R.id.btn_save_makeup) {
            AgoraBeautySDK.saveMakeupEffect();
        } else if (viewId == R.id.btn_reset_makeup) {
            AgoraBeautySDK.resetMakeupEffect();
            if (!makeUp.isChecked()) {
                return;
            }
            sbFaceMakeupStyleIntensity.setProgress((int) (AgoraBeautySDK.getBeautyConfig().getBeautyMakeupStrength() * 10));
            updateMakeupOptionsByStyle();
        } else if (viewId == R.id.btn_save_filter) {
            AgoraBeautySDK.saveFilterEffect();
        } else if (viewId == R.id.btn_reset_filter) {
            AgoraBeautySDK.resetFilterEffect();
            if (!filter.isChecked()) {
                return;
            }
            sbFilterStyleIntensity.setProgress((int) (AgoraBeautySDK.getBeautyConfig().getFilterStrength() * 10));
        }
    }

    /**
     * Update makeup UI based on makeup type
     */
    private void updateMakeupOptionsByStyle() {
        // Makeup include filter effects
        boolean makeupFilterEnable = AgoraBeautySDK.getBeautyConfig().getMakeupFilterEnable();
        resetCheck(makeUpFilter, makeupFilterEnable);

        if (!makeUp.isChecked()) {
            return;
        }
        sbMakeupFilterStrength.setProgress((int) (AgoraBeautySDK.getBeautyConfig().getMakeupFilterStrength() * 10));

        int facialStyleValue = AgoraBeautySDK.getBeautyConfig().getFacialStyle();
        int facialPosition;
        if (facialStyleValue == 2) {
            facialPosition = 1;
        } else if (facialStyleValue == 4) {
            facialPosition = 2;
        } else if (facialStyleValue == 5) {
            facialPosition = 3;
        } else if (facialStyleValue == 6) {
            facialPosition = 4;
        } else {
            facialPosition = 0;
        }
        spinnerFacialStyle.setSelection(facialPosition);
        sbFacialStrength.setProgress((int) (AgoraBeautySDK.getBeautyConfig().getFacialStrength() * 10));

        spinnerWocanStyle.setSelection(AgoraBeautySDK.getBeautyConfig().getWocanStyle());
        sbWocanStrength.setProgress((int) (AgoraBeautySDK.getBeautyConfig().getFacialStrength() * 10));

        spinnerBrowStyle.setSelection(AgoraBeautySDK.getBeautyConfig().getBrowStyle());
        spinnerBrowColor.setSelection(AgoraBeautySDK.getBeautyConfig().getBrowColor());
        sbBrowStrength.setProgress((int) (AgoraBeautySDK.getBeautyConfig().getBrowStrength() * 10));

        int lashStyle = AgoraBeautySDK.getBeautyConfig().getLashStyle();
        int lashPosition = 0;
        if (lashStyle == 3) {
            lashPosition = 1;
        } else if (lashStyle == 5) {
            lashPosition = 2;
        }
        spinnerBrowStyle.setSelection(lashPosition);
        spinnerBrowColor.setSelection(AgoraBeautySDK.getBeautyConfig().getBrowColor());
        sbBrowStrength.setProgress((int) (AgoraBeautySDK.getBeautyConfig().getBrowStrength() * 10));

        int shadowStyle = AgoraBeautySDK.getBeautyConfig().getShadowStyle();
        int shadowPosition = 0;
        if (shadowStyle == 1) {
            shadowPosition = 1;
        } else if (lashStyle == 6) {
            shadowPosition = 2;
        }
        spinnerShadowStyle.setSelection(shadowPosition);
        sbBrowStrength.setProgress((int) (AgoraBeautySDK.getBeautyConfig().getShadowStrength() * 10));

        spinnerPupilStyle.setSelection(AgoraBeautySDK.getBeautyConfig().getPupilStyle());
        sbPupilStrength.setProgress((int) (AgoraBeautySDK.getBeautyConfig().getPupilStrength() * 10));

        int blushStyle = AgoraBeautySDK.getBeautyConfig().getBlushStyle();
        int blushPosition = 0;
        if (blushStyle == 1) {
            blushPosition = 1;
        } else if (lashStyle == 2) {
            blushPosition = 2;
        } else if (lashStyle == 4) {
            blushPosition = 3;
        } else if (lashStyle == 8) {
            blushPosition = 4;
        }
        spinnerBlushStyle.setSelection(blushPosition);
        spinnerBlushColor.setSelection(AgoraBeautySDK.getBeautyConfig().getBlushColor());
        sbBlushStrength.setProgress((int) (AgoraBeautySDK.getBeautyConfig().getBlushStrength() * 10));

        int lipStyle = AgoraBeautySDK.getBeautyConfig().getLipStyle();
        int lipPosition = 0;
        if (lipStyle == 1) {
            lipPosition = 1;
        } else if (lipStyle == 2) {
            lipPosition = 2;
        } else if (lipStyle == 3) {
            lipPosition = 3;
        } else if (lipStyle == 6) {
            lipPosition = 4;
        }
        spinnerLipStyle.setSelection(lipPosition);
        spinnerLipColor.setSelection(AgoraBeautySDK.getBeautyConfig().getLipColor());
        sbLipStrength.setProgress((int) (AgoraBeautySDK.getBeautyConfig().getLipStrength() * 10));
    }

    @SuppressLint("NonConstantResourceId")
    @Override
    public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
        switch (parent.getId()) {
            case R.id.spinner_shape_beautify_style:
                if (!shapeBeauty.isChecked()) {
                    return;
                }
                AgoraBeautySDK.getBeautyConfig().setBeautyShapeStyle(spinnerShapeBeautifyStyle.getSelectedItem().toString());
                sbShapeBeautifyStyleIntensity.setProgress(AgoraBeautySDK.getBeautyConfig().getBeautyShapeStrength());
                updateBasicBeautyOption();
                checkEnable();
                return;
            case R.id.spinner_shape_beauty_area:
                if (!shapeBeauty.isChecked()) {
                    return;
                }
                // Map spinner position to FaceShapeAreaOptions constants
                faceShapeAreaOptions.shapeArea = switch (position) {
                    case 1 -> FaceShapeAreaOptions.FACE_SHAPE_AREA_HEADSCALE;
                    case 2 -> FaceShapeAreaOptions.FACE_SHAPE_AREA_FOREHEAD;
                    case 3 -> FaceShapeAreaOptions.FACE_SHAPE_AREA_FACECONTOUR;
                    case 4 -> FaceShapeAreaOptions.FACE_SHAPE_AREA_FACELENGTH;
                    case 5 -> FaceShapeAreaOptions.FACE_SHAPE_AREA_FACEWIDTH;
                    case 6 -> FaceShapeAreaOptions.FACE_SHAPE_AREA_CHEEKBONE;
                    case 7 -> FaceShapeAreaOptions.FACE_SHAPE_AREA_CHEEK;
                    case 8 -> FaceShapeAreaOptions.FACE_SHAPE_AREA_MANDIBLE;
                    case 9 -> FaceShapeAreaOptions.FACE_SHAPE_AREA_CHIN;
                    case 10 -> FaceShapeAreaOptions.FACE_SHAPE_AREA_EYESCALE;
                    case 11 -> FaceShapeAreaOptions.FACE_SHAPE_AREA_EYEDISTANCE;
                    case 12 -> FaceShapeAreaOptions.FACE_SHAPE_AREA_EYEPOSITION;
                    case 13 -> FaceShapeAreaOptions.FACE_SHAPE_AREA_EYELID;
                    case 14 -> FaceShapeAreaOptions.FACE_SHAPE_AREA_EYEPUPILS;
                    case 15 -> FaceShapeAreaOptions.FACE_SHAPE_AREA_EYEINNERCORNER;
                    case 16 -> FaceShapeAreaOptions.FACE_SHAPE_AREA_EYEOUTERCORNER;
                    case 17 -> FaceShapeAreaOptions.FACE_SHAPE_AREA_NOSELENGTH;
                    case 18 -> FaceShapeAreaOptions.FACE_SHAPE_AREA_NOSEWIDTH;
                    case 19 -> FaceShapeAreaOptions.FACE_SHAPE_AREA_NOSEWING;
                    case 20 -> FaceShapeAreaOptions.FACE_SHAPE_AREA_NOSEROOT;
                    case 21 -> FaceShapeAreaOptions.FACE_SHAPE_AREA_NOSEBRIDGE;
                    case 22 -> FaceShapeAreaOptions.FACE_SHAPE_AREA_NOSETIP;
                    case 23 -> FaceShapeAreaOptions.FACE_SHAPE_AREA_NOSEGENERAL;
                    case 24 -> FaceShapeAreaOptions.FACE_SHAPE_AREA_MOUTHSCALE;
                    case 25 -> FaceShapeAreaOptions.FACE_SHAPE_AREA_MOUTHPOSITION;
                    case 26 -> FaceShapeAreaOptions.FACE_SHAPE_AREA_MOUTHSMILE;
                    case 27 -> FaceShapeAreaOptions.FACE_SHAPE_AREA_MOUTHLIP;
                    case 28 -> FaceShapeAreaOptions.FACE_SHAPE_AREA_EYEBROWPOSITION;
                    case 29 -> FaceShapeAreaOptions.FACE_SHAPE_AREA_EYEBROWTHICKNESS;
                    default -> FaceShapeAreaOptions.FACE_SHAPE_AREA_NONE;
                };
                //get origin beauty option params
                FaceShapeAreaOptions originOptions = engine.getFaceShapeAreaOptions(faceShapeAreaOptions.shapeArea);
                if (originOptions != null) {
                    faceShapeAreaOptions.shapeIntensity = originOptions.shapeIntensity;
                    sbShapeBeautifyAreaIntensity.setProgress(originOptions.shapeIntensity);
                }
                updateFaceShapeBeautyAreaOptions();
                return;
            case R.id.spinner_face_makeup_style:
                if (!makeUp.isChecked()) {
                    return;
                }
                AgoraBeautySDK.getBeautyConfig().setBeautyMakeupStyle(spinnerFaceMakeupStyle.getSelectedItem().toString());
                sbFaceMakeupStyleIntensity.setProgress((int) (AgoraBeautySDK.getBeautyConfig().getBeautyMakeupStrength() * 10));
                updateMakeupOptionsByStyle();
                checkEnable();
                return;
            case R.id.spinner_filter_style:
                if (!filter.isChecked()) {
                    return;
                }
                AgoraBeautySDK.getBeautyConfig().setBeautyFilter(spinnerFilterStyle.getSelectedItem().toString());
                sbFilterStyleIntensity.setProgress((int) (AgoraBeautySDK.getBeautyConfig().getFilterStrength() * 10));
                checkEnable();
                return;
            case R.id.spinner_facial_style:
                if (!makeUp.isChecked()) {
                    return;
                }
                int facialStyleValue = 0;
                if (position == 1) {
                    facialStyleValue = 2;
                } else if (position == 2) {
                    facialStyleValue = 3;
                } else if (position == 3) {
                    facialStyleValue = 5;
                } else if (position == 4) {
                    facialStyleValue = 6;
                }
                AgoraBeautySDK.getBeautyConfig().setFacialStyle(facialStyleValue);
                return;
            case R.id.spinner_wocan_style:
                if (!makeUp.isChecked()) {
                    return;
                }
                AgoraBeautySDK.getBeautyConfig().setWocanStyle(position);
                return;
            case R.id.spinner_brow_style:
                if (!makeUp.isChecked()) {
                    return;
                }
                AgoraBeautySDK.getBeautyConfig().setBrowStyle(position);
                return;
            case R.id.spinner_brow_color:
                if (!makeUp.isChecked()) {
                    return;
                }
                AgoraBeautySDK.getBeautyConfig().setBrowColor(position);
                return;
            case R.id.spinner_lash_style:
                if (!makeUp.isChecked()) {
                    return;
                }
                int lashStyleValue = 0;
                if (position == 1) {
                    lashStyleValue = 3;
                } else if (position == 2) {
                    lashStyleValue = 5;
                }
                AgoraBeautySDK.getBeautyConfig().setLashStyle(lashStyleValue);
                return;
            case R.id.spinner_lash_color:
                if (!makeUp.isChecked()) {
                    return;
                }
                AgoraBeautySDK.getBeautyConfig().setLashColor(position);
                return;
            case R.id.spinner_shadow_style:
                if (!makeUp.isChecked()) {
                    return;
                }
                int shadowStyleValue = 0;
                if (position == 1) {
                    shadowStyleValue = 1;
                } else if (position == 2) {
                    shadowStyleValue = 6;
                }
                AgoraBeautySDK.getBeautyConfig().setShadowStyle(shadowStyleValue);
                return;
            case R.id.spinner_pupil_style:
                if (!makeUp.isChecked()) {
                    return;
                }
                AgoraBeautySDK.getBeautyConfig().setPupilStyle(position);
                return;
            case R.id.spinner_blush_style:
                if (!makeUp.isChecked()) {
                    return;
                }
                int blushStyleValue = 0;
                if (position == 1) {
                    blushStyleValue = 1;
                } else if (position == 2) {
                    blushStyleValue = 2;
                } else if (position == 3) {
                    blushStyleValue = 4;
                } else if (position == 4) {
                    blushStyleValue = 9;
                }
                AgoraBeautySDK.getBeautyConfig().setBlushStyle(blushStyleValue);
                return;
            case R.id.spinner_blush_color:
                if (!makeUp.isChecked()) {
                    return;
                }
                AgoraBeautySDK.getBeautyConfig().setBlushColor(position);
                return;
            case R.id.spinner_lip_style:
                if (!makeUp.isChecked()) {
                    return;
                }
                int lipStyleValue = 0;
                if (position == 1) {
                    lipStyleValue = 1;
                } else if (position == 2) {
                    lipStyleValue = 2;
                } else if (position == 3) {
                    lipStyleValue = 3;
                } else if (position == 4) {
                    lipStyleValue = 6;
                }
                AgoraBeautySDK.getBeautyConfig().setLipStyle(lipStyleValue);
                return;
            case R.id.spinner_lip_color:
                if (!makeUp.isChecked()) {
                    return;
                }
                AgoraBeautySDK.getBeautyConfig().setLipColor(position);
                return;
            default: {

            }
        }
    }

    @Override
    public void onNothingSelected(AdapterView<?> parent) {

    }

    private void checkEnable() {
        if (BuildConfig.DEBUG) {
            boolean beautyShapeEnable = AgoraBeautySDK.getBeautyConfig().getBeautyShapeEnable();
            boolean basicBeautyEnable = AgoraBeautySDK.getBeautyConfig().getBasicBeautyEnable();
            boolean makeUpEnable = AgoraBeautySDK.getBeautyConfig().getMakeUpEnable();
            boolean makeupFilterEnable = AgoraBeautySDK.getBeautyConfig().getMakeupFilterEnable();
            boolean filterEnable = AgoraBeautySDK.getBeautyConfig().getFilterEnable();
            Log.d(TAG, "beautyShapeEnable:" + beautyShapeEnable + "\n" +
                    "basicBeautyEnable:" + basicBeautyEnable + "\n" +
                    "makeUpEnable:" + makeUpEnable + "\n" +
                    "makeupFilterEnable:" + makeupFilterEnable + "\n" +
                    "filterEnable:" + filterEnable
            );
        }
    }

    private void updateBasicBeautyOption() {
        // Beauty mode includes basic retouching
        boolean basicBeautyEnable = AgoraBeautySDK.getBeautyConfig().getBasicBeautyEnable();
        resetCheck(basicBeauty, basicBeautyEnable);

        if (basicBeautyEnable) {
            sbSmoothness.setProgress((int) (AgoraBeautySDK.getBeautyConfig().getSmoothness() * 10));
            sbLightness.setProgress((int) (AgoraBeautySDK.getBeautyConfig().getLightness() * 10));
            sbRedness.setProgress((int) (AgoraBeautySDK.getBeautyConfig().getLightness() * 10));
            sbContrastStrength.setProgress((int) (AgoraBeautySDK.getBeautyConfig().getContrastStrength() * 10));
            sbSharpness.setProgress((int) (AgoraBeautySDK.getBeautyConfig().getSharpness() * 10));

            sbEyePouch.setProgress((int) (AgoraBeautySDK.getBeautyConfig().getEyePouch() * 10));
            sbBrightenEye.setProgress((int) (AgoraBeautySDK.getBeautyConfig().getBrightenEye() * 10));
            sbNasolabialFold.setProgress((int) (AgoraBeautySDK.getBeautyConfig().getNasolabialFold() * 10));
            sbWhitenTeeth.setProgress((int) (AgoraBeautySDK.getBeautyConfig().getWhitenTeeth() * 10));

            int contract = AgoraBeautySDK.getBeautyConfig().getContrast();
            Log.d(TAG, "updateBasicBeautyOption: contract " + contract);
            if (contract == 0) {
                contrastType.check(R.id.contrast_low);
            } else if (contract == 2) {
                contrastType.check(R.id.contrast_high);
            } else {
                contrastType.check(R.id.contrast_normal);
            }
        }
    }

    @Override
    public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
        if (isProgrammaticChange) {
            return;
        }
        int id = buttonView.getId();
        if (id == R.id.switch_face_shape_beautify) {
            if (isChecked && !engine.isFeatureAvailableOnDevice(Constants.FEATURE_VIDEO_BEAUTY_EFFECT)) {
                resetCheck(buttonView, false);
                Toast.makeText(requireContext(), R.string.feature_unavailable, Toast.LENGTH_SHORT).show();
                return;
            }
            if (isChecked) {
                AgoraBeautySDK.getBeautyConfig().setBeautyShapeStyle(spinnerShapeBeautifyStyle.getSelectedItem().toString());
                sbShapeBeautifyStyleIntensity.setProgress(AgoraBeautySDK.getBeautyConfig().getBeautyShapeStrength());
                updateBasicBeautyOption();
                checkEnable();
            } else {
                AgoraBeautySDK.getBeautyConfig().setBeautyShapeStyle(null);
                resetCheck(basicBeauty, false);
            }
        } else if (id == R.id.switch_face_makeup) {
            if (isChecked && !engine.isFeatureAvailableOnDevice(Constants.FEATURE_VIDEO_BEAUTY_EFFECT)) {
                resetCheck(buttonView, false);
                Toast.makeText(requireContext(), R.string.feature_unavailable, Toast.LENGTH_SHORT).show();
                return;
            }
            if (isChecked) {
                AgoraBeautySDK.getBeautyConfig().setBeautyMakeupStyle(spinnerFaceMakeupStyle.getSelectedItem().toString());
                sbFaceMakeupStyleIntensity.setProgress((int) (AgoraBeautySDK.getBeautyConfig().getBeautyMakeupStrength() * 10));
                updateMakeupOptionsByStyle();
                checkEnable();
            } else {
                AgoraBeautySDK.getBeautyConfig().setBeautyMakeupStyle(null);
                resetCheck(makeUpFilter, false);
            }

        } else if (id == R.id.switch_makeup_filter) {
            if (isChecked && !engine.isFeatureAvailableOnDevice(Constants.FEATURE_VIDEO_BEAUTY_EFFECT)) {
                resetCheck(buttonView, false);
                Toast.makeText(requireContext(), R.string.feature_unavailable, Toast.LENGTH_SHORT).show();
                return;
            }
            if (isChecked) {
                if (!makeUp.isChecked()) {
                    // makeup disable
                    Toast.makeText(requireContext(), R.string.face_makeup_disable_tips, Toast.LENGTH_SHORT).show();
                    resetCheck(buttonView, false);
                    return;
                }
                sbMakeupFilterStrength.setProgress((int) (AgoraBeautySDK.getBeautyConfig().getMakeupFilterStrength() * 10));
            } else {
                AgoraBeautySDK.getBeautyConfig().setMakeupFilterEnable(false);
            }

        } else if (id == R.id.switch_filter) {
            if (isChecked && !engine.isFeatureAvailableOnDevice(Constants.FEATURE_VIDEO_BEAUTY_EFFECT)) {
                resetCheck(buttonView, false);
                Toast.makeText(requireContext(), R.string.feature_unavailable, Toast.LENGTH_SHORT).show();
                return;
            }
            if (isChecked) {
                AgoraBeautySDK.getBeautyConfig().setBeautyFilter(spinnerFilterStyle.getSelectedItem().toString());
                sbFilterStyleIntensity.setProgress((int) (AgoraBeautySDK.getBeautyConfig().getFilterStrength() * 10));
                checkEnable();
            } else {
                AgoraBeautySDK.getBeautyConfig().setBeautyFilter(null);
            }
        } else if (id == R.id.switch_basic_beautify) {
            if (isChecked && !engine.isFeatureAvailableOnDevice(Constants.FEATURE_VIDEO_BEAUTY_EFFECT)) {
                resetCheck(buttonView, false);
                Toast.makeText(requireContext(), R.string.feature_unavailable, Toast.LENGTH_SHORT).show();
                return;
            }
            if (isChecked) {
                AgoraBeautySDK.getBeautyConfig().setBasicBeautyEnable(true);
                updateBasicBeautyOption();
            } else {
                AgoraBeautySDK.getBeautyConfig().setBasicBeautyEnable(false);
            }
        } else if (id == virtualBackground.getId()) {
            if (isChecked && !engine.isFeatureAvailableOnDevice(Constants.FEATURE_VIDEO_VIRTUAL_BACKGROUND)) {
                resetCheck(buttonView, false);
                Toast.makeText(requireContext(), R.string.feature_unavailable, Toast.LENGTH_SHORT).show();
                return;
            }
            resetVirtualBackground();
        }
    }

    private void resetCheck(CompoundButton buttonView, boolean checked) {
        isProgrammaticChange = true;
        buttonView.setChecked(checked);
        isProgrammaticChange = false;
    }

    @Override
    public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
        Log.d(TAG, "onProgressChanged " + seekBar.getId() + " " + seekBar.getProgress());
    }

    @Override
    public void onStartTrackingTouch(SeekBar seekBar) {
        Log.d(TAG, "onStartTrackingTouch " + seekBar.getId() + " " + seekBar.getProgress());
    }

    @Override
    public void onStopTrackingTouch(SeekBar seekBar) {
        Log.d(TAG, "onStopTrackingTouch " + seekBar.getId() + " " + seekBar.getProgress());
        int progress = seekBar.getProgress();
        float value = ((float) progress) / 10;
        if (seekBar.getId() == sbShapeBeautifyStyleIntensity.getId()) {
            if (!shapeBeauty.isChecked()) {
                return;
            }
            AgoraBeautySDK.getBeautyConfig().setBeautyShapeStrength(progress);
        } else if (seekBar.getId() == sbShapeBeautifyAreaIntensity.getId()) {
            if (!shapeBeauty.isChecked()) {
                return;
            }
            faceShapeAreaOptions.shapeIntensity = progress;
            updateFaceShapeBeautyAreaOptions();
        } else if (seekBar.getId() == sbFaceMakeupStyleIntensity.getId()) {
            if (!makeUp.isChecked()) {
                return;
            }
            AgoraBeautySDK.getBeautyConfig().setBeautyMakeupStrength(value);
        } else if (seekBar.getId() == sbMakeupFilterStrength.getId()) {
            if (!makeUpFilter.isChecked()) {
                return;
            }
            AgoraBeautySDK.getBeautyConfig().setMakeupFilterStrength(value);
        } else if (seekBar.getId() == sbFacialStrength.getId()) {
            if (!makeUp.isChecked()) {
                return;
            }
            AgoraBeautySDK.getBeautyConfig().setFacialStrength(value);
        } else if (seekBar.getId() == sbWocanStrength.getId()) {
            if (!makeUp.isChecked()) {
                return;
            }
            AgoraBeautySDK.getBeautyConfig().setWocanStrength(value);
        } else if (seekBar.getId() == sbBrowStrength.getId()) {
            if (!makeUp.isChecked()) {
                return;
            }
            AgoraBeautySDK.getBeautyConfig().setBrowStrength(value);
        } else if (seekBar.getId() == sbLashStrength.getId()) {
            if (!makeUp.isChecked()) {
                return;
            }
            AgoraBeautySDK.getBeautyConfig().setLashStrength(value);
        } else if (seekBar.getId() == sbShadowStrength.getId()) {
            if (!makeUp.isChecked()) {
                return;
            }
            AgoraBeautySDK.getBeautyConfig().setShadowStrength(value);
        } else if (seekBar.getId() == sbPupilStrength.getId()) {
            if (!makeUp.isChecked()) {
                return;
            }
            AgoraBeautySDK.getBeautyConfig().setPupilStrength(value);
        } else if (seekBar.getId() == sbBlushStrength.getId()) {
            if (!makeUp.isChecked()) {
                return;
            }
            AgoraBeautySDK.getBeautyConfig().setBlushStrength(value);
        } else if (seekBar.getId() == sbLipStrength.getId()) {
            if (!makeUp.isChecked()) {
                return;
            }
            AgoraBeautySDK.getBeautyConfig().setLipStrength(value);
        } else if (seekBar.getId() == sbFilterStyleIntensity.getId()) {
            if (!filter.isChecked()) {
                return;
            }
            AgoraBeautySDK.getBeautyConfig().setFilterStrength(value);
        } else if (seekBar.getId() == sbLightness.getId()) {
            if (!basicBeauty.isChecked()) {
                return;
            }
            AgoraBeautySDK.getBeautyConfig().setLightness(value);
        } else if (seekBar.getId() == sbRedness.getId()) {
            if (!basicBeauty.isChecked()) {
                return;
            }
            AgoraBeautySDK.getBeautyConfig().setRedness(value);
        } else if (seekBar.getId() == sbSharpness.getId()) {
            if (!basicBeauty.isChecked()) {
                return;
            }
            AgoraBeautySDK.getBeautyConfig().setSharpness(value);
        } else if (seekBar.getId() == sbSmoothness.getId()) {
            if (!basicBeauty.isChecked()) {
                return;
            }
            AgoraBeautySDK.getBeautyConfig().setSmoothness(value);
        } else if (seekBar.getId() == sbContrastStrength.getId()) {
            if (!basicBeauty.isChecked()) {
                return;
            }
            AgoraBeautySDK.getBeautyConfig().setContrastStrength(value);
        } else if (seekBar.getId() == sbEyePouch.getId()) {
            if (!basicBeauty.isChecked()) {
                return;
            }
            // face_buffing_option Basic Beauty Extension
            AgoraBeautySDK.getBeautyConfig().setEyePouch(value);
        } else if (seekBar.getId() == sbBrightenEye.getId()) {
            if (!basicBeauty.isChecked()) {
                return;
            }
            AgoraBeautySDK.getBeautyConfig().setBrightenEye(value);
        } else if (seekBar.getId() == sbNasolabialFold.getId()) {
            if (!basicBeauty.isChecked()) {
                return;
            }
            AgoraBeautySDK.getBeautyConfig().setNasolabialFold(value);
        } else if (seekBar.getId() == sbWhitenTeeth.getId()) {
            if (!basicBeauty.isChecked()) {
                return;
            }
            AgoraBeautySDK.getBeautyConfig().setWhitenTeeth(value);
        }
    }


    /**
     * IRtcEngineEventHandler is an abstract class providing default implementation.
     * The SDK uses this class to report to the app on SDK runtime events.
     */
    private final IRtcEngineEventHandler iRtcEngineEventHandler = new IRtcEngineEventHandler() {
        /**
         * Error code description can be found at:
         * en: https://api-ref.agora.io/en/video-sdk/android/4.x/API/class_irtcengineeventhandler.html#callback_irtcengineeventhandler_onerror
         * cn: https://docs.agora.io/cn/video-call-4.x/API%20Reference/java_ng/API/class_irtcengineeventhandler.html#callback_irtcengineeventhandler_onerror
         */
        @Override
        public void onError(int err) {
            Log.w(TAG, String.format("onError code %d message %s", err, RtcEngine.getErrorDescription(err)));
        }

        /**Occurs when a user leaves the channel.
         * @param stats With this callback, the application retrieves the channel information,
         *              such as the call duration and statistics.*/
        @Override
        public void onLeaveChannel(RtcStats stats) {
            super.onLeaveChannel(stats);
            Log.i(TAG, String.format("local user %d leaveChannel!", myUid));
            showLongToast(String.format("local user %d leaveChannel!", myUid));
        }

        /**Occurs when the local user joins a specified channel.
         * The channel name assignment is based on channelName specified in the joinChannel method.
         * If the uid is not specified when joinChannel is called, the server automatically assigns a uid.
         * @param channel Channel name
         * @param uid User ID
         * @param elapsed Time elapsed (ms) from the user calling joinChannel until this callback is triggered*/
        @Override
        public void onJoinChannelSuccess(String channel, int uid, int elapsed) {
            Log.i(TAG, String.format("onJoinChannelSuccess channel %s uid %d", channel, uid));
            showLongToast(String.format("onJoinChannelSuccess channel %s uid %d", channel, uid));
            myUid = uid;
            joined = true;
            handler.post(new Runnable() {
                @Override
                public void run() {
                    join.setEnabled(true);
                    join.setText(getString(R.string.leave));
                    controlPanel.setVisibility(View.VISIBLE);
                }
            });
        }

        /**Since v2.9.0.
         * This callback indicates the state change of the remote audio stream.
         * PS: This callback does not work properly when the number of users (in the Communication profile) or
         *     broadcasters (in the Live-broadcast profile) in the channel exceeds 17.
         * @param uid ID of the user whose audio state changes.
         * @param state State of the remote audio
         *   REMOTE_AUDIO_STATE_STOPPED(0): The remote audio is in the default state, probably due
         *              to REMOTE_AUDIO_REASON_LOCAL_MUTED(3), REMOTE_AUDIO_REASON_REMOTE_MUTED(5),
         *              or REMOTE_AUDIO_REASON_REMOTE_OFFLINE(7).
         *   REMOTE_AUDIO_STATE_STARTING(1): The first remote audio packet is received.
         *   REMOTE_AUDIO_STATE_DECODING(2): The remote audio stream is decoded and plays normally,
         *              probably due to REMOTE_AUDIO_REASON_NETWORK_RECOVERY(2),
         *              REMOTE_AUDIO_REASON_LOCAL_UNMUTED(4) or REMOTE_AUDIO_REASON_REMOTE_UNMUTED(6).
         *   REMOTE_AUDIO_STATE_FROZEN(3): The remote audio is frozen, probably due to
         *              REMOTE_AUDIO_REASON_NETWORK_CONGESTION(1).
         *   REMOTE_AUDIO_STATE_FAILED(4): The remote audio fails to start, probably due to
         *              REMOTE_AUDIO_REASON_INTERNAL(0).
         * @param reason The reason of the remote audio state change.
         *   REMOTE_AUDIO_REASON_INTERNAL(0): Internal reasons.
         *   REMOTE_AUDIO_REASON_NETWORK_CONGESTION(1): Network congestion.
         *   REMOTE_AUDIO_REASON_NETWORK_RECOVERY(2): Network recovery.
         *   REMOTE_AUDIO_REASON_LOCAL_MUTED(3): The local user stops receiving the remote audio
         *               stream or disables the audio module.
         *   REMOTE_AUDIO_REASON_LOCAL_UNMUTED(4): The local user resumes receiving the remote audio
         *              stream or enables the audio module.
         *   REMOTE_AUDIO_REASON_REMOTE_MUTED(5): The remote user stops sending the audio stream or
         *               disables the audio module.
         *   REMOTE_AUDIO_REASON_REMOTE_UNMUTED(6): The remote user resumes sending the audio stream
         *              or enables the audio module.
         *   REMOTE_AUDIO_REASON_REMOTE_OFFLINE(7): The remote user leaves the channel.
         * @param elapsed Time elapsed (ms) from the local user calling the joinChannel method
         *                  until the SDK triggers this callback.*/
        @Override
        public void onRemoteAudioStateChanged(int uid, int state, int reason, int elapsed) {
            super.onRemoteAudioStateChanged(uid, state, reason, elapsed);
            Log.i(TAG, "onRemoteAudioStateChanged->" + uid + ", state->" + state + ", reason->" + reason);
        }

        /**Since v2.9.0.
         * Occurs when the remote video state changes.
         * PS: This callback does not work properly when the number of users (in the Communication
         *     profile) or broadcasters (in the Live-broadcast profile) in the channel exceeds 17.
         * @param uid ID of the remote user whose video state changes.
         * @param state State of the remote video:
         *   REMOTE_VIDEO_STATE_STOPPED(0): The remote video is in the default state, probably due
         *              to REMOTE_VIDEO_STATE_REASON_LOCAL_MUTED(3), REMOTE_VIDEO_STATE_REASON_REMOTE_MUTED(5),
         *              or REMOTE_VIDEO_STATE_REASON_REMOTE_OFFLINE(7).
         *   REMOTE_VIDEO_STATE_STARTING(1): The first remote video packet is received.
         *   REMOTE_VIDEO_STATE_DECODING(2): The remote video stream is decoded and plays normally,
         *              probably due to REMOTE_VIDEO_STATE_REASON_NETWORK_RECOVERY (2),
         *              REMOTE_VIDEO_STATE_REASON_LOCAL_UNMUTED(4), REMOTE_VIDEO_STATE_REASON_REMOTE_UNMUTED(6),
         *              or REMOTE_VIDEO_STATE_REASON_AUDIO_FALLBACK_RECOVERY(9).
         *   REMOTE_VIDEO_STATE_FROZEN(3): The remote video is frozen, probably due to
         *              REMOTE_VIDEO_STATE_REASON_NETWORK_CONGESTION(1) or REMOTE_VIDEO_STATE_REASON_AUDIO_FALLBACK(8).
         *   REMOTE_VIDEO_STATE_FAILED(4): The remote video fails to start, probably due to
         *              REMOTE_VIDEO_STATE_REASON_INTERNAL(0).
         * @param reason The reason of the remote video state change:
         *   REMOTE_VIDEO_STATE_REASON_INTERNAL(0): Internal reasons.
         *   REMOTE_VIDEO_STATE_REASON_NETWORK_CONGESTION(1): Network congestion.
         *   REMOTE_VIDEO_STATE_REASON_NETWORK_RECOVERY(2): Network recovery.
         *   REMOTE_VIDEO_STATE_REASON_LOCAL_MUTED(3): The local user stops receiving the remote
         *               video stream or disables the video module.
         *   REMOTE_VIDEO_STATE_REASON_LOCAL_UNMUTED(4): The local user resumes receiving the remote
         *               video stream or enables the video module.
         *   REMOTE_VIDEO_STATE_REASON_REMOTE_MUTED(5): The remote user stops sending the video
         *               stream or disables the video module.
         *   REMOTE_VIDEO_STATE_REASON_REMOTE_UNMUTED(6): The remote user resumes sending the video
         *               stream or enables the video module.
         *   REMOTE_VIDEO_STATE_REASON_REMOTE_OFFLINE(7): The remote user leaves the channel.
         *   REMOTE_VIDEO_STATE_REASON_AUDIO_FALLBACK(8): The remote media stream falls back to the
         *               audio-only stream due to poor network conditions.
         *   REMOTE_VIDEO_STATE_REASON_AUDIO_FALLBACK_RECOVERY(9): The remote media stream switches
         *               back to the video stream after the network conditions improve.
         * @param elapsed Time elapsed (ms) from the local user calling the joinChannel method until
         *               the SDK triggers this callback.*/
        @Override
        public void onRemoteVideoStateChanged(int uid, int state, int reason, int elapsed) {
            super.onRemoteVideoStateChanged(uid, state, reason, elapsed);
            Log.i(TAG, "onRemoteVideoStateChanged->" + uid + ", state->" + state + ", reason->" + reason);
        }

        /**Occurs when a remote user (Communication)/host (Live Broadcast) joins the channel.
         * @param uid ID of the user whose audio state changes.
         * @param elapsed Time delay (ms) from the local user calling joinChannel/setClientRole
         *                until this callback is triggered.*/
        @Override
        public void onUserJoined(int uid, int elapsed) {
            super.onUserJoined(uid, elapsed);
            Log.i(TAG, "onUserJoined->" + uid);
            showLongToast(String.format("user %d joined!", uid));
            /*Check if the context is correct*/
            Context context = getContext();
            if (context == null) {
                return;
            } else {
                handler.post(() -> {
                    if (fl_remote.getChildCount() > 0) {
                        fl_remote.removeAllViews();
                    }
                    /*Display remote video stream*/
                    SurfaceView surfaceView = null;
                    // Create render view by RtcEngine
                    surfaceView = new SurfaceView(context);
                    surfaceView.setZOrderMediaOverlay(true);
                    // Add to the remote container
                    fl_remote.addView(surfaceView, new FrameLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));
                    // Setup remote video to render
                    engine.setupRemoteVideo(new VideoCanvas(surfaceView, RENDER_MODE_HIDDEN, uid));
                });
            }
        }

        /**Occurs when a remote user (Communication)/host (Live Broadcast) leaves the channel.
         * @param uid ID of the user whose audio state changes.
         * @param reason Reason why the user goes offline:
         *   USER_OFFLINE_QUIT(0): The user left the current channel.
         *   USER_OFFLINE_DROPPED(1): The SDK timed out and the user dropped offline because no data
         *              packet was received within a certain period of time. If a user quits the
         *               call and the message is not passed to the SDK (due to an unreliable channel),
         *               the SDK assumes the user dropped offline.
         *   USER_OFFLINE_BECOME_AUDIENCE(2): (Live broadcast only.) The client role switched from
         *               the host to the audience.*/
        @Override
        public void onUserOffline(int uid, int reason) {
            Log.i(TAG, String.format("user %d offline! reason:%d", uid, reason));
            showLongToast(String.format("user %d offline! reason:%d", uid, reason));
            handler.post(new Runnable() {
                @Override
                public void run() {
                    /*Clear render view
                     Note: The video will stay at its last frame, to completely remove it you will need to
                     remove the SurfaceView from its parent*/
                    engine.setupRemoteVideo(new VideoCanvas(null, RENDER_MODE_HIDDEN, uid));
                }
            });
        }
    };

}
