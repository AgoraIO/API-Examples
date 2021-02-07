#include "stdafx.h"

#include <tlhelp32.h>
#include <WinSock2.h>
#pragma comment(lib,"Ws2_32.lib")
#include <vector>
#include "commonFun.h"
#include <time.h>

std::string getAbsoluteDir()
{
	TCHAR path[MAXPATHLEN] = { 0 };
	GetModuleFileName(nullptr, path, MAXPATHLEN);

	std::string filePath = CStringA(path).GetBuffer();
	return filePath.substr(0, filePath.rfind("\\") + 1);
}

std::string getFilePath()
{
	TCHAR path[MAXPATHLEN] = { 0 };
	GetModuleFileName(nullptr, path, MAXPATHLEN);
	return CStringA(path).GetBuffer();
}

std::string getCurRunningExeName()
{
	TCHAR path[MAXPATHLEN] = { 0 };
	GetModuleFileName(nullptr, path, MAXPATHLEN);

	std::string filePath = CStringA(path).GetBuffer();
	return filePath.substr(filePath.rfind("\\") + 1, filePath.length() - filePath.rfind("\\"));
}

std::string getFileAbsolutePath(const std::string &file)
{
	HMODULE hModule = GetModuleHandle(CString(file.c_str()));
	TCHAR path[MAXPATHLEN] = { 0 };
	GetModuleFileName(hModule, path, MAXPATHLEN);
	return CStringA(path).GetBuffer();
}

std::string getPirorDir(const std::string &file)
{
	HMODULE hModule = GetModuleHandle(CString(file.c_str()));
	TCHAR path[MAXPATHLEN] = { 0 };
	GetModuleFileName(hModule, path, MAXPATHLEN);
	std::string fullpath = CStringA(path).GetBuffer();
	return fullpath.substr(0, fullpath.rfind("\\") + 1);
}

std::string getPirorDirEx(const std::string &file)
{
	return file.substr(0, file.rfind("\\") + 1);
}

std::string getRootDir(const std::string &file)
{
	std::string FileDir = getFileAbsolutePath(file);
	return FileDir.substr(0, FileDir.find("\\") + 1);
}

std::string int2str(int nNum)
{
	char str[MAXPATHLEN] = { 0 };
	_itoa_s(nNum, str, 10);
	return str;
}

std::string float2str(float fValue)
{
	char str[MAXPATHLEN] = { 0 };
	sprintf_s(str, "%f", fValue);
	return str;
}

int str2int(const std::string &str)
{
	return atoi(str.c_str());
}

int str2long(const std::string &str)
{
	return atoll(str.data());
}

float str2float(const std::string &str)
{
	return (float)atof(str.c_str());
}

CString s2cs(const std::string &str)
{
	return CString(str.c_str());
}

std::string cs2s(const CString &str)
{
	CString sTemp(str);
	return CStringA(sTemp.GetBuffer()).GetBuffer();
}

std::string utf82gbk(const char *utf8)
{
	std::string str;

	if (utf8 != NULL)
	{
		int len = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
		std::wstring strW;

		strW.resize(len);

		MultiByteToWideChar(CP_UTF8, 0, utf8, -1, (LPWSTR)strW.data(), len);

		len = WideCharToMultiByte(936, 0, strW.data(), len - 1, NULL, 0, NULL, NULL);	

		str.resize(len);

		WideCharToMultiByte(936, 0, strW.data(), -1, (LPSTR)str.data(), len, NULL, NULL);
	}

	return str;
}

std::string gbk2utf8(const char *gbk)
{
	std::string str;

	if (gbk != NULL)
	{
		int len = MultiByteToWideChar(936, 0, gbk, -1, NULL, 0);
		std::wstring strW;

		strW.resize(len);

		MultiByteToWideChar(936, 0, gbk, -1, (LPWSTR)strW.data(), len);

		len = WideCharToMultiByte(CP_UTF8, 0, strW.data(), len - 1, NULL, 0, NULL, NULL);

		str.resize(len);

		WideCharToMultiByte(CP_UTF8, 0, strW.data(), -1, (LPSTR)str.data(), len, NULL, NULL);
	}

	return str;
}

std::string gbk2utf8(const std::string &gbk)
{
	return gbk2utf8(gbk.c_str());
}

std::string utf82gbk(const std::string &utf8)
{
	return utf82gbk(utf8.c_str());
}

std::string getTime()
{
	SYSTEMTIME st = { 0 };
	GetLocalTime(&st);
	CString timeStr;
	timeStr.Format(_T("%d%02d%02d-%02d%02d%02d"),st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
	return cs2s(timeStr);
}

int getProcessID(const std::string &processName)
{
	HANDLE hProcessSnap = INVALID_HANDLE_VALUE;
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hProcessSnap)
	{
		CloseHandle(hProcessSnap);
		return -1;
	}

	if (!Process32First(hProcessSnap, &pe32))
	{
		CloseHandle(hProcessSnap);
		return -1;
	}
	do
	{
		std::string processNameEnum = CStringA(pe32.szExeFile).GetBuffer();
		if (processNameEnum == processName)
		{
			CloseHandle(hProcessSnap);
			hProcessSnap = INVALID_HANDLE_VALUE;
			return pe32.th32ProcessID;
		}
	} while (Process32Next(hProcessSnap, &pe32));

	CloseHandle(hProcessSnap);
	return -1;
}

bool closeProcess(const std::string &processName,int &num)
{
	DWORD processId = getProcessID(processName);
	HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, false, processId);
	if (INVALID_HANDLE_VALUE != processHandle && processHandle)
	{
		num++;
		if (TerminateProcess(processHandle, 0))
		{
		}
		else
		{
			WaitForSingleObject(processHandle, 2000);
		}

		CloseHandle(processHandle);
	}
	else
	{
		return true;
	}
	return closeProcess(processName,num);
}

bool closeProcess(DWORD dwProcess){

	HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, false, dwProcess);
	if (INVALID_HANDLE_VALUE != processHandle){
		if (TerminateProcess(processHandle, 0)){}
		else
			WaitForSingleObject(processHandle, 2000);

		return CloseHandle(processHandle);
	}

	return false;
}

void closeCurrentProcess()
{
	DWORD processId = GetCurrentProcessId();
	HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, false, processId);
	if (INVALID_HANDLE_VALUE != processHandle)
	{
		if (TerminateProcess(processHandle, 0))
		{
			CloseHandle(processHandle);
			return;
		}
		else
		{
			WaitForSingleObject(processHandle, 2000);
		}
	}

	CloseHandle(processHandle);
	return;
}

int getProcessIdMutil(const std::string &processName)
{
	std::vector<DWORD> vecProcessid;
	HANDLE hProcessSnap = INVALID_HANDLE_VALUE;
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hProcessSnap)
	{
		return vecProcessid.size();
	}
	if (!Process32First(hProcessSnap, &pe32))
	{
		CloseHandle(hProcessSnap);     // Must clean up the snapshot object!
		return vecProcessid.size();
	}

	do
	{
		if (processName == cs2s(pe32.szExeFile)){

			vecProcessid.push_back(pe32.th32ProcessID);
			printf("processName: %s, processId: %d\n", CStringA(pe32.szExeFile).GetBuffer(), pe32.th32ProcessID);
		}

	} while (Process32Next(hProcessSnap, &pe32));

	CloseHandle(hProcessSnap);
	return vecProcessid.size();
}

std::vector<DWORD> getProcessMutilVec(const std::string processName)
{
	std::vector<DWORD> vecProcessid;
	HANDLE hProcessSnap = INVALID_HANDLE_VALUE;
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hProcessSnap)
	{
		return vecProcessid;
	}
	if (!Process32First(hProcessSnap, &pe32))
	{
		CloseHandle(hProcessSnap);     // Must clean up the snapshot object!
		return vecProcessid;
	}

	do
	{
		if (processName == cs2s(pe32.szExeFile)){

			vecProcessid.push_back(pe32.th32ProcessID);
			printf("processName: %s, processId: %d\n", CStringA(pe32.szExeFile).GetBuffer(), pe32.th32ProcessID);
		}

	} while (Process32Next(hProcessSnap, &pe32));

	CloseHandle(hProcessSnap);
	return vecProcessid;
}

bool registerStartUp()
{
	HKEY hKey;

	long lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Run"), 0, KEY_ALL_ACCESS, &hKey);

	if (lRet == ERROR_SUCCESS)
	{
		CString currRunPath = s2cs((getFilePath()));
		lRet = RegSetValueEx(hKey, _T("AgoraWawajiDemo"), 0, REG_SZ, (const unsigned char*)currRunPath.GetBuffer(), (DWORD)(currRunPath.GetLength() * 2));
		currRunPath.ReleaseBuffer();

		RegCloseKey(hKey);
		return TRUE;
	}

	return FALSE;
}

DWORD openProcess(const std::string &processName,const std::string &cmdLine)
{
	STARTUPINFO si;
	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;
	PROCESS_INFORMATION pi;
	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));

	CString CmdLine;
	std::string fullpath = getAbsoluteDir() + processName;
	CmdLine.Format(_T("%s -%s"), s2cs(fullpath), s2cs(cmdLine));
	BOOL res = CreateProcess(NULL, (CmdLine).GetBuffer(), NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL,&si, &pi);
	return pi.dwProcessId;
}

std::string getMediaSdkLogPath(const std::string &strAttribute)
{
	CString strRet;
	std::string strTime;
	std::string exeName;
	std::string pirorDir;

	pirorDir = getPirorDir(getFilePath());
	strTime = getTime();

	exeName.append("Agora_MediaSdk_");
	exeName.append(strAttribute);
	exeName.append(".log");

	strRet.Format(_T("%slogger\\%s_%s"), s2cs(pirorDir), s2cs(strTime), s2cs(exeName));
	CString logPirorDir = s2cs(getPirorDirEx(cs2s(strRet)));
	BOOL res = CreateDirectory(logPirorDir, NULL);

	return cs2s(strRet);
}
