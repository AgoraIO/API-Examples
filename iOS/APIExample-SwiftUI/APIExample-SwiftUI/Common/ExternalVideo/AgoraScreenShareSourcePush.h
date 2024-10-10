//
//  AgoraScreenShare.h
//  AgoraShareSrceen
//
//  Created by XC on 2021/7/3.
//
#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>
#import <AgoraRtcKit/AgoraRtcEngineKit.h>

NS_ASSUME_NONNULL_BEGIN
@class AgoraScreenShareSourcePush;
@protocol AgoraScreenShareSourcePushDelegate<NSObject>

- (void)screenSourceCapture:(AgoraScreenShareSourcePush *)capture outputBuffer:(CVPixelBufferRef)pixelBuffer rotation:(int)rotation time:(CMTime)time;
- (void)pushAudioAppFrame:(unsigned char *)inAudioFrame frameSize:(int64_t)frameSize;
//- (void)pushAudioAppFrame:(CMSampleBufferRef)inAudioFrame;
//- (void)pushAudioMicFrame:(unsigned char *)inAudioFrame frameSize:(int64_t)frameSize;
- (void)screenShareStop;
@end

@interface AgoraScreenShareSourcePush : NSObject
- (instancetype)initWithDelegate:(id<AgoraScreenShareSourcePushDelegate>)delegate port:(uint16_t) port;
- (void)startCapture;
- (void)stopCapture;
- (void)localSnapshot:(void (^ _Nullable)(UIImage * _Nonnull image))completion;
@end
NS_ASSUME_NONNULL_END
