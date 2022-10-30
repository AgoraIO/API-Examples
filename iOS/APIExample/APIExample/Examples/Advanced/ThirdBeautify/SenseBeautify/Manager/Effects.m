//
//  Effects.m
//  SenseMeEffects
//
//  Created by sunjian on 2021/6/4.
//

#import "Effects.h"
#import <OpenGLES/ES3/glext.h>
#import <CoreMotion/CoreMotion.h>
#import <AVFoundation/AVFoundation.h>

@interface EffectsMotionManager ()
{
    BOOL _begin;
}
@property (nonatomic, strong) CMMotionManager *motionManager;
@end

@implementation EffectsMotionManager

+ (instancetype)sharedInstance{
    static EffectsMotionManager *manager = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        manager = [[EffectsMotionManager alloc] init];
    });
    return manager;
}

#pragma mark - getter/setter
- (CMMotionManager *)motionManager{
    if (!_motionManager) {
        _motionManager = [[CMMotionManager alloc] init];
        _motionManager.accelerometerUpdateInterval = 0.5;
        _motionManager.deviceMotionUpdateInterval = 1 / 25.0;
    }
    return _motionManager;
}

- (void)start{
    if(_begin) return;
    _begin = YES;
    if ([self.motionManager isAccelerometerAvailable]) {
        [self.motionManager startAccelerometerUpdates];
    }
    if ([self.motionManager isDeviceMotionAvailable]) {
        [self.motionManager startDeviceMotionUpdates];
    }
}

- (void)stop{
    _begin = NO;
    [self.motionManager stopAccelerometerUpdates];
    [self.motionManager stopDeviceMotionUpdates];
}

@end


@interface Effects ()
{
    EffectsType _type;
}
//@property (nonatomic, strong) EffectsAudioPlayerManager *audioManager;
@end

@implementation Effects

- (void)dealloc{
#if __has_include("st_mobile_common.h")
    if (self.handle) {
        st_mobile_effect_destroy_handle(self.handle);
    }
    if (self.hConvertHandle) {
        st_mobile_color_convert_destroy(self.hConvertHandle);
    }
#endif
    [self destoryGLResource];
    [[EffectsMotionManager sharedInstance] stop];
}

- (void)destoryGLResource{
    if(self.outputTexture){
        GLuint texture = self.outputTexture;
        glDeleteTextures(1, &texture);
        self.outputTexture = 0;
    }
    if(self.outputPixelBuffer){
        CVPixelBufferRef pixelBuffer = self.outputPixelBuffer;
        CVPixelBufferRelease(pixelBuffer);
    }
    if(self.outputCVTexture){
        CVOpenGLESTextureRef cvTexture = self.outputCVTexture;
        CFRelease(cvTexture);
    }
}


- (instancetype)initWithType:(EffectsType)type context:(EAGLContext *)glContext{
    self = [super init];
    [self createHandleWithType:type context:glContext];
    return self;
}

- (void)createHandleWithType:(EffectsType)type context:(EAGLContext *)glContext{
#if __has_include("st_mobile_common.h")
    st_handle_t handle;
    st_result_t ret;
    _type = type;
    switch (type) {
        case EffectsTypePreview:{
            ret = st_mobile_effect_create_handle(EFFECT_CONFIG_NONE, &handle);
            if (ret != ST_OK) {
                NSLog(@"st_mobile_effect_create_handle error %d", ret);
                return;
            }
//            self.audioManager = [[EffectsAudioPlayerManager alloc] init];
//            self.audioManager.delegate = self;
//            st_result_t setRet = ST_OK;
//            setRet = st_mobile_effect_set_module_state_change_callback(handle, _modul_state_change_callback);
//            if (setRet != ST_OK) {
//                NSLog(@"st_mobile_effect_set_module_state_change_callback error %d", setRet);
//            }
//            setRet = st_mobile_effect_set_packaged_state_change_callback(handle, _package_state_change_callback);
//            if (setRet != ST_OK) {
//                NSLog(@"st_mobile_effect_set_packaged_state_change_callback error %d", setRet);
//            }
            self.glContext = glContext;
        }
            break;
        case EffectsTypeVideo:
            ret = st_mobile_effect_create_handle(EFFECT_CONFIG_IMAGE_MODE, &handle);
            break;
        case EffectsTypePhoto:
            ret = st_mobile_effect_create_handle(EFFECT_CONFIG_IMAGE_MODE, &handle);
            break;
    }
    st_mobile_effect_set_param(handle, EFFECT_PARAM_MAX_MEMORY_BUDGET_MB, 1000.0);
//    st_mobile_effect_set_param(handle, EFFECT_PARAM_QUATERNION_SMOOTH_FRAME, 5.0); /// x< 设置相机姿态平滑参数，表示平滑多少帧, 越大延迟越高，抖动越微弱
            
    if (ret != ST_OK) {
        NSLog(@"st_mobile_effect_create_handle %d", ret);
        return;
    }
    ret = st_mobile_color_convert_create(&_hConvertHandle);
    if (ret != ST_OK) {
        NSLog(@"st_mobile_color_convert_create %d", ret);
        return;
    }
    [[EffectsMotionManager sharedInstance] start];
    
    self.handle = handle;
#endif
}

#if __has_include("st_mobile_common.h")
- (st_result_t)setEffectType:(st_effect_beauty_type_t)type path:(NSString *)path{
    st_result_t iRet = ST_OK;
    iRet = st_mobile_effect_set_beauty(self.handle, type, path.UTF8String);
    if (iRet != ST_OK) {
        NSLog(@"st mobile beautify set beautiy type %d failed: %d", type, iRet);
    }
    return iRet;
}

- (st_result_t)setEffectType:(st_effect_beauty_type_t)type model:(int)model{
    st_result_t iRet = ST_OK;
    iRet = st_mobile_effect_set_beauty_mode(self.handle, type, model);
    if (iRet != ST_OK) {
        NSLog(@"st mobile beautify set beautiy type %d failed: %d", type, iRet);
    }
    return iRet;
}

- (st_result_t)setEffectType:(st_effect_beauty_type_t)type value:(float)value{
    st_result_t iRet = ST_OK;
    iRet = st_mobile_effect_set_beauty_strength(self.handle, type, value);
    if (iRet != ST_OK) {
        NSLog(@"st mobile beautify set beautiy type %d failed: %d", type, iRet);
    }
    return iRet;
}

- (st_result_t)setPackageId:(int)packageId groupType:(st_effect_beauty_group_t)type strength:(float)value{
    st_result_t iRet = ST_OK;
    iRet = st_mobile_effect_set_package_beauty_group_strength(self.handle, packageId, type, value);
    if (iRet != ST_OK) {
        NSLog(@"st_mobile_effect_set_package_beauty_group_strength failed");
    }
    return iRet;
}

- (st_result_t)setTryon:(st_effect_tryon_info_t *)tryonInfo andTryonType:(st_effect_beauty_type_t)tryonType {
    st_result_t iRet = ST_OK;
    iRet = st_mobile_effect_set_tryon_param(self.handle, tryonType, tryonInfo);
    if (iRet != ST_OK) {
        NSLog(@"st_mobile_effect_set_tryon_param failed");
    }
    return iRet;
}

- (st_result_t)getTryon:(st_effect_tryon_info_t *)tryonInfo andTryonType:(st_effect_beauty_type_t)tryonType {
    st_result_t iRet = ST_OK;
    iRet = st_mobile_effect_get_tryon_param(self.handle, tryonType, tryonInfo);
    if (iRet != ST_OK) {
        NSLog(@"st_mobile_effect_get_tryon_param failed");
    }
    return iRet;
}

- (st_result_t)setBeautyParam:(st_effect_beauty_param_t)param andVal:(float)val {
    st_result_t iRet = ST_OK;
    iRet = st_mobile_effect_set_beauty_param(self.handle, param, val);
    if (iRet != ST_OK) {
        NSLog(@"st_mobile_effect_set_beauty_param failed");
    }
    return iRet;
}

- (st_result_t)getBeautyParam:(st_effect_beauty_param_t)param andVal:(float *)val {
    st_result_t iRet = ST_OK;
    iRet = st_mobile_effect_get_beauty_param(self.handle, param, val);
    if (iRet != ST_OK) {
        NSLog(@"st_mobile_effect_get_beauty_param failed");
    }
    return iRet;
}

- (st_result_t)get3dBeautyPartsSize:(int *)partSize {
    st_result_t iRet = ST_OK;
    iRet = st_moobile_effect_get_3d_beauty_parts_count(self.handle, partSize);
    if (iRet != ST_OK) {
        NSLog(@"st_moobile_effect_get_3d_beauty_parts_count failed %d", iRet);
    }
    return iRet;
}

- (st_result_t)get3dBeautyParts:(st_effect_3D_beauty_part_info_t[])parts fromSize:(int)partSize{
    st_result_t iRet = ST_OK;
    iRet = st_mobile_effect_get_3d_beauty_parts(self.handle, parts, partSize);
    if (iRet != ST_OK) {
        NSLog(@"st_mobile_effect_get_3d_beauty_parts failed");
    }
    return iRet;
}

- (st_result_t)set3dBeautyPartsStrength:(st_effect_3D_beauty_part_info_t *)parts andVal:(int)partSize {
    st_result_t iRet = ST_OK;
    iRet = st_mobile_effect_set_3d_beauty_parts_strength(self.handle, parts, partSize);
    if (iRet != ST_OK) {
        NSLog(@"st_mobile_effect_set_3d_beauty_parts_strength failed");
    }
    return iRet;
}

- (st_result_t)f_set3dBeautyPartsStrength:(st_effect_3D_beauty_part_info_t[])parts andVal:(int)partSize {
    st_result_t iRet = ST_OK;
    iRet = st_mobile_effect_set_3d_beauty_parts_strength(self.handle, parts, partSize);
    if (iRet != ST_OK) {
        NSLog(@"st_mobile_effect_set_3d_beauty_parts_strength failed");
    }
    return iRet;
}

- (st_result_t)disableOverlap:(BOOL)isDisableOverlap {
    st_result_t iRet = ST_OK;
    st_mobile_effect_set_param(self.handle, EFFECT_PARAM_DISABLE_BEAUTY_OVERLAP, isDisableOverlap);
    if (iRet != ST_OK) {
        NSLog(@"st_mobile_effect_set_param EFFECT_PARAM_DISABLE_BEAUTY_OVERLAP failed");
    }
    return iRet;
}

- (st_result_t)disableModuleReorder:(BOOL)isDisableModuleReorder {
    st_result_t iRet = ST_OK;
    st_mobile_effect_set_param(self.handle, EFFECT_PARAM_DISABLE_MODULE_REORDER, isDisableModuleReorder);
    if (iRet != ST_OK) {
        NSLog(@"st_mobile_effect_set_param EFFECT_PARAM_DISABLE_MODULE_REORDER failed");
    }
    return iRet;
}

- (void)setStickerWithPath:(NSString *)stickerPath callBack:(void(^)(st_result_t state, int stickerId, uint64_t action))callback{
    int packageId = 0;
    st_result_t iRet = st_mobile_effect_change_package(self.handle, stickerPath.UTF8String, &packageId);
    if (ST_OK != iRet) {
        NSLog(@"st_mobile_sticker_change_package error %d", iRet);
    }
    uint64_t action = 0;
    st_mobile_effect_get_detect_config(self.handle, &action);
    if (callback) {
        callback(iRet, packageId, action);
    }
}

- (void)setStickerWithPath:(NSString *)stickerPath callBackCustomEventIncluded:(void(^)(st_result_t state, int stickerId, uint64_t action, uint64_t customEvent))callback{
    int packageId = 0;
    st_result_t iRet = st_mobile_effect_change_package(self.handle, stickerPath.UTF8String, &packageId);
    if (ST_OK != iRet) {
        NSLog(@"st_mobile_sticker_change_package error %d", iRet);
    }
    uint64_t action = 0;
    st_mobile_effect_get_detect_config(self.handle, &action);
    
    uint64_t customAciton = 0;
    st_mobile_effect_get_custom_event_config(self.handle, &customAciton);
    
    if (callback) {
        callback(iRet, packageId, action, customAciton);
    }
}

- (void)addStickerWithPath:(NSString *)stickerPath callBack:(void (^)(st_result_t, int, uint64_t))callback{
    if (!stickerPath) {
        return;
    }
    int packageId = 0;
    st_result_t iRet = st_mobile_effect_add_package(self.handle, stickerPath.UTF8String, &packageId);
    if (ST_OK != iRet) {
        NSLog(@"st_mobile_effect_add_package error %d", iRet);
        return;
    }
    uint64_t action = 0;
    st_mobile_effect_get_detect_config(self.handle, &action);
    if (callback) {
        callback(iRet, packageId, action);
    }
}

- (void)addStickerWithPath:(NSString *)stickerPath callBackCustomEventIncluded:(void(^)(st_result_t state, int stickerId, uint64_t action, uint64_t customEvent))callback {
    if (!stickerPath) {
        return;
    }
    int packageId = 0;
    st_result_t iRet = st_mobile_effect_add_package(self.handle, stickerPath.UTF8String, &packageId);
    if (ST_OK != iRet) {
        NSLog(@"st_mobile_effect_add_package error %d", iRet);
        return;
    }
    uint64_t action = 0;
    st_mobile_effect_get_detect_config(self.handle, &action);
    
    uint64_t customAciton = 0;
    st_mobile_effect_get_custom_event_config(self.handle, &customAciton);
    
    if (callback) {
        callback(iRet, packageId, action, customAciton);
    }
}

-(st_result_t)getModulesInPackage:(int)package_id modules:(st_effect_module_info_t*)modules {
    st_effect_package_info_t *p_package_info = malloc(sizeof(st_effect_package_info_t));
    st_result_t iRet = st_mobile_effect_get_package_info(self.handle, package_id, p_package_info);
    if (ST_OK != iRet) {
        NSLog(@"st_mobile_effect_get_package_info error %d", iRet);
        return iRet;
    }
    iRet = st_mobile_effect_get_modules_in_package(self.handle, package_id, modules, p_package_info->module_count);
    free(p_package_info);
    if (ST_OK != iRet) {
        NSLog(@"st_mobile_effect_get_modules_in_package error %d", iRet);
        return iRet;
    }
    return iRet;
}

-(st_result_t)setModuleInfo:(st_effect_module_info_t *)module_info {
    st_result_t iRet = st_mobile_effect_set_module_info(self.handle, module_info);
    if (ST_OK != iRet) {
        NSLog(@"st_mobile_effect_set_module_info error %d", iRet);
    }
    return iRet;
}

-(void)replayStickerWithPackage:(int)packageId {
    st_result_t iRet = st_mobile_effect_replay_package(self.handle, packageId);
    if (ST_OK != iRet) {
        NSLog(@"st_mobile_effect_replay_package error %d", iRet);
        return;
    }
}

- (void)getOverLap:(void (^)(st_effect_beauty_info_t * _Nonnull))callback{
    int beauty_num = 0;
    st_mobile_effect_get_overlapped_beauty_count(self.handle, &beauty_num);
    st_effect_beauty_info_t beauty_info[beauty_num];
    st_mobile_effect_get_overlapped_beauty(self.handle, beauty_info, beauty_num);
    if (beauty_num && callback) {
        callback(beauty_info);
    }
}

- (st_effect_beauty_info_t *)getOverlapInfo:(int *)count{
    int beauty_num = 0;
    st_result_t ret = st_mobile_effect_get_overlapped_beauty_count(self.handle, &beauty_num);
    if (ST_OK != ret || !beauty_num) {
        return nil;
    }
    *count = beauty_num;
    st_effect_beauty_info_t *beauty_info = (st_effect_beauty_info_t *)malloc(sizeof(st_effect_beauty_info_t) * beauty_num);
    ret = st_mobile_effect_get_overlapped_beauty(self.handle, beauty_info, beauty_num);
    if (ST_OK != ret) {
        return  nil;
    }
    return beauty_info;
}


- (st_result_t)removeSticker:(int)stickerId{
    return st_mobile_effect_remove_package(self.handle, stickerId);
}

- (st_result_t)cleareStickers{
    return st_mobile_effect_clear_packages(self.handle);
}

- (GLuint)processTexture:(GLuint)inputTexture
               inputData:(unsigned char*)inputData
             inputFormat:(st_pixel_format)inputFormat
           outputTexture:(GLuint)outputTexture
                   width:(int)width
                  height:(int)height
                  stride:(int)stride
                  rotate:(st_rotate_type)rotate
            detectResult:(st_mobile_human_action_t)detectResult
            animalResult:(st_mobile_animal_face_t const *)animalResult
             animalCount:(int)animalCount
         outDetectResult:(st_mobile_human_action_t)outDetectResult
               withCache:(CVOpenGLESTextureCacheRef)cache
          outPixelFormat:(st_pixel_format)fmt_out
               outBuffer:(unsigned char *)img_out {
    st_mobile_face_mesh_list_t tmp = {};
    return [self processTexture:inputTexture inputData:inputData inputFormat:inputFormat outputTexture:outputTexture width:width height:height stride:stride rotate:rotate detectResult:detectResult animalResult:animalResult animalCount:animalCount outDetectResult:outDetectResult withCache:cache outPixelFormat:fmt_out outBuffer:img_out meshList:tmp];
}

- (GLuint)processTexture:(GLuint)inputTexture
               inputData:(unsigned char*)inputData
             inputFormat:(st_pixel_format)inputFormat
           outputTexture:(GLuint)outputTexture
                   width:(int)width
                  height:(int)height
                  stride:(int)stride
                  rotate:(st_rotate_type)rotate
            detectResult:(st_mobile_human_action_t)detectResult
            animalResult:(st_mobile_animal_face_t const *)animalResult
             animalCount:(int)animalCount
         outDetectResult:(st_mobile_human_action_t)outDetectResult
               withCache:(CVOpenGLESTextureCacheRef)cache
          outPixelFormat:(st_pixel_format)fmt_out
               outBuffer:(unsigned char *)img_out
                meshList:(st_mobile_face_mesh_list_t)mesh_list {
    if (!self.handle) return 0;
    if (self.glContext)[self setCurrentEAGLContext:self.glContext];
    if (self.width != width || self.height != height) {
        [self destoryGLResource];
        self.width = width;
        self.height = height;
    }
    st_handle_t handle = self.handle;
    if (!glIsTexture(outputTexture)) return 0;
    st_result_t ret = ST_OK;
    if (handle) {
        st_effect_custom_param_t inputEvent;
        memset(&inputEvent, 0, sizeof(st_effect_custom_param_t));
        uint64_t type = EFFECT_CUSTOM_NONE;
        //get custom param
        st_mobile_effect_get_custom_param_config(handle, &type);
        if (CHECK_FLAG(type, EFFECT_CUSTOM_CAMERA_QUATION)) {
            if (_type == EffectsTypePreview) {
                CMDeviceMotion *motion = [EffectsMotionManager sharedInstance].motionManager.deviceMotion;
                inputEvent.camera_quat.x = motion.attitude.quaternion.x;
                inputEvent.camera_quat.y = motion.attitude.quaternion.y;
                inputEvent.camera_quat.z = motion.attitude.quaternion.z;
                inputEvent.camera_quat.w = motion.attitude.quaternion.w;
            } else { // 获取默认相机四元数
                st_quaternion_t p_default_quat;
                ret = st_mobile_effect_get_default_camera_quaternion(self.handle, self.cameraPosition == AVCaptureDevicePositionFront, &p_default_quat);
                if (ret != ST_OK) {
                    NSLog(@"st_mobile_effect_get_default_camera_quaternion error %d", ret);
                }
                inputEvent.camera_quat = p_default_quat;
            }
        }
        if(CHECK_FLAG(type, EFFECT_CUSTOM_CAMERA_FACING)){
            inputEvent.front_camera = self.cameraPosition == AVCaptureDevicePositionFront;
        }
        
//        EFGlobalSingleton *globalSingleton = [EFGlobalSingleton sharedInstance];
//        if (globalSingleton.efTouchTriggerAction > 0) {
//            inputEvent.event = globalSingleton.efTouchTriggerAction;
//            globalSingleton.efTouchTriggerAction = 0;
//        }
                
        st_effect_texture_t input_texture = {inputTexture, width, height, ST_PIX_FMT_BGRA8888};
        st_effect_render_in_param_t input_param ={};
        input_param.p_custom_param = &inputEvent;
        input_param.p_human = &detectResult;
        input_param.animal_face_count = animalCount;
        input_param.p_animal_face = animalResult;
        input_param.rotate = rotate;
        input_param.front_rotate = rotate;
        input_param.need_mirror = false;
        input_param.p_tex = &input_texture;
        
        st_image_t inputImage = {.data = inputData, .pixel_format = inputFormat, .width = width, .height = height, .stride = stride, .time_stamp= 0.0};
        st_effect_in_image_t effectImag = {.image = inputImage, .rotate = ST_CLOCKWISE_ROTATE_0, .b_mirror = GL_FALSE};
        input_param.p_image = &effectImag;
        st_effect_texture_t output_texture = {outputTexture, width,  height, ST_PIX_FMT_BGRA8888};
        st_effect_render_out_param_t output_param = {};
        output_param.p_tex = &output_texture;
        st_mobile_human_action_t human_out_param;
        memset(&human_out_param, 0, sizeof(st_mobile_human_action_t));
        output_param.p_human = &human_out_param;
        st_handle_t handle = self.handle;
        EFFECTSTIMELOG(key)
        st_result_t iRet = st_mobile_effect_render(handle, &input_param, &output_param);
        
        st_mobile_human_action_delete(&human_out_param);
        
        if (iRet != ST_OK) {
            NSLog(@"st_mobile_process_texture failed: %d", iRet);
        }
        EFFECTSTIMEPRINT(key, "st_mobile_effect_render")
    }
    if (ret != ST_OK) {
        NSLog(@"st_mobile_beautify_process_texture error %d", ret);
        return 0;
    }
    return outputTexture;
}


- (uint64_t)getDetectConfig{
    uint64_t config;
    st_handle_t handle = self.handle;
    st_mobile_effect_get_detect_config(handle, &config);
    return config;
}

- (uint64_t)getAnimalDetectConfig{
    uint64_t config;
    st_handle_t handle = self.handle;
    st_mobile_effect_get_animal_detect_config(handle, &config);
    return config;
}

- (void)convertYUVBuffer:(unsigned char *)buffer
                    rgba:(GLuint)texture
                    size:(CGSize)size{
    if (!buffer || !glIsTexture(texture) || CGSizeEqualToSize(CGSizeZero, size)) {
        NSLog(@"%s input param error", __func__);
        return;
    }
    st_result_t ret = st_mobile_nv12_buffer_to_rgba_tex(_hConvertHandle, size.width, size.height, ST_CLOCKWISE_ROTATE_0, false, buffer, texture);
    if (ret != ST_OK) {
        NSLog(@"st_mobile_nv12_buffer_to_rgba_tex error %d", ret);
    }
}


#pragma mark - EffectsAudioPlayerManagerDelegate
- (void)audioPlayerDidFinishPlayingWithAudioName:(NSString *)audioName{
    if (self.handle) {
        st_result_t iRet = ST_OK;
        st_effect_module_info_t module_info;
        memset(&module_info, 0, sizeof(st_effect_module_info_t));
        module_info.type = EFFECT_MODULE_SOUND;
        module_info.state = EFFECT_MODULE_PAUSED_LAST_FRAME;
        memcpy(module_info.name, audioName.UTF8String, audioName.length);
        iRet = st_mobile_effect_set_module_info(self.handle, &module_info);
        if (iRet != ST_OK) {
            NSLog(@"st mobile set sound complete str failed: %d", iRet);
        }
    }
}

-(st_result_t)setFaceMeshList:(st_mobile_face_mesh_list_t)mesh_list {
    st_result_t iRet = ST_OK;
    iRet = st_mobile_effect_set_face_mesh_list(self.handle, &mesh_list);
    if (iRet != ST_OK) {
        NSLog(@"st_mobile_effect_set_face_mesh_list error: %d", iRet);
    }
    return iRet;
}
#endif

- (void)setCurrentEAGLContext:(EAGLContext*)glContext{
    if ([EAGLContext currentContext] != glContext) {
        [EAGLContext setCurrentContext:glContext];
    }
}
@end
