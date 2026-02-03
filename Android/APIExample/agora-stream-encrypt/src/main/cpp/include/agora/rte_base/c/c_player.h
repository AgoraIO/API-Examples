/**
 *
 * Agora Real Time Engagement
 * Copyright (c) 2024 Agora IO. All rights reserved.
 *
 */
#pragma once

#include <stddef.h>

#include "handle.h"
#include "observer.h"
#include "utils/string.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct Rte Rte;
typedef struct RteStream RteStream;
typedef struct RtePlayerInternal RtePlayerInternal;

/**
 * @brief Player state.
 *
 * @since v4.4.0
 */
typedef enum RtePlayerState {
  /**
   * 0: Idle state.
   */
  kRtePlayerStateIdle,
  /**
   * 1: Opening URL resource. This state is reported after calling `OpenWithUrl`.
   */
  kRtePlayerStateOpening,
  /**
   * 2: URL resource opened successfully. This state is reported after `OpenWithUrl` successfully
   * opens the resource.
   */
  kRtePlayerStateOpenCompleted,
  /**
   * 3: Playing.
   */
  kRtePlayerStatePlaying,
  /**
   * 4: Playback paused. This state is reported after successfully calling `Pause`.
   */
  kRtePlayerStatePaused,
  /**
   * 5: Playback completed.
   */
  kRtePlayerStatePlaybackCompleted,
  /**
   * 6: Playback stopped. This state is reported after successfully calling `Stop`.
   */
  kRtePlayerStateStopped,
  /**
   * 7: Failed state. This state is reported when an internal error occurs. If you receive this state,
   * call `Stop` first and then `OpenWithUrl` to reopen the resource.
   */
  kRtePlayerStateFailed
} RtePlayerState;

/**
 * @brief Player event types.
 *
 * @since v4.4.0
 */
typedef enum RtePlayerEvent {
  /**
   * 0: Start seeking to a specified position for playback.
   */
  kRtePlayerEventSeekBegin = 0,
  /**
   * 1: Seek to the specified position completed.
   */
  kRtePlayerEventSeekComplete = 1,
  /**
   * 2: Error occurred while seeking to the specified position.
   */
  kRtePlayerEventSeekError = 2,
  /**
   * 3: The current buffer is insufficient for playback.
   */
  kRtePlayerEventBufferLow = 3,
  /**
   * 4: The current buffer is just enough to support playback.
   */
  kRtePlayerEventBufferRecover = 4,
  /**
   * 5: Audio or video stutter detected.
   */
  kRtePlayerEventFreezeStart = 5,
  /**
   * 6: Audio and video stutter stopped.
   */
  kRtePlayerEventFreezeStop = 6,
  /**
   * 7: One loop of playback completed.
   */
  kRtePlayerEventOneLoopPlaybackCompleted = 7,
  /**
   * 8: Token is about to expire. You need to regenerate a new token and update the URL via
   * `OpenWithUrl`.
   */
  kRtePlayerEventAuthenticationWillExpire = 8,
  /**
   * 9: Due to network issues, fallback from receiving audio and video to receiving audio only.
   */
  kRtePlayerEventAbrFallbackToAudioOnlyLayer = 9,
  /**
   * 10: After network recovers, resume from receiving audio only to receiving audio and video.
   */
  kRtePlayerEventAbrRecoverFromAudioOnlyLayer = 10,
  /**
   * 11: Start switching to a new URL.
   */
  kRtePlayerEventSwitchBegin = 11,
  /**
   * 12: Switch to the new URL completed.
   */
  kRtePlayerEventSwitchComplete = 12,
  /**
   * 13: Error occurred while switching to the new URL.
   */
  kRtePlayerEventSwitchError = 13,
  /**
   * 14: First video frame displayed.
   */
  kRtePlayerEventFirstDisplayed = 14,
  /**
   * 15: Maximum number of cache files reached.
   */
  kRtePlayerEventReachCacheFileMaxCount = 15,
  /**
   * 16: Maximum cache file size reached.
   */
  kRtePlayerEventReachCacheFileMaxSize = 16,
  /**
   * 17: Start trying to open a new URL.
   */
  kRtePlayerEventTryOpenStart = 17,
  /**
   * 18: Successfully opened the new URL.
   */
  kRtePlayerEventTryOpenSucceed = 18, 
  /**
   * 19: Failed to open the new URL.
   */
  kRtePlayerEventTryOpenFailed = 19,
  /**
   * 20: The current audio track has changed.
   */
  kRtePlayerEventAudioTrackChanged = 20,
} RtePlayerEvent;

/**
 * @brief Quality layer of the subscribed video stream.
 *
 * @since v4.4.0
 *
 * @note To customize the resolution for layers `kRteAbrSubscriptionLayer1` to
 * `kRteAbrSubscriptionLayer6` and subscribe to them, please `contact technical support` to enable
 * the ABR feature.
 */
typedef enum RteAbrSubscriptionLayer {
  /**
   * 0: Highest quality video stream. This layer has the highest resolution.
   */
  kRteAbrSubscriptionHigh = 0,
  /**
   * 1: (Default) Lowest quality video stream. This layer has the lowest resolution.
   */
  kRteAbrSubscriptionLow = 1,
  /**
   * 2: Video quality layer 1. This layer has resolution just below `kRteAbrSubscriptionHigh`.
   */
  kRteAbrSubscriptionLayer1 = 2,
  /**
   * 3: Video quality layer 2. This layer has resolution lower than `kRteAbrSubscriptionLayer1`.
   */
  kRteAbrSubscriptionLayer2 = 3,
  /**
   * 4: Video quality layer 3. This layer has resolution lower than `kRteAbrSubscriptionLayer2`.
   */
  kRteAbrSubscriptionLayer3 = 4,
  /**
   * 5: Video quality layer 4. This layer has resolution lower than `kRteAbrSubscriptionLayer3`.
   */
  kRteAbrSubscriptionLayer4 = 5,
  /**
   * 6: Video quality layer 5. This layer has resolution lower than `kRteAbrSubscriptionLayer4`.
   */
  kRteAbrSubscriptionLayer5 = 6,
  /**
   * 7: Video quality layer 6. This layer has resolution lower than `kRteAbrSubscriptionLayer5`.
   */
  kRteAbrSubscriptionLayer6 = 7,
} RteAbrSubscriptionLayer;


/**
 * @brief Fallback quality layer for video streams.
 *
 * @since v4.4.0
 *
 * @note
 * - To use `kRteAbrFallbackLayer1` to `kRteAbrFallbackLayer6`, you must `contact technical support`
 * to enable the ABR feature.
 *   Once enabled, you can customize the resolution for each layer and select any layer as the
 * **lowest resolution** for video fallback. When the network condition is poor, the SDK dynamically
 * adjusts the resolution within this range, using the selected layer as the lower limit.
 * - When customizing resolutions, be sure to sort the video quality layers from highest to lowest
 * resolution. If resolutions are the same, sort by frame rate from highest to lowest.
 */
typedef enum RteAbrFallbackLayer {
  /**
   * 0: No fallback for audio and video streams, but quality is not guaranteed.
   */
  kRteAbrFallbackDisabled = 0,
  /**
   * 1: (Default) Fallback to the lowest quality video stream. This layer has the lowest resolution.
   */
  kRteAbrFallbackLow = 1,
  /**
   * 2: First try to receive the lowest quality video stream; if the network is too poor to display
   * video, fallback to receiving only the subscribed audio stream.
   */
  kRteAbrFallbackAudioOnly = 2,
  /**
   * 3: Fallback to video quality layer 1. This layer has resolution and bitrate just below the
   * highest subscribed video quality.
   */
  kRteAbrFallbackLayer1 = 3,
  /**
   * 4: Fallback to video quality layer 2. This layer has resolution just below layer 1.
   */
  kRteAbrFallbackLayer2 = 4,
  /**
   * 5: Fallback to video quality layer 3. This layer has resolution just below layer 2.
   */
  kRteAbrFallbackLayer3 = 5,
  /**
   * 6: Fallback to video quality layer 4. This layer has resolution just below layer 3.
   */
  kRteAbrFallbackLayer4 = 6,
  /**
   * 7: Fallback to video quality layer 5. This layer has resolution just below layer 4.
   */
  kRteAbrFallbackLayer5 = 7,
  /**
   * 8: Fallback to video quality layer 6. This layer has resolution just below layer 5.
   */
  kRteAbrFallbackLayer6 = 8,
} RteAbrFallbackLayer;

/**
 * Player information.
 * When playerInfo changes, it will be notified through the PlayerObserver::onPlayerInfoUpdated callback interface. 
 * It can also be actively obtained through the Player::GetInfo interface.
 * @since v4.4.0
 */
typedef struct RtePlayerInfo {
  /** 
   * Current player state
   */
  RtePlayerState state;
  /** 
   * Duration time of the current media source. This is valid when playing local media files or on-demand streams
   */
  size_t duration;
  /** 
   * Stream count. This field is only valid when opening a non-RTE URL.
   */
  size_t stream_count;
  /** 
   * Whether there is an audio stream. Indicates whether the url source contains the audio stream.
   * - true: The url source contains the audio stream.
   * - false: The url source does not contain the audio stream.
   */
  bool has_audio;
  /** 
   * Whether there is a video stream. Indicates whether the url source contains the video stream.
   * - true: The url source contains the video stream.
   * - false: The url source does not contain the video stream.
   */
  bool has_video;
  /** 
   * Whether the audio is muted. Indicates whether the receiver end stops receiving the audio stream.
   * - true: Stop receiving the audio stream.
   * - false: Continue receiving the audio stream.
   */
  bool is_audio_muted;
  /** 
   * Whether the video is muted. Indicates whether the receiver end stops receiving the video stream. This field is only valid when you open an RTE URL.
   * - true: Stop receiving the video stream.
   * - false: Continue receiving the video stream.
   */
  bool is_video_muted;
  /** 
   * Video resolution height
   */
  int video_height;
  /** 
   * Video resolution width
   */
  int video_width;
  /** 
   * The currently subscribed video layer. This field is only valid when you open an RTE URL.
   */
  RteAbrSubscriptionLayer abr_subscription_layer;
  /** 
   * Audio sample rate
   */
  int audio_sample_rate;
  /** 
   * Number of audio channels.
   */
  int audio_channels;
  /** 
   * Audio bits per sample. This field is only valid when opening a non-RTE URL.
   */
  int audio_bits_per_sample;
  /**
   * The URL being played.
   */
  RteString *current_url;

} RtePlayerInfo;

/**
 * @brief Statistics of the media resource being played.
 *
 * @since v4.4.0
 */
typedef struct RtePlayerStats {
  /**
   * Video decode frame rate (fps).
   */
  int video_decode_frame_rate;
  /**
   * Video render frame rate (fps).
   */
  int video_render_frame_rate;
  /**
   * Video bitrate (Kbps).
   */
  int video_bitrate;

  /**
   * Audio bitrate (Kbps).
   */
  int audio_bitrate;
} RtePlayerStats;

typedef struct RteMediaTrackInfo {
  void *placeholder;
} RteMediaTrackInfo;

/**
 * @brief Type of media stream metadata.
 */
typedef enum RtePlayerMetadataType {
  /**
   * 0: SEI (Supplemental Enhancement Information) type.
   */
  kRtePlayerMetadataTypeSei
} RtePlayerMetadataType;

typedef enum RteAudioDualMonoMode {
  RteAudioDualMonoStereo,
  RteAudioDualMonoLeft,
  RteAudioDualMonoRight,
  RteAudioDualMonoMix,
} RteAudioDualMonoMode;

typedef struct RtePlayerInitialConfig {
  bool enable_cache;
  bool _enable_cache_is_set;

  bool enable_multiple_audio_track;
  bool _enable_multiple_audio_track_is_set;

  bool is_agora_source;
  bool _is_agora_source_is_set;

  bool is_live_source;
  bool _is_live_source_is_set;
} RtePlayerInitialConfig;

typedef struct RtePlayerConfig {
  bool auto_play;
  bool _auto_play_is_set;

  int32_t speed;
  bool _speed_is_set;

  int32_t playout_audio_track_idx;
  bool _playout_audio_track_idx_is_set;

  int32_t publish_audio_track_idx;
  bool _publish_audio_track_idx_is_set;

  int32_t subtitle_track_idx;
  bool _subtitle_track_idx_is_set;

  int32_t external_subtitle_track_idx;
  bool _external_subtitle_track_idx_is_set;

  int32_t audio_pitch;
  bool _audio_pitch_is_set;

  int32_t playout_volume;
  bool _playout_volume_is_set;

  int32_t audio_playback_delay;
  bool _audio_playback_delay_is_set;

  RteAudioDualMonoMode audio_dual_mono_mode;
  bool _audio_dual_mono_mode_is_set;

  int32_t publish_volume;
  bool _publish_volume_is_set;

  int32_t loop_count;
  bool _loop_count_is_set;

  RteString *json_parameter;
  bool _json_parameter_is_set;

  // live player options
  RteAbrSubscriptionLayer abr_subscription_layer;
  bool _abr_subscription_layer_is_set;

  RteAbrFallbackLayer abr_fallback_layer;
  bool _abr_fallback_layer_is_set;

} RtePlayerConfig;

typedef struct RtePlayerCustomSourceProvider RtePlayerCustomSourceProvider;
struct RtePlayerCustomSourceProvider {
  void (*on_read_data)(RtePlayerCustomSourceProvider *self);
  void (*on_seek)(RtePlayerCustomSourceProvider *self);
};

typedef struct RtePlayerObserver RtePlayerObserver;
struct RtePlayerObserver {
  RteBaseObserver base_observer;

  void (*on_state_changed)(RtePlayerObserver *observer,
                           RtePlayerState old_state, RtePlayerState new_state,
                           RteError *err);
  void (*on_position_changed)(RtePlayerObserver *observer, uint64_t curr_time,
                              uint64_t utc_time);

  void (*on_resolution_changed)(RtePlayerObserver *observer, int width, int height);

  void (*on_event)(RtePlayerObserver *observer, RtePlayerEvent event);
  void (*on_metadata)(RtePlayerObserver *observer, RtePlayerMetadataType type,
                      const uint8_t *data, size_t length);

  void (*on_player_info_updated)(RtePlayerObserver *observer,
                                    const RtePlayerInfo *info);

  void (*on_audio_volume_indication)(RtePlayerObserver *observer,
                                         int32_t volume);
};

AGORA_RTE_API_C void RtePlayerInfoInit(RtePlayerInfo *info, RteError *err);
AGORA_RTE_API_C void RtePlayerInfoCopy(RtePlayerInfo *dest, const RtePlayerInfo *src, RteError *err);
AGORA_RTE_API_C void RtePlayerInfoDeinit(RtePlayerInfo *info, RteError *err);

AGORA_RTE_API_C void RtePlayerStatsInit(RtePlayerStats *stats, RteError *err);
AGORA_RTE_API_C void RtePlayerStatsDeinit(RtePlayerStats *stats, RteError *err);

AGORA_RTE_API_C void RteMediaTrackInfoInit(RteMediaTrackInfo *info,
                                          RteError *err);
AGORA_RTE_API_C void RteMediaTrackInfoDeinit(RteMediaTrackInfo *info,
                                            RteError *err);

AGORA_RTE_API_C void RtePlayerInitialConfigInit(RtePlayerInitialConfig *config,
                                               RteError *err);

AGORA_RTE_API_C void RtePlayerInitialConfigDeinit(RtePlayerInitialConfig *config,
                                                 RteError *err);

AGORA_RTE_API_C void RtePlayerInitialConfigSetEnableCache(
    RtePlayerInitialConfig *config, bool enable_cache, RteError *err);

AGORA_RTE_API_C void RtePlayerInitialConfigGetEnableCache(
    RtePlayerInitialConfig *config, bool *enable_cache, RteError *err);

AGORA_RTE_API_C void RtePlayerInitialConfigSetEnableMultipleAudioTrack(
    RtePlayerInitialConfig *config, bool enable_multiple_audio_track,
    RteError *err);

AGORA_RTE_API_C void RtePlayerInitialConfigGetEnableMultipleAudioTrack(
    RtePlayerInitialConfig *config, bool *enable_multiple_audio_track,
    RteError *err);

AGORA_RTE_API_C void RtePlayerInitialConfigSetIsAgoraSource(
    RtePlayerInitialConfig *config, bool is_agora_source, RteError *err);

AGORA_RTE_API_C void RtePlayerInitialConfigGetIsAgoraSource(
    RtePlayerInitialConfig *config, bool *is_agora_source, RteError *err);

AGORA_RTE_API_C void RtePlayerInitialConfigSetIsLiveSource(
    RtePlayerInitialConfig *config, bool is_agora_source, RteError *err);

AGORA_RTE_API_C void RtePlayerInitialConfigGetIsLiveSource(
    RtePlayerInitialConfig *config, bool *is_agora_source, RteError *err);

AGORA_RTE_API_C void RtePlayerConfigInit(RtePlayerConfig *config, RteError *err);

AGORA_RTE_API_C void RtePlayerConfigDeinit(RtePlayerConfig *config,
                                          RteError *err);

AGORA_RTE_API_C void RtePlayerConfigCopy(RtePlayerConfig *dst,
                                        const RtePlayerConfig *src,
                                        RteError *err);

AGORA_RTE_API_C void RtePlayerConfigSetAutoPlay(RtePlayerConfig *config,
                                               bool auto_play, RteError *err);

AGORA_RTE_API_C void RtePlayerConfigGetAutoPlay(RtePlayerConfig *config,
                                               bool *auto_play, RteError *err);

AGORA_RTE_API_C void RtePlayerConfigSetPlaybackSpeed(RtePlayerConfig *config,
                                                    int32_t speed,
                                                    RteError *err);

AGORA_RTE_API_C void RtePlayerConfigGetPlaybackSpeed(RtePlayerConfig *config,
                                                    int32_t *speed,
                                                    RteError *err);

AGORA_RTE_API_C void RtePlayerConfigSetPlayoutAudioTrackIdx(
    RtePlayerConfig *config, int32_t idx, RteError *err);

AGORA_RTE_API_C void RtePlayerConfigGetPlayoutAudioTrackIdx(
    RtePlayerConfig *config, int32_t *idx, RteError *err);

AGORA_RTE_API_C void RtePlayerConfigSetPublishAudioTrackIdx(
    RtePlayerConfig *config, int32_t idx, RteError *err);

AGORA_RTE_API_C void RtePlayerConfigGetPublishAudioTrackIdx(
    RtePlayerConfig *config, int32_t *idx, RteError *err);

AGORA_RTE_API_C void RtePlayerConfigSetAudioTrackIdx(RtePlayerConfig *config,
                                                    int32_t idx, RteError *err);

AGORA_RTE_API_C void RtePlayerConfigGetAudioTrackIdx(RtePlayerConfig *config,
                                                    int32_t *idx,
                                                    RteError *err);

AGORA_RTE_API_C void RtePlayerConfigSetSubtitleTrackIdx(RtePlayerConfig *config,
                                                       int32_t idx,
                                                       RteError *err);

AGORA_RTE_API_C void RtePlayerConfigGetSubtitleTrackIdx(RtePlayerConfig *config,
                                                       int32_t *idx,
                                                       RteError *err);

AGORA_RTE_API_C void RtePlayerConfigSetExternalSubtitleTrackIdx(
    RtePlayerConfig *config, int32_t idx, RteError *err);

AGORA_RTE_API_C void RtePlayerConfigGetExternalSubtitleTrackIdx(
    RtePlayerConfig *config, int32_t *idx, RteError *err);

AGORA_RTE_API_C void RtePlayerConfigSetAudioPitch(RtePlayerConfig *config,
                                                 int32_t audio_pitch,
                                                 RteError *err);

AGORA_RTE_API_C void RtePlayerConfigGetAudioPitch(RtePlayerConfig *config,
                                                 int32_t *audio_pitch,
                                                 RteError *err);

AGORA_RTE_API_C void RtePlayerConfigSetPlayoutVolume(RtePlayerConfig *config,
                                                    int32_t volume,
                                                    RteError *err);

AGORA_RTE_API_C void RtePlayerConfigGetPlayoutVolume(RtePlayerConfig *config,
                                                    int32_t *volume,
                                                    RteError *err);

AGORA_RTE_API_C void RtePlayerConfigSetAudioPlaybackDelay(
    RtePlayerConfig *config, int32_t delay, RteError *err);

AGORA_RTE_API_C void RtePlayerConfigGetAudioPlaybackDelay(
    RtePlayerConfig *config, int32_t *delay, RteError *err);

AGORA_RTE_API_C void RtePlayerConfigSetAudioDualMonoMode(
    RtePlayerConfig *config, RteAudioDualMonoMode mode, RteError *err);

AGORA_RTE_API_C void RtePlayerConfigGetAudioDualMonoMode(
    RtePlayerConfig *config, RteAudioDualMonoMode *mode, RteError *err);

AGORA_RTE_API_C void RtePlayerConfigSetPublishVolume(RtePlayerConfig *config,
                                                    int32_t volume,
                                                    RteError *err);

AGORA_RTE_API_C void RtePlayerConfigGetPublishVolume(RtePlayerConfig *config,
                                                    int32_t *volume,
                                                    RteError *err);

AGORA_RTE_API_C void RtePlayerConfigSetLoopCount(RtePlayerConfig *config,
                                                int32_t loop_count,
                                                RteError *err);

AGORA_RTE_API_C void RtePlayerConfigGetLoopCount(RtePlayerConfig *config,
                                                int32_t *loop_count,
                                                RteError *err);

AGORA_RTE_API_C void RtePlayerConfigSetJsonParameter(RtePlayerConfig *config,
                                                    RteString *json_parameter,
                                                    RteError *err);

AGORA_RTE_API_C void RtePlayerConfigGetJsonParameter(RtePlayerConfig *config,
                                                    RteString *json_parameter,
                                                    RteError *err);

AGORA_RTE_API_C void RtePlayerConfigSetAbrSubscriptionLayer(RtePlayerConfig *config,
                                                            RteAbrSubscriptionLayer abr_subscription_layer,
                                                            RteError *err);     

AGORA_RTE_API_C void RtePlayerConfigGetAbrSubscriptionLayer(RtePlayerConfig *config,
                                                            RteAbrSubscriptionLayer *abr_subscription_layer,
                                                            RteError *err); 

AGORA_RTE_API_C void RtePlayerConfigSetAbrFallbackLayer(RtePlayerConfig *config,
                                                        RteAbrFallbackLayer abr_fallback_layer,
                                                        RteError *err); 

AGORA_RTE_API_C void RtePlayerConfigGetAbrFallbackLayer(RtePlayerConfig *config,                                            
                                                        RteAbrFallbackLayer *abr_fallback_layer,
                                                        RteError *err);

AGORA_RTE_API_C RtePlayer RtePlayerCreate(Rte *self, RtePlayerInitialConfig *config,
                                         RteError *err);
AGORA_RTE_API_C void RtePlayerDestroy(RtePlayer *self, RteError *err);

AGORA_RTE_API_C bool RtePlayerPreloadWithUrl(RtePlayer *self, const char *url,
                                            RteError *err);

AGORA_RTE_API_C void RtePlayerOpenWithUrl(
    RtePlayer *self, const char *url, uint64_t start_time,
    void (*cb)(RtePlayer *self, void *cb_data, RteError *err), void *cb_data);

AGORA_RTE_API_C void RtePlayerOpenWithCustomSourceProvider(
    RtePlayer *self, RtePlayerCustomSourceProvider *provider,
    uint64_t start_time,
    void (*cb)(RtePlayer *self, void *cb_data, RteError *err), void *cb_data);

AGORA_RTE_API_C void RtePlayerOpenWithStream(RtePlayer *self, RteStream *stream,
                             void (*cb)(RtePlayer *self, void *cb_data,
                                        RteError *err),
                             void *cb_data);


AGORA_RTE_API_C void RtePlayerGetStats(RtePlayer *self, void (*cb)(RtePlayer *player, RtePlayerStats *stats, void *cb_data, RteError *err), void *cb_data);

AGORA_RTE_API_C bool RtePlayerSetCanvas(RtePlayer *self, RteCanvas *canvas, RteError *err);

AGORA_RTE_API_C bool RtePlayerPlay(RtePlayer *self, RteError *err);
AGORA_RTE_API_C bool RtePlayerStop(RtePlayer *self, RteError *err);
AGORA_RTE_API_C bool RtePlayerPause(RtePlayer *self, RteError *err);
AGORA_RTE_API_C bool RtePlayerSeek(RtePlayer *self, uint64_t new_time,
                                  RteError *err);

AGORA_RTE_API_C void RtePlayerSwitchWithUrl(RtePlayer *self, const char* url, bool sync_pts, void (*cb)(RtePlayer *self, void *cb_data, RteError *err), void *cb_data);
                                  
AGORA_RTE_API_C bool RtePlayerMuteAudio(RtePlayer *self, bool mute, RteError *err);
AGORA_RTE_API_C bool RtePlayerMuteVideo(RtePlayer *self, bool mute, RteError *err);

AGORA_RTE_API_C uint64_t RtePlayerGetPosition(RtePlayer *self, RteError *err);
                                  
AGORA_RTE_API_C bool RtePlayerGetInfo(RtePlayer *self, RtePlayerInfo *info, RteError *err);

AGORA_RTE_API_C bool RtePlayerGetConfigs(RtePlayer *self,
                                        RtePlayerConfig *config, RteError *err);
AGORA_RTE_API_C bool RtePlayerSetConfigs(RtePlayer *self, RtePlayerConfig *config, RteError *err);


AGORA_RTE_API_C bool RtePlayerRegisterObserver(
    RtePlayer *self, RtePlayerObserver *observer, RteError *err);
AGORA_RTE_API_C bool RtePlayerUnregisterObserver(RtePlayer *self,
                                                RtePlayerObserver *observer,
                                                RteError *err);

AGORA_RTE_API_C RtePlayerObserver *RtePlayerObserverCreate(RteError *err);
AGORA_RTE_API_C void RtePlayerObserverDestroy(RtePlayerObserver *observer,
                                             RteError *err);
AGORA_RTE_API_C RtePlayer
RtePlayerObserverGetEventSrc(RtePlayerObserver *observer, RteError *err);

AGORA_RTE_API_C RtePlayerCustomSourceProvider
RtePlayerCustomSourceProviderCreate(Rte *self, RteError *err);

AGORA_RTE_API_C void RtePlayerCustomSourceProviderDestroy(
    RtePlayerCustomSourceProvider *self, RteError *err);

#ifdef __cplusplus
}
#endif  // __cplusplus
