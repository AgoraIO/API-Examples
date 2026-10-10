// ❌ INCORRECT: Direct UI update from event handler
void CExampleRtcEngineEventHandler::onJoinChannelSuccess(const char* channel, uid_t uid, int elapsed) {
    m_pDlg->m_statusText.SetWindowText(_T("Joined"));  // WRONG: may crash
}
