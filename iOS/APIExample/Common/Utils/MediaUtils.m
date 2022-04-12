//
//  MediaUtils.m
//  APIExample
//
//  Created by Arlin on 2022/4/12.
//  Copyright © 2022 Agora Corp. All rights reserved.
//

#import "MediaUtils.h"
#import <CoreImage/CoreImage.h>

@implementation MediaUtils

+ (UIImage *)i420ToImage:(void *)srcY srcU:(void *)srcU srcV:(void *)srcV width:(int)width height:(int)height {
    int size = width * height * 3 / 2;
    int yLength = width * height;
    int uLength = yLength / 4;
    
    unsigned char *buf = (unsigned char *)malloc(size);
    memcpy(buf, srcY, yLength);
    memcpy(buf + yLength, srcU, uLength);
    memcpy(buf + yLength + uLength, srcV, uLength);
    
    unsigned char * NV12buf = (unsigned char *)malloc(size);
    [self yuv420p_to_nv12:buf nv12:NV12buf width:width height:height];
    
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

    UIImage *finalImage = [[UIImage alloc] initWithCGImage:videoImage
                                                     scale:1.0
                                               orientation:UIImageOrientationRight];
    CVPixelBufferRelease(pixelBuffer);
    CGImageRelease(videoImage);
    return finalImage;
}

+ (UIImage *)pixelBufferToImage: (CVPixelBufferRef)pixelBuffer {
    size_t width = CVPixelBufferGetHeight(pixelBuffer);
    size_t height = CVPixelBufferGetWidth(pixelBuffer);
    
    CIImage *coreImage = [CIImage imageWithCVPixelBuffer:pixelBuffer];
    CIContext *temporaryContext = [CIContext contextWithOptions:nil];
    CGImageRef videoImage = [temporaryContext createCGImage:coreImage
                                                   fromRect:CGRectMake(0, 0, height, width)];
    
    UIImage *finalImage = [[UIImage alloc] initWithCGImage:videoImage
                                                     scale:1.0
                                               orientation:UIImageOrientationUp];
    //    CVPixelBufferRelease(pixelBuffer);
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
