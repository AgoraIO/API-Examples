// CAgoraMetaDataDlg.cpp : implementation file


#include "../../stdafx.h"
#include "../../APIExample.h"
#include "CAgoraMetaDataDlg.h"
#include "afxdialogex.h"
//metadata

//set max meta data size.
void CAgoraMetaDataObserver::SetMaxMetadataSize(int maxSize)
{
    if (maxSize > 1024)
        maxSize = 1024;
    m_maxSize = maxSize;
}
/*
    get max meta data size of byte.
*/
int CAgoraMetaDataObserver::getMaxMetadataSize()
{
    return m_maxSize;
}
/*
    The sender is ready to send MetadataThis callback method
    is triggered when the SDK is ready to receive and send Metadata.
    annotations:
    Make sure that the Metadata size passed in to this method does not
    exceed the value set in getMaxMetadataSize.
    parameter:
    metadata :Metadata that the user wants to send.
    return:
    True: send
    False: don't send
*/
bool CAgoraMetaDataObserver::onReadyToSendMetadata(Metadata &metadata)
{
    if (m_sendSEI.length() > 0) {
        memcpy_s(metadata.buffer, m_sendSEI.length(), m_sendSEI.c_str(), m_sendSEI.length());

    }
    metadata.size = m_sendSEI.length();
    return true;
}
/*
    The receiver has received Metadata.The SDK triggers the callback when it
    receives Metadata sent by the remote user.
    parameter:
    metadata:Received Metadata.
*/
void CAgoraMetaDataObserver::onMetadataReceived(const Metadata &metadata)
{
    if (m_hMsgHanlder) {
        Metadata* recvMetaData = new Metadata;
        recvMetaData->size = metadata.size;
        recvMetaData->uid = metadata.uid;
        recvMetaData->timeStampMs = metadata.timeStampMs;
        if (metadata.size > 0) {
            recvMetaData->buffer = new unsigned char[metadata.size];
            memcpy_s(recvMetaData->buffer, metadata.size, metadata.buffer, metadata.size);
            recvMetaData->buffer[metadata.size] = 0;
        }
        ::PostMessage(m_hMsgHanlder, WM_MSGID(RECV_METADATA_MSG), (WPARAM)recvMetaData, 0);
    }
}
//set send string.
void CAgoraMetaDataObserver::SetSendSEI(std::string utf8Msg)
{
    m_sendSEI = utf8Msg;
   
}
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
void CAgoraMetaDataEventHanlder::onJoinChannelSuccess(const char* channel, uid_t uid, int elapsed)
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
    by the callback��ms).
*/
void CAgoraMetaDataEventHanlder::onUserJoined(uid_t uid, int elapsed) {
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
void CAgoraMetaDataEventHanlder::onUserOffline(uid_t uid, USER_OFFLINE_REASON_TYPE reason)
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
void CAgoraMetaDataEventHanlder::onLeaveChannel(const RtcStats& stats)
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
void CAgoraMetaDataEventHanlder::onRemoteVideoStateChanged(uid_t uid, REMOTE_VIDEO_STATE state, REMOTE_VIDEO_STATE_REASON reason, int elapsed)
{
    if (m_hMsgHanlder) {
        PVideoStateStateChanged stateChanged = new VideoStateStateChanged;
        stateChanged->uid    = uid;
        stateChanged->reason = reason;
        stateChanged->state  = state;
        ::PostMessage(m_hMsgHanlder, WM_MSGID(EID_REMOTE_VIDEO_STATE_CHANED), (WPARAM)stateChanged, 0);
    }
}

// CAgoraMetaDataDlg dialog

IMPLEMENT_DYNAMIC(CAgoraMetaDataDlg, CDialogEx)

CAgoraMetaDataDlg::CAgoraMetaDataDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_METADATA, pParent)
{

}

CAgoraMetaDataDlg::~CAgoraMetaDataDlg()
{
}

void CAgoraMetaDataDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_STATIC_CHANNELNAME, m_staChannelName);
    DDX_Control(pDX, IDC_BUTTON_JOINCHANNEL, m_btnJoinChannel);
    DDX_Control(pDX, IDC_STATIC_SENDSEI, m_staSendSEI);
    DDX_Control(pDX, IDC_EDIT_SEI, m_edtSendSEI);
    DDX_Control(pDX, IDC_EDIT_RECV, m_edtRecvSEI);
    DDX_Control(pDX, IDC_STATIC_METADATA_INFO, m_staMetaData);
    DDX_Control(pDX, IDC_LIST_INFO_BROADCASTING, m_lstInfo);
    DDX_Control(pDX, IDC_STATIC_VIDEO, m_staVideoArea);
    DDX_Control(pDX, IDC_EDIT_CHANNELNAME, m_edtChannelName);
    DDX_Control(pDX, IDC_BUTTON_SEND, m_btnSendSEI);
}


BEGIN_MESSAGE_MAP(CAgoraMetaDataDlg, CDialogEx)
    ON_BN_CLICKED(IDC_BUTTON_JOINCHANNEL, &CAgoraMetaDataDlg::OnBnClickedButtonJoinchannel)
    ON_MESSAGE(WM_MSGID(EID_JOINCHANNEL_SUCCESS), &CAgoraMetaDataDlg::OnEIDJoinChannelSuccess)
    ON_MESSAGE(WM_MSGID(EID_LEAVE_CHANNEL), &CAgoraMetaDataDlg::OnEIDLeaveChannel)
    ON_MESSAGE(WM_MSGID(EID_USER_JOINED), &CAgoraMetaDataDlg::OnEIDUserJoined)
    ON_MESSAGE(WM_MSGID(EID_USER_OFFLINE), &CAgoraMetaDataDlg::OnEIDUserOffline)
    ON_MESSAGE(WM_MSGID(EID_REMOTE_VIDEO_STATE_CHANED), &CAgoraMetaDataDlg::OnEIDRemoteVideoStateChanged)
    ON_MESSAGE(WM_MSGID(RECV_METADATA_MSG), &CAgoraMetaDataDlg::OnEIDMetadataReceived)
    ON_WM_SHOWWINDOW()
    ON_BN_CLICKED(IDC_BUTTON_SEND, &CAgoraMetaDataDlg::OnBnClickedButtonSend)
    ON_BN_CLICKED(IDC_BUTTON_CLEAR, &CAgoraMetaDataDlg::OnBnClickedButtonClear)
END_MESSAGE_MAP()


// CAgoraMetaDataDlg message handlers


void CAgoraMetaDataDlg::OnBnClickedButtonJoinchannel()
{
    if (!m_rtcEngine || !m_initialize)
        return;
    CString strInfo;
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


BOOL CAgoraMetaDataDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    InitCtrlText();
    m_localVideoWnd.Create(NULL, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CRect(0, 0, 1, 1), this, ID_BASEWND_VIDEO + 100);
    m_remoteVideoWnd.Create(NULL, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CRect(0, 0, 1, 1), this, ID_BASEWND_VIDEO + 100);

    RECT rcArea;
    m_staVideoArea.GetClientRect(&rcArea);

    RECT rcLeft = rcArea, rcRight = rcArea;
    rcLeft.right = rcLeft.left + (rcArea.right - rcArea.left) / 2;
    rcRight.left = rcLeft.right + 1;
    m_localVideoWnd.MoveWindow(&rcLeft);
    m_remoteVideoWnd.MoveWindow(&rcLeft);
    m_localVideoWnd.ShowWindow(SW_SHOW);
    m_remoteVideoWnd.ShowWindow(SW_SHOW);

    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}

//set control text from config.
void CAgoraMetaDataDlg::InitCtrlText()
{
    m_staMetaData.SetWindowText(videoSEIInformation);
    m_staSendSEI.SetWindowText(metadataCtrlSendSEI);
    m_btnSendSEI.SetWindowText(metadataCtrlBtnSend);
    m_staChannelName.SetWindowText(commonCtrlChannel);
    m_btnJoinChannel.SetWindowText(commonCtrlJoinChannel);
}
//Initialize the Agora SDK
bool CAgoraMetaDataDlg::InitAgora()
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
    //set meta data observer notify window.
    m_metaDataObserver.SetMsgReceiver(m_hWnd);
    //register media meta data observer.
    m_rtcEngine->registerMediaMetadataObserver(&m_metaDataObserver, IMetadataObserver::VIDEO_METADATA);

    m_btnJoinChannel.EnableWindow(TRUE);
    return true;
}

//UnInitialize the Agora SDK
void CAgoraMetaDataDlg::UnInitAgora()
{
    if (m_rtcEngine) {
        //stop preview in the window.
        m_rtcEngine->stopPreview();
        m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("stopPreview"));
        //disalbe video in the engine.
        m_rtcEngine->disableVideo();
        m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("disableVideo"));
        //release engine.
        m_rtcEngine->release(true);
        m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("release rtc engine"));
        m_rtcEngine = NULL;
    }
}

//EID_JOINCHANNEL_SUCCESS mesage window handler.
LRESULT CAgoraMetaDataDlg::OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam)
{
    m_btnJoinChannel.EnableWindow(TRUE);
    joinChannel = true;
    m_btnJoinChannel.SetWindowText(_T("LeaveChannel"));

    CString strInfo;
    strInfo.Format(_T("%s:join success, uid=%u"), getCurrentTime(), wParam);
    m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

    m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

    m_localVideoWnd.SetUID(wParam);
   
    //notify parent window
    ::PostMessage(GetParent()->GetSafeHwnd(), WM_MSGID(EID_JOINCHANNEL_SUCCESS), TRUE, 0);
    return 0;
}
//EID_LEAVE_CHANNEL message window hander.
LRESULT CAgoraMetaDataDlg::OnEIDLeaveChannel(WPARAM wParam, LPARAM lParam)
{
    m_btnJoinChannel.EnableWindow(TRUE);
    joinChannel = false;
    m_btnJoinChannel.SetWindowText(_T("JoinChannel"));

    CString strInfo;
    strInfo.Format(_T("leave channel success %s"), getCurrentTime());
    m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
  
    //notify parent window
    ::PostMessage(GetParent()->GetSafeHwnd(), WM_MSGID(EID_JOINCHANNEL_SUCCESS), FALSE, 0);
    return 0;
}

//EID_USER_JOINED message window handler.
LRESULT CAgoraMetaDataDlg::OnEIDUserJoined(WPARAM wParam, LPARAM lParam)
{
    CString strInfo;
    strInfo.Format(_T("%u joined"), wParam);
    m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

  /*  VideoCanvas canvas;
    canvas.uid = wParam;
    canvas.view = m_remoteVideoWnd.GetSafeHwnd();
    canvas.renderMode = RENDER_MODE_FIT;
    m_rtcEngine->setupRemoteVideo(canvas);*/
    return 0;
}

//EID_USER_OFFLINE message window handler.
LRESULT CAgoraMetaDataDlg::OnEIDUserOffline(WPARAM wParam, LPARAM lParam)
{
    uid_t remoteUid = (uid_t)wParam;
    VideoCanvas canvas;
    canvas.uid = remoteUid;
    canvas.view = NULL;
    //set up remote video in the engine to canvas.
    m_rtcEngine->setupRemoteVideo(canvas);
    CString strInfo;
    strInfo.Format(_T("%u offline, reason:%d"), remoteUid, lParam);
    m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

    return 0;
}

//EID_REMOTE_VIDEO_STATE_CHANED message window handler.
LRESULT CAgoraMetaDataDlg::OnEIDRemoteVideoStateChanged(WPARAM wParam, LPARAM lParam)
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
//RECV_METADATA_MSG message window handler.
LRESULT CAgoraMetaDataDlg::OnEIDMetadataReceived(WPARAM wParam, LPARAM lParam)
{
    IMetadataObserver::Metadata* metaData = (IMetadataObserver::Metadata*)wParam;
    CString strInfo;
    strInfo.Format(_T("onMetadataReceived:uid:%u, ts=%d, size:%d."), metaData->uid, metaData->timeStampMs, metaData->size, metaData->buffer);
    
    if (metaData->size > 0) {
        CString str;
        str.Format(_T("Info: %S"), metaData->buffer);
        strInfo += str;
    }
    m_edtRecvSEI.SetWindowText(strInfo);
    return 0;
}

//render local video from SDK local capture.
void CAgoraMetaDataDlg::RenderLocalVideo()
{
    if (m_rtcEngine) {
        //start preivew in the engine.
        m_rtcEngine->startPreview();
        VideoCanvas canvas;
        canvas.renderMode = RENDER_MODE_FIT;
        canvas.uid = 0;
        canvas.view = m_localVideoWnd.GetSafeHwnd();
        //set up local video in the engine to canvas.
        m_rtcEngine->setupLocalVideo(canvas);
        m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("render local video"));
    }
}

void CAgoraMetaDataDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
    CDialogEx::OnShowWindow(bShow, nStatus);

    if (bShow) {
        RenderLocalVideo();
    }
}

//send button handler.
void CAgoraMetaDataDlg::OnBnClickedButtonSend()
{
    CString strSend;
    m_edtSendSEI.GetWindowText(strSend);
    if (strSend.IsEmpty())
        return;
    std::string utf8msg = cs2utf8(strSend);
    //set send messag string.
    m_metaDataObserver.SetSendSEI(utf8msg);
}

//clear button handler.
void CAgoraMetaDataDlg::OnBnClickedButtonClear()
{
    m_edtSendSEI.SetWindowText(_T(""));
    //set send messag string.
    m_metaDataObserver.SetSendSEI("");
}


