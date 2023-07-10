//
//  FUBeautyRender.m
//  BeautyAPi
//
//  Created by zhaoyongqiang on 2023/6/30.
//

#import "FUBeautyRender.h"
#import "BundleUtil.h"

@interface FUBeautyRender ()

#if __has_include(FURenderMoudle)
/// 当前的贴纸
@property (nonatomic, strong) FUSticker *currentSticker;
#endif

@end

@implementation FUBeautyRender

#if __has_include(FURenderMoudle)
- (FUManager *)fuManager {
    if (_fuManager == nil) {
        _fuManager = [[FUManager alloc] init];
    }
    return _fuManager;
}
#endif

- (void)destory {
#if __has_include(FURenderMoudle)
    [FURenderKit shareRenderKit].beauty = nil;
    [FURenderKit shareRenderKit].makeup = nil;
    [[FURenderKit shareRenderKit].stickerContainer removeAllSticks];
    [FURenderKit destroy];
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
- (void)reset {
#if __has_include(FURenderMoudle)
    [FURenderKit shareRenderKit].beauty = nil;
#endif
}

- (void)setBeautyPreset {
#if __has_include(FURenderMoudle)
    NSBundle *bundle = [BundleUtil bundleWithBundleName:@"FURenderKit" podName:@"fuLib"];
    NSString *faceAIPath = [bundle pathForResource:@"graphics/face_beautification" ofType:@"bundle"];
    FUBeauty *beauty = [[FUBeauty alloc] initWithPath:faceAIPath name:@"FUBeauty"];
    [FURenderKit shareRenderKit].beauty = beauty;
#endif
}

- (void)setMakeup:(BOOL)isSelected {
#if __has_include(FURenderMoudle)
    if (isSelected) {
        NSBundle *bundle = [BundleUtil bundleWithBundleName:@"FURenderKit" podName:@"fuLib"];
        NSString *makeupPath = [bundle pathForResource:@"graphics/face_makeup" ofType:@"bundle"];
        FUMakeup *makeup = [[FUMakeup alloc] initWithPath:makeupPath name:@"face_makeup"];
        NSString *path = [bundle pathForResource:@"美妆/ziyun" ofType:@"bundle"];
        FUItem *makupItem = [[FUItem alloc] initWithPath:path name:@"ziyun"];
        makeup.isMakeupOn = YES;
        [FURenderKit shareRenderKit].makeup = makeup;
        [FURenderKit shareRenderKit].makeup.enable = YES;
        [makeup updateMakeupPackage:makupItem needCleanSubItem:NO];
        makeup.intensity = 0.7;
    } else {
        [FURenderKit shareRenderKit].makeup.enable = NO;
        [FURenderKit shareRenderKit].makeup = nil;
    }
#endif
}

- (void)setSticker:(BOOL)isSelected {
#if __has_include(FURenderMoudle)
    if (isSelected) {
        NSBundle *bundle = [BundleUtil bundleWithBundleName:@"FURenderKit" podName:@"fuLib"];
        NSString *path = [bundle pathForResource:[NSString stringWithFormat:@"贴纸/%@", @"fu_zh_fenshu"] ofType:@"bundle"];
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
