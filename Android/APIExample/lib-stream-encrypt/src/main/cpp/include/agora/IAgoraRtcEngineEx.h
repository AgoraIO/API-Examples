//
//  Agora Media SDK
//
//  Created by Sting Feng in 2015-05.
//  Updated by Tommy Miao in 2020-11.
//  Copyright (c) 2015 Agora IO. All rights reserved.
//
#pragma once

#include <string>
#include "IAgoraRtcEngine.h"

#ifndef OPTIONAL_ENUM_CLASS
#if __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1800)
#define OPTIONAL_ENUM_CLASS enum class
#else
#define OPTIONAL_ENUM_CLASS enum
#endif
#endif

#ifndef OPTIONAL_NULLPTR
#if __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1800)
#define OPTIONAL_NULLPTR nullptr
#else
#define OPTIONAL_NULLPTR NULL
#endif
#endif

namespace agora {
namespace rtc {

// OPTIONAL_ENUM_CLASS RTC_EVENT;

class IRtcEngineEventHandlerEx : public IRtcEngineEventHandler {
 public:
  using IRtcEngineEventHandler::onJoinChannelSuccess;
  using IRtcEngineEventHandler::onRejoinChannelSuccess;
  using IRtcEngineEventHandler::onWarning;
  using IRtcEngineEventHandler::onError;
  using IRtcEngineEventHandler::onAudioQuality;
  using IRtcEngineEventHandler::onLastmileProbeResult;
  using IRtcEngineEventHandler::onAudioVolumeIndication;
  using IRtcEngineEventHandler::onLeaveChannel;
  using IRtcEngineEventHandler::onRtcStats;
  using IRtcEngineEventHandler::onAudioDeviceStateChanged;
  using IRtcEngineEventHandler::onAudioMixingFinished;
  using IRtcEngineEventHandler::onRemoteAudioMixingBegin;
  using IRtcEngineEventHandler::onRemoteAudioMixingEnd;
  using IRtcEngineEventHandler::onAudioEffectFinished;
  using IRtcEngineEventHandler::onVideoDeviceStateChanged;
  using IRtcEngineEventHandler::onMediaDeviceChanged;
  using IRtcEngineEventHandler::onNetworkQuality;
  using IRtcEngineEventHandler::onIntraRequestReceived;
  using IRtcEngineEventHandler::onLastmileQuality;
  using IRtcEngineEventHandler::onFirstLocalVideoFrame;
  using IRtcEngineEventHandler::onFirstLocalVideoFramePublished;
  using IRtcEngineEventHandler::onVideoSourceFrameSizeChanged;
  using IRtcEngineEventHandler::onFirstRemoteVideoDecoded;
  using IRtcEngineEventHandler::onVideoSizeChanged;
  using IRtcEngineEventHandler::onLocalVideoStateChanged;
  using IRtcEngineEventHandler::onRemoteVideoStateChanged;
  using IRtcEngineEventHandler::onFirstRemoteVideoFrame;
  using IRtcEngineEventHandler::onUserJoined;
  using IRtcEngineEventHandler::onUserOffline;
  using IRtcEngineEventHandler::onUserMuteVideo;
  using IRtcEngineEventHandler::onUserEnableVideo;
  using IRtcEngineEventHandler::onUserEnableLocalVideo;
  using IRtcEngineEventHandler::onApiCallExecuted;
  using IRtcEngineEventHandler::onLocalAudioStats;
  using IRtcEngineEventHandler::onRemoteAudioStats;
  using IRtcEngineEventHandler::onLocalVideoStats;
  using IRtcEngineEventHandler::onRemoteVideoStats;
  using IRtcEngineEventHandler::onCameraReady;
  using IRtcEngineEventHandler::onCameraFocusAreaChanged;
  using IRtcEngineEventHandler::onCameraExposureAreaChanged;
  using IRtcEngineEventHandler::onVideoStopped;
  using IRtcEngineEventHandler::onAudioMixingStateChanged;
  using IRtcEngineEventHandler::onConnectionLost;
  using IRtcEngineEventHandler::onConnectionInterrupted;
  using IRtcEngineEventHandler::onConnectionBanned;
  using IRtcEngineEventHandler::onRefreshRecordingServiceStatus;
  using IRtcEngineEventHandler::onStreamMessage;
  using IRtcEngineEventHandler::onStreamMessageError;
  using IRtcEngineEventHandler::onMediaEngineLoadSuccess;
  using IRtcEngineEventHandler::onMediaEngineStartCallSuccess;
  using IRtcEngineEventHandler::onRequestToken;
  using IRtcEngineEventHandler::onTokenPrivilegeWillExpire;
  using IRtcEngineEventHandler::onFirstLocalAudioFramePublished;
  using IRtcEngineEventHandler::onLocalAudioStateChanged;
  using IRtcEngineEventHandler::onRemoteAudioStateChanged;
  using IRtcEngineEventHandler::onActiveSpeaker;
  using IRtcEngineEventHandler::onClientRoleChanged;
  using IRtcEngineEventHandler::onAudioDeviceVolumeChanged;
  using IRtcEngineEventHandler::onRtmpStreamingStateChanged;
  using IRtcEngineEventHandler::onStreamPublished;
  using IRtcEngineEventHandler::onStreamUnpublished;
  using IRtcEngineEventHandler::onTranscodingUpdated;
  using IRtcEngineEventHandler::onStreamInjectedStatus;
  using IRtcEngineEventHandler::onAudioRoutingChanged;
  using IRtcEngineEventHandler::onChannelMediaRelayStateChanged;
  using IRtcEngineEventHandler::onChannelMediaRelayEvent;
  using IRtcEngineEventHandler::onLocalPublishFallbackToAudioOnly;
  using IRtcEngineEventHandler::onRemoteSubscribeFallbackToAudioOnly;
  using IRtcEngineEventHandler::onRemoteAudioTransportStats;
  using IRtcEngineEventHandler::onRemoteVideoTransportStats;
  using IRtcEngineEventHandler::onConnectionStateChanged;
  using IRtcEngineEventHandler::onNetworkTypeChanged;
  using IRtcEngineEventHandler::onEncryptionError;

 /**
  * when join channel success, the function will be called
  * @param [in] channel
  *        the channel name you have joined
  * @param [in] uid
  *        the uid of you in this channel
  * @param [in] elapsed
  *        the time elapsed in ms from the joinChannel been called to joining completed
  */
  virtual void onJoinChannelSuccess(conn_id_t connId, const char* channel, uid_t uid, int elapsed) {
    (void)connId;
    (void)channel;
    (void)uid;
    (void)elapsed;
  }

 /**
  * when join channel success, the function will be called
  * @param [in] channel
  *        the channel name you have joined
  * @param [in] uid
  *        the uid of you in this channel
  * @param [in] elapsed
  *        the time elapsed in ms elapsed
  */
  virtual void onRejoinChannelSuccess(conn_id_t connId, const char* channel, uid_t uid, int elapsed) {
    (void)connId;
    (void)channel;
    (void)uid;
    (void)elapsed;
  }

 /** A warning occurs during the SDK runtime.

  In most cases, the app can ignore the warnings reported by the SDK,
  because the SDK can fix the issues and resume running.

  @param warn The warning code.
  @param msg The detailed warning message: #WARN_CODE_TYPE.
   */
  virtual void onWarning(conn_id_t connId, int warn, const char* msg) {
    (void)connId;
    (void)warn;
    (void)msg;
  }

 /** An error occurs during the SDK runtime.

  @param err The error code.
  @param msg The detailed error message: #ERROR_CODE_TYPE.
   */
  virtual void onError(conn_id_t connId, int err, const char* msg) {
    (void)connId;
    (void)err;
    (void)msg;
  }

 /**
  * when audio quality message come, the function will be called
  * @param [in] uid
  *        the uid of the peer
  * @param [in] quality
  *        the quality of the remote user, see QUALITY_TYPE for value definition
  * @param [in] delay
  *        the average time of the audio packages delayed
  * @param [in] lost
  *        the rate of the audio packages lost
  */
  virtual void onAudioQuality(conn_id_t connId, uid_t uid, int quality, unsigned short delay, unsigned short lost) {
    (void)connId;
    (void)uid;
    (void)quality;
    (void)delay;
    (void)lost;
  }

  /** Reports the result of the last-mile network probe result.
   * The SDK triggers this callback within 30 seconds after the app calls the 
   * \ref IRtcEngine::startLastmileProbeTest "startLastmileProbeTest" method.
   * @param result The uplink and downlink last-mile network probe test result: 
   * LastmileProbeResult.
   */
  virtual void onLastmileProbeResult(conn_id_t connId, const LastmileProbeResult& result) {
      (void)connId;
      (void)result;
  }

  virtual void onAudioVolumeIndication(conn_id_t connId, const AudioVolumeInfo* speakers, unsigned int speakerNumber,
                                       int totalVolume) {
    (void)connId;
    (void)speakers;
    (void)speakerNumber;
    (void)totalVolume;
  }

  /**
   * Occurs when the local user successfully leaves the channel.
   *
   * When the user successfully leaves the channel after calling \ref IRtcEngine::leaveChannel "leaveChannel",
   * the SDK uses this callback to notify the app that the user has left the channel.
   *
   * This callback also reports information such as the call duration and the statistics of data received
   * or transmitted by the SDK.
   *
   * @param stats The statistics on the call: RtcStats.
   */
  virtual void onLeaveChannel(conn_id_t connId, const RtcStats& stats) {
    (void)connId;
    (void)stats;
  }

  /**
   * Reports the statistics of the current call.
   *
   * This callback is triggered once every two seconds after the user joins the channel.
   *
   * @param stats The statistics of the current call: RtcStats.
   */
  virtual void onRtcStats(conn_id_t connId, const RtcStats& stats) {
    (void)connId;
    (void)stats;
  }

  /** Occurs when the audio device state changes.

   This callback notifies the application that the system's audio device state
   is changed. For example, a headset is unplugged from the device.

   @param deviceId Pointer to the device ID.
   @param deviceType Device type: #MEDIA_DEVICE_TYPE.
   @param deviceState Device state: #MEDIA_DEVICE_STATE_TYPE.
   */
  virtual void onAudioDeviceStateChanged(conn_id_t connId, const char* deviceId, int deviceType, int deviceState) {
    (void)connId;
    (void)deviceId;
    (void)deviceType;
    (void)deviceState;
  }

  /** Occurs when the audio mixing file playback finishes.
   @deprecated This method is deprecated, use onAudioMixingStateChanged instead.

   You can start an audio mixing file playback by calling the
   \ref IRtcEngine::startAudioMixing "startAudioMixing" method. The SDK
   triggers this callback when the audio mixing file playback finishes.
   */
  virtual void onAudioMixingFinished(conn_id_t connId) {
    (void)connId;
  }

  virtual void onRemoteAudioMixingBegin(conn_id_t connId) {
    (void)connId;
  }

  virtual void onRemoteAudioMixingEnd(conn_id_t connId) {
    (void)connId;
  }

  virtual void onAudioEffectFinished(conn_id_t connId, int soundId) {
    (void)connId;
    (void)soundId;
  }

  /** Occurs when the video device state changes.

   This callback notifies the application that the system's video device state
   is changed.

   @param deviceId Pointer to the device ID.
   @param deviceType Device type: #MEDIA_DEVICE_TYPE.
   @param deviceState Device state: #MEDIA_DEVICE_STATE_TYPE.
   */
  virtual void onVideoDeviceStateChanged(conn_id_t connId, const char* deviceId, int deviceType, int deviceState) {
    (void)connId;
    (void)deviceId;
    (void)deviceType;
    (void)deviceState;
  }

  /** Occurs when the device changes.

   This callback notifies the application that the system's video device state
   is changed.
   
   @param deviceType Device type: #MEDIA_DEVICE_TYPE.
   */
  virtual void onMediaDeviceChanged(conn_id_t connId, int deviceType) {
    (void)connId;
    (void)deviceType;
  }

 /**
  * report the network quality
  * @param [in] uid
  *        the uid of the remote user
  * @param [in] txQuality
  *        the score of the send network quality 0~5 the higher the better
  * @param [in] rxQuality
  *        the score of the recv network quality 0~5 the higher the better
  */
  virtual void onNetworkQuality(conn_id_t connId, uid_t uid, int txQuality, int rxQuality) {
    (void)connId;
    (void)uid;
    (void)txQuality;
    (void)rxQuality;
  }

  /**
   * Occurs when intra request from remote user is received.
   * 
   * This callback is triggered once remote user needs one Key frame.
   * 
   */
  virtual void onIntraRequestReceived(conn_id_t connId) {
    (void)connId;
  }

  /**
   * Occurs when network info is updated.
   * 
   * This callback is used for notifying user to adjust the send pace based
   * on the target bitrate.
   * 
   * @param info The network info collections.
   */
  /*
  virtual void onBandwidthEstimationUpdated(conn_id_t connId, const NetworkInfo& info) {
    (void)connId;
    (void)info;
  }*/

  /**
   * Reports the last-mile network quality of the local user.
   *
   * This callback is triggered once after
   * \ref IRtcEngine::startLastmileProbeTest "startLastmileProbeTest" is called.
   *
   * When the user is not in a channel and the last-mile network test is enabled
   * (by calling `startLastmileProbeTest`), this callback function is triggered
   * to update the app on the network connection quality of the local user.
   *
   * @param quality The network quality: #QUALITY_TYPE.
   */
  virtual void onLastmileQuality(conn_id_t connId, int quality) {
    (void)connId;
    (void)quality;
  }

 /** Occurs when the first local video frame is displayed on the video window.
  @param width The width (pixels) of the video stream.
  @param height The height (pixels) of the video stream.
  @param elapsed The time elapsed (ms) from the local user calling
  \ref IRtcEngine::joinChannel "joinChannel" until this callback is triggered.
  */
  virtual void onFirstLocalVideoFrame(conn_id_t connId, int width, int height, int elapsed) {
    (void)connId;
    (void)width;
    (void)height;
    (void)elapsed;
  }

  /** Occurs when the first local video frame is published.

  @param elapsed The time elapsed (ms) from the local user calling
  \ref IRtcEngine::joinChannel "joinChannel" to the SDK triggers this callback.
  */
  virtual void onFirstLocalVideoFramePublished(conn_id_t connId, int elapsed) {
    (void)connId;
    (void)elapsed;
  }

  virtual void onVideoSourceFrameSizeChanged(conn_id_t connId, VIDEO_SOURCE_TYPE sourceType, int width, int height) {
    (void)connId;
    (void)sourceType;
    (void)width;
    (void)height;
  }

 /**
  * when the first remote video frame decoded, the function will be called
  * @param [in] uid
  *        the uid of the remote user
  * @param [in] width
  *        the width of the video frame
  * @param [in] height
  *        the height of the video frame
  * @param [in] elapsed
  *        the time elapsed from channel joined in ms
  */
  virtual void onFirstRemoteVideoDecoded(conn_id_t connId, uid_t uid, int width, int height, int elapsed) {
    (void)connId;
    (void)uid;
    (void)width;
    (void)height;
    (void)elapsed;
  }

 /**
  * when video size changed or rotation changed, the function will be called
  * @param [in] uid
  *        the uid of the remote user or local user (0)
  * @param [in] width
  *        the new width of the video
  * @param [in] height
  *        the new height of the video
  * @param [in] rotation
  *        the rotation of the video
  */
  virtual void onVideoSizeChanged(conn_id_t connId, uid_t uid, int width, int height, int rotation) {
    (void)connId;
    (void)uid;
    (void)width;
    (void)height;
    (void)rotation;
  }

  /** Occurs when the local video stream state changes
   *
   * This callback indicates the state of the local video stream, including camera capturing and video encoding,
   * and allows you to troubleshoot issues when exceptions occur.
   *
   * @note For some device models, the SDK will not trigger this callback when the state of the local video changes
   * while the local video capturing device is in use, so you have to make your own timeout judgment.
   * @param state State type #LOCAL_VIDEO_STREAM_STATE. When the state is LOCAL_VIDEO_STREAM_STATE_FAILED (3), see the `error` parameter for details.
   * @param error The detailed error information: #LOCAL_VIDEO_STREAM_ERROR.
   */
  virtual void onLocalVideoStateChanged(conn_id_t connId,
                                        LOCAL_VIDEO_STREAM_STATE state,
                                        LOCAL_VIDEO_STREAM_ERROR errorCode) {
    (void)connId;
    (void)state;
    (void)errorCode;
  }

  /**
   * Occurs when the remote video state has changed.
   * REMOTE_VIDEO_STATE_REASON_REMOTE_OFFLINE is not 100% guaranteed. When the
   * remote user leaves, use onUserOffline to make a business decision.
   *
   * @param uid ID of the user whose video state has changed.
   * @param state The remote video state: #REMOTE_VIDEO_STATE.
   * @param reason The reason of the remote video state change: #REMOTE_VIDEO_STATE_REASON.
   * @param elapsed The time elapsed (ms) from the local user calling ref IRtcEngine::joinChannel "joinChannel" until this callback is triggered.
   */
  virtual void onRemoteVideoStateChanged(conn_id_t connId, uid_t uid, REMOTE_VIDEO_STATE state, REMOTE_VIDEO_STATE_REASON reason, int elapsed) {
    (void)connId;
    (void)uid;
    (void)state;
    (void)reason;
    (void)elapsed;
  }

 /**
  * when the first remote video frame displayed, the function will be called
  * @param [in] uid
  *        the uid of the remote user
  * @param [in] width
  *        the width of the video frame
  * @param [in] height
  *        the height of the video frame
  * @param [in] elapsed
  *        the time elapsed from remote user called joinChannel in ms
  */
  virtual void onFirstRemoteVideoFrame(conn_id_t connId, uid_t uid, int width, int height, int elapsed) {
    (void)connId;
    (void)uid;
    (void)width;
    (void)height;
    (void)elapsed;
  }

 /**
  * when any other user joined in the same channel, the function will be called
  * @param [in] uid
  *        the uid of the remote user
  * @param [in] elapsed
  *        the time elapsed from remote used called joinChannel to joining completed in ms
  */
  virtual void onUserJoined(conn_id_t connId, uid_t uid, int elapsed) {
    (void)connId;
    (void)uid;
    (void)elapsed;
  }

 /**
  * when user offline(exit channel or offline by accident), the function will be called
  * @param [in] uid
  *        the uid of the remote user
  */
  virtual void onUserOffline(conn_id_t connId, uid_t uid, USER_OFFLINE_REASON_TYPE reason) {
    (void)connId;
    (void)uid;
    (void)reason;
  }

 /**
  * when remote user muted the video stream, the function will be called
  * @deprecated Use onRemoteVideoStateChanged instead of.
  * @param [in] uid
  *        the uid of the remote user
  * @param [in] muted
  *        true: the remote user muted the video stream, false: the remote user unmuted the video stream
  */
  virtual void onUserMuteVideo(conn_id_t connId, uid_t uid, bool muted) {
    (void)connId;
    (void)uid;
    (void)muted;
  }

 /**
  * when remote user enable video function, the function will be called
  * @deprecated Use onRemoteVideoStateChanged instead of.
  * @param [in] uid
  *        the uid of the remote user
  * @param [in] enabled
  *        true: the remote user has enabled video function, false: the remote user has disabled video function
  */
  virtual void onUserEnableVideo(conn_id_t connId, uid_t uid, bool enabled) {
    (void)connId;
    (void)uid;
    (void)enabled;
  }

  /**
   * when remote user enable local video function, the function will be called
   * @deprecated Use onRemoteVideoStateChanged instead of.
   * @param [in] uid
   *        the uid of the remote user
   * @param [in] enabled
   *        true: the remote user has enabled local video function, false: the remote user has disabled local video function
   */
  virtual void onUserEnableLocalVideo(conn_id_t connId, uid_t uid, bool enabled) {
    (void)connId;
    (void)uid;
    (void)enabled;
  }

  /**
   * Occurs when an API method is executed.
   *
   * @param api The API method that the SDK executes.
   * @param err The error code that the SDK returns when the method call fails.
   * @param result The result of the method call.
   */
  virtual void onApiCallExecuted(conn_id_t connId, int err, const char* api, const char* result) {
    (void)connId;
    (void)err;
    (void)api;
    (void)result;
  }

  /** Reports the statistics of the local audio stream.
   *
   * The SDK triggers this callback once every two seconds.
   *
   * @param stats The statistics of the local audio stream.
   * See LocalAudioStats.
   */
  virtual void onLocalAudioStats(conn_id_t connId, const LocalAudioStats& stats) {
    (void)connId;
    (void)stats;
  }

  /** Reports the statistics of the audio stream from each remote user/host.

   This callback replaces the \ref IRtcEngineEventHandler::onAudioQuality "onAudioQuality" callback.

   The SDK triggers this callback once every two seconds for each remote
   user/host. If a channel includes multiple remote users, the SDK triggers
   this callback as many times.

   @param stats Statistics of the received remote audio streams. See RemoteAudioStats.
   */
  virtual void onRemoteAudioStats(conn_id_t connId, const RemoteAudioStats& stats) {
    (void)connId;
    (void)stats;
  }

  /**
   * Reports the statistics of the local video.
   *
   * The SDK triggers this callback once every two seconds after the user joins the channel.
   *
   * @param stats The statistics of the uploading local video stream: LocalVideoStats.
   */
  virtual void onLocalVideoStats(conn_id_t connId, const LocalVideoStats& stats) {
    (void)connId;
    (void)stats;
  }

  /** Reports the statistics of the remote video.
   *
   * The SDK triggers this callback once every two seconds for each remote user or broadcaster. If
   * there are multiple users or broadcasters in the channel, the SDK triggers this callback
   * as many times.
   * @param stats The statistics of the remote video streams: RemoteVideoStats.
   */
  virtual void onRemoteVideoStats(conn_id_t connId, const RemoteVideoStats& stats) {
    (void)connId;
    (void)stats;
  }

  virtual void onCameraReady(conn_id_t connId) {
    (void)connId;
  }

  virtual void onCameraFocusAreaChanged(conn_id_t connId, int x, int y, int width, int height) {
    (void)connId;
    (void)x;
    (void)y;
    (void)width;
    (void)height;
  }

  virtual void onCameraExposureAreaChanged(conn_id_t connId, int x, int y, int width, int height) {
    (void)connId;
    (void)x;
    (void)y;
    (void)width;
    (void)height;
  }

  virtual void onVideoStopped(conn_id_t connId) {
    (void)connId;
  }

  /** Occurs when the state of the local user's audio mixing file changes.
   When you call the \ref IRtcEngine::startAudioMixing "startAudioMixing"
   method and the state of audio mixing file changes, the SDK triggers this
   callback.

   - When the audio mixing file plays, pauses playing, or stops playing, this
   callback returns 710, 711, or 713 in `state`, and 0 in `errorCode`.
   - When exceptions occur during playback, this callback returns 714 in
   `state` and an error in `errorCode`.
   - If the local audio mixing file does not exist, or if the SDK does not
   support the file format or cannot access the music file URL, the SDK
   returns #WARN_AUDIO_MIXING_OPEN_ERROR (701).

   @param state The state code. See #AUDIO_MIXING_STATE_TYPE.
   @param errorCode The error code. See #AUDIO_MIXING_ERROR_TYPE.
   */
  virtual void onAudioMixingStateChanged(conn_id_t connId, AUDIO_MIXING_STATE_TYPE state, AUDIO_MIXING_ERROR_TYPE errorCode) {
    (void)connId;
    (void)state;
    (void)errorCode;
  }

  /**
   * Occurs when the SDK cannot reconnect to the server 10 seconds after its connection to the server is
   * interrupted.
   *
   * The SDK triggers this callback when it cannot connect to the server 10 seconds after calling
   * \ref IRtcEngine::joinChannel "joinChannel", regardless of whether it is in the channel or not.
   */
  virtual void onConnectionLost(conn_id_t connId) {
    (void)connId;
  }

  /** Occurs when the connection between the SDK and the server is interrupted.

  The SDK triggers this callback when it loses connection with the serer for more
  than 4 seconds after the connection is established. After triggering this
  callback, the SDK tries to reconnect to the server. If the reconnection fails
  within a certain period (10 seconds by default), the onConnectionLost()
  callback is triggered.

  Once the connection is lost, the SDK continues to reconnect until the
  app calls \ref IRtcEngine::leaveChannel "leaveChannel".
  */
  virtual void onConnectionInterrupted(conn_id_t connId) {
    (void)connId;
  }

  /** Occurs when your connection is banned by the Agora Server.
   */
  virtual void onConnectionBanned(conn_id_t connId) {
    (void)connId;
  }

  virtual void onRefreshRecordingServiceStatus(conn_id_t connId, int status) {
    (void)connId;
    (void)status;
  }

  /**
   * when stream message received, the function will be called
   * @param [in] uid
   *        uid of the peer who sends the message
   * @param [in] streamId
   *        APP can create multiple streams for sending messages of different purposes
   * @param [in] data
   *        the message data
   * @param [in] length
   *        the message length, in bytes
   *        frame rate
   */
  virtual void onStreamMessage(conn_id_t connId, uid_t uid, int streamId, const char* data, size_t length) {
    (void)connId;
    (void)uid;
    (void)streamId;
    (void)data;
    (void)length;
  }

  virtual void onStreamMessage(uid_t uid, int streamId, const char* data, size_t length, uint64_t sentTs) {
    (void)uid;
    (void)streamId;
    (void)data;
    (void)length;
    (void)sentTs;
  }

  /** Occurs when the local user fails to receive a remote data stream.

  The SDK triggers this callback when the local user fails to receive the stream
  message that the remote user sends by calling the \ref IRtcEngine::sendStreamMessage
  "sendStreamMessage()"
  method.

  @param uid ID of the remote user who sends the data stream.
  @param streamId The stream ID.
  @param code The error code: #ERROR_CODE_TYPE.
  @param missed The number of lost messages.
  @param cached The number of incoming cached messages when the data stream is
  interrupted.
   *
   */
  virtual void onStreamMessageError(conn_id_t connId, uid_t uid, int streamId, int code, int missed, int cached) {
    (void)connId;
    (void)uid;
    (void)streamId;
    (void)code;
    (void)missed;
    (void)cached;
  }

  virtual void onMediaEngineLoadSuccess(conn_id_t connId) {
    (void)connId;
  }

  virtual void onMediaEngineStartCallSuccess(conn_id_t connId) {
    (void)connId;
  }

  /**
   * Occurs when the token has expired.
   *
   * If a token is specified when calling the \ref IRtcEngine::joinChannel "joinChannel" method,
   * the token expires after a certain period of time and you need a new token to reconnect to the server.
   *
   * Upon receiving this callback, generate a new token at your app server and call
   * \ref IRtcEngine::renewToken "renewToken" to pass the new token to the SDK.
   *
   * @sa [How to generate a token](https://docs.agora.io/en/Interactive%20Broadcast/token_server_cpp?platform=CPP).
   */
  virtual void onRequestToken(conn_id_t connId) {
    (void)connId;
  }

  /**
   * Occurs when the token will expire in 30 seconds.
   *
   * If the token you specified when calling \ref IRtcEngine::joinChannel "joinChannel" expires,
   * the user will drop offline. This callback is triggered 30 seconds before the token expires, to
   * remind you to renew the token.

   * Upon receiving this callback, generate a new token at your app server and call
   * \ref IRtcEngine::renewToken "renewToken" to pass the new Token to the SDK.
   *
   * @sa [How to generate a token](https://docs.agora.io/en/Interactive%20Broadcast/token_server_cpp?platform=CPP).
   *
   * @param token The token that will expire in 30 seconds.
   */
  virtual void onTokenPrivilegeWillExpire(conn_id_t connId, const char* token) {
    (void)connId;
    (void)token;
  }

  /** Occurs when the first local audio frame is published.

   @param elapsed The time elapsed (ms) from the local user calling
   \ref IRtcEngine::joinChannel "joinChannel" to the SDK triggers this callback.
   */
  virtual void onFirstLocalAudioFramePublished(conn_id_t connId, int elapsed) {
    (void)connId;
    (void)elapsed;
  }

  /** Occurs when the local audio state changes.
   *
   * This callback indicates the state change of the local audio stream,
   * including the state of the audio recording and encoding, and allows
   * you to troubleshoot issues when exceptions occur.
   *
   * @note
   * When the state is #LOCAL_AUDIO_STREAM_STATE_FAILED (3), see the `error`
   * parameter for details.
   *
   * @param state State of the local audio. See #LOCAL_AUDIO_STREAM_STATE.
   * @param error The error information of the local audio.
   * See #LOCAL_AUDIO_STREAM_ERROR.
   */
  virtual void onLocalAudioStateChanged(conn_id_t connId, LOCAL_AUDIO_STREAM_STATE state, LOCAL_AUDIO_STREAM_ERROR error) {
    (void)connId;
    (void)state;
    (void)error;
  }

  /** Occurs when the remote audio state changes.
   * REMOTE_AUDIO_STATE_REASON_REMOTE_OFFLINE is not 100% guaranteed. When the
   * remote user leaves, use onUserOffline to make a business decision.
   *
   * This callback indicates the state change of the remote audio stream.
   *
   * @param uid ID of the remote user whose audio state changes.
   * @param state State of the remote audio. See #REMOTE_AUDIO_STATE.
   * @param reason The reason of the remote audio state change.
   * See #REMOTE_AUDIO_STATE_REASON.
   * @param elapsed Time elapsed (ms) from the local user calling the
   * \ref IRtcEngine::joinChannel "joinChannel" method until the SDK
   * triggers this callback.
   */
  virtual void onRemoteAudioStateChanged(conn_id_t connId, uid_t uid, REMOTE_AUDIO_STATE state, REMOTE_AUDIO_STATE_REASON reason, int elapsed) {
    (void)connId;
    (void)uid;
    (void)state;
    (void)reason;
    (void)elapsed;
  }

  /** @param [in] uid
   *        the speaker uid who is talking in the channel
   */
  virtual void onActiveSpeaker(conn_id_t connId, uid_t uid) {
    (void)connId;
    (void)uid;
  }

  /**
   * Occurs when the user role in a Live-Broadcast channel has switched, for example, from a broadcaster
   * to an audience or vice versa.
   *
   * @param oldRole The old role of the user: #CLIENT_ROLE_TYPE.
   * @param newRole The new role of the user: #CLIENT_ROLE_TYPE.
   */
  virtual void onClientRoleChanged(conn_id_t connId, CLIENT_ROLE_TYPE oldRole, CLIENT_ROLE_TYPE newRole) {
    (void)connId;
    (void)oldRole;
    (void)newRole;
  }

  /** Occurs when the audio device volume changes.
   @param deviceType The device type, see #MEDIA_DEVICE_TYPE
   @param volume The volume of the audio device.
   @param muted Whether the audio device is muted:
   - true: The audio device is muted.
   - false: The audio device is not muted.
   */
  virtual void onAudioDeviceVolumeChanged(conn_id_t connId, MEDIA_DEVICE_TYPE deviceType, int volume, bool muted) {
    (void)connId;
    (void)deviceType;
    (void)volume;
    (void)muted;
  }

  /**
   * Occurs when the state of the RTMP streaming changes.
   *
   * The SDK triggers this callback to report the result of the local user calling the \ref IRtcEngine::addPublishStreamUrl
   * "addPublishStreamUrl" or \ref IRtcEngine::removePublishStreamUrl "removePublishStreamUrl" method.
   *
   * This callback indicates the state of the RTMP streaming. When exceptions occur, you can
   * troubleshoot issues by referring to the detailed error descriptions in the `errCode` parameter.
   *
   * @param url The RTMP URL address.
   * @param state The RTMP streaming state. See: #RTMP_STREAM_PUBLISH_STATE.
   * @param errCode The detailed error information for streaming: #RTMP_STREAM_PUBLISH_ERROR.
   */
  virtual void onRtmpStreamingStateChanged(conn_id_t connId, const char* url,
                                           RTMP_STREAM_PUBLISH_STATE state,
                                           RTMP_STREAM_PUBLISH_ERROR errCode){
    (void)connId;
    (void)url;
    (void)state;
    (void)errCode;
  }

  virtual void onStreamPublished(conn_id_t connId, const char* url, int error) {
    (void)connId;
    (void)url;
    (void)error;
  }

  /**
   * Reports the result of calling the \ref IRtcEngine::removePublishStreamUrl "removePublishStreamUrl()"
   * method.
   *
   * @deprecated Use \ref onRtmpStreamingStateChanged "onRtmpStreamingStateChanged" instead.
   *
   * This callback indicates whether you have successfully removed an RTMP stream from the CDN.
   *
   * @param url The RTMP URL address.
   */
  virtual void onStreamUnpublished(conn_id_t connId, const char* url) {
    (void)connId;
    (void)url;
  }

  /**
   * Occurs when the publisher's transcoding settings are updated.
   *
   * When the `LiveTranscoding` class in \ref IRtcEngine::setLiveTranscoding "setLiveTranscoding" 
   * updates, the SDK triggers this
   * callback to report the update information.
   *
   * @note
   * If you call the `setLiveTranscoding` method to set the `LiveTranscoding` class for the first time, the SDK
   * does not trigger this callback.
   */
  virtual void onTranscodingUpdated(conn_id_t connId) {
    (void)connId;
  }

  /** Reports the status of injecting the online media stream.

  @param url The URL address of theinjected stream.
  @param uid The user ID.
  @param status Status of the injected stream: #INJECT_STREAM_STATUS.
   */
  virtual void onStreamInjectedStatus(conn_id_t connId, const char* url, uid_t uid, int status) {
    (void)connId;
    (void)url;
    (void)uid;
    (void)status;
  }

  /** Occurs when the local audio route changes (for Android and iOS only).

   The SDK triggers this callback when the local audio route switches to an
   earpiece, speakerphone, headset, or Bluetooth device.
   @param routing Audio output routing:
   - -1: Default.
   - 0: Headset.
   - 1: Earpiece.
   - 2: Headset with no microphone.
   - 3: Speakerphone.
   - 4: Loudspeaker.
   - 5: Bluetooth headset.
    */
  virtual void onAudioRoutingChanged(conn_id_t connId, int routing) {
    (void)connId;
    (void)routing;
  }

  virtual void onChannelMediaRelayStateChanged(conn_id_t connId, int state, int code) {
    (void)connId;
    (void)state;
    (void)code;
  }

  virtual void onChannelMediaRelayEvent(conn_id_t connId, int code) {
    (void)connId;
    (void)code;
  }

  virtual void onLocalPublishFallbackToAudioOnly(conn_id_t connId, bool isFallbackOrRecover) {
    (void)connId;
    (void)isFallbackOrRecover;
  }

  virtual void onRemoteSubscribeFallbackToAudioOnly(conn_id_t connId, uid_t uid, bool isFallbackOrRecover) {
    (void)connId;
    (void)uid;
    (void)isFallbackOrRecover;
  }

  /** Reports the transport-layer statistics of each remote audio stream.

  This callback is triggered every two seconds once the user has received the
  audio data packet sent from a remote user.

  @param uid ID of the remote user whose audio data packet is received.
  @param delay The network time delay (ms) from the remote user sending the
  audio packet to the local user.
  @param lost The Packet loss rate (%) of the audio packet sent from the remote
  user.
  @param rxKBitRate Received bitrate (Kbps) of the audio packet sent from the
  remote user.
   */
  virtual void onRemoteAudioTransportStats(conn_id_t connId, uid_t uid, unsigned short delay, unsigned short lost,
                                           unsigned short rxKBitRate) {
    (void)connId;
    (void)uid;
    (void)delay;
    (void)lost;
    (void)rxKBitRate;
  }

  /** Reports the transport-layer statistics of each remote video stream.

  This callback is triggered every two seconds once the user has received the
  video data packet sent from a remote user.

  @param uid ID of the remote user whose video packet is received.
  @param delay The network time delay (ms) from the remote user sending the
  video packet to the local user.
  @param lost The packet loss rate (%) of the video packet sent from the remote
  user.
  @param rxKBitRate The Received bitrate (Kbps) of the video packet sent from
  the remote user.
   */
  virtual void onRemoteVideoTransportStats(conn_id_t connId, uid_t uid, unsigned short delay, unsigned short lost,
                                           unsigned short rxKBitRate) {
    (void)connId;
    (void)uid;
    (void)delay;
    (void)lost;
    (void)rxKBitRate;
  }

  /** Occurs when the connection state of the SDK to the server is changed.

  @param state See #CONNECTION_STATE_TYPE.
  @param reason See #CONNECTION_CHANGED_REASON_TYPE.
   */
  virtual void onConnectionStateChanged(conn_id_t connId,
                                        CONNECTION_STATE_TYPE state,
                                        CONNECTION_CHANGED_REASON_TYPE reason) {
    (void)connId;
    (void)state;
    (void)reason;
  }

  /** Occurs when the network type is changed.

  @param type See #NETWORK_TYPE.
   */
  virtual void onNetworkTypeChanged(conn_id_t connId, NETWORK_TYPE type) {
    (void)connId;
    (void)type;
  }

  /** Reports the error type of encryption.

  @param type See #ENCRYPTION_ERROR_TYPE.
   */
  virtual void onEncryptionError(conn_id_t connId, ENCRYPTION_ERROR_TYPE errorType) {
    (void)connId;
    (void)errorType;
  }

  virtual void onUserAccountUpdated(uid_t uid, const char* userAccount){
    (void)uid;
    (void)userAccount;
  }

  // TODO(tomiao): to be supported in Stream RTC Event Handler
#if 0
  onLeaveChannel()

  onAudioMixingFinished()
  onRemoteAudioMixingBegin()
  onRemoteAudioMixingEnd()

  onIntraRequestReceived()
  onBandwidthEstimationUpdated()

  onFirstLocalVideoFramePublished()
  onVideoSourceFrameSizeChanged()

  onConnectionLost()
  onConnectionInterrupted()

  onUserMuteVideo()
  onUserEnableVideo()
  onUserEnableLocalVideo()

  onStreamInjectedStatus()

  onChannelMediaRelayEvent()

  onEncryptionError()
#endif  // 0

  // specific to this header
  virtual void onAudioTransportQuality(uid_t uid, unsigned int bitrate, unsigned short delay,
                                       unsigned short lost) {
    (void)uid;
    (void)bitrate;
    (void)delay;
    (void)lost;
  }

  virtual void onVideoTransportQuality(uid_t uid, unsigned int bitrate, unsigned short delay,
                                       unsigned short lost) {
    (void)uid;
    (void)bitrate;
    (void)delay;
    (void)lost;
  }

  virtual void onRecap(const char* recapData, int length) {
    (void)recapData;
    (void)length;
  }

  virtual void onMediaEngineEvent(int evt) { (void)evt; }

  virtual bool onCustomizedSei(const void** content, int* length) {
    (void)content;
    (void)length;

    /* return false to indicate the SEI content is left to SDK to generate */
    return false;
  }

  virtual void onExtensionEvent(const char* id, const char* key, const char* json_value) {
    (void)id;
    (void)key;
    (void)json_value;
  }

#ifdef INTERNAL_ENGINE_STATUS
  virtual void onInternalEngineStatus(InternalEngineStatus state) { (void)state; }
#endif  // INTERNAL_ENGINE_STATUS
};

struct Extension {
  // id of extension
  const char* id;
  // .so/.dll path
  const char* path;
  // extension configuration, e.g. resource path.
  // config should be a json string.
  const char* config;

  Extension() : id(OPTIONAL_NULLPTR), path(OPTIONAL_NULLPTR), config(OPTIONAL_NULLPTR) {}
};

struct RtcEngineContextEx {
  IRtcEngineEventHandlerEx* eventHandlerEx;

  const char* appId;

  // For android, it the context(Activity or Application
  void* context;

  // Used to deprecate enalbeAudio and enableVideo before joinChannel()
  bool enableAudio;
  bool enableVideo;
  unsigned int areaCode;

  bool useStringUid;
  // extensions array.
  Extension* extensions;
  int numExtension;

  RtcEngineContextEx()
      : eventHandlerEx(NULL)
      , appId(NULL)
      , context(NULL)
      , enableAudio(true)
      , enableVideo(false)
      , areaCode(AREA_CODE_GLOB)
      , extensions(OPTIONAL_NULLPTR)
      , numExtension(0) {}
};

class IRtcEngineEx : public IRtcEngine {
public:
    using IRtcEngine::initialize;

    virtual int initialize(const RtcEngineContextEx& context) = 0;

    /**
    * Specify video stream parameters based on video profile
    * @param [in] width
    *        width of video resolution in pixel
    * @param [in] height
    *        height of video resolution in pixel
    * @param [in] frameRate
    *        frame rate in fps
    * @param [in] bitrate
    *        bit rate in kbps
    * @return return 0 if success or an error code
    */
    virtual int setVideoProfileEx(int width, int height, int frameRate, int bitrate) = 0;
};

}  // namespace rtc
}  // namespace agora
