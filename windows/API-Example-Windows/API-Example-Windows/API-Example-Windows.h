
// API-Example-Windows.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CAPIExampleWindowsApp:
// See API-Example-Windows.cpp for the implementation of this class
//

class CAPIExampleWindowsApp : public CWinApp
{
public:
	CAPIExampleWindowsApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CAPIExampleWindowsApp theApp;
