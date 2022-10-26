//
//  Effects.h
//  Effects
//
//  Created by sunjian on 2021/5/8.
//  Copyright © 2021 sjuinan. All rights reserved.
//

#import <AVFoundation/AVFoundation.h>
#import <OpenGLES/EAGL.h>
#import "EffectsDetector.h"
#if __has_include("st_mobile_common.h")
#import "st_mobile_common.h"
#import "st_mobile_effect.h"
#endif
#import "EffectMacro.h"

@protocol EFEffectsProcessDelegate <NSObject>

- (void)updateEffectsFacePoint:(CGPoint)point;

#if __has_include("st_mobile_common.h")
- (void)updateCommonObjectPosition:(st_rect_t)rect;
#endif
- (void)updateKeyPoinst:(NSArray *)keyPoints;

@end

@interface EffectsProcess : NSObject

@property (nonatomic, weak)   id<EFEffectsProcessDelegate> delegate;

@property (nonatomic, assign) GLuint inputTexture;

@property (nonatomic, assign) GLuint outputTexture;

@property (nonatomic) CVPixelBufferRef outputPixelBuffer;

@property (nonatomic, assign) uint64_t detectConfig;

/// 鉴权
/// @param licensePath 授权文件路径
+ (BOOL)authorizeWithLicensePath:(NSString *)licensePath;

/// 鉴权
/// @param licenseData 授权文件数据
+ (BOOL)authorizeWithLicenseData:(NSData *)licenseData;

/// 初始化对象
/// @param type 类型
- (instancetype)initWithType:(EffectsType)type glContext:(EAGLContext *)glContext;

#if __has_include("st_mobile_common.h")
/// 加载模型
/// @param modelPath 模型文件路径(可将多个模型放在一个文件中，SDK内部遍历加载Model)
- (st_result_t)setModelPath:(NSString *)modelPath;

/// 设置特效
/// @param type 特效类型
/// @param path 特效素材路径
- (st_result_t)setEffectType:(st_effect_beauty_type_t)type path:(NSString *)path;

/// 设置特效
/// @param type 特效类型
/// @param model 特效素材model
- (st_result_t)setEffectType:(st_effect_beauty_type_t)type model:(int)model;


/// 设置特效强度值
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
/// @param stickerPath stickerPath 贴纸资源路径
/// @param callback 回调Block
- (void)setStickerWithPath:(NSString *)stickerPath
                  callBack:(void(^)(st_result_t state, int stickerId, uint64_t action))callback;

- (void)setStickerWithPath:(NSString *)stickerPath callBackCustomEventIncluded:(void(^)(st_result_t state, int stickerId, uint64_t action, uint64_t customEvent))callback;

/// 添加贴纸
/// @param stickerPath stickerPath 贴纸资源路径
/// @param callback 回调Block
- (void)addStickerWithPath:(NSString *)stickerPath
                  callBack:(void(^)(st_result_t state, int sticker, uint64_t action))callback;

/// 重新播放贴纸
/// @param packageId packageId
-(void)replayStickerWithPackage:(int)packageId;

- (void)addStickerWithPath:(NSString *)stickerPath callBackCustomEventIncluded:(void(^)(st_result_t state, int stickerId, uint64_t action, uint64_t customEvent))callback;

/// 获取获取素材的贴纸信息
/// @param package_id package_id
/// @param modules 贴纸信息
-(st_result_t)getModulesInPackage:(int)package_id modules:(st_effect_module_info_t*)modules;

/// 设置贴纸信息
/// @param module_info 贴纸信息
-(st_result_t)setModuleInfo:(st_effect_module_info_t *)module_info;

/// 获取覆盖生效的美颜的信息
/// @param callback 回调block
- (void)getOverLap:(void(^)(st_effect_beauty_info_t *beauty_info))callback;

/// 获取覆盖生效的美颜的信息
/// If there is st_effect_beauty_info_t, release it when used it;
- (st_effect_beauty_info_t *)getOverlapInfo:(int *)count;

/// 移除贴纸
/// @param stickerId 贴纸Id
- (st_result_t)removeSticker:(int)stickerId;

/// 清空贴纸
- (st_result_t)cleareStickers;

- (st_result_t)detectWithPixelBuffer:(CVPixelBufferRef)pixelBuffer
                              rotate:(st_rotate_type)rotate
                      cameraPosition:(AVCaptureDevicePosition)position
                         humanAction:(st_mobile_human_action_t *)detectResult
                        animalResult:(st_mobile_animal_face_t **)animalResult
                         animalCount:(int *)animalCount;

-(st_result_t)resetHumanAction;

- (st_result_t)renderPixelBuffer:(CVPixelBufferRef)pixelBuffer
                          rotate:(st_rotate_type)rotate
                     humanAction:(st_mobile_human_action_t)detectResult
                    animalResult:(st_mobile_animal_face_t *)animalResult
                     animalCount:(int)animalCount
                      outTexture:(GLuint)outTexture
                  outPixelFormat:(st_pixel_format)fmt_out
                         outData:(unsigned char *)img_out;

/// 处理视频数据
/// @param pixelBuffer 视频数据
/// @param rotate 当前手机的旋转方向
/// @param position 当前手机摄像头方向
/// @param outTexture 目标纹理, 仅支持RGBA纹理
/// @param fmt_out 输出图片的类型,支持NV21,BGR,BGRA,NV12,RGBA,YUV420P格式
/// @param img_out 输出图像数据数组,需要用户分配内存,如果是null, 不输出buffer
- (st_result_t)processPixelBuffer:(CVPixelBufferRef)pixelBuffer
                            rotate:(st_rotate_type)rotate
                    cameraPosition:(AVCaptureDevicePosition)position
                        outTexture:(GLuint)outTexture
                    outPixelFormat:(st_pixel_format)fmt_out
                           outData:(unsigned char *)img_out;

/// 处理图像数据
/// @param data     图像数据
/// @param dataSize 图像数据大小
/// @param width    图像宽度
/// @param height   图像高度
/// @param stride   图像数据跨度
/// @param rotate   手机旋转方向
/// @param pixelFormat 图像格式
/// @param outTexture 输出纹理,仅支持RGBA纹理
/// @param fmt_out 输出图片的类型,支持NV21,BGR,BGRA,NV12,RGBA,YUV420P格式
/// @param outData  输出数据
- (st_result_t)processData:(unsigned char *)data
                       size:(int)dataSize
                      width:(int)width
                     height:(int)height
                     stride:(int)stride
                     rotate:(st_rotate_type)rotate
                pixelFormat:(st_pixel_format)pixelFormat
               inputTexture:(GLuint)inputTexture
                 outTexture:(GLuint)outTexture
             outPixelFormat:(st_pixel_format)fmt_out
                    outData:(unsigned char *)outData;


/// 创建一个纹理
/// @param width 纹理宽度
/// @param height 纹理高度
- (GLuint)createTextureWidth:(int)width height:(int)height;

/// 创建OpenGL对象
/// @param width 像素宽
/// @param height 像素高
/// @param texture 纹理
/// @param pixelBuffer 像素缓存
- (void)createGLObjectWith:(int)width
                    height:(int)height
                   texture:(GLuint *)texture
               pixelBuffer:(CVPixelBufferRef *)pixelBuffer
                 cvTexture:(CVOpenGLESTextureRef *)cvTexture;

- (id<MTLTexture>)createMetalTextureWithDevice:(id<MTLDevice>)metalDevice width:(int)width height:(int)height pixelBuffer:(CVPixelBufferRef)pixelBuffer;

/// 删除一个纹理
/// @param texture 纹理
- (void)deleteTexture:(GLuint *)texture
          pixelBuffer:(CVPixelBufferRef *)pixelBuffer
            cvTexture:(CVOpenGLESTextureRef *)cvTexture;;

/// 获取纹理
/// @param pixelBuffer 输入pixelBuffer
- (GLuint)getTexutreWithPixelBuffer:(CVPixelBufferRef)pixelBuffer;



/// 人脸检测
/// @param pixelBuffer 像素缓存
/// @ [out] 返回检测结果值
- (st_mobile_human_action_t)detectHumanActionWithPixelBuffer:(CVPixelBufferRef)pixelBuffer;


/// 获取人脸形状
/// @param p_face 检测到的人脸信息
/// @out  p_face_shape 人脸形状
- (st_face_shape_t)detectFaceShape:(st_mobile_face_t)p_face;

/// 设置通用物体位置
/// @param rect 物体方框
- (void)setObjectTrackRect:(st_rect_t)rect;

#endif

/// 是否鉴权成功
- (BOOL)isAuthrized;

-(void)getMeshList;

@end
