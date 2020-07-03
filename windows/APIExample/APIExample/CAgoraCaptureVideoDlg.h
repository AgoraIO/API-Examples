#pragma once
#include "AGVideoWnd.h"
#include "DreictShow/AgVideoBuffer.h"
#include "DreictShow/AGDShowVideoCapture.h"


typedef struct _VIDEO_BUFFER {
	BYTE m_lpImageBuffer[VIDEO_BUF_SIZE];
	int  timestamp;						
}VIDEO_BUFFER, *PVIDEO_BUFFER;

class CExtendVideoFrameObserver :
	public agora::media::IVideoFrameObserver
{
public:
	CExtendVideoFrameObserver() { m_lpBuffer = new BYTE[VIDEO_BUF_SIZE]; }
	virtual ~CExtendVideoFrameObserver() { if(m_lpBuffer)delete[]m_lpBuffer; }
	virtual bool onCaptureVideoFrame(VideoFrame& videoFrame);
	virtual bool onRenderVideoFrame(unsigned int uid, VideoFrame& videoFrame);

private:
	LPBYTE				m_lpImageBuffer;
	LPBYTE				m_lpY;
	LPBYTE				m_lpU;
	LPBYTE				m_lpV;
	VIDEO_BUFFER		m_videoBuffer;
	BYTE				* m_lpBuffer;
};


class CAgoraCaptureVideoDlgEngineEventHandler : public IRtcEngineEventHandler {
public:
	void SetMsgReceiver(HWND hWnd) { m_hMsgHanlder = hWnd; }
	virtual void onJoinChannelSuccess(const char* channel, uid_t uid, int elapsed) override;
	virtual void onUserJoined(uid_t uid, int elapsed) override;
	virtual void onUserOffline(uid_t uid, USER_OFFLINE_REASON_TYPE reason) override;
	virtual void onLeaveChannel(const RtcStats& stats) override;
	virtual void onRemoteVideoStateChanged(uid_t uid, REMOTE_VIDEO_STATE state, REMOTE_VIDEO_STATE_REASON reason, int elapsed) override;
private:
	HWND m_hMsgHanlder;
};


class CAgoraCaptureVideoDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAgoraCaptureVideoDlg)

public:
	LRESULT OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam);
	LRESULT OnEIDLeaveChannel(WPARAM wParam, LPARAM lParam);
	LRESULT OnEIDUserJoined(WPARAM wParam, LPARAM lParam);
	LRESULT OnEIDUserOffline(WPARAM wParam, LPARAM lParam);
	LRESULT OnEIDRemoteVideoStateChanged(WPARAM wParam, LPARAM lParam);
	CAgoraCaptureVideoDlg(CWnd* pParent = nullptr); 
	virtual ~CAgoraCaptureVideoDlg();

	bool InitAgora();
	void UnInitAgora();


enum { IDD = IDD_DIALOG_CUSTOM_CAPTURE_VIDEO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX); 
	BOOL EnableExtendVideoCapture(BOOL bEnable);

	void InitCtrlText();
	void RenderLocalVideo();
	IRtcEngine* m_rtcEngine = nullptr;
	CAgoraCaptureVideoDlgEngineEventHandler m_eventHandler;
	CExtendVideoFrameObserver m_extVideoFrameObserver;
	CAGDShowVideoCapture m_agVideoCaptureDevice;

	bool joinChannel = false;
	bool m_initialize = false;
	bool m_remoteJoined = false;
	bool m_extenalCaptureVideo = false;
	CAGVideoWnd m_localVideoWnd;
	CAGVideoWnd m_remoteVideoWnd;
	DECLARE_MESSAGE_MAP()
public:
	CStatic m_staVideoArea;
	CStatic m_staChannelName;
	CStatic m_staCaputreVideo;
	CEdit m_edtChannel;
	CButton m_btnJoinChannel;
	CButton m_btnSetExtCapture;
	CComboBox m_cmbVideoDevice;
	CComboBox m_cmbVideoType;
	CListBox m_lstInfo;
	virtual BOOL OnInitDialog();
	
	afx_msg	void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnClickedButtonStartCaputre();
	afx_msg void OnClickedButtonJoinchannel();
	afx_msg void OnSelchangeComboCaptureVideoDevice();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
