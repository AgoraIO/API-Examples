
// APIExample.h: PROJECT_NAME 
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// 


// CAPIExampleApp:
//

class CAPIExampleApp : public CWinApp
{
public:
	CAPIExampleApp();

// override
public:
	virtual BOOL InitInstance();

// implement

	DECLARE_MESSAGE_MAP()
};

extern CAPIExampleApp theApp;
