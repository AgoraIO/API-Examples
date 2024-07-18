//
// Created on 2024/6/16.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#pragma once

#include "AgoraMediaBase.h"
#include "audio_render.h"
#include "utils.h"

class OriginAudioDataObserver : public agora::media::IAudioFrameObserver {
public:
    OriginAudioDataObserver() {
        audioRender_ = new AudioRenderOHOS();
        audioRender_->init(AudioParameters(48000, 2));
        audioRender_->start();
    }
    ~OriginAudioDataObserver() {
        if (audioRender_ != nullptr) {
            audioRender_->stop();
            audioRender_->release();
            delete audioRender_;
            audioRender_ = nullptr;
        }
    }
    bool onEarMonitoringAudioFrame(AudioFrame &audioFrame) override { return false; };
    bool onPlaybackAudioFrameBeforeMixing(const char *channelId, agora::rtc::uid_t uid,
                                          AudioFrame &audioFrame) override {
        return false;
    }
    bool onMixedAudioFrame(const char *channelId, AudioFrame &audioFrame) override { return false; };
    bool onPlaybackAudioFrameBeforeMixing(const char *channelId, const char *userId, AudioFrame &audioFrame) override {
        return false;
    }
    bool onPlaybackAudioFrame(const char *channelId, AudioFrame &audioFrame) override {
        AG_DEBUG("onPlaybackAudioFrame");
        int audioFrameBufferSize = audioFrame.bytesPerSample * audioFrame.channels * audioFrame.samplesPerChannel;
        if (audioRender_ != nullptr) {
            //audioRender_->putAudioData(audioFrame.buffer, audioFrameBufferSize);
        }
        return false;
    }
    
    bool onRecordAudioFrame(const char *channelId, AudioFrame &audioFrame) override { return false; }
    int getObservedAudioFramePosition() override { return IAudioFrameObserverBase::AUDIO_FRAME_POSITION_PLAYBACK; }
    AudioParams getMixedAudioParams() override { return AudioParams(); }
    AudioParams getEarMonitoringAudioParams() override { return AudioParams(); }
    AudioParams getPlaybackAudioParams() override {
        return AudioParams(48000, 2, agora::rtc::RAW_AUDIO_FRAME_OP_MODE_READ_ONLY, 1024);
    }
    AudioParams getRecordAudioParams() override { return AudioParams(); }

private:
    AudioRenderOHOS *audioRender_ = nullptr;
};
