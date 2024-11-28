#pragma once
#include "AGVideoWnd.h"
#include <list>
#include <string>
// CLiveBroadcastingDlg dialog

#define VIDEO_COUNT                     36
#define IDC_BASEWND_VIDEO               113

class CLiveBroadcastingRtcEngineEventHandler
    : public IRtcEngineEventHandler
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
        uid: user ID¡£If the UID is specified in the joinChannel, that ID is returned here;
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
        by the callback£¨ms).
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
    parametes:
        stats: Call statistics.
    */
	virtual void onLeaveChannel(const RtcStats& stats) override;
	virtual void onNetworkQuality(uid_t uid, int txQuality, int rxQuality) override {
		if (m_hMsgHanlder&& report) {
			PNetworkQuality quality = new NetworkQuality;
			quality->uid = uid;
			quality->txQuality = txQuality;
			quality->txQuality = rxQuality;

			::PostMessage(m_hMsgHanlder, WM_MSGID(EID_NETWORK_QUALITY), (WPARAM)quality, 0);

		}
	}
	virtual void onRtcStats(const RtcStats& stats) override{ 
		if (m_hMsgHanlder&& report) {
			RtcStats* s = new RtcStats;
			*s = stats;

			::PostMessage(m_hMsgHanlder, WM_MSGID(EID_RTC_STATS), (WPARAM)s, 0);

		}
	}

	
	virtual void onLocalAudioStats(const LocalAudioStats& stats) override{
		if (m_hMsgHanlder&& report) {
			LocalAudioStats* s = new LocalAudioStats;
			*s = stats;
			::PostMessage(m_hMsgHanlder, WM_MSGID(EID_LOCAL_AUDIO_STATS), (WPARAM)s, 0);

		}
	}

	virtual void onLocalAudioStateChanged(LOCAL_AUDIO_STREAM_STATE state, LOCAL_AUDIO_STREAM_REASON error) {
		if (m_hMsgHanlder&& report) {
			::PostMessage(m_hMsgHanlder, WM_MSGID(EID_LOCAL_AUDIO_STATE_CHANED), (WPARAM)state, (LPARAM)error);
		}
	}
		
	virtual void onRemoteAudioStats(const RemoteAudioStats& stats) {
		if (m_hMsgHanlder&& report) {
			RemoteAudioStats* s = new RemoteAudioStats;
			*s = stats;
			::PostMessage(m_hMsgHanlder, WM_MSGID(EID_REMOTE_AUDIO_STATS), (WPARAM)s, 0);

		}
	}

	virtual void onRemoteAudioStateChanged(uid_t uid, REMOTE_AUDIO_STATE state, REMOTE_AUDIO_STATE_REASON reason, int elapsed) {
		if (m_hMsgHanlder&& report) {
			PRemoteAudioState s = new RemoteAudioState;
			s->elapsed = elapsed;
			s->uid = uid;
			s->state = state;
			s->reason = reason;
			::PostMessage(m_hMsgHanlder, WM_MSGID(EID_REMOTE_AUDIO_STATE_CHANGED), (WPARAM)s, 0);

		}
	}
		
	virtual void onLocalVideoStateChanged(LOCAL_VIDEO_STREAM_STATE state, LOCAL_AUDIO_STREAM_REASON error) {
		if (m_hMsgHanlder&& report) {
			::PostMessage(m_hMsgHanlder, WM_MSGID(EID_LOCAL_VIDEO_STATE_CHANGED), (WPARAM)state, (LPARAM)error);
		}
	}	
	virtual void onRemoteVideoStats(const RemoteVideoStats& stats) {
		if (m_hMsgHanlder&& report) {
			RemoteVideoStats* s = new RemoteVideoStats;
			*s = stats;
			::PostMessage(m_hMsgHanlder, WM_MSGID(EID_REMOTE_VIDEO_STATS), (WPARAM)s, 0);

		}
	}

	virtual void onRemoteVideoStateChanged(uid_t uid, REMOTE_VIDEO_STATE state, REMOTE_VIDEO_STATE_REASON reason, int elapsed) {
		if (m_hMsgHanlder&& report) {
			PRemoteVideoState s = new RemoteVideoState;
			s->elapsed = elapsed;
			s->uid = uid;
			s->state = state;
			s->reason = reason;
			::PostMessage(m_hMsgHanlder, WM_MSGID(EID_REMOTE_VIDEO_STATE_CHANGED), (WPARAM)s, 0);

		}
	}

	virtual void onContentInspectResult(media::CONTENT_INSPECT_RESULT result) {
		if (m_hMsgHanlder) {
			::PostMessage(m_hMsgHanlder, WM_MSGID(EID_CONTENT_INSPECT_RESULT), (WPARAM)result, 0);
		}
	}

	virtual void onSnapshotTaken(uid_t uid, const char* filePath, int width, int height, int errCode) {
		if (m_hMsgHanlder) {
			CString* _filePath = new CString;
			_filePath->Format(_T("%s"), utf82cs(std::string(filePath)));

			::PostMessage(m_hMsgHanlder, WM_MSGID(EID_SNAPSHOT_TAKEN), (WPARAM)_filePath, errCode);
		}
	}
	void SetReport(bool b) {report = b;}



	void onLocalVideoStats(VIDEO_SOURCE_TYPE source, const LocalVideoStats& stats) override;


	void onVideoRenderingTracingResult(uid_t uid, MEDIA_TRACE_EVENT currentEvent, VideoRenderingTracingInfo info) override;

private:
    HWND m_hMsgHanlder;
	bool report = false;
	bool moderation = false;
};

class CLiveBroadcastingDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CLiveBroadcastingDlg)

public:
	CLiveBroadcastingDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CLiveBroadcastingDlg();

    //Initialize the Agora SDK
    bool InitAgora();
    //UnInitialize the Agora SDK
    void UnInitAgora();
    
	//resume window status
	void ResumeStatus();

// Dialog Data
    enum { IDD = IDD_DIALOG_LIVEBROADCASTING };
    //The number of people supported within the channel
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
	afx_msg LRESULT OnEIDNetworkQuality(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT onEIDRtcStats(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT onEIDLocalAudioStats(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT onEIDLocalAudioStateChanged(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT onEIDRemoteAudioStats(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT onEIDRemoteAudioStateChanged(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT onEIDLocalVideoStats(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT onEIDLocalVideoStateChanged(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT onEIDRemoteVideoStats(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT onEIDRemoteVideoStateChanged(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT onEIDContentInspectResult(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT onEIDSnapshotTaken(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT onEIDVideoRenderingTracingResult(WPARAM wParam, LPARAM lParam);
private:
    //set control text from config.
    void InitCtrlText();
    //create all video window to save m_videoWnds.
    void CreateAllVideoWnds();
    //show all video window from m_videoWnds.
    void ShowVideoWnds();
    //render local video from SDK local capture.
    void RenderLocalVideo();
	//stop local video capture from SDK
	void StopLocalVideo();
	void ResetVideoView();


    IRtcEngine* m_rtcEngine = nullptr;
	VideoEncoderConfiguration m_videoEncoderConfig;
	AAudioDeviceManager* m_audioDeviceManager = nullptr;
    CLiveBroadcastingRtcEngineEventHandler m_eventHandler;
    bool m_joinChannel = false;
    bool m_initialize = false;
    //video wnd
    CAGVideoWnd m_videoWnds[VIDEO_COUNT];
    int m_maxVideoCount = 4;
    std::list<uid_t> m_lstUids;
	int m_uid = 0;
	int m_canvasColor = 0x000000ff;// rgba
	media::base::RENDER_MODE_TYPE m_canvasRenderMode = media::base::RENDER_MODE_HIDDEN;
	std::string m_imgPng;

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonJoinchannel();
	afx_msg void OnSelchangeListInfoBroadcasting();
	afx_msg void OnStnClickedStaticVideo();

	CComboBox m_cmbRole;
    CStatic m_staRole;
    CComboBox m_cmbPersons;
    CEdit m_edtChannelName;
    CEdit m_edtDetailInfo;
    CButton m_btnJoinChannel;
    CListBox m_lstInfo;
    CStatic m_videoArea;
    CStatic m_staPersons;
    CStatic m_staChannelName;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CComboBox m_cmbVideoEncoder;
	CButton m_chkReport;
	CButton m_chkModeration;
	CButton m_chkSnapshot;
	CButton m_chkBFrame;
	CButton m_chkFirstFrameOpt;
	CButton m_chkVideoImage;
	CButton m_rdiEncodeAuto;
	CButton m_rdiEncodeSoft;
	CButton m_rdiEncodeHard;
	CStatic m_staEncode;
	afx_msg void OnBnClickedCheckReport();
	afx_msg void OnBnClickedModeration();
	afx_msg void OnBnClickedSnapshot();
	afx_msg void OnBnClickedRadioEncoder(UINT idCtl);
	afx_msg void OnBnClickedCheckBFrame();
	afx_msg void OnBnClickedCheckFirstFrameOpt();
	afx_msg void OnBnClickedButtonPreload();
	int m_canvasRenderModeValue;
	afx_msg void OnBnClickedRadioCanvasRenderMode();
	afx_msg void OnNMCustomdrawSliderCanvasColor(NMHDR* pNMHDR, LRESULT* pResult);
	CStatic m_staCavasColor;
	CSliderCtrl m_sldCanvasColor;
	afx_msg void OnBnClickedCheckVideoImage();
	CSliderCtrl mLocalFpsSlider;
	CSliderCtrl mRemoteFpsSlider;
	afx_msg void OnNMCustomdrawSliderLocalfps(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMCustomdrawSliderRemotefps(NMHDR* pNMHDR, LRESULT* pResult);
};
