// CJoinChannelVideoByTokenDlg.cpp : implementation file

#include "stdafx.h"
#include "APIExample.h"
#include "CJoinChannelVideoByTokenDlg.h"

/*
note:
    Join the channel callback.This callback method indicates that the client
    successfully joined the specified channel.Channel ids are assigned based
    on the channel name specified in the joinChannel. If IRtcEngine::joinChannel
    is called without a user ID specified. The server will automatically assign one
parameters:
    channel:channel name.
    uid: user ID¡£If the UID is specified in the joinChannel, that ID is returned here;
    Otherwise, use the ID automatically assigned by the Agora server.
    elapsed: The Time from the joinChannel until this event occurred (ms).
*/
void CJoinChannelVideoByTokenRtcEngineEventHandler::onJoinChannelSuccess(const char* channel, uid_t uid, int elapsed)
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
    by the callback£¨ms).
*/
void CJoinChannelVideoByTokenRtcEngineEventHandler::onUserJoined(uid_t uid, int elapsed) {
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
void CJoinChannelVideoByTokenRtcEngineEventHandler::onUserOffline(uid_t uid, USER_OFFLINE_REASON_TYPE reason)
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
void CJoinChannelVideoByTokenRtcEngineEventHandler::onLeaveChannel(const RtcStats& stats)
{
    if (m_hMsgHanlder) {
        ::PostMessage(m_hMsgHanlder, WM_MSGID(EID_LEAVE_CHANNEL), 0, 0);
    }
}


void CJoinChannelVideoByTokenRtcEngineEventHandler::onLocalVideoStats(VIDEO_SOURCE_TYPE source, const LocalVideoStats& stats)
{
	if (m_hMsgHanlder && report) {
		LocalVideoStats* s = new LocalVideoStats;
		*s = stats;
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_LOCAL_VIDEO_STATS), (WPARAM)s, 0);
	}
}

// CJoinChannelVideoByTokenDlg dialog
IMPLEMENT_DYNAMIC(CJoinChannelVideoByTokenDlg, CDialogEx)

CJoinChannelVideoByTokenDlg::CJoinChannelVideoByTokenDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_LIVEBROADCASTING, pParent)
{

}

CJoinChannelVideoByTokenDlg::~CJoinChannelVideoByTokenDlg()
{
}

void CJoinChannelVideoByTokenDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_ROLE, m_cmbRole);
	DDX_Control(pDX, IDC_STATIC_ROLE, m_staRole);
	DDX_Control(pDX, IDC_EDIT_CHANNELNAME, m_edtChannelName);
	DDX_Control(pDX, IDC_EDIT_TOKEN, m_edtToken);
	DDX_Control(pDX, IDC_BUTTON_JOINCHANNEL, m_btnJoinChannel);
	DDX_Control(pDX, IDC_LIST_INFO_BROADCASTING, m_lstInfo);
	DDX_Control(pDX, IDC_STATIC_VIDEO, m_videoArea);
	DDX_Control(pDX, IDC_COMBO_PERSONS, m_cmbPersons);
	DDX_Control(pDX, IDC_STATIC_PERSONS, m_staPersons);
	DDX_Control(pDX, IDC_STATIC_CHANNELNAME, m_staChannelName);
	DDX_Control(pDX, IDC_EDIT_DETAIL_INFO, m_edtDetailInfo);
}


BEGIN_MESSAGE_MAP(CJoinChannelVideoByTokenDlg, CDialogEx)
    ON_BN_CLICKED(IDC_BUTTON_JOINCHANNEL, &CJoinChannelVideoByTokenDlg::OnBnClickedButtonJoinchannel)
    ON_CBN_SELCHANGE(IDC_COMBO_PERSONS, &CJoinChannelVideoByTokenDlg::OnSelchangeComboPersons)
    ON_CBN_SELCHANGE(IDC_COMBO_ROLE, &CJoinChannelVideoByTokenDlg::OnSelchangeComboRole)
    ON_MESSAGE(WM_MSGID(EID_JOINCHANNEL_SUCCESS), &CJoinChannelVideoByTokenDlg::OnEIDJoinChannelSuccess)
    ON_MESSAGE(WM_MSGID(EID_ERROR), &CJoinChannelVideoByTokenDlg::OnEIDError)
    ON_MESSAGE(WM_MSGID(EID_LEAVE_CHANNEL), &CJoinChannelVideoByTokenDlg::OnEIDLeaveChannel)
    ON_MESSAGE(WM_MSGID(EID_USER_JOINED), &CJoinChannelVideoByTokenDlg::OnEIDUserJoined)
    ON_MESSAGE(WM_MSGID(EID_USER_OFFLINE), &CJoinChannelVideoByTokenDlg::OnEIDUserOffline)

	ON_MESSAGE(WM_MSGID(EID_LOCAL_AUDIO_STATS), &CJoinChannelVideoByTokenDlg::onEIDLocalAudioStats)
	ON_MESSAGE(WM_MSGID(EID_REMOTE_AUDIO_STATS), &CJoinChannelVideoByTokenDlg::onEIDRemoteAudioStats)
	ON_MESSAGE(WM_MSGID(EID_LOCAL_VIDEO_STATS), &CJoinChannelVideoByTokenDlg::onEIDLocalVideoStats)
	ON_MESSAGE(WM_MSGID(EID_REMOTE_VIDEO_STATS), &CJoinChannelVideoByTokenDlg::onEIDRemoteVideoStats)

    ON_WM_SHOWWINDOW()
    ON_LBN_SELCHANGE(IDC_LIST_INFO_BROADCASTING, &CJoinChannelVideoByTokenDlg::OnSelchangeListInfoBroadcasting)
    ON_STN_CLICKED(IDC_STATIC_VIDEO, &CJoinChannelVideoByTokenDlg::OnStnClickedStaticVideo)
END_MESSAGE_MAP()


// CJoinChannelVideoByTokenDlg message handlers
BOOL CJoinChannelVideoByTokenDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    CreateAllVideoWnds();
    // clientRole
    int i = 0;
    m_cmbRole.InsertString(i++, agoraRoleBroadcaster);
    m_cmbRole.InsertString(i++, agoraRoleAudience);
    // person number
    i = 0;
    m_cmbPersons.InsertString(i++, _T("1V1"));
    m_cmbPersons.InsertString(i++, _T("1V3"));
    m_cmbPersons.InsertString(i++, _T("1V8"));
    m_cmbPersons.InsertString(i++, _T("1V15"));

	ResumeStatus();
    return TRUE;
}


//set control text from config.
void CJoinChannelVideoByTokenDlg::InitCtrlText()
{
    m_staRole.SetWindowText(commonCtrlClientRole);
    m_staPersons.SetWindowText(liveCtrlPersons);
    m_staChannelName.SetWindowText(commonCtrlChannel);
    m_btnJoinChannel.SetWindowText(commonCtrlJoinChannel);
}

//create all video window to save m_videoWnds.
void CJoinChannelVideoByTokenDlg::CreateAllVideoWnds()
{
    for (int i = 0; i < VIDEO_COUNT; ++i) {
        m_videoWnds[i].Create(NULL, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CRect(0, 0, 1, 1), this, IDC_BASEWND_VIDEO + i);
        //set window background color.
        m_videoWnds[i].SetFaceColor(RGB(0x58, 0x58, 0x58));
    }
}

//show all video window from m_videoWnds.
void CJoinChannelVideoByTokenDlg::ShowVideoWnds()
{
    m_videoArea.ShowWindow(SW_HIDE);
    int row = 2;
    int col = 2;
    m_maxVideoCount = 4;
    switch (m_cmbPersons.GetCurSel()) {
    case PEOPLE_IN_CHANNEL_2: {
        row = 1;
        col = 2;
        m_maxVideoCount = 2;
    }
        break;
    case PEOPLE_IN_CHANNEL_4: {
        int row = 2;
        int col = 2;
        m_maxVideoCount = 4;
    }
        break;
    case PEOPLE_IN_CHANNEL_9: {
        row = 3;
        col = 3;
        m_maxVideoCount = 9;
    }
        break;
    case PEOPLE_IN_CHANNEL_16: {
        row = 4;
        col = 4;
        m_maxVideoCount = 16;
    }
        break;
    }
    

    RECT rcArea;
    m_videoArea.GetClientRect( &rcArea);
    int space = 1;
    
    int w = (rcArea.right -rcArea.left - space * (col - 1)) / col;
    int h = (rcArea.bottom - rcArea.top - space * (row - 1)) / row;
    
    for (int r = 0; r < row; r++) {
        for (int c = 0; c < col; c++) {
            int x = rcArea.left + (w + space) * c;
            int y = rcArea.top + (h + space) * r;
            int nIndex = r * col + c;
            m_videoWnds[nIndex].MoveWindow(x, y, w, h, TRUE);
            m_videoWnds[nIndex].ShowWindow(SW_SHOW);
            m_videoWnds[nIndex].SetParent(this);

            if (!m_videoWnds[nIndex].IsWindowVisible()) {
                m_videoWnds[nIndex].ShowWindow(SW_SHOW);
            }
        }
    }

    for (int i = m_maxVideoCount; i < VIDEO_COUNT; i++) {
        m_videoWnds[i].ShowWindow(0);
        if (m_videoWnds[i].GetUID() != 0) {
            VideoCanvas canvas;
            canvas.uid = m_videoWnds[i].GetUID();
            canvas.view = nullptr;
            m_rtcEngine->setupRemoteVideo(canvas);
			m_videoWnds[i].SetUID(0);
        }
    }
}
//Initialize the Agora SDK
bool CJoinChannelVideoByTokenDlg::InitAgora()
{
    //create Agora RTC engine
    m_rtcEngine = createAgoraRtcEngine();
    if (!m_rtcEngine) {
        m_lstInfo.InsertString(m_lstInfo.GetCount()-1, _T("createAgoraRtcEngine failed"));
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
	m_rtcEngine->enableAudio();
    m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("enable video"));
   
    m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("live broadcasting"));

	m_audioDeviceManager = new AAudioDeviceManager(m_rtcEngine);

	m_rtcEngine->setVideoEncoderConfiguration(m_videoEncoderConfig);
    return true;
}

//UnInitialize the Agora SDK
void CJoinChannelVideoByTokenDlg::UnInitAgora()
{
    if (m_rtcEngine) {
		if(m_joinChannel)
		//leave channel
		m_rtcEngine->leaveChannel();
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("leaveChannel"));
        //stop preview in the engine.
       // m_rtcEngine->stopPreview();
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

void CJoinChannelVideoByTokenDlg::ResumeStatus()
{
	m_lstInfo.ResetContent();
	m_cmbRole.SetCurSel(0);
	m_cmbPersons.SetCurSel(0);
	ShowVideoWnds();
	InitCtrlText();
	
	m_btnJoinChannel.EnableWindow(TRUE);
	m_cmbRole.EnableWindow(TRUE);
	m_edtChannelName.SetWindowText(_T(""));
	m_edtToken.SetWindowText(_T(""));
	m_joinChannel = false;
	m_initialize = false;
}

//render local video from SDK local capture.
void CJoinChannelVideoByTokenDlg::RenderLocalVideo()
{
    if (m_rtcEngine) {
        //start preview in the engine.
		m_rtcEngine->enableLocalVideo(true);
        m_rtcEngine->startPreview();
        VideoCanvas canvas;
        canvas.renderMode = m_canvasRenderMode;
        canvas.uid = 0;
        canvas.view = m_videoWnds[0].GetSafeHwnd();
		canvas.backgroundColor = m_canvasColor;
        //setup local video in the engine to the canvas. 
        m_rtcEngine->setupLocalVideo(canvas);
		m_videoWnds[0].ShowStatsInfo(TRUE, FALSE);
        m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("render local video"));
    }
}

void CJoinChannelVideoByTokenDlg::StopLocalVideo()
{
	if (m_rtcEngine) {
		//start preview in the engine.
		m_rtcEngine->stopPreview();
		VideoCanvas canvas;
		canvas.uid = 0;
		canvas.view = NULL;
		//setup local video in the engine to the canvas. 
		m_rtcEngine->setupLocalVideo(canvas);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("stop local video"));
	}
}


void CJoinChannelVideoByTokenDlg::ResetVideoView()
{
	VideoCanvas canvas;
	canvas.uid = 0;
	canvas.renderMode = m_canvasRenderMode;
	m_rtcEngine->setupLocalVideo(canvas);
	canvas.view = m_videoWnds[0].GetSafeHwnd();
	canvas.backgroundColor = m_canvasColor;
	m_rtcEngine->setupLocalVideo(canvas);

	for (int i = 1; i < m_maxVideoCount; i++) {
		int uid = m_videoWnds[i].GetUID();
		if (uid != 0) {
			VideoCanvas canvas;
			canvas.uid = uid;
			canvas.renderMode = m_canvasRenderMode;
			canvas.view = m_videoWnds[i].GetSafeHwnd();
			canvas.backgroundColor = m_canvasColor;
			//setup remote video in engine to the canvas.
			m_rtcEngine->setupRemoteVideo(canvas);
			m_videoWnds[i].ShowStatsInfo(TRUE);
			break;
		}
	}
}

void CJoinChannelVideoByTokenDlg::OnSelchangeComboPersons()
{
    int index = m_cmbPersons.GetCurSel();
    ShowVideoWnds();
}


void CJoinChannelVideoByTokenDlg::OnSelchangeComboRole()
{
    if (m_rtcEngine) {
        m_rtcEngine->setClientRole(m_cmbRole.GetCurSel() == 0 ? CLIENT_ROLE_BROADCASTER : CLIENT_ROLE_AUDIENCE);

		if (m_cmbRole.GetCurSel() == 0) {
			// start video and render local video
			RenderLocalVideo();
		} else {
			// stop video and unbind local video
			StopLocalVideo();
		}

        m_lstInfo.InsertString(m_lstInfo.GetCount(), m_cmbRole.GetCurSel() == 0 ? _T("setClientRole broadcaster"): _T("setClientRole Audience"));
    }
}


void CJoinChannelVideoByTokenDlg::OnBnClickedButtonJoinchannel()
{
    if (!m_rtcEngine || !m_initialize)
        return;
    CString strInfo;
    if (!m_joinChannel) {
        CString strChannelName;
        m_edtChannelName.GetWindowText(strChannelName);
        if (strChannelName.IsEmpty()) {
            AfxMessageBox(_T("Fill channel name first"));
            return;
        }

        CString strToken;
        m_edtToken.GetWindowText(strToken);

		VideoEncoderConfiguration config;
		m_rtcEngine->setVideoEncoderConfiguration(config);
        std::string szChannelId = cs2utf8(strChannelName);

		ChannelMediaOptions options;
		options.channelProfile = CHANNEL_PROFILE_LIVE_BROADCASTING;
		options.clientRoleType = CLIENT_ROLE_TYPE(m_cmbRole.GetCurSel() + 1);
		options.autoSubscribeAudio = true;
		options.autoSubscribeVideo = true;
        //join channel in the engine.
        int ret = m_rtcEngine->joinChannel(cs2utf8(strToken).c_str(), szChannelId.c_str(), m_uid, options);
        if (ret == 0) {
            strInfo.Format(_T("join channel %s, use ChannelMediaOptions"), getCurrentTime());
            m_btnJoinChannel.EnableWindow(FALSE);
            m_joinChannel = true;
            m_btnJoinChannel.SetWindowText(commonCtrlLeaveChannel);
        }
        else {
            const char* des = getAgoraSdkErrorDescription(ret);
            strInfo.Format(_T("join channel failed >> code=%d, des=%s"), ret, utf82cs(std::string(des)));
        }
    }
    else {
        int ret = m_rtcEngine->leaveChannel();
        if (0 == ret) {
            strInfo.Format(_T("leave channel %s"), getCurrentTime());
            m_btnJoinChannel.EnableWindow(FALSE);
            m_joinChannel = false;
            m_btnJoinChannel.SetWindowText(commonCtrlJoinChannel);
        }
        else {
            const char* des = getAgoraSdkErrorDescription(ret);
            strInfo.Format(_T("leave channel failed >> code=%d, des=%s"), ret, utf82cs(std::string(des)));
        }
    }

    m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
}



void CJoinChannelVideoByTokenDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
    CDialogEx::OnShowWindow(bShow, nStatus);

    if (bShow) {
        RenderLocalVideo();
	}
	else
	{
		ResumeStatus();
	}
}

LRESULT CJoinChannelVideoByTokenDlg::OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam)
{
    m_btnJoinChannel.EnableWindow(TRUE);
	m_edtChannelName.EnableWindow(TRUE);
	m_edtToken.EnableWindow(TRUE);
	
	

    CString strInfo;
    strInfo.Format(_T("%s:join success, uid=%u"), getCurrentTime(), wParam);
    m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

    m_videoWnds[0].SetUID(wParam);
    m_lstUids.push_back(wParam);
    //notify parent window
    ::PostMessage(GetParent()->GetSafeHwnd(), WM_MSGID(EID_JOINCHANNEL_SUCCESS), TRUE, 0);
    return 0;
}

LRESULT CJoinChannelVideoByTokenDlg::OnEIDError(WPARAM wParam, LPARAM lParam)
{
    m_btnJoinChannel.EnableWindow(TRUE);
    m_edtChannelName.EnableWindow(TRUE);
    m_edtToken.EnableWindow(TRUE);

    CString strInfo;
    char* message = (char*)wParam;
    int code = lParam;
    strInfo.Format(_T("Error >> code=%d, message=%s"), code, utf82cs(std::string(message)));
    m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
    delete message;

    
    return 0;
}

LRESULT CJoinChannelVideoByTokenDlg::OnEIDLeaveChannel(WPARAM wParam, LPARAM lParam)
{
    m_btnJoinChannel.EnableWindow(TRUE);
	m_joinChannel = false;
    m_btnJoinChannel.SetWindowText(commonCtrlJoinChannel);

    CString strInfo;
    strInfo.Format(_T("leave channel success %s"), getCurrentTime());
    m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
    m_lstUids.clear();
    for (int i = 0; i < m_maxVideoCount; i++) {
        m_videoWnds[i].SetUID(0);
    }

    //notify parent window
    ::PostMessage(GetParent()->GetSafeHwnd(), WM_MSGID(EID_JOINCHANNEL_SUCCESS), FALSE, 0);
    return 0;
}

LRESULT CJoinChannelVideoByTokenDlg::OnEIDUserJoined(WPARAM wParam, LPARAM lParam)
{
    if (m_lstUids.size() == m_maxVideoCount)
        return 0;
    CString strInfo;
    strInfo.Format(_T("%u joined"), wParam);
    m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

    m_lstUids.push_back(wParam);
    for (int i = 1; i < m_maxVideoCount; i++) {
        if (m_videoWnds[i].GetUID() == 0) {
            VideoCanvas canvas;
            canvas.uid  = wParam;
            canvas.view = m_videoWnds[i].GetSafeHwnd();
            canvas.renderMode = m_canvasRenderMode;
			canvas.backgroundColor = m_canvasColor;
            //setup remote video in engine to the canvas.
            m_rtcEngine->setupRemoteVideo(canvas);
			m_videoWnds[i].SetUID(wParam);
            break;
        }
    }
    return 0;
}

LRESULT CJoinChannelVideoByTokenDlg::OnEIDUserOffline(WPARAM wParam, LPARAM lParam)
{
    uid_t remoteUid = (uid_t)wParam;
    VideoCanvas canvas;
    canvas.uid = remoteUid;
    canvas.view = NULL;
    m_rtcEngine->setupRemoteVideo(canvas);
    CString strInfo;
    strInfo.Format(_T("%u offline, reason:%d"), remoteUid, lParam);
    m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

    for (int i = 1; i < m_maxVideoCount; i++){
        if (m_videoWnds[i].GetUID() == remoteUid) {
            m_videoWnds[i].SetUID(0);
            m_videoWnds[i].Invalidate();
            break;
        }
    }

    for (auto iter = m_lstUids.begin();
        iter != m_lstUids.end(); iter++){
        if (*iter == remoteUid) {
            m_lstUids.erase(iter);
            break;
        }
    }
    return 0;
}


void CJoinChannelVideoByTokenDlg::OnSelchangeListInfoBroadcasting()
{
    int sel = m_lstInfo.GetCurSel();
	if (sel < 0)return;
    CString strDetail;
    m_lstInfo.GetText(sel, strDetail);
	m_edtDetailInfo.SetWindowText(strDetail);
}


void CJoinChannelVideoByTokenDlg::OnStnClickedStaticVideo()
{
}


BOOL CJoinChannelVideoByTokenDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) {
		return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}



LRESULT CJoinChannelVideoByTokenDlg::onEIDLocalAudioStats(WPARAM wParam, LPARAM lParam) {
	LocalAudioStats* stats = (LocalAudioStats*)wParam;

	m_videoWnds[0].SetAudioStatsInfo(stats->sentBitrate, stats->txPacketLossRate);
	
	if (stats) {
		delete stats;
		stats = nullptr;
	}
	return 0;
}

LRESULT CJoinChannelVideoByTokenDlg::onEIDRemoteAudioStats(WPARAM wParam, LPARAM lParam) {
	RemoteAudioStats* stats = (RemoteAudioStats*)wParam;

	for (int i = 0; i < VIDEO_COUNT; i++)
	{
		if (m_videoWnds[i].GetUID() == stats->uid) {
			m_videoWnds[i].SetAudioStatsInfo(stats->receivedBitrate, stats->audioLossRate, stats->jitterBufferDelay);
			break;
		}
	}

	if (stats) {
		delete stats;
		stats = nullptr;
	}
	return 0;
}


LRESULT CJoinChannelVideoByTokenDlg::onEIDLocalVideoStats(WPARAM wParam, LPARAM lParam) {
	LocalVideoStats* stats = (LocalVideoStats*)wParam;

	m_videoWnds[0].SetVideoStatsInfo(stats->encodedFrameWidth, stats->encodedFrameHeight, stats->sentFrameRate, stats->sentBitrate, stats->txPacketLossRate);

	if (stats) {
		delete stats;
		stats = nullptr;
	}

	
	return 0;
}

LRESULT CJoinChannelVideoByTokenDlg::onEIDRemoteVideoStats(WPARAM wParam, LPARAM lParam) {
	RemoteVideoStats* stats = (RemoteVideoStats*)wParam;
	CString strInfo = _T("===onRemoteVideoStats===");
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

	strInfo.Format(_T("uid:%u"), stats->uid);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	strInfo.Format(_T("delay:%d"), stats->delay);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

	strInfo.Format(_T("width:%d"), stats->width);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	strInfo.Format(_T("height:%d"), stats->height);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

	strInfo.Format(_T("receivedBitrate:%d"), stats->receivedBitrate);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	strInfo.Format(_T("decoderOutputFrameRate:%d"), stats->decoderOutputFrameRate);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

	strInfo.Format(_T("rendererOutputFrameRate:%d"), stats->rendererOutputFrameRate);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	strInfo.Format(_T("frameLossRate:%d"), stats->frameLossRate);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

	strInfo.Format(_T("packetLossRate:%d"), stats->packetLossRate);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	strInfo.Format(_T("rxStreamType:%d"), stats->rxStreamType);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

	strInfo.Format(_T("totalFrozenTime:%d"), stats->totalFrozenTime);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	strInfo.Format(_T("frozenRate:%d"), stats->frozenRate);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

	strInfo.Format(_T("avSyncTimeMs:%d"), stats->avSyncTimeMs);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

	strInfo.Format(_T("totalActiveTime:%d"), stats->totalActiveTime);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	strInfo.Format(_T("publishDuration:%d"), stats->publishDuration);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

	for (int i = 0; i < VIDEO_COUNT; i++)
	{
		if (m_videoWnds[i].GetUID() == stats->uid) {
			m_videoWnds[i].SetVideoStatsInfo(stats->width, stats->height, stats->decoderOutputFrameRate, 
				stats->receivedBitrate, stats->packetLossRate, stats->delay);
			break;
		}
	}

	if (stats) {
		delete stats;
		stats = nullptr;
	}
	return 0;
}

