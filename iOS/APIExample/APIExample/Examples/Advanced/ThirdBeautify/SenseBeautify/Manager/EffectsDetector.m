//
//  EffectsCommon.m
//  Effects
//
//  Created by sunjian on 2021/5/8.
//  Copyright © 2021 sjuinan. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "EffectsDetector.h"
#if __has_include("st_mobile_common.h")
#import "st_mobile_human_action.h"
#endif
#import <AVFoundation/AVFoundation.h>
#import "EFGlobalSingleton.h"

@interface EffectsDetector ()
{
#if __has_include("st_mobile_common.h")
    st_handle_t _hDetector;
#endif
}
@end

@implementation EffectsDetector

- (void)dealloc{
#if __has_include("st_mobile_common.h")
    if (_hDetector) {
        st_mobile_human_action_destroy(_hDetector);
    }
#endif
}

- (instancetype)initWithType:(EffectsType)type{
    if ((self = [super init])) {
        [self createHandlerWithType:type];
    }
    return self;
}


- (void)createHandlerWithType:(EffectsType)type{
#if __has_include("st_mobile_common.h")
    if (!_hDetector) {
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
        st_result_t ret = st_mobile_human_action_create(NULL, config, &_hDetector);
        if (ret != ST_OK) {
            NSLog(@"st_mobile_human_action_create error %d", ret);
        }
        
        ret = st_mobile_human_action_setparam(_hDetector, ST_HUMAN_ACTION_PARAM_MESH_MODE, ST_MOBILE_MESH_PART_FACE|ST_MOBILE_MESH_PART_EYE|ST_MOBILE_MESH_PART_MOUTH|ST_MOBILE_MESH_PART_SKULL|ST_MOBILE_MESH_PART_EAR|ST_MOBILE_MESH_PART_NECK|ST_MOBILE_MESH_PART_EYEBROW);
        
        if (ret != ST_OK) {
            NSLog(@"st_mobile_human_action_setparam error %d", ret);
        }
        
        ret = st_mobile_human_action_setparam(_hDetector, ST_HUMAN_ACTION_PARAM_BACKGROUND_SEGMENT_USE_TEMPERATURE, 0.0);
        if (ret != ST_OK) {
            NSLog(@"st_mobile_human_action_setparam error %d", ret);
        }
        
        ret = st_mobile_human_action_setparam(_hDetector, ST_HUMAN_ACTION_PARAM_HEAD_SEGMENT_INSTANCE, 1.0);
        if (ret != ST_OK) {
            NSLog(@"st_mobile_human_action_setparam error %d", ret);
        }
        
        ret = st_mobile_human_action_setparam(_hDetector, ST_HUMAN_ACTION_PARAM_FACE_MESH_OUTPUT_FORMAT, ST_3D_WORLD_COORDINATE);
        if (ret != ST_OK) {
            NSLog(@"st_mobile_human_action_setparam error %d", ret);
        }
    }
#endif
}

#if __has_include("st_mobile_common.h")
st_result_t addSubModel(st_handle_t handle, NSString* file) {
    st_result_t iRet = st_mobile_human_action_add_sub_model(handle, file.UTF8String);
    if (iRet != ST_OK) {
        NSLog(@"st mobile human action add %@ model failed: %d", file, iRet);
    }
    return iRet;
}


- (st_result_t)setModelPath:(NSString *)modelPath{
    
    st_result_t state = ST_OK;

    NSArray *files = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:modelPath error:nil];
//    NSTimeInterval last = CFAbsoluteTimeGetCurrent();
    for(NSString *file in files) {
//        if ([file containsString:@"M_SenseME_Face_"] || [file containsString:@"Occlusion"] || [file containsString:@"M_SenseME_3DMesh_Face2396pt"]) {
            NSString *fullPath = [modelPath stringByAppendingPathComponent:file];
            
            state = st_mobile_human_action_add_sub_model(_hDetector, fullPath.UTF8String);
            if ([fullPath containsString:@"Skin"]) { // 皮肤分割
                [EFGlobalSingleton sharedInstance].efHasSegmentCapability = state != ST_E_NO_CAPABILITY;
            }
            if (state != ST_OK) {
                NSLog(@"st mobile human action add %@ model failed: %d", fullPath, state);
            }
//        }
    }
//    NSLog(@"@mahaomeng cost %f", CFAbsoluteTimeGetCurrent()-last);
    return state;
}

- (st_result_t)setModelPath:(NSString *)modelPath withFirstPhaseFinished:(void(^)(void))finishedCallback {
    st_result_t state = ST_OK;

    NSArray *files = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:modelPath error:nil];
//    NSTimeInterval last = CFAbsoluteTimeGetCurrent();
    for(NSString *file in files) {
        if ([file containsString:@"M_SenseME_Face_"] || [file containsString:@"Occlusion"] || [file containsString:@"M_SenseME_3DMesh_Face2396pt"]) {
            NSString *fullPath = [modelPath stringByAppendingPathComponent:file];
            
            state = st_mobile_human_action_add_sub_model(_hDetector, fullPath.UTF8String);
            if ([fullPath containsString:@"Skin"]) { // 皮肤分割
                [EFGlobalSingleton sharedInstance].efHasSegmentCapability = state != ST_E_NO_CAPABILITY;
            }
            if (state != ST_OK) {
                NSLog(@"st mobile human action add %@ model failed: %d", fullPath, state);
            }
        }
    }
    if (finishedCallback) {
        finishedCallback();
    }
    for(NSString *file in files) {
        if ([file containsString:@"M_SenseME_Face_"] || [file containsString:@"Occlusion"] || [file containsString:@"M_SenseME_3DMesh_Face2396pt"]) {
            continue;
        }
        NSString *fullPath = [modelPath stringByAppendingPathComponent:file];
        
        state = st_mobile_human_action_add_sub_model(_hDetector, fullPath.UTF8String);
        if ([fullPath containsString:@"Skin"]) { // 皮肤分割
            [EFGlobalSingleton sharedInstance].efHasSegmentCapability = state != ST_E_NO_CAPABILITY;
        }
        if (state != ST_OK) {
            NSLog(@"st mobile human action add %@ model failed: %d", fullPath, state);
        }
    }
    
    return state;
}

-(st_result_t)setParam:(st_human_action_param_type)type andValue:(float)value {
    st_result_t iRet = st_mobile_human_action_setparam(_hDetector, type, value);
    if (iRet != ST_OK) {
        NSLog(@"st_mobile_human_action_setparam error %d", iRet);
    }
    return iRet;
}

- (st_result_t)detectHumanActionWithPixelbuffer:(CVPixelBufferRef)pixelBuffer
                                         config:(unsigned long long)detectConfig
                                         rotate:(st_rotate_type)rotate
                                   detectResult:(st_mobile_human_action_t *)detectResult{
    CVPixelBufferLockBaseAddress(pixelBuffer, 0);
    unsigned char* pixelData = (unsigned char*)CVPixelBufferGetBaseAddress(pixelBuffer);
    int pixelDataSize = (int)CVPixelBufferGetDataSize(pixelBuffer);
    int iBytesPerRow = (int)CVPixelBufferGetBytesPerRow(pixelBuffer);
    int iWidth = (int)CVPixelBufferGetWidth(pixelBuffer);
    int iHeight = (int)CVPixelBufferGetHeight(pixelBuffer);
    st_result_t state = [self detectHumanActionWithBuffer:pixelData
                                                     size:pixelDataSize
                                                    config:detectConfig
                                                    rotate:rotate
                                               pixelFormat:ST_PIX_FMT_BGRA8888
                                                     width:iWidth
                                                    height:iHeight
                                                    stride:iBytesPerRow
                                              detectResult:detectResult];
    CVPixelBufferUnlockBaseAddress(pixelBuffer, 0);
    return state;
}

- (st_result_t)detectHumanActionWithBuffer:(unsigned char *)buffer
                                      size:(int)bufferSize
                                    config:(unsigned long long)detect_config
                                    rotate:(st_rotate_type)rotate
                               pixelFormat:(st_pixel_format)pixelFormat
                                     width:(int)width
                                    height:(int)height
                                    stride:(int)stride
                              detectResult:(st_mobile_human_action_t *)detectResult{
    EFFECTSTIMELOG(key)
    st_result_t iRet = ST_OK;
    if (detect_config) {
        iRet = st_mobile_human_action_detect(_hDetector,
                                             buffer,
                                             pixelFormat,
                                             width,
                                             height,
                                             stride,
                                             rotate,
                                             detect_config,
                                             detectResult);
    }
    EFFECTSTIMEPRINT(key, "st_mobile_human_action_detect");
    return iRet;
}

- (st_mobile_human_action_t)detectHumanActionWithPixelBuffer:(CVPixelBufferRef)pixelBuffer {
    
    CVPixelBufferLockBaseAddress(pixelBuffer, 0);
    unsigned char* pBGRAImageIn = (unsigned char*)CVPixelBufferGetBaseAddress(pixelBuffer);
    int iWidth = (int)CVPixelBufferGetWidth(pixelBuffer);
    int iHeight = (int)CVPixelBufferGetHeight(pixelBuffer);
    int iBytesPerRow = (int)CVPixelBufferGetBytesPerRow(pixelBuffer);
    st_mobile_human_action_t detectResult;
    memset(&detectResult, 0, sizeof(st_mobile_human_action_t));
    st_result_t iRet = ST_OK;
    EFFECTSTIMELOG(key)
    iRet = st_mobile_human_action_detect(_hDetector,
                                         pBGRAImageIn,
                                         ST_PIX_FMT_BGRA8888,
                                         iWidth,
                                         iHeight,
                                         iBytesPerRow,
                                         ST_CLOCKWISE_ROTATE_0,
                                         ST_MOBILE_FACE_DETECT ,
                                         &detectResult);
    EFFECTSTIMEPRINT(key, "st_mobile_human_action_detect");
    CVPixelBufferUnlockBaseAddress(pixelBuffer, 0);
    return detectResult;
}


///检测脸型
- (st_face_shape_t)detectFaceShape:(st_mobile_face_t)p_face {
    
    st_face_shape_t shape_t = ST_FACE_SHAPE_UNKNOWN;
    st_result_t iRet = st_mobile_human_action_get_face_shape(_hDetector, &p_face, &shape_t);
    if (iRet != ST_OK) {
        NSLog(@"human action get face shape failed %d", iRet);
    }
    return shape_t;
}

-(st_result_t)getMeshList:(st_mobile_face_mesh_list_t *)p_mesh {
    st_result_t state;
    st_mobile_mesh_info_t mesh_info;
    state = st_mobile_human_action_get_mesh_info(_hDetector, ST_MOBILE_FACE_MESH, &mesh_info);
    
    if (state != ST_OK) {
        NSLog(@"st_mobile_human_action_get_mesh_list failed");
    }
    return state;
}

-(st_result_t)getMeshInfo:(st_mobile_mesh_info_t *)mesh_info {
    st_result_t state;
    state = st_mobile_human_action_get_mesh_info(_hDetector, ST_MOBILE_FACE_MESH, mesh_info);
    if (state != ST_OK) {
        NSLog(@"st_mobile_human_action_get_mesh_info failed");
    }
    return state;
}

-(st_result_t)resetHumanAction {
    if (_hDetector) {
        return st_mobile_human_action_reset(_hDetector);
    }
    return -2;
}

#endif
@end
