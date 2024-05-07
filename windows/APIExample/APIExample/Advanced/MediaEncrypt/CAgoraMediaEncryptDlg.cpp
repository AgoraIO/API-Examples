#include "stdafx.h"
#include "APIExample.h"
#include "CAgoraMediaEncryptDlg.h"



IMPLEMENT_DYNAMIC(CAgoraMediaEncryptDlg, CDialogEx)

CAgoraMediaEncryptDlg::CAgoraMediaEncryptDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_MEDIA_ENCRYPT, pParent)
{

}

CAgoraMediaEncryptDlg::~CAgoraMediaEncryptDlg()
{
}

void CAgoraMediaEncryptDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_VIDEO, m_staVideoArea);
	DDX_Control(pDX, IDC_LIST_INFO_BROADCASTING, m_lstInfo);
	DDX_Control(pDX, IDC_STATIC_CHANNELNAME, m_staChannel);
	DDX_Control(pDX, IDC_EDIT_CHANNELNAME, m_edtChannel);
	DDX_Control(pDX, IDC_BUTTON_JOINCHANNEL, m_btnJoinChannel);
	DDX_Control(pDX, IDC_STATIC_ENCRYPT_MODE, m_staEncryptMode);
	DDX_Control(pDX, IDC_COMBO_ENCRYPT_MODE, m_cmbEncryptMode);
	DDX_Control(pDX, IDC_STATIC_ENCRYPT_KEY, m_staEncryptKey);
	DDX_Control(pDX, IDC_EDIT_ENCRYPT_KEY, m_edtEncryptKey);
	DDX_Control(pDX, IDC_BUTTON_SET_MEDIA_ENCRYPT, m_btnSetEncrypt);
	DDX_Control(pDX, IDC_STATIC_DETAIL, m_staDetails);
}


BEGIN_MESSAGE_MAP(CAgoraMediaEncryptDlg, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_MESSAGE(WM_MSGID(EID_JOINCHANNEL_SUCCESS), &CAgoraMediaEncryptDlg::OnEIDJoinChannelSuccess)
	ON_MESSAGE(WM_MSGID(EID_LEAVE_CHANNEL), &CAgoraMediaEncryptDlg::OnEIDLeaveChannel)
	ON_MESSAGE(WM_MSGID(EID_USER_JOINED), &CAgoraMediaEncryptDlg::OnEIDUserJoined)
	ON_MESSAGE(WM_MSGID(EID_USER_OFFLINE), &CAgoraMediaEncryptDlg::OnEIDUserOffline)
	ON_MESSAGE(WM_MSGID(EID_REMOTE_VIDEO_STATE_CHANGED), &CAgoraMediaEncryptDlg::OnEIDRemoteVideoStateChanged)
	ON_BN_CLICKED(IDC_BUTTON_JOINCHANNEL, &CAgoraMediaEncryptDlg::OnBnClickedButtonJoinchannel)
	ON_BN_CLICKED(IDC_BUTTON_SET_MEDIA_ENCRYPT, &CAgoraMediaEncryptDlg::OnBnClickedButtonSetMediaEncrypt)
	ON_LBN_SELCHANGE(IDC_LIST_INFO_BROADCASTING, &CAgoraMediaEncryptDlg::OnSelchangeListInfoBroadcasting)
END_MESSAGE_MAP()

//Initialize the Ctrl Text.
void CAgoraMediaEncryptDlg::InitCtrlText()
{
	m_staEncryptKey.SetWindowText(mediaEncryptCtrlSecret);
	m_staEncryptMode.SetWindowText(mediaEncryptCtrlMode);
	m_staChannel.SetWindowText(commonCtrlChannel);
	m_btnJoinChannel.SetWindowText(commonCtrlJoinChannel);
	m_btnSetEncrypt.SetWindowText(mediaEncryptCtrlSetEncrypt);
}

//Initialize the Agora SDK
bool CAgoraMediaEncryptDlg::InitAgora()
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
void CAgoraMediaEncryptDlg::UnInitAgora()
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
void CAgoraMediaEncryptDlg::RenderLocalVideo()
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
void CAgoraMediaEncryptDlg::ResumeStatus()
{
	InitCtrlText();
	m_cmbEncryptMode.SetCurSel(0);
	m_edtChannel.SetWindowText(_T(""));
	m_lstInfo.ResetContent();
	m_edtEncryptKey.SetWindowText(_T(""));
	m_staDetails.SetWindowText(_T(""));
	m_btnSetEncrypt.EnableWindow(TRUE);
	m_joinChannel = false;
	m_initialize = false;
	m_setEncrypt = false;
}


BOOL CAgoraMediaEncryptDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_localVideoWnd.Create(NULL, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CRect(0, 0, 1, 1), this, ID_BASEWND_VIDEO + 100);
	RECT rcArea;
	m_staVideoArea.GetClientRect(&rcArea);
	int halfWidth = (rcArea.right - rcArea.left) / 2;
	rcArea.right = rcArea.left + halfWidth;
	m_localVideoWnd.MoveWindow(&rcArea);
	m_localVideoWnd.ShowWindow(SW_SHOW);
	m_remoteVideoWnd.Create(NULL, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CRect(0, 0, 1, 1), this, ID_BASEWND_VIDEO + 101);
	rcArea.left = rcArea.right;
	rcArea.right = rcArea.left + halfWidth;
	m_remoteVideoWnd.MoveWindow(&rcArea);
	m_remoteVideoWnd.ShowWindow(SW_SHOW);
	m_remoteVideoWnd.SetUID(0);

	int nIndex = 0;
	//m_cmbEncryptMode.InsertString(nIndex++, _T("aes-128-xts"));
	//m_cmbEncryptMode.InsertString(nIndex++, _T("aes-128-ecb"));
	//m_cmbEncryptMode.InsertString(nIndex++, _T("aes-256-xts"));
	m_cmbEncryptMode.InsertString(nIndex++, _T("AES_128_GCM2"));
	m_cmbEncryptMode.InsertString(nIndex++, _T("AES_256_GCM2"));
	m_mapEncryptMode.insert(std::make_pair("AES_128_GCM2", AES_128_GCM2));
	m_mapEncryptMode.insert(std::make_pair("AES_256_GCM2", AES_256_GCM2));
	int i = 0;
	ResumeStatus();
	return TRUE;
}


BOOL CAgoraMediaEncryptDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) {
		return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}


void CAgoraMediaEncryptDlg::OnShowWindow(BOOL bShow, UINT nStatus)
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


void CAgoraMediaEncryptDlg::OnBnClickedButtonJoinchannel()
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
		if (0 == m_rtcEngine->joinChannel(APP_TOKEN, szChannelId.c_str(), 0, options)) {
			strInfo.Format(_T("join channel %s, use ChannelMediaOptions"), getCurrentTime());
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


void decodeBase64(const std::string& encoded, std::vector<uint8_t>& out)
{
	auto unpadded = encoded;

	const auto num_padded = std::count(begin(encoded), end(encoded), '=');
	std::replace(begin(unpadded), end(unpadded), '=', 'A'); // A_64 == \0

	std::string decoded{ detail::BinaryFromBase64{begin(unpadded)},
	   detail::BinaryFromBase64{begin(unpadded) + unpadded.length()} };

	decoded.erase(end(decoded) - num_padded, end(decoded));
	std::copy(begin(decoded), end(decoded), out.begin());
}

//suggest generate secret using openssl on server
std::vector<uint8_t> getEncryptionSaltFromServer()
{
	std::string kdfSaltBase64 = "NiIeJ08AbtcQVjvV+oOEvF/4Dz5dy1CIwa805C8J2w0=";
	std::vector<uint8_t> kdfSalt;
	for (int i = 0; i < 32; ++i) {
		kdfSalt.push_back(0);
	}
	decodeBase64(kdfSaltBase64, kdfSalt);
	return kdfSalt;
}

//set media encrypt button click handler
void CAgoraMediaEncryptDlg::OnBnClickedButtonSetMediaEncrypt()
{
	//get window text to convert utf-8 string
	CString strEncryptMode;
	m_cmbEncryptMode.GetWindowText(strEncryptMode);
	std::string encryption = cs2utf8(strEncryptMode);
	CString strSecret;
	m_edtEncryptKey.GetWindowText(strSecret);
	std::string secret = cs2utf8(strSecret);
	
	//set encrypt mode and encrypt secret
	EncryptionConfig config;
	config.encryptionMode = m_mapEncryptMode[encryption.c_str()];
	config.encryptionKey = secret.c_str();
	memcpy(config.encryptionKdfSalt, getEncryptionSaltFromServer().data(), 32);
	//set encrypt mode
	m_rtcEngine->enableEncryption(true, config);
	CString strInfo;
	strInfo.Format(_T("encrypt mode:%s secret:%s"), strEncryptMode,
		strSecret);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
}

// select change for list control handler
void CAgoraMediaEncryptDlg::OnSelchangeListInfoBroadcasting()
{
	int sel = m_lstInfo.GetCurSel();
	if (sel < 0)return;
	CString strDetail;
	m_lstInfo.GetText(sel, strDetail);
	m_staDetails.SetWindowText(strDetail);
}


//EID_JOINCHANNEL_SUCCESS message window handler.
LRESULT CAgoraMediaEncryptDlg::OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam)
{
	m_joinChannel = true;
	m_btnJoinChannel.EnableWindow(TRUE);
	m_btnJoinChannel.SetWindowText(commonCtrlLeaveChannel);
	CString strInfo;
	strInfo.Format(_T("%s:join success, uid=%u"), getCurrentTime(), wParam);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	m_localVideoWnd.SetUID(wParam);
	m_btnSetEncrypt.EnableWindow(FALSE);
	//notify parent window
	::PostMessage(GetParent()->GetSafeHwnd(), WM_MSGID(EID_JOINCHANNEL_SUCCESS), TRUE, 0);
	return 0;
}

//EID_LEAVE_CHANNEL message window handler.
LRESULT CAgoraMediaEncryptDlg::OnEIDLeaveChannel(WPARAM wParam, LPARAM lParam)
{

	m_joinChannel = false;
	m_btnJoinChannel.SetWindowText(commonCtrlJoinChannel);

	CString strInfo;
	strInfo.Format(_T("leave channel success %s"), getCurrentTime());
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	m_btnSetEncrypt.EnableWindow(TRUE);

	if (m_remoteVideoWnd.GetUID() != 0) {
		m_remoteVideoWnd.SetUID(0);
		m_remoteVideoWnd.Reset();
	}

	::PostMessage(GetParent()->GetSafeHwnd(), WM_MSGID(EID_JOINCHANNEL_SUCCESS), FALSE, 0);
	return 0;
}

//EID_USER_JOINED message window handler.
LRESULT CAgoraMediaEncryptDlg::OnEIDUserJoined(WPARAM wParam, LPARAM lParam)
{
	CString strInfo;
	uid_t remoteUid = wParam;
	strInfo.Format(_T("%u joined"), remoteUid);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

	if (m_remoteVideoWnd.GetUID() == 0) {
		m_remoteVideoWnd.SetUID(remoteUid);

		VideoCanvas canvas;
		canvas.view = m_remoteVideoWnd.GetSafeHwnd();
		canvas.renderMode = agora::media::base::RENDER_MODE_HIDDEN;
		canvas.uid = remoteUid;
		m_rtcEngine->setupRemoteVideo(canvas);
	}
	return 0;
}


//EID_USER_OFFLINE message window handler.
LRESULT CAgoraMediaEncryptDlg::OnEIDUserOffline(WPARAM wParam, LPARAM lParam)
{
	uid_t remoteUid = (uid_t)wParam;
	
	CString strInfo;
	strInfo.Format(_T("%u offline, reason:%d"), remoteUid, lParam);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

	if (m_remoteVideoWnd.GetUID() == remoteUid) {
		m_remoteVideoWnd.SetUID(0);
		m_remoteVideoWnd.Reset();

		VideoCanvas canvas;
		canvas.view = nullptr;
		canvas.uid = remoteUid;
		m_rtcEngine->setupRemoteVideo(canvas);
	}

	return 0;
}

//EID_REMOTE_VIDEO_STATE_CHANGED message window handler.
LRESULT CAgoraMediaEncryptDlg::OnEIDRemoteVideoStateChanged(WPARAM wParam, LPARAM lParam)
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
void CAgoraMediaEncryptHandler::onJoinChannelSuccess(const char* channel, uid_t uid, int elapsed)
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
void CAgoraMediaEncryptHandler::onUserJoined(uid_t uid, int elapsed)
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
void CAgoraMediaEncryptHandler::onUserOffline(uid_t uid, USER_OFFLINE_REASON_TYPE reason)
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

void CAgoraMediaEncryptHandler::onLeaveChannel(const RtcStats& stats)
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
void CAgoraMediaEncryptHandler::onRemoteVideoStateChanged(uid_t uid, REMOTE_VIDEO_STATE state, REMOTE_VIDEO_STATE_REASON reason, int elapsed)
{
	if (m_hMsgHanlder) {
		PVideoStateStateChanged stateChanged = new VideoStateStateChanged;
		stateChanged->uid = uid;
		stateChanged->reason = reason;
		stateChanged->state = state;
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_REMOTE_VIDEO_STATE_CHANGED), (WPARAM)stateChanged, 0);
	}
}