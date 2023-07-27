//
//  KFMP4Demuxer.m
//  APIExample
//
//  Created by zhaoyongqiang on 2023/6/25.
//  Copyright © 2023 Agora Corp. All rights reserved.
//

#import "KFMP4Demuxer.h"

@implementation KFDemuxerConfig

- (instancetype)init {
    self = [super init];
    if (self) {
        _demuxerType = KFMediaAV;
    }
    
    return self;
}
@end

@implementation KFVideoPacketExtraData

@end

#define KFMP4DemuxerBadFileError 2000
#define KFMP4DemuxerAddVideoOutputError 2001
#define KFMP4DemuxerAddAudioOutputError 2002
#define KFMP4DemuxerQueueMaxCount 3

@interface KFMP4Demuxer () {
    CMSimpleQueueRef _audioQueue;
    CMSimpleQueueRef _videoQueue;
}
@property (nonatomic, strong, readwrite) KFDemuxerConfig* config;
@property (nonatomic, strong) AVAssetReader *demuxReader; // 解封装器实例。
@property (nonatomic, strong) AVAssetReaderTrackOutput *readerAudioOutput; // Demuxer 的音频输出。
@property (nonatomic, strong) AVAssetReaderTrackOutput *readerVideoOutput; // Demuxer 的视频输出。
@property (nonatomic, strong) dispatch_queue_t demuxerQueue;
@property (nonatomic, strong) dispatch_semaphore_t demuxerSemaphore;
@property (nonatomic, strong) dispatch_semaphore_t audioQueueSemaphore;
@property (nonatomic, strong) dispatch_semaphore_t videoQueueSemaphore;
@property (nonatomic, assign) CMTime lastAudioCopyNextTime; // 上一次拷贝的音频采样的时间戳。
@property (nonatomic, assign) CMTime lastVideoCopyNextTime; // 上一次拷贝的视频采样的时间戳。
@property (nonatomic, assign, readwrite) BOOL hasAudioTrack; // 是否包含音频数据。
@property (nonatomic, assign, readwrite) BOOL hasVideoTrack; // 是否包含视频数据。
@property (nonatomic, assign, readwrite) CGSize videoSize; // 视频大小。
@property (nonatomic, assign, readwrite) CMTime duration; // 媒体时长。
@property (nonatomic, assign, readwrite) CMVideoCodecType codecType; // 编码类型。
@property (nonatomic, assign, readwrite) KFMP4DemuxerStatus demuxerStatus; // 解封装器状态。
@property (nonatomic, assign, readwrite) BOOL audioEOF; // 是否音频结束。
@property (nonatomic, assign, readwrite) BOOL videoEOF; // 是否视频结束。
@property (nonatomic, assign, readwrite) CGAffineTransform preferredTransform; // 图像的变换信息。比如：视频图像旋转。
@end

@implementation KFMP4Demuxer
#pragma mark - LifeCycle
- (instancetype)initWithConfig:(KFDemuxerConfig *)config {
    self = [super init];
    if (self) {
        _config = config;
        _demuxerSemaphore = dispatch_semaphore_create(1);
        _audioQueueSemaphore = dispatch_semaphore_create(1);
        _videoQueueSemaphore = dispatch_semaphore_create(1);
        _demuxerStatus = KFMP4DemuxerStatusUnknown;
        _demuxerQueue = dispatch_queue_create("com.KeyFrameKit.demuxerQueue", DISPATCH_QUEUE_SERIAL);
        CMSimpleQueueCreate(kCFAllocatorDefault, KFMP4DemuxerQueueMaxCount, &_audioQueue);
        CMSimpleQueueCreate(kCFAllocatorDefault, KFMP4DemuxerQueueMaxCount, &_videoQueue);
    }
    return self;
}

- (void)dealloc {
    // 清理状态机。
    if (self.demuxerStatus == KFMP4DemuxerStatusRunning) {
        self.demuxerStatus = KFMP4DemuxerStatusCancelled;
    }
    
    // 清理解封装器实例。
    dispatch_semaphore_wait(_demuxerSemaphore, DISPATCH_TIME_FOREVER);
    if (self.demuxReader && self.demuxReader.status == AVAssetReaderStatusReading) {
        [self.demuxReader cancelReading];
    }
    dispatch_semaphore_signal(_demuxerSemaphore);
    
    // 清理音频数据队列。
    dispatch_semaphore_wait(_audioQueueSemaphore, DISPATCH_TIME_FOREVER);
    while (CMSimpleQueueGetCount(_audioQueue) > 0) {
        CMSampleBufferRef sampleBuffer = (CMSampleBufferRef)CMSimpleQueueDequeue(_audioQueue);
        CFRelease(sampleBuffer);
    }
    dispatch_semaphore_signal(_audioQueueSemaphore);
    
    // 清理视频数据队列。
    dispatch_semaphore_wait(_videoQueueSemaphore, DISPATCH_TIME_FOREVER);
    while (CMSimpleQueueGetCount(_videoQueue) > 0) {
        CMSampleBufferRef sampleBuffer = (CMSampleBufferRef)CMSimpleQueueDequeue(_videoQueue);
        CFRelease(sampleBuffer);
    }
    dispatch_semaphore_signal(_videoQueueSemaphore);
}

#pragma mark - Public Method
- (void)startReading {
    __weak typeof(self) weakSelf = self;
    dispatch_async(_demuxerQueue, ^{
        dispatch_semaphore_wait(weakSelf.demuxerSemaphore, DISPATCH_TIME_FOREVER);
        
        // 在第一次开始读数据时，创建解封装器实例。
        if (!weakSelf.demuxReader) {
            NSError *error;
            [weakSelf _setupDemuxReader:&error];
            weakSelf.audioEOF = !weakSelf.hasAudioTrack;
            weakSelf.videoEOF = !weakSelf.hasVideoTrack;
            weakSelf.demuxerStatus = error ? KFMP4DemuxerStatusFailed : KFMP4DemuxerStatusRunning;
            dispatch_semaphore_signal(weakSelf.demuxerSemaphore);
            if (error == nil) {
                // Demuxer 启动成功后，就可以从它里面获取解封装后的数据了。
                [weakSelf fetchAndSaveDemuxedData];
            } else {
                NSLog(@"KFMP4Demuxer error: %zi %@", error.code, error.localizedDescription);
            }
            return;
        }

        dispatch_semaphore_signal(weakSelf.demuxerSemaphore);
    });
}

- (void)cancelReading {
    __weak typeof(self) weakSelf = self;
    if (self.demuxerQueue == nil) { return; }
    dispatch_async(_demuxerQueue, ^{
        if (weakSelf.demuxerSemaphore == nil) { return; }
        dispatch_semaphore_wait(weakSelf.demuxerSemaphore, DISPATCH_TIME_FOREVER);
        
        // 取消读数据。
        if (weakSelf.demuxReader && weakSelf.demuxReader.status == AVAssetReaderStatusReading) {
            [weakSelf.demuxReader cancelReading];
        }
        weakSelf.demuxerStatus = KFMP4DemuxerStatusCancelled;
        
        dispatch_semaphore_signal(weakSelf.demuxerSemaphore);
    });
}

- (BOOL)hasAudioSampleBuffer {
    // 是否还有音频数据。
    if (self.hasAudioTrack && self.demuxerStatus == KFMP4DemuxerStatusRunning && !self.audioEOF) {
        int32_t audioCount = 0;
        dispatch_semaphore_wait(_audioQueueSemaphore, DISPATCH_TIME_FOREVER);
        if (CMSimpleQueueGetCount(_audioQueue) > 0) {
            audioCount = CMSimpleQueueGetCount(_audioQueue);
        }
        dispatch_semaphore_signal(_audioQueueSemaphore);
        
        return (audioCount == 0 && self.audioEOF) ? NO : YES;
    }
    
    return NO;
}

- (CMSampleBufferRef)copyNextAudioSampleBuffer CF_RETURNS_RETAINED {
    // 拷贝下一份音频采样。
    CMSampleBufferRef sampleBuffer = NULL;
    while (!sampleBuffer && self.demuxerStatus == KFMP4DemuxerStatusRunning && !self.audioEOF) {
        // 先从缓冲队列取数据。
        dispatch_semaphore_wait(_audioQueueSemaphore, DISPATCH_TIME_FOREVER);
        if (CMSimpleQueueGetCount(_audioQueue) > 0) {
            sampleBuffer = (CMSampleBufferRef) CMSimpleQueueDequeue(_audioQueue);
        }
        dispatch_semaphore_signal(_audioQueueSemaphore);
        
        // 缓冲队列没有数据，就同步加载一下试试。
        if (!sampleBuffer && self.demuxerStatus == KFMP4DemuxerStatusRunning) {
            [self _syncLoadNextSampleBuffer];
        }
    }
    
    // 异步加载一下，先缓冲到数据队列中，等下次取。
    [self _asyncLoadNextSampleBuffer];
    
    return sampleBuffer;
}

- (BOOL)hasVideoSampleBuffer {
    // 是否还有视频数据。
    if (self.hasVideoTrack && self.demuxerStatus == KFMP4DemuxerStatusRunning && !self.videoEOF) {
        int32_t videoCount = 0;
        dispatch_semaphore_wait(_videoQueueSemaphore, DISPATCH_TIME_FOREVER);
        if (CMSimpleQueueGetCount(_videoQueue) > 0) {
            videoCount = CMSimpleQueueGetCount(_videoQueue);
        }
        dispatch_semaphore_signal(_videoQueueSemaphore);
        
        return (videoCount == 0 && self.videoEOF) ? NO : YES;
    }
    
    return NO;
}

- (CMSampleBufferRef)copyNextVideoSampleBuffer CF_RETURNS_RETAINED {
    // 拷贝下一份视频采样。
    CMSampleBufferRef sampleBuffer = NULL;
    while (!sampleBuffer && self.demuxerStatus == KFMP4DemuxerStatusRunning && !self.videoEOF) {
        // 先从缓冲队列取数据。
        dispatch_semaphore_wait(_videoQueueSemaphore, DISPATCH_TIME_FOREVER);
        if (CMSimpleQueueGetCount(_videoQueue) > 0) {
            sampleBuffer = (CMSampleBufferRef) CMSimpleQueueDequeue(_videoQueue);
        }
        dispatch_semaphore_signal(_videoQueueSemaphore);
        
        // 缓冲队列没有数据，就同步加载一下试试。
        if (!sampleBuffer && self.demuxerStatus == KFMP4DemuxerStatusRunning) {
            [self _syncLoadNextSampleBuffer];
        }
    }
    
    // 异步加载一下，先缓冲到数据队列中，等下次取。
    [self _asyncLoadNextSampleBuffer];
    
    return sampleBuffer;
}

#pragma mark - Utility
- (void)fetchAndSaveDemuxedData {
    // 异步地从 Demuxer 获取解封装后的 H.264/H.265 编码数据。
    dispatch_async(dispatch_get_global_queue(0, 0), ^{
        while (self.hasVideoSampleBuffer) {
            CMSampleBufferRef videoBuffer = [self copyNextVideoSampleBuffer];
            if (videoBuffer) {
                [self saveSampleBuffer:videoBuffer];
                CFRelease(videoBuffer);
            }
        }
        if (self.demuxerStatus == KFMP4DemuxerStatusCompleted) {
            NSLog(@"KFMP4Demuxer complete");
        }
    });
}

- (KFVideoPacketExtraData *)getPacketExtraData:(CMSampleBufferRef)sampleBuffer {
    // 从 CMSampleBuffer 中获取 extra data。
    if (!sampleBuffer) {
        return nil;
    }
    
    // 获取编码类型。
    CMVideoCodecType codecType = CMVideoFormatDescriptionGetCodecType(CMSampleBufferGetFormatDescription(sampleBuffer));
    
    KFVideoPacketExtraData *extraData = nil;
    if (codecType == kCMVideoCodecType_H264) {
        // 获取 H.264 的 extra data：sps、pps。
        CMFormatDescriptionRef format = CMSampleBufferGetFormatDescription(sampleBuffer);
        size_t sparameterSetSize, sparameterSetCount;
        const uint8_t *sparameterSet;
        OSStatus statusCode = CMVideoFormatDescriptionGetH264ParameterSetAtIndex(format, 0, &sparameterSet, &sparameterSetSize, &sparameterSetCount, 0);
        if (statusCode == noErr) {
            size_t pparameterSetSize, pparameterSetCount;
            const uint8_t *pparameterSet;
            OSStatus statusCode = CMVideoFormatDescriptionGetH264ParameterSetAtIndex(format, 1, &pparameterSet, &pparameterSetSize, &pparameterSetCount, 0);
            if (statusCode == noErr) {
                extraData = [[KFVideoPacketExtraData alloc] init];
                extraData.sps = [NSData dataWithBytes:sparameterSet length:sparameterSetSize];
                extraData.pps = [NSData dataWithBytes:pparameterSet length:pparameterSetSize];
            }
        }
    } else if (codecType == kCMVideoCodecType_HEVC) {
        // 获取 H.265 的 extra data：vps、sps、pps。
        CMFormatDescriptionRef format = CMSampleBufferGetFormatDescription(sampleBuffer);
        size_t vparameterSetSize, vparameterSetCount;
        const uint8_t *vparameterSet;
        if (@available(iOS 11.0, *)) {
            OSStatus statusCode = CMVideoFormatDescriptionGetHEVCParameterSetAtIndex(format, 0, &vparameterSet, &vparameterSetSize, &vparameterSetCount, 0);
            if (statusCode == noErr) {
                size_t sparameterSetSize, sparameterSetCount;
                const uint8_t *sparameterSet;
                OSStatus statusCode = CMVideoFormatDescriptionGetHEVCParameterSetAtIndex(format, 1, &sparameterSet, &sparameterSetSize, &sparameterSetCount, 0);
                if (statusCode == noErr) {
                    size_t pparameterSetSize, pparameterSetCount;
                    const uint8_t *pparameterSet;
                    OSStatus statusCode = CMVideoFormatDescriptionGetHEVCParameterSetAtIndex(format, 2, &pparameterSet, &pparameterSetSize, &pparameterSetCount, 0);
                    if (statusCode == noErr) {
                        extraData = [[KFVideoPacketExtraData alloc] init];
                        extraData.vps = [NSData dataWithBytes:vparameterSet length:vparameterSetSize];
                        extraData.sps = [NSData dataWithBytes:sparameterSet length:sparameterSetSize];
                        extraData.pps = [NSData dataWithBytes:pparameterSet length:pparameterSetSize];
                    }
                }
            }
        } else {
            // 其他编码格式。
        }
    }
    
    return extraData;
}

- (BOOL)isKeyFrame:(CMSampleBufferRef)sampleBuffer {
    CFArrayRef array = CMSampleBufferGetSampleAttachmentsArray(sampleBuffer, true);
    if (!array) {
        return NO;
    }
    
    CFDictionaryRef dic = (CFDictionaryRef)CFArrayGetValueAtIndex(array, 0);
    if (!dic) {
        return NO;
    }
    
    // 检测 sampleBuffer 是否是关键帧。
    BOOL keyframe = !CFDictionaryContainsKey(dic, kCMSampleAttachmentKey_NotSync);
    
    return keyframe;
}

- (void)saveSampleBuffer:(CMSampleBufferRef)sampleBuffer {
    // 将编码数据存储为文件。
    // iOS 的 VideoToolbox 编码和解码只支持 AVCC/HVCC 的码流格式。但是 Android 的 MediaCodec 只支持 AnnexB 的码流格式。这里我们做一下两种格式的转换示范，将 AVCC/HVCC 格式的码流转换为 AnnexB 再存储。
    // 1、AVCC/HVCC 码流格式：[extradata]|[length][NALU]|[length][NALU]|...
    // VPS、SPS、PPS 不用 NALU 来存储，而是存储在 extradata 中；每个 NALU 前有个 length 字段表示这个 NALU 的长度（不包含 length 字段），length 字段通常是 4 字节。
    // 2、AnnexB 码流格式：[startcode][NALU]|[startcode][NALU]|...
    // 每个 NAL 前要添加起始码：0x00000001；VPS、SPS、PPS 也都用这样的 NALU 来存储，一般在码流最前面。
    if (sampleBuffer) {
        NSMutableData *resultData = [NSMutableData new];
        uint8_t nalPartition[] = {0x00, 0x00, 0x00, 0x01};
        
        // 关键帧前添加 vps（H.265)、sps、pps。这里要注意顺序别乱了。
        if ([self isKeyFrame:sampleBuffer]) {
            KFVideoPacketExtraData *extraData = [self getPacketExtraData:sampleBuffer];
            if (extraData.vps) {
                [resultData appendBytes:nalPartition length:4];
                [resultData appendData:extraData.vps];
            }
            [resultData appendBytes:nalPartition length:4];
            [resultData appendData:extraData.sps];
            [resultData appendBytes:nalPartition length:4];
            [resultData appendData:extraData.pps];
        }
        
        // 获取编码数据。这里的数据是 AVCC/HVCC 格式的。
        CMBlockBufferRef dataBuffer = CMSampleBufferGetDataBuffer(sampleBuffer);
        size_t length, totalLength;
        char *dataPointer;
        OSStatus statusCodeRet = CMBlockBufferGetDataPointer(dataBuffer, 0, &length, &totalLength, &dataPointer);
        if (statusCodeRet == noErr) {
            size_t bufferOffset = 0;
            static const int NALULengthHeaderLength = 4;
            // 拷贝编码数据。
            while (bufferOffset < totalLength - NALULengthHeaderLength) {
                // 通过 length 字段获取当前这个 NALU 的长度。
                uint32_t NALUnitLength = 0;
                memcpy(&NALUnitLength, dataPointer + bufferOffset, NALULengthHeaderLength);
                NALUnitLength = CFSwapInt32BigToHost(NALUnitLength);
                
                // 拷贝 AnnexB 起始码字节。
                [resultData appendData:[NSData dataWithBytes:nalPartition length:4]];
                // 拷贝这个 NALU 的字节。
                [resultData appendData:[NSData dataWithBytes:(dataPointer + bufferOffset + NALULengthHeaderLength) length:NALUnitLength]];
                
                // 步进。
                bufferOffset += NALULengthHeaderLength + NALUnitLength;
            }
        }
        if (self.dataCallBack) {
            self.dataCallBack(resultData.copy, sampleBuffer);
        }
    }
}

#pragma mark - Private Method
- (void)_setupDemuxReader:(NSError**)error {
    if (!self.config.asset) {
        *error = [NSError errorWithDomain:NSStringFromClass([self class]) code:40003 userInfo:nil];
        return;
    }
    
    // 1、创建解封装器实例。
    // 使用 AVAssetReader 作为解封装器。解封装的目标是 config 中的 AVAsset 资源。
    _demuxReader = [[AVAssetReader alloc] initWithAsset:self.config.asset error:error];
    if (!_demuxReader) {
        return;
    }
    
    // 2、获取时间信息。
    _duration = [self.config.asset duration];
    
    // 3、处理待解封装的资源中的视频。
    if (self.config.demuxerType & KFMediaVideo) {
        // 取出视频轨道。
        AVAssetTrack *videoTrack = [[self.config.asset tracksWithMediaType:AVMediaTypeVideo] firstObject];
        _hasVideoTrack = videoTrack ? YES : NO;
        if (_hasVideoTrack) {
            // 获取图像变换信息。
            _preferredTransform = videoTrack.preferredTransform;
            
            // 获取图像大小。要应用上图像变换信息。
            _videoSize = CGSizeApplyAffineTransform(videoTrack.naturalSize, videoTrack.preferredTransform);
            _videoSize = CGSizeMake(fabs(_videoSize.width), fabs(_videoSize.height));
            
            // 获取编码格式。
            CMVideoFormatDescriptionRef formatDescription = (__bridge CMVideoFormatDescriptionRef)[[videoTrack formatDescriptions] firstObject];
            if (formatDescription) {
                _codecType = CMVideoFormatDescriptionGetCodecType(formatDescription);
            }
            
            // 基于轨道创建视频输出。
            _readerVideoOutput = [[AVAssetReaderTrackOutput alloc] initWithTrack:videoTrack outputSettings:nil];
            _readerVideoOutput.alwaysCopiesSampleData = NO; // 避免总是做数据拷贝，影响性能。
            
            // 给解封装器绑定视频输出。
            if ([_demuxReader canAddOutput:_readerVideoOutput]) {
                [_demuxReader addOutput:_readerVideoOutput];
            } else {
                *error = _demuxReader.error ? _demuxReader.error : [NSError errorWithDomain:NSStringFromClass([self class]) code:KFMP4DemuxerAddVideoOutputError userInfo:nil];
                return;
            }
        }
    }
    
    // 4、处理待解封装的资源中的音频。
    if (self.config.demuxerType & KFMediaAudio) {
        // 取出音频轨道。
        AVAssetTrack *audioTrack = [[self.config.asset tracksWithMediaType:AVMediaTypeAudio] firstObject];
        _hasAudioTrack = audioTrack ? YES : NO;
        if (_hasAudioTrack) {
            // 基于轨道创建音频输出。
            _readerAudioOutput = [[AVAssetReaderTrackOutput alloc] initWithTrack:audioTrack outputSettings:nil];
            _readerAudioOutput.alwaysCopiesSampleData = NO; // 避免总是做数据拷贝，影响性能。
            
            // 给解封装器绑定音频输出。
            if ([_demuxReader canAddOutput:_readerAudioOutput]) {
                [_demuxReader addOutput:_readerAudioOutput];
            } else {
                *error = _demuxReader.error ? _demuxReader.error : [NSError errorWithDomain:NSStringFromClass([self class]) code:KFMP4DemuxerAddAudioOutputError userInfo:nil];
                return;
            }
        }
    }
    
    // 5、音频和视频数据都没有，就报错。
    if (!_hasVideoTrack && !_hasAudioTrack) {
        *error = [NSError errorWithDomain:NSStringFromClass([self class]) code:KFMP4DemuxerBadFileError userInfo:nil];
        return;
    }
    
    // 6、启动解封装。
    BOOL startSuccess = [self.demuxReader startReading];
    if (!startSuccess) {
        *error = self.demuxReader.error;
    }
}

- (void)_asyncLoadNextSampleBuffer {
    // 异步加载下一份采样数据。
    __weak typeof(self) weakSelf = self;
    dispatch_async(_demuxerQueue, ^{
        dispatch_semaphore_wait(weakSelf.demuxerSemaphore, DISPATCH_TIME_FOREVER);
        [weakSelf _loadNextSampleBuffer];
        dispatch_semaphore_signal(weakSelf.demuxerSemaphore);
    });
}

- (void)_syncLoadNextSampleBuffer {
    // 同步加载下一份采样数据。
    dispatch_semaphore_wait(self.demuxerSemaphore, DISPATCH_TIME_FOREVER);
    [self _loadNextSampleBuffer];
    dispatch_semaphore_signal(self.demuxerSemaphore);
}

- (void)_loadNextSampleBuffer {
    if (self.demuxerStatus != KFMP4DemuxerStatusRunning) {
        return;
    }
    
    // 1、根据解封装器的状态，处理异常情况。
    if (self.demuxReader.status == AVAssetWriterStatusCompleted) {
        self.demuxerStatus = KFMP4DemuxerStatusCompleted;
        return;
    } else if (self.demuxReader.status == AVAssetWriterStatusFailed) {
#if TARGET_OS_IPHONE
        if (self.demuxReader.error.code == AVErrorOperationInterrupted) {
            // 如果当前解封装器的状态是被打断而失败，就尝试重新创建一下。
            NSError *error;
            [self _setupDemuxReader:&error];
            if (!error) {
                // 同时做一下恢复处理。
                [self _resumeLastTime];
            }
        }
#else
        if (self.demuxReader.error.code == AVErrorUnknown) {
            // 如果当前解封装器的状态是被打断而失败，就尝试重新创建一下。
            NSError *error;
            [self _setupDemuxReader:&error];
            if (!error) {
                // 同时做一下恢复处理。
                [self _resumeLastTime];
            }
        }
#endif
        if (self.demuxReader.status == AVAssetWriterStatusFailed) {
            // 如果状态依然是失败，就上报错误。
            self.demuxerStatus = KFMP4DemuxerStatusFailed;
            if (self.errorCallBack) {
                NSError *error = self.demuxReader.error;
                dispatch_async(dispatch_get_main_queue(), ^{
                    self.errorCallBack(error);
                });
            }
            return;
        }
    } else if (self.demuxReader.status == AVAssetWriterStatusCancelled) {
        // 如果状态是取消，就直接 return。
        self.demuxerStatus = KFMP4DemuxerStatusCancelled;
        return;
    }
    
    // 2、解封装器状态正常，加载下一份采样数据。
    BOOL audioNeedLoad = (self.config.demuxerType & KFMediaAudio) && !self.audioEOF;
    BOOL videoNeedLoad = (self.config.demuxerType & KFMediaVideo) && !self.videoEOF;
    while (self.demuxReader && self.demuxReader.status == AVAssetReaderStatusReading && (audioNeedLoad || videoNeedLoad)) {
        // 加载音频数据。
        if (audioNeedLoad) {
            dispatch_semaphore_wait(_audioQueueSemaphore, DISPATCH_TIME_FOREVER);
            int32_t audioCount = CMSimpleQueueGetCount(_audioQueue);
            dispatch_semaphore_signal(_audioQueueSemaphore);
            if (audioCount < KFMP4DemuxerQueueMaxCount) {
                // 从音频输出源读取音频数据。
                CMSampleBufferRef next = [self.readerAudioOutput copyNextSampleBuffer];
                if (next) {
                    if (!CMSampleBufferGetDataBuffer(next)) {
                        CFRelease(next);
                    } else {
                        // 将数据从音频输出源 readerAudioOutput 拷贝到缓冲队列 _audioQueue 中。
                        self.lastAudioCopyNextTime = CMSampleBufferGetPresentationTimeStamp(next);
                        dispatch_semaphore_wait(_audioQueueSemaphore, DISPATCH_TIME_FOREVER);
                        CMSimpleQueueEnqueue(_audioQueue, next);
                        audioCount = CMSimpleQueueGetCount(_audioQueue);
                        dispatch_semaphore_signal(_audioQueueSemaphore);
                    }
                } else {
                    self.audioEOF = self.demuxReader.status == AVAssetReaderStatusReading || self.demuxReader.status == AVAssetWriterStatusCompleted;
                    audioNeedLoad = NO;
                }
            } else {
                audioNeedLoad = NO;
            }
        }
        
        // 加载视频数据。
        if (videoNeedLoad) {
            dispatch_semaphore_wait(_videoQueueSemaphore, DISPATCH_TIME_FOREVER);
            int32_t videoCount = CMSimpleQueueGetCount(_videoQueue);
            dispatch_semaphore_signal(_videoQueueSemaphore);
            if (videoCount < KFMP4DemuxerQueueMaxCount) {
                // 从视频输出源读取视频数据。
                CMSampleBufferRef next = [self.readerVideoOutput copyNextSampleBuffer];
                if (next) {
                    if (!CMSampleBufferGetDataBuffer(next)) {
                        CFRelease(next);
                    } else {
                        // 将数据从视频输出源 readerVideoOutput 拷贝到缓冲队列 _videoQueue 中。
                        self.lastVideoCopyNextTime = CMSampleBufferGetDecodeTimeStamp(next);
                        dispatch_semaphore_wait(_videoQueueSemaphore, DISPATCH_TIME_FOREVER);
                        CMSimpleQueueEnqueue(_videoQueue, next);
                        videoCount = CMSimpleQueueGetCount(_videoQueue);
                        dispatch_semaphore_signal(_videoQueueSemaphore);
                    }
                } else {
                    self.videoEOF = self.demuxReader.status == AVAssetReaderStatusReading || self.demuxReader.status == AVAssetWriterStatusCompleted;
                    videoNeedLoad = NO;
                }
            } else {
                videoNeedLoad = NO;
            }
        }
    }
}

- (void)_resumeLastTime {
    // 对于异常中断后的处理，需要根据记录的时间戳 _lastAudioCopyNextTime/_lastVideoCopyNextTime 做恢复操作。
    BOOL audioNeedLoad = (_lastAudioCopyNextTime.value > 0) && !self.audioEOF;
    BOOL videoNeedLoad = (_lastVideoCopyNextTime.value > 0) && !self.videoEOF;
    while (self.demuxReader && self.demuxReader.status == AVAssetReaderStatusReading && (audioNeedLoad || videoNeedLoad)) {
        if (audioNeedLoad) {
            // 从音频输出源读取音频数据。
            CMSampleBufferRef next = [self.readerAudioOutput copyNextSampleBuffer];
            if (next) {
                if (CMTimeGetSeconds(CMSampleBufferGetPresentationTimeStamp(next)) <= CMTimeGetSeconds(_lastAudioCopyNextTime) || !CMSampleBufferGetDataBuffer(next)) {
                    // 从输出源取出的数据时间戳小于上次标记的时间，则表示这份采样数据已经处理过了。
                    CFRelease(next);
                } else {
                    dispatch_semaphore_wait(_audioQueueSemaphore, DISPATCH_TIME_FOREVER);
                    CMSimpleQueueEnqueue(_audioQueue, next);
                    dispatch_semaphore_signal(_audioQueueSemaphore);
                    audioNeedLoad = NO;
                }
            } else {
                self.audioEOF = self.demuxReader.status == AVAssetReaderStatusReading || self.demuxReader.status == AVAssetWriterStatusCompleted;
                audioNeedLoad = NO;
            }
        }
        
        if (videoNeedLoad) {
            // 从视频输出源读取视频数据。
            CMSampleBufferRef next = [self.readerVideoOutput copyNextSampleBuffer];
            if (next) {
                if (CMTimeGetSeconds(CMSampleBufferGetDecodeTimeStamp(next)) <= CMTimeGetSeconds(_lastVideoCopyNextTime) || !CMSampleBufferGetDataBuffer(next)) {
                    // 从输出源取出的数据时间戳小于上次标记的时间，则表示这份采样数据已经处理过了。
                    CFRelease(next);
                } else {
                    dispatch_semaphore_wait(_videoQueueSemaphore, DISPATCH_TIME_FOREVER);
                    CMSimpleQueueEnqueue(_videoQueue, next);
                    dispatch_semaphore_signal(_videoQueueSemaphore);
                    videoNeedLoad = NO;
                }
            } else {
                self.videoEOF = self.demuxReader.status == AVAssetReaderStatusReading || self.demuxReader.status == AVAssetWriterStatusCompleted;
                videoNeedLoad = NO;
            }
        }
    }
}

@end
