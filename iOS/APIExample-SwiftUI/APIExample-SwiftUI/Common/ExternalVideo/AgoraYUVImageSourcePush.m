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
@property(nonatomic, copy) NSString *fileName;
@end

@implementation AgoraYUVImageSourcePush

- (instancetype)initWithSize: (CGSize)size fileName: (NSString *)fileName frameRate: (int)frameRate {
    if(self = [super init]) {
        self.fileName = fileName;
        self.videoW = size.width;
        self.videoH = size.height;
        self.frameRate = frameRate;
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
    dispatch_source_set_timer(
                              self.timer,
                              dispatch_time(DISPATCH_TIME_NOW, 0.0 * NSEC_PER_SEC),//Start event, start after 0 seconds
                              self.frameRate / 100.0 * NSEC_PER_SEC,//Interval seconds
                              0
                              );
}

-(void)startSource
{
    if(self.timer == nil) {
        [self initTimer];
    }
    NSUInteger frameSizeY = self.videoW*self.videoH;
    NSUInteger frameSizeU = self.videoW*self.videoH/4;
    NSUInteger frameSizeV = self.videoW*self.videoH/4;
    
    __block NSUInteger i = 0;
    NSTimeInterval sT = NSDate.date.timeIntervalSince1970;
    
    __weak AgoraYUVImageSourcePush * weakSelf = self;
    dispatch_source_set_event_handler(self.timer, ^{
        if(weakSelf.delegate) {
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
            CVPixelBufferRef pixelBuffer = [MediaUtils i420ToPixelBuffer:(void *)dataY.bytes
                                                                    srcU:(void *)dataU.bytes
                                                                    srcV:(void *)dataV.bytes
                                                                   width:weakSelf.videoW
                                                                  height:weakSelf.videoH];
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

    dispatch_resume(self.timer);
}

-(void)stopSource
{
    if(self.timer) {
        dispatch_source_cancel(self.timer);
        self.timer = nil;
    }
}

@end
