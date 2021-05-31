//
//  AGVideoPreProcessing.h
//  Agora-Screen-Sharing-iOS-Broadcast
//
//  Created by Alex Zheng on 7/28/16.
//  Copyright © 2016 Agora.io All rights reserved.
//

#import <CoreMedia/CoreMedia.h>
#import <AgoraRtcKit/AgoraRtcEngineKit.h>

@interface AgoraAudioProcessing : NSObject
+ (void)registerAudioPreprocessing:(AgoraRtcEngineKit*) kit;
+ (void)deregisterAudioPreprocessing:(AgoraRtcEngineKit*) kit;
+ (void)pushAudioFrame:(unsigned char *)inAudioFrame withFrameSize:(int64_t)frameSize;
@end
