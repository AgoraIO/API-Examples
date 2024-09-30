//
//  Agora Media SDK
//
//  Created by Xiaosen Wang in 2020.
//  Copyright (c) 2020 Agora IO. All rights reserved.
//
#pragma once
#include "AgoraBase.h"
#include "AgoraRefPtr.h"
#include "IAgoraService.h"

namespace agora {
namespace rtc {
class IMediaRelayObserver;
class IMediaRelayService : public RefCountInterface {
 public:
  /** Starts or update to relay media streams across channels.
   *
   * @since v4.2.0
   * @param configuration The configuration of the media stream relay:
   * ChannelMediaRelayConfiguration.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   *   - -1(ERR_FAILED): A general error occurs (no specified reason).
   *   - -2(ERR_INVALID_ARGUMENT): The argument is invalid.
   *   - -5(ERR_REFUSED): The request is rejected.
   */
  virtual int startOrUpdateChannelMediaRelay(const ChannelMediaRelayConfiguration& configuration) = 0;

  /** Stops the media stream relay.
   *
   * Once the relay stops, the host quits all the destination
   * channels.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   *   - -1(ERR_FAILED): A general error occurs (no specified reason).
   *   - -2(ERR_INVALID_ARGUMENT): The argument is invalid.
   *   - -5(ERR_REFUSED): The request is rejected.
   */
  virtual int stopChannelMediaRelay(aosl_ref_t ares = AOSL_REF_INVALID) = 0;

  /** pause the channels for media stream relay.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   *   - -1(ERR_FAILED): A general error occurs (no specified reason).
   *   - -2(ERR_INVALID_ARGUMENT): The argument is invalid.
   *   - -5(ERR_REFUSED): The request is rejected.
   */
  virtual int pauseAllChannelMediaRelay(aosl_ref_t ares = AOSL_REF_INVALID) = 0;

  /** resume the channels for media stream relay.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   *   - -1(ERR_FAILED): A general error occurs (no specified reason).
   *   - -2(ERR_INVALID_ARGUMENT): The argument is invalid.
   *   - -5(ERR_REFUSED): The request is rejected.
   */
  virtual int resumeAllChannelMediaRelay(aosl_ref_t ares = AOSL_REF_INVALID) = 0;
  
  virtual void registerEventHandler(IMediaRelayObserver* eventObserver, void(*safeDeleter)(IMediaRelayObserver*) = NULL) = 0;
  virtual void unregisterEventHandler(IMediaRelayObserver* eventObserver) = 0;
};

class IMediaRelayObserver {
 public:
  virtual ~IMediaRelayObserver() {}

  /** Occurs when the state of the media stream relay changes.
   *
   * The SDK returns the state of the current media relay with any error
   * message.
   *
   * @param state The state code in #CHANNEL_MEDIA_RELAY_STATE.
   * @param code The error code in #CHANNEL_MEDIA_RELAY_ERROR.
   */
  virtual void onChannelMediaRelayStateChanged(CHANNEL_MEDIA_RELAY_STATE state,
                                               CHANNEL_MEDIA_RELAY_ERROR code) {}
};

}  // namespace rtc
}  // namespace agora
