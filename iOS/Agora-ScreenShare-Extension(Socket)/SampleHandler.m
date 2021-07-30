//
//  SampleHandler.m
//  Agora-ScreenShare-Extension(Socket)
//
//  Created by XC on 2021/7/28.
//  Copyright © 2021 Agora Corp. All rights reserved.
//


#import "SampleHandler.h"
#import "TransferFrame.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netinet/in_systm.h>
#include <netdb.h>
#include <arpa/inet.h>
#import <ifaddrs.h>
#import "AgoraAudioTube.h"
#import <net/if.h>

static const int kAudioBufferPoolSize = 1024 * 1024 * 2;
static unsigned char mRecordingAudioAppPool[kAudioBufferPoolSize];
static size_t mRecordingAudioBufferBytes = 0;
static dispatch_semaphore_t semaphoreLock;

static CMSampleBufferRef bufferCopy;
static NSInteger lastSentTs;
static NSTimer *timer;

@interface SampleHandler ()
{

}

@property (assign, nonatomic) int videoSocket;
@property (assign, nonatomic) int audioSocket;

@property (assign, atomic) BOOL isVideoSending;
@property (assign, atomic) BOOL isAudioSending;

@end

@implementation SampleHandler

+ (dispatch_queue_t)SEND_VIDEO_SERIAL_QUEUE {
    static dispatch_queue_t serialQueue = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        serialQueue = dispatch_queue_create("io.agora.send.video.data", DISPATCH_QUEUE_SERIAL);
    });
    return serialQueue;
}

+ (dispatch_queue_t)SEND_AUDIO_SERIAL_QUEUE {
    static dispatch_queue_t serialQueue = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        serialQueue = dispatch_queue_create("io.agora.send.audio.data", DISPATCH_QUEUE_SERIAL);
    });
    return serialQueue;
}

- (instancetype)init {
    self = [super init];
    if (self) {
        semaphoreLock = dispatch_semaphore_create(1);
        self.isAudioSending = NO;
        self.isVideoSending = NO;
    }
    return self;
}

- (NSString *)ip{
    NSString *ip = nil;
    struct ifaddrs *addrs = NULL;
    struct ifaddrs *tmpAddrs = NULL;
    BOOL res = getifaddrs(&addrs);
    if (res == 0) {
        tmpAddrs = addrs;
        while (tmpAddrs != NULL) {
            if(tmpAddrs->ifa_addr->sa_family == AF_INET) {
                // Check if interface is en0 which is the wifi connection on the iPhone
                NSLog(@"%@",[NSString stringWithUTF8String:inet_ntoa(((struct sockaddr_in *)tmpAddrs->ifa_addr)->sin_addr)]);
                if([[NSString stringWithUTF8String:tmpAddrs->ifa_name] isEqualToString:@"en0"] ||
                   [[NSString stringWithUTF8String:tmpAddrs->ifa_name] isEqualToString:@"en1"] ||
                   [[NSString stringWithUTF8String:tmpAddrs->ifa_name] isEqualToString:@"en2"]) {
                    // Get NSString from C String
                    ip = [NSString stringWithUTF8String:inet_ntoa(((struct sockaddr_in *)tmpAddrs->ifa_addr)->sin_addr)];
                }
            }
            tmpAddrs = tmpAddrs->ifa_next;
        }
    }
    // Free memory
    freeifaddrs(addrs);
    NSLog(@"%@",ip);
    return ip;
}

-(NSInteger)port {
    NSUserDefaults *userDefaults = [[NSUserDefaults alloc] initWithSuiteName:@"group.sharescreen.io"];
    NSInteger port = [userDefaults integerForKey:@"screen_share_port"];
    return port == 0 ? 9191 : port;
}

- (int)getSocket {
    
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        NSLog(@"socket err");
    }

    int optVal = 1024 * 1024 * 2;
    int optLen = sizeof(int);
    int res = setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, (char*)&optVal, optLen);
    NSLog(@"set send buffer:%d",res);
        
    NSString *serverHost = [self ip];
    if (serverHost == nil) {
        close(sockfd);
        NSLog(@"serverHost error");
        return NO;
    }
    struct hostent *server = gethostbyname([serverHost UTF8String]);
    if (server == NULL) {
        close(sockfd);
        NSLog(@"get host error");
        return NO;
    }
    
    struct in_addr *remoteAddr = (struct in_addr *)server->h_addr_list[0];
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr = *remoteAddr;
    addr.sin_port = htons([self port]);

    int ret = connect(sockfd, (const struct sockaddr*)&addr, sizeof(addr));
    if (ret == -1) {
        close(sockfd);
        NSLog(@"connect err(%@)", @(ret));
    }

    return sockfd;
}

- (void)broadcastStartedWithSetupInfo:(NSDictionary<NSString *,NSObject *> *)setupInfo {
    // User has requested to start the broadcast. Setup info from the UI extension can be supplied but optional.
    self.videoSocket = [self getSocket];
    self.audioSocket = [self getSocket];
    [AgoraAudioTube registerAudioPreprocessing:^(unsigned char * _Nonnull inAudioFrame, int64_t frameSize) {
        [self pushAudioFrame:inAudioFrame frameSize:frameSize];
    }];
    timer = [NSTimer timerWithTimeInterval:0.1 repeats:YES block:^(NSTimer * _Nonnull timer) {
        if (lastSentTs <= 0) {
            return;
        }
        NSInteger elapse = ([[NSDate date] timeIntervalSince1970] * 1000) - lastSentTs;
        //NSLog(@"sendVideoBuffer:bufferCopy %@", @(elapse));
        if (elapse > 300) {
            if (bufferCopy) {
                [self processSampleBuffer:bufferCopy withType:RPSampleBufferTypeVideo];
            }
        }
    }];
    [[NSRunLoop mainRunLoop] addTimer:timer forMode:NSRunLoopCommonModes];
}

- (void)broadcastPaused {
    // User has requested to pause the broadcast. Samples will stop being delivered.
}

- (void)broadcastResumed {
    // User has requested to resume the broadcast. Samples delivery will resume.
}

- (void)broadcastFinished {
    // User has requested to finish the broadcast.
    [AgoraAudioTube registerAudioPreprocessing:nil];

    if (timer != nil) {
        [timer invalidate];
        timer = nil;
    }
    if (bufferCopy) {
        if (bufferCopy != nil) {
            CFRelease(bufferCopy);
        }
        bufferCopy = nil;
    }
    
    if (self.videoSocket > 0) {
        close(self.videoSocket);
    }
    self.videoSocket = -1;
    
    if (self.audioSocket > 0) {
        close(self.audioSocket);
    }
    self.audioSocket = -1;
}

- (void)processSampleBuffer:(CMSampleBufferRef)sampleBuffer withType:(RPSampleBufferType)sampleBufferType {
    
    switch (sampleBufferType) {
        case RPSampleBufferTypeVideo:
        {
            // Handle video sample buffer
            if (bufferCopy != sampleBuffer) {
                if (bufferCopy != nil) {
                    CFRelease(bufferCopy);
                }
                bufferCopy = (CMSampleBufferRef)CFRetain(sampleBuffer);
            }
            lastSentTs = [[NSDate date] timeIntervalSince1970] * 1000;
            
            if (self.videoSocket < 0 || self.isVideoSending) {
                return;
            }
            
            self.isVideoSending = YES;
            size_t length = 0;

            void *sendData = [self sampleBufferToData:sampleBuffer length:&length];

            usleep(5000);
            dispatch_async([[self class] SEND_VIDEO_SERIAL_QUEUE], ^{
                
                size_t sendLength = send(self.videoSocket, sendData, length, 0);
                if (sendLength == -1) {
                    NSLog(@"video send data err!! length:%@", @(length));
                } else if(sendLength != length) {
                    NSLog(@"video send data err!! length:%@(%@)", @(sendLength),@(length));
                }
                
                free(sendData);
                self.isVideoSending = NO;
                
            });
        }
            break;
        case RPSampleBufferTypeAudioApp:
            // Handle audio sample buffer for app audio
            //[AgoraAudioTube pushAudioCMSampleBuffer:sampleBuffer resampleRate:48000 type:AudioTypeApp];
            [self processAudio:sampleBuffer isAppAudio:YES];
            break;
        case RPSampleBufferTypeAudioMic:
            // Handle audio sample buffer for mic audio
            //[AgoraAudioTube pushAudioCMSampleBuffer:sampleBuffer resampleRate:48000 type:AudioTypeMic];
            //[self processAudio:sampleBuffer isAppAudio:NO];
            break;
        default:
            break;
    }
}

- (void *)sampleBufferToData:(CMSampleBufferRef)sampleBuffer length:(size_t *)pLength {
    
    int8_t rotation = 0;
    CFStringRef RPVideoSampleOrientationKeyRef = (__bridge CFStringRef)RPVideoSampleOrientationKey;
    NSNumber *orientation = (NSNumber *)CMGetAttachment(sampleBuffer, RPVideoSampleOrientationKeyRef, NULL);
    switch ([orientation integerValue]) {
        case kCGImagePropertyOrientationUp:
        case kCGImagePropertyOrientationUpMirrored:
            rotation = 0;
            break;
        case kCGImagePropertyOrientationDown:
        case kCGImagePropertyOrientationDownMirrored:
            rotation = 2;
            break;
        case kCGImagePropertyOrientationLeft:
        case kCGImagePropertyOrientationLeftMirrored:
            rotation = 1;
            break;
        case kCGImagePropertyOrientationRight:
        case kCGImagePropertyOrientationRightMirrored:
            rotation = 3;
            break;
        default:
            break;
    }

    
    CVPixelBufferRef pixelBuffer = CMSampleBufferGetImageBuffer(sampleBuffer);
    
    if (pixelBuffer == nil || kCVReturnSuccess != CVPixelBufferLockBaseAddress(pixelBuffer, kCVPixelBufferLock_ReadOnly)) {
        return NULL;
    }
    
    TransferFrame frame = {0};
    frame.ver = 0x01;
    frame.frameType = 1;
    frame.magic = MAGIC_NUMBER;
    frame.type = 8;
    frame.rotation = rotation;
    
    //CMTime time = CMSampleBufferGetPresentationTimeStamp(sampleBuffer);
    CMTime time = CMTimeMake(CACurrentMediaTime()*1000, 1000);

    frame.timevalues = time.value;
    frame.timescale = time.timescale;
    
    Boolean isPlanar = CVPixelBufferIsPlanar(pixelBuffer);
    
//    size_t srcStrideY = (int)CVPixelBufferGetBytesPerRowOfPlane(pixelBuffer, 0);
//    size_t srcStrideUV = (int)CVPixelBufferGetBytesPerRowOfPlane(pixelBuffer, 1);
    
    size_t width = isPlanar ? CVPixelBufferGetWidthOfPlane(pixelBuffer, 0) : CVPixelBufferGetWidth(pixelBuffer);
    size_t height = isPlanar ? CVPixelBufferGetHeightOfPlane(pixelBuffer, 0) : CVPixelBufferGetHeight(pixelBuffer);
    frame.width = width;
    frame.height = height;
    
    NSUInteger area = width * height;

    void * src0 = CVPixelBufferGetBaseAddressOfPlane(pixelBuffer, 0);
    void * src1 = CVPixelBufferGetBaseAddressOfPlane(pixelBuffer, 1);
    
    NSUInteger dataLength = area * 3 / 2;
    frame.length = (int32_t)dataLength;

    NSUInteger framesize = sizeof(TransferFrame);
    NSUInteger length = dataLength + framesize;
    if (NULL != pLength) {
        *pLength = length;
    }
//    NSLog(@"width:%@ height:%@ length:%@", @(width),@(height),@(length));
    void * dst = malloc(length);
    memcpy(dst, &frame, framesize);

    memcpy((uint8_t *)dst + framesize, src0, area);
    memcpy((uint8_t *)dst + framesize + area, src1, area / 2);
        
    CVPixelBufferUnlockBaseAddress(pixelBuffer, kCVPixelBufferLock_ReadOnly);
    
    return dst;
}

- (void *)sendAudioAppBuffer:(CMSampleBufferRef)sampleBuffer outSize:(size_t *)pOutSize {
    
    AudioBufferList inAudioBufferList;
    CMBlockBufferRef blockBuffer;
    CMSampleBufferGetAudioBufferListWithRetainedBlockBuffer(sampleBuffer,
                                                            NULL,
                                                            &inAudioBufferList,
                                                            sizeof(inAudioBufferList),
                                                            NULL,
                                                            NULL,
                                                            0,
                                                            &blockBuffer);
    
    AudioBuffer buffer = inAudioBufferList.mBuffers[0];
    uint8_t* p = (uint8_t*)buffer.mData;
    for (int i = 0; i < buffer.mDataByteSize; i += 2) {
        uint8_t tmp;
        tmp = p[i];
        p[i] = p[i + 1];
        p[i + 1] = tmp;
    }
    
    void *data = [self sendAudioBuffer:p frameSize:buffer.mDataByteSize outSize:pOutSize frameType:2];
    
    if (blockBuffer) {
        CFRelease(blockBuffer);
    }
    
    return data;
    
}

- (void *)sendAudioMicBuffer:(CMSampleBufferRef)sampleBuffer outSize:(size_t *)pOutSize {
    
    AudioBufferList inAudioBufferList;
    CMBlockBufferRef blockBuffer;
    CMSampleBufferGetAudioBufferListWithRetainedBlockBuffer(sampleBuffer,
                                                            NULL,
                                                            &inAudioBufferList,
                                                            sizeof(inAudioBufferList),
                                                            NULL,
                                                            NULL,
                                                            0,
                                                            &blockBuffer);
    
    AudioBuffer buffer = inAudioBufferList.mBuffers[0];
    uint8_t* p = (uint8_t*)buffer.mData;
    
    void *data = [self sendAudioBuffer:p frameSize:buffer.mDataByteSize outSize:pOutSize frameType:3];
        
    if (blockBuffer) {
        CFRelease(blockBuffer);
    }
  
    return data;
    
}

- (void *)sendAudioBuffer:(unsigned char *)buffer frameSize:(int)size outSize:(size_t *)pOutSize frameType:(int8_t)frameType {
    
    TransferFrame frame = {0};
    frame.ver = 0x01;
    frame.frameType = frameType;
    frame.magic = MAGIC_NUMBER;
    frame.length = size;
    NSUInteger framesize = sizeof(TransferFrame);
    NSUInteger length = framesize + size;
    void * dst = malloc(length);
    memcpy(dst, &frame, framesize);
    memcpy((uint8_t *)dst + framesize, buffer, size);
    
    if(pOutSize){
        *pOutSize = length;
    }
    return dst;
    
}

- (void)pushAudioFrame:(unsigned char *)inAudioFrame
             frameSize:(int64_t)frameSize {
    if (self.audioSocket < 0) {
        return;
    }
                
    size_t length = 0;
    void *sendData = NULL;
    sendData = [self sendAudioBuffer:inAudioFrame frameSize:(int)frameSize outSize:&length frameType:2];
                
    dispatch_semaphore_wait(semaphoreLock, DISPATCH_TIME_FOREVER);
    
    if(mRecordingAudioBufferBytes + length >= kAudioBufferPoolSize) {
        mRecordingAudioBufferBytes = length;
        memcpy(mRecordingAudioAppPool, sendData, length);
    }else {
        memcpy(mRecordingAudioAppPool + mRecordingAudioBufferBytes, sendData, length);
        mRecordingAudioBufferBytes += length;
    }
    
    dispatch_semaphore_signal(semaphoreLock);
    free(sendData);
    
    if (self.isAudioSending) {
        return;
    }
    
    self.isAudioSending = YES;
    
    dispatch_async([[self class] SEND_AUDIO_SERIAL_QUEUE], ^{

        dispatch_semaphore_wait(semaphoreLock, DISPATCH_TIME_FOREVER);

        if (mRecordingAudioBufferBytes > 0) {

            size_t length = mRecordingAudioBufferBytes;
            void *sendData = malloc(length);
            memcpy(sendData, mRecordingAudioAppPool, length);
            mRecordingAudioBufferBytes = 0;

            dispatch_semaphore_signal(semaphoreLock);

            size_t sendLength = send(self.audioSocket, sendData, length, 0);
            if (sendLength == -1) {
                NSLog(@"audio send data err!! length:%@", @(length));
            } else if(sendLength != length) {
                NSLog(@"audio send data err!! length:%@(%@)", @(sendLength),@(length));
            }

            free(sendData);

        }else {
            dispatch_semaphore_signal(semaphoreLock);
        }

        self.isAudioSending = NO;
        
    });
}

-(void)processAudio:(CMSampleBufferRef)sampleBuffer isAppAudio:(BOOL)isAppAudio {
    if (self.audioSocket < 0) {
        return;
    }

    size_t length = 0;
    void *sendData = NULL;
    if (isAppAudio) {
        sendData = [self sendAudioAppBuffer:sampleBuffer outSize:&length];
    } else {
        sendData = [self sendAudioMicBuffer:sampleBuffer outSize:&length];
    }

    dispatch_semaphore_wait(semaphoreLock, DISPATCH_TIME_FOREVER);

    if(mRecordingAudioBufferBytes + length >= kAudioBufferPoolSize) {
        mRecordingAudioBufferBytes = length;
        memcpy(mRecordingAudioAppPool, sendData, length);
    }else {
        memcpy(mRecordingAudioAppPool + mRecordingAudioBufferBytes, sendData, length);
        mRecordingAudioBufferBytes += length;
    }

    dispatch_semaphore_signal(semaphoreLock);
    free(sendData);

    if (self.isAudioSending) {
        return;
    }

    self.isAudioSending = YES;

    dispatch_async([[self class] SEND_AUDIO_SERIAL_QUEUE], ^{

        dispatch_semaphore_wait(semaphoreLock, DISPATCH_TIME_FOREVER);

        if (mRecordingAudioBufferBytes > 0) {

            size_t length = mRecordingAudioBufferBytes;
            unsigned char *sendData = malloc(length);
            memcpy(sendData, mRecordingAudioAppPool, length);
            mRecordingAudioBufferBytes = 0;

            dispatch_semaphore_signal(semaphoreLock);

            size_t sendLength = send(self.audioSocket, sendData, length, 0);
            if (sendLength == -1) {
                NSLog(@"audio send data err!! length:%@", @(length));
            } else if(sendLength != length) {
                NSLog(@"audio send data err!! length:%@(%@)", @(sendLength),@(length));
            }

            free(sendData);

        }else {
            dispatch_semaphore_signal(semaphoreLock);
        }

        self.isAudioSending = NO;

    });
}

@end
