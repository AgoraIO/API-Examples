// Keep receiver stable until synchronous release completes; discard queued events on exit.
// Correct: Thread-safe UI updates via message map

// Event handler (may be called from background thread)
void CExampleRtcEngineEventHandler::onJoinChannelSuccess(const char* channel, agora::rtc::uid_t uid, int elapsed) {
    if (m_hMsgHandler) {
        // Post message to main thread
        ::PostMessage(m_hMsgHandler, WM_MSGID(EID_JOINCHANNEL_SUCCESS), (WPARAM)uid, 0);
    }
}

// Message handler (runs on main thread)
LRESULT CExampleDlg::OnMsgEngineEvent(WPARAM wParam, LPARAM lParam) {
    if (!m_rtcEngine) return 0;
    // Safe to update UI here
    m_statusText.SetWindowText(_T("Joined channel"));
    return 0;
}
