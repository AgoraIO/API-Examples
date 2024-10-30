//  BEGLTexture.m
// EffectsARSDK


#import "BEGLTexture.h"
#import <OpenGLES/EAGL.h>

#define GL_TEXTURE_SETTING(texture) glBindTexture(GL_TEXTURE_2D, texture); \
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); \
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); \
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); \
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); \
    glBindTexture(GL_TEXTURE_2D, 0);

@implementation BENormalGLTexture {
    
}

@synthesize texture = _texture;
@synthesize type = _type;
@synthesize available = _available;
@synthesize width = _width;
@synthesize height = _height;

- (instancetype)init
{
    self = [super init];
    if (self) {
        _type = BE_NORMAL_TEXTURE;
    }
    return self;
}

- (instancetype)initWithWidth:(int)width height:(int)height {
    if (self = [super init]) {
        _type = BE_NORMAL_TEXTURE;
        glGenTextures(1, &_texture);
        [self update:nil width:width height:height format:GL_RGBA];
    }
    return self;
}

- (instancetype)initWithBuffer:(unsigned char *)buffer width:(int)width height:(int)height format:(GLenum)format {
    if (self = [super init]) {
        _type = BE_NORMAL_TEXTURE;
        glGenTextures(1, &_texture);
        [self update:buffer width:width height:height format:format];
    }
    return self;
}

- (instancetype)initWithTexture:(GLuint)texture width:(int)width height:(int)height {
    if (self = [super init]) {
        [self updateTexture:texture width:width height:height];
    }
    return self;
}

- (void)updateWidth:(int)width height:(int)height {
    [self update:nil width:width height:height format:GL_RGBA];
}

- (void)update:(unsigned char *)buffer width:(int)width height:(int)height format:(GLenum)format {
    if (!glIsTexture(_texture)) {
        NSLog(@"error: not a valid texture %d", _texture);
        _available = NO;
        return;
    }
    glBindTexture(GL_TEXTURE_2D, _texture);
    if (_width == width && _height == height) {
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, format, GL_UNSIGNED_BYTE, buffer);
    } else {
        _width = width;
        _height = height;
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format, GL_UNSIGNED_BYTE, buffer);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    _available = YES;
}

- (void)updateTexture:(GLuint)texture width:(int)width height:(int)height {
    if (glIsTexture(_texture)) {
        glDeleteTextures(1, &_texture);
    }
    
    _texture = texture;
    _width = width;
    _height = height;
    _available = YES;
}

- (void)destroy {
    if (glIsTexture(_texture)) {
        glDeleteTextures(1, &_texture);
    }
    _available = NO;
}

@end

@implementation BEPixelBufferGLTexture {
    CVPixelBufferRef            _pixelBuffer;
    BOOL                        _needReleasePixelBuffer;
    
    CVOpenGLESTextureRef        _cvTexture;
    CVOpenGLESTextureRef        _yuvTexture;
    CVOpenGLESTextureCacheRef   _textureCache;
    
    CVMetalTextureRef           _cvMTLTexture;
    CVMetalTextureCacheRef      _mtlTextureCache;
    
    BOOL                        _needReleaseTextureCache;
    BOOL                        _needReleaseMTLTextureCache;
}

@synthesize texture = _texture;
@synthesize uvTexture = _uvTexture;
@synthesize type = _type;
@synthesize available = _available;
@synthesize width = _width;
@synthesize height = _height;

- (instancetype)init
{
    self = [super init];
    if (self) {
        _type = BE_PIXEL_BUFFER_TEXTURE;
    }
    return self;
}

- (instancetype)initWithTextureCache:(CVOpenGLESTextureCacheRef)textureCache {
    self = [super init];
    if (self) {
        _type = BE_PIXEL_BUFFER_TEXTURE;
        _textureCache = textureCache;
        _needReleaseTextureCache = NO;
    }
    return self;
}

- (instancetype)initWithMTKTextureCache:(CVMetalTextureCacheRef)textureCache {
    self = [super init];
    if (self) {
        _type = BE_PIXEL_BUFFER_TEXTURE;
        _mtlTextureCache = textureCache;
        _needReleaseMTLTextureCache = NO;
    }
    return self;
}

- (instancetype)initWithWidth:(int)width height:(int)height {
    if (self = [super init]) {
        _type = BE_PIXEL_BUFFER_TEXTURE;
        [self update:[self createPxielBuffer:width height:height]];
    }
    return self;
}

- (instancetype)initWithWidth:(int)width height:(int)height textureCache:(CVOpenGLESTextureCacheRef)textureCache {
    if (self = [super init]) {
        _textureCache = textureCache;
        _needReleaseTextureCache = NO;
        _type = BE_PIXEL_BUFFER_TEXTURE;
        [self update:[self createPxielBuffer:width height:height]];
    }
    return self;
}

- (instancetype)initWithWidth:(int)width height:(int)height mtlTextureCache:(CVMetalTextureCacheRef)textureCache {
    if (self = [super init]) {
        _mtlTextureCache = textureCache;
        _needReleaseMTLTextureCache = NO;
        _type = BE_PIXEL_BUFFER_TEXTURE;
        [self update:[self createPxielBuffer:width height:height]];
    }
    return self;
}

- (instancetype)initWithCVPixelBuffer:(CVPixelBufferRef)pixelBuffer textureCache:(CVOpenGLESTextureCacheRef)textureCache {
    if (self = [super init]) {
        _textureCache = textureCache;
        _needReleaseTextureCache = NO;
        _type = BE_PIXEL_BUFFER_TEXTURE;
        [self update:pixelBuffer];
    }
    return self;
}

- (instancetype)initWithCVPixelBuffer:(CVPixelBufferRef)pixelBuffer mtlTextureCache:(CVMetalTextureCacheRef)textureCache {
    if (self = [super init]) {
        _mtlTextureCache = textureCache;
        _needReleaseMTLTextureCache = NO;
        _type = BE_PIXEL_BUFFER_TEXTURE;
        [self update:pixelBuffer];
    }
    return self;
}

- (CVPixelBufferRef)createPxielBuffer:(int)width height:(int)height {
    CVPixelBufferRef pixelBuffer;
    CFDictionaryRef optionsDicitionary = nil;
    // judge whether the device support metal
    if (MTLCreateSystemDefaultDevice()) {
        const void *keys[] = {
            kCVPixelBufferOpenGLCompatibilityKey,
            kCVPixelBufferMetalCompatibilityKey,
            kCVPixelBufferIOSurfacePropertiesKey
        };
        const void *values[] = {
            (__bridge const void *)([NSNumber numberWithBool:YES]),
            (__bridge const void *)([NSNumber numberWithBool:YES]),
            (__bridge const void *)([NSDictionary dictionary])
        };
        optionsDicitionary = CFDictionaryCreate(kCFAllocatorDefault, keys, values, 3, NULL, NULL);
    } else {
        const void *keys[] = {
            kCVPixelBufferOpenGLCompatibilityKey,
            kCVPixelBufferIOSurfacePropertiesKey
        };
        const void *values[] = {
            (__bridge const void *)([NSNumber numberWithBool:YES]),
            (__bridge const void *)([NSDictionary dictionary])
        };
        optionsDicitionary = CFDictionaryCreate(kCFAllocatorDefault, keys, values, 3, NULL, NULL);
    }
    
    CVReturn res = CVPixelBufferCreate(kCFAllocatorDefault, width, height, kCVPixelFormatType_32BGRA, optionsDicitionary, &pixelBuffer);
    CFRelease(optionsDicitionary);
    if (res != kCVReturnSuccess) {
        NSLog(@"CVPixelBufferCreate error: %d", res);
        if (res == kCVReturnInvalidPixelFormat) {
            NSLog(@"only format BGRA and YUV420 can be used");
        }
        _available = NO;
    }
    _available = YES;
    _needReleasePixelBuffer = YES;
    return pixelBuffer;
}

- (void)updateWidth:(int)width height:(int)height {
    if (_width != width || _height != height) {
        [self destroy];
        
        [self update:[self createPxielBuffer:width height:height]];
    }
}

- (void)update:(CVPixelBufferRef)pixelBuffer {
    if (_pixelBuffer && _needReleasePixelBuffer) {
        _needReleasePixelBuffer = NO;
        CVPixelBufferRelease(_pixelBuffer);
    }
    if (pixelBuffer == nil) {
        _available = NO;
        return;
    }
    
    // gl texture
    if (!_textureCache) {
        _needReleaseTextureCache = YES;
        EAGLContext *context = [EAGLContext currentContext];
        CVReturn ret = CVOpenGLESTextureCacheCreate(kCFAllocatorDefault, NULL, context, NULL, &_textureCache);
        if (ret != kCVReturnSuccess) {
            NSLog(@"create CVOpenGLESTextureCacheRef fail: %d", ret);
            _available = NO;
            return;
        }
    }
    
    if (_cvTexture) {
        CFRelease(_cvTexture);
        _cvTexture = nil;
    }
    
    if (_yuvTexture) {
        CFRelease(_yuvTexture);
        _yuvTexture = nil;
    }
    
    OSType pbType = CVPixelBufferGetPixelFormatType(pixelBuffer);
    CVPixelBufferLockBaseAddress(pixelBuffer, 0);
    int bytesPerRow = (int) CVPixelBufferGetBytesPerRow(pixelBuffer);
    int width = (int) CVPixelBufferGetWidth(pixelBuffer);
    int height = (int) CVPixelBufferGetHeight(pixelBuffer);
    size_t iTop, iBottom, iLeft, iRight;
    CVPixelBufferGetExtendedPixels(pixelBuffer, &iLeft, &iRight, &iTop, &iBottom);
    CVPixelBufferUnlockBaseAddress(pixelBuffer, 0);
    width = width + (int) iLeft + (int) iRight;
    height = height + (int) iTop + (int) iBottom;
    bytesPerRow = bytesPerRow + (int) iLeft + (int) iRight;
    CVReturn ret = kCVReturnSuccess;
    
    if (pbType ==  kCVPixelFormatType_420YpCbCr8BiPlanarFullRange || pbType == kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange) {
        // yuv
        size_t planeCount = CVPixelBufferGetPlaneCount(pixelBuffer);
        assert(planeCount == 2);

        CVReturn ret = CVOpenGLESTextureCacheCreateTextureFromImage(kCFAllocatorDefault, _textureCache, pixelBuffer, NULL, GL_TEXTURE_2D, GL_LUMINANCE, width, height, GL_LUMINANCE, GL_UNSIGNED_BYTE, 0, &_cvTexture);
        if (ret != kCVReturnSuccess || !_cvTexture) {
            NSLog(@"create CVOpenGLESTextureRef fail: %d", ret);
            _available = NO;
            return;
        }
        
        _width = width;
        _height = height;
        _pixelBuffer = pixelBuffer;
        _texture = CVOpenGLESTextureGetName(_cvTexture);
        GL_TEXTURE_SETTING(_texture);
        
        ret = CVOpenGLESTextureCacheCreateTextureFromImage(kCFAllocatorDefault, _textureCache, pixelBuffer, NULL, GL_TEXTURE_2D, GL_LUMINANCE_ALPHA, width/2, height/2, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, 1, &_yuvTexture);
        if (ret != kCVReturnSuccess || !_yuvTexture) {
            NSLog(@"create CVOpenGLESTextureRef fail: %d", ret);
            _available = NO;
            return;
        }
        _uvTexture = CVOpenGLESTextureGetName(_yuvTexture);
        GL_TEXTURE_SETTING(_uvTexture);
    } else {
        // bgra
        ret = CVOpenGLESTextureCacheCreateTextureFromImage(kCFAllocatorDefault, _textureCache, pixelBuffer, NULL, GL_TEXTURE_2D, GL_RGBA, width, height, GL_BGRA, GL_UNSIGNED_BYTE, 0, &_cvTexture);
        if (ret != kCVReturnSuccess || !_cvTexture) {
            NSLog(@"create CVOpenGLESTextureRef fail: %d", ret);
            _available = NO;
            return;
        }
        
        _width = width;
        _height = height;
        _pixelBuffer = pixelBuffer;
        _texture = CVOpenGLESTextureGetName(_cvTexture);
        GL_TEXTURE_SETTING(_texture);
    }
    
    // metal texture
    id<MTLDevice> device = MTLCreateSystemDefaultDevice();
    if (device) {
        if(!_mtlTextureCache) {
            _needReleaseMTLTextureCache = YES;
            ret = CVMetalTextureCacheCreate(kCFAllocatorDefault, NULL, device, NULL, &_mtlTextureCache);
            if (ret != kCVReturnSuccess) {
                NSLog(@"create CVMetalTextureCacheRef fail: %d", ret);
                _available = NO;
                return;
            }
        }
        
        ret = CVMetalTextureCacheCreateTextureFromImage(kCFAllocatorDefault, _mtlTextureCache, pixelBuffer, NULL, MTLPixelFormatBGRA8Unorm, width, height, 0, &_cvMTLTexture);
        if (ret != kCVReturnSuccess || !_cvMTLTexture) {
            NSLog(@"create CVMetalTextureRef fail: %d", ret);
            _available = NO;
            return;
        }
        _mtlTexture = CVMetalTextureGetTexture(_cvMTLTexture);
        if (_cvMTLTexture) {
            CFRelease(_cvMTLTexture);
            _cvMTLTexture = nil;
        }
    }
    
    _available = YES;
}

- (CVPixelBufferRef)pixelBuffer {
    return _pixelBuffer;
}

- (void)destroy {
    if (_cvTexture) {
        CFRelease(_cvTexture);
        _cvTexture = nil;
    }
    if (_cvMTLTexture) {
        CFRelease(_cvMTLTexture);
        _cvMTLTexture = nil;
    }
    if (_pixelBuffer && _needReleasePixelBuffer) {
        NSLog(@"release pixelBuffer %@", _pixelBuffer);
        _needReleasePixelBuffer = NO;
        CVPixelBufferRelease(_pixelBuffer);
        _pixelBuffer = nil;
    }
    if (_textureCache && _needReleaseTextureCache) {
        NSLog(@"release CVTextureCache %@", _textureCache);
        CVOpenGLESTextureCacheFlush(_textureCache, 0);
        CFRelease(_textureCache);
        _textureCache = nil;
    }
    if (_mtlTextureCache && _needReleaseMTLTextureCache) {
        NSLog(@"release CVMetalTextureCache %@", _mtlTextureCache);
        CVMetalTextureCacheFlush(_mtlTextureCache, 0);
        CFRelease(_mtlTextureCache);
        _mtlTextureCache = nil;
    }
    _available = NO;
}

@end
