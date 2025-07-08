// CMultipathDlg.cpp : implementation file

#include "stdafx.h"
#include "APIExample.h"
#include "CMultipathDlg.h"

void CMultipathRtcEngineEventHandler::onJoinChannelSuccess(const char *channel, uid_t uid, int elapsed)
{
    if (m_hMsgHanlder)
    {
        ::PostMessage(m_hMsgHanlder, WM_MSGID(EID_JOINCHANNEL_SUCCESS), (WPARAM)uid, (LPARAM)elapsed);
    }
}

void CMultipathRtcEngineEventHandler::onUserJoined(uid_t uid, int elapsed)
{
    if (m_hMsgHanlder)
    {
        ::PostMessage(m_hMsgHanlder, WM_MSGID(EID_USER_JOINED), (WPARAM)uid, (LPARAM)elapsed);
    }
}

void CMultipathRtcEngineEventHandler::onUserOffline(uid_t uid, USER_OFFLINE_REASON_TYPE reason)
{
    if (m_hMsgHanlder)
    {
        ::PostMessage(m_hMsgHanlder, WM_MSGID(EID_USER_OFFLINE), (WPARAM)uid, (LPARAM)reason);
    }
}

void CMultipathRtcEngineEventHandler::onLeaveChannel(const RtcStats &stats)
{
    if (m_hMsgHanlder)
    {
        ::PostMessage(m_hMsgHanlder, WM_MSGID(EID_LEAVE_CHANNEL), 0, 0);
    }
}

void CMultipathRtcEngineEventHandler::onMultipathStats(const MultipathStats& stats)
{
    if (m_hMsgHanlder)
    {
        MultipathStats* statsPtr = new MultipathStats(stats);
        ::PostMessage(m_hMsgHanlder, WM_MSGID(EID_MULTIPATH_STATS), (WPARAM)statsPtr, 0);
    }
}

void CMultipathRtcEngineEventHandler::onNetworkTypeChanged(NETWORK_TYPE type)
{
    if (m_hMsgHanlder)
    {
        ::PostMessage(m_hMsgHanlder, WM_MSGID(EID_NETWORK_TYPE_CHANGED), (WPARAM)type, 0);
    }
}

// CMultipathDlg dialog
IMPLEMENT_DYNAMIC(CMultipathDlg, CDialogEx)

CMultipathDlg::CMultipathDlg(CWnd *pParent /*=nullptr*/)
    : CDialogEx(IDD_DIALOG_MULTIPATH, pParent)
{
}

CMultipathDlg::~CMultipathDlg()
{
}

void CMultipathDlg::DoDataExchange(CDataExchange *pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_COMBO_ROLE, m_cmbRole);
    DDX_Control(pDX, IDC_STATIC_ROLE, m_staRole);
    DDX_Control(pDX, IDC_EDIT_CHANNELNAME, m_edtChannelName);
    DDX_Control(pDX, IDC_BUTTON_JOINCHANNEL, m_btnJoinChannel);
    DDX_Control(pDX, IDC_LIST_INFO_MULTIPATH, m_lstInfo);
    DDX_Control(pDX, IDC_STATIC_VIDEO, m_videoArea);
    DDX_Control(pDX, IDC_STATIC_CHANNELNAME, m_staChannelName);
    DDX_Control(pDX, IDC_COMBO_MULTIPATH_MODE, m_cmbMultipathMode);
    DDX_Control(pDX, IDC_STATIC_MULTIPATH_MODE, m_staMultipathMode);
    DDX_Control(pDX, IDC_CHECK_ENABLE_MULTIPATH, m_chkEnableMultipath);
    DDX_Control(pDX, IDC_STATIC_ENABLE_MULTIPATH, m_staEnableMultipath);
}

BEGIN_MESSAGE_MAP(CMultipathDlg, CDialogEx)
ON_BN_CLICKED(IDC_BUTTON_JOINCHANNEL, &CMultipathDlg::OnBnClickedButtonJoinchannel)
ON_CBN_SELCHANGE(IDC_COMBO_ROLE, &CMultipathDlg::OnSelchangeComboRole)
ON_CBN_SELCHANGE(IDC_COMBO_MULTIPATH_MODE, &CMultipathDlg::OnSelchangeComboMultipathMode)
ON_BN_CLICKED(IDC_CHECK_ENABLE_MULTIPATH, &CMultipathDlg::OnBnClickedCheckEnableMultipath)
ON_MESSAGE(WM_MSGID(EID_JOINCHANNEL_SUCCESS), &CMultipathDlg::OnEIDJoinChannelSuccess)
ON_MESSAGE(WM_MSGID(EID_LEAVE_CHANNEL), &CMultipathDlg::OnEIDLeaveChannel)
ON_MESSAGE(WM_MSGID(EID_USER_JOINED), &CMultipathDlg::OnEIDUserJoined)
ON_MESSAGE(WM_MSGID(EID_USER_OFFLINE), &CMultipathDlg::OnEIDUserOffline)
ON_MESSAGE(WM_MSGID(EID_MULTIPATH_STATS), &CMultipathDlg::OnEIDMultipathStats)
ON_MESSAGE(WM_MSGID(EID_NETWORK_TYPE_CHANGED), &CMultipathDlg::OnEIDNetworkTypeChanged)
ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

// CMultipathDlg message handlers
BOOL CMultipathDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    CreateAllVideoWnds();
    
    // Initialize role combo box
    m_cmbRole.InsertString(0, _T("Broadcaster"));
    m_cmbRole.InsertString(1, _T("Audience"));
    m_cmbRole.SetCurSel(0);
    
    // Initialize multipath mode combo box
    m_cmbMultipathMode.InsertString(0, _T("Duplicate"));
    m_cmbMultipathMode.InsertString(1, _T("Dynamic"));
    m_cmbMultipathMode.SetCurSel(0);
    
    // Initialize multipath related controls
    m_staEnableMultipath.SetWindowText(_T("Enable Multipath:"));
    m_chkEnableMultipath.SetCheck(BST_UNCHECKED);
    
    InitCtrlText();
    
    return TRUE;
}

void CMultipathDlg::InitCtrlText()
{
    m_staRole.SetWindowText(_T("Role:"));
    m_staChannelName.SetWindowText(_T("Channel:"));
    m_btnJoinChannel.SetWindowText(_T("Join Channel"));
    m_staMultipathMode.SetWindowText(_T("Multipath Mode:"));
}

void CMultipathDlg::CreateAllVideoWnds()
{
    for (int i = 0; i < VIDEO_COUNT_MULTIPATH; i++)
    {
        m_videoWnds[i].Create(NULL, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CRect(0, 0, 1, 1), this, IDC_BASEWND_VIDEO_MULTIPATH + i);
        m_videoWnds[i].SetUID(0);
    }
    
    // Position video windows
    RECT rcArea;
    m_videoArea.GetClientRect(&rcArea);
    int w = (rcArea.right - rcArea.left - 5) / 2;
    int h = rcArea.bottom - rcArea.top - 10;
    
    m_videoWnds[0].MoveWindow(5, 5, w, h, TRUE);
    m_videoWnds[1].MoveWindow(w + 10, 5, w, h, TRUE);
    
    for (int i = 0; i < VIDEO_COUNT_MULTIPATH; i++)
    {
        m_videoWnds[i].SetParent(&m_videoArea);
        m_videoWnds[i].ShowWindow(SW_SHOW);
    }
}

bool CMultipathDlg::InitAgora()
{
    // create Agora RTC engine
    m_rtcEngine = createAgoraRtcEngine();
    if (!m_rtcEngine)
    {
        m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("createAgoraRtcEngine failed"));
        return false;
    }
    
    // set message notify receiver window
    m_eventHandler.SetMsgReceiver(m_hWnd);

    RtcEngineContext context;
    std::string strAppID = GET_APP_ID;
    context.appId = strAppID.c_str();
    context.eventHandler = &m_eventHandler;
    context.channelProfile = CHANNEL_PROFILE_LIVE_BROADCASTING;
    
    // initialize the Agora RTC engine context.
    int ret = m_rtcEngine->initialize(context);
    if (ret != 0)
    {
        m_initialize = false;
        CString strInfo;
        strInfo.Format(_T("initialize failed: %d"), ret);
        m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
        return false;
    }
    else
        m_initialize = true;
        
    m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("initialize success"));
    
    // enable video and audio
    m_rtcEngine->enableVideo();
    m_rtcEngine->enableAudio();
    m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("enable video and audio"));
    
    return true;
}

void CMultipathDlg::UnInitAgora()
{
    if (m_rtcEngine)
    {
        if (m_joinChannel)
            m_rtcEngine->leaveChannel();
        m_rtcEngine->release(nullptr);
        m_rtcEngine = nullptr;
    }
}

void CMultipathDlg::ResumeStatus()
{
    m_lstInfo.ResetContent();
    m_lstUids.clear();
    m_joinChannel = false;
    m_btnJoinChannel.SetWindowText(_T("Join Channel"));
    m_btnJoinChannel.EnableWindow(TRUE);
}

void CMultipathDlg::RenderLocalVideo()
{
    if (m_rtcEngine)
    {
        m_rtcEngine->enableLocalVideo(true);
        m_rtcEngine->startPreview();
        VideoCanvas canvas;
        canvas.renderMode = media::base::RENDER_MODE_HIDDEN;
        canvas.uid = 0;
        canvas.view = m_videoWnds[0].GetSafeHwnd();
        m_rtcEngine->setupLocalVideo(canvas);
        m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("render local video"));
    }
}

void CMultipathDlg::StopLocalVideo()
{
    if (m_rtcEngine)
    {
        m_rtcEngine->enableLocalVideo(false);
        m_rtcEngine->stopPreview();
    }
}

void CMultipathDlg::ResetVideoView()
{
    VideoCanvas canvas;
    canvas.uid = 0;
    canvas.renderMode = media::base::RENDER_MODE_HIDDEN;
    m_rtcEngine->setupLocalVideo(canvas);
    canvas.view = m_videoWnds[0].GetSafeHwnd();
    m_rtcEngine->setupLocalVideo(canvas);
    
    if (m_videoWnds[1].GetUID() != 0)
    {
        VideoCanvas remoteCanvas;
        remoteCanvas.uid = m_videoWnds[1].GetUID();
        remoteCanvas.renderMode = media::base::RENDER_MODE_HIDDEN;
        m_rtcEngine->setupRemoteVideo(remoteCanvas);
        remoteCanvas.view = m_videoWnds[1].GetSafeHwnd();
        m_rtcEngine->setupRemoteVideo(remoteCanvas);
    }
}

void CMultipathDlg::OnBnClickedButtonJoinchannel()
{
    if (!m_rtcEngine || !m_initialize)
        return;
        
    if (!m_joinChannel)
    {
        CString strChannelName;
        m_edtChannelName.GetWindowText(strChannelName);
        if (strChannelName.IsEmpty())
        {
            AfxMessageBox(_T("Fill channel name first"));
            return;
        }

        // Set client role
        CLIENT_ROLE_TYPE role = m_cmbRole.GetCurSel() == 0 ? CLIENT_ROLE_BROADCASTER : CLIENT_ROLE_AUDIENCE;
        m_rtcEngine->setClientRole(role);

        // Configure ChannelMediaOptions
        ChannelMediaOptions options;
        options.autoSubscribeAudio = true;
        options.autoSubscribeVideo = true;
        options.publishMicrophoneTrack = (role == CLIENT_ROLE_BROADCASTER);
        options.publishCameraTrack = (role == CLIENT_ROLE_BROADCASTER);
        
        // Multipath configuration
        options.enableMultipath = (m_chkEnableMultipath.GetCheck() == BST_CHECKED);
        
        // Get multipath mode
        int modeIndex = m_cmbMultipathMode.GetCurSel();
        MultipathMode mode = (modeIndex == 0) ? Duplicate : Dynamic;
        options.uplinkMultipathMode = mode;
        options.downlinkMultipathMode = mode;
        
        // Set preferred path type
        options.preferMultipathType = WIFI;

        // Join channel
        std::string stdStrChannelName = cs2utf8(strChannelName);
        int ret = m_rtcEngine->joinChannel(NULL, stdStrChannelName.c_str(), 0, options);
        if (ret == 0)
        {
            m_btnJoinChannel.EnableWindow(FALSE);
        }
    }
    else
    {
        m_rtcEngine->leaveChannel();
    }
}

void CMultipathDlg::OnSelchangeComboRole()
{
    if (m_rtcEngine && m_initialize)
    {
        CLIENT_ROLE_TYPE role = m_cmbRole.GetCurSel() == 0 ? CLIENT_ROLE_BROADCASTER : CLIENT_ROLE_AUDIENCE;
        m_rtcEngine->setClientRole(role);
    }
}

void CMultipathDlg::OnSelchangeComboMultipathMode()
{
    int select_mode = m_cmbMultipathMode.GetCurSel();
    m_multipathModeStr = (select_mode == 0) ? "Duplicate" : "Dynamic";
    
    if (m_rtcEngine && m_initialize && m_joinChannel)
    {
        ChannelMediaOptions options;
        MultipathMode mode = (select_mode == 0) ? Duplicate : Dynamic;
        options.uplinkMultipathMode = mode;
        options.downlinkMultipathMode = mode;
        
        int ret = m_rtcEngine->updateChannelMediaOptions(options);
        
        CString strInfo;
        strInfo.Format(_T("Multipath mode changed to: %s, result: %d"), 
            CString(m_multipathModeStr.c_str()), ret);
        m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
        m_lstInfo.SetCurSel(m_lstInfo.GetCount() - 1);
    }
    else
    {
        CString strInfo;
        strInfo.Format(_T("Multipath mode changed to: %s"), select_mode == 0 ? _T("Duplicate") : _T("Dynamic"));
        m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
    }
}

void CMultipathDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
    CDialogEx::OnShowWindow(bShow, nStatus);
    if (bShow) // bShow is true, show window
    {
        RenderLocalVideo();
    }
    else {
        ResumeStatus();
    }
}

LRESULT CMultipathDlg::OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam)
{
    m_btnJoinChannel.EnableWindow(TRUE);
    m_joinChannel = true;
    m_btnJoinChannel.SetWindowText(_T("Leave Channel"));
    
    // Disable multipath mode combo box when in channel
    m_cmbMultipathMode.EnableWindow(FALSE);
    
    CString strInfo;
    strInfo.Format(_T("join channel success, uid=%u"), wParam);
    m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
    
    //notify parent window
    ::PostMessage(GetParent()->GetSafeHwnd(), WM_MSGID(EID_JOINCHANNEL_SUCCESS), TRUE, 0);
    
    return 0;
}

LRESULT CMultipathDlg::OnEIDLeaveChannel(WPARAM wParam, LPARAM lParam)
{
    m_btnJoinChannel.EnableWindow(TRUE);
    m_joinChannel = false;
    m_btnJoinChannel.SetWindowText(_T("Join Channel"));
    
    // Re-enable multipath mode combo box when leaving channel
    m_cmbMultipathMode.EnableWindow(TRUE);
    
    CString strInfo;
    strInfo.Format(_T("leave channel success"));
    m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
    m_lstUids.clear();
    m_videoWnds[1].SetUID(0);
    
    //notify parent window
    ::PostMessage(GetParent()->GetSafeHwnd(), WM_MSGID(EID_JOINCHANNEL_SUCCESS), FALSE, 0);
    
    return 0;
}

LRESULT CMultipathDlg::OnEIDUserJoined(WPARAM wParam, LPARAM lParam)
{
    CString strInfo;
    strInfo.Format(_T("%u joined"), wParam);
    m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
    m_lstInfo.SetCurSel(m_lstInfo.GetCount() - 1);
    
    m_lstUids.push_back(wParam);
    if (m_videoWnds[1].GetUID() == 0)
    {
        VideoCanvas canvas;
        canvas.uid = wParam;
        canvas.view = m_videoWnds[1].GetSafeHwnd();
        canvas.renderMode = media::base::RENDER_MODE_HIDDEN;
        m_rtcEngine->setupRemoteVideo(canvas);
        m_videoWnds[1].SetUID(wParam);
    }
    
    return 0;
}

LRESULT CMultipathDlg::OnEIDUserOffline(WPARAM wParam, LPARAM lParam)
{
    uid_t remoteUid = (uid_t)wParam;
    VideoCanvas canvas;
    canvas.uid = remoteUid;
    canvas.view = NULL;
    m_rtcEngine->setupRemoteVideo(canvas);
    
    CString strInfo;
    strInfo.Format(_T("%u offline, reason:%d"), remoteUid, lParam);
    m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
    m_lstInfo.SetCurSel(m_lstInfo.GetCount() - 1);
    
    if (m_videoWnds[1].GetUID() == remoteUid)
    {
        m_videoWnds[1].SetUID(0);
        m_videoWnds[1].Invalidate();
    }
    
    m_lstUids.remove(remoteUid);
    return 0;
}

LRESULT CMultipathDlg::OnEIDMultipathStats(WPARAM wParam, LPARAM lParam)
{
    MultipathStats* stats = (MultipathStats*)wParam;
    if (stats)
    {
        m_activePathNum = stats->activePathNum;
        
        CString strInfo;
        strInfo.Format(_T("Multipath Stats - Active Paths: %d"), m_activePathNum);
        m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
        m_lstInfo.SetCurSel(m_lstInfo.GetCount() - 1);
        
        delete stats;
    }
    return 0;
}

LRESULT CMultipathDlg::OnEIDNetworkTypeChanged(WPARAM wParam, LPARAM lParam)
{
    int type = (int)wParam;
    
    switch (type)
    {
    case NETWORK_TYPE_DISCONNECTED:
        m_networkStr = "disconnected";
        break;
    case NETWORK_TYPE_LAN:
        m_networkStr = "lan";
        break;
    case NETWORK_TYPE_WIFI:
        m_networkStr = "wifi";
        break;
    case NETWORK_TYPE_MOBILE_2G:
    case NETWORK_TYPE_MOBILE_3G:
    case NETWORK_TYPE_MOBILE_4G:
    case NETWORK_TYPE_MOBILE_5G:
        m_networkStr = "mobile";
        break;
    default:
        m_networkStr = "unknown";
        break;
    }
    
    CString strInfo;
    strInfo.Format(_T("Network Type Changed: %s"), CString(m_networkStr.c_str()));
    m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
    m_lstInfo.SetCurSel(m_lstInfo.GetCount() - 1);
    
    return 0;
}

void CMultipathDlg::OnBnClickedCheckEnableMultipath()
{
    BOOL bChecked = m_chkEnableMultipath.GetCheck();
    
    CString strInfo;
    strInfo.Format(_T("Multipath %s"), bChecked ? _T("enabled") : _T("disabled"));
    m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
    m_lstInfo.SetCurSel(m_lstInfo.GetCount() - 1);
    
    // If already in channel, update the channel media options
    if (m_rtcEngine && m_initialize && m_joinChannel)
    {
        ChannelMediaOptions options;
        options.enableMultipath = (bChecked == BST_CHECKED);
        
        // Get current multipath mode
        int modeIndex = m_cmbMultipathMode.GetCurSel();
        MultipathMode mode = (modeIndex == 0) ? Duplicate : Dynamic;
        options.uplinkMultipathMode = mode;
        options.downlinkMultipathMode = mode;
        options.preferMultipathType = WIFI;
        
        int ret = m_rtcEngine->updateChannelMediaOptions(options);
        
        CString strUpdateInfo;
        strUpdateInfo.Format(_T("Update multipath settings, result: %d"), ret);
        m_lstInfo.InsertString(m_lstInfo.GetCount(), strUpdateInfo);
        m_lstInfo.SetCurSel(m_lstInfo.GetCount() - 1);
    }
}

BOOL CMultipathDlg::PreTranslateMessage(MSG* pMsg)
{
    if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
    {
        return TRUE;
    }
    return CDialogEx::PreTranslateMessage(pMsg);
}
