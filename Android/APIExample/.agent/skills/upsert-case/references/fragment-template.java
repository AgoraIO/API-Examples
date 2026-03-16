package io.agora.api.example.examples.advanced;

import static io.agora.api.example.common.model.Examples.ADVANCED;
import static io.agora.rtc2.Constants.RENDER_MODE_HIDDEN;

import android.content.Context;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.SurfaceView;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.EditText;
import android.widget.FrameLayout;

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
import io.agora.api.example.utils.PermissonUtils;
import io.agora.api.example.utils.TokenUtils;
import io.agora.rtc2.ChannelMediaOptions;
import io.agora.rtc2.Constants;
import io.agora.rtc2.IRtcEngineEventHandler;
import io.agora.rtc2.RtcEngine;
import io.agora.rtc2.RtcEngineConfig;
import io.agora.rtc2.proxy.LocalAccessPointConfiguration;
import io.agora.rtc2.video.VideoCanvas;

/**
 * This demo demonstrates how to use [describe the feature here].
 *
 * Key APIs used:
 * - RtcEngine.yourApi()
 */
@Example(
        index    = 10,                                           // unique within the group; BASIC: 0–9, ADVANCED: 10+
        group    = ADVANCED,                                     // BASIC or ADVANCED
        name     = R.string.item_your_case_name,
        actionId = R.id.action_mainFragment_to_yourCaseName,
        tipsId   = R.string.your_case_name_tips
)
public class YourCaseName extends BaseFragment implements View.OnClickListener {
    private static final String TAG = YourCaseName.class.getSimpleName();

    // For video cases: add VideoReportLayout fields here
    // private VideoReportLayout fl_local, fl_remote;
    // private Map<Integer, ViewGroup> remoteViews = new ConcurrentHashMap<>();

    private Button join;
    private EditText et_channel;
    private RtcEngine engine;
    private int myUid;
    private boolean joined = false;

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {
        return inflater.inflate(R.layout.fragment_your_case_name, container, false);
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        join = view.findViewById(R.id.btn_join);
        et_channel = view.findViewById(R.id.et_channel);
        join.setOnClickListener(this);
        // bind additional views here
    }

    @Override
    public void onActivityCreated(@Nullable Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);
        Context context = getContext();
        if (context == null) return;
        try {
            RtcEngineConfig config = new RtcEngineConfig();
            config.mContext = context.getApplicationContext();
            config.mAppId = getAgoraAppId();
            config.mChannelProfile = Constants.CHANNEL_PROFILE_LIVE_BROADCASTING;
            config.mEventHandler = iRtcEngineEventHandler;
            config.mAudioScenario = Constants.AudioScenario.getValue(Constants.AudioScenario.DEFAULT);
            config.mAreaCode = ((MainApplication) getActivity().getApplication())
                    .getGlobalSettings().getAreaCode();
            engine = RtcEngine.create(config);
            // REQUIRED in every case — do not remove
            engine.setParameters("{"
                    + "\"rtc.report_app_scenario\":"
                    + "{"
                    + "\"appScenario\":" + 100 + ","
                    + "\"serviceType\":" + 11 + ","
                    + "\"appVersion\":\"" + RtcEngine.getSdkVersion() + "\""
                    + "}"
                    + "}");
            // null-check is mandatory — returns null on non-private-cloud builds
            LocalAccessPointConfiguration localAccessPointConfiguration =
                    ((MainApplication) getActivity().getApplication())
                            .getGlobalSettings().getPrivateCloudConfig();
            if (localAccessPointConfiguration != null) {
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
        if (engine != null) {
            engine.leaveChannel();
        }
        // MUST use handler.post — do NOT call RtcEngine.destroy() directly on main thread
        handler.post(RtcEngine::destroy);
        engine = null;
    }

    @Override
    public void onClick(View v) {
        if (v.getId() == R.id.btn_join) {
            if (!joined) {
                String channelId = et_channel.getText().toString();
                checkOrRequestPermisson(new PermissonUtils.PermissionResultCallback() {
                    @Override
                    public void onPermissionsResult(boolean allPermissionsGranted,
                                                    String[] permissions, int[] grantResults) {
                        if (allPermissionsGranted) {
                            joinChannel(channelId);
                        }
                    }
                });
            } else {
                joined = false;
                engine.leaveChannel();
                join.setText(getString(R.string.join));
            }
        }
    }

    private void joinChannel(String channelId) {
        engine.setClientRole(Constants.CLIENT_ROLE_BROADCASTER);
        // --- feature-specific setup goes here ---
        // e.g. engine.enableVideo(); engine.setVideoEncoderConfiguration(...);

        ChannelMediaOptions options = new ChannelMediaOptions();
        options.autoSubscribeAudio = true;
        options.autoSubscribeVideo = true;
        options.publishMicrophoneTrack = true;
        options.publishCameraTrack = true;  // remove for audio-only cases

        int uid = new Random().nextInt(1000) + 100000;
        TokenUtils.gen(requireContext(), channelId, uid, token -> {
            int res = engine.joinChannel(token, channelId, uid, options);
            if (res != 0) {
                showAlert(RtcEngine.getErrorDescription(Math.abs(res)));
                return;
            }
            join.setEnabled(false);
        });
    }

    private final IRtcEngineEventHandler iRtcEngineEventHandler = new IRtcEngineEventHandler() {
        @Override
        public void onJoinChannelSuccess(String channel, int uid, int elapsed) {
            Log.i(TAG, String.format("onJoinChannelSuccess channel %s uid %d", channel, uid));
            myUid = uid;
            joined = true;
            // ALL UI updates must go through runOnUIThread — callbacks run on background thread
            runOnUIThread(() -> {
                join.setEnabled(true);
                join.setText(getString(R.string.leave));
            });
        }

        @Override
        public void onUserJoined(int uid, int elapsed) {
            Log.i(TAG, "onUserJoined -> " + uid);
            runOnUIThread(() -> {
                // For video cases: create SurfaceView, call engine.setupRemoteVideo(...)
            });
        }

        @Override
        public void onUserOffline(int uid, int reason) {
            Log.i(TAG, String.format("user %d offline, reason %d", uid, reason));
            runOnUIThread(() -> {
                // For video cases: removeAllViews(), call engine.setupRemoteVideo(null, ...)
            });
        }

        @Override
        public void onError(int err) {
            showLongToast("Error code:" + err + ", msg:" + RtcEngine.getErrorDescription(err));
        }
    };
}
