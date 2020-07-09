#pragma once

#include <DShow.h>


#include <tchar.h>
#include <windows.h>

#pragma comment(lib, "strmbase.lib")


typedef struct _AGORA_DEVICE_INFO
{
	GUID	guidDevice;
	TCHAR	szDeviceName[MAX_PATH];
	TCHAR	szDevicePath[MAX_PATH];
	TCHAR	szDeviceDesc[MAX_PATH];

} AGORA_DEVICE_INFO, *PAGORA_DEVICE_INFO, *LPAGORA_DEVICE_INFO;

enum {
	DEVICE_START = 0,			// start capture
	DEVICE_STOP,				// stop capture
};

class IDShowCaptureDevice
{
public:

	virtual BOOL Create() = 0;
	virtual void Close() = 0;

	virtual BOOL EnumDeviceList() = 0;
	virtual int GetDeviceCount() = 0;
	virtual BOOL GetDeviceInfo(int nIndex, LPAGORA_DEVICE_INFO lpDeviceInfo) = 0;

	virtual BOOL OpenDevice(int nIndex) = 0;
	
	virtual int GetCurrentDevice(LPTSTR lpDevicePath, SIZE_T *nDevicePathLen) = 0;
	virtual void CloseDevice() = 0;

	virtual int GetMediaCapCount() = 0;
	virtual BOOL GetMediaCap(int nIndex, AM_MEDIA_TYPE **ppMediaType, LPVOID lpMediaStreamConfigCaps, SIZE_T nSize) = 0;
	virtual BOOL SelectMediaCap(int nIndex) = 0;
	virtual int GetSelectedMediaCap() = 0;
	
	virtual BOOL GetVideoCap(int nIndex, VIDEOINFOHEADER *lpVideoInfo) = 0;
	virtual BOOL GetCurrentVideoCap(VIDEOINFOHEADER *lpVideoInfo) = 0;
	virtual BOOL GetCurrentMediaType(AM_MEDIA_TYPE **pMediaType) = 0;
	virtual BOOL CreateCaptureFilter() = 0;
	virtual BOOL Start() = 0;
	virtual void Stop() = 0;
	virtual BOOL GetAudioCap(int nIndex, WAVEFORMATEX *lpWaveInfo) = 0;
	virtual BOOL GetCurrentAudioCap(WAVEFORMATEX *lpWaveInfo) = 0;
};