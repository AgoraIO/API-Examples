//
//  FUManager.m
//  FULiveDemo
//
//  Created by 刘洋 on 2017/8/18.
//  Copyright © 2017年 刘洋. All rights reserved.
//

#import "FUManager.h"
#import "BundleUtil.h"
#if __has_include(<FURenderKit/FURenderKit.h>)
#import "authpack.h"
#import <FURenderKit/FURenderKit.h>
#endif

static FUManager *shareManager = NULL;

@interface FUManager ()

#if __has_include(<FURenderKit/FURenderKit.h>)
/// Current sticker
@property (nonatomic, strong) FUSticker *currentSticker;
#endif

@end

@implementation FUManager

+ (FUManager *)shareManager
{
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        shareManager = [[FUManager alloc] init];
    });
    
    return shareManager;
}

- (instancetype)init
{
    if (self = [super init]) {
#if __has_include(<FURenderKit/FURenderKit.h>)
        dispatch_async(dispatch_get_global_queue(0, 0), ^{
            NSString *controllerPath = [[NSBundle mainBundle] pathForResource:@"controller_cpp" ofType:@"bundle"];
            NSString *controllerConfigPath = [[NSBundle mainBundle] pathForResource:@"controller_config" ofType:@"bundle"];
            FUSetupConfig *setupConfig = [[FUSetupConfig alloc] init];
            setupConfig.authPack = FUAuthPackMake(g_auth_package, sizeof(g_auth_package));
            setupConfig.controllerPath = controllerPath;
            setupConfig.controllerConfigPath = controllerConfigPath;
            
            // Initialize FURenderKit
            [FURenderKit setupWithSetupConfig:setupConfig];
            
            [FURenderKit setLogLevel:FU_LOG_LEVEL_ERROR];
            
            // Load face AI model
            NSString *faceAIPath = [[NSBundle mainBundle] pathForResource:@"ai_face_processor" ofType:@"bundle"];
            [FUAIKit loadAIModeWithAIType:FUAITYPE_FACEPROCESSOR dataPath:faceAIPath];
            
            // Load body AI model
            NSString *bodyAIPath = [[NSBundle mainBundle] pathForResource:@"ai_human_processor" ofType:@"bundle"];
            [FUAIKit loadAIModeWithAIType:FUAITYPE_HUMAN_PROCESSOR dataPath:bodyAIPath];
            
            NSString *path = [[NSBundle mainBundle] pathForResource:@"tongue" ofType:@"bundle"];
            [FUAIKit loadTongueMode:path];
            
            /* Set mouth flexibility, default = 0 */ //
            float flexible = 0.5;
            [FUAIKit setFaceTrackParam:@"mouth_expression_more_flexible" value:flexible];
            
            // Set face algorithm quality
            [FUAIKit shareKit].faceProcessorFaceLandmarkQuality = [FURenderKit devicePerformanceLevel] == FUDevicePerformanceLevelHigh ? FUFaceProcessorFaceLandmarkQualityHigh : FUFaceProcessorFaceLandmarkQualityMedium;
            
            // Set small face detection
            [FUAIKit shareKit].faceProcessorDetectSmallFace = [FURenderKit devicePerformanceLevel] == FUDevicePerformanceLevelHigh;
        });
        
        [FUAIKit shareKit].maxTrackFaces = 4;
#endif
    }
    return self;
}

- (void)destoryItems {
#if __has_include(<FURenderKit/FURenderKit.h>)
    [FURenderKit shareRenderKit].beauty = nil;
    [FURenderKit shareRenderKit].bodyBeauty = nil;
    [FURenderKit shareRenderKit].makeup = nil;
    [[FURenderKit shareRenderKit].stickerContainer removeAllSticks];
    self.currentSticker = nil;
#endif
}

- (void)setBuauty: (BOOL)isSelected {
#if __has_include(<FURenderKit/FURenderKit.h>)
    if (isSelected) {
        NSString *beautyPath = [[NSBundle mainBundle] pathForResource:@"face_beautification" ofType:@"bundle"];
        FUBeauty *beauty = [[FUBeauty alloc] initWithPath:beautyPath name:@"FUBeauty"];
        // Default uniform blur
        beauty.heavyBlur = 0;
        beauty.blurType = 3;
        [FURenderKit shareRenderKit].beauty = beauty;
    } else {
        [FURenderKit shareRenderKit].beauty = nil;
    }
#endif
}
- (void)setMakeup: (BOOL)isSelected {
#if __has_include(<FURenderKit/FURenderKit.h>)
    if (isSelected) {
        NSString *beautyPath = [[NSBundle mainBundle] pathForResource:@"face_makeup" ofType:@"bundle"];
        FUMakeup *makeup = [[FUMakeup alloc] initWithPath:beautyPath name:@"face_makeup"];
        makeup.isMakeupOn = YES;
        [FURenderKit setLogLevel:FU_LOG_LEVEL_DEBUG];
        
        [FURenderKit shareRenderKit].makeup = makeup;
        [FURenderKit shareRenderKit].makeup.enable = isSelected;
        
        NSBundle *bundle = [BundleUtil bundleWithBundleName:@"FURenderKit" podName:@"fuLib"];
        NSString *makeupPath = [bundle pathForResource:@"美妆/ziyun" ofType:@"bundle"];
        FUItem *makeupItem = [[FUItem alloc] initWithPath:makeupPath name:@"ziyun"];
        [makeup updateMakeupPackage:makeupItem needCleanSubItem:NO];
        makeup.intensity = 0.9;
    } else {
        [FURenderKit shareRenderKit].makeup.enable = NO;
        [FURenderKit shareRenderKit].makeup = nil;
    }
#endif
}
- (void)setSticker: (BOOL)isSelected {
#if __has_include(<FURenderKit/FURenderKit.h>)
    if (isSelected) {
        [self setStickerPath:@"DaisyPig"];
    } else {
        [[FURenderKit shareRenderKit].stickerContainer removeAllSticks];
    }
#endif
}
- (void)setFilter: (BOOL)isSelected {
#if __has_include(<FURenderKit/FURenderKit.h>)
    if (isSelected) {
        NSString *beautyPath = [[NSBundle mainBundle] pathForResource:@"face_beautification" ofType:@"bundle"];
        FUBeauty *beauty = [[FUBeauty alloc] initWithPath:beautyPath name:@"FUBeauty"];
        beauty.filterName = FUFilterMiTao1;
        beauty.filterLevel = 0.8;
        [FURenderKit shareRenderKit].beauty = beauty;
    } else {
        [FURenderKit shareRenderKit].beauty = nil;
    }
#endif
}


- (void)setStickerPath: (NSString *)stickerName {
    NSBundle *bundle = [BundleUtil bundleWithBundleName:@"FURenderKit" podName:@"fuLib"];
    NSString *path = [bundle pathForResource:[NSString stringWithFormat:@"贴纸/%@", stickerName] ofType:@"bundle"];
    if (!path) {
        NSLog(@"FaceUnity：找不到贴纸路径");
        return;
    }
#if __has_include(<FURenderKit/FURenderKit.h>)
    FUSticker *sticker = [[FUSticker alloc] initWithPath:path name:@"sticker"];
    if (self.currentSticker) {
        [[FURenderKit shareRenderKit].stickerContainer replaceSticker:self.currentSticker withSticker:sticker completion:nil];
    } else {
        [[FURenderKit shareRenderKit].stickerContainer addSticker:sticker completion:nil];
    }
    self.currentSticker = sticker;
#endif
}

- (void)updateBeautyBlurEffect {
#if __has_include(<FURenderKit/FURenderKit.h>)
    if (![FURenderKit shareRenderKit].beauty || ![FURenderKit shareRenderKit].beauty.enable) {
        return;
    }
    if ([FURenderKit devicePerformanceLevel] == FUDevicePerformanceLevelHigh) {
        // Set different blur effects based on face detection confidence
        CGFloat score = [FUAIKit fuFaceProcessorGetConfidenceScore:0];
        if (score > 0.95) {
            [FURenderKit shareRenderKit].beauty.blurType = 3;
            [FURenderKit shareRenderKit].beauty.blurUseMask = YES;
        } else {
            [FURenderKit shareRenderKit].beauty.blurType = 2;
            [FURenderKit shareRenderKit].beauty.blurUseMask = NO;
        }
    } else {
        // Set fine blur effect
        [FURenderKit shareRenderKit].beauty.blurType = 2;
        [FURenderKit shareRenderKit].beauty.blurUseMask = NO;
    }
#endif
}


#pragma mark - VideoFilterDelegate

- (CVPixelBufferRef)processFrame:(CVPixelBufferRef)frame {
    [self updateBeautyBlurEffect];
    if ([self.delegate respondsToSelector:@selector(faceUnityManagerCheckAI)]) {
        [self.delegate faceUnityManagerCheckAI];
    }
#if __has_include(<FURenderKit/FURenderKit.h>)
    if ([FURenderKit shareRenderKit].beauty == nil) {
        return frame;
    }
    FURenderInput *input = [[FURenderInput alloc] init];
    input.pixelBuffer = frame;
    // By default, faces in the image are always upright, no need to modify this property when rotating the screen
    input.renderConfig.imageOrientation = FUImageOrientationUP;
    // Enable gravity sensing, internal will automatically calculate correct orientation, set fuSetDefaultRotationMode, no need to set outside
    input.renderConfig.gravityEnable = YES;
    // Must be set if the source is from camera capture, otherwise will cause internal detection abnormal
    input.renderConfig.isFromFrontCamera = YES;
    // This property indicates whether the system camera is mirrored: generally front camera frames are mirrored by default, so need to set this. If camera property is not mirrored, no need to set this property.
    input.renderConfig.isFromMirroredCamera = YES;
    FURenderOutput *output = [[FURenderKit shareRenderKit] renderWithInput:input];
    return output.pixelBuffer;
#else
    return frame;
#endif
}

@end
