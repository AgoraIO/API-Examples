//
//  AgoraMediaPlayerEx.hpp
//  player_demo_apple
//
//  Created by zhanxiaochao on 2020/5/26.
//  Copyright © 2020 agora. All rights reserved.
//
#ifdef MEDIAPLAYER
#ifndef AgoraMediaPlayerEx_h
#define AgoraMediaPlayerEx_h
#include <stdio.h>
#include <iostream>
#import <AgoraMediaPlayer/IAgoraMediaPlayer.h>
#import <AgoraRtcKit/IAgoraRtcEngine.h>
class AgoraMediaPlayerObserver : public agora::rtc::IMediaPlayerObserver
{
    ;
    /**
     * @brief Triggered when the player state changes
     *
     * @param state New player state
     * @param ec Player error message
     */
    virtual void onPlayerStateChanged(agora::media::MEDIA_PLAYER_STATE state,
        agora::media::MEDIA_PLAYER_ERROR ec)
    {

    }

    /**
     * @brief Triggered when the player progress changes, once every 1 second
     *
     * @param position Current playback progress, in seconds
     */
    virtual void onPositionChanged(const int64_t position)
    {

    }
    /**
     * @brief Triggered when the player have some event
     *
     * @param event media player event
     */
    virtual void onPlayerEvent(agora::media::MEDIA_PLAYER_EVENT event)
    {

    };

    /**
     * @brief Triggered when metadata is obtained
     *
     * @param type Metadata type
     * @param data data
     * @param length  data length
     */
    virtual void onMetadata(agora::media::MEDIA_PLAYER_METADATA_TYPE type, const uint8_t* data,
        uint32_t length)
    {

    }
};
 
class IAgoraMediaPlayerEx{
public:
    virtual void attachMediaPlayer(agora::rtc::IMediaPlayer *player,agora::rtc::IRtcEngine *rtcEngine) = 0;
    virtual void registerMediaPlayerObserver(AgoraMediaPlayerObserver * observer) = 0;
    virtual void publishAudio() = 0;
    virtual void publishVideo() = 0;
    virtual void unpublishVideo() = 0;
    virtual void unpublishAudio() = 0;
    virtual void adjustPlayoutSignalVolume(int volume) = 0;
    virtual void adjustPublishSignalVolume(int volume) = 0;
    virtual void detachPlayerFromRtc() = 0;
    virtual ~IAgoraMediaPlayerEx() = default;

};
IAgoraMediaPlayerEx * createAgoraMediaPlayerFactory();




#endif /* AgoraMediaPlayerEx_hpp */
#endif
