
 //Agora SDK

 //Copyright (c) 2021 Agora.io. All rights reserved.

#pragma once  // NOLINT(build/header_guard)

#include "AgoraRefPtr.h"

namespace agora {
namespace rtc {

class AINoiseSuppressorExtension : public RefCountInterface {
public:
 virtual ~AINoiseSuppressorExtension() {};

 virtual int ProcessDenoiseFrame(float* input, float* output) = 0;
 virtual int ClearHiddenState() = 0;
 virtual const char* GetLibVersion(int mode) = 0;
};

}  // namespace rtc
}  // namespace agora
