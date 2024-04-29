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
	m_staMoveTip.SetWindowTextW(SpatialAudioMoveInfo);
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

void CAgoraSpatialAudioDlg::CaculateObjectPosition(CStatic& child, float out[3]) {

	RECT parentRect;
	RECT childRect;
	m_staVideoArea.GetWindowRect(&parentRect);
	child.GetWindowRect(&childRect);

	int parentWidth = parentRect.right - parentRect.left;
	int parentHeight = parentRect.bottom - parentRect.top;

	float retCenterX = (childRect.left - parentRect.left) + (childRect.right - childRect.left) / 2.0f;
	float retCenterY = (childRect.top - parentRect.top) + (childRect.right - childRect.left) / 2.0f;

	// forward
	out[0] = AXIS_MAX_DISTANCE - retCenterY * 1.0f / parentHeight * AXIS_MAX_DISTANCE * 2;
	// right
	out[1] = retCenterX * 1.0f / parentWidth * AXIS_MAX_DISTANCE * 2 - AXIS_MAX_DISTANCE;
	// up
	out[2] = 0;

}

void CAgoraSpatialAudioDlg::CaculateZoneRect(CStatic& zone, RECT* rect) {

	RECT parentRect;
	RECT childRect;
	m_staVideoArea.GetWindowRect(&parentRect);
	zone.GetWindowRect(&childRect);

	int parentWidth = parentRect.right - parentRect.left;
	int parentHeight = parentRect.bottom - parentRect.top;

	int zoneWidth = childRect.right - childRect.left;
	int zoneHeight = childRect.bottom - childRect.top;

	int width = (int)(zoneWidth * 1.0f / parentWidth * AXIS_MAX_DISTANCE * 2 + 0.5f);
	int height = (int)(zoneHeight * 1.0f / parentHeight * AXIS_MAX_DISTANCE * 2 + 0.5f);

	float position[3];
	CaculateObjectPosition(zone, position);

	rect->top = (int)(position[0] - height / 2.0f + 0.5f);
	rect->left = (int)(position[1] - width / 2.0f + 0.5f);
	rect->bottom = rect->top + height;
	rect->right = rect->left + width;

}

//Initialize the Agora SDK
bool CAgoraSpatialAudioDlg::InitAgora()
{
	CString strInfo;
	int ret;

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
	ret = m_rtcEngine->initialize(context);
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
	m_rtcEngine->enableAudio();

	// initialize media player 
	m_mediaPlayerLeft = m_rtcEngine->createMediaPlayer();
	playerLeftObserver.setMediaPlayer(m_mediaPlayerLeft.get());
	m_mediaPlayerLeft->registerPlayerSourceObserver(&playerLeftObserver);

	m_mediaPlayerRight = m_rtcEngine->createMediaPlayer();
	playerRightObserver.setMediaPlayer(m_mediaPlayerRight.get());
	m_mediaPlayerRight->registerPlayerSourceObserver(&playerRightObserver);

	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("initialize MediaPlayer"));

	// initialize spatial audio engine

	m_rtcEngine->queryInterface(AGORA_IID_LOCAL_SPATIAL_AUDIO, (void**)&m_localSpatial);
	LocalSpatialAudioConfig config;
	config.rtcEngine = m_rtcEngine;
	m_localSpatial->initialize(config);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("LocalSpatialAudioEngine initialize"));
	m_localSpatial->setMaxAudioRecvCount(2);
	m_localSpatial->setDistanceUnit(1);
	ret = m_localSpatial->setAudioRecvRange(AXIS_MAX_DISTANCE);
	strInfo.Format(_T("LocalSpatialAudioEngine setAudioRecvRange %f, ret=%d"), AXIS_MAX_DISTANCE, ret);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);



	// update self position in audio space
	float pos[3] = { 0 };
	CaculateObjectPosition(m_staLocalMove, pos);
	float forward[3] = { 1.0f, 0, 0 };
	float right[3] = { 0, 1.0f, 0 };
	float up[3] = { 0, 0, 1.0f };
	m_localSpatial->updateSelfPosition(pos, forward, right, up);
	strInfo.Format(_T("begin self position forward=%f, right=%f"), pos[0], pos[1]);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

	// update media player position
	RemoteVoicePositionInfo playerLeftPositionInfo;
	memset(&playerLeftPositionInfo, 0, sizeof(RemoteVoicePositionInfo));
	CaculateObjectPosition(m_staPlayerLeft, playerLeftPositionInfo.position);
	playerLeftPositionInfo.forward[0] = 1.0f;
	m_localSpatial->updatePlayerPositionInfo(m_mediaPlayerLeft->getMediaPlayerId(), playerLeftPositionInfo);
	strInfo.Format(_T("media player left position forward=%f, right=%f"), playerLeftPositionInfo.position[0], playerLeftPositionInfo.position[1]);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

	RemoteVoicePositionInfo playerRightPositionInfo;
	memset(&playerRightPositionInfo, 0, sizeof(RemoteVoicePositionInfo));
	CaculateObjectPosition(m_staPlayerRight, playerRightPositionInfo.position);
	playerRightPositionInfo.forward[0] = 1.0f;
	m_localSpatial->updatePlayerPositionInfo(m_mediaPlayerRight->getMediaPlayerId(), playerRightPositionInfo);
	strInfo.Format(_T("media player right position forward=%f, right=%f"), playerRightPositionInfo.position[0], playerRightPositionInfo.position[1]);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

	return true;
}


//UnInitialize the Agora SDK
void CAgoraSpatialAudioDlg::UnInitAgora()
{
	if (m_rtcEngine) {
		if (m_mediaPlayerLeft.get() != nullptr) {
			m_mediaPlayerLeft->stop();
			m_mediaPlayerLeft.reset();
		}
		if (m_mediaPlayerRight.get() != nullptr) {
			m_mediaPlayerRight->stop();
			m_mediaPlayerRight.reset();
		}
		m_localSpatial = nullptr;

		if (m_joinChannel)
			//leave channel
			m_joinChannel = !m_rtcEngine->leaveChannel();


		//disable video in the engine.
		m_rtcEngine->disableAudio();
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("disableAudio"));

		//release engine.
		if (m_initialize) {
			m_rtcEngine->release(true);
			m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("release rtc engine"));
		}
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
	remoteLeftUid = 0;
	remoteRightUid = 0;

	GetWindowsRelativeRect(m_staVideoArea, &rcArea);

	GetWindowsRelativeRect(m_staPlayerLeft, &rcPlayerLeft);
	GetWindowsRelativeRect(m_staPlayerRight, &rcPlayerRight);
	GetWindowsRelativeRect(m_staRemoteLeft, &rcRemoteLeft);
	GetWindowsRelativeRect(m_staRemoteRight, &rcRemoteRight);

	GetWindowsRelativeRect(m_staLocalMove, &rcLocal);
	int localWidth = rcLocal.right - rcLocal.left;
	int localHeight = rcLocal.bottom - rcLocal.top;
	rcLocal.left = (int)(rcArea.left + (rcArea.right - rcArea.left - localWidth) / 2.0f);
	rcLocal.top = (int)(rcArea.top + (rcArea.bottom - rcArea.top - localHeight) / 2.0f);
	rcLocal.right = rcLocal.left + localWidth;
	rcLocal.bottom = rcLocal.top + localHeight;
	m_staLocalMove.MoveWindow(&rcLocal);

	m_staPlayerLeft.ShowWindow(FALSE);
	m_staPlayerRight.ShowWindow(FALSE);
	m_staRemoteLeft.ShowWindow(FALSE);
	m_staRemoteRight.ShowWindow(FALSE);
	m_staLocalMove.ShowWindow(FALSE);

	m_staAudioSource.ShowWindow(FALSE);
	m_comAudioSource.ShowWindow(FALSE);
	m_chkAudioSourceMute.ShowWindow(FALSE);
	m_chkAudioSourceBlur.ShowWindow(FALSE);
	m_chkAudioSourceAir.ShowWindow(FALSE);
	m_staAudioSourceAttenuation.ShowWindow(FALSE);
	m_sldAudioSourceAttenuation.ShowWindow(FALSE);

	m_configMap.clear();
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

	DDX_Control(pDX, IDC_STATIC_AUDIO_SOURCE, m_staAudioSource);
	DDX_Control(pDX, IDC_COMBO_AUDIO_SOURCE, m_comAudioSource);
	DDX_Control(pDX, IDC_CHECK_AUDIO_SOURCE_MUTE, m_chkAudioSourceMute);
	DDX_Control(pDX, IDC_CHECK_AUDIO_SOURCE_BLUR, m_chkAudioSourceBlur);
	DDX_Control(pDX, IDC_CHECK_AUDIO_SOURCE_AIRBORNE, m_chkAudioSourceAir);
	DDX_Control(pDX, IDC_STATIC_AUDIO_SOURCE_ATTENUATION, m_staAudioSourceAttenuation);
	DDX_Control(pDX, IDC_SLIDER_AUDIO_SOURCE_ATTENUATION, m_sldAudioSourceAttenuation);
	DDX_Control(pDX, IDC_STATIC_ZONE_BG, m_staZone);
	DDX_Control(pDX, IDC_CHECK_MUTE_LOCAL_AUDIO, m_chkLocalMute);
	DDX_Control(pDX, IDC_CHECK_AUDIO_ZONE, m_chkZone);
	DDX_Control(pDX, IDC_STATIC_MOVE_TIP, m_staMoveTip);

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

	ON_CBN_SELCHANGE(IDC_COMBO_AUDIO_SOURCE, &CAgoraSpatialAudioDlg::OnCbnSelchangeComboAudioSource)
	ON_BN_CLICKED(IDC_CHECK_AUDIO_SOURCE_MUTE, &CAgoraSpatialAudioDlg::OnBnClickedCheckAudioSourceMute)
	ON_BN_CLICKED(IDC_CHECK_AUDIO_SOURCE_BLUR, &CAgoraSpatialAudioDlg::OnBnClickedCheckAudioSourceBlur)
	ON_BN_CLICKED(IDC_CHECK_AUDIO_SOURCE_AIRBORNE, &CAgoraSpatialAudioDlg::OnBnClickedCheckAudioSourceAirborne)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_AUDIO_SOURCE_ATTENUATION, &CAgoraSpatialAudioDlg::OnNMCustomdrawSliderAudioSourceAttenuation)
	ON_BN_CLICKED(IDC_CHECK_AUDIO_ZONE, &CAgoraSpatialAudioDlg::OnBnClickedCheckAudioZone)
	ON_BN_CLICKED(IDC_CHECK_MUTE_LOCAL_AUDIO, &CAgoraSpatialAudioDlg::OnBnClickedCheckMuteLocalAudio)
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

	if (remoteLeftUid == 0) {
		remoteLeftUid = wParam;

		m_staRemoteLeft.ShowWindow(TRUE);

		RemoteVoicePositionInfo posInfo;
		memset(&posInfo, 0, sizeof(RemoteVoicePositionInfo));
		CaculateObjectPosition(m_staRemoteLeft, posInfo.position);
		m_localSpatial->updateRemotePosition(remoteLeftUid, posInfo);

		m_comAudioSource.AddString(CONFIG_KEY_REMOTE_LEFT);
		m_configMap[CONFIG_KEY_REMOTE_LEFT] = CAgoraSpatialAudioConfig();
	}
	else if (remoteRightUid == 0) {
		remoteRightUid = wParam;

		m_staRemoteRight.ShowWindow(TRUE);

		RemoteVoicePositionInfo posInfo;
		memset(&posInfo, 0, sizeof(RemoteVoicePositionInfo));
		CaculateObjectPosition(m_staRemoteRight, posInfo.position);
		m_localSpatial->updateRemotePosition(remoteRightUid, posInfo);

		m_comAudioSource.AddString(CONFIG_KEY_REMOTE_RIGHT);
		m_configMap[CONFIG_KEY_REMOTE_RIGHT] = CAgoraSpatialAudioConfig();
	}


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

	if (remoteLeftUid == remoteUid) {
		remoteLeftUid = 0;

		m_staRemoteLeft.ShowWindow(FALSE);
		m_localSpatial->removeRemotePosition(remoteUid);

		m_comAudioSource.DeleteString(m_comAudioSource.FindString(0, CONFIG_KEY_REMOTE_LEFT));
		m_configMap.erase(CONFIG_KEY_REMOTE_LEFT);
	}
	else if (remoteRightUid == remoteUid) {
		remoteRightUid = 0;

		m_staRemoteRight.ShowWindow(FALSE);
		m_localSpatial->removeRemotePosition(remoteUid);

		m_comAudioSource.DeleteString(m_comAudioSource.FindString(0, CONFIG_KEY_REMOTE_RIGHT));
		m_configMap.erase(CONFIG_KEY_REMOTE_RIGHT);
	}
	return 0;
}


void CAgoraSpatialAudioDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (!canmove) {
		m_lstInfo.InsertString(m_lstInfo.GetCount(), L"Start first");
		return;
	}

	if (PtInRect(&rcLocal, point)) {
		moveLocal = true;
		origin = point;
	}
}


void CAgoraSpatialAudioDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	moveLocal = false;

	CString strInfo;

	float pos[3] = { 0 };
	CaculateObjectPosition(m_staLocalMove, pos);
	float forward[3] = { 1.0f, 0, 0 };
	float right[3] = { 0, 1.0f, 0 };
	float up[3] = { 0, 0, 1.0f };
	m_localSpatial->updateSelfPosition(pos, forward, right, up);


	strInfo.Format(_T("self latest position forward=%f, right=%f"), pos[0], pos[1]);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
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

		// update self position in audio space
		float pos[3] = { 0 };
		CaculateObjectPosition(m_staLocalMove, pos);
		float forward[3] = { 1.0f, 0, 0 };
		float right[3] = { 0, 1.0f, 0 };
		float up[3] = { 0, 0, 1.0f };
		m_localSpatial->updateSelfPosition(pos, forward, right, up);

		origin = point;
	}

}


LRESULT CAgoraSpatialAudioDlg::OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam) {

	canmove = true;

	GetWindowsRelativeRect(m_staLocalMove, &rcLocal);
	int localWidth = rcLocal.right - rcLocal.left;
	int localHeight = rcLocal.bottom - rcLocal.top;
	rcLocal.left = (int)(rcArea.left + (rcArea.right - rcArea.left - localWidth) / 2.0f);
	rcLocal.top = (int)(rcArea.top + (rcArea.bottom - rcArea.top - localHeight) / 2.0f);
	rcLocal.right = rcLocal.left + localWidth;
	rcLocal.bottom = rcLocal.top + localHeight;
	m_staLocalMove.MoveWindow(&rcLocal);

	m_staLocalMove.ShowWindow(TRUE);
	m_staPlayerLeft.ShowWindow(TRUE);
	m_staPlayerRight.ShowWindow(TRUE);

	m_mediaPlayerLeft->open("https://webdemo.agora.io/audiomixing.mp3", 0);
	m_mediaPlayerRight->open("https://webdemo.agora.io/dang.mp3", 0);


	m_staAudioSource.ShowWindow(TRUE);
	m_comAudioSource.ShowWindow(TRUE);
	m_comAudioSource.ResetContent();
	m_comAudioSource.AddString(CONFIG_KEY_PLAYER_LEFT);
	m_comAudioSource.AddString(CONFIG_KEY_PLAYER_RIGHT);
	m_comAudioSource.SetCurSel(0);

	m_configMap.clear();
	m_configMap[CONFIG_KEY_PLAYER_LEFT] = CAgoraSpatialAudioConfig();
	m_configMap[CONFIG_KEY_PLAYER_RIGHT] = CAgoraSpatialAudioConfig();

	m_staMoveTip.ShowWindow(TRUE);


	m_btnJoinChannel.EnableWindow(TRUE);
	m_joinChannel = true;
	m_btnJoinChannel.SetWindowText(commonCtrlLeaveChannel);

	CString strInfo;
	strInfo.Format(_T("%s:join success, uid=%u"), getCurrentTime().AllocSysString(), wParam);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

	//notify parent window
	::PostMessage(GetParent()->GetSafeHwnd(), WM_MSGID(EID_JOINCHANNEL_SUCCESS), TRUE, 0);
	return 0;
}

LRESULT CAgoraSpatialAudioDlg::OnEIDLeaveChannel(WPARAM wParam, LPARAM lParam) {
	canmove = false;

	m_staPlayerLeft.ShowWindow(FALSE);
	m_staPlayerRight.ShowWindow(FALSE);
	m_staRemoteLeft.ShowWindow(FALSE);
	m_staRemoteRight.ShowWindow(FALSE);
	m_staLocalMove.ShowWindow(FALSE);

	m_staAudioSource.ShowWindow(FALSE);
	m_comAudioSource.ShowWindow(FALSE);
	m_chkAudioSourceMute.ShowWindow(FALSE);
	m_chkAudioSourceBlur.ShowWindow(FALSE);
	m_chkAudioSourceAir.ShowWindow(FALSE);
	m_staAudioSourceAttenuation.ShowWindow(FALSE);
	m_sldAudioSourceAttenuation.ShowWindow(FALSE);

	m_staZone.ShowWindow(FALSE);
	m_chkZone.SetCheck(FALSE);

	m_chkLocalMute.SetCheck(FALSE);

	m_staMoveTip.ShowWindow(FALSE);

	if (m_mediaPlayerLeft.get() != nullptr) {
		m_mediaPlayerLeft->stop();
		m_mediaPlayerLeft.reset();
	}
	if (m_mediaPlayerRight.get() != nullptr) {
		m_mediaPlayerRight->stop();
		m_mediaPlayerRight.reset();
	}

	remoteLeftUid = 0;
	remoteRightUid = 0;


	m_btnJoinChannel.EnableWindow(TRUE);
	m_joinChannel = false;
	m_btnJoinChannel.SetWindowText(commonCtrlJoinChannel);

	CString strInfo;
	strInfo.Format(_T("leave channel success %s"), getCurrentTime().AllocSysString());
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



void CAgoraSpatialAudioDlg::OnCbnSelchangeComboAudioSource()
{
	CString strChoosed;
	m_comAudioSource.GetLBText(m_comAudioSource.GetCurSel(), strChoosed);

	if (m_configMap.find(strChoosed) != m_configMap.end()) {
		CAgoraSpatialAudioConfig& config = m_configMap[strChoosed];

		m_chkAudioSourceMute.ShowWindow(TRUE);
		m_chkAudioSourceBlur.ShowWindow(TRUE);
		m_chkAudioSourceAir.ShowWindow(TRUE);
		m_staAudioSourceAttenuation.ShowWindow(TRUE);
		m_sldAudioSourceAttenuation.ShowWindow(TRUE);

		m_chkAudioSourceMute.SetCheck(config.mute);
		m_chkAudioSourceBlur.SetCheck(config.blur);
		m_chkAudioSourceAir.SetCheck(config.airborne);
		m_sldAudioSourceAttenuation.SetRange(0, 100);
		m_sldAudioSourceAttenuation.SetPos((int)(100 * config.attenuation));
	}
}


void CAgoraSpatialAudioDlg::OnBnClickedCheckAudioSourceMute()
{
	CString strChoosed;
	m_comAudioSource.GetLBText(m_comAudioSource.GetCurSel(), strChoosed);
	if (m_configMap.find(strChoosed) != m_configMap.end()) {
		CAgoraSpatialAudioConfig& config = m_configMap[strChoosed];

		BOOL isChecked = m_chkAudioSourceMute.GetCheck();
		config.mute = isChecked;

		if (strChoosed == CONFIG_KEY_PLAYER_LEFT) {
			if (isChecked) {
				m_mediaPlayerLeft->pause();
			}
			else {
				m_mediaPlayerLeft->resume();
			}
		}
		else if (strChoosed == CONFIG_KEY_PLAYER_RIGHT) {
			if (isChecked) {
				m_mediaPlayerRight->pause();
			}
			else {
				m_mediaPlayerRight->resume();
			}
		}
		else if (strChoosed == CONFIG_KEY_REMOTE_LEFT && remoteLeftUid != 0) {
			m_localSpatial->muteRemoteAudioStream(remoteLeftUid, isChecked);
		}
		else if (strChoosed == CONFIG_KEY_REMOTE_RIGHT && remoteRightUid != 0) {
			m_localSpatial->muteRemoteAudioStream(remoteRightUid, isChecked);
		}
	}
}


void CAgoraSpatialAudioDlg::OnBnClickedCheckAudioSourceBlur()
{
	CString strChoosed;
	m_comAudioSource.GetLBText(m_comAudioSource.GetCurSel(), strChoosed);
	if (m_configMap.find(strChoosed) != m_configMap.end()) {
		CAgoraSpatialAudioConfig& config = m_configMap[strChoosed];

		BOOL isChecked = m_chkAudioSourceBlur.GetCheck();
		config.blur = isChecked;

		SpatialAudioParams param;
		param.enable_blur = isChecked;
		param.enable_air_absorb = m_chkAudioSourceAir.GetCheck();
		param.speaker_attenuation = m_sldAudioSourceAttenuation.GetPos() / 100.0f;
		if (strChoosed == CONFIG_KEY_PLAYER_LEFT) {
			m_mediaPlayerLeft->setSpatialAudioParams(param);
		}
		else if (strChoosed == CONFIG_KEY_PLAYER_RIGHT) {
			m_mediaPlayerRight->setSpatialAudioParams(param);
		}
		else if (strChoosed == CONFIG_KEY_REMOTE_LEFT && remoteLeftUid != 0) {
			m_rtcEngine->setRemoteUserSpatialAudioParams(remoteLeftUid, param);
		}
		else if (strChoosed == CONFIG_KEY_REMOTE_RIGHT && remoteRightUid != 0) {
			m_rtcEngine->setRemoteUserSpatialAudioParams(remoteRightUid, param);
		}

	}
}


void CAgoraSpatialAudioDlg::OnBnClickedCheckAudioSourceAirborne()
{
	CString strChoosed;
	m_comAudioSource.GetLBText(m_comAudioSource.GetCurSel(), strChoosed);
	if (m_configMap.find(strChoosed) != m_configMap.end()) {
		CAgoraSpatialAudioConfig& config = m_configMap[strChoosed];

		BOOL isChecked = m_chkAudioSourceAir.GetCheck();
		config.airborne = isChecked;

		SpatialAudioParams param;
		param.enable_blur = m_chkAudioSourceBlur.GetCheck();
		param.enable_air_absorb = isChecked;
		param.speaker_attenuation = m_sldAudioSourceAttenuation.GetPos() / 100.0f;

		if (strChoosed == CONFIG_KEY_PLAYER_LEFT) {
			m_mediaPlayerLeft->setSpatialAudioParams(param);
		}
		else if (strChoosed == CONFIG_KEY_PLAYER_RIGHT) {
			m_mediaPlayerRight->setSpatialAudioParams(param);
		}
		else if (strChoosed == CONFIG_KEY_REMOTE_LEFT && remoteLeftUid != 0) {
			m_rtcEngine->setRemoteUserSpatialAudioParams(remoteLeftUid, param);
		}
		else if (strChoosed == CONFIG_KEY_REMOTE_RIGHT && remoteRightUid != 0) {
			m_rtcEngine->setRemoteUserSpatialAudioParams(remoteRightUid, param);
		}

	}
}


void CAgoraSpatialAudioDlg::OnNMCustomdrawSliderAudioSourceAttenuation(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	CString strChoosed;
	m_comAudioSource.GetLBText(m_comAudioSource.GetCurSel(), strChoosed);
	if (m_configMap.find(strChoosed) != m_configMap.end()) {
		CAgoraSpatialAudioConfig& config = m_configMap[strChoosed];

		float attenuation = m_sldAudioSourceAttenuation.GetPos() / 100.0f;
		config.attenuation = attenuation;

		if (strChoosed == CONFIG_KEY_PLAYER_LEFT) {
			SpatialAudioParams param;
			param.enable_blur = m_chkAudioSourceBlur.GetCheck();
			param.enable_air_absorb = m_chkAudioSourceAir.GetCheck();
			param.speaker_attenuation = attenuation;
			m_mediaPlayerLeft->setSpatialAudioParams(param);
		}
		else if (strChoosed == CONFIG_KEY_PLAYER_RIGHT) {
			SpatialAudioParams param;
			param.enable_blur = m_chkAudioSourceBlur.GetCheck();
			param.enable_air_absorb = m_chkAudioSourceAir.GetCheck();
			param.speaker_attenuation = attenuation;
			m_mediaPlayerRight->setSpatialAudioParams(param);
		}
		else if (strChoosed == CONFIG_KEY_REMOTE_LEFT && remoteLeftUid != 0) {
			m_localSpatial->setRemoteAudioAttenuation(remoteLeftUid, attenuation, FALSE);
		}
		else if (strChoosed == CONFIG_KEY_REMOTE_RIGHT && remoteRightUid != 0) {
			m_localSpatial->setRemoteAudioAttenuation(remoteRightUid, attenuation, FALSE);
		}
	}

	*pResult = 0;
}


void CAgoraSpatialAudioDlg::OnBnClickedCheckAudioZone()
{
	BOOL isChecked = m_chkZone.GetCheck();
	if (m_joinChannel) {
		m_staZone.ShowWindow(isChecked);

		if (isChecked) {
			SpatialAudioZone mediaPlayerLeftZone;
			memset(&mediaPlayerLeftZone, 0, sizeof(SpatialAudioZone));
			mediaPlayerLeftZone.audioAttenuation = 1.0f;

			// zone center point position
			CaculateObjectPosition(m_staZone, mediaPlayerLeftZone.position);

			// zone look
			mediaPlayerLeftZone.forward[0] = 1.0f;
			mediaPlayerLeftZone.right[1] = 1.0f;
			mediaPlayerLeftZone.up[2] = 1.0f;

			// zone size
			RECT zoneSize;
			CaculateZoneRect(m_staZone, &zoneSize);
			mediaPlayerLeftZone.forwardLength = zoneSize.bottom - zoneSize.top * 1.0f;
			mediaPlayerLeftZone.rightLength = zoneSize.right - zoneSize.left * 1.0f;
			mediaPlayerLeftZone.upLength = AXIS_MAX_DISTANCE;

			m_localSpatial->setZones(&mediaPlayerLeftZone, 1);

			CString strInfo;
			strInfo.Format(_T("Zone position forward=%f, right=%f"), mediaPlayerLeftZone.position[0], mediaPlayerLeftZone.position[1]);
			m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
			strInfo.Format(_T("Zone size forwardLength=%f, rightLength=%f"), mediaPlayerLeftZone.forwardLength, mediaPlayerLeftZone.rightLength);
			m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
		}
		else {
			m_localSpatial->setZones(nullptr, 0);
		}
	}
}


void CAgoraSpatialAudioDlg::OnBnClickedCheckMuteLocalAudio()
{
	BOOL isChecked = m_chkLocalMute.GetCheck();
	if (m_joinChannel) {
		m_localSpatial->muteLocalAudioStream(isChecked);
	}
}