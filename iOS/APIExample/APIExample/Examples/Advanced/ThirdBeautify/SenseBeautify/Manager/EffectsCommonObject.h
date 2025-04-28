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

/// General object tracking
/// @param pixelBuffer Image data for each frame
/// @param rect General object position
- (st_result_t)detectObjectWithPixelbuffer:(CVPixelBufferRef)pixelBuffer
                                      rect:(st_rect_t *)rect
                                     score:(float *)score;

/// Set general object tracking
/// @param buffer Image data for each frame
/// @param pixelFormat Video data format (YUV/RGBA/BGRA......)
/// @param width Image width
/// @param height Image height
/// @param stride Image stride
/// @param rect General object position
- (st_result_t)setObjectWithBuffer:(unsigned char *)buffer
                       pixelFormat:(st_pixel_format)pixelFormat
                             width:(int)width
                            height:(int)height
                            stride:(int)stride
                              rect:(st_rect_t *)rect;

/// General object tracking
/// @param buffer Image data for each frame
/// @param pixelFormat Video data format (YUV/RGBA/BGRA......)
/// @param width Image width
/// @param height Image height
/// @param stride Image stride
/// @param rect General object position
- (st_result_t)detectObjectWithBuffer:(unsigned char *)buffer
                          pixelFormat:(st_pixel_format)pixelFormat
                                width:(int)width
                               height:(int)height
                               stride:(int)stride
                                 rect:(st_rect_t *)rect
                                score:(float *)result_score;

#endif

@end

NS_ASSUME_NONNULL_END
