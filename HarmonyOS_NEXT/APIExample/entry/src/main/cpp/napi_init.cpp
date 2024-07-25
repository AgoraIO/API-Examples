//
// Created on 2024/6/16.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "custom_audio_render.h"
#include "napi/native_api.h"

#include "origin_audio_data.h"
#include "origin_video_data.h"
#include "media_metadata.h"
#include "custom_audio_render.h"


EXTERN_C_START
static napi_value Init(napi_env env, napi_value exports) {
    OriginAudioData::Init(env, exports);
    OriginVideoData::Init(env, exports);
    MediaMetadata::Init(env, exports);
    CustomAudioRender::Init(env, exports);
    return exports;
}
EXTERN_C_END

static napi_module demoModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = Init,
    .nm_modname = "rawdataprocessing",
    .nm_priv = ((void *)0),
    .reserved = {0},
};

extern "C" __attribute__((constructor)) void RegisterEntryModule(void) { napi_module_register(&demoModule); }
