//
//  Agora RTMP connection
//
//  Created by Haonong Yu in 2020-02.
//  Copyright (c) 2020 Agora.io. All rights reserved.
//

#pragma once  // NOLINT(build/header_guard)

#include "AgoraBase.h"
#include "AgoraRefPtr.h"

namespace agora {
namespace rtc {

class IRtmpLocalUser;

/**
 * Configurations for the RTMP audio stream.
 */
struct RtmpStreamingAudioConfiguration {
  /**
   * Sampling rate (Hz). The default value is 44100 (i.e. 44.1kHz).
   */
  int sampleRateHz;

  /**
   * Number of bytes per sample. The default value is 2 (i.e. 16-bit PCM).
   */
  int bytesPerSample;

  /**
   * Number of channels. The default value is 1 (i.e. mono).
   */
  int numberOfChannels;

  /**
   * The target bitrate (Kbps) of the output audio stream to be published.
   * The default value is 48 Kbps.
  */
  int bitrate;

  RtmpStreamingAudioConfiguration(): sampleRateHz(44100), bytesPerSample(2),
      numberOfChannels(1), bitrate(48000) {}
};

/**
 * Configurations for the RTMP video stream.
 */
struct RtmpStreamingVideoConfiguration {
  /**
   * The width (in pixels) of the video. The default value is 640.
   *
   * @note
   * - The value of the dimension (with the |height| below) does not indicate the orientation mode
   * of the output ratio. For how to set the video orientation,
   * see {@link OrientationMode OrientationMode}.
   */
  int width;

  /**
   * The height (in pixels) of the video. The default value is 360.
   *
   * @note
   * - The value of the dimension (with the |width| above) does not indicate the orientation mode
   * of the output ratio. For how to set the video orientation,
   * see {@link OrientationMode OrientationMode}.
   */
  int height;

  /**
   * Frame rate (fps) of the output video stream to be published. The default
   * value is 15 fps.
   */
  int framerate;

  /**
   * The target bitrate (Kbps) of the output video stream to be published.
   * The default value is 800 Kbps.
   */
  int bitrate;

  /**
   *  (For future use) The maximum bitrate (Kbps) for video.
   *  The default value is 960 Kbps.
   */
  int maxBitrate;

  /**
   *  (For future use) The minimum bitrate (Kbps) for video.
   *  The default value is 600 Kbps.
   */
  int minBitrate;

  /**
   *  The interval between two keyframes.
   *  The default value is 2000ms.
   */
  unsigned int gopInMs;

  /**
   *  Whether the encoder enables hard coding or soft coding.
   *  The default value is 0.
   *  0: default
   *  1: hardware encoder
   *  2: software encoder
   */
  int encoderHwSwMode;

  /**
   * The orientation mode.
   * See {@link ORIENTATION_MODE ORIENTATION_MODE}.
   */
  ORIENTATION_MODE orientationMode;

  RtmpStreamingVideoConfiguration(): width(640), height(360), framerate(15),
      bitrate(800), maxBitrate(960), minBitrate(600), gopInMs(2000), encoderHwSwMode(0),
      orientationMode(ORIENTATION_MODE_ADAPTIVE) {}
};


/**
 * The RTMP Connection error codes.
 */
enum RTMP_CONNECTION_ERROR {
  /**
   * 0: No error occurs.
   */
  RTMP_CONNECTION_ERR_OK = 0,

  /**
   * 1: A general error occurs (no specified reason).
   */
  RTMP_CONNECTION_ERR_FAILED = 1,

  /**
   * 2: The rtmp url is invalid.
   */
  RTMP_CONNECTION_ERR_INVALID_URL = 2,

  /**
   * 3: Already exist stream name.
   */
  RTMP_CONNECTION_ERR_BAD_NAME = 3,
};

/**
 * The connection state between the SDK and the RTMP server.
 */
enum RTMP_CONNECTION_STATE {
  /**
   * 1: The SDK is disconnected from the RTMP server.
   *
   * This is the initial state when an RTMP Connetion is created by
   * the \ref IAgoraService::createRtmoConnection "createRtmpConnection" method.
   *
   * The RTMP Connection also enters this state if you call the
   * \ref disconnect "disconnect" method when the SDK is in the
   * STATE_CONNECTING(2), STATE_CONNECTED(3), STATE_RECONNECTING(4) or
   * STATE_FAILED(5) state.
   */
  STATE_DISCONNECTED = 1,

  /**
   * 2: The SDK is connecting to the RTMP server.
   *
   * The SDK goes to this state after you call the \ref connect "connect" method,
   * indicating that the SDK is in the process of establishing a connection
   * to the RTMP server.
   *
   * Once successfully connected, it enters the STATE_CONNECTED(3) state.
   *
   * If the SDK fails to connect to RTMP server, the SDK goes to STATE_FAILED(5).
   */
  STATE_CONNECTING = 2,

  /**
   * 3: The SDK is connected to the RTMP server.
   *
   * This state indicates that the SDK has established a connection to the
   * RTMP server, and you can publish media streams through this connection.
   *
   * Once the connection is interrupted, for example, due to network deterioration or
   * network type change, the SDK tries to reconnect to the RTMP server
   * and enters the STATE_RECONNECTING(4) state.
   */
  STATE_CONNECTED = 3,

  /**
   * 4: The SDK is reconnecting to the RTMP server.
   *
   * This state indicates that the connection is interrupted by
   * some network issue. The SDK keeps trying connecting to the
   * server. If the SDK fails to reconnect, the SDK goes to
   * STATE_FAILED(5).
   */
  STATE_RECONNECTING = 4,

  /**
   * 5: The SDK fails to connect to the RTMP server.
   *
   * In this state, SDK stops connecting to the server. Call the
   * - \ref IRtmpConnection::disconnect "disconnect" method to leave this state and the
   * - \ref IRtmpConnection::connect "connect" method to reconnect.
   */
  STATE_FAILED = 5,

  /**
   * 6: The SDK is reconnected to the RTMP server.
   *
   * This state indicates that the connection is interrupted by
   * some network issue. The SDK keeps trying connecting to the
   * server. If the SDK reconnected to server, the SDK goes to
   * STATE_RECONNECTED(6).
   */
  STATE_RECONNECTED = 6
};

/**
 * Configurations for the RTMP connection.
 */
struct RtmpConnectionConfiguration {
  RtmpStreamingAudioConfiguration audioConfig;
  RtmpStreamingVideoConfiguration videoConfig;
  bool enableWriteFlvFile;
  RtmpConnectionConfiguration() : enableWriteFlvFile(false) {}
};

/**
 * The information on the RTMP Connection.
 */
struct RtmpConnectionInfo {
  /**
   * The state of the current connection: #RTMP_CONNECTION_STATE.
   */
  RTMP_CONNECTION_STATE state;

  RtmpConnectionInfo() : state(STATE_DISCONNECTED) {}
};


/**
 * The IRtmpConnectionObserver class, which observes the RTMP connection state of the SDK.
 */
class IRtmpConnectionObserver {
 public:
  /**
   * Occurs when the connection state between the SDK and the RTMP server changes to STATE_CONNECTED(3).
   *
   * @param connectionInfo The information of the current connection: RtmpConnectionInfo.
   */
  virtual void onConnected(const RtmpConnectionInfo& connectionInfo) = 0;

  /**
   * Occurs when the connection state between the SDK and the RTMP server changes to STATE_DISCONNECTED(1).
   *
   * @param connectionInfo The information of the current connection: RtmpConnectionInfo.
   */
  virtual void onDisconnected(const RtmpConnectionInfo& connectionInfo) = 0;

  /**
   * Occurs when the connection state between the SDK and RTMP server changes to STATE_RECONNECTING(4).
   *
   * @param connectionInfo The information of the current connection: RtmpConnectionInfo.
   */
  virtual void onReconnecting(const RtmpConnectionInfo& connectionInfo) = 0;

  /**
   * Occurs when the connection state between the SDK and RTMP server changes to STATE_RECONNECTED(6).
   *
   * @param connectionInfo The information of the current connection: RtmpConnectionInfo.
   */
  virtual void onReconnected(const RtmpConnectionInfo& connectionInfo) = 0;

  /**
   * Occurs when the connection state between the SDK and the RTMP server changes to STATE_FAILED(5).
   *
   * @param connectionInfo The connection information: RtmpConnectionInfo.
   * @param errCode The error code for the connection failure.
   */
  virtual void onConnectionFailure(const RtmpConnectionInfo& connectionInfo,
                                   RTMP_CONNECTION_ERROR errCode) = 0;

  /**
   * Occurs every 1s when the connection transmits data, report the current video bitrate, audio bitrate and video framerate.
   *
   * @param video_bitrate video_bitrate.
   * @param audio_bitrate audio_bitrate.
   * @param video_frame_rate video_frame_rate.
   */
  virtual void onTransferStatistics(uint64_t video_bitrate, uint64_t audio_bitrate, uint64_t video_frame_rate, uint64_t push_video_frame_cnt, uint64_t pop_video_frame_cnt) = 0;

  virtual ~IRtmpConnectionObserver() {}
};

/**
 * The IRtmpConnection class.
 *
 * With this class, an app can establish a connection to a RTMP server.
 * Once connected, the app gets an RtmpUser object, which
 * can publish media streams to the RTMP server.
 *
 * Connecting to the server is done asynchronous, and apps can listen for the
 * connection states or events with IRtmpConnectionObserver.
 */
class IRtmpConnection : public RefCountInterface {
 public:
    ~IRtmpConnection() {}

  /**
   * Connects to a RTMP server.
   *
   * When the method call succeeds, the RTMP connection state changes to
   * STATE_CONNECTING(2).
   *
   * Depending on the success or failure of the connection establishment, the RTMP
   * connection will make a second state transition to either
   * STATE_CONNECTED(3) or STATE_FAILED(5). You will also be notified with the either
   * onConnected() or onDisconnected().
   *
   * @param url The CDN streaming URL in the RTMP format. The maximum length of this parameter is 1024 
   * bytes. The URL address must not contain special characters, such as Chinese language characters.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   *   - ERR_INVALID_ARGUMENT: The passed in argument is invalid.
   *   - ERR_INVALID_STATE: The current connection state is not STATE_DISCONNECTED(3).
   */
  virtual int connect(const char* url) = 0;

  /**
   * Disconnects from the RTMP server.
   *
   * A successful call of this method changes the connection state to
   * STATE_DISCONNECTED(4). You will be notified with the callback
   * \ref onDisconnected "onDisconnected".
   */
  virtual int disconnect() = 0;

  /**
   * Gets the current connection information.
   *
   * @return
   * - A pointer to the RtmpConnectionInfo object, if the method call succeeds.
   * - An empty pointer NULL, if the method call fails.
   */
  virtual RtmpConnectionInfo getConnectionInfo() = 0;

  /**
   * Gets the IRtmpUser object.
   *
   * @return
   * - A pointer to the IRtmpUser object, if the method call succeeds.
   * - An empty pointer NULL, if the method call fails.
   */
  virtual IRtmpLocalUser* getRtmpLocalUser() = 0;

  /**
   * Registers an IRtmpConnectionObserver object.
   *
   * @param observer A pointer to the IRtmpConnectionObserver object.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int registerObserver(IRtmpConnectionObserver* observer, void(*safeDeleter)(IRtmpConnectionObserver*) = NULL) = 0;

  /**
   * Releases the registered IRtmpConnectionObserver object.
   *
   * @param observer The pointer to the IRtmpConnectionObserver object created by the \ref registerObserver
   * "registerObserver" method.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int unregisterObserver(IRtmpConnectionObserver* observer) = 0;
};

}  // namespace rtc
}  // namespace agora
