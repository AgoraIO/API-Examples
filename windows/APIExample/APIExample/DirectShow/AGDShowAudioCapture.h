#pragma once
#include <afxwin.h>
#include <atlbase.h>
#include <atlcoll.h>
#include <DShow.h>
#include "IAGDShowDevice.h"
#include "capture-filter.hpp"
#include "IAgoraRtcEngine.h"
#include "IAgoraMediaEngine.h"
class CAGDShowAudioCapture
    : public IDShowCaptureDevice
{
	
public:

    CAGDShowAudioCapture();
    ~CAGDShowAudioCapture();

    virtual BOOL Create();
    virtual void Close();

    virtual BOOL EnumDeviceList();
    virtual int  GetDeviceCount() { return m_listDeviceInfo.GetCount(); };
    virtual BOOL GetDeviceInfo(int nIndex, LPAGORA_DEVICE_INFO lpDeviceInfo);

    virtual BOOL OpenDevice(int nIndex);
    virtual BOOL OpenDevice(LPCTSTR lpDevicePath);
    virtual BOOL GetCurrentDevice(LPTSTR lpDevicePath, SIZE_T *nDevicePathLen);
    virtual void CloseDevice();

    virtual int GetMediaCapCount();
    virtual BOOL GetMediaCap(int nIndex, AM_MEDIA_TYPE **ppMediaType, LPVOID lpMediaStreamConfigCaps, SIZE_T nSize);
    virtual BOOL SelectMediaCap(int nIndex);
    virtual int GetSelectedMediaCap() { return m_nCapSelected; };
    virtual BOOL GetVideoCap(int nIndex, VIDEOINFOHEADER *lpVideoInfo) { return FALSE; }
    BOOL GetCaptureBuffer(SIZE_T *nBlockSize, SIZE_T *nBlockCount, SIZE_T *nAlign);
	BOOL SetCaptureBuffer(SIZE_T nBlockSize, SIZE_T nBlockCount, SIZE_T nAlign);

    virtual BOOL GetCurrentVideoCap(VIDEOINFOHEADER *lpVideoInfo) { return FALSE; }
    virtual BOOL GetCurrentMediaType(AM_MEDIA_TYPE **pMediaType);
    virtual BOOL CreateCaptureFilter();
    virtual BOOL Start();
    virtual void Stop();
   
	virtual BOOL CaptureControl(int nControlCode = DEVICE_START);

    virtual BOOL GetAudioCap(int nIndex, WAVEFORMATEX *lpWaveInfo);
    virtual BOOL GetCurrentAudioCap(WAVEFORMATEX *lpWaveInfo);

	void InitAudioFrame();
	agora::rtc::IRtcEngine* engine_;

	agora::media::IAudioFrameObserver::AudioFrame				m_audioFrame;
protected:
    BOOL ConnectFilters();
    BOOL ConnectPins(const GUID &category, const GUID &type,
        IBaseFilter *filter, IBaseFilter *capture);
    BOOL DisconnectPins();
    void Receive(bool video, IMediaSample *sample);
    void GetDeviceName(LPTSTR deviceName, SIZE_T *nDeviceLen);
    BOOL FindPin(IPin **ppInputPin, IPin **ppOutputPin);
private:
	TCHAR						m_szActiveDeviceID[MAX_PATH];
	CAtlList<AGORA_DEVICE_INFO> m_listDeviceInfo;

	AUDIO_STREAM_CONFIG_CAPS	m_ascStreamCfgCaps;
	int							m_nCapSelected;
	
	// private dshow object
private:
	CComPtr<IGraphBuilder>			m_ptrGraphBuilder;
	CComPtr<ICaptureGraphBuilder2>	m_ptrCaptureGraphBuilder2;
    CComPtr<IMediaControl>          control;
    CComPtr<CaptureFilter>          audioCapture;
    AM_MEDIA_TYPE*                  curMT = nullptr;
    CComPtr<IPin>                   filterPin = nullptr;
    CComPtr<IPin>                   capturePin = nullptr;
    CString                         filterSourceName;
    CString                          filterCaptureName;
    bool active = false;
};
