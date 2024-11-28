#pragma once

#include <IAgoraRtcEngine.h>
#include "AGVideoWnd.h"
#include "CDlgBeautyEx.h"

// CDlgBeauty 

class CBeautyEventHandler : public agora::rtc::IRtcEngineEventHandler
{
public:
	
	//set the message notify window handler
	void SetMsgReceiver(HWND hWnd) { m_hMsgHanlder = hWnd; }

	
	std::string GetChannelName() { return m_strChannel; }
	/*
	note:
		Join the channel callback.This callback method indicates that the client
		successfully joined the specified channel.Channel ids are assigned based
		on the channel name specified in the joinChannel. If IRtcEngine::joinChannel
		is called without a user ID specified. The server will automatically assign one
	parameters:
		channel:channel name.
		uid: user ID。If the UID is specified in the joinChannel, that ID is returned here;
		Otherwise, use the ID automatically assigned by the Agora server.
		elapsed: The Time from the joinChannel until this event occurred (ms).
	*/
	virtual void onJoinChannelSuccess(const char* channel, agora::rtc::uid_t uid, int elapsed) override {
		if (m_hMsgHanlder) {
			::PostMessage(m_hMsgHanlder, WM_MSGID(EID_JOINCHANNEL_SUCCESS), (WPARAM)uid, 0);
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
	virtual void onLeaveChannel(const agora::rtc::RtcStats& stats) override {
		if (m_hMsgHanlder) {
			::PostMessage(m_hMsgHanlder, WM_MSGID(EID_LEAVE_CHANNEL), 0,0);
		}
	}
private:
	HWND m_hMsgHanlder = NULL;
	std::string m_strChannel = "";
	
};
class CDlgBeauty : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgBeauty)

public:
	CDlgBeauty(CWnd* pParent = nullptr);   // 
	virtual ~CDlgBeauty();

// 

	enum { IDD = IDD_DIALOG_BEAUTY };


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 

	DECLARE_MESSAGE_MAP()
private:
	void SetBeauty();
	void SetColorful();
	bool m_initialize = false;
	bool m_joinChannel = false;
	agora::rtc::IRtcEngine* m_rtcEngine = nullptr;
	CBeautyEventHandler m_eventHandler;
	CAGVideoWnd m_videoWnds[2];
	CDlgBeautyEx* mBeautyDlgEx;
public:
	afx_msg void OnBnClickedButtonJoinchannel();
	LRESULT OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam);
	LRESULT OnEIDLeaveChannel(WPARAM wParam, LPARAM lParam);
	//Initialize the Agora SDK
	bool InitAgora();
	//UnInitialize the Agora SDK
	void UnInitAgora();
	// resume window status.
	void ResumeStatus();
	//Initialize the Ctrl Text.
	void InitCtrlText();

	void UpdateExtentCbState(bool isCheck);

	CStatic m_staChannel;
	CEdit m_edtChannel;
	CButton m_btnJoinChannel;
	CButton m_chkBeauty;
	CStatic m_staContrast;
	CStatic m_staRedness;
	CStatic m_staSmoothness;
	CStatic m_staLightening;
	CButton m_chkEnhance;
	CSliderCtrl m_sldLength;
	CStatic m_staSkin;
	CSliderCtrl m_sldSkin;
	CButton m_chkVideoDenoise;
	CButton m_chkVirtual;
	CButton m_chkLowlight;
	CSliderCtrl m_sdlLightening;
	CSliderCtrl m_sldRedness;
	CSliderCtrl m_sldSmoothness;
	CListBox m_lstInfo;
	CStatic m_staVideoArea;
	CButton m_radioVirtualBgBlur;
	CButton m_radioVirtualBgColor;
	CButton m_radioVirtualBgImage;
	CButton m_radioVirtualBgVideo;
	std::string m_imgPath;
	
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedCheckBeautyEnable();
	CComboBox m_cmbContrast;
	afx_msg void OnNMCustomdrawSliderRedness(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnThumbposchangingSliderLightening(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSliderSmoothness(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSelchangeComboBeauteLighteningContrastLevel();
	afx_msg void OnBnClickedCheckEnhance();
	afx_msg void OnCustomdrawSliderStrength(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCustomdrawSliderSkinProtect(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedCheckVideoDenoise();
	afx_msg void OnBnClickedCheckVideoDenoise2();
	afx_msg void OnBnClickedCheckLowlight();

	afx_msg void OnBnClickedStaticVirtualBgChoose(UINT idCtl);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	CSliderCtrl mSliderBright;
	afx_msg void OnNMCustomdrawSliderBrihtness(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedCheckExtention();
	CButton mCkExtention;
};
