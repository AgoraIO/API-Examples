// AgoraRtmpInjectionDlg.cpp : implementation file


#include "stdafx.h"
#include "APIExample.h"
#include "AgoraRtmpInjectionDlg.h"
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
void CAgoraRtmpInjectionRtcEngineEventHandler::onJoinChannelSuccess(const char* channel, uid_t uid, int elapsed)
{
    if (m_hMsgHanlder) {
        ::PostMessage(m_hMsgHanlder, WM_MSGID(EID_JOINCHANNEL_SUCCESS), (WPARAM)uid, (LPARAM)elapsed);
    }
}
/*
    Enter the online media stream status callback.This callback indicates the state
    of the external video stream being input to the live stream.
parameters:
    url:Enter the URL address of the external video source into the live stream
    uid:user id.
    status:
    Input state of external video source:
    INJECT_STREAM_STATUS_START_SUCCESS(0):External video stream input successful
    INJECT_STREAM_STATUS_START_ALREADY_EXIST(1): External video stream already exists.
    INJECT_STREAM_STATUS_START_UNAUTHORIZED(2): The external video stream input is unauthorized
    INJECT_STREAM_STATUS_START_TIMEDOUT(3): Input external video stream timeout
    INJECT_STREAM_STATUS_START_FAILED(4) : External video stream input failed
    INJECT_STREAM_STATUS_STOP_SUCCESS(5) : INJECT_STREAM_STATUS_STOP_SUCCESS: External video stream stop input successful
    INJECT_STREAM_STATUS_STOP_NOT_FOUND (6): No external video stream to stop input
    INJECT_STREAM_STATUS_STOP_UNAUTHORIZED(7): The input to an external video stream is UNAUTHORIZED
    INJECT_STREAM_STATUS_STOP_TIMEDOUT(8) : Stopped input external video stream timeout
    INJECT_STREAM_STATUS_STOP_FAILED(9) : Failed to stop input external video stream
    INJECT_STREAM_STATUS_BROKEN(10) : Input external video stream has been broken
*/
/*void CAgoraRtmpInjectionRtcEngineEventHandler::onStreamInjectedStatus(const char* url, uid_t uid, int status)
{
    if (m_hMsgHanlder) {
        ::PostMessage(m_hMsgHanlder, WM_MSGID(EID_INJECT_STATUS), (WPARAM)uid, (LPARAM)status);
    }
}*/
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
void CAgoraRtmpInjectionRtcEngineEventHandler::onLeaveChannel(const RtcStats& stats)
{
    if (m_hMsgHanlder) {
        ::PostMessage(m_hMsgHanlder, WM_MSGID(EID_LEAVE_CHANNEL), 0, 0);
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
void CAgoraRtmpInjectionRtcEngineEventHandler::onUserJoined(uid_t uid, int elapsed)
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
void CAgoraRtmpInjectionRtcEngineEventHandler::onUserOffline(uid_t uid, USER_OFFLINE_REASON_TYPE reason)
{
    if (m_hMsgHanlder) {
        ::PostMessage(m_hMsgHanlder, WM_MSGID(EID_USER_OFFLINE), (WPARAM)uid, (LPARAM)reason);
    }
}

// CAgoraRtmpInjectionDlg dialog

IMPLEMENT_DYNAMIC(CAgoraRtmpInjectionDlg, CDialogEx)

CAgoraRtmpInjectionDlg::CAgoraRtmpInjectionDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_RTMPINJECT, pParent)
{

}

CAgoraRtmpInjectionDlg::~CAgoraRtmpInjectionDlg()
{
}

void CAgoraRtmpInjectionDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_INFO_BROADCASTING, m_lstInfo);
    DDX_Control(pDX, IDC_BUTTON_JOINCHANNEL, m_btnJoinChannel);
    DDX_Control(pDX, IDC_BUTTON_ADDSTREAM, m_btnAddStream);
    DDX_Control(pDX, IDC_EDIT_CHANNELNAME, m_edtChannelName);
    DDX_Control(pDX, IDC_EDIT_INJECT_URL, m_edtInjectUrl);
    DDX_Control(pDX, IDC_STATIC_VIDEO, m_staVideoArea);
    DDX_Control(pDX, IDC_STATIC_CHANNELNAME, m_staChannelName);
    DDX_Control(pDX, IDC_STATIC_INJECT_URL, m_staInjectUrl);
    DDX_Control(pDX, IDC_STATIC_DETAIL, m_staDetail);
}


BEGIN_MESSAGE_MAP(CAgoraRtmpInjectionDlg, CDialogEx)
    ON_BN_CLICKED(IDC_BUTTON_ADDSTREAM, &CAgoraRtmpInjectionDlg::OnBnClickedButtonAddstream)
    ON_BN_CLICKED(IDC_BUTTON_JOINCHANNEL, &CAgoraRtmpInjectionDlg::OnBnClickedButtonJoinchannel)
    ON_WM_SHOWWINDOW()
    ON_MESSAGE(WM_MSGID(EID_JOINCHANNEL_SUCCESS), &CAgoraRtmpInjectionDlg::OnEIDJoinChannelSuccess)
    ON_MESSAGE(WM_MSGID(EID_LEAVE_CHANNEL), &CAgoraRtmpInjectionDlg::OnEIDLeaveChannel)
    ON_MESSAGE(WM_MSGID(EID_INJECT_STATUS), &CAgoraRtmpInjectionDlg::OnEIDStreamInjectedStatus)
    ON_MESSAGE(WM_MSGID(EID_USER_JOINED), &CAgoraRtmpInjectionDlg::OnEIDUserJoined)
    ON_MESSAGE(WM_MSGID(EID_USER_OFFLINE), &CAgoraRtmpInjectionDlg::OnEIDUserOffline)
   
    ON_LBN_SELCHANGE(IDC_LIST_INFO_BROADCASTING, &CAgoraRtmpInjectionDlg::OnSelchangeListInfoBroadcasting)
END_MESSAGE_MAP()


// CAgoraRtmpInjectionDlg message handlers
BOOL CAgoraRtmpInjectionDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();
   
    // TODO:  Add extra initialization here
    m_localVideoWnd.Create(NULL, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CRect(0, 0, 1, 1), this, ID_BASEWND_VIDEO + 200);

    RECT rcArea;
    m_staVideoArea.GetClientRect(&rcArea);
    m_localVideoWnd.MoveWindow(&rcArea);
    m_localVideoWnd.ShowWindow(SW_SHOW);
	ResumeStatus();
    return TRUE; 
}
//set control text from config.
void CAgoraRtmpInjectionDlg::InitCtrlText()
{
    m_staInjectUrl.SetWindowText(rtmpInjectCtrlUrl);
    m_btnAddStream.SetWindowText(rtmpInjectCtrlInject);
    m_staChannelName.SetWindowText(commonCtrlChannel);
    m_btnJoinChannel.SetWindowText(commonCtrlJoinChannel);
}
//Initialize the Agora SDK
bool CAgoraRtmpInjectionDlg::InitAgora()
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

    m_btnJoinChannel.EnableWindow(TRUE);
    return true;
}
//UnInitialize the Agora SDK
void CAgoraRtmpInjectionDlg::UnInitAgora()
{
    if (m_rtcEngine) {
		if(m_joinChannel)
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
void CAgoraRtmpInjectionDlg::RenderLocalVideo()
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
// resume window status.
void CAgoraRtmpInjectionDlg::ResumeStatus()
{
	InitCtrlText();
	m_lstInfo.ResetContent();
	m_joinChannel = false;
	m_initialize = false;
	m_addInjectStream = false;
	m_injectUrl="";
	m_btnAddStream.EnableWindow(FALSE);
	m_edtInjectUrl.EnableWindow(FALSE);
	m_edtInjectUrl.SetWindowText(_T(""));
	m_edtChannelName.SetWindowText(_T(""));
	m_staDetail.SetWindowText(_T(""));
}

//bShow is true when the window is displayed
void CAgoraRtmpInjectionDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
    CDialogEx::OnShowWindow(bShow, nStatus);
    if (bShow) {
        RenderLocalVideo();
	}
	else {
		ResumeStatus();
	}
}

//add or remove stream in the engine.
void CAgoraRtmpInjectionDlg::OnBnClickedButtonAddstream()
{
    if (!m_rtcEngine || !m_initialize)
        return;

    if (m_addInjectStream) {
        m_addInjectStream = false;
        m_edtInjectUrl.EnableWindow(TRUE);
        m_btnAddStream.SetWindowText(_T("Inject URL"));
        //remove inject stream in the engine.
        int ret = m_rtcEngine->stopRtmpStream(m_injectUrl.c_str());
    }
    else {
        CString strURL;
        m_edtInjectUrl.GetWindowText(strURL);
        if (strURL.IsEmpty()) {
            AfxMessageBox(_T("Fill INJECT URL first"));
            return;
        }

        std::string szURL = cs2utf8(strURL);
        LiveTranscoding config;
        //add Inject stream url in the engine.
        m_rtcEngine->startRtmpStreamWithTranscoding(szURL.c_str(), config);
        m_injectUrl = szURL;
        m_addInjectStream = true;
        m_edtInjectUrl.EnableWindow(FALSE);
        m_btnAddStream.SetWindowText(_T("Remove URL"));
    }
    m_btnAddStream.EnableWindow(FALSE);
    m_edtInjectUrl.EnableWindow(FALSE);
}

//join or leave channel
void CAgoraRtmpInjectionDlg::OnBnClickedButtonJoinchannel()
{
    if (!m_rtcEngine || !m_initialize)
        return;

    if (!m_joinChannel) {
        CString strChannelName;
        m_edtChannelName.GetWindowText(strChannelName);
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
			CString strInfo;
			strInfo.Format(_T("join channel %s, use ChannelMediaOptions"), getCurrentTime());
			m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
            m_btnJoinChannel.EnableWindow(FALSE);
        }
    }
    else {
        //leave channel in the engine.
        if (0 == m_rtcEngine->leaveChannel()) {
            m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("leave channel"));
            m_btnJoinChannel.EnableWindow(FALSE);
        }
    }
}
//EID_JOINCHANNEL_SUCCESS message window handler.
LRESULT CAgoraRtmpInjectionDlg::OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam)
{
    m_btnJoinChannel.EnableWindow(TRUE);
	m_joinChannel = true;
    m_btnJoinChannel.SetWindowText(commonCtrlLeaveChannel);
    m_btnAddStream.EnableWindow(TRUE);
    m_edtInjectUrl.EnableWindow(TRUE);
    CString strInfo;
    strInfo.Format(_T("%s:join success, uid=%u"), getCurrentTime(), wParam);
    m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

    ::PostMessage(GetParent()->GetSafeHwnd(), WM_MSGID(EID_JOINCHANNEL_SUCCESS), TRUE, 0);
    return 0;
}
//EID_LEAVE_CHANNEL message window handler.
LRESULT CAgoraRtmpInjectionDlg::OnEIDLeaveChannel(WPARAM wParam, LPARAM lParam)
{
    m_btnJoinChannel.EnableWindow(TRUE);
	m_joinChannel = false;
	m_btnJoinChannel.SetWindowText(commonCtrlJoinChannel);

    CString strInfo;
    strInfo.Format(_T("leave channel success"));
    m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
    ::PostMessage(GetParent()->GetSafeHwnd(), WM_MSGID(EID_JOINCHANNEL_SUCCESS), FALSE, 0);
    return 0;
}
//EID_USER_JOINED message window handler.
LRESULT CAgoraRtmpInjectionDlg::OnEIDUserJoined(WPARAM wParam, LPARAM lParam)
{
    uid_t remoteUid = (uid_t)wParam;
    if (remoteUid == 666) {//inject stream
        CString strInfo;
        strInfo.Format(_T("%u joined, 666 is inject stream"), remoteUid);
        m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
        //mute audio stream and video stream in the engine.
        m_rtcEngine->muteRemoteAudioStream(666, true);
        m_rtcEngine->muteRemoteVideoStream(666, true);
    }
   
    return 0;
}
//EID_USER_OFFLINE message window handler.
LRESULT CAgoraRtmpInjectionDlg::OnEIDUserOffline(WPARAM wParam, LPARAM lParam)
{
    uid_t remoteUid = (uid_t)wParam;
    if (remoteUid == 666) {//inject stream
        VideoCanvas canvas;
        canvas.uid = remoteUid;
        canvas.view = NULL;
        //setup remote video in the engine to canvas.
        m_rtcEngine->setupRemoteVideo(canvas);
        CString strInfo;
        strInfo.Format(_T("%u offline, reason:%d"), remoteUid, lParam);
        m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
    }
    return 0;
}


//EID_INJECT_STATUS message window handler.
LRESULT CAgoraRtmpInjectionDlg::OnEIDStreamInjectedStatus(WPARAM wParam, LPARAM lParam)
{
    CString strInfo;
    switch ((INJECT_STREAM_STATUS)lParam)
    {
    case INJECT_STREAM_STATUS_START_SUCCESS:
        strInfo.Format(_T("%s, err: %d。"), agoraInjectStartSucc, 0);
        break;
    case INJECT_STREAM_STATUS_START_ALREADY_EXISTS:
        strInfo.Format(_T("%s, err: %d。"), agoraInjectExist, 1);
        break;
    case INJECT_STREAM_STATUS_START_UNAUTHORIZED:
        strInfo.Format(_T("%s, err: %d。"), agoraInjectStartUnAuth, 2);
        break;
    case INJECT_STREAM_STATUS_START_TIMEDOUT:
        strInfo.Format(_T("%s, err: %d。"), agoraInjectStartTimeout, 3);
        break;

    case INJECT_STREAM_STATUS_START_FAILED:
        strInfo.Format(_T("%s, err: %d。"), agoraInjectStartFailed, 4);
        break;
    case INJECT_STREAM_STATUS_STOP_SUCCESS:
        strInfo.Format(_T("%s, err: %d。"), agoraInjectStopSuccess, 5);
        break;
    case INJECT_STREAM_STATUS_STOP_NOT_FOUND:
        strInfo.Format(_T("%s, err: %d。"), agoraInjectNotFound, 6);
        break;
    case INJECT_STREAM_STATUS_STOP_UNAUTHORIZED:
        strInfo.Format(_T("%s, err: %d。"), agoraInjectStopUnAuth, 7);
        break;

    case INJECT_STREAM_STATUS_STOP_TIMEDOUT:
        strInfo.Format(_T("%s, err: %d。"), agoraInjectStopTimeout, 8);
        break;
    case INJECT_STREAM_STATUS_STOP_FAILED:
        strInfo.Format(_T("%s, err: %d。"), agoraInjectStopFailed, 9);
        break;
    case INJECT_STREAM_STATUS_BROKEN:
        strInfo.Format(_T("%s, err: %d。"), agoraInjectBroken, 10);
        break;
    default:
        break;
    }

    m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
    m_btnAddStream.EnableWindow(TRUE);
    m_edtInjectUrl.EnableWindow(TRUE);
    return 0;
}


//select list information
void CAgoraRtmpInjectionDlg::OnSelchangeListInfoBroadcasting()
{
    int sel = m_lstInfo.GetCurSel();
	if (sel < 0)return;
    CString strDetail;
    m_lstInfo.GetText(sel, strDetail);
    m_staDetail.SetWindowText(strDetail);
}


BOOL CAgoraRtmpInjectionDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) {
		return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}
