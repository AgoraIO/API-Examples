#include "stdafx.h"
#include "APIExample.h"
#include "CAgoraCaptureAudioDlg.h"
#include "afxdialogex.h"
#include "DreictShow/CircleBuffer.hpp"

IMPLEMENT_DYNAMIC(CAgoraCaptureAduioDlg, CDialogEx)




CExtendAudioFrameObserver::CExtendAudioFrameObserver()
{
}


CExtendAudioFrameObserver::~CExtendAudioFrameObserver()
{
}


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

bool CExtendAudioFrameObserver::onPlaybackAudioFrame(AudioFrame& audioFrame)
{
	return true;
}

bool CExtendAudioFrameObserver::onMixedAudioFrame(AudioFrame& audioFrame)
{
	return true;
}

bool CExtendAudioFrameObserver::onPlaybackAudioFrameBeforeMixing(unsigned int uid, AudioFrame& audioFrame)
{
	return true;
}



LRESULT CAgoraCaptureAduioDlg::OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam)
{
	joinChannel = true;
	m_btnJoinChannel.SetWindowText(commonCtrlLeaveChannel);
	CString strInfo;
	strInfo.Format(_T("%s:join success, uid=%u"), getCurrentTime(), wParam);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	m_localVideoWnd.SetUID(wParam);
	//notify parent window
	::PostMessage(GetParent()->GetSafeHwnd(), WM_MSGID(EID_JOINCHANNEL_SUCCESS), TRUE, 0);
	return 0;
}

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

LRESULT CAgoraCaptureAduioDlg::OnEIDUserJoined(WPARAM wParam, LPARAM lParam)
{
	CString strInfo;
	strInfo.Format(_T("%u joined"), wParam);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

	return 0;
}

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


bool CAgoraCaptureAduioDlg::InitAgora()
{
	m_rtcEngine = createAgoraRtcEngine();
	if (!m_rtcEngine) {
		m_lstInfo.InsertString(m_lstInfo.GetCount() - 1, _T("createAgoraRtcEngine failed"));
		return false;
	}
	m_eventHandler.SetMsgReceiver(m_hWnd);

	RtcEngineContext context;
	context.appId = APP_ID;
	context.eventHandler = &m_eventHandler;
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
	m_rtcEngine->enableVideo();
	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("enable video"));
	m_rtcEngine->enableAudio();
	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("enable audio"));
	m_rtcEngine->setChannelProfile(CHANNEL_PROFILE_LIVE_BROADCASTING);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("live broadcasting"));
	m_rtcEngine->setClientRole(CLIENT_ROLE_BROADCASTER);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("setClientRole broadcaster"));

	return true;
}

void CAgoraCaptureAduioDlg::UnInitAgora()
{
	if (m_rtcEngine) {
		m_rtcEngine->stopPreview();
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("stopPreview"));
		m_rtcEngine->disableVideo();
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("disableVideo"));
		m_rtcEngine->disableAudio();
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("disableAudio"));
		m_rtcEngine->release(true);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("release rtc engine"));
		m_rtcEngine = NULL;
	}
}

void CAgoraCaptureAduioDlg::RenderLocalVideo()
{
	if (m_rtcEngine) {
		m_rtcEngine->startPreview();
		VideoCanvas canvas;
		canvas.renderMode = RENDER_MODE_FIT;
		canvas.uid = 0;
		canvas.view = m_localVideoWnd.GetSafeHwnd();
		m_rtcEngine->setupLocalVideo(canvas);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("render local video"));
	}
}

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
		if (0 == m_rtcEngine->joinChannel(APP_TOKEN, szChannelId.c_str(), "", 0)) {
			strInfo.Format(_T("join channel %s"), getCurrentTime());
			m_btnJoinChannel.EnableWindow(FALSE);
		}
	}
	else {
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
		m_agAudioCaptureDevice.SelectMediaCap(m_cmbAudioType.GetCurSel());
		m_agAudioCaptureDevice.GetCurrentAudioCap(&waveFormat);
		nBufferSize = waveFormat.nAvgBytesPerSec / AUDIO_CALLBACK_TIMES;
		m_agAudioCaptureDevice.SetCaptureBuffer(nBufferSize, 16, waveFormat.nBlockAlign);
		RtcEngineParameters rep(*m_rtcEngine);
		int nRet = rep.setRecordingAudioFrameParameters(waveFormat.nSamplesPerSec, waveFormat.nChannels, RAW_AUDIO_FRAME_OP_MODE_READ_WRITE, waveFormat.nSamplesPerSec * waveFormat.nChannels / 100);
		if (!m_agAudioCaptureDevice.CreateCaptureFilter())
			return;
		EnableExtendAudioCapture(TRUE);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("register auido frame observer"));
		m_agAudioCaptureDevice.Start();
		m_btnJoinChannel.EnableWindow(TRUE);
		m_btnSetAudioCtx.SetWindowText(customAudioCaptureCtrlCancelExternlCapture);
	}
	else {
		m_agAudioCaptureDevice.Stop();
		EnableExtendAudioCapture(FALSE);
		m_btnJoinChannel.EnableWindow(TRUE);
		m_btnSetAudioCtx.EnableWindow(FALSE);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("unregister auido frame observer"));
		m_btnSetAudioCtx.SetWindowText(customAudioCaptureCtrlSetExternlCapture);
	}
}


BOOL CAgoraCaptureAduioDlg::EnableExtendAudioCapture(BOOL bEnable)
{
	agora::util::AutoPtr<agora::media::IMediaEngine> mediaEngine;
	mediaEngine.queryInterface(m_rtcEngine, agora::AGORA_IID_MEDIA_ENGINE);
	int nRet = 0;
	if (mediaEngine.get() == NULL)
		return FALSE;
	if (bEnable )
		nRet = mediaEngine->registerAudioFrameObserver(&m_extAudioObserver);
	else
		nRet = mediaEngine->registerAudioFrameObserver(NULL);

	return nRet == 0 ? TRUE : FALSE;
}



void CAgoraCaptureAduioDlgEngineEventHandler::onJoinChannelSuccess(const char* channel, uid_t uid, int elapsed)
{
	if (m_hMsgHanlder) {
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_JOINCHANNEL_SUCCESS), (WPARAM)uid, (LPARAM)elapsed);
	}
}

void CAgoraCaptureAduioDlgEngineEventHandler::onUserJoined(uid_t uid, int elapsed)
{
	if (m_hMsgHanlder) {
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_USER_JOINED), (WPARAM)uid, (LPARAM)elapsed);
	}
}

void CAgoraCaptureAduioDlgEngineEventHandler::onUserOffline(uid_t uid, USER_OFFLINE_REASON_TYPE reason)
{
	if (m_hMsgHanlder) {
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_USER_OFFLINE), (WPARAM)uid, (LPARAM)reason);
	}
}

void CAgoraCaptureAduioDlgEngineEventHandler::onLeaveChannel(const RtcStats& stats)
{
	if (m_hMsgHanlder) {
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_LEAVE_CHANNEL), 0, 0);
	}
}

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


BOOL CAgoraCaptureAduioDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	InitCtrlText();
	m_localVideoWnd.Create(NULL, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CRect(0, 0, 1, 1), this, ID_BASEWND_VIDEO + 100);
	m_remoteVideoWnd.Create(NULL, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CRect(0, 0, 1, 1), this, ID_BASEWND_VIDEO + 100);

	RECT rcArea;
	m_staVideoArea.GetClientRect(&rcArea);
	m_localVideoWnd.MoveWindow(&rcArea);
	m_localVideoWnd.ShowWindow(SW_SHOW);
	m_agAudioCaptureDevice.Create();
	m_btnJoinChannel.EnableWindow(FALSE);
	m_btnSetAudioCtx.EnableWindow(TRUE);
	return TRUE;
}


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


void CAgoraCaptureAduioDlg::OnSelchangeComboCaptureAudioDevice()
{
	// TODO: 在此添加控件通知处理程序代码
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
	for (int nIndex = 0; nIndex < m_agAudioCaptureDevice.GetMediaCapCount(); nIndex++) {
		m_agAudioCaptureDevice.GetAudioCap(nIndex, &wavFormatEx);
		strInfo.Format(_T("%.1fkHz %dbits %dCh"), wavFormatEx.nSamplesPerSec / 1000.0, wavFormatEx.wBitsPerSample, wavFormatEx.nChannels);
		m_cmbAudioType.InsertString(nIndex, strInfo);
	}
	m_cmbAudioType.SetCurSel(0);
}
