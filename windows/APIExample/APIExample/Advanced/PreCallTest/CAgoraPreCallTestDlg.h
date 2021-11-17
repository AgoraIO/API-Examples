#pragma once
#include "AGVideoTestWnd.h"

class CAgoraPreCallTestEvnetHandler :public IRtcEngineEventHandler
{
public:
	void SetMsgReceiver(HWND hWnd) { m_hMsgHanlder = hWnd; }

	 /** Reports which users are speaking, the speakers' volume and whether the local user is speaking.
	 This callback reports the IDs and volumes of the loudest speakers (at most 3 users) at the moment in the channel, and whether the local user is speaking.
	 By default, this callback is disabled. You can enable it by calling the \ref IRtcEngine::enableAudioVolumeIndication(int, int, bool) "enableAudioVolumeIndication" method.
	 Once enabled, this callback is triggered at the set interval, regardless of whether a user speaks or not.
	 The SDK triggers two independent `onAudioVolumeIndication` callbacks at one time, which separately report the volume information of the local user and all the remote speakers.
	 For more information, see the detailed parameter descriptions.
	 @note
	 - To enable the voice activity detection of the local user, ensure that you set `report_vad`(true) in the `enableAudioVolumeIndication` method.
	 - Calling the \ref agora::rtc::IRtcEngine::muteLocalAudioStream "muteLocalAudioStream" method affects the SDK's behavior:
		- If the local user calls the \ref agora::rtc::IRtcEngine::muteLocalAudioStream "muteLocalAudioStream" method, the SDK stops triggering the local user's callback.
		- 20 seconds after a remote speaker calls the *muteLocalAudioStream* method, the remote speakers' callback excludes this remote user's information; 20 seconds after all remote users call the *muteLocalAudioStream* method, the SDK stops triggering the remote speakers' callback.
	 - An empty @p speakers array in the *onAudioVolumeIndication* callback suggests that no remote user is speaking at the moment.
	 @param speakers A pointer to AudioVolumeInfo:
	 - In the local user's callback, this struct contains the following members:
	   - `uid` = 0,
	   - `volume` = `totalVolume`, which reports the sum of the voice volume and audio-mixing volume of the local user, and
	   - `vad`, which reports the voice activity status of the local user.
	 - In the remote speakers' callback, this array contains the following members:
	   - `uid` of the remote speaker,
	   - `volume`, which reports the sum of the voice volume and audio-mixing volume of each remote speaker, and
	   - `vad` = 0.
	   An empty speakers array in the callback indicates that no remote user is speaking at the moment.
	 @param speakerNumber Total number of speakers. The value range is [0, 3].
	 - In the local user's callback, `speakerNumber` = 1, regardless of whether the local user speaks or not.
	 - In the remote speakers' callback, the callback reports the IDs and volumes of the three loudest speakers when there are more than three remote users in the channel, and `speakerNumber` = 3.
	 @param totalVolume Total volume after audio mixing. The value ranges between 0 (lowest volume) and 255 (highest volume).
	 - In the local user's callback, `totalVolume` is the sum of the voice volume and audio-mixing volume of the local user.
	 - In the remote speakers' callback, `totalVolume` is the sum of the voice volume and audio-mixing volume of all the remote speakers.
	 */
	virtual void onAudioVolumeIndication(const AudioVolumeInfo* speakers, unsigned int speakerNumber, int totalVolume) {
		if (m_hMsgHanlder)
			::PostMessage(m_hMsgHanlder, WM_MSGID(EID_AUDIO_VOLUME_INDICATION), totalVolume, 0);
	}

	/** Reports the last mile network quality of the local user once every two seconds before the user joins the channel.
	 Last mile refers to the connection between the local device and Agora's edge server. After the application calls the \ref IRtcEngine::enableLastmileTest "enableLastmileTest" method, this callback reports once every two seconds the uplink and downlink last mile network conditions of the local user before the user joins the channel.
	 @param quality The last mile network quality: #QUALITY_TYPE.
	 */
	void onLastmileQuality(int quality) {
		if (m_hMsgHanlder)
			::PostMessage(m_hMsgHanlder, WM_MSGID(EID_LASTMILE_QUAILTY), quality, 0);
	}

	/** Reports the last-mile network probe result.
   The SDK triggers this callback within 30 seconds after the app calls the \ref agora::rtc::IRtcEngine::startLastmileProbeTest "startLastmileProbeTest" method.
   @param result The uplink and downlink last-mile network probe test result. See LastmileProbeResult.
   */
	void onLastmileProbeResult(LastmileProbeResult) {
		if (m_hMsgHanlder)
			::PostMessage(m_hMsgHanlder, WM_MSGID(EID_LASTMILE_PROBE_RESULT), 0,0);
	}
private:
	HWND m_hMsgHanlder;
};


class CAgoraPreCallTestDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAgoraPreCallTestDlg)

public:
	CAgoraPreCallTestDlg(CWnd* pParent = nullptr);  
	virtual ~CAgoraPreCallTestDlg();

	enum { IDD = IDD_DIALOG_PERCALL_TEST };

	//Initialize the Ctrl Text.
	void InitCtrlText();
	//Initialize the Agora SDK
	bool InitAgora();
	//UnInitialize the Agora SDK
	void UnInitAgora();
	//resume window status
	void ResumeStatus();
	
	void UpdateViews();
	 

private:
	
	IRtcEngine* m_rtcEngine;
	CImageList m_imgNetQuality;
	int m_netQuality;
	CAGVideoTestWnd m_VideoTest;
	CAgoraPreCallTestEvnetHandler m_eventHandler;
	AAudioDeviceManager * m_audioDeviceManager;
	AVideoDeviceManager * m_videoDeviceManager;
	std::map<CString, std::string> m_mapAudioInput;
	std::map<CString, std::string> m_mapAudioOutput;
	std::map<CString, std::string> m_mapCamera;
	bool m_audioInputTest;
	bool m_audioOutputTest;
	bool m_cameraTest;
	bool m_echoTest;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);  
	LRESULT afx_msg OnEIDLastmileQuality(WPARAM wparam,LPARAM lparam);
	LRESULT afx_msg OnEIDLastmileProbeResult(WPARAM wparam, LPARAM lparam);
	LRESULT afx_msg OnEIDAudioVolumeIndication(WPARAM wparam, LPARAM lparam);

	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButtonAudioInputTest();
	afx_msg void OnBnClickedButtonAudioOutputTest();
	afx_msg void OnBnClickedButtonCamera();
	afx_msg void OnSelchangeListInfoBroadcasting();
	afx_msg void OnReleasedcaptureSliderInputVol(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnReleasedcaptureSliderOutputVol(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
public:
	CStatic m_staAudioInput;
	CStatic m_staAudioInputVol;
	CStatic m_staAudioOutput;
	CStatic m_staAudioOutputVol;
	CStatic m_staVideo;
	CComboBox m_cmbVideo;
	CComboBox m_cmbAudioInput;
	CComboBox m_cmbAudioOutput;
	CSliderCtrl m_sldAudioInputVol;
	CSliderCtrl m_sldAudioOutputVol;
	CButton m_btnAudioInputTest;
	CButton m_btnAudioOutputTest;
	CButton m_btnVideoTest;
	CButton m_btnEchoTest1;
	CButton m_btnEchoTest2;
	CStatic m_staVideoArea;
	CListBox m_lstInfo;
	CStatic m_staDetails;
	afx_msg void OnBnClickedButtonCamera2();
	afx_msg void OnEchoTest1();
	afx_msg void OnEchoTest2();
};
