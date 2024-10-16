package io.agora.api.example.examples.advanced;

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

import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;

import io.agora.api.example.MainApplication;
import io.agora.api.example.R;
import io.agora.api.example.annotation.Example;
import io.agora.api.example.common.BaseFragment;
import io.agora.api.example.common.widget.AudioSeatManager;
import io.agora.api.example.utils.CommonUtil;
import io.agora.api.example.utils.PermissonUtils;
import io.agora.api.example.utils.TokenUtils;
import io.agora.rtc2.ChannelMediaOptions;
import io.agora.rtc2.Constants;
import io.agora.rtc2.IAudioFrameObserver;
import io.agora.rtc2.IRtcEngineEventHandler;
import io.agora.rtc2.RtcEngine;
import io.agora.rtc2.RtcEngineConfig;
import io.agora.rtc2.audio.AudioParams;
import io.agora.rtc2.proxy.LocalAccessPointConfiguration;

/**
 * This demo demonstrates how to make a one-to-one voice call
 *
 * @author cjw
 */
@Example(
        index = 9,
        group = ADVANCED,
        name = R.string.item_raw_audio,
        actionId = R.id.action_mainFragment_raw_audio,
        tipsId = R.string.rawaudio
)
public class ProcessAudioRawData extends BaseFragment implements View.OnClickListener, CompoundButton.OnCheckedChangeListener {
    private static final String TAG = ProcessAudioRawData.class.getSimpleName();
    private EditText et_channel;
    private Button join;
    private Switch writeBackAudio;
    private RtcEngine engine;
    private int myUid;
    private boolean joined = false;
    private volatile boolean isWriteBackAudio = false;
    private static final Integer SAMPLE_RATE = 44100;
    private static final Integer SAMPLE_NUM_OF_CHANNEL = 1;
    private static final Integer SAMPLES = 1024;
    private static final String AUDIO_FILE = "output.raw";
    private InputStream inputStream;

    private AudioSeatManager audioSeatManager;

    private void openAudioFile() {
        try {
            inputStream = this.getResources().getAssets().open(AUDIO_FILE);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private void closeAudioFile() {
        try {
            inputStream.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private byte[] readBuffer() {
        int byteSize = SAMPLES * SAMPLE_NUM_OF_CHANNEL * 2;
        byte[] buffer = new byte[byteSize];
        try {
            if (inputStream.read(buffer) < 0) {
                inputStream.reset();
                return readBuffer();
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
        return buffer;
    }

    @Override
    public void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        handler = new Handler();
    }

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment_raw_audio, container, false);
        return view;
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        join = view.findViewById(R.id.btn_join);
        et_channel = view.findViewById(R.id.et_channel);
        view.findViewById(R.id.btn_join).setOnClickListener(this);
        writeBackAudio = view.findViewById(R.id.writebackAudio);
        writeBackAudio.setOnCheckedChangeListener(this);

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
            engine.registerAudioFrameObserver(iAudioFrameObserver);
            engine.setRecordingAudioFrameParameters(SAMPLE_RATE, SAMPLE_NUM_OF_CHANNEL, Constants.RAW_AUDIO_FRAME_OP_MODE_READ_WRITE, SAMPLES);
            engine.setPlaybackAudioFrameParameters(SAMPLE_RATE, SAMPLE_NUM_OF_CHANNEL, Constants.RAW_AUDIO_FRAME_OP_MODE_READ_WRITE, SAMPLES);
            openAudioFile();
        } catch (Exception e) {
            e.printStackTrace();
            getActivity().onBackPressed();
        }
    }

    private byte[] audioAggregate(byte[] origin, byte[] buffer) {
        byte[] output = new byte[buffer.length];
        for (int i = 0; i < origin.length; i++) {
            output[i] = (byte) ((long) origin[i] / 2 + (long) buffer[i] / 2);
            if (i == 2) {
                Log.i(TAG, "origin :" + (int) origin[i] + " audio: " + (int) buffer[i]);
            }
        }
        return output;
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        /*leaveChannel and Destroy the RtcEngine instance*/
        if (engine != null) {
            engine.registerAudioFrameObserver(null);
            engine.leaveChannel();
        }
        handler.post(RtcEngine::destroy);
        engine = null;
        closeAudioFile();
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
        engine.setDefaultAudioRoutetoSpeakerphone(true);

        /*Please configure accessToken in the string_config file.
         * A temporary token generated in Console. A temporary token is valid for 24 hours. For details, see
         *      https://docs.agora.io/en/Agora%20Platform/token?platform=All%20Platforms#get-a-temporary-token
         * A token generated at the server. This applies to scenarios with high-security requirements. For details, see
         *      https://docs.agora.io/en/cloud-recording/token_server_java?platform=Java*/
        TokenUtils.gen(requireContext(), channelId, 0, ret -> {
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

    private final IAudioFrameObserver iAudioFrameObserver = new IAudioFrameObserver() {

        @Override
        public boolean onRecordAudioFrame(String channel, int audioFrameType,
                                          int samples, int bytesPerSample,
                                          int channels, int samplesPerSec,
                                          ByteBuffer byteBuffer, long renderTimeMs, int bufferLength) {
            if (isWriteBackAudio) {
                int length = byteBuffer.remaining();
//                byteBuffer.flip();
                byte[] buffer = readBuffer();
                byte[] origin = new byte[length];
                byteBuffer.get(origin);
                byteBuffer.flip();
                byteBuffer.put(audioAggregate(origin, buffer), 0, byteBuffer.remaining());
                byteBuffer.flip();
            }
            return true;
        }


        @Override
        public boolean onPlaybackAudioFrame(String channel, int audioFrameType,
                                            int samples, int bytesPerSample,
                                            int channels, int samplesPerSec,
                                            ByteBuffer byteBuffer, long renderTimeMs,
                                            int bufferLength) {
            return false;
        }

        @Override
        public boolean onMixedAudioFrame(String channel, int audioFrameType,
                                         int samples, int bytesPerSample, int channels,
                                         int samplesPerSec, ByteBuffer byteBuffer,
                                         long renderTimeMs, int bufferLength) {
            return false;
        }

        @Override
        public boolean onEarMonitoringAudioFrame(int type, int samplesPerChannel, int bytesPerSample,
                                                 int channels, int samplesPerSec,
                                                 ByteBuffer buffer, long renderTimeMs, int avsyncType) {
            return false;
        }

        @Override
        public boolean onPlaybackAudioFrameBeforeMixing(String channelId, int uid, int type, int samplesPerChannel, int bytesPerSample, int channels, int samplesPerSec, ByteBuffer buffer, long renderTimeMs, int avsync_type, int rtpTimestamp) {
            return false;
        }

        @Override
        public int getObservedAudioFramePosition() {
            return 0;
        }

        @Override
        public AudioParams getRecordAudioParams() {
            return null;
        }

        @Override
        public AudioParams getPlaybackAudioParams() {
            return null;
        }

        @Override
        public AudioParams getMixedAudioParams() {
            return null;
        }

        @Override
        public AudioParams getEarMonitoringAudioParams() {
            return null;
        }

    };

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
                    writeBackAudio.setEnabled(true);
                    audioSeatManager.upLocalSeat(uid);
                }
            });
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
        public void onActiveSpeaker(int uid) {
            super.onActiveSpeaker(uid);
            Log.i(TAG, String.format("onActiveSpeaker:%d", uid));
        }
    };

    @Override
    public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
        isWriteBackAudio = b;
    }
}
