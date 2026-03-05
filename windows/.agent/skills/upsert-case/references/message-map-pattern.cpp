// Message Map Pattern - Thread-safe UI updates

// Header
BEGIN_MESSAGE_MAP(CExampleDlg, CDialogEx)
    ON_BN_CLICKED(IDC_BUTTON_JOIN, &CExampleDlg::OnBnClickedButtonJoin)
    ON_MESSAGE(WM_MSGID(EID_JOIN_CHANNEL_SUCCESS), &CExampleDlg::OnMsgEngineEvent)
END_MESSAGE_MAP()

// Implementation
void CExampleDlg::OnBnClickedButtonJoin() {
    JoinChannel();
}

LRESULT CExampleDlg::OnMsgEngineEvent(WPARAM wParam, LPARAM lParam) {
    // Handle engine events on main thread
    return 0;
}
