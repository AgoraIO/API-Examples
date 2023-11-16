#include "stdafx.h"
#include "APIExample.h"
#include "CAgoraAudioMixingDlg.h"



IMPLEMENT_DYNAMIC(CAgoraAudioMixingDlg, CDialogEx)

CAgoraAudioMixingDlg::CAgoraAudioMixingDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_AUDIO_MIX, pParent)
{

}

CAgoraAudioMixingDlg::~CAgoraAudioMixingDlg()
{
}


//Initialize the Ctrl Text.
void CAgoraAudioMixingDlg::InitCtrlText()
{
	m_staChannel.SetWindowText(commonCtrlChannel);
	m_btnJoinChannel.SetWindowText(commonCtrlJoinChannel);
	m_btnMixingStart.SetWindowText(audioMixingCtrlPlay);
	m_btnMixingResume.SetWindowText(audioMixingCtrlResume);
	m_btnMixingPause.SetWindowText(audioMixingCtrlPause);
	m_btnMixingStop.SetWindowText(audioMixingCtrlStop);

	m_btnEffectStart.SetWindowText(audioMixingCtrlPlay);
	m_btnEffectResume.SetWindowText(audioMixingCtrlResume);
	m_btnEffectPause.SetWindowText(audioMixingCtrlPause);
	m_btnEffectStop.SetWindowText(audioMixingCtrlStop);

	m_staMixingTitle.SetWindowTextW(audioMixingCtrlMixing);
	m_staMixingVolume.SetWindowTextW(audioMixingCtrlMixingVolume);
	m_staMixingPlayoutVolume.SetWindowTextW(audioMixingCtrlMixingPlayoutVolume);
	m_staMixingPublishVolume.SetWindowTextW(audioMixingCtrlMixingPublishVolume);

	m_staEffectTitle.SetWindowTextW(audioMixingCtrlEffect);
	m_staEffectVolume.SetWindowTextW(audioMixingCtrlEffectVolume);
}



//Initialize the Agora SDK
bool CAgoraAudioMixingDlg::InitAgora()
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
	return true;
}


//UnInitialize the Agora SDK
void CAgoraAudioMixingDlg::UnInitAgora()
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
		if (m_initialize) {
			m_rtcEngine->release(true);
			m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("release rtc engine"));
		}
		m_rtcEngine = NULL;
	}
}

//render local video from SDK local capture.
void CAgoraAudioMixingDlg::RenderLocalVideo()
{
	if (m_rtcEngine) {
		//start preview in the engine.
		m_rtcEngine->startPreview();
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("startPreview"));
		VideoCanvas canvas;
		canvas.renderMode = media::base::RENDER_MODE_FIT;
		canvas.uid = 0;
		canvas.view = m_localVideoWnd.GetSafeHwnd();
		//setup local video in the engine to canvas.
		m_rtcEngine->setupLocalVideo(canvas);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("setupLocalVideo"));
	}
}


//resume window status
void CAgoraAudioMixingDlg::ResumeStatus()
{
	InitCtrlText();
	m_lstInfo.ResetContent();
	m_staDetail.SetWindowText(_T(""));
	m_edtChannel.SetWindowText(_T(""));

	m_joinChannel = false;
	m_initialize = false;
	m_audioMixing = false;
}


void CAgoraAudioMixingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_VIDEO, m_staVideoArea);
	DDX_Control(pDX, IDC_LIST_INFO_BROADCASTING, m_lstInfo);
	DDX_Control(pDX, IDC_STATIC_DETAIL, m_staDetail);
	DDX_Control(pDX, IDC_STATIC_CHANNELNAME, m_staChannel);
	DDX_Control(pDX, IDC_EDIT_CHANNELNAME, m_edtChannel);
	DDX_Control(pDX, IDC_BUTTON_JOINCHANNEL, m_btnJoinChannel);

	DDX_Control(pDX, IDC_STATIC_MIXING_TITLE, m_staMixingTitle);
	DDX_Control(pDX, IDC_STATIC_MIXING_VOLUME, m_staMixingVolume);
	DDX_Control(pDX, IDC_STATIC_MIXING_PLAYOUT_VOLUME, m_staMixingPlayoutVolume);
	DDX_Control(pDX, IDC_STATIC_MIXING_PUBLISH_VOLUME, m_staMixingPublishVolume);
	DDX_Control(pDX, IDC_BUTTON_MIXING_START, m_btnMixingStart);
	DDX_Control(pDX, IDC_BUTTON_MIXING_RESUME, m_btnMixingResume);
	DDX_Control(pDX, IDC_BUTTON_MIXING_PAUSE, m_btnMixingPause);
	DDX_Control(pDX, IDC_BUTTON_MIXING_STOP, m_btnMixingStop);
	DDX_Control(pDX, IDC_SLIDER_MIXING_VOLUME, m_sldMixingVolume);
	DDX_Control(pDX, IDC_SLIDER_MIXING_PLAYOUT_VOLUME, m_sldMixingPlayoutVolume);
	DDX_Control(pDX, IDC_SLIDER_MIXING_PUBLISH_VOLUME, m_sldMixingPublishVolume);

	DDX_Control(pDX, IDC_STATIC_EFFECT_TITLE, m_staEffectTitle);
	DDX_Control(pDX, IDC_STATIC_EFFECT_VOLUME, m_staEffectVolume);
	DDX_Control(pDX, IDC_BUTTON_EFFECT_START, m_btnEffectStart);
	DDX_Control(pDX, IDC_BUTTON_EFFECT_RESUME, m_btnEffectResume);
	DDX_Control(pDX, IDC_BUTTON_EFFECT_PAUSE, m_btnEffectPause);
	DDX_Control(pDX, IDC_BUTTON_EFFECT_STOP, m_btnEffectStop);
	DDX_Control(pDX, IDC_SLIDER_EFFECT_VOLUME, m_sldEffectVolume);
}


BEGIN_MESSAGE_MAP(CAgoraAudioMixingDlg, CDialogEx)
	ON_LBN_SELCHANGE(IDC_LIST_INFO_BROADCASTING, &CAgoraAudioMixingDlg::OnSelchangeListInfoBroadcasting)
	ON_WM_SHOWWINDOW()
	ON_MESSAGE(WM_MSGID(EID_JOINCHANNEL_SUCCESS), &CAgoraAudioMixingDlg::OnEIDJoinChannelSuccess)
	ON_MESSAGE(WM_MSGID(EID_LEAVE_CHANNEL), &CAgoraAudioMixingDlg::OnEIDLeaveChannel)
	ON_MESSAGE(WM_MSGID(EID_USER_JOINED), &CAgoraAudioMixingDlg::OnEIDUserJoined)
	ON_MESSAGE(WM_MSGID(EID_USER_OFFLINE), &CAgoraAudioMixingDlg::OnEIDUserOffline)
	ON_MESSAGE(WM_MSGID(EID_REMOTE_AUDIO_MIXING_STATE_CHANED), &CAgoraAudioMixingDlg::OnEIDAudioMixingStateChanged)
	ON_BN_CLICKED(IDC_BUTTON_JOINCHANNEL, &CAgoraAudioMixingDlg::OnBnClickedButtonJoinchannel)


	ON_BN_CLICKED(IDC_BUTTON_MIXING_START, &CAgoraAudioMixingDlg::OnBnClickedButtonMixingStart)
	ON_BN_CLICKED(IDC_BUTTON_MIXING_RESUME, &CAgoraAudioMixingDlg::OnBnClickedButtonMixingResume)
	ON_BN_CLICKED(IDC_BUTTON_MIXING_PAUSE, &CAgoraAudioMixingDlg::OnBnClickedButtonMixingPause)
	ON_BN_CLICKED(IDC_BUTTON_MIXING_STOP, &CAgoraAudioMixingDlg::OnBnClickedButtonMixingStop)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_MIXING_VOLUME, &CAgoraAudioMixingDlg::OnNMCustomdrawSliderMixingVolume)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_MIXING_PLAYOUT_VOLUME, &CAgoraAudioMixingDlg::OnNMCustomdrawSliderMixingPlayoutVolume)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_MIXING_PUBLISH_VOLUME, &CAgoraAudioMixingDlg::OnNMCustomdrawSliderMixingPublishVolume)
	ON_BN_CLICKED(IDC_BUTTON_EFFECT_START, &CAgoraAudioMixingDlg::OnBnClickedButtonEffectStart)
	ON_BN_CLICKED(IDC_BUTTON_EFFECT_RESUME, &CAgoraAudioMixingDlg::OnBnClickedButtonEffectResume)
	ON_BN_CLICKED(IDC_BUTTON_EFFECT_PAUSE, &CAgoraAudioMixingDlg::OnBnClickedButtonEffectPause)
	ON_BN_CLICKED(IDC_BUTTON_EFFECT_STOP, &CAgoraAudioMixingDlg::OnBnClickedButtonEffectStop)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_EFFECT_VOLUME, &CAgoraAudioMixingDlg::OnNMCustomdrawSliderEffectVolume)
END_MESSAGE_MAP()


void CAgoraAudioMixingDlg::OnSelchangeListInfoBroadcasting()
{
	int sel = m_lstInfo.GetCurSel();
	if (sel < 0)return;
	CString strDetail;
	m_lstInfo.GetText(sel, strDetail);
	m_staDetail.SetWindowText(strDetail);
}


void CAgoraAudioMixingDlg::OnShowWindow(BOOL bShow, UINT nStatus)
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


BOOL CAgoraAudioMixingDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_localVideoWnd.Create(NULL, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CRect(0, 0, 1, 1), this, ID_BASEWND_VIDEO + 100);
	RECT rcArea;
	m_staVideoArea.GetClientRect(&rcArea);
	m_localVideoWnd.MoveWindow(&rcArea);
	m_localVideoWnd.ShowWindow(SW_SHOW);


	m_sldMixingVolume.SetRange(0, 100, TRUE);
	m_sldMixingVolume.SetPos(100);
	m_sldMixingPlayoutVolume.SetRange(0, 100, TRUE);
	m_sldMixingPlayoutVolume.SetPos(100);
	m_sldMixingPublishVolume.SetRange(0, 100, TRUE);
	m_sldMixingPublishVolume.SetPos(100);

	m_sldEffectVolume.SetRange(0, 100, TRUE);
	m_sldEffectVolume.SetPos(100);



	ResumeStatus();
	return TRUE;  
}


BOOL CAgoraAudioMixingDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) {
		return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}


void CAgoraAudioMixingDlg::OnBnClickedButtonJoinchannel()
{
	if (!m_rtcEngine || !m_initialize)
		return;
	CString strInfo;
	if (!m_joinChannel) {
		CString strChannelName;
		m_edtChannel.GetWindowText(strChannelName);
		if (strChannelName.IsEmpty()) {
			AfxMessageBox(_T("Fill channel name first"));
			return;
		}
		std::string szChannelId = cs2utf8(strChannelName);
		ChannelMediaOptions options;
		options.channelProfile = CHANNEL_PROFILE_LIVE_BROADCASTING;
		options.clientRoleType = CLIENT_ROLE_BROADCASTER;
		//join channel in the engine.
		if (0 == m_rtcEngine->joinChannel(APP_TOKEN, szChannelId.c_str(),  0, options)) {
			strInfo.Format(_T("join channel %s, use ChannelMediaOptions"), getCurrentTime().AllocSysString());
			m_btnJoinChannel.EnableWindow(FALSE);
		}
	}
	else {
		//leave channel in the engine.
		if (0 == m_rtcEngine->leaveChannel()) {
			strInfo.Format(_T("leave channel %s"), getCurrentTime().AllocSysString());
		}
	}
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
}

//EID_JOINCHANNEL_SUCCESS message window handler
LRESULT CAgoraAudioMixingDlg::OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam)
{
	m_joinChannel = true;
	m_btnJoinChannel.SetWindowText(commonCtrlLeaveChannel);
	m_btnJoinChannel.EnableWindow(TRUE);
	CString strInfo;
	strInfo.Format(_T("%s:join success, uid=%u"), getCurrentTime().AllocSysString(), wParam);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	m_localVideoWnd.SetUID(wParam);
	//notify parent window
	::PostMessage(GetParent()->GetSafeHwnd(), WM_MSGID(EID_JOINCHANNEL_SUCCESS), TRUE, 0);
	return 0;
}

//EID_LEAVEHANNEL_SUCCESS message window handler
LRESULT CAgoraAudioMixingDlg::OnEIDLeaveChannel(WPARAM wParam, LPARAM lParam)
{
	m_joinChannel = false;
	m_btnJoinChannel.SetWindowText(commonCtrlJoinChannel);
	CString strInfo;
	strInfo.Format(_T("leave channel success %s"), getCurrentTime().AllocSysString());
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	::PostMessage(GetParent()->GetSafeHwnd(), WM_MSGID(EID_JOINCHANNEL_SUCCESS), FALSE, 0);
	return 0;
}

//EID_USER_JOINED message window handler
LRESULT CAgoraAudioMixingDlg::OnEIDUserJoined(WPARAM wParam, LPARAM lParam)
{
	CString strInfo;
	strInfo.Format(_T("%u joined"), wParam);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	return 0;
}

//EID_USER_OFFLINE message handler.
LRESULT CAgoraAudioMixingDlg::OnEIDUserOffline(WPARAM wParam, LPARAM lParam)
{
	uid_t remoteUid = (uid_t)wParam;
	CString strInfo;
	strInfo.Format(_T("%u offline, reason:%d"), remoteUid, lParam);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	return 0;
}

//EID_REMOTE_VIDEO_STATE_CHANGED message window handler.
LRESULT CAgoraAudioMixingDlg::OnEIDAudioMixingStateChanged(WPARAM wParam, LPARAM lParam)
{
	PAudioMixingState stateChanged = (PAudioMixingState)wParam;
	if (stateChanged) {
		//onAudioMixingStateChanged
		CString strSateInfo;
		CString strErrorInfo;
		AUDIO_MIXING_REASON_TYPE reason = (AUDIO_MIXING_REASON_TYPE)stateChanged->error;
		AUDIO_MIXING_STATE_TYPE state = (AUDIO_MIXING_STATE_TYPE)stateChanged->state;

		switch (reason)
		{
		case agora::rtc::AUDIO_MIXING_REASON_CAN_NOT_OPEN:
			strErrorInfo = _T("CAN_NOT_OPEN");
			break;
		case agora::rtc::AUDIO_MIXING_REASON_TOO_FREQUENT_CALL:
			strErrorInfo = _T("TOO_FREQUENT_CALL");
			break;
		case agora::rtc::AUDIO_MIXING_REASON_INTERRUPTED_EOF:
			strErrorInfo = _T("INTERRUPTED_EOF");
			break;
		case agora::rtc::AUDIO_MIXING_REASON_OK:
			strErrorInfo = _T("OK");
			break;
		case agora::rtc::AUDIO_MIXING_REASON_ONE_LOOP_COMPLETED:
			strErrorInfo = _T("COMPLETED");
			break;
		case agora::rtc::AUDIO_MIXING_REASON_ALL_LOOPS_COMPLETED:
			strErrorInfo = _T("ALL_LOOPS_COMPLETED");
			break;
		default:
			break;
		}

		switch (state)
		{
		case agora::rtc::AUDIO_MIXING_STATE_PLAYING:
			strSateInfo = _T("PLAYING");
			break;
		case agora::rtc::AUDIO_MIXING_STATE_PAUSED:
			strSateInfo = _T("PAUSED");
			break;
		case agora::rtc::AUDIO_MIXING_STATE_STOPPED:
			strSateInfo = _T("STOPPED");
			break;
		case agora::rtc::AUDIO_MIXING_STATE_FAILED:
			strSateInfo = _T("FAILED");
			break;
		default:
			break;
		}
		/*switch (state) {
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
		}*/
		CString strInfo;
		strInfo.Format(_T("onAudioMixingStateChanged")  );
		m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), strSateInfo);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), strErrorInfo);
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
void CAudioMixingEventHandler::onJoinChannelSuccess(const char* channel, uid_t uid, int elapsed)
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
void CAudioMixingEventHandler::onUserJoined(uid_t uid, int elapsed)
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
void CAudioMixingEventHandler::onUserOffline(uid_t uid, USER_OFFLINE_REASON_TYPE reason)
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

void CAudioMixingEventHandler::onLeaveChannel(const RtcStats& stats)
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
void CAudioMixingEventHandler::onAudioMixingStateChanged(AUDIO_MIXING_STATE_TYPE state, AUDIO_MIXING_REASON_TYPE reason)
{
	if (m_hMsgHanlder) {
		PAudioMixingState stateChanged = new AudioMixingState;
		stateChanged->error = reason;
		stateChanged->state = state;
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_REMOTE_AUDIO_MIXING_STATE_CHANED), (WPARAM)stateChanged, 0);
	}
}


void CAgoraAudioMixingDlg::OnBnClickedButtonMixingStart()
{
	CString audioUrl = GetExePath() + _T("\\ID_MUSIC_01.m4a");
	int ret = m_rtcEngine->startAudioMixing(cs2utf8(audioUrl).c_str(), false, -1);

	CString strInfo;
	strInfo.Format(_T("startAudioMixing path:%s, ret:%d"), audioUrl.AllocSysString(), ret);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
}


void CAgoraAudioMixingDlg::OnBnClickedButtonMixingResume()
{
	int ret = m_rtcEngine->resumeAudioMixing();

	CString strInfo;
	strInfo.Format(_T("resumeAudioMixing ret:%d"), ret);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
}


void CAgoraAudioMixingDlg::OnBnClickedButtonMixingPause()
{
	int ret = m_rtcEngine->pauseAudioMixing();

	CString strInfo;
	strInfo.Format(_T("pauseAudioMixing ret:%d"), ret);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
}


void CAgoraAudioMixingDlg::OnBnClickedButtonMixingStop()
{
	int ret = m_rtcEngine->stopAudioMixing();

	CString strInfo;
	strInfo.Format(_T("stopAudioMixing ret:%d"), ret);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
}


void CAgoraAudioMixingDlg::OnNMCustomdrawSliderMixingVolume(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	
	int pos = m_sldMixingVolume.GetPos();
	int ret = m_rtcEngine->adjustAudioMixingVolume(pos);

	*pResult = 0;
}


void CAgoraAudioMixingDlg::OnNMCustomdrawSliderMixingPlayoutVolume(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	int pos = m_sldMixingPlayoutVolume.GetPos();
	int ret = m_rtcEngine->adjustAudioMixingPlayoutVolume(pos);
	*pResult = 0;
}


void CAgoraAudioMixingDlg::OnNMCustomdrawSliderMixingPublishVolume(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	int pos = m_sldMixingPublishVolume.GetPos();
	int ret = m_rtcEngine->adjustAudioMixingPublishVolume(pos);
	*pResult = 0;
}


void CAgoraAudioMixingDlg::OnBnClickedButtonEffectStart()
{
	CString effectUrl = _T("https://webdemo.agora.io/ding.mp3");
	int ret = m_rtcEngine->playEffect(0, cs2utf8(effectUrl).c_str(), -1, 1, 0, 100, true, 0);
	CString strInfo;
	strInfo.Format(_T("playEffect url:%s, ret:%d"), effectUrl.AllocSysString(), ret);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
}


void CAgoraAudioMixingDlg::OnBnClickedButtonEffectResume()
{
	int ret = m_rtcEngine->resumeEffect(0);
	CString strInfo;
	strInfo.Format(_T("resumeEffectret:%d"), ret);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
}


void CAgoraAudioMixingDlg::OnBnClickedButtonEffectPause()
{
	int ret = m_rtcEngine->pauseEffect(0);
	CString strInfo;
	strInfo.Format(_T("pauseEffect:%d"), ret);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
}


void CAgoraAudioMixingDlg::OnBnClickedButtonEffectStop()
{
	int ret = m_rtcEngine->stopEffect(0);
	CString strInfo;
	strInfo.Format(_T("stopEffect:%d"), ret);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
}


void CAgoraAudioMixingDlg::OnNMCustomdrawSliderEffectVolume(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	
	int pos = m_sldEffectVolume.GetPos();
	m_rtcEngine->setEffectsVolume(pos);

	*pResult = 0;
}
