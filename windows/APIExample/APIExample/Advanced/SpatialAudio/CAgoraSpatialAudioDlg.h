﻿#pragma once
#include "AGVideoWnd.h"
#include <map>
#include <IAgoraSpatialAudio.h>
class CSpatialAudioEventHandler : public IRtcEngineEventHandler
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
		uid: user ID.If the UID is specified in the joinChannel, that ID is returned here;
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
		by the callback(ms).
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
	virtual void  onAudioMixingStateChanged(AUDIO_MIXING_STATE_TYPE state, AUDIO_MIXING_REASON_TYPE reason) override;
private:
	HWND m_hMsgHanlder;
};


class CAgoraSpatialAudioDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAgoraSpatialAudioDlg)

public:
	CAgoraSpatialAudioDlg(CWnd* pParent = nullptr);  
	virtual ~CAgoraSpatialAudioDlg();

	enum { IDD = IDD_DIALOG_SPATIAL_AUDIO};

public:
	//Initialize the Ctrl Text.
	void InitCtrlText();
	//Initialize the Agora SDK
	bool InitAgora();
	//UnInitialize the Agora SDK
	void UnInitAgora();

	//resume window status
	void ResumeStatus();

protected:
	virtual void DoDataExchange(CDataExchange* pDX); 
	DECLARE_MESSAGE_MAP()
	
	LRESULT OnEIDUserJoined(WPARAM wParam, LPARAM lParam);
	LRESULT OnEIDUserOffline(WPARAM wParam, LPARAM lParam);
	
	LRESULT OnLButtonDownVideo(WPARAM wParam, LPARAM lParam);
	LRESULT OnLButtonUpVideo(WPARAM wParam, LPARAM lParam);
	void SetSpatialAudioParam();
private:
	bool m_joinChannel = false;
	bool m_initialize = false;
	bool m_SpatialAudio = false;
	IRtcEngine* m_rtcEngine = nullptr;
	
	agora::util::AutoPtr<agora::rtc::ILocalSpatialAudioEngine> m_localSpatial;
	CAGVideoWnd m_localVideoWnd;
	CSpatialAudioEventHandler m_eventHandler;
	CStatic m_staLocal;
	CStatic m_staRemote;
	RECT rcLocal;
	RECT rcRemote;
	bool moveRemote = false;
	CPoint origin;
	RECT rcArea;
	int remoteWidth = 70;
	int localWidth = 70;
	UINT echoTestId = 10086;
	UINT disableSpatialId = 10087;
	unsigned int uid;

	float distanceRate = 1.0f;
	bool canmove = false;

	CImageList remoteImage;
	CImageList localImage;
public:
	CStatic m_staVideoArea;
	CListBox m_lstInfo;
	CStatic m_staDetail;

	CButton m_btnSetAudioMix;
	
	afx_msg void OnSelchangeListInfoBroadcasting();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CStatic m_staVolume;

	std::map<int, CString> m_mapState;
	std::map<int, CString> m_mapReason;
	CString m_audioPath;
	afx_msg void OnBnClickedButtonStart();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CButton m_btnStart;
};
