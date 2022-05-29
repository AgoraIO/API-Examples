#include "stdafx.h"
#include "APIExample.h"
#include "CAgoraMediaPlayer.h"


IMPLEMENT_DYNAMIC(CAgoraMediaPlayer, CDialogEx)

CAgoraMediaPlayer::CAgoraMediaPlayer(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_MEDIA_PLAYER, pParent)
{

}

CAgoraMediaPlayer::~CAgoraMediaPlayer()
{
}

void CAgoraMediaPlayer::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_VIDEO, m_staVideoArea);
	DDX_Control(pDX, IDC_LIST_INFO_BROADCASTING, m_lstInfo);
	DDX_Control(pDX, IDC_STATIC_DETAIL, m_staDetail);
	DDX_Control(pDX, IDC_STATIC_CHANNELNAME, m_staChannel);
	DDX_Control(pDX, IDC_EDIT_CHANNELNAME, m_edtChannel);
	DDX_Control(pDX, IDC_BUTTON_JOINCHANNEL, m_btnJoinChannel);
	DDX_Control(pDX, IDC_STATIC_VIDEO_SOURCE, m_staVideoSource);
	DDX_Control(pDX, IDC_EDIT_VIDEO_SOURCE, m_edtVideoSource);
	DDX_Control(pDX, IDC_BUTTON_OPEN, m_btnOpen);
	DDX_Control(pDX, IDC_BUTTON_STOP, m_btnStop);
	DDX_Control(pDX, IDC_BUTTON_PLAY, m_btnPlay);

	DDX_Control(pDX, IDC_BUTTON_PUBLISH_VIDEO, m_btnPublishVideo);
	
	DDX_Control(pDX, IDC_SLIDER_VIDEO, m_sldVideo);
}


//Initialize the Ctrl Text.
void CAgoraMediaPlayer::InitCtrlText()
{
	m_staVideoSource.SetWindowText(mediaPlayerCtrlVideoSource);
	m_btnPlay.SetWindowText(mediaPlayerCtrlPlay);
	m_btnOpen.SetWindowText(mediaPlayerCtrlOpen);
	m_btnStop.SetWindowText(mediaPlayerCtrlClose);
	m_btnPublishVideo.SetWindowText(mediaPlayerCtrlPublishVideo);
	m_staChannel.SetWindowText(commonCtrlChannel);
	m_btnJoinChannel.SetWindowText(commonCtrlJoinChannel);
}

//Initialize media player.
void CAgoraMediaPlayer::InitMediaPlayerKit()
{
	//create agora media player.
	m_mediaPlayer = m_rtcEngine->createMediaPlayer().get();//createAgoraMediaPlayer();
	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("createAgoraMediaPlayer"));
	//agora::rtc::MediaPlayerContext context;
	//initialize media player context.
	//agora::base::IAgoraService* agoraService;
	//int ret = m_mediaPlayer->initialize(agoraService);
	//set message notify receiver window
	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("mediaplayer initialize"));
	//set show window handle.
	RECT rc = { 0 };
	m_localVideoWnd.GetWindowRect(&rc);
	int ret = m_mediaPlayer->setView((agora::media::base::view_t)m_localVideoWnd.GetSafeHwnd());
	//set message notify receiver window
	m_mediaPlayerEvent.SetMsgReceiver(m_hWnd);
	//register player event observer.
	ret = m_mediaPlayer->registerPlayerSourceObserver(&m_mediaPlayerEvent);
}


//Uninitialized media player .
void CAgoraMediaPlayer::UnInitMediaPlayerKit()
{
	if (m_mediaPlayer)
	{
		//call media player release function.
		//m_mediaPlayer->release();
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("release mediaPlayer"));
		m_mediaPlayer = nullptr;
	}
}

//Initialize the Agora SDK
bool CAgoraMediaPlayer::InitAgora()
{
	//create Agora RTC engine
	m_rtcEngine = createAgoraRtcEngine();
	if (!m_rtcEngine) {
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("createAgoraRtcEngine failed"));
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
	VideoEncoderConfiguration config;
	config.dimensions = VideoDimensions(640, 360);
	config.bitrate = 1200;
	config.frameRate = FRAME_RATE_FPS_15;
	//set video encoder configuration to engine.
	//m_rtcEngine->setVideoEncoderConfiguration(config);
	//init media player kit.
	InitMediaPlayerKit();
	return true;
}


//UnInitialize the Agora SDK
void CAgoraMediaPlayer::UnInitAgora()
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
	//release media player kit.
	UnInitMediaPlayerKit();
}


//resume window status
void CAgoraMediaPlayer::ResumeStatus()
{
	InitCtrlText();
	m_staDetail.SetWindowText(_T(""));
	m_edtChannel.SetWindowText(_T(""));
	
	m_lstInfo.ResetContent();
	m_sldVideo.SetPos(0);

	m_btnPublishVideo.EnableWindow(FALSE);
	
	m_btnPlay.EnableWindow(FALSE);
	m_mediaPlayerState = mediaPLAYER_READY;
	m_joinChannel = false;
	m_initialize = false;
	m_attach = false;
}

BEGIN_MESSAGE_MAP(CAgoraMediaPlayer, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BUTTON_JOINCHANNEL, &CAgoraMediaPlayer::OnBnClickedButtonJoinchannel)
	ON_BN_CLICKED(IDC_BUTTON_OPEN, &CAgoraMediaPlayer::OnBnClickedButtonOpen)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CAgoraMediaPlayer::OnBnClickedButtonStop)
	ON_BN_CLICKED(IDC_BUTTON_PLAY, &CAgoraMediaPlayer::OnBnClickedButtonPlay)

	ON_BN_CLICKED(IDC_BUTTON_PUBLISH_VIDEO, &CAgoraMediaPlayer::OnBnClickedButtonPublishVideo)
	
	ON_LBN_SELCHANGE(IDC_LIST_INFO_BROADCASTING, &CAgoraMediaPlayer::OnSelchangeListInfoBroadcasting)

	ON_MESSAGE(WM_MSGID(mediaPLAYER_STATE_CHANGED), &CAgoraMediaPlayer::OnmediaPlayerStateChanged)
	ON_MESSAGE(WM_MSGID(mediaPLAYER_POSTION_CHANGED), &CAgoraMediaPlayer::OnmediaPlayerPositionChanged)

	ON_MESSAGE(WM_MSGID(EID_JOINCHANNEL_SUCCESS), &CAgoraMediaPlayer::OnEIDJoinChannelSuccess)
	ON_MESSAGE(WM_MSGID(EID_LEAVE_CHANNEL), &CAgoraMediaPlayer::OnEIDLeaveChannel)
	ON_MESSAGE(WM_MSGID(EID_USER_JOINED), &CAgoraMediaPlayer::OnEIDUserJoined)
	ON_MESSAGE(WM_MSGID(EID_USER_OFFLINE), &CAgoraMediaPlayer::OnEIDUserOffline)
	ON_MESSAGE(WM_MSGID(mediaPLAYER_EVENT), &CAgoraMediaPlayer::OnEIDPlayerEvent)
	
	ON_WM_DESTROY()
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_VIDEO, &CAgoraMediaPlayer::OnReleasedcaptureSliderVideo)
END_MESSAGE_MAP()


//WM_SHOWWINDOW	message handler.
void CAgoraMediaPlayer::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);
	if (bShow)//bShwo is true ,show window 
	{
		InitCtrlText();
	}
	else {
		ResumeStatus();
	}

}

//InitDialog handler.
BOOL CAgoraMediaPlayer::OnInitDialog()
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

//join channel handler.
void CAgoraMediaPlayer::OnBnClickedButtonJoinchannel()
{
	if (!m_rtcEngine || !m_initialize)
		return;
	int ret = -1;
	CString strInfo;
	if (!m_joinChannel) {
		CString strChannelName;
		m_edtChannel.GetWindowText(strChannelName);
		if (strChannelName.IsEmpty()) {
			AfxMessageBox(_T("Fill channel name first"));
			return;
		}
		std::string szChannelId = cs2utf8(strChannelName);
		//join channel in the engine.
		ChannelMediaOptions options;
		options.channelProfile = CHANNEL_PROFILE_LIVE_BROADCASTING;
		options.clientRoleType = CLIENT_ROLE_BROADCASTER;
		options.publishCameraTrack = true;
		options.publishAudioTrack = false;
		options.autoSubscribeAudio = false;
		options.autoSubscribeVideo = false;
		
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("registerPlayerSourceObserver"));
		if (0 == m_rtcEngine->joinChannel(APP_TOKEN, szChannelId.c_str(), 0, options)) {
			strInfo.Format(_T("join channel %s, use ChannelMediaOptions"), getCurrentTime());
			m_btnJoinChannel.EnableWindow(FALSE);
		}
	}
	else {
		//unregister player event observer.
		ret = m_mediaPlayer->unregisterPlayerSourceObserver(&m_mediaPlayerEvent);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("unregisterPlayerSourceObserver"));
		//leave channel in the engine.
		if (0 == m_rtcEngine->leaveChannel()) {
			strInfo.Format(_T("leave channel %s"), getCurrentTime());
		}
	}
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
}

//open button click handler.
void CAgoraMediaPlayer::OnBnClickedButtonOpen()
{
	CString strUrl;
	CString strInfo;
	m_edtVideoSource.GetWindowText(strUrl);
	std::string tmp = cs2utf8(strUrl);
	switch (m_mediaPlayerState)
	{
	case mediaPLAYER_READY:
	case mediaPLAYER_STOP:
		
		if (tmp.empty())
		{
			AfxMessageBox(_T("you can fill video source."));
			return;
		}
		//call media player open function
		m_mediaPlayer->open(tmp.c_str(), 0);
		break;
	default:
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("can not open player."));
		break;
	}
}

//stop button click handler. 
void CAgoraMediaPlayer::OnBnClickedButtonStop()
{
	if (m_mediaPlayerState == mediaPLAYER_OPEN ||
		m_mediaPlayerState == mediaPLAYER_PLAYING ||
		m_mediaPlayerState == mediaPLAYER_PAUSE)
	{
		//call media player stop function
		m_mediaPlayer->stop();
		m_mediaPlayerState = mediaPLAYER_STOP;
		m_btnPlay.SetWindowText(mediaPlayerCtrlPlay);
		m_btnPlay.EnableWindow(FALSE);
		//set slider current position.
		m_sldVideo.SetPos(0);
	}
	else {
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("can not stop player"));
	}
}

// play button click handler.
void CAgoraMediaPlayer::OnBnClickedButtonPlay()
{
	int ret;
	switch (m_mediaPlayerState)
	{
	case mediaPLAYER_PAUSE:
	case mediaPLAYER_OPEN:
		//call media player play function
		ret = m_mediaPlayer->play();
		if (ret == 0)
		{
			m_mediaPlayerState = mediaPLAYER_PLAYING;
			m_btnPlay.SetWindowText(mediaPlayerCtrlPause);
		}
		break;
	case mediaPLAYER_PLAYING:
		//call media player pause function
		ret = m_mediaPlayer->pause();
		if (ret == 0)
		{
			m_mediaPlayerState = mediaPLAYER_PAUSE;
			m_btnPlay.SetWindowText(mediaPlayerCtrlPlay);
		}
		break;
	default:
		break;
	}
}


//push video button click handler.
void CAgoraMediaPlayer::OnBnClickedButtonPublishVideo()
{
	if (m_publishMeidaplayer) {
		ChannelMediaOptions op;
		op.clientRoleType = CLIENT_ROLE_BROADCASTER;
		op.publishMediaPlayerVideoTrack = false;
		op.publishMediaPlayerAudioTrack = false;
		op.publishMediaPlayerId = m_mediaPlayer->getMediaPlayerId();
		int ret = m_rtcEngine->updateChannelMediaOptions(op);
		ChannelMediaOptions op2;
		op2.clientRoleType = CLIENT_ROLE_BROADCASTER;
		op2.publishCameraTrack = true;
		ret = m_rtcEngine->updateChannelMediaOptions(op2);
		m_publishMeidaplayer = false;
		m_btnPublishVideo.SetWindowText(mediaPlayerCtrlPublishVideo);
	}
	else {
		ChannelMediaOptions options;
		options.clientRoleType = CLIENT_ROLE_BROADCASTER;
		options.publishMediaPlayerVideoTrack = true;
		options.publishMediaPlayerAudioTrack = true;
		options.publishMediaPlayerId = m_mediaPlayer->getMediaPlayerId();
		options.publishCameraTrack = false;
		options.publishAudioTrack = false;
		options.autoSubscribeAudio = false;
		options.autoSubscribeVideo = false;
		m_rtcEngine->updateChannelMediaOptions(options);
		m_publishMeidaplayer = true;
		m_btnPublishVideo.SetWindowText(mediaPlayerCtrlUnPublishVideo);
	}
}


//show notify information
void CAgoraMediaPlayer::OnSelchangeListInfoBroadcasting()
{
	int sel = m_lstInfo.GetCurSel();
	if (sel < 0)return;
	CString strDetail;
	m_lstInfo.GetText(sel, strDetail);
	m_staDetail.SetWindowText(strDetail);
}

// intercept enter key
BOOL CAgoraMediaPlayer::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) {
		return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}


//media player state changed handler
LRESULT CAgoraMediaPlayer::OnmediaPlayerStateChanged(WPARAM wParam, LPARAM lParam)
{
	CString strState;
	CString strError;
	switch ((agora::media::base::MEDIA_PLAYER_STATE)wParam)
	{
	case  agora::media::base::PLAYER_STATE_OPEN_COMPLETED:
		strState = _T("PLAYER_STATE_OPEN_COMPLETED");
		m_mediaPlayerState = mediaPLAYER_OPEN;
		m_btnPlay.EnableWindow(TRUE);
		int64_t duration;
		m_mediaPlayer->getDuration(duration);
		m_sldVideo.SetRangeMax((int64_t)duration);

		break;
	case  agora::media::base::PLAYER_STATE_OPENING:
		strState = _T("PLAYER_STATE_OPENING");
		break;
	case  agora::media::base::PLAYER_STATE_IDLE:
		strState = _T("PLAYER_STATE_IDLE");
		break;
	case  agora::media::base::PLAYER_STATE_PLAYING:
		strState = _T("PLAYER_STATE_PLAYING");
		break;
	case agora::media::base::PLAYER_STATE_PLAYBACK_COMPLETED:
		strState = _T("PLAYER_STATE_PLAYBACK_COMPLETED");
		break;
	case agora::media::base::PLAYER_STATE_PAUSED:
		strState = _T("PLAYER_STATE_PAUSED");
		break;
	case agora::media::base::PLAYER_STATE_STOPPED:
		strState = _T("PLAYER_STATE_PAUSED");
		break;
	case agora::media::base::PLAYER_STATE_FAILED:
		strState = _T("PLAYER_STATE_FAILED");
		//call media player stop function
		m_mediaPlayer->stop();
		break;
	default:
		strState = _T("PLAYER_STATE_UNKNOWN");
		break;
	}

	switch ((agora::media::base::MEDIA_PLAYER_ERROR)lParam)
	{
	case agora::media::base::PLAYER_ERROR_URL_NOT_FOUND:
		strError = _T("PLAYER_ERROR_URL_NOT_FOUND");
		break;
	case agora::media::base::PLAYER_ERROR_NONE:
		strError = _T("PLAYER_ERROR_NONE");
		break;
	case agora::media::base::PLAYER_ERROR_CODEC_NOT_SUPPORTED:
		strError = _T("PLAYER_ERROR_NONE");
		break;
	case agora::media::base::PLAYER_ERROR_INVALID_ARGUMENTS:
		strError = _T("PLAYER_ERROR_INVALID_ARGUMENTS");
		break;
	case agora::media::base::PLAYER_ERROR_SRC_BUFFER_UNDERFLOW:
		strError = _T("PLAY_ERROR_SRC_BUFFER_UNDERFLOW");
		break;
	case agora::media::base::PLAYER_ERROR_INTERNAL:
		strError = _T("PLAYER_ERROR_INTERNAL");
		break;
	case agora::media::base::PLAYER_ERROR_INVALID_STATE:
		strError = _T("PLAYER_ERROR_INVALID_STATE");
		break;
	case agora::media::base::PLAYER_ERROR_NO_RESOURCE:
		strError = _T("PLAYER_ERROR_NO_RESOURCE");
		break;
	case agora::media::base::PLAYER_ERROR_OBJ_NOT_INITIALIZED:
		strError = _T("PLAYER_ERROR_OBJ_NOT_INITIALIZED");
		break;
	case agora::media::base::PLAYER_ERROR_INVALID_CONNECTION_STATE:
		strError = _T("PLAYER_ERROR_INVALID_CONNECTION_STATE");
		break;
	case agora::media::base::PLAYER_ERROR_UNKNOWN_STREAM_TYPE:
		strError = _T("PLAYER_ERROR_UNKNOWN_STREAM_TYPE");
		break;
	case agora::media::base::PLAYER_ERROR_VIDEO_RENDER_FAILED:
		strError = _T("PLAYER_ERROR_VIDEO_RENDER_FAILED");
		break;
	}
	CString strInfo;
	strInfo.Format(_T("sta:%s,\nerr:%s"), strState, strError);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	return TRUE;
}

LRESULT CAgoraMediaPlayer::OnmediaPlayerPositionChanged(WPARAM wParam, LPARAM lParam)
{
	int64_t * p = (int64_t*)wParam;
	m_sldVideo.SetPos((int)*p);
	delete p;
	return TRUE;
}



//EID_JOINCHANNEL_SUCCESS message window handler
LRESULT CAgoraMediaPlayer::OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam)
{
	m_joinChannel = true;
	m_btnJoinChannel.SetWindowText(commonCtrlLeaveChannel);
	m_btnJoinChannel.EnableWindow(TRUE);
	CString strInfo;
	strInfo.Format(_T("%s:join success, uid=%u"), getCurrentTime(), wParam);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	m_localVideoWnd.SetUID(wParam);
	
    m_btnPublishVideo.EnableWindow(TRUE);
	//notify parent window
	return 0;
}

//EID_LEAVEHANNEL_SUCCESS message window handler
LRESULT CAgoraMediaPlayer::OnEIDLeaveChannel(WPARAM wParam, LPARAM lParam)
{
	m_joinChannel = false;
	m_btnJoinChannel.SetWindowText(commonCtrlJoinChannel);
	CString strInfo;
	strInfo.Format(_T("leave channel success %s"), getCurrentTime());
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	::PostMessage(GetParent()->GetSafeHwnd(), WM_MSGID(EID_JOINCHANNEL_SUCCESS), FALSE, 0);
	return 0;
}

//EID_USER_JOINED message window handler
LRESULT CAgoraMediaPlayer::OnEIDUserJoined(WPARAM wParam, LPARAM lParam)
{
	CString strInfo;
	strInfo.Format(_T("%u joined"), wParam);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	return 0;
}

//EID_USER_OFFLINE message handler.
LRESULT CAgoraMediaPlayer::OnEIDUserOffline(WPARAM wParam, LPARAM lParam)
{
	uid_t remoteUid = (uid_t)wParam;
	CString strInfo;
	strInfo.Format(_T("%u offline, reason:%d"), remoteUid, lParam);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	return 0;
}

LRESULT CAgoraMediaPlayer::OnEIDPlayerEvent(WPARAM wParam, LPARAM lParam) {
	CString strInfo;
	agora::media::base::MEDIA_PLAYER_EVENT event = (agora::media::base::MEDIA_PLAYER_EVENT)wParam;
	switch (event) {
	case PLAYER_EVENT_SEEK_BEGIN:
		strInfo = _T("PLAYER_EVENT_SEEK_BEGIN");
		break;
	case PLAYER_EVENT_SEEK_COMPLETE:
		strInfo = _T("PLAYER_EVENT_SEEK_COMPLETE");
		break;
	case PLAYER_EVENT_SEEK_ERROR:
		strInfo = _T("PLAYER_EVENT_SEEK_ERROR");
		break;
	
	case PLAYER_EVENT_AUDIO_TRACK_CHANGED:
		strInfo = _T("PLAYER_EVENT_AUDIO_TRACK_CHANGED");
		break;
	case PLAYER_EVENT_BUFFER_LOW:
		strInfo = _T("PLAYER_EVENT_BUFFER_LOW");
		break;
	case PLAYER_EVENT_BUFFER_RECOVER:
		strInfo = _T("PLAYER_EVENT_BUFFER_RECOVER");
		break;
	case PLAYER_EVENT_FREEZE_START:
		strInfo = _T("PLAYER_EVENT_FREEZE_START");
		break;
	case PLAYER_EVENT_FREEZE_STOP:
		strInfo = _T("PLAYER_EVENT_FREEZE_STOP");
		break;
	}
	
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
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
void CAgoraMediaPlayerHandler::onJoinChannelSuccess(const char* channel, uid_t uid, int elapsed)
{
	if (m_hMsgHanlder) {
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_JOINCHANNEL_SUCCESS), (WPARAM)uid, (LPARAM)elapsed);
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
void CAgoraMediaPlayerHandler::onUserJoined(uid_t uid, int elapsed)
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
void CAgoraMediaPlayerHandler::onUserOffline(uid_t uid, USER_OFFLINE_REASON_TYPE reason)
{
	if (m_hMsgHanlder) {
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_USER_OFFLINE), (WPARAM)uid, (LPARAM)reason);
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

void CAgoraMediaPlayerHandler::onLeaveChannel(const RtcStats& stats)
{
	if (m_hMsgHanlder) {
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_LEAVE_CHANNEL), 0, 0);
	}
}



void CAgoraMediaPlayer::OnDestroy()
{
	CDialogEx::OnDestroy();
	UnInitMediaPlayerKit();
}


//drag events
void CAgoraMediaPlayer::OnReleasedcaptureSliderVideo(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	int pos = m_sldVideo.GetPos();

	int64_t playPos = 0;
	//m_mediaPlayer->getPlayPosition(playPos);
	m_mediaPlayer->seek(pos);
	*pResult = 0;
}
