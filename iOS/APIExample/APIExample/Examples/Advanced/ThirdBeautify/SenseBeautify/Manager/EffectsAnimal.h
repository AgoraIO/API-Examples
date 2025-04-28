//
//  EffectsAnimal.h
//  SenseMeEffects
//
//  Created by sunjian on 2021/7/16.
//  Copyright © 2021 SoftSugar. All rights reserved.
//

#import <Foundation/Foundation.h>
#if __has_include("st_mobile_effect.h")
#import "st_mobile_common.h"
#import "st_mobile_animal.h"
#endif
#import "EffectMacro.h"

NS_ASSUME_NONNULL_BEGIN

@interface EffectsAnimal : NSObject
- (instancetype)initWithType:(EffectsType)type;

#if __has_include("st_mobile_effect.h")
/// Animal detection function
/// @param pixelBuffer Each frame image data
/// @param rotate Phone rotation direction
/// @param detectResult Detection result
- (st_result_t)detectAnimalWithPixelbuffer:(CVPixelBufferRef)pixelBuffer
                                    rotate:(st_rotate_type)rotate
                                    config:(st_mobile_animal_type)config
                              detectResult:(st_mobile_animal_result_t *)detectResult;

/// Animal detection function
/// @param buffer Each frame image data
/// @param rotate Phone rotation direction
/// @param pixelFormat Video data format (YUV/RGBA/BGRA......)
/// @param width Image width
/// @param height Image height
/// @param stride Image stride
/// @param detectResult Detection result
- (st_result_t)detectAnimalWithBuffer:(unsigned char *)buffer
                               rotate:(st_rotate_type)rotate
                          pixelFormat:(st_pixel_format)pixelFormat
                                width:(int)width
                               height:(int)height
                               stride:(int)stride
                               config:(st_mobile_animal_type)config
                         detectResult:(st_mobile_animal_result_t *)detectResult;

- (st_result_t)resetAnimalFaceTracker;
#endif

@end

NS_ASSUME_NONNULL_END
