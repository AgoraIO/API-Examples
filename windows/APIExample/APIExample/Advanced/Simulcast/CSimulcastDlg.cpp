// CSimulcastDlg.cpp : implementation file
//

#include "stdafx.h"
#include "APIExample.h"
#include "CSimulcastDlg.h"

void CSimulcastRtcEngineEventHandler::onJoinChannelSuccess(const char *channel, uid_t uid, int elapsed)
{
    if (m_hMsgHanlder) {
        ::PostMessage(m_hMsgHanlder, WM_MSGID(EID_JOINCHANNEL_SUCCESS), (WPARAM)uid, (LPARAM)elapsed);
    }
}

void CSimulcastRtcEngineEventHandler::onUserJoined(uid_t uid, int elapsed)
{
    if (m_hMsgHanlder) {
        ::PostMessage(m_hMsgHanlder, WM_MSGID(EID_USER_JOINED), (WPARAM)uid, (LPARAM)elapsed);
    }
}

void CSimulcastRtcEngineEventHandler::onUserOffline(uid_t uid, USER_OFFLINE_REASON_TYPE reason)
{
    if (m_hMsgHanlder) {
        ::PostMessage(m_hMsgHanlder, WM_MSGID(EID_USER_OFFLINE), (WPARAM)uid, (LPARAM)reason);
    }
}

void CSimulcastRtcEngineEventHandler::onLeaveChannel(const RtcStats &stats)
{
    if (m_hMsgHanlder) {
        ::PostMessage(m_hMsgHanlder, WM_MSGID(EID_LEAVE_CHANNEL), 0, 0);
    }
}

void CSimulcastRtcEngineEventHandler::onRemoteVideoStats(const RemoteVideoStats &stats)
{
    if (m_hMsgHanlder) {
        ::PostMessage(m_hMsgHanlder, WM_MSGID(EID_REMOTE_VIDEO_STATS), (WPARAM)&stats, 0);
    }
}

// CSimulcastDlg dialog
IMPLEMENT_DYNAMIC(CSimulcastDlg, CDialogEx)

CSimulcastDlg::CSimulcastDlg(CWnd *pParent /*=nullptr*/)
: CDialogEx(IDD_DIALOG_SIMULCAST, pParent)
{
}

CSimulcastDlg::~CSimulcastDlg()
{
}

void CSimulcastDlg::DoDataExchange(CDataExchange *pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_COMBO_ROLE, m_cmbRole);
    DDX_Control(pDX, IDC_STATIC_ROLE, m_staRole);
    DDX_Control(pDX, IDC_EDIT_CHANNELNAME, m_edtChannelName);
    DDX_Control(pDX, IDC_BUTTON_JOINCHANNEL, m_btnJoinChannel);
    DDX_Control(pDX, IDC_LIST_INFO_SIMULCAST, m_lstInfo);
    DDX_Control(pDX, IDC_STATIC_VIDEO, m_videoArea);
    DDX_Control(pDX, IDC_STATIC_CHANNELNAME, m_staChannelName);
    
    // Broadcaster layer checkboxes
    DDX_Control(pDX, IDC_CHECK_LAYER1, m_chkLayer1);
    DDX_Control(pDX, IDC_CHECK_LAYER2, m_chkLayer2);
    DDX_Control(pDX, IDC_CHECK_LAYER3, m_chkLayer3);
    DDX_Control(pDX, IDC_CHECK_LAYER4, m_chkLayer4);
    DDX_Control(pDX, IDC_STATIC_LAYER_TITLE, m_staLayerTitle);
    
    // Audience layer selection
    DDX_Control(pDX, IDC_COMBO_LAYER_SELECT, m_cmbLayerSelect);
    DDX_Control(pDX, IDC_STATIC_LAYER_SELECT, m_staLayerSelect);
}

BEGIN_MESSAGE_MAP(CSimulcastDlg, CDialogEx)
ON_BN_CLICKED(IDC_BUTTON_JOINCHANNEL, &CSimulcastDlg::OnBnClickedButtonJoinchannel)
ON_CBN_SELCHANGE(IDC_COMBO_ROLE, &CSimulcastDlg::OnSelchangeComboRole)
ON_BN_CLICKED(IDC_CHECK_LAYER1, &CSimulcastDlg::OnBnClickedCheckLayer1)
ON_BN_CLICKED(IDC_CHECK_LAYER2, &CSimulcastDlg::OnBnClickedCheckLayer2)
ON_BN_CLICKED(IDC_CHECK_LAYER3, &CSimulcastDlg::OnBnClickedCheckLayer3)
ON_BN_CLICKED(IDC_CHECK_LAYER4, &CSimulcastDlg::OnBnClickedCheckLayer4)
ON_CBN_SELCHANGE(IDC_COMBO_LAYER_SELECT, &CSimulcastDlg::OnSelchangeComboLayerSelect)
ON_MESSAGE(WM_MSGID(EID_JOINCHANNEL_SUCCESS), &CSimulcastDlg::OnEIDJoinChannelSuccess)
ON_MESSAGE(WM_MSGID(EID_LEAVE_CHANNEL), &CSimulcastDlg::OnEIDLeaveChannel)
ON_MESSAGE(WM_MSGID(EID_USER_JOINED), &CSimulcastDlg::OnEIDUserJoined)
ON_MESSAGE(WM_MSGID(EID_USER_OFFLINE), &CSimulcastDlg::OnEIDUserOffline)
ON_MESSAGE(WM_MSGID(EID_REMOTE_VIDEO_STATS), &CSimulcastDlg::OnEIDRemoteVideoStats)
ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

// CSimulcastDlg message handlers
BOOL CSimulcastDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    
    m_eventHandler.SetMsgReceiver(m_hWnd);
    
    InitCtrlText();
    CreateAllVideoWnds();
    
    // Initialize role combo box
    m_cmbRole.InsertString(0, agoraRoleBroadcaster);
    m_cmbRole.InsertString(1, agoraRoleAudience);
    m_cmbRole.SetCurSel(0);
    
    // Initialize layer selection combo box for audience
    m_cmbLayerSelect.InsertString(0, _T("Layer 1:720p30fps"));
    m_cmbLayerSelect.InsertString(1, _T("Layer 2:540p15fps"));
    m_cmbLayerSelect.InsertString(2, _T("Layer 3:360p15fps"));
    m_cmbLayerSelect.InsertString(3, _T("Layer 4:270p15fps"));
    m_cmbLayerSelect.SetCurSel(0);
    
    UpdateUIForRole();
    
    return TRUE;
}

void CSimulcastDlg::InitCtrlText()
{
    m_staRole.SetWindowText(commonCtrlClientRole);
    m_staChannelName.SetWindowText(commonCtrlChannel);
    m_btnJoinChannel.SetWindowText(commonCtrlJoinChannel);
    m_staLayerTitle.SetWindowText(_T("Simulcast Layers:"));
    m_staLayerSelect.SetWindowText(_T("Select Layer:"));
}

void CSimulcastDlg::CreateAllVideoWnds()
{
    for (int i = 0; i < VIDEO_COUNT_SIMULCAST; i++)
    {
        m_videoWnds[i].Create(NULL, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CRect(0, 0, 1, 1), this, IDC_BASEWND_VIDEO_SIMULCAST + i);
        m_videoWnds[i].ShowWindow(SW_SHOW);
    }
    
    ResetVideoView();
}

void CSimulcastDlg::ResetVideoView()
{
    m_videoArea.ShowWindow(SW_HIDE);
    
    CRect rcArea;
    m_videoArea.GetClientRect(&rcArea);
    
    int nViewWidth = rcArea.Width() / 2;
    int nViewHeight = rcArea.Height();
    
    for (int i = 0; i < VIDEO_COUNT_SIMULCAST; i++)
    {
        CRect rcVideoArea(i * nViewWidth, 0, (i + 1) * nViewWidth, nViewHeight);
        m_videoWnds[i].MoveWindow(&rcVideoArea, TRUE);
        m_videoWnds[i].SetUID(0);
        m_videoWnds[i].SetParent(this);
        m_videoWnds[i].ShowWindow(SW_SHOW);
        m_videoWnds[i].Invalidate();
    }
}

bool CSimulcastDlg::InitAgora()
{
    //create Agora RTC engine
    m_rtcEngine = createAgoraRtcEngine();
    if (!m_rtcEngine) {
        m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("createAgoraRtcEngine failed"));
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
    
    //set client role
    CLIENT_ROLE_TYPE role = m_cmbRole.GetCurSel() == 0 ? CLIENT_ROLE_BROADCASTER : CLIENT_ROLE_AUDIENCE;
    m_rtcEngine->setClientRole(role);
    
    // Setup video encoding configuration for simulcast
    VideoEncoderConfiguration config;
    config.dimensions.width = 1280;
    config.dimensions.height = 720;
    config.frameRate = FRAME_RATE_FPS_30;
    config.bitrate = STANDARD_BITRATE;
    config.orientationMode = ORIENTATION_MODE_ADAPTIVE;
    m_rtcEngine->setVideoEncoderConfiguration(config);
    
    return true;
}

void CSimulcastDlg::UnInitAgora()
{
    if (m_rtcEngine) {
        if (m_joinChannel)
            //leave channel
            m_joinChannel = !m_rtcEngine->leaveChannel();
        //stop preview in the engine.
        m_rtcEngine->stopPreview();
        m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("stopPreview"));
        //disable video in the engine.
        m_rtcEngine->disableVideo();
        m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("disableVideo"));
        //release engine.
        if (m_initialize) {
            m_rtcEngine->release(nullptr);
            m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("release"));
        }
        m_rtcEngine = NULL;
    }
}

void CSimulcastDlg::ResumeStatus()
{
    m_lstInfo.ResetContent();
    m_cmbRole.SetCurSel(0);
    m_joinChannel = false;
    m_initialize = false;
    UpdateUIForRole();
}

void CSimulcastDlg::RenderLocalVideo()
{
    if (m_rtcEngine) {
        //enable local video in the engine.
        m_rtcEngine->enableLocalVideo(true);
        //start preview in the engine.
        m_rtcEngine->startPreview();
        m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("startPreview"));
        VideoCanvas canvas;
        canvas.renderMode = media::base::RENDER_MODE_HIDDEN;
        canvas.uid = 0;
        canvas.view = m_videoWnds[0].GetSafeHwnd();
        //setup local video in the engine to canvas.
        m_rtcEngine->setupLocalVideo(canvas);
        m_videoWnds[0].SetUID(canvas.uid);
        CString strInfo;
        strInfo.Format(_T("setupLocalVideo uid: %u"), canvas.uid);
        m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
    }
}

void CSimulcastDlg::StopLocalVideo()
{
    if (m_rtcEngine) {
        //stop preview in the engine.
        m_rtcEngine->stopPreview();
        //disable local video in the engine.
        m_rtcEngine->enableLocalVideo(false);
        VideoCanvas canvas;
        canvas.view = NULL;
        canvas.uid = 0;
        //setup local video in the engine to canvas.
        m_rtcEngine->setupLocalVideo(canvas);
        m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("setupLocalVideo NULL"));
    }
}

void CSimulcastDlg::UpdateUIForRole()
{
    int role = m_cmbRole.GetCurSel();
    
    if (role == 0) { // Broadcaster
        // Show layer checkboxes
        m_chkLayer1.ShowWindow(SW_SHOW);
        m_chkLayer2.ShowWindow(SW_SHOW);
        m_chkLayer3.ShowWindow(SW_SHOW);
        m_chkLayer4.ShowWindow(SW_SHOW);
        m_staLayerTitle.ShowWindow(SW_SHOW);
        
        // Hide layer selection combo
        m_cmbLayerSelect.ShowWindow(SW_HIDE);
        m_staLayerSelect.ShowWindow(SW_HIDE);
        
        // Set checkbox labels
        m_chkLayer1.SetWindowText(_T("Layer 1:720p30fps"));
        m_chkLayer2.SetWindowText(_T("Layer 2:540p15fps"));
        m_chkLayer3.SetWindowText(_T("Layer 3:360p15fps"));
        m_chkLayer4.SetWindowText(_T("Layer 4:270p15fps"));
        
        // Default check first layer
        m_chkLayer1.SetCheck(BST_CHECKED);
    }
    else { // Audience
        // Hide layer checkboxes
        m_chkLayer1.ShowWindow(SW_HIDE);
        m_chkLayer2.ShowWindow(SW_HIDE);
        m_chkLayer3.ShowWindow(SW_HIDE);
        m_chkLayer4.ShowWindow(SW_HIDE);
        m_staLayerTitle.ShowWindow(SW_HIDE);
        
        // Show layer selection combo
        m_cmbLayerSelect.ShowWindow(SW_SHOW);
        m_staLayerSelect.ShowWindow(SW_SHOW);
    }
}

void CSimulcastDlg::OnBnClickedButtonJoinchannel()
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
        
        // set simulcast config
        if (m_cmbRole.GetCurSel() == 0) { // Broadcaster
            ConfigureSimulcast();
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
        }
    }
    m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
}

void CSimulcastDlg::OnSelchangeComboRole()
{
    int nSel = m_cmbRole.GetCurSel();
    if (m_rtcEngine != NULL)
        m_rtcEngine->setClientRole(nSel == 0 ? CLIENT_ROLE_BROADCASTER : CLIENT_ROLE_AUDIENCE);
    
    UpdateUIForRole();
    
    CString strInfo;
    strInfo.Format(_T("setClientRole changed to %s"), nSel == 0 ? _T("Broadcaster") : _T("Audience"));
    m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
}

int CSimulcastDlg::GetCheckedLayerCount()
{
    int count = 0;
    if (m_chkLayer1.GetCheck() == BST_CHECKED) count++;
    if (m_chkLayer2.GetCheck() == BST_CHECKED) count++;
    if (m_chkLayer3.GetCheck() == BST_CHECKED) count++;
    if (m_chkLayer4.GetCheck() == BST_CHECKED) count++;
    return count;
}

void CSimulcastDlg::OnBnClickedCheckLayer1()
{
    if (m_chkLayer1.GetCheck() == BST_CHECKED && GetCheckedLayerCount() > 3) {
        m_chkLayer1.SetCheck(BST_UNCHECKED);
        AfxMessageBox(_T("Maximum 3 simulcast layers can be enabled simultaneously"));
        return;
    }
    
    CString strInfo;
    strInfo.Format(_T("Layer 1 %s"), m_chkLayer1.GetCheck() ? _T("enabled") : _T("disabled"));
    m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
    
    // Update simulcast configuration when joined
    if (m_joinChannel) {
        ConfigureSimulcast();
    }
}

void CSimulcastDlg::OnBnClickedCheckLayer2()
{
    if (m_chkLayer2.GetCheck() == BST_CHECKED && GetCheckedLayerCount() > 3) {
        m_chkLayer2.SetCheck(BST_UNCHECKED);
        AfxMessageBox(_T("Maximum 3 simulcast layers can be enabled simultaneously"));
        return;
    }
    
    CString strInfo;
    strInfo.Format(_T("Layer 2 %s"), m_chkLayer2.GetCheck() ? _T("enabled") : _T("disabled"));
    m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
    
    // Update simulcast configuration when joined
    if (m_joinChannel) {
        ConfigureSimulcast();
    }
}

void CSimulcastDlg::OnBnClickedCheckLayer3()
{
    if (m_chkLayer3.GetCheck() == BST_CHECKED && GetCheckedLayerCount() > 3) {
        m_chkLayer3.SetCheck(BST_UNCHECKED);
        AfxMessageBox(_T("Maximum 3 simulcast layers can be enabled simultaneously"));
        return;
    }
    
    CString strInfo;
    strInfo.Format(_T("Layer 3 %s"), m_chkLayer3.GetCheck() ? _T("enabled") : _T("disabled"));
    m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
    
    // Update simulcast configuration when joined
    if (m_joinChannel) {
        ConfigureSimulcast();
    }
}

void CSimulcastDlg::OnBnClickedCheckLayer4()
{
    if (m_chkLayer4.GetCheck() == BST_CHECKED && GetCheckedLayerCount() > 3) {
        m_chkLayer4.SetCheck(BST_UNCHECKED);
        AfxMessageBox(_T("Maximum 3 simulcast layers can be enabled simultaneously"));
        return;
    }
    
    CString strInfo;
    strInfo.Format(_T("Layer 4 %s"), m_chkLayer4.GetCheck() ? _T("enabled") : _T("disabled"));
    m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
    
    // Update simulcast configuration when joined
    if (m_joinChannel) {
        ConfigureSimulcast();
    }
}

void CSimulcastDlg::OnSelchangeComboLayerSelect()
{
    int sel = m_cmbLayerSelect.GetCurSel();
    CString strInfo;
    strInfo.Format(_T("Selected layer: %d"), sel + 1);
    m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
    
    // For audience, set remote video stream type based on selected layer
    if (m_cmbRole.GetCurSel() == 1 && m_rtcEngine && m_joinChannel) { // Audience
        VIDEO_STREAM_TYPE streamType;
        switch (sel) {
            case 0: // Layer 1 (High)
                streamType = VIDEO_STREAM_LAYER_1;
                break;
            case 1: // Layer 2 (Medium) 
                streamType = VIDEO_STREAM_LAYER_2;
                break;
            case 2: // Layer 3 (Low)
                streamType = VIDEO_STREAM_LAYER_3;
                break;
            case 3: // Layer 4 (Ultra Low)
                streamType = VIDEO_STREAM_LAYER_4;
                break;
            default:
                streamType = VIDEO_STREAM_LAYER_1;
                break;
        }
        
        // Apply to all remote users
        for (int i = 1; i < VIDEO_COUNT_SIMULCAST; i++) {
            uid_t uid = m_videoWnds[i].GetUID();
            if (uid != 0) {
                m_rtcEngine->setRemoteVideoStreamType(uid, streamType);
            }
        }
    }
}

void CSimulcastDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
    CDialogEx::OnShowWindow(bShow, nStatus);
    if (bShow)//bShwo is true ,show window
    {
        RenderLocalVideo();
    }
    else {
        ResumeStatus();
    }
}

LRESULT CSimulcastDlg::OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam)
{
    m_joinChannel = true;
    m_btnJoinChannel.EnableWindow(TRUE);
    m_btnJoinChannel.SetWindowText(commonCtrlLeaveChannel);
    CString strInfo;
    strInfo.Format(_T("onJoinChannelSuccess: uid=%u"), wParam);
    m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
    m_lstInfo.SetCurSel(m_lstInfo.GetCount() - 1);
    
    //notify parent window
    ::PostMessage(GetParent()->GetSafeHwnd(), WM_MSGID(EID_JOINCHANNEL_SUCCESS), TRUE, 0);
    return 0;
}

LRESULT CSimulcastDlg::OnEIDLeaveChannel(WPARAM wParam, LPARAM lParam)
{
    m_joinChannel = false;
    m_btnJoinChannel.SetWindowText(commonCtrlJoinChannel);
    CString strInfo;
    strInfo.Format(_T("onLeaveChannel"));
    m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
    m_lstInfo.SetCurSel(m_lstInfo.GetCount() - 1);
    //notify parent window
    ::PostMessage(GetParent()->GetSafeHwnd(), WM_MSGID(EID_JOINCHANNEL_SUCCESS), FALSE, 0);
    return 0;
}

LRESULT CSimulcastDlg::OnEIDUserJoined(WPARAM wParam, LPARAM lParam)
{
    uid_t remoteUid = (uid_t)wParam;
    VideoCanvas canvas;
    canvas.renderMode = media::base::RENDER_MODE_HIDDEN;
    canvas.uid = remoteUid;
    canvas.view = m_videoWnds[1].GetSafeHwnd();
    //setup remote video in the engine to canvas.
    m_rtcEngine->setupRemoteVideo(canvas);
    m_videoWnds[1].SetUID(canvas.uid);
    CString strInfo;
    strInfo.Format(_T("onUserJoined: uid=%u"), remoteUid);
    m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
    m_lstInfo.SetCurSel(m_lstInfo.GetCount() - 1);
    return 0;
}

LRESULT CSimulcastDlg::OnEIDUserOffline(WPARAM wParam, LPARAM lParam)
{
    uid_t remoteUid = (uid_t)wParam;
    VideoCanvas canvas;
    canvas.view = NULL;
    canvas.uid = remoteUid;
    //setup remote video in the engine to canvas.
    m_rtcEngine->setupRemoteVideo(canvas);
    m_videoWnds[1].SetUID(0);
    m_videoWnds[1].Invalidate();
    CString strInfo;
    strInfo.Format(_T("onUserOffline: uid=%u"), remoteUid);
    m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
    m_lstInfo.SetCurSel(m_lstInfo.GetCount() - 1);
    return 0;
}

LRESULT CSimulcastDlg::OnEIDRemoteVideoStats(WPARAM wParam, LPARAM lParam)
{
    // RemoteVideoStats* p = reinterpret_cast<RemoteVideoStats*>(wParam);
    // if (p)
    // {
    //     CString strInfo = _T("===onRemoteVideoStats===");
    //     m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

    //     strInfo.Format(_T("uid:%u"), p->uid);
    //     m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
    //     strInfo.Format(_T("delay:%d"), p->delay);
    //     m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

    //     strInfo.Format(_T("width:%d"), p->width);
    //     m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
    //     strInfo.Format(_T("height:%d"), p->height);
    //     m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
    //     delete p;
    // }
    return 0;
}

BOOL CSimulcastDlg::PreTranslateMessage(MSG* pMsg)
{
    if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) {
        return TRUE;
    }
    return CDialogEx::PreTranslateMessage(pMsg);
}

void CSimulcastDlg::ConfigureSimulcast()
{
    if (!m_rtcEngine || !m_initialize) {
        return;
    }
    
    // Configure simulcast only for broadcaster
    if (m_cmbRole.GetCurSel() == 0) { // Broadcaster
        SimulcastConfig simulcastConfig;
        
        // Layer 1 (High quality)
        simulcastConfig.configs[0].dimensions.width = 1280;
        simulcastConfig.configs[0].dimensions.height = 720;
        simulcastConfig.configs[0].framerate = 30;
        simulcastConfig.configs[0].enable = m_chkLayer1.GetCheck() == BST_CHECKED;
        
        // Layer 2 (Medium quality)
        simulcastConfig.configs[1].dimensions.width = 960;
        simulcastConfig.configs[1].dimensions.height = 540;
        simulcastConfig.configs[1].framerate = 15;
        simulcastConfig.configs[1].enable = m_chkLayer2.GetCheck() == BST_CHECKED;
        
        // Layer 3 (Low quality)
        simulcastConfig.configs[2].dimensions.width = 640;
        simulcastConfig.configs[2].dimensions.height = 360;
        simulcastConfig.configs[2].framerate = 15;
        simulcastConfig.configs[2].enable = m_chkLayer3.GetCheck() == BST_CHECKED;
        
        // Layer 4 (Ultra low quality)
        simulcastConfig.configs[3].dimensions.width = 480;
        simulcastConfig.configs[3].dimensions.height = 270;
        simulcastConfig.configs[3].framerate = 15;
        simulcastConfig.configs[3].enable = m_chkLayer4.GetCheck() == BST_CHECKED;
        
        // Apply simulcast configuration
        int ret = m_rtcEngine->setSimulcastConfig(simulcastConfig);
        CString strInfo;
        if (ret == 0) {
            strInfo.Format(_T("Simulcast config applied successfully"));
        } else {
            strInfo.Format(_T("Simulcast config failed: %d"), ret);
        }
        m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
    }
}