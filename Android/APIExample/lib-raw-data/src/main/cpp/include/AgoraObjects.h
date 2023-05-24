//
//  AgoraObjects.h
//  AgoraRtcEngineKit
//
//  Copyright (c) 2018 Agora. All rights reserved.
//

#import <CoreMedia/CoreMedia.h>
#import <Foundation/Foundation.h>
#import "AgoraEnumerates.h"
#import "AgoraMediaFilterEventDelegate.h"

#if TARGET_OS_IPHONE
#import <UIKit/UIKit.h>
typedef UIView VIEW_CLASS;
typedef UIColor COLOR_CLASS;
typedef UIImage IMAGE_CLASS;
#elif TARGET_OS_MAC
#import <AppKit/AppKit.h>
typedef NSView VIEW_CLASS;
typedef NSColor COLOR_CLASS;
typedef NSImage IMAGE_CLASS;
#endif

@protocol AgoraRtcMediaPlayerProtocol;
/**
 * @brief The player requests to read the data callback, you need to fill the specified length of data into the buffer
 * @param playerKit the player instance. \ref AgoraRtcMediaPlayerProtocol.
 * @param buffer the buffer pointer that you need to fill data.
 * @param bufferSize the bufferSize need to fill of the buffer pointer.
 * @return you need return offset value if succeed. return 0 if failed.
 */
typedef int(^AgoraRtcMediaPlayerCustomSourceOnReadCallback)(id<AgoraRtcMediaPlayerProtocol> _Nonnull playerKit, unsigned char * _Nullable buffer, int bufferSize);

/**
 * @brief The Player seek event callback, you need to operate the corresponding stream seek operation, You can refer to the definition of lseek() at https://man7.org/linux/man-pages/man2/lseek.2.html
 * @param playerKit the player instance. \ref AgoraRtcMediaPlayerProtocol.
 * @param offset the value of seek offset,
 * @param whence the postion of start seeking, the directive whence as follows:
 * SEEK_SET : The file offset is set to offset bytes.
 * SEEK_CUR : The file offset is set to its current location plus offset bytes.
 * SEEK_END : The file offset is set to the size of the file plus offset bytes.
 * 65536 - AVSEEK_SIZE : Optional. Passing this as the "whence" parameter to a seek function causes it to return the filesize without seeking anywhere.
 * @return
 * whence == 65536, return filesize if you need.
 * whence >= 0 && whence < 3 , return offset value if succeed. return -1 if failed.
 */
typedef long long(^AgoraRtcMediaPlayerCustomSourceOnSeekCallback)(id<AgoraRtcMediaPlayerProtocol> _Nonnull playerKit, long long offset, int whence);

__attribute__((visibility("default"))) @interface AgoraMediaSource : NSObject
/**
 * The URL of the media file that you want to play.
 */
@property(copy, nonatomic) NSString *_Nullable url;
/**
 * The URI of the media file.
 */
@property(copy, nonatomic) NSString *_Nullable uri;
/**
 * Set the starting position for playback, in seconds (ms)
 */
@property(assign, nonatomic) NSUInteger startPos;
/**
 * Determines whether to autoplay after opening a media resource.
 * - `YES`: (Default) Autoplay after opening a media resource.
 * - `NO`: Do not autoplay after opening a media resource.
 */
@property(assign, nonatomic) BOOL autoPlay;
/**
 * Determines whether to enable cache streaming to local files. If enable cached, the media player will
 * use the url or uri as the cache index.
 *
 * @note
 * The local cache function only supports on-demand video/audio streams and does not support live streams.
 * Caching video and audio files based on the HLS protocol (m3u8) to your local device is not supported.
 *
 * - `YES`: Enable cache.
 * - `NO`: (Default) Disable cache.
 */
@property(assign, nonatomic) BOOL enableCache;
/**
 * Determines whether the opened media resource is a stream through the Agora Broadcast Streaming Network(CDN).
 * - `YES`: It is a stream through the Agora Broadcast Streaming Network.
 * - `NO`: (Default) It is not a stream through the Agora Broadcast Streaming Network.
 */
@property(assign, nonatomic) BOOL isAgoraSource;
/**
 * Determines whether the opened media resource is a live stream. If is a live stream, it can speed up the opening of media resources.
 * - `YES`: It is a live stream.
 * - `NO`: (Default) It is not is a live stream.
 */
@property(assign, nonatomic) BOOL isLiveSource;
/**
 * External custom data source callback
 */
@property(copy, nonatomic) AgoraRtcMediaPlayerCustomSourceOnReadCallback _Nonnull playerOnReadCallback;
@property(copy, nonatomic) AgoraRtcMediaPlayerCustomSourceOnSeekCallback _Nonnull playerOnSeekCallback;

@end

__attribute__((visibility("default"))) @interface AgoraMediaPlayerCacheStatistics : NSObject

@property(assign, nonatomic) NSInteger fileSize;
@property(assign, nonatomic) NSInteger cacheSize;
@property(assign, nonatomic) NSInteger downloadSize;

@end

/**
 * @brief player_id and device_id has value when user trigger interface of opening.
 *
 */
__attribute__((visibility("default"))) @interface AgoraMediaPlayerUpdatedInfo : NSObject
@property(copy, nonatomic) NSString *_Nullable playerId;
@property(copy, nonatomic) NSString *_Nullable deviceId;
@property(strong, nonatomic) AgoraMediaPlayerCacheStatistics *_Nullable cacheStatistics;
@end

/**
 * @brief The information of the media stream object.
 *
 */
__attribute__((visibility("default"))) @interface AgoraMediaPlayerSrcInfo : NSObject
/**
 * The bitrate of the media stream. The unit of the number is kbps.
 */
@property(assign, nonatomic) int bitrateInKbps;
/**
 * The name of the media stream.
*/
@property(copy, nonatomic) NSString *_Nullable name;

@end


/**
 * The statistics of the Direct Cdn Streams.
 */
__attribute__((visibility("default"))) @interface AgoraDirectCdnStreamingStats : NSObject
/**
 * Width of the video pushed by rtmp.
 */
@property(assign, nonatomic) NSUInteger videoWidth;
/**
 * Height of the video pushed by rtmp.
 */
@property(assign, nonatomic) NSUInteger videoHeight;
/**
 * The frame rate of the video pushed by rtmp.
 */
@property(assign, nonatomic) NSUInteger fps;
/**
 * Real-time bit rate of the video streamed by rtmp.
 */
@property(assign, nonatomic) NSUInteger videoBitrate;
/**
 * Real-time bit rate of the audio pushed by rtmp.
 */
@property(assign, nonatomic) NSUInteger audioBitrate;

@end

/**
 * The cdn streaming media options.
 */
__attribute__((visibility("default"))) @interface AgoraDirectCdnStreamingMediaOptions : NSObject
/**
 * Determines whether to publish the video of the camera track.
 * - `YES`: Publish the video track of the camera capturer.
 * - `NO`: (Default) Do not publish the video track of the camera capturer.
 */
@property(assign, nonatomic) BOOL publishCameraTrack;
/**
 * Determines whether to publish the recorded audio.
 * - `YES`: Publish the recorded audio.
 * - `NO`: (Default) Do not publish the recorded audio.
 */
@property(assign, nonatomic) BOOL publishMicrophoneTrack;
/**
 * Determines whether to publish the audio of the custom audio track.
 * - `YES`: Publish the audio of the custom audio track.
 * - `NO`: (Default) Do not publish the audio of the custom audio track.
 */
@property(assign, nonatomic) BOOL publishCustomAudioTrack;
/**
 * Determines whether to publish the video of the custom video track.
 * - `YES`: Publish the video of the custom video track.
 * - `NO`: (Default) Do not publish the video of the custom video track.
 */
@property(assign, nonatomic) BOOL publishCustomVideoTrack;
/**
 * Determines whether to publish the audio track of media player source.
 * - `YES`: Publish the audio track of media player source.
 * - `NO`: (Default) Do not publish the audio track of media player source.
*/
@property(assign, nonatomic) BOOL publishMediaPlayerAudioTrack;
/**
 * Determines which media player source should be published.
 * This parameter get from function getMediaPlayerId() of AgoraMediaPlayer.
*/
@property(assign, nonatomic) NSInteger publishMediaPlayerId;

/**
 * The custom video track id which will used to publish.
 */
@property(assign, nonatomic) NSInteger customVideoTrackId;

@end

/**
 * The video encoded track options.
 */
__attribute__((visibility("default"))) @interface AgoraEncodedVideoTrackOptions : NSObject
/**
 * Whether to enable CC mode.
 * - TCC_ENABLED = 0: (Default) enable cc.
 * - TCC_DISABLED = 1: disable cc.
 */
@property(assign, nonatomic) int ccMode;
/**
 * The codec type used for the encoded images.
 * - VIDEO_CODEC_VP8 = 1: VP8.
 * - VIDEO_CODEC_H264 = 2: (Default) H.264.
 * - VIDEO_CODEC_H265 = 3: H.265.
 * - VIDEO_CODEC_VP9 = 5: VP9.
 * - VIDEO_CODEC_GENERIC = 6: GENERIC.
 * - VIDEO_CODEC_GENERIC_H264 = 7: GENERIC_H264.
 * - VIDEO_CODEC_GENERIC_JPEG = 20: GENERIC_JPEG.
 */
@property(assign, nonatomic) int codecType;
/**
 * Target bitrate (Kbps) for sending encoded video frame.
 */
@property(assign, nonatomic) int targetBitrate;
@end

/**
 * The channel media options.
 */
__attribute__((visibility("default"))) @interface AgoraRtcChannelMediaOptions : NSObject
/**
 * Determines whether to publish the video of the camera track.
 * - `YES`: (Default) Publish the video track of the camera capturer.
 * - `NO`: Do not publish the video track of the camera capturer.
 */
@property(assign, nonatomic) BOOL publishCameraTrack;
/**
 * Determines whether to publish the video of the secondary camera track.
 * - `YES`: Publish the video track of the secondary camera capturer.
 * - `NO`: (Default) Do not publish the video track of the secondary camera capturer.
 */
@property(assign, nonatomic) BOOL publishSecondaryCameraTrack;
/**
 * Determines whether to publish the recorded audio of microphone.
 * - `YES`: (Default) Publish the recorded audio of microphone.
 * - `NO`: Do not publish the recorded audio of microphone.
 */
@property(assign, nonatomic) BOOL publishMicrophoneTrack;
#if TARGET_OS_IPHONE
/**
 * Determines whether to publish the video of the screen capturer.
 * - `YES`: Publish the video track of the screen capturer.
 * - `NO`: (Default) Do not publish the video track of the screen capturer.
 */
@property(assign, nonatomic) BOOL publishScreenCaptureVideo;
/**
 * Determines whether to publish the audio of the screen capturer.
 * - `YES`: Publish the audio track of the screen capturer.
 * - `NO`: (Default) Do not publish the audio track of the screen capturer.
 */
@property(assign, nonatomic) BOOL publishScreenCaptureAudio;
#elif TARGET_OS_MAC
/**
 * Determines whether to publish the video of the screen capturer.
 * - `YES`: Publish the video track of the screen capturer.
 * - `NO`: (Default) Do not publish the video track of the screen capturer.
 */
@property(assign, nonatomic) BOOL publishScreenTrack;
/**
 * Determines whether to publish the video of the secondary screen track.
 * - `YES`: Publish the video track of the secondary screen capturer.
 * - `NO`: (Default) Do not publish the video track of the secondary screen capturer.
 */
@property(assign, nonatomic) BOOL publishSecondaryScreenTrack;
#endif
/**
 * Determines whether to publish the audio of the custom audio track.
 * - `YES`: Publish the audio of the custom audio track.
 * - `NO`: (Default) Do not publish the audio of the custom audio track.
 */
@property(assign, nonatomic) BOOL publishCustomAudioTrack;
/**
 * The custom audio track id. The default value is 0.
 */
@property(assign, nonatomic) NSInteger publishCustomAudioTrackId;
/**
 * Determines whether to publish AEC custom audio track.
 * - `YES`: Publish AEC track.
 * - `NO`: (Default) Do not publish AEC track.
 */
@property(assign, nonatomic) BOOL publishCustomAudioTrackAec;
/**
 * Determines whether to publish the video of the custom video track.
 * - `YES`: Publish the video of the custom video track.
 * - `NO`: (Default) Do not publish the video of the custom video track.
 */
@property(assign, nonatomic) BOOL publishCustomVideoTrack;
/**
 * Determines whether to publish the video of the encoded video track.
 * - `YES`: Publish the video of the encoded video track.
 * - `NO`: (Default) Do not publish the video of the encoded video track.
 */
@property(assign, nonatomic) BOOL publishEncodedVideoTrack;
/**
 * Determines whether to publish the audio track of media player.
 * - `YES`: Publish the audio track of media player.
 * - `NO`: (Default) Do not publish the audio track of media player.
 */
@property(assign, nonatomic) BOOL publishMediaPlayerAudioTrack;
/**
* Determines whether to publish the video track of media player source.
* - `YES`: Publish the video track of media player source.
* - `NO`: (Default) Do not publish the video track of media player source.
*/
@property(assign, nonatomic) BOOL publishMediaPlayerVideoTrack;
/**
* Determines whether to publish the local transcoded video track.
* - `YES`: Publish the video track of local transcoded video track.
* - `NO`: (Default) Do not publish the local transcoded video track.
*/
@property(assign, nonatomic) BOOL publishTrancodedVideoTrack;
/**
 * Determines whether to subscribe all remote audio streams automatically.
 * This property replaces calling \ref AgoraRtcEngineKit.setDefaultMuteAllRemoteAudioStreams: setDefaultMuteAllRemoteAudioStreams
 * before joining a channel.
 * - `YES`: (Default) Subscribe all remote audio streams automatically.
 * - `NO`: Do not subscribe any remote audio stream automatically.
 */
@property(assign, nonatomic) BOOL autoSubscribeAudio;
/**
 * Determines whether to subscribe all remote video streams automatically.
 * This property replaces calling \ref AgoraRtcEngineKit.setDefaultMuteAllRemoteVideoStreams: setDefaultMuteAllRemoteVideoStreams
 * before joining a channel.
 * - `YES`: (Default) Subscribe all remote video streams automatically.
 * - `NO`: Do not subscribe any remote video stream automatically.
 */
@property(assign, nonatomic) BOOL autoSubscribeVideo;
/**
 * Determines whether to enable audio recording or playout.
 * - `YES`: (Default) It's used to publish audio and mix microphone, or subscribe audio and playout.
 * - `NO`: It's used to publish extenal audio frame only without mixing microphone, or no need audio device to playout audio either.
 */
@property(assign, nonatomic) BOOL enableAudioRecordingOrPlayout;
/**
* Determines which media player source should be published.
* This parameter get from function getMediaPlayerId() of AgoraMediaPlayer.
*/
@property(assign, nonatomic) NSInteger publishMediaPlayerId;
/**
 * The client role type: \ref AgoraClientRole.
 * Default is AgoraClientRoleAudience.
 */
@property(assign, nonatomic) AgoraClientRole clientRoleType;
/**
 * The audience latency level type: \ref AgoraAudienceLatencyLevelType.
 * Default is AgoraAudienceLatencyLevelUltraLowLatency.
 */
@property(assign, nonatomic) AgoraAudienceLatencyLevelType audienceLatencyLevel;
/**
 * The default video stream type: \ref AgoraVideoStreamType.
 * Default is AgoraVideoStreamTypeHigh.
 */
@property(assign, nonatomic) AgoraVideoStreamType defaultVideoStreamType;
/**
 * The channel profile: \ref AgoraChannelProfile.
 * Default is AgoraChannelProfileLiveBroadcasting.
 */
@property(assign, nonatomic) AgoraChannelProfile channelProfile;
/**
 * The delay in ms for sending audio frames. This is used for explicit control of A/V sync.
 * To switch off the delay, set the value to zero.
 */
@property(assign, nonatomic) NSInteger audioDelayMs;
/**
 * The delay in ms for sending media player audio frames. This is used for explicit control of A/V sync.
 * To switch off the delay, set the value to zero.
 */
@property(assign, nonatomic) NSInteger mediaPlayerAudioDelayMs;
/**
 * The token to be renewed.
 */
@property(copy, nonatomic) NSString * _Nullable token;
/**
 * Enable media packet encryption.
 * It will be ignored when calling function updateChannelMediaOptions().
 * - `YES`:  Enable media packet encryption.
 * - `NO`: (Default) Do not Enable media packet encryption.
 */
@property(assign, nonatomic) BOOL enableBuiltInMediaEncryption;

/**
 * Determines whether to publish the sound of the rhythm player to remote users.
 * - true: (Default) Publish the sound of the rhythm player.
 * - false: Do not publish the sound of the rhythm player.
 */
@property(assign, nonatomic) BOOL publishRhythmPlayerTrack;
/**
 * This mode is only used for audience. In PK mode, client might join one
 * channel as broadcaster, and join another channel as interactive audience to
 * achieve low lentancy and smooth video from remote user.
 * - `YES`: Enable low lentancy and smooth video when joining as an audience.
 * - `NO`: (default) Use default settings for audience role.
 */
@property(assign, nonatomic) BOOL isInteractiveAudience;

/**
 * The custom video track id which will used to publish or preview.
 */
@property(assign, nonatomic) NSInteger customVideoTrackId;

/**
 * Determines whether local audio stream can be filtered.
 * - `YES`: (Default) Can be filtered when audio level is low.
 * - `NO`: Do not filter this audio stream.
 */
@property(assign, nonatomic) BOOL isAudioFilterable;

@end

/** Properties of the video canvas object.
 */
__attribute__((visibility("default"))) @interface AgoraRtcVideoCanvas : NSObject
/**
 *The video display view. The SDK does not maintain the lifecycle of the view.
 *
 *The view can be safely released after calling [leaveChannel]([AgoraRtcEngineKit
 *leaveChannel:]) with a returned value. The SDK keeps a cache of the view value, so calling
 *[setupLocalVideo]([AgoraRtcEngineKit setupLocalVideo:]) to set the view value to nil can
 *clear the cache before switching or releasing the view.
 */
@property(strong, nonatomic) VIEW_CLASS *_Nullable view;
/**
 * The user id of local video.
 */
@property(assign, nonatomic) NSUInteger uid;
/**
 * The video render mode. See \ref AgoraVideoRenderMode.
 * The default value is AgoraVideoRenderModeHidden.
 */
@property(assign, nonatomic) AgoraVideoRenderMode renderMode;
/**
 * The video mirror mode. See \ref AgoraVideoMirrorMode.
 * The default value is AgoraVideoMirrorModeAuto.
 * @note
 * - For the mirror mode of the local video view: 
 * If you use a front camera, the SDK enables the mirror mode by default;
 * if you use a rear camera, the SDK disables the mirror mode by default.
 * - For the remote user: The mirror mode is disabled by default.
 */
@property(assign, nonatomic) AgoraVideoMirrorMode mirrorMode;
/**
 * The mode of setting up video view. See \ref AgoraVideoViewSetupMode.
 * The default value is AgoraVideoViewSetupReplace.
 */
@property(assign, nonatomic) AgoraVideoViewSetupMode setupMode;
/**
 * The video source type. See \ref AgoraVideoSourceType
 * The default value is AgoraVideoSourceTypeCamera.
 */
@property(nonatomic, assign) AgoraVideoSourceType sourceType;
/**
 * The media player id of AgoraMediaPlayer. It should set this parameter when the 
 * sourceType is AgoraVideoSourceTypeMediaPlayer to show the video that AgoraMediaPlayer is playing.
 * You can get this value by calling the method \ref getMediaPlayerId().
 */
@property(nonatomic, assign) int mediaPlayerId;
/**
 * If you want to display a certain part of a video frame, you can set 
 * this value to crop the video frame to show. 
 * The default value is empty(that is, if it has zero width or height), which means no cropping.
 */
@property(assign, nonatomic) CGRect cropArea;
@end

/**
 * The configurations for the last-mile network probe test.
 */
__attribute__((visibility("default"))) @interface AgoraLastmileProbeConfig : NSObject
/**
 * Sets whether or not to test the uplink network.
 *
 * Some users, for example, the audience in a live-broadcast channel, do not need such a test.
 * - `YES`: Enables the test.
 * - `NO`: Disables the test.
 */
@property (assign, nonatomic) BOOL probeUplink;
/**
 * Sets whether or not to test the downlink network.
 * - `YES`: Enables the test.
 * - `NO`: Disables the test.
 */
@property (assign, nonatomic) BOOL probeDownlink;
/**
 * Sets the expected maximum sending bitrate (bps) of the local user.
 *
 * The value ranges between 100000 and 5000000. Agora recommends
 * setting this value according to the required bitrate of selected video profile.
 */
@property (assign, nonatomic) NSUInteger expectedUplinkBitrate;
/**
 * Sets the expected maximum receiving bitrate (bps) of the local user.
 * The value ranges between 100000 and 5000000.
 */
@property (assign, nonatomic) NSUInteger expectedDownlinkBitrate;
@end

/**
 * The one-way last-mile probe result.
 */
__attribute__((visibility("default"))) @interface AgoraLastmileProbeOneWayResult : NSObject
/**
 * The packet loss rate (%).
 */
@property (assign, nonatomic) NSUInteger packetLossRate;
/**
 * The network jitter (ms).
 */
@property (assign, nonatomic) NSUInteger jitter;
/**
 * The estimated available bandwidth (bps).
 */
@property (assign, nonatomic) NSUInteger availableBandwidth;
@end

/**
 * Statistics of the last-mile probe.
 */
__attribute__((visibility("default"))) @interface AgoraLastmileProbeResult : NSObject
/**
 * The state of the probe test.
 * See \ref AgoraLastmileProbeResultState.
 */
@property (assign, nonatomic) AgoraLastmileProbeResultState state;
/**
 * The round-trip delay time (ms).
 */
@property (assign, nonatomic) NSUInteger rtt;
/**
 * The uplink last-mile network report.
 *
 * See \ref AgoraLastmileProbeOneWayResult.
 */
@property (strong, nonatomic) AgoraLastmileProbeOneWayResult *_Nonnull uplinkReport;
/**
 * The downlink last-mile network report.
 *
 * See \ref AgoraLastmileProbeOneWayResult.
 */
@property (strong, nonatomic) AgoraLastmileProbeOneWayResult *_Nonnull downlinkReport;
@end


/**
 * The statistics of the local video stream.
 */
__attribute__((visibility("default"))) @interface AgoraRtcLocalVideoStats : NSObject
/**
 * Bitrate (Kbps) sent in the reported interval, which does not include
 * the bitrate of the retransmission video after packet loss.
 */
@property(assign, nonatomic) NSUInteger sentBitrate;
/**
 * Frame rate (fps) sent in the reported interval, which does not include
 * the frame rate of the retransmission video after packet loss.
 */
@property(assign, nonatomic) NSUInteger sentFrameRate;
/**
 * ID of the local user.
 */
@property(assign, nonatomic) NSUInteger uid;

/** The capture frame rate (fps) of the local video.
 */
@property(assign, nonatomic) NSInteger captureFrameRate;
/** The width of the capture frame (px).
 */
@property(assign, nonatomic) NSInteger captureFrameWidth;
/** The height of the capture frame (px).
 */
@property(assign, nonatomic) NSInteger captureFrameHeight;
/**
 * The regulated frame rate of capture frame rate according to video encoder configuration.
 */
@property(assign, nonatomic) NSInteger regulatedCaptureFrameRate;
/**
 * The regulated frame width (pixel) of capture frame width according to video encoder configuration.
 */
@property(assign, nonatomic) NSInteger regulatedCaptureFrameWidth;
/**
 * The regulated frame height (pixel) of capture frame height according to video encoder configuration.
 */
@property(assign, nonatomic) NSInteger regulatedCaptureFrameHeight;

/** The encoder output frame rate (fps) of the local video.
 */
@property(assign, nonatomic) NSInteger encoderOutputFrameRate;
/** The render output frame rate (fps) of the local video.
 */
@property(assign, nonatomic) NSInteger rendererOutputFrameRate;
/** The target frame rate (fps) of the current encoder.
  */
@property(assign, nonatomic) NSInteger targetFrameRate;
/** Quality change of the local video in terms of target frame rate and target bit rate in this reported interval, see [AgoraVideoQualityAdaptIndication](AgoraVideoQualityAdaptIndication). */
@property(assign, nonatomic) AgoraVideoQualityAdaptIndication qualityAdaptIndication;
/**
 * The target bitrate (Kbps) of the current encoder. This value is
 * estimated by the SDK based on the current network conditions.
 */
@property(assign, nonatomic) NSInteger targetBitrate;
/**
 * The encoding bitrate (Kbps), which does not include the bitrate of the
 * re-transmission video after packet loss.
 */
@property(assign, nonatomic) NSInteger encodedBitrate;
/**
 * The width of the encoding frame (px).
 */
@property(assign, nonatomic) NSInteger encodedFrameWidth;
/**
 * The height of the encoding frame (px).
 */
@property(assign, nonatomic) NSInteger encodedFrameHeight;
/**
 * The number of the sent frames, represented by an aggregate value.
 */
@property(assign, nonatomic) NSInteger encodedFrameCount;
/**
 * The codec type of the local video:
 * - VIDEO_CODEC_VP8 = 1: VP8.
 * - VIDEO_CODEC_H264 = 2: (Default) H.264.
 */
@property(assign, nonatomic) AgoraVideoCodecType codecType;
/**
 * The hw_encoder_accelerating of the local video:
 * - hwEncoderAccelerating = 0: the encoder is software
 * - hwEncoderAccelerating = 1: the encoder is hardware
 */
@property(assign, nonatomic) AgoraVideoHwEncoderAccelerating hwEncoderAccelerating;
/**
 * The video packet loss rate (%) from the local client to the Agora edge server before applying the anti-packet loss strategies.
 */
@property(assign, nonatomic) NSInteger txPacketLossRate;
/**
 * The brightness level of the video image captured by the local camera. See AgoraCaptureBrightnessLevelType.
 */
@property(assign, nonatomic) AgoraCaptureBrightnessLevelType captureBrightnessLevel NS_SWIFT_NAME(captureBrightnessLevel);
/** 
 * Whether we send dual stream now. 
 */
@property(assign, nonatomic) BOOL dualStreamEnabled;
@end

/** Statistics of the remote video stream. */
__attribute__((visibility("default"))) @interface AgoraRtcRemoteVideoStats : NSObject
/**
 * ID of the remote user sending the video stream.
 */
@property(assign, nonatomic) NSUInteger uid;
/** Time delay (ms).
 */
@property(assign, nonatomic) NSUInteger delay __deprecated;
/** Width (pixels) of the video stream.
 */
@property(assign, nonatomic) NSUInteger width;
/** Height (pixels) of the video stream.
 */
@property(assign, nonatomic) NSUInteger height;
/** Data receive bitrate (Kbps) since last count.
 */
@property(assign, nonatomic) NSUInteger receivedBitrate;
/** Data receive frame rate (fps) since last count.
 */
@property(assign, nonatomic) NSUInteger receivedFrameRate;
/** Video stream type; high- or low-video stream.
 */
@property(assign, nonatomic) AgoraVideoStreamType rxStreamType;

/** The decoder output frame rate (fps) of the remote video.
 */
@property(assign, nonatomic) NSInteger decoderOutputFrameRate;
/** The render output frame rate (fps) of the remote video.
 */
@property(assign, nonatomic) NSInteger rendererOutputFrameRate;
/** The video frame loss rate (%) of the remote video stream in the reported interval.
 */
@property(assign, nonatomic) NSInteger frameLossRate;
/** Packet loss rate (%) of the remote video stream after using the anti-packet-loss method.
 */
@property(assign, nonatomic) NSInteger packetLossRate;
/**
 The total freeze time (ms) of the remote video stream after the remote user joins the channel.
 In a video session where the frame rate is set to no less than 5 fps, video freeze occurs when
 the time interval between two adjacent renderable video frames is more than 500 ms.
 */
@property(assign, nonatomic) NSInteger totalFrozenTime;
/**
 * The total time (ms) when the remote user neither stops sending the audio
 * stream nor disables the audio module after joining the channel.
 */
@property(assign, nonatomic) NSUInteger totalActiveTime;
/**
 * The total publish duration (ms) of the remote audio stream.
 */
@property(assign, nonatomic) NSInteger publishDuration;
/**
 The total video freeze time as a percentage (%) of the total time when the video is available.
 */
@property(assign, nonatomic) NSInteger frozenRate;
/**
 The offset (ms) between audio and video stream. A positive value indicates the audio leads the
 video, and a negative value indicates the audio lags the video.
 */
@property(assign, nonatomic) NSInteger avSyncTimeMs;
/**
 * The quality of the remote video stream in the reported interval. 
 * The quality is determined by the Agora real-time video MOS (Mean Opinion Score) measurement method. 
 * The return value range is [0, 500]. 
 * Dividing the return value by 100 gets the MOS score, which ranges from 0 to 5. The higher the score, the better the video quality.
 * @note For textured video data, this parameter always returns 0.
 */
@property(assign, nonatomic) NSInteger mosValue;
/**
 * Total number of video bytes received (bytes), represented by an aggregate value.
 */
@property(assign, nonatomic) NSUInteger rxVideoBytes;

@end

/**
 * The statistics of the local audio.
 */
__attribute__((visibility("default"))) @interface AgoraRtcLocalAudioStats : NSObject
/**
 * The number of audio channels.
 */
@property(assign, nonatomic) NSUInteger numChannels;
/**
 * The sample rate (Hz).
 */
@property(assign, nonatomic) NSUInteger sentSampleRate;
/**
 * The average sending bitrate (Kbps).
 */
@property(assign, nonatomic) NSUInteger sentBitrate;
/** The internal payload type.
 */
@property(assign, nonatomic) NSUInteger internalCodec;
/** The audio packet loss rate (%) from the local client to the Agora edge server before applying the anti-packet loss strategies.
 */
@property(assign, nonatomic) NSUInteger txPacketLossRate;
/** 
 * The audio delay of the device, contains record and playout delay
 */
@property(assign, nonatomic) NSUInteger audioDeviceDelay;
/**
 * The audio playout delay of the device
 */
@property(assign, nonatomic) NSUInteger audioPlayoutDelay;

@end

/**
 * The statistics of the remote audio.
 */
__attribute__((visibility("default"))) @interface AgoraRtcRemoteAudioStats : NSObject
/**
 * ID of the user sending the audio stream.
 */
@property(assign, nonatomic) NSUInteger uid;
/**
 * The receiving audio quality.
 *
 * - 0: The quality is unknown.
 * - 1: The quality is excellent.
 * - 2: The quality is quite good, but the bitrate may be slightly
 * lower than excellent.
 * - 3: Users can feel the communication slightly impaired.
 * - 4: Users can communicate not very smoothly.
 * - 5: The quality is so bad that users can barely communicate.
 * - 6: The network is disconnected and users cannot communicate at all.
 */
@property(assign, nonatomic) NSUInteger quality;
/**
 * The network delay (ms) from the sender to the receiver.
 */
@property(assign, nonatomic) NSUInteger networkTransportDelay;
/**
 * The jitter buffer delay (ms) at the receiver.
 */
@property(assign, nonatomic) NSUInteger jitterBufferDelay;
/**
 * The packet loss rate in the reported interval.
 */
@property(assign, nonatomic) NSUInteger audioLossRate;
/**
 * The number of audio channels.
 */
@property(assign, nonatomic) NSUInteger numChannels;
/**
 * The sample rate (Hz) of the received audio stream, represented by an instantaneous value.
 */
@property(assign, nonatomic) NSUInteger receivedSampleRate;
/**
 * The bitrate (Kbps) of the received audio stream, represented by an instantaneous value.
 */
@property(assign, nonatomic) NSUInteger receivedBitrate;
/**
 * The total freeze time (ms) of the remote audio stream after the remote user joins the channel.
 * In a session, audio freeze occurs when the audio frame loss rate reaches 4% within two seconds.
 * Agora uses 2 seconds as an audio piece unit to calculate the audio freeze time.
 * The total audio freeze time = The audio freeze number &times; 2 seconds
 */
@property(assign, nonatomic) NSUInteger totalFrozenTime;
/**
 * The total time (ms) when the remote user neither stops sending the audio
 * stream nor disables the audio module after joining the channel.
 */
@property(assign, nonatomic) NSUInteger totalActiveTime;
/**
 * The total publish duration (ms) of the remote audio stream.
 */
@property(assign, nonatomic) NSInteger publishDuration;
/**
 * The total audio freeze time as a percentage (%) of the total time when the audio is available.
 */
@property(assign, nonatomic) NSUInteger frozenRate;
/**
 * The quality of the remote audio stream as determined by the Agora real-time
 * audio MOS (Mean Opinion Score) measurement method in the reported interval.
 * The return value ranges from 0 to 500. Dividing the return value by 100 gets
 * the MOS score, which ranges from 0 to 5. The higher the score, the better the audio quality.
 *
 * The subjective perception of audio quality corresponding to the Agora real-time audio MOS scores is as follows:
 *
 * | MOS score      | Perception of audio quality                                  |
 * | :------------- | :----------------------------------------------------------- |
 * | Greater than 4 | Excellent. The audio sounds clear and smooth.                |
 * | From 3.5 to 4  | Good. The audio has some perceptible impairment, but still sounds clear. |
 * | From 3 to 3.5  | Fair. The audio freezes occasionally and requires attentive listening. |
 * | From 2.5 to 3  | Poor. The audio sounds choppy and requires considerable effort to understand. |
 * | From 2 to 2.5  | Bad. The audio has occasional noise. Consecutive audio dropouts occur, resulting in some information loss. The users can communicate only with difficulty. |
 * | Less than 2    | Very bad. The audio has persistent noise. Consecutive audio dropouts are frequent, resulting in severe information loss. Communication is nearly impossible. |
 */
@property(assign, nonatomic) NSUInteger mosValue;
/**
 * Quality of experience (QoE) of the local user when receiving a remote audio stream.
 */
@property(assign, nonatomic) AgoraExperienceQuality qoeQuality;
/**
  * The reason for poor QoE of the local user when receiving a remote audio stream. See #EXPERIENCE_POOR_REASON.
  */
@property(assign, nonatomic) AgoraExperiencePoorReason qualityChangedReason;
/**
 * Total number of audio bytes received (bytes) before network countermeasures, represented by
 * an aggregate value.
 */
@property(assign, nonatomic) NSUInteger rxAudioBytes;
@end

/** Properties of the audio volume information.
 */
__attribute__((visibility("default"))) @interface AgoraRtcAudioVolumeInfo : NSObject
/** User ID of the speaker.
 */
@property(assign, nonatomic) NSUInteger uid;
/** The volume of the speaker that ranges from 0 (lowest volume) to 255 (highest volume).
 */
@property(assign, nonatomic) NSUInteger volume;

@property(assign, nonatomic) NSUInteger vad;

/** Voice pitch frequency in Hz.
 */
@property (assign, nonatomic) double voicePitch;

@end

/**
 * The Statistics of the channel.
 */
__attribute__((visibility("default"))) @interface AgoraChannelStats : NSObject
/**
 * The call duration in seconds, represented by an aggregate value.
 */
@property(assign, nonatomic) NSUInteger duration;
/**
 * The total number of bytes transmitted, represented by an aggregate value.
 */
@property(assign, nonatomic) NSUInteger txBytes;
/**
 * The total number of bytes received, represented by an aggregate value.
 */
@property(assign, nonatomic) NSUInteger rxBytes;
/**
 * The audio transmission bitrate in Kbps, represented by an instantaneous value.
 */
@property(assign, nonatomic) NSUInteger txAudioKBitrate;
/**
 * The audio receiving bitrate in Kbps, represented by an instantaneous value.
 */
@property(assign, nonatomic) NSUInteger rxAudioKBitrate;
/**
 * The video transmission bitrate in Kbps, represented by an instantaneous value.
 */
@property(assign, nonatomic) NSUInteger txVideoKBitrate;
/**
 * The video receiving bitrate in Kbps, represented by an instantaneous value.
 */
@property(assign, nonatomic) NSUInteger rxVideoKBitrate;
/**
 * Total number of audio bytes sent (bytes), represented by an aggregate value.
 */
@property(assign, nonatomic) NSUInteger txAudioBytes;
/**
 * Total number of video bytes sent (bytes), represented by an aggregate value.
 */
@property(assign, nonatomic) NSUInteger txVideoBytes;
/**
 * Total number of audio bytes received (bytes) before network countermeasures, represented by
 * an aggregate value.
 */
@property(assign, nonatomic) NSUInteger rxAudioBytes;
/**
 * Total number of video bytes received (bytes), represented by an aggregate value.
 */
@property(assign, nonatomic) NSUInteger rxVideoBytes;
/**
 * The client-server latency (ms).
 */
@property(assign, nonatomic) NSUInteger lastmileDelay;
/**
 * The number of users in the channel.
 */
@property(assign, nonatomic) NSUInteger userCount;
/** Application CPU usage (%).
 */
@property(assign, nonatomic) double cpuAppUsage;
/** System CPU usage (%).
 */
@property(assign, nonatomic) double cpuTotalUsage;
/** Gateway Rtt.
 */
@property(assign, nonatomic) NSInteger gatewayRtt;
/**
 * The memory usage ratio of the app (%).
 */
@property(assign, nonatomic) double memoryAppUsageRatio;
/**
 * The memory usage ratio of the system (%).
 */
@property(assign, nonatomic) double memoryTotalUsageRatio;
/**
 * The memory usage of the app (KB).
 */
@property(assign, nonatomic) NSInteger memoryAppUsageInKbytes;
/**
 * The time interval (ms) between establishing the connection and the connection succeeds, 0 if not valid.
 */
@property(assign, nonatomic) NSInteger connectTimeMs;

/**
 * The transmission bitrate in Kbps, represented by an instantaneous value.
 */
@property(assign, nonatomic) NSUInteger txKBitrate;
/**
 * The receiving bitrate in Kbps, represented by an instantaneous value.
 */
@property(assign, nonatomic) NSUInteger rxKBitrate;

/**The duration(ms) between first audio packet received and connection start, 0 if not valid
 */
@property(assign, nonatomic) NSInteger firstAudioPacketDuration;
/** The duration(ms) between first video packet received and connection start, 0 if not valid
 */
@property(assign, nonatomic) NSInteger firstVideoPacketDuration;
/** The duration(ms) between first video key frame received and connection start, 0 if not valid
 */
@property(assign, nonatomic) NSInteger firstVideoKeyFramePacketDuration;
/** Video packet number before first video key frame received, 0 if not valid
 */
@property(assign, nonatomic) NSInteger packetsBeforeFirstKeyFramePacket;
/**
 * The packet loss rate of sender(broadcaster).
 */
@property(assign, nonatomic) NSInteger txPacketLossRate;
/**
 * The packet loss rate of receiver(audience).
 */
@property(assign, nonatomic) NSInteger rxPacketLossRate;

@end
/** Properties of the video encoder configuration.
*/
__attribute__((visibility("default"))) @interface AgoraAdvancedVideoOptions : NSObject
/**
* The video encoder encodingPreference hard or soft
*/
@property(assign, nonatomic) AgoraEncodingPreference encodingPreference;

/** The video compression preference. */
@property(assign, nonatomic) AgoraCompressionPreference compressionPreference;

@end

/** 
 * The codec support information.
*/
__attribute__((visibility("default"))) @interface AgoraVideoCodecCapInfo : NSObject

/** The codec type. */
@property(assign, nonatomic) AgoraVideoCodecType codecType;
/** The codec type mask. bit 1 Hardware decoder support flag, bit 2: Hardware encoder support flag, 
   * bit 3: Software decoder support flag, bit 4: Software encoder support flag */
@property(assign, nonatomic) NSUInteger codecCapMask;

@end
/** Properties of the video encoder configuration.
 */
__attribute__((visibility("default"))) @interface AgoraVideoEncoderConfiguration : NSObject
/** Video resolution that can be set manually or by choosing from one of the following
 enumerations, the default value is 960x540:

 - AgoraVideoDimension120x120
 - AgoraVideoDimension160x120
 - AgoraVideoDimension180x180
 - AgoraVideoDimension240x180
 - AgoraVideoDimension320x180
 - AgoraVideoDimension240x240
 - AgoraVideoDimension320x240
 - AgoraVideoDimension424x240
 - AgoraVideoDimension360x360
 - AgoraVideoDimension480x360
 - AgoraVideoDimension640x360
 - AgoraVideoDimension480x480
 - AgoraVideoDimension640x480
 - AgoraVideoDimension840x480
 - AgoraVideoDimension960x540
 - AgoraVideoDimension960x720
 - AgoraVideoDimension1280x720
 - AgoraVideoDimension1920x1080
 - AgoraVideoDimension2540x1440
 - AgoraVideoDimension3840x2160

 @note Whether 720p can be supported depends on the device. If the device cannot support 720p,
 the frame rate will be lower than the one listed in the table. Agora optimizes the video in
 lower-end devices. Contact mailto:support@agora.io for special requirements.
 */
@property(assign, nonatomic) CGSize dimensions;

/** Codec type. See \ref AgoraVideoCodecType.
 */
@property(assign, nonatomic) AgoraVideoCodecType codecType;

/** Frame rate of the video: AgoraVideoFrameRate
 */
@property(assign, nonatomic) AgoraVideoFrameRate frameRate;

/** Bitrate of the video:

 Refer to AgoraVideoProfile and set your desired bitrate. If the bitrate you set is beyond the
 proper range, the SDK will automatically adjust it to a value within the range. You can also choose
 from the following options:

 - AgoraVideoBitrateStandard:

     - The standard bitrate in [setVideoEncoderConfiguration]([AgoraRtcEngineKit
 setVideoEncoderConfiguration:]). - (Recommended) In a live broadcast, Agora recommends setting a
 larger bitrate to improve the video quality. When you choose AgoraVideoBitrateStandard, the bitrate
 value doubles in a live broadcast mode, and remains the same as in AgoraVideoProfile in a
 communication mode.

 - AgoraVideoBitrateCompatible:

     - The compatible bitrate in [setVideoEncoderConfiguration]([AgoraRtcEngineKit
 setVideoEncoderConfiguration:]). - The bitrate in both the live broadcast and communication modes
 remain the same as in AgoraVideoProfile.
 */
@property(assign, nonatomic) NSInteger bitrate;

@property(assign, nonatomic) NSInteger minBitrate;

/** Video orientation mode of the video: AgoraVideoOutputOrientationMode.
 */
@property(assign, nonatomic) AgoraVideoOutputOrientationMode orientationMode;

/** Video mirror mode of the video: AgoraVideoMirrorMode.
 */
@property(assign, nonatomic) AgoraVideoMirrorMode mirrorMode;

/** The video encoding degradation preference under limited bandwidth.

AgoraDegradationPreference:

* AgoraDegradationMaintainQuality(0): (Default) Degrades the frame rate to guarantee the video quality.
* AgoraDegradationMaintainFramerate(1): Degrades the video quality to guarantee the frame rate.
*/
@property (assign, nonatomic) AgoraDegradationPreference degradationPreference;

/** The video hardwareEncoding. */
@property (strong, nonatomic) AgoraAdvancedVideoOptions *_Nullable advancedVideoOptions;

/** Initializes and returns a newly allocated AgoraVideoEncoderConfiguration object with the
 specified video resolution.

 @param size Video resolution
 @param frameRate Video frame rate
 @param bitrate Video bitrate
 @param orientationMode AgoraVideoOutputOrientationMode
 @param mirrorMode AgoraVideoMirrorMode
 @return An initialized AgoraVideoEncoderConfiguration object
 */
- (instancetype _Nonnull)initWithSize:(CGSize)size
                            frameRate:(AgoraVideoFrameRate)frameRate
                              bitrate:(NSInteger)bitrate
                      orientationMode:(AgoraVideoOutputOrientationMode)orientationMode
                           mirrorMode:(AgoraVideoMirrorMode)mirrorMode NS_SWIFT_NAME(init(size:frameRate:bitrate:orientationMode:mirrorMode:));

/** Initializes and returns a newly allocated AgoraVideoEncoderConfiguration object with the
 specified video width and height.

 @param width Width of the video
 @param height Height of the video
 @param frameRate Video frame rate
 @param bitrate Video bitrate
 @param orientationMode AgoraVideoOutputOrientationMode
 @param mirrorMode AgoraVideoMirrorMode
 @return An initialized AgoraVideoEncoderConfiguration object
 */
- (instancetype _Nonnull)initWithWidth:(NSInteger)width
                                height:(NSInteger)height
                             frameRate:(AgoraVideoFrameRate)frameRate
                               bitrate:(NSInteger)bitrate
                       orientationMode:(AgoraVideoOutputOrientationMode)orientationMode
                            mirrorMode:(AgoraVideoMirrorMode)mirrorMode NS_SWIFT_NAME(init(width:height:frameRate:bitrate:orientationMode:mirrorMode:));
@end

/** A class for providing user-specific audio/video transcoding settings.
 */
__attribute__((visibility("default"))) @interface AgoraLiveTranscodingUser : NSObject
/** User ID.
 */
@property(assign, nonatomic) NSUInteger uid;
/** Position and size of the video frame.
 */
@property(assign, nonatomic) CGRect rect;
/** Video frame layer number, in the range of 1 to 100.

 * 1: The video frame image is in the lowest layer (default)
 * 100: The video frame image is in the highest layer
 */
@property(assign, nonatomic) NSInteger zOrder;
/** Transparency of the video frame, between 0 and 1.0:

 * 0: Completely transparent
 * 1.0: Opaque (default)
 */
@property(assign, nonatomic) double alpha;
/** The audio channel of the sound. The default value is 0:

 * 0: (default) Supports dual channels at most, depending on the upstream of the broadcaster.
 * 1: The audio stream of the broadcaster is in the FL audio channel. If the upstream of the
 broadcaster uses dual-sound channel, only the left-sound channel will be used for streaming. * 2:
 The audio stream of the broadcaster is in the FC audio channel. If the upstream of the broadcaster
 uses dual-sound channel, only the left-sound channel will be used for streaming. * 3: The audio
 stream of the broadcaster is in the FR audio channel. If the upstream of the broadcaster uses
 dual-sound channel, only the left-sound channel will be used for streaming. * 4: The audio stream
 of the broadcaster is in the BL audio channel. If the upstream of the broadcaster uses dual-sound
 channel, only the left-sound channel will be used for streaming. * 5: The audio stream of the
 broadcaster is in the BR audio channel. If the upstream of the broadcaster uses dual-sound channel,
 only the left-sound channel will be used for streaming.
 */
@property(assign, nonatomic) NSInteger audioChannel;
@end

/** The configuration for advanced features of the RTMP or RTMPS streaming with transcoding.
 */
__attribute__((visibility("default"))) @interface AgoraLiveStreamAdvancedFeature : NSObject

/** The name of the advanced feature, including the following:

 - LBHQ: The advanced feature for high-quality video with a lower bitrate.
 - VEO: The advanced feature for the optimized video encoder.
*/
@property(copy, nonatomic) NSString* _Nullable featureName;

/** Whether to enable the advanced feature:

 - YES: Enable the advanced feature.
 - NO: (Default) Disable the advanced feature.
 */
@property(assign, nonatomic) BOOL opened;
@end

/** Watermark image properties.
 */
__attribute__((visibility("default"))) @interface AgoraImage : NSObject
/** URL address of the watermark on the broadcasting video
 */
@property(strong, nonatomic) NSURL *_Nonnull url;
/** Position and size of the watermark on the broadcasting video in CGRect
 */
@property(assign, nonatomic) CGRect rect;
/**
 * Order attribute for an ordering of overlapping two-dimensional objects.
*/
@property (assign, nonatomic) NSInteger zOrder;
/** The transparency level of the image.

 The value ranges between 0.0 and 1.0:

 * 0.0: Completely transparent.
 * 1.0: (Default) Opaque.
 */
@property(assign, nonatomic) double alpha;
@end

/** The options of the watermark image to be added.
 */
__attribute__((visibility("default"))) @interface WatermarkOptions : NSObject
/** Sets whether or not the watermark image is visible in the local video preview:

 - YES: (Default) The watermark image is visible in preview.
 - NO: The watermark image is not visible in preview.
 */
@property(assign, nonatomic) BOOL visibleInPreview;
/** The watermark position in landscape mode of *Rotate the Video*. This parameter contains the following members:

  - `x`: The horizontal offset of the watermark from the top-left corner.
  - `y`: The vertical offset of the watermark from the top-left corner.
  - `width`: The width (pixel) of the watermark region.
  - `height`: The height (pixel) of the watermark region.
 */
@property(assign, nonatomic) CGRect positionInLandscapeMode;
/** The watermark position in portrait mode of *Rotate the Video*. This parameter contains the following members:

  - `x`: The horizontal offset of the watermark from the top-left corner.
  - `y`: The vertical offset of the watermark from the top-left corner.
  - `width`: The width (pixel) of the watermark region.
  - `height`: The height (pixel) of the watermark region.
 */
@property(assign, nonatomic) CGRect positionInPortraitMode;
@end

/** A class for managing CDN transcoding.
 */
__attribute__((visibility("default"))) @interface AgoraLiveTranscoding : NSObject
/** The size of the video (width and height in pixels).

- When pushing video streams to the CDN, note the following:

  - The value range of the width is [64,1920]. If the value is less than 64,
  Agora server automatically adjusts it to 64; if the value is greater than
  1920, Agora server automatically adjusts it to 1920.
  - The value range of the height is [64,1080]. If the value is less than 64,
  Agora server automatically adjusts it to 64; if the value is greater than
  1080, Agora server automatically adjusts it to 1080.

- When pushing audio streams to the CDN, set the width and height as 0.
 */
@property(assign, nonatomic) CGSize size;
/** Bitrate of the CDN live output video stream.

The default value is 400 Kbps.

Set this parameter according to the Video Bitrate Table. If you set a bitrate beyond the proper range, the SDK automatically adapts it to a value within the range.
 */
@property(assign, nonatomic) NSInteger videoBitrate;
/** Frame rate of the CDN live output video stream.

The default value is 15 fps, and the value range is (0,30].

 @note The Agora server adjusts any value over 30 to 30.
 */
@property(assign, nonatomic) NSInteger videoFramerate;
/** Latency mode. **DEPRECATED** from v2.8.0

 * YES: Low latency with unassured quality.
 * NO:（Default）High latency with assured quality.
 */
@property(assign, nonatomic) BOOL lowLatency;
/** Video GOP in frames. The default value is 30 fps. */
@property(assign, nonatomic) NSInteger videoGop;
/** Video codec profile type

Set it as 66, 77, or 100 (default), see [AgoraVideoCodecProfileType](AgoraVideoCodecProfileType).

If you set this parameter to other values, Agora adjusts it to the default value of 100.
 */
@property(assign, nonatomic) AgoraVideoCodecProfileType videoCodecProfile;

/** The video codec type of the output video stream. See AgoraVideoCodecTypeForStream.

 @since v3.2.0
 */
@property(assign, nonatomic) AgoraVideoCodecTypeForStream videoCodecType;

/** An AgoraLiveTranscodingUser object managing the user layout configuration in the CDN live stream. Agora supports a maximum of 17 transcoding users in a CDN live stream channel. See AgoraLiveTranscodingUser.
 */
@property(copy, nonatomic) NSArray<AgoraLiveTranscodingUser*>* _Nullable transcodingUsers;

/** Reserved property. Extra user-defined information to send SEI for the H.264/H.265 video stream to the CDN live client. Maximum length: 4096 bytes. For more information on SEI, see [SEI-related questions](https://docs.agora.io/en/faq/sei).
 */
@property(copy, nonatomic) NSString* _Nullable transcodingExtraInfo;

/**
 * add few watermarks
 */
@property(copy, nonatomic) NSArray<AgoraImage*>* _Nullable watermarkArray;

/**
 * add few backgroundImage
 */
@property(copy, nonatomic) NSArray<AgoraImage*>* _Nullable backgroundImageArray;

/** The background color in RGB hex.

Value only. Do not include a preceding #. For example, 0xFFB6C1 (light pink). The default value is 0x000000 (black).

COLOR_CLASS is a general name for the type:

* iOS: UIColor
* macOS: NSColor
 */
@property(strong, nonatomic) COLOR_CLASS* _Nullable backgroundColor;

/** Self-defined audio sample rate: AgoraAudioSampleRateType.
 */
@property(assign, nonatomic) AgoraAudioSampleRateType audioSampleRate;
/** Bitrate (Kbps) of the CDN live audio output stream. The default value is 48, and the highest value is 128.
 */
@property(assign, nonatomic) NSInteger audioBitrate;
/** The number of audio channels for the CDN live stream.

 Agora recommends choosing 1 (mono), or 2 (stereo) audio channels. Special players are required if you choose 3, 4, or 5.

 * 1: (Default) Mono
 * 2: Stereo
 * 3: Three audio channels
 * 4: Four audio channels
 * 5: Five audio channels
 */
@property(assign, nonatomic) NSInteger audioChannels;
/**
 Audio codec profile. See AgoraAudioCodecProfileType.

 The default value is AgoraAudioCodecProfileLCAAC(0).
 */
@property(assign, nonatomic) AgoraAudioCodecProfileType audioCodecProfile;

/** Creates a default transcoding object.

 @return Default AgoraLiveTranscoding object.
 */
+ (AgoraLiveTranscoding* _Nonnull)defaultTranscoding NS_SWIFT_NAME(default());

/** Adds a user displaying the video in CDN live.

 @param user The transcoding user. See AgoraLiveTranscodingUser.

 @return - 0: Success.
 - < 0: Failure.
 */
- (int)addUser:(AgoraLiveTranscodingUser* _Nonnull)user NS_SWIFT_NAME(add(_:));

/** Removes a user from CDN live.

 @param uid The user ID of the user to remove from CDN live.

 @return - 0: Success.
 - < 0: Failure.
 */
- (int)removeUser:(NSUInteger)uid NS_SWIFT_NAME(removeUser(_:));

/** Enables/Disables advanced features of the RTMP or RTMPS streaming with transcoding.

 @param featureName The name of the advanced feature, including the following:
 <li>LBHQ: The advanced feature for high-quality video with a lower bitrate.</li>
 <li>VEO: The advanced feature for the optimized video encoder.</li>
 @param opened Whether to enable the advanced feature:
 <li>YES: Enable the advanced feature.</li>
 <li>NO: (Default) Disable the advanced feature.</li>
 */
- (void)setAdvancedFeatures:(NSString* _Nonnull)featureName opened:(BOOL)opened NS_SWIFT_NAME(setAdvancedFeatures(_:opened:));

/** Checks whether advanced features of the RTMP or RTMPS streaming with transcoding are enabled.
 @return The name of each advanced feature and whether the advanced feature is enabled.
 */
- (NSArray<AgoraLiveStreamAdvancedFeature*>* _Nullable)getAdvancedFeatures NS_SWIFT_NAME(getAdvancedFeatures());

@end

/** Live broadcast import stream configuration.
 */
__attribute__((visibility("default"))) @interface AgoraLiveInjectStreamConfig : NSObject
/** Size of the stream to be imported into the live broadcast. The default value is 0; same
 * size as the original stream.
 */
@property(assign, nonatomic) CGSize size;
/** Video GOP of the stream to be added into the broadcast. The default value is 30.
 */
@property(assign, nonatomic) NSInteger videoGop;
/** Video frame rate of the stream to be added into the broadcast. The default value is 15 fps.
 */
@property(assign, nonatomic) NSInteger videoFramerate;
/** Video bitrate of the stream to be added into the broadcast. The default value is 400 Kbps.
 */
@property(assign, nonatomic) NSInteger videoBitrate;

/** Audio sampling rate of the stream to be added into the broadcast. The default value is 48000.
 */
@property(assign, nonatomic) AgoraAudioSampleRateType audioSampleRate;
/** Audio bitrate of the stream to be added into the broadcast. The default value is 48 Kbps.
 */
@property(assign, nonatomic) NSInteger audioBitrate;
/** Audio channels to be added into the broadcast. The default value is 1.
 */
@property(assign, nonatomic) NSInteger audioChannels;

/** Create a default stream config

 @return default stream config
 */
+ (AgoraLiveInjectStreamConfig *_Nonnull)defaultConfig NS_SWIFT_NAME(defaultConfig());
@end

    __deprecated

    /** AgoraRtcVideoCompositingRegion array.
     */
    __attribute__((visibility("default"))) @interface AgoraRtcVideoCompositingRegion
    : NSObject
      /** User ID of the user with the video to be displayed in the region.
       */
      @property(assign, nonatomic) NSUInteger uid;
/** Horizontal position of the region on the screen (0.0 to 1.0).
 */
@property(assign, nonatomic) CGFloat x;
/** Vertical position of the region on the screen (0.0 to 1.0).
 */
@property(assign, nonatomic) CGFloat y;
/** Actual width of the region (0.0 to 1.0).
 */
@property(assign, nonatomic) CGFloat width;
/** Actual height of the region (0.0 to 1.0).
 */
@property(assign, nonatomic) CGFloat height;
/** 0 means the region is at the bottom, and 100 means the region is at the top.
 */
@property(assign, nonatomic) NSInteger zOrder;
/** 0 means the region is transparent, and 1 means the region is opaque. The default value is 1.0.
 */
@property(assign, nonatomic) CGFloat alpha;
/** Render mode: AgoraVideoRenderMode
 */
@property(assign, nonatomic) AgoraVideoRenderMode renderMode;
@end

    __deprecated
    /** Rtc video compositing layout.
     */
    __attribute__((visibility("default"))) @interface AgoraRtcVideoCompositingLayout
    : NSObject
      /** Width of the entire canvas (display window or screen).
       */
      @property(assign, nonatomic) NSInteger canvasWidth;
/** Height of the entire canvas (display window or screen).
 */
@property(assign, nonatomic) NSInteger canvasHeight;
/** Enter any of the 6-digit symbols defined in RGB. For example, "#c0c0c0"
 */
@property(copy, nonatomic) NSString *_Nullable backgroundColor;
/** AgoraRtcVideoCompositingRegion array.
 */
@property(copy, nonatomic) NSArray<AgoraRtcVideoCompositingRegion *> *_Nullable regions;
/** Application defined data.
 */
@property(copy, nonatomic) NSString *_Nullable appData;
@end

    /**
     @deprecated

     Sets whether the current host is the RTMP stream owner.
     */
    __deprecated __attribute__((visibility("default"))) @interface AgoraPublisherConfiguration
    : NSObject
      /**
       - YES: The current host is the RTMP stream owner and the push-stream configuration is enabled
       (default). - NO: The current host is not the RTMP stream owner and the push-stream
       configuration is disabled.
       */
      @property(assign, nonatomic) BOOL owner;

/** Width of the output data stream set for CDN Live. 360 is the default value
 */
@property(assign, nonatomic) NSInteger width;
/** Height of the output data stream set for CDN Live. 640 is the default value
 */
@property(assign, nonatomic) NSInteger height;
/** Frame rate of the output data stream set for CDN Live. 15 fps is the default value
 */
@property(assign, nonatomic) NSInteger framerate;
/** Bitrate of the output data stream set for CDN Live. 500 kbit/s is the default value
 */
@property(assign, nonatomic) NSInteger bitrate;
/** Audio sample rate of the output data stream set for CDN Live. The default value is 48000.
 */
@property(assign, nonatomic) NSInteger audiosamplerate;
/** Audio bitrate of the output data stream set for CDN Live.  The default value is 48.
 */
@property(assign, nonatomic) NSInteger audiobitrate;
/** Audio channels of the output data stream set for CDN Live. The default value is 1.
 */
@property(assign, nonatomic) NSInteger audiochannels;

/**

* 0: Tile Horizontally
* 1: Layered Windows
* 2: Tile Vertically
 */
@property(assign, nonatomic) NSInteger defaultLayout;
/** Video stream lifecycle of CDN Live: AgoraRtmpStreamLifeCycle
 */
@property(assign, nonatomic) AgoraRtmpStreamLifeCycle lifeCycle;

/** Width of the stream to be injected. Set it as 0.
 */
@property(assign, nonatomic) NSInteger injectStreamWidth;

/** Height of the stream to be injected. Set it as 0.
 */
@property(assign, nonatomic) NSInteger injectStreamHeight;

/** Address of the stream to be injected to the channel.
 */
@property(copy, nonatomic) NSString *_Nullable injectStreamUrl;

/** The push-stream address for the picture-in-picture layouts. The default value is *NULL*.
 */
@property(copy, nonatomic) NSString *_Nullable publishUrl;

/** The push-stream address of the original stream which does not require picture-blending. The
 * default value is NULL.
 */
@property(copy, nonatomic) NSString *_Nullable rawStreamUrl;

/** Reserved field. The default value is NULL.
 */
@property(copy, nonatomic) NSString *_Nullable extraInfo;

/** Check if configuration is validate
 */
- (BOOL)validate NS_SWIFT_NAME(validate());

- (NSString * _Nullable)toJsonString NS_SWIFT_NAME(toJsonString());
@end

#if (!(TARGET_OS_IPHONE) && (TARGET_OS_MAC))

/** AgoraRtcDeviceInfo array.
 */
__attribute__((visibility("default"))) @interface AgoraRtcDeviceInfo : NSObject
@property (assign, nonatomic) int __deprecated index;

/** AgoraMediaDeviceType
 */
@property(assign, nonatomic) AgoraMediaDeviceType type;

/** Device ID.
 */
@property(copy, nonatomic) NSString *_Nullable deviceId;

/** Device name.
 */
@property(copy, nonatomic) NSString *_Nullable deviceName;
@end
#endif

/** Properties of the AgoraVideoFrame object.
 */
__attribute__((visibility("default"))) @interface AgoraVideoFrame : NSObject
/** Video format:

 * - 1: I420
 * - 2: BGRA
 * - 3: NV21
 * - 4: RGBA
 * - 5: IMC2
 * - 7: ARGB
 * - 8: NV12
 * - 12: iOS texture (CVPixelBufferRef)
 * - 13: Still Image (UIImage for iPhone, NSImage for Mac)
 */
@property(assign, nonatomic) NSInteger format;

/** Timestamp of the incoming video frame (ms). An incorrect timestamp will result in frame loss or
 * unsynchronized audio and video.
 */
@property(assign, nonatomic) CMTime time;  // Time for this frame.

/**
 @deprecated Use strideInPixels instead.
 */
@property(assign, nonatomic) int stride DEPRECATED_MSG_ATTRIBUTE("use strideInPixels instead");

/** Line spacing of the incoming video frame, which must be in pixels instead of bytes. For
 * textures, it is the width of the texture.
 */
@property(assign, nonatomic) int strideInPixels;  // Number of pixels between two consecutive rows.
                                                  // Note: in pixel, not byte. Not used for iOS
                                                  // textures.

/** Height of the incoming video frame.
 */
@property(assign, nonatomic) int height;  // Number of rows of pixels. Not used for iOS textures.

/** CVPixelBuffer
 */
@property(assign, nonatomic) CVPixelBufferRef _Nullable textureBuf;

/** Still Image (UIImage for iPhone, NSImage for Mac)
 */
@property(strong, nonatomic) IMAGE_CLASS * _Nullable image;

/** Raw data buffer.
 */
@property(strong, nonatomic) NSData *_Nullable dataBuf;  // Raw data buffer. Not used for iOS textures.

/** (Optional) Specifies the number of pixels trimmed from the left, which is set as 0 by default.
 */
@property(assign, nonatomic) int cropLeft;  // Number of pixels to crop on the left boundary.
/** (Optional) Specifies the number of pixels trimmed from the top, which is set as 0 by default.
 */
@property(assign, nonatomic) int cropTop;  // Number of pixels to crop on the top boundary.
/** (Optional) Specifies the number of pixels trimmed from the right, which is set as 0 by default.
 */
@property(assign, nonatomic) int cropRight;  // Number of pixels to crop on the right boundary.
/** (Optional) Specifies the number of pixels trimmed from the bottom, which is set as 0 by default.
 */
@property(assign, nonatomic) int cropBottom;  // Number of pixels to crop on the bottom boundary.
/** (Optional) Specifies whether to rotate the incoming video group. Optional values: 0, 90, 180, or
 * 270 clockwise. Set as 0 by default.
 */
@property(assign, nonatomic) int rotation;  // 0, 90, 180, 270. See document for rotation calculation.
/* Note
 * 1. strideInPixels
 *    Stride is in pixels, not bytes
 * 2. About the frame width and height
 *    No field is defined for the width. However, it can be deduced by:
 *       croppedWidth = (strideInPixels - cropLeft - cropRight)
 *    And
 *       croppedHeight = (height - cropTop - cropBottom)
 * 3. About crop
 *    _________________________________________________________________.....
 *    |                        ^                                      |  ^
 *    |                        |                                      |  |
 *    |                     cropTop                                   |  |
 *    |                        |                                      |  |
 *    |                        v                                      |  |
 *    |                ________________________________               |  |
 *    |                |                              |               |  |
 *    |                |                              |               |  |
 *    |<-- cropLeft -->|          valid region        |<- cropRight ->|
 *    |                |                              |               | height
 *    |                |                              |               |
 *    |                |_____________________________ |               |  |
 *    |                        ^                                      |  |
 *    |                        |                                      |  |
 *    |                     cropBottom                                |  |
 *    |                        |                                      |  |
 *    |                        v                                      |  v
 *    _________________________________________________________________......
 *    |                                                               |
 *    |<---------------- strideInPixels ----------------------------->|
 *
 *    If your buffer contains garbage data, you can crop them. For example, the frame size is
 *    360 x 640, often the buffer stride is 368, that is, there extra 8 pixels on the
 *    right are for padding, and should be removed. In this case, you can set:
 *    strideInPixels = 368;
 *    height = 640;
 *    cropRight = 8;
 *    // cropLeft, cropTop, cropBottom are set to a default of 0
 */
@end

__attribute__((visibility("default"))) @interface AgoraLogConfig: NSObject
/** The absolute path of log files.

 Ensure that the directory for the log
 files exists and is writable. The default file path is as follows:

 - iOS: `App Sandbox/Library/caches/agorasdk.log`
 - macOS:
   - Sandbox enabled: `App Sandbox/Library/Logs/agorasdk.log`, such as
   `/Users/<username>/Library/Containers/<App Bundle Identifier>/Data/Library/Logs/agorasdk.log`
   - Sandbox disabled: `�Library/Logs/agorasdk.log`
 */
@property (copy, nonatomic) NSString * _Nullable filePath;
/** The size (KB) of a log file.

 The default value is 1024 KB. If you set
 this parameter to 1024 KB, the SDK outputs at most 5 MB log files; if
 you set it to less than 1024 KB, the setting is invalid, and the maximum
 size of a log file is still 1024 KB.
 */
@property (assign, nonatomic) NSInteger fileSizeInKB;
/** The output log level of the SDK. See details in AgoraLogLevel.

 For example, if you set the log level to `AgoraLogLevelWarn`, the SDK outputs the logs
 within levels `AgoraLogLevelFatal`, `AgoraLogLevelError`, and `AgoraLogLevelWarn`.
 */
@property (assign, nonatomic) AgoraLogLevel level;
@end

/**
 * The config of AgoraRtcEngine.
 */
__attribute__((visibility("default"))) @interface AgoraRtcEngineConfig: NSObject

 /**
  * The App ID issued to the developers by Agora. Apply for a new one from Agora if it is missing from
  * your kit.
  */
 @property (copy, nonatomic) NSString * _Nullable appId;

/**
 * The channel profile. See #AgoraChannelProfile.
 */
 @property (assign, nonatomic) AgoraChannelProfile channelProfile;

  /**
   * The license used for verification when connectting channel. Charge according to the license
   */
  @property (copy, nonatomic) NSString * _Nullable license;

 /**
  * The audio application scenario. See #AgoraAudioScenario.
  *
  * @note Agora recommends the following scenarios:
  * - `AgoraAudioScenarioDefault = 0`
  * - `AgoraAudioScenarioGameStreaming = 3`
  */
 @property (assign, nonatomic) AgoraAudioScenario audioScenario;
 /**
  * The region for connection. This advanced feature applies to scenarios that
  * have regional restrictions.
  *
  * For the regions that Agora supports, see \ref AgoraAreaCodeType.
  *
  * After specifying the region, the SDK connects to the Agora servers within
  * that region.
  */
 @property (assign, nonatomic) AgoraAreaCodeType areaCode;
 @property (strong, nonatomic) AgoraLogConfig * _Nullable logConfig;
 /**
   * Thread priority for SDK common threads
   *
   * See \ref AgoraThreadPriorityType.
   */
 @property (assign, nonatomic) AgoraThreadPriorityType threadPriority;
 @property (weak, nonatomic) id<AgoraMediaFilterEventDelegate> _Nullable eventDelegate;
 /**
  * Determines whether to enable domain limit.
  * - `true`: only connect to servers that already parsed by DNS
  * - `false`: (Default) connect to servers with no limit
  */
 @property (assign, nonatomic) BOOL domainLimit;
@end

/**
 * The class of AgoraAudioFrame.
 */
__attribute__((visibility("default"))) @interface AgoraAudioFrame : NSObject
/** The number of samples per channel.
 */
@property(assign, nonatomic) NSInteger samplesPerChannel;
/** The number of bytes per audio sample. For example, each PCM audio sample
 usually takes up 16 bits (2 bytes).
 */
@property(assign, nonatomic) NSInteger bytesPerSample;
/** The number of audio channels. If the channel uses stereo, the data is
 interleaved.

- 1: Mono.
- 2: Stereo.
 */
@property(assign, nonatomic) NSInteger channels;
/** The sample rate.
 */
@property(assign, nonatomic) NSInteger samplesPerSec;
/** The buffer of the sample audio data. When the audio frame uses a stereo
 channel, the data buffer is interleaved. The size of the data buffer is as
 follows: `buffer` = `samplesPerChannel` × `channels` × `bytesPerSample`.
 */
@property(assign, nonatomic) void* _Nullable buffer;
/** The timestamp of the external audio frame. You can use this parameter for
 the following purposes:

- Restore the order of the captured audio frame.
- Synchronize audio and video frames in video-related scenarios, including
where external video sources are used.
 */
@property(assign, nonatomic) int64_t renderTimeMs;
/** Reserved for future use.
 */
@property(assign, nonatomic) NSInteger avSyncType;
@end

/** The AgoraAudioParams interface
 You can pass the `AgoraAudioParams` object in the return value of the following
 callbacks to set the audio data format for the corresponding callbacks:

 - [getRecordAudioParams]([AgoraAudioFrameDelegate getRecordAudioParams]):
 Sets the audio recording format for the
 [onRecordAudioFrame]([AgoraAudioFrameDelegate onRecordAudioFrame:])
 callback.
 - [getPlaybackAudioParams]([AgoraAudioFrameDelegate getPlaybackAudioParams]):
 Sets the audio playback format for the
 [onPlaybackAudioFrame]([AgoraAudioFrameDelegate onPlaybackAudioFrame:])
 callback.
 - [getMixedAudioParams]([AgoraAudioFrameDelegate getMixedAudioParams]):
 Sets the audio mixing format for the
 [onMixedAudioFrame]([AgoraAudioFrameDelegate onMixedAudioFrame:]) callback.

 @note The SDK calculates the sample interval according to the
 `samplesPerCall`, `sampleRate`, and `channel` values in the
 `AgoraAudioParams` interface and triggers the following callbacks at the
 calculated sample interval:

 - [onRecordAudioFrame]([AgoraAudioFrameDelegate onRecordAudioFrame:])
 - [onPlaybackAudioFrame]([AgoraAudioFrameDelegate onPlaybackAudioFrame:])
 - [onMixedAudioFrame]([AgoraAudioFrameDelegate onMixedAudioFrame:])

 Sample interval (seconds) = `samplesPerCall`/(`sampleRate` × `channel`).
 Ensure that the value of sample interval is equal to or greater than 0.01.
 */
__attribute__((visibility("default"))) @interface AgoraAudioParams : NSObject

/** The audio sample rate (Hz), which can be set as one of the following values:

 - `8000`
 - `16000` (Default)
 - `32000`
 - `44100 `
 - `48000`
 */
@property (assign, nonatomic) NSInteger sampleRate;

/* The number of audio channels, which can be set as either of the following values:

 - `1`: Mono (Default)
 - `2`: Stereo
 */
@property (assign, nonatomic) NSInteger channel;

/* The use mode of the audio data. See AgoraAudioRawFrameOperationMode.
 */
@property (assign, nonatomic) AgoraAudioRawFrameOperationMode mode;

/** The number of samples. For example, set it as 1024 for RTMP or RTMPS
 streaming.
 */
@property (assign, nonatomic) NSInteger samplesPerCall;

@end

/**
 * The class of AgoraEncodedAudioFrameInfo.
 */
__attribute__((visibility("default"))) @interface AgoraEncodedAudioFrameInfo: NSObject
 @property (assign, nonatomic) NSInteger samplesPerChannel;
 @property (assign, nonatomic) NSInteger channels;
 @property (assign, nonatomic) NSInteger samplesPerSec;
 @property (assign, nonatomic) AgoraAudioCodecType codecType;
@end

/**
 * The collections of uplink network info.
 */
__attribute__((visibility("default"))) @interface AgoraUplinkNetworkInfo : NSObject
/**
* The target video encoder bitrate bps.
*/
@property(nonatomic, assign) int videoEncoderTargetBitrateBps;
@end

/**
 * The collections of downlink network info.
 */
__attribute__((visibility("default"))) @interface AgoraDownlinkNetworkInfo : NSObject
/**
* The lastmile buffer delay time in ms.
*/
@property(nonatomic, assign) int lastmileBufferDelayTimeMs;
/**
* The downlink bandwidth estimation bitrate bps.
*/
@property(nonatomic, assign) int bandwidthEstimationBps;
@end

/**
 * The leave channel options.
 */
__attribute__((visibility("default"))) @interface AgoraLeaveChannelOptions : NSObject
/**
 * Determines whether to stop playing and mixing the music file when leave channel.
 * - true: (Default) Stop playing and mixing the music file.
 * - false: Do not stop playing and mixing the music file.
 */
@property(nonatomic, assign) BOOL stopAudioMixing;

/**
 * Determines whether to stop all music effects when leave channel.
 * - true: (Default) Stop all music effects.
 * - false: Do not stop the music effect.
 */
@property(nonatomic, assign) BOOL stopAllEffect;

/**
 * Determines whether to stop microphone recording when leave channel.
 * - true: (Default) Stop microphone recording.
 * - false: Do not stop microphone recording.
 */
@property(nonatomic, assign) BOOL stopMicrophoneRecording;

@end

__attribute__((visibility("default"))) @interface AgoraOutputVideoFrame : NSObject

/** Video format:
 * - 1: I420
 * - 2: BGRA
 * - 3: NV21
 * - 4: RGBA
 * - 5: IMC2
 * - 7: ARGB
 * - 8: NV12
 * - 12: iOS texture NV12 (CVPixelBufferRef)
 * - 13: iOS texture I420 (CVPixelBufferRef)
 * - 14: iOS texture BGRA (CVPixelBufferRef)
 */
@property (nonatomic, assign) NSInteger type;
/**
 * The width of the Video frame.
 */
@property (nonatomic, assign) int width;
/**
 * The height of the video frame.
 */
@property (nonatomic, assign) int height;
/**
 * The line span of Y buffer in the YUV data.
 */
@property (nonatomic, assign) int yStride;
/**
 * The line span of U buffer in the YUV data.
 */
@property (nonatomic, assign) int uStride;
/**
 * The line span of V buffer in the YUV data.
 */
@property (nonatomic, assign) int vStride;
/**
 * The pointer to the Y buffer in the YUV data.
 */
@property (nonatomic, assign) uint8_t* _Nullable yBuffer;
/**
 * The pointer to the U buffer in the YUV data.
 */
@property (nonatomic, assign) uint8_t* _Nullable uBuffer;
/**
 * The pointer to the V buffer in the YUV data.
 */
@property (nonatomic, assign) uint8_t* _Nullable vBuffer;
/**
 * The clockwise rotation information of this frame. You can set it as 0, 90, 180 or 270.
 */
@property (nonatomic, assign) int rotation;
/**
 * The timestamp to render the video stream. Use this parameter for audio-video synchronization when
 * rendering the video.
 *
 * @note This parameter is for rendering the video, not capturing the video.
 */
@property (nonatomic, assign) int64_t renderTimeMs;
/**
 * The type of audio-video synchronization.
 */
@property (nonatomic, assign) int avSyncType;

/** CVPixelBuffer
 */
@property(assign, nonatomic) CVPixelBufferRef _Nullable pixelBuffer;
/**
 *  Portrait Segmentation meta buffer, dimension of which is the same as AgoraOutputVideoFrame.
 *  Pixl value is between 0-255, 0 represents totally background, 255 represents totally foreground.
 */
@property (nonatomic, assign) uint8_t* _Nullable alphaBuffer;

@end

/** Configurations of built-in encryption schemas.
 */
__attribute__((visibility("default"))) @interface AgoraEncryptionConfig: NSObject

 /** Encryption mode. The default encryption mode is `AgoraEncryptionModeAES128GCM2`. See AgoraEncryptionMode.
  */
 @property (assign, nonatomic) AgoraEncryptionMode encryptionMode;

 /** Encryption key in string type.

 **Note**

 If you do not set an encryption key or set it as `nil`, you cannot use the built-in encryption, and the SDK returns `-2` (`AgoraErrorCodeInvalidArgument`).
  */
 @property (copy, nonatomic) NSString * _Nullable encryptionKey;
 @property (strong, nonatomic) NSData * _Nullable encryptionKdfSalt;
 @end

/** AgoraUserInfo.
 */
__attribute__((visibility("default"))) @interface AgoraUserInfo: NSObject

 /** The user ID
  */
@property(assign, nonatomic) NSUInteger uid;

 /** The user account
  */
 @property (copy, nonatomic) NSString * _Nullable userAccount;
 @end

/** Client role options for the AgoraRtcEngineKit instance.
 */
__attribute__((visibility("default"))) @interface AgoraClientRoleOptions: NSObject

/** Audicnce latency level. The default level is `AgoraAudienceLatencyLevelUltraLowLatency`. See AgoraAudienceLatencyLevelType.
  */
@property (assign, nonatomic) AgoraAudienceLatencyLevelType audienceLatencyLevel;

@end

/** The configuration of camera capturer.
 */
__attribute__((visibility("default"))) @interface AgoraCameraCapturerConfiguration: NSObject
/** The camera direction. See AgoraCameraDirection:

 - AgoraCameraDirectionRear: The rear camera.
 - AgoraCameraDirectionFront: The front camera.
 */
#if TARGET_OS_IOS
@property (assign, nonatomic) AgoraCameraDirection cameraDirection;
#elif TARGET_OS_MAC
/**
 *The device ID of the playback device.
 */
@property (copy, nonatomic) NSString * _Nullable deviceId;
#endif

/**
 * The dimensions of camera capture.
 */
@property(assign, nonatomic) CGSize dimensions;

/** Frame rate of the camera capture.
 */
@property(assign, nonatomic) int frameRate;

/** Follow the dimension ratio of setVideoEncoderConfiguration
 * - YES: (Default) follow the dimension ratio.
 * - NO: Do not follow the dimension ratio.
 */
@property(assign, nonatomic) BOOL followEncodeDimensionRatio;

@end

/** the configuration of datastream.
 */
__attribute__((visibility("default"))) @interface AgoraDataStreamConfig: NSObject

@property (assign, nonatomic) BOOL ordered;

@property (assign, nonatomic) BOOL syncWithAudio;
@end


/** The definition of AgoraChannelMediaRelayInfo.
 */
__attribute__((visibility("default"))) @interface AgoraChannelMediaRelayInfo: NSObject
/** The token that enables the user to join the channel.
 */
@property (copy, nonatomic) NSString * _Nullable token;
/** The channel name.
 */
@property (copy, nonatomic) NSString * _Nullable channelName;
/** The user ID.
 */
@property (assign, nonatomic) NSUInteger uid;
/** Initializes the AgoraChannelMediaRelayInfo object

 @param token The token that enables the user to join the channel.
 */
- (instancetype _Nonnull)initWithToken:(NSString *_Nullable)token NS_SWIFT_NAME(init(token:));
@end

/** The definition of AgoraChannelMediaRelayConfiguration.
 */
__attribute__((visibility("default"))) @interface AgoraChannelMediaRelayConfiguration: NSObject
/** The information of the destination channel: AgoraChannelMediaRelayInfo. It contains the following members:

 - `channelName`: The name of the destination channel.
 - `uid`: ID of the broadcaster in the destination channel. The value ranges from 0 to (2<sup>32</sup>-1). To avoid UID conflicts, this `uid` must be different from any other UIDs in the destination channel. The default value is 0, which means the SDK generates a random UID.
 - `token`: The token for joining the destination channel. It is generated with the `channelName` and `uid` you set in `destinationInfos`.

 - If you have not enabled the App Certificate, set this parameter as the default value `nil`, which means the SDK applies the App ID.
 - If you have enabled the App Certificate, you must use the `token` generated with the `channelName` and `uid`.
 */
@property (strong, nonatomic, readonly) NSDictionary<NSString *, AgoraChannelMediaRelayInfo *> *_Nullable destinationInfos;
/** The information of the source channel: AgoraChannelMediaRelayInfo. It contains the following members:

 - `channelName`: The name of the source channel. The default value is `nil`, which means the SDK applies the name of the current channel.
 - `uid`: ID of the broadcaster whose media stream you want to relay. The default value is 0, which means the SDK generates a random UID. You must set it as 0.
 - `token`: The token for joining the source channel. It is generated with the `channelName` and `uid` you set in `sourceInfo`.

 - If you have not enabled the App Certificate, set this parameter as the default value `nil`, which means the SDK applies the App ID.
 - If you have enabled the App Certificate, you must use the `token` generated with the `channelName` and `uid`, and the `uid` must be set as 0.
 */
@property (strong, nonatomic) AgoraChannelMediaRelayInfo *_Nonnull sourceInfo;
/** Sets the information of the destination channel.

 If you want to relay the media stream to multiple channels, call this method as many times (at most four).

 @param destinationInfo The information of the destination channel: AgoraChannelMediaRelayInfo. It contains the following members:

 - `channelName`: The name of the destination channel.
 - `uid`: ID of the broadcaster in the destination channel. The value ranges from 0 to (2<sup>32</sup>-1). To avoid UID conflicts, this `uid` must be different from any other UIDs in the destination channel. The default value is 0, which means the SDK generates a random UID.
 - `token`: The token for joining the destination channel. It is generated with the `channelName` and `uid` you set in `destinationInfo`.

 - If you have not enabled the App Certificate, set this parameter as the default value `nil`, which means the SDK applies the App ID.
 - If you have enabled the App Certificate, you must use the `token` generated with the `channelName` and `uid`.

 @param channelName The name of the destination channel. Ensure that the value of this parameter is the same as that of the `channelName` member in `destinationInfo`.

 @return - YES: Success.
 - NO: Failure.
 */
- (BOOL)setDestinationInfo:(AgoraChannelMediaRelayInfo *_Nonnull)destinationInfo forChannelName:(NSString *_Nonnull)channelName NS_SWIFT_NAME(setDestinationInfo(_:forChannelName:));
/** Removes the destination channel.

 @param channelName The name of the destination channel.

 @return - YES: Success.
 - NO: Failure.
 */
- (BOOL)removeDestinationInfoForChannelName:(NSString *_Nonnull)channelName NS_SWIFT_NAME(removeDestinationInfo(forChannelName:));
@end


/** The image enhancement options in [setBeautyEffectOptions]([AgoraRtcEngineKit setBeautyEffectOptions:options:]). */
__attribute__((visibility("default"))) @interface AgoraBeautyOptions : NSObject

/** The lightening contrast level

[AgoraLighteningContrastLevel](AgoraLighteningContrastLevel), used with the lighteningLevel property:

- 0: Low contrast level.
- 1: (Default) Normal contrast level.
- 2: High contrast level.
*/
@property(nonatomic, assign) AgoraLighteningContrastLevel lighteningContrastLevel;

/** The brightness level.

The default value is 0.7. The value ranges from 0.0 (original) to 1.0.
 */
@property(nonatomic, assign) float lighteningLevel;

/** The smoothness level.

The default value is 0.5. The value ranges from 0.0 (original) to 1.0. This parameter is usually used to remove blemishes.
 */
@property(nonatomic, assign) float smoothnessLevel;

/** The redness level.

The default value is 0.1. The value ranges from 0.0 (original) to 1.0. This parameter adjusts the red saturation level.
*/
@property(nonatomic, assign) float rednessLevel;

/** The sharpness level.

The default value is 0.1. The value ranges from 0.0 (original) to 1.0.
*/
@property(nonatomic, assign) float sharpnessLevel;

@end

/**
 The video noise reduction options.

 **Since** v3.6.2
 */
__attribute__((visibility("default"))) @interface AgoraVideoDenoiserOptions : NSObject

/** The video noise reduction mode. See AgoraVideoDenoiserMode.
 */
@property(nonatomic, assign) AgoraVideoDenoiserMode mode;

/** The video noise reduction level. See AgoraVideoDenoiserLevel.
 */
@property(nonatomic, assign) AgoraVideoDenoiserLevel level;

@end

/**
 The video noise reduction options.

 **Since** v3.6.2
 */
__attribute__((visibility("default"))) @interface AgoraLowlightEnhanceOptions : NSObject

/** The low-light enhancement mode. See AgoraLowlightEnhanceMode. */
@property(nonatomic, assign) AgoraLowlightEnhanceMode mode;

/** The low-light enhancement level. See AgoraLowlightEnhanceLevel. */
@property(nonatomic, assign) AgoraLowlightEnhanceLevel level;

@end

/**
 The color enhancement options.

 **Since** v3.6.2
 */
__attribute__((visibility("default"))) @interface AgoraColorEnhanceOptions : NSObject

/** The level of color enhancement. The value range is [0.0,1.0]. `0.0` is the default value, which means no color enhancement is applied to the video. The higher the value, the higher the level of color enhancement. */
@property(nonatomic, assign) float strengthLevel;

/** The level of skin tone protection. The value range is [0.0,1.0]. `0.0` means no skin tone protection. The higher the value, the higher the level of skin tone protection. The default value is `1.0.` When the level of color enhancement is higher, the portrait skin tone can be significantly distorted, so you need to set the level of skin tone protection; when the level of skin tone protection is higher, the color enhancement effect can be slightly reduced. Therefore, to get the best color enhancement effect, Agora recommends that you adjust strengthLevel and skinProtectLevel to get the most appropriate values. */
@property(nonatomic, assign) float skinProtectLevel;

@end


/** The custom background image.
 */
__attribute__((visibility("default"))) @interface AgoraVirtualBackgroundSource : NSObject

/** The type of the custom background image. See AgoraVirtualBackgroundSourceType.
 */
@property(nonatomic, assign) AgoraVirtualBackgroundSourceType backgroundSourceType NS_SWIFT_NAME(backgroundSourceType);

/** The color of the custom background image. The format is a hexadecimal
 integer defined by RGB, without the # sign, such as `0xFFB6C1` for light pink.
 The default value is `0xFFFFFF`, which signifies white. The value range is
 [0x000000,0xFFFFFF]. If the value is invalid, the SDK replaces the original
 background image with a white background image.
 <p><b>Note</b>: This parameter takes effect only when the type of the custom
 background image is <code>AgoraVirtualBackgroundColor</code>.</p>
 */
@property(nonatomic, assign) NSUInteger color NS_SWIFT_NAME(color);

/** The local absolute path of the custom background image. PNG and JPG formats
 are supported. If the path is invalid, the SDK replaces the original
 background image with a white background image.
 <p><b>Note</b>: This parameter takes effect only when the type of the custom
 background image is <code>AgoraVirtualBackgroundImg</code>.</p>
 */
@property(nonatomic, copy) NSString* _Nullable source NS_SWIFT_NAME(source);

/** Background blur degree, for example: 1 2 3 */
@property(nonatomic, assign) AgoraBlurDegree blurDegree;

@end

/** The custom  green capacity.
 */
__attribute__((visibility("default"))) @interface AgoraSegmentationProperty: NSObject

@property(nonatomic, assign) SegModelType modelType;

@property(nonatomic, assign) float greenCapacity;

@end


/** The definition of AgoraTranscodingVideoStream.
 */
__attribute__((visibility("default"))) @interface AgoraTranscodingVideoStream: NSObject
/**
 * Source type of video stream.
 */
@property (assign, nonatomic) AgoraVideoSourceType sourceType;
/**
 * Remote user uid if sourceType is AgoraMediaSourceTypeRemote.
 */
@property (assign, nonatomic) NSUInteger remoteUserUid;
/**
 * RTC image if sourceType is AgoraMediaSourceTypeRtcImage.
 */
@property (copy, nonatomic) NSString * _Nullable imageUrl;
/**
 * MediaPlayer id if sourceType is AgoraMediaSourceTypeMediaPlayer.
 */
@property(assign, nonatomic) NSUInteger mediaPlayerId;
/**
 * Position and size of the video frame.
 */
@property (assign, nonatomic) CGRect rect;
/**
 * The layer of the video frame that ranges from 1 to 100:
   - 1: (Default) The lowest layer.
   - 100: The highest layer.
 */
@property (assign, nonatomic) NSInteger zOrder;
/**
 * The transparency of the video frame.
 */
@property(assign, nonatomic) double alpha;
/**
 * Mirror of the source video frame (only valid for camera streams)
 */
@property(assign, nonatomic) BOOL mirror;

@end

/** The definition of AgoraLocalTranscoderConfiguration.
 */
__attribute__((visibility("default"))) @interface AgoraLocalTranscoderConfiguration: NSObject
/**
 * The video stream layout configuration in the transcoder.
 */
@property(copy, nonatomic) NSArray<AgoraTranscodingVideoStream *> *_Nullable videoInputStreams;
/**
 * The video encoder configuration of transcoded video.
 */
@property (strong, nonatomic) AgoraVideoEncoderConfiguration *_Nonnull videoOutputConfiguration;

/**
 * Whether to use the timestamp when the primary camera captures the video frame as the timestamp of the mixed video frame.
 * - true: (Default) Use the timestamp of the captured video frame as the timestamp of the mixed video frame.
 * - false: Do not use the timestamp of the captured video frame as the timestamp of the mixed video frame. use the timestamp when the mixed video frame is constructed Instead.
 */
@property(assign, nonatomic) BOOL syncWithPrimaryCamera;

@end

/** The definition of the screen sharing encoding parameters.
 */
__attribute__((visibility("default"))) @interface AgoraScreenCaptureParameters: NSObject
/**
 * The dimensions of the shared region in terms of width &times; height. The default value is 0, which means
 * the original dimensions of the shared screen.
 */
@property (assign, nonatomic) CGSize dimensions;
/**
 * The frame rate (fps) of the shared region. The default value is 5. We do not recommend setting
 * this to a value greater than 15.
 */
@property (assign, nonatomic) NSInteger frameRate;
/**
 * The bitrate (Kbps) of the shared region. The default value is 0, which means the SDK works out a bitrate
 * according to the dimensions of the current screen.
 */
@property (assign, nonatomic) NSInteger bitrate;

/** Sets whether to capture the mouse for screen sharing.

- YES: (Default) Capture the mouse.
- NO: Do not capture the mouse.
 */
@property(assign, nonatomic) BOOL captureMouseCursor;

/** Whether to bring the window to the front when calling [startScreenCaptureByWindowId]([AgoraRtcEngineKit startScreenCaptureByWindowId:rectangle:parameters:]) to share the window:

 - YES: Bring the window to the front.
 - NO: (Default) Do not bring the window to the front.

 @note Due to system limitations, `windowFocus` only supports bringing the main window of an application to the front.

 */
@property(assign, nonatomic) BOOL windowFocus;

/** A list of IDs of windows to be blocked.

 When calling [startScreenCaptureByDisplayId]([AgoraRtcEngineKit startScreenCaptureByDisplayId:rectangle:parameters:])
 and pass `0` in the `displayId` parameter to start sharing a main screen, you can use this parameter to block the specified windows. When calling
 [updateScreenCaptureParameters]([AgoraRtcEngineKit updateScreenCaptureParameters:]) to update the configuration for the main screen sharing, you can use this
 parameter to dynamically block the specified windows during the main screen sharing.

 */
@property(copy, nonatomic) NSArray* _Nullable excludeWindowList;

/** (macOS only) Determines whether to place a border around the shared window or screen:

 - YES: Place a border.
 - NO: (Default) Do not place a border.

 @note When you share a part of a window or screen, the SDK places a border around the entire window or screen if you set `highLighted` as YES.

 */
@property(assign, nonatomic) BOOL highLighted;
/** (macOS only) The color of the border in RGBA format. The default value is 0xFF8CBF26.

 on macOS, `COLOR_CLASS` refers to `NSColor`.

 */
@property(strong, nonatomic) COLOR_CLASS* _Nullable highLightColor;
/** (macOS only) The width (px) of the border. Defaults to 0, and the value range is [0,50].

 */
@property(assign, nonatomic) NSUInteger highLightWidth;

@end

#if (!(TARGET_OS_IPHONE) && (TARGET_OS_MAC))
/** The configuration of camera capturer.
 */
__attribute__((visibility("default"))) @interface AgoraScreenCaptureConfiguration: NSObject
/**
 * Whether to capture the window on the screen:
 * - `true`: Capture the window.
 * - `false`: (Default) Capture the screen, not the window.
 */
@property(assign, nonatomic) BOOL isCaptureWindow;
/**
 * (macOS only) The display ID of the screen.
 */
@property(assign, nonatomic) NSInteger displayId;
/**
 * (For Windows and macOS only) The window ID.
 * @note This parameter takes effect only when you want to capture the window.
 */

@property(assign, nonatomic) NSInteger windowId;

/**
 * (For Windows and macOS only) The screen capture configuration. For details, see ScreenCaptureParameters.
 */
@property(strong, nonatomic) AgoraScreenCaptureParameters* _Nonnull params;
/**
 * (For Windows and macOS only) The relative position of the shared region to the whole screen. For details, see Rectangle.
 *
 * If you do not set this parameter, the SDK shares the whole screen. If the region you set exceeds the boundary of the
 * screen, only the region within in the screen is shared. If you set width or height in Rectangle as 0, the whole
 * screen is shared.
 */
@property(assign, nonatomic) CGRect regionRect;

@end
#endif

__attribute__((visibility("default"))) @interface AgoraScreenVideoParameters : NSObject

/** CGSizezero Since the applicable below 720p

 You can customize the dimension, or select from the following list:

  - AgoraVideoDimension120x120
  - AgoraVideoDimension160x120
  - AgoraVideoDimension180x180
  - AgoraVideoDimension240x180
  - AgoraVideoDimension320x180
  - AgoraVideoDimension240x240
  - AgoraVideoDimension320x240
  - AgoraVideoDimension424x240
  - AgoraVideoDimension360x360
  - AgoraVideoDimension480x360
  - AgoraVideoDimension640x360
  - AgoraVideoDimension480x480
  - AgoraVideoDimension640x480
  - AgoraVideoDimension840x480
  - AgoraVideoDimension960x540
  - AgoraVideoDimension960x720
  - AgoraVideoDimension1280x720
  - AgoraVideoDimension1920x1080 (macOS only)

  Note:

  - The dimension does not specify the orientation mode of the output ratio. For how to set the video orientation, see [AgoraVideoOutputOrientationMode](AgoraVideoOutputOrientationMode).
  - Whether 720p can be supported depends on the device. If the device cannot support 720p, the frame rate will be lower than the one listed in the table. Agora optimizes the video in lower-end devices.
  - iPhones do not support video frame dimensions above 720p.

 */
@property(assign, nonatomic) CGSize dimensions;
/** The frame rate of the video (fps).

 You can either set the frame rate manually or choose from the following options. The default value is 15. We do not recommend setting this to a value greater than 30.

  *  AgoraVideoFrameRateFps1(1): 1 fps
  *  AgoraVideoFrameRateFps7(7): 7 fps
  *  AgoraVideoFrameRateFps10(10): 10 fps
  *  AgoraVideoFrameRateFps15(15): 15 fps
  *  AgoraVideoFrameRateFps24(24): 24 fps
  *  AgoraVideoFrameRateFps30(30): 30 fps
  *  AgoraVideoFrameRateFps60(30): 60 fps (macOS only)
 */
@property(assign, nonatomic) AgoraVideoFrameRate frameRate;
/** The bitrate of the video.

 Sets the video bitrate (Kbps). Refer to the table below and set your bitrate. If you set a bitrate beyond the proper range, the SDK automatically adjusts it to a value within the range. You can also choose from the following options:

 - AgoraVideoBitrateStandard: (recommended) the standard bitrate mode. In this mode, the bitrates differ between the interactive live streaming and Communication profiles:

     - Communication profile: the video bitrate is the same as the base bitrate.
     - Interactive live streaming profile: the video bitrate is twice the base bitrate.

 - AgoraVideoBitrateCompatible: the compatible bitrate mode. In this mode, the bitrate stays the same regardless of the profile. In the interactive live streaming profile, if you choose this mode, the video frame rate may be lower than the set value.

Agora uses different video codecs for different profiles to optimize the user experience. For example, the Communication profile prioritizes the smoothness while the interactive live streaming profile prioritizes the video quality (a higher bitrate). Therefore, Agora recommends setting this parameter as AgoraVideoBitrateStandard.

**Video Bitrate Table**

| Resolution        | Frame Rate (fps) | Base Bitrate (Kbps, for Communication) | Live Bitrate (Kbps, for Live Broadcast) |
|-------------------|------------------|----------------------------------------|-----------------------------------------|
| 160 * 120   | 15               | 65                                     | 130                                     |
| 120 * 120   | 15               | 50                                     | 100                                     |
| 320 * 180   | 15               | 140                                    | 280                                     |
| 180 * 180   | 15               | 100                                    | 200                                     |
| 240 * 180   | 15               | 120                                    | 240                                     |
| 320 * 240   | 15               | 200                                    | 400                                     |
| 240 * 240   | 15               | 140                                    | 280                                     |
| 424 * 240   | 15               | 220                                    | 440                                     |
| 640 * 360   | 15               | 400                                    | 800                                     |
| 360 * 360   | 15               | 260                                    | 520                                     |
| 640 * 360   | 30               | 600                                    | 1200                                    |
| 360 * 360   | 30               | 400                                    | 800                                     |
| 480 * 360   | 15               | 320                                    | 640                                     |
| 480 * 360   | 30               | 490                                    | 980                                     |
| 640 * 480   | 15               | 500                                    | 1000                                    |
| 480 * 480   | 15               | 400                                    | 800                                     |
| 640 * 480   | 30               | 750                                    | 1500                                    |
| 480 * 480   | 30               | 600                                    | 1200                                    |
| 848 * 480   | 15               | 610                                    | 1220                                    |
| 848 * 480   | 30               | 930                                    | 1860                                    |
| 640 * 480   | 10               | 400                                    | 800                                     |
| 1280 * 720  | 15               | 1130                                   | 2260                                    |
| 1280 * 720  | 30               | 1710                                   | 3420                                    |
| 960 * 720   | 15               | 910                                    | 1820                                    |
| 960 * 720   | 30               | 1380                                   | 2760                                    |
| 1920 * 1080 | 15               | 2080                                   | 4160                                    |
| 1920 * 1080 | 30               | 3150                                   | 6300                                    |
| 1920 * 1080 | 60               | 4780                                   | 6500                                    |


**Note:**

The base bitrate in this table applies to the Communication profile. The interactive live streaming profile generally requires a higher bitrate for better video quality. Agora recommends setting the bitrate mode as AgoraVideoBitrateStandard. You can also set the bitrate as twice the base bitrate.


*/
@property(assign, nonatomic) NSInteger bitrate;

/** contentHint The content hint for screen sharing, see [AgoraVideoContentHint](AgoraVideoContentHint).
 */
@property(assign, nonatomic) AgoraVideoContentHint contentHint;

@end

__attribute__((visibility("default"))) @interface AgoraScreenAudioParameters : NSObject

@property(assign, nonatomic) NSInteger captureSignalVolume;

@end

__attribute__((visibility("default"))) @interface AgoraScreenCaptureParameters2 : NSObject
/**
 * when captureAudio is YES, ReplayKit will  push sample buffer with RPSampleBufferTypeAudioApp, default NO.
 */
@property(assign, nonatomic) BOOL captureAudio;
/**
 * screen share with audio parameters
 */
@property(strong, nonatomic) AgoraScreenAudioParameters* _Nonnull audioParams;
/**
 * when captureVideo is YES, ReplayKit will  push sample buffer with RPSampleBufferTypeVideo, default YES.
 */
@property(assign, nonatomic) BOOL captureVideo;
/**
 * screen share with video parameters
 */
@property(strong, nonatomic) AgoraScreenVideoParameters* _Nonnull videoParams;
@end

/** Configurations of SimulcastStreamConfig.
 */

__attribute__((visibility("default"))) @interface AgoraAudioRecordingConfiguration: NSObject
/**
 * The absolute path (including the filename extensions) of the recording file. For example: `/var/mobile/Containers/Data/audio.aac`.
 * @note Ensure that the path you specify exists and is writable.
 */
@property (copy, nonatomic) NSString * _Nullable filePath;
/**
 * Recording sample rate (Hz). The following values are supported:
 * - 16000
 * - (Default) 32000
 * - 44100
 * - 48000
 * @note If this parameter is set to `44100` or `48000`, for better recording effects, Agora recommends recording WAV
 * files or AAC files whose `quality` is `AgoraAudioRecordingQualityMedium` or `AgoraAudioRecordingQualityHigh`.
 */
@property (assign, nonatomic) NSUInteger sampleRate;
/**
 * Recording content. See AgoraAudioFileRecordingType.
 */
@property (assign, nonatomic) AgoraAudioFileRecordingType fileRecordOption;
/**
 * Audio recording quality. See AgoraAudioRecordingQuality.
 * @note This parameter applies for AAC files only.
 */
@property (assign, nonatomic) AgoraAudioRecordingQuality quality;

/**
 * Recording channel.The following values are supported:
 * - (Default) 1
 * - 2
 */
@property (assign, nonatomic) NSInteger recordingChannel;

@end

/** Configurations of SimulcastStreamConfig.
 */
__attribute__((visibility("default"))) @interface AgoraSimulcastStreamConfig: NSObject

/**
 * The video bitrate (Kbps).
 */
 @property (assign, nonatomic) int kBitrate;
/**
 * The video framerate.
 */
 @property (assign, nonatomic) int framerate;
 /**
  * The video frame dimension.
  */
 @property (assign, nonatomic) CGSize dimensions;
 @end

/** The AgoraMediaStreamInfo class, reporting the whole detailed information of
 the media stream.
 */
__attribute__((visibility("default"))) @interface AgoraRtcMediaStreamInfo : NSObject
/** The index of the media stream. */
@property(nonatomic, assign) NSInteger streamIndex;
/** The type of the media stream. See AgoraMediaStreamType for details. */
@property(nonatomic, assign) AgoraMediaStreamType streamType;
/** The codec of the media stream. */
@property(nonatomic, copy) NSString *_Nonnull codecName;
/** The language of the media stream. */
@property(nonatomic, copy) NSString *_Nullable language;
/** For video stream, gets the frame rate (fps). */
@property(nonatomic, assign) NSInteger videoFrameRate;
/** For video stream, gets the bitrate (bps). */
@property(nonatomic, assign) NSInteger videoBitRate;
/** For video stream, gets the width (pixel) of the video. */
@property(nonatomic, assign) NSInteger videoWidth;
/** For video stream, gets the height (pixel) of the video. */
@property(nonatomic, assign) NSInteger videoHeight;
/** For the audio stream, gets the sample rate (Hz). */
@property(nonatomic, assign) NSInteger audioSampleRate;
/** For the audio stream, gets the channel number. */
@property(nonatomic, assign) NSInteger audioChannels;
/** The total duration (s) of the media stream. */
@property(nonatomic, assign) NSInteger duration;
/** The rotation of the video stream. */
@property(nonatomic, assign) NSInteger rotation;

@end

__attribute__((visibility("default"))) @interface AgoraAudioSpectrumInfo : NSObject

/** User ID of the speaker. */
@property(nonatomic, assign) NSInteger uid;
@property(nonatomic, strong) NSArray<NSNumber *> * _Nullable audioSpectrumData;

@end

/**
 * The configurations for the audio encoded freame.
 */
__attribute__((visibility("default"))) @interface AgoraAudioEncodedFrameDelegateConfig: NSObject
/**
 * The position where SDK record the audio, and callback to encoded audio frame observer.
 */
@property (assign, nonatomic) AgoraAudioEncodedFrameDelegatePosition postionType;
/**
 * audio encoding type.
 */
@property (assign, nonatomic) AgoraAudioEncodingType encodingType;
@end

__attribute__((visibility("default"))) @interface AgoraContentInspectModule: NSObject
@property (assign, nonatomic) AgoraContentInspectType type;

@property (assign, nonatomic) NSInteger interval;
@end

__attribute__((visibility("default"))) @interface AgoraContentInspectConfig: NSObject
@property (nonatomic, copy) NSString* _Nullable extraInfo;
@property(copy, nonatomic) NSArray<AgoraContentInspectModule*>* _Nullable modules;
@end
/**
 * The AgoraRtcConnection for the AgoraRtcEngineKitEx.
 */
__attribute__((visibility("default"))) @interface AgoraRtcConnection: NSObject <NSCopying>

/**
 * Init AgoraRtcConnection with channelId and localUid
 *
 * @param channelId Unique channel name for the AgoraRTC session in the string.
 * @param localUid Local User ID. A 32-bit unsigned integer.
 * @return AgoraRtcConnection.
 */
- (instancetype _Nonnull)initWithChannelId:(NSString *_Nonnull)channelId localUid:(NSInteger)localUid;

/* channelId Unique channel name for the AgoraRTC session in the string
 * format. The string length must be less than 64 bytes. Supported character
 * scopes are:
 * - All lowercase English letters: a to z.
 * - All uppercase English letters: A to Z.
 * - All numeric characters: 0 to 9.
 * - The space character.
 * - Punctuation characters and other symbols, including: "!", "#", "$", "%", "&", "(", ")", "+", "-", ":", ";", "<", "=", ".", ">", "?", "@", "[", "]", "^", "_", " {", "}", "|", "~", ",".
 */
@property (nonatomic, copy) NSString *_Nonnull channelId;
/* uid Local User ID. A 32-bit unsigned integer with a value ranging from 1 to
 * (2<sup>32</sup>-1). The `uid` must be unique and not set to 0 . Your app
 * must record and maintain the returned `uid` since the SDK does not do so.
 */
@property (nonatomic, assign) NSUInteger localUid;

@end
/**
 * remote video subscription options
 */
__attribute__((visibility("default"))) @interface AgoraVideoSubscriptionOptions: NSObject

/*
* The type of the video stream to subscribe to.
*
* The default value is `VIDEO_STREAM_HIGH`, which means the high-quality
* video stream.
*/
@property (nonatomic, assign) AgoraVideoStreamType type;
/*
* Whether to subscribe to encoded video data only:
* - `true`: Subscribe to encoded video data only.
* - `false`: (Default) Subscribe to decoded video data.
*/
@property (nonatomic, assign) bool encodedFrameOnly;

@end
/**
 * The configurations for the audio encoded freame.
 */
__attribute__((visibility("default"))) @interface AgoraEncodedVideoFrameInfo: NSObject

/**
 * The video codec: #VIDEO_CODEC_TYPE.
 */
@property (assign, nonatomic) AgoraVideoCodecType codecType;
/**
 * The width (px) of the video.
 */
@property (assign, nonatomic) NSInteger width;
/**
 * The height (px) of the video.
 */
@property (assign, nonatomic) NSInteger height;
/**
 * The number of video frames per second.
 * This value will be used for calculating timestamps of the encoded image.
 * If framesPerSecond equals zero, then real timestamp will be used.
 * Otherwise, timestamp will be adjusted to the value of framesPerSecond set.
 */
@property (assign, nonatomic) NSInteger framesPerSecond;
/**
 * The frame type of the encoded video frame: #VIDEO_FRAME_TYPE.
 */
@property (assign, nonatomic) AgoraVideoFrameType frameType;
/**
 * The rotation information of the encoded video frame: #VIDEO_ORIENTATION.
 */
@property (assign, nonatomic) NSInteger rotation;
/**
 * The track ID of the video frame.
 */
@property (assign, nonatomic) NSInteger trackId;  // This can be reserved for multiple video tracks, we need to create different ssrc
              // and additional payload for later implementation.
/**
  * This is a input parameter which means the timestamp for capturing the video.
  */
@property (assign, nonatomic) NSInteger captureTimeMs;
/**
  * This is a output parameter which means the timestamp for decoding the video.
  */
@property (assign, nonatomic) NSInteger decodeTimeMs;
/**
 * ID of the user.
 */
@property (assign, nonatomic) NSInteger uid;
/**
 * The stream type of video frame.
 */
@property (assign, nonatomic) AgoraVideoStreamType streamType;
@end

NS_SWIFT_NAME(LogUploadServerInfo) __attribute__((visibility("default"))) @interface LogUploadServerInfo : NSObject
/** Log upload server domain
 */
@property(copy, nonatomic) NSString* _Nullable serverDomain;
/** Log upload server path
 */
@property(copy, nonatomic) NSString* _Nullable serverPath;
/** Log upload server port
 */
@property(assign, nonatomic) NSInteger serverPort;
/** Whether to use HTTPS request:
    - true: Use HTTPS request
    - fasle: Use HTTP request
  */
@property(assign, nonatomic) BOOL serverHttps;
@end

NS_SWIFT_NAME(AdvancedConfigInfo) __attribute__((visibility("default"))) @interface AdvancedConfigInfo : NSObject
/** Log upload server
 */
@property(strong, nonatomic) LogUploadServerInfo* _Nullable logUploadServer;
@end

NS_SWIFT_NAME(AgoraLocalAccessPointConfiguration) __attribute__((visibility("default"))) @interface AgoraLocalAccessPointConfiguration : NSObject
/** Local access point IP address list.
 */
@property(copy, nonatomic) NSArray* _Nullable ipList NS_SWIFT_NAME(ipList);
/** Local access point domain list.
 */
@property(copy, nonatomic) NSArray* _Nullable domainList NS_SWIFT_NAME(domainList);
/** Certificate domain name installed on specific local access point. pass "" means using sni domain on specific local access point
 *  SNI(Server Name Indication) is an extension to the TLS protocol.
 */
@property(copy, nonatomic) NSString* _Nullable verifyDomainName NS_SWIFT_NAME(verifyDomainName);
/** Local proxy connection mode, connectivity first or local only.
 */
@property(assign, nonatomic) AgoraLocalProxyMode mode NS_SWIFT_NAME(mode);
/** Local proxy connection, advanced config info.
  */
@property(strong, nonatomic) AdvancedConfigInfo* _Nullable advancedConfig NS_SWIFT_NAME(advancedConfig);
@end

/**
 * The configuration of rhythm player,
 * which is set in startRhythmPlayer or configRhythmPlayer.
 */
__attribute__((visibility("default"))) @interface AgoraRhythmPlayerConfig: NSObject
/**
 * The number of beats per measure. The range is 1 to 9.
 * The default value is 4,
 * which means that each measure contains one downbeat and three upbeats.
 */
@property (assign, nonatomic) int beatsPerMeasure;
/*
 * The range is 60 to 360.
 * The default value is 60,
 * which means that the rhythm player plays 60 beats in one minute.
 */
@property (assign, nonatomic) int beatsPerMinute;

@end

/** AgoraFacePositionInfo.
 */
NS_SWIFT_NAME(AgoraFacePositionInfo) __attribute__((visibility("default"))) @interface AgoraFacePositionInfo : NSObject

/** The x coordinate (px) of the human face in the local video. Taking the top left corner of the captured video as the origin, the x coordinate represents the relative lateral displacement of the top left corner of the human face to the origin.
 */
@property(assign, nonatomic) NSInteger x NS_SWIFT_NAME(x);

/** The y coordinate (px) of the human face in the local video. Taking the top left corner of the captured video as the origin, the y coordinate represents the relative longitudinal displacement of the top left corner of the human face to the origin.
 */
@property(assign, nonatomic) NSInteger y NS_SWIFT_NAME(y);

/** The width (px) of the human face in the captured video.
 */
@property(assign, nonatomic) NSInteger width NS_SWIFT_NAME(width);

/** The height (px) of the human face in the captured video.
 */
@property(assign, nonatomic) NSInteger height NS_SWIFT_NAME(height);

/** The distance (cm) between the human face and the screen.
 */
@property(assign, nonatomic) NSInteger distance NS_SWIFT_NAME(distance);
@end

/** AgoraAdvancedAudioOptions.
 */
__attribute__((visibility("default"))) @interface AgoraAdvancedAudioOptions: NSObject

@property(assign, nonatomic) AgoraAudioProcessChannels audioProcessingChannels;

@end

__attribute__((visibility("default"))) @interface AgoraImageTrackOptions : NSObject
@property(copy, nonatomic) NSString *_Nullable imageUrl;
@property(assign, nonatomic) int fps;
@property(assign, nonatomic) AgoraVideoMirrorMode mirrorMode;
@end

/**
 * spatial audio parameters
 * .
 */
__attribute__((visibility("default"))) @interface AgoraSpatialAudioParams : NSObject
/**
 * Speaker azimuth in a spherical coordinate system centered on the listener.
 */
@property(assign, nonatomic) double speaker_azimuth;
/**
 * Speaker elevation in a spherical coordinate system centered on the listener.
 */
@property(assign, nonatomic) double speaker_elevation;
/**
 * Distance between speaker and listener.
 */
@property(assign, nonatomic) double speaker_distance;
/**
 * Speaker orientation [0-180], 0 degree is the same with listener orientation.
 */
@property(assign, nonatomic) NSInteger speaker_orientation;
/**
 * Enable blur or not for the speaker.
 */
@property(assign, nonatomic) BOOL enable_blur;
/**
 * Enable air absorb or not for the speaker.
 */
@property(assign, nonatomic) BOOL enable_air_absorb;
/**
 * Speaker attenuation factor.
 */
@property(assign, nonatomic) double speaker_attenuation;
/**
  * Enable doppler factor.
  */
@property(assign, nonatomic) BOOL enable_doppler;
@end

NS_SWIFT_NAME(AgoraEchoTestConfiguration) 
__attribute__((visibility("default"))) @interface AgoraEchoTestConfiguration : NSObject
/** The video display view.

 VIEW_CLASS is a general name for this property. See the following definitions for iOS and macOS:

 - iOS: UIView
 - MacOS: NSView
 */
@property(strong, nonatomic) VIEW_CLASS* _Nullable view NS_SWIFT_NAME(view);
/** Whether to enable audio.
 */
@property(assign, nonatomic) BOOL enableAudio NS_SWIFT_NAME(enableAudio);
/** Whether to enable video.
 */
@property(assign, nonatomic) BOOL enableVideo NS_SWIFT_NAME(enableVideo);
/** The token for join channel.
 */
@property(copy, nonatomic) NSString* _Nullable token NS_SWIFT_NAME(token);
/** The channelId.
 */
@property(copy, nonatomic) NSString* _Nonnull channelId NS_SWIFT_NAME(channelId);
@end

/**
 * Indicator optimization degree.
 */
NS_SWIFT_NAME(AgoraWlAccStats) __attribute__((visibility("default"))) @interface AgoraWlAccStats : NSObject
/**
 * End-to-end delay optimization percentage.
 */
@property(assign, nonatomic) NSInteger e2eDelayPercent NS_SWIFT_NAME(e2eDelayPercent);
/**
 * Frozen Ratio optimization percentage.
 */
@property(assign, nonatomic) NSInteger frozenRatioPercent NS_SWIFT_NAME(frozenRatioPercent);
/**
 * Loss Rate optimization percentage.
 */
@property(assign, nonatomic) NSInteger lossRatePercent NS_SWIFT_NAME(lossRatePercent);
@end

__attribute__((visibility("default"))) @interface AgoraMediaRecorderInfo : NSObject

/** Recorder file name. It contains file's absolute path.
 */
@property(copy, nonatomic) NSString* _Nonnull recorderFileName;
/** Record duration (ms).
 */
@property(assign, nonatomic) NSUInteger durationMs;
/** Record file size (Byte).
 */
@property(assign, nonatomic) NSUInteger fileSize;

@end

__attribute__((visibility("default"))) @interface AgoraMediaRecorderConfiguration : NSObject

/** Recorder file storage path. It contains file name (absolute path), such as -xxx.flv, -xxx.mp4, etc.
 */
@property(copy, nonatomic) NSString* _Nonnull storagePath;
/** Container format. See AgoraMediaRecorderContainerFormat.
 */
@property(assign, nonatomic) AgoraMediaRecorderContainerFormat containerFormat;
/** Stream Type. See AgoraMediaRecorderStreamType.
 */
@property(assign, nonatomic) AgoraMediaRecorderStreamType streamType;
/** Max duration (ms). Default 120000ms.
 */
@property(assign, nonatomic) NSUInteger maxDurationMs;
/** Recorder information update interval (ms, [1000~10000]). If it is set to other values, there won't be any callback.
 */
@property(assign, nonatomic) NSUInteger recorderInfoUpdateInterval;

@end

#if (!(TARGET_OS_IPHONE) && (TARGET_OS_MAC))

/**
 The AgoraScreenCaptureSourceInfo interface. (macOS only)
 */
__attribute__((visibility("default"))) @interface AgoraScreenCaptureSourceInfo : NSObject
/** The type of the shared target. See ScreenCaptureSourceType. */
@property(assign, nonatomic) AgoraScreenCaptureSourceType type;
/** The window ID for a window or the display ID for a screen. */
@property(assign, nonatomic) CGWindowID sourceId;
/** The name of the window or screen. UTF-8 encoding. */
@property(copy, nonatomic) NSString* _Nonnull sourceName;
/** The image content of the thumbnail.. */
@property(strong, nonatomic) NSImage* _Nonnull thumbImage;
/** The image content of the icon. . */
@property(strong, nonatomic) NSImage* _Nullable iconImage;
/** The process to which the window belongs. UTF-8 encoding. */
@property(copy, nonatomic) NSString* _Nonnull processPath;
/** The name of the processName. UTF-8 encoding. */
@property(copy, nonatomic) NSString* _Nonnull sourceTitle;
/** The relative position of the shared region to the screen space (A virtual space include all the screens). */
@property(assign, nonatomic) CGRect position;
/** Determines whether the screen is the primary display:

 - YES: The screen is the primary display.
 - NO: The screen is not the primary display.
 */
@property(assign, nonatomic) BOOL primaryMonitor;

@end

#endif

NS_SWIFT_NAME(AgoraExtensionInfo) __attribute__((visibility("default"))) @interface AgoraExtensionInfo : NSObject

/**
 * The type of media device.
 */
@property (assign, nonatomic) AgoraMediaSourceType sourceType NS_SWIFT_NAME(sourceType);

/**
 * The id of the remote user on which the extension works.
 *
 * @note remoteUid = 0 means that the extension works on all remote streams.
 */
@property (assign, nonatomic) NSUInteger remoteUid NS_SWIFT_NAME(remoteUid);

/**
 *  The unique channel name for the AgoraRTC session in the string format. The string
 * length must be less than 64 bytes. Supported character scopes are:
 * - All lowercase English letters: a to z.
 * - All uppercase English letters: A to Z.
 * - All numeric characters: 0 to 9.
 * - The space character.
 * - Punctuation characters and other symbols, including: "!", "#", "$", "%", "&", "(", ")", "+",
 * "-",
 * ":", ";", "<", "=", ".", ">", "?", "@", "[", "]", "^", "_", " {", "}", "|", "~", ",".
 */
@property (copy, nonatomic) NSString *_Nonnull channelId NS_SWIFT_NAME(channelId);

/**
 * User ID: A 32-bit unsigned integer ranging from 1 to (2^32-1). It must be unique.
 */
@property (assign, nonatomic) NSUInteger localUid NS_SWIFT_NAME(localUid);

@end

/**
 * The video rendering tracing result
 */
NS_SWIFT_NAME(AgoraVideoRenderingTracingInfo) __attribute__((visibility("default"))) @interface AgoraVideoRenderingTracingInfo : NSObject
/**
 * Elapsed time from the start tracing time to the time when the tracing event occurred.
 */
@property (assign, nonatomic) NSInteger elapsedTime NS_SWIFT_NAME(elapsedTime);
/**
 * Elapsed time from the start tracing time to the time when join channel.
 * 
 * **Note**
 * If the start tracing time is behind the time when join channel, this value will be negative.
 */
@property (assign, nonatomic) NSInteger start2JoinChannel NS_SWIFT_NAME(start2JoinChannel);
/**
 * Elapsed time from joining channel to finishing joining channel.
 */
@property (assign, nonatomic) NSInteger join2JoinSuccess NS_SWIFT_NAME(join2JoinSuccess);
/**
 * Elapsed time from finishing joining channel to remote user joined.
 * 
 * **Note**
 * If the start tracing time is after the time finishing join channel, this value will be
 * the elapsed time from the start tracing time to remote user joined. The minimum value is 0.
 */
@property (assign, nonatomic) NSInteger joinSuccess2RemoteJoined NS_SWIFT_NAME(joinSuccess2RemoteJoined);
/**
 * Elapsed time from remote user joined to set the view.
 * 
 * **Note**
 * If the start tracing time is after the time when remote user joined, this value will be
 * the elapsed time from the start tracing time to set the view. The minimum value is 0.
 */
@property (assign, nonatomic) NSInteger remoteJoined2SetView NS_SWIFT_NAME(remoteJoined2SetView);
/**
 * Elapsed time from remote user joined to the time subscribing remote video stream.
 * 
 * **Note**
 * If the start tracing time is after the time when remote user joined, this value will be
 * the elapsed time from the start tracing time to the time subscribing remote video stream.
 * The minimum value is 0.
 */
@property (assign, nonatomic) NSInteger remoteJoined2UnmuteVideo NS_SWIFT_NAME(remoteJoined2UnmuteVideo);
/**
 * Elapsed time from remote user joined to the remote video packet received.
 * 
 * **Note**
 * If the start tracing time is after the time when remote user joined, this value will be
 * the elapsed time from the start tracing time to the time subscribing remote video stream.
 * The minimum value is 0.
 */
@property (assign, nonatomic) NSInteger remoteJoined2PacketReceived NS_SWIFT_NAME(remoteJoined2PacketReceived);

@end

/** The configuration of custom audio track
*/
NS_SWIFT_NAME(AgoraAudioTrackConfig) __attribute__((visibility("default"))) @interface AgoraAudioTrackConfig : NSObject
/**
 * Enable local playback, enabled by default
 * true: (Default) Enable local playback
 * false: Do not enable local playback
 */
@property (assign, nonatomic) BOOL enableLocalPlayback NS_SWIFT_NAME(enableLocalPlayback);

@end
