// CAgoraRtmpStreamingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "API-Example-Windows.h"
#include "CAgoraRtmpStreamingDlg.h"
#include "afxdialogex.h"


void CAgoraRtmpStreamingRtcEngineEventHandler::onJoinChannelSuccess(const char* channel, uid_t uid, int elapsed)
{
    if (m_hMsgHanlder) {
        ::PostMessage(m_hMsgHanlder, WM_MSGID(EID_JOINCHANNEL_SUCCESS), (WPARAM)uid, (LPARAM)elapsed);
    }
}

void CAgoraRtmpStreamingRtcEngineEventHandler::onStreamInjectedStatus(const char* url, uid_t uid, int status)
{
    if (m_hMsgHanlder) {
        ::PostMessage(m_hMsgHanlder, WM_MSGID(EID_INJECT_STATUS), (WPARAM)uid, (LPARAM)status);
    }
}

void CAgoraRtmpStreamingRtcEngineEventHandler::onLeaveChannel(const RtcStats& stats)
{
    if (m_hMsgHanlder) {
        ::PostMessage(m_hMsgHanlder, WM_MSGID(EID_LEAVE_CHANNEL), 0, 0);
    }
}

void CAgoraRtmpStreamingRtcEngineEventHandler::onUserJoined(uid_t uid, int elapsed)
{
    if (m_hMsgHanlder) {
        ::PostMessage(m_hMsgHanlder, WM_MSGID(EID_USER_JOINED), (WPARAM)uid, (LPARAM)elapsed);
    }
}

void CAgoraRtmpStreamingRtcEngineEventHandler::onUserOffline(uid_t uid, USER_OFFLINE_REASON_TYPE reason)
{
    if (m_hMsgHanlder) {
        ::PostMessage(m_hMsgHanlder, WM_MSGID(EID_USER_OFFLINE), (WPARAM)uid, (LPARAM)reason);
    }
}

void CAgoraRtmpStreamingRtcEngineEventHandler::onRtmpStreamingStateChanged(const char *url, RTMP_STREAM_PUBLISH_STATE state, RTMP_STREAM_PUBLISH_ERROR errCode)
{
    if (m_hMsgHanlder) {
        PRtmpStreamStreamStateChanged rtmpState = new RtmpStreamStreamStateChanged;
        int len = strlen(url);
        rtmpState->url   = new char[len + 1];
        rtmpState->url[len] = 0;
        strcpy_s(rtmpState->url, len + 1, url);
        rtmpState->state = state;
        rtmpState->error = errCode;
        ::PostMessage(m_hMsgHanlder, WM_MSGID(EID_RTMP_STREAM_STATE_CHANGED), (WPARAM)rtmpState, 0);
    }
}
// CAgoraRtmpStreamingDlg dialog

IMPLEMENT_DYNAMIC(CAgoraRtmpStreamingDlg, CDialogEx)

CAgoraRtmpStreamingDlg::CAgoraRtmpStreamingDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_RTMP_PUBLISH, pParent)
{

}

CAgoraRtmpStreamingDlg::~CAgoraRtmpStreamingDlg()
{
}

void CAgoraRtmpStreamingDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDIT_RTMP_URL, m_edtRtmpUrl);
    DDX_Control(pDX, IDC_EDIT_CHANNELNAME, m_edtChannelName);
    DDX_Control(pDX, ID_JOINCHANNEL, m_btnJoinChannel);
    DDX_Control(pDX, IDC_BUTTON_ADDSTREAM, m_btnAddStream);
    DDX_Control(pDX, IDC_STATIC_INFO, m_staInfo);
    DDX_Control(pDX, IDC_STATIC_LOCALVIDEO, m_staLocalVideo);
    DDX_Control(pDX, IDC_COMBO1, m_cmbRtmpUrl);
    DDX_Control(pDX, IDC_BUTTON_REMOVESTREAM, m_btnRemoveStream);
}


BEGIN_MESSAGE_MAP(CAgoraRtmpStreamingDlg, CDialogEx)
    ON_MESSAGE(WM_MSGID(EID_JOINCHANNEL_SUCCESS), &CAgoraRtmpStreamingDlg::OnEIDJoinChannelSuccess)
    ON_MESSAGE(WM_MSGID(EID_LEAVE_CHANNEL), &CAgoraRtmpStreamingDlg::OnEIDLeaveChannel)

  //  ON_MESSAGE(WM_MSGID(EID_USER_JOINED), &CAgoraRtmpStreamingDlg::OnEIDUserJoined)
  //  ON_MESSAGE(WM_MSGID(EID_USER_OFFLINE), &CAgoraRtmpStreamingDlg::OnEIDUserOffline)
    ON_MESSAGE(WM_MSGID(EID_RTMP_STREAM_STATE_CHANGED), &CAgoraRtmpStreamingDlg::OnEIDRtmpStateChanged)

    ON_BN_CLICKED(ID_JOINCHANNEL, &CAgoraRtmpStreamingDlg::OnBnClickedJoinchannel)
    ON_BN_CLICKED(IDC_BUTTON_ADDSTREAM, &CAgoraRtmpStreamingDlg::OnBnClickedButtonAddstream)
    ON_BN_CLICKED(IDC_BUTTON_REMOVEALLSTREAM, &CAgoraRtmpStreamingDlg::OnBnClickedButtonRemoveallstream)
    ON_WM_DESTROY()
    ON_BN_CLICKED(IDC_BUTTON_REMOVESTREAM, &CAgoraRtmpStreamingDlg::OnBnClickedButtonRemovestream)
END_MESSAGE_MAP()


// CAgoraRtmpStreamingDlg message handlers

LRESULT CAgoraRtmpStreamingDlg::OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam)
{
    m_btnJoinChannel.EnableWindow(TRUE);
    m_btnAddStream.EnableWindow(TRUE);
    m_btnRemoveStream.EnableWindow(TRUE);
    joinChannel = true;
    m_btnJoinChannel.SetWindowText(_T("LeaveChannel"));
    CString strInfo;
    strInfo.Format(_T("join channel success"));
    m_staInfo.SetWindowText(strInfo);
    return 0;
}

LRESULT CAgoraRtmpStreamingDlg::OnEIDLeaveChannel(WPARAM wParam, LPARAM lParam)
{
    m_btnJoinChannel.EnableWindow(TRUE);
    joinChannel = false;
    m_btnJoinChannel.SetWindowText(_T("JoinChannel"));
    CString strInfo;
    strInfo.Format(_T("leave channel"));
    m_staInfo.SetWindowText(strInfo);

    m_btnRemoveStream.EnableWindow(FALSE);
    return 0;
}

/*LRESULT CAgoraRtmpStreamingDlg::OnEIDUserJoined(WPARAM wParam, LPARAM lParam)
{
    CString strInfo;
    strInfo.Format(_T("%u joined"), remoteUid);
    m_staInfo.SetWindowText(strInfo);
    return 0;
}

LRESULT CAgoraRtmpStreamingDlg::OnEIDUserOffline(WPARAM wParam, LPARAM lParam)
{
    uid_t remoteUid = (uid_t)wParam;
    CString strInfo;
    strInfo.Format(_T("%u offline, reason:%d"), remoteUid, lParam);
    m_staInfo.SetWindowText(strInfo);
    return 0;
}*/

LRESULT CAgoraRtmpStreamingDlg::OnEIDRtmpStateChanged(WPARAM wParam, LPARAM lParam)
{
    PRtmpStreamStreamStateChanged rtmpState = (PRtmpStreamStreamStateChanged)wParam;
    CString strInfo;
    m_btnRemoveStream.EnableWindow(TRUE);
    switch (rtmpState->state)
    {
    case RTMP_STREAM_PUBLISH_STATE_IDLE: 
    {
        strInfo.Format(_T("成功删除推流地址:%S。"), rtmpState->url);
        CString strUrl;
        strUrl.Format(_T("%S"), rtmpState->url);
        int sel = m_cmbRtmpUrl.GetCurSel();
        m_cmbRtmpUrl.DeleteString(sel);
        m_cmbRtmpUrl.ResetContent();
        if (m_cmbRtmpUrl.GetCount() > 0) {
            m_cmbRtmpUrl.SetCurSel(0);
            for (auto iter = m_urlSet.begin(); iter != m_urlSet.end(); ++iter)
                if (strUrl.Compare(*iter) == 0) {
                    m_urlSet.erase(iter);
                    break;
                }
        }

        if (bRemoveAll) {
            removeUrlCount++;
            if (removeUrlCount == m_urlSet.size()) {//remove all url when leave channel
                m_urlSet.clear();
                bRemoveAll = false;
                m_rtcEngine->leaveChannel();
            }
        }
        
    }
        break;
    case RTMP_STREAM_PUBLISH_STATE_CONNECTING:
    {
        strInfo.Format(_T("正在连接 Agora 推流服务器和 RTMP 服务器"));
    }
        break;
    case RTMP_STREAM_PUBLISH_STATE_RUNNING:
        strInfo.Format(_T("推流正在进行。"));
        if (rtmpState->error == RTMP_STREAM_PUBLISH_ERROR_OK) {
            strInfo.Format(_T("推流成功。"));
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
        strInfo.Format(_T("正在恢复推流。当 CDN 出现异常，或推流短暂中断时，SDK 会自动尝试恢复推流，并返回该状态。"));
        break;
    case RTMP_STREAM_PUBLISH_STATE_FAILURE:
    {
        switch (rtmpState->state)
        {
        case RTMP_STREAM_PUBLISH_ERROR_INVALID_ARGUMENT: 
        {
            strInfo = _T("参数无效。");
        }
            break;
        case RTMP_STREAM_PUBLISH_ERROR_ENCRYPTED_STREAM_NOT_ALLOWED:
        {
            strInfo = _T("推流已加密，不能推流。");
        }
        break;
        case RTMP_STREAM_PUBLISH_ERROR_CONNECTION_TIMEOUT:
        {
            strInfo = _T("推流超时未成功。可以重新推流。");
        }
        break;
        case RTMP_STREAM_PUBLISH_ERROR_INTERNAL_SERVER_ERROR:
        {
            strInfo = _T("推流服务器出现错误。请重新推流。");
        }
        break;
        case RTMP_STREAM_PUBLISH_ERROR_RTMP_SERVER_ERROR:
        {
            strInfo = _T("RTMP 服务器出现错误。");
        }
        break;
        case RTMP_STREAM_PUBLISH_ERROR_TOO_OFTEN:
        {
            strInfo = _T("推流请求过于频繁。");
        }
        break;
        case RTMP_STREAM_PUBLISH_ERROR_REACH_LIMIT:
        {
            strInfo = _T(" 单个主播的推流地址数目达到上线 10。请删掉一些不用的推流地址再增加推流地址。");
        }
        break;
        case RTMP_STREAM_PUBLISH_ERROR_NOT_AUTHORIZED:
        {
            strInfo = _T("主播操作不属于自己的流。");
        }
        break;
        case RTMP_STREAM_PUBLISH_ERROR_STREAM_NOT_FOUND:
        {
            strInfo = _T("服务器未找到这个流。");
        }
        break;
        case RTMP_STREAM_PUBLISH_ERROR_FORMAT_NOT_SUPPORTED:
        {
            strInfo = _T("推流地址格式有错误。请检查推流地址格式是否正确。");
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
    
   
    m_staInfo.SetWindowText(strInfo);
    delete[] rtmpState->url;
    rtmpState->url = NULL;
    delete[] rtmpState;
    rtmpState = NULL;
    return 0;
}

void CAgoraRtmpStreamingDlg::OnBnClickedJoinchannel()
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
        if (m_cmbRtmpUrl.GetCount() > 0) {
            m_btnAddStream.EnableWindow(FALSE);
            m_btnRemoveStream.EnableWindow(FALSE);
            m_btnJoinChannel.EnableWindow(FALSE);
            RemoveAllRtmpUrls();
        }
        else  if (0 == m_rtcEngine->leaveChannel()) {
            m_btnJoinChannel.EnableWindow(FALSE);
        }
    }
    return;
}

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
        m_staInfo.SetWindowText(strInfo);
        return;
    }
}


void CAgoraRtmpStreamingDlg::OnBnClickedButtonRemoveallstream()
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

BOOL CAgoraRtmpStreamingDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    m_btnAddStream.EnableWindow(FALSE);
    m_btnRemoveStream.EnableWindow(FALSE);
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

    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}


void CAgoraRtmpStreamingDlg::OnDestroy()
{
    CDialogEx::OnDestroy();

    if (m_initialize) {
        m_rtcEngine->stopPreview();
        m_rtcEngine->disableVideo();
    }
    if(m_rtcEngine)
        m_rtcEngine->release(true);
}

void CAgoraRtmpStreamingDlg::OnBnClickedButtonRemovestream()
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

void CAgoraRtmpStreamingDlg::RemoveAllRtmpUrls()
{
    bRemoveAll = true;
    CString strUrl;
    for (int i = 0; i < m_cmbRtmpUrl.GetCount(); ++i) {
        m_cmbRtmpUrl.GetLBText(i, strUrl);
        std::string szUrl = cs2utf8(strUrl);
        m_rtcEngine->removePublishStreamUrl(szUrl.c_str());
    }
    m_cmbRtmpUrl.Clear();
    m_cmbRtmpUrl.ResetContent();
}