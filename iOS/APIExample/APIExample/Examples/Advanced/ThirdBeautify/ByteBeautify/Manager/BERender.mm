// Copyright (C) 2019 Beijing Bytedance Network Technology Co., Ltd.
#import "BERender.h"

#if __has_include("bef_effect_ai_api.h")
#import "bef_effect_ai_yuv_process.h"
#endif

#import "BERenderHelper.h"

static const int TEXTURE_CACHE_NUM = 3;

@interface BERender () {
    BERenderHelper          *_renderHelper;
    GLuint                  _frameBuffer;
    GLuint                  _textureInputs[TEXTURE_CACHE_NUM];
    GLuint                  _textureOutputs[TEXTURE_CACHE_NUM];
    int                     _textureIndex;
    
    CIContext               *_ciContext;
    CVPixelBufferRef        _cachedPixelBuffer;
    unsigned char           *_buffOutPointer;
    unsigned int            _buffOutPointerLength;
    unsigned char           *_yuvBufferOutPointer;
    unsigned int            _yuvBufferOutPointerLength;
    
    // for CVPixelBuffer/Texture binding
    CVOpenGLESTextureCacheRef   _cvTextureCaches[TEXTURE_CACHE_NUM];
    CVOpenGLESTextureRef    _cvTextureInputs[TEXTURE_CACHE_NUM];
    CVOpenGLESTextureRef    _cvTextureOutputs[TEXTURE_CACHE_NUM];
    CVPixelBufferRef        _outputPixelBuffers[TEXTURE_CACHE_NUM];
}

@property (nonatomic, readwrite) NSString *triggerAction;
@property (nonatomic, assign) BOOL effectEnable;

@property (nonatomic, assign) GLuint currentTexture;
@end

@implementation BERender

static NSString* LICENSE_PATH;

- (instancetype)init {
    self = [super init];
    if (self){
        _renderHelper = [[BERenderHelper alloc] init];
        glGenFramebuffers(1, &_frameBuffer);
        
        _cachedPixelBuffer = NULL;
        _buffOutPointer = NULL;
        _textureIndex = 0;
        _useCacheTexture = NO;
    }
    return self;
}

- (void)dealloc {
    free(_buffOutPointer);
    free(_yuvBufferOutPointer);
    if (_cachedPixelBuffer) {
        CVPixelBufferRelease(_cachedPixelBuffer);
        _cachedPixelBuffer = NULL;
    }
    for (int i = 0; i < TEXTURE_CACHE_NUM; i++) {
        if (_cvTextureInputs[i] != NULL) {
            CFRelease(_cvTextureInputs[i]);
            _cvTextureInputs[i] = NULL;
        }
        
        if (_cvTextureCaches[i] != NULL) {
            CFRelease(_cvTextureCaches[i]);
            _cvTextureCaches[i] = NULL;
        }

        if (_outputPixelBuffers[i]) {
            CVPixelBufferRelease(_outputPixelBuffers[i]);
            _outputPixelBuffers[i] = NULL;
        }
        
        if (_cvTextureOutputs[i]) {
            CFRelease(_cvTextureOutputs[i]);
            _cvTextureOutputs[i] = NULL;
        }
    }
    glDeleteFramebuffers(1, &_frameBuffer);
    glDeleteTextures(TEXTURE_CACHE_NUM, _textureInputs);
    glDeleteTextures(TEXTURE_CACHE_NUM, _textureOutputs);
}

- (unsigned char *)transforCVPixelBufferToBuffer:(CVPixelBufferRef)pixelBuffer outputFormat:(BEFormatType)outputFormat {
    BEFormatType inputFormat = [self getCVPixelBufferFormat:pixelBuffer];
    if (inputFormat == BE_UNKNOW) {
        return nil;
    }
    
    unsigned char *result = nil;
    CVPixelBufferLockBaseAddress(pixelBuffer, 0);
    if ([self be_isYuv420:inputFormat]) {
        if ([self be_isRgba:outputFormat]) {
            result = [self be_CVPixelBufferYuvToRgba:pixelBuffer inputFormat:inputFormat outputFormat:outputFormat];
        } else {
            [self be_notSupportNow];
        }
    } else if ([self be_isRgba:inputFormat]) {
        BEPixelBufferInfo *info = [self getCVPixelBufferInfo:pixelBuffer];
        if ([self be_isRgba:outputFormat]) {
            result = [self be_transforRgbaToRgba:(unsigned char *)CVPixelBufferGetBaseAddress(pixelBuffer) inputFormat:inputFormat outputFormat:outputFormat width:info.width height:info.height bytesPerRow:info.bytesPerRow];
        } else if ([self be_isYuv420:outputFormat]) {
            result = [self be_transforRgbaToYuv:(unsigned char *)CVPixelBufferGetBaseAddress(pixelBuffer) width:info.width height:info.height bytesPerRow:info.bytesPerRow inputFormat:inputFormat outputFormat:outputFormat];
        }
        else {
            [self be_notSupportNow];
        }
    } else {
        [self be_notSupportNow];
    }
    CVPixelBufferUnlockBaseAddress(pixelBuffer, 0);
    return result;
}

- (CVPixelBufferRef)getOutputPixelBuffer {
    if (_cvTextureCaches[_textureIndex] == NULL || _cvTextureOutputs[_textureIndex] == NULL || _outputPixelBuffers[_textureIndex] == NULL) {
        return NULL;
    }
    CVOpenGLESTextureCacheFlush(_cvTextureCaches[_textureIndex], 0);
    return _outputPixelBuffers[_textureIndex];
}

- (BOOL)initOutputTextureAndCVPixelBufferWithWidth:(int)width height:(int)height format:(BEFormatType)format {
    if (_outputPixelBuffers[_textureIndex] != NULL) {
        BEPixelBufferInfo *info = [self getCVPixelBufferInfo:_outputPixelBuffers[_textureIndex]];
        if (info.width == width && info.height == height && info.format == format) {
            return YES;
        } else {
            CVPixelBufferRelease(_outputPixelBuffers[_textureIndex]);
            _outputPixelBuffers[_textureIndex] = NULL;
        }
    }
    
    if (![self be_isRgba:format]) {
        [self be_notSupportNow];
        return NO;
    }
    
    // create texture cache
    if (!_cvTextureCaches[_textureIndex]) {
        EAGLContext *glContext = [EAGLContext currentContext];
        CVReturn ret = CVOpenGLESTextureCacheCreate(kCFAllocatorDefault, NULL, glContext, NULL, &_cvTextureCaches[_textureIndex]);
        if (ret != kCVReturnSuccess) {
            NSLog(@"create texture cache error");
            return NO;
        }
    }
    
    if (_cvTextureOutputs[_textureIndex]) {
        CFRelease(_cvTextureOutputs[_textureIndex]);
        _cvTextureOutputs[_textureIndex] = NULL;
    }
    
    CVPixelBufferRef pixelBuffer = [self be_createCVPixelBufferWithWidth:width height:height format:format];
    if (pixelBuffer == NULL) {
        NSLog(@"create pxiel buffer error");
        return NO;
    }
    _outputPixelBuffers[_textureIndex] = pixelBuffer;
    CVReturn ret = CVOpenGLESTextureCacheCreateTextureFromImage(kCFAllocatorDefault, _cvTextureCaches[_textureIndex], pixelBuffer, NULL, GL_TEXTURE_2D, GL_RGBA, width, height, [self getGlFormat:format], GL_UNSIGNED_BYTE, 0, &_cvTextureOutputs[_textureIndex]);
    if (ret != kCVReturnSuccess) {
        NSLog(@"create texture from image error");
        return NO;
    }
    
    _textureOutputs[_textureIndex] = CVOpenGLESTextureGetName(_cvTextureOutputs[_textureIndex]);
    if (!glIsTexture(_textureOutputs[_textureIndex])) {
        NSLog(@"get glTexture error");
        return NO;
    }
    glBindTexture(CVOpenGLESTextureGetTarget(_cvTextureOutputs[_textureIndex]), _textureOutputs[_textureIndex]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);
    return YES;
}

- (GLuint)transforCVPixelBufferToTexture:(CVPixelBufferRef)pixelBuffer {
    BEPixelBufferInfo *info = [self getCVPixelBufferInfo:pixelBuffer];
    if (![self be_isRgba:info.format]) {
        [self be_notSupportNow];
        return -1;
    }
    
    if (_useCacheTexture) {
        _textureIndex = (_textureIndex + 1) % TEXTURE_CACHE_NUM;
    } else {
        _textureIndex = 0;
    }
    
    if (!_cvTextureCaches[_textureIndex]) {
        EAGLContext *glContext = [EAGLContext currentContext];
        CVReturn ret = CVOpenGLESTextureCacheCreate(kCFAllocatorDefault, NULL, glContext, NULL, &_cvTextureCaches[_textureIndex]);
        if (ret != kCVReturnSuccess) {
            NSLog(@"create texture cache error");
            return NO;
        }
    }
    if (_cvTextureInputs[_textureIndex]) {
        CFRelease(_cvTextureInputs[_textureIndex]);
        _cvTextureInputs[_textureIndex] = NULL;
    }
    
    CVReturn ret = CVOpenGLESTextureCacheCreateTextureFromImage(kCFAllocatorDefault,
                                                                _cvTextureCaches[_textureIndex],
                                                                pixelBuffer,
                                                                NULL,
                                                                GL_TEXTURE_2D,
                                                                GL_RGBA,
                                                                info.width,
                                                                info.height,
                                                                [self getGlFormat:info.format],
                                                                GL_UNSIGNED_BYTE,
                                                                0,
                                                                &_cvTextureInputs[_textureIndex]);
    if (!_cvTextureInputs[_textureIndex] || ret != kCVReturnSuccess) {
        return -1;
    }
    _textureInputs[_textureIndex] = CVOpenGLESTextureGetName(_cvTextureInputs[_textureIndex]);
    glBindTexture(GL_TEXTURE_2D , _textureInputs[_textureIndex]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);
    return _textureInputs[_textureIndex];
}

- (unsigned char *)transforTextureToBuffer:(GLuint)texture width:(int)width height:(int)height outputFormat:(BEFormatType)outputFormat bytesPerRowPointer:(int *)bytesPerRowPointer {
    GLenum glFormat = [self getGlFormat:outputFormat];
    unsigned char *buffer = [self be_transforTextureToRgbaBuffer:texture width:width height:height format:glFormat];
    if ([self be_isRgba:outputFormat]) {
        *bytesPerRowPointer = width * 4;
        return buffer;
    }
    if ([self be_isYuv420:outputFormat]) {
        *bytesPerRowPointer = width;
        return [self be_transforRgbaToYuv:buffer width:width height:height bytesPerRow:width * 4 inputFormat:BE_RGBA outputFormat:outputFormat];
    }
    
    [self be_notSupportNow];
    return nil;
}

- (GLuint)transforBufferToTexture:(unsigned char *)buffer width:(int)width height:(int)height bytesPerRow:(int)bytesPerRow inputFormat:(BEFormatType)inputFormat {
    if (_useCacheTexture) {
        _textureIndex = (_textureIndex + 1) % TEXTURE_CACHE_NUM;
    } else {
        _useCacheTexture = 0;
    }
    
//    NSLog(@"use texture index: %d", _textureIndex);
    unsigned char *rgbaBuffer = nullptr;
    GLenum glFormat = GL_RGBA;
    if ([self be_isRgba:inputFormat]) {
        rgbaBuffer = buffer;
        glFormat = [self getGlFormat:inputFormat];
    } else if ([self be_isYuv420:inputFormat]) {
        rgbaBuffer = [self be_transforYuvToRgba:buffer width:width height:height bytesPerRow:bytesPerRow inputFormat:inputFormat outputFormat:BE_RGBA];
        glFormat = GL_RGBA;
    } else {
        [self be_notSupportNow];
    }
    return [self be_transforRgbaBufferToTexture:rgbaBuffer width:width height:height format:glFormat];
}

- (CVPixelBufferRef)transforBufferToCVPixelBuffer:(unsigned char *)buffer width:(int)width height:(int)height bytesPerRow:(int)bytesPerRow inputFormat:(BEFormatType)inputFormat outputFormat:(BEFormatType)outputFormat {
    if (_cachedPixelBuffer != NULL) {
        BEPixelBufferInfo *info = [self getCVPixelBufferInfo:_cachedPixelBuffer];
        if (info.width != width || info.height != height || info.format != outputFormat) {
            CVPixelBufferRelease(_cachedPixelBuffer);
            _cachedPixelBuffer = NULL;
        }
    }
    if (_cachedPixelBuffer == NULL) {
        _cachedPixelBuffer = [self be_createCVPixelBufferWithWidth:width height:height format:outputFormat];
    }
    CVPixelBufferRef pixelBuffer = _cachedPixelBuffer;
    if (!pixelBuffer) {
        return nil;
    }
    return [self transforBufferToCVPixelBuffer:buffer pixelBuffer:pixelBuffer width:width height:height bytesPerRow:bytesPerRow inputFormat:inputFormat outputFormat:outputFormat];
}

- (CVPixelBufferRef)transforBufferToCVPixelBuffer:(unsigned char *)buffer pixelBuffer:(CVPixelBufferRef)pixelBuffer width:(int)width height:(int)height bytesPerRow:(int)bytesPerRow inputFormat:(BEFormatType)inputFormat outputFormat:(BEFormatType)outputFormat {
    CVPixelBufferLockBaseAddress(pixelBuffer, 0);
    if ([self be_isRgba:inputFormat]) {
        [self be_transforRgbaToCVPixelBuffer:pixelBuffer buffer:buffer width:width height:height bytesPerRow:bytesPerRow inputFormat:inputFormat outputFormat:outputFormat];
    } else if ([self be_isYuv420:inputFormat]) {
        [self be_transforYuvToCVPixelBuffer:pixelBuffer buffer:buffer width:width height:height bytesPerRow:bytesPerRow inputFormat:inputFormat outputFormat:outputFormat];
    } else {
        [self be_notSupportNow];
    }
    CVPixelBufferUnlockBaseAddress(pixelBuffer, 0);
    return pixelBuffer;
}

- (UIImage *)transforBufferToUIImage:(unsigned char *)buffer width:(int)width height:(int)height bytesPerRow:(int)bytesPerRow inputFormat:(BEFormatType)inputFormat {
    if ([self be_isRgba:inputFormat]) {
        return [self be_transforRgbaBufferToUIImage:buffer width:width height:height bytesPerRow:bytesPerRow inputFormat:inputFormat];
    } else if ([self be_isYuv420:inputFormat]) {
        unsigned char *rgbaBuffer = [self be_transforYuvToRgba:buffer width:width height:height bytesPerRow:bytesPerRow inputFormat:inputFormat outputFormat:BE_RGBA];
        bytesPerRow = width * 4;
        return [self be_transforRgbaBufferToUIImage:rgbaBuffer width:width height:height bytesPerRow:bytesPerRow inputFormat:BE_RGBA];
    }
    [self be_notSupportNow];
    return nil;
}

- (GLuint)be_transforRgbaBufferToTexture:(unsigned char *)buffer width:(int)width height:(int)height format:(GLenum)format {
    GLuint textureInput = _textureInputs[_textureIndex];
    
    if (glIsTexture(textureInput)) {
        glBindTexture(GL_TEXTURE_2D, textureInput);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, format, GL_UNSIGNED_BYTE, buffer);
        glBindTexture(GL_TEXTURE_2D, 0);
        return textureInput;
    }

    NSLog(@"gen input texture");
    glGenTextures(1, &textureInput);
    glBindTexture(GL_TEXTURE_2D, textureInput);
    
    // 加载相机数据到纹理
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format, GL_UNSIGNED_BYTE, buffer);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);
    _textureInputs[_textureIndex] = textureInput;
    return textureInput;
}

- (GLuint)getOutputTexture:(int)width height:(int)height {
    GLuint textureOutput = _textureOutputs[_textureIndex];
    
    if (glIsTexture(textureOutput)) {
        return textureOutput;
    }
    
    NSLog(@"gen output texture");
    glGenTextures(1, &textureOutput);
    glBindTexture(GL_TEXTURE_2D, textureOutput);
    
    // 为输出纹理开辟空间
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);
    _textureOutputs[_textureIndex] = textureOutput;
    return textureOutput;
}

- (BEFormatType)getCVPixelBufferFormat:(CVPixelBufferRef)pixelBuffer {
    OSType type = CVPixelBufferGetPixelFormatType(pixelBuffer);
    switch (type) {
        case kCVPixelFormatType_32BGRA:
            return BE_BGRA;
        case kCVPixelFormatType_32RGBA:
            return BE_RGBA;
        case kCVPixelFormatType_420YpCbCr8BiPlanarFullRange:
            return BE_YUV420F;
        case kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange:
            return BE_YUV420V;
        default:
            return BE_UNKNOW;
            break;
    }
}

- (OSType)getOsType:(BEFormatType)format {
    switch (format) {
        case BE_RGBA:
            return kCVPixelFormatType_32RGBA;
        case BE_BGRA:
            return kCVPixelFormatType_32BGRA;
        case BE_YUV420F:
            return kCVPixelFormatType_420YpCbCr8BiPlanarFullRange;
        case BE_YUV420V:
            return kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange;
        default:
            return kCVPixelFormatType_32BGRA;
            break;
    }
}

- (GLenum)getGlFormat:(BEFormatType)format {
    switch (format) {
        case BE_RGBA:
            return GL_RGBA;
        case BE_BGRA:
            return GL_BGRA;
        default:
            return GL_RGBA;
            break;
    }
}

- (BEPixelBufferInfo *)getCVPixelBufferInfo:(CVPixelBufferRef)pixelBuffer {
    int bytesPerRow = (int) CVPixelBufferGetBytesPerRow(pixelBuffer);
    int width = (int) CVPixelBufferGetWidth(pixelBuffer);
    int height = (int) CVPixelBufferGetHeight(pixelBuffer);
    size_t iTop, iBottom, iLeft, iRight;
    CVPixelBufferGetExtendedPixels(pixelBuffer, &iLeft, &iRight, &iTop, &iBottom);
    width = width + (int) iLeft + (int) iRight;
    height = height + (int) iTop + (int) iBottom;
    bytesPerRow = bytesPerRow + (int) iLeft + (int) iRight;
    
    BEPixelBufferInfo *info = [BEPixelBufferInfo new];
    info.format = [self getCVPixelBufferFormat:pixelBuffer];
    info.width = width;
    info.height = height;
    info.bytesPerRow = bytesPerRow;
    return info;
}

#pragma mark - setter
- (void)setUseCacheTexture:(bool)useCacheTexture {
    _useCacheTexture = useCacheTexture;
    if (!useCacheTexture) {
        _textureIndex = 0;
    }
}

#pragma mark - private

- (unsigned char *)be_mallocBufferWithWidth:(int)width height:(int)height {
    if (_buffOutPointer == NULL || _buffOutPointerLength != width * height) {
        if (_buffOutPointer != NULL) {
            free(_buffOutPointer);
        }
        _buffOutPointer = (unsigned char *)malloc(width * height * 4 * sizeof(unsigned char));
        _buffOutPointerLength = width * height;
        NSLog(@"malloc size %d", width * height * 4);
    }
    return _buffOutPointer;
}

- (unsigned char *)be_mallocYuvBuffer:(int)size {
    if (_yuvBufferOutPointer == NULL || _yuvBufferOutPointerLength != size) {
        if (_yuvBufferOutPointer != NULL) {
            free(_yuvBufferOutPointer);
        }
        _yuvBufferOutPointer = (unsigned char *)malloc(size * sizeof(unsigned char));
        _yuvBufferOutPointerLength = size;
        NSLog(@"malloc size %d", size);
    }
    return _yuvBufferOutPointer;
}

- (CVPixelBufferRef)be_createCVPixelBufferWithWidth:(int)width height:(int)height format:(BEFormatType)format {
    CVPixelBufferRef pixelBuffer;
    const void *keys[] = {
        kCVPixelBufferOpenGLCompatibilityKey,
        kCVPixelBufferIOSurfacePropertiesKey
    };
    const void *values[] = {
        (__bridge const void *)([NSNumber numberWithBool:YES]),
        (__bridge const void *)([NSDictionary dictionary])
    };
    
    CFDictionaryRef optionsDicitionary = CFDictionaryCreate(kCFAllocatorDefault, keys, values, 2, NULL, NULL);
    
    CVReturn res = CVPixelBufferCreate(kCFAllocatorDefault, width, height, [self getOsType:format], optionsDicitionary, &pixelBuffer);
    CFRelease(optionsDicitionary);
    if (res != kCVReturnSuccess) {
        NSLog(@"CVPixelBufferCreate error: %d", res);
    }
    return pixelBuffer;
}

- (unsigned char *)be_CVPixelBufferYuvToRgba:(CVPixelBufferRef)pixelBuffer inputFormat:(BEFormatType)inputFormat outputFormat:(BEFormatType)outputFormat {
    if (([self be_isYuv420:inputFormat]) && ([self be_isRgba:outputFormat])) {
        unsigned int width = (unsigned int)CVPixelBufferGetWidth(pixelBuffer);
        unsigned int height = (unsigned int)CVPixelBufferGetHeight(pixelBuffer);
        
        uint8_t *yBuffer = (unsigned char *)CVPixelBufferGetBaseAddressOfPlane(pixelBuffer, 0);
        unsigned int yPitch = (unsigned int)CVPixelBufferGetBytesPerRowOfPlane(pixelBuffer, 0);
        uint8_t *uvBuffer = (unsigned char *)CVPixelBufferGetBaseAddressOfPlane(pixelBuffer, 1);
        unsigned int uvPitch = (unsigned int)CVPixelBufferGetBytesPerRowOfPlane(pixelBuffer, 1);
        
        return [self be_transforYuvToRgba:yBuffer uvBuffer:uvBuffer width:width height:height yBytesPerRow:yPitch uvBytesPerRow:uvPitch inputFormat:inputFormat outputFormat:outputFormat];
    } else {
        [self be_notSupportNow];
    }
    return nil;
}

- (unsigned char *)be_transforRgbaToRgba:(unsigned char *)buffer inputFormat:(BEFormatType)inputFormat outputFormat:(BEFormatType)outputFormat width:(int)width height:(int)height bytesPerRow:(int)bytesPerRow {
    int realBytesPerRow = width * 4;
    BOOL aligned = bytesPerRow != realBytesPerRow;
    if (inputFormat == outputFormat && !aligned) {
        return buffer;
    }
    if ([self be_isRgba:inputFormat] && [self be_isRgba:outputFormat]) {
        unsigned char *result = [self be_mallocBufferWithWidth:width height:height];
        if (aligned) {
            [self be_copyBufferFrom:buffer to:result bytesPerRowFrom:bytesPerRow bytesPerRowTo:realBytesPerRow height:height];
        } else {
            memcpy(result, buffer, width * height * 4);
        }
        if (inputFormat != outputFormat) {
            for (int i = 0; i < bytesPerRow * height; i += 4) {
                int16_t r = result[i];
                int16_t b = result[i+2];
                result[i] = b;
                result[i+2] = r;
            }
        }
        return result;
    } else {
        [self be_notSupportNow];
    }
    return nil;
}

- (unsigned char *)be_transforTextureToRgbaBuffer:(GLuint)texture width:(int)width height:(int)height format:(GLenum)format {
    unsigned char *buffer = [self be_mallocBufferWithWidth:width height:height];
    [_renderHelper textureToImage:texture withBuffer:buffer Width:width height:height format:format rotation:0];
    return buffer;
}

- (unsigned char *)be_transforRgbaToYuv:(unsigned char *)rgbaBuffer width:(int)width height:(int)height bytesPerRow:(int)bytesPerRow inputFormat:(BEFormatType)inputFormat outputFormat:(BEFormatType)outputFormat {
    if ([self be_isRgba:inputFormat] && [self be_isYuv420:outputFormat]) {
        unsigned char *yBuffer = [self be_mallocYuvBuffer:width * height * 3/2];
        if (inputFormat == BE_RGBA) {
#if __has_include("bef_effect_ai_api.h")
            cvt_rgba2yuv(rgbaBuffer, yBuffer, BEF_AI_PIX_FMT_NV12, width, height);
#endif
            return yBuffer;
        }

        unsigned char *uvBuffer = &yBuffer[width * height];
        
        for (int j = 0; j < height; j++) {
            unsigned char *rgbaLine = &rgbaBuffer[bytesPerRow * j];
            unsigned char *yBufferLine = &yBuffer[width * j];
            unsigned char *uvBufferLine = &uvBuffer[width * (j >> 1)];
            for (int i = 0; i < width; i++) {
                int p = i * 4;
                int16_t r = rgbaLine[p + 0];
                int16_t g = rgbaLine[p + 1];
                int16_t b = rgbaLine[p + 2];
                
                if (inputFormat == BE_BGRA) {
                    int16_t tmp = r;
                    r = b;
                    b = tmp;
                }
                
                yBufferLine[i] = (int16_t)(0.299 * r + 0.587 * g + 0.114 * b);
                if (outputFormat == BE_YUV420V) {
                    yBufferLine[i] = yBufferLine[i] * 0.875 + 16;
                }
                if ((j & 1) == 0 && (i & 1) == 0) {
                    uvBufferLine[i & ~1] = (int16_t)(-0.169 * r - 0.331 * g + 0.5 * b + 128);
                    uvBufferLine[i | 1] = (int16_t)(0.5 * r - 0.419 * g - 0.081 * b + 128);
                }
            }
        }
        return yBuffer;
    }
    
    [self be_notSupportNow];
    return nil;
}

- (unsigned char *)be_transforYuvToRgba:(unsigned char *)yBuffer uvBuffer:(unsigned char *)uvBuffer width:(int)width height:(int)height yBytesPerRow:(int)yBytesPerRow uvBytesPerRow:(int)uvBytesPerRow inputFormat:(BEFormatType)inputFormat outputFormat:(BEFormatType)outputFormat {
    if ([self be_isYuv420:inputFormat] && [self be_isRgba:outputFormat]) {
        int bytesPerPixel = 4;
        unsigned char *rgbaOut = [self be_mallocBufferWithWidth:width height:height];
        
        unsigned int yPitch = yBytesPerRow;
        unsigned int uvPitch = uvBytesPerRow;

        for (int j = 0; j < height; j++) {
            uint8_t *rgbaBufferLine = &rgbaOut[j * width * bytesPerPixel];
            uint8_t *yBufferLine = &yBuffer[j * yPitch];
            uint8_t *uvBufferLine = &uvBuffer[(j >> 1) * uvPitch];
            
            for (int i = 0; i < width; i++) {
                int16_t y = yBufferLine[i];
                // transfor 420f to 420v
                if (inputFormat == BE_YUV420F) {
                    y = y * 0.875 + 16;
                }
                int16_t u = uvBufferLine[i & ~1] - 128;
                int16_t v = uvBufferLine[i | 1] - 128;
                
                uint8_t *rgba = &rgbaBufferLine[i * bytesPerPixel];
                int16_t r = (int16_t)roundf( y + v *  1.4 );
                int16_t g = (int16_t)roundf( y + u * -0.343 + v * -0.711 );
                int16_t b = (int16_t)roundf( y + u *  1.765);

                if (outputFormat == BE_BGRA) {
                    int16_t tmp = r;
                    r = b;
                    b = tmp;
                }
                
                rgba[0] = r;
                rgba[1] = g;
                rgba[2] = b;
                rgba[3] = 0xff;
            }
        }
        return rgbaOut;
    }
    [self be_notSupportNow];
    return nil;
}

- (unsigned char *)be_transforYuvToRgba:(unsigned char *)yuvBuffer width:(int)width height:(int)height bytesPerRow:(int)bytesPerRow inputFormat:(BEFormatType)inputFormat outputFormat:(BEFormatType)outputFormat {
    if ([self be_isYuv420:inputFormat] && [self be_isRgba:outputFormat]) {
        if (outputFormat == BE_RGBA) {
            unsigned char *rgbaBuffer = [self be_mallocBufferWithWidth:width height:height];
#if __has_include("bef_effect_ai_api.h")
            cvt_yuv2rgba(yuvBuffer, rgbaBuffer, BEF_AI_PIX_FMT_NV12, width, height, width, height, BEF_AI_CLOCKWISE_ROTATE_0, false);
#endif
            return rgbaBuffer;
        }

        unsigned char *yBuffer = yuvBuffer;
        unsigned char *uvBuffer = yuvBuffer + bytesPerRow * height;
        return [self be_transforYuvToRgba:yBuffer uvBuffer:uvBuffer width:width height:height yBytesPerRow:bytesPerRow uvBytesPerRow:bytesPerRow inputFormat:inputFormat outputFormat:outputFormat];
    }
    [self be_notSupportNow];
    return nil;
}

- (void)be_transforRgbaToCVPixelBuffer:(CVPixelBufferRef)pixelBuffer buffer:(unsigned char *)buffer width:(int)width height:(int)height bytesPerRow:(int)bytesPerRow inputFormat:(BEFormatType)inputFormat outputFormat:(BEFormatType)outputFormat {
    int pixelBufferWidth = (int)CVPixelBufferGetWidth(pixelBuffer);
    int pixelBufferHeight = (int)CVPixelBufferGetHeight(pixelBuffer);
    if (width != pixelBufferWidth || height != pixelBufferHeight) {
        NSLog(@"wrong state: width %d height: %d pixelBufferWidth %d pixelBufferHeight %d", width, height, pixelBufferWidth, pixelBufferHeight);
    }
    if ([self be_isRgba:outputFormat]) {
        int pixelBufferBytesPerRow = (int)CVPixelBufferGetBytesPerRow(pixelBuffer);
        unsigned char *baseAddress = (unsigned char *)CVPixelBufferGetBaseAddress(pixelBuffer);
        buffer = [self be_transforRgbaToRgba:buffer inputFormat:inputFormat outputFormat:outputFormat width:width height:height bytesPerRow:bytesPerRow];
        if (pixelBufferBytesPerRow != bytesPerRow) {
            unsigned char *from = buffer;
            unsigned char *to = baseAddress;
            for (int i = 0; i < height; i++) {
                memcpy(to, from, bytesPerRow);
                from += bytesPerRow;
                to += pixelBufferBytesPerRow;
            }
        } else {
            memcpy(baseAddress, buffer, height * bytesPerRow);
        }
    } else if ([self be_isYuv420:outputFormat]) {
        unsigned char *yuvBuffer = [self be_transforRgbaToYuv:buffer width:width height:height bytesPerRow:bytesPerRow inputFormat:inputFormat outputFormat:outputFormat];
        bytesPerRow = width;
        unsigned char *yBaseAddress = (unsigned char *)CVPixelBufferGetBaseAddressOfPlane(pixelBuffer, 0);
        int yBytesPerRow = (int)CVPixelBufferGetBytesPerRowOfPlane(pixelBuffer, 0);
        [self be_copyBufferFrom:yuvBuffer to:yBaseAddress bytesPerRowFrom:bytesPerRow bytesPerRowTo:yBytesPerRow height:height];
        unsigned char *uvBaseAddress = (unsigned char *)CVPixelBufferGetBaseAddressOfPlane(pixelBuffer, 1);
        int uvBytesPerRow = (int)CVPixelBufferGetBytesPerRowOfPlane(pixelBuffer, 1);
        [self be_copyBufferFrom:yuvBuffer + bytesPerRow * height to:uvBaseAddress bytesPerRowFrom:bytesPerRow bytesPerRowTo:uvBytesPerRow height:height/2];
        
    } else {
        [self be_notSupportNow];
    }
}

- (void)be_transforYuvToCVPixelBuffer:(CVPixelBufferRef)pixelBuffer buffer:(unsigned char *)buffer width:(int)width height:(int)height bytesPerRow:(int)bytesPerRow inputFormat:(BEFormatType)inputFormat outputFormat:(BEFormatType)outputFormat {
    int pixelBufferWidth = (int)CVPixelBufferGetWidth(pixelBuffer);
    int pixelBufferHeight = (int)CVPixelBufferGetHeight(pixelBuffer);
    if (width != pixelBufferWidth || height != pixelBufferHeight) {
        NSLog(@"wrong state: width %d height: %d pixelBufferWidth %d pixelBufferHeight %d", width, height, pixelBufferWidth, pixelBufferHeight);
    }
    if ([self be_isYuv420:inputFormat] && [self be_isRgba:outputFormat]) {
        unsigned char *rgbaBuffer = [self be_transforYuvToRgba:buffer width:width height:height bytesPerRow:bytesPerRow inputFormat:inputFormat outputFormat:outputFormat];
        unsigned char *baseAddress = (unsigned char *)CVPixelBufferGetBaseAddress(pixelBuffer);
        int pixelBufferBytesPerRow = (int)CVPixelBufferGetBytesPerRow(pixelBuffer);
        if (pixelBufferBytesPerRow != width * 4) {
            [self be_copyBufferFrom:rgbaBuffer to:baseAddress bytesPerRowFrom:width * 4 bytesPerRowTo:pixelBufferBytesPerRow height:height];
        } else {
            memcpy(baseAddress, rgbaBuffer, width * 4 * height);
        }
        
    } else if ([self be_isYuv420:inputFormat] && [self be_isYuv420:outputFormat]) {
        unsigned char *yBuffer = buffer;
        unsigned char *uvBuffer = buffer + bytesPerRow * height;
        unsigned char *yPixelBuffer = (unsigned char *)CVPixelBufferGetBaseAddressOfPlane(pixelBuffer, 0);
        int yPixelBufferBytesPerRow = (int)CVPixelBufferGetBytesPerRowOfPlane(pixelBuffer, 0);
        unsigned char *uvPixelBuffer = (unsigned char *)CVPixelBufferGetBaseAddressOfPlane(pixelBuffer, 1);
        int uvPixelBufferBytesPerRow = (int)CVPixelBufferGetBytesPerRowOfPlane(pixelBuffer, 1);
        [self be_copyBufferFrom:yBuffer to:yPixelBuffer bytesPerRowFrom:bytesPerRow bytesPerRowTo:yPixelBufferBytesPerRow height:height];
        [self be_copyBufferFrom:uvBuffer to:uvPixelBuffer bytesPerRowFrom:bytesPerRow bytesPerRowTo:uvPixelBufferBytesPerRow height:height/2];
    }
}

- (UIImage *)be_transforRgbaBufferToUIImage:(unsigned char *)buffer width:(int)width height:(int)height bytesPerRow:(int)bytesPerRow inputFormat:(BEFormatType)inputFormat {
    if ([self be_isRgba:inputFormat]) {
        CGDataProviderRef provider = CGDataProviderCreateWithData(
                                                                  NULL,
                                                                  buffer,
                                                                  height * bytesPerRow,
                                                                  NULL);
        
        CGColorSpaceRef colorSpaceRef = CGColorSpaceCreateDeviceRGB();
        CGBitmapInfo bitmapInfo;
        if (inputFormat == BE_RGBA) {
            bitmapInfo = kCGBitmapByteOrderDefault|kCGImageAlphaPremultipliedLast;
        } else {
            bitmapInfo = kCGBitmapByteOrder32Little | kCGImageAlphaPremultipliedFirst;
        }
        CGColorRenderingIntent renderingIntent = kCGRenderingIntentDefault;
        
        CGImageRef imageRef = CGImageCreate(width,
                                            height,
                                            8,
                                            4 * 8,
                                            bytesPerRow,
                                            colorSpaceRef,
                                            bitmapInfo,
                                            provider,
                                            NULL,
                                            NO,
                                            renderingIntent);

        UIImage *uiImage = [UIImage imageWithCGImage:imageRef];
        CGDataProviderRelease(provider);
        CGColorSpaceRelease(colorSpaceRef);
        CGImageRelease(imageRef);
        return uiImage;
    }
    [self be_notSupportNow];
    return nil;
}

- (void)be_notSupportNow {
    NSLog(@"not support now");
}

- (BOOL)be_isRgba:(BEFormatType)format {
    return format == BE_RGBA || format == BE_BGRA;
}

- (BOOL)be_isYuv420:(BEFormatType)format {
    return format == BE_YUV420F || format == BE_YUV420V;
}

- (void)be_copyBufferFrom:(unsigned char *)from to:(unsigned char *)to bytesPerRowFrom:(int)bytesPerRowFrom bytesPerRowTo:(int)bytesPserRowTo height:(int)height {
    int copyLength = bytesPserRowTo > bytesPerRowFrom ? bytesPerRowFrom : bytesPserRowTo;
    for (int i = 0; i < height; i++) {
        memcpy(to, from, copyLength);
        from += bytesPerRowFrom;
        to += bytesPserRowTo;
    }
}

- (BERenderHelper*)renderHelper {
    if (!_renderHelper){
        _renderHelper = [[BERenderHelper alloc] init];
    }
    return _renderHelper;
}

@end

