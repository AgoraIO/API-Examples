// ❌ INCORRECT: Creating engine on every join
void CExampleDlg::JoinChannel() {
    m_rtcEngine = createAgoraRtcEngine();  // WRONG: recreates engine
    m_rtcEngine->joinChannel(...);
}

// ❌ INCORRECT: Not calling release
void CExampleDlg::PostNcDestroy() {
    m_rtcEngine->leaveChannel();
    // Missing: m_rtcEngine->release()
    CDialogEx::PostNcDestroy();
}
