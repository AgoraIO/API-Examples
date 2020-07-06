
// APIExampleDlg.h : header file
//

#pragma once
#include "Basic/LiveBroadcasting/CLiveBroadcastingDlg.h"
#include "Advanced/RTMPinject/AgoraRtmpInjectionDlg.h"
#include "Advanced/RTMPStream/AgoraRtmpStreaming.h"
#include "Advanced/VideoMetadata/CAgoraMetaDataDlg.h"
#include "Advanced/ScreenShare/AgoraScreenCapture.h"
#include "Advanced/CustomAudioCapture/CAgoraCaptureAudioDlg.h"
#include "Advanced/CustomVideoCapture/CAgoraCaptureVideoDlg.h"


#include <vector>
#include <map>
const int MAIN_AREA_BOTTOM = 15;

// CAPIExampleDlg dialog
class CAPIExampleDlg : public CDialogEx
{
// Construction
public:
	CAPIExampleDlg(CWnd* pParent = nullptr);	// standard constructor

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
    CAgoraRtmpInjectionDlg      *m_pRtmpInjectDlg    = nullptr;
    CAgoraRtmpStreamingDlg      *m_pRtmpStreamingDlg = nullptr;
    CAgoraMetaDataDlg           *m_pVideoSEIDlg      = nullptr;
    CAgoraScreenCapture         *m_pScreenCap        = nullptr;
    CAgoraCaptureVideoDlg       *m_pCaputreVideoDlg  = nullptr;
    CAgoraCaptureAduioDlg       *m_pCaptureAudioDlg  = nullptr;


    CString m_preSelectedItemText = _T("");
    std::vector<CString> m_vecBasic, m_vecAdvanced;
    


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
    CStatic m_stalstInfo;
    afx_msg void OnBnClickedButtonDocumentWebsite();
    CStatic m_staAdvancedScene;
    CStatic m_grpBasicScene;
    CButton m_btnDemoWebSite;
    CButton m_btnRegister;
    CButton m_btnFAQ;
    CButton m_btnDocWebsite;
    CStatic m_grpDoc;
};
