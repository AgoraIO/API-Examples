#include "ExtendAudioFrameObserver.h"
#include <stdio.h>

CMeidaPlayerAudioFrameObserver::CMeidaPlayerAudioFrameObserver():agoraAudioBuf(new AudioCircularBuffer<char>(2048,true)), play_back_audio_circular_buffer_(new AudioCircularBuffer<char>(2048, true))
{
	
}
CMeidaPlayerAudioFrameObserver::~CMeidaPlayerAudioFrameObserver()
{

}
bool CMeidaPlayerAudioFrameObserver::onRecordAudioFrame(AudioFrame& audioFrame){

    int bytes = audioFrame.samples * audioFrame.channels * audioFrame.bytesPerSample;
    int16_t *tmpBuf = (int16_t *)malloc(bytes);
    memcpy(tmpBuf, audioFrame.buffer, bytes);

    if (agoraAudioBuf->mAvailSamples < bytes) {
        free(tmpBuf);
        return false;
    }
    mtx.lock();
    int ret = agoraAudioBuf->mAvailSamples - bytes;
    if (ret < 0){
        memcpy(audioFrame.buffer, tmpBuf, bytes);
        free(tmpBuf);
        mtx.unlock();
        return false;
    }
    char *data = (char *)malloc(sizeof(char)*bytes);
    //FILE *fp = NULL;
    agoraAudioBuf->Pop(data, bytes);
    //fp=fopen("C:/Users/zhanxiaochao/Desktop/out.pcm","ab+");

  //  if(fp){
  //      fwrite(data,1,mv_size,fp);
		//fclose(fp);
  //  }
    int16_t* p16 = (int16_t*)data;
    int16_t *audioBuf = (int16_t *)malloc(bytes);
    memcpy(audioBuf, tmpBuf, bytes);
    for (int i = 0; i < bytes / 2; ++i) {
        int tmp = p16[i] * remote_audio_volume_;
        audioBuf[i] = audioBuf[i] * audioMixVolume;
        tmp += audioBuf[i];

        if (tmp > 32767) {
            audioBuf[i] = 32767;
        }
        else if (tmp < -32768) {
            audioBuf[i] = -32768;
        }
        else {
            audioBuf[i] += tmp;
        }
    }
    memcpy(audioFrame.buffer, audioBuf,  bytes);
    free(audioBuf);
    free(tmpBuf);
    free(p16);
    mtx.unlock();
    return true;
}
bool CMeidaPlayerAudioFrameObserver::onPlaybackAudioFrame(AudioFrame& audioFrame){
	int bytes = audioFrame.samples * audioFrame.channels * audioFrame.bytesPerSample;
	int16_t *tmpBuf = (int16_t *)malloc(bytes);
	memcpy(tmpBuf, audioFrame.buffer, bytes);

	if (play_back_audio_circular_buffer_->mAvailSamples < bytes) {
		memcpy(audioFrame.buffer, tmpBuf, bytes);
		free(tmpBuf);
		return true;
	}
	std::lock_guard<std::mutex> _(mtx_);
	int ret = play_back_audio_circular_buffer_->mAvailSamples - bytes;
	if (ret < 0) {
		memcpy(audioFrame.buffer, tmpBuf, bytes);
		free(tmpBuf);
		return true;
	}
	char *data = (char *)malloc(sizeof(char)*bytes);
	//FILE *fp = NULL;
	play_back_audio_circular_buffer_->Pop(data, bytes);
	//fp=fopen("C:/Users/zhanxiaochao/Desktop/out.pcm","ab+");

  //  if(fp){
  //      fwrite(data,1,mv_size,fp);
		//fclose(fp);
  //  }
	int16_t* p16 = (int16_t*)data;
	int16_t *audioBuf = (int16_t *)malloc(bytes);
	memcpy(audioBuf, tmpBuf, bytes);
	for (int i = 0; i < bytes / 2; ++i) {
		int tmp = p16[i] * playout_volume_;
		audioBuf[i] = audioBuf[i] * 1;
		tmp += audioBuf[i];

		if (tmp > 32767) {
			audioBuf[i] = 32767;
		}
		else if (tmp < -32768) {
			audioBuf[i] = -32768;
		}
		else {
			audioBuf[i] = tmp;
		}
	}
	memcpy(audioFrame.buffer, audioBuf, bytes);
	free(audioBuf);
	free(tmpBuf);
	free(p16);
    return true;
}
bool CMeidaPlayerAudioFrameObserver::onPlaybackAudioFrameBeforeMixing(unsigned int uid, AudioFrame& audioFrame){

    return true;
}
bool  CMeidaPlayerAudioFrameObserver::onMixedAudioFrame(AudioFrame& audioFrame){
    return true;
}
void CMeidaPlayerAudioFrameObserver::pushAudioData(void *data,int len){

	mtx.lock();
	agoraAudioBuf->Push((char *)data, len);
	mtx.unlock();
	std::lock_guard<std::mutex> _(mtx_);
	play_back_audio_circular_buffer_->Push((char *)data, len);
	
}
void CMeidaPlayerAudioFrameObserver::reset(){
    agoraAudioBuf.reset(new AudioCircularBuffer<char>(2048, true));
	play_back_audio_circular_buffer_.reset(new AudioCircularBuffer<char>(2048, true));
}
void CMeidaPlayerAudioFrameObserver::setAudioMixing(bool isAudioMix){
    if(isAudioMix){
        audioMixVolume = 0;
    }else{
        audioMixVolume = 1;
    }
}
void CMeidaPlayerAudioFrameObserver::setRemoteVolume(int volume) {
	mtx.lock();
	if (volume < 0)
	{
		mtx.unlock();
		return;
	}
	remote_audio_volume_.store(volume/100.0f);
	mtx.unlock();
}
void CMeidaPlayerAudioFrameObserver::setPlayoutSignalVolume(int volume)
{
	mtx.lock();
	if (volume < 0)
	{
		mtx.unlock();
		return;
	}
	playout_volume_ = volume / 100.0f;
	mtx.unlock();
}
