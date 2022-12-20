//
//  License.h
//  Effects
//
//  Created by sunjian on 2021/5/8.
//  Copyright © 2021 sjuinan. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface EffectsLicense : NSObject

/// 鉴权
/// @param licensePath 授权文件路径
+ (BOOL)authorizeWithLicensePath:(NSString *)licensePath;

/// 鉴权
/// @param dataLicense 授权文件数据
+ (BOOL)authorizeWithLicenseData:(NSData *)dataLicense;
@end

NS_ASSUME_NONNULL_END


