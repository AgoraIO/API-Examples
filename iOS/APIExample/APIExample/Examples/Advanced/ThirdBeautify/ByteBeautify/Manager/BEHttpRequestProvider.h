#ifndef BEHttpRequestProvider_h
#define BEHttpRequestProvider_h

#if __has_include(<effect-sdk/bef_effect_ai_api.h>)
#include <effect-sdk/bef_effect_ai_license_wrapper.h>
#endif

#if __has_include(<effect-sdk/bef_effect_ai_api.h>)
class BEHttpRequestProvider: public EffectsSDK::HttpRequestProvider
{
    
public:
    bool getRequest(const EffectsSDK::RequestInfo* requestInfo, EffectsSDK::ResponseInfo& responseInfo) override;
    
    bool postRequest(const EffectsSDK::RequestInfo* requestInfo, EffectsSDK::ResponseInfo& responseInfo) override;
    
};
#endif
#endif //BEHttpRequestProvider_h
