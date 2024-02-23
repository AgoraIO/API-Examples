#include "stdafx.h"
#include "APIExample.h"
#include "CAgoraMultiChannelDlg.h"




IMPLEMENT_DYNAMIC(CAgoraMultiChannelDlg, CDialogEx)

CAgoraMultiChannelDlg::CAgoraMultiChannelDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_MUTI_CHANNEL, pParent)
{

}

CAgoraMultiChannelDlg::~CAgoraMultiChannelDlg()
{
}


void CAgoraMultiChannelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_VIDEO, m_staVideoArea);
	DDX_Control(pDX, IDC_LIST_INFO_BROADCASTING, m_lstInfo);
	DDX_Control(pDX, IDC_STATIC_CHANNELNAME, m_staChannel);
	DDX_Control(pDX, IDC_EDIT_CHANNELNAME, m_edtChannel);
	DDX_Control(pDX, IDC_BUTTON_JOINCHANNEL, m_btnJoinChannel);
	DDX_Control(pDX, IDC_BUTTON_LEAVE_CHANNEL, m_btnExChannel);
	DDX_Control(pDX, IDC_BUTTON_SNAPSHOT_EX, m_btnExSnapshot);
	DDX_Control(pDX, IDC_BUTTON_STOP_MIC, m_chkStopMic);
	DDX_Control(pDX, IDC_EDIT_DETAIL_INFO, m_edtDetailInfo);
}


BEGIN_MESSAGE_MAP(CAgoraMultiChannelDlg, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_MESSAGE(WM_MSGID(EID_JOINCHANNEL_SUCCESS), &CAgoraMultiChannelDlg::OnEIDJoinChannelSuccess)
	ON_MESSAGE(WM_MSGID(EID_LEAVE_CHANNEL), &CAgoraMultiChannelDlg::OnEIDLeaveChannel)
	ON_MESSAGE(WM_MSGID(EID_USER_JOINED), &CAgoraMultiChannelDlg::OnEIDUserJoined)
	ON_MESSAGE(WM_MSGID(EID_USER_OFFLINE), &CAgoraMultiChannelDlg::OnEIDUserOffline)
	ON_MESSAGE(WM_MSGID(EID_REMOTE_VIDEO_STATE_CHANGED), &CAgoraMultiChannelDlg::OnEIDRemoteVideoStateChanged)
	ON_MESSAGE(WM_MSGID(EID_LOCAL_AUDIO_STATS), &CAgoraMultiChannelDlg::OnEIDLocalAudioStats)
	ON_MESSAGE(WM_MSGID(EID_LOCAL_AUDIO_STATE_CHANED), &CAgoraMultiChannelDlg::OnEIDLocalAudioStateChange)
	ON_MESSAGE(WM_MSGID(EID_LOCAL_VIDEO_STATS), &CAgoraMultiChannelDlg::OnEIDLocalVideoStats)
	ON_MESSAGE(WM_MSGID(EID_SNAPSHOT_TAKEN), &CAgoraMultiChannelDlg::onEIDSnapshotTaken)
	
	ON_BN_CLICKED(IDC_BUTTON_JOINCHANNEL, &CAgoraMultiChannelDlg::OnBnClickedButtonJoinchannel)
	ON_BN_CLICKED(IDC_BUTTON_LEAVE_CHANNEL, &CAgoraMultiChannelDlg::OnBnClickedButtonExChannel)
	ON_BN_CLICKED(IDC_BUTTON_SNAPSHOT_EX, &CAgoraMultiChannelDlg::OnBnClickedButtonExSnapshot)
	ON_LBN_SELCHANGE(IDC_LIST_INFO_BROADCASTING, &CAgoraMultiChannelDlg::OnSelchangeListInfoBroadcasting)
END_MESSAGE_MAP()


//Initialize the Ctrl Text.
void CAgoraMultiChannelDlg::InitCtrlText()
{
	m_staChannel.SetWindowText(commonCtrlChannel);
	m_btnJoinChannel.SetWindowText(commonCtrlJoinChannel);
	m_btnExChannel.SetWindowText(MultiChannelCtrlJoinExChannel);
	m_chkStopMic.SetWindowTextW(MultiChannelCtrlStopMic);
	m_btnExSnapshot.SetWindowTextW(MultiChannelCtrlTakeSnapshotEx);
}


//Initialize the Agora SDK
bool CAgoraMultiChannelDlg::InitAgora()
{
	//create Agora RTC engine
	m_rtcEngine = static_cast<IRtcEngineEx *>(createAgoraRtcEngine());
	if (!m_rtcEngine) {
		m_lstInfo.InsertString(m_lstInfo.GetCount() - 1, _T("createAgoraRtcEngine failed"));
		return false;
	}
	//set message notify receiver window
	m_mainChannelEventHandler.SetMsgReceiver(m_hWnd);
	m_mainChannelEventHandler.SetChannelId(0);
	agora::rtc::RtcEngineContext context;
	std::string strAppID = GET_APP_ID;
	context.appId = strAppID.c_str();
	context.eventHandler = &m_mainChannelEventHandler;
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


	m_exChannelRtcConn.channelId = new char[64];
	return true;
}


//UnInitialize the Agora SDK
void CAgoraMultiChannelDlg::UnInitAgora()
{
	if (m_rtcEngine) {
		delete m_exChannelRtcConn.channelId;

		if (m_joinChannel)
			//leave channel
			m_joinChannel = !m_rtcEngine->leaveChannel();

		// leave ex channel
		if (m_joinExChannel) {
			m_rtcEngine->leaveChannelEx(m_exChannelRtcConn);

			m_joinExChannel = false;
		}
		
		//stop preview in the engine.
		m_rtcEngine->stopPreview();
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("stopPreview"));
		//disable video in the engine.
		m_rtcEngine->disableVideo();
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("disableVideo"));
		//release engine.
		if (m_initialize) {
			m_rtcEngine->release(true);
			m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("release rtc engine"));
		}
		m_rtcEngine = NULL;
	}
}

//render local video from SDK local capture.
void CAgoraMultiChannelDlg::RenderLocalVideo()
{
	if (m_rtcEngine) {
		agora::rtc::VideoCanvas canvas;
		canvas.renderMode = media::base::RENDER_MODE_FIT;
		canvas.uid = 0;
		canvas.view = m_localVideoWnd.GetSafeHwnd();
		canvas.sourceType = VIDEO_SOURCE_CAMERA_PRIMARY;
		//setup local video in the engine to canvas.
		m_rtcEngine->setupLocalVideo(canvas);
		m_rtcEngine->startPreview();
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("setupLocalVideo"));

		m_localVideoWnd.ShowStatsInfo(true, FALSE);
		m_btnExChannel.EnableWindow(false);
	}
}


//resume window status
void CAgoraMultiChannelDlg::ResumeStatus()
{
	InitCtrlText();
	m_edtChannel.SetWindowText(_T(""));
	m_lstInfo.ResetContent();
	m_joinChannel = false;
	m_initialize = false;
	m_setVideoProc = false;
	m_chkStopMic.ShowWindow(FALSE);
	m_chkStopMic.SetCheck(FALSE);
}



void CAgoraMultiChannelDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);
	if (bShow) {
		//init control text.
		InitCtrlText();
		//update window.
		RenderLocalVideo();
	}
	else {
		//resume window status.
		ResumeStatus();
	}

}


BOOL CAgoraMultiChannelDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_localVideoWnd.Create(NULL, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CRect(0, 0, 1, 1), this, ID_BASEWND_VIDEO + 100);
	RECT rcArea;
	m_staVideoArea.GetClientRect(&rcArea);
	m_localVideoWnd.MoveWindow(&rcArea);
	ResumeStatus();
	return TRUE;
}


BOOL CAgoraMultiChannelDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) {
		return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}

void CAgoraMultiChannelDlg::OnBnClickedButtonJoinchannel()
{
	if (!m_rtcEngine || !m_initialize)
		return;

	CString strInfo;
	CString strChannelName;
	m_edtChannel.GetWindowText(strChannelName);
	if (strChannelName.IsEmpty()) {
		AfxMessageBox(_T("Fill channel name first"));
		return;
	}
	std::string szChannelId = cs2utf8(strChannelName);
	if (!m_joinChannel) {
		ChannelMediaOptions options;
		options.channelProfile = CHANNEL_PROFILE_LIVE_BROADCASTING;
		options.clientRoleType = CLIENT_ROLE_AUDIENCE;
		options.autoSubscribeAudio = true;
		options.autoSubscribeVideo = true;
		//join channel in the engine.
		int ret = m_rtcEngine->joinChannel(APP_TOKEN, szChannelId.c_str(), 0, options);
		strInfo.Format(_T("join channel %s, ret=%d"), strChannelName, ret);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

		if (ret == 0) {
			m_btnJoinChannel.EnableWindow(FALSE);

			CString secondChannelName;
			secondChannelName.Format(_T("%s-2"), strChannelName);
			joinSecondChannel(secondChannelName);
		}
	}
	else {
		m_rtcEngine->leaveChannel();
	}
	
}


void CAgoraMultiChannelDlg::OnBnClickedButtonExChannel()
{
	if (!m_rtcEngine || !m_initialize || !m_joinChannel)
		return;


	if (!m_joinExChannel) {
		std::string channelId = m_exChannelRtcConn.channelId;
		joinSecondChannel(utf82cs(channelId));
	}
	else {
		LeaveChannelOptions option;
		option.stopMicrophoneRecording = m_chkStopMic.GetCheck() == 1 ? true : false;
		m_rtcEngine->leaveChannelEx(m_exChannelRtcConn, option);
	}
}

void CAgoraMultiChannelDlg::OnBnClickedButtonExSnapshot()
{
	if (!m_rtcEngine || !m_initialize || !m_joinExChannel || m_secondChannelEventHandler.GetRemoteUid() <= 0) {
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("Please join ex channel and make sure there are users in the ex channel!"));
		return;
	}

	TCHAR szFilePath[MAX_PATH];
	::GetModuleFileName(NULL, szFilePath, MAX_PATH);
	LPTSTR lpLastSlash = _tcsrchr(szFilePath, _T('\\'));


	SIZE_T nNameLen = MAX_PATH - (lpLastSlash - szFilePath + 1);
	_tcscpy_s(lpLastSlash + 1, nNameLen, _T("snapshot_ex.jpg"));
	char filePath[MAX_PATH];
	wcstombs(filePath, szFilePath, wcslen(szFilePath) + 1);

	m_rtcEngine->takeSnapshotEx(m_exChannelRtcConn, m_secondChannelEventHandler.GetRemoteUid(), filePath);
}

void CAgoraMultiChannelDlg::OnSelchangeListInfoBroadcasting()
{
	int sel = m_lstInfo.GetCurSel();
	if (sel < 0)return;
	CString strDetail;
	m_lstInfo.GetText(sel, strDetail);
	m_edtDetailInfo.SetWindowText(strDetail);
}


void CAgoraMultiChannelDlg::joinSecondChannel(CString channelName)
{
	CString strInfo;
	std::string cn = cs2utf8(channelName);
	const char* cChannelName = cn.data();
	strcpy(const_cast<char*>(m_exChannelRtcConn.channelId), cChannelName);
	m_exChannelRtcConn.localUid = generateUid();
	ChannelMediaOptions option;
	option.clientRoleType = CLIENT_ROLE_BROADCASTER;
	option.publishCameraTrack = true;
	option.publishMicrophoneTrack = true;
	option.autoSubscribeVideo = true;
	option.autoSubscribeAudio = true;

	m_secondChannelEventHandler.SetChannelId(1);
	m_secondChannelEventHandler.SetMsgReceiver(m_hWnd);
	int ret = m_rtcEngine->joinChannelEx("", m_exChannelRtcConn, option, &m_secondChannelEventHandler);

	strInfo.Format(_T("join channelEx %s , ret=%d"), channelName, ret);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

	if (ret == 0)
	{
		m_btnExChannel.EnableWindow(FALSE);
	}
}

//EID_JOINCHANNEL_SUCCESS message window handler.
LRESULT CAgoraMultiChannelDlg::OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam)
{
	int cId = (int)lParam;
	
	CString strInfo;
	if (cId == 0) {
		CString strChannelName = utf82cs(m_mainChannelEventHandler.GetChannelName());
		m_joinChannel = true;

		m_btnJoinChannel.EnableWindow(TRUE);
		m_btnJoinChannel.SetWindowTextW(commonCtrlLeaveChannel);

		strInfo.Format(_T("join %s success, uid=%u"), strChannelName, wParam);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
		m_localVideoWnd.SetUID(wParam);
	}
	else {
		CString strChannelName = utf82cs(m_secondChannelEventHandler.GetChannelName());
		m_joinExChannel = true;

		m_btnExChannel.EnableWindow(TRUE);
		m_btnExChannel.SetWindowTextW(MultiChannelCtrlLeaveExChannel);
		m_chkStopMic.ShowWindow(TRUE);

		strInfo.Format(_T("join %s success, uid=%u"), strChannelName, wParam);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

	}

	return 0;
}

//EID_LEAVE_CHANNEL message window handler.
LRESULT CAgoraMultiChannelDlg::OnEIDLeaveChannel(WPARAM wParam, LPARAM lParam)
{
	int cId = (int)wParam;
	
	CString strInfo;
	
	
	if (cId == 0) {
		CString strChannelName = utf82cs(m_mainChannelEventHandler.GetChannelName());
		m_joinChannel = false;
		m_btnJoinChannel.SetWindowText(commonCtrlJoinChannel);
		m_btnExChannel.EnableWindow(FALSE);
		strInfo.Format(_T("leave channel:%s "), strChannelName);
	}
	else {
		CString strChannelName = utf82cs(m_secondChannelEventHandler.GetChannelName());
		m_joinExChannel = false;	
		m_btnExChannel.SetWindowText(MultiChannelCtrlJoinExChannel);
		m_chkStopMic.ShowWindow(FALSE);

		strInfo.Format(_T("leave channel:%s "), strChannelName);
	}
	
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	return 0;
}

//EID_USER_JOINED message window handler.
LRESULT CAgoraMultiChannelDlg::OnEIDUserJoined(WPARAM wParam, LPARAM lParam)
{
	int cId = (int)lParam;
	CString strInfo;
	if (cId == 0) {
		CString strChannelName = utf82cs(m_mainChannelEventHandler.GetChannelName());
		strInfo.Format(_T("%u joined %s"), wParam, strChannelName);
	}
	else {
		CString strChannelName = utf82cs(m_secondChannelEventHandler.GetChannelName());
		strInfo.Format(_T("%u joined %s"), wParam, strChannelName);
	}
	
	
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	return 0;
}


//EID_USER_OFFLINE message window handler.
LRESULT CAgoraMultiChannelDlg::OnEIDUserOffline(WPARAM wParam, LPARAM lParam)
{
	int cId = (int)lParam;
	CString strInfo;
	agora::rtc::uid_t remoteUid = (agora::rtc::uid_t)wParam;
	if (cId == 0) {
		CString strChannelName = utf82cs(m_mainChannelEventHandler.GetChannelName());
		strInfo.Format(_T("%u offline %s"), remoteUid, strChannelName);
	}
	else {
		CString strChannelName = utf82cs(m_secondChannelEventHandler.GetChannelName());
		strInfo.Format(_T("%u offline %s"), remoteUid, strChannelName);
	}
	
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	return 0;
}

//EID_REMOTE_VIDEO_STATE_CHANGED message window handler.
LRESULT CAgoraMultiChannelDlg::OnEIDRemoteVideoStateChanged(WPARAM wParam, LPARAM lParam)
{
	
	return 0;
}


LRESULT CAgoraMultiChannelDlg::OnEIDLocalAudioStats(WPARAM wParam, LPARAM lParam)
{
	LocalAudioStats* stats = reinterpret_cast<LocalAudioStats*>(wParam);

	m_localVideoWnd.SetAudioStatsInfo(stats->sentBitrate, stats->txPacketLossRate);

	delete stats;
	return 0;
}


LRESULT CAgoraMultiChannelDlg::OnEIDLocalAudioStateChange(WPARAM wParam, LPARAM lParam)
{
	LOCAL_AUDIO_STREAM_STATE state = LOCAL_AUDIO_STREAM_STATE(wParam);
	LOCAL_AUDIO_STREAM_REASON error = LOCAL_AUDIO_STREAM_REASON(lParam);

	CString strInfo;
	strInfo.Format(_T("Local Audio State : %d"), state);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

	return 0;
}


LRESULT CAgoraMultiChannelDlg::OnEIDLocalVideoStats(WPARAM wParam, LPARAM lParam)
{
	LocalVideoStats *stats = reinterpret_cast<LocalVideoStats*>(wParam);

	m_localVideoWnd.SetVideoStatsInfo(stats->encodedFrameWidth, stats->encodedFrameHeight,
		stats->sentFrameRate, stats->sentBitrate, stats->txPacketLossRate);

	delete stats;

	return 0;
}


LRESULT CAgoraMultiChannelDlg::onEIDSnapshotTaken(WPARAM wParam, LPARAM lParam) {
	CString strInfo = _T("===onEIDSnapshotTaken===");
	CString* filePath = reinterpret_cast<CString*>(wParam);

	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	int errCode = (int)lParam;
	strInfo.Format(_T("snapshot taken err:%d"), errCode);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	if (errCode == 0) {
		strInfo.Format(_T("path: %s"), *filePath);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	}

	delete filePath;
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
void CAgoraMultiChannelEventHandler::onJoinChannelSuccess(const char* channel, agora::rtc::uid_t uid, int elapsed)
{
	m_strChannel = channel;
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
void CAgoraMultiChannelEventHandler::onUserJoined(agora::rtc::uid_t uid, int elapsed)
{
	m_remoteUid = uid;
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
void CAgoraMultiChannelEventHandler::onUserOffline(agora::rtc::uid_t uid, agora::rtc::USER_OFFLINE_REASON_TYPE reason)
{
	if (m_remoteUid == uid) {
		m_remoteUid = -1;
	}
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
void CAgoraMultiChannelEventHandler::onLeaveChannel(const agora::rtc::RtcStats& stats)
{
	if (m_hMsgHanlder) {
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_LEAVE_CHANNEL), (WPARAM)m_channelId, 0);
	}
}

void CAgoraMultiChannelEventHandler::onRemoteVideoStateChanged(agora::rtc::uid_t uid, agora::rtc::REMOTE_VIDEO_STATE state, agora::rtc::REMOTE_VIDEO_STATE_REASON reason, int elapsed)
{
	if (m_hMsgHanlder) {
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_REMOTE_VIDEO_STATE_CHANGED), (WPARAM)uid, (LPARAM)m_channelId);
	}
}

void CAgoraMultiChannelEventHandler::onLocalAudioStats(const LocalAudioStats& stats)
{
	if (m_hMsgHanlder) {
		LocalAudioStats* s = new LocalAudioStats;
		*s = stats;
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_LOCAL_AUDIO_STATS), (WPARAM)s, 0);
	}
}

void CAgoraMultiChannelEventHandler::onLocalAudioStateChanged(LOCAL_AUDIO_STREAM_STATE state, LOCAL_AUDIO_STREAM_REASON error)
{
	if (m_hMsgHanlder) {
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_LOCAL_AUDIO_STATE_CHANED), (WPARAM)state, error);
	}
}

void CAgoraMultiChannelEventHandler::onLocalVideoStats(VIDEO_SOURCE_TYPE source, const LocalVideoStats& stats)
{
	if (m_hMsgHanlder) {
		LocalVideoStats* s = new LocalVideoStats;
		*s = stats;
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_LOCAL_VIDEO_STATS), (WPARAM)s, source);
	}
}

void CAgoraMultiChannelEventHandler::onSnapshotTaken(uid_t uid, const char* filePath, int width, int height, int errCode)
{
	if (m_hMsgHanlder) {
		CString* _filePath = new CString;
		_filePath->Format(_T("%s"), utf82cs(std::string(filePath)));

		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_SNAPSHOT_TAKEN), (WPARAM)_filePath, errCode);
	}
}
