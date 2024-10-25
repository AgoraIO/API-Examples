//  BEImageUtils.m
// EffectsARSDK


#import "BEImageUtils.h"
#import <Accelerate/Accelerate.h>
#import "BEGLTexture.h"
#import "BEOpenGLRenderHelper.h"

static int TEXTURE_CACHE_NUM = 3;
static int MAX_MALLOC_CACHE = 3;

static bool USE_CACHE_PIXEL_BUFFER = true;

@implementation BEPixelBufferInfo
@end

@implementation BEBuffer
@end

@interface BEImageUtils () {
    int                             _textureIndex;
    NSMutableArray<id<BEGLTexture>> *_inputTextures;
    NSMutableArray<id<BEGLTexture>> *_outputTextures;
    BOOL                            _useCacheTexture;
    CVOpenGLESTextureCacheRef       _textureCache;
    
    NSMutableDictionary<NSNumber *, NSValue *> *_mallocDict;
    CVPixelBufferRef                _cachedPixelBuffer;
}

@property (nonatomic, readonly) CVOpenGLESTextureCacheRef textureCache;
@property (nonatomic, strong) NSMutableDictionary<NSString *, NSValue *> *pixelBufferPoolDict;
@property (nonatomic, strong) BEOpenGLRenderHelper *renderHelper;

@end

@implementation BEImageUtils

- (instancetype)init
{
    self = [super init];
    if (self) {
        _textureIndex = 0;
        _inputTextures = [NSMutableArray arrayWithCapacity:TEXTURE_CACHE_NUM];
        _outputTextures = [NSMutableArray arrayWithCapacity:TEXTURE_CACHE_NUM];
        _textureCache = nil;
        _useCacheTexture = YES;
        _mallocDict = [NSMutableDictionary dictionary];
    }
    return self;
}

- (void)dealloc
{
    // release input/output texture
    for (id<BEGLTexture> texture in _inputTextures) {
        [texture destroy];
    }
    [_inputTextures removeAllObjects];
    for (id<BEGLTexture> texture in _outputTextures) {
        [texture destroy];
    }
    [_outputTextures removeAllObjects];
    if (_textureCache) {
        CVOpenGLESTextureCacheFlush(_textureCache, 0);
        CFRelease(_textureCache);
        _textureCache = nil;
    }
    // release malloced memory
    for (NSValue *value in _mallocDict.allValues) {
        unsigned char *pointer = [value pointerValue];
        free(pointer);
        NSLog(@"release malloced size");
    }
    [_mallocDict removeAllObjects];
    // release CVPixelBufferPool
    if (_cachedPixelBuffer != nil) {
        CVPixelBufferRelease(_cachedPixelBuffer);
    }
    for (NSValue *value in self.pixelBufferPoolDict.allValues) {
        CVPixelBufferPoolRef pool = [value pointerValue];
        CVPixelBufferPoolFlush(pool, kCVPixelBufferPoolFlushExcessBuffers);
        CVPixelBufferPoolRelease(pool);
    }
    [self.pixelBufferPoolDict removeAllObjects];
    self.pixelBufferPoolDict = nil;
}

- (BEPixelBufferGLTexture *)getOutputPixelBufferGLTextureWithWidth:(int)width height:(int)height format:(BEFormatType)format withPipeline:(BOOL)usepipeline {
    if (format != BE_BGRA) {
        NSLog(@"this method only supports BE_BRGA format, please use BE_BGRA");
        return nil;
    }
    
    while (_textureIndex >= _outputTextures.count) {
        [_outputTextures addObject:[[BEPixelBufferGLTexture alloc] initWithTextureCache:self.textureCache]];
    }
    
    id<BEGLTexture> _outputTexture = _outputTextures[_textureIndex];
    if (!_outputTexture || _outputTexture.type != BE_PIXEL_BUFFER_TEXTURE) {
        if (_outputTexture) {
            [_outputTexture destroy];
        }
        _outputTexture = [[BEPixelBufferGLTexture alloc] initWithWidth:width height:height textureCache:self.textureCache];
    }
    
    [_outputTexture updateWidth:width height:height];
    
    if (_useCacheTexture && usepipeline) {
        // If use pipeline, return last output texture if we can.
        // To resolve problems like size changed between two continuous frames
        int lastTextureIndex = (_textureIndex + TEXTURE_CACHE_NUM - 1) % TEXTURE_CACHE_NUM;
        if (_outputTextures.count > lastTextureIndex && _outputTextures[lastTextureIndex].available) {
            _outputTexture = _outputTextures[lastTextureIndex];
        }
    }
    return _outputTexture.available ? _outputTexture : nil;
}

- (void)setUseCachedTexture:(BOOL)useCache {
    _useCacheTexture = useCache;
    if (!useCache) {
        _textureIndex = 0;
    }
}

- (BEBuffer *)transforCVPixelBufferToBuffer:(CVPixelBufferRef)pixelBuffer outputFormat:(BEFormatType)outputFormat {
    BEBuffer *inputBuffer = [self be_getBufferFromCVPixelBuffer:pixelBuffer];
    return [self transforBufferToBuffer:inputBuffer outputFormat:outputFormat];
}

- (CVPixelBufferRef)transforCVPixelBufferToCVPixelBuffer:(CVPixelBufferRef)pixelBuffer outputFormat:(BEFormatType)outputFormat {
    if ([self getCVPixelBufferFormat:pixelBuffer] == outputFormat) {
        return pixelBuffer;
    }
    BEBuffer *inputBuffer = [self be_getBufferFromCVPixelBuffer:pixelBuffer];
    CVPixelBufferRef outputPixelBuffer = [self be_createCVPixelBufferWithWidth:inputBuffer.width height:inputBuffer.height format:outputFormat];
    if (!outputPixelBuffer) {
        return nil;
    }
    CVPixelBufferLockBaseAddress(outputPixelBuffer, 0);
    BEBuffer *outputBuffer = [self be_getBufferFromCVPixelBuffer:outputPixelBuffer];
    BOOL result = [self transforBufferToBuffer:inputBuffer outputBuffer:outputBuffer];
    CVPixelBufferUnlockBaseAddress(outputPixelBuffer, 0);
    if (result) {
        return outputPixelBuffer;
    }
    return nil;
}

- (CVPixelBufferRef)rotateCVPixelBuffer:(CVPixelBufferRef)pixelBuffer rotation:(int)rotation {
    if (rotation == 0) {
        return pixelBuffer;
    }
    
    BEPixelBufferInfo *info = [self getCVPixelBufferInfo:pixelBuffer];
    
    int outputWidth = info.width;
    int outputHeight = info.height;
    if (rotation % 180 == 90) {
        outputWidth = info.height;
        outputHeight = info.width;
    }
    CVPixelBufferRef outputPixelBuffer = [self be_createPixelBufferFromPool:[self getOsType:info.format] heigth:outputHeight width:outputWidth];
    
    BEBuffer *inputBuffer = [self be_getBufferFromCVPixelBuffer:pixelBuffer];
    BEBuffer *outputBuffer = [self be_getBufferFromCVPixelBuffer:outputPixelBuffer];
    
    BOOL ret = [self rotateBufferToBuffer:inputBuffer outputBuffer:outputBuffer rotation:rotation];
    if (!ret) {
        return nil;
    }
    return outputPixelBuffer;
}

- (CVPixelBufferRef)reflectCVPixelBuffer:(CVPixelBufferRef)pixelBuffer orientation:(BEFlipOrientation)orient
{
    BEPixelBufferInfo *info = [self getCVPixelBufferInfo:pixelBuffer];
    
    int outputWidth = info.width;
    int outputHeight = info.height;
    
    CVPixelBufferRef outputPixelBuffer = [self be_createPixelBufferFromPool:[self getOsType:info.format] heigth:outputHeight width:outputWidth];
    
    
    BEBuffer *inputBuffer = [self be_getBufferFromCVPixelBuffer:pixelBuffer];
    BEBuffer *outputBuffer = [self be_getBufferFromCVPixelBuffer:outputPixelBuffer];
    
    vImage_Buffer src, dest;
    {
        src.width = inputBuffer.width;
        src.height = inputBuffer.height;
        src.data = inputBuffer.buffer;
        src.rowBytes = inputBuffer.bytesPerRow;
        dest.width = outputBuffer.width;
        dest.height = outputBuffer.height;
        dest.data = outputBuffer.buffer;
        dest.rowBytes = outputBuffer.bytesPerRow;
    }
    
    if (orient == BE_FlipVertical) {
        vImageVerticalReflect_ARGB8888(&src, &dest, kvImageNoFlags);
    } else {
        vImageHorizontalReflect_ARGB8888(&src, &dest, kvImageNoFlags);
    }
    return outputPixelBuffer;
}


- (id<BEGLTexture>)transforCVPixelBufferToTexture:(CVPixelBufferRef)pixelBuffer {
    BEPixelBufferInfo *info = [self getCVPixelBufferInfo:pixelBuffer];
//    if (info.format != BE_BGRA) {
//        pixelBuffer = [self transforCVPixelBufferToCVPixelBuffer:pixelBuffer outputFormat:BE_BGRA];
////        NSLog(@"this method only supports BRGA format CVPixelBuffer, convert it to BGRA CVPixelBuffer internal");
//    }
    
    if (_useCacheTexture) {
        _textureIndex = (_textureIndex + 1) % TEXTURE_CACHE_NUM;
    } else {
        _textureIndex = 0;
    }
    
    while (_textureIndex >= _inputTextures.count) {
        [_inputTextures addObject:[[BEPixelBufferGLTexture alloc] initWithTextureCache:self.textureCache]];
    }
    
    id<BEGLTexture> texture = _inputTextures[_textureIndex];
    if (texture.type != BE_PIXEL_BUFFER_TEXTURE) {
        [texture destroy];
        texture = [[BEPixelBufferGLTexture alloc] initWithCVPixelBuffer:pixelBuffer textureCache:self.textureCache];
        _inputTextures[_textureIndex] = texture;
    } else {
        [(BEPixelBufferGLTexture *)texture update:pixelBuffer];
    }
    
    return texture;
}

- (CVPixelBufferRef)transforBufferToCVPixelBuffer:(BEBuffer *)buffer outputFormat:(BEFormatType)outputFormat {
    CVPixelBufferRef pixelBuffer = [self be_createCVPixelBufferWithWidth:buffer.width height:buffer.height format:outputFormat];
    if (pixelBuffer == nil) {
        return nil;
    }
    BOOL result = [self transforBufferToCVPixelBuffer:buffer pixelBuffer:pixelBuffer];
    if (result) {
        return pixelBuffer;
    }
    return nil;
}

- (BOOL)transforBufferToCVPixelBuffer:(BEBuffer *)buffer pixelBuffer:(CVPixelBufferRef)pixelBuffer {
    CVPixelBufferLockBaseAddress(pixelBuffer, 0);
    BEBuffer *outputBuffer = [self be_getBufferFromCVPixelBuffer:pixelBuffer];
    BOOL result = [self transforBufferToBuffer:buffer outputBuffer:outputBuffer];
    CVPixelBufferUnlockBaseAddress(pixelBuffer, 0);
    return result;
}

- (BEBuffer *)transforBufferToBuffer:(BEBuffer *)inputBuffer outputFormat:(BEFormatType)outputFormat {
    if (inputBuffer.format == outputFormat) {
        return inputBuffer;
    }
    
    BEBuffer *buffer = nil;
    if ([self be_isRgba:outputFormat]) {
        if ([self be_isRgba:inputBuffer.format]) {
            buffer = [self allocBufferWithWidth:inputBuffer.width height:inputBuffer.height bytesPerRow:inputBuffer.width * 4 format:outputFormat];
        } else {
            buffer = [self allocBufferWithWidth:inputBuffer.width height:inputBuffer.height bytesPerRow:inputBuffer.width * 4 format:outputFormat];
        }
    } else if ([self be_isYuv420:outputFormat]) {
        if ([self be_isYuv420:inputBuffer.format]) {
            buffer = [self allocBufferWithWidth:inputBuffer.yWidth height:inputBuffer.yHeight bytesPerRow:inputBuffer.yBytesPerRow format:outputFormat];
        } else {
            buffer = [self allocBufferWithWidth:inputBuffer.width height:inputBuffer.height bytesPerRow:inputBuffer.bytesPerRow format:outputFormat];
        }
    } else if ([self be_isRgb:outputFormat]) {
        if ([self be_isRgba:inputBuffer.format]) {
            buffer = [self allocBufferWithWidth:inputBuffer.width height:inputBuffer.height bytesPerRow:inputBuffer.width * 3 format:outputFormat];
        }
    }
    if (buffer == nil) {
        return nil;
    }
    BOOL result = [self transforBufferToBuffer:inputBuffer outputBuffer:buffer];
    if (result) {
        return buffer;
    }
    return nil;
}

- (BOOL)transforBufferToBuffer:(BEBuffer *)inputBuffer outputBuffer:(BEBuffer *)outputBuffer {
    if ([self be_isYuv420:outputBuffer.format]) {
        if ([self be_isRgba:inputBuffer.format]) {
            vImage_Buffer rgbaBuffer;
            rgbaBuffer.data = inputBuffer.buffer;
            rgbaBuffer.width = inputBuffer.width;
            rgbaBuffer.height = inputBuffer.height;
            rgbaBuffer.rowBytes = inputBuffer.bytesPerRow;
            vImage_Buffer yBuffer;
            yBuffer.data = outputBuffer.yBuffer;
            yBuffer.width = outputBuffer.yWidth;
            yBuffer.height = outputBuffer.yHeight;
            yBuffer.rowBytes = outputBuffer.yBytesPerRow;
            vImage_Buffer uvBuffer;
            uvBuffer.data = outputBuffer.uvBuffer;
            uvBuffer.width = outputBuffer.uvWidth;
            uvBuffer.height = outputBuffer.uvHeight;
            uvBuffer.rowBytes = outputBuffer.uvBytesPerRow;
            BOOL result = [self be_convertRgbaToYuv:&rgbaBuffer yBuffer:&yBuffer yuBuffer:&uvBuffer inputFormat:inputBuffer.format outputFormat:outputBuffer.format];
            return result;
        }
    } else if ([self be_isRgba:outputBuffer.format]) {
#define PROFILE_TEST false
#if PROFILE_TEST
        if (inputBuffer.format == outputBuffer.format) {
            unsigned char *from = inputBuffer.buffer, *to = outputBuffer.buffer;
            for (int i = 0; i < inputBuffer.height; i++) {
                memcpy(to, from, MIN(inputBuffer.bytesPerRow, outputBuffer.bytesPerRow));
                from += inputBuffer.bytesPerRow;
                to += outputBuffer.bytesPerRow;
            }
            return YES;
        }
#endif
        if ([self be_isRgba:inputBuffer.format]) {
            vImage_Buffer rgbaBuffer;
            rgbaBuffer.data = inputBuffer.buffer;
            rgbaBuffer.width = inputBuffer.width;
            rgbaBuffer.height = inputBuffer.height;
            rgbaBuffer.rowBytes = inputBuffer.bytesPerRow;
            vImage_Buffer bgraBuffer;
            bgraBuffer.data = outputBuffer.buffer;
            bgraBuffer.width = outputBuffer.width;
            bgraBuffer.height = outputBuffer.height;
            bgraBuffer.rowBytes = outputBuffer.bytesPerRow;
            BOOL result = [self be_convertRgbaToBgra:&rgbaBuffer outputBuffer:&bgraBuffer inputFormat:inputBuffer.format outputFormat:outputBuffer.format];
            return result;
        } else if ([self be_isYuv420:inputBuffer.format]) {
            vImage_Buffer yBuffer;
            yBuffer.data = inputBuffer.yBuffer;
            yBuffer.width = inputBuffer.yWidth;
            yBuffer.height = inputBuffer.yHeight;
            yBuffer.rowBytes = inputBuffer.yBytesPerRow;
            vImage_Buffer uvBuffer;
            uvBuffer.data = inputBuffer.uvBuffer;
            uvBuffer.width = inputBuffer.uvWidth;
            uvBuffer.height = inputBuffer.uvHeight;
            uvBuffer.rowBytes = inputBuffer.uvBytesPerRow;
            vImage_Buffer bgraBuffer;
            bgraBuffer.data = outputBuffer.buffer;
            bgraBuffer.width = outputBuffer.width;
            bgraBuffer.height = outputBuffer.height;
            bgraBuffer.rowBytes = outputBuffer.bytesPerRow;
            BOOL result = [self be_convertYuvToRgba:&yBuffer yvBuffer:&uvBuffer rgbaBuffer:&bgraBuffer inputFormat:inputBuffer.format outputFormat:outputBuffer.format];
            return result;
        } else if ([self be_isYuv420Planar:inputBuffer.format]) {
            vImage_Buffer yBuffer;
            yBuffer.data = inputBuffer.yBuffer;
            yBuffer.width = inputBuffer.yWidth;
            yBuffer.height = inputBuffer.yHeight;
            yBuffer.rowBytes = inputBuffer.yBytesPerRow;
            vImage_Buffer uBuffer;
            uBuffer.data = inputBuffer.uBuffer;
            uBuffer.width = inputBuffer.uvWidth;
            uBuffer.height = inputBuffer.uvHeight;
            uBuffer.rowBytes = inputBuffer.uBytesPerRow;
            vImage_Buffer vBuffer;
            vBuffer.data = inputBuffer.vBuffer;
            vBuffer.width = inputBuffer.uvWidth;
            vBuffer.height = inputBuffer.uvHeight;
            vBuffer.rowBytes = inputBuffer.vBytesPerRow;
            vImage_Buffer bgraBuffer;
            bgraBuffer.data = outputBuffer.buffer;
            bgraBuffer.width = outputBuffer.width;
            bgraBuffer.height = outputBuffer.height;
            bgraBuffer.rowBytes = outputBuffer.bytesPerRow;
            BOOL result = [self be_convertYuvToRgba:&yBuffer uBuffer:&uBuffer vBuffer:&vBuffer rgbaBuffer:&bgraBuffer inputFormat:inputBuffer.format outputFormat:outputBuffer.format];
            return result;
        }
    } else if ([self be_isYuv420Planar:outputBuffer.format]) {
        if ([self be_isRgba:inputBuffer.format]) {
            vImage_Buffer rgbaBuffer;
            rgbaBuffer.data = inputBuffer.buffer;
            rgbaBuffer.width = inputBuffer.width;
            rgbaBuffer.height = inputBuffer.height;
            rgbaBuffer.rowBytes = inputBuffer.bytesPerRow;
            vImage_Buffer yBuffer;
            yBuffer.data = outputBuffer.yBuffer;
            yBuffer.width = outputBuffer.yWidth;
            yBuffer.height = outputBuffer.yHeight;
            yBuffer.rowBytes = outputBuffer.yBytesPerRow;
            vImage_Buffer uBuffer;
            uBuffer.data = outputBuffer.uBuffer;
            uBuffer.width = outputBuffer.uvWidth;
            uBuffer.height = outputBuffer.uvHeight;
            uBuffer.rowBytes = outputBuffer.uBytesPerRow;
            vImage_Buffer vBuffer;
            vBuffer.data = outputBuffer.vBuffer;
            vBuffer.width = outputBuffer.uvWidth;
            vBuffer.height = outputBuffer.uvHeight;
            vBuffer.rowBytes = outputBuffer.vBytesPerRow;

            BOOL result = [self be_convertRgbaToYuv:&rgbaBuffer yBuffer:&yBuffer uBuffer:&uBuffer vBuffer:&vBuffer inputFormat:inputBuffer.format outputFormat:outputBuffer.format];
            return result;
        }
    } else if ([self be_isRgb:outputBuffer.format]) {
        if ([self be_isRgba:inputBuffer.format]) {
            vImage_Buffer bgraBuffer;
            bgraBuffer.data = inputBuffer.buffer;
            bgraBuffer.width = inputBuffer.width;
            bgraBuffer.height = inputBuffer.height;
            bgraBuffer.rowBytes = inputBuffer.bytesPerRow;
            vImage_Buffer bgrBuffer;
            bgrBuffer.data = outputBuffer.buffer;
            bgrBuffer.width = outputBuffer.width;
            bgrBuffer.height = outputBuffer.height;
            bgrBuffer.rowBytes = outputBuffer.bytesPerRow;
            BOOL result = [self be_convertBgraToBgr:&bgraBuffer outputBuffer:&bgrBuffer inputFormat:inputBuffer.format outputFormat:outputBuffer.format];
            return result;
        }
    }
    
    return NO;
}

- (BOOL)rotateBufferToBuffer:(BEBuffer *)inputBuffer outputBuffer:(BEBuffer *)outputBuffer rotation:(int)rotation {
    if ([self be_isRgba:inputBuffer.format] && [self be_isRgba:outputBuffer.format]) {
        vImage_Buffer inputVBuffer;
        inputVBuffer.data = inputBuffer.buffer;
        inputVBuffer.width = inputBuffer.width;
        inputVBuffer.height = inputBuffer.height;
        inputVBuffer.rowBytes = inputBuffer.bytesPerRow;
        
        vImage_Buffer outputVBuffer;
        outputVBuffer.data = outputBuffer.buffer;
        outputVBuffer.width = outputBuffer.width;
        outputVBuffer.height = outputBuffer.height;
        outputVBuffer.rowBytes = outputBuffer.bytesPerRow;
        
        return [self be_rotateRgba:&inputVBuffer outputBuffer: &outputVBuffer rotation:rotation];
    }
    
    NSLog(@"not support for format %ld to %ld", (long)inputBuffer.format, (long)outputBuffer.format);
    return NO;
}

- (id<BEGLTexture>)transforBufferToTexture:(BEBuffer *)buffer {
    if (_useCacheTexture) {
        _textureIndex = (_textureIndex + 1) % TEXTURE_CACHE_NUM;
    } else {
        _useCacheTexture = 0;
    }
    
    if (![self be_isRgba:buffer.format]) {
        buffer = [self transforBufferToBuffer:buffer outputFormat:BE_BGRA];
    }
    
    if (buffer == nil) {
        return nil;
    }
    
    while (_textureIndex >= _inputTextures.count) {
        [_inputTextures addObject:[[BENormalGLTexture alloc] initWithBuffer:buffer.buffer width:buffer.width height:buffer.height format:[self getGlFormat:buffer.format]]];
    }
    id<BEGLTexture> texture = _inputTextures[_textureIndex];
    if (texture.type != BE_NORMAL_TEXTURE) {
        [texture destroy];
        texture = [[BENormalGLTexture alloc] initWithBuffer:buffer.buffer width:buffer.width height:buffer.height format:[self getGlFormat:buffer.format]];
        _inputTextures[_textureIndex] = texture;
    } else {
        [(BENormalGLTexture *)texture update:buffer.buffer width:buffer.width height:buffer.height format:[self getGlFormat:buffer.format]];
    }
    
    return texture;
}

- (id<MTLTexture>)transformCVPixelBufferToMTLTexture:(CVPixelBufferRef)pixelBuffer{
    size_t width = CVPixelBufferGetWidth(pixelBuffer);
    size_t height = CVPixelBufferGetHeight(pixelBuffer);
    id<MTLDevice> device = MTLCreateSystemDefaultDevice();
    CVMetalTextureCacheRef _textureCache;
    CVMetalTextureCacheCreate(NULL, NULL, device, NULL, &_textureCache);

    CVMetalTextureRef tmpTexture = NULL;
    CVReturn ret = CVMetalTextureCacheCreateTextureFromImage(kCFAllocatorDefault, _textureCache, pixelBuffer, NULL, MTLPixelFormatBGRA8Unorm, width, height, 0, &tmpTexture);
    if (ret != kCVReturnSuccess) {
        NSLog(@"MetalTextureCreate error: %d", ret);
        return nil;
    }
    id <MTLTexture> mtlTexture = CVMetalTextureGetTexture(tmpTexture);
    CFRelease(tmpTexture);
    
    return mtlTexture;
}


- (UIImage *)transforBufferToUIImage:(BEBuffer *)buffer {
    if (![self be_isRgba:buffer.format]) {
        buffer = [self transforBufferToBuffer:buffer outputFormat:BE_BGRA];
    }
    
    if (buffer == nil) {
        return nil;
    }
    
    CGDataProviderRef provider = CGDataProviderCreateWithData(
                                                              NULL,
                                                              buffer.buffer,
                                                              buffer.height * buffer.bytesPerRow,
                                                              NULL);
    
    CGColorSpaceRef colorSpaceRef = CGColorSpaceCreateDeviceRGB();
    CGBitmapInfo bitmapInfo;
    if (buffer.format == BE_RGBA) {
        bitmapInfo = kCGBitmapByteOrderDefault|kCGImageAlphaLast;
    } else {
        bitmapInfo = kCGBitmapByteOrder32Host | kCGImageAlphaNoneSkipFirst;
    }
    CGColorRenderingIntent renderingIntent = kCGRenderingIntentDefault;
    
    CGImageRef imageRef = CGImageCreate(buffer.width,
                                        buffer.height,
                                        8,
                                        4 * 8,
                                        buffer.bytesPerRow,
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
    NSData *data = UIImageJPEGRepresentation(uiImage, 1);
    uiImage = [UIImage imageWithData:data];
    return uiImage;
}

- (BEFormatType)getCVPixelBufferFormat:(CVPixelBufferRef)pixelBuffer {
    OSType type = CVPixelBufferGetPixelFormatType(pixelBuffer);
    return [self getFormatForOSType:type];
}

- (BEFormatType)getFormatForOSType:(OSType)type {
    switch (type) {
        case kCVPixelFormatType_32BGRA:
            return BE_BGRA;
        case kCVPixelFormatType_32RGBA:
            return BE_RGBA;
        case kCVPixelFormatType_420YpCbCr8BiPlanarFullRange:
            return BE_YUV420F;
        case kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange:
            return BE_YUV420V;
        case kCVPixelFormatType_420YpCbCr8Planar:
            return BE_YUVY420;
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
    
    BEPixelBufferInfo *info = [BEPixelBufferInfo new];
    info.format = [self getCVPixelBufferFormat:pixelBuffer];
    info.width = width;
    info.height = height;
    info.bytesPerRow = bytesPerRow;
    return info;
}

- (BEBuffer *)allocBufferWithWidth:(int)width height:(int)height bytesPerRow:(int)bytesPerRow format:(BEFormatType)format {
    BEBuffer *buffer = [[BEBuffer alloc] init];
    buffer.width = width;
    buffer.height = height;
    buffer.bytesPerRow = bytesPerRow;
    buffer.format = format;
    if ([self be_isRgba:format]) {
        buffer.buffer = [self be_mallocBufferWithSize:bytesPerRow * height];
        return buffer;
    } else if ([self be_isYuv420:format]) {
        buffer.yBuffer = [self be_mallocBufferWithSize:bytesPerRow * height];
        buffer.yWidth = width;
        buffer.yHeight = height;
        buffer.yBytesPerRow = bytesPerRow;
        buffer.uvBuffer = [self be_mallocBufferWithSize:bytesPerRow * height / 2];
        buffer.uvWidth = width / 2;
        buffer.uvHeight = height / 2;
        buffer.uvBytesPerRow = bytesPerRow;
        return buffer;
    } else if ([self be_isRgb:format]) {
        buffer.buffer = [self be_mallocBufferWithSize:bytesPerRow * height * 3];
        return buffer;
    }
    return nil;
}

- (BEBuffer *)transforUIImageToBEBuffer:(UIImage *)image {
    int width = (int)CGImageGetWidth(image.CGImage);
    int height = (int)CGImageGetHeight(image.CGImage);
    int bytesPerRow = 4 * width;
    BEBuffer *buffer = [self allocBufferWithWidth:width height:height bytesPerRow:bytesPerRow format:BE_RGBA];

    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    NSUInteger bitsPerComponent = 8;
    CGContextRef context = CGBitmapContextCreate(buffer.buffer, width, height,
                                                 bitsPerComponent, bytesPerRow, colorSpace,
                                                 kCGImageAlphaPremultipliedLast | kCGBitmapByteOrderDefault);

    CGColorSpaceRelease(colorSpace);
    CGContextDrawImage(context, CGRectMake(0, 0, width, height), image.CGImage);
    CGContextRelease(context);
    
    return buffer;
}

- (BEBuffer *)transforTextureToBEBuffer:(GLuint)texture width:(int)widht height:(int)height outputFormat:(BEFormatType)outputFormat {
    if (![self be_isRgba:outputFormat]) {
        NSLog(@"only rgba support");
        return nil;
    }
    
    BEBuffer *buffer = [self allocBufferWithWidth:widht height:height bytesPerRow:widht * 4 format:outputFormat];
    [self.renderHelper textureToImage:texture withBuffer:buffer.buffer Width:widht height:height format:[self getGlFormat:outputFormat]];
    return buffer;
}

- (CVPixelBufferRef)copyCVPixelBuffer:(CVPixelBufferRef)pixelBuffer {
    CVPixelBufferLockBaseAddress(pixelBuffer, 0);
    int bufferWidth = (int)CVPixelBufferGetWidth(pixelBuffer);
    int bufferHeight = (int)CVPixelBufferGetHeight(pixelBuffer);
    size_t bytesPerRow = CVPixelBufferGetBytesPerRow(pixelBuffer);
    uint8_t *baseAddress = CVPixelBufferGetBaseAddress(pixelBuffer);
    OSType format = CVPixelBufferGetPixelFormatType(pixelBuffer);
    CVPixelBufferUnlockBaseAddress(pixelBuffer, 0);
    
    CVPixelBufferRef pixelBufferCopy = [self be_createPixelBufferFromPool:format heigth:bufferHeight width:bufferWidth];
    CVPixelBufferLockBaseAddress(pixelBufferCopy, 0);
    uint8_t *copyBaseAddress = CVPixelBufferGetBaseAddress(pixelBufferCopy);
    memcpy(copyBaseAddress, baseAddress, bufferHeight * bytesPerRow);
    CVPixelBufferUnlockBaseAddress(pixelBufferCopy, 0);
    return pixelBufferCopy;
}

#pragma mark - private

- (BOOL)be_convertBgraToBgr:(vImage_Buffer *)inputBuffer outputBuffer:(vImage_Buffer *)outputBuffer inputFormat:(BEFormatType)inputFormat
               outputFormat:(BEFormatType)outputFormat {
    if (![self be_isRgba:inputFormat] || ![self be_isRgb:outputFormat]) {
        return NO;
    }
    vImage_Error error = kvImageNoError;
    if (inputFormat == BE_BGRA && outputFormat == BE_BGR)
        error = vImageConvert_BGRA8888toBGR888(inputBuffer, outputBuffer, kvImageNoFlags);
    else if (inputFormat == BE_BGRA && outputFormat == BE_RGB)
        error = vImageConvert_BGRA8888toRGB888(inputBuffer, outputBuffer, kvImageNoFlags);
    else if (inputFormat == BE_RGBA && outputFormat == BE_BGR)
        error = vImageConvert_RGBA8888toBGR888(inputBuffer, outputBuffer, kvImageNoFlags);
    else if (inputFormat == BE_RGBA && outputFormat == BE_RGB)
        error = vImageConvert_RGBA8888toRGB888(inputBuffer, outputBuffer, kvImageNoFlags);
    if (error != kvImageNoError) {
        NSLog(@"be_convertBgraToBgr error: %ld", error);
    }
    return error == kvImageNoError;
}

- (BOOL)be_convertRgbaToBgra:(vImage_Buffer *)inputBuffer outputBuffer:(vImage_Buffer *)outputBuffer inputFormat:(BEFormatType)inputFormat outputFormat:(BEFormatType)outputFormat {
    if (![self be_isRgba:inputFormat] || ![self be_isRgba:outputFormat]) {
        return NO;
    }
    uint8_t map[4] = {0, 1, 2, 3};
    [self be_permuteMap:map format:inputFormat];
    [self be_permuteMap:map format:outputFormat];
    vImage_Error error = vImagePermuteChannels_ARGB8888(inputBuffer, outputBuffer, map, kvImageNoFlags);
    if (error != kvImageNoError) {
        NSLog(@"be_transforRgbaToRgba error: %ld", error);
    }
    return error == kvImageNoError;
}

- (BOOL)be_rotateRgba:(vImage_Buffer *)inputBuffer outputBuffer:(vImage_Buffer *)outputBuffer rotation:(int)rotation {
    uint8_t map[4] = {255, 255, 255, 1};
    
    rotation = 360 - rotation;
    vImage_Error error = vImageRotate90_ARGB8888(inputBuffer, outputBuffer, (rotation / 90), map, kvImageNoFlags);
    if (error != kvImageNoError) {
        NSLog(@"vImageRotate90_ARGB8888 error: %ld", error);
        return NO;
    }
    
    return YES;
}

- (BOOL)be_convertRgbaToYuv:(vImage_Buffer *)inputBuffer yBuffer:(vImage_Buffer *)yBuffer yuBuffer:(vImage_Buffer *)uvBuffer inputFormat:(BEFormatType)inputFormat outputFormat:(BEFormatType)outputFormat {
    if (![self be_isRgba:inputFormat] || ![self be_isYuv420:outputFormat]) {
        return NO;
    }
    uint8_t map[4] = {1, 2, 3, 0};
    [self be_permuteMap:map format:inputFormat];
    vImage_YpCbCrPixelRange pixelRange;
    [self be_yuvPixelRange:&pixelRange format:outputFormat];
    
    vImageARGBType argbType = kvImageARGB8888;
    vImageYpCbCrType yuvType = kvImage420Yp8_CbCr8;
    vImage_ARGBToYpCbCr conversionInfo;
    vImage_Flags flags = kvImageNoFlags;
    
    vImage_Error error = vImageConvert_ARGBToYpCbCr_GenerateConversion(kvImage_ARGBToYpCbCrMatrix_ITU_R_601_4, &pixelRange, &conversionInfo, argbType, yuvType, flags);
    if (error != kvImageNoError) {
        NSLog(@"vImageConvert_ARGBToYpCbCr_GenerateConversion error: %ld", error);
        return NO;
    }
    
    error = vImageConvert_ARGB8888To420Yp8_CbCr8(inputBuffer, yBuffer, uvBuffer, &conversionInfo, map, flags);
    if (error != kvImageNoError) {
        NSLog(@"vImageConvert_ARGB8888To420Yp8_CbCr8 error: %ld", error);
        return NO;
    }
    
    return YES;
}

- (BOOL)be_convertRgbaToYuv:(vImage_Buffer *)inputBuffer
                    yBuffer:(vImage_Buffer *)yBuffer
                    uBuffer:(vImage_Buffer *)uBuffer
                    vBuffer:(vImage_Buffer *)vBuffer
                inputFormat:(BEFormatType)inputFormat
               outputFormat:(BEFormatType)outputFormat {
    if (![self be_isRgba:inputFormat] || ![self be_isYuv420Planar:outputFormat]) {
        return NO;
    }
    uint8_t map[4] = {1, 2, 3, 0};
    [self be_permuteMap:map format:inputFormat];
    vImage_YpCbCrPixelRange pixelRange;
    [self be_yuvPixelRange:&pixelRange format:outputFormat];
    
    vImageARGBType argbType = kvImageARGB8888;
    vImageYpCbCrType yuvType = kvImage420Yp8_Cb8_Cr8;
    vImage_ARGBToYpCbCr conversionInfo;
    vImage_Flags flags = kvImageNoFlags;
    
    vImage_Error error = vImageConvert_ARGBToYpCbCr_GenerateConversion(kvImage_ARGBToYpCbCrMatrix_ITU_R_601_4, &pixelRange, &conversionInfo, argbType, yuvType, flags);
    if (error != kvImageNoError) {
        NSLog(@"vImageConvert_ARGBToYpCbCr_GenerateConversion error: %ld", error);
        return NO;
    }
    
    error = vImageConvert_ARGB8888To420Yp8_Cb8_Cr8(inputBuffer, yBuffer, uBuffer, vBuffer, &conversionInfo, map, flags);
    if (error != kvImageNoError) {
        NSLog(@"vImageConvert_ARGB8888To420Yp8_Cb8_Cr8 error: %ld", error);
        return NO;
    }
    
    return YES;
}


- (BOOL)be_convertYuvToRgba:(vImage_Buffer *)yBuffer yvBuffer:(vImage_Buffer *)uvBuffer rgbaBuffer:(vImage_Buffer *)rgbaBuffer inputFormat:(BEFormatType)inputFormat outputFormat:(BEFormatType)outputFormat {
    if (![self be_isYuv420:inputFormat] || ![self be_isRgba:outputFormat]) {
        return NO;
    }
    
    uint8_t map[4] = {1, 2, 3, 0};
    [self be_permuteMap:map format:outputFormat];
    vImage_YpCbCrPixelRange pixelRange;
    [self be_yuvPixelRange:&pixelRange format:inputFormat];
    
    vImageARGBType argbType = kvImageARGB8888;
    vImageYpCbCrType yuvType = kvImage420Yp8_CbCr8;
    vImage_YpCbCrToARGB conversionInfo;
    vImage_Flags flags = kvImageNoFlags;
    
    vImage_Error error = vImageConvert_YpCbCrToARGB_GenerateConversion(kvImage_YpCbCrToARGBMatrix_ITU_R_601_4, &pixelRange, &conversionInfo, yuvType, argbType, flags);
    if (error != kvImageNoError) {
        NSLog(@"vImageConvert_YpCbCrToARGB_GenerateConversion error: %ld", error);
        return NO;
    }
    
    error = vImageConvert_420Yp8_CbCr8ToARGB8888(yBuffer, uvBuffer, rgbaBuffer, &conversionInfo, map, 255, flags);
    if (error != kvImageNoError) {
        NSLog(@"vImageConvert_420Yp8_CbCr8ToARGB8888 error: %ld", error);
        return NO;
    }
    
    return YES;
}

- (BOOL)be_convertYuvToRgba:(vImage_Buffer *)yBuffer uBuffer:(vImage_Buffer *)uBuffer vBuffer:(vImage_Buffer *)vBuffer rgbaBuffer:(vImage_Buffer *)rgbaBuffer inputFormat:(BEFormatType)inputFormat outputFormat:(BEFormatType)outputFormat {
    if (![self be_isYuv420Planar:inputFormat] || ![self be_isRgba:outputFormat]) {
        return NO;
    }
    
    uint8_t map[4] = {1, 2, 3, 0};
    [self be_permuteMap:map format:outputFormat];
    vImage_YpCbCrPixelRange pixelRange;
    [self be_yuvPixelRange:&pixelRange format:inputFormat];
    
    vImageARGBType argbType = kvImageARGB8888;
    vImageYpCbCrType yuvType = kvImage420Yp8_Cb8_Cr8;
    vImage_YpCbCrToARGB conversionInfo;
    vImage_Flags flags = kvImageNoFlags;
    
    vImage_Error error = vImageConvert_YpCbCrToARGB_GenerateConversion(kvImage_YpCbCrToARGBMatrix_ITU_R_601_4, &pixelRange, &conversionInfo, yuvType, argbType, flags);
    if (error != kvImageNoError) {
        NSLog(@"vImageConvert_YpCbCrToARGB_GenerateConversion error: %ld", error);
        return NO;
    }
    
    error = vImageConvert_420Yp8_Cb8_Cr8ToARGB8888(yBuffer, uBuffer, vBuffer, rgbaBuffer, &conversionInfo, map, 255, flags);
    if (error != kvImageNoError) {
        NSLog(@"vImageConvert_420Yp8_Cb8_Cr8ToARGB8888 error: %ld", error);
        return NO;
    }
    
    return YES;
}

- (BEBuffer *)be_getBufferFromCVPixelBuffer:(CVPixelBufferRef)pixelBuffer {
    BEBuffer *buffer = [[BEBuffer alloc] init];
    BEPixelBufferInfo *info = [self getCVPixelBufferInfo:pixelBuffer];
    buffer.width = info.width;
    buffer.height = info.height;
    buffer.format = info.format;
    
    CVPixelBufferLockBaseAddress(pixelBuffer, 0);
    if ([self be_isRgba:info.format]) {
        buffer.buffer = (unsigned char *)CVPixelBufferGetBaseAddress(pixelBuffer);
        buffer.bytesPerRow = (int)CVPixelBufferGetBytesPerRow(pixelBuffer);
    } else if ([self be_isYuv420:info.format]) {
        buffer.yBuffer = (unsigned char *)CVPixelBufferGetBaseAddressOfPlane(pixelBuffer, 0);
        buffer.yBytesPerRow = (int)CVPixelBufferGetBytesPerRowOfPlane(pixelBuffer, 0);
        buffer.uvBuffer = (unsigned char *)CVPixelBufferGetBaseAddressOfPlane(pixelBuffer, 1);
        buffer.uvBytesPerRow = (int)CVPixelBufferGetBytesPerRowOfPlane(pixelBuffer, 1);
        
        buffer.yWidth = (int)CVPixelBufferGetWidthOfPlane(pixelBuffer, 0);
        buffer.yHeight = (int)CVPixelBufferGetHeightOfPlane(pixelBuffer, 0);
        buffer.uvWidth = (int)CVPixelBufferGetWidthOfPlane(pixelBuffer, 1);
        buffer.uvHeight = (int)CVPixelBufferGetHeightOfPlane(pixelBuffer, 1);
    } else if ([self be_isYuv420Planar:info.format]) {
        buffer.yBuffer = (unsigned char *)CVPixelBufferGetBaseAddressOfPlane(pixelBuffer, 0);
        buffer.yBytesPerRow = (int)CVPixelBufferGetBytesPerRowOfPlane(pixelBuffer, 0);
        buffer.uBuffer = (unsigned char *)CVPixelBufferGetBaseAddressOfPlane(pixelBuffer, 1);
        buffer.uBytesPerRow = (int)CVPixelBufferGetBytesPerRowOfPlane(pixelBuffer, 1);
        buffer.vBuffer = (unsigned char *)CVPixelBufferGetBaseAddressOfPlane(pixelBuffer, 2);
        buffer.vBytesPerRow = (int)CVPixelBufferGetBytesPerRowOfPlane(pixelBuffer, 2);
        
        buffer.yWidth = (int)CVPixelBufferGetWidthOfPlane(pixelBuffer, 0);
        buffer.yHeight = (int)CVPixelBufferGetHeightOfPlane(pixelBuffer, 0);
        buffer.uvWidth = (int)CVPixelBufferGetWidthOfPlane(pixelBuffer, 1);
        buffer.uvHeight = (int)CVPixelBufferGetHeightOfPlane(pixelBuffer, 1);

    }
    CVPixelBufferUnlockBaseAddress(pixelBuffer, 0);
    
    return buffer;
}

- (BOOL)be_isRgb:(BEFormatType)format {
    return format == BE_RGB || format == BE_BGR;
}

- (BOOL)be_isRgba:(BEFormatType)format {
    return format == BE_RGBA || format == BE_BGRA;
}

- (BOOL)be_isYuv420Planar:(BEFormatType)format {
    return format == BE_YUVY420;
}

- (BOOL)be_isYuv420:(BEFormatType)format {
    return format == BE_YUV420F || format == BE_YUV420V;
}

- (void)be_permuteMap:(uint8_t *)map format:(BEFormatType)format {
    int r = map[0], g = map[1], b = map[2], a = map[3];
    switch (format) {
        case BE_RGBA:
            map[0] = r;
            map[1] = g;
            map[2] = b;
            map[3] = a;
            break;
        case BE_BGRA:
            map[0] = b;
            map[1] = g;
            map[2] = r;
            map[3] = a;
        default:
            break;
    }
}

- (void)be_yuvPixelRange:(vImage_YpCbCrPixelRange *)pixelRange format:(BEFormatType)format {
    switch (format) {
        case BE_YUV420F:
            pixelRange->Yp_bias = 0;
            pixelRange->CbCr_bias = 128;
            pixelRange->YpRangeMax = 255;
            pixelRange->CbCrRangeMax = 255;
            pixelRange->YpMax = 255;
            pixelRange->YpMin = 0;
            pixelRange->CbCrMax = 255;
            pixelRange->CbCrMin = 0;
            break;
        case BE_YUV420V:
            pixelRange->Yp_bias = 16;
            pixelRange->CbCr_bias = 128;
            pixelRange->YpRangeMax = 235;
            pixelRange->CbCrRangeMax = 240;
            pixelRange->YpMax = 235;
            pixelRange->YpMin = 16;
            pixelRange->CbCrMax = 240;
            pixelRange->CbCrMin = 16;
            break;
        case BE_YUVY420:
            pixelRange->Yp_bias = 16;
            pixelRange->CbCr_bias = 128;
            pixelRange->YpRangeMax = 235;
            pixelRange->CbCrRangeMax = 240;
            pixelRange->YpMax = 235;
            pixelRange->YpMin = 16;
            pixelRange->CbCrMax = 240;
            pixelRange->CbCrMin = 16;
            break;
        default:
            break;
    }
}

- (unsigned char *)be_mallocBufferWithSize:(int)size {
    NSNumber *key = [NSNumber numberWithInt:size];
    if ([[_mallocDict allKeys] containsObject:key]) {
        return [_mallocDict[key] pointerValue];
    }
    while (_mallocDict.count >= MAX_MALLOC_CACHE) {
        [_mallocDict removeObjectForKey:[_mallocDict.allKeys firstObject]];
    }
    NSLog(@"malloc size: %d", size);
    unsigned char *buffer = malloc(size * sizeof(unsigned char));
    _mallocDict[key] = [NSValue valueWithPointer:buffer];
    return buffer;
}

- (CVPixelBufferRef)be_createCVPixelBufferWithWidth:(int)width height:(int)height format:(BEFormatType)format {
    if (_cachedPixelBuffer != nil && USE_CACHE_PIXEL_BUFFER) {
        BEPixelBufferInfo *info = [self getCVPixelBufferInfo:_cachedPixelBuffer];
        if (info.format == format && info.width == width && info.height == height) {
            return _cachedPixelBuffer;
        } else {
            CVBufferRelease(_cachedPixelBuffer);
        }
    }
    NSLog(@"create CVPixelBuffer");
    CVPixelBufferRef pixelBuffer = [self be_createPixelBufferFromPool:[self getOsType:format] heigth:height width:width];
    if (USE_CACHE_PIXEL_BUFFER) {
        _cachedPixelBuffer = pixelBuffer;
    }
    return pixelBuffer;
}

- (CVPixelBufferRef)be_createPixelBufferFromPool:(OSType)type heigth:(int)height width:(int)width {
    NSString* key = [NSString stringWithFormat:@"%u_%d_%d", (unsigned int)type, height, width];
    CVPixelBufferPoolRef pixelBufferPool = NULL;
    NSValue *bufferPoolAddress = [self.pixelBufferPoolDict objectForKey:key];
    
    /// Means we have not allocate such a pool
    if (!bufferPoolAddress) {
        pixelBufferPool = [self be_createPixelBufferPool:type heigth:height width:width];
        bufferPoolAddress = [NSValue valueWithPointer:pixelBufferPool];
        [self.pixelBufferPoolDict setValue:bufferPoolAddress forKey:key];
    }else {
        pixelBufferPool = [bufferPoolAddress pointerValue];
    }
    
    CVPixelBufferRef buffer = NULL;
    CVReturn ret = CVPixelBufferPoolCreatePixelBuffer(kCFAllocatorDefault, pixelBufferPool, &buffer);
    if (ret != kCVReturnSuccess) {
        NSLog(@"CVPixelBufferCreate error: %d", ret);
        if (ret == kCVReturnInvalidPixelFormat) {
            NSLog(@"only format BGRA and YUV420 can be used");
        }
    }
    return buffer;
}

- (CVPixelBufferPoolRef)be_createPixelBufferPool:(OSType)type heigth:(int)height width:(int)width {
    CVPixelBufferPoolRef pool = NULL;
    
    NSMutableDictionary* attributes = [NSMutableDictionary dictionary];
    
    [attributes setObject:CFBridgingRelease((__bridge_retained CFNumberRef)[NSNumber numberWithBool:YES]) forKey:(NSString*)kCVPixelBufferOpenGLCompatibilityKey];
    if (MTLCreateSystemDefaultDevice()) {
        [attributes setObject:CFBridgingRelease((__bridge_retained CFNumberRef)[NSNumber numberWithBool:YES]) forKey:(NSString*)kCVPixelBufferMetalCompatibilityKey];
    }
    [attributes setObject:[NSNumber numberWithInt:type] forKey:(NSString*)kCVPixelBufferPixelFormatTypeKey];
    [attributes setObject:[NSNumber numberWithInt:width] forKey: (NSString*)kCVPixelBufferWidthKey];
    [attributes setObject:[NSNumber numberWithInt:height] forKey: (NSString*)kCVPixelBufferHeightKey];
    [attributes setObject:@(16) forKey:(NSString*)kCVPixelBufferBytesPerRowAlignmentKey];
    [attributes setObject:[NSDictionary dictionary] forKey:(NSString*)kCVPixelBufferIOSurfacePropertiesKey];
    
    CVReturn ret = CVPixelBufferPoolCreate(kCFAllocatorDefault, NULL, (__bridge CFDictionaryRef)attributes, &pool);
    
    [attributes removeAllObjects];
    if (ret != kCVReturnSuccess){
        NSLog(@"Create pixbuffer pool failed %d", ret);
        return NULL;
    }

    return pool;
}

#pragma mark - getter
- (CVOpenGLESTextureCacheRef)textureCache {
    if (!_textureCache) {
        EAGLContext *context = [EAGLContext currentContext];
        CVReturn ret = CVOpenGLESTextureCacheCreate(kCFAllocatorDefault, NULL, context, NULL, &_textureCache);
        if (ret != kCVReturnSuccess) {
            NSLog(@"create CVOpenGLESTextureCacheRef fail: %d", ret);
        }
    }
    return _textureCache;
}

- (NSMutableDictionary<NSString *,NSValue *> *)pixelBufferPoolDict {
    if (_pixelBufferPoolDict == nil) {
        _pixelBufferPoolDict = [NSMutableDictionary dictionary];
    }
    return _pixelBufferPoolDict;
}

- (BEOpenGLRenderHelper *)renderHelper {
    if (_renderHelper) {
        return _renderHelper;
    }
    
    _renderHelper = [[BEOpenGLRenderHelper alloc] init];
    return _renderHelper;
}

+ (void)setTextureCacheNum:(int)num {
    TEXTURE_CACHE_NUM = num;
    MAX_MALLOC_CACHE = num;
}

+ (void)setUseCachedPixelBuffer:(bool)use {
    USE_CACHE_PIXEL_BUFFER = use;
}

+ (int)textureCacheNum {
    return TEXTURE_CACHE_NUM;
}

+ (bool)useCachedPixelBuffer {
    return USE_CACHE_PIXEL_BUFFER;
}

@end
