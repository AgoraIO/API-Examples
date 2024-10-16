//
//  AgoraYUVImageSourcePush.m
//  APIExample
//
//  Created by 张乾泽 on 2020/11/6.
//  Copyright © 2020 Agora Corp. All rights reserved.
//

#import "AgoraYUVImageSourcePush.h"
#import <UIKit/UIKit.h>
#import "MediaUtils.h"

@interface AgoraYUVImageSourcePush ()
@property(nonatomic, strong) NSData* yuvData;
@property (nonatomic,strong)dispatch_source_t timer;
@property(nonatomic, assign) int videoW;
@property(nonatomic, assign) int videoH;
@property(nonatomic, strong)NSFileHandle *fileHandle;
@property(nonatomic, assign) int frameRate;
@property(nonatomic, assign) BOOL isHDR;
@property(nonatomic, copy) NSString *fileName;
@end

@implementation AgoraYUVImageSourcePush

- (instancetype)initWithSize: (CGSize)size fileName: (NSString *)fileName frameRate: (int)frameRate isHDR: (BOOL)isHDR {
    if(self = [super init]) {
        self.fileName = fileName;
        self.videoW = size.width;
        self.videoH = size.height;
        self.frameRate = frameRate;
        self.isHDR = isHDR;
        [self loadData];
    }
    return self;
}

- (void)loadData {
    NSString *yuvPath = [[NSBundle mainBundle] pathForResource:self.fileName ofType:@"yuv"];
    self.fileHandle = [NSFileHandle fileHandleForReadingAtPath:yuvPath];
}

- (void) initTimer {
    self.timer = dispatch_source_create(DISPATCH_SOURCE_TYPE_TIMER, 0, 0, dispatch_get_global_queue(0, 0));
    /**
    Timer settings
    @param Timer
    @param When to start
    @param Timer delay duration
    @param Interval in seconds for execution
    @param Allowed margin of error
    */
    // GCD requires the time to be provided in nanoseconds, so multiply seconds by NSEC_PER_SEC.
    dispatch_source_set_timer(
                              self.timer,
                              dispatch_time(DISPATCH_TIME_NOW, 0.0 * NSEC_PER_SEC),
                              self.frameRate / 100.0 * NSEC_PER_SEC,
                              0
                              );
}

-(void)startSource
{
    if(self.timer == nil) {
        [self initTimer];
    }
    NSUInteger scale = self.isHDR ? 2 : 1;
    NSUInteger frameSizeY = scale * self.videoW*self.videoH;
    NSUInteger frameSizeU = scale * self.videoW*self.videoH / 4;
    NSUInteger frameSizeV = scale * self.videoW*self.videoH / 4;
    
    __block NSUInteger i = 0;
    NSTimeInterval sT = NSDate.date.timeIntervalSince1970;
    
    __weak AgoraYUVImageSourcePush * weakSelf = self;
    dispatch_source_set_event_handler(self.timer, ^{
        if(weakSelf.delegate) {
            CVPixelBufferRef pixelBuffer = NULL;
            if (self.isHDR) {
                pixelBuffer = [weakSelf yuvToHDRPixelBufferWithWidth:self.videoW height:self.videoH];
            } else {
                NSData *dataY = [weakSelf.fileHandle readDataOfLength:frameSizeY];
                NSData *dataU = [weakSelf.fileHandle readDataOfLength:frameSizeU];
                NSData *dataV = [weakSelf.fileHandle readDataOfLength:frameSizeV];
                
                if (dataY.length==0 || dataV.length==0 || dataU.length==0) {
                    i = 0;
                    [weakSelf loadData];
                    dataY = [weakSelf.fileHandle readDataOfLength:frameSizeY];
                    dataU = [weakSelf.fileHandle readDataOfLength:frameSizeU];
                    dataV = [weakSelf.fileHandle readDataOfLength:frameSizeV];
                }
                pixelBuffer = [MediaUtils i420ToPixelBuffer:(void *)dataY.bytes
                                                       srcU:(void *)dataU.bytes
                                                       srcV:(void *)dataV.bytes
                                                      width:weakSelf.videoW
                                                     height:weakSelf.videoH];
            }
            [weakSelf.delegate onVideoFrame:pixelBuffer size:CGSizeMake(weakSelf.videoW, weakSelf.videoH) trackId: self.trackId rotation:0];
            CVPixelBufferRelease(pixelBuffer);
            NSTimeInterval cT = NSDate.date.timeIntervalSince1970;
            NSTimeInterval dT = cT-sT;
            NSTimeInterval tT = i/25.0;
            if (dT<tT) {
                [NSThread sleepForTimeInterval:tT-dT];
            }
            i++;
        }
    });
    
    // Start Timer
    dispatch_resume(self.timer);
}

-(void)stopSource
{
    if(self.timer) {
        dispatch_source_cancel(self.timer);
        self.timer = nil;
    }
}

- (CVPixelBufferRef)yuvToHDRPixelBufferWithWidth:(NSInteger)width height:(NSInteger)height {
    // Create pixel buffer
    NSDictionary *pixelBufferAttributes = @{
        (NSString *)kCVPixelBufferCGImageCompatibilityKey: @YES,
        (NSString *)kCVPixelBufferCGBitmapContextCompatibilityKey: @YES,
        (NSString *)kCVPixelBufferWidthKey: @(width),
        (NSString *)kCVPixelBufferHeightKey: @(height),
        (NSString *)kCVPixelBufferPixelFormatTypeKey: @(kCVPixelFormatType_420YpCbCr10BiPlanarFullRange),
        (NSString *)kCVPixelBufferMetalCompatibilityKey: @YES,
        (NSString *)kCVPixelBufferPoolAllocationThresholdKey: @1
    };

    CVPixelBufferRef pixelBuffer = NULL;
    CVReturn status = CVPixelBufferCreate(kCFAllocatorDefault,
                                          width,
                                          height,
                                          kCVPixelFormatType_420YpCbCr10BiPlanarFullRange,
                                          (__bridge CFDictionaryRef)pixelBufferAttributes,
                                          &pixelBuffer);

    if (status != kCVReturnSuccess) {
        NSLog(@"Error creating pixel buffer: %d", status);
        return NULL;
    }

    // Lock pixel buffer for writing
    CVPixelBufferLockBaseAddress(pixelBuffer, 0);

    // Get pointers to Y plane and UV plane
    uint16_t *yBuffer = (uint16_t *)CVPixelBufferGetBaseAddressOfPlane(pixelBuffer, 0);
    uint16_t *uvBuffer = (uint16_t *)CVPixelBufferGetBaseAddressOfPlane(pixelBuffer, 1);

    // Move to the beginning of the file
    [self.fileHandle seekToFileOffset:0];

    // Read Y data
    NSData *yData = [self.fileHandle readDataOfLength:sizeof(uint16_t) * width * height];
    [yData getBytes:yBuffer length:yData.length];

    // Read UV data; UV plane is usually half the width and height of Y plane
    [self.fileHandle seekToFileOffset:sizeof(uint16_t) * width * height]; // Move to the UV data offset
    NSData *uvData = [self.fileHandle readDataOfLength:sizeof(uint16_t) * (width / 2) * (height / 2) * 2];
    [uvData getBytes:uvBuffer length:uvData.length];

    // Unlock pixel buffer
    CVPixelBufferUnlockBaseAddress(pixelBuffer, 0);

    // Allocate memory for U and V planes
    size_t uvSize = (width / 2) * (height / 2);
    uint16_t *uPlane = (uint16_t *)malloc(uvSize * sizeof(uint16_t));
    uint16_t *vPlane = (uint16_t *)malloc(uvSize * sizeof(uint16_t));
    
    if (!uPlane || !vPlane) {
        NSLog(@"Failed to allocate memory for U or V plane");
        CVPixelBufferRelease(pixelBuffer);
        free(uPlane);
        free(vPlane);
        return NULL;
    }

    // Separate U and V data
    for (size_t i = 0; i < uvSize; i++) {
        uPlane[i] = uvBuffer[i];      // U component
        vPlane[i] = uvBuffer[uvSize + i];  // V component
    }

    // Lock pixel buffer for writing
    CVPixelBufferLockBaseAddress(pixelBuffer, 0);

    // Get pointer to UV plane
    uint16_t *uvPlane = (uint16_t *)CVPixelBufferGetBaseAddressOfPlane(pixelBuffer, 1);
    
    if (uvPlane == NULL) {
        NSLog(@"UV plane is NULL");
        free(uPlane);
        free(vPlane);
        CVPixelBufferUnlockBaseAddress(pixelBuffer, 0);
        CVPixelBufferRelease(pixelBuffer); 
        return NULL;
    }

    // Interleave U and V data into uvPlane
    for (size_t i = 0; i < uvSize; i++) {
        uvPlane[i * 2] = uPlane[i];        // U component
        uvPlane[i * 2 + 1] = vPlane[i];    // V component
    }

    // Unlock pixel buffer
    CVPixelBufferUnlockBaseAddress(pixelBuffer, 0);

    // Normalize Y data to [0, 65535]
    for (size_t i = 0; i < width * height; i++) {
        yBuffer[i] = (uint16_t)(yBuffer[i] * (65535.0 / 1023.0)); // Normalize Y component
    }

    // Process U and V data
    for (size_t i = 0; i < uvSize; i++) {
        // Ensure uvPlane is valid
        if (uvPlane == NULL) {
            NSLog(@"UV plane is NULL during processing");
            break; // Exit loop to avoid crash
        }

        // Normalize U component
        uint16_t uValue = uPlane[i]; // U
        uValue = (uint16_t)((uValue - 512) * (65535.0 / 511.0) + 32768);

        // Normalize V component
        uint16_t vValue = vPlane[i]; // V
        vValue = (uint16_t)((vValue - 512) * (65535.0 / 511.0) + 32768);

        // Write back normalized values
        uvPlane[i * 2] = uValue;
        uvPlane[i * 2 + 1] = vValue;
    }

    // Free temporary storage
    free(uPlane);
    free(vPlane);

    return pixelBuffer;
}

@end
