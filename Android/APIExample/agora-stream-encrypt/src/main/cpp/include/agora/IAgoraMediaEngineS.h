//
//  Agora Media SDK
//
//  Copyright (c) 2015 Agora IO. All rights reserved.
//
#pragma once

#include "AgoraBaseS.h"
#include "AgoraMediaBaseS.h"
#include "IAgoraMediaEngineBase.h"

namespace agora {

namespace rtc {
struct EncodedVideoFrameInfoS;
}

namespace media {

/**
 * The IMediaEngineS class.
 */
class IMediaEngineS : virtual public IMediaEngineBase {
 public:
  /**
   * Registers a video frame observer object.
   *
   * @technical preview
   * @note
   * - Ensure that you call this method before joining the channel.
   * - If you register an observer for video raw video data, you cannot register an IVideoEncodedFrameObserverS
   * object.
   *
   * @param observer A pointer to the video frame observer: IVideoFrameObserver.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int registerVideoFrameObserver(IVideoFrameObserverS* observer) = 0;
  /**
   * Registers an audio frame observer object.
   *
   * @technical preview
   * @note
   * Ensure that you call this method before \ref IRtcEngine::joinChannel "joinChannel".
   *
   * @param observer A pointer to the audio frame observer object: IAudioFrameObserver,
   * nullptr means unregistering observer instead.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int registerAudioFrameObserver(IAudioFrameObserverBase* observer) = 0;
  /**
   * Registers a receiver object for the encoded video image.
   *
   * @technical preview
   * @note
   * - Ensure that you call this method before joining the channel.
   *
   * @param observer A pointer to the observer of the encoded video image: \ref IVideoEncodedFrameObserverS
   * "IVideoEncodedFrameObserverS".
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int registerVideoEncodedFrameObserver(IVideoEncodedFrameObserverS* observer) = 0;
  /**
   * Pushes the encoded video image to the app.
   * @technical preview
   * @param imageBuffer A pointer to the video image.
   * @param length The data length.
   * @param videoEncodedFrameInfo The reference to the information of the encoded video frame:
   * \ref agora::rtc::EncodedVideoFrameInfoS "EncodedVideoFrameInfoS".
   * @param videoTrackId The id of the video track.
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int pushEncodedVideoImage(const unsigned char* imageBuffer, size_t length,
                                    const agora::rtc::EncodedVideoFrameInfoS& videoEncodedFrameInfo,
                                    unsigned int videoTrackId = 0) = 0;

  /**
   * @hide For internal usage only
   * @technical preview
   */
  virtual int addVideoFrameRenderer(IVideoFrameObserverS *renderer) = 0;

  /**
   * @hide For internal usage only
   * @technical preview
   */
  virtual int removeVideoFrameRenderer(IVideoFrameObserverS *renderer) = 0;
  
 protected:
  virtual ~IMediaEngineS() {}
};

}  // namespace media
}  // namespace agora
