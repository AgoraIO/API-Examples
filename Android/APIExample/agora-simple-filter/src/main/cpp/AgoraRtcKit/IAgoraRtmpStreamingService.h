
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
   * @param errCode The detailed error information for streaming: #RTMP_STREAM_PUBLISH_ERROR_TYPE.
   */
  virtual void onRtmpStreamingStateChanged(const char* url, RTMP_STREAM_PUBLISH_STATE state,
                                           RTMP_STREAM_PUBLISH_ERROR_TYPE errCode) {
    (void)url;
    (void)state;
    (void)errCode;
  }

  /** Reports events during the RTMP or RTMPS streaming.
   *
   * @since v3.1.0
   *
   * @param url The RTMP or RTMPS streaming URL.
   * @param eventCode The event code. See #RTMP_STREAMING_EVENT
   */
  virtual void onRtmpStreamingEvent(const char* url, RTMP_STREAMING_EVENT eventCode) {
    (void)url;
    (void)eventCode;
  }
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

  /** Publishes the local stream without transcoding to a specified CDN live RTMP address.  (CDN live only.)

    * The SDK returns the result of this method call in the \ref IRtcEngineEventHandler::onStreamPublished "onStreamPublished" callback.

    * The \ref agora::rtc::IRtcEngine::startRtmpStreamWithoutTranscoding "startRtmpStreamWithoutTranscoding" method call triggers the \ref agora::rtc::IRtcEngineEventHandler::onRtmpStreamingStateChanged "onRtmpStreamingStateChanged" callback on the local client to report the state of adding a local stream to the CDN.
    * @note
    * - Ensure that the user joins the channel before calling this method.
    * - This method adds only one stream RTMP URL address each time it is called.
    * - The RTMP URL address must not contain special characters, such as Chinese language characters.
    * - This method applies to Live Broadcast only.

    * @param url The CDN streaming URL in the RTMP format. The maximum length of this parameter is 1024 bytes.

    * @return
    * - 0: Success.
    * - < 0: Failure.
    * - #ERR_INVALID_ARGUMENT (2): The RTMP URL address is NULL or has a string length of 0.
    * - #ERR_NOT_INITIALIZED (7): You have not initialized the RTC engine when publishing the stream.
    */
  virtual int startRtmpStreamWithoutTranscoding(const char* url) = 0;

  /** Publishes the local stream with transcoding to a specified CDN live RTMP address.  (CDN live only.)

    * The SDK returns the result of this method call in the \ref IRtcEngineEventHandler::onStreamPublished "onStreamPublished" callback.

    * The \ref agora::rtc::IRtcEngine::startRtmpStreamWithTranscoding "startRtmpStreamWithTranscoding" method call triggers the \ref agora::rtc::IRtcEngineEventHandler::onRtmpStreamingStateChanged "onRtmpStreamingStateChanged" callback on the local client to report the state of adding a local stream to the CDN.
    * @note
    * - Ensure that the user joins the channel before calling this method.
    * - This method adds only one stream RTMP URL address each time it is called.
    * - The RTMP URL address must not contain special characters, such as Chinese language characters.
    * - This method applies to Live Broadcast only.

    * @param url The CDN streaming URL in the RTMP format. The maximum length of this parameter is 1024 bytes.
    * @param transcoding Sets the CDN live audio/video transcoding settings.  See LiveTranscoding.

    * @return
    * - 0: Success.
    * - < 0: Failure.
    *   - #ERR_INVALID_ARGUMENT (2): The RTMP URL address is NULL or has a string length of 0.
    *   - #ERR_NOT_INITIALIZED (7): You have not initialized the RTC engine when publishing the stream.
    */
  virtual int startRtmpStreamWithTranscoding(const char* url, const LiveTranscoding& transcoding) = 0;

  /** Update the video layout and audio settings for CDN live. (CDN live only.)
    * @note This method applies to Live Broadcast only.

    * @param transcoding Sets the CDN live audio/video transcoding settings. See LiveTranscoding.

    * @return
    * - 0: Success.
    * - < 0: Failure.
    */
  virtual int updateRtmpTranscoding(const LiveTranscoding& transcoding) = 0;
  /** Stop an RTMP stream with transcoding or without transcoding from the CDN. (CDN live only.)

    * This method removes the RTMP URL address (added by the \ref IRtcEngine::startRtmpStreamWithoutTranscoding "startRtmpStreamWithoutTranscoding" method
    * or IRtcEngine::startRtmpStreamWithTranscoding "startRtmpStreamWithTranscoding" method) from a CDN live stream.
    * The SDK returns the result of this method call in the \ref IRtcEngineEventHandler::onStreamUnpublished "onStreamUnpublished" callback.

    * The \ref agora::rtc::IRtcEngine::stopRtmpStream "stopRtmpStream" method call triggers the \ref agora::rtc::IRtcEngineEventHandler::onRtmpStreamingStateChanged "onRtmpStreamingStateChanged" callback on the local client to report the state of removing an RTMP stream from the CDN.
    * @note
    * - This method removes only one RTMP URL address each time it is called.
    * - The RTMP URL address must not contain special characters, such as Chinese language characters.
    * - This method applies to Live Broadcast only.

    * @param url The RTMP URL address to be removed. The maximum length of this parameter is 1024 bytes.

    * @return
    * - 0: Success.
    * - < 0: Failure.
    */
  virtual int stopRtmpStream(const char* url) = 0;

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
