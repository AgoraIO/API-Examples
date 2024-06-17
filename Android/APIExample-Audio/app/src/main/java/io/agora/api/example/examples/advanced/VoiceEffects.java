package io.agora.api.example.examples.advanced;

import static io.agora.api.example.common.model.Examples.ADVANCED;
import static io.agora.rtc2.Constants.AUDIO_EFFECT_OFF;
import static io.agora.rtc2.Constants.CHAT_BEAUTIFIER_FRESH;
import static io.agora.rtc2.Constants.CHAT_BEAUTIFIER_MAGNETIC;
import static io.agora.rtc2.Constants.CHAT_BEAUTIFIER_VITALITY;
import static io.agora.rtc2.Constants.PITCH_CORRECTION;
import static io.agora.rtc2.Constants.ROOM_ACOUSTICS_3D_VOICE;
import static io.agora.rtc2.Constants.ROOM_ACOUSTICS_ETHEREAL;
import static io.agora.rtc2.Constants.ROOM_ACOUSTICS_KTV;
import static io.agora.rtc2.Constants.ROOM_ACOUSTICS_PHONOGRAPH;
import static io.agora.rtc2.Constants.ROOM_ACOUSTICS_SPACIAL;
import static io.agora.rtc2.Constants.ROOM_ACOUSTICS_STUDIO;
import static io.agora.rtc2.Constants.ROOM_ACOUSTICS_VIRTUAL_STEREO;
import static io.agora.rtc2.Constants.ROOM_ACOUSTICS_VOCAL_CONCERT;
import static io.agora.rtc2.Constants.STYLE_TRANSFORMATION_POPULAR;
import static io.agora.rtc2.Constants.STYLE_TRANSFORMATION_RNB;
import static io.agora.rtc2.Constants.TIMBRE_TRANSFORMATION_CLEAR;
import static io.agora.rtc2.Constants.TIMBRE_TRANSFORMATION_DEEP;
import static io.agora.rtc2.Constants.TIMBRE_TRANSFORMATION_FALSETTO;
import static io.agora.rtc2.Constants.TIMBRE_TRANSFORMATION_FULL;
import static io.agora.rtc2.Constants.TIMBRE_TRANSFORMATION_MELLOW;
import static io.agora.rtc2.Constants.TIMBRE_TRANSFORMATION_RESOUNDING;
import static io.agora.rtc2.Constants.TIMBRE_TRANSFORMATION_RINGING;
import static io.agora.rtc2.Constants.TIMBRE_TRANSFORMATION_VIGOROUS;
import static io.agora.rtc2.Constants.ULTRA_HIGH_QUALITY_VOICE;
import static io.agora.rtc2.Constants.VOICE_BEAUTIFIER_OFF;
import static io.agora.rtc2.Constants.VOICE_CHANGER_BASS;
import static io.agora.rtc2.Constants.VOICE_CHANGER_CARTOON;
import static io.agora.rtc2.Constants.VOICE_CHANGER_CHILDLIKE;
import static io.agora.rtc2.Constants.VOICE_CHANGER_CHIPMUNK;
import static io.agora.rtc2.Constants.VOICE_CHANGER_DARTH_VADER;
import static io.agora.rtc2.Constants.VOICE_CHANGER_EFFECT_BOY;
import static io.agora.rtc2.Constants.VOICE_CHANGER_EFFECT_GIRL;
import static io.agora.rtc2.Constants.VOICE_CHANGER_EFFECT_HULK;
import static io.agora.rtc2.Constants.VOICE_CHANGER_EFFECT_OLDMAN;
import static io.agora.rtc2.Constants.VOICE_CHANGER_EFFECT_PIGKING;
import static io.agora.rtc2.Constants.VOICE_CHANGER_EFFECT_SISTER;
import static io.agora.rtc2.Constants.VOICE_CHANGER_EFFECT_UNCLE;
import static io.agora.rtc2.Constants.VOICE_CHANGER_GIRLISH_MAN;
import static io.agora.rtc2.Constants.VOICE_CHANGER_GROOT;
import static io.agora.rtc2.Constants.VOICE_CHANGER_IRON_LADY;
import static io.agora.rtc2.Constants.VOICE_CHANGER_MONSTER;
import static io.agora.rtc2.Constants.VOICE_CHANGER_NEUTRAL;
import static io.agora.rtc2.Constants.VOICE_CHANGER_PHONE_OPERATOR;
import static io.agora.rtc2.Constants.VOICE_CHANGER_SHIN_CHAN;
import static io.agora.rtc2.Constants.VOICE_CHANGER_SOLID;
import static io.agora.rtc2.Constants.VOICE_CHANGER_SWEET;
import static io.agora.rtc2.Constants.VOICE_CHANGER_TRANSFORMERS;
import static io.agora.rtc2.Constants.VOICE_CONVERSION_OFF;

import android.content.Context;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.SeekBar;
import android.widget.Spinner;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import com.yanzhenjie.permission.AndPermission;
import com.yanzhenjie.permission.runtime.Permission;

import java.util.Arrays;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;

import io.agora.api.example.MainApplication;
import io.agora.api.example.R;
import io.agora.api.example.annotation.Example;
import io.agora.api.example.common.BaseFragment;
import io.agora.api.example.common.widget.AudioSeatManager;
import io.agora.api.example.utils.CommonUtil;
import io.agora.api.example.utils.TokenUtils;
import io.agora.rtc2.ChannelMediaOptions;
import io.agora.rtc2.Constants;
import io.agora.rtc2.IRtcEngineEventHandler;
import io.agora.rtc2.RtcEngine;
import io.agora.rtc2.RtcEngineConfig;
import io.agora.rtc2.proxy.LocalAccessPointConfiguration;

/**
 * The type Voice effects.
 */
@Example(
        index = 4,
        group = ADVANCED,
        name = R.string.item_voiceeffects,
        actionId = R.id.action_mainFragment_to_VoiceEffects,
        tipsId = R.string.voiceeffects
)
public class VoiceEffects extends BaseFragment implements View.OnClickListener, AdapterView.OnItemSelectedListener, CompoundButton.OnCheckedChangeListener, SeekBar.OnSeekBarChangeListener {
    private static final String TAG = VoiceEffects.class.getSimpleName();

    private RtcEngine engine;
    private int myUid;
    private boolean joined = false;

    private EditText et_channel;
    private Button join;
    private Spinner audioProfile, audioScenario,
            chatBeautifier, timbreTransformation, voiceChanger, styleTransformation, roomAcoustics, pitchCorrection, _pitchModeOption, _pitchValueOption, voiceConversion, ainsMode, voiceAITuner,
            customBandFreq, customReverbKey;
    private ViewGroup _voice3DLayout, _pitchModeLayout, _pitchValueLayout;
    private SeekBar _voice3DCircle, customPitch, customBandGain, customReverbValue, customVoiceFormant;

    private AudioSeatManager audioSeatManager;

    @Override
    public void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        handler = new Handler();
    }

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment_voice_effects, container, false);
        return view;
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);

        // Join layout
        join = view.findViewById(R.id.btn_join);
        audioProfile = view.findViewById(R.id.audio_profile_spinner);
        audioScenario = view.findViewById(R.id.audio_scenario_spinner);
        et_channel = view.findViewById(R.id.et_channel);

        audioScenario.setOnItemSelectedListener(this);
        join.setOnClickListener(this);

        // Voice Beautifier / Effects Preset layout
        chatBeautifier = view.findViewById(R.id.audio_chat_beautifier);
        timbreTransformation = view.findViewById(R.id.audio_timbre_transformation);
        voiceChanger = view.findViewById(R.id.audio_voice_changer);
        styleTransformation = view.findViewById(R.id.audio_style_transformation);
        roomAcoustics = view.findViewById(R.id.audio_room_acoustics);
        _voice3DLayout = view.findViewById(R.id.audio_3d_voice_layout);
        _voice3DCircle = view.findViewById(R.id.audio_3d_voice_circle);
        pitchCorrection = view.findViewById(R.id.audio_pitch_correction);
        _pitchModeLayout = view.findViewById(R.id.audio_pitch_mode_layout);
        _pitchModeOption = view.findViewById(R.id.audio_pitch_mode_option);
        _pitchValueLayout = view.findViewById(R.id.audio_pitch_value_layout);
        _pitchValueOption = view.findViewById(R.id.audio_pitch_value_option);
        voiceConversion = view.findViewById(R.id.audio_voice_conversion);
        ainsMode = view.findViewById(R.id.audio_ains_mode);
        voiceAITuner = view.findViewById(R.id.voice_ai_tuner);

        chatBeautifier.setOnItemSelectedListener(this);
        timbreTransformation.setOnItemSelectedListener(this);
        voiceChanger.setOnItemSelectedListener(this);
        styleTransformation.setOnItemSelectedListener(this);
        roomAcoustics.setOnItemSelectedListener(this);
        pitchCorrection.setOnItemSelectedListener(this);
        voiceConversion.setOnItemSelectedListener(this);
        _voice3DCircle.setOnSeekBarChangeListener(this);
        _pitchModeOption.setOnItemSelectedListener(this);
        _pitchValueOption.setOnItemSelectedListener(this);
        ainsMode.setOnItemSelectedListener(this);
        voiceAITuner.setOnItemSelectedListener(this);

        // Customize Voice Effects Layout
        customPitch = view.findViewById(R.id.audio_custom_pitch); // engine.setLocalVoicePitch()
        customBandFreq = view.findViewById(R.id.audio_custom_band_freq); // engine.setLocalVoiceEqualization()
        customBandGain = view.findViewById(R.id.audio_custom_band_gain); // engine.setLocalVoiceEqualization()
        customReverbKey = view.findViewById(R.id.audio_custom_reverb_key);
        customReverbValue = view.findViewById(R.id.audio_custom_reverb_value); //engine.setLocalVoiceReverb()
        customVoiceFormant = view.findViewById(R.id.audio_voice_formant_value); //engine.setLocalVoiceFormant()

        customPitch.setOnSeekBarChangeListener(this);
        customBandGain.setOnSeekBarChangeListener(this);
        customReverbValue.setOnSeekBarChangeListener(this);
        customVoiceFormant.setOnSeekBarChangeListener(this);
        customBandFreq.setOnItemSelectedListener(this);
        customReverbKey.setOnItemSelectedListener(this);


        audioSeatManager = new AudioSeatManager(
                view.findViewById(R.id.audio_place_01),
                view.findViewById(R.id.audio_place_02)
        );

        resetControlLayoutByJoined();
    }

    private void resetControlLayoutByJoined() {
        audioProfile.setEnabled(!joined);

        chatBeautifier.setEnabled(joined);
        timbreTransformation.setEnabled(joined);
        voiceChanger.setEnabled(joined);
        styleTransformation.setEnabled(joined);
        roomAcoustics.setEnabled(joined);
        _voice3DLayout.setVisibility(View.GONE);
        pitchCorrection.setEnabled(joined);
        _pitchModeLayout.setVisibility(View.GONE);
        _pitchValueLayout.setVisibility(View.GONE);
        voiceConversion.setEnabled(joined);
        ainsMode.setEnabled(joined);
        voiceAITuner.setEnabled(joined);

        customPitch.setEnabled(joined);
        customBandFreq.setEnabled(joined);
        customBandGain.setEnabled(joined);
        customReverbKey.setEnabled(joined);
        customReverbValue.setEnabled(joined);
        customVoiceFormant.setEnabled(joined);


        chatBeautifier.setSelection(0);
        voiceChanger.setSelection(0);
        timbreTransformation.setSelection(0);
        roomAcoustics.setSelection(0);
        pitchCorrection.setSelection(0);
        voiceConversion.setSelection(0);
        ainsMode.setSelection(0);

        customPitch.setProgress(0);
        customBandGain.setProgress(0);
        customReverbValue.setProgress(0);
        customVoiceFormant.setProgress(50);
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
                        Permission.Group.MICROPHONE
                ).onGranted(permissions -> {
                    // Permissions Granted
                    joinChannel(channelId);
                }).start();
            } else {
                joined = false;
                resetControlLayoutByJoined();
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
                audioSeatManager.downAllSeats();
            }
        }
    }

    private int getPitch1Value(String str) {
        switch (str) {
            case "Natural Minor":
                return 2;
            case "Breeze Minor":
                return 3;
            default:
                return 1;
        }
    }

    private int getPitch2Value(String str) {
        switch (str) {
            case "A Pitch":
                return 1;
            case "A# Pitch":
                return 2;
            case "B Pitch":
                return 3;
            case "C# Pitch":
                return 5;
            case "D Pitch":
                return 6;
            case "D# Pitch":
                return 7;
            case "E Pitch":
                return 8;
            case "F Pitch":
                return 9;
            case "F# Pitch":
                return 10;
            case "G Pitch":
                return 11;
            case "G# Pitch":
                return 12;
            default:
                return 4;
        }
    }

    /**
     * @param channelId Specify the channel name that you want to join.
     *                  Users that input the same channel name join the same channel.
     */
    private void joinChannel(String channelId) {
        /*In the demo, the default is to enter as the anchor.*/
        engine.setClientRole(Constants.CLIENT_ROLE_BROADCASTER);
        // audio config
        engine.setAudioProfile(
                Constants.AudioProfile.getValue(Constants.AudioProfile.valueOf(audioProfile.getSelectedItem().toString())),
                Constants.AudioScenario.getValue(Constants.AudioScenario.valueOf(audioScenario.getSelectedItem().toString()))
        );

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
            int res = engine.joinChannel(accessToken, channelId, 0, option);
            if (res != 0) {
                // Usually happens with invalid parameters
                // Error code description can be found at:
                // en: https://docs.agora.io/en/Voice/API%20Reference/java/classio_1_1agora_1_1rtc_1_1_i_rtc_engine_event_handler_1_1_error_code.html
                // cn: https://docs.agora.io/cn/Voice/API%20Reference/java/classio_1_1agora_1_1rtc_1_1_i_rtc_engine_event_handler_1_1_error_code.html
                showAlert(RtcEngine.getErrorDescription(Math.abs(res)));
                Log.e(TAG, RtcEngine.getErrorDescription(Math.abs(res)));
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
                    resetControlLayoutByJoined();
                    audioSeatManager.upLocalSeat(uid);
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
         *   @param elapsed Time elapsed (ms) from the local user calling the joinChannel method
         *                  until the SDK triggers this callback.*/
        @Override
        public void onRemoteAudioStateChanged(int uid, int state, int reason, int elapsed) {
            super.onRemoteAudioStateChanged(uid, state, reason, elapsed);
            Log.i(TAG, "onRemoteAudioStateChanged->" + uid + ", state->" + state + ", reason->" + reason);
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
            runOnUIThread(() -> audioSeatManager.upRemoteSeat(uid));
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
            runOnUIThread(() -> audioSeatManager.downSeat(uid));
        }

        @Override
        public void onLocalAudioStats(LocalAudioStats stats) {
            super.onLocalAudioStats(stats);
            runOnUIThread(() -> {
                Map<String, String> _stats = new LinkedHashMap<>();
                _stats.put("sentSampleRate", stats.sentSampleRate + "");
                _stats.put("sentBitrate", stats.sentBitrate + " kbps");
                _stats.put("internalCodec", stats.internalCodec + "");
                _stats.put("audioDeviceDelay", stats.audioDeviceDelay + " ms");
                audioSeatManager.getLocalSeat().updateStats(_stats);
            });
        }

        @Override
        public void onRemoteAudioStats(RemoteAudioStats stats) {
            super.onRemoteAudioStats(stats);
            runOnUIThread(() -> {
                Map<String, String> _stats = new LinkedHashMap<>();
                _stats.put("numChannels", stats.numChannels + "");
                _stats.put("receivedBitrate", stats.receivedBitrate + " kbps");
                _stats.put("audioLossRate", stats.audioLossRate + "");
                _stats.put("jitterBufferDelay", stats.jitterBufferDelay + " ms");
                audioSeatManager.getRemoteSeat(stats.uid).updateStats(_stats);
            });
        }
    };

    @Override
    public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
        if (parent.getTag() != null) {
            parent.setTag(null);
            return;
        }

        if (parent == audioScenario) {
            engine.setAudioScenario(Constants.AudioScenario.getValue(Constants.AudioScenario.valueOf(audioScenario.getSelectedItem().toString())));
            return;
        }

        // Voice Beautifier / Effects Preset layout
        List<Spinner> voiceBeautifierSpinner = Arrays.asList(chatBeautifier, timbreTransformation);
        if (voiceBeautifierSpinner.contains(parent)) {
            String item = parent.getSelectedItem().toString();
            int voiceBeautifierValue = getVoiceBeautifierValue(item);
            engine.setVoiceBeautifierPreset(voiceBeautifierValue);

            for (Spinner spinner : voiceBeautifierSpinner) {
                if (spinner != parent) {
                    if (spinner.getSelectedItemPosition() != 0) {
                        spinner.setTag("reset");
                        spinner.setSelection(0);
                    }
                }
            }
            return;
        }

        List<Spinner> audioEffectSpinner = Arrays.asList(voiceChanger, styleTransformation, roomAcoustics, pitchCorrection);
        if (audioEffectSpinner.contains(parent)) {
            String item = parent.getSelectedItem().toString();
            int audioEffectPreset = getAudioEffectPreset(item);
            engine.setAudioEffectPreset(audioEffectPreset);

            for (Spinner spinner : audioEffectSpinner) {
                if (spinner != parent) {
                    if (spinner.getSelectedItemPosition() != 0) {
                        spinner.setTag("reset");
                        spinner.setSelection(0);
                    }
                }
            }

            _voice3DLayout.setVisibility(audioEffectPreset == ROOM_ACOUSTICS_3D_VOICE ? View.VISIBLE : View.GONE);
            _pitchModeLayout.setVisibility(audioEffectPreset == PITCH_CORRECTION ? View.VISIBLE : View.GONE);
            _pitchValueLayout.setVisibility(audioEffectPreset == PITCH_CORRECTION ? View.VISIBLE : View.GONE);
            return;
        }

        if (parent == voiceConversion) {
            String item = parent.getSelectedItem().toString();
            engine.setVoiceConversionPreset(getVoiceConversionValue(item));
            return;
        }


        if (parent == _pitchModeOption || parent == _pitchValueOption) {
            int effectOption1 = getPitch1Value(_pitchModeOption.getSelectedItem().toString());
            int effectOption2 = getPitch2Value(_pitchValueOption.getSelectedItem().toString());
            engine.setAudioEffectParameters(PITCH_CORRECTION, effectOption1, effectOption2);
        }

        if (parent == ainsMode) {
            boolean enable = position > 0;
            /*
            The AI noise suppression modes:
                0: (Default) Balance mode. This mode allows for a balanced performance on noice suppression and time delay.
                1: Aggressive mode. In scenarios where high performance on noise suppression is required, such as live streaming
                outdoor events, this mode reduces nosies more dramatically, but sometimes may affect the original character of the audio.
                2: Aggressive mode with low latency. The noise suppression delay of this mode is about only half of that of the balance
                and aggressive modes. It is suitable for scenarios that have high requirements on noise suppression with low latency,
                such as sing together online in real time.
             */
            engine.setAINSMode(enable, position - 1);
        }

        if (parent == voiceAITuner) {
            boolean enable = position > 0;
            String item = parent.getSelectedItem().toString();
            engine.enableVoiceAITuner(enable, enable ? Constants.VOICE_AI_TUNER_TYPE.valueOf(item) : Constants.VOICE_AI_TUNER_TYPE.VOICE_AI_TUNER_MATURE_MALE);
        }
    }

    private int getVoiceConversionValue(String label) {
        switch (label) {
            case "VOICE_CHANGER_NEUTRAL":
                return VOICE_CHANGER_NEUTRAL;
            case "VOICE_CHANGER_SWEET":
                return VOICE_CHANGER_SWEET;
            case "VOICE_CHANGER_SOLID":
                return VOICE_CHANGER_SOLID;
            case "VOICE_CHANGER_BASS":
                return VOICE_CHANGER_BASS;
            case "VOICE_CHANGER_CARTOON":
                return VOICE_CHANGER_CARTOON;
            case "VOICE_CHANGER_CHILDLIKE":
                return VOICE_CHANGER_CHILDLIKE;
            case "VOICE_CHANGER_PHONE_OPERATOR":
                return VOICE_CHANGER_PHONE_OPERATOR;
            case "VOICE_CHANGER_MONSTER":
                return VOICE_CHANGER_MONSTER;
            case "VOICE_CHANGER_TRANSFORMERS":
                return VOICE_CHANGER_TRANSFORMERS;
            case "VOICE_CHANGER_GROOT":
                return VOICE_CHANGER_GROOT;
            case "VOICE_CHANGER_DARTH_VADER":
                return VOICE_CHANGER_DARTH_VADER;
            case "VOICE_CHANGER_IRON_LADY":
                return VOICE_CHANGER_IRON_LADY;
            case "VOICE_CHANGER_SHIN_CHAN":
                return VOICE_CHANGER_SHIN_CHAN;
            case "VOICE_CHANGER_GIRLISH_MAN":
                return VOICE_CHANGER_GIRLISH_MAN;
            case "VOICE_CHANGER_CHIPMUNK":
                return VOICE_CHANGER_CHIPMUNK;
            case "VOICE_CONVERSION_OFF":
            default:
                return VOICE_CONVERSION_OFF;
        }
    }

    private int getVoiceBeautifierValue(String label) {
        int value;
        switch (label) {
            case "CHAT_BEAUTIFIER_MAGNETIC":
                value = CHAT_BEAUTIFIER_MAGNETIC;
                break;
            case "CHAT_BEAUTIFIER_FRESH":
                value = CHAT_BEAUTIFIER_FRESH;
                break;
            case "CHAT_BEAUTIFIER_VITALITY":
                value = CHAT_BEAUTIFIER_VITALITY;
                break;
            case "TIMBRE_TRANSFORMATION_VIGOROUS":
                value = TIMBRE_TRANSFORMATION_VIGOROUS;
                break;
            case "TIMBRE_TRANSFORMATION_DEEP":
                value = TIMBRE_TRANSFORMATION_DEEP;
                break;
            case "TIMBRE_TRANSFORMATION_MELLOW":
                value = TIMBRE_TRANSFORMATION_MELLOW;
                break;
            case "TIMBRE_TRANSFORMATION_FALSETTO":
                value = TIMBRE_TRANSFORMATION_FALSETTO;
                break;
            case "TIMBRE_TRANSFORMATION_FULL":
                value = TIMBRE_TRANSFORMATION_FULL;
                break;
            case "TIMBRE_TRANSFORMATION_CLEAR":
                value = TIMBRE_TRANSFORMATION_CLEAR;
                break;
            case "TIMBRE_TRANSFORMATION_RESOUNDING":
                value = TIMBRE_TRANSFORMATION_RESOUNDING;
                break;
            case "TIMBRE_TRANSFORMATION_RINGING":
                value = TIMBRE_TRANSFORMATION_RINGING;
                break;
            case "ULTRA_HIGH_QUALITY_VOICE":
                value = ULTRA_HIGH_QUALITY_VOICE;
                break;
            default:
                value = VOICE_BEAUTIFIER_OFF;
        }
        return value;
    }

    private int getAudioEffectPreset(String label) {
        int value;
        switch (label) {
            case "ROOM_ACOUSTICS_KTV":
                value = ROOM_ACOUSTICS_KTV;
                break;
            case "ROOM_ACOUSTICS_VOCAL_CONCERT":
                value = ROOM_ACOUSTICS_VOCAL_CONCERT;
                break;
            case "ROOM_ACOUSTICS_STUDIO":
                value = ROOM_ACOUSTICS_STUDIO;
                break;
            case "ROOM_ACOUSTICS_PHONOGRAPH":
                value = ROOM_ACOUSTICS_PHONOGRAPH;
                break;
            case "ROOM_ACOUSTICS_VIRTUAL_STEREO":
                value = ROOM_ACOUSTICS_VIRTUAL_STEREO;
                break;
            case "ROOM_ACOUSTICS_SPACIAL":
                value = ROOM_ACOUSTICS_SPACIAL;
                break;
            case "ROOM_ACOUSTICS_ETHEREAL":
                value = ROOM_ACOUSTICS_ETHEREAL;
                break;
            case "ROOM_ACOUSTICS_3D_VOICE":
                value = ROOM_ACOUSTICS_3D_VOICE;
                break;
            case "VOICE_CHANGER_EFFECT_UNCLE":
                value = VOICE_CHANGER_EFFECT_UNCLE;
                break;
            case "VOICE_CHANGER_EFFECT_OLDMAN":
                value = VOICE_CHANGER_EFFECT_OLDMAN;
                break;
            case "VOICE_CHANGER_EFFECT_BOY":
                value = VOICE_CHANGER_EFFECT_BOY;
                break;
            case "VOICE_CHANGER_EFFECT_SISTER":
                value = VOICE_CHANGER_EFFECT_SISTER;
                break;
            case "VOICE_CHANGER_EFFECT_GIRL":
                value = VOICE_CHANGER_EFFECT_GIRL;
                break;
            case "VOICE_CHANGER_EFFECT_PIGKING":
                value = VOICE_CHANGER_EFFECT_PIGKING;
                break;
            case "VOICE_CHANGER_EFFECT_HULK":
                value = VOICE_CHANGER_EFFECT_HULK;
                break;
            case "STYLE_TRANSFORMATION_RNB":
                value = STYLE_TRANSFORMATION_RNB;
                break;
            case "STYLE_TRANSFORMATION_POPULAR":
                value = STYLE_TRANSFORMATION_POPULAR;
                break;
            case "PITCH_CORRECTION":
                value = PITCH_CORRECTION;
                break;
            default:
                value = AUDIO_EFFECT_OFF;
        }
        return value;
    }

    @Override
    public void onNothingSelected(AdapterView<?> parent) {

    }

    @Override
    public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {

    }

    @Override
    public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
        if (!fromUser) {
            return;
        }
        if (seekBar == _voice3DCircle) {
            int cicle = (int) (1 + 59 * progress * 1.0f / seekBar.getMax());
            // [1,60], 10 default
            engine.setAudioEffectParameters(ROOM_ACOUSTICS_3D_VOICE, cicle, 0);
        } else if (seekBar == customPitch) {
            double pitch = 0.5 + 1.5 * progress * 1.0f / seekBar.getMax();
            // pitch: [0.5,2.0], 1.0 default
            engine.setLocalVoicePitch(pitch);
        } else if (seekBar == customBandGain) {
            int value = (int) (-15 + 30 * progress * 1.0f / seekBar.getMax());
            //  [-15,15], 0 default
            engine.setLocalVoiceEqualization(Constants.AUDIO_EQUALIZATION_BAND_FREQUENCY.valueOf(customBandFreq.getSelectedItem().toString()), value);
        } else if (seekBar == customReverbValue) {
            Constants.AUDIO_REVERB_TYPE reverbKey = Constants.AUDIO_REVERB_TYPE.valueOf(customReverbKey.getSelectedItem().toString());
            int value;
            // AUDIO_REVERB_DRY_LEVEL(0)：dry signal， [-20, 10] dB
            // AUDIO_REVERB_WET_LEVEL(1)：wet signal， [-20, 10] dB
            // AUDIO_REVERB_ROOM_SIZE(2)：[0, 100] dB
            // AUDIO_REVERB_WET_DELAY(3)：Wet signal,  [0, 200] ms
            // AUDIO_REVERB_STRENGTH(4)： [0, 100]
            if (reverbKey == Constants.AUDIO_REVERB_TYPE.AUDIO_REVERB_DRY_LEVEL || reverbKey == Constants.AUDIO_REVERB_TYPE.AUDIO_REVERB_WET_LEVEL) {
                value = (int) (-20 + 30 * progress * 1.0f / seekBar.getMax());
            } else if (reverbKey == Constants.AUDIO_REVERB_TYPE.AUDIO_REVERB_WET_DELAY) {
                value = (int) (200 * progress * 1.0f / seekBar.getMax());
            } else {
                value = (int) (100 * progress * 1.0f / seekBar.getMax());
            }
            engine.setLocalVoiceReverb(reverbKey, value);
        } else if (seekBar == customVoiceFormant) {
            // [-1, 1]
            double value = (progress - 50) * 1.0f / 100;
            engine.setLocalVoiceFormant(value);
        }
    }

    @Override
    public void onStartTrackingTouch(SeekBar seekBar) {

    }

    @Override
    public void onStopTrackingTouch(SeekBar seekBar) {

    }
}
