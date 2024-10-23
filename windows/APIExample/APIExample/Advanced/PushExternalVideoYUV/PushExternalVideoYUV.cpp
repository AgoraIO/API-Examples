#include "stdafx.h"
#include "APIExample.h"
#include "PushExternalVideoYUV.h"

BEGIN_MESSAGE_MAP(PushExternalVideoYUV, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_MESSAGE(WM_MSGID(EID_JOINCHANNEL_SUCCESS), &PushExternalVideoYUV::OnEIDJoinChannelSuccess)
	ON_MESSAGE(WM_MSGID(EID_LEAVE_CHANNEL), &PushExternalVideoYUV::OnEIDLeaveChannel)
	ON_MESSAGE(WM_MSGID(EID_USER_JOINED), &PushExternalVideoYUV::OnEIDUserJoined)
	ON_MESSAGE(WM_MSGID(EID_USER_OFFLINE), &PushExternalVideoYUV::OnEIDUserOffline)
	ON_MESSAGE(WM_MSGID(EID_REMOTE_VIDEO_STATE_CHANGED), &PushExternalVideoYUV::OnEIDRemoteVideoStateChanged)
	ON_BN_CLICKED(IDC_BUTTON_JOINCHANNEL, &PushExternalVideoYUV::OnClickedButtonJoinchannel)
	ON_BN_CLICKED(IDC_CHECK_HDR, &PushExternalVideoYUV::OnBnClickedCheckHdr)
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(PushExternalVideoYUV, CDialogEx)

PushExternalVideoYUV::PushExternalVideoYUV(CWnd* pParent /*=nullptr*/)
	: CDialogEx(PushExternalVideoYUV::IDD, pParent)
{

}

PushExternalVideoYUV::~PushExternalVideoYUV()
{

}

void PushExternalVideoYUV::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_VIDEO, m_staVideoArea);
	DDX_Control(pDX, IDC_STATIC_CHANNELNAME, m_staChannelName);
	DDX_Control(pDX, IDC_EDIT_CHANNELNAME, m_edtChannel);
	DDX_Control(pDX, IDC_BUTTON_JOINCHANNEL, m_btnJoinChannel);
	DDX_Control(pDX, IDC_LIST_INFO_BROADCASTING, m_lstInfo);
	DDX_Control(pDX, IDC_CHECK_HDR, mCbHdr);
}

//set control text from config.
void PushExternalVideoYUV::InitCtrlText()
{
	m_staChannelName.SetWindowText(commonCtrlChannel);
	m_btnJoinChannel.SetWindowText(commonCtrlJoinChannel);
}

/*
	create Agora RTC Engine and initialize context.set channel property.
*/
bool PushExternalVideoYUV::InitAgora()
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

	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("live broadcasting"));
	//set client role in the engine to the CLIENT_ROLE_BROADCASTER.
	m_rtcEngine->setClientRole(CLIENT_ROLE_BROADCASTER);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("setClientRole broadcaster"));
	m_rtcEngine->enableVideo();


	//query interface agora::AGORA_IID_MEDIA_ENGINE in the engine.
	m_mediaEngine.queryInterface(m_rtcEngine, agora::rtc::AGORA_IID_MEDIA_ENGINE);
	if (m_mediaEngine.get() == NULL)
		return FALSE;

	ret = m_mediaEngine->setExternalVideoSource(true, false);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("setExternalVideoSource enable=true, useTexture=false"));


	VideoEncoderConfiguration config;
	//config.dimensions.width = YUVReader::VIDEO_WIDTH;
	//config.dimensions.height = YUVReader::VIDEO_HEIGHT;
	config.dimensions.width = WIDHT_HDR;
	config.dimensions.height = HEIGHT_HDR;
	//set video encoder configuration.
	m_rtcEngine->setVideoEncoderConfiguration(config);


	m_videoFrame.stride = YUVReader::VIDEO_WIDTH;
	m_videoFrame.height = YUVReader::VIDEO_HEIGHT;
	m_videoFrame.rotation = 0;
	m_videoFrame.cropBottom = 0;
	m_videoFrame.cropLeft = 0;
	m_videoFrame.cropRight = 0;
	m_videoFrame.cropTop = 0;
	m_videoFrame.format = agora::media::base::VIDEO_PIXEL_I420;
	m_videoFrame.type = agora::media::base::ExternalVideoFrame::VIDEO_BUFFER_TYPE::VIDEO_BUFFER_RAW_DATA;
	m_imgBuffer = new BYTE[YUVReader::VIDEO_FRAME_SIZE]{ 0 };

	m_imgBuffer16 = new uint16_t[WIDHT_HDR * HEIGHT_HDR * 1.5]{ 0 };
	CString videoUrl = GetExePath() + _T("\\hdr_1280_720.yuv");
	std::string tmp = cs2utf8(videoUrl);
	std::ifstream file(tmp, std::ios::binary);
	if (!file) {
		AfxMessageBox(_T("open hdr file failed"));
	}
	file.seekg(0, std::ios::end);
	std::streamsize fileSize = file.tellg();
	file.seekg(0, std::ios::beg);
	if (fileSize % sizeof(uint16_t) != 0) {
		AfxMessageBox(_T(" hdr file format is wrong"));
	}
	std::vector<char> fileBuffer(fileSize);
	if (!file.read(fileBuffer.data(), fileSize)) {
		AfxMessageBox(_T("read hdr file failed"));
	}
	file.close();
	uint16_t* uint16Buffer = reinterpret_cast<uint16_t*>(fileBuffer.data());
	memcpy_s(m_imgBuffer16, fileSize, uint16Buffer, fileSize);

	m_rtcEngine->startPreview(VIDEO_SOURCE_CUSTOM);
	return true;
}

/*
	stop and release agora rtc engine.
*/
void PushExternalVideoYUV::UnInitAgora()
{
	if (m_rtcEngine) {
		yuvReader.stop();
		if (m_imgBuffer) {
			delete[] m_imgBuffer;
			m_imgBuffer = nullptr;
		}
		if (m_imgBuffer16) {
			delete[] m_imgBuffer16;
			m_imgBuffer16 = nullptr;
		}

		if (m_joinChannel)
			m_joinChannel = !m_rtcEngine->leaveChannel();
		VideoCanvas canvas;
		canvas.sourceType = VIDEO_SOURCE_CUSTOM;
		m_rtcEngine->setupLocalVideo(canvas);
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


/*
	initialize dialog, and set control property.
*/
BOOL PushExternalVideoYUV::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	RECT rcArea;
	m_staVideoArea.GetClientRect(&rcArea);

	m_localVideoWnd.Create(NULL, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CRect(0, 0, 1, 1), this, ID_BASEWND_VIDEO + 100);
	m_localVideoWnd.MoveWindow(rcArea.left, rcArea.top, (rcArea.right - rcArea.left) / 2, rcArea.bottom - rcArea.top, TRUE);

	m_remoteVideoWnd.Create(NULL, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CRect(0, 0, 1, 1), this, ID_BASEWND_VIDEO + 101);
	m_remoteVideoWnd.MoveWindow(rcArea.left + (rcArea.right - rcArea.left) / 2, rcArea.top, (rcArea.right - rcArea.left) / 2, rcArea.bottom - rcArea.top, TRUE);

	ResumeStatus();
	return TRUE;
}


// resume window status.
void PushExternalVideoYUV::ResumeStatus()
{
	m_lstInfo.ResetContent();
	InitCtrlText();
	m_joinChannel = false;
	m_initialize = false;
	m_edtChannel.SetWindowText(_T(""));
	m_remoteVideoWnd.Reset();
	m_remoteVideoWnd.SetUID(0);
	m_localVideoWnd.Reset();
	m_localVideoWnd.SetUID(0);
	mCbHdr.SetCheck(FALSE);
	isUseHdr = false;
}




/*
	Enumerate all the video capture devices and add to the combo box.
*/
void PushExternalVideoYUV::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);
	if (bShow) {
		//init control text.
		InitCtrlText();
	}
	else {
		//resume window status.
		ResumeStatus();
	}
}


//The JoinChannel button's click handler.
//This function either joins or leaves the channel
void PushExternalVideoYUV::OnClickedButtonJoinchannel()
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

		VideoCanvas canvas;
		canvas.renderMode = media::base::RENDER_MODE_FIT;
		canvas.uid = 0;
		canvas.view = m_localVideoWnd.GetSafeHwnd();
		canvas.sourceType = VIDEO_SOURCE_CUSTOM;
		canvas.mirrorMode = VIDEO_MIRROR_MODE_DISABLED;
		//setup local video in the engine to canvas.
		m_rtcEngine->setupLocalVideo(canvas);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("render local video"));

		std::string szChannelId = cs2utf8(strChannelName);
		ChannelMediaOptions options;
		options.channelProfile = CHANNEL_PROFILE_LIVE_BROADCASTING;
		options.clientRoleType = CLIENT_ROLE_BROADCASTER;
		options.autoSubscribeVideo = true;
		//join channel in the engine.
		if (0 == m_rtcEngine->joinChannel(APP_TOKEN, szChannelId.c_str(), 0, options)) {
			strInfo.Format(_T("join channel %s, use ChannelMediaOptions"), getCurrentTime());
			m_btnJoinChannel.EnableWindow(FALSE);

			yuvReader.start(std::bind(&PushExternalVideoYUV::OnYUVRead, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
		}
	}
	else {
		//leave channel in the engine.
		if (0 == m_rtcEngine->leaveChannel()) {
			strInfo.Format(_T("leave channel %s"), getCurrentTime());
			mCbHdr.SetCheck(FALSE);
			isUseHdr = false;
			yuvReader.stop();
			m_remoteVideoWnd.Reset();
			m_remoteVideoWnd.SetUID(0);
			m_localVideoWnd.Reset();
			m_localVideoWnd.SetUID(0);
			VideoCanvas canvas;
			canvas.sourceType = VIDEO_SOURCE_CUSTOM;
			m_rtcEngine->setupLocalVideo(canvas);
		}
	}
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
}

void PushExternalVideoYUV::OnYUVRead(int width, int height, unsigned char* buffer, int size) {
	agora::media::base::ColorSpace colorSpace;
	if (isUseHdr)
	{
		m_videoFrame.buffer = m_imgBuffer16;
		colorSpace.range= agora::media::base::ColorSpace::RangeID::RANGEID_LIMITED;
		colorSpace.transfer = agora::media::base::ColorSpace::TransferID::TRANSFERID_ARIB_STD_B67;
		colorSpace.matrix = agora::media::base::ColorSpace::MatrixID::MATRIXID_BT2020_NCL;
		colorSpace.primaries = agora::media::base::ColorSpace::PrimaryID::PRIMARYID_BT2020;
		m_videoFrame.colorSpace = colorSpace;
		m_videoFrame.stride = WIDHT_HDR;
		m_videoFrame.height = HEIGHT_HDR;
		m_videoFrame.timestamp = m_rtcEngine->getCurrentMonotonicTimeInMs();
		m_videoFrame.format = agora::media::base::VIDEO_PIXEL_I010;
	}
	else
	{
		memcpy_s(m_imgBuffer, size, buffer, size);
		/*colorSpace.range = agora::media::base::ColorSpace::RangeID::RANGEID_FULL;
		colorSpace.transfer = agora::media::base::ColorSpace::TransferID::TRANSFERID_BT709;
		colorSpace.matrix = agora::media::base::ColorSpace::MatrixID::MATRIXID_BT709;
		colorSpace.primaries = agora::media::base::ColorSpace::PrimaryID::PRIMARYID_BT709;*/
		m_videoFrame.colorSpace = colorSpace;
		m_videoFrame.stride = width;
		m_videoFrame.height = height;
		m_videoFrame.buffer = m_imgBuffer;
		m_videoFrame.timestamp = m_rtcEngine->getCurrentMonotonicTimeInMs();
		m_videoFrame.format = agora::media::base::VIDEO_PIXEL_I420;
	}

	if (m_joinChannel && m_mediaEngine.get() != nullptr) {
		m_mediaEngine->pushVideoFrame(&m_videoFrame);
	}
}

BOOL PushExternalVideoYUV::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) {
		return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}

// PushExternalVideoYUVEventHandler event

//EID_JOINCHANNEL_SUCCESS message window handler.
LRESULT PushExternalVideoYUV::OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam)
{
	m_joinChannel = true;
	m_btnJoinChannel.EnableWindow(TRUE);

	m_btnJoinChannel.SetWindowText(commonCtrlLeaveChannel);
	CString strInfo;
	strInfo.Format(_T("%s:join success, uid=%u"), getCurrentTime(), wParam);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	m_localVideoWnd.SetUID(wParam);

	//notify parent window
	::PostMessage(GetParent()->GetSafeHwnd(), WM_MSGID(EID_JOINCHANNEL_SUCCESS), TRUE, 0);
	return 0;
}

//EID_LEAVE_CHANNEL message window handler.
LRESULT PushExternalVideoYUV::OnEIDLeaveChannel(WPARAM wParam, LPARAM lParam)
{
	m_joinChannel = false;
	m_btnJoinChannel.SetWindowText(commonCtrlJoinChannel);
	CString strInfo;
	strInfo.Format(_T("leave channel success %s"), getCurrentTime());
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	::PostMessage(GetParent()->GetSafeHwnd(), WM_MSGID(EID_JOINCHANNEL_SUCCESS), FALSE, 0);
	return 0;
}

//EID_USER_JOINED message window handler.
LRESULT PushExternalVideoYUV::OnEIDUserJoined(WPARAM wParam, LPARAM lParam)
{
	int remoteUid = wParam;
	CString strInfo;
	strInfo.Format(_T("%u joined"), remoteUid);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

	if (m_remoteVideoWnd.GetUID() == 0) {
		m_remoteVideoWnd.SetUID(remoteUid);
		VideoCanvas canvas;
		canvas.view = m_remoteVideoWnd.GetSafeHwnd();
		canvas.uid = remoteUid;
		canvas.renderMode = media::base::RENDER_MODE_FIT;
		m_rtcEngine->setupRemoteVideo(canvas);
	}
	return 0;
}


//EID_USER_OFFLINE message window handler.
LRESULT PushExternalVideoYUV::OnEIDUserOffline(WPARAM wParam, LPARAM lParam)
{
	uid_t remoteUid = (uid_t)wParam;
	CString strInfo;
	strInfo.Format(_T("%u offline, reason:%d"), remoteUid, lParam);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

	if (m_remoteVideoWnd.GetUID() == remoteUid) {
		m_remoteVideoWnd.SetUID(0);
		m_remoteVideoWnd.Reset();
	}

	return 0;
}

//EID_REMOTE_VIDEO_STATE_CHANGED message window handler.
LRESULT PushExternalVideoYUV::OnEIDRemoteVideoStateChanged(WPARAM wParam, LPARAM lParam)
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


// PushExternalVideoYUVEventHandler implements


/*
note:
	Join the channel callback.This callback method indicates that the client
	successfully joined the specified channel.Channel ids are assigned based
	on the channel name specified in the joinChannel. If IRtcEngine::joinChannel
	is called without a user ID specified. The server will automatically assign one
parameters:
	channel:channel name.
	uid: user ID��If the UID is specified in the joinChannel, that ID is returned here;
	Otherwise, use the ID automatically assigned by the Agora server.
	elapsed: The Time from the joinChannel until this event occurred (ms).
*/
void PushExternalVideoYUVEventHandler::onJoinChannelSuccess(const char* channel, uid_t uid, int elapsed)
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
	by the callback��ms).
*/
void PushExternalVideoYUVEventHandler::onUserJoined(uid_t uid, int elapsed)
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
void PushExternalVideoYUVEventHandler::onUserOffline(uid_t uid, USER_OFFLINE_REASON_TYPE reason)
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
void PushExternalVideoYUVEventHandler::onLeaveChannel(const RtcStats& stats)
{
	if (m_hMsgHanlder) {
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_LEAVE_CHANNEL), 0, 0);
	}
}

/**
	Occurs when the remote video state changes.
	@note This callback does not work properly when the number of users
	(in the Communication profile) or broadcasters (in the Live-broadcast profile)
	in the channel exceeds 17.
	@param uid ID of the remote user whose video state changes.
	@param state State of the remote video. See #REMOTE_VIDEO_STATE.
	@param reason The reason of the remote video state change. See
	#REMOTE_VIDEO_STATE_REASON.
	@param elapsed Time elapsed (ms) from the local user calling the
	agora::rtc::IRtcEngine::joinChannel "joinChannel" method until the
	SDK triggers this callback.
*/
void PushExternalVideoYUVEventHandler::onRemoteVideoStateChanged(uid_t uid, REMOTE_VIDEO_STATE state, REMOTE_VIDEO_STATE_REASON reason, int elapsed)
{
	if (m_hMsgHanlder) {
		PVideoStateStateChanged stateChanged = new VideoStateStateChanged;
		stateChanged->uid = uid;
		stateChanged->reason = reason;
		stateChanged->state = state;
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_REMOTE_VIDEO_STATE_CHANGED), (WPARAM)stateChanged, 0);
	}
}


void PushExternalVideoYUV::OnBnClickedCheckHdr()
{
	agora::rtc::HDR_CAPABILITY capability = agora::rtc::HDR_CAPABILITY::HDR_CAPABILITY_UNKNOWN;

	m_rtcEngine->queryHDRCapability(VIDEO_MODULE_HARDWARE_ENCODER, capability);
	if (capability == HDR_CAPABILITY::HDR_CAPABILITY_SUPPORTED) {
		isUseHdr = !isUseHdr;
	}
	else {
		isUseHdr = false;
		mCbHdr.SetCheck(FALSE);
		MessageBox(_T("The current device does not support HDR"));
	}

}
