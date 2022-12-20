//
//  EffectsAnimal.m
//  SenseMeEffects
//
//  Created by sunjian on 2021/7/16.
//  Copyright © 2021 SenseTime. All rights reserved.
//

#import "EffectsAnimal.h"
#if __has_include("st_mobile_effect.h")
#import "st_mobile_effect.h"
#endif
#import <AVFoundation/AVFoundation.h>
#import "BundleUtil.h"

@interface EffectsAnimal ()
{
#if __has_include("st_mobile_common.h")
    st_handle_t _hAnimalHandle;
#endif
}
@end

@implementation EffectsAnimal

- (void)dealloc{
#if __has_include("st_mobile_effect.h")
    if (_hAnimalHandle) {
        st_mobile_tracker_animal_face_destroy(_hAnimalHandle);
        _hAnimalHandle = NULL;
    }
#endif
}

- (instancetype)initWithType:(EffectsType)type{
    if ((self = [super init])) {
        dispatch_async(dispatch_get_global_queue(0, 0), ^{
            [self createHandlerWithType:type];
        });
    }
    return self;
}

- (void)createHandlerWithType:(EffectsType)type{
#if __has_include("st_mobile_common.h")
    NSBundle *bundle = [BundleUtil bundleWithBundleName:@"SenseLib" podName:@"senseLib"];
    NSString *catFaceModelPath = [bundle pathForResource:@"M_SenseME_CatFace_3.0.0" ofType:@"model"];
    NSString *dogFaceModelPath = [bundle pathForResource:@"M_SenseME_DogFace_2.0.0" ofType:@"model"];
    int config = ST_MOBILE_TRACKING_MULTI_THREAD;
    switch (type) {
        case EffectsTypePhoto:
            config = ST_MOBILE_DETECT_MODE_IMAGE;
            break;
            
        case EffectsTypeVideo:
            config = ST_MOBILE_TRACKING_SINGLE_THREAD;
            break;
            
        default:
            break;
    }
    st_result_t ret = st_mobile_tracker_animal_face_create(NULL,
                                                           config,
                                                           &_hAnimalHandle);
    if (ret != ST_OK) {
        NSLog(@"st_mobile_tracker_animal_face_create error %d", ret);
    }
    ret = st_mobile_tracker_animal_face_add_sub_model(_hAnimalHandle, catFaceModelPath.UTF8String);
    if (ret != ST_OK) {
        NSLog(@"st_mobile_tracker_animal_face_add_sub_model %@ error %d", catFaceModelPath, ret);
    }
    ret = st_mobile_tracker_animal_face_add_sub_model(_hAnimalHandle, dogFaceModelPath.UTF8String);
    if (ret != ST_OK) {
        NSLog(@"st_mobile_tracker_animal_face_add_sub_model %@ error %d", dogFaceModelPath, ret);
    }
#endif
}

#if __has_include("st_mobile_effect.h")
- (st_result_t)detectAnimalWithPixelbuffer:(CVPixelBufferRef)pixelBuffer
                                    rotate:(st_rotate_type)rotate
                                    config:(st_mobile_animal_type)config
                              detectResult:(st_mobile_animal_face_t **)detectResult
                               animalCount:(int *)animalCount{
    CVPixelBufferLockBaseAddress(pixelBuffer, 0);
    unsigned char* pixelData = (unsigned char*)CVPixelBufferGetBaseAddress(pixelBuffer);
    int iBytesPerRow = (int)CVPixelBufferGetBytesPerRow(pixelBuffer);
    int iWidth = (int)CVPixelBufferGetWidth(pixelBuffer);
    int iHeight = (int)CVPixelBufferGetHeight(pixelBuffer);
    st_result_t state = [self detectAnimalWithBuffer:pixelData
                                              rotate:rotate
                                         pixelFormat:ST_PIX_FMT_BGRA8888
                                               width:iWidth
                                              height:iHeight
                                              stride:iBytesPerRow
                                              config:config
                                        detectResult:detectResult
                                         animalCount:animalCount];
    CVPixelBufferUnlockBaseAddress(pixelBuffer, 0);
    return state;
}

- (st_result_t)detectAnimalWithBuffer:(unsigned char *)buffer
                               rotate:(st_rotate_type)rotate
                          pixelFormat:(st_pixel_format)pixelFormat
                                width:(int)width
                               height:(int)height
                               stride:(int)stride
                               config:(st_mobile_animal_type)config
                         detectResult:(st_mobile_animal_face_t **)detectResult
                          animalCount:(int *)animalCount{

    EFFECTSTIMELOG(key)
    st_result_t iRet = st_mobile_tracker_animal_face_track(_hAnimalHandle,
                                                           buffer,
                                                           pixelFormat,
                                                           width,
                                                           height,
                                                           stride,
                                                           rotate,
                                                           config,
                                                           detectResult,
                                                           animalCount);
    EFFECTSTIMEPRINT(key, "st_mobile_tracker_animal_face_track");
    return iRet;
}

-(st_result_t)resetAnimalFaceTracker {
    st_result_t iRet;
    if (_hAnimalHandle) {
        iRet = st_mobile_tracker_animal_face_reset(_hAnimalHandle);
    }
    return iRet;
}
#endif

@end
