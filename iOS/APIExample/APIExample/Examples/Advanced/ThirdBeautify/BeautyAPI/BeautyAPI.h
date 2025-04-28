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
    /// Default beauty parameters
    BeautyPresetModeDefault = 0,
    /// Custom beauty parameters, set by external
    BeautyPresetModeCustom = 1
};

@protocol BeautyRenderDelegate <NSObject>

- (CVPixelBufferRef)onCapture:(CVPixelBufferRef)pixelBuffer;
#if __has_include(<AgoraRtcKit/AgoraRtcKit.h>)
- (AgoraVideoFormat)getVideoFormatPreference;
#endif

- (void)destroy;

@optional
- (void)setBeautyPreset;

- (void)setMakeup:(BOOL)isSelected;

- (void)setSticker:(BOOL)isSelected;

- (void)reset;

@end

typedef NS_ENUM(NSInteger, CaptureMode) {
    CaptureModeAgora = 0,
    CaptureModeCustom = 1
};

@interface BeautyStats : NSObject
/// Minimum beauty processing time
@property(nonatomic, assign) double minCostMs;
/// Maximum beauty processing time
@property(nonatomic, assign) double maxCostMs;
/// Average beauty processing time
@property(nonatomic, assign) double averageCostMs;

@end

typedef NS_ENUM(NSInteger, MirrorMode) {
    /// Both local and remote are mirrored, default for front camera
    MirrorMode_LOCAL_REMOTE = 0,
    /// Only local is mirrored, remote is not mirrored. Used for video call scenarios and e-commerce live streaming scenarios (to ensure text on signboards is readable). In this mode, text stickers will be reversed on one side due to local and remote being opposite
    MirrorMode_LOCAL_ONLY = 1,
    /// Only remote is mirrored
    MirrorMode_REMOTE_ONLY = 2,
    /// Neither local nor remote is mirrored, default for rear camera
    MirrorMode_NONE
};

@interface CameraConfig : NSObject
// Default mirror mode for front camera
@property(nonatomic, assign) MirrorMode frontMirror;
// Default mirror mode for rear camera
@property(nonatomic, assign) MirrorMode backMirror;
@end

@interface BeautyConfig : NSObject
#if __has_include(<AgoraRtcKit/AgoraRtcKit.h>)
// RTC object passed from external, cannot be null
@property(nonatomic, weak) AgoraRtcEngineKit *rtcEngine;
#endif
// Beauty SDK interface object passed from external (varies by vendor), cannot be null
@property(nonatomic, weak) id<BeautyRenderDelegate> beautyRender;
// Whether to automatically register raw data callback processing internally
@property(nonatomic, assign) CaptureMode captureMode;
// Event callback, including beauty processing time
@property(nonatomic, copy) void (^eventCallback)(BeautyStats *stats);
// Statistics interval in seconds, default: 1s
@property(nonatomic, assign) NSInteger statsDuration;
// Whether to enable statistics
@property(nonatomic, assign) BOOL statsEnable;
// Camera mirror configuration
@property(nonatomic, strong) CameraConfig *cameraConfig;

@end

@interface BeautyAPI : NSObject

/**
 *  Render
 **/
@property(nonatomic, weak) id<BeautyRenderDelegate> beautyRender;

/**
 * Create and initialize beauty scenario API. If registerVideoFrameObserver has been called externally, create must be called after it
 *
 * @param config Configuration
 *
 * @return 0: Success, non-0: See error codes
 **/
- (int)initialize:(BeautyConfig *)config;

/**
 * Beauty switch
 *
 * @param enable Whether to enable beauty
 *
 * @return 0: Success, non-0: See error codes
 **/
- (int)enable:(BOOL)enable;

/**
 * Read beauty switch status
 **/
@property(nonatomic, readonly, assign) BOOL isEnable;

/**
 * Whether it is front camera
 **/
@property(nonatomic, assign, readonly) BOOL isFrontCamera;

/**
 * Switch camera
 *
 *
 * @return 0: Success; non-0: See error codes
 **/
- (int)switchCamera;

/**
 * Set camera mirror mode, note that front and rear need to be controlled separately
 *
 *
 * @return 0: Success; non-0: See error codes
 **/
- (int)updateCameraConfig:(CameraConfig *)cameraConfig;

/**
 * Local view rendering, mirror issues are handled internally
 *
 * @param view Render view
 * @param renderMode Render scaling mode
 * @return 0: Success, non-0: See error codes
 **/
#if __has_include(<AgoraRtcKit/AgoraRtcKit.h>)
- (int)setupLocalVideo:(UIView *)view renderMode:(AgoraVideoRenderMode)renderMode;
#endif

/**
 * Mirror processing method, only needs to be called when useCustom is true
 *
 * @return Whether mirroring is needed during capture
 **/
- (BOOL)getMirrorApplied;
/**
 * Beauty processing method, only needs to be called when useCustom is true, otherwise will report error
 *
 *
 * @return 0: Success; non-0: See error codes
 **/
- (int)onFrame:(CVPixelBufferRef)pixelBuffer callback:(void (^)(CVPixelBufferRef))callback;

/**
 * Set optimal default beauty parameters
 *
 * @param mode Beauty parameter mode
 *
 * @return 0: Success; non-0: See error codes
 **/
- (int)setBeautyPreset:(BeautyPresetMode)mode;

/**
 * Destroy beauty scenario API.
 * When created with useCustome=true, will call rtcEngine.registerVideoFrameObserver(null) to unbind raw data callback.
 *
 * @return 0: Success; non-0: See error code table
 **/
- (int)destroy;

/**
 * @return Version number
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
