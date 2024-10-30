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

/// 初始化对象
/// @param type 类型
- (instancetype)initWithType:(EffectsType)type;

/// 添加model
/// @param modelPath 模型路径
- (st_result_t)setModelPath:(NSString *)modelPath;
- (st_result_t)setModelPath:(NSString *)modelPath withFirstPhaseFinished:(void(^)(void))finishedCallback;

- (st_result_t)setParam:(st_human_action_param_type)type andValue:(float)value;

/// 人脸检测函数
/// @param pixelBuffer 每帧图像数据
/// @param detect_config 检测配置
/// @param rotate 手机旋转方向
/// @param detectResult 检测结果
- (st_result_t)detectHumanActionWithPixelbuffer:(CVPixelBufferRef)pixelBuffer
                                          config:(unsigned long long)detect_config
                                          rotate:(st_rotate_type)rotate
                                    detectResult:(st_mobile_human_action_t *)detectResult;

/// 人脸检测函数
/// @param buffer 每帧图像数据
/// @param bufferSize 图像数据大小
/// @param detect_config 检测配置
/// @param rotate 手机旋转方向
/// @param pixelFormat 视频数据格式(YUV/RGBA/BGRA......)
/// @param width 图像宽度
/// @param height 图像高度
/// @param stride 图像的stride
/// @param detectResult 检测结果
- (st_result_t)detectHumanActionWithBuffer:(unsigned char *)buffer
                                      size:(int)bufferSize
                                    config:(unsigned long long)detect_config
                                    rotate:(st_rotate_type)rotate
                               pixelFormat:(st_pixel_format)pixelFormat
                                     width:(int)width
                                    height:(int)height
                                    stride:(int)stride
                              detectResult:(st_mobile_human_action_t *)detectResult;



///人脸检测
- (st_mobile_human_action_t)detectHumanActionWithPixelBuffer:(CVPixelBufferRef)pixelBuffer;

///检测脸型
- (st_face_shape_t)detectFaceShape:(st_mobile_face_t)p_face;

-(st_result_t)resetHumanAction;

-(st_result_t)getMeshList:(st_mobile_face_mesh_list_t *)p_mesh;
-(st_result_t)getMeshInfo:(st_mobile_mesh_info_t *)mesh_info;

#endif

@end

