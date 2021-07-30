//
//  SAFilter.m
//  SimpleAudioFilter
//
//  Created by xianing on 2021/7/29.
//  Copyright © 2021 Agora Corp. All rights reserved.
//

#include "SAFilter.h"
#import "SAFErrorCode.h"
#import "SimpleAudioFilterManager.h"

@implementation SAFilter {
    BOOL enabled;
}

using limits_int16 = std::numeric_limits<int16_t>;
int16_t FloatS16ToS16(float v) {
    static const float kMaxRound = (limits_int16::max)() - 0.5f;
    static const float kMinRound = (limits_int16::min)() + 0.5f;
    if (v > 0) {
        return v >= kMaxRound ? (limits_int16::max)() : static_cast<int16_t>(v + 0.5f);
    }
    return v <= kMinRound ? (limits_int16::min)() : static_cast<int16_t>(v - 0.5f);
}

- (BOOL)adaptAudioFrame:(AgoraAudioPcmFrame *)srcFrame dstFrame:(AgoraAudioPcmFrame * _Nullable __autoreleasing *)dstFrame {
    if(enabled){
        size_t length = srcFrame.samplesPerChannel * srcFrame.channelNumbers;
        UInt8 buf[length];
        [srcFrame.pcmBuffer getBytes:buf length:length];
        for (int idx = 0; idx < length; idx++) {
            buf[idx] = FloatS16ToS16(buf[idx] * 2);
        }
        *dstFrame = srcFrame;
        return true;
    }
    return false;
}

- (NSString * _Nonnull)getName {
    return [SimpleAudioFilterManager vendorName];
}


- (NSInteger)getPropertyWithKey:(NSString * _Nonnull)key value:(NSData * _Nullable __autoreleasing * _Nullable)value {
    return -1;
}


- (BOOL)isEnabled {
    return enabled;
}


- (void)setEnabled:(BOOL)enabled {
    enabled = enabled;
}


- (NSInteger)setPropertyWithKey:(NSString * _Nonnull)key value:(NSData * _Nonnull)value {
    return -1;
}


@end
