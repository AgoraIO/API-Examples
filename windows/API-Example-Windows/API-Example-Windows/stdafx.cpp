
// stdafx.cpp : source file that includes just the standard includes
// API-Example-Windows.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"


std::string cs2utf8(CString str)
{
    char szBuf[2*MAX_PATH] = { 0 };
    WideCharToMultiByte(CP_UTF8, 0, str.GetBuffer(0), str.GetLength(), szBuf, 2*MAX_PATH, NULL, NULL);
    return szBuf;
}

CString utf82cs(std::string utf8)
{
    TCHAR szBuf[2 * MAX_PATH] = { 0 };
    MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), 2 * MAX_PATH, szBuf, 2 * MAX_PATH);
    return szBuf;
}