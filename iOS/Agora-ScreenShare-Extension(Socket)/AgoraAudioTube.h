//
//  AgoraAudioTube.h
//  Agora-Screen-Sharing-iOS-Broadcast
//
//  Created by CavanSu on 2019/12/4.
//  Copyright © 2019 Agora. All rights reserved.
//

#import <Foundation/Foundation.h>

typedef NS_OPTIONS(NSUInteger, AudioType) {
    AudioTypeApp = 1,
    AudioTypeMic = 2
};

typedef void (^PushAudioFrame)(unsigned char * _Nonnull inAudioFrame, int64_t frameSize);

@interface AgoraAudioTube : NSObject
+ (void)registerAudioPreprocessing:(PushAudioFrame _Nullable)callback;
+ (void)pushAudioCMSampleBuffer:(CMSampleBufferRef _Nonnull)sampleBuffer resampleRate:(NSUInteger)resampleRate type:(AudioType)type;
@end
