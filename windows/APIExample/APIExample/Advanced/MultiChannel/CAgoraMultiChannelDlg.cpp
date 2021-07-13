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
	ON_MESSAGE(WM_MSGID(EID_REMOTE_CHANNEL_MDIA_REPLAY), &CAgoraMultiChannelDlg::OnEIDMediaReplay)
	ON_MESSAGE(WM_MSGID(EID_CHANNEL_REPLAY_STATE_CHANGED), &CAgoraMultiChannelDlg::OnEIDMediaReplayStateChanged)
	
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

	multiChannelConfig.destCount = 0;
	multiChannelConfig.srcInfo = new ChannelMediaInfo();
	multiChannelConfig.srcInfo->channelName = new char[256];
	multiChannelConfig.srcInfo->token = APP_TOKEN;
	memset(const_cast<char*>(multiChannelConfig.srcInfo->channelName), 0, 256);
	multiChannelConfig.destInfos = new ChannelMediaInfo[4];
	for (int i = 0; i < 4; ++i) {
		multiChannelConfig.destInfos[i].channelName = new char[256];
		memset(const_cast<char*>(multiChannelConfig.destInfos[i].channelName), 0, 256);
		multiChannelConfig.destInfos[i].token = new char[256];
		memset(const_cast<char*>(multiChannelConfig.destInfos[i].token), 0, 256);
	}

	return true;
}


//UnInitialize the Agora SDK
void CAgoraMultiChannelDlg::UnInitAgora()
{
	if (m_rtcEngine) {
		multiChannelConfig.destCount = 0;
		if (multiChannelConfig.srcInfo) {
			delete[] multiChannelConfig.srcInfo->channelName;
			multiChannelConfig.srcInfo->channelName = nullptr;
			delete multiChannelConfig.srcInfo;
			multiChannelConfig.srcInfo = nullptr;
		}
		
		for (int i = 0; i < 4; ++i) {
			auto p = multiChannelConfig.destInfos[i];
			delete[] p.channelName;
			delete[] p.token;
			p.channelName = nullptr;
			p.token = nullptr;			
		}

		delete[] multiChannelConfig.destInfos;
		multiChannelConfig.destInfos = nullptr;
		if (bStart) {
			m_rtcEngine->stopChannelMediaRelay();
			bStart = false;
		}
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

	if (m_cmbChannelList.GetCount() == 4) {
		AfxMessageBox(_T("4 channels at most"));
		return;
	}

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
			MediaReplayInfo info;
			info.channelName = szChannelId;
			info.token = APP_TOKEN;
			m_mapConn.insert(std::make_pair(strChannelName, info));
			strcpy(const_cast<char*>(multiChannelConfig.srcInfo->channelName), szChannelId.c_str());
			multiChannelConfig.destCount = 1;
			strInfo.Format(_T("join channel %s, use ChannelMediaOptions"), strChannelName);
			m_btnJoinChannel.EnableWindow(FALSE);			
			m_cmbChannelList.InsertString(m_cmbChannelList.GetCount(), strChannelName);
		}
		m_cmbChannelList.SetCurSel(0);
	}
	else {
		MediaReplayInfo info;
		info.channelName = szChannelId;
		info.uid = multiChannelConfig.srcInfo->uid;
		m_mapConn.insert(std::make_pair(strChannelName, info));
		m_cmbChannelList.InsertString(m_cmbChannelList.GetCount(), strChannelName);
		int sel = m_cmbChannelList.GetCount() - 1;
		strcpy(const_cast<char*>(multiChannelConfig.destInfos[sel].channelName), szChannelId.c_str());
		multiChannelConfig.destCount = m_cmbChannelList.GetCount();
		int ret = m_rtcEngine->updateChannelMediaRelay(multiChannelConfig);
		CString strInfo;
		strInfo.Format(_T("updateChannelMediaRelay:%d"), ret);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

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
#if 0
	CString strChannel;
	m_cmbChannelList.GetWindowText(strChannel);
	std::string utfChannel = cs2utf8(strChannel);
	//CString strInfo;
	auto conn_id = m_mapConn[strChannel];
	m_mapConn.erase(strChannel);
	
	for (int i = nSel; i < multiChannelConfig.destCount-1 ; ++i) {
		strcpy(const_cast<char*>(multiChannelConfig.destInfos[i].channelName), const_cast<char*>(multiChannelConfig.destInfos[i+1].channelName));
	}
	
	int lastIndex = multiChannelConfig.destCount - 1;
	memset(const_cast<char*>(multiChannelConfig.destInfos[lastIndex].channelName), 0, 256);
	multiChannelConfig.destCount -= 1;

	if (multiChannelConfig.destCount >= 1) {
		int ret = m_rtcEngine->updateChannelMediaRelay(multiChannelConfig);
		CString strInfo;
		strInfo.Format(_T("updateChannelMediaRelay:%d"), ret);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
		m_btnJoinChannel.EnableWindow(FALSE);
		m_btnLeaveChannel.EnableWindow(FALSE);
	}
	else if(multiChannelConfig.destCount == 0){
		int ret = m_rtcEngine->stopChannelMediaRelay();

		CString strInfo;
		strInfo.Format(_T("stopChannelMediaRelay:%d"), ret);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
		m_rtcEngine->leaveChannel();
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("leaveChannel"));
		bStart = false;
	}
	m_cmbChannelList.DeleteString(nSel);
	m_cmbChannelList.SetCurSel(0);
#else
	int ret = m_rtcEngine->stopChannelMediaRelay();
	bStart = false;
	CString strInfo;
	strInfo.Format(_T("stopChannelMediaRelay:%d"), ret);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	m_rtcEngine->leaveChannel();
	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("leaveChannel"));
	m_cmbChannelList.Clear();
	memset(const_cast<char*>(multiChannelConfig.srcInfo->channelName), 0, 256);
	multiChannelConfig.destInfos = new ChannelMediaInfo[4];
	for (int i = 0; i < 4; ++i) {
		multiChannelConfig.destInfos[i].channelName = new char[256];
		memset(const_cast<char*>(multiChannelConfig.destInfos[i].channelName), 0, 256);
		multiChannelConfig.destInfos[i].token = new char[256];
		memset(const_cast<char*>(multiChannelConfig.destInfos[i].token), 0, 256);
	}

	multiChannelConfig.destCount = 0;

	m_mapConn.clear();
	m_cmbChannelList.ResetContent();
	m_edtChannel.SetWindowText(_T(""));
#endif
	
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

	multiChannelConfig.srcInfo->uid = wParam;
	
	for (int i = 0; i < 4; ++i) {
		multiChannelConfig.destInfos[i].uid = wParam;
	}
	strcpy(const_cast<char*>(multiChannelConfig.destInfos[0].channelName), multiChannelConfig.srcInfo->channelName);
	auto& info = m_mapConn[utf82cs(multiChannelConfig.srcInfo->channelName)];
	info.uid = wParam;

	m_rtcEngine->startChannelMediaRelay(multiChannelConfig);
	bStart = true;
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
	
	return 0;
}

LRESULT CAgoraMultiChannelDlg::OnEIDMediaReplay(WPARAM wParam, LPARAM lParam)
{
	CHANNEL_MEDIA_RELAY_EVENT code = (CHANNEL_MEDIA_RELAY_EVENT)wParam;
	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("onChannelMediaRelayEvent"));
	CString strInfo;
	switch (code)
	{
	case agora::rtc::RELAY_EVENT_NETWORK_DISCONNECTED:
		strInfo = _T("NETWORK_DISCONNECTED");
		break;
	case agora::rtc::RELAY_EVENT_NETWORK_CONNECTED:
		strInfo = _T("NETWORK_CONNECTED");
		break;
	case agora::rtc::RELAY_EVENT_PACKET_JOINED_SRC_CHANNEL:
		strInfo = _T("PACKET_JOINED_SRC_CHANNEL");
		break;
	case agora::rtc::RELAY_EVENT_PACKET_JOINED_DEST_CHANNEL:
		strInfo = _T("PACKET_JOINED_DEST_CHANNEL");
		break;
	case agora::rtc::RELAY_EVENT_PACKET_SENT_TO_DEST_CHANNEL:
		strInfo = _T("PACKET_SENT_TO_DEST_CHANNEL");
		break;
	case agora::rtc::RELAY_EVENT_PACKET_RECEIVED_VIDEO_FROM_SRC:
		strInfo = _T("PACKET_RECEIVED_VIDEO_FROM_SRC");
		break;
	case agora::rtc::RELAY_EVENT_PACKET_RECEIVED_AUDIO_FROM_SRC:
		strInfo = _T("PACKET_RECEIVED_AUDIO_FROM_SRC");
		break;
	case agora::rtc::RELAY_EVENT_PACKET_UPDATE_DEST_CHANNEL:
		strInfo = _T("PACKET_UPDATE_DEST_CHANNEL");
		break;
	case agora::rtc::RELAY_EVENT_PACKET_UPDATE_DEST_CHANNEL_REFUSED:
		strInfo = _T("PACKET_UPDATE_DEST_CHANNEL_REFUSED");
		break;
	case agora::rtc::RELAY_EVENT_PACKET_UPDATE_DEST_CHANNEL_NOT_CHANGE:
		strInfo = _T("UPDATE_DEST_CHANNEL_NOT_CHANGE");
		break;
	case agora::rtc::RELAY_EVENT_PACKET_UPDATE_DEST_CHANNEL_IS_NULL:
		strInfo = _T("PACKET_UPDATE_DEST_CHANNEL_IS_NULL");
		break;
	case agora::rtc::RELAY_EVENT_VIDEO_PROFILE_UPDATE:
		strInfo = _T("VIDEO_PROFILE_UPDATE");
		break;
	default:
		break;
	}

	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

	m_btnJoinChannel.EnableWindow(TRUE);
	m_btnLeaveChannel.EnableWindow(TRUE);
	return 0;
}

LRESULT CAgoraMultiChannelDlg::OnEIDMediaReplayStateChanged(WPARAM wParam, LPARAM lParam)
{
	CHANNEL_MEDIA_RELAY_STATE state = (CHANNEL_MEDIA_RELAY_STATE)wParam;
	CString strState;
	switch (state)
	{
	case agora::rtc::RELAY_STATE_IDLE:
		strState = _T("idle");
		break;
	case agora::rtc::RELAY_STATE_CONNECTING:
		strState = _T("connecting");
		break;
	case agora::rtc::RELAY_STATE_RUNNING:
		strState = _T("running");
		break;
	case agora::rtc::RELAY_STATE_FAILURE:
		strState = _T("failure");
		break;
	default:
		break;
	}
	CString strError;
	int error = lParam;
	switch (error)
	{
	case 0:
		strError = _T("OK");
		break;
	case 1:
		strError = _T("SERVER_ERROR_RESPONSE");
		break;
	case 2:
		strError = _T("SERVER_NO_RESPONSE");
		break;
	case 3:
		strError = _T("NO_RESOURCE_AVAILABLE");
		break;
	case 4:
		strError = _T("FAILED_JOIN_SRC");
		break;

	case 5:
		strError = _T("FAILED_JOIN_DEST");
		break;
	case 6:
		strError = _T("FAILED_PACKET_RECEIVED_FROM_SRC");
		break;
	case 7:
		strError = _T("FAILED_PACKET_SENT_TO_DEST");
		break;
	case 8:
		strError = _T("SERVER_CONNECTION_LOST");
		break;

	case 9:
		strError = _T("INTERNAL_ERROR");
		break;
	case 10:
		strError = _T("SRC_TOKEN_EXPIRED");
		break;
	case 11:
		strError = _T("DEST_TOKEN_EXPIRED");
		break;

	default:
		break;
	}
	
	m_lstInfo.InsertString(m_lstInfo.GetCount() , _T("MediaReplayStateChanged"));
	m_lstInfo.InsertString(m_lstInfo.GetCount() , strState);
	m_lstInfo.InsertString(m_lstInfo.GetCount() , strError);

	m_btnJoinChannel.EnableWindow(TRUE);
	m_btnLeaveChannel.EnableWindow(TRUE);
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

void CAgoraMultiChannelEventHandler::onChannelMediaRelayEvent(int code)
{
	if (m_hMsgHanlder) {
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_REMOTE_CHANNEL_MDIA_REPLAY), (WPARAM)code, 0);
	}
}

void CAgoraMultiChannelEventHandler::onChannelMediaRelayStateChanged(int state, int code)
{
	if (m_hMsgHanlder) {
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_CHANNEL_REPLAY_STATE_CHANGED), (WPARAM)state, code);
	}
}