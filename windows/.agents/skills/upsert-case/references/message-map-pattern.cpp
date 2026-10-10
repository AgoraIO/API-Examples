// In the dialog class declaration (.h):
// afx_msg LRESULT OnMsgJoinChannel(WPARAM wParam, LPARAM lParam);
// afx_msg LRESULT OnMsgError(WPARAM wParam, LPARAM lParam);
// DECLARE_MESSAGE_MAP()

// At file scope in the .cpp, not inside the class declaration:
BEGIN_MESSAGE_MAP(CExampleDlg, CDialogEx)
    ON_MESSAGE(WM_MSGID(EID_JOINCHANNEL_SUCCESS), &CExampleDlg::OnMsgJoinChannel)
    ON_MESSAGE(WM_MSGID(EID_ERROR), &CExampleDlg::OnMsgError)
END_MESSAGE_MAP()

LRESULT CExampleDlg::OnMsgError(WPARAM wParam, LPARAM lParam) {
    if (!m_rtcEngine) return 0;
    // The message ID selects this handler; wParam is the error code posted by onError.
    TRACE(_T("RTC error: %d\n"), (int)wParam);
    return 0;
}
