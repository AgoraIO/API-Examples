#pragma once
#include "AGVideoWnd.h"
#include <IAgoraMediaPlayer.h>
#include <IAgoraMediaPlayerSource.h>
#define VIDEO_SLIDER_RANGE		10000
using namespace agora;
using namespace agora::base;
using namespace agora::media::base;
class CMediaPlayerSourceObserver : public IMediaPlayerSourceObserver {
public:
	//set the message notify window handler
	void SetMsgReceiver(HWND hWnd) { m_hMsgHanlder = hWnd; }
	virtual ~CMediaPlayerSourceObserver() {}

	/**
	 * Reports the playback state change.
	 *
	 * When the state of the playback changes, the media player occurs this
	 * callback to report the new playback state and the reason or error for the
	 * change.
	 *
	 * @param state The new playback state after change. See
	 * \ref agora::media::base::MEDIA_PLAYER_STATE "MEDIA_PLAYER_STATE"
	 * @param ec The player's error code. See
	 * \ref agora::media::base::MEDIA_PLAYER_ERROR "MEDIA_PLAYER_ERROR"
	 */
	virtual void onPlayerSourceStateChanged(media::base::MEDIA_PLAYER_STATE state,
		media::base::MEDIA_PLAYER_REASON ec) {
		::PostMessage(m_hMsgHanlder, WM_MSGID(mediaPLAYER_STATE_CHANGED), (WPARAM)state, (LPARAM)ec);
	}

	/**
	 * Reports current playback progress.
	 *
	 * The callback occurs once every one second during the playback and reports
	 * current playback progress.
	 *
	 * @param position Current playback progress (ms).
	 */
	virtual void onPositionChanged(int64_t positionMs, int64_t timestampMs)
	{
		::PostMessage(m_hMsgHanlder, WM_MSGID(mediaPLAYER_POSTION_CHANGED), (WPARAM)new int64_t(positionMs), NULL);
	}
	/**
	 * Reports the playback event.
	 *
	 * - After calling the \ref agora::rtc::IMediaPlayer::seek "seek" method,
	 * the media player occurs the callback to report the results of the seek
	 * operation.
	 * - After calling the
	 * \ref agora::rtc::IMediaPlayer::selectAudioTrack "selectAudioTrack" method,
	 * the media player occurs the callback to report that the audio track
	 * changes.
	 *
	 * @param event The playback event. See
	 *  \ref agora::media::base::MEDIA_PLAYER_EVENT "MEDIA_PLAYER_EVENT"
	 * for details.
	 */
	virtual void onPlayerEvent(media::base::MEDIA_PLAYER_EVENT event, int64_t elapsedTime, const char* message) override
	{
		::PostMessage(m_hMsgHanlder, WM_MSGID(mediaPLAYER_EVENT), (WPARAM)event, NULL);
	}

	/**
	 * Reports the reception of the media metadata.
	 *
	 * The callback occurs when the player receivers the media metadata and
	 * reports the detailed information of the media metadata.
	 *
	 * @param data The detailed data of the media metadata.
	 * @param length The data length (bytes).
	 */
	virtual void onMetaData(const void* data, int length)
	{

	}


	/**
	 * @brief Triggered when play buffer updated, once every 1 second
	 *
	 * @param int cached buffer during playing, in milliseconds
	 */
	virtual void onPlayBufferUpdated(int64_t playCachedBuffer)
	{

	}

	/**
	 * Occurs when one playback of the media file is completed.
	 */
	virtual void onCompleted()
	{

	}

	virtual void onPreloadEvent(const char* src, media::base::PLAYER_PRELOAD_EVENT event) override {

	}

	virtual void onAgoraCDNTokenWillExpire() override {

	}

	virtual void onPlayerSrcInfoChanged(const media::base::SrcInfo& from, const media::base::SrcInfo& to) override {

	}

	virtual void onPlayerInfoUpdated(const media::base::PlayerUpdatedInfo& info)override {

	}

	virtual void onAudioVolumeIndication(int volume)override {

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
	bool m_publishMeidaplayer = false;
	bool m_isVideoSliderCapturing = false;
	std::vector<int> audioStreams;

	IRtcEngine* m_rtcEngine = nullptr;
	CAGVideoWnd m_localVideoWnd;
	CAgoraMediaPlayerHandler m_eventHandler;
	CMediaPlayerSourceObserver	m_mediaPlayerEvent;
	agora_refptr<IMediaPlayer> m_mediaPlayer = nullptr;
	MEDIAPLAYERSTATE m_mediaPlayerState = mediaPLAYER_READY;
	//AgoraRtcChannelPublishHelper m_rtcChannelPublishHelper;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);   
	LRESULT OnmediaPlayerStateChanged(WPARAM wParam, LPARAM lParam);
	LRESULT OnmediaPlayerPositionChanged(WPARAM wParam, LPARAM lParam);
	LRESULT OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam);
	LRESULT OnEIDLeaveChannel(WPARAM wParam, LPARAM lParam);
	LRESULT OnEIDUserJoined(WPARAM wParam, LPARAM lParam);
	LRESULT OnEIDUserOffline(WPARAM wParam, LPARAM lParam);
	LRESULT OnEIDPlayerEvent(WPARAM wParam, LPARAM lParam);
	
	DECLARE_MESSAGE_MAP()
public:
	CStatic m_staVideoArea;
	CListBox m_lstInfo;
	CStatic m_staDetail;
	CStatic m_staChannel;
	CStatic m_staPublishStream;
	CStatic m_staPlayerStream;
	CComboBox m_cmbPublishStream;
	CComboBox m_cmbPlayerStream;
	CEdit m_edtChannel;
	CButton m_btnJoinChannel;
	CStatic m_staVideoSource;
	CEdit m_edtVideoSource;
	CButton m_btnOpen;
	CButton m_btnStop;
	CButton m_btnPlay;
	
	CButton m_btnPublishVideo;
	
	CSliderCtrl m_sldVideo;
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButtonJoinchannel();
	afx_msg void OnBnClickedButtonOpen();
	afx_msg void OnBnClickedButtonStop();
	afx_msg void OnBnClickedButtonPlay();
	void ToggleMediaPlayerPublish();
	afx_msg void OnBnClickedButtonPublishVideo();
	
	afx_msg void OnSelchangeListInfoBroadcasting();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSelchangeComboPublishOrPlayerStream();
};
