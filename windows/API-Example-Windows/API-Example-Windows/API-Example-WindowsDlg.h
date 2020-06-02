
// API-Example-WindowsDlg.h : header file
//

#pragma once
#include <unordered_map>
#include <map>
// CAPIExampleWindowsDlg dialog
class CAPIExampleWindowsDlg : public CDialogEx
{
// Construction
public:
	CAPIExampleWindowsDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_APIEXAMPLEWINDOWS_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
    void InitItems();

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
    afx_msg void OnTvnSelchangedTree2(NMHDR *pNMHDR, LRESULT *pResult);
private:
    CTreeCtrl m_treeCtrl;
    std::map<CString, std::vector<CString> > mapItems;
public:
    afx_msg void OnDblclkTreeInfo(NMHDR *pNMHDR, LRESULT *pResult);
};
