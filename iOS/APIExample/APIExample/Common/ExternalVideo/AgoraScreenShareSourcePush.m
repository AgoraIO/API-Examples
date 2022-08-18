//
//  AgoraScreenShareSourcePush.m
//  AgoraShareSrceen
//
//  Created by XC on 2021/7/3.
//

#import "AgoraScreenShareSourcePush.h"
#import "GCDAsyncSocket.h"
#import <AgoraRtcKit/AgoraRtcEngineKit.h>
#import "TransferFrame.h"
#include "CircularBuffer.h"
#import <AVKit/AVKit.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netinet/in_systm.h>
#include <netdb.h>
#include <arpa/inet.h>
#import <ifaddrs.h>

#define SOCKET_TIMEOUT  (60 * 10)

typedef void (^imageBlock)(UIImage *image);

@interface AgoraScreenShareSourcePush()<AVCaptureVideoDataOutputSampleBufferDelegate, GCDAsyncSocketDelegate>
{
    CircularBuffer recvVideoBuffer;
    CircularBuffer recvAudioBuffer;
}

@property (nonatomic,weak) id<AgoraScreenShareSourcePushDelegate> delegate;
@property (strong, nonatomic) GCDAsyncSocket *serverSocket;
@property (strong, nonatomic) GCDAsyncSocket *videoSocket;
@property (strong, nonatomic) GCDAsyncSocket *audioSocket;

@property (nonatomic, assign) BOOL getOneDidCaptureVideoFrame;
@property (nonatomic, copy) imageBlock imageBlock;
@property (nonatomic, assign) uint16_t port;

@end

@implementation AgoraScreenShareSourcePush

+ (dispatch_queue_t)RECV_SERIAL_QUEUE {
    static dispatch_queue_t serialQueue = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        serialQueue = dispatch_queue_create("io.agora.recv.data", DISPATCH_QUEUE_SERIAL);
    });
    return serialQueue;
}

- (void)dealloc {
    if (recvVideoBuffer != NULL) {
        CircularBufferFree(recvVideoBuffer);
    }
}

- (instancetype)initWithDelegate:(id<AgoraScreenShareSourcePushDelegate>)delegate port:(uint16_t) port
{
    self = [super init];
    if (self) {
        self.delegate = delegate;
        self.port = port;
        recvVideoBuffer = CircularBufferCreate(30 * 1024 * 1024);
        recvAudioBuffer = CircularBufferCreate(5 * 1024 * 1024);
    }
    return self;
}

- (void)startCapture {
    if (self.serverSocket != nil) {
        return;
    }
    self.serverSocket = [[GCDAsyncSocket alloc] initWithDelegate:self delegateQueue:[[self class] RECV_SERIAL_QUEUE]];
    self.serverSocket.autoDisconnectOnClosedReadStream = YES;
    __autoreleasing NSError *error = nil;
    BOOL result = [self.serverSocket acceptOnPort:self.port error:&error];
    if(error || !result){
        NSLog(@"server socket result:%@ error:%@", error, @(result));
    }else {
        NSLog(@"server socket:%@ listen %d", self.serverSocket, self.port);
    }
}

- (void)stopCapture {
    if (self.serverSocket != nil) {
        [self.serverSocket disconnect];
    }
    self.serverSocket = nil;
    self.videoSocket = nil;
    self.audioSocket = nil;
}

int findMagic(char *buffer, int len)
{
    int i = 0;
    if(buffer == NULL || len < 4)
        return -1;
    while(i <= len - 4) {
        if(*(int32_t *)(buffer+i) == MAGIC_NUMBER)
            return i;
    }
    
    return -1;
}

//- (CMSampleBufferRef)bufferConvertAudioBuffer:(void *)buffer frameSize:(int64_t)frameSize {
//    AVAudioChannelLayout *chLayout = [[AVAudioChannelLayout alloc] initWithLayoutTag:kAudioChannelLayoutTag_Stereo];
//    AVAudioFormat *pcmBuffer = [[AVAudioFormat alloc] initWithCommonFormat:AVAudioPCMFormatFloat32
//                                                              sampleRate:48000
//                                                              interleaved:NO
//                                                            channelLayout:chLayout];
//
//    // 创建一个基于上述配置的帧缓存结构
//    AVAudioPCMBuffer *thePCMBuffer = [[AVAudioPCMBuffer alloc] initWithPCMFormat:pcmBuffer frameCapacity:1024];
//    thePCMBuffer.frameLength = thePCMBuffer.frameCapacity;
//
//    // 初始化数据区
//    for (AVAudioChannelCount ch = 0; ch < pcmBuffer.channelCount; ++ch) {
//        memset(thePCMBuffer.floatChannelData[ch], 0, thePCMBuffer.frameLength * pcmBuffer.streamDescription->mBytesPerFrame);
//    }
//
//    memcpy(thePCMBuffer.floatChannelData[0], buffer, frameSize);
//
//    AudioStreamBasicDescription asbd = *thePCMBuffer.format.streamDescription;
//    CMSampleBufferRef sampleBuffer = NULL;
//    CMAudioFormatDescriptionRef format = NULL;
//
//    OSStatus error = CMAudioFormatDescriptionCreate(kCFAllocatorDefault, &asbd, 0, NULL, 0, NULL, NULL, &format);
//    if (error != noErr) {
//        return nil;
//    }
//
//    //CMTime PTS = CMTimeMake([AVAudioTime secondsForHostTime:ATS.hostTime] * ATS.sampleRate, ATS.sampleRate);
//    CMTime PTS = CMTimeMake(CACurrentMediaTime()*1000, 1000);
//    CMItemCount count = thePCMBuffer.audioBufferList->mBuffers[1].mDataByteSize / asbd.mBytesPerFrame;
//
//    error = CMAudioSampleBufferCreateReadyWithPacketDescriptions(kCFAllocatorDefault,
//                                                                 NULL,
//                                                                 format,
//                                                                 count,
//                                                                 PTS,
//                                                                 NULL,
//                                                                 &sampleBuffer);
//
//    if (error != noErr) {
//        CFRelease(format);
//        NSLog(@"CMAudioSampleBufferCreateReadyWithPacketDescriptions returned error: %d", (int)error);
//        return nil;
//    }
//
//    error = CMSampleBufferSetDataBufferFromAudioBufferList(sampleBuffer, kCFAllocatorDefault, kCFAllocatorDefault, kCMSampleBufferFlag_AudioBufferList_Assure16ByteAlignment, thePCMBuffer.audioBufferList);
//
//    if (error != noErr) {
//        CFRelease(format);
//        NSLog(@"CMSampleBufferSetDataBufferFromAudioBufferList returned error: %d", (int)error);
//        return nil;
//    }
//
//    CFRelease(format);
//    return sampleBuffer;
//}

- (CVPixelBufferRef)bufferConvertPixelBuffer:(void *)buffer width:(size_t)bufferWidth height:(size_t)bufferHeight
{
    // Copy the pixel buffer
    CVPixelBufferRef pixelBufferCopy = NULL;
    CVReturn status = CVPixelBufferCreate(kCFAllocatorDefault,
                                          bufferWidth,
                                          bufferHeight,
                                          kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange,
                                          NULL,
                                          &pixelBufferCopy);
    if (status != kCVReturnSuccess) {
        NSLog(@"bufferConvertPixelBuffer failed !!!");
    }
    
    CVPixelBufferLockBaseAddress(pixelBufferCopy, 0);
    
    uint8_t *yDestPlane = (uint8_t *)CVPixelBufferGetBaseAddressOfPlane(pixelBufferCopy, 0);
    memcpy(yDestPlane, buffer, bufferWidth * bufferHeight);
    uint8_t *uvDestPlane = (uint8_t *)CVPixelBufferGetBaseAddressOfPlane(pixelBufferCopy, 1);
    memcpy(uvDestPlane, (unsigned char *)buffer + bufferWidth * bufferHeight, bufferWidth * bufferHeight/2);
    
    CVPixelBufferUnlockBaseAddress(pixelBufferCopy, 0);
    
    return pixelBufferCopy;
}

- (void)handleRecvVideoData {
   size_t frameSize = sizeof(TransferFrame);
   size_t recvLength = CircularBufferGetDataSize(recvVideoBuffer);
   if (recvLength < frameSize) {
       //NSLog(@"handleRecvVideoData return");
       CircularBufferReset(recvVideoBuffer);
       return;
   }
   
   TransferFrame frame = {0};
   CircularBufferRead(recvVideoBuffer, frameSize, &frame);
   
   if (frame.magic == MAGIC_NUMBER && frame.length <= recvLength ) {
       CircularBufferPop(recvVideoBuffer, frameSize, NULL);
       void *buffer = malloc(frame.length);
       CircularBufferPop(recvVideoBuffer, frame.length, buffer);
       
       if (frame.frameType == 1) {
           CVPixelBufferRef pixelBuffer = [self bufferConvertPixelBuffer:buffer
                                                                   width:frame.width
                                                                  height:frame.height];
           CMTime time = CMTimeMake(frame.timevalues, (int32_t)frame.timescale);
           int rotation = (int)frame.rotation;
           switch (rotation) {
               case 2:
                   rotation = 180;
                   break;
               case 1:
                   rotation = 90;
                   break;
               case 3:
                   rotation = 270;
                   break;
               default:
                   break;
           }
           
           //CVPixelBufferLockBaseAddress(pixelBuffer, kCVPixelBufferLock_ReadOnly);
           if ([self.delegate respondsToSelector:@selector(screenSourceCapture:outputBuffer:rotation:time:)]) {
               [self.delegate screenSourceCapture:self outputBuffer:pixelBuffer rotation:rotation time:time];
           }
           //CVPixelBufferUnlockBaseAddress(pixelBuffer, kCVPixelBufferLock_ReadOnly);
           // screenshot
           if (self.getOneDidCaptureVideoFrame) {
               self.getOneDidCaptureVideoFrame = false;
               [self CVBufferRefToImage:pixelBuffer];
           }
           CVPixelBufferRelease(pixelBuffer);
       }
       
       free(buffer);
   }
}

- (void)CVBufferRefToImage:(CVBufferRef)imageBuffer {
    if (self.imageBlock) {
        CIImage *ciImage = [CIImage imageWithCVPixelBuffer:imageBuffer];

        CIContext *temporaryContext = [CIContext contextWithOptions:nil];
        CGImageRef videoImage = [temporaryContext
                           createCGImage:ciImage
                           fromRect:CGRectMake(0, 0,
                                  CVPixelBufferGetWidth(imageBuffer),
                                  CVPixelBufferGetHeight(imageBuffer))];
        UIImage *uiImage = [UIImage imageWithCGImage:videoImage];
        CGImageRelease(videoImage);
        self.imageBlock(uiImage);
    }
}

- (void)handleRecvAudioData {
    size_t frameSize = sizeof(TransferFrame);
    size_t recvLength = CircularBufferGetDataSize(recvAudioBuffer);
    if (recvLength < frameSize) {
        CircularBufferReset(recvAudioBuffer);
        return;
    }
    
    TransferFrame frame = {0};
    CircularBufferRead(recvAudioBuffer, frameSize, &frame);
    
    if (frame.magic == MAGIC_NUMBER && frame.length <= recvLength ) {
      
        CircularBufferPop(recvAudioBuffer, frameSize, NULL);

//        NSLog(@"handle recv frame size:%@ length:%@ w:%@,h:%@ rotation:%@ frametype:%@",@(frameSize),
//              @(frame.length),
//              @(frame.width),
//              @(frame.height),
//              @(frame.rotation),
//              @(frame.frameType));
        
        void *buffer = malloc(frame.length);
        CircularBufferPop(recvAudioBuffer, frame.length, buffer);
        
        if (frame.frameType == 2) {
            if ([self.delegate respondsToSelector:@selector(pushAudioAppFrame:frameSize:)]) {
                [self.delegate pushAudioAppFrame:buffer frameSize:frame.length];
            }
//            if ([self.delegate respondsToSelector:@selector(pushAudioAppFrame:)]) {
//                CMSampleBufferRef audoFrame = [self bufferConvertAudioBuffer:buffer frameSize:frame.length];
//                if (audoFrame != nil) {
//                    [self.delegate pushAudioAppFrame:audoFrame];
//                }
//            }
        } else if (frame.frameType == 3) {
            //if ([self.delegate respondsToSelector:@selector(pushAudioMicFrame:frameSize:)]) {
                //[self.delegate pushAudioMicFrame:buffer frameSize:frame.length];
            //}
        }
    
        free(buffer);
    }
    
}



/**
 读取客户端发送的数据

 @param sock 客户端的Socket
 @param data 客户端发送的数据
 @param tag 当前读取的标记
 */
- (void)socket:(GCDAsyncSocket *)sock didReadData:(NSData *)data withTag:(long)tag
{
    if (sock == self.videoSocket) {
        CircularBufferPush(recvVideoBuffer, (void *)data.bytes, data.length);
        [self handleRecvVideoData];
    }
    
    if (sock == self.audioSocket) {
        CircularBufferPush(recvAudioBuffer, (void *)data.bytes, data.length);
        [self handleRecvAudioData];
    }
    
    [sock readDataWithTimeout:SOCKET_TIMEOUT tag:0];

}

/**
 * Conditionally called if the read stream closes, but the write stream may still be writeable.
 *
 * This delegate method is only called if autoDisconnectOnClosedReadStream has been set to NO.
 * See the discussion on the autoDisconnectOnClosedReadStream method for more information.
**/
- (void)socketDidCloseReadStream:(GCDAsyncSocket *)sock {
    
    if (sock == self.videoSocket) {
        CircularBufferReset(recvVideoBuffer);
    }
    if (sock == self.audioSocket) {
        CircularBufferReset(recvAudioBuffer);
    }
}

- (void)socketDidDisconnect:(GCDAsyncSocket *)sock withError:(nullable NSError *)err {
    NSLog(@"server -- socketDidDisconnect error:%@", err);
    if(self.videoSocket.isDisconnected) {
        CircularBufferReset(recvVideoBuffer);
        NSLog(@"server videoSocket socketDidDisconnect error:%@", err);
        self.videoSocket = nil;
    }
    if (self.audioSocket.isDisconnected) {
        CircularBufferReset(recvAudioBuffer);
        NSLog(@"server audioSocket socketDidDisconnect error:%@", err);
        self.audioSocket = nil;
    }
    
    if (self.videoSocket == nil && self.audioSocket == nil) {
        if ([self.delegate respondsToSelector:@selector(screenShareStop)]) {
            [self.delegate screenShareStop];
        }
    }
}

- (void)socket:(GCDAsyncSocket *)sock didAcceptNewSocket:(GCDAsyncSocket *)newSocket {
    
    if (nil == self.videoSocket) {
        self.videoSocket = newSocket;
        [self.videoSocket enableBackgroundingOnSocket];
    }else if (nil == self.audioSocket) {
        self.audioSocket = newSocket;
        [self.audioSocket enableBackgroundingOnSocket];
    }
    
    [newSocket readDataWithTimeout:SOCKET_TIMEOUT tag:0];
    
    NSLog(@"connectedPort:%@, localPort:%@, connectedAddress:%@, localAddress:%@", @(newSocket.connectedPort),
          @(newSocket.localPort),
          newSocket.connectedAddress,
          newSocket.localAddress);
}

- (void)localSnapshot:(void (^ _Nullable)(UIImage * _Nonnull image))completion {
    self.imageBlock = completion;
    self.getOneDidCaptureVideoFrame = true;
}


@end
