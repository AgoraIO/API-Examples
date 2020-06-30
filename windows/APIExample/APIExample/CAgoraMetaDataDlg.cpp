// CAgoraMetaDataDlg.cpp : implementation file
//

#include "stdafx.h"
#include "APIExample.h"
#include "CAgoraMetaDataDlg.h"
#include "afxdialogex.h"
//metadata


void CAgoraMetaDataObserver::SetMaxMetadataSize(int maxSize)
{
    if (maxSize > 1024)
        maxSize = 1024;
    m_maxSize = maxSize;
}

int CAgoraMetaDataObserver::getMaxMetadataSize()
{
    return m_maxSize;
}
// send metadata callback
bool CAgoraMetaDataObserver::onReadyToSendMetadata(Metadata &metadata)
{
    if (m_sendSEI.length() > 0) {
        memcpy_s(metadata.buffer, m_sendSEI.length(), m_sendSEI.c_str(), m_sendSEI.length());

    }
    metadata.size = m_sendSEI.length();
    return true;
}
// receive metadata callback
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

void CAgoraMetaDataObserver::SetSendSEI(std::string utf8Msg)
{
    m_sendSEI = utf8Msg;
   
}
//event
void CAgoraMetaDataEventHanlder::onJoinChannelSuccess(const char* channel, uid_t uid, int elapsed)
{
    // send EID_JOINCHANNEL_SUCCESS to UI
    if (m_hMsgHanlder) {
        ::PostMessage(m_hMsgHanlder, WM_MSGID(EID_JOINCHANNEL_SUCCESS), (WPARAM)uid, (LPARAM)elapsed);
    }
}

void CAgoraMetaDataEventHanlder::onUserJoined(uid_t uid, int elapsed) {
    // send EID_USER_JOINED to UI
    if (m_hMsgHanlder) {
        ::PostMessage(m_hMsgHanlder, WM_MSGID(EID_USER_JOINED), (WPARAM)uid, (LPARAM)elapsed);
    }
}

void CAgoraMetaDataEventHanlder::onUserOffline(uid_t uid, USER_OFFLINE_REASON_TYPE reason)
{
    // send EID_USER_OFFLINE to UI
    if (m_hMsgHanlder) {
        ::PostMessage(m_hMsgHanlder, WM_MSGID(EID_USER_OFFLINE), (WPARAM)uid, (LPARAM)reason);
    }
}

void CAgoraMetaDataEventHanlder::onLeaveChannel(const RtcStats& stats)
{
    // send EID_LEAVE_CHANNEL to UI
    if (m_hMsgHanlder) {
        ::PostMessage(m_hMsgHanlder, WM_MSGID(EID_LEAVE_CHANNEL), 0, 0);
    }
}

void CAgoraMetaDataEventHanlder::onRemoteVideoStateChanged(uid_t uid, REMOTE_VIDEO_STATE state, REMOTE_VIDEO_STATE_REASON reason, int elapsed)
{
    //send EID_REMOTE_VIDEO_STATE_CHANED to UI
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
    DDX_Control(pDX, IDC_LIST_INFO_METADATA, m_lstInfo);
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
    ON_LBN_SELCHANGE(IDC_LIST_INFO_METADATA, &CAgoraMetaDataDlg::OnSelchangeListInfoMetadata)
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


BOOL CAgoraMetaDataDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    //init ctrl
    InitCtrlText();
    //create local video wnd and remote video wnd
    m_localVideoWnd.Create(NULL, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CRect(0, 0, 1, 1), this, ID_BASEWND_VIDEO + 100);
    m_remoteVideoWnd.Create(NULL, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CRect(0, 0, 1, 1), this, ID_BASEWND_VIDEO + 100);

    //move and show vido wnds
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


void CAgoraMetaDataDlg::InitCtrlText()
{
    m_staMetaData.SetWindowText(videoSEIInformation);
    m_staSendSEI.SetWindowText(metadataCtrlSendSEI);
    m_btnSendSEI.SetWindowText(metadataCtrlBtnSend);
    m_staChannelName.SetWindowText(commonCtrlChannel);
    m_btnJoinChannel.SetWindowText(commonCtrlJoinChannel);
}

bool CAgoraMetaDataDlg::InitAgora()
{
    //create agora rtc engine instance
    //agora rtc engine is Singleton in one process
    m_rtcEngine = createAgoraRtcEngine();
    if (!m_rtcEngine) {
        m_lstInfo.InsertString(m_lstInfo.GetCount() - 1, _T("createAgoraRtcEngine failed"));
        return false;
    }
    //set msg receiver for agora event
    m_eventHandler.SetMsgReceiver(m_hWnd);

    RtcEngineContext context;
    context.appId = APP_ID;//assign appid
    context.eventHandler = &m_eventHandler;
    //initialize engine
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
    //enable video
    m_rtcEngine->enableVideo();
    m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("enable video"));
    //set channel profile to live broadcasting
    m_rtcEngine->setChannelProfile(CHANNEL_PROFILE_LIVE_BROADCASTING);
    m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("live broadcasting"));
    m_rtcEngine->setClientRole(CLIENT_ROLE_BROADCASTER);
    m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("setClientRole broadcaster"));

    //registe mediaMetadataObserver
    m_metaDataObserver.SetMsgReceiver(m_hWnd);
    m_rtcEngine->registerMediaMetadataObserver(&m_metaDataObserver, IMetadataObserver::VIDEO_METADATA);
    m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("registerMediaMetadataObserver"));

    m_btnJoinChannel.EnableWindow(TRUE);
    return true;
}

//uninit agora after receive onLeaveChannel event
void CAgoraMetaDataDlg::UnInitAgora()
{
    if (m_rtcEngine) {
        //stop preview
        m_rtcEngine->stopPreview();
        m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("stopPreview"));
        // disable video
        m_rtcEngine->disableVideo();
        m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("disableVideo"));
        // release rtcengine
        m_rtcEngine->release(true);
        m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("release rtc engine"));
        m_rtcEngine = NULL;
    }
}

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
   
    //notify parent window not change scene after join channel success
    ::PostMessage(GetParent()->GetSafeHwnd(), WM_MSGID(EID_JOINCHANNEL_SUCCESS), TRUE, 0);
    return 0;
}

LRESULT CAgoraMetaDataDlg::OnEIDLeaveChannel(WPARAM wParam, LPARAM lParam)
{
    m_btnJoinChannel.EnableWindow(TRUE);
    joinChannel = false;
    m_btnJoinChannel.SetWindowText(_T("JoinChannel"));

    CString strInfo;
    strInfo.Format(_T("leave channel success %s"), getCurrentTime());
    m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
  
    //notify parent window leave channel success
    ::PostMessage(GetParent()->GetSafeHwnd(), WM_MSGID(EID_JOINCHANNEL_SUCCESS), FALSE, 0);
    return 0;
}

LRESULT CAgoraMetaDataDlg::OnEIDUserJoined(WPARAM wParam, LPARAM lParam)
{
    CString strInfo;
    strInfo.Format(_T("%u joined"), wParam);
    m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
    //setupRemoteViedeo:Initializes the video view of a remote user.
  //VideoCanvas:set canvas for render video by assigning uid and hwnd.
    VideoCanvas canvas;
    canvas.uid = wParam;
    canvas.view = m_remoteVideoWnd.GetSafeHwnd();
    canvas.renderMode = RENDER_MODE_FIT;
    m_rtcEngine->setupRemoteVideo(canvas);
    return 0;
}

LRESULT CAgoraMetaDataDlg::OnEIDUserOffline(WPARAM wParam, LPARAM lParam)
{
    //clear remote video render fater remote user offline
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

LRESULT CAgoraMetaDataDlg::OnEIDMetadataReceived(WPARAM wParam, LPARAM lParam)
{
    //show video metadata send by remote users
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

void CAgoraMetaDataDlg::RenderLocalVideo()
{
    if (m_rtcEngine) {
        //start preview
        m_rtcEngine->startPreview();
        VideoCanvas canvas;
        canvas.renderMode = RENDER_MODE_FIT;
        canvas.uid = 0;
        canvas.view = m_localVideoWnd.GetSafeHwnd();
        // Setup local video to render your local camera preview
        m_rtcEngine->setupLocalVideo(canvas);
        m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("render local video"));
    }
}

void CAgoraMetaDataDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
    CDialogEx::OnShowWindow(bShow, nStatus);
    //start local video render when this scene dialog show
    if (bShow) {
        RenderLocalVideo();
    }
}

//set send sei info
void CAgoraMetaDataDlg::OnBnClickedButtonSend()
{
    CString strSend;
    m_edtSendSEI.GetWindowText(strSend);
    if (strSend.IsEmpty())
        return;
    std::string utf8msg = cs2utf8(strSend);
    m_metaDataObserver.SetSendSEI(utf8msg);
}

//clear send sei info
void CAgoraMetaDataDlg::OnBnClickedButtonClear()
{
    m_edtSendSEI.SetWindowText(_T(""));
    m_metaDataObserver.SetSendSEI("");
}

//show list info details
void CAgoraMetaDataDlg::OnSelchangeListInfoMetadata()
{
    int sel = m_lstInfo.GetCurSel();
    CString strDetail;
    m_lstInfo.GetText(sel, strDetail);
    m_staMetaData.SetWindowText(strDetail);
}
