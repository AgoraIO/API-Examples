#include "stdafx.h"
#include "APIExample.h"
#include "CAgoraCaptureVideoDlg.h"
#include <dsound.h>

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


//set control text from config.
void CAgoraCaptureVideoDlg::InitCtrlText()
{
	m_staChannelName.SetWindowText(commonCtrlChannel);
	m_staCaputreVideo.SetWindowText(customVideoCaptureCtrlCaptureVideoDevice);
	m_btnJoinChannel.SetWindowText(commonCtrlJoinChannel);
	m_btnSetExtCapture.SetWindowText(customVideoCaptureCtrlSetExternlCapture);
}

/*
	create Agora RTC Engine and initialize context.set channel property.
*/
bool CAgoraCaptureVideoDlg::InitAgora()
{
	//create Agora RTC engine
	m_rtcEngine = createAgoraRtcEngine();
	if (!m_rtcEngine) {
		m_lstInfo.InsertString(m_lstInfo.GetCount() - 1, _T("createAgoraRtcEngine failed"));
		return false;
	}
	//set message notify receiver window
	m_eventHandler.SetMsgReceiver(m_hWnd);

	RtcEngineContext context;
	std::string strAppID = GET_APP_ID;
	context.appId = strAppID.c_str();
	context.eventHandler = &m_eventHandler;
	//initialize the Agora RTC engine context.
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
	m_rtcEngine->enableVideo();
	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("initialize success"));
	//set channel profile in the engine to the CHANNEL_PROFILE_LIVE_BROADCASTING.
	m_rtcEngine->setChannelProfile(CHANNEL_PROFILE_LIVE_BROADCASTING);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("live broadcasting"));
	//set client role in the engine to the CLIENT_ROLE_BROADCASTER.
	m_rtcEngine->setClientRole(CLIENT_ROLE_BROADCASTER);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("setClientRole broadcaster"));
	return true;
}

/*
	stop and release agora rtc engine.
*/
void CAgoraCaptureVideoDlg::UnInitAgora()
{
	m_cmbVideoDevice.EnableWindow(TRUE);
	m_cmbVideoType.EnableWindow(TRUE);
	m_btnSetExtCapture.EnableWindow(TRUE);
	if (m_rtcEngine) {
		if (m_joinChannel)
			m_joinChannel = !m_rtcEngine->leaveChannel();
		ResumeStatus();
		EnableExtendVideoCapture(FALSE);
		//stop preview in the engine.
		m_rtcEngine->stopPreview();
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("stopPreview"));
		//disable video in the engine.
		m_rtcEngine->disableVideo();
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("disableVideo"));
		//release engine.
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
	initialize dialog, and set control property.
*/
BOOL CAgoraCaptureVideoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_localVideoWnd.Create(NULL, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CRect(0, 0, 1, 1), this, ID_BASEWND_VIDEO + 100);
	RECT rcArea;
	m_staVideoArea.GetClientRect(&rcArea);
	m_localVideoWnd.MoveWindow(&rcArea);
	//create and initialize video capture object.
	m_agVideoCaptureDevice.Create();
	ResumeStatus();
	return TRUE;
}

/*
	set external video source or cancel.
*/
BOOL CAgoraCaptureVideoDlg::EnableExtendVideoCapture(BOOL bEnable)
{
	agora::util::AutoPtr<agora::media::IMediaEngine> mediaEngine;
	//query interface agora::AGORA_IID_MEDIA_ENGINE in the engine.
	mediaEngine.queryInterface(m_rtcEngine, agora::AGORA_IID_MEDIA_ENGINE);
	int nRet = 0;
	if (mediaEngine.get() == NULL)
		return FALSE;
	if (bEnable) {
		//set external video source
		nRet = mediaEngine->setExternalVideoSource(true, false);
	}
	else {
		//unset external video source
		nRet = mediaEngine->setExternalVideoSource(false, false);
	}
	return nRet == 0 ? TRUE : FALSE;
}

// update window view and control.
void CAgoraCaptureVideoDlg::UpdateViews()
{
	// render local video
	RenderLocalVideo();
	// enumerate device and show.
	UpdateDevice();
}

// enumerate device and show device in combobox.
void CAgoraCaptureVideoDlg::UpdateDevice()
{
	TCHAR				szDevicePath[MAX_PATH] = { 0 };
	SIZE_T				nPathLen = MAX_PATH;
	CString				strInfo;
	AGORA_DEVICE_INFO	agDeviceInfo;
	m_cmbVideoDevice.ResetContent();
	//enum video capture device.
	m_agVideoCaptureDevice.EnumDeviceList();
	for (int nIndex = 0; nIndex < m_agVideoCaptureDevice.GetDeviceCount(); nIndex++) {
		m_agVideoCaptureDevice.GetDeviceInfo(nIndex, &agDeviceInfo);
		m_cmbVideoDevice.InsertString(nIndex, agDeviceInfo.szDeviceName);
	}
	m_cmbVideoDevice.SetCurSel(0);
	OnSelchangeComboCaptureVideoDevice();
}
// resume window status.
void CAgoraCaptureVideoDlg::ResumeStatus()
{
	m_lstInfo.ResetContent();
	InitCtrlText();
	EnableCaputre(FALSE);
	m_joinChannel = false;
	m_initialize = false;
	m_remoteJoined = false;
	m_extenalCaptureVideo = false;
	m_edtChannel.SetWindowText(_T(""));
}

// start or stop capture.
// if bEnable is true start capture otherwise stop capture.
void CAgoraCaptureVideoDlg::EnableCaputre(BOOL bEnable)
{
	if (bEnable == (BOOL)!m_extenalCaptureVideo)return;

	int nIndex = m_cmbVideoType.GetCurSel();
	if (bEnable)
	{
		//select video capture type.
		m_agVideoCaptureDevice.SelectMediaCap(nIndex == -1 ? 0 : nIndex);
		VIDEOINFOHEADER videoInfo;
		VideoEncoderConfiguration config;
		//create video capture filter.
		m_agVideoCaptureDevice.CreateCaptureFilter();
		m_agVideoCaptureDevice.GetCurrentVideoCap(&videoInfo);
		config.dimensions.width = videoInfo.bmiHeader.biWidth;
		config.dimensions.height = videoInfo.bmiHeader.biHeight;
		m_videoFrame.stride = videoInfo.bmiHeader.biWidth;
		m_videoFrame.height = videoInfo.bmiHeader.biHeight;
		m_videoFrame.rotation = 0;
		m_videoFrame.cropBottom = 0;
		m_videoFrame.cropLeft = 0;
		m_videoFrame.cropRight = 0;
		m_videoFrame.cropTop = 0;
		m_videoFrame.format = agora::media::ExternalVideoFrame::VIDEO_PIXEL_I420;
		m_videoFrame.type = agora::media::ExternalVideoFrame::VIDEO_BUFFER_TYPE::VIDEO_BUFFER_RAW_DATA;
		m_fps = (int)(10000000ll / videoInfo.AvgTimePerFrame);
		//set video encoder configuration.
		m_rtcEngine->setVideoEncoderConfiguration(config);
		//set render hwnd,image width,image height,identify yuv.
		m_d3dRender.Init(m_localVideoWnd.GetSafeHwnd(),
			videoInfo.bmiHeader.biWidth, videoInfo.bmiHeader.biHeight, true);
		//start video capture.
		m_agVideoCaptureDevice.Start();
	}
	else {
		//video capture stop.
		m_agVideoCaptureDevice.Stop();
		//remove video capture filter.
		m_agVideoCaptureDevice.RemoveCaptureFilter();
		if (m_rtcEngine)
		{
			m_rtcEngine->stopPreview();
			m_d3dRender.Close();
		}
	}
}

void CAgoraCaptureVideoDlg::PushVideoFrameThread(CAgoraCaptureVideoDlg * self)
{
	agora::util::AutoPtr<agora::media::IMediaEngine> mediaEngine;
	//query interface agora::AGORA_IID_MEDIA_ENGINE in the engine.
	mediaEngine.queryInterface(self->m_rtcEngine, agora::AGORA_IID_MEDIA_ENGINE);
	//start preview in the engine.
	self -> m_rtcEngine->startPreview();
	while (self->m_extenalCaptureVideo && self->m_joinChannel)
	{
		if (self->m_videoFrame.format == agora::media::ExternalVideoFrame::VIDEO_PIXEL_I420) {
			int bufSize = self->m_videoFrame.stride * self->m_videoFrame.height * 3 / 2;
			int timestamp = GetTickCount();
			//read data from custom capture.
			if (CAgVideoBuffer::GetInstance()->readBuffer(self->m_buffer, bufSize, timestamp)) {
				self->m_videoFrame.timestamp = timestamp;
			}
			else
			{
				Sleep(1);
				continue;
			}
			self->m_videoFrame.buffer = self->m_buffer;
			//render image buffer to hwnd.
			self->m_d3dRender.Render((char*)self->m_buffer);
			//push video frame.
			mediaEngine->pushVideoFrame(&self->m_videoFrame);
			Sleep(1000 / self->m_fps);
		}
		else {
			return;
		}
	}
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
	if (bShow) {
		//init control text.
		InitCtrlText();
		//update window.
		UpdateViews();
	}
	else {
		//resume window status.
		ResumeStatus();
	}
}

/*
	start or stop capture,register or unregister video frame observer.
*/
void CAgoraCaptureVideoDlg::OnClickedButtonStartCaputre()
{
	m_extenalCaptureVideo = !m_extenalCaptureVideo;
	if (m_extenalCaptureVideo)
	{
		if (m_cmbVideoType.GetCurSel() == -1)
		{
			m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("can not set vitrual video capture"));
			return;
		}
		EnableExtendVideoCapture(TRUE);
		//register agora video frame observer.
		EnableCaputre(TRUE);
		
		m_btnSetExtCapture.SetWindowText(customVideoCaptureCtrlCancelExternlCapture);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("use extenal video frame observer sucess!"));
		
	}
	else {
		EnableCaputre(FALSE);
		//unregister agora frame observer.
		EnableExtendVideoCapture(FALSE);
		m_btnSetExtCapture.SetWindowText(customVideoCaptureCtrlSetExternlCapture);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("restore video frame observer sucess!"));
	}
}

//The JoinChannel button's click handler.
//This function either joins or leaves the channel
void CAgoraCaptureVideoDlg::OnClickedButtonJoinchannel()
{
	if (!m_rtcEngine || !m_initialize)
		return;
	CString strInfo;
	if (!m_joinChannel) {
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
		}
	}
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
}


//EID_JOINCHANNEL_SUCCESS message window handler.
LRESULT CAgoraCaptureVideoDlg::OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam)
{
	m_cmbVideoDevice.EnableWindow(FALSE);
	m_cmbVideoType.EnableWindow(FALSE);
	m_btnSetExtCapture.EnableWindow(FALSE);
	m_joinChannel = true;
	m_btnJoinChannel.EnableWindow(TRUE);
	m_btnJoinChannel.SetWindowText(commonCtrlLeaveChannel);
	CString strInfo;
	strInfo.Format(_T("%s:join success, uid=%u"), getCurrentTime(), wParam);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	m_localVideoWnd.SetUID(wParam);
	//notify parent window
	::PostMessage(GetParent()->GetSafeHwnd(), WM_MSGID(EID_JOINCHANNEL_SUCCESS), TRUE, 0);
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)PushVideoFrameThread, this, 0, NULL);

	return 0;
}

//EID_LEAVE_CHANNEL message window handler.
LRESULT CAgoraCaptureVideoDlg::OnEIDLeaveChannel(WPARAM wParam, LPARAM lParam)
{
	m_cmbVideoDevice.EnableWindow(TRUE);
	m_cmbVideoType.EnableWindow(TRUE);
	m_btnSetExtCapture.EnableWindow(TRUE);
	m_joinChannel = false;
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
	m_buffer = new BYTE[1920 * 1280 * 4 * 4];
}

CAgoraCaptureVideoDlg::~CAgoraCaptureVideoDlg()
{
	delete m_buffer;
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
	//get current device name.
	m_cmbVideoType.ResetContent();

	BOOL bSuccess = m_agVideoCaptureDevice.GetCurrentDevice(szDevicePath, &nPathLen);
	if (bSuccess)
		m_agVideoCaptureDevice.CloseDevice();

	if (nSel != -1) {
		//open device.
		if (!m_agVideoCaptureDevice.OpenDevice(nSel))
		{
			return;
		}
		//create capture filter.
		//m_agVideoCaptureDevice.CreateCaptureFilter();
	}
	//enumerate video capture device type. 
	int count = m_agVideoCaptureDevice.GetMediaCapCount();
	for (int nIndex = 0; nIndex < count; nIndex++) {
		m_agVideoCaptureDevice.GetVideoCap(nIndex, &vidInfoHeader);
		if (vidInfoHeader.bmiHeader.biCompression == 0)continue;
		switch (vidInfoHeader.bmiHeader.biCompression)
		{
		case MAKEFOURCC('I', '4', '2', '0'):
			strInfo.Format(_T("%d*%d %dfps(YUV420)"), vidInfoHeader.bmiHeader.biWidth, vidInfoHeader.bmiHeader.biHeight, 10000000ll / vidInfoHeader.AvgTimePerFrame);
			break;
		case 0x00000000:
			strInfo.Format(_T("%d*%d %dfps(RGB24)"), vidInfoHeader.bmiHeader.biWidth, vidInfoHeader.bmiHeader.biHeight, 10000000ll / vidInfoHeader.AvgTimePerFrame);
			break;
		case MAKEFOURCC('Y', 'U', 'Y', '2'):
			strInfo.Format(_T("%d*%d %dfps(YUY2)"), vidInfoHeader.bmiHeader.biWidth, vidInfoHeader.bmiHeader.biHeight, 10000000ll / vidInfoHeader.AvgTimePerFrame);
			break;
		case MAKEFOURCC('M', 'J', 'P', 'G'):
			strInfo.Format(_T("%d*%d %dfps(MJPEG)"), vidInfoHeader.bmiHeader.biWidth, vidInfoHeader.bmiHeader.biHeight, 10000000ll / vidInfoHeader.AvgTimePerFrame);
			break;
		case MAKEFOURCC('U', 'Y', 'V', 'Y'):
			strInfo.Format(_T("%d*%d %dfps(UYVY)"), vidInfoHeader.bmiHeader.biWidth, vidInfoHeader.bmiHeader.biHeight, 10000000ll / vidInfoHeader.AvgTimePerFrame);
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
	uid: user ID.If the UID is specified in the joinChannel, that ID is returned here;
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
	by the callback(ms).
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
parameters:
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
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) {
		return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}
