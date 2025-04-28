//
//  EffectsAttribute.m
//  SenseMeEffects
//
//  Created by sunjian on 2021/7/16.
//  Copyright © 2021 SoftSugar. All rights reserved.
//

#import "EffectsAttribute.h"
#import <AVFoundation/AVFoundation.h>

@interface EffectsAttribute ()
{
#if __has_include("st_mobile_common.h")
    st_handle_t _hAttributeHandle;
#endif
}
@end

@implementation EffectsAttribute
- (void)dealloc{
#if __has_include("st_mobile_common.h")
    if (_hAttributeHandle) {
        st_mobile_face_attribute_destroy(_hAttributeHandle);
    }
#endif
}

- (instancetype)init{
    if ((self = [super init])) {
        [self createHandler];
    }
    return self;
}

- (void)createHandler{
    NSString *strAttriModelPath = [[NSBundle mainBundle] pathForResource:@"M_SenseME_Attribute_p_1.2.8.1" ofType:@"model"];
#if __has_include("st_mobile_common.h")
   st_result_t ret = st_mobile_face_attribute_create(strAttriModelPath.UTF8String, &_hAttributeHandle);
    if (ret != ST_OK) {
        NSLog(@"st_mobile_face_attribute_create error %d", ret);
    }
#endif
}

#if __has_include("st_mobile_common.h")
- (st_result_t)detectAttributeWithPixelbuffer:(CVPixelBufferRef)pixelBuffer
                                 detectResult:(st_mobile_human_action_t)detectResult
                                    attrArray:(st_mobile_attributes_t *)pAttrArray{
    if (detectResult.face_count == 0) return ST_E_INVALIDARG;
    st_mobile_106_t *faces = &detectResult.p_faces[0].face106;
    CVPixelBufferLockBaseAddress(pixelBuffer, 0);
    unsigned char* pixelData = (unsigned char*)CVPixelBufferGetBaseAddress(pixelBuffer);
    int iBytesPerRow = (int)CVPixelBufferGetBytesPerRow(pixelBuffer);
    int iWidth = (int)CVPixelBufferGetWidth(pixelBuffer);
    int iHeight = (int)CVPixelBufferGetHeight(pixelBuffer);
    st_result_t ret = [self detectAttributeWithBuffer:pixelData
                                          pixelFormat:ST_PIX_FMT_BGRA8888
                                                width:iWidth
                                               height:iHeight
                                               stride:iBytesPerRow
                                                faces:faces
                                            attrArray:pAttrArray];
    CVPixelBufferUnlockBaseAddress(pixelBuffer, 0);
    return ret;
}

- (st_result_t)detectAttributeWithBuffer:(unsigned char *)buffer
                             pixelFormat:(st_pixel_format)pixelFormat
                                   width:(int)width
                                  height:(int)height
                                  stride:(int)stride
                                   faces:(st_mobile_106_t *)faces
                               attrArray:(st_mobile_attributes_t *)pAttrArray{
    EFFECTSTIMELOG(key)
    st_result_t iRet = st_mobile_face_attribute_detect(_hAttributeHandle,
                                                       buffer,
                                                       pixelFormat,
                                                       width,
                                                       height,
                                                       stride,
                                                       faces,
                                                       1, // Here only one face is
                                                       &pAttrArray);
    EFFECTSTIMEPRINT(key, "st_mobile_face_attribute_detect")
    st_mobile_attributes_t attributeDisplay = pAttrArray[0];
    NSString *strAttrDescription = [self getDescriptionOfAttribute:attributeDisplay];
    NSLog(@"@@@ %@", strAttrDescription);
    return iRet;
}


- (NSString *)getDescriptionOfAttribute:(st_mobile_attributes_t)attribute {
    NSString *strAge , *strGender , *strAttricative = nil;
    
    for (int i = 0; i < attribute.attribute_count; i ++) {
        
        // Read one attribute
        st_mobile_attribute_t attributeOne = attribute.p_attributes[i];
        
        // Get attribute category
        const char *attr_category = attributeOne.category;
        const char *attr_label = attributeOne.label;
        
        // Age
        if (0 == strcmp(attr_category, "age")) {
            
            strAge = [NSString stringWithUTF8String:attr_label];
        }
        
        // Attractive
        if (0 == strcmp(attr_category, "attractive")) {
            
            strAttricative = [NSString stringWithUTF8String:attr_label];
        }
        
        // Gender
        if (0 == strcmp(attr_category, "gender")) {
            
            if (0 == strcmp(attr_label, "male") ) {
                
                strGender = @"Male";
            }
            
            if (0 == strcmp(attr_label, "female") ) {
                
                strGender = @"Female";
            }
        }
    }
    
    NSString *strAttrDescription = [NSString stringWithFormat:@"looks:%@ gender:%@ age:%@" , strAttricative , strGender , strAge];
    
    return strAttrDescription;
}

- (st_result_t)detectAttributeWithPixelbuffer:(CVPixelBufferRef)pixelBuffer
                                 detectResult:(st_mobile_human_action_t)detectResult
                                    attrArray:(st_mobile_attributes_t *)pAttrArray withGenderCallback:(void(^)(BOOL isMale))callback {
    if (detectResult.face_count == 0) return ST_E_INVALIDARG;
    st_mobile_106_t *faces = &detectResult.p_faces[0].face106;
    CVPixelBufferLockBaseAddress(pixelBuffer, 0);
    unsigned char* pixelData = (unsigned char*)CVPixelBufferGetBaseAddress(pixelBuffer);
    int iBytesPerRow = (int)CVPixelBufferGetBytesPerRow(pixelBuffer);
    int iWidth = (int)CVPixelBufferGetWidth(pixelBuffer);
    int iHeight = (int)CVPixelBufferGetHeight(pixelBuffer);
    st_result_t ret = [self detectAttributeWithBuffer:pixelData
                                          pixelFormat:ST_PIX_FMT_BGRA8888
                                                width:iWidth
                                               height:iHeight
                                               stride:iBytesPerRow
                                                faces:faces
                                            attrArray:pAttrArray withGenderCallback:callback];
    CVPixelBufferUnlockBaseAddress(pixelBuffer, 0);
    return ret;
}

- (st_result_t)detectAttributeWithBuffer:(unsigned char *)buffer
                             pixelFormat:(st_pixel_format)pixelFormat
                                   width:(int)width
                                  height:(int)height
                                  stride:(int)stride
                                   faces:(st_mobile_106_t *)faces
                               attrArray:(st_mobile_attributes_t *)pAttrArray withGenderCallback:(void(^)(BOOL isMale))callback {
    if (!_hAttributeHandle) {
        if (callback) {
            callback(NO);
        }
        return ST_E_HANDLE;
    }
    EFFECTSTIMELOG(key)
    st_result_t iRet = st_mobile_face_attribute_detect(_hAttributeHandle,
                                                       buffer,
                                                       pixelFormat,
                                                       width,
                                                       height,
                                                       stride,
                                                       faces,
                                                       1, // Here only the attributes of one face, specifically the first face, are taken as a demonstration
                                                       &pAttrArray);
    if (iRet != ST_OK) {
        NSLog(@"st_mobile_face_attribute_detect error %d", iRet);
    }
    EFFECTSTIMEPRINT(key, "st_mobile_face_attribute_detect")
    st_mobile_attributes_t attributeDisplay = pAttrArray[0];
    [self getDescriptionOfAttribute:attributeDisplay withGenderCallback:callback];
    return iRet;
}

- (void)getDescriptionOfAttribute:(st_mobile_attributes_t)attribute withGenderCallback:(void(^)(BOOL isMale))callback {
    if (callback) {
        for (int i = 0; i < attribute.attribute_count; i ++) {
            
            // Read one attribute
            st_mobile_attribute_t attributeOne = attribute.p_attributes[i];
            
            // Get attribute category
            const char *attr_category = attributeOne.category;
            const char *attr_label = attributeOne.label;
            // Gender
            if (0 == strcmp(attr_category, "gender")) {
                
                if (0 == strcmp(attr_label, "male") ) {
                    callback(YES);
                } else if (0 == strcmp(attr_label, "female") ) {
                    callback(NO);
                }
            }
        }
    }
}
#endif
@end
