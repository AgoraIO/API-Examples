#pragma once
#include <map>
#include <vector>
#define Str(key) CConfig::GetInstance()->GetStringValue(key) 

class CConfig
{
public:
    CConfig();
    ~CConfig();
   
    static CConfig* GetInstance();
    CString GetStringValue(CString key);
private:
    
    TCHAR m_szZhConfigFile[MAX_PATH];
    TCHAR m_szEnConfigFile[MAX_PATH];
    bool m_bChinese = false;

    int hashSize = 1000;
};

