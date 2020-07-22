#include "stdafx.h"
#include "APIExample.h"
#include "CAgoraOriginalVideoDlg.h"



IMPLEMENT_DYNAMIC(CAgoraOriginalVideoDlg, CDialogEx)

CAgoraOriginalVideoDlg::CAgoraOriginalVideoDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_ORIGINAL_VIDEO, pParent)
{

}

CAgoraOriginalVideoDlg::~CAgoraOriginalVideoDlg()
{
}

//Initialize the Ctrl Text.
void CAgoraOriginalVideoDlg::InitCtrlText()
{
	m_staChannel.SetWindowText(commonCtrlChannel);
	m_btnJoinChannel.SetWindowText(commonCtrlJoinChannel);
	m_staVideoProc.SetWindowText(OriginalVideoCtrlProc);
	m_btnSetVideoProc.SetWindowText(OriginalVideoCtrlSetProc);
}


//Initialize the Agora SDK
bool CAgoraOriginalVideoDlg::InitAgora()
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
	return true;
}


//UnInitialize the Agora SDK
void CAgoraOriginalVideoDlg::UnInitAgora()
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
void CAgoraOriginalVideoDlg::RenderLocalVideo()
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
void CAgoraOriginalVideoDlg::ResumeStatus()
{
	InitCtrlText();
	m_staDetail.SetWindowText(_T(""));
	m_edtChannel.SetWindowText(_T(""));
	m_cmbVideoProc.SetCurSel(0);
	m_lstInfo.ResetContent();
	m_joinChannel = false;
	m_initialize = false;
	m_setVideoProc = false;
}

void CAgoraOriginalVideoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_VIDEO, m_staVideoArea);
	DDX_Control(pDX, IDC_STATIC_CHANNELNAME, m_staChannel);
	DDX_Control(pDX, IDC_EDIT_CHANNELNAME, m_edtChannel);
	DDX_Control(pDX, IDC_BUTTON_JOINCHANNEL, m_btnJoinChannel);
	DDX_Control(pDX, IDC_LIST_INFO_BROADCASTING, m_lstInfo);
	DDX_Control(pDX, IDC_STATIC_DETAIL, m_staDetail);
	DDX_Control(pDX, IDC_STATIC_ORIGINAL_VIDEO_PROC, m_staVideoProc);
	DDX_Control(pDX, IDC_COMBO_ORIGINAL_VIDEO_PROC, m_cmbVideoProc);
	DDX_Control(pDX, IDC_BUTTON_SET_ORIGINAL_PROC, m_btnSetVideoProc);
}


BEGIN_MESSAGE_MAP(CAgoraOriginalVideoDlg, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_MESSAGE(WM_MSGID(EID_JOINCHANNEL_SUCCESS), &CAgoraOriginalVideoDlg::OnEIDJoinChannelSuccess)
	ON_MESSAGE(WM_MSGID(EID_LEAVE_CHANNEL), &CAgoraOriginalVideoDlg::OnEIDLeaveChannel)
	ON_MESSAGE(WM_MSGID(EID_USER_JOINED), &CAgoraOriginalVideoDlg::OnEIDUserJoined)
	ON_MESSAGE(WM_MSGID(EID_USER_OFFLINE), &CAgoraOriginalVideoDlg::OnEIDUserOffline)
	ON_MESSAGE(WM_MSGID(EID_REMOTE_VIDEO_STATE_CHANED), &CAgoraOriginalVideoDlg::OnEIDRemoteVideoStateChanged)
	ON_LBN_SELCHANGE(IDC_LIST_INFO_BROADCASTING, &CAgoraOriginalVideoDlg::OnSelchangeListInfoBroadcasting)
	ON_BN_CLICKED(IDC_BUTTON_JOINCHANNEL, &CAgoraOriginalVideoDlg::OnBnClickedButtonJoinchannel)
	ON_BN_CLICKED(IDC_BUTTON_SET_ORIGINAL_PROC, &CAgoraOriginalVideoDlg::OnBnClickedButtonSetOriginalProc)
END_MESSAGE_MAP()




void CAgoraOriginalVideoDlg::OnShowWindow(BOOL bShow, UINT nStatus)
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


BOOL CAgoraOriginalVideoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_localVideoWnd.Create(NULL, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CRect(0, 0, 1, 1), this, ID_BASEWND_VIDEO + 100);
	RECT rcArea;
	m_staVideoArea.GetClientRect(&rcArea);
	m_localVideoWnd.MoveWindow(&rcArea);
	m_localVideoWnd.ShowWindow(SW_SHOW);


	int i = 0;
	m_cmbVideoProc.InsertString(i++, _T("gray"));
	m_mapVideoFrame.insert(std::make_pair(_T("gray"), &m_garyVideoFrameObserver));
	m_cmbVideoProc.InsertString(i++, _T("average_filter"));
	m_mapVideoFrame.insert(std::make_pair(_T("average_filter"), &m_averageFilterVideoFrameObserver));
	ResumeStatus();
	return TRUE;  
}


/*
	register or unregister agora video Frame Observer.
*/
BOOL CAgoraOriginalVideoDlg::RegisterVideoFrameObserver(BOOL bEnable,IVideoFrameObserver * videoFrameObserver)
{
	agora::util::AutoPtr<agora::media::IMediaEngine> mediaEngine;
	//query interface agora::AGORA_IID_MEDIA_ENGINE in the engine.
	mediaEngine.queryInterface(m_rtcEngine, agora::AGORA_IID_MEDIA_ENGINE);
	int nRet = 0;
	AParameter apm(*m_rtcEngine);
	if (mediaEngine.get() == NULL)
		return FALSE;
	if (bEnable) {
		//register agora video frame observer.
		nRet = mediaEngine->registerVideoFrameObserver(videoFrameObserver);
	}
	else {
		//unregister agora video frame observer.
		nRet = mediaEngine->registerVideoFrameObserver(NULL);
	}
	return nRet == 0 ? TRUE : FALSE;
}


void CAgoraOriginalVideoDlg::OnBnClickedButtonJoinchannel()
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





void CAgoraOriginalVideoDlg::OnBnClickedButtonSetOriginalProc()
{
	if (!m_setVideoProc)
	{
		CString strProc;
		CString strInfo;
		m_cmbVideoProc.GetWindowText(strProc);
		if (strProc.IsEmpty())return;
		//register video frame observer from m_mapVideoFrame[strProc].
		RegisterVideoFrameObserver(TRUE, m_mapVideoFrame[strProc]);
		strInfo.Format(_T("set process:%s"), strProc);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
		m_btnSetVideoProc.SetWindowText(OriginalVideoCtrlUnSetProc);
	}
	else {
		//resume video frame observer.
		RegisterVideoFrameObserver(FALSE);
		m_btnSetVideoProc.SetWindowText(OriginalVideoCtrlSetProc);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("cancel the process"));
	}
	m_setVideoProc = !m_setVideoProc;
}



//see the header file for details
bool CGrayVideoProcFrameObserver::onCaptureVideoFrame(VideoFrame & videoFrame)
{
	int nSize = videoFrame.height * videoFrame.width;
	//set UV to 128 to mask color information
	memset(videoFrame.uBuffer, 128, nSize / 4);
	memset(videoFrame.vBuffer, 128, nSize / 4);
	return true;
}

//see the header file for details
bool CGrayVideoProcFrameObserver::onRenderVideoFrame(unsigned int uid, VideoFrame & videoFrame)
{
	return true;
}

//see the header file for details
bool CAverageFilterVideoProcFrameObserver::onCaptureVideoFrame(VideoFrame & videoFrame)
{
	AverageFiltering((unsigned char *)videoFrame.yBuffer, videoFrame.width, videoFrame.height, 3);
	AverageFiltering((unsigned char *)videoFrame.uBuffer, videoFrame.width / 2, videoFrame.height / 2, 3);
	AverageFiltering((unsigned char *)videoFrame.vBuffer, videoFrame.width / 2, videoFrame.height / 2, 3);
	return true;
}

//see the header file for details
bool CAverageFilterVideoProcFrameObserver::onRenderVideoFrame(unsigned int uid, VideoFrame & videoFrame)
{
	return true;
}

//average filtering algorithm
void CAverageFilterVideoProcFrameObserver::AverageFiltering(unsigned char * data, int width, int height, int step)
{
	assert(data && width > 0 && height > 0);
	if (step > width || step > height)
		return;
	if (step == 1)return;
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			int ans = 0;
			int dt = i - step / 2;
			int dl = j - step / 2;
			int cnt = 0;
			for (int k = max(0, dt); k < min(height - 1, dt + step); k++) {
				for (int l = max(dl, 0); l < min(dl + step, width - 1); l++) {
					ans += data[k*width + l];
					cnt++;
				}
			}
			ans /= cnt;
			m_buffer[i*width + j] = ans;
		}
	}
	memcpy(data, m_buffer, width*height);
}


//EID_JOINCHANNEL_SUCCESS message window handler
LRESULT CAgoraOriginalVideoDlg::OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam)
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
LRESULT CAgoraOriginalVideoDlg::OnEIDLeaveChannel(WPARAM wParam, LPARAM lParam)
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
LRESULT CAgoraOriginalVideoDlg::OnEIDUserJoined(WPARAM wParam, LPARAM lParam)
{
	CString strInfo;
	strInfo.Format(_T("%u joined"), wParam);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	return 0;
}

//EID_USER_OFFLINE message handler.
LRESULT CAgoraOriginalVideoDlg::OnEIDUserOffline(WPARAM wParam, LPARAM lParam)
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
LRESULT CAgoraOriginalVideoDlg::OnEIDRemoteVideoStateChanged(WPARAM wParam, LPARAM lParam)
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
	uid: user ID。If the UID is specified in the joinChannel, that ID is returned here;
	Otherwise, use the ID automatically assigned by the Agora server.
	elapsed: The Time from the joinChannel until this event occurred (ms).
*/
void COriginalVideoEventHandler::onJoinChannelSuccess(const char* channel, uid_t uid, int elapsed)
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
void COriginalVideoEventHandler::onUserJoined(uid_t uid, int elapsed)
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
void COriginalVideoEventHandler::onUserOffline(uid_t uid, USER_OFFLINE_REASON_TYPE reason)
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

void COriginalVideoEventHandler::onLeaveChannel(const RtcStats& stats)
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
void COriginalVideoEventHandler::onRemoteVideoStateChanged(uid_t uid, REMOTE_VIDEO_STATE state, REMOTE_VIDEO_STATE_REASON reason, int elapsed)
{
	if (m_hMsgHanlder) {
		PVideoStateStateChanged stateChanged = new VideoStateStateChanged;
		stateChanged->uid = uid;
		stateChanged->reason = reason;
		stateChanged->state = state;
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_REMOTE_VIDEO_STATE_CHANED), (WPARAM)stateChanged, 0);
	}
}




BOOL CAgoraOriginalVideoDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) {
		return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}




void CAgoraOriginalVideoDlg::OnSelchangeListInfoBroadcasting()
{
	int sel = m_lstInfo.GetCurSel();
	if (sel < 0)return;
	CString strDetail;
	m_lstInfo.GetText(sel, strDetail);
	m_staDetail.SetWindowText(strDetail);
}

