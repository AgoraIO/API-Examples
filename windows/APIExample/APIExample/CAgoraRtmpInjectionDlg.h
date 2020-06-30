#pragma once
#include <string>
#include "AGVideoWnd.h"
class CAgoraRtmpInjectionRtcEngineEventHandler
    : public IRtcEngineEventHandler
{
public:
    CAgoraRtmpInjectionRtcEngineEventHandler() {}
    ~CAgoraRtmpInjectionRtcEngineEventHandler() {}
    void SetMsgReceiver(HWND hWnd) { m_hMsgHanlder = hWnd; }
    /**Occurs when the local user joins a specified channel.
         * The channel name assignment is based on channelName specified in the joinChannel method.
         * If the uid is not specified when joinChannel is called, the server automatically assigns a uid.
         * @param channel Channel name
         * @param uid User ID
         * @param elapsed Time elapsed (ms) from the user calling joinChannel until this callback is triggered*/
    virtual void onJoinChannelSuccess(const char* channel, uid_t uid, int elapsed) override;
    /** Occurs when a voice or video stream URL address is added to a live broadcast.

   @param url Pointer to the URL address of the externally injected stream.
   @param uid User ID.
   @param status State of the externally injected stream: #INJECT_STREAM_STATUS.*/
    virtual void onStreamInjectedStatus(const char* url, uid_t uid, int status) override;
    /**Occurs when a user leaves the channel.
         * @param stats With this callback, the application retrieves the channel information,
         *              such as the call duration and statistics.*/
    virtual void onLeaveChannel(const RtcStats& stats) override;
    /**Occurs when a remote user (Communication)/host (Live Broadcast) joins the channel.
     * @param uid ID of the user whose audio state changes.
     * @param elapsed Time delay (ms) from the local user calling joinChannel/setClientRole
     *                until this callback is triggered.*/
    virtual void onUserJoined(uid_t uid, int elapsed) override;
    /**Occurs when a remote user (Communication)/host (Live Broadcast) leaves the channel.
    * @param uid ID of the user whose audio state changes.
    * @param reason Reason why the user goes offline:
    *   USER_OFFLINE_QUIT(0): The user left the current channel.
    *   USER_OFFLINE_DROPPED(1): The SDK timed out and the user dropped offline because no data
    *              packet was received within a certain period of time. If a user quits the
    *               call and the message is not passed to the SDK (due to an unreliable channel),
    *               the SDK assumes the user dropped offline.
    *   USER_OFFLINE_BECOME_AUDIENCE(2): (Live broadcast only.) The client role switched from
    *               the host to the audience.*/
    virtual void onUserOffline(uid_t uid, USER_OFFLINE_REASON_TYPE reason) override;

private:
    HWND m_hMsgHanlder;
};

// CAgoraRtmpInjectionDlg dialog

class CAgoraRtmpInjectionDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAgoraRtmpInjectionDlg)

public:
	CAgoraRtmpInjectionDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CAgoraRtmpInjectionDlg();
    bool InitAgora();
    void UnInitAgora();

// Dialog Data
    enum { IDD = IDD_DIALOG_RTMPINJECT };
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedButtonAddstream();
    afx_msg void OnBnClickedButtonJoinchannel();
    afx_msg LRESULT OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnEIDLeaveChannel(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnEIDStreamInjectedStatus(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnEIDUserJoined(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnEIDUserOffline(WPARAM wParam, LPARAM lParam);
    virtual BOOL OnInitDialog();

private:
    void InitCtrlText();
    void RenderLocalVideo();
    CAGVideoWnd m_localVideoWnd;

    IRtcEngine* m_rtcEngine = nullptr;
    CAgoraRtmpInjectionRtcEngineEventHandler m_eventHandler;
    bool joinChannel = false;
    bool m_initialize = false;
    std::string m_injectUrl;
    bool m_addInjectStream = false;
public:
    afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
    CListBox m_lstInfo;
    CButton m_btnJoinChannel;
    CButton m_btnAddStream;
    CEdit m_edtChannelName;
    CEdit m_edtInjectUrl;
    CStatic m_staVideoArea;

    
    CStatic m_staChannelName;
    CStatic m_staInjectUrl;
    CStatic m_staDetail;
    afx_msg void OnSelchangeListInfoBroadcasting();
};
