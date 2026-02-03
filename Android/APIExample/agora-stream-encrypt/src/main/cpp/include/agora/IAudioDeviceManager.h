//
//  Agora SDK
//
//  Copyright (c) 2021 Agora.io. All rights reserved.
//
#pragma once // NOLINT(build/header_guard)

namespace agora {
namespace rtc {

/**
 * @brief The maximum length of the device ID.
 */
enum MAX_DEVICE_ID_LENGTH_TYPE {
  /**
   * The maximum length of the device ID is 512 bytes.
   */
  MAX_DEVICE_ID_LENGTH = 512
};

/**
 * The IAudioDeviceCollection class.
 */
class IAudioDeviceCollection {
public:
  virtual ~IAudioDeviceCollection() {}

  /**
   * @brief Gets the total number of audio playback or audio capture devices.
   *
   * @details
   * If you call `enumeratePlaybackDevices` before this method, the SDK returns the number of audio
   * playback devices. If you call `enumerateRecordingDevices` before this method, the SDK returns the
   * number of audio capture devices.
   *
   * @return
   * The number of audio playback or audio capture devices.
   */
  virtual int getCount() = 0;

  /**
   * @brief Gets a specified piece of information about an indexed video device.
   *
   * @param index The index value of the video device. The value of this parameter must be less than
   * the value returned in `getCount`.
   * @param deviceName The device name. The maximum length is `MAX_DEVICE_ID_LENGTH_TYPE`.
   * @param deviceId The device ID of the video device. The maximum length is
   * `MAX_DEVICE_ID_LENGTH_TYPE`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int getDevice(int index, char deviceName[MAX_DEVICE_ID_LENGTH],
                        char deviceId[MAX_DEVICE_ID_LENGTH]) = 0;

  /**
   * @brief Gets the audio device information and device type by index.
   *
   * @details
   * You can call this method to get the name, type and ID of a specified audio device.
   *
   * @note This method is for macOS only.
   *
   * @param index An input parameter. The index of the device.
   * @param deviceId An output parameter. The device ID. The maximum length is
   * `MAX_DEVICE_ID_LENGTH_TYPE`.
   * @param deviceName An output parameter. The device name. The maximum length is
   * `MAX_DEVICE_ID_LENGTH_TYPE`.
   * @param deviceTypeName Output parameter; indicates the type of audio devices, such as built-in,
   * USB and HDMI. The maximum length is `MAX_DEVICE_ID_LENGTH_TYPE`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int getDevice(int index, char deviceName[MAX_DEVICE_ID_LENGTH], char deviceTypeName[MAX_DEVICE_ID_LENGTH],
                        char deviceId[MAX_DEVICE_ID_LENGTH]) = 0;                      

  /**
   * @brief Specifies the video capture device with the device ID.
   *
   * @note
   * - Plugging or unplugging a device does not change its device ID.
   * - This method is for Windows and macOS only.
   *
   * @param deviceId The device ID. You can get the device ID by calling `enumerateVideoDevices`.
   * The maximum length is `MAX_DEVICE_ID_LENGTH_TYPE`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setDevice(const char deviceId[MAX_DEVICE_ID_LENGTH]) = 0;

  /**
   * @brief Gets the default audio device of the system.
   *
   * @details
   * - This method is for Windows and macOS only.
   * - You need to call `enumeratePlaybackDevices` or `enumerateRecordingDevices` to get the device
   * list before calling this method.
   *
   * @param deviceName Output parameter; the name of the system's default audio device. The maximum
   * length is `MAX_DEVICE_ID_LENGTH_TYPE`.
   * @param deviceId Output parameter; the device ID of the system default audio device. The maximum
   * length is `MAX_DEVICE_ID_LENGTH_TYPE`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int getDefaultDevice(char deviceName[MAX_DEVICE_ID_LENGTH], char deviceId[MAX_DEVICE_ID_LENGTH]) = 0;

  /**
   * @brief Gets the system‘s default audio device and its type.
   *
   * @details
   * - This method applies to macOS only.
   * - You need to call `enumeratePlaybackDevices` or `enumerateRecordingDevices` to get the device
   * list before calling this method.
   *
   * @param deviceName Output parameter; the name of the system's default audio device. The maximum
   * length is `MAX_DEVICE_ID_LENGTH_TYPE`.
   * @param deviceTypeName Output parameter; indicates the type of audio devices, such as built-in,
   * USB and HDMI. The maximum length is `MAX_DEVICE_ID_LENGTH_TYPE`.
   * @param deviceId Output parameter; the device ID of the system default audio device. The maximum
   * length is `MAX_DEVICE_ID_LENGTH_TYPE`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int getDefaultDevice(char deviceName[MAX_DEVICE_ID_LENGTH], char deviceTypeName[MAX_DEVICE_ID_LENGTH], char deviceId[MAX_DEVICE_ID_LENGTH]) = 0; 

  /**
   * @brief Sets the volume of the app.
   *
   * @param volume The volume of the app. The value range is [0,255].
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setApplicationVolume(int volume) = 0;

  /**
   * @brief Retrieves the volume of the app.
   *
   * @param volume The volume of the app. The value range is [0,255].
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int getApplicationVolume(int &volume) = 0;

  /**
   * @brief Mutes/Unmutes the app.
   *
   * @param mute Whether to mute the app:
   * - `true`: Mute the app.
   * - `false`: Unmute the app.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setApplicationMute(bool mute) = 0;

  /**
   * @brief Checks whether the app is muted.
   *
   * @param mute Whether the app is muted:
   * - `true`: The app is muted.
   * - `false`: The app is not muted.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int isApplicationMute(bool &mute) = 0;

  /**
   * @brief Releases all the resources occupied by the `IAudioDeviceCollection` object.
   */
  virtual void release() = 0;
};

/**
 * The IAudioDeviceManager class.
 */
class IAudioDeviceManager : public RefCountInterface {
public:
  virtual ~IAudioDeviceManager() {}

  /**
   * @brief Enumerates the audio playback devices.
   *
   * @details
   * This method returns an `IAudioDeviceCollection` object that includes all audio playback devices
   * in the system. With the `IAudioDeviceCollection` object, the application can enumerate video
   * devices. The application must call the `release` method to release the returned object after
   * using it.
   *
   * @note This method is for Windows and macOS only.
   *
   * @return
   * - Success: Returns an `IAudioDeviceCollection` object that includes all audio playback devices in
   * the system.
   * - Failure: NULL.
   */
  virtual IAudioDeviceCollection *enumeratePlaybackDevices() = 0;

  /**
   * @brief Enumerates the audio capture devices.
   *
   * @details
   * This method returns an `IAudioDeviceCollection` object that includes all audio capture devices in
   * the system. With the `IAudioDeviceCollection` object, the application can enumerate video
   * devices. The application must call the `release` method to release the returned object after
   * using it.
   *
   * @note This method is for Windows and macOS only.
   *
   * @return
   * - Success: An `IAudioDeviceCollection` object including all audio capture devices.
   * - Failure: NULL.
   */
  virtual IAudioDeviceCollection *enumerateRecordingDevices() = 0;

  /**
   * @brief Sets the audio playback device.
   *
   * @details
   * You can call this method to change the audio route currently being used, but this does not change
   * the default audio route. For example, if the default audio route is speaker 1, you call this
   * method to set the audio route as speaker 2 before joinging a channel and then start a device
   * test, the SDK conducts device test on speaker 2. After the device test is completed and you join
   * a channel, the SDK still uses speaker 1, the default audio route.
   *
   * @note This method is for Windows and macOS only.
   *
   * @param deviceId The ID of the specified audio playback device. You can get the device ID by
   * calling `enumeratePlaybackDevices`. Connecting or disconnecting the audio device does not change
   * the value of `deviceId`.
   * The maximum length is `MAX_DEVICE_ID_LENGTH_TYPE`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setPlaybackDevice(const char deviceId[MAX_DEVICE_ID_LENGTH]) = 0;

  /**
   * @brief Retrieves the audio playback device associated with the device ID.
   *
   * @note This method is for Windows and macOS only.
   *
   * @param deviceId Output parameter. The device ID of the audio playback device. The maximum length
   * is `MAX_DEVICE_ID_LENGTH_TYPE`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int getPlaybackDevice(char deviceId[MAX_DEVICE_ID_LENGTH]) = 0;

  /**
   * @brief Retrieves the information of the audio playback device.
   *
   * @note This method is for Windows and macOS only.
   *
   * @param deviceId Th ID of the audio playback device. The maximum length is
   * `MAX_DEVICE_ID_LENGTH_TYPE`.
   * @param deviceName Output parameter; the name of the playback device. The maximum length is
   * `MAX_DEVICE_ID_LENGTH_TYPE`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int getPlaybackDeviceInfo(char deviceId[MAX_DEVICE_ID_LENGTH],
                                    char deviceName[MAX_DEVICE_ID_LENGTH]) = 0;

  /**
   * @brief Gets the information and type of the audio playback device.
   *
   * @details
   * This method applies to macOS only.
   *
   * @param deviceId Th ID of the audio playback device. The maximum length is
   * `MAX_DEVICE_ID_LENGTH_TYPE`.
   * @param deviceName Output parameter; the name of the playback device. The maximum length is
   * `MAX_DEVICE_ID_LENGTH_TYPE`.
   * @param deviceTypeName Output parameter; indicates the type of audio playback devices, such as
   * built-in, USB and HDMI. The maximum length is `MAX_DEVICE_ID_LENGTH_TYPE`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int getPlaybackDeviceInfo(char deviceId[MAX_DEVICE_ID_LENGTH], char deviceName[MAX_DEVICE_ID_LENGTH], char deviceTypeName[MAX_DEVICE_ID_LENGTH]) = 0;

  /**
   * @brief Sets the volume of the audio playback device.
   *
   * @note This method applies to Windows only.
   *
   * @param volume The volume of the audio playback device. The value range is [0,255].
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setPlaybackDeviceVolume(int volume) = 0;

  /**
   * @brief Retrieves the volume of the audio playback device.
   *
   * @param volume An output parameter. The volume of the audio playback device. The value range is
   * [0,255].
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int getPlaybackDeviceVolume(int *volume) = 0;

  /**
   * @brief Sets the audio capture device.
   *
   * @details
   * You can call this method to change the audio route currently being used, but this does not change
   * the default audio route. For example, if the default audio route is microphone, you call this
   * method to set the audio route as bluetooth earphones before joinging a channel and then start a
   * device test, the SDK conducts device test on the bluetooth earphones. After the device test is
   * completed and you join a channel, the SDK still uses the microphone for audio capturing.
   *
   * @note This method is for Windows and macOS only.
   *
   * @param deviceId The ID of the audio capture device. You can get the Device ID by calling
   * `enumerateRecordingDevices`. Connecting or disconnecting the audio device does not change the
   * value of `deviceId`.
   * The maximum length is `MAX_DEVICE_ID_LENGTH_TYPE`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setRecordingDevice(const char deviceId[MAX_DEVICE_ID_LENGTH]) = 0;

  /**
   * @brief Gets the current audio recording device.
   *
   * @note This method is for Windows and macOS only.
   *
   * @param deviceId An output parameter. The device ID of the recording device. The maximum length is
   * `MAX_DEVICE_ID_LENGTH_TYPE`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int getRecordingDevice(char deviceId[MAX_DEVICE_ID_LENGTH]) = 0;

  /**
   * @brief Retrieves the information of the audio recording device.
   *
   * @note This method is for Windows and macOS only.
   *
   * @param deviceId Th ID of the audio playback device. The maximum length is
   * `MAX_DEVICE_ID_LENGTH_TYPE`.
   * @param deviceName Output parameter; the name of the playback device. The maximum length is
   * `MAX_DEVICE_ID_LENGTH_TYPE`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int getRecordingDeviceInfo(char deviceId[MAX_DEVICE_ID_LENGTH],
                                     char deviceName[MAX_DEVICE_ID_LENGTH]) = 0;

  /**
   * @brief Gets the information and type of the audio capturing device.
   *
   * @details
   * This method applies to macOS only.
   *
   * @param deviceId Th ID of the audio playback device. The maximum length is
   * `MAX_DEVICE_ID_LENGTH_TYPE`.
   * @param deviceName Output parameter; the name of the playback device. The maximum length is
   * `MAX_DEVICE_ID_LENGTH_TYPE`.
   * @param deviceTypeName Output parameter; indicates the type of audio capturing devices, such as
   * built-in, USB and HDMI. The maximum length is `MAX_DEVICE_ID_LENGTH_TYPE`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int getRecordingDeviceInfo(char deviceId[MAX_DEVICE_ID_LENGTH], char deviceName[MAX_DEVICE_ID_LENGTH], char deviceTypeName[MAX_DEVICE_ID_LENGTH]) = 0;    
                               
  /**
   * @brief Sets the volume of the audio capture device.
   *
   * @details
   * This method is for Windows and macOS only.
   *
   * @param volume The volume of the audio recording device. The value range is [0,255]. 0 means no
   * sound, 255 means maximum volume.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setRecordingDeviceVolume(int volume) = 0;

  /**
   * @brief Retrieves the volume of the audio recording device.
   *
   * @note This method applies to Windows only.
   *
   * @param volume An output parameter. It indicates the volume of the audio recording device. The
   * value range is [0,255].
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int getRecordingDeviceVolume(int *volume) = 0;

  /**
   * @brief Sets the loopback device.
   *
   * @details
   * The SDK uses the current playback device as the loopback device by default. If you want to
   * specify another audio device as the loopback device, call this method, and set `deviceId` to the
   * loopback device you want to specify.
   * You can call this method to change the audio route currently being used, but this does not change
   * the default audio route. For example, if the default audio route is microphone, you call this
   * method to set the audio route as a sound card before joinging a channel and then start a device
   * test, the SDK conducts device test on the sound card. After the device test is completed and you
   * join a channel, the SDK still uses the microphone for audio capturing.
   *
   * @note
   * This method is for Windows and macOS only.
   * The scenarios where this method is applicable are as follows:
   * Use app A to play music through a Bluetooth headset; when using app B for a video conference,
   * play through the speakers.
   * - If the loopback device is set as the Bluetooth headset, the SDK publishes the music in app A to
   * the remote end.
   * - If the loopback device is set as the speaker, the SDK does not publish the music in app A to
   * the remote end.
   * - If you set the loopback device as the Bluetooth headset, and then use a wired headset to play
   * the music in app A, you need to call this method again, set the loopback device as the wired
   * headset, and the SDK continues to publish the music in app A to remote end.
   *
   * @param deviceId Specifies the loopback device of the SDK. You can get the device ID by calling
   * `enumeratePlaybackDevices`. Connecting or disconnecting the audio device does not change the
   * value of `deviceId`.
   * The maximum length is `MAX_DEVICE_ID_LENGTH_TYPE`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setLoopbackDevice(const char deviceId[MAX_DEVICE_ID_LENGTH]) = 0;

  /**
   * @brief Gets the current loopback device.
   *
   * @details
   * This method is for Windows and macOS only.
   *
   * @param deviceId Output parameter, the ID of the current loopback device.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int getLoopbackDevice(char deviceId[MAX_DEVICE_ID_LENGTH]) = 0;

  /**
   * @brief Mutes the audio playback device.
   *
   * @param mute Whether to mute the audio playback device:
   * - `true`: Mute the audio playback device.
   * - `false`: Unmute the audio playback device.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setPlaybackDeviceMute(bool mute) = 0;

  /**
   * @brief Retrieves whether the audio playback device is muted.
   *
   * @param mute Whether the audio playback device is muted.
   * - `true`: The audio playback device is muted.
   * - `false`: The audio playback device is unmuted.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int getPlaybackDeviceMute(bool *mute) = 0;

  /**
   * @brief Sets the mute status of the audio capture device.
   *
   * @param mute Whether to mute the audio recording device:
   * - `true`: Mute the audio capture device.
   * - `false`: Unmute the audio capture device.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setRecordingDeviceMute(bool mute) = 0;

  /**
   * @brief Gets whether the audio capture device is muted.
   *
   * @param mute Whether the audio capture device is muted.
   * - `true`: The microphone is muted.
   * - `false`: The microphone is unmuted.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int getRecordingDeviceMute(bool *mute) = 0;

  /**
   * @brief Starts the audio playback device test.
   *
   * @details
   * This method tests whether the audio device for local playback works properly. Once a user starts
   * the test, the SDK plays an audio file specified by the user. If the user can hear the audio, the
   * playback device works properly.
   * After calling this method, the SDK triggers the `onAudioVolumeIndication` callback every 100 ms,
   * reporting `uid` = 1 and the volume information of the playback device.
   * The difference between this method and the `startEchoTest` method is that the former checks if
   * the local audio playback device is working properly, while the latter can check the audio and
   * video devices and network conditions.
   *
   * @note Call this method before joining a channel. After the test is completed, call
   * `stopPlaybackDeviceTest` to stop the test before joining a channel.
   *
   * @param testAudioFilePath The path of the audio file. The data format is string in UTF-8.
   * - Supported file formats: wav, mp3, m4a, and aac.
   * - Supported file sample rates: 8000, 16000, 32000, 44100, and 48000 Hz.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int startPlaybackDeviceTest(const char *testAudioFilePath) = 0;

  /**
   * @brief Stops the audio playback device test.
   *
   * @details
   * This method stops the audio playback device test. You must call this method to stop the test
   * after calling the `startPlaybackDeviceTest` method.
   *
   * @note Call this method before joining a channel.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int stopPlaybackDeviceTest() = 0;

  /**
   * @brief Starts the audio capturing device test.
   *
   * @details
   * This method tests whether the audio capturing device works properly. After calling this method,
   * the SDK triggers the `onAudioVolumeIndication` callback at the time interval set in this method,
   * which reports `uid` = 0 and the volume information of the capturing device.
   * The difference between this method and the `startEchoTest` method is that the former checks if
   * the local audio capturing device is working properly, while the latter can check the audio and
   * video devices and network conditions.
   *
   * @note Call this method before joining a channel. After the test is completed, call
   * `stopRecordingDeviceTest` to stop the test before joining a channel.
   *
   * @param indicationInterval The interval (ms) for triggering the `onAudioVolumeIndication`
   * callback. This value should be set to greater than 10, otherwise, you will not receive the
   * `onAudioVolumeIndication` callback and the SDK returns the error code `-2`. Agora recommends that
   * you set this value to 100.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   *   - -2: Invalid parameters. Check your parameter settings.
   */
  virtual int startRecordingDeviceTest(int indicationInterval) = 0;

  /**
   * @brief Stops the audio capturing device test.
   *
   * @details
   * This method stops the audio capturing device test. You must call this method to stop the test
   * after calling the `startRecordingDeviceTest` method.
   *
   * @note Call this method before joining a channel.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int stopRecordingDeviceTest() = 0;

  /**
   * @brief Starts an audio device loopback test.
   *
   * @details
   * This method tests whether the local audio capture device and playback device are working
   * properly. After starting the test, the audio capture device records the local audio, and the
   * audio playback device plays the captured audio. The SDK triggers two independent
   * `onAudioVolumeIndication` callbacks at the time interval set in this method, which reports the
   * volume information of the capture device ( `uid` = 0) and the volume information of the playback
   * device ( `uid` = 1) respectively.
   *
   * @note
   * - This method is for Windows and macOS only.
   * - You can call this method either before or after joining a channel.
   * - This method only takes effect when called by the host.
   * - This method tests local audio devices and does not report the network conditions.
   * - When you finished testing, call `stopAudioDeviceLoopbackTest` to stop the audio device loopback
   * test.
   *
   * @param indicationInterval The time interval (ms) at which the SDK triggers the
   * `onAudioVolumeIndication` callback. Agora recommends setting a value greater than 200 ms. This
   * value must not be less than 10 ms; otherwise, you can not receive the `onAudioVolumeIndication`
   * callback.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int startAudioDeviceLoopbackTest(int indicationInterval) = 0;

  /**
   * @brief Stops the audio device loopback test.
   *
   * @note
   * - This method is for Windows and macOS only.
   * - You can call this method either before or after joining a channel.
   * - This method only takes effect when called by the host.
   * - Ensure that you call this method to stop the loopback test after calling the
   * `startAudioDeviceLoopbackTest` method.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int stopAudioDeviceLoopbackTest() = 0;

  /**
   * @brief Sets the audio playback device used by the SDK to follow the system default audio playback
   * device.
   *
   * @note This method is for Windows and macOS only.
   *
   * @param enable Whether to follow the system default audio playback device:
   * - `true`: Follow the system default audio playback device. The SDK immediately switches the audio
   * playback device when the system default audio playback device changes.
   * - `false`: Do not follow the system default audio playback device. The SDK switches the audio
   * playback device to the system default audio playback device only when the currently used audio
   * playback device is disconnected.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int followSystemPlaybackDevice(bool enable) = 0;

  /**
   * @brief Sets the audio recording device used by the SDK to follow the system default audio
   * recording device.
   *
   * @note This method is for Windows and macOS only.
   *
   * @param enable Whether to follow the system default audio recording device:
   * - `true`: Follow the system default audio playback device. The SDK immediately switches the audio
   * recording device when the system default audio recording device changes.
   * - `false`: Do not follow the system default audio playback device. The SDK switches the audio
   * recording device to the system default audio recording device only when the currently used audio
   * recording device is disconnected.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int followSystemRecordingDevice(bool enable) = 0;

  /**
   * @brief Sets whether the loopback device follows the system default playback device.
   *
   * @details
   * This method is for Windows and macOS only.
   *
   * @param enable Whether to follow the system default audio playback device:
   * - `true`: Follow the system default audio playback device. When the default playback device of
   * the system is changed, the SDK immediately switches to the loopback device.
   * - `false`: Do not follow the system default audio playback device. The SDK switches the audio
   * loopback device to the system default audio playback device only when the current audio playback
   * device is disconnected.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int followSystemLoopbackDevice(bool enable) = 0;

  /**
   * @brief Releases all the resources occupied by the IAudioDeviceManager object.
   */
  virtual void release() = 0;
};

} // namespace rtc
} // namespace agora
