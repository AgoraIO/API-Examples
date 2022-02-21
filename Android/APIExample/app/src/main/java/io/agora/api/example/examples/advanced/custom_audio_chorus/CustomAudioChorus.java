package io.agora.api.example.examples.advanced.custom_audio_chorus;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.text.TextUtils;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.EditText;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import com.yanzhenjie.permission.AndPermission;
import com.yanzhenjie.permission.runtime.Permission;

import java.nio.ByteBuffer;

import io.agora.api.example.R;
import io.agora.api.example.annotation.Example;
import io.agora.api.example.common.BaseFragment;
import io.agora.api.example.utils.CommonUtil;
import io.agora.rtc2.ChannelMediaOptions;
import io.agora.rtc2.ClientRoleOptions;
import io.agora.rtc2.Constants;
import io.agora.rtc2.IAudioFrameObserver;
import io.agora.rtc2.IRtcEngineEventHandler;
import io.agora.rtc2.RtcEngine;
import io.agora.rtc2.RtcEngineConfig;
import io.agora.rte.custom_opensl_chorus.OpenSlChorusPlayer;

import static io.agora.api.example.common.model.Examples.ADVANCED;
import static io.agora.api.example.examples.advanced.customaudio.AudioRecordService.RecordThread.DEFAULT_CHANNEL_COUNT;
import static io.agora.api.example.examples.advanced.customaudio.AudioRecordService.RecordThread.DEFAULT_SAMPLE_RATE;

/**This demo demonstrates how to make a one-to-one voice call*/
@Example(
    index = 29,
    group = ADVANCED,
    name = R.string.item_customaudiochorus,
    actionId = R.id.action_mainFragment_to_CustomAudioChorus,
    tipsId = R.string.customaudiochorus
)
public class CustomAudioChorus extends BaseFragment implements View.OnClickListener
{
    private OpenSlChorusPlayer openSlChorusPlayer;
    private static final String TAG = CustomAudioChorus.class.getSimpleName();
    private EditText et_channel, et_aggresiveness;
    private Button mute, join, playback, aggresiveness, aecEnable;
    private int myUid;
    private boolean joined = false;
    public static RtcEngine engine;

    @Override
    public void onCreate(@Nullable Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        handler = new Handler();
        openSlChorusPlayer = new OpenSlChorusPlayer();
    }

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState)
    {
        View view = inflater.inflate(R.layout.fragment_custom_audio_chorus, container, false);
        return view;
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState)
    {
        super.onViewCreated(view, savedInstanceState);
        join = view.findViewById(R.id.btn_join);
        et_channel = view.findViewById(R.id.et_channel);
        et_aggresiveness = view.findViewById(R.id.et_aggresiveness);
        view.findViewById(R.id.btn_join).setOnClickListener(this);
        mute = view.findViewById(R.id.btn_mute);
        mute.setText("mute");
        mute.setOnClickListener(this);
        playback = view.findViewById(R.id.btn_playback);
        playback.setText("playback");
        playback.setOnClickListener(this);
        aggresiveness = view.findViewById(R.id.btn_aggressiveness);
        aggresiveness.setText("aggresiveness");
        aggresiveness.setOnClickListener(this);
        aecEnable = view.findViewById(R.id.btn_enable_aec);
        aecEnable.setText("aecEnable");
        aecEnable.setOnClickListener(this);
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
            RtcEngineConfig config = new RtcEngineConfig();
            config.mContext = context.getApplicationContext();
            config.mAppId = getString(R.string.agora_app_id);
            config.mChannelProfile = Constants.CHANNEL_PROFILE_LIVE_BROADCASTING;
            config.mEventHandler = iRtcEngineEventHandler;
            config.mEnableAudioDevice = false;

            config.mAudioScenario = Constants.AudioScenario.getValue(Constants.AudioScenario.HIGH_DEFINITION);
            engine = RtcEngine.create(config);
            engine.setParameters("{\"rtc.audio_resend\":false}");
            engine.setParameters("{\"rtc.audio_fec\":[3,2]}");
            engine.setParameters("{\"rtc.audio.aec.enable\":false}");

            engine.setAudioProfile(Constants.AUDIO_PROFILE_MUSIC_HIGH_QUALITY_STEREO, Constants.AUDIO_SCENARIO_CHORUS);

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
                String channelId = et_channel.getText().toString();
                if (AndPermission.hasPermissions(this, Permission.Group.STORAGE, Permission.Group.MICROPHONE, Permission.Group.CAMERA))
                {
                    joinChannel(channelId);
                    return;
                }
                AndPermission.with(this).runtime().permission(
                    Permission.Group.STORAGE,
                    Permission.Group.MICROPHONE
                ).onGranted(permissions ->
                {
                    joinChannel(channelId);
                }).start();
            }
            else
            {
                joined = false;
                openSlChorusPlayer.stopOpenslDemo();
                engine.leaveChannel();
                join.setText(getString(R.string.join));
                mute.setText(getString(R.string.closemicrophone));
                mute.setEnabled(false);
            }
        }
        else if (v.getId() == R.id.btn_playback)
        {
            if (echoed) {
                openSlChorusPlayer.stopEchoTest();
                playback.setText("playback disabled");
                echoed = false;
            }
            else {
                openSlChorusPlayer.startEchoTest();
                playback.setText("playback enabled");
                echoed = true;
            }
        }
        else if (v.getId() == R.id.btn_mute)
        {
            muteLocalAudioStream(!muted);
            mute.setText("muted: " + muted);
        }
        else if (v.getId() == R.id.btn_aggressiveness)
        {
            engine.setParameters("{\"rtc.audio.aec.aggressiveness\":" + et_aggresiveness.getText().toString() + "}");
            aggresiveness.setText("aggresiveness applied: " + et_aggresiveness.getText());
        }
        else if (v.getId() == R.id.btn_enable_aec)
        {
            mAecEnabled = !mAecEnabled;
            engine.setParameters("{\"rtc.audio.aec.enable_external\":" + mAecEnabled + "}");
            aecEnable.setText("aecEnable: " + mAecEnabled);
        }
    }

    @Override
    public void onDestroyView() {
        openSlChorusPlayer.stopOpenslDemo();
        engine.leaveChannel();
        RtcEngine.destroy();
        super.onDestroyView();
    }

    public void muteLocalAudioStream(boolean mute) {
        ChannelMediaOptions channelMediaOptions = new ChannelMediaOptions();
        channelMediaOptions.publishCustomAudioTrackAec = !mute;
        engine.updateChannelMediaOptions(channelMediaOptions);
        muted = mute;
    }

    boolean echoed = false;
    boolean muted = false;
    boolean mAecEnabled = true;

    private void joinChannel(String channelId) {
        openSlChorusPlayer.initOpenslDemo(engine.getNativeHandle());

        String accessToken = getString(R.string.agora_access_token);
        if (TextUtils.equals(accessToken, "") || TextUtils.equals(accessToken, "<#YOUR ACCESS TOKEN#>")) {
            accessToken = null;
        }
        ChannelMediaOptions channelMediaOptions = new ChannelMediaOptions();
        channelMediaOptions.publishCustomAudioTrackAec = true;
        channelMediaOptions.publishCustomAudioTrack = false;
        channelMediaOptions.audioOptionsAdvanced.setEnableAecExternalForCustom(true);
        int res = engine.joinChannel(accessToken, channelId, 0, channelMediaOptions);
        if (res != 0) {
            showAlert(RtcEngine.getErrorDescription(Math.abs(res)));
            return;
        }
        join.setEnabled(false);
    }

    private final IRtcEngineEventHandler iRtcEngineEventHandler = new IRtcEngineEventHandler() {
        @Override
        public void onWarning(int warn) {
            Log.w(TAG, String.format("onWarning code %d message %s", warn, RtcEngine.getErrorDescription(warn)));
        }

        @Override
        public void onError(int err) {
            Log.e(TAG, String.format("onError code %d message %s", err, RtcEngine.getErrorDescription(err)));
            showAlert(String.format("onError code %d message %s", err, RtcEngine.getErrorDescription(err)));
        }

        @Override
        public void onJoinChannelSuccess(String channel, int uid, int elapsed) {
            Log.i(TAG, String.format("onJoinChannelSuccess channel %s uid %d", channel, uid));
            showLongToast(String.format("onJoinChannelSuccess channel %s uid %d", channel, uid));
            myUid = uid;
            joined = true;
            handler.post(new Runnable()
            {
                @Override
                public void run()
                {
                    mute.setEnabled(true);
                    join.setEnabled(true);
                    join.setText(getString(R.string.leave));
                }
            });
            openSlChorusPlayer.startOpenslDemo();
        }

        @Override
        public void onRemoteAudioStateChanged(int uid, IRtcEngineEventHandler.REMOTE_AUDIO_STATE state, IRtcEngineEventHandler.REMOTE_AUDIO_STATE_REASON reason, int elapsed) {
            super.onRemoteAudioStateChanged(uid, state, reason, elapsed);
            Log.i(TAG, "onRemoteAudioStateChanged->" + uid + ", state->" + state + ", reason->" + reason);
        }
    };
}