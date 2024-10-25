//
//  EffectsAttribute.h
//  SenseMeEffects
//
//  Created by sunjian on 2021/7/16.
//  Copyright © 2021 SoftSugar. All rights reserved.
//

#import <Foundation/Foundation.h>
#if __has_include("st_mobile_common.h")
#import "st_mobile_common.h"
#import "st_mobile_human_action.h"
#import "st_mobile_face_attribute.h"
#endif
#import "EffectMacro.h"

NS_ASSUME_NONNULL_BEGIN

@interface EffectsAttribute : NSObject

#if __has_include("st_mobile_common.h")
- (st_result_t)detectAttributeWithPixelbuffer:(CVPixelBufferRef)pixelBuffer
                                 detectResult:(st_mobile_human_action_t)detectResult
                                    attrArray:(st_mobile_attributes_t *)pAttrArray;

- (st_result_t)detectAttributeWithPixelbuffer:(CVPixelBufferRef)pixelBuffer
                                 detectResult:(st_mobile_human_action_t)detectResult
                                    attrArray:(st_mobile_attributes_t *)pAttrArray withGenderCallback:(void(^)(BOOL isMale))callback;

- (st_result_t)detectAttributeWithBuffer:(unsigned char *)buffer
                             pixelFormat:(st_pixel_format)pixelFormat
                                   width:(int)width
                                  height:(int)height
                                  stride:(int)stride
                                   faces:(st_mobile_106_t *)faces
                               attrArray:(st_mobile_attributes_t *)pAttrArray;

- (st_result_t)detectAttributeWithBuffer:(unsigned char *)buffer
                             pixelFormat:(st_pixel_format)pixelFormat
                                   width:(int)width
                                  height:(int)height
                                  stride:(int)stride
                                   faces:(st_mobile_106_t *)faces
                               attrArray:(st_mobile_attributes_t *)pAttrArray withGenderCallback:(void(^)(BOOL isMale))callback;
#endif
@end

NS_ASSUME_NONNULL_END
