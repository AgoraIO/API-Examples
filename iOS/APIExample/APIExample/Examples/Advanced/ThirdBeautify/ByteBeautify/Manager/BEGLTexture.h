//  BEGLTexture.h
// EffectsARSDK

#ifndef BEGLTexture_h
#define BEGLTexture_h

#import <OpenGLES/ES2/glext.h>
#import <CoreVideo/CoreVideo.h>
#import <Metal/Metal.h>

typedef NS_ENUM(NSInteger, BEGLTextureType) {
    // Textures created by glGenTextures
    BE_NORMAL_TEXTURE,
    // Textures bound to CVPixelBuffer
    BE_PIXEL_BUFFER_TEXTURE
};

// OpenGL texture encapsulation, it can be a texture created directly through glGenTextures,
// It can also be a texture created and bound with CVPixelBufferRef,
// When created with CVPixelBufferRef, only CVPixelBufferRef in kCVPixelFormatType_32BGRA format is supported
@protocol BEGLTexture <NSObject>

// Texture ID
@property(nonatomic) GLuint texture;

// UV texture ID, valid when bound pixelbuffer is yuv format
@property(nonatomic) GLuint uvTexture;

// Texture type
@property(nonatomic) BEGLTextureType type;

// Is it effective
@property(nonatomic) BOOL available;

// Width
@property(nonatomic, readonly) int width;

// High
@property(nonatomic, readonly) int height;

// @brief initialization
// @param width width
// @param height height
- (instancetype)initWithWidth:(int)width height:(int)height;

// @Brief update width and height
// @param width width
// @param height height
- (void)updateWidth:(int)width height:(int)height;

// @Briefly destroy texture
- (void)destroy;

@end

// Ordinary gl texture encapsulation
@interface BENormalGLTexture : NSObject <BEGLTexture>

// @Brief initializes according to texture number, width, and height
// @param texture texture ID
// @param width width
// @param height height
- (instancetype)initWithTexture:(GLuint)texture width:(int)width height:(int)height;

// @Brief initialization based on buffer
/// @param buffer buffer
// @param width width
// @param height height
// @param format buffer format, GL_RGBA/GL_BGRA
- (instancetype)initWithBuffer:(unsigned char *)buffer width:(int)width height:(int)height format:(GLenum)format;

// @BriefUpdate texture content according to buffer
/// @param buffer buffer
// @param width width
// @param height height
// @param format buffer format, GL_RGBA/GL_BGRA
- (void)update:(unsigned char *)buffer width:(int)width height:(int)height format:(GLenum)format;

// Update texture according to texture number, width, and height
// @param texture texture ID
// @param width width
// @param height height
- (void)updateTexture:(GLuint)texture width:(int)width height:(int)height;

@end

// Gl texture package generated according to CVPixelBuffer
// Internally completed the binding of CVPixelBuffer to gl texture and mtl texture. After the texture is processed,
// Call pixelBuffer directly to get the processed CVPixelBuffer
@interface BEPixelBufferGLTexture : NSObject <BEGLTexture>

@property(nonatomic) id<MTLTexture> mtlTexture;

// CVOpenGLESTextureCacheRef initialization
/// @param textureCache cache
- (instancetype)initWithTextureCache:(CVOpenGLESTextureCacheRef)textureCache;

// CVMetalTextureCacheRef initialization
- (instancetype)initWithMTKTextureCache:(CVMetalTextureCacheRef)textureCache;

// CVOpenGLESTextureCacheRef initialization based on width, height
// @param width width
// @param height height
/// @param textureCache cache
- (instancetype)initWithWidth:(int)width height:(int)height textureCache:(CVOpenGLESTextureCacheRef)textureCache;

// CVMetalTextureCacheRef initialization based on width, height
- (instancetype)initWithWidth:(int)width height:(int)height mtlTextureCache:(CVMetalTextureCacheRef)textureCache;

// @Briefing initialization based on CVPixelBuffer
/// @param pixelBuffer CVPixelBuffer
/// @param textureCache cache
- (instancetype)initWithCVPixelBuffer:(CVPixelBufferRef)pixelBuffer textureCache:(CVOpenGLESTextureCacheRef)textureCache;

// @Briefing initialization based on CVPixelBuffer
- (instancetype)initWithCVPixelBuffer:(CVPixelBufferRef)pixelBuffer mtlTextureCache:(CVMetalTextureCacheRef)textureCache;

// @brief update CVPixelBuffer
/// @param pixelBuffer CVPixelBuffer
- (void)update:(CVPixelBufferRef)pixelBuffer;

// @BriefGet the CVPixelBuffer bound with it
- (CVPixelBufferRef)pixelBuffer;

@end

#endif /* BEGLTexture_h */
