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
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import java.util.Random;

import io.agora.api.example.MainApplication;
import io.agora.api.example.R;
import io.agora.api.example.annotation.Example;
import io.agora.api.example.common.BaseFragment;
import io.agora.api.example.common.model.StatisticsInfo;
import io.agora.rtc2.ClientRoleOptions;
import io.agora.rtc2.Constants;
import io.agora.rtc2.EchoTestConfiguration;
import io.agora.rtc2.IRtcEngineEventHandler;
import io.agora.rtc2.RtcEngine;
import io.agora.rtc2.RtcEngineConfig;
import io.agora.rtc2.internal.LastmileProbeConfig;
import io.agora.rtc2.proxy.LocalAccessPointConfiguration;

/**
 * The type Pre call test.
 */
@Example(
        index = 16,
        group = ADVANCED,
        name = R.string.item_precalltest,
        actionId = R.id.action_mainFragment_to_PreCallTest,
        tipsId = R.string.precalltest
)
public class PreCallTest extends BaseFragment implements View.OnClickListener {
    private static final String TAG = PreCallTest.class.getSimpleName();

    private RtcEngine engine;
    private int myUid;
    private Button btn_lastmile, btn_echo_audio, btn_echo_video;
    private StatisticsInfo statisticsInfo;
    private TextView lastmileQuality, lastmileResult;
    private static final Integer MAX_COUNT_DOWN = 8;

    @Override
    public void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        handler = new Handler();
    }

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment_precall_test, container, false);
        return view;
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
             * The SDK uses this class to report to the app on SDK runtime evepnts.
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
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        statisticsInfo = new StatisticsInfo();
        btn_echo_audio = view.findViewById(R.id.btn_echo);
        btn_echo_video = view.findViewById(R.id.btn_echo_video);
        btn_echo_audio.setOnClickListener(this);
        btn_echo_video.setOnClickListener(this);
        btn_lastmile = view.findViewById(R.id.btn_lastmile);
        btn_lastmile.setOnClickListener(this);
        lastmileQuality = view.findViewById(R.id.lastmile_quality);
        lastmileResult = view.findViewById(R.id.lastmile_result);
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        RtcEngine.destroy();
    }

    @Override
    public void onClick(View v) {
        if (v.getId() == R.id.btn_lastmile) {
            // Configure a LastmileProbeConfig instance.
            LastmileProbeConfig config = new LastmileProbeConfig() {
            };
            // Probe the uplink network quality.
            config.probeUplink = true;
            // Probe the downlink network quality.
            config.probeDownlink = true;
            // The expected uplink bitrate (bps). The value range is [100000, 5000000].
            config.expectedUplinkBitrate = 100000;
            // The expected downlink bitrate (bps). The value range is [100000, 5000000].
            config.expectedDownlinkBitrate = 100000;
            // Start the last-mile network test before joining the channel.
            engine.startLastmileProbeTest(config);
            btn_lastmile.setEnabled(false);
            btn_lastmile.setText("Testing ...");
        } else if (v.getId() == R.id.btn_echo) {
            EchoTestConfiguration config = new EchoTestConfiguration();
            config.enableVideo = false;
            config.enableAudio = true;
            config.intervalInSeconds = MAX_COUNT_DOWN;
            config.channelId = "AudioEchoTest" + (new Random().nextInt(1000) + 10000);
            engine.startEchoTest(config);
            btn_echo_audio.setEnabled(false);
            btn_echo_audio.setText("Recording on Microphone ...");
            btn_echo_video.setEnabled(false);
            btn_echo_audio.post(new Runnable() {
                int countDownNum = 0;

                @Override
                public void run() {
                    countDownNum++;
                    if (countDownNum >= MAX_COUNT_DOWN * 2) {
                        btn_echo_video.setEnabled(true);
                        btn_echo_audio.setEnabled(true);
                        btn_echo_audio.setText(R.string.start);
                        engine.stopEchoTest();
                    } else if (countDownNum >= MAX_COUNT_DOWN) {
                        btn_echo_audio.setText("PLaying with " + (MAX_COUNT_DOWN * 2 - countDownNum) + "Seconds");
                        btn_echo_audio.postDelayed(this, 1000);
                    } else {
                        btn_echo_audio.setText("Recording with " + (MAX_COUNT_DOWN - countDownNum) + "Seconds");
                        btn_echo_audio.postDelayed(this, 1000);
                    }
                }
            });
        } else if (v.getId() == R.id.btn_echo_video) {
            EchoTestConfiguration config = new EchoTestConfiguration();
            config.enableVideo = true;
            config.view = requireView().findViewById(R.id.surfaceView);
            config.enableAudio = false;
            config.intervalInSeconds = MAX_COUNT_DOWN;
            config.channelId = "VideoEchoTest" + (new Random().nextInt(1000) + 10000);
            engine.startEchoTest(config);
            btn_echo_audio.setEnabled(false);
            btn_echo_video.setEnabled(false);
            btn_echo_video.setText(R.string.stop);
            btn_echo_video.postDelayed(() -> {
                btn_echo_video.setEnabled(true);
                btn_echo_audio.setEnabled(true);
                btn_echo_video.setText(R.string.start);
                engine.stopEchoTest();
            }, MAX_COUNT_DOWN * 2 * 1000);
        }
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


        @Override
        public void onClientRoleChanged(int oldRole, int newRole, ClientRoleOptions newRoleOptions) {
            super.onClientRoleChanged(oldRole, newRole, newRoleOptions);
            showLongToast("onClientRoleChanged >> newRole = " + newRole);
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
        }

        /**
         * Implemented in the global IRtcEngineEventHandler class.
         * Triggered 2 seconds after starting the last-mile test.
         * @param quality
         */
        @Override
        public void onLastmileQuality(int quality) {
            statisticsInfo.setLastMileQuality(quality);
            updateLastMileResult();
        }

        /**
         * Implemented in the global IRtcEngineEventHandler class.
         * Triggered 30 seconds after starting the last-mile test.
         * @param lastmileProbeResult
         */
        @Override
        public void onLastmileProbeResult(LastmileProbeResult lastmileProbeResult) {
            // (1) Stop the test. Agora recommends not calling any other API method before the test ends.
            engine.stopLastmileProbeTest();
            statisticsInfo.setLastMileProbeResult(lastmileProbeResult);
            updateLastMileResult();
            handler.post(() -> {
                btn_lastmile.setEnabled(true);
                btn_lastmile.setText(R.string.start);
            });
        }

    };

    private void updateLastMileResult() {
        handler.post(() -> {
            if (statisticsInfo.getLastMileQuality() != null) {
                lastmileQuality.setText("Quality: " + statisticsInfo.getLastMileQuality());
            }
            if (statisticsInfo.getLastMileResult() != null) {
                lastmileResult.setText(statisticsInfo.getLastMileResult());
            }
        });
    }

}
