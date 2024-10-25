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
    BEEffectHairColor      = 0x00000044,
    BEEffectTakingPictures = 2200,
};

typedef NS_ENUM(NSInteger, BEEffectPart) {
    BEEffectPart_1      = 1,
    BEEffectPart_2      = 2,
    BEEffectPart_3      = 3,
    BEEffectPart_4      = 4,
    BEEffectPart_5      = 5,   // {zh} 全局染发 {en} Global hair color
    BEEffectPart_6      = 6,   // {zh} 清除染发效果 {en} Clear hair color effect
};

@protocol BEEffectManagerDelegate <NSObject>

- (BOOL)msgProc:(unsigned int)unMsgID arg1:(int)nArg1 arg2:(int)nArg2 arg3:(const char *)cArg3;

@end


@protocol BEEffectResourceProvider <NSObject>

//   {zh} / @brief 模型文件路径     {en} /@Brief model file path
- (const char *)modelDirPath;

//   {zh} / @brief 滤镜路径     {en} /@Brief filter path
//   {zh} / @param filterName 滤镜名称     {en} /@param filterName filter name
- (NSString *)filterPath:(NSString *)filterName;

//   {zh} / @brief 贴纸路径     {en} /@brief sticker path
//   {zh} / @param stickerName 贴纸名称     {en} /@param stickerName sticker name
- (NSString *)stickerPath:(NSString *)stickerName;

//   {zh} / @brief 特效素材路径     {en} /@brief effect material path
//   {zh} / @param nodeName 特效名称     {en} /@param nodeName effect name
- (NSString *)composerNodePath:(NSString *)nodeName;

@end

@interface BEEffectManager : NSObject

@property (nonatomic, assign, setter=setUsePipeline:) BOOL usePipeline;
@property (nonatomic, assign, setter=setUse3Buffer:) BOOL use3buffer;
@property (nonatomic, readonly) NSArray<NSString *> *availableFeatures;
@property (nonatomic, readonly) NSString *sdkVersion;
@property (nonatomic, assign, setter=setFrontCamera:) BOOL frontCamera;
@property (nonatomic, strong) id<BEEffectResourceProvider> provider;
@property (nonatomic, strong) id<BELicenseProvider> licenseProvider;
@property (nonatomic, weak) id<BEEffectManagerDelegate> delegate;
@property (nonatomic, strong) NSString *resourcePath;
@property (atomic, weak) dispatch_queue_t renderQueue;
@property (nonatomic, strong) EAGLContext *glContext;
@property (nonatomic, assign, readonly) BOOL isSuccessLicense;

//   {zh} / @brief 构造函数     {en} /@brief constructor
//   {zh} / @details 需要传入一个 BEEffectResourceProvider 实现，用于提供各种素材的路径，和一个BELicenseProvider的实现，用于获取license     {en} /@details need to pass in a BEEffectResourceProvider implementation to provide the path of various materials, and a BELicenseProvider implementation to get license
//   {zh} / 一般情况下可以直接使用项目中的 BEEffectResourceHelper 。     {en} In general, you can directly use the BEEffectResourceHelper in the project.
//   {zh} / @param provider 特效资源文件获取类     {en} /@param provider effect resource file acquisition class
- (instancetype)initWithResourceProvider:(id<BEEffectResourceProvider>)resourceProvider licenseProvider:(id<BELicenseProvider>)licenseProvider;

#if __has_include(<effect-sdk/bef_effect_ai_api.h>)
//   {zh} / @brief 初始化 SDK     {en} /@brief initialization SDK
- (bef_effect_result_t)initTask;

//   {zh} / @brief SDK 处理     {en} /@brief SDK processing
//   {zh} / @details 只支持 OpenGL 2D 纹理的输入、输出。     {en} /@details only supports the input and output of OpenGL 2D textures.
//   {zh} / @param texture 输入纹理     {en} /@param texture input texture
//   {zh} / @param outputTexture 输出纹理     {en} /@param outputTexture
//   {zh} / @param width 宽     {en} /@param width
//   {zh} / @param height 高     {en} /@param height
//   {zh} / @param rotate 算法检测角度     {en} /@param rotating algorithm detects angle
//   {zh} / @param timeStamp 时间戳     {en} @param timeStamp
- (bef_effect_result_t)processTexture:(GLuint)texture outputTexture:(GLuint)outputTexture width:(int)width height:(int)height rotate:(bef_ai_rotate_type)rotate timeStamp:(double)timeStamp;

//   {zh} / @brief 销毁 SDK     {en} /@brief SDK destruction
- (bef_effect_result_t)destroyTask;

//   {zh} / @brief 设置licenseProvider     {en} /@Briefly set licenseProvider
//   {zh} / @param licenseProvider 传入一个licenseProvider的实现用于license的获取     {en} /@param licenseProvider is a BELicenseProvider implementation to provide the path of license,
#endif

//   {zh} / @brief 设置滤镜路径     {en} /@Briefly set filter path
//   {zh} / @details 相对 FilterResource.bundle/Filter 路径，为 null 时关闭滤镜     {en} /@details Relative to FilterResource .bundle/Filter path, close filter when null
//   {zh} / @param path 相对路径     {en} /@param path relative path
- (void)setFilterPath:(NSString *) path;

// {zh} / @brief 设置滤镜绝对路径 {en} /@Brief Set the absolute path of the filter
// {zh} / @param path 滤镜素材的文件路径，绝对路径 {en} /@Param path The file path of the filter material, absolute path
- (void)setFilterAbsolutePath:(NSString *)path;

//   {zh} / @brief 设置滤镜强度     {en} /@Briefly set filter strength
//   {zh} / @param intensity 滤镜强度，0-1     {en} /@param intensity filter intensity, 0-1
- (void)setFilterIntensity:(float)intensity;

//   {zh} / @brief 设置贴纸路径     {en} /@Briefly set the sticker path
//   {zh} / @details 贴纸素材的文件路径，相对 StickerResource.bundle 路径，为 null 时为关闭贴纸     {en} /@details The file path of the sticker material, relative to the StickerResource.bundle path, is closed when null
//   {zh} / @param path 贴纸路径 relative path of sticker     {en} /@param path sticker path relative path of sticker
- (void)setStickerPath:(NSString*) path;

//   {zh} / @brief 设置贴纸绝对路径     {en} /@Briefly set the absolute path of the sticker
//   {zh} / @details 贴纸素材的文件路径，在 SD 卡上的绝对路径，为 null 是为关闭贴纸     {en} /@details Sticker footage file path, absolute path on SD card, null is for closing sticker
//   {zh} / @param path 贴纸路径 sticker path     {en} /@param path sticker path
- (void)setStickerAbsolutePath:(NSString*)path;

//   {zh} / @brief 设置特效素材     {en} /@Brief set of special effects material
//   {zh} / @details 设置 ComposeMakeup.bundle 下的所有功能，包含美颜、美形、美体、美妆等     {en} /@details Set all functions under ComposeMakeup.bundle, including beauty, shape, body, makeup, etc
//   {zh} / @param nodes 特效素材相对 ComposeMakeup.bundle/ComposeMakeup 的路径     {en} /@Param nodes Effect material relative to the path of ComposeMakeup.bundle/ComposeMakeup
- (void)updateComposerNodes:(NSArray<NSString *> *)nodes;

//   {zh} / @brief 设置特效素材     {en} /@Brief set of special effects material
//   {zh} / @details 设置 ComposeMakeup.bundle 下的所有功能，包含美颜、美形、美体、美妆等     {en} /@details Set all functions under ComposeMakeup.bundle, including beauty, shape, body, makeup, etc
//   {zh} / @param nodes 特效素材相对 ComposeMakeup.bundle/ComposeMakeup 的路径     {en} /@Param nodes Effect material relative to the path of ComposeMakeup.bundle/ComposeMakeup
//   {zh} / @param tags 每一个特效素材对应一个 tag，tag 会传递给 SDK 素材的的一些配置     {en} /@Param tags Each special effect material corresponds to a tag, and the tag will be passed to some configurations of the SDK material
- (void)updateComposerNodes:(NSArray<NSString *> *)nodes withTags:(NSArray<NSString *> *)tags;

//   {zh} / @brief 添加特效素材     {en} /@Brief add special effects material
//   {zh} / @details 在已有素材的基础上增加素材     {en} /@details Add material to existing material
//   {zh} / @param nodes 特效素材相对 ComposeMakeup.bundle/ComposeMakeup 的路径     {en} /@Param nodes Effect material relative to the path of ComposeMakeup.bundle/ComposeMakeup
- (void)appendComposerNodes:(NSArray<NSString *> *)nodes;

//   {zh} / @brief 添加特效素材     {en} /@Brief add special effects material
//   {zh} / @details 在已有素材的基础上增加素材     {en} /@details Add material to existing material
//   {zh} / @param nodes 特效素材相对 ComposeMakeup.bundle/ComposeMakeup 的路径     {en} /@Param nodes Effect material relative to the path of ComposeMakeup.bundle/ComposeMakeup
//   {zh} / @param tags 每一个特效素材对应一个 tag，tag 会传递给 SDK 素材的的一些配置     {en} /@Param tags Each special effect material corresponds to a tag, and the tag will be passed to some configurations of the SDK material
- (void)appendComposerNodes:(NSArray<NSString *> *)nodes withTags:(NSArray<NSString *> *)tags;

//   {zh} / @brief 移除特效素材     {en} /@Briefly remove special effects material
//   {zh} / @param nodes 特效素材相对 ComposeMakeup.bundle/ComposeMakeup 的路径     {en} /@Param nodes Effect material relative to the path of ComposeMakeup.bundle/ComposeMakeup
- (void)removeComposerNodes:(NSArray<NSString *> *)nodes;

//   {zh} / @brief 更新组合特效中某个功能的强度     {en} /@Briefly update the intensity of a function in the combination effect
//   {zh} / @param node 特效素材相对于 ComposeMakeup.bundle/ComposeMakeup 的路径     {en} /@Param node effect material relative to the path of ComposeMakeup.bundle/ComposeMakeup
//   {zh} / @param key 素材中的功能 key     {en} /@param key function key in the material
//   {zh} / @param intensity 强度 0-1     {en} /@param intensity 0-1
- (void)updateComposerNodeIntensity:(NSString *)node key:(NSString *)key intensity:(float)intensity;

// {zh} / @brief 处理触摸事件 {en} Handle touch events briefly
// {zh} / @param eventCode 触摸事件类型 {en} /@param eventCode touch event type
// {zh} / @param x 触摸位置 {en} @Param x touch position
// {zh} / @param y 触摸位置 {en} @Param y touch position
// {zh} / @param force 压力值 {en} @Param force
// {zh} / @param majorRadius 触摸范围 {en} @param majorRadius touch range
// {zh} / @param pointerId 触摸点 id {en} /@param pointerId touch point id
// {zh} / @param pointerCount 触摸点数量 {en} @param pointerCount number of touch points
#if __has_include(<effect-sdk/bef_effect_ai_api.h>)
- (BOOL)processTouchEvent:(bef_ai_touch_event_code)eventCode x:(float)x y:(float)y force:(float)force majorRadius:(float)majorRadius pointerId:(int)pointerId pointerCount:(int)pointerCount;

// {zh} / @brief 处理手势事件 {en} Handle gesture events briefly
// {zh} / @param eventCode 手势事件类型 {en} /@param eventCode Gesture event type
// {zh} / @param x 触摸位置 {en} @Param x touch position
// {zh} / @param y 触摸位置 {en} @Param y touch position
// {zh} / @param dx 移动距离 {en} @Param dx moving distance
// {zh} / @param dy 移动距离 {en} @Param dy moving distance
// {zh} / @param factor 缩放因数 {en} /@param factor scaling factor
- (BOOL)processGestureEvent:(bef_ai_gesture_event_code)eventCode x:(float)x y:(float)y dx:(float)dx dy:(float)dy factor:(float)factor;

//   {zh} / @brief 获取特效 SDK 中的人脸检测结果     {en} /@Brief Get the face detection results in the special effects SDK
- (bef_ai_face_info *)getFaceInfo;

//   {zh} / @brief 获取特效 SDK 中的手势检测结果     {en} /@Brief Get gesture detection results in special effects SDK
- (bef_ai_hand_info *)getHandInfo;

//   {zh} / @brief 获取特效 SDK 中的人体检测结果     {en} /@Briefly get the human detection results in the special effects SDK
- (bef_ai_skeleton_result *)getSkeletonInfo;

//   {zh} / @brief 获取特效 SDK 中的嘴唇 mask 结果     {en} /@Brief Get lip mask results in special effects SDK
- (bef_ai_mouth_mask_info *)getMouthMaskInfo;

//   {zh} / @brief 获取特效 SDK 中的牙齿 mask 结果     {en} /@Briefly get the results of tooth masks in the special effects SDK
- (bef_ai_teeth_mask_info *)getTeethMaskInfo;

//   {zh} / @brief 获取特效 SDK 中的人脸 mask 结果     {en} /@Brief Get the face mask results in the special effects SDK
- (bef_ai_face_mask_info *)getFaceMaskInfo;
#endif

//   {zh} / @brief 是否开启并行渲染     {en} /@Brief whether to turn on parallel rendering
//   {zh} / @details 特效 SDK 内部工作分为两部分，算法检测和特效渲染，当开启并行渲染之后，     {en} /@Details The internal work of the special effects SDK is divided into two parts, algorithm detection and special effects rendering. When parallel rendering is turned on,
//   {zh} / 算法检测和特效渲染将在不同线程执行，以充分利用多多线程进行加速，     {en} /Algorithm detection and effects rendering will be performed on different threads to make full use of multi-threads for acceleration,
//   {zh} / 但会导致渲染效果延迟一帧     {en} /But will cause the rendering effect to be delayed by one frame
//   {zh} / @param usePipeline 是否开启并行渲染     {en} /@param usePipeline whether to turn on parallel rendering
- (void)setUsePipeline:(BOOL)usePipeline;

//   {zh} / @brief 是否开启 3-buffer     {en} /@Brief whether to open 3-buffer
//   {zh} / @details 当开启并行渲染之后，由于算法和特效在不同线程执行，所以需要一些线程同步的工作。     {en} /@Details When parallel rendering is turned on, some thread synchronization work is required because the algorithm and special effects are executed in different threads.
//   {zh} / 当不开启 3buffer 的时候，SDK 会将传进来的每一帧进行拷贝，     {en} /When 3buffer is not turned on, the SDK will copy every frame passed in,
//   {zh} / 当开启 3buffer 的时候，SDK 不会拷贝每一帧纹理，要求外部传进来的纹理是一个循环的队列，     {en} /When 3buffer is turned on, the SDK does not copy every frame of texture, requiring that the external incoming texture is a circular queue,
//   {zh} / 即连续的 3 帧纹理 ID 不能相同     {en} /That is, the texture ID cannot be the same for 3 consecutive frames
//   {zh} / @param use3buffer 是否开启 3buffer     {en} /@param use3buffer whether to open 3buffer
- (void)setUse3buffer:(BOOL)use3buffer;

//   {zh} / @brief 清空并行渲染缓存     {en} /@Brief Clear the parallel rendering cache
//   {zh} / @details 当切换摄像头、暂停或其他会导致画面不连续的情况下调用，     {en} /@details When switching cameras, pausing, or other situations that will cause discontinuity of the picture,
//   {zh} / 清空并行渲染中的缓存。     {en} /Empty the cache in parallel rendering.
- (BOOL)cleanPipeline;

//   {zh} / @brief 设置相机位置     {en} /@Briefly set camera position
/// set camera position
//   {zh} / @param frontCamera 是否为前置摄像头 whether it is front camera     {en} /@param frontCamera whether it is front camera
- (void)setFrontCamera:(BOOL)frontCamera;

//   {zh} / @brief 通过文件设置 render cache texture     {en} /@Briefing Set render cache texture by file
//   {zh} / @details 传入一个固定名字的纹理给到 SDK，传入图片路径，SDK 会将其解析成纹理     {en} /@details pass a texture with a fixed name to the SDK, pass in the picture path, and the SDK will parse it into a texture
//   {zh} / @param key 纹理名称     {en} /@param key texture name
//   {zh} / @param path 图像路径     {en} /@param path image path
- (BOOL)setRenderCacheTexture:(NSString *)key path:(NSString *)path;

//   {zh} / @brief 通过 buffer 设置 render cache texture     {en} /@Briefing render cache texture via buffer settings
//   {zh} / @details 传入一个固定名字的纹理给到 SDK，传入 BEBuffer，SDK 会将其解析成纹理     {en} /@details pass a texture with a fixed name to the SDK, pass BEBuffer, and the SDK will parse it into a texture
//   {zh} / @param key 纹理名称     {en} /@param key texture name
//   {zh} / @param buffer BEBuffer, 仅支持 RGBA 格式     {en} /@param buffer BEBuffer, only supports RGBA format
#if __has_include(<effect-sdk/bef_effect_ai_api.h>)
- (BOOL)setRenderCacheTexture:(NSString *)key buffer:(BEBuffer *)buffer;
#endif

- (void)loadResource:(int)timeout;

//   {zh} / @brief 染发向贴纸发送消息     {en} /@Briefing Send a message to the sticker with hair dye
//   {zh} / @param partIndex 部位     {en} /@param parts
//   {zh} / @param r 色值参数     {en} /@param Color value parameter
//   {zh} / @param g 色值参数     {en} /@param Color value parameter
//   {zh} / @param b 色值参数     {en} /@param Color value parameter
//   {zh} / @param a 色值参数     {en} /@param Color value parameter
- (BOOL)sethairColorByPart:(BEEffectPart)partIndex r:(CGFloat)r g:(CGFloat)g b:(CGFloat)b a:(CGFloat)a;

//   {zh} / @brief 拍照向贴纸发送消息     {en} /@Briefing Take pictures and send messages to stickers
- (BOOL)sendCaptureMessage;

- (UIImage*)getCapturedImageWithKey:(const char*) key;

// {zh} / @brief 开启或关闭强制人脸检测 {en} /@brief Enable or disable forced face detection
// {zh} /detection YES 开启人脸检测 NO关闭人脸检测 {en} /detection YES on face detection NO off face detection
- (void)forcedFaceDetection:(BOOL)detection;

@end

#endif /* BEEffectManager_h */
