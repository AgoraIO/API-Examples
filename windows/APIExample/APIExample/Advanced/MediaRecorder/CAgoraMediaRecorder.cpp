#include "stdafx.h"
#include "APIExample.h"
#include "CAgoraMediaRecorder.h"
#include "afxdialogex.h"

IMPLEMENT_DYNAMIC(CAgoraMediaRecorder, CDialogEx)

CAgoraMediaRecorder::CAgoraMediaRecorder(CWnd* pParent /*= nullptr*/)
	:CDialogEx(IDD, pParent)
{

}

CAgoraMediaRecorder::~CAgoraMediaRecorder()
{

}


BOOL CAgoraMediaRecorder::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) {
		return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}


BOOL CAgoraMediaRecorder::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_localVideoWnd.Create(NULL, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CRect(0, 0, 1, 1), this, ID_BASEWND_VIDEO + 100);
	m_remoteVideoWnd.Create(NULL, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CRect(0, 0, 1, 1), this, ID_BASEWND_VIDEO + 110);

	RECT rcArea;
	m_staVideoArea.GetClientRect(&rcArea);
	RECT rcLeft = rcArea, rcRight = rcArea;
	rcLeft.right = rcLeft.left + (rcArea.right - rcArea.left) / 2;
	rcRight.left = rcLeft.right + 1;

	m_localVideoWnd.MoveWindow(&rcLeft);
	m_remoteVideoWnd.MoveWindow(&rcRight);
	m_localVideoWnd.ShowWindow(SW_SHOW);
	m_remoteVideoWnd.ShowWindow(SW_SHOW);

	ResumeStatus();
	return TRUE;
}

void CAgoraMediaRecorder::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_CHANNELNAME, m_staChannelName);
	DDX_Control(pDX, IDC_BUTTON_JOINCHANNEL, m_btnJoinChannel);
	DDX_Control(pDX, IDC_LIST_INFO_BROADCASTING, m_lstInfo);
	DDX_Control(pDX, IDC_STATIC_VIDEO, m_staVideoArea);
	DDX_Control(pDX, IDC_EDIT_CHANNELNAME, m_edtChannelName);
	DDX_Control(pDX, IDC_BUTTON_LOCALRECORDER, m_btnLocalRecord);
	DDX_Control(pDX, IDC_BUTTON_REMOTERECORDER, m_btnRemoteRecord);
}

BEGIN_MESSAGE_MAP(CAgoraMediaRecorder, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_JOINCHANNEL, &CAgoraMediaRecorder::OnBnClickedButtonJoinchannel)
	ON_BN_CLICKED(IDC_BUTTON_LOCALRECORDER, &CAgoraMediaRecorder::OnBnClickedButtonLocalRecorder)
	ON_BN_CLICKED(IDC_BUTTON_REMOTERECORDER, &CAgoraMediaRecorder::OnBnClickedButtonRemoteRecorder)
	ON_MESSAGE(WM_MSGID(EID_JOINCHANNEL_SUCCESS), &CAgoraMediaRecorder::OnEIDJoinChannelSuccess)
	ON_MESSAGE(WM_MSGID(EID_LEAVE_CHANNEL), &CAgoraMediaRecorder::OnEIDLeaveChannel)
	ON_MESSAGE(WM_MSGID(EID_USER_JOINED), &CAgoraMediaRecorder::OnEIDUserJoined)
	ON_MESSAGE(WM_MSGID(EID_USER_OFFLINE), &CAgoraMediaRecorder::OnEIDUserOffline)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

bool CAgoraMediaRecorder::InitAgora()
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
	//enable video in the engine.
	m_rtcEngine->enableVideo();
	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("enable video"));

	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("live broadcasting"));
	//set client role in the engine to the CLIENT_ROLE_BROADCASTER.
	m_rtcEngine->setClientRole(CLIENT_ROLE_BROADCASTER);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("setClientRole broadcaster"));
	
	m_btnJoinChannel.EnableWindow(TRUE);
	return true;
}

void CAgoraMediaRecorder::UnInitAgora()
{
	if (m_rtcEngine) {
		if (m_joinChannel)
			m_joinChannel = !m_rtcEngine->leaveChannel();
		//stop preview in the window.
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

void CAgoraMediaRecorder::InitCtrlText()
{
	m_staChannelName.SetWindowText(commonCtrlChannel);
	m_btnJoinChannel.SetWindowText(commonCtrlJoinChannel);
	m_btnLocalRecord.SetWindowText(mediaRecorderStartRecording);
	m_btnRemoteRecord.SetWindowText(mediaRecorderStartRecording);
}


void CAgoraMediaRecorder::ResumeStatus()
{
	InitCtrlText();
	m_lstInfo.ResetContent();
	m_edtChannelName.SetWindowText(_T(""));
	m_btnLocalRecord.ShowWindow(FALSE);
	m_btnRemoteRecord.ShowWindow(FALSE);
	m_joinChannel = false;
	m_initialize = false;
	m_remoteJoined = false;
}

void CAgoraMediaRecorder::RenderLocalVideo()
{
	if (m_rtcEngine) {
		//start preview in the engine.
		m_rtcEngine->startPreview();
		VideoCanvas canvas;
		canvas.renderMode = media::base::RENDER_MODE_FIT;
		canvas.uid = 0;
		canvas.view = m_localVideoWnd.GetSafeHwnd();
		//set up local video in the engine to canvas.
		m_rtcEngine->setupLocalVideo(canvas);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("render local video"));
		m_btnLocalRecord.ShowWindow(TRUE);
	}
}

LRESULT CAgoraMediaRecorder::OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam)
{
	m_btnJoinChannel.EnableWindow(TRUE);
	m_joinChannel = true;
	m_uid = wParam;
	m_btnJoinChannel.SetWindowText(commonCtrlLeaveChannel);
	CString strInfo;
	strInfo.Format(_T("%s:join success, uid=%u"), getCurrentTime(), wParam);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	m_localVideoWnd.SetUID(wParam);
	//notify parent window
	::PostMessage(GetParent()->GetSafeHwnd(), WM_MSGID(EID_JOINCHANNEL_SUCCESS), TRUE, 0);
	return 0;
}

LRESULT CAgoraMediaRecorder::OnEIDLeaveChannel(WPARAM wParam, LPARAM lParam)
{
	m_btnJoinChannel.EnableWindow(TRUE);
	m_joinChannel = false;
	m_btnJoinChannel.SetWindowText(commonCtrlJoinChannel);

	m_remoteVideoWnd.SetUID(0);
	m_remoteVideoWnd.Invalidate();

	CString strInfo;
	strInfo.Format(_T("leave channel success %s"), getCurrentTime());
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

	//notify parent window
	::PostMessage(GetParent()->GetSafeHwnd(), WM_MSGID(EID_JOINCHANNEL_SUCCESS), FALSE, 0);
	return 0;
}

LRESULT CAgoraMediaRecorder::OnEIDUserJoined(WPARAM wParam, LPARAM lParam)
{
	CString strInfo;
	strInfo.Format(_T("%u joined"), wParam);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

	if (m_remoteVideoWnd.GetUID() == 0) {
		VideoCanvas canvas;
		canvas.uid = wParam;
		canvas.view = m_remoteVideoWnd.GetSafeHwnd();
		canvas.renderMode = media::base::RENDER_MODE_FIT;
		//setup remote video in engine to the canvas.
		m_rtcEngine->setupRemoteVideo(canvas);
		m_remoteVideoWnd.SetUID(wParam);
	}
	return 0;
}

LRESULT CAgoraMediaRecorder::OnEIDUserOffline(WPARAM wParam, LPARAM lParam)
{
	uid_t remoteUid = (uid_t)wParam;

	CString strInfo;
	strInfo.Format(_T("%u offline, reason:%d"), remoteUid, lParam);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

	VideoCanvas canvas;
	canvas.uid = remoteUid;
	canvas.view = NULL;
	m_rtcEngine->setupRemoteVideo(canvas);

	if (m_remoteVideoWnd.GetUID() == remoteUid) {
		m_remoteVideoWnd.SetUID(0);
		m_remoteVideoWnd.Invalidate();
	}

	return 0;
}

void CAgoraMediaRecorder::OnBnClickedButtonJoinchannel()
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
		//join channel in the engine.
		if (0 == m_rtcEngine->joinChannel(APP_TOKEN, szChannelId.c_str(), 0, options)) {
			strInfo.Format(_T("join channel %s, use ChannelMediaOptions"), getCurrentTime());
			m_btnJoinChannel.EnableWindow(FALSE);
			m_channelName = szChannelId;
		}
	}
	else {
		//leave channel in the engine.
		if (0 == m_rtcEngine->leaveChannel()) {
			strInfo.Format(_T("leave channel %s"), getCurrentTime());
			m_btnJoinChannel.EnableWindow(FALSE);
		}
	}

	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
}


void CAgoraMediaRecorder::OnBnClickedButtonLocalRecorder()
{
	if (m_rtcEngine && m_joinChannel) {
		RtcConnection conn;
		conn.channelId = m_channelName.c_str();
		conn.localUid = m_uid;
		m_localRecorder = m_rtcEngine->createLocalMediaRecorder(conn).get();
		MediaRecorderConfiguration config;
		config.storagePath = "media_recording_local.mp4";
		m_localRecorder->setMediaRecorderObserver(&m_localRecorderObserver);
		int ret = m_localRecorder->startRecording(config);
		CString strInfo;
		strInfo.Format(_T("startRecording : ret=%d"), ret);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	}
}


void CAgoraMediaRecorder::OnBnClickedButtonRemoteRecorder()
{

}

void CAgoraMediaRecorder::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	if (bShow) {
		RenderLocalVideo();
	}
	else {
		ResumeStatus();
	}
}


