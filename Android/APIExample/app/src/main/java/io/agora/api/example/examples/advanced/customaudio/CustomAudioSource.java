package io.agora.api.example.examples.advanced.customaudio;

import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.media.AudioFormat;
import android.media.AudioManager;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.text.TextUtils;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.EditText;
import android.widget.RadioGroup;
import android.widget.SeekBar;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import com.yanzhenjie.permission.AndPermission;
import com.yanzhenjie.permission.runtime.Permission;

import io.agora.api.example.R;
import io.agora.api.example.annotation.Example;
import io.agora.api.example.common.BaseFragment;
import io.agora.api.example.utils.CommonUtil;
import io.agora.rtc.Constants;
import io.agora.rtc.Constants.AudioExternalSourcePos;
import io.agora.rtc.IRtcEngineEventHandler;
import io.agora.rtc.RtcEngine;
import io.agora.rtc.models.ChannelMediaOptions;

import static io.agora.api.example.common.model.Examples.ADVANCED;
import static io.agora.api.example.examples.advanced.customaudio.AudioRecordService.RecordThread.DEFAULT_CHANNEL_COUNT;
import static io.agora.api.example.examples.advanced.customaudio.AudioRecordService.RecordThread.DEFAULT_SAMPLE_RATE;

import java.util.Locale;

/**
 * This demo demonstrates how to make a one-to-one voice call
 */
@Example(
        index = 8,
        group = ADVANCED,
        name = R.string.item_customaudiosource,
        actionId = R.id.action_mainFragment_to_CustomAudioSource,
        tipsId = R.string.customaudio
)
public class CustomAudioSource extends BaseFragment implements View.OnClickListener, SeekBar.OnSeekBarChangeListener {
    private static final String TAG = CustomAudioSource.class.getSimpleName();
    private EditText et_channel;
    private Button mute, join;
    private boolean joined = false;
    public RtcEngine engine;
    private static final Integer SAMPLE_RATE = 44100;
    private static final Integer SAMPLE_NUM_OF_CHANNEL = 2;
    private final int bufferSize = 88200;
    private final byte[] data = new byte[bufferSize];

    private RadioGroup groupAudioSource;
    private SeekBar localSlider;
    private SeekBar preProcessSlider;
    private SeekBar postProcessSlider;

    private AudioRecordService recordService = null;
    private boolean isServiceBind = false;
    private final ServiceConnection recordConnection = new ServiceConnection() {
        @Override
        public void onServiceConnected(ComponentName name, IBinder service) {
            recordService = ((AudioRecordService.RecordBinder) service).getService();
            changeAudioSourcePos(groupAudioSource,groupAudioSource.getCheckedRadioButtonId());
            recordService.engine = engine;
            recordService.startRecording();
            isServiceBind = true;
        }

        @Override
        public void onServiceDisconnected(ComponentName name) {
            isServiceBind = false;
        }
    };

    @Override
    public void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        handler = new Handler();
    }

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        return inflater.inflate(R.layout.fragment_custom_audiorecord, container, false);
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        join = view.findViewById(R.id.btn_join);
        et_channel = view.findViewById(R.id.et_channel);
        view.findViewById(R.id.btn_join).setOnClickListener(this);
        mute = view.findViewById(R.id.btn_mute);
        mute.setOnClickListener(this);

        localSlider = view.findViewById(R.id.volume_local_slider_fg_custom_audio);
        preProcessSlider = view.findViewById(R.id.volume_pre_process_slider_fg_custom_audio);
        postProcessSlider = view.findViewById(R.id.volume_post_process_slider_fg_custom_audio);

        groupAudioSource = view.findViewById(R.id.group_source_local_fg_custom_audio);

        groupAudioSource.setOnCheckedChangeListener(this::changeAudioSourcePos);
        localSlider.setOnSeekBarChangeListener(this);
        preProcessSlider.setOnSeekBarChangeListener(this);
        postProcessSlider.setOnSeekBarChangeListener(this);
    }

    private void changeAudioSourcePos(RadioGroup group, int checkedId) {
        AudioExternalSourcePos desiredPos;
        switch (group.indexOfChild(group.findViewById(checkedId))) {
            case 0:
                desiredPos = AudioExternalSourcePos.AUDIO_EXTERNAL_PLAYOUT_SOURCE;
                break;
            case 1:
                desiredPos = AudioExternalSourcePos.AUDIO_EXTERNAL_RECORD_SOURCE_PRE_PROCESS;
                break;
            default:
                desiredPos = AudioExternalSourcePos.AUDIO_EXTERNAL_RECORD_SOURCE_POST_PROCESS;
                break;
        }
        if (recordService != null)
            recordService.currentSourcePos = getValue(desiredPos);
    }

    private int getValue(AudioExternalSourcePos sourcePos) {
        return AudioExternalSourcePos.getValue(sourcePos);
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
            /**Creates an RtcEngine instance.
             * @param context The context of Android Activity
             * @param appId The App ID issued to you by Agora. See <a href="https://docs.agora.io/en/Agora%20Platform/token#get-an-app-id">
             *              How to get the App ID</a>
             * @param handler IRtcEngineEventHandler is an abstract class providing default implementation.
             *                The SDK uses this class to report to the app on SDK runtime events.*/
            engine = RtcEngine.create(getContext().getApplicationContext(), getString(R.string.agora_app_id),
                    iRtcEngineEventHandler);

        } catch (Exception e) {
            e.printStackTrace();
            requireActivity().onBackPressed();
        }
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        stopAudioRecord();
        /**leaveChannel and Destroy the RtcEngine instance*/
        if (engine != null) {
            engine.leaveChannel();
        }
        handler.postAtFrontOfQueue(RtcEngine::destroy);
        engine = null;
    }

    @Override
    public void onClick(View v) {
        if (v.getId() == R.id.btn_join) {
            if (!joined) {
                CommonUtil.hideInputBoard(requireActivity(), et_channel);
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
                ).onGranted(permissions ->
                {
                    // Permissions Granted
                    joinChannel(channelId);
                }).start();
            } else {
                joined = false;
                stopAudioRecord();
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
                join.setText(getString(R.string.join));
                mute.setText(getString(R.string.closemicrophone));
                mute.setEnabled(false);
            }
        } else if (v.getId() == R.id.btn_mute) {
            mute.setActivated(!mute.isActivated());
            mute.setText(getString(mute.isActivated() ? R.string.openmicrophone : R.string.closemicrophone));
            /**Turn off / on the microphone, stop / start local audio collection and push streaming.*/
            engine.muteLocalAudioStream(mute.isActivated());
        }
    }

    /**
     * @param channelId Specify the channel name that you want to join.
     *                  Users that input the same channel name join the same channel.
     */
    private void joinChannel(String channelId) {
        /** Sets the channel profile of the Agora RtcEngine.
         CHANNEL_PROFILE_COMMUNICATION(0): (Default) The Communication profile.
         Use this profile in one-on-one calls or group calls, where all users can talk freely.
         CHANNEL_PROFILE_LIVE_BROADCASTING(1): The Live-Broadcast profile. Users in a live-broadcast
         channel have a role as either broadcaster or audience. A broadcaster can both send and receive streams;
         an audience can only receive streams.*/
        engine.setChannelProfile(Constants.CHANNEL_PROFILE_LIVE_BROADCASTING);
        /**In the demo, the default is to enter as the anchor.*/
        engine.setClientRole(IRtcEngineEventHandler.ClientRole.CLIENT_ROLE_BROADCASTER);
        /**Sets the external audio source.
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
        engine.setExternalAudioSource(true, DEFAULT_SAMPLE_RATE, DEFAULT_CHANNEL_COUNT);
        /**Please configure accessToken in the string_config file.
         * A temporary token generated in Console. A temporary token is valid for 24 hours. For details, see
         *      https://docs.agora.io/en/Agora%20Platform/token?platform=All%20Platforms#get-a-temporary-token
         * A token generated at the server. This applies to scenarios with high-security requirements. For details, see
         *      https://docs.agora.io/en/cloud-recording/token_server_java?platform=Java*/
        String accessToken = getString(R.string.agora_access_token);
        if (TextUtils.equals(accessToken, "") || TextUtils.equals(accessToken, "<#YOUR ACCESS TOKEN#>")) {
            accessToken = null;
        }
        /** Allows a user to join a channel.
         if you do not specify the uid, we will generate the uid for you*/

        ChannelMediaOptions option = new ChannelMediaOptions();
        option.autoSubscribeAudio = true;
        option.autoSubscribeVideo = false;
        int res = engine.joinChannel(accessToken, channelId, "Extra Optional Data", 0, option);
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

    private void startAudioRecord() {
        Intent intent = new Intent(getContext(), AudioRecordService.class);
        requireActivity().bindService(intent, recordConnection, Context.BIND_AUTO_CREATE);
    }

    private void stopAudioRecord() {
        if (isServiceBind) {
            isServiceBind = false;
            requireActivity().unbindService(recordConnection);
        }
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

        /**Reports an error during SDK runtime.
         * Error code: https://docs.agora.io/en/Voice/API%20Reference/java/classio_1_1agora_1_1rtc_1_1_i_rtc_engine_event_handler_1_1_error_code.html*/
        @Override
        public void onError(int err) {
            String msg = String.format(Locale.getDefault(), "onError code %d message %s", err, RtcEngine.getErrorDescription(err));
            Log.e(TAG, msg);
            showAlert(msg);
        }

        /**Occurs when the local user joins a specified channel.
         * The channel name assignment is based on channelName specified in the joinChannel method.
         * If the uid is not specified when joinChannel is called, the server automatically assigns a uid.
         * @param channel Channel name
         * @param uid User ID
         * @param elapsed Time elapsed (ms) from the user calling joinChannel until this callback is triggered*/
        @Override
        public void onJoinChannelSuccess(String channel, int uid, int elapsed) {
            String msg = String.format(Locale.getDefault(), "onJoinChannelSuccess channel %s uid %d", channel, uid);
            Log.i(TAG, msg);
            showLongToast(msg);
            joined = true;
            handler.post(() -> {
                mute.setEnabled(true);
                join.setEnabled(true);
                join.setText(getString(R.string.leave));
                localSlider.setProgress(localSlider.getMax());
                preProcessSlider.setProgress(preProcessSlider.getMax());
                postProcessSlider.setProgress(postProcessSlider.getMax());
            });
            startAudioRecord();
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
    };

    @Override
    public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {

    }

    @Override
    public void onStartTrackingTouch(SeekBar seekBar) {

    }

    @Override
    public void onStopTrackingTouch(SeekBar seekBar) {
        int value;
        if (seekBar == localSlider)
            value = getValue(AudioExternalSourcePos.AUDIO_EXTERNAL_PLAYOUT_SOURCE);
        else if (seekBar == preProcessSlider)
            value = getValue(AudioExternalSourcePos.AUDIO_EXTERNAL_RECORD_SOURCE_PRE_PROCESS);
        else
            value = getValue(AudioExternalSourcePos.AUDIO_EXTERNAL_RECORD_SOURCE_POST_PROCESS);
        engine.setExternalAudioSourceVolume(value, seekBar.getProgress());
    }
}
