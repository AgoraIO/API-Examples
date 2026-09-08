#pragma once
#include <string>

// Included after stdafx.h, as in existing MFC cases.
// Replace <ExampleName>/<EXAMPLENAME> and register the dialog resource and controls.
class C<ExampleName>RtcEngineEventHandler : public IRtcEngineEventHandler {
public:
    // Set before initialize, then keep stable until synchronous release completes.
    void SetMsgReceiver(HWND hWnd) { m_hMsgHandler = hWnd; }
    void onJoinChannelSuccess(const char* channel, agora::rtc::uid_t uid, int elapsed) override;
    void onError(int err, const char* msg) override;

private:
    HWND m_hMsgHandler = nullptr;
};

class C<ExampleName>Dlg : public CDialogEx {
    DECLARE_DYNAMIC(C<ExampleName>Dlg)

public:
    C<ExampleName>Dlg(CWnd* pParent = nullptr);
    virtual ~C<ExampleName>Dlg();
    enum { IDD = IDD_DIALOG_<EXAMPLENAME> };

    // APIExampleDlg calls these from CreateScene() / ReleaseScene().
    bool InitAgora();
    void UnInitAgora();

protected:
    BOOL OnInitDialog() override;
    void DoDataExchange(CDataExchange* pDX) override;
    afx_msg void OnBnClickedButtonJoin();
    afx_msg void OnBnClickedButtonLeave();
    afx_msg LRESULT OnMsgJoinChannel(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnMsgError(WPARAM wParam, LPARAM lParam);
    DECLARE_MESSAGE_MAP()

private:
    void JoinChannel();
    void LeaveChannel();
    void DiscardEngineMessages();

    IRtcEngine* m_rtcEngine = nullptr;
    C<ExampleName>RtcEngineEventHandler m_eventHandler;
    std::string m_appId;
};
