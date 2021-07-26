//
//  Agora SDK
//
//  Created by Sting Feng in 2018-01.
//  Copyright (c) 2018 Agora.io. All rights reserved.
//

#pragma once  // NOLINT(build/header_guard)

#include "AgoraBase.h"

namespace agora {
namespace rtc {
class IAudioEncodedFrameSender;
class IRtcConnectionObserver;
class INetworkObserver;
class IRtcConnection;
class IVideoEncodedImageSender;
class ILocalUser;

/**
 * The information of an RTC Connection.
 */
struct TConnectionInfo {
  /**
   * The ID of the RTC Connection.
   */
  conn_id_t id;
  /**
   * The ID of the channel. If you have not called \ref agora::rtc::IRtcConnection::connect "connect", this member is `NULL`.
   */
  util::AString channelId;
  /**
   * The connection state: #CONNECTION_STATE_TYPE.
   */
  CONNECTION_STATE_TYPE state;
  /**
   * The ID of the local user.
   */
  util::AString localUserId;
  /**
   * Internal use only.
   */
  uid_t internalUid;

  TConnectionInfo() : id(-1), state(CONNECTION_STATE_DISCONNECTED), internalUid(0) {}
};

/**
 * The audio subscription options.
 */
struct AudioSubscriptionOptions {
  AudioSubscriptionOptions() :
    packetOnly(false),
    bytesPerSample(TWO_BYTES_PER_SAMPLE),
    numberOfChannels(0),
    sampleRateHz(0) {
  }

  AudioSubscriptionOptions(const AudioSubscriptionOptions &rhs) :
      packetOnly(rhs.packetOnly),
      bytesPerSample(rhs.bytesPerSample),
      numberOfChannels(rhs.numberOfChannels),
      sampleRateHz(rhs.sampleRateHz) {
  }
  /**
   * Whether to only subscribe to audio packets.
   * - `true`: Only subscribe to audio packets, which means that the SDK does not decode the remote audio stream. You can use this mode to receive audio packets and handle them in your app.
   * - `false`: (Default) The SDK automatically decodes the remote audio stream.
   *
   * @note If you set `packetOnly` as `true`, other fields in `AudioSubscriptionOptions` are ignored.
  */
  bool packetOnly;
  /**
   * The number of bytes that you expect for each audio sample.
   */
  size_t bytesPerSample;
  /**
   * The number of audio channels that you expect.
   */
  size_t numberOfChannels;
  /**
   * The audio sample rate (Hz) that you expect.
   */
  uint32_t sampleRateHz;
};

/**
 * Configurations for an RTC connection.
 *
 * Set these configurations when calling \ref agora::base::IAgoraService::createRtcConnection "createRtcConnection".
 */
struct RtcConnectionConfiguration {
  /**
   * Whether to subscribe to all audio tracks automatically.
   * - `true`: (Default) Subscribe to all audio tracks automatically.
   * - `false`: Do not subscribe to any audio track automatically.
   */
  bool autoSubscribeAudio;
  /**
   * Whether to subscribe to all video tracks automatically.
   * - `true`: (Default) Subscribe to all video tracks automatically.
   * - `false`: Do not subscribe to any video track automatically.
   */
  bool autoSubscribeVideo;
  /**
   * Whether to enable audio recording or playout.
   * - `true`: Enables audio recording or playout. Use this option when you publish and mix audio tracks, or subscribe to one or multiple audio tracks and play audio.
   * - `false`: Disables audio recording or playout. Use this option when you publish external audio frames without audio mixing, or you do not need audio devices to play audio.
   */
  bool enableAudioRecordingOrPlayout;
  /**
   * The maximum sending bitrate.
   */
  int maxSendBitrate;
  /**
   * The minimum port.
   */
  int minPort;
  /**
   * The maximum port.
   */
  int maxPort;
  /**
   * The options for audio subscription. See \ref agora::rtc::AudioSubscriptionOptions "AudioSubscriptionOptions".
   */
  AudioSubscriptionOptions audioSubscriptionOptions;
  /**
   * The user role. For details, see #CLIENT_ROLE_TYPE. The default user role is `CLIENT_ROLE_AUDIENCE`.
   */
  CLIENT_ROLE_TYPE clientRoleType;
  /** The channel profile. For details, see #CHANNEL_PROFILE_TYPE. The default channel profile is `CHANNEL_PROFILE_LIVE_BROADCASTING`.
   */
  CHANNEL_PROFILE_TYPE  channelProfile;

  /**
   * Determines whether to receive audio media packet or not.
   */
  bool audioRecvMediaPacket;

  /**
   * Determines whether to receive video media packet or not.
   */
  bool videoRecvMediaPacket;

  RtcConnectionConfiguration()
      : autoSubscribeAudio(true),
        autoSubscribeVideo(true),
        enableAudioRecordingOrPlayout(true),
        maxSendBitrate(-1),
        minPort(0),
        maxPort(0),
        clientRoleType(CLIENT_ROLE_AUDIENCE),
        channelProfile(CHANNEL_PROFILE_LIVE_BROADCASTING),
        audioRecvMediaPacket(false),
        videoRecvMediaPacket(false) {}
};

/**
 * The `IRtcConnection` class.
 *
 * You can use this class for managing the connection between your app and an Agora Channel.
 *
 * Once connected, your app gets an `AgoraLocalUser` object for publishing and subscribing to media streams in the Agora Channel.
 *
 * Connecting to a channel is done asynchronously, and your app can listen for the connection states or events through `IRtcConnectionObserver`.
 * `IRtcConnection` also monitors remote users in the channel. The SDK notifies your app when a remote user joins or leaves the channel.
 */
class IRtcConnection : public RefCountInterface {
 protected:
  ~IRtcConnection() {}

 public:
  /**
   * Connects to an Agora channel.
   *
   * When the method call succeeds, the connection state changes from `CONNECTION_STATE_DISCONNECTED(1)` to
   * `CONNECTION_STATE_CONNECTING(2)`.
   *
   * Depending on the whether the connection succeeds or not, the
   * connection state changes to either `CONNECTION_STATE_CONNECTED(3)` or `CONNECTION_STATE_FAILED(5)`. The SDK also triggers `onConnected` or `onDisconnected` to notify you of the state change.
   *
   * @param token The app ID.
   * @param channelId The channel name. It must be in the string format and not exceed 64 bytes in length. Supported character scopes are:
   * - All lowercase English letters: a to z.
   * - All uppercase English letters: A to Z.
   * - All numeric characters: 0 to 9.
   * - The space character.
   * - Punctuation characters and other symbols, including: "!", "#", "$", "%", "&", "(", ")", "+",
   * "-", ":", ";", "<", "=",
   * ".", ">", "?", "@", "[", "]", "^", "_", " {", "}", "|", "~", ","
   * @param userId The ID of the local user. If you do not specify a user ID or set `userId` as `null`,
   * the SDK returns a user ID in the \ref IRtcConnectionObserver::onConnected "onConnected"
   * callback. Your app must record and maintain the `userId` since the SDK does not do so.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   *   - -2(ERR_INVALID_ARGUMENT): The argument that you pass is invalid.
   *   - -8(ERR_INVALID_STATE): The current connection state is not CONNECTION_STATE_DISCONNECTED(1).
   */
  virtual int connect(const char* token, const char* channelId, user_id_t userId) = 0;

  /**
   * Disconnects from the Agora channel.
   *
   * Once your app successful disconnects from the channel, the connection state changes to
   * `CONNECTION_STATE_DISCONNECTED(1)`. You are also notified with the callback
   * \ref IRtcConnectionObserver::onDisconnected "onDisconnected".
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int disconnect() = 0;

  /**
   * Starts the last-mile network probe test.
   *
   * Call this method before connecting to the channel to get the uplink and
   * downlink last-mile network statistics, including the bandwidth, packet loss, jitter, and
   * round-trip time (RTT).
   *
   * After you enable the last-mile network probe test, the SDK triggers the following callbacks:
   * - \ref IRtcConnectionObserver::onLastmileQuality "onLastmileQuality": The SDK triggers this
   * callback within two seconds depending on the network conditions. This callback rates the network
   * conditions and is more closely linked to the user experience.
   * - \ref IRtcConnectionObserver::onLastmileProbeResult "onLastmileProbeResult": The SDK triggers
   * this callback within 30 seconds depending on the network conditions. This callback reports the
   * real-time statistics of the network conditions and is more objective.
   *
   * @note
   * - Do not call any other method before receiving the \ref IRtcConnectionObserver::onLastmileQuality
   * "onLastmileQuality" and \ref IRtcConnectionObserver::onLastmileProbeResult "onLastmileProbeResult"
   * callbacks. Otherwise, the callbacks may be interrupted.
   * - In the live-broadcast profile, a host should not call this method after connecting to a channel.
   *
   * @param config The configurations of the last-mile network probe test. See \ref agora::rtc::LastmileProbeConfig "LastmileProbeConfig".
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int startLastmileProbeTest(const LastmileProbeConfig& config) = 0;

  /**
   * Stops the last-mile network probe test.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int stopLastmileProbeTest() = 0;

  /**
   * Renews the token.
   *
   * The token expires after a certain period of time.
   * When the \ref IRtcConnectionObserver::onError "onError" callback reports `ERR_TOKEN_EXPIRED(109)`, you must generate a new token from the server
   * and then call this method to renew it. Otherwise, the SDK disconnects from the Agora channel.
   *
   * @param token The pointer to the new token.
   */
  virtual int renewToken(const char* token) = 0;

  /**
   * Gets the connection information.
   *
   * @return
   * - The pointer to the \ref agora::rtc::TConnectionInfo "TConnectionInfo" object: Success.
   * - A null pointer: Failure.
   */
  virtual TConnectionInfo getConnectionInfo() = 0;

  /**
   * Gets the \ref agora::rtc::ILocalUser "ILocalUser" object.
   *
   * @return
   * - The pointer to the \ref agora::rtc::ILocalUser "ILocalUser" object: Success.
   * - A null pointer: Failure.
   */
  virtual ILocalUser* getLocalUser() = 0;

  /**
   * Gets the information of all the remote users in the channel.
   *
   * After a user successfully connects to the channel, you can also get the information of this remote user
   * with the \ref IRtcConnectionObserver::onUserJoined "onUserJoined" callback.
   *
   * @param [out] users The reference to the \ref agora::UserList "UserList" object, which contains the information of all users
   * in the channel.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int getRemoteUsers(UserList& users) = 0;

  /**
   * Gets the information of a specified remote user in the channel.
   *
   * @param [in] userId ID of the user whose information you want to get.
   * @param [out] userInfo The reference to the \ref agora::UserInfo "UserInfo" object, which contains the information of the
   * specified user.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int getUserInfo(user_id_t userId, agora::UserInfo& userInfo) = 0;

  /**
   * Registers an RTC connection observer. You can call this method only after creating an `IRtcConnection` object.
   *
   * @param observer The pointer to the IRtcConnectionObserver object.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int registerObserver(IRtcConnectionObserver* observer, void(*safeDeleter)(IRtcConnectionObserver*) = NULL) = 0;

  /**
   * Releases the registered IRtcConnectionObserver object.
   *
   * @param observer The pointer to the IRtcConnectionObserver object created by the \ref registerObserver
   * "registerObserver" method.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int unregisterObserver(IRtcConnectionObserver* observer) = 0;

  /**
   * Registers an network observer object.
   *
   * @param observer The pointer to the INetworkObserver object.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int registerNetworkObserver(INetworkObserver* observer, void(*safeDeleter)(INetworkObserver*) = NULL) = 0;

  /**
   * Releases the registered INetworkObserver object.
   *
   * @param observer The pointer to the INetworkObserver object created by the \ref registerNetworkObserver
   * "registerNetworkObserver" method.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int unregisterNetworkObserver(INetworkObserver* observer) = 0;

  /**
   * Gets the ID of the connection.
   *
   * @return
   * - The connection ID: Success.
   * - A null pointer: Failure.
   */
  virtual conn_id_t getConnId() = 0;

  /**
   * Gets the transportation statistics of the RTC connection.
   *
   * @return
   * - The pointer to \ref agora::rtc::RtcStats "RtcStats": Success.
   * - A null pointer: Failure.
   */
  virtual RtcStats getTransportStats() = 0;

  /**
   * Gets the IAgoraParameter object.
   *
   * @return
   * - The pointer to the \ref agora::base::IAgoraParameter "IAgoraParameter" object.
   * - A null pointer: Failure.
   */
  virtual agora::base::IAgoraParameter* getAgoraParameter() = 0;

  /**
   * Creates a data stream.
   *
   * Each user can create up to five data streams during the lifecycle of an RTC connection.
   *
   * @note Set both the `reliable` and `ordered` parameters as `true` or `false`. Do not set one as `true` and the other as `false`.
   *
   * @param streamId The pointer to the ID of the data stream.
   * @param reliable Whether to guarantee the receivers receive the data stream within five seconds:
   * - `true`: Guarantee that the receivers receive the data stream within five seconds. If the receivers do not receive the data stream within five seconds, the SDK reports an error to the application.
   * - `false`: Do not guarantee that the receivers receive the data stream within five seconds and the SDK does not report any error message for data stream delay or missing.
   * @param ordered Whether the receivers receive the data stream in the order of sending:
   * - `true`: The receivers receive the data stream in the order of sending.
   * - `false`: The receivers do not receive the data stream in the order of sending.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int createDataStream(int* streamId, bool reliable, bool ordered, bool sync) = 0;

  /** Sends data stream messages to all users in a channel.
   *
   * @note This method has the following restrictions:
   * - Up to 30 packets can be sent per second in a channel with a maximum size of 1 kB for each packet.
   * - Each client can send up to 6 kB of data per second.
   * - Each user can have up to five data streams simultaneously.
   *
   * @param streamId The ID of the sent data stream, returned in the \ref agora::rtc::IRtcConnection::createDataStream "createDataStream" method.
   * @param data The pointer to the sent data.
   * @param length The length of the sent data.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int sendStreamMessage(int streamId, const char* data, size_t length) = 0;

  /** Enables/Disables the built-in encryption.
   *
   * In scenarios requiring high security, Agora recommends calling this method to enable the built-in encryption before joining a channel.
   *
   * All users in the same channel must use the same encryption mode and encryption key. Once all users leave the channel, the encryption key of this channel is automatically cleared.
   *
   * @note
   * - If you enable the built-in encryption, you cannot use the RTMP streaming function.
   * - Agora only supports the `SM4_128_ECB` encryption mode for now.
   *
   * @param enabled Whether to enable the built-in encryption:
   * - true: Enable the built-in encryption.
   * - false: Disable the built-in encryption.
   * @param config Configurations of built-in encryption schemas. See \ref agora::rtc::EncryptionConfig "EncryptionConfig".
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int enableEncryption(bool enabled, const EncryptionConfig& config) = 0;

  /**
   * Reports a custom event to Agora.
   *
   * @param id The custom event ID.
   * @param category The category of the custom event.
   * @param event The custom event to report.
   * @param label The label of the custom event.
   * @param value The value of the custom event.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int sendCustomReportMessage(const char* id, const char* category, const char* event, const char* label, int value) = 0;
  /** Gets the user information by user account, which is in string format.
   *
   * @param userAccount The user account of the user.
   * @param [in,out] userInfo A \ref rtc::UserInfo "UserInfo" object that identifies the user:
   * - Input: A userInfo object.
   * - Output: A userInfo object that contains the user account and user ID of the user.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int getUserInfoByUserAccount(const char* userAccount, rtc::UserInfo* userInfo) = 0;
  /** Gets the user information by user ID, which is in integer format.
   *
   * @param uid The ID of the remote user.
   * @param [in,out] userInfo A \ref rtc::UserInfo "UserInfo" object that identifies the user:
   * - Input: A userInfo object.
   * - Output: A userInfo object that contains the user account and user ID of the user.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int getUserInfoByUid(uid_t uid, rtc::UserInfo* userInfo) = 0;
};

/**
 * The IRtcConnectionObserver class, which observes the connection state of the SDK.
 */
class IRtcConnectionObserver {
 public:
  virtual ~IRtcConnectionObserver() {}

  /**
   * Occurs when the connection state between the SDK and the Agora channel changes to `CONNECTION_STATE_CONNECTED(3)`.
   *
   * @param connectionInfo The information of the connection. See \ref agora::rtc::TConnectionInfo "TConnectionInfo".
   * @param reason The reason of the connection state change. See #CONNECTION_CHANGED_REASON_TYPE.
   */
  virtual void onConnected(const TConnectionInfo& connectionInfo, CONNECTION_CHANGED_REASON_TYPE reason) = 0;

  /**
   * Occurs when the connection state between the SDK and the Agora channel changes to `CONNECTION_STATE_DISCONNECTED(1)`.
   *
   * @param connectionInfo The information of the connection. See \ref agora::rtc::TConnectionInfo "TConnectionInfo".
   * @param reason The reason of the connection state change. See #CONNECTION_CHANGED_REASON_TYPE.
   */
  virtual void onDisconnected(const TConnectionInfo& connectionInfo, CONNECTION_CHANGED_REASON_TYPE reason) = 0;

  /**
   * Occurs when the connection state between the SDK and the Agora channel changes to `CONNECTION_STATE_CONNECTING(2)`.
   *
   * @param connectionInfo The information of the connection. See \ref agora::rtc::TConnectionInfo "TConnectionInfo".
   * @param reason The reason of the connection state change. See #CONNECTION_CHANGED_REASON_TYPE.
   */
  virtual void onConnecting(const TConnectionInfo& connectionInfo, CONNECTION_CHANGED_REASON_TYPE reason) = 0;

  /**
   * Occurs when the connection state between the SDK and the Agora channel changes to `CONNECTION_STATE_RECONNECTING(4)`.
   *
   * @param connectionInfo The information of the connection. See \ref agora::rtc::TConnectionInfo "TConnectionInfo".
   * @param reason The reason of the connection state change. See #CONNECTION_CHANGED_REASON_TYPE.
   */
  virtual void onReconnecting(const TConnectionInfo& connectionInfo, CONNECTION_CHANGED_REASON_TYPE reason) = 0;

  // This should be deleted. onConnected is enough.
  virtual void onReconnected(const TConnectionInfo& connectionInfo, CONNECTION_CHANGED_REASON_TYPE reason) = 0;

  /**
   * Occurs when the SDK loses connection with the Agora channel.
   *
   * @param connectionInfo The information of the connection. See \ref agora::rtc::TConnectionInfo "TConnectionInfo".
   */
  virtual void onConnectionLost(const TConnectionInfo& connectionInfo) = 0;

  /**
   * Reports the quality of the last-mile network.
   *
   * The SDK triggers this callback within two seconds after the app calls \ref IRtcConnection::startLastmileProbeTest "startLastmileProbeTest".
   *
   * @param quality Quality of the last-mile network: #QUALITY_TYPE.
   */
  virtual void onLastmileQuality(const QUALITY_TYPE quality) = 0;

  /**
   * Reports the result of the last-mile network probe test.
   *
   * The SDK triggers this callback within 30 seconds after the app calls \ref IRtcConnection::startLastmileProbeTest "startLastmileProbeTest".
   *
   * @param result The result of the last-mile network probe test: \ref agora::rtc::LastmileProbeResult "LastmileProbeResult".
   */
  virtual void onLastmileProbeResult(const LastmileProbeResult& result) = 0;

  /**
   * Occurs when the token expires in 30 seconds.
   *
   * The SDK triggers this callback to remind the app to get a new token before the token privilege expires.
   *
   * Upon receiving this callback, you must generate a new token on your server and call \ref IRtcConnection::renewToken
   * "renewToken" to pass the new token to the SDK.
   *
   * @param token The pointer to the token that expires in 30 seconds.
   */
  virtual void onTokenPrivilegeWillExpire(const char* token) = 0;

  /**
   * Occurs when the token has expired.
   *
   * Upon receiving this callback, you must generate a new token on your server and call \ref IRtcConnection::renewToken
   * "renewToken" to pass the new token to the SDK.
   */
  virtual void onTokenPrivilegeDidExpire() = 0;

  /**
   * Occurs when the connection state between the SDK and the Agora channel changes to `CONNECTION_STATE_FAILED(5)`.
   *
   * @param connectionInfo The connection information: TConnectionInfo.
   * @param reason The reason of the connection state change: #CONNECTION_CHANGED_REASON_TYPE.
   */
  virtual void onConnectionFailure(const TConnectionInfo& connectionInfo,
                                   CONNECTION_CHANGED_REASON_TYPE reason) = 0;

  /**
   * Occurs when a remote user joins the channel.
   *
   * You can get the ID of the remote user in this callback.
   *
   * @param userId The ID of the remote user who joins the channel.
   */
  virtual void onUserJoined(user_id_t userId) = 0;

  /**
   * Occurs when a remote user leaves the channel.
   *
   * You can know why the user leaves the channel through the `reason` parameter.
   *
   * @param userId The ID of the user who leaves the channel.
   * @param reason The reason why the remote user leaves the channel: #USER_OFFLINE_REASON_TYPE.
   */
  virtual void onUserLeft(user_id_t userId, USER_OFFLINE_REASON_TYPE reason) = 0;

  /**
   * Reports the transport statistics of the connection.
   *
   * The SDK triggers this callback once every two seconds when the connection state is `CONNECTION_STATE_CONNECTED`.
   *
   * @param stats The pointer to \ref rtc::RtcStats "RtcStats".
   */
  virtual void onTransportStats(const RtcStats& stats) = 0;

  /**
   * Occurs when the role of the local user changes.
   *
   * @param oldRole The previous role of the local user: \ref rtc::CLIENT_ROLE_TYPE "CLIENT_ROLE_TYPE".
   * @param newRole The current role of the local user: \ref rtc::CLIENT_ROLE_TYPE "CLIENT_ROLE_TYPE".
   */
  virtual void onChangeRoleSuccess(CLIENT_ROLE_TYPE oldRole, CLIENT_ROLE_TYPE newRole) {
    (void)oldRole;
    (void)newRole;
  }

  /**
   * Occurs when the local user fails to change the user role.
   */
  virtual void onChangeRoleFailure() {}

  /**
   * Reports the network quality of each user.
   *
   * The SDK triggers this callback once every two seconds to report the uplink and downlink network conditions
   * of each user in the channel, including the local user.
   *
   * @param userId The ID of the user. If `userId` is empty, this callback reports the network quality of the local user.
   * @param txQuality The uplink network quality: #QUALITY_TYPE.
   * @param rxQuality The downlink network quality: #QUALITY_TYPE.
   */
  virtual void onUserNetworkQuality(user_id_t userId, QUALITY_TYPE txQuality,
                                    QUALITY_TYPE rxQuality) {
    (void)userId;
    (void)txQuality;
    (void)rxQuality;
  }

  /** Occurs when the network type is changed.
   * @param type The current network type. See #NETWORK_TYPE.
   */
  virtual void onNetworkTypeChanged(NETWORK_TYPE type) {
    (void)type;
  }

  /**
   * Occurs when an API method is executed.
   *
   * @param err The error code that the SDK reports when the method call fails. If the SDK reports 0,
   * the method call succeeds.
   * @param api The API method that is executed.
   * @param result The result of the method call.
   */
  virtual void onApiCallExecuted(int err, const char* api, const char* result) {
    (void)err;
    (void)api;
    (void)result;
  }

  /**
   * Reports the error code and error message.
   * @param error The error code: #ERROR_CODE_TYPE.
   * @param msg The error message.
   */
  virtual void onError(ERROR_CODE_TYPE error, const char* msg) {
    (void)error;
    (void)msg;
  }

  /**
   * Reports the warning code and warning message.
   * @param warning The warning code: #WARN_CODE_TYPE.
   * @param msg The warning message.
   */
  virtual void onWarning(WARN_CODE_TYPE warning, const char* msg) {
    (void)warning;
    (void)msg;
  }

  /**
   * Occurs when the state of the channel media relay changes.
   *
   *
   * @param state The state code:
   * - `RELAY_STATE_IDLE(0)`: The SDK is initializing.
   * - `RELAY_STATE_CONNECTING(1)`: The SDK tries to relay the media stream to the destination
   * channel.
   * - `RELAY_STATE_RUNNING(2)`: The SDK successfully relays the media stream to the destination
   * channel.
   * - `RELAY_STATE_FAILURE(3)`: A failure occurs. See the details in `code`.
   * @param code The error code:
   * - `RELAY_OK(0)`: The state is normal.
   * - `RELAY_ERROR_SERVER_ERROR_RESPONSE(1)`: An error occurs in the server response.
   * - `RELAY_ERROR_SERVER_NO_RESPONSE(2)`: No server response. You can call the leaveChannel method
   * to leave the channel.
   * - `RELAY_ERROR_NO_RESOURCE_AVAILABLE(3)`: The SDK fails to access the service, probably due to
   * limited resources of the server.
   * - `RELAY_ERROR_FAILED_JOIN_SRC(4)`: Fails to send the relay request.
   * - `RELAY_ERROR_FAILED_JOIN_DEST(5)`: Fails to accept the relay request.
   * - `RELAY_ERROR_FAILED_PACKET_RECEIVED_FROM_SRC(6)`: The server fails to receive the media
   * stream.
   * - `RELAY_ERROR_FAILED_PACKET_SENT_TO_DEST(7)`: The server fails to send the media stream.
   * - `RELAY_ERROR_SERVER_CONNECTION_LOST(8)`: The SDK disconnects from the server due to poor
   * network connections. You can call the leaveChannel method to leave the channel.
   * - `RELAY_ERROR_INTERNAL_ERROR(9)`: An internal error occurs in the server.
   * - `RELAY_ERROR_SRC_TOKEN_EXPIRED(10)`: The token of the source channel has expired.
   * - `RELAY_ERROR_DEST_TOKEN_EXPIRED(11)`: The token of the destination channel has expired.
   */
  virtual void onChannelMediaRelayStateChanged(int state, int code) = 0;

  /** Occurs when the local user successfully registers a user account by calling the \ref IRtcEngine::joinChannelWithUserAccount "joinChannelWithUserAccount" method.This callback reports the user ID and user account of the local user.
   *
   * @param uid The ID of the local user.
   * @param userAccount The user account of the local user.
   */
  virtual void onLocalUserRegistered(uid_t uid, const char* userAccount) {
    (void)uid;
    (void)userAccount;
  }

  /** Technical Preview, please do not depend on this event. */
  virtual void onUserAccountUpdated(uid_t uid, const char* userAccount){
    (void)uid;
    (void)userAccount;
  }

  /**
   * Reports the error that occurs when receiving data stream messages.
   *
   * @param userId The ID of the user sending the data stream.
   * @param streamId  the ID of the sent data stream, returned in the \ref agora::rtc::IRtcConnection::createDataStream "createDataStream" method.
   * @param code The error code.
   * @param missed The number of lost messages.
   * @param cached The number of incoming cached messages when the data stream is interrupted.
   */
  virtual void onStreamMessageError(user_id_t userId, int streamId, int code, int missed,
                                    int cached) {
    (void)userId;
    (void)streamId;
    (void)code;
    (void)missed;
    (void)cached;
  }

  /**
    * Reports the error type of encryption.
    * @param type See #ENCRYPTION_ERROR_TYPE.
    */
  virtual void onEncryptionError(ENCRYPTION_ERROR_TYPE errorType) {
    (void)errorType;
  }
};

class INetworkObserver {
 public:
  virtual ~INetworkObserver() {}

 public:
  /**
   * Occurs when downlink network info is updated.
   *
   * This callback is used for notifying user to adjust the send pace based
   * on the target bitrate.
   *
   * @param info The uplink network info collections.
   */
  virtual void onUplinkNetworkInfoUpdated(const UplinkNetworkInfo& info) {
    (void)info;
  }

  /**
   * Occurs when downlink network info is updated.
   *
   * This callback is used for notifying user to switch major/minor stream if needed.
   *
   * @param info The downlink network info collections.
   */
  virtual void onDownlinkNetworkInfoUpdated(const DownlinkNetworkInfo& info) {
    (void)info;
  }
};

}  // namespace rtc
}  // namespace agora
