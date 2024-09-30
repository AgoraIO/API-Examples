#pragma once
#include "stdafx.h"
#include <IAgoraMediaPlayerSource.h>
#include <IAgoraRtcEngine.h>
#include <IAgoraMediaEngine.h>

using namespace agora;
using namespace agora::rtc;
class AbstractMediaPlayerSourceObserver :
    public IMediaPlayerSourceObserver
{
public:

    virtual void onPlayerSourceStateChanged(media::base::MEDIA_PLAYER_STATE state, media::base::MEDIA_PLAYER_REASON reason) override;
    void onPositionChanged(int64_t positionMs, int64_t timestampMs) override;
    void onPlayerEvent(media::base::MEDIA_PLAYER_EVENT eventCode, int64_t elapsedTime, const char* message) override;
    void onMetaData(const void* data, int length) override;
    void onPlayBufferUpdated(int64_t playCachedBuffer) override;
    void onPreloadEvent(const char* src, media::base::PLAYER_PRELOAD_EVENT event) override;
    void onCompleted() override;
    void onAgoraCDNTokenWillExpire() override;
    void onPlayerSrcInfoChanged(const media::base::SrcInfo& from, const media::base::SrcInfo& to) override;
    void onPlayerInfoUpdated(const media::base::PlayerUpdatedInfo& info) override;
    void onAudioVolumeIndication(int volume) override;
    
};

