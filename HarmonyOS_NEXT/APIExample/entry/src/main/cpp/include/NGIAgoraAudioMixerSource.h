//
//  Agora SDK
//  Created by wangqin in 2023-05.
//  Copyright (c) 2018 Agora.io. All rights reserved.
//

#pragma once  // NOLINT(build/header_guard)

#include "AgoraBase.h"
#include "AgoraRefPtr.h"
#include "IAgoraService.h"
#include <api/cpp/aosl_ares_class.h>

namespace agora {
namespace rtc {

class IAudioTrack;
/**
 * The IAudioMixerSource class abstracts a multi-in-multi-out audio source which receives audio
 * streams from multiple local or remote audio tracks and generate mixed audio stream in user defined output
 * format. When only one audio track is added to the mixer, it simply forwards the incoming audio frames
 * to its sinks.
 */
class IAudioMixerSource : public RefCountInterface {
public:
  /**
   * Add audio track for mixing.
   * @param id The unique id of the stream.
   * @param track The instance of the audio track that you want mixer to receive its audio stream.
   * @return
   * 0 - Success
   * <0 - Failure
   */
  virtual int addAudioTrack(agora_refptr<IAudioTrack> track, bool is_remote, aosl_ref_t ares = AOSL_REF_INVALID) = 0;
  /**
   * Remove the audio track.
   * @param track The instance of the audio track that you want to remove from the mixer.
   * @return
   * 0 - Success
   * <0 - Failure
   */
  virtual int removeAudioTrack(agora_refptr<IAudioTrack> track, bool is_remote, aosl_ref_t ares = AOSL_REF_INVALID) = 0;
};

}
}