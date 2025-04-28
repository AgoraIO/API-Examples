//  BEEffectResourceHelper.h
//  Effect

#ifndef BELicenseHelper_h
#define BELicenseHelper_h

#import <Foundation/Foundation.h>

typedef NS_ENUM(NSInteger, LICENSE_MODE_ENUM) {
    OFFLINE_LICENSE = 0,
    ONLINE_LICENSE
};

@protocol BELicenseProvider <NSObject>
// @Briefing authorization file path
- (const char *)licensePath;
// @Briefing authorization file path, update
- (const char *)updateLicensePath;
// @Briefing authorization mode, 0: offline  1: online
- (LICENSE_MODE_ENUM)licenseMode;

- (int)errorCode;

- (bool)checkLicenseResult:(NSString *)msg;

- (bool)checkLicenseOK:(const char *)filePath;

- (bool)deleteCacheFile;

- (bool)checkLicense;
@end

@interface BELicenseHelper : NSObject <BELicenseProvider>

@property(atomic, readwrite) int errorCode;

@property(atomic, readwrite) NSString *errorMsg;

+ (instancetype)shareInstance;

- (void)setParam:(NSString *)key value:(NSString *)value;

@end

#endif /* BELicenseHelper_h */
