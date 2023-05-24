//
//  AgoraRtcMediaPlayerProtocol.h
//  AgoraRtcMediaPlayerProtocol
//
//  Copyright (c) 2020 Agora. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "AgoraObjects.h"
#import "AgoraRtcAudioSpectrumDelegate.h"
#if TARGET_OS_IPHONE
#import <UIKit/UIKit.h>
typedef UIView View;
#elif TARGET_OS_MAC
#import <AppKit/AppKit.h>
typedef NSView View;
#endif

@protocol AgoraRtcMediaPlayerAudioFrameDelegate;
@protocol AgoraRtcMediaPlayerVideoFrameDelegate;

NS_ASSUME_NONNULL_BEGIN
@protocol AgoraRtcMediaPlayerProtocol <NSObject>

/**
 * Get unique media player id of the media player entity.
 * @return
 * - >= 0: The mediaPlayerId of this media player entity.
 * - < 0: Failure.
 */
- (int)getMediaPlayerId NS_SWIFT_NAME(getMediaPlayerId());
/**
 * Opens a media file with a specified URL.
 * @param url The URL of the media file that you want to play.
 * @return
 * - 0: Success.
 * - < 0: Failure.
 */
- (int)open:(NSString *)url startPos:(NSInteger)startPos NS_SWIFT_NAME(open(_:startPos:));


/**
 * Opens a media file with MediaSource
 * @param source see `AgoraMediaSource`
 * @return
 * - 0: Success.
 * - < 0: Failure.
 */
- (int)openWithMediaSource:(AgoraMediaSource *)source NS_SWIFT_NAME(open(with:));

/**
 * Open the Agora CDN media source.
 * @param src The src of the media file that you want to play.
 * @param startPos The  playback position (ms).
 * @return
 * - 0: Success.
 * - < 0: Failure.
 */
- (int)openWithAgoraCDNSrc:(NSString *)src startPos:(NSInteger)startPos NS_SWIFT_NAME(open(withAgoraCDNSrc:startPos:));

/**
 * Gets the number of  Agora CDN lines.
 * @return
 * - > 0: number of CDN.
 * - <= 0: Failure.
 */
- (int)getAgoraCDNLineCount NS_SWIFT_NAME(getAgoraCDNLineCount());

/**
 * Switch Agora CDN lines.
 * @param index Specific line.
 * @return
 * - 0: Success.
 * - < 0: Failure.
 */
- (int)switchAgoraCDNLineByIndex:(int)index NS_SWIFT_NAME(switchAgoraCDNLine(by:));

/**
 * Gets the line of the current CDN.
 * @return
 * - >= 0: Specific line.
 * - < 0: Failure.
 */
- (int)getCurrentAgoraCDNIndex NS_SWIFT_NAME(getCurrentAgoraCDNIndex());

/**
 * Enable automatic CDN line switching.
 * @param enable Whether enable.
 * @return
 * - 0: Success.
 * - < 0: Failure.
 */
- (int)enableAutoSwitchAgoraCDN:(BOOL)enable NS_SWIFT_NAME(enableAutoSwitchAgoraCDN(_:));

/**
 * Update the CDN source token and timestamp.
 * @param token token.
 * @param ts ts.
 * @return
 * - 0: Success.
 * - < 0: Failure.
 */
- (int)renewAgoraCDNSrcToken:(NSString *)token ts:(NSInteger)ts NS_SWIFT_NAME(renewAgoraCDNSrcToken(_:ts:));

/**
 * Switch the CDN source.
 * @param src Specific line.
 * @param syncPts Live streaming must be set to false.
 * @return
 * - 0: Success.
 * - < 0: Failure.
 */
- (int)switchAgoraCDNSrc:(NSString *)src syncPts:(BOOL)syncPts NS_SWIFT_NAME(switchAgoraCDNSrc(_:syncPts:));

/**
 * Plays the media file.
 * @return
 * - 0: Success.
 * - < 0: Failure.
 */
- (int)play NS_SWIFT_NAME(play());

/**
 * Pauses playing the media file.
 */
- (int)pause NS_SWIFT_NAME(pause());

/**
 * Stops playing the current media file.
 */
- (int)stop NS_SWIFT_NAME(stop());

/**
 * Resumes playing the media file.
 */
- (int)resume NS_SWIFT_NAME(resume());

/**
 * Sets the current playback position of the media file.
 * @param position The new playback position (ms).
 * @return
 * - 0: Success.
 * - < 0: Failure.
 */
- (int)seekToPosition:(NSInteger)position NS_SWIFT_NAME(seek(toPosition:));

/**
  * Sets the pitch of the current media file.
  * @param pitch Sets the pitch of the local music file by chromatic scale. The default value is 0,
  * which means keeping the original pitch. The value ranges from -12 to 12, and the pitch value
  * between consecutive values is a chromatic value. The greater the absolute value of this
  * parameter, the higher or lower the pitch of the local music file.
  *
  * @return
  * - 0: Success.
  * - < 0: Failure.
  */
- (int)setAudioPitch:(NSInteger)pitch NS_SWIFT_NAME(setAudioPitch(_:));

/**
 * Gets the duration of the media file.
 */
- (NSInteger)getDuration NS_SWIFT_NAME(getDuration());
/**
 * Gets the current playback position of the media file.(ms).
 */
- (NSInteger)getPosition NS_SWIFT_NAME(getPosition());
/**
 * Gets the number of the media streams in the media resource.
 */
- (NSInteger)getStreamCount NS_SWIFT_NAME(getStreamCount());

/** Gets the detailed information of the media stream.

 @param index The index of the media stream.

 @return * If the call succeeds, returns the detailed information of the media
 stream. See AgoraMediaStreamInfo.
 * If the call fails and returns nil.
 */
- (AgoraRtcMediaStreamInfo *_Nullable)getStreamByIndex:(int)index NS_SWIFT_NAME(getStreamBy(_:));

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
- (int)setLoopCount:(int)loopCount NS_SWIFT_NAME(setLoopCount(_:));

/**
 * Change playback speed
 * @param speed the enum of playback speed
 * @return
 * - 0: Success.
 * - < 0: Failure.
 */
- (int)setPlaybackSpeed:(int)speed NS_SWIFT_NAME(setPlaybackSpeed(_:));

/**
 * Slect playback audio track of the media file
 * @param index the index of the audio track in meia file
 * @return
 * - 0: Success.
 * - < 0: Failure.
 */
- (int)selectAudioTrack:(int)index NS_SWIFT_NAME(selectAudioTrack(_:));

/**
 * take screenshot while playing  video
 * @param filename the filename of screenshot file
 * @return
 * - 0: Success.
 * - < 0: Failure.
 */
- (int)takeScreenshot:(NSString *)filename NS_SWIFT_NAME(takeScreenshot(_:));

/**
 * select internal subtitles in video
 * @param index the index of the internal subtitles
 * @return
 * - 0: Success.
 * - < 0: Failure.
 */
- (int)selectInternalSubtitle:(int)index NS_SWIFT_NAME(selectInternalSubtitle(_:));

/**
 * set an external subtitle for video
 * @param url The URL of the subtitle file that you want to load.
 * @return
 * - 0: Success.
 * - < 0: Failure.
 */
- (int)setExternalSubtitle:(NSString *)url NS_SWIFT_NAME(setExternalSubtitle(_:));

/** Gets current playback state.

 @return * The call succeeds and returns current playback state. See
 AgoraMediaPlayerState.
 * The call fails and returns nil.
 */
- (AgoraMediaPlayerState)getPlayerState NS_SWIFT_NAME(getPlayerState());

/**
 * @brief Turn mute on or off
 *
 * @param isMute Whether the media source is mute.
 *   YES: Yes.
 *   NO: No.
 * @return mute Whether to mute on
 */
- (int)mute:(bool)isMute NS_SWIFT_NAME(mute(_:));

/**
 * @brief Get mute state
 *
 * @return mute Whether is mute on
 */
- (BOOL)getMute NS_SWIFT_NAME(getMute());

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
- (int)adjustPlayoutVolume:(int)volume NS_SWIFT_NAME(adjustPlayoutVolume(_:));

/**
 * @brief Get the current playback volume
 *
 * @return  volume
 */
- (int)getPlayoutVolume NS_SWIFT_NAME(getPlayoutVolume());

/**
 * @brief adjust publish signal volume
 */
- (int)adjustPublishSignalVolume:(int)volume NS_SWIFT_NAME(adjustPublishSignalVolume(_:));

/**
 * @brief get publish signal volume
 */
- (int)getPublishSignalVolume NS_SWIFT_NAME(getPublishSignalVolume());

/**
 * @brief Modify player option before opening file or url,
 * @param [in] key
 *        the option key name 
 * @param [in] value
 *        the option value
 * @return 
 * - 0: Success.
 * - < 0: Failure. See AgoraMediaPlayerError.
 */
- (int)setPlayerOption:(NSString *)key value:(NSInteger)value NS_SWIFT_NAME(setPlayerOption(_:value:));

/**
 * @brief Modify player option before opening file or url,
 * @param [in] key
 *        the option key name
 * @param [in] value
 *        the option value
 * @return 
 * - 0: Success.
 * - < 0: Failure. See AgoraMediaPlayerError.
 */
- (int)setPlayerOptionString:(NSString *)key value:(NSString *)value NS_SWIFT_NAME(setPlayerOptionString(_:value:));

/**
 * @brief Set video rendering view
 */
- (int)setView:(View *_Nullable)view NS_SWIFT_NAME(setView(_:));

/**
 * @brief Set video display mode
 *
 * @param mode Video display mode
 * @return int < 0 on behalf of an error, the value corresponds to one of MEDIA_PLAYER_ERROR
 */
- (int)setRenderMode:(AgoraMediaPlayerRenderMode)mode NS_SWIFT_NAME(setRenderMode(_:));

/**
 * Get the current play src.
 * @return
 * - current play src of raw bytes.
 */
- (NSString *)getPlaySrc NS_SWIFT_NAME(getPlaySrc());

/**
 * Switch the media source when open a media through "open" API
 * @param src Specific src.
 * @param syncPts Live streaming must be set to false.
 * @return
 * - 0: Success.
 * - < 0: Failure.
 */
- (int)switchSrc:(NSString *)src syncPts:(BOOL)syncPts NS_SWIFT_NAME(switchSrc(_:syncPts:));

/**
 * Preload a media source
 * @param src Specific src.
 * @param startPos The starting position (ms) for playback. Default value is 0.
 * @return
 * - 0: Success.
 * - < 0: Failure.
 */
- (int)preloadSrc:(NSString *)src startPos:(int)startPos NS_SWIFT_NAME(preloadSrc(_:startPos:));

/**
 * unload a media source
 * @param src Specific src.
 * @return
 * - 0: Success.
 * - < 0: Failure.
 */
- (int)unloadSrc:(NSString *)src NS_SWIFT_NAME(unloadSrc(_:));

/**
 * Play a pre-loaded media source
 * @param src Specific src.
 * @return
 * - 0: Success.
 * - < 0: Failure.
 */
- (int)playPreloadedSrc:(NSString *)src NS_SWIFT_NAME(playPreloadedSrc(_:));

/** Set dual-mono output mode of the music file.
  @param mode The audio dual mono mode. See AgoraAudioDualMonoMode.

  @return
  - 0: Success.
  - < 0: Failure.
  */
- (int)setAudioDualMonoMode:(AgoraAudioDualMonoMode)mode NS_SWIFT_NAME(setAudioDualMonoMode(_:));

/**
 * @deprecated
 * Open media file or stream with custom soucrce.
 * The custom data source provides a data stream input callback, and the player will continue to call back this interface, requesting the user to fill in the data that needs to be played.
 *
 * @param startPos Set the starting position for playback, in seconds.
 * @param onReadDataCallback  The player requests to read the data callback , see `AgoraRtcMediaPlayerOnReadCallback`
 * @param onSeekCallback  The Player seek event callback, see `AgoraRtcMediaPlayerOnSeekCallback`
 * @return
 * - 0: Success.
 * - < 0: Failure.
 */
- (int)openWithCustomSourceAtStartPos:(NSInteger)startPos
                 withPlayerOnReadData:(AgoraRtcMediaPlayerCustomSourceOnReadCallback)onReadDataCallback
                      andPlayerOnSeek:(AgoraRtcMediaPlayerCustomSourceOnSeekCallback)onSeekCallback NS_SWIFT_NAME(openWithCustomSourceAtStartPos(_:withPlayerOnReadData:andPlayerOnSeek:));

/**
  * Set spatial audio params for the music file. It can be called after the media player
  * was created.
  *
  * @params params See `AgoraSpatialAudioParams`. If it's
  * not set, then the spatial audio will be disabled; or it will be enabled.
  * @return
  * - 0: Success.
  * - < 0: Failure.
  */
- (int)setSpatialAudioParams:(AgoraSpatialAudioParams* _Nonnull)params NS_SWIFT_NAME(setSpatialAudioParams(_:));

#pragma mark Callback Audio PCM Frame

/**
 * Registers & unregister the player audio observer
 *
 * @param delegate observer object, pass nil to unregister
 * @return
 * - YES: Success.
 * - NO: Failure.
 */
- (BOOL)setAudioFrameDelegate:(id<AgoraRtcMediaPlayerAudioFrameDelegate> _Nullable)delegate NS_SWIFT_NAME(setAudioFrameDelegate(_:));

#pragma mark Callback Video Frame

/**
 * Registers & unregister the player video observer
 *
 * @param delegate observer object, pass nil to unregister.
 * @return
 * - YES: Success.
 * - NO: Failure.
 */
- (BOOL)setVideoFrameDelegate:(id<AgoraRtcMediaPlayerVideoFrameDelegate> _Nullable)delegate NS_SWIFT_NAME(setVideoFrameDelegate(_:));

- (int)registerMediaPlayerAudioSpectrumDelegate:(id<AgoraAudioSpectrumDelegate> _Nullable)delegate
                                   intervalInMS:(NSUInteger)intervalInMS NS_SWIFT_NAME(registerMediaPlayerAudioSpectrumDelegate(_:intervalInMS:));

- (int)unregisterMediaPlayerAudioSpectrumDelegate:(id<AgoraAudioSpectrumDelegate> _Nullable)delegate NS_SWIFT_NAME(unregisterMediaPlayerAudioSpectrumDelegate(_:));
@end


@protocol AgoraRtcMediaPlayerCacheManagerProtocol <NSObject>

/**
 * Get shared cacheManager instance.
 * @return cacheManager instance.
 */
+ (instancetype)sharedInstance NS_SWIFT_NAME(sharedInstance());
/**
 * Remove all media resource cache files.
 * @return
 * - 0: Success.
 * - < 0: Failure.
 */
- (int)removeAllCaches NS_SWIFT_NAME(removeAllCaches());
/**
 * Remove the latest media resource cache file.
 * @return
 * - 0: Success.
 * - < 0: Failure.
 */
- (int)removeOldCache NS_SWIFT_NAME(removeOldCache());
/**
 * Remove the cache file by uri, setting by MediaSource.
 * @param uri URI，identify the uniqueness of the property, Set from `MeidaSource`
 * @return
 * - 0: Success.
 * - < 0: Failure.
 */
- (int)removeCacheByUri:(NSString *)uri NS_SWIFT_NAME(removeCache(byUri:));
/**
 * Set cache file path that files will be saved to.
 * @param cacheDir cacheDir path.
 * @return
 * - 0: Success.
 * - < 0: Failure.
 */
- (int)setCacheDir:(NSString *)cacheDir NS_SWIFT_NAME(setCacheDir(_:));
/**
 * Set the maximum number of cached files.
 * @param count maximum number of cached files.
 * @return
 * - 0: Success.
 * - < 0: Failure.
 */
- (int)setMaxCacheFileCount:(NSInteger)count NS_SWIFT_NAME(setMaxCacheFileCount(_:));
/**
 * Set the total size of the largest cache file.
 * @param cacheSize total size of the largest cache file.
 * @return
 * - 0: Success.
 * - < 0: Failure.
 */
- (int)setMaxCacheFileSize:(NSInteger)cacheSize NS_SWIFT_NAME(setMaxCacheFileSize(_:));
/**
 * Set whether the player will clean up the cache in the order of resource usage.
 * @param enable enable the player to automatically clear the cache.
 * @return
 * - 0: Success.
 * - < 0: Failure.
 */
- (int)enableAutoRemoveCache:(BOOL)enable NS_SWIFT_NAME(enableAutoRemoveCache(_:));
/**
 * Get the cache directory you have set.
 * @return cacheDir
 */
- (NSString *)cacheDir NS_SWIFT_NAME(cacheDir());
/**
 * Get the maximum number of cached files.
 * @return
 * > 0: file count.
 * - < 0: Failure.
 */
- (NSInteger)maxCacheFileCount NS_SWIFT_NAME(maxCacheFileCount());
/**
 * Get the total size of the largest cache file
 * @return
 * > 0: file size.
 * - < 0: Failure.
 */
- (NSInteger)maxCacheFileSize NS_SWIFT_NAME(maxCacheFileSize());
/**
 * Get the number of all cache files.
 * @return
 * > 0: file count.
 * - < 0: Failure.
 */
- (NSInteger)cacheFileCount NS_SWIFT_NAME(cacheFileCount());

@end

NS_ASSUME_NONNULL_END
