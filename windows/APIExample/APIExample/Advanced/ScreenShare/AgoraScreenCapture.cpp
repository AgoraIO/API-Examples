﻿#include "../../stdafx.h"
#include "../../APIExample.h"
#include "AgoraScreenCapture.h"
#include "afxdialogex.h"


IMPLEMENT_DYNAMIC(CAgoraScreenCapture, CDialogEx)

CAgoraScreenCapture::CAgoraScreenCapture(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_SCREEN_SHARE, pParent)
{

}

CAgoraScreenCapture::~CAgoraScreenCapture()
{
}

void CAgoraScreenCapture::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_VIDEO, m_staVideoArea);
	DDX_Control(pDX, IDC_STATIC_CHANNELNAME, m_staChannel);
	DDX_Control(pDX, IDC_EDIT_CHANNELNAME, m_edtChannel);
	DDX_Control(pDX, IDC_STATIC_SCREEN_CAPTURE, m_staScreenCap);
	DDX_Control(pDX, IDC_COMBO_SCREEN_CAPTURE, m_cmbScreenCap);
	DDX_Control(pDX, IDC_BUTTON_START_CAPUTRE, m_btnStartCap);
	DDX_Control(pDX, IDC_BUTTON_JOINCHANNEL, m_btnJoinChannel);
	DDX_Control(pDX, IDC_LIST_INFO_BROADCASTING, m_lstInfo);
}
//set control text from config.
void CAgoraScreenCapture::InitCtrlText()
{
	m_staScreenCap.SetWindowText(screenShareCtrlScreenCap);
	m_btnStartCap.SetWindowText(screenShareCtrlStartCap);
	m_staChannel.SetWindowText(commonCtrlChannel);
	m_btnJoinChannel.SetWindowText(commonCtrlJoinChannel);
}

//Initialize the Agora SDK
bool CAgoraScreenCapture::InitAgora()
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
	//enable video in the engine.
	m_rtcEngine->enableVideo();
	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("enable video"));
	//set channel profile in the engine to the CHANNEL_PROFILE_LIVE_BROADCASTING.
	m_rtcEngine->setChannelProfile(CHANNEL_PROFILE_LIVE_BROADCASTING);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("live broadcasting"));
	//set clinet role in the engine to the CLIENT_ROLE_BROADCASTER.
	m_rtcEngine->setClientRole(CLIENT_ROLE_BROADCASTER);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("setClientRole broadcaster"));

	m_btnJoinChannel.EnableWindow(TRUE);
	return true;
}
//UnInitialize the Agora SDK
void CAgoraScreenCapture::UnInitAgora()
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
//render local video from SDK local capture.
void CAgoraScreenCapture::RenderLocalVideo()
{
	if (m_rtcEngine) {
		//start preview in the engine.
		m_rtcEngine->startPreview();
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("startPreview"));
		VideoCanvas canvas;
		canvas.renderMode = RENDER_MODE_FIT;
		canvas.uid = 0;
		canvas.view = m_localVideoWnd.GetSafeHwnd();
		//setup local video in the engine to canvas.
		m_rtcEngine->setupLocalVideo(canvas);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("setupLocalVideo"));

	}
}

//EID_JOINCHANNEL_SUCCESS message window handler.
LRESULT CAgoraScreenCapture::OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam)
{
	m_btnJoinChannel.EnableWindow(TRUE);
	m_btnStartCap.EnableWindow(TRUE);
	joinChannel = true;
	m_btnJoinChannel.SetWindowText(commonCtrlLeaveChannel);

	CString strInfo;
	strInfo.Format(_T("%s:join success, uid=%u"), getCurrentTime(), wParam);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

	m_localVideoWnd.SetUID(wParam);

	//notify parent window
	::PostMessage(GetParent()->GetSafeHwnd(), WM_MSGID(EID_JOINCHANNEL_SUCCESS), TRUE, 0);
	return 0;
}
//EID_LEAVE_CHANNEL message window handler.
LRESULT CAgoraScreenCapture::OnEIDLeaveChannel(WPARAM wParam, LPARAM lParam)
{
	m_btnJoinChannel.EnableWindow(TRUE);
	m_btnStartCap.EnableWindow(FALSE);

	joinChannel = false;
	m_btnJoinChannel.SetWindowText(commonCtrlJoinChannel);

	CString strInfo;
	strInfo.Format(_T("leave channel success %s"), getCurrentTime());
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

	::PostMessage(GetParent()->GetSafeHwnd(), WM_MSGID(EID_JOINCHANNEL_SUCCESS), FALSE, 0);
	return 0;
}
//EID_USER_JOINED message window handler.
LRESULT CAgoraScreenCapture::OnEIDUserJoined(WPARAM wParam, LPARAM lParam)
{
	CString strInfo;
	strInfo.Format(_T("%u joined"), wParam);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

	return 0;
}
//EID_USER_OFFLINE message window handler.
LRESULT CAgoraScreenCapture::OnEIDUserOffline(WPARAM wParam, LPARAM lParam)
{
	uid_t remoteUid = (uid_t)wParam;
	VideoCanvas canvas;
	canvas.uid = remoteUid;
	canvas.view = NULL;
	//setup remote video in the engine to canvas.
	m_rtcEngine->setupRemoteVideo(canvas);
	CString strInfo;
	strInfo.Format(_T("%u offline, reason:%d"), remoteUid, lParam);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	return 0;
}
//EID_REMOTE_VIDEO_STATE_CHANED message window handler.
LRESULT CAgoraScreenCapture::OnEIDRemoteVideoStateChanged(WPARAM wParam, LPARAM lParam)
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



BEGIN_MESSAGE_MAP(CAgoraScreenCapture, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_JOINCHANNEL, &CAgoraScreenCapture::OnBnClickedButtonJoinchannel)
	ON_BN_CLICKED(IDC_BUTTON_START_CAPUTRE, &CAgoraScreenCapture::OnBnClickedButtonStartShare)
	ON_MESSAGE(WM_MSGID(EID_JOINCHANNEL_SUCCESS), &CAgoraScreenCapture::OnEIDJoinChannelSuccess)
	ON_MESSAGE(WM_MSGID(EID_LEAVE_CHANNEL), &CAgoraScreenCapture::OnEIDLeaveChannel)
	ON_MESSAGE(WM_MSGID(EID_USER_JOINED), &CAgoraScreenCapture::OnEIDUserJoined)
	ON_MESSAGE(WM_MSGID(EID_USER_OFFLINE), &CAgoraScreenCapture::OnEIDUserOffline)
	ON_MESSAGE(WM_MSGID(EID_REMOTE_VIDEO_STATE_CHANED), &CAgoraScreenCapture::OnEIDRemoteVideoStateChanged)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()



/*
	initialize dialog, and set control proprety.
*/
BOOL CAgoraScreenCapture::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	InitCtrlText();
	m_localVideoWnd.Create(NULL, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CRect(0, 0, 1, 1), this, ID_BASEWND_VIDEO + 100);
	m_remoteVideoWnd.Create(NULL, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CRect(0, 0, 1, 1), this, ID_BASEWND_VIDEO + 100);

	RECT rcArea;
	m_staVideoArea.GetClientRect(&rcArea);
	m_localVideoWnd.MoveWindow(&rcArea);
	m_localVideoWnd.ShowWindow(SW_SHOW);
	m_btnStartCap.EnableWindow(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


//The JoinChannel button's click handler.
//This function either joins or leaves the channel
void CAgoraScreenCapture::OnBnClickedButtonJoinchannel()
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
		if (0 == m_rtcEngine->joinChannel(APP_TOKEN, szChannelId.c_str(), "", 0)) {
			strInfo.Format(_T("join channel %s"), getCurrentTime());
			m_btnJoinChannel.EnableWindow(FALSE);
		}
	}
	else {
		if (0 == m_rtcEngine->leaveChannel()) {
			strInfo.Format(_T("leave channel %s"), getCurrentTime());
			m_btnJoinChannel.EnableWindow(FALSE);
		}
	}

	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
}

// start or stop screen capture.
void CAgoraScreenCapture::OnBnClickedButtonStartShare()
{
	if (!m_rtcEngine || !m_initialize)
		return;
	HWND hWnd = ::GetDesktopWindow();
	if (m_cmbScreenCap.GetCurSel() != m_cmbScreenCap.GetCount()-1)
		hWnd = m_listWnd.GetAt(m_listWnd.FindIndex(m_cmbScreenCap.GetCurSel()));
	int ret = 0;
	m_screenShare = !m_screenShare;
	if (m_screenShare)
	{
		//start screen capture in the engine.
		ret = m_rtcEngine->startScreenCapture(hWnd, 15, NULL, TRUE);
		if (ret == 0)
			m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("screen share start succees！"));
		else
			m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("screen share start failed！"));

		m_btnStartCap.SetWindowText(screenShareCtrlEndCap);
	}
	else {
		//stop screen capture in the engine.
		ret = m_rtcEngine->stopScreenCapture();
		if(ret == 0)
			m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("screen share stop succees！"));
		else
			m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("screen share stop failed！"));
		m_btnStartCap.SetWindowText(screenShareCtrlStartCap);
	}
}

// render local video and refresh zoomed window add m_cmbScreenCap.
void CAgoraScreenCapture::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);
	if (bShow) {
		RenderLocalVideo();
		ReFreshWnd();
	}
}

// call RefreashWndInfo to refresh window list and to m_cmbScreenCap.
void CAgoraScreenCapture::ReFreshWnd()
{
	//refresh window info.
	RefreashWndInfo();
	POSITION	pos = m_listWnd.GetHeadPosition();
	HWND		hWnd = NULL;
	TCHAR		strName[255];
	int index = 0;
	//enumrate hwnd to add m_cmbScreenCap.
	while (pos != NULL) {
		hWnd = m_listWnd.GetNext(pos);
		if (::IsZoomed(hWnd))
		{
			::GetWindowText(hWnd, strName, 255);
			m_cmbScreenCap.InsertString(index++, strName);
		}
	}
	m_cmbScreenCap.InsertString(index++, L"DeskTop");

	m_cmbScreenCap.SetCurSel(0);
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
void CScreenCaputreEventHandler::onJoinChannelSuccess(const char* channel, uid_t uid, int elapsed)
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
void CScreenCaputreEventHandler::onUserJoined(uid_t uid, int elapsed)
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
void CScreenCaputreEventHandler::onUserOffline(uid_t uid, USER_OFFLINE_REASON_TYPE reason)
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

void CScreenCaputreEventHandler::onLeaveChannel(const RtcStats& stats)
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
void CScreenCaputreEventHandler::onRemoteVideoStateChanged(uid_t uid, REMOTE_VIDEO_STATE state, REMOTE_VIDEO_STATE_REASON reason, int elapsed)
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
	enum window callback function.
*/
BOOL CALLBACK CAgoraScreenCapture::WndEnumProc(HWND hWnd, LPARAM lParam)
{
	CList<HWND>* lpListctrl = (CList<HWND>*)lParam;

	LONG lStyle = ::GetWindowLong(hWnd, GWL_STYLE);

	if ((lStyle & WS_VISIBLE) != 0 && (lStyle & (WS_POPUP | WS_SYSMENU)) != 0
		&&::IsZoomed(hWnd))
		lpListctrl->AddTail(hWnd);

	return TRUE;
}

/*
	refresh zoomed window.and add to m_listWnd.
*/
int	 CAgoraScreenCapture::RefreashWndInfo()
{
	m_listWnd.RemoveAll();
	::EnumWindows(&CAgoraScreenCapture::WndEnumProc, (LPARAM)&m_listWnd);
	return static_cast<int>(m_listWnd.GetCount());
}

