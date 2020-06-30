
// APIExampleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "APIExample.h"
#include "APIExampleDlg.h"
#include "afxdialogex.h"

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


// CAPIExampleDlg dialog



CAPIExampleDlg::CAPIExampleDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_APIEXAMPLE_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAPIExampleDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_STATIC_MAIN, m_staMainArea);
    DDX_Control(pDX, IDC_LIST_BASIC, m_lstBasicScene);
    DDX_Control(pDX, IDC_LIST_ADVANCED, m_lstAdvanced);
    DDX_Control(pDX, IDC_STATIC_LIST_INFO, m_stalstInfo);
    DDX_Control(pDX, IDC_STATIC_ADVANCE, m_staAdvancedScene);
    DDX_Control(pDX, IDC_STATIC_GROUP_LIST, m_grpBasicScene);
    DDX_Control(pDX, IDC_BUTTON_DEMO, m_btnDemoWebSite);
    DDX_Control(pDX, IDC_BUTTON_REGISTER, m_btnRegister);
    DDX_Control(pDX, IDC_BUTTON_FAQ, m_btnFAQ);
    DDX_Control(pDX, IDC_BUTTON_DOCUMENT_WEBSITE, m_btnDocWebsite);
    DDX_Control(pDX, IDC_STATIC_GROUP_DOC, m_grpDoc);
}

BEGIN_MESSAGE_MAP(CAPIExampleDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_BUTTON_FAQ, &CAPIExampleDlg::OnBnClickedButtonFaq)
    ON_BN_CLICKED(IDC_BUTTON_REGISTER, &CAPIExampleDlg::OnBnClickedButtonRegister)
    ON_BN_CLICKED(IDC_BUTTON_DEMO, &CAPIExampleDlg::OnBnClickedButtonDemo)

    ON_NOTIFY(NM_CLICK, IDC_LIST_BASIC, &CAPIExampleDlg::OnClickListBasic)
    ON_NOTIFY(NM_CLICK, IDC_LIST_ADVANCED, &CAPIExampleDlg::OnClickListAdvanced)
    ON_NOTIFY(TVN_SELCHANGED, IDC_LIST_ADVANCED, &CAPIExampleDlg::OnSelchangedListAdvanced)
    ON_NOTIFY(TVN_SELCHANGED, IDC_LIST_BASIC, &CAPIExampleDlg::OnSelchangedListBasic)
    ON_NOTIFY(TVN_SELCHANGING, IDC_LIST_BASIC, &CAPIExampleDlg::OnSelchangingListBasic)
    ON_MESSAGE(WM_MSGID(EID_JOINCHANNEL_SUCCESS), &CAPIExampleDlg::OnEIDJoinLeaveChannel)
    ON_NOTIFY(TVN_SELCHANGING, IDC_LIST_ADVANCED, &CAPIExampleDlg::OnSelchangingListAdvanced)
    ON_BN_CLICKED(IDC_BUTTON_DOCUMENT_WEBSITE, &CAPIExampleDlg::OnBnClickedButtonDocumentWebsite)
END_MESSAGE_MAP()


// CAPIExampleDlg message handlers

BOOL CAPIExampleDlg::OnInitDialog()
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

	// TODO: Add extra initialization here
    InitCtrlText();
    InitSceneDialog();
    InitSceneList();
    
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CAPIExampleDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CAPIExampleDlg::OnPaint()
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
HCURSOR CAPIExampleDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CAPIExampleDlg::InitCtrlText()
{
    m_staAdvancedScene.SetWindowText(commonAdvanceScene);
    m_grpDoc.SetWindowText(commonGroupDoc);
    m_grpBasicScene.SetWindowText(commonBasicScene);
    m_btnFAQ.SetWindowText(commonFAQWebsite);
    m_btnRegister.SetWindowText(commonRegisterWebsite);
    m_btnDemoWebSite.SetWindowText(commonDemoWebsite);
    m_btnDocWebsite.SetWindowText(commonDocumentWebsite);
}

void CAPIExampleDlg::InitSceneDialog()
{
    //basic list
    m_vecBasic.push_back(basicLiveBroadcasting);

   m_pLiveBroadcasting = new CLiveBroadcastingDlg(&m_staMainArea);
   RECT rcArea, rcWnd;
   m_staMainArea.GetWindowRect(&rcArea);
   
   m_pLiveBroadcasting->Create(CLiveBroadcastingDlg::IDD);
   m_pLiveBroadcasting->GetWindowRect(&rcWnd);
   int w = rcWnd.right - rcWnd.left;
   int h = rcWnd.bottom - rcWnd.top;
   rcWnd = { rcArea.left, rcArea.top - MAIN_AREA_TOP, rcArea.left + w, rcArea.top + h};
   m_pLiveBroadcasting->MoveWindow(&rcWnd);

   //advanced list
   m_vecAdvanced.push_back(advancedRtmpInject);
   m_vecAdvanced.push_back(advancedRtmpStreaming);
   m_vecAdvanced.push_back(advancedVideoMetadata);

   //inject
   m_pRtmpInjectDlg = new CAgoraRtmpInjectionDlg(&m_staMainArea);
   m_pRtmpInjectDlg->Create(CAgoraRtmpInjectionDlg::IDD);
   m_pRtmpInjectDlg->MoveWindow(&rcWnd);
   //rtmp
   m_pRtmpStreamingDlg = new CAgoraRtmpStreamingDlg(&m_staMainArea);
   m_pRtmpStreamingDlg->Create(CAgoraRtmpStreamingDlg::IDD);
   m_pRtmpStreamingDlg->MoveWindow(&rcWnd);

   //video sei
   m_pVideoSEIDlg = new CAgoraMetaDataDlg(&m_staMainArea);
   m_pVideoSEIDlg->Create(CAgoraMetaDataDlg::IDD);
   m_pVideoSEIDlg->MoveWindow(&rcWnd);
}

void CAPIExampleDlg::InitSceneList()
{
    for (int i = 0; i < m_vecBasic.size(); i++){
        TVINSERTSTRUCT tvInsert;
        tvInsert.hParent = NULL;
        tvInsert.hInsertAfter = NULL;
        tvInsert.item.mask = TVIF_TEXT;
        tvInsert.item.pszText = m_vecBasic[i].GetBuffer(0);
        m_lstBasicScene.InsertItem(&tvInsert);
        
    }
  
    for (int i = 0; i < m_vecAdvanced.size(); i++) {
        TVINSERTSTRUCT tvInsert;
        tvInsert.hParent = NULL;
        tvInsert.hInsertAfter = NULL;
        tvInsert.item.mask = TVIF_TEXT;
        tvInsert.item.pszText = m_vecAdvanced[i].GetBuffer(0);
        m_lstAdvanced.InsertItem(&tvInsert);
    }
}


void CAPIExampleDlg::OnBnClickedButtonFaq()
{
    ShellExecute(NULL, L"open", L"https://docs.agora.io/cn/Video/faq", NULL, NULL, SW_SHOWNORMAL);
}


void CAPIExampleDlg::OnBnClickedButtonRegister()
{
    ShellExecute(NULL, L"open", L"https://sso.agora.io/en/v2/signup", NULL, NULL, SW_SHOWNORMAL);
}


void CAPIExampleDlg::OnBnClickedButtonDemo()
{
    ShellExecute(NULL, L"open", L"https://github.com/AgoraIO/API-Examples", NULL, NULL, SW_SHOWNORMAL);
}

void CAPIExampleDlg::OnSelchangedListBasic(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
    // TODO: Add your control notification handler code here
    *pResult = 0;

    HTREEITEM hItem = m_lstBasicScene.GetSelectedItem();
    m_preSelectedItemText = m_lstBasicScene.GetItemText(hItem);
    CreateScene(m_lstBasicScene, m_preSelectedItemText);
}

void CAPIExampleDlg::OnSelchangedListAdvanced(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
    *pResult = 0;
    HTREEITEM hItem = m_lstAdvanced.GetSelectedItem();
    m_preSelectedItemText = m_lstAdvanced.GetItemText(hItem);
    CreateScene(m_lstBasicScene, m_preSelectedItemText);
}

HTREEITEM  CAPIExampleDlg::GetHitItem(NMHDR *pNMHDR)
{
    DWORD dwPos = GetMessagePos();
    POINT pt;
    pt.x = LOWORD(dwPos);
    pt.y = HIWORD(dwPos);
    ::ScreenToClient(pNMHDR->hwndFrom, &pt);
    TVHITTESTINFO ht = { 0 };

    ht.flags = TVHT_ONITEM;
    HTREEITEM hitItem = TreeView_HitTest(pNMHDR->hwndFrom, &ht);
   
    return hitItem ;
}

void CAPIExampleDlg::OnClickListBasic(NMHDR *pNMHDR, LRESULT *pResult)
{
    *pResult = 0;
}

void CAPIExampleDlg::OnClickListAdvanced(NMHDR *pNMHDR, LRESULT *pResult)
{   
    *pResult = 0;

}

void CAPIExampleDlg::OnSelchangingListBasic(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
    HTREEITEM hOldItem = pNMTreeView->itemOld.hItem;
    HTREEITEM hAdvancedItem = m_lstAdvanced.GetSelectedItem();

    if (m_preSelectedItemText.Compare(m_lstBasicScene.GetItemText(hOldItem)) == 0) {
        ReleaseScene(m_lstBasicScene, hOldItem);
    }
    else  if (m_preSelectedItemText.Compare(m_lstAdvanced.GetItemText(hAdvancedItem)) == 0) {
       // m_lstAdvanced.SetItemState(hAdvancedItem, 0, TVIS_SELECTED);
        m_lstAdvanced.SelectItem(NULL);
        ReleaseScene(m_lstAdvanced, hAdvancedItem);
    }

    HTREEITEM hNewItem = pNMTreeView->itemNew.hItem;
    *pResult = 0;
}

void CAPIExampleDlg::OnSelchangingListAdvanced(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
    
    HTREEITEM hBasicItem = m_lstBasicScene.GetSelectedItem();
    HTREEITEM hOldItem = pNMTreeView->itemOld.hItem;
    if (m_preSelectedItemText.Compare(m_lstBasicScene.GetItemText(hBasicItem)) == 0) {
        m_lstBasicScene.SelectItem(NULL);
        ReleaseScene(m_lstBasicScene, hBasicItem);
    }
    else if (m_preSelectedItemText.Compare(m_lstAdvanced.GetItemText(hOldItem)) == 0) {
        ReleaseScene(m_lstAdvanced, hOldItem);
    }
    
    *pResult = 0;
}

//
void CAPIExampleDlg::CreateScene(CTreeCtrl& treeScene, CString selectedText)
{
    if (selectedText.Compare(basicLiveBroadcasting) == 0) {
        m_pLiveBroadcasting->InitAgora();
        m_pLiveBroadcasting->ShowWindow(SW_SHOW);
    }
    else if (selectedText.Compare(advancedRtmpInject) == 0) {
        m_pRtmpInjectDlg->InitAgora();
        m_pRtmpInjectDlg->ShowWindow(SW_SHOW);
    }
    else if (selectedText.Compare(advancedRtmpStreaming) == 0) {
        m_pRtmpStreamingDlg->InitAgora();
        m_pRtmpStreamingDlg->ShowWindow(SW_SHOW);
    }
    else if (selectedText.Compare(advancedVideoMetadata) == 0) {
        m_pVideoSEIDlg->InitAgora();
        m_pVideoSEIDlg->ShowWindow(SW_SHOW);
    }
}

void CAPIExampleDlg::ReleaseScene(CTreeCtrl& treeScene, HTREEITEM& hSelectItem)
{
    CString str = treeScene.GetItemText(hSelectItem);

    if (str.Compare(basicLiveBroadcasting) == 0
        && m_pLiveBroadcasting->IsWindowVisible()) {//pre sel release first
        m_pLiveBroadcasting->UnInitAgora();
        m_pLiveBroadcasting->ShowWindow(SW_HIDE);
    }
    else if (str.Compare(advancedRtmpInject) == 0) {
        m_pRtmpInjectDlg->UnInitAgora();
        m_pRtmpInjectDlg->ShowWindow(SW_HIDE);
    }
    else if (str.Compare(advancedRtmpStreaming) == 0) {
        m_pRtmpStreamingDlg->UnInitAgora();
        m_pRtmpStreamingDlg->ShowWindow(SW_HIDE);
    }
    else if (str.Compare(advancedVideoMetadata) == 0) {
        m_pVideoSEIDlg->UnInitAgora();
        m_pVideoSEIDlg->ShowWindow(SW_HIDE);
    }
}

LRESULT CAPIExampleDlg::OnEIDJoinLeaveChannel(WPARAM wParam, LPARAM lParam)
{
    m_bJoinChannel = (BOOL)wParam;
    m_lstAdvanced.EnableWindow(!m_bJoinChannel);
    m_lstBasicScene.EnableWindow(!m_bJoinChannel);
    if (m_bJoinChannel) {
        m_stalstInfo.SetWindowText(_T("加入频道成功如需切换场景请先离开频道"));
    }
    else
        m_stalstInfo.SetWindowText(_T(""));
    return 0;
}


void CAPIExampleDlg::OnBnClickedButtonDocumentWebsite()
{
    ShellExecute(NULL, L"open", L"https://docs.agora.io/cn", NULL, NULL, SW_SHOWNORMAL);
}
