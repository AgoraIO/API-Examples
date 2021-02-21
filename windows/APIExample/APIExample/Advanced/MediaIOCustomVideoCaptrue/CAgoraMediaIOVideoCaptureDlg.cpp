#include "stdafx.h"
#include "APIExample.h"
#include "CAgoraMediaIOVideoCaptureDlg.h"

BEGIN_MESSAGE_MAP(CAgoraMediaIOVideoCaptureDlg, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_MESSAGE(WM_MSGID(EID_JOINCHANNEL_SUCCESS), &CAgoraMediaIOVideoCaptureDlg::OnEIDJoinChannelSuccess)
	ON_MESSAGE(WM_MSGID(EID_LEAVE_CHANNEL), &CAgoraMediaIOVideoCaptureDlg::OnEIDLeaveChannel)
	ON_MESSAGE(WM_MSGID(EID_USER_JOINED), &CAgoraMediaIOVideoCaptureDlg::OnEIDUserJoined)
	ON_MESSAGE(WM_MSGID(EID_USER_OFFLINE), &CAgoraMediaIOVideoCaptureDlg::OnEIDUserOffline)
	ON_MESSAGE(WM_MSGID(EID_REMOTE_VIDEO_STATE_CHANED), &CAgoraMediaIOVideoCaptureDlg::OnEIDRemoteVideoStateChanged)
	//ON_BN_CLICKED(IDC_BUTTON_START_CAPUTRE, &CAgoraMediaIOVideoCaptureDlg::OnClickedButtonStartCaputre)
	ON_BN_CLICKED(IDC_BUTTON_JOINCHANNEL, &CAgoraMediaIOVideoCaptureDlg::OnClickedButtonJoinchannel)
	ON_CBN_SELCHANGE(IDC_COMBO_CAPTURE_VIDEO_DEVICE, &CAgoraMediaIOVideoCaptureDlg::OnSelchangeComboCaptureVideoDevice)
	ON_CBN_SELCHANGE(IDC_CMB_MEDIO_CAPTURETYPE, &CAgoraMediaIOVideoCaptureDlg::OnSelchangeCmbMedioCapturetype)
	ON_CBN_SELCHANGE(IDC_COMBO_SDKCAMERA, &CAgoraMediaIOVideoCaptureDlg::OnSelchangeComboSdkcamera)
	ON_CBN_SELCHANGE(IDC_COMBO_SDK_RESOLUTION, &CAgoraMediaIOVideoCaptureDlg::OnSelchangeComboSdkResolution)
END_MESSAGE_MAP()


//set control text from config.
void CAgoraMediaIOVideoCaptureDlg::InitCtrlText()
{
	m_staCaptureType.SetWindowText(mediaIOCaptureType);
	m_staSDKCamera.SetWindowText(mediaIOCaptureSDKCamera);
	m_staChannelName.SetWindowText(commonCtrlChannel);
	m_staCaputreVideo.SetWindowText(customVideoCaptureCtrlCaptureVideoDevice);
	m_btnJoinChannel.SetWindowText(commonCtrlJoinChannel);
}

/*
	create Agora RTC Engine and initialize context.set channel property.
*/
bool CAgoraMediaIOVideoCaptureDlg::InitAgora()
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
	//initialize audio device manager
	m_videoDeviceManager = new AVideoDeviceManager(m_rtcEngine);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("initialize video device manager"));
	//enumera cameras
	m_lpVideoCollection = (*m_videoDeviceManager)->enumerateVideoDevices();
	if (m_lpVideoCollection) {
		for (int i = 0; i < m_lpVideoCollection->getCount(); ++i) {
			char szDeviceId[MAX_DEVICE_ID_LENGTH] = { 0 }, szDeviceName[MAX_DEVICE_ID_LENGTH] = { 0 };
			m_lpVideoCollection->getDevice(i, szDeviceName, szDeviceId);
			m_cmbSDKCamera.InsertString(i, utf82cs(szDeviceName));
		}
	}
	m_cmbSDKCamera.SetCurSel(0);
	//start preview default camera
	m_rtcEngine->startPreview();
	
	EnableControl();
	return true;
}

/*
	stop and release agora rtc engine.
*/
void CAgoraMediaIOVideoCaptureDlg::UnInitAgora()
{
	if (m_rtcEngine) {
		if (m_joinChannel)
			m_joinChannel = !m_rtcEngine->leaveChannel();
		ResumeStatus();
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
void CAgoraMediaIOVideoCaptureDlgEngineEventHandler::onRemoteVideoStateChanged(uid_t uid, REMOTE_VIDEO_STATE state, REMOTE_VIDEO_STATE_REASON reason, int elapsed)
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
BOOL CAgoraMediaIOVideoCaptureDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	
	//Get exe path
	TCHAR szFile[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, szFile, MAX_PATH);
	CString strPath = szFile;
	int pos = strPath.ReverseFind(_T('\\'));
	strPath = strPath.Mid(0, pos + 1);
	//screen.yuv path
	strPath += _T("screen.yuv");
	//video size is external_screen_w * external_screen_h * 3 / 2
	screenBuffer = new uint8_t[external_screen_w * external_screen_h * 3 / 2];
	//read screen buffer, yuv420sp
	_tfopen_s(&m_screenFile, strPath.GetBuffer(), L"rb");
	if (m_screenFile)
		fread(screenBuffer, 1, external_screen_w * external_screen_h * 3 / 2, m_screenFile);

	m_localVideoWnd.Create(NULL, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CRect(0, 0, 1, 1), this, ID_BASEWND_VIDEO + 100);
	RECT rcArea;
	m_staVideoArea.GetClientRect(&rcArea);
	m_localVideoWnd.MoveWindow(&rcArea);
	//create and initialize video capture object.
	m_agVideoCaptureDevice.Create();
	ResumeStatus();
	int i = 0;
	m_cmbCaptureType.InsertString(i++, mediaIOCaptureTypeSDKCamera);
	m_cmbCaptureType.InsertString(i++, mediaIOCaptureTypeSDKScreen);
	m_cmbCaptureType.InsertString(i++, mediaIOCaptureCamera);
	m_cmbCaptureType.InsertString(i++, mediaIOCaptureScreen);
	m_cmbCaptureType.SetCurSel(0);
	m_preCaptureType = VIDEO_SOURCE_SDK_CAMERA;
	
	// enumerate video encoder configuration. Add any resolution and fps as you need.
	
	i = 0;
	encoderConfigs[i].dimensions.width  = 640;
	encoderConfigs[i].dimensions.height = 360;
	encoderConfigs[i++].frameRate = FRAME_RATE_FPS_15;

	encoderConfigs[i].dimensions.width = 640;
	encoderConfigs[i].dimensions.height = 480;
	encoderConfigs[i++].frameRate = FRAME_RATE_FPS_15;

	encoderConfigs[i].dimensions.width = 960;
	encoderConfigs[i].dimensions.height = 540;
	encoderConfigs[i++].frameRate = FRAME_RATE_FPS_15;

	encoderConfigs[i].dimensions.width = 1280;
	encoderConfigs[i].dimensions.height = 720;
	encoderConfigs[i++].frameRate = FRAME_RATE_FPS_15;
	
	for (int i = 0; i < ENCODER_CONFIG_COUNT; ++i) {
		CString strInfo;
		strInfo.Format(_T("%dx%d %dfps"),
			encoderConfigs[i].dimensions.width,
			encoderConfigs[i].dimensions.height,
			encoderConfigs[i].frameRate);
		m_cmbSDKResolution.InsertString(i, strInfo);
	}
	m_cmbSDKResolution.SetCurSel(0);
	
	return TRUE;
}


// update window view and control.
void CAgoraMediaIOVideoCaptureDlg::UpdateViews()
{
	// render local video
	RenderLocalVideo();
	// enumerate device and show.
	UpdateDevice();
}

// enumerate device and show device in combobox.
void CAgoraMediaIOVideoCaptureDlg::UpdateDevice()
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
void CAgoraMediaIOVideoCaptureDlg::ResumeStatus()
{
	m_videoSouce.Stop();
	m_lstInfo.ResetContent();
	InitCtrlText();
	EnableCaputure(FALSE);
	m_joinChannel = false;
	m_initialize = false;
	m_remoteJoined = false;
	m_extenalCaptureVideo = false;
	m_edtChannel.SetWindowText(_T(""));
}

// start or stop capture.
// if bEnable is true start capture otherwise stop capture.
void CAgoraMediaIOVideoCaptureDlg::EnableCaputure(BOOL bEnable)
{
	//if (bEnable == (BOOL)!m_extenalCaptureVideo)return;

	int nIndex = m_cmbVideoResoliton.GetCurSel();
	if (bEnable)
	{
		VIDEOINFOHEADER videoInfo;
		//create video capture filter.
		m_agVideoCaptureDevice.CreateCaptureFilter();
		//select video capture type.
		m_agVideoCaptureDevice.SelectMediaCap(nIndex == -1 ? 0 : nIndex);
		m_agVideoCaptureDevice.GetCurrentVideoCap(&videoInfo);

		CAgVideoBuffer::GetInstance()->SetVideoFormat(&videoInfo.bmiHeader);
		
		//set video encoder configuration.
		m_externalCameraConfig.dimensions.width = videoInfo.bmiHeader.biWidth;
		m_externalCameraConfig.dimensions.height = videoInfo.bmiHeader.biHeight;
		m_externalCameraConfig.frameRate = (FRAME_RATE)(10000000ll / videoInfo.AvgTimePerFrame);
		m_rtcEngine->setVideoEncoderConfiguration(m_externalCameraConfig);
		//start video capture.
		m_agVideoCaptureDevice.Start();
	}
	else {
		//video capture stop.
		m_agVideoCaptureDevice.Stop();
		//remove video capture filter.
		m_agVideoCaptureDevice.RemoveCaptureFilter();
		
	}
}

/*
	set up canvas and local video view.
*/
void CAgoraMediaIOVideoCaptureDlg::RenderLocalVideo()
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
void CAgoraMediaIOVideoCaptureDlg::OnShowWindow(BOOL bShow, UINT nStatus)
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

//The JoinChannel button's click handler.
//This function either joins or leaves the channel
void CAgoraMediaIOVideoCaptureDlg::OnClickedButtonJoinchannel()
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
LRESULT CAgoraMediaIOVideoCaptureDlg::OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam)
{
	m_joinChannel = true;
	m_btnJoinChannel.EnableWindow(TRUE);
	m_btnJoinChannel.SetWindowText(commonCtrlLeaveChannel);
	CString strInfo;
	strInfo.Format(_T("%s:join success, uid=%u"), getCurrentTime(), wParam);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	m_localVideoWnd.SetUID(wParam);
	//notify parent window
	::PostMessage(GetParent()->GetSafeHwnd(), WM_MSGID(EID_JOINCHANNEL_SUCCESS), TRUE, 0);
	//CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)PushVideoFrameTrhead, this, 0, NULL);

	return 0;
}

//EID_LEAVE_CHANNEL message window handler.
LRESULT CAgoraMediaIOVideoCaptureDlg::OnEIDLeaveChannel(WPARAM wParam, LPARAM lParam)
{

	m_joinChannel = false;
	m_btnJoinChannel.SetWindowText(commonCtrlJoinChannel);

	CString strInfo;
	strInfo.Format(_T("leave channel success %s"), getCurrentTime());
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	::PostMessage(GetParent()->GetSafeHwnd(), WM_MSGID(EID_JOINCHANNEL_SUCCESS), FALSE, 0);
	return 0;
}

//EID_USER_JOINED message window handler.
LRESULT CAgoraMediaIOVideoCaptureDlg::OnEIDUserJoined(WPARAM wParam, LPARAM lParam)
{
	CString strInfo;
	strInfo.Format(_T("%u joined"), wParam);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	return 0;
}


//EID_USER_OFFLINE message window handler.
LRESULT CAgoraMediaIOVideoCaptureDlg::OnEIDUserOffline(WPARAM wParam, LPARAM lParam)
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
LRESULT CAgoraMediaIOVideoCaptureDlg::OnEIDRemoteVideoStateChanged(WPARAM wParam, LPARAM lParam)
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

IMPLEMENT_DYNAMIC(CAgoraMediaIOVideoCaptureDlg, CDialogEx)

CAgoraMediaIOVideoCaptureDlg::CAgoraMediaIOVideoCaptureDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_CUSTOM_CAPTURE_MEDIA_IO_VIDEO, pParent)
{
}

CAgoraMediaIOVideoCaptureDlg::~CAgoraMediaIOVideoCaptureDlg()
{
	EnableCaputure(FALSE);
	m_videoSouce.Stop();
	if (m_screenFile) {
		fclose(m_screenFile);
		m_screenFile = NULL;
	}

	if (screenBuffer) {
		delete[] screenBuffer;
		screenBuffer = NULL;
	}
}

void CAgoraMediaIOVideoCaptureDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_VIDEO, m_staVideoArea);
	DDX_Control(pDX, IDC_STATIC_CHANNELNAME, m_staChannelName);
	DDX_Control(pDX, IDC_STATIC_CAPTUREDEVICE, m_staCaputreVideo);
	DDX_Control(pDX, IDC_EDIT_CHANNELNAME, m_edtChannel);
	DDX_Control(pDX, IDC_BUTTON_JOINCHANNEL, m_btnJoinChannel);
	DDX_Control(pDX, IDC_COMBO_CAPTURE_VIDEO_DEVICE, m_cmbVideoDevice);
	DDX_Control(pDX, IDC_COMBO_CAPTURE_VIDEO_TYPE, m_cmbVideoResoliton);
	DDX_Control(pDX, IDC_LIST_INFO_BROADCASTING, m_lstInfo);
	DDX_Control(pDX, IDC_CMB_MEDIO_CAPTURETYPE, m_cmbCaptureType);
	DDX_Control(pDX, IDC_STATIC_SDKCAMERA, m_staSDKCamera);
	DDX_Control(pDX, IDC_STATIC_CAPTURE_TYPE, m_staCaptureType);
	DDX_Control(pDX, IDC_COMBO_SDKCAMERA, m_cmbSDKCamera);
	DDX_Control(pDX, IDC_COMBO_SDK_RESOLUTION, m_cmbSDKResolution);
}

//Enumerates the video capture devices and types, 
//and inserts them into the ComboBox
void CAgoraMediaIOVideoCaptureDlg::OnSelchangeComboCaptureVideoDevice()
{
	TCHAR	szDevicePath[MAX_PATH] = { 0 };
	SIZE_T	nPathLen = MAX_PATH;
	int		nSel = m_cmbVideoDevice.GetCurSel();

	VIDEOINFOHEADER		vidInfoHeader;
	CString				strInfo;
	CString				strCompress;
	//get current device name.
	m_cmbVideoResoliton.ResetContent();

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
		m_cmbVideoResoliton.InsertString(nIndex, strInfo);
	}
	m_cmbVideoResoliton.SetCurSel(0);
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
void CAgoraMediaIOVideoCaptureDlgEngineEventHandler::onJoinChannelSuccess(const char* channel, uid_t uid, int elapsed)
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
void CAgoraMediaIOVideoCaptureDlgEngineEventHandler::onUserJoined(uid_t uid, int elapsed)
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
void CAgoraMediaIOVideoCaptureDlgEngineEventHandler::onUserOffline(uid_t uid, USER_OFFLINE_REASON_TYPE reason)
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
void CAgoraMediaIOVideoCaptureDlgEngineEventHandler::onLeaveChannel(const RtcStats& stats)
{
	if (m_hMsgHanlder) {
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_LEAVE_CHANNEL), 0, 0);
	}
}

BOOL CAgoraMediaIOVideoCaptureDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) {
		return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}


void CAgoraMediaIOVideoCaptureDlg::OnSelchangeCmbMedioCapturetype()
{
	EnableControl();
	int sel = m_cmbCaptureType.GetCurSel();
	if (sel == m_preCaptureType) {
		return;
	}

	if (m_preCaptureType == VIDEO_SOURCE_CUSTOM_CAMERA
		|| m_preCaptureType == VIDEO_SOURCE_CUSTOM_SCREEM) {
		m_videoSouce.ResetConsumeEvent();
		EnableCaputure(FALSE);
	}else if (m_preCaptureType == VIDEO_SOURCE_SDK_SCREEN) {
		m_rtcEngine->stopScreenCapture();
	}


	switch (sel)
	{
	case VIDEO_SOURCE_SDK_CAMERA://sdk camera
	{	
		m_rtcEngine->startPreview();
	}
		break;
	case VIDEO_SOURCE_SDK_SCREEN://sdk screen
	{
		agora::rtc::Rectangle screenRect;
		RECT rc = { 0 };
		GetDesktopWindow()->GetWindowRect(&rc);
		screenRect = { rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top };

		ScreenCaptureParameters capParam;
		capParam.dimensions.width = screenRect.width;
		capParam.dimensions.height = screenRect.height;
		m_rtcEngine->startScreenCaptureByScreenRect(screenRect, screenRect, capParam);
		m_rtcEngine->startPreview();
	}
		break;
	case VIDEO_SOURCE_CUSTOM_CAMERA://custom camera
	{
		if (m_preCaptureType == VIDEO_SOURCE_SDK_CAMERA
			|| m_preCaptureType == VIDEO_SOURCE_SDK_SCREEN) {
			m_rtcEngine->enableLocalVideo(false);
		}
		
		//set video information
		EnableCaputure(TRUE);
		//CAgVideoBuffer::GetInstance()->reset();
		m_videoSouce.SetParameters( m_externalCameraConfig.dimensions.width,
			m_externalCameraConfig.dimensions.height, 0, m_externalCameraConfig.frameRate);
		m_videoSouce.SetVideoCaptureType(VIDEO_CAPTURE_CAMERA);
		m_rtcEngine->setVideoSource(&m_videoSouce);
		
		m_videoSouce.SetConsumeEvent();
		if (m_preCaptureType == VIDEO_SOURCE_SDK_CAMERA
			|| m_preCaptureType == VIDEO_SOURCE_SDK_SCREEN)
			m_rtcEngine->enableLocalVideo(true);
	}
		break;
	case VIDEO_SOURCE_CUSTOM_SCREEM://custom screen
	{
		//set video source capture type
		m_videoSouce.SetVideoCaptureType(VIDEO_CAPTURE_SCREEN);
		//set screen contennt type(encoder type)
		m_videoSouce.SetVideoHintContent(CONTENT_HINT_DETAILS);
		
		//set video source parameter
		m_videoSouce.SetParameters( external_screen_w, external_screen_h, 0, external_screen_fps);
		m_rtcEngine->setVideoSource(&m_videoSouce);
		CAgVideoBuffer::GetInstance()->writeBuffer(screenBuffer, external_screen_w * external_screen_h * 3 / 2, GetTickCount());
		//active external screen capture thread
		m_videoSouce.SetConsumeEvent();
		
		//set video encoder configuration
		VideoEncoderConfiguration config;
		config.dimensions.width = external_screen_w;
		config.dimensions.height = external_screen_h;
		m_rtcEngine->setVideoEncoderConfiguration(config);
	}
		break;
	default:
		break;
	}

	m_preCaptureType = (VIDEO_SOURCE_CAPTURE_TYPE)sel;
}

void CAgoraMediaIOVideoCaptureDlg::EnableControl()
{
	int sel = m_cmbCaptureType.GetCurSel();

	m_cmbSDKCamera.EnableWindow(sel == VIDEO_SOURCE_SDK_CAMERA);
	m_cmbSDKResolution.EnableWindow(sel == VIDEO_SOURCE_SDK_CAMERA);

	m_cmbVideoDevice.EnableWindow(sel == VIDEO_SOURCE_CUSTOM_CAMERA);
	m_cmbVideoResoliton.EnableWindow(sel == VIDEO_SOURCE_CUSTOM_CAMERA);
}


void CAgoraMediaIOVideoCaptureDlg::OnSelchangeComboSdkResolution()
{
	int sel = m_cmbSDKResolution.GetCurSel();
	if (sel < 0)
		return;
	m_rtcEngine->setVideoEncoderConfiguration(encoderConfigs[sel]);
}



void CAgoraMediaIOVideoCaptureDlg::OnSelchangeComboSdkcamera()
{
	m_lpVideoCollection = (*m_videoDeviceManager)->enumerateVideoDevices();
	CString strName;
	m_cmbSDKCamera.GetWindowText(strName);
	if (strName.IsEmpty())
		return;

	if (m_lpVideoCollection) {
		for (int i = 0; i < m_lpVideoCollection->getCount(); ++i) {
			char szDeviceId[MAX_DEVICE_ID_LENGTH] = { 0 }, szDeviceName[MAX_DEVICE_ID_LENGTH] = { 0 };
			m_lpVideoCollection->getDevice(i, szDeviceName, szDeviceId);

			if (strName.Compare(utf82cs(szDeviceName)) == 0) {
				(*m_videoDeviceManager)->setDevice(szDeviceId);
				break;
			 }
		
		}
	}
}

