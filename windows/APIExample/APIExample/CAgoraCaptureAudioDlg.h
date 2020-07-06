#pragma once
#include "AGVideoWnd.h"
#include "DreictShow/AGDShowAudioCapture.h"
#include "IAgoraMediaEngine.h"

class CExtendAudioFrameObserver :
	public agora::media::IAudioFrameObserver
{
public:
	virtual bool onRecordAudioFrame(AudioFrame& audioFrame);
	virtual bool onPlaybackAudioFrame(AudioFrame& audioFrame);
	virtual bool onMixedAudioFrame(AudioFrame& audioFrame);
	virtual bool onPlaybackAudioFrameBeforeMixing(unsigned int uid, AudioFrame& audioFrame);
};


class CAgoraCaptureAduioDlgEngineEventHandler : public IRtcEngineEventHandler {
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




class CAgoraCaptureAduioDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAgoraCaptureAduioDlg)

public:
	LRESULT OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam);
	LRESULT OnEIDLeaveChannel(WPARAM wParam, LPARAM lParam);
	LRESULT OnEIDUserJoined(WPARAM wParam, LPARAM lParam);
	LRESULT OnEIDUserOffline(WPARAM wParam, LPARAM lParam);
	LRESULT OnEIDRemoteVideoStateChanged(WPARAM wParam, LPARAM lParam);
	
	CAgoraCaptureAduioDlg(CWnd* pParent = nullptr);   
	virtual ~CAgoraCaptureAduioDlg();

	bool InitAgora();
	void UnInitAgora();

	void InitCtrlText();
	void RenderLocalVideo();
	IRtcEngine* m_rtcEngine = nullptr;
	CAgoraCaptureAduioDlgEngineEventHandler		m_eventHandler;
	CAGDShowAudioCapture						m_agAudioCaptureDevice;
	CExtendAudioFrameObserver					m_extAudioObserver;
	bool joinChannel = false;
	bool m_initialize = false;
	bool m_remoteJoined = false;
	bool m_extenalCaptureAudio = false;
	CAGVideoWnd m_localVideoWnd;
	CAGVideoWnd m_remoteVideoWnd;

	enum { IDD = IDD_DIALOG_CUSTOM_CAPTURE_AUDIO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonJoinchannel();
	afx_msg void OnBnClickedButtonStartCaputre();
	BOOL EnableExtendAudioCapture(BOOL bEnable);
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	
	CButton m_btnJoinChannel;
	CButton m_btnSetAudioCtx;
	CComboBox m_cmbAudioDevice;
	CComboBox m_cmbAudioType;
	CStatic m_staAudioDevice;
	CStatic m_staChannel;
	CEdit m_edtChannel;
	CStatic m_staVideoArea;
	CListBox m_lstInfo;
	afx_msg void OnSelchangeComboCaptureAudioDevice();
};
