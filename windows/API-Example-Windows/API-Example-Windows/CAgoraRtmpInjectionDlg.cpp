// CAgoraRtmpInjectionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "API-Example-Windows.h"
#include "CAgoraRtmpInjectionDlg.h"
#include "afxdialogex.h"



void CAgoraRtmpInjectionRtcEngineEventHandler::onJoinChannelSuccess(const char* channel, uid_t uid, int elapsed)
{
    if (m_hMsgHanlder) {
        ::PostMessage(m_hMsgHanlder, WM_MSGID(EID_JOINCHANNEL_SUCCESS), (WPARAM)uid, (LPARAM)elapsed);
    }
}

void CAgoraRtmpInjectionRtcEngineEventHandler::onStreamInjectedStatus(const char* url, uid_t uid, int status)
{
    if (m_hMsgHanlder) {
        ::PostMessage(m_hMsgHanlder, WM_MSGID(EID_INJECT_STATUS), (WPARAM)uid, (LPARAM)status);
    }
}

void CAgoraRtmpInjectionRtcEngineEventHandler::onLeaveChannel(const RtcStats& stats)
{
    if (m_hMsgHanlder) {
        ::PostMessage(m_hMsgHanlder, WM_MSGID(EID_LEAVE_CHANNEL), 0, 0);
    }
}

void CAgoraRtmpInjectionRtcEngineEventHandler::onUserJoined(uid_t uid, int elapsed)
{
    if (m_hMsgHanlder) {
        ::PostMessage(m_hMsgHanlder, WM_MSGID(EID_USER_JOINED), (WPARAM)uid, (LPARAM)elapsed);
    }
}

void CAgoraRtmpInjectionRtcEngineEventHandler::onUserOffline(uid_t uid, USER_OFFLINE_REASON_TYPE reason)
{
    if (m_hMsgHanlder) {
        ::PostMessage(m_hMsgHanlder, WM_MSGID(EID_USER_OFFLINE), (WPARAM)uid, (LPARAM)reason);
    }
}

// CAgoraRtmpInjectionDlg dialog

IMPLEMENT_DYNAMIC(CAgoraRtmpInjectionDlg, CDialogEx)

CAgoraRtmpInjectionDlg::CAgoraRtmpInjectionDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_RTMP_INJECTION, pParent)
{

}

CAgoraRtmpInjectionDlg::~CAgoraRtmpInjectionDlg()
{
}

void CAgoraRtmpInjectionDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDIT_RTMP_URL, m_edtRtmpUrl);
    DDX_Control(pDX, IDC_EDIT_CHANNELNAME, m_edtChannelName);
    DDX_Control(pDX, ID_JOINCHANNEL, m_btnJoinChannel);
    DDX_Control(pDX, IDC_BUTTON_ADDSTREAM, m_btnAddStream);
    DDX_Control(pDX, IDC_STATIC_INFO, m_staInfo);
    DDX_Control(pDX, IDC_STATIC_LOCALVIDEO, m_staLocalVideo);
    DDX_Control(pDX, IDC_STATIC_LOCALVIDEO2, m_injectVideoWnd);
}


BEGIN_MESSAGE_MAP(CAgoraRtmpInjectionDlg, CDialogEx)
    ON_BN_CLICKED(IDC_BUTTON_ADDSTREAM, &CAgoraRtmpInjectionDlg::OnBnClickedButtonAddstream)
    ON_BN_CLICKED(ID_JOINCHANNEL, &CAgoraRtmpInjectionDlg::OnBnClickedJoinchannel)
    ON_MESSAGE(WM_MSGID(EID_JOINCHANNEL_SUCCESS), &CAgoraRtmpInjectionDlg::OnEIDJoinChannelSuccess)
    ON_MESSAGE(WM_MSGID(EID_LEAVE_CHANNEL), &CAgoraRtmpInjectionDlg::OnEIDLeaveChannel)
    ON_MESSAGE(WM_MSGID(EID_INJECT_STATUS), &CAgoraRtmpInjectionDlg::OnEIDStreamInjectedStatus)
    ON_MESSAGE(WM_MSGID(EID_USER_JOINED), &CAgoraRtmpInjectionDlg::OnEIDUserJoined)
    ON_MESSAGE(WM_MSGID(EID_USER_OFFLINE), &CAgoraRtmpInjectionDlg::OnEIDUserOffline)

    ON_WM_DESTROY()
END_MESSAGE_MAP()


// CAgoraRtmpInjectionDlg message handlers


BOOL CAgoraRtmpInjectionDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    EnableInjectWindow(FALSE);

    m_rtcEngine = createAgoraRtcEngine();
    if (!m_rtcEngine) {
        m_staInfo.SetWindowText(_T("createAgoraRtcEngine failed"));
        return TRUE;
    }
    m_eventHandlerInjection.SetMsgReceiver(m_hWnd);

    RtcEngineContext context;
    context.appId = APP_ID;
    context.eventHandler = &m_eventHandlerInjection;
    int ret = m_rtcEngine->initialize(context);
    if (ret != 0) {
        m_initialize = false;
        m_staInfo.SetWindowText(_T("initialize failed"));
        return TRUE;
    }
    else
        m_initialize = true;

    m_rtcEngine->enableVideo();
    m_rtcEngine->setChannelProfile(CHANNEL_PROFILE_LIVE_BROADCASTING);
    m_rtcEngine->setClientRole(CLIENT_ROLE_BROADCASTER);

    m_rtcEngine->startPreview();

    VideoCanvas canvas;
    canvas.renderMode = RENDER_MODE_FIT;
    canvas.uid = 0;
    canvas.view = m_staLocalVideo.GetSafeHwnd();
    m_rtcEngine->setupLocalVideo(canvas);
    // TODO:  Add extra initialization here

    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}


void CAgoraRtmpInjectionDlg::EnableInjectWindow(BOOL bEnable)
{
    m_edtRtmpUrl.EnableWindow(bEnable);
    m_btnAddStream.EnableWindow(bEnable);
}


void CAgoraRtmpInjectionDlg::OnBnClickedButtonAddstream()
{
    if (!m_rtcEngine || !m_initialize)
        return;

    if (addInjectStream) {
        addInjectStream = false;
        m_edtRtmpUrl.EnableWindow(TRUE);
        m_btnAddStream.SetWindowText(_T("Add URL"));
        int ret = m_rtcEngine->removeInjectStreamUrl(injectUrl.c_str());
    }
    else {
        CString strURL;
        m_edtRtmpUrl.GetWindowText(strURL);
        if (strURL.IsEmpty()) {
            AfxMessageBox(_T("Fill INJECT URL first"));
            return;
        }

        std::string szURL = cs2utf8(strURL);
        InjectStreamConfig config;
        m_rtcEngine->addInjectStreamUrl(szURL.c_str(), config);
        injectUrl = szURL;
        addInjectStream = true;
        m_edtRtmpUrl.EnableWindow(FALSE);
        m_btnAddStream.SetWindowText(_T("Remove URL"));
    }
    EnableInjectWindow(FALSE);
}


void CAgoraRtmpInjectionDlg::OnBnClickedJoinchannel()
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
        if (0 == m_rtcEngine->joinChannel(APP_TOKEN, szChannelId.c_str(), "", 0)) {
            m_btnJoinChannel.EnableWindow(FALSE);
        }
    }
    else {
        if (0 == m_rtcEngine->leaveChannel()) {
            m_btnJoinChannel.EnableWindow(FALSE);
        }
    }
}

LRESULT CAgoraRtmpInjectionDlg::OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam)
{
    m_btnJoinChannel.EnableWindow(TRUE);
    joinChannel = true;
    m_btnJoinChannel.SetWindowText(_T("LeaveChannel"));
    EnableInjectWindow(TRUE);
    CString strInfo;
    strInfo.Format(_T("join channel success"));
    m_staInfo.SetWindowText(strInfo);
    return 0;
}

LRESULT CAgoraRtmpInjectionDlg::OnEIDLeaveChannel(WPARAM wParam, LPARAM lParam)
{
    m_btnJoinChannel.EnableWindow(FALSE);
    joinChannel = false;
    m_btnJoinChannel.SetWindowText(_T("JoinChannel"));
    CString strInfo;
    strInfo.Format(_T("leave channel"));
    m_staInfo.SetWindowText(strInfo);
    return 0;
}

LRESULT CAgoraRtmpInjectionDlg::OnEIDUserJoined(WPARAM wParam, LPARAM lParam)
{
    uid_t remoteUid = (uid_t)wParam;
    if (remoteUid == 666) {//inject stream
        VideoCanvas canvas;
        canvas.renderMode = RENDER_MODE_FIT;
        canvas.uid = remoteUid;
        canvas.view = m_injectVideoWnd.GetSafeHwnd();
        m_rtcEngine->setupRemoteVideo(canvas);

        CString strInfo;
        strInfo.Format(_T("%u joined"), remoteUid);
        m_staInfo.SetWindowText(strInfo);
    }
    return 0;
}

LRESULT CAgoraRtmpInjectionDlg::OnEIDUserOffline(WPARAM wParam, LPARAM lParam)
{
    uid_t remoteUid = (uid_t)wParam;
    if (remoteUid == 666) {//inject stream
        VideoCanvas canvas;
        canvas.uid = remoteUid;
        canvas.view = NULL;
        m_rtcEngine->setupRemoteVideo(canvas);
        CString strInfo;
        strInfo.Format(_T("%u offline, reason:%d"), remoteUid, lParam);
        m_staInfo.SetWindowText(strInfo);
    }
    return 0;
}


LRESULT CAgoraRtmpInjectionDlg::OnEIDStreamInjectedStatus(WPARAM wParam, LPARAM lParam)
{
    CString strInfo;
    switch ((INJECT_STREAM_STATUS)lParam)
    {
    case INJECT_STREAM_STATUS_START_SUCCESS:
        strInfo.Format(_T("外部视频流导入成功, err: %d。"), 0);
        break;
    case INJECT_STREAM_STATUS_START_ALREADY_EXISTS:
        strInfo.Format(_T("外部视频流已存在, err: %d。"), 1);
        break;
    case INJECT_STREAM_STATUS_START_UNAUTHORIZED:
        strInfo.Format(_T("外部视频流导入未经授权, err: %d。"), 2);
        break;
    case INJECT_STREAM_STATUS_START_TIMEDOUT:
        strInfo.Format(_T("导入外部视频流超时, err: %d。"), 3);
        break;

    case INJECT_STREAM_STATUS_START_FAILED:
        strInfo.Format(_T(" 外部视频流导入失败, err: %d。"), 4);
        break;
    case INJECT_STREAM_STATUS_STOP_SUCCESS:
        strInfo.Format(_T("外部视频流停止导入成功, err: %d。"), 5);
        break;
    case INJECT_STREAM_STATUS_STOP_NOT_FOUND:
        strInfo.Format(_T("未找到要停止导入的外部视频流, err: %d。"), 6);
        break;
    case INJECT_STREAM_STATUS_STOP_UNAUTHORIZED:
        strInfo.Format(_T("要停止导入的外部视频流未经授权, err: %d。"), 7);
        break;

    case INJECT_STREAM_STATUS_STOP_TIMEDOUT:
        strInfo.Format(_T("停止导入外部视频流超时, err: %d。"), 8);
        break;
    case INJECT_STREAM_STATUS_STOP_FAILED:
        strInfo.Format(_T("停止导入外部视频流失败, err: %d。"), 9);
        break;
    case INJECT_STREAM_STATUS_BROKEN:
        strInfo.Format(_T("导入的外部视频流被中断, err: %d。"), 10);
        break;
    default:
        break;
    }
   
    m_staInfo.SetWindowText(strInfo);
    EnableInjectWindow(TRUE);

    return 0;
}

void CAgoraRtmpInjectionDlg::OnDestroy()
{
    if (m_rtcEngine) {
        m_rtcEngine->stopPreview();
        m_rtcEngine->disableVideo();
        m_rtcEngine->release(true);
    }
    CDialogEx::OnDestroy();
}