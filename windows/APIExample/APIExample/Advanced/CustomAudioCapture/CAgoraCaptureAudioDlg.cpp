#include "stdafx.h"
#include "APIExample.h"
#include "CAgoraCaptureAudioDlg.h"
#include "DirectShow/CircleBuffer.hpp"

IMPLEMENT_DYNAMIC(CAgoraCaptureAduioDlg, CDialogEx)


//EID_JOINCHANNEL_SUCCESS message window handler
LRESULT CAgoraCaptureAduioDlg::OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam)
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
LRESULT CAgoraCaptureAduioDlg::OnEIDLeaveChannel(WPARAM wParam, LPARAM lParam)
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
LRESULT CAgoraCaptureAduioDlg::OnEIDUserJoined(WPARAM wParam, LPARAM lParam)
{
	CString strInfo;
	strInfo.Format(_T("%u joined"), wParam);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	return 0;
}

//EID_USER_OFFLINE message handler.
LRESULT CAgoraCaptureAduioDlg::OnEIDUserOffline(WPARAM wParam, LPARAM lParam)
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

//EID_REMOTE_VIDEO_STATE_CHANGED message window handler.
LRESULT CAgoraCaptureAduioDlg::OnEIDRemoteVideoStateChanged(WPARAM wParam, LPARAM lParam)
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

CAgoraCaptureAduioDlg::CAgoraCaptureAduioDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_CUSTOM_CAPTURE_AUDIO, pParent)
	, m_capAudioInfo{48000, 2}
	, m_renderAudioInfo {48000, 2}
{

}

CAgoraCaptureAduioDlg::~CAgoraCaptureAduioDlg()
{

}

/*
	create Agora Rtc Engine and initialize context.set channel property.
*/
bool CAgoraCaptureAduioDlg::InitAgora()
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

	mediaEngine.queryInterface(m_rtcEngine, AGORA_IID_MEDIA_ENGINE);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("initialize success"));
	//enable video in the engine.
	m_rtcEngine->enableVideo();
	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("enable video"));
	//enable audio in the engine.
	//m_rtcEngine->enableAudio();
	//m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("enable audio"));
	//set channel profile in the engine to the CHANNEL_PROFILE_LIVE_BROADCASTING.
	m_rtcEngine->setChannelProfile(CHANNEL_PROFILE_LIVE_BROADCASTING);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("live broadcasting"));
	//set client role in the engine to the CLIENT_ROLE_BROADCASTER.
	m_rtcEngine->setClientRole(CLIENT_ROLE_BROADCASTER);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("setClientRole broadcaster"));

	m_agAudioCaptureDevice.engine_ = m_rtcEngine;
	return true;
}


/*
	UnInitialize the Agora SDK
*/
void CAgoraCaptureAduioDlg::UnInitAgora()
{
	if (m_rtcEngine) {
		//leave channel.
		if (m_joinChannel)
			m_joinChannel = !m_rtcEngine->leaveChannel();
		//stop preview in the engine.
		m_rtcEngine->stopPreview();
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("stopPreview"));
		//disable video in the engine.
		m_rtcEngine->disableVideo();
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("disableVideo"));
		m_agAudioCaptureDevice.Stop();
		m_agAudioCaptureDevice.engine_ = NULL;
		//release engine.
		if (m_initialize) {
			m_rtcEngine->release(true);
			m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("release rtc engine"));
		}
		m_rtcEngine = NULL;
	}
}

/*
	render local video from SDK local capture.
*/
void CAgoraCaptureAduioDlg::RenderLocalVideo()
{
	if (m_rtcEngine) {
		//start preview in the engine.
		m_rtcEngine->startPreview();
		VideoCanvas canvas;
		canvas.renderMode = media::base::RENDER_MODE_FIT;
		canvas.uid = 0;
		canvas.view = m_localVideoWnd.GetSafeHwnd();
		//setup local video in the engine to canvas.
		m_rtcEngine->setupLocalVideo(canvas);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("render local video"));
	}
}

//set control text from config.
void CAgoraCaptureAduioDlg::InitCtrlText()
{
	m_btnJoinChannel.SetWindowText(commonCtrlJoinChannel);
	m_staChannel.SetWindowText(commonCtrlChannel);
	m_staAudioDevice.SetWindowText(customAudioCaptureCtrlCaptureAudioDeivce);
	m_btnSetAudioCtx.SetWindowText(customAudioCaptureCtrlSetExternlCapture);
}

void CAgoraCaptureAduioDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_JOINCHANNEL, m_btnJoinChannel);
	DDX_Control(pDX, IDC_BUTTON_START_CAPUTRE, m_btnSetAudioCtx);
	DDX_Control(pDX, IDC_COMBO_CAPTURE_AUDIO_DEVICE, m_cmbAudioDevice);
	DDX_Control(pDX, IDC_COMBO_CAPTURE_AUDIO_TYPE, m_cmbAudioType);
	DDX_Control(pDX, IDC_STATIC_CAPTUREDEVICE, m_staAudioDevice);
	DDX_Control(pDX, IDC_STATIC_CHANNELNAME, m_staChannel);
	DDX_Control(pDX, IDC_EDIT_CHANNELNAME, m_edtChannel);
	DDX_Control(pDX, IDC_STATIC_VIDEO, m_staVideoArea);
	DDX_Control(pDX, IDC_LIST_INFO_BROADCASTING, m_lstInfo);
	DDX_Control(pDX, IDC_BUTTON_RENDER_AUDIO, m_btnSetAudioRender);
}


BEGIN_MESSAGE_MAP(CAgoraCaptureAduioDlg, CDialogEx)
	ON_MESSAGE(WM_MSGID(EID_JOINCHANNEL_SUCCESS), &CAgoraCaptureAduioDlg::OnEIDJoinChannelSuccess)
	ON_MESSAGE(WM_MSGID(EID_LEAVE_CHANNEL), &CAgoraCaptureAduioDlg::OnEIDLeaveChannel)
	ON_MESSAGE(WM_MSGID(EID_USER_JOINED), &CAgoraCaptureAduioDlg::OnEIDUserJoined)
	ON_MESSAGE(WM_MSGID(EID_USER_OFFLINE), &CAgoraCaptureAduioDlg::OnEIDUserOffline)
	ON_MESSAGE(WM_MSGID(EID_REMOTE_VIDEO_STATE_CHANGED), &CAgoraCaptureAduioDlg::OnEIDRemoteVideoStateChanged)
	ON_BN_CLICKED(IDC_BUTTON_JOINCHANNEL, &CAgoraCaptureAduioDlg::OnBnClickedButtonJoinchannel)
	ON_BN_CLICKED(IDC_BUTTON_START_CAPUTRE, &CAgoraCaptureAduioDlg::OnBnClickedButtonStartCaputre)
	ON_WM_SHOWWINDOW()
	ON_CBN_SELCHANGE(IDC_COMBO_CAPTURE_AUDIO_DEVICE, &CAgoraCaptureAduioDlg::OnSelchangeComboCaptureAudioDevice)
	ON_BN_CLICKED(IDC_BUTTON_RENDER_AUDIO, &CAgoraCaptureAduioDlg::OnBnClickedButtonRenderAudio)
END_MESSAGE_MAP()



//The JoinChannel button's click handler.
//This function either joins or leaves the channel
void CAgoraCaptureAduioDlg::OnBnClickedButtonJoinchannel()
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
		ChannelMediaOptions option;
		option.publishCustomAudioTrack = true;
		option.publishCameraTrack = true;
		option.autoSubscribeAudio = true;
		option.autoSubscribeVideo = true;
		//join channel in the engine.
		if (0 == m_rtcEngine->joinChannel(APP_TOKEN, szChannelId.c_str(),0, option)) {
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

// start or stop capture.
// if bEnable is true start capture otherwise stop capture.
void CAgoraCaptureAduioDlg::EnableCaputre(BOOL bEnable) {
	
	SIZE_T			nBufferSize = 0;
	if (bEnable == (BOOL)m_extenalCaptureAudio)
		return;
	if (bEnable)
	{
		//select media capture.
		m_agAudioCaptureDevice.SelectMediaCap(m_cmbAudioType.GetCurSel());
		//get current audio capture format.
		m_agAudioCaptureDevice.InitAudioFrame();
		//create audio capture filter.
		if (!m_agAudioCaptureDevice.CreateCaptureFilter())
			return;
		//start audio capture.
		m_agAudioCaptureDevice.Start();
	}
	else {
		//stop audio capture.
		m_agAudioCaptureDevice.Stop();
	}
	m_extenalCaptureAudio = !m_extenalCaptureAudio;
}

void CAgoraCaptureAduioDlg::PullAudioFrameThread(CAgoraCaptureAduioDlg * self)
{
	int nRet = 0;
	int interval = 10; // 10ms
	agora::util::AutoPtr<agora::media::IMediaEngine> mediaEngine;
	//query interface agora::AGORA_IID_MEDIA_ENGINE in the engine.
	mediaEngine.queryInterface(self->m_rtcEngine, AGORA_IID_MEDIA_ENGINE);
	IAudioFrameObserver::AudioFrame audioFrame;
	audioFrame.avsync_type = 0;//reserved 
	audioFrame.bytesPerSample = TWO_BYTES_PER_SAMPLE;
	audioFrame.type = agora::media::IAudioFrameObserver::FRAME_TYPE_PCM16;
	audioFrame.channels = self->m_renderAudioInfo.channels;
	audioFrame.samplesPerChannel = self->m_renderAudioInfo.sampleRate / (1000 / interval);
	audioFrame.samplesPerSec = self->m_renderAudioInfo.sampleRate;
	audioFrame.buffer = new BYTE[audioFrame.samplesPerChannel * audioFrame.channels * audioFrame.bytesPerSample];
	while (self->m_extenalRenderAudio )
	{
		nRet = mediaEngine->pullAudioFrame(&audioFrame);
		if (nRet != 0)
		{
			Sleep(interval);
			continue;
		}
		SIZE_T nSize = audioFrame.samplesPerChannel * audioFrame.channels * audioFrame.bytesPerSample;
		self->m_audioRender.Render((BYTE*)audioFrame.buffer, nSize);
	}
	delete audioFrame.buffer;
}



/*
	Start or stop collecting audio devices and 
	use external audio source.
*/
void CAgoraCaptureAduioDlg::OnBnClickedButtonStartCaputre()
{
	if ( !m_extenalCaptureAudio ){
		m_btnSetAudioCtx.SetWindowText(customAudioCaptureCtrlCancelExternlCapture);
		//use external audio source.
		EnableExtendAudioCapture(TRUE);
		//start capture
		EnableCaputre(TRUE);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("use external audio source"));
	//	m_agAudioCaptureDevice.SetCaptureDlg(this);
	}
	else {
		m_btnSetAudioCtx.SetWindowText(customAudioCaptureCtrlSetExternlCapture);
		//use inner audio source.
		EnableExtendAudioCapture(FALSE);
		//stop capture.
		EnableCaputre(FALSE);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("use inner audio source"));
	//	m_agAudioCaptureDevice.SetCaptureDlg(nullptr);
	}

}




/*
	use external audio source.
	sdk will not capture.
*/
BOOL CAgoraCaptureAduioDlg::EnableExtendAudioCapture(BOOL bEnable)
{
	agora::util::AutoPtr<agora::media::IMediaEngine> mediaEngine;
	//query interface agora::AGORA_IID_MEDIA_ENGINE in the engine.
	mediaEngine.queryInterface(m_rtcEngine, AGORA_IID_MEDIA_ENGINE);
	int nRet = 0;
	if ( bEnable )
		nRet = mediaEngine->setExternalAudioSource(true,m_agAudioCaptureDevice.m_audioFrame.samplesPerSec , m_agAudioCaptureDevice.m_audioFrame.channels);
	else
		nRet = mediaEngine->setExternalAudioSource(false, m_agAudioCaptureDevice.m_audioFrame.samplesPerSec, m_agAudioCaptureDevice.m_audioFrame.channels);
	return nRet == 0 ? TRUE : FALSE;
}

//enable external audio sink
BOOL CAgoraCaptureAduioDlg::EnableExternalRenderAudio(BOOL bEnable)
{
	int nRet = 0;
	if ( bEnable )
	{
		//set external audio sink
		nRet = mediaEngine->setExternalAudioSink(true, m_renderAudioInfo.sampleRate, m_renderAudioInfo.channels);
		m_audioRender.Init(GetSafeHwnd(), m_renderAudioInfo.sampleRate, m_renderAudioInfo.channels, m_renderAudioInfo.sampleByte * 8);
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)PullAudioFrameThread, this, 0, NULL);
	}
	else {
		//cancel external audio sink
		//sample rate and channels will not be used.so you can set any value.
		nRet = mediaEngine->setExternalAudioSink(false, 0, 0);
	}
	return nRet == 0 ? TRUE : FALSE;
}


//set external audio render click handler.
void CAgoraCaptureAduioDlg::OnBnClickedButtonRenderAudio()
{
	m_extenalRenderAudio = !m_extenalRenderAudio;
	if (m_extenalRenderAudio)
	{
		//set external render audio mode.
		EnableExternalRenderAudio(true);
		m_lstInfo.InsertString(m_lstInfo.GetCount(),_T("use external audio sink."));
		//m_btnSetAudioRender.SetWindowText(customAudioCaptureCtrlCancelAudioRender);
		m_btnSetAudioRender.SetWindowText(L"Stop Render");
	}
	else {
		//cancel external render audio mode.
		EnableExternalRenderAudio(false);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("disable external audio sink."));
		m_btnSetAudioRender.SetWindowText(L"Start Render");
		//m_btnSetAudioRender.SetWindowText(customAudioCaptureCtrlSetAudioRender);
	}
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
void CAgoraCaptureAduioDlgEngineEventHandler::onJoinChannelSuccess(const char* channel, uid_t uid, int elapsed)
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
void CAgoraCaptureAduioDlgEngineEventHandler::onUserJoined(uid_t uid, int elapsed)
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
void CAgoraCaptureAduioDlgEngineEventHandler::onUserOffline(uid_t uid, USER_OFFLINE_REASON_TYPE reason)
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
void CAgoraCaptureAduioDlgEngineEventHandler::onLeaveChannel(const RtcStats& stats)
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
void CAgoraCaptureAduioDlgEngineEventHandler::onRemoteVideoStateChanged(uid_t uid, REMOTE_VIDEO_STATE state, REMOTE_VIDEO_STATE_REASON reason, int elapsed)
{
	if (m_hMsgHanlder) {
		PVideoStateStateChanged stateChanged = new VideoStateStateChanged;
		stateChanged->uid = uid;
		stateChanged->reason = reason;
		stateChanged->state = state;
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_REMOTE_VIDEO_STATE_CHANGED), (WPARAM)stateChanged, 0);
	}
}

/*
	initialize dialog, and set control property.
*/
BOOL CAgoraCaptureAduioDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_localVideoWnd.Create(NULL, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CRect(0, 0, 1, 1), this, ID_BASEWND_VIDEO + 100);
	RECT rcArea;
	m_staVideoArea.GetClientRect(&rcArea);
	m_localVideoWnd.MoveWindow(&rcArea);
	m_localVideoWnd.ShowWindow(SW_SHOW);
	//create and initialize audio capture object.
	m_agAudioCaptureDevice.Create();
	ResumeStatus();
	m_renderAudioInfo.sampleRate = 44100;
	m_renderAudioInfo.channels = 2;
	m_renderAudioInfo.sampleByte = 2;
	return TRUE;
}


// update window view and control.
void CAgoraCaptureAduioDlg::UpdateViews()
{
	// render local video
	RenderLocalVideo();
	// enumerate device and show.
	UpdateDevice();
}

// enumerate device and show device in combobox.
void CAgoraCaptureAduioDlg::UpdateDevice() 
{
	TCHAR				szDevicePath[MAX_PATH];
	SIZE_T				nPathLen = MAX_PATH;
	AGORA_DEVICE_INFO	agDeviceInfo;
	nPathLen = MAX_PATH;
	m_cmbAudioDevice.ResetContent();
	//enum audio capture device.
	if (m_initialize && m_agAudioCaptureDevice.EnumDeviceList())
	{
		m_agAudioCaptureDevice.GetCurrentDevice(szDevicePath, &nPathLen);
		for (int nIndex = 0; nIndex < m_agAudioCaptureDevice.GetDeviceCount(); nIndex++) {
			m_agAudioCaptureDevice.GetDeviceInfo(nIndex, &agDeviceInfo);
			m_cmbAudioDevice.InsertString(nIndex, agDeviceInfo.szDeviceName);
		}
		if (m_cmbAudioDevice.GetCount() > 0) {
			m_cmbAudioDevice.SetCurSel(0);
			OnSelchangeComboCaptureAudioDevice();
		}
	}
}

// resume window status.
void CAgoraCaptureAduioDlg::ResumeStatus()
{
	m_lstInfo.ResetContent();
	//m_btnSetAudioRender.SetWindowText(customAudioCaptureCtrlSetAudioRender);
	EnableCaputre(FALSE);
	m_edtChannel.SetWindowText(_T(""));
	m_joinChannel = false;
	m_initialize = false;
	m_remoteJoined = false;
	m_extenalCaptureAudio = false;
	m_extenalRenderAudio = false;
}

/*
	Enumerate all the video capture devices and add to the combo box.
*/
void CAgoraCaptureAduioDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);
	if (bShow) {
		//init control text.
		InitCtrlText();
		//update window.
		UpdateViews();
	}
	else {
		//resume window status.
		ResumeStatus();
	}
}

//Enumerates the audio capture devices and types, 
//and inserts them into the ComboBox
void CAgoraCaptureAduioDlg::OnSelchangeComboCaptureAudioDevice()
{
	TCHAR	szDevicePath[MAX_PATH];
	SIZE_T	nPathLen = MAX_PATH;
	int		nSel = m_cmbAudioDevice.GetCurSel();
	WAVEFORMATEX		wavFormatEx;
	CString				strInfo;

	BOOL bSuccess = m_agAudioCaptureDevice.GetCurrentDevice(szDevicePath, &nPathLen);
	if (bSuccess)
		m_agAudioCaptureDevice.CloseDevice();

	if (nSel != -1)
		if (!m_agAudioCaptureDevice.OpenDevice(nSel))return;

	m_cmbAudioType.ResetContent();
	//enum current device support type.
	for (int nIndex = 0; nIndex < m_agAudioCaptureDevice.GetMediaCapCount(); nIndex++) {
		m_agAudioCaptureDevice.GetAudioCap(nIndex, &wavFormatEx);
		strInfo.Format(_T("%.1fkHz %dbits %dCh"), wavFormatEx.nSamplesPerSec / 1000.0, wavFormatEx.wBitsPerSample, wavFormatEx.nChannels);
		m_cmbAudioType.InsertString(nIndex, strInfo);
	}
	m_cmbAudioType.SetCurSel(0);
}


BOOL CAgoraCaptureAduioDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) {
		return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}


