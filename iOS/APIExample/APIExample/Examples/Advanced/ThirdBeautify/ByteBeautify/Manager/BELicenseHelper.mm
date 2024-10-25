//  BELicenseHelper.m
//  BECore


#import "BELicenseHelper.h"
#if __has_include(<effect-sdk/bef_effect_ai_api.h>)
#import <effect-sdk/bef_effect_ai_license_wrapper.h>
#import <effect-sdk/bef_effect_ai_api.h>
#endif
#import "BEHttpRequestProvider.h"
#import <vector>
#import <iostream>
#import "BundleUtil.h"
#import "Config.h"

using namespace std;

static NSString *OFFLIN_LICENSE_PATH = @"LicenseBag";
static NSString *OFFLIN_BUNDLE = @"bundle";
static NSString *LICENSE_URL = @"https://cv.iccvlog.com/cv_tob/v1/api/sdk/tob_license/getlicense";
static NSString *KEY = @"cv_test_online1";
static NSString *SECRET = @"e479f002-4018-11eb-a1e0-b8599f494dc4";
static LICENSE_MODE_ENUM LICENSE_MODE = OFFLINE_LICENSE;
BOOL overSeasVersion = NO;

@interface BELicenseHelper() {
    std::string         _licenseFilePath;
    LICENSE_MODE_ENUM   _licenseMode;
#if __has_include(<effect-sdk/bef_effect_ai_api.h>)
    EffectsSDK::LicenseProvider* _licenseProvider;
    EffectsSDK::HttpRequestProvider* _requestProvider;
#endif
}
@end

@implementation BELicenseHelper

static BELicenseHelper* _instance = nil;


+(instancetype) shareInstance
{
    static dispatch_once_t onceToken ;
    dispatch_once(&onceToken, ^{
        _instance = [[super allocWithZone:NULL] init] ;
    }) ;
    
    return _instance ;
}

+(id) allocWithZone:(struct _NSZone *)zone
{
    return [BELicenseHelper shareInstance] ;
}
 
-(id) copyWithZone:(struct _NSZone *)zone
{
    return [BELicenseHelper shareInstance] ;
}

- (void)setParam:(NSString*)key value:(NSString*) value{
#if __has_include(<effect-sdk/bef_effect_ai_api.h>)
    if (_licenseProvider == nil)
        return;
    
    _licenseProvider->setParam([key UTF8String], [value UTF8String]);
#endif
}

- (id)init {
    self = [super init];
#if __has_include(<effect-sdk/bef_effect_ai_api.h>)
    if (self) {
        _errorCode = 0;
        _licenseMode = LICENSE_MODE;
        _licenseProvider = bef_effect_ai_get_license_wrapper_instance();
        if (_licenseMode == ONLINE_LICENSE)
        {
            _licenseProvider->setParam("mode", "ONLINE");
            _licenseProvider->setParam("url", [[self licenseUrl] UTF8String]);
            _licenseProvider->setParam("key", [[self licenseKey] UTF8String]);
            _licenseProvider->setParam("secret", [[self licenseSecret] UTF8String]);
            NSString *licenseName = [NSString stringWithFormat:@"/%s", "license.bag"];
            NSString *licensePath = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) lastObject];
            licensePath = [licensePath stringByAppendingString:licenseName];
            _licenseProvider->setParam("licensePath", [licensePath UTF8String]);
        }
        else
        {
            _licenseProvider->setParam("mode", "OFFLINE");
            NSString *licenseName = [NSString stringWithFormat:@"/%s", LICENSE_NAME];
            NSBundle *bundle = [BundleUtil bundleWithBundleName:@"ByteEffectLib" podName:@"bytedEffect"];
            NSString* licensePath = [bundle pathForResource:OFFLIN_LICENSE_PATH ofType:OFFLIN_BUNDLE];
            licensePath = [licensePath stringByAppendingString:licenseName];
            _licenseProvider->setParam("licensePath", [licensePath UTF8String]);
        }
        _requestProvider = new BEHttpRequestProvider;
        _licenseProvider->registerHttpProvider(_requestProvider);
    }
#endif

    return self;
}

- (NSString *)licenseUrl {
    NSUserDefaults *def = [NSUserDefaults standardUserDefaults];
    if ([[def objectForKey:@"licenseUrl"] isEqual: @""] || [def objectForKey:@"licenseUrl"] == nil) {
        [def synchronize];
        if (overSeasVersion)
            LICENSE_URL = @"https://cv-tob.byteintl.com/v1/api/sdk/tob_license/getlicense";
        return LICENSE_URL;
    }
    else {
        NSString *licenseUrl = [def objectForKey:@"licenseUrl"];
        [def synchronize];
        return licenseUrl;
    }
}

- (NSString *)licenseKey {
    NSUserDefaults *def = [NSUserDefaults standardUserDefaults];
    if ([[def objectForKey:@"licenseKey"] isEqual: @""] || [def objectForKey:@"licenseKey"] == nil) {
        [def synchronize];
        if (overSeasVersion)
            KEY = @"biz_license_tool_test_key6f4411ef1eb14a858e51bfcdfbe68a60";
        return KEY;
    }
    else {
        NSString *licenseKey = [def objectForKey:@"licenseKey"];
        [def synchronize];
        return licenseKey;
    }
}

- (NSString *)licenseSecret {
    NSUserDefaults *def = [NSUserDefaults standardUserDefaults];
    if ([[def objectForKey:@"licenseSecret"] isEqual: @""] || [def objectForKey:@"licenseSecret"] == nil) {
        [def synchronize];
        
        if (overSeasVersion)
            SECRET = @"969f0a51ae465c4b21f30c59bcb08ea4";
        return SECRET;
    }
    else {
        NSString *licenseSecret = [def objectForKey:@"licenseSecret"];
        [def synchronize];
        return licenseSecret;
    }
}

-(void)dealloc {
#if __has_include(<effect-sdk/bef_effect_ai_api.h>)
    delete _licenseProvider;
    delete _requestProvider;
#endif
}

#if __has_include(<effect-sdk/bef_effect_ai_api.h>)
- (const char *)licensePath {
    _errorCode = 0;
    _errorMsg = @"";
    std::map<std::string, std::string> params;
    _licenseProvider->getLicenseWithParams(params, false, [](const char* retmsg, int retSize, EffectsSDK::ErrorInfo error, void* userdata){
        BELicenseHelper* pThis = CFBridgingRelease(userdata);
        pThis.errorCode = error.errorCode;
        pThis.errorMsg = [[NSString alloc] initWithCString:error.errorMsg.c_str() encoding:NSUTF8StringEncoding];
    }, (void*)CFBridgingRetain(self));

    if (![self checkLicenseResult: @"getLicensePath"])
        return "";

    _licenseFilePath = _licenseProvider->getParam("licensePath");
    return _licenseFilePath.c_str();
}
#endif

#if __has_include(<effect-sdk/bef_effect_ai_api.h>)
- (const char *)updateLicensePath {
    _errorCode = 0;
    _errorMsg = @"";
    std::map<std::string, std::string> params;
    _licenseProvider->updateLicenseWithParams(params, false, [](const char* retmsg, int retSize, EffectsSDK::ErrorInfo error, void* userdata){
        BELicenseHelper* pThis = CFBridgingRelease(userdata);
        pThis.errorCode = error.errorCode;
        pThis.errorMsg = [[NSString alloc] initWithCString:error.errorMsg.c_str() encoding:NSUTF8StringEncoding];
    }, (void*)CFBridgingRetain(self));

    if (![self checkLicenseResult: @"updateLicensePath"])
        return "";
    
    _licenseFilePath = _licenseProvider->getParam("licensePath");
    return _licenseFilePath.c_str();
}
#endif

- (LICENSE_MODE_ENUM) licenseMode{
    return _licenseMode;
}

- (bool)checkLicense {
    NSString *licenseName = [NSString stringWithFormat:@"/%s", LICENSE_NAME];
    NSBundle *bundle = [BundleUtil bundleWithBundleName:@"ByteEffectLib" podName:@"bytedEffect"];
    NSString* licensePath = [bundle pathForResource:OFFLIN_LICENSE_PATH ofType:OFFLIN_BUNDLE];
    licensePath = [licensePath stringByAppendingString:licenseName];
    return [self checkLicenseOK:[licensePath UTF8String]];
}

- (bool)checkLicenseResult:(NSString*) msg {
    if (_errorCode != 0) {
        if ([_errorMsg length] > 0) {
            NSLog(@"%a error: %d, %a", msg, _errorCode, _errorMsg);
            [[NSNotificationCenter defaultCenter] postNotificationName:@"kBESdkErrorNotification" object:nil
                                                      userInfo:@{@"data": _errorMsg}];
        } else {
            NSLog(@"%a error: %d", msg, _errorCode);
            [[NSNotificationCenter defaultCenter] postNotificationName:@"kBESdkErrorNotification" object:nil
                                                        userInfo:@{@"data": [NSString stringWithFormat:@"%a error: %d", msg, _errorCode]}];
        }
        return false;
    }
    return true;
}

- (bool)checkLicenseOK:(const char *) filePath {
#if __has_include(<effect-sdk/bef_effect_ai_api.h>)
    bef_effect_handle_t effectHandle = 0;
    int ret = bef_effect_ai_create(&effectHandle);
    // this property will be held by a singleton, and only got once,
    // so it is necessary to set use_builtin_sensor at the first time
    bef_effect_ai_use_builtin_sensor(effectHandle, YES);

    ret = bef_effect_ai_check_online_license(effectHandle, filePath);
    bef_effect_ai_destroy(effectHandle);
    
    if (ret != 0 && ret != -11 && ret != 1)
    {
        return false;
    }
#endif
    return true;
}

- (bool)deleteCacheFile {
#if __has_include(<effect-sdk/bef_effect_ai_api.h>)
    std::string filePath = _licenseProvider->getParam("licensePath");
    if (!filePath.empty()) {
        NSString *path = [[NSString alloc] initWithUTF8String:filePath.c_str()];
        NSFileManager *fileManager = [NSFileManager defaultManager];
        BOOL isDelete = [fileManager removeItemAtPath:path error:nil];
        if (!isDelete) {
            return false;
        }
    }
#endif
    return true;
}

@end
