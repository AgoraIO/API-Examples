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
@property (nonatomic, strong) AVAssetReader *demuxReader; // Instance of the demuxer.
@property (nonatomic, strong) AVAssetReaderTrackOutput *readerAudioOutput; // Audio output of the demuxer.
@property (nonatomic, strong) AVAssetReaderTrackOutput *readerVideoOutput; // Video output of the demuxer.
@property (nonatomic, strong) dispatch_queue_t demuxerQueue; // Queue for demuxing operations.
@property (nonatomic, strong) dispatch_semaphore_t demuxerSemaphore; // Semaphore for managing demuxing.
@property (nonatomic, strong) dispatch_semaphore_t audioQueueSemaphore; // Semaphore for audio queue management.
@property (nonatomic, strong) dispatch_semaphore_t videoQueueSemaphore; // Semaphore for video queue management.
@property (nonatomic, assign) CMTime lastAudioCopyNextTime; // Timestamp of the last copied audio sample.
@property (nonatomic, assign) CMTime lastVideoCopyNextTime; // Timestamp of the last copied video sample.
@property (nonatomic, assign, readwrite) BOOL hasAudioTrack; // Indicates whether audio data is included.
@property (nonatomic, assign, readwrite) BOOL hasVideoTrack; // Indicates whether video data is included.
@property (nonatomic, assign, readwrite) CGSize videoSize; // Size of the video.
@property (nonatomic, assign, readwrite) CMTime duration; // Duration of the media.
@property (nonatomic, assign, readwrite) CMVideoCodecType codecType; // Encoding type.
@property (nonatomic, assign, readwrite) KFMP4DemuxerStatus demuxerStatus; // Status of the demuxer.
@property (nonatomic, assign, readwrite) BOOL audioEOF; // Indicates whether audio has ended.
@property (nonatomic, assign, readwrite) BOOL videoEOF; // Indicates whether video has ended.
@property (nonatomic, assign, readwrite) CGAffineTransform preferredTransform; // Transformation information for the image, e.g., video image rotation.
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
    // Clean up the state machine.
    if (self.demuxerStatus == KFMP4DemuxerStatusRunning) {
        self.demuxerStatus = KFMP4DemuxerStatusCancelled;
    }
    
    // Clean up the demuxer instance.
    dispatch_semaphore_wait(_demuxerSemaphore, DISPATCH_TIME_FOREVER);
    if (self.demuxReader && self.demuxReader.status == AVAssetReaderStatusReading) {
        [self.demuxReader cancelReading];
    }
    dispatch_semaphore_signal(_demuxerSemaphore);
    
    // Clean up the audio data queue.
    dispatch_semaphore_wait(_audioQueueSemaphore, DISPATCH_TIME_FOREVER);
    while (CMSimpleQueueGetCount(_audioQueue) > 0) {
        CMSampleBufferRef sampleBuffer = (CMSampleBufferRef)CMSimpleQueueDequeue(_audioQueue);
        CFRelease(sampleBuffer);
    }
    dispatch_semaphore_signal(_audioQueueSemaphore);
    
    // Clean up the video data queue.
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
        
        // Create the demuxer instance when starting to read data for the first time.
        if (!weakSelf.demuxReader) {
            NSError *error;
            [weakSelf _setupDemuxReader:&error];
            weakSelf.audioEOF = !weakSelf.hasAudioTrack;
            weakSelf.videoEOF = !weakSelf.hasVideoTrack;
            weakSelf.demuxerStatus = error ? KFMP4DemuxerStatusFailed : KFMP4DemuxerStatusRunning;
            dispatch_semaphore_signal(weakSelf.demuxerSemaphore);
            if (error == nil) {
                // Once the demuxer is successfully started, we can fetch the demuxed data from it.
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
        
        // Cancel reading data.
        if (weakSelf.demuxReader && weakSelf.demuxReader.status == AVAssetReaderStatusReading) {
            [weakSelf.demuxReader cancelReading];
        }
        weakSelf.demuxerStatus = KFMP4DemuxerStatusCancelled;
        
        dispatch_semaphore_signal(weakSelf.demuxerSemaphore);
    });
}

- (BOOL)hasAudioSampleBuffer {
    // Check if there is still audio data available.
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
    // Copy the next audio sample.
    CMSampleBufferRef sampleBuffer = NULL;
    while (!sampleBuffer && self.demuxerStatus == KFMP4DemuxerStatusRunning && !self.audioEOF) {
        // First, try to get data from the buffer queue.
        dispatch_semaphore_wait(_audioQueueSemaphore, DISPATCH_TIME_FOREVER);
        if (CMSimpleQueueGetCount(_audioQueue) > 0) {
            sampleBuffer = (CMSampleBufferRef) CMSimpleQueueDequeue(_audioQueue);
        }
        dispatch_semaphore_signal(_audioQueueSemaphore);
        
        // If the buffer queue has no data, try to load it synchronously.
        if (!sampleBuffer && self.demuxerStatus == KFMP4DemuxerStatusRunning) {
            [self _syncLoadNextSampleBuffer];
        }
    }
    
    // Asynchronously load the next sample buffer into the data queue for later retrieval.
    [self _asyncLoadNextSampleBuffer];
    
    return sampleBuffer;
}

- (BOOL)hasVideoSampleBuffer {
    // Check if there is still video data available.
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
    // Copy the next video sample.
    CMSampleBufferRef sampleBuffer = NULL;
    while (!sampleBuffer && self.demuxerStatus == KFMP4DemuxerStatusRunning && !self.videoEOF) {
        // First, try to get data from the buffer queue.
        dispatch_semaphore_wait(_videoQueueSemaphore, DISPATCH_TIME_FOREVER);
        if (CMSimpleQueueGetCount(_videoQueue) > 0) {
            sampleBuffer = (CMSampleBufferRef) CMSimpleQueueDequeue(_videoQueue);
        }
        dispatch_semaphore_signal(_videoQueueSemaphore);
        
        // If the buffer queue has no data, try to load it synchronously.
        if (!sampleBuffer && self.demuxerStatus == KFMP4DemuxerStatusRunning) {
            [self _syncLoadNextSampleBuffer];
        }
    }
    
    // Asynchronously load the next sample buffer into the data queue for later retrieval.
    [self _asyncLoadNextSampleBuffer];
    
    return sampleBuffer;
}

#pragma mark - Utility
- (void)fetchAndSaveDemuxedData {
    // Asynchronously fetch demuxed H.264/H.265 encoded data from the Demuxer.
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
    // Get extra data from the CMSampleBuffer.
    if (!sampleBuffer) {
        return nil;
    }
    
    // Get the codec type.
    CMVideoCodecType codecType = CMVideoFormatDescriptionGetCodecType(CMSampleBufferGetFormatDescription(sampleBuffer));
    
    KFVideoPacketExtraData *extraData = nil;
    if (codecType == kCMVideoCodecType_H264) {
        // Get H.264 extra data: sps, pps.
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
        // Get H.265 extra data: vps, sps, pps.
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
            // Other encoding formats.
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
    
    // Check if the sampleBuffer is a keyframe.
    BOOL keyframe = !CFDictionaryContainsKey(dic, kCMSampleAttachmentKey_NotSync);
    
    return keyframe;
}

- (void)saveSampleBuffer:(CMSampleBufferRef)sampleBuffer {
    // Store the encoded data as a file.
    // iOS's VideoToolbox encoding and decoding only support AVCC/HVCC stream formats. However, Android's MediaCodec only supports AnnexB stream formats. Here we demonstrate how to convert between the two formats, converting AVCC/HVCC format to AnnexB before storing.
    // 1. AVCC/HVCC stream format: [extradata]|[length][NALU]|[length][NALU]|...
    // VPS, SPS, PPS are stored in extradata, and each NALU is preceded by a length field indicating the NALU's length (excluding the length field), which is typically 4 bytes.
    // 2. AnnexB stream format: [startcode][NALU]|[startcode][NALU]|...
    // Each NAL is prefixed with a start code: 0x00000001; VPS, SPS, PPS are also stored using this NALU format, usually at the beginning of the stream.
    if (sampleBuffer) {
        NSMutableData *resultData = [NSMutableData new];
        uint8_t nalPartition[] = {0x00, 0x00, 0x00, 0x01};
        
        // Prepend vps (H.265), sps, pps before the keyframe. Be careful with the order.
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
        
        // Get the encoded data. This data is in AVCC/HVCC format.
        CMBlockBufferRef dataBuffer = CMSampleBufferGetDataBuffer(sampleBuffer);
        size_t length, totalLength;
        char *dataPointer;
        OSStatus statusCodeRet = CMBlockBufferGetDataPointer(dataBuffer, 0, &length, &totalLength, &dataPointer);
        if (statusCodeRet == noErr) {
            size_t bufferOffset = 0;
            static const int NALULengthHeaderLength = 4;
            // Copy the encoded data.
            while (bufferOffset < totalLength - NALULengthHeaderLength) {
                // Get the length of the current NALU from the length field.
                uint32_t NALUnitLength = 0;
                memcpy(&NALUnitLength, dataPointer + bufferOffset, NALULengthHeaderLength);
                NALUnitLength = CFSwapInt32BigToHost(NALUnitLength);
                
                // Append AnnexB start code bytes.
                [resultData appendData:[NSData dataWithBytes:nalPartition length:4]];
                // Append the bytes of this NALU.
                [resultData appendData:[NSData dataWithBytes:(dataPointer + bufferOffset + NALULengthHeaderLength) length:NALUnitLength]];
                
                // Move forward.
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
    
    // 1. Create a demuxer instance.
    // Use AVAssetReader as the demuxer. The target for demuxing is the AVAsset resource in config.
    _demuxReader = [[AVAssetReader alloc] initWithAsset:self.config.asset error:error];
    if (!_demuxReader) {
        return;
    }
    
    // 2. Get time information.
    _duration = [self.config.asset duration];
    
    // 3. Process the video in the resource to be demuxed.
    if (self.config.demuxerType & KFMediaVideo) {
        // Retrieve the video track.
        AVAssetTrack *videoTrack = [[self.config.asset tracksWithMediaType:AVMediaTypeVideo] firstObject];
        _hasVideoTrack = videoTrack ? YES : NO;
        if (_hasVideoTrack) {
            // Get the image transformation information.
            _preferredTransform = videoTrack.preferredTransform;
            
            // Get the image size. Apply the image transformation information.
            _videoSize = CGSizeApplyAffineTransform(videoTrack.naturalSize, videoTrack.preferredTransform);
            _videoSize = CGSizeMake(fabs(_videoSize.width), fabs(_videoSize.height));
            
            // Get the encoding format.
            CMVideoFormatDescriptionRef formatDescription = (__bridge CMVideoFormatDescriptionRef)[[videoTrack formatDescriptions] firstObject];
            if (formatDescription) {
                _codecType = CMVideoFormatDescriptionGetCodecType(formatDescription);
            }
            
            // Create video output based on the track.
            _readerVideoOutput = [[AVAssetReaderTrackOutput alloc] initWithTrack:videoTrack outputSettings:nil];
            _readerVideoOutput.alwaysCopiesSampleData = NO; // Avoid always copying data to improve performance.
            
            // Bind the video output to the demuxer.
            if ([_demuxReader canAddOutput:_readerVideoOutput]) {
                [_demuxReader addOutput:_readerVideoOutput];
            } else {
                *error = _demuxReader.error ? _demuxReader.error : [NSError errorWithDomain:NSStringFromClass([self class]) code:KFMP4DemuxerAddVideoOutputError userInfo:nil];
                return;
            }
        }
    }
    
    // 4. Process the audio in the resource to be demuxed.
    if (self.config.demuxerType & KFMediaAudio) {
        // Retrieve the audio track.
        AVAssetTrack *audioTrack = [[self.config.asset tracksWithMediaType:AVMediaTypeAudio] firstObject];
        _hasAudioTrack = audioTrack ? YES : NO;
        if (_hasAudioTrack) {
            // Create audio output based on the track.
            _readerAudioOutput = [[AVAssetReaderTrackOutput alloc] initWithTrack:audioTrack outputSettings:nil];
            _readerAudioOutput.alwaysCopiesSampleData = NO; // Avoid always copying data to improve performance.
            
            // Bind the audio output to the demuxer.
            if ([_demuxReader canAddOutput:_readerAudioOutput]) {
                [_demuxReader addOutput:_readerAudioOutput];
            } else {
                *error = _demuxReader.error ? _demuxReader.error : [NSError errorWithDomain:NSStringFromClass([self class]) code:KFMP4DemuxerAddAudioOutputError userInfo:nil];
                return;
            }
        }
    }
    
    // 5. If there is neither audio nor video data, report an error.
    if (!_hasVideoTrack && !_hasAudioTrack) {
        *error = [NSError errorWithDomain:NSStringFromClass([self class]) code:KFMP4DemuxerBadFileError userInfo:nil];
        return;
    }
    
    // 6. Start demuxing.
    BOOL startSuccess = [self.demuxReader startReading];
    if (!startSuccess) {
        *error = self.demuxReader.error;
    }
}

- (void)_asyncLoadNextSampleBuffer {
    // Asynchronously load the next sample data.
    __weak typeof(self) weakSelf = self;
    dispatch_async(_demuxerQueue, ^{
        dispatch_semaphore_wait(weakSelf.demuxerSemaphore, DISPATCH_TIME_FOREVER);
        [weakSelf _loadNextSampleBuffer];
        dispatch_semaphore_signal(weakSelf.demuxerSemaphore);
    });
}

- (void)_syncLoadNextSampleBuffer {
    // Synchronously load the next sample data.
    dispatch_semaphore_wait(self.demuxerSemaphore, DISPATCH_TIME_FOREVER);
    [self _loadNextSampleBuffer];
    dispatch_semaphore_signal(self.demuxerSemaphore);
}

- (void)_loadNextSampleBuffer {
    if (self.demuxerStatus != KFMP4DemuxerStatusRunning) {
        return;
    }
    
    // 1. Handle exceptions based on the state of the demuxer.
    if (self.demuxReader.status == AVAssetWriterStatusCompleted) {
        self.demuxerStatus = KFMP4DemuxerStatusCompleted;
        return;
    } else if (self.demuxReader.status == AVAssetWriterStatusFailed) {
#if TARGET_OS_IPHONE
        if (self.demuxReader.error.code == AVErrorOperationInterrupted) {
            // If the current demuxer state is interrupted and failed, try to recreate it.
            NSError *error;
            [self _setupDemuxReader:&error];
            if (!error) {
                // Also perform recovery processing.
                [self _resumeLastTime];
            }
        }
#else
        if (self.demuxReader.error.code == AVErrorUnknown) {
            // If the current demuxer state is interrupted and failed, try to recreate it.
            NSError *error;
            [self _setupDemuxReader:&error];
            if (!error) {
                // Also perform recovery processing.
                [self _resumeLastTime];
            }
        }
#endif
        if (self.demuxReader.status == AVAssetWriterStatusFailed) {
            // If the status is still failed, report the error.
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
        // If the status is cancelled, just return.
        self.demuxerStatus = KFMP4DemuxerStatusCancelled;
        return;
    }
    
    // 2. If the demuxer status is normal, load the next sample data.
    BOOL audioNeedLoad = (self.config.demuxerType & KFMediaAudio) && !self.audioEOF;
    BOOL videoNeedLoad = (self.config.demuxerType & KFMediaVideo) && !self.videoEOF;
    while (self.demuxReader && self.demuxReader.status == AVAssetReaderStatusReading && (audioNeedLoad || videoNeedLoad)) {
        // Load audio data.
        if (audioNeedLoad) {
            dispatch_semaphore_wait(_audioQueueSemaphore, DISPATCH_TIME_FOREVER);
            int32_t audioCount = CMSimpleQueueGetCount(_audioQueue);
            dispatch_semaphore_signal(_audioQueueSemaphore);
            if (audioCount < KFMP4DemuxerQueueMaxCount) {
                // Read audio data from the audio output source.
                CMSampleBufferRef next = [self.readerAudioOutput copyNextSampleBuffer];
                if (next) {
                    if (!CMSampleBufferGetDataBuffer(next)) {
                        CFRelease(next);
                    } else {
                        // Copy data from the audio output source readerAudioOutput to the buffer queue _audioQueue.
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
        
        // Load video data.
        if (videoNeedLoad) {
            dispatch_semaphore_wait(_videoQueueSemaphore, DISPATCH_TIME_FOREVER);
            int32_t videoCount = CMSimpleQueueGetCount(_videoQueue);
            dispatch_semaphore_signal(_videoQueueSemaphore);
            if (videoCount < KFMP4DemuxerQueueMaxCount) {
                // Read video data from the video output source.
                CMSampleBufferRef next = [self.readerVideoOutput copyNextSampleBuffer];
                if (next) {
                    if (!CMSampleBufferGetDataBuffer(next)) {
                        CFRelease(next);
                    } else {
                        // Copy data from the video output source readerVideoOutput to the buffer queue _videoQueue.
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
    // For handling after an abnormal interruption, recovery operations need to be performed based on the recorded timestamps _lastAudioCopyNextTime/_lastVideoCopyNextTime.
    BOOL audioNeedLoad = (_lastAudioCopyNextTime.value > 0) && !self.audioEOF;
    BOOL videoNeedLoad = (_lastVideoCopyNextTime.value > 0) && !self.videoEOF;
    while (self.demuxReader && self.demuxReader.status == AVAssetReaderStatusReading && (audioNeedLoad || videoNeedLoad)) {
        if (audioNeedLoad) {
            // Read audio data from the audio output source.
            CMSampleBufferRef next = [self.readerAudioOutput copyNextSampleBuffer];
            if (next) {
                if (CMTimeGetSeconds(CMSampleBufferGetPresentationTimeStamp(next)) <= CMTimeGetSeconds(_lastAudioCopyNextTime) || !CMSampleBufferGetDataBuffer(next)) {
                    // If the timestamp of the data retrieved from the output source is less than the last marked time, it indicates that this sample data has already been processed.
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
            // Read video data from the video output source.
            CMSampleBufferRef next = [self.readerVideoOutput copyNextSampleBuffer];
            if (next) {
                if (CMTimeGetSeconds(CMSampleBufferGetDecodeTimeStamp(next)) <= CMTimeGetSeconds(_lastVideoCopyNextTime) || !CMSampleBufferGetDataBuffer(next)) {
                    // If the timestamp of the data retrieved from the output source is less than the last marked time, it indicates that this sample data has already been processed.
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
