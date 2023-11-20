//
//  Agora Media SDK
//
//  Copyright (c) 2022 Agora IO. All rights reserved.
//

#pragma once

#include "IAgoraH265TranscoderBase.h"

namespace agora{
namespace rtc{

/**
 * The IH265TranscoderS class
*/
class IH265TranscoderS: public IH265TranscoderBase, public RefCountInterface {
 public:

  /**
   * Enable transcoding for a channel.
   * @param token The token for authentication.
   * @param channel The unique channel name for the AgoraRTC session in the string format.
   * @param userAccount  User Account.
   * @return
   * -  0: Success.
   * - <0: Failure.
  */
  virtual int enableTranscode(const char *token, const char *channel, const char* userAccount) = 0;

  /**
   * Query the transcoded channel of a channel.
   * @param token The token for authentication.
   * @param channel The unique channel name for the AgoraRTC session in the string format.
   * @param userAccount  User Account.
   * @return
   * -  0: Success.
   * - <0: Failure.
  */
  virtual int queryChannel(const char *token, const char *channel, const char* userAccount) = 0;

  /**
   * Trigger channel transcoding.
   * @param token The token for authentication.
   * @param channel The unique channel name for the AgoraRTC session in the string format.
   * @param userAccount  User Account.
   * @return
   * -  0: Success.
   * - <0: Failure.
  */
  virtual int triggerTranscode(const char* token, const char* channel, const char* userAccount) = 0;

 protected:
  virtual ~IH265TranscoderS() {};
};

} // namespace rtc
} // namespace agora