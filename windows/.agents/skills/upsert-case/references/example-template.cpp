#include "stdafx.h"
#include "APIExample.h"
#include "C<ExampleName>Dlg.h"
#include "CConfig.h"

IMPLEMENT_DYNAMIC(C<ExampleName>Dlg, CDialogEx)

// Event Handler Implementation
void C<ExampleName>RtcEngineEventHandler::onJoinChannelSuccess(const char* channel, uid_t uid, int elapsed) {
    if (m_hMsgHandler) {
        ::PostMessage(m_hMsgHandler, WM_MSGID(EID_JOIN_CHANNEL_SUCCESS), (WPARAM)uid, (LPARAM)elapsed);
    }
}

void C<ExampleName>RtcEngineEventHandler::onUserJoined(uid_t uid, int elapsed) {
    if (m_hMsgHandler) {
        ::PostMessage(m_hMsgHandler, WM_MSGID(EID_USER_JOINED), (WPARAM)uid, (LPARAM)elapsed);
    }
}

void C<ExampleName>RtcEngineEventHandler::onUserOffline(uid_t uid, USER_OFFLINE_REASON_TYPE reason) {
    if (m_hMsgHandler) {
        ::PostMessage(m_hMsgHandler, WM_MSGID(EID_USER_OFFLINE), (WPARAM)uid, (LPARAM)reason);
    }
}

void C<ExampleName>RtcEngineEventHandler::onError(int err) {
    if (m_hMsgHandler) {
        ::PostMessage(m_hMsgHandler, WM_MSGID(EID_ERROR), (WPARAM)err, 0);
    }
}

// Dialog Implementation
C<ExampleName>Dlg::C<ExampleName>Dlg(CWnd* pParent)
    : CDialogEx(IDD_DIALOG_<EXAMPLENAME>, pParent) {
}

C<ExampleName>Dlg::~C<ExampleName>Dlg() {
    if (m_rtcEngine) {
        m_rtcEngine->release();
        m_rtcEngine = nullptr;
    }
}

void C<ExampleName>Dlg::DoDataExchange(CDataExchange* pDX) {
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(C<ExampleName>Dlg, CDialogEx)
    ON_BN_CLICKED(IDC_BUTTON_JOIN, &C<ExampleName>Dlg::OnBnClickedButtonJoin)
    ON_BN_CLICKED(IDC_BUTTON_LEAVE, &C<ExampleName>Dlg::OnBnClickedButtonLeave)
    ON_MESSAGE(WM_MSGID(EID_JOIN_CHANNEL_SUCCESS), &C<ExampleName>Dlg::OnMsgEngineEvent)
    ON_MESSAGE(WM_MSGID(EID_USER_JOINED), &C<ExampleName>Dlg::OnMsgEngineEvent)
    ON_MESSAGE(WM_MSGID(EID_USER_OFFLINE), &C<ExampleName>Dlg::OnMsgEngineEvent)
    ON_MESSAGE(WM_MSGID(EID_ERROR), &C<ExampleName>Dlg::OnMsgEngineEvent)
END_MESSAGE_MAP()

BOOL C<ExampleName>Dlg::OnInitDialog() {
    CDialogEx::OnInitDialog();
    InitializeAgoraEngine();
    return TRUE;
}

void C<ExampleName>Dlg::PostNcDestroy() {
    LeaveChannel();
    CDialogEx::PostNcDestroy();
    delete this;
}

void C<ExampleName>Dlg::InitializeAgoraEngine() {
    m_rtcEngine = createAgoraRtcEngine();
    if (!m_rtcEngine) return;
    
    RtcEngineContext context;
    context.appId = CConfig::GetAppId();
    context.eventHandler = &m_eventHandler;
    m_eventHandler.SetMsgReceiver(m_hWnd);
    
    m_rtcEngine->initialize(context);
    m_rtcEngine->enableVideo();
    m_rtcEngine->enableAudio();
}

void C<ExampleName>Dlg::JoinChannel() {
    if (!m_rtcEngine) return;
    
    const char* token = CConfig::GetToken("test");
    m_rtcEngine->joinChannel(token, "test", "", 0);
}

void C<ExampleName>Dlg::LeaveChannel() {
    if (!m_rtcEngine) return;
    
    m_rtcEngine->leaveChannel();
    m_isJoined = false;
}

void C<ExampleName>Dlg::OnBnClickedButtonJoin() {
    JoinChannel();
}

void C<ExampleName>Dlg::OnBnClickedButtonLeave() {
    LeaveChannel();
}

LRESULT C<ExampleName>Dlg::OnMsgEngineEvent(WPARAM wParam, LPARAM lParam) {
    // Handle engine events
    return 0;
}
