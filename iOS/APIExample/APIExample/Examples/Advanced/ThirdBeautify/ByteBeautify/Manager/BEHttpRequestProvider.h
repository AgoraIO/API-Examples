#ifndef BEHttpRequestProvider_h
#define BEHttpRequestProvider_h

#if __has_include("bef_effect_ai_api.h")
#include "BytedLicenseDefine.h"
#endif

#if __has_include("bef_effect_ai_api.h")
class BEHttpRequestProvider: public HttpRequestProvider
{
    
public:
    bool getRequest(const RequestInfo* requestInfo, ResponseInfo& responseInfo) override;
    
    bool postRequest(const RequestInfo* requestInfo, ResponseInfo& responseInfo) override;
    
};
#endif
#endif //BEHttpRequestProvider_h
