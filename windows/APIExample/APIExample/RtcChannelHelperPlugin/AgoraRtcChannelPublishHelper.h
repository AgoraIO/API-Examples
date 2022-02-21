#pragma once
#include <atomic>
#include <memory>
#include <IAgoraRtcEngine.h>
#include <IAgoraMediaPlayer.h>
#include <AgoraBase.h>
#include <iostream>
class MediaPlayerFrameObserver;
class CMeidaPlayerAudioFrameObserver;
class AgoraRtcChannelPublishHelperObserver : public agora::rtc::IMediaPlayerObserver
{
	;
	/**
	 * @brief Triggered when the player state changes
	 *
	 * @param state New player state
	 * @param ec Player error message
	 */
	virtual void onPlayerStateChanged(agora::media::base::MEDIA_PLAYER_STATE state,
		agora::media::base::MEDIA_PLAYER_ERROR ec)
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
	 * @param event
	 */
	virtual void onPlayerEvent(agora::media::base::MEDIA_PLAYER_EVENT event)
	{

	};

	/**
	 * @brief Triggered when metadata is obtained
	 *
	 * @param type Metadata type
	 * @param data data
	 * @param length  data length
	 */
	virtual void onMetadata(agora::media::base::MEDIA_PLAYER_METADATA_TYPE type, const uint8_t* data,
		uint32_t length)
	{

	}
};
class AgoraRtcChannelPublishHelper
{
public:
	
	std::unique_ptr<CMeidaPlayerAudioFrameObserver> extend_audio_frame_observer_;
	static AgoraRtcChannelPublishHelper *AgoraRtcChannelPublishHelper::Get()
	{
		static  AgoraRtcChannelPublishHelper  helper;
		return &helper;
	}
	void attachPlayerToRtc(agora::rtc::IRtcEngine *engine, agora::rtc::IMediaPlayer *media_player);
	void registerAgoraRtcChannelPublishHelperObserver(AgoraRtcChannelPublishHelperObserver *observer);
	// 启动/停止推送音频流到频道
	void publishAudio();
	void unpublishAudio();
	// 启动/停止推送视频流到频道
	void publishVideo();
	void unpublishVideo();
	// 调节推送到频道内音频流的音量
	void adjustPublishSignalVolume(int volume);
	// 调节本地播放视频音量
	void adjustPlayoutSignalVolume(int volume);
	// 断开 MediaPlayer 和 RTC SDK 的关联
	void detachPlayerFromRtc();
	AgoraRtcChannelPublishHelper();
	~AgoraRtcChannelPublishHelper();
private:
	std::unique_ptr<MediaPlayerFrameObserver> media_player_frame_observer_;
	agora::rtc::IRtcEngine *rtc_engine_;
	agora::rtc::IMediaPlayer *media_player_;
	agora::rtc::IMediaPlayerObserver *media_player_observer_;
};

