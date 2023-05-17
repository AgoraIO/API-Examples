//
//  Agora SDK
//
//  Copyright (c) 2022 Agora.io. All rights reserved.
//
#pragma once  // NOLINT(build/header_guard)

#include "AgoraBase.h"
#include "AgoraMediaBase.h"
#include "IAgoraRtcEngineEx.h"

namespace agora {
namespace rtc {

class IMediaRecorder {
 protected:
  virtual ~IMediaRecorder() {}
 public:
  virtual int setMediaRecorderObserver(const RtcConnection& connection, media::IMediaRecorderObserver* callback) = 0;
  virtual int startRecording(const RtcConnection& connection, const media::MediaRecorderConfiguration& config) = 0;
  virtual int stopRecording(const RtcConnection& connection) = 0;
  virtual void release() = 0;
};

} //namespace rtc
} // namespace agora
