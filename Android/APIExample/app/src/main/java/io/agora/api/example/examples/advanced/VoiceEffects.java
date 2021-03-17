package io.agora.api.example.examples.advanced;

import android.content.Context;
import android.graphics.drawable.ColorDrawable;
import android.os.Bundle;
import android.os.Handler;
import android.text.TextUtils;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.PopupWindow;
import android.widget.SeekBar;
import android.widget.Spinner;
import android.widget.Switch;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import com.yanzhenjie.permission.AndPermission;
import com.yanzhenjie.permission.runtime.Permission;

import io.agora.api.example.R;
import io.agora.api.example.annotation.Example;
import io.agora.api.example.common.BaseFragment;
import io.agora.api.example.utils.CommonUtil;
import io.agora.rtc.Constants;
import io.agora.rtc.IRtcEngineEventHandler;
import io.agora.rtc.RtcEngine;
import io.agora.rtc.models.ChannelMediaOptions;

import static io.agora.api.example.common.model.Examples.ADVANCED;
import static io.agora.rtc.Constants.*;

@Example(
        index = 15,
        group = ADVANCED,
        name = R.string.item_voiceeffects,
        actionId = R.id.action_mainFragment_to_VoiceEffects,
        tipsId = R.string.voiceeffects
)
public class VoiceEffects extends BaseFragment implements View.OnClickListener, AdapterView.OnItemSelectedListener, CompoundButton.OnCheckedChangeListener {
    private static final String TAG = VoiceEffects.class.getSimpleName();
    private EditText et_channel;
    private Button join, effectOptions, ok;
    private RtcEngine engine;
    private int myUid;
    private boolean joined = false;
    private Spinner preset, beautifier, pitch1, pitch2, conversion;
    private PopupWindow popupWindow;
    private Switch effectOption;
    private SeekBar voiceCircle;

    @Override
    public void onCreate(@Nullable Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        handler = new Handler();
    }

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState)
    {
        View view = inflater.inflate(R.layout.fragment_voice_effects, container, false);
        return view;
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState)
    {
        super.onViewCreated(view, savedInstanceState);
        join = view.findViewById(R.id.btn_join);
        et_channel = view.findViewById(R.id.et_channel);
        view.findViewById(R.id.btn_join).setOnClickListener(this);
        preset = view.findViewById(R.id.audio_preset_spinner);
        beautifier = view.findViewById(R.id.voice_beautifier_spinner);
        conversion = view.findViewById(R.id.voice_conversion_spinner);
        preset.setOnItemSelectedListener(this);
        beautifier.setOnItemSelectedListener(this);
        conversion.setOnItemSelectedListener(this);
        effectOptions = view.findViewById(R.id.btn_effect_options);
        effectOptions.setOnClickListener(this);
        LayoutInflater inflater = (LayoutInflater) getContext().getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        View vPopupWindow = inflater.inflate(R.layout.popup_effect_options, null, false);
        popupWindow = new PopupWindow(vPopupWindow,
                ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.WRAP_CONTENT, true);
        popupWindow.setBackgroundDrawable(new ColorDrawable(0xefefefef));
        ok = vPopupWindow.findViewById(R.id.btn_ok);
        ok.setOnClickListener(this);
        pitch1 = vPopupWindow.findViewById(R.id.pitch_option1);
        pitch2 = vPopupWindow.findViewById(R.id.pitch_option2);
        effectOption = vPopupWindow.findViewById(R.id.switch_effect_option);
        effectOption.setOnCheckedChangeListener(this);
        voiceCircle = vPopupWindow.findViewById(R.id.room_acoustics_3d_voice);
        toggleEffectOptionsDisplay(false);
        effectOptions.setEnabled(false);
        preset.setEnabled(false);
        beautifier.setEnabled(false);
        conversion.setEnabled(false);
    }

    @Override
    public void onActivityCreated(@Nullable Bundle savedInstanceState)
    {
        super.onActivityCreated(savedInstanceState);
        // Check if the context is valid
        Context context = getContext();
        if (context == null)
        {
            return;
        }
        try
        {
            /**Creates an RtcEngine instance.
             * @param context The context of Android Activity
             * @param appId The App ID issued to you by Agora. See <a href="https://docs.agora.io/en/Agora%20Platform/token#get-an-app-id">
             *              How to get the App ID</a>
             * @param handler IRtcEngineEventHandler is an abstract class providing default implementation.
             *                The SDK uses this class to report to the app on SDK runtime events.*/
            String appId = getString(R.string.agora_app_id);
            engine = RtcEngine.create(getContext().getApplicationContext(), appId, iRtcEngineEventHandler);
        }
        catch (Exception e)
        {
            e.printStackTrace();
            getActivity().onBackPressed();
        }
    }

    @Override
    public void onDestroy()
    {
        super.onDestroy();
        /**leaveChannel and Destroy the RtcEngine instance*/
        if(engine != null)
        {
            engine.leaveChannel();
        }
        handler.post(RtcEngine::destroy);
        engine = null;
    }

    @Override
    public void onClick(View v)
    {
        if (v.getId() == R.id.btn_join)
        {
            if (!joined)
            {
                CommonUtil.hideInputBoard(getActivity(), et_channel);
                // call when join button hit
                String channelId = et_channel.getText().toString();
                // Check permission
                if (AndPermission.hasPermissions(this, Permission.Group.STORAGE, Permission.Group.MICROPHONE, Permission.Group.CAMERA))
                {
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
            }
            else
            {
                joined = false;
                preset.setEnabled(false);
                beautifier.setEnabled(false);
                conversion.setEnabled(false);
                effectOptions.setEnabled(false);
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
            }
        }
        else if(v.getId() == R.id.btn_effect_options){
            popupWindow.showAsDropDown(v, 50, 0);
        }
        else if(v.getId() == R.id.btn_ok){
            boolean isPitch = effectOption.isChecked();
            if(isPitch){
                int effectOption1 = getPitch1Value(pitch1.getSelectedItem().toString());
                int effectOption2 = getPitch2Value(pitch2.getSelectedItem().toString());
                engine.setAudioEffectParameters(PITCH_CORRECTION, effectOption1, effectOption2);
            }
            else{
                int voiceCircleOption = voiceCircle.getProgress();
                engine.setAudioEffectParameters(ROOM_ACOUSTICS_3D_VOICE, voiceCircleOption, 0);
            }
            popupWindow.dismiss();
        }
    }

    private int getPitch1Value(String str) {
        switch (str){
            case "Natural Minor":
                return 2;
            case "Breeze Minor":
                return 3;
            default:
                return 1;
        }
    }

    private int getPitch2Value(String str) {
        switch (str){
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
     *                  Users that input the same channel name join the same channel.*/
    private void joinChannel(String channelId)
    {
        /** Sets the channel profile of the Agora RtcEngine.
         CHANNEL_PROFILE_COMMUNICATION(0): (Default) The Communication profile.
         Use this profile in one-on-one calls or group calls, where all users can talk freely.
         CHANNEL_PROFILE_LIVE_BROADCASTING(1): The Live-Broadcast profile. Users in a live-broadcast
         channel have a role as either broadcaster or audience. A broadcaster can both send and receive streams;
         an audience can only receive streams.*/
        engine.setChannelProfile(Constants.CHANNEL_PROFILE_LIVE_BROADCASTING);
        /**In the demo, the default is to enter as the anchor.*/
        engine.setClientRole(IRtcEngineEventHandler.ClientRole.CLIENT_ROLE_BROADCASTER);
        /**Please configure accessToken in the string_config file.
         * A temporary token generated in Console. A temporary token is valid for 24 hours. For details, see
         *      https://docs.agora.io/en/Agora%20Platform/token?platform=All%20Platforms#get-a-temporary-token
         * A token generated at the server. This applies to scenarios with high-security requirements. For details, see
         *      https://docs.agora.io/en/cloud-recording/token_server_java?platform=Java*/
        String accessToken = getString(R.string.agora_access_token);
        if (TextUtils.equals(accessToken, "") || TextUtils.equals(accessToken, "<#YOUR ACCESS TOKEN#>"))
        {
            accessToken = null;
        }

        engine.setAudioProfile(AUDIO_PROFILE_MUSIC_HIGH_QUALITY_STEREO, AUDIO_SCENARIO_GAME_STREAMING);

        /** Allows a user to join a channel.
         if you do not specify the uid, we will generate the uid for you*/

        ChannelMediaOptions option = new ChannelMediaOptions();
        option.autoSubscribeAudio = true;
        option.autoSubscribeVideo = true;
        int res = engine.joinChannel(accessToken, channelId, "Extra Optional Data", 0, option);
        if (res != 0)
        {
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
    }

    /**IRtcEngineEventHandler is an abstract class providing default implementation.
     * The SDK uses this class to report to the app on SDK runtime events.*/
    private final IRtcEngineEventHandler iRtcEngineEventHandler = new IRtcEngineEventHandler()
    {
        /**Reports a warning during SDK runtime.
         * Warning code: https://docs.agora.io/en/Voice/API%20Reference/java/classio_1_1agora_1_1rtc_1_1_i_rtc_engine_event_handler_1_1_warn_code.html*/
        @Override
        public void onWarning(int warn)
        {
            Log.w(TAG, String.format("onWarning code %d message %s", warn, RtcEngine.getErrorDescription(warn)));
        }

        /**Reports an error during SDK runtime.
         * Error code: https://docs.agora.io/en/Voice/API%20Reference/java/classio_1_1agora_1_1rtc_1_1_i_rtc_engine_event_handler_1_1_error_code.html*/
        @Override
        public void onError(int err)
        {
            Log.e(TAG, String.format("onError code %d message %s", err, RtcEngine.getErrorDescription(err)));
            showAlert(String.format("onError code %d message %s", err, RtcEngine.getErrorDescription(err)));
        }

        /**Occurs when a user leaves the channel.
         * @param stats With this callback, the application retrieves the channel information,
         *              such as the call duration and statistics.*/
        @Override
        public void onLeaveChannel(RtcStats stats)
        {
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
        public void onJoinChannelSuccess(String channel, int uid, int elapsed)
        {
            Log.i(TAG, String.format("onJoinChannelSuccess channel %s uid %d", channel, uid));
            showLongToast(String.format("onJoinChannelSuccess channel %s uid %d", channel, uid));
            myUid = uid;
            joined = true;
            handler.post(() -> {
                join.setEnabled(true);
                join.setText(getString(R.string.leave));
                conversion.setEnabled(true);
                preset.setEnabled(true);
                beautifier.setEnabled(true);
                effectOptions.setEnabled(true);
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
        public void onRemoteAudioStateChanged(int uid, int state, int reason, int elapsed)
        {
            super.onRemoteAudioStateChanged(uid, state, reason, elapsed);
            Log.i(TAG, "onRemoteAudioStateChanged->" + uid + ", state->" + state + ", reason->" + reason);
        }

        /**Occurs when a remote user (Communication)/host (Live Broadcast) joins the channel.
         * @param uid ID of the user whose audio state changes.
         * @param elapsed Time delay (ms) from the local user calling joinChannel/setClientRole
         *                until this callback is triggered.*/
        @Override
        public void onUserJoined(int uid, int elapsed)
        {
            super.onUserJoined(uid, elapsed);
            Log.i(TAG, "onUserJoined->" + uid);
            showLongToast(String.format("user %d joined!", uid));
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
        public void onUserOffline(int uid, int reason)
        {
            Log.i(TAG, String.format("user %d offline! reason:%d", uid, reason));
            showLongToast(String.format("user %d offline! reason:%d", uid, reason));
        }
    };

    @Override
    public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
        if(parent.getId() == R.id.audio_preset_spinner){
            String item = preset.getSelectedItem().toString();
            engine.setAudioEffectPreset(getAudioEffectPreset(item));
        }
        else if(parent.getId() == R.id.voice_beautifier_spinner){
            String item = beautifier.getSelectedItem().toString();
            engine.setVoiceBeautifierPreset(getVoiceBeautifierValue(item));
        }
        else if(parent.getId() == R.id.voice_conversion_spinner){
            String item = conversion.getSelectedItem().toString();
            engine.setVoiceConversionPreset(getVoiceConversionValue(item));
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
            default:
                value = VOICE_BEAUTIFIER_OFF;
        }
        return value;
    }

    private int getAudioEffectPreset(String label){
        int value;
        switch (label){
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
        toggleEffectOptionsDisplay(isChecked);
    }

    private void toggleEffectOptionsDisplay(boolean isChecked){
        pitch1.setVisibility(isChecked?View.VISIBLE:View.GONE);
        pitch2.setVisibility(isChecked?View.VISIBLE:View.GONE);
        voiceCircle.setVisibility(isChecked?View.GONE:View.VISIBLE);
    }
}
