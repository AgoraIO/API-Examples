#include "stdafx.h"
#include "APIExample.h"
#include "CAgoraReportInCallDlg.h"


IMPLEMENT_DYNAMIC(CAgoraReportInCallDlg, CDialogEx)

CAgoraReportInCallDlg::CAgoraReportInCallDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_PEPORT_IN_CALL, pParent)
{

}

CAgoraReportInCallDlg::~CAgoraReportInCallDlg()
{
}

void CAgoraReportInCallDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_CHANNELNAME, m_staChannel);
	DDX_Control(pDX, IDC_EDIT_CHANNELNAME, m_edtChannel);
	DDX_Control(pDX, IDC_BUTTON_JOINCHANNEL, m_btnJoinChannel);
	DDX_Control(pDX, IDC_STATIC_VIDEO, m_staVideoArea);
	DDX_Control(pDX, IDC_STATIC_NETWORK_TOTAL, m_gopNetWorkTotal);
	DDX_Control(pDX, IDC_STATIC_AUDIO_REMOTE, m_gopAudioRemote);
	DDX_Control(pDX, IDC_STATIC_VIDEO_REMOTE, m_gopVideoRemote);
	DDX_Control(pDX, IDC_STATIC_TXBYTES_RXBTYES, m_staTotalBytes);
	DDX_Control(pDX, IDC_STATIC_TXBYTES_RXBYTES_VAL, m_staTotalBytesVal);
	DDX_Control(pDX, IDC_STATIC_BITRATE_ALL, m_staTotalBitrate);
	DDX_Control(pDX, IDC_STATIC_BITRATE_ALL_VAL, m_staTotalBitrateVal);
	DDX_Control(pDX, IDC_STATIC_AUDIO_NETWORK_DELAY, m_staAudioNetWorkDelay);
	DDX_Control(pDX, IDC_STATIC_AUDIO_NETWORK_DELAY_VAL, m_staAudioNetWorkDelayVal);
	DDX_Control(pDX, IDC_STATIC_AUDIO_RECIVED_BITRATE, m_staAudioRecvBitrate);
	DDX_Control(pDX, IDC_STATIC_AUDIO_RECVIED_BITRATE_VAL, m_staAudioRecvBitrateVal);
	DDX_Control(pDX, IDC_STATIC_VIDEO_NETWORK_DELAY, m_staVideoNetWorkDelay);
	DDX_Control(pDX, IDC_STATIC_VEDIO_NETWORK_DELAY_VAL, m_staVideoNetWorkDelayVal);
	DDX_Control(pDX, IDC_STATIC_VEDIO_RECIVED_BITRATE, m_staVideoRecvBitrate);
	DDX_Control(pDX, IDC_STATIC_VEDIO_RECVIED_BITRATE_VAL2, m_staVideoRecvBitrateVal);
	DDX_Control(pDX, IDC_STATIC_LOCAL_VIDEO_WIDTH_HEIGHT, m_staLocalVideoResoultion);
	DDX_Control(pDX, IDC_STATIC_LOCAL_VIDEO_WITH_HEIGHT_VAL, m_staLocalVideoResoultionVal);
	DDX_Control(pDX, IDC_STATIC_LOCAL_VIDEO_FPS, m_staLocalVideoFPS);
	DDX_Control(pDX, IDC_STATIC_LOCAL_VIDEO_FPS_VAL, m_staLocalVideoFPSVal);
	DDX_Control(pDX, IDC_STATIC_DETAIL, m_staDetails);
	DDX_Control(pDX, IDC_LIST_INFO_BROADCASTING, m_lstInfo);
}


BEGIN_MESSAGE_MAP(CAgoraReportInCallDlg, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BUTTON_JOINCHANNEL, &CAgoraReportInCallDlg::OnBnClickedButtonJoinchannel)
	ON_LBN_SELCHANGE(IDC_LIST_INFO_BROADCASTING, &CAgoraReportInCallDlg::OnSelchangeListInfoBroadcasting)
	ON_MESSAGE(WM_MSGID(EID_JOINCHANNEL_SUCCESS), &CAgoraReportInCallDlg::OnEIDJoinChannelSuccess)
	ON_MESSAGE(WM_MSGID(EID_LEAVE_CHANNEL), &CAgoraReportInCallDlg::OnEIDLeaveChannel)
	ON_MESSAGE(WM_MSGID(EID_USER_JOINED), &CAgoraReportInCallDlg::OnEIDUserJoined)
	ON_MESSAGE(WM_MSGID(EID_USER_OFFLINE), &CAgoraReportInCallDlg::OnEIDUserOffline)
	ON_MESSAGE(WM_MSGID(EID_REMOTE_VIDEO_STATE_CHANGED), &CAgoraReportInCallDlg::OnEIDRemoteVideoStateChanged)

	ON_MESSAGE(WM_MSGID(EID_RTC_STATS), &CAgoraReportInCallDlg::OnEIDRtcStats)
	ON_MESSAGE(WM_MSGID(EID_REMOTE_VIDEO_STATS), &CAgoraReportInCallDlg::OnEIDRemoteVideoStats)
	ON_MESSAGE(WM_MSGID(EID_REMOTE_AUDIO_STATS), &CAgoraReportInCallDlg::OnEIDRemoteAudioStats)
	ON_MESSAGE(WM_MSGID(EID_LOCAL_VIDEO_STATS), &CAgoraReportInCallDlg::OnEIDLocalVideoStats)

END_MESSAGE_MAP()

//Initialize the Ctrl Text.
void CAgoraReportInCallDlg::InitCtrlText()
{
	m_staChannel.SetWindowText(commonCtrlChannel);
	m_btnJoinChannel.SetWindowText(commonCtrlJoinChannel);
	m_staLocalVideoFPS.SetWindowText(ReportInCallCtrlLocalFPS);
	m_staLocalVideoResoultion.SetWindowText(ReportInCallCtrlLocalResoultion);
	m_staTotalBitrate.SetWindowText(ReportInCallCtrlTotalBitrate);
	m_staTotalBytes.SetWindowText(ReportInCallCtrlTotalBytes);
	m_gopAudioRemote.SetWindowText(ReportInCallCtrlGopRemoteAudio);
	m_gopVideoRemote.SetWindowText(ReportInCallCtrlGopRemoteVideo);
	m_gopNetWorkTotal.SetWindowText(ReportInCallCtrlGopTotal);
	m_staVideoRecvBitrate.SetWindowText(ReportInCallCtrlVideoBitrate);
	m_staVideoNetWorkDelay.SetWindowText(ReportInCallCtrlVideoNetWorkDelay);
	m_staAudioNetWorkDelay.SetWindowText(ReportInCallCtrlAudioNetWorkDelay);
	m_staAudioRecvBitrate.SetWindowText(ReportInCallCtrlAudioBitrate);
}

//Initialize the Agora SDK
bool CAgoraReportInCallDlg::InitAgora()
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
void CAgoraReportInCallDlg::UnInitAgora()
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
void CAgoraReportInCallDlg::RenderLocalVideo()
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
void CAgoraReportInCallDlg::ResumeStatus()
{
	InitCtrlText();
	m_edtChannel.SetWindowText(_T(""));
	m_lstInfo.ResetContent();
	m_staDetails.SetWindowText(_T(""));

	m_staLocalVideoFPSVal.SetWindowText(_T(""));
	m_staLocalVideoResoultionVal.SetWindowText(_T(""));
	m_staVideoRecvBitrateVal.SetWindowText(_T(""));
	m_staAudioRecvBitrateVal.SetWindowText(_T(""));
	m_staTotalBitrateVal.SetWindowText(_T(""));
	m_staTotalBytesVal.SetWindowText(_T(""));

	m_staAudioNetWorkDelayVal.SetWindowText(_T(""));
	m_staVideoNetWorkDelayVal.SetWindowText(_T(""));

	m_joinChannel = false;
	m_initialize = false;
	m_setEncrypt = false;
}



void CAgoraReportInCallDlg::OnShowWindow(BOOL bShow, UINT nStatus)
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


BOOL CAgoraReportInCallDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_localVideoWnd.Create(NULL, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CRect(0, 0, 1, 1), this, ID_BASEWND_VIDEO + 100);
	RECT rcArea;
	m_staVideoArea.GetClientRect(&rcArea);
	RECT leftArea = rcArea;
	leftArea.right = (rcArea.right - rcArea.left) / 2;
	RECT rightArea = rcArea;
	rightArea.left = (rcArea.right - rcArea.left) / 2;
	m_localVideoWnd.MoveWindow(&leftArea);
	m_localVideoWnd.ShowWindow(SW_SHOW);
	m_remoteVideoWnd.Create(NULL, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CRect(0, 0, 1, 1), this, ID_BASEWND_VIDEO + 101);
	m_remoteVideoWnd.MoveWindow(&rightArea);
	m_remoteVideoWnd.ShowWindow(SW_SHOW);
	ResumeStatus();
	return TRUE;
}


BOOL CAgoraReportInCallDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) {
		return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}


void CAgoraReportInCallDlg::OnBnClickedButtonJoinchannel()
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
		options.autoSubscribeAudio = true;
		options.autoSubscribeVideo = true;
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


// select change for list control handler
void CAgoraReportInCallDlg::OnSelchangeListInfoBroadcasting()
{
	int sel = m_lstInfo.GetCurSel();
	if (sel < 0)return;
	CString strDetail;
	m_lstInfo.GetText(sel, strDetail);
	m_staDetails.SetWindowText(strDetail);
}


//EID_JOINCHANNEL_SUCCESS message window handler.
LRESULT CAgoraReportInCallDlg::OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam)
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
LRESULT CAgoraReportInCallDlg::OnEIDLeaveChannel(WPARAM wParam, LPARAM lParam)
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
LRESULT CAgoraReportInCallDlg::OnEIDUserJoined(WPARAM wParam, LPARAM lParam)
{
	CString strInfo;
	strInfo.Format(_T("%u joined"), wParam);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	VideoCanvas vc;
	vc.uid = wParam;
	vc.view = m_remoteVideoWnd.GetSafeHwnd();
	vc.renderMode = media::base::RENDER_MODE_FIT;
	m_rtcEngine->setupRemoteVideo(vc);
	return 0;
}


//EID_USER_OFFLINE message window handler.
LRESULT CAgoraReportInCallDlg::OnEIDUserOffline(WPARAM wParam, LPARAM lParam)
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
LRESULT CAgoraReportInCallDlg::OnEIDRemoteVideoStateChanged(WPARAM wParam, LPARAM lParam)
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

//refresh remote video stats
LRESULT CAgoraReportInCallDlg::OnEIDRemoteVideoStats(WPARAM wParam, LPARAM lParam)
{
	RemoteVideoStats * p = reinterpret_cast<RemoteVideoStats*>(wParam);
	if (p)
	{
		CString tmp;
		tmp.Format(_T("%dms"), p->delay);
		m_staVideoNetWorkDelayVal.SetWindowText(tmp);
		tmp.Format(_T("%dKbps"), p->receivedBitrate);
		m_staVideoRecvBitrateVal.SetWindowText(tmp);

		delete p;
	}
	return TRUE;
}

//refresh remote audio stats
LRESULT CAgoraReportInCallDlg::OnEIDRemoteAudioStats(WPARAM wParam, LPARAM lParam)
{
	RemoteAudioStats *p = reinterpret_cast<RemoteAudioStats *>(wParam);
	if (p)
	{
		CString tmp;
		tmp.Format(_T("%dms"), p->networkTransportDelay);
		m_staAudioNetWorkDelayVal.SetWindowText(tmp);

		tmp.Format(_T("%dKbps"), p->receivedBitrate);
		m_staAudioRecvBitrateVal.SetWindowText(tmp);

		delete p;
	}
	return TRUE;
}

//refresh total bitrate and total bytes.
LRESULT CAgoraReportInCallDlg::OnEIDRtcStats(WPARAM wParam, LPARAM lParam)
{
	RtcStats *p = reinterpret_cast<RtcStats *>(wParam);
	if (p)
	{
		CString tmp;
		tmp.Format(_T("%dKbps/%dKbps"), p->txKBitRate, p->rxKBitRate);
		m_staTotalBitrateVal.SetWindowText(tmp);
		tmp.Format(_T("%.2fMB/%.2fMB"), p->txBytes ? p->txBytes / 1024.0 / 1024 : 0, p->rxBytes ? p->rxBytes / 1024.0 / 1024 : 0);
		m_staTotalBytesVal.SetWindowText(tmp);
		delete p;
	}
	return TRUE;
}

//refresh local video stats
LRESULT CAgoraReportInCallDlg::OnEIDLocalVideoStats(WPARAM wParam, LPARAM lParam)
{
	LocalVideoStats *p = reinterpret_cast<LocalVideoStats *>(wParam);
	if (p)
	{
		CString tmp;
		tmp.Format(_T("%d fps"), p->sentFrameRate);
		m_staLocalVideoFPSVal.SetWindowText(tmp);
		tmp.Format(_T("%d X %d"), p->encodedFrameWidth, p->encodedFrameHeight);
		m_staLocalVideoResoultionVal.SetWindowText(tmp);
		delete p;
	}
	return TRUE;
}
