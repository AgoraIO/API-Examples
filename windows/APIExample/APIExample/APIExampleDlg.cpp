
// APIExampleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "APIExample.h"
#include "APIExampleDlg.h"

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

CAPIExampleDlg::~CAPIExampleDlg()
{

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
	//ON_MESSAGE(WM_MSGID(EID_JOINCHANNEL_SUCCESS), &CAPIExampleDlg::OnEIDJoinLeaveChannel)
	ON_NOTIFY(TVN_SELCHANGING, IDC_LIST_ADVANCED, &CAPIExampleDlg::OnSelchangingListAdvanced)
	ON_BN_CLICKED(IDC_BUTTON_DOCUMENT_WEBSITE, &CAPIExampleDlg::OnBnClickedButtonDocumentWebsite)
	ON_WM_CLOSE()
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
	std::string strAppID = GET_APP_ID;
	if (strcmp(strAppID.c_str(), "") == 0)
	{
		AfxMessageBox(_T("APP ID is not set, you can see readme file on how to get an APP ID."));
		ExitProcess(1);
	}
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
	m_vecBasic.push_back(basicJoinChannelVideoByToken);

	m_pLiveBroadcasting = new CLiveBroadcastingDlg(&m_staMainArea);
	RECT rcArea, rcWnd;
	m_staMainArea.GetWindowRect(&rcArea);

	m_pLiveBroadcasting->Create(CLiveBroadcastingDlg::IDD);
	m_pLiveBroadcasting->GetWindowRect(&rcWnd);
	int w = rcWnd.right - rcWnd.left;
	int h = rcWnd.bottom - rcWnd.top;
	rcWnd = { rcArea.left, rcArea.top - MAIN_AREA_TOP, rcArea.left + w, rcArea.top + h };
	m_pLiveBroadcasting->MoveWindow(&rcWnd);


	//join channel video by token
	m_pJoinChannelVideoByTokenDlg = new CJoinChannelVideoByTokenDlg(&m_staMainArea);
	m_pJoinChannelVideoByTokenDlg->Create(CJoinChannelVideoByTokenDlg::IDD);
	m_pJoinChannelVideoByTokenDlg->MoveWindow(&rcWnd);

	//advanced list
	// m_vecAdvanced.push_back(advancedRtmpInject);
	m_vecAdvanced.push_back(advancedRtmpStreaming);
	m_vecAdvanced.push_back(advancedCrossChannel);
	m_vecAdvanced.push_back(advancedMetadata);
	m_vecAdvanced.push_back(advancedMediaPlayer);
	m_vecAdvanced.push_back(advancedMediaRecorder);
	m_vecAdvanced.push_back(advancedScreenCap);
	m_vecAdvanced.push_back(advancedAudioProfile);
	m_vecAdvanced.push_back(advancedAudioMixing);
	// m_vecAdvanced.push_back(advancedAudioEffect);
	m_vecAdvanced.push_back(advancedCustomVideoCapture);
	m_vecAdvanced.push_back(advancedOriginalVideo);
	m_vecAdvanced.push_back(advancedCustomAudioCapture);
	m_vecAdvanced.push_back(advancedOriginalAudio);
	m_vecAdvanced.push_back(advancedFaceCapture);
	m_vecAdvanced.push_back(advancedCustomEncrypt);
	m_vecAdvanced.push_back(advancedMultiChannel);
	m_vecAdvanced.push_back(advancedMultiVideoSource);
	m_vecAdvanced.push_back(advancedMultiVideoSourceTracks);
	m_vecAdvanced.push_back(advancedPerCallTest);
	m_vecAdvanced.push_back(advancedAudioVolume);
	//m_vecAdvanced.push_back(advancedReportInCall);
	m_vecAdvanced.push_back(advancedRegionConn);
	m_vecAdvanced.push_back(advancedMediaEncrypt);
	m_vecAdvanced.push_back(AdvancedLocalVideoTranscoding);
	m_vecAdvanced.push_back(beautyFace);
	m_vecAdvanced.push_back(advancedBeautyAudio);
	m_vecAdvanced.push_back(SpatialAudio);
	m_pMultiChannelDlg = new CAgoraMultiChannelDlg(&m_staMainArea);
	m_pMultiChannelDlg->Create(CAgoraMultiChannelDlg::IDD);
	m_pMultiChannelDlg->MoveWindow(&rcWnd);

	//transparent bg
	m_vecAdvanced.push_back(TransparentBackground);
	m_vecAdvanced.push_back(RteUrlPlayer);

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

	//screen share
	m_pScreenCap = new CAgoraScreenCapture(&m_staMainArea);
	m_pScreenCap->Create(CAgoraScreenCapture::IDD);
	m_pScreenCap->MoveWindow(&rcWnd);


	//audio profile
	m_pAudioProfileDlg = new CAgoraAudioProfile(&m_staMainArea);
	m_pAudioProfileDlg->Create(CAgoraAudioProfile::IDD);
	m_pAudioProfileDlg->MoveWindow(&rcWnd);

	//audio mixing
	m_pAudioMixingDlg = new CAgoraAudioMixingDlg(&m_staMainArea);
	m_pAudioMixingDlg->Create(CAgoraAudioMixingDlg::IDD);
	m_pAudioMixingDlg->MoveWindow(&rcWnd);

	//custom video capture
 //    m_pCaputreVideoDlg = new CAgoraCaptureVideoDlg(&m_staMainArea);
 //    m_pCaputreVideoDlg->Create(CAgoraCaptureVideoDlg::IDD);
 //    m_pCaputreVideoDlg->MoveWindow(&rcWnd);

	//original video process
	m_pOriginalVideoDlg = new CAgoraOriginalVideoDlg(&m_staMainArea);
	m_pOriginalVideoDlg->Create(CAgoraOriginalVideoDlg::IDD);
	m_pOriginalVideoDlg->MoveWindow(&rcWnd);

	//custom audio capture
	m_pCaptureAudioDlg = new CAgoraCaptureAduioDlg(&m_staMainArea);
	m_pCaptureAudioDlg->Create(CAgoraCaptureAduioDlg::IDD);
	m_pCaptureAudioDlg->MoveWindow(&rcWnd);

	//original video process
	m_pOriginalAudioDlg = new CAgoraOriginalAudioDlg(&m_staMainArea);
	m_pOriginalAudioDlg->Create(CAgoraOriginalAudioDlg::IDD);
	m_pOriginalAudioDlg->MoveWindow(&rcWnd);

	m_pAgoraFaceCaptureDlg = new CAgoraFaceCaptureDlg(&m_staMainArea);
	m_pAgoraFaceCaptureDlg->Create(CAgoraFaceCaptureDlg::IDD);
	m_pAgoraFaceCaptureDlg->MoveWindow(&rcWnd);

	//custom encrypt
	m_pCustomEncryptDlg = new CAgoraCustomEncryptDlg(&m_staMainArea);
	m_pCustomEncryptDlg->Create(CAgoraCustomEncryptDlg::IDD);
	m_pCustomEncryptDlg->MoveWindow(&rcWnd);


	//multi video source
	m_pMultiVideoSourceDlg = new CAgoraMutilVideoSourceDlg(&m_staMainArea);
	m_pMultiVideoSourceDlg->Create(CAgoraMutilVideoSourceDlg::IDD);
	m_pMultiVideoSourceDlg->MoveWindow(&rcWnd);

	//media player
	m_pmediaPlayerDlg = new CAgoraMediaPlayer(&m_staMainArea);
	m_pmediaPlayerDlg->Create(CAgoraMediaPlayer::IDD);
	m_pmediaPlayerDlg->MoveWindow(&rcWnd);

	//media recorder
	m_pmediaRecorderDlg = new CAgoraMediaRecorder(&m_staMainArea);
	m_pmediaRecorderDlg->Create(CAgoraMediaRecorder::IDD);
	m_pmediaRecorderDlg->MoveWindow(&rcWnd);

	//per call test
	m_pPerCallTestDlg = new CAgoraPreCallTestDlg(&m_staMainArea);
	m_pPerCallTestDlg->Create(CAgoraPreCallTestDlg::IDD);
	m_pPerCallTestDlg->MoveWindow(&rcWnd);

	m_pAudioVolumeDlg = new CAgoraAudioVolumeDlg(&m_staMainArea);
	m_pAudioVolumeDlg->Create(CAgoraAudioVolumeDlg::IDD);
	m_pAudioVolumeDlg->MoveWindow(&rcWnd);


	m_pReportInCallDlg = new CAgoraReportInCallDlg(&m_staMainArea);
	m_pReportInCallDlg->Create(CAgoraReportInCallDlg::IDD);
	m_pReportInCallDlg->MoveWindow(&rcWnd);

	m_pRegionConnDlg = new CAgoraRegionConnDlg(&m_staMainArea);
	m_pRegionConnDlg->Create(CAgoraRegionConnDlg::IDD);
	m_pRegionConnDlg->MoveWindow(&rcWnd);

	m_pMediaEncrptDlg = new CAgoraMediaEncryptDlg(&m_staMainArea);
	m_pMediaEncrptDlg->Create(CAgoraMediaEncryptDlg::IDD);
	m_pMediaEncrptDlg->MoveWindow(&rcWnd);

	m_pEffectDlg = new CAgoraEffectDlg(&m_staMainArea);
	m_pEffectDlg->Create(CAgoraEffectDlg::IDD);
	m_pEffectDlg->MoveWindow(&rcWnd);


	m_pDlgBeautyAudio = new CAgoraBeautyAudio(&m_staMainArea);
	m_pDlgBeautyAudio->Create(CAgoraBeautyAudio::IDD);
	m_pDlgBeautyAudio->MoveWindow(&rcWnd);

	m_pLocalVideoTranscodingDlg = new CLocalVideoTranscodingDlg(&m_staMainArea);
	m_pLocalVideoTranscodingDlg->Create(CLocalVideoTranscodingDlg::IDD);
	m_pLocalVideoTranscodingDlg->MoveWindow(&rcWnd);

	m_pDlgBeauty = new CDlgBeauty(&m_staMainArea);
	m_pDlgBeauty->Create(CDlgBeauty::IDD);
	m_pDlgBeauty->MoveWindow(&rcWnd);


	// spatial audio
	m_pSpatialAudioDlg = new CAgoraSpatialAudioDlg(&m_staMainArea);
	m_pSpatialAudioDlg->Create(CAgoraSpatialAudioDlg::IDD);
	m_pSpatialAudioDlg->MoveWindow(&rcWnd);

	m_pPushExternalVideoYUV = new PushExternalVideoYUV(&m_staMainArea);
	m_pPushExternalVideoYUV->Create(PushExternalVideoYUV::IDD);
	m_pPushExternalVideoYUV->MoveWindow(&rcWnd);

	m_pMultiVideoSourceTracks = new MultiVideoSourceTracks(&m_staMainArea);
	m_pMultiVideoSourceTracks->Create(MultiVideoSourceTracks::IDD);
	m_pMultiVideoSourceTracks->MoveWindow(&rcWnd);

	m_pAgoraCrossChannelDlg = new CAgoraCrossChannelDlg(&m_staMainArea);
    m_pAgoraCrossChannelDlg->Create(CAgoraCrossChannelDlg::IDD);
    m_pAgoraCrossChannelDlg->MoveWindow(&rcWnd);

	// transparent bg
	m_TransparentDlg = new CTransparentBgDlg(&m_staMainArea);
	m_TransparentDlg->Create(CTransparentBgDlg::IDD);
	m_TransparentDlg->MoveWindow(&rcWnd);

	// rte player
	m_RtePlayer = new CRtePlayerDlg(&m_staMainArea);
	m_RtePlayer->Create(CRtePlayerDlg::IDD);
	m_RtePlayer->MoveWindow(&rcWnd);
	
}

void CAPIExampleDlg::InitSceneList()
{
	for (size_t i = 0; i < m_vecBasic.size(); i++) {
		TVINSERTSTRUCT tvInsert;
		tvInsert.hParent = NULL;
		tvInsert.hInsertAfter = NULL;
		tvInsert.item.mask = TVIF_TEXT;
		tvInsert.item.pszText = m_vecBasic[i].GetBuffer(0);
		m_lstBasicScene.InsertItem(&tvInsert);
	}

	for (size_t i = 0; i < m_vecAdvanced.size(); i++) {
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

void CAPIExampleDlg::OnSelchangedListBasic(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	HTREEITEM hItem = m_lstBasicScene.GetSelectedItem();
	m_preSelectedItemText = m_lstBasicScene.GetItemText(hItem);
	CreateScene(m_lstBasicScene, m_preSelectedItemText);
}

void CAPIExampleDlg::OnSelchangedListAdvanced(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	*pResult = 0;
	HTREEITEM hItem = m_lstAdvanced.GetSelectedItem();
	m_preSelectedItemText = m_lstAdvanced.GetItemText(hItem);
	CreateScene(m_lstBasicScene, m_preSelectedItemText);
}

HTREEITEM  CAPIExampleDlg::GetHitItem(NMHDR* pNMHDR)
{
	DWORD dwPos = GetMessagePos();
	POINT pt;
	pt.x = LOWORD(dwPos);
	pt.y = HIWORD(dwPos);
	::ScreenToClient(pNMHDR->hwndFrom, &pt);
	TVHITTESTINFO ht = { 0 };

	ht.flags = TVHT_ONITEM;
	HTREEITEM hitItem = TreeView_HitTest(pNMHDR->hwndFrom, &ht);

	return hitItem;
}

void CAPIExampleDlg::OnClickListBasic(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = 0;
}

void CAPIExampleDlg::OnClickListAdvanced(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = 0;

}

void CAPIExampleDlg::OnSelchangingListBasic(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	HTREEITEM hOldItem = pNMTreeView->itemOld.hItem;
	HTREEITEM hAdvancedItem = m_lstAdvanced.GetSelectedItem();
	HTREEITEM hNewItem = pNMTreeView->itemNew.hItem;

	if (m_preSelectedItemText.Compare(m_lstBasicScene.GetItemText(hOldItem)) == 0) {
		ReleaseScene(m_lstBasicScene, hOldItem);
	}
	else  if (m_preSelectedItemText.Compare(m_lstAdvanced.GetItemText(hAdvancedItem)) == 0) {
		// m_lstAdvanced.SetItemState(hAdvancedItem, 0, TVIS_SELECTED);
		m_lstAdvanced.SelectItem(NULL);
	}

	*pResult = 0;
}

void CAPIExampleDlg::OnSelchangingListAdvanced(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	HTREEITEM hBasicItem = m_lstBasicScene.GetSelectedItem();
	HTREEITEM hOldItem = pNMTreeView->itemOld.hItem;
	HTREEITEM hNewItem = pNMTreeView->itemNew.hItem;

	if (m_preSelectedItemText.Compare(m_lstBasicScene.GetItemText(hBasicItem)) == 0) {
		m_lstBasicScene.SelectItem(NULL);
	}
	else if (m_preSelectedItemText.Compare(m_lstAdvanced.GetItemText(hOldItem)) == 0) {
		ReleaseScene(m_lstAdvanced, hOldItem);
	}

	*pResult = 0;
}

//
void CAPIExampleDlg::CreateScene(CTreeCtrl& treeScene, CString selectedText)
{
	std::lock_guard<std::mutex>_lock(m_mutex);
	if (selectedText.Compare(basicLiveBroadcasting) == 0) {
		m_pLiveBroadcasting->InitAgora();
		m_pLiveBroadcasting->ShowWindow(SW_SHOW);
	}
	else if (selectedText.Compare(basicJoinChannelVideoByToken) == 0) {
		m_pJoinChannelVideoByTokenDlg->InitAgora();
		m_pJoinChannelVideoByTokenDlg->ShowWindow(SW_SHOW);
	}
	else if (selectedText.Compare(advancedRtmpInject) == 0) {
		m_pRtmpInjectDlg->InitAgora();
		m_pRtmpInjectDlg->ShowWindow(SW_SHOW);
	}
	else if (selectedText.Compare(advancedRtmpStreaming) == 0) {
		m_pRtmpStreamingDlg->InitAgora();
		m_pRtmpStreamingDlg->ShowWindow(SW_SHOW);
	}
	else if (selectedText.Compare(advancedCrossChannel) == 0) {
		m_pAgoraCrossChannelDlg->InitAgora();
		m_pAgoraCrossChannelDlg->ShowWindow(SW_SHOW);
	}
	else if (selectedText.Compare(advancedMetadata) == 0) {
		m_pVideoSEIDlg->InitAgora();
		m_pVideoSEIDlg->ShowWindow(SW_SHOW);
	}
	else if (selectedText.Compare(advancedScreenCap) == 0) {
		m_pScreenCap->InitAgora();
		m_pScreenCap->ShowWindow(SW_SHOW);
	}
	else if (selectedText.Compare(advancedCustomVideoCapture) == 0) {
		m_pPushExternalVideoYUV->InitAgora();
		m_pPushExternalVideoYUV->ShowWindow(SW_SHOW);
	}
	else if (selectedText.Compare(advancedCustomAudioCapture) == 0) {
		m_pCaptureAudioDlg->InitAgora();
		m_pCaptureAudioDlg->ShowWindow(SW_SHOW);
	}
	else if (selectedText.Compare(advancedAudioProfile) == 0) {
		m_pAudioProfileDlg->InitAgora();
		m_pAudioProfileDlg->ShowWindow(SW_SHOW);
	}
	else if (selectedText.Compare(advancedAudioMixing) == 0) {
		m_pAudioMixingDlg->InitAgora();
		m_pAudioMixingDlg->ShowWindow(SW_SHOW);
	}
	else if (selectedText.Compare(advancedOriginalVideo) == 0) {
		m_pOriginalVideoDlg->InitAgora();
		m_pOriginalVideoDlg->ShowWindow(SW_SHOW);
	}
	else if (selectedText.Compare(advancedOriginalAudio) == 0) {
		m_pOriginalAudioDlg->InitAgora();
		m_pOriginalAudioDlg->ShowWindow(SW_SHOW);
	}
	else if (selectedText.Compare(advancedFaceCapture) == 0) {
		m_pAgoraFaceCaptureDlg->InitAgora();
		m_pAgoraFaceCaptureDlg->ShowWindow(SW_SHOW);
	}
	else if (selectedText.Compare(advancedCustomEncrypt) == 0) {
		m_pCustomEncryptDlg->InitAgora();
		m_pCustomEncryptDlg->ShowWindow(SW_SHOW);
	}
	else if (selectedText.Compare(advancedMultiChannel) == 0) {
		m_pMultiChannelDlg->InitAgora();
		m_pMultiChannelDlg->ShowWindow(SW_SHOW);
	}
	else if (selectedText.Compare(advancedPerCallTest) == 0) {
		m_pPerCallTestDlg->InitAgora();
		m_pPerCallTestDlg->ShowWindow(SW_SHOW);
	}
	else if (selectedText.Compare(advancedAudioVolume) == 0) {
		m_pAudioVolumeDlg->InitAgora();
		m_pAudioVolumeDlg->ShowWindow(SW_SHOW);
	}
	else if (selectedText.Compare(advancedReportInCall) == 0) {
		m_pReportInCallDlg->InitAgora();
		m_pReportInCallDlg->ShowWindow(SW_SHOW);
	}
	else if (selectedText.Compare(advancedRegionConn) == 0) {
		m_pRegionConnDlg->ShowWindow(SW_SHOW);
	}
	else if (selectedText.Compare(advancedMediaEncrypt) == 0) {
		m_pMediaEncrptDlg->InitAgora();
		m_pMediaEncrptDlg->ShowWindow(SW_SHOW);
	}
	else if (selectedText.Compare(advancedAudioEffect) == 0) {
		m_pEffectDlg->InitAgora();
		m_pEffectDlg->ShowWindow(SW_SHOW);
	}
	else if (selectedText.Compare(advancedMediaPlayer) == 0) {
		m_pmediaPlayerDlg->InitAgora();
		m_pmediaPlayerDlg->ShowWindow(SW_SHOW);
	}
	else if (selectedText.Compare(advancedMultiVideoSource) == 0) {
		m_pMultiVideoSourceDlg->InitAgora();
		m_pMultiVideoSourceDlg->ShowWindow(SW_SHOW);
	}
	else if (selectedText.Compare(advancedMultiVideoSourceTracks) == 0) {
		m_pMultiVideoSourceTracks->InitAgora();
		m_pMultiVideoSourceTracks->ShowWindow(SW_SHOW);
	}
	else if (selectedText.Compare(advancedBeautyAudio) == 0) {
		m_pDlgBeautyAudio->InitAgora();
		m_pDlgBeautyAudio->ShowWindow(SW_SHOW);
	}

	else if (selectedText.Compare(AdvancedLocalVideoTranscoding) == 0) {
		m_pLocalVideoTranscodingDlg->InitAgora();
		m_pLocalVideoTranscodingDlg->ShowWindow(SW_SHOW);
	}
	else if (selectedText.Compare(beautyFace) == 0) {
		m_pDlgBeauty->InitAgora();
		m_pDlgBeauty->ShowWindow(SW_SHOW);
	}
	else if (selectedText.Compare(SpatialAudio) == 0) {
		m_pSpatialAudioDlg->InitAgora();
		m_pSpatialAudioDlg->ShowWindow(SW_SHOW);
	}
	else if (selectedText.Compare(advancedMediaRecorder) == 0) {
		m_pmediaRecorderDlg->InitAgora();
		m_pmediaRecorderDlg->ShowWindow(SW_SHOW);
	}
	else if (selectedText.Compare(TransparentBackground) == 0) {
		m_TransparentDlg->InitAgora();
		m_TransparentDlg->ShowWindow(SW_SHOW);
	}
	else if (selectedText.Compare(RteUrlPlayer) == 0) {
		m_RtePlayer->InitAgora();
		m_RtePlayer->ShowWindow(SW_SHOW);
	}
	
	//Sleep(500);
}

void CAPIExampleDlg::ReleaseScene(CTreeCtrl& treeScene, HTREEITEM& hSelectItem)
{
	std::lock_guard<std::mutex>_lock(m_mutex);
	CString str = treeScene.GetItemText(hSelectItem);
	if (str.Compare(basicLiveBroadcasting) == 0
		&& m_pLiveBroadcasting->IsWindowVisible()) {//pre sel release first
		m_pLiveBroadcasting->UnInitAgora();
		m_pLiveBroadcasting->ShowWindow(SW_HIDE);
	}
	else if (str.Compare(basicJoinChannelVideoByToken) == 0) {
		m_pJoinChannelVideoByTokenDlg->UnInitAgora();
		m_pJoinChannelVideoByTokenDlg->ShowWindow(SW_HIDE);
	}
	else if (str.Compare(advancedRtmpInject) == 0) {
		m_pRtmpInjectDlg->UnInitAgora();
		m_pRtmpInjectDlg->ShowWindow(SW_HIDE);
	}
	else if (str.Compare(advancedRtmpStreaming) == 0) {
		m_pRtmpStreamingDlg->UnInitAgora();
		m_pRtmpStreamingDlg->ShowWindow(SW_HIDE);
	}
	else if (str.Compare(advancedCrossChannel) == 0) {
		m_pAgoraCrossChannelDlg->UnInitAgora();
		m_pAgoraCrossChannelDlg->ShowWindow(SW_HIDE);
	}
	else if (str.Compare(advancedMetadata) == 0) {
		m_pVideoSEIDlg->UnInitAgora();
		m_pVideoSEIDlg->ShowWindow(SW_HIDE);
	}
	else if (str.Compare(advancedScreenCap) == 0) {
		m_pScreenCap->UnInitAgora();
		m_pScreenCap->ShowWindow(SW_HIDE);
	}
	else if (str.Compare(advancedCustomVideoCapture) == 0) {
		m_pPushExternalVideoYUV->UnInitAgora();
		m_pPushExternalVideoYUV->ShowWindow(SW_HIDE);
	}
	else if (str.Compare(advancedCustomAudioCapture) == 0) {
		m_pCaptureAudioDlg->UnInitAgora();
		m_pCaptureAudioDlg->ShowWindow(SW_HIDE);
	}
	else if (str.Compare(advancedAudioProfile) == 0) {
		m_pAudioProfileDlg->UnInitAgora();
		m_pAudioProfileDlg->ShowWindow(SW_HIDE);
	}
	else if (str.Compare(advancedAudioMixing) == 0) {
		m_pAudioMixingDlg->UnInitAgora();
		m_pAudioMixingDlg->ShowWindow(SW_HIDE);
	}
	else if (str.Compare(advancedOriginalVideo) == 0) {
		m_pOriginalVideoDlg->UnInitAgora();
		m_pOriginalVideoDlg->ShowWindow(SW_HIDE);
	}
	else if (str.Compare(advancedOriginalAudio) == 0) {
		m_pOriginalAudioDlg->UnInitAgora();
		m_pOriginalAudioDlg->ShowWindow(SW_HIDE);
	}
	else if (str.Compare(advancedFaceCapture) == 0) {
		m_pAgoraFaceCaptureDlg->UnInitAgora();
		m_pAgoraFaceCaptureDlg->ShowWindow(SW_HIDE);
	}
	else if (str.Compare(advancedCustomEncrypt) == 0) {
		m_pCustomEncryptDlg->UnInitAgora();
		m_pCustomEncryptDlg->ShowWindow(SW_HIDE);
	}
	else if (str.Compare(advancedMultiChannel) == 0) {
		m_pMultiChannelDlg->UnInitAgora();
		m_pMultiChannelDlg->ShowWindow(SW_HIDE);
	}
	else if (str.Compare(advancedPerCallTest) == 0) {
		m_pPerCallTestDlg->UnInitAgora();
		m_pPerCallTestDlg->ShowWindow(SW_HIDE);
	}
	else if (str.Compare(advancedAudioVolume) == 0) {
		m_pAudioVolumeDlg->UnInitAgora();
		m_pAudioVolumeDlg->ShowWindow(SW_HIDE);
	}
	else if (str.Compare(advancedReportInCall) == 0) {
		m_pReportInCallDlg->UnInitAgora();
		m_pReportInCallDlg->ShowWindow(SW_HIDE);
	}
	else if (str.Compare(advancedRegionConn) == 0) {
		m_pRegionConnDlg->UnInitAgora();
		m_pRegionConnDlg->ShowWindow(SW_HIDE);
	}
	else if (str.Compare(advancedMediaEncrypt) == 0) {
		m_pMediaEncrptDlg->UnInitAgora();
		m_pMediaEncrptDlg->ShowWindow(SW_HIDE);
	}
	else if (str.Compare(advancedAudioEffect) == 0) {
		m_pEffectDlg->UnInitAgora();
		m_pEffectDlg->ShowWindow(SW_HIDE);
	}
	else if (str.Compare(advancedMultiVideoSource) == 0) {
		m_pMultiVideoSourceDlg->UnInitAgora();
		m_pMultiVideoSourceDlg->ShowWindow(SW_HIDE);
	}
	else if (str.Compare(advancedMultiVideoSourceTracks) == 0) {
		m_pMultiVideoSourceTracks->UnInitAgora();
		m_pMultiVideoSourceTracks->ShowWindow(SW_HIDE);
	}
	else if (str.Compare(advancedBeautyAudio) == 0) {
		m_pDlgBeautyAudio->UnInitAgora();
		m_pDlgBeautyAudio->ShowWindow(SW_HIDE);
	}
	else if (str.Compare(AdvancedLocalVideoTranscoding) == 0) {
		m_pLocalVideoTranscodingDlg->UnInitAgora();
		m_pLocalVideoTranscodingDlg->ShowWindow(SW_HIDE);
	}
	else if (str.Compare(advancedMediaPlayer) == 0) {
		m_pmediaPlayerDlg->UnInitAgora();
		m_pmediaPlayerDlg->ShowWindow(SW_HIDE);
	}
	else if (str.Compare(beautyFace) == 0) {
		m_pDlgBeauty->UnInitAgora();
		m_pDlgBeauty->ShowWindow(SW_HIDE);
	}
	else if (str.Compare(SpatialAudio) == 0) {
		m_pSpatialAudioDlg->UnInitAgora();
		m_pSpatialAudioDlg->ShowWindow(SW_HIDE);
	}
	else if (str.Compare(advancedMediaRecorder) == 0) {
		m_pmediaRecorderDlg->UnInitAgora();
		m_pmediaRecorderDlg->ShowWindow(SW_HIDE);
	}
	else if (str.Compare(TransparentBackground) == 0) {
		m_TransparentDlg->UnInitAgora();
		m_TransparentDlg->ShowWindow(SW_HIDE);
	}
	else if (str.Compare(RteUrlPlayer) == 0) {
		m_RtePlayer->UnInitAgora();
		m_RtePlayer->ShowWindow(SW_HIDE);
	}
	//Sleep(500);
}

LRESULT CAPIExampleDlg::OnEIDJoinLeaveChannel(WPARAM wParam, LPARAM lParam)
{
	m_bJoinChannel = (BOOL)wParam;
	m_lstAdvanced.EnableWindow(!m_bJoinChannel);
	m_lstBasicScene.EnableWindow(!m_bJoinChannel);
	if (m_bJoinChannel) {
		m_stalstInfo.SetWindowText(L"you can leave channel first.");
	}
	else
		m_stalstInfo.SetWindowText(_T(""));
	return 0;
}


void CAPIExampleDlg::OnBnClickedButtonDocumentWebsite()
{
	ShellExecute(NULL, L"open", L"https://docs.agora.io/cn", NULL, NULL, SW_SHOWNORMAL);
}


BOOL CAPIExampleDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) {
		return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}


void CAPIExampleDlg::OnClose()
{
	HTREEITEM hBasicItem = m_lstBasicScene.GetSelectedItem();
	HTREEITEM hAdvancedItem = m_lstAdvanced.GetSelectedItem();

	if (hBasicItem) {
		ReleaseScene(m_lstBasicScene, hBasicItem);
	}
	else if (hAdvancedItem) {
		ReleaseScene(m_lstAdvanced, hAdvancedItem);
	}
	CDialogEx::OnClose();
}
