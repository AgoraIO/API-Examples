// Reference skeleton only.
// This file demonstrates the required engine lifecycle, reporting, and private-cloud patterns.
// It is not a drop-in compiled class; each real case must supply its own imports, annotation,
// event handler, engine config fields, and UI logic.
public abstract class ExampleCaseTemplate extends BaseFragment {
    // Supply case-specific handler/config fields in the real Fragment.
    protected IRtcEngineEventHandler iRtcEngineEventHandler;
    protected RtcEngine engine;

    @Override
    public void onActivityCreated(@Nullable Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);
        Context context = getContext();
        if (context == null) {
            return;
        }
        try {
            RtcEngineConfig config = new RtcEngineConfig();
            config.mContext = context.getApplicationContext();
            config.mAppId = getAgoraAppId();
            config.mEventHandler = iRtcEngineEventHandler;
            // Add case-specific config here, for example channel profile or area code.
            engine = RtcEngine.create(config);
            engine.setParameters("{\"rtc.report_app_scenario\":{\"appScenario\":100,\"serviceType\":11,\"appVersion\":\"" + RtcEngine.getSdkVersion() + "\"}}");
            LocalAccessPointConfiguration privateCloud =
                    ((MainApplication) getActivity().getApplication()).getGlobalSettings().getPrivateCloudConfig();
            if (privateCloud != null) {
                engine.setLocalAccessPoint(privateCloud);
            }
        } catch (Exception e) {
            e.printStackTrace();
            // Match the seeded case rollback pattern when engine init fails.
            getActivity().onBackPressed();
        }
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        if (engine != null) {
            engine.leaveChannel();
        }
        handler.post(RtcEngine::destroy);
        engine = null;
    }
}
