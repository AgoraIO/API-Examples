//
//  MediaUtils.m
//  APIExample
//
//  Created by Arlin on 2022/4/12.
//  Copyright © 2022 Agora Corp. All rights reserved.
//

#import "MediaUtils.h"
#import <AppKit/AppKit.h>

@implementation MediaUtils

+ (CVPixelBufferRef)i420ToPixelBuffer:(void *)srcY srcU:(void *)srcU srcV:(void *)srcV width:(int)width height:(int)height {
    int size = width * height * 3 / 2;
    int yLength = width * height;
    int uLength = yLength / 4;
    if (srcY == NULL) {
        return nil;
    }
    unsigned char *buf = (unsigned char *)malloc(size);
    memcpy(buf, srcY, yLength);
    memcpy(buf + yLength, srcU, uLength);
    memcpy(buf + yLength + uLength, srcV, uLength);
    
    unsigned char * NV12buf = (unsigned char *)malloc(size);
    [self yuv420p_to_nv12:buf nv12:NV12buf width:width height:height];
    
    free(buf);
    
    int w = width;
    int h = height;
    NSDictionary *pixelAttributes = @{(NSString*)kCVPixelBufferIOSurfacePropertiesKey:@{}};
    CVPixelBufferRef pixelBuffer = NULL;
    CVReturn result = CVPixelBufferCreate(kCFAllocatorDefault,
                                          w,
                                          h,
                                          kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange,
                                          (__bridge CFDictionaryRef)(pixelAttributes),
                                          &pixelBuffer);
    if (result != kCVReturnSuccess) {
        NSLog(@"Unable to create cvpixelbuffer %d", result);
        free(NV12buf);
        return  nil;
    }
    
    CVPixelBufferLockBaseAddress(pixelBuffer,0);
    void *yDestPlane = CVPixelBufferGetBaseAddressOfPlane(pixelBuffer, 0);
    
    // Here y_ch0 is Y-Plane of YUV(NV12) data.
    unsigned char *y_ch0 = NV12buf;
    unsigned char *y_ch1 = NV12buf + w * h;
    memcpy(yDestPlane, y_ch0, w * h);
    void *uvDestPlane = CVPixelBufferGetBaseAddressOfPlane(pixelBuffer, 1);
    
    // Here y_ch1 is UV-Plane of YUV(NV12) data.
    memcpy(uvDestPlane, y_ch1, w * h * 0.5);
    CVPixelBufferUnlockBaseAddress(pixelBuffer, 0);
    
    free(NV12buf);
    return pixelBuffer;
}


+ (NSData *)dataFromPixelBuffer:(CVPixelBufferRef)pixelBuffer {
    NSMutableData* YUVMutData;
    CVPixelBufferLockBaseAddress(pixelBuffer, 0);
    unsigned char* yBaseAddress = (unsigned char*)CVPixelBufferGetBaseAddressOfPlane(pixelBuffer, 0);
    int32_t yBytesPerRow = (int32_t)CVPixelBufferGetBytesPerRowOfPlane(pixelBuffer, 0);
    int32_t width __unused = (int32_t)CVPixelBufferGetWidth(pixelBuffer);
    int32_t height = (int32_t)CVPixelBufferGetHeight(pixelBuffer);
    int32_t yLength = yBytesPerRow*height;
    YUVMutData = [NSMutableData dataWithBytes: yBaseAddress length: yLength];
    
    unsigned char* uvBaseAddress = (unsigned char*)CVPixelBufferGetBaseAddressOfPlane(pixelBuffer, 1);
    int32_t uvBytesPerRow = (int32_t)CVPixelBufferGetBytesPerRowOfPlane(pixelBuffer, 1);
    int32_t uvLength = uvBytesPerRow*height/2;
    NSMutableData* UVData = [NSMutableData dataWithBytes: uvBaseAddress length: uvLength];
    [YUVMutData appendData:UVData];
    NSData* YUVData = [NSData dataWithData:YUVMutData];
    CVPixelBufferUnlockBaseAddress(pixelBuffer, 0);
    return YUVData;
}

+ (NSImage *)i420ToImage:(void *)srcY srcU:(void *)srcU srcV:(void *)srcV width:(int)width height:(int)height {
    int size = width * height * 3 / 2;
    int yLength = width * height;
    int uLength = yLength / 4;
    
    unsigned char *buf = (unsigned char *)malloc(size);
    memcpy(buf, srcY, yLength);
    memcpy(buf + yLength, srcU, uLength);
    memcpy(buf + yLength + uLength, srcV, uLength);
    
    unsigned char * NV12buf = (unsigned char *)malloc(size);
    [self yuv420p_to_nv12:buf nv12:NV12buf width:width height:height];
    
    free(buf);
    
    int w = width;
    int h = height;
    NSDictionary *pixelAttributes = @{(NSString*)kCVPixelBufferIOSurfacePropertiesKey:@{}};
    CVPixelBufferRef pixelBuffer = NULL;
    CVReturn result = CVPixelBufferCreate(kCFAllocatorDefault,
                                          w,
                                          h,
                                          kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange,
                                          (__bridge CFDictionaryRef)(pixelAttributes),
                                          &pixelBuffer);
    if (result != kCVReturnSuccess) {
        NSLog(@"Unable to create cvpixelbuffer %d", result);
        free(NV12buf);
        return  nil;
    }
    
    CVPixelBufferLockBaseAddress(pixelBuffer,0);
    void *yDestPlane = CVPixelBufferGetBaseAddressOfPlane(pixelBuffer, 0);
    
    // Here y_ch0 is Y-Plane of YUV(NV12) data.
    unsigned char *y_ch0 = NV12buf;
    unsigned char *y_ch1 = NV12buf + w * h;
    memcpy(yDestPlane, y_ch0, w * h);
    void *uvDestPlane = CVPixelBufferGetBaseAddressOfPlane(pixelBuffer, 1);
    
    // Here y_ch1 is UV-Plane of YUV(NV12) data.
    memcpy(uvDestPlane, y_ch1, w * h * 0.5);
    CVPixelBufferUnlockBaseAddress(pixelBuffer, 0);
    
    // CIImage Conversion
    CIImage *coreImage = [CIImage imageWithCVPixelBuffer:pixelBuffer];
    CIContext *temporaryContext = [CIContext contextWithOptions:nil];
    CGImageRef videoImage = [temporaryContext createCGImage:coreImage
                                                   fromRect:CGRectMake(0, 0, w, h)];
    
    NSImage *finalImage = [[NSImage alloc] initWithCGImage:videoImage size:NSMakeSize(width, height)];
    CVPixelBufferRelease(pixelBuffer);
    CGImageRelease(videoImage);
    
    free(NV12buf);
    return finalImage;
}

+ (NSImage *)pixelBufferToImage:(CVPixelBufferRef)pixelBuffer withRotationDegrees:(CGFloat)angleInDegrees {
    CIImage *coreImage = [CIImage imageWithCVPixelBuffer:pixelBuffer];
    CGFloat angleInRadians = -angleInDegrees * (M_PI / 180);
    
    CGAffineTransform rotationTransform = CGAffineTransformMakeRotation(angleInRadians);
    CIImage *rotatedImage = [coreImage imageByApplyingTransform:rotationTransform];
    
    CIContext *temporaryContext = [CIContext contextWithOptions:nil];
    CGImageRef videoImage = [temporaryContext createCGImage:rotatedImage fromRect:[rotatedImage extent]];
    
    NSImage *finalImage = [[NSImage alloc] initWithCGImage:videoImage size:NSZeroSize];
    
    CGImageRelease(videoImage);
    return finalImage;
}

+ (void)yuv420p_to_nv12:(unsigned char*)yuv420p nv12:(unsigned char*)nv12 width:(int)width height:(int)height {
    int i, j;
    int y_size = width * height;
    
    unsigned char* y = yuv420p;
    unsigned char* u = yuv420p + y_size;
    unsigned char* v = yuv420p + y_size * 5 / 4;
    
    unsigned char* y_tmp = nv12;
    unsigned char* uv_tmp = nv12 + y_size;
    
    memcpy(y_tmp, y, y_size);
    
    for (j = 0, i = 0; j < y_size * 0.5; j += 2, i++) {
        uv_tmp[j] = u[i];
        uv_tmp[j+1] = v[i];
    }
}


@end
