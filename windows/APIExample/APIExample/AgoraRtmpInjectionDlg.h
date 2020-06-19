#pragma once
#include <string>
#include "AGVideoWnd.h"
class CAgoraRtmpInjectionRtcEngineEventHandler
    : public IRtcEngineEventHandler
{
public:
    CAgoraRtmpInjectionRtcEngineEventHandler() {}
    ~CAgoraRtmpInjectionRtcEngineEventHandler() {}
    void SetMsgReceiver(HWND hWnd) { m_hMsgHanlder = hWnd; }
    virtual void onJoinChannelSuccess(const char* channel, uid_t uid, int elapsed) override;
    virtual void onStreamInjectedStatus(const char* url, uid_t uid, int status) override;
    virtual void onLeaveChannel(const RtcStats& stats) override;
    virtual void onUserJoined(uid_t uid, int elapsed) override;
    virtual void onUserOffline(uid_t uid, USER_OFFLINE_REASON_TYPE reason) override;

private:
    HWND m_hMsgHanlder;
};

// CAgoraRtmpInjectionDlg dialog

class CAgoraRtmpInjectionDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAgoraRtmpInjectionDlg)

public:
	CAgoraRtmpInjectionDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CAgoraRtmpInjectionDlg();
    bool InitAgora();
    void UnInitAgora();

// Dialog Data
    enum { IDD = IDD_DIALOG_RTMPINJECT };
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedButtonAddstream();
    afx_msg void OnBnClickedButtonJoinchannel();
    afx_msg LRESULT OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnEIDLeaveChannel(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnEIDStreamInjectedStatus(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnEIDUserJoined(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnEIDUserOffline(WPARAM wParam, LPARAM lParam);
    virtual BOOL OnInitDialog();

private:
    void InitCtrlText();
    void RenderLocalVideo();
    CAGVideoWnd m_localVideoWnd;

    IRtcEngine* m_rtcEngine = nullptr;
    CAgoraRtmpInjectionRtcEngineEventHandler m_eventHandler;
    bool joinChannel = false;
    bool m_initialize = false;
    std::string m_injectUrl;
    bool m_addInjectStream = false;
public:
    afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
    CListBox m_lstInfo;
    CButton m_btnJoinChannel;
    CButton m_btnAddStream;
    CEdit m_edtChannelName;
    CEdit m_edtInjectUrl;
    CStatic m_staVideoArea;

    
    CStatic m_staChannelName;
    CStatic m_staInjectUrl;
    CStatic m_staDetail;
    afx_msg void OnSelchangeListInfoBroadcasting();
};
