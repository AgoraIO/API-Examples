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
#else
#define AGORA_API extern "C" __declspec(dllimport)
#endif  // AGORARTC_EXPORT

#define AGORA_CALL __cdecl

#define __deprecated

#elif defined(__APPLE__)

#include <TargetConditionals.h>

#define AGORA_API extern "C" __attribute__((visibility("default")))
#define AGORA_CALL

#elif defined(__ANDROID__) || defined(__linux__)

#define AGORA_API extern "C" __attribute__((visibility("default")))
#define AGORA_CALL

#define __deprecated

#else  // !_WIN32 && !__APPLE__ && !(__ANDROID__ || __linux__)

#define AGORA_API extern "C"
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

#define INVALID_DISPLAY_ID 0xffff

namespace agora {
namespace commons {
namespace cjson {
class JsonWrapper;
}  // namespace cjson
}  // namespace commons

typedef commons::cjson::JsonWrapper any_document_t;

namespace base {
class IEngineBase;

class IParameterEngine {
 public:
  virtual int setParameters(const char* parameters) = 0;
  virtual int getParameters(const char* key, any_document_t& result) = 0;
  virtual ~IParameterEngine() {}
};
}  // namespace base

namespace util {

template <class T>
class AutoPtr {
 protected:
  typedef T value_type;
  typedef T* pointer_type;

 public:
  explicit AutoPtr(pointer_type p = NULL) : ptr_(p) {}

  ~AutoPtr() {
    if (ptr_) {
      ptr_->release();
      ptr_ = NULL;
    }
  }

  operator bool() const { return (ptr_ != NULL); }

  value_type& operator*() const { return *get(); }

  pointer_type operator->() const { return get(); }

  pointer_type get() const { return ptr_; }

  pointer_type release() {
    pointer_type ret = ptr_;
    ptr_ = 0;
    return ret;
  }

  void reset(pointer_type ptr = NULL) {
    if (ptr != ptr_ && ptr_) {
      ptr_->release();
    }

    ptr_ = ptr;
  }

  template <class C1, class C2>
  bool queryInterface(C1* c, C2 iid) {
    pointer_type p = NULL;
    if (c && !c->queryInterface(iid, reinterpret_cast<void**>(&p))) {
      reset(p);
    }

    return (p != NULL);
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
  explicit CopyableAutoPtr(const CopyableAutoPtr& rhs) { this->reset(rhs.clone()); }
  CopyableAutoPtr& operator=(const CopyableAutoPtr& rhs) {
    if (this != &rhs) this->reset(rhs.clone());
    return *this;
  }
  pointer_type clone() const {
    if (!this->get()) return NULL;
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
  explicit AOutputIterator(IIterator* it = NULL) : p(it) {}
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
  bool valid() const { return p && p->current() != NULL; }
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
  AList() : container(NULL), owner(false) {}
  AList(IContainer* c, bool take_ownership) : container(c), owner(take_ownership) {}
  ~AList() { reset(); }
  void reset(IContainer* c = NULL, bool take_ownership = false) {
    if (owner && container) container->release();
    container = c;
    owner = take_ownership;
  }
  iterator begin() { return container ? iterator(container->begin()) : iterator(NULL); }
  iterator end() { return iterator(NULL); }
  size_type size() const { return container ? container->size() : 0; }
  bool empty() const { return size() == 0; }
};

}  // namespace util

/**
 * The channel profile.
 */
enum CHANNEL_PROFILE_TYPE {
  /**
   * 0: Communication.
   *
   * This profile prioritizes smoothness and applies to the one-to-one scenario.
   */
  CHANNEL_PROFILE_COMMUNICATION = 0,
  /**
   * 1: (Default) Live Broadcast.
   *
   * This profile prioritizes supporting a large audience in a live broadcast channel.
   */
  CHANNEL_PROFILE_LIVE_BROADCASTING = 1,
  /**
   * 2: Gaming.
   * @deprecated This profile is deprecated.
   */
  CHANNEL_PROFILE_GAME __deprecated = 2,
  /**
   * 3: Cloud Gaming.
   *
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
 * The error codes.
 */
enum ERROR_CODE_TYPE {
  /**
   * 0: No error occurs.
   */
  ERR_OK = 0,
  // 1~1000
  /**
   * 1: A general error occurs (no specified reason).
   */
  ERR_FAILED = 1,
  /**
   * 2: The argument is invalid. For example, the specific channel name
   * includes illegal characters.
   */
  ERR_INVALID_ARGUMENT = 2,
  /**
   * 3: The SDK module is not ready. Choose one of the following solutions:
   * - Check the audio device.
   * - Check the completeness of the app.
   * - Reinitialize the RTC engine.
   */
  ERR_NOT_READY = 3,
  /**
   * 4: The SDK does not support this function.
   */
  ERR_NOT_SUPPORTED = 4,
  /**
   * 5: The request is rejected.
   */
  ERR_REFUSED = 5,
  /**
   * 6: The buffer size is not big enough to store the returned data.
   */
  ERR_BUFFER_TOO_SMALL = 6,
  /**
   * 7: The SDK is not initialized before calling this method.
   */
  ERR_NOT_INITIALIZED = 7,
  /**
   * 8: The state is invalid.
   */
  ERR_INVALID_STATE = 8,
  /**
   * 9: No permission. This is for internal use only, and does
   * not return to the app through any method or callback.
   */
  ERR_NO_PERMISSION = 9,
  /**
   * 10: An API timeout occurs. Some API methods require the SDK to return the
   * execution result, and this error occurs if the request takes too long
   * (more than 10 seconds) for the SDK to process.
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
   * 17: The request to join the channel is rejected. This error usually occurs
   * when the user is already in the channel, and still calls the method to join
   * the channel, for example, \ref agora::rtc::IRtcEngine::joinChannel "joinChannel()".
   */
  ERR_JOIN_CHANNEL_REJECTED = 17,
  /**
   * 18: The request to leave the channel is rejected. This error usually
   * occurs when the user has already left the channel, and still calls the
   * method to leave the channel, for example, \ref agora::rtc::IRtcEngine::leaveChannel
   * "leaveChannel".
   */
  ERR_LEAVE_CHANNEL_REJECTED = 18,
  /**
   * 19: The resources have been occupied and cannot be reused.
   */
  ERR_ALREADY_IN_USE = 19,
  /**
   * 20: The SDK gives up the request due to too many requests. This is for
   * internal use only, and does not return to the app through any method or callback.
   */
  ERR_ABORTED = 20,
  /**
   * 21: On Windows, specific firewall settings can cause the SDK to fail to
   * initialize and crash.
   */
  ERR_INIT_NET_ENGINE = 21,
  /**
   * 22: The app uses too much of the system resource and the SDK
   * fails to allocate any resource.
   */
  ERR_RESOURCE_LIMITED = 22,
  /**
   * 101: The App ID is invalid, usually because the data format of the App ID is incorrect.
   *
   * Solution: Check the data format of your App ID. Ensure that you use the correct App ID to initialize the Agora service.
   */
  ERR_INVALID_APP_ID = 101,
  /**
   * 102: The specified channel name is invalid. Please try to rejoin the
   * channel with a valid channel name.
   */
  ERR_INVALID_CHANNEL_NAME = 102,
  /**
   * 103: Fails to get server resources in the specified region. Please try to
   * specify another region when calling \ref agora::rtc::IRtcEngine::initialize
   *  "initialize".
   */
  ERR_NO_SERVER_RESOURCES = 103,
  /**
   * 109: The token has expired, usually for the following reasons:
   * - Timeout for token authorization: Once a token is generated, you must use it to access the
   * Agora service within 24 hours. Otherwise, the token times out and you can no longer use it.
   * - The token privilege expires: To generate a token, you need to set a timestamp for the token
   * privilege to expire. For example, If you set it as seven days, the token expires seven days after
   * its usage. In that case, you can no longer access the Agora service. The users cannot make calls,
   * or are kicked out of the channel.
   *
   * Solution: Regardless of whether token authorization times out or the token privilege expires,
   * you need to generate a new token on your server, and try to join the channel.
   */
  ERR_TOKEN_EXPIRED = 109,
  /**
   * 110: The token is invalid, usually for one of the following reasons:
   * - Did not provide a token when joining a channel in a situation where the project has enabled the
   * App Certificate.
   * - Tried to join a channel with a token in a situation where the project has not enabled the App
   * Certificate.
   * - The App ID, user ID and channel name that you use to generate the token on the server do not match
   * those that you use when joining a channel.
   *
   * Solution:
   * - Before joining a channel, check whether your project has enabled the App certificate. If yes, you
   * must provide a token when joining a channel; if no, join a channel without a token.
   * - When using a token to join a channel, ensure that the App ID, user ID, and channel name that you
   * use to generate the token is the same as the App ID that you use to initialize the Agora service, and
   * the user ID and channel name that you use to join the channel.
   */
  ERR_INVALID_TOKEN = 110,
  /**
   * 111: The internet connection is interrupted. This applies to the Agora Web
   * SDK only.
   */
  ERR_CONNECTION_INTERRUPTED = 111,  // only used in web sdk
  /**
   * 112: The internet connection is lost. This applies to the Agora Web SDK
   * only.
   */
  ERR_CONNECTION_LOST = 112,  // only used in web sdk
  /**
   * 113: The user is not in the channel when calling the
   * \ref agora::rtc::IRtcEngine::sendStreamMessage "sendStreamMessage()" method.
   */
  ERR_NOT_IN_CHANNEL = 113,
  /**
   * 114: The data size is over 1024 bytes when the user calls the
   * \ref agora::rtc::IRtcEngine::sendStreamMessage "sendStreamMessage()" method.
   */
  ERR_SIZE_TOO_LARGE = 114,
  /**
   * 115: The bitrate of the sent data exceeds the limit of 6 Kbps when the
   * user calls the \ref agora::rtc::IRtcEngine::sendStreamMessage "sendStreamMessage()".
   */
  ERR_BITRATE_LIMIT = 115,
  /**
   * 116: Too many data streams (over 5) are created when the user
   * calls the \ref agora::rtc::IRtcEngine::createDataStream "createDataStream()" method.
   */
  ERR_TOO_MANY_DATA_STREAMS = 116,
  /**
   * 117: A timeout occurs for the data stream transmission.
   */
  ERR_STREAM_MESSAGE_TIMEOUT = 117,
  /**
   * 119: Switching the user role fails. Please try to rejoin the channel.
   */
  ERR_SET_CLIENT_ROLE_NOT_AUTHORIZED = 119,
  /**
   * 120: Decryption fails. The user may have tried to join the channel with a wrong
   * password. Check your settings or try rejoining the channel.
   */
  ERR_DECRYPTION_FAILED = 120,
  /**
   * 121: The user ID is invalid.
   */
  ERR_INVALID_USER_ID = 121,
  /**
   * 123: The app is banned by the server.
   */
  ERR_CLIENT_IS_BANNED_BY_SERVER = 123,
  /**
   * 130: Encryption is enabled when the user calls the
   * \ref agora::rtc::IRtcEngine::addPublishStreamUrl "addPublishStreamUrl()" method
   * (CDN live streaming does not support encrypted streams).
   */
  ERR_ENCRYPTED_STREAM_NOT_ALLOWED_PUBLISH = 130,

  /**
   * 131: License credential is invalid
   */
  ERR_LICENSE_CREDENTIAL_INVALID = 131,

  /**
   * 134: The user account is invalid, usually because the data format of the user account is incorrect.
   */
  ERR_INVALID_USER_ACCOUNT = 134,

  /** 157: The necessary dynamical library is not integrated. For example, if you call
   * the \ref agora::rtc::IRtcEngine::enableDeepLearningDenoise "enableDeepLearningDenoise" but do not integrate the dynamical
   * library for the deep-learning noise reduction into your project, the SDK reports this error code.
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
  ERR_PCMSEND_FORMAT = 200,           // unsupport pcm format
  ERR_PCMSEND_BUFFEROVERFLOW = 201,  // buffer overflow, the pcm send rate too quickly

  /// @cond
  // signaling: 400~600
  ERR_LOGIN_ALREADY_LOGIN = 428,

  /// @endcond
  // 1001~2000
  /**
   * 1001: Fails to load the media engine.
   */
  ERR_LOAD_MEDIA_ENGINE = 1001,
  /**
   * 1005: Audio device module: A general error occurs in the Audio Device Module (no specified
   * reason). Check if the audio device is used by another app, or try
   * rejoining the channel.
   */
  ERR_ADM_GENERAL_ERROR = 1005,
  /**
   * 1008: Audio Device Module: An error occurs in initializing the playback
   * device.
   */
  ERR_ADM_INIT_PLAYOUT = 1008,
  /**
   * 1009: Audio Device Module: An error occurs in starting the playback device.
   */
  ERR_ADM_START_PLAYOUT = 1009,
  /**
   * 1010: Audio Device Module: An error occurs in stopping the playback device.
   */
  ERR_ADM_STOP_PLAYOUT = 1010,
  /**
   * 1011: Audio Device Module: An error occurs in initializing the recording
   * device.
   */
  ERR_ADM_INIT_RECORDING = 1011,
  /**
   * 1012: Audio Device Module: An error occurs in starting the recording device.
   */
  ERR_ADM_START_RECORDING = 1012,
  /**
   * 1013: Audio Device Module: An error occurs in stopping the recording device.
   */
  ERR_ADM_STOP_RECORDING = 1013,
  /**
   * 1501: Video Device Module: The camera is not authorized.
   */
  ERR_VDM_CAMERA_NOT_AUTHORIZED = 1501,
};

/**
 * The operational permission of the SDK on the audio session.
 */
enum AUDIO_SESSION_OPERATION_RESTRICTION {
  /**
   * 0: No restriction; the SDK can change the audio session.
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
   * 4: The SDK keeps the audio session active when the user leaves the
   * channel, for example, to play an audio file in the background.
   */
  AUDIO_SESSION_OPERATION_RESTRICTION_DEACTIVATE_SESSION = 1 << 2,
  /**
   * 128: Completely restricts the operational permission of the SDK on the
   * audio session; the SDK cannot change the audio session.
   */
  AUDIO_SESSION_OPERATION_RESTRICTION_ALL = 1 << 7,
};

typedef const char* user_id_t;
typedef void* view_t;

/**
 * The definition of the UserInfo struct.
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
 * Reasons for a user being offline.
 */
enum USER_OFFLINE_REASON_TYPE {
  /**
   * 0: The user leaves the current channel.
   */
  USER_OFFLINE_QUIT = 0,
  /**
   * 1: The SDK times out and the user drops offline because no data packet was received within a certain
   * period of time. If a user quits the call and the message is not passed to the SDK (due to an
   * unreliable channel), the SDK assumes that the user drops offline.
   */
  USER_OFFLINE_DROPPED = 1,
  /**
   * 2: (Live Broadcast only.) The user role switches from broadcaster to audience.
   */
  USER_OFFLINE_BECOME_AUDIENCE = 2,
};

enum INTERFACE_ID_TYPE {
  AGORA_IID_AUDIO_DEVICE_MANAGER = 1,
  AGORA_IID_VIDEO_DEVICE_MANAGER = 2,
  AGORA_IID_PARAMETER_ENGINE = 3,
  AGORA_IID_MEDIA_ENGINE = 4,
  AGORA_IID_AUDIO_ENGINE = 5,
  AGORA_IID_VIDEO_ENGINE = 6,
  AGORA_IID_RTC_CONNECTION = 7,
  AGORA_IID_SIGNALING_ENGINE = 8,
  AGORA_IID_MEDIA_ENGINE_REGULATOR = 9,
  AGORA_IID_CLOUD_SPATIAL_AUDIO = 10,
  AGORA_IID_LOCAL_SPATIAL_AUDIO = 11,
  AGORA_IID_MEDIA_RECORDER = 12,
};

/**
 * The network quality types.
 */
enum QUALITY_TYPE {
  /**
   * 0: The network quality is unknown.
   * @deprecated This member is deprecated.
   */
  QUALITY_UNKNOWN __deprecated = 0,
  /**
   * 1: The quality is excellent.
   */
  QUALITY_EXCELLENT = 1,
  /**
   * 2: The quality is quite good, but the bitrate may be slightly
   * lower than excellent.
   */
  QUALITY_GOOD = 2,
  /**
   * 3: Users can feel the communication slightly impaired.
   */
  QUALITY_POOR = 3,
  /**
   * 4: Users cannot communicate smoothly.
   */
  QUALITY_BAD = 4,
  /**
   * 5: Users can barely communicate.
   */
  QUALITY_VBAD = 5,
  /**
   * 6: Users cannot communicate at all.
   */
  QUALITY_DOWN = 6,
  /**
   * 7: (For future use) The network quality cannot be detected.
   */
  QUALITY_UNSUPPORTED = 7,
  /**
   * 8: Detecting the network quality.
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
 * The rotation information.
 */
enum VIDEO_ORIENTATION {
  /**
   * 0: Rotate the video by 0 degree clockwise.
   */
  VIDEO_ORIENTATION_0 = 0,
  /**
   * 90: Rotate the video by 90 degrees clockwise.
   */
  VIDEO_ORIENTATION_90 = 90,
  /**
   * 180: Rotate the video by 180 degrees clockwise.
   */
  VIDEO_ORIENTATION_180 = 180,
  /**
   * 270: Rotate the video by 270 degrees clockwise.
   */
  VIDEO_ORIENTATION_270 = 270
};

/**
 * The video frame rate.
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
   * 60: 60 fps. Applies to Windows and macOS only.
   */
  FRAME_RATE_FPS_60 = 60,
};

enum FRAME_WIDTH {
  FRAME_WIDTH_640 = 640,
};

enum FRAME_HEIGHT {
  FRAME_HEIGHT_360 = 360,
};


/**
 * Types of the video frame.
 */
enum VIDEO_FRAME_TYPE {
  /** (Default) Blank frame */
  VIDEO_FRAME_TYPE_BLANK_FRAME = 0,
  /** (Default) Key frame */
  VIDEO_FRAME_TYPE_KEY_FRAME = 3,
  /** (Default) Delta frame */
  VIDEO_FRAME_TYPE_DELTA_FRAME = 4,
  /** (Default) B frame */
  VIDEO_FRAME_TYPE_B_FRAME = 5,
  /** (Default) Droppable frame */
  VIDEO_FRAME_TYPE_DROPPABLE_FRAME = 6,
  /** (Default) Unknown frame type */
  VIDEO_FRAME_TYPE_UNKNOW
};

/**
 * Video output orientation modes.
 */
enum ORIENTATION_MODE {
  /**
   * 0: (Default) Adaptive mode.
   *
   * In this mode, the output video always follows the orientation of the captured video.
   * - If the captured video is in landscape mode, the output video is in landscape mode.
   * - If the captured video is in portrait mode, the output video is in portrait mode.
   */
  ORIENTATION_MODE_ADAPTIVE = 0,
  /**
   * 1: Landscape mode.
   *
   * In this mode, the output video is always in landscape mode. If the captured video is in portrait
   * mode, the video encoder crops it to fit the output. Applies to scenarios where the receiver
   * cannot process the rotation information, for example, CDN live streaming.
   */
  ORIENTATION_MODE_FIXED_LANDSCAPE = 1,
  /**
   * 2: Portrait mode.
   *
   * In this mode, the output video is always in portrait mode. If the captured video is in landscape
   * mode, the video encoder crops it to fit the output. Applies to scenarios where the receiver
   * cannot process the rotation information, for example, CDN live streaming.
   */
  ORIENTATION_MODE_FIXED_PORTRAIT = 2,
};

/**
 * (For future use) Video degradation preferences under limited bandwidth.
 */
enum DEGRADATION_PREFERENCE {
  /**
   * 0: (Default) Degrade the frame rate and keep resolution to guarantee the video quality.
   */
  MAINTAIN_QUALITY = 0,
  /**
   * 1: Degrade resolution in order to maintain framerate.
   */
  MAINTAIN_FRAMERATE = 1,
  /**
   * 2: Maintain resolution in video quality control process. Under limited bandwidth, degrade video quality first and then degrade frame rate.
   */
  MAINTAIN_BALANCED = 2,
  /**
   * 3: Degrade framerate in order to maintain resolution.
   */
  MAINTAIN_RESOLUTION = 3,
  /**
   * 4: Disable VQC adjustion.
   */
  DISABLED = 100,
};

/**
 * The definition of the VideoDimensions struct.
 */
struct VideoDimensions {
  /**
   * The width of the video in number of pixels.
   */
  int width;
  /**
   * The height of the video in number of pixels.
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
 * In this mode, the bitrates differ between the live broadcast and communication
 * profiles:
 *
 * - Communication profile: The video bitrate is the same as the base bitrate.
 * - Live Broadcast profile: The video bitrate is twice the base bitrate.
 */
const int STANDARD_BITRATE = 0;

/**
 * -1: Compatible bitrate mode.
 *
 * In this mode, the bitrate remains the same regardless of the channel profile. If you choose
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
 * Video codec types.
 */
enum VIDEO_CODEC_TYPE {
  VIDEO_CODEC_NONE = 0,
  /**
   * 1: VP8.
   */
  VIDEO_CODEC_VP8 = 1,
  /**
   * 2: H.264.
   */
  VIDEO_CODEC_H264 = 2,
  /**
   * 3: H.265.
   */
  VIDEO_CODEC_H265 = 3,
  /**
   * 6: Generic.
   */
  VIDEO_CODEC_GENERIC = 6,
  /**
   * 7: Generic H264.
   */
  VIDEO_CODEC_GENERIC_H264 = 7,
  /**
    * 12: AV1.
    */
  VIDEO_CODEC_AV1 = 12,
  /**
   * 5: VP9.
   */
  VIDEO_CODEC_VP9 = 13,
  /**
   * 20: JPEG.
   */
  VIDEO_CODEC_GENERIC_JPEG = 20,
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
   * The codec type used for the encoded images: \ref agora::rtc::VIDEO_CODEC_TYPE "VIDEO_CODEC_TYPE".
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
   * - \ref agora::rtc::COMPATIBLE_BITRATE "COMPATIBLE_BITRATE": Compatible bitrate. The bitrate stays the same
   * regardless of the profile.
   *
   * The Communication profile prioritizes smoothness, while the Live Broadcast
   * profile prioritizes video quality (requiring a higher bitrate). Agora
   * recommends setting the bitrate mode as \ref agora::rtc::STANDARD_BITRATE "STANDARD_BITRATE" or simply to
   * address this difference.
   *
   * The following table lists the recommended video encoder configurations,
   * where the base bitrate applies to the communication profile. Set your
   * bitrate based on this table. If the bitrate you set is beyond the proper
   * range, the SDK automatically sets it to within the range.

   | Resolution             | Frame Rate (fps) | Base Bitrate (Kbps, for Communication) | Live Bitrate (Kbps, for Live Broadcast)|
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

  SenderOptions()
  : ccMode(CC_ENABLED),
    codecType(VIDEO_CODEC_H264),
    targetBitrate(6500) {}
};

/**
 * Audio codec types.
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
   * 8: AAC LC.
   */
  AUDIO_CODEC_AACLC = 8,
  /**
   * 9: HE AAC.
   */
  AUDIO_CODEC_HEAAC = 9,
  /**
   * 10: JC1.
   */
  AUDIO_CODEC_JC1 = 10,
  AUDIO_CODEC_HEAAC2 = 11,
  /**
   * 12: LPCNET.
   */
  AUDIO_CODEC_LPCNET = 12,
};

/**
 * audio encoding type of audio encoded frame observer.
 */
enum AUDIO_ENCODING_TYPE {
  /**
   * 1: codecType AAC; sampleRate 16000; quality low which around 1.2 MB after 10 minutes
   */
  AUDIO_ENCODING_TYPE_AAC_16000_LOW = 0x010101,
  /**
   * 1: codecType AAC; sampleRate 16000; quality medium which around 2 MB after 10 minutes
   */
  AUDIO_ENCODING_TYPE_AAC_16000_MEDIUM = 0x010102,
  /**
   * 1: codecType AAC; sampleRate 32000; quality low which around 1.2 MB after 10 minutes
   */
  AUDIO_ENCODING_TYPE_AAC_32000_LOW = 0x010201,
  /**
   * 1: codecType AAC; sampleRate 32000; quality medium which around 2 MB after 10 minutes
   */
  AUDIO_ENCODING_TYPE_AAC_32000_MEDIUM = 0x010202,
  /**
   * 1: codecType AAC; sampleRate 32000; quality high which around 3.5 MB after 10 minutes
   */
  AUDIO_ENCODING_TYPE_AAC_32000_HIGH = 0x010203,
  /**
   * 1: codecType AAC; sampleRate 48000; quality medium which around 2 MB after 10 minutes
   */
  AUDIO_ENCODING_TYPE_AAC_48000_MEDIUM = 0x010302,
  /**
   * 1: codecType AAC; sampleRate 48000; quality high which around 3.5 MB after 10 minutes
   */
  AUDIO_ENCODING_TYPE_AAC_48000_HIGH = 0x010303,

  /**
   * 1: codecType OPUS; sampleRate 16000; quality low which around 1.2 MB after 10 minutes
   */
  AUDIO_ENCODING_TYPE_OPUS_16000_LOW = 0x020101,
  /**
   * 1: codecType OPUS; sampleRate 16000; quality medium which around 2 MB after 10 minutes
   */
  AUDIO_ENCODING_TYPE_OPUS_16000_MEDIUM = 0x020102,
  /**
   * 1: codecType OPUS; sampleRate 48000; quality medium which around 2 MB after 10 minutes
   */
  AUDIO_ENCODING_TYPE_OPUS_48000_MEDIUM = 0x020302,
  /**
   * 1: codecType OPUS; sampleRate 48000; quality high which around 3.5 MB after 10 minutes
   */
  AUDIO_ENCODING_TYPE_OPUS_48000_HIGH = 0x020303,
};

/**
 * Watermark fit mode
 */
enum WATERMARK_FIT_MODE {
  /**
   * Use the position of positionInLandscapeMode/positionInPortraitMode in #WatermarkOptions
   * the widthRatio will be invalid.
   */
  FIT_MODE_COVER_POSITION,
  /**
   * Use width rotio of video, in this mode, positionInLandscapeMode/positionInPortraitMode
   * in #WatermarkOptions will be invalid, and watermarkRatio will valid.
   */
  FIT_MODE_USE_IMAGE_RATIO
};

/**
 * advanced settings of encoded audio frame.
 */
struct EncodedAudioFrameAdvancedSettings {
  EncodedAudioFrameAdvancedSettings()
    : speech(true),
      sendEvenIfEmpty(true) {}

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
 * The definition of the EncodedAudioFrameInfo struct.
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
   * The audio codec: #AUDIO_CODEC_TYPE.
   */
  AUDIO_CODEC_TYPE codec;
  /**
   * The sample rate (Hz) of the audio frame.
   */
  int sampleRateHz;
  /**
   * The number of samples per audio channel.
   *
   * If this value is not set, it is 1024 for AAC, or 960 for OPUS by default.
   */
  int samplesPerChannel;
  /**
   * The number of audio channels of the audio frame.
   */
  int numberOfChannels;
  /**
   * The advanced settings of the audio frame.
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
  AudioPcmDataInfo() : samplesPerChannel(0), channelNum(0), samplesOut(0), elapsedTimeMs(0), ntpTimeMs(0) {}

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
  SingleNalUnit,       // Mode 0 - only single NALU allowed
};

/**
 * Video stream types.
 */
enum VIDEO_STREAM_TYPE {
  /**
   * 0: The high-quality video stream, which has a higher resolution and bitrate.
   */
  VIDEO_STREAM_HIGH = 0,
  /**
   * 1: The low-quality video stream, which has a lower resolution and bitrate.
   */
  VIDEO_STREAM_LOW = 1,
};

struct VideoSubscriptionOptions {
    /**
     * The type of the video stream to subscribe to.
     *
     * The default value is `VIDEO_STREAM_HIGH`, which means the high-quality
     * video stream.
     */
    Optional<VIDEO_STREAM_TYPE> type;
    /**
     * Whether to subscribe to encoded video data only:
     * - `true`: Subscribe to encoded video data only.
     * - `false`: (Default) Subscribe to decoded video data.
     */
    Optional<bool> encodedFrameOnly;

    VideoSubscriptionOptions() {}
};

/**
 * The definition of the EncodedVideoFrameInfo struct.
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
      uid(0),
      streamType(VIDEO_STREAM_HIGH) {}

  EncodedVideoFrameInfo(const EncodedVideoFrameInfo& rhs)
    : codecType(rhs.codecType),
      width(rhs.width),
      height(rhs.height),
      framesPerSecond(rhs.framesPerSecond),
      frameType(rhs.frameType),
      rotation(rhs.rotation),
      trackId(rhs.trackId),
      captureTimeMs(rhs.captureTimeMs),
      uid(rhs.uid),
      streamType(rhs.streamType) {}

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
    uid = rhs.uid;
    streamType = rhs.streamType;
    return *this;
  }
  /**
   * The video codec: #VIDEO_CODEC_TYPE.
   */
  VIDEO_CODEC_TYPE codecType;
  /**
   * The width (px) of the video.
   */
  int width;
  /**
   * The height (px) of the video.
   */
  int height;
  /**
   * The number of video frames per second.
   * This value will be used for calculating timestamps of the encoded image.
   * If framesPerSecond equals zero, then real timestamp will be used.
   * Otherwise, timestamp will be adjusted to the value of framesPerSecond set.
   */
  int framesPerSecond;
  /**
   * The frame type of the encoded video frame: #VIDEO_FRAME_TYPE.
   */
  VIDEO_FRAME_TYPE frameType;
  /**
   * The rotation information of the encoded video frame: #VIDEO_ORIENTATION.
   */
  VIDEO_ORIENTATION rotation;
  /**
   * The track ID of the video frame.
   */
  int trackId;  // This can be reserved for multiple video tracks, we need to create different ssrc
                // and additional payload for later implementation.
  /**
   * This is a input parameter which means the timestamp for capturing the video.
   */
  int64_t captureTimeMs;
  /**
   * ID of the user.
   */
  uid_t uid;
  /**
   * The stream type of video frame.
   */
  VIDEO_STREAM_TYPE streamType;
};

/**
 * Video mirror mode types.
 */
enum VIDEO_MIRROR_MODE_TYPE {
  /**
   * (Default) 0: The mirror mode determined by the SDK.
   */
  VIDEO_MIRROR_MODE_AUTO = 0,
  /**
   * 1: Enable the mirror mode.
   */
  VIDEO_MIRROR_MODE_ENABLED = 1,
  /**
   * 2: Disable the mirror mode.
   */
  VIDEO_MIRROR_MODE_DISABLED = 2,
};

/**
 * The definition of the VideoEncoderConfiguration struct.
 */
struct VideoEncoderConfiguration {
  /**
   * The video encoder code type: #VIDEO_CODEC_TYPE. reserved , not used now
   */
  VIDEO_CODEC_TYPE codecType;
  /**
   * The video dimension: VideoDimensions.
   */
  VideoDimensions dimensions;
  /**
   * The frame rate of the video. You can set it manually, or choose one from #FRAME_RATE.
   */
  int frameRate;
  /**
   * The bitrate (Kbps) of the video.
   *
   * Refer to the **Video Bitrate Table** below and set your bitrate. If you set a bitrate beyond the
   * proper range, the SDK automatically adjusts it to a value within the range. You can also choose
   * from the following options:
   *
   * - #STANDARD_BITRATE: (Recommended) Standard bitrate mode. In this mode, the bitrates differ between
   * the Live Broadcast and Communication profiles:
   *   - In the Communication profile, the video bitrate is the same as the base bitrate.
   *   - In the Live Broadcast profile, the video bitrate is twice the base bitrate.
   * - #COMPATIBLE_BITRATE: Compatible bitrate mode. The compatible bitrate mode. In this mode, the bitrate
   * stays the same regardless of the profile. If you choose this mode for the Live Broadcast profile,
   * the video frame rate may be lower than the set value.
   *
   * Agora uses different video codecs for different profiles to optimize the user experience. For example,
   * the communication profile prioritizes the smoothness while the live-broadcast profile prioritizes the
   * video quality (a higher bitrate). Therefore, We recommend setting this parameter as #STANDARD_BITRATE.
   *
   * | Resolution             | Frame Rate (fps) | Base Bitrate (Kbps) | Live Bitrate (Kbps)|
   * |------------------------|------------------|---------------------|--------------------|
   * | 160 * 120              | 15               | 65                  | 130                |
   * | 120 * 120              | 15               | 50                  | 100                |
   * | 320 * 180              | 15               | 140                 | 280                |
   * | 180 * 180              | 15               | 100                 | 200                |
   * | 240 * 180              | 15               | 120                 | 240                |
   * | 320 * 240              | 15               | 200                 | 400                |
   * | 240 * 240              | 15               | 140                 | 280                |
   * | 424 * 240              | 15               | 220                 | 440                |
   * | 640 * 360              | 15               | 400                 | 800                |
   * | 360 * 360              | 15               | 260                 | 520                |
   * | 640 * 360              | 30               | 600                 | 1200               |
   * | 360 * 360              | 30               | 400                 | 800                |
   * | 480 * 360              | 15               | 320                 | 640                |
   * | 480 * 360              | 30               | 490                 | 980                |
   * | 640 * 480              | 15               | 500                 | 1000               |
   * | 480 * 480              | 15               | 400                 | 800                |
   * | 640 * 480              | 30               | 750                 | 1500               |
   * | 480 * 480              | 30               | 600                 | 1200               |
   * | 848 * 480              | 15               | 610                 | 1220               |
   * | 848 * 480              | 30               | 930                 | 1860               |
   * | 640 * 480              | 10               | 400                 | 800                |
   * | 1280 * 720             | 15               | 1130                | 2260               |
   * | 1280 * 720             | 30               | 1710                | 3420               |
   * | 960 * 720              | 15               | 910                 | 1820               |
   * | 960 * 720              | 30               | 1380                | 2760               |
   * | 1920 * 1080            | 15               | 2080                | 4160               |
   * | 1920 * 1080            | 30               | 3150                | 6300               |
   * | 1920 * 1080            | 60               | 4780                | 6500               |
   * | 2560 * 1440            | 30               | 4850                | 6500               |
   * | 2560 * 1440            | 60               | 6500                | 6500               |
   * | 3840 * 2160            | 30               | 6500                | 6500               |
   * | 3840 * 2160            | 60               | 6500                | 6500               |
   */
  int bitrate;

  /**
   * (For future use) The minimum encoding bitrate (Kbps).
   *
   * The Agora SDK automatically adjusts the encoding bitrate to adapt to the
   * network conditions.
   *
   * Using a value greater than the default value forces the video encoder to
   * output high-quality images but may cause more packet loss and hence
   * sacrifice the smoothness of the video transmission. That said, unless you
   * have special requirements for image quality, Agora does not recommend
   * changing this value.
   *
   * @note
   * This parameter applies to the live-broadcast profile only.
   */
  int minBitrate;
  /**
   * The video orientation mode: #ORIENTATION_MODE.
   */
  ORIENTATION_MODE orientationMode;
  /**
   *
   * The video degradation preference under limited bandwidth: #DEGRADATION_PREFERENCE.
   */
  DEGRADATION_PREFERENCE degradationPreference;

  /**
   * If mirror_type is set to VIDEO_MIRROR_MODE_ENABLED, then the video frame would be mirrored before encoding.
   */
  VIDEO_MIRROR_MODE_TYPE mirrorMode;

  VideoEncoderConfiguration(const VideoDimensions& d, int f, int b, ORIENTATION_MODE m, VIDEO_MIRROR_MODE_TYPE mirror = VIDEO_MIRROR_MODE_DISABLED)
    : codecType(VIDEO_CODEC_H264),
      dimensions(d),
      frameRate(f),
      bitrate(b),
      minBitrate(DEFAULT_MIN_BITRATE),
      orientationMode(m),
      degradationPreference(MAINTAIN_QUALITY),
      mirrorMode(mirror) {}
  VideoEncoderConfiguration(int width, int height, int f, int b, ORIENTATION_MODE m, VIDEO_MIRROR_MODE_TYPE mirror = VIDEO_MIRROR_MODE_DISABLED)
    : codecType(VIDEO_CODEC_H264),
      dimensions(width, height),
      frameRate(f),
      bitrate(b),
      minBitrate(DEFAULT_MIN_BITRATE),
      orientationMode(m),
      degradationPreference(MAINTAIN_QUALITY),
      mirrorMode(mirror) {}
  VideoEncoderConfiguration(const VideoEncoderConfiguration& config)
    : codecType(config.codecType),
      dimensions(config.dimensions),
      frameRate(config.frameRate),
      bitrate(config.bitrate),
      minBitrate(config.minBitrate),
      orientationMode(config.orientationMode),
      degradationPreference(config.degradationPreference),
      mirrorMode(config.mirrorMode) {}
  VideoEncoderConfiguration()
    : codecType(VIDEO_CODEC_H264),
      dimensions(FRAME_WIDTH_640, FRAME_HEIGHT_360),
      frameRate(FRAME_RATE_FPS_15),
      bitrate(STANDARD_BITRATE),
      minBitrate(DEFAULT_MIN_BITRATE),
      orientationMode(ORIENTATION_MODE_ADAPTIVE),
      degradationPreference(MAINTAIN_QUALITY),
      mirrorMode(VIDEO_MIRROR_MODE_DISABLED) {}

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
    return *this;
  }
};

/** Data stream config
*/
struct DataStreamConfig {
  /** syncWithAudio Sets whether or not the recipients receive the data stream sync with current audio stream.
  */
  bool syncWithAudio;
  /** ordered Sets whether or not the recipients receive the data stream in the sent order:
  */
  bool ordered;
};

/**
 * The definition of SIMULCAST_STREAM_MODE
 */
enum SIMULCAST_STREAM_MODE {
  /*
  * disable simulcast stream until receive request for enable simulcast stream by other broadcaster
  */
  AUTO_SIMULCAST_STREAM = -1,
  /*
  * disable simulcast stream
  */
  DISABLE_SIMULCAST_STREM = 0,
  /*
  * always enable simulcast stream
  */
  ENABLE_SIMULCAST_STREAM = 1,
};

/**
 * The definition of the of SimulcastStreamConfig struct.
 */
struct SimulcastStreamConfig {
  /**
   * The video frame dimension: VideoDimensions.
   */
  VideoDimensions dimensions;
  /**
   * The video bitrate (Kbps).
   */
  int bitrate;
  /**
   * The video framerate.
   */
  int framerate;
  SimulcastStreamConfig() : dimensions(160, 120), bitrate(65), framerate(5) {}
  bool operator==(const SimulcastStreamConfig& rhs) const {
    return dimensions == rhs.dimensions && bitrate == rhs.bitrate && framerate == rhs.framerate;
  }
};

/**
 * The relative location of the region to the screen or window.
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
   * The width of the region.
   */
  int width;
  /**
   * The height of the region.
   */
  int height;

  Rectangle() : x(0), y(0), width(0), height(0) {}
  Rectangle(int xx, int yy, int ww, int hh) : x(xx), y(yy), width(ww), height(hh) {}
};

/** The options of the watermark image to be added. */
struct WatermarkRatio {
  /**
   * The ratio of the width of the video, see #WATERMARK_FIT_MODE::FIT_MODE_USE_IMAGE_RATIO
   */
  float xRatio;
  /**
   * The ratio of the height of the video, see #WATERMARK_FIT_MODE::FIT_MODE_USE_IMAGE_RATIO
   */
  float yRatio;
  /**
   * The ratio of the width of the video, see #WATERMARK_FIT_MODE::FIT_MODE_USE_IMAGE_RATIO
   */
  float widthRatio;

  WatermarkRatio() : xRatio(0.0), yRatio(0.0), widthRatio(0.0) {}
  WatermarkRatio(float x, float y, float width) : xRatio(x), yRatio(y), widthRatio(width) {}
};

/** The options of the watermark image to be added. */
struct WatermarkOptions {
  /** Sets whether or not the watermark image is visible in the local video preview:
   * - true: (Default) The watermark image is visible in preview.
   * - false: The watermark image is not visible in preview.
   */
  bool visibleInPreview;
  /**
   * The watermark position in the landscape mode. See Rectangle.
   * For detailed information on the landscape mode, see the advanced guide *Video Rotation*.
   */
  Rectangle positionInLandscapeMode;
  /**
   * The watermark position in the portrait mode. See #WATERMARK_FIT_MODE::FIT_MODE_USE_IMAGE_RATIO.
   */
  Rectangle positionInPortraitMode;
  /**
   * The watermark position in the ratio mode. See Rectangle.
   * For detailed information on the portrait mode, see the advanced guide *Video Rotation*.
   */
  WatermarkRatio watermarkRatio;
  /**
   * The fit mode of watermark.
   */
  WATERMARK_FIT_MODE mode;

  WatermarkOptions()
    : visibleInPreview(true),
      positionInLandscapeMode(0, 0, 0, 0),
      positionInPortraitMode(0, 0, 0, 0),
      mode(FIT_MODE_COVER_POSITION) {}
};

/**
 * The definition of the RtcStats struct.
 */
struct RtcStats {
  /**
   * The call duration (s), represented by an aggregate value.
   */
  unsigned int duration;
  /**
   * The total number of bytes transmitted, represented by an aggregate value.
   */
  unsigned int txBytes;
  /**
   * The total number of bytes received, represented by an aggregate value.
   */
  unsigned int rxBytes;
  /**
   * The total number of audio bytes sent (bytes), represented by an aggregate value.
   */
  unsigned int txAudioBytes;
  /**
   * The total number of video bytes sent (bytes), represented by an aggregate value.
   */
  unsigned int txVideoBytes;
  /**
   * The total number of audio bytes received (bytes), represented by an aggregate value.
   */
  unsigned int rxAudioBytes;
  /**
   * The total number of video bytes received (bytes), represented by an aggregate value.
   */
  unsigned int rxVideoBytes;
  /**
   * The transmission bitrate (Kbps), represented by an instantaneous value.
   */
  unsigned short txKBitRate;
  /**
   * The receiving bitrate (Kbps), represented by an instantaneous value.
   */
  unsigned short rxKBitRate;
  /**
   * Audio receiving bitrate (Kbps), represented by an instantaneous value.
   */
  unsigned short rxAudioKBitRate;
  /**
   * The audio transmission bitrate (Kbps), represented by an instantaneous value.
   */
  unsigned short txAudioKBitRate;
  /**
   * The video receive bitrate (Kbps), represented by an instantaneous value.
   */
  unsigned short rxVideoKBitRate;
  /**
   * The video transmission bitrate (Kbps), represented by an instantaneous value.
   */
  unsigned short txVideoKBitRate;
  /**
   * The VOS client-server latency (ms).
   */
  unsigned short lastmileDelay;
  /**
   * The number of users in the channel.
   */
  unsigned int userCount;
  /**
   * The app CPU usage (%).
   */
  double cpuAppUsage;
  /**
   * The system CPU usage (%).
   */
  double cpuTotalUsage;
  /**
   * gateway Rtt
  */
  int gatewayRtt;
  /**
   * The memory usage ratio of the app (%).
   */
  double memoryAppUsageRatio;
  /**
   * The memory usage ratio of the system (%).
   */
  double memoryTotalUsageRatio;
  /**
   * The memory usage of the app (KB).
   */
  int memoryAppUsageInKbytes;
  /**
   * The time elapsed from the when the app starts connecting to an Agora channel
   * to when the connection is established. 0 indicates that this member does not apply.
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
   * The packet loss rate of sender(broadcaster).
   */
  int txPacketLossRate;
  /**
   * The packet loss rate of receiver(audience).
   */
  int rxPacketLossRate;
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
      rxPacketLossRate(0) {}
};

/**
* Video source types definition.
**/
enum VIDEO_SOURCE_TYPE {
  /** Video captured by the camera.
   */
  VIDEO_SOURCE_CAMERA_PRIMARY = 0,
  VIDEO_SOURCE_CAMERA = VIDEO_SOURCE_CAMERA_PRIMARY,
  /** Video captured by the secondary camera.
   */
  VIDEO_SOURCE_CAMERA_SECONDARY = 1,
  /** Video for screen sharing.
   */
  VIDEO_SOURCE_SCREEN_PRIMARY = 2,
  VIDEO_SOURCE_SCREEN = VIDEO_SOURCE_SCREEN_PRIMARY,
  /** Video for secondary screen sharing.
   */
  VIDEO_SOURCE_SCREEN_SECONDARY = 3,
  /** Not define.
   */
  VIDEO_SOURCE_CUSTOM = 4,
  /** Video for media player sharing.
   */
  VIDEO_SOURCE_MEDIA_PLAYER = 5,
  /** Video for png image.
   */
  VIDEO_SOURCE_RTC_IMAGE_PNG = 6,
  /** Video for png image.
   */
  VIDEO_SOURCE_RTC_IMAGE_JPEG = 7,
  /** Video for png image.
   */
  VIDEO_SOURCE_RTC_IMAGE_GIF = 8,
  /** Remote video received from network.
   */
  VIDEO_SOURCE_REMOTE = 9,
  /** Video for transcoded.
   */
  VIDEO_SOURCE_TRANSCODED = 10,

  VIDEO_SOURCE_UNKNOWN = 100
};

/**
 * User role types.
 */
enum CLIENT_ROLE_TYPE {
  /**
   * 1: Broadcaster. A broadcaster can both send and receive streams.
   */
  CLIENT_ROLE_BROADCASTER = 1,
  /**
   * 2: Audience. An audience can only receive streams.
   */
  CLIENT_ROLE_AUDIENCE = 2,
};

/** Quality change of the local video in terms of target frame rate and target bit rate since last count.
 */
enum QUALITY_ADAPT_INDICATION {
  /** The quality of the local video stays the same. */
  ADAPT_NONE = 0,
  /** The quality improves because the network bandwidth increases. */
  ADAPT_UP_BANDWIDTH = 1,
  /** The quality worsens because the network bandwidth decreases. */
  ADAPT_DOWN_BANDWIDTH = 2,
};

/** Client role levels in a live broadcast. */
enum AUDIENCE_LATENCY_LEVEL_TYPE
{
  /** 1: Low latency. */
  AUDIENCE_LATENCY_LEVEL_LOW_LATENCY = 1,
  /** 2: Ultra low latency. */
  AUDIENCE_LATENCY_LEVEL_ULTRA_LOW_LATENCY = 2,
};

/** Client role options, contains audience latency level.
 */
struct ClientRoleOptions
{
  /**
  Audience latency level.
  */
  AUDIENCE_LATENCY_LEVEL_TYPE audienceLatencyLevel;

  ClientRoleOptions()
    : audienceLatencyLevel(AUDIENCE_LATENCY_LEVEL_ULTRA_LOW_LATENCY) {}
};

/**
 * Quality of experience (QoE) of the local user when receiving a remote audio stream.
 */
enum EXPERIENCE_QUALITY_TYPE {
  /** 0: QoE of the local user is good.  */
  EXPERIENCE_QUALITY_GOOD = 0,
  /** 1: QoE of the local user is poor.  */
  EXPERIENCE_QUALITY_BAD = 1,
};

/**
 * The reason for poor QoE of the local user when receiving a remote audio stream.
 *
 */
enum EXPERIENCE_POOR_REASON {
  /** 0: No reason, indicating good QoE of the local user.
   */
  EXPERIENCE_REASON_NONE = 0,
  /** 1: The remote user's network quality is poor.
   */
  REMOTE_NETWORK_QUALITY_POOR = 1,
  /** 2: The local user's network quality is poor.
   */
  LOCAL_NETWORK_QUALITY_POOR = 2,
  /** 4: The local user's Wi-Fi or mobile network signal is weak.
   */
  WIRELESS_SIGNAL_POOR = 4,
  /** 8: The local user enables both Wi-Fi and bluetooth, and their signals interfere with each other.
   * As a result, audio transmission quality is undermined.
   */
  WIFI_BLUETOOTH_COEXIST = 8,
};

/**
 * The definition of the RemoteAudioStats struct, which
 * reports the audio statistics of a remote user.
 */
struct RemoteAudioStats
{
  /**
   * User ID of the remote user sending the audio stream.
   */
  uid_t uid;
  /**
   * The quality of the remote audio: #QUALITY_TYPE.
   */
  int quality;
  /**
   * The network delay (ms) from the sender to the receiver.
   */
  int networkTransportDelay;
  /**
   * The network delay (ms) from the receiver to the jitter buffer.
   */
  int jitterBufferDelay;
  /**
   * The audio frame loss rate in the reported interval.
   */
  int audioLossRate;
  /**
   * The number of channels.
   */
  int numChannels;
  /**
   * The sample rate (Hz) of the remote audio stream in the reported interval.
   */
  int receivedSampleRate;
  /**
   * The average bitrate (Kbps) of the remote audio stream in the reported
   * interval.
   */
  int receivedBitrate;
  /**
   * The total freeze time (ms) of the remote audio stream after the remote
   * user joins the channel.
   *
   * In a session, audio freeze occurs when the audio frame loss rate reaches 4%.
   */
  int totalFrozenTime;
  /**
   * The total audio freeze time as a percentage (%) of the total time when the
   * audio is available.
   */
  int frozenRate;
  /**
   * The quality of the remote audio stream as determined by the Agora
   * real-time audio MOS (Mean Opinion Score) measurement method in the
   * reported interval. The return value ranges from 0 to 500. Dividing the
   * return value by 100 gets the MOS score, which ranges from 0 to 5. The
   * higher the score, the better the audio quality.
   *
   * | MOS score       | Perception of audio quality                                                                                                                                 |
   * |-----------------|-------------------------------------------------------------------------------------------------------------------------------------------------------------|
   * | Greater than 4  | Excellent. The audio sounds clear and smooth.                                                                                                               |
   * | From 3.5 to 4   | Good. The audio has some perceptible impairment, but still sounds clear.                                                                                    |
   * | From 3 to 3.5   | Fair. The audio freezes occasionally and requires attentive listening.                                                                                      |
   * | From 2.5 to 3   | Poor. The audio sounds choppy and requires considerable effort to understand.                                                                               |
   * | From 2 to 2.5   | Bad. The audio has occasional noise. Consecutive audio dropouts occur, resulting in some information loss. The users can communicate only with difficulty.  |
   * | Less than 2     | Very bad. The audio has persistent noise. Consecutive audio dropouts are frequent, resulting in severe information loss. Communication is nearly impossible. |
   */
  int mosValue;
  /**
   * The total time (ms) when the remote user neither stops sending the audio
   * stream nor disables the audio module after joining the channel.
   */
  int totalActiveTime;
  /**
   * The total publish duration (ms) of the remote audio stream.
   */
  int publishDuration;
  /**
   * Quality of experience (QoE) of the local user when receiving a remote audio stream. See #EXPERIENCE_QUALITY_TYPE.
   */
  int qoeQuality;
  /**
   * The reason for poor QoE of the local user when receiving a remote audio stream. See #EXPERIENCE_POOR_REASON.
   */
  int qualityChangedReason;

  RemoteAudioStats() :
    uid(0),
    quality(0),
    networkTransportDelay(0),
    jitterBufferDelay(0),
    audioLossRate(0),
    numChannels(0),
    receivedSampleRate(0),
    receivedBitrate(0),
    totalFrozenTime(0),
    frozenRate(0),
    mosValue(0),
    totalActiveTime(0),
    publishDuration(0),
    qoeQuality(0),
    qualityChangedReason(0) {}
};

/**
 * Audio profile types.
 */
enum AUDIO_PROFILE_TYPE {
  /**
   * 0: The default audio profile.
   * - In the Communication profile, it represents a sample rate of 16 kHz, music encoding, mono, and a bitrate
   * of up to 16 Kbps.
   * - In the Live-broadcast profile, it represents a sample rate of 48 kHz, music encoding, mono, and a bitrate
   * of up to 64 Kbps.
   */
  AUDIO_PROFILE_DEFAULT = 0,
  /**
   * 1: A sample rate of 16 kHz, audio encoding, mono, and a bitrate up to 18 Kbps.
   */
  AUDIO_PROFILE_SPEECH_STANDARD = 1,
  /**
   * 2: A sample rate of 48 kHz, music encoding, mono, and a bitrate of up to 64 Kbps.
   */
  AUDIO_PROFILE_MUSIC_STANDARD = 2,
  /**
   * 3: A sample rate of 48 kHz, music encoding, stereo, and a bitrate of up to 80
   * Kbps.
   */
  AUDIO_PROFILE_MUSIC_STANDARD_STEREO = 3,
  /**
   * 4: A sample rate of 48 kHz, music encoding, mono, and a bitrate of up to 96 Kbps.
   */
  AUDIO_PROFILE_MUSIC_HIGH_QUALITY = 4,
  /**
   * 5: A sample rate of 48 kHz, music encoding, stereo, and a bitrate of up to 128 Kbps.
   */
  AUDIO_PROFILE_MUSIC_HIGH_QUALITY_STEREO = 5,
  /**
   * 6: A sample rate of 16 kHz, audio encoding, mono, and a bitrate of up to 64 Kbps.
   */
  AUDIO_PROFILE_IOT = 6,
  AUDIO_PROFILE_NUM = 7
};

/**
 * Audio application scenarios.
 */
enum AUDIO_SCENARIO_TYPE {
  /**
   * 0: (Recommended) The default audio scenario.
   */
  AUDIO_SCENARIO_DEFAULT = 0,
  /**
   * 3: (Recommended) The live gaming scenario, which needs to enable gaming
   * audio effects in the speaker. Choose this scenario to achieve high-fidelity
   * music playback.
   */
  AUDIO_SCENARIO_GAME_STREAMING = 3,
  /**
   * 5: The chatroom scenario, which needs to keep recording when setClientRole to audience.
   * Normally, app developer can also use mute api to achieve the same result,
   * and we implement this 'non-orthogonal' behavior only to make API backward compatible.
   */
  AUDIO_SCENARIO_CHATROOM = 5,
  /**
   * 7: Chorus
   */
  AUDIO_SCENARIO_CHORUS = 7,
  /**
   * 8: Meeting
   */
  AUDIO_SCENARIO_MEETING = 8,
  /**
   * 9: Reserved.
   */
  AUDIO_SCENARIO_NUM = 9,
};

/**
 * The definition of the VideoFormat struct.
 */
struct VideoFormat {
  OPTIONAL_ENUM_SIZE_T {
    /** The maximum value (px) of the width. */
    kMaxWidthInPixels = 3840,
    /** The maximum value (px) of the height. */
    kMaxHeightInPixels = 2160,
    /** The maximum value (fps) of the frame rate. */
    kMaxFps = 60,
  };

  /**
   * The width (px) of the video.
   */
  int width;   // Number of pixels.
  /**
   * The height (px) of the video.
   */
  int height;  // Number of pixels.
  /**
   * The video frame rate (fps).
   */
  int fps;
  VideoFormat() : width(FRAME_WIDTH_640), height(FRAME_HEIGHT_360), fps(FRAME_RATE_FPS_15) {}
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
  bool operator!=(const VideoFormat& fmt) const {
    return !operator==(fmt);
  }
};

/**
 * Video content hints.
 */
enum VIDEO_CONTENT_HINT {
  /**
   * (Default) No content hint. In this case, the SDK balances smoothness with sharpness.
   */
  CONTENT_HINT_NONE,
  /**
   * Choose this option if you prefer smoothness or when
   * you are sharing motion-intensive content such as a video clip, movie, or video game.
   *
   *
   */
  CONTENT_HINT_MOTION,
  /**
   * Choose this option if you prefer sharpness or when you are
   * sharing montionless content such as a picture, PowerPoint slide, ot text.
   *
   */
  CONTENT_HINT_DETAILS
};

enum SCREEN_SCENARIO_TYPE {
  SCREEN_SCENARIO_DOCUMENT = 1,
  SCREEN_SCENARIO_GAMING = 2,
  SCREEN_SCENARIO_VIDEO = 3,
  SCREEN_SCENARIO_RDC = 4,
};

/**
 * The brightness level of the video image captured by the local camera.
 */
enum CAPTURE_BRIGHTNESS_LEVEL_TYPE {
  /** -1: The SDK does not detect the brightness level of the video image.
   * Wait a few seconds to get the brightness level from `CAPTURE_BRIGHTNESS_LEVEL_TYPE` in the next callback.
   */
  CAPTURE_BRIGHTNESS_LEVEL_INVALID = -1,
  /** 0: The brightness level of the video image is normal.
   */
  CAPTURE_BRIGHTNESS_LEVEL_NORMAL = 0,
  /** 1: The brightness level of the video image is too bright.
   */
  CAPTURE_BRIGHTNESS_LEVEL_BRIGHT = 1,
  /** 2: The brightness level of the video image is too dark.
   */
  CAPTURE_BRIGHTNESS_LEVEL_DARK = 2,
};

/**
 * States of the local audio.
 */
enum LOCAL_AUDIO_STREAM_STATE {
  /**
   * 0: The local audio is in the initial state.
   */
  LOCAL_AUDIO_STREAM_STATE_STOPPED = 0,
  /**
   * 1: The audio recording device starts successfully.
   */
  LOCAL_AUDIO_STREAM_STATE_RECORDING = 1,
  /**
   * 2: The first audio frame is encoded successfully.
   */
  LOCAL_AUDIO_STREAM_STATE_ENCODING = 2,
  /**
   * 3: The local audio fails to start.
   */
  LOCAL_AUDIO_STREAM_STATE_FAILED = 3
};

/**
 * Reasons for the local audio failure.
 */
enum LOCAL_AUDIO_STREAM_ERROR {
  /**
   * 0: The local audio is normal.
   */
  LOCAL_AUDIO_STREAM_ERROR_OK = 0,
  /**
   * 1: No specified reason for the local audio failure.
   */
  LOCAL_AUDIO_STREAM_ERROR_FAILURE = 1,
  /**
   * 2: No permission to use the local audio device.
   */
  LOCAL_AUDIO_STREAM_ERROR_DEVICE_NO_PERMISSION = 2,
  /**
   * 3: The microphone is in use.
   */
  LOCAL_AUDIO_STREAM_ERROR_DEVICE_BUSY = 3,
  /**
   * 4: The local audio recording fails. Check whether the recording device
   * is working properly.
   */
  LOCAL_AUDIO_STREAM_ERROR_RECORD_FAILURE = 4,
  /**
   * 5: The local audio encoding fails.
   */
  LOCAL_AUDIO_STREAM_ERROR_ENCODE_FAILURE = 5,
  /** 6: The SDK cannot find the local audio recording device.
   */
  LOCAL_AUDIO_STREAM_ERROR_NO_RECORDING_DEVICE = 6,
  /** 7: The SDK cannot find the local audio playback device.
   */
  LOCAL_AUDIO_STREAM_ERROR_NO_PLAYOUT_DEVICE = 7,
  /**
   * 8: The local audio capturing is interrupted by the system call.
   */
  LOCAL_AUDIO_STREAM_ERROR_INTERRUPTED = 8,
  /** 9: An invalid audio capture device ID.
   */
  LOCAL_AUDIO_STREAM_ERROR_RECORD_INVALID_ID = 9,
  /** 10: An invalid audio playback device ID.
   */
  LOCAL_AUDIO_STREAM_ERROR_PLAYOUT_INVALID_ID = 10,
};

/** Local video state types.
 */
enum LOCAL_VIDEO_STREAM_STATE {
  /**
   * 0: The local video is in the initial state.
   */
  LOCAL_VIDEO_STREAM_STATE_STOPPED = 0,
  /**
   * 1: The capturer starts successfully.
   */
  LOCAL_VIDEO_STREAM_STATE_CAPTURING = 1,
  /**
   * 2: The first video frame is successfully encoded.
   */
  LOCAL_VIDEO_STREAM_STATE_ENCODING = 2,
  /**
   * 3: The local video fails to start.
   */
  LOCAL_VIDEO_STREAM_STATE_FAILED = 3
};

/**
 * Local video state error codes.
 */
enum LOCAL_VIDEO_STREAM_ERROR {
  /** 0: The local video is normal. */
  LOCAL_VIDEO_STREAM_ERROR_OK = 0,
  /** 1: No specified reason for the local video failure. */
  LOCAL_VIDEO_STREAM_ERROR_FAILURE = 1,
  /** 2: No permission to use the local video capturing device. */
  LOCAL_VIDEO_STREAM_ERROR_DEVICE_NO_PERMISSION = 2,
  /** 3: The local video capturing device is in use. */
  LOCAL_VIDEO_STREAM_ERROR_DEVICE_BUSY = 3,
  /** 4: The local video capture fails. Check whether the capturing device is working properly. */
  LOCAL_VIDEO_STREAM_ERROR_CAPTURE_FAILURE = 4,
  /** 5: The local video encoding fails. */
  LOCAL_VIDEO_STREAM_ERROR_ENCODE_FAILURE = 5,
  /** 6: The local video capturing device not avalible due to app did enter background.*/
  LOCAL_VIDEO_STREAM_ERROR_CAPTURE_INBACKGROUND = 6,
  /** 7: The local video capturing device not avalible because the app is running in a multi-app layout (generally on the pad) */
  LOCAL_VIDEO_STREAM_ERROR_CAPTURE_MULTIPLE_FOREGROUND_APPS = 7,
  /** 8: The local capture device cannot be found */
  LOCAL_VIDEO_STREAM_ERROR_DEVICE_NOT_FOUND = 8,
  /** 9: The local capture device is disconnected */
  LOCAL_VIDEO_STREAM_ERROR_DEVICE_DISCONNECTED = 9,
  /** 10:The local captue device id is invalid, for Windows and Mac only */
  LOCAL_VIDEO_STREAM_ERROR_DEVICE_INVALID_ID = 10,
  /** 101: The local video capturing device temporarily being made unavailable due to system pressure. */
  LOCAL_VIDEO_STREAM_ERROR_DEVICE_SYSTEM_PRESSURE = 101,
  /** 11: The local screen capture window is minimized. */
  LOCAL_VIDEO_STREAM_ERROR_SCREEN_CAPTURE_WINDOW_MINIMIZED = 11,
  /** 12: The local screen capture window is closed. */
  LOCAL_VIDEO_STREAM_ERROR_SCREEN_CAPTURE_WINDOW_CLOSED = 12,
  /** 13: The local screen capture window is occluded. */
  LOCAL_VIDEO_STREAM_ERROR_SCREEN_CAPTURE_WINDOW_OCCLUDED = 13,
  /** 20: The local screen capture window is not supported. */
  LOCAL_VIDEO_STREAM_ERROR_SCREEN_CAPTURE_WINDOW_NOT_SUPPORTED = 20,
};

/**
 * Remote audio states.
 */
enum REMOTE_AUDIO_STATE
{
  /**
   * 0: The remote audio is in the default state, probably due to
   * `REMOTE_AUDIO_REASON_LOCAL_MUTED(3)`,
   * `REMOTE_AUDIO_REASON_REMOTE_MUTED(5)`, or
   * `REMOTE_AUDIO_REASON_REMOTE_OFFLINE(7)`.
   */
  REMOTE_AUDIO_STATE_STOPPED = 0,  // Default state, audio is started or remote user disabled/muted audio stream
  /**
   * 1: The first remote audio packet is received.
   */
  REMOTE_AUDIO_STATE_STARTING = 1,  // The first audio frame packet has been received
  /**
   * 2: The remote audio stream is decoded and plays normally, probably
   * due to `REMOTE_AUDIO_REASON_NETWORK_RECOVERY(2)`,
   * `REMOTE_AUDIO_REASON_LOCAL_UNMUTED(4)`, or
   * `REMOTE_AUDIO_REASON_REMOTE_UNMUTED(6)`.
   */
  REMOTE_AUDIO_STATE_DECODING = 2,  // The first remote audio frame has been decoded or fronzen state ends
  /**
   * 3: The remote audio is frozen, probably due to
   * `REMOTE_AUDIO_REASON_NETWORK_CONGESTION(1)`.
   */
  REMOTE_AUDIO_STATE_FROZEN = 3,    // Remote audio is frozen, probably due to network issue
  /**
   * 4: The remote audio fails to start, probably due to
   * `REMOTE_AUDIO_REASON_INTERNAL(0)`.
   */
  REMOTE_AUDIO_STATE_FAILED = 4,    // Remote audio play failed
};

/**
 * Reasons for a remote audio state change.
 */
enum REMOTE_AUDIO_STATE_REASON
{
  /**
   * 0: Internal reasons.
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
   * 3: The local user stops receiving the remote audio stream or
   * disables the audio module.
   */
  REMOTE_AUDIO_REASON_LOCAL_MUTED = 3,
  /**
   * 4: The local user resumes receiving the remote audio stream or
   * enables the audio module.
   */
  REMOTE_AUDIO_REASON_LOCAL_UNMUTED = 4,
  /**
   * 5: The remote user stops sending the audio stream or disables the
   * audio module.
   */
  REMOTE_AUDIO_REASON_REMOTE_MUTED = 5,
  /**
   * 6: The remote user resumes sending the audio stream or enables the
   * audio module.
   */
  REMOTE_AUDIO_REASON_REMOTE_UNMUTED = 6,
  /**
   * 7: The remote user leaves the channel.
   */
  REMOTE_AUDIO_REASON_REMOTE_OFFLINE = 7,
};

/** The state of the remote video. */
enum REMOTE_VIDEO_STATE {
  /** 0: The remote video is in the default state, probably due to
   * #REMOTE_VIDEO_STATE_REASON_LOCAL_MUTED (3),
   * #REMOTE_VIDEO_STATE_REASON_REMOTE_MUTED (5),
   * #REMOTE_VIDEO_STATE_REASON_REMOTE_OFFLINE (7), or
   * #REMOTE_VIDEO_STATE_REASON_AUDIO_FALLBACK (8).
   */
  REMOTE_VIDEO_STATE_STOPPED = 0,
  /** 1: The first remote video packet is received.
   */
  REMOTE_VIDEO_STATE_STARTING = 1,
  /** 2: The remote video stream is decoded and plays normally, probably due to
   * #REMOTE_VIDEO_STATE_REASON_NETWORK_RECOVERY (2),
   * #REMOTE_VIDEO_STATE_REASON_LOCAL_UNMUTED (4),
   * #REMOTE_VIDEO_STATE_REASON_REMOTE_UNMUTED (6), or
   * #REMOTE_VIDEO_STATE_REASON_AUDIO_FALLBACK_RECOVERY (9).
   */
  REMOTE_VIDEO_STATE_DECODING = 2,
  /** 3: The remote video is frozen, probably due to
   * #REMOTE_VIDEO_STATE_REASON_NETWORK_CONGESTION (1).
   */
  REMOTE_VIDEO_STATE_FROZEN = 3,
  /** 4: The remote video fails to start, probably due to
   * #REMOTE_VIDEO_STATE_REASON_INTERNAL (0).
   */
  REMOTE_VIDEO_STATE_FAILED = 4,
};
/** The reason for the remote video state change. */
enum REMOTE_VIDEO_STATE_REASON {
  /**
  * 0: Internal reasons.
  */
  REMOTE_VIDEO_STATE_REASON_INTERNAL = 0,

  /**
  * 1: Network congestion.
  */
  REMOTE_VIDEO_STATE_REASON_NETWORK_CONGESTION = 1,

  /**
  * 2: Network recovery.
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

  /** 8: The remote audio-and-video stream falls back to the audio-only stream
   * due to poor network conditions.
   */
  REMOTE_VIDEO_STATE_REASON_AUDIO_FALLBACK = 8,

  /** 9: The remote audio-only stream switches back to the audio-and-video
   * stream after the network conditions improve.
   */
  REMOTE_VIDEO_STATE_REASON_AUDIO_FALLBACK_RECOVERY = 9,

  /** 10: The remote video stream type change to low stream type
   *  just for internal use
   */
  REMOTE_VIDEO_STATE_REASON_VIDEO_STREAM_TYPE_CHANGE_TO_LOW = 10,
  /** 11: The remote video stream type change to high stream type
   *  just for internal use
   */
  REMOTE_VIDEO_STATE_REASON_VIDEO_STREAM_TYPE_CHANGE_TO_HIGH = 11,
    /** 12: The remote user sdk(only for iOS) in background.
   */
  REMOTE_VIDEO_STATE_REASON_SDK_IN_BACKGROUND = 12,

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
  : isLocal(false), ownerUid(0), trackId(0), channelId(OPTIONAL_NULLPTR)
  , streamType(VIDEO_STREAM_HIGH), codecType(VIDEO_CODEC_H264)
  , encodedFrameOnly(false), sourceType(VIDEO_SOURCE_CAMERA_PRIMARY)
  , observationPosition(agora::media::base::POSITION_POST_CAPTURER) {}
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
   * The video stream type: #VIDEO_STREAM_TYPE.
   */
  VIDEO_STREAM_TYPE streamType;
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
 * The downscale level of the remote video stream . The higher the downscale level, the more the video downscales.
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
 * The definition of the AudioVolumeInfo struct.
 */
struct AudioVolumeInfo {
  /**
   * User ID of the speaker.
   */
  uid_t uid;

  /**
   * The volume of the speaker that ranges from 0 to 255.
   */
  unsigned int volume;  // [0,255]

  /*
   * The activity status of remote users
   */
  unsigned int vad;

  /**
   * Voice pitch frequency in Hz
   */
  double voicePitch;

  AudioVolumeInfo() : uid(0), volume(0), vad(0), voicePitch(0.0) {}
};

/**
 * The definition of the DeviceInfo struct
 */
struct DeviceInfo {
  /*
   * Whether the device support low latency audio. Not support by default
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
   * The definition of the Packet struct.
   */
  struct Packet {
    /**
     * The data buffer of the audio packet.
     */
    const unsigned char* buffer;
    /**
     * The size of the audio packet.
     */
    unsigned int size;

    Packet() : buffer(NULL), size(0) {}
  };
  /**
   * Occurs when the SDK is ready to send the audio packet.
   * @param packet The audio packet to be sent: Packet.
   * @return Whether to send the audio packet:
   * - true: Send the packet.
   * - false: Do not send the packet, in which case the audio packet will be discarded.
   */
  virtual bool onSendAudioPacket(Packet& packet) = 0;
  /**
   * Occurs when the SDK is ready to send the video packet.
   * @param packet The video packet to be sent: Packet.
   * @return Whether to send the video packet:
   * - true: Send the packet.
   * - false: Do not send the packet, in which case the audio packet will be discarded.
   */
  virtual bool onSendVideoPacket(Packet& packet) = 0;
  /**
   * Occurs when the audio packet is received.
   * @param packet The received audio packet: Packet.
   * @return Whether to process the audio packet:
   * - true: Process the packet.
   * - false: Do not process the packet, in which case the audio packet will be discarded.
   */
  virtual bool onReceiveAudioPacket(Packet& packet) = 0;
  /**
   * Occurs when the video packet is received.
   * @param packet The received video packet: Packet.
   * @return Whether to process the audio packet:
   * - true: Process the packet.
   * - false: Do not process the packet, in which case the video packet will be discarded.
   */
  virtual bool onReceiveVideoPacket(Packet& packet) = 0;
};

/**
 * Audio sample rate types.
 */
enum AUDIO_SAMPLE_RATE_TYPE {
  /**
   * 32000: 32 KHz.
   */
  AUDIO_SAMPLE_RATE_32000 = 32000,
  /**
   * 44100: 44.1 KHz.
   */
  AUDIO_SAMPLE_RATE_44100 = 44100,
  /**
   * 48000: 48 KHz.
   */
  AUDIO_SAMPLE_RATE_48000 = 48000,
};
enum VIDEO_CODEC_TYPE_FOR_STREAM {
  /**
   * 1: (Default) H.264
   */
  VIDEO_CODEC_H264_FOR_STREAM = 1,
  /**
   * 2: H.265
   */
  VIDEO_CODEC_H265_FOR_STREAM = 2,
};

/**
 * Video codec profile types.
 */
enum VIDEO_CODEC_PROFILE_TYPE {
  /**
   * 66: Baseline video codec profile. Generally used in video calls on mobile phones.
   */
  VIDEO_CODEC_PROFILE_BASELINE = 66,
  /**
   * 77: Main video codec profile. Generally used in mainstream electronics, such as MP4 players, portable video players, PSP, and iPads.
   */
  VIDEO_CODEC_PROFILE_MAIN = 77,
  /**
   * 100: (Default) High video codec profile. Generally used in high-resolution broadcasts or television.
   */
  VIDEO_CODEC_PROFILE_HIGH = 100,
};


/**
 * Audio codec profile types.
 */
enum AUDIO_CODEC_PROFILE_TYPE {
  /**
   * 0: (Default) LC-AAC, which is the low-complexity audio codec type.
   */
  AUDIO_CODEC_PROFILE_LC_AAC = 0,
  /**
   * 1: HE-AAC, which is the high-efficiency audio codec type.
   */
  AUDIO_CODEC_PROFILE_HE_AAC = 1,
  /**
   *  2: HE-AACv2, which is the high-efficiency audio codec type.
   */
  AUDIO_CODEC_PROFILE_HE_AAC_V2 = 2,
};

/**
 * The definition of the LocalAudioStats struct, which reports audio statistics of
 * the local user.
 */
struct LocalAudioStats
{
  /**
   * The number of audio channels.
   */
  int numChannels;
  /**
   * The sample rate (Hz).
   */
  int sentSampleRate;
  /**
   * The average sending bitrate (Kbps).
   */
  int sentBitrate;
  /**
   * The internal payload type
   */
  int internalCodec;
  /**
   * The audio packet loss rate (%) from the local client to the Agora edge server before applying the anti-packet loss strategies.
   */
  unsigned short txPacketLossRate;
  /**
   * The audio delay of the device, contains record and playout delay
   */
  int audioDeviceDelay;
};


/**
 States of the RTMP or RTMPS streaming.
 */
enum RTMP_STREAM_PUBLISH_STATE {
  /** The RTMP or RTMPS streaming has not started or has ended. This state is also triggered after you remove an RTMP or RTMPS stream from the CDN by calling `removePublishStreamUrl`.
   */
  RTMP_STREAM_PUBLISH_STATE_IDLE = 0,
  /** The SDK is connecting to Agora's streaming server and the CDN server. This state is triggered after you call the \ref IRtcEngine::addPublishStreamUrl "addPublishStreamUrl" method.
   */
  RTMP_STREAM_PUBLISH_STATE_CONNECTING = 1,
  /** The RTMP or RTMPS streaming publishes. The SDK successfully publishes the RTMP or RTMPS streaming and returns this state.
   */
  RTMP_STREAM_PUBLISH_STATE_RUNNING = 2,
  /** The RTMP or RTMPS streaming is recovering. When exceptions occur to the CDN, or the streaming is interrupted, the SDK tries to resume RTMP or RTMPS streaming and returns this state.

   - If the SDK successfully resumes the streaming, #RTMP_STREAM_PUBLISH_STATE_RUNNING (2) returns.
   - If the streaming does not resume within 60 seconds or server errors occur, #RTMP_STREAM_PUBLISH_STATE_FAILURE (4) returns. You can also reconnect to the server by calling the \ref IRtcEngine::removePublishStreamUrl "removePublishStreamUrl" and \ref IRtcEngine::addPublishStreamUrl "addPublishStreamUrl" methods.
   */
  RTMP_STREAM_PUBLISH_STATE_RECOVERING = 3,
  /** The RTMP or RTMPS streaming fails. See the errCode parameter for the detailed error information. You can also call the \ref IRtcEngine::addPublishStreamUrl "addPublishStreamUrl" method to publish the RTMP or RTMPS streaming again.
   */
  RTMP_STREAM_PUBLISH_STATE_FAILURE = 4,
  /** The SDK is disconnecting to Agora's streaming server and the CDN server. This state is triggered after you call the \ref IRtcEngine::removePublishStreamUrl "removePublishStreamUrl" method.
   */
  RTMP_STREAM_PUBLISH_STATE_DISCONNECTING = 5,
};

/**
 Error codes of the RTMP or RTMPS streaming.
 */
enum RTMP_STREAM_PUBLISH_ERROR_TYPE {
  /** The RTMP or RTMPS streaming publishes successfully. */
  RTMP_STREAM_PUBLISH_ERROR_OK = 0,
  /** Invalid argument used. If, for example, you do not call the \ref IRtcEngine::setLiveTranscoding "setLiveTranscoding" method to configure the LiveTranscoding parameters before calling the addPublishStreamUrl method, the SDK returns this error. Check whether you set the parameters in the *setLiveTranscoding* method properly. */
  RTMP_STREAM_PUBLISH_ERROR_INVALID_ARGUMENT = 1,
  /** The RTMP or RTMPS streaming is encrypted and cannot be published. */
  RTMP_STREAM_PUBLISH_ERROR_ENCRYPTED_STREAM_NOT_ALLOWED = 2,
  /** Timeout for the RTMP or RTMPS streaming. Call the \ref IRtcEngine::addPublishStreamUrl "addPublishStreamUrl" method to publish the streaming again. */
  RTMP_STREAM_PUBLISH_ERROR_CONNECTION_TIMEOUT = 3,
  /** An error occurs in Agora's streaming server. Call the `addPublishStreamUrl` method to publish the streaming again. */
  RTMP_STREAM_PUBLISH_ERROR_INTERNAL_SERVER_ERROR = 4,
  /** An error occurs in the CDN server. */
  RTMP_STREAM_PUBLISH_ERROR_RTMP_SERVER_ERROR = 5,
  /** The RTMP or RTMPS streaming publishes too frequently. */
  RTMP_STREAM_PUBLISH_ERROR_TOO_OFTEN = 6,
  /** The host publishes more than 10 URLs. Delete the unnecessary URLs before adding new ones. */
  RTMP_STREAM_PUBLISH_ERROR_REACH_LIMIT = 7,
  /** The host manipulates other hosts' URLs. Check your app logic. */
  RTMP_STREAM_PUBLISH_ERROR_NOT_AUTHORIZED = 8,
  /** Agora's server fails to find the RTMP or RTMPS streaming. */
  RTMP_STREAM_PUBLISH_ERROR_STREAM_NOT_FOUND = 9,
  /** The format of the RTMP or RTMPS streaming URL is not supported. Check whether the URL format is correct. */
  RTMP_STREAM_PUBLISH_ERROR_FORMAT_NOT_SUPPORTED = 10,
  /** Current role is not broadcaster. Check whether the role of the current channel. */
  RTMP_STREAM_PUBLISH_ERROR_NOT_BROADCASTER = 11,  // Note: match to ERR_PUBLISH_STREAM_NOT_BROADCASTER in AgoraBase.h
  /** Call updateTranscoding, but no mix stream. */
  RTMP_STREAM_PUBLISH_ERROR_TRANSCODING_NO_MIX_STREAM = 13,  // Note: match to ERR_PUBLISH_STREAM_TRANSCODING_NO_MIX_STREAM in AgoraBase.h
  /** Network error. */
  RTMP_STREAM_PUBLISH_ERROR_NET_DOWN = 14,  // Note: match to ERR_NET_DOWN in AgoraBase.h
  /** User AppId have not authorized to push stream. */
  RTMP_STREAM_PUBLISH_ERROR_INVALID_APPID = 15,  // Note: match to ERR_PUBLISH_STREAM_APPID_INVALID in AgoraBase.h
  /** invalid privilege. */
  RTMP_STREAM_PUBLISH_ERROR_INVALID_PRIVILEGE = 16,
  /**
   * 100: The streaming has been stopped normally. After you call
   * \ref IRtcEngine::removePublishStreamUrl "removePublishStreamUrl"
   * to stop streaming, the SDK returns this value.
   *
   * @since v3.4.5
   */
  RTMP_STREAM_UNPUBLISH_ERROR_OK = 100,
};

/** Events during the RTMP or RTMPS streaming. */
enum RTMP_STREAMING_EVENT {
  /** An error occurs when you add a background image or a watermark image to the RTMP or RTMPS stream.
   */
  RTMP_STREAMING_EVENT_FAILED_LOAD_IMAGE = 1,
  /** 2: The streaming URL is already being used for CDN live streaming. If you want to start new streaming, use a new streaming URL.
   *
   * @since v3.4.5
   */
  RTMP_STREAMING_EVENT_URL_ALREADY_IN_USE = 2,
  /** advanced feature not support
   */
  RTMP_STREAMING_EVENT_ADVANCED_FEATURE_NOT_SUPPORT = 3,
  /** Client request too frequently.
   */
  RTMP_STREAMING_EVENT_REQUEST_TOO_OFTEN = 4,
};

/** The definition of the RtcImage struct.
 */
typedef struct RtcImage {
  /**
   * The URL address of the watermark on the video.
   */
  const char* url;
  /**
   * The horizontal distance (pixel) between the watermark image's top-left corner and the video's
   * top-left corner.
   */
  int x;
  /**
   * The vertical distance (pixel) between the watermark image's top-left corner and the video's
   * top-left corner.
   */
  int y;
  /**
   * The width of the watermark on the video.
   */
  int width;
  /**
   * The height of the watermark on the video.
   */
  int height;
  /**
   * Order attribute for an ordering of overlapping two-dimensional objects.
   */
  int zOrder;
  /** The transparency level of the image. The value ranges between 0 and 1.0:

   * - 0: Completely transparent
   * - 1.0: (Default) Opaque
   */
  double alpha;

  RtcImage() : url(NULL), x(0), y(0), width(0), height(0), zOrder(0), alpha(1.0) {}
} RtcImage;

struct LiveStreamAdvancedFeature {
  LiveStreamAdvancedFeature() : featureName(NULL), opened(false) {}
  LiveStreamAdvancedFeature(const char* feat_name, bool open) : featureName(feat_name), opened(open) {}
  /** The advanced feature for high-quality video with a lower bitrate. */
  // static const char* LBHQ = "lbhq";
  /** The advanced feature for the optimized video encoder. */
  // static const char* VEO = "veo";

  /** The name of the advanced feature. It contains LBHQ and VEO.
   * "lbhq"
   * "veo"
   */
  const char* featureName;

  /** Whether to enable the advanced feature:
   * - true: Enable the advanced feature.
   * - false: (Default) Disable the advanced feature.
   */
  bool opened;
} ;

/**
 * Connection state types.
 */
enum CONNECTION_STATE_TYPE
{
  /**
   * 1: The SDK is disconnected from the server.
   */
  CONNECTION_STATE_DISCONNECTED = 1,
  /**
   * 2: The SDK is connecting to the server.
   */
  CONNECTION_STATE_CONNECTING = 2,
  /**
   * 3: The SDK is connected to the server and has joined a channel. You can now publish or subscribe to
   * a track in the channel.
   */
  CONNECTION_STATE_CONNECTED = 3,
  /**
   * 4: The SDK keeps rejoining the channel after being disconnected from the channel, probably because of
   * network issues.
   */
  CONNECTION_STATE_RECONNECTING = 4,
  /**
   * 5: The SDK fails to connect to the server or join the channel.
   */
  CONNECTION_STATE_FAILED = 5,
};

/**
 * The definition of the TranscodingUser struct.
 */
struct TranscodingUser {
  /**
   * User ID of the CDN live streaming.
   */
  uid_t uid;
  /**
   * The horizontal position of the top left corner of the video frame.
   */
  int x;
  /**
   * The vertical position of the top left corner of the video frame.
   */
  int y;
  /**
   * The width of the video frame.
   */
  int width;
  /**
   * The height of the video frame.
   */
  int height;
  /**
   * The layer of the video frame that ranges from 1 to 100:
  * - 1: (Default) The lowest layer.
  * - 100: The highest layer.
  */
  int zOrder;
  /**
   * The transparency of the video frame.
   */
  double alpha;
  /**
   * The audio channel of the sound that ranges from 0 to 5. Special players are needed if it is not set
   * as 0.
   * - 0: (default) Supports dual channels at most, depending on the upstream of the broadcaster.
   * - 1: The audio stream of the broadcaster is in the FL audio channel. If the upstream of the
   * broadcaster uses dual sound channel, only the left sound channel is used for streaming.
   * - 2: The audio stream of the broadcaster is in the FC audio channel. If the upstream of the
   * broadcaster uses dual sound channel, only the left sound channel is used for streaming.
   * - 3: The audio stream of the broadcaster is in the FR audio channel. If the upstream of the
   * broadcaster uses dual sound channel, only the left sound channel is used for streaming.
   * - 4: The audio stream of the broadcaster is in the BL audio channel. If the upstream of the
   * broadcaster uses dual sound channel, only the left sound channel is used for streaming.
   * - 5: The audio stream of the broadcaster is in the BR audio channel. If the upstream of the
   * broadcaster uses dual sound channel, only the left sound channel is used for streaming.
  */
  int audioChannel;
  TranscodingUser()
      : uid(0),
        x(0),
        y(0),
        width(0),
        height(0),
        zOrder(0),
        alpha(1.0),
        audioChannel(0) {}
};

/**
 * The definition of the LiveTranscoding struct.
 */
struct LiveTranscoding {
  /** The width of the video in pixels. The default value is 360.
   * - When pushing video streams to the CDN, the value range of `width` is [64,1920].
   * If the value is less than 64, Agora server automatically adjusts it to 64; if the
   * value is greater than 1920, Agora server automatically adjusts it to 1920.
   * - When pushing audio streams to the CDN, set `width` and `height` as 0.
   */
  int width;
  /** The height of the video in pixels. The default value is 640.
   * - When pushing video streams to the CDN, the value range of `height` is [64,1080].
   * If the value is less than 64, Agora server automatically adjusts it to 64; if the
   * value is greater than 1080, Agora server automatically adjusts it to 1080.
   * - When pushing audio streams to the CDN, set `width` and `height` as 0.
   */
  int height;
  /** Bitrate of the CDN live output video stream. The default value is 400 Kbps.

  Set this parameter according to the Video Bitrate Table. If you set a bitrate beyond the proper range, the SDK automatically adapts it to a value within the range.
  */
  int videoBitrate;
  /** Frame rate of the output video stream set for the CDN live streaming. The default value is 15 fps, and the value range is (0,30].

  @note The Agora server adjusts any value over 30 to 30.
  */
  int videoFramerate;

  /** **DEPRECATED** Latency mode:

   - true: Low latency with unassured quality.
   - false: (Default) High latency with assured quality.
   */
  bool lowLatency;

  /** Video GOP in frames. The default value is 30 fps.
   */
  int videoGop;
  /** Self-defined video codec profile: #VIDEO_CODEC_PROFILE_TYPE.

  @note If you set this parameter to other values, Agora adjusts it to the default value of 100.
  */
  VIDEO_CODEC_PROFILE_TYPE videoCodecProfile;
  /** The background color in RGB hex value. Value only. Do not include a preceeding #. For example, 0xFFB6C1 (light pink). The default value is 0x000000 (black).
   */
  unsigned int backgroundColor;

  /** video codec type */
  VIDEO_CODEC_TYPE_FOR_STREAM videoCodecType;

  /** The number of users in the live interactive streaming.
   *  The value range is [0, 17].
   */
  unsigned int userCount;
  /** TranscodingUser
   */
  TranscodingUser* transcodingUsers;
  /** Reserved property. Extra user-defined information to send SEI for the H.264/H.265 video stream to the CDN live client. Maximum length: 4096 Bytes.

   For more information on SEI frame, see [SEI-related questions](https://docs.agora.io/en/faq/sei).
   */
  const char* transcodingExtraInfo;

  /** **DEPRECATED** The metadata sent to the CDN live client defined by the RTMP or HTTP-FLV metadata.
   */
  const char* metadata;
  /** The watermark image added to the CDN live publishing stream.

  Ensure that the format of the image is PNG. Once a watermark image is added, the audience of the CDN live publishing stream can see the watermark image. See RtcImage.
  */
  RtcImage* watermark;

  /**
   * The variables means the count of watermark.
   * if watermark is array, watermarkCount is count of watermark.
   * if watermark is just a pointer, watermarkCount pointer to object address. At the same time, watermarkCount must be 0 or 1.
   * default value: 0, compatible with old user-api
   * The value range is [0, 100].
   */
  unsigned int watermarkCount;

  /** The background image added to the CDN live publishing stream.

   Once a background image is added, the audience of the CDN live publishing stream can see the background image. See RtcImage.
  */
  RtcImage* backgroundImage;
  /**
   * The variables means the count of backgroundImage.
   * if backgroundImage is array, backgroundImageCount is count of backgroundImage.
   * if backgroundImage is just a pointer, backgroundImageCount pointer to object address. At the same time, backgroundImageCount must be 0 or 1.
   * default value: 0, compatible with old user-api
   * The value range is [0, 100].
   */
  unsigned int backgroundImageCount;

  /** Self-defined audio-sample rate: #AUDIO_SAMPLE_RATE_TYPE.
   */
  AUDIO_SAMPLE_RATE_TYPE audioSampleRate;
  /** Bitrate of the CDN live audio output stream. The default value is 48 Kbps, and the highest value is 128.
   */
  int audioBitrate;
  /** The numbder of audio channels for the CDN live stream. Agora recommends choosing 1 (mono), or 2 (stereo) audio channels. Special players are required if you choose option 3, 4, or 5:

   - 1: (Default) Mono.
   - 2: Stereo.
   - 3: Three audio channels.
   - 4: Four audio channels.
   - 5: Five audio channels.
   */
  int audioChannels;
  /** Self-defined audio codec profile: #AUDIO_CODEC_PROFILE_TYPE.
   */

  AUDIO_CODEC_PROFILE_TYPE audioCodecProfile;
  /// @cond
  /** Advanced features of the RTMP or RTMPS streaming with transcoding. See LiveStreamAdvancedFeature.
   *
   * @since v3.1.0
   */
  LiveStreamAdvancedFeature* advancedFeatures;

  /** The number of enabled advanced features. The default value is 0. */
  unsigned int advancedFeatureCount;
  /// @endcond
  LiveTranscoding() : width(360), height(640), videoBitrate(400), videoFramerate(15), lowLatency(false), videoGop(30), videoCodecProfile(VIDEO_CODEC_PROFILE_HIGH), backgroundColor(0x000000), videoCodecType(VIDEO_CODEC_H264_FOR_STREAM), userCount(0), transcodingUsers(NULL), transcodingExtraInfo(NULL), metadata(NULL), watermark(NULL), watermarkCount(0), backgroundImage(NULL), backgroundImageCount(0), audioSampleRate(AUDIO_SAMPLE_RATE_48000), audioBitrate(48), audioChannels(1), audioCodecProfile(AUDIO_CODEC_PROFILE_LC_AAC), advancedFeatures(NULL), advancedFeatureCount(0) {}
};

/**
 * The definition of the LocalTranscodingVideoStream struct.
 */
struct TranscodingVideoStream {
  /**
   * Source type of video stream.
   */
  agora::media::MEDIA_SOURCE_TYPE sourceType;
  /**
   * Remote user uid if sourceType is VIDEO_SOURCE_REMOTE.
   */
  uid_t remoteUserUid;
  /**
   * RTC image if sourceType is VIDEO_SOURCE_RTC_IMAGE.
   */
  const char* imageUrl;
  /**
   * The horizontal position of the top left corner of the video frame.
   */
  int x;
  /**
   * The vertical position of the top left corner of the video frame.
   */
  int y;
  /**
   * The width of the video frame.
   */
  int width;
  /**
   * The height of the video frame.
   */
  int height;
  /**
   * The layer of the video frame that ranges from 1 to 100:
  * - 1: (Default) The lowest layer.
  * - 100: The highest layer.
  */
  int zOrder;
  /**
   * The transparency of the video frame.
   */
  double alpha;
  /**
   * mirror of the source video frame (only valid for camera streams)
   */
  bool mirror;

  TranscodingVideoStream()
    : sourceType(agora::media::PRIMARY_CAMERA_SOURCE),
      remoteUserUid(0),
      imageUrl(NULL),
      x(0),
      y(0),
      width(0),
      height(0),
      zOrder(0),
      alpha(1.0),
      mirror(false) {}
};


/**
 * The definition of the LocalTranscodingConfiguration struct.
 */
struct LocalTranscoderConfiguration {
  /**
   * The number of VideoInputStreams in the transcoder.
   */
  unsigned int streamCount;
  /**
   * The video stream layout configuration in the transcoder.
   */
  TranscodingVideoStream* VideoInputStreams;
  /**
   * The video encoder configuration of transcoded video.
   */
  VideoEncoderConfiguration videoOutputConfiguration;

  LocalTranscoderConfiguration()
    : streamCount(0),
      VideoInputStreams(NULL),
      videoOutputConfiguration() {}
};

/**
 * The definition of the LastmileProbeConfig struct.
 */
struct LastmileProbeConfig {
  /**
   * Determines whether to test the uplink network. Some users, for example,
   * the audience in a live broadcast channel, do not need such a test:
   * - true: Test.
   * - false: Do not test.
   */
  bool probeUplink;
  /**
   * Determines whether to test the downlink network:
   * - true: Test.
   * - false: Do not test.
   */
  bool probeDownlink;
  /**
   * The expected maximum sending bitrate (bps) of the local user. The value
   * ranges between 100000 and 5000000. We recommend setting this parameter
   * according to the bitrate value set by `setVideoEncoderConfiguration`.
   */
  unsigned int expectedUplinkBitrate;
  /**
   * The expected maximum receiving bitrate (bps) of the local user. The value
   * ranges between 100000 and 5000000.
   */
  unsigned int expectedDownlinkBitrate;
};

/**
 * States of the last mile network probe result.
 */
enum LASTMILE_PROBE_RESULT_STATE {
  /**
   * 1: The probe result is complete.
   */
  LASTMILE_PROBE_RESULT_COMPLETE = 1,
  /**
   * 2: The probe result is incomplete and bandwidth estimation is not
   * available, probably due to temporary limited test resources.
   */
  LASTMILE_PROBE_RESULT_INCOMPLETE_NO_BWE = 2,
  /**
   * 3: The probe result is not available, probably due to poor network
   * conditions.
   */
  LASTMILE_PROBE_RESULT_UNAVAILABLE = 3
};

/**
 * The definition of the LastmileProbeOneWayResult struct, which reports the uplink or downlink
 * last-mile network probe test result.
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

  LastmileProbeOneWayResult() : packetLossRate(0),
                                jitter(0),
                                availableBandwidth(0) {}
};

/**
 * The definition of the LastmileProbeResult struct, which reports the uplink and downlink last-mile
 * network probe test result.
 */
struct LastmileProbeResult {
  /**
   * The state of last-mile network probe test: #LASTMILE_PROBE_RESULT_STATE.
   */
  LASTMILE_PROBE_RESULT_STATE state;
  /**
   * The uplink last-mile network probe test result: LastmileProbeOneWayResult.
   */
  LastmileProbeOneWayResult uplinkReport;
  /**
   * The downlink last-mile network probe test result: LastmileProbeOneWayResult.
   */
  LastmileProbeOneWayResult downlinkReport;
  /**
   * The round-trip delay time (ms).
   */
  unsigned int rtt;

  LastmileProbeResult()
    : state(LASTMILE_PROBE_RESULT_UNAVAILABLE),
      rtt(0) {}
};

/**
 * Reasons for a connection state change.
 */
enum CONNECTION_CHANGED_REASON_TYPE
{
  /**
   * 0: The SDK is connecting to the server.
   */
  CONNECTION_CHANGED_CONNECTING = 0,
  /**
   * 1: The SDK has joined the channel successfully.
   */
  CONNECTION_CHANGED_JOIN_SUCCESS = 1,
  /**
   * 2: The connection between the SDK and the server is interrupted.
   */
  CONNECTION_CHANGED_INTERRUPTED = 2,
  /**
   * 3: The connection between the SDK and the server is banned by the server.
   */
  CONNECTION_CHANGED_BANNED_BY_SERVER = 3,
  /**
   * 4: The SDK fails to join the channel for more than 20 minutes and stops reconnecting to the channel.
   */
  CONNECTION_CHANGED_JOIN_FAILED = 4,
  /**
   * 5: The SDK has left the channel.
   */
  CONNECTION_CHANGED_LEAVE_CHANNEL = 5,
  /**
   * 6: The connection fails because the App ID is not valid.
   */
  CONNECTION_CHANGED_INVALID_APP_ID = 6,
  /**
   * 7: The connection fails because the channel name is not valid.
   */
  CONNECTION_CHANGED_INVALID_CHANNEL_NAME = 7,
  /**
   * 8: The connection fails because the token is not valid.
   */
  CONNECTION_CHANGED_INVALID_TOKEN = 8,
  /**
   * 9: The connection fails because the token has expired.
   */
  CONNECTION_CHANGED_TOKEN_EXPIRED = 9,
  /**
   * 10: The connection is rejected by the server.
   */
  CONNECTION_CHANGED_REJECTED_BY_SERVER = 10,
  /**
   * 11: The connection changes to reconnecting because the SDK has set a proxy server.
   */
  CONNECTION_CHANGED_SETTING_PROXY_SERVER = 11,
  /**
   * 12: When the connection state changes because the app has renewed the token.
   */
  CONNECTION_CHANGED_RENEW_TOKEN = 12,
  /**
   * 13: The IP Address of the app has changed. A change in the network type or IP/Port changes the IP
   * address of the app.
   */
  CONNECTION_CHANGED_CLIENT_IP_ADDRESS_CHANGED = 13,
  /**
   * 14: A timeout occurs for the keep-alive of the connection between the SDK and the server.
   */
  CONNECTION_CHANGED_KEEP_ALIVE_TIMEOUT = 14,
  /**
   * 15: The SDK has rejoined the channel successfully.
   */
  CONNECTION_CHANGED_REJOIN_SUCCESS = 15,
  /**
   * 16: The connection between the SDK and the server is lost.
   */
  CONNECTION_CHANGED_LOST = 16,
  /**
   * 17: The change of connection state is caused by echo test.
   */
  CONNECTION_CHANGED_ECHO_TEST = 17,
  /**
   * 18: The local IP Address is changed by user.
   */
  CONNECTION_CHANGED_CLIENT_IP_ADDRESS_CHANGED_BY_USER = 18,
  /**
   * 19: The connection is failed due to join the same channel on another device with the same uid.
   */
  CONNECTION_CHANGED_SAME_UID_LOGIN = 19,
  /**
   * 20: The connection is failed due to too many broadcasters in the channel.
   */
  CONNECTION_CHANGED_TOO_MANY_BROADCASTERS = 20,
};

/**
 * The reason of changing role's failure.
 */
enum CLIENT_ROLE_CHANGE_FAILED_REASON {
  /**
   * 1: Too many broadcasters in the channel.
   */
  CLIENT_ROLE_CHANGE_FAILED_TOO_MANY_BROADCASTERS = 1,
  /**
   * 2: The operation of changing role is not authorized.
   */
  CLIENT_ROLE_CHANGE_FAILED_NOT_AUTHORIZED = 2,
  /**
   * 3: The operation of changing role is timeout.
   */
  CLIENT_ROLE_CHANGE_FAILED_REQUEST_TIME_OUT = 3,
  /**
   * 4: The operation of changing role is interrupted since we lost connection with agora service.
   */
  CLIENT_ROLE_CHANGE_FAILED_CONNECTION_FAILED = 4,
};

/**
 * The reason of notifying the user of a message.
 */
enum WLACC_MESSAGE_REASON {
  /**
   * WIFI signal is weak.
   */
  WLACC_MESSAGE_REASON_WEAK_SIGNAL = 0,
  /**
   * Channel congestion.
   */
  WLACC_MESSAGE_REASON_CHANNEL_CONGESTION = 1,
};

/**
 * Suggest an action for the user.
 */
enum WLACC_SUGGEST_ACTION {
  /**
   * Please get close to AP.
   */
  WLACC_SUGGEST_ACTION_CLOSE_TO_WIFI = 0,
  /**
   * The user is advised to connect to the prompted SSID.
   */
  WLACC_SUGGEST_ACTION_CONNECT_SSID = 1,
  /**
   * The user is advised to check whether the AP supports 5G band and enable 5G band (the aciton link is attached), or purchases an AP that supports 5G. AP does not support 5G band.
   */
  WLACC_SUGGEST_ACTION_CHECK_5G = 2,
  /**
   * The user is advised to change the SSID of the 2.4G or 5G band (the aciton link is attached). The SSID of the 2.4G band AP is the same as that of the 5G band.
   */
  WLACC_SUGGEST_ACTION_MODIFY_SSID = 3,
};

/**
 * Indicator optimization degree.
 */
struct WlAccStats {
  /**
   * End-to-end delay optimization percentage.
   */
  unsigned short e2eDelayPercent;
  /**
   * Frozen Ratio optimization percentage.
   */
  unsigned short frozenRatioPercent;
  /**
   * Loss Rate optimization percentage.
   */
  unsigned short lossRatePercent;
};

/**
 * The network type.
 */
enum NETWORK_TYPE {
  /**
   * -1: The network type is unknown.
   */
  NETWORK_TYPE_UNKNOWN = -1,
  /**
   * 0: The network type is disconnected.
   */
  NETWORK_TYPE_DISCONNECTED = 0,
  /**
   * 1: The network type is LAN.
   */
  NETWORK_TYPE_LAN = 1,
  /**
   * 2: The network type is Wi-Fi.
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
};

/**
 * The mode of setting up video views.
 */
enum VIDEO_VIEW_SETUP_MODE {
  /**
   * 0: replace one view
   */
  VIDEO_VIEW_SETUP_REPLACE = 0,
  /**
   * 1: add one view
   */
  VIDEO_VIEW_SETUP_ADD = 1,
  /**
   * 2: remove one view
   */
  VIDEO_VIEW_SETUP_REMOVE = 2,
};

/**
 * The definition of the VideoCanvas struct, which contains the information of the video display window.
 */
struct VideoCanvas {
  /**
   * The video display window.
   */
  view_t view;
  /**
   * The video display mode: \ref agora::media::base::RENDER_MODE_TYPE "RENDER_MODE_TYPE".
   */
  media::base::RENDER_MODE_TYPE renderMode;
  /**
   * The video mirror mode:
   */
  VIDEO_MIRROR_MODE_TYPE mirrorMode;
  /**
   * The user ID.
   */
  uid_t uid;
  bool isScreenView;

  void* priv;  // private data (underlying video engine denotes it)

  size_t priv_size;

  VIDEO_SOURCE_TYPE sourceType;

  Rectangle cropArea;

  VIDEO_VIEW_SETUP_MODE setupMode;

  VideoCanvas() : view(NULL), renderMode(media::base::RENDER_MODE_HIDDEN), mirrorMode(VIDEO_MIRROR_MODE_AUTO),
    uid(0), isScreenView(false), priv(NULL), priv_size(0), sourceType(VIDEO_SOURCE_CAMERA_PRIMARY), setupMode(VIDEO_VIEW_SETUP_REPLACE) {}
  VideoCanvas(view_t v, media::base::RENDER_MODE_TYPE m, VIDEO_MIRROR_MODE_TYPE mt, uid_t u)
    : view(v), renderMode(m), mirrorMode(mt), uid(u), isScreenView(false), priv(NULL), priv_size(0),
      sourceType(VIDEO_SOURCE_CAMERA_PRIMARY), setupMode(VIDEO_VIEW_SETUP_REPLACE) {}
  VideoCanvas(view_t v, media::base::RENDER_MODE_TYPE m, VIDEO_MIRROR_MODE_TYPE mt, user_id_t)
    : view(v), renderMode(m), mirrorMode(mt), uid(0), isScreenView(false), priv(NULL), priv_size(0),
      sourceType(VIDEO_SOURCE_CAMERA_PRIMARY), setupMode(VIDEO_VIEW_SETUP_REPLACE) {}
};

/** Beauty options.
 */
struct BeautyOptions {
  /** The contrast level, used with the @p lightening parameter.
    */
  enum LIGHTENING_CONTRAST_LEVEL {
      /** Low contrast level. */
      LIGHTENING_CONTRAST_LOW = 0,
      /** (Default) Normal contrast level. */
      LIGHTENING_CONTRAST_NORMAL = 1,
      /** High contrast level. */
      LIGHTENING_CONTRAST_HIGH = 2,
  };

  /** The contrast level, used with the @p lightening parameter.
    */
  LIGHTENING_CONTRAST_LEVEL lighteningContrastLevel;

  /** The brightness level. The value ranges from 0.0 (original) to 1.0. */
  float lighteningLevel;

  /** The smoothness level. The value ranges between 0 (original) and 1. This parameter is usually used to remove blemishes.
    */
  float smoothnessLevel;

  /** The redness level. The value ranges between 0 (original) and 1. This parameter adjusts the red saturation level.
    */
  float rednessLevel;

  /** The sharpness level. The value ranges between 0 (original) and 1.
  */
  float sharpnessLevel;

  BeautyOptions(LIGHTENING_CONTRAST_LEVEL contrastLevel, float lightening, float smoothness, float redness, float sharpness) : lighteningContrastLevel(contrastLevel), lighteningLevel(lightening), smoothnessLevel(smoothness), rednessLevel(redness), sharpnessLevel(sharpness) {}

  BeautyOptions() : lighteningContrastLevel(LIGHTENING_CONTRAST_NORMAL), lighteningLevel(0), smoothnessLevel(0), rednessLevel(0), sharpnessLevel(0) {}
};

struct LowlightEnhanceOptions {
  /**
   * The low-light enhancement mode.
   */
  enum LOW_LIGHT_ENHANCE_MODE {
    /** 0: (Default) Automatic mode. The SDK automatically enables or disables the low-light enhancement feature according to the ambient light to compensate for the lighting level or prevent overexposure, as necessary. */
    LOW_LIGHT_ENHANCE_AUTO = 0,
    /** Manual mode. Users need to enable or disable the low-light enhancement feature manually. */
    LOW_LIGHT_ENHANCE_MANUAL = 1,
  };
  /**
   * The low-light enhancement level.
   */
  enum LOW_LIGHT_ENHANCE_LEVEL {
    /**
     * 0: (Default) Promotes video quality during low-light enhancement. It processes the brightness, details, and noise of the video image. The performance consumption is moderate, the processing speed is moderate, and the overall video quality is optimal.
     */
    LOW_LIGHT_ENHANCE_LEVEL_HIGH_QUALITY = 0,
    /**
     * Promotes performance during low-light enhancement. It processes the brightness and details of the video image. The processing speed is faster.
     */
    LOW_LIGHT_ENHANCE_LEVEL_FAST = 1,
  };

  /** The low-light enhancement mode. See #LOW_LIGHT_ENHANCE_MODE.
   */
  LOW_LIGHT_ENHANCE_MODE mode;

  /** The low-light enhancement level. See #LOW_LIGHT_ENHANCE_LEVEL.
   */
  LOW_LIGHT_ENHANCE_LEVEL level;

  LowlightEnhanceOptions(LOW_LIGHT_ENHANCE_MODE lowlightMode, LOW_LIGHT_ENHANCE_LEVEL lowlightLevel) : mode(lowlightMode), level(lowlightLevel) {}

  LowlightEnhanceOptions() : mode(LOW_LIGHT_ENHANCE_AUTO), level(LOW_LIGHT_ENHANCE_LEVEL_HIGH_QUALITY) {}
};
/**
 * The video noise reduction options.
 *
 * @since v4.0.0
 */
struct VideoDenoiserOptions {
  /** The video noise reduction mode.
   */
  enum VIDEO_DENOISER_MODE {
    /** 0: (Default) Automatic mode. The SDK automatically enables or disables the video noise reduction feature according to the ambient light. */
    VIDEO_DENOISER_AUTO = 0,
    /** Manual mode. Users need to enable or disable the video noise reduction feature manually. */
    VIDEO_DENOISER_MANUAL = 1,
  };
  /**
   * The video noise reduction level.
   */
  enum VIDEO_DENOISER_LEVEL {
    /**
     * 0: (Default) Promotes video quality during video noise reduction. `HIGH_QUALITY` balances performance consumption and video noise reduction quality.
     * The performance consumption is moderate, the video noise reduction speed is moderate, and the overall video quality is optimal.
     */
    VIDEO_DENOISER_LEVEL_HIGH_QUALITY = 0,
    /**
     * Promotes reducing performance consumption during video noise reduction. `FAST` prioritizes reducing performance consumption over video noise reduction quality.
     * The performance consumption is lower, and the video noise reduction speed is faster. To avoid a noticeable shadowing effect (shadows trailing behind moving objects) in the processed video, Agora recommends that you use `FAST` when the camera is fixed.
     */
    VIDEO_DENOISER_LEVEL_FAST = 1,
    /**
     * Enhanced video noise reduction. `STRENGTH` prioritizes video noise reduction quality over reducing performance consumption.
     * The performance consumption is higher, the video noise reduction speed is slower, and the video noise reduction quality is better.
     * If `HIGH_QUALITY` is not enough for your video noise reduction needs, you can use `STRENGTH`.
     */
    VIDEO_DENOISER_LEVEL_STRENGTH = 2,
  };
  /** The video noise reduction mode. See #VIDEO_DENOISER_MODE.
   */
  VIDEO_DENOISER_MODE mode;

  /** The video noise reduction level. See #VIDEO_DENOISER_LEVEL.
   */
  VIDEO_DENOISER_LEVEL level;

  VideoDenoiserOptions(VIDEO_DENOISER_MODE denoiserMode, VIDEO_DENOISER_LEVEL denoiserLevel) : mode(denoiserMode), level(denoiserLevel) {}

  VideoDenoiserOptions() : mode(VIDEO_DENOISER_AUTO), level(VIDEO_DENOISER_LEVEL_HIGH_QUALITY) {}
};

/** The color enhancement options.
 *
 * @since v4.0.0
 */
struct ColorEnhanceOptions {
  /** The level of color enhancement. The value range is [0.0,1.0]. `0.0` is the default value, which means no color enhancement is applied to the video. The higher the value, the higher the level of color enhancement.
   */
  float strengthLevel;

  /** The level of skin tone protection. The value range is [0.0,1.0]. `0.0` means no skin tone protection. The higher the value, the higher the level of skin tone protection.
   * The default value is `1.0`. When the level of color enhancement is higher, the portrait skin tone can be significantly distorted, so you need to set the level of skin tone protection; when the level of skin tone protection is higher, the color enhancement effect can be slightly reduced.
   * Therefore, to get the best color enhancement effect, Agora recommends that you adjust `strengthLevel` and `skinProtectLevel` to get the most appropriate values.
   */
  float skinProtectLevel;

  ColorEnhanceOptions(float stength, float skinProtect) : strengthLevel(stength), skinProtectLevel(skinProtect) {}

  ColorEnhanceOptions() : strengthLevel(0), skinProtectLevel(1) {}
};


struct VirtualBackgroundSource {
  /** The type of the custom background image.
   */
  enum BACKGROUND_SOURCE_TYPE {
    /**
     * 1: (Default) The background image is a solid color.
     */
    BACKGROUND_COLOR = 1,
    /**
     * The background image is a file in PNG or JPG format.
     */
    BACKGROUND_IMG = 2,
    /** Background source is blur background besides human body*/
    BACKGROUND_BLUR = 3,
  };

  /** The blur degree used to blur background in different level.(foreground keeps same as before).
   */
  enum BACKGROUND_BLUR_DEGREE {
    /** blur degree level low, background can see things, but have some blur effect */
    BLUR_DEGREE_LOW = 1,
    /** blur degree level medium, blur more than level medium */
    BLUR_DEGREE_MEDIUM = 2,
    /** blur degree level high, blur default, hard to find background */
    BLUR_DEGREE_HIGH = 3,
  };

  /** The type of the custom background image. See #BACKGROUND_SOURCE_TYPE.
   */
  BACKGROUND_SOURCE_TYPE background_source_type;

  /**
   * The color of the custom background image. The format is a hexadecimal integer defined by RGB, without the # sign,
   * such as 0xFFB6C1 for light pink. The default value is 0xFFFFFF, which signifies white. The value range
   * is [0x000000,0xFFFFFF]. If the value is invalid, the SDK replaces the original background image with a white
   * background image.
   *
   * @note This parameter takes effect only when the type of the custom background image is `BACKGROUND_COLOR`.
   */
  unsigned int color;

  /**
   * The local absolute path of the custom background image. PNG and JPG formats are supported. If the path is invalid,
   * the SDK replaces the original background image with a white background image.
   *
   * @note This parameter takes effect only when the type of the custom background image is `BACKGROUND_IMG`.
   */
  const char* source;

  /** blur degree */
  BACKGROUND_BLUR_DEGREE blur_degree;

  VirtualBackgroundSource() : background_source_type(BACKGROUND_COLOR), color(0xffffff), source(NULL),  blur_degree(BLUR_DEGREE_HIGH) {}
};

struct SegmentationProperty {

    enum SEG_MODEL_TYPE {

    SEG_MODEL_AI = 1,
    SEG_MODEL_GREEN = 2
  };

  SEG_MODEL_TYPE modelType;

  float greenCapacity;


  SegmentationProperty() : modelType(SEG_MODEL_AI), greenCapacity(0.5){}
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
/** The options for SDK preset voice beautifier effects.
 */
enum VOICE_BEAUTIFIER_PRESET {
  /** Turn off voice beautifier effects and use the original voice.
   */
  VOICE_BEAUTIFIER_OFF = 0x00000000,
  /** A more magnetic voice.
   *
   * @note Agora recommends using this enumerator to process a male-sounding voice; otherwise, you
   * may experience vocal distortion.
   */
  CHAT_BEAUTIFIER_MAGNETIC = 0x01010100,
  /** A fresher voice.
   *
   * @note Agora recommends using this enumerator to process a female-sounding voice; otherwise, you
   * may experience vocal distortion.
   */
  CHAT_BEAUTIFIER_FRESH = 0x01010200,
  /** A more vital voice.
   *
   * @note Agora recommends using this enumerator to process a female-sounding voice; otherwise, you
   * may experience vocal distortion.
   */
  CHAT_BEAUTIFIER_VITALITY = 0x01010300,
  /**
   * @since v3.3.0
   *
   * Singing beautifier effect.
   * - If you call \ref IRtcEngine::setVoiceBeautifierPreset "setVoiceBeautifierPreset"
   * (SINGING_BEAUTIFIER), you can beautify a male-sounding voice and add a reverberation effect
   * that sounds like singing in a small room. Agora recommends not using \ref
   * IRtcEngine::setVoiceBeautifierPreset "setVoiceBeautifierPreset" (SINGING_BEAUTIFIER) to process
   * a female-sounding voice; otherwise, you may experience vocal distortion.
   * - If you call \ref IRtcEngine::setVoiceBeautifierParameters
   * "setVoiceBeautifierParameters"(SINGING_BEAUTIFIER, param1, param2), you can beautify a male- or
   * female-sounding voice and add a reverberation effect.
   */
  SINGING_BEAUTIFIER = 0x01020100,
  /** A more vigorous voice.
   */
  TIMBRE_TRANSFORMATION_VIGOROUS = 0x01030100,
  /** A deeper voice.
   */
  TIMBRE_TRANSFORMATION_DEEP = 0x01030200,
  /** A mellower voice.
   */
  TIMBRE_TRANSFORMATION_MELLOW = 0x01030300,
  /** A falsetto voice.
   */
  TIMBRE_TRANSFORMATION_FALSETTO = 0x01030400,
  /** A fuller voice.
   */
  TIMBRE_TRANSFORMATION_FULL = 0x01030500,
  /** A clearer voice.
   */
  TIMBRE_TRANSFORMATION_CLEAR = 0x01030600,
  /** A more resounding voice.
   */
  TIMBRE_TRANSFORMATION_RESOUNDING = 0x01030700,
  /** A more ringing voice.
   */
  TIMBRE_TRANSFORMATION_RINGING = 0x01030800,

  ULTRA_HIGH_QUALITY_VOICE = 0x01040100
};

/** The options for SDK preset audio effects.
 */
enum AUDIO_EFFECT_PRESET {
  /** Turn off audio effects and use the original voice.
   */
  AUDIO_EFFECT_OFF = 0x00000000,
  /** An audio effect typical of a KTV venue.
   *
   * @note To achieve better audio effect quality, Agora recommends calling \ref
   * IRtcEngine::setAudioProfile "setAudioProfile" and setting the `profile` parameter to
   * `AUDIO_PROFILE_MUSIC_HIGH_QUALITY(4)` or `AUDIO_PROFILE_MUSIC_HIGH_QUALITY_STEREO(5)` before
   * setting this enumerator.
   */
  ROOM_ACOUSTICS_KTV = 0x02010100,
  /** An audio effect typical of a concert hall.
   *
   * @note To achieve better audio effect quality, Agora recommends calling \ref
   * IRtcEngine::setAudioProfile "setAudioProfile" and setting the `profile` parameter to
   * `AUDIO_PROFILE_MUSIC_HIGH_QUALITY(4)` or `AUDIO_PROFILE_MUSIC_HIGH_QUALITY_STEREO(5)` before
   * setting this enumerator.
   */
  ROOM_ACOUSTICS_VOCAL_CONCERT = 0x02010200,
  /** An audio effect typical of a recording studio.
   *
   * @note To achieve better audio effect quality, Agora recommends calling \ref
   * IRtcEngine::setAudioProfile "setAudioProfile" and setting the `profile` parameter to
   * `AUDIO_PROFILE_MUSIC_HIGH_QUALITY(4)` or `AUDIO_PROFILE_MUSIC_HIGH_QUALITY_STEREO(5)` before
   * setting this enumerator.
   */
  ROOM_ACOUSTICS_STUDIO = 0x02010300,
  /** An audio effect typical of a vintage phonograph.
   *
   * @note To achieve better audio effect quality, Agora recommends calling \ref
   * IRtcEngine::setAudioProfile "setAudioProfile" and setting the `profile` parameter to
   * `AUDIO_PROFILE_MUSIC_HIGH_QUALITY(4)` or `AUDIO_PROFILE_MUSIC_HIGH_QUALITY_STEREO(5)` before
   * setting this enumerator.
   */
  ROOM_ACOUSTICS_PHONOGRAPH = 0x02010400,
  /** A virtual stereo effect that renders monophonic audio as stereo audio.
   *
   * @note Call \ref IRtcEngine::setAudioProfile "setAudioProfile" and set the `profile` parameter
   * to `AUDIO_PROFILE_MUSIC_STANDARD_STEREO(3)` or `AUDIO_PROFILE_MUSIC_HIGH_QUALITY_STEREO(5)`
   * before setting this enumerator; otherwise, the enumerator setting does not take effect.
   */
  ROOM_ACOUSTICS_VIRTUAL_STEREO = 0x02010500,
  /** A more spatial audio effect.
   *
   * @note To achieve better audio effect quality, Agora recommends calling \ref
   * IRtcEngine::setAudioProfile "setAudioProfile" and setting the `profile` parameter to
   * `AUDIO_PROFILE_MUSIC_HIGH_QUALITY(4)` or `AUDIO_PROFILE_MUSIC_HIGH_QUALITY_STEREO(5)` before
   * setting this enumerator.
   */
  ROOM_ACOUSTICS_SPACIAL = 0x02010600,
  /** A more ethereal audio effect.
   *
   * @note To achieve better audio effect quality, Agora recommends calling \ref
   * IRtcEngine::setAudioProfile "setAudioProfile" and setting the `profile` parameter to
   * `AUDIO_PROFILE_MUSIC_HIGH_QUALITY(4)` or `AUDIO_PROFILE_MUSIC_HIGH_QUALITY_STEREO(5)` before
   * setting this enumerator.
   */
  ROOM_ACOUSTICS_ETHEREAL = 0x02010700,
  /** A 3D voice effect that makes the voice appear to be moving around the user. The default cycle
   * period of the 3D voice effect is 10 seconds. To change the cycle period, call \ref
   * IRtcEngine::setAudioEffectParameters "setAudioEffectParameters" after this method.
   *
   * @note
   * - Call \ref IRtcEngine::setAudioProfile "setAudioProfile" and set the `profile` parameter to
   * `AUDIO_PROFILE_MUSIC_STANDARD_STEREO(3)` or `AUDIO_PROFILE_MUSIC_HIGH_QUALITY_STEREO(5)` before
   * setting this enumerator; otherwise, the enumerator setting does not take effect.
   * - If the 3D voice effect is enabled, users need to use stereo audio playback devices to hear
   * the anticipated voice effect.
   */
  ROOM_ACOUSTICS_3D_VOICE = 0x02010800,
  /** virtual suround sound.
   *
   * @note
   * - Agora recommends using this enumerator to process virtual suround sound; otherwise, you may
   * not hear the anticipated voice effect.
   * - To achieve better audio effect quality, Agora recommends calling \ref
   * IRtcEngine::setAudioProfile "setAudioProfile" and setting the `profile` parameter to
   * `AUDIO_PROFILE_MUSIC_HIGH_QUALITY(4)` or `AUDIO_PROFILE_MUSIC_HIGH_QUALITY_STEREO(5)` before
   * setting this enumerator.
   */
  ROOM_ACOUSTICS_VIRTUAL_SURROUND_SOUND = 0x02010900,
  /** The voice of an uncle.
   *
   * @note
   * - Agora recommends using this enumerator to process a male-sounding voice; otherwise, you may
   * not hear the anticipated voice effect.
   * - To achieve better audio effect quality, Agora recommends calling \ref
   * IRtcEngine::setAudioProfile "setAudioProfile" and setting the `profile` parameter to
   * `AUDIO_PROFILE_MUSIC_HIGH_QUALITY(4)` or `AUDIO_PROFILE_MUSIC_HIGH_QUALITY_STEREO(5)` before
   * setting this enumerator.
   */
  VOICE_CHANGER_EFFECT_UNCLE = 0x02020100,
  /** The voice of an old man.
   *
   * @note
   * - Agora recommends using this enumerator to process a male-sounding voice; otherwise, you may
   * not hear the anticipated voice effect.
   * - To achieve better audio effect quality, Agora recommends calling \ref
   * IRtcEngine::setAudioProfile "setAudioProfile" and setting the `profile` parameter to
   * `AUDIO_PROFILE_MUSIC_HIGH_QUALITY(4)` or `AUDIO_PROFILE_MUSIC_HIGH_QUALITY_STEREO(5)` before
   * setting this enumerator.
   */
  VOICE_CHANGER_EFFECT_OLDMAN = 0x02020200,
  /** The voice of a boy.
   *
   * @note
   * - Agora recommends using this enumerator to process a male-sounding voice; otherwise, you may
   * not hear the anticipated voice effect.
   * - To achieve better audio effect quality, Agora recommends calling \ref
   * IRtcEngine::setAudioProfile "setAudioProfile" and setting the `profile` parameter to
   * `AUDIO_PROFILE_MUSIC_HIGH_QUALITY(4)` or `AUDIO_PROFILE_MUSIC_HIGH_QUALITY_STEREO(5)` before
   * setting this enumerator.
   */
  VOICE_CHANGER_EFFECT_BOY = 0x02020300,
  /** The voice of a young woman.
   *
   * @note
   * - Agora recommends using this enumerator to process a female-sounding voice; otherwise, you may
   * not hear the anticipated voice effect.
   * - To achieve better audio effect quality, Agora recommends calling \ref
   * IRtcEngine::setAudioProfile "setAudioProfile" and setting the `profile` parameter to
   * `AUDIO_PROFILE_MUSIC_HIGH_QUALITY(4)` or `AUDIO_PROFILE_MUSIC_HIGH_QUALITY_STEREO(5)` before
   * setting this enumerator.
   */
  VOICE_CHANGER_EFFECT_SISTER = 0x02020400,
  /** The voice of a girl.
   *
   * @note
   * - Agora recommends using this enumerator to process a female-sounding voice; otherwise, you may
   * not hear the anticipated voice effect.
   * - To achieve better audio effect quality, Agora recommends calling \ref
   * IRtcEngine::setAudioProfile "setAudioProfile" and setting the `profile` parameter to
   * `AUDIO_PROFILE_MUSIC_HIGH_QUALITY(4)` or `AUDIO_PROFILE_MUSIC_HIGH_QUALITY_STEREO(5)` before
   * setting this enumerator.
   */
  VOICE_CHANGER_EFFECT_GIRL = 0x02020500,
  /** The voice of Pig King, a character in Journey to the West who has a voice like a growling
   * bear.
   *
   * @note To achieve better audio effect quality, Agora recommends calling \ref
   * IRtcEngine::setAudioProfile "setAudioProfile" and setting the `profile` parameter to
   * `AUDIO_PROFILE_MUSIC_HIGH_QUALITY(4)` or `AUDIO_PROFILE_MUSIC_HIGH_QUALITY_STEREO(5)` before
   * setting this enumerator.
   */
  VOICE_CHANGER_EFFECT_PIGKING = 0x02020600,
  /** The voice of Hulk.
   *
   * @note To achieve better audio effect quality, Agora recommends calling \ref
   * IRtcEngine::setAudioProfile "setAudioProfile" and setting the `profile` parameter to
   * `AUDIO_PROFILE_MUSIC_HIGH_QUALITY(4)` or `AUDIO_PROFILE_MUSIC_HIGH_QUALITY_STEREO(5)` before
   * setting this enumerator.
   */
  VOICE_CHANGER_EFFECT_HULK = 0x02020700,
  /** An audio effect typical of R&B music.
   *
   * @note To achieve better audio effect quality, Agora recommends calling \ref
   * IRtcEngine::setAudioProfile "setAudioProfile" and setting the `profile` parameter to
   * `AUDIO_PROFILE_MUSIC_HIGH_QUALITY(4)` or `AUDIO_PROFILE_MUSIC_HIGH_QUALITY_STEREO(5)` before
   * setting this enumerator.
   */
  STYLE_TRANSFORMATION_RNB = 0x02030100,
  /** An audio effect typical of popular music.
   *
   * @note To achieve better audio effect quality, Agora recommends calling \ref
   * IRtcEngine::setAudioProfile "setAudioProfile" and setting the `profile` parameter to
   * `AUDIO_PROFILE_MUSIC_HIGH_QUALITY(4)` or `AUDIO_PROFILE_MUSIC_HIGH_QUALITY_STEREO(5)` before
   * setting this enumerator.
   */
  STYLE_TRANSFORMATION_POPULAR = 0x02030200,
  /** A pitch correction effect that corrects the user's pitch based on the pitch of the natural C
   * major scale. To change the basic mode and tonic pitch, call \ref
   * IRtcEngine::setAudioEffectParameters "setAudioEffectParameters" after this method.
   *
   * @note To achieve better audio effect quality, Agora recommends calling \ref
   * IRtcEngine::setAudioProfile "setAudioProfile" and setting the `profile` parameter to
   * `AUDIO_PROFILE_MUSIC_HIGH_QUALITY(4)` or `AUDIO_PROFILE_MUSIC_HIGH_QUALITY_STEREO(5)` before
   * setting this enumerator.
   */
  PITCH_CORRECTION = 0x02040100,

  /** Todo:  Electronic sound, Magic tone haven't been implemented.
   *
   */
};

/** The options for SDK preset voice conversion.
 */
enum VOICE_CONVERSION_PRESET {
  /** Turn off voice conversion and use the original voice.
   */
  VOICE_CONVERSION_OFF = 0x00000000,
  /** A neutral voice.
   */
  VOICE_CHANGER_NEUTRAL = 0x03010100,
  /** A sweet voice.
   */
  VOICE_CHANGER_SWEET = 0x03010200,
  /** A solid voice.
   */
  VOICE_CHANGER_SOLID = 0x03010300,
  /** A bass voice.
   */
  VOICE_CHANGER_BASS = 0x03010400
};

/**
 * The screen sharing encoding parameters.
 */
struct ScreenCaptureParameters {
  /**
   * The dimensions of the shared region in terms of width &times; height. The default value is 0, which means
   * the original dimensions of the shared screen.
   */
  VideoDimensions dimensions;
  /**
   * The frame rate (fps) of the shared region. The default value is 5. We do not recommend setting
   * this to a value greater than 15.
   */
  int frameRate;
  /**
   * The bitrate (Kbps) of the shared region. The default value is 0 (the SDK
   * works out a bitrate according to the dimensions of the current screen).
   */
  int bitrate;
  /** Sets whether or not to capture the mouse for screen sharing:
  * - true: (Default) Capture the mouse.
  * - false: Do not capture the mouse.
  */
  bool captureMouseCursor;
  /** Whether to bring the window to the front when calling
  * \ref IRtcEngine::startScreenCaptureByWindowId "startScreenCaptureByWindowId" to share the window:
  * - true: Bring the window to the front.
  * - false: (Default) Do not bring the window to the front.
  */
  bool windowFocus;
  /**
   * A list of IDs of windows to be blocked.
   */
  view_t *excludeWindowList;
  /**
   * The number of windows to be blocked.
   */
  int excludeWindowCount;

    /** (macOS only) The width (px) of the border. Defaults to 0, and the value range is [0,50].
     *
     */
    int highLightWidth;
    /** (macOS only) The color of the border in RGBA format. The default value is 0xFF8CBF26.
     *
     */
    unsigned int highLightColor;
    /** (macOS only) Determines whether to place a border around the shared window or screen:
     * - true: Place a border.
     * - false: (Default) Do not place a border.
     *
     * @note When you share a part of a window or screen, the SDK places a border around the entire window or screen if you set `enableHighLight` as true.
     *
     */
    bool enableHighLight;

  ScreenCaptureParameters()
    : dimensions(1920, 1080), frameRate(5), bitrate(STANDARD_BITRATE), captureMouseCursor(true), windowFocus(false), excludeWindowList(OPTIONAL_NULLPTR), excludeWindowCount(0), highLightWidth(0), highLightColor(0), enableHighLight(false)  {}
  ScreenCaptureParameters(const VideoDimensions& d, int f, int b)
    : dimensions(d), frameRate(f), bitrate(b), captureMouseCursor(true), windowFocus(false), excludeWindowList(OPTIONAL_NULLPTR), excludeWindowCount(0), highLightWidth(0), highLightColor(0), enableHighLight(false) {}
  ScreenCaptureParameters(int width, int height, int f, int b)
    : dimensions(width, height), frameRate(f), bitrate(b), captureMouseCursor(true), windowFocus(false), excludeWindowList(OPTIONAL_NULLPTR), excludeWindowCount(0), highLightWidth(0), highLightColor(0), enableHighLight(false){}
  ScreenCaptureParameters(int width, int height, int f, int b, bool cur, bool fcs)
    : dimensions(width, height), frameRate(f), bitrate(b), captureMouseCursor(cur), windowFocus(fcs), excludeWindowList(OPTIONAL_NULLPTR), excludeWindowCount(0), highLightWidth(0), highLightColor(0), enableHighLight(false) {}
  ScreenCaptureParameters(int width, int height, int f, int b, view_t *ex, int cnt)
    : dimensions(width, height), frameRate(f), bitrate(b), captureMouseCursor(true), windowFocus(false), excludeWindowList(ex), excludeWindowCount(cnt), highLightWidth(0), highLightColor(0), enableHighLight(false) {}
  ScreenCaptureParameters(int width, int height, int f, int b, bool cur, bool fcs, view_t *ex, int cnt)
    : dimensions(width, height), frameRate(f), bitrate(b), captureMouseCursor(cur), windowFocus(fcs), excludeWindowList(ex), excludeWindowCount(cnt), highLightWidth(0), highLightColor(0), enableHighLight(false) {}
};

/**
 * The audio recording quality type.
 */
enum AUDIO_RECORDING_QUALITY_TYPE {
  /**
   * 0: Low audio recording quality.
   */
  AUDIO_RECORDING_QUALITY_LOW = 0,
  /**
   * 1: Medium audio recording quality.
   */
  AUDIO_RECORDING_QUALITY_MEDIUM = 1,
  /**
   * 2: High audio recording quality.
   */
  AUDIO_RECORDING_QUALITY_HIGH = 2,
  /**
   * 3: Ultra high audio recording quality.
   */
  AUDIO_RECORDING_QUALITY_ULTRA_HIGH = 3,
};

/**
 * The audio file record type.
 */
enum AUDIO_FILE_RECORDING_TYPE {
  /**
   * 1: mic audio file recording.
   */
  AUDIO_FILE_RECORDING_MIC = 1,
  /**
   * 2: playback audio file recording.
   */
  AUDIO_FILE_RECORDING_PLAYBACK = 2,
  /**
   * 3: mixed audio file recording, include mic and playback.
   */
  AUDIO_FILE_RECORDING_MIXED = 3,
};

/**
 * audio encoded frame observer position.
 */
enum AUDIO_ENCODED_FRAME_OBSERVER_POSITION {
  /**
  * 1: mic
  */
  AUDIO_ENCODED_FRAME_OBSERVER_POSITION_RECORD = 1,
  /**
  * 2: playback audio file recording.
  */
  AUDIO_ENCODED_FRAME_OBSERVER_POSITION_PLAYBACK = 2,
  /**
  * 3: mixed audio file recording, include mic and playback.
  */
  AUDIO_ENCODED_FRAME_OBSERVER_POSITION_MIXED = 3,
};

/**
 * The Audio file recording options.
 */
struct AudioRecordingConfiguration {
  /**
   * The path of recording file.
   * The string of the file path is in UTF-8 code.
   */
  const char* filePath;
  /**
   * Determines whether to encode audio data.
   * - true: Encode the audio data with AAC Encoder.
   * - false: (Default) Do not encode the audio data. Save audio data as a wav file.
   */
  bool encode;
  /**
   * The sample rate of audio data. Default is 32000.
   * The optional value is 16000, 32000, 44100, or 48000.
   */
  int sampleRate;
  /**
   * The recording type of audio data.
   */
  AUDIO_FILE_RECORDING_TYPE fileRecordingType;
  /**
   * The recording quality of audio data.
   */
  AUDIO_RECORDING_QUALITY_TYPE quality;

  /**
   * Recording channel. The following values are supported:
   * - (Default) 1
   * - 2
   */
  int recordingChannel;

  AudioRecordingConfiguration()
    : filePath(NULL),
      encode(false),
      sampleRate(32000),
      fileRecordingType(AUDIO_FILE_RECORDING_MIXED),
      quality(AUDIO_RECORDING_QUALITY_LOW),
      recordingChannel(1) {}

  AudioRecordingConfiguration(const char* file_path, int sample_rate, AUDIO_RECORDING_QUALITY_TYPE quality_type, int channel)
    : filePath(file_path),
      encode(false),
      sampleRate(sample_rate),
      fileRecordingType(AUDIO_FILE_RECORDING_MIXED),
      quality(quality_type),
      recordingChannel(channel) {}

  AudioRecordingConfiguration(const char* file_path, bool enc, int sample_rate, AUDIO_FILE_RECORDING_TYPE type, AUDIO_RECORDING_QUALITY_TYPE quality_type, int channel)
    : filePath(file_path),
      encode(enc),
      sampleRate(sample_rate),
      fileRecordingType(type),
      quality(quality_type),
      recordingChannel(channel) {}

  AudioRecordingConfiguration(const AudioRecordingConfiguration &rhs)
    : filePath(rhs.filePath),
      encode(rhs.encode),
      sampleRate(rhs.sampleRate),
      fileRecordingType(rhs.fileRecordingType),
      quality(rhs.quality),
      recordingChannel(rhs.recordingChannel) {}
};

/**
 * The Audio encoded frame receiver options.
 *
 */
struct AudioEncodedFrameObserverConfig {
    /**
     * The position where SDK record the audio, and callback to encoded audio frame receiver.
     */
    AUDIO_ENCODED_FRAME_OBSERVER_POSITION postionType;
    /**
     * The audio encoding type of encoded frame.
     */
    AUDIO_ENCODING_TYPE encodingType;

    AudioEncodedFrameObserverConfig()
    : postionType(AUDIO_ENCODED_FRAME_OBSERVER_POSITION_PLAYBACK),
      encodingType(AUDIO_ENCODING_TYPE_OPUS_48000_MEDIUM){}

};

class IAudioEncodedFrameObserver {
public:
/**
* Occurs each time the SDK receives an encoded recorded audio frame.
* @param frameBuffer The pointer to the audio frame buffer.
* @param length The data length of the audio frame.
* @param audioEncodedFrameInfo The information of the encoded audio frame: EncodedAudioFrameInfo.

*/
virtual void OnRecordAudioEncodedFrame(const uint8_t* frameBuffer,  int length, const EncodedAudioFrameInfo& audioEncodedFrameInfo) = 0;

/**
* Occurs each time the SDK receives an encoded playback audio frame.
* @param frameBuffer The pointer to the audio frame buffer.
* @param length The data length of the audio frame.
* @param audioEncodedFrameInfo The information of the encoded audio frame: EncodedAudioFrameInfo.

*/
virtual void OnPlaybackAudioEncodedFrame(const uint8_t* frameBuffer,  int length, const EncodedAudioFrameInfo& audioEncodedFrameInfo) = 0;

/**
* Occurs each time the SDK receives an encoded mixed audio frame.
* @param frameBuffer The pointer to the audio frame buffer.
* @param length The data length of the audio frame.
* @param audioEncodedFrameInfo The information of the encoded audio frame: EncodedAudioFrameInfo.

*/
virtual void OnMixedAudioEncodedFrame(const uint8_t* frameBuffer,  int length, const EncodedAudioFrameInfo& audioEncodedFrameInfo) = 0;

virtual ~IAudioEncodedFrameObserver () {}
};

/** IP areas.
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
     * (Default) Global.
     */
    AREA_CODE_GLOB = (0xFFFFFFFF)
};

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
     * The global area (except China)
     */
    AREA_CODE_OVS = 0xFFFFFFFE
};

enum CHANNEL_MEDIA_RELAY_ERROR {
  /** 0: The state is normal.
    */
  RELAY_OK = 0,
  /** 1: An error occurs in the server response.
    */
  RELAY_ERROR_SERVER_ERROR_RESPONSE = 1,
  /** 2: No server response. You can call the
    * \ref agora::rtc::IRtcEngine::leaveChannel "leaveChannel" method to
    * leave the channel.
    */
  RELAY_ERROR_SERVER_NO_RESPONSE = 2,
  /** 3: The SDK fails to access the service, probably due to limited
    * resources of the server.
    */
  RELAY_ERROR_NO_RESOURCE_AVAILABLE = 3,
  /** 4: Fails to send the relay request.
    */
  RELAY_ERROR_FAILED_JOIN_SRC = 4,
  /** 5: Fails to accept the relay request.
    */
  RELAY_ERROR_FAILED_JOIN_DEST = 5,
  /** 6: The server fails to receive the media stream.
    */
  RELAY_ERROR_FAILED_PACKET_RECEIVED_FROM_SRC = 6,
  /** 7: The server fails to send the media stream.
    */
  RELAY_ERROR_FAILED_PACKET_SENT_TO_DEST = 7,
  /** 8: The SDK disconnects from the server due to poor network
    * connections. You can call the \ref agora::rtc::IRtcEngine::leaveChannel
    * "leaveChannel" method to leave the channel.
    */
  RELAY_ERROR_SERVER_CONNECTION_LOST = 8,
  /** 9: An internal error occurs in the server.
    */
  RELAY_ERROR_INTERNAL_ERROR = 9,
  /** 10: The token of the source channel has expired.
    */
  RELAY_ERROR_SRC_TOKEN_EXPIRED = 10,
  /** 11: The token of the destination channel has expired.
    */
  RELAY_ERROR_DEST_TOKEN_EXPIRED = 11,
};

//callback event
enum CHANNEL_MEDIA_RELAY_EVENT {
  /** 0: The user disconnects from the server due to poor network
    * connections.
    */
  RELAY_EVENT_NETWORK_DISCONNECTED = 0,
  /** 1: The network reconnects.
    */
  RELAY_EVENT_NETWORK_CONNECTED = 1,
  /** 2: The user joins the source channel.
    */
  RELAY_EVENT_PACKET_JOINED_SRC_CHANNEL = 2,
  /** 3: The user joins the destination channel.
    */
  RELAY_EVENT_PACKET_JOINED_DEST_CHANNEL = 3,
  /** 4: The SDK starts relaying the media stream to the destination channel.
    */
  RELAY_EVENT_PACKET_SENT_TO_DEST_CHANNEL = 4,
  /** 5: The server receives the video stream from the source channel.
    */
  RELAY_EVENT_PACKET_RECEIVED_VIDEO_FROM_SRC = 5,
  /** 6: The server receives the audio stream from the source channel.
    */
  RELAY_EVENT_PACKET_RECEIVED_AUDIO_FROM_SRC = 6,
  /** 7: The destination channel is updated.
    */
  RELAY_EVENT_PACKET_UPDATE_DEST_CHANNEL = 7,
  /** 8: The destination channel update fails due to internal reasons.
    */
  RELAY_EVENT_PACKET_UPDATE_DEST_CHANNEL_REFUSED = 8,
  /** 9: The destination channel does not change, which means that the
    * destination channel fails to be updated.
    */
  RELAY_EVENT_PACKET_UPDATE_DEST_CHANNEL_NOT_CHANGE = 9,
  /** 10: The destination channel name is NULL.
    */
  RELAY_EVENT_PACKET_UPDATE_DEST_CHANNEL_IS_NULL = 10,
  /** 11: The video profile is sent to the server.
    */
  RELAY_EVENT_VIDEO_PROFILE_UPDATE = 11,
    /** 12: pause send packet to dest channel success.
   */
  RELAY_EVENT_PAUSE_SEND_PACKET_TO_DEST_CHANNEL_SUCCESS = 12,
  /** 13: pause send packet to dest channel failed.
   */
  RELAY_EVENT_PAUSE_SEND_PACKET_TO_DEST_CHANNEL_FAILED = 13,
  /** 14: resume send packet to dest channel success.
   */
  RELAY_EVENT_RESUME_SEND_PACKET_TO_DEST_CHANNEL_SUCCESS = 14,
  /** 15: pause send packet to dest channel failed.
   */
  RELAY_EVENT_RESUME_SEND_PACKET_TO_DEST_CHANNEL_FAILED = 15,
};

enum CHANNEL_MEDIA_RELAY_STATE {
  /** 0: The SDK is initializing.
    */
  RELAY_STATE_IDLE = 0,
  /** 1: The SDK tries to relay the media stream to the destination channel.
    */
  RELAY_STATE_CONNECTING = 1,
  /** 2: The SDK successfully relays the media stream to the destination
    * channel.
    */
  RELAY_STATE_RUNNING = 2,
  /** 3: A failure occurs. See the details in code.
    */
  RELAY_STATE_FAILURE = 3,
};

/** The definition of ChannelMediaInfo.
 */
struct ChannelMediaInfo {
    /** The channel name. The default value is NULL, which means that the SDK
     * applies the current channel name.
     */
  const char* channelName;
    /** The token that enables the user to join the channel. The default value
     * is NULL, which means that the SDK applies the current token.
     */
  const char* token;
    /** The user ID.
     */
  uid_t uid;
};

/** The definition of ChannelMediaRelayConfiguration.
 */
struct ChannelMediaRelayConfiguration {
  /** Pointer to the source channel: ChannelMediaInfo.
    *
    * @note
    * - `uid`: ID of the user whose media stream you want to relay. We
    * recommend setting it as 0, which means that the SDK relays the media
    * stream of the current broadcaster.
    * - If you do not use a token, we recommend using the default values of
    * the parameters in ChannelMediaInfo.
    * - If you use a token, set uid as 0, and ensure that the token is
    * generated with the uid set as 0.
    */
  ChannelMediaInfo *srcInfo;
  /** Pointer to the destination channel: ChannelMediaInfo. If you want to
    * relay the media stream to multiple channels, define as many
    * ChannelMediaInfo structs (at most four).
    *
    * @note `uid`: ID of the user who is in the source channel.
    */
  ChannelMediaInfo *destInfos;
  /** The number of destination channels. The default value is 0, and the
    * value range is [0,4). Ensure that the value of this parameter
    * corresponds to the number of ChannelMediaInfo structs you define in
    * `destInfos`.
    */
  int destCount;

  ChannelMediaRelayConfiguration()
    : srcInfo(NULL),
      destInfos(NULL),
      destCount(0)
  {}
};

/**
 * The collections of uplink network info.
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

/**
 * The collections of downlink network info.
 */
struct DownlinkNetworkInfo {
  struct PeerDownlinkInfo {
    /**
     * The ID of the user who owns the remote video stream.
     */
    const char* uid;
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
        : uid(OPTIONAL_NULLPTR),
          stream_type(VIDEO_STREAM_HIGH),
          current_downscale_level(REMOTE_VIDEO_DOWNSCALE_LEVEL_NONE),
          expected_bitrate_bps(-1) {}

    PeerDownlinkInfo& operator=(const PeerDownlinkInfo& rhs) {
      if (this == &rhs) return *this;
      uid = OPTIONAL_NULLPTR;
      stream_type = rhs.stream_type;
      current_downscale_level = rhs.current_downscale_level;
      expected_bitrate_bps = rhs.expected_bitrate_bps;
      if (rhs.uid != OPTIONAL_NULLPTR) {
        char* temp = new char[strlen(rhs.uid) + 1];
        strcpy(temp, rhs.uid);
        uid = temp;
      }
      return *this;
    }

    ~PeerDownlinkInfo() {
      if (uid) { delete [] uid; }
    }
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

  ~DownlinkNetworkInfo() {
    if (peer_downlink_info) delete [] peer_downlink_info;
  }
};

/** Encryption mode.
*/
enum ENCRYPTION_MODE {
  /** 1: 128-bit AES encryption, XTS mode.
   */
  AES_128_XTS = 1,
  /** 2: 128-bit AES encryption, ECB mode.
   */
  AES_128_ECB = 2,
  /** 3: 256-bit AES encryption, XTS mode.
   */
  AES_256_XTS = 3,
  /** 4: 128-bit SM4 encryption, ECB mode.
   */
  SM4_128_ECB = 4,
  /** 5: 128-bit AES encryption, GCM mode.
   */
  AES_128_GCM = 5,
  /** 6: 256-bit AES encryption, GCM mode.
   */
  AES_256_GCM = 6,
  /** 7: (Default) 128-bit AES encryption, GCM mode, with KDF salt.
   */
  AES_128_GCM2 = 7,
  /** 8: 256-bit AES encryption, GCM mode, with KDF salt.
   */
  AES_256_GCM2 = 8,
  /** Enumerator boundary.
   */
  MODE_END,
};

/** Configurations of the built-in encryption schemas. */
struct EncryptionConfig {
  /**
   * The encryption mode. The default encryption mode is `AES_128_GCM2`. See #ENCRYPTION_MODE.
   */
  ENCRYPTION_MODE encryptionMode;
  /**
   * The encryption key in the string format.
   *
   * @note If you do not set an encryption key or set it as NULL, you cannot use the built-in encryption, and the SDK returns #ERR_INVALID_ARGUMENT (-2).
   */
  const char* encryptionKey;
  uint8_t encryptionKdfSalt[32];

  EncryptionConfig()
    : encryptionMode(AES_128_GCM2),
      encryptionKey(NULL)
  {
    memset(encryptionKdfSalt, 0, sizeof(encryptionKdfSalt));
  }

  /// @cond
  const char* getEncryptionString() const {
    switch(encryptionMode) {
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

/** Encryption error type.
 */
enum ENCRYPTION_ERROR_TYPE {
    ENCRYPTION_ERROR_INTERNAL_FAILURE = 0,
    ENCRYPTION_ERROR_DECRYPTION_FAILURE = 1,
    ENCRYPTION_ERROR_ENCRYPTION_FAILURE = 2,
};

enum UPLOAD_ERROR_REASON
{
  UPLOAD_SUCCESS = 0,
  UPLOAD_NET_ERROR = 1,
  UPLOAD_SERVER_ERROR = 2,
};

/** Type of permission.
 */
enum PERMISSION_TYPE {
  RECORD_AUDIO = 0,
  CAMERA = 1,
  SCREEN_CAPTURE = 2,
};

/** Maximum length of user account.
 */
enum MAX_USER_ACCOUNT_LENGTH_TYPE
{
  /** The maximum length of user account is 255 bytes.
   */
  MAX_USER_ACCOUNT_LENGTH = 256
};

/**
 * The stream subscribe state.
 */
enum STREAM_SUBSCRIBE_STATE {
  SUB_STATE_IDLE = 0,
  SUB_STATE_NO_SUBSCRIBED = 1,
  SUB_STATE_SUBSCRIBING = 2,
  SUB_STATE_SUBSCRIBED = 3
};

/**
 * The stream publish state.
 */
enum STREAM_PUBLISH_STATE {
  PUB_STATE_IDLE = 0,
  PUB_STATE_NO_PUBLISHED = 1,
  PUB_STATE_PUBLISHING = 2,
  PUB_STATE_PUBLISHED = 3
};

/**
 * The EchoTestConfiguration struct.
 */
struct EchoTestConfiguration {
  view_t view;
  bool enableAudio;
  bool enableVideo;
  const char* token;
  const char* channelId;

  EchoTestConfiguration(view_t v, bool ea, bool ev, const char* t, const char* c)
   : view(v), enableAudio(ea), enableVideo(ev), token(t), channelId(c) {}

  EchoTestConfiguration()
   : view(OPTIONAL_NULLPTR), enableAudio(true), enableVideo(true), token(OPTIONAL_NULLPTR), channelId(OPTIONAL_NULLPTR) {}
};

/**
 * The UserInfo struct.
 */
struct UserInfo {
  /**
   * The user ID.
   */
  uid_t uid;
  /**
   * The user account.
   */
  char userAccount[MAX_USER_ACCOUNT_LENGTH];
  UserInfo()
      : uid(0) {
    userAccount[0] = '\0';
  }
};

/**
 * Type of ear monitoring filter.
 */
enum EAR_MONITORING_FILTER_TYPE {
  /**
   * 1: Do not add an audio filter to the in-ear monitor.
   */
  EAR_MONITORING_FILTER_NONE = (1<<0),
  /**
   * 2: Enable audio filters to the in-ear monitor.
   */
  EAR_MONITORING_FILTER_BUILT_IN_AUDIO_FILTERS = (1<<1),
  /**
   * 4: Enable noise suppression to the in-ear monitor.
   */
  EAR_MONITORING_FILTER_NOISE_SUPPRESSION = (1<<2)
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

#if defined(__ANDROID__) || (defined(__APPLE__) && TARGET_OS_IOS)

/**
 * The video configuration for the shared screen stream.
 */
struct ScreenVideoParameters {
  /**
   * The dimensions of the video encoding resolution. The default value is `1280` x `720`.
   * For recommended values, see [Recommended video
   * profiles](https://docs.agora.io/en/Interactive%20Broadcast/game_streaming_video_profile?platform=Android#recommended-video-profiles).
   * If the aspect ratio is different between width and height and the screen, the SDK adjusts the
   * video encoding resolution according to the following rules (using an example where `width` ×
   * `height` is 1280 × 720):
   * - When the width and height of the screen are both lower than `width` and `height`, the SDK
   * uses the resolution of the screen for video encoding. For example, if the screen is 640 ×
   * 360, The SDK uses 640 × 360 for video encoding.
   * - When either the width or height of the screen is higher than `width` or `height`, the SDK
   * uses the maximum values that do not exceed those of `width` and `height` while maintaining
   * the aspect ratio of the screen for video encoding. For example, if the screen is 2000 × 1500,
   * the SDK uses 960 × 720 for video encoding.
   *
   * @note
   * - The billing of the screen sharing stream is based on the values of width and height.
   * When you do not pass in these values, Agora bills you at 1280 × 720;
   * when you pass in these values, Agora bills you at those values.
   * For details, see [Pricing for Real-time
   * Communication](https://docs.agora.io/en/Interactive%20Broadcast/billing_rtc).
   * - This value does not indicate the orientation mode of the output ratio.
   * For how to set the video orientation, see `ORIENTATION_MODE`.
   * - Whether the SDK can support a resolution at 720P depends on the performance of the device.
   * If you set 720P but the device cannot support it, the video frame rate can be lower.
   */
  VideoDimensions dimensions;
  /**
   * The video encoding frame rate (fps). The default value is `15`.
   * For recommended values, see [Recommended video
   * profiles](https://docs.agora.io/en/Interactive%20Broadcast/game_streaming_video_profile?platform=Android#recommended-video-profiles).
   */
  int frameRate = 15;
   /**
   * The video encoding bitrate (Kbps). For recommended values, see [Recommended video
   * profiles](https://docs.agora.io/en/Interactive%20Broadcast/game_streaming_video_profile?platform=Android#recommended-video-profiles).
   */
  int bitrate;
  /*
   * The content hint of the screen sharing:
   */
  VIDEO_CONTENT_HINT contentHint = VIDEO_CONTENT_HINT::CONTENT_HINT_MOTION;

  ScreenVideoParameters() : dimensions(1280, 720) {}
};

/**
 * The audio configuration for the shared screen stream.
 */
struct ScreenAudioParameters {
#if defined(__ANDROID__)
  /**
   * The audio sample rate (Hz). The default value is `16000`.
   */
  int sampleRate = 16000;
  /**
   * The number of audio channels. The default value is `2`, indicating dual channels.
   */
  int channels = 2;
#endif
  /**
   * The volume of the captured system audio. The value range is [0,100]. The default value is
   * `100`.
   */
  int captureSignalVolume = 100;
};

/**
 * The configuration of the screen sharing
 */
struct ScreenCaptureParameters2 {
  /**
   * Determines whether to capture system audio during screen sharing:
   * - `true`: Capture.
   * - `false`: (Default)  Do not capture.
   *
   * **Note**
   * Due to system limitations, capturing system audio is only available for Android API level 29
   * and later (that is, Android 10 and later).
   */
  bool captureAudio = false;
  /**
   * The audio configuration for the shared screen stream.
   */
  ScreenAudioParameters audioParams;
  /**
   * Determines whether to capture the screen during screen sharing:
   * - `true`: (Default) Capture.
   * - `false`: Do not capture.
   *
   * **Note**
   * Due to system limitations, screen capture is only available for Android API level 21 and later
   * (that is, Android 5 and later).
   */
  bool captureVideo = true;
  /**
   * The video configuration for the shared screen stream.
   */
  ScreenVideoParameters videoParams;
};
#endif

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
    IAgoraParameter* p = NULL;
    if (engine && !engine->queryInterface(rtc::AGORA_IID_PARAMETER_ENGINE, (void**)&p)) reset(p);
    return p != NULL;
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
 * Spatial audio parameters
 */
struct SpatialAudioParams {
  /**
   * optional azimuth: speaker azimuth in a spherical coordinate system centered on the listener
   */
  Optional<double> speaker_azimuth;
  /**
   * optional azimuth: speaker elevation in a spherical coordinate system centered on the listener
   */
  Optional<double> speaker_elevation;
  /**
   * distance between speaker and listener
   */
  Optional<double> speaker_distance;
  /**
   * speaker orientation [0-180]: 0 degree is the same with listener orientation
   */
  Optional<int> speaker_orientation;
  /**
   * enable blur or not for the speaker
   */
  Optional<bool> enable_blur;
  /**
   * enable air absorb or not for the speaker
   */
  Optional<bool> enable_air_absorb;
  /**
   * speaker attenuation factor
   */
  Optional<double> speaker_attenuation;
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
AGORA_API int AGORA_CALL createAgoraCredential(agora::util::AString &credential);

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
AGORA_API int AGORA_CALL getAgoraCertificateVerifyResult(const char *credential_buf, int credential_len,
    const char *certificate_buf, int certificate_len);

/**
 * @brief Implement the agora::base::LicenseCallback,
 * create a LicenseCallback object to receive callbacks of license.
 *
 * @param [in] callback The object of agora::LiceseCallback,
 *                      set the callback to null before delete it.
 */
AGORA_API void setAgoraLicenseCallback(agora::base::LicenseCallback *callback);

/**
 * @brief Get the LicenseCallback pointer if already setup,
 *  otherwise, return null.
 *
 * @return a pointer of agora::base::LicenseCallback
 */

AGORA_API agora::base::LicenseCallback* getAgoraLicenseCallback();
