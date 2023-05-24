//
//  AgoraMusicContentCenter.h
//  AgoraMusicContentCenter
//
//  Created by dingyusong on 2022/6/1.
//  Copyright © 2022 Agora. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "AgoraRtcEngineKit.h"
/**
 * The status of preload request
 */
typedef NS_ENUM(NSUInteger, AgoraMusicContentCenterPreloadStatus) {
    /**
     * 0: No error occurs and preload succeeds.
     */
    AgoraMusicContentCenterPreloadStatusOK = 0,

    /**
     * 1: A general error occurs.
     */
    AgoraMusicContentCenterPreloadStatusError = 1,

    /**
     * 2: The media file is preloading.
     */
    AgoraMusicContentCenterPreloadStatusPreloading = 2,
    
    /**
     * 3: The media file is removed.
     */
    AgoraMusicContentCenterPreloadStatusRemoveCache = 3,
};

/**
 * the status of search or get top list request
 */
typedef NS_ENUM(NSUInteger, AgoraMusicContentCenterStatusCode) {
    /**
     * 0: No error occurs and request succeeds.
     */
    AgoraMusicContentCenterStatusCodeOK = 0,
    /**
     * 1: A general error occurs.
     */
    AgoraMusicContentCenterStatusCodeError = 1,
    /**
     * 2: The gateway error. There are several possible reasons:
     *  - Token is expired. Check if your token is expired.
     *  - Token is invalid. Check the type of token you passed in.
     *  - Network error. Check your network.
     */
    AgoraMusicContentCenterStatusCodeErrorGateway = 2,
    /**
     * 3: Permission and resource error. There are several possible reasons:
     *  - Your appid may not have the mcc permission. Please contact technical support 
     *  - The resource may not exist. Please contact technical support
     */
    AgoraMusicContentCenterStatusCodeErrorPermissionAndResource = 3,
    /**
     * 4: Internal data parse error. Please contact technical support
     */
    AgoraMusicContentCenterStatusCodeErrorInternalDataParse = 4,
    /**
     * 5: Music loading error. Please contact technical support
     */
    AgoraMusicContentCenterStatusCodeErrorMusicLoading = 5,
    /**
     * 6: Music decryption error. Please contact technical support
     */
    AgoraMusicContentCenterStatusCodeErrorMusicDecryption = 6,

};

typedef NS_ENUM(NSUInteger, AgoraMusicCacheStatusType) {
    /**
     * 0: The media file is already cached.
     */
    AgoraMusicCacheStatusTypeCached = 0,

    /**
     * 1: The media file is being cached.
     */
    AgoraMusicCacheStatusTypeCaching = 1,
};

NS_ASSUME_NONNULL_BEGIN

__attribute__((visibility("default"))) @interface AgoraMusicCacheInfo : NSObject
/**
 * The songCode of the music
 */
@property(nonatomic, assign) NSInteger songCode;

/**
 * The cache status of the music
 */
@property(nonatomic, assign) AgoraMusicCacheStatusType statusType;
@end

__attribute__((visibility("default"))) @interface AgoraMusicChartInfo : NSObject
/**
 * Name of the music chart
 */
@property (nonatomic, copy) NSString *chartName;
/**
 * Id of the music chart, which is used to get music list
 */
@property (nonatomic, assign) NSInteger identify;
@end

__attribute__((visibility("default"))) @interface AgoraMvProperty : NSObject
/**
 * The resolution of the mv
 */
@property (nonatomic, copy) NSString *resolution;
/**
 * The bandwidth of the mv
 */
@property (nonatomic, copy) NSString *bandwidth;
@end

__attribute__((visibility("default"))) @interface AgoraClimaxSegment : NSObject
/**
 * The start time of climax segment
 */
@property(nonatomic, assign) NSInteger startTimeMs;
/**
 * The end time of climax segment
 */
@property(nonatomic, assign) NSInteger endTimeMs;
@end

/**
 * The music info
 */
__attribute__((visibility("default"))) @interface AgoraMusic : NSObject
/**
 * The songCode of music
 */
@property(nonatomic, assign) NSInteger songCode;
/**
 * The type of music
 * 1, mp3 with instrumental accompaniment and original
 * 2, mp3 only with instrumental accompaniment
 * 3, mp3 only with original
 * 4, mp4 with instrumental accompaniment and original
 * 5, mv only
 * 6, new type mp4 with instrumental accompaniment and original
 * detail at document of music media center
 */
@property(nonatomic, assign) NSInteger type;
/**
 * The pitch type of music. 
 * 1, xml lyric has pitch
 * 2, lyric has no pitch
 */
@property(nonatomic, assign) NSInteger pitchType;
/**
 * The name of music
 */
@property(nonatomic, copy) NSString* name;
/**
 * The singer of music
 */
@property(nonatomic, copy) NSString* singer;
/**
 * The poster url of music
 */
@property(nonatomic, copy) NSString* poster;

/**
 * The release time of music
 */
@property(nonatomic, copy) NSString* releaseTime;
/**
 * The duration (in seconds) of music
 */
@property(nonatomic, assign) NSInteger durationS;
/**
 * The lyric list of music
 * 0, xml
 * 1, lrc
 */
@property(nonatomic, strong) NSArray<NSNumber *>* lyricList;
/**
 * The mv property list of music
 */
@property(nonatomic, strong) NSArray<AgoraMvProperty *>* mvPropertyList;
/**
 * The climax segment list of music
 */
@property(nonatomic, strong) NSArray<AgoraClimaxSegment *>* climaxSegmentList;
@end

/**
 * The music collection info
 */
__attribute__((visibility("default"))) @interface AgoraMusicCollection : NSObject
/**
 * This page contains how many AgoraMusic object
 */
@property(nonatomic, assign) NSInteger count;
/**
 * Total number of this search result or total number of the type music sources 
 */
@property(nonatomic, assign) NSInteger total;
/**
 * This  current page number
 */
@property(nonatomic, assign) NSInteger page;
/**
 * This request page size
 */
@property(nonatomic, assign) NSInteger pageSize;
/**
 * This music list of the request result
 */
@property(nonatomic, strong) NSArray<AgoraMusic *>* musicList;
@end

/**
 * The request event delegate callback
 */
@protocol AgoraMusicContentCenterEventDelegate <NSObject>
/**
 * The music chart result callback; occurs when getMusicCharts method is called.
 * 
 * @param requestId The request id same with return from getMusicCharts.
 * @param result The result of music chart collection
 * @param errorCode The status of the request. See MusicContentCenterStatusCode
 */
- (void)onMusicChartsResult:(NSString *)requestId result:(NSArray<AgoraMusicChartInfo*> *)result errorCode:(AgoraMusicContentCenterStatusCode)errorCode;

/**
 * Music collection, occurs when getMusicCollectionByMusicChartId or searchMusic method is called.
 * 
 * @param requestId The request id is the same with that returned by getMusicCollectionByMusicChartId or searchMusic
 * @param result The result of music collection
 * @param errorCode The status of the request. See MusicContentCenterStatusCode
 */
- (void)onMusicCollectionResult:(NSString *)requestId result:(AgoraMusicCollection *)result errorCode:(AgoraMusicContentCenterStatusCode)errorCode;

/**
 * Lyric url callback of getLyric, occurs when getLyric is called
 * 
 * @param requestId The request id same with return from getLyric
 * @param lyricUrl  The lyric url of this music
 * @param errorCode The status of the request. See MusicContentCenterStatusCode 
 */
- (void)onLyricResult:(NSString*)requestId lyricUrl:(NSString* _Nullable)lyricUrl errorCode:(AgoraMusicContentCenterStatusCode)errorCode;

/**
 * Preload process callback, occurs when preload is called
 *
 * @param songCode Song code
 * @param percent Preload progress (0 ~ 100)
 * @param lyricUrl  The lyric url of this music
 * @param status Preload status; see PreloadStatusCode.
 * @param errorCode The status of the request. See MusicContentCenterStatusCode
 */
- (void)onPreLoadEvent:(NSInteger)songCode percent:(NSInteger)percent lyricUrl:(NSString * _Nullable)lyricUrl status:(AgoraMusicContentCenterPreloadStatus)status errorCode:(AgoraMusicContentCenterStatusCode)errorCode;
@end


@class AgoraRtcEngineKit;
__attribute__((visibility("default"))) @interface AgoraMusicContentCenterConfig : NSObject
@property(assign, nonatomic) AgoraRtcEngineKit* _Nullable rtcEngine;
/**
 * The app ID of the project that has enabled the music content center
 */
@property (nonatomic, copy) NSString *appId;
/**
 * music content center need token to connect with server
 */
@property (nonatomic, copy) NSString *token;
/**
 * The user ID when using music content center. It can be different from that of the rtc product.
 */
@property (nonatomic, assign) NSInteger mccUid;
/**
 * The max number which the music content center caches cannot exceed 50.
 */
@property (nonatomic, assign) NSUInteger maxCacheSize;
@end

@protocol AgoraMusicPlayerProtocol <AgoraRtcMediaPlayerProtocol>
/**
 * Open a media file with specified parameters.
 *
 * @param songCode The identifier of the media file that you want to play.
 * @param startPos The playback position (ms) of the music file.
 * @return
 * - 0: Success.
 * - < 0: Failure.
 */
- (NSInteger)openMediaWithSongCode:(NSInteger)songCode startPos:(NSInteger)startPos NS_SWIFT_NAME(openMedia(songCode:startPos:));
@end


__attribute__((visibility("default"))) @interface AgoraMusicContentCenter : NSObject

/**
 * Create an AgoraMusicContentCenter instance.
 *   
 * @param config Configurations for the AgoraMusicContentCenter instance. For details, see AgoraMusicContentCenterConfig.
 * @return An shared instance of AgoraMusicContentCenter
*/
+ (instancetype _Nullable)sharedContentCenterWithConfig:(AgoraMusicContentCenterConfig *)config NS_SWIFT_NAME(sharedContentCenter(config:));

/**
*  Renew token of music content center
*  @param token    The new token.
*  @return 
* - 0: Success.
* - < 0: Failure.
*/
- (NSInteger)renewToken:(NSString * _Nonnull)token;

/**
 * Register an event delegate, only the last delegate is working.
 * 
 * @param eventDelegate the object who need AgoraRtcMediaPlayerDelegate method to get the player information ,
 * if you want remove the delegate, just pass nil
 * @return
 * - 0: Success.
 * - < 0: Failure.
 */
- (NSInteger)registerEventDelegate:(id<AgoraMusicContentCenterEventDelegate> _Nullable)eventDelegate;

/**
 * Creates a music player source object and return its pointer.
 * 
 * @param delegate The object who need AgoraRtcMediaPlayerDelegate method to get the player information
 * @return
 * - The pointer to an object who realize the AgoraMusicPlayerProtocol, if the method call succeeds.
 * - The empty pointer NULL, if the method call fails.
 */
- (id<AgoraMusicPlayerProtocol>)createMusicPlayerWithDelegate:(id<AgoraRtcMediaPlayerDelegate> _Nullable)delegate NS_SWIFT_NAME(createMusicPlayer(delegate:));

/**
 * Get music chart collection of music.If the method call success, get result from the AgoraMusicContentCenterEventDelegate  - (void)onMusicChartsResult:(NSString *)requestId status:(AgoraMusicContentCenterStatusCode)status result:(NSArray<AgoraMusicChartInfo*> *)result; match the callback  "requestId" parameter to get the  request result.
 * 
 * @return The request identification
 */
- (NSString *)getMusicCharts;

/**
 * Get hot music list by hotType and page info.If the method call success, get result from the AgoraMusicContentCenterEventDelegate  - (void)onMusicCollectionResult:(NSString *)requestId status:(AgoraMusicContentCenterStatusCode)status result:(AgoraMusicCollection *)result;  match the callback  "requestId" parameter to get the  request result.
 * 
 * @param musicChartId The music chart id obtained from getMusicCharts.
 * @param page The page of the music chart, starting from 1
 * @param pageSize The page size, max is 50.
 * @param jsonOption The ext param, default is null.
 * @return The request identification
 */
- (NSString *)getMusicCollectionWithMusicChartId:(NSInteger)musicChartId page:(NSInteger)page pageSize:(NSInteger)pageSize jsonOption:(NSString * _Nullable)jsonOption NS_SWIFT_NAME(getMusicCollection(musicChartId:page:pageSize:jsonOption:));

/**
 * Search music by keyword and page info. get result from the AgoraMusicContentCenterEventDelegate  - (void)onMusicCollectionResult:(NSString *)requestId status:(AgoraMusicContentCenterStatusCode)status result:(AgoraMusicCollection *)result;  match the callback  "requestId" parameter to get the  request result.
 * 
 * @param keyWord The key word to search.
 * @param page The page of the music search result, starting from 1
 * @param pageSize The page size, max is 50.
 * @param jsonOption The ext param, default is null.
 * @return The request identification
 */
- (NSString *)searchMusicWithKeyWord:(NSString *)keyWord page:(NSInteger)page pageSize:(NSInteger)pageSize jsonOption:(NSString * _Nullable)jsonOption NS_SWIFT_NAME(searchMusic(keyWord:page:pageSize:jsonOption:));

/**
 * Preload a media file with specified parameters.
 *
 * @param songCode The identify of the media file that you want to play.
 * @param option The ext param, default is null.
 * @return
 * - 0: Success.
 * - < 0: Failure.
 */
- (NSInteger)preloadWithSongCode:(NSInteger)songCode jsonOption:(NSString* _Nullable)jsonOption NS_SWIFT_NAME(preload(songCode:jsonOption:));

/**
 * Preload a media file with specified parameters.
 *
 * @param songCode The identify of the media file that you want to play.
 * @return
 * - 0: Success.
 * - < 0: Failure.
 */
- (NSInteger)isPreloadedWithSongCode:(NSInteger)songCode NS_SWIFT_NAME(isPreloaded(songCode:));

/**
 * Remove a media file cache
 *
 * @param songCode The identifier of the media file that you want to play.
 * @return
 * - 0: Success; the cached media file is removed.
 * - < 0: Failure.
 */
- (NSInteger)removeCacheWithSongCode:(NSInteger)songCode NS_SWIFT_NAME(removeCache(songCode:));

/**
 * Get media cache files.
 *
 * @return The caches Array contains songCode and status of the music.
 */
- (NSArray *)getCaches NS_SWIFT_NAME(getCaches());

/**
 * Get lyric of the song. get result from the AgoraMusicContentCenterEventDelegate  - (void)onLyricResult:(NSString*)requestId lyricUrl:(NSString*)lyricUrl;  match the callback  "requestId" parameter to get the  request result.
 *
 * @param songCode The identify of the media file that you want to play.
 * @param LyricType The type of the lyric file. may be 0:xml or 1:lrc.
 * @return The request identification
 */
- (NSString *)getLyricWithSongCode:(NSInteger)songCode lyricType:(NSInteger)lyricType NS_SWIFT_NAME(getLyric(songCode:lyricType:));

/**
 * If you want AgoraMusicContentCenterEventDelegate methods callback in the mainThread ,you should set enable YES. Default the delegate callback in subthread.
 * - `NO`: (Default)Send the delegate callback in subthread.
 * - `YES`: Send the delegate callback in mainthread.
 */
- (void)enableMainQueueDispatch:(BOOL)enabled;
 
/**
 * Destroy the shared instance of AgoraMusicContentCenter
 * 
 * @note If you call the method, you should call it brefore AgoraRtcEngineKit destroy 
 */
+ (void)destroy;

@end

NS_ASSUME_NONNULL_END
