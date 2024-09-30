//
//  Agora SDK
//
//  Copyright (c) 2021 Agora.io. All rights reserved.
//

#pragma once  // NOLINT(build/header_guard)

#include "AgoraBase.h"
#include "IAgoraLog.h"
#include "AgoraRefPtr.h"
#include "NGIAgoraVideoFrame.h"
#include "AgoraMediaBase.h"

namespace agora {
namespace rtc {

// class ExtensionControlImpl;
class IAgoraVideoQualityAnalyzer : public RefCountInterface {
 public:
  class Control : public RefCountInterface {
  public:
    // virtual int postEvent(const char* key, const char* value) = 0;
    virtual void printLog(commons::LOG_LEVEL level, const char* format, ...) = 0;
  };


  virtual ~IAgoraVideoQualityAnalyzer() {}
  virtual int initializeVQA(const agora_refptr<Control>& control) = 0;

  virtual int pushYuvData(agora::agora_refptr<rtc::IVideoFrame> frame) {
    return -ERR_NOT_SUPPORTED;
  }

  virtual int getVqaMos(int& mos) {
    return -ERR_NOT_SUPPORTED;
  }
};

}  // namespace rtc
}  // namespace agora
