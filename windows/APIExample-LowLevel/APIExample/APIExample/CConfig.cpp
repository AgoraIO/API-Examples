#include "pch.h"
#include "CConfig.h"
CConfig* CConfig::GetInstance()
{
    static CConfig config;
    return &config;
}

CConfig::CConfig()
{
    m_bChinese = false;
    ::GetModuleFileName(NULL, m_szZhConfigFile, MAX_PATH);
    LPTSTR lpLastSlash = _tcsrchr(m_szZhConfigFile, _T('\\')) + 1;
    _tcscpy_s(lpLastSlash, MAX_PATH, _T("zh-cn.ini"));

    if (::GetFileAttributes(m_szZhConfigFile) == INVALID_FILE_ATTRIBUTES) {
        HANDLE hFile = ::CreateFile(m_szZhConfigFile, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        ::CloseHandle(hFile);
    }

    ::GetModuleFileName(NULL, m_szEnConfigFile, MAX_PATH);
    LPTSTR lpLastSlashEn = _tcsrchr(m_szEnConfigFile, _T('\\')) + 1;
    _tcscpy_s(lpLastSlashEn, MAX_PATH, _T("en.ini"));

    if (::GetFileAttributes(m_szEnConfigFile) == INVALID_FILE_ATTRIBUTES) {
        HANDLE hFile = ::CreateFile(m_szEnConfigFile, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        ::CloseHandle(hFile);
    }
    LCID lcid = GetUserDefaultLCID();//LCID https://www.science.co.il/language/Locale-codes.php
    if (lcid == 2052//chinese  
        || lcid == 4100//zh-sg
        || lcid == 5124
        || lcid == 3076
        || lcid == 1028) {//	zh-tw
        m_bChinese = true;
    }
    
}


CConfig::~CConfig()
{
}


CString CConfig::GetStringValue(CString key)
{
    CString strValue = _T("");
    if (m_bChinese)
        ::GetPrivateProfileString(_T("General"), key, _T("Unknown"), strValue.GetBuffer(MAX_PATH), MAX_PATH, m_szZhConfigFile);
    else
        ::GetPrivateProfileString(_T("General"), key, _T("Unknown"), strValue.GetBuffer(MAX_PATH), MAX_PATH, m_szEnConfigFile);
    return strValue;
}


CString CConfig::GetAPP_ID()
{
	CString strAppID(APP_ID);
	
	TCHAR szFilePath[MAX_PATH];
	::GetModuleFileName(NULL, szFilePath, MAX_PATH);
	LPTSTR lpLastSlash = _tcsrchr(szFilePath, _T('\\'));

	if (lpLastSlash == NULL)
		return strAppID;

	SIZE_T nNameLen = MAX_PATH - (lpLastSlash - szFilePath + 1);
	_tcscpy_s(lpLastSlash + 1, nNameLen, _T("AppID.ini"));

	if (!PathFileExists(szFilePath)) {
		HANDLE handle = CreateFile(szFilePath, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_NEW, 0, NULL);
		CloseHandle(handle);
	}

	TCHAR szAppid[MAX_PATH] = { 0 };
	::GetPrivateProfileString(_T("AppID"), _T("AppID"), NULL, szAppid, MAX_PATH, szFilePath);
	if (_tcslen(szAppid) == 0) {
		::WritePrivateProfileString(_T("AppID"), _T("AppID"), _T(""), szFilePath);
		::ShellExecute(NULL, _T("open"), szFilePath, NULL, NULL, SW_MAXIMIZE);
	}

	strAppID = szAppid;

	return strAppID;
}
