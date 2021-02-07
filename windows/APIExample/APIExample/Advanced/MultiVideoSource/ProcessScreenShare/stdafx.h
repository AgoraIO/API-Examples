
// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#endif

#define _CRT_SECURE_NO_WARNINGS
#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions


#include <afxdisp.h>        // MFC Automation classes



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC support for Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // MFC support for ribbons and control bars

#include "../AGMessage.h"
#include <string>
#include <afxdisp.h>
#include <IAgoraRtcEngine.h>
#include "../commonFun.h"
#pragma comment(lib, "agora_rtc_sdk.lib")


//screenshare

typedef enum eScreenShareType
{
	ShareType_BaseInfo,
	ShareType_Start,
	ShareType_Stop,
	ShareType_Close,
}SHARETYPE;

typedef struct _AGE_SCREENSHARE_BASEINFO
{
	std::string channelname;
	std::string appid;
	UINT uMainuID;
	UINT uSubuID;
	HANDLE processHandle = NULL;
}AGE_SCREENSHARE_BASEINFO, *PAGE_SCREENSHARE_BASEINFO, *LPAGE_SCREENSHARE_BASEINFO;


typedef struct _AGE_SCREENSHARE_START
{
	HWND hWnd;
}AGE_SCREENSHARE_START, *PAGE_SCREENSHARE_START, *LPAGE_SCREENSHARE_START;


#define WM_SCREEN_MSG_ID(code) (WM_USER +code)
#define EID_SCREENSHARE_BASEINFO 0x00000051
#define EID_SCREENSHARE_START    0x00000052
#define EID_SCREENSHARE_STOP	 0x00000053
#define EID_SCREENSHARE_CLOSE    0x00000054
#define EID_JOINCHANNEL_SUCCESS						0x00000055
#define EID_LEAVE_CHANNEL							0x00000056
#define EID_USER_JOINED								0x00000057
#define EID_USER_OFFLINE							0x00000058
#define EID_INJECT_STATUS							0x00000059
#define EID_RTMP_STREAM_STATE_CHANGED				0x00000060
#define EID_REMOTE_VIDEO_STATE_CHANED				0x00000061
#define EID_PARENT_PROCESS_EXIT                     0x00000062
#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


