#pragma once
#include <DShow.h>
class CDShowHelper
{
public:
    CDShowHelper();
    ~CDShowHelper();

    static bool PinMatches(IPin *pin, const GUID &type, const GUID &category,
        PIN_DIRECTION &dir);
    static bool PinHasMajorType(IPin *pin, const GUID &type);
    static bool PinConfigHasMajorType(IPin *pin, const GUID &type);
    static bool PinIsDirection(IPin *pin, PIN_DIRECTION dir);
    static bool PinIsCategory(IPin *pin, const GUID &category);
    static HRESULT GetPinCategory(IPin *pin, GUID &category);
    static bool GetPinByName(IBaseFilter *filter, PIN_DIRECTION dir, const wchar_t *name,
        IPin **pin);
    static bool PinNameIs(IPin *pin, const wchar_t *name);
    static BOOL GetFilterPin(IBaseFilter *filter, const GUID &type, const GUID &category,
        PIN_DIRECTION dir, IPin **pin);

    static void FreeMediaType(AM_MEDIA_TYPE *lpAMMediaType);
    static HRESULT CopyMediaType(AM_MEDIA_TYPE *pmtTarget, const AM_MEDIA_TYPE *pmtSource);
    static BITMAPINFOHEADER *GetBitmapInfoHeader(AM_MEDIA_TYPE &mt);
    static bool GetDeviceFilter(const IID &type, const wchar_t *name, const wchar_t *path,
        IBaseFilter **filter);
    static bool EnumDevice(const GUID &type, IMoniker *deviceInfo,
        IBaseFilter** filter, const wchar_t *name, const wchar_t *path);
    static bool EnumDevices(const GUID &type, IBaseFilter** filter,  const wchar_t *name, const wchar_t *path);

   
};

