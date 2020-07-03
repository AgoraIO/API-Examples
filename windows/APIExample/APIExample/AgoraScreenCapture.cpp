#include "stdafx.h"
#include "APIExample.h"
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

void CAgoraScreenCapture::InitCtrlText()
{
	m_staScreenCap.SetWindowText(screenShareCtrlScreenCap);
	m_btnStartCap.SetWindowText(screenShareCtrlStartCap);
	m_staChannel.SetWindowText(commonCtrlChannel);
	m_btnJoinChannel.SetWindowText(commonCtrlJoinChannel);
}

/*
	create Agora Rtc Engine and initialize context.set channel proprety.
*/
bool CAgoraScreenCapture::InitAgora()
{
	m_rtcEngine = createAgoraRtcEngine();
	if (!m_rtcEngine) {
		m_lstInfo.InsertString(m_lstInfo.GetCount() - 1, _T("createAgoraRtcEngine failed"));
		return false;
	}
	m_eventHandler.SetMsgReceiver(m_hWnd);

	RtcEngineContext context;
	context.appId = APP_ID;
	context.eventHandler = &m_eventHandler;
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
	m_rtcEngine->enableVideo();
	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("enable video"));

	m_rtcEngine->setChannelProfile(CHANNEL_PROFILE_LIVE_BROADCASTING);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("live broadcasting"));
	m_rtcEngine->setClientRole(CLIENT_ROLE_BROADCASTER);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("setClientRole broadcaster"));

	m_btnJoinChannel.EnableWindow(TRUE);
	return true;
}

/*
	stop and release agora rtc engine.
*/
void CAgoraScreenCapture::UnInitAgora()
{
	if (m_rtcEngine) {
		m_rtcEngine->stopPreview();
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("stopPreview"));
		m_rtcEngine->disableVideo();
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("disableVideo"));
		m_rtcEngine->release(true);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("release rtc engine"));
		m_rtcEngine = NULL;
	}
}

/*
	set up canvas and local video view.
*/
void CAgoraScreenCapture::RenderLocalVideo()
{
	if (m_rtcEngine) {
		m_rtcEngine->startPreview();
		VideoCanvas canvas;
		canvas.renderMode = RENDER_MODE_FIT;
		canvas.uid = 0;
		canvas.view = m_localVideoWnd.GetSafeHwnd();
		m_rtcEngine->setupLocalVideo(canvas);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("render local video"));
	}
}

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

LRESULT CAgoraScreenCapture::OnEIDUserJoined(WPARAM wParam, LPARAM lParam)
{
	CString strInfo;
	strInfo.Format(_T("%u joined"), wParam);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

	return 0;
}

LRESULT CAgoraScreenCapture::OnEIDUserOffline(WPARAM wParam, LPARAM lParam)
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
		ret = m_rtcEngine->startScreenCapture(hWnd, 15, NULL, TRUE);
		if (ret == 0)
			m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("screen share start succees！"));
		else
			m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("screen share start failed！"));

		m_btnStartCap.SetWindowText(screenShareCtrlEndCap);
	}
	else {
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
	RefreashWndInfo();
	POSITION	pos = m_listWnd.GetHeadPosition();
	HWND		hWnd = NULL;
	TCHAR		strName[255];
	int index = 0;
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






void CScreenCaputreEventHandler::onJoinChannelSuccess(const char* channel, uid_t uid, int elapsed)
{
	if (m_hMsgHanlder) {
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_JOINCHANNEL_SUCCESS), (WPARAM)uid, (LPARAM)elapsed);
	}
}

void CScreenCaputreEventHandler::onUserJoined(uid_t uid, int elapsed)
{
	if (m_hMsgHanlder) {
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_USER_JOINED), (WPARAM)uid, (LPARAM)elapsed);
	}
}

void CScreenCaputreEventHandler::onUserOffline(uid_t uid, USER_OFFLINE_REASON_TYPE reason)
{
	if (m_hMsgHanlder) {
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_USER_OFFLINE), (WPARAM)uid, (LPARAM)reason);
	}
}

void CScreenCaputreEventHandler::onLeaveChannel(const RtcStats& stats)
{
	if (m_hMsgHanlder) {
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_LEAVE_CHANNEL), 0, 0);
	}
}

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

