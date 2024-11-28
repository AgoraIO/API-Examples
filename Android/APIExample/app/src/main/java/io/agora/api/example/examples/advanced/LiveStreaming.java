package io.agora.api.example.examples.advanced;

import static io.agora.api.example.common.model.Examples.ADVANCED;
import static io.agora.rtc2.Constants.CLIENT_ROLE_AUDIENCE;
import static io.agora.rtc2.video.CameraCapturerConfiguration.CAMERA_DIRECTION;
import static io.agora.rtc2.video.CameraCapturerConfiguration.CAMERA_FOCAL_LENGTH_TYPE;
import static io.agora.rtc2.video.VideoEncoderConfiguration.STANDARD_BITRATE;

import android.content.Context;
import android.graphics.Color;
import android.os.Bundle;
import android.text.TextUtils;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.SurfaceView;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.FrameLayout;
import android.widget.SeekBar;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AlertDialog;

import com.google.android.material.bottomsheet.BottomSheetDialog;

import java.io.File;
import java.util.ArrayList;
import java.util.Locale;
import java.util.Random;

import io.agora.api.example.MainApplication;
import io.agora.api.example.R;
import io.agora.api.example.annotation.Example;
import io.agora.api.example.common.BaseFragment;
import io.agora.api.example.common.Constant;
import io.agora.api.example.common.widget.VideoReportLayout;
import io.agora.api.example.databinding.FragmentLiveStreamingBinding;
import io.agora.api.example.databinding.FragmentLiveStreamingSettingBinding;
import io.agora.api.example.databinding.FragmentLiveStreamingVideoTrackingBinding;
import io.agora.api.example.utils.CommonUtil;
import io.agora.api.example.utils.FileUtils;
import io.agora.api.example.utils.PermissonUtils;
import io.agora.api.example.utils.TokenUtils;
import io.agora.rtc2.ChannelMediaOptions;
import io.agora.rtc2.ClientRoleOptions;
import io.agora.rtc2.Constants;
import io.agora.rtc2.IRtcEngineEventHandler;
import io.agora.rtc2.RtcEngine;
import io.agora.rtc2.RtcEngineConfig;
import io.agora.rtc2.proxy.LocalAccessPointConfiguration;
import io.agora.rtc2.video.AgoraFocalLengthInfo;
import io.agora.rtc2.video.CameraCapturerConfiguration;
import io.agora.rtc2.video.ImageTrackOptions;
import io.agora.rtc2.video.SnapshotConfig;
import io.agora.rtc2.video.VideoCanvas;
import io.agora.rtc2.video.VideoEncoderConfiguration;
import io.agora.rtc2.video.WatermarkOptions;

/**
 * This demo demonstrates how to make a one-to-one video call
 * <p>
 * By default, Everyone is a host, entered a channel will see yourself in the background( the big one ).
 * click the frame will switch the position.
 * When turn the Co-host on, others will see you.
 */
@Example(
        index = 0,
        group = ADVANCED,
        name = R.string.item_livestreaming,
        actionId = R.id.action_mainFragment_to_live_streaming,
        tipsId = R.string.livestreaming
)
public class LiveStreaming extends BaseFragment implements View.OnClickListener, SeekBar.OnSeekBarChangeListener, AdapterView.OnItemSelectedListener {
    private static final String TAG = LiveStreaming.class.getSimpleName();
    private static final int FPS_DEFAULT = 15;
    public Constants.VideoModulePosition position = Constants.VideoModulePosition.VIDEO_MODULE_POSITION_PRE_RENDERER;
    private FragmentLiveStreamingBinding mRootBinding;
    private FragmentLiveStreamingSettingBinding mSettingBinding;
    private BottomSheetDialog mSettingDialog;

    private VideoReportLayout foreGroundVideo, backGroundVideo;
    private boolean isLocalVideoForeground;
    private int localFps = 0;
    private int remoteFps = 0;

    private RtcEngine engine;
    private int myUid = 0;
    private String myToken;
    private int remoteUid;
    private boolean joined = false;
    private boolean isHost = false;
    private boolean isPreloaded = false;
    private int canvasBgColor = 0x0000ffff; // RGBA
    private int canvasRenderMode = Constants.RENDER_MODE_HIDDEN;
    private final VideoEncoderConfiguration videoEncoderConfiguration = new VideoEncoderConfiguration();
    private AgoraFocalLengthInfo[] agoraFocalLengthInfos;

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        mRootBinding = FragmentLiveStreamingBinding.inflate(inflater, container, false);
        return mRootBinding.getRoot();
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        foreGroundVideo = mRootBinding.foregroundLayout.foregroundVideo;
        backGroundVideo = mRootBinding.backgroundVideo;
        mRootBinding.btnSetting.setOnClickListener(this);
        mRootBinding.btnJoin.setOnClickListener(this);
        mRootBinding.btnPreload.setOnClickListener(this);
        mRootBinding.btnPublish.setOnClickListener(this);
        mRootBinding.btnLocalScreenshot.setOnClickListener(this);
        mRootBinding.btnRemoteScreenshot.setOnClickListener(this);
        mRootBinding.btnSwitchCamera.setOnClickListener(this);
        foreGroundVideo.setOnClickListener(this);

        mSettingBinding = FragmentLiveStreamingSettingBinding.inflate(LayoutInflater.from(getContext()));
        mSettingBinding.switchWatermark.setOnCheckedChangeListener((buttonView, isChecked) -> enableWatermark(isChecked));
        mSettingBinding.switchBFrame.setOnCheckedChangeListener((buttonView, isChecked) -> enableBFrame(isChecked));
        mSettingBinding.switchLowLatency.setOnCheckedChangeListener((buttonView, isChecked) -> enableLowLegacy(isChecked));
        mSettingBinding.switchLowStream.setOnCheckedChangeListener((buttonView, isChecked) -> enableLowStream(isChecked));
        mSettingBinding.switchFirstFrame.setOnCheckedChangeListener((buttonView, isChecked) -> {
            if (isChecked) {
                new AlertDialog.Builder(requireContext())
                        .setTitle(R.string.tip)
                        .setMessage(R.string.first_frame_optimization_tip)
                        .setNegativeButton(R.string.cancel, (dialog, which) -> {
                            buttonView.setChecked(false);
                            dialog.dismiss();
                        })
                        .setPositiveButton(R.string.confirm, (dialog, which) -> {
                            // Enable FirstFrame Optimization
                            engine.enableInstantMediaRendering();
                            buttonView.setEnabled(false);
                            dialog.dismiss();
                        })
                        .show();
            }
        });
        mSettingBinding.spEncoderType.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                setEncodingPreference(position);
            }

            @Override
            public void onNothingSelected(AdapterView<?> parent) {

            }
        });
        mSettingBinding.spRenderMode.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                if (getString(R.string.render_mode_hidden).equals(parent.getSelectedItem())) {
                    canvasRenderMode = Constants.RENDER_MODE_HIDDEN;
                } else if (getString(R.string.render_mode_fit).equals(parent.getSelectedItem())) {
                    canvasRenderMode = Constants.RENDER_MODE_FIT;
                } else if (getString(R.string.render_mode_adaptive).equals(parent.getSelectedItem())) {
                    canvasRenderMode = Constants.RENDER_MODE_ADAPTIVE;
                }
                updateVideoView();
            }

            @Override
            public void onNothingSelected(AdapterView<?> parent) {

            }
        });
        mSettingBinding.sbColor.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                int color = Color.argb(255, progress, progress, 255 - progress);
                canvasBgColor = (Color.red(color) << 24) | (Color.green(color) << 16) | (Color.blue(color) << 8) | Color.alpha(color);
                mSettingBinding.vColor.setBackgroundColor(color);
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
                updateVideoView();
            }
        });
        mSettingBinding.switchVideoImage.setOnCheckedChangeListener((buttonView, isChecked) -> {
            if (!isHost && isChecked) {
                showShortToast("Please join channel with broadcast role firstly.");
                buttonView.setChecked(false);
                return;
            }
            String path = requireContext().getExternalCacheDir().getPath();
            String fileName = "bg_blue.png";
            FileUtils.copyFilesFromAssets(requireContext(), fileName, path);
            engine.enableVideoImageSource(isChecked, new ImageTrackOptions(
                    path + File.separator + fileName,
                    15
            ));
        });
        mSettingBinding.spCamera.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                AgoraFocalLengthInfo info = agoraFocalLengthInfos[position];
                CAMERA_DIRECTION direction = CAMERA_DIRECTION.CAMERA_FRONT;
                if (info.cameraDirection == CAMERA_DIRECTION.CAMERA_REAR.getValue()) {
                    direction = CAMERA_DIRECTION.CAMERA_REAR;
                } else if (info.cameraDirection == CAMERA_DIRECTION.CAMERA_EXTRAL.getValue()) {
                    direction = CAMERA_DIRECTION.CAMERA_EXTRAL;
                }
                CAMERA_FOCAL_LENGTH_TYPE focalLengthType = CAMERA_FOCAL_LENGTH_TYPE.CAMERA_FOCAL_LENGTH_DEFAULT;
                if (info.focalLengthType == CAMERA_FOCAL_LENGTH_TYPE.CAMERA_FOCAL_LENGTH_WIDE_ANGLE.getValue()) {
                    focalLengthType = CAMERA_FOCAL_LENGTH_TYPE.CAMERA_FOCAL_LENGTH_WIDE_ANGLE;
                } else if (info.focalLengthType == CAMERA_FOCAL_LENGTH_TYPE.CAMERA_FOCAL_LENGTH_ULTRA_WIDE.getValue()) {
                    focalLengthType = CAMERA_FOCAL_LENGTH_TYPE.CAMERA_FOCAL_LENGTH_ULTRA_WIDE;
                } else if (info.focalLengthType == CAMERA_FOCAL_LENGTH_TYPE.CAMERA_FOCAL_LENGTH_TELEPHOTO.getValue()) {
                    focalLengthType = CAMERA_FOCAL_LENGTH_TYPE.CAMERA_FOCAL_LENGTH_TELEPHOTO;
                }
                CameraCapturerConfiguration config = new CameraCapturerConfiguration(
                        direction, focalLengthType
                );
                int ret = engine.setCameraCapturerConfiguration(
                        config
                );
                Log.d(TAG, "setCameraCapturerConfiguration ret=" + ret);
            }

            @Override
            public void onNothingSelected(AdapterView<?> parent) {

            }
        });
        mSettingDialog = new BottomSheetDialog(requireContext());
        mSettingDialog.setContentView(mSettingBinding.getRoot());

        //init fps
        mRootBinding.sbLocal.setOnSeekBarChangeListener(this);
        mRootBinding.sbRemote.setOnSeekBarChangeListener(this);

        mRootBinding.spinnerScenario.setOnItemSelectedListener(this);
        mRootBinding.spinnerSnapshot.setOnItemSelectedListener(this);

        mRootBinding.btnLocalScreenshot.setEnabled(false);
    }

    private void resetFps() {
        localFps = FPS_DEFAULT;
        remoteFps = FPS_DEFAULT;
        mRootBinding.sbLocal.setProgress(localFps);
        mRootBinding.sbRemote.setProgress(remoteFps);
    }

    private void updateVideoView() {

        if (backGroundVideo.getChildCount() > 0 && backGroundVideo.getReportUid() != -1) {
            int reportUid = backGroundVideo.getReportUid();
            SurfaceView videoView = new SurfaceView(requireContext());
            backGroundVideo.removeAllViews();
            backGroundVideo.addView(videoView);
            VideoCanvas local = new VideoCanvas(videoView, canvasRenderMode, reportUid);
            local.backgroundColor = canvasBgColor;
            if (reportUid == myUid) {
                engine.setupLocalVideo(local);
            } else {
                engine.setupRemoteVideo(local);
            }
        }
        if (foreGroundVideo.getChildCount() > 0 && foreGroundVideo.getReportUid() != -1) {
            int reportUid = foreGroundVideo.getReportUid();
            SurfaceView videoView = new SurfaceView(requireContext());
            videoView.setZOrderMediaOverlay(true);
            foreGroundVideo.removeAllViews();
            foreGroundVideo.addView(videoView);
            VideoCanvas local = new VideoCanvas(videoView, canvasRenderMode, reportUid);
            local.backgroundColor = canvasBgColor;
            if (reportUid == myUid) {
                engine.setupLocalVideo(local);
            } else {
                engine.setupRemoteVideo(local);
            }
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

            /*
             * Creates an RtcEngine instance.
             * @param context The context of Android Activity
             * @param appId The App ID issued to you by Agora. See <a href="https://docs.agora.io/en/Agora%20Platform/token#get-an-app-id">
             *              How to get the App ID</a>
             * @param handler IRtcEngineEventHandler is an abstract class providing default implementation.
             *                The SDK uses this class to report to the app on SDK runtime events.*/
            RtcEngineConfig rtcEngineConfig = new RtcEngineConfig();
            rtcEngineConfig.mAppId = getString(R.string.agora_app_id);
            rtcEngineConfig.mContext = context.getApplicationContext();
            rtcEngineConfig.mEventHandler = iRtcEngineEventHandler;
            /* Sets the channel profile of the Agora RtcEngine. */
            rtcEngineConfig.mChannelProfile = Constants.CHANNEL_PROFILE_LIVE_BROADCASTING;
            rtcEngineConfig.mAudioScenario = Constants.AudioScenario.getValue(Constants.AudioScenario.DEFAULT);
            rtcEngineConfig.mAreaCode = ((MainApplication) getActivity().getApplication()).getGlobalSettings().getAreaCode();
            engine = RtcEngine.create(rtcEngineConfig);
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

            engine.setVideoEncoderConfiguration(videoEncoderConfiguration);
            engine.enableDualStreamMode(true);

            agoraFocalLengthInfos = engine.queryCameraFocalLengthCapability();
            ArrayList<String> strings = new ArrayList<>();
            for (AgoraFocalLengthInfo info : agoraFocalLengthInfos) {
                String cameraDirection = getString(R.string.camera_front);
                if (info.cameraDirection == CAMERA_DIRECTION.CAMERA_REAR.getValue()) {
                    cameraDirection = getString(R.string.camera_rear);
                } else if (info.cameraDirection == CAMERA_DIRECTION.CAMERA_EXTRAL.getValue()) {
                    cameraDirection = getString(R.string.camera_extral);
                }
                String focalLength = getString(R.string.camera_focal_default);
                if (info.focalLengthType == CAMERA_FOCAL_LENGTH_TYPE.CAMERA_FOCAL_LENGTH_WIDE_ANGLE.getValue()) {
                    focalLength = getString(R.string.camera_focal_wide_angle);
                } else if (info.focalLengthType == CAMERA_FOCAL_LENGTH_TYPE.CAMERA_FOCAL_LENGTH_ULTRA_WIDE.getValue()) {
                    focalLength = getString(R.string.camera_focal_urltra_wide);
                } else if (info.focalLengthType == CAMERA_FOCAL_LENGTH_TYPE.CAMERA_FOCAL_LENGTH_TELEPHOTO.getValue()) {
                    focalLength = getString(R.string.camera_focal_telephoto);
                }
                strings.add(String.format(Locale.US, "[%s] %s", cameraDirection, focalLength));
            }
            mSettingBinding.spCamera.setAdapter(new ArrayAdapter<String>(requireContext(), android.R.layout.simple_spinner_dropdown_item, strings));
        } catch (Exception e) {
            requireActivity().onBackPressed();
            e.printStackTrace();
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

    @Override
    public void onClick(View v) {
        if (v.getId() == R.id.btn_join) {
            if (!joined) {
                CommonUtil.hideInputBoard(requireActivity(), mRootBinding.etChannel);
                // call when join button hit
                String channelId = mRootBinding.etChannel.getText().toString();
                // Check permission
                checkOrRequestPermisson(new PermissonUtils.PermissionResultCallback() {
                    @Override
                    public void onPermissionsResult(boolean allPermissionsGranted, String[] permissions, int[] grantResults) {
                        if (allPermissionsGranted) {
                            joinChannel(channelId);
                        }
                    }
                });
            } else {
                mRootBinding.btnLocalScreenshot.setEnabled(false);
                joined = false;
                mRootBinding.llContainerFp.setVisibility(View.GONE);
                isHost = false;
                isPreloaded = false;
                mRootBinding.btnJoin.setText(getString(R.string.join));
                mRootBinding.btnPublish.setEnabled(false);
                mRootBinding.btnPreload.setEnabled(true);
                mRootBinding.etChannel.setEnabled(true);
                mRootBinding.btnPublish.setText(getString(R.string.enable_publish));
                mRootBinding.videoTrackingLayout.getRoot().setVisibility(View.GONE);
                mSettingBinding.switchWatermark.setChecked(false);
                remoteUid = 0;
                foreGroundVideo.removeAllViews();
                backGroundVideo.removeAllViews();

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
                engine.stopPreview();
                engine.leaveChannel();

            }
        } else if (v.getId() == R.id.btn_publish) {
            isHost = !isHost;
            if (isHost) {
                engine.setClientRole(Constants.CLIENT_ROLE_BROADCASTER);
            } else {
                ClientRoleOptions clientRoleOptions = new ClientRoleOptions();
                clientRoleOptions.audienceLatencyLevel = mSettingBinding.switchLowLatency.isChecked() ? Constants.AUDIENCE_LATENCY_LEVEL_ULTRA_LOW_LATENCY
                        : Constants.AUDIENCE_LATENCY_LEVEL_LOW_LATENCY;
                engine.setClientRole(CLIENT_ROLE_AUDIENCE, clientRoleOptions);
            }
            mRootBinding.btnPublish.setEnabled(false);
            mRootBinding.btnPublish.setText(isHost ? getString(R.string.disnable_publish) : getString(R.string.enable_publish));
        } else if (v.getId() == R.id.foreground_video) {
            isLocalVideoForeground = !isLocalVideoForeground;
            int foreGroundReportId = foreGroundVideo.getReportUid();
            foreGroundVideo.setReportUid(backGroundVideo.getReportUid());
            backGroundVideo.setReportUid(foreGroundReportId);

            if (foreGroundVideo.getChildCount() > 0) {
                foreGroundVideo.removeAllViews();
            }
            if (backGroundVideo.getChildCount() > 0) {
                backGroundVideo.removeAllViews();
            }
            // Create render view by RtcEngine
            SurfaceView localView = new SurfaceView(getContext());
            SurfaceView remoteView = new SurfaceView(getContext());
            if (isLocalVideoForeground) {
                // Add to the local container
                foreGroundVideo.addView(localView, 0, new FrameLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));
                // Add to the remote container
                backGroundVideo.addView(remoteView, 0, new FrameLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));
                // Setup remote video to render
                VideoCanvas remote = new VideoCanvas(remoteView, canvasRenderMode, remoteUid);
                remote.backgroundColor = canvasBgColor;
                engine.setupRemoteVideo(remote);
                // Setup local video to render your local camera preview
                VideoCanvas local = new VideoCanvas(localView, canvasRenderMode, 0);
                local.backgroundColor = canvasBgColor;
                engine.setupLocalVideo(local);
                localView.setZOrderMediaOverlay(true);
            } else {
                // Add to the local container
                foreGroundVideo.addView(remoteView, 0, new FrameLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));
                // Add to the remote container
                backGroundVideo.addView(localView, 0, new FrameLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));
                // Setup local video to render your local camera preview
                VideoCanvas local = new VideoCanvas(localView, canvasRenderMode, 0);
                local.backgroundColor = canvasBgColor;
                engine.setupLocalVideo(local);
                // Setup remote video to render
                VideoCanvas remote = new VideoCanvas(remoteView, canvasRenderMode, remoteUid);
                remote.backgroundColor = canvasBgColor;
                engine.setupRemoteVideo(remote);
                remoteView.setZOrderMediaOverlay(true);
            }
        } else if (v.getId() == R.id.btn_setting) {
            mSettingDialog.show();
        } else if (v.getId() == R.id.btn_local_screenshot) {
            takeSnapshot(myUid);
        } else if (v.getId() == R.id.btn_remote_screenshot) {
            takeSnapshot(remoteUid);
        } else if (v.getId() == R.id.btn_preload) {
            String channelName = mRootBinding.etChannel.getText().toString();
            if (TextUtils.isEmpty(channelName)) {
                Toast.makeText(getContext(), "The channel name is empty!", Toast.LENGTH_SHORT).show();
            } else {
                myUid = new Random().nextInt(1000) + 10000;
                TokenUtils.gen(getContext(), channelName, myUid, token -> {
                    myToken = token;
                    int ret = engine.preloadChannel(token, channelName, myUid);
                    if (ret == Constants.ERR_OK) {
                        isPreloaded = true;
                        mRootBinding.btnPreload.setEnabled(false);
                        mRootBinding.etChannel.setEnabled(false);
                        Toast.makeText(getContext(), "Preload success : uid=" + myUid, Toast.LENGTH_SHORT).show();
                    }
                });
            }
        } else if (v.getId() == R.id.btn_switch_camera) {
            engine.switchCamera();
        }
    }

    private void joinChannel(String channelId) {
        // Check if the context is valid
        Context context = getContext();
        if (context == null) {
            return;
        }
        resetFps();
        isLocalVideoForeground = false;
        // Create render view by RtcEngine
        SurfaceView surfaceView = new SurfaceView(context);
        if (backGroundVideo.getChildCount() > 0) {
            backGroundVideo.removeAllViews();
        }
        // Add to the local container
        backGroundVideo.addView(surfaceView, new FrameLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));
        // Setup local video to render your local camera preview
        VideoCanvas local = new VideoCanvas(surfaceView, canvasRenderMode, 0);
        local.backgroundColor = canvasBgColor;
        engine.setupLocalVideo(local);
        engine.setDefaultAudioRoutetoSpeakerphone(true);
        engine.startPreview();

        // Enable video module
        engine.enableVideo();
        // Setup video encoding configs
        engine.setVideoEncoderConfiguration(new VideoEncoderConfiguration(
                ((MainApplication) getActivity().getApplication()).getGlobalSettings().getVideoEncodingDimensionObject(),
                VideoEncoderConfiguration.FRAME_RATE.valueOf(((MainApplication) getActivity().getApplication()).getGlobalSettings().getVideoEncodingFrameRate()),
                STANDARD_BITRATE,
                VideoEncoderConfiguration.ORIENTATION_MODE.valueOf(((MainApplication) getActivity().getApplication()).getGlobalSettings().getVideoEncodingOrientation())
        ));

        engine.startMediaRenderingTracing();



        /*
         * Please configure accessToken in the string_config file.
         * A temporary token generated in Console. A temporary token is valid for 24 hours. For details, see
         *      https://docs.agora.io/en/Agora%20Platform/token?platform=All%20Platforms#get-a-temporary-token
         * A token generated at the server. This applies to scenarios with high-security requirements. For details, see
         *      https://docs.agora.io/en/cloud-recording/token_server_java?platform=Java*/
        TokenUtils.gen(requireContext(), channelId, myUid, token -> {
            /* Allows a user to join a channel.
             if you do not specify the uid, we will generate the uid for you*/

            ChannelMediaOptions option = new ChannelMediaOptions();
            option.channelProfile = Constants.CHANNEL_PROFILE_LIVE_BROADCASTING;
            option.clientRoleType = CLIENT_ROLE_AUDIENCE;
            option.autoSubscribeAudio = true;
            option.autoSubscribeVideo = true;
            int res;
            if (isPreloaded) {
                res = engine.joinChannel(myToken, channelId, myUid, option);
            } else {
                res = engine.joinChannel(token, channelId, myUid, option);
            }
            if (res != 0) {
                engine.stopPreview();
                // Usually happens with invalid parameters
                // Error code description can be found at:
                // en: https://docs.agora.io/en/Voice/API%20Reference/java/classio_1_1agora_1_1rtc_1_1_i_rtc_engine_event_handler_1_1_error_code.html
                // cn: https://docs.agora.io/cn/Voice/API%20Reference/java/classio_1_1agora_1_1rtc_1_1_i_rtc_engine_event_handler_1_1_error_code.html
                showAlert(RtcEngine.getErrorDescription(Math.abs(res)));
                return;
            }
            // Prevent repeated entry
            mRootBinding.btnJoin.setEnabled(false);
            mRootBinding.btnPreload.setEnabled(false);
        });
    }

    private void enableWatermark(boolean enable) {
        if (enable) {
            WatermarkOptions watermarkOptions = new WatermarkOptions();
            int size = ((MainApplication) requireActivity().getApplication()).getGlobalSettings().getVideoEncodingDimensionObject().width / 6;
            int height = ((MainApplication) requireActivity().getApplication()).getGlobalSettings().getVideoEncodingDimensionObject().height;
            watermarkOptions.positionInPortraitMode = new WatermarkOptions.Rectangle(10, height / 2, size, size);
            watermarkOptions.positionInLandscapeMode = new WatermarkOptions.Rectangle(10, height / 2, size, size);
            watermarkOptions.visibleInPreview = true;
            int ret = engine.addVideoWatermark(Constant.WATER_MARK_FILE_PATH, watermarkOptions);
            if (ret != Constants.ERR_OK) {
                Log.e(TAG, "addVideoWatermark error=" + ret + ", msg=" + RtcEngine.getErrorDescription(ret));
            }
        } else {
            engine.clearVideoWatermarks();
        }
    }

    private void enableLowStream(boolean enable) {
        engine.setRemoteDefaultVideoStreamType(enable ? Constants.VideoStreamType.VIDEO_STREAM_LOW : Constants.VideoStreamType.VIDEO_STREAM_HIGH);
        if (remoteUid != 0) {
            engine.setRemoteVideoStreamType(remoteUid, enable ? Constants.VideoStreamType.VIDEO_STREAM_LOW : Constants.VideoStreamType.VIDEO_STREAM_HIGH);
        }
    }

    private void setEncodingPreference(int index) {
        VideoEncoderConfiguration.ENCODING_PREFERENCE[] preferences = new VideoEncoderConfiguration.ENCODING_PREFERENCE[]{
                VideoEncoderConfiguration.ENCODING_PREFERENCE.PREFER_AUTO,
                VideoEncoderConfiguration.ENCODING_PREFERENCE.PREFER_HARDWARE,
                VideoEncoderConfiguration.ENCODING_PREFERENCE.PREFER_SOFTWARE,
        };

        VideoEncoderConfiguration.AdvanceOptions advanceOptions = new VideoEncoderConfiguration.AdvanceOptions();
        advanceOptions.encodingPreference = preferences[index];
        videoEncoderConfiguration.advanceOptions = advanceOptions;
        engine.setVideoEncoderConfiguration(videoEncoderConfiguration);
    }

    private void enableBFrame(boolean enable) {
        videoEncoderConfiguration.advanceOptions.compressionPreference = enable
                ? VideoEncoderConfiguration.COMPRESSION_PREFERENCE.PREFER_QUALITY
                : VideoEncoderConfiguration.COMPRESSION_PREFERENCE.PREFER_LOW_LATENCY;
        engine.setVideoEncoderConfiguration(videoEncoderConfiguration);
    }

    private void enableLowLegacy(boolean enable) {
        if (isHost) {
            return;
        }
        ClientRoleOptions clientRoleOptions = new ClientRoleOptions();
        clientRoleOptions.audienceLatencyLevel = enable ? Constants.AUDIENCE_LATENCY_LEVEL_ULTRA_LOW_LATENCY : Constants.AUDIENCE_LATENCY_LEVEL_LOW_LATENCY;
        engine.setClientRole(CLIENT_ROLE_AUDIENCE, clientRoleOptions);
    }

    private void takeSnapshot(int uid) {
        if (!joined) {
            showLongToast(getString(R.string.join_channel_first));
            return;
        }
        String filePath = new File(requireContext().getExternalCacheDir(), uid + "_livestreaming_snapshot.png").getAbsolutePath();
        SnapshotConfig config = new SnapshotConfig();
        config.filePath = filePath;
        if (uid == myUid) {
            config.position = position;
            int ret = engine.takeSnapshot(0, config);
            if (ret != Constants.ERR_OK) {
                showLongToast("takeSnapshot local error code=" + ret + ",msg=" + RtcEngine.getErrorDescription(ret));
            }
        } else {
            if (uid != 0) {
//                config.position = Constants.VideoModulePosition.VIDEO_MODULE_POSITION_PRE_RENDERER;
//                int ret = engine.takeSnapshot(uid, config);
                int ret = engine.takeSnapshot(uid, filePath);
                if (ret != Constants.ERR_OK) {
                    showLongToast("takeSnapshot remote error code=" + ret + ",msg=" + RtcEngine.getErrorDescription(ret));
                }
            } else {
                showLongToast(getString(R.string.remote_screenshot_tip));
            }
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
            Log.e(TAG, String.format("onError code %d message %s", err, RtcEngine.getErrorDescription(err)));
            showAlert(String.format("onError code %d message %s", err, RtcEngine.getErrorDescription(err)));
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
                    mRootBinding.llContainerFp.setVisibility(View.VISIBLE);
                    mRootBinding.btnJoin.setEnabled(true);
                    mRootBinding.btnJoin.setText(getString(R.string.leave));
                    mRootBinding.btnPublish.setEnabled(true);
                    if (isLocalVideoForeground) {
                        foreGroundVideo.setReportUid(uid);
                    } else {
                        backGroundVideo.setReportUid(uid);
                    }
                }
            });
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
            }
            if (remoteUid != 0) {
                return;
            } else {
                remoteUid = uid;
            }
            handler.post(() -> {
                VideoReportLayout videoContainer = isLocalVideoForeground ? backGroundVideo : foreGroundVideo;
                /*Display remote video stream*/
                SurfaceView surfaceView = null;
                if (videoContainer.getChildCount() > 0) {
                    videoContainer.removeAllViews();
                }
                // Create render view by RtcEngine
                surfaceView = new SurfaceView(context);
                surfaceView.setZOrderMediaOverlay(!isLocalVideoForeground);
                // Add to the remote container
                videoContainer.addView(surfaceView, new FrameLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));

                videoContainer.setReportUid(remoteUid);
                // Setup remote video to render
                VideoCanvas remote = new VideoCanvas(surfaceView, canvasRenderMode, remoteUid);
                remote.backgroundColor = canvasBgColor;
                engine.setupRemoteVideo(remote);
            });
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
            if (uid == remoteUid) {
                remoteUid = 0;
                runOnUIThread(new Runnable() {
                    @Override
                    public void run() {
                        /*Clear render view
                         Note: The video will stay at its last frame, to completely remove it you will need to
                         remove the SurfaceView from its parent*/
                        VideoCanvas remote = new VideoCanvas(null, canvasRenderMode, uid);
                        remote.backgroundColor = canvasBgColor;
                        engine.setupRemoteVideo(remote);

                        VideoReportLayout videoContainer = isLocalVideoForeground ? backGroundVideo : foreGroundVideo;
                        videoContainer.setReportUid(-1);
                        videoContainer.removeAllViews();
                    }
                });
            }
        }

        /**
         * Occurs when the user role switches in a live streaming. For example, from a host to an audience or vice versa.
         *
         * The SDK triggers this callback when the local user switches the user role by calling the setClientRole method after joining the channel.
         * @param oldRole Role that the user switches from.
         * @param newRole Role that the user switches to.
         */
        @Override
        public void onClientRoleChanged(int oldRole, int newRole, ClientRoleOptions newRoleOptions) {
            super.onClientRoleChanged(oldRole, newRole, newRoleOptions);
            Log.i(TAG, String.format("client role changed from state %d to %d", oldRole, newRole));
            runOnUIThread(() -> {
                mRootBinding.btnPublish.setEnabled(true);
                if (newRole == Constants.CLIENT_ROLE_BROADCASTER) {
                    mRootBinding.btnLocalScreenshot.setEnabled(true);
                } else {
                    mRootBinding.btnLocalScreenshot.setEnabled(false);
                }
            });
        }

        @Override
        public void onSnapshotTaken(int uid, String filePath, int width, int height, int errCode) {
            super.onSnapshotTaken(uid, filePath, width, height, errCode);
            Log.d(TAG, String.format(Locale.US, "onSnapshotTaken uid=%d, filePath=%s, width=%d, height=%d, errorCode=%d", uid, filePath, width, height, errCode));
            if (errCode == 0) {
                showLongToast("SnapshotTaken path=" + filePath);
            } else {
                showLongToast("SnapshotTaken error=" + RtcEngine.getErrorDescription(errCode));
            }
        }

        @Override
        public void onLocalVideoStats(Constants.VideoSourceType source, LocalVideoStats stats) {
            super.onLocalVideoStats(source, stats);
            if (isLocalVideoForeground) {
                foreGroundVideo.setLocalVideoStats(stats);
            } else {
                backGroundVideo.setLocalVideoStats(stats);
            }
        }

        @Override
        public void onLocalAudioStats(LocalAudioStats stats) {
            super.onLocalAudioStats(stats);
            if (isLocalVideoForeground) {
                foreGroundVideo.setLocalAudioStats(stats);
            } else {
                backGroundVideo.setLocalAudioStats(stats);
            }
        }

        @Override
        public void onRemoteVideoStats(RemoteVideoStats stats) {
            super.onRemoteVideoStats(stats);
            if (!isLocalVideoForeground) {
                foreGroundVideo.setRemoteVideoStats(stats);
            } else {
                backGroundVideo.setRemoteVideoStats(stats);
            }
        }

        @Override
        public void onRemoteAudioStats(RemoteAudioStats stats) {
            super.onRemoteAudioStats(stats);
            if (!isLocalVideoForeground) {
                foreGroundVideo.setRemoteAudioStats(stats);
            } else {
                backGroundVideo.setRemoteAudioStats(stats);
            }
        }

        @Override
        public void onVideoRenderingTracingResult(int uid, Constants.MEDIA_TRACE_EVENT currentEvent, VideoRenderingTracingInfo tracingInfo) {
            super.onVideoRenderingTracingResult(uid, currentEvent, tracingInfo);
            runOnUIThread(() -> {
                FragmentLiveStreamingVideoTrackingBinding videoTrackingLayout = mRootBinding.videoTrackingLayout;
                videoTrackingLayout.getRoot().setVisibility(View.VISIBLE);
                videoTrackingLayout.tvUid.setText(String.valueOf(uid));
                videoTrackingLayout.tvEvent.setText(String.valueOf(currentEvent.getValue()));
                videoTrackingLayout.tvElapsedTime.setText(String.format(Locale.US, "%d ms", tracingInfo.elapsedTime));
                videoTrackingLayout.tvStart2JoinChannel.setText(String.format(Locale.US, "%d ms", tracingInfo.start2JoinChannel));
                videoTrackingLayout.tvJoin2JoinSuccess.setText(String.format(Locale.US, "%d ms", tracingInfo.join2JoinSuccess));
                videoTrackingLayout.tvJoinSuccess2RemoteJoined.setText(String.format(Locale.US, "%d ms", tracingInfo.joinSuccess2RemoteJoined));
                videoTrackingLayout.tvRemoteJoined2SetView.setText(String.format(Locale.US, "%d ms", tracingInfo.remoteJoined2SetView));
                videoTrackingLayout.tvRemoteJoined2UnmuteVideo.setText(String.format(Locale.US, "%d ms", tracingInfo.remoteJoined2UnmuteVideo));
                videoTrackingLayout.tvRemoteJoined2PacketReceived.setText(String.format(Locale.US, "%d ms", tracingInfo.remoteJoined2PacketReceived));
            });
        }

    };

    @Override
    public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
        if (seekBar == mRootBinding.sbLocal) {
            localFps = progress;
            mRootBinding.tvLocalFpsNum.setText(localFps + "");
            if (engine != null) {
                engine.setLocalRenderTargetFps(Constants.VideoSourceType.VIDEO_SOURCE_CAMERA_PRIMARY, localFps);
            }
        } else if (seekBar == mRootBinding.sbRemote) {
            remoteFps = progress;
            mRootBinding.tvRemoteFpsNum.setText(remoteFps + "");
            if (engine != null) {
                engine.setRemoteRenderTargetFps(remoteFps);
            }
        }
    }

    @Override
    public void onStartTrackingTouch(SeekBar seekBar) {

    }

    @Override
    public void onStopTrackingTouch(SeekBar seekBar) {

    }

    @Override
    public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
        if (parent == mRootBinding.spinnerScenario) {
            if (engine != null) {
                // Set the video scenario
                String name = parent.getSelectedItem().toString();
                Constants.VideoScenario videoScenario = Constants.VideoScenario.valueOf(name);
                if (videoScenario != null) {
                    int ret = engine.setVideoScenario(videoScenario);
                    Log.d(TAG, "onItemSelected: setVideoScenario ret=" + ret);
                }
            }
        } else if (parent == mRootBinding.spinnerSnapshot) {
            if (engine != null) {
                String name = parent.getSelectedItem().toString();
                LiveStreaming.this.position = Constants.VideoModulePosition.valueOf(name);
            }
        }
    }

    @Override
    public void onNothingSelected(AdapterView<?> parent) {

    }
}