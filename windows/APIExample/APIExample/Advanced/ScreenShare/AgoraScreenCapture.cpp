#include "stdafx.h"
#include "APIExample.h"
#include "AgoraScreenCapture.h"
#include <dwmapi.h>
#pragma comment(lib,"dwmapi.lib")

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
	DDX_Control(pDX, IDC_CHECK_CURSOR, m_chkShareCursor);
	DDX_Control(pDX, IDC_EDIT_FPS, m_edtFPS);
	DDX_Control(pDX, IDC_EDIT_BITRATE, m_edtBitrate);
	DDX_Control(pDX, IDC_COMBO_SCREEN_SCREEN, m_cmbScreenRegion);


	DDX_Control(pDX, IDC_BUTTON_START_SHARE_SCREEN, m_btnShareScreen);
	DDX_Control(pDX, IDC_STATIC_SCREEN_INFO, m_staScreenInfo);
	DDX_Control(pDX, IDC_STATIC_SCREEN_INFO2, m_staScreenInfo2);
	DDX_Control(pDX, IDC_STATIC_FPS, m_staFPS);
	DDX_Control(pDX, IDC_STATIC_BITRATE, m_staBitrate);
	DDX_Control(pDX, IDC_STATIC_GENERAL, m_staGeneral);
	DDX_Control(pDX, IDC_BUTTON_UPDATEPARAM, m_btnUpdateCaptureParam);
	DDX_Control(pDX, IDC_STATIC_SCREEN_SHARE, m_StaScreen);
}
//set control text from config.
void CAgoraScreenCapture::InitCtrlText()
{
	m_StaScreen.SetWindowText(screenShareCtrlScreen);
	m_staFPS.SetWindowText(screenShareCtrlFPS);
	m_staBitrate.SetWindowText(screenShareCtrlBitrate);
	m_chkShareCursor.SetWindowText(screenShareCtrlShareCursor);
	m_staGeneral.SetWindowText(screenShareCtrlGeneralSettings);
	m_btnUpdateCaptureParam.SetWindowText(screenShareCtrlUpdateCaptureParam);
	m_btnShareScreen.SetWindowText(screenShareCtrlShareSCreen);
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
	//disable video in the engine.
	//m_rtcEngine->enableLocalVideo(false);
	//m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("enable video"));
	
	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("live broadcasting"));
	//set client role in the engine to the CLIENT_ROLE_BROADCASTER.
	m_rtcEngine->setClientRole(CLIENT_ROLE_BROADCASTER);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("setClientRole broadcaster"));

	m_btnJoinChannel.EnableWindow(TRUE);
	return true;
}
//UnInitialize the Agora SDK
void CAgoraScreenCapture::UnInitAgora()
{
	if (m_rtcEngine) {
		if(m_joinChannel)
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
void CAgoraScreenCapture::RenderLocalVideo()
{
	if (m_rtcEngine) {

		VideoCanvas canvas;
		canvas.renderMode = media::base::RENDER_MODE_FIT;
		canvas.uid = 0;
		canvas.view = m_localVideoWnd.GetSafeHwnd();
		canvas.sourceType = VIDEO_SOURCE_SCREEN_PRIMARY;
		canvas.mirrorMode = VIDEO_MIRROR_MODE_DISABLED;
		//setup local video in the engine to canvas.
		m_rtcEngine->setupLocalVideo(canvas);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("setupLocalVideo"));
	}
}


//EID_JOINCHANNEL_SUCCESS message window handler.
LRESULT CAgoraScreenCapture::OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam)
{
	m_btnJoinChannel.EnableWindow(TRUE);
	//m_btnStartCap.EnableWindow(FALSE);
	//m_btnShareScreen.EnableWindow(FALSE);
	m_joinChannel = true;
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
LRESULT CAgoraScreenCapture::OnEIDLeaveChannel(WPARAM wParam, LPARAM lParam)
{
	m_btnJoinChannel.EnableWindow(TRUE);
	//m_btnStartCap.EnableWindow(TRUE);
	//m_btnShareScreen.EnableWindow(TRUE);
	m_joinChannel = false;
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
	
	CString strInfo;
	strInfo.Format(_T("%u offline, reason:%d"), remoteUid, lParam);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	return 0;
}
//EID_REMOTE_VIDEO_STATE_CHANGED message window handler.
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
	ON_MESSAGE(WM_MSGID(EID_REMOTE_VIDEO_STATE_CHANGED), &CAgoraScreenCapture::OnEIDRemoteVideoStateChanged)
	ON_WM_SHOWWINDOW()
    ON_BN_CLICKED(IDC_BUTTON_UPDATEPARAM, &CAgoraScreenCapture::OnBnClickedButtonUpdateparam)
   // ON_BN_CLICKED(IDC_BUTTON_SHARE_DESKTOP, &CAgoraScreenCapture::OnBnClickedButtonShareDesktop)
   // ON_CBN_SELCHANGE(IDC_COMBO_SCREEN_REGION, &CAgoraScreenCapture::OnCbnSelchangeComboScreenRegion)
    ON_BN_CLICKED(IDC_BUTTON_START_SHARE_SCREEN, &CAgoraScreenCapture::OnBnClickedButtonStartShareScreen)
END_MESSAGE_MAP()



/*
	initialize dialog, and set control property.
*/
BOOL CAgoraScreenCapture::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_localVideoWnd.Create(NULL, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CRect(0, 0, 1, 1), this, ID_BASEWND_VIDEO + 100);
	RECT rcArea;
	m_staVideoArea.GetClientRect(&rcArea);
	m_localVideoWnd.MoveWindow(&rcArea);
	m_localVideoWnd.ShowWindow(SW_SHOW);
	ResumeStatus();
    InitMonitorInfos();
	return TRUE;  
}

void CAgoraScreenCapture::InitMonitorInfos()
{
   m_monitors.EnumMonitor();

    std::vector<CMonitors::MonitorInformation>  infos = m_monitors.GetMonitors();
	for (size_t i = 0; i < infos.size(); i++) {
		char szName[MAX_PATH] = { 0 };
		sprintf_s(szName, MAX_PATH, "Display%d", i + 1);
		CString strDevice = infos[i].monitorInfo.szDevice;
		strDevice.Replace(L"\\\\.\\", L"");
		m_cmbScreenRegion.InsertString(i, strDevice.GetBuffer(0));
	}
	m_cmbScreenRegion.SetCurSel(0);

	/*CString str = _T("");
    for (size_t i = 0; i < infos.size(); i++) {
        RECT rcMonitor = infos[i].monitorInfo.rcMonitor;
        CString strInfo;
        strInfo.Format(_T("Screen%d: rect = {%d, %d, %d, %d} ")
            , i + 1, rcMonitor.left, rcMonitor.top, rcMonitor.right, rcMonitor.bottom);
        if (rcMonitor.left < 0 || rcMonitor.top < 0) {//negative coordinate is not supported
			strInfo += _T("not support negative cordinate;");
			str += strInfo;
			continue;
		}
        str += strInfo;
        m_cmbScreenRegion.InsertString(i, utf82cs(infos[i].monitorName));
    }

    m_cmbScreenRegion.InsertString(infos.size(), _T("Select Window Hwnd Rect Area"));
    m_staScreenInfo.SetWindowText(str);*/
    

	//m_cmbScreenRegion.SetCurSel(0);

}

//The JoinChannel button's click handler.
//This function either joins or leaves the channel
void CAgoraScreenCapture::OnBnClickedButtonJoinchannel()
{
	if (!m_rtcEngine || !m_initialize)
		return;

	if (!m_screenShare && !m_windowShare && !m_joinChannel) {
		CString strInfo = _T("you need share window or screen first");
		m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
		return;
	}

	CString strInfo;
	if (!m_joinChannel) {
		CString strChannelName;
		m_edtChannel.GetWindowText(strChannelName);
		if (strChannelName.IsEmpty()) {
			AfxMessageBox(_T("Fill channel name first"));
			return;
		}
		ChannelMediaOptions option;
		option.channelProfile = CHANNEL_PROFILE_LIVE_BROADCASTING;
		option.clientRoleType = CLIENT_ROLE_BROADCASTER;
		option.autoSubscribeAudio = true; option.autoSubscribeVideo = true;
		option.publishAudioTrack = true;
		option.publishScreenTrack = true;
		option.publishCameraTrack = false;
		std::string szChannelId = cs2utf8(strChannelName);
		if (0 == m_rtcEngine->joinChannel(APP_TOKEN, szChannelId.c_str(), 0, option)) {
			strInfo.Format(_T("join channel %s, use ChannelMediaOption"), getCurrentTime());
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
    HWND hWnd = NULL;
    //if (m_cmbScreenCap.GetCurSel() != m_cmbScreenCap.GetCount() - 1)
    hWnd = m_listWnd.GetAt(m_listWnd.FindIndex(m_cmbScreenCap.GetCurSel()));
    int ret = 0;
    m_windowShare = !m_windowShare;
    if (m_windowShare)
    {
        ::SwitchToThisWindow(hWnd, TRUE);
        //start screen capture in the engine.
        ScreenCaptureParameters capParam;
        GetCaptureParameterFromCtrl(capParam);
        CRect rcWnd = { 0 };
        ::GetClientRect(hWnd, &rcWnd);
        agora::rtc::Rectangle rcCapWnd = { rcWnd.left, rcWnd.top, rcWnd.right - rcWnd.left, rcWnd.bottom - rcWnd.top };
        ret = m_rtcEngine->startScreenCaptureByWindowId(hWnd, rcCapWnd, capParam);
		//start preview in the engine.
		m_rtcEngine->startPreview();
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("startPreview"));
		//disable video in the engine.
		//m_rtcEngine->enableLocalVideo(false);
		//m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("disable video in the engine"));
		if (m_joinChannel) {
			ChannelMediaOptions option;
			option.channelProfile = CHANNEL_PROFILE_LIVE_BROADCASTING;
			option.clientRoleType = CLIENT_ROLE_BROADCASTER;
			option.autoSubscribeAudio = true; option.autoSubscribeVideo = true;
			option.publishAudioTrack = true;
			option.publishScreenTrack = true;
			option.publishCameraTrack = false;
			m_rtcEngine->updateChannelMediaOptions(option);
			m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("updateChannelMediaOptions"));
		}
		
        if (ret== 0)
            m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("start share window succees！"));
        else
            m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("start share window failed！"));
        m_btnStartCap.SetWindowText(screenShareCtrlEndCap);
        m_btnShareScreen.EnableWindow(FALSE);
		
    }
    else {
        //stop screen capture in the engine.
        ret = m_rtcEngine->stopScreenCapture();
        if (ret == 0)
            m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("stop screen share stop succees！"));
        else
            m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("stop screen share stop failed！"));
        m_btnStartCap.SetWindowText(screenShareCtrlStartCap);
        m_btnShareScreen.EnableWindow(TRUE);
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
	else {
		ResumeStatus();
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
	//enumerate hwnd to add m_cmbScreenCap.
	while (pos != NULL) {
		hWnd = m_listWnd.GetNext(pos);
		::GetWindowText(hWnd, strName, 255);
		m_cmbScreenCap.InsertString(index++, strName);
	}
	//m_cmbScreenCap.InsertString(index++, L"DeskTop");
	m_cmbScreenCap.SetCurSel(0);
}

//Get ScreenCaptureParameters from ctrl
void CAgoraScreenCapture::GetCaptureParameterFromCtrl(agora::rtc::ScreenCaptureParameters& capParam)
{
    //capParam.captureMouseCursor = m_chkShareCursor.GetCheck();
    CString str;
    m_edtFPS.GetWindowText(str);
    if (str.IsEmpty()) 
        capParam.frameRate = 15; //default fps
    else 
        capParam.frameRate = _ttoi(str);
   
    str.Empty();
    m_edtBitrate.GetWindowText(str);
    if (!str.IsEmpty())
        capParam.bitrate = _ttoi(str);
    else
        capParam.bitrate = 0;//default

}

//resume window status
void CAgoraScreenCapture::ResumeStatus()
{
    m_lstInfo.ResetContent();
    InitCtrlText();
	
    m_joinChannel = false;
    m_initialize = false;
    m_addInjectStream = false;
    m_windowShare = false;
    m_screenShare = false;
    m_edtChannel.SetWindowText(_T(""));
    m_cmbScreenCap.ResetContent();
	m_chkShareCursor.EnableWindow(FALSE);
    m_chkShareCursor.SetCheck(TRUE);
    m_edtFPS.SetWindowText(_T("15"));
    m_edtBitrate.SetWindowText(_T(""));
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
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_REMOTE_VIDEO_STATE_CHANGED), (WPARAM)stateChanged, 0);
	}
}

static
BOOL IsWindowCloaked(HWND hwnd)
{
	BOOL isCloaked = FALSE;
	return (SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED,
		&isCloaked, sizeof(isCloaked))) && isCloaked);
}
static
BOOL IsWindowVisibleOnScreen(HWND hwnd)
{
	return IsWindowVisible(hwnd) &&
		!IsWindowCloaked(hwnd);
}

/*
	enum window callback function.
*/
BOOL CALLBACK CAgoraScreenCapture::WndEnumProc(HWND hWnd, LPARAM lParam)
{
	CList<HWND>* lpListctrl = (CList<HWND>*)lParam;
	TCHAR strName[255];
	::GetWindowText(hWnd, strName, 255);
	CString str = strName;
	LONG lStyle = ::GetWindowLong(hWnd, GWL_STYLE);
	if ((lStyle & WS_VISIBLE) != 0 
		&& (lStyle & (WS_POPUP | WS_SYSMENU)) != 0
		&& IsWindowVisibleOnScreen(hWnd)
		&& !str.IsEmpty()
		&& str.Compare(_T("Program Manager"))
		//&&::IsZoomed(hWnd)
		)
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


BOOL CAgoraScreenCapture::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) {
		return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}


void CAgoraScreenCapture::OnBnClickedButtonUpdateparam()
{
    ScreenCaptureParameters capParam;
    GetCaptureParameterFromCtrl(capParam);
    m_rtcEngine->updateScreenCaptureParameters(capParam);
}


CMonitors::CMonitors()
{

}
CMonitors::~CMonitors()
{

}


BOOL CMonitors::MonitorFunc(HMONITOR hMonitor, HDC hDc, LPRECT lpRect, LPARAM lParam)
{
    CMonitors* pThis = (CMonitors*)lParam;

    MONITORINFOEX info;
    info.cbSize = sizeof(MONITORINFOEX);
    GetMonitorInfo(hMonitor, &info);

    DEVMODE devMode;
    devMode.dmSize = sizeof(DEVMODE);
    EnumDisplaySettings(info.szDevice, ENUM_CURRENT_SETTINGS, &devMode);

    DEVICE_SCALE_FACTOR scale;
    HRESULT hr = GetScaleFactorForMonitor(hMonitor, &scale);
    MonitorInformation monitorInfo;
    monitorInfo.monitorInfo = info;
    monitorInfo.hMonitor = hMonitor;
    if (info.rcMonitor.left < 0
        || info.rcMonitor.top < 0) {
        monitorInfo.canShare = false;
    }
   // UINT dpiX = 0;
   // UINT dpiY = 0;
   // GetDpiForMonitor(hMonitor, MDT_EFFECTIVE_DPI ,&dpiX, &dpiY);
    pThis->m_vecMonitorInfos.push_back(monitorInfo);
    if(monitorInfo.canShare)
        pThis->m_vecEffectiveMonitorInfos.push_back(monitorInfo);
    return TRUE;
}

void CMonitors::Clear()
{
    m_vecMonitorInfos.clear();
    m_screenRegion = { 0,0,0,0 };
}

void CMonitors::EnumMonitor()
{
    Clear();
    EnumDisplayMonitors(NULL, NULL, (MONITORENUMPROC)MonitorFunc, (LPARAM)this);

    for (size_t i = 0; i < m_vecMonitorInfos.size(); i++) {
        MonitorInformation& monitorInfo = m_vecMonitorInfos[i];
        char szName[MAX_PATH] = { 0 };
        sprintf_s(szName, MAX_PATH, "Screen%d", i + 1);
        monitorInfo.monitorName = szName;
    }

    if (m_vecMonitorInfos.size() == 1) {
        m_screenRegion = m_vecMonitorInfos[0].monitorInfo.rcMonitor;
    }
    else {
        m_screenRegion.left = GetSystemMetrics(SM_XVIRTUALSCREEN);
        m_screenRegion.top = GetSystemMetrics(SM_YVIRTUALSCREEN);
        m_screenRegion.right = m_screenRegion.left + GetSystemMetrics(SM_CXVIRTUALSCREEN);
        m_screenRegion.bottom = m_screenRegion.top + GetSystemMetrics(SM_CYVIRTUALSCREEN);
    }
}

agora::rtc::Rectangle CMonitors::GetScreenRect()
{
    return RectToRectangle(m_screenRegion);
}

agora::rtc::Rectangle CMonitors::RectToRectangle(RECT rc)
{
    agora::rtc::Rectangle agoraRect = { rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top };

    return agoraRect;
}

bool CMonitors::IsValid()
{
    int left = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int top = GetSystemMetrics(SM_YVIRTUALSCREEN);

    if (left < 0 || top < 0) {
        return false;
    }

    return true;
}

CMonitors::MonitorInformation CMonitors::GetMonitorInformation(int index)
{
    return m_vecEffectiveMonitorInfos[index];
}

agora::rtc::Rectangle CMonitors::GetMonitorRectangle(int index)
{
    MonitorInformation& info = m_vecEffectiveMonitorInfos[index];
    RECT rc = info.monitorInfo.rcMonitor;

    agora::rtc::Rectangle rcAgora = { 0, 0, (rc.right - rc.left)*info.scale_num / info.scale_den, (rc.bottom - rc.top)*info.scale_num / info.scale_den };
    rcAgora.x = rc.left * info.scale_num / info.scale_den;
    rcAgora.y = rc.top * info.scale_num / info.scale_den;
    return rcAgora;
}

bool CMonitors::CheckMonitorValid(HMONITOR hMonitor)
{
    for (size_t i = 0; i < m_vecEffectiveMonitorInfos.size(); i++){
        MonitorInformation info = m_vecEffectiveMonitorInfos[i];
        if (info.hMonitor == hMonitor) {
            return info.canShare;
        }
    }

    return false;
}

bool CMonitors::GetMonitorRectangle(HMONITOR hMonitor, agora::rtc::Rectangle& screenRegion)
{
    for (size_t i = 0; i < m_vecEffectiveMonitorInfos.size(); i++) {
        MonitorInformation info = m_vecEffectiveMonitorInfos[i];
        if (info.hMonitor == hMonitor) {
            screenRegion = RectToRectangle(info.monitorInfo.rcMonitor);
            return true;
        }
    }
    return false;
}

bool CMonitors::GetWindowRect(HWND hWnd, agora::rtc::Rectangle& regionRect)
{
    if (!hWnd)
        return false;

    HMONITOR hMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
    RECT rcWnd = { 0 };
    ::GetWindowRect(hWnd, &rcWnd);

    agora::rtc::Rectangle rcScreen;
    GetMonitorRectangle(hMonitor, rcScreen);
    
    regionRect = RectToRectangle(rcWnd);
    regionRect.x = rcWnd.left - rcScreen.x;
    regionRect.y = rcWnd.top - rcScreen.y;
    return true;
}

void CAgoraScreenCapture::OnBnClickedButtonStartShareScreen()
{
    m_screenShare = !m_screenShare;
    if (m_screenShare) {
        int sel = m_cmbScreenRegion.GetCurSel();
        agora::rtc::Rectangle regionRect = { 0,0,0,0 };
        ScreenCaptureParameters capParam;
		CString displayId = L"";
		m_cmbScreenRegion.GetWindowText(displayId);
		int id = _ttoi(displayId.Mid(strlen("display"))) - 1;
		m_rtcEngine->startScreenCaptureByDisplayId(id, regionRect, capParam);
       
		m_lstInfo.InsertString(m_lstInfo.GetCount() - 1, _T("startScreenCaptureByDisplayId"));
		CString strInfo;
		strInfo.Format(_T("DisplayId: %d"), id);
		m_lstInfo.InsertString(m_lstInfo.GetCount() - 1, strInfo);
		m_btnShareScreen.SetWindowText(screenShareCtrlStopShare);
		//start preview in the engine.
		m_rtcEngine->startPreview();
		m_lstInfo.InsertString(m_lstInfo.GetCount() - 1, _T("startPreview "));
		if (m_joinChannel) {
			ChannelMediaOptions option;
			option.channelProfile = CHANNEL_PROFILE_LIVE_BROADCASTING;
			option.clientRoleType = CLIENT_ROLE_BROADCASTER;
			option.autoSubscribeAudio = true; option.autoSubscribeVideo = true;
			option.publishAudioTrack = true;
			option.publishScreenTrack = true;
			option.publishCameraTrack = false;
			m_rtcEngine->updateChannelMediaOptions(option);
			m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("updateChannelMediaOptions"));
		}
        m_btnStartCap.EnableWindow(FALSE);
		m_screenShare = true;
    }
    else {
        m_rtcEngine->stopScreenCapture();
		m_btnShareScreen.SetWindowText(screenShareCtrlShareSCreen);
        m_btnStartCap.EnableWindow(TRUE);
		m_screenShare = false;
    }
}

