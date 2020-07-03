// AgoraRtmpInjectionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "APIExample.h"
#include "CAgoraRtmpInjectionDlg.h"
#include "afxdialogex.h"
void CAgoraRtmpInjectionRtcEngineEventHandler::onJoinChannelSuccess(const char* channel, uid_t uid, int elapsed)
{
    // send EID_JOINCHANNEL_SUCCESS to UI
    if (m_hMsgHanlder) {
        ::PostMessage(m_hMsgHanlder, WM_MSGID(EID_JOINCHANNEL_SUCCESS), (WPARAM)uid, (LPARAM)elapsed);
    }
}

void CAgoraRtmpInjectionRtcEngineEventHandler::onStreamInjectedStatus(const char* url, uid_t uid, int status)
{
    // send EID_INJECT_STATUS to UI
    if (m_hMsgHanlder) {
        ::PostMessage(m_hMsgHanlder, WM_MSGID(EID_INJECT_STATUS), (WPARAM)uid, (LPARAM)status);
    }
}

void CAgoraRtmpInjectionRtcEngineEventHandler::onLeaveChannel(const RtcStats& stats)
{
    // send EID_LEAVE_CHANNEL to UI
    if (m_hMsgHanlder) {
        ::PostMessage(m_hMsgHanlder, WM_MSGID(EID_LEAVE_CHANNEL), 0, 0);
    }
}

void CAgoraRtmpInjectionRtcEngineEventHandler::onUserJoined(uid_t uid, int elapsed)
{
    // send EID_USER_JOINED to UI
    if (m_hMsgHanlder) {
        ::PostMessage(m_hMsgHanlder, WM_MSGID(EID_USER_JOINED), (WPARAM)uid, (LPARAM)elapsed);
    }
}

void CAgoraRtmpInjectionRtcEngineEventHandler::onUserOffline(uid_t uid, USER_OFFLINE_REASON_TYPE reason)
{
    // send EID_USER_OFFLINE to UI
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
    //init ctrl
    InitCtrlText();
    //create local video wnd
    m_localVideoWnd.Create(NULL, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CRect(0, 0, 1, 1), this, ID_BASEWND_VIDEO + 200);

    //move local video wnd
    RECT rcArea;
    m_staVideoArea.GetClientRect(&rcArea);
    m_localVideoWnd.MoveWindow(&rcArea);
    m_localVideoWnd.ShowWindow(SW_SHOW);

    m_btnAddStream.EnableWindow(FALSE);
    m_edtInjectUrl.EnableWindow(FALSE);
    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}

void CAgoraRtmpInjectionDlg::InitCtrlText()
{
    m_staInjectUrl.SetWindowText(rtmpInjectCtrlUrl);
    m_btnAddStream.SetWindowText(rtmpInjectCtrlInject);
    m_staChannelName.SetWindowText(commonCtrlChannel);
    m_btnJoinChannel.SetWindowText(commonCtrlJoinChannel);
}

bool CAgoraRtmpInjectionDlg::InitAgora()
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
    //set client role to broadcaster
    m_rtcEngine->setClientRole(CLIENT_ROLE_BROADCASTER);
    m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("setClientRole broadcaster"));

    m_btnJoinChannel.EnableWindow(TRUE);
    return true;
}

//uninit agora after receive onLeaveChannel event
void CAgoraRtmpInjectionDlg::UnInitAgora()
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
void CAgoraRtmpInjectionDlg::RenderLocalVideo()
{
    if (m_rtcEngine) {
        //start preview
        m_rtcEngine->startPreview();
        m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("startPreview"));
        VideoCanvas canvas;
        canvas.renderMode = RENDER_MODE_FIT;
        canvas.uid = 0;
        canvas.view = m_localVideoWnd.GetSafeHwnd();
        // Setup local video to render your local camera preview
        m_rtcEngine->setupLocalVideo(canvas);
        m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("setupLocalVideo"));
       
    }
}


void CAgoraRtmpInjectionDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
    CDialogEx::OnShowWindow(bShow, nStatus);
    //start local video render when this scene dialog show
    if (bShow) {
        RenderLocalVideo();
    }
}

void CAgoraRtmpInjectionDlg::OnBnClickedButtonAddstream()
{
    if (!m_rtcEngine || !m_initialize)
        return;

    if (m_addInjectStream) {//remove stream
        m_addInjectStream = false;
        m_edtInjectUrl.EnableWindow(TRUE);
        m_btnAddStream.SetWindowText(_T("Inject URL"));
        //remove inject stream
        int ret = m_rtcEngine->removeInjectStreamUrl(m_injectUrl.c_str());
    }
    else {//inject stream
        CString strURL;
        m_edtInjectUrl.GetWindowText(strURL);
        if (strURL.IsEmpty()) {
            AfxMessageBox(_T("Fill INJECT URL first"));
            return;
        }

        std::string szURL = cs2utf8(strURL);
        InjectStreamConfig config;
        //inject stream
        m_rtcEngine->addInjectStreamUrl(szURL.c_str(), config);
        m_injectUrl = szURL;
        m_addInjectStream = true;
        //disable add button after inject stream
        m_edtInjectUrl.EnableWindow(FALSE);
        m_btnAddStream.SetWindowText(_T("Remove URL"));
    }
    m_btnAddStream.EnableWindow(FALSE);
    m_edtInjectUrl.EnableWindow(FALSE);
}


void CAgoraRtmpInjectionDlg::OnBnClickedButtonJoinchannel()
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
        //leave channel
        if (0 == m_rtcEngine->leaveChannel()) {
            m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("leave channel"));
            m_btnJoinChannel.EnableWindow(FALSE);
        }
    }
}

LRESULT CAgoraRtmpInjectionDlg::OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam)
{
    m_btnJoinChannel.EnableWindow(TRUE);
    joinChannel = true;
    m_btnJoinChannel.SetWindowText(_T("LeaveChannel"));
    //must inject stream after join channel success,enable add button
    m_btnAddStream.EnableWindow(TRUE);
    m_edtInjectUrl.EnableWindow(TRUE);
    CString strInfo;
    strInfo.Format(_T("%s:join success, uid=%u"), getCurrentTime(), wParam);
    m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
    //notify parent window not change scene after join channel success
    ::PostMessage(GetParent()->GetSafeHwnd(), WM_MSGID(EID_JOINCHANNEL_SUCCESS), TRUE, 0);
    return 0;
}

LRESULT CAgoraRtmpInjectionDlg::OnEIDLeaveChannel(WPARAM wParam, LPARAM lParam)
{
    m_btnJoinChannel.EnableWindow(TRUE);
    joinChannel = false;
    m_btnJoinChannel.SetWindowText(_T("JoinChannel"));
    CString strInfo;
    strInfo.Format(_T("leave channel success"));
    m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
    //notify parent window leave channel success
    ::PostMessage(GetParent()->GetSafeHwnd(), WM_MSGID(EID_JOINCHANNEL_SUCCESS), FALSE, 0);
    return 0;
}

LRESULT CAgoraRtmpInjectionDlg::OnEIDUserJoined(WPARAM wParam, LPARAM lParam)
{
    uid_t remoteUid = (uid_t)wParam;
    if (remoteUid == 666) {//inject stream join channel with uid 666
        CString strInfo;
        strInfo.Format(_T("%u joined, 666 is inject stream"), remoteUid);
        m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
        //mute audio and video steam.
        //If you want receive audio and video steam, skip these tow api.
        m_rtcEngine->muteRemoteAudioStream(666, true);
        m_rtcEngine->muteRemoteVideoStream(666, true);
    }
   
    return 0;
}

LRESULT CAgoraRtmpInjectionDlg::OnEIDUserOffline(WPARAM wParam, LPARAM lParam)
{
    uid_t remoteUid = (uid_t)wParam;
    if (remoteUid == 666) {//inject stream is removed
        CString strInfo;
        strInfo.Format(_T("%u offline, reason:%d"), remoteUid, lParam);
        m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
    }
    return 0;
}

LRESULT CAgoraRtmpInjectionDlg::OnEIDStreamInjectedStatus(WPARAM wParam, LPARAM lParam)
{
    CString strInfo;
    switch ((INJECT_STREAM_STATUS)lParam)
    {
        //The external video stream imported successfully
    case INJECT_STREAM_STATUS_START_SUCCESS:
        strInfo.Format(_T("%s, err: %d。"), agoraInjectStartSucc, 0);
        break;
        //The external video stream already exists
    case INJECT_STREAM_STATUS_START_ALREADY_EXISTS:
        strInfo.Format(_T("%s, err: %d。"), agoraInjectExist, 1);
        break;
        //The external video stream to be imported is unauthorized
    case INJECT_STREAM_STATUS_START_UNAUTHORIZED:
        strInfo.Format(_T("%s, err: %d。"), agoraInjectStartUnAuth, 2);
        break;
        //Import external video stream timeout.
    case INJECT_STREAM_STATUS_START_TIMEDOUT:
        strInfo.Format(_T("%s, err: %d。"), agoraInjectStartTimeout, 3);
        break;
        //Import external video stream failed
    case INJECT_STREAM_STATUS_START_FAILED:
        strInfo.Format(_T("%s, err: %d。"), agoraInjectStartFailed, 4);
        break;
        //The external video stream stopped importing successfully
    case INJECT_STREAM_STATUS_STOP_SUCCESS:
        strInfo.Format(_T("%s, err: %d。"), agoraInjectStopSuccess, 5);
        break;
        //No external video stream is found
    case INJECT_STREAM_STATUS_STOP_NOT_FOUND:
        strInfo.Format(_T("%s, err: %d。"), agoraInjectNotFound, 6);
        break;
        //The external video stream to be stopped importing is unauthorized
    case INJECT_STREAM_STATUS_STOP_UNAUTHORIZED:
        strInfo.Format(_T("%s, err: %d。"), agoraInjectStopUnAuth, 7);
        break;
        //Stop importing external video stream timeout
    case INJECT_STREAM_STATUS_STOP_TIMEDOUT:
        strInfo.Format(_T("%s, err: %d。"), agoraInjectStopTimeout, 8);
        break;
        //Stop importing external video stream failed
    case INJECT_STREAM_STATUS_STOP_FAILED:
        strInfo.Format(_T("%s, err: %d。"), agoraInjectStopFailed, 9);
        break;
        //The external video stream is corrupted
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

//show list info details
void CAgoraRtmpInjectionDlg::OnSelchangeListInfoBroadcasting()
{
    int sel = m_lstInfo.GetCurSel();
    CString strDetail;
    m_lstInfo.GetText(sel, strDetail);
    m_staDetail.SetWindowText(strDetail);
}
