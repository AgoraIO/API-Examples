
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
  
  virtual int ProcessAiNlpFrame(float* input, float* output) = 0;
  virtual int ClearHiddenState(float scale_factor = 0.0) = 0;
  virtual void Version(char ver[MAX_VERSION_LEN]) = 0;
};

}  // namespace rtc
}  // namespace agora
