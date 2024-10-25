//  BEEffectManager.m
//  Core


#import "BEEffectManager.h"
#if __has_include(<effect-sdk/bef_effect_ai_api.h>)
#import <effect-sdk/bef_effect_ai_api.h>
#import <effect-sdk/bef_effect_ai_message_define.h>
#import <effect-sdk/bef_effect_ai_error_code_format.h>
#import <effect-sdk/bef_effect_ai_version.h>
#endif
#import "BETimeRecoder.h"
#import "Core.h"
#import "BEImageUtils.h"
#import "BEGLUtils.h"

#ifdef EFFECT_LOG_ENABLED
typedef enum {
    BEF_LOG_LEVEL_NONE = 0,
    BEF_LOG_LEVEL_DEFAULT = 1,
    BEF_LOG_LEVEL_VERBOSE  = 2,
    BEF_LOG_LEVEL_DEBUG = 3,
    BEF_LOG_LEVEL_INFO = 4,
    BEF_LOG_LEVEL_WARN = 5,
    BEF_LOG_LEVEL_ERROR = 6,
    BEF_LOG_LEVEL_FATAL = 7,
    BEF_LOG_LEVEL_SILENT = 8,
}bef_log_level;
#if __has_include(<effect-sdk/bef_effect_ai_api.h>)
BEF_SDK_API void bef_effect_set_log_level(bef_effect_handle_t handle, bef_log_level logLevel);
BEF_SDK_API typedef int(*logFileFuncPointer)(int logLevel, const char* msg);
BEF_SDK_API bef_effect_result_t bef_effect_set_log_to_local_func(logFileFuncPointer pfunc);
#endif

int effectLogCallback(int logLevel, const char* msg) {
    printf("[EffectSDK] %s\n", msg);
    return 0;
}
#endif

static const bool USE_PIPELINE = YES;

#define BE_LOAD_RESOURCE_TIMEOUT true

#if __has_include(<effect-sdk/bef_effect_ai_api.h>)
@interface BEEffectManager () <RenderMsgDelegate> {
    bef_effect_handle_t         _handle;
    BOOL                        _effectOn;
    
    IRenderMsgDelegateManager   *_msgDelegateManager;
    bef_ai_face_info            *_faceInfo;
    bef_ai_hand_info            *_handInfo;
    bef_ai_skeleton_result      *_skeletonInfo;
    bef_ai_face_mask_info       *_faceMaskInfo;
    bef_ai_mouth_mask_info      *_mouthMaskInfo;
    bef_ai_teeth_mask_info      *_teethMaskInfo;
//    EAGLContext                 *_glContext;
    
#if BE_LOAD_RESOURCE_TIMEOUT
    NSMutableSet<NSString *>    *_existResourcePathes;
    BOOL                        _needLoadResource;
    BOOL                        _isInitSuccess;
}
#else
}
#endif
@end
#endif

@implementation BEEffectManager

@synthesize usePipeline = _usePipeline;
@synthesize use3buffer = _use3buffer;
@synthesize frontCamera = _frontCamera;

- (instancetype)initWithResourceProvider:(id<BEEffectResourceProvider>)resourceProvider licenseProvider:(id<BELicenseProvider>)licenseProvider {
    self = [super init];
#if __has_include(<effect-sdk/bef_effect_ai_api.h>)
    if (self) {
        _faceInfo = nil;
        _handInfo = nil;
        _skeletonInfo = nil;
        _faceMaskInfo = nil;
        _mouthMaskInfo = nil;
        _teethMaskInfo = nil;
        _usePipeline = YES;
#if BE_LOAD_RESOURCE_TIMEOUT
        _existResourcePathes = [NSMutableSet set];
        _needLoadResource = NO;
        _renderQueue = nil;
#endif
        self.provider = resourceProvider;
        self.licenseProvider = licenseProvider;
    }
#endif
    return self;
}

- (int)initTask {
#if __has_include(<effect-sdk/bef_effect_ai_api.h>)
    _effectOn = true;
    _glContext = [EAGLContext currentContext];  // 运行在主线程，使用的是self.glView.context
    if (_glContext == nil) {
        NSLog(@"initTask is not run in thread with glContext!!!");
        _glContext = [BEGLUtils createContextWithDefaultAPI:kEAGLRenderingAPIOpenGLES3];
    }
    if ([EAGLContext currentContext] != _glContext) {
        [EAGLContext setCurrentContext: _glContext];
    }
    int ret = 0;
    ret = bef_effect_ai_create(&_handle);
    CHECK_RET_AND_RETURN(bef_effect_ai_create, ret)
#ifdef EFFECT_LOG_ENABLED
    bef_effect_set_log_level(_handle, 1);
    bef_effect_set_log_to_local_func(effectLogCallback);
#endif
    if (self.licenseProvider.licenseMode == OFFLINE_LICENSE) {
        ret = bef_effect_ai_check_license(_handle, self.licenseProvider.licensePath);
        CHECK_RET_AND_RETURN(bef_effect_ai_check_license, ret)
        _isSuccessLicense = ret == 0;
    }
    else if (self.licenseProvider.licenseMode == ONLINE_LICENSE){
        if (![self.licenseProvider checkLicenseResult: @"getLicensePath"])
            return self.licenseProvider.errorCode;

        ret = bef_effect_ai_check_online_license(_handle, self.licenseProvider.licensePath);
        CHECK_RET_AND_RETURN(bef_effect_ai_check_online_license, ret)
    }

    [self setUsePipeline:USE_PIPELINE];
    CHECK_RET_AND_RETURN(bef_effect_set_render_api, ret)
    ret = bef_effect_ai_set_render_api(_handle, [self renderAPI]);
    CHECK_RET_AND_RETURN(bef_effect_ai_set_render_api, ret)
    ret = bef_effect_ai_use_builtin_sensor(_handle, YES);
    CHECK_RET_AND_RETURN(bef_effect_ai_use_builtin_sensor, ret)
    ret = bef_effect_ai_init(_handle, 10, 10, self.provider.modelDirPath, "");
    CHECK_RET_AND_RETURN(bef_effect_ai_init, ret)
    
    ret = bef_effect_ai_use_3buffer(_handle, false);
    CHECK_RET_AND_RETURN(bef_effect_ai_use_3buffer, ret);
    
    _msgDelegateManager = [[IRenderMsgDelegateManager alloc] init];
    [self addMsgHandler:self];
    _isInitSuccess = ret == 0;
    return ret;
#else
    return -1;
#endif
}

- (int)destroyTask {
#if __has_include(<effect-sdk/bef_effect_ai_api.h>)
    if ([EAGLContext currentContext] != _glContext) {
        NSLog(@"effectsar init and destroy are not run in the same glContext");
        [EAGLContext setCurrentContext:_glContext];
    }
    [self removeMsgHandler:self];
    bef_effect_ai_destroy(_handle);
    [_msgDelegateManager destoryDelegate];
    _msgDelegateManager = nil;
    free(_faceInfo);
    free(_handInfo);
    free(_skeletonInfo);
    free(_faceMaskInfo);
    free(_mouthMaskInfo);
    free(_teethMaskInfo);
    _isInitSuccess = NO;
    return 0;
#else
    return -1;
#endif
}

#pragma mark - public
#if __has_include(<effect-sdk/bef_effect_ai_api.h>)
- (bef_effect_result_t)processTexture:(GLuint)texture outputTexture:(GLuint)outputTexture width:(int)width height:(int)height rotate:(bef_ai_rotate_type)rotate timeStamp:(double)timeStamp {
    if (!_isInitSuccess) {
        return BEF_RESULT_FAIL;
    }
#if BE_LOAD_RESOURCE_TIMEOUT
    if (_renderQueue) {
        if (_needLoadResource) {
            _needLoadResource = NO;
            [self loadResource:-1];
        }
    }
#endif
    if ([EAGLContext currentContext] != _glContext) {
        NSLog(@"effectsar init and process are not run in the same glContext");
        [EAGLContext setCurrentContext:_glContext];
    }

    RECORD_TIME(totalProcess)
    bef_effect_result_t ret = bef_effect_ai_set_width_height(_handle, width, height);
    CHECK_RET_AND_RETURN(bef_effect_ai_set_width_height, ret)
    ret = bef_effect_ai_set_orientation(_handle, rotate);
    CHECK_RET_AND_RETURN(bef_effect_ai_set_orientation, ret)
    RECORD_TIME(algorithmProcess)
    ret = bef_effect_ai_algorithm_texture(_handle, texture, timeStamp);
    STOP_TIME(algorithmProcess)
    CHECK_RET_AND_RETURN(bef_effect_ai_algorithm_texture, ret)
    RECORD_TIME(effectProcess)
    ret = bef_effect_ai_process_texture(_handle, texture, outputTexture, timeStamp);
    STOP_TIME(effectProcess)
    CHECK_RET_AND_RETURN(bef_effect_ai_process_texture, ret)
    STOP_TIME(totalProcess)
    return ret;
}

- (void) setFilterPath:(NSString *)path {
    if (![self be_empty:path]) {
        path = [self.provider filterPath:path];
    }
    
    bef_effect_result_t status = BEF_RESULT_SUC;
    status = bef_effect_ai_set_color_filter_v2(_handle, [path UTF8String]);

    CHECK_RET_AND_RETURN_RESULT(bef_effect_ai_set_color_filter_v2, status, ;)
}

- (void)setFilterAbsolutePath:(NSString *)path {
    bef_effect_result_t status = BEF_RESULT_SUC;
    status = bef_effect_ai_set_color_filter_v2(_handle, [path UTF8String]);
    
    CHECK_RET_AND_RETURN_RESULT(bef_effect_ai_set_color_filter_v2, status, ;)
}

-(void)setFilterIntensity:(float)intensity {
    bef_effect_result_t status = BEF_RESULT_SUC;
    status = bef_effect_ai_set_intensity(_handle, BEF_INTENSITY_TYPE_GLOBAL_FILTER_V2, intensity);
    
    CHECK_RET_AND_RETURN_RESULT(bef_effect_ai_set_intensity, status, ;)
}
#endif

- (void)setStickerPath:(NSString *)path {
#if __has_include(<effect-sdk/bef_effect_ai_api.h>)
    if (![self be_empty:path]) {
        path = [self.provider stickerPath:path];
    }
    
    bef_effect_result_t status = BEF_RESULT_SUC;
    status = bef_effect_ai_set_effect(_handle, [path UTF8String]);
    
    CHECK_RET_AND_RETURN_RESULT(bef_effect_ai_set_effect, status, ;)
#endif
}

- (void)setStickerAbsolutePath:(NSString*)path
{
#if __has_include(<effect-sdk/bef_effect_ai_api.h>)
    bef_effect_result_t status = BEF_RESULT_SUC;
    status = bef_effect_ai_set_effect(_handle, [path UTF8String]);

    CHECK_RET_AND_RETURN_RESULT(bef_effect_ai_set_effect, status, ;)
#endif
}

- (void)setAvatarPath:(NSString*) path {
#if __has_include(<effect-sdk/bef_effect_ai_api.h>)
    bef_effect_result_t status = BEF_RESULT_SUC;
    status = bef_effect_ai_set_effect(_handle, [path UTF8String]);

    CHECK_RET_AND_RETURN_RESULT(bef_effect_ai_set_effect, status, ;)
#endif
}

#if __has_include(<effect-sdk/bef_effect_ai_api.h>)
- (void)releaseEffectManager {
    bef_effect_ai_destroy(_handle);
}
#endif

- (void)updateComposerNodes:(NSArray<NSString *> *)nodes {
    [self updateComposerNodes:nodes withTags:nil];
}

- (void)updateComposerNodes:(NSArray<NSString *> *)nodes withTags:(NSArray<NSString *> *)tags {
#if __has_include(<effect-sdk/bef_effect_ai_api.h>)
    if (tags != nil && nodes.count != tags.count) {
        NSLog(@"bef_effect_ai_composer_set_nodes error: count of tags must equal to nodes");
        return;
    }
    
#if BE_LOAD_RESOURCE_TIMEOUT
    for (NSString *node in nodes) {
        if (![_existResourcePathes containsObject:node]) {
            _needLoadResource = YES;
            break;
        }
    }
    [_existResourcePathes removeAllObjects];
    [_existResourcePathes addObjectsFromArray:nodes];
#endif

    NSMutableArray<NSString *> *paths = [NSMutableArray arrayWithCapacity:nodes.count];
    for (int i = 0; i < nodes.count; i++) {
        [paths addObject:[self.provider composerNodePath:nodes[i]]];
    }
    nodes = paths;
    
    char **nodesPath = (char **)malloc(nodes.count * sizeof(char *));
    char **nodeTags = NULL;
    if (tags != nil) {
        nodeTags = (char **)malloc(nodes.count * sizeof(char *));
    }
    int count = 0;
    
    NSMutableSet *set = [NSMutableSet set];
    for (int i = 0; i < nodes.count; i++) {
        NSString *node = nodes[i];
        if ([set containsObject:node]) {
            continue;
        }
        [set addObject:node];
        
        if ([node canBeConvertedToEncoding:NSUTF8StringEncoding]) {
            NSUInteger strLength  = [node lengthOfBytesUsingEncoding:NSUTF8StringEncoding];
            nodesPath[count] = (char *)malloc((strLength + 1) * sizeof(char *));
            strncpy(nodesPath[count], [node cStringUsingEncoding:NSUTF8StringEncoding], strLength);
            nodesPath[count][strLength] = '\0';
        }
        
        if (tags != nil) {
            NSString *tag = tags[i];
            NSUInteger strLength = [tag lengthOfBytesUsingEncoding:NSUnicodeStringEncoding];
            nodeTags[count] = (char *)malloc((strLength + 1) * sizeof(char *));
            strncpy(nodeTags[count], [tag cStringUsingEncoding:NSUTF8StringEncoding], strLength);
            nodeTags[count][strLength] = '\0';
        }
        
        count++;
    }
    
    bef_effect_result_t result = BEF_RESULT_SUC;
    if (tags == nil) {
        result = bef_effect_ai_composer_set_nodes(_handle, (const char **)nodesPath, count);
    } else {
        result = bef_effect_ai_composer_set_nodes_with_tags(_handle, (const char **)nodesPath, (const char **)nodeTags, count);
    }
    if (result != BEF_RESULT_SUC) {
        NSLog(@"bef_effect_ai_composer_set_nodes error: %d", result);
    }
    
    for (int i = 0; i < count; i++) {
        free(nodesPath[i]);
        if (tags != nil) {
            free(nodeTags[i]);
        }
    }
    free(nodesPath);
    if (tags != nil) {
        free(nodeTags);
    }

#if BE_LOAD_RESOURCE_TIMEOUT
    if (_renderQueue) {
        dispatch_async(_renderQueue, ^{
            if (self->_needLoadResource) {
                [self loadResource:-1];
                self->_needLoadResource = NO;
            }
        });
    }
#endif
#endif
}

- (void)appendComposerNodes:(NSArray<NSString *> *)nodes {
    [self appendComposerNodes:nodes withTags:nil];
}

- (void)appendComposerNodes:(NSArray<NSString *> *)nodes withTags:(NSArray<NSString *> *)tags {
    if (tags != nil && nodes.count != tags.count) {
        NSLog(@"bef_effect_ai_composer_set_nodes error: count of tags must equal to nodes");
        return;
    }
    
#if __has_include(<effect-sdk/bef_effect_ai_api.h>)
#if BE_LOAD_RESOURCE_TIMEOUT
    for (NSString *node in nodes) {
        if (![_existResourcePathes containsObject:node]) {
            _needLoadResource = YES;
            break;
        }
    }
    [_existResourcePathes addObjectsFromArray:nodes];
#endif
#endif
    NSMutableArray<NSString *> *paths = [NSMutableArray arrayWithCapacity:nodes.count];
    for (int i = 0; i < nodes.count; i++) {
        if ([self.resourcePath isEqualToString:@"sticker"]) {
            [paths addObject:[self.provider stickerPath:nodes[i]]];
        }
        else {
            [paths addObject:[self.provider composerNodePath:nodes[i]]];
        }
        
    }
    nodes = paths;
    
    char **nodesPath = (char **)malloc(nodes.count * sizeof(char *));
    char **nodeTags = NULL;
    if (tags != nil) {
        nodeTags = (char **)malloc(nodes.count * sizeof(char *));
    }
    int count = 0;
    
    NSMutableSet *set = [NSMutableSet set];
    for (int i = 0; i < nodes.count; i++) {
        NSString *node = nodes[i];
        if ([set containsObject:node]) {
            continue;
        }
        [set addObject:node];
        
        if ([node canBeConvertedToEncoding:NSUTF8StringEncoding]) {
            NSUInteger strLength  = [node lengthOfBytesUsingEncoding:NSUTF8StringEncoding];
            nodesPath[count] = (char *)malloc((strLength + 1) * sizeof(char *));
            strncpy(nodesPath[count], [node cStringUsingEncoding:NSUTF8StringEncoding], strLength);
            nodesPath[count][strLength] = '\0';
        }
        
        if (tags != nil) {
            NSString *tag = tags[i];
            NSUInteger strLength = [tag lengthOfBytesUsingEncoding:NSUnicodeStringEncoding];
            nodeTags[count] = (char *)malloc((strLength + 1) * sizeof(char *));
            strncpy(nodeTags[count], [tag cStringUsingEncoding:NSUTF8StringEncoding], strLength);
            nodeTags[count][strLength] = '\0';
        }
        
        count++;
    }
#if __has_include(<effect-sdk/bef_effect_ai_api.h>)
    bef_effect_result_t result = BEF_RESULT_SUC;
    if (tags == nil) {
        result = bef_effect_ai_composer_append_nodes(_handle, (const char **)nodesPath, count);
    } else {
        result = bef_effect_ai_composer_append_nodes_with_tags(_handle, (const char **)nodesPath, (const char **)nodeTags, count);
    }
    if (result != BEF_RESULT_SUC) {
        NSLog(@"bef_effect_ai_composer_set_nodes error: %d", result);
    }
    
    for (int i = 0; i < count; i++) {
        free(nodesPath[i]);
        if (tags != nil) {
            free(nodeTags[i]);
        }
    }
    free(nodesPath);
    if (tags != nil) {
        free(nodeTags);
    }

#if BE_LOAD_RESOURCE_TIMEOUT
    if (_renderQueue) {
        dispatch_async(_renderQueue, ^{
            if (self->_needLoadResource) {
                [self loadResource:-1];
                self->_needLoadResource = NO;
            }
        });
    }
#endif
#endif
}

- (void)removeComposerNodes:(NSArray<NSString *> *)nodes {
#if __has_include(<effect-sdk/bef_effect_ai_api.h>)
#if BE_LOAD_RESOURCE_TIMEOUT
    for (NSString *node in nodes) {
        [_existResourcePathes removeObject:node];
    }
#endif

    NSMutableArray<NSString *> *paths = [NSMutableArray arrayWithCapacity:nodes.count];
    for (int i = 0; i < nodes.count; i++) {
        if ([self.resourcePath isEqualToString:@"sticker"]) {
            [paths addObject:[self.provider stickerPath:nodes[i]]];
        }
        else {
            [paths addObject:[self.provider composerNodePath:nodes[i]]];
        }
    }
    nodes = paths;
    
    char **nodesPath = (char **)malloc(nodes.count * sizeof(char *));
    int count = 0;
    
    NSMutableSet *set = [NSMutableSet set];
    for (int i = 0; i < nodes.count; i++) {
        NSString *node = nodes[i];
        if ([set containsObject:node]) {
            continue;
        }
        [set addObject:node];
        
        if ([node canBeConvertedToEncoding:NSUTF8StringEncoding]) {
            NSUInteger strLength  = [node lengthOfBytesUsingEncoding:NSUTF8StringEncoding];
            nodesPath[count] = (char *)malloc((strLength + 1) * sizeof(char *));
            strncpy(nodesPath[count], [node cStringUsingEncoding:NSUTF8StringEncoding], strLength);
            nodesPath[count][strLength] = '\0';
        }
        
        count++;
    }
    
    bef_effect_result_t result = BEF_RESULT_SUC;
    result = bef_effect_ai_composer_remove_nodes(_handle, (const char **)nodesPath, count);
    if (result != BEF_RESULT_SUC) {
        NSLog(@"bef_effect_ai_composer_set_nodes error: %d", result);
    }
    
    for (int i = 0; i < count; i++) {
        free(nodesPath[i]);
    }
    free(nodesPath);
#endif
}

- (void)updateComposerNodeIntensity:(NSString *)node key:(NSString *)key intensity:(float)intensity {
    
    if ([self.resourcePath isEqualToString:@"sticker"]) {
        node = [self.provider stickerPath:node];
    }
    else {
        node = [self.provider composerNodePath:node];
    }
#if __has_include(<effect-sdk/bef_effect_ai_api.h>)
    bef_effect_result_t result = bef_effect_ai_composer_update_node(_handle, (const char *)[node UTF8String], (const char *)[key UTF8String], intensity);
    CHECK_RET_AND_RETURN_RESULT(bef_effect_ai_composer_update_node, result, ;)
#endif
}

#if __has_include(<effect-sdk/bef_effect_ai_api.h>)
- (NSArray<NSString *> *)availableFeatures {
    //Dynamic lookup feature availability
    int feature_len = 60;
    char features[feature_len][BEF_EFFECT_FEATURE_LEN];
    int *pf = &feature_len;
    int code = bef_effect_available_features(features, pf);
    if (code == BEF_RESULT_SUC) {
        NSMutableArray *array = [NSMutableArray arrayWithCapacity:feature_len];
        for (int i = 0; i < feature_len; i++) {
            [array addObject:[NSString stringWithUTF8String:features[i]]];
        }
        return [array copy];
    } else {
        NSLog(@"dynamic lookup feature availability failed");
        if (code == BEF_RESULT_FAIL) {
             NSLog(@"feature size is more than you expected, there is %d features", feature_len);
        }
        else if (code == BEF_RESULT_INVALID_EFFECT_HANDLE) {
            NSLog(@"bef_effect_available_features must be called after bef_effect_ai_init");
        }
        return @[];
    }
}

- (NSString *)sdkVersion {
    char version[20];
    bef_effect_ai_get_version(version, 20);
    return [NSString stringWithUTF8String:version];
}

- (void)setFrontCamera:(BOOL)frontCamera {
    _frontCamera = frontCamera;
    bef_effect_result_t ret = bef_effect_ai_set_camera_device_position(_handle, frontCamera ? bef_ai_camera_position_front : bef_ai_camera_position_back);
    CHECK_RET_AND_RETURN_RESULT(bef_effect_ai_set_camera_device_position, ret, ;)
}

- (void)setUsePipeline:(BOOL)usePipeline {
    _usePipeline = usePipeline;
    
    bef_effect_result_t ret = bef_effect_ai_use_pipeline_processor(_handle, usePipeline);
    CHECK_RET_AND_RETURN_RESULT(bef_effect_ai_use_pipeline_processor, ret, ;)
}

- (void)setUse3buffer:(BOOL)use3buffer {
    _use3buffer = use3buffer;
    bef_effect_result_t ret = bef_effect_ai_use_3buffer(_handle, use3buffer);
    CHECK_RET_AND_RETURN_RESULT(bef_effect_ai_use_3buffer, ret, ;)
}

- (BOOL)cleanPipeline {
    bef_effect_result_t ret = bef_effect_ai_clean_pipeline_processor_task(_handle);
    CHECK_RET_AND_RETURN_RESULT(bef_effect_ai_clean_pipeline_processor_task, ret, ret == BEF_RESULT_SUC)
    return ret == BEF_RESULT_SUC;
}

- (BOOL)processTouchEvent:(bef_ai_touch_event_code)eventCode x:(float)x y:(float)y force:(float)force majorRadius:(float)majorRadius pointerId:(int)pointerId pointerCount:(int)pointerCount {
    bef_effect_result_t ret = bef_effect_ai_process_touch(_handle, eventCode, x, y, force, majorRadius, pointerId, pointerCount);
    CHECK_RET_AND_RETURN_RESULT(bef_effect_ai_process_touch_event_v2, ret, (ret == BEF_RESULT_SUC))
    return ret == BEF_RESULT_SUC;
}

- (BOOL)processGestureEvent:(bef_ai_gesture_event_code)eventCode x:(float)x y:(float)y dx:(float)dx dy:(float)dy factor:(float)factor {
    bef_effect_result_t ret = bef_effect_ai_process_gesture(_handle, eventCode, x, y, dx, dy, factor);
    CHECK_RET_AND_RETURN_RESULT(bef_effect_ai_process_gesture_event, ret, (ret == BEF_RESULT_SUC))
    return ret == BEF_RESULT_SUC;
}

- (bef_ai_face_info *)getFaceInfo {
    if (_faceInfo == nil) {
        _faceInfo = (bef_ai_face_info *)malloc(sizeof(bef_ai_face_info));
    }
    
    memset(_faceInfo, 0, sizeof(bef_ai_face_info));
    int ret = bef_effect_ai_get_face_detect_result(_handle, _faceInfo);
    if (ret != BEF_RESULT_SUC) {
        NSLog(@"bef_effect_ai_get_face_detect_result error: %d", ret);
        return nil;
    }
    return _faceInfo;
}

- (bef_ai_hand_info *)getHandInfo {
    if (_handInfo == nil) {
        _handInfo = (bef_ai_hand_info *)malloc(sizeof(bef_ai_hand_info));
    }
    
    memset(_handInfo, 0, sizeof(bef_ai_hand_info));
    int ret = bef_effect_ai_get_hand_detect_result(_handle, _handInfo);
    if (ret != BEF_RESULT_SUC) {
        NSLog(@"bef_effect_ai_get_hand_detect_result error: %d", ret);
        return nil;
    }
    return _handInfo;
}

- (bef_ai_skeleton_result *)getSkeletonInfo {
    if (_skeletonInfo == nil) {
        _skeletonInfo = (bef_ai_skeleton_result *)malloc(sizeof(bef_ai_skeleton_result));
    }
    
    memset(_skeletonInfo, 0, sizeof(bef_ai_skeleton_result));
    int ret = bef_effect_ai_get_skeleton_detect_result(_handle, _skeletonInfo);
    if (ret != BEF_RESULT_SUC) {
        NSLog(@"bef_effect_ai_get_skeleton_detect_result error: %d", ret);
        return nil;
    }
    return _skeletonInfo;
}

- (bef_ai_face_mask_info *)getFaceMaskInfo {
    if (_faceMaskInfo == nil) {
        _faceMaskInfo = (bef_ai_face_mask_info *)malloc(sizeof(bef_ai_face_mask_info));
    }
    
    memset(_faceMaskInfo, 0, sizeof(bef_ai_face_mask_info));
    int ret = bef_effect_ai_get_face_seg_result(_handle, BEF_FACE_DETECT_FACE_MASK, _faceMaskInfo);
    if (ret != BEF_RESULT_SUC) {
        NSLog(@"bef_effect_ai_get_face_seg_result fetching mouth seg error: %d ", ret);
        return nil;
    }
    return _faceMaskInfo;
}

- (bef_ai_mouth_mask_info *)getMouthMaskInfo {
    if (_mouthMaskInfo != nil) {
        _mouthMaskInfo = (bef_ai_mouth_mask_info *)malloc(sizeof(bef_ai_mouth_mask_info));
    }
    
    memset(_mouthMaskInfo, 0, sizeof(bef_ai_mouth_mask_info));
    int ret = bef_effect_ai_get_face_seg_result(_handle, BEF_FACE_DETECT_MOUTH_MASK, _mouthMaskInfo);
    if (ret != BEF_RESULT_SUC) {
        NSLog(@"bef_effect_ai_get_face_seg_result fetching teeth seg error: %d", ret);
        return nil;
    }
    return _mouthMaskInfo;
}

- (bef_ai_teeth_mask_info *)getTeethMaskInfo {
    if (_teethMaskInfo == nil) {
        _teethMaskInfo = (bef_ai_teeth_mask_info *)malloc(sizeof(bef_ai_teeth_mask_info));
    }
    
    memset(_teethMaskInfo, 0, sizeof(bef_ai_teeth_mask_info));
    int ret = bef_effect_ai_get_face_seg_result(_handle, BEF_FACE_DETECT_TEETH_MASK, _teethMaskInfo);
    if (ret != BEF_RESULT_SUC) {
        return nil;
    }
    return _teethMaskInfo;
}

- (BOOL)getFaceMaskInfo:(bef_ai_face_mask_info *)faceMaskInfo {
    int ret = bef_effect_ai_get_face_seg_result(_handle, BEF_FACE_DETECT_FACE_MASK, faceMaskInfo);
    if (ret != BEF_RESULT_SUC) {
        NSLog(@"bef_effect_ai_get_face_seg_result fetching face seg error: %d", ret);
    }
    return ret == BEF_RESULT_SUC;
}

- (BOOL)sendMsg:(unsigned int)msgID arg1:(long)arg1 arg2:(long)arg2 arg3:(const char *)arg3{
    int ret = bef_effect_ai_send_msg(_handle, msgID, arg1, arg2, arg3);
    if (ret != BEF_RESULT_SUC) {
        NSLog(@"bef_effect_ai_send_msg return  error: %d", ret);
    }
    return ret == BEF_RESULT_SUC;
}

- (BOOL)setRenderCacheTexture:(NSString *)key path:(NSString *)path {
    int ret = bef_effect_ai_set_render_cache_texture(_handle, [key UTF8String], [path UTF8String]);
    CHECK_RET_AND_RETURN_RESULT(bef_effect_set_render_cache_texture, ret, (ret == BEF_RESULT_SUC));
    return ret;
}

- (BOOL)setRenderCacheTexture:(NSString *)key buffer:(BEBuffer *)buffer {
    bef_ai_image aiImage;
    aiImage.data = buffer.buffer;
    aiImage.width = buffer.width;
    aiImage.height = buffer.height;
    aiImage.stride = buffer.bytesPerRow;
    aiImage.format = 0;
    aiImage.rotate = BEF_AI_CLOCKWISE_ROTATE_0;
    int ret = bef_effect_ai_set_render_cache_texture_with_buffer(_handle, [key UTF8String], &aiImage);
    CHECK_RET_AND_RETURN_RESULT(bef_effect_set_render_cache_texture_with_buffer, ret, (ret == BEF_RESULT_SUC));
    return ret == BEF_RESULT_SUC;
}

- (void)loadResource:(int)timeout {
    bef_effect_ai_load_resource_with_timeout(_handle, timeout);
}

- (void)addMsgHandler:(id<RenderMsgDelegate>)handler
{
    [_msgDelegateManager addDelegate:handler];
}

- (void)removeMsgHandler:(id<RenderMsgDelegate>)handler
{
    [_msgDelegateManager removeDelegate:handler];
}

- (UIImage*)getCapturedImageWithKey:(const char*) key
{
    bef_ai_image* pImage = nullptr;
    int ret = bef_effect_ai_get_captured_image_with_key(_handle, key, &pImage);
    if(ret == BEF_RESULT_SUC && pImage != nullptr)
    {
        BEBuffer* buf = [BEBuffer new];
        buf.buffer = (unsigned char*)pImage->data;
        buf.width = pImage->width;
        buf.height = pImage->height;
        buf.bytesPerRow = pImage->stride;
        buf.format = BE_RGBA;
        BEImageUtils* imageUtils = [BEImageUtils new];
        UIImage* img = [imageUtils transforBufferToUIImage:buf];
        // {zh} 由于img的数据地址与buffer一样，需要深拷贝结果图 {en} Since the data address of img is the same as that of buffer, deep copy of the result graph is required
        UIGraphicsBeginImageContext(img.size);
        [img drawInRect:CGRectMake(0, 0, img.size.width, img.size.height)];
        UIImage *copiedImage = UIGraphicsGetImageFromCurrentImageContext();
        UIGraphicsEndImageContext();
        
        // {zh} 释放贴纸内部buffer {en} Release sticker internal buffer
        bef_effect_ai_release_captured_image(_handle, pImage);
        return copiedImage;
    }
    
    return nil;
}

#pragma mark - RenderMsgDelegate
- (BOOL)msgProc:(unsigned int)unMsgID arg1:(int)nArg1 arg2:(int)nArg2 arg3:(const char *)cArg3 {
    BELog(@"message received, type: %d, arg: %d, %d, %s", unMsgID, nArg1, nArg2, cArg3);
    [self.delegate msgProc:unMsgID arg1:nArg1 arg2:nArg2 arg3:cArg3];
    return NO;
}

#pragma mark - private
- (BOOL)be_empty:(NSString *)s {
    return s == nil || [s isEqualToString:@""];
}

- (bef_ai_render_api_type)renderAPI {
    EAGLContext *context = [EAGLContext currentContext];
    EAGLRenderingAPI api = context.API;
    if (api == kEAGLRenderingAPIOpenGLES2) {
        return bef_ai_render_api_gles20;
    }
    return bef_ai_render_api_gles30;
}

- (BOOL)sethairColorByPart:(BEEffectPart)partIndex r:(CGFloat)r g:(CGFloat)g b:(CGFloat)b a:(CGFloat)a {
    NSDictionary *param = [[NSDictionary alloc] initWithObjectsAndKeys:
                           [NSString stringWithFormat:@"%.3f",r],@"r",
                           [NSString stringWithFormat:@"%.3f",g],@"g",
                           [NSString stringWithFormat:@"%.3f",b],@"b",
                           [NSString stringWithFormat:@"%.3f",a],@"a", nil];
    NSData *jsonData = [NSJSONSerialization dataWithJSONObject:param options:NSJSONWritingPrettyPrinted error:nil];
    NSString *jsonString = [[NSString alloc] initWithData:jsonData encoding:NSUTF8StringEncoding];
    return [self sendMsg:BEEffectHairColor arg1:0 arg2:partIndex arg3:[jsonString UTF8String]];
}

- (BOOL)sendCaptureMessage {
    return [self sendMsg:BEEffectTakingPictures arg1:1 arg2:0 arg3:0];
}

// {zh} / @brief 开启或关闭强制人脸检测 {en} /@brief Enable or disable forced face detection
// {zh} /detection YES 开启人脸检测 NO关闭人脸检测 {en} /detection YES on face detection NO off face detection
- (void)forcedFaceDetection:(BOOL)detection
{
    bef_effect_result_t ret = bef_effect_ai_set_algorithm_force_detect(_handle,detection);
    CHECK_RET_AND_RETURN_RESULT(bef_effect_ai_set_algorithm_force_detect, ret, ;)
}
#endif
@end
