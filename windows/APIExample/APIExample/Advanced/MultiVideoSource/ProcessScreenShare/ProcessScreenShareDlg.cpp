
// ProcessScreenShareDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ProcessScreenShare.h"
#include "ProcessScreenShareDlg.h"
#include "afxdialogex.h"
#include "../commonFun.h"
#include <iostream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CProcessScreenShareDlg dialog



CProcessScreenShareDlg::CProcessScreenShareDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CProcessScreenShareDlg::IDD, pParent)
	, m_lpRtcEngine(nullptr)
	, m_hScreenShareWnd(nullptr)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CProcessScreenShareDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CProcessScreenShareDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SHOWWINDOW()
	ON_WM_CLOSE()
	ON_WM_COPYDATA()
	ON_MESSAGE(EID_SCREENSHARE_BASEINFO, OnScreenShareBaseInfo)
	ON_MESSAGE(EID_SCREENSHARE_START, OnScreenShareStart)
	ON_MESSAGE(EID_SCREENSHARE_STOP, OnScreenShareStop)
	ON_MESSAGE(EID_SCREENSHARE_CLOSE,OnScreenShareClose)
	ON_MESSAGE(EID_JOINCHANNEL_SUCCESS, &CProcessScreenShareDlg::OnEIDJoinChannelSuccess)
	ON_MESSAGE(EID_PARENT_PROCESS_EXIT, &CProcessScreenShareDlg::OnEIDParentExit)

	
END_MESSAGE_MAP()


// CProcessScreenShareDlg message handlers

BOOL CProcessScreenShareDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	CString strCmdLine = GetCommandLine();

	int pos1 = strCmdLine.Find(_T(" -"));
	CString strAppid = _T(""), channelName = _T("");
	if (pos1 > 0) {
		strCmdLine = strCmdLine.Mid(pos1 + 2);
		int pos2 = strCmdLine.ReverseFind(_T(' '));
		
		channelName = strCmdLine.Mid(0, pos2);
		strAppid = strCmdLine.Mid(pos2 + 1);

		m_strAppID = cs2s(strAppid);
		m_strChannelName = cs2s(channelName);
	}
	
	UINT threadId = 0;
	m_hMonitorThread = (HANDLE)_beginthreadex(NULL, 0, ThreadFunc, (LPVOID)this, 0, &threadId);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

UINT _stdcall CProcessScreenShareDlg::ThreadFunc(LPVOID lpVoid)
{
	CProcessScreenShareDlg* pThis = (CProcessScreenShareDlg*)lpVoid;
	while (1) {
		int dwProcessId = getProcessID("APIExample.exe");
		if (dwProcessId < 0) {
			pThis->PostMessage(EID_PARENT_PROCESS_EXIT);
		}
		Sleep(1000);
	}
	return 0;
}

void CProcessScreenShareDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CProcessScreenShareDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CProcessScreenShareDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CProcessScreenShareDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
// 	ShowWindow(SW_MINIMIZE);
// 	ShowWindow(SW_HIDE);
}

void CProcessScreenShareDlg::OnClose()
{
	if (m_lpRtcEngine) {
		m_lpRtcEngine->leaveChannel();
		uninitAgoraMedia();
	}

	CDialogEx::OnCancel();
}

LRESULT CProcessScreenShareDlg::OnScreenShareBaseInfo(WPARAM wParam, LPARAM lParam)
{
	//InitRtcEngine
	LPAGE_SCREENSHARE_BASEINFO lpData = (LPAGE_SCREENSHARE_BASEINFO)wParam;
	if (lpData) {
		//m_strChannelName = lpData->channelname;
		m_uId = lpData->uSubuID;
		m_hProcess = lpData->processHandle;
	
		
		//m_strAppID = lpData->appid;
		initAgoraMedia();
	}

	return TRUE;
}


BOOL CProcessScreenShareDlg::EnableScreenCapture(HWND hWnd, int nCapFPS, LPCRECT lpCapRect, BOOL bEnable, int nBitrate)
{
	int ret = 0;
	agora::rtc::Rectangle rcCap;
	agora::rtc::ScreenCaptureParameters capParam;
	capParam.bitrate = nBitrate;
	capParam.frameRate = nCapFPS;
	
	if (bEnable) {
		if (m_bScreenCapture)
			return FALSE;
		if (lpCapRect == NULL) {
			RECT rc;
			
			if (hWnd) {
				::GetWindowRect(hWnd, &rc);
				capParam.dimensions.width = rc.right - rc.left;
				capParam.dimensions.height = rc.bottom - rc.top;
				rcCap = { rc.left, rc.top, rc.right, rc.bottom };
				ret = m_lpRtcEngine->startScreenCaptureByWindowId(hWnd, rcCap, capParam);
			}
			else {
				::GetWindowRect(::GetDesktopWindow(), &rc);
				agora::rtc::Rectangle screenRegion = { rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top };
				capParam.dimensions.width = rc.right - rc.left;
				capParam.dimensions.height = rc.bottom - rc.top;
				rcCap = { rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top };
				
				ret = m_lpRtcEngine->startScreenCaptureByScreenRect(screenRegion, rcCap, capParam);
			}
			//startScreenCapture(hWnd, nCapFPS, NULL, nBitrate);
		}
		else {
			capParam.dimensions.width = lpCapRect->right - lpCapRect->left;
			capParam.dimensions.height = lpCapRect->bottom - lpCapRect->top;

			rcCap.x = lpCapRect->left;
			rcCap.y = lpCapRect->top;
			rcCap.width = lpCapRect->right - lpCapRect->left;
			rcCap.height = lpCapRect->bottom - lpCapRect->top;

			if (hWnd)
				ret = m_lpRtcEngine->startScreenCaptureByWindowId(hWnd, rcCap, capParam);
			else {

				agora::rtc::Rectangle screenRegion = rcCap;
				ret = m_lpRtcEngine->startScreenCaptureByScreenRect(screenRegion, rcCap, capParam);
			}
		}
	}
	else {
		if (!m_bScreenCapture)
			return FALSE;
		ret = m_lpRtcEngine->stopScreenCapture();
	}

	if (ret == 0)
		m_bScreenCapture = bEnable;

	return ret == 0 ? TRUE : FALSE;
}

LRESULT CProcessScreenShareDlg::OnScreenShareStart(WPARAM wParam, LPARAM lParam)
{
	//joinChannel  startScreenShare
	LPAGE_SCREENSHARE_START lpData = (LPAGE_SCREENSHARE_START)wParam;
	int ret = 0;
	if (lpData) {

		m_hScreenShareWnd = lpData->hWnd;
		ret = m_lpRtcEngine->joinChannel(NULL, m_strChannelName.c_str(), NULL, m_uId);	
	}

	return TRUE;
}

LRESULT CProcessScreenShareDlg::OnScreenShareStop(WPARAM wParam, LPARAM lParam)
{
	//stopScreenShare
	m_hScreenShareWnd = nullptr;
	EnableScreenCapture(NULL, 0, NULL, FALSE, 0);
	m_lpRtcEngine->leaveChannel();

	return TRUE;
}

LRESULT CProcessScreenShareDlg::OnScreenShareClose(WPARAM wParam, LPARAM lParam)
{
	PostMessage(WM_COMMAND, IDCANCEL);

	return TRUE;
}

BOOL CProcessScreenShareDlg::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct)
{
	if (pCopyDataStruct && pCopyDataStruct->lpData){
		LRESULT ret = 0;
		SHARETYPE type = (SHARETYPE)pCopyDataStruct->dwData;
		switch (type){
		case SHARETYPE::ShareType_BaseInfo:
			ret = SendMessage(EID_SCREENSHARE_BASEINFO,(WPARAM)(pCopyDataStruct->lpData));
			break;
		case SHARETYPE::ShareType_Start:
			SendMessage(EID_SCREENSHARE_START, (WPARAM)(pCopyDataStruct->lpData));
			break;
		case SHARETYPE::ShareType_Stop:
			SendMessage(EID_SCREENSHARE_STOP, (WPARAM)(pCopyDataStruct->lpData));
			break;
		case SHARETYPE::ShareType_Close:
			SendMessage(EID_SCREENSHARE_CLOSE);
			break;
		default: break;
		}
	}

	return TRUE;
}

inline void CProcessScreenShareDlg::initAgoraMedia()
{
	m_lpRtcEngine = createAgoraRtcEngine();
	ASSERT(m_lpRtcEngine);

	agora::rtc::RtcEngineContext ctx;
	ctx.appId = m_strAppID.c_str();
	ctx.eventHandler = &m_EngineEventHandler;

	m_EngineEventHandler.SetMsgReceiver(m_hWnd);
	m_lpRtcEngine->initialize(ctx);

	m_lpRtcEngine->enableWebSdkInteroperability(TRUE);

	m_lpRtcEngine->enableVideo();
	m_lpRtcEngine->disableAudio();


	m_lpRtcEngine->setChannelProfile(agora::rtc::CHANNEL_PROFILE_LIVE_BROADCASTING);
	m_lpRtcEngine->setClientRole(agora::rtc::CLIENT_ROLE_BROADCASTER);
	m_lpRtcEngine->muteAllRemoteAudioStreams(true);
	m_lpRtcEngine->muteAllRemoteVideoStreams(true);
}

inline void CProcessScreenShareDlg::uninitAgoraMedia()
{
	if (nullptr == m_lpRtcEngine){
		return;
	}

	m_lpRtcEngine->disableVideo();
	if (m_lpRtcEngine != NULL)
		m_lpRtcEngine->release();
}



LRESULT CProcessScreenShareDlg::OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam)
{
	EnableScreenCapture(m_hScreenShareWnd, 15, NULL, TRUE, 0);
	return 0;
}

LRESULT CProcessScreenShareDlg::OnEIDParentExit(WPARAM wParam, LPARAM lParam)
{
	if(m_bScreenCapture)
	OnScreenShareStop(0, 0);
	OnScreenShareClose(0, 0);
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
	uid: user ID¡£If the UID is specified in the joinChannel, that ID is returned here;
	Otherwise, use the ID automatically assigned by the Agora server.
	elapsed: The Time from the joinChannel until this event occurred (ms).
*/
void CScreenShareEventHandler::onJoinChannelSuccess(const char* channel, agora::rtc::uid_t uid, int elapsed)
{
	m_strChannel = channel;
	if (m_hMsgHanlder) {
		::PostMessage(m_hMsgHanlder, EID_JOINCHANNEL_SUCCESS, (WPARAM)uid, (LPARAM)m_channelId);
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
		::PostMessage(m_hMsgHanlder, EID_USER_JOINED, (WPARAM)uid, (LPARAM)m_channelId);
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
		::PostMessage(m_hMsgHanlder, EID_USER_OFFLINE, (WPARAM)uid, (LPARAM)m_channelId);
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
		::PostMessage(m_hMsgHanlder, EID_LEAVE_CHANNEL, (WPARAM)m_channelId, 0);
	}
}

void CScreenShareEventHandler::onRemoteVideoStateChanged(agora::rtc::uid_t uid, agora::rtc::REMOTE_VIDEO_STATE state, agora::rtc::REMOTE_VIDEO_STATE_REASON reason, int elapsed)
{
	if (m_hMsgHanlder) {
		::PostMessage(m_hMsgHanlder, EID_REMOTE_VIDEO_STATE_CHANED, (WPARAM)uid, (LPARAM)m_channelId);
	}
}
