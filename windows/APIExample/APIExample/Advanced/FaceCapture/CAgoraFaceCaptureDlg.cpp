#include "stdafx.h"
#include "APIExample.h"
#include "CAgoraFaceCaptureDlg.h"



IMPLEMENT_DYNAMIC(CAgoraFaceCaptureDlg, CDialogEx)

CAgoraFaceCaptureDlg::CAgoraFaceCaptureDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(CAgoraFaceCaptureDlg::IDD, pParent)
{

}

CAgoraFaceCaptureDlg::~CAgoraFaceCaptureDlg()
{
}

//Initialize the Ctrl Text.
void CAgoraFaceCaptureDlg::InitCtrlText()
{
	m_staChannel.SetWindowText(commonCtrlChannel);
	m_btnJoinChannel.SetWindowText(commonCtrlJoinChannel);
}


//Initialize the Agora SDK
bool CAgoraFaceCaptureDlg::InitAgora()
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

	// load face capture extension
	ret = m_rtcEngine->loadExtensionProvider("libagora_face_capture_extension.dll");
	CString strInfo;
	strInfo.Format(_T("Load face capture extension ret: %d"), ret);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

	// enable face capture
	ret = m_rtcEngine->enableExtension("agora_video_filters_face_capture", "face_capture", true, MEDIA_SOURCE_TYPE::PRIMARY_CAMERA_SOURCE);
	strInfo.Format(_T("Enable face capture extension ret: %d"), ret);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

	// face capture authentication
	CString strAuth;
	strAuth.Format(_T("{\"company_id\":\"%s\",\"license\":\"%s\"}"), _T("agoraTest"), utf82cs(std::string(FACE_CAPTURE_LICENSE)));
	ret = m_rtcEngine->setExtensionProperty("agora_video_filters_face_capture",
		"face_capture",
		"authentication_information",
		cs2utf8(strAuth).c_str(),
		MEDIA_SOURCE_TYPE::PRIMARY_CAMERA_SOURCE);
	strInfo.Format(_T("Auth face capture ret: %d"), ret);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

	// load speech driven extension
	ret = m_rtcEngine->loadExtensionProvider("libagora_lip_sync_extension.dll");
	strInfo.Format(_T("Load speech driven extension ret: %d"), ret);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

	// enable speech driven
	ret = m_rtcEngine->enableExtension("agora_filters_lip_sync", "lip_sync", true, MEDIA_SOURCE_TYPE::SPEECH_DRIVEN_VIDEO_SOURCE);
	strInfo.Format(_T("Enable speech driven extension ret: %d"), ret);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

	// speech driven authentication
	strAuth.Format(_T("{\"company_id\":\"%s\",\"license\":\"%s\",\"open_agc\":true}"), _T("agoraTest"), utf82cs(std::string(FACE_CAPTURE_LICENSE)));
	ret = m_rtcEngine->setExtensionProperty("agora_filters_lip_sync",
		"lip_sync",
		"parameters",
		cs2utf8(strAuth).c_str(),
		MEDIA_SOURCE_TYPE::SPEECH_DRIVEN_VIDEO_SOURCE);
	strInfo.Format(_T("Auth speech driven ret: %d"), ret);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);


	m_videoFrameObserver.SetMsgReceiver(m_hWnd);
	RegisterVideoFrameObserver(TRUE, &m_videoFrameObserver);
	m_faceInfoObserver.SetMsgReceiver(m_hWnd);
	RegisterFaceInfoObserver(TRUE, &m_faceInfoObserver);

	return true;
}


//UnInitialize the Agora SDK
void CAgoraFaceCaptureDlg::UnInitAgora()
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
		RegisterVideoFrameObserver(FALSE);
		m_videoFrameObserver.SetMsgReceiver(nullptr);
		RegisterFaceInfoObserver(FALSE);
		m_faceInfoObserver.SetMsgReceiver(nullptr);
		//release engine.
		if (m_initialize) {
			m_rtcEngine->release(true);
			m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("release rtc engine"));
		}
		m_rtcEngine = NULL;
	}
}

//render local video from SDK local capture.
void CAgoraFaceCaptureDlg::RenderLocalVideo()
{
	if (m_rtcEngine) {

		VideoCanvas canvas;
		canvas.renderMode = media::base::RENDER_MODE_FIT;
		canvas.uid = 0;
		canvas.sourceType = VIDEO_SOURCE_CAMERA_PRIMARY;
		canvas.view = m_localVideoWnd.GetSafeHwnd();
		//setup local video in the engine to canvas.
		m_rtcEngine->setupLocalVideo(canvas);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("setupLocalVideo"));
	}
}


//resume window status
void CAgoraFaceCaptureDlg::ResumeStatus()
{
	InitCtrlText();
	m_staDetail.SetWindowText(_T(""));
	m_edtChannel.SetWindowText(_T(""));
	m_edtMetadataInfo.SetWindowText(_T(""));
	m_btnJoinChannel.EnableWindow(TRUE);
	m_lstInfo.ResetContent();
	m_joinChannel = false;
	m_initialize = false;
}

void CAgoraFaceCaptureDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_VIDEO, m_staVideoArea);
	DDX_Control(pDX, IDC_STATIC_CHANNELNAME, m_staChannel);
	DDX_Control(pDX, IDC_EDIT_CHANNELNAME, m_edtChannel);
	DDX_Control(pDX, IDC_EDIT_METADATA_INFO, m_edtMetadataInfo);
	DDX_Control(pDX, IDC_BUTTON_JOINCHANNEL, m_btnJoinChannel);
	DDX_Control(pDX, IDC_LIST_INFO_BROADCASTING, m_lstInfo);
	DDX_Control(pDX, IDC_STATIC_DETAIL, m_staDetail);
}


BEGIN_MESSAGE_MAP(CAgoraFaceCaptureDlg, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_MESSAGE(WM_MSGID(EID_JOINCHANNEL_SUCCESS), &CAgoraFaceCaptureDlg::OnEIDJoinChannelSuccess)
	ON_MESSAGE(WM_MSGID(EID_LEAVE_CHANNEL), &CAgoraFaceCaptureDlg::OnEIDLeaveChannel)
	ON_MESSAGE(WM_MSGID(EID_USER_JOINED), &CAgoraFaceCaptureDlg::OnEIDUserJoined)
	ON_MESSAGE(WM_MSGID(EID_USER_OFFLINE), &CAgoraFaceCaptureDlg::OnEIDUserOffline)
	ON_MESSAGE(WM_MSGID(EID_REMOTE_VIDEO_STATE_CHANGED), &CAgoraFaceCaptureDlg::OnEIDRemoteVideoStateChanged)
	ON_BN_CLICKED(IDC_BUTTON_JOINCHANNEL, &CAgoraFaceCaptureDlg::OnBnClickedButtonJoinchannel)
	ON_MESSAGE(WM_MSGID(EID_EXTENSION_EVENT), &CAgoraFaceCaptureDlg::OnEIDExtensionEvent)
	ON_MESSAGE(WM_MSGID(EID_ON_CAPTURE_VIDEOFRAME), &CAgoraFaceCaptureDlg::OnEIDonCaptureVideoFrame)
	ON_MESSAGE(WM_MSGID(EID_ON_FACE_INFO), &CAgoraFaceCaptureDlg::OnEIDonFaceInfo)
	ON_LBN_SELCHANGE(IDC_LIST_INFO_BROADCASTING, &CAgoraFaceCaptureDlg::OnSelchangeListInfoBroadcasting)
END_MESSAGE_MAP()




void CAgoraFaceCaptureDlg::OnShowWindow(BOOL bShow, UINT nStatus)
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


BOOL CAgoraFaceCaptureDlg::OnInitDialog()
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


/*
	register or unregister agora video Frame Observer.
*/
BOOL CAgoraFaceCaptureDlg::RegisterVideoFrameObserver(BOOL bEnable, agora::media::IVideoFrameObserver * videoFrameObserver)
{
	agora::util::AutoPtr<agora::media::IMediaEngine> mediaEngine;
	//query interface agora::AGORA_IID_MEDIA_ENGINE in the engine.
	mediaEngine.queryInterface(m_rtcEngine, AGORA_IID_MEDIA_ENGINE);
	int nRet = 0;
	if (mediaEngine.get() == NULL)
		return FALSE;
	if (bEnable) {
		//register agora video frame observer.
		nRet = mediaEngine->registerVideoFrameObserver(videoFrameObserver);
	}
	else {
		//unregister agora video frame observer.
		nRet = mediaEngine->registerVideoFrameObserver(nullptr);
	}
	return nRet == 0 ? TRUE : FALSE;
}

BOOL CAgoraFaceCaptureDlg::RegisterFaceInfoObserver(BOOL bEnable, agora::media::IFaceInfoObserver* faceInfoObserver)
{
	agora::util::AutoPtr<agora::media::IMediaEngine> mediaEngine;
	//query interface agora::AGORA_IID_MEDIA_ENGINE in the engine.
	mediaEngine.queryInterface(m_rtcEngine, AGORA_IID_MEDIA_ENGINE);
	int nRet = 0;
	if (mediaEngine.get() == NULL)
		return FALSE;
	if (bEnable) {
		//register agora video frame observer.
		nRet = mediaEngine->registerFaceInfoObserver(faceInfoObserver);
	}
	else {
		//unregister agora video frame observer.
		nRet = mediaEngine->registerFaceInfoObserver(nullptr);
	}
	return nRet == 0 ? TRUE : FALSE;
}

//click button handler to join channel or leave channel.
void CAgoraFaceCaptureDlg::OnBnClickedButtonJoinchannel()
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

void CAgoraFaceCaptureDlg::OnSelchangeListInfoBroadcasting() {
	int sel = m_lstInfo.GetCurSel();
	if (sel < 0)return;
	CString strDetail;
	m_lstInfo.GetText(sel, strDetail);
	m_staDetail.SetWindowText(strDetail);
}

//EID_JOINCHANNEL_SUCCESS message window handler
LRESULT CAgoraFaceCaptureDlg::OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam)
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
LRESULT CAgoraFaceCaptureDlg::OnEIDLeaveChannel(WPARAM wParam, LPARAM lParam)
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
LRESULT CAgoraFaceCaptureDlg::OnEIDUserJoined(WPARAM wParam, LPARAM lParam)
{
	CString strInfo;
	strInfo.Format(_T("%u joined"), wParam);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	return 0;
}

//EID_USER_OFFLINE message handler.
LRESULT CAgoraFaceCaptureDlg::OnEIDUserOffline(WPARAM wParam, LPARAM lParam)
{
	uid_t remoteUid = (uid_t)wParam;
	CString strInfo;
	strInfo.Format(_T("%u offline, reason:%d"), remoteUid, lParam);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	return 0;
}

//EID_REMOTE_VIDEO_STATE_CHANGED message window handler.
LRESULT CAgoraFaceCaptureDlg::OnEIDRemoteVideoStateChanged(WPARAM wParam, LPARAM lParam)
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


LRESULT CAgoraFaceCaptureDlg::OnEIDExtensionEvent(WPARAM wParam, LPARAM lParam)
{
	PExtensionEvent event = (PExtensionEvent)wParam;
	
	CString strInfo;
	strInfo.Format(_T("OnEIDExtensionEvent: provicder=%s, extension=%s, key=%s, value=%s"), 
		utf82cs(event->provider), 
		utf82cs(event->extension), 
		utf82cs(event->key), 
		utf82cs(event->value));
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	if (strcmp(event->provider, "agora_video_filters_face_capture") == 0
		&& strcmp(event->extension, "face_capture") == 0
		&& strcmp(event->key, "authentication_state") == 0) {
		if (strcmp(event->value, "0") == 0) {
			// authentication successful.
			strInfo.Format(_T("Face caputure authentication successful."));
		}
		else if (strcmp(event->value, "-1") == 0) {
			// authentication failure.
			strInfo.Format(_T("Face caputure authentication failure."));
		}
		else if (strcmp(event->value, "-2") == 0) {
			// authentication unset.
			strInfo.Format(_T("Face caputure authentication unset."));
		}
	}
	else if (strcmp(event->provider, "agora_filters_lip_sync") == 0
		&& strcmp(event->extension, "lip_sync") == 0
		&& strcmp(event->key, "status_code") == 0) {
		if (strcmp(event->value, "0") == 0) {
			// authentication successful.
			strInfo.Format(_T("Speech driven authentication successful."));
		}
		else {
			strInfo.Format(_T("Speech driven authentication failed. code=%s"), event->value);
		}
	}
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

	delete[] event->provider;
	delete[] event->extension;
	delete[] event->key;
	delete[] event->value;
	delete event;
	return 0;
}


LRESULT CAgoraFaceCaptureDlg::OnEIDonCaptureVideoFrame(WPARAM wParam, LPARAM lParam)
{
	char* metedata = (char*)wParam;

	// m_edtMetadataInfo.SetWindowTextW(utf82cs(std::string(metedata)));

	delete[] metedata;
	return 0;
}

LRESULT CAgoraFaceCaptureDlg::OnEIDonFaceInfo(WPARAM wParam, LPARAM lParam)
{
	char* faceInfo = (char*)wParam;

	m_edtMetadataInfo.SetWindowTextW(utf82cs(std::string(faceInfo)));
	delete[] faceInfo;
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
void FaceCaptureEventHandler::onJoinChannelSuccess(const char* channel, uid_t uid, int elapsed)
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
	by the callback（ms).
*/
void FaceCaptureEventHandler::onUserJoined(uid_t uid, int elapsed)
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
void FaceCaptureEventHandler::onUserOffline(uid_t uid, USER_OFFLINE_REASON_TYPE reason)
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

void FaceCaptureEventHandler::onLeaveChannel(const RtcStats& stats)
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
void FaceCaptureEventHandler::onRemoteVideoStateChanged(uid_t uid, REMOTE_VIDEO_STATE state, REMOTE_VIDEO_STATE_REASON reason, int elapsed)
{
	if (m_hMsgHanlder) {
		PVideoStateStateChanged stateChanged = new VideoStateStateChanged;
		stateChanged->uid = uid;
		stateChanged->reason = reason;
		stateChanged->state = state;
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_REMOTE_VIDEO_STATE_CHANGED), (WPARAM)stateChanged, 0);
	}
}

void FaceCaptureEventHandler::onExtensionEventWithContext(const ExtensionContext& context, const char* key, const char* value)
{
	if (m_hMsgHanlder) {

		const char* provider = context.providerName;
		const char* extension = context.extensionName;

		PExtensionEvent evnet = new ExtensionEvent();
		int len = strlen(provider);
		evnet->provider = new char[len + 1];
		evnet->provider[len] = 0;
		strcpy_s(evnet->provider, len + 1, provider);

	
		len = strlen(extension);
		evnet->extension = new char[len + 1];
		evnet->extension[len] = 0;
		strcpy_s(evnet->extension, len + 1, extension);

		len = strlen(key);
		evnet->key = new char[len + 1];
		evnet->key[len] = 0;
		strcpy_s(evnet->key, len + 1, key);

		len = strlen(value);
		evnet->value = new char[len + 1];
		evnet->value[len] = 0;
		strcpy_s(evnet->value, len + 1, value);

		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_EXTENSION_EVENT), (WPARAM)evnet, 0);
	}
}

bool FaceCaptureVideoFrameObserver::onCaptureVideoFrame(agora::rtc::VIDEO_SOURCE_TYPE type, VideoFrame& videoFrame) {
	if (m_hMsgHanlder) {
		
		if (videoFrame.metaInfo != NULL) {
			const char* metaInfo = videoFrame.metaInfo->getMetaInfoStr(media::base::IVideoFrameMetaInfo::META_INFO_KEY::KEY_FACE_CAPTURE);
			if (metaInfo != NULL) {
				int len = strlen(metaInfo);
				char* out = new char[len + 1];
				out[len] = 0;
				strcpy_s(out, len + 1, metaInfo);
				::PostMessage(m_hMsgHanlder, WM_MSGID(EID_ON_CAPTURE_VIDEOFRAME), (WPARAM)out, 0);
			}
		}
		
	}
	return true;
}

bool FaceCaptureFaceInfoObserver::onFaceInfo(const char* outFaceInfo) {
	if (m_hMsgHanlder && outFaceInfo) {

		int len = strlen(outFaceInfo);
		char* out = new char[len + 1];
		out[len] = 0;
		strcpy_s(out, len + 1, outFaceInfo);
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_ON_FACE_INFO), (WPARAM)out, 0);

	}
	return true;
}


BOOL CAgoraFaceCaptureDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) {
		return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}



