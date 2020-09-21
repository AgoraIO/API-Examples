#pragma once
#include "AGVideoWnd.h"
struct AudioIndication
{
	AudioVolumeInfo * speakers;
	unsigned int speakerNumber;
	int totalVolume;
};

class CAudioVolumeEventHandler : public IRtcEngineEventHandler
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
		Reports which users are speaking, the speakers' volume and whether the local user is speaking.
		This callback reports the IDs and volumes of the loudest speakers (at most 3 users) at the moment in the channel, and whether the local user is speaking.
		By default, this callback is disabled. You can enable it by calling the \ref IRtcEngine::enableAudioVolumeIndication(int, int, bool) "enableAudioVolumeIndication" method.
		Once enabled, this callback is triggered at the set interval, regardless of whether a user speaks or not.
		The SDK triggers two independent `onAudioVolumeIndication` callbacks at one time, which separately report the volume information of the local user and all the remote speakers.
		For more information, see the detailed parameter descriptions.
		@note
		- To enable the voice activity detection of the local user, ensure that you set `report_vad`(true) in the `enableAudioVolumeIndication` method.
		- Calling the \ref agora::rtc::IRtcEngine::muteLocalAudioStream "muteLocalAudioStream" method affects the SDK's behavior:
		- If the local user calls the \ref agora::rtc::IRtcEngine::muteLocalAudioStream "muteLocalAudioStream" method, the SDK stops triggering the local user's callback.
		- 20 seconds after a remote speaker calls the *muteLocalAudioStream* method, the remote speakers' callback excludes this remote user's information; 20 seconds after all remote users call the *muteLocalAudioStream* method, the SDK stops triggering the remote speakers' callback.
		- An empty @p speakers array in the *onAudioVolumeIndication* callback suggests that no remote user is speaking at the moment.
		@param speakers A pointer to AudioVolumeInfo:
		- In the local user's callback, this struct contains the following members:
		- `uid` = 0,
		- `volume` = `totalVolume`, which reports the sum of the voice volume and audio-mixing volume of the local user, and
		- `vad`, which reports the voice activity status of the local user.
		- In the remote speakers' callback, this array contains the following members:
		- `uid` of the remote speaker,
		- `volume`, which reports the sum of the voice volume and audio-mixing volume of each remote speaker, and
		- `vad` = 0.
		An empty speakers array in the callback indicates that no remote user is speaking at the moment.
		@param speakerNumber Total number of speakers. The value range is [0, 3].
		- In the local user's callback, `speakerNumber` = 1, regardless of whether the local user speaks or not.
		- In the remote speakers' callback, the callback reports the IDs and volumes of the three loudest speakers when there are more than three remote users in the channel, and `speakerNumber` = 3.
		@param totalVolume Total volume after audio mixing. The value ranges between 0 (lowest volume) and 255 (highest volume).
		- In the local user's callback, `totalVolume` is the sum of the voice volume and audio-mixing volume of the local user.
		- In the remote speakers' callback, `totalVolume` is the sum of the voice volume and audio-mixing volume of all the remote speakers.
	 */
	virtual void onAudioVolumeIndication(const AudioVolumeInfo* speakers, unsigned int speakerNumber, int totalVolume) override;
	/**
		Reports which user is the loudest speaker.
		If the user enables the audio volume indication by calling the \ref IRtcEngine::enableAudioVolumeIndication(int, int, bool) "enableAudioVolumeIndication" method, this callback returns the @p uid of the active speaker detected by the audio volume detection module of the SDK.
		@note
		- To receive this callback, you need to call the \ref IRtcEngine::enableAudioVolumeIndication(int, int, bool) "enableAudioVolumeIndication" method.
		- This callback returns the user ID of the user with the highest voice volume during a period of time, instead of at the moment.
		@param uid User ID of the active speaker. A @p uid of 0 represents the local user.
   */
	virtual void onActiveSpeaker(uid_t uid) override;
private:
	HWND m_hMsgHanlder;
};

class CAgoraAudioVolumeDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAgoraAudioVolumeDlg)

public:
	CAgoraAudioVolumeDlg(CWnd* pParent = nullptr); 
	virtual ~CAgoraAudioVolumeDlg();

	enum { IDD = IDD_DIALOG_VOLUME };
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
	IRtcEngine* m_rtcEngine = nullptr;
	CAGVideoWnd m_localVideoWnd;
	CAudioVolumeEventHandler m_eventHandler;
	AudioIndication *m_audioIndiaction = nullptr;
	AAudioDeviceManager *m_audioDeviceManager = nullptr;
	uid_t *m_activeSpeackerUid = nullptr;

protected:
	LRESULT OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam);
	LRESULT OnEIDLeaveChannel(WPARAM wParam, LPARAM lParam);
	LRESULT OnEIDUserJoined(WPARAM wParam, LPARAM lParam);
	LRESULT OnEIDUserOffline(WPARAM wParam, LPARAM lParam);
	LRESULT OnEIDAudioVolumeIndication(WPARAM wparam, LPARAM lparam);
	LRESULT OnEIDActiveSpeaker(WPARAM wparam, LPARAM lparam);

	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButtonJoinchannel();
	afx_msg void OnSelchangeListInfoBroadcasting();
	afx_msg void OnReleasedcaptureSliderCapVolume(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnReleasedcaptureSliderSignalVolume2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnReleasedcaptureSliderPlaybackVolume(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnReleasedcaptureSliderPlaybackSignalVolume(NMHDR *pNMHDR, LRESULT *pResult);
	virtual void DoDataExchange(CDataExchange* pDX);   

	DECLARE_MESSAGE_MAP()
public:
	CStatic m_staVideoArea;
	CListBox m_lstInfo;
	CStatic m_staChannel;
	CEdit m_edtChannel;
	CButton m_btnJoinChannel;
	CStatic m_staCapVol;
	CSliderCtrl m_sldCapVol;
	CStatic m_staCapSigVol;
	CSliderCtrl m_sldCapSigVol;
	CStatic m_staPlaybackVol;
	CSliderCtrl m_sldPlaybackVol;
	CStatic m_staPlaybackSigVol;
	CSliderCtrl m_sldPlaybackSigVol;
	CStatic m_details;

	CStatic m_staSpeaker_Info;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
