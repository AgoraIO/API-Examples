#include "stdafx.h"
#include "APIExample.h"
#include "CAgoraMutilVideoSourceDlg.h"
#include <dwmapi.h>



IMPLEMENT_DYNAMIC(CAgoraMutilVideoSourceDlg, CDialogEx)

CAgoraMutilVideoSourceDlg::CAgoraMutilVideoSourceDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_MUTI_SOURCE, pParent)
{

}

CAgoraMutilVideoSourceDlg::~CAgoraMutilVideoSourceDlg()
{
}

void CAgoraMutilVideoSourceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_VIDEO, m_staVideoArea);
	DDX_Control(pDX, IDC_LIST_INFO_BROADCASTING, m_lstInfo);
	DDX_Control(pDX, IDC_STATIC_CHANNELNAME, m_staChannel);
	DDX_Control(pDX, IDC_EDIT_CHANNELNAME, m_edtChannel);
	DDX_Control(pDX, IDC_BUTTON_JOINCHANNEL, m_btnJoinChannel);
	DDX_Control(pDX, IDC_BUTTON_PUBLISH, m_btnPublish);
	DDX_Control(pDX, IDC_STATIC_DETAIL, m_staDetail);
	DDX_Control(pDX, IDC_COMBO_SCREEN_SHARE, m_cmbShare);
}


BEGIN_MESSAGE_MAP(CAgoraMutilVideoSourceDlg, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_MESSAGE(WM_MSGID(EID_JOINCHANNEL_SUCCESS), &CAgoraMutilVideoSourceDlg::OnEIDJoinChannelSuccess)
	ON_MESSAGE(WM_MSGID(EID_LEAVE_CHANNEL), &CAgoraMutilVideoSourceDlg::OnEIDLeaveChannel)
	ON_MESSAGE(WM_MSGID(EID_USER_JOINED), &CAgoraMutilVideoSourceDlg::OnEIDUserJoined)
	ON_MESSAGE(WM_MSGID(EID_USER_OFFLINE), &CAgoraMutilVideoSourceDlg::OnEIDUserOffline)
	ON_MESSAGE(WM_MSGID(EID_REMOTE_VIDEO_STATE_CHANED), &CAgoraMutilVideoSourceDlg::OnEIDRemoteVideoStateChanged)
	ON_BN_CLICKED(IDC_BUTTON_JOINCHANNEL, &CAgoraMutilVideoSourceDlg::OnBnClickedButtonJoinchannel)

	ON_BN_CLICKED(IDC_BUTTON_PUBLISH, &CAgoraMutilVideoSourceDlg::OnBnClickedButtonStartShare)
END_MESSAGE_MAP()


//Initialize the Ctrl Text.
void CAgoraMutilVideoSourceDlg::InitCtrlText()
{

	m_btnPublish.SetWindowText(MultiVideoSourceCtrlPublish);//MultiVideoSourceCtrlUnPublish
	m_staChannel.SetWindowText(commonCtrlChannel);
	m_btnJoinChannel.SetWindowText(commonCtrlJoinChannel);
}


//Initialize the Agora SDK
bool CAgoraMutilVideoSourceDlg::InitAgora()
{
	//create Agora RTC engine
	m_rtcEngine = createAgoraRtcEngine();
	if (!m_rtcEngine) {
		m_lstInfo.InsertString(m_lstInfo.GetCount() - 1, _T("createAgoraRtcEngine failed"));
		return false;
	}
	
	//set message notify receiver window
	screenVidoeSourceEventHandler.SetMsgReceiver(m_hWnd);
	screenVidoeSourceEventHandler.SetChannelId(0);
	
	agora::rtc::RtcEngineContext context;
	std::string strAppID = GET_APP_ID;
	context.appId = strAppID.c_str();
	context.eventHandler = &screenVidoeSourceEventHandler;
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
	//set channel profile in the engine to the CHANNEL_PROFILE_LIVE_BROADCASTING.
	m_rtcEngine->setChannelProfile(CHANNEL_PROFILE_LIVE_BROADCASTING);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("live broadcasting"));
	//set client role in the engine to the CLIENT_ROLE_BROADCASTER.
	m_rtcEngine->setClientRole(agora::rtc::CLIENT_ROLE_BROADCASTER);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("setClientRole broadcaster"));


	return true;
}


//UnInitialize the Agora SDK
void CAgoraMutilVideoSourceDlg::UnInitAgora()
{
	if (m_rtcEngine) {
		if (m_joinChannel) {
			//leave channel
			m_joinChannel = !m_rtcEngine->leaveChannel();
			
		}

		if (m_bPublishScreen) {
			m_bPublishScreen = false;
			StopShare();
			Sleep(100);
			m_btnPublish.SetWindowText(MultiVideoSourceCtrlPublish);

		}

		StopMultiVideoSource();

		m_bPublishScreen = false;
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
void CAgoraMutilVideoSourceDlg::RenderLocalVideo()
{
	if (m_rtcEngine) {
		agora::rtc::VideoCanvas canvas;
		canvas.renderMode = agora::rtc::RENDER_MODE_FIT;
		canvas.uid = 0;
		canvas.view = m_videoWnds[0].GetSafeHwnd();
		//setup local video in the engine to canvas.
		m_rtcEngine->setupLocalVideo(canvas);
		
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("setupLocalVideo"));
	}
}


//resume window status
void CAgoraMutilVideoSourceDlg::ResumeStatus()
{

	InitCtrlText();
	m_joinChannel = false;
	m_initialize = false;
	m_bPublishScreen = false;
	m_btnJoinChannel.EnableWindow(TRUE);
}


void CAgoraMutilVideoSourceDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);
	if (bShow) {
		//init control text.
		InitCtrlText();
		//update window.
		RenderLocalVideo();
		ReFreshWnd();
	}
	else {
		//resume window status.
		ResumeStatus();
	}
}


BOOL CAgoraMutilVideoSourceDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	RECT rcArea;
	m_staVideoArea.GetClientRect(&rcArea);
	RECT leftArea = rcArea;
	leftArea.right = (rcArea.right - rcArea.left) ;
	
	for (int i = 0; i < this->VIDOE_COUNT; ++i) {
		m_videoWnds[i].Create(NULL, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CRect(0, 0, 1, 1), &m_staVideoArea, i);
		//set window background color.
		m_videoWnds[i].SetFaceColor(RGB(0x58, 0x58, 0x58));
	}
	m_videoWnds[0].MoveWindow(&leftArea);

	//camera screen
	ResumeStatus();
	m_videoWnds[0].ShowWindow(SW_SHOW);
	return TRUE;
}


BOOL CAgoraMutilVideoSourceDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) {
		return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}

void CAgoraMutilVideoSourceDlg::OnBnClickedButtonJoinchannel()
{
	if (!m_rtcEngine || !m_initialize)
		return;
	CString strInfo;
	CString strChannelName;
	m_edtChannel.GetWindowText(strChannelName);
	std::string szChannelId = cs2utf8(strChannelName);
	if (!m_joinChannel) {
		if (strChannelName.IsEmpty()) {
			AfxMessageBox(_T("Fill channel name first"));
			return;
		}
		//camera
		m_rtcEngine->startPreview();
		m_strChannel = szChannelId;
		//join channel in the engine.
		if (0 == m_rtcEngine->joinChannel(APP_TOKEN, szChannelId.data(), NULL, 0)) {
			//strInfo.Format(_T("join channel %s"), strChannelName);
			m_btnJoinChannel.EnableWindow(FALSE);
		}
		m_strChannel = szChannelId;
		
	}
	else {
		m_rtcEngine->leaveChannel();

		m_strChannel = "";
	}
}


void CAgoraMutilVideoSourceDlg::OnBnClickedButtonStartShare()
{
	if (!m_bPublishScreen) {
		if (!m_joinChannel) {
			AfxMessageBox(_T("join channel first"));
			return;
		}
		m_btnPublish.SetWindowText(MultiVideoSourceCtrlUnPublish);
		StartShare();
	}
	else {
		StopShare();
		m_btnPublish.SetWindowText(MultiVideoSourceCtrlPublish);
	}	
	m_bPublishScreen = !m_bPublishScreen;
}


BOOL CALLBACK EnumWindowsCallback(HWND handle, LPARAM lParam)
{
	HANDLE_DATA& data = *(HANDLE_DATA*)lParam;
	unsigned long process_id = 0;
	GetWindowThreadProcessId(handle, &process_id);
	char szbuf[MAX_PATH] = { '\0' };
	OutputDebugStringA(szbuf);
	if (data.process_id == process_id) {
		sprintf_s(szbuf, "!!!!!!!!!!!!!!!handle :%x, processId: %u\n", handle, process_id);
		OutputDebugStringA(szbuf);
		data.best_handle = handle;
		return FALSE;
	}

	return TRUE;
}


int CAgoraMutilVideoSourceDlg::StartMultiVideoSource()
{
	//ScreenShare
	int nNum = 0;
	int dwProcessId = 0;
	dwProcessId = getProcessID("ProcessScreenShare.exe");
	if (0 >= dwProcessId)
		dwProcessId = openProcess("ProcessScreenShare.exe", m_strChannel + " " + GET_APP_ID);
	m_lstInfo.InsertString(m_lstInfo.GetCount() - 1, _T("start porcess success"));

	m_HandleData.process_id = (unsigned long)dwProcessId;
	do {
		EnumWindows(EnumWindowsCallback, (LPARAM)(&m_HandleData));
	} while (!m_HandleData.best_handle);
	m_WndScreenShare = m_HandleData.best_handle;

	if (!IsWindow(m_WndScreenShare))
		return -1;

	AGE_SCREENSHARE_BASEINFO baseInfoTemp;
	if (TRUE) {
		baseInfoTemp.channelname = m_strChannel;
		baseInfoTemp.uSubuID = m_uid + 1;
		baseInfoTemp.uMainuID = m_uid;
		baseInfoTemp.appid = GET_APP_ID;
		baseInfoTemp.processHandle = GetCurrentProcess();
		m_rtcEngine->muteRemoteVideoStream(baseInfoTemp.uSubuID, true);
		m_rtcEngine->muteRemoteAudioStream(baseInfoTemp.uSubuID, true);

		COPYDATASTRUCT cd;
		cd.dwData = ShareType_BaseInfo;
		cd.cbData = sizeof(baseInfoTemp);
		cd.lpData = (PVOID)&baseInfoTemp;
		::SendMessage(m_WndScreenShare, WM_COPYDATA, WPARAM(m_WndScreenShare), LPARAM(&cd));
	}
	m_lstInfo.InsertString(m_lstInfo.GetCount() - 1, _T("send share info to multi VideoSource"));
	return 0;
}

//EID_JOINCHANNEL_SUCCESS message window handler.
LRESULT CAgoraMutilVideoSourceDlg::OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam)
{
	m_uid = wParam;
	int cId = (int)lParam;
	CString strChannelName = utf82cs(m_strChannel);
	m_joinChannel = true;
	m_btnJoinChannel.EnableWindow(TRUE);
	m_btnJoinChannel.SetWindowText(commonCtrlLeaveChannel);
	CString strInfo;
	strInfo.Format(_T("join %s success,cid=%u, uid=%u"), strChannelName, cId, wParam);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	m_videoWnds[0].SetUID(wParam);
	StartMultiVideoSource();
	
	return 0;
}

void CAgoraMutilVideoSourceDlg::StopMultiVideoSource()
{
	COPYDATASTRUCT pCopyData;
	if (TRUE) {
		pCopyData.dwData = ShareType_Close;
		pCopyData.lpData = NULL;
		pCopyData.cbData = 0;
	}
	::SendMessage(m_WndScreenShare, WM_MSGID(EID_SCREENSHARE_CLOSE), NULL, NULL);
	m_WndScreenShare = nullptr;
	m_HandleData.best_handle = nullptr;
	if (m_HandleData.process_id > 0) {
		closeProcess(m_HandleData.process_id);
		m_HandleData.process_id = 0;
	}
}

void CAgoraMutilVideoSourceDlg::StartShare()
{
	HWND hMarkWnd = NULL;

	if (m_cmbShare.GetCurSel() > 0) {
		hMarkWnd = m_listWnd.GetAt(m_listWnd.FindIndex(m_cmbShare.GetCurSel() + 1));
	}

	if (!hMarkWnd || ::IsWindow(hMarkWnd)) {

		AGE_SCREENSHARE_START StartTemp;
		StartTemp.hWnd = hMarkWnd;
		PCOPYDATASTRUCT pCopyData = new COPYDATASTRUCT;
		pCopyData->dwData = ShareType_Start;
		pCopyData->lpData = (PVOID)&StartTemp;
		pCopyData->cbData = sizeof(StartTemp);
		int ret = ::SendMessage(m_WndScreenShare, WM_COPYDATA, WPARAM(m_hWnd), LPARAM(pCopyData));
	}
}
void CAgoraMutilVideoSourceDlg::StopShare()
{
	AGE_SCREENSHARE_START lpData;
	lpData.hWnd = m_WndScreenShare;
	PCOPYDATASTRUCT pCopyData = new COPYDATASTRUCT;
	if (TRUE) {

		pCopyData->dwData = ShareType_Stop;
		pCopyData->lpData = (PVOID)&lpData;
		pCopyData->cbData = sizeof(lpData);
		::SendMessage(m_WndScreenShare, WM_COPYDATA, WPARAM(m_hWnd), LPARAM(pCopyData));
	}
}

//EID_LEAVE_CHANNEL message window handler.
LRESULT CAgoraMutilVideoSourceDlg::OnEIDLeaveChannel(WPARAM wParam, LPARAM lParam)
{
	if (!m_joinChannel)
		return 0;
	
	CString strChannelName = utf82cs(m_strChannel);
	m_btnJoinChannel.SetWindowText(commonCtrlJoinChannel);
	CString strInfo;
	strInfo.Format(_T("leave channel:%s "), strChannelName);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

	if (m_bPublishScreen) {
		StopShare();
		m_btnPublish.SetWindowText(MultiVideoSourceCtrlPublish);
		m_bPublishScreen = false;
	}
	StopMultiVideoSource();

	m_joinChannel = false;
	
	return 0;
}

//EID_USER_JOINED message window handler.
LRESULT CAgoraMutilVideoSourceDlg::OnEIDUserJoined(WPARAM wParam, LPARAM lParam)
{
	int cId = (int)lParam;
	
	CString strChannelName = utf82cs(m_strChannel);
	CString strInfo;
	strInfo.Format(_T("%u joined %s"), wParam, strChannelName);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	return 0;
}


//EID_USER_OFFLINE message window handler.
LRESULT CAgoraMutilVideoSourceDlg::OnEIDUserOffline(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

//EID_REMOTE_VIDEO_STATE_CHANED message window handler.
LRESULT CAgoraMutilVideoSourceDlg::OnEIDRemoteVideoStateChanged(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

/*
	enum window callback function.
*/
BOOL CALLBACK CAgoraMutilVideoSourceDlg::WndEnumProc(HWND hWnd, LPARAM lParam)
{
	CList<HWND>* lpListctrl = (CList<HWND>*)lParam;
	TCHAR strName[255];
	::GetWindowText(hWnd, strName, 255);
	CString str = strName;
	LONG lStyle = ::GetWindowLong(hWnd, GWL_STYLE);

	BOOL isCloaked = FALSE;
	isCloaked = (SUCCEEDED(DwmGetWindowAttribute(hWnd, DWMWA_CLOAKED, &isCloaked, sizeof(isCloaked))) && isCloaked);
	if ((lStyle & WS_VISIBLE) != 0
		&& (lStyle & (WS_POPUP | WS_SYSMENU)) != 0
		&& ::IsWindowVisible(hWnd)
		&& !isCloaked
		&& !str.IsEmpty()
		&& str.Compare(_T("Program Manager"))
		//&&::IsZoomed(hWnd)
		)
		lpListctrl->AddTail(hWnd);

	return TRUE;
}


// call RefreashWndInfo to refresh window list and to m_cmbScreenCap.
void CAgoraMutilVideoSourceDlg::ReFreshWnd()
{
	//refresh window info.
	RefreashWndInfo();
	POSITION	pos = m_listWnd.GetHeadPosition();
	HWND		hWnd = NULL;
	TCHAR		strName[255];
	int index = 0;
	//enumerate hwnd to add m_cmbScreenCap.
	m_cmbShare.InsertString(index++, _T("Desktop"));
	while (pos != NULL) {
		hWnd = m_listWnd.GetNext(pos);
		::GetWindowText(hWnd, strName, 255);
		m_cmbShare.InsertString(index++, strName);
	
	}
	//m_cmbScreenCap.InsertString(index++, L"DeskTop");

	m_cmbShare.SetCurSel(0);
	
}

int	 CAgoraMutilVideoSourceDlg::RefreashWndInfo()
{
	m_listWnd.RemoveAll();
	::EnumWindows(&CAgoraMutilVideoSourceDlg::WndEnumProc, (LPARAM)&m_listWnd);
	return static_cast<int>(m_listWnd.GetCount());
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
void CScreenShareEventHandler::onJoinChannelSuccess(const char* channel, agora::rtc::uid_t uid, int elapsed)
{
	
	if (m_hMsgHanlder) {
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_JOINCHANNEL_SUCCESS), (WPARAM)uid, (LPARAM)m_channelId);
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
void CScreenShareEventHandler::onUserJoined(agora::rtc::uid_t uid, int elapsed)
{
	if (m_hMsgHanlder) {
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_USER_JOINED), (WPARAM)uid, (LPARAM)m_channelId);
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
void CScreenShareEventHandler::onUserOffline(agora::rtc::uid_t uid, agora::rtc::USER_OFFLINE_REASON_TYPE reason)
{
	if (m_hMsgHanlder) {
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_USER_OFFLINE), (WPARAM)uid, (LPARAM)m_channelId);
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
void CScreenShareEventHandler::onLeaveChannel(const agora::rtc::RtcStats& stats)
{
	if (m_hMsgHanlder) {
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_LEAVE_CHANNEL), (WPARAM)m_channelId, 0);
	}
}

void CScreenShareEventHandler::onRemoteVideoStateChanged(agora::rtc::uid_t uid, agora::rtc::REMOTE_VIDEO_STATE state, agora::rtc::REMOTE_VIDEO_STATE_REASON reason, int elapsed)
{
	if (m_hMsgHanlder) {
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_REMOTE_VIDEO_STATE_CHANED), (WPARAM)uid, (LPARAM)m_channelId);
	}
}

