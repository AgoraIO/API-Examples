#include "stdafx.h"
#include"AGVideoWnd.h"


class CScreenCaptureEventHandler : public IRtcEngineEventHandler
{
public:
    //set the message notify window handler
    void SetMsgReceiver(HWND hWnd) { m_hMsgHanlder = hWnd; }

	/** Occurs when the local video stream state changes.
	 This callback indicates the state of the local video stream, including camera capturing and video encoding, and allows you to troubleshoot issues when exceptions occur.
	 @note For some device models, the SDK will not trigger this callback when the state of the local video changes while the local video capturing device is in use, so you have to make your own timeout judgment.
	 @param localVideoState State type #LOCAL_VIDEO_STREAM_STATE. When the state is LOCAL_VIDEO_STREAM_STATE_FAILED (3), see the `error` parameter for details.
	 @param error The detailed error information: #LOCAL_VIDEO_STREAM_ERROR.
	 */
	virtual void onLocalVideoStateChanged(LOCAL_VIDEO_STREAM_STATE localVideoState, LOCAL_VIDEO_STREAM_ERROR error) override;
    /*
    note:
        Join the channel callback.This callback method indicates that the client
        successfully joined the specified channel.Channel ids are assigned based
        on the channel name specified in the joinChannel. If IRtcEngine::joinChannel
        is called without a user ID specified. The server will automatically assign one
    parameters:
        channel:channel name.
        uid: user ID.If the UID is specified in the joinChannel, that ID is returned here;
        Otherwise, use the ID automatically assigned by the Agora server.
        elapsed: The Time from the joinChannel until this event occurred (ms).
    */
    virtual void onJoinChannelSuccess(const char* channel, uid_t uid, int elapsed) override;
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
        by the callback(ms).
    */
    virtual void onUserJoined(uid_t uid, int elapsed) override;
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
    virtual void onUserOffline(uid_t uid, USER_OFFLINE_REASON_TYPE reason) override;
    /*
    note:
        When the App calls the leaveChannel method, the SDK indicates that the App
        has successfully left the channel. In this callback method, the App can get
        the total call time, the data traffic sent and received by THE SDK and other
        information. The App obtains the call duration and data statistics received
        or sent by the SDK through this callback.
    parameters:
        stats: Call statistics.
    */
    virtual void onLeaveChannel(const RtcStats& stats) override;
    /** 
        Occurs when the remote video state changes.
        @note This callback does not work properly when the number of users (in the Communication profile) or broadcasters (in the Live-broadcast profile) in the channel exceeds 17.

        @param uid ID of the remote user whose video state changes.
        @param state State of the remote video. See #REMOTE_VIDEO_STATE.
        @param reason The reason of the remote video state change. See
        #REMOTE_VIDEO_STATE_REASON.
        @param elapsed Time elapsed (ms) from the local user calling the
        \ref agora::rtc::IRtcEngine::joinChannel "joinChannel" method until the
        SDK triggers this callback.
     */
	virtual void onRemoteVideoStateChanged(uid_t uid, REMOTE_VIDEO_STATE state, REMOTE_VIDEO_STATE_REASON reason, int elapsed) override;
	/** Occurs when screencapture fail to filter window
 *
 *
 * @param ScreenCaptureInfo
 */
	virtual	void onScreenCaptureInfoUpdated(agora::rtc::ScreenCaptureInfo & info) override;
private:
	HWND m_hMsgHanlder;
};


class CMonitors {
public:
    typedef struct _MonitorInformation {
        MONITORINFOEX monitorInfo;
        int scale_num = 4;
        int scale_den = 4;
        std::string monitorName = "";
        bool canShare = true;
        HMONITOR hMonitor;
    }MonitorInformation, *PMonitorInformation, *LPMonitorInformation;
    CMonitors();
    ~CMonitors();
    static BOOL MonitorFunc(HMONITOR hMonitor, HDC hDc, LPRECT lpRect, LPARAM lParam);
    void EnumMonitor();
    agora::rtc::Rectangle RectToRectangle(RECT rc);
    std::vector<MonitorInformation> GetMonitors() { return m_vecMonitorInfos; }
    MonitorInformation GetMonitorInformation(int index);
    agora::rtc::Rectangle GetMonitorRectangle(int index);
    bool GetMonitorRectangle(HMONITOR hMonitor, agora::rtc::Rectangle& screenRegion);
    agora::rtc::Rectangle GetScreenRect();
    int GetMonitorCount() { return m_vecMonitorInfos.size(); }
    bool IsValid();
    bool CheckMonitorValid(HMONITOR hMonitor);
    bool GetWindowRect(HWND hWnd, agora::rtc::Rectangle& regionRect);
private:
    void Clear();
    std::vector<MonitorInformation> m_vecMonitorInfos;
    std::vector<MonitorInformation> m_vecEffectiveMonitorInfos;
    RECT m_screenRegion;

};

class CAgoraScreenCapture : public CDialogEx
{
    DECLARE_DYNAMIC(CAgoraScreenCapture)

public:
    CAgoraScreenCapture(CWnd* pParent = nullptr);  
    virtual ~CAgoraScreenCapture();

    //Initialize the Agora SDK
    bool InitAgora();
    //UnInitialize the Agora SDK
    void UnInitAgora();
    //set control text from config.
    void InitCtrlText();
    //render local video from SDK local capture.
    void RenderLocalVideo();
    //callback window enum.
    static BOOL CALLBACK WndEnumProc(HWND hWnd, LPARAM lParam);
    //refresh window to show.
    void ReFreshWnd();
    //resume window
    void ResumeStatus();
    //refresh window info to list.
    int	RefreashWndInfo();

    enum { IDD = IDD_DIALOG_SCREEN_SHARE };
    afx_msg LRESULT OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnEIDLeaveChannel(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnEIDUserJoined(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnEIDUserOffline(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnEIDRemoteVideoStateChanged(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEIDLocalVideoStateChanged(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEIDScreenCaptureInfoUpdated(WPARAM wParam, LPARAM lParam);
	
protected:
    virtual void DoDataExchange(CDataExchange* pDX);   
    DECLARE_MESSAGE_MAP()

    CAGVideoWnd m_localVideoWnd;
    CList<HWND>	m_listWnd;
    CScreenCaptureEventHandler m_eventHandler;

    IRtcEngine* m_rtcEngine = nullptr;
    bool m_joinChannel = false;
    bool m_initialize = false;
    bool m_addInjectStream = false;
    bool m_windowShare = false;
    bool m_screenShare = false;
    CMonitors m_monitors;

    void GetCaptureParameterFromCtrl(agora::rtc::ScreenCaptureParameters& capParam);
    void InitMonitorInfos();
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnBnClickedButtonJoinchannel();
    afx_msg void OnBnClickedButtonStartShare();
    afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);

    CStatic m_staVideoArea;
    CStatic m_staChannel;
    CEdit m_edtChannel;
    CStatic m_staScreenCap;
    CComboBox m_cmbScreenCap;
    CButton m_btnStartCap;
    CButton m_btnJoinChannel;
    CListBox m_lstInfo;
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnBnClickedButtonUpdateparam();
    CComboBox m_cmbScreenRegion;
    CButton m_chkShareCursor;
    CEdit m_edtFPS;
    CEdit m_edtBitrate;
    //afx_msg void OnBnClickedButtonShareDesktop();
    //afx_msg void OnCbnSelchangeComboScreenRegion();
    CComboBox m_cmbShareDesktopRect;
    CComboBox m_cmbShareRegion;
    afx_msg void OnBnClickedButtonStartShareScreen();
    CButton m_btnShareScreen;
    CStatic m_staScreenInfo;
    CStatic m_staScreenInfo2;
	CStatic m_staFPS;
	CStatic m_staBitrate;
	CStatic m_staGeneral;
	CButton m_btnUpdateCaptureParam;
	CStatic m_StaScreen;
	CComboBox m_cmbExcluedWndList;
	CStatic m_staExcludeWndList;
	CButton m_chkWndFocus;
	afx_msg void OnSelchangeListInfoBroadcasting();
	CStatic m_staDetails;
};
