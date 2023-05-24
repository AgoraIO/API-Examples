//
//  Agora SDK
//
//  Copyright (c) 2020 Agora.io. All rights reserved.
//

#pragma once

#include "AgoraRefPtr.h"

namespace agora {
namespace rtc {

class IRemoteAudioTrack;

/**
 * The IRemoteAudioMixerSource class abstracts a multi-in-single-out audio source which receives audio
 * streams from multiple remote audio tracks and generate mixed audio stream in user defined output
 * format.
 */
class IRemoteAudioMixerSource : public RefCountInterface {
 public:
  virtual ~IRemoteAudioMixerSource() {}

  /**
   * Add a audio track for mixing. Automatically starts mixing if add audio track
   * @param track The instance of the audio track that you want mixer to receive its audio stream.
   */
  virtual int addAudioTrack(agora_refptr<IRemoteAudioTrack> track) = 0;

  /**
   * Remove a audio track for mixing. Automatically stops the mixed stream if all audio tracks are removed
   * @param track The instance of the audio track that you want to remove from the mixer.
   */
  virtual int removeAudioTrack(agora_refptr<IRemoteAudioTrack> track) = 0;

  /**
   * Gets the delay time for mix.
   */
  virtual int getMixDelay() const = 0;
};

} // namespace rtc
} // namespace agora
