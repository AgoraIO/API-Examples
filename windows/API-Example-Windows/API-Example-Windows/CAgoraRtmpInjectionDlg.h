#pragma once

#include <string>
// CAgoraRtmpInjectionDlg dialog
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

class CAgoraRtmpInjectionDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAgoraRtmpInjectionDlg)

public:
	CAgoraRtmpInjectionDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CAgoraRtmpInjectionDlg();
    void EnableInjectWindow(BOOL bEnable);
// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_RTMP_INJECTION };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
    afx_msg LRESULT OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnEIDLeaveChannel(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnEIDStreamInjectedStatus(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnEIDUserJoined(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnEIDUserOffline(WPARAM wParam, LPARAM lParam);
public:
    virtual BOOL OnInitDialog();

private:
    IRtcEngine* m_rtcEngine;
    bool        m_initialize = false;
    CAgoraRtmpInjectionRtcEngineEventHandler m_eventHandlerInjection;
    bool joinChannel = false;
    CString m_strInjectUrl;
    bool addInjectStream = false;
    std::string injectUrl;
public:
    afx_msg void OnBnClickedButtonAddstream();
    afx_msg void OnBnClickedJoinchannel();
    afx_msg void OnDestroy();
    CEdit m_edtRtmpUrl;
    CEdit m_edtChannelName;
    CButton m_btnJoinChannel;
    CButton m_btnAddStream;
    CStatic m_staInfo;
    CStatic m_staLocalVideo;
    CStatic m_injectVideoWnd;
};
