//  Agora SDK
//
//  Copyright (c) 2018 Agora.io. All rights reserved.
//

#pragma once  // NOLINT(build/header_guard)

#include "IAgoraLog.h"
#include "AgoraBase.h"
#include "AgoraOptional.h"

namespace agora {
namespace rtc {
class IRtcConnection;
class IRtmpConnection;
class ILocalUser;
class IMediaDeviceManager;
class INGAudioDeviceManager;
struct TConnectionInfo;
struct RtcConnectionConfiguration;
struct RtmpConnectionConfiguration;
class ILocalAudioTrack;
class IMediaPlayerSource;
class IMediaStreamingSource;
class ICameraCapturer;
class IScreenCapturer;
class IScreenCapturer2;
class IAudioPcmDataSender;
class IAudioEncodedFrameSender;
class IVideoFrameSender;
class IVideoEncodedImageSender;
class IVideoSourceNode;
class IVideoMixerSource;
class IVideoFrameTransceiver;
class ILocalVideoTrack;
class IMediaNodeFactory;
class IRecordingDeviceSource;
class IRemoteAudioMixerSource;
class IRtmpStreamingService;
class IMediaPacketSender;
class IMediaRelayService;
class IAudioDeviceManager;
class IAudioDeviceManagerObserver;

class IMediaExtensionObserver;
class IExtensionProvider;
class IFileUploaderService;
/**
 * The audio encoder configuration.
 */
struct AudioEncoderConfiguration {
  /**
   * The audio profile: #AUDIO_PROFILE_TYPE
   */
  AUDIO_PROFILE_TYPE audioProfile;
  // absl::optional<DtxStatus> dtx;
  // double bitrate_priority = 1.0;
  // absl::optional<int> ptime;
  // FEC parameters
  // Rtx parameters

  AudioEncoderConfiguration() : audioProfile(AUDIO_PROFILE_DEFAULT) {}
};

}  // namespace rtc

namespace signaling {
class ISignalingEngine;
}

namespace rtm {
class IRtmService;
}

namespace base {
class IEngineBase;
class IServiceObserver;

/**
 * The global configurations for \ref agora::base::IAgoraService "AgoraService".
 *
 * Set these configurations when calling \ref agora::base::IAgoraService::initialize "initialize".
 */
struct AgoraServiceConfiguration {
  /**
   * Whether to enable the audio processing module.
   * - `true`: (Default) Enable the audio processing module.
   * - `false`: Disable the audio processing module. If you disable the audio processing module, you cannot create audio tracks.
   */
  bool enableAudioProcessor;
  /**
   * Whether to enable the audio device module. The function of the audio device module is to manage audio devices,
   * such as recording and playing audio.
   * - `true`: (Default) Enable the audio device module. Audio recording and playback is available.
   * - `false`: Disable the audio device module. Audio recording and playback is unavailable.
   *
   * @note
   * If you set `enableAudioDevice` as `false` and set `enableAudioProcessor` as `true`, you cannot use audio devices,
   * but you can push PCM audio data.
   */
  bool enableAudioDevice;
  /**
   * Whether to enable video.
   * - `true`: Enable video.
   * - `false`: (Default) Disable video.
   */
  bool enableVideo;
  /**
   * The user context.
   * - For Windows, it is the handle of the window that loads the video. Specify this value to support plugging or unplugging the video devices while the host is powered on.
   * - For Android, it is the context of activity.
   */
  void* context;
  /**
   * The App ID of your project.
   */
  const char* appId;

  /**
   * The supported area code, default is AREA_CODE_GLOB
   */
  unsigned int areaCode;

  /** The channel profile. For details, see \ref agora::CHANNEL_PROFILE_TYPE "CHANNEL_PROFILE_TYPE". The default channel profile is `CHANNEL_PROFILE_LIVE_BROADCASTING`.
   */
  agora::CHANNEL_PROFILE_TYPE channelProfile;
  /**
   * The audio scenario. See \ref agora::rtc::AUDIO_SCENARIO_TYPE "AUDIO_SCENARIO_TYPE". The default value is `AUDIO_SCENARIO_DEFAULT`.
   */
  rtc::AUDIO_SCENARIO_TYPE audioScenario;
  /**
   * The config for custumer set log path, log size and log level.
   */
  commons::LogConfig logConfig;
  /**
   * Whether to enable string uid.
   */
  bool useStringUid;
  /**
   * The service observer.
   */
  IServiceObserver* serviceObserver;

  /**
   * Thread priority for SDK common threads
   */
  Optional<rtc::THREAD_PRIORITY_TYPE> threadPriority;
  /**
   * Whether use egl context in current thread as sdk‘s root egl context 
   * which shared by all egl related modules. eg. camera capture, video renderer.
   * @note
   * This property applies to Android only.
   */
  bool useExternalEglContext;

  AgoraServiceConfiguration() : enableAudioProcessor(true),
                                enableAudioDevice(true),
                                enableVideo(false),
                                context(NULL),
                                appId(NULL),
                                areaCode(rtc::AREA_CODE_GLOB),
                                channelProfile(agora::CHANNEL_PROFILE_LIVE_BROADCASTING),
                                audioScenario(rtc::AUDIO_SCENARIO_DEFAULT),
                                useStringUid(false),
                                serviceObserver(NULL),
                                useExternalEglContext(false){}
};
/**
 * The audio session configurations.
 *
 * Set these configurations when calling \ref agora::base::IAgoraService::setAudioSessionConfiguration "setAudioSessionConfiguration".
 */
struct AudioSessionConfiguration {
  /**
   * Whether to enable audio input (recording) and audio output (playback):
   * - `true`: Enable audio recording and playback.
   * - `false`: Disable audio recording and playback, which prevents audio input
   * and output.
   *
   * @note
   * - For the recording function to work, the user must grant permission for audio recording.
   * - By default, the audio of your app is nonmixable, which means
   * activating audio sessions in your app interrupts other nonmixable audio sessions.
   * Set `allowMixWithOthers` as `true` to allow audio mixing.
   */
  Optional<bool> playbackAndRecord;
  /**
   * Whether to enable chat mode:
   * - `true`: Enable chat mode. This mode is for apps that are engaged in two-way
   * real-time communication, such as a voice or video chat.
   * - `false`: Disable chat mode.
   *
   * For a video chat, set this member as `true` and set the audio route to the speaker.
   */
  Optional<bool> chatMode;
  /**
   * Whether the audio defaults to the built-in speaker:
   * - `true`: The audio defaults to the built-in speaker.
   * - `false`: The audio does not default to the built-in speaker.
   *
   * @note
   * This member is available only when `playbackAndRecord` is set as `true`.
   */
  Optional<bool> defaultToSpeaker;
  /**
   * Whether to temporarily change the current audio route to the built-in speaker:
   * - `true`: Set the current audio route to the built-in speaker.
   * - `false`: Do not set the current audio route to the built-in speaker.
   *
   * @note
   * This member is available only when the `playbackAndRecord` member is set as `true`.
   */
  Optional<bool> overrideSpeaker;
  /**
   * Whether to mix the audio from this session with the audio from active audio sessions in other apps.
   * - `true`: Mix the audio sessions.
   * - `false`: Do not mix the audio session.
   *
   * @note
   * This member is available only when the `playbackAndRecord` member is set as `true`.
   */
  Optional<bool> allowMixWithOthers;
  /**
   * Whether to allow Bluetooth handsfree devices to appear as available audio input
   * devices:
   * - `true`: Allow Bluetooth handsfree devices to appear as available audio input routes.
   * - `false`: Do not allow Bluetooth handsfree devices to appear as available audio input
   * routes.
   *
   * @note
   * This member is available only when the `playbackAndRecord` member is set as `true`.
   */
  Optional<bool> allowBluetooth;
  /**
   * Whether to allow the audio from this session to be routed to Bluetooth
   * devices that support the Advanced Audio Distribution Profile (A2DP).
   * - `true`: Allow the audio from this session to be routed to Bluetooth devices that
   * support the Advanced Audio Distribution Profile (A2DP).
   * - `false`: Do not allow the audio from this session to be routed to Bluetooth devices that
   * support the Advanced Audio Distribution Profile (A2DP).
   *
   * @note
   * This member is available only when the `playbackAndRecord` member is set as `true`.
   */
  Optional<bool> allowBluetoothA2DP;
  /**
   * The expected audio sample rate (kHz) of this session.
   *
   * The value range is [8,48]. The actual sample rate may differ based on the audio sampling
   * device in use.
   */
  Optional<double> sampleRate;
  /**
   * The expected input and output buffer duration (ms) of this session.
   *
   * The value range is [0,93]. The actual I/O buffer duration might be lower
   * than the set value based on the hardware in use.
   */
  Optional<double> ioBufferDuration;
  /**
   * The expected number of input audio channels of this session.
   */
  Optional<int> inputNumberOfChannels;
  /**
   * The expected number of output audio channels of this session.
   */
  Optional<int> outputNumberOfChannels;

  void SetAll(AudioSessionConfiguration& change) {
    SetFrom(&playbackAndRecord, change.playbackAndRecord);
    SetFrom(&chatMode, change.chatMode);
    SetFrom(&defaultToSpeaker, change.defaultToSpeaker);
    SetFrom(&overrideSpeaker, change.overrideSpeaker);
    SetFrom(&allowMixWithOthers, change.allowMixWithOthers);
    SetFrom(&allowBluetooth, change.allowBluetooth);
    SetFrom(&allowBluetoothA2DP, change.allowBluetoothA2DP);
    SetFrom(&sampleRate, change.sampleRate);
    SetFrom(&ioBufferDuration, change.ioBufferDuration);
    SetFrom(&inputNumberOfChannels, change.inputNumberOfChannels);
    SetFrom(&outputNumberOfChannels, change.outputNumberOfChannels);
  }

  bool operator==(const AudioSessionConfiguration& o) const {
    return playbackAndRecord == o.playbackAndRecord && chatMode == o.chatMode &&
           defaultToSpeaker == o.defaultToSpeaker && overrideSpeaker == o.overrideSpeaker &&
           allowMixWithOthers == o.allowMixWithOthers && allowBluetooth == o.allowBluetooth &&
           allowBluetoothA2DP == o.allowBluetoothA2DP && sampleRate == o.sampleRate &&
           ioBufferDuration == o.ioBufferDuration &&
           inputNumberOfChannels == o.inputNumberOfChannels &&
           outputNumberOfChannels == o.outputNumberOfChannels;
  }
  bool operator!=(const AudioSessionConfiguration& o) const { return !(*this == o); }

 private:
  template <typename T>
  static void SetFrom(Optional<T>* s, const Optional<T>& o) {
    if (o) {
      *s = o;
    }
  }
};
/**
 * The audio mixing mode.
 */
enum TMixMode {
  /**
   * Mix all the local audio tracks in the channel.
   */
  MIX_ENABLED,
  /**
   * Do not mix the local audio tracks in the channel.
   */
  MIX_DISABLED,
};

/**
 * The IServiceObserver class.
 */
class IServiceObserver {
public:
  virtual ~IServiceObserver() {}

  /**
    * Reports the permission error.
    * @param permission {@link PERMISSION}
    */
  virtual void onPermissionError(agora::rtc::PERMISSION_TYPE permissionType) {}
  /**
   * Reports the audio device error.
   * @param error {@link ERROR_CODE_TYPE}
   */
  virtual void onAudioDeviceError(ERROR_CODE_TYPE error, const char* description) {}
};

/**
 * The IAgoraService class.
 *
 * `IAgoraService` is the entry point of Agora low-level APIs. Use this interface to
 * create access points to Agora interfaces, including RTC connections and media tracks.
 *
 * You can create an `IAgoraService` object by calling \ref agora::base::IAgoraService::createAgoraService "createAgoraService".
 *
 * You can configure the `IAgoraService` object for different user scenarios on the global level by using `AgoraServiceConfiguration`.
 */
class IAgoraService {
 public:
  /**
   * Initializes the \ref agora::base::IAgoraService "AgoraService" object.
   *
   * @param config The configuration of the initialization. For details, see \ref agora::base::AgoraServiceConfiguration "AgoraServiceConfiguration".
   * @return
   * - 0: Success.
   * - < 0: Failure.
   *   - `ERR_INVALID_ARGUMENT`, if `context` in `AgoraServiceConfiguration` is not provided for
   * Android.
   *   - `ERR_INIT_NET_ENGINE`, if the network engine cannot be initialized. On Windows, the error occurs mostly because the connection to the local port is disabled by the firewall. In this case, turn off the firewall and then turn it on again.
   */
  virtual int initialize(const AgoraServiceConfiguration& config) = 0;

  /**
   * Flush log & cache before exit
   */
  virtual void atExit() = 0;

  /**
   * Releases the \ref agora::base::IAgoraService "AgoraService" object.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int release() = 0;

  /**
   * Configures the preset audio scenario.
   *
   * @param scenario The preset audio scenario: \ref agora::rtc::AUDIO_SCENARIO_TYPE
   * "AUDIO_SCENARIO_TYPE".
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setAudioSessionPreset(agora::rtc::AUDIO_SCENARIO_TYPE scenario) = 0;

  /**
   * Customizes the audio session configuration.
   *
   * @param config The reference to the audio session configuration: \ref agora::base::AudioSessionConfiguration "AudioSessionConfiguration".
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setAudioSessionConfiguration(const AudioSessionConfiguration& config) = 0;

  /**
   * Gets the audio session configuration.
   *
   * @param [out] config The pointer to the audio session configuration: \ref agora::base::AudioSessionConfiguration "AudioSessionConfiguration".
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int getAudioSessionConfiguration(AudioSessionConfiguration* config) = 0;

  /**
   * Sets the path and size of the SDK log files.
   *
   * The SDK records all the log data during the SDK runtime in two log files,
   * each with a default size of 512 KB. If you set `fileSize` as 1024 KB,
   * the SDK outputs log files with a maximum size of 2 MB. If the total size
   * of the log files exceeds the set value, the new output log
   * overwrites the old output log.
   *
   * @note
   * To ensure that the output log is complete, call this method immediately after calling
   * \ref agora::base::IAgoraService::initialize "initialize".
   *
   * @param filePath The pointer to the log file. Ensure that the directory of the log file exists and is writable.
   * @param fileSize The size of the SDK log file size (KB).
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setLogFile(const char* filePath, unsigned int fileSize) = 0;
  /**
   * Sets the SDK log output filter.
   *
   * The log level follows the sequence of OFF, CRITICAL, ERROR, WARNING, INFO, and DEBUG.
   *
   * Select a level to output the logs in and above the selected level.
   * For example, if you set the log level to WARNING, you can see the logs in the levels of CRITICAL, ERROR, and WARNING.
   *
   * @param filters The log output filter.
   * - `LOG_LEVEL_NONE (0x0000)`: Do not output any log file.
   * - `LOG_LEVEL_INFO (0x0001)`: (Recommended) Output log files of the INFO level.
   * - `LOG_LEVEL_WARN (0x0002)`: Output log files of the WARN level.
   * - `LOG_LEVEL_ERROR (0x0004)`: Output log files of the ERROR level.
   * - `LOG_LEVEL_FATAL (0x0008)`: Output log files of the FATAL level.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setLogFilter(unsigned int filters) = 0;

  /**
   * Creates an \ref agora::rtc::IRtcConnection "RtcConnection" object and returns the pointer.
   *
   * @param cfg The reference to the RTC connection configuration: \ref agora::rtc::RtcConnectionConfiguration "RtcConnectionConfiguration".
   * @return
   * - The pointer to \ref rtc::IRtcConnection "IRtcConnection": Success.
   * - A null pointer: Failure.
   */
  virtual agora_refptr<rtc::IRtcConnection> createRtcConnection(
      const rtc::RtcConnectionConfiguration& cfg) = 0;

  virtual agora_refptr<rtc::IRtmpConnection> createRtmpConnection(
      const rtc::RtmpConnectionConfiguration& cfg) = 0;

  /**
   * Creates a local audio track object and returns the pointer.
   *
   * By default, the audio track is created from the selected audio input device, such as
   * the built-in microphone on a mobile device.
   *
   * @return
   * - The pointer to \ref rtc::ILocalAudioTrack "ILocalAudioTrack": Success.
   * - A null pointer: Failure.
   * - `INVALID_STATE`, if `enableAudioProcessor` in \ref agora::base::AgoraServiceConfiguration "AgoraServiceConfiguration" is set as `false`.
   */
  virtual agora_refptr<rtc::ILocalAudioTrack> createLocalAudioTrack() = 0;

  /**
   * Creates a local audio track object with a PCM data sender and returns the pointer.
   *
   * Once created, this track can be used to send PCM audio data.
   *
   * @param audioSource The pointer to the PCM audio data sender: \ref agora::rtc::IAudioPcmDataSender "IAudioPcmDataSender".
   * @return
   * - The pointer to \ref rtc::ILocalAudioTrack "ILocalAudioTrack": Success.
   * - A null pointer: Failure.
   * - `INVALID_STATE`, if `enableAudioProcessor` in \ref agora::base::AgoraServiceConfiguration "AgoraServiceConfiguration" is set as `false`.
   */
  virtual agora_refptr<rtc::ILocalAudioTrack> createCustomAudioTrack(
      agora_refptr<rtc::IAudioPcmDataSender> audioSource) = 0;

  /**
   * Creates a local audio track object with a PCM data sender and returns the pointer.
   * The source is not intended to be mixed with other source.
   *
   * Once created, this track can be used to send PCM audio data.
   *
   * @param audioSource The pointer to the PCM audio data sender: \ref agora::rtc::IAudioPcmDataSender "IAudioPcmDataSender".
   * @return
   * - The pointer to \ref rtc::ILocalAudioTrack "ILocalAudioTrack": Success.
   * - A null pointer: Failure.
   * - `INVALID_STATE`, if `enableAudioProcessor` in \ref agora::base::AgoraServiceConfiguration "AgoraServiceConfiguration" is set as `false`.
   */
  virtual agora_refptr<rtc::ILocalAudioTrack> createDirectCustomAudioTrack(
      agora_refptr<rtc::IAudioPcmDataSender> audioSource) = 0;

  /**
   * Creates a local audio track object with a PCM data sender and returns the pointer.
   *
   * Once created, this track can be used to send PCM audio data.
   *
   * @param audioSource The pointer to the PCM audio data sender: \ref agora::rtc::IAudioPcmDataSender "IAudioPcmDataSender".
   * @param enableAec Whether enable audio echo cancellation for PCM audio data.
   * @return
   * - The pointer to \ref rtc::ILocalAudioTrack "ILocalAudioTrack": Success.
   * - A null pointer: Failure.
   * - `INVALID_STATE`, if `enableAudioProcessor` in \ref agora::base::AgoraServiceConfiguration "AgoraServiceConfiguration" is set as `false`.
   */
  virtual agora_refptr<rtc::ILocalAudioTrack> createCustomAudioTrack(
      agora_refptr<rtc::IAudioPcmDataSender> audioSource, bool enableAec) = 0;

  /**
   * Creates a local audio track object with two PCM data sender and returns the pointer.
   *
   * Once created, this track can be used to send PCM audio data with echo cancellation.
   *
   * @param audioSource The pointer to the recording PCM audio data sender: \ref agora::rtc::IAudioPcmDataSender "IAudioPcmDataSender".
   * @param audioReverseSource The pointer to the playout PCM audio data sender: \ref agora::rtc::IAudioPcmDataSender "IAudioPcmDataSender".
   * @param audioSourceDelay The delay of recording pcm data when do echo cancellation.
   * @return
   * - The pointer to \ref rtc::ILocalAudioTrack "ILocalAudioTrack": Success.
   * - A null pointer: Failure.
   * - `INVALID_STATE`, if `enableAudioProcessor` in \ref agora::base::AgoraServiceConfiguration "AgoraServiceConfiguration" is set as `false`.
   */
  virtual agora_refptr<rtc::ILocalAudioTrack> createCustomAudioTrack(
      agora_refptr<rtc::IAudioPcmDataSender> audioSource,
      agora_refptr<rtc::IAudioPcmDataSender> audioReverseSource, int audioSourceDelay) = 0;

  /**
   * Creates a local audio track object with a audio mixer source and returns the pointer.
   *
   * Once created, this track can be used to send PCM audio data.
   *
   * @param audioSource The pointer to the audio mixer source : \ref agora::rtc::IRemoteAudioMixerSource "IRemoteAudioMixerSource".
   * @return
   * - The pointer to \ref rtc::ILocalAudioTrack "ILocalAudioTrack", if the method call succeeds.
   * - A null pointer, if the method call fails.
   * - `INVALID_STATE`, if `enableAudioProcessor` in `AgoraServiceConfiguration` is set as `false`.
   */
  virtual agora_refptr<rtc::ILocalAudioTrack> createCustomAudioTrack(
      agora_refptr<rtc::IRemoteAudioMixerSource> audioSource) = 0;

  /**
   * Creates a local audio track object with an encoded audio frame sender and returns the pointer.
   *
   * Once created, this track can be used to send encoded audio frames, such as Opus frames.
   *
   * @param audioSource The pointer to the encoded audio frame sender: \ref agora::rtc::IAudioEncodedFrameSender "IAudioEncoderFrameSender".
   * @param mixMode The mixing mode of the encoded audio in the channel: #TMixMode.
   * @return
   * - The pointer to \ref rtc::ILocalAudioTrack "ILocalAudioTrack": Success.
   * - A null pointer: Failure.
   *   - `INVALID_STATE`, if `enableAudioProcessor` is set as `false` in \ref agora::base::AgoraServiceConfiguration "AgoraServiceConfiguration".
   */
  virtual agora_refptr<rtc::ILocalAudioTrack> createCustomAudioTrack(
      agora_refptr<rtc::IAudioEncodedFrameSender> audioSource, TMixMode mixMode) = 0;
/// @cond (!Linux)
  /**
   * Creates a local audio track object with a media packet sender and returns the pointer.
   *
   * Once created, this track can be used to send audio packets, such as customized UDP/RTP packets.
   *
   * @param source The pointer to the media packet sender: \ref agora::rtc::IMediaPacketSender "IMediaPacketSender".
   * @return
   * - The pointer to \ref rtc::ILocalAudioTrack "ILocalAudioTrack": Success.
   * - A null pointer: Failure.
   * - `INVALID_STATE`, if `enableAudioProcessor` is set as `false` in \ref agora::base::AgoraServiceConfiguration "AgoraServiceConfiguration".
  */
  virtual agora_refptr<rtc::ILocalAudioTrack> createCustomAudioTrack(
      agora_refptr<rtc::IMediaPacketSender> source) = 0;
  /// @endcond
  /**
   * Creates a local audio track object with an IMediaPlayerSource object and returns the pointer.
   *
   * Once created, this track can be used to send PCM audio data decoded from a media player.
   *
   * @param audioSource The pointer to the player source. See \ref agora::rtc::IMediaPlayerSource "IMediaPlayerSource".
   * @return
   * - The pointer to \ref rtc::ILocalAudioTrack "ILocalAudioTrack": Success.
   * - A null pointer: Failure.
   * - `INVALID_STATE`, if `enableAudioProcessor` is set as `false` in \ref agora::base::AgoraServiceConfiguration "AgoraServiceConfiguration".
  */
  virtual agora_refptr<rtc::ILocalAudioTrack> createMediaPlayerAudioTrack(
      agora_refptr<rtc::IMediaPlayerSource> audioSource) = 0;

  /**
   * Creates a local audio track object with an IMediaStreamingSource object and returns the pointer.
   *
   * Once created, this track can be used to send encoded audio data which demuxed from a media streaming.
   *
   * @param streamingSource The pointer to the streaming source. See \ref agora::rtc::IMediaStreamingSource "IMediaStreamingSource".
   * @return
   * - The pointer to \ref rtc::ILocalAudioTrack "ILocalAudioTrack": Success.
   * - A null pointer: Failure.
   * - `INVALID_STATE`, if `enableAudioProcessor` is set as `false` in \ref agora::base::AgoraServiceConfiguration "AgoraServiceConfiguration".
  */
  virtual agora_refptr<rtc::ILocalAudioTrack> createMediaStreamingAudioTrack(
      agora_refptr<rtc::IMediaStreamingSource> streamingSource) = 0;

  /**
   * Creates a local audio track object with the recording device source and returns the pointer.
   *
   * Once created, this track can be used to send audio data got from a recording device.
   * @param audioSource The pointer to the recording device source. See \ref agora::rtc::IRecordingDeviceSource "IRecordingDeviceSource".
   * @param enableAec Whether enable audio echo cancellation for loopback recording. If loopback
   *                  recording device is a virtual sound card, it should be false, or it should be true.
   * @return
   * - The pointer to \ref rtc::ILocalAudioTrack "ILocalAudioTrack": Success.
   * - A null pointer: Failure.
  */
  virtual agora_refptr<rtc::ILocalAudioTrack> createRecordingDeviceAudioTrack(
      agora_refptr<rtc::IRecordingDeviceSource> audioSource, bool enableAec) = 0;

  /**
   * Creates an audio device manager object and returns the pointer.
   *
   * @return
   * - The pointer to \ref rtc::INGAudioDeviceManager "INGAudioDeviceManager": Success.
   * - A null pointer: Failure.
   */
  virtual agora_refptr<rtc::INGAudioDeviceManager> createAudioDeviceManager() = 0;

  /**
   * Creates a media node factory object and returns the pointer.
   *
   * @return
   * - The pointer to \ref rtc::IMediaNodeFactory "IMediaNodeFactory": Success.
   * - A null pointer: Failure.
   */
  virtual agora_refptr<rtc::IMediaNodeFactory> createMediaNodeFactory() = 0;

  /**
   * Creates a local video track object with a camera capturer and returns the pointer.
   *
   * Once created, this track can be used to send video data captured by the camera.
   *
   * @param videoSource The pointer to the camera capturer: \ref agora::rtc::ICameraCapturer "ICameraCapturer".
   *
   * @return
   * - The pointer to \ref rtc::ILocalVideoTrack "ILocalVideoTrack": Success.
   * - A null pointer: Failure.
   */
  virtual agora_refptr<rtc::ILocalVideoTrack> createCameraVideoTrack(
      agora_refptr<rtc::ICameraCapturer> videoSource, const char* id = OPTIONAL_NULLPTR) = 0;

  /**
   * Creates a local video track object with a screen capturer and returns the pointer.
   *
   * Once created, this track can be used to send video data for screen sharing.
   *
   * @param videoSource The pointer to the screen capturer: \ref agora::rtc::IScreenCapturer "IScreenCapturer".
   * @return
   * - The pointer to \ref rtc::ILocalVideoTrack "ILocalVideoTrack": Success.
   * - A null pointer: Failure.
   */
  virtual agora_refptr<rtc::ILocalVideoTrack> createScreenVideoTrack(
      agora_refptr<rtc::IScreenCapturer> videoSource, const char* id = OPTIONAL_NULLPTR) = 0;

   /**
   * Creates a local video track object with a video mixer and returns the pointer.
   *
   * Once created, this track can be used to send video data processed by the video mixer.
   *
   * @param videoSource The pointer to the video mixer. See \ref agora::rtc::IVideoMixerSource "IVideoMixerSource".
   *
   * @return
   * - The pointer to \ref rtc::ILocalVideoTrack "ILocalVideoTrack": Success.
   * - A null pointer: Failure.
   */
  virtual agora_refptr<rtc::ILocalVideoTrack> createMixedVideoTrack(agora_refptr<rtc::IVideoMixerSource> videoSource,
                                                                    const char* id = OPTIONAL_NULLPTR) = 0;

  /**
   * Creates a local video track object with a video frame transceiver and returns the pointer.
   *
   * Once created, this track can be used to send video data processed by the transceiver.
   *
   * @param transceiver The pointer to the video transceiver. See \ref agora::rtc::IVideoFrameTransceiver "IVideoFrameTransceiver".
   *
   * @return
   * - The pointer to \ref rtc::ILocalVideoTrack "ILocalVideoTrack": Success.
   * - A null pointer: Failure.
   */
  virtual agora_refptr<rtc::ILocalVideoTrack> createTranscodedVideoTrack(agora_refptr<rtc::IVideoFrameTransceiver> transceiver,
                                                                         const char* id = OPTIONAL_NULLPTR) = 0;

/// @cond (!RTSA)
  /**
   * Creates a local video track object with a customized video source and returns the pointer.
   *
   * Once created, this track can be used to send video data from a customized source.
   *
   * @param videoSource The pointer to the customized video frame sender: \ref agora::rtc::IVideoFrameSender "IVideoFrameSender".
   * @return
   * - The pointer to \ref rtc::ILocalVideoTrack "ILocalVideoTrack": Success.
   * - A null pointer: Failure.
   */
  virtual agora_refptr<rtc::ILocalVideoTrack> createCustomVideoTrack(
      agora_refptr<rtc::IVideoFrameSender> videoSource, const char* id = OPTIONAL_NULLPTR) = 0;
/// @endcond

  /**
   * Creates a local video track object with an encoded video image sender and returns the pointer.
   *
   * Once created, this track can be used to send encoded video images, such as H.264 or VP8 frames.
   *
   * @param videoSource The pointer to the encoded video frame sender. See \ref agora::rtc::IVideoEncodedImageSender "IVideoEncodedImageSender".
   * @param options The configuration for creating video encoded image track.
   *
   * @return
   * - The pointer to \ref rtc::ILocalVideoTrack "ILocalVideoTrack": Success.
   * - A null pointer: Failure.
   */
  virtual agora_refptr<rtc::ILocalVideoTrack> createCustomVideoTrack(
      agora_refptr<rtc::IVideoEncodedImageSender> videoSource,
      const rtc::SenderOptions& options,
      const char* id = OPTIONAL_NULLPTR) = 0;

#if defined(__ANDROID__) || (defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE)
  /**
   * Creates a local video track object with a screen capture source extension and returns the pointer.
   *
   * Once created, this track can be used to work with the screen capture extension.
   *
   * @param screen The pointer to the screen capture source.
   *
   * @return
   * - The pointer to \ref rtc::ILocalVideoTrack "ILocalVideoTrack": Success.
   * - A null pointer: Failure.
   */
  virtual agora_refptr<rtc::ILocalVideoTrack> createScreenCaptureVideoTrack(
      agora_refptr<rtc::IScreenCapturer2> screen) = 0;

/**
   * Creates a local audio track object with a screen capture source extension and returns the pointer.
   *
   * Once created, this track can be used to work with the screen capture extension.
   *
   * @param screen The pointer to the screen capture source.
   *
   * @return
   * - The pointer to \ref rtc::ILocalAudioTrack "ILocalAudioTrack": Success.
   * - A null pointer: Failure.
   */
  virtual agora_refptr<rtc::ILocalAudioTrack> createScreenCaptureAudioTrack(
    agora_refptr<rtc::IScreenCapturer2> screen) = 0;
#else
  /**
   * Creates a local video track object with a screen capture source extension and returns the pointer.
   *
   * Once created, this track can be used to work with the screen capture extension.
   *
   * @param screen The pointer to the screen capture source.
   *
   * @return
   * - The pointer to \ref rtc::ILocalVideoTrack "ILocalVideoTrack": Success.
   * - A null pointer: Failure.
   */
  virtual agora_refptr<rtc::ILocalVideoTrack> createScreenCaptureVideoTrack(
      agora_refptr<rtc::IScreenCapturer> screen, const char* id = OPTIONAL_NULLPTR) = 0;
#endif

/// @cond (!Linux)
  /**
   * Creates a local video track object with a media packet sender and returns the pointer.
   *
   * Once created, this track can be used to send video packets, such as customized UDP/RTP packets.
   *
   * @param source The pointer to the media packet sender: \ref agora::rtc::IMediaPacketSender "IMediaPacketSender".
   * @return
   * - The pointer to \ref rtc::ILocalVideoTrack "ILocalVideoTrack": Success.
   * - A null pointer: Failure.
  */
  virtual agora_refptr<rtc::ILocalVideoTrack> createCustomVideoTrack(
      agora_refptr<rtc::IMediaPacketSender> source, const char* id = OPTIONAL_NULLPTR) = 0;
/// @endcond
  /**
   * Creates a local video track object with an IMediaPlayerSource object and returns the pointer.
   *
   * Once created, this track can be used to send YUV frames decoded from a player.
   *
   * @param videoSource The pointer to the player source. See \ref agora::rtc::IMediaPlayerSource "IMediaPlayerSource".
   * @return
   * - The pointer to \ref rtc::ILocalAudioTrack "ILocalAudioTrack": Success.
   * - A null pointer: Failure.
  */
  virtual agora_refptr<rtc::ILocalVideoTrack> createMediaPlayerVideoTrack(
      agora_refptr<rtc::IMediaPlayerSource> videoSource, const char* id = OPTIONAL_NULLPTR) = 0;

  /**
   * Creates a local video track object with an IMediaStreamingSource object and returns the pointer.
   *
   * Once created, this track can be used to send H264 frames which demuxed from a streaming.
   *
   * @param streamingSource The pointer to the player source. See \ref agora::rtc::IMediaStreamingSource "IMediaStreamingSource".
   * @return
   * - The pointer to \ref rtc::ILocalAudioTrack "ILocalAudioTrack": Success.
   * - A null pointer: Failure.
  */
  virtual agora_refptr<rtc::ILocalVideoTrack> createMediaStreamingVideoTrack(
      agora_refptr<rtc::IMediaStreamingSource> streamingSource, const char* id = OPTIONAL_NULLPTR) = 0;


  /**
   * Creates an RTMP streaming service object and returns the pointer.
   *
   * @param rtcConnection The pointer to \ref rtc::IRtcConnection "IRtcConnection".
   * @param appId The App ID of the live streaming service.
   * @return
   * - The pointer to \ref rtc::IRtmpStreamingService "IRtmpStreamingService": Success.
   * - A null pointer: Failure.
   */
  virtual agora_refptr<rtc::IRtmpStreamingService> createRtmpStreamingService(
      agora_refptr<rtc::IRtcConnection> rtcConnection, const char* appId) = 0;

  /**
   * Creates an Media Relay service object and returns the pointer.
   *
   * @param rtcConnection The pointer to \ref rtc::IRtcConnection "IRtcConnection".
   * @param appId The App ID of the media relay service.
   * @return
   * - The pointer to \ref rtc::IMediaRelayService "IMediaRelayService", if the method call
   * succeeds.
   * - A null pointer, if the method call fails.
   */
  virtual agora_refptr<rtc::IMediaRelayService> createMediaRelayService(
      agora_refptr<rtc::IRtcConnection> rtcConnection, const char* appId) = 0;

    /**
   * Creates an file upload object and returns the pointer.
   *
   * @param rtcConnection The pointer to \ref rtc::IRtcConnection "IRtcConnection".
   * @param appId The App ID of the media relay service.
   * @return
   * - The pointer to \ref rtc::IFileUploaderService "IFileUploaderService", if the method call
   * succeeds.
   * - A null pointer, if the method call fails.
   */
  virtual agora_refptr<rtc::IFileUploaderService> createFileUploadService(
      agora_refptr<rtc::IRtcConnection> rtcConnection, const char* appId) = 0;

  /**
   * Creates an RTM servive object and returns the pointer.
   *
   * @return
   * - The pointer to \ref rtm::IRtmService "IRtmService": Success.
   * - A null pointer: Failure.
   */
  virtual rtm::IRtmService* createRtmService() = 0;

  virtual int addExtensionObserver(agora::agora_refptr<agora::rtc::IMediaExtensionObserver> observer) = 0;

  virtual int removeExtensionObserver(agora::agora_refptr<agora::rtc::IMediaExtensionObserver> observer) = 0;

  /**
   * Creates an audio device manager and returns the pointer.
   *
   * @return
   * - The pointer to \ref rtc::IAudioDeviceManager "IAudioDeviceManager": Success.
   * - A null pointer: Failure.
   */
  virtual agora_refptr<rtc::IAudioDeviceManager> createAudioDeviceManagerComponent(
      rtc::IAudioDeviceManagerObserver *observer) = 0;
  /**
   * @brief Get the ID of the registered extension
   * 
   * @param provider_name The pointer to provider name string (null-terminated)
   * @param extension_name The pointer to extension name string (null-terminated)
   * @return
   *  - Pointer to the extension id string (null-terminated). The pointer will be valid during service's lifetime
   */
  virtual const char* getExtensionId(const char* provider_name, const char* extension_name) = 0;

#if defined (_WIN32) || defined(__linux__) || defined(__ANDROID__)
  /**
   * @brief load the dynamic library of the extension
   * 
   * @param path path of the extension library
   * @param unload_after_use unload the library when engine release
   * @return int 
   */
  virtual int loadExtensionProvider(const char* path, bool unload_after_use = false) = 0;
#endif
  /**
   * Enable extension.
   * If the extension is enabled, the track loads the extension automatically.
   *
   * @param provider_name name for provider, e.g. agora.io.
   * @param extension_name name for extension, e.g. agora.beauty.
   * @param track_id id for the track, OPTIONAL_NULLPTR means effective on all tracks
   * @param auto_enable_on_track if the extension is automatically open on track.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int enableExtension(
      const char* provider_name, const char* extension_name, const char* track_id = NULL,
      bool auto_enable_on_track = false) = 0;
  /**
   * Disable extension.
   * 
   * @param provider_name name for provider, e.g. agora.io.
   * @param extension_name name for extension, e.g. agora.beauty.
   * @param track_id id for the track, OPTIONAL_NULLPTR means effective on all tracks
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int disableExtension(
      const char* provider_name, const char* extension_name, const char* track_id = NULL) = 0;

 protected:
  virtual ~IAgoraService() {}
};

}  // namespace base
}  // namespace agora

/** \addtogroup createAgoraService
 @{
 */
/**
 * Creates an \ref agora::base::IAgoraService "IAgoraService" object and returns the pointer.
 *
 * @return
 * - The pointer to \ref agora::base::IAgoraService "IAgoraService": Success.
 * - A null pointer: Failure.
 */
AGORA_API agora::base::IAgoraService* AGORA_CALL createAgoraService();
/** @} */
