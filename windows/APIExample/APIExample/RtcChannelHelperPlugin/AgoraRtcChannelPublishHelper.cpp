#include "AgoraRtcChannelPublishHelper.h"
#include "utils/ExtendAudioFrameObserver.h"
#include <IAgoraRtcEngine.h>
#include <IAgoraMediaEngine.h>
#include <IAgoraMediaPlayer.h>
class MediaPlayerFrameObserver :
	public agora::media::base::IAudioFrameObserver,
    public agora::media::base::IVideoFrameObserver
{
public:
	agora::rtc::IRtcEngine *rtc_engine;
	AgoraRtcChannelPublishHelper *helper;
	std::atomic<bool> is_push_videoFrame_{false};
	std::atomic<bool> is_push_audioFrame_{ true };
	//timeStamp
	static UINT64 WINAPI GetHighAccuracyTickCount()
	{
		LARGE_INTEGER	liPC;
		LARGE_INTEGER	liPF;

		::QueryPerformanceCounter(&liPC);
		::QueryPerformanceFrequency(&liPF);

		return static_cast<UINT64>(liPC.QuadPart * 1000 / liPF.QuadPart);
	}
	virtual void onFrame(const agora::media::base::AudioPcmFrame* frame)
	{
		if(!helper || !is_push_audioFrame_ || !helper->extend_audio_frame_observer_)return;
		    helper->extend_audio_frame_observer_->pushAudioData((void *)(&frame->data_[0]), frame->bytes_per_sample * frame->samples_per_channel_);
	};
	virtual void onFrame(const agora::media::base::VideoFrame* frame)
	{
		//set externalVideoSource
		
		int size = frame->width * frame->height;
		BYTE *tmp = (BYTE *)malloc(size * 3/2);
		if (!rtc_engine || !is_push_videoFrame_)
		{
			free(tmp);
			return;
		}
		memcpy(tmp, frame->yBuffer, size);
		memcpy(tmp + size, frame->uBuffer, size >> 2);
		memcpy(tmp+ size + frame->width * frame->height/4, frame->vBuffer, size >> 2);
		agora::media::ExternalVideoFrame vframe;
		vframe.stride = frame->yStride;
		vframe.height = frame->height;
		vframe.timestamp = static_cast<long long>(GetHighAccuracyTickCount());
		vframe.rotation = 0;
		vframe.type = agora::media::ExternalVideoFrame::VIDEO_BUFFER_TYPE::VIDEO_BUFFER_RAW_DATA;
		vframe.format = agora::media::ExternalVideoFrame::VIDEO_PIXEL_FORMAT::VIDEO_PIXEL_I420;
		vframe.cropLeft = 0;
		vframe.cropTop = 0;
		vframe.cropBottom = 0;
		vframe.cropRight = 0;
		vframe.buffer = tmp;
		agora::util::AutoPtr<agora::media::IMediaEngine> mediaEngine;
		mediaEngine.queryInterface(rtc_engine, agora::AGORA_IID_MEDIA_ENGINE);

		if (mediaEngine)
			mediaEngine->pushVideoFrame(&vframe);

		free(tmp);
		
	}
	virtual ~MediaPlayerFrameObserver()
	{
		
	}
};

AgoraRtcChannelPublishHelper::AgoraRtcChannelPublishHelper()
{
}


AgoraRtcChannelPublishHelper::~AgoraRtcChannelPublishHelper()
{
	
}
void AgoraRtcChannelPublishHelper::attachPlayerToRtc(agora::rtc::IRtcEngine* engine, agora::rtc::IMediaPlayer* media_player)
{
	rtc_engine_ = engine;
	media_player_ = media_player;
	media_player_frame_observer_.reset(new MediaPlayerFrameObserver);
	extend_audio_frame_observer_.reset(new CMeidaPlayerAudioFrameObserver);
	media_player_->registerAudioFrameObserver(media_player_frame_observer_.get());
	media_player_->registerVideoFrameObserver(media_player_frame_observer_.get());
	extend_audio_frame_observer_->setRemoteVolume(0);
	media_player_frame_observer_->rtc_engine = engine;
	media_player_frame_observer_->helper = this;
	rtc_engine_->setLocalVideoMirrorMode(agora::rtc::VIDEO_MIRROR_MODE_DISABLED);
	rtc_engine_->setAudioProfile(agora::rtc::AUDIO_PROFILE_MUSIC_HIGH_QUALITY_STEREO, agora::rtc::AUDIO_SCENARIO_CHATROOM_ENTERTAINMENT);
	rtc_engine_->setRecordingAudioFrameParameters(48000, 2, agora::rtc::RAW_AUDIO_FRAME_OP_MODE_READ_WRITE, 960);
	rtc_engine_->setPlaybackAudioFrameParameters(48000, 2, agora::rtc::RAW_AUDIO_FRAME_OP_MODE_READ_WRITE, 960);
	if (rtc_engine_) {
		agora::util::AutoPtr<agora::media::IMediaEngine> mediaEngine;
		mediaEngine.queryInterface(rtc_engine_, agora::AGORA_IID_MEDIA_ENGINE);
		if (mediaEngine)
		{
			mediaEngine->setExternalVideoSource(true, false);
		}
		if (mediaEngine)
		{
			mediaEngine->registerAudioFrameObserver(extend_audio_frame_observer_.get());
		}
	}

	
}
void AgoraRtcChannelPublishHelper::adjustPublishSignalVolume(int volume)
{
	if (extend_audio_frame_observer_)
	{
		extend_audio_frame_observer_->setRemoteVolume(volume);
	}
}
void AgoraRtcChannelPublishHelper::registerAgoraRtcChannelPublishHelperObserver(AgoraRtcChannelPublishHelperObserver* observer)
{
	media_player_observer_ = observer;
	media_player_->registerPlayerObserver(media_player_observer_);
}
void AgoraRtcChannelPublishHelper::adjustPlayoutSignalVolume(int volume)
{
	if (extend_audio_frame_observer_)
	{
		extend_audio_frame_observer_->setPlayoutSignalVolume(volume);
	}
}

void AgoraRtcChannelPublishHelper::publishAudio()
{
	if(extend_audio_frame_observer_)
		extend_audio_frame_observer_->setRemoteVolume(100);

}
void AgoraRtcChannelPublishHelper::unpublishAudio()
{
	if(extend_audio_frame_observer_)
		extend_audio_frame_observer_->setRemoteVolume(0);
}
void AgoraRtcChannelPublishHelper::publishVideo()
{
	if (media_player_frame_observer_)
		media_player_frame_observer_->is_push_videoFrame_.store(true);
}
void AgoraRtcChannelPublishHelper::unpublishVideo()
{
	if (media_player_frame_observer_)
		media_player_frame_observer_->is_push_videoFrame_.store(false);
}
void AgoraRtcChannelPublishHelper::detachPlayerFromRtc()
{
	if (media_player_frame_observer_)
	{
	
		media_player_frame_observer_->is_push_videoFrame_.store(false);
		media_player_frame_observer_->is_push_audioFrame_.store(false);
		
	}
	if (media_player_)
	{
		media_player_->unregisterAudioFrameObserver(media_player_frame_observer_.get());
		media_player_->unregisterVideoFrameObserver(media_player_frame_observer_.get());
		media_player_->unregisterPlayerObserver(media_player_observer_);
		media_player_ = nullptr;
	}
	if (rtc_engine_) {
		agora::util::AutoPtr<agora::media::IMediaEngine> mediaEngine;
		mediaEngine.queryInterface(rtc_engine_, agora::AGORA_IID_MEDIA_ENGINE);
		if (mediaEngine)
		{
			mediaEngine->registerAudioFrameObserver(nullptr);
		}
		rtc_engine_ = nullptr;
	}

}


