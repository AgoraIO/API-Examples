//
//  AgoraSampleBufferRender.m
//  APIExample
//
//  Created by 胡润辰 on 2022/4/2.
//  Copyright © 2022 Agora Corp. All rights reserved.
//

#import "AgoraSampleBufferRender.h"

@interface AgoraSampleBufferRender () {
    NSInteger _videoWidth, _videoHeight;
}

@property (nonatomic, strong) AVSampleBufferDisplayLayer *displayLayer;

@end

@implementation AgoraSampleBufferRender

- (AVSampleBufferDisplayLayer *)displayLayer {
    if (!_displayLayer) {
        _displayLayer = [AVSampleBufferDisplayLayer new];
    }
    
    return _displayLayer;
}

- (instancetype)init {
    if (self = [super init]) {
        [self.layer addSublayer:self.displayLayer];
    }
    return self;
}

- (instancetype)initWithFrame:(CGRect)frame {
    if (self = [super initWithFrame:frame]) {
        [self.layer addSublayer:self.displayLayer];
    }
    return self;
}

- (void)awakeFromNib {
    [super awakeFromNib];
    [self.layer addSublayer:self.displayLayer];
}

- (void)layoutSubviews {
    [super layoutSubviews];
    
    self.clipsToBounds = YES;
    [self layoutDisplayLayer];
}

- (void)layoutDisplayLayer {
    if (_videoWidth == 0 || _videoHeight == 0 || CGSizeEqualToSize(self.frame.size, CGSizeZero)) {
        return;
    }
    
    CGFloat viewWidth = self.frame.size.width;
    CGFloat viewHeight = self.frame.size.height;
    CGFloat videoRatio = (CGFloat)_videoWidth/(CGFloat)_videoHeight;
    CGFloat viewRatio = viewWidth/viewHeight;
    
    CGSize videoSize;
    if (videoRatio >= viewRatio) {
        videoSize.height = viewHeight;
        videoSize.width = videoSize.height * videoRatio;
    }else {
        videoSize.width = viewWidth;
        videoSize.height = videoSize.width / videoRatio;
    }
    
    CGRect renderRect = CGRectMake(0.5 * (viewWidth - videoSize.width), 0.5 * (viewHeight - videoSize.height), videoSize.width, videoSize.height);

    if (!CGRectEqualToRect(renderRect, self.displayLayer.frame)) {
        self.displayLayer.frame = renderRect;
    }
}

- (void)reset {
    [self.displayLayer flushAndRemoveImage];
}

- (void)renderVideoData:(AgoraOutputVideoFrame *_Nonnull)videoData {
    if (!videoData) {
        return;
    }
    
    dispatch_async(dispatch_get_main_queue(), ^{
        self->_videoWidth = videoData.width;
        self->_videoHeight = videoData.height;

        [self layoutDisplayLayer];
    });

    @autoreleasepool {
        CVPixelBufferUnlockBaseAddress(videoData.pixelBuffer, 0);

        CMVideoFormatDescriptionRef videoInfo;
        CMVideoFormatDescriptionCreateForImageBuffer(kCFAllocatorDefault, videoData.pixelBuffer, &videoInfo);
        
        CMSampleTimingInfo timingInfo;
        timingInfo.duration = kCMTimeZero;
        timingInfo.decodeTimeStamp = kCMTimeInvalid;
        timingInfo.presentationTimeStamp = CMTimeMake(CACurrentMediaTime()*1000, 1000);
        
        CMSampleBufferRef sampleBuffer;
        CMSampleBufferCreateReadyWithImageBuffer(kCFAllocatorDefault, videoData.pixelBuffer, videoInfo, &timingInfo, &sampleBuffer);

        [self.displayLayer enqueueSampleBuffer:sampleBuffer];
        if (self.displayLayer.status == AVQueuedSampleBufferRenderingStatusFailed) {
            [self.displayLayer flush];
        }
        CMSampleBufferInvalidate(sampleBuffer);
        CFRelease(sampleBuffer);
    }
}

- (void)renderVideoPixelBuffer:(AgoraOutputVideoFrame *_Nonnull)videoData {
    if (!videoData) {
        return;
    }
    
    dispatch_async(dispatch_get_main_queue(), ^{
        self->_videoWidth = videoData.width;
        self->_videoHeight = videoData.height;
        
        [self layoutDisplayLayer];
    });
    
    @autoreleasepool {
        CVPixelBufferRef pixelBuffer = videoData.pixelBuffer;
       
        CMVideoFormatDescriptionRef videoInfo;
        CMVideoFormatDescriptionCreateForImageBuffer(kCFAllocatorDefault, pixelBuffer, &videoInfo);
        
        CMSampleTimingInfo timingInfo;
        timingInfo.duration = kCMTimeZero;
        timingInfo.decodeTimeStamp = kCMTimeInvalid;
        timingInfo.presentationTimeStamp = CMTimeMake(CACurrentMediaTime()*1000, 1000);
        
        CMSampleBufferRef sampleBuffer;
        CMSampleBufferCreateReadyWithImageBuffer(kCFAllocatorDefault, pixelBuffer, videoInfo, &timingInfo, &sampleBuffer);

        [self.displayLayer enqueueSampleBuffer:sampleBuffer];
        CMSampleBufferInvalidate(sampleBuffer);
        CFRelease(sampleBuffer);
    }
}

@end
