#include "stdafx.h"
#include "APIExample.h"
#include "TransparentBgDialog.h"

void CTransparentBgEventHandler::onJoinChannelSuccess(const char* channel, uid_t uid, int elapsed)
{
    if (m_hMsgHanlder) {
        ::PostMessage(m_hMsgHanlder, WM_MSGID(EID_JOINCHANNEL_SUCCESS), (WPARAM)uid, (LPARAM)elapsed);
    }
}

void CTransparentBgEventHandler::onUserJoined(uid_t uid, int elapsed) {
    if (m_hMsgHanlder) {
        ::PostMessage(m_hMsgHanlder, WM_MSGID(EID_USER_JOINED), (WPARAM)uid, (LPARAM)elapsed);
    }
}

void CTransparentBgEventHandler::onUserOffline(uid_t uid, USER_OFFLINE_REASON_TYPE reason)
{
    if (m_hMsgHanlder) {
        ::PostMessage(m_hMsgHanlder, WM_MSGID(EID_USER_OFFLINE), (WPARAM)uid, (LPARAM)reason);
    }
}

void CTransparentBgEventHandler::onLeaveChannel(const RtcStats& stats)
{
    if (m_hMsgHanlder) {
        ::PostMessage(m_hMsgHanlder, WM_MSGID(EID_LEAVE_CHANNEL), 0, 0);
    }
}


void CTransparentBgEventHandler::onLocalVideoStats(VIDEO_SOURCE_TYPE source, const LocalVideoStats& stats)
{
	if (m_hMsgHanlder && report) {
		LocalVideoStats* s = new LocalVideoStats;
		*s = stats;
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_LOCAL_VIDEO_STATS), (WPARAM)s, 0);
	}
}

IMPLEMENT_DYNAMIC(CTransparentBgDlg, CDialogEx)

CTransparentBgDlg::CTransparentBgDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_TRANSPARENT_BG, pParent)
{

}

CTransparentBgDlg::~CTransparentBgDlg()
{
}

void CTransparentBgDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_STATIC_CHANNELNAME, m_staticChannelName);
    DDX_Control(pDX, IDC_BUTTON_JOINCHANNEL, m_bnJoinChannel);
    DDX_Control(pDX, IDC_EDIT_CHANNELNAME, m_editChannel);
    DDX_Control(pDX, IDC_STATIC_VIDEO, m_staticVideo);
    DDX_Control(pDX, IDC_STATIC_VIDEO_LEFT, m_staticVideoLeft);
    DDX_Control(pDX, IDC_STATIC_VIDEO_RIGHT, m_staticVideoRight);
    DDX_Control(pDX, IDC_LIST_INFO_BROADCASTING, m_listInfo);
}


BEGIN_MESSAGE_MAP(CTransparentBgDlg, CDialogEx)

    ON_MESSAGE(WM_MSGID(EID_JOINCHANNEL_SUCCESS), &CTransparentBgDlg::OnEIDJoinChannelSuccess)
    ON_MESSAGE(WM_MSGID(EID_ERROR), &CTransparentBgDlg::OnEIDError)
    ON_MESSAGE(WM_MSGID(EID_LEAVE_CHANNEL), &CTransparentBgDlg::OnEIDLeaveChannel)
    ON_MESSAGE(WM_MSGID(EID_USER_JOINED), &CTransparentBgDlg::OnEIDUserJoined)
    ON_MESSAGE(WM_MSGID(EID_USER_OFFLINE), &CTransparentBgDlg::OnEIDUserOffline)

	ON_MESSAGE(WM_MSGID(EID_LOCAL_AUDIO_STATS), &CTransparentBgDlg::onEIDLocalAudioStats)
	ON_MESSAGE(WM_MSGID(EID_REMOTE_AUDIO_STATS), &CTransparentBgDlg::onEIDRemoteAudioStats)
	ON_MESSAGE(WM_MSGID(EID_LOCAL_VIDEO_STATS), &CTransparentBgDlg::onEIDLocalVideoStats)
	ON_MESSAGE(WM_MSGID(EID_REMOTE_VIDEO_STATS), &CTransparentBgDlg::onEIDRemoteVideoStats)

    ON_WM_SHOWWINDOW()
   
    ON_BN_CLICKED(IDC_BUTTON_JOINCHANNEL, &CTransparentBgDlg::OnBnClickedButtonJoinchannel)
    ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// CTransparentBgDlg message handlers
BOOL CTransparentBgDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    InitCtrlText();
    return TRUE;
}


//set control text from config.
void CTransparentBgDlg::InitCtrlText()
{
    m_staticChannelName.SetWindowText(commonCtrlChannel);
    m_bnJoinChannel.SetWindowText(commonCtrlJoinChannel);
}

int CTransparentBgDlg::JoinChannel(const char* channel)
{
    VideoEncoderConfiguration config;
    m_rtcEngine->setVideoEncoderConfiguration(config);
    std::string szChannelId = channel;

    ChannelMediaOptions options;
    options.channelProfile = CHANNEL_PROFILE_LIVE_BROADCASTING;
    options.clientRoleType = CLIENT_ROLE_BROADCASTER;
    options.autoSubscribeAudio = false;
    options.autoSubscribeVideo = true;
    options.publishMicrophoneTrack = false;
    options.publishCameraTrack = true;
    //join channel in the engine.
    int ret = m_rtcEngine->joinChannel(APP_TOKEN, szChannelId.c_str(), m_uid, options);
    return ret;
}

int CTransparentBgDlg::LeaveChannel()
{
    if(!m_joinChannel)
	{
		return 0;
    };
    int ret = m_rtcEngine->leaveChannel();
    m_joinChannel = false;
    m_remoteId = 0;
    return ret;
}

//Initialize the Agora SDK
bool CTransparentBgDlg::InitAgora()
{
    //create Agora RTC engine
    m_rtcEngine = createAgoraRtcEngine();
    if (!m_rtcEngine) {
        m_listInfo.InsertString(m_listInfo.GetCount(), _T("createAgoraRtcEngine failed"));
        return false;
    }
    //set message notify receiver window
    m_eventHandler.SetMsgReceiver(m_hWnd);

    RtcEngineContext context;
	std::string strAppID = GET_APP_ID;
	context.appId = strAppID.c_str();
    context.eventHandler = &m_eventHandler;
	//set channel profile in the engine to the CHANNEL_PROFILE_LIVE_BROADCASTING.
	context.channelProfile = CHANNEL_PROFILE_LIVE_BROADCASTING;
    //initialize the Agora RTC engine context.
    int ret = m_rtcEngine->initialize(context);
    if (ret != 0) {
        m_initialize = false;
        CString strInfo;
        strInfo.Format(_T("initialize failed: %d"), ret);
        m_listInfo.InsertString(m_listInfo.GetCount(), strInfo);
        return false;
    }
    else 
    {
        m_initialize = true;
        m_listInfo.InsertString(m_listInfo.GetCount(), _T("createAgoraRtcEngine success"));
    }
    m_rtcEngine->enableVideo();
	m_rtcEngine->enableAudio();
    return true;
}

//UnInitialize the Agora SDK
void CTransparentBgDlg::UnInitAgora()
{
    if (m_rtcEngine) {
		if(m_joinChannel)
		m_rtcEngine->leaveChannel();
       // m_rtcEngine->stopPreview();
        m_rtcEngine->disableVideo();
		if (m_initialize) {
			m_rtcEngine->release(true);
		}
        m_rtcEngine = NULL;
    }
}


LRESULT CTransparentBgDlg::OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam)
{
	CString strInfo;
	strInfo.Format(TEXT("self join success, wParam=%u"), wParam);
	m_listInfo.InsertString(m_listInfo.GetCount(), strInfo);
    //notify parent window
    ::PostMessage(GetParent()->GetSafeHwnd(), WM_MSGID(EID_JOINCHANNEL_SUCCESS), TRUE, 0);
    return 0;
}

LRESULT CTransparentBgDlg::OnEIDError(WPARAM wParam, LPARAM lParam)
{
    CString strInfo;
    char* message = (char*)wParam;
    int code = lParam;
    strInfo.Format(TEXT("Error >> code=%d, message=%s"), code, utf82cs(std::string(message)));
    m_listInfo.InsertString(m_listInfo.GetCount(), strInfo);
    delete message;
    return 0;
}

LRESULT CTransparentBgDlg::OnEIDLeaveChannel(WPARAM wParam, LPARAM lParam)
{
    m_uid = 0;
    CString strInfo;
    strInfo.Format(TEXT("leave channel success"));
    m_listInfo.InsertString(m_listInfo.GetCount(), strInfo);
    //notify parent window
    ::PostMessage(GetParent()->GetSafeHwnd(), WM_MSGID(EID_JOINCHANNEL_SUCCESS), FALSE, 0);
    return 0;
}

LRESULT CTransparentBgDlg::OnEIDUserJoined(WPARAM wParam, LPARAM lParam)
{
    if (m_remoteId!=0)
    {
        m_listInfo.InsertString(m_listInfo.GetCount(), _T("user joined already"));
        return 0;
    }
    CString strInfo;
	strInfo.Format(TEXT("user %u joined"), wParam);
	m_listInfo.InsertString(m_listInfo.GetCount(), strInfo);

	VideoCanvas canvas;
    canvas.enableAlphaMask = true;
	canvas.uid = wParam;
	canvas.view = m_staticVideoRight.GetSafeHwnd();
	canvas.renderMode = media::base::RENDER_MODE_FIT;
	m_rtcEngine->setupRemoteVideo(canvas);
    return 0;
}

LRESULT CTransparentBgDlg::OnEIDUserOffline(WPARAM wParam, LPARAM lParam)
{
    m_remoteId = 0;
    uid_t remoteUid = (uid_t)wParam;
    VideoCanvas canvas;
    canvas.uid = remoteUid;
    canvas.view = NULL;
    m_rtcEngine->setupRemoteVideo(canvas);
    CString strInfo;
    strInfo.Format(TEXT("%u offline, reason:%d"), remoteUid, lParam);
    m_listInfo.InsertString(m_listInfo.GetCount(), strInfo);
    return 0;
}

BOOL CTransparentBgDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) {
		return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}



LRESULT CTransparentBgDlg::onEIDLocalAudioStats(WPARAM wParam, LPARAM lParam) {
	LocalAudioStats* stats = (LocalAudioStats*)wParam;

	//m_videoWnds[0].SetAudioStatsInfo(stats->sentBitrate, stats->txPacketLossRate);
	
	if (stats) {
		delete stats;
		stats = nullptr;
	}
	return 0;
}

LRESULT CTransparentBgDlg::onEIDRemoteAudioStats(WPARAM wParam, LPARAM lParam) {
	RemoteAudioStats* stats = (RemoteAudioStats*)wParam;

	/*for (int i = 0; i < VIDEO_COUNT; i++)
	{
		if (m_videoWnds[i].GetUID() == stats->uid) {
			m_videoWnds[i].SetAudioStatsInfo(stats->receivedBitrate, stats->audioLossRate, stats->jitterBufferDelay);
			break;
		}
	}*/

	if (stats) {
		delete stats;
		stats = nullptr;
	}
	return 0;
}


LRESULT CTransparentBgDlg::onEIDLocalVideoStats(WPARAM wParam, LPARAM lParam) {
	LocalVideoStats* stats = (LocalVideoStats*)wParam;

	/*m_videoWnds[0].SetVideoStatsInfo(stats->encodedFrameWidth, stats->encodedFrameHeight, stats->sentFrameRate, stats->sentBitrate, stats->txPacketLossRate);

	if (stats) {
		delete stats;
		stats = nullptr;
	}*/

	
	return 0;
}

LRESULT CTransparentBgDlg::onEIDRemoteVideoStats(WPARAM wParam, LPARAM lParam) {
	RemoteVideoStats* stats = (RemoteVideoStats*)wParam;
//	CString strInfo = _T("===onRemoteVideoStats===");
//    m_listInfo.InsertString(m_listInfo.GetCount(), strInfo);
	return 0;
}



void CTransparentBgDlg::OnBnClickedButtonJoinchannel()
{
    CString info;
    if (!m_joinChannel) {
        CString strChannelName;
        m_editChannel.GetWindowText(strChannelName);
        if (strChannelName.IsEmpty()) {
            MessageBox(_T("频道号不能为空"));
            return;
        }

        std::string szChannelName = cs2utf8(strChannelName);
        int ret = JoinChannel(szChannelName.c_str());
        if (ret == 0) {
            m_joinChannel = true;
            m_bnJoinChannel.SetWindowText(commonCtrlLeaveChannel);
            m_rtcEngine->enableVideo();
            m_rtcEngine->startPreview();
            VideoCanvas canvas;
            canvas.renderMode = media::base::RENDER_MODE_FIT;
            canvas.uid = m_uid;
            canvas.view = m_staticVideoLeft.GetSafeHwnd();
            m_rtcEngine->setupLocalVideo(canvas);
        }
        else {
            const char* des = getAgoraSdkErrorDescription(ret);
            info.Format(TEXT("join channel failed >> code=%d, des=%s"), ret, utf82cs(std::string(des)));
            m_listInfo.InsertString(m_listInfo.GetCount(), info);
        }
    }
    else {
        int ret = LeaveChannel();
        if (0 == ret) {
            m_bnJoinChannel.SetWindowText(commonCtrlJoinChannel);
        }
        else {
            const char* des = getAgoraSdkErrorDescription(ret);
            info.Format(TEXT("leave channel failed >> code=%d, des=%s"), ret, utf82cs(std::string(des)));
            m_listInfo.InsertString(m_listInfo.GetCount(), info);
        }
    }

}


void CTransparentBgDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
    CDialogEx::OnShowWindow(bShow, nStatus);

   if (bShow) {
        //resume
	}
	else
	{
       //pause
	}
}
