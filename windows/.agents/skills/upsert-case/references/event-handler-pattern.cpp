// Event Handler Pattern - Post messages to main thread

class CExampleRtcEngineEventHandler : public IRtcEngineEventHandler {
public:
    void SetMsgReceiver(HWND hWnd) { m_hMsgHandler = hWnd; }
    
    virtual void onJoinChannelSuccess(const char* channel, uid_t uid, int elapsed) override {
        if (m_hMsgHandler) {
            // Post message to main thread
            ::PostMessage(m_hMsgHandler, WM_MSGID(EID_JOIN_CHANNEL_SUCCESS), (WPARAM)uid, 0);
        }
    }
    
private:
    HWND m_hMsgHandler = nullptr;
};
