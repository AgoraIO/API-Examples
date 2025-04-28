//  BEEffectManager.h
// EffectsARSDK

#ifndef BEEffectManager_h
#define BEEffectManager_h

#import <Foundation/Foundation.h>
#import <OpenGLES/ES2/glext.h>
#import <UIKit/UIKit.h>
#import "BELicenseHelper.h"
#if __has_include(<effect-sdk/bef_effect_ai_api.h>)
#import <effect-sdk/bef_effect_ai_api.h>
#import <effect-sdk/bef_effect_ai_message_define.h>
#endif
#import "BEImageUtils.h"

typedef NS_ENUM(int, BEEffectSendMsg) {
    BEEffectHairColor = 0x00000044,
    BEEffectTakingPictures = 2200,
};

typedef NS_ENUM(NSInteger, BEEffectPart) {
    BEEffectPart_1 = 1,
    BEEffectPart_2 = 2,
    BEEffectPart_3 = 3,
    BEEffectPart_4 = 4,
    BEEffectPart_5 = 5, // Global hair color
    BEEffectPart_6 = 6, // Clear hair color effect
};

@protocol BEEffectManagerDelegate <NSObject>

- (BOOL)msgProc:(unsigned int)unMsgID arg1:(int)nArg1 arg2:(int)nArg2 arg3:(const char *)cArg3;

@end

@protocol BEEffectResourceProvider <NSObject>

// @brief model file path
- (const char *)modelDirPath;

// @brief filter path
// @param filterName filter name
- (NSString *)filterPath:(NSString *)filterName;

// @brief sticker path
// @param stickerName sticker name
- (NSString *)stickerPath:(NSString *)stickerName;

// @brief effect material path
// @param nodeName effect name
- (NSString *)composerNodePath:(NSString *)nodeName;

@end

@interface BEEffectManager : NSObject

@property(nonatomic, assign, setter=setUsePipeline:) BOOL usePipeline;
@property(nonatomic, assign, setter=setUse3Buffer:) BOOL use3buffer;
@property(nonatomic, readonly) NSArray<NSString *> *availableFeatures;
@property(nonatomic, readonly) NSString *sdkVersion;
@property(nonatomic, assign, setter=setFrontCamera:) BOOL frontCamera;
@property(nonatomic, strong) id<BEEffectResourceProvider> provider;
@property(nonatomic, strong) id<BELicenseProvider> licenseProvider;
@property(nonatomic, weak) id<BEEffectManagerDelegate> delegate;
@property(nonatomic, strong) NSString *resourcePath;
@property(atomic, weak) dispatch_queue_t renderQueue;
@property(nonatomic, strong) EAGLContext *glContext;
@property(nonatomic, assign, readonly) BOOL isSuccessLicense;

// @brief constructor
// @details need to pass in a BEEffectResourceProvider implementation to provide the path of various materials, and a BELicenseProvider implementation to get license
// In general, you can directly use the BEEffectResourceHelper in the project.
// @param provider effect resource file acquisition class
- (instancetype)initWithResourceProvider:(id<BEEffectResourceProvider>)resourceProvider licenseProvider:(id<BELicenseProvider>)licenseProvider;

#if __has_include(<effect-sdk/bef_effect_ai_api.h>)
// @brief initialization SDK
- (bef_effect_result_t)initTask;

// @brief SDK processing
// @details only supports the input and output of OpenGL 2D textures.
// @param texture input texture
// @param outputTexture
// @param width
// @param height
// @param rotate algorithm detects angle
// @param timeStamp
- (bef_effect_result_t)processTexture:(GLuint)texture outputTexture:(GLuint)outputTexture width:(int)width height:(int)height rotate:(bef_ai_rotate_type)rotate timeStamp:(double)timeStamp;

// @brief SDK destruction
- (bef_effect_result_t)destroyTask;

// @brief set licenseProvider
// @param licenseProvider is a BELicenseProvider implementation to provide the path of license,
#endif

// @brief set filter path
// @details Relative to FilterResource .bundle/Filter path, close filter when null
// @param path relative path
- (void)setFilterPath:(NSString *)path;

// @brief Set the absolute path of the filter
// @param path The file path of the filter material, absolute path
- (void)setFilterAbsolutePath:(NSString *)path;

// @brief set filter strength
// @param intensity filter intensity, 0-1
- (void)setFilterIntensity:(float)intensity;

// @brief set the sticker path
// @details The file path of the sticker material, relative to the StickerResource.bundle path, is closed when null
// @param path sticker path relative path of sticker
- (void)setStickerPath:(NSString *)path;

// @brief set the absolute path of the sticker
// @details Sticker footage file path, absolute path on SD card, null is for closing sticker
// @param path sticker path
- (void)setStickerAbsolutePath:(NSString *)path;

// @brief set of special effects material
// @details Set all functions under ComposeMakeup.bundle, including beauty, shape, body, makeup, etc
// @param nodes Effect material relative to the path of ComposeMakeup.bundle/ComposeMakeup
- (void)updateComposerNodes:(NSArray<NSString *> *)nodes;

// @brief set of special effects material
// @details Set all functions under ComposeMakeup.bundle, including beauty, shape, body, makeup, etc
// @param nodes Effect material relative to the path of ComposeMakeup.bundle/ComposeMakeup
// @param tags Each special effect material corresponds to a tag, and the tag will be passed to some configurations of the SDK material
- (void)updateComposerNodes:(NSArray<NSString *> *)nodes withTags:(NSArray<NSString *> *)tags;

// @brief add special effects material
// @details Add material to existing material
// @param nodes Effect material relative to the path of ComposeMakeup.bundle/ComposeMakeup
- (void)appendComposerNodes:(NSArray<NSString *> *)nodes;

// @brief add special effects material
// @details Add material to existing material
// @param nodes Effect material relative to the path of ComposeMakeup.bundle/ComposeMakeup
// @param tags Each special effect material corresponds to a tag, and the tag will be passed to some configurations of the SDK material
- (void)appendComposerNodes:(NSArray<NSString *> *)nodes withTags:(NSArray<NSString *> *)tags;

// @brief remove special effects material
// @param nodes Effect material relative to the path of ComposeMakeup.bundle/ComposeMakeup
- (void)removeComposerNodes:(NSArray<NSString *> *)nodes;

// @brief update the intensity of a function in the combination effect
// @param node effect material relative to the path of ComposeMakeup.bundle/ComposeMakeup
// @param key function key in the material
// @param intensity 0-1
- (void)updateComposerNodeIntensity:(NSString *)node key:(NSString *)key intensity:(float)intensity;

// @brief Handle touch events
// @param eventCode touch event type
// @param x touch position
// @param y touch position
// @param force
// @param majorRadius touch range
// @param pointerId touch point id
// @param pointerCount number of touch points
#if __has_include(<effect-sdk/bef_effect_ai_api.h>)
- (BOOL)processTouchEvent:(bef_ai_touch_event_code)eventCode x:(float)x y:(float)y force:(float)force majorRadius:(float)majorRadius pointerId:(int)pointerId pointerCount:(int)pointerCount;

// @brief Handle gesture events
// @param eventCode Gesture event type
// @param x touch position
// @param y touch position
// @param dx moving distance
// @param dy moving distance
// @param factor scaling factor
- (BOOL)processGestureEvent:(bef_ai_gesture_event_code)eventCode x:(float)x y:(float)y dx:(float)dx dy:(float)dy factor:(float)factor;

// @brief Get the face detection results in the special effects SDK
- (bef_ai_face_info *)getFaceInfo;

// @brief Get gesture detection results in special effects SDK
- (bef_ai_hand_info *)getHandInfo;

// @brief get the human detection results in the special effects SDK
- (bef_ai_skeleton_result *)getSkeletonInfo;

// @brief Get lip mask results in special effects SDK
- (bef_ai_mouth_mask_info *)getMouthMaskInfo;

// @brief get the results of tooth masks in the special effects SDK
- (bef_ai_teeth_mask_info *)getTeethMaskInfo;

// @brief Get the face mask results in the special effects SDK
- (bef_ai_face_mask_info *)getFaceMaskInfo;
#endif

// @brief whether to turn on parallel rendering
// @details The internal work of the special effects SDK is divided into two parts, algorithm detection and special effects rendering. When parallel rendering is turned on,
// Algorithm detection and effects rendering will be performed on different threads to make full use of multi-threads for acceleration,
// But will cause the rendering effect to be delayed by one frame
// @param usePipeline whether to turn on parallel rendering
- (void)setUsePipeline:(BOOL)usePipeline;

// @brief whether to open 3-buffer
// @details When parallel rendering is turned on, some thread synchronization work is required because the algorithm and special effects are executed in different threads.
// When 3buffer is not turned on, the SDK will copy every frame passed in,
// When 3buffer is turned on, the SDK does not copy every frame of texture, requiring that the external incoming texture is a circular queue,
// That is, the texture ID cannot be the same for 3 consecutive frames
// @param use3buffer whether to open 3buffer
- (void)setUse3buffer:(BOOL)use3buffer;

// @brief Clear the parallel rendering cache
// @details When switching cameras, pausing, or other situations that will cause discontinuity of the picture,
// Empty the cache in parallel rendering.
- (BOOL)cleanPipeline;

// @brief set camera position
/// set camera position
// @param frontCamera whether it is front camera
- (void)setFrontCamera:(BOOL)frontCamera;

// @brief Set render cache texture by file
// @details pass a texture with a fixed name to the SDK, pass in the picture path, and the SDK will parse it into a texture
// @param key texture name
// @param path image path
- (BOOL)setRenderCacheTexture:(NSString *)key path:(NSString *)path;

// @brief render cache texture via buffer settings
// @details pass a texture with a fixed name to the SDK, pass BEBuffer, and the SDK will parse it into a texture
// @param key texture name
// @param buffer BEBuffer, only supports RGBA format
#if __has_include(<effect-sdk/bef_effect_ai_api.h>)
- (BOOL)setRenderCacheTexture:(NSString *)key buffer:(BEBuffer *)buffer;
#endif

- (void)loadResource:(int)timeout;

// @brief Send a message to the sticker with hair dye
// @param parts
// @param Color value parameter
// @param Color value parameter
// @param Color value parameter
// @param Color value parameter
- (BOOL)sethairColorByPart:(BEEffectPart)partIndex r:(CGFloat)r g:(CGFloat)g b:(CGFloat)b a:(CGFloat)a;

// @brief Take pictures and send messages to stickers
- (BOOL)sendCaptureMessage;

- (UIImage *)getCapturedImageWithKey:(const char *)key;

// @brief Enable or disable forced face detection
// detection YES on face detection NO off face detection
- (void)forcedFaceDetection:(BOOL)detection;

@end

#endif /* BEEffectManager_h */
