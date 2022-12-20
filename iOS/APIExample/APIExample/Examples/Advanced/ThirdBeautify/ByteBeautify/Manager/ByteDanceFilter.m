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
        [_processor setStickerPath:@"test_sticker"];
        [_processor updateComposerNodes:@[@"/beauty_IOS_lite"]];
        [_processor updateComposerNodeIntensity:@"/beauty_IOS_lite" key:@"smooth" intensity:0.8];
        [_processor updateComposerNodeIntensity:@"/beauty_IOS_lite" key:@"whiten" intensity:0.9];
        [_processor updateComposerNodeIntensity:@"/beauty_IOS_lite" key:@"sharp" intensity:0.96];
        [_processor updateComposerNodeIntensity:@"/reshape_lite" key:@"Internal_Deform_Overall" intensity:0.95];
        [_processor updateComposerNodeIntensity:@"/reshape_lite" key:@"Internal_Deform_Eye" intensity:0.95];
        [_processor updateComposerNodeIntensity:@"/reshape_lite" key:@"Internal_Deform_MovNose" intensity:0.0];
    }
    
    return self;
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
