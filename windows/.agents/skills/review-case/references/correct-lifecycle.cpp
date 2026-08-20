// Correct Engine Lifecycle Pattern

BOOL CExampleDlg::OnInitDialog() {
    CDialogEx::OnInitDialog();
    InitializeAgoraEngine();  // Create once
    return TRUE;
}

void CExampleDlg::PostNcDestroy() {
    LeaveChannel();
    if (m_rtcEngine) {
        m_rtcEngine->release();
        m_rtcEngine = nullptr;
    }
    CDialogEx::PostNcDestroy();
    delete this;
}

void CExampleDlg::JoinChannel() {
    if (!m_rtcEngine) return;
    m_rtcEngine->joinChannel(token, channelName, "", 0);
}

void CExampleDlg::LeaveChannel() {
    if (!m_rtcEngine) return;
    m_rtcEngine->leaveChannel();
}
