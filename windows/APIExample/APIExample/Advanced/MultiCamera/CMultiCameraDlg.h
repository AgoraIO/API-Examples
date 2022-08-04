#pragma once

#include "AGVideoWnd.h"
#include <map>
#include <string>
#include <IAgoraRtcEngineEx.h>

// CMultiCameraDlg 
typedef struct _CameraInfos {
    std::string deviceId;
    std::string deviceName;
    std::vector<agora::rtc::VideoFormat> videoFormats;
}CAMERAINFO, *PCAMERAINFO;

#define IDC_MULTICALERAM_VIDEO      10000

class CMultiCameraEventHandler : public agora::rtc::IRtcEngineEventHandler
{
public:
	//set the message notify window handler
	void SetMsgReceiver(HWND hWnd) { m_hMsgHanlder = hWnd; }

	int GetId() { return m_Id; };
	void SetId(int id) { m_Id = id; };

	std::string GetChannelName() { return m_strChannel; }
	/*
	note:
		Join the channel callback.This callback method indicates that the client
		successfully joined the specified channel.Channel ids are assigned based
		on the channel name specified in the joinChannel. If IRtcEngine::joinChannel
		is called without a user ID specified. The server will automatically assign one
	parameters:
		channel:channel name.
		uid: user ID。If the UID is specified in the joinChannel, that ID is returned here;
		Otherwise, use the ID automatically assigned by the Agora server.
		elapsed: The Time from the joinChannel until this event occurred (ms).
	*/
	virtual void onJoinChannelSuccess(const char* channel, agora::rtc::uid_t uid, int elapsed) override;
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
		by the callback(ms).
	*/
	virtual void onUserJoined(agora::rtc::uid_t uid, int elapsed) override;
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
	virtual void onUserOffline(agora::rtc::uid_t uid, agora::rtc::USER_OFFLINE_REASON_TYPE reason) override;
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
	virtual void onLeaveChannel(const agora::rtc::RtcStats& stats) override;
	
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
	virtual void onRemoteVideoStateChanged(agora::rtc::uid_t uid, agora::rtc::REMOTE_VIDEO_STATE state, agora::rtc::REMOTE_VIDEO_STATE_REASON reason, int elapsed) override;

	

private:
	HWND m_hMsgHanlder = NULL;
	std::string m_strChannel = "";
	int m_Id = 0;
};

class CMultiCameraDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMultiCameraDlg)

public:
	CMultiCameraDlg(CWnd* pParent = nullptr);   
	virtual ~CMultiCameraDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_MULTICAMERA };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV Support

	// Disconnect, reset connections. Broad caster has more function:
	// stop camera, remove local renders, unpublish video tracks, unpublish audio track,disbale audio track.
    void LeaveChannel();
	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();

private:
    void ShowVideoWnds();
	void InitCtrlText();
public:
    afx_msg void OnBnClickedButtonJoinchannel();
	LRESULT OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam);
	LRESULT OnEIDLeaveChannel(WPARAM wParam, LPARAM lParam);

	afx_msg void OnBnClickedButtonPublish2();
    
private:
    int m_maxVideoCount = 0;
    CAGVideoWnd m_videoWnds[MAX_VIDEO_COUNT];
    bool m_bConnected = false;

	bool m_joinChannel  = false;
	bool m_initialize   = false;
	bool m_bScecondJoin = false;
	bool m_bStartCapture1 = false;
	bool m_bStartCapture2 = false;
	std::string m_strChannel;

	agora::rtc::IRtcEngineEx* m_rtcEngine = nullptr;

	conn_id_t m_conn_camera2;
	conn_id_t m_conn_camera;
	std::vector<CAMERAINFO> m_vecCameraInfos;
	
	AVideoDeviceManager*  videoDeviceManager = nullptr;
	CMultiCameraEventHandler m_camera2EventHandler;
	CMultiCameraEventHandler m_cameraEventHandler;


	agora::rtc::RtcConnection connection;
public:
    //Initialize the Agora SDK
    bool InitAgora();
    //UnInitialize the Agora SDK
    void UnInitAgora();
	// resume window status.
	void ResumeStatus();
    CEdit m_edtChannelName;
    CStatic m_videoArea;
    CComboBox m_cmbCameras;
   
    CButton m_btnJoinChannel;
	CListBox m_lstInfo;
	CComboBox m_cmbCamera2;
	CButton m_btnPublish2;
	CStatic m_staCamera1;
	CStatic m_staCamera2;
	CStatic m_staChannel;
	afx_msg void OnBnClickedButtonCamera1();
	afx_msg void OnBnClickedButtonCamera2();
	CButton m_btnCapture1;
	CButton m_btnCapture2;
};
