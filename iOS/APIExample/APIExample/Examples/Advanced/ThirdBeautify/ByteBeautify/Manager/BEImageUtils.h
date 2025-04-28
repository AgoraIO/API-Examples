//  BEImageUtils.h
// EffectsARSDK

#ifndef BEImageUtils_h
#define BEImageUtils_h

#import <OpenGLES/ES2/glext.h>
#import <UIKit/UIKit.h>
#import <Metal/Metal.h>
#import <CoreVideo/CoreVideo.h>
#import "BEGLTexture.h"

// Data format
typedef NS_ENUM(NSInteger, BEFormatType) {
    // Unknown format
    BE_UNKNOW,
    // 8bit R G B A
    BE_RGBA,
    // 8bit B G R A
    BE_BGRA,
    // video range, 8bit Y1 Y2 Y3 Y4... U1 V1...
    BE_YUV420V,
    // full range, 8bit Y1 Y2 Y3 Y4... U1 V1...
    BE_YUV420F,
    // 8bit Y1 Y2 Y3 Y4... U1... V1...
    BE_YUVY420,
    BE_RGB,
    BE_BGR
};

typedef NS_ENUM(NSInteger, BEFlipOrientation) {
    BE_FlipHorizontal,

    BE_FlipVertical
};

@interface BEPixelBufferInfo : NSObject

@property(nonatomic, assign) BEFormatType format;
@property(nonatomic, assign) int width;
@property(nonatomic, assign) int height;
@property(nonatomic, assign) int bytesPerRow;

@end

@interface BEBuffer : NSObject

// Buffer pointer for RGBA format
@property(nonatomic, assign) unsigned char *buffer;

// Y buffer pointer, only for YUV format
@property(nonatomic, assign) unsigned char *yBuffer;

// UV buffer pointer, only for YUV format
@property(nonatomic, assign) unsigned char *uvBuffer;

// U buffer pointer, only for YUV format(y420)
@property(nonatomic, assign) unsigned char *uBuffer;

// v buffer pointer, only for YUV format(y420)
@property(nonatomic, assign) unsigned char *vBuffer;

// Wide for RGBA format
@property(nonatomic, assign) int width;

// High, for RGBA format
@property(nonatomic, assign) int height;

// Y buffer width for YUV format
@property(nonatomic, assign) int yWidth;

// High/y buffer for YUV format
@property(nonatomic, assign) int yHeight;

// Wide/uv buffer for YUV format
@property(nonatomic, assign) int uvWidth;

// High/uv buffer for YUV format
@property(nonatomic, assign) int uvHeight;

// Line width for RGBA format
@property(nonatomic, assign) int bytesPerRow;

// Y buffer line width for YUV format
@property(nonatomic, assign) int yBytesPerRow;

// UV buffer line width for YUV format
@property(nonatomic, assign) int uvBytesPerRow;

// U buffer line width for YUV format
@property(nonatomic, assign) int uBytesPerRow;

// V buffer line width for YUV format
@property(nonatomic, assign) int vBytesPerRow;

// Format
@property(nonatomic, assign) BEFormatType format;

@end

@interface BEImageUtils : NSObject

#pragma mark - Init output texture and get

// @brief initializes a texture bound to CVPixelBufferRef
// @param width
// @param height
// @param format, only support BE_BGRA/BE_YUV420F/BE_YUV420V
- (BEPixelBufferGLTexture *)getOutputPixelBufferGLTextureWithWidth:(int)width height:(int)height format:(BEFormatType)format withPipeline:(BOOL)usepipeline;

// @brief open texture cache
// @details When turned on, call getOutputPixelBufferGLTextureWithWidth: height: format:
// Hour, three different textures will be output in a loop to ensure that any consecutive 3 frames of textures will not be repeated, which is used for parallel rendering of SDK
// @param useCache whether to open texture cache
- (void)setUseCachedTexture:(BOOL)useCache;

#pragma mark - CVPixelBuffer to others

// @Briefing CVPixelBuffer to BEBuffer
/// @param pixelBuffer CVPixelBuffer
// @param outputFormat output BEBuffer format
- (BEBuffer *)transforCVPixelBufferToBuffer:(CVPixelBufferRef)pixelBuffer outputFormat:(BEFormatType)outputFormat;

// @Briefing CVPixelBuffer, texture
/// @param pixelBuffer CVPixelBuffer
- (BEPixelBufferGLTexture *)transforCVPixelBufferToTexture:(CVPixelBufferRef)pixelBuffer;

// @Briefing CVPixelBuffer to metal texture
/// @param pixelBuffer CVPixelBuffer
- (id<MTLTexture>)transformCVPixelBufferToMTLTexture:(CVPixelBufferRef)pixelBuffer;

// @Briefing CVPixelBuffer to CVPixelBuffer
// @param pixelBuffer Enter CVPixelBuffer
// @param outputFormat output CVPixelBuffer format
- (CVPixelBufferRef)transforCVPixelBufferToCVPixelBuffer:(CVPixelBufferRef)pixelBuffer outputFormat:(BEFormatType)outputFormat;

// @Briefing Rotate CVPixelBuffer
// @details The output CVPixelBuffer needs to be released manually by calling CVPixelBufferRelease
/// @param pixelBuffer CVPixelBuffer
// @param rotation angle, 90/180/270
- (CVPixelBufferRef)rotateCVPixelBuffer:(CVPixelBufferRef)pixelBuffer rotation:(int)rotation;

- (CVPixelBufferRef)reflectCVPixelBuffer:(CVPixelBufferRef)pixelBuffer orientation:(BEFlipOrientation)orient;

#pragma mark - BEBuffer to others

// @Briefing BEBuffer to CVPixelBuffer
/// @param buffer BEBuffer
// @param outputFormat output format
- (CVPixelBufferRef)transforBufferToCVPixelBuffer:(BEBuffer *)buffer outputFormat:(BEFormatType)outputFormat;

// @Briefing BEBuffer to CVPixelBuffer
// @details Copy the contents of the BEBuffer to the existing target CVPixleBuffer, which can be formatted at the same time
/// @param buffer BEBuffer
// @param pixelBuffer Target CVPixelBuffer
- (BOOL)transforBufferToCVPixelBuffer:(BEBuffer *)buffer pixelBuffer:(CVPixelBufferRef)pixelBuffer;

// @Briefing BEBuffer to BEBuffer
// @param inputBuffer Enter BEBuffer
// @param outputFormat output format
- (BEBuffer *)transforBufferToBuffer:(BEBuffer *)inputBuffer outputFormat:(BEFormatType)outputFormat;

// @Briefing BEBuffer to BEBuffer
// @details Copy the contents of the BEBuffer to the existing target BEBuffer, and format conversion can be performed at the same time
// @param inputBuffer Enter BEBuffer
// @param outputBuffer output BEBuffer
- (BOOL)transforBufferToBuffer:(BEBuffer *)inputBuffer outputBuffer:(BEBuffer *)outputBuffer;

// @Briefing BEBuffer
// @param inputBuffer Enter BEBuffer
// @param outputBuffer output BEBuffer
// @param rotation angle, 90/180/270
- (BOOL)rotateBufferToBuffer:(BEBuffer *)inputBuffer outputBuffer:(BEBuffer *)outputBuffer rotation:(int)rotation;

// @Brief BEBuffer, texture
/// @param buffer BEBuffer
- (id<BEGLTexture>)transforBufferToTexture:(BEBuffer *)buffer;

// @Briefing BEBuffer to UIImage
/// @param buffer BEBuffer
- (UIImage *)transforBufferToUIImage:(BEBuffer *)buffer;

#pragma mark - Texture to others

// @brief texture to BEBuffer
// @param texture texture ID
// @param widht
// @param height
// @param outputFormat output BEBuffer format, only supports RGBA/BGRA
- (BEBuffer *)transforTextureToBEBuffer:(GLuint)texture width:(int)widht height:(int)height outputFormat:(BEFormatType)outputFormat;

#pragma mark - UIImage to others

// @Briefing UIImage to BEBuffer
/// @param image UIImage
- (BEBuffer *)transforUIImageToBEBuffer:(UIImage *)image;

#pragma mark - Utils

// @Briefing Get the CVPxielBuffer format
/// @param pixelBuffer CVPixelBuffer
- (BEFormatType)getCVPixelBufferFormat:(CVPixelBufferRef)pixelBuffer;

// @Briefing OSType to BEFormatType
/// @param type OSType
- (BEFormatType)getFormatForOSType:(OSType)type;

// @Briefing BEFormatType to OSType
/// @param format BEFormatType
- (OSType)getOsType:(BEFormatType)format;

// @Briefing BEFormatType to Glenum
/// @param format BEFormatType
- (GLenum)getGlFormat:(BEFormatType)format;

// @Briefing for CVPixelBuffer information
/// @param pixelBuffer CVPixelBuffer
- (BEPixelBufferInfo *)getCVPixelBufferInfo:(CVPixelBufferRef)pixelBuffer;

// @Briefing Create BEBuffer
// @Details can calculate the required size based on information such as width, height, bytesPerRow, format, etc.
// External no need to consider memory release
// @param width
// @param height
/// @param bytesPerRow bytesPerRow
/// @param format BEFormatType
- (BEBuffer *)allocBufferWithWidth:(int)width height:(int)height bytesPerRow:(int)bytesPerRow format:(BEFormatType)format;

// @Briefing Copy CVPixelBuffer, revoker should be resposible for the life cycle.
// @param src pixelBuffer
- (CVPixelBufferRef)copyCVPixelBuffer:(CVPixelBufferRef)pixelBuffer;

// change default settings
+ (void)setTextureCacheNum:(int)num;
+ (void)setUseCachedPixelBuffer:(bool)use;
+ (int)textureCacheNum;
+ (bool)useCachedPixelBuffer;

@end

#endif /* BEImageUtils_h */
