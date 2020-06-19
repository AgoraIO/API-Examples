#pragma once

#include "AGVideoWnd.h"

#include <list>
// CLiveBroadcastingDlg dialog
#define VIDEO_COUNT      36
#define IDC_BASEWND_VIDEO               113
class CLiveBroadcastingRtcEngineEventHandler
    : public IRtcEngineEventHandler
{
public:
    CLiveBroadcastingRtcEngineEventHandler() {}
    ~CLiveBroadcastingRtcEngineEventHandler() {}
    void SetMsgReceiver(HWND hWnd) { m_hMsgHanlder = hWnd; }
    virtual void onJoinChannelSuccess(const char* channel, uid_t uid, int elapsed) override;
    virtual void onUserJoined(uid_t uid, int elapsed) override;
    virtual void onUserOffline(uid_t uid, USER_OFFLINE_REASON_TYPE reason) override;
    virtual void onLeaveChannel(const RtcStats& stats) override;
private:
    HWND m_hMsgHanlder;
};

class CLiveBroadcastingDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CLiveBroadcastingDlg)

public:
	CLiveBroadcastingDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CLiveBroadcastingDlg();

    bool InitAgora();
    void UnInitAgora();
    
// Dialog Data
    enum { IDD = IDD_DIALOG_LIVEBROADCASTING };
    enum PEOPLE_IN_CHANNEL_TYPE {
        PEOPLE_IN_CHANNEL_2 = 0,
        PEOPLE_IN_CHANNEL_4,
        PEOPLE_IN_CHANNEL_9,
        PEOPLE_IN_CHANNEL_16
    };
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnSelchangeComboPersons();
    afx_msg void OnSelchangeComboRole();
    afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
    //Agora Event handler
    afx_msg LRESULT OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnEIDLeaveChannel(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnEIDUserJoined(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnEIDUserOffline(WPARAM wParam, LPARAM lParam);
private:
    void InitCtrlText();
    void CreateAllVideoWnds();
    void ShowVideoWnds();
    void RenderLocalVideo();
    IRtcEngine* m_rtcEngine = nullptr;
    CLiveBroadcastingRtcEngineEventHandler m_eventHandler;
    bool joinChannel = false;
    bool m_initialize = false;
    //video wnd
    CAGVideoWnd m_videoWnds[VIDEO_COUNT];
    int m_maxVideoCount = 4;
    std::list<uid_t> m_lstUids;
public:
    CComboBox m_cmbRole;
    CStatic m_staRole;
    CComboBox m_cmbPersons;
    CEdit m_edtChannelName;
    CButton m_btnJoinChannel;
    afx_msg void OnBnClickedButtonJoinchannel();
    CListBox m_lstInfo;
    virtual BOOL OnInitDialog();
    CStatic m_videoArea;

    CStatic m_staPersons;
    CStatic m_staChannelName;
    CStatic m_staDetail;
    afx_msg void OnSelchangeListInfoBroadcasting();
};
