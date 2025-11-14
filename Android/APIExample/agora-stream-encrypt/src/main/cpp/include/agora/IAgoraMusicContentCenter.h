//
//  Agora Media SDK
//
//  Created by FanYuanYuan in 2022-05.
//  Copyright (c) 2022 Agora IO. All rights reserved.
//

#pragma once

#include "AgoraRefPtr.h"
#include "IAgoraMediaPlayer.h"

namespace agora {
namespace rtc {

/**
 * @brief Playback mode of a music resource.
 */
typedef enum
{
    /**
     * 0: Original vocals.
     */
    kMusicPlayModeOriginal = 0,

    /**
     * 1: Accompaniment.
     */
    kMusicPlayModeAccompany = 1,
    
    /**
     * 2: Vocal guide.
     */
    kMusicPlayModeLeadSing = 2,

} MusicPlayMode;

/**
 * @brief Loading state of a music resource.
 */
typedef enum
{
    /**
     * 0: Music resource loading completed.
     */
    kPreloadStateCompleted = 0,

    /**
     * 1: Music resource loading failed.
     */
    kPreloadStateFailed = 1,

    /**
     * 2: Music resource is currently loading.
     */
    kPreloadStatePreloading = 2,
    /**
     * 3: Cached music resource has been removed.
     */
    kPreloadStateRemoved = 3,
} PreloadState;

/**
 * @brief Request status codes for the Music Content Center.
 */
typedef enum
{
    /**
     * 0: Request succeeded.
     */
    kMusicContentCenterReasonOk = 0,
    /**
     * 1: General error with no specific cause.
     */
    kMusicContentCenterReasonError = 1,
    /**
     * 2: Gateway error. Possible reasons include:
     * - The current token has expired. Please regenerate the token.
     * - The token provided is invalid. Make sure you are using an RTM token.
     * - Network error. Please check your connection.
     */
    kMusicContentCenterReasonGateway = 2,
    /**
     * 3: Permission error or music resource does not exist. Make sure your project has Music Content
     * Center enabled. Please `contact technical support`.
     */
    kMusicContentCenterReasonPermissionAndResource = 3,
    /**
     * 4: Internal data parsing error. Please `contact technical support`.
     */
    kMusicContentCenterReasonInternalDataParse = 4,
    /**
     * 5: Error occurred while loading the music resource. Please `contact technical support`.
     */
    kMusicContentCenterReasonMusicLoading = 5,
    /**
     * 6: Error occurred while decrypting the music resource. Please `contact technical support`.
     */
    kMusicContentCenterReasonMusicDecryption = 6, 
    /**
     * 7: Internal HTTP error. Please try again later.
     */
    kMusicContentCenterReasonHttpInternalError = 7, 
} MusicContentCenterStateReason;

/**
 * @brief Detailed information about a music chart.
 */
typedef struct 
{
    /**
     * Name of the chart.
     */
    const char* chartName;
    /**
     * ID of the music chart.
     */
    int32_t id;
} MusicChartInfo;

/**
 * @brief Cache status of a music resource.
 */
enum MUSIC_CACHE_STATUS_TYPE {
    /**
     * 0: The music resource is cached.
     */
    MUSIC_CACHE_STATUS_TYPE_CACHED = 0,
    /**
     * 1: The music resource is being cached.
     */
    MUSIC_CACHE_STATUS_TYPE_CACHING = 1
};

/**
 * @brief Information about a cached music resource.
 */
struct MusicCacheInfo {
    /**
     * The ID of the music resource, used to identify the resource.
     */
    int64_t songCode;
    /**
     * Cache status of the music resource. See `MUSIC_CACHE_STATUS_TYPE`.
     */
    MUSIC_CACHE_STATUS_TYPE status;
    MusicCacheInfo():songCode(0), status(MUSIC_CACHE_STATUS_TYPE_CACHED) {}
};

/**
 * @brief Detailed information about music charts.
 */
class MusicChartCollection : public RefCountInterface {
public:
    /**
     * @brief Gets the number of music charts in this request.
     *
     * @return
     * The number of music charts in this request.
     */
    virtual int getCount() = 0;
    /**
     * @brief Gets the detailed information of a music chart.
     *
     * @param index Index of the `MusicChartInfo` array.
     *
     * @return
     * `MusicChartInfo`, containing the detailed information of the music chart.
     */
    virtual MusicChartInfo* get(int index) = 0;
protected:
    virtual ~MusicChartCollection() = default;
};

struct MvProperty
{
    /**
     * The resolution of the mv
     */
    const char* resolution;
    /**
     * The bandwidth of the mv
     */
    const char* bandwidth;
};

/**
 * @brief The climax parts of the music.
 */
struct ClimaxSegment
{
    /**
     * The time (ms) when the climax part begins.
     */
    int32_t startTimeMs;
    /**
     * The time (ms) when the climax part ends.
     */
    int32_t endTimeMs;
};

/**
 * @brief Detailed information of a music resource.
 */
struct Music
{
    /**
     * The ID of the music resource, used to identify a music item.
     */
    int64_t songCode;
    /**
     * Name of the music resource.
     */
    const char* name;
    /**
     * Name of the singer.
     */
    const char* singer;
    /**
     * Download URL of the music poster.
     */
    const char* poster;
    /**
     * Release time of the music resource.
     */
    const char* releaseTime;
    /**
     * Total duration of the music resource (in seconds).
     */
    int32_t durationS;
    /**
     * Type of the music resource:
     * - 1: Single-track with accompaniment on the left channel and original vocals on the right
     * channel.
     * - 2: Single-track with accompaniment only.
     * - 3: Single-track with original vocals only.
     * - 4: Multi-track audio.
     */
    int32_t type;
    /**
     * Whether the song supports pitch scoring:
     * - 1: The song supports pitch scoring.
     * - 2: The song does not support pitch scoring.
     */
    int32_t pitchType;
    /**
     * Number of lyrics available for the song.
     */
    int32_t lyricCount;
    /**
     * Supported lyric formats:
     * - 0: XML format.
     * - 1: LRC format.
     */
    int32_t* lyricList;
    /**
     * Number of climax segments.
     */
    int32_t climaxSegmentCount;
    /**
     * List of climax segments. See `ClimaxSegment`.
     */
    ClimaxSegment* climaxSegmentList;
    /**
     * The number of mv of the music
     * If this value is greater than zero, it means the current music has MV resource
     */
    int32_t mvPropertyCount;
    /**
     * The mv property list of music
     */
    MvProperty* mvPropertyList;
};

/**
 * @brief Detailed information about the music resource list.
 */
class MusicCollection : public RefCountInterface {
public:
    /**
     * @brief Gets the number of music items in this request.
     *
     * @return
     * The number of music items in this request.
     */
    virtual int getCount() = 0;
    /**
     * @brief Gets the total number of music resources in the list.
     *
     * @return
     * The total number of music resources in the list.
     */
    virtual int getTotal() = 0;
    /**
     * @brief Gets the current page number of the music resource list.
     *
     * @return
     * The current page number.
     */
    virtual int getPage() = 0;
    /**
     * @brief Gets the actual number of music resources returned by the SDK.
     *
     * @return
     * The actual number of music resources returned by the SDK.
     */
    virtual int getPageSize() = 0;
    /**
     * @brief Gets the detailed information of a music resource in the current page list.
     *
     * @param index Index of the `Music` array.
     *
     * @return
     * A `Music` instance.
     */
    virtual Music* getMusic(int32_t index) = 0;
protected:
    virtual ~MusicCollection() = default;
};


class IMusicContentCenterEventHandler {
public:
    /**
     * @brief Callback for retrieving music charts.
     *
     * @details
     * After you call the `getMusicCharts` method to retrieve all music charts, the SDK triggers this
     * callback.
     *
     * @param requestId Request ID. A unique identifier for this request.
     * @param reason The request status code from the Music Content Center. See
     * `MusicContentCenterStateReason`.
     * @param result The list of currently playable music charts. See `MusicChartCollection`.
     *
     */
    virtual void onMusicChartsResult(const char* requestId, agora_refptr<MusicChartCollection> result, MusicContentCenterStateReason reason) = 0;

    /**
     * @brief Callback for retrieving the music resource list.
     *
     * @details
     * When you call the `getMusicCollectionWithMusicChartId` method to get the music resource list of a
     * specific chart or call `searchMusic` to search for music resources, the SDK
     * triggers this callback to report the detailed information of the music resource list.
     *
     * @param requestId Request ID. A unique identifier for this request.
     * @param reason The request status code from the Music Content Center. See
     * `MusicContentCenterStateReason`.
     * @param result Detailed information of the music resource list. See `MusicCollection`.
     *
     */
    virtual void onMusicCollectionResult(const char* requestId, agora_refptr<MusicCollection> result, MusicContentCenterStateReason reason) = 0;

    /**
     * @brief Callback for the lyrics download URL.
     *
     * @details
     * After you call `getLyric` to get the lyrics download URL for a specific song, the SDK triggers
     * this callback.
     *
     * @param requestId Request ID. A unique identifier for this request.
     * @param songCode The ID of the music resource, used to identify the music.
     * @param lyricUrl The download URL of the lyrics.
     * @param reason The request status code from the Music Content Center. See
     * `MusicContentCenterStateReason`.
     *
     */
    virtual void onLyricResult(const char* requestId, int64_t songCode, const char* lyricUrl, MusicContentCenterStateReason reason) = 0;

    /**
     * @brief Callback for detailed information of a music resource.
     *
     * @details
     * After you call `getSongSimpleInfo` to get detailed information of a music resource, the SDK
     * triggers this callback.
     *
     * @param requestId Request ID. A unique identifier for this request.
     * @param songCode The ID of the music resource, used to identify the music.
     * @param simpleInfo Information about the music resource, including the following:
     * - Start and end time of the chorus segment (ms)
     * - Download URL of the chorus lyrics
     * - Duration of the chorus segment (ms)
     * - Song name
     * - Artist name
     * @param reason Request status code from the Music Content Center. See
     * `MusicContentCenterStateReason`.
     *
     */
    virtual void onSongSimpleInfoResult(const char* requestId, int64_t songCode, const char* simpleInfo, MusicContentCenterStateReason reason) = 0;

    /**
     * @brief Reports events related to preloading music resources.
     *
     * @details
     * After you call `preload(int64_t songCode, const char* jsonOption = nullptr)` or
     * `preload(agora::util::AString& requestId, int64_t songCode)` to preload a music resource, the SDK
     * triggers this callback.
     *
     * @param requestId Request ID. A unique identifier for this request.
     * @param songCode The ID of the music resource, used to identify a music item.
     * @param percent Current loading progress of the music resource, ranging from [0, 100].
     * @param lyricUrl Download URL of the lyrics.
     * @param state Current loading state of the music resource. See `PreloadState`.
     * @param reason Request status code from the Music Content Center. See
     * `MusicContentCenterStateReason`.
     *
     */
    virtual void onPreLoadEvent(const char* requestId, int64_t songCode, int percent, const char* lyricUrl, PreloadState state, MusicContentCenterStateReason reason) = 0;

    virtual ~IMusicContentCenterEventHandler() {};
};

/**
 * @brief Configuration for the Music Content Center.
 */
struct MusicContentCenterConfiguration {
    /**
     * App ID of the project with Music Content Center enabled.
     */
    const char *appId;
    /**
     * RTM Token used for authentication when using the Music Content Center.
     * @note
     * - Agora recommends using AccessToken2 for authentication. See `Deploy Token Server`. When
     * generating the token, pass a `String` type `mccUid` to `uid`.
     * - When your token is about to expire, you can call `renewToken` to pass in a new token.
     */
    const char *token;
    /**
     * User ID for using the Music Content Center. This ID can be the same as the `uid` used when
     * joining an RTC channel, but it cannot be 0.
     */
    int64_t mccUid;
    /**
     * Number of music resources that can be cached. The maximum is 50.
     */
    int32_t maxCacheSize;
    /**
    * @technical preview
    */
    const char* mccDomain;
    /**
     * Event handler to receive callbacks. See `IMusicContentCenterEventHandler`.
     */
    IMusicContentCenterEventHandler* eventHandler;
    MusicContentCenterConfiguration():appId(nullptr),token(nullptr),eventHandler(nullptr),mccUid(0),maxCacheSize(10), mccDomain(nullptr){}
    MusicContentCenterConfiguration(const char*appid,const char* token,int64_t id,IMusicContentCenterEventHandler* handler,int32_t maxSize = 10, const char* apiurl = nullptr):
        appId(appid),token(token),mccUid(id),eventHandler(handler),maxCacheSize(maxSize), mccDomain(apiurl){}
};

class IMusicPlayer : public IMediaPlayer {
protected:
    virtual ~IMusicPlayer() {};

public:
    IMusicPlayer() {};
    using IMediaPlayer::open;
    /**
     * @brief Opens a music resource by its song code.
     *
     * @details
     * Before calling this method, make sure the music resource to be played has been loaded. You can
     * call `isPreloaded` to check whether the resource has been preloaded, or listen for the
     * `onPreLoadEvent` callback.
     * After calling this method, the `onPlayerSourceStateChanged` callback is triggered. Once you
     * receive a playback state of `PLAYER_STATE_OPEN_COMPLETED`, you can call the `play` method to play
     * the media file.
     *
     * @note Note: If the music resource you want to open is protected by digital rights management
     * (DRM), you must use this method to open it. For non-DRM-protected resources, you can choose to
     * open them using this method or the `open` method under the `IMediaPlayer` class.
     *
     * @param songCode The song code of the music resource, used to identify the music.
     * @param startPos The start playback position in milliseconds. Default is 0.
     *
     * @return
     * - 0: Success.
     * - < 0: Failure. See `Error Codes` for details and resolution suggestions.
     */
    virtual int open(int64_t songCode, int64_t startPos = 0) = 0;

    /**
     * @brief Sets the playback mode of a music resource.
     *
     * @details
     * You can call this method to enable original vocals, accompaniment, or vocal guide. If you do not
     * call this method, accompaniment is played by default; if the music resource has no accompaniment,
     * the original vocals are played.
     * Applicable scenarios: In entertainment scenarios such as online karaoke or talent shows, if you
     * need to play copyrighted music provided by Agora's content center, you can call this method to
     * set the playback mode.
     * Call timing: This method must be called after `createMusicPlayer`.
     *
     * @note
     * You can get detailed information about the music resource from the `onMusicCollectionResult`
     * callback, and determine the supported playback types of the copyrighted music from the `result`
     * parameter.
     *
     * @param mode Playback mode. See `MusicPlayMode`.
     *
     * @return
     * - 0: Success.
     * - < 0: Failure. See `Error Codes` for details and resolution suggestions.
     *   - -2: Invalid parameter. Please reset the parameter.
     */
    virtual int setPlayMode(MusicPlayMode mode) = 0;
};

class IMusicContentCenter
{
protected:
    virtual ~IMusicContentCenter(){};
public:
    IMusicContentCenter() {};

    /**
     * @brief Initializes the `IMusicContentCenter`.
     *
     * @details
     * You must call this method to initialize `IMusicContentCenter` before using any other methods
     * under the `IMusicContentCenter` class.
     *
     * @param configuration Configuration for `IMusicContentCenter`. See
     * `MusicContentCenterConfiguration`.
     *
     * @return
     * - 0: Success.
     * - < 0: Failure. See `Error Codes` for details and resolution suggestions.
     */
    virtual int initialize(const MusicContentCenterConfiguration & configuration) = 0;

    /**
     * @brief Renews the token.
     *
     * @details
     * When the token used for authentication is about to expire or has already expired, you can call
     * this method to pass in a newly generated token.
     *
     * @param token The new token.
     *
     * @return
     * - 0: Success.
     * - < 0: Failure. See `Error Codes` for details and resolution suggestions.
     */
    virtual int renewToken(const char* token) = 0;

    /**
     * @brief Releases all resources used by the Music Content Center.
     *
     * @details
     * This method must be called before the `release` method of `IRtcEngine`.
     *
     */
    virtual void release() = 0;

    /**
     * @brief Registers the Music Content Center event handler.
     *
     * @param eventHandler The event handler to register. See `IMusicContentCenterEventHandler`.
     *
     * @return
     * - 0: Success.
     * - < 0: Failure. See `Error Codes` for details and resolution suggestions.
     */
    virtual int registerEventHandler(IMusicContentCenterEventHandler* eventHandler) = 0;
    
    /**
     * @brief Unregisters the Music Content Center event callback.
     *
     * @return
     * - 0: Success.
     * - < 0: Failure. See `Error Codes` for details and resolution suggestions.
     */
    virtual int unregisterEventHandler() = 0;

    /**
     * @brief Creates a music player.
     *
     * @details
     * If you need to play music resources from the Music Content Center, you must first call this
     * method to create a music player.
     *
     * @return
     * - If the method call succeeds: Returns an `IMusicPlayer` object.
     * - If the method call fails: Returns a null pointer.
     */
    virtual agora_refptr<IMusicPlayer> createMusicPlayer() = 0;

    /**
     * @brief Destroys the music player object.
     *
     * @details
     * When you no longer need to use the music player, you can call this method to destroy the music
     * player object. If you need to use the music player again after destruction, call
     * `createMusicPlayer` to recreate a music player object.
     * Call timing: This method can be called before or after joining a channel, but make sure to call
     * it before the `release` method of `IRtcEngine`.
     *
     * @param music_player Pointer to the `IMusicPlayer` object.
     *
     * @return
     * - 0: Success.
     * - < 0: Failure. See `Error Codes` for details and resolution suggestions.
     */
    virtual int destroyMusicPlayer(agora_refptr<IMusicPlayer> music_player) = 0;
    
    /**
     * @brief Gets all music charts.
     *
     * @details
     * After you call this method, the SDK triggers the `onMusicChartsResult` callback to report
     * detailed information about the music charts.
     *
     * @param requestId Request ID. A unique identifier for this request.
     *
     * @return
     * - 0: Success.
     * - < 0: Failure. See `Error Codes` for details and resolution suggestions.
     */
    virtual int getMusicCharts(agora::util::AString& requestId) = 0;
    
    /**
     * @brief Gets the list of music resources from a specified chart by its music chart ID.
     *
     * @details
     * After successfully calling this method, the SDK triggers the `onMusicCollectionResult` callback
     * to report detailed information about the music resources in the chart.
     *
     * @param requestId Request ID. A unique identifier for this request.
     * @param musicChartId The ID of the music chart. You can obtain it from the `onMusicChartsResult`
     * callback. You can also use RESTful APIs to `get the full music library list` or
     * `get incremental music list`.
     * @param page Current page number, starting from 1 by default.
     * @param pageSize Total number of items per page in the music resource list. The maximum value is
     * 50.
     * @param jsonOption Extended JSON field, default is NULL. You can use this field to filter the
     * music resources you need. Currently supports filtering by scoreable music and chorus segments:
     * | Key           | Value                                                                 | Example                  |
     * | ------------- | ---------------------------------------------------------------------- | ------------------------ |
     * | pitchType     | Whether scoring is supported: - 1: Scoreable music. - 2: Non-scoreable music. | {"pitchType":1}         |
     * | needHighPart  | Whether chorus segment is needed: - `true`: Chorus segment needed. - `false`: Not needed. | {"needHighPart":true}   |
     *
     * @return
     * - 0: Success.
     * - < 0: Failure. See `Error Codes` for details and resolution suggestions.
     */
    virtual int getMusicCollectionByMusicChartId(agora::util::AString& requestId, int32_t musicChartId, int32_t page, int32_t pageSize, const char* jsonOption = nullptr) = 0;
    
    /**
     * @brief Searches for music resources.
     *
     * @details
     * After successfully calling this method, the SDK triggers the `onMusicCollectionResult` callback
     * to report the list of retrieved music resources.
     *
     * @param keyword Search keyword. Supports searching by song name or artist.
     * @param page The target page number of the music resource list to retrieve.
     * @param pageSize Maximum number of music resources displayed per page. The maximum value is 50.
     * @param jsonOption Extended JSON field. Default is NULL. You can use this field to filter the
     * music resources you need. Currently supports filtering by scoreable music and chorus segments:
     * | Key           | Value                                                                 | Example                  |
     * | ------------- | ---------------------------------------------------------------------- | ------------------------ |
     * | pitchType     | Whether scoring is supported: - 1: Scoreable music. - 2: Non-scoreable music. | {"pitchType":1}         |
     * | needHighPart  | Whether chorus segment is needed: - `true`: Chorus segment needed. - `false`: Not needed. | {"needHighPart":true}   |
     * @param requestId Request ID. A unique identifier for this request.
     *
     * @return
     * - 0: Success.
     * - < 0: Failure. See `Error Codes` for details and resolution suggestions.
     */
    virtual int searchMusic(agora::util::AString& requestId, const char* keyWord, int32_t page, int32_t pageSize, const char* jsonOption = nullptr) = 0;
    
    /**
     * @brief Preloads a music resource.
     *
     * @deprecated This method is deprecated. Use preload(int64_t songCode) instead.
     *
     * @details
     * You can call this method to preload the music resource you want to play. After successfully
     * calling this method, the SDK triggers the `onPreLoadEvent` callback to report the preload event.
     * Before calling this method to preload a music resource, you need to call
     * `getMusicCollectionWithMusicChartId` or `searchMusic`
     * to get the music resource you want to play, and obtain the song code (`songCode`) from the
     * `onMusicCollectionResult` callback.
     *
     * @note To destroy the `IRtcEngine` object, make sure to call the `release` method only after
     * receiving the `onPreLoadEvent` callback.
     *
     * @param songCode The song code of the music resource, used to identify the music.
     * @param jsonOption Extended JSON field.
     * Agora charges based on the application scenario you pass in the `sceneType` field. Different
     * scenarios have different rates. Refer to the `Billing Description` for details.
     * - 1: Live scene: Karaoke and background music playback.
     * - 2: Live scene: Background music playback.
     * - 3: (Default) Voice chat scene: Karaoke.
     * - 4: Voice chat scene: Background music playback.
     * - 5: VR scene: Karaoke and background music playback.
     *                 If you need to switch to a different scenario, call this method again and pass
     * the new `sceneType` value in this field.
     * Example: `{"sceneType":1}`
     *
     * @return
     * - 0: Success.
     * - < 0: Failure. See `Error Codes` for details and resolution suggestions.
     */
    virtual int preload(int64_t songCode, const char* jsonOption) __deprecated = 0;

    /**
     * @brief Preloads a music resource.
     *
     * @details
     * You can call this method to preload the music resource you want to play. After successfully
     * calling this method, the SDK triggers the `onPreLoadEvent` callback to report the preload event.
     * Before calling this method to preload a music resource, you need to call
     * `getMusicCollectionWithMusicChartId` or `searchMusic`
     * to get the music resource you want to play, and obtain the song code (`songCode`) from the
     * `onMusicCollectionResult` callback.
     *
     * @note To destroy the `IRtcEngine` object, make sure to call the `release` method only after
     * receiving the `onPreLoadEvent` callback.
     *
     * @param songCode The song code of the music resource, used to identify the music.
     * @param requestId Output parameter. Request ID. A unique identifier for this request.
     *
     * @return
     * - 0: Success.
     * - < 0: Failure. See `Error Codes` for details and resolution suggestions.
     */
    virtual int preload(agora::util::AString& requestId, int64_t songCode) = 0;

    /**
     * @brief Deletes a cached music resource.
     *
     * @details
     * You can call this method to delete a specific cached music resource. To delete multiple
     * resources, call this method multiple times.
     *
     * @note Note: This method does not delete cached music resources that are currently being played.
     *
     * @param songCode The ID of the music resource to be deleted.
     *
     * @return
     * - 0: Success. The music resource has been deleted.
     * - < 0: Failure. See `Error Codes` for details and resolution suggestions.
     */
    virtual int removeCache(int64_t songCode) = 0;

    /**
     * @brief Gets information about cached music resources.
     *
     * @details
     * Before calling this method, you need to pre-allocate a certain amount of memory to store
     * information about cached music resources. If you want to set the number of music resources that
     * can be cached, you can configure it through the `configuration` parameter in `initialize`.
     * When you no longer need the cached music resources, you should release the memory in time to
     * prevent memory leaks.
     *
     * @param cacheInfo Output parameter. A pointer to the memory buffer used to store cached music
     * resource information.
     * @param cacheInfoSize Input and output parameter.
     * - Input: The length of the `cacheInfo` array, i.e., the number of `MusicCacheInfo` structures you
     * allocated.
     * - Output: The number of `MusicCacheInfo` structures returned after the method execution.
     *
     * @return
     * - 0: Success.
     * - < 0: Failure. See `Error Codes` for details and resolution suggestions.
     */
    virtual int getCaches(MusicCacheInfo *cacheInfo, int32_t* cacheInfoSize) = 0;

    /**
     * @brief Checks whether a music resource has been preloaded.
     *
     * @details
     * This method is synchronous. To preload a new music resource, call `preload(agora::util::AString&
     * requestId, int64_t songCode)`.
     *
     * @param songCode The ID of the music resource, used to identify a music item.
     *
     * @return
     * - 0: Success. The music resource has been preloaded.
     * - < 0: Failure. See `Error Codes` for details and resolution suggestions.
     */
    virtual int isPreloaded(int64_t songCode) = 0;

    /**
     * @brief Gets the download URL of the lyrics for a music resource.
     *
     * @details
     * After successfully calling this method, the SDK triggers the `onLyricResult` callback to report
     * the lyrics download URL.
     *
     * @param songCode The ID of the music resource, used to identify the music.
     * @param lyricType Type of lyrics:
     * - 0: XML format.
     * - 1: LRC format.
     * @param requestId Request ID. A unique identifier for this request.
     *
     * @return
     * - 0: Success.
     * - < 0: Failure. See `Error Codes` for details and resolution suggestions.
     */
    virtual int getLyric(agora::util::AString& requestId, int64_t songCode, int32_t lyricType = 0) = 0;

    /**
     * @brief Gets detailed information of a specific music resource.
     *
     * @details
     * Before calling this method, you need to obtain the song code of the corresponding music resource.
     * You can get it by calling `getMusicCollectionWithMusicChartId` or `searchMusic`,
     * and retrieve the song code from the `onMusicCollectionResult` callback triggered by those
     * methods.
     * After you call this method, the SDK triggers the `onSongSimpleInfoResult` callback to report the
     * detailed information of the music resource.
     *
     * @param songCode The song code of the music resource, used to identify the music.
     * @param requestId Request ID. A unique identifier for this request.
     *
     * @return
     * - 0: Success.
     * - < 0: Failure. See `Error Codes` for details and resolution suggestions.
     */
    virtual int getSongSimpleInfo(agora::util::AString& requestId, int64_t songCode) = 0;

    /**
     * @brief Creates an internal song code for the chorus segment of a music resource.
     *
     * @details
     * Applicable scenarios: Before playing the chorus segment of a music resource, you need to call
     * this method to create an internal song code for the chorus segment using the `jsonOption`
     * parameter and the music resource's `songCode`.
     * This internal song code serves as the unique identifier for the resource. Once you obtain this
     * code, use it as the `songCode` parameter when calling methods to open, preload, or remove the
     * resource.
     *
     * @param songCode The song code of the music resource, used to identify the resource. You can
     * obtain it by calling `getMusicCollectionWithMusicChartId` or `searchMusic`,
     * and retrieve the song code from the `onMusicCollectionResult` callback triggered by those
     * methods.
     * @param jsonOption Extended JSON field, default is NULL. Currently supports the following values:
     * | Key       | Value              | Example                        |
     * | ---------- | -------------------------------- | ----------------------------- |
     * | sceneType  | Scene type: - 1: Live scene: Karaoke and background music playback. - 2: Live scene: Background music playback. - 3: (Default) Voice chat scene: Karaoke. - 4: Voice chat scene: Background music playback. - 5: VR scene: Karaoke and background music playback. Note: Agora charges based on the scene type you pass in `sceneType`. Different scenes have different rates. See `Billing Description` for details. To switch scenes, you need to call this method again with a new `sceneType`. | {"sceneType":1}              |
     * | highPart   | Index of the chorus segment. You can get the index from the `onMusicCollectionResult` callback and pass it here. The index starts from 0.                                                                                                                         | {"format": {"highpart": 0}} |
     * @param internalSongCode Output parameter, the internal song code of the music resource.
     *
     * @return
     * - 0: Success.
     * - < 0: Failure. See `Error Codes` for details and resolution suggestions.
     */
    virtual int getInternalSongCode(int64_t songCode, const char* jsonOption, int64_t& internalSongCode) = 0;

};

}  // namespace rtc
}  // namespace agora
