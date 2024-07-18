//
// Created on 2023/12/12.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "audio_render.h"
#include "utils.h"
AudioRenderOHOS* AudioRenderOHOS::audio_render_wrapper_ = nullptr;
int32_t AudioRenderOHOS::OnWriteData(OH_AudioRenderer *renderer, void *userData, void *buffer, int32_t lenth){
//    AG_DEBUG("cqdebug onWriteData called length:%{public}d buffer:%{public}x",lenth,buffer);
    int count = 0;
    memset(buffer,0,lenth);
    if(audio_render_wrapper_ != nullptr){
        count = audio_render_wrapper_->getAudioData(buffer,lenth);
    }
    return count;
}
void AudioRenderOHOS::init(AudioParameters audioParameters) {
    // check audio parameter is valid
    if(!audioParameters.is_valid()){
        AG_ERROR("invalid audio parameters ");
        return;
    }
    this->audio_parameters_ = audioParameters;
    OH_AudioStreamBuilder *builder;
    OH_AudioStreamBuilder_Create(&builder, AUDIOSTREAM_TYPE_RENDERER);
    OH_AudioStreamBuilder_SetSamplingRate(builder, audio_parameters_.sample_rate());
    OH_AudioStreamBuilder_SetChannelCount(builder, audio_parameters_.channels());
    OH_AudioStreamBuilder_SetSampleFormat(builder, AUDIOSTREAM_SAMPLE_S16LE);
    OH_AudioStreamBuilder_SetEncodingType(builder, AUDIOSTREAM_ENCODING_TYPE_RAW);
    OH_AudioStreamBuilder_SetRendererInfo(builder, AUDIOSTREAM_USAGE_VOICE_COMMUNICATION);
    OH_AudioRenderer_Callbacks callbacks;
    callbacks.OH_AudioRenderer_OnWriteData = OnWriteData;
    OH_AudioStreamBuilder_SetRendererCallback(builder, callbacks, nullptr);
    OH_AudioStream_Result result = OH_AudioStreamBuilder_GenerateRenderer(builder, &audio_render_);
    if(result != AUDIOSTREAM_SUCCESS) {
        AG_ERROR("create audio renderer error :%{public}d ",result);
    }
    buffer_ = new SimpleCircularBuffer(audio_parameters_.sample_rate()* audio_parameters_.channels()*2/10*5); // buffer for 100ms data;
    OH_AudioStreamBuilder_Destroy(builder);
}

void AudioRenderOHOS::start(){
    if(audio_render_ != nullptr){
        OH_AudioRenderer_Start(audio_render_);
    }
}
void AudioRenderOHOS::pause() {
    if (audio_render_ != nullptr) {
        OH_AudioRenderer_Pause(audio_render_);
    }
}
void AudioRenderOHOS::stop(){
    if (audio_render_ != nullptr) {
        OH_AudioRenderer_Stop(audio_render_);
    }
}

void AudioRenderOHOS::release() {
    if (audio_render_ != nullptr) {
        OH_AudioRenderer_Flush(audio_render_);
        OH_AudioRenderer_Release(audio_render_);
    }
    if(buffer_ != nullptr){
        delete buffer_;
    }
}


int AudioRenderOHOS::putAudioData(void *buffer, uint32_t size){
    if(buffer == nullptr || buffer_ == nullptr) return 0;
    return buffer_->add((char*)buffer, size);
}

int AudioRenderOHOS::getAudioData(void *buffer, uint32_t size) {
    if (buffer == nullptr || buffer_ == nullptr)
        return 0;
    return buffer_->get((char*)buffer, size);
} 
