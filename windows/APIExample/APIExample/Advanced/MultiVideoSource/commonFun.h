#ifndef __COMMONFUN_H__
#define __COMMONFUN_H__

#define MAXPATHLEN 512
//#define MAX_DEVICE_ID_LENGTH 128


#include <vector>
//comfun
std::string getAbsoluteDir();
std::string getFilePath();
std::string getCurRunningExeName();
std::string getFileAbsolutePath(const std::string &file);
std::string getPirorDir(const std::string &file);
std::string getPirorDirEx(const std::string &file);
std::string getRootDir(const std::string &file);

std::string int2str(int nNum);
std::string float2str(float fValue);
std::string gbk2utf8(const char *gbk);
std::string utf82gbk(const char *utf8);
std::string gbk2utf8(const std::string &gbk);
std::string utf82gbk(const std::string &utf8);
int str2int(const std::string &str);
int str2long(const std::string &str);
float str2float(const std::string &str);
CString s2cs(const std::string &str);
std::string cs2s(const CString &str);

std::string getTime();
int getProcessID(const std::string &processName);
bool closeProcess(const std::string &processName,int &num);
bool closeProcess(DWORD dwProcess);
int getProcessIdMutil(const std::string &processName);
std::vector<DWORD> getProcessMutilVec(const std::string processName);
void closeCurrentProcess();
bool registerStartUp();
DWORD openProcess(const std::string &processName,const std::string &cmdLine);

//Log

std::string getMediaSdkLogPath(const std::string &strAttribute);


#endif