#include "stdafx.h"
#include "APIExample.h"
#include "MultiVideoSourceTracks.h"
#include "afxdialogex.h"


IMPLEMENT_DYNAMIC(MultiVideoSourceTracks, CDialogEx)

MultiVideoSourceTracks::MultiVideoSourceTracks(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD, pParent)
{

}

MultiVideoSourceTracks::~MultiVideoSourceTracks()
{
}

void MultiVideoSourceTracks::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_VIDEO, m_videoArea);
	DDX_Control(pDX, IDC_BUTTON_JOINCHANNEL, m_btnJoinChannel);
	DDX_Control(pDX, IDC_LIST_INFO_BROADCASTING, m_lstInfo);
	DDX_Control(pDX, IDC_STATIC_CHANNELNAME, m_staChannel);
	DDX_Control(pDX, IDC_EDIT_CHANNELNAME, m_edtChannelName);
	DDX_Control(pDX, IDC_BUTTON_CREATE_TRACK, m_btnCreateTrack);
	DDX_Control(pDX, IDC_BUTTON_DESTROY_TRACK, m_btnDestroyTrack);
}


BEGIN_MESSAGE_MAP(MultiVideoSourceTracks, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BUTTON_JOINCHANNEL, &MultiVideoSourceTracks::OnBnClickedButtonJoinchannel)
	ON_MESSAGE(WM_MSGID(EID_JOINCHANNEL_SUCCESS), &MultiVideoSourceTracks::OnEIDJoinChannelSuccess)
	ON_MESSAGE(WM_MSGID(EID_LEAVE_CHANNEL), &MultiVideoSourceTracks::OnEIDLeaveChannel)
	ON_MESSAGE(WM_MSGID(EID_USER_JOINED), &MultiVideoSourceTracks::OnEIDUserJoined)
	ON_MESSAGE(WM_MSGID(EID_USER_OFFLINE), &MultiVideoSourceTracks::OnEIDUserOffline)
	ON_MESSAGE(WM_MSGID(EID_LOCAL_AUDIO_STATS), &MultiVideoSourceTracks::OnEIDLocalAudioStats)
	ON_MESSAGE(WM_MSGID(EID_LOCAL_VIDEO_STATS), &MultiVideoSourceTracks::OnEIDLocalVideoStats)
	ON_MESSAGE(WM_MSGID(EID_REMOTE_AUDIO_STATS), &MultiVideoSourceTracks::OnEIDRemoteAudioStats)
	ON_MESSAGE(WM_MSGID(EID_REMOTE_VIDEO_STATS), &MultiVideoSourceTracks::OnEIDRemoteVideoStats)
END_MESSAGE_MAP()


// MultiVideoSourceTracks


BOOL MultiVideoSourceTracks::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	InitCtrlText();
	// initialize render wnd
	for (int i = 0; i < VIDEO_WINDOWS_SIZE; ++i) {
		m_videoWnds[i].Create(NULL, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CRect(0, 0, 1, 1), this, ID_BASEWND_VIDEO + i);
		//set window background color.
		m_videoWnds[i].SetFaceColor(RGB(0x58, 0x58, 0x58));
		m_videoWnds[i].ShowWindow(SW_HIDE);
	}
	ShowVideoWnds();
	return TRUE;  // return TRUE unless you set the focus to a control
}


void MultiVideoSourceTracks::InitCtrlText()
{
	m_staChannel.SetWindowText(commonCtrlChannel);
	m_btnJoinChannel.SetWindowText(commonCtrlJoinChannel);

}


void MultiVideoSourceTracks::OnBnClickedButtonJoinchannel()
{
	CString strChannelName;
	m_edtChannelName.GetWindowText(strChannelName);
	std::string szChannelId = cs2utf8(strChannelName);
	if (!m_joinChannel) {
		if (strChannelName.IsEmpty()) {
			AfxMessageBox(_T("Fill channel name first"));
			return;
		}
		
		//joinchannelex option
		agora::rtc::ChannelMediaOptions optionsCamera;
		optionsCamera.autoSubscribeAudio = true;
		optionsCamera.autoSubscribeVideo = true;
		optionsCamera.clientRoleType = CLIENT_ROLE_AUDIENCE;

		// join channel first camera
		m_rtcEngine->joinChannel(APP_TOKEN, szChannelId.data(), 0, optionsCamera);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("joinChannel primary camera, use ChannelMediaOption"));

		m_btnJoinChannel.SetWindowText(commonCtrlLeaveChannel);
	}
	else {
		//leaveChannel primary camera
		m_rtcEngine->leaveChannel();
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("leaveChannel primary camera"));

		m_btnJoinChannel.SetWindowText(commonCtrlJoinChannel);
	}

	m_joinChannel = !m_joinChannel;
}

void MultiVideoSourceTracks::ShowVideoWnds()
{
	m_videoArea.ShowWindow(SW_HIDE);
	int row = 2;
	int col = 2;
	int videoCount = VIDEO_WINDOWS_SIZE;
	if (videoCount == 1) {
		row = 1;
		col = 1;
	}
	else  if (videoCount == 2) {
		row = 1;
		col = 2;
	}
	else  if (videoCount > 2 && videoCount <= 4) {
		row = 2;
		col = 2;
	}
	else  if (videoCount > 4 && videoCount <= 6) {
		row = 2;
		col = 3;
	}
	else  if (videoCount > 6 && videoCount <= 9) {
		row = 3;
		col = 3;
	}
	else  if (videoCount > 9 && videoCount <= 12) {
		row = 3;
		col = 4;
	}
	else  if (videoCount > 13 && videoCount <= 16) {
		row = 4;
		col = 4;
	}

	RECT rcArea;
	m_videoArea.GetClientRect(&rcArea);
	int space = 1;

	int w = (rcArea.right - rcArea.left - space * (col - 1)) / col;
	int h = (rcArea.bottom - rcArea.top - space * (row - 1)) / row;

	for (int r = 0; r < row; r++) {
		for (int c = 0; c < col; c++) {
			int x = rcArea.left + (w + space) * c;
			int y = rcArea.top + (h + space) * r;
			int nIndex = r * col + c;
			m_videoWnds[nIndex].MoveWindow(x, y, w, h, TRUE);
			m_videoWnds[nIndex].ShowWindow(SW_SHOW);
			m_videoWnds[nIndex].SetParent(this);

			if (!m_videoWnds[nIndex].IsWindowVisible()) {
				m_videoWnds[nIndex].ShowWindow(SW_SHOW);
			}
		}
	}

}


//resume window status
void MultiVideoSourceTracks::ResumeStatus()
{
	InitCtrlText();


	m_joinChannel = false;

	m_btnJoinChannel.EnableWindow(TRUE);
}

//Initialize the Agora SDK
bool MultiVideoSourceTracks::InitAgora()
{
	//create Agora RTC engine

	agora::rtc::IRtcEngine* engine = createAgoraRtcEngine();
	m_rtcEngine = (IRtcEngineEx*)engine;
	if (!m_rtcEngine) {
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("createAgoraRtcEngine failed"));
		return false;
	}
	m_eventHandler.SetId(0);
	m_eventHandler.SetMsgReceiver(m_hWnd);
	agora::rtc::RtcEngineContext context;
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

	return true;
}
//UnInitialize the Agora SDK
void MultiVideoSourceTracks::UnInitAgora()
{
	if (m_rtcEngine) {
		if (m_joinChannel) {
			//leave channel primary camera
			m_joinChannel = !m_rtcEngine->leaveChannel();
			m_joinChannel = false;
		}

		m_rtcEngine->stopPreview();
		//release engine.
		m_rtcEngine->release(true);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("release rtc engine"));
		m_rtcEngine = NULL;
	}
}


void MultiVideoSourceTracks::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);
	ResumeStatus();
}



LRESULT MultiVideoSourceTracks::OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam)
{
	int cId = (int)lParam;
	unsigned int uid = (unsigned int)wParam;

	m_btnJoinChannel.EnableWindow(TRUE);
	CString strInfo;
	strInfo.Format(_T("join %s success, uid=%u, cId=%d"), m_strChannel.c_str(), wParam, uid);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	

	return 0;
}

//EID_LEAVE_CHANNEL message window handler.
LRESULT MultiVideoSourceTracks::OnEIDLeaveChannel(WPARAM wParam, LPARAM lParam)
{

	int cId = (int)lParam;
	unsigned int uid = (unsigned int)lParam;
	m_btnJoinChannel.SetWindowText(commonCtrlJoinChannel);
	CString strInfo;
	strInfo.Format(_T("leave channel:%s "), m_strChannel.c_str());
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);


	return 0;
}

LRESULT MultiVideoSourceTracks::OnEIDUserJoined(WPARAM wParam, LPARAM lParam)
{
	int cId = wParam;
	int uid = lParam;
	if (cId == 0) {
		int idleVideoWndIndex = -1;
		for (int Index = 0; Index < sizeof(m_videoWnds) ; Index++)
		{
			if (m_videoWnds[Index].GetUID() == 0) {
				idleVideoWndIndex = Index;
				break;
			}
		}
		if (idleVideoWndIndex != -1) {
			m_videoWnds[idleVideoWndIndex].SetUID(uid);
			VideoCanvas canvas;
			canvas.uid = uid;
			canvas.view = m_videoWnds[idleVideoWndIndex].GetSafeHwnd();
			canvas.renderMode = agora::media::base::RENDER_MODE_HIDDEN;
			m_rtcEngine->setupRemoteVideo(canvas);
		}
	}
	

	return 0;
}


LRESULT MultiVideoSourceTracks::OnEIDUserOffline(WPARAM wParam, LPARAM lParam)
{
	int cId = wParam;
	int uid = lParam;
	if (cId == 0) {
		for (int Index = 0; Index < sizeof(m_videoWnds); Index++)
		{
			if (m_videoWnds[Index].GetUID() == uid) {
				m_videoWnds[Index].Reset();
				m_videoWnds[Index].SetUID(0);
				break;
			}
		}
	}
	
	return 0;
}


LRESULT MultiVideoSourceTracks::OnEIDLocalVideoStats(WPARAM wParam, LPARAM lParam)
{
	int cId = wParam;
	LocalVideoStats* stats = reinterpret_cast<LocalVideoStats*>(lParam);

	if (cId > 0)
	{
		if (m_videoWnds[cId - 1].GetUID() != 0) {
			m_videoWnds[cId - 1].SetVideoStatsInfo(stats->encodedFrameWidth, stats->encodedFrameHeight,
				stats->sentFrameRate, stats->sentBitrate, stats->txPacketLossRate);
		}
	}
	
	delete stats;
	return 0;
}


LRESULT MultiVideoSourceTracks::OnEIDLocalAudioStats(WPARAM wParam, LPARAM lParam)
{
	int cId = wParam;
	LocalAudioStats* stats = reinterpret_cast<LocalAudioStats*>(lParam);

	if (cId > 0)
	{
		if (m_videoWnds[cId - 1].GetUID() != 0) {
			m_videoWnds[cId - 1].SetAudioStatsInfo(stats->sentBitrate, stats->txPacketLossRate);
		}
	}

	delete stats;
	return 0;
}


LRESULT MultiVideoSourceTracks::OnEIDRemoteVideoStats(WPARAM wParam, LPARAM lParam)
{
	int cId = wParam;
	RemoteVideoStats* stats = reinterpret_cast<RemoteVideoStats*>(wParam);

	if (cId == 0)
	{
		if (m_videoWnds[cId - 1].GetUID() != 0) {
			m_videoWnds[cId - 1].SetVideoStatsInfo(stats->width, stats->height,
				stats->decoderOutputFrameRate, stats->receivedBitrate, stats->packetLossRate, stats->delay);
		}
	}

	return 0;
}


LRESULT MultiVideoSourceTracks::OnEIDRemoteAudioStats(WPARAM wParam, LPARAM lParam)
{
	int cId = wParam;
	RemoteAudioStats* stats = reinterpret_cast<RemoteAudioStats*>(lParam);

	if (cId > 0)
	{
		if (m_videoWnds[cId - 1].GetUID() != 0) {
			m_videoWnds[cId - 1].SetAudioStatsInfo(stats->receivedBitrate, stats->audioLossRate, stats->jitterBufferDelay);
		}
	}

	return 0;
}


// MultiVideoSourceTracksEventHandler implement

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
void MultiVideoSourceTracksEventHandler::onJoinChannelSuccess(const char* channel, agora::rtc::uid_t uid, int elapsed)
{
	m_strChannel = channel;
	if (m_hMsgHanlder) {
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_JOINCHANNEL_SUCCESS), (WPARAM)m_Id, (LPARAM)uid);
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
void MultiVideoSourceTracksEventHandler::onUserJoined(agora::rtc::uid_t uid, int elapsed)
{
	if (m_hMsgHanlder) {
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_USER_JOINED), (WPARAM)m_Id, (LPARAM)uid);
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
void MultiVideoSourceTracksEventHandler::onUserOffline(agora::rtc::uid_t uid, agora::rtc::USER_OFFLINE_REASON_TYPE reason)
{
	if (m_hMsgHanlder) {
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_USER_OFFLINE), (WPARAM)m_Id, (LPARAM)uid);
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
void MultiVideoSourceTracksEventHandler::onLeaveChannel(const agora::rtc::RtcStats& stats)
{
	if (m_hMsgHanlder) {
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_LEAVE_CHANNEL), m_Id, 0);
	}
}


void MultiVideoSourceTracksEventHandler::onRemoteVideoStateChanged(agora::rtc::uid_t uid, agora::rtc::REMOTE_VIDEO_STATE state, agora::rtc::REMOTE_VIDEO_STATE_REASON reason, int elapsed)
{
	if (m_hMsgHanlder) {
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_REMOTE_VIDEO_STATE_CHANGED), (WPARAM)m_Id, (LPARAM)uid);
	}
}



void MultiVideoSourceTracksEventHandler::onLocalVideoStats(VIDEO_SOURCE_TYPE source, const LocalVideoStats& stats)
{
	if (m_hMsgHanlder) {
		LocalVideoStats* _stats = new LocalVideoStats;
		*_stats = stats;
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_LOCAL_VIDEO_STATS), (WPARAM)m_Id, (LPARAM)_stats);
	}
}


void MultiVideoSourceTracksEventHandler::onLocalAudioStats(const LocalAudioStats& stats)
{
	if (m_hMsgHanlder) {
		LocalAudioStats* _stats = new LocalAudioStats;
		*_stats = stats;
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_LOCAL_AUDIO_STATS), (WPARAM)m_Id, (LPARAM)_stats);
	}
}


void MultiVideoSourceTracksEventHandler::onRemoteAudioStats(const RemoteAudioStats& stats)
{
	if (m_hMsgHanlder) {
		RemoteAudioStats* _stats = new RemoteAudioStats;
		*_stats = stats;
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_REMOTE_AUDIO_STATS), (WPARAM)m_Id, (LPARAM)_stats);
	}
}


void MultiVideoSourceTracksEventHandler::onRemoteVideoStats(const RemoteVideoStats& stats)
{
	if (m_hMsgHanlder) {
		RemoteVideoStats* _stats = new RemoteVideoStats;
		*_stats = stats;
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_REMOTE_VIDEO_STATS), (WPARAM)m_Id, (LPARAM)_stats);
	}
}
