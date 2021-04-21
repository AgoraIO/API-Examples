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
	m_btnSetAudioMix.SetWindowText(audioMixingCtrlSetAudioMixing);
	m_staAudioMix.SetWindowText(audioMixingCtrlMixingPath);
	m_staAudioRepeat.SetWindowText(audioMixingCtrlRepeatTimes);
	m_chkOnlyLocal.SetWindowText(audioMixingCtrlOnlyLocal);
	m_chkMicroPhone.SetWindowText(audioMixingCtrlReplaceMicroPhone);
	m_staVolume.SetWindowTextW(AudioEffectCtrlVolume);
	m_staDuration.SetWindowText(audioMixingCtrlDuration);
}



//Initialize the Agora SDK
bool CAgoraAudioMixingDlg::InitAgora()
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


	
	m_mapState[710] = L"AUDIO_MIXING_STATE_PLAYING";
	m_mapState[711] = L"AUDIO_MIXING_STATE_PAUSED";
	m_mapState[713] = L"AUDIO_MIXING_STATE_STOPPED";
	m_mapState[714] = L"AUDIO_MIXING_STATE_FAILED";

	m_mapReason[701] = L"AUDIO_MIXING_REASON_CAN_NOT_OPEN";
	m_mapReason[702] = L"AUDIO_MIXING_REASON_TOO_FREQUENT_CALL";
	m_mapReason[703] = L"AUDIO_MIXING_REASON_INTERRUPTED_EOF";
	m_mapReason[720] = L"AUDIO_MIXING_REASON_STARTED_BY_USER";
	m_mapReason[721] = L"AUDIO_MIXING_REASON_ONE_LOOP_COMPLETED";
	m_mapReason[722] = L"AUDIO_MIXING_REASON_START_NEW_LOOP";
	m_mapReason[723] = L"AUDIO_MIXING_REASON_ALL_LOOPS_COMPLETED";
	m_mapReason[724] = L"AUDIO_MIXING_REASON_STOPPED_BY_USER";
	m_mapReason[725] = L"AUDIO_MIXING_REASON_PAUSED_BY_USER";
	m_mapReason[726] = L"AUDIO_MIXING_REASON_RESUMED_BY_USER";

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
		m_rtcEngine->release(true);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("release rtc engine"));
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
		canvas.renderMode = RENDER_MODE_FIT;
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
	m_edtRepatTimes.SetWindowText(_T(""));
	m_chkOnlyLocal.SetCheck(BST_UNCHECKED);
	m_chkOnlyLocal.EnableWindow(TRUE);
	m_chkMicroPhone.SetCheck(BST_UNCHECKED);
	m_chkMicroPhone.EnableWindow(TRUE);
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
	DDX_Control(pDX, IDC_STATIC_AUDIO_MIX, m_staAudioMix);
	DDX_Control(pDX, IDC_STATIC_AUDIO_REPEAT, m_staAudioRepeat);
	DDX_Control(pDX, IDC_EDIT_AUDIO_MIX_PATH, m_edtAudioMix);
	DDX_Control(pDX, IDC_BUTTON_SET_AUDIO_MIX, m_btnSetAudioMix);
	DDX_Control(pDX, IDC_EDIT_AUDIO_REPEAT_TIMES, m_edtRepatTimes);
	DDX_Control(pDX, IDC_CHK_ONLY_LOCAL, m_chkOnlyLocal);
	DDX_Control(pDX, IDC_CHK_REPLACE_MICROPHONE, m_chkMicroPhone);
	DDX_Control(pDX, IDC_STATIC_AUDIO_VOLUME, m_staVolume);
	DDX_Control(pDX, IDC_SLIDER_VOLUME, m_sldVolume);
	DDX_Control(pDX, IDC_STATIC_DURATION, m_staDuration);
	DDX_Control(pDX, IDC_STATIC_SECOND, m_staSecond);
}


BEGIN_MESSAGE_MAP(CAgoraAudioMixingDlg, CDialogEx)
	ON_LBN_SELCHANGE(IDC_LIST_INFO_BROADCASTING, &CAgoraAudioMixingDlg::OnSelchangeListInfoBroadcasting)
	ON_WM_SHOWWINDOW()
	ON_MESSAGE(WM_MSGID(EID_JOINCHANNEL_SUCCESS), &CAgoraAudioMixingDlg::OnEIDJoinChannelSuccess)
	ON_MESSAGE(WM_MSGID(EID_LEAVE_CHANNEL), &CAgoraAudioMixingDlg::OnEIDLeaveChannel)
	ON_MESSAGE(WM_MSGID(EID_USER_JOINED), &CAgoraAudioMixingDlg::OnEIDUserJoined)
	ON_MESSAGE(WM_MSGID(EID_USER_OFFLINE), &CAgoraAudioMixingDlg::OnEIDUserOffline)
	ON_MESSAGE(WM_MSGID(EID_REMOTE_VIDEO_STATE_CHANED), &CAgoraAudioMixingDlg::OnEIDRemoteVideoStateChanged)
	ON_MESSAGE(WM_MSGID(EID_AUDIO_MIXING_STATE_CHANGED), &CAgoraAudioMixingDlg::OnEIDAudioMixingStateChanged)
	ON_BN_CLICKED(IDC_BUTTON_JOINCHANNEL, &CAgoraAudioMixingDlg::OnBnClickedButtonJoinchannel)
	ON_BN_CLICKED(IDC_BUTTON_SET_AUDIO_MIX, &CAgoraAudioMixingDlg::OnBnClickedButtonSetAudioMix)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_VOLUME, &CAgoraAudioMixingDlg::OnReleasedcaptureSliderVolume)
		
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
	m_sldVolume.SetRange(0, 100);
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
		//join channel in the engine.
		if (0 == m_rtcEngine->joinChannel(APP_TOKEN, szChannelId.c_str(), "", 0)) {
			strInfo.Format(_T("join channel %s"), getCurrentTime());
			m_btnJoinChannel.EnableWindow(FALSE);
		}
	}
	else {
		//leave channel in the engine.
		if (0 == m_rtcEngine->leaveChannel()) {
			strInfo.Format(_T("leave channel %s"), getCurrentTime());
		}
	}
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
}


void CAgoraAudioMixingDlg::OnBnClickedButtonSetAudioMix()
{
	CString strPath;
	m_edtAudioMix.GetWindowText(strPath);
	std::string strAudioPath = cs2utf8(strPath);
	BOOL bOnlyLocal = FALSE;
	BOOL bReplaceMicroPhone = TRUE;
	int iRepeatTimes = 1;
	if (!m_audioMixing)
	{
		if (strAudioPath.empty())
		{
			AfxMessageBox(_T("audio path can not empty."));
			return;
		}
		bOnlyLocal = m_chkOnlyLocal.GetCheck() ? TRUE : FALSE;
		bReplaceMicroPhone = m_chkMicroPhone.GetCheck() ? TRUE : FALSE;
		CString strTimes;
		CString strInfo;
		m_edtRepatTimes.GetWindowText(strTimes);
		iRepeatTimes = _ttoi(strTimes);
		
		//start audio mixing in the engine.
		int nRet = m_rtcEngine->startAudioMixing(strAudioPath.c_str(),
			bOnlyLocal,
			bReplaceMicroPhone,
			iRepeatTimes
		);
		strInfo.Format(_T("path:%s,\nonlyLocal:%s,\nReplaceMicroPhone:%s,\nRepeatTimes:%d"), strPath,
			bOnlyLocal?_T("TRUE"):_T("FALSE"), bReplaceMicroPhone?_T("TRUE"):_T("FALSE"),
			iRepeatTimes);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
		m_btnSetAudioMix.SetWindowText(audioMixingCtrlUnSetAudioMixing);
		m_chkMicroPhone.EnableWindow(FALSE);
		m_chkOnlyLocal.EnableWindow(FALSE);
	}
	else {
		//stop audio mixing in the engine.
		m_rtcEngine->stopAudioMixing();
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("cancel audio mixing"));
		m_btnSetAudioMix.SetWindowText(audioMixingCtrlSetAudioMixing);
		m_chkOnlyLocal.EnableWindow(TRUE);
		m_chkMicroPhone.EnableWindow(TRUE);
		
	}
	m_audioMixing = !m_audioMixing;
}



//EID_JOINCHANNEL_SUCCESS message window handler
LRESULT CAgoraAudioMixingDlg::OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam)
{
	m_joinChannel = true;
	m_btnJoinChannel.SetWindowText(commonCtrlLeaveChannel);
	m_btnJoinChannel.EnableWindow(TRUE);
	CString strInfo;
	strInfo.Format(_T("%s:join success, uid=%u"), getCurrentTime(), wParam);
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
	strInfo.Format(_T("leave channel success %s"), getCurrentTime());
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
	VideoCanvas canvas;
	canvas.uid = remoteUid;
	canvas.view = NULL;
	m_rtcEngine->setupRemoteVideo(canvas);
	CString strInfo;
	strInfo.Format(_T("%u offline, reason:%d"), remoteUid, lParam);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	return 0;
}

//EID_REMOTE_VIDEO_STATE_CHANED message window handler.
LRESULT CAgoraAudioMixingDlg::OnEIDRemoteVideoStateChanged(WPARAM wParam, LPARAM lParam)
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

LRESULT CAgoraAudioMixingDlg::OnEIDAudioMixingStateChanged(WPARAM wParam, LPARAM lParam)
{
	if (wParam == AUDIO_MIXING_STATE_PLAYING) {
		
		int duration = m_rtcEngine->getAudioMixingDuration()/1000;
		CString strSecond;
		strSecond.Format(_T("%d%s"), duration, audioMixingCtrlSecond);
		m_staSecond.SetWindowText(strSecond);
	}
	else if (wParam == AUDIO_MIXING_STATE_STOPPED) {
		m_staSecond.SetWindowText(L"");
	}
	else if (wParam == AUDIO_MIXING_STATE_FAILED) {
		CString strInfo;
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("mix failed"));
		strInfo.Format(_T("state:%d "), m_mapState[wParam]);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
		strInfo.Format(_T("reason:%d"),  m_mapReason[lParam]);
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
void CAudioMixingEventHandler::onRemoteVideoStateChanged(uid_t uid, REMOTE_VIDEO_STATE state, REMOTE_VIDEO_STATE_REASON reason, int elapsed)
{
	if (m_hMsgHanlder) {
		PVideoStateStateChanged stateChanged = new VideoStateStateChanged;
		stateChanged->uid = uid;
		stateChanged->reason = reason;
		stateChanged->state = state;
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_REMOTE_VIDEO_STATE_CHANED), (WPARAM)stateChanged, 0);
	}
}


void CAgoraAudioMixingDlg::OnReleasedcaptureSliderVolume(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	int pos = m_sldVolume.GetPos();
	m_rtcEngine->adjustAudioMixingPlayoutVolume(pos);
	m_rtcEngine->adjustAudioMixingPublishVolume(pos);
	*pResult = 0;
}

void CAudioMixingEventHandler::onAudioMixingStateChanged(AUDIO_MIXING_STATE_TYPE state, AUDIO_MIXING_REASON_TYPE reason)
{
	if (m_hMsgHanlder) {
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_AUDIO_MIXING_STATE_CHANGED), (WPARAM)state, (LPARAM)reason);
	}

}