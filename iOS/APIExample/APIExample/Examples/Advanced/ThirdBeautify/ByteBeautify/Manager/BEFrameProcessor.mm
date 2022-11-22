// Copyright (C) 2018 Beijing Bytedance Network Technology Co., Ltd.
#import "BEFrameProcessor.h"

#import <OpenGLES/ES2/glext.h>
#if __has_include("bef_effect_ai_api.h")
#import "RenderMsgDelegate.h"
#endif

#import "BERender.h"
#import "BEEffectManager.h"
#import "BEResourceHelper.h"
#import "BEEffectResourceHelper.h"


@implementation BEProcessResultBuffer
@end

@implementation BEProcessResult
@end

#if __has_include("bef_effect_ai_api.h")
@interface BEFrameProcessor() <RenderMsgDelegate> {
    
    EAGLContext *_glContext;
    
    BOOL                    _effectOn;
    BEEffectManager         *_effectManager;
    BERender                *_render;
    BEResourceHelper        *_resourceHelper;
    IRenderMsgDelegateManager *_manager;
    BEFormatType            _inputFormat;
    
    BOOL                    _shouldResetComposer;
#if __has_include("bef_effect_ai_api.h")
    bef_ai_face_info        *_faceInfo;
    bef_ai_hand_info        *_handInfo;
    bef_ai_skeleton_result  *_skeletonInfo;
#endif
}

@end
#endif

@implementation BEFrameProcessor

/**
 * license有效时间2019-03-01到2019-04-30
 * license只是为了追踪使用情况，可以随时申请无任何限制license
 */

- (instancetype)initWithContext:(EAGLContext *)context resourceDelegate:(id<BEResourceHelperDelegate>)delegate {
    self = [super init];
#if __has_include("bef_effect_ai_api.h")
    if (self) {
        _glContext = context;
        [EAGLContext setCurrentContext:context];
        
        _effectOn = YES;
        _shouldResetComposer = YES;
        _pixelBufferAccelerate = YES;
        _processorResult = BECVPixelBuffer;
        _faceInfo = NULL;
        _handInfo = NULL;
        _skeletonInfo = NULL;
        BEEffectResourceHelper *resourceHelper = [BEEffectResourceHelper new];
        _effectManager = [[BEEffectManager alloc] initWithResourceProvider:resourceHelper licenseProvider:[BELicenseHelper shareInstance]];
        int ret = [_effectManager initTask];
        NSLog(@"ret == %d", ret);
        if (ret == BEF_RESULT_SUC) {
            [self setEffectOn:true];
        }
        _render = [[BERender alloc] init];
        _resourceHelper = [[BEResourceHelper alloc] init];
        _resourceHelper.delegate = delegate;
        self.usePipeline = YES;
    }
#endif
    return self;
}

- (void)dealloc {
    NSLog(@"BEFrameProcessor dealloc %@", NSStringFromSelector(_cmd));
#if __has_include("bef_effect_ai_api.h")
    free(_faceInfo);
    free(_handInfo);
    free(_skeletonInfo);
    [EAGLContext setCurrentContext:_glContext];
#endif
    [self be_releaseSDK];
}

/*
 * 帧处理流程
 */
- (BEProcessResult *)process:(CVPixelBufferRef)pixelBuffer timeStamp:(double)timeStamp{
    CVPixelBufferLockBaseAddress(pixelBuffer, 0);
#if __has_include("bef_effect_ai_api.h")
    BEPixelBufferInfo *info = [_render getCVPixelBufferInfo:pixelBuffer];
    if (info.format == BE_UNKNOW) {
        NSLog(@"unknow pixelBuffer format, use format show in BEFormatType...");
        return nil;
    }
    _inputFormat = info.format;
#endif
#if __has_include("bef_effect_ai_api.h")
    // 设置 OpenGL 环境 , 需要与初始化 SDK 时一致
    if ([EAGLContext currentContext] != _glContext) {
        [EAGLContext setCurrentContext:_glContext];
    }
#endif
    
    BEProcessResult *result;
    if (_pixelBufferAccelerate) {
#if __has_include("bef_effect_ai_api.h")
        GLuint inputTexture = [_render transforCVPixelBufferToTexture:pixelBuffer];
        [_render initOutputTextureAndCVPixelBufferWithWidth:info.width height:info.height format:info.format];
        result = [self process:inputTexture width:info.width height:info.height timeStamp:timeStamp fromPixelBuffer:YES];
#endif
    } else {
#if __has_include("bef_effect_ai_api.h")
        int bytesPerRow = info.width * 4;
        unsigned char *baseAddress = [_render transforCVPixelBufferToBuffer:pixelBuffer outputFormat:info.format];
        if (baseAddress == nil) {
            CVPixelBufferUnlockBaseAddress(pixelBuffer, 0);
            return nil;
        }
        result = [self process:baseAddress width:info.width height:info.height bytesPerRow:bytesPerRow timeStamp:timeStamp format:info.format fromPixelBuffer:YES];
#endif
    }
    
    if ((_processorResult & BECVPixelBuffer)) {
        if (_pixelBufferAccelerate) {
#if __has_include("bef_effect_ai_api.h")
            result.pixelBuffer = [_render getOutputPixelBuffer];
#endif
        } else {
            BEProcessResultBuffer *buffer = result.buffer;
            if (buffer) {
#if __has_include("bef_effect_ai_api.h")
                result.pixelBuffer = [_render transforBufferToCVPixelBuffer:buffer.buffer pixelBuffer:pixelBuffer width:buffer.width height:buffer.height bytesPerRow:buffer.bytesPerRow inputFormat:buffer.format outputFormat:[self be_outputFormat]];
#endif
            }
        }
    }
    
    CVPixelBufferUnlockBaseAddress(pixelBuffer, 0);
    return result;
}

- (BEProcessResult *)process:(unsigned char *)buffer width:(int)width height:(int)height bytesPerRow:(int)bytesPerRow timeStamp:(double)timeStamp format:(BEFormatType)format {
    // 设置 OpenGL 环境 , 需要与初始化 SDK 时一致
#if __has_include("bef_effect_ai_api.h")
    if ([EAGLContext currentContext] != _glContext) {
        [EAGLContext setCurrentContext:_glContext];
    }
    _inputFormat = format;
#endif
    return [self process:buffer width:width height:height bytesPerRow:bytesPerRow timeStamp:timeStamp format:format fromPixelBuffer:NO];
}

- (BEProcessResult *)process:(unsigned char *)buffer width:(int)width height:(int)height bytesPerRow:(int)bytesPerRow timeStamp:(double)timeStamp format:(BEFormatType)format fromPixelBuffer:(BOOL)fromPixelBuffer {
#if __has_include("bef_effect_ai_api.h")
    // transfor buffer to texture
    GLuint inputTexture = [_render transforBufferToTexture:buffer width:width height:height bytesPerRow:bytesPerRow inputFormat:format];
    
    return [self process:inputTexture width:width height:height timeStamp:timeStamp fromPixelBuffer:fromPixelBuffer];
#else
    return nil;
#endif
}

- (BEProcessResult *)process:(GLuint)texture width:(int)width height:(int)height timeStamp:(double)timeStamp {
    // 设置 OpenGL 环境 , 需要与初始化 SDK 时一致
#if __has_include("bef_effect_ai_api.h")
    if ([EAGLContext currentContext] != _glContext) {
        [EAGLContext setCurrentContext:_glContext];
    }
    
    _inputFormat = BE_RGBA;
    return [self process:texture width:width height:height timeStamp:timeStamp fromPixelBuffer:NO];
#else
    return nil;
#endif
}

- (BEProcessResult *)process:(GLuint)texture width:(int)width height:(int)height timeStamp:(double)timeStamp fromPixelBuffer:(BOOL)fromPixelBuffer {
    //设置后续美颜以及其他识别功能的基本参数
    //    [_effectManager setWidth:width height:height orientation:[self getDeviceOrientation]];
#if __has_include("bef_effect_ai_api.h")
    GLuint textureResult;
    if (_effectOn) {
        GLuint outputTexutre = [_render getOutputTexture:width height:height];
        textureResult = [_effectManager processTexture:texture outputTexture:outputTexutre width:width height:height rotate:BEF_AI_CLOCKWISE_ROTATE_0 timeStamp:timeStamp];
    } else {
        textureResult = texture;
    }
    
    // transfor texture to buffer/CVPxielbuffer/UIImage with format be_outputFormat
    BEProcessResult *result = [self be_transforTextureToResult:textureResult width:width height:height fromPixelBuffer:fromPixelBuffer];
    
    // check and capture current frame, for taking photo
    [self be_checkAndCaptureFrame:result];
    
    return result;
#else
    return nil;
#endif
}

/*
 * 设置滤镜强度
 */
-(void)setFilterIntensity:(float)intensity{
#if __has_include("bef_effect_ai_api.h")
    [_effectManager setFilterIntensity:intensity];
#endif
}

/*
 * 设置贴纸资源
 */
- (void)setStickerPath:(NSString *)path {
#if __has_include("bef_effect_ai_api.h")
    if (path != nil && ![path isEqualToString:@""]) {
        _shouldResetComposer = true;
        path = [_resourceHelper stickerPath:path];
    }
    [_effectManager setStickerPath:path];
#endif
}

- (void)setComposerMode:(int)mode {
    _composerMode = mode;
    //    [_effectManager setComposerMode:mode];
}

- (void)updateComposerNodes:(NSArray<NSString *> *)nodes {
    [self be_checkAndSetComposer];
    
#if __has_include("bef_effect_ai_api.h")
    NSMutableArray<NSString *> *paths = [NSMutableArray arrayWithCapacity:nodes.count];
    for (int i = 0; i < nodes.count; i++) {
        NSString *path = [_resourceHelper composerNodePath:nodes[i]];
        if (path) {
            [paths addObject:path];
        }
    }
    [_effectManager updateComposerNodes:paths];
#endif
}

- (void)updateComposerNodeIntensity:(NSString *)node key:(NSString *)key intensity:(CGFloat)intensity {
#if __has_include("bef_effect_ai_api.h")
    [_effectManager updateComposerNodeIntensity:[_resourceHelper composerNodePath:node] key:key intensity:intensity];
#endif
}

/*
 * 设置滤镜资源路径和系数
 */
- (void)setFilterPath:(NSString *)path {
#if __has_include("bef_effect_ai_api.h")
    if (path != nil && ![path isEqualToString:@""]) {
        path = [_resourceHelper filterPath:path];
    }
    [_effectManager setFilterPath:path];
#endif
}

- (void)setEffectOn:(BOOL)on
{
#if __has_include("bef_effect_ai_api.h")
    _effectOn = on;
#endif
}

- (NSArray<NSString *> *)availableFeatures {
#if __has_include("bef_effect_ai_api.h")
    return [_effectManager availableFeatures];
#else
    return nil;
#endif
}

- (NSString *)sdkVersion {
#if __has_include("bef_effect_ai_api.h")
    return [_effectManager sdkVersion];
#else
    return nil;
#endif
}

- (BOOL)setCameraPosition:(BOOL)isFront {
#if __has_include("bef_effect_ai_api.h")
    [_effectManager setFrontCamera:isFront];
#endif
    return YES;
}

- (BOOL)setImageMode:(BOOL)imageMode {
    //    return [_effectManager setImageMode:imageMode];
    return YES;
}

- (BOOL)processTouchEvent:(float)x y:(float)y {
    //    return [_effectManager processTouchEvent:x y:y];
    return YES;
}

#if __has_include("bef_effect_ai_api.h")
- (bef_ai_face_info *)getFaceInfo {
    return [_effectManager getFaceInfo];
}

- (bef_ai_hand_info *)getHandInfo {
    return [_effectManager getHandInfo];
}

- (bef_ai_skeleton_result *)getSkeletonInfo {
    return [_effectManager getSkeletonInfo];
}
#endif

#pragma mark - RenderMsgDelegate
- (BOOL)msgProc:(unsigned int)unMsgID arg1:(int)nArg1 arg2:(int)nArg2 arg3:(const char *)cArg3 {
#ifdef DEBUG_LOG
    NSLog(@"msg proc: %d, arg: %d in processor: %lu", unMsgID, nArg1, self.hash);
#endif
    return NO;
}

#pragma mark - setter
- (void)setUsePipeline:(BOOL)usePipeline {
    _usePipeline = usePipeline;
#if __has_include("bef_effect_ai_api.h")
    if (_effectManager != nil) {
        [_effectManager setUsePipeline:usePipeline];
    }
    if (_render != nil) {
        _render.useCacheTexture = usePipeline;
    }
#endif
}

#pragma mark - private

- (void)be_releaseSDK {
    // 要在opengl上下文中调用
#if __has_include("bef_effect_ai_api.h")
    [_effectManager destroyTask];
#endif
}

- (void)be_checkAndSetComposer {
    if ([self be_shouldResetComposer]) {
        //        [_effectManager initEffectCompose:[_resourceHelper composerPath]];
#if __has_include("bef_effect_ai_api.h")
        _shouldResetComposer = false;
#endif
    }
}

- (BOOL)be_shouldResetComposer {
#if __has_include("bef_effect_ai_api.h")
    return _shouldResetComposer && _composerMode == 0;
#else
    return NO;
#endif
}

- (BEFormatType)be_outputFormat {
    if (_outputFormat) {
        return _outputFormat;
    }
#if __has_include("bef_effect_ai_api.h")
    return _inputFormat;
#else
    return _outputFormat;
#endif
}

- (BEProcessResult *)be_transforTextureToResult:(GLuint)texture  width:(int)width height:(int)height fromPixelBuffer:(BOOL)fromPixelBuffer {
    BEProcessResult *result = [BEProcessResult new];
    result.texture = texture;
    result.size = CGSizeMake(width, height);
    
    BEProcessResultBuffer *buffer;
    if (_processorResult & (BERawData | (BECVPixelBuffer & !_pixelBufferAccelerate) | BEImage)) {
        buffer = [BEProcessResultBuffer new];
        buffer.format = [self be_outputFormat];
        buffer.width = width;
        buffer.height = height;
        int bytesPerRow = 0;
#if __has_include("bef_effect_ai_api.h")
        buffer.buffer = [_render transforTextureToBuffer:texture width:width height:height outputFormat:[self be_outputFormat] bytesPerRowPointer:&bytesPerRow];
#endif
        buffer.bytesPerRow = bytesPerRow;
        result.buffer = buffer;
    }
    if (!fromPixelBuffer && (_processorResult & BECVPixelBuffer)) {
        if (buffer) {
#if __has_include("bef_effect_ai_api.h")
            result.pixelBuffer = [_render transforBufferToCVPixelBuffer:buffer.buffer width:buffer.width height:buffer.height bytesPerRow:buffer.bytesPerRow inputFormat:buffer.format outputFormat:[self be_outputFormat]];
#endif
        } else {
            NSLog(@"getCVPixelBuffer error: no buffer");
        }
    }
    if ((_processorResult & BEImage)) {
        if (buffer) {
#if __has_include("bef_effect_ai_api.h")
            result.image = [_render transforBufferToUIImage:buffer.buffer
                                                      width:buffer.width
                                                     height:buffer.height
                                                bytesPerRow:buffer.bytesPerRow
                                                inputFormat:buffer.format];
#endif
        } else {
            NSLog(@"getImage error: no buffer");
        }
    }
    return result;
}

- (void)be_checkAndCaptureFrame:(BEProcessResult *)result {
    if (_captureNextFrame) {
        
        int width = result.size.width;
        int height = result.size.height;
        UIImage *image;
        if (result.image) {
            image = result.image;
        } else if (result.buffer) {
            
            BEProcessResultBuffer *buffer = result.buffer;
#if __has_include("bef_effect_ai_api.h")
            image = [_render transforBufferToUIImage:buffer.buffer
                                               width:buffer.width
                                              height:buffer.height
                                         bytesPerRow:buffer.bytesPerRow
                                         inputFormat:buffer.format];
#endif
        } else {
            
            int bytesPerRow;
            BEFormatType format = BE_RGBA;
#if __has_include("bef_effect_ai_api.h")
            unsigned char *buffer = [_render transforTextureToBuffer:result.texture
                                                               width:width
                                                              height:height
                                                        outputFormat:format
                                                  bytesPerRowPointer:&bytesPerRow];
            image = [_render transforBufferToUIImage:buffer
                                               width:width
                                              height:height
                                         bytesPerRow:bytesPerRow
                                         inputFormat:format];
#endif
        }
        if (self.captureDelegate) {
            if (image) {
                [self.captureDelegate onImageCapture:image];
            } else {
                NSLog(@"captureNextFrame error: no image");
            }
        }
        _captureNextFrame = NO;
    }
}

/*
 * 获取设备旋转角度
 */
- (int)getDeviceOrientation {
#if __has_include("bef_effect_ai_api.h")
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
#else
    return -1;
#endif
}

@end

