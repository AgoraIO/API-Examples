#pragma once
#include "AGVideoWnd.h"
#include <AgoraRtcChannelPublishHelper.h>

#define VIDEO_SLIDER_RANGE		10000

class AgoraMediaPlayerEvent : public AgoraRtcChannelPublishHelperObserver
{
public:
	// set notify window.
	void SetMsgReceiver(HWND hWnd) { m_hMsgHanlder = hWnd; }

	/**
	 * @brief Triggered when the player state changes
	 *
	 * @param state New player state
	 * @param ec Player error message
	 */
	virtual void onPlayerStateChanged(agora::media::base::MEDIA_PLAYER_STATE state,
		agora::media::base::MEDIA_PLAYER_ERROR ec)
	{
		
		::PostMessage(m_hMsgHanlder, WM_MSGID(mediaPLAYER_STATE_CHANGED), (WPARAM)state, (LPARAM) ec);
	}

	/**
	 * @brief Triggered when the player progress changes, once every 1 second
	 *
	 * @param position Current playback progress, in seconds
	 */
	virtual void onPositionChanged(const int64_t position)
	{
		::PostMessage(m_hMsgHanlder, WM_MSGID(mediaPLAYER_POSTION_CHANGED), (WPARAM)new int64_t(position), NULL);
	}
	/**
	 * @brief Triggered when the player have some event
	 *
	 * @param event
	 */
	virtual void onPlayerEvent(agora::media::base::MEDIA_PLAYER_EVENT event)
	{

	};

	/**
	 * @brief Triggered when metadata is obtained
	 *
	 * @param type Metadata type
	 * @param data data
	 * @param length  data length
	 */
	virtual void onMetadata(agora::media::base::MEDIA_PLAYER_METADATA_TYPE type, const uint8_t* data,
		uint32_t length)
	{

	}

	virtual void onPreloadEvent(const char* src, media::base::PLAYER_PRELOAD_EVENT event)override {

	}
	
	virtual void onPlayBufferUpdated(int64_t playCachedBuffer) override {

	}
	
	virtual void onCompleted()  override {

	}

private:
	HWND m_hMsgHanlder;
};

class CAgoraMediaPlayerHandler : public agora::rtc::IRtcEngineEventHandler
{
public:
	virtual ~CAgoraMediaPlayerHandler() {}

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
private:
	HWND m_hMsgHanlder;
};

// media player state
enum MEDIAPLAYERSTATE
{
	mediaPLAYER_READY,
	mediaPLAYER_OPEN,
	mediaPLAYER_PLAYING,
	mediaPLAYER_PAUSE,
	mediaPLAYER_STOP,
};


class CAgoraMediaPlayer : public CDialogEx
{
	DECLARE_DYNAMIC(CAgoraMediaPlayer)

public:
	CAgoraMediaPlayer(CWnd* pParent = nullptr);   
	virtual ~CAgoraMediaPlayer();

	enum { IDD = IDD_DIALOG_MEDIA_PLAYER };

public:
	//Initialize the Ctrl Text.
	void InitCtrlText();
	//Initialize media player.
	void InitMediaPlayerKit();
	//Uninitialized media player .
	void UnInitMediaPlayerKit();
	//Initialize the Agora SDK
	bool InitAgora();
	//UnInitialize the Agora SDK
	void UnInitAgora();
	//resume window status
	void ResumeStatus();
	
private:
	bool m_joinChannel = false;
	bool m_initialize = false;
	bool m_attach = false;
	bool m_publishVideo = false;
	bool m_publishAudio = false;

	IRtcEngine* m_rtcEngine = nullptr;
	CAGVideoWnd m_localVideoWnd;
	CAgoraMediaPlayerHandler m_eventHandler;
	AgoraMediaPlayerEvent	m_mediaPlayerEnvet;
	IMediaPlayer *m_mediaPlayer = nullptr;
	MEDIAPLAYERSTATE m_mediaPlayerState = mediaPLAYER_READY;
	AgoraRtcChannelPublishHelper m_rtcChannelPublishHelper;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);   
	LRESULT OnmediaPlayerStateChanged(WPARAM wParam, LPARAM lParam);
	LRESULT OnmediaPlayerPositionChanged(WPARAM wParam, LPARAM lParam);
	LRESULT OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam);
	LRESULT OnEIDLeaveChannel(WPARAM wParam, LPARAM lParam);
	LRESULT OnEIDUserJoined(WPARAM wParam, LPARAM lParam);
	LRESULT OnEIDUserOffline(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	CStatic m_staVideoArea;
	CListBox m_lstInfo;
	CStatic m_staDetail;
	CStatic m_staChannel;
	CEdit m_edtChannel;
	CButton m_btnJoinChannel;
	CStatic m_staVideoSource;
	CEdit m_edtVideoSource;
	CButton m_btnOpen;
	CButton m_btnStop;
	CButton m_btnPlay;
	CButton m_btnAttchPlayer;
	CButton m_btnPublishVideo;
	CButton m_btnPublishAudio;
	CSliderCtrl m_sldVideo;
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButtonJoinchannel();
	afx_msg void OnBnClickedButtonOpen();
	afx_msg void OnBnClickedButtonStop();
	afx_msg void OnBnClickedButtonPlay();
	afx_msg void OnBnClickedButtonAttach();
	afx_msg void OnBnClickedButtonPublishVideo();
	afx_msg void OnBnClickedButtonPublishAudio();
	afx_msg void OnSelchangeListInfoBroadcasting();
	afx_msg void OnDestroy();
	afx_msg void OnReleasedcaptureSliderVideo(NMHDR *pNMHDR, LRESULT *pResult);
};
