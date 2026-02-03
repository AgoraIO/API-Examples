/**
 *
 * Agora Real Time Engagement
 * Copyright (c) 2024 Agora IO. All rights reserved.
 *
 */
#pragma once
#include <functional>
#include <string>

#include "rte_base/c/c_rte.h"
#include "rte_base/c/c_player.h"

#include "rte_cpp_error.h"
#include "rte_cpp_callback_utils.h"
#include "rte_cpp_canvas.h"
#include "rte_cpp_string.h"
#include "rte_cpp_stream.h"

namespace rte {

using PlayerState = ::RtePlayerState;
using PlayerEvent = ::RtePlayerEvent;
using PlayerMetadataType = ::RtePlayerMetadataType;
using PlayerStats = ::RtePlayerStats;
using PlayerCustomSourceProvider = ::RtePlayerCustomSourceProvider;
using AbrSubscriptionLayer = ::RteAbrSubscriptionLayer;
using AbrFallbackLayer = ::RteAbrFallbackLayer;

class PlayerInitialConfig {};

/**
 * @brief Information about the player and media stream.
 *
 * @since v4.5.1
 */
class PlayerInfo {
 public:
  PlayerInfo() { RtePlayerInfoInit(&c_player_info, nullptr); }
  ~PlayerInfo() { RtePlayerInfoDeinit(&c_player_info, nullptr); }

  PlayerInfo(const RtePlayerInfo* other) {
    RtePlayerInfoInit(&c_player_info, nullptr);
    RtePlayerInfoCopy(&c_player_info, other, nullptr);
  }

  PlayerInfo(const PlayerInfo& other) {
    RtePlayerInfoInit(&c_player_info, nullptr);
    RtePlayerInfoCopy(&c_player_info, &other.c_player_info, nullptr);
  }

  PlayerInfo& operator=(const PlayerInfo& other) {
    RtePlayerInfoCopy(&c_player_info, &other.c_player_info, nullptr);
    return *this;
  }

  PlayerInfo& operator=(const RtePlayerInfo* other) {
    RtePlayerInfoCopy(&c_player_info, other, nullptr);
    return *this;
  }

  /**
   * @brief Gets the current player state.
   *
   * @since v4.5.1
   *
   * @return
   * - If the method call succeeds, returns the current player state. See `RtePlayerState`.
   */
  RtePlayerState State() const {
    return c_player_info.state;
  }

  /**
   * @brief Gets the duration of the current media resource.
   *
   * @since v4.5.1
   *
   * @note
   * Valid only when playing local media files or on-demand streams.
   *
   * @return
   * If the method call succeeds, returns the duration of the current media resource in milliseconds.
   */
  size_t Duration() const {
    return c_player_info.duration;
  }

  /**
   * @brief Gets the number of streams in the current playback source.
   *
   * @since v4.5.1
   *
   * @note
   * This method is valid when opening non-RTE URLs.
   *
   * @return
   * If the method call succeeds, returns the number of streams in the current playback source.
   */
  size_t StreamCount() const {
    return c_player_info.stream_count;
  }

  /**
   * @brief Determines whether the media resource contains an audio stream.
   *
   * @since v4.5.1
   *
   * @details
   * Indicates whether the URL source contains an audio stream.
   *
   * @return
   * - `true`: The media resource contains an audio stream.
   * - `false`: The media resource does not contain an audio stream.
   */
  bool HasAudio() const {
    return c_player_info.has_audio;
  }

  /**
   * @brief Determines whether a video stream is included.
   *
   * @since v4.5.1
   *
   * @return
   * - `true`: The URL source contains a video stream.
   * - `false`: The URL source does not contain a video stream.
   */
  bool HasVideo() const {
    return c_player_info.has_video;
  }

  /**
   * @brief Determines whether the player has stopped receiving the audio stream.
   *
   * @since v4.5.1
   *
   * @details
   * This method is used to determine whether the player has stopped receiving the audio stream.
   *
   * @return
   * - `true`: The player has stopped receiving the audio stream.
   * - `false`: The player is still receiving the audio stream.
   */
  bool IsAudioMuted() const {
    return c_player_info.is_audio_muted;
  }

  /**
   * @brief Determines whether the player has stopped receiving the video stream.
   *
   * @since v4.5.1
   *
   * @note
   * The `IsVideoMuted` API is only valid when opening an RTE URL.
   *
   * @return
   * - `true`: The player has stopped receiving the video stream.
   * - `false`: The player is still receiving the video stream.
   */
  bool IsVideoMuted() const {
    return c_player_info.is_video_muted;
  }

  /**
   * @brief Gets the height of the video resolution.
   *
   * @since v4.5.1
   *
   * @return
   * If the method call succeeds, returns the height of the video resolution in pixels.
   */
  int VideoHeight() const {
    return c_player_info.video_height;
  }

  /**
   * @brief Gets the width of the video resolution.
   *
   * @since v4.5.1
   *
   * @return
   * If the method call succeeds, returns the width of the video resolution in pixels.
   */
  int VideoWidth() const {
    return c_player_info.video_width;
  }

  /**
   * @brief Gets the currently subscribed video layer.
   *
   * @since v4.5.1
   *
   * @note
   * This field is only valid when you open an RTE URL.
   *
   * @return
   * - If the method call succeeds, returns the currently subscribed video layer. See
   * `RteAbrSubscriptionLayer`.
   */
  AbrSubscriptionLayer AbrSubscriptionLayer() const {
    return c_player_info.abr_subscription_layer;
  }

  /**
   * @brief Gets the audio sample rate.
   *
   * @since v4.5.1
   *
   * @return
   * If the method call succeeds, returns the audio sample rate in Hz.
   */
  int AudioSampleRate() const {
    return c_player_info.audio_sample_rate;
  }

  /**
   * @brief Gets the number of audio channels.
   *
   * @since v4.5.1
   *
   * @return
   * If the method call succeeds, returns the number of audio channels.
   */
  int AudioChannels() const {
    return c_player_info.audio_channels;
  }

  /**
   * @brief Gets the number of bits per audio sample.
   *
   * @since v4.5.1
   *
   * @note
   * This field is only valid when opening a non-RTE URL.
   *
   * @return
   * If the method call succeeds, returns the number of bits per audio sample, in bits.
   */
  int AudioBitsPerSample() const {
    return c_player_info.audio_bits_per_sample;
  }

  /**
   * @brief Gets the currently playing URL.
   *
   * @since v4.5.1
   *
   * @return
   * - If the method call succeeds, returns the currently playing URL.
   * - If the method call fails, returns an empty string.
   */
  std::string CurrentUrl() const {
    String str(c_player_info.current_url);
    return std::string(str.CStr());
  }

  /**
   * @brief Sets the current URL.
   * @technical preview
   * @param url The current URL.
   * @return void
   */
  void SetCurrentUrl(const std::string& url) {
    if(c_player_info.current_url != nullptr){
      RteStringDestroy(c_player_info.current_url, nullptr);
      c_player_info.current_url = nullptr;
    }

    c_player_info.current_url = RteStringCreate(nullptr);
    RteStringInitWithCStr(c_player_info.current_url, url.c_str(), nullptr);
  }

  ::RtePlayerInfo *get_underlying_impl() { return &c_player_info; }

  private:
    ::RtePlayerInfo c_player_info;
};

static void onStateChanged(::RtePlayerObserver *observer,
                          RtePlayerState old_state, RtePlayerState new_state,
                          RteError *err);

static void onPositionChanged(::RtePlayerObserver *observer, uint64_t curr_time,
                            uint64_t utc_time);

static void onResolutionChanged(::RtePlayerObserver *observer, int width, int height);

static void onEvent(::RtePlayerObserver *observer, RtePlayerEvent event);

static void onMetadata(::RtePlayerObserver *observer, ::RtePlayerMetadataType type,
                    const uint8_t *data, size_t length);

static void onPlayerInfoUpdated(::RtePlayerObserver *observer, const RtePlayerInfo *info);

static void onAudioVolumeIndication(::RtePlayerObserver *observer, int32_t volume);


/**
 * @brief The PlayerObserver class is used to observe the event of Player object.
 * @since v4.4.0
 */
class PlayerObserver {
 public:
  PlayerObserver() : c_player_observer(::RtePlayerObserverCreate(nullptr)) {

    c_player_observer->base_observer.me_in_target_lang = this;
    
    c_player_observer->on_state_changed = rte::onStateChanged;
    c_player_observer->on_position_changed = rte::onPositionChanged;
    c_player_observer->on_resolution_changed = rte::onResolutionChanged;
    c_player_observer->on_event = rte::onEvent;
    c_player_observer->on_metadata = rte::onMetadata;
    c_player_observer->on_player_info_updated = rte::onPlayerInfoUpdated;
    c_player_observer->on_audio_volume_indication = rte::onAudioVolumeIndication;
  }
  virtual ~PlayerObserver(){ RtePlayerObserverDestroy(c_player_observer, nullptr); }

  // @{
  PlayerObserver(PlayerObserver &other) = delete;
  PlayerObserver(PlayerObserver &&other) = delete;
  PlayerObserver &operator=(const PlayerObserver &cmd) = delete;
  PlayerObserver &operator=(PlayerObserver &&cmd) = delete;
  // @}

  /**
   * @brief Occurs when the player state changes.
   *
   * @since v4.4.0
   *
   * @details
   * If you need to monitor changes in the player state, you must first call `RegisterObserver` to
   * register the player observer object.
   * Call timing: When the player state changes, the SDK triggers this callback to report the current
   * and previous states.
   *
   * @param old_state The previous state of the player. See `RtePlayerState`.
   * @param new_state The current state of the player. See `RtePlayerState`. If the current state is
   * `kRtePlayerStateFailed`, you can get detailed error information from the `err` parameter.
   * @param err Status or error information. See `Error`.
   *
   */
  virtual void onStateChanged(PlayerState old_state, PlayerState new_state,
                    rte::Error *err) {};

  /**
   * @brief Reports the current playback progress of the media resource.
   *
   * @since v4.4.0
   *
   * @details
   * Call timing: This callback is triggered once per second during media playback.
   *
   * @param curr_time Current playback progress in milliseconds.
   * @param utc_time Current NTP (Network Time Protocol) time in milliseconds.
   *
   */
  virtual void onPositionChanged(uint64_t curr_time,
                      uint64_t utc_time) {};

  /**
   * @brief Occurs when the video resolution changes.
   *
   * @since v4.4.0
   *
   * @details
   * If you need to monitor changes in the resolution of the video stream played by the player, you
   * must first call `RegisterObserver` to register the player observer object.
   * Call timing: When the resolution of the video stream changes, the SDK triggers this callback to
   * report the current width and height of the video.
   *
   * @param width Width of the video frame (px).
   * @param height Height of the video frame (px).
   *
   */
  virtual void onResolutionChanged(int width, int height) {};

  /**
   * @brief Callback for player events.
   *
   * @since v4.4.0
   *
   * @details
   * If you need to monitor player events, you must first call `RegisterObserver` to register the
   * player observer object.
   * Call timing: The SDK triggers this callback when a player event occurs.
   *
   * @param event Player event. See `RtePlayerEvent`.
   *
   */
  virtual void onEvent(PlayerEvent event) {};

  /**
   * @brief Callback for received media metadata.
   *
   * @since v4.4.0
   *
   * @details
   * If you need to obtain metadata from the media stream, you must first call `RegisterObserver` to
   * register the player observer object.
   * Call timing: After the SDK parses the metadata from the media stream being played, it triggers
   * this callback to report the metadata type and its content.
   *
   * @param type Metadata type. See `RtePlayerMetadataType`.
   * @param data Parsed metadata.
   * @param length Size of the data in bytes.
   *
   */
  virtual void onMetadata(PlayerMetadataType type,
                      const uint8_t *data, size_t length) {};
  
  /**
   * @brief Occurs when player and media stream information changes.
   *
   * @since v4.4.0
   *
   * @details
   * If you need to get information about the player and media stream, you must first call
   * `RegisterObserver` to register the player observer object. You can also directly retrieve it
   * using the `GetInfo` method.
   * Call timing: The SDK triggers this callback when information about the player or media stream
   * changes.
   *
   * @param info Information about the player and media stream. See `PlayerInfo`.
   *
   */
  virtual void onPlayerInfoUpdated(const PlayerInfo *info) {};

  /**
   * @brief Callback for player audio volume indication.
   *
   * @since v4.4.0
   *
   * @details
   * Call timing: The SDK triggers this callback when the player's volume changes.
   *
   * @param volume The current volume of the player, ranging from [0, 225].
   *
   */
  virtual void onAudioVolumeIndication(int32_t volume) {};

 private:
  friend class Player;

  ::RtePlayerObserver *c_player_observer;
};

void onStateChanged(::RtePlayerObserver *observer,
                          RtePlayerState old_state, RtePlayerState new_state,
                          RteError *err){
  auto *player_observer = static_cast<PlayerObserver *>(observer->base_observer.me_in_target_lang);
  if (player_observer != nullptr){
    Error cpp_err(err);
    player_observer->onStateChanged(old_state, new_state, &cpp_err);
  }
}
void onPositionChanged(::RtePlayerObserver *observer, uint64_t curr_time,
                            uint64_t utc_time){
  auto *player_observer = static_cast<PlayerObserver *>(observer->base_observer.me_in_target_lang);
  if (player_observer != nullptr){
    player_observer->onPositionChanged(curr_time, utc_time);
    }
}

void onResolutionChanged(::RtePlayerObserver *observer, int width, int height){
  auto *player_observer = static_cast<PlayerObserver *>(observer->base_observer.me_in_target_lang);
  if (player_observer != nullptr){
    player_observer->onResolutionChanged(width, height);
  }
}

void onEvent(::RtePlayerObserver *observer, RtePlayerEvent event){
  auto *player_observer = static_cast<PlayerObserver *>(observer->base_observer.me_in_target_lang);
  if (player_observer != nullptr){
    player_observer->onEvent(event);
  }
}

void onMetadata(::RtePlayerObserver *observer, RtePlayerMetadataType type,
                    const uint8_t *data, size_t length){
  auto *player_observer = static_cast<PlayerObserver *>(observer->base_observer.me_in_target_lang);
  if (player_observer != nullptr){
    player_observer->onMetadata(type, data, length);
  }
}

void onPlayerInfoUpdated(::RtePlayerObserver *observer, const RtePlayerInfo *info){
  auto *player_observer = static_cast<PlayerObserver *>(observer->base_observer.me_in_target_lang);
  if (player_observer != nullptr){
    PlayerInfo cpp_info(info);
    player_observer->onPlayerInfoUpdated(&cpp_info);
  }
}

void onAudioVolumeIndication(::RtePlayerObserver *observer, int32_t volume){
  auto *player_observer = static_cast<PlayerObserver *>(observer->base_observer.me_in_target_lang);
  if (player_observer != nullptr){
    player_observer->onAudioVolumeIndication(volume);
  }
}

/**
 * @brief This class provides methods for configuring the player.
 *
 * @since v4.4.0
 */
class PlayerConfig {
 public:
  PlayerConfig() { RtePlayerConfigInit(&c_player_config, nullptr); }
  ~PlayerConfig() { RtePlayerConfigDeinit(&c_player_config, nullptr); }

  // @{
  PlayerConfig(PlayerConfig &other) = delete;
  PlayerConfig(PlayerConfig &&other) = delete;
  PlayerConfig &operator=(PlayerConfig &&cmd) = delete;

  PlayerConfig &operator=(const PlayerConfig &other) {
    RtePlayerConfigCopy(&c_player_config, &other.c_player_config, nullptr);
    return *this;
  };

    PlayerConfig &operator=(const RtePlayerConfig* other) {
    RtePlayerConfigCopy(&c_player_config, other, nullptr);
    return *this;
  };
  // @}

  /**
   * @brief Sets whether to autoplay.
   *
   * @since v4.4.0
   *
   * @details
   * Before you call `OpenWithUrl` to open a media stream, you can call this method to set whether to
   * autoplay. If not set, autoplay is enabled by default.
   * Call timing: This method must be called before `OpenWithUrl`.
   *
   * @param auto_play Whether to enable autoplay:
   * - `true`: (Default) Enable autoplay.
   * - `false`: Disable autoplay.
   * @param err Status or error information. See `Error`.
   *
   */
  void SetAutoPlay(bool auto_play, Error *err = nullptr) {
    RtePlayerConfigSetAutoPlay(&c_player_config, auto_play,
                               err != nullptr ? err->get_underlying_impl() : nullptr);
  } 

  /**
   * @brief Gets the auto-play setting.
   *
   * @since v4.4.0
   *
   * @details
   * You can call this method to check the current auto-play setting of the player.
   * Call timing: Call this method after `GetConfigs(PlayerConfig* config, Error* err)`.
   *
   * @param err Status or error information. See `Error`.
   *
   * @return
   * Whether the auto-play setting is retrieved successfully:
   * - `true`: Retrieved successfully.
   * - `false`: Failed to retrieve.
   */
  bool GetAutoPlay(Error *err = nullptr) {
    bool auto_play;
    RtePlayerConfigGetAutoPlay(&c_player_config, &auto_play,
                               err != nullptr ? err->get_underlying_impl() : nullptr);
    return auto_play;
  }

  /**
   * @brief Sets the playback speed parameter.
   *
   * @since v4.5.1
   *
   * @note
   * You can call this method after calling the `OpenWithUrl` method.
   *
   * @param speed Playback speed. The valid range is [50, 400].
   * @param err Status or error information. See `Error`.
   * - kRteOk: Call succeeded.
   * - kRteErrorInvalidArgument: The `speed` parameter is set to an invalid value.
   *
   */
  void SetPlaybackSpeed(int32_t speed, Error *err = nullptr) {
    RtePlayerConfigSetPlaybackSpeed(&c_player_config, speed,
                                    err != nullptr ? err->get_underlying_impl() : nullptr);
  }

  /**
   * @brief Gets the playback speed parameter.
   *
   * @since v4.5.1
   *
   * @param err Status or error information. See `Error`.
   * - kRteOk: Indicates success.
   *
   * @return
   * If the method call succeeds, returns the playback speed value.
   */
  int32_t GetPlaybackSpeed(Error *err = nullptr) {
    int32_t speed;
    RtePlayerConfigGetPlaybackSpeed(&c_player_config, &speed,
                                    err != nullptr ? err->get_underlying_impl() : nullptr);
    return speed;
  }

  /**
   * Set the playout audio track index parameter.
   * @since v4.4.0
   * @param idx 
   * @param err 
   * @return void
   * @technical preview
   */
  void SetPlayoutAudioTrackIdx(int idx, Error *err = nullptr) {
    RtePlayerConfigSetPlayoutAudioTrackIdx(&c_player_config, idx,
                                          err != nullptr ? err->get_underlying_impl() : nullptr);
  }

  /**
   * Get the playout audio track index parameter.
   * @since v4.4.0
   * @param err 
   * @return int32_t
   * @technical preview
   */
  int32_t GetPlayoutAudioTrackIdx(Error *err = nullptr) {
    int32_t idx;
    RtePlayerConfigGetPlayoutAudioTrackIdx(&c_player_config, &idx,
                                          err != nullptr ? err->get_underlying_impl() : nullptr);
    return idx;
  }

  /**
   * Set the publish audio track index parameter.
   * @since v4.4.0
   * @param idx 
   * @param err 
   * @return void
   * @technical preview
   */
  void SetPublishAudioTrackIdx(int32_t idx, Error *err = nullptr) {
    RtePlayerConfigSetPublishAudioTrackIdx(&c_player_config, idx,
                                          err != nullptr ? err->get_underlying_impl() : nullptr);
  }

  /**
   * Get the publish audio track index parameter.
   * @since v4.4.0
   * @param err 
   * @return int32_t
   * @technical preview
   */
  int32_t GetPublishAudioTrackIdx(Error *err = nullptr) {
    int32_t idx;
    RtePlayerConfigGetPublishAudioTrackIdx(&c_player_config, &idx,
                                          err != nullptr ? err->get_underlying_impl() : nullptr);
    return idx;
  }

  /**
   * Set the audio track index parameter.
   * @since v4.4.0
   * @param idx 
   * @param err 
   * @return void
   * @technical preview
   */
  void SetAudioTrackIdx(int32_t idx, Error *err = nullptr) {
    RtePlayerConfigSetAudioTrackIdx(&c_player_config, idx,
                                  err != nullptr ? err->get_underlying_impl() : nullptr);
  }

  /**
   * Get the audio track index parameter.
   * @since v4.4.0
   * @param err 
   * @return int32_t
   * @technical preview
   */
  int32_t GetAudioTrackIdx(Error *err = nullptr) {
    int32_t idx;
    RtePlayerConfigGetAudioTrackIdx(&c_player_config, &idx,
                                  err != nullptr ? err->get_underlying_impl() : nullptr);
    return idx;
  }

  /**
   * Set the subtitle track index parameter.
   * @since v4.4.0
   * @param idx 
   * @param err 
   * @return void
   * @technical preview
   */
  void SetSubtitleTrackIdx(int32_t idx, Error *err = nullptr) {
    RtePlayerConfigSetSubtitleTrackIdx(&c_player_config, idx,
                                     err != nullptr ? err->get_underlying_impl() : nullptr);
  }

  /**
   * Get the subtitle track index parameter.
   * @since v4.4.0
   * @param err 
   * @return int32_t
   * @technical preview
   */
  int32_t GetSubtitleTrackIdx(Error *err = nullptr) {
    int32_t idx;
    RtePlayerConfigGetSubtitleTrackIdx(&c_player_config, &idx,
                                     err != nullptr ? err->get_underlying_impl() : nullptr);
    return idx;
  }

  /**
   * Set the external subtitle track index parameter.
   * @since v4.4.0
   * @param idx 
   * @param err 
   * @return void
   * @technical preview
   */
  void SetExternalSubtitleTrackIdx(int32_t idx, Error *err = nullptr) {
    RtePlayerConfigSetExternalSubtitleTrackIdx(&c_player_config, idx,
                                           err != nullptr ? err->get_underlying_impl() : nullptr);
  }

  /**
   * Get the external subtitle track index parameter.
   * @since v4.4.0
   * @param err 
   * @return int32_t
   * @technical preview
   */
  int32_t GetExternalSubtitleTrackIdx(Error *err = nullptr) {
    int32_t idx;
    RtePlayerConfigGetExternalSubtitleTrackIdx(&c_player_config, &idx,
                                           err != nullptr ? err->get_underlying_impl() : nullptr);
    return idx;
  }

  /**
   * Set the audio pitch parameter.
   * @since v4.4.0
   * @param audio_pitch 
   * @param err 
   * @return void
   * @technical preview
   */
  void SetAudioPitch(int32_t audio_pitch, Error *err = nullptr) {
    RtePlayerConfigSetAudioPitch(&c_player_config, audio_pitch,
                              err != nullptr ? err->get_underlying_impl() : nullptr);
  }

  /**
   * Get the audio pitch parameter.
   * @since v4.4.0
   * @param err 
   * @return int32_t
   * @technical preview
   */
  int32_t GetAudioPitch(Error *err = nullptr) {
    int32_t audio_pitch;
    RtePlayerConfigGetAudioPitch(&c_player_config, &audio_pitch,
                              err != nullptr ? err->get_underlying_impl() : nullptr);
    return audio_pitch;
  }

  /**
   * @brief Sets the playback volume parameter.
   *
   * @since v4.5.1
   *
   * @details
   * You can use this method to set the playback volume. The valid range is [0, 400].
   *
   * @param volume The volume value to set. The valid range is [0, 400].
   * @param err Status or error information. See `Error`.
   * Possible error codes include:
   * - kRteOk: Setting succeeded.
   * - kRteErrorInvalidArgument: The volume parameter is set to an invalid value.
   *
   */
  void SetPlayoutVolume(int32_t volume, Error *err = nullptr) {
    RtePlayerConfigSetPlayoutVolume(&c_player_config, volume,
                                 err != nullptr ? err->get_underlying_impl() : nullptr);
  }

  /**
   * @brief Gets the playback volume of the player.
   *
   * @since v4.5.1
   *
   * @param err Status or error information. See `Error`.
   * - kRteOk: Indicates success.
   *
   * @return
   * If the method call succeeds, returns the volume value of the player.
   */
  int32_t GetPlayoutVolume(Error *err = nullptr) {
    int32_t volume;
    RtePlayerConfigGetPlayoutVolume(&c_player_config, &volume,
                                 err != nullptr ? err->get_underlying_impl() : nullptr);
    return volume;
  }

  /**
   * Set the audio playback delay parameter.
   * @since v4.4.0
   * @param volume 
   * @param err 
   * @return void
   * @technical preview
   */
  void SetAudioPlaybackDelay(int32_t delay, Error *err = nullptr) {
    RtePlayerConfigSetAudioPlaybackDelay(&c_player_config, delay,
                                     err != nullptr ? err->get_underlying_impl() : nullptr);
  }

  /**
   * Get the audio playback delay parameter.
   * @since v4.4.0
   * @param err 
   * @return int32_t
   * @technical preview
   */
  int32_t GetAudioPlaybackDelay(Error *err = nullptr) {
    int32_t delay;
    RtePlayerConfigGetAudioPlaybackDelay(&c_player_config, &delay,
                                     err != nullptr ? err->get_underlying_impl() : nullptr);
    return delay;
  }

  /**
   * Set the audio dual mono mode parameter.
   * @since v4.4.0
   * @param mode 
   * @param err 
   * @return void
   * @technical preview
   */
  void SetAudioDualMonoMode(RteAudioDualMonoMode mode, Error *err = nullptr) {
    RtePlayerConfigSetAudioDualMonoMode(&c_player_config, mode,
                                    err != nullptr ? err->get_underlying_impl() : nullptr);
  }

  /**
   * Get the audio dual mono mode parameter.
   * @since v4.4.0
   * @param err 
   * @return RteAudioDualMonoMode
   * @technical preview
   */
  RteAudioDualMonoMode GetAudioDualMonoMode(Error *err = nullptr) {
    RteAudioDualMonoMode mode;
    RtePlayerConfigGetAudioDualMonoMode(&c_player_config, &mode,
                                    err != nullptr ? err->get_underlying_impl() : nullptr);
    return mode;
  }

  /**
   * Set the publish volume parameter.
   * @since v4.4.0
   * @param volume 
   * @param err 
   * @return void
   * @technical preview
   */
  void SetPublishVolume(int32_t volume, Error *err = nullptr) {
    RtePlayerConfigSetPublishVolume(&c_player_config, volume,
                                 err != nullptr ? err->get_underlying_impl() : nullptr);
  }

  /**
   * Get the publish volume parameter.
   * @since v4.4.0
   * @param err 
   * @return int32_t
   * @technical preview
   */
  int32_t GetPublishVolume(Error *err = nullptr) {
    int32_t volume;
    RtePlayerConfigGetPublishVolume(&c_player_config, &volume,
                                 err != nullptr ? err->get_underlying_impl() : nullptr);
    return volume;
  }

  /**
   * @brief Sets the loop count for media file playback.
   *
   * @since v4.5.1
   *
   * @param count The number of times the media file should loop.
   * - `1`: Play once.
   * - `2`: Play twice.
   * - `-1`: Loop indefinitely until `Stop` is called.
   * @param err Status or error information. See `Error`.
   * - kRteOk: Call succeeded.
   * - kRteErrorInvalidArgument: The `count` parameter is set to an invalid value.
   *
   */
  void SetLoopCount(int32_t count, Error *err = nullptr) {
    RtePlayerConfigSetLoopCount(&c_player_config, count,
                             err != nullptr ? err->get_underlying_impl() : nullptr);
  }

  /**
   * @brief Gets the loop count parameter of the media file.
   *
   * @since v4.5.1
   *
   * @param err Output parameter, status or error information. See `Error`.
   * - kRteOk: Indicates success.
   *
   * @return
   * If the method call succeeds, returns the loop count of the media file.
   */
  int32_t GetLoopCount(Error *err = nullptr) {
    int32_t count;
    RtePlayerConfigGetLoopCount(&c_player_config, &count,
                             err != nullptr ? err->get_underlying_impl() : nullptr);
    return count;
  }

  /**
   * Set player private parameters. This parameter setting can be done according to actual needs, referring to the suggestions of Agora SA.
   * @since v4.4.0
   * @param json_parameter JSON formatted string
   * @param err Possible return values for ErrorCode:
   *  - kRteOk: Success
   *  - kRteErrorInvalidArgument: Indicates that the json_parameter parameter is empty.
   * @return void
   */
  void SetJsonParameter(const char *json_parameter, Error *err = nullptr) {
    String str(json_parameter);
    RtePlayerConfigSetJsonParameter(&c_player_config, str.get_underlying_impl(), 
                                 err != nullptr ? err->get_underlying_impl() : nullptr);
  }

  /**
   * Get the currently configured private parameters of the player.
   * @since v4.4.0
   * @param err Possible return values for ErrorCode:
   *  - kRteOk: Success
   * @return std::string 
   */
  std::string GetJsonParameter(Error *err = nullptr) {
    String str;
    RtePlayerConfigGetJsonParameter(&c_player_config, str.get_underlying_impl(),
                                 err != nullptr ? err->get_underlying_impl() : nullptr);
    return std::string(str.CStr());
  }

  /**
   * @brief Sets the quality layer of the subscribed media stream.
   *
   * @since v4.4.0
   *
   * @details
   * You can switch between different video stream quality layers based on actual network conditions,
   * each corresponding to a different resolution.
   * By default, after calling this method, the audience can only switch between high and low video
   * stream quality, i.e., between `kRteAbrSubscriptionHigh` and `kRteAbrSubscriptionLow`.
   * If you have higher requirements for video resolution switching, you can `contact technical
   * support` to enable
   * the ABR (Adaptive Bitrate) feature. Once enabled, you can customize the resolution for each video
   * quality layer, and the audience can switch between all video quality layers as needed. For
   * detailed implementation, see `Audience-side URL streaming`.
   * Applicable scenarios: In unstable network environments, the audience can choose an appropriate
   * video quality level based on actual network conditions to ensure a smooth viewing experience.
   * Call timing: This method must be called before `SetConfigs(PlayerConfig* config, Error* err)`.
   *
   * @note If you have not enabled the ABR feature when calling this method, the subscribed video
   * quality layer can only be `kRteAbrSubscriptionHigh` or `kRteAbrSubscriptionLow`.
   *
   * @param abr_subscription_layer The video quality layer to subscribe to. See
   * `RteAbrSubscriptionLayer`.
   *                     When customizing the resolution of `kRteAbrSubscriptionLayer1` to
   * `kRteAbrSubscriptionLayer6`, you can refer to the table below or set it according to your actual
   * needs:
   *                         Note: When customizing resolutions, be sure to sort the video quality
   * layers from highest to lowest resolution, and by frame rate from highest to lowest when
   * resolutions are the same.
   * | Video Quality Layer                | Resolution  |
   * | --------------------------- | --------- |
   * | `kRteAbrSubscriptionLayer1` | 2160p (4K) |
   * | `kRteAbrSubscriptionLayer2` | 1440p (2K) |
   * | `kRteAbrSubscriptionLayer3` | 1080p (HD) |
   * | `kRteAbrSubscriptionLayer4` | 720p (HD)  |
   * | `kRteAbrSubscriptionLayer5` | 540p (SD)  |
   * | `kRteAbrSubscriptionLayer6` | 480p (SD)  |
   * @param err Status or error information. See `Error`.
   *
   */
  void SetAbrSubscriptionLayer(AbrSubscriptionLayer abr_subscription_layer, Error *err = nullptr) {
    RtePlayerConfigSetAbrSubscriptionLayer(&c_player_config, abr_subscription_layer,
                                 err != nullptr ? err->get_underlying_impl() : nullptr);
  }

  /**
   * @brief Gets the quality layer of the subscribed video stream.
   *
   * @since v4.4.0
   *
   * @details
   * Call timing: Call this method after `GetConfigs(PlayerConfig* config, Error* err)`.
   *
   * @param err Status or error information. See `Error`.
   *
   * @return
   * The currently set video quality layer. See `RteAbrSubscriptionLayer`.
   */
  AbrSubscriptionLayer GetAbrSubscriptionLayer(Error *err = nullptr) {
    AbrSubscriptionLayer abr_subscription_layer;
    RtePlayerConfigGetAbrSubscriptionLayer(&c_player_config, &abr_subscription_layer,
                                 err != nullptr ? err->get_underlying_impl() : nullptr);
    return abr_subscription_layer;
  }

  /**
   * @brief Sets the fallback option for the subscribed video stream.
   *
   * @since v4.4.0
   *
   * @details
   * In poor network conditions, the quality of real-time audio and video may degrade. After calling
   * this method, the SDK will downgrade the resolution of the video stream to the specified fallback
   * quality layer when the network condition is poor. Different quality layers correspond to
   * different resolutions and bitrates. At the same time, the SDK
   * continuously monitors the network quality and restores the video stream to the subscribed quality
   * layer when the network improves.
   * By default, after calling this method, you can only choose to fallback to `kRteAbrFallbackLow` or
   * `kRteAbrFallbackAudioOnly`. If you have higher requirements for video experience, you can
   * `contact technical support`
   * to enable the ABR feature. Once enabled, you can fallback to video streams of all quality layers
   * and customize the resolution for each layer.
   * Applicable scenarios: - In general scenarios, you can call this method and set the fallback
   * quality layer to `kRteAbrFallbackLow` or
   * `kRteAbrFallbackAudioOnly`, and the SDK will fallback to a low-quality video stream or receive
   * only the audio stream when the network is poor.
   * - If you have higher resolution requirements for fallback video streams, you can `contact
   * technical support` to enable the ABR feature. After enabling, you can customize the resolution
   * for each layer. The SDK
   * will use the resolution of the lowest quality layer you specify as the lower limit and
   * dynamically adjust the resolution within this range based on network conditions. For detailed
   * implementation, see `Audience-side URL streaming`.
   * Call timing: This method must be called before `SetConfigs(PlayerConfig* config, Error* err)`.
   *
   * @note If you have not enabled the ABR feature when calling this method, the fallback quality
   * layer for the video stream can only be `kRteAbrFallbackLow` or `kRteAbrFallbackAudioOnly`.
   *
   * @param abr_fallback_layer The fallback quality layer of the video stream. See `RteAbrFallbackLayer`. When customizing the resolution for `kRteAbrFallbackLayer1` to `kRteAbrFallbackLayer6`, you can refer to the table below, or set it according to your actual needs: | Video Quality Layer                | Resolution  |
   * | --------------------------- | --------- |
   * | `kRteAbrSubscriptionLayer1` | 2160p (4K) |
   * | `kRteAbrSubscriptionLayer2` | 1440p (2K) |
   * | `kRteAbrSubscriptionLayer3` | 1080p (HD) |
   * | `kRteAbrSubscriptionLayer4` | 720p (HD)  |
   * | `kRteAbrSubscriptionLayer5` | 540p (SD)  |
   * | `kRteAbrSubscriptionLayer6` | 480p (SD)  |
   * Note: When customizing resolutions, be sure to sort the video quality layers from highest to
   * lowest resolution, and by frame rate from highest to lowest when resolutions are the same.
   * @param err Status or error information. See `Error`.
   *
   */
  void SetAbrFallbackLayer(AbrFallbackLayer abr_fallback_layer, Error *err = nullptr) {
    RtePlayerConfigSetAbrFallbackLayer(&c_player_config, abr_fallback_layer,
                                 err != nullptr ? err->get_underlying_impl() : nullptr);
  }


  /**
   * @brief Gets the configured video stream fallback option.
   *
   * @since v4.4.0
   *
   * @details
   * Call timing: Call this method after `GetConfigs(PlayerConfig* config, Error* err)`.
   *
   * @param err Status or error information. See `Error`.
   *
   * @return
   * The configured video stream fallback option. See `RteAbrFallbackLayer`.
   */
  AbrFallbackLayer GetAbrFallbackLayer(Error *err = nullptr) {
    AbrFallbackLayer abr_fallback_layer;
    RtePlayerConfigGetAbrFallbackLayer(&c_player_config, &abr_fallback_layer,
                                 err != nullptr ? err->get_underlying_impl() : nullptr);
    return abr_fallback_layer;
  }


  ::RtePlayerConfig* get_underlying_impl() { return &c_player_config; }

 private:
  friend class Player;

  ::RtePlayerConfig c_player_config;
};

/**
 * The Player class can be used to play URL resources.
 * @since v4.4.0
 */
class Player {
 public:
  /**
   * @brief Constructs a player object.
   *
   * @since v4.4.0
   *
   * @details
   * Call timing: This method must be called after `InitMediaEngine`.
   *
   * @param self An `Rte` object.
   * @param config Initial configuration for the player object. You can pass a null pointer.
   *
   */
  explicit Player(Rte *self, PlayerInitialConfig *config = nullptr) 
  : c_player(::RtePlayerCreate(&self->c_rte, nullptr, nullptr)) {};
  ~Player() { 
      RtePlayerDestroy(&c_player, nullptr); 
  };

  Player(Player &other) = default;
  Player(Player &&other) = default;

  // @{
  Player &operator=(const Player &cmd) = delete;
  Player &operator=(Player &&cmd) = delete;
  // @}

  /**
   * @brief Preloads a URL resource.
   *
   * @since v4.4.0
   *
   * @details
   * After successful preloading, the speed of opening the URL resource via `OpenWithUrl` can be
   * improved. When these resources are needed, they can be accessed more quickly, reducing waiting
   * time.
   * Applicable scenarios: Preloading media resources before opening them can reduce user wait time
   * and provide a smoother audiovisual experience.
   * Call timing: This method must be called before `OpenWithUrl`.
   *
   * @note This method currently only works for URLs prefixed with `rte://`. A maximum of 20 URLs can
   * be preloaded. If the limit is exceeded, the earliest URL will be replaced by the newly preloaded
   * one.
   *
   * @param url A URL prefixed with `rte://`. For details on the fields, see `Audience-side URL
   * streaming`.
   * @param err Status or error information. See `Error`.
   *
   * @return
   * Whether the preload is successful:
   * - `true`: Preload succeeded.
   * - `false`: Preload failed.
   */
  static bool PreloadWithUrl(const char* url, Error *err = nullptr)  {
    return RtePlayerPreloadWithUrl(nullptr, url, err != nullptr ? err->get_underlying_impl() : nullptr);
  };

  /**
   * @brief Opens a URL resource.
   *
   * @since v4.4.0
   *
   * @details
   * This method supports opening real-time streaming media via URL. If you want to speed up the URL
   * opening process, you can call `PreloadWithUrl` to preload the resource before calling this
   * method.
   * If the resource fails to open, you will receive the `onStateChanged` callback with the state
   * reported as `kRtePlayerStateFailed`. In this case, you need to call `Stop` first and then call
   * `OpenWithUrl` again to reopen the URL resource. If you have disabled autoplay, you can call
   * `Play` to start playback after the resource is opened.
   * Call timing: This method must be called after `Player`.
   * Related callbacks: After calling this method, the `onStateChanged` callback is triggered to
   * report the player state as `kRtePlayerStateOpening`, indicating the URL is being opened.
   * Upon successful opening, the player state is reported as `kRtePlayerStateOpenCompleted`.
   *
   * @note This method currently only supports URLs that start with `rte://`, CDN URLs, and local
   * media files.
   *
   * @param url The URL to open. It can be a CDN URL, local media file, or a URL prefixed with
   * `rte://`. For details on the fields of an RTE URL, see `Audience-side URL streaming`.
   * @param start_time The start playback position, in milliseconds.
   * @param cb An asynchronous callback function used to notify the result of opening the URL
   * resource. If an error occurs during the opening process, you can get the specific error
   * information through the `err` parameter in the callback. See `Error`.
   *
   */
  void OpenWithUrl(const char* url, uint64_t start_time, std::function<void(rte::Error* err)> cb)  {
    CallbackContext<Player>* callbackCtx = new CallbackContext<Player>(this, cb);
    RtePlayerOpenWithUrl(&c_player, url, start_time, &CallbackFunc<::RtePlayer, Player>, callbackCtx);
  };

  /**
   * Open a custom source provider.
   * @since v4.4.0
   * @param provider 
   * @param start_time 
   * @param cb 
   * @return void
   * @technical preview
   */
  void OpenWithCustomSourceProvider(PlayerCustomSourceProvider* provider, uint64_t start_time,
                                    std::function<void(rte::Error* err)> cb) {
    CallbackContext<Player>* callbackCtx = new CallbackContext<Player>(this, cb);
    RtePlayerOpenWithCustomSourceProvider(&c_player, provider, start_time, &CallbackFunc<::RtePlayer, Player>, callbackCtx); 
  };

  /**
   * Open a stream.
   * @since v4.4.0
   * @param stream 
   * @param cb 
   * @return void
   * @technical preview
   */
  void OpenWithStream(Stream* stream, std::function<void(rte::Error* err)> cb)  {
    CallbackContext<Player>* callbackCtx = new CallbackContext<Player>(this, cb);
    RtePlayerOpenWithStream(&c_player, stream != nullptr ? &stream->c_rte_stream : nullptr, &CallbackFunc<::RtePlayer, Player>, callbackCtx);
  };


  /**
   * @brief Switches to a new URL during playback.
   *
   * @since v4.5.1
   *
   * @note
   * This method is only valid when the player has opened a non-RTE URL. Call this method when the SDK
   * reports the player state as `kRtePlayerStateOpenCompleted`.
   *
   * @param url The new URL to switch to.
   * @param sync_pts - `true`: Synchronize playback position.
   * - `false`: (Default) Do not synchronize playback position.
   * @param cb Callback for asynchronously notifying the result of the switch operation. You can get
   * the result or error code through the `err` parameter in the callback. See `Error`.
   * Possible values for `err` include:
   * - kRteOk: Switch succeeded.
   * - kRteErrorDefault: Switch failed.
   * - kRteErrorInvalidArgument: The provided URL is empty or has an invalid format.
   * - kRteErrorInvalidOperation:
   *   - The corresponding internal `Player` object has been destroyed or is invalid.
   *   - The currently opened URL is an `Rte` URL, which does not support switching.
   *
   */
  void SwitchWithUrl(const char* url, bool sync_pts, std::function<void(rte::Error* err)> cb){
    CallbackContext<Player>* callbackCtx = new CallbackContext<Player>(this, cb);
    RtePlayerSwitchWithUrl(&c_player, url, sync_pts, &CallbackFunc<::RtePlayer, Player>, callbackCtx);
  }

  /**
   * @brief Gets statistics of the media resource currently being played by the player.
   *
   * @since v4.4.0
   *
   * @details
   * This method is used to obtain player statistics, including decoding, rendering frame rate, audio
   * and video bitrates, etc., and returns the result asynchronously via a callback function.
   * Call timing: This method must be called after `OpenWithUrl`.
   * @param cb An asynchronous callback function used to notify the result and error information when
   * retrieving player media resource statistics. The function includes the following parameters:
   * - `stats`: Statistical data of the media resource currently being played by the player. See
   * `RtePlayerStats`.
   * - `err`: Error information. See `Error`.
   *
   */
  void GetStats(std::function<void(rte::PlayerStats *stats, rte::Error *err)> cb){
    CallbackContextWithArgs<Player, rte::PlayerStats*> *ctx = new CallbackContextWithArgs<Player, rte::PlayerStats*>(this, cb);
    RtePlayerGetStats(&c_player, &CallbackFuncWithArgs<::RtePlayer, Player, rte::PlayerStats*>, ctx);
  }

  /**
   * @brief Sets the view window for displaying video.
   *
   * @since v4.4.0
   *
   * @details
   * This method is used to specify a `Canvas` object to display video. Once the video stream is
   * successfully played, the video will be shown on the specified `Canvas`.
   * Call timing: This method must be called after `Player`.
   *
   * @param canvas The `Canvas` object used to render video frames. See `Canvas`.
   * @param err Status or error information. See `Error`.
   *
   * @return
   * Whether the setting is successful:
   * - `true`: Setting succeeded.
   * - `false`: Setting failed. You can check the error code returned in the `err` parameter for
   * details.
   */
  bool SetCanvas(Canvas *canvas, Error *err = nullptr)  {
    return RtePlayerSetCanvas(&c_player, canvas != nullptr ? &canvas->c_canvas : nullptr, err != nullptr ? err->get_underlying_impl() : nullptr);
  };

  /**
   * @brief Plays a URL resource.
   *
   * @since v4.4.0
   *
   * @details
   * If you have disabled autoplay, after successfully calling `OpenWithUrl` to open a real-time
   * stream, you can call this method to start playback.
   * Call timing: Call this method after receiving the `onStateChanged` callback reporting the state
   * as `kRtePlayerStateOpenCompleted`.
   * Related callbacks: After this method is successfully called, the `onStateChanged` callback is
   * triggered to report the player state.
   *
   * @note This method currently only supports URLs prefixed with `rte://`.
   *
   * @param err Status or error information. See `Error`.
   *
   * @return
   * Whether playback is successful:
   * - `true`: Playback succeeded.
   * - `false`: Playback failed.
   */
  bool Play(Error *err = nullptr)  {
    return RtePlayerPlay(&c_player, err != nullptr ? err->get_underlying_impl() : nullptr);
  }

  /**
   * @brief Stops media playback.
   *
   * @since v4.4.0
   *
   * @details
   * After successfully opening a URL stream and calling `Play`, you can call this method to stop
   * playback. If you want to pause playback, call `Pause` instead.
   * If you fail to open the URL stream using `OpenWithUrl`, you need to call this method first before
   * calling `OpenWithUrl` again to reopen the URL.
   * Call timing: This method must be called after `OpenWithUrl`.
   * Related callbacks: After this method is successfully called, the `onStateChanged` callback is
   * triggered and reports the player state as `kRtePlayerStateStopped`.
   *
   * @param err Status or error information. See `Error`.
   *
   * @return
   * Whether the playback is successfully stopped:
   * - `true`: Playback stopped successfully.
   * - `false`: Failed to stop playback.
   */
  bool Stop(Error *err = nullptr)  {
    return RtePlayerStop(&c_player, err != nullptr ? err->get_underlying_impl() : nullptr);
  }

  /**
   * @brief Pauses playback.
   *
   * @since v4.4.0
   *
   * @details
   * After you call `Play` to play a URL resource, you can call this method to pause playback. If you
   * want to stop playback, call `Stop`.
   * Call timing: This method must be called after `Play`.
   * Related callbacks: After this method is successfully called, the `onStateChanged` callback is
   * triggered to report the player state as `kRtePlayerStatePaused`.
   *
   * @param err Status or error information. See `Error`.
   *
   * @return
   * Whether the playback is successfully paused:
   * - `true`: Playback paused successfully.
   * - `false`: Failed to pause playback.
   */
  bool Pause(Error *err = nullptr)  {
    return RtePlayerPause(&c_player, err != nullptr ? err->get_underlying_impl() : nullptr);
  }

  /**
   * @brief Seeks to a new playback position.
   *
   * @since v4.5.1
   *
   * @param new_time The new playback position to seek to.
   * @param err Output parameter indicating the error code:
   * - kRteOk: Success.
   * - kRteErrorInvalidOperation:
   *   - The corresponding internal `Player` object has been destroyed or is invalid.
   *   - The opened URL is an RTE URL, which does not support seeking. See `Error`.
   *
   *  @return
   * Whether the SDK successfully sought to the specified playback position:
   * - `true`: Successfully sought to the playback position.
   * - `false`: Failed to seek to the playback position.
   *
   */
  bool Seek(uint64_t new_time, Error *err = nullptr)  {
    return RtePlayerSeek(&c_player, new_time, err != nullptr ? err->get_underlying_impl() : nullptr);
  }

  /**
   * @brief Receives or stops receiving the audio stream.
   *
   * @since v4.4.0
   *
   * @details
   * Call timing: This method must be called after `OpenWithUrl`.
   * Related callbacks: After this method is successfully called, the `onPlayerInfoUpdated(const
   * PlayerInfo *info)` callback is triggered to report the latest player and media stream
   * information.
   *
   * @param mute Whether to receive the audio stream:
   * - `true`: Do not receive the audio stream.
   * - `false`: Receive the audio stream.
   * @param err Status or error information. See `Error`.
   *
   * @return
   * Whether the subscribe or unsubscribe operation succeeds:
   * - `true`: Operation succeeded.
   * - `false`: Operation failed.
   */
  bool MuteAudio(bool mute, Error *err = nullptr)  {
    return RtePlayerMuteAudio(&c_player, mute, err != nullptr ? err->get_underlying_impl() : nullptr);
  }

  /**
   * @brief Receives or stops receiving the video stream.
   *
   * @since v4.4.0
   *
   * @details
   * Call timing: This method must be called after `OpenWithUrl`.
   * Related callbacks: After this method is successfully called, the `onPlayerInfoUpdated(const
   * PlayerInfo *info)` callback is triggered to report the latest player and media stream
   * information.
   *
   * @param mute Whether to receive the video stream:
   * - `true`: Do not receive the video stream.
   * - `false`: Receive the video stream.
   * @param err Status or error information. See `Error`.
   *
   * @return
   * Whether the subscribe or unsubscribe operation succeeds:
   * - `true`: Operation succeeded.
   * - `false`: Operation failed.
   */
  bool MuteVideo(bool mute, Error *err = nullptr)  {
    return RtePlayerMuteVideo(&c_player, mute, err != nullptr ? err->get_underlying_impl() : nullptr);
  }


  /**
   * @brief Gets the current playback position.
   *
   * @since v4.5.1
   *
   * @details
   * This method is used to get the current playback time position, in milliseconds.
   *
   * @param err Status or error information. See `Error`.
   * Possible error codes include:
   * - kRteOk: Call succeeded.
   * - kRteErrorInvalidOperation:
   *   - The corresponding internal `Player` object has been destroyed or is invalid.
   *   - The opened URL is an RTE URL, which does not support calling `GetPosition`.
   *
   * @return
   * If the method call succeeds, returns the current playback position in milliseconds.
   */
  uint64_t GetPosition(Error *err = nullptr){
    return RtePlayerGetPosition(&c_player, err != nullptr ? err->get_underlying_impl() : nullptr);
  }

  /**
   * @brief Gets information about the player and media stream.
   *
   * @since v4.4.0
   *
   * @details
   * You can use this method to get information about the player and media stream, such as audio
   * sample rate, video frame size, and more.
   * Call timing: Call this method after `Player`.
   *
   * @param info Information about the player and media stream. See `PlayerInfo`.
   * @param err Status or error information. See `Error`.
   *
   * @return
   * Whether the information is retrieved successfully:
   * - `true`: Retrieved successfully.
   * - `false`: Failed to retrieve.
   */
  bool GetInfo(PlayerInfo *info, Error *err = nullptr){
    return RtePlayerGetInfo(&c_player, info != nullptr ? info->get_underlying_impl() : nullptr, err != nullptr ? err->get_underlying_impl() : nullptr);
  }

  /**
   * @brief Gets the current player settings.
   *
   * @since v4.4.0
   *
   * @details
   * Call timing: Call this method after `Player`.
   *
   * @param config The player settings object. See `PlayerConfig`.
   * @param err Status or error information. See `Error`.
   *
   * @return
   * Whether the configuration is retrieved successfully:
   * - `true`: Retrieved successfully.
   * - `false`: Failed to retrieve.
   */
  bool GetConfigs(PlayerConfig* config, Error *err = nullptr)  {
    return RtePlayerGetConfigs(&c_player, config->get_underlying_impl(), err != nullptr ? err->get_underlying_impl() : nullptr);
  }

  /**
   * @brief Configures the player.
   *
   * @since v4.4.0
   *
   * @details
   * You can call this method to configure the player, such as enabling autoplay, subscribing to video
   * streams of different resolutions and bitrates, etc.
   * Call timing: This method must be called after `Player`.
   *
   * @param config Player configuration object. See `PlayerConfig`.
   * @param err Status or error information. See `Error`.
   *
   * @return
   * Whether the player configuration is successful:
   * - `true`: Configuration succeeded.
   * - `false`: Configuration failed.
   */
  bool SetConfigs(PlayerConfig* config, Error *err = nullptr)  {
    return RtePlayerSetConfigs(&c_player, config->get_underlying_impl(), err != nullptr ? err->get_underlying_impl() : nullptr);
  }

  /**
   * @brief Registers a player observer object.
   *
   * @since v4.4.0
   *
   * @details
   * Before calling this method, you need to implement an interface class that inherits from
   * `PlayerObserver`.
   * Call timing: This method must be called after `Player`.
   *
   * @param observer An instance of the interface object. See `PlayerObserver`.
   * @param err Status or error information. See `Error`.
   *
   * @return
   * Whether the registration is successful:
   * - `true`: Registration succeeded.
   * - `false`: Registration failed.
   */
  bool RegisterObserver(PlayerObserver *observer, Error *err = nullptr) {
    return RtePlayerRegisterObserver(
        &c_player, observer != nullptr ? observer->c_player_observer : nullptr, err != nullptr ? err->get_underlying_impl() : nullptr);
  }

  /**
   * @brief Unregisters the player event observer object.
   *
   * @since v4.4.0
   *
   * @details
   * After calling `RegisterObserver` to register a player event observer, call this method if you
   * need to unregister it.
   * Call timing: This method must be called after `RegisterObserver`.
   *
   * @param observer Instance of the interface object. See `PlayerObserver`.
   * @param err Status or error information. See `Error`.
   *
   * @return
   * Whether the unregistration is successful:
   * - `true`: Unregistration succeeded.
   * - `false`: Unregistration failed.
   */
  bool UnregisterObserver(PlayerObserver *observer, Error *err = nullptr){
    return RtePlayerUnregisterObserver(&c_player, observer != nullptr ? observer->c_player_observer : nullptr,
                                err != nullptr ? err->get_underlying_impl() : nullptr);
  }


 private:
  ::RtePlayer c_player;
};

}  // namespace rte
