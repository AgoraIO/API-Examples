//
//  EffectsTracker.h
//  SenseMeEffects
//
//  Created by sunjian on 2021/7/19.
//  Copyright © 2021 SenseTime. All rights reserved.
//

#import <Foundation/Foundation.h>
#if __has_include("st_mobile_common.h")
#import "st_mobile_common.h"
#import "st_mobile_object.h"
#endif
#import "EffectMacro.h"

NS_ASSUME_NONNULL_BEGIN

@interface EffectsCommonObject : NSObject

#if __has_include("st_mobile_common.h")
- (void)setObjectRect:(st_rect_t)rect;

/// 通用物体跟踪
/// @param pixelBuffer 每帧图像数据
/// @param rect 通用物体位置
- (st_result_t)detectObjectWithPixelbuffer:(CVPixelBufferRef)pixelBuffer
                                      rect:(st_rect_t*)rect
                                     score:(float*)score;

/// 设置通用物体跟踪
/// @param buffer 每帧图像数据
/// @param pixelFormat 视频数据格式(YUV/RGBA/BGRA......)
/// @param width 图像宽度
/// @param height 图像高度
/// @param stride 图像的stride
/// @param rect 通用物体位置
- (st_result_t)setObjectWithBuffer:(unsigned char *)buffer
                       pixelFormat:(st_pixel_format)pixelFormat
                             width:(int)width
                            height:(int)height
                            stride:(int)stride
                              rect:(st_rect_t*)rect;

/// 通用物体跟踪
/// @param buffer 每帧图像数据
/// @param pixelFormat 视频数据格式(YUV/RGBA/BGRA......)
/// @param width 图像宽度
/// @param height 图像高度
/// @param stride 图像的stride
/// @param rect 通用物体位置
- (st_result_t)detectObjectWithBuffer:(unsigned char *)buffer
                          pixelFormat:(st_pixel_format)pixelFormat
                                width:(int)width
                               height:(int)height
                               stride:(int)stride
                                 rect:(st_rect_t*)rect
                                score:(float *)result_score;


#endif

@end

NS_ASSUME_NONNULL_END
