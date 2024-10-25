#import "BEHttpRequestProvider.h"
#import <Foundation/Foundation.h>

#if __has_include(<effect-sdk/bef_effect_ai_api.h>)
// {zh} post请求暂时不需要实现 {en} The post request does not need to be implemented for the time being
bool BEHttpRequestProvider::getRequest(const EffectsSDK::RequestInfo* requestInfo, EffectsSDK::ResponseInfo& responseInfo)
{
    return false;
}

bool BEHttpRequestProvider::postRequest(const EffectsSDK::RequestInfo* requestInfo, EffectsSDK::ResponseInfo& responseInfo)
{
    NSString* nsUrl = [[NSString alloc] initWithCString:requestInfo->url.c_str() encoding:NSUTF8StringEncoding];
    NSURL *URL = [NSURL URLWithString:nsUrl];
    NSURLSession *session = [NSURLSession sharedSession];
    NSMutableURLRequest *request = [NSMutableURLRequest requestWithURL:URL];
    [request setHTTPMethod:@"POST"];
    for (auto iter = requestInfo->requestHead.begin(); iter != requestInfo->requestHead.end(); iter++) {
        NSString* headKey = [[NSString alloc] initWithCString:iter->first.c_str() encoding:NSUTF8StringEncoding];
        NSString* headValue = [[NSString alloc] initWithCString:iter->second.c_str() encoding:NSUTF8StringEncoding];
        [request setValue:headValue forHTTPHeaderField:headKey];
    }
    NSString* nsBody = [[NSString alloc] initWithCString:requestInfo->bodydata encoding:NSUTF8StringEncoding];
    request.HTTPBody = [nsBody dataUsingEncoding:NSUTF8StringEncoding];
    
    __block bool requestRet = false;
    dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);
    NSURLSessionDataTask *dataTask = [session dataTaskWithRequest:request completionHandler:^(NSData * _Nullable data, NSURLResponse * _Nullable response, NSError * _Nullable error) {
        if (data)
        {
            NSHTTPURLResponse *urlResponse = (NSHTTPURLResponse *)response;
            responseInfo.status_code = urlResponse.statusCode;
            responseInfo.bodySize = [data length];
            responseInfo.bodydata = new char[responseInfo.bodySize];
            memcpy(responseInfo.bodydata, [data bytes], responseInfo.bodySize);
            requestRet = true;
        }
        dispatch_semaphore_signal(semaphore);
    }];
    
    [dataTask resume];
    dispatch_semaphore_wait(semaphore, DISPATCH_TIME_FOREVER);
    
    return requestRet;
}
#endif
