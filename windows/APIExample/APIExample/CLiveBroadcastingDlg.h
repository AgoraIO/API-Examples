#pragma once

#include "AGVideoWnd.h"

#include <list>
// CLiveBroadcastingDlg dialog
#define VIDEO_COUNT      36
#define IDC_BASEWND_VIDEO               113
class CLiveBroadcastingRtcEngineEventHandler
    : public IRtcEngineEventHandler
{
public:
    CLiveBroadcastingRtcEngineEventHandler() {}
    ~CLiveBroadcastingRtcEngineEventHandler() {}
    void SetMsgReceiver(HWND hWnd) { m_hMsgHanlder = hWnd; }
    /**Occurs when the local user joins a specified channel.
        * The channel name assignment is based on channelName specified in the joinChannel method.
        * If the uid is not specified when joinChannel is called, the server automatically assigns a uid.
        * @param channel Channel name
        * @param uid User ID
        * @param elapsed Time elapsed (ms) from the user calling joinChannel until this callback is triggered*/
    virtual void onJoinChannelSuccess(const char* channel, uid_t uid, int elapsed) override;
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
    /**Occurs when a user leaves the channel.
         * @param stats With this callback, the application retrieves the channel information,
         *              such as the call duration and statistics.*/
    virtual void onLeaveChannel(const RtcStats& stats) override;
private:
    HWND m_hMsgHanlder;
};

class CLiveBroadcastingDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CLiveBroadcastingDlg)

public:
	CLiveBroadcastingDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CLiveBroadcastingDlg();

    bool InitAgora();
    void UnInitAgora();
    
// Dialog Data
    enum { IDD = IDD_DIALOG_LIVEBROADCASTING };
    enum PEOPLE_IN_CHANNEL_TYPE {
        PEOPLE_IN_CHANNEL_2 = 0,
        PEOPLE_IN_CHANNEL_4,
        PEOPLE_IN_CHANNEL_9,
        PEOPLE_IN_CHANNEL_16
    };
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnSelchangeComboPersons();
    afx_msg void OnSelchangeComboRole();
    afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
    //Agora Event handler
    afx_msg LRESULT OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnEIDLeaveChannel(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnEIDUserJoined(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnEIDUserOffline(WPARAM wParam, LPARAM lParam);
private:
    void InitCtrlText();
    void CreateAllVideoWnds();
    void ShowVideoWnds();
    void RenderLocalVideo();
    IRtcEngine* m_rtcEngine = nullptr;
    CLiveBroadcastingRtcEngineEventHandler m_eventHandler;
    bool joinChannel = false;
    bool m_initialize = false;
    //video wnd
    CAGVideoWnd m_videoWnds[VIDEO_COUNT];
    int m_maxVideoCount = 4;
    std::list<uid_t> m_lstUids;
public:
    CComboBox m_cmbRole;
    CStatic m_staRole;
    CComboBox m_cmbPersons;
    CEdit m_edtChannelName;
    CButton m_btnJoinChannel;
    afx_msg void OnBnClickedButtonJoinchannel();
    CListBox m_lstInfo;
    virtual BOOL OnInitDialog();
    CStatic m_videoArea;

    CStatic m_staPersons;
    CStatic m_staChannelName;
    CStatic m_staDetail;
    afx_msg void OnSelchangeListInfoBroadcasting();
};
