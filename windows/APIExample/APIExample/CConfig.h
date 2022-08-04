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
    
    CString m_strZhConfigFile;
	CString m_strEnConfigFile;
    bool m_bChinese = false;

    int hashSize = 1000;
};

