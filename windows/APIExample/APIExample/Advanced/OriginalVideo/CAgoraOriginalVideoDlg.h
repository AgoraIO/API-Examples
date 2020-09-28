#pragma once
#include "AGVideoWnd.h"


// Video gray scale processing Frame Observer
class CGrayVideoProcFrameObserver :
	public agora::media::IVideoFrameObserver
{
public:
	virtual ~CGrayVideoProcFrameObserver() {  }
	/*
		Obtain video data from the local camera.After successfully registering
		a video data observer, the SDK triggers this callback when each video
		frame is captured. You can retrieve the video data from the local camera
		in the callback, and then pre-process the video data according to the needs
		of the scene.After the preprocessing is done, you can send the processed
		video data back to the SDK in this callback.
		annotations:
		If the video data type you get is RGBA, Agora does not support sending the
		processed RGBA data back to the SDK through this callback.
		parameter:
		videoFrame :VideoFramedata, see VideoFrame for more details
		return If the video pre-processing fails,whether to ignore the video frame:
		True: No ignore.
		False: Ignored, the frame data is not sent back to the SDK.
	*/
	virtual bool onCaptureVideoFrame(VideoFrame& videoFrame);
	/*
		Gets video data sent remotely.After successfully registering a video data observer,
		the SDK triggers this callback when each video frame is captured. You can retrieve
		the video data sent remotely in the callback, and then post-process the video data
		according to the scenario requirements.After the post-processing, you can send the
		processed video data back to the SDK in the callback.
		annotations:
		If the video data type you get is RGBA, Agora does not support sending the processed RGBA data back
		to the SDK through this callback.
		parameter:
		uid: The remote user ID to send the frame video
		videoFrame: VideoFrame data, see VideoFrame for more details
		return If the video pre-processing fails,whether to ignore the video frame:
		True: No ignore.
		False: Ignored, the frame data is not sent back to the SDK.
	*/
	virtual bool onRenderVideoFrame(unsigned int uid, VideoFrame& videoFrame);
};


// Video gray scale processing Frame Observer
class CAverageFilterVideoProcFrameObserver :
	public agora::media::IVideoFrameObserver
{
private:
	unsigned char * m_buffer = new unsigned char[1920 * 1280 * 3];
	unsigned char * m_height = new unsigned char[1920];
public:

	virtual ~CAverageFilterVideoProcFrameObserver() { delete[]m_buffer; delete[]m_height; }
	/*
		Obtain video data from the local camera.After successfully registering
		a video data observer, the SDK triggers this callback when each video
		frame is captured. You can retrieve the video data from the local camera
		in the callback, and then pre-process the video data according to the needs
		of the scene.After the preprocessing is done, you can send the processed
		video data back to the SDK in this callback.
		annotations:
		If the video data type you get is RGBA, Agora does not support sending the
		processed RGBA data back to the SDK through this callback.
		parameter:
		videoFrame :VideoFramedata, see VideoFrame for more details
		return If the video pre-processing fails,whether to ignore the video frame:
		True: No ignore.
		False: Ignored, the frame data is not sent back to the SDK.
	*/
	virtual bool onCaptureVideoFrame(VideoFrame& videoFrame);
	/*
		Gets video data sent remotely.After successfully registering a video data observer,
		the SDK triggers this callback when each video frame is captured. You can retrieve
		the video data sent remotely in the callback, and then post-process the video data
		according to the scenario requirements.After the post-processing, you can send the
		processed video data back to the SDK in the callback.
		annotations:
		If the video data type you get is RGBA, Agora does not support sending the processed RGBA data back
		to the SDK through this callback.
		parameter:
		uid: The remote user ID to send the frame video
		videoFrame: VideoFrame data, see VideoFrame for more details
		return If the video pre-processing fails,whether to ignore the video frame:
		True: No ignore.
		False: Ignored, the frame data is not sent back to the SDK.
	*/
	virtual bool onRenderVideoFrame(unsigned int uid, VideoFrame& videoFrame);
	void AverageFiltering(unsigned char * data, int width, int height, int step);
};


class COriginalVideoEventHandler : public IRtcEngineEventHandler
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
private:
	HWND m_hMsgHanlder;
};



class CAgoraOriginalVideoDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAgoraOriginalVideoDlg)

public:
	CAgoraOriginalVideoDlg(CWnd* pParent = nullptr); 
	virtual ~CAgoraOriginalVideoDlg();

	enum { IDD = IDD_DIALOG_ORIGINAL_VIDEO };
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
	//register or unregister agora video Frame Observer.
	BOOL RegisterVideoFrameObserver(BOOL bEnable, IVideoFrameObserver * videoFrameObserver = NULL);




private:
	bool m_joinChannel = false;
	bool m_initialize = false;
	bool m_setVideoProc = false;
	IRtcEngine* m_rtcEngine = nullptr;
	CAGVideoWnd m_localVideoWnd;
	COriginalVideoEventHandler m_eventHandler;

	CGrayVideoProcFrameObserver m_garyVideoFrameObserver;
	CAverageFilterVideoProcFrameObserver m_averageFilterVideoFrameObserver;
	std::map<CString,IVideoFrameObserver *> m_mapVideoFrame;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);   
	LRESULT OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam);
	LRESULT OnEIDLeaveChannel(WPARAM wParam, LPARAM lParam);
	LRESULT OnEIDUserJoined(WPARAM wParam, LPARAM lParam);
	LRESULT OnEIDUserOffline(WPARAM wParam, LPARAM lParam);
	LRESULT OnEIDRemoteVideoStateChanged(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	CStatic m_staVideoArea;
	CStatic m_staChannel;
	CEdit m_edtChannel;
	CButton m_btnJoinChannel;
	CListBox m_lstInfo;
	CStatic m_staDetail;
	CStatic m_staVideoProc;
	CComboBox m_cmbVideoProc;
	CButton m_btnSetVideoProc;
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnSelchangeListInfoBroadcasting();
	afx_msg void OnBnClickedButtonJoinchannel();
	afx_msg void OnBnClickedButtonSetOriginalProc();
};
