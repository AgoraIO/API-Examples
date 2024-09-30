
 //Agora SDK

 //Copyright (c) 2021 Agora.io. All rights reserved.

#pragma once  // NOLINT(build/header_guard)

#include "AgoraRefPtr.h"

namespace agora {
namespace rtc {

class AINoiseSuppressorExtension : public RefCountInterface {
public:
 virtual ~AINoiseSuppressorExtension() {};
 virtual int Init(const char* weight_raw_buffer, size_t raw_buffer_bytes) = 0;
 virtual int ProcessDenoiseFrame(float* input, float* output, int stage) = 0;
 virtual int ClearHiddenState(bool clear_hidden, int clear_lpsbuffer_nframe) = 0;
 virtual const char* GetLibVersion(int mode = 0) = 0;
};


}  // namespace rtc
}  // namespace agora
