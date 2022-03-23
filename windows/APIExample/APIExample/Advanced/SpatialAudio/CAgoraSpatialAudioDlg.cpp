#include "stdafx.h"
#include "APIExample.h"
#include "CAgoraSpatialAudioDlg.h"



IMPLEMENT_DYNAMIC(CAgoraSpatialAudioDlg, CDialogEx)

CAgoraSpatialAudioDlg::CAgoraSpatialAudioDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_AUDIO_MIX, pParent)
{

}

CAgoraSpatialAudioDlg::~CAgoraSpatialAudioDlg()
{
}


//Initialize the Ctrl Text.
void CAgoraSpatialAudioDlg::InitCtrlText()
{

}



//Initialize the Agora SDK
bool CAgoraSpatialAudioDlg::InitAgora()
{
	//create Agora RTC engine
	m_rtcEngine = createAgoraRtcEngine();
	if (!m_rtcEngine) {
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("createAgoraRtcEngine failed"));
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
	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("initialize success"));
	
	//set channel profile in the engine to the CHANNEL_PROFILE_LIVE_BROADCASTING.
	m_rtcEngine->setChannelProfile(CHANNEL_PROFILE_LIVE_BROADCASTING);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("live broadcasting"));
	//set client role in the engine to the CLIENT_ROLE_BROADCASTER.
	m_rtcEngine->setClientRole(CLIENT_ROLE_BROADCASTER);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("setClientRole broadcaster"));


	
	m_mapState[710] = L"AUDIO_MIXING_STATE_PLAYING";
	m_mapState[711] = L"AUDIO_MIXING_STATE_PAUSED";
	m_mapState[713] = L"AUDIO_MIXING_STATE_STOPPED";
	m_mapState[714] = L"AUDIO_MIXING_STATE_FAILED";

	m_mapReason[701] = L"AUDIO_MIXING_REASON_CAN_NOT_OPEN";
	m_mapReason[702] = L"AUDIO_MIXING_REASON_TOO_FREQUENT_CALL";
	m_mapReason[703] = L"AUDIO_MIXING_REASON_INTERRUPTED_EOF";
	m_mapReason[720] = L"AUDIO_MIXING_REASON_STARTED_BY_USER";
	m_mapReason[721] = L"AUDIO_MIXING_REASON_ONE_LOOP_COMPLETED";
	m_mapReason[722] = L"AUDIO_MIXING_REASON_START_NEW_LOOP";
	m_mapReason[723] = L"AUDIO_MIXING_REASON_ALL_LOOPS_COMPLETED";
	m_mapReason[724] = L"AUDIO_MIXING_REASON_STOPPED_BY_USER";
	m_mapReason[725] = L"AUDIO_MIXING_REASON_PAUSED_BY_USER";
	m_mapReason[726] = L"AUDIO_MIXING_REASON_RESUMED_BY_USER";

	return true;
}


//UnInitialize the Agora SDK
void CAgoraSpatialAudioDlg::UnInitAgora()
{
	if (m_rtcEngine) {
		if (m_joinChannel)
			//leave channel
			m_joinChannel = !m_rtcEngine->leaveChannel();
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

//render local video from SDK local capture.
void CAgoraSpatialAudioDlg::RenderLocalVideo()
{
	if (m_rtcEngine) {
		//start preview in the engine.
		m_rtcEngine->startPreview();
	//	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("startPreview"));
		VideoCanvas canvas;
		canvas.renderMode = RENDER_MODE_FIT;
		canvas.uid = 0;
		//canvas.view = m_localVideoWnd.GetSafeHwnd();
		//setup local video in the engine to canvas.
	//	m_rtcEngine->setupLocalVideo(canvas);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("setupLocalVideo"));
	}
}

//resume window status
void CAgoraSpatialAudioDlg::ResumeStatus()
{
	InitCtrlText();
	m_lstInfo.ResetContent();
	m_staDetail.SetWindowText(_T(""));
	
	m_joinChannel = false;
	m_initialize = false;
	m_SpatialAudio = false;
	m_btnStart.EnableWindow(TRUE);

	rcLocal = { 0 };
	rcRemote = { 0 };
	moveRemote = false;
	origin = { 0 };

}


void CAgoraSpatialAudioDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_VIDEO, m_staVideoArea);
	DDX_Control(pDX, IDC_LIST_INFO_BROADCASTING, m_lstInfo);
	DDX_Control(pDX, IDC_STATIC_DETAIL, m_staDetail);


	DDX_Control(pDX, IDC_BUTTON_START, m_btnStart);
}


BEGIN_MESSAGE_MAP(CAgoraSpatialAudioDlg, CDialogEx)
	ON_LBN_SELCHANGE(IDC_LIST_INFO_BROADCASTING, &CAgoraSpatialAudioDlg::OnSelchangeListInfoBroadcasting)
	ON_WM_SHOWWINDOW()
	
	ON_MESSAGE(WM_MSGID(EID_USER_JOINED), &CAgoraSpatialAudioDlg::OnEIDUserJoined)
	ON_MESSAGE(WM_MSGID(EID_USER_OFFLINE), &CAgoraSpatialAudioDlg::OnEIDUserOffline)
	
	ON_MESSAGE(WM_LBUTTON_DOWN_WND, &CAgoraSpatialAudioDlg::OnLButtonDownVideo)
	ON_MESSAGE(WM_LBUTTON_UP_WND, &CAgoraSpatialAudioDlg::OnLButtonUpVideo)
	
	ON_BN_CLICKED(IDC_BUTTON_START, &CAgoraSpatialAudioDlg::OnBnClickedButtonStart)
	ON_WM_TIMER()
END_MESSAGE_MAP()


void CAgoraSpatialAudioDlg::OnSelchangeListInfoBroadcasting()
{
	int sel = m_lstInfo.GetCurSel();
	if (sel < 0)return;
	CString strDetail;
	m_lstInfo.GetText(sel, strDetail);
	m_staDetail.SetWindowText(strDetail);
}


void CAgoraSpatialAudioDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);
	if (bShow)//bShwo is true ,show window 
	{
		InitCtrlText();
		RenderLocalVideo();
	}
	else {
		ResumeStatus();
	}
}

BOOL CAgoraSpatialAudioDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_localVideoWnd.Create(NULL, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CRect(0, 0, 1, 1), this, ID_BASEWND_VIDEO + 100);

	m_staVideoArea.GetClientRect(&rcArea);
	// m_staVideoArea.ShowWindow(SW_SHOW);
	m_localVideoWnd.MoveWindow(&rcArea);
	m_localVideoWnd.ShowWindow(SW_SHOW);

	m_localVideoWnd.ShowVideoInfo(SpatialAudioInitInfo, TRUE);
	ResumeStatus();

	m_staLocal.Create(L"", WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CRect(0, 0, 1, 1), this, ID_BASEWND_VIDEO + 100000);
	m_staLocal.SetParent(&m_localVideoWnd);
	CRect rc = rcArea;
	int x = int((rc.Width() - localWidth) / 2.0f);
	int y = int((rc.Height() - localWidth) / 2.0f);
	rcLocal.left = x;
	rcLocal.top = y;
	rcLocal.right = x + localWidth;
	rcLocal.bottom = y + localWidth;
	m_staLocal.MoveWindow(x, y, localWidth, localWidth);
	m_staLocal.ShowWindow(SW_SHOW);


	m_staRemote.Create(L"", WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CRect(0, 0, 1, 1), this, ID_BASEWND_VIDEO + 100001);
	m_staRemote.SetParent(&m_localVideoWnd);

	x = 0;
	y = 0;
	rcRemote.left = x;
	rcRemote.top = y;
	rcRemote.right = x + remoteWidth;
	rcRemote.bottom = y + remoteWidth;
	m_staRemote.MoveWindow(x, y, remoteWidth, remoteWidth);
	m_staRemote.ShowWindow(SW_SHOW);

	m_audioPath = GetExePath() + _T("\\audiomixing.mp3");

	int deltaX = abs(rcRemote.left - rcLocal.left);
	int deltaY = abs(rcRemote.top - rcLocal.top);
	int maxdis = sqrt(deltaX*deltaX + deltaY * deltaY);
	distanceRate = maxdis / 30.0f;
	//remoteImage.Create(70, 70, ILC_COLOR32 | ILC_MASK, 1, 1);
	//CBitmap bmp;
	//bmp.LoadBitmap(IDB_BITMAP_NETWORK_STATE);
	//remoteImage.Add(&bmp, RGB(255, 255, 255));
	//bmp.DeleteObject();
	CString remoteImage = GetExePath() + _T("\\remote.bmp");
	HBITMAP hBmp = (HBITMAP)::LoadImage(0, remoteImage, IMAGE_BITMAP, 70, 70, LR_LOADFROMFILE);
	m_staRemote.ModifyStyle(NULL, SS_BITMAP);
	m_staRemote.SetBitmap(hBmp);

	CString localImage = GetExePath() + _T("\\local.bmp");
	HBITMAP hBmpLocal = (HBITMAP)::LoadImage(0, localImage, IMAGE_BITMAP, 70, 70, LR_LOADFROMFILE);
	m_staLocal.ModifyStyle(NULL, SS_BITMAP);
	m_staLocal.SetBitmap(hBmpLocal);
	return TRUE;
}


BOOL CAgoraSpatialAudioDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) {
		return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}



//EID_USER_JOINED message window handler
LRESULT CAgoraSpatialAudioDlg::OnEIDUserJoined(WPARAM wParam, LPARAM lParam)
{
	CString strInfo;
	strInfo.Format(_T("%u joined"), wParam);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	uid = wParam;
	SetSpatialAudioParam();
	return 0;
}

//EID_USER_OFFLINE message handler.
LRESULT CAgoraSpatialAudioDlg::OnEIDUserOffline(WPARAM wParam, LPARAM lParam)
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


LRESULT CAgoraSpatialAudioDlg::OnLButtonDownVideo(WPARAM wParam, LPARAM lParam)
{
	if (!canmove) {
		m_lstInfo.InsertString(m_lstInfo.GetCount(), L"Start first");
		return 0;
	}
	POINT pt = { wParam, lParam };
	if (PtInRect(&rcRemote, pt)) {
		moveRemote = true;
		origin = pt;
	}
	return 0;
}

void CAgoraSpatialAudioDlg::SetSpatialAudioParam()
{
	POINT ptLocal = { rcLocal.left + localWidth / 2.0f,rcLocal.top + localWidth / 2.0f };
	POINT ptRemote = { rcRemote.left + remoteWidth / 2.0f,rcRemote.top + remoteWidth / 2.0f };

	SpatialAudioParams spatial_audio_params;
	int deltaX = ptRemote.x - ptLocal.x;
	int deltaY = ptLocal.y - ptRemote.y;

	int tanAngle = 0;
	float tanValue = 1;
	if (deltaX == 0) {
		if (deltaY > 0)
			tanAngle = 360;
		else if (deltaY < 0)
			tanAngle = 180;
	}else{
		tanValue = fabs(deltaY) / fabs(deltaX);
		tanAngle = atan(tanValue) * 180.0 / 3.1415926;
	}

	int spatialAngle = 0.0; // It is Angle 0 when at Y direction
	if (deltaX > 0 && deltaY > 0) { // scope I
		spatialAngle = 270.0 + tanAngle;
	}
	else if (deltaX < 0 && deltaY > 0) { // scope II
		spatialAngle = 90.0 - tanAngle;
	}
	else if (deltaX < 0 && deltaY < 0) { // scope III
		spatialAngle = 90.0 + tanAngle;
	}
	else if (deltaX > 0 && deltaY < 0) { // scope VI
		spatialAngle = 270.0 - tanAngle;
	}

	CString str;
	str.Format(_T("%d\n"), spatialAngle);
	OutputDebugString(str);
	int spatialDistance =  sqrt(deltaX*deltaX + deltaY * deltaY) / distanceRate;
	spatial_audio_params.speaker_distance = 0;// spatialDistance;
	spatial_audio_params.speaker_azimuth = spatialAngle;
	spatial_audio_params.speaker_elevation = 0;
	spatial_audio_params.speaker_orientation = 0;
	m_rtcEngine->setRemoteUserSpatialAudioParams(uid, spatial_audio_params);
}

LRESULT CAgoraSpatialAudioDlg::OnLButtonUpVideo(WPARAM wParam, LPARAM lParam)
{
	POINT pt = { wParam, lParam };
	if (moveRemote) {
		moveRemote = true;
		int x = rcRemote.left + (pt.x - origin.x);
		if (x < 0)
			x = 0;
		else if (x + remoteWidth > rcArea.right)
			x = rcArea.right - remoteWidth;
		int y = rcRemote.top + (pt.y - origin.y);
		
		if (y < 0)
			y = 0;
		else if (y + remoteWidth > rcArea.bottom)
			y = rcArea.bottom - remoteWidth;

		rcRemote.left = x;
		rcRemote.top = y;
		rcRemote.right = rcRemote.left + remoteWidth;
		rcRemote.bottom = rcRemote.top + remoteWidth;
		m_staRemote.MoveWindow(&rcRemote);

	    
		SetSpatialAudioParam();

	}
	moveRemote = false;
	return 0;
}

void CAgoraSpatialAudioDlg::OnBnClickedButtonStart()
{
	m_SpatialAudio = !m_SpatialAudio;
	SetTimer(echoTestId, 10000, NULL);
	m_rtcEngine->startEchoTest(10);
	CString strInfo;
	strInfo.Format(_T("startEchoTest %d seconds"), 10);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	m_rtcEngine->startAudioMixing(cs2utf8(m_audioPath).c_str(),  false, true, 1, 0);
	strInfo.Format(_T("startAudioMixing"));
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	
	m_btnStart.EnableWindow(FALSE);
	m_localVideoWnd.ShowVideoInfo(SpatialAudioPrepareInfo, TRUE);
}


void CAgoraSpatialAudioDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (echoTestId == nIDEvent) {
		CString strInfo;
		strInfo.Format(_T("stopAudioMixing "));
		m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
		m_rtcEngine->stopAudioMixing();
		m_rtcEngine->enableSpatialAudio(true);
		strInfo.Format(_T("enableSpatialAudio true "));
		m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
		SetTimer(disableSpatialId, 10000, NULL);
		KillTimer(echoTestId);
		m_localVideoWnd.ShowVideoInfo(SpatialAudioMoveInfo, TRUE);
		canmove = true;
	}
	else if (disableSpatialId == nIDEvent) {
		m_rtcEngine->enableSpatialAudio(false);
		CString strInfo;
		strInfo.Format(_T("enableSpatialAudio false "));
		m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
		KillTimer(disableSpatialId);
		m_rtcEngine->stopEchoTest();
		strInfo.Format(_T("stopEchoTest"));
		m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
		canmove = false;
		m_localVideoWnd.ShowVideoInfo(SpatialAudioInitInfo, TRUE);

		m_btnStart.EnableWindow(TRUE);
	}

	CDialogEx::OnTimer(nIDEvent);
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
void CSpatialAudioEventHandler::onJoinChannelSuccess(const char* channel, uid_t uid, int elapsed)
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
void CSpatialAudioEventHandler::onUserJoined(uid_t uid, int elapsed)
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
void CSpatialAudioEventHandler::onUserOffline(uid_t uid, USER_OFFLINE_REASON_TYPE reason)
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

void CSpatialAudioEventHandler::onLeaveChannel(const RtcStats& stats)
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
void CSpatialAudioEventHandler::onRemoteVideoStateChanged(uid_t uid, REMOTE_VIDEO_STATE state, REMOTE_VIDEO_STATE_REASON reason, int elapsed)
{
	if (m_hMsgHanlder) {
		PVideoStateStateChanged stateChanged = new VideoStateStateChanged;
		stateChanged->uid = uid;
		stateChanged->reason = reason;
		stateChanged->state = state;
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_REMOTE_VIDEO_STATE_CHANED), (WPARAM)stateChanged, 0);
	}
}

void CSpatialAudioEventHandler::onAudioMixingStateChanged(AUDIO_MIXING_STATE_TYPE state, AUDIO_MIXING_REASON_TYPE reason)
{
	if (m_hMsgHanlder) {
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_AUDIO_MIXING_STATE_CHANGED), (WPARAM)state, (LPARAM)reason);
	}

}

