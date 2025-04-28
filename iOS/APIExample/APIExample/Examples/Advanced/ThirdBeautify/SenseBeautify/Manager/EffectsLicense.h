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

/// Authorize
/// @param licensePath License file path
+ (BOOL)authorizeWithLicensePath:(NSString *)licensePath;

/// Authorize
/// @param dataLicense License file data
+ (BOOL)authorizeWithLicenseData:(NSData *)dataLicense;
@end

NS_ASSUME_NONNULL_END
