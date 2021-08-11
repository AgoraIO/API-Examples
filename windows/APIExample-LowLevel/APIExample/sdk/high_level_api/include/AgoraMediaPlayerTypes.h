//
//  Agora Engine SDK
//
//  Created by Sting Feng in 2020-05.
//  Copyright (c) 2017 Agora.io. All rights reserved.

#pragma once  // NOLINT(build/header_guard)

#include <cstring>
#if __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1800)
#include <cstdint>
#endif

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
 *
 */
enum MEDIA_PLAYER_STATE {
  /** Default state.
   */
  PLAYER_STATE_IDLE = 0,
  /** Opening the media file.
   */
  PLAYER_STATE_OPENING,
  /** The media file is opened successfully.
   */
  PLAYER_STATE_OPEN_COMPLETED,
  /** Playing the media file.
   */
  PLAYER_STATE_PLAYING,
  /** The playback is paused.
   */
  PLAYER_STATE_PAUSED,
  /** The playback is completed.
   */
  PLAYER_STATE_PLAYBACK_COMPLETED,
  /** All loops are completed.
   */
  PLAYER_STATE_PLAYBACK_ALL_LOOPS_COMPLETED,
  /** The playback is stopped.
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
  /** The playback fails.
   */
  PLAYER_STATE_FAILED = 100,
};
/**
 * @brief Player error code
 *
 */
enum MEDIA_PLAYER_ERROR {
  /** No error.
   */
  PLAYER_ERROR_NONE = 0,
  /** The parameter is invalid.
   */
  PLAYER_ERROR_INVALID_ARGUMENTS = -1,
  /** Internel error.
   */
  PLAYER_ERROR_INTERNAL = -2,
  /** No resource.
   */
  PLAYER_ERROR_NO_RESOURCE = -3,
  /** Invalid media source.
   */
  PLAYER_ERROR_INVALID_MEDIA_SOURCE = -4,
  /** The type of the media stream is unknown.
   */
  PLAYER_ERROR_UNKNOWN_STREAM_TYPE = -5,
  /** The object is not initialized.
   */
  PLAYER_ERROR_OBJ_NOT_INITIALIZED = -6,
  /** The codec is not supported.
   */
  PLAYER_ERROR_CODEC_NOT_SUPPORTED = -7,
  /** Invalid renderer.
   */
  PLAYER_ERROR_VIDEO_RENDER_FAILED = -8,
  /** An error occurs in the internal state of the player.
   */
  PLAYER_ERROR_INVALID_STATE = -9,
  /** The URL of the media file cannot be found.
   */
  PLAYER_ERROR_URL_NOT_FOUND = -10,
  /** Invalid connection between the player and the Agora server.
   */
  PLAYER_ERROR_INVALID_CONNECTION_STATE = -11,
  /** The playback buffer is insufficient.
   */
  PLAYER_ERROR_SRC_BUFFER_UNDERFLOW = -12,
  /** The audio mixing file playback is interrupted.
   */
  PLAYER_ERROR_INTERRUPTED = -13,
};

/**
 * @brief The playback speed.
 *
 */
enum MEDIA_PLAYER_PLAYBACK_SPEED {
  /** The original playback speed.
   */
  PLAYBACK_SPEED_ORIGINAL = 100,
  /** 0.5 times the original playback speed.
 */
  PLAYBACK_SPEED_50_PERCENT = 50,
  /** 0.75 times the original playback speed.
   */
  PLAYBACK_SPEED_75_PERCENT = 75,
  /** 1.25 times the original playback speed.
   */
  PLAYBACK_SPEED_125_PERCENT = 125,
  /** 1.5 times the original playback speed.
   */
  PLAYBACK_SPEED_150_PERCENT = 150,
  /** 2.0 times the original playback.
   */
  PLAYBACK_SPEED_200_PERCENT = 200,
};

/**
 * @brief The type of the media stream.
 *
 */
enum MEDIA_STREAM_TYPE {
  /** The type is unknown.
   */
  STREAM_TYPE_UNKNOWN = 0,
  /** The video stream.
   */
  STREAM_TYPE_VIDEO = 1,
  /** The audio stream.
   */
  STREAM_TYPE_AUDIO = 2,
  /** The subtitle stream.
   */
  STREAM_TYPE_SUBTITLE = 3,
};

/**
 * @brief The playback event.
 *
 */
enum MEDIA_PLAYER_EVENT {
  /** The player begins to seek to the new playback position.
   */
  PLAYER_EVENT_SEEK_BEGIN = 0,
  /** The seek operation completes.
   */
  PLAYER_EVENT_SEEK_COMPLETE = 1,
  /** An error occurs during the seek operation.
   */
  PLAYER_EVENT_SEEK_ERROR = 2,
  /** The player publishes a video track.
   */
  PLAYER_EVENT_VIDEO_PUBLISHED = 3,
  /** The player publishes an audio track.
   */
  PLAYER_EVENT_AUDIO_PUBLISHED = 4,
  /** The player changes the audio track for playback.
   */
  PLAYER_EVENT_AUDIO_TRACK_CHANGED = 5,
  /** player buffer low
   */
  PLAYER_EVENT_BUFFER_LOW = 6,
    /** player buffer recover
   */
  PLAYER_EVENT_BUFFER_RECOVER = 7,
  /** The video or audio is interrupted
   */
  PLAYER_EVENT_FREEZE_START = 8,
  /** Interrupt at the end of the video or audio
   */
  PLAYER_EVENT_FREEZE_STOP = 9,
};

/**
 * @brief The information of the media stream object.
 *
 */
struct PlayerStreamInfo {
  /** The index of the media stream. */
  int streamIndex;

  /** The type of the media stream. See {@link MEDIA_STREAM_TYPE}. */
  MEDIA_STREAM_TYPE streamType;

  /** The codec of the media stream. */
  char codecName[kMaxCharBufferLength];

  /** The language of the media stream. */
  char language[kMaxCharBufferLength];

  /** The frame rate (fps) if the stream is video. */
  int videoFrameRate;

  /** The video bitrate (bps) if the stream is video. */
  int videoBitRate;

  /** The video width (pixel) if the stream is video. */
  int videoWidth;

  /** The video height (pixel) if the stream is video. */
  int videoHeight;

  /** The rotation angle if the steam is video. */
  int videoRotation;

  /** The sample rate if the stream is audio. */
  int audioSampleRate;

  /** The number of audio channels if the stream is audio. */
  int audioChannels;

  /** The number of bits per sample if the stream is audio. */
  int audioBitsPerSample;

  /** The total duration (second) of the media stream. */
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
 * @brief The type of the media metadata.
 *
 */
enum MEDIA_PLAYER_METADATA_TYPE {
  /** The type is unknown.
   */
  PLAYER_METADATA_TYPE_UNKNOWN = 0,
  /** The type is SEI.
   */
  PLAYER_METADATA_TYPE_SEI = 1,
};

}  // namespace base
}  // namespace media
}  // namespace agora
