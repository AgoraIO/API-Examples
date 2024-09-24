#include "AbstractMediaPlayerSourceObserver.h"

void AbstractMediaPlayerSourceObserver::onPlayerSourceStateChanged(media::base::MEDIA_PLAYER_STATE state, media::base::MEDIA_PLAYER_REASON reason)
{
}

void AbstractMediaPlayerSourceObserver::onPositionChanged(int64_t positionMs, int64_t timestampMs)
{
}

void AbstractMediaPlayerSourceObserver::onPlayerEvent(media::base::MEDIA_PLAYER_EVENT eventCode, int64_t elapsedTime, const char *message)
{
}

void AbstractMediaPlayerSourceObserver::onMetaData(const void *data, int length)
{
}

void AbstractMediaPlayerSourceObserver::onPlayBufferUpdated(int64_t playCachedBuffer)
{
}

void AbstractMediaPlayerSourceObserver::onPreloadEvent(const char *src, media::base::PLAYER_PRELOAD_EVENT event)
{
}

void AbstractMediaPlayerSourceObserver::onCompleted()
{
}

void AbstractMediaPlayerSourceObserver::onAgoraCDNTokenWillExpire()
{
}

void AbstractMediaPlayerSourceObserver::onPlayerSrcInfoChanged(const media::base::SrcInfo &from, const media::base::SrcInfo &to)
{
}

void AbstractMediaPlayerSourceObserver::onPlayerInfoUpdated(const media::base::PlayerUpdatedInfo &info)
{
}

void AbstractMediaPlayerSourceObserver::onAudioVolumeIndication(int volume)
{
}
