//
//  AgoraAudioTube.h
//  Agora-Screen-Sharing-iOS-Broadcast
//
//  Created by CavanSu on 2019/12/4.
//  Copyright © 2019 Agora. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <AgoraRtcKit/AgoraRtcEngineKit.h>

typedef NS_OPTIONS(NSUInteger, AudioType) {
    AudioTypeApp = 1,
    AudioTypeMic = 2
};

@interface AgoraAudioTube : NSObject
+ (void)agoraKit:(AgoraRtcEngineKit * _Nonnull)agoraKit pushAudioCMSampleBuffer:(CMSampleBufferRef _Nonnull)sampleBuffer resampleRate:(NSUInteger)resampleRate type:(AudioType)type;
@end
