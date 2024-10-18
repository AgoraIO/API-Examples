
// APIExampleDlg.h : header file
//

#pragma once
#include "Basic/LiveBroadcasting/CLiveBroadcastingDlg.h"
#include "Basic/JoinChannelVideoByToken/CJoinChannelVideoByTokenDlg.h"
#include "Advanced/RTMPinject/AgoraRtmpInjectionDlg.h"
#include "Advanced/RTMPStream/AgoraRtmpStreaming.h"
#include "Advanced/Metadata/CAgoraMetaDataDlg.h"
#include "Advanced/ScreenShare/AgoraScreenCapture.h"
#include "Advanced/CustomAudioCapture/CAgoraCaptureAudioDlg.h"
#include "Advanced/CustomVideoCapture/CAgoraCaptureVideoDlg.h"
#include "Advanced/AudioProfile/CAgoraAudioProfile.h"
#include "Advanced/AudioMixing/CAgoraAudioMixingDlg.h"
#include "Advanced/OriginalVideo/CAgoraOriginalVideoDlg.h"
#include "Advanced/OriginalAudio/CAgoraOriginalAudioDlg.h"
#include "Advanced/CustomEncrypt/CAgoraCustomEncryptDlg.h"
#include "Advanced/MultiChannel/CAgoraMultiChannelDlg.h"
#include "Advanced/MultiVideoSource/CAgoraMutilVideoSourceDlg.h"
#include "Advanced/PreCallTest/CAgoraPreCallTestDlg.h"
#include "Advanced/AudioVolume/CAgoraAudioVolumeDlg.h"
#include "Advanced/ReportInCall/CAgoraReportInCallDlg.h"
#include "Advanced/RegionConn/CAgoraRegionConnDlg.h"
#include "Advanced/MediaEncrypt/CAgoraMediaEncryptDlg.h"
#include "Advanced/AudioEffect/CAgoraEffectDlg.h"
#include "Advanced/MediaPlayer/CAgoraMediaPlayer.h"
#include "Advanced/LocalVideoTranscoding/CLocalVideoTranscodingDlg.h"
#include "Advanced/Beauty/CDlgBeauty.h"
#include "Advanced/BeautyAudio/CAgoraBeautyAudio.h"
#include "Advanced/SpatialAudio/CAgoraSpatialAudioDlg.h"
#include "Advanced/MediaRecorder/CAgoraMediaRecorder.h"
#include "Advanced/PushExternalVideoYUV/PushExternalVideoYUV.h"
#include "Advanced/MultiVideoSourceTracks/MultiVideoSourceTracks.h"
#include "Advanced/FaceCapture/CAgoraFaceCaptureDlg.h"
#include "Advanced/TransparentBg/TransparentBgDialog.h"
#include "Advanced/RtePlayer/RtePlayerDlg.h"
#include "Advanced/CrossChannel/CAgoraCrossChannelDlg.h"
#include <mutex>
#include <vector>
#include <map>
const int MAIN_AREA_BOTTOM = 15;

// CAPIExampleDlg dialog
class CAPIExampleDlg : public CDialogEx
{
// Construction
public:
	CAPIExampleDlg(CWnd* pParent = nullptr);	// standard constructor
	~CAPIExampleDlg();
// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_APIEXAMPLE_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedButtonFaq();
    afx_msg void OnBnClickedButtonRegister();
    afx_msg void OnBnClickedButtonDemo();
    afx_msg LRESULT OnEIDJoinLeaveChannel(WPARAM wParam, LPARAM lParam);

private:
    void InitSceneDialog();
    void InitSceneList();
    void InitCtrlText();
    HTREEITEM GetHitItem(NMHDR *pNMHDR);

    void ReleaseScene(CTreeCtrl& treeScene, HTREEITEM& hSelectItem);
    void CreateScene(CTreeCtrl& treeScene, CString selectedText);
    CLiveBroadcastingDlg        *m_pLiveBroadcasting = nullptr;
    CJoinChannelVideoByTokenDlg *m_pJoinChannelVideoByTokenDlg = nullptr;
    CAgoraRtmpInjectionDlg      *m_pRtmpInjectDlg    = nullptr;
    CAgoraRtmpStreamingDlg      *m_pRtmpStreamingDlg = nullptr;
    CAgoraMetaDataDlg           *m_pVideoSEIDlg      = nullptr;
    CAgoraScreenCapture         *m_pScreenCap        = nullptr;
    CAgoraCaptureVideoDlg       *m_pCaputreVideoDlg  = nullptr;
    CAgoraCaptureAduioDlg       *m_pCaptureAudioDlg  = nullptr;
	CAgoraAudioProfile			*m_pAudioProfileDlg  = nullptr;
	CAgoraAudioMixingDlg		*m_pAudioMixingDlg   = nullptr;
	CAgoraOriginalVideoDlg		*m_pOriginalVideoDlg = nullptr;
	CAgoraOriginalAudioDlg		*m_pOriginalAudioDlg = nullptr;
	CAgoraCustomEncryptDlg		*m_pCustomEncryptDlg = nullptr;
	CAgoraMultiChannelDlg		*m_pMultiChannelDlg   = nullptr;
	CAgoraMutilVideoSourceDlg	*m_pMultiVideoSourceDlg = nullptr;
	CAgoraPreCallTestDlg		*m_pPerCallTestDlg		= nullptr;
	CAgoraReportInCallDlg		*m_pReportInCallDlg		= nullptr;
	CAgoraRegionConnDlg			*m_pRegionConnDlg		= nullptr;
	CAgoraMediaEncryptDlg		*m_pMediaEncrptDlg		= nullptr;
	CAgoraEffectDlg				*m_pEffectDlg			= nullptr;
	CAgoraMediaPlayer			*m_pmediaPlayerDlg = nullptr;
	CAgoraMediaRecorder			*m_pmediaRecorderDlg = nullptr;
	CAgoraAudioVolumeDlg		*m_pAudioVolumeDlg = nullptr;
	CAgoraBeautyAudio           *m_pDlgBeautyAudio = nullptr;
	CLocalVideoTranscodingDlg   *m_pLocalVideoTranscodingDlg = nullptr;
	CAgoraSpatialAudioDlg       *m_pSpatialAudioDlg = nullptr;
    PushExternalVideoYUV        *m_pPushExternalVideoYUV = nullptr;
    MultiVideoSourceTracks      *m_pMultiVideoSourceTracks = nullptr;
    CAgoraFaceCaptureDlg        *m_pAgoraFaceCaptureDlg = nullptr;
    CAgoraCrossChannelDlg       *m_pAgoraCrossChannelDlg = nullptr;
	CDlgBeauty * m_pDlgBeauty = nullptr;
    CTransparentBgDlg* m_TransparentDlg = nullptr;
    CRtePlayerDlg* m_RtePlayer = nullptr;
    CString m_preSelectedItemText = _T("");
    std::vector<CString> m_vecBasic, m_vecAdvanced;
	std::mutex m_mutex;
	
    BOOL m_bJoinChannel = FALSE;
public:
    CStatic m_staMainArea;
    CTreeCtrl m_lstBasicScene;
    CTreeCtrl m_lstAdvanced;
  
    afx_msg void OnClickListBasic(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnSelchangedListAdvanced(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnClickListAdvanced(NMHDR *pNMHDR, LRESULT *pResult);
   
    afx_msg void OnSelchangedListBasic(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnSelchangingListBasic(NMHDR *pNMHDR, LRESULT *pResult);
    
    afx_msg void OnSelchangingListAdvanced(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnBnClickedButtonDocumentWebsite();
	CStatic m_stalstInfo;
    CStatic m_staAdvancedScene;
    CStatic m_grpBasicScene;
    CButton m_btnDemoWebSite;
    CButton m_btnRegister;
    CButton m_btnFAQ;
    CButton m_btnDocWebsite;
    CStatic m_grpDoc;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnClose();
};

