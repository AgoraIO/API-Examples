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
//   {zh} / @brief 授权文件路径     {en} /@brief authorization file path
- (const char *)licensePath;
//   {zh} / @brief 授权文件路径, 更新license     {en} /@brief authorization file path, update
- (const char *)updateLicensePath;
//   {zh} / @brief 授权模式， 0:离线  1:在线     {en} /@brief authorization file path
- (LICENSE_MODE_ENUM) licenseMode;

- (int) errorCode;

- (bool)checkLicenseResult:(NSString*) msg;

- (bool)checkLicenseOK:(const char *) filePath;

- (bool)deleteCacheFile;

- (bool)checkLicense;
@end

@interface BELicenseHelper : NSObject<BELicenseProvider>

@property (atomic, readwrite) int errorCode;

@property (atomic, readwrite) NSString* errorMsg;

+(instancetype) shareInstance;

- (void)setParam:(NSString*)key value:(NSString*) value;

@end

#endif /* BELicenseHelper_h */
