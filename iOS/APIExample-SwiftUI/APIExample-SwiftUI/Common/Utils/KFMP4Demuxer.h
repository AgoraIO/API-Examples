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
    KFMediaAudio = 1 << 0,                   // Audio only.
    KFMediaVideo = 1 << 1,                   // Video only.
    KFMediaAV = KFMediaAudio | KFMediaVideo, // Both audio and video.
};

typedef NS_ENUM(NSInteger, KFMP4DemuxerStatus) {
    KFMP4DemuxerStatusUnknown = 0,
    KFMP4DemuxerStatusRunning = 1,
    KFMP4DemuxerStatusFailed = 2,
    KFMP4DemuxerStatusCompleted = 3,
    KFMP4DemuxerStatusCancelled = 4,
};

@interface KFDemuxerConfig : NSObject
@property(nonatomic, strong) AVAsset *asset;          // Asset to be demuxed.
@property(nonatomic, assign) KFMediaType demuxerType; // Demuxer type.
@end

@interface KFVideoPacketExtraData : NSObject
@property(nonatomic, strong) NSData *sps;
@property(nonatomic, strong) NSData *pps;
@property(nonatomic, strong) NSData *vps;
@end

@interface KFMP4Demuxer : NSObject
+ (instancetype)new NS_UNAVAILABLE;
- (instancetype)init NS_UNAVAILABLE;
- (instancetype)initWithConfig:(KFDemuxerConfig *)config;

@property(nonatomic, strong, readonly) KFDemuxerConfig *config;
@property(nonatomic, copy) void (^errorCallBack)(NSError *error);
@property(nonatomic, assign, readonly) BOOL hasAudioTrack;                                     // Whether contains audio data.
@property(nonatomic, assign, readonly) BOOL hasVideoTrack;                                     // Whether contains video data.
@property(nonatomic, assign, readonly) CGSize videoSize;                                       // Video size.
@property(nonatomic, assign, readonly) CMTime duration;                                        // Media duration.
@property(nonatomic, assign, readonly) CMVideoCodecType codecType;                             // Codec type.
@property(nonatomic, assign, readonly) KFMP4DemuxerStatus demuxerStatus;                       // Demuxer status.
@property(nonatomic, assign, readonly) BOOL audioEOF;                                          // Whether audio has ended.
@property(nonatomic, assign, readonly) BOOL videoEOF;                                          // Whether video has ended.
@property(nonatomic, assign, readonly) CGAffineTransform preferredTransform;                   // Image transform info, e.g. video rotation.
@property(nonatomic, copy) void (^dataCallBack)(NSData *data, CMSampleBufferRef sampleBuffer); // Callback for decoded data

- (void)startReading;  // Start reading data for demuxing.
- (void)cancelReading; // Cancel reading.

- (BOOL)hasAudioSampleBuffer;                                       // Whether has more audio data.
- (CMSampleBufferRef)copyNextAudioSampleBuffer CF_RETURNS_RETAINED; // Copy next audio sample.

- (BOOL)hasVideoSampleBuffer;                                       // Whether has more video data.
- (CMSampleBufferRef)copyNextVideoSampleBuffer CF_RETURNS_RETAINED; // Copy next video sample.
@end
