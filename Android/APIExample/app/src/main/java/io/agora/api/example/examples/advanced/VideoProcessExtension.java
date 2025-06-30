package io.agora.api.example.examples.advanced;

import static io.agora.api.example.common.model.Examples.ADVANCED;
import static io.agora.rtc2.Constants.RENDER_MODE_HIDDEN;
import static io.agora.rtc2.video.VideoEncoderConfiguration.STANDARD_BITRATE;

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

import io.agora.api.example.MainApplication;
import io.agora.api.example.R;
import io.agora.api.example.annotation.Example;
import io.agora.api.example.bean.MpOptions;
import io.agora.api.example.common.BaseFragment;
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
import io.agora.rtc2.video.BeautyOptions;
import io.agora.rtc2.video.ColorEnhanceOptions;
import io.agora.rtc2.video.FaceShapeAreaOptions;
import io.agora.rtc2.video.FaceShapeBeautyOptions;
import io.agora.rtc2.video.FilterEffectOptions;
import io.agora.rtc2.video.LowLightEnhanceOptions;
import io.agora.rtc2.video.SegmentationProperty;
import io.agora.rtc2.video.VideoCanvas;
import io.agora.rtc2.video.VideoDenoiserOptions;
import io.agora.rtc2.video.VideoEncoderConfiguration;
import io.agora.rtc2.video.VirtualBackgroundSource;

/**
 * This demo demonstrates how to make a VideoProcessExtension
 */
@Example(
        index = 19,
        group = ADVANCED,
        name = R.string.item_videoProcessExtension,
        actionId = R.id.action_mainFragment_video_enhancement,
        tipsId = R.string.videoEnhancement
)
public class VideoProcessExtension extends BaseFragment implements View.OnClickListener, CompoundButton.OnCheckedChangeListener, SeekBar.OnSeekBarChangeListener, AdapterView.OnItemSelectedListener {
    private static final String TAG = VideoProcessExtension.class.getSimpleName();

    private FrameLayout fl_local, fl_remote;
    private LinearLayout controlPanel;
    private Button join;
    private Switch shapeBeauty, beauty, virtualBackground, lightness2, colorful2, noiseReduce2;
    private SeekBar seek_lightness, seek_redness, seek_sharpness, seek_videoEnhance, seek_smoothness, seek_strength, seek_skin;
    // Beauty Shape
    private SeekBar sbShapeBeautifyAreaIntensity, sbShapeBeautifyStyleIntensity;
    private Spinner spinnerShapeBeautyArea, spinnerShapeBeautifyStyle;
    private EditText et_channel;
    private RadioGroup virtualBgType;
    private RtcEngine engine;
    private int myUid;
    private boolean joined = false;
    private BeautyOptions beautyOptions = new BeautyOptions();
    private FilterEffectOptions filterEffectOptions = new FilterEffectOptions();
    private FaceShapeBeautyOptions faceShapeBeautyOptions = new FaceShapeBeautyOptions();
    private FaceShapeAreaOptions faceShapeAreaOptions = new FaceShapeAreaOptions();
    private double skinProtect = 1.0;
    private double strength = 0.5;
    private VirtualBackgroundSource virtualBackgroundSource = new VirtualBackgroundSource();

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment_video_enhancement, container, false);
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
        shapeBeauty = view.findViewById(R.id.switch_face_shape_beautify);
        shapeBeauty.setOnCheckedChangeListener(this);
        beauty = view.findViewById(R.id.switch_face_beautify);
        beauty.setOnCheckedChangeListener(this);
        lightness2 = view.findViewById(R.id.switch_lightness2);
        lightness2.setOnCheckedChangeListener(this);
        colorful2 = view.findViewById(R.id.switch_color2);
        colorful2.setOnCheckedChangeListener(this);
        virtualBackground = view.findViewById(R.id.switch_virtual_background);
        virtualBackground.setOnCheckedChangeListener(this);
        noiseReduce2 = view.findViewById(R.id.switch_video_noise_reduce2);
        noiseReduce2.setOnCheckedChangeListener(this);
        seek_lightness = view.findViewById(R.id.lightening);
        seek_lightness.setOnSeekBarChangeListener(this);
        seek_redness = view.findViewById(R.id.redness);
        seek_redness.setOnSeekBarChangeListener(this);
        seek_videoEnhance = view.findViewById(R.id.sb_video_enhance);
        seek_videoEnhance.setOnSeekBarChangeListener(this);
        seek_sharpness = view.findViewById(R.id.sharpness);
        seek_sharpness.setOnSeekBarChangeListener(this);
        seek_smoothness = view.findViewById(R.id.smoothness);
        seek_smoothness.setOnSeekBarChangeListener(this);
        seek_strength = view.findViewById(R.id.strength);
        seek_strength.setOnSeekBarChangeListener(this);
        seek_skin = view.findViewById(R.id.skinProtect);
        seek_skin.setOnSeekBarChangeListener(this);

        // Beauty Shape
        sbShapeBeautifyAreaIntensity = view.findViewById(R.id.sb_shape_beautify_area_intensity);
        sbShapeBeautifyAreaIntensity.setOnSeekBarChangeListener(this);
        sbShapeBeautifyStyleIntensity = view.findViewById(R.id.sb_shape_beautify_style_intensity);
        sbShapeBeautifyStyleIntensity.setOnSeekBarChangeListener(this);

        spinnerShapeBeautyArea = view.findViewById(R.id.spinner_shape_beauty_area);
        spinnerShapeBeautyArea.setOnItemSelectedListener(this);
        spinnerShapeBeautifyStyle = view.findViewById(R.id.spinner_shape_beautify_style);
        spinnerShapeBeautifyStyle.setOnItemSelectedListener(this);

        virtualBgType = view.findViewById(R.id.virtual_bg_type);
        virtualBgType.setOnCheckedChangeListener((group, checkedId) -> {
            resetVirtualBackground();
        });


    }

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

            engine.enableExtension("agora_video_filters_clear_vision", "clear_vision", true);
            updateFaceShapeBeautyStyleOptions();
        } catch (Exception e) {
            e.printStackTrace();
            getActivity().onBackPressed();
        }
    }

    private void updateFaceShapeBeautyAreaOptions() {
        if (engine != null) {
            engine.setFaceShapeAreaOptions(faceShapeAreaOptions);
        }
    }


    private void updateFaceShapeBeautyStyleOptions() {
        if (engine != null) {
            engine.setFaceShapeBeautyOptions(shapeBeauty.isChecked(), faceShapeBeautyOptions);
        }
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        /*leaveChannel and Destroy the RtcEngine instance*/
        if (engine != null) {
            engine.leaveChannel();
        }
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
        if (v.getId() == R.id.btn_join) {
            if (!joined) {
                CommonUtil.hideInputBoard(getActivity(), et_channel);
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
        }
    }

    @Override
    public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {

        switch (parent.getId()) {
            case R.id.spinner_shape_beauty_area:
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
            case R.id.spinner_shape_beautify_style:
                faceShapeBeautyOptions.shapeStyle = position;
                updateFaceShapeBeautyStyleOptions();
                return;
        }
    }

    @Override
    public void onNothingSelected(AdapterView<?> parent) {

    }

    @Override
    public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
        if (buttonView.getId() == shapeBeauty.getId()) {
            if (isChecked && !engine.isFeatureAvailableOnDevice(Constants.FEATURE_VIDEO_BEAUTY_EFFECT)) {
                buttonView.setChecked(false);
                Toast.makeText(requireContext(), R.string.feature_unavailable, Toast.LENGTH_SHORT).show();
                return;
            }
            updateFaceShapeBeautyStyleOptions();
        }  else if (buttonView.getId() == beauty.getId()) {
            if (isChecked && !engine.isFeatureAvailableOnDevice(Constants.FEATURE_VIDEO_BEAUTY_EFFECT)) {
                buttonView.setChecked(false);
                Toast.makeText(requireContext(), R.string.feature_unavailable, Toast.LENGTH_SHORT).show();
                return;
            }

            engine.setBeautyEffectOptions(isChecked, beautyOptions);
            engine.setFilterEffectOptions(isChecked, filterEffectOptions);
        } else if (buttonView.getId() == lightness2.getId()) {
            LowLightEnhanceOptions options = new LowLightEnhanceOptions();
            options.lowlightEnhanceLevel = LowLightEnhanceOptions.LOW_LIGHT_ENHANCE_LEVEL_HIGH_QUALITY;
            options.lowlightEnhanceMode = LowLightEnhanceOptions.LOW_LIGHT_ENHANCE_MANUAL;
            engine.setLowlightEnhanceOptions(isChecked, options);
        } else if (buttonView.getId() == colorful2.getId()) {
            setColorEnhance(isChecked);
        } else if (buttonView.getId() == virtualBackground.getId()) {
            if (isChecked && !engine.isFeatureAvailableOnDevice(Constants.FEATURE_VIDEO_VIRTUAL_BACKGROUND)) {
                buttonView.setChecked(false);
                Toast.makeText(requireContext(), R.string.feature_unavailable, Toast.LENGTH_SHORT).show();
                return;
            }
            resetVirtualBackground();
        } else if (buttonView.getId() == noiseReduce2.getId()) {
            VideoDenoiserOptions options = new VideoDenoiserOptions();
            options.denoiserLevel = VideoDenoiserOptions.VIDEO_DENOISER_LEVEL_HIGH_QUALITY;
            options.denoiserMode = VideoDenoiserOptions.VIDEO_DENOISER_AUTO;
            engine.setVideoDenoiserOptions(isChecked, options);
        }
    }

    private void setColorEnhance(boolean isChecked) {
        ColorEnhanceOptions options = new ColorEnhanceOptions();
        options.strengthLevel = (float) strength;
        options.skinProtectLevel = (float) skinProtect;
        engine.setColorEnhanceOptions(isChecked, options);
    }

    @Override
    public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
        float value = ((float) progress) / 10;
        if (seekBar.getId() == sbShapeBeautifyAreaIntensity.getId()) {
            faceShapeAreaOptions.shapeIntensity = progress;
            updateFaceShapeBeautyAreaOptions();
        } else if (seekBar.getId() == sbShapeBeautifyStyleIntensity.getId()) {
            faceShapeBeautyOptions.styleIntensity = progress;
            updateFaceShapeBeautyStyleOptions();
        } else if (seekBar.getId() == seek_lightness.getId()) {
            beautyOptions.lighteningLevel = value;
            engine.setBeautyEffectOptions(beauty.isChecked(), beautyOptions);
        } else if (seekBar.getId() == seek_redness.getId()) {
            beautyOptions.rednessLevel = value;
            engine.setBeautyEffectOptions(beauty.isChecked(), beautyOptions);
        } else if (seekBar.getId() == seek_sharpness.getId()) {
            beautyOptions.sharpnessLevel = value;
            engine.setBeautyEffectOptions(beauty.isChecked(), beautyOptions);
        } else if (seekBar.getId() == seek_videoEnhance.getId()) {
            filterEffectOptions.strength = value;
            filterEffectOptions.path = "built_in_whiten_filter";
            int ret = engine.setFilterEffectOptions(beauty.isChecked(), filterEffectOptions);
            Log.d(TAG, "onProgressChanged: ret=" + ret);
        } else if (seekBar.getId() == seek_smoothness.getId()) {
            beautyOptions.smoothnessLevel = value;
            engine.setBeautyEffectOptions(beauty.isChecked(), beautyOptions);
        } else if (seekBar.getId() == seek_strength.getId()) {
            strength = value;
            setColorEnhance(colorful2.isChecked());
        } else if (seekBar.getId() == seek_skin.getId()) {
            skinProtect = value;
            setColorEnhance(colorful2.isChecked());
        }
    }

    @Override
    public void onStartTrackingTouch(SeekBar seekBar) {

    }

    @Override
    public void onStopTrackingTouch(SeekBar seekBar) {

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
