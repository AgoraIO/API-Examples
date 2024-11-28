//  BEGLTexture.h
// EffectsARSDK


#ifndef BEGLTexture_h
#define BEGLTexture_h

#import <OpenGLES/ES2/glext.h>
#import <CoreVideo/CoreVideo.h>
#import <Metal/Metal.h>

typedef NS_ENUM(NSInteger, BEGLTextureType) {
    //   {zh} / 通过 glGenTextures 创建的纹理     {en} /Textures created by glGenTextures
    BE_NORMAL_TEXTURE,
    //   {zh} / 与 CVPixelBuffer 绑定的纹理     {en} /Textures bound to CVPixelBuffer
    BE_PIXEL_BUFFER_TEXTURE
};

//   {zh} / OpenGL 纹理的封装，它可以是直接通过 glGenTextures 创建的纹理，     {en} /OpenGL texture encapsulation, it can be a texture created directly through glGenTextures,
//   {zh} / 也可以是通过 CVPixelBufferRef 创建并与之绑定的纹理，     {en} /It can also be a texture created and bound with CVPixelBufferRef,
//   {zh} / 当使用 CVPixelBufferRef 创建时，仅支持 kCVPixelFormatType_32BGRA 格式的 CVPixelBufferRef     {en} /When created with CVPixelBufferRef, only CVPixelBufferRef in kCVPixelFormatType_32BGRA format is supported
@protocol BEGLTexture <NSObject>

//   {zh} / 纹理 ID     {en} /Texture ID
@property (nonatomic) GLuint texture;

// uv纹理ID，在绑定的pixelbuffer是yuv格式时该纹理号有效
@property (nonatomic) GLuint uvTexture;

//   {zh} / 纹理类型     {en} /Texture type
@property (nonatomic) BEGLTextureType type;

//   {zh} / 是否有效     {en} /Is it effective
@property (nonatomic) BOOL available;

//   {zh} / 宽     {en} /Width
@property (nonatomic, readonly) int width;

//   {zh} / 高     {en} /High
@property (nonatomic, readonly) int height;

//   {zh} / @brief 初始化     {en} /@brief initialization
//   {zh} / @param width 宽     {en} /@param width
//   {zh} / @param height 高     {en} /@param height
- (instancetype)initWithWidth:(int)width height:(int)height;

//   {zh} / @brief 更新宽高     {en} /@Brief update width and height
//   {zh} / @param width 宽     {en} /@param width
//   {zh} / @param height 高     {en} /@param height
- (void)updateWidth:(int)width height:(int)height;

//   {zh} / @brief 销毁纹理     {en} /@Briefly destroy texture
- (void)destroy;

@end

//   {zh} / 普通 gl 纹理的封装     {en} /Ordinary gl texture encapsulation
@interface BENormalGLTexture : NSObject <BEGLTexture>

//   {zh} / @brief 根据纹理号、宽、高初始化     {en} /@Brief initializes according to texture number, width, and height
//   {zh} / @param texture 纹理 ID     {en} /@param texture texture ID
//   {zh} / @param width 宽     {en} /@param width
//   {zh} / @param height 高     {en} /@param height
- (instancetype)initWithTexture:(GLuint)texture width:(int)width height:(int)height;

//   {zh} / @brief 根据 buffer 初始化     {en} /@Brief initialization based on buffer
/// @param buffer buffer
//   {zh} / @param width 宽     {en} /@param width
//   {zh} / @param height 高     {en} /@param height
//   {zh} / @param format buffer 格式，GL_RGBA/GL_BGRA     {en} /@param format buffer format, GL_RGBA/GL_BGRA
- (instancetype)initWithBuffer:(unsigned char *)buffer width:(int)width height:(int)height format:(GLenum)format;

//   {zh} / @brief 根据 buffer 更新纹理内容     {en} /@BriefUpdate texture content according to buffer
/// @param buffer buffer
//   {zh} / @param width 宽     {en} /@param width
//   {zh} / @param height 高     {en} /@param height
//   {zh} / @param format buffer 格式，GL_RGBA/GL_BGRA     {en} /@param format buffer format, GL_RGBA/GL_BGRA
- (void)update:(unsigned char *)buffer width:(int)width height:(int)height format:(GLenum)format;

//   {zh} / @brief 根据纹理号、宽、高更新纹理     {en} Update texture according to texture number, width, and height
//   {zh} / @param texture 纹理 ID     {en} /@param texture texture ID
//   {zh} / @param width 宽     {en} /@param width
//   {zh} / @param height 高     {en} /@param height
- (void)updateTexture:(GLuint)texture width:(int)width height:(int)height;


@end

//   {zh} / 根据 CVPixelBuffer 生成的 gl 纹理封装     {en} /Gl texture package generated according to CVPixelBuffer
//   {zh} / 内部完成了 CVPixelBuffer 与 gl 纹理及 mtl 纹理的绑定，当完成对纹理的处理之后，     {en} /Internally completed the binding of CVPixelBuffer to gl texture and mtl texture. After the texture is processed,
//   {zh} / 直接调用 pixelBuffer 就可以得到处理之后的 CVPixelBuffer     {en} /Call pixelBuffer directly to get the processed CVPixelBuffer
@interface BEPixelBufferGLTexture : NSObject <BEGLTexture>

@property (nonatomic) id<MTLTexture> mtlTexture;

//   {zh} / @brief 根据 CVOpenGLESTextureCacheRef 初始化     {en} CVOpenGLESTextureCacheRef initialization
/// @param textureCache cache
- (instancetype)initWithTextureCache:(CVOpenGLESTextureCacheRef)textureCache;

//   {zh} / @brief 根据 CVMetalTextureCacheRef 初始化     {en} CVMetalTextureCacheRef initialization
- (instancetype)initWithMTKTextureCache:(CVMetalTextureCacheRef)textureCache;

//   {zh} / @brief 根据宽、高、CVOpenGLESTextureCacheRef 初始化     {en} CVOpenGLESTextureCacheRef initialization based on width, height
//   {zh} / @param width 宽     {en} /@param width
//   {zh} / @param height 高     {en} /@param height
/// @param textureCache cache
- (instancetype)initWithWidth:(int)width height:(int)height textureCache:(CVOpenGLESTextureCacheRef)textureCache;

//   {zh} / @brief 根据宽、高、CVMetalTextureCacheRef 初始化     {en} CVMetalTextureCacheRef initialization based on width, height
- (instancetype)initWithWidth:(int)width height:(int)height mtlTextureCache:(CVMetalTextureCacheRef)textureCache;

//   {zh} / @brief 根据 CVPixelBuffer 初始化     {en} /@Briefing initialization based on CVPixelBuffer
/// @param pixelBuffer CVPixelBuffer
/// @param textureCache cache
- (instancetype)initWithCVPixelBuffer:(CVPixelBufferRef)pixelBuffer textureCache:(CVOpenGLESTextureCacheRef)textureCache;

//   {zh} / @brief 根据 CVPixelBuffer 初始化     {en} /@Briefing initialization based on CVPixelBuffer
- (instancetype)initWithCVPixelBuffer:(CVPixelBufferRef)pixelBuffer mtlTextureCache:(CVMetalTextureCacheRef)textureCache;

//   {zh} / @brief 更新 CVPixelBuffer     {en} /@brief update CVPixelBuffer
/// @param pixelBuffer CVPixelBuffer
- (void)update:(CVPixelBufferRef)pixelBuffer;

//   {zh} / @brief 获取与之绑定的 CVPixelBuffer     {en} /@BriefGet the CVPixelBuffer bound with it
- (CVPixelBufferRef)pixelBuffer;

@end

#endif /* BEGLTexture_h */
