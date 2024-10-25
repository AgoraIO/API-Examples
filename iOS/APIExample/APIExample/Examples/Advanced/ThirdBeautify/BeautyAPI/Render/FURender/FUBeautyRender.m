//
//  FUBeautyRender.m
//  BeautyAPi
//
//  Created by zhaoyongqiang on 2023/6/30.
//

#import "FUBeautyRender.h"
#import "BundleUtil.h"
#if __has_include("FUManager.h")
#import "authpack.h"
#endif

@interface FUBeautyRender ()

#if __has_include(FURenderMoudle)
/// 当前的贴纸
@property (nonatomic, strong) FUSticker *currentSticker;
@property (nonatomic, strong) FUAnimoji *currentAnimoji;
#endif
@property (nonatomic, copy) NSString *makeupKey;

@end

@implementation FUBeautyRender

- (instancetype)init {
    if (self == [super init]) {
#if __has_include("FUManager.h")
        self.fuManager = [[FUManager alloc] init];
#endif
    }
    return self;
}

+ (BOOL)checkLicense {
    BOOL success = NO;
#if __has_include(FURenderMoudle)
    FUSetupConfig *setupConfig = [[FUSetupConfig alloc] init];
    setupConfig.authPack = FUAuthPackMake(g_auth_package, sizeof(g_auth_package));
    
    // 初始化 FURenderKit
    success = [FURenderKit setupWithSetupConfig:setupConfig];
#endif
    return success;
}

- (void)destroy {
#if __has_include(FURenderMoudle)
    dispatch_queue_t referQueue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_LOW, 0);
    dispatch_async(referQueue, ^{
        [FURenderKit shareRenderKit].beauty = nil;
        [FURenderKit shareRenderKit].makeup = nil;
        [[FURenderKit shareRenderKit].stickerContainer removeAllSticks];
        [FURenderKit destroy];
    });
    _fuManager = nil;
#endif
}

- (nonnull CVPixelBufferRef)onCapture:(nonnull CVPixelBufferRef)pixelBuffer {
#if __has_include(FURenderMoudle)
    return [self.fuManager processFrame:pixelBuffer];
#endif
    return pixelBuffer;
}
#if __has_include(<AgoraRtcKit/AgoraRtcKit.h>)
- (AgoraVideoFormat)getVideoFormatPreference {
    return AgoraVideoFormatCVPixelNV12;
}
#endif

- (void)setBeautyWithPath:(NSString *)path key:(NSString *)key value:(float)value {
#if __has_include(FURenderMoudle)
    FUBeauty *beauty = [FURenderKit shareRenderKit].beauty;
    if (beauty == nil) {
        NSString *faceAIPath = [[NSBundle mainBundle] pathForResource:[NSString stringWithFormat:@"%@", path] ofType:@"bundle"];
        beauty = [[FUBeauty alloc] initWithPath:faceAIPath name:@"FUBeauty"];
        beauty.heavyBlur = 0;
    }
    if ([key isEqualToString:@"blurLevel"]) {
        beauty.blurLevel = value * 6.0;
    } else if ([key isEqualToString:@"whiten"]) {
        beauty.colorLevel = value;
    } else if ([key isEqualToString:@"thin"]) {
        beauty.cheekThinning = value;
    } else if ([key isEqualToString:@"rosy"]) {
        beauty.redLevel = value;
    } else if ([key isEqualToString:@"contouring"]) {
        beauty.faceThreed = value;
    } else if ([key isEqualToString:@"cheekNarrow"]) {
        beauty.cheekNarrow = value;
    } else if ([key isEqualToString:@"cheekShort"]) {
        beauty.cheekShort = value;
    } else if ([key isEqualToString:@"cheekSmall"]) {
        beauty.cheekSmall = value;
    } else if ([key isEqualToString:@"cheek"]) {
        beauty.intensityCheekbones = value;
    } else if ([key isEqualToString:@"cheekV"]) {
        beauty.cheekV = value;
    }  else if ([key isEqualToString:@"chin"]) {
        beauty.intensityChin = value;
    } else if ([key isEqualToString:@"forehead"]) {
        beauty.intensityForehead = value;
    } else if ([key isEqualToString:@"enlarge"]) {
        beauty.eyeEnlarging = value;
    } else if ([key isEqualToString:@"eyeBright"]) {
        beauty.eyeBright = value;
    } else if ([key isEqualToString:@"eyeCircle"]) {
        beauty.intensityEyeCircle = value;
    } else if ([key isEqualToString:@"eyeSpace"]) {
        beauty.intensityEyeSpace = value;
    } else if ([key isEqualToString:@"eyeLid"]) {
        beauty.intensityEyeLid = value;
    } else if ([key isEqualToString:@"pouchStrength"]) {
        beauty.removePouchStrength = value;
    } else if ([key isEqualToString:@"browHeight"]) {
        beauty.intensityBrowHeight = value;
    } else if ([key isEqualToString:@"browThick"]) {
        beauty.intensityBrowThick = value;
    } else if ([key isEqualToString:@"nose"]) {
        beauty.intensityNose = value;
    } else if ([key isEqualToString:@"wrinkles"]) {
        beauty.removeNasolabialFoldsStrength = value;
    } else if ([key isEqualToString:@"philtrum"]) {
        beauty.intensityPhiltrum = value;
    } else if ([key isEqualToString:@"longNose"]) {
        beauty.intensityLongNose = value;
    } else if ([key isEqualToString:@"lowerJaw"]) {
        beauty.intensityLowerJaw = value;
    } else if ([key isEqualToString:@"mouth"]) {
        beauty.intensityMouth = value;
    } else if ([key isEqualToString:@"lipThick"]) {
        beauty.intensityLipThick = value;
    } else if ([key isEqualToString:@"intensityEyeHeight"]) {
        beauty.intensityEyeHeight = value;
    } else if ([key isEqualToString:@"intensityCanthus"]) {
        beauty.intensityCanthus = value;
    } else if ([key isEqualToString:@"toothWhiten"]) {
        beauty.toothWhiten = value;
    } else if ([key isEqualToString:@"intensityEyeRotate"]) {
        beauty.intensityEyeRotate = value;
    } else if ([key isEqualToString:@"intensitySmile"]) {
        beauty.intensitySmile = value;
    } else if ([key isEqualToString:@"intensityBrowSpace"]) {
        beauty.intensityBrowSpace = value;
    } else if ([key isEqualToString:@"sharpen"]) {
        beauty.sharpen = value;
    }
    beauty.enable = YES;
    [FURenderKit shareRenderKit].beauty = beauty;
#endif
}

- (void)setStyleWithPath:(NSString *)path key:(NSString *)key value:(float)value isCombined:(BOOL)isCombined {
#if __has_include(FURenderMoudle)
    [self setupBeauty];
    FUMakeup *makeup = [FURenderKit shareRenderKit].makeup;
    if (isCombined) {
        if (makeup == nil || self.makeupKey != key) {
            NSBundle *bundle = [BundleUtil bundleWithBundleName:@"FURenderKit" podName:@"fuLib"];
            NSString *stylePath = [bundle pathForResource:key ofType:@"bundle"];
            makeup = [[FUMakeup alloc] initWithPath:stylePath name:@"makeup"];
            makeup.isMakeupOn = YES;
            dispatch_queue_t referQueue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_LOW, 0);
            dispatch_async(referQueue, ^{
                [FURenderKit shareRenderKit].makeup = makeup;
                [FURenderKit shareRenderKit].makeup.intensity = value;
                [FURenderKit shareRenderKit].makeup.enable = YES;
            });
        }
        [FURenderKit shareRenderKit].makeup.intensity = value;
        self.makeupKey = key;
    } else {
        NSString *makeupPath = [[NSBundle mainBundle] pathForResource:path ofType:@"bundle"];
        if (makeup == nil || self.makeupKey != path) {
            makeup = [[FUMakeup alloc] initWithPath:makeupPath name:@"face_makeup"];
            makeup.isMakeupOn = YES;
            [FURenderKit shareRenderKit].makeup = makeup;
            [FURenderKit shareRenderKit].makeup.enable = YES;
        }
        NSBundle *bundle = [BundleUtil bundleWithBundleName:@"FURenderKit" podName:@"fuLib"];
        NSString *stylePath = [bundle pathForResource:key ofType:@"bundle"];
        FUItem *makupItem = [[FUItem alloc] initWithPath:stylePath name:key];
        [makeup updateMakeupPackage:makupItem needCleanSubItem:NO];
        makeup.intensity = value;
        self.makeupKey = path;
    }
    NSBundle *bundle = [BundleUtil bundleWithBundleName:@"FURenderKit" podName:@"fuLib"];
    NSString *stylePath = [bundle pathForResource:key ofType:@"bundle"];
    FUItem *makupItem = [[FUItem alloc] initWithPath:stylePath name:key];
    [makeup updateMakeupPackage:makupItem needCleanSubItem:NO];
    makeup.intensity = value;
#endif
}

- (void)setAnimojiWithPath:(NSString *)path {
#if __has_include(FURenderMoudle)
    [self setupBeauty];
    if (self.currentSticker) {
        [[FURenderKit shareRenderKit].stickerContainer removeSticker:self.currentSticker completion:nil];
        self.currentSticker = nil;
    }
    NSBundle *bundle = [BundleUtil bundleWithBundleName:@"FURenderKit" podName:@"fuLib"];
    NSString *makeupPath = [bundle pathForResource:[NSString stringWithFormat:@"Animoji/%@",path] ofType:@"bundle"];
    FUAnimoji *animoji = [[FUAnimoji alloc] initWithPath:makeupPath name:@"animoji"];
    if (self.currentAnimoji) {
        [[FURenderKit shareRenderKit].stickerContainer replaceSticker:self.currentAnimoji withSticker:animoji completion:^{
            self.currentAnimoji = animoji;
        }];
    } else {
        [[FURenderKit shareRenderKit].stickerContainer addSticker:animoji completion:^{
            self.currentAnimoji = animoji;
        }];
    }
#endif
}

- (void)setStickerWithPath:(NSString *)path {
#if __has_include(FURenderMoudle)
    [self setupBeauty];
    NSBundle *bundle = [BundleUtil bundleWithBundleName:@"FURenderKit" podName:@"fuLib"];
    NSString *stickerPath = [bundle pathForResource:[NSString stringWithFormat:@"sticker/%@", path] ofType:@"bundle"];
    if (stickerPath == nil && self.currentSticker == nil) {
        return;
    }
    FUSticker *sticker = [[FUSticker alloc] initWithPath:stickerPath
                          name:path];
    if (self.currentAnimoji) {
        [[FURenderKit shareRenderKit].stickerContainer removeSticker:self.currentAnimoji completion:nil];
        self.currentAnimoji = nil;
    }
    if (self.currentSticker) {
        [[FURenderKit shareRenderKit].stickerContainer replaceSticker:self.currentSticker withSticker:sticker completion:nil];
    } else {
        [[FURenderKit shareRenderKit].stickerContainer addSticker:sticker completion:nil];
    }
    self.currentSticker = sticker;
#endif
}

- (void)reset {
#if __has_include(FURenderMoudle)
    [FURenderKit shareRenderKit].beauty.enable = NO;
#endif
}

- (void)resetStyle {
#if __has_include(FURenderMoudle)
    [FURenderKit shareRenderKit].makeup.enable = NO;
#endif
    self.makeupKey = nil;
}

- (void)resetSticker {
#if __has_include(FURenderMoudle)
    [[FURenderKit shareRenderKit].stickerContainer removeAllSticks];
    self.currentAnimoji = nil;
    self.currentSticker = nil;
#endif
}

- (void)setBeautyPreset {
#if __has_include(FURenderMoudle)
    dispatch_queue_t referQueue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_LOW, 0);
    dispatch_async(referQueue, ^{
        NSString *faceAIPath = [[NSBundle mainBundle] pathForResource:@"face_beautification" ofType:@"bundle"];
        FUBeauty *beauty = [[FUBeauty alloc] initWithPath:faceAIPath name:@"FUBeauty"];
        [FURenderKit shareRenderKit].beauty = beauty;
    });
#endif
}

- (void)setupBeauty {
#if __has_include(FURenderMoudle)
    if ([FURenderKit shareRenderKit].beauty == nil) {
        NSString *faceAIPath = [[NSBundle mainBundle] pathForResource:@"face_beautification" ofType:@"bundle"];
        FUBeauty *beauty = [[FUBeauty alloc] initWithPath:faceAIPath name:@"FUBeauty"];
        [FURenderKit shareRenderKit].beauty = beauty;
    }
#endif
}

- (void)setMakeup:(BOOL)isSelected {
#if __has_include(FURenderMoudle)
    if (isSelected) {
        NSString *makeupPath = [[NSBundle mainBundle] pathForResource:@"face_makeup" ofType:@"bundle"];
        FUMakeup *makeup = [[FUMakeup alloc] initWithPath:makeupPath name:@"face_makeup"];
        NSBundle *bundle = [BundleUtil bundleWithBundleName:@"FURenderKit" podName:@"fuLib"];
        NSString *path = [bundle pathForResource:@"makeup/ziyun" ofType:@"bundle"];
        FUItem *makupItem = [[FUItem alloc] initWithPath:path name:@"ziyun"];
        makeup.isMakeupOn = YES;
        [FURenderKit shareRenderKit].makeup = makeup;
        [FURenderKit shareRenderKit].makeup.enable = YES;
        [makeup updateMakeupPackage:makupItem needCleanSubItem:NO];
        makeup.intensity = 0.7;
    } else {
        [FURenderKit shareRenderKit].makeup.enable = NO;
    }
#endif
}

- (void)setSticker:(BOOL)isSelected {
#if __has_include(FURenderMoudle)
    if (isSelected) {
        NSBundle *bundle = [BundleUtil bundleWithBundleName:@"FURenderKit" podName:@"fuLib"];
        NSString *path = [bundle pathForResource:[NSString stringWithFormat:@"sticker/%@", @"fu_zh_fenshu"] ofType:@"bundle"];
        FUSticker *sticker = [[FUSticker alloc] initWithPath:path name:@"sticker"];
        if (self.currentSticker) {
            [[FURenderKit shareRenderKit].stickerContainer replaceSticker:self.currentSticker withSticker:sticker completion:nil];
        } else {
            [[FURenderKit shareRenderKit].stickerContainer addSticker:sticker completion:nil];
        }
        self.currentSticker = sticker;
    } else {
        [[FURenderKit shareRenderKit].stickerContainer removeAllSticks];
    }
#endif
}

@end
