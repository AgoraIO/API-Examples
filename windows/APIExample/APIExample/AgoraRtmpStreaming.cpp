// AgoraRtmpStreaming.cpp : implementation file
//

#include "stdafx.h"
#include "APIExample.h"
#include "CAgoraRtmpStreaming.h"
#include "afxdialogex.h"

void CAgoraRtmpStreamingDlgRtcEngineEventHandler::onJoinChannelSuccess(const char* channel, uid_t uid, int elapsed)
{
    // send EID_JOINCHANNEL_SUCCESS to UI
    if (m_hMsgHanlder) {
        ::PostMessage(m_hMsgHanlder, WM_MSGID(EID_JOINCHANNEL_SUCCESS), (WPARAM)uid, (LPARAM)elapsed);
    }
}

void CAgoraRtmpStreamingDlgRtcEngineEventHandler::onLeaveChannel(const RtcStats& stats)
{
    // send EID_LEAVE_CHANNEL to UI
    if (m_hMsgHanlder) {
        ::PostMessage(m_hMsgHanlder, WM_MSGID(EID_LEAVE_CHANNEL), 0, 0);
    }
}

void CAgoraRtmpStreamingDlgRtcEngineEventHandler::onUserJoined(uid_t uid, int elapsed)
{
    // send EID_USER_JOINED to UI
    if (m_hMsgHanlder) {
        ::PostMessage(m_hMsgHanlder, WM_MSGID(EID_USER_JOINED), (WPARAM)uid, (LPARAM)elapsed);
    }
}

void CAgoraRtmpStreamingDlgRtcEngineEventHandler::onUserOffline(uid_t uid, USER_OFFLINE_REASON_TYPE reason)
{
    // send EID_USER_OFFLINE to UI
    if (m_hMsgHanlder) {
        ::PostMessage(m_hMsgHanlder, WM_MSGID(EID_USER_OFFLINE), (WPARAM)uid, (LPARAM)reason);
    }
}
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
// CAgoraRtmpStreamingDlgDlg dialog

// CAgoraRtmpStreamingDlg dialog

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


// CAgoraRtmpStreamingDlg message handlers


bool CAgoraRtmpStreamingDlg::InitAgora()
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
    m_rtcEngine->initialize(context);
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

    m_rtcEngine->setChannelProfile(CHANNEL_PROFILE_LIVE_BROADCASTING);
    m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("live broadcasting"));
    m_rtcEngine->setClientRole(CLIENT_ROLE_BROADCASTER);
    m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("setClientRole broadcaster"));

    m_btnJoinChannel.EnableWindow(TRUE);
    return true;
}


//uninit agora after receive onLeaveChannel event
void CAgoraRtmpStreamingDlg::UnInitAgora()
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

BOOL CAgoraRtmpStreamingDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    //init ctrls
    InitCtrlText();
    //create local render video wnd
    m_localVideoWnd.Create(NULL, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CRect(0, 0, 1, 1), this, ID_BASEWND_VIDEO + 100);

    RECT rcArea;
    m_staVideoArea.GetClientRect(&rcArea);
    m_localVideoWnd.MoveWindow(&rcArea);
    m_localVideoWnd.ShowWindow(SW_SHOW);
    m_btnAddStream.EnableWindow(FALSE);
    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}

//init ctrl text
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
    //start local video render when this scene dialog show
    if (bShow) {
        RenderLocalVideo();
    }
}


void CAgoraRtmpStreamingDlg::RenderLocalVideo()
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

//remove all publish urls
void CAgoraRtmpStreamingDlg::RemoveAllRtmpUrls()
{
    m_bRemoveAll = true;
    CString strUrl;
    for (int i = 0; i < m_cmbRtmpUrl.GetCount(); ++i) {
        m_cmbRtmpUrl.GetLBText(i, strUrl);
        std::string szUrl = cs2utf8(strUrl);
        m_rtcEngine->removePublishStreamUrl(szUrl.c_str());
    }
    m_cmbRtmpUrl.Clear();
    m_cmbRtmpUrl.ResetContent();
}

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
        //join channel, if uid=0 agora server will calculate an uid and return it 
        if (0 == m_rtcEngine->joinChannel(APP_TOKEN, szChannelId.c_str(), "", 0)) {
            m_btnJoinChannel.EnableWindow(FALSE);
        }
    }
    else {
        if (m_cmbRtmpUrl.GetCount() > 0) {
            m_btnAddStream.EnableWindow(FALSE);
            m_btnRemoveStream.EnableWindow(FALSE);
            m_btnJoinChannel.EnableWindow(FALSE);
            //remove all publish urls before leave channel
            RemoveAllRtmpUrls();
        }
        else  if (0 == m_rtcEngine->leaveChannel()) {//leave channel
            m_btnJoinChannel.EnableWindow(FALSE);
        }
    }
}

//add publish stream url. Add one publish url once.
//you can add many times, less than 10.
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
    int ret = m_rtcEngine->addPublishStreamUrl(szURL.c_str(), false);// no transcoding

    if (ret != 0) {
        CString strInfo;
        strInfo.Format(_T("addPublishStreamUrl failed:%d"), ret);
        m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
        return;
    }
}

//remove publish stream url
void CAgoraRtmpStreamingDlg::OnBnClickedButtonRemoveStream()
{
    if (!m_rtcEngine || !m_initialize)
        return;

    if (m_cmbRtmpUrl.GetCount() == 0)
        return;

    CString strUrl;
    m_cmbRtmpUrl.GetWindowText(strUrl);
    std::string szUrl = cs2utf8(strUrl);
    m_rtcEngine->removePublishStreamUrl(szUrl.c_str());
}

//remove all publish stream urls
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
    m_rtcEngine->removePublishStreamUrl(szUrl.c_str());
    m_btnRemoveStream.EnableWindow(FALSE);
}

LRESULT CAgoraRtmpStreamingDlg::OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam)
{
    //must add publish stream after join channel success,enable add button
    m_btnJoinChannel.EnableWindow(TRUE);
    m_btnAddStream.EnableWindow(TRUE);
    m_btnRemoveStream.EnableWindow(TRUE);
    joinChannel = true;
    m_btnJoinChannel.SetWindowText(_T("LeaveChannel"));
    CString strInfo;
    strInfo.Format(_T("%s:join success, uid=%u"), getCurrentTime(), wParam);
    m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

    m_btnAddStream.EnableWindow(TRUE);
    //notify parent window not change scene after join channel success
    ::PostMessage(GetParent()->GetSafeHwnd(), WM_MSGID(EID_JOINCHANNEL_SUCCESS), TRUE, 0);
    return 0;
}

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
    //notify parent window leave channel success
    ::PostMessage(GetParent()->GetSafeHwnd(), WM_MSGID(EID_JOINCHANNEL_SUCCESS), FALSE, 0);
    return 0;
}

LRESULT CAgoraRtmpStreamingDlg::OnEIDRtmpStateChanged(WPARAM wParam, LPARAM lParam)
{
    PRtmpStreamStreamStateChanged rtmpState = (PRtmpStreamStreamStateChanged)wParam;
    CString strInfo;
    m_btnRemoveStream.EnableWindow(TRUE);
    switch (rtmpState->state)
    {
        //Idle remove stream url success
    case RTMP_STREAM_PUBLISH_STATE_IDLE:
    {
        strInfo.Format(_T("%s:%S¡£"), agoraRtmpStateIdle, rtmpState->url);
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
    //The SDK is connecting to Agora's streaming server and the RTMP server.
    case RTMP_STREAM_PUBLISH_STATE_CONNECTING:
    {
        strInfo = agoraRtmpStateConnecting;
    }
    break;
    //The RTMP streaming publishes. 
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
        //The RTMP streaming is recovering
    case RTMP_STREAM_PUBLISH_STATE_RECOVERING:
        strInfo.Format(agoraRtmpStateRecovering);
        break;
        // RTMP Streaming failed
    case RTMP_STREAM_PUBLISH_STATE_FAILURE:
    {
        switch (rtmpState->state)
        {
            //Invalid argument used.
        case RTMP_STREAM_PUBLISH_ERROR_INVALID_ARGUMENT:
        {
            strInfo = agoraRtmpStateInvalidArg;
        }
        break;
        //Invalid argument used.
        case RTMP_STREAM_PUBLISH_ERROR_ENCRYPTED_STREAM_NOT_ALLOWED:
        {
            strInfo = agoraRtmpStateEncrypted;
        }
        break;
        //Timeout for the RTMP streaming.
        case RTMP_STREAM_PUBLISH_ERROR_CONNECTION_TIMEOUT:
        {
            strInfo = agoraRtmpStateConnTimeout;
        }
        break;
        //Timeout for the RTMP streaming.
        case RTMP_STREAM_PUBLISH_ERROR_INTERNAL_SERVER_ERROR:
        {
            strInfo = agoraRtmpStateInrealErr;
        }
        break;
        //Timeout for the RTMP streaming.
        case RTMP_STREAM_PUBLISH_ERROR_RTMP_SERVER_ERROR:
        {
            strInfo = agoraRtmpStateServerErr;
        }
        break;
        //The RTMP streaming publishes too frequently.
        case RTMP_STREAM_PUBLISH_ERROR_TOO_OFTEN:
        {
            strInfo = agoraRtmpStateTooOften;
        }
        break;
        //The host publishes more than 10 URLs. Delete the unnecessary URLs before adding new ones.
        case RTMP_STREAM_PUBLISH_ERROR_REACH_LIMIT:
        {
            strInfo = agoraRtmpStateReachLimit;
        }
        break;
        //The host manipulates other hosts' URLs. Check your app logic.
        case RTMP_STREAM_PUBLISH_ERROR_NOT_AUTHORIZED:
        {
            strInfo = agoraRtmpStateNotAuth;
        }
        break;
        //Agora's server fails to find the RTMP streaming.
        case RTMP_STREAM_PUBLISH_ERROR_STREAM_NOT_FOUND:
        {
            strInfo = agoraRtmpStateNotFound;
        }
        break;
        //The format of the RTMP streaming URL is not supported.
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
//show list info details
void CAgoraRtmpStreamingDlg::OnSelchangeListInfoBroadcasting()
{
    int sel = m_lstInfo.GetCurSel();
    CString strDetail;
    m_lstInfo.GetText(sel, strDetail);
    m_staDetail.SetWindowText(strDetail);
}

