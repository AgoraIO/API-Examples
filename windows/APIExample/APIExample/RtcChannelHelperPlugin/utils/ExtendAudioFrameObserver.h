#pragma once
#include <atomic>
#include <IAgoraMediaEngine.h>
#include <iostream>
#include <mutex>
#include "AudioCircularBuffer.h"
#include "scoped_ptr.h"
#include <list>
using namespace AgoraRTC;
using namespace std;
class CMeidaPlayerAudioFrameObserver:public agora::media::IAudioFrameObserver
{
private:
	std::mutex mtx;
	std::mutex mtx_;
	std::list<char*> play_back_audio_cache_;
    float audioMixVolume = 1.0f;
	std::atomic<float> remote_audio_volume_{ 1.0f };
	std::atomic<float> playout_volume_{ 1.0f };
	virtual bool onRecordAudioFrame(AudioFrame& audioFrame);
	virtual bool onPlaybackAudioFrame(AudioFrame& audioFrame);
	virtual bool onMixedAudioFrame(AudioFrame& audioFrame);
	virtual bool onPlaybackAudioFrameBeforeMixing(unsigned int uid, AudioFrame& audioFrame);
	scoped_ptr<AudioCircularBuffer<char>> agoraAudioBuf;
	scoped_ptr<AudioCircularBuffer<char>> play_back_audio_circular_buffer_;

public:
	CMeidaPlayerAudioFrameObserver();
	~CMeidaPlayerAudioFrameObserver();
	int sample_rate;
	int re_sample_rate;
	int re_channels;
	void pushAudioData(void *data, int len);
	void reset();
    void setAudioMixing(bool isAudioMix);
	void setRemoteVolume(int volume);
	void setPlayoutSignalVolume(int volume);
};

