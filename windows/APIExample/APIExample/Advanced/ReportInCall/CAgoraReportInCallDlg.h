#pragma once
#include "AGVideoWnd.h"
#include <map>


class CAgoraReportInCallHandler : public IRtcEngineEventHandler
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
	virtual void onJoinChannelSuccess(const char* channel, uid_t uid, int elapsed) override
	{
		if (m_hMsgHanlder) {
			::PostMessage(m_hMsgHanlder, WM_MSGID(EID_JOINCHANNEL_SUCCESS), (WPARAM)uid, (LPARAM)elapsed);
		}
	}
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
	virtual void onUserJoined(uid_t uid, int elapsed) override
	{
		if (m_hMsgHanlder) {
			::PostMessage(m_hMsgHanlder, WM_MSGID(EID_USER_JOINED), (WPARAM)uid, (LPARAM)elapsed);
		}
	}
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
	virtual void onUserOffline(uid_t uid, USER_OFFLINE_REASON_TYPE reason) override
	{
		if (m_hMsgHanlder) {
			::PostMessage(m_hMsgHanlder, WM_MSGID(EID_USER_OFFLINE), (WPARAM)uid, (LPARAM)reason);
		}
	}
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
	virtual void onLeaveChannel(const RtcStats& stats) override
	{
		if (m_hMsgHanlder) {
			::PostMessage(m_hMsgHanlder, WM_MSGID(EID_LEAVE_CHANNEL), 0, 0);
		}

	}
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
	virtual void onRemoteVideoStateChanged(uid_t uid, REMOTE_VIDEO_STATE state, REMOTE_VIDEO_STATE_REASON reason, int elapsed) override
	{
		if (m_hMsgHanlder) {
			PVideoStateStateChanged stateChanged = new VideoStateStateChanged;
			stateChanged->uid = uid;
			stateChanged->reason = reason;
			stateChanged->state = state;
			::PostMessage(m_hMsgHanlder, WM_MSGID(EID_REMOTE_VIDEO_STATE_CHANGED), (WPARAM)stateChanged, 0);
		}
	}

	/** 
	Reports the last mile network quality of each user in the channel once every two seconds.
	 Last mile refers to the connection between the local device and Agora's edge server. This callback reports 
	 once every two seconds the last mile network conditions of each user in the channel. If a channel includes 
	 multiple users, the SDK triggers this callback as many times.
	 @param uid User ID. The network quality of the user with this @p uid is reported. If @p uid is 0, 
	 the local network quality is reported.
	 @param txQuality Uplink transmission quality rating of the user in terms of the transmission bitrate,
	 packet loss rate, average RTT (Round-Trip Time), and jitter of the uplink network. 
	 @p txQuality is a quality rating helping you understand how well the current uplink network conditions 
	 can support the selected VideoEncoderConfiguration. For example, a 1000 Kbps uplink network may be adequate 
	 for video frames with a resolution of 640 * 480 and a frame rate of 15 fps in the `LIVE_BROADCASTING` profile,
	 but may be inadequate for resolutions higher than 1280 * 720. See #QUALITY_TYPE.
	 @param rxQuality Downlink network quality rating of the user in terms of the packet loss rate, average RTT, 
	 and jitter of the downlink network. See #QUALITY_TYPE.
	 */
	virtual void onNetworkQuality(uid_t uid, int txQuality, int rxQuality)override {
		;
	}

	/** 
		Reports the statistics of the current call.
		The SDK triggers this callback once every two seconds after the user joins the channel.
		@param stats Statistics of the IRtcEngine: RtcStats.
	*/
	virtual void onRtcStats(const RtcStats& stats)override {
		if (m_hMsgHanlder)
			::PostMessage(m_hMsgHanlder, WM_MSGID(EID_RTC_STATS),(WPARAM)new RtcStats(stats), 0);
	}

	/** 
		 Reports the statistics of the local audio stream.
		 The SDK triggers this callback once every two seconds.
		 @param stats The statistics of the local audio stream.
		 See LocalAudioStats.
	 */
	virtual void onLocalAudioStats(const LocalAudioStats& stats) override {
		(void)stats;
	}

	/** Occurs when the local audio state changes.
	 * This callback indicates the state change of the local audio stream,
	 * including the state of the audio recording and encoding, and allows
	 * you to troubleshoot issues when exceptions occur.
	 *
	 * @note
	 * When the state is #LOCAL_AUDIO_STREAM_STATE_FAILED (3), see the `error`
	 * parameter for details.
	 *
	 * @param state State of the local audio. See #LOCAL_AUDIO_STREAM_STATE.
	 * @param error The error information of the local audio.
	 * See #LOCAL_AUDIO_STREAM_ERROR.
	 */
	virtual void onLocalAudioStateChanged(LOCAL_AUDIO_STREAM_STATE state, LOCAL_AUDIO_STREAM_REASON error) override {
		(void)state;
		(void)error;
	}

	/** 
		Reports the statistics of the audio stream from each remote user/host.
		This callback replaces the \ref agora::rtc::IRtcEngineEventHandler::onAudioQuality "onAudioQuality" callback.
		The SDK triggers this callback once every two seconds for each remote user/host. If a channel includes multiple remote users, the SDK triggers this callback as many times.
		@param stats Pointer to the statistics of the received remote audio streams. See RemoteAudioStats.
	 */
	virtual void onRemoteAudioStats(const RemoteAudioStats& stats)override {
		if (m_hMsgHanlder)
			::PostMessage(m_hMsgHanlder, WM_MSGID(EID_REMOTE_AUDIO_STATS), (WPARAM)new RemoteAudioStats(stats), 0);
	}


	/** Occurs when the remote audio state changes.

	 This callback indicates the state change of the remote audio stream.
	 @note This callback does not work properly when the number of users (in the `COMMUNICATION` profile) or hosts (in the `LIVE_BROADCASTING` profile) in the channel exceeds 17.

	 @param uid ID of the remote user whose audio state changes.
	 @param state State of the remote audio. See #REMOTE_AUDIO_STATE.
	 @param reason The reason of the remote audio state change.
	 See #REMOTE_AUDIO_STATE_REASON.
	 @param elapsed Time elapsed (ms) from the local user calling the
	 \ref IRtcEngine::joinChannel "joinChannel" method until the SDK
	 triggers this callback.
	 */
	virtual void onRemoteAudioStateChanged(uid_t uid, REMOTE_AUDIO_STATE state, REMOTE_AUDIO_STATE_REASON reason, int elapsed)override {
		(void)uid;
		(void)state;
		(void)reason;
		(void)elapsed;
	}

	/** Reports the statistics of the local video stream.
	 *
	 * The SDK triggers this callback once every two seconds for each
	 * user/host. If there are multiple users/hosts in the channel, the SDK
	 * triggers this callback as many times.
	 *
	 * @note
	 * If you have called the
	 * \ref agora::rtc::IRtcEngine::enableDualStreamMode "enableDualStreamMode"
	 * method, the \ref onLocalVideoStats() "onLocalVideoStats" callback
	 * reports the statistics of the high-video
	 * stream (high bitrate, and high-resolution video stream).
	 *
	 * @param stats Statistics of the local video stream. See LocalVideoStats.
	 */
	virtual void onLocalVideoStats(VIDEO_SOURCE_TYPE source, const LocalVideoStats& stats) override {
		if (m_hMsgHanlder)
			::PostMessage(m_hMsgHanlder, WM_MSGID(EID_LOCAL_VIDEO_STATS), (WPARAM)new LocalVideoStats(stats), 0);
	}

	/** Occurs when the local video stream state changes.
	 This callback indicates the state of the local video stream, including camera capturing and video encoding, and allows you to troubleshoot issues when exceptions occur.
	 @note For some device models, the SDK will not trigger this callback when the state of the local video changes while the local video capturing device is in use, so you have to make your own timeout judgment.
	 @param localVideoState State type #LOCAL_VIDEO_STREAM_STATE. When the state is LOCAL_VIDEO_STREAM_STATE_FAILED (3), see the `error` parameter for details.
	 @param error The detailed error information: #LOCAL_VIDEO_STREAM_ERROR.
	 */
	virtual void onLocalVideoStateChanged(VIDEO_SOURCE_TYPE source, LOCAL_VIDEO_STREAM_STATE localVideoState, LOCAL_VIDEO_STREAM_REASON error)override {
		(void)localVideoState;
		(void)error;
	}

	/** Reports the statistics of the video stream from each remote user/host.
	*
	* The SDK triggers this callback once every two seconds for each remote
	* user/host. If a channel includes multiple remote users, the SDK
	* triggers this callback as many times.
	*
	* @param stats Statistics of the remote video stream. See
	* RemoteVideoStats.
	*/
	virtual void onRemoteVideoStats(const RemoteVideoStats& stats)override {
		if (m_hMsgHanlder)
			::PostMessage(m_hMsgHanlder, WM_MSGID(EID_REMOTE_VIDEO_STATS), (WPARAM)new RemoteVideoStats(stats), 0);
	}

private:
	HWND m_hMsgHanlder;
};


class CAgoraReportInCallDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAgoraReportInCallDlg)

public:
	CAgoraReportInCallDlg(CWnd* pParent = nullptr);   
	virtual ~CAgoraReportInCallDlg();

	enum { IDD = IDD_DIALOG_PEPORT_IN_CALL };
public:
	//Initialize the Ctrl Text.
	void InitCtrlText();
	//Initialize the Agora SDK
	bool InitAgora();
	//UnInitialize the Agora SDK
	void UnInitAgora();
	//render local video from SDK local capture.
	void RenderLocalVideo();
	//resume window status
	void ResumeStatus();

private:
	bool m_joinChannel = false;
	bool m_initialize = false;
	bool m_setEncrypt = false;
	IRtcEngine* m_rtcEngine = nullptr;
	CAGVideoWnd m_localVideoWnd;
	CAGVideoWnd m_remoteVideoWnd;

	CAgoraReportInCallHandler m_eventHandler;

	RemoteVideoStats m_remoteVideStats;
	RemoteAudioStats m_remoteAudioStats;
	


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    
	DECLARE_MESSAGE_MAP()
	// agora sdk message window handler
	LRESULT OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam);
	LRESULT OnEIDLeaveChannel(WPARAM wParam, LPARAM lParam);
	LRESULT OnEIDUserJoined(WPARAM wParam, LPARAM lParam);
	LRESULT OnEIDUserOffline(WPARAM wParam, LPARAM lParam);
	LRESULT OnEIDRemoteVideoStateChanged(WPARAM wParam, LPARAM lParam);
	LRESULT OnEIDRemoteVideoStats(WPARAM wParam, LPARAM lParam);
	LRESULT OnEIDRemoteAudioStats(WPARAM wParam, LPARAM lParam);
	LRESULT OnEIDRtcStats(WPARAM wParam, LPARAM lParam);
	LRESULT OnEIDLocalVideoStats(WPARAM wParam, LPARAM lParam);



public:
	CStatic m_staChannel;
	CEdit m_edtChannel;
	CButton m_btnJoinChannel;
	CStatic m_staVideoArea;
	CStatic m_gopNetWorkTotal;
	CStatic m_gopAudioRemote;
	CStatic m_gopVideoRemote;
	CStatic m_staUpDownLinkVal;
	CStatic m_staTotalBytes;
	CStatic m_staTotalBytesVal;
	CStatic m_staTotalBitrate;
	CStatic m_staTotalBitrateVal;
	CStatic m_staAudioNetWorkDelay;
	CStatic m_staAudioNetWorkDelayVal;
	CStatic m_staAudioRecvBitrate;
	CStatic m_staAudioRecvBitrateVal;
	CStatic m_staVideoNetWorkDelay;
	CStatic m_staVideoNetWorkDelayVal;
	CStatic m_staVideoRecvBitrate;
	CStatic m_staVideoRecvBitrateVal;
	CStatic m_staLocalVideoResoultion;
	CStatic m_staLocalVideoResoultionVal;
	CStatic m_staLocalVideoFPS;
	CStatic m_staLocalVideoFPSVal;
	CStatic m_staDetails;
	CListBox m_lstInfo;
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButtonJoinchannel();
	afx_msg void OnSelchangeListInfoBroadcasting();

};
