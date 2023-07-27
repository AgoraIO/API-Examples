//
//  BytesBeautyRender.m
//  BeautyAPi
//
//  Created by zhaoyongqiang on 2023/6/30.
//

#import "BytesBeautyRender.h"

@interface BytesBeautyRender ()

@property (nonatomic, strong) NSMutableArray *bytesNodes;

@end

@implementation BytesBeautyRender

#if __has_include(BytesMoudle)
- (BEFrameProcessor *)frameProcessor {
    if (_frameProcessor == nil) {
        EAGLContext *context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
        [EAGLContext setCurrentContext:context];
        _frameProcessor = [[BEFrameProcessor alloc]initWithContext:context resourceDelegate:nil];
        _frameProcessor.processorResult = BECVPixelBuffer;
        [_frameProcessor setEffectOn:YES];
        [_frameProcessor updateComposerNodes:self.bytesNodes];
    }
    return _frameProcessor;
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

- (void)destory { 
#if __has_include(BytesMoudle)
    _frameProcessor = nil;
    _imageUtils = nil;
#endif
}

- (nonnull CVPixelBufferRef)onCapture:(nonnull CVPixelBufferRef)pixelBuffer { 
#if __has_include(BytesMoudle)
    pixelBuffer = [self.imageUtils transforCVPixelBufferToCVPixelBuffer:pixelBuffer outputFormat:BE_BGRA];
    CVPixelBufferRef px = [self.frameProcessor process: pixelBuffer
                                               timeStamp: [NSDate date].timeIntervalSince1970].pixelBuffer;
    return px;
#endif
    return nil;
}
#if __has_include(<AgoraRtcKit/AgoraRtcKit.h>)
- (AgoraVideoFormat)getVideoFormatPreference {
    return AgoraVideoFormatCVPixelBGRA;
}
#endif

- (void)reset { 
#if __has_include(BytesMoudle)
    [self.frameProcessor updateComposerNodeIntensity:@"/beauty_IOS_lite" key:@"whiten" intensity:0];
    [self.frameProcessor updateComposerNodeIntensity:@"/beauty_IOS_lite" key:@"smooth" intensity:0];
    [self.frameProcessor updateComposerNodeIntensity:@"/reshape_lite" key:@"Internal_Deform_Overall" intensity:0];
    [self.frameProcessor updateComposerNodeIntensity:@"/reshape_lite" key:@"Internal_Deform_Zoom_Cheekbone" intensity:0];
    [self.frameProcessor updateComposerNodeIntensity:@"/reshape_lite" key:@"Internal_Deform_Eye" intensity:0];
    [self.frameProcessor updateComposerNodeIntensity:@"/reshape_lite" key:@"Internal_Deform_Nose" intensity:0];
    [self.frameProcessor updateComposerNodeIntensity:@"/reshape_lite" key:@"Internal_Deform_Chin" intensity:0];
    [self.frameProcessor updateComposerNodeIntensity:@"/reshape_lite" key:@"Internal_Deform_Zoom_Jawbone" intensity:0];
    [self.frameProcessor updateComposerNodeIntensity:@"/reshape_lite" key:@"Internal_Deform_Forehead" intensity:0];
    [self.frameProcessor updateComposerNodeIntensity:@"/reshape_lite" key:@"Internal_Deform_ZoomMouth" intensity:0];
    [self.frameProcessor updateComposerNodeIntensity:@"/beauty_4Items" key:@"BEF_BEAUTY_WHITEN_TEETH" intensity:0];
#endif
}

- (void)setBeautyPreset { 
#if __has_include(BytesMoudle)
    [self.frameProcessor updateComposerNodeIntensity:@"/beauty_IOS_lite" key:@"whiten" intensity:0.2];
    [self.frameProcessor updateComposerNodeIntensity:@"/beauty_IOS_lite" key:@"smooth" intensity:0.3];
    [self.frameProcessor updateComposerNodeIntensity:@"/reshape_lite" key:@"Internal_Deform_Overall" intensity:0.15];
    [self.frameProcessor updateComposerNodeIntensity:@"/reshape_lite" key:@"Internal_Deform_Zoom_Cheekbone" intensity:0.3];
    [self.frameProcessor updateComposerNodeIntensity:@"/reshape_lite" key:@"Internal_Deform_Eye" intensity:0.15];
    [self.frameProcessor updateComposerNodeIntensity:@"/reshape_lite" key:@"Internal_Deform_Nose" intensity:0.15];
    [self.frameProcessor updateComposerNodeIntensity:@"/reshape_lite" key:@"Internal_Deform_Chin" intensity:0.46];
    [self.frameProcessor updateComposerNodeIntensity:@"/reshape_lite" key:@"Internal_Deform_Zoom_Jawbone" intensity:0.46];
    [self.frameProcessor updateComposerNodeIntensity:@"/reshape_lite" key:@"Internal_Deform_Forehead" intensity:0.4];
    [self.frameProcessor updateComposerNodeIntensity:@"/reshape_lite" key:@"Internal_Deform_ZoomMouth" intensity:0.16];
    [self.frameProcessor updateComposerNodeIntensity:@"/beauty_4Items" key:@"BEF_BEAUTY_WHITEN_TEETH" intensity:0.2];
#endif
}

- (void)setMakeup:(BOOL)isSelected { 
#if __has_include(BytesMoudle)
    if (isSelected) {
        if (![self.bytesNodes containsObject:@"/style_makeup/qise"]) {
            [self.bytesNodes addObject:@"/style_makeup/qise"];
            [self.frameProcessor updateComposerNodes:self.bytesNodes];
        }
        [self.frameProcessor updateComposerNodeIntensity:@"/style_makeup/qise" key:@"Makeup_ALL" intensity:0.6];
    } else {
        if ([self.bytesNodes containsObject:@"/style_makeup/qise"]) {
            [self.bytesNodes removeObject:@"/style_makeup/qise"];
            [self.frameProcessor updateComposerNodes:self.bytesNodes];
        }
        [self.frameProcessor updateComposerNodeIntensity:@"/style_makeup/qise" key:@"Makeup_ALL" intensity:0];
    }
#endif
}

- (void)setSticker:(BOOL)isSelected { 
#if __has_include(BytesMoudle)
    if (isSelected) {
        [self.frameProcessor setStickerPath:@"matting_bg"];
    } else {
        [self.frameProcessor setStickerPath:@""];
    }
#endif
}

@end
