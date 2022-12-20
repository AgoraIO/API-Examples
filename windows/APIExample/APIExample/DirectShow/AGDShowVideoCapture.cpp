#define HAVE_JPEG

#include "AGDShowVideoCapture.h"
#include "DShowHelper.h"
#include "AgVideoBuffer.h"
#include "libyuv.h"
#include <Dvdmedia.h>
#ifdef DEBUG
#pragma comment(lib, "yuv.lib")
#pragma comment(lib, "jpeg-static.lib")
#else
#pragma comment(lib, "yuv.lib")
#pragma comment(lib, "jpeg-static.lib")
#endif

using namespace libyuv;
#define MAX_VIDEO_BUFFER_SIZE (4*1920*1080*4) //4K RGBA max size
CAGDShowVideoCapture::CAGDShowVideoCapture()
	: m_ptrGraphBuilder(nullptr)
	, m_ptrCaptureGraphBuilder2(nullptr)
	, m_nCapSelected(-1)
{
	memset(m_szActiveDeviceID, 0, MAX_PATH*sizeof(TCHAR));
    m_lpYUVBuffer = new BYTE[MAX_VIDEO_BUFFER_SIZE];
    filterName = L"Video Filter";
}


CAGDShowVideoCapture::~CAGDShowVideoCapture()
{
    Close();
    if (m_lpYUVBuffer) {
        delete[] m_lpYUVBuffer;
        m_lpYUVBuffer = nullptr;
    }
}

BOOL CAGDShowVideoCapture::Create()
{
	HRESULT			hResult = S_OK;
    BOOL bRet = FALSE;
    do {
        if (S_OK != CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER
            , IID_IFilterGraph, (void**)&m_ptrGraphBuilder))
            break;
       
        if (S_OK != CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC_SERVER
            , IID_ICaptureGraphBuilder2, (void**)&m_ptrCaptureGraphBuilder2))
            break;

        if (S_OK != m_ptrCaptureGraphBuilder2->SetFiltergraph(m_ptrGraphBuilder))
            break;

        if (S_OK != m_ptrGraphBuilder->QueryInterface(IID_IMediaControl, (void**)&control))
            break;

        bRet = TRUE;
    } while (false);
	return bRet;
}

void CAGDShowVideoCapture::Close()
{
    CComPtr<IEnumFilters>     filterEnum = nullptr;
    HRESULT                  hr;

    if (!m_ptrGraphBuilder)
        return;

    hr = m_ptrGraphBuilder->EnumFilters(&filterEnum);
    if (FAILED(hr))
        return;

    CComPtr<IBaseFilter> filter = nullptr;
    while (filterEnum->Next(1, &filter, nullptr) == S_OK) {
        m_ptrGraphBuilder->RemoveFilter(filter);
        filterEnum->Reset();
        filter.Release();
    }

    m_ptrGraphBuilder.Release();
    m_ptrCaptureGraphBuilder2.Release();
  
}

BOOL CAGDShowVideoCapture::EnumDeviceList()
{
	HRESULT		hResult = S_OK;

	CComVariant		var;
	WCHAR			*wszDevicePath = nullptr;

    CComPtr<ICreateDevEnum>     ptrCreateDevEnum = nullptr;
    CComPtr<IEnumMoniker>       ptrEnumMoniker   = nullptr;
    CComPtr<IMoniker>    	    ptrMoniker       = nullptr;
   
	AGORA_DEVICE_INFO			agDeviceInfo;

	hResult = CoCreateInstance(CLSID_SystemDeviceEnum, NULL,
        CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (void**)&ptrCreateDevEnum);
	if (FAILED(hResult))
		return FALSE;

	hResult = ptrCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &ptrEnumMoniker, 0);
	if (FAILED(hResult))
		return FALSE;

	m_listDeviceInfo.RemoveAll();

    do {

        hResult = ptrEnumMoniker->Next(1, &ptrMoniker, nullptr);
        if (hResult != S_OK)
            break;
        IBaseFilter*  filter;
        if (SUCCEEDED(ptrMoniker->BindToObject(NULL, 0, IID_IBaseFilter,
            (void**)&filter))) {
            CComPtr<IPropertyBag>       ptrPropertyBag = nullptr;

            hResult = ptrMoniker->BindToStorage(nullptr, nullptr, IID_IPropertyBag, (void**)(&ptrPropertyBag));
            if (hResult != S_OK)
                break;

            memset(&agDeviceInfo, 0, sizeof(AGORA_DEVICE_INFO));

            var.Clear();
            hResult = ptrPropertyBag->Read(L"FriendlyName", &var, nullptr);



            if (SUCCEEDED(hResult)) {
#ifdef UNICODE
                _tcscpy_s(agDeviceInfo.szDeviceName, var.bstrVal);
#else
                ::WideCharToMultiByte(CP_ACP, 0, var.bstrVal, -1, agDeviceInfo.szDeviceName, MAX_PATH, nullptr, nullptr);
#endif
            }
            var.Clear();
            hResult = ptrPropertyBag->Read(_T("DevicePath"), &var, nullptr);
            if (SUCCEEDED(hResult)) {
                _tcscpy_s(agDeviceInfo.szDevicePath, var.bstrVal);
            }

            m_listDeviceInfo.AddTail(agDeviceInfo);
        }
        if (ptrMoniker)
            ptrMoniker.Release();

    } while (TRUE);

	return TRUE;
}

BOOL CAGDShowVideoCapture::GetDeviceInfo(int nIndex, LPAGORA_DEVICE_INFO lpDeviceInfo)
{
	ATLASSERT(lpDeviceInfo != nullptr);
	ATLASSERT(nIndex >= 0 && nIndex < static_cast<int>(m_listDeviceInfo.GetCount()));

	POSITION pos = m_listDeviceInfo.FindIndex(nIndex);
	if (pos == nullptr)
		return FALSE;

	AGORA_DEVICE_INFO &agDeviceInfo = m_listDeviceInfo.GetAt(pos);
	memcpy(lpDeviceInfo, &agDeviceInfo, sizeof(AGORA_DEVICE_INFO));

	return TRUE;
}

BOOL CAGDShowVideoCapture::OpenDevice(int nIndex)
{
	ATLASSERT(nIndex >= 0 && nIndex < static_cast<int>(m_listDeviceInfo.GetCount()) );

	m_nCapSelected = -1;
	POSITION pos = m_listDeviceInfo.FindIndex(nIndex);
	if (pos == nullptr)
		return FALSE;

	LPCTSTR	lpDevicePath = m_listDeviceInfo.GetAt(pos).szDevicePath;

	return OpenDevice(lpDevicePath, m_listDeviceInfo.GetAt(pos).szDeviceName);

}

BOOL CAGDShowVideoCapture::OpenDevice(LPCTSTR lpDevicePath, LPCTSTR lpDeviceName)
{
    HRESULT		         hResult = S_OK;
    IBaseFilter*         filter  = nullptr;
    if (CDShowHelper::GetDeviceFilter(CLSID_VideoInputDeviceCategory, lpDeviceName, lpDevicePath,&filter)) {
        hResult = m_ptrGraphBuilder->AddFilter(filter, filterName);
        ATLASSERT(SUCCEEDED(hResult));
        if (hResult != S_OK)
            return FALSE;

        int nDeviceLen = _tcslen(m_szActiveDeviceID);
        if (nDeviceLen > 0) {
            _tcscpy_s(m_szActiveDeviceID, MAX_PATH, lpDevicePath);
        }
        else {
            _tcscpy_s(m_szActiveDeviceID, MAX_PATH, lpDeviceName);
        }
        SelectMediaCap(0);
		return TRUE;
    }

    return FALSE;
}

BOOL CAGDShowVideoCapture::GetCurrentDevice(LPTSTR lpDevicePath, SIZE_T *nDevicePathLen)
{
	int nDeviceLen = _tcslen(m_szActiveDeviceID);
	if (nDeviceLen >= static_cast<int>(*nDevicePathLen)) {
		*nDevicePathLen = nDeviceLen+1;
		return FALSE;
	}

	if (nDeviceLen == 0)
		return FALSE;

	_tcscpy_s(lpDevicePath, *nDevicePathLen, m_szActiveDeviceID);
	*nDevicePathLen = nDeviceLen + 1;

	return TRUE;
}

void CAGDShowVideoCapture::CloseDevice()
{
    HRESULT               hResult = S_OK;
    CComPtr<IBaseFilter>  ptrCaptureFilter = nullptr;
    hResult = m_ptrGraphBuilder->FindFilterByName(filterName, &ptrCaptureFilter);
    ATLASSERT(SUCCEEDED(hResult));
    if (FAILED(hResult))
        return;
    m_ptrGraphBuilder->RemoveFilter(ptrCaptureFilter);
    
    ZeroMemory(m_szActiveDeviceID, MAX_PATH * sizeof(TCHAR));
}

int CAGDShowVideoCapture::GetMediaCapCount()
{
	int		nCount  = 0;
	int		nSize   = 0;
	HRESULT hResult = S_OK;

    CComPtr<IBaseFilter>			ptrCaptureFilter = nullptr;
    CComPtr<IAMStreamConfig>		ptrStreamConfig  = nullptr;

	hResult = m_ptrGraphBuilder->FindFilterByName(filterName, &ptrCaptureFilter);
	ATLASSERT(SUCCEEDED(hResult));
	if (FAILED(hResult))
		return 0;

	hResult = m_ptrCaptureGraphBuilder2->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, ptrCaptureFilter, IID_IAMStreamConfig, (void**)&ptrStreamConfig);
	ATLASSERT(SUCCEEDED(hResult));
	if (FAILED(hResult))
		return 0;

	hResult = ptrStreamConfig->GetNumberOfCapabilities(&nCount, &nSize);
	ATLASSERT(SUCCEEDED(hResult));
	if (FAILED(hResult))
		return 0;
  
	return nCount;
}

BOOL CAGDShowVideoCapture::GetMediaCap(int nIndex, AM_MEDIA_TYPE **ppMediaType, LPVOID lpMediaStreamConfigCaps, SIZE_T nSize)
{
	int		nCount = 0;
	int		nCapSize = 0;
	HRESULT hResult = S_OK;

    CComPtr<IBaseFilter>			ptrCaptureFilter = nullptr;
    CComPtr<IAMStreamConfig>		ptrStreamConfig  = nullptr;

	hResult = m_ptrGraphBuilder->FindFilterByName(filterName, &ptrCaptureFilter);
	ATLASSERT(SUCCEEDED(hResult));
	if (FAILED(hResult))
		return FALSE;

	hResult = m_ptrCaptureGraphBuilder2->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, ptrCaptureFilter, IID_IAMStreamConfig, (void**)&ptrStreamConfig);
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


BOOL CAGDShowVideoCapture::SelectMediaCap(int nIndex)
{
    int		nCount = 0;
    int		nSize = 0;
    HRESULT hResult = S_OK;

    AM_MEDIA_TYPE	*lpMediaType = NULL;

    CComPtr<IBaseFilter>			ptrCaptureFilter = nullptr;
    CComPtr<IAMStreamConfig>		ptrStreamConfig  = nullptr;

    hResult = m_ptrGraphBuilder->FindFilterByName(filterName, &ptrCaptureFilter);
    ATLASSERT(SUCCEEDED(hResult));
    if (FAILED(hResult))
        return FALSE;

    hResult = m_ptrCaptureGraphBuilder2->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, ptrCaptureFilter, IID_IAMStreamConfig, (void**)&ptrStreamConfig);
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

    ATLASSERT(nSize <= sizeof(VIDEO_STREAM_CONFIG_CAPS));

    do {
        hResult = ptrStreamConfig->GetStreamCaps(nIndex, &lpMediaType, reinterpret_cast<BYTE*>(&m_vscStreamCfgCaps));
        ATLASSERT(SUCCEEDED(hResult));
        if (FAILED(hResult))
            break;

        hResult = ptrStreamConfig->SetFormat(lpMediaType);
        ATLASSERT(SUCCEEDED(hResult));
        if (FAILED(hResult))
            break;

    } while (FALSE);

    CDShowHelper::FreeMediaType(lpMediaType);
    
    return SUCCEEDED(hResult);
}

BOOL CAGDShowVideoCapture::GetVideoCap(int nIndex, VIDEOINFOHEADER *lpVideoInfo)
{
	int		nCount = 0;
	int		nSize = 0;

	AM_MEDIA_TYPE *lpAMMediaType = NULL;
	VIDEO_STREAM_CONFIG_CAPS videoStreamCfgCaps;

	BOOL bSuccess = GetMediaCap(nIndex, &lpAMMediaType, &videoStreamCfgCaps, sizeof(VIDEO_STREAM_CONFIG_CAPS));

	if (lpAMMediaType->formattype == FORMAT_VideoInfo) {
		VIDEOINFOHEADER* pVideoInfo = reinterpret_cast<VIDEOINFOHEADER*>(lpAMMediaType->pbFormat);
		memcpy_s(lpVideoInfo, sizeof(VIDEOINFOHEADER), pVideoInfo, sizeof(VIDEOINFOHEADER));

		bSuccess = TRUE;
	}
	else if (lpAMMediaType->formattype == FORMAT_VideoInfo2) {
		VIDEOINFOHEADER2* pVideoInfo2 = reinterpret_cast< VIDEOINFOHEADER2*>(lpAMMediaType->pbFormat);
		memcpy_s(&lpVideoInfo->bmiHeader, sizeof(BITMAPINFOHEADER), &pVideoInfo2->bmiHeader, sizeof(BITMAPINFOHEADER));
		lpVideoInfo->AvgTimePerFrame = pVideoInfo2->AvgTimePerFrame;
		lpVideoInfo->dwBitErrorRate = pVideoInfo2->dwBitErrorRate;
		lpVideoInfo->dwBitRate = pVideoInfo2->dwBitRate;
		bSuccess = TRUE;
	}
	else
		bSuccess = FALSE;

    CDShowHelper::FreeMediaType(lpAMMediaType);

	return bSuccess;
}

BOOL CAGDShowVideoCapture::GetCurrentVideoCap(VIDEOINFOHEADER *lpVideoInfo)
{
	BOOL			bSuccess = FALSE;
	HRESULT			hResult = S_OK;
	AM_MEDIA_TYPE	*lpAMMediaType = NULL;

    CComPtr<IBaseFilter>			ptrCaptureFilter = nullptr;
    CComPtr<IAMStreamConfig>		ptrStreamConfig  = nullptr;

	hResult = m_ptrGraphBuilder->FindFilterByName(filterName, &ptrCaptureFilter);
	ATLASSERT(SUCCEEDED(hResult));
	if (FAILED(hResult))
		return FALSE;

	hResult = m_ptrCaptureGraphBuilder2->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, ptrCaptureFilter, IID_IAMStreamConfig, (void**)&ptrStreamConfig);
	ATLASSERT(SUCCEEDED(hResult));
	if (FAILED(hResult))
		return FALSE;

	hResult = ptrStreamConfig->GetFormat(&lpAMMediaType);
	ATLASSERT(SUCCEEDED(hResult));
	if (FAILED(hResult))
		return FALSE;

	if (lpAMMediaType->formattype == FORMAT_VideoInfo) {
		VIDEOINFOHEADER* pVideoInfo = reinterpret_cast<VIDEOINFOHEADER*>(lpAMMediaType->pbFormat);
		memcpy_s(lpVideoInfo, sizeof(VIDEOINFOHEADER), pVideoInfo, sizeof(VIDEOINFOHEADER));

		bSuccess = TRUE;
	}
	else if (lpAMMediaType->formattype == FORMAT_VideoInfo2) {
		VIDEOINFOHEADER2* pVideoInfo2 = reinterpret_cast< VIDEOINFOHEADER2*>(lpAMMediaType->pbFormat);
		memcpy_s(&lpVideoInfo->bmiHeader, sizeof(BITMAPINFOHEADER), &pVideoInfo2->bmiHeader, sizeof(BITMAPINFOHEADER));
		lpVideoInfo->AvgTimePerFrame = pVideoInfo2->AvgTimePerFrame;
		lpVideoInfo->dwBitErrorRate = pVideoInfo2->dwBitErrorRate;
		lpVideoInfo->dwBitRate = pVideoInfo2->dwBitRate;
		bSuccess = TRUE;
	}
	else
		bSuccess = FALSE;

    CDShowHelper::FreeMediaType(lpAMMediaType);

	return bSuccess;
}

BOOL CAGDShowVideoCapture::RemoveCaptureFilter()
{
    if (videoCapture) {
        m_ptrGraphBuilder->RemoveFilter(videoCapture);
        videoCapture.Release();
        return TRUE;
    }
    return FALSE;
}


BOOL CAGDShowVideoCapture::CreateCaptureFilter()
{
    if (videoCapture) {
        m_ptrGraphBuilder->RemoveFilter(videoCapture);
        videoCapture.Release();
    }

    AM_MEDIA_TYPE*  mt = nullptr;
    if (GetCurrentMediaType(&mt)) {
        PinCaptureInfo info;
        info.callback = [this](IMediaSample *s) {Receive(true, s); };
        info.expectedMajorType = mt->majortype;
        info.expectedSubType   = mt->subtype;
        videoCapture = new CaptureFilter(info);

        bmiHeader = CDShowHelper::GetBitmapInfoHeader(*mt);
       // CVideoPackageQueue::GetInstance()->SetVideoFormat(bmiHeader);
        HRESULT hr = m_ptrGraphBuilder->AddFilter(videoCapture, L"Video Capture Filter");
        if (SUCCEEDED(hr))
            return TRUE;
        CDShowHelper::FreeMediaType(mt);
    }
    return FALSE;
}

BOOL CAGDShowVideoCapture::Start()
{
    if (ConnectFilters()) {
        control->Run();
        active = true;
        return TRUE;
    }
    return FALSE;
}

void CAGDShowVideoCapture::Stop()
{
    if (active) {
        control->Stop();
        active = false;
    }
       
}

void CAGDShowVideoCapture::GetDeviceName(LPTSTR deviceName, SIZE_T *nDeviceLen)
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

BOOL CAGDShowVideoCapture::ConnectFilters()
{
    CComPtr<IBaseFilter>	filter = nullptr;
    HRESULT hResult = m_ptrGraphBuilder->FindFilterByName(filterName, &filter);
    TCHAR deviceName[MAX_PATH] = { 0 };
    SIZE_T len = 0;
    GetDeviceName(deviceName, &len);
    if (SUCCEEDED(hResult) && filter && videoCapture) {
        bool success = ConnectPins(PIN_CATEGORY_CAPTURE,
            MEDIATYPE_Video, filter,
            videoCapture);
        return TRUE;
    }
       
    return FALSE;
}

BOOL CAGDShowVideoCapture::ConnectPins(const GUID &category, const GUID &type,
    IBaseFilter *filter, IBaseFilter *capture)
{
    HRESULT hr = S_OK;
    CComPtr<IPin>   filterPin  = nullptr;
    CComPtr<IPin>   capturePin = nullptr;

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

BOOL CAGDShowVideoCapture::GetCurrentMediaType(AM_MEDIA_TYPE **lpAMMediaType)
{
    BOOL			bSuccess = FALSE;
    HRESULT			hResult = S_OK;

    CComPtr<IBaseFilter>			ptrCaptureFilter = nullptr;
    CComPtr<IAMStreamConfig>		ptrStreamConfig = nullptr;

    hResult = m_ptrGraphBuilder->FindFilterByName(filterName, &ptrCaptureFilter);
    ATLASSERT(SUCCEEDED(hResult));
    if (FAILED(hResult))
        return FALSE;

    hResult = m_ptrCaptureGraphBuilder2->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, ptrCaptureFilter, IID_IAMStreamConfig, (void**)&ptrStreamConfig);
    ATLASSERT(SUCCEEDED(hResult));
    if (FAILED(hResult))
        return FALSE;

    hResult = ptrStreamConfig->GetFormat(lpAMMediaType);
    ATLASSERT(SUCCEEDED(hResult));
    if (FAILED(hResult))
        return FALSE;

    return TRUE;
}

void CAGDShowVideoCapture::Receive(bool video, IMediaSample *sample)
{
    BYTE *pBuffer;
    if (!sample)
        return;
   
    int size = sample->GetActualDataLength();
    if (!size)
        return;

    if (FAILED(sample->GetPointer(&pBuffer)))
        return;
    long long startTime, stopTime;
    bool hasTime = SUCCEEDED(sample->GetTime(&startTime, &stopTime));
   

#ifdef DEBUG
    HANDLE hFile = INVALID_HANDLE_VALUE;
    DWORD  dwBytesWritten = 0;

    switch (bmiHeader->biCompression)
    {
    case 0x00000000:	// RGB24
        hFile = ::CreateFile(_T("d:\\pictest\\test.rgb24"), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        break;
    case MAKEFOURCC('I', '4', '2', '0'):	// I420
        hFile = ::CreateFile(_T("d:\\pictest\\test.i420"), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        break;
    case MAKEFOURCC('Y', 'U', 'Y', '2'):	// YUY2
        hFile = ::CreateFile(_T("d:\\pictest\\test.yuy2"), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        break;
    case MAKEFOURCC('M', 'J', 'P', 'G'):	// MJPEG
        hFile = ::CreateFile(_T("d:\\pictest\\test.jpeg"), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        break;
    case MAKEFOURCC('U', 'Y', 'V', 'Y'):	// UYVY
        hFile = ::CreateFile(_T("d:\\pictest\\test.uyvy"), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        break;
    default:
        break;
    }

    if (hFile != INVALID_HANDLE_VALUE) {
        ::WriteFile(hFile, pBuffer, size, &dwBytesWritten, NULL);
        ::CloseHandle(hFile);
    }
#endif
    m_lpY = m_lpYUVBuffer;
    m_lpU = m_lpY + bmiHeader->biWidth*bmiHeader->biHeight;
    m_lpV = m_lpU + bmiHeader->biWidth*bmiHeader->biHeight / 4;
    switch (bmiHeader->biCompression)
    {
    case 0x00000000:	// RGB24
		RGB24ToI420(pBuffer, bmiHeader->biWidth * 3, 
            m_lpY, bmiHeader->biWidth,
            m_lpU, bmiHeader->biWidth / 2,
            m_lpV, bmiHeader->biWidth / 2,
            bmiHeader->biWidth, bmiHeader->biHeight);
        break;
    case MAKEFOURCC('I', '4', '2', '0'):	// I420
        memcpy_s(m_lpYUVBuffer, 0x800000, pBuffer, size);
        break;
    case MAKEFOURCC('Y', 'U', 'Y', '2'):	// YUY2
        YUY2ToI420(pBuffer, bmiHeader->biWidth * 2,
            m_lpY, bmiHeader->biWidth,
            m_lpU, bmiHeader->biWidth / 2,
            m_lpV, bmiHeader->biWidth / 2,
            bmiHeader->biWidth, bmiHeader->biHeight);
        break;
    case MAKEFOURCC('M', 'J', 'P', 'G'):	// MJPEG
         MJPGToI420(pBuffer, size,
            m_lpY, bmiHeader->biWidth,
            m_lpU, bmiHeader->biWidth / 2,
            m_lpV, bmiHeader->biWidth / 2,
            bmiHeader->biWidth, bmiHeader->biHeight,
            bmiHeader->biWidth, bmiHeader->biHeight);
        break;
    case MAKEFOURCC('U', 'Y', 'V', 'Y'):	// UYVY
       UYVYToI420(pBuffer, bmiHeader->biWidth,
            m_lpY, bmiHeader->biWidth,
            m_lpU, bmiHeader->biWidth / 2,
            m_lpV, bmiHeader->biWidth / 2,
            bmiHeader->biWidth, bmiHeader->biHeight);
        break;
    default:
        ATLASSERT(FALSE);
        break;
    }
    SIZE_T nYUVSize = bmiHeader->biWidth*bmiHeader->biHeight * 3 / 2;
    if (!CAgVideoBuffer::GetInstance()->writeBuffer(m_lpYUVBuffer, nYUVSize, GetTickCount())) {
        OutputDebugString(L"CAgVideoBuffer::GetInstance()->writeBuffer failed.");
        return;
    }
#ifdef DEBUG
    //hFile = ::CreateFile(_T("d:\\pictest\\trans.i420"), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hFile != INVALID_HANDLE_VALUE) {
        ::WriteFile(hFile, m_lpYUVBuffer, nYUVSize, &dwBytesWritten, NULL);
        ::CloseHandle(hFile);
    }

#endif
}
