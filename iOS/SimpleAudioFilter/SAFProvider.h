//
//  SAFProvider.h
//  SimpleAudioFilter
//
//  Created by xianing on 2021/7/27.
//  Copyright © 2021 Agora Corp. All rights reserved.
//

#pragma once

#include <AgoraRtcKit/AgoraMediaFilterExtensionDelegate.h>

@interface SAFProvider : NSObject <AgoraExtProviderDelegate>

- (instancetype)init;
- (NSInteger)log:(AgoraExtLogLevel)level message:(NSString * __nullable)message;
- (NSInteger)fireEvent:(NSString * __nonnull)vendor key:(NSString * __nullable)key value:(NSString * __nullable)value;

@end
