﻿#include "stdafx.h"
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
	DDX_Control(pDX, IDC_STATIC_CHANNEL_LIST, m_staCurChannel);
	DDX_Control(pDX, IDC_COMBO_CHANNEL_LIST, m_cmbChannelList);
	DDX_Control(pDX, IDC_BUTTON_LEAVE_CHANNEL, m_btnLeaveChannel);
}


BEGIN_MESSAGE_MAP(CAgoraMultiChannelDlg, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_MESSAGE(WM_MSGID(EID_JOINCHANNEL_SUCCESS), &CAgoraMultiChannelDlg::OnEIDJoinChannelSuccess)
	ON_MESSAGE(WM_MSGID(EID_LEAVE_CHANNEL), &CAgoraMultiChannelDlg::OnEIDLeaveChannel)
	ON_MESSAGE(WM_MSGID(EID_USER_JOINED), &CAgoraMultiChannelDlg::OnEIDUserJoined)
	ON_MESSAGE(WM_MSGID(EID_USER_OFFLINE), &CAgoraMultiChannelDlg::OnEIDUserOffline)
	ON_MESSAGE(WM_MSGID(EID_REMOTE_VIDEO_STATE_CHANED), &CAgoraMultiChannelDlg::OnEIDRemoteVideoStateChanged)
	ON_BN_CLICKED(IDC_BUTTON_JOINCHANNEL, &CAgoraMultiChannelDlg::OnBnClickedButtonJoinchannel)
	ON_BN_CLICKED(IDC_BUTTON_LEAVE_CHANNEL, &CAgoraMultiChannelDlg::OnBnClickedButtonLeaveChannel)
	ON_LBN_SELCHANGE(IDC_LIST_INFO_BROADCASTING, &CAgoraMultiChannelDlg::OnSelchangeListInfoBroadcasting)
END_MESSAGE_MAP()


//Initialize the Ctrl Text.
void CAgoraMultiChannelDlg::InitCtrlText()
{
	m_staChannel.SetWindowText(commonCtrlChannel);
	m_btnJoinChannel.SetWindowText(commonCtrlJoinChannel);
	m_btnLeaveChannel.SetWindowText(commonCtrlLeaveChannel);
	m_staCurChannel.SetWindowText(MultiChannelCtrlChannelList);
}


//Initialize the Agora SDK
bool CAgoraMultiChannelDlg::InitAgora()
{
	//create Agora RTC engine
	m_rtcEngine = createAgoraRtcEngine();
	if (!m_rtcEngine) {
		m_lstInfo.InsertString(m_lstInfo.GetCount() - 1, _T("createAgoraRtcEngine failed"));
		return false;
	}
	CAgoraMultiChannelEventHandler * p = new CAgoraMultiChannelEventHandler;
	//set message notify receiver window
	p->SetMsgReceiver(m_hWnd);
	p->SetChannelId(0);
	m_vecChannelEventHandler.push_back(p);
	agora::rtc::RtcEngineContext context;
	std::string strAppID = GET_APP_ID;
	context.appId = strAppID.c_str();
	context.eventHandler = p;
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
	return true;
}


//UnInitialize the Agora SDK
void CAgoraMultiChannelDlg::UnInitAgora()
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
	}
}


//resume window status
void CAgoraMultiChannelDlg::ResumeStatus()
{

	for (int i = 1; i < m_vecChannelEventHandler.size(); i++)
	{
		delete m_vecChannelEventHandler.back();
		m_vecChannelEventHandler.pop_back();
	}

	InitCtrlText();
	m_edtChannel.SetWindowText(_T(""));
	m_mapConn.clear();
	m_cmbChannelList.ResetContent();
	m_lstInfo.ResetContent();
	m_joinChannel = false;
	m_initialize = false;
	m_setVideoProc = false;
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
	if (m_mapConn.size()!=0 && m_mapConn.find(strChannelName) != m_mapConn.end())
	{
		AfxMessageBox(_T("you already joined this channel."));
		return;
	}
	std::string szChannelId = cs2utf8(strChannelName);
	if (!m_joinChannel) {
		ChannelMediaOptions options;
		options.channelProfile = CHANNEL_PROFILE_LIVE_BROADCASTING;
		options.clientRoleType = CLIENT_ROLE_BROADCASTER;
		//join channel in the engine.
		if (0 == m_rtcEngine->joinChannel(APP_TOKEN, szChannelId.c_str(), 0, options)) {
			strInfo.Format(_T("join channel %s, use ChannelMediaOptions"), strChannelName);
			m_btnJoinChannel.EnableWindow(FALSE);
			m_mapConn.insert(std::make_pair(strChannelName,DEFAULT_CONNECTION_ID));
			m_cmbChannelList.InsertString(m_cmbChannelList.GetCount(), strChannelName);
		}
		m_cmbChannelList.SetCurSel(0);
	}
	else {
		conn_id_t conn_id;
		agora::rtc::ChannelMediaOptions options;
		options.autoSubscribeAudio = true;
		options.autoSubscribeVideo = true;
		options.clientRoleType = CLIENT_ROLE_BROADCASTER;
		CAgoraMultiChannelEventHandler * p = new CAgoraMultiChannelEventHandler;
		p->SetChannelId(m_vecChannelEventHandler.size());
		p->SetMsgReceiver(GetSafeHwnd());
		m_vecChannelEventHandler.push_back(p);
		if (0 == m_rtcEngine->joinChannelEx(APP_TOKEN, szChannelId.c_str(), 0, options, p, &conn_id))
		{
			strInfo.Format(_T("join channel ex %d"), conn_id);
			m_mapConn.insert(std::make_pair(strChannelName, conn_id));
			m_cmbChannelList.InsertString(m_cmbChannelList.GetCount(), strChannelName);
			m_btnJoinChannel.EnableWindow(FALSE);
		}
	}
	//m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
}


void CAgoraMultiChannelDlg::OnBnClickedButtonLeaveChannel()
{
	int nSel = 	m_cmbChannelList.GetCurSel();
	if (nSel < 0)
	{
		return;
	}
	CString strChannel;
	m_cmbChannelList.GetWindowText(strChannel);
	std::string utfChannel = cs2utf8(strChannel);
	//CString strInfo;
	auto conn_id = m_mapConn[strChannel];
	m_mapConn.erase(strChannel);
	m_rtcEngine->leaveChannelEx(utfChannel.c_str(), conn_id);
	//strInfo.Format(_T("conn_id: %d:leave channel."), conn_id);
	//m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	m_cmbChannelList.DeleteString(nSel);
	m_cmbChannelList.SetCurSel(0);
}


void CAgoraMultiChannelDlg::OnSelchangeListInfoBroadcasting()
{
	int sel = m_lstInfo.GetCurSel();
}

//EID_JOINCHANNEL_SUCCESS message window handler.
LRESULT CAgoraMultiChannelDlg::OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam)
{
	int cId = (int)lParam;
	CString strChannelName =  utf82cs(m_vecChannelEventHandler[cId]->GetChannelName());
	m_joinChannel = true;
	m_btnJoinChannel.EnableWindow(TRUE);
	CString strInfo;
	strInfo.Format(_T("join %s success, uid=%u"), strChannelName, wParam);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	m_localVideoWnd.SetUID(wParam);
	return 0;
}

//EID_LEAVE_CHANNEL message window handler.
LRESULT CAgoraMultiChannelDlg::OnEIDLeaveChannel(WPARAM wParam, LPARAM lParam)
{
	int cId = (int)wParam;
	CString strChannelName = utf82cs(m_vecChannelEventHandler[cId]->GetChannelName());
	m_btnJoinChannel.SetWindowText(commonCtrlJoinChannel);
	CString strInfo;
	strInfo.Format(_T("leave channel:%s "), strChannelName);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	
	if (cId == 0) {
		m_joinChannel = false;
	}
	else {
		delete m_vecChannelEventHandler[cId];
		m_vecChannelEventHandler.erase(m_vecChannelEventHandler.begin() + cId);
		for (int i = cId; i < m_vecChannelEventHandler.size(); ++i)
		{
			m_vecChannelEventHandler[cId]->SetChannelId(m_vecChannelEventHandler[cId]->GetChannelId()-1);
		}
	}
	return 0;
}

//EID_USER_JOINED message window handler.
LRESULT CAgoraMultiChannelDlg::OnEIDUserJoined(WPARAM wParam, LPARAM lParam)
{
	int cId = (int)lParam;
	CString strChannelName = utf82cs(m_vecChannelEventHandler[cId]->GetChannelName());
	CString strInfo;
	strInfo.Format(_T("%u joined %s"), wParam, strChannelName);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	return 0;
}


//EID_USER_OFFLINE message window handler.
LRESULT CAgoraMultiChannelDlg::OnEIDUserOffline(WPARAM wParam, LPARAM lParam)
{
	int cId = (int)lParam;
	CString strChannelName = utf82cs(m_vecChannelEventHandler[cId]->GetChannelName());

	agora::rtc::uid_t remoteUid = (agora::rtc::uid_t)wParam;
	CString strInfo;
	strInfo.Format(_T("%u offline %s"), remoteUid, strChannelName);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	return 0;
}

//EID_REMOTE_VIDEO_STATE_CHANED message window handler.
LRESULT CAgoraMultiChannelDlg::OnEIDRemoteVideoStateChanged(WPARAM wParam, LPARAM lParam)
{
	//PVideoStateStateChanged stateChanged = (PVideoStateStateChanged)wParam;
	//if (stateChanged) {
	//	//onRemoteVideoStateChanged
	//	CString strSateInfo;
	//	switch (stateChanged->state) {
	//	case agora::rtc::REMOTE_VIDEO_STATE_STARTING:
	//		strSateInfo = _T("REMOTE_VIDEO_STATE_STARTING");
	//		break;
	//	case agora::rtc::REMOTE_VIDEO_STATE_STOPPED:
	//		strSateInfo = _T("strSateInfo");
	//		break;
	//	case agora::rtc::REMOTE_VIDEO_STATE_DECODING:
	//		strSateInfo = _T("REMOTE_VIDEO_STATE_DECODING");
	//		break;
	//	case agora::rtc::REMOTE_VIDEO_STATE_FAILED:
	//		strSateInfo = _T("REMOTE_VIDEO_STATE_FAILED ");
	//		break;
	//	case agora::rtc::REMOTE_VIDEO_STATE_FROZEN:
	//		strSateInfo = _T("REMOTE_VIDEO_STATE_FROZEN  ");
	//		break;
	//	}
	//	CString strInfo;
	//	strInfo.Format(_T("onRemoteVideoStateChanged: uid=%u, %s"), stateChanged->uid, strSateInfo);
	//	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	//}
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
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_REMOTE_VIDEO_STATE_CHANED), (WPARAM)uid, (LPARAM)m_channelId);
	}
}
