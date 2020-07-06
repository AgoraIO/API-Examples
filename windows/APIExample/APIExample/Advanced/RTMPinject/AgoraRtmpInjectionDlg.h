#pragma once
#include <string>
#include "../../AGVideoWnd.h"

class CAgoraRtmpInjectionRtcEngineEventHandler
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
        uid: user ID��If the UID is specified in the joinChannel, that ID is returned here;
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
        by the callback��ms).
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
    parameters:
        stats: Call statistics.
    */
    virtual void onLeaveChannel(const RtcStats& stats) override;
    /*
        Enter the online media stream status callback.This callback indicates the state
        of the external video stream being input to the live stream.
    parameters:
        url:Enter the URL address of the external video source into the live stream
        uid:user id.
        status:
        Input state of external video source:
        INJECT_STREAM_STATUS_START_SUCCESS(0):External video stream input successful
        INJECT_STREAM_STATUS_START_ALREADY_EXIST(1): External video stream already exists.
        INJECT_STREAM_STATUS_START_UNAUTHORIZED(2): The external video stream input is unauthorized
        INJECT_STREAM_STATUS_START_TIMEDOUT(3): Input external video stream timeout
        INJECT_STREAM_STATUS_START_FAILED(4) : External video stream input failed
        INJECT_STREAM_STATUS_STOP_SUCCESS(5) : INJECT_STREAM_STATUS_STOP_SUCCESS: External video stream stop input successful
        INJECT_STREAM_STATUS_STOP_NOT_FOUND (6): No external video stream to stop input
        INJECT_STREAM_STATUS_STOP_UNAUTHORIZED(7): The input to an external video stream is UNAUTHORIZED
        INJECT_STREAM_STATUS_STOP_TIMEDOUT(8) : Stopped input external video stream timeout
        INJECT_STREAM_STATUS_STOP_FAILED(9) : Failed to stop input external video stream
        INJECT_STREAM_STATUS_BROKEN(10) : Input external video stream has been broken
    */
    virtual void onStreamInjectedStatus(const char* url, uid_t uid, int status) override;

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

public:
    //Initialize the Agora SDK
    bool InitAgora();
    //UnInitialize the Agora SDK
    void UnInitAgora();
    //set control text from config.
    void InitCtrlText();
    //render local video from SDK local capture.
    void RenderLocalVideo();
private:
    
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
