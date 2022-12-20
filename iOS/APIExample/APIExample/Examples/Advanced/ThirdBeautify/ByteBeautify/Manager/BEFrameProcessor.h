// Copyright (C) 2018 Beijing Bytedance Network Technology Co., Ltd.
#import <GLKit/GLKit.h>
#import "BEResourceHelper.h"
#import "BEEffectManager.h"

@class BEFrameProcessor;

/// result type of process
/// can be composite
typedef NS_ENUM(NSInteger, BEProcessorResult) {
    BETexture                   = 1 << 0,
    BERawData                   = 1 << 1,
    BECVPixelBuffer             = 1 << 2,
    BEImage                     = 1 << 3
};

/// represent buffer result
@interface BEProcessResultBuffer : NSObject
/// raw data pointer
@property (nonatomic, assign) unsigned char *buffer;
/// with of buffer
@property (nonatomic, assign) int width;
/// height of buffer
@property (nonatomic, assign) int height;
/// bytes per row of buffer
@property (nonatomic, assign) int bytesPerRow;
/// format of buffer
@property (nonatomic, assign) BEFormatType format;

@end

/// output of (BEProcessResult *)process:(CVPixelBufferRef)pixelBuffer timeStamp:(double)timeStamp and (BEProcessResult *)process:(unsigned char *)buffer width:(int)width height:(int)height bytesPerRow:(int)bytesPerRow timeStamp:(double)timeStamp
@interface BEProcessResult : NSObject
/// always set
@property (nonatomic, assign) GLuint texture;
/// avaliable when set BERawData/BECVPixelBuffer/BEImage to BEFrameProcessor's processorResult
@property (nonatomic, strong) BEProcessResultBuffer *buffer;
/// available when set BECVPixelBuffer to BEFrameProcessor's processorResult
@property (nonatomic, assign) CVPixelBufferRef pixelBuffer;
/// available when set BEImage to BEFrameProcessor's processResult
@property (nonatomic, assign) UIImage *image;
/// size of result
@property (nonatomic, assign) CGSize size;
@end


/// capture image delegate, will be invoked when set BEFrameProcessor's captureNextFrame YES
@protocol BECaptureDelegate <NSObject>

- (void)onImageCapture:(UIImage *)image;

@end

@interface BEFrameProcessor : NSObject

/// bind texture and CVPixelBuffer, accelerate pixel reading
@property (nonatomic, assign) BOOL pixelBufferAccelerate;

/// dispath algorithm and effect render to different thread
@property (nonatomic, assign) BOOL usePipeline;

/// process result type, buffer/CVPixelBuffer
@property (nonatomic, assign) NSInteger processorResult;

/// process result format, if not set, will be the same to inputFormat
/// such as for processorResult BECVPixelBuffer and BERawData, BE_RGBA BE_BGRA BE_YUV420P BEYUV420V are available
/// for BEImage and BETexture, no available
@property (nonatomic, assign) BEFormatType outputFormat;

/// get composer Mode, 0/1
@property (nonatomic, readonly) int composerMode;

/// capture next frame when set YES
@property (nonatomic, assign) BOOL captureNextFrame;

/// capture frame delegate
@property (nonatomic, weak) id<BECaptureDelegate> captureDelegate;

/// init function
/// @param context gl context
/// @param delegate resource delegate, nullable
- (instancetype)initWithContext:(EAGLContext *)context resourceDelegate:(id<BEResourceHelperDelegate>)delegate;

/// process CVPixelBuffer
/// @param pixelBuffer original pixelBuffer
/// @param timeStamp current time
- (BEProcessResult *)process:(CVPixelBufferRef)pixelBuffer timeStamp:(double)timeStamp;

/// process buffer
/// @param buffer original buffer
/// @param width with of buffer
/// @param height height of buffer
/// @param bytesPerRow bytesPerRow of buffer
/// @param timeStamp current time
/// @param format pixel format, see BEFormatType
- (BEProcessResult *)process:(unsigned char *)buffer width:(int)width height:(int)height bytesPerRow:(int)bytesPerRow timeStamp:(double)timeStamp format:(BEFormatType)format;

/// process texture
/// @param texture original texture
/// @param width width of texture
/// @param height height of texture
/// @param timeStamp current time
- (BEProcessResult *)process:(GLuint)texture width:(int)width height:(int)height timeStamp:(double)timeStamp;

/// set filter path
/// @param path relative path
- (void)setFilterPath:(NSString *)path;

/// set filter intensity
/// @param intensity 0-1
- (void)setFilterIntensity:(float)intensity;

/// set sticker path
/// @param path relative path
- (void)setStickerPath:(NSString *)path;

/// set composer mode
/// @param mode 0: exclusive between composer and sticker, 1: not exclusive between composer and sticker
- (void)setComposerMode:(int)mode;

/// update composer nodes
/// @param nodes relative path of nodes
- (void)updateComposerNodes:(NSArray<NSString *> *)nodes;

/// update composer node intensity
/// @param node relative path of node
/// @param key key of feature, such as smooth,white...
/// @param intensity 0-1
- (void)updateComposerNodeIntensity:(NSString *)node key:(NSString *)key intensity:(CGFloat)intensity;

/// set if effect is on
/// @param on YES: do render NO: not do render, just return origin texture/buffer/CVPixelBuffer
- (void)setEffectOn:(BOOL)on;

/// get available features in sdk
- (NSArray<NSString *> *)availableFeatures;

/// get sdk version
- (NSString *)sdkVersion;

/// set camera position
/// @param isFront YES: texture/buffer/CVPxielBuffer is from front camera
- (BOOL)setCameraPosition:(BOOL)isFront;

/// set image mode
/// @param imageMode YES for image process when reuse texture
- (BOOL)setImageMode:(BOOL)imageMode;

/// process touch event
/// @param x x
/// @param y y
- (BOOL)processTouchEvent:(float)x y:(float)y;

#if __has_include("bef_effect_ai_api.h")
/// get face detect result
- (bef_ai_face_info *)getFaceInfo;

/// get hand detect result
- (bef_ai_hand_info *)getHandInfo;

/// get skeleton detect result
- (bef_ai_skeleton_result *)getSkeletonInfo;
#endif

@end
