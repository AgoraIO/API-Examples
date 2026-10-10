// Methods inside the case dialog; InitAgora() is paired with this cleanup.
// APIExampleDlg precreates the window, then calls InitAgora() only on scene selection.
BOOL CExampleDlg::OnInitDialog() {
    CDialogEx::OnInitDialog();
    // Initialize controls only.
    return TRUE;
}

// ReleaseScene() must call this BEFORE ShowWindow(SW_HIDE).
void CExampleDlg::UnInitAgora() {
    // Invalidate pending Token/permission requests first.
    if (m_rtcEngine) {
        m_rtcEngine->leaveChannel();
        // Stop case-owned media, timers and observers before destruction.
        m_rtcEngine->release(nullptr); // Synchronous; keep m_eventHandler alive until return.
        m_rtcEngine = nullptr;
    }
    m_eventHandler.SetMsgReceiver(nullptr);
    DiscardEngineMessages(); // See the upsert-case template for the owned message IDs.
}
