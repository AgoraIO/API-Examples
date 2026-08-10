//
//  Effects.h
//  SenseMeEffects
//
//  Created by sunjian on 2021/6/4.
//

#import <Foundation/Foundation.h>
#if __has_include("st_mobile_common.h")
#import "st_mobile_common.h"
#import "st_mobile_effect.h"
#endif
#import "EffectMacro.h"

NS_ASSUME_NONNULL_BEGIN

@interface EffectsMotionManager : NSObject
@end

@interface Effects : NSObject
@property (nonatomic, strong) EAGLContext *glContext;
#if __has_include("st_mobile_common.h")
@property (nonatomic) st_handle_t handle;
@property (nonatomic) st_handle_t hConvertHandle;
#endif
@property (nonatomic, assign) uint64_t config;
@property (nonatomic, assign) GLuint outputTexture;
@property (nonatomic) CVPixelBufferRef outputPixelBuffer;
@property (nonatomic) CVOpenGLESTextureRef outputCVTexture;
@property (nonatomic, assign) int width;
@property (nonatomic, assign) int height;
@property (nonatomic) AVCaptureDevicePosition cameraPosition;

- (instancetype)initWithType:(EffectsType)type context:(EAGLContext *)glContext;

/// 设置当前EAGLContext
/// @param glContext 当前GLContext
- (void)setCurrentEAGLContext:(EAGLContext*)glContext;

#if __has_include("st_mobile_common.h")
/// 设置特效
/// @param type 特效类型
/// @param path 特效素材路径
- (st_result_t)setEffectType:(st_effect_beauty_type_t)type path:(NSString *)path;

- (st_result_t)setEffectType:(st_effect_beauty_type_t)type model:(int)model;

/// 设置特效程度值
/// @param type 特效类型
/// @param value 强度值
- (st_result_t)setEffectType:(st_effect_beauty_type_t)type value:(float)value;

/// 设置风格强度
/// @param packageId packageID
/// @param type 类型
/// @param value 强度值
- (st_result_t)setPackageId:(int)packageId groupType:(st_effect_beauty_group_t)type strength:(float)value;

- (st_result_t)setTryon:(st_effect_tryon_info_t *)tryonInfo andTryonType:(st_effect_beauty_type_t)tryonType;

- (st_result_t)getTryon:(st_effect_tryon_info_t *)tryonInfo andTryonType:(st_effect_beauty_type_t)tryonType;

- (st_result_t)setBeautyParam:(st_effect_beauty_param_t)param andVal:(float)val;

- (st_result_t)getBeautyParam:(st_effect_beauty_param_t)param andVal:(float *)val;

- (st_result_t)get3dBeautyPartsSize:(int *)partSize;

- (st_result_t)get3dBeautyParts:(st_effect_3D_beauty_part_info_t[])parts fromSize:(int)partSize;

- (st_result_t)set3dBeautyPartsStrength:(st_effect_3D_beauty_part_info_t *)parts andVal:(int)partSize;

- (st_result_t)f_set3dBeautyPartsStrength:(st_effect_3D_beauty_part_info_t[])parts andVal:(int)partSize;

- (st_result_t)disableOverlap:(BOOL)isDisableOverlap;
- (st_result_t)disableModuleReorder:(BOOL)isDisableModuleReorder;

/// 设置贴纸
/// @param stickerPath stickerPath 2D贴纸
/// @param callback 回调Block
- (void)setStickerWithPath:(NSString *)stickerPath
                  callBack:(void(^)(st_result_t state, int stickerId, uint64_t action))callback;

- (void)setStickerWithPath:(NSString *)stickerPath callBackCustomEventIncluded:(void(^)(st_result_t state, int stickerId, uint64_t action, uint64_t customEvent))callback;

/// 添加贴纸
/// @param stickerPath stickerPath 贴纸资源路径
/// @param callback 回调Block
- (void)addStickerWithPath:(NSString *)stickerPath
                  callBack:(void(^)(st_result_t state, int sticker, uint64_t action))callback;

- (void)addStickerWithPath:(NSString *)stickerPath callBackCustomEventIncluded:(void(^)(st_result_t state, int stickerId, uint64_t action, uint64_t customEvent))callback;

-(void)changeStickerWithPath:(NSString *)stickerPath callBackCustomEventIncluded:(void(^)(st_result_t state, int stickerId, uint64_t action, uint64_t customEvent))callback;

/// 获取获取素材的贴纸信息
/// @param package_id package_id
/// @param modules 贴纸信息
-(st_result_t)getModulesInPackage:(int)package_id modules:(st_effect_module_info_t*)modules;

/// 设置贴纸信息
/// @param module_info 贴纸信息
-(st_result_t)setModuleInfo:(st_effect_module_info_t *)module_info;

/// 重新播放贴纸
/// @param packageId packageId
-(void)replayStickerWithPackage:(int)packageId;


/// 获取覆盖生效的美颜的信息
/// @param callback 回调block
- (void)getOverLap:(void(^)(st_effect_beauty_info_t *beauty_info))callback;

/// 获取覆盖生效的美颜的信息
- (st_effect_beauty_info_t *)getOverlapInfo:(int *)count;

/// 移除贴纸
/// @param stickerId 贴纸Id
- (st_result_t)removeSticker:(int)stickerId;

/// 清空贴纸
- (st_result_t)cleareStickers;

- (uint64_t)getDetectConfig;
- (uint64_t)getDetectConfigWithMode:(EFDetectConfigMode)configMode;

- (uint64_t)getAnimalDetectConfig;

- (GLuint)processTexture:(GLuint)inputTexture
               inputData:(unsigned char*)inputData
             inputFormat:(st_pixel_format)inputFormat
           outputTexture:(GLuint)outputTexture
                   width:(int)width
                  height:(int)height
                  stride:(int)stride
                  rotate:(st_rotate_type)rotate
            detectResult:(st_mobile_human_action_t)detectResult
            animalResult:(st_mobile_animal_result_t *)animalResult
         outDetectResult:(st_mobile_human_action_t)outDetectResult
               withCache:(CVOpenGLESTextureCacheRef)cache
          outPixelFormat:(st_pixel_format)fmt_out
               outBuffer:(unsigned char *)img_out;

- (GLuint)processTexture:(GLuint)inputTexture
               inputData:(unsigned char*)inputData
             inputFormat:(st_pixel_format)inputFormat
           outputTexture:(GLuint)outputTexture
                   width:(int)width
                  height:(int)height
                  stride:(int)stride
                  rotate:(st_rotate_type)rotate
            detectResult:(st_mobile_human_action_t)detectResult
            animalResult:(st_mobile_animal_result_t *)animalResult
         outDetectResult:(st_mobile_human_action_t)outDetectResult
               withCache:(CVOpenGLESTextureCacheRef)cache
          outPixelFormat:(st_pixel_format)fmt_out
               outBuffer:(unsigned char *)img_out
                meshList:(st_mobile_face_mesh_list_t)mesh_list;

- (void)convertYUVBuffer:(unsigned char *)buffer
                    rgba:(GLuint)texture
                    size:(CGSize)size;

-(st_result_t)setParam:(st_effect_param_t)param andValue:(float)value;

-(st_result_t)setFaceMeshList:(st_mobile_face_mesh_list_t)mesh_list;

#endif

@end

NS_ASSUME_NONNULL_END
