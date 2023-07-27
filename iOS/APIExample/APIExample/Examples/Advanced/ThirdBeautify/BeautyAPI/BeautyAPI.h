//
//  BeautyAPI.h
//  BeautyAPi
//
//  Created by zhaoyongqiang on 2023/5/31.
//

#import <Foundation/Foundation.h>
#import "BeautyConfig.h"

NS_ASSUME_NONNULL_BEGIN

typedef NS_ENUM(NSInteger, BeautyPresetMode) {
    /// 默认美颜参数
    BeautyPresetModeDefault = 0,
    /// 外部自定义美颜参数, 外部自行设置
    BeautyPresetModeCustom = 1
};

@interface BeautyAPI : NSObject

/**
 * 是否是前置摄像头
 * 切换摄像头要给此属性赋值,  解决镜像问题
 **/
@property (nonatomic, assign) BOOL isFrontCamera;

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
- (int)destory;

@end

NS_ASSUME_NONNULL_END
