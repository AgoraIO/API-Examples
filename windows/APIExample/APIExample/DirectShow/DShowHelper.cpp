#include "../stdafx.h"
#include "DShowHelper.h"
#include <string>
#include <vector>
#include "amvideo.h"
#include <Dvdmedia.h>
CDShowHelper::CDShowHelper()
{
}


CDShowHelper::~CDShowHelper()
{
}


bool CDShowHelper::PinConfigHasMajorType(IPin *pin, const GUID &type)
{
    HRESULT hr;
    IAMStreamConfig* config = nullptr;
    int count = 0, size = 0;

    hr = pin->QueryInterface(IID_IAMStreamConfig, (void**)&config);
    if (FAILED(hr))
        return false;

    hr = config->GetNumberOfCapabilities(&count, &size);
    if (FAILED(hr))
        return false;

    std::vector<BYTE> caps;
    caps.resize(size);

    for (int i = 0; i < count; i++) {
        AM_MEDIA_TYPE* mt;
        if (SUCCEEDED(config->GetStreamCaps(i, &mt, caps.data())))
            if (mt->majortype == type)
                return true;
    }

    return false;
}

bool CDShowHelper::PinHasMajorType(IPin *pin, const GUID &type)
{
    HRESULT hr;
    AM_MEDIA_TYPE* mt = nullptr;
    IEnumMediaTypes* mediaEnum = nullptr;

    /* first, check the config caps. */
    if (PinConfigHasMajorType(pin, type))
        return true;

    /* then let's check the media type for the pin */
    if (FAILED(pin->EnumMediaTypes(&mediaEnum)))
        return false;

    ULONG curVal = 0;
    hr = mediaEnum->Next(1, &mt, &curVal);
    if (hr != S_OK)
        return false;

    return mt->majortype == type;
}

bool CDShowHelper::PinIsDirection(IPin *pin, PIN_DIRECTION dir)
{
    if (!pin)
        return false;

    PIN_DIRECTION pinDir;
    return SUCCEEDED(pin->QueryDirection(&pinDir)) && pinDir == dir;
}

HRESULT CDShowHelper::GetPinCategory(IPin *pin, GUID &category)
{
    if (!pin)
        return E_POINTER;
    IKsPropertySet*  propertySet = nullptr;
    pin->QueryInterface(_uuidof(IKsPropertySet), (void**)&propertySet);
    DWORD size = 0;
    if (propertySet == NULL)
        return E_NOINTERFACE;

    return propertySet->Get(AMPROPSETID_Pin, AMPROPERTY_PIN_CATEGORY,
        NULL, 0, &category, sizeof(GUID), &size);
}

bool CDShowHelper::PinIsCategory(IPin *pin, const GUID &category)
{
    if (!pin) return false;

    GUID pinCategory;
    HRESULT hr = GetPinCategory(pin, pinCategory);

    /* if the pin has no category interface, chances are we created it */
    if (FAILED(hr))
        return (hr == E_NOINTERFACE);

    return category == pinCategory;
}

bool CDShowHelper::PinMatches(IPin *pin, const GUID &type, const GUID &category,
    PIN_DIRECTION &dir)
{
    if (!PinHasMajorType(pin, type))
        return false;
    if (!PinIsDirection(pin, dir))
        return false;
    if (!PinIsCategory(pin, category))
        return false;

    return true;
}

BOOL CDShowHelper::GetFilterPin(IBaseFilter *filter, const GUID &type, const GUID &category,
    PIN_DIRECTION dir, IPin **pin)
{
    IPin*       curPin   = nullptr;
    IEnumPins*  pinsEnum = nullptr;
    ULONG       num      = 0;

    if (!filter)
        return false;
    if (FAILED(filter->EnumPins(&pinsEnum)))
        return false;

    while (pinsEnum->Next(1, &curPin, &num) == S_OK) {

        if (PinMatches(curPin, type, category, dir)) {
            *pin = curPin;
            (*pin)->AddRef();
            return true;
        }
    }
    return false;
}

bool CDShowHelper::PinNameIs(IPin *pin, const wchar_t *name)
{
    if (!pin) return false;
    if (!name) return true;

    PIN_INFO pinInfo;

    if (FAILED(pin->QueryPinInfo(&pinInfo)))
        return false;

    if (pinInfo.pFilter)
        pinInfo.pFilter->Release();

    return wcscmp(name, pinInfo.achName) == 0;
}

bool CDShowHelper::GetPinByName(IBaseFilter *filter, PIN_DIRECTION dir, const wchar_t *name,
    IPin **pin)
{
    CComPtr<IPin>       curPin   = nullptr;
    CComPtr<IEnumPins>  pinsEnum = nullptr;
    ULONG       num      = 0;

    if (!filter)
        return false;
    if (FAILED(filter->EnumPins(&pinsEnum)))
        return false;

    while (pinsEnum->Next(1, &curPin, &num) == S_OK) {
        
        if (PinIsDirection(curPin, dir) && PinNameIs(curPin, name)) {
            *pin = curPin.Detach();
            return true;
        }
    }

    return false;
}



void CDShowHelper::FreeMediaType(AM_MEDIA_TYPE *lpAMMediaType)
{
    if (lpAMMediaType == NULL)
        return;

    if (lpAMMediaType->cbFormat != 0) {
        ::CoTaskMemFree((PVOID)lpAMMediaType->pbFormat);
        lpAMMediaType->cbFormat = 0;
        lpAMMediaType->pbFormat = NULL;
    }

    if (lpAMMediaType->pUnk != NULL) {
        lpAMMediaType->pUnk->Release();
        lpAMMediaType->pUnk = NULL;
    }

    ::CoTaskMemFree(lpAMMediaType);
}


HRESULT CDShowHelper::CopyMediaType(AM_MEDIA_TYPE *pmtTarget, const AM_MEDIA_TYPE *pmtSource)
{
    if (!pmtSource || !pmtTarget)
        return S_FALSE;

    *pmtTarget = *pmtSource;

    if (pmtSource->cbFormat && pmtSource->pbFormat) {
        pmtTarget->pbFormat =
            (PBYTE)CoTaskMemAlloc(pmtSource->cbFormat);

        if (pmtTarget->pbFormat == nullptr) {
            pmtTarget->cbFormat = 0;
            return E_OUTOFMEMORY;
        }
        else {
            memcpy(pmtTarget->pbFormat, pmtSource->pbFormat,
                pmtTarget->cbFormat);
        }
    }

    if (pmtTarget->pUnk != nullptr)
        pmtTarget->pUnk->AddRef();

    return S_OK;
}


BITMAPINFOHEADER *CDShowHelper::GetBitmapInfoHeader(AM_MEDIA_TYPE &mt)
{
    if (mt.formattype == FORMAT_VideoInfo) {
        VIDEOINFOHEADER *vih;
        vih = reinterpret_cast<VIDEOINFOHEADER*>(mt.pbFormat);
        return &vih->bmiHeader;

    }
    else if (mt.formattype == FORMAT_VideoInfo2) {
        VIDEOINFOHEADER2 *vih;
        vih = reinterpret_cast<VIDEOINFOHEADER2*>(mt.pbFormat);
        return &vih->bmiHeader;
    }

    return NULL;
}

bool CDShowHelper::EnumDevice(const GUID &type, IMoniker *deviceInfo, 
    IBaseFilter** outfilter, const wchar_t *name, const wchar_t *path)
{
    IPropertyBag*  propertyData = nullptr;
    IBaseFilter*   filter       = nullptr;
    HRESULT hr;

    hr = deviceInfo->BindToStorage(0, 0, IID_IPropertyBag,
        (void**)&propertyData);
    if (FAILED(hr))
        return true;

    VARIANT deviceName, devicePath;
    deviceName.vt = VT_BSTR;
    devicePath.vt = VT_BSTR;
    devicePath.bstrVal = NULL;

    hr = propertyData->Read(L"FriendlyName", &deviceName, NULL);
    if (FAILED(hr))
        return true;


    propertyData->Read(L"DevicePath", &devicePath, NULL);

    hr = deviceInfo->BindToObject(NULL, 0, IID_IBaseFilter,
        (void**)&filter);
    if (SUCCEEDED(hr)) {
		if (deviceName.bstrVal && name && wcscmp(name, deviceName.bstrVal) != 0)
            return true;

// 		if (!devicePath.bstrVal || wcscmp(path, devicePath.bstrVal) != 0)
// 			return true;

        *outfilter = filter;
        return false;
    }

    return true;
}

bool CDShowHelper::EnumDevices(const GUID &type, IBaseFilter** outFilter, const wchar_t *name, const wchar_t *path)
{
    //std::lock_guard<recursive_mutex* lock(enumMutex);
    ICreateDevEnum*  deviceEnum = nullptr;
    IEnumMoniker*    enumMoniker = nullptr;
    IMoniker*        deviceInfo = nullptr;
    IBaseFilter*     filter = nullptr;
    HRESULT                 hr;
    DWORD                   count = 0;
  
    hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL,
        CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (void**)&deviceEnum);
    if (FAILED(hr)) {
        OutputDebugString(L"EnumDevices: Could not create "
            L"ICreateDeviceEnum");
        return false;
    }

    hr = deviceEnum->CreateClassEnumerator(type, &enumMoniker, 0);
    if (FAILED(hr)) {
        OutputDebugString(L"EnumDevices: CreateClassEnumerator failed");
        return false;
    }

    if (hr == S_OK) {
        while (enumMoniker->Next(1, &deviceInfo, &count) == S_OK) {
            if (!EnumDevice(type, deviceInfo, outFilter, name, path))
                //*outFilter = filter;
                return true;
        }
    }
    return true;
}

bool CDShowHelper::GetDeviceFilter(const IID &type, const wchar_t *name, const wchar_t *path,
    IBaseFilter **out)
{
    IBaseFilter*     filter      = nullptr;
    CComPtr<ICreateDevEnum>  deviceEnum  = nullptr;
    CComPtr<IEnumMoniker>    enumMoniker = nullptr;
    CComPtr<IMoniker>        deviceInfo  = nullptr;
    HRESULT                  hr;
    DWORD                    count = 0;

    hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL,
        CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (void**)&deviceEnum);
    if (FAILED(hr)) {
        OutputDebugString(L"Could not create ICreateDeviceEnum");
        return false;
    }

    hr = deviceEnum->CreateClassEnumerator(type, &enumMoniker, 0);
    if (FAILED(hr)) {
        OutputDebugString(L"CreateClassEnumerator failed");
        return false;
    }

    if (hr == S_OK) {
        while (enumMoniker->Next(1, &deviceInfo, &count) == S_OK) {
            if (!EnumDevice(type, deviceInfo, &filter, name, path)) {
                if (filter != nullptr) {
                    *out = filter;
                    return true;
                }
            }
            deviceInfo.Release();
        }
    }
    return false;
}