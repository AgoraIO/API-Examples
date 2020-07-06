#include "../../stdafx.h"
#include "../../APIExample.h"
#include "CAgoraCaptureAudioDlg.h"
#include "afxdialogex.h"
#include "../../DirectShow/CircleBuffer.hpp"

IMPLEMENT_DYNAMIC(CAgoraCaptureAduioDlg, CDialogEx)

/*
*	According to the setting of audio collection frame rate, 
*	the Agora SDK calls this callback function at an appropriate time 
*	to obtain the audio data collected by the user.
*/
bool CExtendAudioFrameObserver::onRecordAudioFrame(AudioFrame& audioFrame)
{
	SIZE_T nSize = audioFrame.channels * audioFrame.samples * 2;
	unsigned int readByte = 0;
	int timestamp = GetTickCount();
	CircleBuffer::GetInstance()->readBuffer(audioFrame.buffer, nSize, &readByte, timestamp);
	CString strInfo;
	strInfo.Format(_T("audio Frame buffer size:%d, readByte:%d, timestamp:%d \n"), nSize, readByte, timestamp);
	OutputDebugString(strInfo);
	audioFrame.renderTimeMs = timestamp;
	return true;
}
/*
	Get the sound played.
	parameter:
	audioFrame:Audio naked data.
	See: AudioFrame
	return
	True: Buffer data in AudioFrame is valid, the data will be sent;
	False: The buffer data in the AudioFrame is invalid and will be discarded.
*/
bool CExtendAudioFrameObserver::onPlaybackAudioFrame(AudioFrame& audioFrame)
{
	return true;
}
/*
	Gets the data after recording and playing the voice mix.
	annotations:
		This method returns only single-channel data.
	parameter:
	audioFrame Audio naked data. See: AudioFrame
	return:
	True: Buffer data in AudioFrame is valid, the data will be sent;
	False: The buffer data in the AudioFrame is invalid and will be discarded.
*/
bool CExtendAudioFrameObserver::onMixedAudioFrame(AudioFrame& audioFrame)
{
	return true;
}
/*
	Gets the specified user's voice before the mix.
	parameter:
	uid: Specifies the user ID of the user.
	audioFrame: Audio naked data. See: AudioFrame.
	return:
	True: Buffer data in AudioFrame is valid, the data will be sent;
	False: The buffer data in the AudioFrame is invalid and will be discarded.
*/
bool CExtendAudioFrameObserver::onPlaybackAudioFrameBeforeMixing(unsigned int uid, AudioFrame& audioFrame)
{
	return true;
}

//EID_JOINCHANNEL_SUCCESS message window handler
LRESULT CAgoraCaptureAduioDlg::OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam)
{
	joinChannel = true;
	m_btnJoinChannel.SetWindowText(commonCtrlLeaveChannel);
	m_btnSetAudioCtx.EnableWindow(TRUE);
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
	m_btnSetAudioCtx.EnableWindow(TRUE);
	joinChannel = false;
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

//EID_REMOTE_VIDEO_STATE_CHANED message window handler.
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
{

}

CAgoraCaptureAduioDlg::~CAgoraCaptureAduioDlg()
{
}

/*
	create Agora Rtc Engine and initialize context.set channel proprety.
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
	context.appId = APP_ID;
	context.eventHandler = &m_eventHandler;
	//initalize the Agora RTC engine context.  
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
	//enable audio in the engine.
	m_rtcEngine->enableAudio();
	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("enable audio"));
	//set channel profile in the engine to the CHANNEL_PROFILE_LIVE_BROADCASTING.
	m_rtcEngine->setChannelProfile(CHANNEL_PROFILE_LIVE_BROADCASTING);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("live broadcasting"));
	//set clinet role in the engine to the CLIENT_ROLE_BROADCASTER.
	m_rtcEngine->setClientRole(CLIENT_ROLE_BROADCASTER);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("setClientRole broadcaster"));
	return true;
}


/*
	UnInitialize the Agora SDK
*/
void CAgoraCaptureAduioDlg::UnInitAgora()
{
	if (m_rtcEngine) {
		//stop preview in the engine.
		m_rtcEngine->stopPreview();
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("stopPreview"));
		//disable video in the engine.
		m_rtcEngine->disableVideo();
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("disableVideo"));
		//relase engine.
		m_rtcEngine->release(true);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("release rtc engine"));
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
		canvas.renderMode = RENDER_MODE_FIT;
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
}


BEGIN_MESSAGE_MAP(CAgoraCaptureAduioDlg, CDialogEx)
	ON_MESSAGE(WM_MSGID(EID_JOINCHANNEL_SUCCESS), &CAgoraCaptureAduioDlg::OnEIDJoinChannelSuccess)
	ON_MESSAGE(WM_MSGID(EID_LEAVE_CHANNEL), &CAgoraCaptureAduioDlg::OnEIDLeaveChannel)
	ON_MESSAGE(WM_MSGID(EID_USER_JOINED), &CAgoraCaptureAduioDlg::OnEIDUserJoined)
	ON_MESSAGE(WM_MSGID(EID_USER_OFFLINE), &CAgoraCaptureAduioDlg::OnEIDUserOffline)
	ON_MESSAGE(WM_MSGID(EID_REMOTE_VIDEO_STATE_CHANED), &CAgoraCaptureAduioDlg::OnEIDRemoteVideoStateChanged)
	ON_BN_CLICKED(IDC_BUTTON_JOINCHANNEL, &CAgoraCaptureAduioDlg::OnBnClickedButtonJoinchannel)
	ON_BN_CLICKED(IDC_BUTTON_START_CAPUTRE, &CAgoraCaptureAduioDlg::OnBnClickedButtonStartCaputre)
	ON_WM_SHOWWINDOW()
	ON_CBN_SELCHANGE(IDC_COMBO_CAPTURE_AUDIO_DEVICE, &CAgoraCaptureAduioDlg::OnSelchangeComboCaptureAudioDevice)
END_MESSAGE_MAP()



//The JoinChannel button's click handler.
//This function either joins or leaves the channel
void CAgoraCaptureAduioDlg::OnBnClickedButtonJoinchannel()
{
	if (!m_rtcEngine || !m_initialize)
		return;
	CString strInfo;
	if (!joinChannel) {
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
			m_btnJoinChannel.EnableWindow(FALSE);
		}
	}
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
}

/*
	Start or stop collecting audio devices and 
	register or unregister external audio observer objects.
*/
void CAgoraCaptureAduioDlg::OnBnClickedButtonStartCaputre()
{
	WAVEFORMATEX	waveFormat;
	SIZE_T			nBufferSize = 0;
	m_extenalCaptureAudio = !m_extenalCaptureAudio;
	if (m_extenalCaptureAudio)
	{
		//select meida capture.
		m_agAudioCaptureDevice.SelectMediaCap(m_cmbAudioType.GetCurSel());
		//get current audio capture format.
		m_agAudioCaptureDevice.GetCurrentAudioCap(&waveFormat);
		nBufferSize = waveFormat.nAvgBytesPerSec / AUDIO_CALLBACK_TIMES;
		//create capture Buffer.
		m_agAudioCaptureDevice.SetCaptureBuffer(nBufferSize, 16, waveFormat.nBlockAlign);
		RtcEngineParameters rep(*m_rtcEngine);
		//set recording audio frame parameters in the engine.
		int nRet = rep.setRecordingAudioFrameParameters(waveFormat.nSamplesPerSec, waveFormat.nChannels, RAW_AUDIO_FRAME_OP_MODE_READ_WRITE, waveFormat.nSamplesPerSec * waveFormat.nChannels / 100);
		//create audio capture filter.
		if (!m_agAudioCaptureDevice.CreateCaptureFilter())
			return;
		//register agora audio frame observer.
		EnableExtendAudioCapture(TRUE);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("register auido frame observer"));
		//start audio capture.
		m_agAudioCaptureDevice.Start();
		m_btnSetAudioCtx.EnableWindow(FALSE);
		m_btnJoinChannel.EnableWindow(TRUE);
		::PostMessage(GetParent()->GetSafeHwnd(), WM_MSGID(EID_JOINCHANNEL_SUCCESS), TRUE, 0);
		m_btnSetAudioCtx.SetWindowText(customAudioCaptureCtrlCancelExternlCapture);
	}
	else {
		//stop audio capture.
		m_agAudioCaptureDevice.Stop();
		//unregister agora audio frame observer.
		EnableExtendAudioCapture(FALSE);
		m_btnJoinChannel.EnableWindow(TRUE);
		m_btnSetAudioCtx.EnableWindow(FALSE);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("unregister auido frame observer"));
		m_btnSetAudioCtx.SetWindowText(customAudioCaptureCtrlSetExternlCapture);
	}
}

/*
	register or unregister agora audio Frame Observer.
*/
BOOL CAgoraCaptureAduioDlg::EnableExtendAudioCapture(BOOL bEnable)
{
	agora::util::AutoPtr<agora::media::IMediaEngine> mediaEngine;
	//query interface agora::AGORA_IID_MEDIA_ENGINE in the engine.
	mediaEngine.queryInterface(m_rtcEngine, agora::AGORA_IID_MEDIA_ENGINE);
	int nRet = 0;
	if (mediaEngine.get() == NULL)
		return FALSE;
	//register audio frame observer.
	if ( bEnable )
		nRet = mediaEngine->registerAudioFrameObserver(&m_extAudioObserver);
	else
	//unregister audio frame observer.
		nRet = mediaEngine->registerAudioFrameObserver(NULL);

	return nRet == 0 ? TRUE : FALSE;
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
	by the callback（ms).
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
parametes:
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
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_REMOTE_VIDEO_STATE_CHANED), (WPARAM)stateChanged, 0);
	}
}

/*
	initialize dialog, and set control proprety.
*/
BOOL CAgoraCaptureAduioDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	InitCtrlText();
	m_localVideoWnd.Create(NULL, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CRect(0, 0, 1, 1), this, ID_BASEWND_VIDEO + 100);

	RECT rcArea;
	m_staVideoArea.GetClientRect(&rcArea);
	m_localVideoWnd.MoveWindow(&rcArea);
	m_localVideoWnd.ShowWindow(SW_SHOW);
	//create and initialize audio capture object.
	m_agAudioCaptureDevice.Create();
	m_btnJoinChannel.EnableWindow(FALSE);
	m_btnSetAudioCtx.EnableWindow(TRUE);
	return TRUE;
}

/*
	Enumerate all the video capture devices and add to the combo box.
*/
void CAgoraCaptureAduioDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);
	if (!bShow)return;
	RenderLocalVideo(); 
	TCHAR				szDevicePath[MAX_PATH];
	SIZE_T				nPathLen = MAX_PATH;
	AGORA_DEVICE_INFO	agDeviceInfo;
	nPathLen = MAX_PATH;
	m_cmbAudioDevice.ResetContent();
	//enum audio capture device.
	if (m_agAudioCaptureDevice.EnumDeviceList())
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

//Enumerates the aduio capture devices and types, 
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
		m_agAudioCaptureDevice.OpenDevice(nSel);

	m_cmbAudioType.ResetContent();
	//enum current deivce support type.
	for (int nIndex = 0; nIndex < m_agAudioCaptureDevice.GetMediaCapCount(); nIndex++) {
		m_agAudioCaptureDevice.GetAudioCap(nIndex, &wavFormatEx);
		strInfo.Format(_T("%.1fkHz %dbits %dCh"), wavFormatEx.nSamplesPerSec / 1000.0, wavFormatEx.wBitsPerSample, wavFormatEx.nChannels);
		m_cmbAudioType.InsertString(nIndex, strInfo);
	}
	m_cmbAudioType.SetCurSel(0);
}
