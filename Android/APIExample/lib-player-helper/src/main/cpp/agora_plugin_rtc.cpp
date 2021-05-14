#include <jni.h>
#include <string>
#include <android/native_window_jni.h>
#include "IAgoraMediaEngine.h"
#include "IAgoraRtcEngine.h"
#include "AudioCircularBuffer.h"
#include <thread>
#include <iostream>

#include <stdio.h>
#include <stdlib.h>

#include <mutex>
#include "time.h"

#ifdef ANDROID
#include <android/log.h>
#define XLOGD(...) __android_log_print(ANDROID_LOG_DEBUG,"[player_native]",__VA_ARGS__)
#define XLOGI(...) __android_log_print(ANDROID_LOG_INFO,"[player_native]",__VA_ARGS__)
#define XLOGW(...) __android_log_print(ANDROID_LOG_WARN,"[player_native]",__VA_ARGS__)
#define XLOGE(...) __android_log_print(ANDROID_LOG_ERROR,"[player_native]",__VA_ARGS__)
#else
#include <stdio.h>
#define XLOGD(format, ...) printf("[player_native][DEBUG][%s][%d]: " format "\n", __FUNCTION__,\
                            __LINE__, ##__VA_ARGS__)
#define XLOGI(format, ...) printf("[player_native][INFO][%s][%d]: " format "\n", __FUNCTION__,\
                            __LINE__, ##__VA_ARGS__)
#define XLOGW(format, ...) printf("[player_native][WARN][%s][%d]: " format "\n", __FUNCTION__,\
                            __LINE__, ##__VA_ARGS__)
#define XLOGE(format, ...) printf("[player_native][ERROR][%s][%d]: " format "\n", __FUNCTION__,\
                            __LINE__, ##__VA_ARGS__)
#endif

//#define DUMP_AUDIO_DATA

using namespace AgoraRTC;
static scoped_ptr<AudioCircularBuffer<char>> agoraAudioBuf(new AudioCircularBuffer<char>(2048, true));
static scoped_ptr<AudioCircularBuffer<char>> playBackBuf(new AudioCircularBuffer<char>(2048, true));

static bool  is_init_new_agora_rtc_  = false;
static int totalTime= 0;
static JavaVM *gJVM = nullptr;
std::mutex recordMux;
std::mutex playoutMux;
static agora::rtc::IRtcEngine *rtcEngine = nullptr;
static agora::media::IMediaEngine* mediaEngine_ = nullptr;


static FILE *g_file_handle = NULL;

extern "C"
JNIEXPORT
jint JNI_OnLoad(JavaVM *vm, void *res) {
    XLOGD("JNI_OnLoad plugin_rtc");
    return JNI_VERSION_1_4;
}

void Sleep(int mis){
    std::chrono::milliseconds du(mis);
    std::this_thread::sleep_for(du);
}

struct AudioRawFrame {
    void * samples;
    size_t sampleRataHz;
    size_t bytesPerSample;
    size_t channelNums;
    size_t samplesPerChannel;
    size_t timestamp;
};

void destroyAudioBuffer() {
    recordMux.lock();
    agoraAudioBuf.release();
    agoraAudioBuf.reset(new AudioCircularBuffer<char>(2048,true));
    recordMux.unlock();
    playoutMux.lock();
    playBackBuf.release();
    playBackBuf.reset(new AudioCircularBuffer<char>(2048,true));
    playoutMux.unlock();
}


class IAudioFrameObserver;

class AgoraAudioFrameObserver : public agora::media::IAudioFrameObserver {



public:
    AgoraAudioFrameObserver() {
        audio_sonSum = 1.00f;
        audio_voiceSum = 1.00f;
        audio_local_sonSum = 1.00f;
        enable_local_playout_volume = false;
    }

    ~AgoraAudioFrameObserver() {
    }



public:
    double  audio_sonSum;
    double  audio_voiceSum;
    double  audio_local_sonSum;
    bool    enable_local_playout_volume;
    bool    enable_push_playout_volume;
    void pushAudioData(AudioRawFrame audioFrame){

//        if(!readPcmPointer) {
//            char *path = "/sdcard/file_in_7.pcm";
//            readPcmPointer = fopen(path, "wb");
//        }
//        fwrite(data.data, 2, data.size, readPcmPointer);

        // 采集端的音频
        int size = audioFrame.samplesPerChannel*audioFrame.bytesPerSample;
        char *audioBuf = (char *)malloc(sizeof(char)* size);
/*        XLOGI("tjy pushAudioData: push address %d push data1 %d %d %d %d %d %d %d %d %d %d ",data.data,
              data.data[0],data.data[1],data.data[2],data.data[3],data.data[4],
              data.data[5],data.data[6],data.data[7],data.data[8],data.data[9]);*/
        memcpy(audioBuf, audioFrame.samples, size);
//        audioPool.add_elements(audioBuf, (int)data.size);
        if (enable_push_playout_volume) {
            recordMux.lock();
            agoraAudioBuf->Push(audioBuf, size);
            recordMux.unlock();
        }
        if (enable_local_playout_volume){
            playoutMux.lock();
            playBackBuf->Push(audioBuf, size);
            playoutMux.unlock();
        }
        delete(audioBuf);

    }


    virtual bool onRecordAudioFrame(AudioFrame &audioFrame) override {
       if (!enable_push_playout_volume){
           return true;
       }
        size_t bytes = audioFrame.samples * audioFrame.bytesPerSample * audioFrame.channels;
        //XLOGI("tjy onRecordAudioFrame want bytes: %d,%d,%d,%d,%d  available bytes: %d",
         //   audioFrame.bytesPerSample,audioFrame.channels,audioFrame.samples,audioFrame.samplesPerSec,bytes,agoraAudioBuf->mAvailSamples);
        if (agoraAudioBuf->mAvailSamples < bytes) {
            return true;
        }

        XLOGI("tjy onRecordAudioFrame got audio frame %f,%f",audio_sonSum,audio_voiceSum);
        char *data = (char *) malloc(sizeof(short) * bytes);
        recordMux.lock();
        agoraAudioBuf->Pop(data, (int)bytes);
        recordMux.unlock();
        short *mixedBuffer = (short *)data;
        short *tmpBuf = (short *)malloc((u_int16_t)bytes);

#ifdef DUMP_AUDIO_DATA
        FILE* fp_ = fopen("/sdcard/test_record_1_cpp.pcm", "ab+");
        fwrite(audioFrame.buffer, 1, bytes, fp_);
        fclose(fp_);
#endif
        memcpy(tmpBuf, audioFrame.buffer, bytes);
        ///***
        for (int i = 0 ; i < (short)bytes/2; i++) {
            int tmp = (short)(((double)1.00f * mixedBuffer[i]) * audio_sonSum);
            tmpBuf[i]  = (short)(((double)1.00f * tmpBuf[i]) * audio_voiceSum);
            tmp += tmpBuf[i];

            if (tmp > 32767) {
                tmpBuf[i] = 32767;
            } else if (tmp < -32768) {
                tmpBuf[i] = -32768;
            } else {
                tmpBuf[i] = tmp;
            }
        }
        //***/
        memcpy(audioFrame.buffer, tmpBuf,bytes);

#ifdef DUMP_AUDIO_DATA
        FILE* fp_2 = fopen("/sdcard/test_record_2_cpp.pcm", "ab+");
        fwrite(audioFrame.buffer, 1, bytes, fp_2);
        fclose(fp_2);
#endif

        free(mixedBuffer);
        free(tmpBuf);
        return true;
    }

    virtual bool onPlaybackAudioFrame(AudioFrame &audioFrame) override {
       if (!enable_local_playout_volume){
           return true;
       }
       size_t bytes = audioFrame.samples * audioFrame.bytesPerSample * audioFrame.channels;
       if (playBackBuf->mAvailSamples < bytes) {
           return true;
       }
       //XLOGI("tjy onPlaybackAudioFrame want bytes: %d,%d,%d,%d,%d  available bytes: %d",
       //    audioFrame.bytesPerSample,audioFrame.channels,audioFrame.samples,audioFrame.samplesPerSec,bytes,agoraAudioBuf->mAvailSamples);

       char *data = (char *) malloc(sizeof(short) * bytes);
       playoutMux.lock();
       playBackBuf->Pop(data, (int)bytes);
       playoutMux.unlock();
       short *mixedBuffer = (short *)data;
       short *tmpBuf = (short *)malloc((u_int16_t)bytes);

#ifdef DUMP_AUDIO_DATA
       FILE* fp_ = fopen("/sdcard/test_playout_1_cpp.pcm", "ab+");
       fwrite(audioFrame.buffer, 1, bytes, fp_);
       fclose(fp_);
#endif
       memcpy(tmpBuf, audioFrame.buffer, bytes);
       ///***
       for (int i = 0 ; i < (short)bytes/2; i++) {
           int tmp = (short)(((double)1.00f * mixedBuffer[i]) * audio_local_sonSum);
           tmp += tmpBuf[i];

           if (tmp > 32767) {
               tmpBuf[i] = 32767;
           } else if (tmp < -32768) {
               tmpBuf[i] = -32768;
           } else {
               tmpBuf[i] = tmp;
           }
       }
       //***/
       memcpy(audioFrame.buffer, tmpBuf,bytes);

#ifdef DUMP_AUDIO_DATA
       FILE* fp_2 = fopen("/sdcard/test_playout_2_cpp.pcm", "ab+");
       fwrite(audioFrame.buffer, 1, bytes, fp_2);
       fclose(fp_2);
#endif

       free(mixedBuffer);
       free(tmpBuf);
        return true;
    }

    virtual bool onPlaybackAudioFrameBeforeMixing(unsigned int uid, AudioFrame &audioFrame) override {
               //XLOGI("tjy onPlaybackAudioFrameBeforeMixing");
        return true;
    }

    virtual bool onMixedAudioFrame(AudioFrame &audioFrame) override {
           //XLOGI("tjy onMixedAudioFrame");
        return true;
    }
};


static AgoraAudioFrameObserver s_audioFrameObserver;

#ifdef __cplusplus
extern "C" {
#endif

int __attribute__((visibility("default")))
loadAgoraRtcEnginePlugin(agora::rtc::IRtcEngine *engine) {
    XLOGI("TJY loadAgoraRtcEnginePlugin--------- ");
    rtcEngine = engine;
    return 0;
}

void __attribute__((visibility("default")))
unloadAgoraRtcEnginePlugin(agora::rtc::IRtcEngine *engine) {
    XLOGI("TJY unloadAgoraRtcEnginePlugin--------- ");
    if(mediaEngine_){
        mediaEngine_->registerAudioFrameObserver(nullptr);
    }
    is_init_new_agora_rtc_ = false;
    rtcEngine = nullptr;
    mediaEngine_ = nullptr;
}
}

extern "C"
JNIEXPORT void JNICALL
Java_io_agora_ktvkit_KTVKit_destroyAudioBuf(JNIEnv *env, jobject instance) {
    agoraAudioBuf.release();
    playBackBuf.release();
    agoraAudioBuf.reset(new AudioCircularBuffer<char>(2048,true));
    playBackBuf.reset(new AudioCircularBuffer<char>(2048,true));
}



extern "C"
JNIEXPORT void JNICALL
Java_io_agora_RtcChannelPublishHelper_nativeOnAudioData(JNIEnv *env, jobject instance,
            jobject audio_buffer, jint sampleRataHz, jint bytesPerSample, jint channelNums, jint samplesPerChannel, jlong timestamp) {
    if(!s_audioFrameObserver.enable_push_playout_volume && !s_audioFrameObserver.enable_local_playout_volume) {
        return;
    }
    //XLOGI("tjy jni nativeOnAudioData %d,%d,%d,%d",bytesPerSample,channelNums,samplesPerChannel,bytesPerSample*channelNums*samplesPerChannel);
    void * direct_audio_buffer = env->GetDirectBufferAddress(audio_buffer);
    AudioRawFrame audio_frame;
    audio_frame.samples = direct_audio_buffer;
    audio_frame.sampleRataHz = sampleRataHz;
    audio_frame.bytesPerSample = bytesPerSample;
    audio_frame.channelNums = channelNums;
    audio_frame.samplesPerChannel = samplesPerChannel;
    audio_frame.timestamp = timestamp;
    s_audioFrameObserver.pushAudioData(audio_frame);
}

extern "C"
JNIEXPORT void JNICALL
Java_io_agora_RtcChannelPublishHelper_nativeEnablePushToRtc(JNIEnv *env, jobject instance) {
    XLOGI("TJY Java_io_agora_RtcChannelPublishHelper_nativeEnablePushToRtc %d",is_init_new_agora_rtc_);
    if (!is_init_new_agora_rtc_) {
        is_init_new_agora_rtc_ = true;
        rtcEngine->queryInterface(agora::AGORA_IID_MEDIA_ENGINE,
                            reinterpret_cast<void**>(&mediaEngine_));
        XLOGI("TJY mediaEngine_ init %X",mediaEngine_);
        if (mediaEngine_) {
            mediaEngine_->registerAudioFrameObserver(&s_audioFrameObserver);
            if (rtcEngine) {
               rtcEngine->setRecordingAudioFrameParameters(48000,2,agora::rtc::RAW_AUDIO_FRAME_OP_MODE_READ_WRITE,1024);
               rtcEngine->setPlaybackAudioFrameParameters(48000,2,agora::rtc::RAW_AUDIO_FRAME_OP_MODE_READ_WRITE,1024);
                //agora::rtc::RtcEngineParameters *param = new agora::rtc::RtcEngineParameters(rtcEngine);
                //param->setRecordingAudioFrameParameters(32000,1,agora::rtc::RAW_AUDIO_FRAME_OP_MODE_READ_WRITE,1024);
                //param->setPlaybackAudioFrameParameters(32000,1,agora::rtc::RAW_AUDIO_FRAME_OP_MODE_READ_WRITE,1024);

                //delete param;
            }

           XLOGI("TJY jni nativeEnablePushToRtc mediaEngine ok");
        } else {
           XLOGE("TJY jni nativeEnablePushToRtc mediaEngine init error");
        }
    }
}


extern "C"
JNIEXPORT void JNICALL
Java_io_agora_RtcChannelPublishHelper_adjustPublishSignalVolume(JNIEnv *env, jobject instance,jfloat volume) {
    XLOGI("jni nativeMovieVolume %f",volume);
    s_audioFrameObserver.audio_sonSum = volume;
}

extern "C"
JNIEXPORT void JNICALL
Java_io_agora_RtcChannelPublishHelper_adjustPublishVoiceVolume(JNIEnv *env, jobject instance,jfloat volume) {
    XLOGI("TJY nativeVoiceVolume %f",volume);
    s_audioFrameObserver.audio_voiceSum = volume;
}

extern "C"
JNIEXPORT void JNICALL
Java_io_agora_RtcChannelPublishHelper_nativeEnablePushAudioToRtc(JNIEnv *env, jobject instance,jboolean enable) {
    XLOGI("TJY nativeEnablePushAudioToRtc %d %p",enable,rtcEngine);
    s_audioFrameObserver.enable_push_playout_volume = enable;
    if(!enable) {
        destroyAudioBuffer();
    }
    if (rtcEngine && enable) {
        //rtcEngine->setRecordingAudioFrameParameters(32000,1,agora::rtc::RAW_AUDIO_FRAME_OP_MODE_READ_WRITE,1024);
        //rtcEngine->setPlaybackAudioFrameParameters(32000,1,agora::rtc::RAW_AUDIO_FRAME_OP_MODE_READ_WRITE,1024);
        //agora::rtc::RtcEngineParameters *param = new agora::rtc::RtcEngineParameters(rtcEngine);
        //param->setRecordingAudioFrameParameters(32000,1,agora::rtc::RAW_AUDIO_FRAME_OP_MODE_READ_WRITE,1024);
        //param->setPlaybackAudioFrameParameters(32000,1,agora::rtc::RAW_AUDIO_FRAME_OP_MODE_READ_WRITE,1024);

        //delete param;
       XLOGI("TJY jni nativeEnablePushAudioToRtc mediaEngine ok");
    } else {
       XLOGE("TJY jni nativeEnablePushAudioToRtc mediaEngine init error");
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_io_agora_RtcChannelPublishHelper_nativeEnableLocalPlayoutVolume(JNIEnv *env, jobject instance,jboolean enable) {
    XLOGI("TJY nativeEnableLocalPlayoutVolume %d",enable);
    s_audioFrameObserver.enable_local_playout_volume = enable;
}

extern "C"
JNIEXPORT void JNICALL
Java_io_agora_RtcChannelPublishHelper_nativeAdjustPublishLocalVoiceVolume(JNIEnv *env, jobject instance,jfloat volume) {
XLOGI("jni adjustPublishLocalSignalVolume %f",volume);
s_audioFrameObserver.audio_local_sonSum = volume;
}

extern "C"
JNIEXPORT void JNICALL
Java_io_agora_RtcChannelPublishHelper_nativeRelease(JNIEnv *env, jobject instance) {
    XLOGI("TJY nativeRelease");
    if (is_init_new_agora_rtc_) {
        is_init_new_agora_rtc_ = false;
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_io_agora_RtcChannelPublishHelper_nativeUnregisterAudioFrameObserver(JNIEnv *env, jobject instance){
    XLOGI("TJY nativeUnregisterAudioFrameObserver");
    if(mediaEngine_){
        mediaEngine_->registerAudioFrameObserver(nullptr);
    }
    if (is_init_new_agora_rtc_) {
        is_init_new_agora_rtc_ = false;
    }
}
