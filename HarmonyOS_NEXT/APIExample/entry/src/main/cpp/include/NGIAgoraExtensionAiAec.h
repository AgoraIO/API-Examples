
//  Agora SDK
//
//  Created by LLF on 2022-8-15.
//  Copyright (c) 2022 Agora. All rights reserved.
//

#pragma once  // NOLINT(build/header_guard)

#include "AgoraRefPtr.h"

#define MAX_VERSION_LEN 64

namespace agora {
namespace rtc {

class AiEchoCancellationExtension : public RefCountInterface {
 public:
  virtual ~AiEchoCancellationExtension() {};
  virtual int Init(const char* weight_raw_buffer, size_t raw_buffer_bytes) = 0;
  virtual int ProcessAiNlpFrame(const float* linear,  const float* nearin, const float* far_ref, float* out_mask) = 0;
  virtual int ClearHiddenState(float scale_factor = 0.0,int clear_lpsbuffer_nframe = 0) = 0;
  virtual const char* GetLibVersion(int mode = 0) = 0;
};

}  // namespace rtc
}  // namespace agora
