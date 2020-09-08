
// APIExample.cpp: 定义应用程序的类行为。
//

#include "pch.h"
#include "framework.h"
#include "APIExample.h"
#include "APIExampleDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAPIExampleApp

BEGIN_MESSAGE_MAP(CAPIExampleApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CAPIExampleApp 

CAPIExampleApp::CAPIExampleApp()
{
	//
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	
}

CAPIExampleApp theApp;


// CAPIExampleApp initialize

BOOL CAPIExampleApp::InitInstance()
{
	
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();
    

	AfxEnableControlContainer();


	CShellManager *pShellManager = new CShellManager;

	
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	
	SetRegistryKey(_T("Application"));

	CAPIExampleDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		
	}
	else if (nResponse == IDCANCEL)
	{
		
	}
	else if (nResponse == -1)
	{
	
	}

	
	if (pShellManager != nullptr)
	{
		delete pShellManager;
	}

#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
	ControlBarCleanUp();
#endif
	return FALSE;
}

