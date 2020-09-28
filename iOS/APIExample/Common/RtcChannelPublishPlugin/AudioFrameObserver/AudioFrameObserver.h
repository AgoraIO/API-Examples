//
//  AudioFrameObserver.hpp
//  player_demo_apple
//
//  Created by zhanxiaochao on 2020/5/27.
//  Copyright © 2020 agora. All rights reserved.
//

#ifndef AudioFrameObserver_h
#define AudioFrameObserver_h
#include <stdio.h>
#import <AgoraRtcKit/IAgoraRtcEngine.h>
#import <AgoraRtcKit/IAgoraMediaEngine.h>
#import "AudioCircularBuffer.h"
#import "scoped_ptr.h"
using namespace AgoraRTC;
class AgoraAudioFrameObserver:public agora::media::IAudioFrameObserver
{
public:
    std::atomic<float>  publishSignalValue_{1.0f};
    std::atomic<float>  playOutSignalValue_{1.0f};
    scoped_ptr<AudioCircularBuffer<char>> agoraAudioBuf;
    scoped_ptr<AudioCircularBuffer<char>> agoraPlayoutBuf;
    AgoraAudioFrameObserver(){
        agoraAudioBuf.reset(new AudioCircularBuffer<char>(2048,true));
        agoraPlayoutBuf.reset(new AudioCircularBuffer<char>(2048,true));
    }
    void setPublishSignalVolume(int volume){
            publishSignalValue_ = volume/100.0f;
    }
    void setPlayoutSignalVolume(int volume){
            playOutSignalValue_ = volume/100.0f;
     }
    void pushData(char *data,int length){
           agoraAudioBuf->Push(data, length);
           agoraPlayoutBuf->Push(data, length);

    }
    void resetAudioBuf(){
        agoraAudioBuf.reset(new AudioCircularBuffer<char>(2048,true));
        agoraPlayoutBuf.reset(new AudioCircularBuffer<char>(2048,true));
    }
    virtual bool onRecordAudioFrame(AudioFrame& audioFrame){

                int bytes = audioFrame.samples * audioFrame.channels * audioFrame.bytesPerSample;
                int16_t *tmpBuf = (int16_t *)malloc(sizeof(int16_t)*bytes);
                memcpy(tmpBuf, audioFrame.buffer, bytes);
                if (agoraAudioBuf->mAvailSamples < bytes) {
                    memcpy(audioFrame.buffer, tmpBuf, sizeof(int16_t)*bytes);
                    free(tmpBuf);
                    return true;
                }
                //计算重采样钱的数据大小 重采样的采样率 * SDK回调时间 * 声道数 * 字节数
                int mv_size = bytes;
                char *data = (char *)malloc(sizeof(char)*mv_size);
                agoraAudioBuf->Pop(data, mv_size);
                int16_t* p16 = (int16_t*) data;
                int16_t *audioBuf = (int16_t *)malloc(bytes);
                memcpy(audioBuf, tmpBuf, bytes);
                for (int i = 0; i < bytes / 2; ++i) {
                    tmpBuf[i] += (p16[i] * publishSignalValue_);
                    //audio overflow
                    if (tmpBuf[i] > 32767) {
                        audioBuf[i] = 32767;
                    }
                    else if (tmpBuf[i] < -32768) {
                        audioBuf[i] = -32768;
                    }
                    else {
                        audioBuf[i] = tmpBuf[i];
                    }
                }
                memcpy(audioFrame.buffer, audioBuf,bytes);
                free(audioBuf);
                free(tmpBuf);
                free(p16);
        return true;
    }
    /**
     * Occurs when the playback audio frame is received.
     * @param audioframe The reference to the audio frame: AudioFrame.
     * @return
     * - true: The playback audio frame is valid and is encoded and sent.
     * - false: The playback audio frame is invalid and is not encoded or sent.
     */
    virtual bool onPlaybackAudioFrame(AudioFrame& audioFrame){
        int bytes = audioFrame.samples * audioFrame.channels * audioFrame.bytesPerSample;
        int16_t *tmpBuf = (int16_t *)malloc(bytes);
        memcpy(tmpBuf, audioFrame.buffer, bytes);
        if (agoraPlayoutBuf->mAvailSamples < bytes) {
            memcpy(audioFrame.buffer, tmpBuf,bytes);
            free(tmpBuf);
            return true;
        }
        //计算重采样钱的数据大小 重采样的采样率 * SDK回调时间 * 声道数 * 字节数
        int mv_size = bytes;
        char *data = (char *)malloc(mv_size);
        agoraPlayoutBuf->Pop(data, mv_size);
        int16_t* p16 = (int16_t*) data;
        int16_t *audioBuf = (int16_t *)malloc(bytes);
        memcpy(audioBuf, tmpBuf, bytes);
        for (int i = 0; i < bytes / 2; ++i) {
            tmpBuf[i] += (p16[i] * playOutSignalValue_);
            //audio overflow
            if (tmpBuf[i] > 32767) {
                audioBuf[i] = 32767;
            }
            else if (tmpBuf[i] < -32768) {
                audioBuf[i] = -32768;
            }
            else {
                audioBuf[i] = tmpBuf[i];
            }
        }
        memcpy(audioFrame.buffer, audioBuf,bytes);
        free(audioBuf);
        free(tmpBuf);
        free(p16);
        return true;
    }
    /**
     * Occurs when the mixed audio data is received.
     * @param audioframe The reference to the audio frame: AudioFrame.
     * @return
     * - true: The mixed audio data is valid and is encoded and sent.
     * - false: The mixed audio data is invalid and is not encoded or sent.
     */
    virtual bool onMixedAudioFrame(AudioFrame& audioFrame){
        return false;
    }
    /**
     * Occurs when the playback audio frame before mixing is received.
     * @param audioframe The reference to the audio frame: AudioFrame.
     * @return
     * - true: The playback audio frame before mixing is valid and is encoded and sent.
     * - false: The playback audio frame before mixing is invalid and is not encoded or sent.
     */
    virtual bool onPlaybackAudioFrameBeforeMixing(unsigned int uid, AudioFrame& audioFrame){
        return false;
    }
};





#endif /* AudioFrameObserver_hpp */
