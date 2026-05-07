import android.content.Context;
import android.os.Bundle;

import androidx.annotation.Nullable;

import io.agora.api.example.MainApplication;
import io.agora.api.example.common.BaseFragment;
import io.agora.api.example.common.widget.AudioSeatManager;
import io.agora.rtc2.ChannelMediaOptions;
import io.agora.rtc2.IRtcEngineEventHandler;
import io.agora.rtc2.RtcEngine;
import io.agora.rtc2.RtcEngineConfig;
import io.agora.rtc2.proxy.LocalAccessPointConfiguration;

/**
 * Reference skeleton only. Adapt this to a real case Fragment with its own UI, join flow,
 * and permission handling before compiling it into the project.
 */
public abstract class AudioExampleCaseTemplate extends BaseFragment {
    protected RtcEngine engine;
    protected AudioSeatManager audioSeatManager;
    protected final IRtcEngineEventHandler iRtcEngineEventHandler = new IRtcEngineEventHandler() {
    };

    @Override
    public void onActivityCreated(@Nullable Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);
        Context context = getContext();
        if (context == null || engine != null) {
            return;
        }
        RtcEngineConfig config = new RtcEngineConfig();
        config.mContext = context.getApplicationContext();
        config.mAppId = getAgoraAppId();
        config.mEventHandler = iRtcEngineEventHandler;
        config.mAreaCode =
                ((MainApplication) getActivity().getApplication()).getGlobalSettings().getAreaCode();
        engine = RtcEngine.create(config);
        engine.setParameters("{\"rtc.report_app_scenario\":{\"appScenario\":100,\"serviceType\":11,\"appVersion\":\""
                + RtcEngine.getSdkVersion() + "\"}}");
        LocalAccessPointConfiguration privateCloud =
                ((MainApplication) getActivity().getApplication()).getGlobalSettings().getPrivateCloudConfig();
        if (privateCloud != null) {
            engine.setLocalAccessPoint(privateCloud);
        }
    }

    protected ChannelMediaOptions buildAudioOnlyOptions() {
        ChannelMediaOptions options = new ChannelMediaOptions();
        options.autoSubscribeAudio = true;
        options.publishMicrophoneTrack = true;
        return options;
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        if (engine != null) {
            engine.leaveChannel();
            RtcEngine.destroy();
            engine = null;
        }
    }
}
