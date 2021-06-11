//
//  Agora SDK
//
//  Copyright (c) 2020 Agora.io. All rights reserved.
//
#pragma once  // NOLINT(build/header_guard)

#include "AgoraBase.h"
#include "AgoraMediaBase.h"
#include "AgoraMediaPlayerTypes.h"
#include "AgoraRefPtr.h"

namespace agora {
namespace base {
class IAgoraService;
}
namespace rtc {

class ILocalAudioTrack;
class ILocalVideoTrack;
class IMediaPlayerSourceObserver;

/**
 * The IMediaPlayerEntity class provides access to a media player entity. If yout want to playout
 * multiple media sources simultaneously, create multiple media player source objects.
 */
class IMediaPlayer : public RefCountInterface {
protected:
  virtual ~IMediaPlayer() {}

public:
  virtual int initialize(base::IAgoraService* agora_service) = 0;

  /**
   * Get unique media player id of the media player entity.
   * @return
   * - >= 0: The source id of this media player entity.
   * - < 0: Failure.
   */
  virtual int getMediaPlayerId() const = 0;

  /**
   * Opens a media file with a specified URL.
   * @param url The URL of the media file that you want to play.
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
   * Pauses playing the media file.
   */
  virtual int pause() = 0;

  /**
   * Stops playing the current media file.
   */
  virtual int stop() = 0;

  /**
   * Resumes playing the media file.
   */
  virtual int resume() = 0;

  /**
   * Sets the current playback position of the media file.
   * @param newPos The new playback position (ms).
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int seek(int64_t newPos) = 0;

  /**
   * Gets the duration of the media file.
   * @param duration A reference to the duration of the media file.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int getDuration(int64_t& duration) = 0;

  /**
   * Gets the current playback position of the media file.
   * @param currentPosition A reference to the current playback position (ms).
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int getPlayPosition(int64_t& pos) = 0;

  virtual int getStreamCount(int64_t& count) = 0;

  virtual int getStreamInfo(int64_t index, media::base::PlayerStreamInfo* info) = 0;

  /**
   * Sets whether to loop the media file for playback.
   * @param loopCount the number of times looping the media file.
   * - 0: Play the audio effect once.
   * - 1: Play the audio effect twice.
   * - -1: Play the audio effect in a loop indefinitely, until stopEffect() or stop() is called.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setLoopCount(int loopCount) = 0;

  /**
   * Change playback speed
   * @param speed the enum of playback speed
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int changePlaybackSpeed(media::base::MEDIA_PLAYER_PLAYBACK_SPEED speed) = 0;

  /**
   * Slect playback audio track of the media file
   * @param speed the index of the audio track in meia file
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int selectAudioTrack(int index) = 0;

  /**
   * change player option before play a file
   * @param key the key of the option param
   * @param value the value of option param
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setPlayerOption(const char* key, int value) = 0;

  /**
   * change player option before play a file
   * @param key the key of the option param
   * @param value the value of option param
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setPlayerOption(const char* key, const char* value) = 0;
  /**
   * take screenshot while playing  video
   * @param filename the filename of screenshot file
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int takeScreenshot(const char* filename) = 0;

  /**
   * select internal subtitles in video
   * @param index the index of the internal subtitles
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int selectInternalSubtitle(int index) = 0;

  /**
   * set an external subtitle for video
   * @param url The URL of the subtitle file that you want to load.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setExternalSubtitle(const char* url) = 0;

  virtual media::base::MEDIA_PLAYER_STATE getState() = 0;

  /**
   * @brief Turn mute on or off
   *
   * @param mute Whether to mute on
   * @return int < 0 on behalf of an error, the value corresponds to one of MEDIA_PLAYER_ERROR
   */
  virtual int mute(bool mute) = 0;

  /**
   * @brief Get mute state
   *
   * @param[out] mute Whether is mute on
   * @return int < 0 on behalf of an error, the value corresponds to one of MEDIA_PLAYER_ERROR
   */
  virtual int getMute(bool& mute) = 0;

  /**
   * @brief Adjust playback volume
   *
   * @param volume The volume value to be adjusted
   * The volume can be adjusted from 0 to 400:
   * 0: mute;
   * 100: original volume;
   * 400: Up to 4 times the original volume (with built-in overflow protection).
   * @return int < 0 on behalf of an error, the value corresponds to one of MEDIA_PLAYER_ERROR
   */
  virtual int adjustPlayoutVolume(int volume) = 0;

  /**
   * @brief Get the current playback volume
   *
   * @param[out] volume
   * @return int < 0 on behalf of an error, the value corresponds to one of MEDIA_PLAYER_ERROR
   */
  virtual int getPlayoutVolume(int& volume) = 0;

  /**
   * @brief adjust publish signal volume
   *
   * @return int < 0 on behalf of an error, the value corresponds to one of MEDIA_PLAYER_ERROR
   */
  virtual int adjustPublishSignalVolume(int volume) = 0;

  /**
   * @brief get publish signal volume
   *
   * @return int < 0 on behalf of an error, the value corresponds to one of MEDIA_PLAYER_ERROR
   */
  virtual int getPublishSignalVolume(int& volume) = 0;

  /**
   * @brief Set video rendering view
   *
   * @param view view object, windows platform is HWND
   * @return int < 0 on behalf of an error, the value corresponds to one of MEDIA_PLAYER_ERROR
   */
  virtual int setView(media::base::view_t view) = 0;

  /**
   * @brief Set video display mode
   *
   * @param renderMode Video display mode
   * @return int < 0 on behalf of an error, the value corresponds to one of MEDIA_PLAYER_ERROR
   */
  virtual int setRenderMode(media::base::RENDER_MODE_TYPE renderMode) = 0;

  /**
   * Registers a media player source observer.
   *
   * Once the media player source observer is registered, you can use the observer to monitor the state change of the media player.
   * @param observer The pointer to the IMediaPlayerSourceObserver object.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int registerPlayerSourceObserver(IMediaPlayerSourceObserver* observer) = 0;

  /**
   * Releases the media player source observer.
   * @param observer The pointer to the IMediaPlayerSourceObserver object.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int unregisterPlayerSourceObserver(IMediaPlayerSourceObserver* observer) = 0;

  /**
   * Register the audio frame observer.
   *
   * @param observer The pointer to the IAudioFrameObserver object.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int registerAudioFrameObserver(media::base::IAudioFrameObserver* observer) = 0;

  /**
   * Releases the audio frame observer.
   * @param observer The pointer to the IAudioFrameObserver object.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int unregisterAudioFrameObserver(media::base::IAudioFrameObserver* observer) = 0;

  /**
   * @brief Register the player video observer
   *
   * @param observer observer object
   * @return int < 0 on behalf of an error, the value corresponds to one of MEDIA_PLAYER_ERROR
   */
  virtual int registerVideoFrameObserver(media::base::IVideoFrameObserver* observer) = 0;

  /**
   * @brief UnRegister the player video observer
   *
   * @param observer observer object
   * @return int < 0 on behalf of an error, the value corresponds to one of MEDIA_PLAYER_ERROR
   */
  virtual int unregisterVideoFrameObserver(agora::media::base::IVideoFrameObserver* observer) = 0;

  virtual const char* getPlayerSdkVersion() = 0;
};

} //namespace rtc
} // namespace agora
