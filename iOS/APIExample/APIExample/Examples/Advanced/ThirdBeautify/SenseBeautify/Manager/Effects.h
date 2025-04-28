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
@property(nonatomic, strong) EAGLContext *glContext;
#if __has_include("st_mobile_common.h")
@property(nonatomic) st_handle_t handle;
@property(nonatomic) st_handle_t hConvertHandle;
#endif
@property(nonatomic, assign) uint64_t config;
@property(nonatomic, assign) GLuint outputTexture;
@property(nonatomic) CVPixelBufferRef outputPixelBuffer;
@property(nonatomic) CVOpenGLESTextureRef outputCVTexture;
@property(nonatomic, assign) int width;
@property(nonatomic, assign) int height;
@property(nonatomic) AVCaptureDevicePosition cameraPosition;

- (instancetype)initWithType:(EffectsType)type context:(EAGLContext *)glContext;

/// Set current EAGLContext
/// @param glContext Current GLContext
- (void)setCurrentEAGLContext:(EAGLContext *)glContext;

#if __has_include("st_mobile_common.h")
/// Set effect
/// @param type Effect type
/// @param path Effect resource path
- (st_result_t)setEffectType:(st_effect_beauty_type_t)type path:(NSString *)path;

- (st_result_t)setEffectType:(st_effect_beauty_type_t)type model:(int)model;

/// Set effect intensity value
/// @param type Effect type
/// @param value Intensity value
- (st_result_t)setEffectType:(st_effect_beauty_type_t)type value:(float)value;

/// Set style intensity
/// @param packageId packageID
/// @param type Type
/// @param value Intensity value
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

/// Set sticker
/// @param stickerPath stickerPath 2D sticker
/// @param callback Callback Block
- (void)setStickerWithPath:(NSString *)stickerPath
                  callBack:(void (^)(st_result_t state, int stickerId, uint64_t action))callback;

- (void)setStickerWithPath:(NSString *)stickerPath callBackCustomEventIncluded:(void (^)(st_result_t state, int stickerId, uint64_t action, uint64_t customEvent))callback;

/// Add sticker
/// @param stickerPath Sticker resource path
/// @param callback Callback Block
- (void)addStickerWithPath:(NSString *)stickerPath
                  callBack:(void (^)(st_result_t state, int sticker, uint64_t action))callback;

- (void)addStickerWithPath:(NSString *)stickerPath callBackCustomEventIncluded:(void (^)(st_result_t state, int stickerId, uint64_t action, uint64_t customEvent))callback;

- (void)changeStickerWithPath:(NSString *)stickerPath callBackCustomEventIncluded:(void (^)(st_result_t state, int stickerId, uint64_t action, uint64_t customEvent))callback;

/// Get sticker information from resource
/// @param package_id package_id
/// @param modules Sticker information
- (st_result_t)getModulesInPackage:(int)package_id modules:(st_effect_module_info_t *)modules;

/// Set sticker information
/// @param module_info Sticker information
- (st_result_t)setModuleInfo:(st_effect_module_info_t *)module_info;

/// Replay sticker
/// @param packageId packageId
- (void)replayStickerWithPackage:(int)packageId;

/// Get information about overlapping beauty effects
/// @param callback Callback block
- (void)getOverLap:(void (^)(st_effect_beauty_info_t *beauty_info))callback;

/// Get information about overlapping beauty effects
- (st_effect_beauty_info_t *)getOverlapInfo:(int *)count;

/// Remove sticker
/// @param stickerId Sticker Id
- (st_result_t)removeSticker:(int)stickerId;

/// Clear all stickers
- (st_result_t)cleareStickers;

- (uint64_t)getDetectConfig;
- (uint64_t)getDetectConfigWithMode:(EFDetectConfigMode)configMode;

- (uint64_t)getAnimalDetectConfig;

- (GLuint)processTexture:(GLuint)inputTexture
               inputData:(unsigned char *)inputData
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
               inputData:(unsigned char *)inputData
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

- (st_result_t)setParam:(st_effect_param_t)param andValue:(float)value;

- (st_result_t)setFaceMeshList:(st_mobile_face_mesh_list_t)mesh_list;

#endif

@end

NS_ASSUME_NONNULL_END
