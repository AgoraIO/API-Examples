---
name: review-case
description: >
  Code review for API examples. Ensures examples follow project conventions,
  handle lifecycle correctly, manage threads safely, and use APIs properly.
compatibility: [Cursor, Kiro, Windsurf, Claude, Copilot]
license: MIT
metadata:
  author: APIExample Team
  version: 1.0.0
  platform: Windows
---

# Review Case Skill — Windows

## When to Use

Use this skill when you need to:
- Review a new or modified example for correctness
- Ensure the example follows project conventions
- Verify lifecycle management and thread safety
- Check API usage and error handling

## Review Dimensions (Priority Order)

### 1. Engine Lifecycle (CRITICAL)

**Check:**
- [ ] Engine is created in `InitializeAgoraEngine()` or similar
- [ ] Engine is initialized with `RtcEngineContext`
- [ ] `leaveChannel()` is called before `release()`
- [ ] `release()` is called in `PostNcDestroy()` or cleanup method
- [ ] No engine leaks (engine not recreated on every join)

**Correct Pattern:**
```cpp
BOOL CExampleDlg::OnInitDialog() {
    CDialogEx::OnInitDialog();
    InitializeAgoraEngine();  // Create once
    return TRUE;
}

void CExampleDlg::PostNcDestroy() {
    LeaveChannel();
    if (m_rtcEngine) {
        m_rtcEngine->release();
        m_rtcEngine = nullptr;
    }
    CDialogEx::PostNcDestroy();
    delete this;
}

void CExampleDlg::JoinChannel() {
    if (!m_rtcEngine) return;
    m_rtcEngine->joinChannel(token, channelName, "", 0);
}

void CExampleDlg::LeaveChannel() {
    if (!m_rtcEngine) return;
    m_rtcEngine->leaveChannel();
}
```

**Incorrect Pattern:**
See `references/incorrect-lifecycle.cpp` for common mistakes.

---

### 2. Thread Safety (CRITICAL)

**Check:**
- [ ] All UI updates in event handler use message map pattern
- [ ] Event handler posts messages to main thread via `PostMessage()`
- [ ] No direct UI updates from background threads
- [ ] Message handlers update UI on main thread

**Correct Pattern:**
```cpp
// Event handler (may be called from background thread)
void CExampleRtcEngineEventHandler::onJoinChannelSuccess(const char* channel, uid_t uid, int elapsed) {
    if (m_hMsgHandler) {
        // Post message to main thread
        ::PostMessage(m_hMsgHandler, WM_MSGID(EID_JOIN_CHANNEL_SUCCESS), (WPARAM)uid, 0);
    }
}

// Message handler (runs on main thread)
LRESULT CExampleDlg::OnMsgEngineEvent(WPARAM wParam, LPARAM lParam) {
    // Safe to update UI here
    m_statusText.SetWindowText(_T("Joined channel"));
    return 0;
}
```

**Incorrect Pattern:**
See `references/incorrect-thread-safety.cpp` for common mistakes.

---

### 3. Permission Handling (HIGH)

**Check:**
- [ ] Microphone permission checked before `enableAudio()`
- [ ] Camera permission checked before `enableVideo()`
- [ ] Device availability verified

**Correct Pattern:**
```cpp
void CExampleDlg::InitializeAgoraEngine() {
    m_rtcEngine = createAgoraRtcEngine();
    if (!m_rtcEngine) return;
    
    RtcEngineContext context;
    context.appId = CConfig::GetAppId();
    context.eventHandler = &m_eventHandler;
    m_eventHandler.SetMsgReceiver(m_hWnd);
    
    m_rtcEngine->initialize(context);
    
    // Check device availability
    if (m_rtcEngine->enableVideo() == 0) {
        // Video enabled successfully
    }
    if (m_rtcEngine->enableAudio() == 0) {
        // Audio enabled successfully
    }
}
```

---

### 4. Error Handling (HIGH)

**Check:**
- [ ] `joinChannel()` return value checked
- [ ] Token expiration is handled
- [ ] Network errors are logged or displayed
- [ ] Invalid parameters are validated
- [ ] `onError()` callback implemented

**Correct Pattern:**
```cpp
void CExampleDlg::JoinChannel() {
    if (!m_rtcEngine) return;
    
    const char* token = CConfig::GetToken("test");
    int ret = m_rtcEngine->joinChannel(token, "test", "", 0);
    if (ret != 0) {
        // Handle error
        MessageBox(_T("Failed to join channel"), _T("Error"));
    }
}

void CExampleRtcEngineEventHandler::onError(int err) {
    if (m_hMsgHandler) {
        ::PostMessage(m_hMsgHandler, WM_MSGID(EID_ERROR), (WPARAM)err, 0);
    }
}

LRESULT CExampleDlg::OnMsgEngineEvent(WPARAM wParam, LPARAM lParam) {
    if (wParam == EID_ERROR) {
        int errorCode = (int)lParam;
        // Handle error
    }
    return 0;
}
```

---

### 5. Code Convention (MEDIUM)

**Check:**
- [ ] Dialog class name follows pattern: `C<ExampleName>Dlg`
- [ ] Event handler class name: `C<ExampleName>RtcEngineEventHandler`
- [ ] File names match class names (PascalCase with C prefix)
- [ ] Member variables use `m_` prefix
- [ ] Message map properly defined
- [ ] Comments explain non-obvious logic

**Correct Pattern:**
```cpp
// Header: CScreenShareDlg.h
class CScreenShareRtcEngineEventHandler : public IRtcEngineEventHandler {
    // ...
};

class CScreenShareDlg : public CDialogEx {
    DECLARE_DYNAMIC(CScreenShareDlg)
    
private:
    IRtcEngine* m_rtcEngine = nullptr;
    CScreenShareRtcEngineEventHandler m_eventHandler;
    uid_t m_remoteUid = 0;
    bool m_isJoined = false;
    
    BEGIN_MESSAGE_MAP(CScreenShareDlg, CDialogEx)
        ON_BN_CLICKED(IDC_BUTTON_JOIN, &CScreenShareDlg::OnBnClickedButtonJoin)
    END_MESSAGE_MAP()
};
```

---

### 6. API Usage Correctness (MEDIUM)

**Check:**
- [ ] SDK methods called in correct order
- [ ] Required parameters are provided
- [ ] Optional parameters are used correctly
- [ ] Return values are checked where necessary
- [ ] Deprecated APIs are not used

**Correct Pattern:**
```cpp
// Correct order: create -> initialize -> enable -> join
m_rtcEngine = createAgoraRtcEngine();
m_rtcEngine->initialize(context);
m_rtcEngine->enableVideo();
m_rtcEngine->enableAudio();
m_rtcEngine->joinChannel(token, channelName, "", 0);
```

**Incorrect Pattern:**
```cpp
// ❌ Wrong order
m_rtcEngine->joinChannel(...);  // Join first
m_rtcEngine->enableVideo();     // Enable after join (too late)
```

---

### 7. Resource Cleanup (MEDIUM)

**Check:**
- [ ] Audio files are stopped and released
- [ ] Video captures are stopped
- [ ] Custom audio/video sources are cleaned up
- [ ] Observers are unregistered
- [ ] Timers are killed

**Correct Pattern:**
```cpp
void CExampleDlg::LeaveChannel() {
    if (!m_rtcEngine) return;
    
    m_rtcEngine->stopAudioMixing();      // Stop audio
    m_rtcEngine->stopScreenCapture();    // Stop screen share
    m_rtcEngine->leaveChannel();
    m_isJoined = false;
}

void CExampleDlg::PostNcDestroy() {
    LeaveChannel();
    if (m_rtcEngine) {
        m_rtcEngine->release();
        m_rtcEngine = nullptr;
    }
    CDialogEx::PostNcDestroy();
    delete this;
}
```

---

## Review Output Format

When reviewing, provide feedback in this format:

```
## Review Results

### ✅ Passed
- Engine lifecycle correctly managed
- Thread safety ensured with message map pattern
- Error handling implemented for joinChannel

### ⚠️ Issues Found

**[HIGH] Thread Safety Issue**
- File: `CScreenShareDlg.cpp`
- Line: 45
- Issue: Direct UI update in event handler without PostMessage
- Suggestion: Use PostMessage to post event to main thread

**[MEDIUM] Missing Error Handling**
- File: `CScreenShareDlg.cpp`
- Line: 78
- Issue: joinChannel() return value not checked
- Suggestion: Check return value and handle errors

### 🔧 Recommendations
- Add logging for debugging
- Consider adding retry logic for network failures
```

---

## Platform-Specific Checks

### Windows-Specific

**Check:**
- [ ] Using MFC — not WinForms or WPF
- [ ] Using C++ — not C#
- [ ] Following MFC naming conventions (C prefix, m_ prefix)
- [ ] Message map properly defined
- [ ] Dialog resource properly configured
- [ ] No modern C++ patterns unless already in codebase

**Correct Pattern:**
```cpp
// Windows: Use MFC
#include "stdafx.h"
#include "APIExample.h"

class CExampleDlg : public CDialogEx {
    DECLARE_DYNAMIC(CExampleDlg)
    
    BEGIN_MESSAGE_MAP(CExampleDlg, CDialogEx)
        ON_BN_CLICKED(IDC_BUTTON_JOIN, &CExampleDlg::OnBnClickedButtonJoin)
    END_MESSAGE_MAP()
};
```

**Incorrect Pattern:**
```cpp
// ❌ Non-MFC patterns
using namespace std;  // Avoid in MFC
auto ptr = std::make_unique<IRtcEngine>();  // Modern C++ not typical in MFC
```

---

## NEVER List

**Do NOT accept:**
- Engine not released (memory leak)
- Direct UI updates from event handler without PostMessage
- Multiple engine instances in one example
- Hardcoded App ID or token (must use CConfig)
- Missing `leaveChannel()` before `release()`
- C# or other languages (C++ only)
- WinForms or WPF (MFC only)
- Examples outside `APIExample/APIExample/[Basic|Advanced]/` structure
- Missing event handler implementation
- No error handling for joinChannel failures
- Deviation from MFC naming conventions

---

## Review Checklist

Use this checklist when reviewing an example:

**Lifecycle:**
- [ ] Engine created once in initialization
- [ ] `leaveChannel()` called before `release()`
- [ ] `release()` called in `PostNcDestroy()`
- [ ] No engine leaks

**Thread Safety:**
- [ ] All UI updates use message map pattern
- [ ] Event handler posts messages via `PostMessage()`
- [ ] No direct UI updates from callbacks
- [ ] Message handlers run on main thread

**Permissions:**
- [ ] Microphone availability checked
- [ ] Camera availability checked
- [ ] Device errors handled

**Error Handling:**
- [ ] joinChannel return value checked
- [ ] Token expiration handled
- [ ] Network errors logged
- [ ] onError() callback implemented

**Code Quality:**
- [ ] Follows MFC naming conventions
- [ ] Message map properly defined
- [ ] Comments explain non-obvious logic
- [ ] No hardcoded credentials

**API Usage:**
- [ ] Methods called in correct order
- [ ] Required parameters provided
- [ ] Return values checked
- [ ] No deprecated APIs

**Resources:**
- [ ] Audio/video properly stopped
- [ ] Observers unregistered
- [ ] Timers killed
- [ ] No resource leaks

**Platform:**
- [ ] Using MFC (not WinForms/WPF)
- [ ] Using C++ (not C#)
- [ ] Following MFC conventions
- [ ] No modern C++ patterns unless existing

---

## Common Issues and Fixes

### Issue: "Engine not initialized"
**Cause:** `release()` called without `leaveChannel()` first
**Fix:** Always call `leaveChannel()` before `release()`

### Issue: "UI crashes or doesn't update"
**Cause:** Direct UI update from event handler
**Fix:** Use PostMessage to post event to main thread

### Issue: "Memory leak detected"
**Cause:** `release()` not called or engine recreated
**Fix:** Ensure `release()` in `PostNcDestroy()` and create engine once

### Issue: "Token expired error"
**Cause:** No token refresh handling
**Fix:** Implement token refresh in error handler

### Issue: "No audio/video"
**Cause:** Device not available or not enabled
**Fix:** Check return values of `enableAudio()` / `enableVideo()`

---

## References

- **Agora RTC SDK for Windows:** [Documentation](https://docs.agora.io/en/video-calling/reference/windows-sdk)
- **Existing examples:** Review `APIExample/APIExample/Basic/JoinChannelVideoByToken/` for reference
- **MFC Documentation:** [Microsoft Foundation Classes](https://docs.microsoft.com/en-us/cpp/mfc/mfc-desktop-applications)
- **Message Map:** [MFC Message Maps](https://docs.microsoft.com/en-us/cpp/mfc/message-maps)
