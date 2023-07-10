//
//  FUManager.m
//  FULiveDemo
//
//  Created by 刘洋 on 2017/8/18.
//  Copyright © 2017年 刘洋. All rights reserved.
//

#import "FUManager.h"
#import "authpack.h"
#import "BundleUtil.h"
#if __has_include(<FURenderKit/FURenderKit.h>)
#import <FURenderKit/FURenderKit.h>
#endif

static FUManager *shareManager = NULL;

@interface FUManager ()

#if __has_include(<FURenderKit/FURenderKit.h>)
/// 当前的贴纸
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
        NSBundle *bundle = [BundleUtil bundleWithBundleName:@"FURenderKit" podName:@"fuLib"];
//        [[NSBundle mainBundle] pathForResource:@"controller_cpp" ofType:@"bundle" inDirectory:@"Frameworks"]
        NSString *controllerPath = [bundle pathForResource:@"graphics/controller_cpp" ofType:@"bundle"];
//        NSString *controllerPath = [[NSBundle mainBundle] pathForResource:@"controller_cpp" ofType:@"bundle"];
        NSString *controllerConfigPath = [bundle pathForResource:@"graphics/controller_config" ofType:@"bundle"];
        FUSetupConfig *setupConfig = [[FUSetupConfig alloc] init];
        setupConfig.authPack = FUAuthPackMake(g_auth_package, sizeof(g_auth_package));
        setupConfig.controllerPath = controllerPath;
        setupConfig.controllerConfigPath = controllerConfigPath;
                
        // 初始化 FURenderKit
        [FURenderKit setupWithSetupConfig:setupConfig];
        
        [FURenderKit setLogLevel:FU_LOG_LEVEL_ERROR];
        
        dispatch_async(dispatch_get_global_queue(0, 0), ^{
            // 加载人脸 AI 模型
            NSString *faceAIPath = [bundle pathForResource:@"model/ai_face_processor" ofType:@"bundle"];//[[NSBundle mainBundle] pathForResource:@"ai_face_processor" ofType:@"bundle"];
            [FUAIKit loadAIModeWithAIType:FUAITYPE_FACEPROCESSOR dataPath:faceAIPath];
            
            // 加载身体 AI 模型
            NSString *bodyAIPath = [bundle pathForResource:@"model/ai_human_processor" ofType:@"bundle"];//[[NSBundle mainBundle] pathForResource:@"ai_human_processor" ofType:@"bundle"];
            [FUAIKit loadAIModeWithAIType:FUAITYPE_HUMAN_PROCESSOR dataPath:bodyAIPath];
            
            NSString *path = [bundle pathForResource:@"graphics/tongue" ofType:@"bundle"];//[[NSBundle mainBundle] pathForResource:@"tongue" ofType:@"bundle"];
            [FUAIKit loadTongueMode:path];
            
            /* 设置嘴巴灵活度 默认= 0*/ //
            float flexible = 0.5;
            [FUAIKit setFaceTrackParam:@"mouth_expression_more_flexible" value:flexible];
            
            // 设置人脸算法质量
            [FUAIKit shareKit].faceProcessorFaceLandmarkQuality = [FURenderKit devicePerformanceLevel] == FUDevicePerformanceLevelHigh ? FUFaceProcessorFaceLandmarkQualityHigh : FUFaceProcessorFaceLandmarkQualityMedium;
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
        NSBundle *bundle = [BundleUtil bundleWithBundleName:@"FURenderKit" podName:@"fuLib"];
        NSString *beautyPath = [bundle pathForResource:@"graphics/face_beautification" ofType:@"bundle"];
        FUBeauty *beauty = [[FUBeauty alloc] initWithPath:beautyPath name:@"FUBeauty"];
        // 默认均匀磨皮
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
        NSBundle *bundle = [BundleUtil bundleWithBundleName:@"FURenderKit" podName:@"fuLib"];
        NSString *beautyPath = [bundle pathForResource:@"graphics/face_makeup" ofType:@"bundle"];
        FUMakeup *makeup = [[FUMakeup alloc] initWithPath:beautyPath name:@"face_makeup"];
        makeup.isMakeupOn = YES;
        [FURenderKit setLogLevel:FU_LOG_LEVEL_DEBUG];
        
        [FURenderKit shareRenderKit].makeup = makeup;
        [FURenderKit shareRenderKit].makeup.enable = isSelected;
        
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
        NSBundle *bundle = [BundleUtil bundleWithBundleName:@"FURenderKit" podName:@"fuLib"];
        NSString *beautyPath = [bundle pathForResource:@"graphics/face_beautification" ofType:@"bundle"];
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
        // 根据人脸置信度设置不同磨皮效果
        CGFloat score = [FUAIKit fuFaceProcessorGetConfidenceScore:0];
        if (score > 0.95) {
            [FURenderKit shareRenderKit].beauty.blurType = 3;
            [FURenderKit shareRenderKit].beauty.blurUseMask = YES;
        } else {
            [FURenderKit shareRenderKit].beauty.blurType = 2;
            [FURenderKit shareRenderKit].beauty.blurUseMask = NO;
        }
    } else {
        // 设置精细磨皮效果
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
    FURenderInput *input = [[FURenderInput alloc] init];
    input.pixelBuffer = frame;
    //默认图片内部的人脸始终是朝上，旋转屏幕也无需修改该属性。
    input.renderConfig.imageOrientation = FUImageOrientationUP;
    //开启重力感应，内部会自动计算正确方向，设置fuSetDefaultRotationMode，无须外面设置
    input.renderConfig.gravityEnable = YES;
    //如果来源相机捕获的图片一定要设置，否则将会导致内部检测异常
    input.renderConfig.isFromFrontCamera = YES;
    //该属性是指系统相机是否做了镜像: 一般情况前置摄像头出来的帧都是设置过镜像，所以默认需要设置下。如果相机属性未设置镜像，改属性不用设置。
    input.renderConfig.isFromMirroredCamera = YES;
    FURenderOutput *output = [[FURenderKit shareRenderKit] renderWithInput:input];
    return output.pixelBuffer;
#else
    return frame;
#endif
}

@end
