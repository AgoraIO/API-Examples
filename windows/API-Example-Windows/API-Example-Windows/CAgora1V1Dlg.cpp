// CAgora1V1Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "API-Example-Windows.h"
#include "CAgora1V1Dlg.h"
#include "afxdialogex.h"

void CAgora1V1RtcEngineEventHandler::onJoinChannelSuccess(const char* channel, uid_t uid, int elapsed)
{
    if (m_hMsgHanlder) {
        ::PostMessage(m_hMsgHanlder, WM_MSGID(EID_JOINCHANNEL_SUCCESS), (WPARAM)uid, (LPARAM)elapsed);
    }
}

void CAgora1V1RtcEngineEventHandler::onUserJoined(uid_t uid, int elapsed) {
     if (m_hMsgHanlder) {
        ::PostMessage(m_hMsgHanlder, WM_MSGID(EID_USER_JOINED), (WPARAM)uid, (LPARAM)elapsed);
    }
}

void CAgora1V1RtcEngineEventHandler::onUserOffline(uid_t uid, USER_OFFLINE_REASON_TYPE reason)
{
    if (m_hMsgHanlder) {
        ::PostMessage(m_hMsgHanlder, WM_MSGID(EID_USER_OFFLINE), (WPARAM)uid, (LPARAM)reason);
    }
}

void CAgora1V1RtcEngineEventHandler::onLeaveChannel(const RtcStats& stats)
{
    if (m_hMsgHanlder) {
        ::PostMessage(m_hMsgHanlder, WM_MSGID(EID_LEAVE_CHANNEL), 0, 0);
    }
}
// CAgora1V1Dlg dialog

IMPLEMENT_DYNAMIC(CAgora1V1Dlg, CDialogEx)

CAgora1V1Dlg::CAgora1V1Dlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_JOINCHANNEL, pParent)
{

}

CAgora1V1Dlg::~CAgora1V1Dlg()
{
}

void CAgora1V1Dlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDIT_CHANNELNAME, m_edtChannelName);
    DDX_Control(pDX, IDC_STATIC_REMOTE, m_remoteVideoWnd);
    DDX_Control(pDX, IDC_STATIC_LOCALVIDEO, m_localVideoWnd);
    DDX_Control(pDX, ID_JOINCHANNEL, m_btnJoinChannel);
    DDX_Control(pDX, IDC_STATIC_INFO, m_staInfo);
}


BEGIN_MESSAGE_MAP(CAgora1V1Dlg, CDialogEx)
    ON_BN_CLICKED(ID_JOINCHANNEL, &CAgora1V1Dlg::OnBnClickedJoinchannel)
    ON_MESSAGE(WM_MSGID(EID_JOINCHANNEL_SUCCESS), &CAgora1V1Dlg::OnEIDJoinChannelSuccess)
    ON_MESSAGE(WM_MSGID(EID_LEAVE_CHANNEL), &CAgora1V1Dlg::OnEIDLeaveChannel)
    ON_MESSAGE(WM_MSGID(EID_USER_JOINED), &CAgora1V1Dlg::OnEIDUserJoined)
    ON_MESSAGE(WM_MSGID(EID_USER_OFFLINE), &CAgora1V1Dlg::OnEIDUserOffline)

    ON_WM_DESTROY()
END_MESSAGE_MAP()


// CAgora1V1Dlg message handlers

BOOL CAgora1V1Dlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // TODO:  Add extra initialization here
    m_rtcEngine = createAgoraRtcEngine();
    if (!m_rtcEngine) {
        m_staInfo.SetWindowText(_T("createAgoraRtcEngine failed"));
        return TRUE;
    }
    m_eventHandler1V1.SetMsgReceiver(m_hWnd);

    RtcEngineContext context;
    context.appId = APP_ID;
    context.eventHandler = &m_eventHandler1V1;
    m_rtcEngine->initialize(context);
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
    canvas.uid  = 0;
    canvas.view = m_localVideoWnd.GetSafeHwnd();
    m_rtcEngine->setupLocalVideo(canvas);
    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}

void CAgora1V1Dlg::OnBnClickedJoinchannel()
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

void CAgora1V1Dlg::OnDestroy()
{
    if (m_rtcEngine) {
        m_rtcEngine->stopPreview();
        m_rtcEngine->disableVideo();
        m_rtcEngine->release(true);
    }
    CDialogEx::OnDestroy();
}

LRESULT CAgora1V1Dlg::OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam)
{
    m_btnJoinChannel.EnableWindow(TRUE);
    joinChannel = true;
    m_btnJoinChannel.SetWindowText(_T("LeaveChannel"));
    
    CString strInfo;
    strInfo.Format(_T("join channel success"));
    m_staInfo.SetWindowText(strInfo);
    return 0;
}

LRESULT CAgora1V1Dlg::OnEIDLeaveChannel(WPARAM wParam, LPARAM lParam)
{
    m_btnJoinChannel.EnableWindow(TRUE);
    joinChannel = false;
    m_btnJoinChannel.SetWindowText(_T("JoinChannel"));

    CString strInfo;
    strInfo.Format(_T("leave channel"));
    m_staInfo.SetWindowText(strInfo);
    return 0;
}

LRESULT CAgora1V1Dlg::OnEIDUserJoined(WPARAM wParam, LPARAM lParam)
{
    uid_t remoteUid = (uid_t)wParam;
    VideoCanvas canvas;
    canvas.renderMode = RENDER_MODE_FIT;
    canvas.uid = remoteUid;
    canvas.view = m_remoteVideoWnd.GetSafeHwnd();
    m_rtcEngine->setupRemoteVideo(canvas);

    CString strInfo;
    strInfo.Format(_T("%u joined"), remoteUid);
    m_staInfo.SetWindowText(strInfo);
    return 0;
}

LRESULT CAgora1V1Dlg::OnEIDUserOffline(WPARAM wParam, LPARAM lParam)
{
    uid_t remoteUid = (uid_t)wParam;
    VideoCanvas canvas;
    canvas.uid  = remoteUid;
    canvas.view = NULL;
    m_rtcEngine->setupRemoteVideo(canvas);
    CString strInfo;
    strInfo.Format(_T("%u offline, reason:%d"), remoteUid, lParam);
    m_staInfo.SetWindowText(strInfo);
    return 0;
}
