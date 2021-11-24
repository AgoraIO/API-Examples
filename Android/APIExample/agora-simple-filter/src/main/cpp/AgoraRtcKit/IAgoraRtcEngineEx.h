//
//  Agora Media SDK
//
//  Created by Sting Feng in 2015-05.
//  Updated by Tommy Miao in 2020-11.
//  Copyright (c) 2015 Agora IO. All rights reserved.
//
#pragma once

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

/**
 * Rtc Connection.
 */
struct RtcConnection {
  /**
   *  The unique channel name for the AgoraRTC session in the string format. The string
   * length must be less than 64 bytes. Supported character scopes are:
   * - All lowercase English letters: a to z.
   * - All uppercase English letters: A to Z.
   * - All numeric characters: 0 to 9.
   * - The space character.
   * - Punctuation characters and other symbols, including: "!", "#", "$", "%", "&", "(", ")", "+", "-",
   * ":", ";", "<", "=", ".", ">", "?", "@", "[", "]", "^", "_", " {", "}", "|", "~", ",".
   */
  const char* channelId;
  /**
   * User ID: A 32-bit unsigned integer ranging from 1 to (2^32-1). It must be unique.
   */
  uid_t localUid;

  RtcConnection() : channelId(NULL), localUid(0) {}
  RtcConnection(const char* channel_id, uid_t local_uid)
      : channelId(channel_id), localUid(local_uid) {}
};

class IRtcEngineEventHandlerEx : public IRtcEngineEventHandler {
 public:
  using IRtcEngineEventHandler::eventHandlerType;
  using IRtcEngineEventHandler::onJoinChannelSuccess;
  using IRtcEngineEventHandler::onRejoinChannelSuccess;
  using IRtcEngineEventHandler::onAudioQuality;
  using IRtcEngineEventHandler::onAudioVolumeIndication;
  using IRtcEngineEventHandler::onLeaveChannel;
  using IRtcEngineEventHandler::onRtcStats;
  using IRtcEngineEventHandler::onNetworkQuality;
  using IRtcEngineEventHandler::onIntraRequestReceived;
  using IRtcEngineEventHandler::onFirstLocalVideoFrame;
  using IRtcEngineEventHandler::onFirstLocalVideoFramePublished;
  using IRtcEngineEventHandler::onVideoSourceFrameSizeChanged;
  using IRtcEngineEventHandler::onFirstRemoteVideoDecoded;
  using IRtcEngineEventHandler::onVideoSizeChanged;
  using IRtcEngineEventHandler::onLocalVideoStateChanged;
  using IRtcEngineEventHandler::onContentInspectResult;
  using IRtcEngineEventHandler::onSnapshotTaken;
  using IRtcEngineEventHandler::onRemoteVideoStateChanged;
  using IRtcEngineEventHandler::onFirstRemoteVideoFrame;
  using IRtcEngineEventHandler::onUserJoined;
  using IRtcEngineEventHandler::onUserOffline;
  using IRtcEngineEventHandler::onUserMuteAudio;
  using IRtcEngineEventHandler::onUserMuteVideo;
  using IRtcEngineEventHandler::onUserEnableVideo;
  using IRtcEngineEventHandler::onUserEnableLocalVideo;
  using IRtcEngineEventHandler::onLocalAudioStats;
  using IRtcEngineEventHandler::onRemoteAudioStats;
  using IRtcEngineEventHandler::onLocalVideoStats;
  using IRtcEngineEventHandler::onRemoteVideoStats;
  using IRtcEngineEventHandler::onConnectionLost;
  using IRtcEngineEventHandler::onConnectionInterrupted;
  using IRtcEngineEventHandler::onConnectionBanned;
  using IRtcEngineEventHandler::onStreamMessage;
  using IRtcEngineEventHandler::onStreamMessageError;
  using IRtcEngineEventHandler::onRequestToken;
  using IRtcEngineEventHandler::onTokenPrivilegeWillExpire;
  using IRtcEngineEventHandler::onFirstLocalAudioFramePublished;
  using IRtcEngineEventHandler::onLocalAudioStateChanged;
  using IRtcEngineEventHandler::onRemoteAudioStateChanged;
  using IRtcEngineEventHandler::onActiveSpeaker;
  using IRtcEngineEventHandler::onClientRoleChanged;
  using IRtcEngineEventHandler::onRemoteAudioTransportStats;
  using IRtcEngineEventHandler::onRemoteVideoTransportStats;
  using IRtcEngineEventHandler::onConnectionStateChanged;
  using IRtcEngineEventHandler::onNetworkTypeChanged;
  using IRtcEngineEventHandler::onEncryptionError;
  using IRtcEngineEventHandler::onUploadLogResult;
  using IRtcEngineEventHandler::onUserAccountUpdated;
  using IRtcEngineEventHandler::onAudioSubscribeStateChanged;
  using IRtcEngineEventHandler::onVideoSubscribeStateChanged;
  using IRtcEngineEventHandler::onAudioPublishStateChanged;
  using IRtcEngineEventHandler::onVideoPublishStateChanged;

  virtual const char* eventHandlerType() const { return "event_handler_ex"; }

  /**
   * Occurs when the local user successfully joins the specified channel.
   *
   * @param connection The connection of the local user.
   * @param elapsed The time elapsed (ms) from the local user calling `joinChannel` until this event occurs.
   */
  virtual void onJoinChannelSuccess(const RtcConnection& connection, int elapsed) {
    (void)connection;
    (void)elapsed;
  }

  /**
   * Occurs when the local user rejoins the channel after being disconnected due to
   * network problems.
   *
   * When the app loses connection with the server because of network
   * problems, the SDK automatically tries to reconnect to the server, and triggers this
   * callback method upon reconnection.
   *
   * @param connection The connection of the local user.
   * @param elapsed Time elapsed (ms) from starting to reconnect until this callback is triggered.
   */
  virtual void onRejoinChannelSuccess(const RtcConnection& connection, int elapsed) {
    (void)connection;
    (void)elapsed;
  }

  /**
   * when audio quality message come, the function will be called
   * @param [in] connection
   *        the connection of the local user.
   * @param [in] remoteUid
   *        the uid of the peer
   * @param [in] quality
   *        the quality of the remote user, see QUALITY_TYPE for value definition
   * @param [in] delay
   *        the average time of the audio packages delayed
   * @param [in] lost
   *        the rate of the audio packages lost
   */
  virtual void onAudioQuality(const RtcConnection& connection, uid_t remoteUid, int quality, unsigned short delay, unsigned short lost) {
    (void)connection;
    (void)remoteUid;
    (void)quality;
    (void)delay;
    (void)lost;
  }

  virtual void onAudioVolumeIndication(const RtcConnection& connection, const AudioVolumeInfo* speakers,
                                       unsigned int speakerNumber, int totalVolume) {
    (void)connection;
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
   * @param connection The connection of the local user.
   * @param stats The statistics on the call: RtcStats.
   */
  virtual void onLeaveChannel(const RtcConnection& connection, const RtcStats& stats) {
    (void)connection;
    (void)stats;
  }

  /**
   * Reports the statistics of the current call.
   *
   * This callback is triggered once every two seconds after the user joins the channel.
   *
   * @param connection The connection of the local user.
   * @param stats The statistics of the current call: RtcStats.
   */
  virtual void onRtcStats(const RtcConnection& connection, const RtcStats& stats) {
    (void)connection;
    (void)stats;
  }

  /**
   * report the network quality
   * @param [in] connection
   *        the connection of the local user.
   * @param [in] remoteUid
   *        the uid of the remote user
   * @param [in] txQuality
   *        the score of the send network quality 0~5 the higher the better
   * @param [in] rxQuality
   *        the score of the recv network quality 0~5 the higher the better
   */
  virtual void onNetworkQuality(const RtcConnection& connection, uid_t remoteUid, int txQuality, int rxQuality) {
    (void)connection;
    (void)remoteUid;
    (void)txQuality;
    (void)rxQuality;
  }

  /**
   * Occurs when intra request from remote user is received.
   *
   * This callback is triggered once remote user needs one Key frame.
   * @param connection The connection of the local user.
   */
  virtual void onIntraRequestReceived(const RtcConnection& connection) {
    (void)connection;
  }

  /** Occurs when the first local video frame is displayed on the video window.
   @param connection The connection of the local user.
   @param width The width (pixels) of the video stream.
   @param height The height (pixels) of the video stream.
   @param elapsed The time elapsed (ms) from the local user calling
   \ref IRtcEngine::joinChannel "joinChannel" until this callback is triggered.
   */
  virtual void onFirstLocalVideoFrame(const RtcConnection& connection, int width, int height, int elapsed) {
    (void)connection;
    (void)width;
    (void)height;
    (void)elapsed;
  }

  /** Occurs when the first local video frame is published.

   @param elapsed The time elapsed (ms) from the local user calling
  \ref IRtcEngine::joinChannel "joinChannel" to the SDK triggers this callback.
  */
  virtual void onFirstLocalVideoFramePublished(const RtcConnection& connection, int elapsed) {
    (void)connection;
    (void)elapsed;
  }

  virtual void onVideoSourceFrameSizeChanged(const RtcConnection& connection, VIDEO_SOURCE_TYPE sourceType, int width, int height) {
    (void)connection;
    (void)sourceType;
    (void)width;
    (void)height;
  }

  /**
   * when the first remote video frame decoded, the function will be called
   * @param [in] remoteUid
   *        the uid of the remote user
   * @param [in] width
   *        the width of the video frame
   * @param [in] height
   *        the height of the video frame
   * @param [in] elapsed
   *        the time elapsed from channel joined in ms
   */
  virtual void onFirstRemoteVideoDecoded(const RtcConnection& connection, uid_t remoteUid, int width, int height, int elapsed) {
    (void)connection;
    (void)remoteUid;
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
  virtual void onVideoSizeChanged(const RtcConnection& connection, uid_t uid, int width, int height, int rotation) {
    (void)connection;
    (void)uid;
    (void)width;
    (void)height;
    (void)rotation;
  }
      /** Reports result of Content Inspect*/
  virtual void onContentInspectResult(media::CONTENT_INSPECT_RESULT result) { (void)result; }
    /** Occurs when takeSnapshot API result is obtained
   *
   *
   * @brief snapshot taken callback
   *
   * @param channel channel name
   * @param uid user id
   * @param filePath image is saveed file path
   * @param width image width
   * @param height image height
   * @param errCode 0 is ok negative is error
   */
  virtual void onSnapshotTaken(const RtcConnection& connection, const char* filePath, int width, int height, int errCode) {
    (void)connection;
    (void)filePath;
    (void)width;
    (void)height;
    (void)errCode;
  }
  /** Occurs when the local video stream state changes
   *
   * This callback indicates the state of the local video stream, including camera capturing and video encoding,
   * and allows you to troubleshoot issues when exceptions occur.
   *
   * @note For some device models, the SDK will not trigger this callback when the state of the local video changes
   * while the local video capturing device is in use, so you have to make your own timeout judgment.
   * @param connection The connection of the local user.
   * @param state State type #LOCAL_VIDEO_STREAM_STATE. When the state is LOCAL_VIDEO_STREAM_STATE_FAILED (3), see the `error` parameter for details.
   * @param error The detailed error information: #LOCAL_VIDEO_STREAM_ERROR.
   */
  virtual void onLocalVideoStateChanged(const RtcConnection& connection,
                                        LOCAL_VIDEO_STREAM_STATE state,
                                        LOCAL_VIDEO_STREAM_ERROR errorCode) {
    (void)connection;
    (void)state;
    (void)errorCode;
  }

  /**
   * Occurs when the remote video state has changed.
   * REMOTE_VIDEO_STATE_REASON_REMOTE_OFFLINE is not 100% guaranteed. When the
   * remote user leaves, use onUserOffline to make a business decision.
   *
   * @param connection The connection of the local user.
   * @param remoteUid ID of the user whose video state has changed.
   * @param state The remote video state: #REMOTE_VIDEO_STATE.
   * @param reason The reason of the remote video state change: #REMOTE_VIDEO_STATE_REASON.
   * @param elapsed The time elapsed (ms) from the local user calling ref IRtcEngine::joinChannel "joinChannel" until this callback is triggered.
   */
  virtual void onRemoteVideoStateChanged(const RtcConnection& connection, uid_t remoteUid, REMOTE_VIDEO_STATE state, REMOTE_VIDEO_STATE_REASON reason, int elapsed) {
    (void)connection;
    (void)remoteUid;
    (void)state;
    (void)reason;
    (void)elapsed;
  }

  /**
   * when the first remote video frame displayed, the function will be called
   * @param [in] connection
   *        the connection of the local user.
   * @param [in] remoteUid
   *        the uid of the remote user
   * @param [in] width
   *        the width of the video frame
   * @param [in] height
   *        the height of the video frame
   * @param [in] elapsed
   *        the time elapsed from remote user called joinChannel in ms
   */
  virtual void onFirstRemoteVideoFrame(const RtcConnection& connection, uid_t remoteUid, int width, int height, int elapsed) {
    (void)connection;
    (void)remoteUid;
    (void)width;
    (void)height;
    (void)elapsed;
  }

  /**
   * when any other user joined in the same channel, the function will be called
   * @param [in] remoteUid
   *        the uid of the remote user
   * @param [in] elapsed
   *        the time elapsed from remote used called joinChannel to joining completed in ms
   */
  virtual void onUserJoined(const RtcConnection& connection, uid_t remoteUid, int elapsed) {
    (void)connection;
    (void)remoteUid;
    (void)elapsed;
  }

  /**
   * when user offline(exit channel or offline by accident), the function will be called
   * @param [in] connection
   *        the connection of the local user.
   * @param [in] remoteUid
   *        the uid of the remote user
   */
  virtual void onUserOffline(const RtcConnection& connection, uid_t remoteUid, USER_OFFLINE_REASON_TYPE reason) {
    (void)connection;
    (void)remoteUid;
    (void)reason;
  }

  /**
   * when remote user muted the audio stream, the function will be called
   * @deprecated Use onRemoteAudioStateChanged instead of.
   * @param [in] remoteUid
   *        the uid of the remote user
   * @param [in] muted
   *        true: the remote user muted the audio stream, false: the remote user unmuted the audio stream
   */
 virtual void onUserMuteAudio(const RtcConnection& connection, uid_t remoteUid, bool muted) {
    (void)connection;
    (void)remoteUid;
    (void)muted;
  }

  /**
   * when remote user muted the video stream, the function will be called
   * @deprecated Use onRemoteVideoStateChanged instead of.
   * @param [in] remoteUid
   *        the uid of the remote user
   * @param [in] muted
   *        true: the remote user muted the video stream, false: the remote user unmuted the video stream
   */
 virtual void onUserMuteVideo(const RtcConnection& connection, uid_t remoteUid, bool muted) {
    (void)connection;
    (void)remoteUid;
    (void)muted;
  }

  /**
   * when remote user enable video function, the function will be called
   * @deprecated Use onRemoteVideoStateChanged instead of.
   * @param [in] remoteUid
   *        the uid of the remote user
   * @param [in] enabled
   *        true: the remote user has enabled video function, false: the remote user has disabled video function
   */
 virtual void onUserEnableVideo(const RtcConnection& connection, uid_t remoteUid, bool enabled) {
    (void)connection;
    (void)remoteUid;
    (void)enabled;
  }

  /**
   * when remote user enable local video function, the function will be called
   * @deprecated Use onRemoteVideoStateChanged instead of.
   * @param [in] remoteUid
   *        the uid of the remote user
   * @param [in] enabled
   *        true: the remote user has enabled local video function, false: the remote user has disabled local video function
   */
  virtual void onUserEnableLocalVideo(const RtcConnection& connection, uid_t remoteUid, bool enabled) {
    (void)connection;
    (void)remoteUid;
    (void)enabled;
  }

  /** Reports the statistics of the local audio stream.
   *
   * The SDK triggers this callback once every two seconds.
   *
   * @param stats The statistics of the local audio stream.
   * See LocalAudioStats.
   */
  virtual void onLocalAudioStats(const RtcConnection& connection, const LocalAudioStats& stats) {
    (void)connection;
    (void)stats;
  }

  /** Reports the statistics of the audio stream from each remote user/host.

   This callback replaces the \ref IRtcEngineEventHandler::onAudioQuality "onAudioQuality" callback.

   The SDK triggers this callback once every two seconds for each remote
   user/host. If a channel includes multiple remote users, the SDK triggers
   this callback as many times.

   @param stats Statistics of the received remote audio streams. See RemoteAudioStats.
   */
  virtual void onRemoteAudioStats(const RtcConnection& connection, const RemoteAudioStats& stats) {
    (void)connection;
    (void)stats;
  }

  /**
   * Reports the statistics of the local video.
   *
   * The SDK triggers this callback once every two seconds after the user joins the channel.
   *
   * @param stats The statistics of the uploading local video stream: LocalVideoStats.
   */
  virtual void onLocalVideoStats(const RtcConnection& connection, const LocalVideoStats& stats) {
    (void)connection;
    (void)stats;
  }

  /** Reports the statistics of the remote video.
   *
   * The SDK triggers this callback once every two seconds for each remote user or broadcaster. If
   * there are multiple users or broadcasters in the channel, the SDK triggers this callback
   * as many times.
   * @param stats The statistics of the remote video streams: RemoteVideoStats.
   */
  virtual void onRemoteVideoStats(const RtcConnection& connection, const RemoteVideoStats& stats) {
    (void)connection;
    (void)stats;
  }

  /**
   * Occurs when the SDK cannot reconnect to the server 10 seconds after its connection to the server is
   * interrupted.
   *
   * The SDK triggers this callback when it cannot connect to the server 10 seconds after calling
   * \ref IRtcEngine::joinChannel "joinChannel", regardless of whether it is in the channel or not.
   */
  virtual void onConnectionLost(const RtcConnection& connection) {
    (void)connection;
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
  virtual void onConnectionInterrupted(const RtcConnection& connection) {
    (void)connection;
  }

  /** Occurs when your connection is banned by the Agora Server.
   */
  virtual void onConnectionBanned(const RtcConnection& connection) {
    (void)connection;
  }

  /** Occurs when the user receives the data stream.
   *
   * The SDK triggers this callback when the user receives the data stream that another user sends
   * by calling the \ref agora::rtc::IRtcEngine::sendStreamMessage "sendStreamMessage" method
   * within 5 seconds.
   *
   * @param userId ID of the user who sends the data stream.
   * @param streamId The ID of the stream data.
   * @param data The data stream.
   * @param length The length (byte) of the data stream.
   * @param sentTs The time of the data stream sent.
   */

  virtual void onStreamMessage(const RtcConnection& connection, uid_t remoteUid, int streamId, const char* data, size_t length, uint64_t sentTs) {
    (void)connection;
    (void)remoteUid;
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

  @param remoteUid ID of the remote user who sends the data stream.
  @param streamId The stream ID.
  @param code The error code: #ERROR_CODE_TYPE.
  @param missed The number of lost messages.
  @param cached The number of incoming cached messages when the data stream is
  interrupted.
   *
   */
  virtual void onStreamMessageError(const RtcConnection& connection, uid_t remoteUid, int streamId, int code, int missed, int cached) {
    (void)connection;
    (void)remoteUid;
    (void)streamId;
    (void)code;
    (void)missed;
    (void)cached;
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
  virtual void onRequestToken(const RtcConnection& connection) {
    (void)connection;
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
  virtual void onTokenPrivilegeWillExpire(const RtcConnection& connection, const char* token) {
    (void)connection;
    (void)token;
  }

  /** Occurs when the first local audio frame is published.

   @param elapsed The time elapsed (ms) from the local user calling
   \ref IRtcEngine::joinChannel "joinChannel" to the SDK triggers this callback.
   */
  virtual void onFirstLocalAudioFramePublished(const RtcConnection& connection, int elapsed) {
    (void)connection;
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
  virtual void onLocalAudioStateChanged(const RtcConnection& connection, LOCAL_AUDIO_STREAM_STATE state, LOCAL_AUDIO_STREAM_ERROR error) {
    (void)connection;
    (void)state;
    (void)error;
  }

  /** Occurs when the remote audio state changes.
   * REMOTE_AUDIO_STATE_REASON_REMOTE_OFFLINE is not 100% guaranteed. When the
   * remote user leaves, use onUserOffline to make a business decision.
   *
   * This callback indicates the state change of the remote audio stream.
   *
   * @param remoteUid ID of the remote user whose audio state changes.
   * @param state State of the remote audio. See #REMOTE_AUDIO_STATE.
   * @param reason The reason of the remote audio state change.
   * See #REMOTE_AUDIO_STATE_REASON.
   * @param elapsed Time elapsed (ms) from the local user calling the
   * \ref IRtcEngine::joinChannel "joinChannel" method until the SDK
   * triggers this callback.
   */
  virtual void onRemoteAudioStateChanged(const RtcConnection& connection, uid_t remoteUid, REMOTE_AUDIO_STATE state, REMOTE_AUDIO_STATE_REASON reason, int elapsed) {
    (void)connection;
    (void)remoteUid;
    (void)state;
    (void)reason;
    (void)elapsed;
  }

  /** @param [in] uid
   *        the speaker uid who is talking in the channel
   */
  virtual void onActiveSpeaker(const RtcConnection& connection, uid_t uid) {
    (void)connection;
    (void)uid;
  }

  /**
   * Occurs when the user role in a Live-Broadcast channel has switched, for example, from a broadcaster
   * to an audience or vice versa.
   *
   * @param oldRole The old role of the user: #CLIENT_ROLE_TYPE.
   * @param newRole The new role of the user: #CLIENT_ROLE_TYPE.
   */
  virtual void onClientRoleChanged(const RtcConnection& connection, CLIENT_ROLE_TYPE oldRole, CLIENT_ROLE_TYPE newRole) {
    (void)connection;
    (void)oldRole;
    (void)newRole;
  }

  /** Reports the transport-layer statistics of each remote audio stream.

  This callback is triggered every two seconds once the user has received the
  audio data packet sent from a remote user.

  @param remoteUid ID of the remote user whose audio data packet is received.
  @param delay The network time delay (ms) from the remote user sending the
  audio packet to the local user.
  @param lost The Packet loss rate (%) of the audio packet sent from the remote
  user.
  @param rxKBitRate Received bitrate (Kbps) of the audio packet sent from the
  remote user.
   */
  virtual void onRemoteAudioTransportStats(const RtcConnection& connection, uid_t remoteUid, unsigned short delay, unsigned short lost,
                                           unsigned short rxKBitRate) {
    (void)connection;
    (void)remoteUid;
    (void)delay;
    (void)lost;
    (void)rxKBitRate;
  }

  /** Reports the transport-layer statistics of each remote video stream.

  This callback is triggered every two seconds once the user has received the
  video data packet sent from a remote user.

  @param remoteUid ID of the remote user whose video packet is received.
  @param delay The network time delay (ms) from the remote user sending the
  video packet to the local user.
  @param lost The packet loss rate (%) of the video packet sent from the remote
  user.
  @param rxKBitRate The Received bitrate (Kbps) of the video packet sent from
  the remote user.
   */
  virtual void onRemoteVideoTransportStats(const RtcConnection& connection, uid_t remoteUid, unsigned short delay, unsigned short lost,
                                           unsigned short rxKBitRate) {
    (void)connection;
    (void)remoteUid;
    (void)delay;
    (void)lost;
    (void)rxKBitRate;
  }

  /** Occurs when the connection state of the SDK to the server is changed.

  @param state See #CONNECTION_STATE_TYPE.
  @param reason See #CONNECTION_CHANGED_REASON_TYPE.
   */
  virtual void onConnectionStateChanged(const RtcConnection& connection,
                                        CONNECTION_STATE_TYPE state,
                                        CONNECTION_CHANGED_REASON_TYPE reason) {
    (void)connection;
    (void)state;
    (void)reason;
  }

  /** Occurs when the network type is changed.

  @param type See #NETWORK_TYPE.
   */
  virtual void onNetworkTypeChanged(const RtcConnection& connection, NETWORK_TYPE type) {
    (void)connection;
    (void)type;
  }

  /** Reports the error type of encryption.

  @param type See #ENCRYPTION_ERROR_TYPE.
   */
  virtual void onEncryptionError(const RtcConnection& connection, ENCRYPTION_ERROR_TYPE errorType) {
    (void)connection;
    (void)errorType;
  }
  /**
   * Reports the user log upload result
   * @param requestId RequestId of the upload
   * @param success Is upload success
   * @param reason Reason of the upload, 0: OK, 1 Network Error, 2 Server Error.
   */
  virtual void onUploadLogResult(const RtcConnection& connection, const char* requestId, bool success, UPLOAD_ERROR_REASON reason) {
    (void)connection;
    (void)requestId;
    (void)success;
    (void)reason;
  }

  virtual void onUserAccountUpdated(const RtcConnection& connection, uid_t remoteUid, const char* userAccount){
    (void)connection;
    (void)remoteUid;
    (void)userAccount;
  }
};

class IRtcEngineEx : public IRtcEngine {
public:
    /**
     * Joins a channel with the connection ID.
     *
     * You can call this method multiple times to join more than one channels at a time.
     *
     * @note
     * - If you are already in a channel, you cannot rejoin it with the same uid.
     * - We recommend using different user IDs for different channels.
     * - If you want to join the same channel from different devices, ensure that the user IDs in all devices are different.
     * - Ensure that the app ID you use to generate the token is the same with the app ID used when creating the RtcEngine instance.
     * @param token The token generated at your server:
     * - In situations not requiring high security: You can use the temporary token generated at Console. For details, see [Get a temporary token](https://docs.agora.io/en/Agora%20Platform/token?platform=All%20Platforms#get-a-temporary-token).
     * - In situations requiring high security: Set it as the token generated at your server. For details, see [Generate a token](https://docs.agora.io/en/Interactive%20Broadcast/token_server_cpp?platform=CPP).
     * @param channelId The unique channel name for the AgoraRTC session in the string format. The string length must be less than 64 bytes. Supported character scopes are:
     * - All lowercase English letters: a to z.
     * - All uppercase English letters: A to Z.
     * - All numeric characters: 0 to 9.
     * - The space character.
     * - Punctuation characters and other symbols, including: "!", "#", "$", "%", "&", "(", ")", "+", "-", ":", ";", "<", "=", ".", ">", "?", "@", "[", "]", "^", "_", " {", "}", "|", "~", ",".
     * @param uid The user ID. A 32-bit unsigned integer with a value ranging from 1 to (2^32-1).
     * @param options The channel media options: ChannelMediaOptions.
     * @param eventHandler The pointer to the IRtcEngine event handler: IRtcEngineEventHandler.
     * you join the same channel multiple times.
     *
     * @return
     * - 0: Success.
     * - < 0: Failure.
     */
    virtual int joinChannelEx(const char* token, const RtcConnection& connection,
                              const ChannelMediaOptions& options,
                              IRtcEngineEventHandler* eventHandler) = 0;

    /**
     * Leaves the channel with the connection ID.
     *
     * @param channelId The channel name.
     * @param localUid The user ID.
     * @return
     * - 0: Success.
     * - < 0: Failure.
     */
    virtual int leaveChannelEx(const RtcConnection& connection) = 0;

    /**
     *  Updates the channel media options after joining the channel.
     *
     * @param options The channel media options: ChannelMediaOptions.
     * @param channelId The channel name.
     * @param uid The user ID.
     * @return
     * - 0: Success.
     * - < 0: Failure.
     */
    virtual int updateChannelMediaOptionsEx(const ChannelMediaOptions& options, const RtcConnection& connection) = 0;

    virtual int setVideoEncoderConfigurationEx(const VideoEncoderConfiguration& config, const RtcConnection& connection) = 0;

    virtual int setupRemoteVideoEx(const VideoCanvas& canvas, const RtcConnection& connection) = 0;

    virtual int muteRemoteAudioStreamEx(uid_t remoteUid, bool mute, const RtcConnection& connection) = 0;

    virtual int muteRemoteVideoStreamEx(uid_t remoteUid, bool mute, const RtcConnection& connection) = 0;

    virtual int setRemoteVideoStreamTypeEx(uid_t remoteUid, VIDEO_STREAM_TYPE streamType, const RtcConnection& connection) = 0;

    virtual int setRemoteVoicePositionEx(uid_t remoteUid, double pan, double gain, const RtcConnection& connection) = 0;

    virtual int setRemoteRenderModeEx(uid_t remoteUid, media::base::RENDER_MODE_TYPE renderMode,
                                      VIDEO_MIRROR_MODE_TYPE mirrorMode, const RtcConnection& connection) = 0;

    virtual int enableLoopbackRecordingEx(bool enabled, const RtcConnection& connection) = 0;

    virtual CONNECTION_STATE_TYPE getConnectionStateEx(const RtcConnection& connection) = 0;

    virtual int enableEncryptionEx(const RtcConnection& connection, bool enabled, const EncryptionConfig& config) = 0;

    virtual int createDataStreamEx(int* streamId, bool reliable, bool ordered, const RtcConnection& connection) = 0;

    virtual int createDataStreamEx(int* streamId, DataStreamConfig& config, const RtcConnection& connection) = 0;

    virtual int sendStreamMessageEx(int streamId, const char* data, size_t length, const RtcConnection& connection) = 0;

    virtual int addVideoWatermarkEx(const char* watermarkUrl, const WatermarkOptions& options, const RtcConnection& connection) = 0;

    virtual int clearVideoWatermarkEx(const RtcConnection& connection) = 0;

    virtual int sendCustomReportMessageEx(const char* id, const char* category, const char* event, const char* label,
                                          int value, const RtcConnection& connection) = 0;

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
