#include "stdafx.h"
#include "CConfig.h"
CConfig* CConfig::GetInstance()
{
    static CConfig config;
    return &config;
}

CConfig::CConfig()
{
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
        ::GetPrivateProfileString(_T("General"), key, _T("Unknown"), strValue.GetBuffer(MAX_PATH), MAX_PATH, m_szZhConfigFile);
    else
        ::GetPrivateProfileString(_T("General"), key, _T("Unknown"), strValue.GetBuffer(MAX_PATH), MAX_PATH, m_szEnConfigFile);
    return strValue;
}