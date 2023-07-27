//
//  BeautyConfig.h
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

@protocol BeautyRenderDelegate <NSObject>

- (void)setBeautyPreset;

- (CVPixelBufferRef)onCapture: (CVPixelBufferRef)pixelBuffer;
#if __has_include(<AgoraRtcKit/AgoraRtcKit.h>)
- (AgoraVideoFormat)getVideoFormatPreference;
#endif

- (void)setMakeup: (BOOL)isSelected;

- (void)setSticker: (BOOL)isSelected;

- (void)reset;

- (void)destory;

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

@end

NS_ASSUME_NONNULL_END
