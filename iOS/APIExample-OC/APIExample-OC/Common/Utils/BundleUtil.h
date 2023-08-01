//
//  BundleUtil.h
//  APIExample
//
//  Created by zhaoyongqiang on 2022/10/25.
//  Copyright © 2022 Agora Corp. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface BundleUtil : NSObject

+ (NSBundle*)bundleWithBundleName:(NSString *)bundleName podName:(NSString *)podName;

@end

NS_ASSUME_NONNULL_END
