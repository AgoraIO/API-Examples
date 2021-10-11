#include "stdafx.h"
#include "CConfig.h"
CConfig* CConfig::GetInstance()
{
    static CConfig config;
    return &config;
}

CConfig::CConfig()
{
	TCHAR szZhConfigFile[MAX_PATH] = { 0 };
	TCHAR szEnConfigFile[MAX_PATH] = { 0 };
    ::GetModuleFileName(NULL, szZhConfigFile, MAX_PATH);
    LPTSTR lpLastSlash = _tcsrchr(szZhConfigFile, _T('\\')) + 1;
	*lpLastSlash = 0;
	m_strZhConfigFile = szZhConfigFile;
	m_strZhConfigFile += _T("zh-cn.ini");

    if (::GetFileAttributes(m_strZhConfigFile) == INVALID_FILE_ATTRIBUTES) {
        HANDLE hFile = ::CreateFile(m_strZhConfigFile, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        ::CloseHandle(hFile);
    }

    ::GetModuleFileName(NULL, szEnConfigFile, MAX_PATH);
    LPTSTR lpLastSlashEn = _tcsrchr(szEnConfigFile, _T('\\')) + 1;
	*lpLastSlashEn = 0;
	m_strEnConfigFile = szEnConfigFile;
	m_strEnConfigFile += _T("en.ini");

    if (::GetFileAttributes(m_strEnConfigFile) == INVALID_FILE_ATTRIBUTES) {
        HANDLE hFile = ::CreateFile(m_strEnConfigFile, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        ::CloseHandle(hFile);
    }
    LCID lcid = GetUserDefaultLCID();//LCID https://www.science.co.il/language/Locale-codes.php
    if (lcid == 2052) {//chinese  
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
        ::GetPrivateProfileString(_T("General"), key, _T("Unknown"), strValue.GetBuffer(MAX_PATH), MAX_PATH, m_strZhConfigFile);
    else
        ::GetPrivateProfileString(_T("General"), key, _T("Unknown"), strValue.GetBuffer(MAX_PATH), MAX_PATH, m_strEnConfigFile);
    return strValue;
}


CString CConfig::GetAPP_ID()
{
	CString strAppID(APP_ID);
	if (strAppID.Compare(_T("<enter your agora app id>")))
		return strAppID;
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
