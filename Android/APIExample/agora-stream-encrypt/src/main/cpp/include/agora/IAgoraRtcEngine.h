//
//  Agora Rtc Engine SDK
//
//  Copyright (c) 2018 Agora.io. All rights reserved.
//
#pragma once

#include "AgoraBase.h"
#include "AgoraMediaBase.h"
#include "IAgoraLog.h"
#include "AgoraOptional.h"
#include "IAudioDeviceManager.h"
#include "IAgoraRhythmPlayer.h"
#include "IAgoraMediaEngine.h"
#include "IAgoraH265Transcoder.h"

namespace agora {
namespace rtm {
class IStreamChannel;
}
namespace rtc {

template <typename T>
static void SetFrom(Optional<T>* s, const Optional<T>& o) {
  if (o) {
    *s = o;
  }
}

template <typename T>
static void ReplaceBy(Optional<T>* s, const Optional<T>& o) {
  *s = o;
}

//class IAudioDeviceManager;

/**
 * @brief Media device types.
 */
enum MEDIA_DEVICE_TYPE {
  /**
   * -1: Unknown device type.
   */
  UNKNOWN_AUDIO_DEVICE = -1,
  /**
   * 0: Audio playback device.
   */
  AUDIO_PLAYOUT_DEVICE = 0,
  /**
   * 1: Audio capturing device.
   */
  AUDIO_RECORDING_DEVICE = 1,
  /**
   * 2: Video rendering device (graphics card).
   */
  VIDEO_RENDER_DEVICE = 2,
  /**
   * 3: Video capturing device.
   */
  VIDEO_CAPTURE_DEVICE = 3,
  /**
   * 4: Audio playback device for an app.
   */
  AUDIO_APPLICATION_PLAYOUT_DEVICE = 4,
  /**
   * (For macOS only) 5: Virtual audio playback device (virtual sound card).
   */
  AUDIO_VIRTUAL_PLAYOUT_DEVICE = 5,
  /**
   * (For macOS only) 6: Virtual audio capturing device (virtual sound card).
   */
  AUDIO_VIRTUAL_RECORDING_DEVICE = 6,
};

/**
 * @brief The playback state of the music file.
 */
enum AUDIO_MIXING_STATE_TYPE {
  /**
   * 710: The music file is playing.
   */
  AUDIO_MIXING_STATE_PLAYING = 710,
  /**
   * 711: The music file pauses playing.
   */
  AUDIO_MIXING_STATE_PAUSED = 711,
  /**
   * 713: The music file stops playing.
   * The possible reasons include:
   * - AUDIO_MIXING_REASON_ALL_LOOPS_COMPLETED (723)
   * - AUDIO_MIXING_REASON_STOPPED_BY_USER (724)
   */
  AUDIO_MIXING_STATE_STOPPED = 713,
  /**
   * 714: An error occurs during the playback of the audio mixing file.
   * The possible reasons include:
   * - AUDIO_MIXING_REASON_CAN_NOT_OPEN (701)
   * - AUDIO_MIXING_REASON_TOO_FREQUENT_CALL (702)
   * - AUDIO_MIXING_REASON_INTERRUPTED_EOF (703)
   */
  AUDIO_MIXING_STATE_FAILED = 714,
};

/**
 * @brief The reason why the playback state of the music file changes. Reported in the
 * `onAudioMixingStateChanged` callback.
 */
enum AUDIO_MIXING_REASON_TYPE {
  /**
   * 701: The SDK cannot open the music file. For example, the local music file does not exist, the
   * SDK does not support the file format, or the the SDK cannot access the music file URL.
   */
  AUDIO_MIXING_REASON_CAN_NOT_OPEN = 701,
  /**
   * 702: The SDK opens the music file too frequently. If you need to call `startAudioMixing` multiple
   * times, ensure that the call interval is more than 500 ms.
   */
  AUDIO_MIXING_REASON_TOO_FREQUENT_CALL = 702,
  /**
   * 703: The music file playback is interrupted.
   */
  AUDIO_MIXING_REASON_INTERRUPTED_EOF = 703,
  /**
   * 721: The music file completes a loop playback.
   */
  AUDIO_MIXING_REASON_ONE_LOOP_COMPLETED = 721,
  /**
   * 723: The music file completes all loop playback.
   */
  AUDIO_MIXING_REASON_ALL_LOOPS_COMPLETED = 723,
  /**
   * 724: Successfully call `stopAudioMixing` to stop playing the music file.
   */
  AUDIO_MIXING_REASON_STOPPED_BY_USER = 724,
  /** 726: The audio mixing playback has resumed by user */
  AUDIO_MIXING_REASON_RESUMED_BY_USER = 726,
  /**
   * 0: The SDK opens music file successfully.
   */
  AUDIO_MIXING_REASON_OK = 0,
};

/**
 * The status of importing an external video stream in a live broadcast.
 */
enum INJECT_STREAM_STATUS {
  /**
   * 0: The media stream is injected successfully.
   */
  INJECT_STREAM_STATUS_START_SUCCESS = 0,
  /**
   * 1: The media stream already exists.
   */
  INJECT_STREAM_STATUS_START_ALREADY_EXISTS = 1,
  /**
   * 2: The media stream injection is unauthorized.
   */
  INJECT_STREAM_STATUS_START_UNAUTHORIZED = 2,
  /**
   * 3: Timeout occurs when injecting a media stream.
   */
  INJECT_STREAM_STATUS_START_TIMEDOUT = 3,
  /**
   * 4: The media stream injection fails.
   */
  INJECT_STREAM_STATUS_START_FAILED = 4,
  /**
   * 5: The media stream stops being injected successfully.
   */
  INJECT_STREAM_STATUS_STOP_SUCCESS = 5,
  /**
   * 6: The media stream injection that you want to stop is found.
   */
  INJECT_STREAM_STATUS_STOP_NOT_FOUND = 6,
  /**
   * 7: You are not authorized to stop the media stream injection.
   */
  INJECT_STREAM_STATUS_STOP_UNAUTHORIZED = 7,
  /**
   * 8: Timeout occurs when you stop injecting the media stream.
   */
  INJECT_STREAM_STATUS_STOP_TIMEDOUT = 8,
  /**
   * 9: Stopping injecting the media stream fails.
   */
  INJECT_STREAM_STATUS_STOP_FAILED = 9,
  /**
   * 10: The media stream is broken.
   */
  INJECT_STREAM_STATUS_BROKEN = 10,
};

/**
 * @brief The midrange frequency for audio equalization.
 */
enum AUDIO_EQUALIZATION_BAND_FREQUENCY {
  /**
   * 0: 31 Hz
   */
  AUDIO_EQUALIZATION_BAND_31 = 0,
  /**
   * 1: 62 Hz
   */
  AUDIO_EQUALIZATION_BAND_62 = 1,
  /**
   * 2: 125 Hz
   */
  AUDIO_EQUALIZATION_BAND_125 = 2,
  /**
   * 3: 250 Hz
   */
  AUDIO_EQUALIZATION_BAND_250 = 3,
  /**
   * 4: 500 Hz
   */
  AUDIO_EQUALIZATION_BAND_500 = 4,
  /**
   * 5: 1 kHz
   */
  AUDIO_EQUALIZATION_BAND_1K = 5,
  /**
   * 6: 2 kHz
   */
  AUDIO_EQUALIZATION_BAND_2K = 6,
  /**
   * 7: 4 kHz
   */
  AUDIO_EQUALIZATION_BAND_4K = 7,
  /**
   * 8: 8 kHz
   */
  AUDIO_EQUALIZATION_BAND_8K = 8,
  /**
   * 9: 16 kHz
   */
  AUDIO_EQUALIZATION_BAND_16K = 9,
};

/**
 * @brief Audio reverberation types.
 */
enum AUDIO_REVERB_TYPE {
  /**
   * 0: The level of the dry signal (dB). The value is between -20 and 10.
   */
  AUDIO_REVERB_DRY_LEVEL = 0,
  /**
   * 1: The level of the early reflection signal (wet signal) (dB). The value is between -20 and 10.
   */
  AUDIO_REVERB_WET_LEVEL = 1,
  /**
   * 2: The room size of the reflection. The value is between 0 and 100.
   */
  AUDIO_REVERB_ROOM_SIZE = 2,
  /**
   * 3: The length of the initial delay of the wet signal (ms). The value is between 0 and 200.
   */
  AUDIO_REVERB_WET_DELAY = 3,
  /**
   * 4: The reverberation strength. The value is between 0 and 100.
   */
  AUDIO_REVERB_STRENGTH = 4,
};

/**
 * @brief Options for handling audio and video stream fallback when network conditions are weak.
 */
enum STREAM_FALLBACK_OPTIONS {
  /**
   * 0: No fallback processing is performed on audio and video streams, the quality of the audio and
   * video streams cannot be guaranteed.
   */
  STREAM_FALLBACK_OPTION_DISABLED = 0,
  /**
   * 1: Only receive low-quality (low resolution, low bitrate) video stream.
   */
  STREAM_FALLBACK_OPTION_VIDEO_STREAM_LOW = 1,
  /**
   * 2: When the network conditions are weak, try to receive the low-quality video stream first. If
   * the video cannot be displayed due to extremely weak network environment, then fall back to
   * receiving audio-only stream.
   */
  STREAM_FALLBACK_OPTION_AUDIO_ONLY = 2,
  /** 3~8: If the receiver SDK uses RtcEngineParameters::setRemoteSubscribeFallbackOption，it will receive
     one of the streams from agora::rtc::VIDEO_STREAM_LAYER_1 to agora::rtc::VIDEO_STREAM_LAYER_6
     if the related layer exists when the network condition is poor. The lower bound of fallback depends on
     the STREAM_FALLBACK_OPTION_VIDEO_STREAM_LAYER_X. */
  STREAM_FALLBACK_OPTION_VIDEO_STREAM_LAYER_1 = 3,
  STREAM_FALLBACK_OPTION_VIDEO_STREAM_LAYER_2 = 4,
  STREAM_FALLBACK_OPTION_VIDEO_STREAM_LAYER_3 = 5,
  STREAM_FALLBACK_OPTION_VIDEO_STREAM_LAYER_4 = 6,
  STREAM_FALLBACK_OPTION_VIDEO_STREAM_LAYER_5 = 7,
  STREAM_FALLBACK_OPTION_VIDEO_STREAM_LAYER_6 = 8,
};

enum PRIORITY_TYPE {
  /** 50: High priority.
   */
  PRIORITY_HIGH = 50,
  /** 100: (Default) normal priority.
   */
  PRIORITY_NORMAL = 100,
};

struct RtcConnection;

/**
 * @brief The statistics of the local video stream.
 */
struct LocalVideoStats
{
  /**
   * The ID of the local user.
   */
  uid_t uid;
  /**
   * The actual bitrate (Kbps) while sending the local video stream.
   * @note This value does not include the bitrate for resending the video after packet loss.
   */
  int sentBitrate;
  /**
   * The actual frame rate (fps) while sending the local video stream.
   * @note This value does not include the frame rate for resending the video after packet loss.
   */
  int sentFrameRate;
  /**
   * The frame rate (fps) for capturing the local video stream.
   */
  int captureFrameRate;
  /**
   * The width (px) for capturing the local video stream.
   */
  int captureFrameWidth;
  /**
   * The height (px) for capturing the local video stream.
   */
  int captureFrameHeight;
  /**
   * The frame rate (fps) adjusted by the built-in video capture adapter (regulator) of the SDK for
   * capturing the local video stream. The regulator adjusts the frame rate of the video captured by
   * the camera according to the video encoding configuration.
   */
  int regulatedCaptureFrameRate;
  /**
   * The width (px) adjusted by the built-in video capture adapter (regulator) of the SDK for
   * capturing the local video stream. The regulator adjusts the height and width of the video
   * captured by the camera according to the video encoding configuration.
   */
  int regulatedCaptureFrameWidth;
  /**
   * The height (px) adjusted by the built-in video capture adapter (regulator) of the SDK for
   * capturing the local video stream. The regulator adjusts the height and width of the video
   * captured by the camera according to the video encoding configuration.
   */
  int regulatedCaptureFrameHeight;
  /**
   * The output frame rate (fps) of the local video encoder.
   */
  int encoderOutputFrameRate;
  /**
   * The width of the encoded video (px).
   */
  int encodedFrameWidth;
  /**
   * The height of the encoded video (px).
   */
  int encodedFrameHeight;
  /**
   * The output frame rate (fps) of the local video renderer.
   */
  int rendererOutputFrameRate;
  /**
   * The target bitrate (Kbps) of the current encoder. This is an estimate made by the SDK based on
   * the current network conditions.
   */
  int targetBitrate;
  /**
   * The target frame rate (fps) of the current encoder.
   */
  int targetFrameRate;
  /**
   * The quality adaptation of the local video stream in the reported interval (based on the target
   * frame rate and target bitrate). See `QUALITY_ADAPT_INDICATION`.
   */
  QUALITY_ADAPT_INDICATION qualityAdaptIndication;
  /**
   * The bitrate (Kbps) while encoding the local video stream.
   * @note This value does not include the bitrate for resending the video after packet loss.
   */
  int encodedBitrate;
  /**
   * The number of the sent video frames, represented by an aggregate value.
   */
  int encodedFrameCount;
  /**
   * The codec type of the local video. See `VIDEO_CODEC_TYPE`.
   */
  VIDEO_CODEC_TYPE codecType;
  /**
   * The video packet loss rate (%) from the local client to the Agora server before applying the
   * anti-packet loss strategies.
   */
  unsigned short txPacketLossRate;
  /**
   * The brightness level of the video image captured by the local camera. See
   * `CAPTURE_BRIGHTNESS_LEVEL_TYPE`.
   */
  CAPTURE_BRIGHTNESS_LEVEL_TYPE captureBrightnessLevel;
  /**
    * Whether we send dual stream now.
    */
  bool dualStreamEnabled;
  /**
   * The local video encoding acceleration type.
   * - 0: Software encoding is applied without acceleration.
   * - 1: Hardware encoding is applied for acceleration.
   */
  int hwEncoderAccelerating;
  /** The dimensions of the simulcast streams's encoding frame.
    */
  VideoDimensions simulcastDimensions[SimulcastConfig::STREAM_LAYER_COUNT_MAX];
  /**
   * @technical preview
   * The encodedFrameDepth of the local video:
   * - SDR = 8.
   * - HDR = 10.
   */
  int encodedFrameDepth;
};

/**
 * @brief Audio statistics of the remote user.
 */
struct RemoteAudioStats
{
  /**
   * The user ID of the remote user.
   */
  uid_t uid;
  /**
   * The quality of the audio stream sent by the user. See `QUALITY_TYPE`.
   */
  int quality;
  /**
   * The network delay (ms) from the sender to the receiver.
   */
  int networkTransportDelay;
  /**
   * The network delay (ms) from the audio receiver to the jitter buffer.
   * @note When the receiving end is an audience member and `audienceLatencyLevel` of
   * `ClientRoleOptions` is 1, this parameter does not take effect.
   */
  int jitterBufferDelay;
  /**
   * The frame loss rate (%) of the remote audio stream in the reported interval.
   */
  int audioLossRate;
  /**
   * The number of audio channels.
   */
  int numChannels;
  /**
   * The sampling rate of the received audio stream in the reported interval.
   */
  int receivedSampleRate;
  /**
   * The average bitrate (Kbps) of the received audio stream in the reported interval.
   */
  int receivedBitrate;
  /**
   * The total freeze time (ms) of the remote audio stream after the remote user joins the channel. In
   * a session, audio freeze occurs when the audio frame loss rate reaches 4%.
   */
  int totalFrozenTime;
  /**
   * The total audio freeze time as a percentage (%) of the total time when the audio is available.
   * The audio is considered available when the remote user neither stops sending the audio stream nor
   * disables the audio module after joining the channel.
   */
  int frozenRate;
  /**
   * The quality of the remote audio stream in the reported interval. The quality is determined by the
   * Agora real-time audio MOS (Mean Opinion Score) measurement method. The return value range is [0,
   * 500]. Dividing the return value by 100 gets the MOS score, which ranges from 0 to 5. The higher
   * the score, the better the audio quality.
   * The subjective perception of audio quality corresponding to the Agora real-time audio MOS scores is as follows:
   * | MOS score      | Perception of audio quality                                                                                                                                  |
   * | -------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------ |
   * | Greater than 4 | Excellent. The audio sounds clear and smooth.                                                                                                                |
   * | From 3.5 to 4  | Good. The audio has some perceptible impairment but still sounds clear.                                                                                      |
   * | From 3 to 3.5  | Fair. The audio freezes occasionally and requires attentive listening.                                                                                       |
   * | From 2.5 to 3  | Poor. The audio sounds choppy and requires considerable effort to understand.                                                                                |
   * | From 2 to 2.5  | Bad. The audio has occasional noise. Consecutive audio dropouts occur, resulting in some information loss. The users can communicate only with difficulty.   |
   * | Less than 2    | Very bad. The audio has persistent noise. Consecutive audio dropouts are frequent, resulting in severe information loss. Communication is nearly impossible. |
   */
  int mosValue;
  /**
   * If the packet loss concealment (PLC) occurs for N consecutive times, freeze is considered as PLC occurring for M consecutive times.
   * freeze cnt = (n_plc - n) / m
   */
  uint32_t frozenRateByCustomPlcCount;
  /**
   * The number of audio packet loss concealment
   */
  uint32_t plcCount;

  /**
   * @technical preview
   * The number of times the remote audio stream has experienced freezing.
   */
  uint32_t frozenCntByCustom;

  /**
   * @technical preview
   * The total duration (ms) that the remote audio stream has been in a frozen state.
   */
  uint32_t frozenTimeByCustom;

  /**
   * The total active time (ms) between the start of the audio call and the callback of the remote
   * user.
   * The active time refers to the total duration of the remote user without the mute state.
   */
  int totalActiveTime;
  /**
   * The total duration (ms) of the remote audio stream.
   */
  int publishDuration;
  /**
   * The Quality of Experience (QoE) of the local user when receiving a remote audio stream. See
   * `EXPERIENCE_QUALITY_TYPE`.
   */
  int qoeQuality;
  /**
   * Reasons why the QoE of the local user when receiving a remote audio stream is poor. See
   * `EXPERIENCE_POOR_REASON`.
   */
  int qualityChangedReason;
  /**
   * The total number of audio bytes received (bytes), inluding the FEC bytes, represented by an aggregate value.
   */
  unsigned int rxAudioBytes;
  /**
   * End-to-end audio delay (in milliseconds), which refers to the time from when the audio is
   * captured by the remote user to when it is played by the local user.
   */
  int e2eDelay;

  RemoteAudioStats()
      : uid(0),
        quality(0),
        networkTransportDelay(0),
        jitterBufferDelay(0),
        audioLossRate(0),
        numChannels(0),
        receivedSampleRate(0),
        receivedBitrate(0),
        totalFrozenTime(0),
        frozenRate(0),
        mosValue(0),
        frozenRateByCustomPlcCount(0),
        plcCount(0),
        frozenCntByCustom(0),
        frozenTimeByCustom(0),
        totalActiveTime(0),
        publishDuration(0),
        qoeQuality(0),
        qualityChangedReason(0),
        rxAudioBytes(0),
        e2eDelay(0) {}
};

/**
 * @brief Statistics of the remote video stream.
 */
struct RemoteVideoStats {
  /**
   * The user ID of the remote user sending the video stream.
   */
  uid_t uid;
  /**
   * Deprecated:
   * In scenarios where audio and video are synchronized, you can get the video delay data from
   * `networkTransportDelay` and `jitterBufferDelay` in `RemoteAudioStats`.
   * The video delay (ms).
   * @deprecated Time delay (ms).
   */
  int delay __deprecated;
  /**
   * End-to-end video latency (ms). That is, the time elapsed from the video capturing on the remote
   * user's end to the receiving and rendering of the video on the local user's end.
   */
  int e2eDelay;
  /**
   * The width (pixels) of the video.
   */
  int width;
  /**
   * The height (pixels) of the video.
   */
  int height;
  /**
   * The bitrate (Kbps) of the remote video received since the last count.
   */
  int receivedBitrate;
  /** The decoder input frame rate (fps) of the remote video.
   */
  int decoderInputFrameRate;
  /**
   * The frame rate (fps) of decoding the remote video.
   */
  int decoderOutputFrameRate;
  /**
   * The frame rate (fps) of rendering the remote video.
   */
  int rendererOutputFrameRate;
  /**
   * The packet loss rate (%) of the remote video.
   */
  int frameLossRate;
  /**
   * The packet loss rate (%) of the remote video after using the anti-packet-loss technology.
   */
  int packetLossRate;
  /**
   * The type of the video stream. See `VIDEO_STREAM_TYPE`.
   */
  VIDEO_STREAM_TYPE rxStreamType;
  /**
   * The total freeze time (ms) of the remote video stream after the remote user joins the channel. In
   * a video session where the frame rate is set to no less than 5 fps, video freeze occurs when the
   * time interval between two adjacent renderable video frames is more than 500 ms.
   */
  int totalFrozenTime;
  /**
   * The total video freeze time as a percentage (%) of the total time the video is available. The
   * video is considered available as long as that the remote user neither stops sending the video
   * stream nor disables the video module after joining the channel.
   */
  int frozenRate;
  /**
   * The amount of time (ms) that the audio is ahead of the video.
   * @note If this value is negative, the audio is lagging behind the video.
   */
  int avSyncTimeMs;
  /**
   * The total active time (ms) of the video.
   * As long as the remote user or host neither stops sending the video stream nor disables the video
   * module after joining the channel, the video is available.
   */
  int totalActiveTime;
  /**
   * The total duration (ms) of the remote video stream.
   */
  int publishDuration;
  /**
   * The quality of the remote video stream in the reported interval.
   * The quality is determined by the Agora real-time video MOS (Mean Opinion Score) measurement method.
   * The return value range is [0, 500].
   * Dividing the return value by 100 gets the MOS score, which ranges from 0 to 5. The higher the score, the better the video quality.
   * @note For textured video data, this parameter always returns 0.
   */
  int mosValue;
  /**
   * The total number of video bytes received (bytes), inluding the FEC bytes, represented by an aggregate value.
   */
  unsigned int rxVideoBytes;
};

struct VideoCompositingLayout {
  struct Region {
    /** User ID of the user whose video is to be displayed in the region.
     */
    uid_t uid;
    /** Horizontal position of the region on the screen.
     */
    double x;  // [0,1]
    /** Vertical position of the region on the screen.
     */
    double y;  // [0,1]
    /**
     Actual width of the region.
    */
    double width;  // [0,1]
    /** Actual height of the region. */
    double height;  // [0,1]
    /** 0 means the region is at the bottom, and 100 means the region is at the
     * top.
     */
    int zOrder;  // optional, [0, 100] //0 (default): bottom most, 100: top most

    /** 0 means the region is transparent, and 1 means the region is opaque. The
     * default value is 1.0.
     */
    double alpha;

    media::base::RENDER_MODE_TYPE renderMode;  // RENDER_MODE_HIDDEN: Crop, RENDER_MODE_FIT: Zoom to fit

    Region()
        : uid(0),
          x(0),
          y(0),
          width(0),
          height(0),
          zOrder(0),
          alpha(1.0),
          renderMode(media::base::RENDER_MODE_HIDDEN) {}
  };

  /** Ignore this parameter. The width of the canvas is set by
   agora::rtc::IRtcEngine::configPublisher, and not by
   agora::rtc::VideoCompositingLayout::canvasWidth.
  */
  int canvasWidth;
  /** Ignore this parameter. The height of the canvas is set by
   agora::rtc::IRtcEngine::configPublisher, and not by
   agora::rtc::VideoCompositingLayout::canvasHeight.
  */
  int canvasHeight;
  /** Enter any of the 6-digit symbols defined in RGB.
   */
  const char* backgroundColor;  // e.g. "#C0C0C0" in RGB
  /** Region array. Each host in the channel can have a region to display the
   * video on the screen.
   */
  const Region* regions;
  /** Number of users in the channel.
   */
  int regionCount;
  /** User-defined data.
   */
  const char* appData;
  /** Length of the user-defined data.
   */
  int appDataLength;

  VideoCompositingLayout()
      : canvasWidth(0),
        canvasHeight(0),
        backgroundColor(OPTIONAL_NULLPTR),
        regions(NULL),
        regionCount(0),
        appData(OPTIONAL_NULLPTR),
        appDataLength(0) {}
};

/** The definition of InjectStreamConfig.
 */
struct InjectStreamConfig {
  /** Width of the stream to be added into the broadcast. The default value is
  0; same width as the original stream.
  */
  int width;
  /** Height of the stream to be added into the broadcast. The default value is
  0; same height as the original stream.
  */
  int height;
  /** Video GOP of the stream to be added into the broadcast. The default value
  is 30.
  */
  int videoGop;
  /** Video frame rate of the stream to be added into the broadcast. The
  default value is 15 fps.
  */
  int videoFramerate;
  /** Video bitrate of the stream to be added into the broadcast. The default
  value is 400 Kbps.
  */
  int videoBitrate;
  /** Audio-sampling rate of the stream to be added into the broadcast:
  #AUDIO_SAMPLE_RATE_TYPE. The default value is 48000.
  */
  AUDIO_SAMPLE_RATE_TYPE audioSampleRate;
  /** Audio bitrate of the stream to be added into the broadcast. The default
  value is 48.
  */
  int audioBitrate;
  /** Audio channels to be added into the broadcast. The default value is 1.
  */
  int audioChannels;

  // width / height default set to 0 means pull the stream with its original
  // resolution
  InjectStreamConfig()
      : width(0),
        height(0),
        videoGop(30),
        videoFramerate(15),
        videoBitrate(400),
        audioSampleRate(AUDIO_SAMPLE_RATE_48000),
        audioBitrate(48),
        audioChannels(1) {}
};

/**
 * @brief Lifecycle of the CDN live video stream.
 */
enum RTMP_STREAM_LIFE_CYCLE_TYPE {
  /**
   * Bind to the channel lifecycle. If all hosts leave the channel, the CDN live streaming stops after
   * 30 seconds.
   */
  RTMP_STREAM_LIFE_CYCLE_BIND2CHANNEL = 1,
  /**
   * Bind to the owner of the RTMP stream. If the owner leaves the channel, the CDN live streaming
   * stops immediately.
   */
  RTMP_STREAM_LIFE_CYCLE_BIND2OWNER = 2,
};

/** The definition of PublisherConfiguration.
*/
struct PublisherConfiguration {
  /** Width of the output data stream set for CDN Live. The default value is
  360.
  */
  int width;
  /** Height of the output data stream set for CDN Live. The default value is
  640.
  */
  int height;
  /** Frame rate of the output data stream set for CDN Live. The default value
  is 15 fps.
  */
  int framerate;
  /** Bitrate of the output data stream set for CDN Live. The default value is
  500 Kbps.
  */
  int bitrate;
  /** The default layout:
  - 0: Tile horizontally
  - 1: Layered windows
  - 2: Tile vertically
  */
  int defaultLayout;
  /** The video stream lifecycle of CDN Live: RTMP_STREAM_LIFE_CYCLE_TYPE
  */
  int lifecycle;
  /** Whether the current user is the owner of the RTMP stream:
  - True: Yes (default). The push-stream configuration takes effect.
  - False: No. The push-stream configuration will not work.
  */
  bool owner;
  /** Width of the stream to be injected. Set it as 0.
  */
  int injectStreamWidth;
  /** Height of the stream to be injected. Set it as 0.
  */
  int injectStreamHeight;
  /** URL address of the stream to be injected to the channel.
  */
  const char* injectStreamUrl;
  /** Push-stream URL address for the picture-in-picture layouts. The default
  value is NULL.
  */
  const char* publishUrl;
  /** Push-stream URL address of the original stream which does not require
  picture-blending. The default value is NULL.
  */
  const char* rawStreamUrl;
  /** Reserved field. The default value is NULL.
  */
  const char* extraInfo;

  PublisherConfiguration()
      : width(640),
        height(360),
        framerate(15),
        bitrate(500),
        defaultLayout(1),
        lifecycle(RTMP_STREAM_LIFE_CYCLE_BIND2CHANNEL),
        owner(true),
        injectStreamWidth(0),
        injectStreamHeight(0),
        injectStreamUrl(NULL),
        publishUrl(NULL),
        rawStreamUrl(NULL),
        extraInfo(NULL) {}
};

/**
 * @brief The camera direction.
 */
enum CAMERA_DIRECTION {
  /**
   * 0: The rear camera.
   */
  CAMERA_REAR = 0,
  /**
   * 1: (Default) The front camera.
   */
  CAMERA_FRONT = 1,
};

/**
 * @brief The cloud proxy type.
 *
 * @since v3.3.0
 */
enum CLOUD_PROXY_TYPE {
  /**
   * 0: The automatic mode. The SDK has this mode enabled by default. In this mode, the SDK attempts a
   * direct connection to SD-RTN™ and automatically switches to TCP/TLS 443 if the attempt fails.
   */
  NONE_PROXY = 0,
  /**
   * 1: The cloud proxy for the UDP protocol, that is, the Force UDP cloud proxy mode. In this mode,
   * the SDK always transmits data over UDP.
   */
  UDP_PROXY = 1,
  /// @cond
  /**
   * 2: The cloud proxy for the TCP (encryption) protocol, that is, the Force TCP cloud proxy mode. In
   * this mode, the SDK always transmits data over TCP/TLS 443.
   */
  TCP_PROXY = 2,
  /// @endcond
};

/**
 * @brief The camera capturer preference.
 */
struct CameraCapturerConfiguration {
  /** Camera direction settings (for Android/iOS only). See: #CAMERA_DIRECTION. */
#if defined(__ANDROID__) || (defined(__APPLE__) && TARGET_OS_IOS) || defined(__OHOS__)
  /**
   * (Optional) The camera direction. See `CAMERA_DIRECTION`.
   * @note This parameter is for Android and iOS only.
   */
  Optional<CAMERA_DIRECTION> cameraDirection;

  /**
   * (Optional) The camera focal length type. See `CAMERA_FOCAL_LENGTH_TYPE`.
   * @note
   * - This parameter is for Android and iOS only.
   * - To set the focal length type of the camera, it is only supported to specify the camera through
   * `cameraDirection`, and not supported to specify it through `cameraId`.
   * - For iOS devices equipped with multi-lens rear cameras, such as those featuring dual-camera
   * (wide-angle and ultra-wide-angle) or triple-camera (wide-angle, ultra-wide-angle, and telephoto),
   * you can use one of the following methods to capture video with an ultra-wide-angle perspective:
   *   - Method one: Set this parameter to `CAMERA_FOCAL_LENGTH_ULTRA_WIDE` (2) (ultra-wide lens).
   *   - Method two: Set this parameter to `CAMERA_FOCAL_LENGTH_DEFAULT` (0) (standard lens), then
   * call `setCameraZoomFactor` to set the camera's zoom factor to a value less than 1.0, with the
   * minimum setting being 0.5.
   * The difference is that the size of the ultra-wide angle in method one is not adjustable, whereas
   * method two supports adjusting the camera's zoom factor freely.
   */
  Optional<CAMERA_FOCAL_LENGTH_TYPE> cameraFocalLengthType;
#else
  /**
   * The camera ID. The maximum length is `MAX_DEVICE_ID_LENGTH_TYPE`.
   * @note This parameter is for Windows and macOS only.
   */
  Optional<const char *> deviceId;
#endif

#if defined(__ANDROID__)
  /**
   * (Optional) The camera ID. The default value is the camera ID of the front camera. You can get the
   * camera ID through the Android native system API, see `Camera.open()` and
   * `CameraManager.getCameraIdList()` for details.
   * @note
   * - This parameter is for Android only.
   * - This parameter and `cameraDirection` are mutually exclusive in specifying the camera; you can
   * choose one based on your needs. The differences are as follows:
   *   - Specifying the camera via `cameraDirection` is more straightforward. You only need to
   * indicate the camera direction (front or rear), without specifying a specific camera ID; the SDK
   * will retrieve and confirm the actual camera ID through Android native system APIs.
   *   - Specifying via `cameraId` allows for more precise identification of a particular camera. For
   * devices with multiple cameras, where `cameraDirection` cannot recognize or access all available
   * cameras, it is recommended to use `cameraId` to specify the desired camera ID directly.
   */
  Optional<const char *> cameraId;
#endif
  /**
   * (Optional) Whether to follow the video aspect ratio set in `setVideoEncoderConfiguration`:
   * - `true`: (Default) Follow the set video aspect ratio. The SDK crops the captured video according
   * to the set video aspect ratio and synchronously changes the local preview screen and the video
   * frame in `onCaptureVideoFrame` and `onPreEncodeVideoFrame`.
   * - `false`: Do not follow the system default audio playback device. The SDK does not change the
   * aspect ratio of the captured video frame.
   */
  Optional<bool> followEncodeDimensionRatio;
  /**
   * (Optional) The format of the video frame. See `VideoFormat`.
   */
  VideoFormat format;
  CameraCapturerConfiguration() : format(VideoFormat(0, 0, 0)) {}
};
/**
 * @brief The configuration of the captured screen.
 */
struct ScreenCaptureConfiguration {
  /**
   * Whether to capture the window on the screen:
   * - `true`: Capture the window.
   * - `false`: (Default) Capture the screen, not the window.
   */
  bool isCaptureWindow; // true - capture window, false - capture display
  /**
   * (macOS only) The display ID of the screen.
   * @note This parameter takes effect only when you want to capture the screen on macOS.
   */
  int64_t displayId;
  /**
   * (Windows only) The relative position of the shared screen to the virtual screen.
   * @note This parameter takes effect only when you want to capture the screen on Windows.
   */
  Rectangle screenRect; //Windows only
  /**
   * (For Windows and macOS only) Window ID.
   * @note This parameter takes effect only when you want to capture the window.
   */
  int64_t windowId;
  /**
   * (For Windows and macOS only) The screen capture configuration. See `ScreenCaptureParameters`.
   */
  ScreenCaptureParameters params;
  /**
   * (For Windows and macOS only) The relative position of the shared region to the whole screen. See
   * `Rectangle`. If you do not set this parameter, the SDK shares the whole screen. If the region you
   * set exceeds the boundary of the screen, only the region within in the screen is shared. If you
   * set width or height in `Rectangle` as 0, the whole screen is shared.
   */
  Rectangle regionRect;

  ScreenCaptureConfiguration() : isCaptureWindow(false), displayId(0), windowId(0) {}
};

#if (defined(__APPLE__) && TARGET_OS_MAC && !TARGET_OS_IPHONE)|| (defined(__linux__) && !defined(__ANDROID__) && !defined(__OHOS__))
/** The size of the screen shot to the screen or window.
 */
struct SIZE {
  /** The width of the screen shot.
   */
  int width;
  /** The width of the screen shot.
   */
  int height;

  SIZE() : width(0), height(0) {}
  SIZE(int ww, int hh) : width(ww), height(hh) {}
};
#endif

#if defined(_WIN32) || (defined(__APPLE__) && TARGET_OS_MAC && !TARGET_OS_IPHONE) || (defined(__linux__) && !defined(__ANDROID__) && !defined(__OHOS__))
/**
 * @brief The image content of the thumbnail or icon. Set in `ScreenCaptureSourceInfo`.
 *
 * @note The default image is in the ARGB format. If you need to use another format, you need to
 * convert the image on your own.
 *
 */
struct ThumbImageBuffer {
  /**
   * The buffer of the thumbnail or icon.
   */
  const char* buffer;
  /**
   * The buffer length of the thumbnail or icon, in bytes.
   */
  unsigned int length;
  /**
   * The actual width (px) of the thumbnail or icon.
   */
  unsigned int width;
  /**
   * The actual height (px) of the thumbnail or icon.
   */
  unsigned int height;
  ThumbImageBuffer() : buffer(nullptr), length(0), width(0), height(0) {}
};
/**
 * @brief The type of the shared target. Set in `ScreenCaptureSourceInfo`.
 */
enum ScreenCaptureSourceType {
  /**
   * -1: Unknown type.
   */
  ScreenCaptureSourceType_Unknown = -1,
  /**
   * 0: The shared target is a window.
   */
  ScreenCaptureSourceType_Window = 0,
  /**
   * 1: The shared target is a screen of a particular monitor.
   */
  ScreenCaptureSourceType_Screen = 1,
  /**
   * 2: Reserved parameter.
   */
  ScreenCaptureSourceType_Custom = 2,
};
/**
 * @brief The information about the specified shareable window or screen. The information is
 * returned in `IScreenCaptureSourceList`.
 */
struct ScreenCaptureSourceInfo {
  /**
   * The type of the shared target. See `ScreenCaptureSourceType`.
   */
  ScreenCaptureSourceType type;
  /**
   * The window ID for a window or the display ID for a screen.
   */
  int64_t sourceId;
  /**
   * The name of the window or screen. UTF-8 encoding.
   */
  const char* sourceName;
  /**
   * The image content of the thumbnail. See `ThumbImageBuffer`.
   */
  ThumbImageBuffer thumbImage;
  /**
   * The image content of the icon. See `ThumbImageBuffer`.
   */
  ThumbImageBuffer iconImage;
  /**
   * The process to which the window belongs. UTF-8 encoding.
   */
  const char* processPath;
  /**
   * The title of the window. UTF-8 encoding.
   */
  const char* sourceTitle;
  /**
   * Determines whether the screen is the primary display:
   * - `true`: The screen is the primary display.
   * - `false`: The screen is not the primary display.
   */
  bool primaryMonitor;
  bool isOccluded;
  /**
   * The position of a window relative to the entire screen space (including all shareable screens).
   * See `Rectangle`.
   */
  Rectangle position;
#if defined(_WIN32)
  /**
   * (For Windows only) Whether the window is minimized:
   * - `true`: The window is minimized.
   * - `false`: The window is not minimized.
   */
  bool minimizeWindow;
  /**
   * (For Windows only) Screen ID where the window is located. If the window is displayed across
   * multiple screens, this parameter indicates the ID of the screen with which the window has the
   * largest intersection area. If the window is located outside of the visible screens, the value of
   * this member is -2.
   */
  int64_t sourceDisplayId;
  ScreenCaptureSourceInfo() : type(ScreenCaptureSourceType_Unknown), sourceId(0), sourceName(nullptr),
                              processPath(nullptr), sourceTitle(nullptr), primaryMonitor(false), isOccluded(false), minimizeWindow(false), sourceDisplayId(-2) {}
#else
  ScreenCaptureSourceInfo() : type(ScreenCaptureSourceType_Unknown), sourceId(0), sourceName(nullptr), processPath(nullptr), sourceTitle(nullptr), primaryMonitor(false), isOccluded(false) {}
#endif
};
/**
 * The IScreenCaptureSourceList class. This class is returned in the getScreenCaptureSources method.
 */
class IScreenCaptureSourceList {
 protected:
  virtual ~IScreenCaptureSourceList(){};

 public:
  /**
   * @brief Gets the number of shareable windows and screens.
   *
   * @note This method applies to macOS and Windows only.
   *
   * @return
   * The number of shareable windows and screens.
   */
  virtual unsigned int getCount() = 0;
  /**
   * @brief Gets information about the specified shareable window or screen.
   *
   * @details
   * After you get `IScreenCaptureSourceList`, you can pass in the index value of the specified
   * shareable window or screen to get information about that window or screen from
   * `ScreenCaptureSourceInfo`.
   *
   * @note This method applies to macOS and Windows only.
   *
   * @param index The index of the specified shareable window or screen. The value range is [0,
   * `getCount` `()`).
   *
   * @return
   * `ScreenCaptureSourceInfo`
   */
  virtual ScreenCaptureSourceInfo getSourceInfo(unsigned int index) = 0;
  /**
   * @brief Releases `IScreenCaptureSourceList`.
   *
   * @details
   * After you get the list of shareable windows and screens, to avoid memory leaks, call this method
   * to release `IScreenCaptureSourceList` instead of deleting `IScreenCaptureSourceList` directly.
   *
   * @note This method applies to macOS and Windows only.
   *
   */
  virtual void release() = 0;
};
#endif // _WIN32 || (__APPLE__ && !TARGET_OS_IPHONE && TARGET_OS_MAC)
/**
 * @brief The advanced options for audio.
 */
struct AdvancedAudioOptions {
   /**
    * The number of channels for audio preprocessing:
    * - 1: Mono.
    * - 2: Stereo.
    */
   Optional<int> audioProcessingChannels;

   AdvancedAudioOptions() {}
  ~AdvancedAudioOptions() {}
};

/**
 * @brief Image configurations.
 */
struct ImageTrackOptions {
  /**
   * The image URL. Supported formats of images include JPEG, JPG, PNG and GIF. This method supports
   * adding an image from the local absolute or relative file path.
   * @note On the Android platform, adding images from `/assets/` is not supported.
   */
  const char* imageUrl;
  /**
   * The frame rate of the video streams being published. The value range is [1,30]. The default value
   * is 1.
   */
  int fps;
  VIDEO_MIRROR_MODE_TYPE mirrorMode;
  ImageTrackOptions() : imageUrl(NULL), fps(1), mirrorMode(VIDEO_MIRROR_MODE_DISABLED) {}
};

/**
 * @brief The channel media options.
 *
 * @details
 * Agora supports publishing multiple audio streams and one video stream at the same time and in the
 * same `RtcConnection`. For example, `publishMicrophoneTrack`, `publishCustomAudioTrack`, and
 * `publishMediaPlayerAudioTrack` can be set as `true` at the same time, but only one of
 * `publishCameraTrack`, `publishScreenCaptureVideo`, `publishScreenTrack`,
 * `publishCustomVideoTrack`, or `publishEncodedVideoTrack` can be set as `true`.
 *
 * @note Agora recommends that you set member parameter values yourself according to your business
 * scenario, otherwise the SDK will automatically assign values to member parameters.
 *
 */
struct ChannelMediaOptions {
  /**
   * Whether to publish the video captured by the camera:
   * - `true`: Publish the video captured by the camera.
   * - `false`: Do not publish the video captured by the camera.
   */
  Optional<bool> publishCameraTrack;
  /**
   * Whether to publish the video captured by the second camera:
   * - `true`: Publish the video captured by the second camera.
   * - `false`: Do not publish the video captured by the second camera.
   */
  Optional<bool> publishSecondaryCameraTrack;
  /**
   * Whether to publish the video captured by the third camera:
   * - `true`: Publish the video captured by the third camera.
   * - `false`: Do not publish the video captured by the third camera.
   * @note This parameter is for Android, Windows and macOS only.
   */
  Optional<bool> publishThirdCameraTrack;
  /**
   * Whether to publish the video captured by the fourth camera:
   * - `true`: Publish the video captured by the fourth camera.
   * - `false`: Do not publish the video captured by the fourth camera.
   * @note This parameter is for Android, Windows and macOS only.
   */
  Optional<bool> publishFourthCameraTrack;
  /**
   * Whether to publish the audio captured by the microphone:
   * - `true`: Publish the audio captured by the microphone.
   * - `false`: Do not publish the audio captured by the microphone.
   */
  Optional<bool> publishMicrophoneTrack;
  #if defined(__ANDROID__) || (defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE) || defined(TARGET_OS_MAC) || defined(__OHOS__)
  /**
   * Whether to publish the audio track of the screen capturer:
   * - `true`: Publish the video audio of the screen capturer.
   * - `false`: (Default) Do not publish the audio track of the screen capturer.
   */
   Optional<bool> publishScreenCaptureAudio;
  #endif
  #if defined(__ANDROID__) || (defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE) || defined(__OHOS__)
  /**
   * Whether to publish the video captured from the screen:
   * - `true`: Publish the video captured from the screen.
   * - `false`: Do not publish the video captured from the screen.
   * @note This parameter is for Android and iOS only.
   */
  Optional<bool> publishScreenCaptureVideo;

  #else
  /**
   * Whether to publish the video captured from the screen:
   * - `true`: Publish the video captured from the screen.
   * - `false`: Do not publish the video captured from the screen.
   * @note This is for Windows and macOS only.
   */
  Optional<bool> publishScreenTrack;
  /**
   * Whether to publish the video captured from the second screen:
   * - `true`: Publish the video captured from the second screen.
   * - `false`: Do not publish the video captured from the second screen.
   */
  Optional<bool> publishSecondaryScreenTrack;
  /**
   * Whether to publish the video captured from the third screen:
   * - `true`: Publish the captured video from the third screen.
   * - `false`: Do not publish the video captured from the third screen.
   * @note This is for Windows and macOS only.
   */
  Optional<bool> publishThirdScreenTrack;
  /**
   * Whether to publish the video captured from the fourth screen:
   * - `true`: Publish the captured video from the fourth screen.
   * - `false`: Do not publish the video captured from the fourth screen.
   * @note This is for Windows and macOS only.
   */
  Optional<bool> publishFourthScreenTrack;
  #endif

  /**
   * Whether to publish the audio captured from a custom source:
   * - `true`: Publish the audio captured from the custom source.
   * - `false`: Do not publish the captured audio from a custom source.
   */
  Optional<bool> publishCustomAudioTrack;
  /**
   * The ID of the custom audio track to be published. The default value is 0. You can obtain the
   * custom audio track ID through the `createCustomAudioTrack` method.
   */
  Optional<int> publishCustomAudioTrackId;
  /**
   * Whether to publish the video captured from a custom source:
   * - `true`: Publish the video captured from the custom source.
   * - `false`: Do not publish the captured video from a custom source.
   */
  Optional<bool> publishCustomVideoTrack;
  /**
   * Whether to publish the encoded video:
   * - `true`: Publish the encoded video.
   * - `false`: Do not publish the encoded video.
   */
  Optional<bool> publishEncodedVideoTrack;
  /**
   * Whether to publish the audio from the media player:
   * - `true`: Publish the audio from the media player.
   * - `false`: Do not publish the audio from the media player.
   */
  Optional<bool> publishMediaPlayerAudioTrack;
  /**
   * Whether to publish the video from the media player:
   * - `true`: Publish the video from the media player.
   * - `false`: Do not publish the video from the media player.
   */
  Optional<bool> publishMediaPlayerVideoTrack;
  /**
   * Whether to publish the local transcoded video:
   * - `true`: Publish the local transcoded video.
   * - `false`: Do not publish the local transcoded video.
   * @note As of v4.2.0, the parameter name is corrected from `publishTrancodedVideoTrack` to
   * `publishTranscodedVideoTrack`.
   */
  Optional<bool> publishTranscodedVideoTrack;
  /**
   * Whether to publish the mixed audio track:
   * - `true`: Publish the mixed audio track.
   * - `false`: Do not publish the mixed audio track.
   */
  Optional<bool> publishMixedAudioTrack;
  /**
   * Whether to publish the video track processed by the speech driven extension:
   * - `true`: Publish the video track processed by the speech driven extension.
   * - `false`: (Default) Do not publish the video track processed by the speech driven extension.
   */
  Optional<bool> publishLipSyncTrack;
  /**
   * Whether to automatically subscribe to all remote audio streams when the user joins a channel:
   * - `true`: Subscribe to all remote audio streams.
   * - `false`: Do not automatically subscribe to any remote audio streams.
   */
  Optional<bool> autoSubscribeAudio;
  /**
   * Whether to automatically subscribe to all remote video streams when the user joins the channel:
   * - `true`: Subscribe to all remote video streams.
   * - `false`: Do not automatically subscribe to any remote video streams.
   */
  Optional<bool> autoSubscribeVideo;
  /**
   * Whether to enable audio capturing or playback:
   * - `true`: Enable audio capturing or playback.
   * - `false`: Do not enable audio capturing or playback.
   * @note If you need to publish the audio streams captured by your microphone, ensure this parameter
   * is set as `true`.
   */
  Optional<bool> enableAudioRecordingOrPlayout;
  /**
   * The ID of the media player to be published. The default value is 0.
   */
  Optional<int> publishMediaPlayerId;
  /**
   * The user role. See `CLIENT_ROLE_TYPE`.
   * @note If you set the user role as an audience member, you cannot publish audio and video streams
   * in the channel. If you want to publish media streams in a channel during live streaming, ensure
   * you set the user role as broadcaster.
   */
  Optional<CLIENT_ROLE_TYPE> clientRoleType;
  /**
   * The latency level of an audience member in interactive live streaming. See
   * `AUDIENCE_LATENCY_LEVEL_TYPE`.
   */
  Optional<AUDIENCE_LATENCY_LEVEL_TYPE> audienceLatencyLevel;
  /**
   * The default video-stream type. See `VIDEO_STREAM_TYPE`.
   */
  Optional<VIDEO_STREAM_TYPE> defaultVideoStreamType;
  /**
   * The channel profile. See `CHANNEL_PROFILE_TYPE`.
   */
  Optional<CHANNEL_PROFILE_TYPE> channelProfile;
  /**
   * Delay (in milliseconds) for sending audio frames. You can use this parameter to set the delay of
   * the audio frames that need to be sent, to ensure audio and video synchronization.
   * To switch off the delay, set the value to 0.
   */
  Optional<int> audioDelayMs;
  /**
   * The delay in ms for sending media player audio frames. This is used for explicit control of A/V sync.
   * To switch off the delay, set the value to zero.
   */
  Optional<int> mediaPlayerAudioDelayMs;
  /**
   * (Optional) The token generated on your server for authentication. See .
   * @note
   * - This parameter takes effect only when calling `updateChannelMediaOptions` or
   * `updateChannelMediaOptionsEx`.
   * - Ensure that the App ID, channel name, and user name used for creating the token are the same as
   * those used by the `initialize` method for initializing the RTC engine, and those used by the
   * `joinChannel(const char* token, const char* channelId, uid_t uid, const ChannelMediaOptions&
   * options)` and `joinChannelEx` methods for joining the channel.
   */
  Optional<const char*> token;
  /**
   * Whether to enable media packet encryption:
   * - `true`: Yes.
   * - `false`: (Default) No.
   *
   * @note This parameter is ignored when calling `updateChannelMediaOptions`.
   */
  Optional<bool> enableBuiltInMediaEncryption;
  /**
   * Whether to publish the sound of a metronome to remote users:
   * - `true`: Publish processed audio frames. Both the local user and remote users can hear the
   * metronome.
   * - `false`: Do not publish the sound of the metronome. Only the local user can hear the metronome.
   */
  Optional<bool> publishRhythmPlayerTrack;
  /**
   * Whether to enable interactive mode:
   * - `true`: Enable interactive mode. Once this mode is enabled and the user role is set as
   * audience, the user can receive remote video streams with low latency.
   * - `false`:Do not enable interactive mode. If this mode is disabled, the user receives the remote
   * video streams in default settings.
   * @note
   * - This parameter only applies to co-streaming scenarios. The cohosts need to call the
   * `joinChannelEx` method to join the other host's channel as an audience member, and set
   * `isInteractiveAudience` to `true`.
   * - This parameter takes effect only when the user role is `CLIENT_ROLE_AUDIENCE`.
   */
  Optional<bool> isInteractiveAudience;
  /**
   * The video track ID returned by calling the `createCustomVideoTrack` method. The default value is
   * 0.
   */
  Optional<video_track_id_t> customVideoTrackId;
  /**
   * Whether the audio stream being published is filtered according to the volume algorithm:
   * - `true`: The audio stream is filtered. If the audio stream filter is not enabled, this setting
   * does not takes effect.
   * - `false`: The audio stream is not filtered.
   * @note If you need to enable this function, contact `support@agora.io`.
   */
  Optional<bool> isAudioFilterable;

  /** Provides the technical preview functionalities or special customizations by configuring the SDK with JSON options.
      Pointer to the set parameters in a JSON string.
    * @technical preview
   */
  Optional<const char*> parameters;

  /**
   * Whether to enable multiple transmisssion paths:
   * - `true`: Enable multiple transmisssion path.
   * - `false`: Dsiable multiple transmisssion path.
   * @note Permissions and system requirements: Android: Android 7.0 or higher (API level 24 or
   * higher), and the ACCESS_NETWORK_STATE and CHANGE_NETWORK_STATE permissions are required. iOS: iOS
   * 12.0 or later. macOS: 10.14 or later. Windows: Windows Vista or higher.
   * @since 4.6.0
   */
  Optional<bool> enableMultipath;

  /**
   * Uplink transmission mode. See `MultipathMode`.
   * @note When using this parameter, make sure that `enableMultipath` is set to `true`.
   * @since 4.6.0
   */
  Optional<MultipathMode> uplinkMultipathMode;

  /**
   * Downlink transmission mode. See `MultipathMode`.
   * @note When using this parameter, make sure that `enableMultipath` is set to `true`.
   * @since 4.6.0
   */
  Optional<MultipathMode> downlinkMultipathMode;

  /**
   * Preferred type of transmission path. See `MultipathType`.
   * @note When using this parameter, make sure that `enableMultipath` is set to `true`.
   * @since 4.6.0
   */
  Optional<MultipathType> preferMultipathType;

  ChannelMediaOptions() {}
  ~ChannelMediaOptions() {}

  void SetAll(const ChannelMediaOptions& change) {
#define SET_FROM(X) SetFrom(&X, change.X)

      SET_FROM(publishCameraTrack);
      SET_FROM(publishSecondaryCameraTrack);
      SET_FROM(publishThirdCameraTrack);
      SET_FROM(publishFourthCameraTrack);
      SET_FROM(publishMicrophoneTrack);
#if defined(__ANDROID__) || (defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE) || defined(TARGET_OS_MAC) || defined(__OHOS__)
      SET_FROM(publishScreenCaptureAudio);
#endif
#if defined(__ANDROID__) || (defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE) || defined(__OHOS__)
      SET_FROM(publishScreenCaptureVideo);
#else
      SET_FROM(publishScreenTrack);
      SET_FROM(publishSecondaryScreenTrack);
      SET_FROM(publishThirdScreenTrack);
      SET_FROM(publishFourthScreenTrack);
#endif
      SET_FROM(publishTranscodedVideoTrack);
      SET_FROM(publishMixedAudioTrack);
      SET_FROM(publishLipSyncTrack);
      SET_FROM(publishCustomAudioTrack);
      SET_FROM(publishCustomAudioTrackId);
      SET_FROM(publishCustomVideoTrack);
      SET_FROM(publishEncodedVideoTrack);
      SET_FROM(publishMediaPlayerAudioTrack);
      SET_FROM(publishMediaPlayerVideoTrack);
      SET_FROM(autoSubscribeAudio);
      SET_FROM(autoSubscribeVideo);
      SET_FROM(publishMediaPlayerId);
      SET_FROM(enableAudioRecordingOrPlayout);
      SET_FROM(clientRoleType);
      SET_FROM(audienceLatencyLevel);
      SET_FROM(defaultVideoStreamType);
      SET_FROM(channelProfile);
      SET_FROM(audioDelayMs);
      SET_FROM(mediaPlayerAudioDelayMs);
      SET_FROM(token);
      SET_FROM(enableBuiltInMediaEncryption);
      SET_FROM(publishRhythmPlayerTrack);
      SET_FROM(customVideoTrackId);
      SET_FROM(isAudioFilterable);
      SET_FROM(isInteractiveAudience);
      SET_FROM(parameters);
      SET_FROM(enableMultipath);
      SET_FROM(uplinkMultipathMode);
      SET_FROM(downlinkMultipathMode);
      SET_FROM(preferMultipathType);
#undef SET_FROM
  }

  bool operator==(const ChannelMediaOptions& o) const {
#define BEGIN_COMPARE() bool b = true
#define ADD_COMPARE(X) b = (b && (X == o.X))
#define END_COMPARE()

      BEGIN_COMPARE();
      ADD_COMPARE(publishCameraTrack);
      ADD_COMPARE(publishSecondaryCameraTrack);
      ADD_COMPARE(publishThirdCameraTrack);
      ADD_COMPARE(publishFourthCameraTrack);
      ADD_COMPARE(publishMicrophoneTrack);
#if defined(__ANDROID__) || (defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE) || defined(TARGET_OS_MAC) || defined(__OHOS__)
      ADD_COMPARE(publishScreenCaptureAudio);
#endif
#if defined(__ANDROID__) || (defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE) || defined(__OHOS__)
      ADD_COMPARE(publishScreenCaptureVideo);
#else
      ADD_COMPARE(publishScreenTrack);
      ADD_COMPARE(publishSecondaryScreenTrack);
      ADD_COMPARE(publishThirdScreenTrack);
      ADD_COMPARE(publishFourthScreenTrack);
#endif
      ADD_COMPARE(publishTranscodedVideoTrack);
      ADD_COMPARE(publishMixedAudioTrack);
      ADD_COMPARE(publishLipSyncTrack);
      ADD_COMPARE(publishCustomAudioTrack);
      ADD_COMPARE(publishCustomAudioTrackId);
      ADD_COMPARE(publishCustomVideoTrack);
      ADD_COMPARE(publishEncodedVideoTrack);
      ADD_COMPARE(publishMediaPlayerAudioTrack);
      ADD_COMPARE(publishMediaPlayerVideoTrack);
      ADD_COMPARE(autoSubscribeAudio);
      ADD_COMPARE(autoSubscribeVideo);
      ADD_COMPARE(publishMediaPlayerId);
      ADD_COMPARE(enableAudioRecordingOrPlayout);
      ADD_COMPARE(clientRoleType);
      ADD_COMPARE(audienceLatencyLevel);
      ADD_COMPARE(defaultVideoStreamType);
      ADD_COMPARE(channelProfile);
      ADD_COMPARE(audioDelayMs);
      ADD_COMPARE(mediaPlayerAudioDelayMs);
      ADD_COMPARE(token);
      ADD_COMPARE(enableBuiltInMediaEncryption);
      ADD_COMPARE(publishRhythmPlayerTrack);
      ADD_COMPARE(customVideoTrackId);
      ADD_COMPARE(isAudioFilterable);
      ADD_COMPARE(isInteractiveAudience);
      ADD_COMPARE(parameters);
      ADD_COMPARE(enableMultipath);
      ADD_COMPARE(uplinkMultipathMode);
      ADD_COMPARE(downlinkMultipathMode);
      ADD_COMPARE(preferMultipathType);
      END_COMPARE();

#undef BEGIN_COMPARE
#undef ADD_COMPARE
#undef END_COMPARE
      return b;
  }

  ChannelMediaOptions& operator=(const ChannelMediaOptions& replace) {
    if (this != &replace) {
#define REPLACE_BY(X) ReplaceBy(&X, replace.X)

        REPLACE_BY(publishCameraTrack);
        REPLACE_BY(publishSecondaryCameraTrack);
        REPLACE_BY(publishThirdCameraTrack);
        REPLACE_BY(publishFourthCameraTrack);
        REPLACE_BY(publishMicrophoneTrack);
#if defined(__ANDROID__) || (defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE) || defined(TARGET_OS_MAC) || defined(__OHOS__)
        REPLACE_BY(publishScreenCaptureAudio);
#endif
#if defined(__ANDROID__) || (defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE) || defined(__OHOS__)
        REPLACE_BY(publishScreenCaptureVideo);
#else
        REPLACE_BY(publishScreenTrack);
        REPLACE_BY(publishSecondaryScreenTrack);
        REPLACE_BY(publishThirdScreenTrack);
        REPLACE_BY(publishFourthScreenTrack);
#endif
        REPLACE_BY(publishTranscodedVideoTrack);
        REPLACE_BY(publishMixedAudioTrack);
        REPLACE_BY(publishLipSyncTrack);
        REPLACE_BY(publishCustomAudioTrack);
        REPLACE_BY(publishCustomAudioTrackId);
        REPLACE_BY(publishCustomVideoTrack);
        REPLACE_BY(publishEncodedVideoTrack);
        REPLACE_BY(publishMediaPlayerAudioTrack);
        REPLACE_BY(publishMediaPlayerVideoTrack);
        REPLACE_BY(autoSubscribeAudio);
        REPLACE_BY(autoSubscribeVideo);
        REPLACE_BY(publishMediaPlayerId);
        REPLACE_BY(enableAudioRecordingOrPlayout);
        REPLACE_BY(clientRoleType);
        REPLACE_BY(audienceLatencyLevel);
        REPLACE_BY(defaultVideoStreamType);
        REPLACE_BY(channelProfile);
        REPLACE_BY(audioDelayMs);
        REPLACE_BY(mediaPlayerAudioDelayMs);
        REPLACE_BY(token);
        REPLACE_BY(enableBuiltInMediaEncryption);
        REPLACE_BY(publishRhythmPlayerTrack);
        REPLACE_BY(customVideoTrackId);
        REPLACE_BY(isAudioFilterable);
        REPLACE_BY(isInteractiveAudience);
        REPLACE_BY(parameters);
        REPLACE_BY(enableMultipath);
        REPLACE_BY(uplinkMultipathMode);
        REPLACE_BY(downlinkMultipathMode);
        REPLACE_BY(preferMultipathType);
#undef REPLACE_BY
    }
    return *this;
  }
};

/**
 * @brief The cloud proxy type.
 */
enum PROXY_TYPE {
  /**
   * 0: Reserved for future use.
   */
  NONE_PROXY_TYPE = 0,
  /**
   * 1: The cloud proxy for the UDP protocol, that is, the Force UDP cloud proxy mode. In this mode,
   * the SDK always transmits data over UDP.
   */
  UDP_PROXY_TYPE = 1,
  /**
   * 2: The cloud proxy for the TCP (encryption) protocol, that is, the Force TCP cloud proxy mode. In
   * this mode, the SDK always transmits data over TCP/TLS 443.
   */
  TCP_PROXY_TYPE = 2,
  /**
   * 3: Reserved for future use.
   */
  LOCAL_PROXY_TYPE = 3,
  /**
   * 4: Automatic mode. In this mode, the SDK attempts a direct connection to SD-RTN™ and
   * automatically switches to TCP/TLS 443 if the attempt fails.
   */
  TCP_PROXY_AUTO_FALLBACK_TYPE = 4,
  /** 5: The http proxy.
   */
  HTTP_PROXY_TYPE = 5,
  /** 6: The https proxy.
   */
  HTTPS_PROXY_TYPE = 6,
};

/**
 * @brief The type of the advanced feature.
 */
enum FeatureType {
  /**
   * 1: Virtual background.
   */
  VIDEO_VIRTUAL_BACKGROUND = 1,
  /**
   * 2: Image enhancement.
   */
  VIDEO_BEAUTY_EFFECT = 2,
};

/**
 * @brief The options for leaving a channel.
 */
struct LeaveChannelOptions {
  /**
   * Whether to stop playing and mixing the music file when a user leaves the channel.
   * - `true`: (Default) Stop playing and mixing the music file.
   * - `false`: Do not stop playing and mixing the music file.
   */
  bool stopAudioMixing;
  /**
   * Whether to stop playing all audio effects when a user leaves the channel.
   * - `true`: (Default) Stop playing all audio effects.
   * - `false`: Do not stop playing any audio effect.
   */
  bool stopAllEffect;
  /**
   * Whether to stop microphone recording when a user leaves the channel.
   * - `true`: (Default) Stop microphone recording.
   * - `false`: Do not stop microphone recording.
   */
  bool stopMicrophoneRecording;

  LeaveChannelOptions() : stopAudioMixing(true), stopAllEffect(true), stopMicrophoneRecording(true) {}
};

/**
 * The IRtcEngineEventHandler class.
 *
 * The SDK uses this class to send callback event notifications to the app, and the app inherits
 * the methods in this class to retrieve these event notifications.
 *
 * All methods in this class have their default (empty)  implementations, and the app can inherit
 * only some of the required events instead of all. In the callback methods, the app should avoid
 * time-consuming tasks or calling blocking APIs, otherwise the SDK may not work properly.
 */
class IRtcEngineEventHandler {
 public:
  virtual ~IRtcEngineEventHandler() {}

  virtual const char* eventHandlerType() const { return "event_handler"; }

  /**
   * @brief Occurs when a user joins a channel.
   *
   * @details
   * This callback notifies the application that a user joins a specified channel.
   * Call timing: The SDK triggers this callback when you call `joinChannel(const char* token, const
   * char* channelId, const char* info, uid_t uid)`, `joinChannel(const char* token, const char*
   * channelId, uid_t uid, const ChannelMediaOptions& options)`
   * , `joinChannelWithUserAccount(const char* token, const char* channelId, const char*
   * userAccount)`, `joinChannelWithUserAccount(const char* token, const char* channelId, const char*
   * userAccount, const ChannelMediaOptions& options)` , `joinChannelEx`
   * or `joinChannelWithUserAccountEx` to join a channel.
   *
   * @param channel The channel name.
   * @param uid The ID of the user who joins the channel.
   * @param elapsed The time elapsed (ms) from the local user calling `joinChannel(const char* token,
   * const char* channelId, uid_t uid, const ChannelMediaOptions& options)` until the
   * SDK triggers this callback.
   *
   */
  virtual void onJoinChannelSuccess(const char* channel, uid_t uid, int elapsed) {
    (void)channel;
    (void)uid;
    (void)elapsed;
  }

  /**
   * @brief Occurs when a user rejoins the channel.
   *
   * @details
   * Call timing: When a user loses connection with the server because of network problems, the SDK
   * automatically tries to reconnect and triggers this callback upon reconnection.
   *
   * @param channel The channel name.
   * @param uid The ID of the user who rejoins the channel.
   * @param elapsed Time elapsed (ms) from the local user calling `joinChannel(const char* token,
   * const char* channelId, uid_t uid, const ChannelMediaOptions& options)` until the SDK
   * triggers this callback.
   *
   */
  virtual void onRejoinChannelSuccess(const char* channel, uid_t uid, int elapsed) {
    (void)channel;
    (void)uid;
    (void)elapsed;
  }

  /**
   * @brief Reports the proxy connection state.
   *
   * @details
   * You can use this callback to listen for the state of the SDK connecting to a proxy. For example,
   * when a user calls `setCloudProxy` and joins a channel successfully, the SDK triggers this
   * callback to report the user ID, the proxy type connected, and the time elapsed fromthe user
   * calling `joinChannel(const char* token, const char* channelId, uid_t uid, const
   * ChannelMediaOptions& options)` until this callback is triggered.
   *
   * @param channel The channel name.
   * @param uid The user ID.
   * @param proxyType The proxy type connected. See `PROXY_TYPE`.
   * @param localProxyIp Reserved for future use.
   * @param elapsed The time elapsed (ms) from the user calling `joinChannel(const char* token, const
   * char* channelId, uid_t uid, const ChannelMediaOptions& options)` until this
   * callback is triggered.
   *
   */
  virtual void onProxyConnected(const char* channel, uid_t uid, PROXY_TYPE proxyType, const char* localProxyIp, int elapsed) {
    (void)channel;
    (void)uid;
    (void)proxyType;
    (void)localProxyIp;
    (void)elapsed;
  }

  /**
   * @brief Reports an error during SDK runtime.
   *
   * @details
   * This callback indicates that an error (concerning network or media) occurs during SDK runtime. In
   * most cases, the SDK cannot fix the issue and resume running. The SDK requires the app to take
   * action or informs the user about the issue.
   *
   * @param err Error code. See `ERROR_CODE_TYPE`.
   * @param msg The error message.
   *
   */
  virtual void onError(int err, const char* msg) {
    (void)err;
    (void)msg;
  }

  /**
   * @brief Reports the statistics of the audio stream sent by each remote user.
   *
   * @deprecated This callback is deprecated. Use onRemoteAudioStats instead.
   *
   * @details
   * The SDK triggers this callback once every two seconds to report the audio quality of each remote
   * user who is sending an audio stream. If a channel has multiple users sending audio streams, the
   * SDK triggers this callback as many times.
   *
   * @param uid The user ID of the remote user sending the audio stream.
   * @param quality Audio quality of the user. See `QUALITY_TYPE`.
   * @param delay The network delay (ms) from the sender to the receiver, including the delay caused
   * by audio sampling pre-processing, network transmission, and network jitter buffering.
   * @param lost The packet loss rate (%) of the audio packet sent from the remote user to the
   * receiver.
   *
   */
  virtual void onAudioQuality(uid_t uid, int quality, unsigned short delay, unsigned short lost) __deprecated {
    (void)uid;
    (void)quality;
    (void)delay;
    (void)lost;
  }

  /**
   * @brief Reports the last mile network probe result.
   *
   * @details
   * The SDK triggers this callback within 30 seconds after the app calls `startLastmileProbeTest`.
   *
   * @param result The uplink and downlink last-mile network probe test result. See
   * `LastmileProbeResult`.
   *
   */
  virtual void onLastmileProbeResult(const LastmileProbeResult& result) {
      (void)result;
  }

  /**
   * @brief Reports the volume information of users.
   *
   * @details
   * By default, this callback is disabled. You can enable it by calling
   * `enableAudioVolumeIndication`. Once this callback is enabled and users send streams in the
   * channel, the SDK triggers the `onAudioVolumeIndication` callback according to the time interval
   * set in `enableAudioVolumeIndication`. The SDK triggers two independent `onAudioVolumeIndication`
   * callbacks simultaneously, which separately report the volume information of the local user who
   * sends a stream and the remote users (up to three) whose instantaneous volume is the highest.
   *
   * @note
   * Once this callback is enabled, if the local user calls the `muteLocalAudioStream` method to mute,
   * the SDK continues to report the volume indication of the local user.
   * If a remote user whose volume is one of the three highest in the channel stops publishing the
   * audio stream for 20 seconds, the callback excludes this user's information; if all remote users
   * stop publishing audio streams for 20 seconds, the SDK stops triggering the callback for remote
   * users.
   *
   * @param speakers The volume information of the users. See `AudioVolumeInfo`. An empty `speakers`
   * array in the callback indicates that no remote user is in the channel or is sending a stream.
   * @param speakerNumber The total number of users.
   * - In the callback for the local user, if the local user is sending streams, the value of
   * `speakerNumber` is 1.
   * - In the callback for remote users, the value range of `speakerNumber` is [0,3]. If the number of
   * remote users who send streams is greater than or equal to three, the value of `speakerNumber` is
   * 3.
   * @param totalVolume The volume of the speaker. The value range is [0,255].
   * - In the callback for the local user, `totalVolume` is the volume of the local user who sends a
   * stream.
   * - In the callback for remote users, `totalVolume` is the sum of the volume of all remote users
   * (up to three) whose instantaneous volume is the highest. If the user calls
   * `startAudioMixing(const char* filePath, bool loopback, int cycle, int startPos)`
   * , then `totalVolume` is the volume after audio mixing.
   *
   */
  virtual void onAudioVolumeIndication(const AudioVolumeInfo* speakers, unsigned int speakerNumber,
                                       int totalVolume) {
    (void)speakers;
    (void)speakerNumber;
    (void)totalVolume;
  }

  /**
   * @brief Occurs when a user leaves a channel.
   *
   * @details
   * You can obtain information such as the total duration of a call, and the data traffic that the
   * SDK transmits and receives.
   * Call timing: The SDK triggers this callback after you call `leaveChannel()`, `leaveChannel(const
   * LeaveChannelOptions& options)`
   * , `leaveChannelEx(const RtcConnection& connection)`, or `leaveChannelEx(const RtcConnection&
   * connection, const LeaveChannelOptions& options)` to leave a channel.
   *
   * @param stats Call statistics. See `RtcStats`.
   *
   */
  virtual void onLeaveChannel(const RtcStats& stats) { (void)stats; }

  /**
   * @brief Reports the statistics about the current call.
   *
   * @details
   * Call timing: The SDK triggers this callback once every two seconds after the user joins the
   * channel.
   *
   * @param stats Statistics of the RTC engine. See `RtcStats`.
   *
   */
  virtual void onRtcStats(const RtcStats& stats) { (void)stats; }

  /**
   * @brief Occurs when the audio device state changes.
   *
   * @details
   * This callback notifies the application that the system's audio device state is changed. For
   * example, a headset is unplugged from the device.
   *
   * @note This method is for Windows and macOS only.
   *
   * @param deviceId The device ID.
   * @param deviceType The device type. See `MEDIA_DEVICE_TYPE`.
   * @param deviceState The device state. See `MEDIA_DEVICE_STATE_TYPE`.
   *
   */
  virtual void onAudioDeviceStateChanged(const char* deviceId, int deviceType, int deviceState) {
    (void)deviceId;
    (void)deviceType;
    (void)deviceState;
  }

  /**
   * @brief Reports the playback progress of a music file.
   *
   * @details
   * After you called the `startAudioMixing(const char* filePath, bool loopback, int cycle, int
   * startPos)` method to play a music file, the SDK triggers this
   * callback every two seconds to report the playback progress.
   *
   * @param position The playback progress (ms).
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual void onAudioMixingPositionChanged(int64_t position) {}

  /**
   * @brief Occurs when the playback of the local music file finishes.
   *
   * @deprecated This method is deprecated, use onAudioMixingStateChanged instead.
   *
   * @details
   * After you call `startAudioMixing(const char* filePath, bool loopback, int cycle, int startPos)`
   * to play a local music file, this callback occurs when the
   * playback finishes. If the call of `startAudioMixing(const char* filePath, bool loopback, int
   * cycle, int startPos)` fails, the error code
   * `WARN_AUDIO_MIXING_OPEN_ERROR` is returned.
   *
   */
  virtual void onAudioMixingFinished() __deprecated {}

  /**
   * @brief Occurs when the playback of the local music file finishes.
   *
   * @details
   * This callback occurs when the local audio effect file finishes playing.
   *
   * @param soundId The ID of the audio effect. The unique ID of each audio effect file.
   *
   */
  virtual void onAudioEffectFinished(int soundId) {}

  /**
   * @brief Occurs when the video device state changes.
   *
   * @details
   * This callback reports the change of system video devices, such as being unplugged or removed. On
   * a Windows device with an external camera for video capturing, the video disables once the
   * external camera is unplugged.
   *
   * @note This callback is for Windows and macOS only.
   *
   * @param deviceId The device ID.
   * @param deviceType Media device types. See `MEDIA_DEVICE_TYPE`.
   * @param deviceState Media device states. See `MEDIA_DEVICE_STATE_TYPE`.
   *
   */
  virtual void onVideoDeviceStateChanged(const char* deviceId, int deviceType, int deviceState) {
    (void)deviceId;
    (void)deviceType;
    (void)deviceState;
  }

  /**
   * @brief Reports the last mile network quality of each user in the channel.
   *
   * @details
   * This callback reports the last mile network conditions of each user in the channel. Last mile
   * refers to the connection between the local device and Agora's edge server.
   * The SDK triggers this callback once every two seconds. If a channel includes multiple users, the
   * SDK triggers this callback as many times.
   * This callback provides feedback on network quality through sending and receiving broadcast
   * packets within the channel. Excessive broadcast packets can lead to broadcast storms. To prevent
   * broadcast storms from causing a large amount of data transmission within the channel, this
   * callback supports feedback on the network quality of up to 4 remote hosts simultaneously by
   * default.
   *
   * @note `txQuality` is `UNKNOWN` when the user is not sending a stream; `rxQuality` is `UNKNOWN`
   * when the user is not receiving a stream.
   *
   * @param uid The user ID. The network quality of the user with this user ID is reported. If the uid
   * is 0, the local network quality is reported.
   * @param txQuality Uplink network quality rating of the user in terms of the transmission bit rate,
   * packet loss rate, average RTT (Round-Trip Time) and jitter of the uplink network. This parameter
   * is a quality rating helping you understand how well the current uplink network conditions can
   * support the selected video encoder configuration. For example, a 1000 Kbps uplink network may be
   * adequate for video frames with a resolution of 640 × 480 and a frame rate of 15 fps in the
   * LIVE_BROADCASTING profile, but might be inadequate for resolutions higher than 1280 × 720. See
   * `QUALITY_TYPE`.
   * @param rxQuality Downlink network quality rating of the user in terms of packet loss rate,
   * average RTT, and jitter of the downlink network. See `QUALITY_TYPE`.
   *
   */
  virtual void onNetworkQuality(uid_t uid, int txQuality, int rxQuality) {
    (void)uid;
    (void)txQuality;
    (void)rxQuality;
  }

  /**
   * Occurs when intra request from remote user is received.
   *
   * This callback is triggered once remote user needs one Key frame.
   *
   */
  virtual void onIntraRequestReceived() {}

  /**
   * @brief Occurs when the uplink network information changes.
   *
   * @details
   * The SDK triggers this callback when the uplink network information changes.
   *
   * @note This callback only applies to scenarios where you push externally encoded video data in
   * H.264 format to the SDK.
   *
   * @param info The uplink network information. See `UplinkNetworkInfo`.
   *
   */
  virtual void onUplinkNetworkInfoUpdated(const UplinkNetworkInfo& info) {
    (void)info;
  }

  /**
   * @brief Reports the last-mile network quality of the local user.
   *
   * @details
   * This callback reports the last-mile network conditions of the local user before the user joins
   * the channel. Last mile refers to the connection between the local device and Agora's edge server.
   * Before the user joins the channel, this callback is triggered by the SDK once
   * `startLastmileProbeTest` is called and reports the last-mile network conditions of the local
   * user.
   *
   * @param quality The last-mile network quality. See `QUALITY_TYPE`.
   *
   */
  virtual void onLastmileQuality(int quality) { (void)quality; }

  /**
   * @brief Occurs when the first local video frame is displayed on the local video view.
   *
   * @details
   * The SDK triggers this callback when the first local video frame is displayed on the local video
   * view.
   *
   * @param source The type of the video source. See `VIDEO_SOURCE_TYPE`.
   * @param width The width (px) of the first local video frame.
   * @param height The height (px) of the first local video frame.
   * @param elapsed The time elapsed (ms) from the local user calling `joinChannel(const char* token,
   * const char* channelId, const char* info, uid_t uid)` or
   * `joinChannel(const char* token, const char* channelId, uid_t uid, const ChannelMediaOptions&
   * options)` to join the channel to when the SDK triggers this callback. If `startPreview()`
   * / `startPreview(VIDEO_SOURCE_TYPE sourceType)` is called before joining the channel, this
   * parameter indicates the
   * time elapsed from calling `startPreview()` or `startPreview(VIDEO_SOURCE_TYPE sourceType)` to
   * when this event
   * occurred.
   *
   */
  virtual void onFirstLocalVideoFrame(VIDEO_SOURCE_TYPE source, int width, int height, int elapsed) {
    (void)source;
    (void)width;
    (void)height;
    (void)elapsed;
  }

  /**
   * @brief Occurs when the first video frame is published.
   *
   * @details
   * The SDK triggers this callback under one of the following circumstances:
   * - The local client enables the video module and calls `joinChannel(const char* token, const char*
   * channelId, const char* info, uid_t uid)` or `joinChannel(const char* token, const char*
   * channelId, uid_t uid, const ChannelMediaOptions& options)`
   * to join the channel successfully.
   * - The local client calls `muteLocalVideoStream` (`true`) and `muteLocalVideoStream` (`false`) in
   * sequence.
   * - The local client calls `disableVideo` and `enableVideo` in sequence.
   * - The local client calls `pushVideoFrame` to successfully push the video frame to the SDK.
   *
   * @param source The type of the video source. See `VIDEO_SOURCE_TYPE`.
   * @param elapsed Time elapsed (ms) from the local user calling `joinChannel(const char* token,
   * const char* channelId, const char* info, uid_t uid)` or `joinChannel(const char* token, const
   * char* channelId, uid_t uid, const ChannelMediaOptions& options)`
   * until this callback is triggered.
   *
   */
  virtual void onFirstLocalVideoFramePublished(VIDEO_SOURCE_TYPE source, int elapsed) {
    (void)source;
    (void)elapsed;
  }

  /**
   * @brief Occurs when the first remote video frame is received and decoded.
   *
   * @details
   * The SDK triggers this callback under one of the following circumstances:
   * - The remote user joins the channel and sends the video stream.
   * - The remote user stops sending the video stream and re-sends it after 15 seconds. Reasons for
   * such an interruption include:
   *   - The remote user leaves the channel.
   *   - The remote user drops offline.
   *   - The remote user calls `disableVideo` to disable video.
   *
   * @param uid The user ID of the remote user sending the video stream.
   * @param width The width (px) of the video stream.
   * @param height The height (px) of the video stream.
   * @param elapsed The time elapsed (ms) from the local user calling `joinChannel(const char* token,
   * const char* channelId, const char* info, uid_t uid)` or
   * `joinChannel(const char* token, const char* channelId, uid_t uid, const ChannelMediaOptions&
   * options)` until the SDK triggers this callback.
   *
   */
  virtual void onFirstRemoteVideoDecoded(uid_t uid, int width, int height, int elapsed) __deprecated {
    (void)uid;
    (void)width;
    (void)height;
    (void)elapsed;
  }

  /**
   * @brief Occurs when the video size or rotation of a specified user changes.
   *
   * @param sourceType The type of the video source. See `VIDEO_SOURCE_TYPE`.
   * @param uid The ID of the user whose video size or rotation changes. (The `uid` for the local user
   * is 0. The video is the local user's video preview).
   * @param width The width (pixels) of the video stream.
   * @param height The height (pixels) of the video stream.
   * @param rotation The rotation information. The value range is [0,360).Note: On the iOS platform,
   * the parameter value is always 0.
   *
   */
  virtual void onVideoSizeChanged(VIDEO_SOURCE_TYPE sourceType, uid_t uid, int width, int height, int rotation) {
    (void)uid;
    (void)width;
    (void)height;
    (void)rotation;
  }

  /** 
   * @brief Occurs when the local video event occurs.
   *
   * @since v4.6.1
   *
   * @details
   * This callback is triggered when a video event occurs. You can use this callback to get the reason for such an event.
   *
   * @param source The video source type: #VIDEO_SOURCE_TYPE.
   * @param event The local video event type: #LOCAL_VIDEO_EVENT_TYPE.
   *
   */
  virtual void onLocalVideoEvent(VIDEO_SOURCE_TYPE source, LOCAL_VIDEO_EVENT_TYPE event) {
    (void)source;
    (void)event;
  }

  /** 
   * @brief Occurs when the local video stream state changes.
   *
   * @details
   * When the status of the local video changes, the SDK triggers this callback to report the current
   * local video state and the reason for the state change.
   * Applicable scenarios: You can use this callback to stay updated on the state changes of the local
   * video stream, and take corresponding measures based on the reasons for the state changes, to
   * better manage and debug issues related to the video stream.
   * Call timing: - The SDK triggeres this callback under the following circumstances, with the
   * `state` as LOCAL_VIDEO_STREAM_STATE_FAILED, and the `reason` as
   * `LOCAL_VIDEO_STREAM_REASON_CAPTURE_FAILURE`:
   *   - The app switches to the background, and the system revokes the camera resource.
   *   - For Android 9 and later versions, after an app is in the background for a period, the system
   * automatically revokes camera permissions.
   *   - For Android 6 and later versions, if the camera is held by a third-party app for a certain
   * duration and then released, the SDK triggers this callback and reports the
   * `onLocalVideoStateChanged` (`LOCAL_VIDEO_STREAM_STATE_CAPTURING,LOCAL_VIDEO_STREAM_REASON_OK`)
   * callback.
   *   - The camera starts normally, but does not output video frames for four consecutive seconds.
   * - When the camera outputs captured video frames, if the SDK detects 15 consecutive duplicate
   * video frames, it triggers this callback, with the `state` as `LOCAL_VIDEO_STREAM_STATE_CAPTURING`
   * and the `reason` as `LOCAL_VIDEO_STREAM_REASON_CAPTURE_FAILURE`.Note:
   * - Note that the video frame duplication detection is only available for video frames with a
   * resolution greater than 200 × 200, a frame rate greater than or equal to 10 fps, and a bitrate
   * less than 20 Kbps.
   * - Normally, if there is an error in video capturing, the issue can be troubleshooted through the
   * `reason` parameter in this callback. However, on some devices, when there is an issue with
   * capturing (such as freezing), the Android system will not throw any error callbacks, so the SDK
   * cannot report the reason for the change in local video status. In this case, you can determine if
   * there is no video frame being captured by checking the following: this callback reports the
   * `state` as `LOCAL_VIDEO_STREAM_STATE_CAPTURING` or `LOCAL_VIDEO_STREAM_STATE_ENCODING`, and the
   * `captureFrameRate` in the `onLocalVideoStats` callback is 0.
   *
   * @note
   * - Note that the video frame duplication detection is only available for video frames with a
   * resolution greater than 200 × 200, a frame rate greater than or equal to 10 fps, and a bitrate
   * less than 20 Kbps.
   * - Normally, if there is an error in video capturing, the issue can be troubleshooted through the
   * `reason` parameter in this callback. However, on some devices, when there is an issue with
   * capturing (such as freezing), the Android system will not throw any error callbacks, so the SDK
   * cannot report the reason for the change in local video status. In this case, you can determine if
   * there is no video frame being captured by checking the following: this callback reports the
   * `state` as `LOCAL_VIDEO_STREAM_STATE_CAPTURING` or `LOCAL_VIDEO_STREAM_STATE_ENCODING`, and the
   * `captureFrameRate` in the `onLocalVideoStats` callback is 0.
   *
   * @param source The type of the video source. See `VIDEO_SOURCE_TYPE`.
   * @param state The state of the local video, see `LOCAL_VIDEO_STREAM_STATE`.
   * @param reason The reasons for changes in local video state. See `LOCAL_VIDEO_STREAM_REASON`.
   *
   */
  virtual void onLocalVideoStateChanged(VIDEO_SOURCE_TYPE source, LOCAL_VIDEO_STREAM_STATE state, LOCAL_VIDEO_STREAM_REASON reason) {
    (void)source;
    (void)state;
    (void)reason;
  }

  /**
   * @brief Occurs when the remote video stream state changes.
   *
   * @note This callback does not work properly when the number of users (in the communication
   * profile) or hosts (in the live streaming channel) in a channel exceeds 32.
   *
   * @param uid The ID of the remote user whose video state changes.
   * @param state The state of the remote video. See `REMOTE_VIDEO_STATE`.
   * @param reason The reason for the remote video state change. See `REMOTE_VIDEO_STATE_REASON`.
   * @param elapsed Time elapsed (ms) from the local user calling the `joinChannel(const char* token,
   * const char* channelId, uid_t uid, const ChannelMediaOptions& options)` method until
   * the SDK triggers this callback.
   *
   */
  virtual void onRemoteVideoStateChanged(uid_t uid, REMOTE_VIDEO_STATE state, REMOTE_VIDEO_STATE_REASON reason, int elapsed) {
    (void)uid;
    (void)state;
    (void)reason;
    (void)elapsed;
  }

  /**
   * @brief Occurs when the renderer receives the first frame of the remote video.
   *
   * @note This callback is only triggered when the video frame is rendered by the SDK; it will not be
   * triggered if the user employs custom video rendering.You need to implement this independently
   * using methods outside the SDK.
   *
   * @param uid The user ID of the remote user sending the video stream.
   * @param width The width (px) of the video stream.
   * @param height The height (px) of the video stream.
   * @param elapsed The time elapsed (ms) from the local user calling `joinChannel(const char* token,
   * const char* channelId, const char* info, uid_t uid)` or
   * `joinChannel(const char* token, const char* channelId, uid_t uid, const ChannelMediaOptions&
   * options)` until the SDK triggers this callback.
   *
   */
  virtual void onFirstRemoteVideoFrame(uid_t uid, int width, int height, int elapsed) {
    (void)uid;
    (void)width;
    (void)height;
    (void)elapsed;
  }

  /**
   * @brief Occurs when a remote user (in the communication profile)/ host (in the live streaming
   * profile) joins the channel.
   *
   * @details
   * - In a communication channel, this callback indicates that a remote user joins the channel. The
   * SDK also triggers this callback to report the existing users in the channel when a user joins the
   * channel.
   * - In a live-broadcast channel, this callback indicates that a host joins the channel. The SDK
   * also triggers this callback to report the existing hosts in the channel when a host joins the
   * channel. Agora recommends limiting the number of co-hosts to 32, with a maximum of 17 video
   * hosts.
   * Call timing: The SDK triggers this callback under one of the following circumstances:
   * - A remote user/host joins the channel.
   * - A remote user switches the user role to the host after joining the channel.
   * - A remote user/host rejoins the channel after a network interruption.
   *
   * @param uid The ID of the user or host who joins the channel.
   * @param elapsed Time delay (ms) from the local user calling `joinChannel(const char* token, const
   * char* channelId, const char* info, uid_t uid)` or `joinChannel(const char* token, const char*
   * channelId, uid_t uid, const ChannelMediaOptions& options)`
   * until this callback is triggered.
   *
   */
  virtual void onUserJoined(uid_t uid, int elapsed) {
    (void)uid;
    (void)elapsed;
  }

  /**
   * @brief Occurs when a remote user (in the communication profile)/ host (in the live streaming
   * profile) leaves the channel.
   *
   * @details
   * There are generally two reasons for users to become offline:
   * - Leave the channel: When a user/host leaves the channel, the user/host sends a goodbye message.
   * - Drop offline: When no data packet of the user or host is received for a certain period of time
   * (20 seconds for the communication profile, and more for the live broadcast profile), the SDK
   * assumes that the user/host drops offline. A poor network connection may lead to false detections.
   * It is recommended to use the Agora RTM SDK for reliable offline detection.
   * Call timing: This callback is triggered when a remote user (in the communication profile) or host
   * (in the live streaming profile) leaves a channel.
   *
   * @param uid The ID of the user who leaves the channel or goes offline.
   * @param reason Reasons why a remote user (in the communication profile) or host (in the live
   * streaming profile) goes offline. See `USER_OFFLINE_REASON_TYPE`.
   *
   */
  virtual void onUserOffline(uid_t uid, USER_OFFLINE_REASON_TYPE reason) {
    (void)uid;
    (void)reason;
  }

  /**
   * @brief Occurs when a remote user (in the communication profile) or a host (in the live streaming
   * profile) stops/resumes sending the audio stream.
   *
   * @details
   * The SDK triggers this callback when the remote user stops or resumes sending the audio stream by
   * calling the `muteLocalAudioStream` method.
   *
   * @note This callback does not work properly when the number of users (in the communication
   * profile) or hosts (in the live streaming channel) in a channel exceeds 32.
   *
   * @param uid The user ID.
   * @param muted Whether the remote user's audio stream is muted:
   * - `true`: User's audio stream is muted.
   * - `false`: User's audio stream is unmuted.
   *
   */
  virtual void onUserMuteAudio(uid_t uid, bool muted) {
    (void)uid;
    (void)muted;
  }

  /**
   * @brief Occurs when a remote user stops or resumes publishing the video stream.
   *
   * @details
   * When a remote user calls `muteLocalVideoStream` to stop or resume publishing the video stream,
   * the SDK triggers this callback to report to the local user the state of the streams published by
   * the remote user.
   *
   * @note This callback can be inaccurate when the number of users (in the communication profile) or
   * hosts (in the live streaming profile) in a channel exceeds 32.
   *
   * @param uid The user ID of the remote user.
   * @param muted Whether the remote user stops publishing the video stream:
   * - `true`: The remote user stops publishing the video stream.
   * - `false`: The remote user resumes publishing the video stream.
   *
   */
  virtual void onUserMuteVideo(uid_t uid, bool muted) {
    (void)uid;
    (void)muted;
  }

  /**
   * @brief Occurs when a remote user enables or disables the video module.
   *
   * @details
   * Once the video module is disabled, the user can only use a voice call. The user cannot send or
   * receive any video.
   * The SDK triggers this callback when a remote user enables or disables the video module by calling
   * the `enableVideo` or `disableVideo` method.
   *
   * @param uid The user ID of the remote user.
   * @param enabled
   *  - `true`: The video module is enabled.
   * - `false`: The video module is disabled.
   *
   */
  virtual void onUserEnableVideo(uid_t uid, bool enabled) {
    (void)uid;
    (void)enabled;
  }

  /**
   * Occurs when the remote user audio or video state is updated.
   * @param uid The uid of the remote user.
   * @param state The remote user's audio or video state: #REMOTE_USER_STATE.
   */
  virtual void onUserStateChanged(uid_t uid, REMOTE_USER_STATE state) {
    (void)uid;
    (void)state;
  }

  /**
   * @brief Occurs when a specific remote user enables/disables the local video capturing function.
   *
   * @details
   * The SDK triggers this callback when the remote user resumes or stops capturing the video stream
   * by calling the `enableLocalVideo` method.
   *
   * @param uid The user ID of the remote user.
   * @param enabled Whether the specified remote user enables/disables local video capturing:
   * - `true`: The video module is enabled. Other users in the channel can see the video of this
   * remote user.
   * - `false`: The video module is disabled. Other users in the channel can no longer receive the
   * video stream from this remote user, while this remote user can still receive the video streams
   * from other users.
   *
   */
  virtual void onUserEnableLocalVideo(uid_t uid, bool enabled) __deprecated {
    (void)uid;
    (void)enabled;
  }

  /**
   * @brief Reports the transport-layer statistics of each remote audio stream.
   *
   * @details
   * The SDK triggers this callback once every two seconds for each remote user who is sending audio
   * streams. If a channel includes multiple remote users, the SDK triggers this callback as many
   * times.
   *
   * @param stats The statistics of the received remote audio streams. See `RemoteAudioStats`.
   *
   */
  virtual void onRemoteAudioStats(const RemoteAudioStats& stats) {
    (void)stats;
  }

  /**
   * @brief Reports the statistics of the local audio stream.
   *
   * @details
   * The SDK triggers this callback once every two seconds.
   *
   * @param stats Local audio statistics. See `LocalAudioStats`.
   *
   */
  virtual void onLocalAudioStats(const LocalAudioStats& stats) {
    (void)stats;
  }

  /**
   * @brief Reports the statistics of the local video stream.
   *
   * @details
   * The SDK triggers this callback once every two seconds to report the statistics of the local video
   * stream.
   *
   * @param source The type of the video source. See `VIDEO_SOURCE_TYPE`.
   * @param stats The statistics of the local video stream. See `LocalVideoStats`.
   *
   */
  virtual void onLocalVideoStats(VIDEO_SOURCE_TYPE source, const LocalVideoStats& stats) {
    (void)source;
    (void)stats;
  }

  /**
   * @brief Reports the statistics of the video stream sent by each remote users.
   *
   * @details
   * Reports the statistics of the video stream from the remote users. The SDK triggers this callback
   * once every two seconds for each remote user. If a channel has multiple users/hosts sending video
   * streams, the SDK triggers this callback as many times.
   *
   * @param stats Statistics of the remote video stream. See `RemoteVideoStats`.
   *
   */
  virtual void onRemoteVideoStats(const RemoteVideoStats& stats) {
    (void)stats;
  }

  /**
   * @brief Occurs when the camera turns on and is ready to capture the video.
   *
   * @deprecated Use `LOCAL_VIDEO_STREAM_STATE_CAPTURING(1)` in onLocalVideoStateChanged instead.
   *
   * @details
   * This callback indicates that the camera has been successfully turned on and you can start to
   * capture video.
   *
   */
  virtual void onCameraReady() __deprecated {}

  /**
   * @brief Occurs when the camera focus area changes.
   *
   * @note This callback is for Android and iOS only.
   *
   * @param x The x-coordinate of the changed camera focus area.
   * @param y The y-coordinate of the changed camera focus area.
   * @param width The width of the changed camera focus area.
   * @param height The height of the changed camera focus area.
   *
   */
  virtual void onCameraFocusAreaChanged(int x, int y, int width, int height) {
    (void)x;
    (void)y;
    (void)width;
    (void)height;
  }
  /**
   * @brief Occurs when the camera exposure area changes.
   *
   * @details
   * The SDK triggers this callback when the local user changes the camera exposure position by
   * calling `setCameraExposurePosition`.
   *
   * @note This callback is for Android and iOS only.
   *
   * @param x The x coordinate of the changed camera exposure area.
   * @param y The y coordinate of the changed camera exposure area.
   * @param width The width of the changed camera exposure area.
   * @param height The height of the changed exposure area.
   *
   */
  virtual void onCameraExposureAreaChanged(int x, int y, int width, int height) {
    (void)x;
    (void)y;
    (void)width;
    (void)height;
  }
#if defined(__ANDROID__) || (defined(__APPLE__) && TARGET_OS_IOS) || defined(__OHOS__)
  /**
   * @brief Reports the face detection result of the local user.
   *
   * @details
   * Once you enable face detection by calling `enableFaceDetection` `(true)`, you can get the
   * following information on the local user in real-time:
   * - The width and height of the local video.
   * - The position of the human face in the local view.
   * - The distance between the human face and the screen.
   * This value is based on the fitting calculation of the local video size and the position of the
   * human face.
   *
   * @note
   * - This callback is for Android and iOS only.
   * - When it is detected that the face in front of the camera disappears, the callback will be
   * triggered immediately. When no human face is detected, the frequency of this callback to be
   * triggered wil be decreased to reduce power consumption on the local device.
   * - The SDK stops triggering this callback when a human face is in close proximity to the screen.
   * - On Android, the value of distance reported in this callback may be slightly different from the
   * actual `distance`. Therefore, Agora does not recommend using it for accurate calculation.
   *
   * @param imageWidth The width (px) of the video image captured by the local camera.
   * @param imageHeight The height (px) of the video image captured by the local camera.
   * @param vecRectangle An array of `numFaces`, representing the detected face information:
   * - `x`: The x-coordinate (px) of the human face in the local view. Taking the top left corner of
   * the view as the origin, the x-coordinate represents the horizontal position of the human face
   * relative to the origin.
   * - `y`: The y-coordinate (px) of the human face in the local view. Taking the top left corner of
   * the view as the origin, the y-coordinate represents the vertical position of the human face
   * relative to the origin.
   * - `width`: The width (px) of the human face in the captured view.
   * - `height`: The height (px) of the human face in the captured view.
   * @param vecDistance An array of `numFaces`, representing the distance (cm) between a face and the
   * device screen.
   * @param numFaces The number of faces detected. If the value is 0, it means that no human face is
   * detected.
   *
   */
  virtual void onFacePositionChanged(int imageWidth, int imageHeight,
                                     const Rectangle* vecRectangle, const int* vecDistance,
                                     int numFaces) {
    (void) imageWidth;
    (void) imageHeight;
    (void) vecRectangle;
    (void) vecDistance;
    (void) numFaces;
  }
#endif
  /**
   * @brief Occurs when the video stops playing.
   *
   * @deprecated Use `LOCAL_VIDEO_STREAM_STATE_STOPPED(0)` in the onLocalVideoStateChanged callback
   * instead.
   *
   * @details
   * The application can use this callback to change the configuration of the `view` (for example,
   * displaying other pictures in the view) after the video stops playing.
   *
   */
  virtual void onVideoStopped() __deprecated {}

  /**
   * @brief Occurs when the playback state of the music file changes.
   *
   * @details
   * This callback occurs when the playback state of the music file changes, and reports the current
   * state and error code.
   *
   * @param state The playback state of the music file. See `AUDIO_MIXING_STATE_TYPE`.
   * @param reason Error code. See `AUDIO_MIXING_REASON_TYPE`.
   *
   */
  virtual void onAudioMixingStateChanged(AUDIO_MIXING_STATE_TYPE state, AUDIO_MIXING_REASON_TYPE reason) {
    (void)state;
    (void)reason;
  }

  /**
   * @brief Occurs when the state of virtual metronome changes.
   *
   * @details
   * When the state of the virtual metronome changes, the SDK triggers this callback to report the
   * current state of the virtual metronome. This callback indicates the state of the local audio
   * stream and enables you to troubleshoot issues when audio exceptions occur.
   *
   * @note This callback is for Android and iOS only.
   *
   * @param state For the current virtual metronome status, see `RHYTHM_PLAYER_STATE_TYPE`.
   * @param reason For the error codes and error messages related to virtual metronome errors, see
   * `RHYTHM_PLAYER_REASON`.
   *
   */
  virtual void onRhythmPlayerStateChanged(RHYTHM_PLAYER_STATE_TYPE state, RHYTHM_PLAYER_REASON reason) {
    (void)state;
    (void)reason;
  }

  /**
   * @brief Occurs when the SDK cannot reconnect to Agora's edge server 10 seconds after its
   * connection to the server is interrupted.
   *
   * @details
   * The SDK triggers this callback when it cannot connect to the server 10 seconds after calling the
   * `joinChannel(const char* token, const char* channelId, uid_t uid, const ChannelMediaOptions&
   * options)` method, regardless of whether it is in the channel. If the SDK fails to
   * rejoin the channel 20 minutes after being disconnected from Agora's edge server, the SDK stops
   * rejoining the channel.
   *
   */
  virtual void onConnectionLost() {}

  /**
   * @brief Occurs when the connection between the SDK and the server is interrupted.
   *
   * @deprecated Use `onConnectionStateChanged` instead.
   *
   * @details
   * The SDK triggers this callback when it loses connection with the server for more than four
   * seconds after the connection is established. After triggering this callback, the SDK tries to
   * reconnect to the server. You can use this callback to implement pop-up reminders. The differences
   * between this callback and `onConnectionLost` are as follow:
   * - The SDK triggers the `onConnectionInterrupted` callback when it loses connection with the
   * server for more than four seconds after it successfully joins the channel.
   * - The SDK triggers the `onConnectionLost` callback when it loses connection with the server for
   * more than 10 seconds, whether or not it joins the channel.
   * If the SDK fails to rejoin the channel 20 minutes after being disconnected from Agora's edge
   * server, the SDK stops rejoining the channel.
   *
   */
  virtual void onConnectionInterrupted() __deprecated {}

  /**
   * @brief Occurs when the connection is banned by the Agora server.
   *
   * @deprecated Use `onConnectionStateChanged` instead.
   */
  virtual void onConnectionBanned() __deprecated {}

  /**
   * @brief Occurs when the local user receives the data stream from the remote user.
   *
   * @details
   * The SDK triggers this callback when the local user receives the stream message that the remote
   * user sends by calling the `sendStreamMessage` method.
   *
   * @note If you need a more comprehensive solution for low-latency, high-concurrency, and scalable
   * real-time messaging and status synchronization, it is recommended to use `Signaling`.
   *
   * @param uid The ID of the remote user sending the message.
   * @param streamId The stream ID of the received message.
   * @param data The data received.
   * @param length The data length (byte).
   * @param sentTs The time when the data stream is sent.
   *
   */
  virtual void onStreamMessage(uid_t uid, int streamId, const char* data, size_t length, uint64_t sentTs) {
    (void)uid;
    (void)streamId;
    (void)data;
    (void)length;
    (void)sentTs;
  }

  /**
   * @brief Occurs when the local user does not receive the data stream from the remote user.
   *
   * @details
   * The SDK triggers this callback when the local user fails to receive the stream message that the
   * remote user sends by calling the `sendStreamMessage` method.
   *
   * @note If you need a more comprehensive solution for low-latency, high-concurrency, and scalable
   * real-time messaging and status synchronization, it is recommended to use `Signaling`.
   *
   * @param uid The ID of the remote user sending the message.
   * @param streamId The stream ID of the received message.
   * @param code Error code.
   * @param missed The number of lost messages.
   * @param cached Number of incoming cached messages when the data stream is interrupted.
   *
   */
  virtual void onStreamMessageError(uid_t uid, int streamId, int code, int missed, int cached) {
    (void)uid;
    (void)streamId;
    (void)code;
    (void)missed;
    (void)cached;
  }

  /**
   * @brief Occurs when the local user receives data via Reliable Data Transmission (RDT) from a remote user.
   *
   * @technical preview
   *
   * @details The SDK triggers this callback when the user receives the data stream that another user sends
   *          by calling the \ref agora::rtc::IRtcEngine::sendRdtMessage "sendRdtMessage" method.
   *
   * @param userId ID of the user who sends the data.
   * @param type The RDT stream type. See RdtStreamType.
   * @param data The data received.
   * @param length The length (byte) of the data.
   */
  virtual void onRdtMessage(uid_t userId, RdtStreamType type, const char *data, size_t length) {
    (void)userId;
    (void)type;
    (void)data;
    (void)length;
  };

  /**
   * @brief Occurs when the RDT tunnel state changed
   *
   * @technical preview
   *
   * @param userId ID of the user who sends the data.
   * @param state The RDT tunnel state. See RdtState.
   */
  virtual void onRdtStateChanged(uid_t userId, RdtState state) {
    (void)userId;
    (void)state;
  }

  /**
   * @brief Occurs when the local user receives media control message sent by a remote user.
   *
   * @technical preview
   *
   * @details The SDK triggers this callback when the user receives data sent by a remote user using the sendMediaControlMessage method.
   *
   * @param userId ID of the user who sends the data.
   * @param data The data received.
   * @param length The length (byte) of the data.
   */
  virtual void onMediaControlMessage(uid_t userId, const char* data, size_t length) {
    (void)userId;
    (void)data;
    (void)length;
  }

  /**
   * @brief Occurs when the token expires.
   *
   * @details
   * The SDK triggers this callback if the token expires.
   * When receiving this callback, you need to generate a new token on your token server and you can
   * renew your token through one of the following ways:
   * - In scenarios involving one channel:
   *   - Call `renewToken` to pass in the new token.
   *   - Call `leaveChannel(const LeaveChannelOptions& options)` to leave the current channel and then
   * pass in the new token when
   * you call `joinChannel(const char* token, const char* channelId, uid_t uid, const
   * ChannelMediaOptions& options)` to join a channel.
   * - In scenarios involving mutiple channels: Call `updateChannelMediaOptionsEx` to pass in the new
   * token.
   *
   */
  virtual void onRequestToken() {}

  /**
   * @brief Occurs when the token expires in 30 seconds.
   *
   * @details
   * When receiving this callback, you need to generate a new token on your token server and you can
   * renew your token through one of the following ways:
   * - In scenarios involving one channel:
   *   - Call `renewToken` to pass in the new token.
   *   - Call `leaveChannel(const LeaveChannelOptions& options)` to leave the current channel and then
   * pass in the new token when
   * you call `joinChannel(const char* token, const char* channelId, uid_t uid, const
   * ChannelMediaOptions& options)` to join a channel.
   * - In scenarios involving mutiple channels: Call `updateChannelMediaOptionsEx` to pass in the new
   * token.
   * Call timing: The SDK triggers this callback 30 seconds before the token expires, reminding the
   * app to update the token.
   *
   * @param token The token that is about to expire.
   *
   */
  virtual void onTokenPrivilegeWillExpire(const char* token) {
    (void)token;
  }

  /**
   * Occurs when connection license verification fails.
   *
   * You can know the reason accordding to error code
   */
  virtual void onLicenseValidationFailure(LICENSE_ERROR_TYPE error) {
    (void)error;
  }

  /**
   * @brief Occurs when the first audio frame is published.
   *
   * @details
   * The SDK triggers this callback under one of the following circumstances:
   * - The local client enables the audio module and calls `joinChannel(const char* token, const char*
   * channelId, uid_t uid, const ChannelMediaOptions& options)` successfully.
   * - The local client calls `muteLocalAudioStream` (`true`) and `muteLocalAudioStream` (`false`) in
   * sequence.
   * - The local client calls `disableAudio` and `enableAudio` in sequence.
   * - The local client calls `pushAudioFrame` to successfully push the audio frame to the SDK.
   *
   * @param elapsed Time elapsed (ms) from the local user calling `joinChannel(const char* token,
   * const char* channelId, uid_t uid, const ChannelMediaOptions& options)` until the SDK
   * triggers this callback.
   *
   */
  virtual void onFirstLocalAudioFramePublished(int elapsed) {
    (void)elapsed;
  }

  /**
   * @brief Occurs when the SDK decodes the first remote audio frame for playback.
   *
   * @deprecated Use `onRemoteAudioStateChanged` instead.
   *
   * @details
   * The SDK triggers this callback under one of the following circumstances:
   * - The remote user joins the channel and sends the audio stream for the first time.
   * - The remote user's audio is offline and then goes online to re-send audio. It means the local
   * user cannot receive audio in 15 seconds. Reasons for such an interruption include:
   *   - The remote user leaves channel.
   *   - The remote user drops offline.
   *   - The remote user calls `muteLocalAudioStream` to stop sending the audio stream.
   *   - The remote user calls `disableAudio` to disable audio.
   *
   * @param uid The user ID of the remote user.
   * @param elapsed The time elapsed (ms) from the local user calling `joinChannel(const char* token,
   * const char* channelId, uid_t uid, const ChannelMediaOptions& options)` until the
   * SDK triggers this callback.
   *
   */
  virtual void onFirstRemoteAudioDecoded(uid_t uid, int elapsed) __deprecated {
    (void)uid;
    (void)elapsed;
  }

  /**
   * @brief Occurs when the SDK receives the first audio frame from a specific remote user.
   *
   * @deprecated Use `onRemoteAudioStateChanged` instead.
   *
   * @param uid The user ID of the remote user.
   * @param elapsed The time elapsed (ms) from the local user calling `joinChannel(const char* token,
   * const char* channelId, uid_t uid, const ChannelMediaOptions& options)` until the
   * SDK triggers this callback.
   *
   */
  virtual void onFirstRemoteAudioFrame(uid_t uid, int elapsed) __deprecated {
    (void)uid;
    (void)elapsed;
  }

  /**
   * @brief Occurs when the local audio stream state changes.
   *
   * @details
   * When the state of the local audio stream changes (including the state of the audio capture and
   * encoding), the SDK triggers this callback to report the current state. This callback indicates
   * the state of the local audio stream, and allows you to troubleshoot issues when audio exceptions
   * occur.
   *
   * @note When the state is `LOCAL_AUDIO_STREAM_STATE_FAILED` (3), you can view the error information
   * in the `error` parameter.
   *
   * @param state The state of the local audio. See `LOCAL_AUDIO_STREAM_STATE`.
   * @param reason Reasons for local audio state changes. See `LOCAL_AUDIO_STREAM_REASON`.
   *
   */
  virtual void onLocalAudioStateChanged(LOCAL_AUDIO_STREAM_STATE state, LOCAL_AUDIO_STREAM_REASON reason) {
    (void)state;
    (void)reason;
  }

  /**
   * @brief Occurs when the remote audio state changes.
   *
   * @details
   * When the audio state of a remote user (in a voice/video call channel) or host (in a live
   * streaming channel) changes, the SDK triggers this callback to report the current state of the
   * remote audio stream.
   *
   * @note This callback does not work properly when the number of users (in the communication
   * profile) or hosts (in the live streaming channel) in a channel exceeds 32.
   *
   * @param uid The ID of the remote user whose audio state changes.
   * @param state The state of the remote audio. See `REMOTE_AUDIO_STATE`.
   * @param reason The reason of the remote audio state change. See `REMOTE_AUDIO_STATE_REASON`.
   * @param elapsed Time elapsed (ms) from the local user calling the `joinChannel(const char* token,
   * const char* channelId, uid_t uid, const ChannelMediaOptions& options)` method until
   * the SDK triggers this callback.
   *
   */
  virtual void onRemoteAudioStateChanged(uid_t uid, REMOTE_AUDIO_STATE state, REMOTE_AUDIO_STATE_REASON reason, int elapsed) {
    (void)uid;
    (void)state;
    (void)reason;
    (void)elapsed;
  }

  /**
   * @brief Occurs when the most active remote speaker is detected.
   *
   * @details
   * After a successful call of `enableAudioVolumeIndication`, the SDK continuously detects which
   * remote user has the loudest volume. During the current period, the remote user whose volume is
   * detected as the loudest for the most times, is the most active user.
   * When the number of users is no less than two and an active remote speaker exists, the SDK
   * triggers this callback and reports the `uid` of the most active remote speaker.
   * - If the most active remote speaker is always the same user, the SDK triggers the
   * `onActiveSpeaker` callback only once.
   * - If the most active remote speaker changes to another user, the SDK triggers this callback again
   * and reports the `uid` of the new active remote speaker.
   *
   * @param uid The user ID of the most active speaker.
   *
   */
  virtual void onActiveSpeaker(uid_t uid) {
    (void)uid;
  }

  /** Reports result of content inspection.
   *
   * @param result The result of content inspection: #CONTENT_INSPECT_RESULT.
   */
  virtual void onContentInspectResult(media::CONTENT_INSPECT_RESULT result) { (void)result; }

  /**
   * @brief Reports the result of taking a video snapshot.
   *
   * @details
   * After a successful `takeSnapshot(uid_t uid, const char* filePath)` method call, the SDK triggers
   * this callback to report
   * whether the snapshot is successfully taken as well as the details for the snapshot taken.
   *
   * @param uid The user ID. One `uid` of 0 indicates the local user.
   * @param filePath The local path of the snapshot.
   * @param width The width (px) of the snapshot.
   * @param height The height (px) of the snapshot.
   * @param errCode The message that confirms success or gives the reason why the snapshot is not
   * successfully taken:
   * - 0: Success.
   * - < 0: Failure:
   *   - -1: The SDK fails to write data to a file or encode a JPEG image.
   *   - -2: The SDK does not find the video stream of the specified user within one second after the
   * `takeSnapshot(uid_t uid, const char* filePath)` method call succeeds. The possible reasons are:
   * local capture stops, remote
   * end stops publishing, or video data processing is blocked.
   *   - -3: Calling the `takeSnapshot(uid_t uid, const char* filePath)` method too frequently.
   *
   */
  virtual void onSnapshotTaken(uid_t uid, const char* filePath, int width, int height, int errCode) {
    (void)uid;
    (void)filePath;
    (void)width;
    (void)height;
    (void)errCode;
  }

  /**
   * @brief Occurs when the user role or the audience latency level changes.
   *
   * @details
   * Call timing: This callback will be triggered in any of the following situations:
   * - Calling `setClientRole(CLIENT_ROLE_TYPE role)` or `setClientRole(CLIENT_ROLE_TYPE role, const
   * ClientRoleOptions& options)` to set the user role or audience latency
   * level **after joining a channel**
   * - Calling `setClientRole(CLIENT_ROLE_TYPE role)` or `setClientRole(CLIENT_ROLE_TYPE role, const
   * ClientRoleOptions& options)` and set the user role to `AUDIENCE`
   * **before joining a channel**.
   *
   * @note This callback will not be triggered when you call `setClientRole(CLIENT_ROLE_TYPE role)` or
   * `setClientRole(CLIENT_ROLE_TYPE role, const ClientRoleOptions& options)`
   * and set the user role to `BROADCASTER` **before joining a channel**.
   *
   * @param oldRole Role that the user switches from: `CLIENT_ROLE_TYPE`.
   * @param newRole Role that the user switches to: `CLIENT_ROLE_TYPE`.
   * @param newRoleOptions Since
   * v4.1.0
   * Properties of the role that the user switches to. See `ClientRoleOptions`.
   *
   */
  virtual void onClientRoleChanged(CLIENT_ROLE_TYPE oldRole, CLIENT_ROLE_TYPE newRole, const ClientRoleOptions& newRoleOptions) {
    (void)oldRole;
    (void)newRole;
    (void)newRoleOptions;
  }

  /**
   * @brief Occurs when switching a user role fails.
   *
   * @details
   * This callback informs you about the reason for failing to switching and your current user role.
   * Call timing: The SDK triggers this callback when the local user calls
   * `setClientRole(CLIENT_ROLE_TYPE role)` or
   * `setClientRole(CLIENT_ROLE_TYPE role, const ClientRoleOptions& options)` after joining a channel
   * to switch the user role but the switching fails.
   *
   * @param reason The reason for a user role switch failure. See `CLIENT_ROLE_CHANGE_FAILED_REASON`.
   * @param currentRole Current user role. See `CLIENT_ROLE_TYPE`.
   *
   */
  virtual void onClientRoleChangeFailed(CLIENT_ROLE_CHANGE_FAILED_REASON reason, CLIENT_ROLE_TYPE currentRole) {
    (void)reason;
    (void)currentRole;
  }

  /**
   * @brief Reports the volume change of the audio device or app.
   *
   * @details
   * Occurs when the volume on the playback device, audio capture device, or the volume of the app
   * changes.
   *
   * @note This callback is for Windows and macOS only.
   *
   * @param deviceType The device type. See `MEDIA_DEVICE_TYPE`.
   * @param volume The volume value. The range is [0, 255].
   * @param muted Whether the audio device is muted:
   * - `true`: The audio device is muted.
   * - `false`: The audio device is not muted.
   *
   */
  virtual void onAudioDeviceVolumeChanged(MEDIA_DEVICE_TYPE deviceType, int volume, bool muted) {
    (void)deviceType;
    (void)volume;
    (void)muted;
  }

  /**
   * @brief Occurs when the state of Media Push changes.
   *
   * @details
   * When the state of Media Push changes, the SDK triggers this callback and reports the URL address
   * and the current state of the Media Push. This callback indicates the state of the Media Push.
   * When exceptions occur, you can troubleshoot issues by referring to the detailed error
   * descriptions in the error code parameter.
   *
   * @param url The URL address where the state of the Media Push changes.
   * @param state The current state of the Media Push. See `RTMP_STREAM_PUBLISH_STATE`.
   * @param reason Reasons for the changes in the Media Push status. See `RTMP_STREAM_PUBLISH_REASON`.
   *
   */
  virtual void onRtmpStreamingStateChanged(const char* url, RTMP_STREAM_PUBLISH_STATE state,
                                           RTMP_STREAM_PUBLISH_REASON reason) {
    (void)url;
    (void)state;
    (void)reason;
  }

  /**
   * @brief Reports events during the Media Push.
   *
   * @param url The URL for Media Push.
   * @param eventCode The event code of Media Push. See `RTMP_STREAMING_EVENT`.
   *
   */
  virtual void onRtmpStreamingEvent(const char* url, RTMP_STREAMING_EVENT eventCode) {
    (void)url;
    (void)eventCode;
  }

  /**
   * @brief Occurs when the publisher's transcoding is updated.
   *
   * @details
   * When the `LiveTranscoding` class in the `startRtmpStreamWithTranscoding` method updates, the SDK
   * triggers the `onTranscodingUpdated` callback to report the update information.
   *
   * @note If you call the `startRtmpStreamWithTranscoding` method to set the `LiveTranscoding` class
   * for the first time, the SDK does not trigger this callback.
   *
   */
  virtual void onTranscodingUpdated() {}

  /**
   * @brief Occurs when the local audio route changes.
   *
   * @note This method is for Android, iOS and macOS only.
   *
   * @param routing The current audio routing. See `AudioRoute`.
   *
   */
  virtual void onAudioRoutingChanged(int routing) { (void)routing; }

  /**
   * @brief Occurs when the state of the media stream relay changes.
   *
   * @details
   * The SDK returns the state of the current media relay with any error message.
   *
   * @param state The state code. See `CHANNEL_MEDIA_RELAY_STATE`.
   * @param code The error code of the channel media relay. See `CHANNEL_MEDIA_RELAY_ERROR`.
   *
   */
  virtual void onChannelMediaRelayStateChanged(int state, int code) {
    (void)state;
    (void)code;
  }

  /**
   * @brief Occurs when the remote media stream falls back to the audio-only stream due to poor
   * network conditions or switches back to the video stream after the network conditions improve.
   *
   * @details
   * If you call `setRemoteSubscribeFallbackOption` and set `option` to
   * `STREAM_FALLBACK_OPTION_AUDIO_ONLY`, the SDK triggers this callback in the following situations:
   * - The downstream network condition is poor, and the subscribed video stream is downgraded to
   * audio-only stream.
   * - The downstream network condition has improved, and the subscribed stream has been restored to
   * video stream.
   *
   * @note Once the remote media stream switches to the low-quality video stream due to weak network
   * conditions, you can monitor the stream switch between a high-quality and low-quality stream in
   * the `onRemoteVideoStats` callback.
   *
   * @param uid The user ID of the remote user.
   * @param isFallbackOrRecover - `true`: The subscribed media stream falls back to audio-only due to
   * poor network conditions.
   * - `false`: The subscribed media stream switches back to the video stream after the network
   * conditions improve.
   *
   */
  virtual void onRemoteSubscribeFallbackToAudioOnly(uid_t uid, bool isFallbackOrRecover) {
    (void)uid;
    (void)isFallbackOrRecover;
  }

  /**
   * @brief Reports the transport-layer statistics of each remote audio stream.
   *
   * @deprecated Use `onRemoteAudioStats` instead.
   *
   * @details
   * This callback reports the transport-layer statistics, such as the packet loss rate and network
   * time delay after the local user receives an audio packet from a remote user. During a call, when
   * the user receives the audio packet sent by the remote user, the callback is triggered every 2
   * seconds.
   *
   * @param uid The ID of the remote user sending the audio streams.
   * @param delay The network delay (ms) from the remote user to the receiver.
   * @param lost The packet loss rate (%) of the audio packet sent from the remote user to the
   * receiver.
   * @param rxKBitrate The bitrate of the received audio (Kbps).
   *
   */
  virtual void onRemoteAudioTransportStats(uid_t uid, unsigned short delay, unsigned short lost, unsigned short rxKBitRate) __deprecated {
    (void)uid;
    (void)delay;
    (void)lost;
    (void)rxKBitRate;
  }

  /**
   * @brief Reports the transport-layer statistics of each remote video stream.
   *
   * @deprecated Use `onRemoteVideoStats` instead.
   *
   * @details
   * This callback reports the transport-layer statistics, such as the packet loss rate and network
   * time delay after the local user receives a video packet from a remote user.
   * During a call, when the user receives the video packet sent by the remote user/host, the callback
   * is triggered every 2 seconds.
   *
   * @param uid The ID of the remote user sending the video packets.
   * @param delay The network delay (ms) from the sender to the receiver.
   * @param lost The packet loss rate (%) of the video packet sent from the remote user.
   * @param rxKBitRate The bitrate of the received video (Kbps).
   *
   */
  virtual void onRemoteVideoTransportStats(uid_t uid, unsigned short delay, unsigned short lost, unsigned short rxKBitRate) __deprecated {
    (void)uid;
    (void)delay;
    (void)lost;
    (void)rxKBitRate;
  }

  /**
   * @brief Occurs when the network connection state changes.
   *
   * @details
   * When the network connection state changes, the SDK triggers this callback and reports the current
   * connection state and the reason for the change.
   *
   * @param state The current connection state. See `CONNECTION_STATE_TYPE`.
   * @param reason The reason for a connection state change. See `CONNECTION_CHANGED_REASON_TYPE`.
   *
   */
  virtual void onConnectionStateChanged(
      CONNECTION_STATE_TYPE state, CONNECTION_CHANGED_REASON_TYPE reason) {
    (void)state;
    (void)reason;
  }

  /**
   * @brief Occurs when the local network type changes.
   *
   * @details
   * This callback occurs when the connection state of the local user changes. You can get the
   * connection state and reason for the state change in this callback. When the network connection is
   * interrupted, this callback indicates whether the interruption is caused by a network type change
   * or poor network conditions.
   *
   * @param type The type of the local network connection. See `NETWORK_TYPE`.
   *
   */
  virtual void onNetworkTypeChanged(NETWORK_TYPE type) {
    (void)type;
  }

  /**
   * @brief Reports the built-in encryption errors.
   *
   * @details
   * When encryption is enabled by calling `enableEncryption`, the SDK triggers this callback if an
   * error occurs in encryption or decryption on the sender or the receiver side.
   *
   * @param errorType Details about the error type. See `ENCRYPTION_ERROR_TYPE`.
   *
   */
  virtual void onEncryptionError(ENCRYPTION_ERROR_TYPE errorType) {
    (void)errorType;
  }

  /**
   * @brief Occurs when the SDK cannot get the device permission.
   *
   * @details
   * When the SDK fails to get the device permission, the SDK triggers this callback to report which
   * device permission cannot be got.
   *
   * @param permissionType The type of the device permission. See `PERMISSION_TYPE`.
   *
   */
  virtual void onPermissionError(PERMISSION_TYPE permissionType) {
    (void)permissionType;
  }

#if defined(__ANDROID__)
  /**
    * Reports the permission granted.
    * @param permission {@link PERMISSION}
    */
  virtual void onPermissionGranted(agora::rtc::PERMISSION_TYPE permissionType) {}
#endif

  /**
   * @brief Occurs when the local user registers a user account.
   *
   * @details
   * After the local user successfully calls `registerLocalUserAccount` to register the user account
   * or calls `joinChannelWithUserAccount(const char* token, const char* channelId, const char*
   * userAccount, const ChannelMediaOptions& options)` to join a channel, the SDK triggers the
   * callback and
   * informs the local user's UID and User Account.
   *
   * @param uid The ID of the local user.
   * @param userAccount The user account of the local user.
   *
   */
  virtual void onLocalUserRegistered(uid_t uid, const char* userAccount) {
    (void)uid;
    (void)userAccount;
  }

  /**
   * @brief Occurs when the SDK gets the user ID and user account of the remote user.
   *
   * @details
   * After a remote user joins the channel, the SDK gets the UID and user account of the remote user,
   * caches them in a mapping table object, and triggers this callback on the local client.
   *
   * @param uid The user ID of the remote user.
   * @param info The UserInfo object that contains the user ID and user account of the remote user.
   * See `UserInfo` for details.
   *
   */
  virtual void onUserInfoUpdated(uid_t uid, const UserInfo& info) {
    (void)uid;
    (void)info;
  }

  /**
   * Occurs when the user account is updated.
   *
   * @param uid The user ID.
   * @param userAccount The user account.
   */
  virtual void onUserAccountUpdated(uid_t uid, const char* userAccount){
    (void)uid;
    (void)userAccount;
  }

  /**
   * @brief Video frame rendering event callback.
   *
   * @details
   * After calling the `startMediaRenderingTracing` method or joining a channel, the SDK triggers this
   * callback to report the events of video frame rendering and the indicators during the rendering
   * process. Developers can optimize the indicators to improve the efficiency of the first video
   * frame rendering.
   *
   * @param uid The user ID.
   * @param currentEvent The current video frame rendering event. See `MEDIA_TRACE_EVENT`.
   * @param tracingInfo The indicators during the video frame rendering process. Developers need to
   * reduce the value of indicators as much as possible in order to improve the efficiency of the
   * first video frame rendering. See `VideoRenderingTracingInfo`.
   *
   */
  virtual void onVideoRenderingTracingResult(uid_t uid, MEDIA_TRACE_EVENT currentEvent, VideoRenderingTracingInfo tracingInfo) {
    (void)uid;
    (void)currentEvent;
    (void)tracingInfo;
  }

  /**
   * @brief Occurs when there's an error during the local video mixing.
   *
   * @details
   * When you fail to call `startLocalVideoTranscoder` or `updateLocalTranscoderConfiguration`, the
   * SDK triggers this callback to report the reason.
   *
   * @param stream The video streams that cannot be mixed during video mixing. See
   * `TranscodingVideoStream`.
   * @param error The reason for local video mixing error. See `VIDEO_TRANSCODER_ERROR`.
   *
   */
  virtual void onLocalVideoTranscoderError(const TranscodingVideoStream& stream, VIDEO_TRANSCODER_ERROR error){
    (void)stream;
    (void)error;
  }

  /**
   * Reports the user log upload result
   * @param requestId RequestId of the upload
   * @param success Is upload success
   * @param reason Reason of the upload, 0: OK, 1 Network Error, 2 Server Error.
   */
  virtual void onUploadLogResult(const char* requestId, bool success, UPLOAD_ERROR_REASON reason) {
    (void)requestId;
    (void)success;
    (void)reason;
  }

  /**
   * @brief Occurs when the audio subscribing state changes.
   *
   * @param channel The channel name.
   * @param uid The user ID of the remote user.
   * @param oldState The previous subscribing status. See `STREAM_SUBSCRIBE_STATE`.
   * @param newState The current subscribing status. See `STREAM_SUBSCRIBE_STATE`.
   * @param elapseSinceLastState The time elapsed (ms) from the previous state to the current state.
   *
   */
  virtual void onAudioSubscribeStateChanged(const char* channel, uid_t uid, STREAM_SUBSCRIBE_STATE oldState, STREAM_SUBSCRIBE_STATE newState, int elapseSinceLastState) {
    (void)channel;
    (void)uid;
    (void)oldState;
    (void)newState;
    (void)elapseSinceLastState;
  }

  /**
   * @brief Occurs when the video subscribing state changes.
   *
   * @param channel The channel name.
   * @param uid The user ID of the remote user.
   * @param oldState The previous subscribing status. See `STREAM_SUBSCRIBE_STATE`.
   * @param newState The current subscribing status. See `STREAM_SUBSCRIBE_STATE`.
   * @param elapseSinceLastState The time elapsed (ms) from the previous state to the current state.
   *
   */
  virtual void onVideoSubscribeStateChanged(const char* channel, uid_t uid, STREAM_SUBSCRIBE_STATE oldState, STREAM_SUBSCRIBE_STATE newState, int elapseSinceLastState) {
    (void)channel;
    (void)uid;
    (void)oldState;
    (void)newState;
    (void)elapseSinceLastState;
  }

  /**
   * @brief Occurs when the audio publishing state changes.
   *
   * @param channel The channel name.
   * @param oldState The previous publishing state. See `STREAM_PUBLISH_STATE`.
   * @param newState The current publishing stat. See `STREAM_PUBLISH_STATE`.
   * @param elapseSinceLastState The time elapsed (ms) from the previous state to the current state.
   *
   */
  virtual void onAudioPublishStateChanged(const char* channel, STREAM_PUBLISH_STATE oldState, STREAM_PUBLISH_STATE newState, int elapseSinceLastState) {
    (void)channel;
    (void)oldState;
    (void)newState;
    (void)elapseSinceLastState;
  }

  /**
   * @brief Occurs when the video publishing state changes.
   *
   * @param channel The channel name.
   * @param source The type of the video source. See `VIDEO_SOURCE_TYPE`.
   * @param oldState The previous publishing state. See `STREAM_PUBLISH_STATE`.
   * @param newState The current publishing stat. See `STREAM_PUBLISH_STATE`.
   * @param elapseSinceLastState The time elapsed (ms) from the previous state to the current state.
   *
   */
  virtual void onVideoPublishStateChanged(VIDEO_SOURCE_TYPE source, const char* channel, STREAM_PUBLISH_STATE oldState, STREAM_PUBLISH_STATE newState, int elapseSinceLastState) {
    (void)source;
    (void)channel;
    (void)oldState;
    (void)newState;
    (void)elapseSinceLastState;
  }

  /**
   * @brief Occurs when the local user receives a mixed video stream carrying layout information.
   *
   * @details
   * When the local user receives a mixed video stream sent by the video mixing server for the first
   * time, or when there is a change in the layout information of the mixed stream, the SDK triggers
   * this callback, reporting the layout information of each sub-video stream within the mixed video
   * stream.
   *
   * @note This callback is for Android and iOS only.
   *
   * @param uid User ID who published this mixed video stream.
   * @param width Width (px) of the mixed video stream.
   * @param height Heitht (px) of the mixed video stream.
   * @param layoutCount The number of layout information in the mixed video stream.
   * @param layoutlist Layout information of a specific sub-video stream within the mixed stream. See
   * `VideoLayout`.
   *
   */
  virtual void onTranscodedStreamLayoutInfo(uid_t uid, int width, int height, int layoutCount,const VideoLayout* layoutlist) {
    (void)uid;
    (void)width;
    (void)height;
    (void)layoutCount;
    (void)layoutlist;
  }

  /**
   * Occurs when the SDK receives audio metadata.
   * @since v4.3.1
   * @param uid ID of the remote user.
   * @param metadata The pointer of metadata
   * @param length Size of metadata
   * @technical preview
   */
  virtual void onAudioMetadataReceived(uid_t uid, const char* metadata, size_t length) {
    (void)uid;
    (void)metadata;
    (void)length;
  }

  /**
   * @brief The event callback of the extension.
   *
   * @details
   * To listen for events while the extension is running, you need to register this callback.
   *
   * @param context The context information of the extension, see `ExtensionContext`.
   * @param key The key of the extension.
   * @param value The value of the extension key.
   *
   */
  virtual void onExtensionEventWithContext(const ExtensionContext &context, const char* key, const char* value) {
    (void)context;
    (void)key;
    (void)value;
  }

  /**
   * @brief Occurrs when the extension is enabled.
   *
   * @details
   * The callback is triggered after the extension is successfully enabled.
   *
   * @param context The context information of the extension, see `ExtensionContext`.
   *
   */
  virtual void onExtensionStartedWithContext(const ExtensionContext &context) {
    (void)context;
  }

  /**
   * @brief Occurs when the extension is disabled.
   *
   * @details
   * The callback is triggered after the extension is successfully disabled.
   *
   * @param context The context information of the extension, see `ExtensionContext`.
   *
   */
  virtual void onExtensionStoppedWithContext(const ExtensionContext &context) {
    (void)context;
  }

  /**
   * @brief Occurs when the extension runs incorrectly.
   *
   * @details
   * In case of extension enabling failure or runtime errors, the extension triggers this callback and
   * reports the error code along with the reasons.
   *
   * @param context The context information of the extension, see `ExtensionContext`.
   * @param error Error code. For details, see the extension documentation provided by the extension
   * provider.
   * @param message Reason. For details, see the extension documentation provided by the extension
   * provider.
   *
   */
  virtual void onExtensionErrorWithContext(const ExtensionContext &context, int error, const char* message) {
    (void)context;
    (void)error;
    (void)message;
  }

  /**
   * Occurs when the SDK receives RTM setting change response.
   *
   * @technical preview
   * @param code The error code.
   */
  virtual void onSetRtmFlagResult(int code) {
    (void)code;
  }

  /**
   * @brief Report the multipath transmission statistics
   *
   * @post This callback is triggered after you set `enableMultipath` to `true` to enable multipath transmission.
   *
   * @since 4.6.0
   *
   * @param stats The multipath statistics. See the MultipathStats structure for details.
   */  virtual void onMultipathStats(const MultipathStats& stats) {
    (void)stats;
  }

  /**
   * @brief Callback for `renewToken` call result.
   *
   * @since 4.6.0
   *
   * @details
   * This callback is triggered after the user calls the `renewToken` method to update the token, and
   * is used to notify the app of the result.
   *
   * @param token Token.
   * @param code Error code. See `RENEW_TOKEN_ERROR_CODE`.
   *
   */
  virtual void onRenewTokenResult(const char* token, RENEW_TOKEN_ERROR_CODE code) {
    (void)token;
    (void)code;
  }
};

/**
 * The IVideoDeviceCollection class. You can get information related to video devices through this interface.
 */
class IVideoDeviceCollection {
 public:
  virtual ~IVideoDeviceCollection() {}

  /**
   * @brief Gets the total number of the indexed video devices in the system.
   *
   * @return
   * The total number of the indexed video devices in the system.
   */
  virtual int getCount() = 0;

  /**
   * Specifies a device with the device ID.
   *
   * @param deviceIdUTF8 The device ID. The maximum length is #MAX_DEVICE_ID_LENGTH_TYPE. Plugging or
   * unplugging the audio device does not change the value of deviceId.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setDevice(const char deviceIdUTF8[MAX_DEVICE_ID_LENGTH]) = 0;

  /**
   * Gets a specified piece of information about an indexed video device.
   *
   * @param index The index value of the video device. The value of this parameter must be less than the value returned in `getCount`.
   * @param deviceNameUTF8 The name of the device. The maximum length is #MAX_DEVICE_ID_LENGTH.
   * @param deviceIdUTF8 The device ID of the video device. The maximum length is #MAX_DEVICE_ID_LENGTH.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int getDevice(int index, char deviceNameUTF8[MAX_DEVICE_ID_LENGTH],
                        char deviceIdUTF8[MAX_DEVICE_ID_LENGTH]) = 0;

  /**
   * @brief Releases all the resources occupied by the `IVideoDeviceCollection` object.
   */
  virtual void release() = 0;
};

/**
 * The IVideoDeviceManager class.
 */
class IVideoDeviceManager {
 public:
  virtual ~IVideoDeviceManager() {}
  /**
   * @brief Enumerates the video devices.
   *
   * @details
   * This method returns an `IVideoDeviceCollection` object including all video devices in the system.
   * With the `IVideoDeviceCollection` object, the application can enumerate video devices. The
   * application must call the `release` method to release the returned object after using it.
   *
   * @note This method is for Windows and macOS only.
   *
   * @return
   * - Success: One `IVideoDeviceCollection` object including all video devices in the system.
   */
  virtual IVideoDeviceCollection* enumerateVideoDevices() = 0;

  /**
   * Specifies the video capture device with the device ID.
   *
   * @param deviceIdUTF8 he device ID. You can get the device ID by calling `enumerateVideoDevices`.
   * The maximum length is #MAX_DEVICE_ID_LENGTH.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setDevice(const char deviceIdUTF8[MAX_DEVICE_ID_LENGTH]) = 0;

  /**
   * @brief Retrieves the current video capture device.
   *
   * @note This method is for Windows and macOS only.
   *
   * @param deviceIdUTF8 An output parameter. The device ID. The maximum length is
   * `MAX_DEVICE_ID_LENGTH_TYPE`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int getDevice(char deviceIdUTF8[MAX_DEVICE_ID_LENGTH]) = 0;

#if defined(_WIN32) || (defined(__linux__) && !defined(__ANDROID__) && !defined(__OHOS__)) || \
    (defined(__APPLE__) && TARGET_OS_MAC && !TARGET_OS_IPHONE)
  /**
   * @brief Gets the number of video formats supported by the specified video capture device.
   *
   * @details
   * Video capture devices may support multiple video formats, and each format supports different
   * combinations of video frame width, video frame height, and frame rate.
   * You can call this method to get how many video formats the specified video capture device can
   * support, and then call `getCapability` to get the specific video frame information in the
   * specified video format.
   *
   * @note This method is for Windows and macOS only.
   *
   * @param deviceIdUTF8 The ID of the video capture device.
   *
   * @return
   * - > 0: Success. Returns the number of video formats supported by this device. For example: If the
   * specified camera supports 10 different video formats, the return value is 10.
   * - ≤ 0: Failure.
   */
  virtual int numberOfCapabilities(const char* deviceIdUTF8) = 0;

  /**
   * @brief Gets the detailed video frame information of the video capture device in the specified
   * video format.
   *
   * @details
   * After calling `numberOfCapabilities` to get the number of video formats supported by the video
   * capture device, you can call this method to get the specific video frame information supported by
   * the specified index number.
   *
   * @note This method is for Windows and macOS only.
   *
   * @param deviceIdUTF8 The ID of the video capture device.
   * @param deviceCapabilityNumber The index number of the video format. If the return value of
   * `numberOfCapabilities` is i, the value range of this parameter is [0,i).
   * @param capability An output parameter. Indicates the specific information of the specified video
   * format, including width (px), height (px), and frame rate (fps). See `VideoFormat`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int getCapability(const char* deviceIdUTF8, const uint32_t deviceCapabilityNumber, VideoFormat& capability) = 0;
#endif
  /**
   * Starts the video capture device test.
   *
   * This method tests whether the video capture device works properly.
   * Before calling this method, ensure that you have already called
   * \ref IRtcEngine::enableVideo "enableVideo", and the HWND window handle of
   * the incoming parameter is valid.
   *
   * @param hwnd An Output parameter that specifies the window handle to display the video.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int startDeviceTest(view_t hwnd) = 0;

  /**
   * Stops the video capture device test.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int stopDeviceTest() = 0;

  /**
   * @brief Releases all the resources occupied by the `IVideoDeviceManager` object.
   *
   * @note This method is for Windows and macOS only.
   *
   */
  virtual void release() = 0;
};

/**
 * @brief Provides methods to manage and configure video effects, such as beauty, style makeup, and filter.
 *
 * @since v4.6.0
 */
class IVideoEffectObject : public RefCountInterface {
 public:
  virtual ~IVideoEffectObject() {}

  /**
   * @brief Types of applicable video effect nodes.
   *
   * @since v4.6.0
   */
  enum class VIDEO_EFFECT_NODE_ID : uint32_t {
    /**
     * (1): Beauty effect node.
     */
    BEAUTY       = 1U << 0,
    /**
     * (2): Style makeup effect node.
     */
    STYLE_MAKEUP = 1U << 1,
    /**
     * (4): Filter effect node.
     */
    FILTER       = 1U << 2,
  };

  /**
   * @brief Actions that can be performed on video effect nodes.
   *
   * @since v4.6.0
   */
  enum VIDEO_EFFECT_ACTION {
    /**
     * (1): Save the current parameters of the video effect.
     */
    SAVE = 1,
    /**
     * (2): Reset the video effect to default parameters.
     */
    RESET = 2,
  };

  /**
   * @brief Adds or updates the video effect for the specified node ID and template.
   *
   * @since v4.6.0
   *
   * @note
   * Priority rules:
   * - The `VIDEO_EFFECT_NODE_ID::STYLE_MAKEUP` node takes precedence over the
   * `VIDEO_EFFECT_NODE_ID::FILTER` parameter.
   * - To apply the `VIDEO_EFFECT_NODE_ID::FILTER` parameter, you must first remove the
   * `VIDEO_EFFECT_NODE_ID::STYLE_MAKEUP` node:
   * ```
   * removeVideoEffect(VIDEO_EFFECT_NODE_ID::STYLE_MAKEUP);
   * addOrUpdateVideoEffect(VIDEO_EFFECT_NODE_ID::FILTER, "template name");
   * ```
   *
   * @param nodeId The unique identifier or combination of identifiers for the video effect node. See
   * `VIDEO_EFFECT_NODE_ID`.
   * Examples:
   * - Single effect: `VIDEO_EFFECT_NODE_ID::BEAUTY`
   * - Combined effects: `VIDEO_EFFECT_NODE_ID::BEAUTY | VIDEO_EFFECT_NODE_ID::STYLE_MAKEUP`
   * @param templateName The name of the effect template. If set to null or an empty string, the SDK
   * loads the default configuration from the resource package.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int addOrUpdateVideoEffect(uint32_t nodeId, const char* templateName) = 0;

  /**
   * @brief Removes the video effect with the specified node ID.
   *
   * @since v4.6.0
   *
   * @param nodeId The unique identifier of the video effect node to remove. See
   * `VIDEO_EFFECT_NODE_ID`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int removeVideoEffect(uint32_t nodeId) = 0;

  /**
   * @brief Performs an action on the specified video effect node.
   *
   * @since v4.6.0
   *
   * @param nodeId The unique identifier of the video effect node.
   * @param actionId The action to perform. See `VIDEO_EFFECT_ACTION`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int performVideoEffectAction(uint32_t nodeId, VIDEO_EFFECT_ACTION actionId) = 0;

  /**
   * @brief Sets the float parameter for video effects.
   *
   * @since v4.6.0
   *
   * @param option The category of the parameter option.
   * @param key The key name of the parameter.
   * @param param The float value to set.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setVideoEffectFloatParam(const char* option, const char* key, float param) = 0;

  /**
   * @brief Sets an integer parameter for video effects.
   *
   * @since v4.6.0
   *
   * @param option The category of the option to which the parameter belongs.
   * @param key The key name of the parameter.
   * @param param The integer parameter value to set.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setVideoEffectIntParam(const char* option, const char* key, int param) = 0;

  /**
   * @brief Sets the boolean parameter for video effects.
   *
   * @since v4.6.0
   *
   * @param option The category of the parameter option.
   * @param key The key name of the parameter.
   * @param param The boolean value to set.
   * - `true`: Enables the option.
   * - `false`: Disables the option.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setVideoEffectBoolParam(const char* option, const char* key, bool param) = 0;

  /**
   * @brief Retrieves `float` type parameters in video effects.
   *
   * @since v4.6.0
   *
   * @details
   * Used to retrieve the value of a `float` type parameter corresponding to the specified option and
   * key in video effects.
   *
   * @param option The category of the option to which the parameter belongs.
   * @param key The key name of the parameter.
   *
   * @return
   * - If the parameter exists, returns the corresponding `float` value.
   * - If the parameter does not exist or an error occurs, returns 0.0f.
   */
  virtual float getVideoEffectFloatParam(const char* option, const char* key) = 0;

  /**
   * @brief Retrieves integer parameters in video effects.
   *
   * @since v4.6.0
   *
   * @details
   * Used to retrieve integer-type parameters in video effects.
   *
   * @param option The category of the parameter option.
   * @param key The key name of the parameter.
   *
   * @return
   * - If the parameter exists, returns the corresponding integer value.
   * - If the parameter does not exist or an error occurs, returns 0.
   */
  virtual int getVideoEffectIntParam(const char* option, const char* key) = 0;

  /**
   * @brief Gets the boolean parameter in video effects.
   *
   * @since v4.6.0
   *
   * @param option The option category to which the parameter belongs.
   * @param key The key name of the parameter.
   *
   * @return
   * - `true`: The parameter is enabled.
   * - `false`: The parameter is not enabled or does not exist.
   */
  virtual bool getVideoEffectBoolParam(const char* option, const char* key) = 0;

};

/**
 * @brief Configurations for the `RtcEngineContext` instance.
 */
struct RtcEngineContext {
  /**
   * The event handler for `IRtcEngine`. See `IRtcEngineEventHandler`.
   */
  IRtcEngineEventHandler* eventHandler;
  /**
   * The App ID issued by Agora for your project. Only users in apps with the same App ID can join the
   * same channel and communicate with each other. An App ID can only be used to create one
   * `IRtcEngine` instance. To change your App ID, call `release` to destroy the current `IRtcEngine`
   * instance, and then create a new one.
   */
  const char* appId;
  /**
   * - For Windows, it is the window handle of the app. Once set, this parameter enables you to
   * connect or disconnect the video devices while they are powered.
   * - For Android, it is the context of Android Activity.
   */
  void* context;
  /**
   * The channel profile. See `CHANNEL_PROFILE_TYPE`.
   */
  CHANNEL_PROFILE_TYPE channelProfile;

  /**
   * The license used for verification when connectting channel. Charge according to the license
   */
  const char* license;

  /**
   * The audio scenarios. Under different audio scenarios, the device uses different volume types. See
   * `AUDIO_SCENARIO_TYPE`.
   */
  AUDIO_SCENARIO_TYPE audioScenario;
  /**
   * The region for connection. This is an advanced feature and applies to scenarios that have
   * regional restrictions. For details on supported regions, see `AREA_CODE`. The area codes support
   * bitwise operation.
   */
  unsigned int areaCode;

  /**
   * Sets the log file size. See `LogConfig`.
   * By default, the SDK generates five SDK log files and five API call log files with the following
   * rules:
   * - The SDK log files are: `agorasdk.log`, `agorasdk.1.log`, `agorasdk.2.log`, `agorasdk.3.log`,
   * and `agorasdk.4.log`.
   * - The API call log files are: `agoraapi.log`, `agoraapi.1.log`, `agoraapi.2.log`,
   * `agoraapi.3.log`, and `agoraapi.4.log`.
   * - The default size of each SDK log file and API log file is 2,048 KB. These log files are encoded
   * in UTF-8.
   * - The SDK writes the latest logs in `agorasdk.log` or `agoraapi.log`.
   * - When `agorasdk.log` is full, the SDK processes the log files in the following order:1. Delete
   * the `agorasdk.4.log` file (if any).
   * 2. Rename `agorasdk.3.log` to `agorasdk.4.log`.
   * 3. Rename `agorasdk.2.log` to `agorasdk.3.log`.
   * 4. Rename `agorasdk.1.log` to `agorasdk.2.log`.
   * 5. Create a new `agorasdk.log` file.
   * - The overwrite rules for the `agoraapi.log` file are the same as for `agorasdk.log`.
   */
  commons::LogConfig logConfig;

  /**
   * Thread priority for SDK common threads
   */
  Optional<THREAD_PRIORITY_TYPE> threadPriority;

  /**
   * Whether to use egl context in the current thread as sdk's root egl context,
   * which is shared by all egl related modules. eg. camera capture, video renderer.
   *
   * @note
   * This property applies to Android only.
   */
  bool useExternalEglContext;

  /**
   * Whether to enable domain name restriction:
   * - `true`: Enables the domain name restriction. This value is suitable for scenarios where IoT
   * devices use IoT cards for network access. The SDK will only connect to servers in the domain name
   * or IP whitelist that has been reported to the operator.
   * - `false`: (Default) Disables the domain name restriction. This value is suitable for most common
   * scenarios.
   */
  bool domainLimit;

  /**
   * Whether to automatically register the Agora extensions when initializing `IRtcEngine`:
   * - `true`: (Default) Automatically register the Agora extensions when initializing `IRtcEngine`.
   * - `false`: Do not register the Agora extensions when initializing `IRtcEngine`. You need to call
   * `enableExtension` to register the Agora extensions.
   */
  bool autoRegisterAgoraExtensions;

  RtcEngineContext()
      : eventHandler(NULL), appId(NULL), context(NULL), channelProfile(CHANNEL_PROFILE_LIVE_BROADCASTING),
        license(NULL), audioScenario(AUDIO_SCENARIO_DEFAULT), areaCode(AREA_CODE_GLOB),
        logConfig(), useExternalEglContext(false), domainLimit(false), autoRegisterAgoraExtensions(true) {}
};

/** Definition of IMetadataObserver
*/
class IMetadataObserver {
public:
    virtual ~IMetadataObserver() {}

    /**
     * @brief Metadata type of the observer. We only support video metadata for now.
     */
    enum METADATA_TYPE
    {
        /**
         * -1: The type of metadata is unknown.
         */
        UNKNOWN_METADATA = -1,
        /**
         * 0: The type of metadata is video.
         */
        VIDEO_METADATA = 0,
    };
    /**
      * The maximum metadata size.
      */
    enum MAX_METADATA_SIZE_TYPE
    {
        INVALID_METADATA_SIZE_IN_BYTE = -1,
        DEFAULT_METADATA_SIZE_IN_BYTE = 512,
        MAX_METADATA_SIZE_IN_BYTE = 1024
    };

    /**
     * @brief Media metadata.
     */
    struct Metadata
    {
        /**
         * The channel name.
         */
        const char* channelId;
        /**
         * The user ID.
         * - For the recipient: The ID of the remote user who sent the `Metadata`.
         * - For the sender: Ignore it.
         */
        unsigned int uid;
        /**
         * The buffer size of the sent or received `Metadata`.
         */
        unsigned int size;
        /**
         * The buffer address of the received `Metadata`.
         */
        unsigned char *buffer;
        /**
         * The timestamp (ms) of when the `Metadata` is sent.
         */
        long long timeStampMs;

         Metadata() : channelId(NULL), uid(0), size(0), buffer(NULL), timeStampMs(0) {}
    };

    /**
     * @brief Occurs when the SDK requests the maximum size of the metadata.
     *
     * @details
     * After successfully complete the registration by calling `registerMediaMetadataObserver`, the SDK
     * triggers this callback once every video frame is sent. You need to specify the maximum size of
     * the metadata in the return value of this callback.
     *
     * @return
     * The maximum size of the `buffer` of the metadata that you want to use. The highest value is 1024
     * bytes. Ensure that you set the return value.
     */
    virtual int getMaxMetadataSize() { return DEFAULT_METADATA_SIZE_IN_BYTE; }

    /**
     * @brief Occurs when the SDK is ready to send metadata.
     *
     * @details
     * This callback is triggered when the SDK is ready to send metadata.
     *
     * @note Ensure that the size of the metadata does not exceed the value set in the
     * `getMaxMetadataSize` callback.
     *
     * @param source_type Video data type. See `VIDEO_SOURCE_TYPE`.
     * @param metadata The metadata that the user wants to send. See `Metadata`.
     *
     * @return
     * - `true`: Send the video frame.
     * - `false`: Do not send the video frame.
     */
    virtual bool onReadyToSendMetadata(Metadata &metadata, VIDEO_SOURCE_TYPE source_type) = 0;

    /**
     * @brief Occurs when the local user receives the metadata.
     *
     * @param metadata The metadata received. See `Metadata`.
     *
     */
    virtual void onMetadataReceived(const Metadata& metadata) = 0;
};

/**
 * @brief Reasons for the changes in CDN streaming status.
 *
 * @deprecated v4.6.0.
 */
enum DIRECT_CDN_STREAMING_REASON {
  // No error occurs.
  /**
   * 0: No error.
   */
  DIRECT_CDN_STREAMING_REASON_OK = 0,
  // A general error occurs (no specified reason).
  /**
   * 1: A general error; no specific reason. You can try to push the media stream again.
   */
  DIRECT_CDN_STREAMING_REASON_FAILED = 1,
  // Audio publication error.
  /**
   * 2: An error occurs when pushing audio streams. For example, the local audio capture device is not
   * working properly, is occupied by another process, or does not get the permission required.
   */
  DIRECT_CDN_STREAMING_REASON_AUDIO_PUBLICATION = 2,
  // Video publication error.
  /**
   * 3: An error occurs when pushing video streams. For example, the local video capture device is not
   * working properly, is occupied by another process, or does not get the permission required.
   */
  DIRECT_CDN_STREAMING_REASON_VIDEO_PUBLICATION = 3,

  /**
   * 4: Fails to connect to the CDN.
   */
  DIRECT_CDN_STREAMING_REASON_NET_CONNECT = 4,
  // Already exist stream name.
  /**
   * 5: The URL is already being used. Use a new URL for streaming.
   */
  DIRECT_CDN_STREAMING_REASON_BAD_NAME = 5,
};

/**
 * @brief The current CDN streaming state.
 *
 * @deprecated v4.6.0.
 */
enum DIRECT_CDN_STREAMING_STATE {

  /**
   * 0: The initial state before the CDN streaming starts.
   */
  DIRECT_CDN_STREAMING_STATE_IDLE = 0,

  /**
   * 1: Streams are being pushed to the CDN. The SDK returns this value when you call the
   * `startDirectCdnStreaming` method to push streams to the CDN.
   */
  DIRECT_CDN_STREAMING_STATE_RUNNING = 1,

  /**
   * 2: Stops pushing streams to the CDN. The SDK returns this value when you call the
   * `stopDirectCdnStreaming` method to stop pushing streams to the CDN.
   */
  DIRECT_CDN_STREAMING_STATE_STOPPED = 2,

  /**
   * 3: Fails to push streams to the CDN. You can troubleshoot the issue with the information reported
   * by the `onDirectCdnStreamingStateChanged` callback, and then push streams to the CDN again.
   */
  DIRECT_CDN_STREAMING_STATE_FAILED = 3,

  /**
   * 4: Tries to reconnect the Agora server to the CDN. The SDK attempts to reconnect a maximum of 10
   * times; if the connection is not restored, the streaming state becomes
   * DIRECT_CDN_STREAMING_STATE_FAILED.
   */
  DIRECT_CDN_STREAMING_STATE_RECOVERING = 4,
};

/**
 * @brief The statistics of the current CDN streaming.
 *
 * @deprecated v4.6.0.
 */
struct DirectCdnStreamingStats {
    /**
     * The width (px) of the video frame.
     */
    int videoWidth;

    /**
     * The height (px) of the video frame.
     */
    int videoHeight;

    /**
     * The frame rate (fps) of the current video frame.
     */
    int fps;

    /**
     * The bitrate (bps) of the current video frame.
     */
    int videoBitrate;

    /**
     * The bitrate (bps) of the current audio frame.
     */
    int audioBitrate;
};

/**
 * The event handler for direct cdn streaming
 *
 * @deprecated v4.6.0.
 *
 */
class IDirectCdnStreamingEventHandler {
 public:
  virtual ~IDirectCdnStreamingEventHandler() {}

  /**
   * @brief Occurs when the CDN streaming state changes.
   *
   * @details
   * When the host directly pushes streams to the CDN, if the streaming state changes, the SDK
   * triggers this callback to report the changed streaming state, error codes, and other information.
   * You can troubleshoot issues by referring to this callback.
   *
   * @param state The current CDN streaming state. See `DIRECT_CDN_STREAMING_STATE`.
   * @param reason Reasons for changes in the status of CDN streaming. See
   * `DIRECT_CDN_STREAMING_REASON`.
   * @param message The information about the changed streaming state.
   *
   */
  virtual void onDirectCdnStreamingStateChanged(DIRECT_CDN_STREAMING_STATE state, DIRECT_CDN_STREAMING_REASON reason, const char* message)  {
    (void)state;
    (void)reason;
    (void)message;
  };

  /**
   * @brief Reports the CDN streaming statistics.
   *
   * @details
   * When the host directly pushes media streams to the CDN, the SDK triggers this callback every one
   * second.
   *
   * @param stats The statistics of the current CDN streaming. See `DirectCdnStreamingStats`.
   *
   */
  virtual void onDirectCdnStreamingStats(const DirectCdnStreamingStats& stats)  {
    (void)stats;
  };
};

/**
 * @brief The media setting options for the host.
 *
 * @deprecated v4.6.0.
 */
struct DirectCdnStreamingMediaOptions {
  /**
   * Sets whether to publish the video captured by the camera:
   * - `true`: Publish the video captured by the camera.
   * - `false`: (Default) Do not publish the video captured by the camera.
   */
  Optional<bool> publishCameraTrack;
  /**
   * Sets whether to publish the audio captured by the microphone:
   * - `true`: Publish the audio captured by the microphone.
   * - `false`: (Default) Do not publish the audio captured by the microphone.
   */
  Optional<bool> publishMicrophoneTrack;
  /**
   * Sets whether to publish the captured audio from a custom source:
   * - `true`: Publish the captured audio from a custom source.
   * - `false`: (Default) Do not publish the captured audio from the custom source.
   */
  Optional<bool> publishCustomAudioTrack;
  /**
   * Sets whether to publish the captured video from a custom source:
   * - `true`: Publish the captured video from a custom source.
   * - `false`: (Default) Do not publish the captured video from the custom source.
   */
  Optional<bool> publishCustomVideoTrack;
  /**
  * Determines whether to publish the audio track of media player source.
  * - true: Publish the audio track of media player source.
  * - false: (Default) Do not publish the audio track of media player source.
  */
  Optional<bool> publishMediaPlayerAudioTrack;
  /**
  * Determines which media player source should be published.
  * You can get the MediaPlayerId after calling getMediaPlayerId() of AgoraMediaPlayer.
  */
  Optional<int> publishMediaPlayerId;
  /**
   * The video track ID returned by calling the `createCustomVideoTrack` method. The default value is
   * 0.
   */
  Optional<video_track_id_t> customVideoTrackId;

  DirectCdnStreamingMediaOptions() {}
  ~DirectCdnStreamingMediaOptions() {}

  void SetAll(const DirectCdnStreamingMediaOptions& change) {
#define SET_FROM(X) SetFrom(&X, change.X)
      SET_FROM(publishCameraTrack);
      SET_FROM(publishMicrophoneTrack);
      SET_FROM(publishCustomAudioTrack);
      SET_FROM(publishCustomVideoTrack);
      SET_FROM(publishMediaPlayerAudioTrack);
      SET_FROM(publishMediaPlayerId);
      SET_FROM(customVideoTrackId);
#undef SET_FROM
  }

  bool operator==(const DirectCdnStreamingMediaOptions& o) const {
#define BEGIN_COMPARE() bool b = true
#define ADD_COMPARE(X) b = (b && (X == o.X))
#define END_COMPARE()

      BEGIN_COMPARE();
      ADD_COMPARE(publishCameraTrack);
      ADD_COMPARE(publishMicrophoneTrack);
      ADD_COMPARE(publishCustomAudioTrack);
      ADD_COMPARE(publishCustomVideoTrack);
      ADD_COMPARE(publishMediaPlayerAudioTrack);
      ADD_COMPARE(customVideoTrackId);
      ADD_COMPARE(publishMediaPlayerId);
      END_COMPARE();

#undef BEGIN_COMPARE
#undef ADD_COMPARE
#undef END_COMPARE
      return b;
  }

  DirectCdnStreamingMediaOptions& operator=(const DirectCdnStreamingMediaOptions& replace) {
    if (this != &replace) {
#define REPLACE_BY(X) ReplaceBy(&X, replace.X)

        REPLACE_BY(publishCameraTrack);
        REPLACE_BY(publishMicrophoneTrack);
        REPLACE_BY(publishCustomAudioTrack);
        REPLACE_BY(publishCustomVideoTrack);
        REPLACE_BY(publishMediaPlayerAudioTrack);
        REPLACE_BY(customVideoTrackId);
        REPLACE_BY(publishMediaPlayerId);
#undef REPLACE_BY
    }
    return *this;
  }
};

/**
 * The information for extension.
 */
struct ExtensionInfo {
  /**
   * The type of media device.
   */
  agora::media::MEDIA_SOURCE_TYPE mediaSourceType;

  /**
   * The id of the remote user on which the extension works.
   *
   * @note remoteUid = 0 means that the extension works on all remote streams.
   */
  uid_t remoteUid;

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
  const char* channelId;

  /**
   * User ID: A 32-bit unsigned integer ranging from 1 to (2^32-1). It must be unique.
   */
  uid_t localUid;

  ExtensionInfo() : mediaSourceType(agora::media::UNKNOWN_MEDIA_SOURCE), remoteUid(0), channelId(NULL), localUid(0) {}
};

class IMediaPlayer;
class IMediaRecorder;

/**
 * @brief Callback triggered when `IRtcEngine` is released.
 *
 * @since v4.6.0
 *
 * @details
 * This callback is triggered when the `release` method is called to asynchronously release the
 * `IRtcEngine` object.
 * Call timing: This callback is triggered when the `release` method is called to asynchronously
 * release the `IRtcEngine` object.
 *
 */
using RtcEngineReleaseCallback = void(*)();

/**
 * The IRtcEngine class, which is the basic interface of the Agora SDK that implements the core functions of real-time communication.
 *
 * `IRtcEngine` provides the main methods that your app can call.
 *
 */
class IRtcEngine : public agora::base::IEngineBase {
 public:
  /**
   * @brief Releases the `IRtcEngine` instance.
   *
   * @details
   * This method releases all resources used by the Agora SDK. Use this method for apps in which users
   * occasionally make voice or video calls. When users do not make calls, you can free up resources
   * for other operations.
   * After a successful method call, you can no longer use any method or callback in the SDK anymore.
   * If you want to use the real-time communication functions again, you must call
   * `createAgoraRtcEngine` and `initialize` to create a new `IRtcEngine` instance.
   *
   * @note Agora does not recommend you calling `release` in any callback of the SDK. Otherwise, the
   * SDK cannot release the resources until the callbacks return results, which may result in a
   * deadlock.
   *
   * @param callback (Optional) Callback function pointer for setting the destruction mode of the
   * engine to either synchronous or asynchronous. See `RtcEngineReleaseCallback`.
   * - Non `nullptr`: Destroy the engine asynchronously. The method will return immediately, at which
   * point the engine resources may not have been fully released yet. After the engine is destroyed,
   * the SDK triggers `RtcEngineReleaseCallback`.
   * - `nullptr`: Destroy the engine synchronously. This method only returns after the engine
   * resources have been fully released.
   *
   */
  AGORA_CPP_API static void release(RtcEngineReleaseCallback callback = nullptr);

  /**
   * @brief Initializes `IRtcEngine`.
   *
   * @details
   * Call timing: Before calling other APIs, you must call `createAgoraRtcEngine` and `initialize` to
   * create and initialize the `IRtcEngine` object.
   *
   * @note
   * The SDK supports creating only one `IRtcEngine` instance for an app.
   * All called methods provided by the `IRtcEngine` class are executed asynchronously. Agora
   * recommends calling these methods in the same thread.
   *
   * @param context Configurations for the `IRtcEngine` instance. See `RtcEngineContext`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   *   - -1: A general error occurs (no specified reason).
   *   - -2: The parameter is invalid.
   *   - -7: The SDK is not initialized.
   *   - -22: The resource request failed. The SDK fails to allocate resources because your app
   * consumes too much system resource or the system resources are insufficient.
   *   - -101: The App ID is invalid.
   */
  virtual int initialize(const RtcEngineContext& context) = 0;

  /**
   * @brief Gets the pointer to the specified interface.
   *
   * @param iid The ID of the interface. See `INTERFACE_ID_TYPE`.
   * @param inter An output parameter. The pointer to the specified interface.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int queryInterface(INTERFACE_ID_TYPE iid, void** inter) = 0;


  /**
   * @brief Gets the SDK version.
   *
   * @param build The SDK build index.
   *
   * @return
   * The SDK version number. The format is a string.
   */
  virtual const char* getVersion(int* build) = 0;

  /**
   * @brief Gets the warning or error description.
   *
   * @param code The error code reported by the SDK.
   *
   * @return
   * The specific error description.
   */
  virtual const char* getErrorDescription(int code) = 0;

  /**
   * @brief Queries the video codec capabilities of the SDK.
   *
   * @param codecInfo Input and output parameter. An array representing the video codec capabilities
   * of the SDK. See `CodecCapInfo`.
   * - Input value: One `CodecCapInfo` defined by the user when executing this method, representing
   * the video codec capability to be queried.
   * - Output value: The `CodecCapInfo` after the method is executed, representing the actual video
   * codec capabilities of the SDK.
   * @param size Input and output parameter, represent the size of the `CodecCapInfo` array.
   * - Input value: Size of the `CodecCapInfo` defined by the user when executing the method.
   * - Output value: Size of the output `CodecCapInfo` after this method is executed.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int queryCodecCapability(CodecCapInfo* codecInfo, int& size) = 0;

  /**
   * @brief Queries device score.
   *
   * @details
   * Applicable scenarios: In high-definition or ultra-high-definition video scenarios, you can first
   * call this method to query the device's score. If the returned score is low (for example, below
   * 60), you need to lower the video resolution to avoid affecting the video experience. The minimum
   * device score required for different business scenarios is varied. For specific score
   * recommendations, please `technical support`.
   *
   * @return
   * - >0: The method call succeeeds, the value is the current device's score, the range is [0,100],
   * the larger the value, the stronger the device capability. Most devices are rated between 60 and
   * 100.
   * - < 0: Failure.
   */
  virtual int queryDeviceScore() = 0;

  /**
   * @brief Preloads a channel with `token`, `channelId`, and `uid`.
   *
   * @details
   * When audience members need to switch between different channels frequently, calling the method
   * can help shortening the time of joining a channel, thus reducing the time it takes for audience
   * members to hear and see the host.
   * If you join a preloaded channel, leave it and want to rejoin the same channel, you do not need to
   * call this method unless the token for preloading the channel expires.
   * Call timing: To improve the user experience of preloading channels, Agora recommends that before
   * joining the channel, calling this method as early as possible once confirming the channel name
   * and user information.
   *
   * @note
   * - When calling this method, ensure you set the user role as audience and do not set the audio
   * scenario as `AUDIO_SCENARIO_CHORUS`, otherwise, this method does not take effect.
   * - You also need to make sure that the channel name, user ID and token passed in for preloading
   * are the same as the values passed in when joinning the channel, otherwise, this method does not
   * take effect.
   * - One `IRtcEngine` instance supports preloading 20 channels at most. When exceeding this limit,
   * the latest 20 preloaded channels take effect.
   * Failing to preload a channel does not mean that you can't join a channel, nor will it increase
   * the time of joining a channel.
   *
   * @param token The token generated on your server for authentication. See .When the token for
   * preloading channels expires, you can update the token based on the number of channels you
   * preload.
   * - When preloading one channel, calling this method to pass in the new token.
   * - When preloading more than one channels:
   *   - If you use a wildcard token for all preloaded channels, call `updatePreloadChannelToken` to
   * update the token.Note: When generating a wildcard token, ensure the user ID is not set as 0. See
   * `Secure authentication with tokens`.
   *   - If you use different tokens to preload different channels, call this method to pass in your
   * user ID, channel name and the new token.
   * @param channelId The channel name that you want to preload. This parameter signifies the channel
   * in which users engage in real-time audio and video interaction. Under the premise of the same App
   * ID, users who fill in the same channel ID enter the same channel for audio and video interaction.
   * The string length must be less than 64 bytes. Supported characters (89 characters in total):
   * - All lowercase English letters: a to z.
   * - All uppercase English letters: A to Z.
   * - All numeric characters: 0 to 9.
   * - "!", "#", "$", "%", "&", "(", ")", "+", "-", ":", ";", "<", "=", ".", ">", "?", "@", "[", "]",
   * "^", "_", "{", "}", "|", "~", ","
   * @param uid The user ID. This parameter is used to identify the user in the channel for real-time
   * audio and video interaction. You need to set and manage user IDs yourself, and ensure that each
   * user ID in the same channel is unique. This parameter is a 32-bit unsigned integer. The value
   * range is 1 to 2^32-1. If the user ID is not assigned (or set to 0), the SDK assigns a random user
   * ID and `onJoinChannelSuccess` returns it in the callback. Your application must record and
   * maintain the returned user ID, because the SDK does not do so.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   *   - -7: The `IRtcEngine` object has not been initialized. You need to initialize the `IRtcEngine`
   * object before calling this method.
   *   - -102: The channel name is invalid. You need to pass in a valid channel name and join the
   * channel again.
   */
  virtual int preloadChannel(const char* token, const char* channelId, uid_t uid) = 0;

  /**
   * @brief Preloads a channel with `token`, `channelId`, and `userAccount`.
   *
   * @details
   * When audience members need to switch between different channels frequently, calling the method
   * can help shortening the time of joining a channel, thus reducing the time it takes for audience
   * members to hear and see the host.
   * If you join a preloaded channel, leave it and want to rejoin the same channel, you do not need to
   * call this method unless the token for preloading the channel expires.
   * Call timing: To improve the user experience of preloading channels, Agora recommends that before
   * joining the channel, calling this method as early as possible once confirming the channel name
   * and user information.
   *
   * @note
   * - When calling this method, ensure you set the user role as audience and do not set the audio
   * scenario as `AUDIO_SCENARIO_CHORUS`, otherwise, this method does not take effect.
   * - You also need to make sure that the User Account, channel ID and token passed in for preloading
   * are the same as the values passed in when joining the channel, otherwise, this method does not
   * take effect.
   * - One `IRtcEngine` instance supports preloading 20 channels at most. When exceeding this limit,
   * the latest 20 preloaded channels take effect.
   * Failing to preload a channel does not mean that you can't join a channel, nor will it increase
   * the time of joining a channel.
   *
   * @param token The token generated on your server for authentication. See .When the token for
   * preloading channels expires, you can update the token based on the number of channels you
   * preload.
   * - When preloading one channel, calling this method to pass in the new token.
   * - When preloading more than one channels:
   *   - If you use a wildcard token for all preloaded channels, call `updatePreloadChannelToken` to
   * update the token.Note: When generating a wildcard token, ensure the user ID is not set as 0. See
   * `Secure authentication with tokens`.
   *   - If you use different tokens to preload different channels, call this method to pass in your
   * user ID, channel name and the new token.
   * @param channelId The channel name that you want to preload. This parameter signifies the channel
   * in which users engage in real-time audio and video interaction. Under the premise of the same App
   * ID, users who fill in the same channel ID enter the same channel for audio and video interaction.
   * The string length must be less than 64 bytes. Supported characters (89 characters in total):
   * - All lowercase English letters: a to z.
   * - All uppercase English letters: A to Z.
   * - All numeric characters: 0 to 9.
   * - "!", "#", "$", "%", "&", "(", ")", "+", "-", ":", ";", "<", "=", ".", ">", "?", "@", "[", "]",
   * "^", "_", "{", "}", "|", "~", ","
   * @param userAccount The user account. This parameter is used to identify the user in the channel
   * for real-time audio and video engagement. You need to set and manage user accounts yourself and
   * ensure that each user account in the same channel is unique. The maximum length of this parameter
   * is 255 bytes. Ensure that you set this parameter and do not set it as NULL. Supported characters
   * are as follows(89 in total):
   * - The 26 lowercase English letters: a to z.
   * - The 26 uppercase English letters: A to Z.
   * - All numeric characters: 0 to 9.
   * - Space
   * - "!", "#", "$", "%", "&", "(", ")", "+", "-", ":", ";", "<", "=", ".", ">", "?", "@", "[", "]",
   * "^", "_", "{", "}", "|", "~", ","
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   *   - -2: The parameter is invalid. For example, the User Account is empty. You need to pass in a
   * valid parameter and join the channel again.
   *   - -7: The `IRtcEngine` object has not been initialized. You need to initialize the `IRtcEngine`
   * object before calling this method.
   *   - -102: The channel name is invalid. You need to pass in a valid channel name and join the
   * channel again.
   */
  virtual int preloadChannelWithUserAccount(const char* token, const char* channelId, const char* userAccount) = 0;

  /**
   * @brief Updates the wildcard token for preloading channels.
   *
   * @details
   * You need to maintain the life cycle of the wildcard token by yourself. When the token expires,
   * you need to generate a new wildcard token and then call this method to pass in the new token.
   * Applicable scenarios: In scenarios involving multiple channels, such as switching between
   * different channels, using a wildcard token means users do not need to apply for a new token every
   * time joinning a new channel, which can save users time for switching channels and reduce the
   * pressure on your token server.
   *
   * @param token The new token.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   *   - -2: The parameter is invalid. For example, the token is invalid. You need to pass in a valid
   * parameter and join the channel again.
   *   - -7: The `IRtcEngine` object has not been initialized. You need to initialize the `IRtcEngine`
   * object before calling this method.
   */
  virtual int updatePreloadChannelToken(const char* token) = 0;

  /**
   * @brief Joins a channel.
   *
   * @details
   * By default, the user subscribes to the audio and video streams of all the other users in the
   * channel, giving rise to usage and **billings**. To stop subscribing to a specified stream or all
   * remote streams, call the corresponding `mute` methods.
   * Call timing: Call this method after `initialize`.
   * Related callbacks: A successful call of this method triggers the following callbacks:
   * - The local client: The `onJoinChannelSuccess` and `onConnectionStateChanged` callbacks.
   * - The remote client: The `onUserJoined` callback, if a user joining the channel in the
   * COMMUNICATION profile, or a host joining a channel in the LIVE_BROADCASTING profile.
   * When the connection between the local client and Agora's server is interrupted due to poor
   * network conditions, the SDK tries reconnecting to the server. When the local client successfully
   * rejoins the channel, the SDK triggers the `onRejoinChannelSuccess` callback on the local client.
   *
   * @note
   * - This method only supports users joining one channel at a time.
   * - Users with different App IDs cannot call each other.
   * - Before joining a channel, ensure that the App ID you use to generate a token is the same as
   * that you pass in the `initialize` method; otherwise, you may fail to join the channel with the
   * token.
   *
   * @param token The token generated on your server for authentication. See .Note:
   * - (Recommended) If your project has enabled the security mode (using APP ID and Token for
   * authentication), this parameter is required.
   * - If you have only enabled the testing mode (using APP ID for authentication), this parameter is
   * optional. You will automatically exit the channel 24 hours after successfully joining in.
   * - If you need to join different channels at the same time or switch between channels, Agora
   * recommends using a wildcard token so that you don't need to apply for a new token every time
   * joining a channel. See `Secure authentication with tokens`.
   * @param channelId The channel name. This parameter signifies the channel in which users engage in
   * real-time audio and video interaction. Under the premise of the same App ID, users who fill in
   * the same channel ID enter the same channel for audio and video interaction. The string length
   * must be less than 64 bytes. Supported characters (89 characters in total):
   * - All lowercase English letters: a to z.
   * - All uppercase English letters: A to Z.
   * - All numeric characters: 0 to 9.
   * - "!", "#", "$", "%", "&", "(", ")", "+", "-", ":", ";", "<", "=", ".", ">", "?", "@", "[", "]",
   * "^", "_", "{", "}", "|", "~", ","
   * @param info (Optional) Reserved for future use.
   * @param uid The user ID. This parameter is used to identify the user in the channel for real-time
   * audio and video interaction. You need to set and manage user IDs yourself, and ensure that each
   * user ID in the same channel is unique. This parameter is a 32-bit unsigned integer. The value
   * range is 1 to 2^32-1. If the user ID is not assigned (or set to 0), the SDK assigns a random user
   * ID and `onJoinChannelSuccess` returns it in the callback. Your application must record and
   * maintain the returned user ID, because the SDK does not do so.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   *   - -2: The parameter is invalid. For example, the token is invalid, the `uid` parameter is not
   * set to an integer, or the value of a member in `ChannelMediaOptions` is invalid. You need to pass
   * in a valid parameter and join the channel again.
   *   - -3: Fails to initialize the `IRtcEngine` object. You need to reinitialize the `IRtcEngine`
   * object.
   *   - -7: The `IRtcEngine` object has not been initialized. You need to initialize the `IRtcEngine`
   * object before calling this method.
   *   - -8: The internal state of the `IRtcEngine` object is wrong. The typical cause is that after
   * calling `startEchoTest` to start a call loop test, you call this method to join the channel
   * without calling `stopEchoTest` to stop the test. You need to call `stopEchoTest` before calling
   * this method.
   *   - -17: The request to join the channel is rejected. The typical cause is that the user is
   * already in the channel. Agora recommends that you use the `onConnectionStateChanged` callback to
   * see whether the user is in the channel. Do not call this method to join the channel unless you
   * receive the `CONNECTION_STATE_DISCONNECTED` (1) state.
   *   - -102: The channel name is invalid. You need to pass in a valid channel name in `channelId` to
   * rejoin the channel.
   *   - -121: The user ID is invalid. You need to pass in a valid user ID in `uid` to rejoin the
   * channel.
   */
  virtual int joinChannel(const char* token, const char* channelId, const char* info, uid_t uid) = 0;

  /**
   * @brief Joins a channel with media options.
   *
   * @details
   * Compared to `joinChannel(const char* token, const char* channelId, const char* info, uid_t uid)`,
   * this method has the `options` parameter which is used to set
   * media options, such as whether to publish audio and video streams within a channel, or whether to
   * automatically subscribe to the audio and video streams of all remote users when joining a
   * channel. By default, the user subscribes to the audio and video streams of all the other users in
   * the channel, giving rise to usage and **billings**. To stop subscribing to other streams, set the
   * `options` parameter or call the corresponding `mute` methods.
   * Call timing: Call this method after `initialize`.
   * Related callbacks: A successful call of this method triggers the following callbacks:
   * - The local client: The `onJoinChannelSuccess` and `onConnectionStateChanged` callbacks.
   * - The remote client: The `onUserJoined` callback, if a user joining the channel in the
   * COMMUNICATION profile, or a host joining a channel in the LIVE_BROADCASTING profile.
   * When the connection between the local client and Agora's server is interrupted due to poor
   * network conditions, the SDK tries reconnecting to the server. When the local client successfully
   * rejoins the channel, the SDK triggers the `onRejoinChannelSuccess` callback on the local client.
   *
   * @note
   * - This method only supports users joining one channel at a time.
   * - Users with different App IDs cannot call each other.
   * - Before joining a channel, ensure that the App ID you use to generate a token is the same as
   * that you pass in the `initialize` method; otherwise, you may fail to join the channel with the
   * token.
   *
   * @param token The token generated on your server for authentication. See .Note:
   * - (Recommended) If your project has enabled the security mode (using APP ID and Token for
   * authentication), this parameter is required.
   * - If you have only enabled the testing mode (using APP ID for authentication), this parameter is
   * optional. You will automatically exit the channel 24 hours after successfully joining in.
   * - If you need to join different channels at the same time or switch between channels, Agora
   * recommends using a wildcard token so that you don't need to apply for a new token every time
   * joining a channel. See `Secure authentication with tokens`.
   * @param channelId The channel name. This parameter signifies the channel in which users engage in
   * real-time audio and video interaction. Under the premise of the same App ID, users who fill in
   * the same channel ID enter the same channel for audio and video interaction. The string length
   * must be less than 64 bytes. Supported characters (89 characters in total):
   * - All lowercase English letters: a to z.
   * - All uppercase English letters: A to Z.
   * - All numeric characters: 0 to 9.
   * - "!", "#", "$", "%", "&", "(", ")", "+", "-", ":", ";", "<", "=", ".", ">", "?", "@", "[", "]",
   * "^", "_", "{", "}", "|", "~", ","
   * @param uid The user ID. This parameter is used to identify the user in the channel for real-time
   * audio and video interaction. You need to set and manage user IDs yourself, and ensure that each
   * user ID in the same channel is unique. This parameter is a 32-bit unsigned integer. The value
   * range is 1 to 2^32-1. If the user ID is not assigned (or set to 0), the SDK assigns a random user
   * ID and `onJoinChannelSuccess` returns it in the callback. Your application must record and
   * maintain the returned user ID, because the SDK does not do so.
   * @param options The channel media options. See `ChannelMediaOptions`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   *   - -2: The parameter is invalid. For example, the token is invalid, the `uid` parameter is not
   * set to an integer, or the value of a member in `ChannelMediaOptions` is invalid. You need to pass
   * in a valid parameter and join the channel again.
   *   - -3: Fails to initialize the `IRtcEngine` object. You need to reinitialize the `IRtcEngine`
   * object.
   *   - -7: The `IRtcEngine` object has not been initialized. You need to initialize the `IRtcEngine`
   * object before calling this method.
   *   - -8: The internal state of the `IRtcEngine` object is wrong. The typical cause is that after
   * calling `startEchoTest` to start a call loop test, you call this method to join the channel
   * without calling `stopEchoTest` to stop the test. You need to call `stopEchoTest` before calling
   * this method.
   *   - -17: The request to join the channel is rejected. The typical cause is that the user is
   * already in the channel. Agora recommends that you use the `onConnectionStateChanged` callback to
   * see whether the user is in the channel. Do not call this method to join the channel unless you
   * receive the `CONNECTION_STATE_DISCONNECTED` (1) state.
   *   - -102: The channel name is invalid. You need to pass in a valid channel name in `channelId` to
   * rejoin the channel.
   *   - -121: The user ID is invalid. You need to pass in a valid user ID in `uid` to rejoin the
   * channel.
   */
  virtual int joinChannel(const char* token, const char* channelId, uid_t uid, const ChannelMediaOptions& options) = 0;

  /**
   * @brief Updates the channel media options after joining the channel.
   *
   * @param options The channel media options. See `ChannelMediaOptions`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   *   - -2: The value of a member in `ChannelMediaOptions` is invalid. For example, the token or the
   * user ID is invalid. You need to fill in a valid parameter.
   *   - -7: The `IRtcEngine` object has not been initialized. You need to initialize the `IRtcEngine`
   * object before calling this method.
   *   - -8: The internal state of the `IRtcEngine` object is wrong. The possible reason is that the
   * user is not in the channel. Agora recommends that you use the `onConnectionStateChanged` callback
   * to see whether the user is in the channel. If you receive the `CONNECTION_STATE_DISCONNECTED` (1)
   * or `CONNECTION_STATE_FAILED` (5) state, the user is not in the channel. You need to call
   * `joinChannel(const char* token, const char* channelId, uid_t uid, const ChannelMediaOptions&
   * options)` to join a channel before calling this method.
   */
  virtual int updateChannelMediaOptions(const ChannelMediaOptions& options) = 0;

  /**
   * @brief Leaves a channel.
   *
   * @details
   * After calling this method, the SDK terminates the audio and video interaction, leaves the current
   * channel, and releases all resources related to the session.
   * After joining the channel, you must call this method to end the call; otherwise, the next call
   * cannot be started.
   * Call timing: Call this method after joining a channel.
   * Related callbacks: A successful call of this method triggers the following callbacks:
   * - The local client: The `onLeaveChannel` callback will be triggered.
   * - The remote client: The `onUserOffline` callback will be triggered after the remote host leaves
   * the channel.
   *
   * @note
   * If you call `release` immediately after calling this method, the SDK does not trigger the
   * `onLeaveChannel` callback.
   * - This method call is asynchronous. When this method returns, it does not necessarily mean that
   * the user has left the channel.
   * - If you have called `joinChannelEx` to join multiple channels, calling this method will leave
   * all the channels you joined.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   *   - -1: A general error occurs (no specified reason).
   *   - -2: The parameter is invalid.
   *   - -7: The SDK is not initialized.
   */
  virtual int leaveChannel() = 0;

  /**
   * @brief Sets channel options and leaves the channel.
   *
   * @details
   * After calling this method, the SDK terminates the audio and video interaction, leaves the current
   * channel, and releases all resources related to the session.
   * After joining a channel, you must call this method or `leaveChannel()` to end the call,
   * otherwise, the next call cannot be started. If you have called `joinChannelEx` to join multiple
   * channels, calling this method will leave all the channels you joined.
   * Call timing: Call this method after joining a channel.
   * Related callbacks: A successful call of this method triggers the following callbacks:
   * - The local client: The `onLeaveChannel` callback will be triggered.
   * - The remote client: The `onUserOffline` callback will be triggered after the remote host leaves
   * the channel.
   *
   * @note
   * If you call `release` immediately after calling this method, the SDK does not trigger the
   * `onLeaveChannel` callback.
   * This method call is asynchronous. When this method returns, it does not necessarily mean that the
   * user has left the channel.
   *
   * @param options The options for leaving the channel. See `LeaveChannelOptions`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int leaveChannel(const LeaveChannelOptions& options) = 0;

  /**
   * @brief Renews the token.
   *
   * @details
   * This method is used to update the token. After successfully calling this method, the SDK will
   * trigger the `onRenewTokenResult` callback. A token will expire after a certain period of time, at
   * which point the SDK will be unable to establish a connection with the server.
   * Call timing: In any of the following cases, Agora recommends that you generate a new token on
   * your server and then call this method to renew your token:
   * - Receiving the `onTokenPrivilegeWillExpire` callback reporting the token is about to expire.
   * - Receiving the `onRequestToken` callback reporting the token has expired.
   * - Receiving the `onConnectionStateChanged` callback reporting `CONNECTION_CHANGED_TOKEN_EXPIRED`
   * (9).
   *
   * @param token The new token.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   *   - -2: The parameter is invalid. For example, the token is empty.
   *   - -7: The `IRtcEngine` object has not been initialized. You need to initialize the `IRtcEngine`
   * object before calling this method.
   *   - 110: Invalid token. Ensure the following:
   *     - The user ID specified when generating the token is consistent with the user ID used when
   * joining the channel.
   *     - The generated token is the same as the token passed in to join the channel.
   */
  virtual int renewToken(const char* token) = 0;

  /**
   * @brief Sets the channel profile.
   *
   * @details
   * You can call this method to set the channel profile. The SDK adopts different optimization
   * strategies for different channel profiles. For example, in a live streaming scenario, the SDK
   * prioritizes video quality. After initializing the SDK, the default channel profile is the live
   * streaming profile.
   * Call timing: Call this method before joining a channel.
   *
   * @note
   * To ensure the quality of real-time communication, Agora recommends that all users in a channel
   * use the same channel profile.
   * In different channel scenarios, the default audio routing of the SDK is different. See
   * `setDefaultAudioRouteToSpeakerphone`.
   *
   * @param profile The channel profile. See `CHANNEL_PROFILE_TYPE`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   *   - -2: The parameter is invalid.
   *   - -7: The SDK is not initialized.
   */
  virtual int setChannelProfile(CHANNEL_PROFILE_TYPE profile) = 0;

  /**
   * @brief Sets the client role.
   *
   * @details
   * By default,the SDK sets the user role as audience. You can call this method to set the user role
   * as host. The user role ( `roles` ) determines the users' permissions at the SDK level, including
   * whether they can publish audio and video streams in a channel.
   * Call timing: You can call this method either before or after joining a channel.
   * If you call this method to set the user role as the host before joining the channel and set the
   * local video property through the `setupLocalVideo` method, the local video preview is
   * automatically enabled when the user joins the channel.
   * If you call this method to set the user role after joining a channel, the SDK will automatically
   * call the `muteLocalAudioStream` and `muteLocalVideoStream` method to change the state for
   * publishing audio and video streams.
   * Related callbacks: If you call this method to switch the user role after joining the channel, the
   * SDK triggers the following callbacks:
   * - Triggers `onClientRoleChanged` on the local client.Note: Calling this method before joining a
   * channel and set the `role` to `AUDIENCE` will trigger this callback as well.
   * - Triggers `onUserJoined` or `onUserOffline` on the remote client.
   * If you call this method to set the user role after joining a channel but encounter a failure, the
   * SDK trigger the `onClientRoleChangeFailed` callback to report the reason for the failure and the
   * current user role.
   *
   * @note
   * When calling this method before joining a channel and setting the user role to `BROADCASTER`, the
   * `onClientRoleChanged` callback will not be triggered on the local client.
   * Calling this method before joining a channel and set the `role` to `AUDIENCE` will trigger this
   * callback as well.
   *
   * @param role The user role. See `CLIENT_ROLE_TYPE`.
   * Note: If you set the user role as an audience member, you cannot publish audio and video streams
   * in the channel. If you want to publish media streams in a channel during live streaming, ensure
   * you set the user role as broadcaster.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   *   - -1: A general error occurs (no specified reason).
   *   - -2: The parameter is invalid.
   *   - -7: The SDK is not initialized.
   */
  virtual int setClientRole(CLIENT_ROLE_TYPE role) = 0;

  /**
   * @brief Sets the user role and the audience latency level in a live streaming scenario.
   *
   * @details
   * By default,the SDK sets the user role as audience. You can call this method to set the user role
   * as host. The user role ( `roles` ) determines the users' permissions at the SDK level, including
   * whether they can publish audio and video streams in a channel.
   * The difference between this method and `setClientRole(CLIENT_ROLE_TYPE role)` is that, this
   * method supports
   * setting the `audienceLatencyLevel`. `audienceLatencyLevel` needs to be used together with `role`
   * to determine the level of service that users can enjoy within their permissions. For example, an
   * audience member can choose to receive remote streams with low latency or ultra-low latency.
   * Call timing: You can call this method either before or after joining a channel.
   * If you call this method to set the user role as the host before joining the channel and set the
   * local video property through the `setupLocalVideo` method, the local video preview is
   * automatically enabled when the user joins the channel.
   * If you call this method to set the user role after joining a channel, the SDK will automatically
   * call the `muteLocalAudioStream` and `muteLocalVideoStream` method to change the state for
   * publishing audio and video streams.
   * Related callbacks: If you call this method to switch the user role after joining the channel, the
   * SDK triggers the following callbacks:
   * - Triggers `onClientRoleChanged` on the local client.Note: Calling this method before joining a
   * channel and set the `role` to `AUDIENCE` will trigger this callback as well.
   * - Triggers `onUserJoined` or `onUserOffline` on the remote client.
   * If you call this method to set the user role after joining a channel but encounter a failure, the
   * SDK trigger the `onClientRoleChangeFailed` callback to report the reason for the failure and the
   * current user role.
   *
   * @note
   * When the user role is set to host, the audience latency level can only be set to
   * AUDIENCE_LATENCY_LEVEL_ULTRA_LOW_LATENCY.
   * When calling this method before joining a channel and setting the `role` to `BROADCASTER`, the
   * `onClientRoleChanged` callback will not be triggered on the local client.
   * Calling this method before joining a channel and set the `role` to `AUDIENCE` will trigger this
   * callback as well.
   *
   * @param role The user role. See `CLIENT_ROLE_TYPE`.
   * Note: If you set the user role as an audience member, you cannot publish audio and video streams
   * in the channel. If you want to publish media streams in a channel during live streaming, ensure
   * you set the user role as broadcaster.
   * @param options The detailed options of a user, including the user level. See `ClientRoleOptions`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   *   - -1: A general error occurs (no specified reason).
   *   - -2: The parameter is invalid.
   *   - -5: The request is rejected.
   *   - -7: The SDK is not initialized.
   */
  virtual int setClientRole(CLIENT_ROLE_TYPE role, const ClientRoleOptions& options) = 0;

  /**
   * @brief Starts an audio device loopback test.
   *
   * @details
   * To test whether the user's local sending and receiving streams are normal, you can call this
   * method to perform an audio and video call loop test, which tests whether the audio and video
   * devices and the user's upstream and downstream networks are working properly.
   * After starting the test, the user needs to make a sound or face the camera. The audio or video is
   * output after about two seconds. If the audio playback is normal, the audio device and the user's
   * upstream and downstream networks are working properly; if the video playback is normal, the video
   * device and the user's upstream and downstream networks are working properly.
   * Call timing: You can call this method either before or after joining a channel.
   *
   * @note
   * - When calling in a channel, make sure that no audio or video stream is being published.
   * - After calling this method, call `stopEchoTest` to end the test; otherwise, the user cannot
   * perform the next audio and video call loop test and cannot join the channel.
   * - In live streaming scenarios, this method only applies to hosts.
   *
   * @param config The configuration of the audio and video call loop test. See
   * `EchoTestConfiguration`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int startEchoTest(const EchoTestConfiguration& config) = 0;

  /**
   * @brief Stops the audio call test.
   *
   * @details
   * After calling `startEchoTest`, you must call this method to end the test; otherwise, the user
   * cannot perform the next audio and video call loop test and cannot join the channel.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   *   - -5(ERR_REFUSED): Failed to stop the echo test. The echo test may not be running.
   */
  virtual int stopEchoTest() = 0;

#if defined(__APPLE__) && TARGET_OS_IOS
  /**
   * @brief Enables or disables multi-camera capture.
   *
   * @details
   * In scenarios where there are existing cameras to capture video, Agora recommends that you use the
   * following steps to capture and publish video with multiple cameras:1. Call this method to enable
   * multi-channel camera capture.
   * 2. Call `startPreview(VIDEO_SOURCE_TYPE sourceType)` to start the local video preview.
   * 3. Call `startCameraCapture`, and set `sourceType` to start video capture with the second camera.
   * 4. Call `joinChannelEx`, and set `publishSecondaryCameraTrack` to `true` to publish the video
   * stream captured by the second camera in the channel.
   * If you want to disable multi-channel camera capture, use the following steps:1. Call
   * `stopCameraCapture`.
   * 2. Call this method with `enabled` set to `false`.
   * This method applies to iOS only.
   * When using this function, ensure that the system version is 13.0 or later.
   * The minimum iOS device types that support multi-camera capture are as follows:
   * - iPhone XR
   * - iPhone XS
   * - iPhone XS Max
   * - iPad Pro 3rd generation and later
   *
   * @note
   * You can call this method before and after `startPreview(VIDEO_SOURCE_TYPE sourceType)` to enable
   * multi-camera capture:
   * - If it is enabled before `startPreview(VIDEO_SOURCE_TYPE sourceType)`, the local video preview
   * shows the image captured
   * by the two cameras at the same time.
   * - If it is enabled after `startPreview(VIDEO_SOURCE_TYPE sourceType)`, the SDK stops the current
   * camera capture first,
   * and then enables the primary camera and the second camera. The local video preview appears black
   * for a short time, and then automatically returns to normal.
   *
   * @param enabled Whether to enable multi-camera video capture mode:
   * - `true`: Enable multi-camera capture mode; the SDK uses multiple cameras to capture video.
   * - `false`: Disable multi-camera capture mode; the SDK uses a single camera to capture video.
   * @param config Capture configuration for the second camera. See `CameraCapturerConfiguration`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int enableMultiCamera(bool enabled, const CameraCapturerConfiguration& config) = 0;
#endif
  /**
   * @brief Enables the video module.
   *
   * @details
   * The video module is disabled by default, call this method to enable it. If you need to disable
   * the video module later, you need to call `disableVideo`.
   * Call timing: This method can be called either before joining a channel or while in the channel:
   * - If called before joining a channel, it enables the video module.
   * - If called during an audio-only call, the audio call automatically switches to a video call.
   * Related callbacks: A successful call of this method triggers the `onRemoteVideoStateChanged`
   * callback on the remote client.
   *
   * @note
   * - This method enables the internal engine and is valid after leaving the channel.
   * - Calling this method will reset the entire engine, resulting in a slow response time. You can
   * use the following methods to independently control a specific function of the video module based
   * on your actual needs:
   *   - `enableLocalVideo`: Whether to enable the camera to create the local video stream.
   *   - `muteLocalVideoStream`: Whether to publish the local video stream.
   *   - `muteRemoteVideoStream`: Whether to subscribe to and play the remote video stream.
   *   - `muteAllRemoteVideoStreams`: Whether to subscribe to and play all remote video streams.
   * - A successful call of this method resets `enableLocalVideo`, `muteRemoteVideoStream`, and
   * `muteAllRemoteVideoStreams`. Proceed it with caution.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int enableVideo() = 0;

  /**
   * @brief Disables the video module.
   *
   * @details
   * This method is used to disable the video module.
   * Call timing: This method can be called either before or after joining the channel.
   * - If it is called before joining the channel, the audio-only mode is enabled.
   * - If it is called after joining the channel, it switches from video mode to audio-only mode.
   * Then, calling `enableVideo` can swithch to video mode again.
   * Related callbacks: A successful call of this method triggers the `onUserEnableVideo` (`false`)
   * callback on the remote client.
   *
   * @note
   * - This method affects the internal engine and can be called after leaving the channel.
   * - Calling this method will reset the entire engine, resulting in a slow response time. You can
   * use the following methods to independently control a specific function of the video module based
   * on your actual needs:
   *   - `enableLocalVideo`: Whether to enable the camera to create the local video stream.
   *   - `muteLocalVideoStream`: Whether to publish the local video stream.
   *   - `muteRemoteVideoStream`: Whether to subscribe to and play the remote video stream.
   *   - `muteAllRemoteVideoStreams`: Whether to subscribe to and play all remote video streams.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int disableVideo() = 0;

  /**
   * @brief Enables the local video preview.
   *
   * @details
   * You can call this method to enable local video preview.
   * Call timing: This method must be called after `enableVideo` and `setupLocalVideo`.
   *
   * @note
   * - The local preview enables the mirror mode by default.
   * - After leaving the channel, local preview remains enabled. You need to call `stopPreview()`
   * to disable local preview.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int startPreview() = 0;

  /**
   * @brief Enables the local video preview and specifies the video source for the preview.
   *
   * @details
   * This method is used to start local video preview and specify the video source that appears in the
   * preview screen.
   * Call timing: This method must be called after `enableVideo` and `setupLocalVideo`.
   *
   * @note
   * - The local preview enables the mirror mode by default.
   * - After leaving the channel, local preview remains enabled. You need to call `stopPreview()`
   * to disable local preview.
   *
   * @param sourceType The type of the video source. See `VIDEO_SOURCE_TYPE`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int startPreview(VIDEO_SOURCE_TYPE sourceType) = 0;

  /**
   * @brief Stops the local video preview.
   *
   * @details
   * Applicable scenarios: After calling `startPreview()` to start the preview, if you want to
   * stop the local video preview, call this method.
   * Call timing: Call this method before joining a channel or after leaving a channel.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int stopPreview() = 0;

  /**
   * @brief Stops the local video preview.
   *
   * @details
   * Applicable scenarios: After calling `startPreview(VIDEO_SOURCE_TYPE sourceType)` to start the
   * preview, if you want to
   * stop the local video preview, call this method.
   * Call timing: Call this method before joining a channel or after leaving a channel.
   *
   * @param sourceType The type of the video source. See `VIDEO_SOURCE_TYPE`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int stopPreview(VIDEO_SOURCE_TYPE sourceType) = 0;

  /**
   * @brief Starts the last mile network probe test.
   *
   * @details
   * This method starts the last-mile network probe test before joining a channel to get the uplink
   * and downlink last mile network statistics, including the bandwidth, packet loss, jitter, and
   * round-trip time (RTT).
   * Call timing: Do not call other methods before receiving the `onLastmileQuality` and
   * `onLastmileProbeResult` callbacks. Otherwise, the callbacks may be interrupted.
   * Related callbacks: After successfully calling this method, the SDK sequentially triggers the
   * following 2 callbacks:
   * - `onLastmileQuality`: The SDK triggers this callback within two seconds depending on the network
   * conditions. This callback rates the network conditions and is more closely linked to the user
   * experience.
   * - `onLastmileProbeResult`: The SDK triggers this callback within 30 seconds depending on the
   * network conditions. This callback returns the real-time statistics of the network conditions and
   * is more objective.
   *
   * @param config The configurations of the last-mile network probe test. See `LastmileProbeConfig`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int startLastmileProbeTest(const LastmileProbeConfig& config) = 0;

  /**
   * @brief Stops the last mile network probe test.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int stopLastmileProbeTest() = 0;

  /**
   * @brief Sets the video encoder configuration.
   *
   * @details
   * Sets the encoder configuration for the local video. Each configuration profile corresponds to a
   * set of video parameters, including the resolution, frame rate, and bitrate.
   * Call timing: You can call this method either before or after joining a channel. If the user does
   * not need to reset the video encoding properties after joining the channel, Agora recommends
   * calling this method before `enableVideo` to reduce the time to render the first video frame.
   *
   * @note
   * - Both this method and the `getMirrorApplied` method support setting the mirroring effect. Agora
   * recommends that you only choose one method to set it up. Using both methods at the same time
   * causes the mirroring effect to overlap, and the mirroring settings fail.
   * - The `config` specified in this method is the maximum value under ideal network conditions. If
   * the video engine cannot render the video using the specified `config` due to unreliable network
   * conditions, the parameters further down the list are considered until a successful configuration
   * is found.
   *
   * @param config Video profile. See `VideoEncoderConfiguration`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setVideoEncoderConfiguration(const VideoEncoderConfiguration& config) = 0;

  /**
   * @brief Sets the image enhancement options.
   *
   * @details
   * Enables or disables image enhancement, and sets the options.
   * Call timing: Call this method after calling `enableVideo` or `startPreview(VIDEO_SOURCE_TYPE
   * sourceType)`.
   *
   * @note
   * - This method relies on the image enhancement dynamic library
   * `libagora_clear_vision_extension.dll`. If the dynamic library is deleted, the function cannot be
   * enabled normally.
   * - This feature has high requirements on device performance. When calling this method, the SDK
   * automatically checks the capabilities of the current device.
   *
   * @param enabled Whether to enable the image enhancement function:
   * - `true`: Enable the image enhancement function.
   * - `false`: (Default) Disable the image enhancement function.
   * @param options The image enhancement options. See `BeautyOptions`.
   * @param type The type of the media source to which the filter effect is applied. See
   * `MEDIA_SOURCE_TYPE`.Attention: In this method, this parameter supports only the following two
   * settings:
   * - Use the default value `PRIMARY_CAMERA_SOURCE` if you use camera to capture local video.
   * - Set this parameter to `CUSTOM_VIDEO_SOURCE` if you use custom video source.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   *   - -4: The current device does not support this feature. Possible reasons include:
   *     - The current device capabilities do not meet the requirements for image enhancement. Agora
   * recommends you replace it with a high-performance device.
   */
  virtual int setBeautyEffectOptions(bool enabled, const BeautyOptions& options, agora::media::MEDIA_SOURCE_TYPE type = agora::media::PRIMARY_CAMERA_SOURCE) = 0;
  /**
   * @brief Sets the face shape options and specifies the media source.
   *
   * @details
   * Calling this method allows for modifying various parts of the face, achieving slimming, enlarging
   * eyes, slimming nose, and other minor cosmetic effects all at once using preset parameters,
   * supporting fine-tuning the overall modification intensity.
   * Call timing: Call this method after calling `enableVideo`.
   *
   * @note
   * - This method only applies to Android 4.4 or later.
   * - This method relies on the image enhancement dynamic library
   * `libagora_clear_vision_extension.dll`. If the dynamic library is deleted, the function cannot be
   * enabled normally.
   * - This feature has high requirements on device performance. When calling this method, the SDK
   * automatically checks the capabilities of the current device.
   *
   * @param enabled Whether to enable the face shape effect:
   * - `true`: Enable the face shape effect.
   * - `false`: (Default) Disable the face shape effect.
   * @param options Face shaping style options, see `FaceShapeBeautyOptions`.
   * @param type The type of the media source to which the filter effect is applied. See
   * `MEDIA_SOURCE_TYPE`.Attention: In this method, this parameter supports only the following two
   * settings:
   * - Use the default value `PRIMARY_CAMERA_SOURCE` if you use camera to capture local video.
   * - Set this parameter to `CUSTOM_VIDEO_SOURCE` if you use custom video source.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   *   - -4: The current device does not support this feature. Possible reasons include:
   *     - The current device capabilities do not meet the requirements for image enhancement. Agora
   * recommends you replace it with a high-performance device.
   *     - The current device version is lower than Android 4.4 and does not support this feature.
   * Agora recommends you replace the device or upgrade the operating system.
   */
  virtual int setFaceShapeBeautyOptions(bool enabled, const FaceShapeBeautyOptions& options, agora::media::MEDIA_SOURCE_TYPE type = agora::media::PRIMARY_CAMERA_SOURCE) = 0;
  /**
   * @brief Sets the image enhancement options for facial areas and specifies the media source.
   *
   * @details
   * If the preset beauty effects implemented in the `setFaceShapeBeautyOptions` method do not meet
   * expectations, you can use this method to set beauty area options, individually fine-tune each
   * part of the face, and achieve a more refined beauty effect.
   * Call timing: Call this method after calling `setFaceShapeBeautyOptions`.
   *
   * @note
   * - This method only applies to Android 4.4 or later.
   * - This method relies on the image enhancement dynamic library
   * `libagora_clear_vision_extension.dll`. If the dynamic library is deleted, the function cannot be
   * enabled normally.
   * - This feature has high requirements on device performance. When calling this method, the SDK
   * automatically checks the capabilities of the current device.
   *
   * @param options Facial enhancement areas, see `FaceShapeAreaOptions`.
   * @param type The type of the media source to which the filter effect is applied. See
   * `MEDIA_SOURCE_TYPE`.Attention: In this method, this parameter supports only the following two
   * settings:
   * - Use the default value `PRIMARY_CAMERA_SOURCE` if you use camera to capture local video.
   * - Set this parameter to `CUSTOM_VIDEO_SOURCE` if you use custom video source.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   *   - -4: The current device does not support this feature. Possible reasons include:
   *     - The current device capabilities do not meet the requirements for image enhancement. Agora
   * recommends you replace it with a high-performance device.
   *     - The current device version is lower than Android 4.4 and does not support this feature.
   * Agora recommends you replace the device or upgrade the operating system.
   */
  virtual int setFaceShapeAreaOptions(const FaceShapeAreaOptions& options, agora::media::MEDIA_SOURCE_TYPE type = agora::media::PRIMARY_CAMERA_SOURCE) = 0;

  /**
   * @brief Gets the beauty effect options.
   *
   * @details
   * Calling this method can retrieve the current settings of the beauty effect.
   * Applicable scenarios: When the user opens the beauty style and style intensity menu in the app,
   * you can call this method to get the current beauty effect options, then refresh the menu in the
   * user interface according to the results, and update the UI.
   * Call timing: Call this method after calling `enableVideo`.
   *
   * @param options Face shaping style options, see `FaceShapeBeautyOptions`.
   * @param type The type of the media source to which the filter effect is applied. See
   * `MEDIA_SOURCE_TYPE`.Attention: In this method, this parameter supports only the following two
   * settings:
   * - Use the default value `PRIMARY_CAMERA_SOURCE` if you use camera to capture local video.
   * - Set this parameter to `CUSTOM_VIDEO_SOURCE` if you use custom video source.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int getFaceShapeBeautyOptions(FaceShapeBeautyOptions& options, agora::media::MEDIA_SOURCE_TYPE type = agora::media::PRIMARY_CAMERA_SOURCE) = 0;

  /**
   * @brief Gets the facial beauty area options.
   *
   * @details
   * Calling this method can retrieve the current settings of the beauty effect.
   * Applicable scenarios: When the user opens the facial beauty area and shaping intensity menu in
   * the app, you can call this method to get the current beauty effect options, then refresh the menu
   * in the user interface according to the results, and update the UI.
   * Call timing: Call this method after calling `enableVideo`.
   *
   * @param shapeArea Facial enhancement areas. See `FACE_SHAPE_AREA`.
   * @param options Facial enhancement areas, see `FaceShapeAreaOptions`.
   * @param type The type of the media source to which the filter effect is applied. See
   * `MEDIA_SOURCE_TYPE`.Attention: In this method, this parameter supports only the following two
   * settings:
   * - Use the default value `PRIMARY_CAMERA_SOURCE` if you use camera to capture local video.
   * - Set this parameter to `CUSTOM_VIDEO_SOURCE` if you use custom video source.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int getFaceShapeAreaOptions(agora::rtc::FaceShapeAreaOptions::FACE_SHAPE_AREA shapeArea, FaceShapeAreaOptions& options, agora::media::MEDIA_SOURCE_TYPE type = agora::media::PRIMARY_CAMERA_SOURCE) = 0;

  /**
   * @brief Sets the filter effect options and specifies the media source.
   *
   * @since v4.4.1
   *
   * @details
   * Call timing: Call this method after calling `enableVideo`.
   *
   * @note
   * - This method relies on the image enhancement dynamic library
   * `libagora_clear_vision_extension.dll`. If the dynamic library is deleted, the function cannot be
   * enabled normally.
   * - This feature has high requirements on device performance. When calling this method, the SDK
   * automatically checks the capabilities of the current device.
   *
   * @param enabled Whether to enable the filter effect:
   * - `true`: Yes.
   * - `false`: (Default) No.
   * @param options The filter effect options. See `FilterEffectOptions`.
   * @param type The type of the media source to which the filter effect is applied. See
   * `MEDIA_SOURCE_TYPE`.Attention: In this method, this parameter supports only the following two
   * settings:
   * - Use the default value `PRIMARY_CAMERA_SOURCE` if you use camera to capture local video.
   * - Set this parameter to `CUSTOM_VIDEO_SOURCE` if you use custom video source.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setFilterEffectOptions(bool enabled, const FilterEffectOptions& options, agora::media::MEDIA_SOURCE_TYPE type = agora::media::PRIMARY_CAMERA_SOURCE) = 0;


  /**
   * @brief Creates a video effect object.
   *
   * @since v4.6.0
   *
   * @details
   * Creates an `IVideoEffectObject` video effect object and returns its pointer.
   *
   * @param bundlePath The path to the video effect bundle.
   * @param type The media source type. See `MEDIA_SOURCE_TYPE`.
   *
   * @return
   * - The `IVideoEffectObject` object pointer, if the method call succeeds.
   * - An empty pointer, if the method call fails.
   */
  virtual agora_refptr<IVideoEffectObject> createVideoEffectObject(const char* bundlePath, agora::media::MEDIA_SOURCE_TYPE type = agora::media::PRIMARY_CAMERA_SOURCE) = 0;


  /**
   * @brief Destroys a video effect object.
   *
   * @since v4.6.0
   *
   * @param videoEffectObject The video effect object to be destroyed. See `IVideoEffectObject`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int destroyVideoEffectObject(agora_refptr<IVideoEffectObject> videoEffectObject) = 0;

  /**
   * @brief Sets low-light enhancement.
   *
   * @since v4.0.0
   *
   * @details
   * You can call this method to enable the color enhancement feature and set the options of the color
   * enhancement effect.
   * Applicable scenarios: The low-light enhancement feature can adaptively adjust the brightness
   * value of the video captured in situations with low or uneven lighting, such as backlit, cloudy,
   * or dark scenes. It restores or highlights the image details and improves the overall visual
   * effect of the video.
   * Call timing: Call this method after calling `enableVideo`.
   *
   * @note
   * - This method relies on the image enhancement dynamic library
   * `libagora_clear_vision_extension.dll`. If the dynamic library is deleted, the function cannot be
   * enabled normally.
   * - Dark light enhancement has certain requirements for equipment performance. The low-light
   * enhancement feature has certain performance requirements on devices. If your device overheats
   * after you enable low-light enhancement, Agora recommends modifying the low-light enhancement
   * options to a less performance-consuming level or disabling low-light enhancement entirely.
   * - If you want to prioritize image quality ( LOW_LIGHT_ENHANCE_LEVEL_HIGH_QUALITY ) when using the
   * low-light enhancement function, you need to first call `setVideoDenoiserOptions` to achieve video
   * noise reduction, the specific corresponding relationship is as follows:
   *   - When low light enhancement is set to automatic mode ( LOW_LIGHT_ENHANCE_AUTO ), video noise
   * reduction needs to be set to prioritize image quality ( VIDEO_DENOISER_LEVEL_HIGH_QUALITY ) and
   * automatic mode ( VIDEO_DENOISER_AUTO ).
   *   - When low-light enhancement is set to manual mode ( LOW_LIGHT_ENHANCE_MANUAL ), video noise
   * reduction needs to be set to prioritize image quality ( VIDEO_DENOISER_LEVEL_HIGH_QUALITY ) and
   * manual mode ( VIDEO_DENOISER_MANUAL ).
   *
   * @param enabled Whether to enable low-light enhancement:
   * - `true`: Enable low-light enhancement.
   * - `false`: (Default) Disable low-light enhancement.
   * @param options The low-light enhancement options. See `LowlightEnhanceOptions`.
   * @param type The type of the media source to which the filter effect is applied. See
   * `MEDIA_SOURCE_TYPE`.Attention: In this method, this parameter supports only the following two
   * settings:
   * - Use the default value `PRIMARY_CAMERA_SOURCE` if you use camera to capture local video.
   * - Set this parameter to `CUSTOM_VIDEO_SOURCE` if you use custom video source.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setLowlightEnhanceOptions(bool enabled, const LowlightEnhanceOptions& options, agora::media::MEDIA_SOURCE_TYPE type = agora::media::PRIMARY_CAMERA_SOURCE) = 0;
  /**
   * @brief Sets video noise reduction.
   *
   * @since v4.0.0
   *
   * @details
   * You can call this method to enable the video noise reduction feature and set the options of the
   * video noise reduction effect.
   * Applicable scenarios: dark environments and low-end video capture devices can cause video images
   * to contain significant noise, which affects video quality. In real-time interactive scenarios,
   * video noise also consumes bitstream resources and reduces encoding efficiency during encoding.
   * Call timing: Call this method after calling `enableVideo`.
   *
   * @note
   * - This method relies on the image enhancement dynamic library
   * `libagora_clear_vision_extension.dll`. If the dynamic library is deleted, the function cannot be
   * enabled normally.
   * - Video noise reduction has certain requirements for equipment performance. If your device
   * overheats after you enable video noise reduction, Agora recommends modifying the video noise
   * reduction options to a less performance-consuming level or disabling video noise reduction
   * entirely.
   * If the noise reduction implemented by this method does not meet your needs, Agora recommends that
   * you call the `setBeautyEffectOptions` method to enable the beauty and skin smoothing function to
   * achieve better video noise reduction effects. The recommended `BeautyOptions` settings for
   * intense noise reduction effect are as follows:
   * - `lighteningContrastLevel` LIGHTENING_CONTRAST_NORMAL
   * - `lighteningLevel`: 0.0
   * - `smoothnessLevel`: 0.5
   * - `rednessLevel`: 0.0
   * - `sharpnessLevel`: 0.1
   *
   * @param enabled Whether to enable video noise reduction:
   * - `true`: Enable video noise reduction.
   * - `false`: (Default) Disable video noise reduction.
   * @param options The video noise reduction options. See `VideoDenoiserOptions`.
   * @param type The type of the media source to which the filter effect is applied. See
   * `MEDIA_SOURCE_TYPE`.Attention: In this method, this parameter supports only the following two
   * settings:
   * - Use the default value `PRIMARY_CAMERA_SOURCE` if you use camera to capture local video.
   * - Set this parameter to `CUSTOM_VIDEO_SOURCE` if you use custom video source.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setVideoDenoiserOptions(bool enabled, const VideoDenoiserOptions& options, agora::media::MEDIA_SOURCE_TYPE type = agora::media::PRIMARY_CAMERA_SOURCE) = 0;
  /**
   * @brief Sets color enhancement.
   *
   * @since v4.0.0
   *
   * @details
   * The video images captured by the camera can have color distortion. The color enhancement feature
   * intelligently adjusts video characteristics such as saturation and contrast to enhance the video
   * color richness and color reproduction, making the video more vivid.
   * You can call this method to enable the color enhancement feature and set the options of the color
   * enhancement effect.
   *
   * @note
   * - Call this method after calling `enableVideo`.
   * - The color enhancement feature has certain performance requirements on devices. With color
   * enhancement turned on, Agora recommends that you change the color enhancement level to one that
   * consumes less performance or turn off color enhancement if your device is experiencing severe
   * heat problems.
   * - This method relies on the image enhancement dynamic library
   * `libagora_clear_vision_extension.dll`. If the dynamic library is deleted, the function cannot be
   * enabled normally.
   *
   * @param enabled Whether to enable color enhancement:
   * - `true` Enable color enhancement.
   * - `false`: (Default) Disable color enhancement.
   * @param options The color enhancement options. See `ColorEnhanceOptions`.
   * @param type The type of the media source to which the filter effect is applied. See
   * `MEDIA_SOURCE_TYPE`.Attention: In this method, this parameter supports only the following two
   * settings:
   * - Use the default value `PRIMARY_CAMERA_SOURCE` if you use camera to capture local video.
   * - Set this parameter to `CUSTOM_VIDEO_SOURCE` if you use custom video source.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setColorEnhanceOptions(bool enabled, const ColorEnhanceOptions& options, agora::media::MEDIA_SOURCE_TYPE type = agora::media::PRIMARY_CAMERA_SOURCE) = 0;

  /**
   * @brief Enables/Disables the virtual background.
   *
   * @since v3.7.200
   *
   * @details
   * The virtual background feature enables the local user to replace their original background with a
   * static image, dynamic video, blurred background, or portrait-background segmentation to achieve
   * picture-in-picture effect. Once the virtual background feature is enabled, all users in the
   * channel can see the custom background.
   * Call this method after calling `enableVideo` or `startPreview(VIDEO_SOURCE_TYPE sourceType)`.
   *
   * @note
   * - Using a video as a your virtual background will lead to continuous increase in memory usage,
   * which may cause issues such as app crashes. Therefore,it is recommended to reduce the resolution
   * and frame rate of the video when using it.
   * - This feature has high requirements on device performance. When calling this method, the SDK
   * automatically checks the capabilities of the current device. Agora recommends you use virtual
   * background on devices with the following processors:
   *   - Snapdragon 700 series 750G and later
   *   - Snapdragon 800 series 835 and later
   *   - Dimensity 700 series 720 and later
   *   - Kirin 800 series 810 and later
   *   - Kirin 900 series 980 and later
   *   - Devices with an i5 CPU and better
   *   - Devices with an A9 chip and better, as follows:
   *     - iPhone 6S and later
   *     - iPad Air 3rd generation and later
   *     - iPad 5th generation and later
   *     - iPad Pro 1st generation and later
   *     - iPad mini 5th generation and later
   * - Agora recommends that you use this feature in scenarios that meet the following conditions:
   *   - A high-definition camera device is used, and the environment is uniformly lit.
   *   - There are few objects in the captured video. Portraits are half-length and unobstructed.
   * Ensure that the background is a solid color that is different from the color of the user's
   * clothing.
   * - This method relies on the virtual background dynamic library
   * `libagora_segmentation_extension.dll`. If the dynamic library is deleted, the function cannot be
   * enabled normally.
   *
   * @param enabled Whether to enable virtual background:
   * - `true`: Enable virtual background.
   * - `false`: Disable virtual background.
   * @param backgroundSource The custom background. See `VirtualBackgroundSource`. To adapt the
   * resolution of the custom background image to that of the video captured by the SDK, the SDK
   * scales and crops the custom background image while ensuring that the content of the custom
   * background image is not distorted.
   * @param segproperty Processing properties for background images. See `SegmentationProperty`.
   * @param type The type of the media source to which the filter effect is applied. See
   * `MEDIA_SOURCE_TYPE`.Attention: In this method, this parameter supports only the following two
   * settings:
   * - Use the default value `PRIMARY_CAMERA_SOURCE` if you use camera to capture local video.
   * - Set this parameter to `CUSTOM_VIDEO_SOURCE` if you use custom video source.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   *   - -4: The device capabilities do not meet the requirements for the virtual background feature.
   * Agora recommends you try it on devices with higher performance.
   */
  virtual int enableVirtualBackground(bool enabled, VirtualBackgroundSource backgroundSource, SegmentationProperty segproperty, agora::media::MEDIA_SOURCE_TYPE type = agora::media::PRIMARY_CAMERA_SOURCE) = 0;

  /**
   * @brief Initializes the video view of a remote user.
   *
   * @details
   * This method initializes the video view of a remote stream on the local device. It affects only
   * the video view that the local user sees. Call this method to bind the remote video stream to a
   * video view and to set the rendering and mirror modes of the video view.
   * You need to specify the ID of the remote user in this method. If the remote user ID is unknown to
   * the application, set it after the app receives the `onUserJoined` callback.
   * To unbind the remote user from the view, set the `view` parameter to NULL.
   * Once the remote user leaves the channel, the SDK unbinds the remote user.
   * In the scenarios of custom layout for mixed videos on the mobile end, you can call this method
   * and set a separate `view` for rendering each sub-video stream of the mixed video stream.
   *
   * @note
   * - To update the rendering or mirror mode of the remote video view during a call, use the
   * `setRemoteRenderMode` method.
   * - When using the recording service, the app does not need to bind a view, as it does not send a
   * video stream. If your app does not recognize the recording service, bind the remote user to the
   * view when the SDK triggers the `onFirstRemoteVideoDecoded` callback.
   *
   * @param canvas The remote video view and settings. See `VideoCanvas`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setupRemoteVideo(const VideoCanvas& canvas) = 0;

  /**
   * @brief Initializes the local video view.
   *
   * @details
   * This method initializes the video view of a local stream on the local device. It only affects the
   * video seen by the local user and does not impact the publishing of the local video. Call this
   * method to bind the local video stream to a video view ( `view` ) and to set the rendering and
   * mirror modes of the video view.
   * The binding remains valid after leaving the channel. To stop rendering or unbind the local video
   * from the view, set `view` as NULL.
   * Applicable scenarios: After initialization, call this method to set the local video and then join
   * the channel.
   * In real-time interactive scenarios, if you need to simultaneously view multiple preview frames in
   * the local video preview, and each frame is at a different observation position along the video
   * link, you can repeatedly call this method to set different `view` s and set different observation
   * positions for each `view. ` For example, by setting the video source to the camera and then
   * configuring two `view` s with `position` setting to POSITION_POST_CAPTURER_ORIGIN and
   * POSITION_POST_CAPTURER, you can simultaneously preview the raw, unprocessed video frame and the
   * video frame that has undergone preprocessing (image enhancement effects, virtual background,
   * watermark) in the local video preview.
   * Call timing: You can call this method either before or after joining a channel.
   *
   * @note To update only the rendering or mirror mode of the local video view during a call, call
   * `setLocalRenderMode(media::base::RENDER_MODE_TYPE renderMode, VIDEO_MIRROR_MODE_TYPE mirrorMode)`
   * instead.
   *
   * @param canvas The local video view and settings. See `VideoCanvas`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setupLocalVideo(const VideoCanvas& canvas) = 0;

  /**
   * @brief Sets video application scenarios.
   *
   * @since v4.2.0
   *
   * @details
   * After successfully calling this method, the SDK will automatically enable the best practice
   * strategies and adjust key performance metrics based on the specified scenario, to optimize the
   * video experience.
   *
   * @note Call this method before joining a channel.
   *
   * @param scenarioType The type of video application scenario. See
   * `VIDEO_APPLICATION_SCENARIO_TYPE`.`APPLICATION_SCENARIO_MEETING` (1) is suitable for meeting
   * scenarios. The SDK automatically enables the following strategies:
   * - In meeting scenarios where low-quality video streams are required to have a high bitrate, the
   * SDK automatically enables multiple technologies used to deal with network congestions, to enhance
   * the performance of the low-quality streams and to ensure the smooth reception by subscribers.
   * - The SDK monitors the number of subscribers to the high-quality video stream in real time and
   * dynamically adjusts its configuration based on the number of subscribers.
   *   - If nobody subscribers to the high-quality stream, the SDK automatically reduces its bitrate
   * and frame rate to save upstream bandwidth.
   *   - If someone subscribes to the high-quality stream, the SDK resets the high-quality stream to
   * the `VideoEncoderConfiguration` configuration used in the most recent calling of
   * `setVideoEncoderConfiguration`. If no configuration has been set by the user previously, the
   * following values are used:
   *     - Resolution: (Windows and macOS) 1280 × 720; (Android and iOS) 960 × 540
   *     - Frame rate: 15 fps
   *     - Bitrate: (Windows and macOS) 1600 Kbps; (Android and iOS) 1000 Kbps
   * - The SDK monitors the number of subscribers to the low-quality video stream in real time and
   * dynamically enables or disables it based on the number of subscribers.Note: If the user has
   * called `setDualStreamMode(SIMULCAST_STREAM_MODE mode, const SimulcastStreamConfig& streamConfig)`
   * to set that never send low-quality video stream (
   * `DISABLE_SIMULCAST_STREAM` ), the dynamic adjustment of the low-quality stream in meeting
   * scenarios will not take effect.
   *   - If nobody subscribes to the low-quality stream, the SDK automatically disables it to save
   * upstream bandwidth.
   *   - If someone subscribes to the low-quality stream, the SDK enables the low-quality stream and
   * resets it to the `SimulcastStreamConfig` configuration used in the most recent calling of
   * `setDualStreamMode(SIMULCAST_STREAM_MODE mode, const SimulcastStreamConfig& streamConfig)`. If no
   * configuration has been set by the user previously, the following
   * values are used:
   *     - Resolution: 480 × 272
   *     - Frame rate: 15 fps
   *     - Bitrate: 500 Kbps
   * `APPLICATION_SCENARIO_1V1` (2) This is applicable to the `one to one live` scenario. To meet the
   * requirements for low latency and high-quality video in this scenario, the SDK optimizes its
   * strategies, improving performance in terms of video quality, first frame rendering, latency on
   * mid-to-low-end devices, and smoothness under weak network conditions.Attention: This enumeration
   * value is only applicable to the broadcaster vs. broadcaster scenario.
   * `APPLICATION_SCENARIO_LIVESHOW` (3) This is applicable to the `show room` scenario. In this
   * scenario, fast video rendering and high image quality are crucial. The SDK implements several
   * performance optimizations, including automatically enabling accelerated audio and video frame
   * rendering to minimize first-frame latency (no need to call `enableInstantMediaRendering` ), and
   * B-frame encoding to achieve better image quality and bandwidth efficiency. The SDK also provides
   * enhanced video quality and smooth playback, even in poor network conditions or on lower-end
   * devices.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   *   - -1: A general error occurs (no specified reason).
   *   - -4: Video application scenarios are not supported. Possible reasons include that you use the
   * Voice SDK instead of the Video SDK.
   *   - -7: The `IRtcEngine` object has not been initialized. You need to initialize the `IRtcEngine`
   * object before calling this method.
   */
  virtual int setVideoScenario(VIDEO_APPLICATION_SCENARIO_TYPE scenarioType) = 0;

    /**
   * Sets the video qoe preference.
   *
   * @since v4.2.1
   *
   * You can call this method to set the expected QoE Preference.
   * The SDK will optimize the video experience for each preference you set.
   *
   *
   * @param qoePreference The qoe preference type. See #VIDEO_QOE_PREFERENCE_TYPE.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   * - ERR_FAILED (1): A general error occurs (no specified reason).
   * - ERR_NOT_SUPPORTED (4): Unable to set video application scenario.
   * - ERR_NOT_INITIALIZED (7): The SDK is not initialized.
   */
  virtual int setVideoQoEPreference(VIDEO_QOE_PREFERENCE_TYPE qoePreference) = 0;

  /**
   * @brief Enables the audio module.
   *
   * @details
   * The audio module is enabled by default After calling `disableAudio` to disable the audio module,
   * you can call this method to re-enable it.
   * Call timing: This method can be called either before or after joining the channel. It is still
   * valid after one leaves channel.
   *
   * @note
   * - Calling this method will reset the entire engine, resulting in a slow response time. You can
   * use the following methods to independently control a specific function of the audio module based
   * on your actual needs:
   *   - `enableLocalAudio`: Whether to enable the microphone to create the local audio stream.
   *   - `muteLocalAudioStream`: Whether to publish the local audio stream.
   *   - `muteRemoteAudioStream`: Whether to subscribe and play the remote audio stream.
   *   - `muteAllRemoteAudioStreams`: Whether to subscribe to and play all remote audio streams.
   * - A successful call of this method resets `enableLocalAudio`, `muteRemoteAudioStream`, and
   * `muteAllRemoteAudioStreams`. Proceed it with caution.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int enableAudio() = 0;

  /**
   * @brief Disables the audio module.
   *
   * @details
   * The audio module is enabled by default, and you can call this method to disable the audio module.
   * Call timing: This method can be called either before or after joining the channel. It is still
   * valid after one leaves channel.
   *
   * @note
   * This method resets the internal engine and takes some time to take effect. Agora recommends using
   * the following API methods to control the audio modules separately:
   * - `enableLocalAudio`: Whether to enable the microphone to create the local audio stream.
   * - `enableLoopbackRecording`: Whether to enable loopback audio capturing.
   * - `muteLocalAudioStream`: Whether to publish the local audio stream.
   * - `muteRemoteAudioStream`: Whether to subscribe and play the remote audio stream.
   * - `muteAllRemoteAudioStreams`: Whether to subscribe to and play all remote audio streams.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int disableAudio() = 0;

  /**
   * @brief Sets the audio profile and audio scenario.
   *
   * @deprecated This method is deprecated. You can use the `setAudioProfile(AUDIO_PROFILE_TYPE profile) = 0` 
   * method instead. To set the audio scenario, call the `initialize` method and pass value in the 
   * `audioScenario` member in the RtcEngineContext struct.
   *
   * @details
   * Applicable scenarios: This method is suitable for various audio scenarios. You can choose as
   * needed. For example, in scenarios with high audio quality requirements such as music teaching, it
   * is recommended to set `profile` to `AUDIO_PROFILE_MUSIC_HIGH_QUALITY`(4) and `scenario` to
   * `AUDIO_SCENARIO_GAME_STREAMING`(3).
   * Call timing: You can call this method either before or after joining a channel.
   *
   * @note Due to iOS system restrictions, some audio routes cannot be recognized in call volume mode.
   * Therefore, if you need to use an external sound card, it is recommended to set the audio scenario
   * to `AUDIO_SCENARIO_GAME_STREAMING`(3). In this scenario, the SDK will switch to media volume to
   * avoid this issue.
   *
   * @param profile The audio profile, including the sampling rate, bitrate, encoding mode, and the
   * number of channels. See `AUDIO_PROFILE_TYPE`.
   * @param scenario The audio scenarios. Under different audio scenarios, the device uses different
   * volume types. See `AUDIO_SCENARIO_TYPE`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setAudioProfile(AUDIO_PROFILE_TYPE profile, AUDIO_SCENARIO_TYPE scenario) __deprecated = 0;

  /**
   * @brief Sets audio profiles.
   *
   * @details
   * If you need to set the audio scenario, you can either call `setAudioScenario`, or `initialize`
   * and set the `audioScenario` in `RtcEngineContext`.
   * Applicable scenarios: This method is suitable for various audio scenarios. You can choose as
   * needed. For example, in scenarios with high audio quality requirements such as music teaching, it
   * is recommended to set `profile` to `AUDIO_PROFILE_MUSIC_HIGH_QUALITY`(4).
   * Call timing: You can call this method either before or after joining a channel.
   *
   * @param profile The audio profile, including the sampling rate, bitrate, encoding mode, and the
   * number of channels. See `AUDIO_PROFILE_TYPE`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setAudioProfile(AUDIO_PROFILE_TYPE profile) = 0;
  /**
   * @brief Sets audio scenarios.
   *
   * @details
   * Applicable scenarios: This method is suitable for various audio scenarios. You can choose as
   * needed. For example, in scenarios such as music teaching that require high sound quality, it is
   * recommended to set `scenario` to `AUDIO_SCENARIO_GAME_STREAMING`(3).
   * Call timing: You can call this method either before or after joining a channel.
   *
   * @note Due to iOS system restrictions, some audio routes cannot be recognized in call volume mode.
   * Therefore, if you need to use an external sound card, it is recommended to set the audio scenario
   * to `AUDIO_SCENARIO_GAME_STREAMING`(3). In this scenario, the SDK will switch to media volume to
   * avoid this issue.
   *
   * @param scenario The audio scenarios. Under different audio scenarios, the device uses different
   * volume types. See `AUDIO_SCENARIO_TYPE`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setAudioScenario(AUDIO_SCENARIO_TYPE scenario) = 0;
  /**
   * @brief Enables or disables the local audio capture.
   *
   * @details
   * The audio function is enabled by default when users joining a channel. This method disables or
   * re-enables the local audio function to stop or restart local audio capturing.
   * The difference between this method and `muteLocalAudioStream` are as follows:
   * - `enableLocalAudio`: Disables or re-enables the local audio capturing and processing. If you
   * disable or re-enable local audio capturing using the `enableLocalAudio` method, the local user
   * might hear a pause in the remote audio playback.
   * - `muteLocalAudioStream`: Sends or stops sending the local audio streams without affecting the
   * audio capture status.
   * Applicable scenarios: This method does not affect receiving the remote audio streams.
   * `enableLocalAudio` `(false)` is suitable for scenarios where the user wants to receive remote
   * audio streams without sending locally captured audio.
   * Call timing: You can call this method either before or after joining a channel. Calling it before
   * joining a channel only sets the device state, and it takes effect immediately after you join the
   * channel.
   * Related callbacks: Once the local audio function is disabled or re-enabled, the SDK triggers the
   * `onLocalAudioStateChanged` callback, which reports `LOCAL_AUDIO_STREAM_STATE_STOPPED` (0) or
   * `LOCAL_AUDIO_STREAM_STATE_RECORDING` (1).
   *
   * @param enabled
   * - `true`: (Default) Re-enable the local audio function, that is, to start the
   * local audio capturing device (for example, the microphone).
   * - `false`: Disable the local audio function, that is, to stop local audio capturing.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int enableLocalAudio(bool enabled) = 0;

  /**
   * @brief Stops or resumes publishing the local audio stream.
   *
   * @details
   * This method is used to control whether to publish the locally captured audio stream. If you call
   * this method to stop publishing locally captured audio streams, the audio capturing device will
   * still work and won't be affected.
   * Call timing: This method can be called either before or after joining the channel.
   * Related callbacks: After successfully calling this method, the local end triggers callback
   * `onAudioPublishStateChanged`; the remote end triggers `onUserMuteAudio` and
   * `onRemoteAudioStateChanged` callbacks.
   *
   * @param mute Whether to stop publishing the local audio stream:
   * - `true`: Stops publishing the local audio stream.
   * - `false`: (Default) Resumes publishing the local audio stream.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int muteLocalAudioStream(bool mute) = 0;

  /**
   Stops or resumes receiving all remote audio stream.

   This method works for all remote users that join or will join a channel
   using the `joinChannel` method. It is
   equivalent to the `autoSubscribeAudio` member in the ChannelMediaOptions
   class.
   - Ensure that you call this method after joining a channel.
   - If you call muteAllRemoteAudioStreams(true) after joining a channel, the
   local use stops receiving any audio stream from any user in the channel,
   including any user who joins the channel after you call this method.
   - If you call muteAllRemoteAudioStreams(true) after leaving a channel,
   the local user does not receive any audio stream the next time the user
   joins a channel.

   After you successfully call muteAllRemoteAudioStreams(true), you can take
   the following actions:
   - To resume receiving all remote audio streams, call
   muteAllRemoteAudioStreams(false).
   - To resume receiving the audio stream of a specified user, call
   muteRemoteAudioStream(uid, false), where uid is the ID of the user whose
   audio stream you want to resume receiving.

   @note
   - The result of calling this method is affected by calling
   \ref IRtcEngine::enableAudio "enableAudio" and
   \ref IRtcEngine::disableAudio "disableAudio". Settings in
   muteAllRemoteAudioStreams stop taking effect if either of the following occurs:
     - Calling `enableAudio` after muteAllRemoteAudioStreams(true).
     - Calling `disableAudio` after muteAllRemoteAudioStreams(false).
   - This method only works for the channel created by calling
   `joinChannel`. To set whether to receive remote
   audio streams for a specific channel, Agora recommends using
   `autoSubscribeAudio` in the ChannelMediaOptions class.
   @param mute Whether to stop receiving remote audio streams:
   - true: Stop receiving any remote audio stream.
   - false: (Default) Resume receiving all remote audio streams.
   @return
   - 0: Success.
   - < 0: Failure.
   */
  /**
   * @brief Stops or resumes subscribing to the audio streams of all remote users.
   *
   * @details
   * After successfully calling this method, the local user stops or resumes subscribing to the audio
   * streams of all remote users, including all subsequent users.
   * Call timing: Call this method after joining a channel.
   *
   * @note
   * If you call this method and then call `enableAudio` or `disableAudio`, the latest call will
   * prevail.
   * By default, the SDK subscribes to the audio streams of all remote users when joining a channel.
   * To modify this behavior, you can set `autoSubscribeAudio` to `false` when calling
   * `joinChannel(const char* token, const char* channelId, uid_t uid, const ChannelMediaOptions&
   * options)`
   * to join the channel, which will cancel the subscription to the audio streams of all users
   * upon joining the channel.
   *
   * @param mute Whether to stop subscribing to the audio streams of all remote users:
   * - `true`: Stops subscribing to the audio streams of all remote users.
   * - `false`: (Default) Subscribes to the audio streams of all remote users by default.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int muteAllRemoteAudioStreams(bool mute) = 0;

  /**
   * @brief Stops or resumes subscribing to the audio stream of a specified user.
   *
   * @details
   * Call timing: Call this method after joining a channel.
   * Related callbacks: After a successful method call, the SDK triggers the
   * `onAudioSubscribeStateChanged` callback.
   *
   * @param uid The user ID of the specified user.
   * @param mute Whether to subscribe to the specified remote user's audio stream.
   * - `true`: Stop subscribing to the audio stream of the specified user.
   * - `false`: (Default) Subscribe to the audio stream of the specified user.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int muteRemoteAudioStream(uid_t uid, bool mute) = 0;

  /**
   * @brief Stops or resumes publishing the local video stream.
   *
   * @details
   * This method is used to control whether to publish the locally captured video stream. If you call
   * this method to stop publishing locally captured video streams, the video capturing device will
   * still work and won't be affected.
   * Compared to `enableLocalVideo` (`false`), which can also cancel the publishing of local video
   * stream by turning off the local video stream capture, this method responds faster.
   * Call timing: This method can be called either before or after joining the channel.
   * Related callbacks: After successfully calling this method, the local end triggers callback
   * `onVideoPublishStateChanged`; the remote end triggers `onUserMuteVideo` and
   * `onRemoteVideoStateChanged` callbacks.
   *
   * @param mute Whether to stop publishing the local video stream.
   * - `true`: Stop publishing the local video stream.
   * - `false`: (Default) Publish the local video stream.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int muteLocalVideoStream(bool mute) = 0;

  /**
   * @brief Enables/Disables the local video capture.
   *
   * @details
   * This method disables or re-enables the local video capture, and does not affect receiving the
   * remote video stream.
   * After calling `enableVideo`, the local video capture is enabled by default.
   * If you call `enableLocalVideo` (`false`) to disable local video capture within the channel, it
   * also simultaneously stops publishing the video stream within the channel. If you want to restart
   * video catpure, you can call `enableLocalVideo` (`true`) and then call `updateChannelMediaOptions`
   * to set the `options` parameter to publish the locally captured video stream in the channel.
   * After the local video capturer is successfully disabled or re-enabled, the SDK triggers the
   * `onRemoteVideoStateChanged` callback on the remote client.
   *
   * @note
   * - You can call this method either before or after joining a channel. However, if you call it
   * before joining, the settings will only take effect once you have joined the channel.
   * - This method enables the internal engine and is valid after leaving the channel.
   *
   * @param enabled Whether to enable the local video capture.
   * - `true`: (Default) Enable the local video capture.
   * - `false`: Disable the local video capture. Once the local video is disabled, the remote users
   * cannot receive the video stream of the local user, while the local user can still receive the
   * video streams of remote users. When set to `false`, this method does not require a local camera.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int enableLocalVideo(bool enabled) = 0;

  /** Stops or resumes receiving all remote video streams.

   This method works for all remote users that join or will join a channel
   using the `joinChannel` method. It is
   equivalent to the `autoSubscribeVideo` member in the ChannelMediaOptions
   class.
   - Ensure that you call this method after joining a channel.
   - If you call muteAllRemoteVideoStreams(true) after joining a channel, the
   local use stops receiving any video stream from any user in the channel,
   including any user who joins the channel after you call this method.
   - If you call muteAllRemoteVideoStreams(true) after leaving a channel,
   the local user does not receive any video stream the next time the user
   joins a channel.

   After you successfully call muteAllRemoteVideoStreams(true), you can take
   the following actions:
   - To resume receiving all remote video streams, call
   muteAllRemoteVideoStreams(false).
   - To resume receiving the video stream of a specified user, call
   muteRemoteVideoStream(uid, false), where uid is the ID of the user whose
   video stream you want to resume receiving.

   @note
   - The result of calling this method is affected by calling
   \ref IRtcEngine::enableVideo "enableVideo" and
   \ref IRtcEngine::disableVideo "disableVideo". Settings in
   muteAllRemoteVideoStreams stop taking effect if either of the following occurs:
     - Calling `enableVideo` after muteAllRemoteVideoStreams(true).
     - Calling `disableVideo` after muteAllRemoteVideoStreams(false).
   - This method only works for the channel created by calling `joinChannel`.
   To set whether to receive remote audio streams for a specific channel, Agora
   recommends using `autoSubscribeVideo` in the ChannelMediaOptions class.
   @param mute Whether to stop receiving remote video streams:
   - true: Stop receiving any remote video stream.
   - false: (Default) Resume receiving all remote video streams.
   @return
   - 0: Success.
   - < 0: Failure.
   */
  /**
   * @brief Stops or resumes subscribing to the video streams of all remote users.
   *
   * @details
   * After successfully calling this method, the local user stops or resumes subscribing to the video
   * streams of all remote users, including all subsequent users.
   * Call timing: Call this method after joining a channel.
   *
   * @note
   * If you call this method and then call `enableVideo` or `disableVideo`, the latest call will
   * prevail.
   * By default, the SDK subscribes to the video streams of all remote users when joining a channel.
   * To modify this behavior, you can set `autoSubscribeVideo` to`false` when calling
   * `joinChannel(const char* token, const char* channelId, uid_t uid, const ChannelMediaOptions&
   * options)`
   * to join the channel, which will cancel the subscription to the video streams of all users
   * upon joining the channel.
   *
   * @param mute Whether to stop subscribing to the video streams of all remote users.
   * - `true`: Stop subscribing to the video streams of all remote users.
   * - `false`: (Default) Subscribe to the video streams of all remote users by default.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int muteAllRemoteVideoStreams(bool mute) = 0;

  /**
   * @brief Sets the default video stream type to subscribe to.
   *
   * @details
   * Depending on the default behavior of the sender and the specific settings when calling
   * `setDualStreamMode(SIMULCAST_STREAM_MODE mode, const SimulcastStreamConfig& streamConfig)`, the
   * scenarios for the receiver calling this method are as follows:
   * - The SDK enables low-quality video stream adaptive mode ( `AUTO_SIMULCAST_STREAM` ) on the
   * sender side by default, meaning only the high-quality video stream is transmitted. Only the
   * receiver with the role of the **host**can call this method to initiate a low-quality video stream
   * request. Once the sender receives the request, it starts automatically sending the low-quality
   * video stream. At this point, all users in the channel can call this method to switch to
   * low-quality video stream subscription mode.
   * - If the sender calls `setDualStreamMode(SIMULCAST_STREAM_MODE mode, const SimulcastStreamConfig&
   * streamConfig)` and sets `mode` to `DISABLE_SIMULCAST_STREAM`
   * (never send low-quality video stream), then calling this method will have no effect.
   * - If the sender calls `setDualStreamMode(SIMULCAST_STREAM_MODE mode, const SimulcastStreamConfig&
   * streamConfig)` and sets `mode` to `ENABLE_SIMULCAST_STREAM`
   * (always send low-quality video stream), both the host and audience receivers can call this method
   * to switch to low-quality video stream subscription mode.
   * The SDK will dynamically adjust the size of the corresponding video stream based on the size of
   * the video window to save bandwidth and computing resources. The default aspect ratio of the
   * low-quality video stream is the same as that of the high-quality video stream. According to the
   * current aspect ratio of the high-quality video stream, the system will automatically allocate the
   * resolution, frame rate, and bitrate of the low-quality video stream.
   * Call timing: Call this method before joining a channel. The SDK does not support changing the
   * default subscribed video stream type after joining a channel.
   *
   * @note If you call both this method and `setRemoteVideoStreamType`, the setting of
   * `setRemoteVideoStreamType` takes effect.
   *
   * @param streamType The default video-stream type. See `VIDEO_STREAM_TYPE`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setRemoteDefaultVideoStreamType(VIDEO_STREAM_TYPE streamType) = 0;

  /**
   * @brief Stops or resumes subscribing to the video stream of a specified user.
   *
   * @details
   * Call timing: Call this method after joining a channel.
   * Related callbacks: After a successful method call, the SDK triggers the
   * `onVideoSubscribeStateChanged` callback.
   *
   * @param uid The user ID of the specified user.
   * @param mute Whether to subscribe to the specified remote user's video stream.
   * - `true`: Stop subscribing to the video streams of the specified user.
   * - `false`: (Default) Subscribe to the video stream of the specified user.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int muteRemoteVideoStream(uid_t uid, bool mute) = 0;

  /**
   * @brief Sets the video stream type to subscribe to.
   *
   * @details
   * Depending on the default behavior of the sender and the specific settings when calling
   * `setDualStreamMode(SIMULCAST_STREAM_MODE mode, const SimulcastStreamConfig& streamConfig)`, the
   * scenarios for the receiver calling this method are as follows:
   * - The SDK enables low-quality video stream adaptive mode ( `AUTO_SIMULCAST_STREAM` ) on the
   * sender side by default, meaning only the high-quality video stream is transmitted. Only the
   * receiver with the role of the **host**can call this method to initiate a low-quality video stream
   * request. Once the sender receives the request, it starts automatically sending the low-quality
   * video stream. At this point, all users in the channel can call this method to switch to
   * low-quality video stream subscription mode.
   * - If the sender calls `setDualStreamMode(SIMULCAST_STREAM_MODE mode, const SimulcastStreamConfig&
   * streamConfig)` and sets `mode` to `DISABLE_SIMULCAST_STREAM`
   * (never send low-quality video stream), then calling this method will have no effect.
   * - If the sender calls `setDualStreamMode(SIMULCAST_STREAM_MODE mode, const SimulcastStreamConfig&
   * streamConfig)` and sets `mode` to `ENABLE_SIMULCAST_STREAM`
   * (always send low-quality video stream), both the host and audience receivers can call this method
   * to switch to low-quality video stream subscription mode.
   * The SDK will dynamically adjust the size of the corresponding video stream based on the size of
   * the video window to save bandwidth and computing resources. The default aspect ratio of the
   * low-quality video stream is the same as that of the high-quality video stream. According to the
   * current aspect ratio of the high-quality video stream, the system will automatically allocate the
   * resolution, frame rate, and bitrate of the low-quality video stream.
   *
   * @note
   * - You can call this method either before or after joining a channel.
   * - If you call both this method and `setRemoteDefaultVideoStreamType`, the setting of this method
   * takes effect.
   *
   * @param uid The user ID.
   * @param streamType The video stream type, see `VIDEO_STREAM_TYPE`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setRemoteVideoStreamType(uid_t uid, VIDEO_STREAM_TYPE streamType) = 0;

  /**
   * @brief Options for subscribing to remote video streams.
   *
   * @details
   * When a remote user has enabled dual-stream mode, you can call this method to choose the option
   * for subscribing to the video streams sent by the remote user. The default subscription behavior
   * of the SDK for remote video streams depends on the type of registered video observer:
   * - If the `IVideoFrameObserver` observer is registered, the default is to subscribe to both raw
   * data and encoded data.
   * - If the `IVideoEncodedFrameObserver` observer is registered, the default is to subscribe only to
   * the encoded data.
   * - If both types of observers are registered, the default behavior follows the last registered
   * video observer. For example, if the last registered observer is the `IVideoFrameObserver`
   * observer, the default is to subscribe to both raw data and encoded data.
   * If you want to modify the default behavior, or set different subscription options for different
   * `uids`, you can call this method to set it.
   *
   * @param uid The user ID of the remote user.
   * @param options The video subscription options. See `VideoSubscriptionOptions`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setRemoteVideoSubscriptionOptions(uid_t uid, const VideoSubscriptionOptions &options) = 0;

  /**
   * @brief Sets the blocklist of subscriptions for audio streams.
   *
   * @details
   * You can call this method to specify the audio streams of a user that you do not want to subscribe
   * to.
   *
   * @note
   * - You can call this method either before or after joining a channel.
   * - The blocklist is not affected by the setting in `muteRemoteAudioStream`,
   * `muteAllRemoteAudioStreams`, and `autoSubscribeAudio` in `ChannelMediaOptions`.
   * - Once the blocklist of subscriptions is set, it is effective even if you leave the current
   * channel and rejoin the channel.
   * - If a user is added in the allowlist and blocklist at the same time, only the blocklist takes
   * effect.
   *
   * @param uidList The user ID list of users that you do not want to subscribe to.
   * If you want to specify the audio streams of a user that you do not want to subscribe to, add the
   * user ID in this list. If you want to remove a user from the blocklist, you need to call the
   * `setSubscribeAudioBlocklist` method to update the user ID list; this means you only add the `uid`
   * of users that you do not want to subscribe to in the new user ID list.
   * @param uidNumber The number of users in the user ID list.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setSubscribeAudioBlocklist(uid_t* uidList, int uidNumber) = 0;

  /**
   * @brief Sets the allowlist of subscriptions for audio streams.
   *
   * @details
   * You can call this method to specify the audio streams of a user that you want to subscribe to.
   *
   * @note
   * - You can call this method either before or after joining a channel.
   * - The allowlist is not affected by the setting in `muteRemoteAudioStream`,
   * `muteAllRemoteAudioStreams` and `autoSubscribeAudio` in `ChannelMediaOptions`.
   * - Once the allowlist of subscriptions is set, it is effective even if you leave the current
   * channel and rejoin the channel.
   * - If a user is added in the allowlist and blocklist at the same time, only the blocklist takes
   * effect.
   *
   * @param uidList The user ID list of users that you want to subscribe to.
   * If you want to specify the audio streams of a user for subscription, add the user ID in this
   * list. If you want to remove a user from the allowlist, you need to call the
   * `setSubscribeAudioAllowlist` method to update the user ID list; this means you only add the `uid`
   * of users that you want to subscribe to in the new user ID list.
   * @param uidNumber The number of users in the user ID list.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setSubscribeAudioAllowlist(uid_t* uidList, int uidNumber) = 0;

  /**
   * @brief Sets the blocklist of subscriptions for video streams.
   *
   * @details
   * You can call this method to specify the video streams of a user that you do not want to subscribe
   * to.
   *
   * @note
   * - You can call this method either before or after joining a channel.
   * - The blocklist is not affected by the setting in `muteRemoteVideoStream`,
   * `muteAllRemoteVideoStreams` and `autoSubscribeAudio` in `ChannelMediaOptions`.
   * - Once the blocklist of subscriptions is set, it is effective even if you leave the current
   * channel and rejoin the channel.
   * - If a user is added in the allowlist and blocklist at the same time, only the blocklist takes
   * effect.
   *
   * @param uidList The user ID list of users that you do not want to subscribe to.
   * If you want to specify the video streams of a user that you do not want to subscribe to, add the
   * user ID of that user in this list. If you want to remove a user from the blocklist, you need to
   * call the `setSubscribeVideoBlocklist` method to update the user ID list; this means you only add
   * the `uid` of users that you do not want to subscribe to in the new user ID list.
   * @param uidNumber The number of users in the user ID list.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setSubscribeVideoBlocklist(uid_t* uidList, int uidNumber) = 0;

  /**
   * @brief Sets the allowlist of subscriptions for video streams.
   *
   * @details
   * You can call this method to specify the video streams of a user that you want to subscribe to.
   *
   * @note
   * - You can call this method either before or after joining a channel.
   * - The allowlist is not affected by the setting in `muteRemoteVideoStream`,
   * `muteAllRemoteVideoStreams` and `autoSubscribeAudio` in `ChannelMediaOptions`.
   * - Once the allowlist of subscriptions is set, it is effective even if you leave the current
   * channel and rejoin the channel.
   * - If a user is added in the allowlist and blocklist at the same time, only the blocklist takes
   * effect.
   *
   * @param uidList The user ID list of users that you want to subscribe to.
   * If you want to specify the video streams of a user for subscription, add the user ID of that user
   * in this list. If you want to remove a user from the allowlist, you need to call the
   * `setSubscribeVideoAllowlist` method to update the user ID list; this means you only add the `uid`
   * of users that you want to subscribe to in the new user ID list.
   * @param uidNumber The number of users in the user ID list.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setSubscribeVideoAllowlist(uid_t* uidList, int uidNumber) = 0;

  /**
   * @brief Enables the reporting of users' volume indication.
   *
   * @details
   * This method enables the SDK to regularly report the volume information to the app of the local
   * user who sends a stream and remote users (three users at most) whose instantaneous volumes are
   * the highest.
   * Call timing: This method can be called either before or after joining the channel.
   * Related callbacks: The SDK triggers the `onAudioVolumeIndication` callback according to the
   * interval you set if this method is successfully called and there are users publishing streams in
   * the channel.
   *
   * @param interval Sets the time interval between two consecutive volume indications:
   * - ≤ 0: Disables the volume indication.
   * - > 0: Time interval (ms) between two consecutive volume indications. Ensure this parameter is
   * set to a value greater than 10, otherwise you will not receive the `onAudioVolumeIndication`
   * callback. Agora recommends that this value is set as greater than 100.
   * @param smooth The smoothing factor that sets the sensitivity of the audio volume indicator. The
   * value ranges between 0 and 10. The recommended value is 3. The greater the value, the more
   * sensitive the indicator.
   * @param reportVad - `true`: Enables the voice activity detection of the local user. Once it is
   * enabled, the `vad` parameter of the `onAudioVolumeIndication` callback reports the voice activity
   * status of the local user.
   * - `false`: (Default) Disables the voice activity detection of the local user. Once it is
   * disabled, the `vad` parameter of the `onAudioVolumeIndication` callback does not report the voice
   * activity status of the local user, except for the scenario where the engine automatically detects
   * the voice activity of the local user.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int enableAudioVolumeIndication(int interval, int smooth, bool reportVad) = 0;

  /**
   * @brief Starts client-side audio recording with recording configuration.
   *
   * @details
   * The SDK supports recording on the client during a call. After calling this method, you can record
   * the audio of users in the channel and obtain a recording file. The recording file supports the
   * following formats only:
   * - WAV: Higher audio fidelity, larger file size. For example, with a sample rate of 32000 Hz, a
   * 10-minute recording is about 73 MB.
   * - AAC: Lower audio fidelity, smaller file size. For example, with a sample rate of 32000 Hz and
   * recording quality set to AUDIO_RECORDING_QUALITY_MEDIUM, a 10-minute recording is about 2 MB.
   * Recording automatically stops when the user leaves the channel.
   * Call timing: This method must be called after joining a channel.
   *
   * @param config Recording configuration. See `AudioRecordingConfiguration`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure. See `Error Codes` for details and resolution suggestions.
   */
  virtual int startAudioRecording(const char* filePath,
                                  AUDIO_RECORDING_QUALITY_TYPE quality) = 0;
  /**
   * @brief Starts client-side audio recording and sets the recording sample rate.
   *
   * @details
   * The SDK supports recording on the client during a call. After calling this method, you can record
   * the audio of all users in the channel and obtain a recording file that includes all voices. The
   * recording file supports the following formats only:
   * - .wav: Large file size, higher audio fidelity.
   * - .aac: Smaller file size, lower audio fidelity.
   *
   * @note
   * - Make sure the path you specify in this method exists and is writable.
   * - This method must be called after `joinChannel(const char* token, const char* channelId, uid_t
   * uid, const ChannelMediaOptions& options)`. If `leaveChannel(const LeaveChannelOptions& options)`
   * is called while recording is in progress, the recording will automatically stop.
   * - To ensure recording quality, when `sampleRate` is set to 44.1 kHz or 48 kHz, it is recommended
   * to set `quality` to `AUDIO_RECORDING_QUALITY_MEDIUM`
   * or `AUDIO_RECORDING_QUALITY_HIGH`.
   *
   * @param filePath The absolute path where the recording file will be saved locally, including the
   * file name and extension. For example: `C:\music\audio.aac`.
   *                     Note:
   * Make sure the specified path exists and is writable.
   * @param sampleRate Recording sample rate (Hz). You can set it to one of the following values:
   * - 16000
   * - 32000 (default)
   * - 44100
   * - 48000
   * @param quality Recording quality. See `AUDIO_RECORDING_QUALITY_TYPE`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure. See `Error Codes` for details and resolution suggestions.
   */
  virtual int startAudioRecording(const char* filePath,
                                  int sampleRate,
                                  AUDIO_RECORDING_QUALITY_TYPE quality) = 0;

  /** Starts an audio recording.

  The SDK allows recording during a call, which supports either one of the
  following two formats:

  - .wav: Large file size with high sound fidelity
  - .aac: Small file size with low sound fidelity

  Ensure that the directory to save the recording file exists and is writable.
  This method is usually called after the joinChannel() method.
  The recording automatically stops when the leaveChannel() method is
  called.

  @param config Audio recording config.
  @return
  - 0: Success.
  - < 0: Failure.
  */
  virtual int startAudioRecording(const AudioRecordingConfiguration& config) = 0;

  /**
   * @brief Registers an encoded audio observer.
   *
   * @note
   * - Call this method after joining a channel.
   * - You can call this method or `startAudioRecording [3/3]` to set the recording type and quality
   * of audio files, but Agora does not recommend using this method and `startAudioRecording [3/3]` at
   * the same time. Only the method called later will take effect.
   *
   * @param config Observer settings for the encoded audio. See `AudioEncodedFrameObserverConfig`.
   * @param observer The encoded audio observer. See `IAudioEncodedFrameObserver`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int registerAudioEncodedFrameObserver(const AudioEncodedFrameObserverConfig& config,  IAudioEncodedFrameObserver *observer) = 0;

  /**
   * @brief Stops client-side audio recording.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure. See `Error Codes` for details and resolution suggestions.
   */
  virtual int stopAudioRecording() = 0;

  /**
   * @brief Creates a media player object.
   *
   * @details
   * Before calling any APIs in the `IMediaPlayer` class, you need to call this method to create an
   * instance of the media player. If you need to create multiple instances, you can call this method
   * multiple times.
   * Call timing: You can call this method either before or after joining a channel.
   *
   * @return
   * - An `IMediaPlayer` object, if the method call succeeds.
   * - An empty pointer, if the method call fails.
   */
  virtual agora_refptr<IMediaPlayer> createMediaPlayer() = 0;

  /**
   * @brief Destroys the media player instance.
   *
   * @param media_player `IMediaPlayer` object.
   *
   * @return
   * - ≥ 0: Success. Returns the ID of media player instance.
   * - < 0: Failure.
   */
  virtual int destroyMediaPlayer(agora_refptr<IMediaPlayer> media_player) = 0;

  /**
   * @brief Creates an audio and video recording object.
   *
   * @details
   * Before starting to record audio and video streams, you need to call this method to create a
   * recording object. The SDK supports recording multiple audio and video streams from local or
   * remote users. You can call this method multiple times to create recording objects, and use the
   * `info`
   * parameter to specify the channel name and the user ID of the stream to be recorded.
   * After successful creation, you need to call `setMediaRecorderObserver` to register an observer
   * for the recording object to listen for related callbacks, and then call `startRecording` to begin
   * recording.
   *
   * @param info Information about the audio and video stream to be recorded. See
   * `RecorderStreamInfo`.
   *
   * @return
   * - If the method call succeeds: Returns an `IMediaRecorder` object.
   * - If the method call fails: Returns a null pointer.
   */
  virtual agora_refptr<IMediaRecorder> createMediaRecorder(const RecorderStreamInfo& info) = 0;

  /**
   * @brief Destroys an audio and video recording object.
   *
   * @details
   * When you no longer need to record audio and video streams, you can call this method to destroy
   * the corresponding recording object. If recording is in progress, call `stopRecording` first, then
   * call this method to destroy the recording object.
   *
   * @param mediaRecorder The `IMediaRecorder` object to be destroyed.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure. See `Error Codes` for details and resolution suggestions.
   */
  virtual int destroyMediaRecorder(agora_refptr<IMediaRecorder> mediaRecorder) = 0;

  /** Starts playing and mixing the music file.

  This method mixes the specified local audio file with the audio stream from
  the microphone. You can choose whether the other user can hear the local
  audio playback and specify the number of playback loops. This method also
  supports online music playback.

  After calling this method successfully, the SDK triggers the

  \ref IRtcEngineEventHandler::onAudioMixingStateChanged "onAudioMixingStateChanged" (PLAY)
  callback on the local client.
  When the audio mixing file playback finishes after calling this method, the
  SDK triggers the
  \ref IRtcEngineEventHandler::onAudioMixingStateChanged "onAudioMixingStateChanged" (STOPPED)
  callback on the local client.

  @note
  - Call this method after joining a channel, otherwise issues may occur.
  - If the local audio mixing file does not exist, or if the SDK does not
  support the file format or cannot access the music file URL, the SDK returns
  #WARN_AUDIO_MIXING_OPEN_ERROR (701).
  - If you want to play an online music file, ensure that the time interval
  between calling this method is more than 100 ms, or the
  `AUDIO_MIXING_ERROR_TOO_FREQUENT_CALL(702)` error code occurs.

  @param filePath Pointer to the absolute path (including the suffixes of the
  filename) of the local or online audio file to mix, for example, c:/music/au
  dio.mp4. Supported audio formats: 3GP, ASF, ADTS, AVI, MP3, MP4, MPEG-4,
  SAMI, and WAVE.
  @param loopback Sets which user can hear the audio mixing:
  - true: Only the local user can hear the audio mixing.
  - false: Both users can hear the audio mixing.

  @param cycle Sets the number of playback loops:
  - Positive integer: Number of playback loops.
  - `-1`: Infinite playback loops.

  @return
  - 0: Success.
  - < 0: Failure.
  */
  /**
   * @brief Starts playing the music file.
   *
   * @details
   * For the audio file formats supported by this method, see `What formats of audio files does the
   * Agora RTC SDK support`. If the local music file does not exist, the SDK does not support the file
   * format, or the the SDK cannot access the music file URL, the SDK reports
   * AUDIO_MIXING_REASON_CAN_NOT_OPEN.
   * Call timing: You can call this method either before or after joining a channel.
   * Related callbacks: A successful method call triggers the `onAudioMixingStateChanged`
   * (`AUDIO_MIXING_STATE_PLAYING`) callback. When the audio mixing file playback finishes, the SDK
   * triggers the `onAudioMixingStateChanged` (`AUDIO_MIXING_STATE_STOPPED`) callback on the local
   * client.
   *
   * @note
   * - If you call this method to play short sound effect files, you may encounter playback failure.
   * Agora recommends using `playEffect` instead to play such files.
   * - If you need to call this method multiple times, ensure that the time interval between calling
   * this method is more than 500 ms.
   * - On Android, there are following considerations:
   *   - To use this method, ensure that the Android device is v4.2 or later, and the API version is
   * v16 or later.
   *   - If you need to play an online music file, Agora does not recommend using the redirected URL
   * address. Some Android devices may fail to open a redirected URL address.
   *   - If you call this method on an emulator, ensure that the music file is in the `/sdcard/`
   * directory and the format is MP3.
   *
   * @param filePath The file path. The SDK supports URLs and absolute path of local files. The
   * absolute path needs to be accurate to the file name and extension. Supported audio formats
   * include MP3, AAC, M4A, MP4, WAV, and 3GP. See `Supported Audio Formats`.
   * Attention: If you have preloaded an audio effect into memory by calling `preloadEffect`, ensure
   * that the value of this parameter is the same as that of `filePath` in `preloadEffect`.
   * @param loopback Whether to only play music files on the local client:
   * - `true`: Only play music files on the local client so that only the local user can hear the
   * music.
   * - `false`: Publish music files to remote clients so that both the local user and remote users can
   * hear the music.
   * @param cycle The number of times the music file plays.
   * - >0: The number of times for playback. For example, 1 represents playing 1 time.
   * - -1: Play the audio file in an infinite loop.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   *   - -1: A general error occurs (no specified reason).
   *   - -2: The parameter is invalid.
   *   - -3: The SDK is not ready.
   *     - The audio module is disabled.
   *     - The program is not complete.
   *     - The initialization of `IRtcEngine` fails. Reinitialize the `IRtcEngine`.
   */
  virtual int startAudioMixing(const char* filePath, bool loopback, int cycle) = 0;

  /** Starts playing and mixing the music file.

  This method mixes the specified local audio file with the audio stream from
  the microphone. You can choose whether the other user can hear the local
  audio playback and specify the number of playback loops. This method also
  supports online music playback.

  After calling this method successfully, the SDK triggers the

  \ref IRtcEngineEventHandler::onAudioMixingStateChanged "onAudioMixingStateChanged" (PLAY)
  callback on the local client.
  When the audio mixing file playback finishes after calling this method, the
  SDK triggers the
  \ref IRtcEngineEventHandler::onAudioMixingStateChanged "onAudioMixingStateChanged" (STOPPED)
  callback on the local client.

  @note
  - Call this method after joining a channel, otherwise issues may occur.
  - If the local audio mixing file does not exist, or if the SDK does not
  support the file format or cannot access the music file URL, the SDK returns
  #WARN_AUDIO_MIXING_OPEN_ERROR (701).
  - If you want to play an online music file, ensure that the time interval
  between calling this method is more than 100 ms, or the
  `AUDIO_MIXING_ERROR_TOO_FREQUENT_CALL(702)` error code occurs.

  @param filePath Pointer to the absolute path (including the suffixes of the
  filename) of the local or online audio file to mix, for example, c:/music/au
  dio.mp4. Supported audio formats: 3GP, ASF, ADTS, AVI, MP3, MP4, MPEG-4,
  SAMI, and WAVE.
  @param loopback Sets which user can hear the audio mixing:
  - true: Only the local user can hear the audio mixing.
  - false: Both users can hear the audio mixing.

  @param cycle Sets the number of playback loops:
  - Positive integer: Number of playback loops.
  - `-1`: Infinite playback loops.

  @param startPos The playback position (ms) of the music file.

  @return
  - 0: Success.
  - < 0: Failure.
  */
  /**
   * @brief Starts playing the music file.
   *
   * @details
   * For the audio file formats supported by this method, see `What formats of audio files does the
   * Agora RTC SDK support`. If the local music file does not exist, the SDK does not support the file
   * format, or the the SDK cannot access the music file URL, the SDK reports
   * AUDIO_MIXING_REASON_CAN_NOT_OPEN.
   * Call timing: You can call this method either before or after joining a channel.
   * Related callbacks: A successful method call triggers the `onAudioMixingStateChanged`
   * (`AUDIO_MIXING_STATE_PLAYING`) callback. When the audio mixing file playback finishes, the SDK
   * triggers the `onAudioMixingStateChanged` (`AUDIO_MIXING_STATE_STOPPED`) callback on the local
   * client.
   *
   * @note
   * - If you call this method to play short sound effect files, you may encounter playback failure.
   * Agora recommends using `playEffect` instead to play such files.
   * - If you need to call this method multiple times, ensure that the time interval between calling
   * this method is more than 500 ms.
   * - On Android, there are following considerations:
   *   - To use this method, ensure that the Android device is v4.2 or later, and the API version is
   * v16 or later.
   *   - If you need to play an online music file, Agora does not recommend using the redirected URL
   * address. Some Android devices may fail to open a redirected URL address.
   *   - If you call this method on an emulator, ensure that the music file is in the `/sdcard/`
   * directory and the format is MP3.
   *
   * @param filePath File path:
   * - Android: The file path, which needs to be accurate to the file name and suffix. Agora supports
   * URL addresses, absolute paths, or file paths that start with `/assets/`. You might encounter
   * permission issues if you use an absolute path to access a local file, so Agora recommends using a
   * URI address instead. For example`:
   * content://com.android.providers.media.documents/document/audio%3A14441`
   * - Windows: The absolute path or URL address (including the suffixes of the filename) of the audio
   * effect file. For example`: C:\music\audio.mp4`.
   * @param loopback Whether to only play music files on the local client:
   * - `true`: Only play music files on the local client so that only the local user can hear the
   * music.
   * - `false`: Publish music files to remote clients so that both the local user and remote users can
   * hear the music.
   * @param cycle The number of times the music file plays.
   * - >0: The number of times for playback. For example, 1 represents playing 1 time.
   * - -1: Play the audio file in an infinite loop.
   * @param startPos The playback position (ms) of the music file.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   *   - -1: A general error occurs (no specified reason).
   *   - -2: The parameter is invalid.
   *   - -3: The SDK is not ready.
   *     - The audio module is disabled.
   *     - The program is not complete.
   *     - The initialization of `IRtcEngine` fails. Reinitialize the `IRtcEngine`.
   */
  virtual int startAudioMixing(const char* filePath, bool loopback, int cycle, int startPos) = 0;

  /**
   * @brief Stops playing the music file.
   *
   * @details
   * After calling `startAudioMixing(const char* filePath, bool loopback, int cycle, int startPos)` to
   * play a music file, you can call this method to stop the
   * playing. If you only need to pause the playback, call `pauseAudioMixing`.
   * Call timing: Call this method after joining a channel.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int stopAudioMixing() = 0;

  /**
   * @brief Pauses playing and mixing the music file.
   *
   * @details
   * After calling `startAudioMixing(const char* filePath, bool loopback, int cycle, int startPos)` to
   * play a music file, you can call this method to pause
   * the playing. If you need to stop the playback, call `stopAudioMixing`.
   * Call timing: Call this method after joining a channel.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int pauseAudioMixing() = 0;

  /**
   * @brief Resumes playing and mixing the music file.
   *
   * @details
   * After calling `pauseAudioMixing` to pause the playback, you can call this method to resume the
   * playback.
   * Call timing: Call this method after joining a channel.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int resumeAudioMixing() = 0;

  /**
   * @brief Selects the audio track used during playback.
   *
   * @details
   * After getting the track index of the audio file, you can call this method to specify any track to
   * play. For example, if different tracks of a multi-track file store songs in different languages,
   * you can call this method to set the playback language.
   *
   * @note
   * - For the supported formats of audio files, see
   * `https://docs.agora.io/en/help/general-product-inquiry/audio_format#extended-audio-file-formats`.
   * - You need to call this method after calling `startAudioMixing(const char* filePath, bool
   * loopback, int cycle, int startPos)` and receiving the
   * `onAudioMixingStateChanged` `(AUDIO_MIXING_STATE_PLAYING)` callback.
   *
   * @param index The audio track you want to specify. The value should be greater than 0 and less
   * than that of returned by `getAudioTrackCount`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int selectAudioTrack(int index) = 0;
  /**
   * @brief Gets the index of audio tracks of the current music file.
   *
   * @note You need to call this method after calling `startAudioMixing(const char* filePath, bool
   * loopback, int cycle, int startPos)` and receiving the
   * `onAudioMixingStateChanged` `(AUDIO_MIXING_STATE_PLAYING)` callback.
   *
   * @return
   * - The SDK returns the index of the audio tracks if the method call succeeds.
   * - < 0: Failure.
   */
  virtual int getAudioTrackCount() = 0;

  /**
   * @brief Adjusts the volume during audio mixing.
   *
   * @details
   * This method adjusts the audio mixing volume on both the local client and remote clients.
   * Call timing: Call this method after `startAudioMixing(const char* filePath, bool loopback, int
   * cycle, int startPos)`.
   *
   * @note This method does not affect the volume of the audio file set in the `playEffect` method.
   *
   * @param volume Audio mixing volume. The value ranges between 0 and 100. The default value is 100,
   * which means the original volume.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int adjustAudioMixingVolume(int volume) = 0;

  /**
   * @brief Adjusts the volume of audio mixing for publishing.
   *
   * @details
   * This method adjusts the volume of audio mixing for publishing (sending to other users).
   * Call timing: Call this method after calling `startAudioMixing(const char* filePath, bool
   * loopback, int cycle, int startPos)` and receiving the
   * `onAudioMixingStateChanged` `(AUDIO_MIXING_STATE_PLAYING)` callback.
   *
   * @param volume The volume of audio mixing for local playback. The value ranges between 0 and 100
   * (default). 100 represents the original volume.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int adjustAudioMixingPublishVolume(int volume) = 0;

  /**
   * @brief Retrieves the audio mixing volume for publishing.
   *
   * @details
   * This method helps troubleshoot audio volume‑related issues.
   *
   * @note You need to call this method after calling `startAudioMixing(const char* filePath, bool
   * loopback, int cycle, int startPos)` and receiving the
   * `onAudioMixingStateChanged` `(AUDIO_MIXING_STATE_PLAYING)` callback.
   *
   * @return
   * - ≥ 0: The audio mixing volume, if this method call succeeds. The value range is [0,100].
   * - < 0: Failure.
   */
  virtual int getAudioMixingPublishVolume() = 0;

  /**
   * @brief Adjusts the volume of audio mixing for local playback.
   *
   * @details
   * Call timing: You need to call this method after calling `startAudioMixing(const char* filePath,
   * bool loopback, int cycle, int startPos)` and receiving
   * the `onAudioMixingStateChanged` `(AUDIO_MIXING_STATE_PLAYING)` callback.
   *
   * @param volume The volume of audio mixing for local playback. The value ranges between 0 and 100
   * (default). 100 represents the original volume.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int adjustAudioMixingPlayoutVolume(int volume) = 0;

  /**
   * @brief Retrieves the audio mixing volume for local playback.
   *
   * @details
   * You can call this method to get the local playback volume of the mixed audio file, which helps in
   * troubleshooting volume‑related issues.
   * Call timing: Call this method after `startAudioMixing(const char* filePath, bool loopback, int
   * cycle, int startPos)` and receiving the
   * `onAudioMixingStateChanged` `(AUDIO_MIXING_STATE_PLAYING)` callback.
   *
   * @return
   * - ≥ 0: The audio mixing volume, if this method call succeeds. The value range is [0,100].
   * - < 0: Failure.
   */
  virtual int getAudioMixingPlayoutVolume() = 0;

  /**
   * @brief Retrieves the duration (ms) of the music file.
   *
   * @details
   * Retrieves the total duration (ms) of the audio.
   * Call timing: Call this method after `startAudioMixing(const char* filePath, bool loopback, int
   * cycle, int startPos)` and receiving the
   * `onAudioMixingStateChanged` `(AUDIO_MIXING_STATE_PLAYING)` callback.
   *
   * @return
   * - ≥ 0: The audio mixing duration, if this method call succeeds.
   * - < 0: Failure.
   */
  virtual int getAudioMixingDuration() = 0;

  /**
   * @brief Retrieves the playback position (ms) of the music file.
   *
   * @details
   * Retrieves the playback position (ms) of the audio.
   *
   * @note
   * - You need to call this method after calling `startAudioMixing(const char* filePath, bool
   * loopback, int cycle, int startPos)` and receiving the
   * `onAudioMixingStateChanged` `(AUDIO_MIXING_STATE_PLAYING)` callback.
   * - If you need to call `getAudioMixingCurrentPosition` multiple times, ensure that the time
   * interval between calling this method is more than 500 ms.
   *
   * @return
   * - ≥ 0: The current playback position (ms) of the audio mixing, if this method call succeeds. 0
   * represents that the current music file does not start playing.
   * - < 0: Failure.
   */
  virtual int getAudioMixingCurrentPosition() = 0;

  /**
   * @brief Sets the audio mixing position.
   *
   * @details
   * Call this method to set the playback position of the music file to a different starting position
   * (the default plays from the beginning).
   * Call timing: Call this method after `startAudioMixing(const char* filePath, bool loopback, int
   * cycle, int startPos)` and receiving the
   * `onAudioMixingStateChanged` `(AUDIO_MIXING_STATE_PLAYING)` callback.
   *
   * @param pos Integer. The playback position (ms).
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setAudioMixingPosition(int pos /*in ms*/) = 0;

  /**
   * @brief Sets the channel mode of the current audio file.
   *
   * @details
   * In a stereo music file, the left and right channels can store different audio data. According to
   * your needs, you can set the channel mode to original mode, left channel mode, right channel mode,
   * or mixed channel mode.
   * Applicable scenarios: For example, in the KTV scenario, the left channel of the music file stores
   * the musical accompaniment, and the right channel stores the original singer's vocals. You can set
   * according to actual needs:
   * - If you only want to hear the accompaniment, use this method to set the audio file's channel
   * mode to left channel mode.
   * - If you need to hear both the accompaniment and the original vocals simultaneously, call this
   * method to set the channel mode to mixed mode.
   * Call timing: Call this method after `startAudioMixing(const char* filePath, bool loopback, int
   * cycle, int startPos)` and receiving the
   * `onAudioMixingStateChanged` `(AUDIO_MIXING_STATE_PLAYING)` callback.
   *
   * @note This method only applies to stereo audio files.
   *
   * @param mode The channel mode. See `AUDIO_MIXING_DUAL_MONO_MODE`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setAudioMixingDualMonoMode(media::AUDIO_MIXING_DUAL_MONO_MODE mode) = 0;

  /**
   * @brief Sets the pitch of the local music file.
   *
   * @details
   * When a local music file is mixed with a local human voice, call this method to set the pitch of
   * the local music file only.
   * Call timing: You need to call this method after calling `startAudioMixing(const char* filePath,
   * bool loopback, int cycle, int startPos)` and receiving
   * the `onAudioMixingStateChanged` `(AUDIO_MIXING_STATE_PLAYING)` callback.
   *
   * @param pitch Sets the pitch of the local music file by the chromatic scale. The default value is
   * 0, which means keeping the original pitch. The value ranges from -12 to 12, and the pitch value
   * between consecutive values is a chromatic value. The greater the absolute value of this
   * parameter, the higher or lower the pitch of the local music file.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setAudioMixingPitch(int pitch) = 0;

  /**
   * @brief Sets the playback speed of the current audio file.
   *
   * @details
   * Ensure you call this method after calling `startAudioMixing(const char* filePath, bool loopback,
   * int cycle, int startPos)` receiving the
   * `onAudioMixingStateChanged` callback reporting the state as `AUDIO_MIXING_STATE_PLAYING`.
   *
   * @param speed The playback speed. Agora recommends that you set this to a value between 50 and
   * 400, defined as follows:
   * - 50: Half the original speed.
   * - 100: The original speed.
   * - 400: 4 times the original speed.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setAudioMixingPlaybackSpeed(int speed) = 0;

  /**
   * @brief Retrieves the volume of the audio effects.
   *
   * @details
   * The volume is an integer ranging from 0 to 100. The default value is 100, which means the
   * original volume.
   *
   * @note Call this method after `playEffect`.
   *
   * @return
   * - Volume of the audio effects, if this method call succeeds.
   * - < 0: Failure.
   */
  virtual int getEffectsVolume() = 0;
  /**
   * @brief Sets the volume of the audio effects.
   *
   * @details
   * Call timing: Call this method after `playEffect`.
   *
   * @param volume The playback volume. The value range is [0, 100]. The default value is 100, which
   * represents the original volume.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setEffectsVolume(int volume) = 0;
  /**
   * @brief Preloads a specified audio effect file into the memory.
   *
   * @details
   * Ensure the size of all preloaded files does not exceed the limit.
   * For the audio file formats supported by this method, see `What formats of audio files does the
   * Agora RTC SDK support`.
   * Call timing: Agora recommends that you call this method before joining a channel.
   * 
   * @note
   * - If preloadEffect is called before playEffect is executed, the file resource will not be closed after playEffect. 
   * The next time playEffect is executed, it will directly seek to play at the beginning.
   * - If preloadEffect is not called before playEffect is executed, the resource will be destroyed after playEffect. 
   * The next time playEffect is executed, it will try to reopen the file and play it from the beginning.
   * 
   * @param soundId The audio effect ID. The ID of each audio effect file is unique.
   * @param filePath File path:
   * - Android: The file path, which needs to be accurate to the file name and suffix. Agora supports
   * URL addresses, absolute paths, or file paths that start with `/assets/`. You might encounter
   * permission issues if you use an absolute path to access a local file, so Agora recommends using a
   * URI address instead. For example:
   * `content://com.android.providers.media.documents/document/audio%3A14441`
   * - Windows: The absolute path or URL address (including the suffixes of the filename) of the audio
   * effect file. For example: `C:\music\audio.mp4`.
   * - iOS or macOS: The absolute path or URL address (including the suffixes of the filename) of the audio effect file. For example: `/var/mobile/Containers/Data/audio.mp4`.
   * @param startPos The playback position (ms) of the audio effect file.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int preloadEffect(int soundId, const char* filePath, int startPos = 0) = 0;
  /**
   * @brief Plays the specified local or online audio effect file.
   *
   * @details
   * To play multiple audio effect files at the same time, call this method multiple times with
   * different `soundId` and `filePath`. To achieve the optimal user experience, Agora recommends that
   * you do not playing more than three audio files at the same time.
   * Call timing: You can call this method either before or after joining a channel.
   * Related callbacks: After the playback of an audio effect file completes, the SDK triggers the
   * `onAudioEffectFinished` callback.
   *
   * @note 
   * - If you need to play an online audio effect file, Agora recommends that you cache the online
   * audio effect file to your local device, call `preloadEffect` to preload the file into memory, and
   * then call this method to play the audio effect. Otherwise, you might encounter playback failures
   * or no sound during playback due to loading timeouts or failures.
   * - If preloadEffect is called before playEffect is executed, the file resource will not be closed after playEffect. 
   * The next time playEffect is executed, it will directly seek to play at the beginning.
   * - If preloadEffect is not called before playEffect is executed, the resource will be destroyed after playEffect. 
   * The next time playEffect is executed, it will try to reopen the file and play it from the beginning.
   *
   * @param soundId The audio effect ID. The ID of each audio effect file is unique.Attention: If you
   * have preloaded an audio effect into memory by calling `preloadEffect`, ensure that the value of
   * this parameter is the same as that of `soundId` in `preloadEffect`.
   * @param filePath The file path. The SDK supports URLs and absolute path of local files. The
   * absolute path needs to be accurate to the file name and extension. Supported audio formats
   * include MP3, AAC, M4A, MP4, WAV, and 3GP. See `Supported Audio Formats`.
   * Attention: If you have preloaded an audio effect into memory by calling `preloadEffect`, ensure
   * that the value of this parameter is the same as that of `filePath` in `preloadEffect`.
   * @param loopCount The number of times the audio effect loops.
   * - ≥ 0: The number of playback times. For example, 1 means looping one time, which means playing
   * the audio effect two times in total.
   * - -1: Play the audio file in an infinite loop.
   * @param pitch The pitch of the audio effect. The value range is 0.5 to 2.0. The default value is
   * 1.0, which means the original pitch. The lower the value, the lower the pitch.
   * @param pan The spatial position of the audio effect. The value ranges between -1.0 and 1.0:
   * - -1.0: The audio effect is heard on the left of the user.
   * - 0.0: The audio effect is heard in front of the user.
   * - 1.0: The audio effect is heard on the right of the user.
   * @param gain The volume of the audio effect. The value range is 0.0 to 100.0. The default value is
   * 100.0, which means the original volume. The smaller the value, the lower the volume.
   * @param publish Whether to publish the audio effect to the remote users:
   * - `true`: Publish the audio effect to the remote users. Both the local user and remote users can
   * hear the audio effect.
   * - `false`: Do not publish the audio effect to the remote users. Only the local user can hear the
   * audio effect.
   * @param startPos The playback position (ms) of the audio effect file.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int playEffect(int soundId, const char* filePath, int loopCount, double pitch, double pan, int gain, bool publish = false, int startPos = 0) = 0;
  /**
   * @brief Plays all audio effect files.
   *
   * @details
   * After calling `preloadEffect` multiple times to preload multiple audio effects into the memory,
   * you can call this method to play all the specified audio effects for all users in the channel.
   *
   * @param loopCount The number of times the audio effect loops:
   * - -1: Play the audio effect files in an indefinite loop until you call `stopEffect` or
   * `stopAllEffects`.
   * - 0: Play the audio effect once.
   * - 1: Play the audio effect twice.
   * @param pitch The pitch of the audio effect. The value ranges between 0.5 and 2.0. The default
   * value is 1.0 (original pitch). The lower the value, the lower the pitch.
   * @param pan The spatial position of the audio effect. The value ranges between -1.0 and 1.0:
   * - -1.0: The audio effect shows on the left.
   * - 0: The audio effect shows ahead.
   * - 1.0: The audio effect shows on the right.
   * @param gain The volume of the audio effect. The value range is [0, 100]. The default value is 100
   * (original volume). The smaller the value, the lower the volume.
   * @param publish Whether to publish the audio effect to the remote users:
   * - `true`: Publish the audio effect to the remote users. Both the local user and remote users can
   * hear the audio effect.
   * - `false`: (Default) Do not publish the audio effect to the remote users. Only the local user can
   * hear the audio effect.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int playAllEffects(int loopCount, double pitch, double pan, int gain, bool publish = false) = 0;

  /**
   * @brief Gets the volume of a specified audio effect file.
   *
   * @param soundId The ID of the audio effect file.
   *
   * @return
   * - ≥ 0: Returns the volume of the specified audio effect, if the method call is successful. The
   * value ranges between 0 and 100. 100 represents the original volume.
   * - < 0: Failure.
   */
  virtual int getVolumeOfEffect(int soundId) = 0;

  /**
   * @brief Gets the volume of a specified audio effect file.
   *
   * @details
   * Call timing: Call this method after `playEffect`.
   *
   * @param soundId The ID of the audio effect. The unique ID of each audio effect file.
   * @param volume The playback volume. The value range is [0, 100]. The default value is 100, which
   * represents the original volume.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setVolumeOfEffect(int soundId, int volume) = 0;
  /**
   * @brief Pauses a specified audio effect file.
   *
   * @param soundId The audio effect ID. The ID of each audio effect file is unique.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int pauseEffect(int soundId) = 0;
  /**
   * @brief Pauses all audio effects.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int pauseAllEffects() = 0;
  /**
   * @brief Resumes playing a specified audio effect.
   *
   * @param soundId The audio effect ID. The ID of each audio effect file is unique.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int resumeEffect(int soundId) = 0;
  /**
   * @brief Resumes playing all audio effect files.
   *
   * @details
   * After you call `pauseAllEffects` to pause the playback, you can call this method to resume the
   * playback.
   * Call timing: Call this method after `pauseAllEffects`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int resumeAllEffects() = 0;
  /**
   * @brief Stops playing a specified audio effect.
   *
   * @details
   * When you no longer need to play the audio effect, you can call this method to stop the playback.
   * If you only need to pause the playback, call `pauseEffect`.
   * Call timing: Call this method after `playEffect`.
   *
   * @param soundId The ID of the audio effect. Each audio effect has a unique ID.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int stopEffect(int soundId) = 0;
  /**
   * @brief Stops playing all audio effects.
   *
   * @details
   * When you no longer need to play the audio effect, you can call this method to stop the playback.
   * If you only need to pause the playback, call `pauseAllEffects`.
   * Call timing: Call this method after `playEffect`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int stopAllEffects() = 0;
  /**
   * @brief Releases a specified preloaded audio effect from the memory.
   *
   * @details
   * After loading the audio effect file into memory using `preloadEffect`, if you need to release the
   * audio effect file, call this method.
   * Call timing: You can call this method either before or after joining a channel.
   *
   * @param soundId The ID of the audio effect. Each audio effect has a unique ID.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int unloadEffect(int soundId) = 0;
  /**
   * @brief Releases a specified preloaded audio effect from the memory.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int unloadAllEffects() = 0;
  /**
   * @brief Retrieves the duration of the audio effect file.
   *
   * @note Call this method after joining a channel.
   *
   * @param filePath File path:
   * - Android: The file path, which needs to be accurate to the file name and suffix. Agora supports
   * URL addresses, absolute paths, or file paths that start with `/assets/`. You might encounter
   * permission issues if you use an absolute path to access a local file, so Agora recommends using a
   * URI address instead. For example:
   * `content://com.android.providers.media.documents/document/audio%3A14441`
   * - Windows: The absolute path or URL address (including the suffixes of the filename) of the audio
   * effect file. For example: `C:\music\audio.mp4`.
   * - iOS or macOS: The absolute path or URL address (including the suffixes of the filename) of the audio effect file. For example: `/var/mobile/Containers/Data/audio.mp4`.
   *
   * @return
   * - The total duration (ms) of the specified audio effect file, if the method call succeeds.
   * - < 0: Failure.
   */
  virtual int getEffectDuration(const char* filePath) = 0;
  /**
   * @brief Sets the playback position of an audio effect file.
   *
   * @details
   * After a successful setting, the local audio effect file starts playing at the specified position.
   *
   * @note Call this method after `playEffect`.
   *
   * @param soundId The audio effect ID. The ID of each audio effect file is unique.
   * @param pos The playback position (ms) of the audio effect file.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setEffectPosition(int soundId, int pos) = 0;
  /**
   * @brief Retrieves the playback position of the audio effect file.
   *
   * @note Call this method after `playEffect`.
   *
   * @param soundId The audio effect ID. The ID of each audio effect file is unique.
   *
   * @return
   * - The playback position (ms) of the specified audio effect file, if the method call succeeds.
   * - < 0: Failure.
   */
  virtual int getEffectCurrentPosition(int soundId) = 0;
  /**
   * @brief Enables or disables stereo panning for remote users.
   *
   * @details
   * Ensure that you call this method before joining a channel to enable stereo panning for remote
   * users so that the local user can track the position of a remote user by calling
   * `setRemoteVoicePosition`.
   *
   * @param enabled Whether to enable stereo panning for remote users:
   * - `true`: Enable stereo panning.
   * - `false`: Disable stereo panning.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int enableSoundPositionIndication(bool enabled) = 0;

  /**
   * @brief Sets the 2D position (the position on the horizontal plane) of the remote user's voice.
   *
   * @details
   * This method sets the 2D position and volume of a remote user, so that the local user can easily
   * hear and identify the remote user's position.
   * When the local user calls this method to set the voice position of a remote user, the voice
   * difference between the left and right channels allows the local user to track the real-time
   * position of the remote user, creating a sense of space. This method applies to massive
   * multiplayer online games, such as Battle Royale games.
   *
   * @note
   * - For this method to work, enable stereo panning for remote users by calling the
   * `enableSoundPositionIndication` method before joining a channel.
   * - For the best voice positioning, Agora recommends using a wired headset.
   * - Call this method after joining a channel.
   *
   * @param uid The user ID of the remote user.
   * @param pan The voice position of the remote user. The value ranges from -1.0 to 1.0:
   * - 0.0: (Default) The remote voice comes from the front.
   * - -1.0: The remote voice comes from the left.
   * - 1.0: The remote voice comes from the right.
   * @param gain The volume of the remote user. The value ranges from 0.0 to 100.0. The default value
   * is 100.0 (the original volume of the remote user). The smaller the value, the lower the volume.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setRemoteVoicePosition(uid_t uid, double pan, double gain) = 0;

  /**
   * @brief Enables or disables the spatial audio effect.
   *
   * @details
   * After enabling the spatial audio effect, you can call `setRemoteUserSpatialAudioParams` to set
   * the spatial audio effect parameters of the remote user.
   *
   * @note
   * - You can call this method either before or after joining a channel.
   * - This method relies on the spatial audio dynamic library `libagora_spatial_audio_extension.dll`.
   * If the dynamic library is deleted, the function cannot be enabled normally.
   *
   * @param enabled Whether to enable the spatial audio effect:
   * - `true`: Enable the spatial audio effect.
   * - `false`: Disable the spatial audio effect.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int enableSpatialAudio(bool enabled) = 0;

  /**
   * @brief Sets the spatial audio effect parameters of the remote user.
   *
   * @details
   * Call this method after `enableSpatialAudio`. After successfully setting the spatial audio effect
   * parameters of the remote user, the local user can hear the remote user with a sense of space.
   *
   * @param uid The user ID. This parameter must be the same as the user ID passed in when the user
   * joined the channel.
   * @param params The spatial audio parameters. See `SpatialAudioParams`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setRemoteUserSpatialAudioParams(uid_t uid, const agora::SpatialAudioParams& params) = 0;

  /**
   * @brief Sets a preset voice beautifier effect.
   *
   * @details
   * Call this method to set a preset voice beautifier effect for the local user who sends an audio
   * stream. After setting a voice beautifier effect, all users in the channel can hear the effect.
   * You can set different voice beautifier effects for different scenarios.
   * Call timing: This method can be called either before or after joining the channel.
   * To achieve better vocal effects, it is recommended that you call the following APIs before
   * calling this method:
   * - Call `setAudioScenario` to set the audio scenario to high-quality audio scenario, namely
   * `AUDIO_SCENARIO_GAME_STREAMING` (3).
   * - Call `setAudioProfile(AUDIO_PROFILE_TYPE profile)` to set the `profile` parameter to
   * `AUDIO_PROFILE_MUSIC_HIGH_QUALITY` (4) or `AUDIO_PROFILE_MUSIC_HIGH_QUALITY_STEREO` (5).
   *
   * @note
   * - Do not set the `profile` parameter in `setAudioProfile(AUDIO_PROFILE_TYPE profile)` to
   * `AUDIO_PROFILE_SPEECH_STANDARD` (1) or `AUDIO_PROFILE_IOT` (6), or the method does not take
   * effect.
   * - This method has the best effect on human voice processing, and Agora does not recommend calling
   * this method to process audio data containing music.
   * - After calling `setVoiceBeautifierPreset`, Agora does not recommend calling the following
   * methods, otherwise the effect set by `setVoiceBeautifierPreset` will be overwritten:
   *   - `setAudioEffectPreset`
   *   - `setAudioEffectParameters`
   *   - `setLocalVoicePitch`
   *   - `setLocalVoiceEqualization`
   *   - `setLocalVoiceReverb`
   *   - `setVoiceBeautifierParameters`
   *   - `setVoiceConversionPreset`
   * - This method relies on the voice beautifier dynamic library
   * `libagora_audio_beauty_extension.dll`. If the dynamic library is deleted, the function cannot be
   * enabled normally.
   *
   * @param preset The preset voice beautifier effect options: `VOICE_BEAUTIFIER_PRESET`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setVoiceBeautifierPreset(VOICE_BEAUTIFIER_PRESET preset) = 0;

  /**
   * @brief Sets an SDK preset audio effect.
   *
   * @details
   * Call this method to set an SDK preset audio effect for the local user who sends an audio stream.
   * This audio effect does not change the gender characteristics of the original voice. After setting
   * an audio effect, all users in the channel can hear the effect.
   * Call timing: This method can be called either before or after joining the channel.
   * To achieve better vocal effects, it is recommended that you call the following APIs before
   * calling this method:
   * - Call `setAudioScenario` to set the audio scenario to high-quality audio scenario, namely
   * `AUDIO_SCENARIO_GAME_STREAMING` (3).
   * - Call `setAudioProfile(AUDIO_PROFILE_TYPE profile)` to set the `profile` parameter to
   * `AUDIO_PROFILE_MUSIC_HIGH_QUALITY` (4) or `AUDIO_PROFILE_MUSIC_HIGH_QUALITY_STEREO` (5).
   *
   * @note
   * - Do not set the `profile` parameter in `setAudioProfile(AUDIO_PROFILE_TYPE profile)` to
   * `AUDIO_PROFILE_SPEECH_STANDARD` (1) or `AUDIO_PROFILE_IOT` (6), or the method does not take
   * effect.
   * - If you call `setAudioEffectPreset` and set enumerators except for `ROOM_ACOUSTICS_3D_VOICE` or
   * `PITCH_CORRECTION`, do not call `setAudioEffectParameters`; otherwise, `setAudioEffectPreset` is
   * overridden.
   * - After calling `setAudioEffectPreset`, Agora does not recommend you to call the following
   * methods, otherwise the effect set by `setAudioEffectPreset` will be overwritten:
   *   - `setVoiceBeautifierPreset`
   *   - `setLocalVoicePitch`
   *   - `setLocalVoiceEqualization`
   *   - `setLocalVoiceReverb`
   *   - `setVoiceBeautifierParameters`
   *   - `setVoiceConversionPreset`
   * - This method relies on the voice beautifier dynamic library
   * `libagora_audio_beauty_extension.dll`. If the dynamic library is deleted, the function cannot be
   * enabled normally.
   *
   * @param preset The options for SDK preset audio effects. See `AUDIO_EFFECT_PRESET`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setAudioEffectPreset(AUDIO_EFFECT_PRESET preset) = 0;

  /**
   * @brief Sets a preset voice beautifier effect.
   *
   * @details
   * Call this method to set a preset voice changing effect for the local user who publishes an audio
   * stream in a channel. After setting the voice changing effect, all users in the channel can hear
   * the effect. You can set different voice changing effects for the user depending on different
   * scenarios.
   * Call timing: This method can be called either before or after joining the channel.
   * To achieve better vocal effects, it is recommended that you call the following APIs before
   * calling this method:
   * - Call `setAudioScenario` to set the audio scenario to high-quality audio scenario, namely
   * `AUDIO_SCENARIO_GAME_STREAMING` (3).
   * - Call `setAudioProfile(AUDIO_PROFILE_TYPE profile)` to set the `profile` parameter to
   * `AUDIO_PROFILE_MUSIC_HIGH_QUALITY` (4) or `AUDIO_PROFILE_MUSIC_HIGH_QUALITY_STEREO` (5).
   *
   * @note
   * - Do not set the `profile` parameter in `setAudioProfile(AUDIO_PROFILE_TYPE profile)` to
   * `AUDIO_PROFILE_SPEECH_STANDARD` (1) or `AUDIO_PROFILE_IOT` (6), or the method does not take
   * effect.
   * - This method has the best effect on human voice processing, and Agora does not recommend calling
   * this method to process audio data containing music.
   * - After calling `setVoiceConversionPreset`, Agora does not recommend you to call the following
   * methods, otherwise the effect set by `setVoiceConversionPreset` will be overwritten:
   *   - `setAudioEffectPreset`
   *   - `setAudioEffectParameters`
   *   - `setVoiceBeautifierPreset`
   *   - `setVoiceBeautifierParameters`
   *   - `setLocalVoicePitch`
   *   - `setLocalVoiceFormant`
   *   - `setLocalVoiceEqualization`
   *   - `setLocalVoiceReverb`
   * - This method relies on the voice beautifier dynamic library
   * `libagora_audio_beauty_extension.dll`. If the dynamic library is deleted, the function cannot be
   * enabled normally.
   *
   * @param preset The options for the preset voice beautifier effects: `VOICE_CONVERSION_PRESET`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setVoiceConversionPreset(VOICE_CONVERSION_PRESET preset) = 0;

  /**
   * @brief Sets parameters for SDK preset audio effects.
   *
   * @details
   * Call this method to set the following parameters for the local user who sends an audio stream:
   * - 3D voice effect: Sets the cycle period of the 3D voice effect.
   * - Pitch correction effect: Sets the basic mode and tonic pitch of the pitch correction effect.
   * Different songs have different modes and tonic pitches. Agora recommends bounding this method
   * with interface elements to enable users to adjust the pitch correction interactively.
   * After setting the audio parameters, all users in the channel can hear the effect.
   * To achieve better vocal effects, it is recommended that you call the following APIs before
   * calling this method:
   * - Call `setAudioScenario` to set the audio scenario to high-quality audio scenario, namely
   * `AUDIO_SCENARIO_GAME_STREAMING` (3).
   * - Call `setAudioProfile(AUDIO_PROFILE_TYPE profile)` to set the `profile` parameter to
   * `AUDIO_PROFILE_MUSIC_HIGH_QUALITY` (4) or `AUDIO_PROFILE_MUSIC_HIGH_QUALITY_STEREO` (5).
   *
   * @note
   * - You can call this method either before or after joining a channel.
   * - Do not set the `profile` parameter in `setAudioProfile(AUDIO_PROFILE_TYPE profile)` to
   * `AUDIO_PROFILE_SPEECH_STANDARD` (1) or `AUDIO_PROFILE_IOT` (6), or the method does not take
   * effect.
   * - This method has the best effect on human voice processing, and Agora does not recommend calling
   * this method to process audio data containing music.
   * - After calling `setAudioEffectParameters`, Agora does not recommend you to call the following
   * methods, otherwise the effect set by `setAudioEffectParameters` will be overwritten:
   *   - `setAudioEffectPreset`
   *   - `setVoiceBeautifierPreset`
   *   - `setLocalVoicePitch`
   *   - `setLocalVoiceEqualization`
   *   - `setLocalVoiceReverb`
   *   - `setVoiceBeautifierParameters`
   *   - `setVoiceConversionPreset`
   * - This method relies on the voice beautifier dynamic library
   * `libagora_audio_beauty_extension.dll`. If the dynamic library is deleted, the function cannot be
   * enabled normally.
   *
   * @param preset The options for SDK preset audio effects:
   * - `ROOM_ACOUSTICS_3D_VOICE`, 3D voice effect:
   *   - You need to set the `profile` parameter in `setAudioProfile(AUDIO_PROFILE_TYPE profile)` to
   * `AUDIO_PROFILE_MUSIC_STANDARD_STEREO` (3) or `AUDIO_PROFILE_MUSIC_HIGH_QUALITY_STEREO` (5) before
   * setting this enumerator; otherwise, the enumerator setting does not take effect.
   *   - If the 3D voice effect is enabled, users need to use stereo audio playback devices to hear
   * the anticipated voice effect.
   * - `PITCH_CORRECTION`, Pitch correction effect:
   * @param param1 - If you set `preset` to `ROOM_ACOUSTICS_3D_VOICE`, `param1` sets the cycle period
   * of the 3D voice effect. The value range is [1,60] and the unit is seconds. The default value is
   * 10, indicating that the voice moves around you every 10 seconds.
   * - If you set `preset` to `PITCH_CORRECTION`, `param1` indicates the basic mode of the pitch
   * correction effect:
   *   - `1`: (Default) Natural major scale.
   *   - `2`: Natural minor scale.
   *   - `3`: Japanese pentatonic scale.
   * @param param2 - If you set `preset` to `ROOM_ACOUSTICS_3D_VOICE` , you need to set `param2` to
   * `0`.
   * - If you set `preset` to `PITCH_CORRECTION`, `param2` indicates the tonic pitch of the pitch
   * correction effect:
   *   - `1`: A
   *   - `2`: A#
   *   - `3`: B
   *   - `4`: (Default) C
   *   - `5`: C#
   *   - `6`: D
   *   - `7`: D#
   *   - `8`: E
   *   - `9`: F
   *   - `10`: F#
   *   - `11`: G
   *   - `12`: G#
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setAudioEffectParameters(AUDIO_EFFECT_PRESET preset, int param1, int param2) = 0;

  /**
   * @brief Sets parameters for the preset voice beautifier effects.
   *
   * @details
   * Call this method to set a gender characteristic and a reverberation effect for the singing
   * beautifier effect. This method sets parameters for the local user who sends an audio stream.
   * After setting the audio parameters, all users in the channel can hear the effect.
   * To achieve better vocal effects, it is recommended that you call the following APIs before
   * calling this method:
   * - Call `setAudioScenario` to set the audio scenario to high-quality audio scenario, namely
   * `AUDIO_SCENARIO_GAME_STREAMING` (3).
   * - Call `setAudioProfile(AUDIO_PROFILE_TYPE profile)` to set the `profile` parameter to
   * `AUDIO_PROFILE_MUSIC_HIGH_QUALITY` (4) or `AUDIO_PROFILE_MUSIC_HIGH_QUALITY_STEREO` (5).
   *
   * @note
   * - You can call this method either before or after joining a channel.
   * - Do not set the `profile` parameter in `setAudioProfile(AUDIO_PROFILE_TYPE profile)` to
   * `AUDIO_PROFILE_SPEECH_STANDARD` (1) or `AUDIO_PROFILE_IOT` (6), or the method does not take
   * effect.
   * - This method has the best effect on human voice processing, and Agora does not recommend calling
   * this method to process audio data containing music.
   * - After calling `setVoiceBeautifierParameters`, Agora does not recommend calling the following
   * methods, otherwise the effect set by `setVoiceBeautifierParameters` will be overwritten:
   *   - `setAudioEffectPreset`
   *   - `setAudioEffectParameters`
   *   - `setVoiceBeautifierPreset`
   *   - `setLocalVoicePitch`
   *   - `setLocalVoiceEqualization`
   *   - `setLocalVoiceReverb`
   *   - `setVoiceConversionPreset`
   * - This method relies on the voice beautifier dynamic library
   * `libagora_audio_beauty_extension.dll`. If the dynamic library is deleted, the function cannot be
   * enabled normally.
   *
   * @param preset The option for the preset audio effect:
   * - `SINGING_BEAUTIFIER`: The singing beautifier effect.
   * @param param1 The gender characteristics options for the singing voice:
   * - `1`: A male-sounding voice.
   * - `2`: A female-sounding voice.
   * @param param2 The reverberation effect options for the singing voice:
   * - `1`: The reverberation effect sounds like singing in a small room.
   * - `2`: The reverberation effect sounds like singing in a large room.
   * - `3`: The reverberation effect sounds like singing in a hall.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setVoiceBeautifierParameters(VOICE_BEAUTIFIER_PRESET preset,
                                            int param1, int param2) = 0;

  /** Set parameters for SDK preset voice conversion.
   *
   * @note
   * - reserved interface
   *
   * @param preset The options for SDK preset audio effects. See #VOICE_CONVERSION_PRESET.
   * @param param1 reserved.
   * @param param2 reserved.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setVoiceConversionParameters(VOICE_CONVERSION_PRESET preset,
                                            int param1, int param2) = 0;

  /**
   * @brief Changes the voice pitch of the local speaker.
   *
   * @details
   * Call timing: This method can be called either before or after joining the channel.
   *
   * @param pitch The local voice pitch. The value range is [0.5,2.0]. The lower the value, the lower
   * the pitch. The default value is 1.0 (no change to the pitch).
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setLocalVoicePitch(double pitch) = 0;

  /**
   * @brief Sets the formant ratio to change the timbre of human voice.
   *
   * @details
   * Formant ratio affects the timbre of voice. The smaller the value, the deeper the sound will be,
   * and the larger, the sharper. After you set the formant ratio, all users in the channel can hear
   * the changed voice. If you want to change the timbre and pitch of voice at the same time, Agora
   * recommends using this method together with `setLocalVoicePitch`.
   * Applicable scenarios: You can call this method to set the formant ratio of local audio to change
   * the timbre of human voice.
   * Call timing: This method can be called either before or after joining the channel.
   *
   * @param formantRatio The formant ratio. The value range is [-1.0, 1.0]. The default value is 0.0,
   * which means do not change the timbre of the voice.Note: Agora recommends setting this value
   * within the range of [-0.4, 0.6]. Otherwise, the voice may be seriously distorted.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setLocalVoiceFormant(double formantRatio) = 0;

  /**
   * @brief Sets the local voice equalization effect.
   *
   * @details
   * Call timing: This method can be called either before or after joining the channel.
   *
   * @param bandFrequency The band frequency. The value ranges between 0 and 9; representing the
   * respective 10-band center frequencies of the voice effects, including 31, 62, 125, 250, 500, 1k,
   * 2k, 4k, 8k, and 16k Hz. See `AUDIO_EQUALIZATION_BAND_FREQUENCY`.
   * @param bandGain The gain of each band in dB. The value ranges between -15 and 15. The default
   * value is 0.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setLocalVoiceEqualization(AUDIO_EQUALIZATION_BAND_FREQUENCY bandFrequency, int bandGain) = 0;

  /**
   * @brief Sets the local voice reverberation.
   *
   * @details
   * The SDK provides an easier-to-use method, `setAudioEffectPreset`, to directly implement preset
   * reverb effects for such as pop, R&B, and KTV.
   *
   * @note You can call this method either before or after joining a channel.
   *
   * @param reverbKey The reverberation key. Agora provides five reverberation keys, see
   * `AUDIO_REVERB_TYPE`.
   * @param value The value of the reverberation key.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setLocalVoiceReverb(AUDIO_REVERB_TYPE reverbKey, int value) = 0;
  /**
   * @brief Sets the preset headphone equalization effect.
   *
   * @details
   * This method is mainly used in spatial audio effect scenarios. You can select the preset headphone
   * equalizer to listen to the audio to achieve the expected audio experience.
   *
   * @note If the headphones you use already have a good equalization effect, you may not get a
   * significant improvement when you call this method, and could even diminish the experience.
   *
   * @param preset The preset headphone equalization effect. See `HEADPHONE_EQUALIZER_PRESET`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   *   - -1: A general error occurs (no specified reason).
   */
  virtual int setHeadphoneEQPreset(HEADPHONE_EQUALIZER_PRESET preset) = 0;

  /**
   * @brief Sets the low- and high-frequency parameters of the headphone equalizer.
   *
   * @details
   * In a spatial audio effect scenario, if the preset headphone equalization effect is not achieved
   * after calling the `setHeadphoneEQPreset` method, you can further adjust the headphone
   * equalization effect by calling this method.
   *
   * @param lowGain The low-frequency parameters of the headphone equalizer. The value range is
   * [-10,10]. The larger the value, the deeper the sound.
   * @param highGain The high-frequency parameters of the headphone equalizer. The value range is
   * [-10,10]. The larger the value, the sharper the sound.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   *   - -1: A general error occurs (no specified reason).
   */
  virtual int setHeadphoneEQParameters(int lowGain, int highGain) = 0;

  /**
   * @brief Enables or disables the voice AI tuner.
   *
   * @details
   * The voice AI tuner supports enhancing sound quality and adjusting tone style.
   * Applicable scenarios: Social entertainment scenes including online KTV, online podcast and live
   * streaming in showrooms, where high sound quality is required.
   * Call timing: This method can be called either before or after joining the channel.
   *
   * @param enabled Whether to enable the voice AI tuner:
   * - `true`: Enables the voice AI tuner.
   * - `false`: (Default) Disable the voice AI tuner.
   * @param type Voice AI tuner sound types, see `VOICE_AI_TUNER_TYPE`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int enableVoiceAITuner(bool enabled, VOICE_AI_TUNER_TYPE type) = 0;

  /**
   * @brief Sets the log file.
   *
   * @details
   * Specifies an SDK output log file. The log file records all log data for the SDK’s operation.
   * Call timing: This method needs to be called immediately after `initialize`, otherwise the output
   * log may be incomplete.
   *
   * @note Ensure that the directory for the log file exists and is writable.
   *
   * @param filePath The complete path of the log files. These log files are encoded in UTF-8.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setLogFile(const char* filePath) = 0;

  /**
   * @brief Sets the log output level of the SDK.
   *
   * @details
   * This method sets the output log level of the SDK. You can use one or a combination of the log
   * filter levels. The log level follows the sequence of `LOG_FILTER_OFF`, `LOG_FILTER_CRITICAL`,
   * `LOG_FILTER_ERROR`, `LOG_FILTER_WARN`, `LOG_FILTER_INFO`, and `LOG_FILTER_DEBUG`. Choose a level
   * to see the logs preceding that level.
   * If, for example, you set the log level to `LOG_FILTER_WARN`, you see the logs within levels
   * `LOG_FILTER_CRITICAL`, `LOG_FILTER_ERROR` and `LOG_FILTER_WARN`.
   *
   * @param filter The output log level of the SDK. See `LOG_FILTER_TYPE`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setLogFilter(unsigned int filter) = 0;

  /**
   * @brief Sets the output log level of the SDK.
   *
   * @details
   * Choose a level to see the logs preceding that level.
   *
   * @param level The log level. See `LOG_LEVEL`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setLogLevel(commons::LOG_LEVEL level) = 0;

  /**
   * @brief Sets the log file size.
   *
   * @details
   * By default, the SDK generates five SDK log files and five API call log files with the following
   * rules:
   * - The SDK log files are: `agorasdk.log`, `agorasdk.1.log`, `agorasdk.2.log`, `agorasdk.3.log`,
   * and `agorasdk.4.log`.
   * - The API call log files are: `agoraapi.log`, `agoraapi.1.log`, `agoraapi.2.log`,
   * `agoraapi.3.log`, and `agoraapi.4.log`.
   * - The default size of each SDK log file and API log file is 2,048 KB. These log files are encoded
   * in UTF-8.
   * - The SDK writes the latest logs in `agorasdk.log` or `agoraapi.log`.
   * - When `agorasdk.log` is full, the SDK processes the log files in the following order:1. Delete
   * the `agorasdk.4.log` file (if any).
   * 2. Rename `agorasdk.3.log` to `agorasdk.4.log`.
   * 3. Rename `agorasdk.2.log` to `agorasdk.3.log`.
   * 4. Rename `agorasdk.1.log` to `agorasdk.2.log`.
   * 5. Create a new `agorasdk.log` file.
   * - The overwrite rules for the `agoraapi.log` file are the same as for `agorasdk.log`.
   *
   * @note This method is used to set the size of the `agorasdk.log` file only and does not effect the
   * `agoraapi.log file`.
   *
   * @param fileSizeInKBytes The size (KB) of an `agorasdk.log` file. The value range is [128,20480].
   * The default value is 2,048 KB. If you set `fileSizeInKByte` smaller than 128 KB, the SDK
   * automatically adjusts it to 128 KB; if you set `fileSizeInKByte` greater than 20,480 KB, the SDK
   * automatically adjusts it to 20,480 KB.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setLogFileSize(unsigned int fileSizeInKBytes) = 0;

  /** Upload current log file immediately to server.
   *  only use this when an error occurs
   *  block before log file upload success or timeout.
   *
   *  @return
   *  - 0: Success.
   *  - < 0: Failure.
   */
  virtual int uploadLogFile(agora::util::AString& requestId) = 0;

   /** * Write the log to SDK . @technical preview
   *
   * You can Write the log to SDK log files of the specified level.
   *
   * @param level The log level:
   * - `LOG_LEVEL_NONE (0x0000)`: Do not output any log file.
   * - `LOG_LEVEL_INFO (0x0001)`: (Recommended) Output log files of the INFO level.
   * - `LOG_LEVEL_WARN (0x0002)`: Output log files of the WARN level.
   * - `LOG_LEVEL_ERROR (0x0004)`: Output log files of the ERROR level.
   * - `LOG_LEVEL_FATAL (0x0008)`: Output log files of the FATAL level.
   *
   *  @return
   *  - 0: Success.
   *  - < 0: Failure.
   */
  virtual int writeLog(commons::LOG_LEVEL level, const char* fmt, ...) = 0;

  /**
   * @brief Updates the display mode of the local video view.
   *
   * @details
   * After initializing the local video view, you can call this method to update its rendering and
   * mirror modes. It affects only the video view that the local user sees and does not impact the
   * publishing of the local video.
   * Call timing: - Ensure that you have called the `setupLocalVideo` method to initialize the local
   * video view before calling this method.
   * - During a call, you can call this method as many times as necessary to update the display mode
   * of the local video view.
   *
   * @note This method only takes effect on the primary camera `(PRIMARY_CAMERA_SOURCE)`. In scenarios
   * involving custom video capture or the use of alternative video sources, you need to use
   * `setupLocalVideo` instead of this method.
   *
   * @param renderMode The local video display mode. See `RENDER_MODE_TYPE`.
   * @param mirrorMode The mirror mode of the local video view. See `VIDEO_MIRROR_MODE_TYPE`.
   * Attention: If you use a front camera, the SDK enables the mirror mode by default; if you use a
   * rear camera, the SDK disables the mirror mode by default.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setLocalRenderMode(media::base::RENDER_MODE_TYPE renderMode, VIDEO_MIRROR_MODE_TYPE mirrorMode) = 0;

  /**
   * @brief Updates the display mode of the video view of a remote user.
   *
   * @details
   * After initializing the video view of a remote user, you can call this method to update its
   * rendering and mirror modes. This method affects only the video view that the local user sees.
   *
   * @note
   * - Call this method after initializing the remote view by calling the `setupRemoteVideo` method.
   * - During a call, you can call this method as many times as necessary to update the display mode
   * of the video view of a remote user.
   *
   * @param uid The user ID of the remote user.
   * @param renderMode The rendering mode of the remote user view. For details, see
   * `RENDER_MODE_TYPE`.
   * @param mirrorMode The mirror mode of the remote user view. See `VIDEO_MIRROR_MODE_TYPE`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setRemoteRenderMode(uid_t uid, media::base::RENDER_MODE_TYPE renderMode,
                                  VIDEO_MIRROR_MODE_TYPE mirrorMode) = 0;
  /**
   * @brief Sets the maximum frame rate for rendering local video.
   *
   * @details
   * Applicable scenarios: In scenarios where the requirements for video rendering frame rate are not
   * high (such as screen sharing or online education), you can call this method to set the maximum
   * frame rate for local video rendering. The SDK will attempt to keep the actual frame rate of local
   * rendering close to this value, which helps to reduce CPU consumption and improving system
   * performance.
   * Call timing: You can call this method either before or after joining a channel.
   *
   * @param sourceType The type of the video source. See `VIDEO_SOURCE_TYPE`.
   * @param targetFps The capture frame rate (fps) of the local video. Sopported values are: 1, 7, 10,
   * 15, 24, 30, 60.CAUTION: Set this parameter to a value lower than the actual video frame rate;
   * otherwise, the settings do not take effect.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setLocalRenderTargetFps(VIDEO_SOURCE_TYPE sourceType, int targetFps) = 0;
  /**
   * @brief Sets the maximum frame rate for rendering remote video.
   *
   * @details
   * Applicable scenarios: In scenarios where the video rendering frame rate is not critical (e.g.,
   * screen sharing, online education) or when the remote users are using mid-to-low-end devices, you
   * can call this method to set the maximum frame rate for video rendering on the remote client. The
   * SDK will attempt to render the actual frame rate as close as possible to this value, which helps
   * to reduce CPU consumption and improve system performance.
   * Call timing: You can call this method either before or after joining a channel.
   *
   * @param targetFps The capture frame rate (fps) of the local video. Sopported values are: 1, 7, 10,
   * 15, 24, 30, 60.CAUTION: Set this parameter to a value lower than the actual video frame rate;
   * otherwise, the settings do not take effect.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setRemoteRenderTargetFps(int targetFps) = 0;
  // The following APIs are either deprecated and going to deleted.

  /**
   * Updates the display mode of the local video view.
   *
   * After initializing the local video view, you can call this method to  update its rendering mode.
   * It affects only the video view that the local user sees, not the published local video stream.
   *
   * @note
   * - Ensure that you have called \ref setupLocalVideo "setupLocalVideo" to initialize the local video
   * view before this method.
   * - During a call, you can call this method as many times as necessary to update the local video view.
   *
   * @param renderMode Sets the local display mode. See #RENDER_MODE_TYPE.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setLocalRenderMode(media::base::RENDER_MODE_TYPE renderMode) __deprecated = 0;

  /**
   * @brief Sets the local video mirror mode.
   *
   * @param mirrorMode The local video mirror mode. See `VIDEO_MIRROR_MODE_TYPE`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setLocalVideoMirrorMode(VIDEO_MIRROR_MODE_TYPE mirrorMode) __deprecated = 0;

  /**
   * @brief Enables or disables dual-stream mode on the sender side.
   *
   * @details
   * Dual streams are a pairing of a high-quality video stream and a low-quality video stream:
   * - High-quality video stream: High bitrate, high resolution.
   * - Low-quality video stream: Low bitrate, low resolution.
   * After you enable dual-stream mode, you can call `setRemoteVideoStreamType` to choose to receive
   * either the high-quality video stream or the low-quality video stream on the subscriber side.
   *
   * @note
   * - This method is applicable to all types of streams from the sender, including but not limited to
   * video streams collected from cameras, screen sharing streams, and custom-collected video streams.
   * - If you need to enable dual video streams in a multi-channel scenario, you can call the
   * `enableDualStreamModeEx` method.
   * - You can call this method either before or after joining a channel.
   *
   * @param enabled Whether to enable dual-stream mode:
   * - `true`: Enable dual-stream mode.
   * - `false`: (Default) Disable dual-stream mode.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int enableDualStreamMode(bool enabled) __deprecated = 0;

  /**
   * @brief Sets the dual-stream mode on the sender side and the low-quality video stream.
   *
   * @details
   * You can call this method to enable or disable the dual-stream mode on the publisher side. Dual
   * streams are a pairing of a high-quality video stream and a low-quality video stream:
   * - High-quality video stream: High bitrate, high resolution.
   * - Low-quality video stream: Low bitrate, low resolution.
   * After you enable dual-stream mode, you can call `setRemoteVideoStreamType` to choose to receive
   * either the high-quality video stream or the low-quality video stream on the subscriber side.
   *
   * @note
   * - This method is applicable to all types of streams from the sender, including but not limited to
   * video streams collected from cameras, screen sharing streams, and custom-collected video streams.
   * - If you need to enable dual video streams in a multi-channel scenario, you can call the
   * `enableDualStreamModeEx` method.
   * - You can call this method either before or after joining a channel.
   *
   * @param enabled Whether to enable dual-stream mode:
   * - `true`: Enable dual-stream mode.
   * - `false`: (Default) Disable dual-stream mode.
   * @param streamConfig The configuration of the low-quality video stream. See
   * `SimulcastStreamConfig`.Note: When setting `mode` to `DISABLE_SIMULCAST_STREAM`, setting
   * `streamConfig` will not take effect.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int enableDualStreamMode(bool enabled, const SimulcastStreamConfig& streamConfig) __deprecated = 0;


  /**
   * @brief Sets the dual-stream mode on the sender side.
   *
   * @details
   * The SDK defaults to enabling low-quality video stream adaptive mode ( `AUTO_SIMULCAST_STREAM` )
   * on the sender side, which means the sender does not actively send low-quality video stream. The
   * receiving end with the role of the **host** can initiate a low-quality video stream request by
   * calling `setRemoteVideoStreamType`, and upon receiving the request, the sending end automatically
   * starts sending low-quality stream.
   * - If you want to modify this behavior, you can call this method and set `mode` to
   * `DISABLE_SIMULCAST_STREAM` (never send low-quality video streams) or `ENABLE_SIMULCAST_STREAM`
   * (always send low-quality video streams).
   * - If you want to restore the default behavior after making changes, you can call this method
   * again with `mode` set to `AUTO_SIMULCAST_STREAM`.
   *
   * @note
   * The difference and connection between this method and `enableDualStreamMode(bool enabled)` is as
   * follows:
   * - When calling this method and setting `mode` to `DISABLE_SIMULCAST_STREAM`, it has the same
   * effect as `enableDualStreamMode(bool enabled)` `(false)`.
   * - When calling this method and setting `mode` to `ENABLE_SIMULCAST_STREAM`, it has the same
   * effect as `enableDualStreamMode(bool enabled)` `(true)`.
   * - Both methods can be called before and after joining a channel. If both methods are used, the
   * settings in the method called later takes precedence.
   *
   * @param mode The mode in which the video stream is sent. See `SIMULCAST_STREAM_MODE`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setDualStreamMode(SIMULCAST_STREAM_MODE mode) = 0;

  /**
   * @brief Sets the simulcast video stream configuration.
   *
   * @since v4.6.0
   *
   * @details
   * You can call this method to set video streams with different resolutions for the same video
   * source. The subscribers can call `setRemoteVideoStreamType` to select which stream layer to
   * receive. The broadcaster can publish up to four layers of video streams: one main stream (highest
   * resolution) and three additional streams of different quality levels.
   *
   * @param simulcastConfig This configuration includes seven layers, from STREAM_LAYER_1 to
   * STREAM_LOW, with a maximum of three layers enabled simultaneously. See `SimulcastConfig`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setSimulcastConfig(const SimulcastConfig& simulcastConfig) = 0;

  /**
   * @brief Sets dual-stream mode configuration on the sender side.
   *
   * @details
   * The SDK defaults to enabling low-quality video stream adaptive mode ( `AUTO_SIMULCAST_STREAM` )
   * on the sender side, which means the sender does not actively send low-quality video stream. The
   * receiving end with the role of the **host** can initiate a low-quality video stream request by
   * calling `setRemoteVideoStreamType`, and upon receiving the request, the sending end automatically
   * starts sending low-quality stream.
   * - If you want to modify this behavior, you can call this method and set `mode` to
   * `DISABLE_SIMULCAST_STREAM` (never send low-quality video streams) or `ENABLE_SIMULCAST_STREAM`
   * (always send low-quality video streams).
   * - If you want to restore the default behavior after making changes, you can call this method
   * again with `mode` set to `AUTO_SIMULCAST_STREAM`.
   * The difference between this method and `setDualStreamMode(SIMULCAST_STREAM_MODE mode)` is that
   * this method can also
   * configure the low-quality video stream, and the SDK sends the stream according to the
   * configuration in `streamConfig`.
   *
   * @note
   * The difference and connection between this method and `enableDualStreamMode(bool enabled, const
   * SimulcastStreamConfig& streamConfig)` is as follows:
   * - When calling this method and setting `mode` to `DISABLE_SIMULCAST_STREAM`, it has the same
   * effect as calling `enableDualStreamMode(bool enabled, const SimulcastStreamConfig& streamConfig)`
   * and setting `enabled` to `false`.
   * - When calling this method and setting `mode` to `ENABLE_SIMULCAST_STREAM`, it has the same
   * effect as calling `enableDualStreamMode(bool enabled, const SimulcastStreamConfig& streamConfig)`
   * and setting `enabled` to `true`.
   * - Both methods can be called before and after joining a channel. If both methods are used, the
   * settings in the method called later takes precedence.
   *
   * @param mode The mode in which the video stream is sent. See `SIMULCAST_STREAM_MODE`.
   * @param streamConfig The configuration of the low-quality video stream. See
   * `SimulcastStreamConfig`.Note: When setting `mode` to `DISABLE_SIMULCAST_STREAM`, setting
   * `streamConfig` will not take effect.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setDualStreamMode(SIMULCAST_STREAM_MODE mode, const SimulcastStreamConfig& streamConfig) = 0;

  /**
   * @brief Sets whether to enable the local playback of external audio source.
   *
   * @details
   * After calling this method to enable the local playback of external audio source, if you need to
   * stop local playback, you can call this method again and set `enabled` to `false`.
   * You can call `adjustCustomAudioPlayoutVolume` to adjust the local playback volume of the custom
   * audio track.
   *
   * @note Ensure you have called the `createCustomAudioTrack` method to create a custom audio track
   * before calling this method.
   *
   * @param trackId The audio track ID. Set this parameter to the custom audio track ID returned in
   * `createCustomAudioTrack`.
   * @param enabled Whether to play the external audio source:
   * - `true`: Play the external audio source.
   * - `false`: (Default) Do not play the external source.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int enableCustomAudioLocalPlayback(track_id_t trackId, bool enabled) = 0;

  /**
   * @brief Sets the format of the captured raw audio data.
   *
   * @details
   * The SDK calculates the sampling interval based on the `samplesPerCall`, `sampleRate` and
   * `channel` parameters set in this method.Sample interval (sec) = `samplePerCall` /( `sampleRate` ×
   * `channel` ). Ensure that the sample interval ≥ 0.01 (s). The SDK triggers the
   * `onRecordAudioFrame` callback according to the sampling interval.
   * Call timing: Call this method before joining a channel.
   *
   * @param sampleRate The sample rate returned in the callback, which can be set as 8000, 16000,
   * 32000, 44100, or 48000 Hz.
   * @param channel The number of audio channels. You can set the value as 1 or 2.
   * - 1: Mono.
   * - 2: Stereo.
   * @param mode The use mode of the audio frame. See `RAW_AUDIO_FRAME_OP_MODE_TYPE`.
   * @param samplesPerCall The number of data samples, such as 1024 for the Media Push.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setRecordingAudioFrameParameters(int sampleRate, int channel,
                                               RAW_AUDIO_FRAME_OP_MODE_TYPE mode,
                                               int samplesPerCall) = 0;

  /**
   * @brief Sets the format of the raw audio playback data.
   *
   * @details
   * The SDK calculates the sampling interval based on the `samplesPerCall`, `sampleRate` and
   * `channel` parameters set in this method.Sample interval (sec) = `samplePerCall` /( `sampleRate` ×
   * `channel` ). Ensure that the sample interval ≥ 0.01 (s). The SDK triggers the
   * `onPlaybackAudioFrame` callback according to the sampling interval.
   * Call timing: Call this method before joining a channel.
   *
   * @param sampleRate The sample rate returned in the callback, which can be set as 8000, 16000,
   * 24000, 32000, 44100, or 48000 Hz.
   * @param channel The number of audio channels. You can set the value as 1 or 2.
   * - 1: Mono.
   * - 2: Stereo.
   * @param mode The use mode of the audio frame. See `RAW_AUDIO_FRAME_OP_MODE_TYPE`.
   * @param samplesPerCall The number of data samples, such as 1024 for the Media Push.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setPlaybackAudioFrameParameters(int sampleRate, int channel,
                                              RAW_AUDIO_FRAME_OP_MODE_TYPE mode,
                                              int samplesPerCall) = 0;

  /**
   * @brief Sets the format of the raw audio data after mixing for audio capture and playback.
   *
   * @details
   * The SDK calculates the sampling interval based on the `samplesPerCall`, `sampleRate` and
   * `channel` parameters set in this method.Sample interval (sec) = `samplePerCall` /( `sampleRate` ×
   * `channel` ). Ensure that the sample interval ≥ 0.01 (s). The SDK triggers the `onMixedAudioFrame`
   * callback according to the sampling interval.
   * Call timing: Call this method before joining a channel.
   *
   * @param sampleRate The sample rate returned in the callback, which can be set as 8000, 16000,
   * 32000, 44100, or 48000 Hz.
   * @param channel The number of audio channels. You can set the value as 1 or 2.
   * - 1: Mono.
   * - 2: Stereo.
   * @param samplesPerCall The number of data samples, such as 1024 for the Media Push.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setMixedAudioFrameParameters(int sampleRate, int channel, int samplesPerCall) = 0;

  /**
   * @brief Sets the format of the in-ear monitoring raw audio data.
   *
   * @details
   * This method is used to set the in-ear monitoring audio data format reported by the
   * `onEarMonitoringAudioFrame` callback.
   *
   * @note
   * - Before calling this method, you need to call `enableInEarMonitoring`, and set
   * `includeAudioFilters` to `EAR_MONITORING_FILTER_BUILT_IN_AUDIO_FILTERS` or
   * `EAR_MONITORING_FILTER_NOISE_SUPPRESSION`.
   * - The SDK calculates the sampling interval based on the `samplesPerCall`, `sampleRate` and
   * `channel` parameters set in this method.Sample interval (sec) = `samplePerCall` /( `sampleRate` ×
   * `channel` ). Ensure that the sample interval ≥ 0.01 (s). The SDK triggers the
   * `onEarMonitoringAudioFrame` callback according to the sampling interval.
   *
   * @param sampleRate The sample rate of the audio data reported in the `onEarMonitoringAudioFrame`
   * callback, which can be set as 8,000, 16,000, 32,000, 44,100, or 48,000 Hz.
   * @param channel The number of audio channels reported in the `onEarMonitoringAudioFrame` callback.
   * - 1: Mono.
   * - 2: Stereo.
   * @param mode The use mode of the audio frame. See `RAW_AUDIO_FRAME_OP_MODE_TYPE`.
   * @param samplesPerCall The number of data samples reported in the `onEarMonitoringAudioFrame`
   * callback, such as 1,024 for the Media Push.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setEarMonitoringAudioFrameParameters(int sampleRate, int channel,
                                                   RAW_AUDIO_FRAME_OP_MODE_TYPE mode,
                                                   int samplesPerCall) = 0;

  /**
   * @brief Sets the format of the raw audio playback data before mixing.
   *
   * @details
   * The SDK triggers the `onPlaybackAudioFrameBeforeMixing` callback according to the sampling
   * interval.
   * Call timing: Call this method before joining a channel.
   *
   * @param sampleRate The sample rate returned in the callback, which can be set as 8000, 16000,
   * 32000, 44100, or 48000 Hz.
   * @param channel The number of audio channels. You can set the value as 1 or 2.
   * - 1: Mono.
   * - 2: Stereo.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setPlaybackAudioFrameBeforeMixingParameters(int sampleRate, int channel) = 0;

  /**
   * @brief Sets the format of audio data in the `onPlaybackAudioFrameBeforeMixing` callback.
   *
   * @details
   * Used to set the sample rate, number of channels, and number of samples per callback for the audio
   * data returned in the `onPlaybackAudioFrameBeforeMixing` callback.
   *
   * @param sampleRate Set the sample rate returned in the `onPlaybackAudioFrameBeforeMixing`
   * callback. It can be set as the following values: 8000、16000、32000、44100 or 48000.
   * @param channel Set the number of channels for the audio data returned in the
   * `onPlaybackAudioFrameBeforeMixing` callback. It can be set to:
   * - 1: Mono.
   * - 2: Stereo.
   * @param samplesPerCall Set the sample rate of the audio data returned in the
   * `onPlaybackAudioFrameBeforeMixing` callback. In the RTMP streaming scenario, it is recommended to
   * set it to 1024.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setPlaybackAudioFrameBeforeMixingParameters(int sampleRate, int channel, int samplesPerCall) = 0;

  /**
   * @brief Turns on audio spectrum monitoring.
   *
   * @details
   * If you want to obtain the audio spectrum data of local or remote users, you can register the
   * audio spectrum observer and enable audio spectrum monitoring.
   *
   * @note You can call this method either before or after joining a channel.
   *
   * @param intervalInMS The interval (in milliseconds) at which the SDK triggers the
   * `onLocalAudioSpectrum` and `onRemoteAudioSpectrum` callbacks. The default value is 100. Do not
   * set this parameter to a value less than 10, otherwise calling this method would fail.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   *   - -2: Invalid parameters.
   */
  virtual int enableAudioSpectrumMonitor(int intervalInMS = 100) = 0;
  /**
   * @brief Disables audio spectrum monitoring.
   *
   * @details
   * After calling `enableAudioSpectrumMonitor`, if you want to disable audio spectrum monitoring, you
   * can call this method.
   *
   * @note You can call this method either before or after joining a channel.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int disableAudioSpectrumMonitor() = 0;

  /**
   * @brief Registers an audio spectrum observer.
   *
   * @details
   * After successfully registering the audio spectrum observer and calling
   *  `enableAudioSpectrumMonitor` to enable the audio spectrum monitoring, the SDK reports the
   * callback that you implement in the `IAudioSpectrumObserver` class according to the time interval
   * you set.
   *
   * @note You can call this method either before or after joining a channel.
   *
   * @param observer The audio spectrum observer. See `IAudioSpectrumObserver`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int registerAudioSpectrumObserver(agora::media::IAudioSpectrumObserver * observer) = 0;
  /**
   * @brief Unregisters the audio spectrum observer.
   *
   * @details
   * After calling `registerAudioSpectrumObserver`, if you want to disable audio spectrum monitoring,
   * you can call this method.
   *
   * @note You can call this method either before or after joining a channel.
   *
   * @param observer The audio spectrum observer. See `IAudioSpectrumObserver`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int unregisterAudioSpectrumObserver(agora::media::IAudioSpectrumObserver * observer) = 0;

  /**
   * @brief Adjusts the capturing signal volume.
   *
   * @details
   * If you only need to mute the audio signal, Agora recommends that you use `muteRecordingSignal`
   * instead.
   * Call timing: This method can be called either before or after joining the channel.
   *
   * @param volume The volume of the user. The value range is [0,400].
   * - 0: Mute.
   * - 100: (Default) The original volume.
   * - 400: Four times the original volume (amplifying the audio signals by four times).
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int adjustRecordingSignalVolume(int volume) = 0;

  /**
   * @brief Whether to mute the recording signal.
   *
   * @details
   * If you have already called `adjustRecordingSignalVolume` to adjust the recording signal volume,
   * when you call this method and set it to `true`, the SDK behaves as follows:1. Records the
   * adjusted volume.
   * 2. Mutes the recording signal.
   * When you call this method again and set it to `false`, the recording signal volume will be
   * restored to the volume recorded by the SDK before muting.
   * Call timing: This method can be called either before or after joining the channel.
   *
   * @param mute - `true`: Mute the recording signal.
   * - `false`: (Default) Do not mute the recording signal.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int muteRecordingSignal(bool mute) = 0;

  /**
   * @brief Adjusts the playback signal volume of all remote users.
   *
   * @details
   * This method is used to adjust the signal volume of all remote users mixed and played locally. If
   * you need to adjust the signal volume of a specified remote user played locally, it is recommended
   * that you call `adjustUserPlaybackSignalVolume` instead.
   * Call timing: This method can be called either before or after joining the channel.
   *
   * @param volume The volume of the user. The value range is [0,400].
   * - 0: Mute.
   * - 100: (Default) The original volume.
   * - 400: Four times the original volume (amplifying the audio signals by four times).
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int adjustPlaybackSignalVolume(int volume) = 0;

  /**
   * @brief Adjusts the playback signal volume of a specified remote user.
   *
   * @details
   * You can call this method to adjust the playback volume of a specified remote user. To adjust the
   * playback volume of different remote users, call the method as many times, once for each remote
   * user.
   * Call timing: Call this method after joining a channel.
   *
   * @param uid The user ID of the remote user.
   * @param volume The volume of the user. The value range is [0,400].
   * - 0: Mute.
   * - 100: (Default) The original volume.
   * - 400: Four times the original volume (amplifying the audio signals by four times).
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int adjustUserPlaybackSignalVolume(uid_t uid, int volume) = 0;

  /**
   * @brief Sets the fallback option for the subscribed video stream based on the network conditions.
   *
   * @details
   * An unstable network affects the audio and video quality in a video call or interactive live video
   * streaming. If `option` is set as `STREAM_FALLBACK_OPTION_VIDEO_STREAM_LOW` or
   * `STREAM_FALLBACK_OPTION_AUDIO_ONLY`, the SDK automatically switches the video from a high-quality
   * stream to a low-quality stream or disables the video when the downlink network conditions cannot
   * support both audio and video to guarantee the quality of the audio. Meanwhile, the SDK
   * continuously monitors network quality and resumes subscribing to audio and video streams when the
   * network quality improves.
   * When the subscribed video stream falls back to an audio-only stream, or recovers from an
   * audio-only stream to an audio-video stream, the SDK triggers the
   * `onRemoteSubscribeFallbackToAudioOnly` callback.
   *
   * @param option Fallback options for the subscribed stream. See `STREAM_FALLBACK_OPTIONS`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setRemoteSubscribeFallbackOption(STREAM_FALLBACK_OPTIONS option) = 0;

  /** Sets the high priority user list and their fallback level in weak network condition.
   * @note
   * - This method can be called before and after joining a channel.
   * - If a subscriber is set to high priority, this stream only fallback to lower stream after all normal priority users fallback to their fallback level on weak network condition if needed.
   *
   * @param uidList The high priority user list.
   * @param uidNum The size of uidList.
   * @param option The fallback level of high priority users.
   *
   * @return int
   * - 0 : Success.
   * - <0 : Failure.
   */
  virtual int setHighPriorityUserList(uid_t* uidList, int uidNum, STREAM_FALLBACK_OPTIONS option) = 0;

  /**
   * Enable/Disable an extension.
   * By calling this function, you can dynamically enable/disable the extension without changing the pipeline.
   * For example, enabling/disabling Extension_A means the data will be adapted/bypassed by Extension_A.
   *
   * NOTE: For compatibility reasons, if you haven't call registerExtension,
   * enableExtension will automatically register the specified extension.
   * We suggest you call registerExtension explicitly.
   *
   * @param provider The name of the extension provider, e.g. agora.io.
   * @param extension The name of the extension, e.g. agora.beauty.
   * @param extensionInfo The information for extension.
   * @param enable Whether to enable the extension:
   * - true: (Default) Enable the extension.
   * - false: Disable the extension.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int enableExtension(const char* provider, const char* extension, const ExtensionInfo& extensionInfo, bool enable = true) = 0;

  /**
   * Sets the properties of an extension.
   *
   * @param provider The name of the extension provider, e.g. agora.io.
   * @param extension The name of the extension, e.g. agora.beauty.
   * @param extensionInfo The information for extension.
   * @param key The key of the extension.
   * @param value The JSON formatted value of the extension key.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setExtensionProperty(const char* provider, const char* extension, const ExtensionInfo& extensionInfo, const char* key, const char* value) = 0;

  /**
   * Gets the properties of an extension.
   *
   * @param provider The name of the extension provider, e.g. agora.io.
   * @param extension The name of the extension, e.g. agora.beauty.
   * @param extensionInfo The information for extension.
   * @param key The key of the extension.
   * @param value The value of the extension key.
   * @param buf_len Maximum length of the JSON string indicating the extension property.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int getExtensionProperty(const char* provider, const char* extension, const ExtensionInfo& extensionInfo, const char* key, char* value, int buf_len)  = 0;

  /**
   * @brief Enables loopback audio capturing.
   *
   * @details
   * If you enable loopback audio capturing, the output of the sound card is mixed into the audio
   * stream sent to the other end.
   *
   * @note
   * - This method applies to the macOS and Windows only.
   * - You can call this method either before or after joining a channel.
   * - If you call the `disableAudio` method to disable the audio module, audio capturing will be
   * disabled as well. If you need to enable audio capturing, call the `enableAudio` method to enable
   * the audio module and then call the `enableLoopbackRecording` method.
   *
   * @param enabled Sets whether to enable loopback audio capturing.
   * - `true`: Enable sound card capturing. You can find the name of the virtual sound card in your
   * system's**Audio Devices > Output**.
   * - `false`: Disable sound card capturing. The name of the virtual sound card will not be shown in
   * your system's **Audio Devices > Output**.
   * @param deviceName - macOS: The device name of the virtual sound card. The default value is set to
   * NULL, which means using AgoraALD for loopback audio capturing.
   * - Windows: The device name of the sound card. The default is set to NULL, which means the SDK
   * uses the sound card of your device for loopback audio capturing.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int enableLoopbackRecording(bool enabled, const char* deviceName = NULL) = 0;


  /**
   * @brief Adjusts the volume of the signal captured by the sound card.
   *
   * @details
   * After calling `enableLoopbackRecording` to enable loopback audio capturing, you can call this
   * method to adjust the volume of the signal captured by the sound card.
   *
   * @param volume Audio mixing volume. The value ranges between 0 and 100. The default value is 100,
   * which means the original volume.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int adjustLoopbackSignalVolume(int volume) = 0;

  /** Retrieves the audio volume for recording loopback.
  @note Call this method when you are in a channel.
  @return
  - &ge; 0: The audio volume for loopback, if this method call succeeds. The value range is [0,100].
  - < 0: Failure.
  */
  virtual int getLoopbackRecordingVolume() = 0;

  /**
   * @brief Enables in-ear monitoring.
   *
   * @details
   * This method enables or disables in-ear monitoring.
   * Call timing: This method can be called either before or after joining the channel.
   *
   * @note Users must use earphones (wired or Bluetooth) to hear the in-ear monitoring effect.
   *
   * @param enabled Enables or disables in-ear monitoring.
   * - `true`: Enables in-ear monitoring.
   * - `false`: (Default) Disables in-ear monitoring.
   * @param includeAudioFilters The audio filter types of in-ear monitoring. See
   * `EAR_MONITORING_FILTER_TYPE`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   *   - - 8: Make sure the current audio routing is Bluetooth or headset.
   */
  virtual int enableInEarMonitoring(bool enabled, int includeAudioFilters) = 0;

  /**
   * @brief Sets the volume of the in-ear monitor.
   *
   * @details
   * Call timing: This method can be called either before or after joining the channel.
   *
   * @param volume The volume of the user. The value range is [0,400].
   * - 0: Mute.
   * - 100: (Default) The original volume.
   * - 400: Four times the original volume (amplifying the audio signals by four times).
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   *   - -2: Invalid parameter settings, such as in-ear monitoring volume exceeding the valid range (<
   * 0 or > 400).
   */
  virtual int setInEarMonitoringVolume(int volume) = 0;

#if defined(_WIN32) || defined(__linux__) || defined(__ANDROID__)
  /**
   * @brief Loads an extension.
   *
   * @details
   * This method is used to add extensions external to the SDK (such as those from Extensions
   * Marketplace and SDK extensions) to the SDK.
   * Call timing: Make sure the `IRtcEngine` is initialized before you call this method.
   *
   * @note
   * If you want to load multiple extensions, you need to call this method multiple times.
   * (For Windows and Android only)
   * (For Windows and Android only)
   *
   * @param path The extension library path and name. For example:
   * `/library/libagora_segmentation_extension.dll`.
   * @param unload_after_use Whether to uninstall the current extension when you no longer using it:
   * - `true`: Uninstall the extension when the `IRtcEngine` is destroyed.
   * - `false`: (Rcommended) Do not uninstall the extension until the process terminates.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int loadExtensionProvider(const char* path, bool unload_after_use = false) = 0;
#endif

  /**
   * @brief Sets the properties of the extension provider.
   *
   * @details
   * You can call this method to set the attributes of the extension provider and initialize the
   * relevant parameters according to the type of the provider.
   * Call timing: Call this method before `enableExtension` and after `registerExtension`.
   *
   * @note If you want to set the properties of the extension provider for multiple extensions, you
   * need to call this method multiple times.
   *
   * @param provider The name of the extension provider.
   * @param key The key of the extension.
   * @param value The value of the extension key.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setExtensionProviderProperty(const char* provider, const char* key, const char* value) = 0;

  /**
   * @brief Registers an extension.
   *
   * @details
   * For extensions external to the SDK (such as those from Extensions Marketplace and SDK
   * Extensions), you need to load them before calling this method. Extensions internal to the SDK
   * (those included in the full SDK package) are automatically loaded and registered after the
   * initialization of `IRtcEngine`.
   * Call timing: - Agora recommends you call this method after the initialization of `IRtcEngine` and
   * before joining a channel.
   * - For video extensions (such as the image enhancement extension), you need to call this method
   * after enabling the video module by calling `enableVideo` or `enableLocalVideo`.
   * - Before calling this method, you need to call `loadExtensionProvider` to load the extension
   * first.
   *
   * @note
   * - If you want to register multiple extensions, you need to call this method multiple times.
   * - The data processing order of different extensions in the SDK is determined by the order in
   * which the extensions are registered. That is, the extension that is registered first will process
   * the data first.
   *
   * @param provider The name of the extension provider.
   * @param extension The name of the extension.
   * @param type Source type of the extension. See `MEDIA_SOURCE_TYPE`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   *   - -3: The extension library is not loaded. Agora recommends that you check the storage location
   * or the name of the dynamic library.
   */
  virtual int registerExtension(const char* provider, const char* extension, agora::media::MEDIA_SOURCE_TYPE type = agora::media::UNKNOWN_MEDIA_SOURCE) = 0;

  /**
   * @brief Enables or disables extensions.
   *
   * @details
   * Call timing: Agora recommends that you call this method after joining a channel.
   * Related callbacks: When this method is successfully called within the channel, it triggers
   * `onExtensionStartedWithContext` or `onExtensionStoppedWithContext`.
   *
   * @note
   * - If you want to enable multiple extensions, you need to call this method multiple times.
   * - After a successful call of this method, you cannot load other extensions.
   *
   * @param provider The name of the extension provider.
   * @param extension The name of the extension.
   * @param enable Whether to enable the extension:
   * - `true`: Enable the extension.
   * - `false`: Disable the extension.
   * @param type Source type of the extension. See `MEDIA_SOURCE_TYPE`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   *   - -3: The extension library is not loaded. Agora recommends that you check the storage location
   * or the name of the dynamic library.
   */
  virtual int enableExtension(const char* provider, const char* extension, bool enable=true, agora::media::MEDIA_SOURCE_TYPE type = agora::media::UNKNOWN_MEDIA_SOURCE) = 0;

  /**
   * @brief Sets the properties of the extension.
   *
   * @details
   * After enabling the extension, you can call this method to set the properties of the extension.
   * Call timing: Call this mehtod after calling `enableExtension`.
   * Related callbacks: After calling this method, it may trigger the `onExtensionEventWithContext`
   * callback, and the specific triggering logic is related to the extension itself.
   *
   * @note If you want to set properties for multiple extensions, you need to call this method
   * multiple times.
   *
   * @param provider The name of the extension provider.
   * @param extension The name of the extension.
   * @param key The key of the extension.
   * @param value The value of the extension key.
   * @param type Source type of the extension. See `MEDIA_SOURCE_TYPE`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setExtensionProperty(
      const char* provider, const char* extension,
      const char* key, const char* value, agora::media::MEDIA_SOURCE_TYPE type = agora::media::UNKNOWN_MEDIA_SOURCE) = 0;

  /**
   * @brief Gets detailed information on the extensions.
   *
   * @details
   * Call timing: This method can be called either before or after joining the channel.
   *
   * @param provider An output parameter. The name of the extension provider.
   * @param extension An output parameter. The name of the extension.
   * @param key An output parameter. The key of the extension.
   * @param value An output parameter. The value of the extension key.
   * @param type Source type of the extension. See `MEDIA_SOURCE_TYPE`.
   * @param buf_len Maximum length of the JSON string indicating the extension property. The maximum
   * value is 512 bytes.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int getExtensionProperty(
      const char* provider, const char* extension,
      const char* key, char* value, int buf_len, agora::media::MEDIA_SOURCE_TYPE type = agora::media::UNKNOWN_MEDIA_SOURCE) = 0;

  /**
   * @brief Sets the camera capture configuration.
   *
   * @details
   * Call timing: Call this method before enabling local camera capture, such as before calling
   * `startPreview(VIDEO_SOURCE_TYPE sourceType)` and `joinChannel(const char* token, const char*
   * channelId, uid_t uid, const ChannelMediaOptions& options)`.
   *
   * @note
   * To adjust the camera focal length configuration, It is recommended to call
   * `queryCameraFocalLengthCapability` first to check the device's focal length capabilities, and
   * then configure based on the query results.
   * Due to limitations on some Android devices, even if you set the focal length type according to
   * the results returned in `queryCameraFocalLengthCapability`, the settings may not take effect.
   *
   * @param config The camera capture configuration. See `CameraCapturerConfiguration`.Attention: In
   * this method, you do not need to set the `deviceId` parameter.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setCameraCapturerConfiguration(const CameraCapturerConfiguration& config) = 0;

  /**
   * @brief Creates a custom video track.
   *
   * @details
   * To publish a custom video source, see the following steps:1. Call this method to create a video
   * track and get the video track ID.
   * 2. Call `joinChannel(const char* token, const char* channelId, uid_t uid, const
   * ChannelMediaOptions& options)` to join the channel. In `ChannelMediaOptions`, set
   * `customVideoTrackId` to the video track ID that you want to publish, and set
   * `publishCustomVideoTrack` to `true`.
   * 3. Call `pushVideoFrame` and specify `videoTrackId` as the video track ID set in step 2. You can
   * then publish the corresponding custom video source in the channel.
   *
   * @return
   * - If the method call is successful, the video track ID is returned as the unique identifier of
   * the video track.
   * - If the method call fails, 0xffffffff is returned.
   */
  virtual video_track_id_t createCustomVideoTrack() = 0;

  /**
   * Get an custom encoded video track id created by internal,which could used to publish or preview
   *
   * @return
   * - > 0: the useable video track id.
   * - < 0: Failure.
   */
  virtual video_track_id_t createCustomEncodedVideoTrack(const SenderOptions& sender_option) = 0;

  /**
   * @brief Destroys the specified video track.
   *
   * @param video_track_id The video track ID returned by calling the `createCustomVideoTrack` method.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int destroyCustomVideoTrack(video_track_id_t video_track_id) = 0;

  /**
   * destroy a created custom encoded video track id
   *
   * @param video_track_id The video track id which was created by createCustomEncodedVideoTrack
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int destroyCustomEncodedVideoTrack(video_track_id_t video_track_id) = 0;

#if defined(__ANDROID__) || (defined(__APPLE__) && TARGET_OS_IOS) || defined(__OHOS__)
  /**
   * @brief Switches between front and rear cameras.
   *
   * @details
   * You can call this method to dynamically switch cameras based on the actual camera availability
   * during the app's runtime, without having to restart the video stream or reconfigure the video
   * source.
   * Call timing: This method must be called after the camera is successfully enabled, that is, after
   * the SDK triggers the `onLocalVideoStateChanged` callback and returns the local video state as
   * `LOCAL_VIDEO_STREAM_STATE_CAPTURING` (1).
   *
   * @note
   * - This method only switches the camera for the video stream captured by the first camera, that is,
   * the video source set to `VIDEO_SOURCE_CAMERA` (0) when calling `startCameraCapture`.
   * - This method is for Android and iOS only.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int switchCamera() = 0;

  /**
   * @brief Checks whether the device supports camera zoom.
   *
   * @details
   * Call timing: This method must be called after the SDK triggers the `onLocalVideoStateChanged`
   * callback and returns the local video state as `LOCAL_VIDEO_STREAM_STATE_CAPTURING` (1).
   *
   * @note This method is for Android and iOS only.
   *
   * @return
   * - `true`: The device supports camera zoom.
   * - `false`: The device does not support camera zoom.
   */
  virtual bool isCameraZoomSupported() = 0;

  /**
   * @brief Checks whether the device camera supports face detection.
   *
   * @note
   * - This method is for Android and iOS only.
   * - This method must be called after the SDK triggers the `onLocalVideoStateChanged` callback and
   * returns the local video state as `LOCAL_VIDEO_STREAM_STATE_CAPTURING` (1).
   *
   * @return
   * - `true`: The device camera supports face detection.
   * - `false`: The device camera does not support face detection.
   */
  virtual bool isCameraFaceDetectSupported() = 0;

  /**
   * @brief Checks whether the device supports camera flash.
   *
   * @note
   * - This method is for Android and iOS only.
   * - This method must be called after the SDK triggers the `onLocalVideoStateChanged` callback and
   * returns the local video state as `LOCAL_VIDEO_STREAM_STATE_CAPTURING` (1).
   * - The app enables the front camera by default. If your front camera does not support flash, this
   * method returns false. If you want to check whether the rear camera supports the flash function,
   * call `switchCamera` before this method.
   * - On iPads with system version 15, even if `isCameraTorchSupported` returns `true`, you might
   * fail to successfully enable the flash by calling `setCameraTorchOn` due to system issues.
   *
   * @return
   * - `true`: The device supports camera flash.
   * - `false`: The device does not support camera flash.
   */
  virtual bool isCameraTorchSupported() = 0;

  /**
   * @brief Check whether the device supports the manual focus function.
   *
   * @note
   * - This method is for Android and iOS only.
   * - This method must be called after the SDK triggers the `onLocalVideoStateChanged` callback and
   * returns the local video state as `LOCAL_VIDEO_STREAM_STATE_CAPTURING` (1).
   *
   * @return
   * - `true`: The device supports the manual focus function.
   * - `false`: The device does not support the manual focus function.
   */
  virtual bool isCameraFocusSupported() = 0;

  /**
   * @brief Checks whether the device supports the face auto-focus function.
   *
   * @note
   * - This method is for Android and iOS only.
   * - This method must be called after the SDK triggers the `onLocalVideoStateChanged` callback and
   * returns the local video state as `LOCAL_VIDEO_STREAM_STATE_CAPTURING` (1).
   *
   * @return
   * - `true`: The device supports the face auto-focus function.
   * - `false`: The device does not support the face auto-focus function.
   */
  virtual bool isCameraAutoFocusFaceModeSupported() = 0;

  /**
   * @brief Sets the camera zoom factor.
   *
   * @details
   * For iOS devices equipped with multi-lens rear cameras, such as those featuring dual-camera
   * (wide-angle and ultra-wide-angle) or triple-camera (wide-angle, ultra-wide-angle, and telephoto),
   * you can call `setCameraCapturerConfiguration` first to set the `cameraFocalLengthType` as
   * `CAMERA_FOCAL_LENGTH_DEFAULT` (0) (standard lens). Then, adjust the camera zoom factor to a value
   * less than 1.0. This configuration allows you to capture video with an ultra-wide-angle
   * perspective.
   *
   * @note
   * - This method is for Android and iOS only.
   * - You must call this method after `enableVideo`. The setting result will take effect after the
   * camera is successfully turned on, that is, after the SDK triggers the `onLocalVideoStateChanged`
   * callback and returns the local video state as `LOCAL_VIDEO_STREAM_STATE_CAPTURING` (1).
   *
   * @param factor Camera zoom factor. For devices that do not support ultra-wide-angle, the value
   * ranges from 1.0 to the maximum zoom factor; for devices that support ultra-wide-angle, the value
   * ranges from 0.5 to the maximum zoom factor. You can get the maximum zoom factor supported by the
   * device by calling the `getCameraMaxZoomFactor` method.
   *
   * @return
   * - The camera zoom `factor` value, if successful.
   * - < 0: if the method if failed.
   */
  virtual int setCameraZoomFactor(float factor) = 0;

  /**
   * @brief Enables or disables face detection for the local user.
   *
   * @details
   * Call timing: This method needs to be called after the camera is started (for example, by calling
   * `startPreview(VIDEO_SOURCE_TYPE sourceType)` or `enableVideo` ).
   * Related callbacks: Once face detection is enabled, the SDK triggers the `onFacePositionChanged`
   * callback to report the face information of the local user, which includes the following:
   * - The width and height of the local video.
   * - The position of the human face in the local view.
   * - The distance between the human face and the screen.
   *
   * @note This method is for Android and iOS only.
   *
   * @param enabled Whether to enable face detection for the local user:
   * - `true`: Enable face detection.
   * - `false`: (Default) Disable face detection.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int enableFaceDetection(bool enabled) = 0;

  /**
   * @brief Gets the maximum zoom ratio supported by the camera.
   *
   * @note
   * - This method is for Android and iOS only.
   * - This method must be called after the SDK triggers the `onLocalVideoStateChanged` callback and
   * returns the local video state as `LOCAL_VIDEO_STREAM_STATE_CAPTURING` (1).
   *
   * @return
   * The maximum zoom ratio supported by the camera.
   */
  virtual float getCameraMaxZoomFactor() = 0;

  /**
   * @brief Sets the camera manual focus position.
   *
   * @note
   * - This method is for Android and iOS only.
   * - You must call this method after `enableVideo`. The setting result will take effect after the
   * camera is successfully turned on, that is, after the SDK triggers the `onLocalVideoStateChanged`
   * callback and returns the local video state as `LOCAL_VIDEO_STREAM_STATE_CAPTURING` (1).
   * - After a successful method call, the SDK triggers the `onCameraFocusAreaChanged` callback.
   *
   * @param positionX The horizontal coordinate of the touchpoint in the view.
   * @param positionY The vertical coordinate of the touchpoint in the view.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setCameraFocusPositionInPreview(float positionX, float positionY) = 0;

  /**
   * @brief Enables the camera flash.
   *
   * @note
   * - This method is for Android and iOS only.
   * - You must call this method after `enableVideo`. The setting result will take effect after the
   * camera is successfully turned on, that is, after the SDK triggers the `onLocalVideoStateChanged`
   * callback and returns the local video state as `LOCAL_VIDEO_STREAM_STATE_CAPTURING` (1).
   *
   * @param isOn Whether to turn on the camera flash:
   * - `true`: Turn on the flash.
   * - `false`: (Default) Turn off the flash.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setCameraTorchOn(bool isOn) = 0;

  /**
   * @brief Enables the camera auto-face focus function.
   *
   * @details
   * By default, the SDK disables face autofocus on Android and enables face autofocus on iOS. To set
   * face autofocus, call this method.
   * Call timing: This method must be called after the SDK triggers the `onLocalVideoStateChanged`
   * callback and returns the local video state as `LOCAL_VIDEO_STREAM_STATE_CAPTURING` (1).
   *
   * @note This method is for Android and iOS only.
   *
   * @param enabled Whether to enable face autofocus:
   * - `true`: Enable the camera auto-face focus function.
   * - `false`: Disable face autofocus.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setCameraAutoFocusFaceModeEnabled(bool enabled) = 0;

  /**
   * @brief Checks whether the device supports manual exposure.
   *
   * @since v2.3.2.
   *
   * @note
   * - This method is for Android and iOS only.
   * - This method must be called after the SDK triggers the `onLocalVideoStateChanged` callback and
   * returns the local video state as `LOCAL_VIDEO_STREAM_STATE_CAPTURING` (1).
   *
   * @return
   * - `true`: The device supports manual exposure.
   * - `false`: The device does not support manual exposure.
   */
  virtual bool isCameraExposurePositionSupported() = 0;

  /**
   * @brief Sets the camera exposure position.
   *
   * @since v2.3.2.
   *
   * @note
   * - This method is for Android and iOS only.
   * - You must call this method after `enableVideo`. The setting result will take effect after the
   * camera is successfully turned on, that is, after the SDK triggers the `onLocalVideoStateChanged`
   * callback and returns the local video state as `LOCAL_VIDEO_STREAM_STATE_CAPTURING` (1).
   * - After a successful method call, the SDK triggers the `onCameraExposureAreaChanged` callback.
   *
   * @param positionXinView The horizontal coordinate of the touchpoint in the view.
   * @param positionYinView The vertical coordinate of the touchpoint in the view.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setCameraExposurePosition(float positionXinView, float positionYinView) = 0;

  /**
   * @brief Queries whether the current camera supports adjusting exposure value.
   *
   * @since v4.2.2
   *
   * @note
   * - This method is for Android and iOS only.
   * - This method must be called after the SDK triggers the `onLocalVideoStateChanged` callback and
   * returns the local video state as `LOCAL_VIDEO_STREAM_STATE_CAPTURING` (1).
   * - Before calling `setCameraExposureFactor`, Agora recoomends that you call this method to query
   * whether the current camera supports adjusting the exposure value.
   * - By calling this method, you adjust the exposure value of the currently active camera, that is,
   * the camera specified when calling `setCameraCapturerConfiguration`.
   *
   * @return
   * - `true`: Success.
   * - `false`: Failure.
   */
  virtual bool isCameraExposureSupported() = 0;

  /**
   * @brief Sets the camera exposure value.
   *
   * @since v4.2.2
   *
   * @details
   * Insufficient or excessive lighting in the shooting environment can affect the image quality of
   * video capture. To achieve optimal video quality, you can use this method to adjust the camera's
   * exposure value.
   *
   * @note
   * - This method is for Android and iOS only.
   * - You must call this method after `enableVideo`. The setting result will take effect after the
   * camera is successfully turned on, that is, after the SDK triggers the `onLocalVideoStateChanged`
   * callback and returns the local video state as `LOCAL_VIDEO_STREAM_STATE_CAPTURING` (1).
   * - Before calling this method, Agora recommends calling `isCameraExposureSupported` to check
   * whether the current camera supports adjusting the exposure value.
   * - By calling this method, you adjust the exposure value of the currently active camera, that is,
   * the camera specified when calling `setCameraCapturerConfiguration`.
   *
   * @param factor The camera exposure value. The default value is 0, which means using the default
   * exposure of the camera. The larger the value, the greater the exposure. When the video image is
   * overexposed, you can reduce the exposure value; when the video image is underexposed and the dark
   * details are lost, you can increase the exposure value. If the exposure value you specified is
   * beyond the range supported by the device, the SDK will automatically adjust it to the actual
   * supported range of the device.
   * On Android, the value range is [-20.0, 20.0]. On iOS, the value range is [-8.0, 8.0].
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setCameraExposureFactor(float factor) = 0;

#if defined(__APPLE__)
  /**
   * @brief Checks whether the device supports auto exposure.
   *
   * @note
   * - This method applies to iOS only.
   * - This method must be called after the SDK triggers the `onLocalVideoStateChanged` callback and
   * returns the local video state as `LOCAL_VIDEO_STREAM_STATE_CAPTURING` (1).
   *
   * @return
   * - `true`: The device supports auto exposure.
   * - `false`: The device does not support auto exposure.
   */
  virtual bool isCameraAutoExposureFaceModeSupported() = 0;


  /**
   * @brief Sets whether to enable auto exposure.
   *
   * @note
   * - This method applies to iOS only.
   * - You must call this method after `enableVideo`. The setting result will take effect after the
   * camera is successfully turned on, that is, after the SDK triggers the `onLocalVideoStateChanged`
   * callback and returns the local video state as `LOCAL_VIDEO_STREAM_STATE_CAPTURING` (1).
   *
   * @param enabled Whether to enable auto exposure:
   * - `true`: Enable auto exposure.
   * - `false`: Disable auto exposure.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setCameraAutoExposureFaceModeEnabled(bool enabled) = 0;

  /**
   * @brief Sets the camera stabilization mode.
   *
   * @details
   * The camera stabilization mode is off by default. You need to call this method to turn it on and
   * set the appropriate stabilization mode.
   * Applicable scenarios: When shooting on the move, in low light conditions, or with mobile devices,
   * you can set the camera stabilization mode to reduce the impact of camera shake and get a more
   * stable, clear picture.
   * Call timing: This method must be called after the camera is successfully enabled, that is, after
   * the SDK triggers the `onLocalVideoStateChanged` callback and returns the local video state as
   * `LOCAL_VIDEO_STREAM_STATE_CAPTURING` (1).
   *
   * @note
   * - Camera stabilization only works for scenarios with a video resolution greater than 1280 x 720.
   * - After enabling camera stabilization, the higher the camera stabilization level, the smaller the
   * camera's field of view and the greater the camera's latency. To improve user experience, it is
   * recommended that you set the `mode` parameter to `CAMERA_STABILIZATION_MODE_LEVEL_1`.
   * This method applies to iOS only.
   *
   * @param mode Camera stabilization mode. See `CAMERA_STABILIZATION_MODE`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setCameraStabilizationMode(CAMERA_STABILIZATION_MODE mode) = 0;
#endif

  /**
   * @brief Sets the default audio playback route.
   *
   * @details
   * Most mobile phones have two audio routes: an earpiece at the top, and a speakerphone at the
   * bottom. The earpiece plays at a lower volume, and the speakerphone at a higher volume. When
   * setting the default audio route, you determine whether audio playback comes through the earpiece
   * or speakerphone when no external audio device is connected.
   * In different scenarios, the default audio routing of the system is also different. See the
   * following:
   * - Voice call: Earpiece.
   * - Audio broadcast: Speakerphone.
   * - Video call: Speakerphone.
   * - Video broadcast: Speakerphone.
   * You can call this method to change the default audio route.
   * Call timing: Call this method before joining a channel. If you need to change the audio route
   * after joining a channel, call `setEnableSpeakerphone`.
   * Related callbacks: After successfully calling this method, the SDK triggers the
   * `onAudioRoutingChanged` callback to report the current audio route.
   *
   * @note This method is for Android and iOS only.
   *
   * @param defaultToSpeaker Whether to set the speakerphone as the default audio route:
   * - `true`: Set the speakerphone as the default audio route.
   * - `false`: Set the earpiece as the default audio route.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setDefaultAudioRouteToSpeakerphone(bool defaultToSpeaker) = 0;

  /**
   * @brief Enables/Disables the audio route to the speakerphone.
   *
   * @details
   * Applicable scenarios: If the default audio route of the SDK or the setting in
   * `setDefaultAudioRouteToSpeakerphone` cannot meet your requirements, you can call this method to
   * switch the current audio route.
   * Call timing: Call this method after joining a channel.
   * Related callbacks: After successfully calling this method, the SDK triggers the
   * `onAudioRoutingChanged` callback to report the current audio route.
   *
   * @note
   * - This method only sets the audio route in the current channel and does not influence the default
   * audio route. If the user leaves the current channel and joins another channel, the default audio
   * route is used.
   * - If the user uses an external audio playback device such as a Bluetooth or wired headset, this
   * method does not take effect, and the SDK plays audio through the external device. When the user
   * uses multiple external devices, the SDK plays audio through the last connected device.
   * This method is for Android and iOS only.
   *
   * @param speakerOn Sets whether to enable the speakerphone or earpiece:
   * - `true`: Enable device state monitoring. The audio route is the speakerphone.
   * - `false`: Disable device state monitoring. The audio route is the earpiece.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setEnableSpeakerphone(bool speakerOn) = 0;

  /**
   * @brief Checks whether the speakerphone is enabled.
   *
   * @details
   * Call timing: You can call this method either before or after joining a channel.
   *
   * @note This method is for Android and iOS only.
   *
   * @return
   * - `true`: The speakerphone is enabled, and the audio plays from the speakerphone.
   * - `false`: The speakerphone is not enabled, and the audio plays from devices other than the
   * speakerphone. For example, the headset or earpiece.
   */
  virtual bool isSpeakerphoneEnabled() = 0;

  /**
   * @brief Selects the audio playback route in communication audio mode.
   *
   * @details
   * This method is used to switch the audio route from Bluetooth headphones to earpiece, wired
   * headphones or speakers in communication audio mode ( `MODE_IN_COMMUNICATION` ).
   * Call timing: This method can be called either before or after joining the channel.
   * Related callbacks: After successfully calling this method, the SDK triggers the
   * `onAudioRoutingChanged` callback to report the current audio route.
   *
   * @note
   * Using this method and the `setEnableSpeakerphone` method at the same time may cause conflicts.
   * Agora recommends that you use the `setRouteInCommunicationMode` method alone.
   * This method is for Android only.
   *
   * @param route The audio playback route you want to use:
   * - -1: The default audio route.
   * - 0: Headphones with microphone.
   * - 1: Handset.
   * - 2: Headphones without microphone.
   * - 3: Device's built-in speaker.
   * - 4: (Not supported yet) External speakers.
   * - 5: Bluetooth headphones.
   * - 6: USB device.
   *
   * @return
   * Without practical meaning.
   */
  virtual int setRouteInCommunicationMode(int route) = 0;
#endif  // __ANDROID__ || (__APPLE__ && TARGET_OS_IOS) || __OHOS__

#if defined(__APPLE__)
  /**
   * @brief Checks if the camera supports portrait center stage.
   *
   * @details
   * Before calling `enableCameraCenterStage` to enable portrait center stage, it is recommended to
   * call this method to check if the current device supports the feature.
   * Call timing: This method must be called after the camera is successfully enabled, that is, after
   * the SDK triggers the `onLocalVideoStateChanged` callback and returns the local video state as
   * `LOCAL_VIDEO_STREAM_STATE_CAPTURING` (1).
   *
   * @note This method is for iOS and macOS only.
   *
   * @return
   * - `true`: The current camera supports the portrait center stage.
   * - `false`: The current camera supports the portrait center stage.
   */
  virtual bool isCameraCenterStageSupported() = 0;

  /**
   * @brief Enables or disables portrait center stage.
   *
   * @details
   * The portrait center stage feature is off by default. You need to call this method to turn it on.
   * If you need to disable this feature, you need to call this method again and set `enabled` to
   * `false`.
   * Applicable scenarios: The portrait center stage feature can be widely used in scenarios such as
   * online meetings, shows, online education, etc. The host can use this feature to ensure that they
   * are always in the center of the screen, whether they move or not, in order to achieve a good
   * display effect.
   * Call timing: This method must be called after the camera is successfully enabled, that is, after
   * the SDK triggers the `onLocalVideoStateChanged` callback and returns the local video state as
   * `LOCAL_VIDEO_STREAM_STATE_CAPTURING` (1).
   *
   * @note
   * Due to the high performance requirements of this feature, you need to use it on the following
   * types of devices or devices with higher performance:
   * - iPad:
   *   - 12.9-inch iPad Pro (5th generation)
   *   - 11-inch iPad Pro (3rd generation)
   *   - iPad (9th generation)
   *   - iPad mini (6th generation)
   *   - iPad Air (5th generation)
   * - 2020 M1 MacBook Pro 13-inch + iPhone 11 (using iPhone as external camera for the MacBook)
   * Agora recommends that you call `isCameraCenterStageSupported` to check whether the current device
   * supports portrait center stage before enabling this feature.
   * This method is for iOS and macOS only.
   *
   * @param enabled Whether to enable the portrait center stage:
   * - `true`: Enable portrait center stage.
   * - `false`: Disable portrait center stage.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int enableCameraCenterStage(bool enabled) = 0;
#endif

#if defined(_WIN32) || (defined(__APPLE__) && TARGET_OS_MAC && !TARGET_OS_IPHONE)|| (defined(__linux__) && !defined(__ANDROID__) && !defined(__OHOS__))
  /**
   * @brief Gets a list of shareable screens and windows.
   *
   * @details
   * You can call this method before sharing a screen or window to get a list of shareable screens and
   * windows, which enables a user to use thumbnails in the list to easily choose a particular screen
   * or window to share. This list also contains important information such as window ID and screen
   * ID, with which you can call `startScreenCaptureByWindowId` or `startScreenCaptureByDisplayId` to
   * start the sharing.
   *
   * @note This method applies to macOS and Windows only.
   *
   * @param thumbSize The target size of the screen or window thumbnail (the width and height are in
   * pixels). See `SIZE`. The SDK scales the original image to make the length of the longest side of
   * the image the same as that of the target size without distorting the original image. For example,
   * if the original image is 400 × 300 and thumbSize is 100 × 100, the actual size of the `thumbnail`
   * is 100 × 75. If the target size is larger than the original size, the thumbnail is the original
   * image and the SDK does not scale it.
   * @param iconSize The target size of the icon corresponding to the application program (the width
   * and height are in pixels). See `SIZE`. The SDK scales the original image to make the length of
   * the longest side of the image the same as that of the target size without distorting the original
   * image. For example, if the original image is 400 × 300 and iconSize is 100 × 100, the actual size
   * of the ` icon` is 100 × 75. If the target size is larger than the original size, the icon is the
   * original image and the SDK does not scale it.
   * @param includeScreen Whether the SDK returns the screen information in addition to the window
   * information:
   * - `true`: The SDK returns screen and window information.
   * - `false`: The SDK returns window information only.
   *
   * @return
   * `IScreenCaptureSourceList`
   */
  virtual IScreenCaptureSourceList* getScreenCaptureSources(const SIZE& thumbSize, const SIZE& iconSize, const bool includeScreen) = 0;
#endif // _WIN32 || (__APPLE__ && !TARGET_OS_IPHONE && TARGET_OS_MAC)
#if (defined(__APPLE__) && TARGET_OS_IOS)
  /**
   * @brief Sets the operational permission of the SDK on the audio session.
   *
   * @details
   * The SDK and the app can both configure the audio session by default. If you need to only use the
   * app to configure the audio session, this method restricts the operational permission of the SDK
   * on the audio session.
   * You can call this method either before or after joining a channel. Once you call this method to
   * restrict the operational permission of the SDK on the audio session, the restriction takes effect
   * when the SDK needs to change the audio session.
   *
   * @note
   * - This method is only available for iOS.
   * - This method does not restrict the operational permission of the app on the audio session.
   *
   * @param restriction The operational permission of the SDK on the audio session. See
   * `AUDIO_SESSION_OPERATION_RESTRICTION`. This parameter is in bit mask format, and each bit
   * corresponds to a permission.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setAudioSessionOperationRestriction(AUDIO_SESSION_OPERATION_RESTRICTION restriction) = 0;
#endif // __APPLE__ && TARGET_OS_IOS

#if defined(_WIN32) || (defined(__APPLE__) && !TARGET_OS_IPHONE && TARGET_OS_MAC) || (defined(__linux__) && !defined(__ANDROID__) && !defined(__OHOS__))

  /**
   * @brief Captures the screen by specifying the display ID.
   *
   * @details
   * Captures the video stream of a screen or a part of the screen area.
   * Applicable scenarios: In the screen sharing scenario, you need to call this method to start
   * capturing the screen video stream.
   * Call timing: You can call this method either before or after joining the channel, with the
   * following differences:
   * - Call this method before joining a channel, and then call `joinChannel(const char* token, const
   * char* channelId, uid_t uid, const ChannelMediaOptions& options)` to join a channel
   * and set `publishScreenTrack` or `publishSecondaryScreenTrack` to `true` to start screen sharing.
   * - Call this method after joining a channel, and then call `updateChannelMediaOptions` to join a
   * channel and set `publishScreenTrack` or `publishSecondaryScreenTrack` to `true` to start screen
   * sharing.
   *
   * @note This method is for Windows and macOS only.
   *
   * @param displayId The display ID of the screen to be shared.Note: For the Windows platform, if you
   * need to simultaneously share two screens (main screen and secondary screen), you can set
   * `displayId` to `-1` when calling this method.
   * @param regionRect (Optional) Sets the relative location of the region to the screen. Pass in
   * `nil` to share the entire screen. See `Rectangle`.
   * @param captureParams Screen sharing configurations. The default video dimension is 1920 x 1080,
   * that is, 2,073,600 pixels. Agora uses the value of this parameter to calculate the charges. See
   * `ScreenCaptureParameters`.Attention: The video properties of the screen sharing stream only need
   * to be set through this parameter, and are unrelated to `setVideoEncoderConfiguration`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   *   - -2: The parameter is invalid.
   *   - -8: The screen sharing state is invalid. Probably because you have shared other screens or
   * windows. Try calling `stopScreenCapture()` to stop the current sharing and start sharing the
   * screen again.
   */
  virtual int startScreenCaptureByDisplayId(int64_t displayId, const Rectangle& regionRect,
                                            const ScreenCaptureParameters& captureParams) = 0;

#endif  // __APPLE__ && TARGET_OS_MAC && !TARGET_OS_IPHONE

#if defined(_WIN32)
  /**
   * @brief Captures the whole or part of a screen by specifying the screen rect.
   *
   * @deprecated This method is deprecated, use \ref IRtcEngine::startScreenCaptureByDisplayId
   * "startScreenCaptureByDisplayId" instead. Agora strongly recommends using
   * `startScreenCaptureByDisplayId` if you need to start screen sharing on a device connected to
   * another display.
   *
   * @details
   * This method shares a screen or part of the screen. You need to specify the area of the screen to
   * be shared.
   * You can call this method either before or after joining the channel, with the following
   * differences:
   * - Call this method before joining a channel, and then call `joinChannel(const char* token, const
   * char* channelId, uid_t uid, const ChannelMediaOptions& options)` to join a channel
   * and set `publishScreenTrack` or `publishSecondaryScreenTrack` to `true` to start screen sharing.
   * - Call this method after joining a channel, and then call `updateChannelMediaOptions` to join a
   * channel and set `publishScreenTrack` or `publishSecondaryScreenTrack` to `true` to start screen
   * sharing.
   *
   * @note This method applies to Windows only.
   *
   * @param screenRect Sets the relative location of the screen to the virtual screen.
   * @param regionRect (Optional) Sets the relative location of the region to the screen. If you do
   * not set this parameter, the SDK shares the whole screen. See `Rectangle`. If the specified region
   * overruns the screen, the SDK shares only the region within it; if you set width or height as 0,
   * the SDK shares the whole screen.
   * @param captureParams The screen sharing encoding parameters. The default video resolution is 1920
   * × 1080, that is, 2,073,600 pixels. Agora uses the value of this parameter to calculate the
   * charges. See `ScreenCaptureParameters`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   *   - -2: The parameter is invalid.
   *   - -8: The screen sharing state is invalid. Probably because you have shared other screens or
   * windows. Try calling `stopScreenCapture()` to stop the current sharing and start sharing the
   * screen again.
   */
  virtual int startScreenCaptureByScreenRect(const Rectangle& screenRect,
                                             const Rectangle& regionRect,
                                             const ScreenCaptureParameters& captureParams) __deprecated = 0;
#endif

#if defined(__ANDROID__)
  /**
   * @brief Gets the audio device information.
   *
   * @details
   * After calling this method, you can get whether the audio device supports ultra-low-latency
   * capture and playback.
   *
   * @note
   * - This method is for Android only.
   * - You can call this method either before or after joining a channel.
   *
   * @param deviceInfo Input and output parameter. A `DeviceInfo` object that identifies the audio
   * device information.
   * - Input value: A `DeviceInfo` object.
   * - Output value: A `DeviceInfo` object containing audio device information.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int getAudioDeviceInfo(DeviceInfo& deviceInfo) = 0;
#endif  // __ANDROID__

#if defined(_WIN32) || (defined(__APPLE__) && TARGET_OS_MAC && !TARGET_OS_IPHONE) || (defined(__linux__) && !defined(__ANDROID__) && !defined(__OHOS__))

  /**
   * @brief Captures the whole or part of a window by specifying the window ID.
   *
   * @details
   * This method captures a window or part of the window. You need to specify the ID of the window to
   * be captured.
   * This method supports window sharing of UWP (Universal Windows Platform) applications. Agora tests
   * the mainstream UWP applications by using the lastest SDK, see details as follows:
   *  | **System version** | **Software**                         | **Compatible versions** | **Support** |
   * | ------------------ | ------------------------------------ | ----------------------- | ----------- |
   * | win10              | Chrome                               | 76.0.3809.100           | No          |
   * |                    | Office Word                          | 18.1903.1152.0          | Yes         |
   * |                    | Office Excel                         |                         | No          |
   * |                    | Office PPT                           |                         | Yes         |
   * |                    | WPS Word                             | 11.1.0.9145             | Yes         |
   * |                    | WPS Excel                            |                         |             |
   * |                    | WPS PPT                              |                         |             |
   * |                    | Media Player (comes with the system) | All                     | Yes         |
   * | win8               | Chrome                               | All                     | Yes         |
   * |                    | Office Word                          | All                     | Yes         |
   * |                    | Office Excel                         |                         |             |
   * |                    | Office PPT                           |                         |             |
   * |                    | WPS Word                             | 11.1.0.9098             | Yes         |
   * |                    | WPS Excel                            |                         |             |
   * |                    | WPS PPT                              |                         |             |
   * |                    | Media Player (comes with the system) | All                     | Yes         |
   * | win7               | Chrome                               | 73.0.3683.103           | No          |
   * |                    | Office Word                          | All                     | Yes         |
   * |                    | Office Excel                         |                         |             |
   * |                    | Office PPT                           |                         |             |
   * |                    | WPS Word                             | 11.1.0.9098             | No          |
   * |                    | WPS Excel                            |                         |             |
   * |                    | WPS PPT                              | 11.1.0.9098             | Yes         |
   * |                    | Media Player (comes with the system) | All                     | No          |
   * Applicable scenarios: In the screen sharing scenario, you need to call this method to start
   * capturing the screen video stream.
   * Call timing: You can call this method either before or after joining the channel, with the
   * following differences:
   * - Call this method before joining a channel, and then call `joinChannel(const char* token, const
   * char* channelId, uid_t uid, const ChannelMediaOptions& options)` to join a channel
   * and set `publishScreenTrack` or `publishSecondaryScreenTrack` to `true` to start screen sharing.
   * - Call this method after joining a channel, and then call `updateChannelMediaOptions` to join a
   * channel and set `publishScreenTrack` or `publishSecondaryScreenTrack` to `true` to start screen
   * sharing.
   *
   * @note
   * The window sharing feature of the Agora SDK relies on WGC (Windows Graphics Capture) or GDI
   * (Graphics Device Interface) capture, and WGC cannot be set to disable mouse capture on systems
   * earlier than Windows 10 2004. Therefore, `captureMouseCursor(false)` might not work when you
   * start window sharing on a device with a system earlier than Windows 10 2004. See
   * `ScreenCaptureParameters`.
   * This method applies to the macOS and Windows only.
   *
   * @param windowId The ID of the window to be shared.
   * @param regionRect (Optional) Sets the relative location of the region to the screen. If you do
   * not set this parameter, the SDK shares the whole screen. See `Rectangle`. If the specified region
   * overruns the window, the SDK shares only the region within it; if you set width or height as 0,
   * the SDK shares the whole window.
   * @param captureParams Screen sharing configurations. The default video resolution is 1920 × 1080,
   * that is, 2,073,600 pixels. Agora uses the value of this parameter to calculate the charges. See
   * `ScreenCaptureParameters`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   *   - -2: The parameter is invalid.
   *   - -8: The screen sharing state is invalid. Probably because you have shared other screens or
   * windows. Try calling `stopScreenCapture()` to stop the current sharing and start sharing the
   * screen again.
   */
  virtual int startScreenCaptureByWindowId(int64_t windowId, const Rectangle& regionRect,
                                           const ScreenCaptureParameters& captureParams) = 0;

  /**
   * @brief Sets the content hint for screen sharing.
   *
   * @details
   * A content hint suggests the type of the content being shared, so that the SDK applies different
   * optimization algorithms to different types of content. If you don't call this method, the default
   * content hint is CONTENT_HINT_NONE.
   *
   * @note You can call this method either before or after you start screen sharing.
   *
   * @param contentHint The content hint for screen sharing. See `VIDEO_CONTENT_HINT`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   *   - -2: The parameter is invalid.
   *   - -8: The screen sharing state is invalid. Probably because you have shared other screens or
   * windows. Try calling `stopScreenCapture()` to stop the current sharing and start sharing the
   * screen again.
   */
  virtual int setScreenCaptureContentHint(VIDEO_CONTENT_HINT contentHint) = 0;

  /**
   * @brief Updates the screen capturing region.
   *
   * @note Call this method after starting screen sharing or window sharing.
   *
   * @param regionRect The relative location of the screen-share area to the screen or window. If you
   * do not set this parameter, the SDK shares the whole screen or window. See `Rectangle`. If the
   * specified region overruns the screen or window, the SDK shares only the region within it; if you
   * set width or height as 0, the SDK shares the whole screen or window.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   *   - -2: The parameter is invalid.
   *   - -8: The screen sharing state is invalid. Probably because you have shared other screens or
   * windows. Try calling `stopScreenCapture()` to stop the current sharing and start sharing the
   * screen again.
   */
  virtual int updateScreenCaptureRegion(const Rectangle& regionRect) = 0;

  /**
   * @brief Updates the screen capturing parameters.
   *
   * @note
   * - This method is for Windows and macOS only.
   * - Call this method after starting screen sharing or window sharing.
   *
   * @param captureParams The screen sharing encoding parameters. See
   * `ScreenCaptureParameters`.Attention: The video properties of the screen sharing stream only need
   * to be set through this parameter, and are unrelated to `setVideoEncoderConfiguration`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   *   - -2: The parameter is invalid.
   *   - -8: The screen sharing state is invalid. Probably because you have shared other screens or
   * windows. Try calling `stopScreenCapture()` to stop the current sharing and start sharing the
   * screen again.
   */
  virtual int updateScreenCaptureParameters(const ScreenCaptureParameters& captureParams) = 0;
#endif // _WIN32 || (__APPLE__ && !TARGET_OS_IPHONE && TARGET_OS_MAC)

#if defined(__ANDROID__) || (defined(__APPLE__) && TARGET_OS_IOS) || defined(__OHOS__)
  /**
   * @brief Starts screen capture.
   *
   * @details
   * Applicable scenarios: In the screen sharing scenario, you need to call this method to start
   * capturing the screen video stream.
   * Call timing: You can call this method either before or after joining the channel, with the
   * following differences:
   * - Call this method first and then call `joinChannel(const char* token, const char* channelId,
   * uid_t uid, const ChannelMediaOptions& options)` to join channel and set
   * `publishScreenCaptureVideo` to `true` to start screen sharing.
   * - Call this method after joining a channel, then call `updateChannelMediaOptions` and set
   * `publishScreenCaptureVideo` to `true` to start screen sharing.
   *
   * @note
   * - On the iOS platform, screen sharing is only available on iOS 12.0 and later.
   * - If you are using the custom audio source instead of the SDK to capture audio, Agora recommends
   * you add the keep-alive processing logic to your application to avoid screen sharing stopping when
   * the application goes to the background.
   * - This feature requires high-performance device, and Agora recommends that you use it on iPhone X
   * and later models.
   * - This method relies on the iOS screen sharing dynamic library
   * `AgoraReplayKitExtension.xcframework`. If the dynamic library is deleted, screen sharing cannot
   * be enabled normally.
   * - On the Android platform, if the user has not granted the app screen capture permission, the SDK
   * reports the `onPermissionError` `(2)` callback.
   * - On Android 9 and later, to avoid the application being killed by the system after going to the
   * background, Agora recommends you add the foreground service
   * `android.permission.FOREGROUND_SERVICE` to the `/app/Manifests/AndroidManifest.xml` file.
   * - Due to performance limitations, screen sharing is not supported on Android TV.
   * - Due to system limitations, if you are using Huawei phones, do not adjust the video encoding
   * resolution of the screen sharing stream during the screen sharing, or you could experience
   * crashes.
   * - Due to system limitations, some Xiaomi devices do not support capturing system audio during
   * screen sharing.
   * - To avoid system audio capture failure when sharing screen, Agora recommends that you set the
   * audio application scenario to `AUDIO_SCENARIO_GAME_STREAMING` by using the `setAudioScenario`
   * method before joining the channel.
   * - This method is for Android and iOS only.
   * - The billing for the screen sharing stream is based on the `dimensions` in
   * `ScreenVideoParameters`:
   *   - When you do not pass in a value, Agora bills you at 1280 × 720.
   *   - When you pass in a value, Agora bills you at that value.
   *
   * @param captureParams The screen sharing encoding parameters. See `ScreenCaptureParameters2`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   *   - -2 (iOS platform): Empty parameter.
   *   - -2 (Android platform): The system version is too low. Ensure that the Android API level is
   * not lower than 21.
   *   - -3 (Android platform): Unable to capture system audio. Ensure that the Android API level is
   * not lower than 29.
   */
  virtual int startScreenCapture(const ScreenCaptureParameters2& captureParams) = 0;

  /**
   * @brief Updates the screen capturing parameters.
   *
   * @details
   * If the system audio is not captured when screen sharing is enabled, and then you want to update
   * the parameter configuration and publish the system audio, you can refer to the following steps:1.
   * Call this method, and set `captureAudio` to `true`.
   * 2. Call `updateChannelMediaOptions`, and set `publishScreenCaptureAudio` to `true` to publish the
   * audio captured by the screen.
   *
   * @note
   * - This method is for Android and iOS only.
   * - On the iOS platform, screen sharing is only available on iOS 12.0 and later.
   *
   * @param captureParams The screen sharing encoding parameters. See `ScreenCaptureParameters2`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   *   - -2: The parameter is invalid.
   *   - -8: The screen sharing state is invalid. Probably because you have shared other screens or
   * windows. Try calling `stopScreenCapture()` to stop the current sharing and start sharing the
   * screen again.
   */
  virtual int updateScreenCapture(const ScreenCaptureParameters2& captureParams) = 0;

  /**
   * @brief Queries the highest frame rate supported by the device during screen sharing.
   *
   * @since v4.2.0
   *
   * @details
   * Applicable scenarios: This method is for Android and iOS only.
   * To ensure optimal screen sharing performance, particularly in enabling high frame rates like 60
   * fps, Agora recommends you to query the device's maximum supported frame rate using this method
   * beforehand. This way, if the device cannot support such a high frame rate, you can adjust the
   * screen sharing stream accordingly to avoid any negative impact on the sharing quality. If the
   * device does not support high frame rate, you can reduce the frame rate of the screen sharing
   * stream appropriately when sharing the screen to ensure that the sharing effect meets your
   * expectation.
   *
   * @return
   * - The highest frame rate supported by the device, if the method is called successfully. See
   * `SCREEN_CAPTURE_FRAMERATE_CAPABILITY`.
   * - < 0: Failure.
   */
  virtual int queryScreenCaptureCapability() = 0;

  /**
   * @brief Queries the focal length capability supported by the camera.
   *
   * @details
   * If you want to enable the wide-angle or ultra-wide-angle mode for camera capture, it is
   * recommended to start by calling this method to check whether the device supports the required
   * focal length capability. Then, adjust the camera's focal length configuration based on the query
   * result by calling `setCameraCapturerConfiguration`, ensuring the best camera capture performance.
   *
   * @note This method is for Android and iOS only.
   *
   * @param focalLengthInfos Input and output parameter. The pointer to an array of `FocalLengthInfo`
   * objects:
   * - Input value: The pointer to an array of `FocalLengthInfo` objects, used to store focal length
   * information.
   * - Output value: After the method is executed, output the queried focal length information.
   * @param size Input and output parameter. The number of focal length information items:
   * - Input value: Specifies the maximum number of focal length information items that
   * `focalLengthInfos` can hold. Ensure this value is not less than 8, meaning `focalLengthInfos` has
   * space for at least 8 focal length information items.
   * - Output value: After the method is executed, output the number of focal length information items
   * retrieved.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int queryCameraFocalLengthCapability(agora::rtc::FocalLengthInfo* focalLengthInfos, int& size) = 0;

#if defined(__ANDROID__)
  /**
   * @brief Configures `MediaProjection` outside of the SDK to capture screen video streams.
   *
   * @technical preview
   *
   * @details
   * After successfully calling this method, the external `MediaProjection` you set will replace the
   * `MediaProjection` requested by the SDK to capture the screen video stream.
   * When the screen sharing is stopped or `IRtcEngine` is destroyed, the SDK will automatically
   * release the MediaProjection.
   * Applicable scenarios: If you are able to apply for `MediaProjection`, you can directly use your
   * `MediaProjection` instead of the one applied for by the SDK. The following lists two applicable
   * scenarios:``
   * - On custom system devices, it can avoid system pop-ups (such as requiring user permission to
   * capture the screen) and directly start capturing the screen video stream.
   * - In a screen sharing process that involves one or more sub-processes, it can help avoid errors
   * that might occur when creating objects within these sub-processes, which could otherwise lead to
   * failures in screen capturing.
   * Call timing: Call this method after `startScreenCapture(const ScreenCaptureParameters2&
   * captureParams)`.
   *
   * @note
   * Before calling this method, you must first apply for `MediaProjection` permission.
   * This method is for Android only.
   *
   * @param mediaProjection An `MediaProjection` object used to capture screen video streams.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setExternalMediaProjection(void* mediaProjection) = 0;
#endif
#endif

#if defined(_WIN32) || defined(__APPLE__) || defined(__ANDROID__) || (defined(__linux__) && !defined(__ANDROID__) && !defined (__OHOS__))
  /**
   * @brief Sets the screen sharing scenario.
   *
   * @details
   * When you start screen sharing or window sharing, you can call this method to set the screen
   * sharing scenario. The SDK adjusts the video quality and experience of the sharing according to
   * the scenario.
   *
   * @note Agora recommends that you call this method before joining a channel.
   *
   * @param screenScenario The screen sharing scenario. See `SCREEN_SCENARIO_TYPE`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setScreenCaptureScenario(SCREEN_SCENARIO_TYPE screenScenario) = 0;

  /**
   * @brief Stops screen capture.
   *
   * @details
   * Applicable scenarios: If you start screen capture by calling `startScreenCapture(const
   * ScreenCaptureParameters2& captureParams)`,
   * `startScreenCaptureByWindowId`, or `startScreenCaptureByDisplayId`, you need to call this method
   * to stop screen capture.
   * Call timing: You can call this method either before or after joining a channel.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int stopScreenCapture() = 0;
#endif  // _WIN32 || (__APPLE__ && !TARGET_OS_IPHONE && TARGET_OS_MAC) || __ANDROID__

  /**
   * @brief Retrieves the call ID.
   *
   * @details
   * When a user joins a channel on a client, a `callId` is generated to identify the call from the
   * client. You can call this method to get `callId`, and pass it in when calling methods such as
   * `rate` and `complain`.
   * Call timing: Call this method after joining a channel.
   *
   * @param callId Output parameter, the current call ID.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int getCallId(agora::util::AString& callId) = 0;

  /**
   * @brief Allows a user to rate a call after the call ends.
   *
   * @note Ensure that you call this method after leaving a channel.
   *
   * @param callId The current call ID. You can get the call ID by calling `getCallId`.
   * @param rating The value is between 1 (the lowest score) and 5 (the highest score).
   * @param description (Optional) A description of the call. The string length should be less than
   * 800 bytes.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   *   - -1: A general error occurs (no specified reason).
   *   - -2: The parameter is invalid.
   */
  virtual int rate(const char* callId, int rating, const char* description) = 0;  // 0~10

  /**
   * @brief Allows a user to complain about the call quality after a call ends.
   *
   * @details
   * This method allows users to complain about the quality of the call. Call this method after the
   * user leaves the channel.
   *
   * @param callId The current call ID. You can get the call ID by calling `getCallId`.
   * @param description (Optional) A description of the call. The string length should be less than
   * 800 bytes.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   *   - -1: A general error occurs (no specified reason).
   *   - -2: The parameter is invalid.
   *   - -7: The method is called before `IRtcEngine` is initialized.
   */
  virtual int complain(const char* callId, const char* description) = 0;

  /**
   * @brief Starts pushing media streams to a CDN without transcoding.
   *
   * @details
   * Agora recommends that you use the server-side Media Push function. For details, see `Use RESTful
   * API`.
   * You can call this method to push an audio or video stream to the specified CDN address. This
   * method can push media streams to only one CDN address at a time, so if you need to push streams
   * to multiple addresses, call this method multiple times.
   * After you call this method, the SDK triggers the `onRtmpStreamingStateChanged` callback on the
   * local client to report the state of the streaming.
   *
   * @note
   * - Call this method after joining a channel.
   * - Only hosts in the LIVE_BROADCASTING profile can call this method.
   * - If you want to retry pushing streams after a failed push, make sure to call `stopRtmpStream`
   * first, then call this method to retry pushing streams; otherwise, the SDK returns the same error
   * code as the last failed push.
   *
   * @param url The address of Media Push. The format is RTMP or RTMPS. The character length cannot
   * exceed 1024 bytes. Special characters such as Chinese characters are not supported.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   *   - -2: The URL or configuration of transcoding is invalid; check your URL and transcoding
   * configurations.
   *   - -7: The SDK is not initialized before calling this method.
   *   - -19: The Media Push URL is already in use; use another URL instead.
   */
  virtual int startRtmpStreamWithoutTranscoding(const char* url) = 0;

  /**
   * @brief Starts Media Push and sets the transcoding configuration.
   *
   * @details
   * Agora recommends that you use the server-side Media Push function. For details, see `Use RESTful
   * API`.
   * You can call this method to push a live audio-and-video stream to the specified CDN address and
   * set the transcoding configuration. This method can push media streams to only one CDN address at
   * a time, so if you need to push streams to multiple addresses, call this method multiple times.
   * Under one Agora project, the maximum number of concurrent tasks to push media streams is 200 by
   * default. If you need a higher quota, contact `technical support`.
   * After you call this method, the SDK triggers the `onRtmpStreamingStateChanged` callback on the
   * local client to report the state of the streaming.
   *
   * @note
   * - Call this method after joining a channel.
   * - Only hosts in the LIVE_BROADCASTING profile can call this method.
   * - If you want to retry pushing streams after a failed push, make sure to call `stopRtmpStream`
   * first, then call this method to retry pushing streams; otherwise, the SDK returns the same error
   * code as the last failed push.
   *
   * @param url The address of Media Push. The format is RTMP or RTMPS. The character length cannot
   * exceed 1024 bytes. Special characters such as Chinese characters are not supported.
   * @param transcoding The transcoding configuration for Media Push. See `LiveTranscoding`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   *   - -2: The URL or configuration of transcoding is invalid; check your URL and transcoding
   * configurations.
   *   - -7: The SDK is not initialized before calling this method.
   *   - -19: The Media Push URL is already in use; use another URL instead.
   */
  virtual int startRtmpStreamWithTranscoding(const char* url, const LiveTranscoding& transcoding) = 0;

  /**
   * @brief Updates the transcoding configuration.
   *
   * @details
   * Agora recommends that you use the server-side Media Push function. For details, see `Use RESTful
   * API`.
   * After you start pushing media streams to CDN with transcoding, you can dynamically update the
   * transcoding configuration according to the scenario. The SDK triggers the `onTranscodingUpdated`
   * callback after the transcoding configuration is updated.
   *
   * @param transcoding The transcoding configuration for Media Push. See `LiveTranscoding`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int updateRtmpTranscoding(const LiveTranscoding& transcoding) = 0;

  /**
   * @brief Starts the local video mixing.
   *
   * @details
   * After calling this method, you can merge multiple video streams into one video stream locally.
   * For example, you can merge the video streams captured by the camera, screen sharing, media
   * player, remote video, video files, images, etc. into one video stream, and then publish the mixed
   * video stream to the channel.
   * Applicable scenarios: You can enable the local video mixing function in scenarios such as remote
   * conferences, live streaming, and online education, which allows users to view and manage multiple
   * videos more conveniently, and supports portrait-in-picture effect and other functions.
   * The following is a typical use case for implementing the portrait-in-picture effect:1. Call
   * `enableVirtualBackground`, and set the custom background image to `BACKGROUND_NONE`, that is,
   * separate the portrait and the background in the video captured by the camera.
   * 2. Call `startScreenCapture(VIDEO_SOURCE_TYPE sourceType, const ScreenCaptureConfiguration&
   * config)` to start capturing the screen sharing video stream.
   * 3. Call this method and set the video source for capturing portraits as one of the video sources
   * participating in the local video mixing, picture-in-picture of the portrait can be achived in the
   * mixed video.
   * Call timing: - If you need to mix the locally collected video streams, you need to call this
   * method after `startCameraCapture` or `startScreenCapture(VIDEO_SOURCE_TYPE sourceType, const
   * ScreenCaptureConfiguration& config)`.
   * - If you want to publish the mixed video stream to the channel, you need to set
   * `publishTranscodedVideoTrack` in `ChannelMediaOptions` to `true` when calling `joinChannel(const
   * char* token, const char* channelId, uid_t uid, const ChannelMediaOptions& options)`
   * or `updateChannelMediaOptions`.
   * Related callbacks: When you fail to call this method, the SDK triggers the
   * `onLocalVideoTranscoderError` callback to report the reason.
   *
   * @note
   * - Local video mixing requires more CPU resources. Therefore, Agora recommends enabling this
   * function on devices with higher performance.
   * - If you need to mix locally captured video streams, the SDK supports the following capture
   * combinations:
   *   - On the Windows platform, it supports up to 4 video streams captured by cameras + 4 screen
   * sharing streams.
   *   - On the macOS platform, it supports up to 4 video streams captured by cameras + 1 screen
   * sharing stream.
   *   - On Android and iOS platforms, it supports video streams captured by up to 2 cameras (the
   * device itself needs to support dual cameras or supports external cameras) + 1 screen sharing
   * stream.
   * - When configuring the local video mixing, it is necessary to ensure that the layer number of the
   * video stream capturing the portrait is greater than the layer number of the screen sharing
   * stream. Otherwise, the portrait will be covered by the screen sharing and will not be displayed
   * in the final mixed video stream.
   *
   * @param config Configuration of the local video mixing, see
   * `LocalTranscoderConfiguration`.Attention:
   * - The maximum resolution of each video stream participating in the local video mixing is 4096 ×
   * 2160. If this limit is exceeded, video mixing does not take effect.
   * - The maximum resolution of the mixed video stream is 4096 × 2160.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int startLocalVideoTranscoder(const LocalTranscoderConfiguration& config) = 0;
  /**
   * @brief Updates the local video mixing configuration.
   *
   * @details
   * After calling `startLocalVideoTranscoder`, call this method if you want to update the local video
   * mixing configuration.
   *
   * @note If you want to update the video source type used for local video mixing, such as adding a
   * second camera or screen to capture video, you need to call this method after `startCameraCapture`
   * or `startScreenCapture(VIDEO_SOURCE_TYPE sourceType, const ScreenCaptureConfiguration& config)`.
   *
   * @param config Configuration of the local video mixing, see `LocalTranscoderConfiguration`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int updateLocalTranscoderConfiguration(const LocalTranscoderConfiguration& config) = 0;

  /**
   * @brief Stops pushing media streams to a CDN.
   *
   * @details
   * Agora recommends that you use the server-side Media Push function. For details, see `Use RESTful
   * API`.
   * You can call this method to stop the live stream on the specified CDN address. This method can
   * stop pushing media streams to only one CDN address at a time, so if you need to stop pushing
   * streams to multiple addresses, call this method multiple times.
   * After you call this method, the SDK triggers the `onRtmpStreamingStateChanged` callback on the
   * local client to report the state of the streaming.
   *
   * @param url The address of Media Push. The format is RTMP or RTMPS. The character length cannot
   * exceed 1024 bytes. Special characters such as Chinese characters are not supported.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int stopRtmpStream(const char* url) = 0;

  /**
   * @brief Stops the local video mixing.
   *
   * @details
   * After calling `startLocalVideoTranscoder`, call this method if you want to stop the local video
   * mixing.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int stopLocalVideoTranscoder() = 0;

  /**
   * @brief Starts local audio mixing.
   *
   * @details
   * This method supports merging multiple audio streams into one audio stream locally. For example,
   * merging the audio streams captured from the local microphone, and that from the media player, the
   * sound card, and the remote users into one audio stream, and then publish the merged audio stream
   * to the channel.
   * - If you want to mix the locally captured audio streams, you can set publishMixedAudioTrack in
   * `ChannelMediaOptions` to `true`, and then publish the mixed audio stream to the channel.
   * - If you want to mix the remote audio stream, ensure that the remote audio stream has been
   * published in the channel and you have subcribed to the audio stream that you need to mix.
   * Applicable scenarios: You can enable this function in the following scenarios:
   * - By utilizing the local video mixing feature, the associated audio streams of the mixed video
   * streams can be simultaneously captured and published.
   * - In live streaming scenarios, users can receive audio streams within the channel, mix multiple
   * audio streams locally, and then forward the mixed audio stream to other channels.
   * - In online classes, teachers can mix the audio from interactions with students locally and then
   * forward the mixed audio stream to other channels.
   * Call timing: You can call this method either before or after joining a channel.
   *
   * @note To ensure audio quality, it is recommended that the number of audio streams to be mixed
   * does not exceed 10.
   *
   * @param config The configurations for mixing the lcoal audio. See `LocalAudioMixerConfiguration`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   *   - -7: The `IRtcEngine` object has not been initialized. You need to initialize the `IRtcEngine`
   * object before calling this method.
   */
  virtual int startLocalAudioMixer(const LocalAudioMixerConfiguration& config) = 0;

  /**
   * @brief Updates the configurations for mixing audio streams locally.
   *
   * @details
   * After calling `startLocalAudioMixer`, call this method if you want to update the local audio
   * mixing configuration.
   * Call timing: Call this method after `startLocalAudioMixer`.
   *
   * @note To ensure audio quality, it is recommended that the number of audio streams to be mixed
   * does not exceed 10.
   *
   * @param config The configurations for mixing the lcoal audio. See `LocalAudioMixerConfiguration`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   *   - -7: The `IRtcEngine` object has not been initialized. You need to initialize the `IRtcEngine`
   * object before calling this method.
   */
  virtual int updateLocalAudioMixerConfiguration(const LocalAudioMixerConfiguration& config) = 0;

  /**
   * @brief Stops the local audio mixing.
   *
   * @details
   * After calling `startLocalAudioMixer`, call this method if you want to stop the local audio
   * mixing.
   * Call timing: Call this method after `startLocalAudioMixer`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   *   - -7: The `IRtcEngine` object has not been initialized. You need to initialize the `IRtcEngine`
   * object before calling this method.
   */
  virtual int stopLocalAudioMixer() = 0;

  /**
   * @brief Starts camera capture.
   *
   * @details
   * You can call this method to start capturing video from one or more cameras by specifying
   * `sourceType`.
   *
   * @note On the iOS platform, if you want to enable multi-camera capture, you need to call
   * `enableMultiCamera` and set `enabled` to `true` before calling this method.
   *
   * @param sourceType The type of the video source. See `VIDEO_SOURCE_TYPE`.
   * Note:
   * - On iOS devices, you can capture video from up to 2 cameras, provided the device has multiple
   * cameras or supports external cameras.
   * - On Android devices, you can capture video from up to 4 cameras, provided the device has
   * multiple cameras or supports external cameras.
   * - On the desktop platforms, you can capture video from up to 4 cameras.
   * @param config The configuration of the video capture. See `CameraCapturerConfiguration`.
   * Note: On the iOS platform, this parameter has no practical function. Use the `config` parameter
   * in `enableMultiCamera` instead to set the video capture configuration.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int startCameraCapture(VIDEO_SOURCE_TYPE sourceType, const CameraCapturerConfiguration& config) = 0;

  /**
   * @brief Stops camera capture.
   *
   * @details
   * After calling `startCameraCapture` to start capturing video through one or more cameras, you can
   * call this method and set the `sourceType` parameter to stop the capture from the specified
   * cameras.
   *
   * @note
   * If you are using the local video mixing function, calling this method can cause the local video
   * mixing to be interrupted.
   * On the iOS platform, if you want to disable multi-camera capture, you need to call
   * `enableMultiCamera` after calling this method and set `enabled` to `false`.
   *
   * @param sourceType The type of the video source. See `VIDEO_SOURCE_TYPE`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int stopCameraCapture(VIDEO_SOURCE_TYPE sourceType) = 0;
  /**
   * @brief Sets the rotation angle of the captured video.
   *
   * @note
   * - This method applies to Windows only.
   * - You must call this method after `enableVideo`. The setting result will take effect after the
   * camera is successfully turned on, that is, after the SDK triggers the `onLocalVideoStateChanged`
   * callback and returns the local video state as `LOCAL_VIDEO_STREAM_STATE_CAPTURING` (1).
   * - When the video capture device does not have the gravity sensing function, you can call this
   * method to manually adjust the rotation angle of the captured video.
   *
   * @param type The video source type. See `VIDEO_SOURCE_TYPE`.
   * @param orientation The clockwise rotation angle. See `VIDEO_ORIENTATION`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setCameraDeviceOrientation(VIDEO_SOURCE_TYPE type, VIDEO_ORIENTATION orientation) = 0;
  /**
   * Sets the rotation angle of the video captured by the screen.
   *
   * When the screen capture device does not have the gravity sensing function, you can call this method to manually adjust the rotation angle of the captured video.
   *
   * @param type The video source type. See #VIDEO_SOURCE_TYPE.
   * @param orientation The clockwise rotation angle. See #VIDEO_ORIENTATION.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setScreenCaptureOrientation(VIDEO_SOURCE_TYPE type, VIDEO_ORIENTATION orientation) = 0;

  /**
   * @brief Starts screen capture from the specified video source.
   *
   * @details
   * Applicable scenarios: In the screen sharing scenario, you need to call this method to start
   * capturing the screen video stream.
   * The SDK supports a series of methods for screen capturing, with the following distinctions
   * between them. Please choose according to the actual scenario:
   * - `startScreenCapture(const ScreenCaptureParameters2& captureParams)` /
   * `startScreenCaptureByDisplayId` / `startScreenCaptureByWindowId`:
   * Only supports capturing a single screen or window, suitable for scenarios where only a single
   * screen is shared.
   * - `startScreenCapture(VIDEO_SOURCE_TYPE sourceType, const ScreenCaptureConfiguration& config)`:
   * Supports specifying multiple video sources to capture multiple
   * screen sharing streams, used for local video mixing or multi-channel scenarios.
   * Call timing: You can call this method either before or after joining the channel, with the
   * following differences:
   * - Call this method first and then call `joinChannel(const char* token, const char* channelId,
   * uid_t uid, const ChannelMediaOptions& options)` to join channel and set
   * `publishScreenCaptureVideo` to `true` to start screen sharing.
   * - Call this method after joining a channel, then call `updateChannelMediaOptions` and set
   * `publishScreenCaptureVideo` to `true` to start screen sharing.
   *
   * @note
   * - If you start screen capture by calling this method, you need to call
   * `stopScreenCapture(VIDEO_SOURCE_TYPE sourceType)`
   * to stop screen capture.
   * - On the Windows platform, it supports up to four screen capture video streams.
   * - On the macOS platform, it supports only one screen capture video stream.
   * This method applies to the macOS and Windows only.
   *
   * @param sourceType The type of the video source. See `VIDEO_SOURCE_TYPE`.Attention: On the macOS
   * platform, this parameter can only be set to VIDEO_SOURCE_SCREEN (2).
   * @param config The configuration of the captured screen. See `ScreenCaptureConfiguration`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int startScreenCapture(VIDEO_SOURCE_TYPE sourceType, const ScreenCaptureConfiguration& config) = 0;

  /**
   * @brief Stops screen capture from the specified video source.
   *
   * @details
   * Applicable scenarios: If you start screen capture from one or more screens by calling
   * `startScreenCapture(VIDEO_SOURCE_TYPE sourceType, const ScreenCaptureConfiguration& config)`, you
   * need to call this method to stop screen capture, specifying the
   * screen through the `sourceType` parameter.
   * Call timing: You can call this method either before or after joining a channel.
   *
   * @note This method applies to the macOS and Windows only.
   *
   * @param sourceType The type of the video source. See `VIDEO_SOURCE_TYPE`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int stopScreenCapture(VIDEO_SOURCE_TYPE sourceType) = 0;

  /**
   * @brief Gets the current connection state of the SDK.
   *
   * @details
   * Call timing: This method can be called either before or after joining the channel.
   *
   * @return
   * The current connection state. See `CONNECTION_STATE_TYPE`.
   */
  virtual CONNECTION_STATE_TYPE getConnectionState() = 0;

  // The following APIs are not implemented yet.
  virtual bool registerEventHandler(IRtcEngineEventHandler* eventHandler) = 0;
  virtual bool unregisterEventHandler(IRtcEngineEventHandler* eventHandler) = 0;
  virtual int setRemoteUserPriority(uid_t uid, PRIORITY_TYPE userPriority) = 0;

  /**
   * @brief Registers a packet observer.
   *
   * @details
   * Call this method registers a packet observer. When the Agora SDK triggers `IPacketObserver`
   * callbacks registered by for voice or video packet transmission, you can call this method to
   * process the packets, such as encryption and decryption.
   *
   * @note
   * - The size of the packet sent to the network after processing should not exceed 1200 bytes,
   * otherwise, the SDK may fail to send the packet.
   * - Ensure that both receivers and senders call this method; otherwise, you may meet undefined
   * behaviors such as no voice and black screen.
   * - When you use the Media Push or recording functions, Agora doesn't recommend calling this
   * method.
   * - Call this method before joining a channel.
   *
   * @param observer `IPacketObserver`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int registerPacketObserver(IPacketObserver* observer) = 0;


  /**
   * @brief Enables or disables the built-in encryption.
   *
   * @details
   * After the user leaves the channel, the SDK automatically disables the built-in encryption. To
   * enable the built-in encryption, call this method before the user joins the channel again.
   * Applicable scenarios: Scenarios with higher security requirements.
   * Call timing: Call this method before joining a channel.
   *
   * @note
   * - All users within the same channel must set the same encryption configurations when calling this
   * method.
   * - If you enable the built-in encryption, you cannot use the Media Push function.
   *
   * @param enabled Whether to enable built-in encryption:
   * - true: Enable the built-in encryption.
   * - false: (Default) Disable the built-in encryption.
   * @param config Built-in encryption configurations. See `EncryptionConfig`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   *   - -2: An invalid parameter is used. Set the parameter with a valid value.
   *   - -4: The built-in encryption mode is incorrect or the SDK fails to load the external
   * encryption library. Check the enumeration or reload the external encryption library.
   *   - -7: The SDK is not initialized. Initialize the `IRtcEngine` instance before calling this
   * method.
   */
  virtual int enableEncryption(bool enabled, const EncryptionConfig& config) = 0;

  /**
   * @brief Creates a data stream.
   *
   * @details
   * You can call this method to create a data stream and improve the reliability and ordering of data
   * transmission.
   * Call timing: You can call this method either before or after joining a channel.
   * Related callbacks: After setting `reliable` to `true`, if the recipient does not receive the data
   * within five seconds, the SDK triggers the `onStreamMessageError` callback and returns an error
   * code.
   *
   * @note
   * Each user can create up to five data streams during the lifecycle of `IRtcEngine`. The data
   * stream will be destroyed when leaving the channel, and the data stream needs to be recreated if
   * needed.
   * If you need a more comprehensive solution for low-latency, high-concurrency, and scalable
   * real-time messaging and status synchronization, it is recommended to use `Signaling`.
   *
   * @param streamId An output parameter; the ID of the data stream created.
   * @param reliable Sets whether the recipients are guaranteed to receive the data stream within five
   * seconds:
   * - `true`: The recipients receive the data from the sender within five seconds. If the recipient
   * does not receive the data within five seconds, the SDK triggers the `onStreamMessageError`
   * callback and returns an error code.
   * - `false`: There is no guarantee that the recipients receive the data stream within five seconds
   * and no error message is reported for any delay or missing data stream.
   * Attention: Please ensure that `reliable` and `ordered` are either both set to`true` or both set
   * to `false`.
   * @param ordered Sets whether the recipients receive the data stream in the sent order:
   * - `true`: The recipients receive the data in the sent order.
   * - `false`: The recipients do not receive the data in the sent order.
   *
   * @return
   * - 0: The data stream is successfully created.
   * - < 0: Failure.
   */
  virtual int createDataStream(int* streamId, bool reliable, bool ordered) = 0;

  /**
   * @brief Creates a data stream.
   *
   * @details
   * Compared to `createDataStream(int* streamId, bool reliable, bool ordered)`, this method does not
   * guarantee the reliability of data
   * transmission. If a data packet is not received five seconds after it was sent, the SDK directly
   * discards the data.
   * Call timing: You can call this method either before or after joining a channel.
   *
   * @note
   * Each user can create up to five data streams during the lifecycle of `IRtcEngine`. The data
   * stream will be destroyed when leaving the channel, and the data stream needs to be recreated if
   * needed.
   * If you need a more comprehensive solution for low-latency, high-concurrency, and scalable
   * real-time messaging and status synchronization, it is recommended to use `Signaling`.
   *
   * @param streamId An output parameter; the ID of the data stream created.
   * @param config The configurations for the data stream. See `DataStreamConfig`.
   *
   * @return
   * - 0: The data stream is successfully created.
   * - < 0: Failure.
   */
  virtual int createDataStream(int* streamId, const DataStreamConfig& config) = 0;

  /**
   * @brief Sends data stream messages.
   *
   * @details
   * After calling `createDataStream(int* streamId, const DataStreamConfig& config)`, you can call
   * this method to send data stream messages to
   * all users in the channel.
   * The SDK has the following restrictions on this method:
   * - Each client within the channel can have up to 5 data channels simultaneously, with a total
   * shared packet bitrate limit of 30 KB/s for all data channels.
   * - Each data channel can send up to 60 packets per second, with each packet being a maximum of 1
   * KB.
   * A successful method call triggers the `onStreamMessage` callback on the remote client, from which
   * the remote user gets the stream message. A failed method call triggers the `onStreamMessageError`
   * callback on the remote client.
   *
   * @note
   * If you need a more comprehensive solution for low-latency, high-concurrency, and scalable
   * real-time messaging and status synchronization, it is recommended to use `Signaling`.
   * - This method needs to be called after `createDataStream(int* streamId, const DataStreamConfig&
   * config)` and joining the channel.
   * - This method applies to broadcasters only.
   *
   * @param streamId The data stream ID. You can get the data stream ID by calling
   * `createDataStream(int* streamId, const DataStreamConfig& config)`
   * .
   * @param data The message to be sent.
   * @param length The length of the data.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int sendStreamMessage(int streamId, const char* data, size_t length) = 0;

  /**
   * @brief Send Reliable message to remote uid in channel.
   *
   * @technical preview
   *
   * @param uid remote user id.
   * @param type Reliable Data Transmission tunnel message type. See RdtStreamType
   * @param data The pointer to the sent data.
   * @param length The length of the sent data.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int sendRdtMessage(uid_t uid, RdtStreamType type, const char *data, size_t length) = 0;

  /**
   * @brief Send media control message
   *
   * @technical preview
   *
   * @param uid Remote user id. In particular, if the uid is set to 0, it means broadcasting the message to the entire channel.
   * @param data The pointer to the sent data.
   * @param length The length of the sent data, max 1024.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int sendMediaControlMessage(uid_t uid, const char* data, size_t length) = 0;

  /**
   * @brief Adds a watermark image to the local video.
   *
   * @details
   * This method adds a PNG watermark image to the local video stream in a live streaming session.
   * Once the watermark image is added, all the users in the channel (CDN audience included) and the
   * video capturing device can see and capture it. If you only want to add a watermark to the CDN
   * live streaming, see `startRtmpStreamWithTranscoding`.
   *
   * @note
   * - The URL descriptions are different for the local video and CDN live streaming: In a local video
   * stream, URL refers to the absolute path of the added watermark image file in the local video
   * stream. In a CDN live stream, URL refers to the URL address of the added watermark image in the
   * CDN live streaming.
   * - The source file of the watermark image must be in the PNG file format. If the width and height
   * of the PNG file differ from your settings in this method, the PNG file will be cropped to conform
   * to your settings.
   * - The Agora SDK supports adding only one watermark image onto a local video or CDN live stream.
   * The newly added watermark image replaces the previous one.
   *
   * @param watermark The watermark image to be added to the local live streaming: `RtcImage`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int addVideoWatermark(const RtcImage& watermark) __deprecated = 0;

  /**
   * @brief Adds a watermark image to the local video.
   *
   * @deprecated Use addVideoWatermarkEx(const WatermarkConfig& config, const RtcConnection&
   * connection) instead.
   *
   * @details
   * This method adds a PNG watermark image to the local video in the live streaming. Once the
   * watermark image is added, all the audience in the channel (CDN audience included), and the
   * capturing device can see and capture it. The Agora SDK supports adding only one watermark image
   * onto a live video stream. The newly added watermark image replaces the previous one.
   * The watermark coordinates are dependent on the settings in the `setVideoEncoderConfiguration`
   * method:
   * - If the orientation mode of the encoding video ( `ORIENTATION_MODE` ) is fixed landscape mode or
   * the adaptive landscape mode, the watermark uses the landscape orientation.
   * - If the orientation mode of the encoding video ( `ORIENTATION_MODE` ) is fixed portrait mode or
   * the adaptive portrait mode, the watermark uses the portrait orientation.
   * - When setting the watermark position, the region must be less than the dimensions set in the
   * `setVideoEncoderConfiguration` method; otherwise, the watermark image will be cropped.
   * You can control the visibility of the watermark during preview by setting the `visibleInPreview` parameter when calling this method. However, whether it ultimately takes effect also depends on the position parameter you set when calling `setupLocalVideo` (the ` position` of the video frame in the video link). Refer to the table below for details.| Observation position               | visibleInPreview value | Watermark visibility |
   * | ---------------------------------- | ---------------------- | -------------------- |
   * | (Default) `POSITION_POST_CAPTURER` | `true`                 | Yes                  |
   * |                                    | `false`                | No                   |
   * | `POSITION_PRE_ENCODER`             | `true`                 | Yes                  |
   * |                                    | `false`                | Yes                  |
   *
   * @note
   * - Ensure that calling this method after `enableVideo`.
   * - If you only want to add a watermark to the media push, you can call this method or the
   * `startRtmpStreamWithTranscoding` method.
   * - This method supports adding a watermark image in the PNG file format only. Supported pixel
   * formats of the PNG image are RGBA, RGB, Palette, Gray, and Alpha_gray.
   * - If the dimensions of the PNG image differ from your settings in this method, the image will be
   * cropped or zoomed to conform to your settings.
   * - If you have enabled the mirror mode for the local video, the watermark on the local video is
   * also mirrored. To avoid mirroring the watermark, Agora recommends that you do not use the mirror
   * and watermark functions for the local video at the same time. You can implement the watermark
   * function in your application layer.
   *
   * @param watermarkUrl The local file path of the watermark image to be added. This method supports
   * adding a watermark image from the local absolute or relative file path.
   * @param options The options of the watermark image to be added. See `WatermarkOptions`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int addVideoWatermark(const char* watermarkUrl, const WatermarkOptions& options) = 0;

  /**
   * @brief Adds a watermark image to the local video.
   *
   * @since 4.6.0
   *
   * @details
   * You can use this method to overlay a watermark image on the local video stream, and configure the
   * watermark's position, size, and visibility in the preview using `WatermarkConfig`.
   *
   * @param configs Watermark configuration. See `WatermarkConfig`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int addVideoWatermark(const WatermarkConfig& configs) = 0;

  /**
   * @brief Removes the watermark image from the local video.
   *
   * @since 4.6.0
   *
   * @details
   * This method removes a previously added watermark image from the local video stream using the
   * specified unique ID.
   *
   * @param id The ID of the watermark to be removed. This value should match the ID used when the
   * watermark was added.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int removeVideoWatermark(const char* id) = 0;

  /**
   * @brief Removes the watermark image from the video stream.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int clearVideoWatermarks() = 0;

  // The following APIs are either deprecated and going to deleted.

  /** @deprecated Use disableAudio() instead.

   Disables the audio function in the channel.

   @return int
   - 0: Success.
   - < 0: Failure.
   */
   virtual int pauseAudio() __deprecated = 0;
   /** @deprecated Use enableAudio() instead.

   Resumes the audio function in the channel.

   @return int
   - 0: Success.
   - < 0: Failure.
   */
   virtual int resumeAudio() __deprecated = 0;

  /**
   * @brief Enables interoperability with the Agora Web SDK (applicable only in the live streaming
   *
   * @deprecated The Agora NG SDK enables the interoperablity with the Web SDK.
   * scenarios).
   *
   * @details
   * You can call this method to enable or disable interoperability with the Agora Web SDK. If a
   * channel has Web SDK users, ensure that you call this method, or the video of the Native user will
   * be a black screen for the Web user.
   * This method is only applicable in live streaming scenarios, and interoperability is enabled by
   * default in communication scenarios.
   *
   * @param enabled Whether to enable interoperability:
   * - `true`: Enable interoperability.
   * - `false`: (Default) Disable interoperability.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int enableWebSdkInteroperability(bool enabled) __deprecated = 0;

  /**
   * @brief Reports customized messages.
   *
   * @details
   * Agora supports reporting and analyzing customized messages. This function is in the beta stage
   * with a free trial. The ability provided in its beta test version is reporting a maximum of 10
   * message pieces within 6 seconds, with each message piece not exceeding 256 bytes and each string
   * not exceeding 100 bytes. To try out this function, contact `support@agora.io` and discuss the
   * format of customized messages with us.
   *
   */
  virtual int sendCustomReportMessage(const char* id, const char* category, const char* event, const char* label, int value) = 0;

  /**
   * @brief Registers the metadata observer.
   *
   * @details
   * You need to implement the `IMetadataObserver` class and specify the metadata type in this method.
   * This method enables you to add synchronized metadata in the video stream for more diversified
   *  live interactive streaming, such as sending shopping links, digital coupons, and online quizzes.
   * A successful call of this method triggers the `getMaxMetadataSize` callback.
   *
   * @note Call this method before `joinChannel(const char* token, const char* channelId, uid_t uid,
   * const ChannelMediaOptions& options)`.
   *
   * @param observer The metadata observer. See `IMetadataObserver`.
   * @param type The metadata type. The SDK currently only supports `VIDEO_METADATA`. See
   * `METADATA_TYPE`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int registerMediaMetadataObserver(IMetadataObserver *observer, IMetadataObserver::METADATA_TYPE type) = 0;

  /**
   * @brief Unregisters the specified metadata observer.
   *
   * @param observer The metadata observer. See `IMetadataObserver`.
   * @param type The metadata type. The SDK currently only supports `VIDEO_METADATA`. See
   * `METADATA_TYPE`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int unregisterMediaMetadataObserver(IMetadataObserver* observer, IMetadataObserver::METADATA_TYPE type) = 0;

  /** Start audio frame dump.

   Optional `location` is: "pre_apm_proc", "apm", "pre_send_proc", "filter", "enc", "tx_mixer",
                         "at_record", "atw_record" for audio sending.
                         "dec", "mixed", "play", "rx_mixer", "playback_mixer", "pcm_source_playback_mixer",
                         "pre_play_proc", "at_playout", "atw_playout" for audio receiving.

   */
  virtual int startAudioFrameDump(const char* channel_id, uid_t uid, const char* location, const char* uuid, const char* passwd, long duration_ms, bool auto_upload) = 0;

  /**
   * Stops the audio frame dump.
   */
  virtual int stopAudioFrameDump(const char* channel_id, uid_t uid, const char* location) = 0;

  /**
   * @brief Sets whether to enable the AI ​​noise suppression function and set the noise suppression
   * mode.
   *
   * @details
   * You can call this method to enable AI noise suppression function. Once enabled, the SDK
   * automatically detects and reduces stationary and non-stationary noise from your audio on the
   * premise of ensuring the quality of human voice. Stationary noise refers to noise signal with
   * constant average statistical properties and negligibly small fluctuations of level within the
   * period of observation. Common sources of stationary noises are:
   * - Television;
   * - Air conditioner;
   * - Machinery, etc.
   * Non-stationary noise refers to noise signal with huge fluctuations of level within the period of
   * observation; common sources of non-stationary noises are:
   * - Thunder;
   * - Explosion;
   * - Cracking, etc.
   * Applicable scenarios: In scenarios such as co-streaming, online education and video meeting, this
   * function can detect and reduce background noises to improve experience.
   * Call timing: You can call this method either before or after joining a channel.
   *
   * @note
   * - This method relies on the AI noise suppression dynamic library
   * `libagora_ai_noise_suppression_extension.dll`. If the dynamic library is deleted, the function
   * cannot be enabled.
   * - Agora does not recommend enabling this function on devices running Android 6.0 and below.
   *
   * @param enabled Whether to enable the AI noise suppression function:
   * - `true`: Enable the AI noise suppression.
   * - `false`: (Default) Disable the AI noise suppression.
   * @param mode The AI noise suppression modes. See `AUDIO_AINS_MODE`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setAINSMode(bool enabled,  AUDIO_AINS_MODE mode) = 0;

  /**
   * @brief Registers a user account.
   *
   * @details
   * Once registered, the user account can be used to identify the local user when the user joins the
   * channel. After the registration is successful, the user account can identify the identity of the
   * local user, and the user can use it to join the channel.
   * This method is optional. If you want to join a channel using a user account, you can choose one
   * of the following methods:
   * - Call the `registerLocalUserAccount` method to register a user account, and then call the
   * `joinChannelWithUserAccount(const char* token, const char* channelId, const char* userAccount)`
   * or `joinChannelWithUserAccount(const char* token, const char* channelId, const char* userAccount,
   * const ChannelMediaOptions& options)` method to join a
   * channel, which can shorten the time it takes to enter the channel.
   * - Call the `joinChannelWithUserAccount(const char* token, const char* channelId, const char*
   * userAccount)` or `joinChannelWithUserAccount(const char* token, const char* channelId, const
   * char* userAccount, const ChannelMediaOptions& options)` method to
   * join a channel.
   * Related callbacks: After successfully calling this method, the `onLocalUserRegistered` callback
   * will be triggered on the local client to report the local user's UID and user account.
   *
   * @note
   * - Starting from v4.6.0, the SDK will no longer automatically map Int UID to the String
   * `userAccount` used when registering a User Account. If you want to join a channel with the
   * original String `userAccount` used during registration, call the
   * `joinChannelWithUserAccount(const char* token, const char* channelId, const char* userAccount,
   * const ChannelMediaOptions& options)`
   * method to join the channel, instead of calling `joinChannel(const char* token, const char*
   * channelId, uid_t uid, const ChannelMediaOptions& options)` and pass in the Int UID
   * obtained through this method
   * - Ensure that the `userAccount` is unique in the channel.
   * - To ensure smooth communication, use the same parameter type to identify the user. For example,
   * if a user joins the channel with a UID, then ensure all the other users use the UID too. The same
   * applies to the user account. If a user joins the channel with the Agora Web SDK, ensure that the
   * ID of the user is set to the same parameter type.
   *
   * @param appId The App ID of your project on Agora Console.
   * @param userAccount The user account. This parameter is used to identify the user in the channel
   * for real-time audio and video engagement. You need to set and manage user accounts yourself and
   * ensure that each user account in the same channel is unique. The maximum length of this parameter
   * is 255 bytes. Ensure that you set this parameter and do not set it as NULL. Supported characters
   * are as follow(89 in total):
   * - The 26 lowercase English letters: a to z.
   * - The 26 uppercase English letters: A to Z.
   * - All numeric characters: 0 to 9.
   * - Space
   * - "!", "#", "$", "%", "&", "(", ")", "+", "-", ":", ";", "<", "=", ".", ">", "?", "@", "[", "]",
   * "^", "_", "{", "}", "|", "~", ","
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int registerLocalUserAccount(const char* appId, const char* userAccount) = 0;

  /**
   * @brief Joins a channel with a User Account and Token.
   *
   * @details
   * Before calling this method, if you have not called `registerLocalUserAccount` to register a user
   * account, when you call this method to join a channel, the SDK automatically creates a user
   * account for you. Calling the `registerLocalUserAccount` method to register a user account, and
   * then calling this method to join a channel can shorten the time it takes to enter the channel.
   * Once a user joins the channel, the user subscribes to the audio and video streams of all the
   * other users in the channel by default, giving rise to usage and billings. To stop subscribing to
   * a specified stream or all remote streams, call the corresponding `mute` methods.
   * Call timing: Call this method after `initialize`.
   * Related callbacks: After the user successfully joins the channel, the SDK triggers the following
   * callbacks:
   * - The local client: `onLocalUserRegistered`, `onJoinChannelSuccess` and
   * `onConnectionStateChanged` callbacks.
   * - The remote client: The `onUserJoined` and `onUserInfoUpdated` callbacks if a user joins the
   * channel in the COMMUNICATION profile, or if a host joins the channel in the LIVE_BROADCASTING
   * profile.
   *
   * @note
   * - This method only supports users joining one channel at a time.
   * - Users with different App IDs cannot call each other.
   * - Before joining a channel, ensure that the App ID you use to generate a token is the same as
   * that you pass in the `initialize` method; otherwise, you may fail to join the channel with the
   * token.
   * To ensure smooth communication, use the same parameter type to identify the user. For example, if
   * a user joins the channel with a UID, then ensure all the other users use the UID too. The same
   * applies to the user account. If a user joins the channel with the Agora Web SDK, ensure that the
   * ID of the user is set to the same parameter type.
   *
   * @param token The token generated on your server for authentication. See .Note:
   * - (Recommended) If your project has enabled the security mode (using APP ID and Token for
   * authentication), this parameter is required.
   * - If you have only enabled the testing mode (using APP ID for authentication), this parameter is
   * optional. You will automatically exit the channel 24 hours after successfully joining in.
   * - If you need to join different channels at the same time or switch between channels, Agora
   * recommends using a wildcard token so that you don't need to apply for a new token every time
   * joining a channel. See `Secure authentication with tokens`.
   * @param channelId The channel name. This parameter signifies the channel in which users engage in
   * real-time audio and video interaction. Under the premise of the same App ID, users who fill in
   * the same channel ID enter the same channel for audio and video interaction. The string length
   * must be less than 64 bytes. Supported characters (89 characters in total):
   * - All lowercase English letters: a to z.
   * - All uppercase English letters: A to Z.
   * - All numeric characters: 0 to 9.
   * - "!", "#", "$", "%", "&", "(", ")", "+", "-", ":", ";", "<", "=", ".", ">", "?", "@", "[", "]",
   * "^", "_", "{", "}", "|", "~", ","
   * @param userAccount The user account. This parameter is used to identify the user in the channel
   * for real-time audio and video engagement. You need to set and manage user accounts yourself and
   * ensure that each user account in the same channel is unique. The maximum length of this parameter
   * is 255 bytes. Ensure that you set this parameter and do not set it as NULL. Supported characters
   * are as follows(89 in total):
   * - The 26 lowercase English letters: a to z.
   * - The 26 uppercase English letters: A to Z.
   * - All numeric characters: 0 to 9.
   * - Space
   * - "!", "#", "$", "%", "&", "(", ")", "+", "-", ":", ";", "<", "=", ".", ">", "?", "@", "[", "]",
   * "^", "_", "{", "}", "|", "~", ","
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   *   - -2: The parameter is invalid. For example, the token is invalid, the `uid` parameter is not
   * set to an integer, or the value of a member in `ChannelMediaOptions` is invalid. You need to pass
   * in a valid parameter and join the channel again.
   *   - -3: Fails to initialize the `IRtcEngine` object. You need to reinitialize the `IRtcEngine`
   * object.
   *   - -7: The `IRtcEngine` object has not been initialized. You need to initialize the `IRtcEngine`
   * object before calling this method.
   *   - -8: The internal state of the `IRtcEngine` object is wrong. The typical cause is that after
   * calling `startEchoTest` to start a call loop test, you call this method to join the channel
   * without calling `stopEchoTest` to stop the test. You need to call `stopEchoTest` before calling
   * this method.
   *   - -17: The request to join the channel is rejected. The typical cause is that the user is
   * already in the channel. Agora recommends that you use the `onConnectionStateChanged` callback to
   * see whether the user is in the channel. Do not call this method to join the channel unless you
   * receive the `CONNECTION_STATE_DISCONNECTED` (1) state.
   *   - -102: The channel name is invalid. You need to pass in a valid channel name in `channelId` to
   * rejoin the channel.
   *   - -121: The user ID is invalid. You need to pass in a valid user ID in `uid` to rejoin the
   * channel.
   */
  virtual int joinChannelWithUserAccount(const char* token, const char* channelId, const char* userAccount) = 0;

  /**
   * @brief Join a channel using a user account and token, and set the media options.
   *
   * @details
   * Before calling this method, if you have not called `registerLocalUserAccount` to register a user
   * account, when you call this method to join a channel, the SDK automatically creates a user
   * account for you. Calling the `registerLocalUserAccount` method to register a user account, and
   * then calling this method to join a channel can shorten the time it takes to enter the channel.
   * Compared to `joinChannelWithUserAccount(const char* token, const char* channelId, const char*
   * userAccount)`, this method has the `options` parameter which is
   * used to set media options, such as whether to publish audio and video streams within a channel.
   * By default, the user subscribes to the audio and video streams of all the other users in the
   * channel, giving rise to usage and **billings**. To stop subscribing to other streams, set the
   * `options` parameter or call the corresponding `mute` methods.
   * Call timing: Call this method after `initialize`.
   * Related callbacks: After the user successfully joins the channel, the SDK triggers the following
   * callbacks:
   * - The local client: `onLocalUserRegistered`, `onJoinChannelSuccess` and
   * `onConnectionStateChanged` callbacks.
   * - The remote client: The `onUserJoined` and `onUserInfoUpdated` callbacks if a user joins the
   * channel in the COMMUNICATION profile, or if a host joins the channel in the LIVE_BROADCASTING
   * profile.
   *
   * @note
   * - This method only supports users joining one channel at a time.
   * - Users with different App IDs cannot call each other.
   * - Before joining a channel, ensure that the App ID you use to generate a token is the same as
   * that you pass in the `initialize` method; otherwise, you may fail to join the channel with the
   * token.
   * To ensure smooth communication, use the same parameter type to identify the user. For example, if
   * a user joins the channel with a UID, then ensure all the other users use the UID too. The same
   * applies to the user account. If a user joins the channel with the Agora Web SDK, ensure that the
   * ID of the user is set to the same parameter type.
   *
   * @param token The token generated on your server for authentication. See .Note:
   * - (Recommended) If your project has enabled the security mode (using APP ID and Token for
   * authentication), this parameter is required.
   * - If you have only enabled the testing mode (using APP ID for authentication), this parameter is
   * optional. You will automatically exit the channel 24 hours after successfully joining in.
   * - If you need to join different channels at the same time or switch between channels, Agora
   * recommends using a wildcard token so that you don't need to apply for a new token every time
   * joining a channel. See `Secure authentication with tokens`.
   * @param channelId The channel name. This parameter signifies the channel in which users engage in
   * real-time audio and video interaction. Under the premise of the same App ID, users who fill in
   * the same channel ID enter the same channel for audio and video interaction. The string length
   * must be less than 64 bytes. Supported characters (89 characters in total):
   * - All lowercase English letters: a to z.
   * - All uppercase English letters: A to Z.
   * - All numeric characters: 0 to 9.
   * - "!", "#", "$", "%", "&", "(", ")", "+", "-", ":", ";", "<", "=", ".", ">", "?", "@", "[", "]",
   * "^", "_", "{", "}", "|", "~", ","
   * @param userAccount The user account. This parameter is used to identify the user in the channel
   * for real-time audio and video engagement. You need to set and manage user accounts yourself and
   * ensure that each user account in the same channel is unique. The maximum length of this parameter
   * is 255 bytes. Ensure that you set this parameter and do not set it as NULL. Supported characters
   * are as follows(89 in total):
   * - The 26 lowercase English letters: a to z.
   * - The 26 uppercase English letters: A to Z.
   * - All numeric characters: 0 to 9.
   * - Space
   * - "!", "#", "$", "%", "&", "(", ")", "+", "-", ":", ";", "<", "=", ".", ">", "?", "@", "[", "]",
   * "^", "_", "{", "}", "|", "~", ","
   * @param options The channel media options. See `ChannelMediaOptions`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   *   - -2: The parameter is invalid. For example, the token is invalid, the `uid` parameter is not
   * set to an integer, or the value of a member in `ChannelMediaOptions` is invalid. You need to pass
   * in a valid parameter and join the channel again.
   *   - -3: Fails to initialize the `IRtcEngine` object. You need to reinitialize the `IRtcEngine`
   * object.
   *   - -7: The `IRtcEngine` object has not been initialized. You need to initialize the `IRtcEngine`
   * object before calling this method.
   *   - -8: The internal state of the `IRtcEngine` object is wrong. The typical cause is that after
   * calling `startEchoTest` to start a call loop test, you call this method to join the channel
   * without calling `stopEchoTest` to stop the test. You need to call `stopEchoTest` before calling
   * this method.
   *   - -17: The request to join the channel is rejected. The typical cause is that the user is
   * already in the channel. Agora recommends that you use the `onConnectionStateChanged` callback to
   * see whether the user is in the channel. Do not call this method to join the channel unless you
   * receive the `CONNECTION_STATE_DISCONNECTED` (1) state.
   *   - -102: The channel name is invalid. You need to pass in a valid channel name in `channelId` to
   * rejoin the channel.
   *   - -121: The user ID is invalid. You need to pass in a valid user ID in `uid` to rejoin the
   * channel.
   */
  virtual int joinChannelWithUserAccount(const char* token, const char* channelId, const char* userAccount, const ChannelMediaOptions& options) = 0;

  /**
   * @brief Join a channel using a user account and token, and set the media options.
   *
   * @details
   * Before calling this method, if you have not called `registerLocalUserAccount` to register a user
   * account, when you call this method to join a channel, the SDK automatically creates a user
   * account for you. Calling the `registerLocalUserAccount` method to register a user account, and
   * then calling this method to join a channel can shorten the time it takes to enter the channel.
   * Once a user joins the channel, the user subscribes to the audio and video streams of all the
   * other users in the channel by default, giving rise to usage and **billings**. If you want to stop
   * subscribing to the media stream of other users, you can set the `options` parameter or call the
   * corresponding `mute` method.
   * Call timing: Call this method after `initialize`.
   * Related callbacks: After the user successfully joins the channel, the SDK triggers the following
   * callbacks:
   * - The local client: `onLocalUserRegistered`, `onJoinChannelSuccess` and
   * `onConnectionStateChanged` callbacks.
   * - The remote client: The `onUserJoined` and `onUserInfoUpdated` callbacks if a user joins the
   * channel in the COMMUNICATION profile, or if a host joins the channel in the LIVE_BROADCASTING
   * profile.
   *
   * @note
   * - This method only supports users joining one channel at a time.
   * - Users with different App IDs cannot call each other.
   * - Before joining a channel, ensure that the App ID you use to generate a token is the same as
   * that you pass in the `initialize` method; otherwise, you may fail to join the channel with the
   * token.
   * To ensure smooth communication, use the same parameter type to identify the user. For example, if
   * a user joins the channel with a UID, then ensure all the other users use the UID too. The same
   * applies to the user account. If a user joins the channel with the Agora Web SDK, ensure that the
   * ID of the user is set to the same parameter type.
   *
   * @param token The token generated on your server for authentication. See .Note:
   * - (Recommended) If your project has enabled the security mode (using APP ID and Token for
   * authentication), this parameter is required.
   * - If you have only enabled the testing mode (using APP ID for authentication), this parameter is
   * optional. You will automatically exit the channel 24 hours after successfully joining in.
   * - If you need to join different channels at the same time or switch between channels, Agora
   * recommends using a wildcard token so that you don't need to apply for a new token every time
   * joining a channel. See `Secure authentication with tokens`.
   * @param channelId The channel name. This parameter signifies the channel in which users engage in
   * real-time audio and video interaction. Under the premise of the same App ID, users who fill in
   * the same channel ID enter the same channel for audio and video interaction. The string length
   * must be less than 64 bytes. Supported characters (89 characters in total):
   * - All lowercase English letters: a to z.
   * - All uppercase English letters: A to Z.
   * - All numeric characters: 0 to 9.
   * - "!", "#", "$", "%", "&", "(", ")", "+", "-", ":", ";", "<", "=", ".", ">", "?", "@", "[", "]",
   * "^", "_", "{", "}", "|", "~", ","
   * @param userAccount The user account. This parameter is used to identify the user in the channel
   * for real-time audio and video engagement. You need to set and manage user accounts yourself and
   * ensure that each user account in the same channel is unique. The maximum length of this parameter
   * is 255 bytes. Ensure that you set this parameter and do not set it as NULL. Supported characters
   * are as follows(89 in total):
   * - The 26 lowercase English letters: a to z.
   * - The 26 uppercase English letters: A to Z.
   * - All numeric characters: 0 to 9.
   * - Space
   * - "!", "#", "$", "%", "&", "(", ")", "+", "-", ":", ";", "<", "=", ".", ">", "?", "@", "[", "]",
   * "^", "_", "{", "}", "|", "~", ","
   * @param options The channel media options. See `ChannelMediaOptions`.
   * @param eventHandler The callback class of `IRtcEngineEx`. See `IRtcEngineEventHandler`. You can
   * get the callback events of multiple channels through the `eventHandler` object passed in this
   * parameter.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   *   - -2: The parameter is invalid. For example, the token is invalid, the `uid` parameter is not
   * set to an integer, or the value of a member in `ChannelMediaOptions` is invalid. You need to pass
   * in a valid parameter and join the channel again.
   *   - -3: Fails to initialize the `IRtcEngine` object. You need to reinitialize the `IRtcEngine`
   * object.
   *   - -7: The `IRtcEngine` object has not been initialized. You need to initialize the `IRtcEngine`
   * object before calling this method.
   *   - -8: The internal state of the `IRtcEngine` object is wrong. The typical cause is that after
   * calling `startEchoTest` to start a call loop test, you call this method to join the channel
   * without calling `stopEchoTest` to stop the test. You need to call `stopEchoTest` before calling
   * this method.
   *   - -17: The request to join the channel is rejected. The typical cause is that the user is
   * already in the channel. Agora recommends that you use the `onConnectionStateChanged` callback to
   * see whether the user is in the channel. Do not call this method to join the channel unless you
   * receive the `CONNECTION_STATE_DISCONNECTED` (1) state.
   *   - -102: The channel name is invalid. You need to pass in a valid channel name in `channelId` to
   * rejoin the channel.
   *   - -121: The user ID is invalid. You need to pass in a valid user ID in `uid` to rejoin the
   * channel.
   */
  virtual int joinChannelWithUserAccountEx(const char* token, const char* channelId,
                                           const char* userAccount, const ChannelMediaOptions& options,
                                           IRtcEngineEventHandler* eventHandler) = 0;

  /**
   * @brief Gets the user information by passing in the user account.
   *
   * @details
   * After a remote user joins the channel, the SDK gets the UID and user account of the remote user,
   * caches them in a mapping table object, and triggers the `onUserInfoUpdated` callback on the local
   * client. After receiving the callback, you can call this method and pass in the user account to
   * get the UID of the remote user from the `UserInfo` object.
   * Call timing: Call this method after receiving the `onUserInfoUpdated` callback.
   * Related callbacks: `onUserInfoUpdated`
   *
   * @param userAccount The user account.
   * @param userInfo Input and output parameter. The `UserInfo` object that identifies the user
   * information.
   * - Input value: A `UserInfo` object.
   * - Output: A `UserInfo` object that contains both the user account and UID.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int getUserInfoByUserAccount(const char* userAccount, rtc::UserInfo* userInfo) = 0;

  /**
   * @brief Gets the user information by passing in the user ID.
   *
   * @details
   * After a remote user joins the channel, the SDK gets the UID and user account of the remote user,
   * caches them in a mapping table object, and triggers the `onUserInfoUpdated` callback on the local
   * client. After receiving the callback, you can call this method and passi in the UID.to get the
   * user account of the specified user from the `UserInfo` object.
   * Call timing: Call this method after receiving the `onUserInfoUpdated` callback.
   * Related callbacks: `onUserInfoUpdated`
   *
   * @param uid The user ID.
   * @param userInfo Input and output parameter. The `UserInfo` object that identifies the user
   * information.
   * - Input value: A `UserInfo` object.
   * - Output: A `UserInfo` object that contains both the user account and UID.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int getUserInfoByUid(uid_t uid, rtc::UserInfo* userInfo) = 0;

  /**
   * @brief Starts relaying media streams across channels or updates channels for media relay.
   *
   * @since v4.2.0
   *
   * @details
   * The first successful call to this method starts relaying media streams from the source channel to
   * the destination channels. To relay the media stream to other channels, or exit one of the current
   * media relays, you can call this method again to update the destination channels. This feature
   * supports relaying media streams to a maximum of six destination channels.
   * After a successful method call, the SDK triggers the `onChannelMediaRelayStateChanged` callback,
   * and this callback returns the state of the media stream relay. Common states are as follows:
   * - If the `onChannelMediaRelayStateChanged` callback returns `RELAY_STATE_RUNNING` (2) and
   * `RELAY_OK` (0), it means that the SDK starts relaying media streams from the source channel to
   * the destination channel.
   * - If the `onChannelMediaRelayStateChanged` callback returns `RELAY_STATE_FAILURE` (3), an
   * exception occurs during the media stream relay.
   *
   * @note
   * - Call this method after joining the channel.
   * - This method takes effect only when you are a host in a live streaming channel.
   * - The relaying media streams across channels function needs to be enabled by contacting
   * `technical support`.
   * - Agora does not support string user accounts in this API.
   *
   * @param configuration The configuration of the media stream relay. See
   * `ChannelMediaRelayConfiguration`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   *   - -1: A general error occurs (no specified reason).
   *   - -2: The parameter is invalid.
   *   - -8: Internal state error. Probably because the user is not a broadcaster.
   */
  virtual int startOrUpdateChannelMediaRelay(const ChannelMediaRelayConfiguration &configuration) = 0;

  /**
   * @brief Stops the media stream relay. Once the relay stops, the host quits all the target
   * channels.
   *
   * @details
   * After a successful method call, the SDK triggers the `onChannelMediaRelayStateChanged` callback.
   * If the callback reports `RELAY_STATE_IDLE` (0) and `RELAY_OK` (0), the host successfully stops
   * the relay.
   *
   * @note If the method call fails, the SDK triggers the `onChannelMediaRelayStateChanged` callback
   * with the `RELAY_ERROR_SERVER_NO_RESPONSE` (2) or `RELAY_ERROR_SERVER_CONNECTION_LOST` (8) status
   * code. You can call the `leaveChannel(const LeaveChannelOptions& options)` method to leave the
   * channel, and the media stream
   * relay automatically stops.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   *   - -5: The method call was rejected. There is no ongoing channel media relay.
   */
  virtual int stopChannelMediaRelay() = 0;

  /**
   * @brief Pauses the media stream relay to all target channels.
   *
   * @details
   * After the cross-channel media stream relay starts, you can call this method to pause relaying
   * media streams to all target channels; after the pause, if you want to resume the relay, call
   * `resumeAllChannelMediaRelay`.
   *
   * @note Call this method after `startOrUpdateChannelMediaRelay`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   *   - -5: The method call was rejected. There is no ongoing channel media relay.
   */
  virtual int pauseAllChannelMediaRelay() = 0;

  /**
   * @brief Resumes the media stream relay to all target channels.
   *
   * @details
   * After calling the `pauseAllChannelMediaRelay` method, you can call this method to resume relaying
   * media streams to all destination channels.
   *
   * @note Call this method after `pauseAllChannelMediaRelay`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   *   - -5: The method call was rejected. There is no paused channel media relay.
   */
  virtual int resumeAllChannelMediaRelay() = 0;

  /**
   * @brief Sets the audio profile of the audio streams directly pushed to the CDN by the host.
   *
   * @deprecated v4.6.0.
   *
   * @details
   * When you set the `publishMicrophoneTrack` or `publishCustomAudioTrack` in the
   * `DirectCdnStreamingMediaOptions` as `true` to capture audios, you can call this method to set the
   * audio profile.
   *
   * @param profile The audio profile, including the sampling rate, bitrate, encoding mode, and the
   * number of channels. See `AUDIO_PROFILE_TYPE`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setDirectCdnStreamingAudioConfiguration(AUDIO_PROFILE_TYPE profile) = 0;

  /**
   * @brief Sets the video profile of the media streams directly pushed to the CDN by the host.
   *
   * @deprecated v4.6.0.
   *
   * @details
   * This method only affects video streams captured by cameras or screens, or from custom video
   * capture sources. That is, when you set `publishCameraTrack` or `publishCustomVideoTrack` in
   * `DirectCdnStreamingMediaOptions` as `true` to capture videos, you can call this method to set the
   * video profiles.
   * If your local camera does not support the video resolution you set,the SDK automatically adjusts
   * the video resolution to a value that is closest to your settings for capture, encoding or
   * streaming, with the same aspect ratio as the resolution you set. You can get the actual
   * resolution of the video streams through the `onDirectCdnStreamingStats` callback.
   *
   * @param config Video profile. See `VideoEncoderConfiguration`.Note: During CDN live streaming,
   * Agora only supports setting `ORIENTATION_MODE` as `ORIENTATION_MODE_FIXED_LANDSCAPE` or
   * `ORIENTATION_MODE_FIXED_PORTRAIT`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setDirectCdnStreamingVideoConfiguration(const VideoEncoderConfiguration& config) = 0;

  /**
   * @brief Starts pushing media streams to the CDN directly.
   *
   * @deprecated v4.6.0.
   *
   * @details
   * Aogra does not support pushing media streams to one URL repeatedly.
   * **Media options**
   * Agora does not support setting the value of `publishCameraTrack` and `publishCustomVideoTrack` as
   * `true`, or the value of `publishMicrophoneTrack` and `publishCustomAudioTrack` as `true` at the
   * same time. When choosing media setting options ( `DirectCdnStreamingMediaOptions` ), you can
   * refer to the following examples:
   * If you want to push audio and video streams captured by the host from a custom source, the media
   * setting options should be set as follows:
   * - `publishCustomAudioTrack` is set as `true` and call the `pushAudioFrame` method
   * - `publishCustomVideoTrack` is set as `true` and call the `pushVideoFrame` method
   * - `publishCameraTrack` is set as `false` (the default value)
   * - `publishMicrophoneTrack` is set as `false` (the default value)
   * As of v4.2.0, Agora SDK supports audio-only live streaming. You can set `publishCustomAudioTrack`
   * or `publishMicrophoneTrack` in `DirectCdnStreamingMediaOptions` as `true` and call
   * `pushAudioFrame` to push audio streams.
   *
   * @note Agora only supports pushing one audio and video streams or one audio streams to CDN.
   *
   * @param eventHandler See `onDirectCdnStreamingStateChanged` and `onDirectCdnStreamingStats`.
   * @param publishUrl The CDN live streaming URL.
   * @param options The media setting options for the host. See `DirectCdnStreamingMediaOptions`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int startDirectCdnStreaming(IDirectCdnStreamingEventHandler* eventHandler,
                                      const char* publishUrl, const DirectCdnStreamingMediaOptions& options) = 0;

  /**
   * @brief Stops pushing media streams to the CDN directly.
   *
   * @deprecated v4.6.0.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int stopDirectCdnStreaming() = 0;

  /** Change the media source during the pushing
   *
   * @deprecated v4.6.0.
   *
   * @note
   * This method is temporarily not supported.
   *
   * @param options The direct cdn streaming media options: DirectCdnStreamingMediaOptions.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int updateDirectCdnStreamingMediaOptions(const DirectCdnStreamingMediaOptions& options) = 0;

  /**
   * @brief Enables the virtual metronome.
   *
   * @details
   * - After enabling the virtual metronome, the SDK plays the specified audio effect file from the
   * beginning, and controls the playback duration of each file according to `beatsPerMinute` you set
   * in `AgoraRhythmPlayerConfig`. For example, if you set `beatsPerMinute` as `60`, the SDK plays one
   * beat every second. If the file duration exceeds the beat duration, the SDK only plays the audio
   * within the beat duration.
   * - By default, the sound of the virtual metronome is published in the channel. If you want the
   * sound to be heard by the remote users, you can set `publishRhythmPlayerTrack` in
   * `ChannelMediaOptions` as `true`.
   * Applicable scenarios: In music education, physical education and other scenarios, teachers
   * usually need to use a metronome so that students can practice with the correct beat. The meter is
   * composed of a downbeat and upbeats. The first beat of each measure is called a downbeat, and the
   * rest are called upbeats.
   * Call timing: This method can be called either before or after joining the channel.
   * Related callbacks: After successfully calling this method, the SDK triggers the
   * `onRhythmPlayerStateChanged` callback locally to report the status of the virtual metronome.
   *
   * @param sound1 The absolute path or URL address (including the filename extensions) of the file
   * for the downbeat. For example, `C:\music\audio.mp4`. For the audio file formats supported by this
   * method, see `What formats of audio files does the Agora RTC SDK support`.
   * @param sound2 The absolute path or URL address (including the filename extensions) of the file
   * for the upbeats. For example, `C:\music\audio.mp4`. For the audio file formats supported by this
   * method, see `What formats of audio files does the Agora RTC SDK support`.
   * @param config The metronome configuration. See `AgoraRhythmPlayerConfig`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   *   - -22: Cannot find audio effect files. Please set the correct paths for `sound1` and `sound2`.
   */
  virtual int startRhythmPlayer(const char* sound1, const char* sound2, const AgoraRhythmPlayerConfig& config) = 0;

  /**
   * @brief Disables the virtual metronome.
   *
   * @details
   * After calling `startRhythmPlayer`, you can call this method to disable the virtual metronome.
   *
   * @note This method is for Android and iOS only.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int stopRhythmPlayer() = 0;

  /**
   * @brief Configures the virtual metronome.
   *
   * @details
   * - After calling `startRhythmPlayer`, you can call this method to reconfigure the virtual
   * metronome.
   * - After enabling the virtual metronome, the SDK plays the specified audio effect file from the
   * beginning, and controls the playback duration of each file according to `beatsPerMinute` you set
   * in `AgoraRhythmPlayerConfig`. For example, if you set `beatsPerMinute` as `60`, the SDK plays one
   * beat every second. If the file duration exceeds the beat duration, the SDK only plays the audio
   * within the beat duration.
   * - By default, the sound of the virtual metronome is published in the channel. If you want the
   * sound to be heard by the remote users, you can set `publishRhythmPlayerTrack` in
   * `ChannelMediaOptions` as `true`.
   * Call timing: This method can be called either before or after joining the channel.
   * Related callbacks: After successfully calling this method, the SDK triggers the
   * `onRhythmPlayerStateChanged` callback locally to report the status of the virtual metronome.
   *
   * @param config The metronome configuration. See `AgoraRhythmPlayerConfig`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int configRhythmPlayer(const AgoraRhythmPlayerConfig& config) = 0;

  /**
   * @brief Takes a snapshot of a video stream.
   *
   * @details
   * This method takes a snapshot of a video stream from the specified user, generates a JPG image,
   * and saves it to the specified path.
   * Call timing: Call this method after joining a channel.
   * Related callbacks: After a successful call of this method, the SDK triggers the `onSnapshotTaken`
   * callback to report whether the snapshot is successfully taken, as well as the details for that
   * snapshot.
   *
   * @note
   * - The method is asynchronous, and the SDK has not taken the snapshot when the method call
   * returns.
   * - When used for local video snapshots, this method takes a snapshot for the video streams
   * specified in `ChannelMediaOptions`.
   * - If the user's video has been preprocessed, for example, watermarked or beautified, the
   * resulting snapshot includes the pre-processing effect.
   *
   * @param uid The user ID. Set uid as 0 if you want to take a snapshot of the local user's video.
   * @param filePath The local path (including filename extensions) of the snapshot. For example:
   * - Windows: `C:\Users\<user_name>\AppData\Local\Agora\<process_name>\example.jpg`
   * - iOS: `/App Sandbox/Library/Caches/example.jpg`
   * - macOS: `～/Library/Logs/example.jpg`
   * - Android: `/storage/emulated/0/Android/data/<package name>/files/example.jpg`
   * Attention: Ensure that the path you specify exists and is writable.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int takeSnapshot(uid_t uid, const char* filePath)  = 0;

  /**
   * @brief Takes a screenshot of the video at the specified observation point.
   *
   * @details
   * This method takes a snapshot of a video stream from the specified user, generates a JPG image,
   * and saves it to the specified path.
   * Call timing: Call this method after joining a channel.
   * Related callbacks: After a successful call of this method, the SDK triggers the `onSnapshotTaken`
   * callback to report whether the snapshot is successfully taken, as well as the details for that
   * snapshot.
   *
   * @note
   * - The method is asynchronous, and the SDK has not taken the snapshot when the method call
   * returns.
   * - When used for local video snapshots, this method takes a snapshot for the video streams
   * specified in `ChannelMediaOptions`.
   *
   * @param uid The user ID. Set uid as 0 if you want to take a snapshot of the local user's video.
   * @param config The configuration of the snaptshot. See `SnapshotConfig`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int takeSnapshot(uid_t uid, const media::SnapshotConfig& config)  = 0;

  /**
   * @brief Enables or disables video screenshot and upload.
   *
   * @details
   * When video screenshot and upload function is enabled, the SDK takes screenshots and uploads
   * videos sent by local users based on the type and frequency of the module you set in
   * `ContentInspectConfig`. After video screenshot and upload, the Agora server sends the callback
   * notification to your app server in HTTPS requests and sends all screenshots to the third-party
   * cloud storage service.
   * Call timing: This method can be called either before or after joining the channel.
   *
   * @note
   * - Before calling this method, make sure you have enabled video screenshot and upload on Agora
   * console.
   * - When the video moderation module is set to video moderation via Agora self-developed extension(
   * `CONTENT_INSPECT_SUPERVISION` ), the video screenshot and upload dynamic library
   * `libagora_content_inspect_extension.dll` is required. Deleting this library disables the
   * screenshot and upload feature.
   *
   * @param enabled Whether to enalbe video screenshot and upload:
   * - `true`: Enables video screenshot and upload.
   * - `false`: Disables video screenshot and upload.
   * @param config Screenshot and upload configuration. See `ContentInspectConfig`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int enableContentInspect(bool enabled, const media::ContentInspectConfig &config) = 0;
  /**
   * @brief Adjusts the volume of the custom audio track played remotely.
   *
   * @details
   * If you want to change the volume of the audio played remotely, you need to call this method
   * again.
   *
   * @note Ensure you have called the `createCustomAudioTrack` method to create a custom audio track
   * before calling this method.
   *
   * @param trackId The audio track ID. Set this parameter to the custom audio track ID returned in
   * `createCustomAudioTrack`.
   * @param volume The volume of the audio source. The value can range from 0 to 100. 0 means mute;
   * 100 means the original volume.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int adjustCustomAudioPublishVolume(track_id_t trackId, int volume) = 0;

  /**
   * @brief Adjusts the volume of the custom audio track played locally.
   *
   * @details
   * If you want to change the volume of the audio to be played locally, you need to call this method
   * again.
   *
   * @note Ensure you have called the `createCustomAudioTrack` method to create a custom audio track
   * before calling this method.
   *
   * @param trackId The audio track ID. Set this parameter to the custom audio track ID returned in
   * `createCustomAudioTrack`.
   * @param volume The volume of the audio source. The value can range from 0 to 100. 0 means mute;
   * 100 means the original volume.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int adjustCustomAudioPlayoutVolume(track_id_t trackId, int volume) = 0;

  /**
   * @brief Sets up cloud proxy service.
   *
   * @since v3.3.0
   *
   * @details
   * When users' network access is restricted by a firewall, configure the firewall to allow specific
   * IP addresses and ports provided by Agora; then, call this method to enable the cloud `proxyType`
   * and set the cloud proxy type with the proxyType parameter.
   * After successfully connecting to the cloud proxy, the SDK triggers the `onConnectionStateChanged`
   * ( CONNECTION_STATE_CONNECTING, CONNECTION_CHANGED_SETTING_PROXY_SERVER ) callback.
   * To disable the cloud proxy that has been set, call the `setCloudProxy(NONE_PROXY)`.
   * To change the cloud proxy type that has been set, call the `setCloudProxy` `(NONE_PROXY) ` first,
   * and then call the `setCloudProxy` to set the `proxyType` you want.
   *
   * @note
   * - Agora recommends that you call this method before joining a channel.
   * - When a user is behind a firewall and uses the Force UDP cloud proxy, the services for Media
   * Push and cohosting across channels are not available.
   * - When you use the Force TCP cloud proxy, note that an error would occur when calling the
   * `startAudioMixing(const char* filePath, bool loopback, int cycle, int startPos)` method to play
   * online music files in the HTTP protocol. The services for
   * Media Push and cohosting across channels use the cloud proxy with the TCP protocol.
   *
   * @param proxyType The type of the cloud proxy. See `CLOUD_PROXY_TYPE`.
   * This parameter is mandatory. The SDK reports an error if you do not pass in a value.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   *   - -2: The parameter is invalid.
   *   - -7: The SDK is not initialized.
   */
  virtual int setCloudProxy(CLOUD_PROXY_TYPE proxyType) = 0;
  /**
   * @brief Configures the connection to Agora's Private Media Server access module.
   *
   * @details
   * After successfully deploying the Agora Private Media Server and integrating the 4.x RTC SDK on
   * intranet clients, you can call this method to specify the Local Access Point and assign the
   * access module to the SDK.
   * Call timing: This method must be called before joining a channel.
   *
   * @note This method takes effect only after deploying the Agora Hybrid Cloud solution. You can
   * `contact sales` to learn more and deploy the Agora Hybrid Cloud.
   *
   * @param config Local Access Point configuration. See `LocalAccessPointConfiguration`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure. See `Error Codes` for details and resolution suggestions.
   */
  virtual int setLocalAccessPoint(const LocalAccessPointConfiguration& config) = 0;

  /**
   * @brief Sets audio advanced options.
   *
   * @details
   * If you have advanced audio processing requirements, such as capturing and sending stereo audio,
   * you can call this method to set advanced audio options.
   *
   * @note Call this method after calling `joinChannel(const char* token, const char* channelId, uid_t
   * uid, const ChannelMediaOptions& options)`, `enableAudio` and `enableLocalAudio`.
   *
   * @param options The advanced options for audio. See `AdvancedAudioOptions`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setAdvancedAudioOptions(AdvancedAudioOptions& options, int sourceType = 0) = 0;

  /**
   * @brief Sets audio-video synchronization for the sender.
   *
   * @details
   * A user may use two separate devices to send audio and video streams. To ensure audio and video
   * are synchronized on the receiving end, you can call this method on the video sender and pass in
   * the channel name and user ID of the audio sender. The SDK
   * will use the timestamp of the audio stream as the reference to automatically adjust the video
   * stream, ensuring audio-video synchronization even if the two sending devices are on different
   * uplink networks (e.g., Wi-Fi and 4G).
   *
   * @note Agora recommends that you call this method before joining a channel.
   *
   * @param channelId The channel name of the audio sender.
   * @param uid The user ID of the audio sender.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure. See `Error Codes` for details and resolution suggestions.
   */
  virtual int setAVSyncSource(const char* channelId, uid_t uid) = 0;

  /**
   * @brief Sets whether to replace the current video feeds with images when publishing video streams.
   *
   * @details
   * When publishing video streams, you can call this method to replace the current video feeds with
   * custom images.
   * Once you enable this function, you can select images to replace the video feeds through the
   * `ImageTrackOptions` parameter. If you disable this function, the remote users see the video feeds
   * that you publish.
   * Call timing: Call this method after joining a channel.
   *
   * @param enable Whether to replace the current video feeds with custom images:
   * - `true`: Replace the current video feeds with custom images.
   * - `false`: (Default) Do not replace the current video feeds with custom images.
   * @param options Image configurations. See `ImageTrackOptions`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int enableVideoImageSource(bool enable, const ImageTrackOptions& options) = 0;

  /**
   * @brief Gets the current Monotonic Time of the SDK.
   *
   * @details
   * Monotonic Time refers to a monotonically increasing time series whose value increases over time.
   * The unit is milliseconds.
   * In custom video capture and custom audio capture scenarios, in order to ensure audio and video
   * synchronization, Agora recommends that you call this method to obtain the current Monotonic Time
   * of the SDK, and then pass this value into the timestamp parameter in the captured video frame (
   * `VideoFrame` ) and audio frame ( `AudioFrame` ).
   * Call timing: This method can be called either before or after joining the channel.
   *
   * @return
   * - ≥0: The method call is successful, and returns the current Monotonic Time of the SDK (in
   * milliseconds).
   * - < 0: Failure.
   */
  virtual int64_t getCurrentMonotonicTimeInMs() = 0;

  /**
   * @brief Gets the type of the local network connection.
   *
   * @details
   * You can use this method to get the type of network in use at any stage.
   *
   * @note You can call this method either before or after joining a channel.
   *
   * @return
   * - ≥ 0: The method call is successful, and the local network connection type is returned.
   *   - 0: The SDK disconnects from the network.
   *   - 1: The network type is LAN.
   *   - 2: The network type is Wi-Fi (including hotspots).
   *   - 3: The network type is mobile 2G.
   *   - 4: The network type is mobile 3G.
   *   - 5: The network type is mobile 4G.
   *   - 6: The network type is mobile 5G.
   * - < 0: The method call failed with an error code.
   *   - -1: The network type is unknown.
   */
  virtual int getNetworkType() = 0;

  /**
   * @brief Provides technical preview functionalities or special customizations by configuring the
   * SDK with JSON options.
   *
   * @details
   * Contact `technical support` to get the JSON configuration method.
   *
   * @param parameters Pointer to the set parameters in a JSON string.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setParameters(const char* parameters) = 0;

  /**
   * @brief Enables tracing the video frame rendering process.
   *
   * @since v4.1.1
   *
   * @details
   * The SDK starts tracing the rendering status of the video frames in the channel from the moment
   * this method is successfully called and reports information about the event through the
   * `onVideoRenderingTracingResult` callback.
   * Applicable scenarios: Agora recommends that you use this method in conjunction with the UI
   * settings (such as buttons and sliders) in your app to improve the user experience. For example,
   * call this method when the user clicks the Join Channel button, and then get the time spent during
   * the video frame rendering process through the `onVideoRenderingTracingResult` callback, so as to
   * optimize the indicators accordingly.
   *
   * @note
   * - If you have not called this method, the SDK tracks the rendering events of the video frames
   * from the moment you call `joinChannel(const char* token, const char* channelId, uid_t uid, const
   * ChannelMediaOptions& options)` to join the channel. You can call this method at an
   * appropriate time according to the actual application scenario to set the starting position for
   * tracking video rendering events.
   * - After the local user leaves the current channel, the SDK automatically tracks the video
   * rendering events from the moment you join a channel.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   *   - -7: The method is called before `IRtcEngine` is initialized.
   */
  virtual int startMediaRenderingTracing() = 0;

  /**
   * @brief Enables audio and video frame instant rendering.
   *
   * @since v4.1.1
   *
   * @details
   * After successfully calling this method, the SDK enables the instant frame rendering mode, which
   * can speed up the first frame rendering after the user joins the channel.
   * Applicable scenarios: Agora recommends that you enable this mode for the audience in a live
   * streaming scenario.
   * Call timing: Call this method before joining a channel.
   *
   * @note
   * Both the host and audience member need to call this method in order to experience instant
   * rendering of audio and video frames.
   * Once the method is successfully called, you can only cancel instant rendering by calling
   * `release` to destroy the `IRtcEngine` object.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   *   - -7: The method is called before `IRtcEngine` is initialized.
   */
  virtual int enableInstantMediaRendering() = 0;

  /**
   * @brief Gets the current NTP (Network Time Protocol) time.
   *
   * @details
   * In the real-time chorus scenario, especially when the downlink connections are inconsistent due
   * to network issues among multiple receiving ends, you can call this method to obtain the current
   * NTP time as the reference time, in order to align the lyrics and music of multiple receiving ends
   * and achieve chorus synchronization.
   *
   * @return
   * The Unix timestamp (ms) of the current NTP time.
   */
  virtual uint64_t getNtpWallTimeInMs() = 0;

  /**
   * @brief Checks whether the device supports the specified advanced feature.
   *
   * @since v4.3.0
   *
   * @details
   * Checks whether the capabilities of the current device meet the requirements for advanced features
   * such as virtual background and image enhancement.
   * Applicable scenarios: Before using advanced features, you can check whether the current device
   * supports these features based on the call result. This helps to avoid performance degradation or
   * unavailable features when enabling advanced features on low-end devices. Based on the return
   * value of this method, you can decide whether to display or enable the corresponding feature
   * button, or notify the user when the device's capabilities are insufficient.
   *
   * @param type The type of the advanced feature, see `FeatureType`.
   *
   * @return
   * - `true`: The current device supports the specified feature.
   * - `false`: The current device does not support the specified feature.
   */
  virtual bool isFeatureAvailableOnDevice(FeatureType type) = 0;

  /**
   * @brief send audio metadata
   * @since v4.3.1
   * @param metadata The pointer of metadata
   * @param length Size of metadata
   * @return
   * - 0: success
   * - <0: failure
   * @technical preview
  */
  virtual int sendAudioMetadata(const char* metadata, size_t length) = 0;

  /**
   * @brief Queries the HDR capability of the video module
   * @since v4.6.0
   * @param videoModule The video module. See VIDEO_MODULE_TYPE
   * @param capability HDR capability of video module. See HDR_CAPABILITY
   * @return
   * - 0: success
   * - <0: failure
   * @technical preview
  */
  virtual int queryHDRCapability(VIDEO_MODULE_TYPE videoModule, HDR_CAPABILITY& capability) = 0;
};

// The following types are either deprecated or not implmented yet.
enum QUALITY_REPORT_FORMAT_TYPE {
  /** 0: The quality report in JSON format,
   */
  QUALITY_REPORT_JSON = 0,
  /** 1: The quality report in HTML format.
   */
  QUALITY_REPORT_HTML = 1,
};

/**
 * @brief Media device states.
 */
enum MEDIA_DEVICE_STATE_TYPE {
  /**
   * 0: The device is ready for use.
   */
  MEDIA_DEVICE_STATE_IDLE = 0,
  /**
   * 1: The device is in use.
   */
  MEDIA_DEVICE_STATE_ACTIVE = 1,
  /**
   * 2: The device is disabled.
   */
  MEDIA_DEVICE_STATE_DISABLED = 2,

  /**
   * 3: The device is plugged in.
   */
  MEDIA_DEVICE_STATE_PLUGGED_IN = 3,

  /**
   * 4: The device is not found.
   */
  MEDIA_DEVICE_STATE_NOT_PRESENT = 4,
  /**
   * 8: The device is unplugged.
   */
  MEDIA_DEVICE_STATE_UNPLUGGED = 8
};

enum VIDEO_PROFILE_TYPE {
  /** 0: 160 x 120  @ 15 fps */      // res       fps
  VIDEO_PROFILE_LANDSCAPE_120P = 0,  // 160x120   15
  /** 2: 120 x 120 @ 15 fps */
  VIDEO_PROFILE_LANDSCAPE_120P_3 = 2,   // 120x120   15
                                        /** 10: 320 x 180 @ 15 fps */
  VIDEO_PROFILE_LANDSCAPE_180P = 10,    // 320x180   15
                                        /** 12: 180 x 180  @ 15 fps */
  VIDEO_PROFILE_LANDSCAPE_180P_3 = 12,  // 180x180   15
                                        /** 13: 240 x 180 @ 15 fps */
  VIDEO_PROFILE_LANDSCAPE_180P_4 = 13,  // 240x180   15
                                        /** 20: 320 x 240 @ 15 fps */
  VIDEO_PROFILE_LANDSCAPE_240P = 20,    // 320x240   15
  /** 22: 240 x 240 @ 15 fps */
  VIDEO_PROFILE_LANDSCAPE_240P_3 = 22,  // 240x240   15
  /** 23: 424 x 240 @ 15 fps */
  VIDEO_PROFILE_LANDSCAPE_240P_4 = 23,  // 424x240   15
  /** 30: 640 x 360 @ 15 fps */
  VIDEO_PROFILE_LANDSCAPE_360P = 30,  // 640x360   15
  /** 32: 360 x 360 @ 15 fps */
  VIDEO_PROFILE_LANDSCAPE_360P_3 = 32,  // 360x360   15
  /** 33: 640 x 360 @ 30 fps */
  VIDEO_PROFILE_LANDSCAPE_360P_4 = 33,  // 640x360   30
  /** 35: 360 x 360 @ 30 fps */
  VIDEO_PROFILE_LANDSCAPE_360P_6 = 35,  // 360x360   30
  /** 36: 480 x 360 @ 15 fps */
  VIDEO_PROFILE_LANDSCAPE_360P_7 = 36,  // 480x360   15
  /** 37: 480 x 360 @ 30 fps */
  VIDEO_PROFILE_LANDSCAPE_360P_8 = 37,  // 480x360   30
  /** 38: 640 x 360 @ 15 fps */
  VIDEO_PROFILE_LANDSCAPE_360P_9 = 38,   // 640x360   15
                                         /** 39: 640 x 360 @ 24 fps */
  VIDEO_PROFILE_LANDSCAPE_360P_10 = 39,  // 640x360   24
  /** 100: 640 x 360 @ 24 fps */
  VIDEO_PROFILE_LANDSCAPE_360P_11 = 100,  // 640x360   24
  /** 40: 640 x 480 @ 15 fps */
  VIDEO_PROFILE_LANDSCAPE_480P = 40,  // 640x480   15
  /** 42: 480 x 480 @ 15 fps */
  VIDEO_PROFILE_LANDSCAPE_480P_3 = 42,  // 480x480   15
  /** 43: 640 x 480 @ 30 fps */
  VIDEO_PROFILE_LANDSCAPE_480P_4 = 43,  // 640x480   30
                                        /** 45: 480 x 480 @ 30 fps */
  VIDEO_PROFILE_LANDSCAPE_480P_6 = 45,  // 480x480   30
  /** 47: 848 x 480 @ 15 fps */
  VIDEO_PROFILE_LANDSCAPE_480P_8 = 47,  // 848x480   15
                                        /** 48: 848 x 480 @ 30 fps */
  VIDEO_PROFILE_LANDSCAPE_480P_9 = 48,  // 848x480   30
  /** 49: 640 x 480 @ 10 fps */
  VIDEO_PROFILE_LANDSCAPE_480P_10 = 49,  // 640x480   10
  /** 50: 1280 x 720 @ 15 fps */
  VIDEO_PROFILE_LANDSCAPE_720P = 50,  // 1280x720  15
  /** 52: 1280 x 720 @ 30 fps */
  VIDEO_PROFILE_LANDSCAPE_720P_3 = 52,  // 1280x720  30
  /** 54: 960 x 720 @ 15 fps */
  VIDEO_PROFILE_LANDSCAPE_720P_5 = 54,  // 960x720   15
  /** 55: 960 x 720 @ 30 fps */
  VIDEO_PROFILE_LANDSCAPE_720P_6 = 55,  // 960x720   30
  /** 60: 1920 x 1080 @ 15 fps */
  VIDEO_PROFILE_LANDSCAPE_1080P = 60,  // 1920x1080 15
  /** 62: 1920 x 1080 @ 30 fps */
  VIDEO_PROFILE_LANDSCAPE_1080P_3 = 62,  // 1920x1080 30
  /** 64: 1920 x 1080 @ 60 fps */
  VIDEO_PROFILE_LANDSCAPE_1080P_5 = 64,  // 1920x1080 60
  /** 66: 2560 x 1440 @ 30 fps */
  VIDEO_PROFILE_LANDSCAPE_1440P = 66,  // 2560x1440 30
  /** 67: 2560 x 1440 @ 60 fps */
  VIDEO_PROFILE_LANDSCAPE_1440P_2 = 67,  // 2560x1440 60
  /** 70: 3840 x 2160 @ 30 fps */
  VIDEO_PROFILE_LANDSCAPE_4K = 70,  // 3840x2160 30
  /** 72: 3840 x 2160 @ 60 fps */
  VIDEO_PROFILE_LANDSCAPE_4K_3 = 72,     // 3840x2160 60
                                         /** 1000: 120 x 160 @ 15 fps */
  VIDEO_PROFILE_PORTRAIT_120P = 1000,    // 120x160   15
                                         /** 1002: 120 x 120 @ 15 fps */
  VIDEO_PROFILE_PORTRAIT_120P_3 = 1002,  // 120x120   15
                                         /** 1010: 180 x 320 @ 15 fps */
  VIDEO_PROFILE_PORTRAIT_180P = 1010,    // 180x320   15
  /** 1012: 180 x 180 @ 15 fps */
  VIDEO_PROFILE_PORTRAIT_180P_3 = 1012,  // 180x180   15
  /** 1013: 180 x 240 @ 15 fps */
  VIDEO_PROFILE_PORTRAIT_180P_4 = 1013,  // 180x240   15
  /** 1020: 240 x 320 @ 15 fps */
  VIDEO_PROFILE_PORTRAIT_240P = 1020,  // 240x320   15
  /** 1022: 240 x 240 @ 15 fps */
  VIDEO_PROFILE_PORTRAIT_240P_3 = 1022,  // 240x240   15
  /** 1023: 240 x 424 @ 15 fps */
  VIDEO_PROFILE_PORTRAIT_240P_4 = 1023,  // 240x424   15
  /** 1030: 360 x 640 @ 15 fps */
  VIDEO_PROFILE_PORTRAIT_360P = 1030,  // 360x640   15
  /** 1032: 360 x 360 @ 15 fps */
  VIDEO_PROFILE_PORTRAIT_360P_3 = 1032,  // 360x360   15
  /** 1033: 360 x 640 @ 30 fps */
  VIDEO_PROFILE_PORTRAIT_360P_4 = 1033,  // 360x640   30
                                         /** 1035: 360 x 360 @ 30 fps */
  VIDEO_PROFILE_PORTRAIT_360P_6 = 1035,  // 360x360   30
  /** 1036: 360 x 480 @ 15 fps */
  VIDEO_PROFILE_PORTRAIT_360P_7 = 1036,  // 360x480   15
  /** 1037: 360 x 480 @ 30 fps */
  VIDEO_PROFILE_PORTRAIT_360P_8 = 1037,  // 360x480   30
                                         /** 1038: 360 x 640 @ 15 fps */
  VIDEO_PROFILE_PORTRAIT_360P_9 = 1038,  // 360x640   15
  /** 1039: 360 x 640 @ 24 fps */
  VIDEO_PROFILE_PORTRAIT_360P_10 = 1039,  // 360x640   24
  /** 1100: 360 x 640 @ 24 fps */
  VIDEO_PROFILE_PORTRAIT_360P_11 = 1100,  // 360x640   24
  /** 1040: 480 x 640 @ 15 fps */
  VIDEO_PROFILE_PORTRAIT_480P = 1040,  // 480x640   15
  /** 1042: 480 x 480 @ 15 fps */
  VIDEO_PROFILE_PORTRAIT_480P_3 = 1042,  // 480x480   15
  /** 1043: 480 x 640 @ 30 fps */
  VIDEO_PROFILE_PORTRAIT_480P_4 = 1043,  // 480x640   30
                                         /** 1045: 480 x 480 @ 30 fps */
  VIDEO_PROFILE_PORTRAIT_480P_6 = 1045,  // 480x480   30
                                         /** 1047: 480 x 848 @ 15 fps */
  VIDEO_PROFILE_PORTRAIT_480P_8 = 1047,  // 480x848   15
  /** 1048: 480 x 848 @ 30 fps */
  VIDEO_PROFILE_PORTRAIT_480P_9 = 1048,  // 480x848   30
  /** 1049: 480 x 640 @ 10 fps */
  VIDEO_PROFILE_PORTRAIT_480P_10 = 1049,  // 480x640   10
  /** 1050: 720 x 1280 @ 15 fps */
  VIDEO_PROFILE_PORTRAIT_720P = 1050,  // 720x1280  15
  /** 1052: 720 x 1280 @ 30 fps */
  VIDEO_PROFILE_PORTRAIT_720P_3 = 1052,  // 720x1280  30
  /** 1054: 720 x 960 @ 15 fps */
  VIDEO_PROFILE_PORTRAIT_720P_5 = 1054,  // 720x960   15
                                         /** 1055: 720 x 960 @ 30 fps */
  VIDEO_PROFILE_PORTRAIT_720P_6 = 1055,  // 720x960   30
  /** 1060: 1080 x 1920 @ 15 fps */
  VIDEO_PROFILE_PORTRAIT_1080P = 1060,    // 1080x1920 15
                                          /** 1062: 1080 x 1920 @ 30 fps */
  VIDEO_PROFILE_PORTRAIT_1080P_3 = 1062,  // 1080x1920 30
                                          /** 1064: 1080 x 1920 @ 60 fps */
  VIDEO_PROFILE_PORTRAIT_1080P_5 = 1064,  // 1080x1920 60
  /** 1066: 1440 x 2560 @ 30 fps */
  VIDEO_PROFILE_PORTRAIT_1440P = 1066,  // 1440x2560 30
  /** 1067: 1440 x 2560 @ 60 fps */
  VIDEO_PROFILE_PORTRAIT_1440P_2 = 1067,  // 1440x2560 60
                                          /** 1070: 2160 x 3840 @ 30 fps */
  VIDEO_PROFILE_PORTRAIT_4K = 1070,       // 2160x3840 30
  /** 1072: 2160 x 3840 @ 60 fps */
  VIDEO_PROFILE_PORTRAIT_4K_3 = 1072,  // 2160x3840 60
  /** Default 640 x 360 @ 15 fps */
  VIDEO_PROFILE_DEFAULT = VIDEO_PROFILE_LANDSCAPE_360P,
};

class AAudioDeviceManager : public agora::util::AutoPtr<IAudioDeviceManager> {
 public:
  AAudioDeviceManager(IRtcEngine* engine) {
    queryInterface(engine, AGORA_IID_AUDIO_DEVICE_MANAGER);
  }
};

class AVideoDeviceManager : public agora::util::AutoPtr<IVideoDeviceManager> {
 public:
  AVideoDeviceManager(IRtcEngine* engine) {
    queryInterface(engine, AGORA_IID_VIDEO_DEVICE_MANAGER);
  }
};

}  // namespace rtc
}  // namespace agora

/** Gets the SDK version number.

@param build Build number of Agora the SDK.
* @return String of the SDK version.
*/
#define getAgoraRtcEngineVersion getAgoraSdkVersion

////////////////////////////////////////////////////////
/** \addtogroup createAgoraRtcEngine
 @{
 */
////////////////////////////////////////////////////////

/**
 * @brief Creates one `IRtcEngine` object.
 *
 * @details
 * Currently, the Agora RTC SDK v4.x supports creating only one `IRtcEngine` object for each app.
 *
 * @return
 * Pointer to the `IRtcEngine` object.
 */
AGORA_API agora::rtc::IRtcEngine* AGORA_CALL createAgoraRtcEngine();

////////////////////////////////////////////////////////
/** @} */
////////////////////////////////////////////////////////

/** Creates the RTC engine object and returns the pointer.

 @param err Error Code.
* @return Description of the Error Code: agora::ERROR_CODE_TYPE
*/
#define getAgoraRtcEngineErrorDescription getAgoraSdkErrorDescription
#define setAgoraRtcEngineExternalSymbolLoader setAgoraSdkExternalSymbolLoader
