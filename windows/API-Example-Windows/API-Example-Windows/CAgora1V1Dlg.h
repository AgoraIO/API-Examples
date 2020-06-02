#pragma once


// CAgora1V1Dlg dialog

class CAgora1V1RtcEngineEventHandler
    : public IRtcEngineEventHandler
{
public:
    CAgora1V1RtcEngineEventHandler() {}
    ~CAgora1V1RtcEngineEventHandler() {}
    void SetMsgReceiver(HWND hWnd) { m_hMsgHanlder = hWnd; }
    virtual void onJoinChannelSuccess(const char* channel, uid_t uid, int elapsed) override;
    virtual void onUserJoined(uid_t uid, int elapsed) override;
    virtual void onUserOffline(uid_t uid, USER_OFFLINE_REASON_TYPE reason) override;
    virtual void onLeaveChannel(const RtcStats& stats) override;
private:
    HWND m_hMsgHanlder;
};


class CAgora1V1Dlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAgora1V1Dlg)

public:
	CAgora1V1Dlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CAgora1V1Dlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_JOINCHANNEL };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedJoinchannel();
    //Agora Event handler
    afx_msg LRESULT OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnEIDLeaveChannel(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnEIDUserJoined(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnEIDUserOffline(WPARAM wParam, LPARAM lParam);
    virtual BOOL OnInitDialog();
private:
    IRtcEngine* m_rtcEngine;
    CAgora1V1RtcEngineEventHandler m_eventHandler1V1;
    bool joinChannel = false;
    bool m_initialize = false;
public:
    CEdit m_edtChannelName;
    CStatic m_remoteVideoWnd;
    CStatic m_localVideoWnd;
    CButton m_btnJoinChannel;
    afx_msg void OnDestroy();
    CStatic m_staInfo;
};
