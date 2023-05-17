//
//  Agora Media SDK
//
//  Created by FanYuanYuan in 2022-05.
//  Copyright (c) 2022 Agora IO. All rights reserved.
//

#pragma once

#include <AgoraRefPtr.h>
#include "IAgoraMediaPlayer.h"

namespace agora {
namespace rtc {

typedef enum
{
    /**
     * 0: No error occurs and preload succeeds.
     */
    kPreloadStatusCompleted = 0,

    /**
     * 1: A general error occurs.
     */
    kPreloadStatusFailed = 1,

    /**
     * 2: The media is preloading.
     */
    kPreloadStatusPreloading = 2,
} PreloadStatusCode;

typedef enum
{
    /**
     * 0: No error occurs and request succeeds.
     */
    kMusicContentCenterStatusOk = 0,
    /**
     * 1: A general error occurs.
     */
    kMusicContentCenterStatusErr = 1,
} MusicContentCenterStatusCode;

typedef struct 
{
    /**
     * Name of the music chart
     */
    const char* chartName;
    /**
     * Id of the music chart, which is used to get music list
     */
    int32_t id;
} MusicChartInfo;

class MusicChartCollection : public RefCountInterface {
public:
    virtual int getCount() = 0;
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

struct ClimaxSegment
{
    /**
     * The start time of climax segment
     */
    int32_t startTimeMs;
    /**
     * The end time of climax segment
     */
    int32_t endTimeMs;
};

struct Music
{
    /**
     * The songCode of music
     */
    int64_t songCode;
    /**
     * The name of music
     */
    const char* name;
    /**
     * The singer of music
     */
    const char* singer;
    /**
     * The poster url of music
     */
    const char* poster;
    /**
     * The release time of music
     */
    const char* releaseTime;
    /**
     * The duration (in seconds) of music
     */
    int32_t durationS;
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
    int32_t type;
    /**
     * The pitch type of music. 
     * 1, xml lyric has pitch
     * 2, lyric has no pitch
     */
    int32_t pitchType;
    /**
     * The number of lyrics available for the music
     */
    int32_t lyricCount;
    /**
     * The lyric list of music
     * 0, xml
     * 1, lrc
     */
    int32_t* lyricList;
    /**
     * The number of climax segments of the music
     */
    int32_t climaxSegmentCount;
    /**
     * The climax segment list of music
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

class MusicCollection : public RefCountInterface {
public:
    virtual int getCount() = 0;
    virtual int getTotal() = 0;
    virtual int getPage() = 0;
    virtual int getPageSize() = 0;
    virtual Music* getMusic(int32_t index) = 0;
protected:
    virtual ~MusicCollection() = default;
};


class IMusicContentCenterEventHandler {
public:
    /**
     * The music chart result callback; occurs when getMusicCharts method is called.
     * @param requestId The request id same with return from getMusicCharts.
     * @param status The status of the request. See MusicContentCenterStatusCode
     * @param result The result of music chart collection
     */
    virtual void onMusicChartsResult(const char* requestId, MusicContentCenterStatusCode status, agora_refptr<MusicChartCollection> result) = 0;

    /**
     * Music collection, occurs when getMusicCollectionByMusicChartId or searchMusic method is called.
     * @param requestId The request id same with return from getMusicCollectionByMusicChartId or searchMusic
     * @param status The status of the request. See MusicContentCenterStatusCode
     * @param result The result of music collection
     */
    virtual void onMusicCollectionResult(const char* requestId, MusicContentCenterStatusCode status, agora_refptr<MusicCollection> result) = 0;

    /**
     * Lyric url callback of getLyric, occurs when getLyric is called
     * @param requestId The request id same with return from getLyric
     * @param lyricUrl  The lyric url of this music
     */
    virtual void onLyricResult(const char* requestId, const char* lyricUrl) = 0;
    /**
     * preload process callback, occurs when preload is called
     *
     * @param songCode Song code
     * @param percent Preload progress (0 ~ 100)
     * @param status Preload status; see PreloadStatusCode.
     * @param msg The extra information
     * @param lyricUrl  The lyric url of this music
     */
    virtual void onPreLoadEvent(int64_t songCode, int percent, PreloadStatusCode status, const char* msg, const char* lyricUrl = nullptr) = 0;

    virtual ~IMusicContentCenterEventHandler() {};
};

struct MusicContentCenterConfiguration {
    /**
     * The app ID of the project that has enabled the music content center
     */
    const char *appId;
    /**
     * music content center need token to connect with server
     */
    const char *token;
    /**
     * The user ID when using music content center. It can be different from that of the rtc product.
     */
    int64_t mccUid;
    /**
     * event handler to get callback result.
     */
    IMusicContentCenterEventHandler* eventHandler;
    MusicContentCenterConfiguration():appId(nullptr),token(nullptr),eventHandler(nullptr),mccUid(0){}
    MusicContentCenterConfiguration(const char*appid,const char* token,int64_t id,IMusicContentCenterEventHandler* handler):
        appId(appid),token(token),mccUid(id),eventHandler(handler){}
};

class IMusicPlayer : public IMediaPlayer {
protected:
    virtual ~IMusicPlayer() {};

public:
    IMusicPlayer() {};
    /**
    * open a media file with specified parameters.
    *
    * @param songCode The identifier of the media file that you want to play.
    * @param startPos The playback position (ms) of the music file.
    * @return
    * - 0: Success.
    * - < 0: Failure.
    */
    virtual int open(int64_t songCode, int64_t startPos = 0) = 0;
};

class IMusicContentCenter
{
protected:
    virtual ~IMusicContentCenter(){};
public:
    IMusicContentCenter() {};

    /**
     * Initializes the IMusicContentCenter
     * Set token of music content center and other params
     *
     * @param configuration
     * @return
     * - 0: Success.
     * - < 0: Failure.
     */
    virtual int initialize(const MusicContentCenterConfiguration & configuration) = 0;

    /**
     * Renew token of music content center
     * 
     * @param token The new token.
     * @return 
     * - 0: Success.
     * - < 0: Failure.
     */
    virtual int renewToken(const char* token) = 0;

    /**
     * release music content center resource.
     * 
     */
    virtual void release() = 0;

    /**
    *  register event handler.
    */
    virtual int registerEventHandler(IMusicContentCenterEventHandler* eventHandler) = 0;
    
    /**
    *  unregister event handler.
    */
    virtual int unregisterEventHandler() = 0;

    /**
     * Creates a music player source object and return its pointer.
     * @return
     * - The pointer to \ref rtc::IMusicPlayer "IMusicPlayer",
     *   if the method call succeeds.
     * - The empty pointer NULL, if the method call fails.
     */
    virtual agora_refptr<IMusicPlayer> createMusicPlayer() = 0;
    
    /**
     * Get music chart collection of music.
     * If the method call succeeds, get result from the 
     * \ref agora::rtc::IMusicContentCenterEventHandler::onMusicChartsResult
     * "onMusicChartsResult" callback 
     * @param requestId The request id you will get of this query, format is uuid.
     * @return
     * - 0: Success.
     * - < 0: Failure.
     */
    virtual int getMusicCharts(agora::util::AString& requestId) = 0;
    
    /**
     * Get music collection of the music chart by musicChartId and page info. 
     * If the method call success, get result from the 
     * \ref agora::rtc::IMusicContentCenterEventHandler::onMusicCollectionResult
     * "onMusicCollectionResult" callback 
     * @param requestId The request id you will get of this query, format is uuid.
     * @param musicChartId The music chart id obtained from getMusicCharts.
     * @param page The page of the music chart, starting from 1
     * @param pageSize The page size, max is 50.
     * @param jsonOption The ext param, default is null.
     * @return
     * - 0: Success.
     * - < 0: Failure.
     */
    virtual int getMusicCollectionByMusicChartId(agora::util::AString& requestId, int32_t musicChartId, int32_t page, int32_t pageSize, const char* jsonOption = nullptr) = 0;
    
    /**
     * Search music by keyword and page info.
     * If the method call success, get result from the 
     * \ref agora::rtc::IMusicContentCenterEventHandler::onMusicCollectionResult
     * "onMusicCollectionResult" callback 
     * @param requestId The request id you will get of this query, format is uuid.
     * @param keyWord The key word to search.
     * @param page The page of music search result , start from 1.
     * @param pageSize The page size, max is 50.
     * @param jsonOption The ext param, default is null.
     * @return
     * - 0: Success.
     * - < 0: Failure.
     */
    virtual int searchMusic(agora::util::AString& requestId, const char* keyWord, int32_t page, int32_t pageSize, const char* jsonOption = nullptr) = 0;
    
    /**
     * preload a media file with specified parameters.
     *
     * @param songCode The identifier of the media file that you want to play.
     * @param jsonOption The ext param, default is null.
     * @return
     * - 0: Success.
     * - < 0: Failure.
     */
    virtual int preload(int64_t songCode, const char* jsonOption = nullptr) = 0;
    
    /**
     * check if the media file is preloaded
     *
     * @param songCode The identifier of the media file that you want to play.
     * @return
     * - 0: Success, file is preloaded.
     * - < 0: Failure.
     */
    virtual int isPreloaded(int64_t songCode) = 0;

    /**
     * get lyric of the music.
     *
     * @param songCode The identifier of the media file that you want to play.
     * @param LyricType The type of the lyric file. 0:xml or 1:lrc.
     * @return
     * - 0: Success.
     * - < 0: Failure.
     */
    virtual int getLyric(agora::util::AString& requestId, int64_t songCode, int32_t LyricType = 0) = 0;
};

}  // namespace rtc
}  // namespace agora
