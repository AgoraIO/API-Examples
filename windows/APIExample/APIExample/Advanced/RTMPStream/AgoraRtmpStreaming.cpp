// AgoraRtmpStreaming.cpp : implementation file


#include "../../stdafx.h"
#include "../../APIExample.h"
#include "AgoraRtmpStreaming.h"
#include "afxdialogex.h"
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
void CAgoraRtmpStreamingDlgRtcEngineEventHandler::onJoinChannelSuccess(const char* channel, uid_t uid, int elapsed)
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
void CAgoraRtmpStreamingDlgRtcEngineEventHandler::onStreamInjectedStatus(const char* url, uid_t uid, int status)
{
    if (m_hMsgHanlder) {
        ::PostMessage(m_hMsgHanlder, WM_MSGID(EID_INJECT_STATUS), (WPARAM)uid, (LPARAM)status);
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
void CAgoraRtmpStreamingDlgRtcEngineEventHandler::onLeaveChannel(const RtcStats& stats)
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
void CAgoraRtmpStreamingDlgRtcEngineEventHandler::onUserJoined(uid_t uid, int elapsed)
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
void CAgoraRtmpStreamingDlgRtcEngineEventHandler::onUserOffline(uid_t uid, USER_OFFLINE_REASON_TYPE reason)
{
    if (m_hMsgHanlder) {
        ::PostMessage(m_hMsgHanlder, WM_MSGID(EID_USER_OFFLINE), (WPARAM)uid, (LPARAM)reason);
    }
}
/**
    Occurs when the state of the RTMP streaming changes.
    The SDK triggers this callback to report the result of the local user calling the \ref agora::rtc::IRtcEngine::addPublishStreamUrl "addPublishStreamUrl" or \ref agora::rtc::IRtcEngine::removePublishStreamUrl "removePublishStreamUrl" method.
    This callback indicates the state of the RTMP streaming. When exceptions occur, you can troubleshoot issues by referring to the detailed error descriptions in the *errCode* parameter.
    @param url The RTMP URL address.
    @param state The RTMP streaming state. See: #RTMP_STREAM_PUBLISH_STATE.
    @param errCode The detailed error information for streaming. See: #RTMP_STREAM_PUBLISH_ERROR.
 */
void CAgoraRtmpStreamingDlgRtcEngineEventHandler::onRtmpStreamingStateChanged(const char *url, RTMP_STREAM_PUBLISH_STATE state, RTMP_STREAM_PUBLISH_ERROR errCode)
{
    if (m_hMsgHanlder) {
        PRtmpStreamStreamStateChanged rtmpState = new RtmpStreamStreamStateChanged;
        int len = strlen(url);
        rtmpState->url = new char[len + 1];
        rtmpState->url[len] = 0;
        strcpy_s(rtmpState->url, len + 1, url);
        rtmpState->state = state;
        rtmpState->error = errCode;
        ::PostMessage(m_hMsgHanlder, WM_MSGID(EID_RTMP_STREAM_STATE_CHANGED), (WPARAM)rtmpState, 0);
    }
}


IMPLEMENT_DYNAMIC(CAgoraRtmpStreamingDlg, CDialogEx)

CAgoraRtmpStreamingDlg::CAgoraRtmpStreamingDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_RTMP_STREAMING, pParent)
{

}

CAgoraRtmpStreamingDlg::~CAgoraRtmpStreamingDlg()
{
}

void CAgoraRtmpStreamingDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_INFO_BROADCASTING, m_lstInfo);
    DDX_Control(pDX, IDC_BUTTON_JOINCHANNEL, m_btnJoinChannel);
    DDX_Control(pDX, IDC_STATIC_VIDEO, m_staVideoArea);
    DDX_Control(pDX, IDC_EDIT_CHANNELNAME, m_edtChannelName);
    DDX_Control(pDX, IDC_EDIT_RTMP_URL, m_edtRtmpUrl);
    DDX_Control(pDX, IDC_BUTTON_ADDSTREAM, m_btnAddStream);
    DDX_Control(pDX, IDC_BUTTON_REMOVE_STREAM, m_btnRemoveStream);
    DDX_Control(pDX, IDC_COMBO_RTMPURLS, m_cmbRtmpUrl);
    DDX_Control(pDX, IDC_STATIC_CHANNELNAME, m_staChannelName);
    DDX_Control(pDX, IDC_STATIC_RTMP_URL, m_staPublishUrl);
    DDX_Control(pDX, IDC_BUTTON_REMOVE_ALLSTREAM, m_btnRemoveAll);
    DDX_Control(pDX, IDC_STATIC_RTMP_URLS, m_staRemoveUrl);
    DDX_Control(pDX, IDC_STATIC_DETAIL, m_staDetail);
}


BEGIN_MESSAGE_MAP(CAgoraRtmpStreamingDlg, CDialogEx)
    ON_WM_SHOWWINDOW()
    ON_MESSAGE(WM_MSGID(EID_JOINCHANNEL_SUCCESS), &CAgoraRtmpStreamingDlg::OnEIDJoinChannelSuccess)
    ON_MESSAGE(WM_MSGID(EID_LEAVE_CHANNEL), &CAgoraRtmpStreamingDlg::OnEIDLeaveChannel)
    ON_MESSAGE(WM_MSGID(EID_RTMP_STREAM_STATE_CHANGED), &CAgoraRtmpStreamingDlg::OnEIDRtmpStateChanged)
    ON_BN_CLICKED(IDC_BUTTON_JOINCHANNEL, &CAgoraRtmpStreamingDlg::OnBnClickedButtonJoinchannel)
    ON_BN_CLICKED(IDC_BUTTON_ADDSTREAM, &CAgoraRtmpStreamingDlg::OnBnClickedButtonAddstream)
    ON_BN_CLICKED(IDC_BUTTON_REMOVE_STREAM, &CAgoraRtmpStreamingDlg::OnBnClickedButtonRemoveStream)
    ON_BN_CLICKED(IDC_BUTTON_REMOVE_ALLSTREAM, &CAgoraRtmpStreamingDlg::OnBnClickedButtonRemoveAllstream)
    ON_LBN_SELCHANGE(IDC_LIST_INFO_BROADCASTING, &CAgoraRtmpStreamingDlg::OnSelchangeListInfoBroadcasting)
END_MESSAGE_MAP()


//Initialize the Agora SDK
bool CAgoraRtmpStreamingDlg::InitAgora()
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
    //set channel profile in the engine to the CHANNEL_PROFILE_LIVE_BROADCASTING.
    m_rtcEngine->setChannelProfile(CHANNEL_PROFILE_LIVE_BROADCASTING);
    m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("live broadcasting"));
    //set clinet role in the engine to the CLIENT_ROLE_BROADCASTER.
    m_rtcEngine->setClientRole(CLIENT_ROLE_BROADCASTER);
    m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("setClientRole broadcaster"));

    m_btnJoinChannel.EnableWindow(TRUE);
    return true;
}
//UnInitialize the Agora SDK
void CAgoraRtmpStreamingDlg::UnInitAgora()
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
//render local video from SDK local capture.
void CAgoraRtmpStreamingDlg::RenderLocalVideo()
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
BOOL CAgoraRtmpStreamingDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    InitCtrlText();
    m_localVideoWnd.Create(NULL, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CRect(0, 0, 1, 1), this, ID_BASEWND_VIDEO + 100);

    RECT rcArea;
    m_staVideoArea.GetClientRect(&rcArea);
    m_localVideoWnd.MoveWindow(&rcArea);
    m_localVideoWnd.ShowWindow(SW_SHOW);
    m_btnAddStream.EnableWindow(FALSE);
    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}
//set control text from config.
void CAgoraRtmpStreamingDlg::InitCtrlText()
{
    m_staRemoveUrl.SetWindowText(rtmpStreamingCtrlPublishUrl);
    m_staPublishUrl.SetWindowText(rtmpStreamingCtrlPublishUrl);
    m_btnAddStream.SetWindowText(rtmpStreamingCtrlAdd);
    m_staChannelName.SetWindowText(commonCtrlChannel);
    m_btnJoinChannel.SetWindowText(commonCtrlJoinChannel);
    m_btnRemoveStream.SetWindowText(rtmpStreamingCtrlRemove);
    m_btnRemoveAll.SetWindowText(rtmpStreamingCtrlRemoveAll);
}

void CAgoraRtmpStreamingDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
    CDialogEx::OnShowWindow(bShow, nStatus);

    if (bShow) {
        RenderLocalVideo();
    }
}

//remove all rtmp stream in the engine.
void CAgoraRtmpStreamingDlg::RemoveAllRtmpUrls()
{
    m_bRemoveAll = true;
    CString strUrl;
    //remove all publish stream in the engine.
    for (int i = 0; i < m_cmbRtmpUrl.GetCount(); ++i) {
        m_cmbRtmpUrl.GetLBText(i, strUrl);
        std::string szUrl = cs2utf8(strUrl);
        m_rtcEngine->removePublishStreamUrl(szUrl.c_str());
    }
    m_cmbRtmpUrl.Clear();
    m_cmbRtmpUrl.ResetContent();
}

//join or leave channel button handler.
void CAgoraRtmpStreamingDlg::OnBnClickedButtonJoinchannel()
{
    if (!m_rtcEngine || !m_initialize)
        return;

    if (!joinChannel) {
        CString strChannelName;
        m_edtChannelName.GetWindowText(strChannelName);
        if (strChannelName.IsEmpty()) {
            AfxMessageBox(_T("Fill channel name first"));
            return;
        }

        std::string szChannelId = cs2utf8(strChannelName);
        //join channel in the engine.
        if (0 == m_rtcEngine->joinChannel(APP_TOKEN, szChannelId.c_str(), "", 0)) {
            m_btnJoinChannel.EnableWindow(FALSE);
        }
    }
    else {
        if (m_cmbRtmpUrl.GetCount() > 0) {
            m_btnAddStream.EnableWindow(FALSE);
            m_btnRemoveStream.EnableWindow(FALSE);
            m_btnJoinChannel.EnableWindow(FALSE);
            //remove all rtmp streams.
            RemoveAllRtmpUrls();
        }
        //leave channel in the engine.
        else  if (0 == m_rtcEngine->leaveChannel()) {
            m_btnJoinChannel.EnableWindow(FALSE);
        }
    }
}

// add stream button handler.
void CAgoraRtmpStreamingDlg::OnBnClickedButtonAddstream()
{
    if (!m_rtcEngine || !m_initialize)
        return;

    CString strURL;
    m_edtRtmpUrl.GetWindowText(strURL);
    if (strURL.IsEmpty()) {
        AfxMessageBox(_T("Fill Publish URL first"));
        return;
    }
    if (m_urlSet.find(strURL) != m_urlSet.end()) {
        AfxMessageBox(_T("have add Publish Stream URL"));
        return;
    }
    std::string szURL = cs2utf8(strURL);
    // add publish stream in the engine.
    int ret = m_rtcEngine->addPublishStreamUrl(szURL.c_str(), false);// no transcoding

    if (ret != 0) {
        CString strInfo;
        strInfo.Format(_T("addPublishStreamUrl failed:%d"), ret);
        m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
        return;
    }
}

//remove stream button handler.
void CAgoraRtmpStreamingDlg::OnBnClickedButtonRemoveStream()
{
    if (!m_rtcEngine || !m_initialize)
        return;

    if (m_cmbRtmpUrl.GetCount() == 0)
        return;

    CString strUrl;
    m_cmbRtmpUrl.GetWindowText(strUrl);
    std::string szUrl = cs2utf8(strUrl);
    //remove publish stream in the engine.
    m_rtcEngine->removePublishStreamUrl(szUrl.c_str());
}

//remove all streams in the engine.
void CAgoraRtmpStreamingDlg::OnBnClickedButtonRemoveAllstream()
{
    if (m_cmbRtmpUrl.GetCount() == 0 && m_cmbRtmpUrl.GetCurSel() >= 0) {
        return;
    }
    if (!m_rtcEngine || !m_initialize)
        return;

    CString strUrl;
    m_cmbRtmpUrl.GetWindowText(strUrl);

    std::string szUrl = cs2utf8(strUrl);
    //remove public stream in the engine.
    m_rtcEngine->removePublishStreamUrl(szUrl.c_str());
    m_btnRemoveStream.EnableWindow(FALSE);
}

//EID_JOINCHANNEL_SUCCESS message window handler.
LRESULT CAgoraRtmpStreamingDlg::OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam)
{
    m_btnJoinChannel.EnableWindow(TRUE);
    m_btnAddStream.EnableWindow(TRUE);
    m_btnRemoveStream.EnableWindow(TRUE);
    joinChannel = true;
    m_btnJoinChannel.SetWindowText(_T("LeaveChannel"));
    CString strInfo;
    strInfo.Format(_T("%s:join success, uid=%u"), getCurrentTime(), wParam);
    m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

    m_btnAddStream.EnableWindow(TRUE);
    ::PostMessage(GetParent()->GetSafeHwnd(), WM_MSGID(EID_JOINCHANNEL_SUCCESS), TRUE, 0);
    return 0;
}

//EID_LEAVE_CHANNEL message window handler.
LRESULT CAgoraRtmpStreamingDlg::OnEIDLeaveChannel(WPARAM wParam, LPARAM lParam)
{
    m_btnJoinChannel.EnableWindow(TRUE);
    joinChannel = false;
    m_btnJoinChannel.SetWindowText(_T("JoinChannel"));
    CString strInfo;
    strInfo.Format(_T("leave channel success"));
    m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
    m_btnRemoveStream.EnableWindow(FALSE);
    m_btnAddStream.EnableWindow(TRUE);
    ::PostMessage(GetParent()->GetSafeHwnd(), WM_MSGID(EID_JOINCHANNEL_SUCCESS), FALSE, 0);
    return 0;
}
//EID_INJECT_STATUS message window handler.
LRESULT CAgoraRtmpStreamingDlg::OnEIDRtmpStateChanged(WPARAM wParam, LPARAM lParam)
{
    PRtmpStreamStreamStateChanged rtmpState = (PRtmpStreamStreamStateChanged)wParam;
    CString strInfo;
    m_btnRemoveStream.EnableWindow(TRUE);
    switch (rtmpState->state)
    {
    case RTMP_STREAM_PUBLISH_STATE_IDLE:
    {
        strInfo.Format(_T("%s:%S��"), agoraRtmpStateIdle, rtmpState->url);
        CString strUrl;
        strUrl.Format(_T("%S"), rtmpState->url);
        int sel = m_cmbRtmpUrl.GetCurSel();
        m_cmbRtmpUrl.DeleteString(sel);
        m_cmbRtmpUrl.ResetContent();
        if (m_cmbRtmpUrl.GetCount() > 0) {
            m_cmbRtmpUrl.SetCurSel(0);
        }
        for (auto iter = m_urlSet.begin(); iter != m_urlSet.end(); ++iter)
            if (strUrl.Compare(*iter) == 0) {
                m_urlSet.erase(iter);
                break;
            }
        if (m_bRemoveAll) {
            m_removeUrlCount++;
            if (m_removeUrlCount == m_urlSet.size()) {//remove all url when leave channel
                m_urlSet.clear();
                m_bRemoveAll = false;
                m_rtcEngine->leaveChannel();
                m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("leaveChannel"));
            }
        }

    }
    break;
    case RTMP_STREAM_PUBLISH_STATE_CONNECTING:
    {
        strInfo = agoraRtmpStateConnecting;
    }
    break;
    case RTMP_STREAM_PUBLISH_STATE_RUNNING:
        strInfo = agoraRtmpStateRunning;
        if (rtmpState->error == RTMP_STREAM_PUBLISH_ERROR_OK) {
            strInfo = agoraRtmpStateRunningSuccess;
            CString  strUrl;
            strUrl.Format(_T("%S"), rtmpState->url);
            if (m_urlSet.find(strUrl) == m_urlSet.end()) {
                m_cmbRtmpUrl.AddString(strUrl);
                m_urlSet.insert(strUrl);
                if (m_cmbRtmpUrl.GetCurSel() < 0)
                    m_cmbRtmpUrl.SetCurSel(0);
            }

        }
        break;
    case RTMP_STREAM_PUBLISH_STATE_RECOVERING:
        strInfo.Format(agoraRtmpStateRecovering);
        break;
    case RTMP_STREAM_PUBLISH_STATE_FAILURE:
    {
        switch (rtmpState->state)
        {
        case RTMP_STREAM_PUBLISH_ERROR_INVALID_ARGUMENT:
        {
            strInfo = agoraRtmpStateInvalidArg;
        }
        break;
        case RTMP_STREAM_PUBLISH_ERROR_ENCRYPTED_STREAM_NOT_ALLOWED:
        {
            strInfo = agoraRtmpStateEncrypted;
        }
        break;
        case RTMP_STREAM_PUBLISH_ERROR_CONNECTION_TIMEOUT:
        {
            strInfo = agoraRtmpStateConnTimeout;
        }
        break;
        case RTMP_STREAM_PUBLISH_ERROR_INTERNAL_SERVER_ERROR:
        {
            strInfo = agoraRtmpStateInrealErr;
        }
        break;
        case RTMP_STREAM_PUBLISH_ERROR_RTMP_SERVER_ERROR:
        {
            strInfo = agoraRtmpStateServerErr;
        }
        break;
        case RTMP_STREAM_PUBLISH_ERROR_TOO_OFTEN:
        {
            strInfo = agoraRtmpStateTooOften;
        }
        break;
        case RTMP_STREAM_PUBLISH_ERROR_REACH_LIMIT:
        {
            strInfo = agoraRtmpStateReachLimit;
        }
        break;
        case RTMP_STREAM_PUBLISH_ERROR_NOT_AUTHORIZED:
        {
            strInfo = agoraRtmpStateNotAuth;
        }
        break;
        case RTMP_STREAM_PUBLISH_ERROR_STREAM_NOT_FOUND:
        {
            strInfo = agoraRtmpStateNotFound;
        }
        break;
        case RTMP_STREAM_PUBLISH_ERROR_FORMAT_NOT_SUPPORTED:
        {
            strInfo = agoraRtmpStateNotSupported;
        }
        break;
        default:
            break;
        }
    }
    break;
    default:
        break;
    }
    m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
    delete[] rtmpState->url;
    rtmpState->url = NULL;
    delete[] rtmpState;
    rtmpState = NULL;
    return 0;
}

//show list information to m_staDetail.
void CAgoraRtmpStreamingDlg::OnSelchangeListInfoBroadcasting()
{
    int sel = m_lstInfo.GetCurSel();
    CString strDetail;
    m_lstInfo.GetText(sel, strDetail);
    m_staDetail.SetWindowText(strDetail);
}

