//
//  EffectsAnimal.h
//  SenseMeEffects
//
//  Created by sunjian on 2021/7/16.
//  Copyright © 2021 SenseTime. All rights reserved.
//

#import <Foundation/Foundation.h>
#if __has_include("st_mobile_common.h")
#import "st_mobile_common.h"
#import "st_mobile_animal.h"
#endif
#import "EffectMacro.h"

NS_ASSUME_NONNULL_BEGIN

@interface EffectsAnimal : NSObject
- (instancetype)initWithType:(EffectsType)type;
#if __has_include("st_mobile_common.h")
/// 动物检测函数
/// @param pixelBuffer 每帧图像数据
/// @param rotate 手机旋转方向
/// @param detectResult 检测结果
- (st_result_t)detectAnimalWithPixelbuffer:(CVPixelBufferRef)pixelBuffer
                                    rotate:(st_rotate_type)rotate
                                    config:(st_mobile_animal_type)config
                              detectResult:(st_mobile_animal_face_t **)detectResult
                               animalCount:(int *)animalCount;

/// 动物检测函数
/// @param buffer 每帧图像数据
/// @param rotate 手机旋转方向
/// @param pixelFormat 视频数据格式(YUV/RGBA/BGRA......)
/// @param width 图像宽度
/// @param height 图像高度
/// @param stride 图像的stride
/// @param detectResult 检测结果
- (st_result_t)detectAnimalWithBuffer:(unsigned char *)buffer
                               rotate:(st_rotate_type)rotate
                          pixelFormat:(st_pixel_format)pixelFormat
                                width:(int)width
                               height:(int)height
                               stride:(int)stride
                               config:(st_mobile_animal_type)config
                         detectResult:(st_mobile_animal_face_t **)detectResult
                          animalCount:(int *)animalCount;

-(st_result_t)resetAnimalFaceTracker;

#endif

@end

NS_ASSUME_NONNULL_END
