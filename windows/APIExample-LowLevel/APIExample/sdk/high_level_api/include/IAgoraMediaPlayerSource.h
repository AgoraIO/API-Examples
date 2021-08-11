//
//  Agora SDK
//
//  Copyright (c) 2018 Agora.io. All rights reserved.
//
#pragma once  // NOLINT(build/header_guard)

#include "AgoraBase.h"
#include "AgoraMediaBase.h"
#include "AgoraMediaPlayerTypes.h"
#include "AgoraRefPtr.h"

namespace agora {
namespace rtc {

class IMediaPlayerSourceObserver;

/**
 * The IMediaPlayerSource class provides access to a media player source. To playout multiple media sources simultaneously,
 * create multiple media player source objects.
 */
class IMediaPlayerSource : public RefCountInterface {
protected:
  virtual ~IMediaPlayerSource() {}

public:

  /**
   * Gets the unique source ID of the media player source.
   * @return
   * - ≧ 0: The source ID of this media player source.
   * - < 0: Failure.
   */
  virtual int getSourceId() const = 0;

  /**
   * Opens a media file with a specified URL.
   * @param url The path of the media file. Both the local path and online path are supported.
   * @param startPos The starting position (ms) for playback. Default value is 0.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int open(const char* url, int64_t startPos) = 0;

  /**
   * Plays the media file.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int play() = 0;

  /**
   * Pauses the playback.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int pause() = 0;

  /**
   * Stops the playback.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int stop() = 0;

  /**
   * Resumes the playback.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int resume() = 0;

  /**
   * Sets the playback position of the media file.
   * @param newPos The new playback position (ms).
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int seek(int64_t newPos) = 0;

  /**
   * Gets the duration of the media file.
   * @param [out] duration A reference to the duration of the media file.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int getDuration(int64_t& duration) = 0;

  /**
   * Gets the current playback position of the media file.
   * @param [out] pos A reference to the current playback position (ms).
   * @return
   * - 0: Success.
   * - < 0: Failure. See {@link media::base::MEDIA_PLAYER_ERROR MEDIA_PLAYER_ERROR}.
   */
  virtual int getPlayPosition(int64_t& pos) = 0;

  /**
   * Gets the number of the media streams in the media source.
   * @param [out] count The number of the media streams in the media source.
   * @return
   * - 0: Success.
   * - < 0: Failure. See {@link media::base::MEDIA_PLAYER_ERROR MEDIA_PLAYER_ERROR}.
   */
  virtual int getStreamCount(int64_t& count) = 0;

  /**
   * Gets the detailed information of a media stream.
   * @param index The index of the media stream.
   * @param [out] info The detailed information of the media stream. See \ref media::base::PlayerStreamInfo "PlayerStreamInfo" for details.
   * @return
   * - 0: Success.
   * - < 0: Failure. See {@link media::base::MEDIA_PLAYER_ERROR MEDIA_PLAYER_ERROR}.
   */
  virtual int getStreamInfo(int64_t index, media::base::PlayerStreamInfo* info) = 0;

  /**
   * Sets whether to loop the media file for playback.
   * @param loopCount The number of times of looping the media file.
   * - 0: Play the media file once.
   * - 1: Play the media file twice.
   * - -1: Play the media file in a loop indefinitely, until {@link stop} is called.
   * @return
   * - 0: Success.
   * - < 0: Failure. See {@link media::base::MEDIA_PLAYER_ERROR MEDIA_PLAYER_ERROR}.
   */
  virtual int setLoopCount(int64_t loopCount) = 0;

  /**
   * Changes the playback speed.
   * @param speed The playback speed. See \ref media::base::MEDIA_PLAYER_PLAYBACK_SPEED "MEDIA_PLAYER_PLAYBACK_SPEED" for details.
   * @return
   * - 0: Success.
   * - < 0: Failure. See {@link media::base::MEDIA_PLAYER_ERROR MEDIA_PLAYER_ERROR}.
   */
  virtual int changePlaybackSpeed(media::base::MEDIA_PLAYER_PLAYBACK_SPEED speed) = 0;

  /**
   * Selects an audio track of the media file for playback.
   * @param index The index of the audio track in media file.
   * @return
   * - 0: Success.
   * - < 0: Failure. See {@link media::base::MEDIA_PLAYER_ERROR MEDIA_PLAYER_ERROR}.
   */
  virtual int selectAudioTrack(int64_t index) = 0;

  /**
   * Changes the player option before playing a file.
   * @param key The key of the option paramemter.
   * @param value The value of option parameter.
   * @return
   * - 0: Success.
   * - < 0: Failure. See {@link media::base::MEDIA_PLAYER_ERROR MEDIA_PLAYER_ERROR}.
   */
  virtual int setPlayerOption(const char* key, int64_t value) = 0;

  /**
   * Changes the player option before playing a file.
   * @param key The key of the option paramemter.
   * @param value The value of option parameter.
   * @return
   * - 0: Success.
   * - < 0: Failure. See {@link media::base::MEDIA_PLAYER_ERROR MEDIA_PLAYER_ERROR}.
   */
  virtual int setPlayerOption(const char* key, const char* value) = 0;

  /**
   * Takes a screenshot when playing a video file.
   * @param filename The filename of the screenshot file.
   * @return
   * - 0: Success.
   * - < 0: Failure. See {@link media::base::MEDIA_PLAYER_ERROR MEDIA_PLAYER_ERROR}.
   */
  virtual int takeScreenshot(const char* filename) = 0;

  /**
   * Selects internal subtitles for a video file.
   * @param index The index of the internal subtitles.
   * @return
   * - 0: Success.
   * - < 0: Failure. See {@link media::base::MEDIA_PLAYER_ERROR MEDIA_PLAYER_ERROR}.
   */
  virtual int selectInternalSubtitle(int64_t index) = 0;

  /**
   * Sets an external subtitle file for a video file.
   * @param url The URL of the subtitle file.
   * @return
   * - 0: Success.
   * - < 0: Failure. See {@link media::base::MEDIA_PLAYER_ERROR MEDIA_PLAYER_ERROR}.
   */
  virtual int setExternalSubtitle(const char* url) = 0;

  /**
   * Gets the playback state.
   * @return The current playback state. See {@link media::base::MEDIA_PLAYER_STATE MEDIA_PLAYER_STATE} for details.
   */
  virtual media::base::MEDIA_PLAYER_STATE getState() = 0;

  /**
   * Registers a media player source observer.
   *
   * Once the media player source observer is registered, you can use the observer to monitor the state change of the media player.
   * @param observer The pointer to the IMediaPlayerSourceObserver object.
   * @return
   * - 0: Success.
   * - < 0: Failure. See {@link media::base::MEDIA_PLAYER_ERROR MEDIA_PLAYER_ERROR}.
   */
  virtual int registerPlayerSourceObserver(IMediaPlayerSourceObserver* observer) = 0;

  /**
   * Releases the media player source observer.
   * @param observer The pointer to the IMediaPlayerSourceObserver object.
   * @return
   * - 0: Success.
   * - < 0: Failure. See {@link media::base::MEDIA_PLAYER_ERROR MEDIA_PLAYER_ERROR}.
   */
  virtual int unregisterPlayerSourceObserver(IMediaPlayerSourceObserver* observer) = 0;

  /**
   * Registers the audio frame observer.
   *
   * @param observer The pointer to the {@link media::base::IAudioFrameObserver IAudioFrameObserver} object.
   * @return
   * - 0: Success.
   * - < 0: Failure. See {@link media::base::MEDIA_PLAYER_ERROR MEDIA_PLAYER_ERROR}.
   */
  virtual int registerAudioFrameObserver(media::base::IAudioFrameObserver* observer) = 0;

  /**
   * Releases the audio frame observer.
   * @param observer The pointer to the {@link media::base::IAudioFrameObserver IAudioFrameObserver} object.
   * @return
   * - 0: Success.
   * - < 0: Failure. See {@link media::base::MEDIA_PLAYER_ERROR MEDIA_PLAYER_ERROR}.
   */
  virtual int unregisterAudioFrameObserver(media::base::IAudioFrameObserver* observer) = 0;
};

/**
 * This class class reports runtime events to the applications.
 */
class IMediaPlayerSourceObserver {
 public:
  virtual ~IMediaPlayerSourceObserver() {}

  /**
   * @brief Reports the playback state change.
   *
   * When the state of the playback changes, the SDK triggers this callback to report the new playback state and the reason or error for the change.
   * @param state The new playback state after change. See {@link media::base::MEDIA_PLAYER_STATE MEDIA_PLAYER_STATE}.
   * @param ec The player's error code. See {@link media::base::MEDIA_PLAYER_ERROR MEDIA_PLAYER_ERROR}.
   */
  virtual void onPlayerSourceStateChanged(media::base::MEDIA_PLAYER_STATE state,
                                          media::base::MEDIA_PLAYER_ERROR ec) = 0;

  /**
   * @brief Reports current playback progress.
   *
   * The callback occurs once every one second during the playback and reports the current playback progress.
   * @param position Current playback progress (second).
   */
  virtual void onPositionChanged(int64_t position) = 0;

  /**
   * @brief Reports the playback event.
   *
   * - After calling the `seek` method, the SDK triggers the callback to report the results of the seek operation.
   * - After calling the `selectAudioTrack` method, the SDK triggers the callback to report that the audio track changes.
   *
   * @param event The playback event. See {@link media::base::MEDIA_PLAYER_EVENT MEDIA_PLAYER_EVENT}.
   */
  virtual void onPlayerEvent(media::base::MEDIA_PLAYER_EVENT event) = 0;

  /**
   * @brief Occurs when the metadata is received.
   *
   * The callback occurs when the player receives the media metadata and reports the detailed information of the media metadata.
   * @param data The detailed data of the media metadata.
   * @param length The data length (bytes).
   */
  virtual void onMetaData(const void* data, int length) = 0;


  /**
   * @brief Triggered when play buffer updated, once every 1 second
   *
   * @param int cached buffer during playing, in milliseconds
   */
  virtual void onPlayBufferUpdated(int64_t playCachedBuffer) = 0;

  /**
   * @brief Occurs when one playback of the media file is completed.
   */
  virtual void onCompleted() = 0;
};

} //namespace rtc
} // namespace agora
