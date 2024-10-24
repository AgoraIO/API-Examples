#include "stdafx.h"
#include "APIExample.h"
#include "CAgoraAudioProfile.h"



IMPLEMENT_DYNAMIC(CAgoraAudioProfile, CDialogEx)

CAgoraAudioProfile::CAgoraAudioProfile(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_AUDIO_PROFILE, pParent)
{
	sourceStreams =(MixedAudioStream*) malloc(sizeof(MixedAudioStream)*2);
	mainchannelName = new char[128];
	mSubChannelName = new char[128];
	m_exChannelRtcConn.channelId = mSubChannelName;
}

CAgoraAudioProfile::~CAgoraAudioProfile()
{
	if (sourceStreams)
	{
		AfxMessageBox(_T("free sourceStreams"));
		free(sourceStreams);
		sourceStreams = nullptr;
	}
	if (mainchannelName)
	{
		delete[] mainchannelName;
		mainchannelName = nullptr;
	}
	if (mSubChannelName)
	{
		delete[] mSubChannelName;
		mSubChannelName = nullptr;
	}
}

void CAgoraAudioProfile::DoDataExchange(CDataExchange *pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_VIDEO, m_staVideoArea);
	DDX_Control(pDX, IDC_STATIC_CHANNELNAME, m_staChannel);
	DDX_Control(pDX, IDC_EDIT_CHANNELNAME, m_edtChannel);
	DDX_Control(pDX, IDC_BUTTON_JOINCHANNEL, m_btnJoinChannel);
	DDX_Control(pDX, IDC_STATIC_ADUIO_PROFILE, m_staAudioProfile);
	DDX_Control(pDX, IDC_STATIC_ADUIO_SCENARIO, m_staAudioScenario);
	DDX_Control(pDX, IDC_COMBO_AUDIO_PROFILE, m_cmbAudioProfile);
	DDX_Control(pDX, IDC_COMBO_AUDIO_SCENARIO, m_cmbAudioScenario);
	DDX_Control(pDX, IDC_BUTTON_SET_AUDIO_PROFILE, m_btnSetAudioProfile);
	DDX_Control(pDX, IDC_LIST_INFO_BROADCASTING, m_lstInfo);
	DDX_Control(pDX, IDC_STATIC_DETAIL, m_staDetail);
	DDX_Control(pDX, IDC_EDIT_DEST_CHANNEL_NAME, mDestChannelEc);
	DDX_Control(pDX, IDC_CHECK_SECOND, mJoinSecondCheckbox);
}

// init ctrl text.
void CAgoraAudioProfile::InitCtrlText()
{
	m_staChannel.SetWindowText(commonCtrlChannel);
	m_btnJoinChannel.SetWindowText(commonCtrlJoinChannel);
	m_staAudioProfile.SetWindowText(audioProfileCtrlProfile);
	m_staAudioScenario.SetWindowText(audioProfileCtrlScenario);
	m_btnSetAudioProfile.SetWindowText(audioProfileCtrlSetAudioProfile);
}

// Initialize the Agora SDK
bool CAgoraAudioProfile::InitAgora()
{
	// create Agora RTC engine
	m_rtcEngine = static_cast<IRtcEngineEx*>(createAgoraRtcEngine());
	if (!m_rtcEngine)
	{
		m_lstInfo.InsertString(m_lstInfo.GetCount() - 1, _T("createAgoraRtcEngine failed"));
		return false;
	}
	// set message notify receiver window
	m_eventHandler.SetMsgReceiver(m_hWnd);

	RtcEngineContext context;
	std::string strAppID = GET_APP_ID;
	context.appId = strAppID.c_str();
	context.eventHandler = &m_eventHandler;
	// set channel profile in the engine to the CHANNEL_PROFILE_LIVE_BROADCASTING.
	context.channelProfile = CHANNEL_PROFILE_LIVE_BROADCASTING;
	// initialize the Agora RTC engine context.
	int ret = m_rtcEngine->initialize(context);
	if (ret != 0)
	{
		m_initialize = false;
		CString strInfo;
		strInfo.Format(_T("initialize failed: %d"), ret);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
		return false;
	}
	else
		m_initialize = true;
	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("initialize success"));
	// enable video in the engine.
	m_rtcEngine->enableVideo();
	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("enable video"));

	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("live broadcasting"));
	// set client role in the engine to the CLIENT_ROLE_BROADCASTER.
	// m_rtcEngine->setClientRole(CLIENT_ROLE_BROADCASTER);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("setClientRole broadcaster"));
	return true;
}

void CAgoraAudioProfile::UnInitAgora()
{
	if (m_rtcEngine)
	{
		if (m_joinChannel)
			// leave channel
			m_joinChannel = !m_rtcEngine->leaveChannel();
		// stop preview in the engine.
		m_rtcEngine->stopPreview();
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("stopPreview"));
		// disable video in the engine.
		m_rtcEngine->disableVideo();
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("disableVideo"));
		// release engine.
		if (m_initialize)
		{
			m_rtcEngine->release(true);
			m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("release rtc engine"));
		}
		m_rtcEngine = NULL;
	}
}

// render local video from SDK local capture.
void CAgoraAudioProfile::RenderLocalVideo()
{
	if (m_rtcEngine)
	{
		// start preview in the engine.
		m_rtcEngine->startPreview();
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("startPreview"));
		VideoCanvas canvas;
		canvas.renderMode = media::base::RENDER_MODE_FIT;
		canvas.uid = 0;
		canvas.view = m_localVideoWnd.GetSafeHwnd();
		// setup local video in the engine to canvas.
		m_rtcEngine->setupLocalVideo(canvas);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("setupLocalVideo"));
	}
}

// resume status.
void CAgoraAudioProfile::ResumeStatus()
{
	InitCtrlText();
	m_staDetail.SetWindowText(_T(""));
	m_edtChannel.SetWindowText(_T(""));
	m_cmbAudioProfile.SetCurSel(0);
	m_cmbAudioScenario.SetCurSel(0);
	m_btnSetAudioProfile.EnableWindow(TRUE);
	m_lstInfo.ResetContent();
	m_joinChannel = false;
	m_initialize = false;
	m_setAudio = false;
}

BEGIN_MESSAGE_MAP(CAgoraAudioProfile, CDialogEx)
ON_WM_SHOWWINDOW()
ON_BN_CLICKED(IDC_BUTTON_JOINCHANNEL, &CAgoraAudioProfile::OnBnClickedButtonJoinchannel)
ON_BN_CLICKED(IDC_BUTTON_SET_AUDIO_PROFILE, &CAgoraAudioProfile::OnBnClickedButtonSetAudioProfile)
ON_MESSAGE(WM_MSGID(EID_JOINCHANNEL_SUCCESS), &CAgoraAudioProfile::OnEIDJoinChannelSuccess)
ON_MESSAGE(WM_MSGID(EID_LEAVE_CHANNEL), &CAgoraAudioProfile::OnEIDLeaveChannel)
ON_MESSAGE(WM_MSGID(EID_USER_JOINED), &CAgoraAudioProfile::OnEIDUserJoined)
ON_MESSAGE(WM_MSGID(EID_USER_OFFLINE), &CAgoraAudioProfile::OnEIDUserOffline)
ON_MESSAGE(WM_MSGID(EID_REMOTE_VIDEO_STATE_CHANGED), &CAgoraAudioProfile::OnEIDRemoteVideoStateChanged)
ON_LBN_SELCHANGE(IDC_LIST_INFO_BROADCASTING, &CAgoraAudioProfile::OnSelchangeListInfoBroadcasting)
ON_BN_CLICKED(IDC_CHECK_SECOND, &CAgoraAudioProfile::OnBnClickedCheckSecond)
END_MESSAGE_MAP()

void CAgoraAudioProfile::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);
	if (bShow) // bShwo is true ,show window
	{
		InitCtrlText();
		RenderLocalVideo();
	}
	else
	{
		ResumeStatus();
	}
}

BOOL CAgoraAudioProfile::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_localVideoWnd.Create(NULL, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CRect(0, 0, 1, 1), this, ID_BASEWND_VIDEO + 100);
	RECT rcArea;
	m_staVideoArea.GetClientRect(&rcArea);
	m_localVideoWnd.MoveWindow(&rcArea);
	m_localVideoWnd.ShowWindow(SW_SHOW);

	int nIndex = 0;
	m_cmbAudioProfile.InsertString(nIndex++, _T("AUDIO_PROFILE_DEFAULT"));
	m_cmbAudioProfile.InsertString(nIndex++, _T("AUDIO_PROFILE_SPEECH_STANDARD"));
	m_cmbAudioProfile.InsertString(nIndex++, _T("AUDIO_PROFILE_MUSIC_STANDARD"));
	m_cmbAudioProfile.InsertString(nIndex++, _T("AUDIO_PROFILE_MUSIC_STANDARD_STEREO"));
	m_cmbAudioProfile.InsertString(nIndex++, _T("AUDIO_PROFILE_MUSIC_HIGH_QUALITY"));

	m_cmbAudioProfile.InsertString(nIndex++, _T("AUDIO_PROFILE_MUSIC_HIGH_QUALITY_STEREO"));

	nIndex = 0;
	m_cmbAudioScenario.InsertString(nIndex++, _T("AUDIO_SCENARIO_DEFAULT"));
	/*m_cmbAudioScenario.InsertString(nIndex++, _T("AUDIO_SCENARIO_CHATROOM_ENTERTAINMENT"));
	m_cmbAudioScenario.InsertString(nIndex++, _T("AUDIO_SCENARIO_EDUCATION"));
	m_cmbAudioScenario.InsertString(nIndex++, _T("AUDIO_SCENARIO_GAME_STREAMING"));
	m_cmbAudioScenario.InsertString(nIndex++, _T("AUDIO_SCENARIO_SHOWROOM"));
	m_cmbAudioScenario.InsertString(nIndex++, _T("AUDIO_SCENARIO_HIGH_DEFINITION"));*/
	m_cmbAudioScenario.InsertString(nIndex++, _T("AUDIO_SCENARIO_CHATROOM_GAMING"));
	ResumeStatus();

	return TRUE;
}


void CAgoraAudioProfile::OnBnClickedButtonJoinchannel()
{
	if (!m_rtcEngine || !m_initialize)
		return;
	CString strInfo;
	if (!m_joinChannel)
	{
		CString strChannelName;
		m_edtChannel.GetWindowText(strChannelName);
		if (strChannelName.IsEmpty())
		{
			AfxMessageBox(_T("Fill channel name first"));
			return;
		}
		std::string mainChannelId = cs2utf8(strChannelName);
		ChannelMediaOptions options;
		options.channelProfile = CHANNEL_PROFILE_LIVE_BROADCASTING;
		options.clientRoleType = CLIENT_ROLE_BROADCASTER;
		// join channel in the engine.
		if (0 == m_rtcEngine->joinChannel(APP_TOKEN, mainChannelId.c_str(), 0, options))
		{
			strcpy(mainchannelName, mainChannelId.c_str());
			strInfo.Format(_T("join channel %s, use ChannelMediaOptions"), getCurrentTime());
			m_btnJoinChannel.EnableWindow(FALSE);
		}
	}
	else
	{
		leaveSecondChannelAndStopAudioMixer();
		mJoinSecondCheckbox.SetCheck(0);
		// leave channel in the engine.
		if (0 == m_rtcEngine->leaveChannel())
		{
			strInfo.Format(_T("leave channel %s"), getCurrentTime());
		}
	}
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
}

// set audio profile or unset.
void CAgoraAudioProfile::OnBnClickedButtonSetAudioProfile()
{
	if (!m_setAudio)
	{
		int nProfileSel = m_cmbAudioProfile.GetCurSel();
		int nScenSel = m_cmbAudioScenario.GetCurSel();
		CString strInfo;
		CString strAudioProfile;
		CString strAudioScenario;
		m_cmbAudioProfile.GetWindowText(strAudioProfile);
		m_cmbAudioScenario.GetWindowText(strAudioScenario);
		strInfo.Format(_T("Profile:%s,\nScenario:%s"), strAudioProfile, strAudioScenario);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
		AUDIO_SCENARIO_TYPE type = AUDIO_SCENARIO_DEFAULT;
		if (nScenSel == 1)
			type = AUDIO_SCENARIO_GAME_STREAMING;
		// set audio profile.
		m_rtcEngine->setAudioProfile((AUDIO_PROFILE_TYPE)nProfileSel, (AUDIO_SCENARIO_TYPE)nScenSel);
		m_btnSetAudioProfile.SetWindowText(audioProfileCtrlUnSetAudioProfile);
	}
	else
	{
		m_rtcEngine->setAudioProfile((AUDIO_PROFILE_TYPE)0, (AUDIO_SCENARIO_TYPE)0);
		m_btnSetAudioProfile.SetWindowText(audioProfileCtrlSetAudioProfile);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("reset default audio profile"));
	}
	m_setAudio = !m_setAudio;
}

void CAgoraAudioProfile::joinSecondChannelAndStartAudioMixer()
{
	CString channelNameEx;
	mDestChannelEc.GetWindowTextW(channelNameEx);
	if (channelNameEx.IsEmpty())
	{
		AfxMessageBox(_T("Fill ex channel name first"));
		return;
	}
	std::string subChannelId = cs2utf8(channelNameEx);

	ChannelMediaOptions options;
	options.channelProfile = CHANNEL_PROFILE_LIVE_BROADCASTING;
	options.clientRoleType = CLIENT_ROLE_BROADCASTER;
	options.publishMicrophoneTrack = false;
	options.publishMixedAudioTrack = true;
	options.enableAudioRecordingOrPlayout = false;
	options.autoSubscribeAudio = false;

	m_exChannelRtcConn.localUid = generateUid();
	strcpy(mSubChannelName, subChannelId.c_str());
	int ret = m_rtcEngine->joinChannelEx("", m_exChannelRtcConn, options, &m_eventHandlerEx);
	if (ret == 0)
	{
		CString strInfo;
		strInfo.Format(_T("join channelEx %s , ret=%d"), channelNameEx, ret);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

		LocalAudioMixerConfiguration config;
		config.syncWithLocalMic = false;
		config.streamCount = 2;
		config.audioInputStreams = sourceStreams;
		
		MixedAudioStream remoteStream( AUDIO_SOURCE_REMOTE_CHANNEL);
		remoteStream.channelId = mainchannelName;
		config.audioInputStreams[0] = remoteStream;

		MixedAudioStream remoteStream2(AUDIO_SOURCE_MICROPHONE);
		remoteStream2.channelId = mainchannelName;
		config.audioInputStreams[1] = remoteStream2;
		int ret = m_rtcEngine->startLocalAudioMixer(config);
		if (ret != 0)
		{
			AfxMessageBox(_T("startLocalAudioMixer failed"));
		}
	}
	else
	{
		AfxMessageBox(_T("join channelEx failed"));
	}

}

void CAgoraAudioProfile::leaveSecondChannelAndStopAudioMixer()
{
	 m_rtcEngine->stopLocalAudioMixer();
	m_rtcEngine->leaveChannelEx(m_exChannelRtcConn);
}

// EID_JOINCHANNEL_SUCCESS message window handler
LRESULT CAgoraAudioProfile::OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam)
{
	m_joinChannel = true;
	m_btnJoinChannel.SetWindowText(commonCtrlLeaveChannel);
	m_btnJoinChannel.EnableWindow(TRUE);
	CString strInfo;
	strInfo.Format(_T("%s:join success, uid=%u"), getCurrentTime(), wParam);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	m_localVideoWnd.SetUID(wParam);
	// notify parent window
	::PostMessage(GetParent()->GetSafeHwnd(), WM_MSGID(EID_JOINCHANNEL_SUCCESS), TRUE, 0);
	return 0;
}

// EID_LEAVEHANNEL_SUCCESS message window handler
LRESULT CAgoraAudioProfile::OnEIDLeaveChannel(WPARAM wParam, LPARAM lParam)
{
	m_joinChannel = false;
	m_btnJoinChannel.SetWindowText(commonCtrlJoinChannel);
	m_btnSetAudioProfile.EnableWindow(TRUE);
	CString strInfo;
	strInfo.Format(_T("leave channel success %s"), getCurrentTime());
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	::PostMessage(GetParent()->GetSafeHwnd(), WM_MSGID(EID_JOINCHANNEL_SUCCESS), FALSE, 0);
	return 0;
}

// EID_USER_JOINED message window handler
LRESULT CAgoraAudioProfile::OnEIDUserJoined(WPARAM wParam, LPARAM lParam)
{
	CString strInfo;
	strInfo.Format(_T("%u joined"), wParam);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

	return 0;
}

// EID_USER_OFFLINE message handler.
LRESULT CAgoraAudioProfile::OnEIDUserOffline(WPARAM wParam, LPARAM lParam)
{
	uid_t remoteUid = (uid_t)wParam;
	CString strInfo;
	strInfo.Format(_T("%u offline, reason:%d"), remoteUid, lParam);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	return 0;
}

// EID_REMOTE_VIDEO_STATE_CHANGED message window handler.
LRESULT CAgoraAudioProfile::OnEIDRemoteVideoStateChanged(WPARAM wParam, LPARAM lParam)
{
	PVideoStateStateChanged stateChanged = (PVideoStateStateChanged)wParam;
	if (stateChanged)
	{
		// onRemoteVideoStateChanged
		CString strSateInfo;
		switch (stateChanged->state)
		{
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
void CAudioProfileEventHandler::onJoinChannelSuccess(const char *channel, uid_t uid, int elapsed)
{
	if (m_hMsgHanlder)
	{
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
void CAudioProfileEventHandler::onUserJoined(uid_t uid, int elapsed)
{
	if (m_hMsgHanlder)
	{
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
void CAudioProfileEventHandler::onUserOffline(uid_t uid, USER_OFFLINE_REASON_TYPE reason)
{
	if (m_hMsgHanlder)
	{
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

void CAudioProfileEventHandler::onLeaveChannel(const RtcStats &stats)
{
	if (m_hMsgHanlder)
	{
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
void CAudioProfileEventHandler::onRemoteVideoStateChanged(uid_t uid, REMOTE_VIDEO_STATE state, REMOTE_VIDEO_STATE_REASON reason, int elapsed)
{
	if (m_hMsgHanlder)
	{
		PVideoStateStateChanged stateChanged = new VideoStateStateChanged;
		stateChanged->uid = uid;
		stateChanged->reason = reason;
		stateChanged->state = state;
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_REMOTE_VIDEO_STATE_CHANGED), (WPARAM)stateChanged, 0);
	}
}

BOOL CAgoraAudioProfile::PreTranslateMessage(MSG *pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}

void CAgoraAudioProfile::OnSelchangeListInfoBroadcasting()
{
	int sel = m_lstInfo.GetCurSel();
	if (sel < 0)
		return;
	CString strDetail;
	m_lstInfo.GetText(sel, strDetail);
	m_staDetail.SetWindowText(strDetail);
}


void CAgoraAudioProfile::OnBnClickedCheckSecond()
{
	int isChecked = mJoinSecondCheckbox.GetCheck();
	if (!isChecked) {
		leaveSecondChannelAndStopAudioMixer();
	}
	else {
		if (!m_joinChannel)
		{
			AfxMessageBox(TEXT("Join main channel first"));
			mJoinSecondCheckbox.SetCheck(0);
			return;
		}
		CString channelNameEx;
		mDestChannelEc.GetWindowTextW(channelNameEx);
		if (channelNameEx.IsEmpty())
		{
			AfxMessageBox(TEXT("Fill ex channel name first"));
			mJoinSecondCheckbox.SetCheck(0);
			return;
		}
		joinSecondChannelAndStartAudioMixer();
	}
}
