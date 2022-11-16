//
//  VideoProcessingManager.m
//  BeautifyExample
//
//  Created by zhaoyongqiang on 2022/6/28.
//  Copyright © 2022 Agora. All rights reserved.
//

#import "VideoProcessingManager.h"
#import "EFMotionManager.h"
#import "BundleUtil.h"
#import <GLKit/GLKit.h>

@interface VideoProcessingManager ()
{
    @public
    GLuint _outTexture;
    CVPixelBufferRef _outputPixelBuffer;
    CVOpenGLESTextureRef _outputCVTexture;
    BOOL _isFirstLaunch;
}

@property (nonatomic, strong) EffectsProcess *effectsProcess;
@property (nonatomic, strong) EAGLContext *glContext;
@property (nonatomic) UIDeviceOrientation deviceOrientation;
@property (nonatomic) dispatch_queue_t renderQueue;
///贴纸id
@property (nonatomic, assign) int stickerId;

@end

@implementation VideoProcessingManager

- (instancetype)init {
    if (self = [super init]) {
        self.renderQueue = dispatch_queue_create("com.render.queue", DISPATCH_QUEUE_SERIAL);
        self.glContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES3];
        self.effectsProcess = [[EffectsProcess alloc] initWithType:EffectsTypeVideo glContext:self.glContext];
        //effects
        dispatch_async(self.renderQueue, ^{
#if __has_include("st_mobile_common.h")
            NSBundle *bundle = [BundleUtil bundleWithBundleName:@"SenseLib" podName:@"senseLib"];
            [self.effectsProcess setModelPath:[bundle pathForResource:@"model" ofType:@"bundle"]];
            [EAGLContext setCurrentContext:self.glContext];
            self.effectsProcess.detectConfig = ST_MOBILE_FACE_DETECT;
            [self.effectsProcess setBeautyParam:EFFECT_BEAUTY_PARAM_ENABLE_WHITEN_SKIN_MASK andVal:0.7];
            [self.effectsProcess setEffectType:EFFECT_BEAUTY_RESHAPE_SHRINK_FACE value:0.8];
            [self.effectsProcess setEffectType:EFFECT_BEAUTY_BASE_WHITTEN value:0.6];
            [self.effectsProcess setEffectType:EFFECT_BEAUTY_RESHAPE_ENLARGE_EYE value:1.0];
            [self.effectsProcess setEffectType:EFFECT_BEAUTY_RESHAPE_ROUND_EYE value:1.0];
            [self.effectsProcess setEffectType:EFFECT_BEAUTY_PLASTIC_OPEN_CANTHUS value:0.7];
#endif
        });
    }
    return self;
}

- (CVPixelBufferRef)videoProcessHandler:(CVPixelBufferRef)pixelBuffer {
    if (!pixelBuffer) return pixelBuffer;
    
    if (!self.effectsProcess) {
        return pixelBuffer;
    }
    // 设置 OpenGL 环境 , 需要与初始化 SDK 时一致
    if ([EAGLContext currentContext] != self.glContext) {
        [EAGLContext setCurrentContext:self.glContext];
    }
    
    CVPixelBufferLockBaseAddress(pixelBuffer, 0);
    int width = (int)CVPixelBufferGetWidth(pixelBuffer);
    int heigh = (int)CVPixelBufferGetHeight(pixelBuffer);
    if(!_outTexture){
#if __has_include("st_mobile_common.h")
        [self.effectsProcess createGLObjectWith:width
                                         height:heigh
                                        texture:&_outTexture
                                    pixelBuffer:&_outputPixelBuffer
                                      cvTexture:&_outputCVTexture];
#endif
    }
#if __has_include("st_mobile_common.h")
    st_mobile_human_action_t detectResult;
    memset(&detectResult, 0, sizeof(st_mobile_human_action_t));
    st_result_t ret = [self.effectsProcess detectWithPixelBuffer:pixelBuffer
                                                          rotate:[self getRotateType]
                                                  cameraPosition:AVCaptureDevicePositionFront
                                                     humanAction:&detectResult
                                                    animalResult:nil
                                                     animalCount:nil];
    if (ret != ST_OK) {
        NSLog(@"人脸检测失败");
        CVPixelBufferUnlockBaseAddress(pixelBuffer, 0);
        return pixelBuffer;
    }

    [self.effectsProcess renderPixelBuffer:pixelBuffer
                                    rotate:[self getRotateType]
                               humanAction:detectResult
                              animalResult:nil
                               animalCount:0
                                outTexture:self->_outTexture
                            outPixelFormat:ST_PIX_FMT_BGRA8888
                                   outData:nil];
    
#endif

    CVPixelBufferUnlockBaseAddress(pixelBuffer, 0);
    
    return  self->_outputPixelBuffer;
}

- (void)getDeviceOrientation:(CMAccelerometerData *)accelerometerData {
    if (accelerometerData.acceleration.x >= 0.75) {
        _deviceOrientation = UIDeviceOrientationLandscapeRight;
    } else if (accelerometerData.acceleration.x <= -0.75) {
        _deviceOrientation = UIDeviceOrientationLandscapeLeft;
    } else if (accelerometerData.acceleration.y <= -0.75) {
        _deviceOrientation = UIDeviceOrientationPortrait;
    } else if (accelerometerData.acceleration.y >= 0.75) {
        _deviceOrientation = UIDeviceOrientationPortraitUpsideDown;
    } else {
        _deviceOrientation = UIDeviceOrientationPortrait;
    }
}

#if __has_include("st_mobile_common.h")
- (st_rotate_type)getRotateType{
    BOOL isFrontCamera = YES;//self.camera.devicePosition == AVCaptureDevicePositionFront;
    BOOL isVideoMirrored = YES;//self.camera.videoConnection.isVideoMirrored;
    
    [self getDeviceOrientation:[EFMotionManager sharedInstance].motionManager.accelerometerData];
    
    switch (_deviceOrientation) {
            
        case UIDeviceOrientationPortrait:
            return ST_CLOCKWISE_ROTATE_0;
            
        case UIDeviceOrientationPortraitUpsideDown:
            return ST_CLOCKWISE_ROTATE_180;
            
        case UIDeviceOrientationLandscapeLeft:
            return ((isFrontCamera && isVideoMirrored) || (!isFrontCamera && !isVideoMirrored)) ? ST_CLOCKWISE_ROTATE_270 : ST_CLOCKWISE_ROTATE_90;
            
        case UIDeviceOrientationLandscapeRight:
            return ((isFrontCamera && isVideoMirrored) || (!isFrontCamera && !isVideoMirrored)) ? ST_CLOCKWISE_ROTATE_90 : ST_CLOCKWISE_ROTATE_270;
            
        default:
            return ST_CLOCKWISE_ROTATE_0;
    }
}
#endif

@end
