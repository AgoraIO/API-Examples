//  BEImageUtils.h
// EffectsARSDK


#ifndef BEImageUtils_h
#define BEImageUtils_h

#import <OpenGLES/ES2/glext.h>
#import <UIKit/UIKit.h>
#import <Metal/Metal.h>
#import <CoreVideo/CoreVideo.h>
#import "BEGLTexture.h"

//   {zh} / 数据格式     {en} /Data format
typedef NS_ENUM(NSInteger, BEFormatType) {
    //    {zh} 未知格式        {en} Unknown format
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

@property (nonatomic, assign) BEFormatType format;
@property (nonatomic, assign) int width;
@property (nonatomic, assign) int height;
@property (nonatomic, assign) int bytesPerRow;

@end

@interface BEBuffer : NSObject

//   {zh} / buffer 指针，用于 RGBA 格式     {en} /Buffer pointer for RGBA format
@property (nonatomic, assign) unsigned char *buffer;

//   {zh} / y buffer 指针，只用于 YUV 格式     {en} /Y buffer pointer, only for YUV format
@property (nonatomic, assign) unsigned char *yBuffer;

//   {zh} / uv buffer 指针，只用于 YUV 格式     {en} /UV buffer pointer, only for YUV format
@property (nonatomic, assign) unsigned char *uvBuffer;

//   {zh} / u buffer 指针，只用于 YUV 格式(y420)     {en} /U buffer pointer, only for YUV format(y420)
@property (nonatomic, assign) unsigned char *uBuffer;

//   {zh} / v buffer 指针，只用于 YUV 格式(y420)     {en} /v buffer pointer, only for YUV format(y420)
@property (nonatomic, assign) unsigned char *vBuffer;

//   {zh} / 宽，用于 RGBA 格式     {en} /Wide for RGBA format
@property (nonatomic, assign) int width;

//   {zh} / 高，用于 RGBA 格式     {en} /High, for RGBA format
@property (nonatomic, assign) int height;

//   {zh} / y buffer 宽，用于 YUV 格式     {en} /Y buffer width for YUV format
@property (nonatomic, assign) int yWidth;

//   {zh} / y buffer 高，用于 YUV 格式     {en} High/y buffer for YUV format
@property (nonatomic, assign) int yHeight;

//   {zh} / uv buffer 宽，用于 YUV 格式     {en} Wide/uv buffer for YUV format
@property (nonatomic, assign) int uvWidth;

//   {zh} / uv buffer 高，用于 YUV 格式     {en} High/uv buffer for YUV format
@property (nonatomic, assign) int uvHeight;

//   {zh} / 行宽，用于 RGBA 格式     {en} /Line width for RGBA format
@property (nonatomic, assign) int bytesPerRow;

//   {zh} / y buffer 行宽，用于 YUV 格式     {en} /Y buffer line width for YUV format
@property (nonatomic, assign) int yBytesPerRow;

//   {zh} / uv buffer 行宽，用于 YUV 格式     {en} /UV buffer line width for YUV format
@property (nonatomic, assign) int uvBytesPerRow;

//   {zh} / u buffer 行宽，用于 YUV 格式     {en} /U buffer line width for YUV format
@property (nonatomic, assign) int uBytesPerRow;

//   {zh} / v buffer 行宽，用于 YUV 格式     {en} /V buffer line width for YUV format
@property (nonatomic, assign) int vBytesPerRow;

//   {zh} / 格式     {en} /Format
@property (nonatomic, assign) BEFormatType format;

@end

@interface BEImageUtils : NSObject

#pragma mark - Init output texture and get

//   {zh} / @brief 初始化一个与 CVPixelBufferRef 绑定的纹理     {en} /@brief initializes a texture bound to CVPixelBufferRef
//   {zh} / @param width 宽     {en} /@param width
//   {zh} / @param height 高     {en} /@param height
//   {zh} / @param format 格式，仅支持 BE_BGRA/BE_YUV420F/BE_YUV420V     {en} /@param format, only support BE_BGRA/BE_YUV420F/BE_YUV420V
- (BEPixelBufferGLTexture *)getOutputPixelBufferGLTextureWithWidth:(int)width height:(int)height format:(BEFormatType)format withPipeline:(BOOL)usepipeline;

//   {zh} / @brief 开启纹理缓存     {en} /@brief open texture cache
//   {zh} / @details 当开启之后，调用 getOutputPixelBufferGLTextureWithWidth:height:format:     {en} /@details When turned on, call getOutputPixelBufferGLTextureWithWidth: height: format:
//   {zh} / 时，会循环输出三个不同的纹理，保证任意连续的 3 帧纹理不会重复，用于 SDK 的并行渲染     {en} /Hour, three different textures will be output in a loop to ensure that any consecutive 3 frames of textures will not be repeated, which is used for parallel rendering of SDK
//   {zh} / @param useCache 是否开启纹理缓存     {en} /@param useCache whether to open texture cache
- (void)setUseCachedTexture:(BOOL)useCache;

#pragma mark - CVPixelBuffer to others

//   {zh} / @brief CVPixelBuffer 转 BEBuffer     {en} /@Briefing CVPixelBuffer to BEBuffer
/// @param pixelBuffer CVPixelBuffer
//   {zh} / @param outputFormat 输出 BEBuffer 格式     {en} /@param outputFormat output BEBuffer format
- (BEBuffer *)transforCVPixelBufferToBuffer:(CVPixelBufferRef)pixelBuffer outputFormat:(BEFormatType)outputFormat;

//   {zh} / @brief CVPixelBuffer 转 纹理     {en} /@Briefing CVPixelBuffer, texture
/// @param pixelBuffer CVPixelBuffer
- (BEPixelBufferGLTexture *)transforCVPixelBufferToTexture:(CVPixelBufferRef)pixelBuffer;

//   {zh} / @brief CVPixelBuffer 转 metal纹理     {en} /@Briefing CVPixelBuffer to metal texture
/// @param pixelBuffer CVPixelBuffer
- (id<MTLTexture>)transformCVPixelBufferToMTLTexture:(CVPixelBufferRef)pixelBuffer;

//   {zh} / @brief CVPixelBuffer 转 CVPixelBuffer     {en} /@Briefing CVPixelBuffer to CVPixelBuffer
//   {zh} / @param pixelBuffer 输入 CVPixelBuffer     {en} /@param pixelBuffer Enter CVPixelBuffer
//   {zh} / @param outputFormat 输出 CVPixelBuffer 格式     {en} /@param outputFormat output CVPixelBuffer format
- (CVPixelBufferRef)transforCVPixelBufferToCVPixelBuffer:(CVPixelBufferRef)pixelBuffer outputFormat:(BEFormatType)outputFormat;

//   {zh} / @brief 旋转 CVPixelBuffer     {en} /@Briefing Rotate CVPixelBuffer
//   {zh} / @details 输出的 CVPixelBuffer 需要手动调用 CVPixelBufferRelease 释放     {en} /@details The output CVPixelBuffer needs to be released manually by calling CVPixelBufferRelease
/// @param pixelBuffer CVPixelBuffer
//   {zh} / @param rotation 旋转角度，90/180/270     {en} /@param rotation angle, 90/180/270
- (CVPixelBufferRef)rotateCVPixelBuffer:(CVPixelBufferRef)pixelBuffer rotation:(int)rotation;


- (CVPixelBufferRef)reflectCVPixelBuffer:(CVPixelBufferRef)pixelBuffer orientation:(BEFlipOrientation)orient;

#pragma mark - BEBuffer to others

//   {zh} / @brief BEBuffer 转 CVPixelBuffer     {en} @Briefing BEBuffer to CVPixelBuffer
/// @param buffer BEBuffer
//   {zh} / @param outputFormat 输出格式     {en} /@param outputFormat output format
- (CVPixelBufferRef)transforBufferToCVPixelBuffer:(BEBuffer *)buffer outputFormat:(BEFormatType)outputFormat;

//   {zh} / @brief BEBuffer 转 CVPixelBuffer     {en} @Briefing BEBuffer to CVPixelBuffer
//   {zh} / @details 将 BEBuffer 的内容复制到已存在的目标 CVPixleBuffer 中，可以同时进行格式转换     {en} /@details Copy the contents of the BEBuffer to the existing target CVPixleBuffer, which can be formatted at the same time
/// @param buffer BEBuffer
//   {zh} / @param pixelBuffer 目标 CVPixelBuffer     {en} /@param pixelBuffer Target CVPixelBuffer
- (BOOL)transforBufferToCVPixelBuffer:(BEBuffer *)buffer pixelBuffer:(CVPixelBufferRef)pixelBuffer;

//   {zh} / @brief BEBuffer 转 BEBuffer     {en} /@Briefing BEBuffer to BEBuffer
//   {zh} / @param inputBuffer 输入BEBuffer     {en} /@param inputBuffer Enter BEBuffer
//   {zh} / @param outputFormat 输出格式     {en} /@param outputFormat output format
- (BEBuffer *)transforBufferToBuffer:(BEBuffer *)inputBuffer outputFormat:(BEFormatType)outputFormat;

//   {zh} / @brief BEBuffer 转 BEBuffer     {en} /@Briefing BEBuffer to BEBuffer
//   {zh} / @details 将 BEBuffer 的内容复制到已存在的目标 BEBuffer 中，可以同时进行格式转换     {en} /@details Copy the contents of the BEBuffer to the existing target BEBuffer, and format conversion can be performed at the same time
//   {zh} / @param inputBuffer 输入 BEBuffer     {en} /@param inputBuffer Enter BEBuffer
//   {zh} / @param outputBuffer 输出 BEBuffer     {en} /@param outputBuffer output BEBuffer
- (BOOL)transforBufferToBuffer:(BEBuffer *)inputBuffer outputBuffer:(BEBuffer *)outputBuffer;

//   {zh} / @brief 旋转 BEBuffer     {en} @Briefing BEBuffer
//   {zh} / @param inputBuffer 输入 BEBuffer     {en} /@param inputBuffer Enter BEBuffer
//   {zh} / @param outputBuffer 输出 BEBuffer     {en} /@param outputBuffer output BEBuffer
//   {zh} / @param rotation 旋转角度，90/180/270     {en} /@param rotation angle, 90/180/270
- (BOOL)rotateBufferToBuffer:(BEBuffer *)inputBuffer outputBuffer:(BEBuffer *)outputBuffer rotation:(int)rotation;

//   {zh} / @brief BEBuffer 转 纹理     {en} /@Brief BEBuffer, texture
/// @param buffer BEBuffer
- (id<BEGLTexture>)transforBufferToTexture:(BEBuffer *)buffer;

//   {zh} / @brief BEBuffer 转 UIImage     {en} @Briefing BEBuffer to UIImage
/// @param buffer BEBuffer
- (UIImage *)transforBufferToUIImage:(BEBuffer *)buffer;

#pragma mark - Texture to others

//   {zh} / @brief 纹理转 BEBuffer     {en} /@brief texture to BEBuffer
//   {zh} / @param texture 纹理 ID     {en} /@param texture texture ID
//   {zh} / @param widht 宽     {en} /@param widht
//   {zh} / @param height 高     {en} /@param height
//   {zh} / @param outputFormat 输出 BEBuffer 格式，仅支持 RGBA/BGRA     {en} /@param outputFormat output BEBuffer format, only supports RGBA/BGRA
- (BEBuffer *)transforTextureToBEBuffer:(GLuint)texture width:(int)widht height:(int)height outputFormat:(BEFormatType)outputFormat;

#pragma mark - UIImage to others

//   {zh} / @brief UIImage 转 BEBuffer     {en} @Briefing UIImage to BEBuffer
/// @param image UIImage
- (BEBuffer *)transforUIImageToBEBuffer:(UIImage *)image;

#pragma mark - Utils

//   {zh} / @brief 获取 CVPxielBuffer 格式     {en} /@Briefing Get the CVPxielBuffer format
/// @param pixelBuffer CVPixelBuffer
- (BEFormatType)getCVPixelBufferFormat:(CVPixelBufferRef)pixelBuffer;

//   {zh} / @brief OSType 转 BEFormatType     {en} @Briefing OSType to BEFormatType
/// @param type OSType
- (BEFormatType)getFormatForOSType:(OSType)type;

//   {zh} / @brief BEFormatType 转 OSType     {en} @Briefing BEFormatType to OSType
/// @param format BEFormatType
- (OSType)getOsType:(BEFormatType)format;

//   {zh} / @brief BEFormatType 转 Glenum     {en} @Briefing BEFormatType to Glenum
/// @param format BEFormatType
- (GLenum)getGlFormat:(BEFormatType)format;

//   {zh} / @brief 获取 CVPixelBuffer 信息     {en} /@Briefing for CVPixelBuffer information
/// @param pixelBuffer CVPixelBuffer
- (BEPixelBufferInfo *)getCVPixelBufferInfo:(CVPixelBufferRef)pixelBuffer;

//   {zh} / @brief 创建 BEBuffer     {en} /@Briefing Create BEBuffer
//   {zh} / @details 可以根据宽、高、bytesPerRow、格式等信息计算出所需的大小，     {en} /@Details can calculate the required size based on information such as width, height, bytesPerRow, format, etc.
//   {zh} / 外部无需考虑内存释放     {en} /External no need to consider memory release
//   {zh} / @param width 宽     {en} /@param width
//   {zh} / @param height 高     {en} /@param height
/// @param bytesPerRow bytesPerRow
/// @param format BEFormatType
- (BEBuffer *)allocBufferWithWidth:(int)width height:(int)height bytesPerRow:(int)bytesPerRow format:(BEFormatType)format;

//   {zh} / @brief 拷贝pixelbuffer，调用者需要管理返回buffer的生命周期     {en} /@Briefing Copy CVPixelBuffer, revoker should be resposible for the life cycle.
//   {zh} / @param pixelBuffer 源buffer     {en} /@param src pixelBuffer
- (CVPixelBufferRef)copyCVPixelBuffer:(CVPixelBufferRef)pixelBuffer;

// change default settings
+ (void)setTextureCacheNum:(int)num;
+ (void)setUseCachedPixelBuffer:(bool)use;
+ (int)textureCacheNum;
+ (bool)useCachedPixelBuffer;

@end

#endif /* BEImageUtils_h */
