package io.agora.api.example.examples.advanced;

import static io.agora.api.example.common.model.Examples.ADVANCED;

import android.content.Context;
import android.os.Bundle;
import android.os.Handler;
import android.text.TextUtils;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.EditText;
import android.widget.SeekBar;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.widget.AppCompatTextView;

import com.yanzhenjie.permission.AndPermission;
import com.yanzhenjie.permission.runtime.Permission;

import io.agora.api.component.Constant;
import io.agora.api.example.R;
import io.agora.api.example.annotation.Example;
import io.agora.api.example.common.BaseFragment;
import io.agora.api.example.utils.CommonUtil;
import io.agora.rtc.Constants;
import io.agora.rtc.IAudioEffectManager;
import io.agora.rtc.IRtcEngineEventHandler;
import io.agora.rtc.RtcEngine;
import io.agora.rtc.models.ChannelMediaOptions;

@Example(
        index = 14,
        group = ADVANCED,
        name = R.string.item_playaudiofiles,
        actionId = R.id.action_mainFragment_to_PlayAudioFiles,
        tipsId = R.string.playaudiofiles
)
public class PlayAudioFiles extends BaseFragment implements View.OnClickListener, SeekBar.OnSeekBarChangeListener {
    private static final String TAG = PlayAudioFiles.class.getSimpleName();
    private EditText et_channel;
    private AppCompatTextView progressText;
    private Button join, bgm_start, bgm_resume, bgm_pause, bgm_stop, effect;
    private SeekBar mixingPublishVolBar, mixingPlayoutVolBar, effectVolBar, mixingVolBar, mixingProgressBar, sliderSpeed;
    private TextView titleSpeed;
    private TextView titleTrack;
    private RtcEngine engine;
    private int myUid;
    private boolean joined = false;
    private IAudioEffectManager audioEffectManager;

    private String currentMusic = Constant.URL_PLAY_AUDIO_FILES;
    private int currentTrackIndex;


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
        return inflater.inflate(R.layout.fragment_play_audio_files, container, false);
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState)
    {
        super.onViewCreated(view, savedInstanceState);
        join = view.findViewById(R.id.btn_join);
        et_channel = view.findViewById(R.id.et_channel);
        progressText = view.findViewById(R.id.mixingProgressLabel);
        view.findViewById(R.id.btn_join).setOnClickListener(this);
        bgm_start = view.findViewById(R.id.bgmStart);
        bgm_start.setOnClickListener(this);
        bgm_resume = view.findViewById(R.id.bgmResume);
        bgm_resume.setOnClickListener(this);
        bgm_pause = view.findViewById(R.id.bgmPause);
        bgm_pause.setOnClickListener(this);
        bgm_stop = view.findViewById(R.id.bgmStop);
        bgm_stop.setOnClickListener(this);
        effect = view.findViewById(R.id.btn_effect);
        sliderSpeed = view.findViewById(R.id.slider_speed_fg_audio_file);
        titleSpeed = view.findViewById(R.id.title_speed_fg_audio_file);
        titleTrack = view.findViewById(R.id.title_track_fg_audio_file);
        effect.setOnClickListener(this);
        mixingPublishVolBar = view.findViewById(R.id.mixingPublishVolBar);
        mixingPlayoutVolBar = view.findViewById(R.id.mixingPlayoutVolBar);
        mixingVolBar = view.findViewById(R.id.mixingVolBar);
        effectVolBar = view.findViewById(R.id.effectVolBar);
        mixingProgressBar = view.findViewById(R.id.mixingProgress);
        mixingProgressBar.setOnSeekBarChangeListener(this);
        mixingPlayoutVolBar.setOnSeekBarChangeListener(this);
        mixingPublishVolBar.setOnSeekBarChangeListener(this);
        mixingVolBar.setOnSeekBarChangeListener(this);
        effectVolBar.setOnSeekBarChangeListener(this);
        sliderSpeed.setOnSeekBarChangeListener(this);

        view.findViewById(R.id.btn_to_last_track_fg_audio_file).setOnClickListener(this);
        view.findViewById(R.id.btn_to_next_track_fg_audio_file).setOnClickListener(this);

        view.findViewById(R.id.btn_channel_left_fg_audio_file).setOnClickListener(this);
        view.findViewById(R.id.btn_channel_right_fg_audio_file).setOnClickListener(this);
        view.findViewById(R.id.btn_channel_stereo_fg_audio_file).setOnClickListener(this);
        view.findViewById(R.id.btn_channel_mix_fg_audio_file).setOnClickListener(this);

        updateSpeedTitle();
        updateTrackTitle();
    }

    /**
     * 声道切换
     */
    private void updateChannel(int index){
        int value = Constants.AudioMixingDualMonoMode.getValue(Constants.AudioMixingDualMonoMode.values()[index]);
        engine.setAudioMixingDualMonoMode(value);
    }

    private void updateSpeedTitle(){
        titleSpeed.setText(getString(R.string.play_speed,(sliderSpeed.getProgress()+50)/100f));
    }
    private void updateTrackTitle(){
        int count = 0;
        if(engine!=null) count = engine.getAudioTrackCount();
        String desc = getString(R.string.track_change,count);
        titleTrack.setText(desc);
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

    /**
     * To ensure smooth communication, limit the size of the audio effect file.
     * We recommend using this method to preload the audio effect before calling the joinChannel method.
     */
    private void preloadAudioEffect(){
        // Gets the global audio effect manager.
        audioEffectManager = engine.getAudioEffectManager();
        // Preloads the audio effect (recommended). Note the file size, and preload the file before joining the channel.
        // Only mp3, aac, m4a, 3gp, and wav files are supported.
        // You may need to record the sound IDs and their file paths.
        int id = 0;
        audioEffectManager.preloadEffect(0, Constant.EFFECT_FILE_PATH);
        /** Plays an audio effect file.
         * Returns
         * 0: Success.
         * < 0: Failure.
         */
        audioEffectManager.playEffect(
                0,  // The sound ID of the audio effect file to be played.
                Constant.EFFECT_FILE_PATH,  // The file path of the audio effect file.
                -1,   // The number of playback loops. -1 means an infinite loop.
                1,    // pitch	The pitch of the audio effect. The value ranges between 0.5 and 2. The default value is 1 (no change to the pitch). The lower the value, the lower the pitch.
                0.0,  // Sets the spatial position of the effect. 0 means the effect shows ahead.
                100,  // Sets the volume. The value ranges between 0 and 100. 100 is the original volume.
                true, // Sets whether to publish the audio effect.
                0 // Start position
        );
        // Pauses all audio effects.
        audioEffectManager.pauseAllEffects();
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
                et_channel.clearFocus();
                CommonUtil.hideInputBoard(requireActivity(), et_channel);
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
                bgm_start.setEnabled(false);
                bgm_pause.setEnabled(false);
                bgm_resume.setEnabled(false);
                bgm_stop.setEnabled(false);
                effect.setEnabled(false);
                effect.setText(getString(R.string.effect_on));
            }
        }
        else if(v.getId() == R.id.bgmStart)
        {
            sliderSpeed.setProgress(50);
            engine.startAudioMixing(currentMusic, false, false, -1, 0);
            engine.getAudioFileInfo(currentMusic);
            startProgressTimer();
        }
        else if(v.getId() == R.id.bgmStop){
            engine.stopAudioMixing();
            stopProgressTimer();
        }
        else if(v.getId() == R.id.bgmResume){
            engine.resumeAudioMixing();
        }
        else if(v.getId() == R.id.bgmPause){
            engine.pauseAudioMixing();
        }
        else if (v.getId() == R.id.btn_effect)
        {
            effect.setActivated(!effect.isActivated());
            effect.setText(!effect.isActivated() ? getString(R.string.effect_on): getString(R.string.effect_off));
            if(effect.isActivated()){
                // Resumes playing all audio effects.
                audioEffectManager.resumeAllEffects();
            }
            else {
                // Pauses all audio effects.
                audioEffectManager.pauseAllEffects();
            }
        }
        else{
            switch (v.getId()){
                case R.id.btn_to_last_track_fg_audio_file:{
                    updateTrackTitle();
                    if(currentTrackIndex==0)
                        currentTrackIndex = engine.getAudioTrackCount() - 1;
                    else currentTrackIndex--;
                    engine.selectAudioTrack(currentTrackIndex);
                    break;
                }
                case R.id.btn_to_next_track_fg_audio_file:{
                    updateTrackTitle();
                    currentTrackIndex = (++currentTrackIndex) % engine.getAudioTrackCount();
                    engine.selectAudioTrack(currentTrackIndex);
                    break;
                }

                case R.id.btn_channel_left_fg_audio_file:updateChannel(1);
                break;
                case R.id.btn_channel_right_fg_audio_file:updateChannel(2);
                break;
                case R.id.btn_channel_stereo_fg_audio_file:updateChannel(0);
                break;
                case R.id.btn_channel_mix_fg_audio_file:updateChannel(3);
                break;
            }
        }
    }

    private void stopProgressTimer() {
        mixingProgressBar.setProgress(0);
        handler.removeCallbacksAndMessages(null);
    }

    private void startProgressTimer() {
        if(engine != null) {
            int currentPosition = engine.getAudioMixingCurrentPosition();
            if (mixingProgressBar.getMax() != 0 && !mixingProgressBar.isPressed())
                mixingProgressBar.setProgress(currentPosition);
            handler.postDelayed(this::startProgressTimer, 1000);
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
            preloadAudioEffect();
            showLongToast(String.format("onJoinChannelSuccess channel %s uid %d", channel, uid));
            myUid = uid;
            joined = true;
            handler.post(new Runnable()
            {
                @Override
                public void run()
                {
                    join.setEnabled(true);
                    join.setText(getString(R.string.leave));
                    bgm_start.setEnabled(true);
                    bgm_resume.setEnabled(true);
                    bgm_pause.setEnabled(true);
                    bgm_stop.setEnabled(true);
                    effect.setEnabled(true);
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

        @Override
        public void onRequestAudioFileInfo(AudioFileInfo info, int error) {
            Log.d(TAG, "onRequestAudioFileInfo: "+info.durationMs);
            handler.post(()-> mixingProgressBar.setMax(info.durationMs));
        }
    };

    @Override
    public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
        if(seekBar.getId() == R.id.mixingPublishVolBar){
            /**
             * Adjusts the volume of audio mixing for publishing (sending to other users).
             * @param volume: Audio mixing volume for publishing. The value ranges between 0 and 100 (default).
             */
            engine.adjustAudioMixingPublishVolume(progress);
        }
        else if(seekBar.getId() == R.id.mixingPlayoutVolBar){
            /**
             * Adjusts the volume of audio mixing for local playback.
             * @param volume: Audio mixing volume for local playback. The value ranges between 0 and 100 (default).
             */
            engine.adjustAudioMixingPlayoutVolume(progress);
        }
        else if(seekBar.getId() == R.id.mixingVolBar){
            /**
             * Adjusts the volume of audio mixing.
             * Call this method when you are in a channel.
             * @param volume: Audio mixing volume. The value ranges between 0 and 100 (default).
             */
            engine.adjustAudioMixingVolume(progress);
        }
        else if(seekBar.getId() == R.id.effectVolBar){
            audioEffectManager.setVolumeOfEffect(0, progress);
        }else if(seekBar.getId() == R.id.mixingProgress){
            String durationText = io.agora.api.example.utils.TextUtils.durationFormat((long) progress);
            progressText.setText(durationText);
        }else if(seekBar.getId() == R.id.slider_speed_fg_audio_file){
           updateSpeedTitle();
        }

    }

    @Override
    public void onStartTrackingTouch(SeekBar seekBar) {
        // no need to update when user changes
        if(seekBar.getId() == R.id.mixingProgress)
            handler.removeCallbacksAndMessages(null);
    }

    @Override
    public void onStopTrackingTouch(SeekBar seekBar) {
        if(seekBar.getId() == R.id.mixingProgress) {
            engine.setAudioMixingPosition(seekBar.getProgress());
            handler.postDelayed(this::startProgressTimer, 1000);
        } else if(seekBar.getId() == R.id.slider_speed_fg_audio_file){
            if(sliderSpeed.isPressed())
                engine.setAudioMixingPlaybackSpeed(seekBar.getProgress()+50);
        }
    }
}
