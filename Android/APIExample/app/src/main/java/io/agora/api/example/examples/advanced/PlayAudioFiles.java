package io.agora.api.example.examples.advanced;

import static io.agora.api.example.common.model.Examples.ADVANCED;

import android.content.Context;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.EditText;
import android.widget.SeekBar;
import android.widget.Spinner;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import java.util.LinkedHashMap;
import java.util.Map;

import io.agora.api.example.MainApplication;
import io.agora.api.example.R;
import io.agora.api.example.annotation.Example;
import io.agora.api.example.common.BaseFragment;
import io.agora.api.example.common.Constant;
import io.agora.api.example.common.widget.AudioSeatManager;
import io.agora.api.example.utils.CommonUtil;
import io.agora.api.example.utils.PermissonUtils;
import io.agora.api.example.utils.TokenUtils;
import io.agora.rtc2.ChannelMediaOptions;
import io.agora.rtc2.Constants;
import io.agora.rtc2.IAudioEffectManager;
import io.agora.rtc2.IRtcEngineEventHandler;
import io.agora.rtc2.RtcEngine;
import io.agora.rtc2.RtcEngineConfig;
import io.agora.rtc2.proxy.LocalAccessPointConfiguration;

/**
 * The type Play audio files.
 */
@Example(
        index = 15,
        group = ADVANCED,
        name = R.string.item_playaudiofiles,
        actionId = R.id.action_mainFragment_to_PlayAudioFiles,
        tipsId = R.string.playaudiofiles
)
public class PlayAudioFiles extends BaseFragment implements View.OnClickListener,
        SeekBar.OnSeekBarChangeListener, AdapterView.OnItemSelectedListener {
    private static final String TAG = PlayAudioFiles.class.getSimpleName();
    private static final int EFFECT_SOUND_ID = 0;
    private EditText et_channel;
    private Button join;
    private Spinner audioProfile, audioScenario;
    private TextView mixingStart, mixingResume, mixingPause, mixingStop,
            effectStart, effectResume, effectPause, effectStop;
    private SeekBar mixingPublishVolBar, mixingPlayoutVolBar, mixingVolBar, effectVolBar;
    private RtcEngine engine;
    private int myUid;
    private boolean joined = false;
    private IAudioEffectManager audioEffectManager;

    private AudioSeatManager audioSeatManager;

    @Override
    public void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        handler = new Handler();
    }

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment_play_audio_files, container, false);
        return view;
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);

        join = view.findViewById(R.id.btn_join);
        et_channel = view.findViewById(R.id.et_channel);
        join.setOnClickListener(this);
        audioProfile = view.findViewById(R.id.audio_profile_spinner);
        audioScenario = view.findViewById(R.id.audio_scenario_spinner);
        audioScenario.setOnItemSelectedListener(this);

        // mixing
        mixingStart = view.findViewById(R.id.mixing_start);
        mixingResume = view.findViewById(R.id.mixing_resume);
        mixingPause = view.findViewById(R.id.mixing_pause);
        mixingStop = view.findViewById(R.id.mixing_stop);
        mixingVolBar = view.findViewById(R.id.mixingVolBar);
        mixingPlayoutVolBar = view.findViewById(R.id.mixingPlayoutVolBar);
        mixingPublishVolBar = view.findViewById(R.id.mixingPublishVolBar);

        mixingStart.setOnClickListener(this);
        mixingResume.setOnClickListener(this);
        mixingPause.setOnClickListener(this);
        mixingStop.setOnClickListener(this);
        mixingVolBar.setOnSeekBarChangeListener(this);
        mixingPlayoutVolBar.setOnSeekBarChangeListener(this);
        mixingPublishVolBar.setOnSeekBarChangeListener(this);

        // effect
        effectStart = view.findViewById(R.id.effect_start);
        effectResume = view.findViewById(R.id.effect_resume);
        effectPause = view.findViewById(R.id.effect_pause);
        effectStop = view.findViewById(R.id.effect_stop);
        effectVolBar = view.findViewById(R.id.effectVolBar);

        effectStart.setOnClickListener(this);
        effectResume.setOnClickListener(this);
        effectPause.setOnClickListener(this);
        effectStop.setOnClickListener(this);
        effectVolBar.setOnSeekBarChangeListener(this);

        audioSeatManager = new AudioSeatManager(
                view.findViewById(R.id.audio_place_01),
                view.findViewById(R.id.audio_place_02)
        );

        resetLayoutByJoin();
    }

    private void resetLayoutByJoin() {
        audioProfile.setEnabled(!joined);

        mixingStart.setClickable(joined);
        mixingResume.setClickable(joined);
        mixingPause.setClickable(joined);
        mixingStop.setClickable(joined);
        mixingVolBar.setEnabled(joined);
        mixingPlayoutVolBar.setEnabled(joined);
        mixingPublishVolBar.setEnabled(joined);

        effectStart.setClickable(joined);
        effectResume.setClickable(joined);
        effectPause.setClickable(joined);
        effectStop.setClickable(joined);
        effectVolBar.setEnabled(joined);
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
            preloadAudioEffect();
        } catch (Exception e) {
            e.printStackTrace();
            getActivity().onBackPressed();
        }
    }

    /**
     * To ensure smooth communication, limit the size of the audio effect file.
     * We recommend using this method to preload the audio effect before calling the joinChannel method.
     */
    private void preloadAudioEffect() {
        // Gets the global audio effect manager.
        audioEffectManager = engine.getAudioEffectManager();
        // Preloads the audio effect (recommended). Note the file size, and preload the file before joining the channel.
        // Only mp3, aac, m4a, 3gp, and wav files are supported.
        // You may need to record the sound IDs and their file paths.
        audioEffectManager.preloadEffect(EFFECT_SOUND_ID, Constant.EFFECT_FILE_PATH);
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
    public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
        if (parent == audioScenario) {
            engine.setAudioScenario(Constants.AudioScenario.getValue(Constants.AudioScenario.valueOf(audioScenario.getSelectedItem().toString())));
        }
    }

    @Override
    public void onNothingSelected(AdapterView<?> parent) {

    }

    @Override
    public void onClick(View v) {
        if (v == join) {
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
                resetLayoutByJoin();
                audioSeatManager.downAllSeats();
            }
        } else if (v == mixingStart) {
            int ret = engine.startAudioMixing(Constant.MIX_FILE_PATH, false, -1, 0);
            Log.i(TAG, "startAudioMixing >> ret=" + ret);
        } else if (v == mixingResume) {
            int ret = engine.resumeAudioMixing();
            Log.i(TAG, "resumeAudioMixing >> ret=" + ret);
        } else if (v == mixingPause) {
            int ret = engine.pauseAudioMixing();
            Log.i(TAG, "pauseAudioMixing >> ret=" + ret);
        } else if (v == mixingStop) {
            int ret = engine.stopAudioMixing();
            Log.i(TAG, "stopAudioMixing >> ret=" + ret);
        } else if (v == effectStart) {
            /*Plays an audio effect file.
             * Returns
             * 0: Success.
             * < 0: Failure.
             */
            int playRet = audioEffectManager.playEffect(
                EFFECT_SOUND_ID, // The sound ID of the audio effect file to be played.
                Constant.EFFECT_FILE_PATH, // The file path of the audio effect file.
                -1, // The number of playback loops. -1 means an infinite loop.
                1, // pitch The pitch of the audio effect. The value ranges between 0.5 and 2. The default value is 1 (no change to the pitch). The lower the value, the lower the pitch.
                0.0, // Sets the spatial position of the effect. 0 means the effect shows ahead.
                100, // Sets the volume. The value ranges between 0 and 100. 100 is the original volume.
                true // Sets whether to publish the audio effect.
            );
            Log.i(TAG, "result playRet:" + playRet);
        } else if (v == effectResume) {
            int ret = engine.resumeEffect(EFFECT_SOUND_ID);
            Log.i(TAG, "resumeEffect >> ret=" + ret);
        } else if (v == effectPause) {
            int ret = engine.pauseEffect(EFFECT_SOUND_ID);
            Log.i(TAG, "resumeEffect >> ret=" + ret);
        } else if (v == effectStop) {
            int ret = engine.stopEffect(EFFECT_SOUND_ID);
            Log.i(TAG, "resumeEffect >> ret=" + ret);
        }
    }

    /**
     * @param channelId Specify the channel name that you want to join.
     *                  Users that input the same channel name join the same channel.
     */
    private void joinChannel(String channelId) {
        /*In the demo, the default is to enter as the anchor.*/
        engine.setClientRole(Constants.CLIENT_ROLE_BROADCASTER);
        engine.setAudioProfile(
                Constants.AudioProfile.getValue(Constants.AudioProfile.valueOf(audioProfile.getSelectedItem().toString())),
                Constants.AudioScenario.getValue(Constants.AudioScenario.valueOf(audioScenario.getSelectedItem().toString()))
        );

        /*Please configure accessToken in the string_config file.
         * A temporary token generated in Console. A temporary token is valid for 24 hours. For details, see
         *      https://docs.agora.io/en/Agora%20Platform/token?platform=All%20Platforms#get-a-temporary-token
         * A token generated at the server. This applies to scenarios with high-security requirements. For details, see
         *      https://docs.agora.io/en/cloud-recording/token_server_java?platform=Java*/
        TokenUtils.gen(requireContext(), channelId, 0, ret -> {
            /* Allows a user to join a channel.
             if you do not specify the uid, we will generate the uid for you*/
            ChannelMediaOptions option = new ChannelMediaOptions();
            option.autoSubscribeAudio = true;
            option.autoSubscribeVideo = true;
            int res = engine.joinChannel(ret, channelId, 0, option);
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
                    resetLayoutByJoin();
                    audioSeatManager.upLocalSeat(uid);
                }
            });
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
        public void onAudioMixingStateChanged(int state, int errorCode) {
            showLongToast(String.format("onAudioMixingStateChanged %d error code:%d", state, errorCode));
        }

        @Override
        public void onAudioMixingFinished() {
            super.onAudioMixingFinished();
        }
    };

    @Override
    public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
        if (seekBar.getId() == R.id.mixingPublishVolBar) {
            /*
             * Adjusts the volume of audio mixing for publishing (sending to other users).
             * @param volume: Audio mixing volume for publishing. The value ranges between 0 and 100 (default).
             */
            engine.adjustAudioMixingPublishVolume(progress);
        } else if (seekBar.getId() == R.id.mixingPlayoutVolBar) {
            /*
             * Adjusts the volume of audio mixing for local playback.
             * @param volume: Audio mixing volume for local playback. The value ranges between 0 and 100 (default).
             */
            engine.adjustAudioMixingPlayoutVolume(progress);
        } else if (seekBar.getId() == R.id.mixingVolBar) {
            /*
             * Adjusts the volume of audio mixing.
             * Call this method when you are in a channel.
             * @param volume: Audio mixing volume. The value ranges between 0 and 100 (default).
             */
            engine.adjustAudioMixingVolume(progress);
        } else if (seekBar.getId() == R.id.effectVolBar) {
            engine.setEffectsVolume(progress);
        }
    }

    @Override
    public void onStartTrackingTouch(SeekBar seekBar) {

    }

    @Override
    public void onStopTrackingTouch(SeekBar seekBar) {

    }
}
