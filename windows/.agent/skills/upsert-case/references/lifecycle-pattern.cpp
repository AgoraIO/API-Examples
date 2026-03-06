// Proper Engine Lifecycle Pattern

// Create
m_rtcEngine = createAgoraRtcEngine();

// Initialize
RtcEngineContext context;
context.appId = CConfig::GetAppId();
context.eventHandler = &m_eventHandler;
m_rtcEngine->initialize(context);

// Use
m_rtcEngine->joinChannel(token, channelName, "", 0);

// Cleanup
m_rtcEngine->leaveChannel();
m_rtcEngine->release();
m_rtcEngine = nullptr;
