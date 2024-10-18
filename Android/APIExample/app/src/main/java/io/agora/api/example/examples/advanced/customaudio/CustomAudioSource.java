package io.agora.api.example.examples.advanced.customaudio;

import static io.agora.api.example.common.model.Examples.ADVANCED;

import android.content.Context;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.Switch;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import io.agora.api.example.MainApplication;
import io.agora.api.example.R;
import io.agora.api.example.annotation.Example;
import io.agora.api.example.common.BaseFragment;
import io.agora.api.example.common.widget.AudioSeatManager;
import io.agora.api.example.utils.AudioFileReader;
import io.agora.api.example.utils.CommonUtil;
import io.agora.api.example.utils.PermissonUtils;
import io.agora.api.example.utils.TokenUtils;
import io.agora.rtc2.ChannelMediaOptions;
import io.agora.rtc2.Constants;
import io.agora.rtc2.IRtcEngineEventHandler;
import io.agora.rtc2.RtcEngine;
import io.agora.rtc2.RtcEngineConfig;
import io.agora.rtc2.RtcEngineEx;
import io.agora.rtc2.audio.AudioTrackConfig;
import io.agora.rtc2.proxy.LocalAccessPointConfiguration;

/**
 * This demo demonstrates how to make a one-to-one voice call
 */
@Example(index = 5, group = ADVANCED, name = R.string.item_customaudiosource, actionId = R.id.action_mainFragment_to_CustomAudioSource, tipsId = R.string.customaudio)
public class CustomAudioSource extends BaseFragment implements View.OnClickListener, CompoundButton.OnCheckedChangeListener {
    private static final String TAG = CustomAudioSource.class.getSimpleName();
    private EditText et_channel;
    private Button join;
    private int myUid;
    private boolean joined = false;
    /**
     * The constant engine.
     */
    public static RtcEngineEx engine;
    private Switch mic, pcm;
    private ChannelMediaOptions option = new ChannelMediaOptions();
    private int pushTimes = 0;

    private AudioSeatManager audioSeatManager;
    private AudioFileReader audioPushingHelper;
    private int customAudioTrack = -1;

    @Override
    public void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        handler = new Handler();
        initMediaOption();
    }

    private void initMediaOption() {
        option.autoSubscribeAudio = true;
        option.autoSubscribeVideo = true;
        option.publishMicrophoneTrack = true;
        option.publishCustomAudioTrack = false;
        option.clientRoleType = Constants.CLIENT_ROLE_BROADCASTER;
        option.enableAudioRecordingOrPlayout = true;
    }

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment_custom_audio_source, container, false);
        return view;
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        join = view.findViewById(R.id.btn_join);
        et_channel = view.findViewById(R.id.et_channel);
        view.findViewById(R.id.btn_join).setOnClickListener(this);
        mic = view.findViewById(R.id.microphone);
        pcm = view.findViewById(R.id.localAudio);
        mic.setOnCheckedChangeListener(this);
        pcm.setOnCheckedChangeListener(this);

        audioSeatManager = new AudioSeatManager(
                view.findViewById(R.id.audio_place_01),
                view.findViewById(R.id.audio_place_02),
                view.findViewById(R.id.audio_place_03),
                view.findViewById(R.id.audio_place_04),
                view.findViewById(R.id.audio_place_05),
                view.findViewById(R.id.audio_place_06),
                view.findViewById(R.id.audio_place_07),
                view.findViewById(R.id.audio_place_08),
                view.findViewById(R.id.audio_place_09)
        );
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
            engine = (RtcEngineEx) RtcEngine.create(config);
            /*
             * This parameter is for reporting the usages of APIExample to agora background.
             * Generally, it is not necessary for you to set this parameter.
             */
            engine.setParameters("{" + "\"rtc.report_app_scenario\":"
                    + "{" + "\"appScenario\":"
                    + 100 + "," + "\"serviceType\":" + 11 + ","
                    + "\"appVersion\":\"" + RtcEngine.getSdkVersion() + "\"" + "}" + "}");
            /* setting the local access point if the private cloud ip was set, otherwise the config will be invalid.*/
            LocalAccessPointConfiguration localAccessPointConfiguration = ((MainApplication) getActivity().getApplication()).getGlobalSettings().getPrivateCloudConfig();
            if (localAccessPointConfiguration != null) {
                // This api can only be used in the private media server scenario, otherwise some problems may occur.
                engine.setLocalAccessPoint(localAccessPointConfiguration);
            }

            audioPushingHelper = new AudioFileReader(requireContext(), (buffer, timestamp) -> {
                if (joined && engine != null && customAudioTrack != -1) {
                    int ret = engine.pushExternalAudioFrame(buffer, timestamp,
                            AudioFileReader.SAMPLE_RATE,
                            AudioFileReader.SAMPLE_NUM_OF_CHANNEL,
                            Constants.BytesPerSample.TWO_BYTES_PER_SAMPLE,
                            customAudioTrack);
                    Log.i(TAG, "pushExternalAudioFrame times:" + (++pushTimes) + ", ret=" + ret);
                }
            });
        } catch (Exception e) {
            e.printStackTrace();
            getActivity().onBackPressed();
        }
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        if (customAudioTrack != -1) {
            engine.destroyCustomAudioTrack(customAudioTrack);
            customAudioTrack = -1;
        }
        if (audioPushingHelper != null) {
            audioPushingHelper.stop();
        }
        /*leaveChannel and Destroy the RtcEngine instance*/
        if (engine != null) {
            engine.leaveChannel();
        }
        handler.post(RtcEngine::destroy);
        engine = null;
    }


    @Override
    public void onCheckedChanged(CompoundButton compoundButton, boolean checked) {
        if (compoundButton.getId() == R.id.microphone) {
            option.publishMicrophoneTrack = checked;
            engine.updateChannelMediaOptions(option);
        } else if (compoundButton.getId() == R.id.localAudio) {
            option.publishCustomAudioTrackId = customAudioTrack;
            option.publishCustomAudioTrack = checked;
            engine.updateChannelMediaOptions(option);
            engine.enableCustomAudioLocalPlayback(customAudioTrack, checked);
        }
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
                mic.setEnabled(false);
                pcm.setEnabled(false);
                pcm.setChecked(false);
                mic.setChecked(true);
                if (audioPushingHelper != null) {
                    audioPushingHelper.stop();
                }
                audioSeatManager.downAllSeats();
            }
        }
    }

    /**
     * @param channelId Specify the channel name that you want to join.
     *                  Users that input the same channel name join the same channel.
     */
    private void joinChannel(String channelId) {
        /*In the demo, the default is to enter as the anchor.*/
        engine.setClientRole(Constants.CLIENT_ROLE_BROADCASTER);
        /*Sets the external audio source.
         * @param enabled Sets whether to enable/disable the external audio source:
         *                  true: Enable the external audio source.
         *                  false: (Default) Disable the external audio source.
         * @param sampleRate Sets the sample rate (Hz) of the external audio source, which can be
         *                   set as 8000, 16000, 32000, 44100, or 48000 Hz.
         * @param channels Sets the number of channels of the external audio source:
         *                  1: Mono.
         *                  2: Stereo.
         * @return
         *   0: Success.
         *   < 0: Failure.
         * PS: Ensure that you call this method before the joinChannel method.*/
        AudioTrackConfig config = new AudioTrackConfig();
        config.enableLocalPlayback = false;
        customAudioTrack = engine.createCustomAudioTrack(Constants.AudioTrackType.AUDIO_TRACK_MIXABLE, config);

        /*Please configure accessToken in the string_config file.
         * A temporary token generated in Console. A temporary token is valid for 24 hours. For details, see
         *      https://docs.agora.io/en/Agora%20Platform/token?platform=All%20Platforms#get-a-temporary-token
         * A token generated at the server. This applies to scenarios with high-security requirements. For details, see
         *      https://docs.agora.io/en/cloud-recording/token_server_java?platform=Java*/
        TokenUtils.gen(requireContext(), channelId, 0, ret -> {

            /* Allows a user to join a channel.
             if you do not specify the uid, we will generate the uid for you*/
            int res = engine.joinChannel(ret, channelId, 0, option);
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
                    mic.setEnabled(true);
                    pcm.setEnabled(true);
                    join.setEnabled(true);
                    join.setText(getString(R.string.leave));
                    if (audioPushingHelper != null) {
                        pushTimes = 0;
                        audioPushingHelper.start();
                    }
                    audioSeatManager.upLocalSeat(uid);
                    if (pcm.isChecked()) {
                        engine.enableCustomAudioLocalPlayback(0, true);
                    }
                }
            });
        }


        @Override
        public void onUserJoined(int uid, int elapsed) {
            super.onUserJoined(uid, elapsed);
            runOnUIThread(() -> audioSeatManager.upRemoteSeat(uid));
        }

        @Override
        public void onUserOffline(int uid, int reason) {
            super.onUserOffline(uid, reason);
            runOnUIThread(() -> audioSeatManager.downSeat(uid));
        }
    };

}
