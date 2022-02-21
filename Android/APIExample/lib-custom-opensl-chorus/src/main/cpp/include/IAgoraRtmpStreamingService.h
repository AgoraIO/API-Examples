
// Copyright (c) 2019 Agora.io. All rights reserved

// This program is confidential and proprietary to Agora.io.
// And may not be copied, reproduced, modified, disclosed to others, published
// or used, in whole or in part, without the express prior written permission
// of Agora.io.

#pragma once  // NOLINT(build/header_guard)

#include "AgoraBase.h"
#include "AgoraRefPtr.h"
#include "IAgoraService.h"
#include "NGIAgoraRtcConnection.h"

namespace agora {
namespace rtc {

/**
 * The events in the RTMP channel.
*/
enum RTMP_CHANNEL_EVENT
{
  /**
   * 0: The live stream service is disconnected.
   */
  RTMP_CHANNEL_EVENT_DISCONNECT = 0,
  /**
   * 1: The live stream service has left the channel.
   */
  RTMP_CHANNEL_EVENT_LEAVE_CHANNEL = 1,
  /**
   * 2: The live stream service is banned by the server.
   */
  RTMP_CHANNEL_EVENT_BANNED_BY_SERVER = 2
};
/**
 * The IRtmpStreamingObserver class, which monitors events in of the live streaming service.
 */
class IRtmpStreamingObserver {
public:
  /**
   * Occurs when the state of the RTMP streaming changes.
   *
   * The SDK triggers this callback to report the result of the local user calling
   * `addPublishStreamUrl` or `removePublishStreamUrl`.
   *
   * This callback also reports the streaming URL and its current streaming state. When exceptions
   * occur, you can troubleshoot issues by referring to the detailed error description in the `errCode`
   * parameter.
   *
   * @param url The RTMP URL address.
   * @param state The RTMP streaming state: #RTMP_STREAM_PUBLISH_STATE.
   * @param errCode The detailed error information for streaming: #RTMP_STREAM_PUBLISH_ERROR.
   */
  virtual void onRtmpStreamingStateChanged(const char* url, RTMP_STREAM_PUBLISH_STATE state,
                                           RTMP_STREAM_PUBLISH_ERROR errCode) {
    (void)url;
    (void)state;
    (void)errCode;
  }
  /**
   * Reports the result of the `addPublishStreamUrl` method.
   *
   * This callback reports whether you have successfully added an RTMP stream to the CDN.
   *
   * @param url The RTMP URL address.
   * @param error The detailed error information.
   */
  virtual void onStreamPublished(const char* url, int error) {
    (void)url;
    (void)error;
  }
  /**
   * Reports the result of calling the `removePublishStreamUrl` method.
   *
   * This callback reports whether you have successfully removed an RTMP stream from the CDN.
   *
   * @param url The RTMP URL address.
   */
  virtual void onStreamUnpublished(const char* url) { (void)url; }
  /**
   * Occurs when the publisher's transcoding settings are updated.
   *
   * When the `LiveTranscoding` class in the `setLiveTransocding` method updates, the SDK triggers this callback
   * to report the update information.
   *
   * @note
   * If you call the `setLiveTranscoding` method to set the `LiveTranscoding` class for the first time, the SDK
   * does not trigger this callback.
   */
  virtual void onTranscodingUpdated() {}
};
/**
 * The IRtmpStreamingService class, which enables the live stream service.
 */
class IRtmpStreamingService : public RefCountInterface {
 public:
  /**
   * Publishes the local stream to the CDN.
   *
   * This method triggers the `onRtmpStreamingStateChanged` callback on the local client to report the state of
   * adding a local stream to the CDN.
   *
   * @note
   * - Ensure that you enable the RTMP Converter service before using this function.
   * - This method applies to Live-Broadcast only.
   * - Ensure that the user joins a channel before calling this method.
   * - This method adds only one stream URL address each time it is called.
   *
   * @param url The CDN streaming URL in the RTMP format. The maximum length of this parameter is 1024 bytes.
   * The URL address must not contain special character, such as Chinese language characters.
   * @param transcodingEnabled Sets whether transcoding is enabled/disabled. If you set this parameter as `true`,
   * ensure that you call the `setLiveTranscoding` method before this method.
   * - true: Enable transcoding.
   * - false: Disable transcoding.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int addPublishStreamUrl(const char* url, bool transcodingEnabled) = 0;
  /**
   * Removes an RTMP stream from the CDN.
   *
   * This method removes the RTMP URL address added by `addPublishStreamUrl` from a CDN live stream. The SDK triggers the `onRtmpStreamingStated` callback
   * on the local client to report the state of removing an RTMP stream from the CDN.
   *
   * @note
   * - Ensure that you enable the RTMP Converter service before using this function.
   * - This method applies to Live-Broadcast only.
   * - This method removes only one stream URL address each time it is called.
   *
   * @param url The RTMP URL address to be removed. The maximum length of this parameter is 1024 bytes.
   * The URL address must not contain special character, such as Chinese language characters.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int removePublishStreamUrl(const char* url) = 0;
  /**
   * Sets the video layout and audio settings for CDN live.
   *
   * The SDK triggers the `onTranscodingUpdated` callback when the `LiveTranscoding` class is updated using this method.
   * If you call this method to set `LiveTrancoding` for the first time, the SDK does not trigger this callback.
   *
   * @note
   * - Ensure that you enable the RTMP Converter service before using this function.
   * - This method applies to Live-Broadcast only.
   * - Ensure that you call this method before calling addPublishStreamUrl().
   */
  virtual int setLiveTranscoding(const LiveTranscoding& transcoding) = 0;

  /**
   * Registers an RTMP streaming observer.
   * @param observer The pointer to an RTMP streaming observer. See \ref agora::rtc::IRtmpStreamingObserver "IRtmpStreamingObserver".
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int registerObserver(IRtmpStreamingObserver* observer) = 0;
  /**
   * Releases the RTMP streaming observer created by registerObserver().
   * @param observer The pointer to the RTMP streaming observer that you want to release. See \ref agora::rtc::IRtmpStreamingObserver "IRtmpStreamingObserver".
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int unregisterObserver(IRtmpStreamingObserver* observer) = 0;

 protected:
  ~IRtmpStreamingService() {}
};




}  // namespace rtc
}  // namespace agora
