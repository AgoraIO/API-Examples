//
//  Agora Media SDK
//
//  Copyright (c) 2015 Agora IO. All rights reserved.
//
#pragma once

#include "AgoraBase.h"
#include "AgoraMediaBase.h"
#include "IAgoraMediaEngineBase.h"

namespace agora {

namespace rtc {
struct EncodedVideoFrameInfo;
}

namespace media {

/**
 * The IMediaEngine class.
 */
class IMediaEngine : virtual public IMediaEngineBase {
 public:
  /**
   * Registers a video frame observer object.
   *
   * @note
   * - Ensure that you call this method before joining the channel.
   * - If you register an observer for video raw video data, you cannot register an IVideoEncodedFrameObserver
   * object.
   *
   * @param observer A pointer to the video frame observer: IVideoFrameObserver.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int registerVideoFrameObserver(IVideoFrameObserver* observer) = 0;
  /**
   * Registers an audio frame observer object.
   *
   * @note
   * Ensure that you call this method before \ref IRtcEngine::joinChannel "joinChannel".
   *
   * @param observer A pointer to the audio frame observer object: IAudioFrameObserver,
   * nullptr means unregistering observer instead.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int registerAudioFrameObserver(IAudioFrameObserver* observer) = 0;
  /**
   * Registers a receiver object for the encoded video image.
   *
   * @note
   * - Ensure that you call this method before joining the channel.
   *
   * @param observer A pointer to the observer of the encoded video image: \ref IVideoEncodedFrameObserver
   * "IVideoEncodedFrameObserver".
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int registerVideoEncodedFrameObserver(IVideoEncodedFrameObserver* observer) = 0;
  /**
   * Pushes the encoded video image to the app.
   * @param imageBuffer A pointer to the video image.
   * @param length The data length.
   * @param videoEncodedFrameInfo The reference to the information of the encoded video frame:
   * \ref agora::rtc::EncodedVideoFrameInfo "EncodedVideoFrameInfo".
   * @param videoTrackId The id of the video track.
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int pushEncodedVideoImage(const unsigned char* imageBuffer, size_t length,
                                    const agora::rtc::EncodedVideoFrameInfo& videoEncodedFrameInfo,
                                    unsigned int videoTrackId = 0) = 0;
  
  /**
   * @hide For internal usage only
   */
  virtual int addVideoFrameRenderer(IVideoFrameObserver *renderer) = 0;

  /**
   * @hide For internal usage only
   */
  virtual int removeVideoFrameRenderer(IVideoFrameObserver *renderer) = 0;

 protected:
  virtual ~IMediaEngine() {}
};

}  // namespace media
}  // namespace agora
