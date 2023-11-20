//
//  Agora Rtc Engine SDK
//
//  Copyright (c) 2018 Agora.io. All rights reserved.
//
#pragma once

#include "AgoraBase.h"
#include "IAgoraRtcEngineBase.h"
#include "AgoraMediaBase.h"
#include "IAgoraLog.h"
#include "AgoraOptional.h"
#include "IAudioDeviceManager.h"
#include "IAgoraRhythmPlayer.h"
#include "IAgoraMediaEngine.h"
#include "IAgoraH265Transcoder.h"

namespace agora {

namespace rtm {
class IStreamChannel;
}

namespace rtc {

struct RtcConnection;

/** Statistics of the local video stream.
 */
struct LocalVideoStats : public LocalVideoStatsBase {
  /**
   * ID of the local user.
   */
  uid_t uid;
};

/**
 * Audio statistics of the remote user.
 */
struct RemoteAudioStats : public RemoteAudioStatsBase {
  /**
   * User ID of the remote user sending the audio stream.
   */
  uid_t uid;

  RemoteAudioStats(): uid(0) {} 
};

/**
 * The statistics of the remote video stream.
 */
struct RemoteVideoStats : public RemoteVideoStatsBase {
  /**
   * ID of the remote user sending the video stream.
   */
  uid_t uid;
  /**
   * @deprecated Time delay (ms).
   *
   * In scenarios where audio and video is synchronized, you can use the
   * value of `networkTransportDelay` and `jitterBufferDelay` in `RemoteAudioStats`
   * to know the delay statistics of the remote video.
   */
  int delay __deprecated;
};

struct VideoCompositingLayout : public VideoCompositingLayoutBase {
  struct Region : public RegionBase {
    /** User ID of the user whose video is to be displayed in the region.
     */
    uid_t uid;    

    Region() : uid(0) {}
  };

  /** Region array. Each host in the channel can have a region to display the
   * video on the screen.
   */
  const Region* regions;
  /** Number of users in the channel.
   */
  int regionCount;

  VideoCompositingLayout()
      : regions(OPTIONAL_NULLPTR),
        regionCount(0) {}
};

/**
 * The IRtcEngineEventHandler class.
 *
 * The SDK uses this class to send callback event notifications to the app, and the app inherits
 * the methods in this class to retrieve these event notifications.
 *
 * All methods in this class have their default (empty)  implementations, and the app can inherit
 * only some of the required events instead of all. In the callback methods, the app should avoid
 * time-consuming tasks or calling blocking APIs, otherwise the SDK may not work properly.
 */
class IRtcEngineEventHandler : virtual public IRtcEngineEventHandlerBase {
 public:
  virtual ~IRtcEngineEventHandler() {}

  virtual const char* eventHandlerType() const { return "event_handler"; }

  /**
   * Occurs when a user joins a channel.
   *
   * This callback notifies the application that a user joins a specified channel.
   *
   * @param channel The channel name.
   * @param uid The ID of the user who joins the channel.
   * @param elapsed The time elapsed (ms) from the local user calling joinChannel until the SDK triggers this callback.
   */
  virtual void onJoinChannelSuccess(const char* channel, uid_t uid, int elapsed) {
    (void)channel;
    (void)uid;
    (void)elapsed;
  }

  /**
   * Occurs when a user rejoins the channel.
   *
   * When a user loses connection with the server because of network problems, the SDK automatically tries to reconnect
   * and triggers this callback upon reconnection.
   *
   * @param channel The channel name.
   * @param uid The ID of the user who rejoins the channel.
   * @param elapsed Time elapsed (ms) from the local user calling the joinChannel method until this callback is triggered.
   */
  virtual void onRejoinChannelSuccess(const char* channel, uid_t uid, int elapsed) {
    (void)channel;
    (void)uid;
    (void)elapsed;
  }

  /** Occurs when join success after calling \ref IRtcEngine::setLocalAccessPoint "setLocalAccessPoint" or \ref IRtcEngine::setCloudProxy "setCloudProxy"
  @param channel Channel name.
  @param uid  User ID of the user joining the channel.
  @param proxyType type of proxy agora sdk connected, proxyType will be NONE_PROXY_TYPE if not connected to proxy(fallback).
  @param localProxyIp local proxy ip. if not join local proxy, it will be "".
  @param elapsed Time elapsed (ms) from the user calling the \ref IRtcEngine::joinChannel "joinChannel" method until the SDK triggers this callback.
   */
  virtual void onProxyConnected(const char* channel, uid_t uid, PROXY_TYPE proxyType, const char* localProxyIp, int elapsed) {
    (void)channel;
    (void)uid;
    (void)proxyType;
    (void)localProxyIp;
    (void)elapsed;
  }

  /** Reports the statistics of the audio stream from each remote
  user/broadcaster.

  @deprecated This callback is deprecated. Use onRemoteAudioStats instead.

  The SDK triggers this callback once every two seconds to report the audio
  quality of each remote user/host sending an audio stream. If a channel has
  multiple remote users/hosts sending audio streams, the SDK triggers this
  callback as many times.

  @param uid The user ID of the remote user sending the audio stream.
  @param quality The audio quality of the user: #QUALITY_TYPE
  @param delay The network delay (ms) from the sender to the receiver, including the delay caused by audio sampling pre-processing, network transmission, and network jitter buffering.
  @param lost The audio packet loss rate (%) from the sender to the receiver.
  */
  virtual void onAudioQuality(uid_t uid, int quality, unsigned short delay, unsigned short lost) __deprecated {
    (void)uid;
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
  virtual void onAudioVolumeIndication(const AudioVolumeInfo* speakers, unsigned int speakerNumber,
                                       int totalVolume) {
    (void)speakers;
    (void)speakerNumber;
    (void)totalVolume;
  }

  /** Occurs when the audio mixing file playback finishes.
   @deprecated This method is deprecated, use onAudioMixingStateChanged instead.

   After you call startAudioMixing to play a local music file, this callback occurs when the playback finishes.
   If the startAudioMixing method call fails, the SDK returns the error code 701.
   */
  virtual void onAudioMixingFinished() __deprecated {}

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
   * @param uid The user ID. The network quality of the user with this user ID is reported.
   * @param txQuality Uplink network quality rating of the user in terms of the transmission bit rate, packet loss rate,
   * average RTT (Round-Trip Time) and jitter of the uplink network. This parameter is a quality rating helping you
   * understand how well the current uplink network conditions can support the selected video encoder configuration.
   * For example, a 1000 Kbps uplink network may be adequate for video frames with a resolution of 640 × 480 and a frame
   * rate of 15 fps in the LIVE_BROADCASTING profile, but may be inadequate for resolutions higher than 1280 × 720.
   * See #QUALITY_TYPE.
   * @param rxQuality Downlink network quality rating of the user in terms of packet loss rate, average RTT, and jitter
   * of the downlink network. See #QUALITY_TYPE.
   */
  virtual void onNetworkQuality(uid_t uid, int txQuality, int rxQuality) {
    (void)uid;
    (void)txQuality;
    (void)rxQuality;
  }

  /** Occurs when the first remote video frame is received and decoded.

  The SDK triggers this callback under one of the following circumstances:
  - The remote user joins the channel and sends the video stream.
  - The remote user stops sending the video stream and re-sends it after 15 seconds. Reasons for such an interruption include:
   - The remote user leaves the channel.
   - The remote user drops offline.
   - The remote user calls `muteLocalVideoStream` to stop sending the video stream.
   - The remote user calls `disableVideo` to disable video.

  @param uid The user ID of the remote user sending the video stream.
  @param width The width (pixels) of the video stream.
  @param height The height (pixels) of the video stream.
  @param elapsed The time elapsed (ms) from the local user calling `joinChannel`
  until the SDK triggers this callback.
  */
  virtual void onFirstRemoteVideoDecoded(uid_t uid, int width, int height, int elapsed) __deprecated {
    (void)uid;
    (void)width;
    (void)height;
    (void)elapsed;
  }

  /**
   * Occurs when the remote video state changes.
   *
   * @note This callback does not work properly when the number of users (in the voice/video call
   * channel) or hosts (in the live streaming channel) in the channel exceeds 17.
   *
   * @param uid The ID of the user whose video state has changed.
   * @param state The remote video state: #REMOTE_VIDEO_STATE.
   * @param reason The reason of the remote video state change: #REMOTE_VIDEO_STATE_REASON.
   * @param elapsed The time elapsed (ms) from the local client calling `joinChannel` until this callback is triggered.
   */
  virtual void onRemoteVideoStateChanged(uid_t uid, REMOTE_VIDEO_STATE state, REMOTE_VIDEO_STATE_REASON reason, int elapsed) {
    (void)uid;
    (void)state;
    (void)reason;
    (void)elapsed;
  }

    /**
   * Occurs when the local or remote video size or rotation has changed.
   * @param sourceType The video source type: #VIDEO_SOURCE_TYPE.
   * @param uid The user ID. 0 indicates the local user.
   * @param width The new width (pixels) of the video.
   * @param height The new height (pixels) of the video.
   * @param rotation The rotation information of the video.
   */
  virtual void onVideoSizeChanged(VIDEO_SOURCE_TYPE sourceType, uid_t uid, int width, int height, int rotation) {
    (void)uid;
    (void)width;
    (void)height;
    (void)rotation;
  }

  /** Occurs when the renderer receives the first frame of the remote video.
   *
   * @param uid The user ID of the remote user sending the video stream.
   * @param width The width (px) of the video frame.
   * @param height The height (px) of the video stream.
   * @param elapsed The time elapsed (ms) from the local user calling `joinChannel` until the SDK triggers this callback.
   */
  virtual void onFirstRemoteVideoFrame(uid_t uid, int width, int height, int elapsed) {
    (void)uid;
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
   * @param uid The ID of the remote user or broadcaster joining the channel.
   * @param elapsed The time elapsed (ms) from the local user calling `joinChannel` or `setClientRole`
   * until this callback is triggered.
  */
  virtual void onUserJoined(uid_t uid, int elapsed) {
    (void)uid;
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
   * @param uid The ID of the remote user or broadcaster who leaves the channel or drops offline.
   * @param reason The reason why the remote user goes offline: #USER_OFFLINE_REASON_TYPE.
   */
  virtual void onUserOffline(uid_t uid, USER_OFFLINE_REASON_TYPE reason) {
    (void)uid;
    (void)reason;
  }

  /** Occurs when a remote user's audio stream playback pauses/resumes.

     The SDK triggers this callback when the remote user stops or resumes sending the audio stream by
     calling the `muteLocalAudioStream` method.

     @note This callback can be inaccurate when the number of users (in the `COMMUNICATION` profile) or hosts (in the `LIVE_BROADCASTING` profile) in the channel exceeds 17.

     @param uid The user ID.
     @param muted Whether the remote user's audio stream is muted/unmuted:
     - true: Muted.
     - false: Unmuted.
     */
  virtual void onUserMuteAudio(uid_t uid, bool muted) {
    (void)uid;
    (void)muted;
  }

  /** Occurs when a remote user pauses or resumes sending the video stream.
   *
   * When a remote user calls `muteLocalVideoStream` to stop or resume publishing the video stream, the
   * SDK triggers this callback to report the state of the remote user's publishing stream to the local
   * user.

   @note This callback is invalid when the number of users or broadacasters in a
   channel exceeds 20.

   @param userId ID of the remote user.
   @param muted Whether the remote user stops publishing the video stream:
   - true: The remote user has paused sending the video stream.
   - false: The remote user has resumed sending the video stream.
   */
  virtual void onUserMuteVideo(uid_t uid, bool muted) {
    (void)uid;
    (void)muted;
  }

  /** Occurs when a remote user enables or disables the video module.

  Once the video function is disabled, the users cannot see any video.

  The SDK triggers this callback when a remote user enables or disables the video module by calling the
  `enableVideo` or `disableVideo` method.

  @param uid The ID of the remote user.
  @param enabled Whether the video of the remote user is enabled:
  - true: The remote user has enabled video.
  - false: The remote user has disabled video.
  */
  virtual void onUserEnableVideo(uid_t uid, bool enabled) {
    (void)uid;
    (void)enabled;
  }

  /**
   * Occurs when the remote user audio or video state is updated.
   * @param uid The uid of the remote user.
   * @param state The remote user's audio or video state: #REMOTE_USER_STATE.
   */
  virtual void onUserStateChanged(uid_t uid, REMOTE_USER_STATE state) {
    (void)uid;
    (void)state;
  }

  /** Occurs when a remote user enables or disables local video capturing.

  The SDK triggers this callback when the remote user resumes or stops capturing the video stream by
  calling the `enableLocalVideo` method.

  @param uid The ID of the remote user.
  @param enabled Whether the specified remote user enables/disables local video:
  - `true`: The remote user has enabled local video capturing.
  - `false`: The remote user has disabled local video capturing.
  */
  virtual void onUserEnableLocalVideo(uid_t uid, bool enabled) __deprecated {
    (void)uid;
    (void)enabled;
  }

  /** Reports the statistics of the audio stream from each remote user/host.

   The SDK triggers this callback once every two seconds for each remote user who is sending audio
   streams. If a channel includes multiple remote users, the SDK triggers this callback as many times.

   @param stats Statistics of the received remote audio streams. See RemoteAudioStats.
   */
  virtual void onRemoteAudioStats(const RemoteAudioStats& stats) {
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
   * @param source The video source type. See #VIDEO_SOURCE_TYPE.
   * @param stats Statistics of the local video stream. See LocalVideoStats.
   */
  virtual void onLocalVideoStats(VIDEO_SOURCE_TYPE source, const LocalVideoStats& stats) {
    (void)source;
    (void)stats;
  }

  /** Reports the statistics of the video stream from each remote user/host.
   *
   * The SDK triggers this callback once every two seconds for each remote user. If a channel has
   * multiple users/hosts sending video streams, the SDK triggers this callback as many times.
   *
   * @param stats Statistics of the remote video stream. See
   * RemoteVideoStats.
   */
  virtual void onRemoteVideoStats(const RemoteVideoStats& stats) {
    (void)stats;
  }

  /**
   * Occurs when the camera turns on and is ready to capture the video.
   * @deprecated Use `LOCAL_VIDEO_STREAM_STATE_CAPTURING(1)` in onLocalVideoStateChanged instead.
   * This callback indicates that the camera has been successfully turned on and you can start to capture video.
   */
  virtual void onCameraReady() __deprecated {}
  /**
   * Occurs when the video stops playing.
   * @deprecated Use `LOCAL_VIDEO_STREAM_STATE_STOPPED(0)` in the onLocalVideoStateChanged callback instead.
   *
   * The app can use this callback to change the configuration of the view (for example, displaying
   * other pictures in the view) after the video stops playing.
   */
  virtual void onVideoStopped() __deprecated {}

  /** Occurs when the connection between the SDK and the server is interrupted.
   * @deprecated Use `onConnectionStateChanged` instead.

  The SDK triggers this callback when it loses connection with the serer for more
  than 4 seconds after the connection is established. After triggering this
  callback, the SDK tries to reconnect to the server. If the reconnection fails
  within a certain period (10 seconds by default), the onConnectionLost()
  callback is triggered. If the SDK fails to rejoin the channel 20 minutes after
  being disconnected from Agora's edge server, the SDK stops rejoining the channel.

  */
  virtual void onConnectionInterrupted() __deprecated {}

  /** Occurs when your connection is banned by the Agora Server.
   * @deprecated Use `onConnectionStateChanged` instead.
   */
  virtual void onConnectionBanned() __deprecated {}

  /** Occurs when the local user receives the data stream from the remote user.
   *
   * The SDK triggers this callback when the user receives the data stream that another user sends
   * by calling the \ref agora::rtc::IRtcEngine::sendStreamMessage "sendStreamMessage" method.
   *
   * @param uid ID of the user who sends the data stream.
   * @param streamId The ID of the stream data.
   * @param data The data stream.
   * @param length The length (byte) of the data stream.
   * @param sentTs The time when the data stream sent.
   */
  virtual void onStreamMessage(uid_t uid, int streamId, const char* data, size_t length, uint64_t sentTs) {
    (void)uid;
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
   * @param uid ID of the user who sends the data stream.
   * @param streamId The ID of the stream data.
   * @param code The error code.
   * @param missed The number of lost messages.
   * @param cached The number of incoming cached messages when the data stream is
   * interrupted.
   */
  virtual void onStreamMessageError(uid_t uid, int streamId, int code, int missed, int cached) {
    (void)uid;
    (void)streamId;
    (void)code;
    (void)missed;
    (void)cached;
  }

  /** Occurs when the SDK receives the first audio frame from a specific remote user.
   * @deprecated Use `onRemoteAudioStateChanged` instead.
   *
   * @param uid ID of the remote user.
   * @param elapsed The time elapsed (ms) from the loca user calling `joinChannel`
   * until this callback is triggered.
   */
  virtual void onFirstRemoteAudioFrame(uid_t uid, int elapsed) __deprecated {
    (void)uid;
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
   * @param uid User ID of the remote user sending the audio stream.
   * @param elapsed The time elapsed (ms) from the loca user calling `joinChannel`
   * until this callback is triggered.
   */
  virtual void onFirstRemoteAudioDecoded(uid_t uid, int elapsed) __deprecated {
    (void)uid;
    (void)elapsed;
  }

  /** Occurs when the remote audio state changes.
   *
   * When the audio state of a remote user (in the voice/video call channel) or host (in the live streaming channel)
   * changes, the SDK triggers this callback to report the current state of the remote audio stream.
   *
   * @note This callback does not work properly when the number of users (in the voice/video call channel) or hosts
   * (in the live streaming channel) in the channel exceeds 17.
   *
   * @param uid ID of the remote user whose audio state changes.
   * @param state State of the remote audio. See #REMOTE_AUDIO_STATE.
   * @param reason The reason of the remote audio state change.
   * See #REMOTE_AUDIO_STATE_REASON.
   * @param elapsed Time elapsed (ms) from the local user calling the
   * `joinChannel` method until the SDK
   * triggers this callback.
   */
  virtual void onRemoteAudioStateChanged(uid_t uid, REMOTE_AUDIO_STATE state, REMOTE_AUDIO_STATE_REASON reason, int elapsed) {
    (void)uid;
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
   * @param userId The ID of the active speaker. A `uid` of 0 means the local user.
   */
  virtual void onActiveSpeaker(uid_t uid) { 
    (void)uid;
  }

  /** Reports the result of taking a video snapshot.
   *
   * After a successful `takeSnapshot` method call, the SDK triggers this callback to report whether the snapshot is
   * successfully taken, as well as the details for that snapshot.
   *
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
  virtual void onSnapshotTaken(uid_t uid, const char* filePath, int width, int height, int errCode) {
    (void)uid;
    (void)filePath;
    (void)width;
    (void)height;
    (void)errCode;
  }

  /**
   * Occurs when the remote media stream falls back to audio-only stream due to poor network conditions or
   * switches back to video stream after the network conditions improve.
   *
   * If you call `setRemoteSubscribeFallbackOption` and set `option` as `STREAM_FALLBACK_OPTION_AUDIO_ONLY(2)`, this
   * callback is triggered when the remotely subscribed media stream falls back to audio-only mode due to poor downlink
   * conditions, or when the remotely subscribed media stream switches back to the video after the downlink network
   * condition improves.
   *
   * @note Once the remote media stream is switched to the low stream due to poor network conditions, you can monitor
   * the stream switch between a high and low stream in the `onRemoteVideoStats` callback.
   *
   * @param uid ID of the remote user sending the stream.
   * @param isFallbackOrRecover Whether the remote media stream fell back to audio-only or switched back to the video:
   * - `true`: The remote media stream fell back to audio-only due to poor network conditions.
   * - `false`: The remote media stream switched back to the video stream after the network conditions improved.
   */
  virtual void onRemoteSubscribeFallbackToAudioOnly(uid_t uid, bool isFallbackOrRecover) {
    (void)uid;
    (void)isFallbackOrRecover;
  }

  /** Reports the transport-layer statistics of each remote audio stream.
   * @deprecated Use `onRemoteAudioStats` instead.

  This callback reports the transport-layer statistics, such as the packet loss rate and network time delay, once every
  two seconds after the local user receives an audio packet from a remote user. During a call, when the user receives
  the audio packet sent by the remote user/host, the callback is triggered every 2 seconds.

  @param uid ID of the remote user whose audio data packet is received.
  @param delay The network time delay (ms) from the sender to the receiver.
  @param lost The Packet loss rate (%) of the audio packet sent from the remote
  user.
  @param rxKBitRate Received bitrate (Kbps) of the audio packet sent from the
  remote user.
  */
  virtual void onRemoteAudioTransportStats(uid_t uid, unsigned short delay, unsigned short lost, unsigned short rxKBitRate) __deprecated {
    (void)uid;
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

  @param uid ID of the remote user whose video packet is received.
  @param delay The network time delay (ms) from the remote user sending the
  video packet to the local user.
  @param lost The packet loss rate (%) of the video packet sent from the remote
  user.
  @param rxKBitRate The bitrate (Kbps) of the video packet sent from
  the remote user.
  */
  virtual void onRemoteVideoTransportStats(uid_t uid, unsigned short delay, unsigned short lost, unsigned short rxKBitRate) __deprecated {
    (void)uid;
    (void)delay;
    (void)lost;
    (void)rxKBitRate;
  }

  /** Occurs when the local user registers a user account.
   *
   * After the local user successfully calls `registerLocalUserAccount` to register the user account
   * or calls `joinChannelWithUserAccount` to join a channel, the SDK triggers the callback and
   * informs the local user's UID and User Account.

    @param uid The ID of the local user.
    @param userAccount The user account of the local user.
   */
  virtual void onLocalUserRegistered(uid_t uid, const char* userAccount) {
    (void)uid;
    (void)userAccount;
  }

  /** Occurs when the SDK gets the user ID and user account of the remote user.

    After a remote user joins the channel, the SDK gets the UID and user account of the remote user,
    caches them in a mapping table object (`userInfo`), and triggers this callback on the local client.

    @param uid The ID of the remote user.
    @param info The `UserInfo` object that contains the user ID and user account of the remote user.
    */
  virtual void onUserInfoUpdated(uid_t uid, const UserInfo& info) {
    (void)uid;
    (void)info;
  }

  /**
   * Occurs when the user account is updated.
   *
   * @param uid The user ID.
   * @param userAccount The user account.
   */
  virtual void onUserAccountUpdated(uid_t uid, const char* userAccount){
    (void)uid;
    (void)userAccount;
  }

  /**
   * Occurs when the audio subscribing state changes.
   *
   * @param channel The name of the channel.
   * @param uid The ID of the remote user.
   * @param oldState The previous subscribing status: #STREAM_SUBSCRIBE_STATE.
   * @param newState The current subscribing status: #STREAM_SUBSCRIBE_STATE.
   * @param elapseSinceLastState The time elapsed (ms) from the previous state to the current state.
   */
  virtual void onAudioSubscribeStateChanged(const char* channel, uid_t uid, STREAM_SUBSCRIBE_STATE oldState, STREAM_SUBSCRIBE_STATE newState, int elapseSinceLastState) {
    (void)channel;
    (void)uid;
    (void)oldState;
    (void)newState;
    (void)elapseSinceLastState;
  }

  /**
   * Occurs when the video subscribing state changes.
   *
   * @param channel The name of the channel.
   * @param uid The ID of the remote user.
   * @param oldState The previous subscribing status: #STREAM_SUBSCRIBE_STATE.
   * @param newState The current subscribing status: #STREAM_SUBSCRIBE_STATE.
   * @param elapseSinceLastState The time elapsed (ms) from the previous state to the current state.
   */
  virtual void onVideoSubscribeStateChanged(const char* channel, uid_t uid, STREAM_SUBSCRIBE_STATE oldState, STREAM_SUBSCRIBE_STATE newState, int elapseSinceLastState) {
    (void)channel;
    (void)uid;
    (void)oldState;
    (void)newState;
    (void)elapseSinceLastState;
  }

  /**
   * Reports the tracing result of video rendering event of the user.
   * 
   * @param uid The user ID.
   * @param currentEvent The current event of the tracing result: #MEDIA_TRACE_EVENT.
   * @param tracingInfo The tracing result: #VideoRenderingTracingInfo.
   */
  virtual void onVideoRenderingTracingResult(uid_t uid, MEDIA_TRACE_EVENT currentEvent, VideoRenderingTracingInfo tracingInfo) {
    (void)uid;
    (void)currentEvent;
    (void)tracingInfo;
  }

  /**
   * Occurs when local video transcoder stream has an error.
   *
   * @param stream Stream type of TranscodingVideoStream.
   * @param error Error code of VIDEO_TRANSCODER_ERROR.
   */
  virtual void onLocalVideoTranscoderError(const TranscodingVideoStream& stream, VIDEO_TRANSCODER_ERROR error){
    (void)stream;
    (void)error;
  }
  /**
   * Occurs when receive a video transcoder stream which has video layout info.
   *
   * @param uid user id of the transcoded stream.
   * @param width width of the transcoded stream.
   * @param height height of the transcoded stream.
   * @param layoutCount count of layout info in the transcoded stream.
   * @param layoutlist video layout info list of the transcoded stream.
   */
  virtual void onTranscodedStreamLayoutInfo(uid_t uid, int width, int height, int layoutCount,const VideoLayout* layoutlist) {
    (void)uid;
    (void)width;
    (void)height;
    (void)layoutCount;
    (void)layoutlist;
  }

};

/**
 * The context of IRtcEngine.
 */
struct RtcEngineContext : virtual public RtcEngineContextBase {
  /**
   * The event handler for IRtcEngine.
   */
  IRtcEngineEventHandler* eventHandler;
  
  RtcEngineContext() : eventHandler(NULL) {}
};

/** Definition of IMetadataObserver
*/
class IMetadataObserver : virtual public IMetadataObserverBase {
public:
    virtual ~IMetadataObserver() {}

    /** Metadata.
     */
    struct Metadata : public MetadataBase
    {
        /** The User ID that sent the metadata.
         * - For the receiver: The user ID of the user who sent the `metadata`.
         * - For the sender: Ignore this value.
         */
        unsigned int uid;

        Metadata() : uid(0) {}
    };

    /** Occurs when the local user receives the metadata.

     @note Ensure that the size of the metadata does not exceed the value set in the `getMaxMetadataSize` callback.

     @param metadata The metadata that the user wants to send. For details, see Metadata.
     @param source_type The video data type: #VIDEO_SOURCE_TYPE.
     @return
     - true:  Send.
     - false: Do not send.
     */
    virtual bool onReadyToSendMetadata(Metadata &metadata, VIDEO_SOURCE_TYPE source_type) = 0;

    /** Occurs when the local user receives the metadata.
     *
     * @param metadata The metadata received. See Metadata.
     *
     * @note If the receiver is audience, the receiver cannot get the NTP timestamp (ms)
     * that the metadata sends.
     */
    virtual void onMetadataReceived(const Metadata& metadata) = 0;
};

/**
 * The information for extension.
 */
struct ExtensionInfo : public ExtensionInfoBase {
  /**
   * The id of the remote user on which the extension works.
   *
   * @note remoteUid = 0 means that the extension works on all remote streams.
   */
  uid_t remoteUid;

  /**
   * User ID: A 32-bit unsigned integer ranging from 1 to (2^32-1). It must be unique.
   */
  uid_t localUid;

  ExtensionInfo() : remoteUid(0), localUid(0) {}
};

class IMediaPlayer;
class IMediaRecorder;

/**
 * The IRtcEngine class, which is the basic interface of the Agora SDK that implements the core functions of real-time communication.
 *
 * `IRtcEngine` provides the main methods that your app can call.
 *
 * Before calling other APIs, you must call createAgoraRtcEngine to create an `IRtcEngine` object.
 */
class IRtcEngine : virtual public IRtcEngineBase {
 public:
  using IRtcEngineBase::setAudioProfile;
  using IRtcEngineBase::addVideoWatermark;
  using IRtcEngineBase::enableExtension;
  using IRtcEngineBase::setExtensionProperty;
  using IRtcEngineBase::getExtensionProperty;

  /**
   * Initializes `IRtcEngine`.
   *
   * All called methods provided by the `IRtcEngine` class are executed asynchronously. Agora
   * recommends calling these methods in the same thread.
   *
   * @note
   * - Before calling other APIs, you must call `createAgoraRtcEngine` and `initialize `to create and
   * initialize the `IRtcEngine` object.
   * - The SDK supports creating only one `IRtcEngine` instance for an app.
   *
   * @param context The RtcEngineContext object.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int initialize(const RtcEngineContext& context) = 0;

  /**
   * Preload a channel.
   *
   * This method enables users to preload a channel.
   *
   * A successful call of this method will reduce the time of joining the same channel.
   *
   * Note:
   *  1. The SDK supports preloading up to 20 channels. Once the preloaded channels exceed the limit, the SDK will keep the latest 20 available.
   *  2. Renew the token of the preloaded channel by calling this method with the same 'channelId' and 'uid'.
   *
   * @param token The token generated on your server for authentication.
   * @param channelId The channel name. This parameter signifies the channel in which users engage in
   * real-time audio and video interaction. Under the premise of the same App ID, users who fill in
   * the same channel ID enter the same channel for audio and video interaction. The string length
   * must be less than 64 bytes. Supported character scopes are:
   * - All lowercase English letters: a to z.
   * - All uppercase English letters: A to Z.
   * - All numeric characters: 0 to 9.
   * - The space character.
   * - Punctuation characters and other symbols, including: "!", "#", "$", "%", "&", "(", ")", "+", "-",
   * ":", ";", "<", "=", ".", ">", "?", "@", "[", "]", "^", "_", " {", "}", "|", "~", ",".
   * @param uid The user ID. This parameter is used to identify the user in the channel for real-time
   * audio and video interaction. You need to set and manage user IDs yourself, and ensure that each
   * user ID in the same channel is unique. This parameter is a 32-bit unsigned integer. The value
   * range is 1 to 2<h>32</h>-1. If the user ID is not assigned (or set to 0), the SDK assigns a random user
   * ID and returns it in the onJoinChannelSuccess callback. Your application must record and maintain
   * the returned user ID, because the SDK does not do so.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   *   - -7: The IRtcEngine object has not been initialized. You need to initialize the IRtcEngine
   * object before calling this method.
   *   - -102: The channel name is invalid. You need to pass in a valid channel name in channelId to
   * preload the channel again.
   */
  virtual int preloadChannel(const char* token, const char* channelId, uid_t uid) = 0;

  /**
   * Preload a channel.
   *
   * This method enables users to preload a channel.
   *
   * A successful call of this method will reduce the time of joining the same channel.
   *
   * Note:
   *  1. The SDK supports preloading up to 20 channels. Once the preloaded channels exceed the limit, the SDK will keep the latest 20 available.
   *  2. Renew the token of the preloaded channel by calling this method with the same 'channelId' and 'userAccount'.
   *
   * @param token The token generated on your server for authentication.
   * @param channelId The channel name. This parameter signifies the channel in which users engage in
   * real-time audio and video interaction. Under the premise of the same App ID, users who fill in
   * the same channel ID enter the same channel for audio and video interaction. The string length
   * must be less than 64 bytes. Supported character scopes are:
   * - All lowercase English letters: a to z.
   * - All uppercase English letters: A to Z.
   * - All numeric characters: 0 to 9.
   * - The space character.
   * - Punctuation characters and other symbols, including: "!", "#", "$", "%", "&", "(", ")", "+", "-",
   * ":", ";", "<", "=", ".", ">", "?", "@", "[", "]", "^", "_", " {", "}", "|", "~", ",".
   * @param userAccount The user account. The maximum length of this parameter is 255 bytes. Ensure that you set this parameter and do not set it as null. Supported character scopes are:
   * - All lowercase English letters: a to z.
   * - All uppercase English letters: A to Z.
   * - All numeric characters: 0 to 9.
   * - The space character.
   * - Punctuation characters and other symbols, including: "!", "#", "$", "%", "&", "(", ")", "+", "-", ":", ";", "<", "=", ".", ">", "?", "@", "[", "]", "^", "_", " {", "}", "|", "~", ",".
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   *   - -2: The parameter is invalid. For example, the userAccount parameter is empty.
   * You need to pass in a valid parameter and preload the channel again.
   *   - -7: The IRtcEngine object has not been initialized. You need to initialize the IRtcEngine
   * object before calling this method.
   *   - -102: The channel name is invalid. You need to pass in a valid channel name in channelId to
   * preload the channel again.
   */
  virtual int preloadChannelWithUserAccount(const char* token, const char* channelId, const char* userAccount) = 0;

  /**
   * Joins a channel.
   *
   * This method enables users to join a channel. Users in the same channel can talk to each other,
   * and multiple users in the same channel can start a group chat. Users with different App IDs
   * cannot call each other.
   *
   * A successful call of this method triggers the following callbacks:
   * - The local client: The `onJoinChannelSuccess` and `onConnectionStateChanged` callbacks.
   * - The remote client: `onUserJoined`, if the user joining the channel is in the Communication
   * profile or is a host in the Live-broadcasting profile.
   *
   * When the connection between the client and Agora's server is interrupted due to poor network
   * conditions, the SDK tries reconnecting to the server. When the local client successfully rejoins
   * the channel, the SDK triggers the `onRejoinChannelSuccess` callback on the local client.
   *
   * @note Once a user joins the channel, the user subscribes to the audio and video streams of all
   * the other users in the channel by default, giving rise to usage and billing calculation. To
   * stop subscribing to a specified stream or all remote streams, call the corresponding `mute` methods.
   *
   * @param token The token generated on your server for authentication.
   * @param channelId The channel name. This parameter signifies the channel in which users engage in
   * real-time audio and video interaction. Under the premise of the same App ID, users who fill in
   * the same channel ID enter the same channel for audio and video interaction. The string length
   * must be less than 64 bytes. Supported character scopes are:
   * - All lowercase English letters: a to z.
   * - All uppercase English letters: A to Z.
   * - All numeric characters: 0 to 9.
   * - The space character.
   * - Punctuation characters and other symbols, including: "!", "#", "$", "%", "&", "(", ")", "+", "-",
   * ":", ";", "<", "=", ".", ">", "?", "@", "[", "]", "^", "_", " {", "}", "|", "~", ",".
   * @param info (Optional) Reserved for future use.
   * @param uid The user ID. This parameter is used to identify the user in the channel for real-time
   * audio and video interaction. You need to set and manage user IDs yourself, and ensure that each
   * user ID in the same channel is unique. This parameter is a 32-bit unsigned integer. The value
   * range is 1 to 2<h>32</h>-1. If the user ID is not assigned (or set to 0), the SDK assigns a random user
   * ID and returns it in the onJoinChannelSuccess callback. Your application must record and maintain
   * the returned user ID, because the SDK does not do so.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   *   - -2: The parameter is invalid. For example, the token is invalid, the uid parameter is not set
   * to an integer, or the value of a member in the `ChannelMediaOptions` structure is invalid. You need
   * to pass in a valid parameter and join the channel again.
   *   - -3: Failes to initialize the `IRtcEngine` object. You need to reinitialize the IRtcEngine object.
   *   - -7: The IRtcEngine object has not been initialized. You need to initialize the IRtcEngine
   * object before calling this method.
   *   - -8: The internal state of the IRtcEngine object is wrong. The typical cause is that you call
   * this method to join the channel without calling `stopEchoTest` to stop the test after calling
   * `startEchoTest` to start a call loop test. You need to call `stopEchoTest` before calling this method.
   *   - -17: The request to join the channel is rejected. The typical cause is that the user is in the
   * channel. Agora recommends using the `onConnectionStateChanged` callback to get whether the user is
   * in the channel. Do not call this method to join the channel unless you receive the
   * `CONNECTION_STATE_DISCONNECTED(1)` state.
   *   - -102: The channel name is invalid. You need to pass in a valid channel name in channelId to
   * rejoin the channel.
   *   - -121: The user ID is invalid. You need to pass in a valid user ID in uid to rejoin the channel.
   */
  virtual int joinChannel(const char* token, const char* channelId, const char* info, uid_t uid) = 0;

  /**
   * Joins a channel with media options.
   *
   * This method enables users to join a channel. Users in the same channel can talk to each other,
   * and multiple users in the same channel can start a group chat. Users with different App IDs
   * cannot call each other.
   *
   * A successful call of this method triggers the following callbacks:
   * - The local client: The `onJoinChannelSuccess` and `onConnectionStateChanged` callbacks.
   * - The remote client: `onUserJoined`, if the user joining the channel is in the Communication
   * profile or is a host in the Live-broadcasting profile.
   *
   * When the connection between the client and Agora's server is interrupted due to poor network
   * conditions, the SDK tries reconnecting to the server. When the local client successfully rejoins
   * the channel, the SDK triggers the `onRejoinChannelSuccess` callback on the local client.
   *
   * Compared to `joinChannel`, this method adds the options parameter to configure whether to
   * automatically subscribe to all remote audio and video streams in the channel when the user
   * joins the channel. By default, the user subscribes to the audio and video streams of all
   * the other users in the channel, giving rise to usage and billings. To unsubscribe, set the
   * `options` parameter or call the `mute` methods accordingly.
   *
   * @note
   * - This method allows users to join only one channel at a time.
   * - Ensure that the app ID you use to generate the token is the same app ID that you pass in the
   * `initialize` method; otherwise, you may fail to join the channel by token.
   *
   * @param token The token generated on your server for authentication.
   *
   * @param channelId The channel name. This parameter signifies the channel in which users engage in
   * real-time audio and video interaction. Under the premise of the same App ID, users who fill in
   * the same channel ID enter the same channel for audio and video interaction. The string length
   * must be less than 64 bytes. Supported character scopes are:
   * - All lowercase English letters: a to z.
   * - All uppercase English letters: A to Z.
   * - All numeric characters: 0 to 9.
   * - The space character.
   * - Punctuation characters and other symbols, including: "!", "#", "$", "%", "&", "(", ")", "+", "-",
   * ":", ";", "<", "=", ".", ">", "?", "@", "[", "]", "^", "_", " {", "}", "|", "~", ",".
   * @param uid The user ID. This parameter is used to identify the user in the channel for real-time
   * audio and video interaction. You need to set and manage user IDs yourself, and ensure that each
   * user ID in the same channel is unique. This parameter is a 32-bit unsigned integer. The value
   * range is 1 to 2<h>32</h>-1. If the user ID is not assigned (or set to 0), the SDK assigns a random user
   * ID and returns it in the `onJoinChannelSuccess` callback. Your application must record and maintain
   * the returned user ID, because the SDK does not do so.
   * @param options The channel media options: ChannelMediaOptions.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   *   - -2: The parameter is invalid. For example, the token is invalid, the uid parameter is not set
   * to an integer, or the value of a member in the `ChannelMediaOptions` structure is invalid. You need
   * to pass in a valid parameter and join the channel again.
   *   - -3: Failes to initialize the `IRtcEngine` object. You need to reinitialize the IRtcEngine object.
   *   - -7: The IRtcEngine object has not been initialized. You need to initialize the IRtcEngine
   * object before calling this method.
   *   - -8: The internal state of the IRtcEngine object is wrong. The typical cause is that you call
   * this method to join the channel without calling `stopEchoTest` to stop the test after calling
   * `startEchoTest` to start a call loop test. You need to call `stopEchoTest` before calling this method.
   *   - -17: The request to join the channel is rejected. The typical cause is that the user is in the
   * channel. Agora recommends using the `onConnectionStateChanged` callback to get whether the user is
   * in the channel. Do not call this method to join the channel unless you receive the
   * `CONNECTION_STATE_DISCONNECTED(1)` state.
   *   - -102: The channel name is invalid. You need to pass in a valid channel name in channelId to
   * rejoin the channel.
   *   - -121: The user ID is invalid. You need to pass in a valid user ID in uid to rejoin the channel.
   */
  virtual int joinChannel(const char* token, const char* channelId, uid_t uid, const ChannelMediaOptions& options) = 0;

  /**
   * Initializes the video view of a remote user.
   *
   * This method initializes the video view of a remote stream on the local device. It affects only the
   * video view that the local user sees.
   *
   * Usually the app should specify the `uid` of the remote video in the method call before the
   * remote user joins the channel. If the remote `uid` is unknown to the app, set it later when the
   * app receives the \ref IRtcEngineEventHandler::onUserJoined "onUserJoined" callback.
   *
   * To unbind the remote user from the view, set `view` in VideoCanvas as `null`.
   *
   * @note
   * Ensure that you call this method in the UI thread.
   *
   * @param canvas The remote video view settings: VideoCanvas.
   * @return int
   *  VIRTUAL_BACKGROUND_SOURCE_STATE_REASON_SUCCESS = 0,
   *  VIRTUAL_BACKGROUND_SOURCE_STATE_REASON_IMAGE_NOT_EXIST = -1,
   *  VIRTUAL_BACKGROUND_SOURCE_STATE_REASON_COLOR_FORMAT_NOT_SUPPORTED = -2,
   *  VIRTUAL_BACKGROUND_SOURCE_STATE_REASON_DEVICE_NOT_SUPPORTED = -3,
   */
  virtual int setupRemoteVideo(const VideoCanvas& canvas) = 0;

  /**
   * Initializes the local video view.
   *
   * This method initializes the video view of the local stream on the local device. It affects only
   * the video view that the local user sees, not the published local video stream.
   *
   * To unbind the local video from the view, set `view` in VideoCanvas as `null`.
   *
   * @note
   * Call this method before joining a channel.
   *
   * @param canvas The local video view setting: VideoCanvas.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setupLocalVideo(const VideoCanvas& canvas) = 0;

  /**
   * Sets the audio parameters and application scenarios.
   *
   * @deprecated This method is deprecated. You can use the
   * \ref IRtcEngine::setAudioProfile(AUDIO_PROFILE_TYPE) "setAudioProfile"
   * method instead. To set the audio scenario, call the \ref IRtcEngine::initialize "initialize"
   * method and pass value in the `audioScenario` member in the RtcEngineContext struct.
   *
   * @note
   * - Call this method before calling the `joinChannel` method.
   * - In scenarios requiring high-quality audio, we recommend setting `profile` as `MUSIC_HIGH_QUALITY`(4)
   * and `scenario` as `AUDIO_SCENARIO_GAME_STREAMING`(3).
   *
   * @param profile Sets the sample rate, bitrate, encoding mode, and the number of channels:
   * #AUDIO_PROFILE_TYPE.
   * @param scenario Sets the audio application scenarios: #AUDIO_SCENARIO_TYPE.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setAudioProfile(AUDIO_PROFILE_TYPE profile, AUDIO_SCENARIO_TYPE scenario) __deprecated = 0;

  /**
   * Determines whether to receive all remote audio streams by default.
   *
   * @deprecated This method is deprecated. To set whether to receive remote
   * audio streams by default, call
   * \ref IRtcEngine::muteAllRemoteAudioStreams "muteAllRemoteAudioStreams"
   * before calling `joinChannel`
   *
   * Use this method to set whether to receive audio streams of subsequent peer
   * users. Agora recommends calling it before joining a channel.
   *
   * A successful call of setDefaultMuteAllRemoteAudioStreams(true) results in
   * that the local user not receiving any audio stream after joining a channel.
   * @param mute Whether to receive remote audio streams by default:
   * - true: Do not receive any remote audio stream by default.
   * - false: (Default) Receive remote audio streams by default.
   *
   * @return int
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setDefaultMuteAllRemoteAudioStreams(bool mute) __deprecated = 0;

  /**
   * Stops or resumes receiving the audio stream of a specified user.
   *
   * @note
   * You can call this method before or after joining a channel. If a user
   * leaves a channel, the settings in this method become invalid.
   *
   * @param uid The ID of the specified user.
   * @param mute Whether to stop receiving the audio stream of the specified user:
   * - true: Stop receiving the audio stream of the specified user.
   * - false: (Default) Resume receiving the audio stream of the specified user.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int muteRemoteAudioStream(uid_t uid, bool mute) = 0;

  /**
   Determines whether to receive all remote video streams by default.

   @deprecated This method is deprecated. To set whether to receive remote
   video streams by default, call
   \ref IRtcEngine::muteAllRemoteVideoStreams "muteAllRemoteVideoStreams"
   before calling `joinChannel`.

   Use this method to set whether to receive video streams of subsequent peer
   users. Agora recommends calling it before joining a channel.

   A successful call of setDefaultMuteAllRemoteVideoStreams(true) results in
   that the local user not receiving any video stream after joining a channel.

   @param mute Whether to receive remote video streams by default:
   - true: Do not receive any remote video stream by default.
   - false: (Default) Receive remote video streams by default.
   @return int
   - 0: Success.
   - < 0: Failure.
   */
  virtual int setDefaultMuteAllRemoteVideoStreams(bool mute) __deprecated = 0;

  /**
   * Stops or resumes receiving the video stream of a specified user.
   *
   * @note
   * You can call this method before or after joining a channel. If a user
   * leaves a channel, the settings in this method become invalid.
   *
   * @param uid The ID of the specified user.
   * @param mute Whether to stop receiving the video stream of the specified user:
   * - true: Stop receiving the video stream of the specified user.
   * - false: (Default) Resume receiving the video stream of the specified user.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int muteRemoteVideoStream(uid_t uid, bool mute) = 0;

  /**
   * Sets the remote video stream type.
   *
   * If the remote user has enabled the dual-stream mode, by default the SDK receives the high-stream video by
   * Call this method to switch to the low-stream video.
   *
   * @note
   * This method applies to scenarios where the remote user has enabled the dual-stream mode using
   * \ref enableDualStreamMode "enableDualStreamMode"(true) before joining the channel.
   *
   * @param uid ID of the remote user sending the video stream.
   * @param streamType Sets the video stream type: #VIDEO_STREAM_TYPE.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setRemoteVideoStreamType(uid_t uid, VIDEO_STREAM_TYPE streamType) = 0;

  /**
   * Sets the remote video subscription options
   *
   *
   * @param uid ID of the remote user sending the video stream.
   * @param options Sets the video subscription options.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setRemoteVideoSubscriptionOptions(uid_t uid, const VideoSubscriptionOptions &options) = 0;

  /**
   * Sets the blocklist of subscribe remote stream audio.
   *
   * @param uidList The id list of users whose audio you do not want to subscribe to.
   * @param uidNumber The number of uid in uidList.
   *
   * @note
   * If uid is in uidList, the remote user's audio will not be subscribed,
   * even if muteRemoteAudioStream(uid, false) and muteAllRemoteAudioStreams(false) are operated.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setSubscribeAudioBlocklist(uid_t* uidList, int uidNumber) = 0;

  /**
   * Sets the allowlist of subscribe remote stream audio.
   *
   * @param uidList The id list of users whose audio you want to subscribe to.
   * @param uidNumber The number of uid in uidList.
   *
   * @note
   * If uid is in uidList, the remote user's audio will be subscribed,
   * even if muteRemoteAudioStream(uid, true) and muteAllRemoteAudioStreams(true) are operated.
   *
   * If a user is in the blocklist and allowlist at the same time, only the blocklist takes effect.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setSubscribeAudioAllowlist(uid_t* uidList, int uidNumber) = 0;

  /**
   * Sets the blocklist of subscribe remote stream video.
   *
   * @param uidList The id list of users whose video you do not want to subscribe to.
   * @param uidNumber The number of uid in uidList.
   *
   * @note
   * If uid is in uidList, the remote user's video will not be subscribed,
   * even if muteRemoteVideoStream(uid, false) and muteAllRemoteVideoStreams(false) are operated.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setSubscribeVideoBlocklist(uid_t* uidList, int uidNumber) = 0;

  /**
   * Sets the allowlist of subscribe remote stream video.
   *
   * @param uidList The id list of users whose video you want to subscribe to.
   * @param uidNumber The number of uid in uidList.
   *
   * @note
   * If uid is in uidList, the remote user's video will be subscribed,
   * even if muteRemoteVideoStream(uid, true) and muteAllRemoteVideoStreams(true) are operated.
   *
   * If a user is in the blocklist and allowlist at the same time, only the blocklist takes effect.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setSubscribeVideoAllowlist(uid_t* uidList, int uidNumber) = 0;

  /**
   * Creates a media recorder object and return its pointer.
   *
   * @param info The RecorderStreamInfo object. It contains the user ID and the channel name.
   * 
   * @return
   * - The pointer to \ref rtc::IMediaRecorder "IMediaRecorder",
   *   if the method call succeeds.
   * - The empty pointer NULL, if the method call fails.
   */
  virtual agora_refptr<IMediaRecorder> createMediaRecorder(const RecorderStreamInfo& info) = 0;

  /**
   * Destroy a media recorder object.
   *
   * @param mediaRecorder The pointer to \ref rtc::IMediaRecorder.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int destroyMediaRecorder(agora_refptr<IMediaRecorder> mediaRecorder) = 0;

  /** Sets the sound position and gain of a remote user.

   When the local user calls this method to set the sound position of a remote user, the sound difference between the left and right channels allows the local user to track the real-time position of the remote user, creating a real sense of space. This method applies to massively multiplayer online games, such as Battle Royale games.

   @note
   - For this method to work, enable stereo panning for remote users by calling the \ref agora::rtc::IRtcEngine::enableSoundPositionIndication "enableSoundPositionIndication" method before joining a channel.
   - This method requires hardware support. For the best sound positioning, we recommend using a wired headset.
   - Ensure that you call this method after joining a channel.

   @param uid The ID of the remote user.
   @param pan The sound position of the remote user. The value ranges from -1.0 to 1.0:
   - 0.0: the remote sound comes from the front.
   - -1.0: the remote sound comes from the left.
   - 1.0: the remote sound comes from the right.
   @param gain Gain of the remote user. The value ranges from 0.0 to 100.0. The default value is 100.0 (the original gain of the remote user). The smaller the value, the less the gain.

   @return
   - 0: Success.
   - < 0: Failure.
   */
  virtual int setRemoteVoicePosition(uid_t uid, double pan, double gain) = 0;

  /** Sets remote user parameters for spatial audio

   @param uid The ID of the remote user.
   @param param spatial audio parameters: SpatialAudioParams.

   @return int
   - 0: Success.
   - < 0: Failure.
   */
  virtual int setRemoteUserSpatialAudioParams(uid_t uid, const agora::SpatialAudioParams& params) = 0;

  /**
   * Updates the display mode of the video view of a remote user.
   *
   * After initializing the video view of a remote user, you can call this method to update its
   * rendering and mirror modes. This method affects only the video view that the local user sees.
   *
   * @note
   * - Ensure that you have called \ref setupRemoteVideo "setupRemoteVideo" to initialize the remote video
   * view before calling this method.
   * - During a call, you can call this method as many times as necessary to update the display mode
   * of the video view of a remote user.
   *
   * @param uid ID of the remote user.
   * @param renderMode Sets the remote display mode. See #RENDER_MODE_TYPE.
   * @param mirrorMode Sets the mirror type. See #VIDEO_MIRROR_MODE_TYPE.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setRemoteRenderMode(uid_t uid, media::base::RENDER_MODE_TYPE renderMode,
                                  VIDEO_MIRROR_MODE_TYPE mirrorMode) = 0;

  /**
   * Registers an audio spectrum observer.
   *
   * You need to implement the `IAudioSpectrumObserver` class in this method, and register the following callbacks
   * according to your scenario:
   * - \ref agora::media::IAudioSpectrumObserver::onAudioSpectrumComputed "onAudioSpectrumComputed": Occurs when
   * the SDK receives the audio data and at set intervals.
   *
   * @param observer A pointer to the audio spectrum observer: \ref agora::media::IAudioSpectrumObserver
   * "IAudioSpectrumObserver".
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int registerAudioSpectrumObserver(agora::media::IAudioSpectrumObserver * observer) = 0;
  /**
   * Releases the audio spectrum observer.
   *
   * @param observer The pointer to the audio spectrum observer: \ref agora::media::IAudioSpectrumObserver
   * "IAudioSpectrumObserver".
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int unregisterAudioSpectrumObserver(agora::media::IAudioSpectrumObserver * observer) = 0;

  /*
   * Adjust the playback volume of the user specified by uid.
   *
   * You can call this method to adjust the playback volume of the user specified by uid
   * in call. If you want to adjust playback volume of the multi user, you can call this
   * this method multi times.
   *
   * @note
   * Please call this method after join channel.
   * This method adjust the playback volume of specified user.
   *
   * @param uid Remote user ID.
   * @param volume The playback volume of the specified remote user. The value ranges between 0 and 400, including the following:
   * 0: Mute.
   * 100: (Default) Original volume.
   * 400: Four times the original volume with signal-clipping protection.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int adjustUserPlaybackSignalVolume(uid_t uid, int volume) = 0;

  /** Sets the high priority user list and their fallback level in weak network condition.
   * @note
   * - This method can be called before and after joining a channel.
   * - If a subscriber is set to high priority, this stream only fallback to lower stream after all normal priority users fallback to their fallback level on weak network condition if needed.
   *
   * @param uidList The high priority user list.
   * @param uidNum The size of uidList.
   * @param option The fallback level of high priority users.
   *
   * @return int
   * - 0 : Success.
   * - <0 : Failure.
   */
  virtual int setHighPriorityUserList(uid_t* uidList, int uidNum, STREAM_FALLBACK_OPTIONS option) = 0;

  /**
   * Enable/Disable an extension.
   * By calling this function, you can dynamically enable/disable the extension without changing the pipeline.
   * For example, enabling/disabling Extension_A means the data will be adapted/bypassed by Extension_A.
   *
   * NOTE: For compatibility reasons, if you haven't call registerExtension,
   * enableExtension will automatically register the specified extension.
   * We suggest you call registerExtension explicitly.
   *
   * @param provider The name of the extension provider, e.g. agora.io.
   * @param extension The name of the extension, e.g. agora.beauty.
   * @param extensionInfo The information for extension.
   * @param enable Whether to enable the extension:
   * - true: (Default) Enable the extension.
   * - false: Disable the extension.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int enableExtension(const char* provider, const char* extension, const ExtensionInfo& extensionInfo, bool enable = true) = 0;

  /**
   * Sets the properties of an extension.
   *
   * @param provider The name of the extension provider, e.g. agora.io.
   * @param extension The name of the extension, e.g. agora.beauty.
   * @param extensionInfo The information for extension.
   * @param key The key of the extension.
   * @param value The JSON formatted value of the extension key.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setExtensionProperty(const char* provider, const char* extension, const ExtensionInfo& extensionInfo, const char* key, const char* value) = 0;

  /**
   * Gets the properties of an extension.
   *
   * @param provider The name of the extension provider, e.g. agora.io.
   * @param extension The name of the extension, e.g. agora.beauty.
   * @param extensionInfo The information for extension.
   * @param key The key of the extension.
   * @param value The value of the extension key.
   * @param buf_len Maximum length of the JSON string indicating the extension property.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int getExtensionProperty(const char* provider, const char* extension, const ExtensionInfo& extensionInfo, const char* key, char* value, int buf_len)  = 0;


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
    *   - #ERR_ALREADY_IN_USE (19): This streaming URL is already in use. Use a new streaming URL for CDN streaming.
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

  virtual int startLocalVideoTranscoder(const LocalTranscoderConfiguration& config) = 0;
  virtual int updateLocalTranscoderConfiguration(const LocalTranscoderConfiguration& config) = 0;

  // The following APIs are not implemented yet.
  virtual bool registerEventHandler(IRtcEngineEventHandler* eventHandler) = 0;
  virtual bool unregisterEventHandler(IRtcEngineEventHandler* eventHandler) = 0;
  virtual int setRemoteUserPriority(uid_t uid, PRIORITY_TYPE userPriority) = 0;

  /**
   * Sets the built-in encryption mode.
   *
   * @deprecated This method is deprecated. Use enableEncryption(bool enabled, const EncryptionConfig&) instead.
   *
   * The Agora Native SDK supports built-in encryption.
   * Call this API to set the encryption mode.
   *
   * All users in the same channel must use the same encryption mode and password.
   * Refer to information related to the encryption algorithm on the differences
   * between encryption modes.
   *
   * @note
   * Call \ref setEncryptionSecret "setEncryptionSecret" to enable the built-in encryption function
   * before calling this API.
   * @param encryptionMode Encryption mode:
   * - "sm4-128-ecb": 128-bit SM4 encryption, ECB mode.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setEncryptionMode(const char* encryptionMode) __deprecated = 0;

  /**
   * Enables built-in encryption.
   *
   * @deprecated This method is deprecated. Use enableEncryption(bool enabled, const EncryptionConfig&) instead.
   *
   * Use this method to specify an encryption password to enable built-in
   * encryption before joining a channel. All users in a channel must set the same
   * encryption password. The encryption password is automatically cleared once a
   * user has left the channel. If the encryption password is not specified or set to
   * empty, the encryption function will be disabled.
   *
   * @param secret The encryption password.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setEncryptionSecret(const char* secret) __deprecated = 0;

  /** **DEPRECATED** Adds a watermark image to the local video or CDN live stream.

   This method is not recommend, Use \ref agora::rtc::IRtcEngine::addVideoWatermark(const char* watermarkUrl, const WatermarkOptions& options) "addVideoWatermark"2 instead.

   This method adds a PNG watermark image to the local video stream for the recording device, channel audience, and CDN live audience to view and capture.

   To add the PNG file to the CDN live publishing stream, see the \ref IRtcEngine::setLiveTranscoding "setLiveTranscoding" method.

   @param watermark Pointer to the watermark image to be added to the local video stream. See RtcImage.

   @note
   - The URL descriptions are different for the local video and CDN live streams:
      - In a local video stream, `url` in RtcImage refers to the absolute path of the added watermark image file in the local video stream.
      - In a CDN live stream, `url` in RtcImage refers to the URL address of the added watermark image in the CDN live broadcast.
   - The source file of the watermark image must be in the PNG file format. If the width and height of the PNG file differ from your settings in this method, the PNG file will be cropped to conform to your settings.
   - The Agora SDK supports adding only one watermark image onto a local video or CDN live stream. The newly added watermark image replaces the previous one.

   @return
   - 0: Success.
   - < 0: Failure.
   */
  virtual int addVideoWatermark(const RtcImage& watermark) __deprecated = 0;


  // The following APIs are either deprecated and going to deleted.

  /** @deprecated Use disableAudio() instead.

   Disables the audio function in the channel.

   @return int
   - 0: Success.
   - < 0: Failure.
   */
   virtual int pauseAudio() __deprecated = 0;
   /** @deprecated Use enableAudio() instead.

   Resumes the audio function in the channel.

   @return int
   - 0: Success.
   - < 0: Failure.
   */
   virtual int resumeAudio() __deprecated = 0;

  /**
   * Enables interoperability with the Agora Web SDK (Live Broadcast only).
   *
   * @deprecated The Agora NG SDK enables the interoperablity with the Web SDK.
   *
   * Use this method when the channel profile is Live Broadcast. Interoperability
   * with the Agora Web SDK is enabled by default when the channel profile is
   * Communication.
   *
   * @param enabled Determines whether to enable interoperability with the Agora Web SDK.
   * - true: (Default) Enable interoperability with the Agora Native SDK.
   * - false: Disable interoperability with the Agora Native SDK.
   *
   * @return int
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int enableWebSdkInteroperability(bool enabled) __deprecated = 0;

  /** Registers the metadata observer.

   You need to implement the IMetadataObserver class and specify the metadata type
   in this method. This method enables you to add synchronized metadata in the video
   stream for more diversified live interactive streaming, such as sending
   shopping links, digital coupons, and online quizzes.

   A successful call of this method triggers
   the \ref agora::rtc::IMetadataObserver::getMaxMetadataSize "getMaxMetadataSize" callback.

   @note
   - Call this method before the `joinChannel` method.
   - This method applies to the `LIVE_BROADCASTING` channel profile.

   @param observer IMetadataObserver.
   @param type The metadata type. See \ref IMetadataObserver::METADATA_TYPE "METADATA_TYPE". The SDK supports VIDEO_METADATA (0) only for now.

   @return
   - 0: Success.
   - < 0: Failure.
  */
  virtual int registerMediaMetadataObserver(IMetadataObserver *observer, IMetadataObserver::METADATA_TYPE type) = 0;

  /** Unregisters the metadata observer.
   @param observer IMetadataObserver.
   @param type The metadata type. See \ref IMetadataObserver::METADATA_TYPE "METADATA_TYPE". The SDK supports VIDEO_METADATA (0) only for now.

   @return
   - 0: Success.
   - < 0: Failure.
  */
  virtual int unregisterMediaMetadataObserver(IMetadataObserver* observer, IMetadataObserver::METADATA_TYPE type) = 0;

  /** Start audio frame dump.

   Optional `location` is: "pre_apm_proc", "apm", "pre_send_proc", "filter", "enc", "tx_mixer",
                         "at_record", "atw_record" for audio sending.
                         "dec", "mixed", "play", "rx_mixer", "playback_mixer", "pcm_source_playback_mixer",
                         "pre_play_proc", "at_playout", "atw_playout" for audio receiving.

   */
  virtual int startAudioFrameDump(const char* channel_id, uid_t uid, const char* location, const char* uuid, const char* passwd, long duration_ms, bool auto_upload) = 0;

  /**
   * Stops the audio frame dump.
   */
  virtual int stopAudioFrameDump(const char* channel_id, uid_t uid, const char* location) = 0;

  /** Registers a user account.
   *
   * Once registered, the user account can be used to identify the local user when the user joins the channel.
   * After the user successfully registers a user account, the SDK triggers the \ref agora::rtc::IRtcEngineEventHandler::onLocalUserRegistered "onLocalUserRegistered" callback on the local client,
   * reporting the user ID and user account of the local user.
   *
   * To join a channel with a user account, you can choose either of the following:
   *
   * - Call the \ref agora::rtc::IRtcEngine::registerLocalUserAccount "registerLocalUserAccount" method to create a user account, and then the \ref agora::rtc::IRtcEngine::joinChannelWithUserAccount "joinChannelWithUserAccount" method to join the channel.
   * - Call the \ref agora::rtc::IRtcEngine::joinChannelWithUserAccount "joinChannelWithUserAccount" method to join the channel.
   *
   * The difference between the two is that for the former, the time elapsed between calling the \ref agora::rtc::IRtcEngine::joinChannelWithUserAccount "joinChannelWithUserAccount" method
   * and joining the channel is shorter than the latter.
   *
   * @note
   * - Ensure that you set the `userAccount` parameter. Otherwise, this method does not take effect.
   * - Ensure that the value of the `userAccount` parameter is unique in the channel.
   * - To ensure smooth communication, use the same parameter type to identify the user. For example, if a user joins the channel with a user ID, then ensure all the other users use the user ID too. The same applies to the user account. If a user joins the channel with the Agora Web SDK, ensure that the uid of the user is set to the same parameter type.
   *
   * @param appId The App ID of your project.
   * @param userAccount The user account. The maximum length of this parameter is 255 bytes. Ensure that you set this parameter and do not set it as null. Supported character scopes are:
   * - All lowercase English letters: a to z.
   * - All uppercase English letters: A to Z.
   * - All numeric characters: 0 to 9.
   * - The space character.
   * - Punctuation characters and other symbols, including: "!", "#", "$", "%", "&", "(", ")", "+", "-", ":", ";", "<", "=", ".", ">", "?", "@", "[", "]", "^", "_", " {", "}", "|", "~", ",".
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int registerLocalUserAccount(const char* appId, const char* userAccount) = 0;

  /** Joins the channel with a user account.
   *
   * After the user successfully joins the channel, the SDK triggers the following callbacks:
   *
   * - The local client: \ref agora::rtc::IRtcEngineEventHandler::onLocalUserRegistered "onLocalUserRegistered" and \ref agora::rtc::IRtcEngineEventHandler::onJoinChannelSuccess "onJoinChannelSuccess" .
   * - The remote client: \ref agora::rtc::IRtcEngineEventHandler::onUserJoined "onUserJoined" and \ref agora::rtc::IRtcEngineEventHandler::onUserInfoUpdated "onUserInfoUpdated" , if the user joining the channel is in the `COMMUNICATION` profile, or is a host in the `LIVE_BROADCASTING` profile.
   *
   * @note To ensure smooth communication, use the same parameter type to identify the user. For example, if a user joins the channel with a user ID, then ensure all the other users use the user ID too. The same applies to the user account.
   * If a user joins the channel with the Agora Web SDK, ensure that the uid of the user is set to the same parameter type.
   *
   * @param token The token generated at your server:
   * - For low-security requirements: You can use the temporary token generated at Console. For details, see [Get a temporary toke](https://docs.agora.io/en/Voice/token?platform=All%20Platforms#get-a-temporary-token).
   * - For high-security requirements: Set it as the token generated at your server. For details, see [Get a token](https://docs.agora.io/en/Voice/token?platform=All%20Platforms#get-a-token).
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
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int joinChannelWithUserAccount(const char* token, const char* channelId, const char* userAccount) = 0;

  /** Joins the channel with a user account.
   *
   * After the user successfully joins the channel, the SDK triggers the following callbacks:
   *
   * - The local client: \ref agora::rtc::IRtcEngineEventHandler::onLocalUserRegistered "onLocalUserRegistered" and \ref agora::rtc::IRtcEngineEventHandler::onJoinChannelSuccess "onJoinChannelSuccess" .
   * - The remote client: \ref agora::rtc::IRtcEngineEventHandler::onUserJoined "onUserJoined" and \ref agora::rtc::IRtcEngineEventHandler::onUserInfoUpdated "onUserInfoUpdated" , if the user joining the channel is in the `COMMUNICATION` profile, or is a host in the `LIVE_BROADCASTING` profile.
   *
   * @note To ensure smooth communication, use the same parameter type to identify the user. For example, if a user joins the channel with a user ID, then ensure all the other users use the user ID too. The same applies to the user account.
   * If a user joins the channel with the Agora Web SDK, ensure that the uid of the user is set to the same parameter type.
   *
   * @param token The token generated at your server:
   * - For low-security requirements: You can use the temporary token generated at Console. For details, see [Get a temporary toke](https://docs.agora.io/en/Voice/token?platform=All%20Platforms#get-a-temporary-token).
   * - For high-security requirements: Set it as the token generated at your server. For details, see [Get a token](https://docs.agora.io/en/Voice/token?platform=All%20Platforms#get-a-token).
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
   * @param options  The channel media options: \ref agora::rtc::ChannelMediaOptions::ChannelMediaOptions "ChannelMediaOptions"
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int joinChannelWithUserAccount(const char* token, const char* channelId, const char* userAccount, const ChannelMediaOptions& options) = 0;

  /** Joins the channel with a user account.
   *
   * After the user successfully joins the channel, the SDK triggers the following callbacks:
   *
   * - The local client: \ref agora::rtc::IRtcEngineEventHandler::onLocalUserRegistered "onLocalUserRegistered" and \ref agora::rtc::IRtcEngineEventHandler::onJoinChannelSuccess "onJoinChannelSuccess" .
   * - The remote client: \ref agora::rtc::IRtcEngineEventHandler::onUserJoined "onUserJoined" and \ref agora::rtc::IRtcEngineEventHandler::onUserInfoUpdated "onUserInfoUpdated" , if the user joining the channel is in the `COMMUNICATION` profile, or is a host in the `LIVE_BROADCASTING` profile.
   *
   * @note To ensure smooth communication, use the same parameter type to identify the user. For example, if a user joins the channel with a user ID, then ensure all the other users use the user ID too. The same applies to the user account.
   * If a user joins the channel with the Agora Web SDK, ensure that the uid of the user is set to the same parameter type.
   *
   * @param token The token generated at your server:
   * - For low-security requirements: You can use the temporary token generated at Console. For details, see [Get a temporary toke](https://docs.agora.io/en/Voice/token?platform=All%20Platforms#get-a-temporary-token).
   * - For high-security requirements: Set it as the token generated at your server. For details, see [Get a token](https://docs.agora.io/en/Voice/token?platform=All%20Platforms#get-a-token).
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
   * @param options  The channel media options: \ref agora::rtc::ChannelMediaOptions::ChannelMediaOptions "ChannelMediaOptions"
   * @param eventHandler The pointer to the IRtcEngine event handler: IRtcEngineEventHandler.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int joinChannelWithUserAccountEx(const char* token, const char* channelId,
                                           const char* userAccount, const ChannelMediaOptions& options,
                                           IRtcEngineEventHandler* eventHandler) = 0;

  /** Gets the user information by passing in the user account.
   *
   * After a remote user joins the channel, the SDK gets the user ID and user account of the remote user, caches them
   * in a mapping table object (`userInfo`), and triggers the \ref agora::rtc::IRtcEngineEventHandler::onUserInfoUpdated "onUserInfoUpdated" callback on the local client.
   *
   * After receiving the o\ref agora::rtc::IRtcEngineEventHandler::onUserInfoUpdated "onUserInfoUpdated" callback, you can call this method to get the user ID of the
   * remote user from the `userInfo` object by passing in the user account.
   *
   * @param userAccount The user account of the user. Ensure that you set this parameter.
   * @param [in,out] userInfo  A userInfo object that identifies the user:
   * - Input: A userInfo object.
   * - Output: A userInfo object that contains the user account and user ID of the user.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int getUserInfoByUserAccount(const char* userAccount, rtc::UserInfo* userInfo) = 0;

  /** Gets the user information by passing in the user ID.
   *
   * After a remote user joins the channel, the SDK gets the user ID and user account of the remote user,
   * caches them in a mapping table object (`userInfo`), and triggers the \ref agora::rtc::IRtcEngineEventHandler::onUserInfoUpdated "onUserInfoUpdated" callback on the local client.
   *
   * After receiving the \ref agora::rtc::IRtcEngineEventHandler::onUserInfoUpdated "onUserInfoUpdated" callback, you can call this method to get the user account of the remote user
   * from the `userInfo` object by passing in the user ID.
   *
   * @param uid The user ID of the remote user. Ensure that you set this parameter.
   * @param[in,out] userInfo A userInfo object that identifies the user:
   * - Input: A userInfo object.
   * - Output: A userInfo object that contains the user account and user ID of the user.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int getUserInfoByUid(uid_t uid, rtc::UserInfo* userInfo) = 0;

    /** Starts relaying media streams across channels or updates the channels for media relay.
     *
     * After a successful method call, the SDK triggers the
     * \ref agora::rtc::IRtcEngineEventHandler::onChannelMediaRelayStateChanged
     *  "onChannelMediaRelayStateChanged" callback, and this callback return the state of the media stream relay.
     * - If the
     * \ref agora::rtc::IRtcEngineEventHandler::onChannelMediaRelayStateChanged
     *  "onChannelMediaRelayStateChanged" callback returns
     * #RELAY_STATE_RUNNING (2) and #RELAY_OK (0), the host starts sending data to the destination channel.
     * - If the
     * \ref agora::rtc::IRtcEngineEventHandler::onChannelMediaRelayStateChanged
     *  "onChannelMediaRelayStateChanged" callback returns
     * #RELAY_STATE_FAILURE (3), an exception occurs during the media stream
     * relay.
     *
     * @note
     * - Call this method after the \ref joinChannel() "joinChannel" method.
     * - This method takes effect only when you are a host in a
     * `LIVE_BROADCASTING` channel.
     * - Contact sales-us@agora.io before implementing this function.
     * - We do not support string user accounts in this API.
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
     *   - -8(ERR_INVALID_STATE): The current status is invalid, only allowed to be called when the role is the broadcaster.
     */
  virtual int startOrUpdateChannelMediaRelay(const ChannelMediaRelayConfiguration &configuration) = 0;

  /**
   * Takes a snapshot of a video stream.
   *
   * This method takes a snapshot of a video stream from the specified user, generates a JPG
   * image, and saves it to the specified path.
   *
   * The method is asynchronous, and the SDK has not taken the snapshot when the method call
   * returns. After a successful method call, the SDK triggers the `onSnapshotTaken` callback
   * to report whether the snapshot is successfully taken, as well as the details for that
   * snapshot.
   *
   * @note
   * - Call this method after joining a channel.
   * - This method takes a snapshot of the published video stream specified in `ChannelMediaOptions`.
   * - If the user's video has been preprocessed, for example, watermarked or beautified, the resulting
   * snapshot includes the pre-processing effect.
   *
   * @param uid The user ID. Set uid as 0 if you want to take a snapshot of the local user's video.
   * @param filePath The local path (including filename extensions) of the snapshot. For example:
   * - Windows: `C:\Users\<user_name>\AppData\Local\Agora\<process_name>\example.jpg`
   * - iOS: `/App Sandbox/Library/Caches/example.jpg`
   * - macOS: `～/Library/Logs/example.jpg`
   * - Android: `/storage/emulated/0/Android/data/<package name>/files/example.jpg`
   *
   * Ensure that the path you specify exists and is writable.
   * @return
   * - 0 : Success.
   * - < 0 : Failure.
   */
  virtual int takeSnapshot(uid_t uid, const char* filePath)  = 0;

  /** Bind local user and a remote user as an audio&video sync group. The remote user is defined by cid and uid.
   *  There’s a usage limit that local user must be a video stream sender. On the receiver side, media streams from same sync group will be time-synced
   *
   * @param channelId The channel id
   * @param uid The user ID of the remote user to be bound with (local user)
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setAVSyncSource(const char* channelId, uid_t uid) = 0;

#if defined(_WIN32)
  /**
   * Shares the whole or part of a screen by specifying the screen rect.
   *
   * @deprecated This method is deprecated, use \ref IRtcEngine::startScreenCaptureByDisplayId "startScreenCaptureByDisplayId" instead. Agora strongly recommends using `startScreenCaptureByDisplayId` if you need to start screen sharing on a device connected to another display.
   *
   * @note This method applies to Windows only.
   *
   * @param screenRect Sets the relative location of the screen to the virtual
   * screen. For information on how to get screenRect, see the advanced guide:
   * Share the Screen.
   * @param regionRect (Optional) Sets the relative location of the region to the
   * screen. NULL means sharing the whole screen. See Rectangle.
   * If the specified region overruns the screen, the SDK shares only the region
   * within it; if you set width or height as 0, the SDK shares the whole screen.
   * @param captureParams Sets the screen sharing encoding parameters. See
   * ScreenCaptureParameters.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure:
   * - ERR_INVALID_ARGUMENT (2): The argument is invalid.
   * - ERR_NOT_INITIALIZED (7): You have not initialized IRtcEngine when try to start screen capture.
  */
  virtual int startScreenCaptureByScreenRect(const Rectangle& screenRect,
                                             const Rectangle& regionRect,
                                             const ScreenCaptureParameters& captureParams) __deprecated = 0;
#endif
};

class AAudioDeviceManager : public agora::util::AutoPtr<IAudioDeviceManager> {
 public:
  AAudioDeviceManager(IRtcEngine* engine) {
    queryInterface(engine, AGORA_IID_AUDIO_DEVICE_MANAGER);
  }
};

class AVideoDeviceManager : public agora::util::AutoPtr<IVideoDeviceManager> {
 public:
  AVideoDeviceManager(IRtcEngine* engine) {
    queryInterface(engine, AGORA_IID_VIDEO_DEVICE_MANAGER);
  }
};

}  // namespace rtc
}  // namespace agora

////////////////////////////////////////////////////////
/** \addtogroup createAgoraRtcEngine
 @{
 */
////////////////////////////////////////////////////////

/** Creates the RTC engine object and returns the pointer.

* @return Pointer of the RTC engine object.
*/
AGORA_API agora::rtc::IRtcEngine* AGORA_CALL createAgoraRtcEngine();
