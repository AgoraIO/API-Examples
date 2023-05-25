//
//  ByteDanceFilter.m
//  FULiveDemo
//
//  Created by 刘洋 on 2017/8/18.
//  Copyright © 2017年 刘洋. All rights reserved.
//

#import "ByteDanceFilter.h"
#import "BEFrameProcessor.h"

@interface ByteDanceFilter(){
    BEFrameProcessor *_processor;
}

@end

static ByteDanceFilter *shareManager = NULL;

@implementation ByteDanceFilter

+ (ByteDanceFilter *)shareManager
{
    __block ByteDanceFilter *shareManager;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        shareManager = [[ByteDanceFilter alloc] init];
    });
    return shareManager;
}

- (instancetype)init
{
    if (self = [super init]) {
        EAGLContext *context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
        [EAGLContext setCurrentContext:context];
        _processor = [[BEFrameProcessor alloc] initWithContext:context resourceDelegate:nil];
        _processor.processorResult = BECVPixelBuffer;
        
        [_processor setEffectOn:YES];
//        [_processor setFilterPath:@"Filter_32_Po10"];
//        [_processor setStickerPath:@"test_sticker"];
        [_processor updateComposerNodes:@[@"/beauty_IOS_lite"]];
    }
    
    return self;
}

- (void)setBuauty: (BOOL)isSelected {
#if __has_include("bef_effect_ai_api.h")
    if (isSelected) {
        [_processor updateComposerNodeIntensity:@"/beauty_IOS_lite" key:@"whiten" intensity:0.6];
        [_processor updateComposerNodeIntensity:@"/beauty_IOS_lite" key:@"smooth" intensity:0.6];
    } else {
        [_processor updateComposerNodeIntensity:@"/beauty_IOS_lite" key:@"whiten" intensity:0];
        [_processor updateComposerNodeIntensity:@"/beauty_IOS_lite" key:@"smooth" intensity:0];
    }
#endif
}
- (void)setMakeup: (BOOL)isSelected {
#if __has_include("bef_effect_ai_api.h")
    if (isSelected) {
        [_processor updateComposerNodeIntensity:@"/style_makeup/tianmei" key:@"Makeup_ALL" intensity:0.6];
    } else {
        [_processor updateComposerNodeIntensity:@"/style_makeup/tianmei" key:@"Makeup_ALL" intensity:0];
    }
#endif
}
- (void)setSticker: (BOOL)isSelected {
#if __has_include("bef_effect_ai_api.h")
    if (isSelected) {
        [_processor setStickerPath:@"wochaotian"];
    } else {
        [_processor setStickerPath:@""];
    }
#endif
}
- (void)setFilter: (BOOL)isSelected {
#if __has_include("bef_effect_ai_api.h")
    if (isSelected) {
        [_processor setFilterPath:@"Filter_02_14"];
        [_processor setFilterIntensity:0.4];
    } else {
        [_processor setFilterIntensity:0];
    }
#endif
}


#pragma mark - VideoFilterDelegate
/// process your video frame here
- (CVPixelBufferRef)processFrame:(CVPixelBufferRef)frame timeStamp:(double)timeStamp{
    if(self.enabled) {
        BEProcessResult *result = [_processor process:frame timeStamp:timeStamp];
        return result.pixelBuffer;
    }
    return frame;
}


@end
