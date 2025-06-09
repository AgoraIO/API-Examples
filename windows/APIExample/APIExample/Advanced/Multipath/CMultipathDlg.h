#pragma once
#include "AGVideoWnd.h"
#include <list>
#include <string>

// CMultipathDlg dialog

#define VIDEO_COUNT_MULTIPATH           2
#define IDC_BASEWND_VIDEO_MULTIPATH     200

class CMultipathRtcEngineEventHandler
    : public IRtcEngineEventHandler
{
public:
    //set the message notify window handler
    void SetMsgReceiver(HWND hWnd) { m_hMsgHanlder = hWnd; }
    
    virtual void onJoinChannelSuccess(const char* channel, uid_t uid, int elapsed) override;
    virtual void onUserJoined(uid_t uid, int elapsed) override;
    virtual void onUserOffline(uid_t uid, USER_OFFLINE_REASON_TYPE reason) override;
    virtual void onLeaveChannel(const RtcStats& stats) override;
    virtual void onMultipathStats(const MultipathStats& stats) override;
    virtual void onNetworkTypeChanged(NETWORK_TYPE type) override;

private:
    HWND m_hMsgHanlder;
};

class CMultipathDlg : public CDialogEx
{
    DECLARE_DYNAMIC(CMultipathDlg)

public:
    CMultipathDlg(CWnd* pParent = nullptr);   // standard constructor
    virtual ~CMultipathDlg();

    //Initialize the Agora SDK
    bool InitAgora();
    //UnInitialize the Agora SDK
    void UnInitAgora();
    
    //resume window status
    void ResumeStatus();

// Dialog Data
    enum { IDD = IDD_DIALOG_MULTIPATH };
    
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnBnClickedButtonJoinchannel();
    afx_msg void OnSelchangeComboRole();
    afx_msg void OnSelchangeComboMultipathMode();
    afx_msg void OnBnClickedCheckEnableMultipath();
    afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
    
    //Agora Event handler
    afx_msg LRESULT OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnEIDLeaveChannel(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnEIDUserJoined(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnEIDUserOffline(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnEIDMultipathStats(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnEIDNetworkTypeChanged(WPARAM wParam, LPARAM lParam);

private:
    //set control text from config.
    void InitCtrlText();
    //create all video window to save m_videoWnds.
    void CreateAllVideoWnds();
    //render local video from SDK local capture.
    void RenderLocalVideo();
    //stop local video capture from SDK
    void StopLocalVideo();
    void ResetVideoView();

    IRtcEngine* m_rtcEngine = nullptr;
    CMultipathRtcEngineEventHandler m_eventHandler;
    bool m_joinChannel = false;
    bool m_initialize = false;
    //video wnd
    CAGVideoWnd m_videoWnds[VIDEO_COUNT_MULTIPATH];
    std::list<uid_t> m_lstUids;
    int m_uid = 0;

public:
    CComboBox m_cmbRole;
    CStatic m_staRole;
    CEdit m_edtChannelName;
    CButton m_btnJoinChannel;
    CListBox m_lstInfo;
    CStatic m_videoArea;
    CStatic m_staChannelName;
    CComboBox m_cmbMultipathMode;
    CStatic m_staMultipathMode;
    CButton m_chkEnableMultipath;
    CStatic m_staEnableMultipath;
    
    virtual BOOL PreTranslateMessage(MSG* pMsg);

private:
    std::string m_multipathModeStr = "";
    std::string m_networkStr = "unknown";  
    int m_activePathNum = 0;
};