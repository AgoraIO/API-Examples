// CLocalVideoTranscodingDlg.cpp: 
//

#include "stdafx.h"
#include "APIExample.h"
#include "CLocalVideoTranscodingDlg.h"
#include "afxdialogex.h"

// CLocalVideoTranscodingDlg

IMPLEMENT_DYNAMIC(CLocalVideoTranscodingDlg, CDialogEx)

CLocalVideoTranscodingDlg::CLocalVideoTranscodingDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_LOCAL_VIDEO_TRANSCODING, pParent)
{

}

CLocalVideoTranscodingDlg::~CLocalVideoTranscodingDlg()
{
}

void CLocalVideoTranscodingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_INFO_BROADCASTING, m_lstInfo);
	DDX_Control(pDX, IDC_EDIT_CHANNELNAME, m_edtChannel);
	DDX_Control(pDX, IDC_BUTTON_JOINCHANNEL, m_btnJoinChannel);
	DDX_Control(pDX, IDC_STATIC_VIDEO, m_staVideoArea);
	DDX_Control(pDX, IDC_STATIC_CHANNELNAME, m_staChannel);
	DDX_Control(pDX, IDC_STATIC_Cameras, m_staCamra);
	DDX_Control(pDX, IDC_COMBO_CAMERAS, m_cmbCamera);
	DDX_Control(pDX, IDC_CHECK_VIRTUAL_BG, m_chkVirtualBg);
}


BEGIN_MESSAGE_MAP(CLocalVideoTranscodingDlg, CDialogEx)
	ON_MESSAGE(WM_MSGID(EID_LEAVE_CHANNEL), &CLocalVideoTranscodingDlg::OnEIDLeaveChannel)
	ON_MESSAGE(WM_MSGID(EID_USER_JOINED), &CLocalVideoTranscodingDlg::OnEIDJoinChannelSuccess)
	ON_BN_CLICKED(IDC_BUTTON_JOINCHANNEL, &CLocalVideoTranscodingDlg::OnBnClickedButtonJoinchannel)
	ON_WM_SHOWWINDOW()
	ON_CBN_SELCHANGE(IDC_COMBO_CAMERAS, &CLocalVideoTranscodingDlg::OnSelchangeComboCameras)
	ON_BN_CLICKED(IDC_CHECK_VIRTUAL_BG, &CLocalVideoTranscodingDlg::OnBnClickedCheckVirtualBg)
END_MESSAGE_MAP()


// CLocalVideoTranscodingDlg

BOOL CLocalVideoTranscodingDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	InitCtrlText();
	for (int i = 0; i < 2; ++i) {
		m_videoWnds[i].Create(NULL, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CRect(0, 0, 1, 1), &m_staVideoArea, i);
		//set window background color.
		m_videoWnds[i].SetFaceColor(RGB(0x58, 0x58, 0x58));
	}
	ResumeStatus();

	CString strPath = GetExePath() + _T("\\agora.png");
	m_imgPng = cs2utf8(strPath);

	strPath = GetExePath() + _T("\\agora.jpg");
	m_imgJpg = cs2utf8(strPath);
	return TRUE;  // return TRUE unless you set the focus to a control
}


void CLocalVideoTranscodingDlg::OnBnClickedButtonJoinchannel()
{
	if (!m_joinChannel) {
		CString strChannelName;
		m_edtChannel.GetWindowText(strChannelName);
		std::string szChannelId = cs2utf8(strChannelName);
		if (strChannelName.IsEmpty()) {
			AfxMessageBox(_T("Fill channel name first"));
			return;
		}
		CString strInfo;
		agora::rtc::Rectangle rect;
		auto params = agora::rtc::ScreenCaptureParameters(640, 360, 15, 800);

		RECT rc;		
		::GetWindowRect(GetDesktopWindow()->GetSafeHwnd(), &rc);
		rect = { rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top };

		
		//primary camera configuration
		CameraCapturerConfiguration config;
		config.format.width = 640;
		config.format.height = 360;
		config.format.fps = 15;
		//get selected camera device id
		char* buffer = new char[512] {0};
		for (UINT i = 0; i < m_vecCameraInfos.size(); i++)
		{
			LOCALVIDEOTRANSCODING_CAMERAINFO info = m_vecCameraInfos[i];
			CString strName;
			m_cmbCamera.GetWindowText(strName);
			if (info.deviceName.compare(cs2utf8(strName)) == 0) {
				strcpy_s(buffer, 512, info.deviceId.c_str());
				config.deviceId = buffer;
				break;
			}
		}

		//start primary camera capture
		int ret = m_rtcEngine->startCameraCapture(VIDEO_SOURCE_CAMERA_PRIMARY, config);
		delete[] buffer;
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("start primary camera capture"));

		//start Screen capture
		ret = m_rtcEngine->startScreenCaptureByScreenRect(rect, rect, params);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("start Screen capture"));
		int i = 0;
		//screen
		stream_infos[i].sourceType = VIDEO_SOURCE_SCREEN_PRIMARY;
	
		stream_infos[i].x = 0;
		stream_infos[i].y = 0;
		stream_infos[i].width = 1280;
		stream_infos[i].height = 720;
		stream_infos[i].mirror = false;
		stream_infos[i].zOrder = 1;
		++i;
		//camera
		
		stream_infos[i].sourceType = VIDEO_SOURCE_CAMERA_PRIMARY;
		
		stream_infos[i].x = 0;
		stream_infos[i].y = 360;
		stream_infos[i].width = 640;
		stream_infos[i].height = 360;
		stream_infos[i].mirror = true;
		stream_infos[i].zOrder = 2;
		++i;
		
		//png imge
		
		stream_infos[i].sourceType = VIDEO_SOURCE_RTC_IMAGE_PNG;
		
		stream_infos[i].x = 0;
		stream_infos[i].y = 0;
		stream_infos[i].width = 60;
		stream_infos[i].height = 60;
		stream_infos[i].imageUrl = m_imgPng.c_str();
		stream_infos[i].mirror = false;
		stream_infos[i].zOrder = 3;
		++i;

		//jpg image
		
		stream_infos[i].sourceType = VIDEO_SOURCE_RTC_IMAGE_JPEG;
		
		stream_infos[i].x = 640 - 64;
		stream_infos[i].y = 180 - 64;
		stream_infos[i].width = 64;
		stream_infos[i].height = 64;
		stream_infos[i].imageUrl = m_imgJpg.c_str();
		stream_infos[i].mirror = false;
		stream_infos[i].zOrder = 4;
		++i;

		//video encoder configuration
		agora::rtc::VideoEncoderConfiguration encoder_config;
		encoder_config.codecType = agora::rtc::VIDEO_CODEC_H264;
		encoder_config.dimensions = { 1280, 720 };
		encoder_config.bitrate = 2000;
		encoder_config.frameRate = 15;
		
		// local transcoder configuration
		agora::rtc::LocalTranscoderConfiguration transcoder_config;
		transcoder_config.streamCount = i;
		transcoder_config.videoInputStreams = stream_infos;
		transcoder_config.videoOutputConfiguration = encoder_config;
		ret = m_rtcEngine->startLocalVideoTranscoder(transcoder_config);
		m_lstInfo.InsertString(m_lstInfo.GetCount() , _T("start local video Transcoder"));

		//setup local video in the engine to canvas.
		agora::rtc::VideoCanvas canvas;
		canvas.renderMode = media::base::RENDER_MODE_FIT;
		canvas.uid = 0;
		canvas.view = m_videoWnds[0].GetSafeHwnd();
		canvas.sourceType = VIDEO_SOURCE_TRANSCODED;
		canvas.mirrorMode = VIDEO_MIRROR_MODE_DISABLED;
		m_rtcEngine->setupLocalVideo(canvas);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("setupLocalVideo"));

		//start preview
		m_rtcEngine->startPreview();
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("start preview"));
		

		//join channel
		agora::rtc::ChannelMediaOptions op;
		op.publishTranscodedVideoTrack = true;
		op.channelProfile = CHANNEL_PROFILE_LIVE_BROADCASTING;
		op.clientRoleType = agora::rtc::CLIENT_ROLE_BROADCASTER;
		if (0 == m_rtcEngine->joinChannel(APP_TOKEN, szChannelId.data(), 0, op)) {
			strInfo.Format(_T("join channel %s, use ChannelMediaOptions"), strChannelName);
			m_lstInfo.InsertString(m_lstInfo.GetCount() , strInfo);
			m_conn_camera = DEFAULT_CONNECTION_ID;
		}

		m_btnJoinChannel.SetWindowText(commonCtrlLeaveChannel);
	}
	else {

		//leave channel
		m_rtcEngine->leaveChannel();
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("leave channel"));

		// stop local video transcoder
		m_rtcEngine->stopLocalVideoTranscoder();
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("stop local video transcoder"));

		//stop screen capture
		m_rtcEngine->stopScreenCapture();
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("stop screen capture"));
		//stop screen capture
		m_rtcEngine->stopCameraCapture(VIDEO_SOURCE_CAMERA_PRIMARY);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("stop camera capture"));
		//stop preview
		//m_rtcEngine->stopPreview();
		//m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("stop preview"));
		

		m_btnJoinChannel.SetWindowText(commonCtrlJoinChannel);
		m_videoWnds[0].Invalidate();

	}
	m_joinChannel = !m_joinChannel;
}
//Initialize the Agora SDK
bool CLocalVideoTranscodingDlg::InitAgora()
{
	//create Agora RTC engine
	m_rtcEngine = createAgoraRtcEngine();
	if (!m_rtcEngine) {
		m_lstInfo.InsertString(m_lstInfo.GetCount() - 1, _T("createAgoraRtcEngine failed"));
		return false;
	}
	m_eventHandler.SetId(0);
	m_eventHandler.SetMsgReceiver(m_hWnd);
	agora::rtc::RtcEngineContext context;
	std::string strAppID = GET_APP_ID;
	context.appId = strAppID.c_str();
	context.eventHandler = &m_eventHandler;
	//set channel profile in the engine to the CHANNEL_PROFILE_LIVE_BROADCASTING.
	context.channelProfile = CHANNEL_PROFILE_LIVE_BROADCASTING;
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
	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("initialize success"));
	m_rtcEngine->loadExtensionProvider("libagora_segmentation_extension.dll");
	m_rtcEngine->enableExtension("agora_segmentation", "PortraitSegmentation", true, agora::media::PRIMARY_CAMERA_SOURCE);
	//enable video in the engine.
	m_rtcEngine->enableVideo();
	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("enable video"));
	
	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("live broadcasting"));
	//set client role in the engine to the CLIENT_ROLE_BROADCASTER.
	m_rtcEngine->setClientRole(agora::rtc::CLIENT_ROLE_BROADCASTER);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("setClientRole broadcaster"));
	
	//initliaze all available cameras
	videoDeviceManager = new AVideoDeviceManager(m_rtcEngine);
	IVideoDeviceCollection* collections = (*videoDeviceManager)->enumerateVideoDevices();
	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("initialize video device manager"));

	int count = collections->getCount();
	if (count < 0) {
		m_lstInfo.InsertString(m_lstInfo.GetCount() - 1, _T("no video devices available."));
		return true;
	}
	// show cameras information
	CString strCameras;
	strCameras.Format(_T("Get cameras inforamation, count:%d"), count);
	m_lstInfo.InsertString(m_lstInfo.GetCount() - 1, strCameras);
	for (int i = 0; i < count; ++i) {
		char deviceId[512] = { 0 };
		char deviceName[512] = { 0 };
		// Get camera information
		collections->getDevice(i, deviceName, deviceId);
		m_cmbCamera.InsertString(i, utf82cs(deviceName));

		LOCALVIDEOTRANSCODING_CAMERAINFO cameraInfo;
		cameraInfo.deviceId = deviceId;
		cameraInfo.deviceName = deviceName;
		m_vecCameraInfos.push_back(cameraInfo);
	}
	m_cmbCamera.SetCurSel(0);
	return true;
}
//UnInitialize the Agora SDK
void CLocalVideoTranscodingDlg::UnInitAgora()
{
	if (m_rtcEngine) {
		m_vecCameraInfos.clear();

		if (m_joinChannel) {
			//leave channel primary camera
			m_joinChannel = !m_rtcEngine->leaveChannel();
			m_lstInfo.InsertString(m_lstInfo.GetCount() - 1, _T("leaveChannel"));
			//stop local video transcoder
			m_rtcEngine->stopLocalVideoTranscoder();
			m_lstInfo.InsertString(m_lstInfo.GetCount() - 1, _T("stop local video transcoder"));

			//stop screen capture
			m_rtcEngine->stopScreenCapture();
			m_lstInfo.InsertString(m_lstInfo.GetCount() - 1, _T("stop screen capture"));
			m_conn_camera = 0;
			m_joinChannel = false;
			m_btnJoinChannel.SetWindowText(commonCtrlJoinChannel);
		}

		//stop preview in the engine.
		m_rtcEngine->stopPreview();
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("stopPreview"));

		//release engine.
		if (m_initialize) {
			m_rtcEngine->release(true);
			m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("release rtc engine"));
		}
		m_rtcEngine = NULL;
	}
}


//Initialize the Ctrl Text.
void CLocalVideoTranscodingDlg::InitCtrlText()
{
	m_staCamra.SetWindowText(PerCallTestCtrlCamera);//MultiVideoSourceCtrlUnPublish
	m_staChannel.SetWindowText(commonCtrlChannel);
	m_btnJoinChannel.SetWindowText(commonCtrlJoinChannel);
	m_chkVirtualBg.SetWindowText(localVideoTranscodingVirtualBg);
}

// resume window status.
void CLocalVideoTranscodingDlg::ResumeStatus()
{
	m_staVideoArea.ShowWindow(SW_SHOW);
	RECT rcArea;
	m_staVideoArea.GetClientRect(&rcArea);
	
	m_videoWnds[0].MoveWindow(&rcArea);
	
	m_videoWnds[0].ShowWindow(SW_SHOW);
	m_videoWnds[1].ShowWindow(SW_HIDE);
	
	m_lstInfo.ResetContent();
	m_cmbCamera.ResetContent();
	m_btnJoinChannel.EnableWindow(TRUE);
	m_edtChannel.SetWindowText(_T(""));
	m_joinChannel = false;
	m_initialize = false;
}


LRESULT CLocalVideoTranscodingDlg::OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam)
{
	int cId = (int)wParam;

	CString strInfo;
	strInfo.Format(_T("join channel success, uid=%u, cId=%d"), wParam, cId);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	return 0;
}

//EID_LEAVE_CHANNEL message window handler.
LRESULT CLocalVideoTranscodingDlg::OnEIDLeaveChannel(WPARAM wParam, LPARAM lParam)
{
	int cId = (int)wParam;
	
	m_btnJoinChannel.SetWindowText(commonCtrlJoinChannel);
	CString strInfo;
	strInfo.Format(_T("leave channel success"));
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	return 0;
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
void CLocalVideoTranscodingEventHandler::onJoinChannelSuccess(const char* channel, agora::rtc::uid_t uid, int elapsed)
{
	m_strChannel = channel;
	if (m_hMsgHanlder) {
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_JOINCHANNEL_SUCCESS), (WPARAM)uid, (LPARAM)m_Id);
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
void CLocalVideoTranscodingEventHandler::onUserJoined(agora::rtc::uid_t uid, int elapsed)
{
	if (m_hMsgHanlder) {
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_USER_JOINED), (WPARAM)uid, (LPARAM)m_Id);
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
void CLocalVideoTranscodingEventHandler::onUserOffline(agora::rtc::uid_t uid, agora::rtc::USER_OFFLINE_REASON_TYPE reason)
{
	if (m_hMsgHanlder) {
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_USER_OFFLINE), (WPARAM)uid, (LPARAM)m_Id);
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
void CLocalVideoTranscodingEventHandler::onLeaveChannel(const agora::rtc::RtcStats& stats)
{
	if (m_hMsgHanlder) {
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_LEAVE_CHANNEL), (WPARAM)m_Id, 0);
	}
}

void CLocalVideoTranscodingEventHandler::onRemoteVideoStateChanged(agora::rtc::uid_t uid, agora::rtc::REMOTE_VIDEO_STATE state, agora::rtc::REMOTE_VIDEO_STATE_REASON reason, int elapsed)
{
	if (m_hMsgHanlder) {
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_REMOTE_VIDEO_STATE_CHANGED), (WPARAM)uid, (LPARAM)m_Id);
	}
}



void CLocalVideoTranscodingDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	if (bShow) {
		//init control text.
		InitCtrlText();
	}
	else {
		//resume window status.
		ResumeStatus();
	}
}


void CLocalVideoTranscodingDlg::OnSelchangeComboCameras()
{
	//initliaze all available cameras
	int sel = m_cmbCamera.GetCurSel();
	IVideoDeviceCollection* collections = (*videoDeviceManager)->enumerateVideoDevices();
	char deviceId[512] = { 0 };
	char deviceName[512] = { 0 };
	// Get camera information
	collections->getDevice(sel, deviceName, deviceId);
	(*videoDeviceManager)->setDevice(deviceId);
}




void CLocalVideoTranscodingDlg::OnBnClickedCheckVirtualBg()
{
	if (m_rtcEngine == nullptr) {
		return;
	}
	int isCheck = m_chkVirtualBg.GetCheck();
	if (isCheck) {
		VirtualBackgroundSource virtualBackgroundSource;
		virtualBackgroundSource.color = 0;
		m_rtcEngine->enableVirtualBackground(true, virtualBackgroundSource, SegmentationProperty(), PRIMARY_CAMERA_SOURCE);
		CString strInfo;
		strInfo.Format(_T("enableVirtualBackground"));
		m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	}
	else {
		m_rtcEngine->enableVirtualBackground(false, VirtualBackgroundSource(), SegmentationProperty(), PRIMARY_CAMERA_SOURCE);
		CString strInfo;
		strInfo.Format(_T("disableVirtualBackground"));
		m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	}
}
