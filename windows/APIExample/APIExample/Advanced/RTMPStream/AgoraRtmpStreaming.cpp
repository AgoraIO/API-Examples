// AgoraRtmpStreaming.cpp : implementation file


#include "stdafx.h"
#include "APIExample.h"
#include "AgoraRtmpStreaming.h"
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
void CAgoraRtmpStreamingDlgRtcEngineEventHandler::onJoinChannelSuccess(const char* channel, uid_t uid, int elapsed)
{
	if (m_hMsgHanlder) {
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_JOINCHANNEL_SUCCESS), (WPARAM)uid, (LPARAM)elapsed);
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
void CAgoraRtmpStreamingDlgRtcEngineEventHandler::onLeaveChannel(const RtcStats& stats)
{
	if (m_hMsgHanlder) {
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_LEAVE_CHANNEL), 0, 0);
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
void CAgoraRtmpStreamingDlgRtcEngineEventHandler::onUserJoined(uid_t uid, int elapsed)
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
void CAgoraRtmpStreamingDlgRtcEngineEventHandler::onUserOffline(uid_t uid, USER_OFFLINE_REASON_TYPE reason)
{
	if (m_hMsgHanlder) {
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_USER_OFFLINE), (WPARAM)uid, (LPARAM)reason);
	}
}
/**
	Occurs when the state of the RTMP streaming changes.
	The SDK triggers this callback to report the result of the local user calling the \ref agora::rtc::IRtcEngine::addPublishStreamUrl "addPublishStreamUrl" or \ref agora::rtc::IRtcEngine::removePublishStreamUrl "removePublishStreamUrl" method.
	This callback indicates the state of the RTMP streaming. When exceptions occur, you can troubleshoot issues by referring to the detailed error descriptions in the *errCode* parameter.
	@param url The RTMP URL address.
	@param state The RTMP streaming state. See: #RTMP_STREAM_PUBLISH_STATE.
	@param errCode The detailed error information for streaming. See: #RTMP_STREAM_PUBLISH_ERROR.
 */
void CAgoraRtmpStreamingDlgRtcEngineEventHandler::onRtmpStreamingStateChanged(const char *url, RTMP_STREAM_PUBLISH_STATE state, RTMP_STREAM_PUBLISH_ERROR_TYPE errCode)
{
	if (m_hMsgHanlder) {
		PRtmpStreamStreamStateChanged rtmpState = new RtmpStreamStreamStateChanged;
		int len = strlen(url);
		rtmpState->url = new char[len + 1];
		rtmpState->url[len] = 0;
		strcpy_s(rtmpState->url, len + 1, url);
		rtmpState->state = state;
		rtmpState->error = errCode;
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_RTMP_STREAM_STATE_CHANGED), (WPARAM)rtmpState, 0);
	}
}

void CAgoraRtmpStreamingDlgRtcEngineEventHandler::onRtmpStreamingEvent(const char* url, RTMP_STREAMING_EVENT eventCode)
{
	if (m_hMsgHanlder) {
		PRtmpStreamEvent rtmpEvent = new RtmpStreamEvent;
		int len = strlen(url);
		rtmpEvent->url = new char[len + 1];
		rtmpEvent->url[len] = 0;
		strcpy_s(rtmpEvent->url, len + 1, url);
		rtmpEvent->eventCode = eventCode;
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_RTMP_STREAM_EVENT), (WPARAM)rtmpEvent, 0);
	}
}

IMPLEMENT_DYNAMIC(CAgoraRtmpStreamingDlg, CDialogEx)

CAgoraRtmpStreamingDlg::CAgoraRtmpStreamingDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_RTMP_STREAMING, pParent)
{

}

CAgoraRtmpStreamingDlg::~CAgoraRtmpStreamingDlg()
{
}

void CAgoraRtmpStreamingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_INFO_BROADCASTING, m_lstInfo);
	DDX_Control(pDX, IDC_BUTTON_JOINCHANNEL, m_btnJoinChannel);
	DDX_Control(pDX, IDC_STATIC_VIDEO, m_staVideoArea);
	DDX_Control(pDX, IDC_EDIT_CHANNELNAME, m_edtChannelName);
	DDX_Control(pDX, IDC_EDIT_RTMP_URL, m_edtRtmpUrl);
	DDX_Control(pDX, IDC_BUTTON_ADDSTREAM, m_btnAddStream);
	DDX_Control(pDX, IDC_BUTTON_REMOVE_STREAM, m_btnRemoveStream);
	DDX_Control(pDX, IDC_COMBO_RTMPURLS, m_cmbRtmpUrl);
	DDX_Control(pDX, IDC_STATIC_CHANNELNAME, m_staChannelName);
	DDX_Control(pDX, IDC_STATIC_RTMP_URL, m_staPublishUrl);
	DDX_Control(pDX, IDC_BUTTON_REMOVE_ALLSTREAM, m_btnRemoveAll);
	DDX_Control(pDX, IDC_STATIC_RTMP_URLS, m_staRemoveUrl);
	DDX_Control(pDX, IDC_STATIC_DETAIL, m_staDetail);
	DDX_Control(pDX, IDC_CHK_TRANS_CODING, m_chkTransCoding);

	DDX_Control(pDX, IDC_CHK_TRANS_CODING, m_chkTransCoding);
}


BEGIN_MESSAGE_MAP(CAgoraRtmpStreamingDlg, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_MESSAGE(WM_MSGID(EID_JOINCHANNEL_SUCCESS), &CAgoraRtmpStreamingDlg::OnEIDJoinChannelSuccess)
	ON_MESSAGE(WM_MSGID(EID_LEAVE_CHANNEL), &CAgoraRtmpStreamingDlg::OnEIDLeaveChannel)
	ON_MESSAGE(WM_MSGID(EID_RTMP_STREAM_STATE_CHANGED), &CAgoraRtmpStreamingDlg::OnEIDRtmpStateChanged)
	ON_MESSAGE(WM_MSGID(EID_USER_JOINED), &CAgoraRtmpStreamingDlg::OnEIDUserJoined)
	ON_MESSAGE(WM_MSGID(EID_USER_OFFLINE), &CAgoraRtmpStreamingDlg::OnEIDUserOffline)
	ON_BN_CLICKED(IDC_BUTTON_JOINCHANNEL, &CAgoraRtmpStreamingDlg::OnBnClickedButtonJoinchannel)
	ON_BN_CLICKED(IDC_BUTTON_ADDSTREAM, &CAgoraRtmpStreamingDlg::OnBnClickedButtonAddstream)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE_STREAM, &CAgoraRtmpStreamingDlg::OnBnClickedButtonRemoveStream)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE_ALLSTREAM, &CAgoraRtmpStreamingDlg::OnBnClickedButtonRemoveAllstream)
	ON_LBN_SELCHANGE(IDC_LIST_INFO_BROADCASTING, &CAgoraRtmpStreamingDlg::OnSelchangeListInfoBroadcasting)
	ON_MESSAGE(WM_MSGID(EID_RTMP_STREAM_EVENT), &CAgoraRtmpStreamingDlg::OnEIDRtmpEvent)
	ON_WM_TIMER()
END_MESSAGE_MAP()


//Initialize the Agora SDK
bool CAgoraRtmpStreamingDlg::InitAgora()
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
	m_rtcEngine->setClientRole(CLIENT_ROLE_BROADCASTER);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("setClientRole broadcaster"));
	m_btnJoinChannel.EnableWindow(TRUE);
	return true;
}
//UnInitialize the Agora SDK
void CAgoraRtmpStreamingDlg::UnInitAgora()
{
	if (m_rtcEngine) {
		if (m_joinChannle)
			m_rtcEngine->leaveChannel();
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
void CAgoraRtmpStreamingDlg::RenderLocalVideo()
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

BOOL CAgoraRtmpStreamingDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_localVideoWnd.Create(NULL, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CRect(0, 0, 1, 1), this, ID_BASEWND_VIDEO + 100);
	RECT rcArea;
	m_staVideoArea.GetClientRect(&rcArea);
	m_localVideoWnd.MoveWindow(&rcArea);
	m_localVideoWnd.ShowWindow(SW_SHOW);
	ResumeStatus();
	return TRUE;
}

//set control text from config.
void CAgoraRtmpStreamingDlg::InitCtrlText()
{
	m_chkTransCoding.SetWindowText(rtmpStreamingCtrlTransCoding);
	m_staRemoveUrl.SetWindowText(rtmpStreamingCtrlPublishUrl);
	m_staPublishUrl.SetWindowText(rtmpStreamingCtrlPublishUrl);
	m_btnAddStream.SetWindowText(rtmpStreamingCtrlAdd);
	m_staChannelName.SetWindowText(commonCtrlChannel);
	m_btnJoinChannel.SetWindowText(commonCtrlJoinChannel);
	m_btnRemoveStream.SetWindowText(rtmpStreamingCtrlRemove);
	m_btnRemoveAll.SetWindowText(rtmpStreamingCtrlRemoveAll);
}

void CAgoraRtmpStreamingDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);
	if (bShow) {
		InitCtrlText();
		RenderLocalVideo();
	}
	else {
		ResumeStatus();
	}
}

//remove all rtmp stream in the engine.
void CAgoraRtmpStreamingDlg::RemoveAllRtmpUrls()
{
	m_bRemoveAll = true;
	CString strUrl;
	//remove all publish stream in the engine.
	for (int i = 0; i < m_cmbRtmpUrl.GetCount(); ++i) {
		m_cmbRtmpUrl.GetLBText(i, strUrl);
		std::string szUrl = cs2utf8(strUrl);
		m_rtcEngine->stopRtmpStream(szUrl.c_str());
	}
	m_cmbRtmpUrl.Clear();
	m_cmbRtmpUrl.ResetContent();
}

void CAgoraRtmpStreamingDlg::ResumeStatus()
{
	m_lstInfo.ResetContent();
	m_btnAddStream.EnableWindow(FALSE);
	InitCtrlText();
	m_joinChannle = false;
	m_initialize = false;
	m_urlSet.clear();
	m_removeUrlCount = 0;
	m_bRemoveAll = false;
	m_edtRtmpUrl.SetWindowText(_T(""));
	m_chkTransCoding.SetCheck(0);
	m_edtChannelName.SetWindowText(_T(""));
	m_staDetail.SetWindowText(_T(""));

}

//join or leave channel button handler.
void CAgoraRtmpStreamingDlg::OnBnClickedButtonJoinchannel()
{
	if (!m_rtcEngine || !m_initialize)
		return;

	if (!m_joinChannle) {
		CString strChannelName;
		m_edtChannelName.GetWindowText(strChannelName);
		if (strChannelName.IsEmpty()) {
			AfxMessageBox(_T("Fill channel name first"));
			return;
		}

		std::string szChannelId = cs2utf8(strChannelName);
		//join channel in the engine.
		if (0 == m_rtcEngine->joinChannel(APP_TOKEN, szChannelId.c_str(), "", 0)) {
			m_btnJoinChannel.EnableWindow(FALSE);
		}
	}
	else {
		if (m_cmbRtmpUrl.GetCount() > 0) {
			m_btnAddStream.EnableWindow(FALSE);
			m_btnRemoveStream.EnableWindow(FALSE);
			m_btnJoinChannel.EnableWindow(FALSE);
			//remove all rtmp streams.
			RemoveAllRtmpUrls();
		}
		//leave channel in the engine.
		else  if (0 == m_rtcEngine->leaveChannel()) {
			m_btnJoinChannel.EnableWindow(FALSE);
		}
	}
}

// add stream button handler.
void CAgoraRtmpStreamingDlg::OnBnClickedButtonAddstream()
{
	if (!m_rtcEngine || !m_initialize)
		return;

	CString strURL;
	m_edtRtmpUrl.GetWindowText(strURL);
	if (strURL.IsEmpty()) {
		AfxMessageBox(_T("Fill Publish URL first"));
		return;
	}
	if (m_urlSet.find(strURL) != m_urlSet.end()) {
		AfxMessageBox(_T("have add Publish Stream URL"));
		return;
	}
	std::string szURL = cs2utf8(strURL);
	BOOL isTransCoding = m_chkTransCoding.GetCheck();
	// add publish stream in the engine.
	int ret;

	if (isTransCoding) {
		ret = m_rtcEngine->startRtmpStreamWithTranscoding(szURL.c_str(), m_liveTransCoding);
	}
	else {
		ret = m_rtcEngine->startRtmpStreamWithoutTranscoding(szURL.c_str());
	}

	if (ret != 0) {
		CString strInfo;
		strInfo.Format(_T("addPublishStreamUrl failed:%d"), ret);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
		return;
	}
}


//remove stream button handler.
void CAgoraRtmpStreamingDlg::OnBnClickedButtonRemoveStream()
{
	if (!m_rtcEngine || !m_initialize)
		return;

	if (m_cmbRtmpUrl.GetCount() == 0)
		return;

	CString strUrl;
	m_cmbRtmpUrl.GetWindowText(strUrl);
	std::string szUrl = cs2utf8(strUrl);
	//remove publish stream in the engine.
	m_rtcEngine->stopRtmpStream(szUrl.c_str());
}

//remove all streams in the engine.
void CAgoraRtmpStreamingDlg::OnBnClickedButtonRemoveAllstream()
{
	if (m_cmbRtmpUrl.GetCount() == 0 && m_cmbRtmpUrl.GetCurSel() >= 0) {
		return;
	}
	if (!m_rtcEngine || !m_initialize)
		return;

	CString strUrl;
	m_cmbRtmpUrl.GetWindowText(strUrl);

	std::string szUrl = cs2utf8(strUrl);
	//remove public stream in the engine.
	m_rtcEngine->stopRtmpStream(szUrl.c_str());
	m_btnRemoveStream.EnableWindow(FALSE);
}

//EID_JOINCHANNEL_SUCCESS message window handler.
LRESULT CAgoraRtmpStreamingDlg::OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam)
{
	m_btnJoinChannel.EnableWindow(TRUE);
	m_btnAddStream.EnableWindow(TRUE);
	m_btnRemoveStream.EnableWindow(TRUE);
	m_joinChannle = true;
	m_btnJoinChannel.SetWindowText(commonCtrlLeaveChannel);
	CString strInfo;
	strInfo.Format(_T("%s:join success, uid=%u"), getCurrentTime(), wParam);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

	m_btnAddStream.EnableWindow(TRUE);
	::PostMessage(GetParent()->GetSafeHwnd(), WM_MSGID(EID_JOINCHANNEL_SUCCESS), TRUE, 0);
	return 0;
}


//Change liveTranscoding when users joined
LRESULT CAgoraRtmpStreamingDlg::OnEIDUserJoined(WPARAM wParam, LPARAM lParam)
{
	TranscodingUser tanrsCodingUser;
	auto p = new TranscodingUser[++m_liveTransCoding.userCount];
	if (m_liveTransCoding.userCount != 1)
	{
		memcpy(p, m_liveTransCoding.transcodingUsers, sizeof(TranscodingUser)*m_liveTransCoding.userCount++);
		free(m_liveTransCoding.transcodingUsers);
	}
	tanrsCodingUser.uid = wParam;
	tanrsCodingUser.alpha = 1;
	tanrsCodingUser.y = 0;
	tanrsCodingUser.height = m_liveTransCoding.height;
	tanrsCodingUser.width = m_liveTransCoding.width / m_liveTransCoding.userCount;
	p[m_liveTransCoding.userCount - 1] = tanrsCodingUser;
	for (size_t i = 0; i < m_liveTransCoding.userCount; i++)
	{
		p[i].x = tanrsCodingUser.width;
	}
	//add user info to TranscodingUsers.
	m_liveTransCoding.transcodingUsers = p;
	//set current live trans coding.
	m_rtcEngine->updateRtmpTranscoding(m_liveTransCoding);
	return TRUE;
}


//Change liveTranscoding when users leave
LRESULT CAgoraRtmpStreamingDlg::OnEIDUserOffline(WPARAM wParam, LPARAM lParam)
{
	for (size_t i = 0; i < m_liveTransCoding.userCount; i++)
	{
		if (m_liveTransCoding.transcodingUsers[i].uid == wParam)
		{
			for (size_t j = i; j < m_liveTransCoding.userCount; j++)
			{
				m_liveTransCoding.transcodingUsers[j] = m_liveTransCoding.transcodingUsers[j + 1];
			}
		}
	}
	m_liveTransCoding.userCount--;
	int width = m_liveTransCoding.width / m_liveTransCoding.userCount;
	for (size_t i = 0; i < m_liveTransCoding.userCount; i++)
	{
		m_liveTransCoding.transcodingUsers[i].x = width * i;
		m_liveTransCoding.transcodingUsers[i].width = width;
	}
	//set current live trans coding.
	m_rtcEngine->updateRtmpTranscoding(m_liveTransCoding);
	return TRUE;
}

//EID_LEAVE_CHANNEL message window handler.
LRESULT CAgoraRtmpStreamingDlg::OnEIDLeaveChannel(WPARAM wParam, LPARAM lParam)
{
	m_btnJoinChannel.EnableWindow(TRUE);
	m_joinChannle = false;
	m_btnJoinChannel.SetWindowText(commonCtrlJoinChannel);
	CString strInfo;
	strInfo.Format(_T("leave channel success"));
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	m_btnRemoveStream.EnableWindow(FALSE);
	m_btnAddStream.EnableWindow(TRUE);
	::PostMessage(GetParent()->GetSafeHwnd(), WM_MSGID(EID_JOINCHANNEL_SUCCESS), FALSE, 0);
	return 0;
}
//EID_INJECT_STATUS message window handler.
LRESULT CAgoraRtmpStreamingDlg::OnEIDRtmpStateChanged(WPARAM wParam, LPARAM lParam)
{
	PRtmpStreamStreamStateChanged rtmpState = (PRtmpStreamStreamStateChanged)wParam;
	CString strInfo;
	m_btnRemoveStream.EnableWindow(TRUE);
	switch (rtmpState->state)
	{
	case RTMP_STREAM_PUBLISH_STATE_IDLE:
	{
		strInfo.Format(_T("%s:%S��"), agoraRtmpStateIdle, rtmpState->url);
		CString strUrl;
		strUrl.Format(_T("%S"), rtmpState->url);
		int sel = m_cmbRtmpUrl.GetCurSel();
		m_cmbRtmpUrl.DeleteString(sel);
		m_cmbRtmpUrl.ResetContent();
		if (m_cmbRtmpUrl.GetCount() > 0) {
			m_cmbRtmpUrl.SetCurSel(0);
		}
		for (auto iter = m_urlSet.begin(); iter != m_urlSet.end(); ++iter)
			if (strUrl.Compare(*iter) == 0) {
				m_urlSet.erase(iter);
				break;
			}
		if (m_bRemoveAll) {
			m_removeUrlCount++;
			if (m_removeUrlCount == m_urlSet.size()) {//remove all url when leave channel
				m_urlSet.clear();
				m_bRemoveAll = false;
				m_rtcEngine->leaveChannel();
				m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("leaveChannel"));
			}
		}

	}
	break;
	case RTMP_STREAM_PUBLISH_STATE_CONNECTING:
	{
		strInfo = agoraRtmpStateConnecting;
	}
	break;
	case RTMP_STREAM_PUBLISH_STATE_RUNNING:
		strInfo = agoraRtmpStateRunning;
		if (rtmpState->error == RTMP_STREAM_PUBLISH_ERROR_OK) {
			strInfo = agoraRtmpStateRunningSuccess;
			CString  strUrl;
			strUrl.Format(_T("%S"), rtmpState->url);
			if (m_urlSet.find(strUrl) == m_urlSet.end()) {
				m_cmbRtmpUrl.AddString(strUrl);
				m_urlSet.insert(strUrl);
				if (m_cmbRtmpUrl.GetCurSel() < 0)
					m_cmbRtmpUrl.SetCurSel(0);
			}

		}
		break;
	case RTMP_STREAM_PUBLISH_STATE_RECOVERING:
		strInfo.Format(agoraRtmpStateRecovering);
		break;
	case RTMP_STREAM_PUBLISH_STATE_FAILURE:
	{
		strInfo = agoraRtmpStateRunningSuccess;
		CString  strUrl;
		strUrl.Format(_T("%S"), rtmpState->url);
		std::string szUrl = cs2utf8(strUrl);
		m_rtcEngine->stopRtmpStream(szUrl.c_str());

		int error = lParam;
		if (error == RTMP_STREAM_PUBLISH_ERROR_CONNECTION_TIMEOUT
			|| error == RTMP_STREAM_PUBLISH_ERROR_INTERNAL_SERVER_ERROR
			|| error == RTMP_STREAM_PUBLISH_ERROR_STREAM_NOT_FOUND
			|| error == RTMP_STREAM_PUBLISH_ERROR_RTMP_SERVER_ERROR
			|| error == RTMP_STREAM_PUBLISH_ERROR_NET_DOWN) {
			if (m_mapRepublishFlag.find(szUrl.c_str()) != m_mapRepublishFlag.end()
				&& m_mapRemoveFlag.find(szUrl.c_str()) != m_mapRemoveFlag.end()) {
				if (m_mapRepublishFlag[szUrl.c_str()]
					&& !m_mapRemoveFlag[szUrl.c_str()]) {
					//republish, removePublish when error
					m_rtcEngine->startRtmpStreamWithoutTranscoding(szUrl.c_str());
				}
			}
		}
		else {
		    // stop retrying
			m_mapRemoveFlag[szUrl.c_str()] = false;
			m_mapRepublishFlag[szUrl.c_str()] = true;
			CString  strUrl;
			strUrl.Format(_T("%S"), szUrl.c_str());
			for (int i = 0; i < m_cmbRtmpUrl.GetCount(); ++i) {
				CString strText;
				m_cmbRtmpUrl.GetLBText(i, strText);
				if (strText.Compare(strUrl) == 0) {
					m_cmbRtmpUrl.DeleteString(i);
					break;
				}
			}

			if (m_urlSet.find(strUrl) != m_urlSet.end()) {
				m_urlSet.erase(strUrl);
			}

			if (m_cmbRtmpUrl.GetCurSel() < 0 && m_cmbRtmpUrl.GetCount() > 0)
				m_cmbRtmpUrl.SetCurSel(0);
		}


		switch (rtmpState->state)
		{
		case RTMP_STREAM_PUBLISH_ERROR_INVALID_ARGUMENT:
		{
			strInfo = agoraRtmpStateInvalidArg;
		}
		break;
		case RTMP_STREAM_PUBLISH_ERROR_ENCRYPTED_STREAM_NOT_ALLOWED:
		{
			strInfo = agoraRtmpStateEncrypted;
		}
		break;
		case RTMP_STREAM_PUBLISH_ERROR_CONNECTION_TIMEOUT:
		{
			strInfo = agoraRtmpStateConnTimeout;
		}
		break;
		case RTMP_STREAM_PUBLISH_ERROR_INTERNAL_SERVER_ERROR:
		{
			strInfo = agoraRtmpStateInrealErr;
		}
		break;
		case RTMP_STREAM_PUBLISH_ERROR_RTMP_SERVER_ERROR:
		{
			strInfo = agoraRtmpStateServerErr;
		}
		break;
		case RTMP_STREAM_PUBLISH_ERROR_TOO_OFTEN:
		{
			strInfo = agoraRtmpStateTooOften;
		}
		break;
		case RTMP_STREAM_PUBLISH_ERROR_REACH_LIMIT:
		{
			strInfo = agoraRtmpStateReachLimit;
		}
		break;
		case RTMP_STREAM_PUBLISH_ERROR_NOT_AUTHORIZED:
		{
			strInfo = agoraRtmpStateNotAuth;
		}
		break;
		case RTMP_STREAM_PUBLISH_ERROR_STREAM_NOT_FOUND:
		{
			strInfo = agoraRtmpStateNotFound;
		}
		break;
		case RTMP_STREAM_PUBLISH_ERROR_FORMAT_NOT_SUPPORTED:
		{
			strInfo = agoraRtmpStateNotSupported;
		}
		break;
		default:
			break;
		}
	}
	break;
	default:
		break;
	}
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	delete[] rtmpState->url;
	rtmpState->url = NULL;
	delete[] rtmpState;
	rtmpState = NULL;
	return 0;
}

//show list information to m_staDetail.
void CAgoraRtmpStreamingDlg::OnSelchangeListInfoBroadcasting()
{
	int sel = m_lstInfo.GetCurSel();
	if (sel < 0)return;
	CString strDetail;
	m_lstInfo.GetText(sel, strDetail);
	m_staDetail.SetWindowText(strDetail);
}



BOOL CAgoraRtmpStreamingDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) {
		return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}

LRESULT CAgoraRtmpStreamingDlg::OnEIDRtmpEvent(WPARAM wParam, LPARAM lParam)
{
	PRtmpStreamEvent streamEvent = (PRtmpStreamEvent)wParam;

	if (streamEvent) {
		RTMP_STREAMING_EVENT eventCode = (RTMP_STREAMING_EVENT)streamEvent->eventCode;
		switch (eventCode)
		{
		case agora::rtc::RTMP_STREAMING_EVENT_FAILED_LOAD_IMAGE:
			m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("failed load image"));
			break;
		case agora::rtc::RTMP_STREAMING_EVENT_URL_ALREADY_IN_USE:
			m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("rtmp url in use, you need use new url"));
			break;
		default:
			break;
		}
		if (streamEvent->url) {
			delete[] streamEvent->url;
			streamEvent->url = nullptr;
		}
		delete streamEvent;
		streamEvent = nullptr;
	}

	return 0;
}


void CAgoraRtmpStreamingDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (m_mapTimerToRepublishCount.find(nIDEvent)
		!= m_mapTimerToRepublishCount.end()) {
		if (m_mapTimerToRepublishCount[nIDEvent] == 5) {
			KillTimer(nIDEvent);
			m_mapUrlToTimer.erase(m_mapTimerToUrl[nIDEvent]);
			m_mapTimerToRepublishCount.erase(nIDEvent);
			m_mapTimerToUrl.erase(nIDEvent);
			return;
		}

		m_mapTimerToRepublishCount[nIDEvent]++;
		m_rtcEngine->startRtmpStreamWithoutTranscoding(m_mapTimerToUrl[nIDEvent].c_str());
	}
}