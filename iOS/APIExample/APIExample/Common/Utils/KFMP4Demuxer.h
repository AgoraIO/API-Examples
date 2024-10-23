//
//  KFMP4Demuxer.h
//  APIExample
//
//  Created by zhaoyongqiang on 2023/6/25.
//  Copyright © 2023 Agora Corp. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>
#import <CoreMedia/CoreMedia.h>

typedef NS_ENUM(NSInteger, KFMediaType) {
    KFMediaNone = 0,
    KFMediaAudio = 1 << 0, // Audio only.
    KFMediaVideo = 1 << 1, // Video only.
    KFMediaAV = KFMediaAudio | KFMediaVideo,  // Both audio and video.
};

typedef NS_ENUM(NSInteger, KFMP4DemuxerStatus) {
    KFMP4DemuxerStatusUnknown = 0,
    KFMP4DemuxerStatusRunning = 1,
    KFMP4DemuxerStatusFailed = 2,
    KFMP4DemuxerStatusCompleted = 3,
    KFMP4DemuxerStatusCancelled = 4,
};


@interface KFDemuxerConfig : NSObject
@property (nonatomic, strong) AVAsset *asset; // The resource to be demuxed.
@property (nonatomic, assign) KFMediaType demuxerType; // The type of demuxing.
@end

@interface KFVideoPacketExtraData : NSObject
@property (nonatomic, strong) NSData *sps;
@property (nonatomic, strong) NSData *pps;
@property (nonatomic, strong) NSData *vps;
@end


@interface KFMP4Demuxer : NSObject
+ (instancetype)new NS_UNAVAILABLE;
- (instancetype)init NS_UNAVAILABLE;
- (instancetype)initWithConfig:(KFDemuxerConfig *)config;

@property (nonatomic, strong, readonly) KFDemuxerConfig *config;
@property (nonatomic, copy) void (^errorCallBack)(NSError *error);
@property (nonatomic, assign, readonly) BOOL hasAudioTrack; // Indicates whether audio data is included.
@property (nonatomic, assign, readonly) BOOL hasVideoTrack; // Indicates whether video data is included.
@property (nonatomic, assign, readonly) CGSize videoSize; // Size of the video.
@property (nonatomic, assign, readonly) CMTime duration; // Duration of the media.
@property (nonatomic, assign, readonly) CMVideoCodecType codecType; // Encoding type.
@property (nonatomic, assign, readonly) KFMP4DemuxerStatus demuxerStatus; // Status of the demuxer.
@property (nonatomic, assign, readonly) BOOL audioEOF; // Indicates whether audio has ended.
@property (nonatomic, assign, readonly) BOOL videoEOF; // Indicates whether video has ended.
@property (nonatomic, assign, readonly) CGAffineTransform preferredTransform; // Transformation information for the image, e.g., video image rotation.
@property (nonatomic, copy) void (^dataCallBack)(NSData *data, CMSampleBufferRef sampleBuffer); // Callback for decoded data

- (void)startReading; // Start reading data for demuxing.
- (void)cancelReading; // Cancel reading.

- (BOOL)hasAudioSampleBuffer; // Indicates whether there are more audio samples.
- (CMSampleBufferRef)copyNextAudioSampleBuffer CF_RETURNS_RETAINED; // Copy the next audio sample.

- (BOOL)hasVideoSampleBuffer; // Indicates whether there are more video samples.
- (CMSampleBufferRef)copyNextVideoSampleBuffer CF_RETURNS_RETAINED; // Copy the next video sample.
@end
