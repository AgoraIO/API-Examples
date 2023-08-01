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

- (OSType)getFormatType: (NSInteger)type {
    switch (type) {
        case 1:
            return kCVPixelFormatType_420YpCbCr8Planar;
            
        case 2:
            return kCVPixelFormatType_32BGRA;
            
        default:
            return kCVPixelFormatType_32BGRA;
    }
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
    
    size_t width = videoData.width;
    size_t height = videoData.height;
    size_t yStride = videoData.yStride;
    size_t uStride = videoData.uStride;
    size_t vStride = videoData.vStride;
    
    void* yBuffer = videoData.yBuffer;
    void* uBuffer = videoData.uBuffer;
    void* vBuffer = videoData.vBuffer;
    
    @autoreleasepool {
        CVPixelBufferRef pixelBuffer = NULL;
        NSDictionary *pixelAttributes = @{(id)kCVPixelBufferIOSurfacePropertiesKey : @{}};
        OSType type = [self getFormatType:videoData.type];
        CVReturn result = CVPixelBufferCreate(kCFAllocatorDefault,
                                              width,
                                              height,
                                              type,
                                              (__bridge CFDictionaryRef)(pixelAttributes),
                                              &pixelBuffer);
        
        if (result != kCVReturnSuccess) {
            NSLog(@"Unable to create cvpixelbuffer %d", result);
        }
        
        CVPixelBufferLockBaseAddress(pixelBuffer, 0);
        
        void *yPlane = CVPixelBufferGetBaseAddressOfPlane(pixelBuffer, 0);
        int pixelBufferYBytes = (int)CVPixelBufferGetBytesPerRowOfPlane(pixelBuffer, 0);
        
        if (yStride == pixelBufferYBytes) {
            memcpy(yPlane, yBuffer, yStride*height);
        }else {
            for (int i = 0; i < height; ++i) {
                memcpy(yPlane + pixelBufferYBytes * i, yBuffer + yStride * i, MIN(yStride, pixelBufferYBytes));
            }
        }
        
        void *uPlane = CVPixelBufferGetBaseAddressOfPlane(pixelBuffer, 1);
        int pixelBufferUBytes = (int)CVPixelBufferGetBytesPerRowOfPlane(pixelBuffer, 1);
        if (uStride == pixelBufferUBytes) {
            memcpy(uPlane, uBuffer, uStride*height/2);
        }else {
            for (int i = 0; i < height/2; ++i) {
                memcpy(uPlane + pixelBufferUBytes * i, uBuffer + uStride * i, MIN(uStride, pixelBufferUBytes));
            }
        }
        
        void *vPlane = (void *)CVPixelBufferGetBaseAddressOfPlane(pixelBuffer, 2);
        int pixelBufferVBytes = (int)CVPixelBufferGetBytesPerRowOfPlane(pixelBuffer, 2);
        if (vStride == pixelBufferVBytes) {
            memcpy(vPlane, vBuffer, vStride*height/2);
        }else {
            for (int i = 0; i < height/2; ++i) {
                memcpy(vPlane + pixelBufferVBytes * i, vBuffer + vStride * i, MIN(vStride, pixelBufferVBytes));
            }
        }
        
        CVPixelBufferUnlockBaseAddress(pixelBuffer, 0);
        
        CMVideoFormatDescriptionRef videoInfo;
        CMVideoFormatDescriptionCreateForImageBuffer(kCFAllocatorDefault, pixelBuffer, &videoInfo);
        
        CMSampleTimingInfo timingInfo;
        timingInfo.duration = kCMTimeZero;
        timingInfo.decodeTimeStamp = kCMTimeInvalid;
        timingInfo.presentationTimeStamp = CMTimeMake(CACurrentMediaTime()*1000, 1000);
        
        CMSampleBufferRef sampleBuffer;
        CMSampleBufferCreateReadyWithImageBuffer(kCFAllocatorDefault, pixelBuffer, videoInfo, &timingInfo, &sampleBuffer);
        
        [self.displayLayer enqueueSampleBuffer:sampleBuffer];
        if (self.displayLayer.status == AVQueuedSampleBufferRenderingStatusFailed) {
            [self.displayLayer flush];
        }
        CMSampleBufferInvalidate(sampleBuffer);
        CFRelease(sampleBuffer);
        
        CVPixelBufferRelease(pixelBuffer);
    }
}

- (void)renderVideoSampleBuffer:(CMSampleBufferRef)sampleBufferRef size:(CGSize)size {
    if (!sampleBufferRef) {
        return;
    }
    
    dispatch_async(dispatch_get_main_queue(), ^{
        self->_videoWidth = size.width;
        self->_videoHeight = size.height;
        
        [self layoutDisplayLayer];
    });
    @autoreleasepool {
        CMSampleTimingInfo timingInfo;
        timingInfo.duration = kCMTimeZero;
        timingInfo.decodeTimeStamp = kCMTimeInvalid;
        timingInfo.presentationTimeStamp = CMTimeMake(CACurrentMediaTime()*1000, 1000);
        
        if (sampleBufferRef) {
            [self.displayLayer enqueueSampleBuffer:sampleBufferRef];
            [self.displayLayer setNeedsDisplay];
            [_displayLayer display];
            [self.layer display];
        }
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
        CMVideoFormatDescriptionCreateForImageBuffer(kCFAllocatorDefault,
                                                     pixelBuffer,
                                                     &videoInfo);
        
        CMSampleTimingInfo timingInfo;
        timingInfo.duration = kCMTimeZero;
        timingInfo.decodeTimeStamp = kCMTimeInvalid;
        timingInfo.presentationTimeStamp = CMTimeMake(CACurrentMediaTime()*1000, 1000);
        
        CMSampleBufferRef sampleBuffer;
        CMSampleBufferCreateReadyWithImageBuffer(kCFAllocatorDefault,
                                                 pixelBuffer,
                                                 videoInfo,
                                                 &timingInfo,
                                                 &sampleBuffer);
        
        [self.displayLayer enqueueSampleBuffer:sampleBuffer];
        if (self.displayLayer.status == AVQueuedSampleBufferRenderingStatusFailed) {
            [self.displayLayer flush];
        }
        CMSampleBufferInvalidate(sampleBuffer);
        CFRelease(sampleBuffer);
    }
}

@end
