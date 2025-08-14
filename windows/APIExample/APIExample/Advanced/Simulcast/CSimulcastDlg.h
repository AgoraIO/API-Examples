#pragma once
#include "AGVideoWnd.h"
#include <list>
#include <string>

// CSimulcastDlg dialog

#define VIDEO_COUNT_SIMULCAST           2
#define IDC_BASEWND_VIDEO_SIMULCAST     300

class CSimulcastRtcEngineEventHandler
    : public IRtcEngineEventHandler
{
public:
    //set the message notify window handler
    void SetMsgReceiver(HWND hWnd) { m_hMsgHanlder = hWnd; }
    
    virtual void onJoinChannelSuccess(const char* channel, uid_t uid, int elapsed) override;
    virtual void onUserJoined(uid_t uid, int elapsed) override;
    virtual void onUserOffline(uid_t uid, USER_OFFLINE_REASON_TYPE reason) override;
    virtual void onLeaveChannel(const RtcStats& stats) override;
    virtual void onRemoteVideoStats(const RemoteVideoStats& stats) override;

private:
    HWND m_hMsgHanlder;
};

class CSimulcastDlg : public CDialogEx
{
    DECLARE_DYNAMIC(CSimulcastDlg)

public:
    CSimulcastDlg(CWnd* pParent = nullptr);   // standard constructor
    virtual ~CSimulcastDlg();

    //Initialize the Agora SDK
    bool InitAgora();
    //UnInitialize the Agora SDK
    void UnInitAgora();
    
    //resume window status
    void ResumeStatus();

// Dialog Data
    enum { IDD = IDD_DIALOG_SIMULCAST };
    
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnBnClickedButtonJoinchannel();
    afx_msg void OnSelchangeComboRole();
    afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
    
    // Layer checkbox handlers for broadcaster
    afx_msg void OnBnClickedCheckLayer1();
    afx_msg void OnBnClickedCheckLayer2();
    afx_msg void OnBnClickedCheckLayer3();
    afx_msg void OnBnClickedCheckLayer4();
    
    // Layer selection for audience
    afx_msg void OnSelchangeComboLayerSelect();
    
    //Agora Event handler
    afx_msg LRESULT OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnEIDLeaveChannel(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnEIDUserJoined(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnEIDUserOffline(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnEIDRemoteVideoStats(WPARAM wParam, LPARAM lParam);

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
    
    // Update UI based on role selection
    void UpdateUIForRole();
    // Get the number of checked layers
    int GetCheckedLayerCount();

    IRtcEngine* m_rtcEngine = nullptr;
    CSimulcastRtcEngineEventHandler m_eventHandler;
    bool m_joinChannel = false;
    bool m_initialize = false;
    //video wnd
    CAGVideoWnd m_videoWnds[VIDEO_COUNT_SIMULCAST];
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
    
    // Broadcaster layer checkboxes
    CButton m_chkLayer1;
    CButton m_chkLayer2;
    CButton m_chkLayer3;
    CButton m_chkLayer4;
    CStatic m_staLayerTitle;
    
    // Audience layer selection
    CComboBox m_cmbLayerSelect;
    CStatic m_staLayerSelect;
    
    virtual BOOL PreTranslateMessage(MSG* pMsg);

    void ConfigureSimulcast();
};