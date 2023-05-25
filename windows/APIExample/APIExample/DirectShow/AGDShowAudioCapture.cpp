#include "AGDShowAudioCapture.h"
#include "DShowHelper.h"
#include "CircleBuffer.hpp"
#include <Dvdmedia.h>

CAGDShowAudioCapture::CAGDShowAudioCapture()
	: m_ptrGraphBuilder(NULL)
	, m_ptrCaptureGraphBuilder2(NULL)
	, m_nCapSelected(-1)
{
	memset(m_szActiveDeviceID, 0, MAX_PATH*sizeof(TCHAR));

    filterSourceName  = _T("Audio Filter");
    filterCaptureName = _T("Audio Capture Filter");
	m_audioFrame.buffer = new BYTE[48000 * 4 * 4];
}


CAGDShowAudioCapture::~CAGDShowAudioCapture()
{
	Close();
	if (m_audioFrame.buffer)
	{
		delete m_audioFrame.buffer;
		m_audioFrame.buffer = nullptr;
	}
}


void CAGDShowAudioCapture::InitAudioFrame()
{
	WAVEFORMATEX	waveFormat;
	GetCurrentAudioCap(&waveFormat);
	int nBufferSize = waveFormat.nAvgBytesPerSec / AUDIO_CALLBACK_TIMES;
	//create capture Buffer.
	SetCaptureBuffer(nBufferSize, 16, waveFormat.nBlockAlign);
	m_audioFrame.avsync_type = 0;
	m_audioFrame.bytesPerSample = agora::rtc::TWO_BYTES_PER_SAMPLE;
	m_audioFrame.type = agora::media::IAudioFrameObserver::FRAME_TYPE_PCM16;
	m_audioFrame.channels = waveFormat.nChannels;
	m_audioFrame.samplesPerSec = waveFormat.nSamplesPerSec;
	m_audioFrame.samplesPerChannel = m_audioFrame.samplesPerSec / 100;

}

BOOL CAGDShowAudioCapture::Create()
{
	HRESULT	    hResult = S_OK;
    BOOL        bRet = FALSE;
	do {
		hResult = m_ptrGraphBuilder.CoCreateInstance(CLSID_FilterGraph);
		ATLASSERT(SUCCEEDED(hResult));
		if (hResult != S_OK)
			break;
		hResult = m_ptrCaptureGraphBuilder2.CoCreateInstance(CLSID_CaptureGraphBuilder2);
		ATLASSERT(SUCCEEDED(hResult));
		if (hResult != S_OK)
			break;

		hResult = m_ptrCaptureGraphBuilder2->SetFiltergraph(m_ptrGraphBuilder);
		ATLASSERT(SUCCEEDED(hResult));
		if (FAILED(hResult))
			break;

        if (S_OK != m_ptrGraphBuilder->QueryInterface(IID_IMediaControl, (void**)&control))
            break;
        bRet = TRUE;

	} while (FALSE);

	return bRet;
}

void CAGDShowAudioCapture::Close()
{
    CComPtr<IEnumFilters>     filterEnum = nullptr;
    HRESULT                  hr;

    if (!m_ptrGraphBuilder)
        return;

    hr = m_ptrGraphBuilder->EnumFilters(&filterEnum);
    if (FAILED(hr))
        return;

    if (capturePin)
        capturePin.Release();

    if (filterPin)
        filterPin.Release();

    CComPtr<IBaseFilter> filter = nullptr;
    while (filterEnum->Next(1, &filter, nullptr) == S_OK) {
        m_ptrGraphBuilder->RemoveFilter(filter);
        filterEnum->Reset();
        filter.Release();
    }

    control.Release();
    m_ptrGraphBuilder.Release();
    m_ptrCaptureGraphBuilder2.Release();
}

BOOL CAGDShowAudioCapture::EnumDeviceList()
{
    HRESULT		hResult = S_OK;

    CComVariant		var;
    WCHAR			*wszDevicePath = NULL;

    CComPtr<ICreateDevEnum>		ptrCreateDevEnum = NULL;
    CComPtr<IEnumMoniker>		ptrEnumMoniker = NULL;
    CComPtr<IMoniker>			ptrMoniker = NULL;

    AGORA_DEVICE_INFO			agDeviceInfo;

    hResult = ptrCreateDevEnum.CoCreateInstance(CLSID_SystemDeviceEnum);
    if (FAILED(hResult))
        return FALSE;

    hResult = ptrCreateDevEnum->CreateClassEnumerator(CLSID_AudioInputDeviceCategory, &ptrEnumMoniker, 0);
    if (hResult != S_OK)
        return FALSE;

    m_listDeviceInfo.RemoveAll();

    do {
        ptrMoniker = NULL;
        hResult = ptrEnumMoniker->Next(1, &ptrMoniker, NULL);
        if (hResult != S_OK)
            break;

        IBaseFilter*  filter;
        if (SUCCEEDED(ptrMoniker->BindToObject(NULL, 0, IID_IBaseFilter,
            (void**)&filter))) {
            CComPtr<IPropertyBag>		ptrPropertyBag = NULL;
            hResult = ptrMoniker->BindToStorage(NULL, NULL, IID_IPropertyBag, (void**)(&ptrPropertyBag));
            if (hResult != S_OK)
                break;

            memset(&agDeviceInfo, 0, sizeof(AGORA_DEVICE_INFO));

            var.Clear();
            hResult = ptrPropertyBag->Read(L"FriendlyName", &var, NULL);
            if (SUCCEEDED(hResult)) {
#ifdef UNICODE
                _tcscpy_s(agDeviceInfo.szDeviceName, var.bstrVal);
#else
                ::WideCharToMultiByte(CP_ACP, 0, var.bstrVal, -1, agDeviceInfo.szDeviceName, MAX_PATH, NULL, NULL);
#endif
            }

            hResult = ptrMoniker->GetDisplayName(NULL, NULL, &wszDevicePath);
            if (SUCCEEDED(hResult)) {
#ifdef UNICODE
                _tcscpy_s(agDeviceInfo.szDevicePath, wszDevicePath);
#else
                ::WideCharToMultiByte(CP_ACP, 0, wszDevicePath, -1, agDeviceInfo.szDevicePath, MAX_PATH, NULL, NULL);
#endif
                ::CoTaskMemFree(wszDevicePath);
            }

            m_listDeviceInfo.AddTail(agDeviceInfo);
        }
    } while (TRUE);

    return TRUE;
}

    BOOL CAGDShowAudioCapture::GetDeviceInfo(int nIndex, LPAGORA_DEVICE_INFO lpDeviceInfo)
{
	ATLASSERT(lpDeviceInfo != NULL);
	ATLASSERT(nIndex >= 0 && nIndex < static_cast<int>(m_listDeviceInfo.GetCount()));

	POSITION pos = m_listDeviceInfo.FindIndex(nIndex);
	if (pos == NULL)
		return FALSE;

	AGORA_DEVICE_INFO &agDeviceInfo = m_listDeviceInfo.GetAt(pos);
	memcpy(lpDeviceInfo, &agDeviceInfo, sizeof(AGORA_DEVICE_INFO));

	return TRUE;
}

BOOL CAGDShowAudioCapture::OpenDevice(int nIndex)
{
	ATLASSERT(nIndex >= 0 && nIndex < static_cast<int>(m_listDeviceInfo.GetCount()));

	m_nCapSelected = -1;
	POSITION pos = m_listDeviceInfo.FindIndex(nIndex);
	if (pos == NULL)
		return FALSE;

	LPCTSTR	lpDevicePath = m_listDeviceInfo.GetAt(pos).szDevicePath;
	
	return OpenDevice(lpDevicePath);
}

BOOL CAGDShowAudioCapture::OpenDevice(LPCTSTR lpDevicePath)
{
	ULONG		ulEaten = 0;
	HRESULT		hResult = S_OK;

	CComPtr<IBindCtx>		lpBindCtx = NULL;
	CComPtr<IMoniker>		ptrMoniker = NULL;
	CComPtr<IBaseFilter>	ptrSourceFilter = NULL;

	m_nCapSelected = -1;

	ATLASSERT(_tcslen(m_szActiveDeviceID) == 0);

	hResult = ::CreateBindCtx(0, &lpBindCtx);
	if (hResult != S_OK)
		return FALSE;

#ifdef UNICODE
	hResult = ::MkParseDisplayName(lpBindCtx, lpDevicePath, &ulEaten, &ptrMoniker);
#else
	WCHAR wszDeviceID[128];
	memset(wszDeviceID, 0, 128 * sizeof(WCHAR));
	::MultiByteToWideChar(CP_ACP, 0, lpDevicePath, -1, wszDeviceID, 128);
	hResult = ::MkParseDisplayName(lpBindCtx, wszDeviceID, &ulEaten, &ptrMoniker);
#endif

	if (hResult != S_OK)
		return FALSE;

	hResult = ptrMoniker->BindToObject(0, 0, IID_IBaseFilter, (void **)&ptrSourceFilter);
	ATLASSERT(SUCCEEDED(hResult));
	if (hResult != S_OK)
		return FALSE;

	hResult = m_ptrGraphBuilder->AddFilter(ptrSourceFilter, filterSourceName);
	ATLASSERT(SUCCEEDED(hResult));
	if (hResult != S_OK)
		return FALSE;

	_tcscpy_s(m_szActiveDeviceID, MAX_PATH, lpDevicePath);
	SelectMediaCap(0);

	return TRUE;
}

BOOL CAGDShowAudioCapture::GetCurrentDevice(LPTSTR lpDevicePath, SIZE_T *nDevicePathLen)
{
	int nDeviceLen = _tcslen(m_szActiveDeviceID);
	if (nDeviceLen >= static_cast<int>(*nDevicePathLen)) {
		*nDevicePathLen = nDeviceLen + 1;
		return FALSE;
	}

	if (nDeviceLen == 0)
		return FALSE;

	_tcscpy_s(lpDevicePath, *nDevicePathLen, m_szActiveDeviceID);
	*nDevicePathLen = nDeviceLen + 1;

	return TRUE;
}

void CAGDShowAudioCapture::CloseDevice()
{
	CComPtr<IBaseFilter>	ptrSourceFilter = NULL;
	
	HRESULT hResult = m_ptrGraphBuilder->FindFilterByName(filterSourceName, &ptrSourceFilter);

	if (SUCCEEDED(hResult)) {
		m_ptrGraphBuilder->RemoveFilter(ptrSourceFilter);
		m_nCapSelected = -1;
	}

	memset(m_szActiveDeviceID, 0, MAX_PATH*sizeof(TCHAR));
}

int CAGDShowAudioCapture::GetMediaCapCount()
{
	int		nCount = 0;
	int		nSize = 0;
	HRESULT hResult = S_OK;

	CComPtr<IBaseFilter>			ptrCaptureFilter = NULL;
	CComPtr<IAMStreamConfig>		ptrStreamConfig = NULL;

	hResult = m_ptrGraphBuilder->FindFilterByName(filterSourceName, &ptrCaptureFilter);
	ATLASSERT(SUCCEEDED(hResult));
	if (FAILED(hResult))
		return 0;

	hResult = m_ptrCaptureGraphBuilder2->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Audio, ptrCaptureFilter, IID_IAMStreamConfig, (void**)&ptrStreamConfig);
	ATLASSERT(SUCCEEDED(hResult));
	if (FAILED(hResult))
		return 0;

	hResult = ptrStreamConfig->GetNumberOfCapabilities(&nCount, &nSize);
	ATLASSERT(SUCCEEDED(hResult));
	if (FAILED(hResult))
		return 0;

	return nCount;
}

BOOL CAGDShowAudioCapture::GetMediaCap(int nIndex, AM_MEDIA_TYPE **ppMediaType, LPVOID lpMediaStreamConfigCaps, SIZE_T nSize)
{
	int		nCount = 0;
	int		nCapSize = 0;
	HRESULT hResult = S_OK;

	CComPtr<IBaseFilter>			ptrCaptureFilter = NULL;
	CComPtr<IAMStreamConfig>		ptrStreamConfig = NULL;

	hResult = m_ptrGraphBuilder->FindFilterByName(filterSourceName, &ptrCaptureFilter);
	ATLASSERT(SUCCEEDED(hResult));
	if (FAILED(hResult))
		return FALSE;

	hResult = m_ptrCaptureGraphBuilder2->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Audio, ptrCaptureFilter, IID_IAMStreamConfig, (void**)&ptrStreamConfig);
	ATLASSERT(SUCCEEDED(hResult));
	if (FAILED(hResult))
		return FALSE;

	hResult = ptrStreamConfig->GetNumberOfCapabilities(&nCount, &nCapSize);
	ATLASSERT(SUCCEEDED(hResult));
	if (FAILED(hResult))
		return FALSE;

	ATLASSERT(nCapSize <= static_cast<int>(nSize));
	if (nCapSize > static_cast<int>(nSize))
		return FALSE;

	hResult = ptrStreamConfig->GetStreamCaps(nIndex, ppMediaType, reinterpret_cast<BYTE*>(lpMediaStreamConfigCaps));
	ATLASSERT(SUCCEEDED(hResult));
	if (FAILED(hResult))
		return FALSE;
	
	return TRUE;
}

BOOL CAGDShowAudioCapture::GetAudioCap(int nIndex, WAVEFORMATEX *lpWaveInfo)
{
	int		nCount = 0;
	int		nSize = 0;

	AM_MEDIA_TYPE *lpAMMediaType = NULL;
	AUDIO_STREAM_CONFIG_CAPS audioStreamCfgCaps;

	BOOL bSuccess = GetMediaCap(nIndex, &lpAMMediaType, &audioStreamCfgCaps, sizeof(AUDIO_STREAM_CONFIG_CAPS));

	if (bSuccess && lpAMMediaType->formattype == FORMAT_WaveFormatEx) {
		WAVEFORMATEX* pWavInfo = reinterpret_cast<WAVEFORMATEX*>(lpAMMediaType->pbFormat);
		memcpy_s(lpWaveInfo, sizeof(WAVEFORMATEX), pWavInfo, sizeof(WAVEFORMATEX));
	}
	else
		bSuccess = FALSE;

    CDShowHelper::FreeMediaType(lpAMMediaType);

	return bSuccess;
}

BOOL CAGDShowAudioCapture::GetCurrentAudioCap(WAVEFORMATEX *lpWaveInfo)
{
	BOOL			bSuccess = FALSE;
	HRESULT			hResult = S_OK;
	AM_MEDIA_TYPE	*lpAMMediaType = NULL;

	CComPtr<IBaseFilter>			ptrCaptureFilter = NULL;
	CComPtr<IAMStreamConfig>		ptrStreamConfig = NULL;

	hResult = m_ptrGraphBuilder->FindFilterByName(filterSourceName, &ptrCaptureFilter);
	ATLASSERT(SUCCEEDED(hResult));
	if (FAILED(hResult))
		return FALSE;

	hResult = m_ptrCaptureGraphBuilder2->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Audio, ptrCaptureFilter, IID_IAMStreamConfig, (void**)&ptrStreamConfig);
	ATLASSERT(SUCCEEDED(hResult));
	if (FAILED(hResult))
		return FALSE;

	hResult = ptrStreamConfig->GetFormat(&lpAMMediaType);
	ATLASSERT(SUCCEEDED(hResult));
	if (FAILED(hResult))
		return FALSE;

	if (lpAMMediaType->formattype == FORMAT_WaveFormatEx) {
		WAVEFORMATEX* pWavInfo = reinterpret_cast<WAVEFORMATEX*>(lpAMMediaType->pbFormat);
		memcpy_s(lpWaveInfo, sizeof(WAVEFORMATEX), pWavInfo, sizeof(WAVEFORMATEX));

		bSuccess = TRUE;
	}
	else
		bSuccess = FALSE;

    CDShowHelper::FreeMediaType(lpAMMediaType);

	return bSuccess;
}

BOOL CAGDShowAudioCapture::SelectMediaCap(int nIndex)
{
	int		nCount = 0;
	int		nSize = 0;
	HRESULT hResult = S_OK;

	AM_MEDIA_TYPE	*lpMediaType = NULL;

	CComPtr<IBaseFilter>		ptrCaptureFilter = NULL;
	CComPtr<IAMStreamConfig>	ptrStreamConfig = NULL;

	hResult = m_ptrGraphBuilder->FindFilterByName(filterSourceName, &ptrCaptureFilter);
	ATLASSERT(SUCCEEDED(hResult));
	if (FAILED(hResult))
		return FALSE;

	hResult = m_ptrCaptureGraphBuilder2->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Audio, ptrCaptureFilter, IID_IAMStreamConfig, (void**)&ptrStreamConfig);
	ATLASSERT(SUCCEEDED(hResult));
	if (FAILED(hResult))
		return FALSE;

	hResult = ptrStreamConfig->GetNumberOfCapabilities(&nCount, &nSize);
	ATLASSERT(SUCCEEDED(hResult));
	if (FAILED(hResult))
		return FALSE;

	ATLASSERT(nIndex >= 0 && nIndex < nCount);
	if (nIndex < 0 || nIndex >= nCount)
		nIndex = 0;

	ATLASSERT(nSize <= sizeof(AUDIO_STREAM_CONFIG_CAPS));
	
	do {
		hResult = ptrStreamConfig->GetStreamCaps(nIndex, &lpMediaType, reinterpret_cast<BYTE*>(&m_ascStreamCfgCaps));
		ATLASSERT(SUCCEEDED(hResult));
		if (FAILED(hResult))
			break;

		hResult = ptrStreamConfig->SetFormat(lpMediaType);
		ATLASSERT(SUCCEEDED(hResult));
		if (FAILED(hResult))
			break;

		m_nCapSelected = nIndex;

	} while (FALSE);

	CDShowHelper::FreeMediaType(lpMediaType);

	return SUCCEEDED(hResult);
}

BOOL CAGDShowAudioCapture::FindPin(IPin **ppInputPin, IPin **ppOutputPin)
{
	HRESULT hResult = S_OK;

	CComPtr<IBaseFilter> ptrCaptureFilter = NULL;
	CComPtr<IBaseFilter> ptrSourceFilter = NULL;

	hResult = m_ptrGraphBuilder->FindFilterByName(filterCaptureName, &ptrCaptureFilter);
	ATLASSERT(SUCCEEDED(hResult));
	if (FAILED(hResult))
		return FALSE;

	hResult = m_ptrGraphBuilder->FindFilterByName(filterSourceName, &ptrSourceFilter);
	ATLASSERT(SUCCEEDED(hResult));
	if (FAILED(hResult))
		return FALSE; 

	return TRUE;
}

BOOL CAGDShowAudioCapture::GetCaptureBuffer(SIZE_T *nBlockSize, SIZE_T *nBlockCount, SIZE_T *nAlign)
{
	HRESULT							hResult = S_OK;
	CComPtr<IBaseFilter>			ptrCaptureFilter = NULL;
	CComPtr<IAMBufferNegotiation>	ptrBufferNegotiation = NULL;

	hResult = m_ptrGraphBuilder->FindFilterByName(L"Source", &ptrCaptureFilter);
	ATLASSERT(SUCCEEDED(hResult));
	if (FAILED(hResult))
		return FALSE;

	hResult = m_ptrCaptureGraphBuilder2->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Audio, ptrCaptureFilter, IID_IAMBufferNegotiation, (void**)&ptrBufferNegotiation);
	ATLASSERT(SUCCEEDED(hResult));
	if (FAILED(hResult))
		return FALSE;

	ALLOCATOR_PROPERTIES alcProper;

	memset(&alcProper, 0, sizeof(ALLOCATOR_PROPERTIES));
	hResult = ptrBufferNegotiation->GetAllocatorProperties(&alcProper);
	ATLASSERT(SUCCEEDED(hResult));
	if (FAILED(hResult))
		return FALSE;

	*nBlockSize = alcProper.cbBuffer;
	*nBlockCount = alcProper.cBuffers;
	*nAlign = alcProper.cbAlign;

	return TRUE;
}

BOOL CAGDShowAudioCapture::SetCaptureBuffer(SIZE_T nBlockSize, SIZE_T nBlockCount, SIZE_T nAlign)
{
	HRESULT							hResult = S_OK;
	CComPtr<IBaseFilter>			ptrCaptureFilter = NULL;
	CComPtr<IAMBufferNegotiation>	ptrBufferNegotiation = NULL;

	hResult = m_ptrGraphBuilder->FindFilterByName(filterSourceName, &ptrCaptureFilter);
	ATLASSERT(SUCCEEDED(hResult));
	if (FAILED(hResult))
		return FALSE;

	hResult = m_ptrCaptureGraphBuilder2->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Audio, ptrCaptureFilter, IID_IAMBufferNegotiation, (void**)&ptrBufferNegotiation);
	ATLASSERT(SUCCEEDED(hResult));
	if (FAILED(hResult))
		return FALSE;

	ALLOCATOR_PROPERTIES alcProper;

	alcProper.cbBuffer = nBlockSize;
	alcProper.cBuffers = nBlockCount;
	alcProper.cbAlign = nAlign;
	alcProper.cbPrefix = 0;

	hResult = ptrBufferNegotiation->SuggestAllocatorProperties(&alcProper);
	ATLASSERT(SUCCEEDED(hResult));
	if (FAILED(hResult))
		return FALSE;

	return TRUE;
}

BOOL CAGDShowAudioCapture::CaptureControl(int nControlCode)
{
	CComPtr<IPin> ptrInputPin = NULL;
	CComPtr<IPin> ptrOutputPin = NULL;

	CComQIPtr<IMediaControl, &IID_IMediaControl> ptrControl(m_ptrGraphBuilder);
	
	FindPin(&ptrInputPin, &ptrOutputPin);

	HRESULT hResult = S_OK;

	switch (nControlCode)
	{
	case DEVICE_START:
		m_ptrGraphBuilder->Connect(ptrInputPin, ptrOutputPin);
		hResult = ptrControl->Run();
		break;
	case DEVICE_STOP:
		hResult = ptrControl->Stop();
		ATLASSERT(SUCCEEDED(hResult));
		if (FAILED(hResult))
			return FALSE;

		hResult = m_ptrGraphBuilder->Disconnect(ptrInputPin);
		hResult = m_ptrGraphBuilder->Disconnect(ptrOutputPin);
		break;
	default:
		ATLASSERT(FALSE);
		break;
	}
	
	ATLASSERT(SUCCEEDED(hResult));

	return hResult == S_OK ? TRUE : FALSE;
}

BOOL CAGDShowAudioCapture::GetCurrentMediaType(AM_MEDIA_TYPE **pMediaType)
{
    BOOL			bSuccess = FALSE;
    HRESULT			hResult = S_OK;

    CComPtr<IBaseFilter>			ptrCaptureFilter = nullptr;
    CComPtr<IAMStreamConfig>		ptrStreamConfig = nullptr;

    hResult = m_ptrGraphBuilder->FindFilterByName(filterSourceName, &ptrCaptureFilter);
    ATLASSERT(SUCCEEDED(hResult));
    if (FAILED(hResult))
        return FALSE;

    hResult = m_ptrCaptureGraphBuilder2->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Audio, ptrCaptureFilter, IID_IAMStreamConfig, (void**)&ptrStreamConfig);
    ATLASSERT(SUCCEEDED(hResult));
    if (FAILED(hResult))
        return FALSE;

    hResult = ptrStreamConfig->GetFormat(pMediaType);
    ATLASSERT(SUCCEEDED(hResult));
    if (FAILED(hResult))
        return FALSE;

    return TRUE;
}
BOOL CAGDShowAudioCapture::CreateCaptureFilter()
{
    if (audioCapture) {
        m_ptrGraphBuilder->RemoveFilter(audioCapture);
        audioCapture.Release();
    }

    AM_MEDIA_TYPE* mt;
    if (GetCurrentMediaType(&mt)) {
        PinCaptureInfo info;
        info.callback          = [this](IMediaSample *s) {Receive(false, s); };
        info.expectedMajorType = mt->majortype;
        info.expectedSubType   = mt->subtype;
        audioCapture           = new CaptureFilter(info);

        HRESULT hr = m_ptrGraphBuilder->AddFilter(audioCapture, filterCaptureName);
		IBaseFilter* filter;

		m_ptrGraphBuilder->FindFilterByName(filterCaptureName,&filter);
        if (SUCCEEDED(hr))
            return TRUE;

        CDShowHelper::FreeMediaType(mt);
    }

    return FALSE;
}
BOOL CAGDShowAudioCapture::Start()
{
    if (ConnectFilters()) {
        control->Run();
        active = true;
        return TRUE;
    }
    return FALSE;
}
void CAGDShowAudioCapture::Stop()
{
    if (active) {
        control->Stop();
        active = false;
    }

    DisconnectPins();
}

void CAGDShowAudioCapture::Receive(bool video, IMediaSample *sample)
{
    BYTE *pBuffer;
    if (!sample || !engine_)
        return;
    
    int size = sample->GetActualDataLength();
    if (!size)
        return;

    if (FAILED(sample->GetPointer(&pBuffer)))
        return;
	m_audioFrame.renderTimeMs = GetTickCount64();
	agora::util::AutoPtr<agora::media::IMediaEngine> mediaEngine;
	//query interface agora::AGORA_IID_MEDIA_ENGINE in the engine.
	mediaEngine.queryInterface(engine_, agora::rtc::AGORA_IID_MEDIA_ENGINE);
	int fps = m_audioFrame.samplesPerSec / m_audioFrame.samplesPerChannel;
	memcpy(m_audioFrame.buffer,pBuffer , size);
	SIZE_T nSize = m_audioFrame.samplesPerChannel *  m_audioFrame.channels *  m_audioFrame.bytesPerSample;
	mediaEngine->pushAudioFrame(&m_audioFrame);

    //CircleBuffer::GetInstance()->writeBuffer(pBuffer, size, GetTickCount());
}

void CAGDShowAudioCapture::GetDeviceName(LPTSTR deviceName, SIZE_T *nDeviceLen)
{
    for (size_t i = 0; i < m_listDeviceInfo.GetCount(); ++i) {
        POSITION pos = m_listDeviceInfo.FindIndex(i);
        AGORA_DEVICE_INFO &agDeviceInfo = m_listDeviceInfo.GetAt(pos);
        if (_tcscmp(m_szActiveDeviceID, agDeviceInfo.szDevicePath) == 0) {
            *nDeviceLen = _tcslen(agDeviceInfo.szDeviceName);
            _tcscpy_s(deviceName, *nDeviceLen + 1, agDeviceInfo.szDeviceName);

            break;
        }
    }
}

BOOL CAGDShowAudioCapture::ConnectFilters()
{
    CComPtr<IBaseFilter>	filter = nullptr;
    HRESULT hResult = m_ptrGraphBuilder->FindFilterByName(filterSourceName, &filter);
    TCHAR deviceName[MAX_PATH] = { 0 };
    SIZE_T len = 0;
    GetDeviceName(deviceName, &len);
    if (SUCCEEDED(hResult) && filter && audioCapture) {
        bool success = ConnectPins(PIN_CATEGORY_CAPTURE,
            MEDIATYPE_Audio, filter,
            audioCapture);
        return TRUE;
    }

    return FALSE;
}

BOOL CAGDShowAudioCapture::ConnectPins(const GUID &category, const GUID &type,
    IBaseFilter *filter, IBaseFilter *capture)
{
    HRESULT hr = S_OK;

    if (!CDShowHelper::GetFilterPin(filter, type, category, PINDIR_OUTPUT, &filterPin)) {
        OutputDebugString(L"Failed to find pin");
        return FALSE;
    }

    if (!CDShowHelper::GetPinByName(capture, PINDIR_INPUT, nullptr, &capturePin)) {
        OutputDebugString(L"Failed to find capture pin");
        return FALSE;
    }
    OutputDebugString(L"ConnectDirect\n");
    hr = m_ptrGraphBuilder->ConnectDirect(filterPin, capturePin, nullptr);
    if (FAILED(hr)) {
        OutputDebugString(L"failed to connect pins");
        return FALSE;
    }

    return TRUE;
}

BOOL CAGDShowAudioCapture::DisconnectPins()
{
    if (!m_ptrGraphBuilder)
        return FALSE;

    if (!capturePin && !filterPin)
        return FALSE;
    HRESULT hr = S_FALSE;
    if (capturePin) {
        hr = m_ptrGraphBuilder->Disconnect(capturePin);
        capturePin.Release();
    }
    ATLASSERT(SUCCEEDED(hr));

    if (filterPin) {
        hr = m_ptrGraphBuilder->Disconnect(filterPin);
        filterPin.Release();
    }
    ATLASSERT(SUCCEEDED(hr));
    return TRUE;
}