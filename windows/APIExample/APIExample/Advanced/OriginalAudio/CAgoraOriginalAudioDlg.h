#pragma once
#include "AGVideoWnd.h"

class CAudioEncodedFrameObserver : public agora::rtc::IAudioEncodedFrameObserver
{
public:
	/**
	* Occurs each time the SDK receives an encoded recorded audio frame.
	* @param frameBufferThe pointer to the audio frame buffer.
	* @param length The data length of the audio frame.
	* @param audioEncodedFrameInfoThe information of the encoded audio frame: EncodedAudioFrameInfo.

	*/
	virtual void onRecordAudioEncodedFrame(const uint8_t* frameBuffer, int length, const EncodedAudioFrameInfo& audioEncodedFrameInfo) override;

	/**
	* Occurs each time the SDK receives an encoded playback audio frame.
	* @param frameBufferThe pointer to the audio frame buffer.
	* @param length The data length of the audio frame.
	* @param audioEncodedFrameInfoThe information of the encoded audio frame: EncodedAudioFrameInfo.

	*/
	virtual void onPlaybackAudioEncodedFrame(const uint8_t* frameBuffer, int length, const EncodedAudioFrameInfo& audioEncodedFrameInfo) override;

	/**
	* Occurs each time the SDK receives an encoded mixed audio frame.
	* @param frameBufferThe pointer to the audio frame buffer.
	* @param length The data length of the audio frame.
	* @param audioEncodedFrameInfoThe information of the encoded audio frame: EncodedAudioFrameInfo.

	*/
	virtual void onMixedAudioEncodedFrame(const uint8_t* frameBuffer, int length, const EncodedAudioFrameInfo& audioEncodedFrameInfo) override;

};
class COriginalAudioProcFrameObserver :
	public agora::media::IAudioFrameObserver
{
public:


	/*
	*	According to the setting of audio collection frame rate,
	*	the Agora SDK calls this callback function at an appropriate time
	*	to obtain the audio data collected by the user.
	*/
	virtual bool onRecordAudioFrame(const char* channelId, AudioFrame& audioFrame)override;
	/*
		Get the sound played.
		parameter:
		audioFrame:Audio naked data.
		See: AudioFrame
		return
		True: Buffer data in AudioFrame is valid, the data will be sent;
		False: The buffer data in the AudioFrame is invalid and will be discarded.
	*/
	virtual bool onPlaybackAudioFrame(const char* channelId, AudioFrame& audioFrame)override;
	/*
		Gets the data after recording and playing the voice mix.
		annotations:
			This method returns only single-channel data.
		parameter:
		audioFrame Audio naked data. See: AudioFrame
		return:
		True: Buffer data in AudioFrame is valid, the data will be sent;
		False: The buffer data in the AudioFrame is invalid and will be discarded.
	*/
	virtual bool onMixedAudioFrame(const char* channelId, AudioFrame& audioFrame)override;
	/*
		Gets the specified user's voice before the mix.
		parameter:
		uid: Specifies the user ID of the user.
		audioFrame: Audio naked data. See: AudioFrame.
		return:
		True: Buffer data in AudioFrame is valid, the data will be sent;
		False: The buffer data in the AudioFrame is invalid and will be discarded.
	*/
	virtual bool onPlaybackAudioFrameBeforeMixing(const char* channelId, media::base::user_id_t uid, AudioFrame& audioFrame)override;
	virtual bool onPlaybackAudioFrameBeforeMixing(const char* channelId, rtc::uid_t uid, AudioFrame& audioFrame) override;
	/**
   * Sets the frame position for the audio observer.
   * @return A bit mask that controls the frame position of the audio observer.
   * @note - Use '|' (the OR operator) to observe multiple frame positions.
   * <p>
   * After you successfully register the audio observer, the SDK triggers this callback each time it receives a audio frame. You can determine which position to observe by setting the return value.
   * The SDK provides 4 positions for observer. Each position corresponds to a callback function:
   * - `AUDIO_FRAME_POSITION_PLAYBACK (1 << 0)`: The position for playback audio frame is received, which corresponds to the \ref onPlaybackFrame "onPlaybackFrame" callback.
   * - `AUDIO_FRAME_POSITION_RECORD (1 << 1)`: The position for record audio frame is received, which corresponds to the \ref onRecordFrame "onRecordFrame" callback.
   * - `AUDIO_FRAME_POSITION_MIXED (1 << 2)`: The position for mixed audio frame is received, which corresponds to the \ref onMixedFrame "onMixedFrame" callback.
   * - `AUDIO_FRAME_POSITION_BEFORE_MIXING (1 << 3)`: The position for playback audio frame before mixing is received, which corresponds to the \ref onPlaybackFrameBeforeMixing "onPlaybackFrameBeforeMixing" callback.
   *  @return The bit mask that controls the audio observation positions.
   * See AUDIO_FRAME_POSITION.
   */

	virtual int getObservedAudioFramePosition() override;

	/** Sets the audio playback format
	 **Note**:

	 - The SDK calculates the sample interval according to the `AudioParams`
	 you set in the return value of this callback and triggers the
	 `onPlaybackAudioFrame` callback at the calculated sample interval.
	 Sample interval (seconds) = `samplesPerCall`/(`sampleRate` × `channel`).
	 Ensure that the value of sample interval is equal to or greater than 0.01.

	 @return Sets the audio format. See AgoraAudioParams.
	 */
	virtual AudioParams getPlaybackAudioParams() override;

	/** Sets the audio recording format
	 **Note**:
	 - The SDK calculates the sample interval according to the `AudioParams`
	 you set in the return value of this callback and triggers the
	 `onRecordAudioFrame` callback at the calculated sample interval.
	 Sample interval (seconds) = `samplesPerCall`/(`sampleRate` × `channel`).
	 Ensure that the value of sample interval is equal to or greater than 0.01.

	 @return Sets the audio format. See AgoraAudioParams.
	 */
	virtual AudioParams getRecordAudioParams() override;

	/** Sets the audio mixing format
	 **Note**:
	 - The SDK calculates the sample interval according to the `AudioParams`
	 you set in the return value of this callback and triggers the
	 `onMixedAudioFrame` callback at the calculated sample interval.
	 Sample interval (seconds) = `samplesPerCall`/(`sampleRate` × `channel`).
	 Ensure that the value of sample interval is equal to or greater than 0.01.

	 @return Sets the audio format. See AgoraAudioParams.
	 */
	virtual AudioParams getMixedAudioParams() override;

	bool onEarMonitoringAudioFrame(AudioFrame& audioFrame) override;


	AudioParams getEarMonitoringAudioParams() override;

};


class COriginalAudioEventHandler : public IRtcEngineEventHandler
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
		uid: user ID。If the UID is specified in the joinChannel, that ID is returned here;
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
		by the callback（ms).
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
private:
	HWND m_hMsgHanlder;
};



class CAgoraOriginalAudioDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAgoraOriginalAudioDlg)

public:
	CAgoraOriginalAudioDlg(CWnd* pParent = nullptr);
	virtual ~CAgoraOriginalAudioDlg();

	enum {
		IDD = IDD_DIALOG_ORIGINAL_AUDIO_
	};
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
	//register or unregister audio frame observer.
	BOOL RegisterAudioFrameObserver(BOOL bEnable, IAudioFrameObserver *audioFrameObserver=NULL);

private:
	bool m_joinChannel = false;
	bool m_initialize = false;
	bool m_setAudioProc = false;
	IRtcEngine* m_rtcEngine = nullptr;
	CAGVideoWnd m_localVideoWnd;
	COriginalAudioEventHandler m_eventHandler;
	COriginalAudioProcFrameObserver m_originalAudioProcFrameObserver;
	std::map<CString, IAudioFrameObserver *> m_mapAudioFrame;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	LRESULT OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam);
	LRESULT OnEIDLeaveChannel(WPARAM wParam, LPARAM lParam);
	LRESULT OnEIDUserJoined(WPARAM wParam, LPARAM lParam);
	LRESULT OnEIDUserOffline(WPARAM wParam, LPARAM lParam);
	LRESULT OnEIDRemoteVideoStateChanged(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButtonJoinchannel();
	afx_msg void OnBnClickedButtonSetOriginalProc();
	afx_msg void OnSelchangeListInfoBroadcasting();

public:
	CStatic m_staVideoArea;
	CListBox m_lstInfo;
	CStatic m_staChannel;
	CEdit m_edtChannel;
	CButton m_btnJoinChannel;
	CStatic m_staOriginalAudio;
	CComboBox m_cmbOriginalAudio;
	CButton m_btnSetAudioProc;
	CStatic m_staDetail;

	bool bRegisterEncoded = false;

	CAudioEncodedFrameObserver encodeObserver;
	afx_msg void OnBnClickedButtonEncodedAudio();
	CButton m_btnEncodedAudio;
};

