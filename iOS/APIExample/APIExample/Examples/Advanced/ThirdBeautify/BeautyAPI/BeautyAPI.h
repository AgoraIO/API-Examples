//
//  BeautyAPI.h
//  BeautyAPi
//
//  Created by zhaoyongqiang on 2023/5/31.
//

#import <Foundation/Foundation.h>
#if __has_include(<AgoraRtcKit/AgoraRtcKit.h>)
#import <AgoraRtcKit/AgoraRtcKit.h>
#else
#import <AVFoundation/AVFoundation.h>
#endif

NS_ASSUME_NONNULL_BEGIN

typedef NS_ENUM(NSInteger, BeautyPresetMode) {
    /// 默认美颜参数
    BeautyPresetModeDefault = 0,
    /// 外部自定义美颜参数, 外部自行设置
    BeautyPresetModeCustom = 1
};

@protocol BeautyRenderDelegate <NSObject>

- (CVPixelBufferRef)onCapture: (CVPixelBufferRef)pixelBuffer;
#if __has_include(<AgoraRtcKit/AgoraRtcKit.h>)
- (AgoraVideoFormat)getVideoFormatPreference;
#endif

- (void)destroy;

@optional
- (void)setBeautyPreset;

- (void)setMakeup: (BOOL)isSelected;

- (void)setSticker: (BOOL)isSelected;

- (void)reset;

@end


typedef NS_ENUM(NSInteger, CaptureMode) {
    CaptureModeAgora = 0,
    CaptureModeCustom = 1
};

@interface BeautyStats : NSObject
/// 美颜最小耗时
@property (nonatomic, assign)double minCostMs;
/// 美颜最大耗时
@property (nonatomic, assign)double maxCostMs;
/// 美颜平均耗时
@property (nonatomic, assign)double averageCostMs;

@end

typedef NS_ENUM(NSInteger, MirrorMode) {
    /// 本地远端都镜像，前置默认
    MirrorMode_LOCAL_REMOTE = 0,
    /// 仅本地镜像，远端不镜像，用于打电话场景，电商直播场景(保证电商直播后面的告示牌文字是正的)；这种模式因为本地远端是反的，所以肯定有一边的文字贴纸方向会是反的
    MirrorMode_LOCAL_ONLY = 1,
    /// 仅远端镜像
    MirrorMode_REMOTE_ONLY= 2,
    /// 本地远端都不镜像，后置默认
    MirrorMode_NONE
};

@interface CameraConfig : NSObject
// 前置默认镜像
@property(nonatomic, assign) MirrorMode frontMirror;
// 后置默认镜像
@property(nonatomic, assign) MirrorMode backMirror;
@end

@interface BeautyConfig : NSObject
#if __has_include(<AgoraRtcKit/AgoraRtcKit.h>)
// 由外部传入的rtc对象，不可为空
@property(nonatomic, weak)AgoraRtcEngineKit *rtcEngine;
#endif
// 由外部传入的美颜SDK接口对象(不同厂家不一样)，不可为空
@property(nonatomic, weak)id<BeautyRenderDelegate>beautyRender;
// 是否由内部自动注册祼数据回调处理
@property(nonatomic, assign)CaptureMode captureMode;
// 事件回调，包含美颜耗时
@property(nonatomic, copy)void (^eventCallback)(BeautyStats *stats);
// 统计区间 单位: 秒 默认: 1s
@property(nonatomic, assign)NSInteger statsDuration;
// 是否开启统计
@property(nonatomic, assign)BOOL statsEnable;
// 配置摄像头镜像
@property(nonatomic, strong)CameraConfig *cameraConfig;

@end

@interface BeautyAPI : NSObject

/**
 *  Render
 **/
@property (nonatomic, weak) id<BeautyRenderDelegate>beautyRender;

/**
 * 创建并初始化美颜场景化API，如果外部调用过registerVideoFrameObserver，那create必须在此之后调用
 *
 * @param config 配置
 *
 * @return 0: 成功, 非0: 见错误码
 **/
- (int)initialize: (BeautyConfig *)config;

/**
 * 美颜开关
 *
 * @param enable 是否打开美颜
 *
 * @return 0: 成功, 非0: 见错误码
 **/
- (int)enable: (BOOL)enable;

/**
 * 读取美颜开关状态
 **/
@property (nonatomic, readonly, assign) BOOL isEnable;

/**
 * 是否是前置摄像头
 **/
@property (nonatomic, assign, readonly) BOOL isFrontCamera;

/**
 * 切换摄像头
 *
 *
 * @return 0: 成功；非0：见错误码
 **/
- (int)switchCamera;

/**
 * 设置摄像头镜像模式，注意前置和后置要单独控制
 *
 *
 * @return 0: 成功；非0：见错误码
 **/
- (int)updateCameraConfig: (CameraConfig *)cameraConfig;

/**
 * 本地视图渲染，由内部来处理镜像问题
 *
 * @param view 渲染视图
 * @param renderMode 渲染缩放模式
 * @return 0: 成功, 非0: 见错误码
 **/
#if __has_include(<AgoraRtcKit/AgoraRtcKit.h>)
- (int)setupLocalVideo: (UIView *)view renderMode: (AgoraVideoRenderMode)renderMode;
#endif

/**
 * 镜像处理方法，当useCustom为true时才需要调用
 *
 * @return 采集时是否需要镜像
 **/
- (BOOL)getMirrorApplied;
/**
 * 美颜处理方法，当useCustom为true时才需要调用，否则会报错
 *
 *
 * @return 0: 成功；非0：见错误码
 **/
- (int)onFrame: (CVPixelBufferRef)pixelBuffer callback: (void (^)(CVPixelBufferRef))callback;

/**
 * 设置美颜最佳默认参数
 *
 * @param mode 美颜参数模式
 *
 * @return 0: 成功；非0：见错误码
 **/
- (int)setBeautyPreset: (BeautyPresetMode)mode;

/**
 * 销毁美颜场景化API。
 * 当创建useCustome=true时，会调用rtcEngine.registerVideoFrameObserver(null)将祼数据回调解绑。
 *
 * @return 0: 成功；非0: 见错误码表
 **/
- (int)destroy;

/**
 * @return 版本号
 **/
- (NSString *)getVersion;

@end

typedef NS_ENUM(NSInteger, LogLevel) {
    LogLevelInfo,
    LogLevelError,
    LogLevelDebug
};
@interface LogUtil : NSObject

+ (void)log:(NSString *)message;

+ (void)log:(NSString *)message level:(LogLevel)level;

@end

NS_ASSUME_NONNULL_END
