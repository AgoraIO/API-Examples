package io.agora.api.example.examples.advanced;

import static io.agora.api.example.common.model.Examples.ADVANCED;
import static io.agora.rtc2.Constants.RENDER_MODE_HIDDEN;
import static io.agora.rtc2.video.VideoEncoderConfiguration.STANDARD_BITRATE;

import android.annotation.SuppressLint;
import android.content.Context;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.SurfaceView;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.FrameLayout;
import android.widget.Spinner;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import java.util.Map;
import java.util.Random;
import java.util.concurrent.ConcurrentHashMap;

import io.agora.api.example.MainApplication;
import io.agora.api.example.R;
import io.agora.api.example.annotation.Example;
import io.agora.api.example.common.BaseFragment;
import io.agora.api.example.common.widget.VideoReportLayout;
import io.agora.api.example.utils.CommonUtil;
import io.agora.api.example.utils.TokenUtils;
import io.agora.rtc2.ChannelMediaOptions;
import io.agora.rtc2.Constants;
import io.agora.rtc2.IRtcEngineEventHandler;
import io.agora.rtc2.RtcEngine;
import io.agora.rtc2.RtcEngineConfig;
import io.agora.rtc2.SimulcastConfig;
import io.agora.rtc2.proxy.LocalAccessPointConfiguration;
import io.agora.rtc2.video.VideoCanvas;
import io.agora.rtc2.video.VideoEncoderConfiguration;
import kotlin.Pair;

/**
 * This example demonstrates how to use Simulcast
 */
@Example(
        index = 28,
        group = ADVANCED,
        name = R.string.item_simulcast,
        actionId = R.id.action_mainFragment_to_simulcast,
        tipsId = R.string.tip_simulcast
)
public class Simulcast extends BaseFragment implements View.OnClickListener, CompoundButton.OnCheckedChangeListener,
        AdapterView.OnItemSelectedListener {
    private static final String TAG = Simulcast.class.getSimpleName();

    private VideoReportLayout fl_local, fl_remote;
    private Button btn_join;
    private CheckBox cb_layer1, cb_layer2, cb_layer3, cb_layer4;
    private Spinner spinner_stream_layer, spinner_role;

    private EditText et_channel;
    private RtcEngine engine;
    private int myUid;
    private boolean joined = false;
    private Map<Integer, Pair<VideoReportLayout, Spinner>> remoteViews = new ConcurrentHashMap<>();
    private SimulcastConfig simulcastConfig = new SimulcastConfig();
    private int selectedLayerCount = 3; // Default selected 3 layers

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment_simulcast, container, false);
        return view;
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        et_channel = view.findViewById(R.id.et_channel);
        btn_join = view.findViewById(R.id.btn_join);
        btn_join.setOnClickListener(this);

        spinner_stream_layer = view.findViewById(R.id.spinner_stream_layer);
        spinner_stream_layer.setOnItemSelectedListener(this);
        spinner_role = view.findViewById(R.id.spinner_role);
        spinner_role.setOnItemSelectedListener(this);

        cb_layer1 = view.findViewById(R.id.cb_layer1);
        cb_layer2 = view.findViewById(R.id.cb_layer2);
        cb_layer3 = view.findViewById(R.id.cb_layer3);
        cb_layer4 = view.findViewById(R.id.cb_layer4);
        cb_layer1.setOnCheckedChangeListener(this);
        cb_layer2.setOnCheckedChangeListener(this);
        cb_layer3.setOnCheckedChangeListener(this);
        cb_layer4.setOnCheckedChangeListener(this);

        fl_local = view.findViewById(R.id.fl_local);
        fl_remote = view.findViewById(R.id.fl_remote);
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
        } catch (Exception e) {
            e.printStackTrace();
            getActivity().onBackPressed();
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

    @SuppressLint("WrongConstant")
    @Override
    public void onClick(View v) {
        if (v.getId() == R.id.btn_join) {
            if (!joined) {
                CommonUtil.hideInputBoard(getActivity(), et_channel);
                // call when join button hit
                String channelId = et_channel.getText().toString();
                // Check permission
                checkOrRequestPermisson((allPermissionsGranted, permissions, grantResults) -> {
                    // Permissions Granted
                    if (allPermissionsGranted) {
                        joinChannel(channelId, spinner_role.getSelectedItemPosition() == 0);
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
                btn_join.setText(getString(R.string.join));
                spinner_role.setEnabled(true);
                for (Pair<VideoReportLayout, Spinner> value : remoteViews.values()) {
                    value.getFirst().removeAllViews();
                    value.getSecond().setVisibility(View.GONE);
                }
                remoteViews.clear();
            }
        }
    }

    @Override
    public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
        if (isChecked) {
            // If already selected 3 layers, cancel the selection
            if (selectedLayerCount >= 3) {
                buttonView.setChecked(false);
                showLongToast("Maximum 3 layers can be selected");
                return;
            }
            selectedLayerCount++;
        } else {
            selectedLayerCount--;
        }

        int id = buttonView.getId();
        if (id == R.id.cb_layer1) {
            int layer1 = SimulcastConfig.StreamLayerIndex.STREAM_LAYER_1.getValue();
            simulcastConfig.configs[layer1].enable = cb_layer1.isChecked();
        } else if (id == R.id.cb_layer2) {
            int layer2 = SimulcastConfig.StreamLayerIndex.STREAM_LAYER_2.getValue();
            simulcastConfig.configs[layer2].enable = cb_layer2.isChecked();
        } else if (id == R.id.cb_layer3) {
            int layer3 = SimulcastConfig.StreamLayerIndex.STREAM_LAYER_3.getValue();
            simulcastConfig.configs[layer3].enable = cb_layer3.isChecked();
        } else if (id == R.id.cb_layer4) {
            int layer4 = SimulcastConfig.StreamLayerIndex.STREAM_LAYER_4.getValue();
            simulcastConfig.configs[layer4].enable = cb_layer4.isChecked();
        }
        
        // Update simulcast configuration
        if (engine != null && joined) {
            engine.setSimulcastConfig(simulcastConfig);
        }
    }

    @Override
    public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
        if (parent.getId() == R.id.spinner_role) {
            // nothing
        } else if (parent.getId() == R.id.spinner_stream_layer) {
            remoteViews.keySet().toArray();

        }
    }

    @Override
    public void onNothingSelected(AdapterView<?> parent) {

    }

    private void joinChannel(String channelId, boolean broadcast) {
        // Check if the context is valid
        Context context = getContext();
        if (context == null) {
            return;
        }

        if (broadcast) {
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
        } else {
            engine.setClientRole(Constants.CLIENT_ROLE_AUDIENCE);
        }
        // Setup video encoding configs, VideoDimensions should bigger StreamLayerConfig
        engine.setVideoEncoderConfiguration(new VideoEncoderConfiguration(
                VideoEncoderConfiguration.VD_1280x720,
                VideoEncoderConfiguration.FRAME_RATE.FRAME_RATE_FPS_30,
                STANDARD_BITRATE,
                VideoEncoderConfiguration.ORIENTATION_MODE.ORIENTATION_MODE_ADAPTIVE
        ));

        ChannelMediaOptions option = new ChannelMediaOptions();
        option.autoSubscribeAudio = true;
        option.autoSubscribeVideo = true;
        option.publishMicrophoneTrack = broadcast;
        option.publishCameraTrack = broadcast;

        if (broadcast) {
            // set simulcast config
            int layer1 = SimulcastConfig.StreamLayerIndex.STREAM_LAYER_1.getValue();
            simulcastConfig.configs[layer1].dimensions.width = 1280;
            simulcastConfig.configs[layer1].dimensions.height = 720;
            simulcastConfig.configs[layer1].framerate = 30;
            simulcastConfig.configs[layer1].enable = cb_layer1.isChecked();
            int layer2 = SimulcastConfig.StreamLayerIndex.STREAM_LAYER_2.getValue();
            simulcastConfig.configs[layer2].dimensions.width = 960;
            simulcastConfig.configs[layer2].dimensions.height = 540;
            simulcastConfig.configs[layer2].framerate = 15;
            simulcastConfig.configs[layer2].enable = cb_layer2.isChecked();
            int layer3 = SimulcastConfig.StreamLayerIndex.STREAM_LAYER_3.getValue();
            simulcastConfig.configs[layer3].dimensions.width = 640;
            simulcastConfig.configs[layer3].dimensions.height = 360;
            simulcastConfig.configs[layer3].framerate = 15;
            simulcastConfig.configs[layer3].enable = cb_layer3.isChecked();
            int layer4 = SimulcastConfig.StreamLayerIndex.STREAM_LAYER_4.getValue();
            simulcastConfig.configs[layer4].dimensions.width = 480;
            simulcastConfig.configs[layer4].dimensions.height = 270;
            simulcastConfig.configs[layer4].framerate = 15;
            simulcastConfig.configs[layer4].enable = cb_layer4.isChecked();
            engine.setSimulcastConfig(simulcastConfig);
        }

        /*Please configure accessToken in the string_config file.
         * A temporary token generated in Console. A temporary token is valid for 24 hours. For details, see
         *      https://docs.agora.io/en/Agora%20Platform/token?platform=All%20Platforms#get-a-temporary-token
         * A token generated at the server. This applies to scenarios with high-security requirements. For details, see
         *      https://docs.agora.io/en/cloud-recording/token_server_java?platform=Java*/
        int uid = new Random().nextInt(1000) + 100000;
        TokenUtils.gen(requireContext(), channelId, uid, ret -> {

            /* Allows a user to join a channel.
             if you do not specify the uid, we will generate the uid for you*/
            int res = engine.joinChannel(ret, channelId, uid, option);
            if (res != 0) {
                // Usually happens with invalid parameters
                // Error code description can be found at:
                // en: https://docs.agora.io/en/Voice/API%20Reference/java/classio_1_1agora_1_1rtc_1_1_i_rtc_engine_event_handler_1_1_error_code.html
                // cn: https://docs.agora.io/cn/Voice/API%20Reference/java/classio_1_1agora_1_1rtc_1_1_i_rtc_engine_event_handler_1_1_error_code.html
                showAlert(RtcEngine.getErrorDescription(Math.abs(res)));
                return;
            }
            // Prevent repeated entry
            btn_join.setEnabled(false);
            spinner_role.setEnabled(false);
        });
    }

    /**
     * IRtcEngineEventHandler is an abstract class providing default implementation.
     * The SDK uses this class to report to the app on SDK runtime events.
     */
    private final IRtcEngineEventHandler iRtcEngineEventHandler = new IRtcEngineEventHandler() {
        /**
         * Error code description can be found at:
         * en: {@see https://api-ref.agora.io/en/video-sdk/android/4.x/API/class_irtcengineeventhandler.html#callback_irtcengineeventhandler_onerror}
         * cn: {@see https://docs.agora.io/cn/video-call-4.x/API%20Reference/java_ng/API/class_irtcengineeventhandler.html#callback_irtcengineeventhandler_onerror}
         */
        @Override
        public void onError(int err) {
            super.onError(err);
            showLongToast("Error code:" + err + ", msg:" + RtcEngine.getErrorDescription(err));
            if (err == Constants.ERR_INVALID_TOKEN || err == Constants.ERR_TOKEN_EXPIRED) {
                engine.leaveChannel();
                runOnUIThread(() -> {
                    btn_join.setEnabled(true);
                    spinner_role.setEnabled(true);
                });

                if (Constants.ERR_INVALID_TOKEN == err) {
                    showAlert(getString(R.string.token_invalid));
                } else {
                    showAlert(getString(R.string.token_expired));
                }
            }
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

            runOnUIThread(() -> {
                btn_join.setEnabled(true);
                btn_join.setText(getString(R.string.leave));
                fl_local.setReportUid(uid);
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

        private void updateStream(Spinner spinnerStreamLayout, int uid) {
            String videoStream = spinnerStreamLayout.getSelectedItem().toString();
            Constants.VideoStreamType videoStreamType = Constants.VideoStreamType.valueOf(videoStream);
            engine.setRemoteVideoStreamType(uid, videoStreamType);
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
            if (!remoteViews.containsKey(uid)) {
                handler.post(() -> {
                    /*Display remote video stream*/
                    SurfaceView surfaceView = null;
                    // Create render view by RtcEngine
                    surfaceView = new SurfaceView(context);
                    surfaceView.setZOrderMediaOverlay(true);
                    Pair<VideoReportLayout, Spinner> availableContainer = getAvailableContainer();
                    if (availableContainer == null) {
                        return;
                    }

                    Spinner spinner = availableContainer.getSecond();
                    spinner.setVisibility(View.VISIBLE);
                    spinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
                        @Override
                        public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                            updateStream(spinner, uid);
                        }

                        @Override
                        public void onNothingSelected(AdapterView<?> parent) {

                        }
                    });

                    updateStream(spinner, uid);
                    VideoReportLayout view = availableContainer.getFirst();
                    view.setReportUid(uid);
                    remoteViews.put(uid, availableContainer);
                    // Add to the remote container
                    view.addView(surfaceView, new FrameLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));
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
            handler.post(() -> {
                /*Clear render view
                 Note: The video will stay at its last frame, to completely remove it you will need to
                 remove the SurfaceView from its parent*/
                Pair<VideoReportLayout, Spinner> view = remoteViews.get(uid);
                if (view != null) {
                    view.getFirst().removeAllViews();
                    remoteViews.remove(uid);
                    engine.setupRemoteVideo(new VideoCanvas(null, RENDER_MODE_HIDDEN, uid));
                    spinner_stream_layer.setVisibility(View.GONE);
                }
            });
        }

        @Override
        public void onLocalAudioStats(LocalAudioStats stats) {
            super.onLocalAudioStats(stats);
            fl_local.setLocalAudioStats(stats);
        }

        @Override
        public void onRemoteAudioStats(RemoteAudioStats stats) {
            super.onRemoteAudioStats(stats);
            fl_remote.setRemoteAudioStats(stats);
        }

        @Override
        public void onLocalVideoStats(Constants.VideoSourceType source, LocalVideoStats stats) {
            super.onLocalVideoStats(source, stats);
            fl_local.setLocalVideoStats(stats);
        }

        @Override
        public void onRemoteVideoStats(RemoteVideoStats stats) {
            super.onRemoteVideoStats(stats);
            fl_remote.setRemoteVideoStats(stats);
        }
    };

    private Pair<VideoReportLayout, Spinner> getAvailableContainer() {
        if (fl_remote.getChildCount() == 0) {
            return new Pair<>(fl_remote, spinner_stream_layer);
        } else {
            return null;
        }
    }
}
