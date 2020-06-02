
// API-Example-WindowsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "API-Example-Windows.h"
#include "API-Example-WindowsDlg.h"
#include "afxdialogex.h"
#include "CAgora1V1Dlg.h"
#include "CAgoraRtmpInjectionDlg.h"
#include "CAgoraRtmpStreamingDlg.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CAPIExampleWindowsDlg dialog



CAPIExampleWindowsDlg::CAPIExampleWindowsDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_APIEXAMPLEWINDOWS_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAPIExampleWindowsDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_TREE_INFO, m_treeCtrl);
}

BEGIN_MESSAGE_MAP(CAPIExampleWindowsDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_NOTIFY(TVN_SELCHANGED, IDC_TREE2, &CAPIExampleWindowsDlg::OnTvnSelchangedTree2)
    ON_NOTIFY(NM_DBLCLK, IDC_TREE_INFO, &CAPIExampleWindowsDlg::OnDblclkTreeInfo)
END_MESSAGE_MAP()


// CAPIExampleWindowsDlg message handlers

BOOL CAPIExampleWindowsDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

    InitItems();
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CAPIExampleWindowsDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CAPIExampleWindowsDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CAPIExampleWindowsDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CAPIExampleWindowsDlg::OnTvnSelchangedTree2(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
    // TODO: Add your control notification handler code here
    *pResult = 0;
}

void CAPIExampleWindowsDlg::InitItems()
{
    std::vector<CString> vecSubItems;
    //Basic Audio Video
    vecSubItems.push_back(L"1V1");
    mapItems.insert(std::make_pair(L"Baisc Audio Video", vecSubItems));

    //Live Broadcasting
    vecSubItems.clear();
    vecSubItems.push_back(L"RTMP Injection");
    vecSubItems.push_back(L"RTMP Streaming");
    mapItems.insert(std::make_pair(L"Live Streaming", vecSubItems));
   
    for (auto iter = mapItems.begin(); iter != mapItems.end(); ++iter){
        TVINSERTSTRUCT tvInsert;
        tvInsert.hParent = NULL;
        tvInsert.hInsertAfter = NULL;
        tvInsert.item.mask = TVIF_TEXT;
        tvInsert.item.pszText = iter->first.AllocSysString();
        HTREEITEM hMainItem = m_treeCtrl.InsertItem(&tvInsert);
        SysFreeString(tvInsert.item.pszText);
        m_treeCtrl.SetItemState(hMainItem, TVIS_BOLD, TVIS_BOLD);

        std::vector<CString> vecSubItems = iter->second;
        for (size_t i = 0; i < vecSubItems.size(); ++i){
            HTREEITEM hSubItem = m_treeCtrl.InsertItem(TVIF_TEXT,
                vecSubItems[i], 0, 0, 0, 0, 0, hMainItem, NULL);
        }
        m_treeCtrl.Expand(hMainItem, TVE_EXPAND);
    }
}

void CAPIExampleWindowsDlg::OnDblclkTreeInfo(NMHDR *pNMHDR, LRESULT *pResult)
{
    if (strlen(APP_ID) == 0) {
        AfxMessageBox(_T("APP_ID is empty, define APP_ID in stdafx.h"));
        return ;
    }
    // TODO: Add your control notification handler code here
    *pResult = 0;

    HTREEITEM hItem = m_treeCtrl.GetSelectedItem();

    if (!m_treeCtrl.ItemHasChildren(hItem)) {
        CString itemText = m_treeCtrl.GetItemText(hItem);
        if (itemText.Compare(_T("1V1")) == 0) {
            ShowWindow(SW_HIDE);
            CAgora1V1Dlg dlg;
            dlg.DoModal();
            ShowWindow(SW_SHOW);
        }
        else if (itemText.Compare(_T("RTMP Injection")) == 0) {
            ShowWindow(SW_HIDE);
            CAgoraRtmpInjectionDlg injectDlg;
            injectDlg.DoModal();
            ShowWindow(SW_SHOW);
        }
        else if (itemText.Compare(_T("RTMP Streaming")) == 0) {
            ShowWindow(SW_HIDE);
            CAgoraRtmpStreamingDlg dlg;
            dlg.DoModal();
            ShowWindow(SW_SHOW);
        }
    }
}
