#pragma once
#include "AGVideoWnd.h"
#include <map>
#include <IAgoraSpatialAudio.h>
#include <IAgoraMediaPlayer.h>
#include <IAgoraMediaPlayerSource.h>

class CSpatialAudioPlayerObserver : public IMediaPlayerSourceObserver {
public:
	

	virtual ~CSpatialAudioPlayerObserver() {
		m_player = nullptr;
	}

	void setMediaPlayer(IMediaPlayer* player) {
		m_player = player;
	}

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
	virtual void onPlayerSourceStateChanged( media::base::MEDIA_PLAYER_STATE state,
		media::base::MEDIA_PLAYER_REASON ec) {
		if (state == media::base::MEDIA_PLAYER_STATE::PLAYER_STATE_OPEN_COMPLETED) {
			if (m_player != nullptr) {
				m_player->setLoopCount(-1);
				m_player->play();
			}
		}
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
	IMediaPlayer *m_player;
};


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

struct CAgoraSpatialAudioConfig
{
	BOOL mute = FALSE;
	BOOL blur = FALSE;
	BOOL airborne = FALSE;
	float attenuation = 0.5f;
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

	void GetWindowsRelativeRect(CStatic& child, RECT* childRect);

	void CaculateObjectPosition(CStatic& child, float out[3]);

	void CaculateZoneRect(CStatic& zone, RECT* rect);

protected:
	virtual void DoDataExchange(CDataExchange* pDX); 
	DECLARE_MESSAGE_MAP()

private:
	bool m_joinChannel = false;
	bool m_initialize = false;
	bool m_SpatialAudio = false;
	IRtcEngine* m_rtcEngine = nullptr;
	
	ILocalSpatialAudioEngine* m_localSpatial = nullptr;
	agora_refptr<IMediaPlayer> m_mediaPlayerLeft;
	CSpatialAudioPlayerObserver playerLeftObserver;
	agora_refptr<IMediaPlayer> m_mediaPlayerRight;
	CSpatialAudioPlayerObserver playerRightObserver;

	CSpatialAudioEventHandler m_eventHandler;

	INT ICON_SIZE = 70;

	CStatic m_staChannelName;
	CEdit m_edtChannelName;
	CButton m_btnJoinChannel;

	CStatic m_staVideoArea;
	CListBox m_lstInfo;
	CStatic m_staDetail;

	CStatic m_staPlayerLeft;
	CStatic m_staPlayerRight;
	CStatic m_staRemoteLeft;
	CStatic m_staRemoteRight;
	CStatic m_staLocalMove;

	RECT rcPlayerLeft;
	RECT rcPlayerRight;
	RECT rcLocal;
	RECT rcRemoteLeft;
	RECT rcRemoteRight;

	CStatic m_staAudioSource;
	CComboBox m_comAudioSource;
	CButton m_chkAudioSourceMute;
	CButton m_chkAudioSourceBlur;
	CButton m_chkAudioSourceAir;
	CStatic m_staAudioSourceAttenuation;
	CSliderCtrl m_sldAudioSourceAttenuation;

	CStatic m_staZone;
	CButton m_chkZone;
	CButton m_chkLocalMute;

	CStatic m_staMoveTip;

	volatile bool moveLocal = false;
	CPoint origin;
	RECT rcArea;

	unsigned int uid;
	unsigned int remoteLeftUid;
	unsigned int remoteRightUid;
	bool canmove = false;

	const CString CONFIG_KEY_PLAYER_LEFT = _T("Left Media Player");
	const CString CONFIG_KEY_PLAYER_RIGHT = _T("Right Media Player");
	const CString CONFIG_KEY_REMOTE_LEFT = _T("Left Remote User");
	const CString CONFIG_KEY_REMOTE_RIGHT = _T("Right Remote User");
	std::map<CString, CAgoraSpatialAudioConfig> m_configMap;


	const float AXIS_MAX_DISTANCE = 10.0f;

public:
	
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	//Agora Event handler
	afx_msg LRESULT OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEIDLeaveChannel(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEIDUserJoined(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEIDUserOffline(WPARAM wParam, LPARAM lParam);
	
	afx_msg void OnSelchangeListInfoBroadcasting();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedButtonJoinchannel();

	afx_msg void OnCbnSelchangeComboAudioSource();
	afx_msg void OnBnClickedCheckAudioSourceMute();
	afx_msg void OnBnClickedCheckAudioSourceBlur();
	afx_msg void OnBnClickedCheckAudioSourceAirborne();
	afx_msg void OnNMCustomdrawSliderAudioSourceAttenuation(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedCheckAudioZone();
	afx_msg void OnBnClickedCheckMuteLocalAudio();
};
