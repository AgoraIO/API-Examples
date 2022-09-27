#include "stdafx.h"
#include "APIExample.h"
#include "CAgoraSpatialAudioDlg.h"




IMPLEMENT_DYNAMIC(CAgoraSpatialAudioDlg, CDialogEx)

CAgoraSpatialAudioDlg::CAgoraSpatialAudioDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_SPATIAL_AUDIO, pParent)
{

}

CAgoraSpatialAudioDlg::~CAgoraSpatialAudioDlg()
{
}


//Initialize the Ctrl Text.
void CAgoraSpatialAudioDlg::InitCtrlText()
{
	
}

void CAgoraSpatialAudioDlg::GetWindowsRelativeRect(CStatic& child, RECT* childRect) {
	RECT parentRect;
	GetWindowRect(&parentRect);

	child.GetWindowRect(childRect);

	int childWidth = childRect->right - childRect->left;
	int childHeight = childRect->bottom - childRect->top;
	childRect->left = childRect->left - parentRect.left;
	childRect->top = childRect->top - parentRect.top;
	childRect->right = childRect->left + childWidth;
	childRect->bottom = childRect->top + childHeight;
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


	return true;
}


//UnInitialize the Agora SDK
void CAgoraSpatialAudioDlg::UnInitAgora()
{
	if (m_rtcEngine) {
		if (m_joinChannel)
			//leave channel
			m_joinChannel = !m_rtcEngine->leaveChannel();


		//disable video in the engine.
		m_rtcEngine->disableVideo();
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("disableVideo"));
		//release engine.
		m_rtcEngine->release(true);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("release rtc engine"));
		m_rtcEngine = NULL;
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

	moveLocal = false;
	origin = { 0 };

	GetWindowsRelativeRect(m_staVideoArea, &rcArea);

	GetWindowsRelativeRect(m_staPlayerLeft, &rcPlayerLeft);
	GetWindowsRelativeRect(m_staPlayerRight, &rcPlayerRight);
	GetWindowsRelativeRect(m_staRemoteLeft, &rcRemoteLeft);
	GetWindowsRelativeRect(m_staRemoteRight, &rcRemoteRight);

	GetWindowsRelativeRect(m_staLocalMove, &rcLocal);
	int localWidth = rcLocal.right - rcLocal.left;
	int localHeight = rcLocal.bottom - rcLocal.top;
	rcLocal.left = rcArea.left + (rcArea.right - rcArea.left - localWidth) / 2.0f;
	rcLocal.top = rcArea.top + (rcArea.bottom - rcArea.top - localHeight) / 2.0f;
	rcLocal.right = rcLocal.left + localWidth;
	rcLocal.bottom = rcLocal.top + localHeight;
	m_staLocalMove.MoveWindow(&rcLocal);
}


void CAgoraSpatialAudioDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_CHANNELNAME, m_staChannelName);
	DDX_Control(pDX, IDC_EDIT_CHANNELNAME, m_edtChannelName);
	DDX_Control(pDX, IDC_BUTTON_JOINCHANNEL, m_btnJoinChannel);

	DDX_Control(pDX, IDC_STATIC_VIDEO, m_staVideoArea);
	DDX_Control(pDX, IDC_LIST_INFO_BROADCASTING, m_lstInfo);
	DDX_Control(pDX, IDC_STATIC_DETAIL, m_staDetail);


	DDX_Control(pDX, IDC_STATIC_PLAYER_LEFT, m_staPlayerLeft);
	DDX_Control(pDX, IDC_STATIC_PLAYER_RIGHT, m_staPlayerRight);
	DDX_Control(pDX, IDC_STATIC_REMOTE_LEFT, m_staRemoteLeft);
	DDX_Control(pDX, IDC_STATIC_REMOTE_RIGHT, m_staRemoteRight);
	DDX_Control(pDX, IDC_STATIC_LOCAL_MOVE, m_staLocalMove);
}


BEGIN_MESSAGE_MAP(CAgoraSpatialAudioDlg, CDialogEx)

	ON_LBN_SELCHANGE(IDC_LIST_INFO_BROADCASTING, &CAgoraSpatialAudioDlg::OnSelchangeListInfoBroadcasting)
	ON_WM_SHOWWINDOW()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	
	ON_MESSAGE(WM_MSGID(EID_USER_JOINED), &CAgoraSpatialAudioDlg::OnEIDUserJoined)
	ON_MESSAGE(WM_MSGID(EID_USER_OFFLINE), &CAgoraSpatialAudioDlg::OnEIDUserOffline)
	ON_MESSAGE(WM_MSGID(EID_JOINCHANNEL_SUCCESS), &CAgoraSpatialAudioDlg::OnEIDJoinChannelSuccess)
	ON_MESSAGE(WM_MSGID(EID_LEAVE_CHANNEL), &CAgoraSpatialAudioDlg::OnEIDLeaveChannel)

	ON_BN_CLICKED(IDC_BUTTON_JOINCHANNEL, &CAgoraSpatialAudioDlg::OnBnClickedButtonJoinchannel)
	ON_STN_CLICKED(IDC_STATIC_PLAYER_LEFT, &CAgoraSpatialAudioDlg::OnStnClickedStaticPlayerLeft)
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
	}
	else {
		ResumeStatus();
	}
}

BOOL CAgoraSpatialAudioDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	ResumeStatus();
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


void CAgoraSpatialAudioDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	/*if (!canmove) {
		m_lstInfo.InsertString(m_lstInfo.GetCount(), L"Start first");
		return;
	}*/

	

	CString stInfo;
	stInfo.Format(_T("PlayerLeft rect l=%d, t=%d, r=%d, b=%d"), rcPlayerLeft.left, rcPlayerLeft.top, rcPlayerLeft.right, rcPlayerLeft.bottom);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), stInfo);
	stInfo.Format(_T("PlayerRight rect l=%d, t=%d, r=%d, b=%d"), rcPlayerRight.left, rcPlayerRight.top, rcPlayerRight.right, rcPlayerRight.bottom);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), stInfo);
	stInfo.Format(_T("RemoteLeft rect l=%d, t=%d, r=%d, b=%d"), rcRemoteLeft.left, rcRemoteLeft.top, rcRemoteLeft.right, rcRemoteLeft.bottom);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), stInfo);
	stInfo.Format(_T("RemoteRight rect l=%d, t=%d, r=%d, b=%d"), rcRemoteRight.left, rcRemoteRight.top, rcRemoteRight.right, rcRemoteRight.bottom);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), stInfo);
	stInfo.Format(_T("Local rect l=%d, t=%d, r=%d, b=%d"), rcLocal.left, rcLocal.top, rcLocal.right, rcLocal.bottom);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), stInfo);

	
	stInfo.Format(_T("Down point x=%d, y=%d"), point.x, point.y);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), stInfo);

	if (PtInRect(&rcLocal, point)) {
		moveLocal = true;
		origin = point;
	}

}

void CAgoraSpatialAudioDlg::SetSpatialAudioParam()
{
	/*POINT ptLocal = {rcLocal.left + localWidth / 2.0f,rcLocal.top + localWidth / 2.0f};
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
	m_rtcEngine->setRemoteUserSpatialAudioParams(uid, spatial_audio_params);*/
}

void CAgoraSpatialAudioDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	moveLocal = false;
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


void CAgoraSpatialAudioDlg::OnMouseMove(UINT nFlags, CPoint point) {
	if (moveLocal && nFlags == MK_LBUTTON) {

		int iconWidth = rcLocal.right - rcLocal.left;
		int iconHeight = rcLocal.bottom - rcLocal.top;

		int x = rcLocal.left + (point.x - origin.x);
		if (x < rcArea.left)
			x = rcArea.left;
		else if (x + iconWidth > rcArea.right)
			x = rcArea.right - iconWidth;

		int y = rcLocal.top + (point.y - origin.y);

		if (y < rcArea.top)
			y = rcArea.top;
		else if (y + iconHeight > rcArea.bottom)
			y = rcArea.bottom - iconHeight;

		rcLocal.left = x;
		rcLocal.top = y;
		rcLocal.right = rcLocal.left + iconWidth;
		rcLocal.bottom = rcLocal.top + iconHeight;
		m_staLocalMove.MoveWindow(&rcLocal);

		origin = point;
	}

}


LRESULT CAgoraSpatialAudioDlg::OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam) {

	m_btnJoinChannel.EnableWindow(TRUE);

	m_joinChannel = true;
	m_btnJoinChannel.SetWindowText(commonCtrlLeaveChannel);

	CString strInfo;
	strInfo.Format(_T("%s:join success, uid=%u"), getCurrentTime(), wParam);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

	//notify parent window
	::PostMessage(GetParent()->GetSafeHwnd(), WM_MSGID(EID_JOINCHANNEL_SUCCESS), TRUE, 0);
	return 0;
}

LRESULT CAgoraSpatialAudioDlg::OnEIDLeaveChannel(WPARAM wParam, LPARAM lParam) {
	m_btnJoinChannel.EnableWindow(TRUE);
	m_joinChannel = false;
	m_btnJoinChannel.SetWindowText(commonCtrlJoinChannel);

	CString strInfo;
	strInfo.Format(_T("leave channel success %s"), getCurrentTime());
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	

	//notify parent window
	::PostMessage(GetParent()->GetSafeHwnd(), WM_MSGID(EID_JOINCHANNEL_SUCCESS), FALSE, 0);
	return 0;
}


void CAgoraSpatialAudioDlg::OnBnClickedButtonJoinchannel()
{
	if (!m_rtcEngine || !m_initialize)
		return;
	CString strInfo;
	if (!m_joinChannel) {
		CString strChannelName;
		m_edtChannelName.GetWindowText(strChannelName);
		if (strChannelName.IsEmpty()) {
			AfxMessageBox(_T("Fill channel name first"));
			return;
		}
		
		std::string szChannelId = cs2utf8(strChannelName);

		ChannelMediaOptions options;
		options.channelProfile = CHANNEL_PROFILE_LIVE_BROADCASTING;
		options.clientRoleType = CLIENT_ROLE_BROADCASTER;
		options.autoSubscribeAudio = true;
		options.autoSubscribeVideo = true;
		//join channel in the engine.
		if (0 == m_rtcEngine->joinChannel(APP_TOKEN, szChannelId.c_str(), 0, options)) {
			strInfo.Format(_T("join channel %s, use ChannelMediaOptions"), getCurrentTime().AllocSysString());
			m_btnJoinChannel.EnableWindow(FALSE);
		}
	}
	else {
		if (0 == m_rtcEngine->leaveChannel()) {
			strInfo.Format(_T("leave channel %s"), getCurrentTime().AllocSysString());
			m_btnJoinChannel.EnableWindow(FALSE);
		}
	}

	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
}





void CAgoraSpatialAudioDlg::OnStnClickedStaticPlayerLeft()
{
	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("OnStnClickedStaticPlayerLeft"));
}
