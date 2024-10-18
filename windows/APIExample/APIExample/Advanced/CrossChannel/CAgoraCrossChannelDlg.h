#pragma once
#include "AGVideoWnd.h"

class CAgoraCrossChannelEventHandler : public IRtcEngineEventHandler
{
public:
	//set the message notify window handler
	void SetMsgReceiver(HWND hWnd) { m_hMsgHanlder = hWnd; }

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
	virtual void onJoinChannelSuccess(const char* channel, uid_t uid, int elapsed)
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
		by the callback(ms).
	*/
	virtual void onUserJoined(uid_t uid, int elapsed) override
	{
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
	virtual void onUserOffline(uid_t uid, USER_OFFLINE_REASON_TYPE reason) override
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
	parameters:
		stats: Call statistics.
	*/
	virtual void onLeaveChannel(const RtcStats& stats) override
	{
		if (m_hMsgHanlder) {
			::PostMessage(m_hMsgHanlder, WM_MSGID(EID_LEAVE_CHANNEL), 0, 0);
		}
	}


	virtual void onChannelMediaRelayStateChanged(int state, int code)override {
		if (m_hMsgHanlder)
			::PostMessage(m_hMsgHanlder, WM_MSGID(EID_CHANNEL_MEDIA_RELAY_STATE_CHNAGENED), state, code);
	}

	/** Reports events during the media stream relay.
	 *
	 * @param code The event code in #CHANNEL_MEDIA_RELAY_EVENT.
	 */
	virtual void onChannelMediaRelayEvent(int code) {
		if (m_hMsgHanlder)
			::PostMessage(m_hMsgHanlder, WM_MSGID(EID_CHANNEL_MEDIA_RELAY_EVENT), code, 0);
	}

private:
	HWND m_hMsgHanlder;
};

class CAgoraCrossChannelDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAgoraCrossChannelDlg)

public:
	CAgoraCrossChannelDlg(CWnd* pParent = nullptr);   
	virtual ~CAgoraCrossChannelDlg();

	enum { IDD = IDD_DIALOG_CROSS_CHANNEL };
public:
	//Initialize the Ctrl Text.
	void InitCtrlText();
	//Initialize the Agora SDK
	bool InitAgora();
	//UnInitialize the Agora SDK
	void UnInitAgora();
	//render local video from SDK local capture.
	void RenderLocalVideo();
	//resume window status
	void ResumeStatus();


private:
	bool m_joinChannel = false;
	bool m_initialize = false;
	bool m_startMediaRelay = false;
	IRtcEngine* m_rtcEngine = nullptr;
	CAGVideoWnd m_localVideoWnd;
	CAgoraCrossChannelEventHandler m_eventHandler;
	std::vector<ChannelMediaInfo> m_vecChannelMedias;
	ChannelMediaInfo * m_srcInfo;
	

protected:
	virtual void DoDataExchange(CDataExchange* pDX);   
	LRESULT OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam);
	LRESULT OnEIDLeaveChannel(WPARAM wParam, LPARAM lParam);
	LRESULT OnEIDUserJoined(WPARAM wParam, LPARAM lParam);
	LRESULT OnEIDUserOffline(WPARAM wParam, LPARAM lParam);

	LRESULT OnEIDChannelMediaRelayStateChanged(WPARAM wParam, LPARAM lParam);
	LRESULT OnEIDChannelMediaRelayEvent(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
public:
	CStatic m_staVideoArea;
	CListBox m_lstInfo;
	CStatic m_staChannel;
	CEdit m_edtChannel;
	CButton m_btnJoinChannel;
	CEdit m_edtCrossChannel;
	CStatic m_staToken;
	CEdit m_edtToken;
	CStatic m_staUserID;
	CEdit m_edtUserID;
	CComboBox m_cmbCrossChannelList;
	CStatic m_staCrossChannel;
	CStatic m_staCrossChannelList;
	CButton m_btnAddChannel;
	CButton m_btnRemove;
	CButton m_btnStartMediaRelay;
	CStatic m_staDetails;
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButtonJoinchannel();
	afx_msg void OnBnClickedButtonAddCrossChannel();
	afx_msg void OnBnClickedButtonRemoveCrossChannel2();
	afx_msg void OnBnClickedButtonStartMediaRelay();
	afx_msg void OnSelchangeListInfoBroadcasting();
	CButton m_btnUpdate;
	afx_msg void OnBnClickedButtonUpdate();
};
