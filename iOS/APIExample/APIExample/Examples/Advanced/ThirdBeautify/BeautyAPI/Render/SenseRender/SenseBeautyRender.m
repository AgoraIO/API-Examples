//
//  SenseBeatuyRender.m
//  BeautyAPi
//
//  Created by zhaoyongqiang on 2023/6/30.
//

#import "SenseBeautyRender.h"

@interface SenseBeautyRender ()

#if __has_include(Sensetime)
@property (nonatomic, strong) NSTimer *timer;
///贴纸id
@property (nonatomic, assign) int stickerId;
@property (nonatomic, assign) int filterId;
#endif

@end

@implementation SenseBeautyRender

- (instancetype)init {
    if (self == [super init]) {
#if __has_include(Sensetime)
        [self checkSensetimeLicense];
#endif
    }
    return self;
}

- (NSDictionary *)sensetimeDefault {
    NSDictionary *params = @{
        @"103": @(0.55),
        @"101": @(0.2),
        @"201": @(0.2),
        @"318": @(0),
        @"202": @(0.3),
        @"306": @(0),
        @"303": @(0),
        @"320": @(0),
        @"304": @(0),
        @"309": @(0),
        @"317": @(0),
    };
    return params;
}

#if __has_include("VideoProcessingManager.h")
- (VideoProcessingManager *)videoProcessing {
    if (_videoProcessing == nil) {
        _videoProcessing = [VideoProcessingManager new];
    }
    return _videoProcessing;
}
#endif

#if __has_include(Sensetime)
- (void)checkSensetimeLicense {
    NSString *licensePath = [[NSBundle mainBundle] pathForResource:@"SENSEME" ofType:@"lic"];
    self.isSuccessLicense = [EffectsProcess authorizeWithLicensePath:licensePath];
    __weak SenseBeautyRender *weakSelf = self;
    self.timer = [NSTimer timerWithTimeInterval:1 repeats:YES block:^(NSTimer * _Nonnull timer) {
        weakSelf.isSuccessLicense = weakSelf.videoProcessing.effectsProcess.isAuthrized;
        if (weakSelf.isSuccessLicense) {
            [weakSelf.timer invalidate];
            weakSelf.timer = nil;
        }
        if (weakSelf.licenseEventCallback) {
            weakSelf.licenseEventCallback(weakSelf.isSuccessLicense);
        }
    }];
    [[NSRunLoop mainRunLoop]addTimer:self.timer forMode:NSRunLoopCommonModes];
}
#endif

- (void)destroy { 
#if __has_include(Sensetime)
    [self reset];
    _videoProcessing = nil;
#endif
}

- (nonnull CVPixelBufferRef)onCapture:(nonnull CVPixelBufferRef)pixelBuffer { 
#if __has_include(Sensetime)
    if (self.isSuccessLicense) {
        return [self.videoProcessing videoProcessHandler:pixelBuffer];
    }
    return nil;
#endif
    return pixelBuffer;
}

#if __has_include(<AgoraRtcKit/AgoraRtcKit.h>)
- (AgoraVideoFormat)getVideoFormatPreference {
    return AgoraVideoFormatCVPixelNV12;
}
#endif

- (void)reset { 
#if __has_include(Sensetime)
    for (NSString *key in [self sensetimeDefault].allKeys) {
        int type = key.intValue;
        [self.videoProcessing setEffectType:type value: 0];
    }
#endif
}

- (void)setBeautyPreset { 
    if (self.isSuccessLicense == NO) {
        __weak SenseBeautyRender *weakSelf = self;
        self.licenseEventCallback = ^(BOOL isSuccess) {
            if (isSuccess) {
                [weakSelf setBeautyDefault];
            }
        };
        return;
    }
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, 0.1 * NSEC_PER_SEC), dispatch_get_main_queue(), ^{
        [self setBeautyDefault];
    });
}

- (void)setBeautyDefault {
#if __has_include(Sensetime)
    for (NSString *key in [self sensetimeDefault].allKeys) {
        int type = key.intValue;
        float value = [[self sensetimeDefault][key]floatValue];
        [self.videoProcessing setEffectType:type value: value];
    }
#endif
}

- (void)setMakeup:(BOOL)isSelected { 
#if __has_include(Sensetime)
    if (isSelected) {
        NSString *path = [[NSBundle mainBundle] pathForResource:@"qise.zip" ofType:nil];
        __weak SenseBeautyRender *weakself = self;
        [self.videoProcessing.effectsProcess addStickerWithPath:path callBack:^(st_result_t state, int sticker, uint64_t action) {
            [weakself.videoProcessing.effectsProcess setPackageId:sticker groupType:EFFECT_BEAUTY_GROUP_MAKEUP strength:0.5];
            weakself.stickerId = sticker;
        }];
    } else {
        [self.videoProcessing.effectsProcess removeSticker:self.stickerId];
        self.stickerId = 0;
    }
#endif
}

- (void)setSticker:(BOOL)isSelected { 
#if __has_include(Sensetime)
    if (isSelected) {
        NSString *path = [[NSBundle mainBundle] pathForResource:@"lianxingface.zip" ofType:nil];
        [self.videoProcessing.effectsProcess setStickerWithPath:path callBack:^(st_result_t state, int stickerId, uint64_t action) {
            
        }];
    } else {
        [self.videoProcessing cleareStickers];
    }
#endif
}

@end
