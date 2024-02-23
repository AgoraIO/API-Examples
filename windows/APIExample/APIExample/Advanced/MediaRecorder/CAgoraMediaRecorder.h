#pragma once
#include "AGVideoWnd.h"
#include <IAgoraRtcEngineEx.h>
#include <IAgoraMediaRecorder.h>

class CAgoraMediaRecorderEventHandler : public IRtcEngineEventHandler 
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
    void onJoinChannelSuccess(const char* channel, uid_t uid, int elapsed) {
		if (m_hMsgHanlder) {
			::PostMessage(m_hMsgHanlder, WM_MSGID(EID_JOINCHANNEL_SUCCESS), (WPARAM)uid, (LPARAM)elapsed);
		}
    };
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
    void onUserJoined(uid_t uid, int elapsed)
    {
		if (m_hMsgHanlder) {
			::PostMessage(m_hMsgHanlder, WM_MSGID(EID_USER_JOINED), (WPARAM)uid, (LPARAM)elapsed);
		}
    };
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
    void onUserOffline(uid_t uid, USER_OFFLINE_REASON_TYPE reason)
    {
		if (m_hMsgHanlder) {
			::PostMessage(m_hMsgHanlder, WM_MSGID(EID_USER_OFFLINE), (WPARAM)uid, (LPARAM)reason);
		}
    };
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
    void onLeaveChannel(const RtcStats& stats) 
    {
		if (m_hMsgHanlder) {
			::PostMessage(m_hMsgHanlder, WM_MSGID(EID_LEAVE_CHANNEL), 0, 0);
		}
    };

private:
    HWND m_hMsgHanlder;
};


class CAgoraMediaRecorderObserver : public IMediaRecorderObserver
{
	

public:
	//set the message notify window handler
	void SetMsgReceiver(HWND hWnd) { m_hMsgHanlder = hWnd; }

    void onRecorderStateChanged(const char* channelId, rtc::uid_t uid, RecorderState state, RecorderReasonCode error) {
		if (m_hMsgHanlder) {
			::PostMessage(m_hMsgHanlder, WM_MSGID(EID_RECORDER_STATE_CHANGE), (WPARAM)state, (LPARAM)error);
		}
	};


    void onRecorderInfoUpdated(const char* channelId, rtc::uid_t uid, const RecorderInfo& info) {
		if (m_hMsgHanlder) {
            std::string* filePath = new std::string(info.fileName);
			::PostMessage(m_hMsgHanlder, WM_MSGID(EID_RECORDER_INFO_UPDATE), (WPARAM)uid, (LPARAM)filePath);
		}
	};
private:
	HWND m_hMsgHanlder;
};

class CAgoraMediaRecorder : public CDialogEx
{
	DECLARE_DYNAMIC(CAgoraMediaRecorder)

public:
	CAgoraMediaRecorder(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CAgoraMediaRecorder();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	//Initialize the Agora SDK
	bool InitAgora();
	//UnInitialize the Agora SDK
	void UnInitAgora();
	//set control text from config.
	void InitCtrlText();
	//resume window status.
	void ResumeStatus();
	//render local video from SDK local capture.
	void RenderLocalVideo();
	

	enum { IDD = IDD_DIALOG_MEDIA_RECORDER };

	//Agora Event handler
	afx_msg LRESULT OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEIDLeaveChannel(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEIDUserJoined(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEIDUserOffline(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEIDRecorderStateChanged(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEIDRecorderInfoUpdated(WPARAM wParam, LPARAM lParam);

	afx_msg void OnBnClickedButtonJoinchannel();
	afx_msg void OnBnClickedButtonLocalRecorder();
	afx_msg void OnBnClickedButtonRemoteRecorder();
	afx_msg void OnSelchangeListInfo();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	CStatic m_staChannelName;
	CButton m_btnJoinChannel;
	CListBox m_lstInfo;
	CStatic m_staVideoArea;
	CEdit m_edtChannelName;
    CEdit m_edtDetailInfo;

	IRtcEngine* m_rtcEngine = nullptr;
    CAgoraMediaRecorderEventHandler m_eventHandler;

	bool m_joinChannel = false;
	bool m_initialize = false;
	bool m_remoteJoined = false;
    std::string m_channelName;
    int m_uid;
	CAGVideoWnd m_localVideoWnd;
    CButton m_btnLocalRecord;
	CAGVideoWnd m_remoteVideoWnd;
    CButton m_btnRemoteRecord;

    IMediaRecorder *m_localRecorder = nullptr;
    CAgoraMediaRecorderObserver m_localRecorderObserver;
    IMediaRecorder *m_remoteRecorder = nullptr;
    CAgoraMediaRecorderObserver m_remoteRecorderObserver;

};