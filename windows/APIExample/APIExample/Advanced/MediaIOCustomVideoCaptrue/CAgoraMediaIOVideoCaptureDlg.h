#pragma once
#include "AGVideoWnd.h"
#include "DirectShow/AgVideoBuffer.h"
#include "DirectShow/AGDShowVideoCapture.h"
#include <mutex>

class CAgoraMediaIOVideoCaptureDlgEngineEventHandler : public IRtcEngineEventHandler {
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
		 @note This callback does not work properly when the number of users
		 (in the Communication profile) or broadcasters (in the Live-broadcast profile)
		 in the channel exceeds 17.
		 @param uid ID of the remote user whose video state changes.
		 @param state State of the remote video. See #REMOTE_VIDEO_STATE.
		 @param reason The reason of the remote video state change. See
		 #REMOTE_VIDEO_STATE_REASON.
		 @param elapsed Time elapsed (ms) from the local user calling the
		 agora::rtc::IRtcEngine::joinChannel "joinChannel" method until the
		 SDK triggers this callback.
	 */
	virtual void onRemoteVideoStateChanged(uid_t uid, REMOTE_VIDEO_STATE state, REMOTE_VIDEO_STATE_REASON reason, int elapsed);

private:
	HWND m_hMsgHanlder;
};


class CAgoraVideoSource :public IVideoSource {
	/** Notification for initializing the custom video source.
	 *
	 * The SDK triggers this callback to remind you to initialize the custom video source. After receiving this callback,
	 * you can do some preparation, such as enabling the camera, and then use the return value to tell the SDK whether the
	 * custom video source is prepared.
	 *
	 * @param consumer An IVideoFrameConsumer object that the SDK passes to you. You need to reserve this object and use it
	 * to send the video frame to the SDK once the custom video source is started. See IVideoFrameConsumer.
	 *
	 * @return
	 * - true: The custom video source is initialized.
	 * - false: The custom video source is not ready or fails to initialize. The SDK stops and reports the error.
	 */
	virtual bool onInitialize(IVideoFrameConsumer *consumer) override
	{
		std::lock_guard<std::mutex> m(m_mutex);
		m_videoConsumer = consumer;
		OutputDebugString(_T("onInitialize\n"));
		return true;
	}

	/** Notification for disabling the custom video source.
	 *
	 * The SDK triggers this callback to remind you to disable the custom video source device. This callback tells you
	 * that the SDK is about to release the IVideoFrameConsumer object. Ensure that you no longer use IVideoFrameConsumer
	 * after receiving this callback.
	 */
	virtual void onDispose() override
	{
		OutputDebugString(_T("onDispose\n"));
		Stop();
	}

	/** Notification for starting the custom video source.
	 *
	 * The SDK triggers this callback to remind you to start the custom video source for capturing video. The SDK uses
	 * IVideoFrameConsumer to receive the video frame that you capture after the video source is started. You must use
	 * the return value to tell the SDK whether the custom video source is started.
	 *
	 * @return
	 * - true: The custom video source is started.
	 * - false: The custom video source fails to start. The SDK stops and reports the error.
	 */
	virtual bool onStart() override
	{
		OutputDebugString(_T("onStart\n"));
		m_hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadRun, this, 0, NULL);
		return true;
	}


	//worker thread to read data and send data to sdk.
	static void ThreadRun(CAgoraVideoSource* self)
	{
		//wait for consume event until consume event is signaled
		while (WaitForSingleObject(self->m_hConsumeEvent, INFINITE) == WAIT_OBJECT_0)
		{
			//std::lock_guard<std::mutex> m(self->mutex);
			int bufSize = self->m_width * self->m_height * 3 / 2;
			int timestamp = GetTickCount();
			if (!CAgVideoBuffer::GetInstance()->readBuffer(self->m_buffer, bufSize, timestamp)) {
				Sleep(1);
				continue;
			}
			self->m_mutex.lock();//lock consumer and buffer
			if (self->m_videoConsumer)
			{
				//consume Raw Video Frame
				self->m_videoConsumer->consumeRawVideoFrame(self->m_buffer, ExternalVideoFrame::VIDEO_PIXEL_I420,
					self->m_width, self->m_height, self->m_rotation, timestamp);
				self->m_mutex.unlock();
			}else
				self->m_mutex.unlock();
		}
	}

	/** Notification for stopping capturing video.
	 *
	 * The SDK triggers this callback to remind you to stop capturing video. This callback tells you that the SDK is about
	 * to stop using IVideoFrameConsumer to receive the video frame that you capture.
	 */
	virtual void onStop() override
	{
		OutputDebugString(_T("onStop\n"));
		Stop();
	}

	/** Gets the video frame type.
	 *
	 * Before you initialize the custom video source, the SDK triggers this callback to query the video frame type. You
	 * must specify the video frame type in the return value and then pass it to the SDK.
	 *
	 * @note Ensure that the video frame type that you specify in this callback is the same as that in the \ref agora::rtc::IVideoFrameConsumer::consumeRawVideoFrame "consumeRawVideoFrame" method.
	 *
	 * @return \ref agora::media::ExternalVideoFrame::VIDEO_PIXEL_FORMAT "VIDEO_PIXEL_FORMAT"
	 */
	virtual agora::media::ExternalVideoFrame::VIDEO_PIXEL_FORMAT getBufferType() override
	{
		return ExternalVideoFrame::VIDEO_PIXEL_I420;
	}

	/** Gets the capture type of the custom video source.
	 *
	 * Before you initialize the custom video source, the SDK triggers this callback to query the capture type of the video source.
	 * You must specify the capture type in the return value and then pass it to the SDK. The SDK enables the corresponding video
	 * processing algorithm according to the capture type after receiving the video frame.
	 *
	 * @return #VIDEO_CAPTURE_TYPE
	 */
	virtual VIDEO_CAPTURE_TYPE getVideoCaptureType() override
	{
		return m_capType;// VIDEO_CAPTURE_CAMERA;
	}


	/** Gets the content hint of the custom video source.
	 *
	 * If you specify the custom video source as a screen-sharing video, the SDK triggers this callback to query the
	 * content hint of the video source before you initialize the video source. You must specify the content hint in the
	 * return value and then pass it to the SDK. The SDK enables the corresponding video processing algorithm according
	 * to the content hint after receiving the video frame.
	 *
	 * @return \ref agora::rtc::VideoContentHint "VideoContentHint"
	 */
	virtual VideoContentHint getVideoContentHint() override
	{
		return m_videoHintContent;
	}

	
public:
	CAgoraVideoSource()
	{
		m_buffer = new BYTE[1920 * 1080 * 4 * 4];
		//manual set event, initial state is not signaled
		m_hConsumeEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	}

	~CAgoraVideoSource()
	{
		if (m_hThread) {
			DWORD exitCode;
			bool ret = ::GetExitCodeThread(m_hThread, &exitCode);
			if (exitCode == STILL_ACTIVE) {
				ResetEvent(m_hConsumeEvent);   // set event nonsignaled, suspend thread
				TerminateThread(m_hThread, 0); // terminate thread
				CloseHandle(m_hThread);
				m_hThread = NULL;
			}
		}

		if (m_hConsumeEvent) {
			CloseHandle(m_hConsumeEvent);
			m_hConsumeEvent = NULL;
		}
		delete[] m_buffer;
		m_buffer = nullptr;
	}
	void SetVideoCaptureType(VIDEO_CAPTURE_TYPE type) { m_capType = type; }
	void SetVideoHintContent(VideoContentHint content) { m_videoHintContent = content; }
	void Stop()
	{
		std::lock_guard<std::mutex> m(m_mutex);
		//m_isExit = true;
		m_videoConsumer = nullptr;
		if (m_hThread) {
			CloseHandle(m_hThread);
			m_hThread = NULL;
		}
	}

	void SetParameters(int width, int height, int rotation,int fps)
	{
		std::lock_guard<std::mutex> m(m_mutex);
	//	m_isExit = isExit;
		m_width = width;
		m_height = height;
		m_rotation = rotation;
		m_fps = fps;
	}
	void SetConsumeEvent() { SetEvent(m_hConsumeEvent); }
	void ResetConsumeEvent() { ResetEvent(m_hConsumeEvent); }
private:
	IVideoFrameConsumer * m_videoConsumer;
	//bool m_isExit;
	BYTE * m_buffer;
	int m_width;
	int m_height;
	int m_rotation;
	int m_fps;
	std::mutex m_mutex;
	HANDLE m_hThread = NULL;
	VIDEO_CAPTURE_TYPE m_capType = VIDEO_CAPTURE_UNKNOWN;
	VideoContentHint m_videoHintContent = CONTENT_HINT_NONE;
	HANDLE m_hConsumeEvent = NULL;
};

#define ENCODER_CONFIG_COUNT 4
class CAgoraMediaIOVideoCaptureDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAgoraMediaIOVideoCaptureDlg)

public:
	// agora sdk message window handler
	LRESULT OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam);
	LRESULT OnEIDLeaveChannel(WPARAM wParam, LPARAM lParam);
	LRESULT OnEIDUserJoined(WPARAM wParam, LPARAM lParam);
	LRESULT OnEIDUserOffline(WPARAM wParam, LPARAM lParam);
	LRESULT OnEIDRemoteVideoStateChanged(WPARAM wParam, LPARAM lParam);

	CAgoraMediaIOVideoCaptureDlg(CWnd* pParent = nullptr);
	virtual ~CAgoraMediaIOVideoCaptureDlg();
	//Initialize the Agora SDK
	bool InitAgora();
	//UnInitialize the Agora SDK
	void UnInitAgora();
	//set control text from config.
	void InitCtrlText();
	//render local video from SDK local capture.
	void RenderLocalVideo();
	
	// update window view and control.
	void UpdateViews();
	// enumerate device and show device in combobox.
	void UpdateDevice();
	// resume window status.
	void ResumeStatus();
	// start or stop capture.
	// if bEnable is true start capture otherwise stop capture.
	void EnableCaputure(BOOL bEnable);

	enum {
		IDD = IDD_DIALOG_CUSTOM_CAPTURE_MEDIA_IO_VIDEO
	};

	enum VIDEO_SOURCE_CAPTURE_TYPE {
		VIDEO_SOURCE_SDK_CAMERA = 0,
		VIDEO_SOURCE_SDK_SCREEN ,
		VIDEO_SOURCE_CUSTOM_CAMERA,
		VIDEO_SOURCE_CUSTOM_SCREEM,
	};

	VIDEO_SOURCE_CAPTURE_TYPE m_preCaptureType = VIDEO_SOURCE_SDK_CAMERA;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	void EnableControl();
	CAgoraMediaIOVideoCaptureDlgEngineEventHandler m_eventHandler;
	CAGDShowVideoCapture m_agVideoCaptureDevice;
	CAGVideoWnd m_localVideoWnd;
	CAgoraVideoSource m_videoSouce;

	IRtcEngine* m_rtcEngine = nullptr;
	AVideoDeviceManager* m_videoDeviceManager = nullptr;
	agora::rtc::IVideoDeviceCollection* m_lpVideoCollection = nullptr;
	bool m_joinChannel = false;
	bool m_initialize = false;
	bool m_remoteJoined = false;
	bool m_extenalCaptureVideo = false;
	
	uint8_t * screenBuffer = NULL;
	FILE* m_screenFile = NULL;
	const int external_screen_w = 1920;
	const int external_screen_h = 1080;
	const int external_screen_fps = 15;

	VideoEncoderConfiguration m_externalCameraConfig;

	VideoEncoderConfiguration encoderConfigs[ENCODER_CONFIG_COUNT];
	DECLARE_MESSAGE_MAP()
public:
	CStatic m_staVideoArea;
	CStatic m_staChannelName;
	CStatic m_staCaputreVideo;
	CEdit m_edtChannel;
	CButton m_btnJoinChannel;
	CComboBox m_cmbVideoDevice;
	CComboBox m_cmbVideoResoliton;
	CComboBox m_cmbCaptureType;
	CStatic m_staSDKCamera;
	CStatic m_staCaptureType;
	CComboBox m_cmbSDKCamera;
	CComboBox m_cmbSDKResolution;

	CListBox m_lstInfo;
	virtual BOOL OnInitDialog();
	afx_msg	void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnClickedButtonStartCaputre();
	afx_msg void OnClickedButtonJoinchannel();
	afx_msg void OnSelchangeComboCaptureVideoDevice();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	
	afx_msg void OnSelchangeCmbMedioCapturetype();
	afx_msg void OnSelchangeComboSdkcamera();

	afx_msg void OnSelchangeComboSdkResolution();
};
