//
// Created on 2023/12/12.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#pragma once
#include "AgoraMediaBase.h"
#include "napi/native_api.h"
#include <ohaudio/native_audiorenderer.h>
#include <ohaudio/native_audiostreambuilder.h>
#include <mutex>
#include <memory>

class AudioParameters {
public:
  // This implementation does only support 16-bit PCM samples.
  static const size_t kBitsPerSample = 16;
  AudioParameters()
      : sample_rate_(0), channels_(0), audio_source_(0), frames_per_10ms_buffer_(0) {}
  AudioParameters(int sample_rate, size_t channels)
      : sample_rate_(sample_rate), channels_(channels), audio_source_(0),
        frames_per_10ms_buffer_(static_cast<size_t>(sample_rate / 100)) {}
  int sample_rate() const { return sample_rate_; }
  size_t channels() const { return channels_; }
  bool is_valid() const { return ((sample_rate_ > 0) && (channels_ > 0)); }
private:
  int sample_rate_;
  size_t channels_;
  size_t audio_source_;
  size_t frames_per_10ms_buffer_;
};


class SimpleCircularBuffer {
public: 
    SimpleCircularBuffer(uint32_t size): size(size),count(0),head(0),tail(0){
        buffer = (unsigned char*)malloc(size);
        memset(buffer,0,size);
    }
    int add(char* data,int length){
        int available = size - count;  
        int copied = length - available > 0? available:length;  
        if( copied == 0){  
            return copied;  
        }
        int tail_available = size - tail;
        std::lock_guard<std::mutex> l(lock_);
        if(tail_available >= copied){
            memcpy(buffer+tail,data,copied);
            tail += copied;
            if(tail == size){
                tail = 0;
            }
        } else {
            memcpy(buffer+tail,data,tail_available);
            memcpy(buffer,data+tail_available, copied - tail_available);
            tail = copied - tail_available;
        }
        count += copied;
        return copied;
    }
    
    int get(char* data,int length){
        int available = count;
        int copied = length - available > 0? available:length;
        if( copied == 0){
            return copied;
        }
        std::lock_guard<std::mutex> l(lock_);
        if(head < tail){
            memcpy(data,buffer+head,copied);
            head += copied;
            if(head == size){
                head = 0;
            }
        } else {
            int tail_available = size - head;
            if(tail_available > copied){
                memcpy(data,buffer+head,copied);
                head += copied;
                if(head == size){
                    head = 0;
                } 
            } else {
                memcpy(data,buffer+head,tail_available);
                memcpy(data+tail_available,buffer, copied -tail_available);
                head = copied -tail_available;
            }
        }
        count -= copied;
        return copied;
    }
    ~SimpleCircularBuffer(){
        if(buffer != nullptr){
            free(buffer);
            buffer = nullptr;
        }
    }
private:
    std::mutex lock_;
    unsigned char* buffer = nullptr;
    uint32_t size = 0;
    uint32_t count = 0;
    uint32_t head = 0;
    uint32_t tail = 0;
};

class AudioRenderOHOS {
public:
    AudioRenderOHOS(){
        audio_render_wrapper_ = this;
    }
    static int32_t OnWriteData(OH_AudioRenderer *renderer, void *userData, void *buffer,int32_t lenth);
    static AudioRenderOHOS* audio_render_wrapper_;
    void init(AudioParameters audioParameters);
    void start();
    void pause();
    void stop();
    void release();
    int putAudioData(void* buffer,uint32_t size);
    int getAudioData(void* buffer,uint32_t size);
private:
    SimpleCircularBuffer* buffer_{nullptr};
    OH_AudioRenderer* audio_render_{nullptr};
    AudioParameters audio_parameters_;
};