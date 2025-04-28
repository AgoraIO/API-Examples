//
//  EffectsCommon.h
//  Effects
//
//  Created by sunjian on 2021/5/8.
//  Copyright © 2021 sjuinan. All rights reserved.
//

#import <Foundation/Foundation.h>
#if __has_include("st_mobile_common.h")
#import "st_mobile_common.h"
#import "st_mobile_human_action.h"
#endif
#import "EffectMacro.h"

@interface EffectsDetector : NSObject

#if __has_include("st_mobile_common.h")

/// Initialize object
/// @param type Type
- (instancetype)initWithType:(EffectsType)type;

/// Add model
/// @param modelPath Model path
- (st_result_t)setModelPath:(NSString *)modelPath;
- (st_result_t)setModelPath:(NSString *)modelPath withFirstPhaseFinished:(void (^)(void))finishedCallback;

- (st_result_t)setParam:(st_human_action_param_type)type andValue:(float)value;

/// Human action detection function
/// @param pixelBuffer Each frame image data
/// @param detect_config Detection configuration
/// @param rotate Phone rotation direction
/// @param detectResult Detection result
- (st_result_t)detectHumanActionWithPixelbuffer:(CVPixelBufferRef)pixelBuffer
                                         config:(unsigned long long)detect_config
                                         rotate:(st_rotate_type)rotate
                                   detectResult:(st_mobile_human_action_t *)detectResult;

/// Human action detection function
/// @param buffer Each frame image data
/// @param bufferSize Image data size
/// @param detect_config Detection configuration
/// @param rotate Phone rotation direction
/// @param pixelFormat Video data format (YUV/RGBA/BGRA......)
/// @param width Image width
/// @param height Image height
/// @param stride Image stride
/// @param detectResult Detection result
- (st_result_t)detectHumanActionWithBuffer:(unsigned char *)buffer
                                      size:(int)bufferSize
                                    config:(unsigned long long)detect_config
                                    rotate:(st_rotate_type)rotate
                               pixelFormat:(st_pixel_format)pixelFormat
                                     width:(int)width
                                    height:(int)height
                                    stride:(int)stride
                              detectResult:(st_mobile_human_action_t *)detectResult;

/// Human action detection
- (st_mobile_human_action_t)detectHumanActionWithPixelBuffer:(CVPixelBufferRef)pixelBuffer;

/// Detect face shape
- (st_face_shape_t)detectFaceShape:(st_mobile_face_t)p_face;

- (st_result_t)resetHumanAction;

- (st_result_t)getMeshList:(st_mobile_face_mesh_list_t *)p_mesh;
- (st_result_t)getMeshInfo:(st_mobile_mesh_info_t *)mesh_info;

#endif

@end
