#pragma once
#include <afx.h>
#include <atlbase.h>
#include <atlcoll.h>
#include "IAGDShowDevice.h"
#include "capture-filter.hpp"
#include <vector>
class CAGDShowVideoCapture
    : public IDShowCaptureDevice
{
public:
	CAGDShowVideoCapture();
	~CAGDShowVideoCapture();

	virtual BOOL Create();
	virtual void Close();

	virtual BOOL EnumDeviceList();
	virtual int  GetDeviceCount() { return m_listDeviceInfo.GetCount(); };
	virtual BOOL GetDeviceInfo(int nIndex, LPAGORA_DEVICE_INFO lpDeviceInfo);

	virtual BOOL OpenDevice(int nIndex);
	
	virtual BOOL GetCurrentDevice(LPTSTR lpDevicePath, SIZE_T *nDevicePathLen);
	virtual void CloseDevice();

	virtual int GetMediaCapCount();
	virtual BOOL GetMediaCap(int nIndex, AM_MEDIA_TYPE **ppMediaType, LPVOID lpMediaStreamConfigCaps, SIZE_T nSize);
	virtual BOOL SelectMediaCap(int nIndex);
	virtual int GetSelectedMediaCap() { return m_nCapSelected; };

    virtual BOOL GetVideoCap(int nIndex, VIDEOINFOHEADER *lpVideoInfo);
    virtual BOOL GetCurrentVideoCap(VIDEOINFOHEADER *lpVideoInfo);

    BOOL RemoveCaptureFilter();
   
    virtual BOOL GetCurrentMediaType(AM_MEDIA_TYPE **pMediaType);
    virtual BOOL CreateCaptureFilter();
    virtual BOOL Start();
    virtual void Stop();
   
    virtual BOOL GetAudioCap(int nIndex, WAVEFORMATEX *lpWaveInfo) { return FALSE; }
    virtual BOOL GetCurrentAudioCap(WAVEFORMATEX *lpWaveInfo) { return FALSE; }
private:
    BOOL ConnectFilters();
    BOOL ConnectPins(const GUID &category, const GUID &type,
        IBaseFilter *filter, IBaseFilter *capture);
    BOOL OpenDevice(LPCTSTR lpDevicePath, LPCTSTR lpDeviceName);
    void GetDeviceName(LPTSTR deviceName, SIZE_T *nDeviceLen);
    void Receive(bool video, IMediaSample *sample);
protected:
	BOOL FindPin(IPin **ppInputPin, IPin **ppOutputPin);

private:
	TCHAR						m_szActiveDeviceID[MAX_PATH];
	CAtlList<AGORA_DEVICE_INFO> m_listDeviceInfo;
	VIDEO_STREAM_CONFIG_CAPS	m_vscStreamCfgCaps;
	int							m_nCapSelected;

private:
	
    CComPtr<IGraphBuilder>     		 m_ptrGraphBuilder;//filter graph
    CComPtr<ICaptureGraphBuilder2>   m_ptrCaptureGraphBuilder2;//filter graph manager
    CComPtr<IMediaControl>           control;
    CComPtr<CaptureFilter>           videoCapture;
    AM_MEDIA_TYPE*                   curMT     = nullptr;
    BITMAPINFOHEADER*                bmiHeader = nullptr;
    bool                             active    = false;
    CString     filterName;

    LPBYTE		m_lpYUVBuffer = nullptr;
    LPBYTE		m_lpY = nullptr;
    LPBYTE		m_lpU = nullptr;
    LPBYTE		m_lpV = nullptr;
};

