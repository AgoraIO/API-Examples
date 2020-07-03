
// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#endif

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
#include <afxcontrolbars.h>


#pragma warning(disable:4819)

#define APP_ID     ""
#define APP_TOKEN  ""   

#include <IAgoraRtcEngine.h>
#include <IAgoraMediaEngine.h>
#include <string>
#include <afxcontrolbars.h>
#include <afxcontrolbars.h>

#include "Language.h"
#include <afxcontrolbars.h>
#pragma comment(lib, "agora_rtc_sdk.lib")
using namespace agora;
using namespace agora::rtc;
using namespace agora::media;
#define WM_MSGID(code) (WM_USER+0x200+code)
//Agora Event Handler Message and structure
#define EID_JOINCHANNEL_SUCCESS       0x00000001
#define EID_LEAVE_CHANNEL             0x00000002
#define EID_USER_JOINED               0x00000003
#define EID_USER_OFFLINE              0x00000004
#define EID_INJECT_STATUS             0x00000005
#define EID_RTMP_STREAM_STATE_CHANGED 0x00000006
#define EID_REMOTE_VIDEO_STATE_CHANED 0x00000007
#define RECV_METADATA_MSG             0x00000008
typedef struct _tagRtmpStreamStateChanged {
    char* url;
    int state;
    int error;
}RtmpStreamStreamStateChanged, *PRtmpStreamStreamStateChanged;

typedef struct _tagVideoStateStateChanged {
    uid_t uid;
    REMOTE_VIDEO_STATE   state;
    REMOTE_VIDEO_STATE_REASON reason;
}VideoStateStateChanged, *PVideoStateStateChanged;

std::string cs2utf8(CString str);
CString utf82cs(std::string utf8);
CString getCurrentTime();


#define ID_BASEWND_VIDEO      20000
#define MAIN_AREA_TOP 20
#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


