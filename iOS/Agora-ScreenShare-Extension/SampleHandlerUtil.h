//
//  SampleHandlerUtil.h
//  APIExample
//
//  Created by Arlin on 2022/5/7.
//  Copyright © 2022 Agora Corp. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <ReplayKit/ReplayKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface SampleHandlerUtil : NSObject
+ (void)finishBroadcastWithNilError:(nullable RPBroadcastSampleHandler *)sampleHandler;
@end

NS_ASSUME_NONNULL_END
