//
//  Agora SDK
//
//  Copyright (c) 2022 Agora.io. All rights reserved.
//
#pragma once  // NOLINT(build/header_guard)

#include "AgoraBase.h"
#include "AgoraMediaBase.h"
#include "IAgoraRtcEngineEx.h"
#include "IAgoraMediaRecorderBase.h"

namespace agora {
namespace rtc {

class IMediaRecorder : public IMediaRecorderBase, public RefCountInterface {
 protected:
  virtual ~IMediaRecorder() {}

 public:
  /**
   * Registers the IMediaRecorderObserver object.
   *
   * @since v4.0.0
   *
   * @note Call this method before the startRecording method.
   *
   * @param callback The callbacks for recording audio and video streams. See \ref IMediaRecorderObserver.
   *
   * @return
   * - 0(ERR_OK): Success.
   * - < 0: Failure:
   */
  virtual int setMediaRecorderObserver(media::IMediaRecorderObserver* callback) = 0;
};

} //namespace rtc
} // namespace agora
