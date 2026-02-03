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
class IMediaPlayerCustomDataProvider;

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
   * @brief Gets the ID of the media player.
   *
   * @return
   * - Success. The ID of the media player.
   * - < 0: Failure.
   */
  virtual int getMediaPlayerId() const = 0;

  /**
   * @brief Opens the media resource.
   *
   * @details
   * Call timing: This method can be called either before or after joining the channel.
   * Related callbacks: After calling this method, the SDK triggers the `onPlayerSourceStateChanged`
   * callback. After receiving the report of the playback status as `PLAYER_STATE_OPEN_COMPLETED`, you
   * can call the `play` method to play the media file.
   *
   * @note This method is called asynchronously.
   *
   * @param url The path of the media file. Both local path and online path are supported.
   * @param startPos The starting position (ms) for playback. Default value is 0.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int open(const char* url, int64_t startPos) = 0;

  /**
   * @brief Opens a media file and configures the playback scenarios.
   *
   * @details
   * This method supports opening media files of different sources, including a custom media source,
   * and allows you to configure the playback scenarios.
   * Call timing: You can call this method either before or after joining a channel.
   *
   * @note This method is called asynchronously. If you need to play a media file, make sure you
   * receive the `onPlayerSourceStateChanged` callback reporting `PLAYER_STATE_OPEN_COMPLETED` before
   * calling the `play` method to play the file.
   *
   * @param source Media resources. See `MediaSource`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int openWithMediaSource(const media::base::MediaSource &source) = 0;

  /**
   * @brief Plays the media file.
   *
   * @details
   * Call timing: - Call this method after calling `open` or `openWithMediaSource` opening a media
   * file and receiving a `onPlayerSourceStateChanged` callback reporting the status as
   * PLAYER_STATE_OPEN_COMPLETED.
   * - Call the method after calling `seek`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int play() = 0;

  /**
   * @brief Pauses the playback.
   *
   * @details
   * Call timing: You can call this method either before or after joining a channel.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int pause() = 0;

  /**
   * @brief Stops playing the media track.
   *
   * @details
   * After calling this method to stop playback, if you want to play again, you need to call `open` or
   * `openWithMediaSource` to open the media resource.
   * Call timing: Call this method after play.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int stop() = 0;

  /**
   * @brief Resumes playing the media file.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int resume() = 0;

  /**
   * @brief Seeks to a new playback position.
   *
   * @details
   * - If you call `seek` after the playback has completed (upon receiving callback
   * `onPlayerSourceStateChanged` reporting playback status as PLAYER_STATE_PLAYBACK_COMPLETED or
   * PLAYER_STATE_PLAYBACK_ALL_LOOPS_COMPLETED ), the SDK will play the media file from the specified
   * position. At this point, you will receive callback `onPlayerSourceStateChanged` reporting
   * playback status as PLAYER_STATE_PLAYING.
   * - If you call `seek` while the playback is paused, upon successful call of this method, the SDK
   * will seek to the specified position. To resume playback, call `resume` or `play` .
   * Call timing: You can call this method either before or after joining a channel.
   * Related callbacks: After successfully calling this method, you will receive the `onPlayerEvent`
   * callback, reporting the result of the seek operation to the new playback position.
   *
   * @param newPos The new playback position (ms).
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int seek(int64_t newPos) = 0;
  
  /**
   * @brief Sets the pitch of the current media resource.
   *
   * @note Call this method after calling `open`.
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
  virtual int setAudioPitch(int pitch) = 0;

  /**
   * @brief Gets the duration of the media resource.
   *
   * @param duration An output parameter. The total duration (ms) of the media file.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int getDuration(int64_t& duration) = 0;

  /**
   * @brief Gets current local playback progress.
   *
   * @param pos The playback position (ms) of the audio effect file.
   *
   * @return
   * - Returns the current playback progress (ms) if the call succeeds.
   * - < 0: Failure. See `MEDIA_PLAYER_REASON`.
   */
  virtual int getPlayPosition(int64_t& pos) = 0;

  /**
   * @brief Gets the number of the media streams in the media resource.
   *
   * @note Call this method after you call `open` and receive the `onPlayerSourceStateChanged`
   * callback reporting the state `PLAYER_STATE_OPEN_COMPLETED`.
   *
   * @param count An output parameter. The number of the media streams in the media resource.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure. See `MEDIA_PLAYER_REASON`.
   */
  virtual int getStreamCount(int64_t& count) = 0;

  /**
   * @brief Gets the detailed information of the media stream.
   *
   * @details
   * Call timing: Call this method after calling `getStreamCount`.
   *
   * @param index The index of the media stream. This parameter needs to be less than the `count`
   * parameter of `getStreamCount`.
   * @param info An output parameter. The detailed information of the media stream. See
   * `PlayerStreamInfo`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int getStreamInfo(int64_t index, media::base::PlayerStreamInfo* info) = 0;

  /**
   * @brief Sets the loop playback.
   *
   * @details
   * If you want to loop, call this method and set the number of the loops.
   * When the loop finishes, the SDK triggers `onPlayerSourceStateChanged` and reports the playback
   * state as PLAYER_STATE_PLAYBACK_ALL_LOOPS_COMPLETED.
   *
   * @param loopCount The number of times the audio effect loops:
   * - ≥0: Number of times for playing. For example, setting it to 0 means no loop playback, playing
   * only once; setting it to 1 means loop playback once, playing a total of twice.
   * - -1: Play the audio file in an infinite loop.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setLoopCount(int loopCount) = 0;

  /**
   * @brief Sets the channel mode of the current audio file.
   *
   * @details
   * Call this method after calling `open`.
   *
   * @param speed The playback speed. Agora recommends that you set this to a value between 30 and
   * 400, defined as follows:
   * - 30: 0.3 times the original speed.
   * - 100: The original speed.
   * - 400: 4 times the original speed.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setPlaybackSpeed(int speed) = 0;

  /**
   * @brief Selects the audio track used during playback.
   *
   * @details
   * After getting the track index of the audio file, you can call this method to specify any track to
   * play. For example, if different tracks of a multi-track file store songs in different languages,
   * you can call this method to set the playback language.
   *
   * @note You need to call this method after calling `getStreamInfo` to get the audio stream index
   * value.
   *
   * @param index The index of the audio track.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int selectAudioTrack(int index) = 0;

  /**
   * @brief Selects the audio tracks that you want to play on your local device and publish to the
   * channel respectively.
   *
   * @details
   * You can call this method to determine the audio track to be played on your local device and
   * published to the channel.
   * Before calling this method, you need to open the media file with the `openWithMediaSource` method
   * and set `enableMultiAudioTrack` in `MediaSource` as `true`.
   * Applicable scenarios: For example, in KTV scenarios, the host can choose to play the original
   * sound locally and publish the accompaniment track to the channel.
   *
   * @param playoutTrackIndex The index of audio tracks for local playback. You can obtain the index
   * through `getStreamInfo`.
   * @param publishTrackIndex The index of audio tracks to be published in the channel. You can obtain
   * the index through `getStreamInfo`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int selectMultiAudioTrack(int playoutTrackIndex, int publishTrackIndex) = 0;

  /**
   * @brief Sets media player options.
   *
   * @details
   * The media player supports setting options through `key` and `value`.
   * The difference between this method and `setPlayerOption(const char* key, const char* value)` is
   * that the `value` parameter of
   * this method is of type Int, while the `value` of `setPlayerOption(const char* key, const char*
   * value)` is of type String. These
   * two methods cannot be used together.
   * Applicable scenarios: Scenarios that require technical previews or special customization
   * features. In general, you do not need to call this method; you can simply use the default options
   * provided by the media player.
   * Call timing: Call this method before the `open` or `openWithMediaSource` method.
   *
   * @param key The key of the option.
   * @param value The value of the key.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setPlayerOption(const char* key, int value) = 0;

  /**
   * @brief Sets media player options.
   *
   * @details
   * The media player supports setting options through `key` and `value`.
   * The difference between this method and `setPlayerOption(const char* key, int value)` is that the
   * `value` parameter of
   * this method is of type String, while the `value` of `setPlayerOption(const char* key, int value)`
   * is of type String.
   * These two methods cannot be used together.
   * Applicable scenarios: Scenarios that require technical previews or special customization
   * features. In general, you do not need to call this method; you can simply use the default options
   * provided by the media player.
   * Call timing: Call this method before the `open` or `openWithMediaSource` method.
   *
   * @param key The key of the option.
   * @param value The value of the key.
   *
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

  /**
   * @brief Gets current playback state.
   *
   * @return
   * The current playback state. See `MEDIA_PLAYER_STATE`.
   */
  virtual media::base::MEDIA_PLAYER_STATE getState() = 0;

  /**
   * @brief Sets whether to mute the media file.
   *
   * @details
   * Call timing: You can call this method either before or after joining a channel.
   *
   * @param muted Whether to mute the media file:
   * - `true`: Mute the media file.
   * - `false`: (Default) Unmute the media file.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int mute(bool muted) = 0;

  /**
   * @brief Reports whether the media resource is muted.
   *
   * @param mute An output parameter. Whether the media file is muted:
   * - `true`: The media file is muted.
   * - `false`: The media file is not muted.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int getMute(bool& muted) = 0;

  /**
   * @brief Adjusts the local playback volume.
   *
   * @details
   * Call timing: This method can be called either before or after joining the channel.
   *
   * @param volume The local playback volume, which ranges from 0 to 100:
   * - 0: Mute.
   * - 100: (Default) The original volume.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int adjustPlayoutVolume(int volume) = 0;

  /**
   * @brief Gets the local playback volume.
   *
   * @param volume An output parameter. The local playback volume, which ranges from 0 to 100:
   * - 0: Mute.
   * - 100: (Default) The original volume.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int getPlayoutVolume(int& volume) = 0;

  /**
   * @brief Adjusts the volume of the media file for publishing.
   *
   * @details
   * After connected to the Agora server, you can call this method to adjust the volume of the media
   * file heard by the remote user.
   * Call timing: This method can be called either before or after joining the channel.
   *
   * @param volume The volume, which ranges from 0 to 400:
   * - 0: Mute.
   * - 100: (Default) The original volume.
   * - 400: Four times the original volume (amplifying the audio signals by four times).
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int adjustPublishSignalVolume(int volume) = 0;

  /**
   * @brief Gets the volume of the media file for publishing.
   *
   * @param volume An output parameter. The remote playback volume.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int getPublishSignalVolume(int& volume) = 0;

  /**
   * @brief Sets the view.
   *
   * @details
   * Call timing: You can call this method either before or after joining a channel.
   *
   * @param view The render view. On Windows, this parameter sets the window handle (HWND).
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setView(media::base::view_t view) = 0;

  /**
   * @brief Sets the render mode of the media player.
   *
   * @param renderMode Sets the render mode of the view. See `RENDER_MODE_TYPE`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setRenderMode(media::base::RENDER_MODE_TYPE renderMode) = 0;

  /**
   * @brief Registers a media player observer.
   *
   * @details
   * Call timing: This method can be called either before or after joining the channel.
   *
   * @param observer The player observer, listening for events during the playback. See
   * `IMediaPlayerSourceObserver`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int registerPlayerSourceObserver(IMediaPlayerSourceObserver* observer) = 0;

  /**
   * @brief Releases a media player observer.
   *
   * @param observer The player observer, listening for events during the playback. See
   * `IMediaPlayerSourceObserver`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int unregisterPlayerSourceObserver(IMediaPlayerSourceObserver* observer) = 0;

  /**
   * @brief Registers a PCM audio frame observer object.
   *
   * @details
   * You need to implement the `IAudioPcmFrameSink` class in this method and register callbacks
   * according to your scenarios. After you successfully register the video frame observer, the SDK
   * triggers the registered callbacks each time a video frame is received.
   *
   * @param observer The audio frame observer, reporting the reception of each audio frame. See
   * `IAudioPcmFrameSink`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int registerAudioFrameObserver(media::IAudioPcmFrameSink* observer) = 0;

  /**
   * @brief Registers an audio frame observer object.
   *
   * @param observer The audio frame observer, reporting the reception of each audio frame. See
   * `IAudioPcmFrameSink`.
   * @param mode The use mode of the audio frame. See `RAW_AUDIO_FRAME_OP_MODE_TYPE`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int registerAudioFrameObserver(media::IAudioPcmFrameSink* observer,
                                         RAW_AUDIO_FRAME_OP_MODE_TYPE mode) = 0;

  /**
   * @brief Unregisters an audio frame observer.
   *
   * @param observer The audio observer. See `IAudioPcmFrameSink`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int unregisterAudioFrameObserver(media::IAudioPcmFrameSink* observer) = 0;

  /**
   * @brief Registers a video frame observer object.
   *
   * @details
   * You need to implement the `IVideoFrameObserver` class in this method and register callbacks
   * according to your scenarios. After you successfully register the video frame observer, the SDK
   * triggers the registered callbacks each time a video frame is received.
   *
   * @param observer The video observer, reporting the reception of each video frame. See
   * `IVideoFrameObserver`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int registerVideoFrameObserver(media::base::IVideoFrameObserver* observer) = 0;

  /**
   * @brief Unregisters the video frame observer.
   *
   * @param observer The video observer, reporting the reception of each video frame. See
   * `IVideoFrameObserver`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int unregisterVideoFrameObserver(agora::media::base::IVideoFrameObserver* observer) = 0;

   /**
   * Registers the audio frame spectrum observer.
   *
   * @param observer The pointer to the {@link media::base::IAudioSpectrumObserver  IAudioSpectrumObserver} object.
   * @param intervalInMS Sets the time interval(ms) between two consecutive audio spectrum callback.
   * The default value is 100. This param should be larger than 10.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int registerMediaPlayerAudioSpectrumObserver(media::IAudioSpectrumObserver* observer, int intervalInMS) = 0;

  /**
   * Releases the audio frame spectrum observer.
   * @param observer The pointer to the {@link media::base::IAudioSpectrumObserver IAudioSpectrumObserver} object.
   * @return
   * - 0: Success.
   * - < 0: Failure. 
   */
   virtual int unregisterMediaPlayerAudioSpectrumObserver(media::IAudioSpectrumObserver* observer) = 0;

  /**
   * @brief Sets the channel mode of the current audio file.
   *
   * @details
   * In a stereo music file, the left and right channels can store different audio data. According to
   * your needs, you can set the channel mode to original mode, left channel mode, right channel mode,
   * or mixed channel mode. For example, in the KTV scenario, the left channel of the music file
   * stores the musical accompaniment, and the right channel stores the singing voice. If you only
   * need to listen to the accompaniment, call this method to set the channel mode of the music file
   * to left channel mode; if you need to listen to the accompaniment and the singing voice at the
   * same time, call this method to set the channel mode to mixed channel mode.
   *
   * @note
   * - Call this method after calling `open`.
   * - This method only applies to stereo audio files.
   *
   * @param mode The channel mode. See `AUDIO_DUAL_MONO_MODE`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setAudioDualMonoMode(agora::media::base::AUDIO_DUAL_MONO_MODE mode) = 0;

  /**
    * get sdk version and build number of player SDK.
    * @return String of the SDK version.
    * 
    * @deprecated This method is deprecated.
   */
  virtual const char* getPlayerSdkVersion() = 0;

  /**
   * @brief Gets the path of the media resource being played.
   *
   * @return
   * The path of the media resource being played.
   */
  virtual const char* getPlaySrc() = 0;


    /**
   * Open the Agora CDN media source.
   * 
   * @deprecated 4.6.0
   * 
   * @param src The src of the media file that you want to play.
   * @param startPos The  playback position (ms).
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int openWithAgoraCDNSrc(const char* src, int64_t startPos) = 0;

  /**
   * Gets the number of  Agora CDN lines.
   * 
   * @deprecated 4.6.0
   * 
   * @return
   * - > 0: number of CDN.
   * - <= 0: Failure.
   */
  virtual int getAgoraCDNLineCount() = 0;

  /**
   * Switch Agora CDN lines.
   * 
   * @deprecated 4.6.0
   * 
   * @param index Specific CDN line index.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int switchAgoraCDNLineByIndex(int index) = 0;

  /**
   * Gets the line of the current CDN.
   * 
   * @deprecated 4.6.0
   * 
   * @return
   * - >= 0: Specific line.
   * - < 0: Failure.
   */
  virtual int getCurrentAgoraCDNIndex() = 0;

  /**
   * Enable automatic CDN line switching.
   * 
   * @deprecated 4.6.0
   * 
   * @param enable Whether enable.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int enableAutoSwitchAgoraCDN(bool enable) = 0;

  /**
   * Update the CDN source token and timestamp.
   * 
   * @deprecated 4.6.0
   * 
   * @param token token.
   * @param ts ts.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int renewAgoraCDNSrcToken(const char* token, int64_t ts) = 0;

  /**
   * Switch the CDN source when open a media through "openWithAgoraCDNSrc" API
   * 
   * @deprecated 4.6.0
   * 
   * @param src Specific src.
   * @param syncPts Live streaming must be set to false.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int switchAgoraCDNSrc(const char* src, bool syncPts = false) = 0;

  /**
   * @brief Switches the media resource being played.
   *
   * @details
   * You can call this method to switch the media resource to be played according to the current
   * network status. For example:
   * - When the network is poor, the media resource to be played is switched to a media resource
   * address with a lower bitrate.
   * - When the network is good, the media resource to be played is switched to a media resource
   * address with a higher bitrate.
   * After calling this method, if you receive the `onPlayerEvent` callback report the
   * `PLAYER_EVENT_SWITCH_COMPLETE` event, the switching is successful. If the switching fails, the
   * SDK will automatically retry 3 times. If it still fails, you will receive the `onPlayerEvent`
   * callback reporting the `PLAYER_EVENT_SWITCH_ERROR` event indicating an error occurred during
   * media resource switching.
   *
   * @note
   * - Ensure that you call this method after `open`.
   * - To ensure normal playback, pay attention to the following when calling this method:
   *   - Do not call this method when playback is paused.
   *   - Do not call the `seek` method during switching.
   *   - Before switching the media resource, make sure that the playback position does not exceed the
   * total duration of the media resource to be switched.
   *
   * @param src The URL of the media resource.
   * @param syncPts Whether to synchronize the playback position (ms) before and after the switch:
   * - `true`: Synchronize the playback position before and after the switch.
   * - `false`: (Default) Do not synchronize the playback position before and after the switch.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int switchSrc(const char* src, bool syncPts = true) = 0;

  /**
   * @brief Preloads a media resource.
   *
   * @details
   * You can call this method to preload a media resource into the playlist. If you need to preload
   * multiple media resources, you can call this method multiple times.
   * After calling this method, if you receive the `PLAYER_PRELOAD_EVENT_COMPLETE` event in the
   * `onPreloadEvent` callback, the preload is successful; If you receive the
   * `PLAYER_PRELOAD_EVENT_ERROR` event in the `onPreloadEvent` callback, the preload fails.
   * If the preload is successful and you want to play the media resource, call `playPreloadedSrc`; if
   * you want to clear the playlist, call `stop`.
   *
   * @note
   * - Before calling this method, ensure that you have called `open` or `openWithMediaSource` to open
   * the media resource successfully.
   * - Agora does not support preloading duplicate media resources to the playlist. However, you can
   * preload the media resources that are being played to the playlist again.
   *
   * @param src The URL of the media resource.
   * @param startPos The starting position (ms) for playing after the media resource is preloaded to
   * the playlist. When preloading a live stream, set this parameter to 0.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int preloadSrc(const char* src, int64_t startPos) = 0;

  /**
   * @brief Plays preloaded media resources.
   *
   * @details
   * After calling the `preloadSrc` method to preload the media resource into the playlist, you can
   * call this method to play the preloaded media resource. After calling this method, if you receive
   * the `onPlayerSourceStateChanged` callback which reports the `PLAYER_STATE_PLAYING` state, the
   * playback is successful.
   * If you want to change the preloaded media resource to be played, you can call this method again
   * and specify the URL of the new media resource that you want to preload. If you want to replay the
   * media resource, you need to call `preloadSrc` to preload the media resource to the playlist again
   * before playing. If you want to clear the playlist, call the `stop` method.
   *
   * @note If you call this method when playback is paused, this method does not take effect until
   * playback is resumed.
   *
   * @param src The URL of the media resource in the playlist must be consistent with the `src` set by
   * the `preloadSrc` method; otherwise, the media resource cannot be played.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int playPreloadedSrc(const char* src) = 0;

  /**
   * @brief Unloads media resources that are preloaded.
   *
   * @note This method cannot release the media resource being played.
   *
   * @param src The URL of the media resource.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int unloadSrc(const char* src) = 0;

  /**
   * @brief Enables or disables the spatial audio effect for the media player.
   *
   * @details
   * After successfully setting the spatial audio effect parameters of the media player, the SDK
   * enables the spatial audio effect for the media player, and the local user can hear the media
   * resources with a sense of space.
   * If you need to disable the spatial audio effect for the media player, set the `params` parameter
   * to null.
   *
   * @param params The spatial audio effect parameters of the media player. See `SpatialAudioParams`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setSpatialAudioParams(const SpatialAudioParams& params) = 0;

  /**
   * Set sound position params for the music file. It can be called after the media player
   * was created.
   *
   *@param pan The sound position of the music file. The value ranges from -1.0 to 1.0:
   *- 0.0: the music sound comes from the front.
   *- -1.0: the music sound comes from the left.
   *- 1.0: the music sound comes from the right.
   *@param gain Gain of the music. The value ranges from 0.0 to 100.0. The default value is 100.0 (the original gain of the music). The smaller the value, the less the gain.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setSoundPositionParams(float pan, float gain) = 0;

  /**
   * @brief Gets the audio buffer delay when playing the media file.
   * @param[out] delayMs delay in millisecond.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int getAudioBufferDelay(int32_t& delayMs) = 0;

};

/**
 * This class is used to set and manage the player cache, implemented in the
 * form of a singleton, independent of the player.
 */
class IMediaPlayerCacheManager {
public:
  /**
   * @brief Deletes all cached media files in the media player.
   *
   * @note The cached media file currently being played will not be deleted.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure. See `MEDIA_PLAYER_REASON`.
   */
  virtual int removeAllCaches() = 0;
  /**
   * @brief Deletes a cached media file that is the least recently used.
   *
   * @details
   * You can call this method to delete a cached media file when the storage space for the cached
   * files is about to reach its limit. After you call this method, the SDK deletes the cached media
   * file that is least used.
   *
   * @note The cached media file currently being played will not be deleted.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure. See `MEDIA_PLAYER_REASON`.
   */
  virtual int removeOldCache() = 0;
  /**
   * @brief Deletes a cached media file.
   *
   * @note The cached media file currently being played will not be deleted.
   *
   * @param uri The URI (Uniform Resource Identifier) of the media file to be deleted.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure. See `MEDIA_PLAYER_REASON`.
   */
  virtual int removeCacheByUri(const char *uri) = 0;
  /**
   * @brief Sets the storage path for the media files that you want to cache.
   *
   * @note Make sure `IRtcEngine` is initialized before you call this method.
   *
   * @param path The absolute path of the media files to be cached. Ensure that the directory for the
   * media files exists and is writable.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure. See `MEDIA_PLAYER_REASON`.
   */
  virtual int setCacheDir(const char *path) = 0;
  /**
   * @brief Sets the maximum number of media files that can be cached.
   *
   * @param count The maximum number of media files that can be cached. The default value is 1,000.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure. See `MEDIA_PLAYER_REASON`.
   */
  virtual int setMaxCacheFileCount(int count) = 0;
  /**
   * @brief Sets the maximum size of the aggregate storage space for cached media files.
   *
   * @param cacheSize The maximum size (bytes) of the aggregate storage space for cached media files.
   * The default value is 1 GB.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure. See `MEDIA_PLAYER_REASON`.
   */
  virtual int setMaxCacheFileSize(int64_t cacheSize) = 0;
  /**
   * @brief Sets whether to delete cached media files automatically.
   *
   * @details
   * If you enable this function to remove cached media files automatically, when the cached media
   * files exceed either the number or size limit you set, the SDK automatically deletes the least
   * recently used cache file.
   *
   * @param enable Whether to enable the SDK to delete cached media files automatically:
   * - `true`: Delete cached media files automatically.
   * - `false`: (Default) Do not delete cached media files automatically.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure. See `MEDIA_PLAYER_REASON`.
   */
  virtual int enableAutoRemoveCache(bool enable) = 0;
  /**
   * @brief Gets the storage path of the cached media files.
   *
   * @details
   * If you have not called the `setCacheDir` method to set the storage path for the media files to be
   * cached before calling this method, you get the default storage path used by the SDK.
   *
   * @param path An output parameter; the storage path for the media file to be cached.
   * @param length An input parameter; the maximum length of the cache file storage path string. Fill
   * in according to the cache file storage `path` string you obtained from path.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure. See `MEDIA_PLAYER_REASON`.
   */
  virtual int getCacheDir(char* path, int length) = 0;
  /**
   * @brief Gets the maximum number of media files that can be cached.
   *
   * @details
   * By default, the maximum number of media files that can be cached is 1,000.
   *
   * @return
   * - > 0: The call succeeds and returns the maximum number of media files that can be cached.
   * - < 0: Failure. See `MEDIA_PLAYER_REASON`.
   */
  virtual int getMaxCacheFileCount() = 0;
  /**
   * @brief Gets the maximum size of the aggregate storage space for cached media files.
   *
   * @details
   * By default, the maximum size of the aggregate storage space for cached media files is 1 GB. You
   * can call the `setMaxCacheFileSize` method to set the limit according to your scenarios.
   *
   * @return
   * - > 0: The call succeeds and returns the maximum size (in bytes) of the aggregate storage space
   * for cached media files.
   * - < 0: Failure. See `MEDIA_PLAYER_REASON`.
   */
  virtual int64_t getMaxCacheFileSize() = 0;
  /**
   * @brief Gets the number of media files that are cached.
   *
   * @return
   * - ≥ 0: The call succeeds and returns the number of media files that are cached.
   * - < 0: Failure. See `MEDIA_PLAYER_REASON`.
   */
  virtual int getCacheFileCount() = 0;

  virtual ~IMediaPlayerCacheManager(){};
};

} //namespace rtc
} // namespace agora

/**
 * @brief Gets one `IMediaPlayerCacheManager` instance.
 *
 * @details
 * Before calling any APIs in the `IMediaPlayerCacheManager` class, you need to call this method to
 * get a cache manager instance of a media player.
 * Call timing: Make sure the `IRtcEngine` is initialized before you call this method.
 *
 * @note The cache manager is a singleton pattern. Therefore, multiple calls to this method returns
 * the same instance.
 *
 * @return
 * The `IMediaPlayerCacheManager` instance.
 */
AGORA_API agora::rtc::IMediaPlayerCacheManager* AGORA_CALL getMediaPlayerCacheManager();
