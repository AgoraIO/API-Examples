#pragma once
#include "AGVideoWnd.h"
#include <list>
#include <string>

// Event handler for Agora engine callbacks
class C<ExampleName>RtcEngineEventHandler : public IRtcEngineEventHandler {
public:
    void SetMsgReceiver(HWND hWnd) { m_hMsgHandler = hWnd; }
    
    virtual void onJoinChannelSuccess(const char* channel, uid_t uid, int elapsed) override;
    virtual void onUserJoined(uid_t uid, int elapsed) override;
    virtual void onUserOffline(uid_t uid, USER_OFFLINE_REASON_TYPE reason) override;
    virtual void onError(int err) override;
    
private:
    HWND m_hMsgHandler = nullptr;
};

// Main dialog class
class C<ExampleName>Dlg : public CDialogEx {
    DECLARE_DYNAMIC(C<ExampleName>Dlg)
    
public:
    C<ExampleName>Dlg(CWnd* pParent = nullptr);
    virtual ~C<ExampleName>Dlg();
    
    enum { IDD = IDD_DIALOG_<EXAMPLENAME> };
    
protected:
    virtual BOOL OnInitDialog();
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual void PostNcDestroy();
    
    afx_msg void OnBnClickedButtonJoin();
    afx_msg void OnBnClickedButtonLeave();
    afx_msg LRESULT OnMsgEngineEvent(WPARAM wParam, LPARAM lParam);
    
    DECLARE_MESSAGE_MAP()
    
private:
    void InitializeAgoraEngine();
    void JoinChannel();
    void LeaveChannel();
    void UpdateUI();
    
    IRtcEngine* m_rtcEngine = nullptr;
    C<ExampleName>RtcEngineEventHandler m_eventHandler;
    uid_t m_remoteUid = 0;
    bool m_isJoined = false;
};
