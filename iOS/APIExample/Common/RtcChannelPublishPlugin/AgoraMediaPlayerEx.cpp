//
//  AgoraMediaPlayerEx.cpp
//  player_demo_apple
//
//  Created by zhanxiaochao on 2020/5/26.
//  Copyright © 2020 agora. All rights reserved.
//
#ifdef MEDIAPLAYER
#include "AgoraMediaPlayerEx.h"
#include "AudioFrameObserver.h"
#include <chrono>
#include <ctime>
using namespace agora::media::base;
using namespace agora::rtc;
using namespace std;
class AgoraMediaPlayerEx : public IAgoraMediaPlayerEx,public agora::media::base::IVideoFrameObserver,public agora::media::base::IAudioFrameObserver
{
public:
    ///get  ms timestamp
    int64_t GetHighAccuracyTickCount(){
        typedef chrono::time_point<chrono::system_clock, chrono::milliseconds> microClock_type;
        microClock_type tp = chrono::time_point_cast<chrono::milliseconds>(chrono::system_clock::now());
        return tp.time_since_epoch().count();
    }
    ///push videoFrame
    virtual void onFrame(const VideoFrame* frame){
        if (!is_push_video_) {
            return;
        }
        int size = frame->width * frame->height;
        uint8_t *tmp = (uint8_t *)malloc(size * 3/2);
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
        mediaEngine.queryInterface(rtcEngine_, agora::AGORA_IID_MEDIA_ENGINE);

        if (mediaEngine)
            mediaEngine->pushVideoFrame(&vframe);

        free(tmp);
    }
    ///pushAudioFrame
    virtual void onFrame(const AudioPcmFrame* frame){
        if (!is_push_audio_) {
            return;
        }
        audioFrameObserver_->pushData((char *)&frame->data_[0], (int)(frame->samples_per_channel_ * frame->bytes_per_sample));
    }
    virtual void detachPlayerFromRtc(){
        if (player_) {
            player_->unregisterPlayerObserver(observer_);
            player_->unregisterAudioFrameObserver(this);
            player_->unregisterVideoFrameObserver(this);
            observer_ = nullptr;
            player_ = nullptr;
        }
        
        
    }
    virtual void attachMediaPlayer(agora::rtc::IMediaPlayer *player,agora::rtc::IRtcEngine *rtcEngine)
    {
        audioFrameObserver_.reset(new AgoraAudioFrameObserver);
        rtcEngine_ = rtcEngine;
        rtcEngine_->setAudioProfile(AUDIO_PROFILE_MUSIC_STANDARD_STEREO, AUDIO_SCENARIO_CHATROOM_ENTERTAINMENT);
        rtcEngine_->setPlaybackAudioFrameParameters(48000, 2, RAW_AUDIO_FRAME_OP_MODE_READ_WRITE, 1920);
        rtcEngine_->setRecordingAudioFrameParameters(48000, 2, RAW_AUDIO_FRAME_OP_MODE_READ_WRITE, 1920);
        this->player_ = player;
        agora::util::AutoPtr<agora::media::IMediaEngine> mediaEngine;
        mediaEngine.queryInterface(rtcEngine, agora::AGORA_IID_MEDIA_ENGINE);
        if (mediaEngine) {
            mediaEngine->registerAudioFrameObserver(audioFrameObserver_.get());
            mediaEngine->setExternalVideoSource(true, false);
        }
        player_->registerAudioFrameObserver(this);
        player_->registerVideoFrameObserver(this);
        
    }
    virtual void publishAudio(){
        is_push_audio_ = true;
    }
    virtual void publishVideo(){
        is_push_video_ = true;
    }
    virtual void unpublishVideo(){
        is_push_video_ = false;
    }
    virtual void unpublishAudio(){
        is_push_audio_ = false;
    }
    virtual void registerMediaPlayerObserver(AgoraMediaPlayerObserver * observer){
        player_->registerPlayerObserver(observer);
    }
    virtual void adjustPlayoutSignalVolume(int volume){
        audioFrameObserver_->setPlayoutSignalVolume(volume);
    }
    virtual void adjustPublishSignalVolume(int volume){
        audioFrameObserver_->setPublishSignalVolume(volume);
    }
    ~AgoraMediaPlayerEx(){
        
    }
private:
    agora::rtc::IMediaPlayer *player_;
    std::unique_ptr<AgoraAudioFrameObserver> audioFrameObserver_;
    agora::rtc::IRtcEngine *rtcEngine_;
    std::atomic<bool> is_push_audio_{false};
    std::atomic<bool> is_push_video_{false};
    IMediaPlayerObserver *observer_;
};
IAgoraMediaPlayerEx *createAgoraMediaPlayerFactory(){
    return new AgoraMediaPlayerEx;
}
#endif
