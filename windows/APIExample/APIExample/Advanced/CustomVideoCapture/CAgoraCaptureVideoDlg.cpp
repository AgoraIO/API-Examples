#include "../../stdafx.h"
#include "../../APIExample.h"
#include "CAgoraCaptureVideoDlg.h"
#include "afxdialogex.h"


BEGIN_MESSAGE_MAP(CAgoraCaptureVideoDlg, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_MESSAGE(WM_MSGID(EID_JOINCHANNEL_SUCCESS), &CAgoraCaptureVideoDlg::OnEIDJoinChannelSuccess)
	ON_MESSAGE(WM_MSGID(EID_LEAVE_CHANNEL), &CAgoraCaptureVideoDlg::OnEIDLeaveChannel)
	ON_MESSAGE(WM_MSGID(EID_USER_JOINED), &CAgoraCaptureVideoDlg::OnEIDUserJoined)
	ON_MESSAGE(WM_MSGID(EID_USER_OFFLINE), &CAgoraCaptureVideoDlg::OnEIDUserOffline)
	ON_MESSAGE(WM_MSGID(EID_REMOTE_VIDEO_STATE_CHANED), &CAgoraCaptureVideoDlg::OnEIDRemoteVideoStateChanged)
	ON_BN_CLICKED(IDC_BUTTON_START_CAPUTRE, &CAgoraCaptureVideoDlg::OnClickedButtonStartCaputre)
	ON_BN_CLICKED(IDC_BUTTON_JOINCHANNEL, &CAgoraCaptureVideoDlg::OnClickedButtonJoinchannel)
	ON_CBN_SELCHANGE(IDC_COMBO_CAPTURE_VIDEO_DEVICE, &CAgoraCaptureVideoDlg::OnSelchangeComboCaptureVideoDevice)
END_MESSAGE_MAP()

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
bool CExtendVideoFrameObserver::onCaptureVideoFrame(VideoFrame & videoFrame)
{
	int bufSize = videoFrame.width * videoFrame.height * 3 / 2;
	int timestamp = GetTickCount();
	//read video capture buffer data and get timestamp copy to video Frame.
	if (CAgVideoBuffer::GetInstance()->readBuffer(m_lpBuffer, bufSize, timestamp)) {
		memcpy_s(m_videoBuffer.m_lpImageBuffer, bufSize, m_lpBuffer, bufSize);
		m_videoBuffer.timestamp = timestamp;
	}
	else
		OutputDebugString(L"readBuffer failed");
	m_lpY = m_videoBuffer.m_lpImageBuffer;
	m_lpU = m_videoBuffer.m_lpImageBuffer + videoFrame.height * videoFrame.width;
	m_lpV = m_videoBuffer.m_lpImageBuffer + 5 * videoFrame.height * videoFrame.width / 4;
	//copy yuv data to video frame
	memcpy_s(videoFrame.yBuffer, videoFrame.height * videoFrame.width, m_lpY, videoFrame.height * videoFrame.width);
	videoFrame.yStride = videoFrame.width;
	memcpy_s(videoFrame.uBuffer, videoFrame.height * videoFrame.width / 4, m_lpU, videoFrame.height * videoFrame.width / 4);
	videoFrame.uStride = videoFrame.width / 2;
	memcpy_s(videoFrame.vBuffer, videoFrame.height * videoFrame.width / 4, m_lpV, videoFrame.height * videoFrame.width / 4);
	videoFrame.vStride = videoFrame.width / 2;
	//set video frame type.
	videoFrame.type = FRAME_TYPE_YUV420;
	//set video rotation.
	videoFrame.rotation = 0;
	return true;
}
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
bool CExtendVideoFrameObserver::onRenderVideoFrame(unsigned int uid, VideoFrame & videoFrame)
{
	return true;
}
//set control text from config.
void CAgoraCaptureVideoDlg::InitCtrlText()
{
	m_staChannelName.SetWindowText(commonCtrlChannel);
	m_staCaputreVideo.SetWindowText(customVideoCaptureCtrlCaptureVideoDevice);
	m_btnJoinChannel.SetWindowText(commonCtrlJoinChannel);
	m_btnSetExtCapture.SetWindowText(customVideoCaptureCtrlSetExternlCapture);
}

/*
	create Agora Rtc Engine and initialize context.set channel proprety.
*/
bool CAgoraCaptureVideoDlg::InitAgora()
{
	//create Agora RTC engine
	m_rtcEngine = createAgoraRtcEngine();
	m_lstInfo.ResetContent();
	if (!m_rtcEngine) {
		m_lstInfo.InsertString(m_lstInfo.GetCount() - 1, _T("createAgoraRtcEngine failed"));
		return false;
	}
	//set message notify receiver window
	m_eventHandler.SetMsgReceiver(m_hWnd);

	RtcEngineContext context;
	context.appId = APP_ID;
	context.eventHandler = &m_eventHandler;
	//initalize the Agora RTC engine context.
	int ret = m_rtcEngine->initialize(context);
	if (ret != 0) {
		m_initialize = false;
		CString strInfo;
		strInfo.Format(_T("initialize failed: %d"), ret);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
		return false;
	}
	else
		m_initialize = true;
	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("initialize success"));
	//set channel profile in the engine to the CHANNEL_PROFILE_LIVE_BROADCASTING.
	m_rtcEngine->setChannelProfile(CHANNEL_PROFILE_LIVE_BROADCASTING);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("live broadcasting"));
	//set clinet role in the engine to the CLIENT_ROLE_BROADCASTER.
	m_rtcEngine->setClientRole(CLIENT_ROLE_BROADCASTER);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("setClientRole broadcaster"));
	m_btnJoinChannel.EnableWindow(TRUE);
	return true;
}

/*
	stop and release agora rtc engine.
*/
void CAgoraCaptureVideoDlg::UnInitAgora()
{
	if (m_rtcEngine) {
		//stop preview in the engine.
		m_rtcEngine->stopPreview();
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("stopPreview"));
		//disable video in the engine.
		m_rtcEngine->disableVideo();
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("disableVideo"));
		//relase engine.
		m_rtcEngine->release(true);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("release rtc engine"));
		m_rtcEngine = NULL;
	}
}

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
void CAgoraCaptureVideoDlgEngineEventHandler::onRemoteVideoStateChanged(uid_t uid, REMOTE_VIDEO_STATE state, REMOTE_VIDEO_STATE_REASON reason, int elapsed)
{
	if (m_hMsgHanlder) {
		PVideoStateStateChanged stateChanged = new VideoStateStateChanged;
		stateChanged->uid = uid;
		stateChanged->reason = reason;
		stateChanged->state = state;
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_REMOTE_VIDEO_STATE_CHANED), (WPARAM)stateChanged, 0);
	}
}

/*
	initialize dialog, and set control proprety.
*/
BOOL CAgoraCaptureVideoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	InitCtrlText();
	m_localVideoWnd.Create(NULL, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CRect(0, 0, 1, 1), this, ID_BASEWND_VIDEO + 100);
	RECT rcArea;
	m_staVideoArea.GetClientRect(&rcArea);
	//create and initialize video capture object.
	m_agVideoCaptureDevice.Create();
	m_localVideoWnd.MoveWindow(&rcArea);
	m_localVideoWnd.ShowWindow(SW_SHOW);
	m_btnSetExtCapture.EnableWindow(FALSE);
	return TRUE;  
}

/*
	register or unregister agora video Frame Observer.
*/
BOOL CAgoraCaptureVideoDlg::EnableExtendVideoCapture(BOOL bEnable)
{
	agora::util::AutoPtr<agora::media::IMediaEngine> mediaEngine;
	//query interface agora::AGORA_IID_MEDIA_ENGINE in the engine.
	mediaEngine.queryInterface(m_rtcEngine, agora::AGORA_IID_MEDIA_ENGINE);
	int nRet = 0;
	AParameter apm(*m_rtcEngine);
	if (mediaEngine.get() == NULL)
		return FALSE;
	if (bEnable) {
		//set local video camera index.
		apm->setParameters("{\"che.video.local.camera_index\":1024}");
		//register agora video frame observer.
		nRet = mediaEngine->registerVideoFrameObserver(&m_extVideoFrameObserver);
	}
	else {
		//unregister agora video frame observer.
		nRet = mediaEngine->registerVideoFrameObserver(NULL);
	}
	m_extenalCaptureVideo = bEnable;
	return nRet == 0 ? TRUE : FALSE;
}

/*
	set up canvas and local video view.
*/
void CAgoraCaptureVideoDlg::RenderLocalVideo()
{
	if (m_rtcEngine) {
		VideoCanvas canvas;
		canvas.renderMode = RENDER_MODE_FIT;
		canvas.uid = 0;
		canvas.view = m_localVideoWnd.GetSafeHwnd();
		//setup local video in the engine to canvas.
		m_rtcEngine->setupLocalVideo(canvas);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("render local video"));
	}
}


/*
	Enumerate all the video capture devices and add to the combo box.
*/
void CAgoraCaptureVideoDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);
	if (bShow == FALSE)return;
	TCHAR				szDevicePath[MAX_PATH] = { 0 };
	SIZE_T				nPathLen = MAX_PATH;
	CString				strInfo;
	AGORA_DEVICE_INFO	agDeviceInfo;
	
	
	m_cmbVideoDevice.ResetContent();
	//enum video capture deivce.
	m_agVideoCaptureDevice.EnumDeviceList();
	for (int nIndex = 0; nIndex < m_agVideoCaptureDevice.GetDeviceCount(); nIndex++) {
		m_agVideoCaptureDevice.GetDeviceInfo(nIndex, &agDeviceInfo);
		m_cmbVideoDevice.InsertString(nIndex, agDeviceInfo.szDeviceName);
	}
	m_cmbVideoDevice.SetCurSel(0);
	OnSelchangeComboCaptureVideoDevice();
	RenderLocalVideo();
}

/*
	start or stop capture,register or unregister video frame observer. 
*/
void CAgoraCaptureVideoDlg::OnClickedButtonStartCaputre()
{
	if (!m_extenalCaptureVideo)
	{
		//register agora video frame observer.
		if (EnableExtendVideoCapture(TRUE))
		{ 	
			//select video capture type.
			m_agVideoCaptureDevice.SelectMediaCap(m_cmbVideoType.GetCurSel());
			VIDEOINFOHEADER videoInfo;
			VideoEncoderConfiguration config;
			m_extenalCaptureVideo = true;
			//create video caputre filter.
			m_agVideoCaptureDevice.CreateCaptureFilter();
			m_agVideoCaptureDevice.GetCurrentVideoCap(&videoInfo);
			config.dimensions.width = videoInfo.bmiHeader.biWidth;
			config.dimensions.height = videoInfo.bmiHeader.biHeight;
			//set video encoder configuration.
			m_rtcEngine->setVideoEncoderConfiguration(config);
			//start video capture.
			m_agVideoCaptureDevice.Start();
			//enable video in the engine.
			m_rtcEngine->enableVideo();
			//start preview in the engine.
			m_rtcEngine->startPreview();
			m_btnSetExtCapture.SetWindowText(customVideoCaptureCtrlCancelExternlCapture);
			m_btnJoinChannel.EnableWindow(FALSE);
			m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("use extenal video frame observer sucess!"));
		}
		else {
			m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("use extenal video frame observer falied!"));
		}
	}
	else {
		//video capture stop.
		m_agVideoCaptureDevice.Stop();
		//remove video capture filter.
		m_agVideoCaptureDevice.RemoveCaptureFilter();
		//disable video in the engine.
		m_rtcEngine->disableVideo();
		//stop preview in the engine.
		m_rtcEngine->stopPreview();
		//unregister agora frame observer.
		if (EnableExtendVideoCapture(FALSE))
		{
			m_extenalCaptureVideo = false;
			m_btnJoinChannel.EnableWindow(TRUE);
			m_btnSetExtCapture.SetWindowText(customVideoCaptureCtrlSetExternlCapture);
			m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("restore video frame observer sucess!"));
		}
		else {
			m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("restore video frame observer falied!"));
		}
	}


}

//The JoinChannel button's click handler.
//This function either joins or leaves the channel
void CAgoraCaptureVideoDlg::OnClickedButtonJoinchannel()
{
	if (!m_rtcEngine || !m_initialize)
		return;
	CString strInfo;
	if (!joinChannel) {
		CString strChannelName;
		m_edtChannel.GetWindowText(strChannelName);
		if (strChannelName.IsEmpty()) {
			AfxMessageBox(_T("Fill channel name first"));
			return;
		}
		std::string szChannelId = cs2utf8(strChannelName);
		//join channel in the engine.
		if (0 == m_rtcEngine->joinChannel(APP_TOKEN, szChannelId.c_str(), "", 0)) {
			strInfo.Format(_T("join channel %s"), getCurrentTime());
			m_btnJoinChannel.EnableWindow(FALSE);
		}
	}
	else {
		//leave channel in the engine.
		if (0 == m_rtcEngine->leaveChannel()) {
			strInfo.Format(_T("leave channel %s"), getCurrentTime());
			m_btnJoinChannel.EnableWindow(FALSE);
		}
	}
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
}


//EID_JOINCHANNEL_SUCCESS message window handler.
LRESULT CAgoraCaptureVideoDlg::OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam)
{
	m_btnJoinChannel.EnableWindow(TRUE);
	m_btnSetExtCapture.EnableWindow(TRUE);
	joinChannel = true;
	m_btnJoinChannel.SetWindowText(commonCtrlLeaveChannel);
	CString strInfo;
	strInfo.Format(_T("%s:join success, uid=%u"), getCurrentTime(), wParam);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	m_localVideoWnd.SetUID(wParam);
	//notify parent window
	::PostMessage(GetParent()->GetSafeHwnd(), WM_MSGID(EID_JOINCHANNEL_SUCCESS), TRUE, 0);
	return 0;
}

//EID_LEAVE_CHANNEL message window handler.
LRESULT CAgoraCaptureVideoDlg::OnEIDLeaveChannel(WPARAM wParam, LPARAM lParam)
{
	m_btnJoinChannel.EnableWindow(TRUE);
	m_btnSetExtCapture.EnableWindow(FALSE);

	joinChannel = false;
	m_btnJoinChannel.SetWindowText(commonCtrlJoinChannel);

	CString strInfo;
	strInfo.Format(_T("leave channel success %s"), getCurrentTime());
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	::PostMessage(GetParent()->GetSafeHwnd(), WM_MSGID(EID_JOINCHANNEL_SUCCESS), FALSE, 0);
	return 0;
}

//EID_USER_JOINED message window handler.
LRESULT CAgoraCaptureVideoDlg::OnEIDUserJoined(WPARAM wParam, LPARAM lParam)
{
	CString strInfo;
	strInfo.Format(_T("%u joined"), wParam);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

	return 0;
}
//EID_USER_OFFLINE message window handler.
LRESULT CAgoraCaptureVideoDlg::OnEIDUserOffline(WPARAM wParam, LPARAM lParam)
{
	uid_t remoteUid = (uid_t)wParam;
	VideoCanvas canvas;
	canvas.uid = remoteUid;
	canvas.view = NULL;
	m_rtcEngine->setupRemoteVideo(canvas);
	CString strInfo;
	strInfo.Format(_T("%u offline, reason:%d"), remoteUid, lParam);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

	return 0;
}

//EID_REMOTE_VIDEO_STATE_CHANED message window handler.
LRESULT CAgoraCaptureVideoDlg::OnEIDRemoteVideoStateChanged(WPARAM wParam, LPARAM lParam)
{
	PVideoStateStateChanged stateChanged = (PVideoStateStateChanged)wParam;
	if (stateChanged) {
		//onRemoteVideoStateChanged
		CString strSateInfo;
		switch (stateChanged->state) {
		case REMOTE_VIDEO_STATE_STARTING:
			strSateInfo = _T("REMOTE_VIDEO_STATE_STARTING");
			break;
		case REMOTE_VIDEO_STATE_STOPPED:
			strSateInfo = _T("strSateInfo");
			break;
		case REMOTE_VIDEO_STATE_DECODING:
			strSateInfo = _T("REMOTE_VIDEO_STATE_DECODING");
			break;
		case REMOTE_VIDEO_STATE_FAILED:
			strSateInfo = _T("REMOTE_VIDEO_STATE_FAILED ");
			break;
		case REMOTE_VIDEO_STATE_FROZEN:
			strSateInfo = _T("REMOTE_VIDEO_STATE_FROZEN  ");
			break;
		}
		CString strInfo;
		strInfo.Format(_T("onRemoteVideoStateChanged: uid=%u, %s"), stateChanged->uid, strSateInfo);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	}
	return 0;
}

IMPLEMENT_DYNAMIC(CAgoraCaptureVideoDlg, CDialogEx)

CAgoraCaptureVideoDlg::CAgoraCaptureVideoDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_CUSTOM_CAPTURE_VIDEO, pParent)
{

}

CAgoraCaptureVideoDlg::~CAgoraCaptureVideoDlg()
{
}

void CAgoraCaptureVideoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_VIDEO, m_staVideoArea);
	DDX_Control(pDX, IDC_STATIC_CHANNELNAME, m_staChannelName);
	DDX_Control(pDX, IDC_STATIC_CAPTUREDEVICE, m_staCaputreVideo);
	DDX_Control(pDX, IDC_EDIT_CHANNELNAME, m_edtChannel);
	DDX_Control(pDX, IDC_BUTTON_JOINCHANNEL, m_btnJoinChannel);
	DDX_Control(pDX, IDC_BUTTON_START_CAPUTRE, m_btnSetExtCapture);
	DDX_Control(pDX, IDC_COMBO_CAPTURE_VIDEO_DEVICE, m_cmbVideoDevice);
	DDX_Control(pDX, IDC_COMBO_CAPTURE_VIDEO_TYPE, m_cmbVideoType);
	DDX_Control(pDX, IDC_LIST_INFO_BROADCASTING, m_lstInfo);
}

//Enumerates the video capture devices and types, 
//and inserts them into the ComboBox
void CAgoraCaptureVideoDlg::OnSelchangeComboCaptureVideoDevice()
{
	TCHAR	szDevicePath[MAX_PATH] = { 0 };
	SIZE_T	nPathLen = MAX_PATH;
	int		nSel = m_cmbVideoDevice.GetCurSel();

	VIDEOINFOHEADER		vidInfoHeader;
	CString				strInfo;
	CString				strCompress;
	//get current deivce name.
	BOOL bSuccess = m_agVideoCaptureDevice.GetCurrentDevice(szDevicePath, &nPathLen);
	if (bSuccess)
		m_agVideoCaptureDevice.CloseDevice();

	if (nSel != -1) {
		//open device.
		m_agVideoCaptureDevice.OpenDevice(nSel);
		//create capture filter.
		m_agVideoCaptureDevice.CreateCaptureFilter();
	}
	m_cmbVideoType.ResetContent();
	//enumrate video capture device type. 
	int count = m_agVideoCaptureDevice.GetMediaCapCount();
	for (int nIndex = 0; nIndex < count; nIndex++) {
		m_agVideoCaptureDevice.GetVideoCap(nIndex, &vidInfoHeader);
		switch (vidInfoHeader.bmiHeader.biCompression)
		{
		case 0x00000000:
			strInfo.Format(_T("%d*%d %dfps(RGB24)"), vidInfoHeader.bmiHeader.biWidth, vidInfoHeader.bmiHeader.biHeight, 10000000 / vidInfoHeader.AvgTimePerFrame);
			break;
		case MAKEFOURCC('I', '4', '2', '0'):
			strInfo.Format(_T("%d*%d %dfps(YUV420)"), vidInfoHeader.bmiHeader.biWidth, vidInfoHeader.bmiHeader.biHeight, 10000000 / vidInfoHeader.AvgTimePerFrame);
			break;
		case MAKEFOURCC('Y', 'U', 'Y', '2'):
			strInfo.Format(_T("%d*%d %dfps(YUY2)"), vidInfoHeader.bmiHeader.biWidth, vidInfoHeader.bmiHeader.biHeight, 10000000 / vidInfoHeader.AvgTimePerFrame);
			break;
		case MAKEFOURCC('M', 'J', 'P', 'G'):
			strInfo.Format(_T("%d*%d %dfps(MJPEG)"), vidInfoHeader.bmiHeader.biWidth, vidInfoHeader.bmiHeader.biHeight, 10000000 / vidInfoHeader.AvgTimePerFrame);
			break;
		case MAKEFOURCC('U', 'Y', 'V', 'Y'):
			strInfo.Format(_T("%d*%d %dfps(UYVY)"), vidInfoHeader.bmiHeader.biWidth, vidInfoHeader.bmiHeader.biHeight, 10000000 / vidInfoHeader.AvgTimePerFrame);
			break;
		}

		m_cmbVideoType.InsertString(nIndex, strInfo);
	}
	m_cmbVideoType.SetCurSel(0);
}





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
void CAgoraCaptureVideoDlgEngineEventHandler::onJoinChannelSuccess(const char* channel, uid_t uid, int elapsed)
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
	by the callback（ms).
*/
void CAgoraCaptureVideoDlgEngineEventHandler::onUserJoined(uid_t uid, int elapsed)
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
void CAgoraCaptureVideoDlgEngineEventHandler::onUserOffline(uid_t uid, USER_OFFLINE_REASON_TYPE reason)
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
parametes:
	stats: Call statistics.
*/
void CAgoraCaptureVideoDlgEngineEventHandler::onLeaveChannel(const RtcStats& stats)
{
	if (m_hMsgHanlder) {
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_LEAVE_CHANNEL), 0, 0);
	}
}

BOOL CAgoraCaptureVideoDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN&&pMsg->wParam==VK_RETURN) {
		return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}
