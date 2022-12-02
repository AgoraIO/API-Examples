// CAgoraMutilVideoSourceDlg.cpp: 
//

#include "stdafx.h"
#include "APIExample.h"
#include "CAgoraMutilVideoSourceDlg.h"
#include "afxdialogex.h"


// CAgoraMutilVideoSourceDlg 

IMPLEMENT_DYNAMIC(CAgoraMutilVideoSourceDlg, CDialogEx)

CAgoraMutilVideoSourceDlg::CAgoraMutilVideoSourceDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD, pParent)
{

}

CAgoraMutilVideoSourceDlg::~CAgoraMutilVideoSourceDlg()
{
}

void CAgoraMutilVideoSourceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_CAMERAS, m_cmbCameras);
	//DDX_Control(pDX, IDC_COMBO_Resolution, m_cmbResolution);
	DDX_Control(pDX, IDC_EDIT_CHANNELNAME, m_edtChannelName);
	DDX_Control(pDX, IDC_STATIC_VIDEO, m_videoArea);
	DDX_Control(pDX, IDC_BUTTON_JOINCHANNEL, m_btnJoinChannel);
	DDX_Control(pDX, IDC_LIST_INFO_BROADCASTING, m_lstInfo);
	DDX_Control(pDX, IDC_COMBO_CAMERAS2, m_cmbCamera2);
	DDX_Control(pDX, IDC_BUTTON_PUBLISH2, m_btnPublish2);
	DDX_Control(pDX, IDC_STATIC_Cameras, m_staCamera1);
	DDX_Control(pDX, IDC_STATIC_Cameras2, m_staCamera2);
	DDX_Control(pDX, IDC_STATIC_CHANNELNAME, m_staChannel);
	DDX_Control(pDX, IDC_BUTTON_CAMERA1, m_btnCapture1);
	DDX_Control(pDX, IDC_BUTTON_CAMERA2, m_btnCapture2);

	DDX_Control(pDX, IDC_STATIC_SCREEN, m_staScreen);
	DDX_Control(pDX, IDC_BUTTON_CAPTURE_SCREEN, m_btnScreenCapture);
	DDX_Control(pDX, IDC_BUTTON_PUBLISH_SCREEN, m_btnScreenPublish);
}


BEGIN_MESSAGE_MAP(CAgoraMutilVideoSourceDlg, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BUTTON_JOINCHANNEL, &CAgoraMutilVideoSourceDlg::OnBnClickedButtonJoinchannel)
	ON_MESSAGE(WM_MSGID(EID_JOINCHANNEL_SUCCESS), &CAgoraMutilVideoSourceDlg::OnEIDJoinChannelSuccess)
	ON_MESSAGE(WM_MSGID(EID_LEAVE_CHANNEL), &CAgoraMutilVideoSourceDlg::OnEIDLeaveChannel)
	ON_MESSAGE(WM_MSGID(EID_USER_JOINED), &CAgoraMutilVideoSourceDlg::OnEIDUserJoined)
	ON_MESSAGE(WM_MSGID(EID_USER_OFFLINE), &CAgoraMutilVideoSourceDlg::OnEIDUserOffline)
	ON_MESSAGE(WM_MSGID(EID_LOCAL_AUDIO_STATS), &CAgoraMutilVideoSourceDlg::OnEIDLocalAudioStats)
	ON_MESSAGE(WM_MSGID(EID_LOCAL_VIDEO_STATS), &CAgoraMutilVideoSourceDlg::OnEIDLocalVideoStats)
	ON_MESSAGE(WM_MSGID(EID_REMOTE_AUDIO_STATS), &CAgoraMutilVideoSourceDlg::OnEIDRemoteAudioStats)
	ON_MESSAGE(WM_MSGID(EID_REMOTE_VIDEO_STATS), &CAgoraMutilVideoSourceDlg::OnEIDRemoteVideoStats)
	ON_BN_CLICKED(IDC_BUTTON_PUBLISH2, &CAgoraMutilVideoSourceDlg::OnBnClickedButtonPublish2)
	ON_BN_CLICKED(IDC_BUTTON_CAMERA1, &CAgoraMutilVideoSourceDlg::OnBnClickedButtonCamera1)
	ON_BN_CLICKED(IDC_BUTTON_CAMERA2, &CAgoraMutilVideoSourceDlg::OnBnClickedButtonCamera2)
	ON_BN_CLICKED(IDC_BUTTON_CAPTURE_SCREEN, &CAgoraMutilVideoSourceDlg::OnBnClickedButtonCaptureScreen)
	ON_BN_CLICKED(IDC_BUTTON_PUBLISH_SCREEN, &CAgoraMutilVideoSourceDlg::OnBnClickedButtonPublishScreen)
END_MESSAGE_MAP()


// CAgoraMutilVideoSourceDlg


BOOL CAgoraMutilVideoSourceDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	InitCtrlText();
	// initialize render wnd
	for (int i = 0; i < MAX_VIDEO_COUNT; ++i) {
		m_videoWnds[i].Create(NULL, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CRect(0, 0, 1, 1), this, IDC_MULTICALERAM_VIDEO + i);
		//set window background color.
		m_videoWnds[i].SetFaceColor(RGB(0x58, 0x58, 0x58));
		m_videoWnds[i].ShowWindow(SW_HIDE);
	}
	m_maxVideoCount = 4;
	ShowVideoWnds();
	return TRUE;  // return TRUE unless you set the focus to a control
}


void CAgoraMutilVideoSourceDlg::InitCtrlText()
{
	m_staChannel.SetWindowText(commonCtrlChannel);
	m_btnJoinChannel.SetWindowText(commonCtrlJoinChannel);

	// Camera 1
	m_staCamera1.SetWindowText(MultiVideoSourceCamera1);
	m_btnCapture1.SetWindowText(MultiVideoSourceStartCapture);

	// Camera 2
	m_staCamera2.SetWindowText(MultiVideoSourceCamera2);
	m_btnCapture2.SetWindowText(MultiVideoSourceStartCapture);
	m_btnPublish2.SetWindowText(MultiVideoSourcePublish);//MultiVideoSourceCtrlUnPublish

	// Screen Sharing
	m_staScreen.SetWindowText(MultiVideoSourceScreen);
	m_btnScreenCapture.SetWindowText(MultiVideoSourceStartCapture);
	m_btnScreenPublish.SetWindowTextW(MultiVideoSourcePublish);
}


void CAgoraMutilVideoSourceDlg::OnBnClickedButtonJoinchannel()
{
	CString strChannelName;
	m_edtChannelName.GetWindowText(strChannelName);
	std::string szChannelId = cs2utf8(strChannelName);
	if (!m_joinChannel) {
		if (strChannelName.IsEmpty()) {
			AfxMessageBox(_T("Fill channel name first"));
			return;
		}
		//If more than one camera are available,primary camera and secondary camera must be different
		if (m_vecCameraInfos.size() > 1 &&
			m_cmbCameras.GetCurSel() == m_cmbCamera2.GetCurSel()) {
			AfxMessageBox(_T("Camera2 need select a different camera"));
			return;
		}
		if (!m_bStartCapture1)
			OnBnClickedButtonCamera1();
		//joinchannelex option
		agora::rtc::ChannelMediaOptions optionsCamera;
		optionsCamera.autoSubscribeAudio = true;
		optionsCamera.autoSubscribeVideo = true;
		optionsCamera.publishMicrophoneTrack = true;
		optionsCamera.publishCameraTrack = true;
		optionsCamera.publishScreenTrack = false;
		optionsCamera.clientRoleType = CLIENT_ROLE_BROADCASTER;//broadcaster

		// join channel first camera
		m_rtcEngine->joinChannel(APP_TOKEN, szChannelId.data(), 1122, optionsCamera);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("joinChannel primary camera, use ChannelMediaOption"));

		m_btnJoinChannel.SetWindowText(commonCtrlLeaveChannel);
	}
	else {
		//leaveChannel primary camera
		if (m_bStartCapture1)
			OnBnClickedButtonCamera1();
		m_rtcEngine->leaveChannel();
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("leaveChannel primary camera"));

		m_btnJoinChannel.SetWindowText(commonCtrlJoinChannel);
	}

	m_joinChannel = !m_joinChannel;
}

void CAgoraMutilVideoSourceDlg::OnBnClickedButtonPublish2()
{
	CString strChannelName;
	m_edtChannelName.GetWindowText(strChannelName);
	std::string szChannelId = cs2utf8(strChannelName);
	if (!m_bScecondJoin) {

		if (m_vecCameraInfos.size() > 0) {
			connection.localUid = generateUid();
			m_camera2EventHandler.SetId(connection.localUid);
			m_camera2EventHandler.SetMsgReceiver(m_hWnd);

			if (!m_bStartCapture2)
				OnBnClickedButtonCamera2();
			//joinchannelex option
			agora::rtc::ChannelMediaOptions options2;
			options2.autoSubscribeAudio = false;
			options2.autoSubscribeVideo = false;
			options2.publishMicrophoneTrack = false;
			options2.publishCameraTrack = false;
			options2.publishSecondaryCameraTrack = true;
			options2.clientRoleType = CLIENT_ROLE_BROADCASTER;
			// joinChannelEx secondary camera capture(broadcaster)
			connection.channelId = szChannelId.data();
			int ret = m_rtcEngine->joinChannelEx(APP_TOKEN, connection, options2, &m_camera2EventHandler);
			CString str;
			str.Format(_T("joinChannelEx: %d"), ret);
			m_lstInfo.InsertString(m_lstInfo.GetCount(), str);

		}
		m_btnPublish2.SetWindowText(MultiCamearaStopPublish);
	}
	else {
		if (m_bStartCapture2)
			OnBnClickedButtonCamera2();
		//leaveChannel secondary camera
		connection.channelId = szChannelId.data();
		m_rtcEngine->leaveChannelEx(connection);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("leaveChannel secondary camera"));

		m_btnPublish2.SetWindowText(MultiVideoSourcePublish);
	}

	m_bScecondJoin = !m_bScecondJoin;
}

void CAgoraMutilVideoSourceDlg::ShowVideoWnds()
{
	m_videoArea.ShowWindow(SW_HIDE);
	int row = 2;
	int col = 2;

	if (m_maxVideoCount == 1) {
		row = 1;
		col = 1;
	}
	else  if (m_maxVideoCount == 2) {
		row = 1;
		col = 2;
	}
	else  if (m_maxVideoCount > 2 && m_maxVideoCount <= 4) {
		row = 2;
		col = 2;
	}
	else  if (m_maxVideoCount > 4 && m_maxVideoCount <= 6) {
		row = 2;
		col = 3;
	}
	else  if (m_maxVideoCount > 6 && m_maxVideoCount <= 9) {
		row = 3;
		col = 3;
	}
	else  if (m_maxVideoCount > 9 && m_maxVideoCount <= 12) {
		row = 3;
		col = 4;
	}
	else  if (m_maxVideoCount > 13 && m_maxVideoCount <= 16) {
		row = 4;
		col = 4;
	}

	RECT rcArea;
	m_videoArea.GetClientRect(&rcArea);
	int space = 1;

	int w = (rcArea.right - rcArea.left - space * (col - 1)) / col;
	int h = (rcArea.bottom - rcArea.top - space * (row - 1)) / row;

	for (int r = 0; r < row; r++) {
		for (int c = 0; c < col; c++) {
			int x = rcArea.left + (w + space) * c;
			int y = rcArea.top + (h + space) * r;
			int nIndex = r * col + c;
			m_videoWnds[nIndex].MoveWindow(x, y, w, h, TRUE);
			m_videoWnds[nIndex].ShowWindow(SW_SHOW);
			m_videoWnds[nIndex].SetParent(this);

			if (!m_videoWnds[nIndex].IsWindowVisible()) {
				m_videoWnds[nIndex].ShowWindow(SW_SHOW);
			}

			m_videoWnds[nIndex].ShowStatsInfo(TRUE, nIndex > 2);
		}
	}

	for (int i = row * col; i < MAX_VIDEO_COUNT; ++i) {
		m_videoWnds[i].ShowWindow(SW_HIDE);
	}
}


//resume window status
void CAgoraMutilVideoSourceDlg::ResumeStatus()
{
	InitCtrlText();

	m_bConnected = false;

	m_joinChannel = false;
	m_initialize = false;
	m_bScecondJoin = false;
	m_bScreenJoin = false;
	m_bStartCapture1 = false;
	m_bStartCapture2 = false;
	m_bStartScreenSharing = false;

	m_btnJoinChannel.EnableWindow(TRUE);
	m_btnScreenPublish.EnableWindow(FALSE);
	m_btnPublish2.EnableWindow(FALSE);
}

//Initialize the Agora SDK
bool CAgoraMutilVideoSourceDlg::InitAgora()
{
	//create Agora RTC engine

	agora::rtc::IRtcEngine* engine = createAgoraRtcEngine();
	m_rtcEngine = (IRtcEngineEx*)engine;
	if (!m_rtcEngine) {
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("createAgoraRtcEngine failed"));
		return false;
	}
	m_cameraEventHandler.SetId(0);
	m_cameraEventHandler.SetMsgReceiver(m_hWnd);
	agora::rtc::RtcEngineContext context;
	std::string strAppID = GET_APP_ID;
	context.appId = strAppID.c_str();
	context.eventHandler = &m_cameraEventHandler;
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

	int count = collections->getCount();
	if (count < 0) {
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("no video devices available."));
		return true;
	}

	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("enumerate video devices ."));
	// show cameras information
	CString strCameras;
	strCameras.Format(_T("Get cameras inforamation, count:%d"), count);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strCameras);
	for (int i = 0; i < count; ++i) {
		char deviceId[512] = { 0 };
		char deviceName[512] = { 0 };
		// Get camera information
		collections->getDevice(i, deviceName, deviceId);
		MULTIVIDEOSOURCE_CAMERAINFO cameraInfo;
		cameraInfo.deviceId = deviceId;
		cameraInfo.deviceName = deviceName;

		m_vecCameraInfos.push_back(cameraInfo);
		m_cmbCameras.InsertString(i, utf82cs(deviceName));
		m_cmbCamera2.InsertString(i, utf82cs(deviceName));
	}
	m_maxVideoCount = 4;

	m_cmbCameras.SetCurSel(0);
	if (m_vecCameraInfos.size() > 1) {
		m_cmbCamera2.SetCurSel(1);
	}
	return true;
}
//UnInitialize the Agora SDK
void CAgoraMutilVideoSourceDlg::UnInitAgora()
{
	if (m_rtcEngine) {
		if (m_joinChannel) {
			//leave channel primary camera
			m_joinChannel = !m_rtcEngine->leaveChannel();
			//stop primary camera capture
			m_rtcEngine->stopPrimaryCameraCapture();
			m_conn_camera = 0;
			m_joinChannel = false;
		}


		if (m_bScecondJoin) {
			//leaveChannel secondary camera
			m_rtcEngine->leaveChannelEx(connection);
			m_bScecondJoin = false;
		}
		if (m_bStartCapture2) {
			m_rtcEngine->stopSecondaryCameraCapture();
			m_bStartCapture2 = false;
		}

		if (m_bScreenJoin) {
			m_rtcEngine->stopScreenCapture();
			m_rtcEngine->leaveChannelEx(screenConnection);
			m_bScreenJoin = false;
		}

		m_rtcEngine->stopPreview();
		//release engine.
		m_rtcEngine->release(true);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("release rtc engine"));
		m_rtcEngine = NULL;
	}
}


void CAgoraMutilVideoSourceDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);
	ResumeStatus();
}

LRESULT CAgoraMutilVideoSourceDlg::OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam)
{
	int cId = (int)lParam;
	unsigned int uid = (unsigned int)wParam;
	CString strChannelName = utf82cs(m_cameraEventHandler.GetChannelName());

	if (lParam == 10086) {//second
		m_btnPublish2.EnableWindow(TRUE);
		// primary mute second
	}
	else {//primary
		m_btnJoinChannel.EnableWindow(TRUE);
	}

	CString strInfo;
	strInfo.Format(_T("join %s success, uid=%u, cId=%d"), strChannelName, wParam, uid);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	m_rtcEngine->muteRemoteAudioStream((uid_t)wParam, true);
	m_rtcEngine->muteRemoteVideoStream((uid_t)wParam, true);

	m_btnPublish2.EnableWindow(TRUE);
	m_btnScreenPublish.EnableWindow(TRUE);
	return 0;
}

//EID_LEAVE_CHANNEL message window handler.
LRESULT CAgoraMutilVideoSourceDlg::OnEIDLeaveChannel(WPARAM wParam, LPARAM lParam)
{

	CString strChannelName = utf82cs(m_cameraEventHandler.GetChannelName());
	unsigned int uid = (unsigned int)lParam;
	if (uid == connection.localUid) {
		CString strInfo;
		strInfo.Format(_T("leaveChannelEx:%s "), strChannelName);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	}
	else if(uid == screenConnection.localUid){
		CString strInfo;
		strInfo.Format(_T("leave screen channel:%s "), strChannelName);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	}
	else {
		m_btnJoinChannel.SetWindowText(commonCtrlJoinChannel);
		CString strInfo;
		strInfo.Format(_T("leave channel:%s "), strChannelName);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);


		m_btnPublish2.EnableWindow(FALSE);
		m_btnScreenPublish.EnableWindow(FALSE);
		if (m_bScecondJoin) {
			OnBnClickedButtonPublish2();
		}
		else if (m_bStartCapture2) {
			OnBnClickedButtonCamera2();
		}
		if (m_bScreenJoin) {
			OnBnClickedButtonPublishScreen();
		}
		else if (m_bStartScreenSharing) {
			OnBnClickedButtonCaptureScreen();
		}

		m_videoWnds[3].Reset();
	}

	return 0;
}

LRESULT CAgoraMutilVideoSourceDlg::OnEIDUserJoined(WPARAM wParam, LPARAM lParam)
{
	int uid = wParam;
	m_videoWnds[3].Reset();
	m_videoWnds[3].SetUID(uid);

	VideoCanvas canvas;
	canvas.uid = uid;
	canvas.view = m_videoWnds[3].GetSafeHwnd();
	canvas.renderMode = agora::media::base::RENDER_MODE_HIDDEN;
	m_rtcEngine->setupRemoteVideo(canvas);

	return 0;
}


LRESULT CAgoraMutilVideoSourceDlg::OnEIDUserOffline(WPARAM wParam, LPARAM lParam)
{
	int uid = wParam;
	if (m_videoWnds[3].GetUID() == uid) {
		m_videoWnds[3].SetUID(0);
		VideoCanvas canvas;
		canvas.uid = uid;
		m_rtcEngine->setupRemoteVideo(canvas);
		m_videoWnds[3].Reset();
	}
	return 0;
}


LRESULT CAgoraMutilVideoSourceDlg::OnEIDLocalVideoStats(WPARAM wParam, LPARAM lParam)
{
	LocalVideoStats* stats = reinterpret_cast<LocalVideoStats*>(wParam);
	VIDEO_SOURCE_TYPE sourceType = VIDEO_SOURCE_TYPE(lParam);

	if (sourceType == VIDEO_SOURCE_CAMERA_PRIMARY)
	{
		m_videoWnds[0].SetVideoStatsInfo(stats->encodedFrameWidth, stats->encodedFrameHeight,
			stats->sentFrameRate, stats->sentBitrate, stats->txPacketLossRate);
	}
	else if (sourceType == VIDEO_SOURCE_CAMERA_SECONDARY)
	{
		m_videoWnds[1].SetVideoStatsInfo(stats->encodedFrameWidth, stats->encodedFrameHeight,
			stats->sentFrameRate, stats->sentBitrate, stats->txPacketLossRate);
	}
	else if (sourceType == VIDEO_SOURCE_SCREEN)
	{
		m_videoWnds[2].SetVideoStatsInfo(stats->encodedFrameWidth, stats->encodedFrameHeight,
			stats->sentFrameRate, stats->sentBitrate, stats->txPacketLossRate);
	}
	delete stats;
	return 0;
}


LRESULT CAgoraMutilVideoSourceDlg::OnEIDLocalAudioStats(WPARAM wParam, LPARAM lParam)
{
	LocalAudioStats* stats = reinterpret_cast<LocalAudioStats*>(wParam);
	VIDEO_SOURCE_TYPE sourceType = VIDEO_SOURCE_TYPE(lParam);

	if (sourceType == VIDEO_SOURCE_CAMERA_PRIMARY)
	{
		m_videoWnds[0].SetAudioStatsInfo(stats->sentBitrate, stats->txPacketLossRate);
	}
	else if (sourceType == VIDEO_SOURCE_CAMERA_SECONDARY)
	{
		m_videoWnds[0].SetAudioStatsInfo(stats->sentBitrate, stats->txPacketLossRate);
	}
	else if (sourceType == VIDEO_SOURCE_SCREEN)
	{
		m_videoWnds[0].SetAudioStatsInfo(stats->sentBitrate, stats->txPacketLossRate);
	}
	delete stats;
	return 0;
}


LRESULT CAgoraMutilVideoSourceDlg::OnEIDRemoteVideoStats(WPARAM wParam, LPARAM lParam)
{
	RemoteVideoStats* stats = reinterpret_cast<RemoteVideoStats*>(wParam);

	if (stats->uid == m_videoWnds[3].GetUID()) {
		m_videoWnds[3].SetVideoStatsInfo(stats->width, stats->height,
			stats->decoderOutputFrameRate, stats->receivedBitrate, stats->packetLossRate, stats->delay);
	}
	return 0;
}


LRESULT CAgoraMutilVideoSourceDlg::OnEIDRemoteAudioStats(WPARAM wParam, LPARAM lParam)
{
	RemoteAudioStats* stats = reinterpret_cast<RemoteAudioStats*>(wParam);

	if (stats->uid == m_videoWnds[3].GetUID()) {
		m_videoWnds[3].SetAudioStatsInfo(stats->receivedBitrate, stats->audioLossRate, stats->jitterBufferDelay);
	}
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
void CMultiVideoSourceEventHandler::onJoinChannelSuccess(const char* channel, agora::rtc::uid_t uid, int elapsed)
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
void CMultiVideoSourceEventHandler::onUserJoined(agora::rtc::uid_t uid, int elapsed)
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
void CMultiVideoSourceEventHandler::onUserOffline(agora::rtc::uid_t uid, agora::rtc::USER_OFFLINE_REASON_TYPE reason)
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
void CMultiVideoSourceEventHandler::onLeaveChannel(const agora::rtc::RtcStats& stats)
{
	if (m_hMsgHanlder) {
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_LEAVE_CHANNEL), 0, m_Id);
	}
}


void CMultiVideoSourceEventHandler::onRemoteVideoStateChanged(agora::rtc::uid_t uid, agora::rtc::REMOTE_VIDEO_STATE state, agora::rtc::REMOTE_VIDEO_STATE_REASON reason, int elapsed)
{
	if (m_hMsgHanlder) {
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_REMOTE_VIDEO_STATE_CHANGED), (WPARAM)uid, (LPARAM)m_Id);
	}
}



void CMultiVideoSourceEventHandler::onLocalVideoStats(VIDEO_SOURCE_TYPE source, const LocalVideoStats& stats)
{
	if (m_hMsgHanlder) {
		LocalVideoStats* _stats = new LocalVideoStats;
		*_stats = stats;
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_LOCAL_VIDEO_STATS), (WPARAM)_stats, (LPARAM)source);
	}
}


void CMultiVideoSourceEventHandler::onLocalAudioStats(const LocalAudioStats& stats)
{
	if (m_hMsgHanlder) {
		LocalAudioStats* _stats = new LocalAudioStats;
		*_stats = stats;
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_LOCAL_AUDIO_STATS), (WPARAM)_stats, (LPARAM)m_Id);
	}
}


void CMultiVideoSourceEventHandler::onRemoteAudioStats(const RemoteAudioStats& stats)
{
	if (m_hMsgHanlder) {
		RemoteAudioStats* _stats = new RemoteAudioStats;
		*_stats = stats;
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_REMOTE_AUDIO_STATS), (WPARAM)_stats, (LPARAM)stats.uid);
	}
}


void CMultiVideoSourceEventHandler::onRemoteVideoStats(const RemoteVideoStats& stats)
{
	if (m_hMsgHanlder) {
		RemoteVideoStats* _stats = new RemoteVideoStats;
		*_stats = stats;
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_REMOTE_VIDEO_STATS), (WPARAM)_stats, (LPARAM)stats.uid);
	}
}

void CAgoraMutilVideoSourceDlg::OnBnClickedButtonCamera1()
{
	if (!m_bStartCapture1) {
		//primary camera configuration
		CameraCapturerConfiguration config;
		config.format.width = 640;
		config.format.height = 360;
		config.format.fps = 15;
		//get selected camera device id
		for (UINT i = 0; i < m_vecCameraInfos.size(); i++)
		{
			MULTIVIDEOSOURCE_CAMERAINFO info = m_vecCameraInfos[i];
			CString strName;
			m_cmbCameras.GetWindowText(strName);
			if (info.deviceName.compare(cs2utf8(strName)) == 0) {
				strcpy_s(config.deviceId, 512, info.deviceId.c_str());
				break;
			}
		}
		//start primary camera capture
		m_rtcEngine->startPrimaryCameraCapture(config);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("start primary camera capture"));
		VideoCanvas canvas;
		canvas.uid = 0;
		canvas.sourceType = VIDEO_SOURCE_CAMERA_PRIMARY;
		canvas.view = m_videoWnds[0].GetSafeHwnd();
		//setuplocalVideo canvas
		m_rtcEngine->setupLocalVideo(canvas);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("setupLocalVideo primary camera"));
		//startPreview
		m_rtcEngine->startPreview(VIDEO_SOURCE_CAMERA_PRIMARY);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("startpreview primary camera"));
		//show video wnds
		ShowVideoWnds();
		m_btnCapture1.SetWindowText(MultiVideoSourceStopCapture);
	}
	else {
		//stop primary camera capture
		m_rtcEngine->stopPrimaryCameraCapture();
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("stop primary camera capture"));
		m_btnCapture1.SetWindowText(MultiVideoSourceStartCapture);
		m_videoWnds[0].Reset();
	}
	m_bStartCapture1 = !m_bStartCapture1;
}

void CAgoraMutilVideoSourceDlg::OnBnClickedButtonCamera2()
{
	//If more than one camera are available,primary camera and secondary camera must be different
	if (m_vecCameraInfos.size() > 1 &&
		m_cmbCameras.GetCurSel() == m_cmbCamera2.GetCurSel()) {
		AfxMessageBox(_T("Camera2 need select a different camera"));
		return;
	}
	if (!m_bStartCapture2) {
		//camera2 configuration
		CameraCapturerConfiguration config2;
		config2.format.width = 640;
		config2.format.height = 360;
		config2.format.fps = 15;
		//set camera2 deviceId
		for (UINT i = 0; i < m_vecCameraInfos.size(); i++)
		{
			MULTIVIDEOSOURCE_CAMERAINFO info = m_vecCameraInfos[i];
			CString strName;
			m_cmbCamera2.GetWindowText(strName);
			if (info.deviceName.compare(cs2utf8(strName)) == 0) {
				strcpy_s(config2.deviceId, 512, info.deviceId.c_str());
				break;
			}
		}

		//start secondary camera capture
		m_rtcEngine->startSecondaryCameraCapture(config2);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("start secondary camera capture"));
		m_btnCapture2.SetWindowText(MultiVideoSourceStopCapture);
		VideoCanvas canvas;
		canvas.uid = connection.localUid;
		canvas.sourceType = VIDEO_SOURCE_CAMERA_SECONDARY;
		canvas.view = m_videoWnds[1].GetSafeHwnd();
		//setuplocalVideo canvas
		m_rtcEngine->setupLocalVideo(canvas);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("setupLocalVideo primary camera"));
		m_rtcEngine->startPreview(VIDEO_SOURCE_CAMERA_SECONDARY);

	}
	else {
		VideoCanvas canvas;
		canvas.uid = connection.localUid;
		canvas.sourceType = VIDEO_SOURCE_CAMERA_SECONDARY;
		canvas.view = nullptr;
		//setuplocalVideo canvas
		m_rtcEngine->setupLocalVideo(canvas);

		m_rtcEngine->stopPreview(VIDEO_SOURCE_CAMERA_SECONDARY);
		//stop secondary camera capture
		m_rtcEngine->stopSecondaryCameraCapture();
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("stop secondary camera capture"));
		m_btnCapture2.SetWindowText(MultiVideoSourceStartCapture);

		m_videoWnds[1].Reset();
	}
	m_bStartCapture2 = !m_bStartCapture2;
}

void CAgoraMutilVideoSourceDlg::OnBnClickedButtonCaptureScreen()
{
	if (!m_bStartScreenSharing) {

		agora::rtc::Rectangle rc;
		ScreenCaptureParameters scp;
		scp.frameRate = 15;
		scp.bitrate = 0;
		HWND hWnd = ::GetDesktopWindow();
		RECT destop_rc;
		::GetWindowRect(hWnd, &destop_rc);
		scp.dimensions.width = destop_rc.right - destop_rc.left;
		scp.dimensions.height = destop_rc.bottom - destop_rc.top;
		m_rtcEngine->startScreenCaptureByScreenRect(rc, rc, scp);

		VideoCanvas canvas;
		canvas.uid = connection.localUid;
		canvas.sourceType = VIDEO_SOURCE_SCREEN;
		canvas.view = m_videoWnds[2].GetSafeHwnd();
		//setuplocalVideo canvas
		m_rtcEngine->setupLocalVideo(canvas);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("setupLocalVideo screen"));
		m_btnScreenCapture.SetWindowText(MultiVideoSourceStopCapture);
		m_rtcEngine->startPreview(VIDEO_SOURCE_SCREEN);
	}
	else {
		VideoCanvas canvas;
		canvas.uid = connection.localUid;
		canvas.sourceType = VIDEO_SOURCE_SCREEN;
		canvas.view = nullptr;
		//setuplocalVideo canvas
		m_rtcEngine->setupLocalVideo(canvas);

		m_rtcEngine->stopScreenCapture();
		m_rtcEngine->stopPreview(VIDEO_SOURCE_SCREEN);

		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("stop screen capture"));
		m_btnScreenCapture.SetWindowText(MultiVideoSourceStartCapture);

		m_videoWnds[2].Reset();
	}

	m_bStartScreenSharing = !m_bStartScreenSharing;
}


void CAgoraMutilVideoSourceDlg::OnBnClickedButtonPublishScreen()
{
	CString strChannelName;
	m_edtChannelName.GetWindowText(strChannelName);
	std::string szChannelId = cs2utf8(strChannelName);
	if (!m_bScreenJoin) {

		screenConnection.localUid = generateUid();
		m_screenEventHandler.SetId(screenConnection.localUid);
		m_screenEventHandler.SetMsgReceiver(m_hWnd);

		if (!m_bStartScreenSharing)
			OnBnClickedButtonCaptureScreen();

		//joinchannelex option
		agora::rtc::ChannelMediaOptions options2;
		options2.autoSubscribeAudio = false;
		options2.autoSubscribeVideo = false;
		options2.publishMicrophoneTrack = false;
		options2.publishCameraTrack = false;
		options2.publishScreenTrack = true;
		options2.clientRoleType = CLIENT_ROLE_BROADCASTER;
		// joinChannelEx secondary camera capture(broadcaster)
		screenConnection.channelId = szChannelId.data();
		int ret = m_rtcEngine->joinChannelEx(APP_TOKEN, screenConnection, options2, &m_screenEventHandler);
		CString str;
		str.Format(_T("joinChannelEx: %d"), ret);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), str);
		m_btnScreenPublish.SetWindowText(MultiCamearaStopPublish);
	}
	else {
		if (m_bStartScreenSharing)
			OnBnClickedButtonCaptureScreen();
		//leaveChannel secondary camera
		screenConnection.channelId = szChannelId.data();
		m_rtcEngine->leaveChannelEx(screenConnection);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("leaveChannel screen"));

		m_btnScreenPublish.SetWindowText(MultiVideoSourcePublish);
	}

	m_bScreenJoin = !m_bScreenJoin;
}
