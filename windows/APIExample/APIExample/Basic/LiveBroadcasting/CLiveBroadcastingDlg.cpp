// CLiveBroadcastingDlg.cpp : implementation file

#include "stdafx.h"
#include "APIExample.h"
#include "CLiveBroadcastingDlg.h"

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
void CLiveBroadcastingRtcEngineEventHandler::onJoinChannelSuccess(const char* channel, uid_t uid, int elapsed)
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
void CLiveBroadcastingRtcEngineEventHandler::onUserJoined(uid_t uid, int elapsed) {
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
void CLiveBroadcastingRtcEngineEventHandler::onUserOffline(uid_t uid, USER_OFFLINE_REASON_TYPE reason)
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
void CLiveBroadcastingRtcEngineEventHandler::onLeaveChannel(const RtcStats& stats)
{
    if (m_hMsgHanlder) {
        ::PostMessage(m_hMsgHanlder, WM_MSGID(EID_LEAVE_CHANNEL), 0, 0);
    }
}

void CLiveBroadcastingRtcEngineEventHandler::onAudioDeviceStateChanged(const char* deviceId, int deviceType, int deviceState)
{
	if (m_hMsgHanlder) {
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_AUDIO_DEVICE_STATE_CHANGED), 0, 0);
	}
}

// CLiveBroadcastingDlg dialog
IMPLEMENT_DYNAMIC(CLiveBroadcastingDlg, CDialogEx)

CLiveBroadcastingDlg::CLiveBroadcastingDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_LIVEBROADCASTING, pParent)
{

}

CLiveBroadcastingDlg::~CLiveBroadcastingDlg()
{
}

void CLiveBroadcastingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_ROLE, m_cmbRole);
	DDX_Control(pDX, IDC_STATIC_ROLE, m_staRole);
	DDX_Control(pDX, IDC_EDIT_CHANNELNAME, m_edtChannelName);
	DDX_Control(pDX, IDC_BUTTON_JOINCHANNEL, m_btnJoinChannel);
	DDX_Control(pDX, IDC_LIST_INFO_BROADCASTING, m_lstInfo);
	DDX_Control(pDX, IDC_STATIC_VIDEO, m_videoArea);
	DDX_Control(pDX, IDC_COMBO_PERSONS, m_cmbPersons);
	DDX_Control(pDX, IDC_STATIC_PERSONS, m_staPersons);
	DDX_Control(pDX, IDC_STATIC_CHANNELNAME, m_staChannelName);
	DDX_Control(pDX, IDC_STATIC_DETAIL, m_staDetail);
	DDX_Control(pDX, IDC_SLIDER_LOOPBACK, m_sldVolume);
	DDX_Control(pDX, IDC_CHECK_LOOPBACK, m_chkEnable);
	DDX_Control(pDX, IDC_COMBO_LOOPBACK_DEVICE, m_cmbLoopbackDevice);
	DDX_Control(pDX, IDC_STATIC_LOOPBACK_DEVICE, m_staLoopbackDevice);
	DDX_Control(pDX, IDC_STATIC_LOOPBACK_VOLUME, m_staLoopVolume);
	DDX_Control(pDX, IDC_STATIC_AUDIENCE_LATENCY, m_staAudienceLatency);
	DDX_Control(pDX, IDC_COMBO_AUDIENCE_LATENCY, m_cmbLatency);
	DDX_Control(pDX, IDC_STATIC_BACKGROUND, m_staBackground);
	DDX_Control(pDX, IDC_COMBO_BACKGROUND_TYPE, m_cmbBackground);
	DDX_Control(pDX, IDC_STATIC_COLOR, m_staBackColor);
	DDX_Control(pDX, IDC_COMBO_COLOR, m_cmbColor);
	DDX_Control(pDX, IDC_BUTTON_IMAGE, m_btnImagePath);
	DDX_Control(pDX, IDC_CHECK_ENABLE_BACKGROUND, m_chkEnableBackground);
	DDX_Control(pDX, IDC_EDIT_IMAGE_PATH, m_edtImagePath);
	DDX_Control(pDX, IDC_CHECK_REPORT, m_chkReport);
}


BEGIN_MESSAGE_MAP(CLiveBroadcastingDlg, CDialogEx)
    ON_BN_CLICKED(IDC_BUTTON_JOINCHANNEL, &CLiveBroadcastingDlg::OnBnClickedButtonJoinchannel)
    ON_CBN_SELCHANGE(IDC_COMBO_PERSONS, &CLiveBroadcastingDlg::OnSelchangeComboPersons)
    ON_CBN_SELCHANGE(IDC_COMBO_ROLE, &CLiveBroadcastingDlg::OnSelchangeComboRole)
    ON_MESSAGE(WM_MSGID(EID_JOINCHANNEL_SUCCESS), &CLiveBroadcastingDlg::OnEIDJoinChannelSuccess)
    ON_MESSAGE(WM_MSGID(EID_LEAVE_CHANNEL), &CLiveBroadcastingDlg::OnEIDLeaveChannel)
    ON_MESSAGE(WM_MSGID(EID_USER_JOINED), &CLiveBroadcastingDlg::OnEIDUserJoined)
    ON_MESSAGE(WM_MSGID(EID_USER_OFFLINE), &CLiveBroadcastingDlg::OnEIDUserOffline)
    ON_WM_SHOWWINDOW()
    ON_LBN_SELCHANGE(IDC_LIST_INFO_BROADCASTING, &CLiveBroadcastingDlg::OnSelchangeListInfoBroadcasting)
    ON_STN_CLICKED(IDC_STATIC_VIDEO, &CLiveBroadcastingDlg::OnStnClickedStaticVideo)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_CHECK_LOOPBACK, &CLiveBroadcastingDlg::OnClickedCheckLoopback)
	ON_CBN_SELCHANGE(IDC_COMBO_AUDIENCE_LATENCY, &CLiveBroadcastingDlg::OnSelchangeComboAudienceLatency)
	ON_STN_CLICKED(IDC_STATIC_DETAIL, &CLiveBroadcastingDlg::OnStnClickedStaticDetail)
	ON_BN_CLICKED(IDC_BUTTON_IMAGE, &CLiveBroadcastingDlg::OnBnClickedButtonImage)
	ON_BN_CLICKED(IDC_CHECK_ENABLE_BACKGROUND, &CLiveBroadcastingDlg::OnBnClickedCheckEnableBackground)
	ON_CBN_SELCHANGE(IDC_COMBO_BACKGROUND_TYPE, &CLiveBroadcastingDlg::OnSelchangeComboBackgroundType)


	ON_MESSAGE(WM_MSGID(EID_NETWORK_QUALITY), &CLiveBroadcastingDlg::OnEIDNetworkQuality)
	ON_MESSAGE(WM_MSGID(EID_RTC_STATS), &CLiveBroadcastingDlg::onEIDRtcStats)
	ON_MESSAGE(WM_MSGID(EID_LOCAL_AUDIO_STATS), &CLiveBroadcastingDlg::onEIDLocalAudioStats)
	ON_MESSAGE(WM_MSGID(EID_LOCAL_AUDIO_STATE_CHANED), &CLiveBroadcastingDlg::onEIDLocalAudioStateChanged)
	ON_MESSAGE(WM_MSGID(EID_REMOTE_AUDIO_STATS), &CLiveBroadcastingDlg::onEIDRemoteAudioStats)
	ON_MESSAGE(WM_MSGID(EID_REMOTE_AUDIO_STATE_CHANGED), &CLiveBroadcastingDlg::onEIDRemoteAudioStateChanged)
	ON_MESSAGE(WM_MSGID(EID_LOCAL_VIDEO_STATS), &CLiveBroadcastingDlg::onEIDLocalVideoStats)
	ON_MESSAGE(WM_MSGID(EID_LOCAL_VIDEO_STATE_CHANGED), &CLiveBroadcastingDlg::onEIDLocalVideoStateChanged)
	ON_MESSAGE(WM_MSGID(EID_REMOTE_VIDEO_STATS), &CLiveBroadcastingDlg::onEIDRemoteVideoStats)
	ON_BN_CLICKED(IDC_CHECK_REPORT, &CLiveBroadcastingDlg::OnBnClickedCheckReport)
	ON_CBN_SELCHANGE(IDC_COMBO_COLOR, &CLiveBroadcastingDlg::OnSelchangeComboColor)
END_MESSAGE_MAP()


// CLiveBroadcastingDlg message handlers
BOOL CLiveBroadcastingDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    CreateAllVideoWnds();
    // clientRole
    int i = 0;
    m_cmbRole.InsertString(i++, agoraRoleBroadcaster);
    m_cmbRole.InsertString(i++, agoraRoleAudience);
    //
    i = 0;
    m_cmbPersons.InsertString(i++, _T("1V1"));
    m_cmbPersons.InsertString(i++, _T("1V3"));
    m_cmbPersons.InsertString(i++, _T("1V8"));
    m_cmbPersons.InsertString(i++, _T("1V15"));

	i = 0;
	m_cmbLatency.InsertString(i++, liveCtrlAudienceLowLatency);
	m_cmbLatency.InsertString(i++, liveCtrlAudienceUltraLowLatency);
	i = 0;
	m_cmbBackground.InsertString(i++, videoBackgroundSourceTypeNone);
	m_cmbBackground.InsertString(i++, videoBackgroundSourceTypeColor);
	m_cmbBackground.InsertString(i++, videoBackgroundSourceTypeImg);
	m_cmbBackground.InsertString(i++, videoBackgroundSourceTypeBlur);
	m_cmbBackground.SetCurSel(0);
	i = 0;
	m_cmbColor.InsertString(i++, videoBackgroundSourceTypeRed);
	m_cmbColor.InsertString(i++, videoBackgroundSourceTypeGreen);
	m_cmbColor.InsertString(i++, videoBackgroundSourceTypeBlue);
	m_cmbColor.SetCurSel(0);
	m_chkEnableBackground.SetWindowText(videoBackgroundSourceTypeEnable);
	m_btnImagePath.SetWindowText(videoBackgroundSourceTypeImagePath);
	m_staBackColor.SetWindowText(videoBackgroundSourceTypeColor);
	m_staBackground.SetWindowText(videoBackgroundSourceType);
	ResumeStatus();
	m_sldVolume.SetRange(0, 100);
	m_sldVolume.EnableWindow(FALSE);

	m_staLoopbackDevice.SetWindowText(liveCtrlLoopbackDevice);
	m_staLoopVolume.SetWindowText(liveCtrlLoopbackVolume);
	m_chkEnable.SetWindowText(liveCtrlLoopbackEnable);
	m_cmbLatency.EnableWindow(FALSE);
    return TRUE;
}


//set control text from config.
void CLiveBroadcastingDlg::InitCtrlText()
{
    m_staRole.SetWindowText(commonCtrlClientRole);
    m_staPersons.SetWindowText(liveCtrlPersons);
    m_staChannelName.SetWindowText(commonCtrlChannel);
    m_btnJoinChannel.SetWindowText(commonCtrlJoinChannel);
	m_staAudienceLatency.SetWindowText(liveCtrlAudienceLatency);

	m_staBackColor.SetWindowText(videoBackgroundSourceTypeColor);
	m_staBackground.SetWindowText(videoBackgroundSourceType);
	m_btnImagePath.SetWindowText(videoBackgroundSourceTypeImagePath);
	m_staBackColor.ShowWindow(SW_HIDE);
	m_staBackground.ShowWindow(SW_HIDE);
	m_cmbBackground.ShowWindow(SW_HIDE);
	m_cmbColor.ShowWindow(SW_HIDE);
	m_btnImagePath.ShowWindow(SW_HIDE);
}

//create all video window to save m_videoWnds.
void CLiveBroadcastingDlg::CreateAllVideoWnds()
{
    for (int i = 0; i < VIDEO_COUNT; ++i) {
        m_videoWnds[i].Create(NULL, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CRect(0, 0, 1, 1), this, IDC_BASEWND_VIDEO + i);
        //set window background color.

        m_videoWnds[i].SetFaceColor(RGB(0x58, 0x58, 0x58));
    }
}

//show all video window from m_videoWnds.
void CLiveBroadcastingDlg::ShowVideoWnds()
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
            canvas.view = m_videoWnds[i].GetSafeHwnd();
            m_rtcEngine->setupRemoteVideo(canvas);
        }
    }
}
//Initialize the Agora SDK
bool CLiveBroadcastingDlg::InitAgora()
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

	m_audioDeviceManager = new AAudioDeviceManager(m_rtcEngine);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("cereate audio device manager"));
	m_videoDeviceManager = new AVideoDeviceManager(m_rtcEngine);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("initialize video device manager"));

	m_playbackDevices = (*m_audioDeviceManager)->enumeratePlaybackDevices();
	for (int i = 0; i < m_playbackDevices->getCount(); ++i) {
		char id[MAX_DEVICE_ID_LENGTH] = { 0 };
		char name[MAX_DEVICE_ID_LENGTH] = { 0 };
		m_playbackDevices->getDevice(0, name, id);
		m_cmbLoopbackDevice.InsertString(i, utf82cs(name));
	}

	if (m_cmbLoopbackDevice.GetCount() > 0)
		m_cmbLoopbackDevice.SetCurSel(0);
    return true;
}

//UnInitialize the Agora SDK
void CLiveBroadcastingDlg::UnInitAgora()
{
    if (m_rtcEngine) {
		if(m_joinChannel)
		//leave channel
		m_rtcEngine->leaveChannel();
        //stop preview in the engine.
        m_rtcEngine->stopPreview();
        m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("stopPreview"));

		delete m_audioDeviceManager;
		delete m_videoDeviceManager;
		m_audioDeviceManager = nullptr;
		m_videoDeviceManager = nullptr;
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("release audio device manager"));
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("release video device manager"));

        //disable video in the engine.
        m_rtcEngine->disableVideo();
        m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("disableVideo"));
        //release engine.
        m_rtcEngine->release(true);
        m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("release rtc engine"));
        m_rtcEngine = NULL;
    }
}

void CLiveBroadcastingDlg::ResumeStatus()
{
	m_lstInfo.ResetContent();
	m_cmbRole.SetCurSel(0);
	m_cmbPersons.SetCurSel(0);
	m_cmbLatency.SetCurSel(0);
	ShowVideoWnds();
	InitCtrlText();
	m_btnJoinChannel.EnableWindow(TRUE);
	m_cmbRole.EnableWindow(TRUE);
	m_edtChannelName.SetWindowText(_T(""));
	m_sldVolume.EnableWindow(FALSE);
	m_chkEnable.SetCheck(0);
	m_joinChannel = false;
	m_initialize = false;
	m_cmbLoopbackDevice.Clear();
}

//render local video from SDK local capture.
void CLiveBroadcastingDlg::RenderLocalVideo()
{
    if (m_rtcEngine) {
        //start preview in the engine.
        m_rtcEngine->startPreview();
        VideoCanvas canvas;
        canvas.renderMode = RENDER_MODE_FIT;
        canvas.uid = 0;
        canvas.view = m_videoWnds[0].GetSafeHwnd();
        //setup local video in the engine to the canvas. 
        m_rtcEngine->setupLocalVideo(canvas);
        m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("render local video"));
    }
}


void CLiveBroadcastingDlg::OnSelchangeComboPersons()
{
    int index = m_cmbPersons.GetCurSel();
    ShowVideoWnds();
}


void CLiveBroadcastingDlg::OnSelchangeComboRole()
{
	if (m_cmbRole.GetCurSel() == 0) {
		m_cmbLatency.EnableWindow(FALSE);
	}
	else {
		m_cmbLatency.EnableWindow(TRUE);
	}
    if (m_rtcEngine) {
        m_rtcEngine->setClientRole(CLIENT_ROLE_TYPE(m_cmbRole.GetCurSel() + 1));

        m_lstInfo.InsertString(m_lstInfo.GetCount(), m_cmbRole.GetCurSel() == 0 ? _T("setClientRole broadcaster"): _T("setClientRole Audience"));
    }
}

void CLiveBroadcastingDlg::OnBnClickedButtonJoinchannel()
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
        std::string szChannelId = cs2utf8(strChannelName);
        //join channel in the engine.
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



void CLiveBroadcastingDlg::OnShowWindow(BOOL bShow, UINT nStatus)
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

LRESULT CLiveBroadcastingDlg::OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam)
{
    m_btnJoinChannel.EnableWindow(TRUE);
	m_joinChannel = true;
    m_btnJoinChannel.SetWindowText(commonCtrlLeaveChannel);

    CString strInfo;
    strInfo.Format(_T("%s:join success, uid=%u"), getCurrentTime(), wParam);
    m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

    m_videoWnds[0].SetUID(wParam);
    m_lstUids.push_back(wParam);

    //notify parent window
    ::PostMessage(GetParent()->GetSafeHwnd(), WM_MSGID(EID_JOINCHANNEL_SUCCESS), TRUE, 0);
    return 0;
}

LRESULT CLiveBroadcastingDlg::OnEIDLeaveChannel(WPARAM wParam, LPARAM lParam)
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

LRESULT CLiveBroadcastingDlg::OnEIDUserJoined(WPARAM wParam, LPARAM lParam)
{
    if (m_lstUids.size() == m_maxVideoCount)
        return 0;
    CString strInfo;
    strInfo.Format(_T("%u joined"), wParam);
    m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

    m_lstUids.push_back(wParam);
    for (int i = 0; i < m_maxVideoCount; i++) {
        if (m_videoWnds[i].GetUID() == 0) {
            VideoCanvas canvas;
            canvas.uid  = wParam;
            canvas.view = m_videoWnds[i].GetSafeHwnd();
            canvas.renderMode = RENDER_MODE_FIT;
			m_videoWnds[i].SetUID(wParam);
            //setup remote video in engine to the canvas.
            m_rtcEngine->setupRemoteVideo(canvas);
            break;
        }
    }
    return 0;
}

LRESULT CLiveBroadcastingDlg::OnEIDUserOffline(WPARAM wParam, LPARAM lParam)
{
    uid_t remoteUid = (uid_t)wParam;
    VideoCanvas canvas;
    canvas.uid = remoteUid;
    canvas.view = NULL;
    m_rtcEngine->setupRemoteVideo(canvas);
    CString strInfo;
    strInfo.Format(_T("%u offline, reason:%d"), remoteUid, lParam);
    m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

    for (int i = 0; i < m_maxVideoCount; i++){
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

LRESULT CLiveBroadcastingDlg::OnEIDAudioDeviceStateChanged(const char* deviceId, int deviceType, int deviceState)
{
	CString strInfo;
//	strInfo.Format(_T("onAudioDeviceStateChanged:"));
	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("onAudioDeviceStateChanged"));

	IAudioDeviceCollection* playbackDevices = (*m_audioDeviceManager)->enumeratePlaybackDevices();
	IAudioDeviceCollection* recordDevices = (*m_audioDeviceManager)->enumerateRecordingDevices();
	IVideoDeviceCollection* videoDevices = (*m_videoDeviceManager)->enumerateVideoDevices();
	agora::rtc::MEDIA_DEVICE_TYPE type = (agora::rtc::MEDIA_DEVICE_TYPE)deviceType;

	switch (type)
	{
	case agora::rtc::UNKNOWN_AUDIO_DEVICE:
		m_lstInfo.InsertString(m_lstInfo.GetCount(), adscUnknown);
		break;
	case agora::rtc::AUDIO_PLAYOUT_DEVICE:
	{
		for (int i = 0; i < playbackDevices->getCount(); ++i) {
			char id[MAX_DEVICE_ID_LENGTH] = { 0 };
			char name[MAX_DEVICE_ID_LENGTH] = { 0 };
			playbackDevices->getDevice(i, name, id);
			if (strcmp(deviceId, id) == 0) {
				strInfo.Format(_T("%S"), name);
				m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
			}
		}
		m_lstInfo.InsertString(m_lstInfo.GetCount(), adscPlayback);
	}
		break;
	case agora::rtc::AUDIO_RECORDING_DEVICE:
	{
		for (int i = 0; i < recordDevices->getCount(); ++i) {
			char id[MAX_DEVICE_ID_LENGTH] = { 0 };
			char name[MAX_DEVICE_ID_LENGTH] = { 0 };
			recordDevices->getDevice(i, name, id);
			if (strcmp(deviceId, id) == 0) {
				strInfo.Format(_T("%S"), name);
				m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
			}
		}
		m_lstInfo.InsertString(m_lstInfo.GetCount(), adscCapturing);
	}
		break;
	case agora::rtc::VIDEO_RENDER_DEVICE:
	{
		for (int i = 0; i < videoDevices->getCount(); ++i) {
			char id[MAX_DEVICE_ID_LENGTH] = { 0 };
			char name[MAX_DEVICE_ID_LENGTH] = { 0 };
			videoDevices->getDevice(i, name, id);
			if (strcmp(deviceId, id) == 0) {
				strInfo.Format(_T("%S"), name);
				m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
			}
		}
		m_lstInfo.InsertString(m_lstInfo.GetCount(), adscRenderer);
	}
		break;
	case agora::rtc::VIDEO_CAPTURE_DEVICE:
	{
		for (int i = 0; i < videoDevices->getCount(); ++i) {
			char id[MAX_DEVICE_ID_LENGTH] = { 0 };
			char name[MAX_DEVICE_ID_LENGTH] = { 0 };
			videoDevices->getDevice(i, name, id);
			if (strcmp(deviceId, id) == 0) {
				strInfo.Format(_T("%S"), name);
				m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
			}
		}
		m_lstInfo.InsertString(m_lstInfo.GetCount(), adscCapturer);
	}
		break;
	case agora::rtc::AUDIO_APPLICATION_PLAYOUT_DEVICE:
		m_lstInfo.InsertString(m_lstInfo.GetCount(), adscAPPPlayback);
		break;
	default:
		break;
	}

	agora::rtc::MEDIA_DEVICE_STATE_TYPE stateType = (agora::rtc::MEDIA_DEVICE_STATE_TYPE)deviceState;
	switch (stateType)
	{
	case agora::rtc::MEDIA_DEVICE_STATE_ACTIVE:
		m_lstInfo.InsertString(m_lstInfo.GetCount(), adscAcitve);
		break;
	case agora::rtc::MEDIA_DEVICE_STATE_DISABLED:
		m_lstInfo.InsertString(m_lstInfo.GetCount(), adscDisabled);
		break;
	case agora::rtc::MEDIA_DEVICE_STATE_NOT_PRESENT:
		m_lstInfo.InsertString(m_lstInfo.GetCount(), adscNoPresent);
		break;
	case agora::rtc::MEDIA_DEVICE_STATE_UNPLUGGED:
		m_lstInfo.InsertString(m_lstInfo.GetCount(), adscUnPlugined);
		break;
	case agora::rtc::MEDIA_DEVICE_STATE_UNRECOMMENDED:
		m_lstInfo.InsertString(m_lstInfo.GetCount(), adscUnRecommend);
		break;
	default:
		break;
	}

	return 0;
}

void CLiveBroadcastingDlg::OnSelchangeListInfoBroadcasting()
{
    int sel = m_lstInfo.GetCurSel();
	if (sel < 0)return;
    CString strDetail;
    m_lstInfo.GetText(sel, strDetail);
    m_staDetail.SetWindowText(strDetail);
}


void CLiveBroadcastingDlg::OnStnClickedStaticVideo()
{
}


BOOL CLiveBroadcastingDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) {
		return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}


void CLiveBroadcastingDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (!m_chkEnable.GetCheck()) {
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("enable loopback first"));
		return;
	}

	if (pScrollBar->GetSafeHwnd() == m_sldVolume.GetSafeHwnd()) {
		m_rtcEngine->adjustLoopbackRecordingSignalVolume(nPos);
	}
	
	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CLiveBroadcastingDlg::OnClickedCheckLoopback()
{
	
	if (m_chkEnable.GetCheck() == 0) {
		m_sldVolume.EnableWindow(FALSE);
		m_rtcEngine->enableLoopbackRecording(false);
	}
	else {
		m_sldVolume.EnableWindow(TRUE);
		int sel = m_cmbLoopbackDevice.GetCurSel();
		char id[MAX_DEVICE_ID_LENGTH] = { 0 };
		char name[MAX_DEVICE_ID_LENGTH] = { 0 };
		m_playbackDevices->getDevice(sel, name, id);

		m_rtcEngine->enableLoopbackRecording(true, name);
	}
}


void CLiveBroadcastingDlg::OnSelchangeComboAudienceLatency()
{
	ClientRoleOptions role_options;
	role_options.audienceLatencyLevel = (AUDIENCE_LATENCY_LEVEL_TYPE)(m_cmbLatency.GetCurSel() + 1);
    
	//set client role in the engine to the CLIENT_ROLE_BROADCASTER.
	m_rtcEngine->setClientRole((CLIENT_ROLE_TYPE)(m_cmbRole.GetCurSel()+1), role_options);
}


void CLiveBroadcastingDlg::OnStnClickedStaticDetail()
{
	// TODO: Add your control notification handler code here
}


void CLiveBroadcastingDlg::OnBnClickedButtonImage()
{
	// TODO: Add your control notification handler code here
}

void CLiveBroadcastingDlg::SetVideoSource()
{
	agora::rtc::VirtualBackgroundSource source;
	source.background_source_type = (agora::rtc::VirtualBackgroundSource::BACKGROUND_SOURCE_TYPE)m_cmbBackground.GetCurSel();

	if (m_cmbBackground.GetCurSel() == 0) {
		m_staBackColor.ShowWindow(SW_HIDE);
		m_cmbColor.ShowWindow(SW_HIDE);
		m_btnImagePath.ShowWindow(SW_HIDE);
		m_edtImagePath.ShowWindow(SW_HIDE);
	}
	else if (m_cmbBackground.GetCurSel() == 1) {
		m_staBackColor.ShowWindow(SW_HIDE);
		m_cmbColor.ShowWindow(SW_SHOW);
		m_btnImagePath.ShowWindow(SW_HIDE);
		m_edtImagePath.ShowWindow(SW_HIDE);
		if (m_cmbColor.GetCurSel() == 0)
			source.color = 0xFF0000;
		else if (m_cmbColor.GetCurSel() == 1)
			source.color = 0x00FF00;
		else if (m_cmbColor.GetCurSel() == 2)
			source.color = 0x0000FF;
	}
	else if (m_cmbBackground.GetCurSel() == 2) {
		m_staBackColor.ShowWindow(SW_HIDE);
		m_cmbColor.ShowWindow(SW_HIDE);
		m_btnImagePath.ShowWindow(SW_SHOW);
		m_edtImagePath.ShowWindow(SW_SHOW);

		LPCTSTR lpszFilter = L"BMP Files|*.bmp|JPG Files|*.jpg|PNG Files|*.ong||";
		CFileDialog dlg(TRUE, lpszFilter, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, lpszFilter, NULL);
		CString filename;
		CFile file;
		if (dlg.DoModal() == IDOK)
		{
			filename = dlg.GetPathName();
			m_edtImagePath.SetWindowText(filename);
			source.source = cs2utf8(filename).c_str();
		}
	}

	m_rtcEngine->enableVirtualBackground(true, source);
}

void CLiveBroadcastingDlg::OnBnClickedCheckEnableBackground()
{
	if (m_chkEnableBackground.GetCheck()) {
		SetVideoSource();
		m_staBackground.ShowWindow(SW_SHOW);
		m_cmbBackground.ShowWindow(SW_SHOW);
		m_btnImagePath.ShowWindow(SW_SHOW);
	}
	else {
		agora::rtc::VirtualBackgroundSource source;
		m_staBackColor.ShowWindow(SW_HIDE);
		m_staBackground.ShowWindow(SW_HIDE);
		m_cmbBackground.ShowWindow(SW_HIDE);
		m_cmbColor.ShowWindow(SW_HIDE);
		m_btnImagePath.ShowWindow(SW_HIDE);

		m_rtcEngine->enableVirtualBackground(false, source);
	}
}

void CLiveBroadcastingDlg::OnSelchangeComboBackgroundType()
{
	SetVideoSource();
}

LRESULT CLiveBroadcastingDlg::OnEIDNetworkQuality(WPARAM wParam, LPARAM lParam) {
	PNetworkQuality quality = (PNetworkQuality)wParam;
	CString strInfo = _T("===onNetworkQuality===");
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	strInfo.Format(_T("uid:%u"), quality->uid);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	strInfo.Format(_T("txQuality:%d"), quality->txQuality);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	strInfo.Format(_T("rxQuality:%u"), quality->rxQuality);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

	if (quality) {
		delete quality;
		quality = nullptr;
	}
	return 0;
}
LRESULT CLiveBroadcastingDlg::onEIDRtcStats(WPARAM wParam, LPARAM lParam) {
	RtcStats* stats = (RtcStats*)wParam;
	CString strInfo = _T("===onRtcStats===");
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	strInfo.Format(_T("duration:%u"), stats->duration);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	strInfo.Format(_T("txBytes:%u"), stats->txBytes);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	strInfo.Format(_T("rxBytes:%u"), stats->rxBytes);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

	strInfo.Format(_T("txAudioBytes:%u"), stats->txAudioBytes);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	strInfo.Format(_T("txVideoBytes:%u"), stats->txVideoBytes);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

	strInfo.Format(_T("rxAudioBytes:%u"), stats->rxAudioBytes);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	strInfo.Format(_T("rxVideoBytes:%u"), stats->rxVideoBytes);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

	strInfo.Format(_T("txKBitRate:%u"), stats->txKBitRate);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	strInfo.Format(_T("rxKBitRate:%u"), stats->rxKBitRate);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

	strInfo.Format(_T("rxAudioKBitRate:%u"), stats->rxAudioKBitRate);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	strInfo.Format(_T("txAudioKBitRate:%u"), stats->txAudioKBitRate);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

	strInfo.Format(_T("rxVideoKBitRate:%u"), stats->rxVideoKBitRate);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	strInfo.Format(_T("txVideoKBitRate:%u"), stats->txVideoKBitRate);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

	strInfo.Format(_T("lastmileDelay:%u"), stats->lastmileDelay);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	strInfo.Format(_T("userCount:%u"), stats->userCount);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

	strInfo.Format(_T("cpuAppUsage:%u"), stats->cpuAppUsage);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	strInfo.Format(_T("cpuTotalUsage:%u"), stats->cpuTotalUsage);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);


	strInfo.Format(_T("gatewayRtt:%u"), stats->gatewayRtt);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	strInfo.Format(_T("memoryAppUsageRatio:%u"), stats->memoryAppUsageRatio);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	strInfo.Format(_T("memoryTotalUsageRatio:%u"), stats->memoryTotalUsageRatio);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	strInfo.Format(_T("memoryAppUsageInKbytes:%u"), stats->memoryAppUsageInKbytes);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	

	strInfo.Format(_T("txPacketLossRate:%u"), stats->txPacketLossRate);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	strInfo.Format(_T("rxPacketLossRate:%u"), stats->rxPacketLossRate);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);


	if (stats) {
		delete stats;
		stats = nullptr;
	}
	return 0;
}

LRESULT CLiveBroadcastingDlg::onEIDLocalAudioStats(WPARAM wParam, LPARAM lParam) {
	LocalAudioStats* stats = (LocalAudioStats*)wParam;
	CString strInfo = _T("===onLocalAudioStats===");
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

	strInfo.Format(_T("numChannels:%u"), stats->numChannels);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	strInfo.Format(_T("sentSampleRate:%u"), stats->sentSampleRate);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

	strInfo.Format(_T("sentBitrate:%u"), stats->sentBitrate);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	strInfo.Format(_T("txPacketLossRate:%u"), stats->txPacketLossRate);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);


	if (stats) {
		delete stats;
		stats = nullptr;
	}
	return 0;
}

LRESULT CLiveBroadcastingDlg::onEIDLocalAudioStateChanged(WPARAM wParam, LPARAM lParam) {
	LOCAL_AUDIO_STREAM_STATE state = (LOCAL_AUDIO_STREAM_STATE)wParam;
	LOCAL_AUDIO_STREAM_ERROR error = (LOCAL_AUDIO_STREAM_ERROR)lParam;
	CString strInfo = _T("===onLocalAudioStateChanged===");
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

	strInfo.Format(_T("state:%d"), state);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	strInfo.Format(_T("error:%d"), error);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	return 0;
}
LRESULT CLiveBroadcastingDlg::onEIDRemoteAudioStats(WPARAM wParam, LPARAM lParam) {
	RemoteAudioStats* stats = (RemoteAudioStats*)wParam;
	CString strInfo = _T("===onRemoteAudioStats===");
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

	strInfo.Format(_T("uid:%u"), stats->uid);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	strInfo.Format(_T("quality:%d"), stats->quality);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

	strInfo.Format(_T("networkTransportDelay:%d"), stats->networkTransportDelay);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	strInfo.Format(_T("jitterBufferDelay:%d"), stats->jitterBufferDelay);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

	strInfo.Format(_T("audioLossRate:%d"), stats->audioLossRate);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	strInfo.Format(_T("numChannels:%d"), stats->numChannels);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	strInfo.Format(_T("receivedSampleRate:%d"), stats->receivedSampleRate);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	strInfo.Format(_T("receivedBitrate:%d"), stats->receivedBitrate);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

	strInfo.Format(_T("totalFrozenTime:%d"), stats->totalFrozenTime);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	strInfo.Format(_T("frozenRate:%d"), stats->frozenRate);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	strInfo.Format(_T("mosValue:%d"), stats->mosValue);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	strInfo.Format(_T("totalActiveTime:%d"), stats->totalActiveTime);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

	strInfo.Format(_T("publishDuration:%d"), stats->publishDuration);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	if (stats) {
		delete stats;
		stats = nullptr;
	}
	return 0;
}
LRESULT CLiveBroadcastingDlg::onEIDRemoteAudioStateChanged(WPARAM wParam, LPARAM lParam) {
	CString strInfo = _T("===onRemoteAudioStateChanged===");
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	PRemoteAudioState state = (PRemoteAudioState)wParam;

	strInfo.Format(_T("elapsed:%d"), state->elapsed);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	strInfo.Format(_T("uid:%u"), state->uid);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	strInfo.Format(_T("state:%d"), state->state);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	strInfo.Format(_T("reason:%d"), state->reason);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	return 0;
}
LRESULT CLiveBroadcastingDlg::onEIDLocalVideoStats(WPARAM wParam, LPARAM lParam) {
	LocalVideoStats* stats = (LocalVideoStats*)wParam;
	CString strInfo = _T("===onLocalVideoStats===");
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

	strInfo.Format(_T("sentBitrate:%d"), stats->sentBitrate);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

	strInfo.Format(_T("sentFrameRate:%d"), stats->sentFrameRate);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	strInfo.Format(_T("encoderOutputFrameRate:%d"), stats->encoderOutputFrameRate);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

	strInfo.Format(_T("rendererOutputFrameRate:%d"), stats->rendererOutputFrameRate);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	strInfo.Format(_T("targetBitrate:%d"), stats->targetBitrate);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

	strInfo.Format(_T("qualityAdaptIndication:%d"), stats->qualityAdaptIndication);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	strInfo.Format(_T("encodedBitrate:%d"), stats->encodedBitrate);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

	strInfo.Format(_T("encodedFrameWidth:%d"), stats->encodedFrameWidth);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	strInfo.Format(_T("encodedFrameHeight:%d"), stats->encodedFrameHeight);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

	strInfo.Format(_T("encodedFrameCount:%d"), stats->encodedFrameCount);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	strInfo.Format(_T("codecType:%d"), stats->codecType);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

	if (stats) {
		delete stats;
		stats = nullptr;
	}
	return 0;
}
LRESULT CLiveBroadcastingDlg::onEIDLocalVideoStateChanged(WPARAM wParam, LPARAM lParam) {

	LOCAL_VIDEO_STREAM_STATE state = (LOCAL_VIDEO_STREAM_STATE)wParam;
	LOCAL_VIDEO_STREAM_ERROR error = (LOCAL_VIDEO_STREAM_ERROR)lParam;
	CString strInfo = _T("===onLocalVideoStateChanged===");
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

	strInfo.Format(_T("state:%d"), state);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	strInfo.Format(_T("error:%d"), error);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	return 0;
}
LRESULT CLiveBroadcastingDlg::onEIDRemoteVideoStats(WPARAM wParam, LPARAM lParam) {
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

	strInfo.Format(_T("packetLossRate:%d"), stats->packetLossRate);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	strInfo.Format(_T("rxStreamType:%d"), stats->rxStreamType);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

	strInfo.Format(_T("totalFrozenTime:%d"), stats->totalFrozenTime);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	strInfo.Format(_T("frozenRate:%d"), stats->frozenRate);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

	strInfo.Format(_T("totalActiveTime:%d"), stats->totalActiveTime);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	strInfo.Format(_T("publishDuration:%d"), stats->publishDuration);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

	if (stats) {
		delete stats;
		stats = nullptr;
	}
	return 0;
}
LRESULT CLiveBroadcastingDlg::onEIDRemoteVideoStateChanged(WPARAM wParam, LPARAM lParam) {
	CString strInfo = _T("===onRemoteVideoStateChanged===");
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	PRemoteVideoState state = (PRemoteVideoState)wParam;

	strInfo.Format(_T("elapsed:%d"), state->elapsed);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	strInfo.Format(_T("uid:%u"), state->uid);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	strInfo.Format(_T("state:%d"), state->state);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	strInfo.Format(_T("reason:%d"), state->reason);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	return 0;
}

void CLiveBroadcastingDlg::OnBnClickedCheckReport()
{
	m_eventHandler.SetReport(m_chkReport.GetCheck() != 0);
}


void CLiveBroadcastingDlg::OnSelchangeComboColor()
{
	agora::rtc::VirtualBackgroundSource source;
	if (m_cmbColor.GetCurSel() == 0)
		source.color = 0xFF0000;
	else if (m_cmbColor.GetCurSel() == 1)
		source.color = 0x00FF00;
	else if (m_cmbColor.GetCurSel() == 2)
		source.color = 0x0000FF;
	m_rtcEngine->enableVirtualBackground(true, source);
}
