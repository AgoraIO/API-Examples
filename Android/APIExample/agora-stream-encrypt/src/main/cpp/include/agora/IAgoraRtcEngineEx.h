//
//  Agora Media SDK
//
//  Created by Sting Feng in 2015-05.
//  Updated by Tommy Miao in 2020-11.
//  Copyright (c) 2015 Agora IO. All rights reserved.
//
#pragma once

#include "IAgoraRtcEngine.h"

namespace agora {
namespace rtc {

// OPTIONAL_ENUM_CLASS RTC_EVENT;

/**
 * @brief Contains connection information.
 */
struct RtcConnection {
  /**
   * The channel name.
   */
  const char* channelId;
  /**
   * The ID of the local user.
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
  using IRtcEngineEventHandler::onFirstLocalVideoFramePublished;
  using IRtcEngineEventHandler::onFirstRemoteVideoDecoded;
  using IRtcEngineEventHandler::onVideoSizeChanged;
  using IRtcEngineEventHandler::onLocalVideoStateChanged;
  using IRtcEngineEventHandler::onRemoteVideoStateChanged;
  using IRtcEngineEventHandler::onFirstRemoteVideoFrame;
  using IRtcEngineEventHandler::onUserJoined;
  using IRtcEngineEventHandler::onUserOffline;
  using IRtcEngineEventHandler::onUserMuteAudio;
  using IRtcEngineEventHandler::onUserMuteVideo;
  using IRtcEngineEventHandler::onUserEnableVideo;
  using IRtcEngineEventHandler::onUserEnableLocalVideo;
  using IRtcEngineEventHandler::onUserStateChanged;
  using IRtcEngineEventHandler::onLocalAudioStats;
  using IRtcEngineEventHandler::onRemoteAudioStats;
  using IRtcEngineEventHandler::onLocalVideoStats;
  using IRtcEngineEventHandler::onRemoteVideoStats;
  using IRtcEngineEventHandler::onConnectionLost;
  using IRtcEngineEventHandler::onConnectionInterrupted;
  using IRtcEngineEventHandler::onConnectionBanned;
  using IRtcEngineEventHandler::onStreamMessage;
  using IRtcEngineEventHandler::onStreamMessageError;
  using IRtcEngineEventHandler::onRdtMessage;
  using IRtcEngineEventHandler::onRdtStateChanged;
  using IRtcEngineEventHandler::onMediaControlMessage;
  using IRtcEngineEventHandler::onRequestToken;
  using IRtcEngineEventHandler::onTokenPrivilegeWillExpire;
  using IRtcEngineEventHandler::onLicenseValidationFailure;
  using IRtcEngineEventHandler::onFirstLocalAudioFramePublished;
  using IRtcEngineEventHandler::onFirstRemoteAudioFrame;
  using IRtcEngineEventHandler::onFirstRemoteAudioDecoded;
  using IRtcEngineEventHandler::onLocalAudioStateChanged;
  using IRtcEngineEventHandler::onRemoteAudioStateChanged;
  using IRtcEngineEventHandler::onActiveSpeaker;
  using IRtcEngineEventHandler::onClientRoleChanged;
  using IRtcEngineEventHandler::onClientRoleChangeFailed;
  using IRtcEngineEventHandler::onRemoteAudioTransportStats;
  using IRtcEngineEventHandler::onRemoteVideoTransportStats;
  using IRtcEngineEventHandler::onConnectionStateChanged;
  using IRtcEngineEventHandler::onNetworkTypeChanged;
  using IRtcEngineEventHandler::onEncryptionError;
  using IRtcEngineEventHandler::onUploadLogResult;
  using IRtcEngineEventHandler::onUserInfoUpdated;
  using IRtcEngineEventHandler::onUserAccountUpdated;
  using IRtcEngineEventHandler::onAudioSubscribeStateChanged;
  using IRtcEngineEventHandler::onVideoSubscribeStateChanged;
  using IRtcEngineEventHandler::onAudioPublishStateChanged;
  using IRtcEngineEventHandler::onVideoPublishStateChanged;
  using IRtcEngineEventHandler::onSnapshotTaken;
  using IRtcEngineEventHandler::onVideoRenderingTracingResult;
  using IRtcEngineEventHandler::onSetRtmFlagResult;
  using IRtcEngineEventHandler::onTranscodedStreamLayoutInfo;
  using IRtcEngineEventHandler::onAudioMetadataReceived;
  using IRtcEngineEventHandler::onMultipathStats;
  using IRtcEngineEventHandler::onRenewTokenResult;

  virtual const char* eventHandlerType() const { return "event_handler_ex"; }

  /**
   * Occurs when a user joins a channel.
   *
   * This callback notifies the application that a user joins a specified channel.
   *
   * @param connection The RtcConnection object.
   * @param elapsed The time elapsed (ms) from the local user calling joinChannel until the SDK triggers this callback.
   */
  virtual void onJoinChannelSuccess(const RtcConnection& connection, int elapsed) {
    (void)connection;
    (void)elapsed;
  }

  /**
   * Occurs when a user rejoins the channel.
   *
   * When a user loses connection with the server because of network problems, the SDK automatically tries to reconnect
   * and triggers this callback upon reconnection.
   *
   * @param connection The RtcConnection object.
   * @param elapsed Time elapsed (ms) from the local user calling the joinChannel method until this callback is triggered.
   */
  virtual void onRejoinChannelSuccess(const RtcConnection& connection, int elapsed) {
    (void)connection;
    (void)elapsed;
  }

  /** Reports the statistics of the audio stream from each remote
  user/broadcaster.

  @deprecated This callback is deprecated. Use onRemoteAudioStats instead.

  The SDK triggers this callback once every two seconds to report the audio
  quality of each remote user/host sending an audio stream. If a channel has
  multiple remote users/hosts sending audio streams, the SDK triggers this
  callback as many times.

  @param connection The RtcConnection object.
  @param remoteUid The user ID of the remote user sending the audio stream.
  @param quality The audio quality of the user: #QUALITY_TYPE
  @param delay The network delay (ms) from the sender to the receiver, including the delay caused by audio sampling pre-processing, network transmission, and network jitter buffering.
  @param lost The audio packet loss rate (%) from the sender to the receiver.
  */
  virtual void onAudioQuality(const RtcConnection& connection, uid_t remoteUid, int quality, unsigned short delay, unsigned short lost) __deprecated {
    (void)connection;
    (void)remoteUid;
    (void)quality;
    (void)delay;
    (void)lost;
  }
  /**
   * Reports the volume information of users.
   *
   * By default, this callback is disabled. You can enable it by calling `enableAudioVolumeIndication`. Once this
   * callback is enabled and users send streams in the channel, the SDK triggers the `onAudioVolumeIndication`
   * callback at the time interval set in `enableAudioVolumeIndication`. The SDK triggers two independent
   * `onAudioVolumeIndication` callbacks simultaneously, which separately report the volume information of the
   * local user who sends a stream and the remote users (up to three) whose instantaneous volume is the highest.
   *
   * @note After you enable this callback, calling muteLocalAudioStream affects the SDK's behavior as follows:
   * - If the local user stops publishing the audio stream, the SDK stops triggering the local user's callback.
   * - 20 seconds after a remote user whose volume is one of the three highest stops publishing the audio stream,
   * the callback excludes this user's information; 20 seconds after all remote users stop publishing audio streams,
   * the SDK stops triggering the callback for remote users.
   *
   * @param connection The RtcConnection object.
   * @param speakers The volume information of the users, see AudioVolumeInfo. An empty `speakers` array in the
   * callback indicates that no remote user is in the channel or sending a stream at the moment.
   * @param speakerNumber The total number of speakers.
   * - In the local user's callback, when the local user sends a stream, `speakerNumber` is 1.
   * - In the callback for remote users, the value range of speakerNumber is [0,3]. If the number of remote users who
   * send streams is greater than or equal to three, the value of `speakerNumber` is 3.
   * @param totalVolume The volume of the speaker. The value ranges between 0 (lowest volume) and 255 (highest volume).
   * - In the local user's callback, `totalVolume` is the volume of the local user who sends a stream.
   * - In the remote users' callback, `totalVolume` is the sum of all remote users (up to three) whose instantaneous
   * volume is the highest. If the user calls `startAudioMixing`, `totalVolume` is the volume after audio mixing.
   */
  virtual void onAudioVolumeIndication(const RtcConnection& connection, const AudioVolumeInfo* speakers,
                                       unsigned int speakerNumber, int totalVolume) {
    (void)connection;
    (void)speakers;
    (void)speakerNumber;
    (void)totalVolume;
  }

  /**
   * Occurs when a user leaves a channel.
   *
   * This callback notifies the app that the user leaves the channel by calling `leaveChannel`. From this callback,
   * the app can get information such as the call duration and quality statistics.
   *
   * @param connection The RtcConnection object.
   * @param stats The statistics on the call: RtcStats.
   */
  virtual void onLeaveChannel(const RtcConnection& connection, const RtcStats& stats) {
    (void)connection;
    (void)stats;
  }

  /**
   * Reports the statistics of the current call.
   *
   * The SDK triggers this callback once every two seconds after the user joins the channel.
   *
   * @param connection The RtcConnection object.
   * @param stats The statistics of the current call: RtcStats.
   */
  virtual void onRtcStats(const RtcConnection& connection, const RtcStats& stats) {
    (void)connection;
    (void)stats;
  }

  /**
   * Reports the last mile network quality of each user in the channel.
   *
   * This callback reports the last mile network conditions of each user in the channel. Last mile refers to the
   * connection between the local device and Agora's edge server.
   *
   * The SDK triggers this callback once every two seconds. If a channel includes multiple users, the SDK triggers
   * this callback as many times.
   *
   * @note `txQuality` is UNKNOWN when the user is not sending a stream; `rxQuality` is UNKNOWN when the user is not
   * receiving a stream.
   *
   * @param connection The RtcConnection object.
   * @param remoteUid The user ID. The network quality of the user with this user ID is reported.
   * @param txQuality Uplink network quality rating of the user in terms of the transmission bit rate, packet loss rate,
   * average RTT (Round-Trip Time) and jitter of the uplink network. This parameter is a quality rating helping you
   * understand how well the current uplink network conditions can support the selected video encoder configuration.
   * For example, a 1000 Kbps uplink network may be adequate for video frames with a resolution of 640 × 480 and a frame
   * rate of 15 fps in the LIVE_BROADCASTING profile, but may be inadequate for resolutions higher than 1280 × 720.
   * See #QUALITY_TYPE.
   * @param rxQuality Downlink network quality rating of the user in terms of packet loss rate, average RTT, and jitter
   * of the downlink network. See #QUALITY_TYPE.
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
   *
   * @param connection The RtcConnection object.
   */
  virtual void onIntraRequestReceived(const RtcConnection& connection) {
    (void)connection;
  }

  /** Occurs when the first local video frame is published.
   * The SDK triggers this callback under one of the following circumstances:
   * - The local client enables the video module and calls `joinChannel` successfully.
   * - The local client calls `muteLocalVideoStream(true)` and muteLocalVideoStream(false) in sequence.
   * - The local client calls `disableVideo` and `enableVideo` in sequence.
   * - The local client calls `pushVideoFrame` to successfully push the video frame to the SDK.
   *
   * @param connection The RtcConnection object.
   * @param elapsed The time elapsed (ms) from the local user calling joinChannel` to the SDK triggers
   * this callback.
  */
  virtual void onFirstLocalVideoFramePublished(const RtcConnection& connection, int elapsed) {
    (void)connection;
    (void)elapsed;
  }

  /** Occurs when the first remote video frame is received and decoded.

  The SDK triggers this callback under one of the following circumstances:
  - The remote user joins the channel and sends the video stream.
  - The remote user stops sending the video stream and re-sends it after 15 seconds. Reasons for such an interruption include:
   - The remote user leaves the channel.
   - The remote user drops offline.
   - The remote user calls `muteLocalVideoStream` to stop sending the video stream.
   - The remote user calls `disableVideo` to disable video.

  @param connection The RtcConnection object.
  @param remoteUid The user ID of the remote user sending the video stream.
  @param width The width (pixels) of the video stream.
  @param height The height (pixels) of the video stream.
  @param elapsed The time elapsed (ms) from the local user calling `joinChannel`
  until the SDK triggers this callback.
  */
  virtual void onFirstRemoteVideoDecoded(const RtcConnection& connection, uid_t remoteUid, int width, int height, int elapsed) {
    (void)connection;
    (void)remoteUid;
    (void)width;
    (void)height;
    (void)elapsed;
  }

  /**
   * Occurs when the local or remote video size or rotation has changed.
   *
   * @param connection The RtcConnection object.
   * @param sourceType The video source type: #VIDEO_SOURCE_TYPE.
   * @param uid The user ID. 0 indicates the local user.
   * @param width The new width (pixels) of the video.
   * @param height The new height (pixels) of the video.
   * @param rotation The rotation information of the video.
   */
  virtual void onVideoSizeChanged(const RtcConnection& connection, VIDEO_SOURCE_TYPE sourceType, uid_t uid, int width, int height, int rotation) {
    (void)connection;
    (void)sourceType;
    (void)uid;
    (void)width;
    (void)height;
    (void)rotation;
  }

  /**
   * Occurs when the remote video state changes.
   *
   * @note This callback does not work properly when the number of users (in the voice/video call
   * channel) or hosts (in the live streaming channel) in the channel exceeds 17.
   *
   * @param connection The RtcConnection object.
   * @param remoteUid The ID of the user whose video state has changed.
   * @param state The remote video state: #REMOTE_VIDEO_STATE.
   * @param reason The reason of the remote video state change: #REMOTE_VIDEO_STATE_REASON.
   * @param elapsed The time elapsed (ms) from the local client calling `joinChannel` until this callback is triggered.
   */
  virtual void onRemoteVideoStateChanged(const RtcConnection& connection, uid_t remoteUid, REMOTE_VIDEO_STATE state, REMOTE_VIDEO_STATE_REASON reason, int elapsed) {
    (void)connection;
    (void)remoteUid;
    (void)state;
    (void)reason;
    (void)elapsed;
  }

  /** Occurs when the renderer receives the first frame of the remote video.
   *
   * @param connection The RtcConnection object.
   * @param remoteUid The user ID of the remote user sending the video stream.
   * @param width The width (px) of the video frame.
   * @param height The height (px) of the video stream.
   * @param elapsed The time elapsed (ms) from the local user calling `joinChannel` until the SDK triggers this callback.
   */
  virtual void onFirstRemoteVideoFrame(const RtcConnection& connection, uid_t remoteUid, int width, int height, int elapsed) {
    (void)connection;
    (void)remoteUid;
    (void)width;
    (void)height;
    (void)elapsed;
  }

  /**
   * Occurs when a remote user or broadcaster joins the channel.
   *
   * - In the COMMUNICATION channel profile, this callback indicates that a remote user joins the channel.
   * The SDK also triggers this callback to report the existing users in the channel when a user joins the
   * channel.
   * In the LIVE_BROADCASTING channel profile, this callback indicates that a host joins the channel. The
   * SDK also triggers this callback to report the existing hosts in the channel when a host joins the
   * channel. Agora recommends limiting the number of hosts to 17.
   *
   * The SDK triggers this callback under one of the following circumstances:
   * - A remote user/host joins the channel by calling the `joinChannel` method.
   * - A remote user switches the user role to the host after joining the channel.
   * - A remote user/host rejoins the channel after a network interruption.
   *
   * @param connection The RtcConnection object.
   * @param remoteUid The ID of the remote user or broadcaster joining the channel.
   * @param elapsed The time elapsed (ms) from the local user calling `joinChannel` or `setClientRole`
   * until this callback is triggered.
  */
  virtual void onUserJoined(const RtcConnection& connection, uid_t remoteUid, int elapsed) {
    (void)connection;
    (void)remoteUid;
    (void)elapsed;
  }

  /**
   * Occurs when a remote user or broadcaster goes offline.
   *
   * There are two reasons for a user to go offline:
   * - Leave the channel: When the user leaves the channel, the user sends a goodbye message. When this
   * message is received, the SDK determines that the user leaves the channel.
   * - Drop offline: When no data packet of the user is received for a certain period of time, the SDK assumes
   * that the user drops offline. A poor network connection may lead to false detection, so we recommend using
   * the RTM SDK for reliable offline detection.
   * - The user switches the user role from a broadcaster to an audience.
   *
   * @param connection The RtcConnection object.
   * @param remoteUid The ID of the remote user or broadcaster who leaves the channel or drops offline.
   * @param reason The reason why the remote user goes offline: #USER_OFFLINE_REASON_TYPE.
   */
  virtual void onUserOffline(const RtcConnection& connection, uid_t remoteUid, USER_OFFLINE_REASON_TYPE reason) {
    (void)connection;
    (void)remoteUid;
    (void)reason;
  }

  /** Occurs when a remote user's audio stream playback pauses/resumes.
   * The SDK triggers this callback when the remote user stops or resumes sending the audio stream by
   * calling the `muteLocalAudioStream` method.
   * @note This callback can be inaccurate when the number of users (in the `COMMUNICATION` profile) or hosts (in the `LIVE_BROADCASTING` profile) in the channel exceeds 17.
   *
   * @param connection The RtcConnection object.
   * @param remoteUid The user ID.
   * @param muted Whether the remote user's audio stream is muted/unmuted:
   * - true: Muted.
   * - false: Unmuted.
   */
 virtual void onUserMuteAudio(const RtcConnection& connection, uid_t remoteUid, bool muted) __deprecated {
    (void)connection;
    (void)remoteUid;
    (void)muted;
  }

  /** Occurs when a remote user pauses or resumes sending the video stream.
   *
   * When a remote user calls `muteLocalVideoStream` to stop or resume publishing the video stream, the
   * SDK triggers this callback to report the state of the remote user's publishing stream to the local
   * user.
   *
   * @note This callback can be inaccurate when the number of users or broadacasters in a
   * channel exceeds 20.
   *
   * @param connection The RtcConnection object.
   * @param remoteUid ID of the remote user.
   * @param muted Whether the remote user stops publishing the video stream:
   * - true: The remote user has paused sending the video stream.
   * - false: The remote user has resumed sending the video stream.
   */
 virtual void onUserMuteVideo(const RtcConnection& connection, uid_t remoteUid, bool muted) {
    (void)connection;
    (void)remoteUid;
    (void)muted;
  }

  /** Occurs when a remote user enables or disables the video module.

  Once the video function is disabled, the users cannot see any video.

  The SDK triggers this callback when a remote user enables or disables the video module by calling the
  `enableVideo` or `disableVideo` method.

  @param connection The RtcConnection object.
  @param remoteUid The ID of the remote user.
  @param enabled Whether the video of the remote user is enabled:
  - true: The remote user has enabled video.
  - false: The remote user has disabled video.
  */
 virtual void onUserEnableVideo(const RtcConnection& connection, uid_t remoteUid, bool enabled) {
    (void)connection;
    (void)remoteUid;
    (void)enabled;
  }

  /** Occurs when a remote user enables or disables local video capturing.

  The SDK triggers this callback when the remote user resumes or stops capturing the video stream by
  calling the `enableLocalVideo` method.

  @param connection The RtcConnection object.
  @param remoteUid The ID of the remote user.
  @param enabled Whether the specified remote user enables/disables local video:
  - `true`: The remote user has enabled local video capturing.
  - `false`: The remote user has disabled local video capturing.
  */
  virtual void onUserEnableLocalVideo(const RtcConnection& connection, uid_t remoteUid, bool enabled) __deprecated {
    (void)connection;
    (void)remoteUid;
    (void)enabled;
  }

  /**
   * Occurs when the remote user state is updated.
   *
   * @param connection The RtcConnection object.
   * @param remoteUid The uid of the remote user.
   * @param state The remote user state: #REMOTE_USER_STATE.
   */
  virtual void onUserStateChanged(const RtcConnection& connection, uid_t remoteUid, uint32_t state) {
    (void)connection;
    (void)remoteUid;
    (void)state;
  }

  /** Reports the statistics of the local audio stream.
   *
   * The SDK triggers this callback once every two seconds.
   *
   * @param connection The RtcConnection object.
   * @param stats The statistics of the local audio stream.
   * See LocalAudioStats.
   */
  virtual void onLocalAudioStats(const RtcConnection& connection, const LocalAudioStats& stats) {
    (void)connection;
    (void)stats;
  }

  /** Reports the statistics of the audio stream from each remote user/host.

   The SDK triggers this callback once every two seconds for each remote user who is sending audio
   streams. If a channel includes multiple remote users, the SDK triggers this callback as many times.
   @param connection The RtcConnection object.
   @param stats Statistics of the received remote audio streams. See RemoteAudioStats.
   */
  virtual void onRemoteAudioStats(const RtcConnection& connection, const RemoteAudioStats& stats) {
    (void)connection;
    (void)stats;
  }

  /** Reports the statistics of the local video stream.
   *
   * The SDK triggers this callback once every two seconds for each
   * user/host. If there are multiple users/hosts in the channel, the SDK
   * triggers this callback as many times.
   *
   * @note If you have called the `enableDualStreamMode`
   * method, this callback reports the statistics of the high-video
   * stream (high bitrate, and high-resolution video stream).
   *
   * @param connection The RtcConnection object.
   * @param sourceType The video source type: #VIDEO_SOURCE_TYPE.
   * @param stats Statistics of the local video stream. See LocalVideoStats.
   */
  virtual void onLocalVideoStats(const RtcConnection& connection, VIDEO_SOURCE_TYPE sourceType, const LocalVideoStats& stats) {
    (void)connection;
    (void)stats;
  }

  /** Reports the statistics of the video stream from each remote user/host.
   *
   * The SDK triggers this callback once every two seconds for each remote user. If a channel has
   * multiple users/hosts sending video streams, the SDK triggers this callback as many times.
   *
   * @param connection The RtcConnection object.
   * @param stats Statistics of the remote video stream. See
   * RemoteVideoStats.
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
   * `joinChannel`, regardless of whether it is in the channel or not. If the SDK fails to rejoin
   * the channel 20 minutes after being disconnected from Agora's edge server, the SDK stops rejoining the channel.
   *
   * @param connection The RtcConnection object.
   */
  virtual void onConnectionLost(const RtcConnection& connection) {
    (void)connection;
  }

  /** Occurs when the connection between the SDK and the server is interrupted.
   * @deprecated Use `onConnectionStateChanged` instead.

  The SDK triggers this callback when it loses connection with the serer for more
  than 4 seconds after the connection is established. After triggering this
  callback, the SDK tries to reconnect to the server. If the reconnection fails
  within a certain period (10 seconds by default), the onConnectionLost()
  callback is triggered. If the SDK fails to rejoin the channel 20 minutes after
  being disconnected from Agora's edge server, the SDK stops rejoining the channel.

  @param connection The RtcConnection object.

  */
  virtual void onConnectionInterrupted(const RtcConnection& connection) __deprecated {
    (void)connection;
  }

  /** Occurs when your connection is banned by the Agora Server.
   *
   * @param connection The RtcConnection object.
   */
  virtual void onConnectionBanned(const RtcConnection& connection) {
    (void)connection;
  }

  /** Occurs when the local user receives the data stream from the remote user.
   *
   * The SDK triggers this callback when the user receives the data stream that another user sends
   * by calling the \ref agora::rtc::IRtcEngine::sendStreamMessage "sendStreamMessage" method.
   *
   * @param connection The RtcConnection object.
   * @param remoteUid ID of the user who sends the data stream.
   * @param streamId The ID of the stream data.
   * @param data The data stream.
   * @param length The length (byte) of the data stream.
   * @param sentTs The time when the data stream sent.
   */
  virtual void onStreamMessage(const RtcConnection& connection, uid_t remoteUid, int streamId, const char* data, size_t length, uint64_t sentTs) {
    (void)connection;
    (void)remoteUid;
    (void)streamId;
    (void)data;
    (void)length;
    (void)sentTs;
  }

  /** Occurs when the local user does not receive the data stream from the remote user.
   *
   * The SDK triggers this callback when the user fails to receive the data stream that another user sends
   * by calling the \ref agora::rtc::IRtcEngine::sendStreamMessage "sendStreamMessage" method.
   *
   * @param connection The RtcConnection object.
   * @param remoteUid ID of the user who sends the data stream.
   * @param streamId The ID of the stream data.
   * @param code The error code.
   * @param missed The number of lost messages.
   * @param cached The number of incoming cached messages when the data stream is
   * interrupted.
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
   ** @brief Occurs when the local user receives data via Reliable Data Transmission (RDT) from a remote user.
   *
   * @technical preview
   *
   * @details The SDK triggers this callback when the user receives the data stream that another user sends
   *          by calling the \ref agora::rtc::IRtcEngine::sendRdtMessage "sendRdtMessage" method.
   *
   * @param connection The RtcConnection object.
   * @param userId ID of the user who sends the data.
   * @param type The RDT stream type. See RdtStreamType.
   * @param data The data received.
   * @param length The length (byte) of the data.
   */
  virtual void onRdtMessage(const RtcConnection& connection, uid_t userId, RdtStreamType type, const char *data, size_t length) {
    (void)connection;
    (void)userId;
    (void)type;
    (void)data;
    (void)length;
  }

  /**
   * @brief Occurs when the RDT tunnel state changed
   *
   * @technical preview
   *
   * @param connection The RtcConnection object.
   * @param userId ID of the user who sends the data.
   * @param state The RDT tunnel state. See RdtState.
   */
  virtual void onRdtStateChanged(const RtcConnection& connection, uid_t userId, RdtState state) {
    (void)connection;
    (void)userId;
    (void)state;
  }

  /**
   * @brief Occurs when the local user receives media control message sent by a remote user.
   *
   * @technical preview
   *
   * @details The SDK triggers this callback when the user receives data sent by a remote user using the sendMediaControlMessage method.
   *
   * @param connection The RtcConnection object.
   * @param userId ID of the user who sends the data.
   * @param data The data received.
   * @param length The length (byte) of the data.
   */
  virtual void onMediaControlMessage(const RtcConnection& connection, uid_t userId, const char* data, size_t length) {
    (void)connection;
    (void)userId;
    (void)data;
    (void)length;
  }

  /**
   * Occurs when the token expires.
   *
   * When the token expires during a call, the SDK triggers this callback to remind the app to renew the token.
   *
   * Upon receiving this callback, generate a new token at your app server and call
   * `joinChannel` to pass the new token to the SDK.
   *
   * @param connection The RtcConnection object.
   */
  virtual void onRequestToken(const RtcConnection& connection) {
    (void)connection;
  }

  /**
   * Occurs when connection license verification fails.
   *
   * You can know the reason accordding to error code
   */
  virtual void onLicenseValidationFailure(const RtcConnection& connection, LICENSE_ERROR_TYPE reason) {
    (void)connection;
    (void)reason;
  }

  /**
   * Occurs when the token will expire in 30 seconds.
   *
   * When the token is about to expire in 30 seconds, the SDK triggers this callback to remind the app to renew the token.

   * Upon receiving this callback, generate a new token at your app server and call
   * \ref IRtcEngine::renewToken "renewToken" to pass the new Token to the SDK.
   *
   * @param connection The RtcConnection object.
   * @param token The token that will expire in 30 seconds.
   */
  virtual void onTokenPrivilegeWillExpire(const RtcConnection& connection, const char* token) {
    (void)connection;
    (void)token;
  }

  /** Occurs when the first local audio frame is published.
   *
   * The SDK triggers this callback under one of the following circumstances:
   * - The local client enables the audio module and calls `joinChannel` successfully.
   * - The local client calls `muteLocalAudioStream(true)` and `muteLocalAudioStream(false)` in sequence.
   * - The local client calls `disableAudio` and `enableAudio` in sequence.
   * - The local client calls `pushAudioFrame` to successfully push the audio frame to the SDK.
   *
   * @param connection The RtcConnection object.
   * @param elapsed The time elapsed (ms) from the local user calling `joinChannel` to the SDK triggers this callback.
   */
  virtual void onFirstLocalAudioFramePublished(const RtcConnection& connection, int elapsed) {
    (void)connection;
    (void)elapsed;
  }

  /** Occurs when the SDK receives the first audio frame from a specific remote user.
   * @deprecated Use `onRemoteAudioStateChanged` instead.
   *
   * @param connection The RtcConnection object.
   * @param userId ID of the remote user.
   * @param elapsed The time elapsed (ms) from the loca user calling `joinChannel`
   * until this callback is triggered.
   */
  virtual void onFirstRemoteAudioFrame(const RtcConnection& connection, uid_t userId, int elapsed) __deprecated {
    (void)connection;
    (void)userId;
    (void)elapsed;
  }

  /**
   * Occurs when the SDK decodes the first remote audio frame for playback.
   *
   * @deprecated Use `onRemoteAudioStateChanged` instead.
   * The SDK triggers this callback under one of the following circumstances:
   * - The remote user joins the channel and sends the audio stream for the first time.
   * - The remote user's audio is offline and then goes online to re-send audio. It means the local user cannot
   * receive audio in 15 seconds. Reasons for such an interruption include:
   *   - The remote user leaves channel.
   *   - The remote user drops offline.
   *   - The remote user calls muteLocalAudioStream to stop sending the audio stream.
   *   - The remote user calls disableAudio to disable audio.
   * @param connection The RtcConnection object.
   * @param uid User ID of the remote user sending the audio stream.
   * @param elapsed The time elapsed (ms) from the loca user calling `joinChannel`
   * until this callback is triggered.
   */
  virtual void onFirstRemoteAudioDecoded(const RtcConnection& connection, uid_t uid, int elapsed) __deprecated {
    (void)connection;
    (void)uid;
    (void)elapsed;
  }

  /** Occurs when the local audio state changes.
   *
   * When the state of the local audio stream changes (including the state of the audio capture and encoding), the SDK
   * triggers this callback to report the current state. This callback indicates the state of the local audio stream,
   * and allows you to troubleshoot issues when audio exceptions occur.
   *
   * @note
   * When the state is `LOCAL_AUDIO_STREAM_STATE_FAILED(3)`, see the `error`
   * parameter for details.
   *
   * @param connection The RtcConnection object.
   * @param state State of the local audio. See #LOCAL_AUDIO_STREAM_STATE.
   * @param reason The reason information of the local audio.
   * See #LOCAL_AUDIO_STREAM_REASON.
   */
  virtual void onLocalAudioStateChanged(const RtcConnection& connection, LOCAL_AUDIO_STREAM_STATE state, LOCAL_AUDIO_STREAM_REASON reason) {
    (void)connection;
    (void)state;
    (void)reason;
  }

  /** Occurs when the remote audio state changes.
   *
   * When the audio state of a remote user (in the voice/video call channel) or host (in the live streaming channel)
   * changes, the SDK triggers this callback to report the current state of the remote audio stream.
   *
   * @note This callback does not work properly when the number of users (in the voice/video call channel) or hosts
   * (in the live streaming channel) in the channel exceeds 17.
   *
   * @param connection The RtcConnection object.
   * @param remoteUid ID of the remote user whose audio state changes.
   * @param state State of the remote audio. See #REMOTE_AUDIO_STATE.
   * @param reason The reason of the remote audio state change.
   * See #REMOTE_AUDIO_STATE_REASON.
   * @param elapsed Time elapsed (ms) from the local user calling the
   * `joinChannel` method until the SDK
   * triggers this callback.
   */
  virtual void onRemoteAudioStateChanged(const RtcConnection& connection, uid_t remoteUid, REMOTE_AUDIO_STATE state, REMOTE_AUDIO_STATE_REASON reason, int elapsed) {
    (void)connection;
    (void)remoteUid;
    (void)state;
    (void)reason;
    (void)elapsed;
  }

  /**
   * Occurs when an active speaker is detected.
   *
   * After a successful call of `enableAudioVolumeIndication`, the SDK continuously detects which remote user has the
   * loudest volume. During the current period, the remote user, who is detected as the loudest for the most times,
   * is the most active user.
   *
   * When the number of users is no less than two and an active remote speaker exists, the SDK triggers this callback and reports the uid of the most active remote speaker.
   * - If the most active remote speaker is always the same user, the SDK triggers the `onActiveSpeaker` callback only once.
   * - If the most active remote speaker changes to another user, the SDK triggers this callback again and reports the uid of the new active remote speaker.
   *
   * @param connection The RtcConnection object.
   * @param uid The ID of the active speaker. A `uid` of 0 means the local user.
   */
  virtual void onActiveSpeaker(const RtcConnection& connection, uid_t uid) {
    (void)connection;
    (void)uid;
  }

  /**
   * Occurs when the user role switches in the interactive live streaming.
   *
   * @param connection The RtcConnection of the local user: #RtcConnection
   * @param oldRole The old role of the user: #CLIENT_ROLE_TYPE
   * @param newRole The new role of the user: #CLIENT_ROLE_TYPE
   * @param newRoleOptions The client role options of the new role: #ClientRoleOptions.
   */
  virtual void onClientRoleChanged(const RtcConnection& connection, CLIENT_ROLE_TYPE oldRole, CLIENT_ROLE_TYPE newRole, const ClientRoleOptions& newRoleOptions) {
    (void)connection;
    (void)oldRole;
    (void)newRole;
    (void)newRoleOptions;
  }

  /**
   * Occurs when the user role in a Live-Broadcast channel fails to switch, for example, from a broadcaster
   * to an audience or vice versa.
   *
   * @param connection The RtcConnection object.
   * @param reason The reason for failing to change the client role: #CLIENT_ROLE_CHANGE_FAILED_REASON.
   * @param currentRole The current role of the user: #CLIENT_ROLE_TYPE.
   */
  virtual void onClientRoleChangeFailed(const RtcConnection& connection, CLIENT_ROLE_CHANGE_FAILED_REASON reason, CLIENT_ROLE_TYPE currentRole) {
    (void)connection;
    (void)reason;
    (void)currentRole;
  }

  /** Reports the transport-layer statistics of each remote audio stream.
   * @deprecated Use `onRemoteAudioStats` instead.

  This callback reports the transport-layer statistics, such as the packet loss rate and network time delay, once every
  two seconds after the local user receives an audio packet from a remote user. During a call, when the user receives
  the audio packet sent by the remote user/host, the callback is triggered every 2 seconds.

  @param connection The RtcConnection object.
  @param remoteUid ID of the remote user whose audio data packet is received.
  @param delay The network time delay (ms) from the sender to the receiver.
  @param lost The Packet loss rate (%) of the audio packet sent from the remote
  user.
  @param rxKBitRate Received bitrate (Kbps) of the audio packet sent from the
  remote user.
  */
  virtual void onRemoteAudioTransportStats(const RtcConnection& connection, uid_t remoteUid, unsigned short delay, unsigned short lost,
                                           unsigned short rxKBitRate) __deprecated {
    (void)connection;
    (void)remoteUid;
    (void)delay;
    (void)lost;
    (void)rxKBitRate;
  }

  /** Reports the transport-layer statistics of each remote video stream.
   * @deprecated Use `onRemoteVideoStats` instead.

  This callback reports the transport-layer statistics, such as the packet loss rate and network time
  delay, once every two seconds after the local user receives a video packet from a remote user.

  During a call, when the user receives the video packet sent by the remote user/host, the callback is
  triggered every 2 seconds.

  @param connection The RtcConnection object.
  @param remoteUid ID of the remote user whose video packet is received.
  @param delay The network time delay (ms) from the remote user sending the
  video packet to the local user.
  @param lost The packet loss rate (%) of the video packet sent from the remote
  user.
  @param rxKBitRate The bitrate (Kbps) of the video packet sent from
  the remote user.
  */
  virtual void onRemoteVideoTransportStats(const RtcConnection& connection, uid_t remoteUid, unsigned short delay, unsigned short lost,
                                           unsigned short rxKBitRate) __deprecated {
    (void)connection;
    (void)remoteUid;
    (void)delay;
    (void)lost;
    (void)rxKBitRate;
  }

  /** Occurs when the network connection state changes.
   *
   * When the network connection state changes, the SDK triggers this callback and reports the current
   * connection state and the reason for the change.
   *
   * @param connection The RtcConnection object.
   * @param state The current connection state. See #CONNECTION_STATE_TYPE.
   * @param reason The reason for a connection state change. See #CONNECTION_CHANGED_REASON_TYPE.
   */
  virtual void onConnectionStateChanged(const RtcConnection& connection,
                                        CONNECTION_STATE_TYPE state,
                                        CONNECTION_CHANGED_REASON_TYPE reason) {
    (void)connection;
    (void)state;
    (void)reason;
  }

  /** Occurs when the local network type changes.
   *
   * This callback occurs when the connection state of the local user changes. You can get the
   * connection state and reason for the state change in this callback. When the network connection
   * is interrupted, this callback indicates whether the interruption is caused by a network type
   * change or poor network conditions.
   * @param connection The RtcConnection object.
   * @param type The type of the local network connection. See #NETWORK_TYPE.
   */
  virtual void onNetworkTypeChanged(const RtcConnection& connection, NETWORK_TYPE type) {
    (void)connection;
    (void)type;
  }

  /** Reports the built-in encryption errors.
   *
   * When encryption is enabled by calling `enableEncryption`, the SDK triggers this callback if an
   * error occurs in encryption or decryption on the sender or the receiver side.
   * @param connection The RtcConnection object.
   * @param errorType The error type. See #ENCRYPTION_ERROR_TYPE.
   */
  virtual void onEncryptionError(const RtcConnection& connection, ENCRYPTION_ERROR_TYPE errorType) {
    (void)connection;
    (void)errorType;
  }
  /**
   * Reports the user log upload result
   * @param connection The RtcConnection object.
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

  /**
   * Occurs when the user account is updated.
   *
   * @param connection The RtcConnection object.
   * @param remoteUid The user ID.
   * @param userAccount The user account.
   */
  virtual void onUserAccountUpdated(const RtcConnection& connection, uid_t remoteUid, const char* remoteUserAccount){
    (void)connection;
    (void)remoteUid;
    (void)remoteUserAccount;
  }

  /** Reports the result of taking a video snapshot.
   *
   * After a successful `takeSnapshot` method call, the SDK triggers this callback to report whether the snapshot is
   * successfully taken, as well as the details for that snapshot.
   * @param connection The RtcConnection object.
   * @param uid The user ID. A `uid` of 0 indicates the local user.
   * @param filePath The local path of the snapshot.
   * @param width The width (px) of the snapshot.
   * @param height The height (px) of the snapshot.
   * @param errCode The message that confirms success or gives the reason why the snapshot is not successfully taken:
   * - 0: Success.
   * - &lt; 0: Failure.
   *   - -1: The SDK fails to write data to a file or encode a JPEG image.
   *   - -2: The SDK does not find the video stream of the specified user within one second after the `takeSnapshot` method call succeeds.
   *   - -3: Calling the `takeSnapshot` method too frequently. Call the `takeSnapshot` method after receiving the `onSnapshotTaken`
   * callback from the previous call.
   */
  virtual void onSnapshotTaken(const RtcConnection& connection, uid_t uid, const char* filePath, int width, int height, int errCode) {
    (void)uid;
    (void)filePath;
    (void)width;
    (void)height;
    (void)errCode;
  }

  /**
   * Reports the tracing result of video rendering event of the user.
   *
   * @param connection The RtcConnection object.
   * @param uid The user ID.
   * @param currentEvent The current event of the tracing result: #MEDIA_TRACE_EVENT.
   * @param tracingInfo The tracing result: #VideoRenderingTracingInfo.
   */
  virtual void onVideoRenderingTracingResult(const RtcConnection& connection, uid_t uid, MEDIA_TRACE_EVENT currentEvent, VideoRenderingTracingInfo tracingInfo) {
    (void)uid;
    (void)currentEvent;
    (void)tracingInfo;
  }

  /**
   * Occurs when receive use rtm response.
   *
   * @param connection The RtcConnection object.
   * @param code The error code:
   */
  virtual void onSetRtmFlagResult(const RtcConnection& connection, int code) {
    (void)connection;
    (void)code;
  }
  /**
   * Occurs when receive a video transcoder stream which has video layout info.
   *
   * @param connection The RtcConnection object.
   * @param uid user id of the transcoded stream.
   * @param width width of the transcoded stream.
   * @param height height of the transcoded stream.
   * @param layoutCount count of layout info in the transcoded stream.
   * @param layoutlist video layout info list of the transcoded stream.
   */
  virtual void onTranscodedStreamLayoutInfo(const RtcConnection& connection, uid_t uid, int width, int height, int layoutCount,const VideoLayout* layoutlist) {
    (void)uid;
    (void)width;
    (void)height;
    (void)layoutCount;
    (void)layoutlist;
  }

  /**
   * The audio metadata received.
   *
   * @param connection The RtcConnection object.
   * @param uid ID of the remote user.
   * @param metadata The pointer of metadata
   * @param length Size of metadata
   * @technical preview
   */
  virtual void onAudioMetadataReceived(const RtcConnection& connection, uid_t uid, const char* metadata, size_t length) {
    (void)metadata;
    (void)length;
  }

  /**
   * @brief Callback for multipath transmission statistics.
   *
   * @since 4.6.0
   *
   * @details
   * Call timing: This callback is triggered after you set `enableMultipath` to `true` to enable
   * multipath transmission.
   *
   * @param stats Multipath transmission statistics. See `MultipathStats`.
   *
   */
  virtual void onMultipathStats(const RtcConnection& connection, const MultipathStats& stats) {
    (void)stats;
    (void)connection;
  }

  /**
   * Occurs when a user renews the token.
   *
   * This callback notifies the app that the user renews the token by calling `renewToken`. From this callback,
   * the app can get the result of `renewToken`.
   *
   * @param connection The RtcConnection object.
   * @param token The token.
   * @param code The error code.
   */
  virtual void onRenewTokenResult(const RtcConnection& connection, const char* token, RENEW_TOKEN_ERROR_CODE code) {
    (void)token;
    (void)code;
  }
};

class IRtcEngineEx : public IRtcEngine {
public:
    /**
     * @brief Joins a channel.
     *
     * @details
     * You can call this method multiple times to join more than one channel. If you want to join the
     * same channel from different devices, ensure that the user IDs are different for all devices.
     * Applicable scenarios: This method can be called in scenarios involving multiple channels.
     * Call timing: Call this method after `initialize`.
     * In a multi-camera capture scenario, you need to call the `startPreview(VIDEO_SOURCE_TYPE
     * sourceType)` method after
     * calling this method to set the `sourceType` to `VIDEO_SOURCE_CAMERA_SECONDARY`, to ensure that
     * the second camera captures normally.
     * Related callbacks: A successful call of this method triggers the following callbacks:
     * - The local client: The `onJoinChannelSuccess` and `onConnectionStateChanged` callbacks.
     * - The remote client: The `onUserJoined` callback, if a user joining the channel in the
     * COMMUNICATION profile, or a host joining a channel in the LIVE_BROADCASTING profile.
     * When the connection between the local client and Agora's server is interrupted due to poor
     * network conditions, the SDK tries reconnecting to the server. When the local client successfully
     * rejoins the channel, the SDK triggers the `onRejoinChannelSuccess` callback on the local client.
     *
     * @note
     * If you are already in a channel, you cannot rejoin the channel with the same user ID.
     * Before joining a channel, ensure that the App ID you use to generate a token is the same as that
     * you pass in the `initialize` method; otherwise, you may fail to join the channel with the token.
     *
     * @param token The token generated on your server for authentication. See .Note:
     * - (Recommended) If your project has enabled the security mode (using APP ID and Token for
     * authentication), this parameter is required.
     * - If you have only enabled the testing mode (using APP ID for authentication), this parameter is
     * optional. You will automatically exit the channel 24 hours after successfully joining in.
     * - If you need to join different channels at the same time or switch between channels, Agora
     * recommends using a wildcard token so that you don't need to apply for a new token every time
     * joining a channel. See `Secure authentication with tokens`.
     * @param connection The connection information. See `RtcConnection`.
     * @param options The channel media options. See `ChannelMediaOptions`.
     * @param eventHandler The callback class of `IRtcEngineEx`. See `IRtcEngineEventHandler`. You can
     * get the callback events of multiple channels through the `eventHandler` object passed in this
     * parameter.
     *
     * @return
     * - 0: Success.
     * - < 0: Failure.
     *   - -2: The parameter is invalid. For example, the token is invalid, the `uid` parameter is not
     * set to an integer, or the value of a member in `ChannelMediaOptions` is invalid. You need to pass
     * in a valid parameter and join the channel again.
     *   - -3: Fails to initialize the `IRtcEngine` object. You need to reinitialize the `IRtcEngine`
     * object.
     *   - -7: The `IRtcEngine` object has not been initialized. You need to initialize the `IRtcEngine`
     * object before calling this method.
     *   - -8: The internal state of the `IRtcEngine` object is wrong. The typical cause is that after
     * calling `startEchoTest` to start a call loop test, you call this method to join the channel
     * without calling `stopEchoTest` to stop the test. You need to call `stopEchoTest` before calling
     * this method.
     *   - -17: The request to join the channel is rejected. The typical cause is that the user is
     * already in the channel. Agora recommends that you use the `onConnectionStateChanged` callback to
     * see whether the user is in the channel. Do not call this method to join the channel unless you
     * receive the `CONNECTION_STATE_DISCONNECTED` (1) state.
     *   - -102: The channel name is invalid. You need to pass in a valid channel name in `channelId` to
     * rejoin the channel.
     *   - -121: The user ID is invalid. You need to pass in a valid user ID in `uid` to rejoin the
     * channel.
     */
    virtual int joinChannelEx(const char* token, const RtcConnection& connection,
                              const ChannelMediaOptions& options,
                              IRtcEngineEventHandler* eventHandler) = 0;

    /**
     * @brief Leaves a channel.
     *
     * @details
     * After calling this method, the SDK terminates the audio and video interaction, leaves the current
     * channel, and releases all resources related to the session.
     * After calling `joinChannelEx` to join a channel, you must call this method or
     * `leaveChannelEx(const RtcConnection& connection, const LeaveChannelOptions& options)`
     * to end the call, otherwise, the next call cannot be started.
     * Applicable scenarios: This method can be called in scenarios involving multiple channels.
     * Call timing: Call this method after `joinChannelEx`.
     * Related callbacks: A successful call of this method triggers the following callbacks:
     * - The local client: The `onLeaveChannel` callback will be triggered.
     * - The remote client: The `onUserOffline` callback will be triggered after the remote host leaves
     * the channel.
     *
     * @note
     * If you call `release` immediately after calling this method, the SDK does not trigger the
     * `onLeaveChannel` callback.
     * - This method call is asynchronous. When this method returns, it does not necessarily mean that
     * the user has left the channel.
     * - If you call `leaveChannel()` or `leaveChannel(const LeaveChannelOptions& options)`, you will
     * leave all the channels you
     * have joined by calling `joinChannel(const char* token, const char* channelId, const char* info,
     * uid_t uid)`, `joinChannel(const char* token, const char* channelId, uid_t uid, const
     * ChannelMediaOptions& options)`, or `joinChannelEx`.
     *
     * @param connection The connection information. See `RtcConnection`.
     *
     * @return
     * - 0: Success.
     * - < 0: Failure.
     */
    virtual int leaveChannelEx(const RtcConnection& connection) = 0;

    /**
     * @brief Sets channel options and leaves the channel.
     *
     * @details
     * After calling this method, the SDK terminates the audio and video interaction, leaves the current
     * channel, and releases all resources related to the session.
     * After calling `joinChannelEx` to join a channel, you must call this method or
     * `leaveChannelEx(const RtcConnection& connection)`
     * to end the call, otherwise, the next call cannot be started.
     * Applicable scenarios: This method can be called in scenarios involving multiple channels.
     * Call timing: Call this method after `joinChannelEx`.
     * Related callbacks: A successful call of this method triggers the following callbacks:
     * - The local client: The `onLeaveChannel` callback will be triggered.
     * - The remote client: The `onUserOffline` callback will be triggered after the remote host leaves
     * the channel.
     *
     * @note
     * If you call `release` immediately after calling this method, the SDK does not trigger the
     * `onLeaveChannel` callback.
     * - This method call is asynchronous. When this method returns, it does not necessarily mean that
     * the user has left the channel.
     * - If you call `leaveChannel()` or `leaveChannel(const LeaveChannelOptions& options)`, you will
     * leave all the channels you
     * have joined by calling `joinChannel(const char* token, const char* channelId, const char* info,
     * uid_t uid)`, `joinChannel(const char* token, const char* channelId, uid_t uid, const
     * ChannelMediaOptions& options)`, or `joinChannelEx`.
     *
     * @param connection The connection information. See `RtcConnection`.
     * @param options Since
     * v4.1.0
     * The options for leaving the channel. See `LeaveChannelOptions`.
     * Note: This parameter only supports the `stopMicrophoneRecording` member in the
     * `LeaveChannelOptions` settings; setting other members does not take effect.
     *
     * @return
     * - 0: Success.
     * - < 0: Failure.
     */
    virtual int leaveChannelEx(const RtcConnection& connection, const LeaveChannelOptions& options) = 0;

  /**
    * Leaves a channel with the channel ID and user account.
    *
    * This method allows a user to leave the channel, for example, by hanging up or exiting a call.
    *
    * This method is an asynchronous call, which means that the result of this method returns even before
    * the user has not actually left the channel. Once the user successfully leaves the channel, the
    * SDK triggers the \ref IRtcEngineEventHandler::onLeaveChannel "onLeaveChannel" callback.
    *
    * @param channelId The channel name. The maximum length of this parameter is 64 bytes. Supported character scopes are:
    * - All lowercase English letters: a to z.
    * - All uppercase English letters: A to Z.
    * - All numeric characters: 0 to 9.
    * - The space character.
    * - Punctuation characters and other symbols, including: "!", "#", "$", "%", "&", "(", ")", "+", "-", ":", ";", "<", "=", ".", ">", "?", "@", "[", "]", "^", "_", " {", "}", "|", "~", ",".
    * @param userAccount The user account. The maximum length of this parameter is 255 bytes. Ensure that you set this parameter and do not set it as null. Supported character scopes are:
    * - All lowercase English letters: a to z.
    * - All uppercase English letters: A to Z.
    * - All numeric characters: 0 to 9.
    * - The space character.
    * - Punctuation characters and other symbols, including: "!", "#", "$", "%", "&", "(", ")", "+", "-", ":", ";", "<", "=", ".", ">", "?", "@", "[", "]", "^", "_", " {", "}", "|", "~", ",".
    * @return
    * - 0: Success.
    * - < 0: Failure.
    */
    virtual int leaveChannelWithUserAccountEx(const char* channelId, const char* userAccount) = 0;

  /**
    * Leaves a channel with the channel ID and user account and sets the options for leaving.
    *
    * @param channelId The channel name. The maximum length of this parameter is 64 bytes. Supported character scopes are:
    * - All lowercase English letters: a to z.
    * - All uppercase English letters: A to Z.
    * - All numeric characters: 0 to 9.
    * - The space character.
    * - Punctuation characters and other symbols, including: "!", "#", "$", "%", "&", "(", ")", "+", "-", ":", ";", "<", "=", ".", ">", "?", "@", "[", "]", "^", "_", " {", "}", "|", "~", ",".
    * @param userAccount The user account. The maximum length of this parameter is 255 bytes. Ensure that you set this parameter and do not set it as null. Supported character scopes are:
    * - All lowercase English letters: a to z.
    * - All uppercase English letters: A to Z.
    * - All numeric characters: 0 to 9.
    * - The space character.
    * - Punctuation characters and other symbols, including: "!", "#", "$", "%", "&", "(", ")", "+", "-", ":", ";", "<", "=", ".", ">", "?", "@", "[", "]", "^", "_", " {", "}", "|", "~", ",".
    * @param options The options for leaving the channel. See #LeaveChannelOptions.
    * @return int
    * - 0: Success.
    * - < 0: Failure.
    */
    virtual int leaveChannelWithUserAccountEx(const char* channelId, const char* userAccount, const LeaveChannelOptions& options) = 0;

    /**
     * @brief Updates the channel media options after joining the channel.
     *
     * @param options The channel media options. See `ChannelMediaOptions`.
     * @param connection The connection information. See `RtcConnection`.
     *
     * @return
     * - 0: Success.
     * - < 0: Failure.
     *   - -2: The value of a member in `ChannelMediaOptions` is invalid. For example, the token or the
     * user ID is invalid. You need to fill in a valid parameter.
     *   - -7: The `IRtcEngine` object has not been initialized. You need to initialize the `IRtcEngine`
     * object before calling this method.
     *   - -8: The internal state of the `IRtcEngine` object is wrong. The possible reason is that the
     * user is not in the channel. Agora recommends that you use the `onConnectionStateChanged` callback
     * to see whether the user is in the channel. If you receive the `CONNECTION_STATE_DISCONNECTED` (1)
     * or `CONNECTION_STATE_FAILED` (5) state, the user is not in the channel. You need to call
     * `joinChannel(const char* token, const char* channelId, uid_t uid, const ChannelMediaOptions&
     * options)` to join a channel before calling this method.
     */
    virtual int updateChannelMediaOptionsEx(const ChannelMediaOptions& options, const RtcConnection& connection) = 0;
    /**
     * @brief Sets the video encoder configuration.
     *
     * @details
     * Sets the encoder configuration for the local video. Each configuration profile corresponds to a
     * set of video parameters, including the resolution, frame rate, and bitrate.
     * Call timing: Call this method after `joinChannelEx`.
     *
     * @note The `config` specified in this method is the maximum value under ideal network conditions.
     * If the video engine cannot render the video using the specified `config` due to unreliable
     * network conditions, the parameters further down the list are considered until a successful
     * configuration is found.
     *
     * @param config Video profile. See `VideoEncoderConfiguration`.
     * @param connection The connection information. See `RtcConnection`.
     *
     * @return
     * - 0: Success.
     * - < 0: Failure.
     */
    virtual int setVideoEncoderConfigurationEx(const VideoEncoderConfiguration& config, const RtcConnection& connection) = 0;
    /**
     * @brief Initializes the video view of a remote user.
     *
     * @details
     * This method initializes the video view of a remote stream on the local device. It affects only
     * the video view that the local user sees. Call this method to bind the remote video stream to a
     * video view and to set the rendering and mirror modes of the video view.
     * The application specifies the uid of the remote video in the `VideoCanvas` method before the
     * remote user joins the channel.
     * If the remote uid is unknown to the application, set it after the application receives the
     * `onUserJoined` callback. If the Video Recording function is enabled, the Video Recording Service
     * joins the channel as a dummy client, causing other clients to also receive the `onUserJoined`
     * callback. Do not bind the dummy client to the application view because the dummy client does not
     * send any video streams.
     * To unbind the remote user from the view, set the `view` parameter to NULL.
     * Once the remote user leaves the channel, the SDK unbinds the remote user.
     *
     * @note
     * - Call this method after `joinChannelEx`.
     * - To update the rendering or mirror mode of the remote video view during a call, use the
     * `setRemoteRenderModeEx` method.
     *
     * @param canvas The remote video view settings. See `VideoCanvas`.
     * @param connection The connection information. See `RtcConnection`.
     *
     * @return
     * - 0: Success.
     * - < 0: Failure.
     */
    virtual int setupRemoteVideoEx(const VideoCanvas& canvas, const RtcConnection& connection) = 0;
    /**
     * @brief Stops or resumes receiving the audio stream of a specified user.
     *
     * @details
     *             This method is used to stops or resumes receiving the audio stream of a specified
     * user. You can call this method before or after joining a channel. If a user leaves a channel, the
     * settings in this method become invalid.
     *
     * @param uid The ID of the specified user.
     * @param mute Whether to stop receiving the audio stream of the specified user:
     * - `true`: Stop receiving the audio stream of the specified user.
     * - `false`: (Default) Resume receiving the audio stream of the specified user.
     * @param connection The connection information. See `RtcConnection`.
     *
     * @return
     * - 0: Success.
     * - < 0: Failure.
     */
    virtual int muteRemoteAudioStreamEx(uid_t uid, bool mute, const RtcConnection& connection) = 0;
    /**
     * @brief Stops or resumes receiving the video stream of a specified user.
     *
     * @details
     * This method is used to stop or resume receiving the video stream of a specified user. You can
     * call this method before or after joining a channel. If a user leaves a channel, the settings in
     * this method become invalid.
     *
     * @param uid The user ID of the remote user.
     * @param mute Whether to stop receiving the video stream of the specified user:
     * - `true`: Stop receiving the video stream of the specified user.
     * - `false`: (Default) Resume receiving the video stream of the specified user.
     * @param connection The connection information. See `RtcConnection`.
     *
     * @return
     * - 0: Success.
     * - < 0: Failure.
     */
    virtual int muteRemoteVideoStreamEx(uid_t uid, bool mute, const RtcConnection& connection) = 0;
    /**
     * @brief Sets the video stream type to subscribe to.
     *
     * @details
     * Depending on the default behavior of the sender and the specific settings when calling
     * `setDualStreamMode(SIMULCAST_STREAM_MODE mode, const SimulcastStreamConfig& streamConfig)`, the
     * scenarios for the receiver calling this method are as follows:
     * - The SDK enables low-quality video stream adaptive mode ( `AUTO_SIMULCAST_STREAM` ) on the
     * sender side by default, meaning only the high-quality video stream is transmitted. Only the
     * receiver with the role of the **host**can call this method to initiate a low-quality video stream
     * request. Once the sender receives the request, it starts automatically sending the low-quality
     * video stream. At this point, all users in the channel can call this method to switch to
     * low-quality video stream subscription mode.
     * - If the sender calls `setDualStreamMode(SIMULCAST_STREAM_MODE mode, const SimulcastStreamConfig&
     * streamConfig)` and sets `mode` to `DISABLE_SIMULCAST_STREAM`
     * (never send low-quality video stream), then calling this method will have no effect.
     * - If the sender calls `setDualStreamMode(SIMULCAST_STREAM_MODE mode, const SimulcastStreamConfig&
     * streamConfig)` and sets `mode` to `ENABLE_SIMULCAST_STREAM`
     * (always send low-quality video stream), both the host and audience receivers can call this method
     * to switch to low-quality video stream subscription mode.
     * The SDK will dynamically adjust the size of the corresponding video stream based on the size of
     * the video window to save bandwidth and computing resources. The default aspect ratio of the
     * low-quality video stream is the same as that of the high-quality video stream. According to the
     * current aspect ratio of the high-quality video stream, the system will automatically allocate the
     * resolution, frame rate, and bitrate of the low-quality video stream.
     *
     * @note If the publisher has already called `setDualStreamModeEx` and set `mode` to
     * `DISABLE_SIMULCAST_STREAM` (never send low-quality video stream), calling this method will not
     * take effect, you should call `setDualStreamModeEx` again on the sending end and adjust the
     * settings.
     *
     * @param uid The user ID.
     * @param streamType The video stream type, see `VIDEO_STREAM_TYPE`.
     * @param connection The connection information. See `RtcConnection`.
     *
     * @return
     * - 0: Success.
     * - < 0: Failure.
     */
    virtual int setRemoteVideoStreamTypeEx(uid_t uid, VIDEO_STREAM_TYPE streamType, const RtcConnection& connection) = 0;
    /**
     * @brief Stops or resumes publishing the local audio stream.
     *
     * @details
     * A successful call of this method triggers the `onUserMuteAudio` and `onRemoteAudioStateChanged`
     * callbacks on the remote client.
     *
     * @note This method does not affect any ongoing audio recording, because it does not disable the
     * audio capture device.
     *
     * @param mute Whether to stop publishing the local audio stream:
     * - `true`: Stops publishing the local audio stream.
     * - `false`: (Default) Resumes publishing the local audio stream.
     * @param connection The connection information. See `RtcConnection`.
     *
     * @return
     * - 0: Success.
     * - < 0: Failure.
     */
    virtual int muteLocalAudioStreamEx(bool mute, const RtcConnection& connection) = 0;

    /**
     * @brief Stops or resumes publishing the local video stream.
     *
     * @details
     * A successful call of this method triggers the `onUserMuteVideo` callback on the remote client.
     *
     * @note This method does not affect any ongoing video recording, because it does not disable the
     * camera.
     *
     * @param mute Whether to stop publishing the local video stream.
     * - `true`: Stop publishing the local video stream.
     * - `false`: (Default) Publish the local video stream.
     * @param connection The connection information. See `RtcConnection`.
     *
     * @return
     * - 0: Success.
     * - < 0: Failure.
     */
    virtual int muteLocalVideoStreamEx(bool mute, const RtcConnection& connection) = 0;

    /**
     * @brief Stops or resumes subscribing to the audio streams of all remote users.
     *
     * @details
     * After successfully calling this method, the local user stops or resumes subscribing to the audio
     * streams of all remote users, including the ones join the channel subsequent to this call.
     *
     * @note
     * - Call this method after joining a channel.
     * - If you do not want to subscribe the audio streams of remote users before joining a channel, you
     * can set `autoSubscribeAudio` as `false` when calling `joinChannel(const char* token, const char*
     * channelId, uid_t uid, const ChannelMediaOptions& options)`.
     *
     * @param mute Whether to stop subscribing to the audio streams of all remote users:
     * - `true`: Stops subscribing to the audio streams of all remote users.
     * - `false`: (Default) Subscribes to the audio streams of all remote users by default.
     * @param connection The connection information. See `RtcConnection`.
     *
     * @return
     * - 0: Success.
     * - < 0: Failure.
     */
    virtual int muteAllRemoteAudioStreamsEx(bool mute, const RtcConnection& connection) = 0;

    /**
     * @brief Stops or resumes subscribing to the video streams of all remote users.
     *
     * @details
     * After successfully calling this method, the local user stops or resumes subscribing to the video
     * streams of all remote users, including all subsequent users.
     *
     * @param mute Whether to stop subscribing to the video streams of all remote users.
     * - `true`: Stop subscribing to the video streams of all remote users.
     * - `false`: (Default) Subscribe to the video streams of all remote users by default.
     * @param connection The connection information. See `RtcConnection`.
     *
     * @return
     * - 0: Success.
     * - < 0: Failure.
     */
    virtual int muteAllRemoteVideoStreamsEx(bool mute, const RtcConnection& connection) = 0;


    /**
     * @brief Sets the blocklist of subscriptions for audio streams.
     *
     * @details
     * You can call this method to specify the audio streams of a user that you do not want to subscribe
     * to.
     *
     * @note
     * - You can call this method either before or after joining a channel.
     * - The blocklist is not affected by the setting in `muteRemoteAudioStream`,
     * `muteAllRemoteAudioStreams`, and `autoSubscribeAudio` in `ChannelMediaOptions`.
     * - Once the blocklist of subscriptions is set, it is effective even if you leave the current
     * channel and rejoin the channel.
     * - If a user is added in the allowlist and blocklist at the same time, only the blocklist takes
     * effect.
     *
     * @param uidList The user ID list of users that you do not want to subscribe to.
     * If you want to specify the audio streams of a user that you do not want to subscribe to, add the
     * user ID in this list. If you want to remove a user from the blocklist, you need to call the
     * `setSubscribeAudioBlocklist` method to update the user ID list; this means you only add the `uid`
     * of users that you do not want to subscribe to in the new user ID list.
     * @param uidNumber The number of users in the user ID list.
     * @param connection The connection information. See `RtcConnection`.
     *
     * @return
     * - 0: Success.
     * - < 0: Failure.
     */
    virtual int setSubscribeAudioBlocklistEx(uid_t* uidList, int uidNumber, const RtcConnection& connection) = 0;

    /**
     * @brief Sets the allowlist of subscriptions for audio streams.
     *
     * @details
     * You can call this method to specify the audio streams of a user that you want to subscribe to.
     *
     * @note
     * - You can call this method either before or after joining a channel.
     * - The allowlist is not affected by the setting in `muteRemoteAudioStream`,
     * `muteAllRemoteAudioStreams` and `autoSubscribeAudio` in `ChannelMediaOptions`.
     * - Once the allowlist of subscriptions is set, it is effective even if you leave the current
     * channel and rejoin the channel.
     * - If a user is added in the allowlist and blocklist at the same time, only the blocklist takes
     * effect.
     *
     * @param uidList The user ID list of users that you want to subscribe to.
     * If you want to specify the audio streams of a user for subscription, add the user ID in this
     * list. If you want to remove a user from the allowlist, you need to call the
     * `setSubscribeAudioAllowlist` method to update the user ID list; this means you only add the `uid`
     * of users that you want to subscribe to in the new user ID list.
     * @param uidNumber The number of users in the user ID list.
     * @param connection The connection information. See `RtcConnection`.
     *
     * @return
     * - 0: Success.
     * - < 0: Failure.
     */
    virtual int setSubscribeAudioAllowlistEx(uid_t* uidList, int uidNumber, const RtcConnection& connection) = 0;

    /**
     * @brief Sets the blocklist of subscriptions for video streams.
     *
     * @details
     * You can call this method to specify the video streams of a user that you do not want to subscribe
     * to.
     *
     * @note
     * - You can call this method either before or after joining a channel.
     * - The blocklist is not affected by the setting in `muteRemoteVideoStream`,
     * `muteAllRemoteVideoStreams` and `autoSubscribeAudio` in `ChannelMediaOptions`.
     * - Once the blocklist of subscriptions is set, it is effective even if you leave the current
     * channel and rejoin the channel.
     * - If a user is added in the allowlist and blocklist at the same time, only the blocklist takes
     * effect.
     *
     * @param uidList The user ID list of users that you do not want to subscribe to.
     * If you want to specify the video streams of a user that you do not want to subscribe to, add the
     * user ID of that user in this list. If you want to remove a user from the blocklist, you need to
     * call the `setSubscribeVideoBlocklist` method to update the user ID list; this means you only add
     * the `uid` of users that you do not want to subscribe to in the new user ID list.
     * @param uidNumber The number of users in the user ID list.
     * @param connection The connection information. See `RtcConnection`.
     *
     * @return
     * - 0: Success.
     * - < 0: Failure.
     */
    virtual int setSubscribeVideoBlocklistEx(uid_t* uidList, int uidNumber, const RtcConnection& connection) = 0;

    /**
     * @brief Sets the allowlist of subscriptions for video streams.
     *
     * @details
     * You can call this method to specify the video streams of a user that you want to subscribe to.
     *
     * @note
     * - You can call this method either before or after joining a channel.
     * - The allowlist is not affected by the setting in `muteRemoteVideoStream`,
     * `muteAllRemoteVideoStreams` and `autoSubscribeAudio` in `ChannelMediaOptions`.
     * - Once the allowlist of subscriptions is set, it is effective even if you leave the current
     * channel and rejoin the channel.
     * - If a user is added in the allowlist and blocklist at the same time, only the blocklist takes
     * effect.
     *
     * @param uidList The user ID list of users that you want to subscribe to.
     * If you want to specify the video streams of a user for subscription, add the user ID of that user
     * in this list. If you want to remove a user from the allowlist, you need to call the
     * `setSubscribeVideoAllowlist` method to update the user ID list; this means you only add the `uid`
     * of users that you want to subscribe to in the new user ID list.
     * @param uidNumber The number of users in the user ID list.
     * @param connection The connection information. See `RtcConnection`.
     *
     * @return
     * - 0: Success.
     * - < 0: Failure.
     */
    virtual int setSubscribeVideoAllowlistEx(uid_t* uidList, int uidNumber, const RtcConnection& connection) = 0;
    /**
     * @brief Sets options for subscribing to remote video streams.
     *
     * @details
     * When a remote user has enabled dual-stream mode, you can call this method to choose the option
     * for subscribing to the video streams sent by the remote user.
     *
     * @param uid The user ID of the remote user.
     * @param options The video subscription options. See `VideoSubscriptionOptions`.
     * @param connection The connection information. See `RtcConnection`.
     *
     * @return
     * - 0: Success.
     * - < 0: Failure.
     */
    virtual int setRemoteVideoSubscriptionOptionsEx(uid_t uid, const VideoSubscriptionOptions& options, const RtcConnection& connection) = 0;
    /**
     * @brief Sets the 2D position (the position on the horizontal plane) of the remote user's voice.
     *
     * @details
     * This method sets the voice position and volume of a remote user.
     * When the local user calls this method to set the voice position of a remote user, the voice
     * difference between the left and right channels allows the local user to track the real-time
     * position of the remote user, creating a sense of space. This method applies to massive
     * multiplayer online games, such as Battle Royale games.
     *
     * @note
     * - For the best voice positioning, Agora recommends using a wired headset.
     * - Call this method after joining a channel.
     *
     * @param uid The user ID of the remote user.
     * @param pan The voice position of the remote user. The value ranges from -1.0 to 1.0:
     * - -1.0: The remote voice comes from the left.
     * - 0.0: (Default) The remote voice comes from the front.
     * - 1.0: The remote voice comes from the right.
     * @param gain The volume of the remote user. The value ranges from 0.0 to 100.0. The default value
     * is 100.0 (the original volume of the remote user). The smaller the value, the lower the volume.
     * @param connection The connection information. See `RtcConnection`.
     *
     * @return
     * - 0: Success.
     * - < 0: Failure.
     */
    virtual int setRemoteVoicePositionEx(uid_t uid, double pan, double gain, const RtcConnection& connection) = 0;
    /** Sets remote user parameters for spatial audio

    @param uid The ID of the remote user.
    @param param Spatial audio parameters. See SpatialAudioParams.
    @param connection The RtcConnection object.

    @return int
    - 0: Success.
    - < 0: Failure.
    */
    virtual int setRemoteUserSpatialAudioParamsEx(uid_t uid, const agora::SpatialAudioParams& params, const RtcConnection& connection) = 0;
    /**
     * @brief Sets the video display mode of a specified remote user.
     *
     * @details
     * After initializing the video view of a remote user, you can call this method to update its
     * rendering and mirror modes. This method affects only the video view that the local user sees.
     *
     * @note
     * - Call this method after initializing the remote view by calling the `setupRemoteVideo` method.
     * - During a call, you can call this method as many times as necessary to update the display mode
     * of the video view of a remote user.
     *
     * @param uid The user ID of the remote user.
     * @param renderMode The video display mode of the remote user. See `RENDER_MODE_TYPE`.
     * @param mirrorMode The mirror mode of the remote user view. See `VIDEO_MIRROR_MODE_TYPE`.
     * @param connection The connection information. See `RtcConnection`.
     *
     * @return
     * - 0: Success.
     * - < 0: Failure.
     */
    virtual int setRemoteRenderModeEx(uid_t uid, media::base::RENDER_MODE_TYPE renderMode,
                                      VIDEO_MIRROR_MODE_TYPE mirrorMode, const RtcConnection& connection) = 0;
    /**
     * @brief Enables loopback audio capturing.
     *
     * @details
     * If you enable loopback audio capturing, the output of the sound card is mixed into the audio
     * stream sent to the other end.
     *
     * @note
     * - This method applies to the macOS and Windows only.
     * - macOS does not support loopback audio capture of the default sound card. If you need to use
     * this function, use a virtual sound card and pass its name to the `deviceName` parameter. Agora
     * recommends using AgoraALD as the virtual sound card for audio capturing.
     * - This method only supports using one sound card for audio capturing.
     *
     * @param connection The connection information. See `RtcConnection`.
     * @param enabled Sets whether to enable loopback audio capture:
     * - `true`: Enable loopback audio capturing.
     * - `false`: (Default) Disable loopback audio capturing.
     * @param deviceName - macOS: The device name of the virtual sound card. The default value is set to
     * NULL, which means using AgoraALD for loopback audio capturing.
     * - Windows: The device name of the sound card. The default is set to NULL, which means the SDK
     * uses the sound card of your device for loopback audio capturing.
     *
     * @return
     * - 0: Success.
     * - < 0: Failure.
     */
    virtual int enableLoopbackRecordingEx(const RtcConnection& connection, bool enabled, const char* deviceName = NULL) = 0;

    /**
     * Adjusts the recording volume.
     *
     * @param volume The recording volume, which ranges from 0 to 400:
     * - 0  : Mute the recording volume.
     * - 100: The original volume.
     * - 400: (Maximum) Four times the original volume with signal clipping protection.
     *
     * @param connection The RtcConnection object.
     *
     * @return
     * - 0  : Success.
     * - < 0: Failure.
     */
    virtual int adjustRecordingSignalVolumeEx(int volume, const RtcConnection& connection) = 0;

    /**
     * Mute or resume recording signal volume.
     *
     * @param mute Determines whether to mute or resume the recording signal volume.
     * -  true: Mute the recording signal volume.
     * - false: (Default) Resume the recording signal volume.
     *
     * @param connection The RtcConnection object.
     *
     * @return
     * - 0  : Success.
     * - < 0: Failure.
     */
    virtual int muteRecordingSignalEx(bool mute, const RtcConnection& connection) = 0;

    /**
     * @brief Adjusts the playback signal volume of a specified remote user.
     *
     * @details
     * You can call this method to adjust the playback volume of a specified remote user. To adjust the
     * playback volume of different remote users, call the method as many times, once for each remote
     * user.
     * Call timing: Call this method after `joinChannelEx`.
     *
     * @param uid The user ID of the remote user.
     * @param volume The volume of the user. The value range is [0,400].
     * - 0: Mute.
     * - 100: (Default) The original volume.
     * - 400: Four times the original volume (amplifying the audio signals by four times).
     * @param connection The connection information. See `RtcConnection`.
     *
     * @return
     * - 0: Success.
     * - < 0: Failure.
     */
    virtual int adjustUserPlaybackSignalVolumeEx(uid_t uid, int volume, const RtcConnection& connection) = 0;

    /**
     * @brief Gets the current connection state of the SDK.
     *
     * @details
     * Call timing: This method can be called either before or after joining the channel.
     *
     * @param connection The connection information. See `RtcConnection`.
     *
     * @return
     * The current connection state. See `CONNECTION_STATE_TYPE`.
     */
    virtual CONNECTION_STATE_TYPE getConnectionStateEx(const RtcConnection& connection) = 0;
    /**
     * @brief Enables or disables the built-in encryption.
     *
     * @details
     * After the user leaves the channel, the SDK automatically disables the built-in encryption. To
     * enable the built-in encryption, call this method before the user joins the channel again.
     * Applicable scenarios: Scenarios with higher security requirements.
     * Call timing: Call this method before joining a channel.
     *
     * @note
     * - All users within the same channel must set the same encryption configurations when calling this
     * method.
     * - If you enable the built-in encryption, you cannot use the Media Push function.
     *
     * @param enabled Whether to enable built-in encryption:
     * - true: Enable the built-in encryption.
     * - false: (Default) Disable the built-in encryption.
     * @param config Built-in encryption configurations. See `EncryptionConfig`.
     * @param connection The connection information. See `RtcConnection`.
     *
     * @return
     * - 0: Success.
     * - < 0: Failure.
     */
    virtual int enableEncryptionEx(const RtcConnection& connection, bool enabled, const EncryptionConfig& config) = 0;
    /**
     * @brief Creates a data stream.
     *
     * @details
     * You can call this method to create a data stream and improve the reliability and ordering of data
     * transmission.
     * Call timing: Call this method after `joinChannelEx`.
     * Related callbacks: After setting `reliable` to `true`, if the recipient does not receive the data
     * within five seconds, the SDK triggers the `onStreamMessageError` callback and returns an error
     * code.
     *
     * @note Each user can create up to five data streams during the lifecycle of `IRtcEngine`. The data
     * stream will be destroyed when leaving the channel, and the data stream needs to be recreated if
     * needed.
     *
     * @param streamId An output parameter; the ID of the data stream created.
     * @param reliable Sets whether the recipients are guaranteed to receive the data stream within five
     * seconds:
     * - `true`: The recipients receive the data from the sender within five seconds. If the recipient
     * does not receive the data within five seconds, the SDK triggers the `onStreamMessageError`
     * callback and returns an error code.
     * - `false`: There is no guarantee that the recipients receive the data stream within five seconds
     * and no error message is reported for any delay or missing data stream.
     * Attention: Please ensure that `reliable` and `ordered` are either both set to`true` or both set
     * to `false`.
     * @param ordered Sets whether the recipients receive the data stream in the sent order:
     * - `true`: The recipients receive the data in the sent order.
     * - `false`: The recipients do not receive the data in the sent order.
     * @param connection The connection information. See `RtcConnection`.
     *
     * @return
     * - 0: The data stream is successfully created.
     * - < 0: Failure.
     */
    virtual int createDataStreamEx(int* streamId, bool reliable, bool ordered, const RtcConnection& connection) = 0;
    /**
     * @brief Creates a data stream.
     *
     * @details
     * Compared to `createDataStreamEx(int* streamId, bool reliable, bool ordered, const RtcConnection&
     * connection)`, this method does not guarantee the reliability of data
     * transmission. If a data packet is not received five seconds after it was sent, the SDK directly
     * discards the data.
     * Call timing: Call this method after `joinChannelEx`.
     *
     * @note
     * Each user can create up to five data streams during the lifecycle of `IRtcEngine`. The data
     * stream will be destroyed when leaving the channel, and the data stream needs to be recreated if
     * needed.
     * If you need a more comprehensive solution for low-latency, high-concurrency, and scalable
     * real-time messaging and status synchronization, it is recommended to use `Signaling`.
     *
     * @param streamId An output parameter; the ID of the data stream created.
     * @param config The configurations for the data stream. See `DataStreamConfig`.
     * @param connection The connection information. See `RtcConnection`.
     *
     * @return
     * - 0: The data stream is successfully created.
     * - < 0: Failure.
     */
    virtual int createDataStreamEx(int* streamId, const DataStreamConfig& config, const RtcConnection& connection) = 0;
    /**
     * @brief Sends data stream messages.
     *
     * @details
     * After calling `createDataStreamEx(int* streamId, const DataStreamConfig& config, const
     * RtcConnection& connection)`, you can call this method to send data stream messages
     * to all users in the channel.
     * The SDK has the following restrictions on this method:
     * - Each client within the channel can have up to 5 data channels simultaneously, with a total
     * shared packet bitrate limit of 30 KB/s for all data channels.
     * - Each data channel can send up to 60 packets per second, with each packet being a maximum of 1
     * KB.
     * A successful method call triggers the `onStreamMessage` callback on the remote client, from which
     * the remote user gets the stream message. A failed method call triggers the `onStreamMessageError`
     * callback on the remote client.
     *
     * @note
     * - If you need a more comprehensive solution for low-latency, high-concurrency, and scalable
     * real-time messaging and status synchronization, it is recommended to use `Signaling`.
     * - Call this method after `joinChannelEx`.
     * - Ensure that you call `createDataStreamEx(int* streamId, const DataStreamConfig& config, const
     * RtcConnection& connection)` to create a data channel before calling this
     * method.
     *
     * @param streamId The data stream ID. You can get the data stream ID by calling
     * `createDataStreamEx(int* streamId, const DataStreamConfig& config, const RtcConnection&
     * connection)`
     * .
     * @param data The message to be sent.
     * @param length The length of the data.
     * @param connection The connection information. See `RtcConnection`.
     *
     * @return
     * - 0: Success.
     * - < 0: Failure.
     */
    virtual int sendStreamMessageEx(int streamId, const char* data, size_t length, const RtcConnection& connection) = 0;

    /**
     * @brief Send Reliable message to remote uid in channel.
     *
     * @technical preview
     *
     * @param uid Remote user id.
     * @param type Reliable Data Transmission tunnel message type. See RdtStreamType
     * @param data The pointer to the sent data.
     * @param length The length of the sent data.
     * @param connection The RtcConnection object.
     * @return
     * - 0: Success.
     * - < 0: Failure.
     */
    virtual int sendRdtMessageEx(uid_t uid, RdtStreamType type, const char *data, size_t length, const RtcConnection& connection) = 0;

    /**
     * @brief Send media control message
     *
     * @technical preview
     *
     * @param uid Remote user id. In particular, if the uid is set to 0, it means broadcasting the message to the entire channel.
     * @param data The pointer to the sent data.
     * @param length The length of the sent data, max 1024.
     * @param connection The RtcConnection object.
     * @return
     * - 0: Success.
     * - < 0: Failure.
     */
    virtual int sendMediaControlMessageEx(uid_t uid, const char *data, size_t length, const RtcConnection& connection) = 0;

    /**
     * @brief Adds a watermark image to the local video.
     *
     * @deprecated v4.6.0. This method is deprecated. Use addVideoWatermarkEx(const WatermarkConfig&
     * config, const RtcConnection& connection) instead.
     *
     * @details
     * This method adds a PNG watermark image to the local video in the live streaming. Once the
     * watermark image is added, all the audience in the channel (CDN audience included), and the
     * capturing device can see and capture it. The Agora SDK supports adding only one watermark image
     * onto a live video stream. The newly added watermark image replaces the previous one.
     * The watermark coordinates are dependent on the settings in the `setVideoEncoderConfigurationEx`
     * method:
     * - If the orientation mode of the encoding video ( `ORIENTATION_MODE` ) is fixed landscape mode or
     * the adaptive landscape mode, the watermark uses the landscape orientation.
     * - If the orientation mode of the encoding video ( `ORIENTATION_MODE` ) is fixed portrait mode or
     * the adaptive portrait mode, the watermark uses the portrait orientation.
     * - When setting the watermark position, the region must be less than the dimensions set in the
     * `setVideoEncoderConfigurationEx` method; otherwise, the watermark image will be cropped.
     *
     * @note
     * - Ensure that you have called `enableVideo` before calling this method.
     * - This method supports adding a watermark image in the PNG file format only. Supported pixel
     * formats of the PNG image are RGBA, RGB, Palette, Gray, and Alpha_gray.
     * - If the dimensions of the PNG image differ from your settings in this method, the image will be
     * cropped or zoomed to conform to your settings.
     * - If you have enabled the local video preview by calling the `startPreview(VIDEO_SOURCE_TYPE
     * sourceType)` method, you can
     * use the `visibleInPreview` member to set whether or not the watermark is visible in the preview.
     * - If you have enabled the mirror mode for the local video, the watermark on the local video is
     * also mirrored. To avoid mirroring the watermark, Agora recommends that you do not use the mirror
     * and watermark functions for the local video at the same time. You can implement the watermark
     * function in your application layer.
     *
     * @param watermarkUrl The local file path of the watermark image to be added. This method supports
     * adding a watermark image from the local absolute or relative file path.
     * @param options The options of the watermark image to be added. See `WatermarkOptions`.
     * @param connection The connection information. See `RtcConnection`.
     *
     * @return
     * - 0: Success.
     * - < 0: Failure.
     */
    virtual int addVideoWatermarkEx(const char* watermarkUrl, const WatermarkOptions& options, const RtcConnection& connection) = 0;

    /**
     * @brief Adds a watermark image to the local video.
     *
     * @since 4.6.0
     *
     * @details
     * Applicable scenarios: This method applies to multi-channel scenarios.
     *
     * @param config Watermark configuration. See `WatermarkConfig`.
     * @param connection `RtcConnection` object. See `RtcConnection`.
     *
     * @return
     * - 0: Success.
     * - < 0: Failure.
     */
    virtual int addVideoWatermarkEx(const WatermarkConfig& config, const RtcConnection& connection) = 0;

    /**
     * @brief Removes the specified watermark image from the local or remote video stream.
     *
     * @since 4.6.0
     *
     * @details
     * Applicable scenarios: This method applies to multi-channel scenarios.
     *
     * @param id Watermark ID.
     * @param connection `RtcConnection` object. See `RtcConnection`.
     *
     * @return
     * - 0: Success.
     * - < 0: Failure.
     */
    virtual int removeVideoWatermarkEx(const char* id, const RtcConnection& connection) = 0;

    /**
     * @brief Removes the watermark image from the video stream.
     *
     * @param connection The connection information. See `RtcConnection`.
     *
     * @return
     * - 0: Success.
     * - < 0: Failure.
     */
    virtual int clearVideoWatermarkEx(const RtcConnection& connection) = 0;
    /**
     * @brief Agora supports reporting and analyzing customized messages.
     *
     * @details
     * Agora supports reporting and analyzing customized messages. This function is in the beta stage
     * with a free trial. The ability provided in its beta test version is reporting a maximum of 10
     * message pieces within 6 seconds, with each message piece not exceeding 256 bytes and each string
     * not exceeding 100 bytes. To try out this function, contact `support@agora.io` and discuss the
     * format of customized messages with us.
     *
     */
    virtual int sendCustomReportMessageEx(const char* id, const char* category, const char* event, const char* label,
                                          int value, const RtcConnection& connection) = 0;

    /**
     * @brief Enables the reporting of users' volume indication.
     *
     * @details
     * This method enables the SDK to regularly report the volume information to the app of the local
     * user who sends a stream and remote users (three users at most) whose instantaneous volumes are
     * the highest.
     * Call timing: Call this method after `joinChannelEx`.
     * Related callbacks: The SDK triggers the `onAudioVolumeIndication` callback according to the
     * interval you set if this method is successfully called and there are users publishing streams in
     * the channel.
     *
     * @param interval Sets the time interval between two consecutive volume indications:
     * - ≤ 0: Disables the volume indication.
     * - > 0: Time interval (ms) between two consecutive volume indications. Ensure this parameter is
     * set to a value greater than 10, otherwise you will not receive the `onAudioVolumeIndication`
     * callback. Agora recommends that this value is set as greater than 100.
     * @param smooth The smoothing factor that sets the sensitivity of the audio volume indicator. The
     * value ranges between 0 and 10. The recommended value is 3. The greater the value, the more
     * sensitive the indicator.
     * @param reportVad - `true`: Enables the voice activity detection of the local user. Once it is
     * enabled, the `vad` parameter of the `onAudioVolumeIndication` callback reports the voice activity
     * status of the local user.
     * - `false`: (Default) Disables the voice activity detection of the local user. Once it is
     * disabled, the `vad` parameter of the `onAudioVolumeIndication` callback does not report the voice
     * activity status of the local user, except for the scenario where the engine automatically detects
     * the voice activity of the local user.
     * @param connection The connection information. See `RtcConnection`.
     *
     * @return
     * - 0: Success.
     * - < 0: Failure.
     */
    virtual int enableAudioVolumeIndicationEx(int interval, int smooth, bool reportVad, const RtcConnection& connection) = 0;

    /**
     * @brief Starts pushing media streams to a CDN without transcoding.
     *
     * @details
     * Agora recommends that you use the server-side Media Push function. For details, see `Use RESTful
     * API`.
     * You can call this method to push an audio or video stream to the specified CDN address. This
     * method can push media streams to only one CDN address at a time, so if you need to push streams
     * to multiple addresses, call this method multiple times.
     * After you call this method, the SDK triggers the `onRtmpStreamingStateChanged` callback on the
     * local client to report the state of the streaming.
     *
     * @note
     * - Call this method after joining a channel.
     * - Only hosts in the LIVE_BROADCASTING profile can call this method.
     * - If you want to retry pushing streams after a failed push, make sure to call `stopRtmpStream`
     * first, then call this method to retry pushing streams; otherwise, the SDK returns the same error
     * code as the last failed push.
     *
     * @param url The address of Media Push. The format is RTMP or RTMPS. The character length cannot
     * exceed 1024 bytes. Special characters such as Chinese characters are not supported.
     * @param connection The connection information. See `RtcConnection`.
     *
     * @return
     * - 0: Success.
     * - < 0: Failure.
     *   - -2: The URL or configuration of transcoding is invalid; check your URL and transcoding
     * configurations.
     *   - -7: The SDK is not initialized before calling this method.
     *   - -19: The Media Push URL is already in use; use another URL instead.
     */
    virtual int startRtmpStreamWithoutTranscodingEx(const char* url, const RtcConnection& connection) = 0;

    /**
     * @brief Starts Media Push and sets the transcoding configuration.
     *
     * @details
     * Agora recommends that you use the server-side Media Push function. For details, see `Use RESTful
     * API`.
     * You can call this method to push a live audio-and-video stream to the specified CDN address and
     * set the transcoding configuration. This method can push media streams to only one CDN address at
     * a time, so if you need to push streams to multiple addresses, call this method multiple times.
     * After you call this method, the SDK triggers the `onRtmpStreamingStateChanged` callback on the
     * local client to report the state of the streaming.
     *
     * @note
     * - Ensure that you enable the Media Push service before using this function.
     * - Call this method after joining a channel.
     * - Only hosts in the LIVE_BROADCASTING profile can call this method.
     * - If you want to retry pushing streams after a failed push, make sure to call `stopRtmpStreamEx`
     * first, then call this method to retry pushing streams; otherwise, the SDK returns the same error
     * code as the last failed push.
     *
     * @param url The address of Media Push. The format is RTMP or RTMPS. The character length cannot
     * exceed 1024 bytes. Special characters such as Chinese characters are not supported.
     * @param transcoding The transcoding configuration for Media Push. See `LiveTranscoding`.
     * @param connection The connection information. See `RtcConnection`.
     *
     * @return
     * - 0: Success.
     * - < 0: Failure.
     *   - -2: The URL or configuration of transcoding is invalid; check your URL and transcoding
     * configurations.
     *   - -7: The SDK is not initialized before calling this method.
     *   - -19: The Media Push URL is already in use; use another URL instead.
     */
    virtual int startRtmpStreamWithTranscodingEx(const char* url, const LiveTranscoding& transcoding, const RtcConnection& connection) = 0;

    /**
     * @brief Updates the transcoding configuration.
     *
     * @details
     * Agora recommends that you use the server-side Media Push function. For details, see `Use RESTful
     * API`.
     * After you start pushing media streams to CDN with transcoding, you can dynamically update the
     * transcoding configuration according to the scenario. The SDK triggers the `onTranscodingUpdated`
     * callback after the transcoding configuration is updated.
     *
     * @param transcoding The transcoding configuration for Media Push. See `LiveTranscoding`.
     * @param connection The connection information. See `RtcConnection`.
     *
     * @return
     * - 0: Success.
     * - < 0: Failure.
     */
    virtual int updateRtmpTranscodingEx(const LiveTranscoding& transcoding, const RtcConnection& connection) = 0;

    /**
     * @brief Stops pushing media streams to a CDN.
     *
     * @details
     * Agora recommends that you use the server-side Media Push function. For details, see `Use RESTful
     * API`.
     * You can call this method to stop the live stream on the specified CDN address. This method can
     * stop pushing media streams to only one CDN address at a time, so if you need to stop pushing
     * streams to multiple addresses, call this method multiple times.
     * After you call this method, the SDK triggers the `onRtmpStreamingStateChanged` callback on the
     * local client to report the state of the streaming.
     *
     * @param url The address of Media Push. The format is RTMP or RTMPS. The character length cannot
     * exceed 1024 bytes. Special characters such as Chinese characters are not supported.
     * @param connection The connection information. See `RtcConnection`.
     *
     * @return
     * - 0: Success.
     * - < 0: Failure.
     */
    virtual int stopRtmpStreamEx(const char* url, const RtcConnection& connection) = 0;

    /**
     * @brief Starts relaying media streams across channels or updates channels for media relay.
     *
     * @since v4.2.0
     *
     * @details
     * The first successful call to this method starts relaying media streams from the source channel to
     * the destination channels. To relay the media stream to other channels, or exit one of the current
     * media relays, you can call this method again to update the destination channels. This feature
     * supports relaying media streams to a maximum of six destination channels.
     * After a successful method call, the SDK triggers the `onChannelMediaRelayStateChanged` callback,
     * and this callback returns the state of the media stream relay. Common states are as follows:
     * - If the `onChannelMediaRelayStateChanged` callback returns `RELAY_STATE_RUNNING` (2) and
     * `RELAY_OK` (0), it means that the SDK starts relaying media streams from the source channel to
     * the destination channel.
     * - If the `onChannelMediaRelayStateChanged` callback returns `RELAY_STATE_FAILURE` (3), an
     * exception occurs during the media stream relay.
     *
     * @note
     * - Call this method after joining the channel.
     * - This method takes effect only when you are a host in a live streaming channel.
     * - The relaying media streams across channels function needs to be enabled by contacting
     * `technical support`.
     * - Agora does not support string user accounts in this API.
     *
     * @param configuration The configuration of the media stream relay. See
     * `ChannelMediaRelayConfiguration`.
     * @param connection The connection information. See `RtcConnection`.
     *
     * @return
     * - 0: Success.
     * - < 0: Failure.
     *   - -1: A general error occurs (no specified reason).
     *   - -2: The parameter is invalid.
     *   - -8: Internal state error. Probably because the user is not a broadcaster.
     */
    virtual int startOrUpdateChannelMediaRelayEx(const ChannelMediaRelayConfiguration& configuration, const RtcConnection& connection) = 0;

    /**
     * @brief Stops the media stream relay. Once the relay stops, the host quits all the target
     * channels.
     *
     * @details
     * After a successful method call, the SDK triggers the `onChannelMediaRelayStateChanged` callback.
     * If the callback reports `RELAY_STATE_IDLE` (0) and `RELAY_OK` (0), the host successfully stops
     * the relay.
     *
     * @note If the method call fails, the SDK triggers the `onChannelMediaRelayStateChanged` callback
     * with the `RELAY_ERROR_SERVER_NO_RESPONSE` (2) or `RELAY_ERROR_SERVER_CONNECTION_LOST` (8) status
     * code. You can call the `leaveChannel(const LeaveChannelOptions& options)` method to leave the
     * channel, and the media stream
     * relay automatically stops.
     *
     * @param connection The connection information. See `RtcConnection`.
     *
     * @return
     * - 0: Success.
     * - < 0: Failure.
     *   - -5: The method call was rejected. There is no ongoing channel media relay.
     */
    virtual int stopChannelMediaRelayEx(const RtcConnection& connection) = 0;

    /**
     * @brief Pauses the media stream relay to all target channels.
     *
     * @details
     * After the cross-channel media stream relay starts, you can call this method to pause relaying
     * media streams to all target channels; after the pause, if you want to resume the relay, call
     * `resumeAllChannelMediaRelay`.
     *
     * @note Call this method after `startOrUpdateChannelMediaRelayEx`.
     *
     * @param connection The connection information. See `RtcConnection`.
     *
     * @return
     * - 0: Success.
     * - < 0: Failure.
     *   - -5: The method call was rejected. There is no ongoing channel media relay.
     */
    virtual int pauseAllChannelMediaRelayEx(const RtcConnection& connection) = 0;

    /**
     * @brief Resumes the media stream relay to all target channels.
     *
     * @details
     * After calling the `pauseAllChannelMediaRelayEx` method, you can call this method to resume
     * relaying media streams to all destination channels.
     *
     * @note Call this method after `pauseAllChannelMediaRelayEx`.
     *
     * @param connection The connection information. See `RtcConnection`.
     *
     * @return
     * - 0: Success.
     * - < 0: Failure.
     *   - -5: The method call was rejected. There is no paused channel media relay.
     */
    virtual int resumeAllChannelMediaRelayEx(const RtcConnection& connection) = 0;

   /** Gets the user information by passing in the user account.
    *  It is same as agora::rtc::IRtcEngine::getUserInfoByUserAccount.
    *
    * @param userAccount The user account of the user. Ensure that you set this parameter.
    * @param [in,out] userInfo  A userInfo object that identifies the user:
    * - Input: A userInfo object.
    * - Output: A userInfo object that contains the user account and user ID of the user.
    * @param connection The RtcConnection object.
    *
    * @return
    * - 0: Success.
    * - < 0: Failure.
    */
    virtual int getUserInfoByUserAccountEx(const char* userAccount, rtc::UserInfo* userInfo, const RtcConnection& connection) = 0;

    /** Gets the user information by passing in the user ID.
    *  It is same as agora::rtc::IRtcEngine::getUserInfoByUid.
    *
    * @param uid The user ID of the remote user. Ensure that you set this parameter.
    * @param[in,out] userInfo A userInfo object that identifies the user:
    * - Input: A userInfo object.
    * - Output: A userInfo object that contains the user account and user ID of the user.
    * @param connection The RtcConnection object.
    *
    * @return
    * - 0: Success.
    * - < 0: Failure.
    */
    virtual int getUserInfoByUidEx(uid_t uid, rtc::UserInfo* userInfo, const RtcConnection& connection) = 0;

    /**
     * @brief Enables or disables dual-stream mode on the sender side.
     *
     * @deprecated v4.2.0. This method is deprecated. Use setDualStreamModeEx instead
     *
     * @details
     * You can call this method to enable or disable the dual-stream mode on the publisher side. Dual
     * streams are a pairing of a high-quality video stream and a low-quality video stream:
     * - High-quality video stream: High bitrate, high resolution.
     * - Low-quality video stream: Low bitrate, low resolution.
     * After you enable dual-stream mode, you can call `setRemoteVideoStreamType` to choose to receive
     * either the high-quality video stream or the low-quality video stream on the subscriber side.
     *
     * @note This method is applicable to all types of streams from the sender, including but not
     * limited to video streams collected from cameras, screen sharing streams, and custom-collected
     * video streams.
     *
     * @param enabled Whether to enable dual-stream mode:
     * - `true`: Enable dual-stream mode.
     * - `false`: (Default) Disable dual-stream mode.
     * @param streamConfig The configuration of the low-quality video stream. See
     * `SimulcastStreamConfig`.Note: When setting `mode` to `DISABLE_SIMULCAST_STREAM`, setting
     * `streamConfig` will not take effect.
     * @param connection The connection information. See `RtcConnection`.
     *
     * @return
     * - 0: Success.
     * - < 0: Failure.
     */
    virtual int enableDualStreamModeEx(bool enabled, const SimulcastStreamConfig& streamConfig,
                                       const RtcConnection& connection) = 0;
    /**
     * @brief Sets the dual-stream mode on the sender side.
     *
     * @details
     * The SDK defaults to enabling low-quality video stream adaptive mode ( `AUTO_SIMULCAST_STREAM` )
     * on the sender side, which means the sender does not actively send low-quality video stream. The
     * receiving end with the role of the **host** can initiate a low-quality video stream request by
     * calling `setRemoteVideoStreamTypeEx`, and upon receiving the request, the sending end
     * automatically starts sending low-quality stream.
     * - If you want to modify this behavior, you can call this method and set `mode` to
     * `DISABLE_SIMULCAST_STREAM` (never send low-quality video streams) or `ENABLE_SIMULCAST_STREAM`
     * (always send low-quality video streams).
     * - If you want to restore the default behavior after making changes, you can call this method
     * again with `mode` set to `AUTO_SIMULCAST_STREAM`.
     *
     * @note
     * The difference and connection between this method and `enableDualStreamModeEx` is as follows:
     * - When calling this method and setting `mode` to DISABLE_SIMULCAST_STREAM, it has the same effect
     * as `enableDualStreamModeEx` `(false)`.
     * - When calling this method and setting `mode` to ENABLE_SIMULCAST_STREAM, it has the same effect
     * as `enableDualStreamModeEx` `(true)`.
     * - Both methods can be called before and after joining a channel. If both methods are used, the
     * settings in the method called later takes precedence.
     *
     * @param mode The mode in which the video stream is sent. See `SIMULCAST_STREAM_MODE`.
     * @param streamConfig The configuration of the low-quality video stream. See
     * `SimulcastStreamConfig`.Note: When setting `mode` to `DISABLE_SIMULCAST_STREAM`, setting
     * `streamConfig` will not take effect.
     * @param connection The connection information. See `RtcConnection`.
     *
     * @return
     * - 0: Success.
     * - < 0: Failure.
     */
    virtual int setDualStreamModeEx(SIMULCAST_STREAM_MODE mode,
                                   const SimulcastStreamConfig& streamConfig,
                                   const RtcConnection& connection) = 0;

    /**
     * @brief Sets the simulcast video stream configuration.
     *
     * @since v4.6.0
     *
     * @details
     * This method can be called in scenarios involving multiple channels. You can call the
     * `setSimulcastConfig` method to set video streams with different resolutions for the same video
     * source. The subscribers can call to select which stream layer to receive. The broadcaster can
     * publish up to four layers of video streams: one main stream (highest resolution) and three
     * additional streams of different quality levels. `setRemoteVideoStreamType`
     * Applicable scenarios: This method can be called in scenarios involving multiple channels.
     *
     * @param simulcastConfig This configuration includes seven layers, from STREAM_LAYER_1 to
     * STREAM_LOW, with a maximum of three layers enabled simultaneously. See `SimulcastConfig`.
     * @param connection Connection information. See `RtcConnection`.
     *
     * @return
     * - 0: Success.
     * - < 0: Failure.
     */
    virtual int setSimulcastConfigEx(const SimulcastConfig& simulcastConfig,
                                     const RtcConnection& connection) = 0;

  /**
    * Set the high priority user list and their fallback level in weak network condition.
    *
    * @note
    * - This method can be called before and after joining a channel.
    * - If a subscriber is set to high priority, this stream only fallback to lower stream after all normal priority users fallback to their fallback level on weak network condition if needed.
    *
    * @param uidList The high priority user list.
    * @param uidNum The size of uidList.
    * @param option The fallback level of high priority users.
    * @param connection An output parameter which is used to control different connection instances.
    *
    * @return int
    * - 0 : Success.
    * - <0 : Failure.
    */
    virtual int setHighPriorityUserListEx(uid_t* uidList, int uidNum,
                                          STREAM_FALLBACK_OPTIONS option,
                                          const RtcConnection& connection) = 0;

    /**
     * @brief Takes a snapshot of a video stream using connection ID.
     *
     * @details
     * This method takes a snapshot of a video stream from the specified user, generates a JPG image,
     * and saves it to the specified path.
     * Call timing: Call this method after `joinChannelEx`.
     * Related callbacks: After a successful call of this method, the SDK triggers the `onSnapshotTaken`
     * callback to report whether the snapshot is successfully taken, as well as the details for that
     * snapshot.
     *
     * @note
     * - The method is asynchronous, and the SDK has not taken the snapshot when the method call
     * returns.
     * - When used for local video snapshots, this method takes a snapshot for the video streams
     * specified in `ChannelMediaOptions`.
     * - If the user's video has been preprocessed, for example, watermarked or beautified, the
     * resulting snapshot includes the pre-processing effect.
     *
     * @param connection The connection information. See `RtcConnection`.
     * @param uid The user ID. Set uid as 0 if you want to take a snapshot of the local user's video.
     * @param filePath The local path (including filename extensions) of the snapshot. For example:
     * - Windows: `C:\Users\<user_name>\AppData\Local\Agora\<process_name>\example.jpg`
     * - iOS: `/App Sandbox/Library/Caches/example.jpg`
     * - macOS: `～/Library/Logs/example.jpg`
     * - Android: `/storage/emulated/0/Android/data/<package name>/files/example.jpg`
     * Attention: Ensure that the path you specify exists and is writable.
     *
     * @return
     * - 0: Success.
     * - < 0: Failure.
     */
    virtual int takeSnapshotEx(const RtcConnection& connection, uid_t uid, const char* filePath)  = 0;

    /**
     * @brief Gets a video screenshot of the specified observation point using the connection ID.
     *
     * @details
     * This method takes a snapshot of a video stream from the specified user, generates a JPG image,
     * and saves it to the specified path.
     * Call timing: Call this method after `joinChannelEx`.
     * Related callbacks: After a successful call of this method, the SDK triggers the `onSnapshotTaken`
     * callback to report whether the snapshot is successfully taken, as well as the details for that
     * snapshot.
     *
     * @note
     * - The method is asynchronous, and the SDK has not taken the snapshot when the method call
     * returns.
     * - When used for local video snapshots, this method takes a snapshot for the video streams
     * specified in `ChannelMediaOptions`.
     * - If the user's video has been preprocessed, for example, watermarked or beautified, the
     * resulting snapshot includes the pre-processing effect.
     *
     * @param connection The connection information. See `RtcConnection`.
     * @param uid The user ID. Set uid as 0 if you want to take a snapshot of the local user's video.
     * @param config The configuration of the snaptshot. See `SnapshotConfig`.
     *
     * @return
     * - 0: Success.
     * - < 0: Failure.
     */
    virtual int takeSnapshotEx(const RtcConnection& connection, uid_t uid, const media::SnapshotConfig& config)  = 0;

    /**
     * @brief Enables or disables video screenshot and upload.
     *
     * @details
     * This method can take screenshots for multiple video streams and upload them. When video
     * screenshot and upload function is enabled, the SDK takes screenshots and uploads videos sent by
     * local users based on the type and frequency of the module you set in `ContentInspectConfig`.
     * After video screenshot and upload, the Agora server sends the callback notification to your app
     * server in HTTPS requests and sends all screenshots to the third-party cloud storage service.
     * Call timing: This method can be called either before or after joining the channel.
     *
     * @note Before calling this method, ensure that you have contacted `technical support` to activate
     * the video screenshot upload service.
     *
     * @param enabled Whether to enalbe video screenshot and upload:
     * - `true`: Enables video screenshot and upload.
     * - `false`: Disables video screenshot and upload.
     * @param config Screenshot and upload configuration. See `ContentInspectConfig`.
     * @param connection The connection information. See `RtcConnection`.
     *
     * @return
     * - 0: Success.
     * - < 0: Failure.
     */
    virtual int enableContentInspectEx(bool enabled, const media::ContentInspectConfig &config, const RtcConnection& connection) = 0;

    /**
     * @brief Enables tracing the video frame rendering process.
     *
     * @since v4.1.1
     *
     * @details
     * The SDK starts tracing the rendering status of the video frames in the channel from the moment
     * this method is successfully called and reports information about the event through the
     * `onVideoRenderingTracingResult` callback.
     * Applicable scenarios: Agora recommends that you use this method in conjunction with the UI
     * settings (such as buttons and sliders) in your app to improve the user experience. For example,
     * call this method when the user clicks the Join Channel button, and then get the time spent during
     * the video frame rendering process through the `onVideoRenderingTracingResult` callback, so as to
     * optimize the indicators accordingly.
     *
     * @note
     * - If you have not called this method, the SDK tracks the rendering events of the video frames
     * from the moment you call `joinChannel(const char* token, const char* channelId, uid_t uid, const
     * ChannelMediaOptions& options)` to join the channel. You can call this method at an
     * appropriate time according to the actual application scenario to set the starting position for
     * tracking video rendering events.
     * - After the local user leaves the current channel, the SDK automatically tracks the video
     * rendering events from the moment you join a channel.
     *
     * @param connection The connection information. See `RtcConnection`.
     *
     * @return
     * - 0: Success.
     * - < 0: Failure.
     */
    virtual int startMediaRenderingTracingEx(const RtcConnection& connection) = 0;

    /** Provides the technical preview functionalities or special customizations by configuring the SDK with JSON options.
    @since v4.3.0
    @param connection The connection information. See RtcConnection.
    @param parameters Pointer to the set parameters in a JSON string.
    @return
    - 0: Success.
    - < 0: Failure.
    */
    virtual int setParametersEx(const RtcConnection& connection, const char* parameters) = 0;

    /**
     * @brief Gets the call ID with the connection ID.
     *
     * @details
     * When a user joins a channel on a client, a `callId` is generated to identify the call from the
     * client. You can call this method to get `callId`, and pass it in when calling methods such as
     * `rate` and `complain`.
     * Call timing: Call this method after joining a channel.
     *
     * @param callId Output parameter, the current call ID.
     * @param connection The connection information. See `RtcConnection`.
     *
     */
    virtual int getCallIdEx(agora::util::AString& callId, const RtcConnection& connection) = 0;

    /**
     * send audio metadata
     * @since v4.3.1
     * @param connection The RtcConnection object.
     * @param metadata The pointer of metadata
     * @param length Size of metadata
     * @return
     * - 0: success
     * - <0: failure
     * @technical preview
    */
    virtual int sendAudioMetadataEx(const RtcConnection& connection, const char* metadata, size_t length) = 0;

    /**
     * @brief Preloads a specified sound effect to a channel.
     *
     * @since v4.6.0
     *
     * @details
     * Each time you call this method, you can only preload one sound effect file into memory. If you
     * need to preload multiple sound files, please call this method multiple times. After preloading is
     * complete, you can call `playEffect` to play the preloaded sound effects, or call `playAllEffects`
     * to play all preloaded sound effects.
     * Applicable scenarios: This method can be called in scenarios involving multiple channels.
     *
     * @note
     * - To ensure a smooth experience, the size of sound effect files should not exceed the limit.
     * - Agora recommends that you call this method before joining a channel.
     * - If preloadEffectEx is called before playEffectEx is executed, the file resource will not be closed after playEffectEx. 
     * The next time playEffectEx is executed, it will directly seek to play at the beginning.
     * - If preloadEffectEx is not called before playEffectEx is executed, the resource will be destroyed after playEffectEx. 
     * The next time playEffectEx is executed, it will try to reopen the file and play it from the beginning.
     *
     * @param connection One `RtcConnection` object. See `RtcConnection`.
     * @param soundId The audio effect ID.
     * @param filePath The absolute path of the local file or the URL of the online file. Supported
     * audio formats include: mp3, mp4, m4a, aac, 3gp, mkv and wav.
     * @param startPos The playback position (ms) of the audio effect file.
     *
     * @return
     * - 0: Success.
     * - < 0: Failure.
     */
    virtual int preloadEffectEx(const RtcConnection& connection, int soundId, const char* filePath, int startPos = 0) = 0;

    /**
     * @brief Plays a specified sound effect in a channel.
     *
     * @since v4.6.0
     *
     * @details
     * You can call this method to play a specified sound effect to all users in the channel. Each call
     * to this method can only play one sound effect. To play multiple sound effects simultaneously,
     * please call this method multiple times. This method allows you to set whether to publish sound
     * effects in a channel. In order to play multiple sound files simultaneously, simply call the
     * method multiple times with different `soundId` and `filePath` parameters.
     * Applicable scenarios: This method can be called in scenarios involving multiple channels.
     *
     * @note
     * - Agora recommends not playing more than three sound effects at the same time.
     * - The sound effect ID and file path in this method must be consistent with those in the
     * `preloadEffectEx` method.
     * - If preloadEffectEx is called before playEffectEx is executed, the file resource will not be closed after playEffectEx. 
     * The next time playEffectEx is executed, it will directly seek to play at the beginning.
     * - If preloadEffectEx is not called before playEffectEx is executed, the resource will be destroyed after playEffectEx. 
     * The next time playEffectEx is executed, it will try to reopen the file and play it from the beginning.
     *
     * @param connection One `RtcConnection` object. See `RtcConnection`.
     * @param soundId The audio effect ID.
     * @param filePath The absolute path of the local file or the URL of the online file. Supported
     * audio formats: mp3, mp4, m4a, aac, 3gp, mkv and wav.
     * @param loopCount Number of times the sound effect to be looped:
     * - `-1`: Loop infinitely until calling `stopEffect` or `stopAllEffects`.
     * - `0`: Play once.
     * - `1`: Play twice.
     * @param pitch The pitch of the audio effect. The range is from 0.5 to 2.0, with a default value of
     * 1.0 (original pitch). The lower the value, the lower the pitch.
     * @param pan The spatial position of the audio effect. The range of values is from -1.0 to 1.0:
     * - `-1.0`: The audio effect is heard on the left of the user.
     * - `0.0`: The audio effect is heard in front of the user.
     * - `1.0`: The audio effect is heard on the right of the user.
     * @param gain The volume of the audio effect. The value range is from 0 to 100, with a default
     * value of 100 (original volume). The smaller the value, the lower the volume.
     * @param publish Whether to publish the audio effect in the channel:
     * - `true`: Publish the audio effect in the channel.
     * - `false`: (Default) Do not publish the audio effect in the channel.
     * @param startPos The playback position (ms) of the audio effect file.
     *
     * @return
     * - 0: Success.
     * - < 0: Failure.
     */
    virtual int playEffectEx(const RtcConnection& connection, int soundId, const char* filePath, int loopCount, double pitch, double pan, int gain, bool publish = false, int startPos = 0) = 0;
};

}  // namespace rtc
}  // namespace agora
