
#include"AGVideoWnd.h"
// AgoraScreenCapture 对话框

class CScreenCaputreEventHandler : public IRtcEngineEventHandler
{
public:
	CScreenCaputreEventHandler() {}
	~CScreenCaputreEventHandler() {}
	void SetMsgReceiver(HWND hWnd) { m_hMsgHanlder = hWnd; }
	virtual void onJoinChannelSuccess(const char* channel, uid_t uid, int elapsed) override;
	virtual void onUserJoined(uid_t uid, int elapsed) override;
	virtual void onUserOffline(uid_t uid, USER_OFFLINE_REASON_TYPE reason) override;
	virtual void onLeaveChannel(const RtcStats& stats) override;
	virtual void onRemoteVideoStateChanged(uid_t uid, REMOTE_VIDEO_STATE state, REMOTE_VIDEO_STATE_REASON reason, int elapsed) override;
private:
	HWND m_hMsgHanlder;
};


class CAgoraScreenCapture : public CDialogEx
{
	DECLARE_DYNAMIC(CAgoraScreenCapture)

public:
	CAgoraScreenCapture(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CAgoraScreenCapture();

	bool InitAgora();
	void UnInitAgora();
	void RenderLocalVideo();
	int	RefreashWndInfo();

// 对话框数据
	enum { IDD = IDD_DIALOG_SCREEN_SHARE };
	afx_msg LRESULT OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEIDLeaveChannel(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEIDUserJoined(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEIDUserOffline(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEIDRemoteVideoStateChanged(WPARAM wParam, LPARAM lParam);
	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	static BOOL CALLBACK WndEnumProc(HWND hWnd, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
	CAGVideoWnd m_localVideoWnd;
	CAGVideoWnd m_remoteVideoWnd;
	CList<HWND>	m_listWnd;
	IRtcEngine* m_rtcEngine = nullptr;
	CScreenCaputreEventHandler m_eventHandler;
	bool joinChannel = false;
	bool m_initialize = false;
	std::string m_injectUrl;
	bool m_addInjectStream = false;
	bool m_screenShare = false;

public:
	void InitCtrlText();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonJoinchannel();
	afx_msg void OnBnClickedButtonStartShare();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	void ReFreshWnd();

	CStatic m_staVideoArea;
	CStatic m_staChannel;
	CEdit m_edtChannel;
	CStatic m_staScreenCap;
	CComboBox m_cmbScreenCap;
	CButton m_btnStartCap;
	CButton m_btnJoinChannel;
	CListBox m_lstInfo;
};
