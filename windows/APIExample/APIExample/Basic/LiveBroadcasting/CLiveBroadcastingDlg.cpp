// CLiveBroadcastingDlg.cpp : implementation file

#include "../../stdafx.h"
#include "../../APIExample.h"
#include "CLiveBroadcastingDlg.h"
#include "afxdialogex.h"

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
void CLiveBroadcastingRtcEngineEventHandler::onJoinChannelSuccess(const char* channel, uid_t uid, int elapsed)
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
    by the callback（ms).
*/
void CLiveBroadcastingRtcEngineEventHandler::onUserJoined(uid_t uid, int elapsed) {
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
void CLiveBroadcastingRtcEngineEventHandler::onUserOffline(uid_t uid, USER_OFFLINE_REASON_TYPE reason)
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
parametes:
    stats: Call statistics.
*/
void CLiveBroadcastingRtcEngineEventHandler::onLeaveChannel(const RtcStats& stats)
{
    if (m_hMsgHanlder) {
        ::PostMessage(m_hMsgHanlder, WM_MSGID(EID_LEAVE_CHANNEL), 0, 0);
    }
}
// CLiveBroadcastingDlg dialog
IMPLEMENT_DYNAMIC(CLiveBroadcastingDlg, CDialogEx)

CLiveBroadcastingDlg::CLiveBroadcastingDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_LIVEBROADCASTING, pParent)
{

}

CLiveBroadcastingDlg::~CLiveBroadcastingDlg()
{
}

void CLiveBroadcastingDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_COMBO_ROLE, m_cmbRole);
    DDX_Control(pDX, IDC_STATIC_ROLE, m_staRole);
    DDX_Control(pDX, IDC_EDIT_CHANNELNAME, m_edtChannelName);
    DDX_Control(pDX, IDC_BUTTON_JOINCHANNEL, m_btnJoinChannel);
    DDX_Control(pDX, IDC_LIST_INFO_BROADCASTING, m_lstInfo);
    DDX_Control(pDX, IDC_STATIC_VIDEO, m_videoArea);
    DDX_Control(pDX, IDC_COMBO_PERSONS, m_cmbPersons);
    DDX_Control(pDX, IDC_STATIC_PERSONS, m_staPersons);
    DDX_Control(pDX, IDC_STATIC_CHANNELNAME, m_staChannelName);
    DDX_Control(pDX, IDC_STATIC_DETAIL, m_staDetail);
}


BEGIN_MESSAGE_MAP(CLiveBroadcastingDlg, CDialogEx)
    ON_BN_CLICKED(IDC_BUTTON_JOINCHANNEL, &CLiveBroadcastingDlg::OnBnClickedButtonJoinchannel)
    ON_CBN_SELCHANGE(IDC_COMBO_PERSONS, &CLiveBroadcastingDlg::OnSelchangeComboPersons)
    ON_CBN_SELCHANGE(IDC_COMBO_ROLE, &CLiveBroadcastingDlg::OnSelchangeComboRole)
    ON_MESSAGE(WM_MSGID(EID_JOINCHANNEL_SUCCESS), &CLiveBroadcastingDlg::OnEIDJoinChannelSuccess)
    ON_MESSAGE(WM_MSGID(EID_LEAVE_CHANNEL), &CLiveBroadcastingDlg::OnEIDLeaveChannel)
    ON_MESSAGE(WM_MSGID(EID_USER_JOINED), &CLiveBroadcastingDlg::OnEIDUserJoined)
    ON_MESSAGE(WM_MSGID(EID_USER_OFFLINE), &CLiveBroadcastingDlg::OnEIDUserOffline)
    ON_WM_SHOWWINDOW()
    ON_LBN_SELCHANGE(IDC_LIST_INFO_BROADCASTING, &CLiveBroadcastingDlg::OnSelchangeListInfoBroadcasting)
    ON_STN_CLICKED(IDC_STATIC_VIDEO, &CLiveBroadcastingDlg::OnStnClickedStaticVideo)
END_MESSAGE_MAP()


// CLiveBroadcastingDlg message handlers
BOOL CLiveBroadcastingDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    InitCtrlText();
    CreateAllVideoWnds();
    // clientRole
    int i = 0;
    m_cmbRole.InsertString(i++, agoraRoleBroadcaster);
    m_cmbRole.InsertString(i++, agoraRoleAudience);
    m_cmbRole.SetCurSel(0);
    //
    i = 0;
    m_cmbPersons.InsertString(i++, _T("1V1"));
    m_cmbPersons.InsertString(i++, _T("1V3"));
    m_cmbPersons.InsertString(i++, _T("1V8"));
    m_cmbPersons.InsertString(i++, _T("1V15"));
    m_cmbPersons.SetCurSel(0);
    ShowVideoWnds();

    m_btnJoinChannel.EnableWindow(FALSE);
    m_cmbRole.EnableWindow(FALSE);
    return TRUE;
}


//set control text from config.
void CLiveBroadcastingDlg::InitCtrlText()
{
    m_staRole.SetWindowText(commonCtrlClientRole);
    m_staPersons.SetWindowText(liveCtrlPersons);
    m_staChannelName.SetWindowText(commonCtrlChannel);
    m_btnJoinChannel.SetWindowText(commonCtrlJoinChannel);
}

//create all video window to save m_videoWnds.
void CLiveBroadcastingDlg::CreateAllVideoWnds()
{
    for (int i = 0; i < VIDEO_COUNT; ++i) {
        m_videoWnds[i].Create(NULL, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CRect(0, 0, 1, 1), this, IDC_BASEWND_VIDEO + i);
        //set window background color.
        m_videoWnds[i].SetFaceColor(RGB(0x58, 0x58, 0x58));
    }
}

//show all video window from m_videoWnds.
void CLiveBroadcastingDlg::ShowVideoWnds()
{
    m_videoArea.ShowWindow(SW_HIDE);
    int row = 2;
    int col = 2;
    m_maxVideoCount = 4;
    switch (m_cmbPersons.GetCurSel()) {
    case PEOPLE_IN_CHANNEL_2: {
        row = 1;
        col = 2;
        m_maxVideoCount = 2;
    }
        break;
    case PEOPLE_IN_CHANNEL_4: {
        int row = 2;
        int col = 2;
        m_maxVideoCount = 4;
    }
        break;
    case PEOPLE_IN_CHANNEL_9: {
        row = 3;
        col = 3;
        m_maxVideoCount = 9;
    }
        break;
    case PEOPLE_IN_CHANNEL_16: {
        row = 4;
        col = 4;
        m_maxVideoCount = 16;
    }
        break;
    }
    

    RECT rcArea;
    m_videoArea.GetClientRect( &rcArea);
    int space = 1;
    
    int w = (rcArea.right -rcArea.left - space * (col - 1)) / col;
    int h = (rcArea.bottom - rcArea.top - space * (row - 1)) / row;
    
    for (int r = 0; r < row; r++) {
        for (int c = 0; c < col; c++) {
            int x = rcArea.left + (w + space) * c;
            int y = rcArea.top + (h + space) * r;
            int nIndex = r * col + c;
            m_videoWnds[nIndex].MoveWindow(x, y, w, h, TRUE);
            m_videoWnds[nIndex].ShowWindow(SW_SHOW);
            m_videoWnds[nIndex].SetParent(this);

            if (!m_videoWnds[nIndex].IsWindowVisible()) {
                m_videoWnds[nIndex].ShowWindow(SW_SHOW);
            }
        }
    }

    for (int i = m_maxVideoCount; i < VIDEO_COUNT; i++) {
        m_videoWnds[i].ShowWindow(0);
        if (m_videoWnds[i].GetUID() != 0) {
            VideoCanvas canvas;
            canvas.uid = m_videoWnds[i].GetUID();
            canvas.view = m_videoWnds[i].GetSafeHwnd();
            m_rtcEngine->setupRemoteVideo(canvas);
        }
    }
}
//Initialize the Agora SDK
bool CLiveBroadcastingDlg::InitAgora()
{
    //create Agora RTC engine
    m_rtcEngine = createAgoraRtcEngine();
    if (!m_rtcEngine) {
        m_lstInfo.InsertString(m_lstInfo.GetCount()-1, _T("createAgoraRtcEngine failed"));
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
    m_cmbRole.EnableWindow(TRUE);
    return true;
}

//UnInitialize the Agora SDK
void CLiveBroadcastingDlg::UnInitAgora()
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
void CLiveBroadcastingDlg::RenderLocalVideo()
{
    if (m_rtcEngine) {
        //start preview in the engine.
        m_rtcEngine->startPreview();
        VideoCanvas canvas;
        canvas.renderMode = RENDER_MODE_FIT;
        canvas.uid = 0;
        canvas.view = m_videoWnds[0].GetSafeHwnd();
        //setup local video in the engine to the canvas. 
        m_rtcEngine->setupLocalVideo(canvas);
        m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("render local video"));
    }
}


void CLiveBroadcastingDlg::OnSelchangeComboPersons()
{
    int index = m_cmbPersons.GetCurSel();
    ShowVideoWnds();
}


void CLiveBroadcastingDlg::OnSelchangeComboRole()
{
    if (m_rtcEngine) {
        m_rtcEngine->setClientRole(CLIENT_ROLE_TYPE(m_cmbRole.GetCurSel() + 1));

        m_lstInfo.InsertString(m_lstInfo.GetCount(), m_cmbRole.GetCurSel() == 0 ? _T("setClientRole broadcaster"): _T("setClientRole Audience"));
    }
}

void CLiveBroadcastingDlg::OnBnClickedButtonJoinchannel()
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
        if (0 == m_rtcEngine->leaveChannel()) {
            strInfo.Format(_T("leave channel %s"), getCurrentTime());
            m_btnJoinChannel.EnableWindow(FALSE);
        }
    }

    m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
}



void CLiveBroadcastingDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
    CDialogEx::OnShowWindow(bShow, nStatus);

    if (bShow) {
        RenderLocalVideo();
    }
}

LRESULT CLiveBroadcastingDlg::OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam)
{
    m_btnJoinChannel.EnableWindow(TRUE);
    joinChannel = true;
    m_btnJoinChannel.SetWindowText(_T("LeaveChannel"));

    CString strInfo;
    strInfo.Format(_T("%s:join success, uid=%u"), getCurrentTime(), wParam);
    m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

    m_videoWnds[0].SetUID(wParam);
    m_lstUids.push_back(wParam);

    //notify parent window
    ::PostMessage(GetParent()->GetSafeHwnd(), WM_MSGID(EID_JOINCHANNEL_SUCCESS), TRUE, 0);
    return 0;
}

LRESULT CLiveBroadcastingDlg::OnEIDLeaveChannel(WPARAM wParam, LPARAM lParam)
{
    m_btnJoinChannel.EnableWindow(TRUE);
    joinChannel = false;
    m_btnJoinChannel.SetWindowText(_T("JoinChannel"));

    CString strInfo;
    strInfo.Format(_T("leave channel success %s"), getCurrentTime());
    m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
    m_lstUids.clear();
    for (int i = 0; i < m_maxVideoCount; i++) {
        m_videoWnds[i].SetUID(0);
    }

    //notify parent window
    ::PostMessage(GetParent()->GetSafeHwnd(), WM_MSGID(EID_JOINCHANNEL_SUCCESS), FALSE, 0);
    return 0;
}

LRESULT CLiveBroadcastingDlg::OnEIDUserJoined(WPARAM wParam, LPARAM lParam)
{
    if (m_lstUids.size() == m_maxVideoCount)
        return 0;
    CString strInfo;
    strInfo.Format(_T("%u joined"), wParam);
    m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

    m_lstUids.push_back(wParam);
    for (int i = 0; i < m_maxVideoCount; i++) {
        if (m_videoWnds[i].GetUID() == 0) {
            VideoCanvas canvas;
            canvas.uid  = wParam;
            canvas.view = m_videoWnds[i].GetSafeHwnd();
            canvas.renderMode = RENDER_MODE_FIT;
            //setup remote video in engine to the canvas.
            m_rtcEngine->setupRemoteVideo(canvas);
            break;
        }
    }
    return 0;
}

LRESULT CLiveBroadcastingDlg::OnEIDUserOffline(WPARAM wParam, LPARAM lParam)
{
    uid_t remoteUid = (uid_t)wParam;
    VideoCanvas canvas;
    canvas.uid = remoteUid;
    canvas.view = NULL;
    m_rtcEngine->setupRemoteVideo(canvas);
    CString strInfo;
    strInfo.Format(_T("%u offline, reason:%d"), remoteUid, lParam);
    m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

    for (int i = 0; i < m_maxVideoCount; i++){
        if (m_videoWnds[i].GetUID() == remoteUid) {
            m_videoWnds[i].SetUID(0);
            m_videoWnds[i].Invalidate();
            break;
        }
    }

    for (auto iter = m_lstUids.begin();
        iter != m_lstUids.end(); iter++){
        if (*iter == remoteUid) {
            m_lstUids.erase(iter);
            break;
        }
    }
    return 0;
}


void CLiveBroadcastingDlg::OnSelchangeListInfoBroadcasting()
{

    int sel = m_lstInfo.GetCurSel();
    CString strDetail;
    m_lstInfo.GetText(sel, strDetail);
    m_staDetail.SetWindowText(strDetail);
}


void CLiveBroadcastingDlg::OnStnClickedStaticVideo()
{
    // TODO: 在此添加控件通知处理程序代码
}
