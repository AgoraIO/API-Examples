//
//  Agora SDK
//
//  Copyright (c) 2018 Agora.io. All rights reserved.
//

#pragma once  // NOLINT(build/header_guard)

#include <cstring>

#include "AgoraBase.h"
#include "AgoraRefPtr.h"

namespace agora {
namespace media {
namespace base {
class IAudioFrameObserver;
} // namespace base
} // namespace media

namespace rtc {

static const int kAdmMaxDeviceNameSize = 128;
static const int kAdmMaxGuidSize = 128;
static const int kIntervalInMillseconds = 200;


#if defined(_WIN32) || (defined(__APPLE__) && TARGET_OS_MAC && !TARGET_OS_IPHONE) 
/**
 * The struct of AudioDeviceInfo.
 *
 * @note
 * This struct applies to Windows and macOS only.
 */
struct AudioDeviceInfo {
  /**
   * The name of the device. The maximum name size is 128 bytes. The default value is 0.
   */
  char deviceName[kAdmMaxDeviceNameSize];
  /**
   * The ID of the device. The maximum size is 128 bytes. The default value is 0.
   */
  char deviceId[kAdmMaxGuidSize];
  /**
   * Determines whether the current device is selected for audio capturing or playback.
   * - true: Select the current device for audio capturing or playback.
   * - false: (Default) Do not select the current device for audio capturing or playback.
   */
  bool isCurrentSelected;
  /**
   * Determines whether the current device is the audio playout device.
   * - true: (Default) The current device is the playout device.
   * - false: The current device is not the playout device.
   */
  bool isPlayoutDevice;

  AudioDeviceInfo() : isCurrentSelected(false),
                      isPlayoutDevice(true) {
    memset(deviceName, 0, sizeof(deviceName));
    memset(deviceId, 0, sizeof(deviceId));
  }
};
#endif  // _WIN32 || (TARGET_OS_MAC && !TARGET_OS_IPHONE)

/**
 * The IAudioDeviceManagerObserver class.
 */
class IAudioDeviceManagerObserver
{
public:
  virtual ~IAudioDeviceManagerObserver() {}

  /**
   * Occurs when the device state changes, for example, when a device is added or removed.
   *
   * To get the current information of the connected audio devices, call \ref agora::rtc::INGAudioDeviceManager::getNumberOfPlayoutDevices "getNumberOfPlayoutDevices".
   */
  virtual void onDeviceStateChanged() = 0;

  /**
   * Occurs when the device state changes, for example, when a device is added or removed or default device change.
   * 
   * @note
   * This method applies to Windows only now.
   *
   * @param deviceId Pointer to the device ID.
   * @param deviceType Device type: #MEDIA_DEVICE_TYPE.
   * @param deviceState Device state: #MEDIA_DEVICE_STATE_TYPE..
   */
  virtual void onAudioDeviceStateChanged(const char *deviceId, int deviceType, int deviceState) = 0;

  /** Indicates incoming volume. This can be used to test microphone.
   *
   * @param volume volume between 0 (lowest volume) to 255 (highest volume).
   */
  virtual void onVolumeIndication(int volume) = 0;

  /**
   * Occurs when the audio route changes.
   *
   * @param route The current audio route. See #AudioRoute.
   */
  virtual void onRoutingChanged(AudioRoute route) = 0;

  /**
   * Occurs when the audio device volume changes.
   *
   * @param deviceType The device type, see #MEDIA_DEVICE_TYPE.
   * @param volume The volume of the audio device.
   * @param muted Whether the audio device is muted:
   * - true: The audio device is muted.
   * - false: The audio device is not muted.
   */
  virtual void onAudioDeviceVolumeChanged(int deviceType, int volume, bool muted) = 0;
};

class IRecordingDeviceSource : public RefCountInterface {
  public:
  /**
   * Initialize the recording device source.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
    virtual int initRecording(const char* deviceName = NULL) = 0;

  /**
   * Start the recording device.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
    virtual int startRecording() = 0;

  /**
   * Stop the recording device.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
    virtual int stopRecording() = 0;

  /**
   * Registers an audio frame observer.
   *
   * @param observer The pointer to the IAudioFrameObserver object.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
    virtual int registerAudioFrameObserver(media::base::IAudioFrameObserver* observer) = 0;

  /**
   * Releases the registered IAudioFrameObserver object.
   *
   * @param observer The pointer to the IAudioFrameObserver object created by the `registerAudioPcmDataCallback` method.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
    virtual int unregisterAudioFrameObserver(media::base::IAudioFrameObserver* observer) = 0;

    virtual ~IRecordingDeviceSource() {}
};

/**
 * The INGAudioDeciceManager class.
 *
 * This class provides access to audio volume and audio route control, as well as device enumeration and
 * selection on the PC.
 */
class INGAudioDeviceManager : public RefCountInterface {
public:
  /**
   * Creates a audio device source object and returns the pointer.
   *
   * @return
   * - The pointer to \ref rtc::IRecordingDeviceSource "IRecordingDeviceSource", if the method call
   * succeeds.
   * - An empty pointer NULL: Failure.
   */
  virtual agora_refptr<IRecordingDeviceSource> createRecordingDeviceSource(char deviceId[kAdmMaxDeviceNameSize]) = 0;

  /**
   * Sets the volume of the microphone.
   * @param volume The volume of the microphone. The value range is [0, 255].
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setMicrophoneVolume(unsigned int volume) = 0;
  /**
   * Gets the volume of the microphone.
   * @param volume The volume of the microphone.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int getMicrophoneVolume(unsigned int& volume) = 0;
  /**
   * Sets the volume of the speaker.
   * @param volume The volume of the speaker. The value range is [2, 255].
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setSpeakerVolume(unsigned int volume) = 0;
  /**
   * Gets the volume of the speaker.
   * @param volume The volume of the speaker.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int getSpeakerVolume(unsigned int& volume) = 0;
  /**
   * Captures or stops capturing the local audio with the microphone.
   * @param mute Determines whether to capture or stop capturing the local audio with the microphone.
   * - true: Stop capturing the local audio.
   * - false: (Default) Capture the local audio.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setMicrophoneMute(bool mute) = 0;
  /**
   * Gets the mute state of the microphone.
   * @param mute The mute state of the microphone.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int getMicrophoneMute(bool& mute) = 0;
  /**
   * Plays or stops playing the remote audio with the speaker.
   * @param mute Determines whether to play or stop playing the remote audio.
   * - true: Stop playing the remote audio.
   * - false: (Default) Play the remote audio.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setSpeakerMute(bool mute) = 0;
  /**
   * Gets the mute state of the speaker.
   * @param mute A reference to the mute state of the speaker.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int getSpeakerMute(bool& mute) = 0;

  /**
   * Get the playout parameters of audio device.
   * @param params A point to the struct AudioParameters.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int getPlayoutAudioParameters(AudioParameters* params) const = 0;

  /**
   * Get the record parameters of audio device.
   * @param params A point to the struct AudioParameters.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int getRecordAudioParameters(AudioParameters* params) const = 0;

#if defined(__ANDROID__) || (defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE)
  /**
   * Sets the default audio routing.
   *
   * This method allows apps to change the current audio route for the received audio.
   * Noted: In Low Level API, we don't support default audio routing, i.e.,
   * setDefaultAudioRouteToSpeakerphone. This can be done in RTC engine.
   *
   * @note
   * This method applies to Android and iOS only.
   *
   * @param route The default audio route. See #AudioRoute.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setDefaultAudioRouting(AudioRoute route) = 0;
  /**
   * Changes the current audio routing.
   *
   * @note
   * This method applies to Android and iOS only.
   *
   * @param route The audio route that you want to change to. See #AudioRoute.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int changeAudioRouting(AudioRoute route) = 0;
  /**
   * Gets the current audio routing.
   *
   * @note
   * This method applies to Android and iOS only.
   *
   * @param route A reference to the audio route: AudioRoute.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int getCurrentRouting(AudioRoute& route) = 0;
#endif  // __ANDROID__ || TARGET_OS_IPHONE

#if defined(_WIN32) || (defined(__APPLE__) && TARGET_OS_MAC && !TARGET_OS_IPHONE) 
  /**
   * Gets the index numbers of all audio playout devices.
   *
   * @note
   * This method applies to Windows or macOS only.
   *
   * @return
   * - The index numbers of the audio playout devices: Success.
   * - < 0: Failure.
   */
  virtual int getNumberOfPlayoutDevices() = 0;

  /**
   * Gets the index numbers of all audio recording devices.
   *
   * @note
   * This method applies to Windows or macOS only.
   *
   * @return
   * - The index numbers of the audio recording devices: Success.
   * - < 0: Failure.
   */
  virtual int getNumberOfRecordingDevices() = 0;
  /**
   * Gets the information of the current audio playout device.
   *
   * @note
   * This method applies to Windows or macOS only.
   *
   * @param index The index number of the current audio playout device.
   * @return
   * The information of the audio playout device. See \ref agora::rtc::AudioDeviceInfo "AudioDeviceInfo".
   */
  virtual AudioDeviceInfo getPlayoutDeviceInfo(int index) = 0;
  /**
   * Gets the information of the current recording device.
   *
   * @note
   * This method applies to Windows or macOS only.
   *
   * @param index The index number of the current recording device.
   * @return
   * The information of the recording device. See \ref agora::rtc::AudioDeviceInfo "AudioDeviceInfo".
   */
  virtual AudioDeviceInfo getRecordingDeviceInfo(int index) = 0;
  /**
   * Sets the audio playback device.
   *
   * @note
   * This method applies to Windows or macOS only.
   *
   * @param index The index number of the audio playout device.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setPlayoutDevice(int index) = 0;
  /**
   * Sets the recording device.
   *
   * @note
   * This method applies to Windows or macOS only.
   *
   * @param index The index number of the recording device.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setRecordingDevice(int index) = 0;
  /** The status of following system default playback device.

   @note The status of following system default playback device.

   @param enable Variable to whether the current device follow system default playback device or not.
   - true: The current device will change when the system default playback device changed.
   - false: The current device will change only current device is removed.
   @return
   - 0: Success.
   - < 0: Failure.
   */
  virtual int followSystemPlaybackDevice(bool enable) = 0;

  /** The status of following system default recording device.

   @note The status of following system default recording device.

   @param enable Variable to whether the current device follow system default recording device or not.
   - true: The current device will change when the system default recording device changed.
   - false: The current device will change only current device is removed.
   @return
   - 0: Success.
   - < 0: Failure.
   */
  virtual int followSystemRecordingDevice(bool enable) = 0;
#endif  // _WIN32 || (TARGET_OS_MAC && !TARGET_OS_IPHONE)

#if defined(_WIN32)
  /**
   * Sets the volume of the app.
   *
   * @note
   * This method applies to Windows only.
   *
   * @param volume The volume of the app. The value range is [0, 255].
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setApplicationVolume(unsigned int volume) = 0;
  /**
   * Gets the volume of the app.
   *
   * @note
   * This method applies to Windows only.
   *
   * @param volume The volume of the app. The value range is [0, 255].
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int getApplicationVolume(unsigned int& volume) = 0;
  /**
   * Sets the mute state of the app.
   *
   * @note
   * This method applies to Windows only.
   *
   * @param mute Determines whether to set the app to the mute state.
   * - true: Set the app to the mute state.
   * - false: (Default) Do not set the app to the mute state.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setApplicationMuteState(bool mute) = 0;
  /**
   * Gets the mute state of the app.
   *
   * @note
   * This method applies to Windows only.
   *
   * @param mute A reference to the mute state of the app.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int getApplicationMuteState(bool& mute) = 0;
#endif  // _WIN32

  /**
   * Registers an IAudioDeviceManagerObserver object.
   *
   * You need to implement the IAudioDeviceManageObserver class in this method, and register callbacks
   * according to your scenario.
   *
   * @param observer A pointer to the IAudioDeviceManagerObserver class.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int registerObserver(IAudioDeviceManagerObserver* observer, void(*safeDeleter)(IAudioDeviceManagerObserver*) = NULL) = 0;
  /**
   * Releases the IAudioDeviceManagerObserver object.
   * @param observer The pointer to the IAudioDeviceManagerObserver class registered using #registerObserver.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int unregisterObserver(IAudioDeviceManagerObserver* observer) = 0;

protected:
  ~INGAudioDeviceManager() {}
};

} //namespace rtc
} // namespace agora
