//
//  SampleHandlerUtil.m
//  APIExample
//
//  Created by Arlin on 2022/5/7.
//  Copyright © 2022 Agora Corp. All rights reserved.
//

#import "SampleHandlerUtil.h"

@implementation SampleHandlerUtil
+ (void)finishBroadcastWithNilError:(nullable RPBroadcastSampleHandler *)sampleHandler {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnonnull"
    [sampleHandler finishBroadcastWithError:nil];
#pragma clang diagnostic pop
}
@end
