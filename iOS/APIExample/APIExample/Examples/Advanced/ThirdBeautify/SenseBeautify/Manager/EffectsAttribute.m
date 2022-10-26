//
//  EffectsAttribute.m
//  SenseMeEffects
//
//  Created by sunjian on 2021/7/16.
//  Copyright © 2021 SenseTime. All rights reserved.
//

#import "EffectsAttribute.h"
#import "BundleUtil.h"
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
#if __has_include("st_mobile_common.h")
    NSBundle *bundle = [BundleUtil bundleWithBundleName:@"SenseLib" podName:@"senseLib"];
    NSString *strAttriModelPath = [bundle pathForResource:@"M_SenseME_Attribute_1.0.1" ofType:@"model"];
    
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
    st_mobile_106_t *faces = (st_mobile_106_t *)malloc(sizeof(st_mobile_106_t) * detectResult.face_count);
    memset(faces, 0, sizeof(st_mobile_106_t)*detectResult.face_count);
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
    free(faces);faces=nil;
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
                                                       1, // 这里仅取一张脸也就是第一张脸的属性作为演示
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
        
        // 读取一条属性
        st_mobile_attribute_t attributeOne = attribute.p_attributes[i];
        
        // 获取属性类别
        const char *attr_category = attributeOne.category;
        const char *attr_label = attributeOne.label;
        
        // 年龄
        if (0 == strcmp(attr_category, "age")) {
            
            strAge = [NSString stringWithUTF8String:attr_label];
        }
        
        // 颜值
        if (0 == strcmp(attr_category, "attractive")) {
            
            strAttricative = [NSString stringWithUTF8String:attr_label];
        }
        
        // 性别
        if (0 == strcmp(attr_category, "gender")) {
            
            if (0 == strcmp(attr_label, "male") ) {
                
                strGender = @"男";
            }
            
            if (0 == strcmp(attr_label, "female") ) {
                
                strGender = @"女";
            }
        }
    }
    
    NSString *strAttrDescription = [NSString stringWithFormat:@"颜值:%@ 性别:%@ 年龄:%@" , strAttricative , strGender , strAge];
    
    return strAttrDescription;
}
#endif
@end
