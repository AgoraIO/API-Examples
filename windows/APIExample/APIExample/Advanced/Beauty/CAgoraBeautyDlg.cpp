#include "stdafx.h"
#include "APIExample.h"
#include "CAgoraBeautyDlg.h"


IMPLEMENT_DYNAMIC(CAgoraBeautyDlg, CDialogEx)

CAgoraBeautyDlg::CAgoraBeautyDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_BEAUTY, pParent)
{

}

CAgoraBeautyDlg::~CAgoraBeautyDlg()
{
}

//Initialize the ctrl text.
void CAgoraBeautyDlg::InitCtrlText()
{
	m_btnJoinChannel.SetWindowText(commonCtrlJoinChannel);
	m_staChannel.SetWindowText(commonCtrlChannel);
	m_staRedness.SetWindowText(beautyCtrlRedness);
	m_staLight.SetWindowText(beautyCtrlLightening);
	m_staLightContrast.SetWindowText(beautyCtrlLighteningContrastLevel);
	m_staSoomthness.SetWindowText(beautyCtrlSmoothness);
	m_chkBeauty.SetWindowText(beautyCtrlEnable);
}

//Initialize the Agora SDK
bool CAgoraBeautyDlg::InitAgora()
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
	
	//load extebsion
	m_rtcEngine->loadExtensionProvider("libagora_video_process_extension.dll");
	m_rtcEngine->enableExtension("agora", "beauty", true, PRIMARY_CAMERA_SOURCE);
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
void CAgoraBeautyDlg::UnInitAgora()
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
void CAgoraBeautyDlg::RenderLocalVideo()
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
void CAgoraBeautyDlg::ResumeStatus()
{
	m_edtChannel.SetWindowText(_T(""));
	m_edtLightLevel.SetWindowText(_T(""));
	m_edtReadness.SetWindowText(_T(""));
	m_edtSmoothness.SetWindowText(_T(""));
	m_staDetail.SetWindowText(_T(""));

	m_chkBeauty.SetCheck(BST_UNCHECKED);
	m_cmbBeautyLevel.SetCurSel(0);
	m_lstInfo.ResetContent();
	SetBeauty(false);
	m_joinChannel = false;
	m_initialize = false;
}


void CAgoraBeautyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_CHANNELNAME, m_staChannel);
	DDX_Control(pDX, IDC_EDIT_CHANNELNAME, m_edtChannel);
	DDX_Control(pDX, IDC_CHECK_BEAUTY_ENABLE, m_chkBeauty);
	DDX_Control(pDX, IDC_BUTTON_JOINCHANNEL, m_btnJoinChannel);
	DDX_Control(pDX, IDC_COMBO_BEAUTE_LIGHTENING_CONTRAST_LEVEL, m_cmbBeautyLevel);
	DDX_Control(pDX, IDC_EDIT_LIGHTENING, m_edtLightLevel);
	DDX_Control(pDX, IDC_STATIC_BEAUTY_REDNESS, m_staRedness);
	DDX_Control(pDX, IDC_STATIC_BEAUTY_SMOOTHNESS, m_staSoomthness);
	DDX_Control(pDX, IDC_EDIT_BEAUTY_REDNESS, m_edtReadness);
	DDX_Control(pDX, IDC_EDIT_BEAUTY_SMOOTHNESS, m_edtSmoothness);
	DDX_Control(pDX, IDC_STATIC_VIDEO, m_staVideoArea);
	DDX_Control(pDX, IDC_LIST_INFO_BROADCASTING, m_lstInfo);
	DDX_Control(pDX, IDC_STATIC_BEAUTY_LIGHTENING_CONTRAST_LEVEL, m_staLightContrast);
	DDX_Control(pDX, IDC_STATIC_BEAUTY_LIGHTENING, m_staLight);
	DDX_Control(pDX, IDC_STATIC_DETAIL, m_staDetail);
}


BEGIN_MESSAGE_MAP(CAgoraBeautyDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_JOINCHANNEL, &CAgoraBeautyDlg::OnBnClickedButtonJoinchannel)
	ON_MESSAGE(WM_MSGID(EID_JOINCHANNEL_SUCCESS), &CAgoraBeautyDlg::OnEIDJoinChannelSuccess)
	ON_MESSAGE(WM_MSGID(EID_LEAVE_CHANNEL), &CAgoraBeautyDlg::OnEIDLeaveChannel)
	ON_MESSAGE(WM_MSGID(EID_USER_JOINED), &CAgoraBeautyDlg::OnEIDUserJoined)
	ON_MESSAGE(WM_MSGID(EID_USER_OFFLINE), &CAgoraBeautyDlg::OnEIDUserOffline)
	ON_MESSAGE(WM_MSGID(EID_REMOTE_VIDEO_STATE_CHANED), &CAgoraBeautyDlg::OnEIDRemoteVideoStateChanged)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_CHECK_BEAUTY_ENABLE, &CAgoraBeautyDlg::OnBnClickedCheckbeautyCtrlEnable)
	ON_LBN_SELCHANGE(IDC_LIST_INFO_BROADCASTING, &CAgoraBeautyDlg::OnSelchangeListInfoBroadcasting)
END_MESSAGE_MAP()



// join channel or level channel.
void CAgoraBeautyDlg::OnBnClickedButtonJoinchannel()
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


// Set the lighteningContrastLevel,lighteningLevel,rednessLevel,smoothnessLevel.
void CAgoraBeautyDlg::SetBeauty(bool enabled,
	agora::rtc::BeautyOptions::LIGHTENING_CONTRAST_LEVEL lighteningContrastLevel,
	float lighteningLevel,
	float rednessLevel,
	float smoothnessLevel)
{
	//Beauty options to set 
	agora::rtc::BeautyOptions options;
	options.lighteningContrastLevel = lighteningContrastLevel;
	options.lighteningLevel = lighteningLevel;
	options.smoothnessLevel = smoothnessLevel;
	options.rednessLevel = rednessLevel;
	//set Beauty options.
	if (m_rtcEngine)
	{
		int ret = m_rtcEngine->setBeautyEffectOptions(enabled, options);
		CString strInfo;
		strInfo.Format(_T("set beauty."));
		m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	}
}

//check box clicked to set beauty.
void CAgoraBeautyDlg::OnBnClickedCheckbeautyCtrlEnable()
{
	bool enabled = m_chkBeauty.GetCheck() == BST_CHECKED ? TRUE : FALSE;
	//Beauty options to set 
	CString tmp;
	auto lighteningContrastLevel = (agora::rtc::BeautyOptions::LIGHTENING_CONTRAST_LEVEL)m_cmbBeautyLevel.GetCurSel();
	float lighteningLevel;
	float rednessLevel;
	float smoothnessLevel;
	m_edtLightLevel.GetWindowText(tmp);
	auto func = [](float a)->float {
		return a <0.0f ? 0.0f : a>1.0f ? 1.0f : a;
	};
	lighteningLevel = func(static_cast<float>(_ttof(tmp)/10));
	m_edtReadness.GetWindowText(tmp);
	rednessLevel = func(static_cast<float>(_ttof(tmp)/10));
	m_edtSmoothness.GetWindowText(tmp);
	smoothnessLevel = func(static_cast<float>(_ttof(tmp)/10));
	CString strInfo;
	CString strlighteningContrastLevel;
	m_cmbBeautyLevel.GetWindowText(strlighteningContrastLevel);
	SetBeauty(enabled, lighteningContrastLevel, lighteningLevel, rednessLevel, smoothnessLevel);
	if (enabled)
	{
		strInfo.Format(_T("lighteningContrastLevel:%s,\nlightening:%.1f,\nredness:%.1f,\nsmoothness:%.1f"),
			strlighteningContrastLevel,
			lighteningLevel, rednessLevel, smoothnessLevel);
	}
	else {
		strInfo.Format(_T("unset beauty."));
	}
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
}

//create views and init data.
BOOL CAgoraBeautyDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_localVideoWnd.Create(NULL, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CRect(0, 0, 1, 1), this, ID_BASEWND_VIDEO + 100);
	RECT rcArea;
	m_staVideoArea.GetClientRect(&rcArea);
	m_localVideoWnd.MoveWindow(&rcArea);
	m_localVideoWnd.ShowWindow(SW_SHOW);

	int nIndex = 0;
	m_cmbBeautyLevel.InsertString(nIndex++, _T("Low contrast level"));
	m_cmbBeautyLevel.InsertString(nIndex++, _T("Normal contrast level."));
	m_cmbBeautyLevel.InsertString(nIndex++, _T("High contrast level"));

	ResumeStatus();
	return TRUE;
}


//show window or hide window.
void CAgoraBeautyDlg::OnShowWindow(BOOL bShow, UINT nStatus)
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




//EID_JOINCHANNEL_SUCCESS message window handler
LRESULT CAgoraBeautyDlg::OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam)
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
LRESULT CAgoraBeautyDlg::OnEIDLeaveChannel(WPARAM wParam, LPARAM lParam)
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
LRESULT CAgoraBeautyDlg::OnEIDUserJoined(WPARAM wParam, LPARAM lParam)
{
	CString strInfo;
	strInfo.Format(_T("%u joined"), wParam);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

	return 0;
}

//EID_USER_OFFLINE message handler.
LRESULT CAgoraBeautyDlg::OnEIDUserOffline(WPARAM wParam, LPARAM lParam)
{
	uid_t remoteUid = (uid_t)wParam;
	VideoCanvas canvas;
	canvas.uid = remoteUid;
	canvas.view = NULL;
	//set remote video to the engine.
	m_rtcEngine->setupRemoteVideo(canvas);
	CString strInfo;
	strInfo.Format(_T("%u offline, reason:%d"), remoteUid, lParam);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	return 0;
}

//EID_REMOTE_VIDEO_STATE_CHANED message window handler.
LRESULT CAgoraBeautyDlg::OnEIDRemoteVideoStateChanged(WPARAM wParam, LPARAM lParam)
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
void CBeautyEventHandler::onJoinChannelSuccess(const char* channel, uid_t uid, int elapsed)
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
void CBeautyEventHandler::onUserJoined(uid_t uid, int elapsed)
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
void CBeautyEventHandler::onUserOffline(uid_t uid, USER_OFFLINE_REASON_TYPE reason)
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

void CBeautyEventHandler::onLeaveChannel(const RtcStats& stats)
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
void CBeautyEventHandler::onRemoteVideoStateChanged(uid_t uid, REMOTE_VIDEO_STATE state, REMOTE_VIDEO_STATE_REASON reason, int elapsed)
{
	if (m_hMsgHanlder) {
		PVideoStateStateChanged stateChanged = new VideoStateStateChanged;
		stateChanged->uid = uid;
		stateChanged->reason = reason;
		stateChanged->state = state;
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_REMOTE_VIDEO_STATE_CHANED), (WPARAM)stateChanged, 0);
	}
}



BOOL CAgoraBeautyDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) {
		return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}


void CAgoraBeautyDlg::OnSelchangeListInfoBroadcasting()
{
	int sel = m_lstInfo.GetCurSel();
	if (sel < 0)return;
	CString strDetail;
	m_lstInfo.GetText(sel, strDetail);
	m_staDetail.SetWindowText(strDetail);
}
