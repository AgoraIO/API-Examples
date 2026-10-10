// Scene lifecycle fragments; see example-template.h/.cpp for the complete skeleton.

// OnInitDialog(): UI only. APIExampleDlg precreates inactive dialogs.

// InitAgora(), invoked by CreateScene():
m_appId = GET_APP_ID; // Member-owned UTF-8 value survives initialize().
m_rtcEngine = createAgoraRtcEngine();
if (!m_rtcEngine) return false;
m_eventHandler.SetMsgReceiver(m_hWnd);
RtcEngineContext context;
context.appId = m_appId.c_str();
context.eventHandler = &m_eventHandler;
if (m_rtcEngine->initialize(context) != 0) {
    UnInitAgora();
    return false;
}

// Join after device/permission checks; token/channelName/uid belong to the active case.
int result = m_rtcEngine->joinChannel(token.c_str(), channelName.c_str(), "", uid);
// Handle nonzero result; success is reported asynchronously.

// UnInitAgora(), invoked by ReleaseScene() before hiding the window:
// Invalidate pending requests and stop case-owned media before release.
if (m_rtcEngine) {
    m_rtcEngine->leaveChannel();
    m_rtcEngine->release(nullptr); // Synchronous in this SDK; never in an SDK callback.
    m_rtcEngine = nullptr;
}
m_eventHandler.SetMsgReceiver(nullptr);
DiscardEngineMessages(); // Drop scalar callback messages left by the old engine.
