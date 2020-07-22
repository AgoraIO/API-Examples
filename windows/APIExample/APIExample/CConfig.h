#pragma once
#include <map>
#include <vector>
#define Str(key) CConfig::GetInstance()->GetStringValue(key) 
#define GET_APP_ID cs2utf8(CConfig::GetInstance()->GetAPP_ID())


class CConfig
{
public:
    CConfig();
    ~CConfig();
   
    static CConfig* GetInstance();
    CString GetStringValue(CString key);
	CString GetAPP_ID();
private:
    
    TCHAR m_szZhConfigFile[MAX_PATH];
    TCHAR m_szEnConfigFile[MAX_PATH];
    bool m_bChinese = false;

    int hashSize = 1000;
};

