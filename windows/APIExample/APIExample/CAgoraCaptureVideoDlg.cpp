#include "stdafx.h"
#include "APIExample.h"
#include "CAgoraCaptureVideoDlg.h"
#include "afxdialogex.h"

BEGIN_MESSAGE_MAP(CAgoraCaptureVideoDlg, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_MESSAGE(WM_MSGID(EID_JOINCHANNEL_SUCCESS), &CAgoraCaptureVideoDlg::OnEIDJoinChannelSuccess)
	ON_MESSAGE(WM_MSGID(EID_LEAVE_CHANNEL), &CAgoraCaptureVideoDlg::OnEIDLeaveChannel)
	ON_MESSAGE(WM_MSGID(EID_USER_JOINED), &CAgoraCaptureVideoDlg::OnEIDUserJoined)
	ON_MESSAGE(WM_MSGID(EID_USER_OFFLINE), &CAgoraCaptureVideoDlg::OnEIDUserOffline)
	ON_MESSAGE(WM_MSGID(EID_REMOTE_VIDEO_STATE_CHANED), &CAgoraCaptureVideoDlg::OnEIDRemoteVideoStateChanged)
	ON_BN_CLICKED(IDC_BUTTON_START_CAPUTRE, &CAgoraCaptureVideoDlg::OnClickedButtonStartCaputre)
	ON_BN_CLICKED(IDC_BUTTON_JOINCHANNEL, &CAgoraCaptureVideoDlg::OnClickedButtonJoinchannel)
	ON_CBN_SELCHANGE(IDC_COMBO_CAPTURE_VIDEO_DEVICE, &CAgoraCaptureVideoDlg::OnSelchangeComboCaptureVideoDevice)
END_MESSAGE_MAP()

/**
**	The Agora SDK needs to collect video data, call the function, 
**  need to set the VideoFrame, provide to the Agora SDK.
*/

bool CExtendVideoFrameObserver::onCaptureVideoFrame(VideoFrame & videoFrame)
{
	int bufSize = videoFrame.width * videoFrame.height * 3 / 2;
	int timestamp = GetTickCount();
	if (CAgVideoBuffer::GetInstance()->readBuffer(m_lpBuffer, bufSize, timestamp)) {
		memcpy_s(m_videoBuffer.m_lpImageBuffer, bufSize, m_lpBuffer, bufSize);
		m_videoBuffer.timestamp = timestamp;
	}
	else
		OutputDebugString(L"readBuffer failed");
	m_lpY = m_videoBuffer.m_lpImageBuffer;
	m_lpU = m_videoBuffer.m_lpImageBuffer + videoFrame.height * videoFrame.width;
	m_lpV = m_videoBuffer.m_lpImageBuffer + 5 * videoFrame.height * videoFrame.width / 4;
	memcpy_s(videoFrame.yBuffer, videoFrame.height * videoFrame.width, m_lpY, videoFrame.height * videoFrame.width);
	videoFrame.yStride = videoFrame.width;
	memcpy_s(videoFrame.uBuffer, videoFrame.height * videoFrame.width / 4, m_lpU, videoFrame.height * videoFrame.width / 4);
	videoFrame.uStride = videoFrame.width / 2;
	memcpy_s(videoFrame.vBuffer, videoFrame.height * videoFrame.width / 4, m_lpV, videoFrame.height * videoFrame.width / 4);
	videoFrame.vStride = videoFrame.width / 2;
	videoFrame.type = FRAME_TYPE_YUV420;
	videoFrame.rotation = 0;
	return true;
}

bool CExtendVideoFrameObserver::onRenderVideoFrame(unsigned int uid, VideoFrame & videoFrame)
{
	return true;
}

void CAgoraCaptureVideoDlg::InitCtrlText()
{
	m_staChannelName.SetWindowText(commonCtrlChannel);
	m_staCaputreVideo.SetWindowText(customVideoCaptureCtrlCaptureVideoDevice);
	m_btnJoinChannel.SetWindowText(commonCtrlJoinChannel);
	m_btnSetExtCapture.SetWindowText(customVideoCaptureCtrlSetExternlCapture);
}


bool CAgoraCaptureVideoDlg::InitAgora()
{
	m_rtcEngine = createAgoraRtcEngine();
	m_lstInfo.ResetContent();
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
	m_rtcEngine->setChannelProfile(CHANNEL_PROFILE_LIVE_BROADCASTING);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("live broadcasting"));
	m_rtcEngine->setClientRole(CLIENT_ROLE_BROADCASTER);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("setClientRole broadcaster"));
	m_btnJoinChannel.EnableWindow(TRUE);
	return true;
}

void CAgoraCaptureVideoDlg::UnInitAgora()
{
	if (m_rtcEngine) {
		m_rtcEngine->stopPreview();
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("stopPreview"));
		m_rtcEngine->disableVideo();
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("disableVideo"));
		m_rtcEngine->release(true);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("release rtc engine"));
		m_rtcEngine = NULL;
	}
}

void CAgoraCaptureVideoDlgEngineEventHandler::onRemoteVideoStateChanged(uid_t uid, REMOTE_VIDEO_STATE state, REMOTE_VIDEO_STATE_REASON reason, int elapsed)
{
	if (m_hMsgHanlder) {
		PVideoStateStateChanged stateChanged = new VideoStateStateChanged;
		stateChanged->uid = uid;
		stateChanged->reason = reason;
		stateChanged->state = state;
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_REMOTE_VIDEO_STATE_CHANED), (WPARAM)stateChanged, 0);
	}
}


BOOL CAgoraCaptureVideoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	InitCtrlText();
	m_localVideoWnd.Create(NULL, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CRect(0, 0, 1, 1), this, ID_BASEWND_VIDEO + 100);
	m_remoteVideoWnd.Create(NULL, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CRect(0, 0, 1, 1), this, ID_BASEWND_VIDEO + 100);

	RECT rcArea;
	m_staVideoArea.GetClientRect(&rcArea);
	m_agVideoCaptureDevice.Create();

	m_localVideoWnd.MoveWindow(&rcArea);
	m_localVideoWnd.ShowWindow(SW_SHOW);


	m_btnSetExtCapture.EnableWindow(FALSE);

	return TRUE;  
}

BOOL CAgoraCaptureVideoDlg::EnableExtendVideoCapture(BOOL bEnable)
{
	agora::util::AutoPtr<agora::media::IMediaEngine> mediaEngine;
	mediaEngine.queryInterface(m_rtcEngine, agora::AGORA_IID_MEDIA_ENGINE);
	int nRet = 0;
	AParameter apm(*m_rtcEngine);
	if (mediaEngine.get() == NULL)
		return FALSE;
	if (bEnable) {
		apm->setParameters("{\"che.video.local.camera_index\":1024}");
		nRet = mediaEngine->registerVideoFrameObserver(&m_extVideoFrameObserver);
	}
	else {
		nRet = mediaEngine->registerVideoFrameObserver(NULL);
	}
	m_extenalCaptureVideo = bEnable;
	return nRet == 0 ? TRUE : FALSE;
}


void CAgoraCaptureVideoDlg::RenderLocalVideo()
{
	if (m_rtcEngine) {
		VideoCanvas canvas;
		canvas.renderMode = RENDER_MODE_FIT;
		canvas.uid = 0;
		canvas.view = m_localVideoWnd.GetSafeHwnd();
		m_rtcEngine->setupLocalVideo(canvas);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("render local video"));
	}
}

void CAgoraCaptureVideoDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);
	if (bShow == FALSE)return;
	TCHAR				szDevicePath[MAX_PATH] = { 0 };
	SIZE_T				nPathLen = MAX_PATH;
	CString				strInfo;
	AGORA_DEVICE_INFO	agDeviceInfo;

	nPathLen = MAX_PATH;
	m_cmbVideoDevice.ResetContent();
	m_agVideoCaptureDevice.EnumDeviceList();
	for (int nIndex = 0; nIndex < m_agVideoCaptureDevice.GetDeviceCount(); nIndex++) {
		m_agVideoCaptureDevice.GetDeviceInfo(nIndex, &agDeviceInfo);
		m_cmbVideoDevice.InsertString(nIndex, agDeviceInfo.szDeviceName);
	}
	m_cmbVideoDevice.SetCurSel(0);
	OnSelchangeComboCaptureVideoDevice();
	RenderLocalVideo();
}


void CAgoraCaptureVideoDlg::OnClickedButtonStartCaputre()
{
	if (!m_extenalCaptureVideo)
	{
		if (EnableExtendVideoCapture(TRUE))
		{ 	
			m_agVideoCaptureDevice.SelectMediaCap(m_cmbVideoType.GetCurSel());
			VIDEOINFOHEADER videoInfo;
			VideoEncoderConfiguration config;
			m_extenalCaptureVideo = true;
			m_agVideoCaptureDevice.CreateCaptureFilter();
			m_agVideoCaptureDevice.GetCurrentVideoCap(&videoInfo);
			config.dimensions.width = videoInfo.bmiHeader.biWidth;
			config.dimensions.height = videoInfo.bmiHeader.biHeight;
			m_rtcEngine->setVideoEncoderConfiguration(config);
			m_agVideoCaptureDevice.Start();
			m_rtcEngine->enableVideo();
			m_rtcEngine->startPreview();
			m_btnSetExtCapture.SetWindowText(customVideoCaptureCtrlCancelExternlCapture);
			m_btnJoinChannel.EnableWindow(FALSE);
			m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("use extenal video frame observer sucess!"));
		}
		else {
			m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("use extenal video frame observer falied!"));
		}
	}
	else {
		m_agVideoCaptureDevice.Stop();
		m_agVideoCaptureDevice.RemoveCaptureFilter();
		m_rtcEngine->disableVideo();
		m_rtcEngine->stopPreview();
		if (EnableExtendVideoCapture(FALSE))
		{
			m_extenalCaptureVideo = false;
			m_btnJoinChannel.EnableWindow(TRUE);
			m_btnSetExtCapture.SetWindowText(customVideoCaptureCtrlSetExternlCapture);
			m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("restore video frame observer sucess!"));
		}
		else {
			m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("restore video frame observer falied!"));
		}
	}


}

//The JoinChannel button's click handler.
//This function either joins or leaves the channel
void CAgoraCaptureVideoDlg::OnClickedButtonJoinchannel()
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



LRESULT CAgoraCaptureVideoDlg::OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam)
{
	m_btnJoinChannel.EnableWindow(TRUE);
	m_btnSetExtCapture.EnableWindow(TRUE);
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

LRESULT CAgoraCaptureVideoDlg::OnEIDLeaveChannel(WPARAM wParam, LPARAM lParam)
{
	m_btnJoinChannel.EnableWindow(TRUE);
	m_btnSetExtCapture.EnableWindow(FALSE);

	joinChannel = false;
	m_btnJoinChannel.SetWindowText(commonCtrlJoinChannel);

	CString strInfo;
	strInfo.Format(_T("leave channel success %s"), getCurrentTime());
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	::PostMessage(GetParent()->GetSafeHwnd(), WM_MSGID(EID_JOINCHANNEL_SUCCESS), FALSE, 0);
	return 0;
}

LRESULT CAgoraCaptureVideoDlg::OnEIDUserJoined(WPARAM wParam, LPARAM lParam)
{
	CString strInfo;
	strInfo.Format(_T("%u joined"), wParam);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

	return 0;
}

LRESULT CAgoraCaptureVideoDlg::OnEIDUserOffline(WPARAM wParam, LPARAM lParam)
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

LRESULT CAgoraCaptureVideoDlg::OnEIDRemoteVideoStateChanged(WPARAM wParam, LPARAM lParam)
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

IMPLEMENT_DYNAMIC(CAgoraCaptureVideoDlg, CDialogEx)

CAgoraCaptureVideoDlg::CAgoraCaptureVideoDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_CUSTOM_CAPTURE_VIDEO, pParent)
{

}

CAgoraCaptureVideoDlg::~CAgoraCaptureVideoDlg()
{
}

void CAgoraCaptureVideoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_VIDEO, m_staVideoArea);
	DDX_Control(pDX, IDC_STATIC_CHANNELNAME, m_staChannelName);
	DDX_Control(pDX, IDC_STATIC_CAPTUREDEVICE, m_staCaputreVideo);
	DDX_Control(pDX, IDC_EDIT_CHANNELNAME, m_edtChannel);
	DDX_Control(pDX, IDC_BUTTON_JOINCHANNEL, m_btnJoinChannel);
	DDX_Control(pDX, IDC_BUTTON_START_CAPUTRE, m_btnSetExtCapture);
	DDX_Control(pDX, IDC_COMBO_CAPTURE_VIDEO_DEVICE, m_cmbVideoDevice);
	DDX_Control(pDX, IDC_COMBO_CAPTURE_VIDEO_TYPE, m_cmbVideoType);
	DDX_Control(pDX, IDC_LIST_INFO_BROADCASTING, m_lstInfo);
}

//Enumerates the video capture devices and types, 
//and inserts them into the ComboBox
void CAgoraCaptureVideoDlg::OnSelchangeComboCaptureVideoDevice()
{
	TCHAR	szDevicePath[MAX_PATH] = { 0 };
	SIZE_T	nPathLen = MAX_PATH;
	int		nSel = m_cmbVideoDevice.GetCurSel();

	VIDEOINFOHEADER		vidInfoHeader;
	CString				strInfo;
	CString				strCompress;

	BOOL bSuccess = m_agVideoCaptureDevice.GetCurrentDevice(szDevicePath, &nPathLen);
	if (bSuccess)
		m_agVideoCaptureDevice.CloseDevice();

	if (nSel != -1) {
		m_agVideoCaptureDevice.OpenDevice(nSel);
		m_agVideoCaptureDevice.CreateCaptureFilter();
	}
	m_cmbVideoType.ResetContent();
	int count = m_agVideoCaptureDevice.GetMediaCapCount();
	for (int nIndex = 0; nIndex < count; nIndex++) {
		m_agVideoCaptureDevice.GetVideoCap(nIndex, &vidInfoHeader);
		switch (vidInfoHeader.bmiHeader.biCompression)
		{
		case 0x00000000:
			strInfo.Format(_T("%d*%d %dfps(RGB24)"), vidInfoHeader.bmiHeader.biWidth, vidInfoHeader.bmiHeader.biHeight, 10000000 / vidInfoHeader.AvgTimePerFrame);
			break;
		case MAKEFOURCC('I', '4', '2', '0'):
			strInfo.Format(_T("%d*%d %dfps(YUV420)"), vidInfoHeader.bmiHeader.biWidth, vidInfoHeader.bmiHeader.biHeight, 10000000 / vidInfoHeader.AvgTimePerFrame);
			break;
		case MAKEFOURCC('Y', 'U', 'Y', '2'):
			strInfo.Format(_T("%d*%d %dfps(YUY2)"), vidInfoHeader.bmiHeader.biWidth, vidInfoHeader.bmiHeader.biHeight, 10000000 / vidInfoHeader.AvgTimePerFrame);
			break;
		case MAKEFOURCC('M', 'J', 'P', 'G'):
			strInfo.Format(_T("%d*%d %dfps(MJPEG)"), vidInfoHeader.bmiHeader.biWidth, vidInfoHeader.bmiHeader.biHeight, 10000000 / vidInfoHeader.AvgTimePerFrame);
			break;
		case MAKEFOURCC('U', 'Y', 'V', 'Y'):
			strInfo.Format(_T("%d*%d %dfps(UYVY)"), vidInfoHeader.bmiHeader.biWidth, vidInfoHeader.bmiHeader.biHeight, 10000000 / vidInfoHeader.AvgTimePerFrame);
			break;
		}

		m_cmbVideoType.InsertString(nIndex, strInfo);
	}
	m_cmbVideoType.SetCurSel(0);
}






void CAgoraCaptureVideoDlgEngineEventHandler::onJoinChannelSuccess(const char* channel, uid_t uid, int elapsed)
{
	if (m_hMsgHanlder) {
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_JOINCHANNEL_SUCCESS), (WPARAM)uid, (LPARAM)elapsed);
	}
}

void CAgoraCaptureVideoDlgEngineEventHandler::onUserJoined(uid_t uid, int elapsed)
{
	if (m_hMsgHanlder) {
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_USER_JOINED), (WPARAM)uid, (LPARAM)elapsed);
	}
}

void CAgoraCaptureVideoDlgEngineEventHandler::onUserOffline(uid_t uid, USER_OFFLINE_REASON_TYPE reason)
{
	if (m_hMsgHanlder) {
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_USER_OFFLINE), (WPARAM)uid, (LPARAM)reason);
	}
}

void CAgoraCaptureVideoDlgEngineEventHandler::onLeaveChannel(const RtcStats& stats)
{
	if (m_hMsgHanlder) {
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_LEAVE_CHANNEL), 0, 0);
	}
}

//Prevent window exit
BOOL CAgoraCaptureVideoDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN&&pMsg->wParam==VK_RETURN) {
		return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}
