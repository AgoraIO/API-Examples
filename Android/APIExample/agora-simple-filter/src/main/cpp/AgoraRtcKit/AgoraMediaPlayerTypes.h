//
//  Agora Engine SDK
//
//  Created by Sting Feng in 2020-05.
//  Copyright (c) 2017 Agora.io. All rights reserved.

#pragma once  // NOLINT(build/header_guard)

#include <cstring>
#include <stdint.h>

#include "AgoraOptional.h"

/**
 * set analyze duration for real time stream
 * @example "setPlayerOption(KEY_PLAYER_REAL_TIME_STREAM_ANALYZE_DURATION,1000000)"
 */
#define KEY_PLAYER_REAL_TIME_STREAM_ANALYZE_DURATION    "analyze_duration"

/**
 * make the player to enable audio or not
 * @example  "setPlayerOption(KEY_PLAYER_ENABLE_AUDIO,0)"
 */
#define KEY_PLAYER_ENABLE_AUDIO                  "enable_audio"

/**
 * make the player to enable video or not
 * @example  "setPlayerOption(KEY_PLAYER_ENABLE_VIDEO,0)"
 */
#define KEY_PLAYER_ENABLE_VIDEO                  "enable_video"

/**
 * set the player enable to search metadata
 * @example  "setPlayerOption(KEY_PLAYER_DISABLE_SEARCH_METADATA,0)"
 */
#define KEY_PLAYER_ENABLE_SEARCH_METADATA         "enable_search_metadata"

/**
 * set the player sei filter type
 * @example  "setPlayerOption(KEY_PLAYER_SEI_FILTER_TYPE,"5")"
 */
#define KEY_PLAYER_SEI_FILTER_TYPE         "set_sei_filter_type"

namespace agora {

namespace media {

namespace base {
static const uint8_t kMaxCharBufferLength = 50;
/**
 * @brief The playback state.
 */
enum MEDIA_PLAYER_STATE {
  /**
   * 0: The default state. The media player returns this state code before you open the media resource
   * or after you stop the playback.
   */
  PLAYER_STATE_IDLE = 0,
  /**
   * 1: Opening the media resource.
   */
  PLAYER_STATE_OPENING,
  /**
   * 2: Opens the media resource successfully.
   */
  PLAYER_STATE_OPEN_COMPLETED,
  /**
   * 3: The media resource is playing.
   */
  PLAYER_STATE_PLAYING,
  /**
   * 4: Pauses the playback.
   */
  PLAYER_STATE_PAUSED,
  /**
   * 5: The playback is complete.
   */
  PLAYER_STATE_PLAYBACK_COMPLETED,
  /**
   * 6: The loop is complete.
   */
  PLAYER_STATE_PLAYBACK_ALL_LOOPS_COMPLETED,
  /**
   * 7: The playback stops.
   */
  PLAYER_STATE_STOPPED,
  /** Player pausing (internal)
   */
  PLAYER_STATE_PAUSING_INTERNAL = 50,
  /** Player stopping (internal)
   */
  PLAYER_STATE_STOPPING_INTERNAL,
  /** Player seeking state (internal)
   */
  PLAYER_STATE_SEEKING_INTERNAL,
  /** Player getting state (internal)
   */
  PLAYER_STATE_GETTING_INTERNAL,
  /** None state for state machine (internal)
   */
  PLAYER_STATE_NONE_INTERNAL,
  /** Do nothing state for state machine (internal)
   */
  PLAYER_STATE_DO_NOTHING_INTERNAL,
  /** Player set track state (internal)
   */
  PLAYER_STATE_SET_TRACK_INTERNAL,
  /**
   * 100: The media player fails to play the media resource.
   */
  PLAYER_STATE_FAILED = 100,
};
/**
 * @brief Reasons for the changes in the media player status.
 */
enum MEDIA_PLAYER_REASON {
  /**
   * 0: No error.
   */
  PLAYER_REASON_NONE = 0,
  /**
   * -1: Invalid arguments.
   */
  PLAYER_REASON_INVALID_ARGUMENTS = -1,
  /**
   * -2: Internal error.
   */
  PLAYER_REASON_INTERNAL = -2,
  /**
   * -3: No resource.
   */
  PLAYER_REASON_NO_RESOURCE = -3,
  /**
   * -4: Invalid media resource.
   */
  PLAYER_REASON_INVALID_MEDIA_SOURCE = -4,
  /**
   * -5: The media stream type is unknown.
   */
  PLAYER_REASON_UNKNOWN_STREAM_TYPE = -5,
  /**
   * -6: The object is not initialized.
   */
  PLAYER_REASON_OBJ_NOT_INITIALIZED = -6,
  /**
   * -7: The codec is not supported.
   */
  PLAYER_REASON_CODEC_NOT_SUPPORTED = -7,
  /**
   * -8: Invalid renderer.
   */
  PLAYER_REASON_VIDEO_RENDER_FAILED = -8,
  /**
   * -9: An error with the internal state of the player occurs.
   */
  PLAYER_REASON_INVALID_STATE = -9,
  /**
   * -10: The URL of the media resource cannot be found.
   */
  PLAYER_REASON_URL_NOT_FOUND = -10,
  /**
   * -11: Invalid connection between the player and the Agora Server.
   */
  PLAYER_REASON_INVALID_CONNECTION_STATE = -11,
  /**
   * -12: The playback buffer is insufficient.
   */
  PLAYER_REASON_SRC_BUFFER_UNDERFLOW = -12,
  /**
   * -13: The playback is interrupted.
   */
  PLAYER_REASON_INTERRUPTED = -13,
  /**
   * -14: The SDK does not support the method being called.
   */
  PLAYER_REASON_NOT_SUPPORTED = -14,
  /** The token has expired.
   */
  PLAYER_REASON_TOKEN_EXPIRED = -15,
  /** The ip has expired.
   */
  PLAYER_REASON_IP_EXPIRED = -16,
  /**
   * -17: An unknown error.
   */
  PLAYER_REASON_UNKNOWN = -17,
};

/**
 * @brief The type of the media stream.
 */
enum MEDIA_STREAM_TYPE {
  /**
   * 0: The type is unknown.
   */
  STREAM_TYPE_UNKNOWN = 0,
  /**
   * 1: The video stream.
   */
  STREAM_TYPE_VIDEO = 1,
  /**
   * 2: The audio stream.
   */
  STREAM_TYPE_AUDIO = 2,
  /**
   * 3: The subtitle stream.
   */
  STREAM_TYPE_SUBTITLE = 3,
};

/**
 * @brief Media player events.
 */
enum MEDIA_PLAYER_EVENT {
  /**
   * 0: The player begins to seek to a new playback position.
   */
  PLAYER_EVENT_SEEK_BEGIN = 0,
  /**
   * 1: The player finishes seeking to a new playback position.
   */
  PLAYER_EVENT_SEEK_COMPLETE = 1,
  /**
   * 2: An error occurs when seeking to a new playback position.
   */
  PLAYER_EVENT_SEEK_ERROR = 2,
  /**
   * 5: The audio track used by the player has been changed.
   */
  PLAYER_EVENT_AUDIO_TRACK_CHANGED = 5,
  /**
   * 6: The currently buffered data is not enough to support playback.
   */
  PLAYER_EVENT_BUFFER_LOW = 6,
  /**
   * 7: The currently buffered data is just enough to support playback.
   */
  PLAYER_EVENT_BUFFER_RECOVER = 7,
  /**
   * 8: The audio or video playback freezes.
   */
  PLAYER_EVENT_FREEZE_START = 8,
  /**
   * 9: The audio or video playback resumes without freezing.
   */
  PLAYER_EVENT_FREEZE_STOP = 9,
  /**
   * 10: The player starts switching the media resource.
   */
  PLAYER_EVENT_SWITCH_BEGIN = 10,
  /**
   * 11: Media resource switching is complete.
   */
  PLAYER_EVENT_SWITCH_COMPLETE = 11,
  /**
   * 12: Media resource switching error.
   */
  PLAYER_EVENT_SWITCH_ERROR = 12,
  /**
   * 13: The first video frame is rendered.
   */
  PLAYER_EVENT_FIRST_DISPLAYED = 13,
  /**
   * 14: The cached media files reach the limit in number.
   */
  PLAYER_EVENT_REACH_CACHE_FILE_MAX_COUNT = 14,
  /**
   * 15: The cached media files reach the limit in aggregate storage space.
   */
  PLAYER_EVENT_REACH_CACHE_FILE_MAX_SIZE = 15,
  /** Triggered when a retry is required to open the media
   */
  PLAYER_EVENT_TRY_OPEN_START = 16,
  /** Triggered when the retry to open the media is successful
   */
  PLAYER_EVENT_TRY_OPEN_SUCCEED = 17,
  /** Triggered when retrying to open media fails
   */
  PLAYER_EVENT_TRY_OPEN_FAILED = 18,
  /** Triggered when an http redirect occurs
   * @technical preview 
   */
  PLAYER_EVENT_HTTP_REDIRECT = 19,
};

/**
 * @brief Events that occur when media resources are preloaded.
 */
enum PLAYER_PRELOAD_EVENT  {
  /**
   * 0: Starts preloading media resources.
   */
  PLAYER_PRELOAD_EVENT_BEGIN = 0,
  /**
   * 1: Preloading media resources is complete.
   */
  PLAYER_PRELOAD_EVENT_COMPLETE = 1,
  /**
   * 2: An error occurs when preloading media resources.
   */
  PLAYER_PRELOAD_EVENT_ERROR = 2,
};

/**
 * @brief The detailed information of the media stream.
 */
struct PlayerStreamInfo {
  /**
   * The index of the media stream.
   */
  int streamIndex;

  /**
   * The type of the media stream. See `MEDIA_STREAM_TYPE`.
   */
  MEDIA_STREAM_TYPE streamType;

  /**
   * The codec of the media stream.
   */
  char codecName[kMaxCharBufferLength];

  /**
   * The language of the media stream.
   */
  char language[kMaxCharBufferLength];

  /**
   * This parameter only takes effect for video streams, and indicates the video frame rate (fps).
   */
  int videoFrameRate;

  /**
   * This parameter only takes effect for video streams, and indicates the video bitrate (bps).
   */
  int videoBitRate;

  /**
   * This parameter only takes effect for video streams, and indicates the video width (pixel).
   */
  int videoWidth;

  /**
   * This parameter only takes effect for video streams, and indicates the video height (pixel).
   */
  int videoHeight;

  /**
   * This parameter only takes effect for video streams, and indicates the video rotation angle.
   */
  int videoRotation;

  /**
   * This parameter only takes effect for audio streams, and indicates the audio sample rate (Hz).
   */
  int audioSampleRate;

  /**
   * This parameter only takes effect for audio streams, and indicates the audio channel number.
   */
  int audioChannels;

  /**
   * This parameter only takes effect for audio streams, and indicates the bit number of each audio
   * sample.
   */
  int audioBitsPerSample;

  /**
   * The total duration (ms) of the media stream.
   */
  int64_t duration;

  PlayerStreamInfo() : streamIndex(0),
                       streamType(STREAM_TYPE_UNKNOWN),
                       videoFrameRate(0),
                       videoBitRate(0),
                       videoWidth(0),
                       videoHeight(0),
                       videoRotation(0),
                       audioSampleRate(0),
                       audioChannels(0),
                       audioBitsPerSample(0),
                       duration(0) {
    memset(codecName, 0, sizeof(codecName));
    memset(language, 0, sizeof(language));
  }
};

/**
 * @brief Information about the video bitrate of the media resource being played.
 */
struct SrcInfo {
  /**
   * The video bitrate (Kbps) of the media resource being played.
   */
  int bitrateInKbps;

  /**
   * The name of the media resource.
   */
  const char* name;

};

/**
 * @brief The type of media metadata.
 */
enum MEDIA_PLAYER_METADATA_TYPE {
  /**
   * 0: The type is unknown.
   */
  PLAYER_METADATA_TYPE_UNKNOWN = 0,
  /**
   * 1: The type is SEI.
   */
  PLAYER_METADATA_TYPE_SEI = 1,
};

/**
 * @brief Statistics about the media files being cached.
 */
struct CacheStatistics {
  /**
   * The size (bytes) of the media file being played.
   */
  int64_t fileSize;
  /**
   * The size (bytes) of the media file that you want to cache.
   */
  int64_t cacheSize;
  /**
   * The size (bytes) of the media file that has been downloaded.
   */
  int64_t downloadSize;
};

/**
 * @brief The information of the media file being played.
 */
struct PlayerPlaybackStats {
  /**
   * The frame rate (fps) of the video.
   */
  int videoFps;
  /**
   * The bitrate (kbps) of the video.
   */
  int videoBitrateInKbps;
  /**
   * The bitrate (kbps) of the audio.
   */
  int audioBitrateInKbps;
  /**
   * The total bitrate (kbps) of the media stream.
   */
  int totalBitrateInKbps;
};

/**
 * @brief Information related to the media player.
 */
struct PlayerUpdatedInfo {
  /** @technical preview
   */
  const char* internalPlayerUuid;
  /**
   * The ID of a deivce.
   */
  const char* deviceId;
  /**
   * Height (pixel) of the video.
   */
  int videoHeight;
  /**
   * Width (pixel) of the video.
   */
  int videoWidth;
  /**
   * Audio sample rate (Hz).
   */
  int audioSampleRate;
  /**
   * The number of audio channels.
   */
  int audioChannels;
  /**
   * The number of bits per audio sample point.
   */
  int audioBitsPerSample;

  PlayerUpdatedInfo()
      : internalPlayerUuid(NULL),
        deviceId(NULL),
        videoHeight(0),
        videoWidth(0),
        audioSampleRate(0),
        audioChannels(0),
        audioBitsPerSample(0) {}
};

/**
 * The custom data source provides a data stream input callback, and the player will continue to call back this interface, requesting the user to fill in the data that needs to be played.
 */
class IMediaPlayerCustomDataProvider {
public:
    
    /**
     * @brief Occurs when the SDK reads the media resource data.
     *
     * @details
     * When you call the `openWithMediaSource` method to open a media resource, the SDK triggers this
     * callback and request you to pass in the buffer of the media resource data.
     *
     * @param buffer An input parameter. Data buffer (bytes). Write the `bufferSize` data reported by
     * the SDK into this parameter.
     * @param bufferSize The length of the data buffer (bytes).
     *
     * @return
     * - If the data is read successfully, pass in the length of the data (bytes) you actually read in
     * the return value.
     * - If reading the data fails, pass in 0 in the return value.
     */
    virtual int onReadData(unsigned char *buffer, int bufferSize) = 0;
    
    /**
     * @brief Occurs when the SDK seeks the media resource data.
     *
     * @details
     * When you call the `openWithMediaSource` or `open` method to open a custom media resource, the SDK
     * triggers this callback to request the specified location in the media resource.
     *
     * @param offset An input parameter. The offset of the target position relative to the starting
     * point, in bytes. The value can be positive or negative.
     * @param whence An input parameter. The starting point. You can set it as one of the following
     * values:
     * - 0: The starting point is the head of the data, and the actual data offset after seeking is
     * `offset`.
     * - 1: The starting point is the current position, and the actual data offset after seeking is the
     * current position plus `offset`.
     * - 2: The starting point is the end of the data, and the actual data offset after seeking is the
     * whole data length plus `offset`.
     * - 65536: Do not perform position seeking, return the file size. Agora recommends that you use
     * this parameter value when playing pure audio files such as MP3 and WAV.
     *
     * @return
     * - When `whence` is `65536`, the media file size is returned.
     * - When `whence` is `0`, `1`, or `2`, the actual data offset after the seeking is returned.
     * - -1: Seeking failed.
     */
    virtual int64_t onSeek(int64_t offset, int whence) = 0;
    
    virtual ~IMediaPlayerCustomDataProvider() {}
};

/**
 * @brief Information related to the media file to be played and the playback scenario
 * configurations.
 */
struct MediaSource {
  /**
   * The URL of the media file to be played.
   * @note If you open a common media resource, pass in the value to `url`. If you open a custom media
   * resource, pass in the value to `provider`. Agora recommends that you do not pass in values to
   * both parameters in one call; otherwise, this call may fail.
   */
  const char* url;
  /**
   * The URI (Uniform Resource Identifier) of the media file.
   */
  const char* uri;
  /**
   * The starting position (ms) for playback. The default value is 0.
   */
  int64_t startPos;
  /**
   * Whether to enable autoplay once the media file is opened:
   * - `true`: (Default) Yes.
   * - `false`: No.
   * @note If autoplay is disabled, you need to call the `play` method to play a media file after it
   * is opened.
   */
  bool autoPlay;
  /**
   * Whether to cache the media file when it is being played:
   * - `true`:Enables caching.
   * - `false`: (Default) Disables caching.
   * @note
   * - Agora only supports caching on-demand audio and video streams that are not transmitted in HLS
   * protocol.
   * - If you need to enable caching, pass in a value to `uri`; otherwise, caching is based on the
   * `url` of the media file.
   * - If you enable this function, the Media Player caches part of the media file being played on
   * your local device, and you can play the cached media file without internet connection. The
   * statistics about the media file being cached are updated every second after the media file is
   * played. See `CacheStatistics`.
   */
  bool enableCache;
  /**
   * Whether to allow the selection of different audio tracks when playing this media file:
   * - `true`: Allow to select different audio tracks.
   * - `false`: (Default) Do not allow to select different audio tracks.
   * If you need to set different audio tracks for local playback and publishing to the channel, you
   * need to set this parameter to `true`, and then call the `selectMultiAudioTrack` method to select
   * the audio track.
   */
  bool enableMultiAudioTrack;
  /**
   * Whether the media resource to be opened is a live stream or on-demand video distributed through
   * Media Broadcast service:
   * - `true`: The media resource to be played is a live or on-demand video distributed through Media
   * Broadcast service.
   * - `false`: (Default) The media resource is not a live stream or on-demand video distributed
   * through Media Broadcast service.
   * @note If you need to open a live stream or on-demand video distributed through Broadcast
   * Streaming service, pass in the URL of the media resource to `url`, and set `isAgoraSource` as
   * `true`; otherwise, you don't need to set the `isAgoraSource` parameter.
   */
  Optional<bool> isAgoraSource;
  /**
   * Whether the media resource to be opened is a live stream:
   * - `true`: The media resource is a live stream.
   * - `false`: (Default) The media resource is not a live stream.
   * If the media resource you want to open is a live stream, Agora recommends that you set this
   * parameter as `true` so that the live stream can be loaded more quickly.
   * @note If the media resource you open is not a live stream, but you set `isLiveSource` as `true`,
   * the media resource is not to be loaded more quickly.
   */
  Optional<bool> isLiveSource;
  /**
   * The callback for custom media resource files. See `IMediaPlayerCustomDataProvider`.
   * @note If you open a custom media resource, pass in the value to `provider`. If you open a common
   * media resource, pass in the value to `url`. Agora recommends that you do not pass in values to
   * both `url` and `provider` in one call; otherwise, this call may fail.
   */
  IMediaPlayerCustomDataProvider* provider;

  MediaSource() : url(NULL), uri(NULL), startPos(0), autoPlay(true), enableCache(false),
                  enableMultiAudioTrack(false), provider(NULL){
  }
};

}  // namespace base
}  // namespace media
}  // namespace agora
