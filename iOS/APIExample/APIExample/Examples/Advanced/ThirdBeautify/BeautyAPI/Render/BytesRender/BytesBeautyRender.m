//
//  BytesBeautyRender.m
//  BeautyAPi
//
//  Created by zhaoyongqiang on 2023/6/30.
//

#import "BytesBeautyRender.h"


@interface BytesBeautyRender ()

@property (nonatomic, strong) NSMutableArray *bytesNodes;
@property (nonatomic, weak) BEPixelBufferGLTexture *outTexture;

@end

@implementation BytesBeautyRender

#if __has_include("BEImageUtils.h") && __has_include("BEEffectManager.h")
- (BEEffectManager *)effectManager {
    if (_effectManager == nil) {
        _effectManager = [[BEEffectManager alloc] initWithResourceProvider:[BEEffectResourceHelper new] licenseProvider:[BELicenseHelper shareInstance]];
#if __has_include(BytesMoudle)
        int ret = [_effectManager initTask];
        if (ret == BEF_RESULT_SUC){
            [_effectManager updateComposerNodes:self.bytesNodes];
        }
#endif
    }
    return _effectManager;
}

- (BEImageUtils *)imageUtils {
    if (_imageUtils == nil) {
        _imageUtils = [[BEImageUtils alloc] init];
    }
    return _imageUtils;
}
#endif

- (NSMutableArray *)bytesNodes {
    if (_bytesNodes == nil) {
        _bytesNodes = [[NSMutableArray alloc] initWithArray:@[@"/beauty_IOS_lite", @"/reshape_lite", @"/beauty_4Items"]];
    }
    return _bytesNodes;
}

- (BOOL)checkLicense {
#if __has_include(BytesMoudle)
    return [[BELicenseHelper shareInstance] checkLicense];
#else
    return NO;
#endif
}

- (void)destroy {
#if __has_include(BytesMoudle)
    [_effectManager cleanPipeline];
    [_effectManager destroyTask];
    _effectManager = nil;
    _imageUtils = nil;
    [self.outTexture destroy];
    self.outTexture = nil;
#endif
}

- (nonnull CVPixelBufferRef)onCapture:(nonnull CVPixelBufferRef)pixelBuffer {
#if __has_include(BytesMoudle)
    double timeStamp = [[NSDate date] timeIntervalSince1970];
    BEPixelBufferInfo *pixelBufferInfo = [self.imageUtils getCVPixelBufferInfo:pixelBuffer];
    if (pixelBufferInfo.format != BE_BGRA) {
        pixelBuffer = [self.imageUtils transforCVPixelBufferToCVPixelBuffer:pixelBuffer
                                                               outputFormat:pixelBufferInfo.format];
    }
    
    if ([self getDeviceOrientation] != BEF_AI_CLOCKWISE_ROTATE_0) {
        pixelBuffer = [self.imageUtils rotateCVPixelBuffer:pixelBuffer rotation:BEF_AI_CLOCKWISE_ROTATE_0];
    }
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
    if ([EAGLContext currentContext] != self.effectManager.glContext) {
        [EAGLContext setCurrentContext: self.effectManager.glContext];
    }
#pragma clang diagnostic pop
    id<BEGLTexture> texture = [self.imageUtils transforCVPixelBufferToTexture:pixelBuffer];
    BEPixelBufferGLTexture *outTexture = nil;

    outTexture = [self.imageUtils getOutputPixelBufferGLTextureWithWidth:texture.width
                                                                  height:texture.height
                                                                  format:pixelBufferInfo.format
                                                            withPipeline:self.effectManager.usePipeline];
    self.outTexture = outTexture;
    int ret = [self.effectManager processTexture:texture.texture
                                   outputTexture:outTexture.texture
                                           width:pixelBufferInfo.width
                                          height:pixelBufferInfo.height
                                          rotate:[self getDeviceOrientation]
                                       timeStamp:timeStamp];
    if (ret != BEF_RESULT_SUC) {
        outTexture = texture;
    }
    return [(BEPixelBufferGLTexture *)outTexture pixelBuffer];
#else
    return pixelBuffer;
#endif
}

#if __has_include(BytesMoudle)
- (bef_ai_rotate_type)getDeviceOrientation {
    UIDeviceOrientation orientation = [[UIDevice currentDevice] orientation];
    switch (orientation) {
        case UIDeviceOrientationPortrait:
            return BEF_AI_CLOCKWISE_ROTATE_0;

        case UIDeviceOrientationPortraitUpsideDown:
            return BEF_AI_CLOCKWISE_ROTATE_180;

        case UIDeviceOrientationLandscapeLeft:
            return BEF_AI_CLOCKWISE_ROTATE_270;

        case UIDeviceOrientationLandscapeRight:
            return BEF_AI_CLOCKWISE_ROTATE_90;

        default:
            return BEF_AI_CLOCKWISE_ROTATE_0;
    }
}
#endif

#if __has_include(<AgoraRtcKit/AgoraRtcKit.h>)
- (AgoraVideoFormat)getVideoFormatPreference {
    return AgoraVideoFormatCVPixelBGRA;
}
#endif

- (void)reset {
#if __has_include(BytesMoudle)
    [self.effectManager updateComposerNodeIntensity:@"/beauty_IOS_lite" key:@"whiten" intensity:0];
    [self.effectManager updateComposerNodeIntensity:@"/beauty_IOS_lite" key:@"smooth" intensity:0];
    [self.effectManager updateComposerNodeIntensity:@"/reshape_lite" key:@"Internal_Deform_Overall" intensity:0];
    [self.effectManager updateComposerNodeIntensity:@"/reshape_lite" key:@"Internal_Deform_Zoom_Cheekbone" intensity:0];
    [self.effectManager updateComposerNodeIntensity:@"/reshape_lite" key:@"Internal_Deform_Eye" intensity:0];
    [self.effectManager updateComposerNodeIntensity:@"/reshape_lite" key:@"Internal_Deform_Nose" intensity:0];
    [self.effectManager updateComposerNodeIntensity:@"/reshape_lite" key:@"Internal_Deform_Chin" intensity:0];
    [self.effectManager updateComposerNodeIntensity:@"/reshape_lite" key:@"Internal_Deform_Zoom_Jawbone" intensity:0];
    [self.effectManager updateComposerNodeIntensity:@"/reshape_lite" key:@"Internal_Deform_Forehead" intensity:0];
    [self.effectManager updateComposerNodeIntensity:@"/reshape_lite" key:@"Internal_Deform_ZoomMouth" intensity:0];
    [self.effectManager updateComposerNodeIntensity:@"/beauty_4Items" key:@"BEF_BEAUTY_WHITEN_TEETH" intensity:0];
#endif
}

- (void)setBeautyPreset {
#if __has_include(BytesMoudle)
    [self.effectManager updateComposerNodeIntensity:@"/beauty_IOS_lite" key:@"whiten" intensity:0.2];
    [self.effectManager updateComposerNodeIntensity:@"/beauty_IOS_lite" key:@"smooth" intensity:0.3];
    [self.effectManager updateComposerNodeIntensity:@"/reshape_lite" key:@"Internal_Deform_Overall" intensity:0.15];
    [self.effectManager updateComposerNodeIntensity:@"/reshape_lite" key:@"Internal_Deform_Zoom_Cheekbone" intensity:0.3];
    [self.effectManager updateComposerNodeIntensity:@"/reshape_lite" key:@"Internal_Deform_Eye" intensity:0.15];
    [self.effectManager updateComposerNodeIntensity:@"/reshape_lite" key:@"Internal_Deform_Nose" intensity:0.15];
    [self.effectManager updateComposerNodeIntensity:@"/reshape_lite" key:@"Internal_Deform_Chin" intensity:0.46];
    [self.effectManager updateComposerNodeIntensity:@"/reshape_lite" key:@"Internal_Deform_Zoom_Jawbone" intensity:0.46];
    [self.effectManager updateComposerNodeIntensity:@"/reshape_lite" key:@"Internal_Deform_Forehead" intensity:0.4];
    [self.effectManager updateComposerNodeIntensity:@"/reshape_lite" key:@"Internal_Deform_ZoomMouth" intensity:0.16];
    [self.effectManager updateComposerNodeIntensity:@"/beauty_4Items" key:@"BEF_BEAUTY_WHITEN_TEETH" intensity:0.2];
#endif
}

- (void)setMakeup:(BOOL)isSelected {
#if __has_include(BytesMoudle)
    if (isSelected) {
        if (![self.bytesNodes containsObject:@"/style_makeup/qise"]) {
            [self.bytesNodes addObject:@"/style_makeup/qise"];
            [self.effectManager updateComposerNodes:self.bytesNodes];
        }
        [self.effectManager updateComposerNodeIntensity:@"/style_makeup/qise" key:@"Makeup_ALL" intensity:0.6];
    } else {
        if ([self.bytesNodes containsObject:@"/style_makeup/qise"]) {
            [self.bytesNodes removeObject:@"/style_makeup/qise"];
            [self.effectManager updateComposerNodes:self.bytesNodes];
        }
        [self.effectManager updateComposerNodeIntensity:@"/style_makeup/qise" key:@"Makeup_ALL" intensity:0];
    }
#endif
}

- (void)setSticker:(BOOL)isSelected {
#if __has_include(BytesMoudle)
    if (isSelected) {
        [self.effectManager setStickerPath:@"stickers_zhaocaimao"];
    } else {
        [self.effectManager setStickerPath:@""];
    }
#endif
}

@end
