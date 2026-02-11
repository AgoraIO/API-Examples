//
//  Agora Engine SDK
//
//  Created by Sting Feng in 2017-11.
//  Copyright (c) 2017 Agora.io. All rights reserved.
//

// This header file is included by both high level and low level APIs,
#pragma once  // NOLINT(build/header_guard)

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <cassert>

#include "IAgoraParameter.h"
#include "AgoraMediaBase.h"
#include "AgoraRefPtr.h"
#include "AgoraOptional.h"

#define MAX_PATH_260 (260)

#if defined(_WIN32)

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif  // !WIN32_LEAN_AND_MEAN
#if defined(__aarch64__)
#include <arm64intr.h>
#endif
#include <Windows.h>

#if defined(AGORARTC_EXPORT)
#define AGORA_API extern "C" __declspec(dllexport)
#define AGORA_CPP_API __declspec(dllexport)
#else
#define AGORA_API extern "C" __declspec(dllimport)
#define AGORA_CPP_API __declspec(dllimport)
#endif  // AGORARTC_EXPORT

#define AGORA_CALL __cdecl

#define __deprecated

#define AGORA_CPP_INTERNAL_API extern

#elif defined(__APPLE__)

#include <TargetConditionals.h>

#define AGORA_API extern "C" __attribute__((visibility("default")))
#define AGORA_CPP_API __attribute__((visibility("default")))
#define AGORA_CALL

#define AGORA_CPP_INTERNAL_API __attribute__((visibility("hidden")))

#elif defined(__ANDROID__) || defined(__linux__)

#define AGORA_API extern "C" __attribute__((visibility("default")))
#define AGORA_CPP_API __attribute__((visibility("default")))
#define AGORA_CALL

#define __deprecated

#define AGORA_CPP_INTERNAL_API __attribute__((visibility("hidden")))

#else  // !_WIN32 && !__APPLE__ && !(__ANDROID__ || __linux__)

#define AGORA_API extern "C"
#define AGORA_CPP_API
#define AGORA_CALL

#define __deprecated

#endif  // _WIN32

#ifndef OPTIONAL_ENUM_SIZE_T
#if __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1800)
#define OPTIONAL_ENUM_SIZE_T enum : size_t
#else
#define OPTIONAL_ENUM_SIZE_T enum
#endif
#endif

#ifndef OPTIONAL_NULLPTR
#if __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1800)
#define OPTIONAL_NULLPTR nullptr
#else
#define OPTIONAL_NULLPTR NULL
#endif
#endif

#define INVALID_DISPLAY_ID (-2)

namespace agora {
namespace util {

template <class T>
class AutoPtr {
 protected:
  typedef T value_type;
  typedef T* pointer_type;

 public:
  explicit AutoPtr(pointer_type p = OPTIONAL_NULLPTR) : ptr_(p) {}

  ~AutoPtr() {
    if (ptr_) {
      ptr_->release();
      ptr_ = OPTIONAL_NULLPTR;
    }
  }

  operator bool() const { return (ptr_ != OPTIONAL_NULLPTR); }

  value_type& operator*() const { return *get(); }

  pointer_type operator->() const { return get(); }

  pointer_type get() const { return ptr_; }

  pointer_type release() {
    pointer_type ret = ptr_;
    ptr_ = 0;
    return ret;
  }

  void reset(pointer_type ptr = OPTIONAL_NULLPTR) {
    if (ptr != ptr_ && ptr_) {
      ptr_->release();
    }

    ptr_ = ptr;
  }

  template <class C1, class C2>
  bool queryInterface(C1* c, C2 iid) {
    pointer_type p = OPTIONAL_NULLPTR;
    if (c && !c->queryInterface(iid, reinterpret_cast<void**>(&p))) {
      reset(p);
    }

    return (p != OPTIONAL_NULLPTR);
  }

 private:
  AutoPtr(const AutoPtr&);
  AutoPtr& operator=(const AutoPtr&);

 private:
  pointer_type ptr_;
};

template <class T>
class CopyableAutoPtr : public AutoPtr<T> {
  typedef typename AutoPtr<T>::pointer_type pointer_type;

 public:
  explicit CopyableAutoPtr(pointer_type p = 0) : AutoPtr<T>(p) {}
  CopyableAutoPtr(const CopyableAutoPtr& rhs) { this->reset(rhs.clone()); }
  CopyableAutoPtr& operator=(const CopyableAutoPtr& rhs) {
    if (this != &rhs) this->reset(rhs.clone());
    return *this;
  }
  pointer_type clone() const {
    if (!this->get()) return OPTIONAL_NULLPTR;
    return this->get()->clone();
  }
};

class IString {
 public:
  virtual bool empty() const = 0;
  virtual const char* c_str() = 0;
  virtual const char* data() = 0;
  virtual size_t length() = 0;
  virtual IString* clone() = 0;
  virtual void release() = 0;
  virtual ~IString() {}
};
typedef CopyableAutoPtr<IString> AString;

class IIterator {
 public:
  virtual void* current() = 0;
  virtual const void* const_current() const = 0;
  virtual bool next() = 0;
  virtual void release() = 0;
  virtual ~IIterator() {}
};

class IContainer {
 public:
  virtual IIterator* begin() = 0;
  virtual size_t size() const = 0;
  virtual void release() = 0;
  virtual ~IContainer() {}
};

template <class T>
class AOutputIterator {
  IIterator* p;

 public:
  typedef T value_type;
  typedef value_type& reference;
  typedef const value_type& const_reference;
  typedef value_type* pointer;
  typedef const value_type* const_pointer;
  explicit AOutputIterator(IIterator* it = OPTIONAL_NULLPTR) : p(it) {}
  ~AOutputIterator() {
    if (p) p->release();
  }
  AOutputIterator(const AOutputIterator& rhs) : p(rhs.p) {}
  AOutputIterator& operator++() {
    p->next();
    return *this;
  }
  bool operator==(const AOutputIterator& rhs) const {
    if (p && rhs.p)
      return p->current() == rhs.p->current();
    else
      return valid() == rhs.valid();
  }
  bool operator!=(const AOutputIterator& rhs) const { return !this->operator==(rhs); }
  reference operator*() { return *reinterpret_cast<pointer>(p->current()); }
  const_reference operator*() const { return *reinterpret_cast<const_pointer>(p->const_current()); }
  bool valid() const { return p && p->current() != OPTIONAL_NULLPTR; }
};

template <class T>
class AList {
  IContainer* container;
  bool owner;

 public:
  typedef T value_type;
  typedef value_type& reference;
  typedef const value_type& const_reference;
  typedef value_type* pointer;
  typedef const value_type* const_pointer;
  typedef size_t size_type;
  typedef AOutputIterator<value_type> iterator;
  typedef const AOutputIterator<value_type> const_iterator;

 public:
  AList() : container(OPTIONAL_NULLPTR), owner(false) {}
  AList(IContainer* c, bool take_ownership) : container(c), owner(take_ownership) {}
  ~AList() { reset(); }
  void reset(IContainer* c = OPTIONAL_NULLPTR, bool take_ownership = false) {
    if (owner && container) container->release();
    container = c;
    owner = take_ownership;
  }
  iterator begin() { return container ? iterator(container->begin()) : iterator(OPTIONAL_NULLPTR); }
  iterator end() { return iterator(OPTIONAL_NULLPTR); }
  size_type size() const { return container ? container->size() : 0; }
  bool empty() const { return size() == 0; }
};

}  // namespace util

/**
 * @brief The channel profile.
 */
enum CHANNEL_PROFILE_TYPE {
  /**
   * 0: Communication. Use this profile when there are only two users in the channel.
   */
  CHANNEL_PROFILE_COMMUNICATION = 0,
  /**
   * 1: Live streaming. Use this profile when there are more than two users in the channel.
   */
  CHANNEL_PROFILE_LIVE_BROADCASTING = 1,
  /**
   * 2: Gaming.
   * @deprecated This profile is deprecated.
   */
  CHANNEL_PROFILE_GAME __deprecated = 2,
  /**
   * Cloud gaming. The scenario is optimized for latency. Use this profile if the use case requires
   * frequent interactions between users.
   * @deprecated This profile is deprecated.
   */
  CHANNEL_PROFILE_CLOUD_GAMING __deprecated = 3,

  /**
   * 4: Communication 1v1.
   * @deprecated This profile is deprecated.
   */
  CHANNEL_PROFILE_COMMUNICATION_1v1 __deprecated = 4,
};

/**
 * The warning codes.
 */
enum WARN_CODE_TYPE {
  /**
   * 8: The specified view is invalid. To use the video function, you need to specify
   * a valid view.
   */
  WARN_INVALID_VIEW = 8,
  /**
   * 16: Fails to initialize the video function, probably due to a lack of
   * resources. Users fail to see each other, but can still communicate with voice.
   */
  WARN_INIT_VIDEO = 16,
  /**
   * 20: The request is pending, usually because some module is not ready,
   * and the SDK postpones processing the request.
   */
  WARN_PENDING = 20,
  /**
   * 103: No channel resources are available, probably because the server cannot
   * allocate any channel resource.
   */
  WARN_NO_AVAILABLE_CHANNEL = 103,
  /**
   * 104: A timeout occurs when looking for the channel. When joining a channel,
   * the SDK looks up the specified channel. This warning usually occurs when the
   * network condition is too poor to connect to the server.
   */
  WARN_LOOKUP_CHANNEL_TIMEOUT = 104,
  /**
   * 105: The server rejects the request to look for the channel. The server
   * cannot process this request or the request is illegal.
   */
  WARN_LOOKUP_CHANNEL_REJECTED = 105,
  /**
   * 106: A timeout occurs when opening the channel. Once the specific channel
   * is found, the SDK opens the channel. This warning usually occurs when the
   * network condition is too poor to connect to the server.
   */
  WARN_OPEN_CHANNEL_TIMEOUT = 106,
  /**
   * 107: The server rejects the request to open the channel. The server
   * cannot process this request or the request is illegal.
   */
  WARN_OPEN_CHANNEL_REJECTED = 107,

  // sdk: 100~1000
  /**
   * 111: A timeout occurs when switching the live video.
   */
  WARN_SWITCH_LIVE_VIDEO_TIMEOUT = 111,
  /**
   * 118: A timeout occurs when setting the user role.
   */
  WARN_SET_CLIENT_ROLE_TIMEOUT = 118,
  /**
   * 121: The ticket to open the channel is invalid.
   */
  WARN_OPEN_CHANNEL_INVALID_TICKET = 121,
  /**
   * 122: The SDK is trying connecting to another server.
   */
  WARN_OPEN_CHANNEL_TRY_NEXT_VOS = 122,
  /**
   * 131: The channel connection cannot be recovered.
   */
  WARN_CHANNEL_CONNECTION_UNRECOVERABLE = 131,
  /**
   * 132: The SDK connection IP has changed.
   */
  WARN_CHANNEL_CONNECTION_IP_CHANGED = 132,
  /**
   * 133: The SDK connection port has changed.
   */
  WARN_CHANNEL_CONNECTION_PORT_CHANGED = 133,
  /** 134: The socket error occurs, try to rejoin channel.
   */
  WARN_CHANNEL_SOCKET_ERROR = 134,
  /**
   * 701: An error occurs when opening the file for audio mixing.
   */
  WARN_AUDIO_MIXING_OPEN_ERROR = 701,
  /**
   * 1014: Audio Device Module: An exception occurs in the playback device.
   */
  WARN_ADM_RUNTIME_PLAYOUT_WARNING = 1014,
  /**
   * 1016: Audio Device Module: A warning occurs in the recording device.
   */
  WARN_ADM_RUNTIME_RECORDING_WARNING = 1016,
  /**
   * 1019: Audio Device Module: No valid audio data is collected.
   */
  WARN_ADM_RECORD_AUDIO_SILENCE = 1019,
  /**
   * 1020: Audio Device Module: The playback device fails to start.
   */
  WARN_ADM_PLAYOUT_MALFUNCTION = 1020,
  /**
   * 1021: Audio Device Module: The recording device fails to start.
   */
  WARN_ADM_RECORD_MALFUNCTION = 1021,
  /**
   * 1031: Audio Device Module: The recorded audio volume is too low.
   */
  WARN_ADM_RECORD_AUDIO_LOWLEVEL = 1031,
  /**
   * 1032: Audio Device Module: The playback audio volume is too low.
   */
  WARN_ADM_PLAYOUT_AUDIO_LOWLEVEL = 1032,
  /**
   * 1040: Audio device module: An exception occurs with the audio drive.
   * Choose one of the following solutions:
   * - Disable or re-enable the audio device.
   * - Re-enable your device.
   * - Update the sound card drive.
   */
  WARN_ADM_WINDOWS_NO_DATA_READY_EVENT = 1040,
  /**
   * 1051: Audio Device Module: The SDK detects howling.
   */
  WARN_APM_HOWLING = 1051,
  /**
   * 1052: Audio Device Module: The audio device is in a glitching state.
   */
  WARN_ADM_GLITCH_STATE = 1052,
  /**
   * 1053: Audio Device Module: The settings are improper.
   */
  WARN_ADM_IMPROPER_SETTINGS = 1053,
  /**
   * 1055: Audio Device Module: The audio device is in a pop state.
   */
  WARN_ADM_POP_STATE = 1055,
  /**
   * 1322: No recording device.
   */
  WARN_ADM_WIN_CORE_NO_RECORDING_DEVICE = 1322,
  /**
   * 1323: Audio device module: No available playback device.
   * You can try plugging in the audio device.
   */
  WARN_ADM_WIN_CORE_NO_PLAYOUT_DEVICE = 1323,
  /**
   * 1324: Audio device module: The capture device is released improperly.
   * Choose one of the following solutions:
   * - Disable or re-enable the audio device.
   * - Re-enable your audio device.
   * - Update the sound card drive.
   */
  WARN_ADM_WIN_CORE_IMPROPER_CAPTURE_RELEASE = 1324,
};

/**
 * @brief Error codes.
 *
 * @details
 * An error code indicates that the SDK encountered an unrecoverable error that requires application
 * intervention. For example, an error is returned when the camera fails to open, and the app needs
 * to inform the user that the camera cannot be used.
 *
 */
enum ERROR_CODE_TYPE {
  /**
   * 0: No error.
   */
  ERR_OK = 0,
  // 1~1000
  /**
   * 1: General error with no classified reason. Try calling the method again.
   */
  ERR_FAILED = 1,
  /**
   * 2: An invalid parameter is used. For example, the specified channel name includes illegal
   * characters. Reset the parameter.
   */
  ERR_INVALID_ARGUMENT = 2,
  /**
   * 3: The SDK is not ready. Possible reasons include the following:
   * - The initialization of `IRtcEngine` fails. Reinitialize the `IRtcEngine`.
   * - No user has joined the channel when the method is called. Check the code logic.
   * - The user has not left the channel when the `rate` or `complain` method is called. Check the
   * code logic.
   * - The audio module is disabled.
   * - The program is not complete.
   */
  ERR_NOT_READY = 3,
  /**
   * 4: The `IRtcEngine` does not support the request. Possible reasons include the following:
   * - The built-in encryption mode is incorrect, or the SDK fails to load the external encryption
   * library. Check the encryption mode setting, or reload the external encryption library.
   */
  ERR_NOT_SUPPORTED = 4,
  /**
   * 5: The request is rejected. Possible reasons include the following:
   * - The `IRtcEngine` initialization fails. Reinitialize the `IRtcEngine`.
   * - The channel name is set as the empty string `""` when joining the channel. Reset the channel
   * name.
   * - When the `joinChannelEx` method is called to join multiple channels, the specified channel name
   * is already in use. Reset the channel name.
   */
  ERR_REFUSED = 5,
  /**
   * 6: The buffer size is insufficient to store the returned data.
   */
  ERR_BUFFER_TOO_SMALL = 6,
  /**
   * 7: A method is called before the initialization of `IRtcEngine`. Ensure that the `IRtcEngine`
   * object is initialized before using this method.
   */
  ERR_NOT_INITIALIZED = 7,
  /**
   * 8: Invalid state.
   */
  ERR_INVALID_STATE = 8,
  /**
   * 9: Permission to access is not granted. Check whether your app has access to the audio and video
   * device.
   */
  ERR_NO_PERMISSION = 9,
  /**
   * 10: A timeout occurs. Some API calls require the SDK to return the execution result. This error
   * occurs if the SDK takes too long (more than 10 seconds) to return the result.
   */
  ERR_TIMEDOUT = 10,
  /**
   * 11: The request is cancelled. This is for internal use only,
   * and does not return to the app through any method or callback.
   */
  ERR_CANCELED = 11,
  /**
   * 12: The method is called too often. This is for internal use
   * only, and does not return to the app through any method or
   * callback.
   */
  ERR_TOO_OFTEN = 12,
  /**
   * 13: The SDK fails to bind to the network socket. This is for internal
   * use only, and does not return to the app through any method or
   * callback.
   */
  ERR_BIND_SOCKET = 13,
  /**
   * 14: The network is unavailable. This is for internal use only, and
   * does not return to the app through any method or callback.
   */
  ERR_NET_DOWN = 14,
  /**
   * 17: The request to join the channel is rejected. Possible reasons include the following:
   * - The user is already in the channel. Agora recommends that you use the
   * `onConnectionStateChanged` callback to see whether the user is in the channel. Do not call this
   * method to join the channel unless you receive the `CONNECTION_STATE_DISCONNECTED` (1) state.
   * - After calling `startEchoTest` for the call test, the user tries to join the channel without
   * calling `stopEchoTest` to end the current test. To join a channel, the call test must be ended by
   * calling `stopEchoTest`.
   */
  ERR_JOIN_CHANNEL_REJECTED = 17,
  /**
   * 18: Fails to leave the channel. Possible reasons include the following:
   * - The user has left the channel before calling the `leaveChannel(const LeaveChannelOptions&
   * options)` method. Stop calling this
   * method to clear this error.
   * - The user calls the `leaveChannel(const LeaveChannelOptions& options)` method to leave the
   * channel before joining the channel.
   * In this case, no extra operation is needed.
   */
  ERR_LEAVE_CHANNEL_REJECTED = 18,
  /**
   * 19: Resources are already in use.
   */
  ERR_ALREADY_IN_USE = 19,
  /**
   * 20: The request is abandoned by the SDK, possibly because the request has been sent too
   * frequently.
   */
  ERR_ABORTED = 20,
  /**
   * 21: The `IRtcEngine` fails to initialize and has crashed because of specific Windows firewall
   * settings.
   */
  ERR_INIT_NET_ENGINE = 21,
  /**
   * 22: The SDK fails to allocate resources because your app uses too many system resources or system
   * resources are insufficient.
   */
  ERR_RESOURCE_LIMITED = 22,
  /**
   * 23: The function is prohibited. Please allow it in the console, or contact the Agora technical support.
   * @technical preview
   */
  ERR_FUNC_IS_PROHIBITED = 23,
  /**
   * 101: The specified App ID is invalid. Rejoin the channel with a valid App ID.
   */
  ERR_INVALID_APP_ID = 101,
  /**
   * 102: The specified channel name is invalid. A possible reason is that the parameter's data type
   * is incorrect. Rejoin the channel with a valid channel name.
   */
  ERR_INVALID_CHANNEL_NAME = 102,
  /**
   * 103: Fails to get server resources in the specified region. Try another region when initializing
   * `IRtcEngine`.
   */
  ERR_NO_SERVER_RESOURCES = 103,
  /**
   * 109: The current token has expired. Apply for a new token on the server and call `renewToken`.
   */
  ERR_TOKEN_EXPIRED = 109,
  /**
   * 110: Invalid token. Typical reasons include the following:
   * - App Certificate is enabled in Agora Console, but the code still uses App ID for authentication.
   * Once App Certificate is enabled for a project, you must use token-based authentication.
   * - The `uid` used to generate the token is not the same as the `uid` used to join the channel.
   */
  ERR_INVALID_TOKEN = 110,
  /**
   * 111: The network connection is interrupted. The SDK triggers this callback when it loses
   * connection with the server for more than four seconds after the connection is established.
   */
  ERR_CONNECTION_INTERRUPTED = 111,  // only used in web sdk
  /**
   * 112: The network connection is lost. Occurs when the SDK cannot reconnect to Agora's edge server
   * 10 seconds after its connection to the server is interrupted.
   */
  ERR_CONNECTION_LOST = 112,  // only used in web sdk
  /**
   * 113: The user is not in the channel when calling the `sendStreamMessage` method.
   */
  ERR_NOT_IN_CHANNEL = 113,
  /**
   * 114: The data size exceeds 1 KB when calling the `sendStreamMessage` method.
   */
  ERR_SIZE_TOO_LARGE = 114,
  /**
   * 115: The data bitrate exceeds 6 KB/s when calling the `sendStreamMessage` method.
   */
  ERR_BITRATE_LIMIT = 115,
  /**
   * 116: More than five data streams are created when calling the `createDataStream(int* streamId,
   * const DataStreamConfig& config)` method.
   */
  ERR_TOO_MANY_DATA_STREAMS = 116,
  /**
   * 117: The data stream transmission times out.
   */
  ERR_STREAM_MESSAGE_TIMEOUT = 117,
  /**
   * 119: Switching roles fails, try rejoining the channel.
   */
  ERR_SET_CLIENT_ROLE_NOT_AUTHORIZED = 119,
  /**
   * 120: Media streams decryption fails. The user might use an incorrect password to join the
   * channel. Check the entered password, or tell the user to try rejoining the channel.
   */
  ERR_DECRYPTION_FAILED = 120,
  /**
   * 121: The user ID is invalid.
   */
  ERR_INVALID_USER_ID = 121,
  /**
   * 122: Data streams decryption fails. The user might use an incorrect password to join the channel.
   * Check the entered password, or tell the user to try rejoining the channel.
   */
  ERR_DATASTREAM_DECRYPTION_FAILED = 122,
  /**
   * 123: The user is banned from the server.
   */
  ERR_CLIENT_IS_BANNED_BY_SERVER = 123,
  /**
   * 130: The SDK does not support pushing encrypted streams to CDN.
   */
  ERR_ENCRYPTED_STREAM_NOT_ALLOWED_PUBLISH = 130,

  /**
   * 131: License credential is invalid
   */
  ERR_LICENSE_CREDENTIAL_INVALID = 131,

  /**
   * 134: The user account is invalid, possibly because it contains invalid parameters.
   */
  ERR_INVALID_USER_ACCOUNT = 134,

  /** 157: The necessary dynamical library is not integrated. For example, if you call
   * the \ref agora::rtc::IRtcEngine::enableDeepLearningDenoise "enableDeepLearningDenoise" but do
   * not integrate the dynamical library for the deep-learning noise reduction into your project,
   * the SDK reports this error code.
   *
   */
  ERR_MODULE_NOT_FOUND = 157,

  // Licensing, keep the license error code same as the main version
  ERR_CERT_RAW = 157,
  ERR_CERT_JSON_PART = 158,
  ERR_CERT_JSON_INVAL = 159,
  ERR_CERT_JSON_NOMEM = 160,
  ERR_CERT_CUSTOM = 161,
  ERR_CERT_CREDENTIAL = 162,
  ERR_CERT_SIGN = 163,
  ERR_CERT_FAIL = 164,
  ERR_CERT_BUF = 165,
  ERR_CERT_NULL = 166,
  ERR_CERT_DUEDATE = 167,
  ERR_CERT_REQUEST = 168,

  // PcmSend Error num
  /**
   * 200: Unsupported PCM format.
   */
  ERR_PCMSEND_FORMAT = 200,          // unsupport pcm format
  /**
   * 201: Buffer overflow, the PCM send rate too quickly.
   */
  ERR_PCMSEND_BUFFEROVERFLOW = 201,  // buffer overflow, the pcm send rate too quickly

  /// @cond
  // RDT error code: 250~270
  /**
   * 250: The user does not exist
   * @technical preview
   */
  ERR_RDT_USER_NOT_EXIST = 250,
  /**
   * 251: The RDT state with the user is not ready
   * @technical preview
   */
  ERR_RDT_USER_NOT_READY = 251,
  /**
   * 252: The RDT data stream is blocked
   * @technical preview
   */
  ERR_RDT_DATA_BLOCKED = 252,
  /**
   * 253: The RDT CMD stream exceeds the limit (size <= 256 Bytes, freq <= 100/sec)
   * @technical preview
   */
  ERR_RDT_CMD_EXCEED_LIMIT = 253,
  /**
   * 254: The RDT DATA stream exceeds the limit (size <= 128 KBytes, speed <= 4 Mbps)
   * @technical preview
   */
  ERR_RDT_DATA_EXCEED_LIMIT = 254,
  /**
   * 255: The RDT encryption error. The SDK Failed to process RDT data encryption/decryption
   * @technical preview
   */
  ERR_RDT_ENCRYPTION = 255,
  /// @endcond

  /// @cond
  // signaling: 400~600
  ERR_LOGIN_ALREADY_LOGIN = 428,

  /// @endcond
  // 1001~2000
  /**
   * 1001: The SDK fails to load the media engine.
   */
  ERR_LOAD_MEDIA_ENGINE = 1001,
  /**
   * 1005: A general error occurs (no specified reason). Check whether the audio device is already in
   * use by another app, or try rejoining the channel.
   */
  ERR_ADM_GENERAL_ERROR = 1005,
  /**
   * 1008: An error occurs when initializing the playback device. Check whether the playback device is
   * already in use by another app, or try rejoining the channel.
   */
  ERR_ADM_INIT_PLAYOUT = 1008,
  /**
   * 1009: An error occurs when starting the playback device. Check the playback device.
   */
  ERR_ADM_START_PLAYOUT = 1009,
  /**
   * 1010: An error occurs when stopping the playback device.
   */
  ERR_ADM_STOP_PLAYOUT = 1010,
  /**
   * 1011: An error occurs when initializing the recording device. Check the recording device, or try
   * rejoining the channel.
   */
  ERR_ADM_INIT_RECORDING = 1011,
  /**
   * 1012: An error occurs when starting the recording device. Check the recording device.
   */
  ERR_ADM_START_RECORDING = 1012,
  /**
   * 1013: An error occurs when stopping the recording device.
   */
  ERR_ADM_STOP_RECORDING = 1013,
  /**
   * 1501: Permission to access the camera is not granted. Check whether permission to access the
   * camera permission is granted.
   */
  ERR_VDM_CAMERA_NOT_AUTHORIZED = 1501,
};

enum LICENSE_ERROR_TYPE {
  /**
   * 1: Invalid license
   */
  LICENSE_ERR_INVALID = 1,
  /**
   * 2: License expired
   */
  LICENSE_ERR_EXPIRE = 2,
  /**
   * 3: Exceed license minutes limit
   */
  LICENSE_ERR_MINUTES_EXCEED = 3,
  /**
   * 4: License use in limited period
   */
  LICENSE_ERR_LIMITED_PERIOD = 4,
  /**
   * 5: Same license used in different devices at the same time
   */
  LICENSE_ERR_DIFF_DEVICES = 5,
  /**
   * 99: SDK internal error
   */
  LICENSE_ERR_INTERNAL = 99,
};

/**
 * @brief The operation permissions of the SDK on the audio session.
 */
enum AUDIO_SESSION_OPERATION_RESTRICTION {
  /**
   * 0: No restriction, the SDK can change the audio session.
   */
  AUDIO_SESSION_OPERATION_RESTRICTION_NONE = 0,
  /**
   * 1: The SDK cannot change the audio session category.
   */
  AUDIO_SESSION_OPERATION_RESTRICTION_SET_CATEGORY = 1,
  /**
   * 2: The SDK cannot change the audio session category, mode, or categoryOptions.
   */
  AUDIO_SESSION_OPERATION_RESTRICTION_CONFIGURE_SESSION = 1 << 1,
  /**
   * 4: The SDK keeps the audio session active when the user leaves the channel, for example, to play
   * an audio file in the background.
   */
  AUDIO_SESSION_OPERATION_RESTRICTION_DEACTIVATE_SESSION = 1 << 2,
  /**
   * 128: Completely restricts the operation permissions of the SDK on the audio session; the SDK
   * cannot change the audio session.
   */
  AUDIO_SESSION_OPERATION_RESTRICTION_ALL = 1 << 7,
};

typedef const char* user_id_t;
typedef void* view_t;

/**
 * @brief The information of the user.
 */
struct UserInfo {
  /**
   * ID of the user.
   */
  util::AString userId;
  /**
   * Whether the user has enabled audio:
   * - true: The user has enabled audio.
   * - false: The user has disabled audio.
   */
  bool hasAudio;
  /**
   * Whether the user has enabled video:
   * - true: The user has enabled video.
   * - false: The user has disabled video.
   */
  bool hasVideo;

  UserInfo() : hasAudio(false), hasVideo(false) {}
};

typedef util::AList<UserInfo> UserList;

// Shared between Agora Service and Rtc Engine
namespace rtc {

/**
 * @brief Reasons for a user being offline.
 */
enum USER_OFFLINE_REASON_TYPE {
  /**
   * 0: The user quits the call.
   */
  USER_OFFLINE_QUIT = 0,
  /**
   * 1: The SDK times out and the user drops offline because no data packet is received within a
   * certain period of time.
   * @note If the user quits the call and the message is not passed to the SDK (due to an unreliable
   * channel), the SDK assumes the user dropped offline.
   */
  USER_OFFLINE_DROPPED = 1,
  /**
   * 2: The user switches the client role from the host to the audience.
   */
  USER_OFFLINE_BECOME_AUDIENCE = 2,
};

/**
 * @brief The interface class.
 */
enum INTERFACE_ID_TYPE {
  /**
   * 1: The `IAudioDeviceManager` interface class.
   */
  AGORA_IID_AUDIO_DEVICE_MANAGER = 1,
  /**
   * 2: The `IVideoDeviceManager` interface class.
   */
  AGORA_IID_VIDEO_DEVICE_MANAGER = 2,
  /**
   * This interface class is deprecated.
   */
  AGORA_IID_PARAMETER_ENGINE = 3,
  /**
   * 4: The `IMediaEngine` interface class.
   */
  AGORA_IID_MEDIA_ENGINE = 4,
  AGORA_IID_AUDIO_ENGINE = 5,
  AGORA_IID_VIDEO_ENGINE = 6,
  AGORA_IID_RTC_CONNECTION = 7,
  /**
   * This interface class is deprecated.
   */
  AGORA_IID_SIGNALING_ENGINE = 8,
  AGORA_IID_MEDIA_ENGINE_REGULATOR = 9,
  AGORA_IID_LOCAL_SPATIAL_AUDIO = 11,
  AGORA_IID_STATE_SYNC = 13,
  AGORA_IID_META_SERVICE = 14,
  AGORA_IID_MUSIC_CONTENT_CENTER = 15,
  AGORA_IID_H265_TRANSCODER = 16,
};

/**
 * @brief Network quality types.
 */
enum QUALITY_TYPE {
  /**
   * 0: The network quality is unknown.
   * @deprecated This member is deprecated.
   */
  QUALITY_UNKNOWN __deprecated = 0,
  /**
   * 1: The network quality is excellent.
   */
  QUALITY_EXCELLENT = 1,
  /**
   * 2: The network quality is quite good, but the bitrate may be slightly lower than excellent.
   */
  QUALITY_GOOD = 2,
  /**
   * 3: Users can feel the communication is slightly impaired.
   */
  QUALITY_POOR = 3,
  /**
   * 4: Users cannot communicate smoothly.
   */
  QUALITY_BAD = 4,
  /**
   * 5: The quality is so bad that users can barely communicate.
   */
  QUALITY_VBAD = 5,
  /**
   * 6: The network is down and users cannot communicate at all.
   */
  QUALITY_DOWN = 6,
  /**
   * 7: (For future use) The network quality cannot be detected.
   */
  QUALITY_UNSUPPORTED = 7,
  /**
   * 8: The last-mile network probe test is in progress.
   */
  QUALITY_DETECTING = 8,
};

/**
 * Content fit modes.
 */
enum FIT_MODE_TYPE {
  /**
   * 1: Uniformly scale the video until it fills the visible boundaries (cropped).
   * One dimension of the video may have clipped contents.
   */
  MODE_COVER = 1,

  /**
   * 2: Uniformly scale the video until one of its dimension fits the boundary
   * (zoomed to fit). Areas that are not filled due to disparity in the aspect
   * ratio are filled with black.
   */
  MODE_CONTAIN = 2,
};

/**
 * @brief The clockwise rotation of the video.
 */
enum VIDEO_ORIENTATION {
  /**
   * 0: (Default) No rotation.
   */
  VIDEO_ORIENTATION_0 = 0,
  /**
   * 90: 90 degrees.
   */
  VIDEO_ORIENTATION_90 = 90,
  /**
   * 180: 180 degrees.
   */
  VIDEO_ORIENTATION_180 = 180,
  /**
   * 270: 270 degrees.
   */
  VIDEO_ORIENTATION_270 = 270
};

/**
 * @brief The video frame rate.
 */
enum FRAME_RATE {
  /**
   * 1: 1 fps.
   */
  FRAME_RATE_FPS_1 = 1,
  /**
   * 7: 7 fps.
   */
  FRAME_RATE_FPS_7 = 7,
  /**
   * 10: 10 fps.
   */
  FRAME_RATE_FPS_10 = 10,
  /**
   * 15: 15 fps.
   */
  FRAME_RATE_FPS_15 = 15,
  /**
   * 24: 24 fps.
   */
  FRAME_RATE_FPS_24 = 24,
  /**
   * 30: 30 fps.
   */
  FRAME_RATE_FPS_30 = 30,
  /**
   * 60: 60 fps.
   * @note For Windows and macOS only.
   */
  FRAME_RATE_FPS_60 = 60,
};

enum FRAME_WIDTH {
  FRAME_WIDTH_960 = 960,
};

enum FRAME_HEIGHT {
  FRAME_HEIGHT_540 = 540,
};

/**
 * @brief The video frame type.
 */
enum VIDEO_FRAME_TYPE {
  /**
   * 0: A black frame.
   */
  VIDEO_FRAME_TYPE_BLANK_FRAME = 0,
  /**
   * 3: Key frame.
   */
  VIDEO_FRAME_TYPE_KEY_FRAME = 3,
  /**
   * 4: Delta frame.
   */
  VIDEO_FRAME_TYPE_DELTA_FRAME = 4,
  /**
   * 5: The B frame.
   */
  VIDEO_FRAME_TYPE_B_FRAME = 5,
  /**
   * 6: A discarded frame.
   */
  VIDEO_FRAME_TYPE_DROPPABLE_FRAME = 6,
  /**
   * Unknown frame.
   */
  VIDEO_FRAME_TYPE_UNKNOW
};

/**
 * @brief Video output orientation mode.
 */
enum ORIENTATION_MODE {
  /**
   * 0: (Default) The output video always follows the orientation of the captured video. The receiver
   * takes the rotational information passed on from the video encoder. This mode applies to scenarios
   * where video orientation can be adjusted on the receiver.
   * - If the captured video is in landscape mode, the output video is in landscape mode.
   * - If the captured video is in portrait mode, the output video is in portrait mode.
   */
  ORIENTATION_MODE_ADAPTIVE = 0,
  /**
   * 1: In this mode, the SDK always outputs videos in landscape (horizontal) mode. If the captured
   * video is in portrait mode, the video encoder crops it to fit the output. Applies to situations
   * where the receiving end cannot process the rotational information. For example, CDN live
   * streaming.
   */
  ORIENTATION_MODE_FIXED_LANDSCAPE = 1,
  /**
   * 2: In this mode, the SDK always outputs video in portrait (portrait) mode. If the captured video
   * is in landscape mode, the video encoder crops it to fit the output. Applies to situations where
   * the receiving end cannot process the rotational information. For example, CDN live streaming.
   */
  ORIENTATION_MODE_FIXED_PORTRAIT = 2,
};

/**
 * @brief Video degradation preferences when the bandwidth is a constraint.
 */
enum DEGRADATION_PREFERENCE {
  /**
   * -1: (Default) Automatic mode. The SDK will automatically select MAINTAIN_FRAMERATE,
   * MAINTAIN_BALANCED or MAINTAIN_RESOLUTION based on the video scenario you set, in order to achieve
   * the best overall quality of experience (QoE).
   */
  MAINTAIN_AUTO = -1,
  /**
   * 0: Prefers to reduce the video frame rate while maintaining video resolution during video
   * encoding under limited bandwidth. This degradation preference is suitable for scenarios where
   * video quality is prioritized.
   */
  MAINTAIN_QUALITY = 0,
  /**
   * 1: Reduces the video resolution while maintaining the video frame rate during video encoding
   * under limited bandwidth. This degradation preference is suitable for scenarios where smoothness
   * is prioritized and video quality is allowed to be reduced.
   */
  MAINTAIN_FRAMERATE = 1,
  /**
   * 2: Reduces the video frame rate and video resolution simultaneously during video encoding under
   * limited bandwidth. The MAINTAIN_BALANCED has a lower reduction than MAINTAIN_QUALITY and
   * MAINTAIN_FRAMERATE, and this preference is suitable for scenarios where both smoothness and video
   * quality are a priority.
   * @note The resolution of the video sent may change, so remote users need to handle this issue. See
   * `onVideoSizeChanged`.
   */
  MAINTAIN_BALANCED = 2,
  /**
   * 3: Reduces the video frame rate while maintaining the video resolution during video encoding
   * under limited bandwidth. This degradation preference is suitable for scenarios where video
   * quality is prioritized.
   */
  MAINTAIN_RESOLUTION = 3,
  /**
   * 4: Disable VQC adjustion.
   */
  DISABLED = 100,
};

/**
 * @brief The video dimension.
 */
struct VideoDimensions {
  /**
   * The width (pixels) of the video.
   */
  int width;
  /**
   * The height (pixels) of the video.
   */
  int height;
  VideoDimensions() : width(640), height(480) {}
  VideoDimensions(int w, int h) : width(w), height(h) {}
  bool operator==(const VideoDimensions& rhs) const {
    return width == rhs.width && height == rhs.height;
  }
};

/**
 * (Recommended) 0: Standard bitrate mode.
 *
 * In this mode, the video bitrate is twice the base bitrate.
 */
const int STANDARD_BITRATE = 0;

/**
 * -1: Compatible bitrate mode.
 *
 * In this mode, the video bitrate is the same as the base bitrate.. If you choose
 * this mode in the live-broadcast profile, the video frame rate may be lower
 * than the set value.
 */
const int COMPATIBLE_BITRATE = -1;

/**
 * -1: (For future use) The default minimum bitrate.
 */
const int DEFAULT_MIN_BITRATE = -1;

/**
 * -2: (For future use) Set minimum bitrate the same as target bitrate.
 */
const int DEFAULT_MIN_BITRATE_EQUAL_TO_TARGET_BITRATE = -2;

/**
 * @brief The highest frame rate supported by the screen sharing device.
 */
enum SCREEN_CAPTURE_FRAMERATE_CAPABILITY {
  /**
   * 0: The device supports the frame rate of up to 15 fps.
   */
  SCREEN_CAPTURE_FRAMERATE_CAPABILITY_15_FPS = 0,
  /**
   * 1: The device supports the frame rate of up to 30 fps.
   */
  SCREEN_CAPTURE_FRAMERATE_CAPABILITY_30_FPS = 1,
  /**
   * 2: The device supports the frame rate of up to 60 fps.
   */
  SCREEN_CAPTURE_FRAMERATE_CAPABILITY_60_FPS = 2,
};

/**
 * @brief The level of the codec capability.
 */
enum VIDEO_CODEC_CAPABILITY_LEVEL {
  /**
   * -1: Unsupported video type. Currently, only H.264 and H.265 formats are supported. If the video
   * is in another format, this value will be returned.
   */
  CODEC_CAPABILITY_LEVEL_UNSPECIFIED = -1,
  /**
   * 5: Supports encoding and decoding videos up to 1080p and 30 fps.
   */
  CODEC_CAPABILITY_LEVEL_BASIC_SUPPORT = 5,
  /**
   * 10: Supports encoding and decoding videos up to1080p and 30 fps.
   */
  CODEC_CAPABILITY_LEVEL_1080P30FPS = 10,
  /**
   * 20: Support encoding and decoding videos up to 1080p and 60 fps.
   */
  CODEC_CAPABILITY_LEVEL_1080P60FPS = 20,
  /**
   * 30: Support encoding and decoding videos up to 4K and 30 fps.
   */
  CODEC_CAPABILITY_LEVEL_4K60FPS = 30,
};

/**
 * @brief Video codec types.
 */
enum VIDEO_CODEC_TYPE {
  /**
   * 0: (Default) Unspecified codec format. The SDK automatically matches the appropriate codec format
   * based on the current video stream's resolution and device performance.
   */
  VIDEO_CODEC_NONE = 0,
  /**
   * 1: Standard VP8.
   */
  VIDEO_CODEC_VP8 = 1,
  /**
   * 2: Standard H.264.
   */
  VIDEO_CODEC_H264 = 2,
  /**
   * 3: Standard H.265.
   */
  VIDEO_CODEC_H265 = 3,
  /**
   * 6: Generic. This type is used for transmitting raw video data, such as encrypted video frames.
   * The SDK returns this type of video frames in callbacks, and you need to decode and render the
   * frames yourself.
   */
  VIDEO_CODEC_GENERIC = 6,
  /**
   * 7: Generic H264.
   * @deprecated This codec type is deprecated.
   */
  VIDEO_CODEC_GENERIC_H264 = 7,
  /**
   * 12: AV1.
   * @technical preview
   */
  VIDEO_CODEC_AV1 = 12,
  /**
   * 13: VP9.
   */
  VIDEO_CODEC_VP9 = 13,
  /**
   * 20: Generic JPEG. This type consumes minimum computing resources and applies to IoT devices.
   */
  VIDEO_CODEC_GENERIC_JPEG = 20,
};

/**
 * @brief The camera focal length types.
 *
 * @note This enumeration class applies to Android and iOS only.
 *
 */
enum CAMERA_FOCAL_LENGTH_TYPE {
  /**
   * 0: (Default) Standard lens.
   */
  CAMERA_FOCAL_LENGTH_DEFAULT = 0,
  /**
   * 1: Wide-angle lens.
   */
  CAMERA_FOCAL_LENGTH_WIDE_ANGLE = 1,
  /**
   * 2: Ultra-wide-angle lens.
   */
  CAMERA_FOCAL_LENGTH_ULTRA_WIDE = 2,
  /**
   * 3: (For iOS only) Telephoto lens.
   */
  CAMERA_FOCAL_LENGTH_TELEPHOTO = 3,
};

/**
 * The CC (Congestion Control) mode options.
 */
enum TCcMode {
  /**
   * Enable CC mode.
   */
  CC_ENABLED,
  /**
   * Disable CC mode.
   */
  CC_DISABLED,
};

/**
 * The configuration for creating a local video track with an encoded image sender.
 */
struct SenderOptions {
  /**
   * Whether to enable CC mode. See #TCcMode.
   */
  TCcMode ccMode;
  /**
   * The codec type used for the encoded images: \ref agora::rtc::VIDEO_CODEC_TYPE
   * "VIDEO_CODEC_TYPE".
   */
  VIDEO_CODEC_TYPE codecType;

  /**
   * Target bitrate (Kbps) for video encoding.
   *
   * Choose one of the following options:
   *
   * - \ref agora::rtc::STANDARD_BITRATE "STANDARD_BITRATE": (Recommended) Standard bitrate.
   *   - Communication profile: The encoding bitrate equals the base bitrate.
   *   - Live-broadcast profile: The encoding bitrate is twice the base bitrate.
   * - \ref agora::rtc::COMPATIBLE_BITRATE "COMPATIBLE_BITRATE": Compatible bitrate. The bitrate
   stays the same
   * regardless of the profile.
   *
   * The Communication profile prioritizes smoothness, while the Live Broadcast
   * profile prioritizes video quality (requiring a higher bitrate). Agora
   * recommends setting the bitrate mode as \ref agora::rtc::STANDARD_BITRATE "STANDARD_BITRATE" or
   simply to
   * address this difference.
   *
   * The following table lists the recommended video encoder configurations,
   * where the base bitrate applies to the communication profile. Set your
   * bitrate based on this table. If the bitrate you set is beyond the proper
   * range, the SDK automatically sets it to within the range.

   | Resolution             | Frame Rate (fps) | Base Bitrate (Kbps, for Communication) | Live
   Bitrate (Kbps, for Live Broadcast)|
   |------------------------|------------------|----------------------------------------|----------------------------------------|
   | 160 &times; 120        | 15               | 65                                     | 130 |
   | 120 &times; 120        | 15               | 50                                     | 100 |
   | 320 &times; 180        | 15               | 140                                    | 280 |
   | 180 &times; 180        | 15               | 100                                    | 200 |
   | 240 &times; 180        | 15               | 120                                    | 240 |
   | 320 &times; 240        | 15               | 200                                    | 400 |
   | 240 &times; 240        | 15               | 140                                    | 280 |
   | 424 &times; 240        | 15               | 220                                    | 440 |
   | 640 &times; 360        | 15               | 400                                    | 800 |
   | 360 &times; 360        | 15               | 260                                    | 520 |
   | 640 &times; 360        | 30               | 600                                    | 1200 |
   | 360 &times; 360        | 30               | 400                                    | 800 |
   | 480 &times; 360        | 15               | 320                                    | 640 |
   | 480 &times; 360        | 30               | 490                                    | 980 |
   | 640 &times; 480        | 15               | 500                                    | 1000 |
   | 480 &times; 480        | 15               | 400                                    | 800 |
   | 640 &times; 480        | 30               | 750                                    | 1500 |
   | 480 &times; 480        | 30               | 600                                    | 1200 |
   | 848 &times; 480        | 15               | 610                                    | 1220 |
   | 848 &times; 480        | 30               | 930                                    | 1860 |
   | 640 &times; 480        | 10               | 400                                    | 800 |
   | 1280 &times; 720       | 15               | 1130                                   | 2260 |
   | 1280 &times; 720       | 30               | 1710                                   | 3420 |
   | 960 &times; 720        | 15               | 910                                    | 1820 |
   | 960 &times; 720        | 30               | 1380                                   | 2760 |
   | 1920 &times; 1080      | 15               | 2080                                   | 4160 |
   | 1920 &times; 1080      | 30               | 3150                                   | 6300 |
   | 1920 &times; 1080      | 60               | 4780                                   | 6500 |
   | 2560 &times; 1440      | 30               | 4850                                   | 6500 |
   | 2560 &times; 1440      | 60               | 6500                                   | 6500 |
   | 3840 &times; 2160      | 30               | 6500                                   | 6500 |
   | 3840 &times; 2160      | 60               | 6500                                   | 6500 |
   */
  int targetBitrate;

  SenderOptions() : ccMode(CC_ENABLED), codecType(VIDEO_CODEC_H265), targetBitrate(6500) {}
};

/**
 * @brief The codec type of audio.
 */
enum AUDIO_CODEC_TYPE {
  /**
   * 1: OPUS.
   */
  AUDIO_CODEC_OPUS = 1,
  // kIsac = 2,
  /**
   * 3: PCMA.
   */
  AUDIO_CODEC_PCMA = 3,
  /**
   * 4: PCMU.
   */
  AUDIO_CODEC_PCMU = 4,
  /**
   * 5: G722.
   */
  AUDIO_CODEC_G722 = 5,
  // kIlbc = 6,
  /** 7: AAC. */
  // AUDIO_CODEC_AAC = 7,
  /**
   * 8: LC-AAC.
   */
  AUDIO_CODEC_AACLC = 8,
  /**
   * 9: HE-AAC.
   */
  AUDIO_CODEC_HEAAC = 9,
  /**
   * 10: JC1.
   */
  AUDIO_CODEC_JC1 = 10,
  /**
   * 11: HE-AAC v2.
   */
  AUDIO_CODEC_HEAAC2 = 11,
  /**
   * 12: LPCNET.
   */
  AUDIO_CODEC_LPCNET = 12,
  /**
   * 13: Opus codec, supporting 3 to 8 channels audio.
   */
  AUDIO_CODEC_OPUSMC = 13,
};

/**
 * @brief Audio encoding type.
 */
enum AUDIO_ENCODING_TYPE {
  /**
   * 0x010101: AAC encoding format, 16000 Hz sampling rate, bass quality. A file with an audio
   * duration of 10 minutes is approximately 1.2 MB after encoding.
   */
  AUDIO_ENCODING_TYPE_AAC_16000_LOW = 0x010101,
  /**
   * 0x010102: AAC encoding format, 16000 Hz sampling rate, medium sound quality. A file with an audio
   * duration of 10 minutes is approximately 2 MB after encoding.
   */
  AUDIO_ENCODING_TYPE_AAC_16000_MEDIUM = 0x010102,
  /**
   * 0x010201: AAC encoding format, 32000 Hz sampling rate, bass quality. A file with an audio
   * duration of 10 minutes is approximately 1.2 MB after encoding.
   */
  AUDIO_ENCODING_TYPE_AAC_32000_LOW = 0x010201,
  /**
   * 0x010202: AAC encoding format, 32000 Hz sampling rate, medium sound quality. A file with an audio
   * duration of 10 minutes is approximately 2 MB after encoding.
   */
  AUDIO_ENCODING_TYPE_AAC_32000_MEDIUM = 0x010202,
  /**
   * 0x010203: AAC encoding format, 32000 Hz sampling rate, high sound quality. A file with an audio
   * duration of 10 minutes is approximately 3.5 MB after encoding.
   */
  AUDIO_ENCODING_TYPE_AAC_32000_HIGH = 0x010203,
  /**
   * 0x010302: AAC encoding format, 48000 Hz sampling rate, medium sound quality. A file with an audio
   * duration of 10 minutes is approximately 2 MB after encoding.
   */
  AUDIO_ENCODING_TYPE_AAC_48000_MEDIUM = 0x010302,
  /**
   * 0x010303: AAC encoding format, 48000 Hz sampling rate, high sound quality. A file with an audio
   * duration of 10 minutes is approximately 3.5 MB after encoding.
   */
  AUDIO_ENCODING_TYPE_AAC_48000_HIGH = 0x010303,
  /**
   * 0x020101: OPUS encoding format, 16000 Hz sampling rate, bass quality. A file with an audio
   * duration of 10 minutes is approximately 2 MB after encoding.
   */
  AUDIO_ENCODING_TYPE_OPUS_16000_LOW = 0x020101,
  /**
   * 0x020102: OPUS encoding format, 16000 Hz sampling rate, medium sound quality. A file with an
   * audio duration of 10 minutes is approximately 2 MB after encoding.
   */
  AUDIO_ENCODING_TYPE_OPUS_16000_MEDIUM = 0x020102,
  /**
   * 0x020302: OPUS encoding format, 48000 Hz sampling rate, medium sound quality. A file with an
   * audio duration of 10 minutes is approximately 2 MB after encoding.
   */
  AUDIO_ENCODING_TYPE_OPUS_48000_MEDIUM = 0x020302,
  /**
   * 0x020303: OPUS encoding format, 48000 Hz sampling rate, high sound quality. A file with an audio
   * duration of 10 minutes is approximately 3.5 MB after encoding.
   */
  AUDIO_ENCODING_TYPE_OPUS_48000_HIGH = 0x020303,
};

/**
 * @brief The adaptation mode of the watermark.
 */
enum WATERMARK_FIT_MODE {
  /**
   * 0: Use the `positionInLandscapeMode` and `positionInPortraitMode` values you set in
   * `WatermarkOptions`. The settings in `WatermarkRatio` are invalid.
   */
  FIT_MODE_COVER_POSITION = 0,
  /**
   * 1: Use the value you set in `WatermarkRatio`. The settings in `positionInLandscapeMode` and
   * `positionInPortraitMode` in `WatermarkOptions` are invalid.
   */
  FIT_MODE_USE_IMAGE_RATIO = 1,
};

/**
 * The advanced settings of encoded audio frame.
 */
struct EncodedAudioFrameAdvancedSettings {
  EncodedAudioFrameAdvancedSettings() : speech(true), sendEvenIfEmpty(true) {}

  /**
   * Determines whether the audio source is speech.
   * - true: (Default) The audio source is speech.
   * - false: The audio source is not speech.
   */
  bool speech;
  /**
   * Whether to send the audio frame even when it is empty.
   * - true: (Default) Send the audio frame even when it is empty.
   * - false: Do not send the audio frame when it is empty.
   */
  bool sendEvenIfEmpty;
};

/**
 * @brief Audio information after encoding.
 */
struct EncodedAudioFrameInfo {
  EncodedAudioFrameInfo()
      : codec(AUDIO_CODEC_AACLC),
        sampleRateHz(0),
        samplesPerChannel(0),
        numberOfChannels(0),
        captureTimeMs(0) {}

  EncodedAudioFrameInfo(const EncodedAudioFrameInfo& rhs)
      : codec(rhs.codec),
        sampleRateHz(rhs.sampleRateHz),
        samplesPerChannel(rhs.samplesPerChannel),
        numberOfChannels(rhs.numberOfChannels),
        advancedSettings(rhs.advancedSettings),
        captureTimeMs(rhs.captureTimeMs) {}
  /**
   * Audio Codec type: `AUDIO_CODEC_TYPE`.
   */
  AUDIO_CODEC_TYPE codec;
  /**
   * Audio sample rate (Hz).
   */
  int sampleRateHz;
  /**
   * The number of audio samples per channel.
   */
  int samplesPerChannel;
  /**
   * The number of audio channels.
   */
  int numberOfChannels;
  /**
   * This function is currently not supported.
   */
  EncodedAudioFrameAdvancedSettings advancedSettings;

  /**
   * This is a input parameter which means the timestamp for capturing the audio frame.
   */
  int64_t captureTimeMs;
};
/**
 * The definition of the AudioPcmDataInfo struct.
 */
struct AudioPcmDataInfo {
  AudioPcmDataInfo()
      : samplesPerChannel(0), channelNum(0), samplesOut(0), elapsedTimeMs(0), ntpTimeMs(0) {}

  AudioPcmDataInfo(const AudioPcmDataInfo& rhs)
      : samplesPerChannel(rhs.samplesPerChannel),
        channelNum(rhs.channelNum),
        samplesOut(rhs.samplesOut),
        elapsedTimeMs(rhs.elapsedTimeMs),
        ntpTimeMs(rhs.ntpTimeMs) {}

  /**
   * The sample count of the PCM data that you expect.
   */
  size_t samplesPerChannel;

  int16_t channelNum;

  // Output
  /**
   * The number of output samples.
   */
  size_t samplesOut;
  /**
   * The rendering time (ms).
   */
  int64_t elapsedTimeMs;
  /**
   * The NTP (Network Time Protocol) timestamp (ms).
   */
  int64_t ntpTimeMs;
};
/**
 * Packetization modes. Applies to H.264 only.
 */
enum H264PacketizeMode {
  /**
   * Non-interleaved mode. See RFC 6184.
   */
  NonInterleaved = 0,  // Mode 1 - STAP-A, FU-A is allowed
  /**
   * Single NAL unit mode. See RFC 6184.
   */
  SingleNalUnit,  // Mode 0 - only single NALU allowed
};

/**
 * @brief The type of video streams.
 */
enum VIDEO_STREAM_TYPE {
  /**
   * 0: High-quality video stream, that is, a video stream with the highest resolution and bitrate.
   */
  VIDEO_STREAM_HIGH = 0,
  /**
   * 1: Low-quality video stream, that is, a video stream with the lowest resolution and bitrate.
   */
  VIDEO_STREAM_LOW = 1,
  /**
   * 4. Video stream layer 1. The resolution of this quality level is only lower than that of
   * VIDEO_STREAM_HIGH.
   */
  VIDEO_STREAM_LAYER_1 = 4,
  /**
   * 5: Video stream layer 2. The resolution of this quality level is only lower than that of
   * VIDEO_STREAM_LAYER_1.
   */
  VIDEO_STREAM_LAYER_2 = 5,
  /**
   * 6: Video stream layer 3. The resolution of this quality level is only lower than that of
   * VIDEO_STREAM_LAYER_2.
   */
  VIDEO_STREAM_LAYER_3 = 6,
  /**
   * 7: Video stream layer 4. The resolution of this quality level is only lower than that of
   * VIDEO_STREAM_LAYER_3.
   */
  VIDEO_STREAM_LAYER_4 = 7,
  /**
   * 8: Video stream layer 5. The resolution of this quality level is only lower than that of
   * VIDEO_STREAM_LAYER_4.
   */
  VIDEO_STREAM_LAYER_5 = 8,
  /**
   * 9: Video stream layer 6. The resolution of this quality level is only lower than that of
   * VIDEO_STREAM_LAYER_5.
   */
  VIDEO_STREAM_LAYER_6 = 9,

};

/**
 * @brief Video subscription options.
 */
struct VideoSubscriptionOptions {
  /**
   * The video stream type that you want to subscribe to. The default value is VIDEO_STREAM_HIGH,
   * indicating that the high-quality video streams are subscribed. See `VIDEO_STREAM_TYPE`.
   */
  Optional<VIDEO_STREAM_TYPE> type;
  /**
   * Whether to subscribe to encoded video frames only:
   * - `true`: Subscribe to the encoded video data (structured data) only; the SDK does not decode or
   * render raw video data.
   * - `false`: (Default) Subscribe to both raw video data and encoded video data.
   */
  Optional<bool> encodedFrameOnly;

  VideoSubscriptionOptions() {}
};

/**
 * @brief The maximum length of the user account.
 */
enum MAX_USER_ACCOUNT_LENGTH_TYPE {
  /**
   * The maximum length of the user account is 256 bytes.
   */
  MAX_USER_ACCOUNT_LENGTH = 256
};

/**
 * @brief Information about externally encoded video frames.
 */
struct EncodedVideoFrameInfo {
  EncodedVideoFrameInfo()
      : codecType(VIDEO_CODEC_H264),
        width(0),
        height(0),
        framesPerSecond(0),
        frameType(VIDEO_FRAME_TYPE_BLANK_FRAME),
        rotation(VIDEO_ORIENTATION_0),
        trackId(0),
        captureTimeMs(0),
        decodeTimeMs(0),
        streamType(VIDEO_STREAM_HIGH),
        presentationMs(-1) {}

  EncodedVideoFrameInfo(const EncodedVideoFrameInfo& rhs)
      : codecType(rhs.codecType),
        width(rhs.width),
        height(rhs.height),
        framesPerSecond(rhs.framesPerSecond),
        frameType(rhs.frameType),
        rotation(rhs.rotation),
        trackId(rhs.trackId),
        captureTimeMs(rhs.captureTimeMs),
        decodeTimeMs(rhs.decodeTimeMs),
        streamType(rhs.streamType),
        presentationMs(rhs.presentationMs) {}

  EncodedVideoFrameInfo& operator=(const EncodedVideoFrameInfo& rhs) {
    if (this == &rhs) return *this;
    codecType = rhs.codecType;
    width = rhs.width;
    height = rhs.height;
    framesPerSecond = rhs.framesPerSecond;
    frameType = rhs.frameType;
    rotation = rhs.rotation;
    trackId = rhs.trackId;
    captureTimeMs = rhs.captureTimeMs;
    decodeTimeMs = rhs.decodeTimeMs;
    streamType = rhs.streamType;
    presentationMs = rhs.presentationMs;
    return *this;
  }

  /**
   * The codec type of the local video stream. See `VIDEO_CODEC_TYPE`. The default value is
   * `VIDEO_CODEC_H264 (2)`.
   */
  VIDEO_CODEC_TYPE codecType;
  /**
   * Width (pixel) of the video frame.
   */
  int width;
  /**
   * Height (pixel) of the video frame.
   */
  int height;
  /**
   * The number of video frames per second.
   * When this parameter is not `0`, you can use it to calculate the Unix timestamp of externally
   * encoded video frames.
   */
  int framesPerSecond;
  /**
   * The video frame type. See `VIDEO_FRAME_TYPE`.
   */
  VIDEO_FRAME_TYPE frameType;
  /**
   * The rotation information of the video frame. See `VIDEO_ORIENTATION`.
   */
  VIDEO_ORIENTATION rotation;
  /**
   * Reserved for future use.
   */
  int trackId;  // This can be reserved for multiple video tracks, we need to create different ssrc
                // and additional payload for later implementation.
  /**
   * The Unix timestamp (ms) for capturing the external encoded video frames.
   */
  int64_t captureTimeMs;
  /**
   * The Unix timestamp (ms) for decoding the external encoded video frames.
   */
  int64_t decodeTimeMs;
  /**
   * The type of video streams. See `VIDEO_STREAM_TYPE`.
   */
  VIDEO_STREAM_TYPE streamType;

  // @technical preview
  int64_t presentationMs;
};

/**
 * @brief Compression preference for video encoding.
 */
enum COMPRESSION_PREFERENCE {
  /**
   * -1: (Default) Automatic mode. The SDK will automatically select PREFER_LOW_LATENCY or
   * PREFER_QUALITY based on the video scenario you set to achieve the best user experience.
   */
  PREFER_COMPRESSION_AUTO = -1,
  /**
   * 0: Low latency preference. The SDK compresses video frames to reduce latency. This preference is
   * suitable for scenarios where smoothness is prioritized and reduced video quality is acceptable.
   */
  PREFER_LOW_LATENCY = 0,
  /**
   * 1: High quality preference. The SDK compresses video frames while maintaining video quality. This
   * preference is suitable for scenarios where video quality is prioritized.
   */
  PREFER_QUALITY = 1,
};

/**
 * @brief Video encoder preference.
 */
enum ENCODING_PREFERENCE {
  /**
   * -1: Adaptive preference. The SDK automatically selects the optimal encoding type for encoding
   * based on factors such as platform and device type.
   */
  PREFER_AUTO = -1,
  /**
   * 0: Software coding preference. The SDK prefers software encoders for video encoding.
   */
  PREFER_SOFTWARE = 0,
  /**
   * 1: Hardware encoding preference. The SDK prefers a hardware encoder for video encoding. When the
   * device does not support hardware encoding, the SDK automatically uses software encoding and
   * reports the currently used video encoder type through `hwEncoderAccelerating` in the
   * `onLocalVideoStats` callback.
   */
  PREFER_HARDWARE = 1,
};

/**
 * @brief Advanced options for video encoding.
 */
struct AdvanceOptions {
  /**
   * Video encoder preference. See `ENCODING_PREFERENCE`.
   */
  ENCODING_PREFERENCE encodingPreference;

  /**
   * Compression preference for video encoding. See `COMPRESSION_PREFERENCE`.
   */
  COMPRESSION_PREFERENCE compressionPreference;

  /**
   * Whether to encode and send the Alpha data present in the video frame to the remote end:
   * - `true`: Encode and send Alpha data.
   * - `false`: (Default) Do not encode and send Alpha data.
   */
  bool encodeAlpha;

  AdvanceOptions() : encodingPreference(PREFER_AUTO), 
                     compressionPreference(PREFER_COMPRESSION_AUTO),
                     encodeAlpha(false) {}

  AdvanceOptions(ENCODING_PREFERENCE encoding_preference, 
                 COMPRESSION_PREFERENCE compression_preference,
                 bool encode_alpha) : 
                 encodingPreference(encoding_preference),
                 compressionPreference(compression_preference),
                 encodeAlpha(encode_alpha) {}

  bool operator==(const AdvanceOptions& rhs) const {
    return encodingPreference == rhs.encodingPreference && 
           compressionPreference == rhs.compressionPreference &&
           encodeAlpha == rhs.encodeAlpha;
  }
};

/**
 * @brief Video mirror mode.
 */
enum VIDEO_MIRROR_MODE_TYPE {
  /**
   * 0: The SDK determines the mirror mode.
   * - For the mirror mode of the local video view: If you use a front camera, the SDK enables the
   * mirror mode by default; if you use a rear camera, the SDK disables the mirror mode by default.
   * - For the remote user: The mirror mode is disabled by default.
   */
  VIDEO_MIRROR_MODE_AUTO = 0,
  /**
   * 1: Enable mirror mode.
   */
  VIDEO_MIRROR_MODE_ENABLED = 1,
  /**
   * 2: Disable mirror mode.
   */
  VIDEO_MIRROR_MODE_DISABLED = 2,
};

#if defined(__APPLE__) && TARGET_OS_IOS
/**
 * Camera capturer configuration for format type.
 */
enum CAMERA_FORMAT_TYPE {
  /** 0: (Default) NV12. */
  CAMERA_FORMAT_NV12,
  /** 1: BGRA. */
  CAMERA_FORMAT_BGRA,
};
#endif

enum VIDEO_MODULE_TYPE {
  /** Video capture module */
  VIDEO_MODULE_CAPTURER = 0,
  /** Video software encoder module */
  VIDEO_MODULE_SOFTWARE_ENCODER = 1,
  /** Video hardware encoder module */
  VIDEO_MODULE_HARDWARE_ENCODER = 2,
  /** Video software decoder module */
  VIDEO_MODULE_SOFTWARE_DECODER = 3,
  /** Video hardware decoder module */
  VIDEO_MODULE_HARDWARE_DECODER = 4,
  /** Video render module */
  VIDEO_MODULE_RENDERER = 5,
};
 
enum HDR_CAPABILITY {
  /** The result of static check is not reliable, by defualt*/
  HDR_CAPABILITY_UNKNOWN = -1,
  /** The module you query doesn't support HDR */
  HDR_CAPABILITY_UNSUPPORTED = 0,
  /** The module you query supports HDR */
  HDR_CAPABILITY_SUPPORTED = 1,
};

/**
 * @brief The bit mask of the codec type.
 */
enum CODEC_CAP_MASK {
  /**
   * (0): The device does not support encoding or decoding.
   */
  CODEC_CAP_MASK_NONE = 0,

  /**
   * (1 << 0): The device supports hardware decoding.
   */
  CODEC_CAP_MASK_HW_DEC = 1 << 0,

  /**
   * (1 << 1): The device supports hardware encoding.
   */
  CODEC_CAP_MASK_HW_ENC = 1 << 1,

  /**
   * (1 << 2): The device supports software decoding.
   */
  CODEC_CAP_MASK_SW_DEC = 1 << 2,

  /**
   * (1 << 3): The device supports software ecoding.
   */
  CODEC_CAP_MASK_SW_ENC = 1 << 3,
};

/**
 * @brief The level of the codec capability.
 */
struct CodecCapLevels {
  /**
   * Hardware decoding capability level, which represents the device's ability to perform hardware
   * decoding on videos of different quality. See `VIDEO_CODEC_CAPABILITY_LEVEL`.
   */
  VIDEO_CODEC_CAPABILITY_LEVEL hwDecodingLevel;
  /**
   * Software decoding capability level, which represents the device's ability to perform software
   * decoding on videos of different quality. See `VIDEO_CODEC_CAPABILITY_LEVEL`.
   */
  VIDEO_CODEC_CAPABILITY_LEVEL swDecodingLevel;

  CodecCapLevels()
      : hwDecodingLevel(CODEC_CAPABILITY_LEVEL_UNSPECIFIED),
        swDecodingLevel(CODEC_CAPABILITY_LEVEL_UNSPECIFIED) {}
};

/**
 * @brief The codec capability of the SDK.
 */
struct CodecCapInfo {
  /**
   * The video codec types. See `VIDEO_CODEC_TYPE`.
   */
  VIDEO_CODEC_TYPE codecType;
  /**
   * Bit mask of the codec types in SDK. See `CODEC_CAP_MASK`.
   */
  int codecCapMask;
  /**
   * Codec capability of the SDK. See `CodecCapLevels`.
   */
  CodecCapLevels codecLevels;

  CodecCapInfo() : codecType(VIDEO_CODEC_NONE), codecCapMask(0) {}
};

/**
 * @brief Focal length information supported by the camera, including the camera direction and focal
 * length type.
 *
 * @note This enumeration class applies to Android and iOS only.
 *
 */
struct FocalLengthInfo {
  /**
   * The camera direction. See `CAMERA_DIRECTION`.
   */
  int cameraDirection;
  /**
   * The focal length type. See `CAMERA_FOCAL_LENGTH_TYPE`.
   */
  CAMERA_FOCAL_LENGTH_TYPE focalLengthType;
};

/**
 * @brief Video encoder configurations.
 */
struct VideoEncoderConfiguration {
  /**
   * The codec type of the local video stream. See `VIDEO_CODEC_TYPE`.
   */
  VIDEO_CODEC_TYPE codecType;
  /**
   * The dimensions of the encoded video (px). See `VideoDimensions`. This parameter measures the
   * video encoding quality in the format of length × width. The default value is 960 × 540. You can
   * set a custom value.
   */
  VideoDimensions dimensions;
  /**
   * The frame rate (fps) of the encoding video frame. The default value is 15. See `FRAME_RATE`.
   */
  int frameRate;
  /**
   * The encoding bitrate (Kbps) of the video. This parameter does not need to be set; keeping the
   * default value `STANDARD_BITRATE` is sufficient. The SDK automatically matches the most suitable
   * bitrate based on the video resolution and frame rate you have set. For the correspondence between
   * video resolution and frame rate, see `Video profile`.
   * - STANDARD_BITRATE (0): (Recommended) Standard bitrate mode.
   * - COMPATIBLE_BITRATE (-1): Adaptive bitrate mode. In general, Agora suggests that you do not use
   * this value.
   */
  int bitrate;

  /**
   * The minimum encoding bitrate (Kbps) of the video.
   * The SDK automatically adjusts the encoding bitrate to adapt to the network conditions. Using a
   * value greater than the default value forces the video encoder to output high-quality images but
   * may cause more packet loss and sacrifice the smoothness of the video transmission. Unless you
   * have special requirements for image quality, Agora does not recommend changing this value.
   * @note This parameter only applies to the interactive streaming profile.
   */
  int minBitrate;
  /**
   * The orientation mode of the encoded video. See `ORIENTATION_MODE`.
   */
  ORIENTATION_MODE orientationMode;
  /**
   * Video degradation preference under limited bandwidth. See `DEGRADATION_PREFERENCE`.
   * @note When this parameter is set to MAINTAIN_FRAMERATE (1) or MAINTAIN_BALANCED (2),
   * `orientationMode` needs to be set to ORIENTATION_MODE_ADAPTIVE (0) at the same time, otherwise
   * the setting will not take effect.
   */
  DEGRADATION_PREFERENCE degradationPreference;

  /**
   * Sets the mirror mode of the published local video stream. It only affects the video that the
   * remote user sees. See `VIDEO_MIRROR_MODE_TYPE`.
   * @note By default, the video is not mirrored.
   */
  VIDEO_MIRROR_MODE_TYPE mirrorMode;

  /**
   * Advanced options for video encoding. See `AdvanceOptions`.
   */
  AdvanceOptions advanceOptions;

  VideoEncoderConfiguration(const VideoDimensions& d, int f, int b, ORIENTATION_MODE m, VIDEO_MIRROR_MODE_TYPE mirror = VIDEO_MIRROR_MODE_DISABLED)
    : codecType(VIDEO_CODEC_NONE),
      dimensions(d),
      frameRate(f),
      bitrate(b),
      minBitrate(DEFAULT_MIN_BITRATE),
      orientationMode(m),
      degradationPreference(MAINTAIN_AUTO),
      mirrorMode(mirror),
      advanceOptions(PREFER_AUTO, PREFER_COMPRESSION_AUTO, false) {}
  VideoEncoderConfiguration(int width, int height, int f, int b, ORIENTATION_MODE m, VIDEO_MIRROR_MODE_TYPE mirror = VIDEO_MIRROR_MODE_DISABLED)
    : codecType(VIDEO_CODEC_NONE),
      dimensions(width, height),
      frameRate(f),
      bitrate(b),
      minBitrate(DEFAULT_MIN_BITRATE),
      orientationMode(m),
      degradationPreference(MAINTAIN_AUTO),
      mirrorMode(mirror),
      advanceOptions(PREFER_AUTO, PREFER_COMPRESSION_AUTO, false) {}
  VideoEncoderConfiguration(const VideoEncoderConfiguration& config)
      : codecType(config.codecType),
        dimensions(config.dimensions),
        frameRate(config.frameRate),
        bitrate(config.bitrate),
        minBitrate(config.minBitrate),
        orientationMode(config.orientationMode),
        degradationPreference(config.degradationPreference),
        mirrorMode(config.mirrorMode),
        advanceOptions(config.advanceOptions) {}
  VideoEncoderConfiguration()
    : codecType(VIDEO_CODEC_NONE),
      dimensions(FRAME_WIDTH_960, FRAME_HEIGHT_540),
      frameRate(FRAME_RATE_FPS_15),
      bitrate(STANDARD_BITRATE),
      minBitrate(DEFAULT_MIN_BITRATE),
      orientationMode(ORIENTATION_MODE_ADAPTIVE),
      degradationPreference(MAINTAIN_AUTO),
      mirrorMode(VIDEO_MIRROR_MODE_DISABLED),
      advanceOptions(PREFER_AUTO, PREFER_COMPRESSION_AUTO, false) {}

  VideoEncoderConfiguration& operator=(const VideoEncoderConfiguration& rhs) {
    if (this == &rhs) return *this;
    codecType = rhs.codecType;
    dimensions = rhs.dimensions;
    frameRate = rhs.frameRate;
    bitrate = rhs.bitrate;
    minBitrate = rhs.minBitrate;
    orientationMode = rhs.orientationMode;
    degradationPreference = rhs.degradationPreference;
    mirrorMode = rhs.mirrorMode;
    advanceOptions = rhs.advanceOptions;
    return *this;
  }
};

/**
 * @brief The configurations for the data stream.
 *
 * @details
 * The following table shows the SDK behaviors under different parameter settings:
 * | `syncWithAudio` | `ordered` | SDK behaviors                                                                                                                                                                                                                                                                               |
 * | --------------- | --------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
 * | `false`         | `false`   | The SDK triggers the `onStreamMessage` callback immediately after the receiver receives a data packet.                                                                                                                                                                                      |
 * | `true`          | `false`   | If the data packet delay is within the audio delay, the SDK triggers the onStreamMessage callback when the synchronized audio packet is played out. If the data packet delay exceeds the audio delay, the SDK triggers the onStreamMessage callback as soon as the data packet is received. |
 * | `false`         | `true`    | If the delay of a data packet is less than five seconds, the SDK corrects the order of the data packet. If the delay of a data packet exceeds five seconds, the SDK discards the data packet.                                                                                               |
 * | `true`          | `true`    | If the delay of the data packet is within the range of the audio delay, the SDK corrects the order of the data packet. If the delay of a data packet exceeds the audio delay, the SDK discards this data packet.                                                                            |
 *
 */
struct DataStreamConfig {
  /**
   * Whether to synchronize the data packet with the published audio packet.
   * - `true`: Synchronize the data packet with the audio packet. This setting is suitable for special
   * scenarios such as lyrics synchronization.
   * - `false`: Do not synchronize the data packet with the audio packet. This setting is suitable for
   * scenarios where data packets need to arrive at the receiving end immediately.
   * When you set the data packet to synchronize with the audio, then if the data packet delay is
   * within the audio delay, the SDK triggers the `onStreamMessage` callback when the synchronized
   * audio packet is played out.
   */
  bool syncWithAudio;
  /**
   * Whether the SDK guarantees that the receiver receives the data in the sent order.
   * - `true`: Guarantee that the receiver receives the data in the sent order.
   * - `false`: Do not guarantee that the receiver receives the data in the sent order.
   * Do not set this parameter as `true` if you need the receiver to receive the data packet
   * immediately.
   */
  bool ordered;
};

/**
 * @brief The mode in which the video stream is sent.
 */
enum SIMULCAST_STREAM_MODE {
  /**
   * -1: By default, do not send the low-quality video stream until a subscription request for the
   * low-quality video stream is received from the receiving end, then automatically start sending
   * low-quality video stream.
   */
  AUTO_SIMULCAST_STREAM = -1,
  /**
   * 0: Never send low-quality video stream.
   */
  DISABLE_SIMULCAST_STREAM = 0,
  /**
   * 1: Always send low-quality video stream.
   */
  ENABLE_SIMULCAST_STREAM = 1,
};

/**
 * @brief The configuration of the low-quality video stream.
 */
struct SimulcastStreamConfig {
  /**
   * The video dimension. See `VideoDimensions`. The default value is 50% of the high-quality video
   * stream.
   */
  VideoDimensions dimensions;
  /**
   * Video bitrate (Kbps). The default value is -1. This parameter does not need to be set. The SDK
   * automatically matches the most suitable bitrate based on the video resolution and frame rate you
   * set.
   */
  int kBitrate;
  /**
   * The frame rate (fps) of the local video. The default value is 5.
   */
  int framerate;
  SimulcastStreamConfig() : dimensions(160, 120), kBitrate(65), framerate(5) {}
  SimulcastStreamConfig(const SimulcastStreamConfig& other) : dimensions(other.dimensions), kBitrate(other.kBitrate), framerate(other.framerate) {}
  bool operator==(const SimulcastStreamConfig& rhs) const {
    return dimensions == rhs.dimensions && kBitrate == rhs.kBitrate && framerate == rhs.framerate;
  }
};

/**
 * @brief Configure video streams of different quality levels.
 *
 * @since v4.6.0
 */
struct SimulcastConfig {
  /**
   * @brief Index of video streams of different quality levels.
   */
  enum StreamLayerIndex {
   /**
    * （0）: Video stream layer_1, with lower resolution and bitrate than VIDEO_STREAM_HIGH.
    */
   STREAM_LAYER_1 = 0,
   /**
    * （1）: Video stream layer_2, with lower resolution and bitrate than VIDEO_STREAM_LAYER_1.
    */
   STREAM_LAYER_2 = 1,
   /**
    * （2）: Video stream layer_3, with lower resolution and bitrate than VIDEO_STREAM_LAYER_2.
    */
   STREAM_LAYER_3 = 2,
   /**
    * （3）: Video stream layer_4, with lower resolution and bitrate than VIDEO_STREAM_LAYER_3.
    */
   STREAM_LAYER_4 = 3,
   /**
    * （4）: Video stream layer_5, with lower resolution and bitrate than VIDEO_STREAM_LAYER_4.
    */
   STREAM_LAYER_5 = 4,
   /**
    * （5）: Video stream layer_6, with lower resolution and bitrate than VIDEO_STREAM_LAYER_5.
    */
   STREAM_LAYER_6 = 5,
   /**
    * （6）: Low-quality video stream, with the lowest resolution and bitrate.
    */
   STREAM_LOW = 6,
   /**
    * （7）: Maximum number of video stream layers.
    */
   STREAM_LAYER_COUNT_MAX = 7
  };
  /**
   * @brief Configures the parameters of a specific layer in multi-quality video streams.
   *
   * @details
   * Used to configure the resolution, frame rate, and enable status of a specific layer in
   * multi-quality video streams.
   *
   */
  struct StreamLayerConfig {
    /**
     * Video frame size. Default is 0. See `VideoDimensions`.
     */
    VideoDimensions dimensions;
    /**
     * Frame rate (fps) of the local video capture. Default is 0.
     */
    int framerate;
    /**
     * Whether to enable the video stream for the corresponding layer. Default is false.
     * - `true`: Enables the video stream for the corresponding layer.
     * - `false`: (Default) Disables the video stream for the corresponding layer.
     */
    bool enable;
    StreamLayerConfig() : dimensions(0, 0), framerate(0), enable(false) {}
  };

  /**
   * Configurations for multi-layer streaming: `StreamLayerConfig`.
   */
  StreamLayerConfig configs[STREAM_LAYER_COUNT_MAX];
  /**
   * Whether to enable fallback publishing:
   * - `true`: Enable fallback publishing. When the device performance or network is poor at the
   * publishing end, the SDK will dynamically disable multiple video streams of different quality
   * levels, from layer1 to layer6. At least the video streams of the highest and lowest quality are
   * retained to maintain basic video continuity.
   * - `false`: (Default) Disable fallback publishing.
   */
  bool publish_fallback_enable;
  
  SimulcastConfig(): publish_fallback_enable(false) {}
};
/**
 * @brief The location of the target area relative to the screen or window. If you do not set this
 * parameter, the SDK selects the whole screen or window.
 */
struct Rectangle {
  /**
   * The horizontal offset from the top-left corner.
   */
  int x;
  /**
   * The vertical offset from the top-left corner.
   */
  int y;
  /**
   * The width of the target area.
   */
  int width;
  /**
   * The height of the target area.
   */
  int height;

  Rectangle() : x(0), y(0), width(0), height(0) {}
  Rectangle(int xx, int yy, int ww, int hh) : x(xx), y(yy), width(ww), height(hh) {}
};

/**
 * @brief The position and size of the watermark on the screen.
 *
 * @details
 * The position and size of the watermark on the screen are determined by `xRatio`, `yRatio`, and
 * `widthRatio`:
 * - ( `xRatio`, `yRatio` ) refers to the coordinates of the upper left corner of the watermark,
 * which determines the distance from the upper left corner of the watermark to the upper left
 * corner of the screen.
 * - The `widthRatio` determines the width of the watermark.
 *
 */
struct WatermarkRatio {
  /**
   * The x-coordinate of the upper left corner of the watermark. The horizontal position relative to
   * the origin, where the upper left corner of the screen is the origin, and the x-coordinate is the
   * upper left corner of the watermark. The value range is [0.0,1.0], and the default value is 0.
   */
  float xRatio;
  /**
   * The y-coordinate of the upper left corner of the watermark. The vertical position relative to the
   * origin, where the upper left corner of the screen is the origin, and the y-coordinate is the
   * upper left corner of the screen. The value range is [0.0,1.0], and the default value is 0.
   */
  float yRatio;
  /**
   * The width of the watermark. The SDK calculates the height of the watermark proportionally
   * according to this parameter value to ensure that the enlarged or reduced watermark image is not
   * distorted. The value range is [0,1], and the default value is 0, which means no watermark is
   * displayed.
   */
  float widthRatio;

  WatermarkRatio() : xRatio(0.0), yRatio(0.0), widthRatio(0.0) {}
  WatermarkRatio(float x, float y, float width) : xRatio(x), yRatio(y), widthRatio(width) {}
};

/**
 * @brief Watermark image configurations.
 *
 * @details
 * Configuration options for setting the watermark image to be added.
 *
 */
struct WatermarkOptions {
  /**
   * Whether the watermark is visible in the local preview view:
   * - `true`: (Default) The watermark is visible in the local preview view.
   * - `false`: The watermark is not visible in the local preview view.
   */
  bool visibleInPreview;
  /**
   * When the adaptation mode of the watermark is FIT_MODE_COVER_POSITION, it is used to set the area
   * of the watermark image in landscape mode. See `Rectangle`.
   */
  Rectangle positionInLandscapeMode;
  /**
   * When the adaptation mode of the watermark is FIT_MODE_COVER_POSITION, it is used to set the area
   * of the watermark image in portrait mode. See `Rectangle`.
   */
  Rectangle positionInPortraitMode;
  /**
   * When the watermark adaptation mode is FIT_MODE_USE_IMAGE_RATIO, this parameter is used to set the
   * watermark coordinates. See `WatermarkRatio`.
   */
  WatermarkRatio watermarkRatio;
  /**
   * The adaptation mode of the watermark. See `WATERMARK_FIT_MODE`.
   */
  WATERMARK_FIT_MODE mode;
  /**
   * Layer order of the watermark image. The default value is 0.
   */
  int zOrder;

  WatermarkOptions()
      : visibleInPreview(true),
        positionInLandscapeMode(0, 0, 0, 0),
        positionInPortraitMode(0, 0, 0, 0),
        mode(FIT_MODE_COVER_POSITION),
        zOrder(0) {}
};

/**
 * @brief Type of watermark source.
 *
 * @since 4.6.0
 */
enum WATERMARK_SOURCE_TYPE {
  /**
   * (0): The watermark source is an image.
   */
  IMAGE = 0,
  /**
   * (1): The watermark source is a buffer.
   */
  BUFFER = 1,
  /**
   * 2: The watermark source is a literal.
   *
   * @note This is only supported in linux platform.
   */
  LITERAL = 2,
  /**
   * 3: The watermark source is a timestamp.
   *
   * @note This is only supported in linux platform.
   */
  TIMESTAMPS = 3,
};

/**
 * @brief The definition of the WatermarkTimestamp struct.
 *
 * @since 4.6.0
 * @note This is only supported in linux platform.
 */
struct WatermarkTimestamp{
  /**
   * The font size of the timestamp. The default value is 10.
   */
  int fontSize;
  /**
   * The path of the font file for the timestamp. The default value is NULL.
   * The font file should be a .ttf file. If not set, the SDK uses the system default font if available.
   * 
   * @note If used asynchronously, copy the path to memory that will not be released.
   */
  const char* fontFilePath;
  /**
   * The stroke width of the timestamp. The default value is 1.
   */
  int strokeWidth;
  /**
   * The format of the timestamp. The default is '%F %X'.
   * The format follows the standard C library function strftime. You can find in the website: 
   * https://cplusplus.com/reference/ctime/strftime/?kw=strftime
   * 
   * @note If used asynchronously, copy the format string to memory that will not be released.
   */
  const char* format;

  WatermarkTimestamp() : fontSize(10), fontFilePath(NULL), strokeWidth(1), format(NULL) {}
};

/**
 * @brief The definition of the WatermarkLiteral struct.
 *
 * @since 4.6.0
 * @note This is only supported in linux platform..
 */
struct WatermarkLiteral {
  
  /**
   * The font size of the literal. The default value is 10.
   */
  int fontSize;
  /**
   * The stroke width of the literal. The default value is 1.
   */
  int strokeWidth;
  /**
   * The literal content of the watermark. The default value is NULL.
   * 
   * @note If used asynchronously, copy the string to memory that will not be released.
   */
  const char* wmLiteral;
  /**
   * The path of the font file for the literal. The default value is NULL.
   * The font file should be a .ttf file. If not set, the SDK uses the system default font if available.
   * 
   * @note If used asynchronously, copy the string to memory that will not be released.
   */
  const char* fontFilePath;

  WatermarkLiteral() : wmLiteral(NULL), fontFilePath(NULL), fontSize(10), strokeWidth(1) {}
};

/**
 * @brief Configures the format, size, and pixel buffer of the watermark image.
 *
 * @since 4.6.0
 *
 * @details
 * Defines the buffer data structure of the watermark image, including image width, height, format,
 * length, and image data buffer.
 *
 */
struct WatermarkBuffer {
  
  /**
   * Width of the watermark buffer, in pixels.
   */
  int width;
  /**
   * Height of the watermark buffer, in pixels.
   */
  int height;
  /**
   * Length of the watermark buffer, in bytes.
   */
  int length;
  /**
   * Format of the watermark buffer. See `VIDEO_PIXEL_FORMAT`. Default is VIDEO_PIXEL_I420. Currently
   * supported formats include: VIDEO_PIXEL_I420, VIDEO_PIXEL_RGBA, VIDEO_PIXEL_BGRA, and
   * VIDEO_PIXEL_NV21.
   */
  media::base::VIDEO_PIXEL_FORMAT format;

  /**
   * Buffer data of the watermark.
   */
  const uint8_t* buffer;

  WatermarkBuffer() : buffer(NULL), width(0), height(0), length(0), format(media::base::VIDEO_PIXEL_I420) {}
};

/**
 * @brief Used to configure watermark-related information.
 *
 * @since 4.6.0
 */
struct WatermarkConfig {
  /**
   * Unique identifier for the watermark. It is recommended to use a UUID.
   */
  const char* id;
  /**
   * Type of the watermark. See `WATERMARK_SOURCE_TYPE`.
   */
  WATERMARK_SOURCE_TYPE type;
  union {
    /**
     * Buffer of the watermark. See `WatermarkBuffer`.
     */
    WatermarkBuffer buffer;
    /**
     * The watermark timestamp. See WatermarkTimestamp.
     * 
     * @note This is only supported in linux platform.
     */
    WatermarkTimestamp timestamp;
    /**
     * The watermark literal. See WatermarkLiteral.
     *
     * @note This is only supported in linux platform.
     */
    WatermarkLiteral literal;
    /**
     * URL of the watermark image file. Default value is NULL.
     */
    const char* imageUrl;
  };
  
  /**
   * Options for the watermark. See `WatermarkOptions`.
   */
  WatermarkOptions options;

  WatermarkConfig() : id(NULL), type(IMAGE), imageUrl(NULL) {}
};

/**
 * @brief The transmission mode of data over multiple network paths.
 *
 * @since 4.6.0
 */
enum MultipathMode {
  /**
    * Duplicate mode, the same piece of data is redundantly transmitted over all available paths.
    * @technical preview
    */
  Duplicate= 0,
  /**
   * (1): Dynamic transmission mode. The SDK dynamically selects the optimal path for data
   * transmission based on the current network conditions to improve transmission performance.
   */
  Dynamic
};

/**
 * @brief Network path types used in multipath transmission.
 *
 * @since 4.6.0
 */
enum MultipathType {
  /**
   * (0): Local Area Network (LAN) path.
   */
  LAN = 0,
  /**
   * (1): Wi-Fi path.
   */
  WIFI,
  /**
   * (2): Mobile network path.
   */
  Mobile,
  /**
   * (99): Unknown or unspecified network path.
   */
  Unknown = 99
};

/**
 * @brief Statistical information about a specific network path.
 *
 * @since 4.6.0
 */
struct PathStats {
  /**
   * Types of network path. See `MultipathType`.
   */
  MultipathType type;
  /**
   * The transmission bitrate of the path in Kbps.
   */
  int txKBitRate;
  /**
   * The receiving bitrate of the path in Kbps.
   */
  int rxKBitRate;
  PathStats() : type(Unknown), txKBitRate(0), rxKBitRate(0) {}
  PathStats(MultipathType t, int tx, int rx) : type(t), txKBitRate(tx), rxKBitRate(rx) {}
};

/**
 * @brief Aggregates statistics of each network path in multipath transmission.
 *
 * @since 4.6.0
 */
struct MultipathStats {
  /**
   * The total number of bytes sent over the LAN path.
   */
  uint32_t lanTxBytes;
  /**
   * The total number of bytes received over the LAN path.
   */
  uint32_t lanRxBytes;
  /**
   * The total number of bytes sent over the Wi-Fi path.
   */
  uint32_t wifiTxBytes;
  /**
   * The total number of bytes received over the Wi-Fi path.
   */
  uint32_t wifiRxBytes;
  /**
   * The total number of bytes sent over the mobile network path.
   */
  uint32_t mobileTxBytes;
  /**
   * The total number of bytes received over the mobile network path.
   */
  uint32_t mobileRxBytes;
  /**
   * The number of active transmission paths.
   */
  int activePathNum;
  /**
   * An array of statistics for each active transmission path. See `PathStats`.
   */
  const PathStats* pathStats;
  MultipathStats()
      : lanTxBytes(0),
        lanRxBytes(0),
        wifiTxBytes(0),
        wifiRxBytes(0),
        mobileTxBytes(0),
        mobileRxBytes(0),
        activePathNum(0),
        pathStats(nullptr) {}
};

/**
 * @brief Statistics of a call session.
 */
struct RtcStats {
  /**
   * Call duration of the local user in seconds, represented by an aggregate value.
   */
  unsigned int duration;
  /**
   * The number of bytes sent.
   */
  unsigned int txBytes;
  /**
   * The number of bytes received.
   */
  unsigned int rxBytes;
  /**
   * The total number of audio bytes sent, represented by an aggregate value.
   */
  unsigned int txAudioBytes;
  /**
   * The total number of video bytes sent, represented by an aggregate value.
   */
  unsigned int txVideoBytes;
  /**
   * The total number of audio bytes received, represented by an aggregate value.
   */
  unsigned int rxAudioBytes;
  /**
   * The total number of video bytes received, represented by an aggregate value.
   */
  unsigned int rxVideoBytes;
  /**
   * The actual bitrate (Kbps) while sending the local video stream.
   */
  unsigned short txKBitRate;
  /**
   * The receiving bitrate (Kbps).
   */
  unsigned short rxKBitRate;
  /**
   * The bitrate (Kbps) of receiving the audio.
   */
  unsigned short rxAudioKBitRate;
  /**
   * The bitrate (Kbps) of sending the audio packet.
   */
  unsigned short txAudioKBitRate;
  /**
   * The bitrate (Kbps) of receiving the video.
   */
  unsigned short rxVideoKBitRate;
  /**
   * The bitrate (Kbps) of sending the video.
   */
  unsigned short txVideoKBitRate;
  /**
   * The client-to-server delay (milliseconds).
   */
  unsigned short lastmileDelay;
  /**
   * The number of users in the channel.
   */
  unsigned int userCount;
  /**
   * Application CPU usage (%).
   * @note
   * - The value of `cpuAppUsage` is always reported as 0 in the `onLeaveChannel` callback.
   * - As of Android 8.1, you cannot get the CPU usage from this attribute due to system limitations.
   */
  double cpuAppUsage;
  /**
   * The system CPU usage (%).
   * For Windows, in the multi-kernel environment, this member represents the average CPU usage. The
   * value = (100 - System Idle Progress in Task Manager)/100.
   * @note
   * - The value of `cpuTotalUsage` is always reported as 0 in the `onLeaveChannel` callback.
   * - As of Android 8.1, you cannot get the CPU usage from this attribute due to system limitations.
   */
  double cpuTotalUsage;
  /**
   * The round-trip time delay (ms) from the client to the local router.
   * @note
   * This property is disabled on devices running iOS 14 or later, and enabled on devices running
   * versions earlier than iOS 14 by default.
   * To enable this property on devices running iOS 14 or later, `technical support`.
   * On Android, to get `gatewayRtt`, ensure that you add the `android.permission.ACCESS_WIFI_STATE`
   * permission after `</application>` in the `AndroidManifest.xml` file in your project.
   */
  int gatewayRtt;
  /**
   * The memory ratio occupied by the app (%).
   * @note This value is for reference only. Due to system limitations, you may not get this value.
   */
  double memoryAppUsageRatio;
  /**
   * The memory occupied by the system (%).
   * @note This value is for reference only. Due to system limitations, you may not get this value.
   */
  double memoryTotalUsageRatio;
  /**
   * The memory size occupied by the app (KB).
   * @note This value is for reference only. Due to system limitations, you may not get this value.
   */
  int memoryAppUsageInKbytes;
  /**
   * The duration (ms) between the SDK starts connecting and the connection is established. If the
   * value reported is 0, it means invalid.
   */
  int connectTimeMs;
  /**
   * The duration (ms) between the app starting connecting to an Agora channel
   * and the first audio packet is received. 0 indicates that this member does not apply.
   */
  int firstAudioPacketDuration;
  /**
   * The duration (ms) between the app starting connecting to an Agora channel
   * and the first video packet is received. 0 indicates that this member does not apply.
   */
  int firstVideoPacketDuration;
  /**
   * The duration (ms) between the app starting connecting to an Agora channel
   * and the first video key frame is received. 0 indicates that this member does not apply.
   */
  int firstVideoKeyFramePacketDuration;
  /**
   * The number of video packets before the first video key frame is received.
   * 0 indicates that this member does not apply.
   */
  int packetsBeforeFirstKeyFramePacket;
  /**
   * The duration (ms) between the last time unmute audio and the first audio packet is received.
   * 0 indicates that this member does not apply.
   */
  int firstAudioPacketDurationAfterUnmute;
  /**
   * The duration (ms) between the last time unmute video and the first video packet is received.
   * 0 indicates that this member does not apply.
   */
  int firstVideoPacketDurationAfterUnmute;
  /**
   * The duration (ms) between the last time unmute video and the first video key frame is received.
   * 0 indicates that this member does not apply.
   */
  int firstVideoKeyFramePacketDurationAfterUnmute;
  /**
   * The duration (ms) between the last time unmute video and the first video key frame is decoded.
   * 0 indicates that this member does not apply.
   */
  int firstVideoKeyFrameDecodedDurationAfterUnmute;
  /**
   * The duration (ms) between the last time unmute video and the first video key frame is rendered.
   * 0 indicates that this member does not apply.
   */
  int firstVideoKeyFrameRenderedDurationAfterUnmute;
  /**
   * The packet loss rate (%) from the client to the Agora server before applying the anti-packet-loss
   * algorithm.
   */
  int txPacketLossRate;
  /**
   * The packet loss rate (%) from the Agora server to the client before using the anti-packet-loss
   * method.
   */
  int rxPacketLossRate;
  /**
   * The local network acceleration state.
   * A value of 1 indicates that local network acceleration is active, while 0 indicates it is inactive.
   * @technical preview
   */
  int lanAccelerateState;

  RtcStats()
      : duration(0),
        txBytes(0),
        rxBytes(0),
        txAudioBytes(0),
        txVideoBytes(0),
        rxAudioBytes(0),
        rxVideoBytes(0),
        txKBitRate(0),
        rxKBitRate(0),
        rxAudioKBitRate(0),
        txAudioKBitRate(0),
        rxVideoKBitRate(0),
        txVideoKBitRate(0),
        lastmileDelay(0),
        userCount(0),
        cpuAppUsage(0.0),
        cpuTotalUsage(0.0),
        gatewayRtt(0),
        memoryAppUsageRatio(0.0),
        memoryTotalUsageRatio(0.0),
        memoryAppUsageInKbytes(0),
        connectTimeMs(0),
        firstAudioPacketDuration(0),
        firstVideoPacketDuration(0),
        firstVideoKeyFramePacketDuration(0),
        packetsBeforeFirstKeyFramePacket(0),
        firstAudioPacketDurationAfterUnmute(0),
        firstVideoPacketDurationAfterUnmute(0),
        firstVideoKeyFramePacketDurationAfterUnmute(0),
        firstVideoKeyFrameDecodedDurationAfterUnmute(0),
        firstVideoKeyFrameRenderedDurationAfterUnmute(0),
        txPacketLossRate(0),
        rxPacketLossRate(0),
        lanAccelerateState(0) {}
};

/**
 * @brief The user role in the interactive live streaming.
 */
enum CLIENT_ROLE_TYPE {
  /**
   * 1: Host. A host can both send and receive streams.
   */
  CLIENT_ROLE_BROADCASTER = 1,
  /**
   * 2: (Default) Audience. An audience member can only receive streams.
   */
  CLIENT_ROLE_AUDIENCE = 2,
};

/**
 * @brief Quality change of the local video in terms of target frame rate and target bit rate since
 * last count.
 */
enum QUALITY_ADAPT_INDICATION {
  /**
   * 0: The local video quality stays the same.
   */
  ADAPT_NONE = 0,
  /**
   * 1: The local video quality improves because the network bandwidth increases.
   */
  ADAPT_UP_BANDWIDTH = 1,
  /**
   * 2: The local video quality deteriorates because the network bandwidth decreases.
   */
  ADAPT_DOWN_BANDWIDTH = 2,
};

/**
 * @brief The latency level of an audience member in interactive live streaming. This enum takes
 * effect only when the user role is set to CLIENT_ROLE_AUDIENCE .
 */
enum AUDIENCE_LATENCY_LEVEL_TYPE {
  /**
   * 1: Low latency.
   */
  AUDIENCE_LATENCY_LEVEL_LOW_LATENCY = 1,
  /**
   * 2: (Default) Ultra low latency.
   */
  AUDIENCE_LATENCY_LEVEL_ULTRA_LOW_LATENCY = 2,
};

/**
 * @brief Setting of user role properties.
 */
struct ClientRoleOptions {
  /**
   * The latency level of an audience member in interactive live streaming. See
   * `AUDIENCE_LATENCY_LEVEL_TYPE`.
   */
  AUDIENCE_LATENCY_LEVEL_TYPE audienceLatencyLevel;

  ClientRoleOptions() : audienceLatencyLevel(AUDIENCE_LATENCY_LEVEL_ULTRA_LOW_LATENCY) {}
};

/**
 * @brief The Quality of Experience (QoE) of the local user when receiving a remote audio stream.
 */
enum EXPERIENCE_QUALITY_TYPE {
  /**
   * 0: The QoE of the local user is good.
   */
  EXPERIENCE_QUALITY_GOOD = 0,
  /**
   * 1: The QoE of the local user is poor.
   */
  EXPERIENCE_QUALITY_BAD = 1,
};

/**
 * @brief Reasons why the QoE of the local user when receiving a remote audio stream is poor.
 */
enum EXPERIENCE_POOR_REASON {
  /**
   * 0: No reason, indicating a good QoE of the local user.
   */
  EXPERIENCE_REASON_NONE = 0,
  /**
   * 1: The remote user's network quality is poor.
   */
  REMOTE_NETWORK_QUALITY_POOR = 1,
  /**
   * 2: The local user's network quality is poor.
   */
  LOCAL_NETWORK_QUALITY_POOR = 2,
  /**
   * 4: The local user's Wi-Fi or mobile network signal is weak.
   */
  WIRELESS_SIGNAL_POOR = 4,
  /**
   * 8: The local user enables both Wi-Fi and bluetooth, and their signals interfere with each other.
   * As a result, audio transmission quality is undermined.
   */
  WIFI_BLUETOOTH_COEXIST = 8,
};

/**
 * @brief AI noise suppression modes.
 */
enum AUDIO_AINS_MODE {
  /**
   * 0: (Default) Balance mode. This mode allows for a balanced performance on noice suppression and
   * time delay.
   */
  AINS_MODE_BALANCED = 0,
  /**
   * 1: Aggressive mode. In scenarios where high performance on noise suppression is required, such as
   * live streaming outdoor events, this mode reduces nosie more dramatically, but may sometimes
   * affect the original character of the audio.
   */
  AINS_MODE_AGGRESSIVE = 1,
  /**
   * 2: Aggressive mode with low latency. The noise suppression delay of this mode is about only half
   * of that of the balance and aggressive modes. It is suitable for scenarios that have high
   * requirements on noise suppression with low latency, such as sing together online in real time.
   */
  AINS_MODE_ULTRALOWLATENCY = 2
};

/**
 * @brief The audio profile.
 */
enum AUDIO_PROFILE_TYPE {
  /**
   * 0: The default audio profile.
   * - For the interactive streaming profile: A sample rate of 48 kHz, music encoding, mono, and a
   * bitrate of up to 64 Kbps.
   * - For the communication profile:
   *   - Windows: A sample rate of 16 kHz, audio encoding, mono, and a bitrate of up to 16 Kbps.
   *   - Android/macOS/iOS: A sample rate of 32 kHz, audio encoding, mono, and a bitrate of up to 18
   * Kbps.
   */
  AUDIO_PROFILE_DEFAULT = 0,
  /**
   * 1: A sample rate of 32 kHz, audio encoding, mono, and a bitrate of up to 18 Kbps.
   */
  AUDIO_PROFILE_SPEECH_STANDARD = 1,
  /**
   * 2: A sample rate of 48 kHz, music encoding, mono, and a bitrate of up to 64 Kbps.
   */
  AUDIO_PROFILE_MUSIC_STANDARD = 2,
  /**
   * 3: A sample rate of 48 kHz, music encoding, stereo, and a bitrate of up to 80 Kbps. To implement
   * stereo audio, you also need to call `setAdvancedAudioOptions` and set `audioProcessingChannels`
   * to `AUDIO_PROCESSING_STEREO` in `AdvancedAudioOptions`.
   */
  AUDIO_PROFILE_MUSIC_STANDARD_STEREO = 3,
  /**
   * 4: A sample rate of 48 kHz, music encoding, mono, and a bitrate of up to 96 Kbps.
   */
  AUDIO_PROFILE_MUSIC_HIGH_QUALITY = 4,
  /**
   * 5: A sample rate of 48 kHz, music encoding, stereo, and a bitrate of up to 128 Kbps. To implement
   * stereo audio, you also need to call `setAdvancedAudioOptions` and set `audioProcessingChannels`
   * to `AUDIO_PROCESSING_STEREO` in `AdvancedAudioOptions`.
   */
  AUDIO_PROFILE_MUSIC_HIGH_QUALITY_STEREO = 5,
  /**
   * 6: A sample rate of 16 kHz, audio encoding, mono, and Acoustic Echo Cancellation (AEC) enabled.
   */
  AUDIO_PROFILE_IOT = 6,
  /**
   * Enumerator boundary.
   */
  AUDIO_PROFILE_NUM = 7
};

/**
 * @brief The audio scenarios.
 */
enum AUDIO_SCENARIO_TYPE {
  /**
   * 0: (Default) Automatic scenario match, where the SDK chooses the appropriate audio quality
   * according to the user role and audio route.
   */
  AUDIO_SCENARIO_DEFAULT = 0,
  /**
   * 3: High-quality audio scenario, where users mainly play music. For example, instrument tutoring.
   */
  AUDIO_SCENARIO_GAME_STREAMING = 3,
  /**
   * 5: Chatroom scenario, where users need to frequently switch the user role or mute and unmute the
   * microphone. For example, education scenarios.
   */
  AUDIO_SCENARIO_CHATROOM = 5,
  /**
   * 7: Real-time chorus scenario, where users have good network conditions and require ultra-low
   * latency.
   */
  AUDIO_SCENARIO_CHORUS = 7,
  /**
   * 8: Meeting scenario that mainly contains the human voice.
   */
  AUDIO_SCENARIO_MEETING = 8,
  /**
   * 9: AI Server.
   * @technical preview
   */
  AUDIO_SCENARIO_AI_SERVER = 9,
  /**
   * 10: AI conversation scenario, which is only applicable to scenarios where the user interacts with
   * the conversational AI agent created by `Conversational AI Engine`.
   */
  AUDIO_SCENARIO_AI_CLIENT = 10,
  /**
   * The number of enumerations.
   */
  AUDIO_SCENARIO_NUM = 11,
};

/**
 * @brief The format of the video frame.
 */
struct VideoFormat {
  OPTIONAL_ENUM_SIZE_T{
      /** The maximum value (px) of the width. */
      kMaxWidthInPixels = 3840,
      /** The maximum value (px) of the height. */
      kMaxHeightInPixels = 2160,
      /** The maximum value (fps) of the frame rate. */
      kMaxFps = 60,
  };

  /**
   * The width (px) of the video frame. The default value is 960.
   */
  int width;  // Number of pixels.
  /**
   * The height (px) of the video frame. The default value is 540.
   */
  int height;  // Number of pixels.
  /**
   * The video frame rate (fps). The default value is 15.
   */
  int fps;
  VideoFormat() : width(FRAME_WIDTH_960), height(FRAME_HEIGHT_540), fps(FRAME_RATE_FPS_15) {}
  VideoFormat(int w, int h, int f) : width(w), height(h), fps(f) {}

  bool operator<(const VideoFormat& fmt) const {
    if (height != fmt.height) {
      return height < fmt.height;
    } else if (width != fmt.width) {
      return width < fmt.width;
    } else {
      return fps < fmt.fps;
    }
  }
  bool operator==(const VideoFormat& fmt) const {
    return width == fmt.width && height == fmt.height && fps == fmt.fps;
  }
  bool operator!=(const VideoFormat& fmt) const { return !operator==(fmt); }
};

/**
 * @brief The content hint for screen sharing.
 */
enum VIDEO_CONTENT_HINT {
  /**
   * (Default) No content hint.
   */
  CONTENT_HINT_NONE,
  /**
   * Motion-intensive content. Choose this option if you prefer smoothness or when you are sharing a
   * video clip, movie, or video game.
   */
  CONTENT_HINT_MOTION,
  /**
   * Motionless content. Choose this option if you prefer sharpness or when you are sharing a picture,
   * PowerPoint slides, or texts.
   */
  CONTENT_HINT_DETAILS
};
/**
 * @brief The screen sharing scenario.
 */
enum SCREEN_SCENARIO_TYPE {
  /**
   * 1: (Default) Document. This scenario prioritizes the video quality of screen sharing and reduces
   * the latency of the shared video for the receiver. If you share documents, slides, and tables, you
   * can set this scenario.
   */
  SCREEN_SCENARIO_DOCUMENT = 1,
  /**
   * 2: Game. This scenario prioritizes the smoothness of screen sharing. If you share games, you can
   * set this scenario.
   */
  SCREEN_SCENARIO_GAMING = 2,
  /**
   * 3: Video. This scenario prioritizes the smoothness of screen sharing. If you share movies or live
   * videos, you can set this scenario.
   */
  SCREEN_SCENARIO_VIDEO = 3,
  /**
   * 4: Remote control. This scenario prioritizes the video quality of screen sharing and reduces the
   * latency of the shared video for the receiver. If you share the device desktop being remotely
   * controlled, you can set this scenario.
   */
  SCREEN_SCENARIO_RDC = 4,
};

/**
 * @brief The video application scenarios.
 */
enum VIDEO_APPLICATION_SCENARIO_TYPE {
  /**
   * 0: (Default) The general scenario.
   */
  APPLICATION_SCENARIO_GENERAL = 0,
  /**
   * 1: The meeting scenario.
   * `APPLICATION_SCENARIO_MEETING` (1) is suitable for meeting scenarios. The SDK automatically
   * enables the following strategies:
   * - In meeting scenarios where low-quality video streams are required to have a high bitrate, the
   * SDK automatically enables multiple technologies used to deal with network congestions, to enhance
   * the performance of the low-quality streams and to ensure the smooth reception by subscribers.
   * - The SDK monitors the number of subscribers to the high-quality video stream in real time and
   * dynamically adjusts its configuration based on the number of subscribers.
   *   - If nobody subscribers to the high-quality stream, the SDK automatically reduces its bitrate
   * and frame rate to save upstream bandwidth.
   *   - If someone subscribes to the high-quality stream, the SDK resets the high-quality stream to
   * the `VideoEncoderConfiguration` configuration used in the most recent calling of
   * `setVideoEncoderConfiguration`. If no configuration has been set by the user previously, the
   * following values are used:
   *     - Resolution: (Windows and macOS) 1280 × 720; (Android and iOS) 960 × 540
   *     - Frame rate: 15 fps
   *     - Bitrate: (Windows and macOS) 1600 Kbps; (Android and iOS) 1000 Kbps
   * - The SDK monitors the number of subscribers to the low-quality video stream in real time and
   * dynamically enables or disables it based on the number of subscribers.
   *   - If nobody subscribes to the low-quality stream, the SDK automatically disables it to save
   * upstream bandwidth.
   *   - If someone subscribes to the low-quality stream, the SDK enables the low-quality stream and
   * resets it to the `SimulcastStreamConfig` configuration used in the most recent calling of
   * `setDualStreamMode(SIMULCAST_STREAM_MODE mode, const SimulcastStreamConfig& streamConfig)`. If no
   * configuration has been set by the user previously, the following
   * values are used:
   *     - Resolution: 480 × 272
   *     - Frame rate: 15 fps
   *     - Bitrate: 500 Kbps
   * @note If the user has called `setDualStreamMode(SIMULCAST_STREAM_MODE mode, const
   * SimulcastStreamConfig& streamConfig)` to set that never send low-quality video
   * stream ( `DISABLE_SIMULCAST_STREAM` ), the dynamic adjustment of the low-quality stream in
   * meeting scenarios will not take effect.
   */
  APPLICATION_SCENARIO_MEETING = 1,
  /**
   * 2: 1v1 video call scenario.
   * `APPLICATION_SCENARIO_1V1` (2) This is applicable to the `one to one live` scenario. To meet the
   * requirements for low latency and high-quality video in this scenario, the SDK optimizes its
   * strategies, improving performance in terms of video quality, first frame rendering, latency on
   * mid-to-low-end devices, and smoothness under weak network conditions.
   * @note This enumeration value is only applicable to the broadcaster vs. broadcaster scenario.
   */
  APPLICATION_SCENARIO_1V1 = 2,
  /**
   * 3. Live show scenario.
   * `APPLICATION_SCENARIO_LIVESHOW` (3) This is applicable to the `show room` scenario. In this
   * scenario, fast video rendering and high image quality are crucial. The SDK implements several
   * performance optimizations, including automatically enabling accelerated audio and video frame
   * rendering to minimize first-frame latency (no need to call `enableInstantMediaRendering` ), and
   * B-frame encoding to achieve better image quality and bandwidth efficiency. The SDK also provides
   * enhanced video quality and smooth playback, even in poor network conditions or on lower-end
   * devices.
   */
  APPLICATION_SCENARIO_LIVESHOW = 3,
};

/**
 * The video QoE preference type.
 */
enum VIDEO_QOE_PREFERENCE_TYPE {
  /**
   * 1: Default QoE type, balance the delay, picture quality and fluency.
   */
  VIDEO_QOE_PREFERENCE_BALANCE = 1,
  /**
   * 2: lower the e2e delay.
   */
  VIDEO_QOE_PREFERENCE_DELAY_FIRST = 2,
  /**
   * 3: picture quality.
   */
  VIDEO_QOE_PREFERENCE_PICTURE_QUALITY_FIRST = 3,
  /**
   * 4: more fluency.
   */
  VIDEO_QOE_PREFERENCE_FLUENCY_FIRST = 4,

};

/**
 * @brief The brightness level of the video image captured by the local camera.
 */
enum CAPTURE_BRIGHTNESS_LEVEL_TYPE {
  /**
   * -1: The SDK does not detect the brightness level of the video image. Wait a few seconds to get
   * the brightness level from `captureBrightnessLevel` in the next callback.
   */
  CAPTURE_BRIGHTNESS_LEVEL_INVALID = -1,
  /**
   * 0: The brightness level of the video image is normal.
   */
  CAPTURE_BRIGHTNESS_LEVEL_NORMAL = 0,
  /**
   * 1: The brightness level of the video image is too bright.
   */
  CAPTURE_BRIGHTNESS_LEVEL_BRIGHT = 1,
  /**
   * 2: The brightness level of the video image is too dark.
   */
  CAPTURE_BRIGHTNESS_LEVEL_DARK = 2,
};

/**
 * @brief Camera stabilization modes.
 *
 * @details
 * The camera stabilization effect increases in the order of 1 < 2 < 3, and the latency will also
 * increase accordingly.
 *
 */
enum CAMERA_STABILIZATION_MODE {
  /**
   * -1: (Default) Camera stabilization mode off.
   */
  CAMERA_STABILIZATION_MODE_OFF = -1,
  /**
   * 0: Automatic camera stabilization. The system automatically selects a stabilization mode based on
   * the status of the camera. However, the latency is relatively high in this mode, so it is
   * recommended not to use this enumeration.
   */
  CAMERA_STABILIZATION_MODE_AUTO = 0,
  /**
   * 1: (Recommended) Level 1 camera stabilization.
   */
  CAMERA_STABILIZATION_MODE_LEVEL_1 = 1,
  /**
   * 2: Level 2 camera stabilization.
   */
  CAMERA_STABILIZATION_MODE_LEVEL_2 = 2,
  /**
   * 3: Level 3 camera stabilization.
   */
  CAMERA_STABILIZATION_MODE_LEVEL_3 = 3,
  /** The maximum level of the camera stabilization mode.
   */
  CAMERA_STABILIZATION_MODE_MAX_LEVEL = CAMERA_STABILIZATION_MODE_LEVEL_3,
};

/**
 * @brief The state of the local audio.
 */
enum LOCAL_AUDIO_STREAM_STATE {
  /**
   * 0: The local audio is in the initial state.
   */
  LOCAL_AUDIO_STREAM_STATE_STOPPED = 0,
  /**
   * 1: The local audio capturing device starts successfully.
   */
  LOCAL_AUDIO_STREAM_STATE_RECORDING = 1,
  /**
   * 2: The first audio frame encodes successfully.
   */
  LOCAL_AUDIO_STREAM_STATE_ENCODING = 2,
  /**
   * 3: The local audio fails to start.
   */
  LOCAL_AUDIO_STREAM_STATE_FAILED = 3
};

/**
 * @brief Reasons for local audio state changes.
 */
enum LOCAL_AUDIO_STREAM_REASON {
  /**
   * 0: The local audio is normal.
   */
  LOCAL_AUDIO_STREAM_REASON_OK = 0,
  /**
   * 1: No specified reason for the local audio failure. Remind your users to try to rejoin the
   * channel.
   */
  LOCAL_AUDIO_STREAM_REASON_FAILURE = 1,
  /**
   * 2: No permission to use the local audio capturing device. Remind your users to grant permission.
   */
  LOCAL_AUDIO_STREAM_REASON_DEVICE_NO_PERMISSION = 2,
  /**
   * 3: (Android and iOS only) The local audio capture device is already in use. Remind your users to
   * check whether another application occupies the microphone. Local audio capture automatically
   * resumes after the microphone is idle for about five seconds. You can also try to rejoin the
   * channel after the microphone is idle.
   */
  LOCAL_AUDIO_STREAM_REASON_DEVICE_BUSY = 3,
  /**
   * 4: The local audio capture fails.
   */
  LOCAL_AUDIO_STREAM_REASON_RECORD_FAILURE = 4,
  /**
   * 5: The local audio encoding fails.
   */
  LOCAL_AUDIO_STREAM_REASON_ENCODE_FAILURE = 5,
  /**
   * 6: (Windows and macOS only) No local audio capture device. Remind your users to check whether the
   * microphone is connected to the device properly in the control panel of the device or if the
   * microphone is working properly.
   */
  LOCAL_AUDIO_STREAM_REASON_NO_RECORDING_DEVICE = 6,
  /**
   * 7: (Windows and macOS only) No local audio capture device. Remind your users to check whether the
   * speaker is connected to the device properly in the control panel of the device or if the speaker
   * is working properly.
   */
  LOCAL_AUDIO_STREAM_REASON_NO_PLAYOUT_DEVICE = 7,
  /**
   * 8: (Android and iOS only) The local audio capture is interrupted by a system call, smart
   * assistants, or alarm clock. Prompt your users to end the phone call, smart assistants, or alarm
   * clock if the local audio capture is required.
   */
  LOCAL_AUDIO_STREAM_REASON_INTERRUPTED = 8,
  /**
   * 9: (Windows only) The ID of the local audio-capture device is invalid. Prompt the user to check
   * the audio capture device ID.
   */
  LOCAL_AUDIO_STREAM_REASON_RECORD_INVALID_ID = 9,
  /**
   * 10: (Windows only) The ID of the local audio-playback device is invalid. Prompt the user to check
   * the audio playback device ID.
   */
  LOCAL_AUDIO_STREAM_REASON_PLAYOUT_INVALID_ID = 10,
};

/**
 * @brief Local video state types.
 */
enum LOCAL_VIDEO_STREAM_STATE {
  /**
   * 0: The local video is in the initial state.
   */
  LOCAL_VIDEO_STREAM_STATE_STOPPED = 0,
  /**
   * 1: The local video capturing device starts successfully. The SDK also reports this state when you
   * call `startScreenCaptureByWindowId` to share a maximized window.
   */
  LOCAL_VIDEO_STREAM_STATE_CAPTURING = 1,
  /**
   * 2: The first video frame is successfully encoded.
   */
  LOCAL_VIDEO_STREAM_STATE_ENCODING = 2,
  /**
   * 3: Fails to start the local video.
   */
  LOCAL_VIDEO_STREAM_STATE_FAILED = 3
};

/**
 * @brief The local video event type.
 * @since v4.6.1
 */
enum LOCAL_VIDEO_EVENT_TYPE {
  /**
   * 1: (Android only) The screen capture window is hidden.
   */
  LOCAL_VIDEO_EVENT_TYPE_SCREEN_CAPTURE_WINDOW_HIDDEN = 1,
  /**
   * 2: (Android only) The screen capture window is recovered from hidden.
   */
  LOCAL_VIDEO_EVENT_TYPE_SCREEN_CAPTURE_WINDOW_RECOVER_FROM_HIDDEN = 2,
  /**
   * 3: (Android only) The screen capture is stopped by user.
   */
  LOCAL_VIDEO_EVENT_TYPE_SCREEN_CAPTURE_STOPPED_BY_USER = 3,
  /**
   * 4: (Android only) An internal error occurs during the screen capture.
   */
  LOCAL_VIDEO_EVENT_TYPE_SCREEN_CAPTURE_SYSTEM_INTERNAL_ERROR = 4,
};

/**
 * @brief Reasons for local video state changes.
 */
enum LOCAL_VIDEO_STREAM_REASON {
  /**
   * 0: The local video is normal.
   */
  LOCAL_VIDEO_STREAM_REASON_OK = 0,
  /**
   * 1: No specified reason for the local video failure.
   */
  LOCAL_VIDEO_STREAM_REASON_FAILURE = 1,
  /**
   * 2: No permission to use the local video capturing device. Prompt the user to grant permissions
   * and rejoin the channel.
   */
  LOCAL_VIDEO_STREAM_REASON_DEVICE_NO_PERMISSION = 2,
  /**
   * 3: The local video capturing device is in use. Prompt the user to check if the camera is being
   * used by another app, or try to rejoin the channel.
   */
  LOCAL_VIDEO_STREAM_REASON_DEVICE_BUSY = 3,
  /**
   * 4: The local video capture fails. Prompt the user to check whether the video capture device is
   * working properly, whether the camera is used by another app, or try to rejoin the channel.
   */
  LOCAL_VIDEO_STREAM_REASON_CAPTURE_FAILURE = 4,
  /**
   * 5: The local video encoding fails.
   */
  LOCAL_VIDEO_STREAM_REASON_CODEC_NOT_SUPPORT = 5,
  /**
   * 6: (iOS only) The app is in the background. Prompt the user that video capture cannot be
   * performed normally when the app is in the background.
   */
  LOCAL_VIDEO_STREAM_REASON_CAPTURE_INBACKGROUND = 6,
  /**
   * 7: (iOS only) The current app window is running in Slide Over, Split View, or Picture in Picture
   * mode, and another app is occupying the camera. Prompt the user that the app cannot capture video
   * properly when it is running in Slide Over, Split View, or Picture in Picture mode and another app
   * is occupying the camera.
   */
  LOCAL_VIDEO_STREAM_REASON_CAPTURE_MULTIPLE_FOREGROUND_APPS = 7,
  /**
   * 8: Fails to find a local video capture device. Remind the user to check whether the camera is
   * connected to the device properly or the camera is working properly, and then to rejoin the
   * channel.
   */
  LOCAL_VIDEO_STREAM_REASON_DEVICE_NOT_FOUND = 8,
  /**
   * 9: (macOS and Windows only) The video capture device currently in use is disconnected (such as
   * being unplugged).
   */
  LOCAL_VIDEO_STREAM_REASON_DEVICE_DISCONNECTED = 9,
  /**
   * 10: (macOS and Windows only) The SDK cannot find the video device in the video device list. Check
   * whether the ID of the video device is valid.
   */
  LOCAL_VIDEO_STREAM_REASON_DEVICE_INVALID_ID = 10,
  /**
   * 14: (Android only) Video capture is interrupted. Possible reasons include the following:
   * - The camera is being used by another app. Prompt the user to check if the camera is being used
   * by another app.
   * - The current app has been switched to the background. You can use foreground services to notify
   * the operating system and ensure that the app can still collect video when it switches to the
   * background.
   */
  LOCAL_VIDEO_STREAM_REASON_DEVICE_INTERRUPT = 14,
  /**
   * 15: (Android only) The video capture device encounters an error. Prompt the user to close and
   * restart the camera to restore functionality. If this operation does not solve the problem, check
   * if the camera has a hardware failure.
   */
  LOCAL_VIDEO_STREAM_REASON_DEVICE_FATAL_ERROR = 15,
  /**
   * 101: The current video capture device is unavailable due to excessive system pressure.
   */
  LOCAL_VIDEO_STREAM_REASON_DEVICE_SYSTEM_PRESSURE = 101,
  /**
   * 11: (macOS and Windows only) The shared window is minimized when you call the
   * `startScreenCaptureByWindowId` method to share a window. The SDK cannot share a minimized window.
   * Please prompt the user to unminimize the shared window.
   */
  LOCAL_VIDEO_STREAM_REASON_SCREEN_CAPTURE_WINDOW_MINIMIZED = 11,
  /**
   * 12: (macOS and Windows only) The error code indicates that a window shared by the window ID has
   * been closed or a full-screen window shared by the window ID has exited full-screen mode. After
   * exiting full-screen mode, remote users cannot see the shared window. To prevent remote users from
   * seeing a black screen, Agora recommends that you immediately stop screen sharing.
   * Common scenarios reporting this error code:
   * - The local user closes the shared window.
   * - The local user shows some slides in full-screen mode first, and then shares the windows of the
   * slides. After the user exits full-screen mode, the SDK reports this error code.
   * - The local user watches a web video or reads a web document in full-screen mode first, and then
   * shares the window of the web video or document. After the user exits full-screen mode, the SDK
   * reports this error code.
   */
  LOCAL_VIDEO_STREAM_REASON_SCREEN_CAPTURE_WINDOW_CLOSED = 12,
  /**
   * 13: (Windows only) The window being shared is overlapped by another window, so the overlapped
   * area is blacked out by the SDK during window sharing.
   */
  LOCAL_VIDEO_STREAM_REASON_SCREEN_CAPTURE_WINDOW_OCCLUDED = 13,
  /** 20: The local screen capture window is not supported. */
  LOCAL_VIDEO_STREAM_REASON_SCREEN_CAPTURE_WINDOW_NOT_SUPPORTED = 20,
  /**
   * 21: (Windows and Android only) The currently captured window has no data.
   */
  LOCAL_VIDEO_STREAM_REASON_SCREEN_CAPTURE_FAILURE = 21,
  /**
   * 22: (Windows and macOS only) No permission for screen capture.
   */
  LOCAL_VIDEO_STREAM_REASON_SCREEN_CAPTURE_NO_PERMISSION = 22,
  /**
   * 24: (Windows only) An unexpected error occurred during screen sharing (possibly due to window
   * blocking failure), resulting in decreased performance, but the screen sharing process itself was
   * not affected.
   * @note During screen sharing, if blocking a specific window fails due to device driver issues, the
   * SDK will report this event and automatically fall back to sharing the entire screen. If your use
   * case requires masking specific windows to protect privacy, we recommend listening for this event
   * and implementing additional privacy protection mechanisms when it is triggered.
   */
  LOCAL_VIDEO_STREAM_REASON_SCREEN_CAPTURE_AUTO_FALLBACK = 24,
  /**
   * 25: (Windows only) The window for the current screen capture is hidden and not visible on the
   * current screen.
   */
  LOCAL_VIDEO_STREAM_REASON_SCREEN_CAPTURE_WINDOW_HIDDEN = 25,
  /**
   * 26: (Windows only) The window for screen capture has been restored from hidden state.
   */
  LOCAL_VIDEO_STREAM_REASON_SCREEN_CAPTURE_WINDOW_RECOVER_FROM_HIDDEN = 26,
  /**
   * 27: (macOS and Windows only) The window for screen capture has been restored from the minimized
   * state.
   */
  LOCAL_VIDEO_STREAM_REASON_SCREEN_CAPTURE_WINDOW_RECOVER_FROM_MINIMIZED = 27,
  /**
   * 28: (Windows only) Screen capture has been paused. Common scenarios reporting this error code:
   * The current screen may have been switched to a secure desktop, such as a UAC dialog box or
   * Winlogon desktop.
   */
  LOCAL_VIDEO_STREAM_REASON_SCREEN_CAPTURE_PAUSED = 28,
  /**
   * 29: (Windows only) Screen capture has resumed from paused state.
   */
  LOCAL_VIDEO_STREAM_REASON_SCREEN_CAPTURE_RESUMED = 29,
  /**
   * 30: (Windows and macOS only) The displayer used for screen capture is disconnected. The current
   * screen sharing has been paused. Prompt the user to restart the screen sharing.
   */
  LOCAL_VIDEO_STREAM_REASON_SCREEN_CAPTURE_DISPLAY_DISCONNECTED = 30,
  /* 30: (HMOS only) ScreenCapture stopped by user */
  LOCAL_VIDEO_STREAM_REASON_SCREEN_CAPTURE_STOPPED_BY_USER = 31,
  /* 31: (HMOS only) ScreenCapture interrupted by other screen capture */
  LOCAL_VIDEO_STREAM_REASON_SCREEN_CAPTURE_INTERRUPTED_BY_OTHER = 32,
  /* 32: (HMOS only) ScreenCapture stopped by SIM call */
  LOCAL_VIDEO_STREAM_REASON_SCREEN_CAPTURE_STOPPED_BY_CALL = 33,
  /** 34: (Windows only) Some windows of the exclude window list failed to be excluded from the screen capture. */
  LOCAL_VIDEO_STREAM_REASON_SCREEN_CAPTURE_EXCLUDE_WINDOW_FAILED = 34,
};

/**
 * @brief Remote audio states.
 */
enum REMOTE_AUDIO_STATE {
  /**
   * 0: The local audio is in the initial state. The SDK reports this state in the case of
   * `REMOTE_AUDIO_REASON_LOCAL_MUTED`, `REMOTE_AUDIO_REASON_REMOTE_MUTED` or
   * `REMOTE_AUDIO_REASON_REMOTE_OFFLINE`.
   */
  REMOTE_AUDIO_STATE_STOPPED =
      0,  // Default state, audio is started or remote user disabled/muted audio stream
  /**
   * 1: The first remote audio packet is received.
   */
  REMOTE_AUDIO_STATE_STARTING = 1,  // The first audio frame packet has been received
  /**
   * 2: The remote audio stream is decoded and plays normally. The SDK reports this state in the case
   * of `REMOTE_AUDIO_REASON_NETWORK_RECOVERY`, `REMOTE_AUDIO_REASON_LOCAL_UNMUTED` or
   * `REMOTE_AUDIO_REASON_REMOTE_UNMUTED`.
   */
  REMOTE_AUDIO_STATE_DECODING =
      2,  // The first remote audio frame has been decoded or fronzen state ends
  /**
   * 3: The remote audio is frozen. The SDK reports this state in the case of
   * `REMOTE_AUDIO_REASON_NETWORK_CONGESTION`.
   */
  REMOTE_AUDIO_STATE_FROZEN = 3,  // Remote audio is frozen, probably due to network issue
  /**
   * 4: The remote audio fails to start. The SDK reports this state in the case of
   * `REMOTE_AUDIO_REASON_INTERNAL`.
   */
  REMOTE_AUDIO_STATE_FAILED = 4,  // Remote audio play failed
};

/**
 * @brief The reason for the remote audio state change.
 */
enum REMOTE_AUDIO_STATE_REASON {
  /**
   * 0: The SDK reports this reason when the audio state changes.
   */
  REMOTE_AUDIO_REASON_INTERNAL = 0,
  /**
   * 1: Network congestion.
   */
  REMOTE_AUDIO_REASON_NETWORK_CONGESTION = 1,
  /**
   * 2: Network recovery.
   */
  REMOTE_AUDIO_REASON_NETWORK_RECOVERY = 2,
  /**
   * 3: The local user stops receiving the remote audio stream or disables the audio module.
   */
  REMOTE_AUDIO_REASON_LOCAL_MUTED = 3,
  /**
   * 4: The local user resumes receiving the remote audio stream or enables the audio module.
   */
  REMOTE_AUDIO_REASON_LOCAL_UNMUTED = 4,
  /**
   * 5: The remote user stops sending the audio stream or disables the audio module.
   */
  REMOTE_AUDIO_REASON_REMOTE_MUTED = 5,
  /**
   * 6: The remote user resumes sending the audio stream or enables the audio module.
   */
  REMOTE_AUDIO_REASON_REMOTE_UNMUTED = 6,
  /**
   * 7: The remote user leaves the channel.
   */
  REMOTE_AUDIO_REASON_REMOTE_OFFLINE = 7,
  /**
   * 8: The local user does not receive any audio packet from remote user.
   */
  REMOTE_AUDIO_REASON_NO_PACKET_RECEIVE = 8,
  /**
   * 9: The local user receives remote audio packet but fails to play.
   */
  REMOTE_AUDIO_REASON_LOCAL_PLAY_FAILED = 9,
};

/**
 * @brief The state of the remote video stream.
 */
enum REMOTE_VIDEO_STATE {
  /**
   * 0: The remote video is in the initial state. The SDK reports this state in the case of
   * `REMOTE_VIDEO_STATE_REASON_LOCAL_MUTED`, `REMOTE_VIDEO_STATE_REASON_REMOTE_MUTED`, or
   * `REMOTE_VIDEO_STATE_REASON_REMOTE_OFFLINE`.
   */
  REMOTE_VIDEO_STATE_STOPPED = 0,
  /**
   * 1: The first remote video packet is received.
   */
  REMOTE_VIDEO_STATE_STARTING = 1,
  /**
   * 2: The remote video stream is decoded and plays normally. The SDK reports this state in the case
   * of `REMOTE_VIDEO_STATE_REASON_NETWORK_RECOVERY`, `REMOTE_VIDEO_STATE_REASON_LOCAL_UNMUTED`,
   * `REMOTE_VIDEO_STATE_REASON_REMOTE_UNMUTED`, or
   * `REMOTE_VIDEO_STATE_REASON_AUDIO_FALLBACK_RECOVERY`.
   */
  REMOTE_VIDEO_STATE_DECODING = 2,
  /**
   * 3: The remote video is frozen. The SDK reports this state in the case of
   * `REMOTE_VIDEO_STATE_REASON_NETWORK_CONGESTION` or `REMOTE_VIDEO_STATE_REASON_AUDIO_FALLBACK`.
   */
  REMOTE_VIDEO_STATE_FROZEN = 3,
  /**
   * 4: The remote video fails to start. The SDK reports this state in the case of
   * `REMOTE_VIDEO_STATE_REASON_INTERNAL`.
   */
  REMOTE_VIDEO_STATE_FAILED = 4,
};
/**
 * @brief The reason for the remote video state change.
 */
enum REMOTE_VIDEO_STATE_REASON {
  /**
   * 0: The SDK reports this reason when the video state changes.
   */
  REMOTE_VIDEO_STATE_REASON_INTERNAL = 0,
  /**
   * 1: Network congestion.
   */
  REMOTE_VIDEO_STATE_REASON_NETWORK_CONGESTION = 1,
  /**
   * 2: Network is recovered.
   */
  REMOTE_VIDEO_STATE_REASON_NETWORK_RECOVERY = 2,
  /**
   * 3: The local user stops receiving the remote video stream or disables the video module.
   */
  REMOTE_VIDEO_STATE_REASON_LOCAL_MUTED = 3,
  /**
   * 4: The local user resumes receiving the remote video stream or enables the video module.
   */
  REMOTE_VIDEO_STATE_REASON_LOCAL_UNMUTED = 4,
  /**
   * 5: The remote user stops sending the video stream or disables the video module.
   */
  REMOTE_VIDEO_STATE_REASON_REMOTE_MUTED = 5,
  /**
   * 6: The remote user resumes sending the video stream or enables the video module.
   */
  REMOTE_VIDEO_STATE_REASON_REMOTE_UNMUTED = 6,
  /**
   * 7: The remote user leaves the channel.
   */
  REMOTE_VIDEO_STATE_REASON_REMOTE_OFFLINE = 7,
  /**
   * 8: The remote audio-and-video stream falls back to the audio-only stream due to poor network
   * conditions.
   */
  REMOTE_VIDEO_STATE_REASON_AUDIO_FALLBACK = 8,
  /**
   * 9: The remote audio-only stream switches back to the audio-and-video stream after the network
   * conditions improve.
   */
  REMOTE_VIDEO_STATE_REASON_AUDIO_FALLBACK_RECOVERY = 9,
  /** (Internal use only) 10: The remote video stream type change to low stream type
   */
  REMOTE_VIDEO_STATE_REASON_VIDEO_STREAM_TYPE_CHANGE_TO_LOW = 10,
  /** (Internal use only)  11: The remote video stream type change to high stream type
   */
  REMOTE_VIDEO_STATE_REASON_VIDEO_STREAM_TYPE_CHANGE_TO_HIGH = 11,
  /**
   * 12: (iOS only) The remote user's app has switched to the background.
   */
  REMOTE_VIDEO_STATE_REASON_SDK_IN_BACKGROUND = 12,

  /**
   * 13: The local video decoder does not support decoding the remote video stream.
   */
  REMOTE_VIDEO_STATE_REASON_CODEC_NOT_SUPPORT = 13,

};

/**
 * The remote user state information.
 */
enum REMOTE_USER_STATE {
  /**
   * The remote user has muted the audio.
   */
  USER_STATE_MUTE_AUDIO = (1 << 0),
  /**
   * The remote user has muted the video.
   */
  USER_STATE_MUTE_VIDEO = (1 << 1),
  /**
   * The remote user has enabled the video, which includes video capturing and encoding.
   */
  USER_STATE_ENABLE_VIDEO = (1 << 4),
  /**
   * The remote user has enabled the local video capturing.
   */
  USER_STATE_ENABLE_LOCAL_VIDEO = (1 << 8),
};

/**
 * The definition of the VideoTrackInfo struct, which contains information of
 * the video track.
 */
struct VideoTrackInfo {
  VideoTrackInfo()
      : isLocal(false),
        ownerUid(0),
        trackId(0),
        channelId(OPTIONAL_NULLPTR),
        codecType(VIDEO_CODEC_H265),
        encodedFrameOnly(false),
        sourceType(VIDEO_SOURCE_CAMERA_PRIMARY),
        observationPosition(agora::media::base::POSITION_POST_CAPTURER) {}
  /**
   * Whether the video track is local or remote.
   * - true: The video track is local.
   * - false: The video track is remote.
   */
  bool isLocal;
  /**
   * ID of the user who publishes the video track.
   */
  uid_t ownerUid;
  /**
   * ID of the video track.
   */
  track_id_t trackId;
  /**
   * The channel ID of the video track.
   */
  const char* channelId;
  /**
   * The video codec type: #VIDEO_CODEC_TYPE.
   */
  VIDEO_CODEC_TYPE codecType;
  /**
   * Whether the video track contains encoded video frame only.
   * - true: The video track contains encoded video frame only.
   * - false: The video track does not contain encoded video frame only.
   */
  bool encodedFrameOnly;
  /**
   * The video source type: #VIDEO_SOURCE_TYPE
   */
  VIDEO_SOURCE_TYPE sourceType;
  /**
   * the frame position for the video observer: #VIDEO_MODULE_POSITION
   */
  uint32_t observationPosition;
};

/**
 * The downscale level of the remote video stream . The higher the downscale level, the more the
 * video downscales.
 */
enum REMOTE_VIDEO_DOWNSCALE_LEVEL {
  /**
   * No downscale.
   */
  REMOTE_VIDEO_DOWNSCALE_LEVEL_NONE,
  /**
   * Downscale level 1.
   */
  REMOTE_VIDEO_DOWNSCALE_LEVEL_1,
  /**
   * Downscale level 2.
   */
  REMOTE_VIDEO_DOWNSCALE_LEVEL_2,
  /**
   * Downscale level 3.
   */
  REMOTE_VIDEO_DOWNSCALE_LEVEL_3,
  /**
   * Downscale level 4.
   */
  REMOTE_VIDEO_DOWNSCALE_LEVEL_4,
};

/**
 * @brief The volume information of users.
 */
struct AudioVolumeInfo {
  /**
   * The user ID.
   * - In the local user's callback, `uid` is 0.
   * - In the remote users' callback, `uid` is the user ID of a remote user whose instantaneous volume
   * is the highest.
   */
  uid_t uid;
  /**
   * The volume of the user. The value ranges between 0 (the lowest volume) and 255 (the highest
   * volume). If the local user enables audio capturing and calls `muteLocalAudioStream` and set it as
   * `true` to mute, the value of `volume` indicates the volume of locally captured audio signal. If
   * the user calls `startAudioMixing(const char* filePath, bool loopback, int cycle, int startPos)`,
   * the value of `volume` indicates the volume after audio
   * mixing.
   */
  unsigned int volume;  // [0,255]
  /**
   * Voice activity status of the local user.
   * - 0: The local user is not speaking.
   * - 1: The local user is speaking.
   * @note
   * - The `vad` parameter does not report the voice activity status of remote users. In a remote
   * user's callback, the value of `vad` is always 1.
   * - To use this parameter, you must set `reportVad` to `true` when calling
   * `enableAudioVolumeIndication`.
   */
  unsigned int vad;
  /**
   * The voice pitch of the local user. The value ranges between 0.0 and 4000.0.
   * @note The `voicePitch` parameter does not report the voice pitch of remote users. In the remote
   * users' callback, the value of `voicePitch` is always 0.0.
   */
  double voicePitch;

  AudioVolumeInfo() : uid(0), volume(0), vad(0), voicePitch(0.0) {}
};

/**
 * @brief The audio device information.
 *
 * @note This class is for Android only.
 *
 */
struct DeviceInfo {
  /**
   * Whether the audio device supports ultra-low-latency capture and playback:
   * - `true`: The device supports ultra-low-latency capture and playback.
   * - `false`: The device does not support ultra-low-latency capture and playback.
   */
  bool isLowLatencyAudioSupported;

  DeviceInfo() : isLowLatencyAudioSupported(false) {}
};

/**
 * The definition of the IPacketObserver struct.
 */
class IPacketObserver {
 public:
  virtual ~IPacketObserver() {}
  /**
   * @brief Configurations for the `Packet` instance.
   */
  struct Packet {
    /**
     * The buffer address of the sent or received data.
     * @note Agora recommends setting `buffer` to a value larger than 2048 bytes. Otherwise, you may
     * encounter undefined behaviors (such as crashes).
     */
    const unsigned char* buffer;
    /**
     * The buffer size of the sent or received data.
     */
    unsigned int size;

    Packet() : buffer(OPTIONAL_NULLPTR), size(0) {}
  };
  /**
   * @brief Occurs when the local user sends an audio packet.
   *
   * @param packet The sent audio packet, see `Packet`.
   *
   * @return
   * - `true`: The audio packet is sent successfully.
   * - `false`: The audio packet is discarded.
   */
  virtual bool onSendAudioPacket(Packet& packet) = 0;
  /**
   * @brief Occurs when the local user sends a video packet.
   *
   * @param packet The sent video packet, see `Packet`.
   *
   * @return
   * - `true`: The video packet is sent successfully.
   * - `false`: The video packet is discarded.
   */
  virtual bool onSendVideoPacket(Packet& packet) = 0;
  /**
   * @brief Occurs when the local user receives an audio packet.
   *
   * @param packet The received audio packet, see `Packet`.
   *
   * @return
   * - `true`: The audio packet is received successfully.
   * - `false`: The audio packet is discarded.
   */
  virtual bool onReceiveAudioPacket(Packet& packet) = 0;
  /**
   * @brief Occurs when the local user receives a video packet.
   *
   * @param packet The received video packet, see `Packet`.
   *
   * @return
   * - `true`: The video packet is received successfully.
   * - `false`: The video packet is discarded.
   */
  virtual bool onReceiveVideoPacket(Packet& packet) = 0;
};

/**
 * @brief The audio sampling rate of the stream to be pushed to the CDN.
 */
enum AUDIO_SAMPLE_RATE_TYPE {
  /**
   * 32000: 32 kHz
   */
  AUDIO_SAMPLE_RATE_32000 = 32000,
  /**
   * 44100: 44.1 kHz
   */
  AUDIO_SAMPLE_RATE_44100 = 44100,
  /**
   * 48000: (Default) 48 kHz
   */
  AUDIO_SAMPLE_RATE_48000 = 48000,
};
/**
 * @brief The codec type of the output video.
 */
enum VIDEO_CODEC_TYPE_FOR_STREAM {
  /**
   * 1: (Default) H.264.
   */
  VIDEO_CODEC_H264_FOR_STREAM = 1,
  /**
   * 2: H.265.
   */
  VIDEO_CODEC_H265_FOR_STREAM = 2,
};

/**
 * @brief Video codec profile types.
 */
enum VIDEO_CODEC_PROFILE_TYPE {
  /**
   * 66: Baseline video codec profile; generally used for video calls on mobile phones.
   */
  VIDEO_CODEC_PROFILE_BASELINE = 66,
  /**
   * 77: Main video codec profile; generally used in mainstream electronics such as MP4 players,
   * portable video players, PSP, and iPads.
   */
  VIDEO_CODEC_PROFILE_MAIN = 77,
  /**
   * 100: (Default) High video codec profile; generally used in high-resolution live streaming or
   * television.
   */
  VIDEO_CODEC_PROFILE_HIGH = 100,
};

/**
 * @brief Self-defined audio codec profile.
 */
enum AUDIO_CODEC_PROFILE_TYPE {
  /**
   * 0: (Default) LC-AAC.
   */
  AUDIO_CODEC_PROFILE_LC_AAC = 0,
  /**
   * 1: HE-AAC.
   */
  AUDIO_CODEC_PROFILE_HE_AAC = 1,
  /**
   * 2: HE-AAC v2.
   */
  AUDIO_CODEC_PROFILE_HE_AAC_V2 = 2,
};

/**
 * @brief Local audio statistics.
 */
struct LocalAudioStats {
  /**
   * The number of audio channels.
   */
  int numChannels;
  /**
   * The sampling rate (Hz) of sending the local user's audio stream.
   */
  int sentSampleRate;
  /**
   * The average bitrate (Kbps) of sending the local user's audio stream.
   */
  int sentBitrate;
  /**
   * The internal payload codec.
   */
  int internalCodec;
  /**
   * The packet loss rate (%) from the local client to the Agora server before applying the
   * anti-packet loss strategies.
   */
  unsigned short txPacketLossRate;
  /**
   * The audio device module delay (ms) when playing or recording audio.
   */
  int audioDeviceDelay;
  /**
   * The playout delay of the device
   */
  int audioPlayoutDelay;
  /**
   * The ear monitor delay (ms), which is the delay from microphone input to headphone output.
   */
  int earMonitorDelay;
  /**
   * Acoustic echo cancellation (AEC) module estimated delay (ms), which is the signal delay between
   * when audio is played locally before being locally captured.
   */
  int aecEstimatedDelay;
};

/**
 * @brief States of the Media Push.
 */
enum RTMP_STREAM_PUBLISH_STATE {
  /**
   * 0: The Media Push has not started or has ended.
   */
  RTMP_STREAM_PUBLISH_STATE_IDLE = 0,
  /**
   * 1: The streaming server and CDN server are being connected.
   */
  RTMP_STREAM_PUBLISH_STATE_CONNECTING = 1,
  /**
   * 2: The RTMP or RTMPS streaming publishes. The SDK successfully publishes the RTMP or RTMPS
   * streaming and returns this state.
   */
  RTMP_STREAM_PUBLISH_STATE_RUNNING = 2,
  /**
   * 3: The RTMP or RTMPS streaming is recovering. When exceptions occur to the CDN, or the streaming
   * is interrupted, the SDK tries to resume RTMP or RTMPS streaming and returns this state.
   * - If the SDK successfully resumes the streaming, RTMP_STREAM_PUBLISH_STATE_RUNNING (2) returns.
   * - If the streaming does not resume within 60 seconds or server errors occur,
   * RTMP_STREAM_PUBLISH_STATE_FAILURE (4) returns. If you feel that 60 seconds is too long, you can
   * also actively try to reconnect.
   */
  RTMP_STREAM_PUBLISH_STATE_RECOVERING = 3,
  /**
   * 4: The RTMP or RTMPS streaming fails. After a failure, you can troubleshoot the cause of the
   * error through the returned error code.
   */
  RTMP_STREAM_PUBLISH_STATE_FAILURE = 4,
  /**
   * 5: The SDK is disconnecting from the Agora streaming server and CDN. When you call
   * `stopRtmpStream` to stop the Media Push normally, the SDK reports the Media Push state as
   * `RTMP_STREAM_PUBLISH_STATE_DISCONNECTING` and `RTMP_STREAM_PUBLISH_STATE_IDLE` in sequence.
   */
  RTMP_STREAM_PUBLISH_STATE_DISCONNECTING = 5,
};

/**
 * @brief Reasons for changes in the status of RTMP or RTMPS streaming.
 */
enum RTMP_STREAM_PUBLISH_REASON {
  /**
   * 0: The RTMP or RTMPS streaming has not started or has ended.
   */
  RTMP_STREAM_PUBLISH_REASON_OK = 0,
  /**
   * 1: Invalid argument used. Check the parameter setting.
   */
  RTMP_STREAM_PUBLISH_REASON_INVALID_ARGUMENT = 1,
  /**
   * 2: The RTMP or RTMPS streaming is encrypted and cannot be published.
   */
  RTMP_STREAM_PUBLISH_REASON_ENCRYPTED_STREAM_NOT_ALLOWED = 2,
  /**
   * 3: Timeout for the RTMP or RTMPS streaming.
   */
  RTMP_STREAM_PUBLISH_REASON_CONNECTION_TIMEOUT = 3,
  /**
   * 4: An error occurs in Agora's streaming server.
   */
  RTMP_STREAM_PUBLISH_REASON_INTERNAL_SERVER_ERROR = 4,
  /**
   * 5: An error occurs in the CDN server.
   */
  RTMP_STREAM_PUBLISH_REASON_RTMP_SERVER_ERROR = 5,
  /**
   * 6: The RTMP or RTMPS streaming publishes too frequently.
   */
  RTMP_STREAM_PUBLISH_REASON_TOO_OFTEN = 6,
  /**
   * 7: The host publishes more than 10 URLs. Delete the unnecessary URLs before adding new ones.
   */
  RTMP_STREAM_PUBLISH_REASON_REACH_LIMIT = 7,
  /**
   * 8: The host manipulates other hosts' URLs. For example, the host updates or stops other hosts'
   * streams. Check your app logic.
   */
  RTMP_STREAM_PUBLISH_REASON_NOT_AUTHORIZED = 8,
  /**
   * 9: Agora's server fails to find the RTMP or RTMPS streaming.
   */
  RTMP_STREAM_PUBLISH_REASON_STREAM_NOT_FOUND = 9,
  /**
   * 10: The format of the RTMP or RTMPS streaming URL is not supported. Check whether the URL format
   * is correct.
   */
  RTMP_STREAM_PUBLISH_REASON_FORMAT_NOT_SUPPORTED = 10,
  /**
   * 11: The user role is not host, so the user cannot use the CDN live streaming function. Check your
   * application code logic.
   */
  RTMP_STREAM_PUBLISH_REASON_NOT_BROADCASTER =
      11,  // Note: match to ERR_PUBLISH_STREAM_NOT_BROADCASTER in AgoraBase.h
  /**
   * 13: The `updateRtmpTranscoding` method is called to update the transcoding configuration in a
   * scenario where there is streaming without transcoding. Check your application code logic.
   */
  RTMP_STREAM_PUBLISH_REASON_TRANSCODING_NO_MIX_STREAM =
      13,  // Note: match to ERR_PUBLISH_STREAM_TRANSCODING_NO_MIX_STREAM in AgoraBase.h
  /**
   * 14: Errors occurred in the host's network.
   */
  RTMP_STREAM_PUBLISH_REASON_NET_DOWN = 14,  // Note: match to ERR_NET_DOWN in AgoraBase.h
  /**
   * 15: Your App ID does not have permission to use the CDN live streaming function.
   */
  RTMP_STREAM_PUBLISH_REASON_INVALID_APPID =
      15,  // Note: match to ERR_PUBLISH_STREAM_APPID_INVALID in AgoraBase.h
  /**
   * 16: Your project does not have permission to use streaming services.
   */
  RTMP_STREAM_PUBLISH_REASON_INVALID_PRIVILEGE = 16,
  /**
   * 100: The streaming has been stopped normally. After you stop the Media Push, the SDK returns this
   * value.
   */
  RTMP_STREAM_UNPUBLISH_REASON_OK = 100,
};

/**
 * @brief Events during the Media Push.
 */
enum RTMP_STREAMING_EVENT {
  /**
   * 1: An error occurs when you add a background image or a watermark image in the Media Push.
   */
  RTMP_STREAMING_EVENT_FAILED_LOAD_IMAGE = 1,
  /**
   * 2: The streaming URL is already being used for Media Push. If you want to start new streaming,
   * use a new streaming URL.
   */
  RTMP_STREAMING_EVENT_URL_ALREADY_IN_USE = 2,
  /**
   * 3: The feature is not supported.
   */
  RTMP_STREAMING_EVENT_ADVANCED_FEATURE_NOT_SUPPORT = 3,
  /**
   * 4: Reserved.
   */
  RTMP_STREAMING_EVENT_REQUEST_TOO_OFTEN = 4,
};

/**
 * @brief Image properties.
 *
 * @details
 * This class sets the properties of the watermark and background images in the live video.
 *
 */
typedef struct RtcImage {
  /**
   * The HTTP/HTTPS URL address of the image in the live video. The maximum length of this parameter
   * is 1024 bytes.
   */
  const char* url;
  /**
   * The x-coordinate (px) of the image on the video frame (taking the upper left corner of the video
   * frame as the origin).
   */
  int x;
  /**
   * The y-coordinate (px) of the image on the video frame (taking the upper left corner of the video
   * frame as the origin).
   */
  int y;
  /**
   * The width (px) of the image on the video frame.
   */
  int width;
  /**
   * The height (px) of the image on the video frame.
   */
  int height;
  /**
   * The layer index of the watermark or background image. When you use the watermark array to add a
   * watermark or multiple watermarks, you must pass a value to `zOrder` in the range [1,255];
   * otherwise, the SDK reports an error. In other cases, `zOrder` can optionally be passed in the
   * range [0,255], with 0 being the default value. 0 means the bottom layer and 255 means the top
   * layer.
   */
  int zOrder;
  /**
   * The transparency of the watermark or background image. The range of the value is [0.0,1.0]:
   * - 0.0: Completely transparent.
   * - 1.0: (Default) Opaque.
   */
  double alpha;

  RtcImage() : url(OPTIONAL_NULLPTR), x(0), y(0), width(0), height(0), zOrder(0), alpha(1.0) {}
} RtcImage;
/**
 * @brief The configuration for advanced features of the RTMP or RTMPS streaming with transcoding.
 *
 * @details
 * If you want to enable the advanced features of streaming with transcoding, contact
 * `support@agora.io`.
 *
 */
struct LiveStreamAdvancedFeature {
  LiveStreamAdvancedFeature() : featureName(OPTIONAL_NULLPTR), opened(false) {}
  LiveStreamAdvancedFeature(const char* feat_name, bool open)
      : featureName(feat_name), opened(open) {}
  /** The advanced feature for high-quality video with a lower bitrate. */
  // static const char* LBHQ = "lbhq";
  /** The advanced feature for the optimized video encoder. */
  // static const char* VEO = "veo";

  /**
   * The feature names, including LBHQ (high-quality video with a lower bitrate) and VEO (optimized
   * video encoder).
   */
  const char* featureName;

  /**
   * Whether to enable the advanced features of streaming with transcoding:
   * - `true`: Enable the advanced features.
   * - `false`: (Default) Do not enable the advanced features.
   */
  bool opened;
};

/**
 * @brief Connection states.
 */
enum CONNECTION_STATE_TYPE {
  /**
   * 1: The SDK is disconnected from the Agora edge server. The state indicates the SDK is in one of
   * the following phases:
   * - Theinitial state before calling the `joinChannel(const char* token, const char* channelId,
   * uid_t uid, const ChannelMediaOptions& options)` method.
   * - The app calls the `leaveChannel()` method.
   */
  CONNECTION_STATE_DISCONNECTED = 1,
  /**
   * 2: The SDK is connecting to the Agora edge server. This state indicates that the SDK is
   * establishing a connection with the specified channel after the app calls `joinChannel(const char*
   * token, const char* channelId, uid_t uid, const ChannelMediaOptions& options)`.
   * - If the SDK successfully joins the channel, it triggers the `onConnectionStateChanged` callback
   * and the connection state switches to CONNECTION_STATE_CONNECTED.
   * - After the connection is established, the SDK also initializes the media and triggers
   * `onJoinChannelSuccess` when everything is ready.
   */
  CONNECTION_STATE_CONNECTING = 2,
  /**
   * 3: The SDK is connected to the Agora edge server. This state also indicates that the user has
   * joined a channel and can now publish or subscribe to a media stream in the channel. If the
   * connection to the channel is lost because, for example, if the network is down or switched, the
   * SDK automatically tries to reconnect and triggers `onConnectionStateChanged` callback, notifying
   * that the current network state becomes CONNECTION_STATE_RECONNECTING.
   */
  CONNECTION_STATE_CONNECTED = 3,
  /**
   * 4: The SDK keeps reconnecting to the Agora edge server. The SDK keeps rejoining the channel after
   * being disconnected from a joined channel because of network issues.
   * - If the SDK cannot rejoin the channel within 10 seconds, it triggers `onConnectionLost`, stays
   * in the CONNECTION_STATE_RECONNECTING state, and keeps rejoining the channel.
   * - If the SDK fails to rejoin the channel 20 minutes after being disconnected from the Agora edge
   * server, the SDK triggers the `onConnectionStateChanged` callback, switches to the
   * CONNECTION_STATE_FAILED state, and stops rejoining the channel.
   */
  CONNECTION_STATE_RECONNECTING = 4,
  /**
   * 5: The SDK fails to connect to the Agora edge server or join the channel. This state indicates
   * that the SDK stops trying to rejoin the channel. You must call `leaveChannel()` to leave the
   * channel.
   * - You can call `joinChannel(const char* token, const char* channelId, uid_t uid, const
   * ChannelMediaOptions& options)` to rejoin the channel.
   * - If the SDK is banned from joining the channel by the Agora edge server through the RESTful API,
   * the SDK triggers the `onConnectionStateChanged` callback.
   */
  CONNECTION_STATE_FAILED = 5,
};

/**
 * @brief Transcoding configurations of each host.
 */
struct TranscodingUser {
  /**
   * The user ID of the host.
   */
  uid_t uid;
  /**
   * The x coordinate (pixel) of the host's video on the output video frame (taking the upper left
   * corner of the video frame as the origin). The value range is [0, width], where width is the
   * `width` set in `LiveTranscoding`.
   */
  int x;
  /**
   * The y coordinate (pixel) of the host's video on the output video frame (taking the upper left
   * corner of the video frame as the origin). The value range is [0, height], where height is the
   * `height` set in `LiveTranscoding`.
   */
  int y;
  /**
   * The width (pixel) of the host's video.
   */
  int width;
  /**
   * The height (pixel) of the host's video.
   */
  int height;
  /**
   * The layer index number of the host's video. The value range is [0, 100].
   * - 0: (Default) The host's video is the bottom layer.
   * - 100: The host's video is the top layer.
   * @note
   * - If the value is less than 0 or greater than 100, `ERR_INVALID_ARGUMENT` error is returned.
   * - Setting zOrder to 0 is supported.
   */
  int zOrder;
  /**
   * The transparency of the host's video. The value range is [0.0,1.0].
   * - 0.0: Completely transparent.
   * - 1.0: (Default) Opaque.
   */
  double alpha;
  /**
   * The audio channel used by the host's audio in the output audio. The default value is 0, and the
   * value range is [0, 5].
   * - `0`: (Recommended) The defaut setting, which supports dual channels at most and depends on the
   * upstream of the host.
   * - `1`: The host's audio uses the FL audio channel. If the host's upstream uses multiple audio
   * channels, the Agora server mixes them into mono first.
   * - `2`: The host's audio uses the FC audio channel. If the host's upstream uses multiple audio
   * channels, the Agora server mixes them into mono first.
   * - `3`: The host's audio uses the FR audio channel. If the host's upstream uses multiple audio
   * channels, the Agora server mixes them into mono first.
   * - `4`: The host's audio uses the BL audio channel. If the host's upstream uses multiple audio
   * channels, the Agora server mixes them into mono first.
   * - `5`: The host's audio uses the BR audio channel. If the host's upstream uses multiple audio
   * channels, the Agora server mixes them into mono first.
   * - `0xFF` or a value greater than `5`: The host's audio is muted, and the Agora server removes the
   * host's audio.
   * @note If the value is not `0`, a special player is required.
   */
  int audioChannel;

  TranscodingUser()
      : uid(0), x(0), y(0), width(0), height(0), zOrder(0), alpha(1.0), audioChannel(0) {}
};

/**
 * @brief Transcoding configurations for Media Push.
 */
struct LiveTranscoding {
  /**
   * The width of the video in pixels. The default value is 360.
   * - When pushing video streams to the CDN, the value range of `width` is [64,1920]. If the value is
   * less than 64, Agora server automatically adjusts it to 64; if the value is greater than 1920,
   * Agora server automatically adjusts it to 1920.
   * - When pushing audio streams to the CDN, set `width` and `height` as 0.
   */
  int width;
  /**
   * The height of the video in pixels. The default value is 640.
   * - When pushing video streams to the CDN, the value range of` height` is [64,1080]. If the value
   * is less than 64, Agora server automatically adjusts it to 64; if the value is greater than 1080,
   * Agora server automatically adjusts it to 1080.
   * - When pushing audio streams to the CDN, set `width` and `height` as 0.
   */
  int height;
  /**
   * The encoding bitrate (Kbps) of the video. This parameter does not need to be set; keeping the
   * default value `STANDARD_BITRATE` is sufficient. The SDK automatically matches the most suitable
   * bitrate based on the video resolution and frame rate you have set. For the correspondence between
   * video resolution and frame rate, see `Video profile`.
   */
  int videoBitrate;
  /**
   * Frame rate (fps) of the output video stream set for Media Push. The default value is 15. The
   * value range is (0,30].
   * @note The Agora server adjusts any value over 30 to 30.
   */
  int videoFramerate;

  /**
   * Deprecated
   * This member is deprecated.
   * Latency mode:
   * - `true`: Low latency with unassured quality.
   * - `false`: (Default) High latency with assured quality.
   */
  bool lowLatency;

  /**
   * GOP (Group of Pictures) in fps of the video frames for Media Push. The default value is 30.
   */
  int videoGop;
  /**
   * Video codec profile type for Media Push. Set it as 66, 77, or 100 (default). See
   * `VIDEO_CODEC_PROFILE_TYPE` for details.
   * @note If you set this parameter to any other value, Agora adjusts it to the default value.
   */
  VIDEO_CODEC_PROFILE_TYPE videoCodecProfile;
  /**
   * The background color in RGB hex value. Value only. Do not include a preceeding #. For example,
   * 0xFFB6C1 (light pink). The default value is 0x000000 (black).
   */
  unsigned int backgroundColor;
  /**
   * Video codec profile types for Media Push. See `VIDEO_CODEC_TYPE_FOR_STREAM`.
   */
  VIDEO_CODEC_TYPE_FOR_STREAM videoCodecType;
  /**
   * The number of users in the Media Push. The value range is [0,17].
   */
  unsigned int userCount;
  /**
   * Manages the user layout configuration in the Media Push. Agora supports a maximum of 17
   * transcoding users in a Media Push channel. See `TranscodingUser`.
   */
  TranscodingUser* transcodingUsers;
  /**
   * Reserved property. Extra user-defined information to send SEI for the H.264/H.265 video stream to
   * the CDN live client. Maximum length: 4096 bytes. For more information on SEI, see SEI-related
   * questions.
   */
  const char* transcodingExtraInfo;

  /**
   * Deprecated
   * Obsolete and not recommended for use.
   * The metadata sent to the CDN client.
   */
  const char* metadata;
  /**
   * The watermark on the live video. The image format needs to be PNG. See `RtcImage`.
   * You can add one watermark, or add multiple watermarks using an array. This parameter is used with
   * `watermarkCount`.
   */
  RtcImage* watermark;
  /**
   * The number of watermarks on the live video. The total number of watermarks and background images
   * can range from 0 to 10. This parameter is used with `watermark`.
   */
  unsigned int watermarkCount;

  /**
   * The number of background images on the live video. The image format needs to be PNG. See
   * `RtcImage`.
   * You can add a background image or use an array to add multiple background images. This parameter
   * is used with `backgroundImageCount`.
   */
  RtcImage* backgroundImage;
  /**
   * The number of background images on the live video. The total number of watermarks and background
   * images can range from 0 to 10. This parameter is used with `backgroundImage`.
   */
  unsigned int backgroundImageCount;

  /**
   * The audio sampling rate (Hz) of the output media stream. See `AUDIO_SAMPLE_RATE_TYPE`.
   */
  AUDIO_SAMPLE_RATE_TYPE audioSampleRate;
  /**
   * Bitrate (Kbps) of the audio output stream for Media Push. The default value is 48, and the
   * highest value is 128.
   */
  int audioBitrate;
  /**
   * The number of audio channels for Media Push. Agora recommends choosing 1 (mono), or 2 (stereo)
   * audio channels. Special players are required if you choose 3, 4, or 5.
   * - 1: (Default) Mono.
   * - 2: Stereo.
   * - 3: Three audio channels.
   * - 4: Four audio channels.
   * - 5: Five audio channels.
   */
  int audioChannels;
  /**
   * Audio codec profile type for Media Push. See `AUDIO_CODEC_PROFILE_TYPE`.
   */
  AUDIO_CODEC_PROFILE_TYPE audioCodecProfile;
  /**
   * Advanced features of the Media Push with transcoding. See `LiveStreamAdvancedFeature`.
   */
  LiveStreamAdvancedFeature* advancedFeatures;

  /**
   * The number of enabled advanced features. The default value is 0.
   */
  unsigned int advancedFeatureCount;

  LiveTranscoding()
      : width(360),
        height(640),
        videoBitrate(400),
        videoFramerate(15),
        lowLatency(false),
        videoGop(30),
        videoCodecProfile(VIDEO_CODEC_PROFILE_HIGH),
        backgroundColor(0x000000),
        videoCodecType(VIDEO_CODEC_H264_FOR_STREAM),
        userCount(0),
        transcodingUsers(OPTIONAL_NULLPTR),
        transcodingExtraInfo(OPTIONAL_NULLPTR),
        metadata(OPTIONAL_NULLPTR),
        watermark(OPTIONAL_NULLPTR),
        watermarkCount(0),
        backgroundImage(OPTIONAL_NULLPTR),
        backgroundImageCount(0),
        audioSampleRate(AUDIO_SAMPLE_RATE_48000),
        audioBitrate(48),
        audioChannels(1),
        audioCodecProfile(AUDIO_CODEC_PROFILE_LC_AAC),
        advancedFeatures(OPTIONAL_NULLPTR),
        advancedFeatureCount(0) {}
};

/**
 * @brief The video streams for local video mixing.
 */
struct TranscodingVideoStream {
  /**
   * The video source type for local video mixing. See `VIDEO_SOURCE_TYPE`.
   */
  VIDEO_SOURCE_TYPE sourceType;
  /**
   * The user ID of the remote user.
   * @note Use this parameter only when the source type is `VIDEO_SOURCE_REMOTE` for local video
   * mixing.
   */
  uid_t remoteUserUid;
  /**
   * The file path of local images.
   * Examples:
   * - Windows: `C:\\Users\\{username}\\Pictures\\image.png`
   * @note Use this parameter only when the source type is the image for local video mixing.
   */
  const char* imageUrl;
  /**
   * (Optional) Media player ID. Use the parameter only when you set `sourceType` to
   * `VIDEO_SOURCE_MEDIA_PLAYER`.
   */
  int mediaPlayerId;
  /**
   * The relative lateral displacement of the top left corner of the video for local video mixing to
   * the origin (the top left corner of the canvas).
   */
  int x;
  /**
   * The relative longitudinal displacement of the top left corner of the captured video to the origin
   * (the top left corner of the canvas).
   */
  int y;
  /**
   * The width (px) of the video for local video mixing on the canvas.
   */
  int width;
  /**
   * The height (px) of the video for local video mixing on the canvas.
   */
  int height;
  /**
   * The number of the layer to which the video for the local video mixing belongs. The value range is
   * [0, 100].
   * - 0: (Default) The layer is at the bottom.
   * - 100: The layer is at the top.
   */
  int zOrder;
  /**
   * The transparency of the video for local video mixing. The value range is [0.0, 1.0]. 0.0
   * indicates that the video is completely transparent, and 1.0 indicates that it is opaque.
   */
  double alpha;
  /**
   * Whether to mirror the video for the local video mixing.
   * - `true`: Mirror the video for the local video mixing.
   * - `false`: (Default) Do not mirror the video for the local video mixing.
   * @note This parameter only takes effect on video source types that are cameras.
   */
  bool mirror;

  TranscodingVideoStream()
      : sourceType(VIDEO_SOURCE_CAMERA_PRIMARY),
        remoteUserUid(0),
        imageUrl(OPTIONAL_NULLPTR),
        x(0),
        y(0),
        width(0),
        height(0),
        zOrder(0),
        alpha(1.0),
        mirror(false) {}
};

/**
 * @brief The configuration of the video mixing on the local client.
 */
struct LocalTranscoderConfiguration {
  /**
   * The number of the video streams for the video mixing on the local client.
   */
  unsigned int streamCount;
  /**
   * The video streams for local video mixing. See `TranscodingVideoStream`.
   */
  TranscodingVideoStream* videoInputStreams;
  /**
   * The encoding configuration of the mixed video stream after the local video mixing. See
   * `VideoEncoderConfiguration`.
   */
  VideoEncoderConfiguration videoOutputConfiguration;
  /**
   * Whether to use the timestamp when the primary camera captures the video frame as the timestamp
   * of the mixed video frame.
   * - true: (Default) Use the timestamp of the captured video frame as the timestamp of the mixed
   * video frame.
   * - false: Do not use the timestamp of the captured video frame as the timestamp of the mixed
   * video frame. Instead, use the timestamp when the mixed video frame is constructed.
   */
  bool syncWithPrimaryCamera;

  LocalTranscoderConfiguration()
      : streamCount(0),
        videoInputStreams(OPTIONAL_NULLPTR),
        videoOutputConfiguration(),
        syncWithPrimaryCamera(true) {}
};

/**
 * @brief The error code of the local video mixing failure.
 */
enum VIDEO_TRANSCODER_ERROR {
  /**
   * 1: The selected video source has not started video capture. You need to create a video track for
   * it and start video capture.
   */
  VT_ERR_VIDEO_SOURCE_NOT_READY = 1,
  /**
   * 2: The video source type is invalid. You need to re-specify the supported video source type.
   */
  VT_ERR_INVALID_VIDEO_SOURCE_TYPE = 2,
  /**
   * 3: The image path is invalid. You need to re-specify the correct image path.
   */
  VT_ERR_INVALID_IMAGE_PATH = 3,
  /**
   * 4: The image format is invalid. Make sure the image format is one of PNG, JPEG, or GIF.
   */
  VT_ERR_UNSUPPORT_IMAGE_FORMAT = 4,
  /**
   * 5: The video encoding resolution after video mixing is invalid.
   */
  VT_ERR_INVALID_LAYOUT = 5,
  /**
   * 20: Unknown internal error.
   */
  VT_ERR_INTERNAL = 20
};


/**
 * @brief The source of the audio streams that are mixed locally.
 */
struct MixedAudioStream {
  /**
   * The type of the audio source. See `AUDIO_SOURCE_TYPE`.
   */
  AUDIO_SOURCE_TYPE sourceType;
  /**
   * The user ID of the remote user.
   * @note Set this parameter if the source type of the locally mixed audio steams is
   * AUDIO_SOURCE_REMOTE_USER.
   */
  uid_t remoteUserUid;
  /**
   * The channel name. This parameter signifies the channel in which users engage in real-time audio
   * and video interaction. Under the premise of the same App ID, users who fill in the same channel
   * ID enter the same channel for audio and video interaction. The string length must be less than 64
   * bytes. Supported characters (89 characters in total):
   * - All lowercase English letters: a to z.
   * - All uppercase English letters: A to Z.
   * - All numeric characters: 0 to 9.
   * - "!", "#", "$", "%", "&", "(", ")", "+", "-", ":", ";", "<", "=", ".", ">", "?", "@", "[", "]",
   * "^", "_", "{", "}", "|", "~", ","
   * @note Set this parameter if the source type of the locally mixed audio streams is
   * AUDIO_SOURCE_REMOTE_CHANNEL or AUDIO_SOURCE_REMOTE_USER.
   */
  const char* channelId;
  /**
   * The audio track ID. Set this parameter to the custom audio track ID returned in
   * `createCustomAudioTrack`.
   * @note Set this parameter if the source type of the locally mixed audio steams is
   * AUDIO_SOURCE_CUSTOM.
   */
  track_id_t trackId;

  MixedAudioStream(AUDIO_SOURCE_TYPE source)
    : sourceType(source),
      remoteUserUid(0),
      channelId(NULL),
      trackId(-1) {}

  MixedAudioStream(AUDIO_SOURCE_TYPE source, track_id_t track)
    : sourceType(source),
      trackId(track) {}

  MixedAudioStream(AUDIO_SOURCE_TYPE source, uid_t uid, const char* channel)
    : sourceType(source),
      remoteUserUid(uid),
      channelId(channel) {}

  MixedAudioStream(AUDIO_SOURCE_TYPE source, uid_t uid, const char* channel, track_id_t track)
    : sourceType(source),
      remoteUserUid(uid),
      channelId(channel),
      trackId(track) {}

};

/**
 * @brief The configurations for mixing the lcoal audio.
 */
struct LocalAudioMixerConfiguration {
  /**
   * The number of the audio streams that are mixed locally.
   */
  unsigned int streamCount;
  /**
   * The source of the audio streams that are mixed locally. See `MixedAudioStream`.
   */
  MixedAudioStream* audioInputStreams;

  /**
   * Whether the mxied audio stream uses the timestamp of the audio frames captured by the local
   * microphone.
   * - `true`: (Default) Yes. Set to this value if you want all locally captured audio streams
   * synchronized.
   * - `false`: No. The SDK uses the timestamp of the audio frames at the time when they are mixed.
   */
  bool syncWithLocalMic;

  LocalAudioMixerConfiguration() : streamCount(0), syncWithLocalMic(true) {}
};

/**
 * @brief Configurations of the last-mile network test.
 */
struct LastmileProbeConfig {
  /**
   * Sets whether to test the uplink network. Some users, for example, the audience members in a
   * LIVE_BROADCASTING channel, do not need such a test.
   * - `true`: Test the uplink network.
   * - `false`: Do not test the uplink network.
   */
  bool probeUplink;
  /**
   * Sets whether to test the downlink network:
   * - `true`: Test the downlink network.
   * - `false`: Do not test the downlink network.
   */
  bool probeDownlink;
  /**
   * The expected maximum uplink bitrate (bps) of the local user. The value range is [100000,
   * 5000000]. Agora recommends referring to `setVideoEncoderConfiguration` to set the value.
   */
  unsigned int expectedUplinkBitrate;
  /**
   * The expected maximum downlink bitrate (bps) of the local user. The value range is
   * [100000,5000000].
   */
  unsigned int expectedDownlinkBitrate;
};

/**
 * @brief The status of the last-mile probe test.
 */
enum LASTMILE_PROBE_RESULT_STATE {
  /**
   * 1: The last-mile network probe test is complete.
   */
  LASTMILE_PROBE_RESULT_COMPLETE = 1,
  /**
   * 2: The last-mile network probe test is incomplete because the bandwidth estimation is not
   * available due to limited test resources. One possible reason is that testing resources are
   * temporarily limited.
   */
  LASTMILE_PROBE_RESULT_INCOMPLETE_NO_BWE = 2,
  /**
   * 3: The last-mile network probe test is not carried out. Probably due to poor network conditions.
   */
  LASTMILE_PROBE_RESULT_UNAVAILABLE = 3
};

/**
 * @brief Results of the uplink or downlink last-mile network test.
 */
struct LastmileProbeOneWayResult {
  /**
   * The packet loss rate (%).
   */
  unsigned int packetLossRate;
  /**
   * The network jitter (ms).
   */
  unsigned int jitter;
  /**
   * The estimated available bandwidth (bps).
   */
  unsigned int availableBandwidth;

  LastmileProbeOneWayResult() : packetLossRate(0), jitter(0), availableBandwidth(0) {}
};

/**
 * @brief Results of the uplink and downlink last-mile network tests.
 */
struct LastmileProbeResult {
  /**
   * The status of the last-mile network tests. See `LASTMILE_PROBE_RESULT_STATE`.
   */
  LASTMILE_PROBE_RESULT_STATE state;
  /**
   * Results of the uplink last-mile network test. See `LastmileProbeOneWayResult`.
   */
  LastmileProbeOneWayResult uplinkReport;
  /**
   * Results of the downlink last-mile network test. See `LastmileProbeOneWayResult`.
   */
  LastmileProbeOneWayResult downlinkReport;
  /**
   * The round-trip time (ms).
   */
  unsigned int rtt;

  LastmileProbeResult() : state(LASTMILE_PROBE_RESULT_UNAVAILABLE), rtt(0) {}
};

/**
 * @brief Reasons causing the change of the connection state.
 */
enum CONNECTION_CHANGED_REASON_TYPE {
  /**
   * 0: The SDK is connecting to the Agora edge server.
   */
  CONNECTION_CHANGED_CONNECTING = 0,
  /**
   * 1: The SDK has joined the channel successfully.
   */
  CONNECTION_CHANGED_JOIN_SUCCESS = 1,
  /**
   * 2: The connection between the SDK and the Agora edge server is interrupted.
   */
  CONNECTION_CHANGED_INTERRUPTED = 2,
  /**
   * 3: The connection between the SDK and the Agora edge server is banned by the Agora edge server.
   * For example, when a user is kicked out of the channel, this status will be returned.
   */
  CONNECTION_CHANGED_BANNED_BY_SERVER = 3,
  /**
   * 4: The SDK fails to join the channel. When the SDK fails to join the channel for more than 20
   * minutes, this code will be returned and the SDK stops reconnecting to the channel. You need to
   * prompt the user to try to switch to another network and rejoin the channel.
   */
  CONNECTION_CHANGED_JOIN_FAILED = 4,
  /**
   * 5: The SDK has left the channel.
   */
  CONNECTION_CHANGED_LEAVE_CHANNEL = 5,
  /**
   * 6: The App ID is invalid. You need to rejoin the channel with a valid APP ID and make sure the
   * App ID you are using is consistent with the one generated in the Agora Console.
   */
  CONNECTION_CHANGED_INVALID_APP_ID = 6,
  /**
   * 7: Invalid channel name. Rejoin the channel with a valid channel name. A valid channel name is a
   * string of up to 64 bytes in length. Supported characters (89 characters in total):
   * - All lowercase English letters: a to z.
   * - All uppercase English letters: A to Z.
   * - All numeric characters: 0 to 9.
   * - "!", "#", "$", "%", "&", "(", ")", "+", "-", ":", ";", "<", "=", ".", ">", "?", "@", "[", "]",
   * "^", "_", "{", "}", "|", "~", ","
   */
  CONNECTION_CHANGED_INVALID_CHANNEL_NAME = 7,
  /**
   * 8: Invalid token. Possible reasons are as follows:
   * - The App Certificate for the project is enabled in Agora Console, but you do not pass in a token
   * when joining a channel.
   * - The uid specified when calling `joinChannel(const char* token, const char* channelId, uid_t
   * uid, const ChannelMediaOptions& options)` to join the channel is inconsistent with the
   * uid passed in when generating the token.
   * - The generated token and the token used to join the channel are not consistent.
   * Ensure the following:
   * - When your project enables App Certificate, you need to pass in a token to join a channel.
   * - The user ID specified when generating the token is consistent with the user ID used when
   * joining the channel.
   * - The generated token is the same as the token passed in to join the channel.
   */
  CONNECTION_CHANGED_INVALID_TOKEN = 8,
  /**
   * 9: The token currently being used has expired. You need to generate a new token on your server
   * and rejoin the channel with the new token.
   */
  CONNECTION_CHANGED_TOKEN_EXPIRED = 9,
  /**
   * 10: The connection is rejected by server. Possible reasons are as follows:
   * - The user is already in the channel and still calls a method, for example, `joinChannel(const
   * char* token, const char* channelId, uid_t uid, const ChannelMediaOptions& options)`,
   * to join the channel. Stop calling this method to clear this error.
   * - The user tries to join a channel while a test call is in progress. The user needs to join the
   * channel after the call test ends.
   */
  CONNECTION_CHANGED_REJECTED_BY_SERVER = 10,
  /**
   * 11: The connection state changed to reconnecting because the SDK has set a proxy server.
   */
  CONNECTION_CHANGED_SETTING_PROXY_SERVER = 11,
  /**
   * 12: The connection state changed because the token is renewed.
   */
  CONNECTION_CHANGED_RENEW_TOKEN = 12,
  /**
   * 13: Client IP address changed. If you receive this code multiple times, You need to prompt the
   * user to switch networks and try joining the channel again.
   */
  CONNECTION_CHANGED_CLIENT_IP_ADDRESS_CHANGED = 13,
  /**
   * 14: Timeout for the keep-alive of the connection between the SDK and the Agora edge server. The
   * SDK tries to reconnect to the server automatically.
   */
  CONNECTION_CHANGED_KEEP_ALIVE_TIMEOUT = 14,
  /**
   * 15: The user has rejoined the channel successfully.
   */
  CONNECTION_CHANGED_REJOIN_SUCCESS = 15,
  /**
   * 16: The connection between the SDK and the server is lost.
   */
  CONNECTION_CHANGED_LOST = 16,
  /**
   * 17: The connection state changes due to the echo test.
   */
  CONNECTION_CHANGED_ECHO_TEST = 17,
  /**
   * 18: The local IP address was changed by the user.
   */
  CONNECTION_CHANGED_CLIENT_IP_ADDRESS_CHANGED_BY_USER = 18,
  /**
   * 19: The user joined the same channel from different devices with the same UID.
   */
  CONNECTION_CHANGED_SAME_UID_LOGIN = 19,
  /**
   * 20: The number of hosts in the channel has reached the upper limit.
   */
  CONNECTION_CHANGED_TOO_MANY_BROADCASTERS = 20,

  /**
   * 21: The connection is failed due to license validation failure.
   */
  CONNECTION_CHANGED_LICENSE_VALIDATION_FAILURE = 21,
  /*
   * 22: The connection is failed due to certification verify failure.
   */
  CONNECTION_CHANGED_CERTIFICATION_VERYFY_FAILURE = 22,
  /**
   * 23: The connection is failed due to the lack of granting permission to the stream channel.
   */
  CONNECTION_CHANGED_STREAM_CHANNEL_NOT_AVAILABLE = 23,
  /**
   * 24: The connection is failed due to join channel with an inconsistent appid.
   */
  CONNECTION_CHANGED_INCONSISTENT_APPID = 24,
};

/**
 * @brief The reason for a user role switch failure.
 */
enum CLIENT_ROLE_CHANGE_FAILED_REASON {
  /**
   * 1: The number of hosts in the channel exceeds the limit.
   * @note This enumerator is reported only when the support for 128 users is enabled. The maximum
   * number of hosts is based on the actual number of hosts configured when you enable the 128-user
   * feature.
   */
  CLIENT_ROLE_CHANGE_FAILED_TOO_MANY_BROADCASTERS = 1,
  /**
   * 2: The request is rejected by the Agora server. Agora recommends you prompt the user to try to
   * switch their user role again.
   */
  CLIENT_ROLE_CHANGE_FAILED_NOT_AUTHORIZED = 2,
  /**
   * 3: The request is timed out. Agora recommends you prompt the user to check the network connection
   * and try to switch their user role again.
   * @deprecated This reason is deprecated.
   */
  CLIENT_ROLE_CHANGE_FAILED_REQUEST_TIME_OUT __deprecated = 3,
  /**
   * 4: The SDK is disconnected from the Agora edge server. You can troubleshoot the failure through
   * the `reason` reported by `onConnectionStateChanged`.
   * @deprecated This reason is deprecated.
   */
  CLIENT_ROLE_CHANGE_FAILED_CONNECTION_FAILED __deprecated = 4,
};

/**
 * @brief Network type.
 */
enum NETWORK_TYPE {
  /**
   * -1: The network type is unknown.
   */
  NETWORK_TYPE_UNKNOWN = -1,
  /**
   * 0: The SDK disconnects from the network.
   */
  NETWORK_TYPE_DISCONNECTED = 0,
  /**
   * 1: The network type is LAN.
   */
  NETWORK_TYPE_LAN = 1,
  /**
   * 2: The network type is Wi-Fi (including hotspots).
   */
  NETWORK_TYPE_WIFI = 2,
  /**
   * 3: The network type is mobile 2G.
   */
  NETWORK_TYPE_MOBILE_2G = 3,
  /**
   * 4: The network type is mobile 3G.
   */
  NETWORK_TYPE_MOBILE_3G = 4,
  /**
   * 5: The network type is mobile 4G.
   */
  NETWORK_TYPE_MOBILE_4G = 5,
  /**
   * 6: The network type is mobile 5G.
   */
  NETWORK_TYPE_MOBILE_5G = 6,
};

/**
 * @brief Setting mode of the view.
 */
enum VIDEO_VIEW_SETUP_MODE {
  /**
   * 0: (Default) Clear all added views and replace with a new view.
   */
  VIDEO_VIEW_SETUP_REPLACE = 0,
  /**
   * 1: Adds a view.
   */
  VIDEO_VIEW_SETUP_ADD = 1,
  /**
   * 2: Deletes a view.
   * @note When you no longer need to use a certain view, it is recommended to delete the view by
   * setting `setupMode` to VIDEO_VIEW_SETUP_REMOVE, otherwise it may lead to leak of rendering
   * resources.
   */
  VIDEO_VIEW_SETUP_REMOVE = 2,
};

/**
 * @brief Attributes of the video canvas object.
 */
struct VideoCanvas {
  /**
   * User ID that publishes the video source.
   */
  uid_t uid;

  /**
   * The ID of the user who publishes a specific sub-video stream within the mixed video stream.
   */
  uid_t subviewUid;
  /**
   * The video display window.
   * @note In one `VideoCanvas`, you can only choose to set either `view` or `surfaceTexture`. If both
   * are set, only the settings in `view` take effect.
   */
  view_t view;
  /**
   * The background color of the video canvas in RGBA format. The default value is 0x00000000, which
   * represents black.
   */
  uint32_t backgroundColor;
  /**
   * The rendering mode of the video. See `RENDER_MODE_TYPE`.
   */
  media::base::RENDER_MODE_TYPE renderMode;
  /**
   * The mirror mode of the view. See `VIDEO_MIRROR_MODE_TYPE`.
   * @note
   * - For the mirror mode of the local video view: If you use a front camera, the SDK enables the
   * mirror mode by default; if you use a rear camera, the SDK disables the mirror mode by default.
   * - For the remote user: The mirror mode is disabled by default.
   */
  VIDEO_MIRROR_MODE_TYPE mirrorMode;
  /**
   * Setting mode of the view. See `VIDEO_VIEW_SETUP_MODE`.
   */
  VIDEO_VIEW_SETUP_MODE setupMode;
  /**
   * The type of the video source. See `VIDEO_SOURCE_TYPE`.
   */
  VIDEO_SOURCE_TYPE sourceType;
  /**
   * The ID of the media player. You can get the Device ID by calling `getMediaPlayerId`.
   */
  int mediaPlayerId;
  /**
   * (Optional) Display area of the video frame, see `Rectangle`. `width` and `height` represent the
   * video pixel width and height of the area. The default value is null (width or height is 0), which
   * means that the actual resolution of the video frame is displayed.
   */
  Rectangle cropArea;
  /**
   * (Optional) Whether to enable alpha mask rendering:
   * - `true`: Enable alpha mask rendering.
   * - `false`: (Default) Disable alpha mask rendering.
   * Alpha mask rendering can create images with transparent effects and extract portraits from
   * videos. When used in combination with other methods, you can implement effects such as
   * portrait-in-picture and watermarking.
   * @note
   * - The receiver can render alpha channel information only when the sender enables alpha
   * transmission.
   * - To enable alpha transmission, `technical support`.
   */
  bool enableAlphaMask;
  /**
   * The observation position of the video frame in the video link. See `VIDEO_MODULE_POSITION`.
   */
  media::base::VIDEO_MODULE_POSITION position;

  VideoCanvas()
      : uid(0),
        subviewUid(0),
        view(NULL),
        backgroundColor(0x00000000),
        renderMode(media::base::RENDER_MODE_HIDDEN),
        mirrorMode(VIDEO_MIRROR_MODE_AUTO),
        setupMode(VIDEO_VIEW_SETUP_REPLACE),
        sourceType(VIDEO_SOURCE_CAMERA_PRIMARY),
        mediaPlayerId(-ERR_NOT_READY),
        cropArea(0, 0, 0, 0),
        enableAlphaMask(false),
        position(media::base::POSITION_POST_CAPTURER) {}

  VideoCanvas(view_t v, media::base::RENDER_MODE_TYPE m, VIDEO_MIRROR_MODE_TYPE mt)
      : uid(0),
        subviewUid(0),
        view(v),
        backgroundColor(0x00000000),
        renderMode(m),
        mirrorMode(mt),
        setupMode(VIDEO_VIEW_SETUP_REPLACE),
        sourceType(VIDEO_SOURCE_CAMERA_PRIMARY),
        mediaPlayerId(-ERR_NOT_READY),
        cropArea(0, 0, 0, 0),
        enableAlphaMask(false),
        position(media::base::POSITION_POST_CAPTURER) {}

  VideoCanvas(view_t v, media::base::RENDER_MODE_TYPE m, VIDEO_MIRROR_MODE_TYPE mt, uid_t u)
      : uid(u),
        subviewUid(0),
        view(v),
        backgroundColor(0x00000000),
        renderMode(m),
        mirrorMode(mt),
        setupMode(VIDEO_VIEW_SETUP_REPLACE),
        sourceType(VIDEO_SOURCE_CAMERA_PRIMARY),
        mediaPlayerId(-ERR_NOT_READY),
        cropArea(0, 0, 0, 0),
        enableAlphaMask(false),
        position(media::base::POSITION_POST_CAPTURER) {}

  VideoCanvas(view_t v, media::base::RENDER_MODE_TYPE m, VIDEO_MIRROR_MODE_TYPE mt, uid_t u,
              uid_t subu)
      : uid(u),
        subviewUid(subu),
        view(v),
        backgroundColor(0x00000000),
        renderMode(m),
        mirrorMode(mt),
        setupMode(VIDEO_VIEW_SETUP_REPLACE),
        sourceType(VIDEO_SOURCE_CAMERA_PRIMARY),
        mediaPlayerId(-ERR_NOT_READY),
        cropArea(0, 0, 0, 0),
        enableAlphaMask(false),
        position(media::base::POSITION_POST_CAPTURER) {}
};

/**
 * @brief Image enhancement options.
 */
struct BeautyOptions {
  /**
   * @brief The contrast level.
   */
  enum LIGHTENING_CONTRAST_LEVEL {
    /**
     * 0: Low contrast level.
     */
    LIGHTENING_CONTRAST_LOW = 0,
    /**
     * 1: (Default) Normal contrast level.
     */
    LIGHTENING_CONTRAST_NORMAL = 1,
    /**
     * 2: High contrast level.
     */
    LIGHTENING_CONTRAST_HIGH = 2,
  };

  /**
   * The contrast level, used with the `lighteningLevel` parameter. The larger the value, the greater
   * the contrast between light and dark. See `LIGHTENING_CONTRAST_LEVEL`.
   */
  LIGHTENING_CONTRAST_LEVEL lighteningContrastLevel;

  /**
   * The brightening level, in the range [0.0,1.0], where 0.0 means the original brightening. The
   * default value is 0.0. The higher the value, the greater the degree of brightening.
   */
  float lighteningLevel;

  /**
   * The smoothness level, in the range [0.0,1.0], where 0.0 means the original smoothness. The
   * default value is 0.0. The greater the value, the greater the smoothness level.
   */
  float smoothnessLevel;

  /**
   * The redness level, in the range [0.0,1.0], where 0.0 means the original redness. The default
   * value is 0.0. The larger the value, the greater the redness level.
   */
  float rednessLevel;

  /**
   * The sharpness level, in the range [0.0,1.0], where 0.0 means the original sharpness. The default
   * value is 0.0. The larger the value, the greater the sharpness level.
   */
  float sharpnessLevel;

  BeautyOptions(LIGHTENING_CONTRAST_LEVEL contrastLevel, float lightening, float smoothness,
                float redness, float sharpness)
      : lighteningContrastLevel(contrastLevel),
        lighteningLevel(lightening),
        smoothnessLevel(smoothness),
        rednessLevel(redness),
        sharpnessLevel(sharpness) {}

  BeautyOptions()
      : lighteningContrastLevel(LIGHTENING_CONTRAST_NORMAL),
        lighteningLevel(0),
        smoothnessLevel(0),
        rednessLevel(0),
        sharpnessLevel(0) {}
};

/**
 * @brief Filter effect options.
 *
 * @since v4.4.0
 */
struct FaceShapeAreaOptions {
  /**
   * @brief Chooses the specific facial areas that need to be adjusted.
   *
   * @since v4.4.0
   */
  enum FACE_SHAPE_AREA {
    /**
     * -1: (Default) Invalid area; facial enhancement effects do not take effect.
     */
    FACE_SHAPE_AREA_NONE = -1,
    /**
     * (100): Head, used to achieve a smaller head effect. The value range is 0 to 100, and the default
     * value is 50. The larger the value, the more noticeable the adjustment.
     */
    FACE_SHAPE_AREA_HEADSCALE = 100,
    /**
     * (101): Forehead, used to adjust the hairline height. The range is [0, 100], with a default value
     * of 0. The larger the value, the more noticeable the adjustment.
     */
    FACE_SHAPE_AREA_FOREHEAD = 101,
    /**
     * (102): Face contour, used to achieve a slimmer face effect. The range is [0, 100], with a default
     * value of 0. The larger the value, the more noticeable the adjustment.
     */
    FACE_SHAPE_AREA_FACECONTOUR = 102,
    /**
     * (103): Face length, used to achieve a longer face effect. The range is [-100, 100], with a
     * default value of 0. The greater the absolute value, the more noticeable the adjustment. Negative
     * values indicate the opposite direction.
     */
    FACE_SHAPE_AREA_FACELENGTH = 103,
    /**
     * (104): Face width, used to achieve a narrower face effect. The range is [0, 100], with a default
     * value of 0. The larger the value, the more noticeable the adjustment.
     */
    FACE_SHAPE_AREA_FACEWIDTH = 104,
    /**
     * (105): Cheekbone, used to adjust cheekbone width. The range is [0, 100], with a default value of
     * 0. The larger the value, the more noticeable the adjustment.The larger the value, the more
     * noticeable the adjustment.
     */
    FACE_SHAPE_AREA_CHEEKBONE = 105,
    /**
     * (106): Cheek, used to adjust cheek width. The range is [0, 100], with a default value of 0. The
     * larger the value, the more noticeable the adjustment.
     */
    FACE_SHAPE_AREA_CHEEK = 106,
    /**
     * (107): Adjustment of the mandible. The range is [0, 100], with a default value of 0. The larger
     * the value, the more noticeable the adjustment.
     * @since v4.6.0
     */
    FACE_SHAPE_AREA_MANDIBLE = 107,
    /**
     * (108): Chin, used to adjust chin length. The range is [-100, 100], with a default value of 0. The
     * greater the absolute value, the more noticeable the adjustment. Negative values indicate the
     * opposite direction.
     */
    FACE_SHAPE_AREA_CHIN = 108,
    /**
     * (200): Eyes, used to achieve a larger eye effect. The value range is 0 to 100, and the default
     * value is 50. The larger the value, the more noticeable the adjustment.
     */
    FACE_SHAPE_AREA_EYESCALE = 200,
    /**
     * (201): Eye distance adjustment. The range is [-100, 100], with a default value of 0. The greater
     * the absolute value, the more noticeable the adjustment. Negative values indicate the opposite
     * direction.
     * @since v4.6.0
     */
    FACE_SHAPE_AREA_EYEDISTANCE = 201,
    /**
     * (202): Eye position adjustment. The range is [-100, 100], with a default value of 0. The greater
     * the absolute value, the more noticeable the adjustment. Negative values indicate the opposite
     * direction.
     * @since v4.6.0
     */
    FACE_SHAPE_AREA_EYEPOSITION = 202,
    /**
     * (203): Lower eyelid adjustment.(203): Lower eyelid adjustment. The range is [0, 100], with a
     * default value of 0. The larger the value, the more noticeable the adjustment.
     * @since v4.6.0
     */
    FACE_SHAPE_AREA_LOWEREYELID = 203,
    /**
     * (204): Pupil size adjustment. The range is [0, 100], with a default value of 0. The larger the
     * value, the more noticeable the adjustment.
     * @since v4.6.0
     */
    FACE_SHAPE_AREA_EYEPUPILS = 204,
    /**
     * (205): Inner eye corner adjustment. The range is [-100, 100], with a default value of 0. The
     * greater the absolute value, the more noticeable the adjustment. Negative values indicate the
     * opposite direction.
     * @since v4.6.0
     */
    FACE_SHAPE_AREA_EYEINNERCORNER = 205,
    /**
     * (206): Outer eye corner adjustment. The range is [-100, 100], with a default value of 0. The
     * greater the absolute value, the more noticeable the adjustment. Negative values indicate the
     * opposite direction.
     * @since v4.6.0
     */
    FACE_SHAPE_AREA_EYEOUTERCORNER = 206,
    /**
     * (300): Nose length, used to achieve a longer nose effect. The range is [-100, 100], with a
     * default value of 0.
     */
    FACE_SHAPE_AREA_NOSELENGTH = 300,
    /**
     * (301): Nose width, used to achieve a slimmer nose effect. The range is [0, 100], with a default
     * value of 0. The larger the value, the more noticiable the effect of narrowing the nose.
     * @since v4.6.0
     */
    FACE_SHAPE_AREA_NOSEWIDTH = 301,
    /**
     * (302): Nose wing adjustment. The value range is 0 to 100, and the default value is 10. The larger
     * the value, the more noticeable the adjustment.
     * @since v4.6.0
     */
    FACE_SHAPE_AREA_NOSEWING = 302,
    /**
     * (303): Nose root adjustment. The range is [0, 100], with a default value of 0. The larger the
     * value, the more noticeable the adjustment.
     * @since v4.6.0
     */
    FACE_SHAPE_AREA_NOSEROOT = 303,
    /**
     * (304): Nose bridge adjustment. The value range is 0 to 100, and the default value is 50. The
     * larger the value, the more noticeable the adjustment.
     * @since v4.6.0
     */
    FACE_SHAPE_AREA_NOSEBRIDGE = 304,
    /**
     * (305): Nose tip adjustment. The value range is 0 to 100, and the default value is 50. The larger
     * the value, the more noticeable the adjustment.
     * @since v4.6.0
     */
    FACE_SHAPE_AREA_NOSETIP = 305,
    /**
     * (306): Overall nose adjustment. The range is [-100, 100], with a default value of 50. The greater
     * the absolute value, the more noticeable the adjustment. Negative values indicate the opposite
     * direction.
     * @since v4.6.0
     */
    FACE_SHAPE_AREA_NOSEGENERAL = 306,
    /**
     * (400): Mouth, used to achieve a larger mouth effect. The range is [-100, 100], with a default
     * value of 20. The greater the absolute value, the more noticeable the adjustment. Negative values
     * indicate the opposite direction.
     * @since v4.6.0
     */
    FACE_SHAPE_AREA_MOUTHSCALE = 400,
    /**
     * (401): Mouth position adjustment. The range is [0, 100], with a default value of 0. The larger
     * the value, the more noticeable the adjustment.
     * @since v4.6.0
     */
    FACE_SHAPE_AREA_MOUTHPOSITION = 401,
    /**
     * (402): Mouth smile adjustment. The value range is [0,1], and the default value is 0. The larger
     * the value, the more noticeable the adjustment.
     * @since v4.6.0
     */
    FACE_SHAPE_AREA_MOUTHSMILE = 402,
    /**
     * (403): Lip shape adjustment. The range is [0, 100], with a default value of 0. The larger the
     * value, the more noticeable the adjustment.
     * @note v.4.6.0.
     * @since v4.6.0
     */
    FACE_SHAPE_AREA_MOUTHLIP = 403,
    /**
     * (500): Eyebrow position adjustment. The range is [-100, 100], with a default value of 0. The
     * greater the absolute value, the more noticeable the adjustment. Negative values indicate the
     * opposite direction.
     * @since v4.6.0
     */
    FACE_SHAPE_AREA_EYEBROWPOSITION = 500,
    /**
     * (501): Eyebrow thickness adjustment. The range is [-100, 100], with a default value of 0. The
     * larger the value, the more noticeable the adjustment.
     * @since v4.6.0
     */
    FACE_SHAPE_AREA_EYEBROWTHICKNESS = 501,
  };
  
  /**
   * Facial enhancement areas: `FACE_SHAPE_AREA`
   */
  FACE_SHAPE_AREA shapeArea;
  
  /**
   * The intensity of the enhancement. The definition of enhancement intensity varies according to the
   * different face areas, such as its orientation, range, and preset value. See `FACE_SHAPE_AREA`.
   */
  int shapeIntensity;
  
  FaceShapeAreaOptions(FACE_SHAPE_AREA shapeArea, int areaIntensity) : shapeArea(shapeArea), shapeIntensity(areaIntensity) {}

  FaceShapeAreaOptions() : shapeArea(FACE_SHAPE_AREA_NONE), shapeIntensity(0) {}
};

/**
 * @brief The facial enhancement style options.
 *
 * @since v4.4.0
 */
struct FaceShapeBeautyOptions {
  /**
   * @brief The facial enhancement style options.
   *
   * @since v4.4.0
   */
  enum FACE_SHAPE_BEAUTY_STYLE {
  /**
   * 0: (Default) Feminine style.
   */
  FACE_SHAPE_BEAUTY_STYLE_FEMALE = 0,
  /**
   * 1: Masculine style.
   */
  FACE_SHAPE_BEAUTY_STYLE_MALE = 1,
  /**
   * 2: The natural style beauty effect only makes minimal adjustments to facial features.
   * @since v4.6.0
   */
  FACE_SHAPE_BEAUTY_STYLE_NATURAL = 2,
  };
  
  /**
   * Facial enhancement style options: `FACE_SHAPE_BEAUTY_STYLE`.
   */
  FACE_SHAPE_BEAUTY_STYLE shapeStyle;
  
  /**
   * The intensity of the facial enhancement style, with a value range oof [0.0,1.0]. The default
   * value is 0.0, which means no face enhancement effect. The higher the value, the more obvious the
   * facial enhancement effect.
   */
  int styleIntensity;
  
  FaceShapeBeautyOptions(FACE_SHAPE_BEAUTY_STYLE shapeStyle, int styleIntensity) : shapeStyle(shapeStyle), styleIntensity(styleIntensity) {}

  FaceShapeBeautyOptions() : shapeStyle(FACE_SHAPE_BEAUTY_STYLE_FEMALE), styleIntensity(50) {}
};

/**
 * @brief Filter effect options.
 *
 * @since v4.4.1
 */
struct FilterEffectOptions {
  /**
   * The absolute path to the local cube map texture file, which can be used to customize the filter
   * effect. The specified .cude file should strictly follow the Cube LUT Format Specification;
   * otherwise, the filter options do not take effect. The following is a sample of the .cude file:
   * ```
   * LUT_3D_SIZE 32
   * 0.0039215689 0 0.0039215682
   * 0.0086021447 0.0037950677 0
   * ...
   * 0.0728652592 0.0039215689 0
   * ```
   * @note
   * - The identifier `LUT_3D_SIZE` on the first line of the cube map file represents the size of the
   * three-dimensional lookup table. The LUT size for filter effect can only be set to 32.
   * - The SDK provides a built-in `built_in_whiten_filter.cube` file. You can pass the absolute path
   * of this file to get the whitening filter effect.
   */
  const char * path;
  
  /**
   * The intensity of the filter effect, with a range value of [0.0,1.0], in which 0.0 represents no
   * filter effect. The default value is 0.5. The higher the value, the stronger the filter effect.
   */
  float strength;
  
  FilterEffectOptions(const char * lut3dPath, float filterStrength) : path(lut3dPath), strength(filterStrength) {}

  FilterEffectOptions() : path(OPTIONAL_NULLPTR), strength(0.5) {}
};

/**
 * @brief The low-light enhancement options.
 */
struct LowlightEnhanceOptions {
  /**
   * @brief The low-light enhancement mode.
   */
  enum LOW_LIGHT_ENHANCE_MODE {
    /**
     * 0: (Default) Automatic mode. The SDK automatically enables or disables the low-light enhancement
     * feature according to the ambient light to compensate for the lighting level or prevent
     * overexposure, as necessary.
     */
    LOW_LIGHT_ENHANCE_AUTO = 0,
    /**
     * 1: Manual mode. Users need to enable or disable the low-light enhancement feature manually.
     */
    LOW_LIGHT_ENHANCE_MANUAL = 1,
  };
  /**
   * @brief The low-light enhancement level.
   */
  enum LOW_LIGHT_ENHANCE_LEVEL {
    /**
     * 0: (Default) Promotes video quality during low-light enhancement. It processes the brightness,
     * details, and noise of the video image. The performance consumption is moderate, the processing
     * speed is moderate, and the overall video quality is optimal.
     */
    LOW_LIGHT_ENHANCE_LEVEL_HIGH_QUALITY = 0,
    /**
     * 1: Promotes performance during low-light enhancement. It processes the brightness and details of
     * the video image. The processing speed is faster.
     */
    LOW_LIGHT_ENHANCE_LEVEL_FAST = 1,
  };

  /**
   * The low-light enhancement mode. See `LOW_LIGHT_ENHANCE_MODE`.
   */
  LOW_LIGHT_ENHANCE_MODE mode;

  /**
   * The low-light enhancement level. See `LOW_LIGHT_ENHANCE_LEVEL`.
   */
  LOW_LIGHT_ENHANCE_LEVEL level;

  LowlightEnhanceOptions(LOW_LIGHT_ENHANCE_MODE lowlightMode, LOW_LIGHT_ENHANCE_LEVEL lowlightLevel)
      : mode(lowlightMode), level(lowlightLevel) {}

  LowlightEnhanceOptions()
      : mode(LOW_LIGHT_ENHANCE_AUTO), level(LOW_LIGHT_ENHANCE_LEVEL_HIGH_QUALITY) {}
};
/**
 * @brief Video noise reduction options.
 *
 * @since v4.0.0
 */
struct VideoDenoiserOptions {
  /**
   * @brief Video noise reduction mode.
   */
  enum VIDEO_DENOISER_MODE {
    /**
     * 0: (Default) Automatic mode. The SDK automatically enables or disables the video noise reduction
     * feature according to the ambient light.
     */
    VIDEO_DENOISER_AUTO = 0,
    /**
     * 1: Manual mode. Users need to enable or disable the video noise reduction feature manually.
     */
    VIDEO_DENOISER_MANUAL = 1,
  };
  /**
   * @brief Video noise reduction level.
   */
  enum VIDEO_DENOISER_LEVEL {
    /**
     * 0: (Default) Promotes video quality during video noise reduction. balances performance
     * consumption and video noise reduction quality. The performance consumption is moderate, the video
     * noise reduction speed is moderate, and the overall video quality is optimal.
     */
    VIDEO_DENOISER_LEVEL_HIGH_QUALITY = 0,
    /**
     * 1: Promotes reducing performance consumption during video noise reduction. It prioritizes
     * reducing performance consumption over video noise reduction quality. The performance consumption
     * is lower, and the video noise reduction speed is faster. To avoid a noticeable shadowing effect
     * (shadows trailing behind moving objects) in the processed video, Agora recommends that you use
     * this setting when the camera is fixed.
     */
    VIDEO_DENOISER_LEVEL_FAST = 1,
  };
  /**
   * Video noise reduction mode. See `VIDEO_DENOISER_MODE`.
   */
  VIDEO_DENOISER_MODE mode;

  /**
   * Video noise reduction level. See `VIDEO_DENOISER_LEVEL`.
   */
  VIDEO_DENOISER_LEVEL level;

  VideoDenoiserOptions(VIDEO_DENOISER_MODE denoiserMode, VIDEO_DENOISER_LEVEL denoiserLevel)
      : mode(denoiserMode), level(denoiserLevel) {}

  VideoDenoiserOptions() : mode(VIDEO_DENOISER_AUTO), level(VIDEO_DENOISER_LEVEL_HIGH_QUALITY) {}
};

/**
 * @brief The color enhancement options.
 *
 * @since v4.0.0
 */
struct ColorEnhanceOptions {
  /**
   * The level of color enhancement. The value range is [0.0, 1.0]. `0.0` is the default value, which
   * means no color enhancement is applied to the video. The higher the value, the higher the level of
   * color enhancement. The default value is `0.5`.
   */
  float strengthLevel;

  /**
   * The level of skin tone protection. The value range is [0.0, 1.0]. `0.0` means no skin tone
   * protection. The higher the value, the higher the level of skin tone protection. The default value
   * is `1.0`.
   * - When the level of color enhancement is higher, the portrait skin tone can be significantly
   * distorted, so you need to set the level of skin tone protection.
   * - When the level of skin tone protection is higher, the color enhancement effect can be slightly
   * reduced.
   * Therefore, to get the best color enhancement effect, Agora recommends that you adjust
   * `strengthLevel` and `skinProtectLevel` to get the most appropriate values.
   */
  float skinProtectLevel;

  ColorEnhanceOptions(float stength, float skinProtect)
      : strengthLevel(stength), skinProtectLevel(skinProtect) {}

  ColorEnhanceOptions() : strengthLevel(0), skinProtectLevel(1) {}
};

/**
 * @brief The custom background.
 */
struct VirtualBackgroundSource {
  /**
   * @brief The custom background.
   */
  enum BACKGROUND_SOURCE_TYPE {
    /**
     * 0: Process the background as alpha data without replacement, only separating the portrait and the
     * background. After setting this value, you can call `startLocalVideoTranscoder` to implement the
     * picture-in-picture effect.
     */
    BACKGROUND_NONE = 0,
    /**
     * 1: (Default) The background image is a solid color.
     */
    BACKGROUND_COLOR = 1,
    /**
     * 2: The background is an image in PNG or JPG format.
     */
    BACKGROUND_IMG = 2,
    /**
     * 3: The background is a blurred version of the original background.
     */
    BACKGROUND_BLUR = 3,
    /**
     * 4: The background is a local video in MP4, AVI, MKV, FLV, or other supported formats.
     */
    BACKGROUND_VIDEO = 4,
  };

  /**
   * @brief The degree of blurring applied to the custom background image.
   */
  enum BACKGROUND_BLUR_DEGREE {
    /**
     * 1: The degree of blurring applied to the custom background image is low. The user can almost see
     * the background clearly.
     */
    BLUR_DEGREE_LOW = 1,
    /**
     * 2: The degree of blurring applied to the custom background image is medium. It is difficult for
     * the user to recognize details in the background.
     */
    BLUR_DEGREE_MEDIUM = 2,
    /**
     * 3: (Default) The degree of blurring applied to the custom background image is high. The user can
     * barely see any distinguishing features in the background.
     */
    BLUR_DEGREE_HIGH = 3,
  };

  /**
   * The custom background. See `BACKGROUND_SOURCE_TYPE`.
   */
  BACKGROUND_SOURCE_TYPE background_source_type;

  /**
   * The type of the custom background image. The color of the custom background image. The format is
   * a hexadecimal integer defined by RGB, without the # sign, such as 0xFFB6C1 for light pink. The
   * default value is 0xFFFFFF, which signifies white. The value range is [0x000000, 0xffffff]. If the
   * value is invalid, the SDK replaces the original background image with a white background image.
   * @note
   * This parameter is only applicable to custom backgrounds of the following types:
   * - BACKGROUND_COLOR: The background image is a solid-colored image of the color passed in by the
   * parameter.
   * - BACKGROUND_IMG: If the image in `source` has a transparent background, the transparent
   * background will be filled with the color passed in by the parameter.
   */
  unsigned int color;

  /**
   * The local absolute path of the custom background image. Supports PNG, JPG, MP4, AVI, MKV, and FLV
   * formats. If the path is invalid, the SDK will use either the original background image or the
   * solid color image specified by `color`.
   * @note This parameter takes effect only when the type of the custom background image is
   * BACKGROUND_IMG or BACKGROUND_VIDEO.
   */
  const char* source;

  /**
   * The degree of blurring applied to the custom background image. See `BACKGROUND_BLUR_DEGREE`.
   * @note This parameter takes effect only when the type of the custom background image is
   * BACKGROUND_BLUR.
   */
  BACKGROUND_BLUR_DEGREE blur_degree;

  VirtualBackgroundSource()
      : background_source_type(BACKGROUND_COLOR),
        color(0xffffff),
        source(OPTIONAL_NULLPTR),
        blur_degree(BLUR_DEGREE_HIGH) {}
};

/**
 * @brief Processing properties for background images.
 */
struct SegmentationProperty {
  /**
   * @brief The type of algorithms to user for background processing.
   */
  enum SEG_MODEL_TYPE {
    /**
     * 1: (Default) Use the algorithm suitable for all scenarios.
     */
    SEG_MODEL_AI = 1,
    /**
     * 2: Use the algorithm designed specifically for scenarios with a green screen background.
     */
    SEG_MODEL_GREEN = 2
  };

  /**
   * @brief Screen color type.
   */
  enum SCREEN_COLOR_TYPE {
    /**
     * (0): Automatically selects screen color.
     */
    SCREEN_COLOR_AUTO = 0,
    /**
     * (1): Green screen.
     */
    SCREEN_COLOR_GREEN = 1,
    /**
     * (2): Blue screen.
     */
    SCREEN_COLOR_BLUE = 2
  };

  /**
   * The type of algorithms to user for background processing. See `SEG_MODEL_TYPE`.
   */
  SEG_MODEL_TYPE modelType;

  /**
   * The accuracy range for recognizing background colors in the image. The value range is [0,1], and
   * the default value is 0.5. The larger the value, the wider the range of identifiable shades of
   * pure color. When the value of this parameter is too large, the edge of the portrait and the pure
   * color in the portrait range are also detected. Agora recommends that you dynamically adjust the
   * value of this parameter according to the actual effect.
   * @note This parameter only takes effect when `modelType` is set to `SEG_MODEL_GREEN`.
   */
  float greenCapacity;

  /**
   * The screen color. See `SCREEN_COLOR_TYPE`.
   */
  SCREEN_COLOR_TYPE screenColorType;

  SegmentationProperty() : modelType(SEG_MODEL_AI), greenCapacity(0.5), screenColorType(SCREEN_COLOR_AUTO) {}
};

/**
 * @brief The type of the audio track.
 */
enum AUDIO_TRACK_TYPE {
  /**
   * -1: Invalid audio track
   */
  AUDIO_TRACK_INVALID = -1,
  /**
   * 0: Mixable audio tracks. This type of audio track supports mixing with other audio streams (such
   * as audio streams captured by microphone) and playing locally or publishing to channels after
   * mixing. The latency of mixable audio tracks is higher than that of direct audio tracks.
   */
  AUDIO_TRACK_MIXABLE = 0,
  /**
   * 1: Direct audio tracks. This type of audio track will replace the audio streams captured by the
   * microphone and does not support mixing with other audio streams. The latency of direct audio
   * tracks is lower than that of mixable audio tracks.
   * @note If `AUDIO_TRACK_DIRECT` is specified for this parameter, you must set
   * `publishMicrophoneTrack` to `false` in `ChannelMediaOptions` when calling `joinChannel(const
   * char* token, const char* channelId, uid_t uid, const ChannelMediaOptions& options)` to
   * join the channel; otherwise, joining the channel fails and returns the error code -2.
   */
  AUDIO_TRACK_DIRECT = 1,
};

/**
 * @brief The configuration of custom audio tracks.
 */
struct AudioTrackConfig {
  /**
   * Whether to enable the local audio-playback device:
   * - `true`: (Default) Enable the local audio-playback device.
   * - `false`: Do not enable the local audio-playback device.
   */
  bool enableLocalPlayback;
  /**
   * Whether to enable audio processing module:
   * - `true`Enable the audio processing module to apply the Automatic Echo Cancellation (AEC),
   * Automatic Noise Suppression (ANS), and Automatic Gain Control (AGC) effects.
   * - `false`: (Default) Do not enable the audio processing module.
   * @note This parameter only takes effect on AUDIO_TRACK_DIRECT in custom audio capturing.
   */
  bool enableAudioProcessing;

  AudioTrackConfig() : enableLocalPlayback(true),enableAudioProcessing(false) {}
};

/**
 * Preset local voice reverberation options.
 * bitmap allocation:
 * |  bit31  |    bit30 - bit24   |        bit23 - bit16        | bit15 - bit8 |  bit7 - bit0   |
 * |---------|--------------------|-----------------------------|--------------|----------------|
 * |reserved | 0x1: voice beauty  | 0x1: chat beautification    | effect types | effect settings|
 * |         |                    | 0x2: singing beautification |              |                |
 * |         |                    | 0x3: timbre transform       |              |                |
 * |         |                    | 0x4: ultra high_quality     |              |                |
 * |         |--------------------|-----------------------------|              |                |
 * |         | 0x2: audio effect  | 0x1: space construction     |              |                |
 * |         |                    | 0x2: voice changer effect   |              |                |
 * |         |                    | 0x3: style transform        |              |                |
 * |         |                    | 0x4: electronic sound       |              |                |
 * |         |                    | 0x5: magic tone             |              |                |
 * |         |--------------------|-----------------------------|              |                |
 * |         | 0x3: voice changer | 0x1: voice transform        |              |                |
 */
/**
 * @brief The options for SDK preset voice beautifier effects.
 */
enum VOICE_BEAUTIFIER_PRESET {
  /**
   * Turn off voice beautifier effects and use the original voice.
   */
  VOICE_BEAUTIFIER_OFF = 0x00000000,
  /**
   * A more magnetic voice.
   * @note Agora recommends using this enumerator to process a male-sounding voice; otherwise, you may
   * experience vocal distortion.
   */
  CHAT_BEAUTIFIER_MAGNETIC = 0x01010100,
  /**
   * A fresher voice.
   * @note Agora recommends using this enumerator to process a female-sounding voice; otherwise, you
   * may experience vocal distortion.
   */
  CHAT_BEAUTIFIER_FRESH = 0x01010200,
  /**
   * A more vital voice.
   * @note Agora recommends using this enumerator to process a female-sounding voice; otherwise, you
   * may experience vocal distortion.
   */
  CHAT_BEAUTIFIER_VITALITY = 0x01010300,
  /**
   * Singing beautifier effect.
   * - If you call `setVoiceBeautifierPreset` ( SINGING_BEAUTIFIER ), you can beautify a male-sounding
   * voice and add a reverberation effect that sounds like singing in a small room. Agora recommends
   * using this enumerator to process a male-sounding voice; otherwise, you might experience vocal
   * distortion.
   * - If you call `setVoiceBeautifierParameters` ( SINGING_BEAUTIFIER, param1, param2), you can
   * beautify a male or female-sounding voice and add a reverberation effect.
   */
  SINGING_BEAUTIFIER = 0x01020100,
  /**
   * A more vigorous voice.
   */
  TIMBRE_TRANSFORMATION_VIGOROUS = 0x01030100,
  /**
   * A deep voice.
   */
  TIMBRE_TRANSFORMATION_DEEP = 0x01030200,
  /**
   * A mellower voice.
   */
  TIMBRE_TRANSFORMATION_MELLOW = 0x01030300,
  /**
   * Falsetto.
   */
  TIMBRE_TRANSFORMATION_FALSETTO = 0x01030400,
  /**
   * A fuller voice.
   */
  TIMBRE_TRANSFORMATION_FULL = 0x01030500,
  /**
   * A clearer voice.
   */
  TIMBRE_TRANSFORMATION_CLEAR = 0x01030600,
  /**
   * A more resounding voice.
   */
  TIMBRE_TRANSFORMATION_RESOUNDING = 0x01030700,
  /**
   * A more ringing voice.
   */
  TIMBRE_TRANSFORMATION_RINGING = 0x01030800,
  /**
   * A ultra-high quality voice, which makes the audio clearer and restores more details.
   * - To achieve better audio effect quality, Agora recommends that you set the `profile` of
   * `setAudioProfile(AUDIO_PROFILE_TYPE profile)` to `AUDIO_PROFILE_MUSIC_HIGH_QUALITY` (4) or
   * `AUDIO_PROFILE_MUSIC_HIGH_QUALITY_STEREO` (5) and `scenario` to `AUDIO_SCENARIO_GAME_STREAMING`
   * (3) before calling `setVoiceBeautifierPreset`.
   * - If you have an audio capturing device that can already restore audio details to a high degree,
   * Agora recommends that you do not enable ultra-high quality; otherwise, the SDK may over-restore
   * audio details, and you may not hear the anticipated voice effect.
   */
  ULTRA_HIGH_QUALITY_VOICE = 0x01040100
};

/**
 * @brief Preset audio effects.
 *
 * @details
 * To get better audio effects, Agora recommends calling `setAudioProfile(AUDIO_PROFILE_TYPE profile, AUDIO_SCENARIO_TYPE scenario)` and setting the `profile` parameter as recommended below before using the preset audio effects.
 * | Preset audio effects                                                                              | `profile`                                                                          |
 * | ------------------------------------------------------------------------------------------------- | ---------------------------------------------------------------------------------- |
 * | - ROOM_ACOUSTICS_VIRTUAL_STEREO - ROOM_ACOUSTICS_3D_VOICE - ROOM_ACOUSTICS_VIRTUAL_SURROUND_SOUND | `AUDIO_PROFILE_MUSIC_HIGH_QUALITY_STEREO` or `AUDIO_PROFILE_MUSIC_STANDARD_STEREO` |
 * | Other preset audio effects (except for `AUDIO_EFFECT_OFF` )                                       | `AUDIO_PROFILE_MUSIC_HIGH_QUALITY` or `AUDIO_PROFILE_MUSIC_HIGH_QUALITY_STEREO`    |
 *
 */
enum AUDIO_EFFECT_PRESET {
  /**
   * Turn off voice effects, that is, use the original voice.
   */
  AUDIO_EFFECT_OFF = 0x00000000,
  /**
   * The voice effect typical of a KTV venue.
   */
  ROOM_ACOUSTICS_KTV = 0x02010100,
  /**
   * The voice effect typical of a concert hall.
   */
  ROOM_ACOUSTICS_VOCAL_CONCERT = 0x02010200,
  /**
   * The voice effect typical of a recording studio.
   */
  ROOM_ACOUSTICS_STUDIO = 0x02010300,
  /**
   * The voice effect typical of a vintage phonograph.
   */
  ROOM_ACOUSTICS_PHONOGRAPH = 0x02010400,
  /**
   * The virtual stereo effect, which renders monophonic audio as stereo audio.
   */
  ROOM_ACOUSTICS_VIRTUAL_STEREO = 0x02010500,
  /**
   * A more spatial voice effect.
   */
  ROOM_ACOUSTICS_SPACIAL = 0x02010600,
  /**
   * A more ethereal voice effect.
   */
  ROOM_ACOUSTICS_ETHEREAL = 0x02010700,
  /**
   * A 3D voice effect that makes the voice appear to be moving around the user. The default cycle
   * period is 10 seconds. After setting this effect, you can call `setAudioEffectParameters` to
   * modify the movement period.
   * @note If the 3D voice effect is enabled, users need to use stereo audio playback devices to hear
   * the anticipated voice effect.
   */
  ROOM_ACOUSTICS_3D_VOICE = 0x02010800,
  /**
   * Virtual surround sound, that is, the SDK generates a simulated surround sound field on the basis
   * of stereo channels, thereby creating a surround sound effect.
   * @note If the virtual surround sound is enabled, users need to use stereo audio playback devices
   * to hear the anticipated audio effect.
   */
  ROOM_ACOUSTICS_VIRTUAL_SURROUND_SOUND = 0x02010900,
  /**
   * The audio effect of chorus. Agora recommends using this effect in chorus scenarios to enhance the
   * sense of depth and dimension in the vocals.
   */
  ROOM_ACOUSTICS_CHORUS = 0x02010D00,
  /**
   * A middle-aged man's voice.
   * @note Agora recommends using this preset to process a male-sounding voice; otherwise, you may not
   * hear the anticipated voice effect.
   */
  VOICE_CHANGER_EFFECT_UNCLE = 0x02020100,
  /**
   * An older man's voice.
   * @note Agora recommends using this preset to process a male-sounding voice; otherwise, you may not
   * hear the anticipated voice effect.
   */
  VOICE_CHANGER_EFFECT_OLDMAN = 0x02020200,
  /**
   * A boy's voice.
   * @note Agora recommends using this preset to process a male-sounding voice; otherwise, you may not
   * hear the anticipated voice effect.
   */
  VOICE_CHANGER_EFFECT_BOY = 0x02020300,
  /**
   * A young woman's voice.
   * @note Agora recommends using this preset to process a female-sounding voice; otherwise, you may
   * not hear the anticipated voice effect.
   */
  VOICE_CHANGER_EFFECT_SISTER = 0x02020400,
  /**
   * A girl's voice.
   * @note Agora recommends using this preset to process a female-sounding voice; otherwise, you may
   * not hear the anticipated voice effect.
   */
  VOICE_CHANGER_EFFECT_GIRL = 0x02020500,
  /**
   * The voice of Pig King, a character in Journey to the West who has a voice like a growling bear.
   */
  VOICE_CHANGER_EFFECT_PIGKING = 0x02020600,
  /**
   * The Hulk's voice.
   */
  VOICE_CHANGER_EFFECT_HULK = 0x02020700,
  /**
   * The voice effect typical of R&B music.
   */
  STYLE_TRANSFORMATION_RNB = 0x02030100,
  /**
   * The voice effect typical of popular music.
   */
  STYLE_TRANSFORMATION_POPULAR = 0x02030200,
  /**
   * A pitch correction effect that corrects the user's pitch based on the pitch of the natural C
   * major scale. After setting this voice effect, you can call `setAudioEffectParameters` to adjust
   * the basic mode of tuning and the pitch of the main tone.
   */
  PITCH_CORRECTION = 0x02040100,

  /** Todo:  Electronic sound, Magic tone haven't been implemented.
   *
   */
};

/**
 * @brief The options for SDK preset voice conversion effects.
 */
enum VOICE_CONVERSION_PRESET {
  /**
   * Turn off voice conversion effects and use the original voice.
   */
  VOICE_CONVERSION_OFF = 0x00000000,
  /**
   * A gender-neutral voice. To avoid audio distortion, ensure that you use this enumerator to process
   * a female-sounding voice.
   */
  VOICE_CHANGER_NEUTRAL = 0x03010100,
  /**
   * A sweet voice. To avoid audio distortion, ensure that you use this enumerator to process a
   * female-sounding voice.
   */
  VOICE_CHANGER_SWEET = 0x03010200,
  /**
   * A steady voice. To avoid audio distortion, ensure that you use this enumerator to process a
   * male-sounding voice.
   */
  VOICE_CHANGER_SOLID = 0x03010300,
  /**
   * A deep voice. To avoid audio distortion, ensure that you use this enumerator to process a
   * male-sounding voice.
   */
  VOICE_CHANGER_BASS = 0x03010400,
  /** A voice like a cartoon character.
   */
  VOICE_CHANGER_CARTOON = 0x03010500,
  /** A voice like a child.
   */
  VOICE_CHANGER_CHILDLIKE = 0x03010600,
  /** A voice like a phone operator.
   */
  VOICE_CHANGER_PHONE_OPERATOR = 0x03010700,
  /** A monster voice.
   */
  VOICE_CHANGER_MONSTER = 0x03010800,
  /** A voice like Transformers.
   */
  VOICE_CHANGER_TRANSFORMERS = 0x03010900,
  /** A voice like Groot.
   */
  VOICE_CHANGER_GROOT = 0x03010A00,
  /** A voice like Darth Vader.
   */
  VOICE_CHANGER_DARTH_VADER = 0x03010B00,
  /** A rough female voice.
   */
  VOICE_CHANGER_IRON_LADY = 0x03010C00,
  /** A voice like Crayon Shin-chan.
   */
  VOICE_CHANGER_SHIN_CHAN = 0x03010D00,
  /** A voice like a castrato.
   */
  VOICE_CHANGER_GIRLISH_MAN = 0x03010E00,
  /** A voice like chipmunk.
   */
  VOICE_CHANGER_CHIPMUNK = 0x03010F00,

};

/**
 * @brief Preset headphone equalizer types.
 */
enum HEADPHONE_EQUALIZER_PRESET {
  /**
   * The headphone equalizer is disabled, and the original audio is heard.
   */
  HEADPHONE_EQUALIZER_OFF = 0x00000000,
  /**
   * An equalizer is used for headphones.
   */
  HEADPHONE_EQUALIZER_OVEREAR = 0x04000001,
  /**
   * An equalizer is used for in-ear headphones.
   */
  HEADPHONE_EQUALIZER_INEAR = 0x04000002
};

/**
 * @brief Voice AI tuner sound types.
 */
enum VOICE_AI_TUNER_TYPE {
  /**
   * 0: Mature male voice. A deep and magnetic male voice.
   */
  VOICE_AI_TUNER_MATURE_MALE,
  /**
   * 1: Fresh male voice. A fresh and slightly sweet male voice.
   */
  VOICE_AI_TUNER_FRESH_MALE,
  /**
   * 2: Elegant female voice. A deep and charming female voice.
   */
  VOICE_AI_TUNER_ELEGANT_FEMALE,
  /**
   * 3: Sweet female voice. A high-pitched and cute female voice.
   */
  VOICE_AI_TUNER_SWEET_FEMALE,
  /**
   * 4: Warm male singing. A warm and melodious male voice.
   */
  VOICE_AI_TUNER_WARM_MALE_SINGING,
  /**
   * 5: Gentle female singing. A soft and delicate female voice.
   */
  VOICE_AI_TUNER_GENTLE_FEMALE_SINGING,
  /**
   * 6: Husky male singing. A unique husky male voice.
   */
  VOICE_AI_TUNER_HUSKY_MALE_SINGING,
  /**
   * 7: Warm elegant female singing. A warm and mature female voice.
   */
  VOICE_AI_TUNER_WARM_ELEGANT_FEMALE_SINGING,
  /**
   * 8: Powerful male singing. A strong and powerful male voice.
   */
  VOICE_AI_TUNER_POWERFUL_MALE_SINGING,
  /**
   * 9: Dreamy female singing. A dreamy and soft female voice.
   */
  VOICE_AI_TUNER_DREAMY_FEMALE_SINGING,
};

/**
 * @brief The audio configuration for the shared screen stream.
 *
 * @details
 * Only available where `captureAudio` is `true`.
 *
 */
struct ScreenAudioParameters {
  /**
   * Audio sample rate (Hz). 
   */
  int sampleRate;
  /**
   * The number of audio channels. The default value is 2, which means stereo.
   */
  int channels;
  /**
   * The volume of the captured system audio. The value range is [0, 100]. The default value is 100.
   */
  int captureSignalVolume;

#if defined(__APPLE__) && !TARGET_OS_IOS
  /**
   * @technical preview
   */
  bool excludeCurrentProcessAudio = true;
  ScreenAudioParameters(): sampleRate(48000), channels(2), captureSignalVolume(100) {}
#else
  ScreenAudioParameters(): sampleRate(16000), channels(2), captureSignalVolume(100) {}
#endif
};

/**
 * @brief Screen sharing configurations.
 */
struct ScreenCaptureParameters {

  /**
   * Determines whether to capture system audio during screen sharing:
   * - `true`: Capture.
   * - `false`: (Default)  Do not capture.
   *
   * @note
   * Due to system limitations, capturing system audio is only available for Android API level 29
   * and later (that is, Android 10 and later).
   */
  bool captureAudio;
  /**
   * The audio configuration for the shared screen stream.
   * @note This parameter only takes effect when `captureAudio` is `true`.
   * See `ScreenAudioParameters`.
   */
  ScreenAudioParameters audioParams;

  /**
   * The video encoding resolution of the screen sharing stream. See `VideoDimensions`. The default
   * value is 1920 × 1080, that is, 2,073,600 pixels. Agora uses the value of this parameter to
   * calculate the charges.
   * If the screen dimensions are different from the value of this parameter, Agora applies the
   * following strategies for encoding. Suppose `dimensions` is set to 1920 × 1080:
   * - If the value of the screen dimensions is lower than that of `dimensions`, for example, 1000 ×
   * 1000 pixels, the SDK uses the screen dimensions, that is, 1000 × 1000 pixels, for encoding.
   * - If the value of the screen dimensions is higher than that of `dimensions`, for example, 2000 ×
   * 1500, the SDK uses the maximum value under `dimensions` with the aspect ratio of the screen
   * dimension (4:3) for encoding, that is, 1440 × 1080.
   * @note
   * When setting the encoding resolution in the scenario of sharing documents (
   * SCREEN_SCENARIO_DOCUMENT ), choose one of the following two methods:
   * - If you require the best image quality, it is recommended to set the encoding resolution to be
   * the same as the capture resolution.
   * - If you wish to achieve a relative balance between image quality, bandwidth, and system
   * performance, then:
   *   - When the capture resolution is greater than 1920 × 1080, it is recommended that the encoding
   * resolution is not less than 1920 × 1080.
   *   - When the capture resolution is less than 1920 × 1080, it is recommended that the encoding
   * resolution is not less than 1280 × 720.
   */
  VideoDimensions dimensions;
  /**
   * On Windows and macOS, this represents the video encoding frame rate (fps) of the screen sharing
   * stream. The frame rate (fps) of the shared region. The default value is 5. Agora does not
   * recommend setting this to a value greater than 15.
   */
  int frameRate;
  /**
   * On Windows and macOS, this represents the video encoding bitrate of the screen sharing stream.
   * The bitrate (Kbps) of the shared region. The default value is 0 (the SDK works out a bitrate
   * according to the dimensions of the current screen).
   */
  int bitrate;
  /**
   * Whether to capture the mouse in screen sharing:
   * - `true`: (Default) Capture the mouse.
   * - `false`: Do not capture the mouse.
   * @note Due to macOS system restrictions, setting this parameter to `false` is ineffective during
   * screen sharing (it has no impact when sharing a window).
   */
  bool captureMouseCursor;
  /**
   * Whether to bring the window to the front when calling the `startScreenCaptureByWindowId` method
   * to share it:
   * - `true`: Bring the window to the front.
   * - `false`: (Default) Do not bring the window to the front.
   * @note Due to macOS system limitations, when setting this member to bring the window to the front,
   * if the current app has multiple windows, only the main window will be brought to the front.
   */
  bool windowFocus;
  /**
   * The ID list of the windows to be blocked. When calling `startScreenCaptureByDisplayId` to start
   * screen sharing, you can use this parameter to block a specified window. When calling
   * `updateScreenCaptureParameters` to update screen sharing configurations, you can use this
   * parameter to dynamically block a specified window.
   */
  view_t* excludeWindowList;
  /**
   * The number of windows to be excluded.
   * @note On the Windows platform, the maximum value of this parameter is 24; if this value is
   * exceeded, excluding the window fails.
   */
  int excludeWindowCount;

  /**
   * (For macOS and Windows only) The width (px) of the border. The default value is 5, and the value
   * range is (0, 50].
   * @note This parameter only takes effect when `highLighted` is set to `true`.
   */
  int highLightWidth;
  /**
   * (For macOS and Windows only)
   * - On Windows platforms, the color of the border in ARGB format. The default value is 0xFF8CBF26.
   * - On macOS, `COLOR_CLASS` refers to `NSColor`.
   */
  unsigned int highLightColor;
  /**
   * (For macOS and Windows only) Whether to place a border around the shared window or screen:
   * - `true`: Place a border.
   * - `false`: (Default) Do not place a border.
   * @note When you share a part of a window or screen, the SDK places a border around the entire
   * window or screen if you set this parameter to `true`.
   */
  bool enableHighLight;

  ScreenCaptureParameters()
      : captureAudio(false),
        dimensions(1920, 1080),
        frameRate(5),
        bitrate(STANDARD_BITRATE),
        captureMouseCursor(true),
        windowFocus(false),
        excludeWindowList(OPTIONAL_NULLPTR),
        excludeWindowCount(0),
        highLightWidth(0),
        highLightColor(0),
        enableHighLight(false) {}
  ScreenCaptureParameters(const VideoDimensions& d, int f, int b)
      : captureAudio(false),dimensions(d),
        frameRate(f),
        bitrate(b),
        captureMouseCursor(true),
        windowFocus(false),
        excludeWindowList(OPTIONAL_NULLPTR),
        excludeWindowCount(0),
        highLightWidth(0),
        highLightColor(0),
        enableHighLight(false) {}
  ScreenCaptureParameters(int width, int height, int f, int b)
      : captureAudio(false),
        dimensions(width, height),
        frameRate(f),
        bitrate(b),
        captureMouseCursor(true),
        windowFocus(false),
        excludeWindowList(OPTIONAL_NULLPTR),
        excludeWindowCount(0),
        highLightWidth(0),
        highLightColor(0),
        enableHighLight(false) {}
  ScreenCaptureParameters(int width, int height, int f, int b, bool cur, bool fcs)
      : captureAudio(false),
        dimensions(width, height),
        frameRate(f),
        bitrate(b),
        captureMouseCursor(cur),
        windowFocus(fcs),
        excludeWindowList(OPTIONAL_NULLPTR),
        excludeWindowCount(0),
        highLightWidth(0),
        highLightColor(0),
        enableHighLight(false) {}
  ScreenCaptureParameters(int width, int height, int f, int b, view_t* ex, int cnt)
      : captureAudio(false),
        dimensions(width, height),
        frameRate(f),
        bitrate(b),
        captureMouseCursor(true),
        windowFocus(false),
        excludeWindowList(ex),
        excludeWindowCount(cnt),
        highLightWidth(0),
        highLightColor(0),
        enableHighLight(false) {}
  ScreenCaptureParameters(int width, int height, int f, int b, bool cur, bool fcs, view_t* ex,
                          int cnt)
      : captureAudio(false),
        dimensions(width, height),
        frameRate(f),
        bitrate(b),
        captureMouseCursor(cur),
        windowFocus(fcs),
        excludeWindowList(ex),
        excludeWindowCount(cnt),
        highLightWidth(0),
        highLightColor(0),
        enableHighLight(false) {}
};

/**
 * @brief Recording quality.
 */
enum AUDIO_RECORDING_QUALITY_TYPE {
  /**
   * 0: Low quality. The sample rate is 32 kHz, and the file size is around 1.2 MB after 10 minutes of
   * recording.
   */
  AUDIO_RECORDING_QUALITY_LOW = 0,
  /**
   * 1: Medium quality. The sample rate is 32 kHz, and the file size is around 2 MB after 10 minutes
   * of recording.
   */
  AUDIO_RECORDING_QUALITY_MEDIUM = 1,
  /**
   * 2: High quality. The sample rate is 32 kHz, and the file size is around 3.75 MB after 10 minutes
   * of recording.
   */
  AUDIO_RECORDING_QUALITY_HIGH = 2,
  /**
   * 3: Ultra high quality. The sample rate is 32 kHz, and the file size is around 7.5 MB after 10
   * minutes of recording.
   */
  AUDIO_RECORDING_QUALITY_ULTRA_HIGH = 3,
};

/**
 * @brief Recording content. Set in `startAudioRecording [3/3]`.
 */
enum AUDIO_FILE_RECORDING_TYPE {
  /**
   * 1: Only records the audio of the local user.
   */
  AUDIO_FILE_RECORDING_MIC = 1,
  /**
   * 2: Only records the audio of all remote users.
   */
  AUDIO_FILE_RECORDING_PLAYBACK = 2,
  /**
   * 3: Records the mixed audio of the local and all remote users.
   */
  AUDIO_FILE_RECORDING_MIXED = 3,
};

/**
 * @brief Audio profile.
 */
enum AUDIO_ENCODED_FRAME_OBSERVER_POSITION {
  /**
   * 1: Only records the audio of the local user.
   */
  AUDIO_ENCODED_FRAME_OBSERVER_POSITION_RECORD = 1,
  /**
   * 2: Only records the audio of all remote users.
   */
  AUDIO_ENCODED_FRAME_OBSERVER_POSITION_PLAYBACK = 2,
  /**
   * 3: Records the mixed audio of the local and all remote users.
   */
  AUDIO_ENCODED_FRAME_OBSERVER_POSITION_MIXED = 3,
};

/**
 * @brief Recording configurations.
 */
struct AudioRecordingConfiguration {
  /**
   * The absolute path (including the filename extensions) of the recording file. For example:
   * `C:\music\audio.aac`.
   * @note Ensure that the directory for the log files exists and is writable.
   */
  const char* filePath;
  /**
   * Whether to encode the audio data:
   * - `true`: Encode audio data in AAC.
   * - `false`: (Default) Do not encode audio data, but save the recorded audio data directly.
   */
  bool encode;
  /**
   * Recording sample rate (Hz).
   * - 16000
   * - (Default) 32000
   * - 44100
   * - 48000
   * @note If you set this parameter to 44100 or 48000, Agora recommends recording WAV files, or AAC
   * files with `quality` set as AUDIO_RECORDING_QUALITY_MEDIUM or AUDIO_RECORDING_QUALITY_HIGH for
   * better recording quality.
   */
  int sampleRate;
  /**
   * The recording content. See `AUDIO_FILE_RECORDING_TYPE`.
   */
  AUDIO_FILE_RECORDING_TYPE fileRecordingType;
  /**
   * Recording quality. See `AUDIO_RECORDING_QUALITY_TYPE`.
   * @note This parameter applies to AAC files only.
   */
  AUDIO_RECORDING_QUALITY_TYPE quality;

  /**
   * The audio channel of recording: The parameter supports the following values:
   * - 1: (Default) Mono.
   * - 2: Stereo.
   * @note
   * The actual recorded audio channel is related to the audio channel that you capture.
   * - If the captured audio is mono and `recordingChannel` is `2`, the recorded audio is the
   * dual-channel data that is copied from mono data, not stereo.
   * - If the captured audio is dual channel and `recordingChannel` is `1`, the recorded audio is the
   * mono data that is mixed by dual-channel data.
   * The integration scheme also affects the final recorded audio channel. If you need to record in
   * stereo, contact `technical support`.
   */
  int recordingChannel;

  AudioRecordingConfiguration()
      : filePath(OPTIONAL_NULLPTR),
        encode(false),
        sampleRate(32000),
        fileRecordingType(AUDIO_FILE_RECORDING_MIXED),
        quality(AUDIO_RECORDING_QUALITY_LOW),
        recordingChannel(1) {}

  AudioRecordingConfiguration(const char* file_path, int sample_rate,
                              AUDIO_RECORDING_QUALITY_TYPE quality_type, int channel)
      : filePath(file_path),
        encode(false),
        sampleRate(sample_rate),
        fileRecordingType(AUDIO_FILE_RECORDING_MIXED),
        quality(quality_type),
        recordingChannel(channel) {}

  AudioRecordingConfiguration(const char* file_path, bool enc, int sample_rate,
                              AUDIO_FILE_RECORDING_TYPE type,
                              AUDIO_RECORDING_QUALITY_TYPE quality_type, int channel)
      : filePath(file_path),
        encode(enc),
        sampleRate(sample_rate),
        fileRecordingType(type),
        quality(quality_type),
        recordingChannel(channel) {}

  AudioRecordingConfiguration(const AudioRecordingConfiguration& rhs)
      : filePath(rhs.filePath),
        encode(rhs.encode),
        sampleRate(rhs.sampleRate),
        fileRecordingType(rhs.fileRecordingType),
        quality(rhs.quality),
        recordingChannel(rhs.recordingChannel) {}
};

/**
 * @brief Observer settings for the encoded audio.
 */
struct AudioEncodedFrameObserverConfig {
  /**
   * Audio profile. See `AUDIO_ENCODED_FRAME_OBSERVER_POSITION`.
   */
  AUDIO_ENCODED_FRAME_OBSERVER_POSITION postionType;
  /**
   * Audio encoding type. See `AUDIO_ENCODING_TYPE`.
   */
  AUDIO_ENCODING_TYPE encodingType;

  AudioEncodedFrameObserverConfig()
      : postionType(AUDIO_ENCODED_FRAME_OBSERVER_POSITION_PLAYBACK),
        encodingType(AUDIO_ENCODING_TYPE_OPUS_48000_MEDIUM) {}
};
/**
 * The encoded audio observer.
 */
class IAudioEncodedFrameObserver {
 public:
  /**
   * @brief Gets the encoded audio data of the local user.
   *
   * @details
   * After calling `registerAudioEncodedFrameObserver` and setting the encoded audio as
   * AUDIO_ENCODED_FRAME_OBSERVER_POSITION_RECORD, you can get the encoded audio data of the local
   * user from this callback.
   *
   * @param frameBuffer The audio buffer.
   * @param length The data length (byte).
   * @param audioEncodedFrameInfo Audio information after encoding. See `EncodedAudioFrameInfo`.
   *
   */
  virtual void onRecordAudioEncodedFrame(const uint8_t* frameBuffer, int length,
                                         const EncodedAudioFrameInfo& audioEncodedFrameInfo) = 0;

  /**
   * @brief Gets the encoded audio data of all remote users.
   *
   * @details
   * After calling `registerAudioEncodedFrameObserver` and setting the encoded audio as
   * AUDIO_ENCODED_FRAME_OBSERVER_POSITION_PLAYBACK, you can get encoded audio data of all remote
   * users through this callback.
   *
   * @param frameBuffer The audio buffer.
   * @param length The data length (byte).
   * @param audioEncodedFrameInfo Audio information after encoding. See `EncodedAudioFrameInfo`.
   *
   */
  virtual void onPlaybackAudioEncodedFrame(const uint8_t* frameBuffer, int length,
                                           const EncodedAudioFrameInfo& audioEncodedFrameInfo) = 0;

  /**
   * @brief Gets the mixed and encoded audio data of the local and all remote users.
   *
   * @details
   * After calling `registerAudioEncodedFrameObserver` and setting the audio profile as
   * AUDIO_ENCODED_FRAME_OBSERVER_POSITION_MIXED, you can get the mixed and encoded audio data of the
   * local and all remote users through this callback.
   *
   * @param frameBuffer The audio buffer.
   * @param length The data length (byte).
   * @param audioEncodedFrameInfo Audio information after encoding. See `EncodedAudioFrameInfo`.
   *
   */
  virtual void onMixedAudioEncodedFrame(const uint8_t* frameBuffer, int length,
                                        const EncodedAudioFrameInfo& audioEncodedFrameInfo) = 0;

  virtual ~IAudioEncodedFrameObserver() {}
};

/**
 * @brief The region for connection, which is the region where the server the SDK connects to is
 * located.
 */
enum AREA_CODE {
  /**
   * Mainland China.
   */
  AREA_CODE_CN = 0x00000001,
  /**
   * North America.
   */
  AREA_CODE_NA = 0x00000002,
  /**
   * Europe.
   */
  AREA_CODE_EU = 0x00000004,
  /**
   * Asia, excluding Mainland China.
   */
  AREA_CODE_AS = 0x00000008,
  /**
   * Japan.
   */
  AREA_CODE_JP = 0x00000010,
  /**
   * India.
   */
  AREA_CODE_IN = 0x00000020,
  /**
   * Global.
   */
  AREA_CODE_GLOB = (0xFFFFFFFF)
};

/**
  Extra region code
  @technical preview
*/
enum AREA_CODE_EX {
    /**
     * Oceania
    */
    AREA_CODE_OC = 0x00000040,
    /**
     * South-American
    */
    AREA_CODE_SA = 0x00000080,
    /**
     * Africa
    */
    AREA_CODE_AF = 0x00000100,
    /**
     * South Korea
     */
    AREA_CODE_KR = 0x00000200,
    /**
     * Hong Kong and Macou
     */
    AREA_CODE_HKMC = 0x00000400,
    /**
     * United States
     */
    AREA_CODE_US = 0x00000800,
    /**
     * Russia
     */
    AREA_CODE_RU = 0x00001000,
    /**
     * The global area (except China)
     */
    AREA_CODE_OVS = 0xFFFFFFFE
};

/**
 * @brief The error code of the channel media relay.
 */
enum CHANNEL_MEDIA_RELAY_ERROR {
  /**
   * 0: No error.
   */
  RELAY_OK = 0,
  /**
   * 1: An error occurs in the server response.
   */
  RELAY_ERROR_SERVER_ERROR_RESPONSE = 1,
  /**
   * 2: No server response.
   * This error may be caused by poor network connections. If this error occurs when initiating a
   * channel media relay, you can try again later; if this error occurs during channel media relay,
   * you can call `leaveChannel(const LeaveChannelOptions& options)` to leave the channel.
   * This error can also occur if the channel media relay service is not enabled in the project. You
   * can contact `technical support` to enable the service.
   */
  RELAY_ERROR_SERVER_NO_RESPONSE = 2,
  /**
   * 3: The SDK fails to access the service, probably due to limited resources of the server.
   */
  RELAY_ERROR_NO_RESOURCE_AVAILABLE = 3,
  /**
   * 4: Fails to send the relay request.
   */
  RELAY_ERROR_FAILED_JOIN_SRC = 4,
  /**
   * 5: Fails to accept the relay request.
   */
  RELAY_ERROR_FAILED_JOIN_DEST = 5,
  /**
   * 6: The server fails to receive the media stream.
   */
  RELAY_ERROR_FAILED_PACKET_RECEIVED_FROM_SRC = 6,
  /**
   * 7: The server fails to send the media stream.
   */
  RELAY_ERROR_FAILED_PACKET_SENT_TO_DEST = 7,
  /**
   * 8: The SDK disconnects from the server due to poor network connections. You can call
   * `leaveChannel(const LeaveChannelOptions& options)` to leave the channel.
   */
  RELAY_ERROR_SERVER_CONNECTION_LOST = 8,
  /**
   * 9: An internal error occurs in the server.
   */
  RELAY_ERROR_INTERNAL_ERROR = 9,
  /**
   * 10: The token of the source channel has expired.
   */
  RELAY_ERROR_SRC_TOKEN_EXPIRED = 10,
  /**
   * 11: The token of the destination channel has expired.
   */
  RELAY_ERROR_DEST_TOKEN_EXPIRED = 11,
};

/**
 * @brief The state code of the channel media relay.
 */
enum CHANNEL_MEDIA_RELAY_STATE {
  /**
   * 0: The initial state. After you successfully stop the channel media relay by calling
   * `stopChannelMediaRelay`, the `onChannelMediaRelayStateChanged` callback returns this state.
   */
  RELAY_STATE_IDLE = 0,
  /**
   * 1: The SDK tries to relay the media stream to the destination channel.
   */
  RELAY_STATE_CONNECTING = 1,
  /**
   * 2: The SDK successfully relays the media stream to the destination channel.
   */
  RELAY_STATE_RUNNING = 2,
  /**
   * 3: An error occurs. See `code` in `onChannelMediaRelayStateChanged` for the error code.
   */
  RELAY_STATE_FAILURE = 3,
};

/**
 * @brief Channel media information.
 */
struct ChannelMediaInfo {
  /**
   * The user ID.
   */
  uid_t uid;
  /**
   * The channel name.
   */
  const char* channelName;
  /**
   * The token that enables the user to join the channel.
   */
  const char* token;

  ChannelMediaInfo() : uid(0), channelName(NULL), token(NULL) {}
  ChannelMediaInfo(const char* c, const char* t, uid_t u) : uid(u), channelName(c), token(t) {}
};

/**
 * @brief Configuration of cross channel media relay.
 */
struct ChannelMediaRelayConfiguration {
  /**
   * The information of the source channel. See `ChannelMediaInfo`. It contains the following members:
   * - `channelName`: The name of the source channel. The default value is `NULL`, which means the SDK
   * applies the name of the current channel.
   * - `token`: The `token` for joining the source channel. This token is generated with the
   * `channelName` and `uid` you set in `srcInfo`.
   *   - If you have not enabled the App Certificate, set this parameter as the default value `NULL`,
   * which means the SDK applies the App ID.
   *   - If you have enabled the App Certificate, you must use the `token` generated with the
   * `channelName` and `uid`, and the `uid` must be set as 0.
   * - `uid`: The unique user ID to identify the relay stream in the source channel. Agora recommends
   * leaving the default value of 0 unchanged.
   */
  ChannelMediaInfo* srcInfo;
  /**
   * The information of the target channel `ChannelMediaInfo`. It contains the following members:
   * - `channelName`: The name of the target channel.
   * - `token`: The `token` for joining the target channel. It is generated with the `channelName` and
   * `uid` you set in `destInfos`.
   *   - If you have not enabled the App Certificate, set this parameter as the default value `NULL`,
   * which means the SDK applies the App ID.
   *   - If you have enabled the App Certificate, you must use the `token` generated with the
   * `channelName` and `uid`.
   * - `uid`: The unique user ID to identify the relay stream in the target channel. The value ranges
   * from 0 to (2 32-1). To avoid user ID conflicts, this user ID must be different from any other
   * user ID in the target channel. The default value is 0, which means the SDK generates a random
   * UID.
   * @note If the token of any target channel expires, the whole media relay stops; hence Agora
   * recommends that you specify the same expiration time for the tokens of all the target channels.
   */
  ChannelMediaInfo* destInfos;
  /**
   * The number of target channels. The default value is 0, and the value range is from 0 to 6. Ensure
   * that the value of this parameter corresponds to the number of `ChannelMediaInfo` structs you
   * define in `destInfo`.
   */
  int destCount;

  ChannelMediaRelayConfiguration()
      : srcInfo(OPTIONAL_NULLPTR), destInfos(OPTIONAL_NULLPTR), destCount(0) {}
};

/**
 * @brief The uplink network information.
 */
struct UplinkNetworkInfo {
  /**
   * The target video encoder bitrate (bps).
   */
  int video_encoder_target_bitrate_bps;

  UplinkNetworkInfo() : video_encoder_target_bitrate_bps(0) {}

  bool operator==(const UplinkNetworkInfo& rhs) const {
    return (video_encoder_target_bitrate_bps == rhs.video_encoder_target_bitrate_bps);
  }
};

struct DownlinkNetworkInfo {
  struct PeerDownlinkInfo {
    /**
     * The ID of the user who owns the remote video stream.
     */
    const char* userId;
    /**
     * The remote video stream type: #VIDEO_STREAM_TYPE.
     */
    VIDEO_STREAM_TYPE stream_type;
    /**
     * The remote video downscale type: #REMOTE_VIDEO_DOWNSCALE_LEVEL.
     */
    REMOTE_VIDEO_DOWNSCALE_LEVEL current_downscale_level;
    /**
     * The expected bitrate in bps.
     */
    int expected_bitrate_bps;

    PeerDownlinkInfo()
        : userId(OPTIONAL_NULLPTR),
          stream_type(VIDEO_STREAM_HIGH),
          current_downscale_level(REMOTE_VIDEO_DOWNSCALE_LEVEL_NONE),
          expected_bitrate_bps(-1) {}

    PeerDownlinkInfo(const PeerDownlinkInfo& rhs)
        : stream_type(rhs.stream_type),
          current_downscale_level(rhs.current_downscale_level),
          expected_bitrate_bps(rhs.expected_bitrate_bps) {
      if (rhs.userId != OPTIONAL_NULLPTR) {
        const size_t len = std::strlen(rhs.userId);
        char* buf = new char[len + 1];
        std::memcpy(buf, rhs.userId, len);
        buf[len] = '\0';
        userId = buf;
      }
    }

    PeerDownlinkInfo& operator=(const PeerDownlinkInfo& rhs) {
      if (this == &rhs) return *this;
      userId = OPTIONAL_NULLPTR;
      stream_type = rhs.stream_type;
      current_downscale_level = rhs.current_downscale_level;
      expected_bitrate_bps = rhs.expected_bitrate_bps;
      if (rhs.userId != OPTIONAL_NULLPTR) {
        const size_t len = std::strlen(rhs.userId);
        char* buf = new char[len + 1];
        std::memcpy(buf, rhs.userId, len);
        buf[len] = '\0';
        userId = buf;
      }
      return *this;
    }

    ~PeerDownlinkInfo() { delete[] userId; }
  };

  /**
   * The lastmile buffer delay queue time in ms.
   */
  int lastmile_buffer_delay_time_ms;
  /**
   * The current downlink bandwidth estimation(bps) after downscale.
   */
  int bandwidth_estimation_bps;
  /**
   * The total video downscale level count.
   */
  int total_downscale_level_count;
  /**
   * The peer video downlink info array.
   */
  PeerDownlinkInfo* peer_downlink_info;
  /**
   * The total video received count.
   */
  int total_received_video_count;

  DownlinkNetworkInfo()
      : lastmile_buffer_delay_time_ms(-1),
        bandwidth_estimation_bps(-1),
        total_downscale_level_count(-1),
        peer_downlink_info(OPTIONAL_NULLPTR),
        total_received_video_count(-1) {}

  DownlinkNetworkInfo(const DownlinkNetworkInfo& info)
      : lastmile_buffer_delay_time_ms(info.lastmile_buffer_delay_time_ms),
        bandwidth_estimation_bps(info.bandwidth_estimation_bps),
        total_downscale_level_count(info.total_downscale_level_count),
        peer_downlink_info(OPTIONAL_NULLPTR),
        total_received_video_count(info.total_received_video_count) {
    if (total_received_video_count <= 0) return;
    peer_downlink_info = new PeerDownlinkInfo[total_received_video_count];
    for (int i = 0; i < total_received_video_count; ++i)
      peer_downlink_info[i] = info.peer_downlink_info[i];
  }

  DownlinkNetworkInfo& operator=(const DownlinkNetworkInfo& rhs) {
    if (this == &rhs) return *this;
    lastmile_buffer_delay_time_ms = rhs.lastmile_buffer_delay_time_ms;
    bandwidth_estimation_bps = rhs.bandwidth_estimation_bps;
    total_downscale_level_count = rhs.total_downscale_level_count;
    peer_downlink_info = OPTIONAL_NULLPTR;
    total_received_video_count = rhs.total_received_video_count;
    if (total_received_video_count > 0) {
      peer_downlink_info = new PeerDownlinkInfo[total_received_video_count];
      for (int i = 0; i < total_received_video_count; ++i)
        peer_downlink_info[i] = rhs.peer_downlink_info[i];
    }
    return *this;
  }

  ~DownlinkNetworkInfo() { delete[] peer_downlink_info; }
};

/**
 * @brief The built-in encryption mode.
 *
 * @details
 * Agora recommends using AES_128_GCM2 or AES_256_GCM2 encrypted mode. These two modes support the
 * use of salt for higher security.
 *
 */
enum ENCRYPTION_MODE {
  /**
   * 1: 128-bit AES encryption, XTS mode.
   */
  AES_128_XTS = 1,
  /**
   * 2: 128-bit AES encryption, ECB mode.
   */
  AES_128_ECB = 2,
  /**
   * 3: 256-bit AES encryption, XTS mode.
   */
  AES_256_XTS = 3,
  /**
   * 4: 128-bit SM4 encryption, ECB mode.
   */
  SM4_128_ECB = 4,
  /**
   * 5: 128-bit AES encryption, GCM mode.
   */
  AES_128_GCM = 5,
  /**
   * 6: 256-bit AES encryption, GCM mode.
   */
  AES_256_GCM = 6,
  /**
   * 7: (Default) 128-bit AES encryption, GCM mode. This encryption mode requires the setting of salt
   * (`encryptionKdfSalt`).
   */
  AES_128_GCM2 = 7,
  /**
   * 8: 256-bit AES encryption, GCM mode. This encryption mode requires the setting of salt
   * (`encryptionKdfSalt`).
   */
  AES_256_GCM2 = 8,
  /**
   * Enumerator boundary.
   */
  MODE_END,
};

/**
 * @brief Built-in encryption configurations.
 */
struct EncryptionConfig {
  /**
   * The built-in encryption mode. See `ENCRYPTION_MODE`. Agora recommends using `AES_128_GCM2` or
   * `AES_256_GCM2` encrypted mode. These two modes support the use of salt for higher security.
   */
  ENCRYPTION_MODE encryptionMode;
  /**
   * Encryption key in string type with unlimited length. Agora recommends using a 32-byte key.
   * @note If you do not set an encryption key or set it as `NULL`, you cannot use the built-in
   * encryption, and the SDK returns `-2`.
   */
  const char* encryptionKey;
  /**
   * Salt, 32 bytes in length. Agora recommends that you use OpenSSL to generate salt on the server
   * side. See Media Stream Encryption for details.
   * @note This parameter takes effect only in `AES_128_GCM2` or `AES_256_GCM2` encrypted mode. In
   * this case, ensure that this parameter is not `0`.
   */
  uint8_t encryptionKdfSalt[32];

  /**
   * Whether to enable data stream encryption:
   * - `true`: Enable data stream encryption.
   * - `false`: (Default) Disable data stream encryption.
   */
  bool datastreamEncryptionEnabled;

  EncryptionConfig()
      : encryptionMode(AES_128_GCM2),
        encryptionKey(OPTIONAL_NULLPTR),
        datastreamEncryptionEnabled(false) {
    memset(encryptionKdfSalt, 0, sizeof(encryptionKdfSalt));
  }

  /// @cond
  const char* getEncryptionString() const {
    switch (encryptionMode) {
      case AES_128_XTS:
        return "aes-128-xts";
      case AES_128_ECB:
        return "aes-128-ecb";
      case AES_256_XTS:
        return "aes-256-xts";
      case SM4_128_ECB:
        return "sm4-128-ecb";
      case AES_128_GCM:
        return "aes-128-gcm";
      case AES_256_GCM:
        return "aes-256-gcm";
      case AES_128_GCM2:
        return "aes-128-gcm-2";
      case AES_256_GCM2:
        return "aes-256-gcm-2";
      default:
        return "aes-128-gcm-2";
    }
    return "aes-128-gcm-2";
  }
  /// @endcond
};

/**
 * @brief Encryption error type.
 */
enum ENCRYPTION_ERROR_TYPE {
  /**
   * 0: Internal reason.
   */
  ENCRYPTION_ERROR_INTERNAL_FAILURE = 0,
  /**
   * 1: Media stream decryption error. Ensure that the receiver and the sender use the same encryption
   * mode and key.
   */
  ENCRYPTION_ERROR_DECRYPTION_FAILURE = 1,
  /**
   * 2: Media stream encryption error.
   */
  ENCRYPTION_ERROR_ENCRYPTION_FAILURE = 2,
  /**
   * 3: Data stream decryption error. Ensure that the receiver and the sender use the same encryption
   * mode and key.
   */
  ENCRYPTION_ERROR_DATASTREAM_DECRYPTION_FAILURE = 3,
  /**
   * 4: Data stream encryption error.
   */
  ENCRYPTION_ERROR_DATASTREAM_ENCRYPTION_FAILURE = 4,
};

enum UPLOAD_ERROR_REASON {
  UPLOAD_SUCCESS = 0,
  UPLOAD_NET_ERROR = 1,
  UPLOAD_SERVER_ERROR = 2,
};

/**
 * @brief Represents the error codes after calling `renewToken`.
 *
 * @since 4.6.0
 */
enum RENEW_TOKEN_ERROR_CODE {
  /**
   * (0): Token updated successfully.
   */
  RENEW_TOKEN_SUCCESS = 0,
  /**
   * (1): Token update failed due to an unknown server error. It is recommended to check the
   * parameters used to generate the Token, regenerate the Token, and retry `renewToken`.
   */
  RENEW_TOKEN_FAILURE = 1,
  /**
   * (2): Token update failed because the provided Token has expired. It is recommended to generate a
   * new Token with a longer expiration time and retry `renewToken`.
   */
  RENEW_TOKEN_TOKEN_EXPIRED = 2,
  /**
   * (3): Token update failed because the provided Token is invalid. Common reasons include: the
   * project has enabled App Certificate in the Agora Console but did not use a Token when joining the
   * channel; the uid specified in `joinChannel` is inconsistent with the uid used when generating the
   * Token; the channel name specified in `joinChannel` is inconsistent with the one used when
   * generating the Token. It is recommended to check the Token generation process, generate a new
   * Token, and retry `renewToken`.
   */
  RENEW_TOKEN_INVALID_TOKEN = 3,
  /**
   * (4): Token update failed because the channel name in the Token does not match the current
   * channel. It is recommended to check the channel name, generate a new Token, and retry
   * `renewToken`.
   */
  RENEW_TOKEN_INVALID_CHANNEL_NAME = 4,
  /**
   * (5): Token update failed because the App ID in the Token does not match the current App ID. It is
   * recommended to check the App ID, generate a new Token, and retry `renewToken`.
   */
  RENEW_TOKEN_INCONSISTENT_APPID = 5,
  /**
   * (6): The previous Token update request was canceled due to a new request being initiated.
   */
  RENEW_TOKEN_CANCELED_BY_NEW_REQUEST = 6,
};

/**
 * @brief The type of the device permission.
 */
enum PERMISSION_TYPE {
  /**
   * 0: Permission for the audio capture device.
   */
  RECORD_AUDIO = 0,
  /**
   * 1: Permission for the camera.
   */
  CAMERA = 1,

  /**
   * (For Android only) 2: Permission for screen sharing.
   */
  SCREEN_CAPTURE = 2,
};

/**
 * @brief The subscribing state.
 */
enum STREAM_SUBSCRIBE_STATE {
  /**
   * 0: The initial publishing state after joining the channel.
   */
  SUB_STATE_IDLE = 0,
  /**
   * 1: Fails to subscribe to the remote stream. Possible reasons:
   * - The remote user:
   *   - Calls `muteLocalAudioStream` (`true`) or `muteLocalVideoStream` (`true`) to stop sending
   * local media stream.
   *   - Calls `disableAudio` or `disableVideo` to disable the local audio or video module.
   *   - Calls `enableLocalAudio` ( false ) or `enableLocalVideo` ( false ) to disable local audio or
   * video capture.
   *   - The role of the remote user is audience.
   * - The local user calls the following methods to stop receiving remote streams:
   *   - Call `muteRemoteAudioStream` ( true ) or `muteAllRemoteAudioStreams` ( true ) to stop
   * receiving the remote audio stream.
   *   - Call `muteRemoteVideoStream` ( true ) or `muteAllRemoteVideoStreams` ( true ) to stop
   * receiving the remote video stream.
   */
  SUB_STATE_NO_SUBSCRIBED = 1,
  /**
   * 2: Subscribing.
   */
  SUB_STATE_SUBSCRIBING = 2,
  /**
   * 3: The remote stream is received, and the subscription is successful.
   */
  SUB_STATE_SUBSCRIBED = 3
};

/**
 * @brief The publishing state.
 */
enum STREAM_PUBLISH_STATE {
  /**
   * 0: The initial publishing state after joining the channel.
   */
  PUB_STATE_IDLE = 0,
  /**
   * 1: Fails to publish the local stream. Possible reasons:
   * - The local user calls `muteLocalAudioStream` (`true`) or `muteLocalVideoStream` (`true`) to stop
   * sending local media streams.
   * - The local user calls `disableAudio` or `disableVideo` to disable the local audio or video
   * module.
   * - The local user calls `enableLocalAudio` (`false`) or `enableLocalVideo` (`false`) to disable
   * the local audio or video capture.
   * - The role of the local user is audience.
   */
  PUB_STATE_NO_PUBLISHED = 1,
  /**
   * 2: Publishing.
   */
  PUB_STATE_PUBLISHING = 2,
  /**
   * 3: Publishes successfully.
   */
  PUB_STATE_PUBLISHED = 3
};

/**
 * @brief The configuration of the audio and video call loop test.
 */
struct EchoTestConfiguration {
  /**
   * The view used to render the local user's video. This parameter is only applicable to scenarios
   * testing video devices, that is, when `enableVideo` is true.
   */
  view_t view;
  /**
   * Whether to enable the audio device for the loop test:
   * - `true`: (Default) Enable the audio device. To test the audio device, set this parameter as
   * true.
   * - `false`: Disable the audio device.
   */
  bool enableAudio;
  /**
   * Whether to enable the video device for the loop test:
   * - `true`: (Default) Enable the video device. To test the video device, set this parameter as
   * true.
   * - `false`: Disable the video device.
   */
  bool enableVideo;
  /**
   * The token used to secure the audio and video call loop test. If you do not enable App Certificate
   * in Agora Console, you do not need to pass a value in this parameter; if you have enabled App
   * Certificate in Agora Console, you must pass a token in this parameter; the `uid` used when you
   * generate the token must be 0xFFFFFFFF, and the channel name used must be the channel name that
   * identifies each audio and video call loop tested. For server-side token generation, see .
   */
  const char* token;
  /**
   * The channel name that identifies each audio and video call loop. To ensure proper loop test
   * functionality, the channel name passed in to identify each loop test cannot be the same when
   * users of the same project (App ID) perform audio and video call loop tests on different devices.
   */
  const char* channelId;
  /**
   * Set the time interval or delay for returning the results of the audio and video loop test. The
   * value range is [2,10], in seconds, with the default value being 2 seconds.
   * - For audio loop tests, the test results will be returned according to the time interval you set.
   * - For video loop tests, the video will be displayed in a short time, after which the delay will
   * gradually increase until it reaches the delay you set.
   */
  int intervalInSeconds;

  EchoTestConfiguration(view_t v, bool ea, bool ev, const char* t, const char* c, const int is)
      : view(v), enableAudio(ea), enableVideo(ev), token(t), channelId(c), intervalInSeconds(is) {}

  EchoTestConfiguration()
      : view(OPTIONAL_NULLPTR),
        enableAudio(true),
        enableVideo(true),
        token(OPTIONAL_NULLPTR),
        channelId(OPTIONAL_NULLPTR),
        intervalInSeconds(2) {}
};

/**
 * @brief The information of the user.
 */
struct UserInfo {
  /**
   * The user ID.
   */
  uid_t uid;
  /**
   * User account. The maximum data length is `MAX_USER_ACCOUNT_LENGTH_TYPE`.
   */
  char userAccount[MAX_USER_ACCOUNT_LENGTH];

  UserInfo() : uid(0) { userAccount[0] = '\0'; }
};

/**
 * @brief The audio filter types of in-ear monitoring.
 */
enum EAR_MONITORING_FILTER_TYPE {
  /**
   * 1<<0: No audio filter added to in-ear monitoring.
   */
  EAR_MONITORING_FILTER_NONE = (1 << 0),
  /**
   * 1<<1: Add vocal effects audio filter to in-ear monitoring. If you implement functions such as
   * voice beautifier and audio effect, users can hear the voice after adding these effects.
   */
  EAR_MONITORING_FILTER_BUILT_IN_AUDIO_FILTERS = (1 << 1),
  /**
   * 1<<2: Add noise suppression audio filter to in-ear monitoring.
   */
  EAR_MONITORING_FILTER_NOISE_SUPPRESSION = (1 << 2),
  /**
   * 1<<15: Reuse the audio filter that has been processed on the sending end for in-ear monitoring.
   * This enumerator reduces CPU usage while increasing in-ear monitoring latency, which is suitable
   * for latency-tolerant scenarios requiring low CPU consumption.
   */
  EAR_MONITORING_FILTER_REUSE_POST_PROCESSING_FILTER = (1 << 15),
};

/**
 * Thread priority type.
 */
enum THREAD_PRIORITY_TYPE {
  /**
   * 0: Lowest priority.
   */
  LOWEST = 0,
  /**
   * 1: Low priority.
   */
  LOW = 1,
  /**
   * 2: Normal priority.
   */
  NORMAL = 2,
  /**
   * 3: High priority.
   */
  HIGH = 3,
  /**
   * 4. Highest priority.
   */
  HIGHEST = 4,
  /**
   * 5. Critical priority.
   */
  CRITICAL = 5,
};

#if defined(__ANDROID__) || (defined(__APPLE__) && TARGET_OS_IOS) || defined(__OHOS__)

/**
 * @brief The video configuration for the shared screen stream.
 */
struct ScreenVideoParameters {
  /**
   * The video encoding dimension. The default value is 1280 × 720.
   */
  VideoDimensions dimensions;
  /**
   * The video encoding frame rate (fps). The default value is 15.
   */
  int frameRate = 15;
  /**
   * The video encoding bitrate (Kbps).
   */
  int bitrate;
  /**
   * The content hint for screen sharing. See `VIDEO_CONTENT_HINT`.
   */
  VIDEO_CONTENT_HINT contentHint = VIDEO_CONTENT_HINT::CONTENT_HINT_MOTION;

  ScreenVideoParameters() : dimensions(1280, 720) {}
};

/**
 * @brief Screen sharing configurations.
 */
struct ScreenCaptureParameters2 {
  /**
   * Determines whether to capture system audio during screen sharing:
   * - `true`: Capture system audio.
   * - `false`: (Default) Do not capture system audio.
   * @note
   * - Due to system limitations, capturing system audio is only applicable to Android API level 29
   * and later (that is, Android 10 and later).
   * - To improve the success rate of capturing system audio during screen sharing, ensure that you
   * have called the `setAudioScenario` method and set the audio scenario to
   * `AUDIO_SCENARIO_GAME_STREAMING`.
   */
  bool captureAudio = false;
  /**
   * The audio configuration for the shared screen stream. See `ScreenAudioParameters`.
   * @note This parameter only takes effect when `captureAudio` is `true`.
   */
  ScreenAudioParameters audioParams;
  /**
   * Whether to capture the screen when screen sharing:
   * - `true`: (Default) Capture the screen.
   * - `false`: Do not capture the screen.
   * @note Due to system limitations, the capture screen is only applicable to Android API level 21
   * and above, that is, Android 5 and above.
   */
  bool captureVideo = true;
  /**
   * The video configuration for the shared screen stream. See `ScreenVideoParameters`.
   * @note This parameter only takes effect when `captureVideo` is `true`.
   */
  ScreenVideoParameters videoParams;
};
#endif

/**
 * @brief The rendering state of the media frame.
 */
enum MEDIA_TRACE_EVENT {
  /**
   * 0: The video frame has been rendered.
   */
  MEDIA_TRACE_EVENT_VIDEO_RENDERED = 0,
  /**
   * 1: The video frame has been decoded.
   */
  MEDIA_TRACE_EVENT_VIDEO_DECODED,
};

/**
 * @brief Indicators during video frame rendering progress.
 */
struct VideoRenderingTracingInfo {
  /**
   * The time interval (ms) from `startMediaRenderingTracing` to SDK triggering the
   * `onVideoRenderingTracingResult` callback. Agora recommends you call `startMediaRenderingTracing`
   * before joining a channel.
   */
  int elapsedTime;
  /**
   * The time interval (ms) from `startMediaRenderingTracing` to `joinChannel(const char* token, const
   * char* channelId, const char* info, uid_t uid)` or `joinChannel(const char* token, const char*
   * channelId, uid_t uid, const ChannelMediaOptions& options)`
   * . A negative number indicates that `startMediaRenderingTracing` is called after calling
   * `joinChannel(const char* token, const char* channelId, uid_t uid, const ChannelMediaOptions&
   * options)`.
   */
  int start2JoinChannel;
  /**
   * The time interval (ms) from `joinChannel(const char* token, const char* channelId, const char*
   * info, uid_t uid)` or `joinChannel(const char* token, const char* channelId, uid_t uid, const
   * ChannelMediaOptions& options)` to successfully joining
   * the channel.
   */
  int join2JoinSuccess;
  /**
   * - If the local user calls `startMediaRenderingTracing` before successfully joining the channel,
   * this value is the time interval (ms) from the local user successfully joining the channel to the
   * remote user joining the channel.
   * - If the local user calls `startMediaRenderingTracing` after successfully joining the channel,
   * the value is the time interval (ms) from `startMediaRenderingTracing` to when the remote user
   * joins the channel.
   * @note
   * - If the local user calls `startMediaRenderingTracing` after the remote user joins the channel,
   * the value is 0 and meaningless.
   * - In order to reduce the time of rendering the first frame for remote users, Agora recommends
   * that the local user joins the channel when the remote user is in the channel to reduce this
   * value.
   */
  int joinSuccess2RemoteJoined;
  /**
   * - If the local user calls `startMediaRenderingTracing` before the remote user joins the channel,
   * this value is the time interval (ms) from when the remote user joins the channel to when the
   * local user sets the remote view.
   * - If the local user calls `startMediaRenderingTracing` after the remote user joins the channel,
   * this value is the time interval (ms) from calling `startMediaRenderingTracing` to setting the
   * remote view.
   * @note
   * - If the local user calls `startMediaRenderingTracing` after setting the remote view, the value
   * is 0 and has no effect.
   * - In order to reduce the time of rendering the first frame for remote users, Agora recommends
   * that the local user sets the remote view before the remote user joins the channel, or sets the
   * remote view immediately after the remote user joins the channel to reduce this value.
   */
  int remoteJoined2SetView;
  /**
   * - If the local user calls `startMediaRenderingTracing` before the remote user joins the channel,
   * this value is the time interval (ms) from the remote user joining the channel to subscribing to
   * the remote video stream.
   * - If the local user calls `startMediaRenderingTracing` after the remote user joins the channel,
   * this value is the time interval (ms) from `startMediaRenderingTracing` to subscribing to the
   * remote video stream.
   * @note
   * - If the local user calls `startMediaRenderingTracing` after subscribing to the remote video
   * stream, the value is 0 and has no effect.
   * - In order to reduce the time of rendering the first frame for remote users, Agora recommends
   * that after the remote user joins the channel, the local user immediately subscribes to the remote
   * video stream to reduce this value.
   */
  int remoteJoined2UnmuteVideo;
  /**
   * - If the local user calls `startMediaRenderingTracing` before the remote user joins the channel,
   * this value is the time interval (ms) from when the remote user joins the channel to when the
   * local user receives the remote video stream.
   * - If the local user calls `startMediaRenderingTracing` after the remote user joins the channel,
   * this value is the time interval (ms) from `startMediaRenderingTracing` to receiving the remote
   * video stream.
   * @note
   * - If the local user calls `startMediaRenderingTracing` after receiving the remote video stream,
   * the value is 0 and has no effect.
   * - In order to reduce the time of rendering the first frame for remote users, Agora recommends
   * that the remote user publishes video streams immediately after joining the channel, and the local
   * user immediately subscribes to remote video streams to reduce this value.
   */
  int remoteJoined2PacketReceived;
};

enum CONFIG_FETCH_TYPE {
  /**
   * 1: Fetch config when initializing RtcEngine, without channel info.
   */
  CONFIG_FETCH_TYPE_INITIALIZE = 1,
  /**
   * 2: Fetch config when joining channel with channel info, such as channel name and uid.
   */
  CONFIG_FETCH_TYPE_JOIN_CHANNEL = 2,
};

/**
 * @brief Connection mode with the Agora Private Media Server.
 */
enum LOCAL_PROXY_MODE {
  /**
   * 0: The SDK first tries to connect to the specified Agora Private Media Server; if it fails, it
   * connects to the Agora SD-RTN™.
   */
  ConnectivityFirst = 0,
  /**
   * 1: The SDK only tries to connect to the specified Agora Private Media Server.
   */
  LocalOnly = 1,
};

/**
 * @brief Configuration information for the log server.
 */
struct LogUploadServerInfo {
  /**
   * Domain name of the log server.
   */
  const char* serverDomain;
  /**
   * Storage path for logs on the server.
   */
  const char* serverPath;
  /**
   * Port of the log server.
   */
  int serverPort;
  /**
   * Whether the log server uses HTTPS protocol:
   * - `true`: Uses HTTPS.
   * - `false`: Uses HTTP.
   */
  bool serverHttps;

  LogUploadServerInfo() : serverDomain(NULL), serverPath(NULL), serverPort(0), serverHttps(true) {}

  LogUploadServerInfo(const char* domain, const char* path, int port, bool https)
      : serverDomain(domain), serverPath(path), serverPort(port), serverHttps(https) {}
};

/**
 * @brief Advanced options for the Local Access Point.
 */
struct AdvancedConfigInfo {
  /**
   * Custom log upload server. By default, the SDK uploads logs to the Agora log server. You can use
   * this parameter to change the log upload server. See `LogUploadServerInfo`.
   */
  LogUploadServerInfo logUploadServer;
};

/**
 * @brief Configuration for the Local Access Point.
 */
struct LocalAccessPointConfiguration {
  /**
   * Internal IP address list of the Local Access Point. Either ipList or domainList must be
   * specified.
   */
  const char** ipList;
  /**
   * Number of internal IP addresses for the Local Access Point. This value must match the number of
   * IP addresses you provide.
   */
  int ipListSize;
  /**
   * Domain name list of the Local Access Point. The SDK resolves the IP addresses of the Local Access
   * Point from the provided domain names. The DNS resolution timeout is 10 seconds. Either ipList or
   * domainList must be specified. If you specify both IP addresses and domain names, the SDK merges
   * and deduplicates the resolved IP addresses and the specified IP addresses, then randomly selects
   * one for load balancing.
   */
  const char** domainList;
  /**
   * Number of domain names for the Local Access Point. This value must match the number of domain
   * names you provide.
   */
  int domainListSize;
  /**
   * Domain name for internal certificate verification. If left empty, the SDK uses the default domain
   * name `secure-edge.local` for certificate verification.
   */
  const char* verifyDomainName;
  /**
   * Connection mode. See `LOCAL_PROXY_MODE`.
   */
  LOCAL_PROXY_MODE mode;
  /**
   * Advanced options for the Local Access Point. See `AdvancedConfigInfo`.
   */
  AdvancedConfigInfo advancedConfig;
  /**
    * Whether to disable vos-aut:
    - true: (Default)disable vos-aut.
    - false: not disable vos-aut
  */
  bool disableAut;
  LocalAccessPointConfiguration()
      : ipList(NULL),
        ipListSize(0),
        domainList(NULL),
        domainListSize(0),
        verifyDomainName(NULL),
        mode(ConnectivityFirst),
        disableAut(true) {}
};

/**
 * @brief Type of video stream to be recorded.
 */
enum RecorderStreamType {
  /**
   * 0: (Default) Video stream in the channel.
   */
  RTC,
  /**
   * 1: Local preview video stream before joining the channel.
   */
  PREVIEW,
};

/**
 * @brief The information about the media streams to be recorded.
 */
struct RecorderStreamInfo {
  /**
   * The name of the channel in which the media streams publish.
   */
  const char* channelId;
  /**
   * The ID of the user whose media streams you want to record.
   */
  uid_t uid;
  /**
   * The Recoder Stream type.
   */
  RecorderStreamType type;
  RecorderStreamInfo() : channelId(NULL), uid(0), type(RTC) {}
  RecorderStreamInfo(const char* channelId, uid_t uid)
      : channelId(channelId), uid(uid), type(RTC) {}
  RecorderStreamInfo(const char* channelId, uid_t uid, RecorderStreamType type)
      : channelId(channelId), uid(uid), type(type) {}
};

/**
 * @brief Reliable Data Transmission Tunnel message stream type
 *
 * @technical preview
 */
enum RdtStreamType {
  /**
   * Command stream type.
   * Characterized by: reliability, high priority, and not affected by congestion control.
   * Transmission limits: a maximum of 256 bytes per packet, and 100 packets per second.
   */
  RDT_STREAM_CMD,
  /**
   * Data stream type.
   * Characterized by: reliability, low priority, and affected by congestion control.
   * Transmission limits: a maximum of 128 KBytes per packet, with a rate of 4 Mbps.
   */
  RDT_STREAM_DATA,
  /**
   * Reliable Data Transmission stream type count
   */
  RDT_STREAM_COUNT,
};

/**
 * @brief Reliable Data Transmission tunnel state
 *
 * @technical preview
 */
enum RdtState {
  /**
   * The RDT tunnel is in the initial or is closed.
   */
  RDT_STATE_CLOSED,
  /**
   * The RDT tunnel is open, and data can only be sent in this state.
   */
  RDT_STATE_OPENED,
  /**
   * The send buffer of the RDT tunnel is full. RDT_STREAM_DATA cannot be sent,
   * but RDT_STREAM_CMD can be sent, as the latter is not affected by congestion control.
   */
  RDT_STATE_BLOCKED,
  /**
   * The RDT tunnel is in a suspended state because SDK has disconnected.
   * It will automatically resume to the RDT_STATE_OPENED state after rejoining the channel.
   */
  RDT_STATE_PENDING,
  /**
   * The RDT channel is broken, and the data being sent and received will be cleared.
   * It will automatically resume to the RDT_STATE_OPENED state later.
   * Reason for occurrence: The remote user actively called the API to leave the
   * channel and then rejoined the channel, without being detected by this end.
   */
  RDT_STATE_BROKEN,
};
}  // namespace rtc

namespace base {

class IEngineBase {
 public:
  virtual int queryInterface(rtc::INTERFACE_ID_TYPE iid, void** inter) = 0;
  virtual ~IEngineBase() {}
};

class AParameter : public agora::util::AutoPtr<IAgoraParameter> {
 public:
  AParameter(IEngineBase& engine) { initialize(&engine); }
  AParameter(IEngineBase* engine) { initialize(engine); }
  AParameter(IAgoraParameter* p) : agora::util::AutoPtr<IAgoraParameter>(p) {}

 private:
  bool initialize(IEngineBase* engine) {
    IAgoraParameter* p = OPTIONAL_NULLPTR;
    if (engine && !engine->queryInterface(rtc::AGORA_IID_PARAMETER_ENGINE, (void**)&p)) reset(p);
    return p != OPTIONAL_NULLPTR;
  }
};

class LicenseCallback {
 public:
  virtual ~LicenseCallback() {}
  virtual void onCertificateRequired() = 0;
  virtual void onLicenseRequest() = 0;
  virtual void onLicenseValidated() = 0;
  virtual void onLicenseError(int result) = 0;
};

}  // namespace base

/**
 * @brief The spatial audio parameters.
 */
struct SpatialAudioParams {
  /**
   * The azimuth angle of the remote user or media player relative to the local user. The value range
   * is [0,360], and the unit is degrees, The values are as follows:
   * - 0: (Default) 0 degrees, which means directly in front on the horizontal plane.
   * - 90: 90 degrees, which means directly to the left on the horizontal plane.
   * - 180: 180 degrees, which means directly behind on the horizontal plane.
   * - 270: 270 degrees, which means directly to the right on the horizontal plane.
   * - 360: 360 degrees, which means directly in front on the horizontal plane.
   */
  Optional<double> speaker_azimuth;
  /**
   * The elevation angle of the remote user or media player relative to the local user. The value
   * range is [-90,90], and the unit is degrees, The values are as follows:
   * - 0: (Default) 0 degrees, which means that the horizontal plane is not rotated.
   * - -90: -90 degrees, which means that the horizontal plane is rotated 90 degrees downwards.
   * - 90: 90 degrees, which means that the horizontal plane is rotated 90 degrees upwards.
   */
  Optional<double> speaker_elevation;
  /**
   * The distance of the remote user or media player relative to the local user. The value range is
   * [1,50], and the unit is meters. The default value is 1 meter.
   */
  Optional<double> speaker_distance;
  /**
   * The orientation of the remote user or media player relative to the local user. The value range is
   * [0,180], and the unit is degrees, The values are as follows:
   * - 0: (Default) 0 degrees, which means that the sound source and listener face the same direction.
   * - 180: 180 degrees, which means that the sound source and listener face each other.
   */
  Optional<int> speaker_orientation;
  /**
   * Whether to enable audio blurring:
   * - `true`: Enable audio blurring.
   * - `false`: (Default) Disable audio blurring.
   */
  Optional<bool> enable_blur;
  /**
   * Whether to enable air absorption, that is, to simulate the sound attenuation effect of sound
   * transmitting in the air; under a certain transmission distance, the attenuation speed of
   * high-frequency sound is fast, and the attenuation speed of low-frequency sound is slow.
   * - `true`: (Default) Enable air absorption. Make sure that the value of `speaker_attenuation` is
   * not `0`; otherwise, this setting does not take effect.
   * - `false`: Disable air absorption.
   */
  Optional<bool> enable_air_absorb;
  /**
   * The sound attenuation coefficient of the remote user or media player. The value range is [0,1].
   * The values are as follows:
   * - 0: Broadcast mode, where the volume and timbre are not attenuated with distance, and the volume
   * and timbre heard by local users do not change regardless of distance.
   * - (0,0.5): Weak attenuation mode, where the volume and timbre only have a weak attenuation during
   * the propagation, and the sound can travel farther than that in a real environment.
   * `enable_air_absorb` needs to be enabled at the same time.
   * - 0.5: (Default) Simulates the attenuation of the volume in the real environment; the effect is
   * equivalent to not setting the `speaker_attenuation` parameter.
   * - (0.5,1]: Strong attenuation mode, where volume and timbre attenuate rapidly during the
   * propagation. `enable_air_absorb` needs to be enabled at the same time.
   */
  Optional<double> speaker_attenuation;
  /**
   * Whether to enable the Doppler effect: When there is a relative displacement between the sound
   * source and the receiver of the sound source, the tone heard by the receiver changes.
   * - `true`: Enable the Doppler effect.
   * - `false`: (Default) Disable the Doppler effect.
   * @note
   * - This parameter is suitable for scenarios where the sound source is moving at high speed (for
   * example, racing games). It is not recommended for common audio and video interactive scenarios
   * (for example, voice chat, co-streaming, or online KTV).
   * - When this parameter is enabled, Agora recommends that you set a regular period (such as 30 ms),
   * and then call the `updatePlayerPositionInfo`, `updateSelfPosition`, and `updateRemotePosition`
   * methods to continuously update the relative distance between the sound source and the receiver.
   * The following factors can cause the Doppler effect to be unpredictable or the sound to be
   * jittery: the period of updating the distance is too long, the updating period is irregular, or
   * the distance information is lost due to network packet loss or delay.
   */
  Optional<bool> enable_doppler;
};
/**
 * @brief Layout information of a specific sub-video stream within the mixed stream.
 */
struct VideoLayout {
  /**
   * The channel name to which the sub-video stream belongs.
   */
  const char* channelId;
  /**
   * User ID who published this sub-video stream.
   */
  rtc::uid_t uid;
  /**
   * Reserved for future use.
   */
  user_id_t strUid;
  /**
   * X-coordinate (px) of the sub-video stream on the mixing canvas. The relative lateral displacement
   * of the top left corner of the video for video mixing to the origin (the top left corner of the
   * canvas).
   */
  uint32_t x;
  /**
   * Y-coordinate (px) of the sub-video stream on the mixing canvas. The relative longitudinal
   * displacement of the top left corner of the captured video to the origin (the top left corner of
   * the canvas).
   */
  uint32_t y;
  /**
   * Width (px) of the sub-video stream.
   */
  uint32_t width;
  /**
   * Heitht (px) of the sub-video stream.
   */
  uint32_t height;
  /**
   * Status of the sub-video stream on the video mixing canvas.
   * - 0: Normal. The sub-video stream has been rendered onto the mixing canvas.
   * - 1: Placeholder image. The sub-video stream has no video frames and is displayed as a
   * placeholder on the mixing canvas.
   * - 2: Black image. The sub-video stream is replaced by a black image.
   */
  uint32_t videoState;

  VideoLayout()
      : channelId(OPTIONAL_NULLPTR),
        uid(0),
        strUid(OPTIONAL_NULLPTR),
        x(0),
        y(0),
        width(0),
        height(0),
        videoState(0) {}
};
}  // namespace agora

/**
 * Gets the version of the SDK.
 * @param [out] build The build number of Agora SDK.
 * @return The string of the version of the SDK.
 */
AGORA_API const char* AGORA_CALL getAgoraSdkVersion(int* build);

/**
 * Gets error description of an error code.
 * @param [in] err The error code.
 * @return The description of the error code.
 */
AGORA_API const char* AGORA_CALL getAgoraSdkErrorDescription(int err);

AGORA_API int AGORA_CALL setAgoraSdkExternalSymbolLoader(void* (*func)(const char* symname));

/**
 * Generate credential
 * @param [in, out] credential The content of the credential.
 * @return The description of the error code.
 * @note For license only, everytime will generate a different credential.
 * So, just need to call once for a device, and then save the credential
 */
AGORA_API int AGORA_CALL createAgoraCredential(agora::util::AString& credential);

/**
 * Verify given certificate and return the result
 * When you receive onCertificateRequired event, you must validate the certificate by calling
 * this function. This is sync call, and if validation is success, it will return ERR_OK. And
 * if failed to pass validation, you won't be able to joinChannel and ERR_CERT_FAIL will be
 * returned.
 * @param [in] credential_buf pointer to the credential's content.
 * @param [in] credential_len the length of the credential's content.
 * @param [in] certificate_buf pointer to the certificate's content.
 * @param [in] certificate_len the length of the certificate's content.
 * @return The description of the error code.
 * @note For license only.
 */
AGORA_API int AGORA_CALL getAgoraCertificateVerifyResult(const char* credential_buf,
                                                         int credential_len,
                                                         const char* certificate_buf,
                                                         int certificate_len);

/**
 * @brief Implement the agora::base::LicenseCallback,
 * create a LicenseCallback object to receive callbacks of license.
 *
 * @param [in] callback The object of agora::LiceseCallback,
 *                      set the callback to null before delete it.
 */
AGORA_API void setAgoraLicenseCallback(agora::base::LicenseCallback* callback);

/**
 * @brief Gets the LicenseCallback pointer if already setup,
 *  otherwise, return null.
 *
 * @return a pointer of agora::base::LicenseCallback
 */

AGORA_API agora::base::LicenseCallback* getAgoraLicenseCallback();

/*
 * Get monotonic time in ms which can be used by capture time,
 * typical scenario is as follows:
 *
 *  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  |  // custom audio/video base capture time, e.g. the first audio/video capture time. | | int64_t
 * custom_capture_time_base;                                                             | | | |
 * int64_t agora_monotonic_time = getAgoraCurrentMonotonicTimeInMs();                            |
 *  | | |  // offset is fixed once calculated in the begining. | |  const int64_t offset =
 * agora_monotonic_time - custom_capture_time_base;                       | | | |  //
 * realtime_custom_audio/video_capture_time is the origin capture time that customer provided.| | //
 * actual_audio/video_capture_time is the actual capture time transfered to sdk.              | |
 * int64_t actual_audio_capture_time = realtime_custom_audio_capture_time + offset;              |
 *  |  int64_t actual_video_capture_time = realtime_custom_video_capture_time + offset; |
 *  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * @return
 * - >= 0: Success.
 * - < 0: Failure.
 */
AGORA_API int64_t AGORA_CALL getAgoraCurrentMonotonicTimeInMs();
