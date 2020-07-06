#pragma once
#include "../../AGVideoWnd.h"
#include <list>
// CLiveBroadcastingDlg dialog

#define VIDEO_COUNT                     36
#define IDC_BASEWND_VIDEO               113

class CLiveBroadcastingRtcEngineEventHandler
    : public IRtcEngineEventHandler
{
public:
    //set the message notify window handler
    void SetMsgReceiver(HWND hWnd) { m_hMsgHanlder = hWnd; }
    /*
    note:
        Join the channel callback.This callback method indicates that the client 
        successfully joined the specified channel.Channel ids are assigned based 
        on the channel name specified in the joinChannel. If IRtcEngine::joinChannel 
        is called without a user ID specified. The server will automatically assign one
    parameters:
        channel:channel name.
        uid: user ID¡£If the UID is specified in the joinChannel, that ID is returned here;
        Otherwise, use the ID automatically assigned by the Agora server.
        elapsed: The Time from the joinChannel until this event occurred (ms).
    */
    virtual void onJoinChannelSuccess(const char* channel, uid_t uid, int elapsed) override;
    /*
    note:
        In the live broadcast scene, each anchor can receive the callback
        of the new anchor joining the channel, and can obtain the uID of the anchor.
        Viewers also receive a callback when a new anchor joins the channel and 
        get the anchor's UID.When the Web side joins the live channel, the SDK will 
        default to the Web side as long as there is a push stream on the 
        Web side and trigger the callback.
    parameters:
        uid: remote user/anchor ID for newly added channel.
        elapsed: The joinChannel is called from the local user to the delay triggered 
        by the callback£¨ms).
    */
    virtual void onUserJoined(uid_t uid, int elapsed) override;
    /*
    note:
        Remote user (communication scenario)/anchor (live scenario) is called back from
        the current channel.A remote user/anchor has left the channel (or dropped the line).
        There are two reasons for users to leave the channel, namely normal departure and 
        time-out:When leaving normally, the remote user/anchor will send a message like
        "goodbye". After receiving this message, determine if the user left the channel.
        The basis of timeout dropout is that within a certain period of time 
        (live broadcast scene has a slight delay), if the user does not receive any
        packet from the other side, it will be judged as the other side dropout.
        False positives are possible when the network is poor. We recommend using the
        Agora Real-time messaging SDK for reliable drop detection.
    parameters:
        uid: The user ID of an offline user or anchor.
        reason:Offline reason: USER_OFFLINE_REASON_TYPE.
    */
    virtual void onUserOffline(uid_t uid, USER_OFFLINE_REASON_TYPE reason) override;
    /*
    note:
        When the App calls the leaveChannel method, the SDK indicates that the App 
        has successfully left the channel. In this callback method, the App can get
        the total call time, the data traffic sent and received by THE SDK and other
        information. The App obtains the call duration and data statistics received 
        or sent by the SDK through this callback.
    parametes:
        stats: Call statistics.
    */
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

    //Initialize the Agora SDK
    bool InitAgora();
    //UnInitialize the Agora SDK
    void UnInitAgora();
    
// Dialog Data
    enum { IDD = IDD_DIALOG_LIVEBROADCASTING };
    //The number of people supported within the channel
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
    //set control text from config.
    void InitCtrlText();
    //create all video window to save m_videoWnds.
    void CreateAllVideoWnds();
    //show all video window from m_videoWnds.
    void ShowVideoWnds();
    //render local video from SDK local capture.
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
    afx_msg void OnStnClickedStaticVideo();
};
