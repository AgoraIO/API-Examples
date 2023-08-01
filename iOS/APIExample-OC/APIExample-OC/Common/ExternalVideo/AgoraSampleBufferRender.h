//
//  AgoraSampleBufferRender.h
//  APIExample
//
//  Created by 胡润辰 on 2022/4/2.
//  Copyright © 2022 Agora Corp. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <AVFoundation/AVFoundation.h>
#import <AgoraRtcKit/AgoraRtcEngineKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface AgoraSampleBufferRender : UIView

@property (nonatomic, readonly) AVSampleBufferDisplayLayer *displayLayer;

- (void)reset;

- (void)renderVideoData:(AgoraOutputVideoFrame *_Nonnull)videoData;

- (void)renderVideoPixelBuffer:(AgoraOutputVideoFrame *_Nonnull)videoData;

- (void)renderVideoSampleBuffer:(CMSampleBufferRef)sampleBufferRef size:(CGSize)size;

@end

NS_ASSUME_NONNULL_END
