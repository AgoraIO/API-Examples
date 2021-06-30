#include "stdafx.h"
#include "APIExample.h"
#include "CAgoraMultiChannelDlg.h"


IMPLEMENT_DYNAMIC(CAgoraMultiChannelDlg, CDialogEx)

CAgoraMultiChannelDlg::CAgoraMultiChannelDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_BEAUTY, pParent)
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
	DDX_Control(pDX, IDC_STATIC_CHANNEL_LIST, m_staChannelList);
	DDX_Control(pDX, IDC_COMBO_CHANNEL_LIST, m_cmbChannelList);
	DDX_Control(pDX, IDC_BUTTON_LEAVE_CHANNEL, m_btnLeaveChannel);
	DDX_Control(pDX, IDC_STATIC_DETAIL, m_staDetail);
	DDX_Control(pDX, IDC_CHECK_PUBLISH_AUDIO, m_chkPublishAudio);
	DDX_Control(pDX, IDC_CHECK_PUBLISH_VIDEO, m_chkPublishVideo);
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
	m_staChannelList.SetWindowText(MultiChannelCtrlChannelList);
	m_staChannel.SetWindowText(commonCtrlChannel);
	m_btnJoinChannel.SetWindowText(commonCtrlJoinChannel);
	m_btnLeaveChannel.SetWindowText(commonCtrlLeaveChannel);
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
	return true;
}


//UnInitialize the Agora SDK
void CAgoraMultiChannelDlg::UnInitAgora()
{
	if (m_rtcEngine) {
		for (auto &info : m_channels)
		{
			info.channel->release();
			delete info.evnetHandler;
		}
		m_channels.clear();
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


//resume window status
void CAgoraMultiChannelDlg::ResumeStatus()
{
	InitCtrlText();
	m_lstInfo.ResetContent();
	m_staDetail.SetWindowText(_T(""));
	m_edtChannel.SetWindowText(_T(""));
	m_cmbChannelList.ResetContent();
	for (auto &info:m_channels)
	{
		info.channel->release();
		delete info.evnetHandler;
	}
	m_channels.clear();
	m_joinChannel = false;
	m_initialize = false;
	m_audioMixing = false;

	m_chkPublishAudio.SetCheck(false);
	m_chkPublishVideo.SetCheck(false);
}



void CAgoraMultiChannelDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);
	if (bShow)//bShwo is true ,show window 
	{
		InitCtrlText();
		RenderLocalVideo();
	}
	else {
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
	m_localVideoWnd.ShowWindow(SW_SHOW);

	m_chkPublishAudio.SetWindowText(mediaPlayerCtrlPublishAudio);
	m_chkPublishVideo.SetWindowText(mediaPlayerCtrlPublishVideo);
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
	CString strTmp;
	for (int nIndex = 0; nIndex < m_cmbChannelList.GetCount(); nIndex++)
	{
		m_cmbChannelList.GetLBText(nIndex, strTmp);
		if (strTmp.Trim() == strChannelName)
		{
			AfxMessageBox(_T("you joined this channel!"));
			return;
		}
	}
	//create channel by channel id.
	IChannel * pChannel = static_cast<IRtcEngine2 *>(m_rtcEngine)->createChannel(szChannelId.c_str());
	//create channel event handler.
	ChannelEventHandler* pEvt = new ChannelEventHandler;
	//set message receiver window.
	pEvt->setMsgHandler(GetSafeHwnd());
	//add channels.
	m_channels.emplace_back(szChannelId, pChannel, pEvt);
	//set channel event handler.
	pChannel->setChannelEventHandler(pEvt);
	ChannelMediaOptions options;
	options.autoSubscribeAudio = true;
	options.autoSubscribeVideo = true;
	options.publishLocalAudio = m_chkPublishAudio.GetCheck();
	options.publishLocalVideo = m_chkPublishVideo.GetCheck();
	if (m_chkPublishAudio.GetCheck() || m_chkPublishVideo.GetCheck()) {
		pChannel->setClientRole(CLIENT_ROLE_BROADCASTER);
	}
	else {
		pChannel->setClientRole(CLIENT_ROLE_AUDIENCE);
	}
	//join channel
	if (0 == pChannel->joinChannel(APP_TOKEN, "", 0, options))
	{
		m_btnJoinChannel.EnableWindow(FALSE);
		m_cmbChannelList.InsertString(m_cmbChannelList.GetCount(), strChannelName);
		strInfo.Format(_T("join channel:%s ...."), strChannelName);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	}
}


void CAgoraMultiChannelDlg::OnBnClickedButtonLeaveChannel()
{
	CString strInfo;
	int nSel = m_cmbChannelList.GetCurSel();
	if (nSel < 0) {
		return;
	}
	CString strChannelName;
	m_cmbChannelList.GetWindowText(strChannelName);
	std::string szChannelName = cs2utf8(strChannelName);
	bool bFind = false;

	int i = 0;
	for (auto & channelInfo : m_channels)
	{
		if (channelInfo.channelName == szChannelName)
		{
			//leave other channel
			channelInfo.channel->leaveChannel();
			strInfo.Format(_T("leave channel %s"), strChannelName);
			bFind = true;
			break;
		}
		i++;
	}
	if (!bFind)
	{
		//leave main channel in the engine.
		if (0 == m_rtcEngine->leaveChannel()) {
			strInfo.Format(_T("leave channel %s"), strChannelName);
		}
	}
	m_cmbChannelList.DeleteString(nSel);
	m_cmbChannelList.SetCurSel(nSel - 1 < 0 ? 0 : nSel - 1);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
}


void CAgoraMultiChannelDlg::OnSelchangeListInfoBroadcasting()
{
	int sel = m_lstInfo.GetCurSel();
	if (sel < 0)return;
	CString strDetail;
	m_lstInfo.GetText(sel, strDetail);
	m_staDetail.SetWindowText(strDetail);
}


//EID_JOINCHANNEL_SUCCESS message window handler
LRESULT CAgoraMultiChannelDlg::OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam)
{
	IChannel* pChannel = (IChannel*)wParam;
	m_joinChannel = true;
	m_btnJoinChannel.EnableWindow(TRUE);
	CString strInfo;
	if (pChannel == 0)
	{
		strInfo.Format(_T("join :%s success, uid=:%u"), m_strMainChannel, lParam);
		m_localVideoWnd.SetUID(lParam);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	}
	else {
		for (auto & info:m_channels)
		{
			if (info.channel == pChannel)
			{
				strInfo.Format(_T("join :%s success, uid=:%u"),utf82cs(info.channelName), lParam);
				m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
			}
		}
	}
	return 0;
}

//EID_LEAVEHANNEL_SUCCESS message window handler
LRESULT CAgoraMultiChannelDlg::OnEIDLeaveChannel(WPARAM wParam, LPARAM lParam)
{
	IChannel* pChannel = (IChannel*)wParam;
	CString strInfo;
	if (pChannel == 0)
	{
		strInfo.Format(_T("leave %s channel success"), m_strMainChannel);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
		m_joinChannel = false;
	}
	else {
		int i = 0;
		for (auto & info:m_channels)
		{
			if (info.channel == pChannel)
			{
				strInfo.Format(_T("leave %s channel success"), utf82cs(info.channelName));
				m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
				info.channel->release();
				delete info.evnetHandler;
				m_channels.erase(m_channels.begin() + i);
				break;
			}
			i++;
		}
	}
	return 0;
}

//EID_USER_JOINED message window handler
LRESULT CAgoraMultiChannelDlg::OnEIDUserJoined(WPARAM wParam, LPARAM lParam)
{
	IChannel* pChannel = (IChannel*)wParam;
	CString strInfo;
	if (pChannel == 0)
	{
		strInfo.Format(_T("%u joined %s"), lParam, m_strMainChannel);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	}
	else {
		for (auto & info : m_channels)
		{
			if (info.channel == pChannel)
			{
				strInfo.Format(_T("%u joined %s"), lParam, utf82cs(info.channelName));
				m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
				break;
			}
		}
	}
	return 0;
}

//EID_USER_OFFLINE message handler.
LRESULT CAgoraMultiChannelDlg::OnEIDUserOffline(WPARAM wParam, LPARAM lParam)
{
	CString strInfo;
	IChannel* pChannel = (IChannel*)wParam;
	uid_t remoteUid = (uid_t)lParam;
	VideoCanvas canvas;
	canvas.uid = remoteUid;
	canvas.view = NULL;
	
	if (pChannel == 0)
	{
		strInfo.Format(_T("%u offline %s"), remoteUid, m_strMainChannel);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
		m_rtcEngine->setupRemoteVideo(canvas);
	}
	else {
		for (auto & info : m_channels)
		{
			if (info.channel == pChannel)
			{
				strInfo.Format(_T("%u offline %s"), remoteUid, utf82cs(info.channelName));
				m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
				break;
			}
		}
	}
	return 0;
}

//EID_REMOTE_VIDEO_STATE_CHANED message window handler.
LRESULT CAgoraMultiChannelDlg::OnEIDRemoteVideoStateChanged(WPARAM wParam, LPARAM lParam)
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
void CMultiChannelEventHandler::onJoinChannelSuccess(const char* channel, uid_t uid, int elapsed)
{
	if (m_hMsgHanlder) {
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_JOINCHANNEL_SUCCESS), (WPARAM)0, (LPARAM)uid);
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
void CMultiChannelEventHandler::onUserJoined(uid_t uid, int elapsed)
{
	if (m_hMsgHanlder) {
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_USER_JOINED), (WPARAM)0, (LPARAM)uid);
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
void CMultiChannelEventHandler::onUserOffline(uid_t uid, USER_OFFLINE_REASON_TYPE reason)
{
	if (m_hMsgHanlder) {
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_USER_OFFLINE), 0, (LPARAM)uid);
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

void CMultiChannelEventHandler::onLeaveChannel(const RtcStats& stats)
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
void CMultiChannelEventHandler::onRemoteVideoStateChanged(uid_t uid, REMOTE_VIDEO_STATE state, REMOTE_VIDEO_STATE_REASON reason, int elapsed)
{
	if (m_hMsgHanlder) {
		PVideoStateStateChanged stateChanged = new VideoStateStateChanged;
		stateChanged->uid = uid;
		stateChanged->reason = reason;
		stateChanged->state = state;
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_REMOTE_VIDEO_STATE_CHANED), (WPARAM)stateChanged, 0);
	}
}


