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
  /** Starts to relay media streams across channels.
   *
   * @param configuration The configuration of the media stream relay:
   * ChannelMediaRelayConfiguration.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int startChannelMediaRelay(const ChannelMediaRelayConfiguration& configuration) = 0;
  /** Updates the channels for media stream relay. After a successful
   * \ref startChannelMediaRelay() "startChannelMediaRelay" method call, if
   * you want to relay the media stream to more channels, or leave the
   * current relay channel, you can call the
   * \ref updateChannelMediaRelay() "updateChannelMediaRelay" method.
   *
   * @note
   * Call this method after the
   * \ref startChannelMediaRelay() "startChannelMediaRelay" method to update
   * the destination channel.
   *
   * @param configuration The media stream relay configuration:
   * ChannelMediaRelayConfiguration.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int updateChannelMediaRelay(const ChannelMediaRelayConfiguration& configuration) = 0;
  /** Stops the media stream relay.
   *
   * Once the relay stops, the host quits all the destination
   * channels.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int stopChannelMediaRelay() = 0;

  /** pause the channels for media stream relay.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int pauseAllChannelMediaRelay() = 0;

  /** resume the channels for media stream relay.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int resumeAllChannelMediaRelay() = 0;
  
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
  /** Reports events during the media stream relay.
   *
   * @param code The event code in #CHANNEL_MEDIA_RELAY_EVENT.
   */
  virtual void onChannelMediaRelayEvent(CHANNEL_MEDIA_RELAY_EVENT code) {}
};

}  // namespace rtc
}  // namespace agora
