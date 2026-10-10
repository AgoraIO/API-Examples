#include "stdafx.h"
#include "APIExample.h"
#include "C<ExampleName>Dlg.h"
#include "CConfig.h"

// Skeleton: wire these resource/control IDs in APIExample.rc, host scene registration,
// localization and project membership. Add case-specific media, permission/device checks,
// connection UI state and Token renewal. This join publishes microphone audio only.
IMPLEMENT_DYNAMIC(C<ExampleName>Dlg, CDialogEx)

void C<ExampleName>RtcEngineEventHandler::onJoinChannelSuccess(
    const char* channel, agora::rtc::uid_t uid, int elapsed) {
    if (m_hMsgHandler) {
        ::PostMessage(m_hMsgHandler, WM_MSGID(EID_JOINCHANNEL_SUCCESS), (WPARAM)uid, (LPARAM)elapsed);
    }
}

void C<ExampleName>RtcEngineEventHandler::onError(int err, const char* msg) {
    if (m_hMsgHandler) {
        // Copy only the numeric code; the SDK-owned msg pointer expires after return.
        ::PostMessage(m_hMsgHandler, WM_MSGID(EID_ERROR), (WPARAM)err, 0);
    }
}

C<ExampleName>Dlg::C<ExampleName>Dlg(CWnd* pParent)
    : CDialogEx(IDD_DIALOG_<EXAMPLENAME>, pParent) {
}

C<ExampleName>Dlg::~C<ExampleName>Dlg() {
    UnInitAgora(); // Fallback; ReleaseScene must clean up before hiding this window.
}

void C<ExampleName>Dlg::DoDataExchange(CDataExchange* pDX) {
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(C<ExampleName>Dlg, CDialogEx)
    ON_BN_CLICKED(IDC_BUTTON_JOINCHANNEL, &C<ExampleName>Dlg::OnBnClickedButtonJoin)
    ON_BN_CLICKED(IDC_BUTTON_LEAVE_CHANNEL, &C<ExampleName>Dlg::OnBnClickedButtonLeave)
    ON_MESSAGE(WM_MSGID(EID_JOINCHANNEL_SUCCESS), &C<ExampleName>Dlg::OnMsgJoinChannel)
    ON_MESSAGE(WM_MSGID(EID_ERROR), &C<ExampleName>Dlg::OnMsgError)
END_MESSAGE_MAP()

BOOL C<ExampleName>Dlg::OnInitDialog() {
    CDialogEx::OnInitDialog();
    // UI only: the host precreates all scenes, including inactive ones.
    return TRUE;
}

bool C<ExampleName>Dlg::InitAgora() {
    if (m_rtcEngine) return true;
    m_appId = GET_APP_ID;
    if (m_appId.empty()) return false;
    m_rtcEngine = createAgoraRtcEngine();
    if (!m_rtcEngine) return false;

    m_eventHandler.SetMsgReceiver(m_hWnd);
    RtcEngineContext context;
    context.appId = m_appId.c_str();
    context.eventHandler = &m_eventHandler;
    int result = m_rtcEngine->initialize(context);
    if (result != 0) {
        TRACE(_T("initialize failed: %d\n"), result);
        UnInitAgora();
        return false;
    }
    return true;
}

void C<ExampleName>Dlg::UnInitAgora() {
    // Invalidate any case-specific asynchronous Token/permission work before cleanup.
    if (m_rtcEngine) {
        LeaveChannel(); // Also covers an accepted join that has not completed yet.
        // Stop case-owned capture, players, timers and observers here.
        m_rtcEngine->release(nullptr); // Current SDK: synchronous, never from an SDK callback.
        m_rtcEngine = nullptr;
    }
    // No more SDK callbacks after synchronous release; the receiver can now be cleared.
    m_eventHandler.SetMsgReceiver(nullptr);
    DiscardEngineMessages();
}

void C<ExampleName>Dlg::DiscardEngineMessages() {
    if (!::IsWindow(m_hWnd)) return;
    MSG message;
    const UINT ids[] = { WM_MSGID(EID_JOINCHANNEL_SUCCESS), WM_MSGID(EID_ERROR) };
    for (UINT id : ids) {
        while (::PeekMessage(&message, m_hWnd, id, id, PM_REMOVE)) {}
    }
    // These messages contain scalar values only. Free owned payloads if adding any.
}

void C<ExampleName>Dlg::JoinChannel() {
    if (!m_rtcEngine) return;
    CString channelInput, tokenInput;
    GetDlgItemText(IDC_EDIT_CHANNELNAME, channelInput);
    GetDlgItemText(IDC_EDIT_TOKEN, tokenInput);
    if (channelInput.IsEmpty()) return;
    const std::string channelName = cs2utf8(channelInput);
    const std::string token = cs2utf8(tokenInput);
    // Obtain microphone permission/device access before wiring this action.
    // The supplied Token must match this channel and uid (0 here).
    ChannelMediaOptions options;
    options.channelProfile = CHANNEL_PROFILE_LIVE_BROADCASTING;
    options.clientRoleType = CLIENT_ROLE_BROADCASTER;
    options.publishMicrophoneTrack = true;
    options.publishCameraTrack = false;
    int result = m_rtcEngine->joinChannel(token.c_str(), channelName.c_str(), 0, options);
    if (result != 0) {
        TRACE(_T("joinChannel failed: %d\n"), result);
    }
}

void C<ExampleName>Dlg::LeaveChannel() {
    if (!m_rtcEngine) return;
    int result = m_rtcEngine->leaveChannel();
    if (result != 0) TRACE(_T("leaveChannel failed: %d\n"), result);
}

void C<ExampleName>Dlg::OnBnClickedButtonJoin() { JoinChannel(); }
void C<ExampleName>Dlg::OnBnClickedButtonLeave() { LeaveChannel(); }

LRESULT C<ExampleName>Dlg::OnMsgJoinChannel(WPARAM wParam, LPARAM lParam) {
    if (!m_rtcEngine) return 0;
    TRACE(_T("Join callback: uid=%u, elapsed=%d\n"), (unsigned int)wParam, (int)lParam);
    // Update the case's connection UI state here, accounting for leave/rejoin requests.
    return 0;
}

LRESULT C<ExampleName>Dlg::OnMsgError(WPARAM wParam, LPARAM lParam) {
    if (!m_rtcEngine) return 0;
    TRACE(_T("RTC error: %d\n"), (int)wParam);
    return 0;
}
