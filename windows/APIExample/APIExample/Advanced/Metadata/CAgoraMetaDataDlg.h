#pragma once
#include "AGVideoWnd.h"

class CAgoraMetaDataObserver : public IMetadataObserver
{
public:
    void SetMsgReceiver(HWND hWnd) { m_hMsgHanlder = hWnd; }
    /*
        get max meta data size of byte.
    */
    virtual int getMaxMetadataSize()override;
    /*
        The sender is ready to send MetadataThis callback method
        is triggered when the SDK is ready to receive and send Metadata.
        annotations:
        Make sure that the Metadata size passed in to this method does not
        exceed the value set in getMaxMetadataSize.
        parameter: 
        metadata :Metadata that the user wants to send.
        return:
        True: send
        False: don't send
    */
	virtual bool onReadyToSendMetadata(Metadata &metadata, VIDEO_SOURCE_TYPE source_type)override;
    /*
        The receiver has received Metadata.The SDK triggers the callback when it
        receives Metadata sent by the remote user.
        parameter:
        metadata:Received Metadata.
    */
    virtual void onMetadataReceived(const Metadata &metadata)override;
    //set max meta data size.
    void SetMaxMetadataSize(int maxSize);
    //set send string.
    void SetSendSEI(std::string utf8Msg);
private:
    int m_maxSize = 1024;
    std::string m_sendSEI;
    HWND m_hMsgHanlder;
};


class CAgoraMetaDataEventHanlder : public IRtcEngineEventHandler
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
    parameters:
        stats: Call statistics.
    */
    virtual void onLeaveChannel(const RtcStats& stats) override;
    /** 
         Occurs when the remote video state changes.
         @note This callback does not work properly when the number of users (in the Communication profile) or broadcasters (in the Live-broadcast profile) in the channel exceeds 17.

         @param uid ID of the remote user whose video state changes.
         @param state State of the remote video. See #REMOTE_VIDEO_STATE.
         @param reason The reason of the remote video state change. See
         #REMOTE_VIDEO_STATE_REASON.
         @param elapsed Time elapsed (ms) from the local user calling the
         \ref agora::rtc::IRtcEngine::joinChannel "joinChannel" method until the
         SDK triggers this callback.
     */
    virtual void onRemoteVideoStateChanged(uid_t uid, REMOTE_VIDEO_STATE state, REMOTE_VIDEO_STATE_REASON reason, int elapsed) override;


    virtual void onAudioMetadataReceived(uid_t uid, const char* metadata, size_t length) override;

private:
    HWND m_hMsgHanlder;
};

// CAgoraMetaDataDlg dialog

class CAgoraMetaDataDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAgoraMetaDataDlg)

public:
	CAgoraMetaDataDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CAgoraMetaDataDlg();
    //Initialize the Agora SDK
    bool InitAgora();
    //UnInitialize the Agora SDK
    void UnInitAgora();
    //set control text from config.
    void InitCtrlText();
    //render local video from SDK local capture.
    void RenderLocalVideo();
	//resume window status.
	void ResumeStatus();

	enum { IDD = IDD_DIALOG_METADATA };

    //Agora Event handler
    afx_msg LRESULT OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnEIDLeaveChannel(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnEIDUserJoined(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnEIDUserOffline(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnEIDRemoteVideoStateChanged(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnEIDMetadataReceived(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnEIDAudioMetadataReceived(WPARAM wParam, LPARAM lParam);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
    CStatic m_staChannelName;
    CButton m_btnJoinChannel;
    CStatic m_staVideoMetadata;
    CEdit m_edtVideoMetadata;
    CStatic m_btnSendVideoMetadata;
    CStatic m_staAudioMetadata;
    CEdit m_edtAudioMetadata;
    CStatic m_btnSendAudioMetadata;
    CListBox m_lstInfo;
    afx_msg void OnBnClickedButtonJoinchannel();
    virtual BOOL OnInitDialog();

private:
    IRtcEngine* m_rtcEngine = nullptr;
    CAgoraMetaDataEventHanlder m_eventHandler;

    bool m_joinChannel    = false;
    bool m_initialize   = false;
    bool m_remoteJoined = false;
    CAGVideoWnd m_localVideoWnd;
    CAGVideoWnd m_remoteVideoWnd;

    CAgoraMetaDataObserver m_metaDataObserver;
public:
    CStatic m_staVideoArea;
    CEdit m_edtChannelName;
    afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
    afx_msg void OnBnClickedButtonSendVideoMetadata();
    afx_msg void OnBnClickedButtonSendAudioMetadata();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
