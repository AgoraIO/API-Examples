package io.agora.api.example.examples.advanced;

import android.content.Context;
import android.os.Bundle;
import android.text.TextUtils;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.SurfaceView;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.FrameLayout;
import android.widget.LinearLayout;
import android.widget.SeekBar;
import android.widget.Switch;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import com.yanzhenjie.permission.AndPermission;
import com.yanzhenjie.permission.runtime.Permission;

import org.json.JSONException;

import java.io.File;

import io.agora.api.component.Constant;
import io.agora.api.example.MainApplication;
import io.agora.api.example.R;
import io.agora.api.example.annotation.Example;
import io.agora.api.example.common.BaseFragment;
import io.agora.api.example.utils.CommonUtil;
import io.agora.rtc.IRtcEngineEventHandler;
import io.agora.rtc.RtcEngine;
import io.agora.rtc.RtcEngineConfig;
import io.agora.rtc.models.ChannelMediaOptions;
import io.agora.rtc.video.BeautyOptions;
import io.agora.rtc.video.ColorEnhanceOptions;
import io.agora.rtc.video.LowLightEnhanceOptions;
import io.agora.rtc.video.VideoCanvas;
import io.agora.rtc.video.VideoDenoiserOptions;
import io.agora.rtc.video.VideoEncoderConfiguration;
import io.agora.rtc.video.VirtualBackgroundSource;

import static android.view.View.INVISIBLE;
import static android.view.View.VISIBLE;
import static io.agora.api.example.common.model.Examples.ADVANCED;
import static io.agora.rtc.Constants.CHANNEL_PROFILE_LIVE_BROADCASTING;
import static io.agora.rtc.Constants.RENDER_MODE_HIDDEN;
import static io.agora.rtc.video.VideoEncoderConfiguration.STANDARD_BITRATE;
import static io.agora.rtc.video.VirtualBackgroundSource.BACKGROUND_BLUR;
import static io.agora.rtc.video.VirtualBackgroundSource.BACKGROUND_COLOR;
import static io.agora.rtc.video.VirtualBackgroundSource.BACKGROUND_IMG;

/**
 * This demo demonstrates how to make a VideoProcessExtension
 */
@Example(
        index = 28,
        group = ADVANCED,
        name = R.string.item_videoProcessExtension,
        actionId = R.id.action_mainFragment_video_enhancement,
        tipsId = R.string.videoEnhancement
)
public class FaceBeauty extends BaseFragment implements View.OnClickListener, CompoundButton.OnCheckedChangeListener, SeekBar.OnSeekBarChangeListener {
    private static final String TAG = FaceBeauty.class.getSimpleName();

    private FrameLayout fl_local, fl_remote;
    private LinearLayout controlPanel, virtualControlPanel;
    private Button join;
    private Switch beauty, lightness, colorful, noiseReduce, virtualBackground;
    private SeekBar seek_lightness, seek_redness, seek_sharpness, seek_smoothness, seek_strength, seek_skin;
    private EditText et_channel;
    private RtcEngine engine;
    private int myUid;
    private boolean joined = false;
    private BeautyOptions beautyOptions = new BeautyOptions();
    private LowLightEnhanceOptions lowLightEnhanceOptions = new LowLightEnhanceOptions();
    private ColorEnhanceOptions colorEnhanceOptions = new ColorEnhanceOptions();
    private VideoDenoiserOptions videoDenoiserOptions = new VideoDenoiserOptions();
    private VirtualBackgroundSource virtualBackgroundSource = new VirtualBackgroundSource();
    private float skinProtect = 1.0f;
    private float strength = 0.5f;

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
        view.findViewById(R.id.color).setOnClickListener(this);
        view.findViewById(R.id.pic).setOnClickListener(this);
        view.findViewById(R.id.blur).setOnClickListener(this);
        et_channel = view.findViewById(R.id.et_channel);
        fl_local = view.findViewById(R.id.fl_local);
        fl_remote = view.findViewById(R.id.fl_remote);
        controlPanel = view.findViewById(R.id.controlPanel);
        virtualControlPanel = view.findViewById(R.id.controller);
        beauty = view.findViewById(R.id.switch_face_beautify);
        beauty.setOnCheckedChangeListener(this);
        lightness = view.findViewById(R.id.switch_lightness);
        lightness.setOnCheckedChangeListener(this);
        colorful = view.findViewById(R.id.switch_color);
        colorful.setOnCheckedChangeListener(this);
        virtualBackground = view.findViewById(R.id.switch_virtual_background);
        virtualBackground.setOnCheckedChangeListener(this);
        noiseReduce = view.findViewById(R.id.switch_video_noise_reduce);
        noiseReduce.setOnCheckedChangeListener(this);
        seek_lightness = view.findViewById(R.id.lightening);
        seek_lightness.setOnSeekBarChangeListener(this);
        seek_redness = view.findViewById(R.id.redness);
        seek_redness.setOnSeekBarChangeListener(this);
        seek_sharpness = view.findViewById(R.id.sharpness);
        seek_sharpness.setOnSeekBarChangeListener(this);
        seek_smoothness = view.findViewById(R.id.smoothness);
        seek_smoothness.setOnSeekBarChangeListener(this);
        seek_strength = view.findViewById(R.id.strength);
        seek_strength.setOnSeekBarChangeListener(this);
        seek_skin = view.findViewById(R.id.skinProtect);
        seek_skin.setOnSeekBarChangeListener(this);

        virtualBackgroundSource.blur_degree = 2;
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
            /**
             * The context of Android Activity
             */
            config.mContext = context.getApplicationContext();
            /**
             * The App ID issued to you by Agora. See <a href="https://docs.agora.io/en/Agora%20Platform/token#get-an-app-id"> How to get the App ID</a>
             */
            config.mAppId = getString(R.string.agora_app_id);
            /**
             * IRtcEngineEventHandler is an abstract class providing default implementation.
             * The SDK uses this class to report to the app on SDK runtime events.
             */
            config.mEventHandler = iRtcEngineEventHandler;
            engine = RtcEngine.create(config);
            engine.setChannelProfile(CHANNEL_PROFILE_LIVE_BROADCASTING);
        } catch (Exception e) {
            e.printStackTrace();
            getActivity().onBackPressed();
        }
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        /**leaveChannel and Destroy the RtcEngine instance*/
        if (engine != null) {
            engine.leaveChannel();
            engine.stopPreview();
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
        SurfaceView surfaceView = RtcEngine.CreateRendererView(context);
        if (fl_local.getChildCount() > 0) {
            fl_local.removeAllViews();
        }
        // Add to the local container
        fl_local.addView(surfaceView, new FrameLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));
        // Setup local video to render your local camera preview
        engine.setupLocalVideo(new VideoCanvas(surfaceView, RENDER_MODE_HIDDEN, 0));
        // Set audio route to microPhone
        engine.setDefaultAudioRoutetoSpeakerphone(true);

        /**In the demo, the default is to enter as the anchor.*/
        engine.setClientRole(IRtcEngineEventHandler.ClientRole.CLIENT_ROLE_BROADCASTER);
        // Enable video module
        engine.enableVideo();
        // Setup video encoding configs
        engine.setVideoEncoderConfiguration(new VideoEncoderConfiguration(
                ((MainApplication) getActivity().getApplication()).getGlobalSettings().getVideoEncodingDimensionObject(),
                VideoEncoderConfiguration.FRAME_RATE.valueOf(((MainApplication) getActivity().getApplication()).getGlobalSettings().getVideoEncodingFrameRate()),
                STANDARD_BITRATE,
                VideoEncoderConfiguration.ORIENTATION_MODE.valueOf(((MainApplication) getActivity().getApplication()).getGlobalSettings().getVideoEncodingOrientation())
        ));

        /**Please configure accessToken in the string_config file.
         * A temporary token generated in Console. A temporary token is valid for 24 hours. For details, see
         *      https://docs.agora.io/en/Agora%20Platform/token?platform=All%20Platforms#get-a-temporary-token
         * A token generated at the server. This applies to scenarios with high-security requirements. For details, see
         *      https://docs.agora.io/en/cloud-recording/token_server_java?platform=Java*/
        String accessToken = getString(R.string.agora_access_token);
        if (TextUtils.equals(accessToken, "") || TextUtils.equals(accessToken, "<#YOUR ACCESS TOKEN#>")) {
            accessToken = null;
        }
        /**
         * enable face beauty by default
         */
        engine.startPreview();
        /** Allows a user to join a channel.
         if you do not specify the uid, we will generate the uid for you*/


        ChannelMediaOptions option = new ChannelMediaOptions();
        option.autoSubscribeAudio = true;
        option.autoSubscribeVideo = true;
        int res = engine.joinChannel(accessToken, channelId, "", 0);
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
    }

    @Override
    public void onClick(View v) {
        if (v.getId() == R.id.btn_join) {
            if (!joined) {
                CommonUtil.hideInputBoard(getActivity(), et_channel);
                // call when join button hit
                String channelId = et_channel.getText().toString();
                // Check permission
                if (AndPermission.hasPermissions(this, Permission.Group.STORAGE, Permission.Group.MICROPHONE, Permission.Group.CAMERA)) {
                    joinChannel(channelId);
                    return;
                }
                // Request permission
                AndPermission.with(this).runtime().permission(
                        Permission.Group.STORAGE,
                        Permission.Group.MICROPHONE,
                        Permission.Group.CAMERA
                ).onGranted(permissions ->
                {
                    // Permissions Granted
                    joinChannel(channelId);
                }).start();
            } else {
                joined = false;
                /**After joining a channel, the user must call the leaveChannel method to end the
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
                engine.stopPreview();
                join.setText(getString(R.string.join));
                controlPanel.setVisibility(INVISIBLE);
            }
        }
        else if (v.getId() == R.id.color) {
            virtualBackgroundSource.backgroundSourceType = BACKGROUND_COLOR;
            virtualBackgroundSource.color = 0xFF0000;
            engine.enableVirtualBackground(true, virtualBackgroundSource);
        }
        else if (v.getId() == R.id.pic) {
            virtualBackgroundSource.backgroundSourceType = BACKGROUND_IMG;
            virtualBackgroundSource.source = getContext().getFilesDir() + Constant.WATER_MARK_FILE_PATH;
            engine.enableVirtualBackground(true, virtualBackgroundSource);
        }
        else if (v.getId() == R.id.blur) {
            virtualBackgroundSource.backgroundSourceType = BACKGROUND_BLUR;
            engine.enableVirtualBackground(true, virtualBackgroundSource);
        }
    }

    @Override
    public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
        if (buttonView.getId() == beauty.getId()) {
            engine.setBeautyEffectOptions(isChecked, beautyOptions);
        } else if (buttonView.getId() == lightness.getId()) {
            engine.setLowlightEnhanceOptions(isChecked, lowLightEnhanceOptions);
        } else if (buttonView.getId() == colorful.getId()) {
            colorEnhanceOptions.skinProtectLevel = skinProtect;
            colorEnhanceOptions.strengthLevel = strength;
            engine.setColorEnhanceOptions(isChecked, colorEnhanceOptions);
        } else if (buttonView.getId() == noiseReduce.getId()) {
            engine.setVideoDenoiserOptions(isChecked, videoDenoiserOptions);
        } else if (buttonView.getId() == virtualBackground.getId()) {
            virtualControlPanel.setVisibility(isChecked ? VISIBLE : INVISIBLE);
            engine.enableVirtualBackground(isChecked, virtualBackgroundSource);
        }
    }

    @Override
    public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
        float value = ((float) progress) / 10;
        if (seekBar.getId() == seek_lightness.getId()) {
            beautyOptions.lighteningLevel = value;
            engine.setBeautyEffectOptions(beauty.isChecked(), beautyOptions);
        } else if (seekBar.getId() == seek_redness.getId()) {
            beautyOptions.rednessLevel = value;
            engine.setBeautyEffectOptions(beauty.isChecked(), beautyOptions);
        } else if (seekBar.getId() == seek_sharpness.getId()) {
            beautyOptions.sharpnessLevel = value;
            engine.setBeautyEffectOptions(beauty.isChecked(), beautyOptions);
        } else if (seekBar.getId() == seek_smoothness.getId()) {
            beautyOptions.smoothnessLevel = value;
            engine.setBeautyEffectOptions(beauty.isChecked(), beautyOptions);
        } else if (seekBar.getId() == seek_strength.getId()) {
            colorEnhanceOptions.strengthLevel = value;
            engine.setColorEnhanceOptions(colorful.isChecked(), colorEnhanceOptions);
        } else if (seekBar.getId() == seek_skin.getId()) {
            colorEnhanceOptions.skinProtectLevel = value;
            engine.setColorEnhanceOptions(colorful.isChecked(), colorEnhanceOptions);
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
        /**Reports a warning during SDK runtime.
         * Warning code: https://docs.agora.io/en/Voice/API%20Reference/java/classio_1_1agora_1_1rtc_1_1_i_rtc_engine_event_handler_1_1_warn_code.html*/
        @Override
        public void onWarning(int warn) {
            Log.w(TAG, String.format("onWarning code %d message %s", warn, RtcEngine.getErrorDescription(warn)));
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
                    controlPanel.setVisibility(VISIBLE);
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
            /**Check if the context is correct*/
            Context context = getContext();
            if (context == null) {
                return;
            } else {
                handler.post(() ->
                {
                    if (fl_remote.getChildCount() > 0) {
                        fl_remote.removeAllViews();
                    }
                    /**Display remote video stream*/
                    SurfaceView surfaceView = null;
                    // Create render view by RtcEngine
                    surfaceView = RtcEngine.CreateRendererView(context);
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
                    /**Clear render view
                     Note: The video will stay at its last frame, to completely remove it you will need to
                     remove the SurfaceView from its parent*/
                    engine.setupRemoteVideo(new VideoCanvas(null, RENDER_MODE_HIDDEN, uid));
                }
            });
        }
    };
}
