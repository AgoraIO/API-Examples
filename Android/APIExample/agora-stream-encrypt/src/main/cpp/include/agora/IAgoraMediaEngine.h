//
//  Agora Media SDK
//
//  Copyright (c) 2015 Agora IO. All rights reserved.
//
#pragma once

#include "AgoraBase.h"
#include "AgoraMediaBase.h"
#include "AgoraRefPtr.h"

namespace agora {
namespace media {

/**
 * @brief The channel mode.
 */
enum AUDIO_MIXING_DUAL_MONO_MODE {
  /**
   * 0: Original mode.
   */
  AUDIO_MIXING_DUAL_MONO_AUTO = 0,
  /**
   * 1: Left channel mode. This mode replaces the audio of the right channel with the audio of the
   * left channel, which means the user can only hear the audio of the left channel.
   */
  AUDIO_MIXING_DUAL_MONO_L = 1,
  /**
   * 2: Right channel mode. This mode replaces the audio of the left channel with the audio of the
   * right channel, which means the user can only hear the audio of the right channel.
   */
  AUDIO_MIXING_DUAL_MONO_R = 2,
  /**
   * 3: Mixed channel mode. This mode mixes the audio of the left channel and the right channel, which
   * means the user can hear the audio of the left channel and the right channel at the same time.
   */
  AUDIO_MIXING_DUAL_MONO_MIX = 3
};


/**
 * The IMediaEngine class.
 */
class IMediaEngine {
 public:
  /**
   * @brief Registers an audio frame observer object.
   *
   * @details
   * Call this method to register an audio frame observer object (register a callback). When you need
   * the SDK to trigger the `onMixedAudioFrame`, `onRecordAudioFrame`, `onPlaybackAudioFrame`,
   * `onPlaybackAudioFrameBeforeMixing` or `onEarMonitoringAudioFrame` callback, you need to use this
   * method to register the callbacks.
   * Call timing: Call this method before joining a channel.
   *
   * @param observer The observer instance. See `IAudioFrameObserver`. Set the value as NULL to
   * release the instance. Agora recommends calling this method after receiving `onLeaveChannel` to
   * release the audio observer object.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int registerAudioFrameObserver(IAudioFrameObserver* observer) = 0;
  /**
   * @brief Registers a raw video frame observer object.
   *
   * @details
   * If you want to observe raw video frames (such as YUV or RGBA format), Agora recommends that you
   * implement one `IVideoFrameObserver` class with this method.
   * When calling this method to register a video observer, you can register callbacks in the
   * `IVideoFrameObserver` class as needed. After you successfully register the video frame observer,
   * the SDK triggers the registered callbacks each time a video frame is received.
   * Applicable scenarios: After registering the raw video observer, you can use the obtained raw
   * video data in various video pre-processing scenarios, such as virtual backgrounds and image
   * enhacement by yourself.
   * Call timing: Call this method before joining a channel.
   *
   * @note
   * When handling the video data returned in the callbacks, pay attention to the changes in the
   * `width` and `height` parameters, which may be adapted under the following circumstances:
   * - When network conditions deteriorate, the video resolution decreases incrementally.
   * - If the user adjusts the video profile, the resolution of the video returned in the callbacks
   * also changes.
   *
   * @param observer The observer instance. See `IVideoFrameObserver`. To release the instance, set
   * the value as NULL.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int registerVideoFrameObserver(IVideoFrameObserver* observer) = 0;
  /**
   * @brief Registers a receiver object for the encoded video image.
   *
   * @details
   * If you only want to observe encoded video frames (such as H.264 format) without decoding and
   * rendering the video, Agora recommends that you implement one `IVideoEncodedFrameObserver` class
   * through this method.
   *
   * @note Call this method before joining a channel.
   *
   * @param observer The video frame observer object. See `IVideoEncodedFrameObserver`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int registerVideoEncodedFrameObserver(IVideoEncodedFrameObserver* observer) = 0;
  
  /**
   * @brief Registers or unregisters a facial information observer.
   *
   * @details
   * You can call this method to register the `onFaceInfo` callback to receive the facial information
   * processed by Agora speech driven extension. When calling this method to register a facial
   * information observer, you can register callbacks in the `IFaceInfoObserver` class as needed.
   * After successfully registering the facial information observer, the SDK triggers the callback you
   * have registered when it captures the facial information converted by the speech driven extension.
   * Applicable scenarios: Facial information processed by the Agora speech driven extension is BS
   * (Blend Shape) data that complies with ARkit standards. You can further process the BS data using
   * third-party 3D rendering engines, such as driving avatar to make mouth movements corresponding to
   * speech.
   *
   * @note
   * - Call this method before joining a channel.
   * - Before calling this method, you need to make sure that the speech driven extension has been
   * enabled by calling `enableExtension`.
   *
   * @param observer Facial information observer, see `IFaceInfoObserver`. If you need to unregister a
   * facial information observer, pass in NULL.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int registerFaceInfoObserver(IFaceInfoObserver* observer) = 0;
  
  /**
   * @brief Pushes the external audio frame.
   *
   * @details
   * Call this method to push external audio frames through the audio track.
   * Call timing: Before calling this method to push external audio data, perform the following
   * steps:1. Call `createCustomAudioTrack` to create a custom audio track and get the audio track ID.
   * 2. Call `joinChannel(const char* token, const char* channelId, uid_t uid, const
   * ChannelMediaOptions& options)` to join the channel. In `ChannelMediaOptions`, set
   * `publishCustomAudioTrackId` to the audio track ID that you want to publish, and set
   * `publishCustomAudioTrack` to `true`.
   *
   * @param frame The external audio frame. See `AudioFrame`.
   * @param trackId The audio track ID. If you want to publish a custom external audio source, set
   * this parameter to the ID of the corresponding custom audio track you want to publish.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int pushAudioFrame(IAudioFrameObserverBase::AudioFrame* frame, rtc::track_id_t trackId = 0) = 0;

  /**
   * @brief Pulls the remote audio data.
   *
   * @details
   * After a successful call of this method, the app pulls the decoded and mixed audio data for
   * playback.
   * Call timing: Call this method after joining a channel.
   * Before calling this method, call `setExternalAudioSink` `(enabled: true)` to notify the app to
   * enable and set the external audio rendering.
   *
   * @note
   * Both this method and the `onPlaybackAudioFrame` callback can be used to get audio data after
   * remote mixing. After calling `setExternalAudioSink` to enable external audio rendering, the app
   * will no longer be able to obtain data from the `onPlaybackAudioFrame` callback. Therefore, you
   * should choose between this method and the `onPlaybackAudioFrame` callback based on your actual
   * business requirements. The specific distinctions between them are as follows:
   * - After calling this method, the app automatically pulls the audio data from the SDK. By setting
   * the audio data parameters, the SDK adjusts the frame buffer to help the app handle latency,
   * effectively avoiding audio playback jitter.
   * - After registering the `onPlaybackAudioFrame` callback, the SDK sends the audio data to the app
   * through the callback. Any delay in processing the audio frames may result in audio jitter.
   * This method is only used for retrieving audio data after remote mixing. If you need to get audio
   * data from different audio processing stages such as capture and playback, you can register the
   * corresponding callbacks by calling `registerAudioFrameObserver`.
   *
   * @param frame Pointers to `AudioFrame`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int pullAudioFrame(IAudioFrameObserverBase::AudioFrame* frame) = 0;

  /**
   * @brief Configures the external video source.
   *
   * @details
   * After calling this method to enable an external video source, you can call `pushVideoFrame` to
   * push external video data to the SDK.
   * Call timing: Call this method before joining a channel.
   *
   * @note Dynamic switching of video sources is not supported within the channel. To switch from an
   * external video source to an internal video source, you must first leave the channel, call this
   * method to disable the external video source, and then rejoin the channel.
   *
   * @param enabled Whether to use the external video source:
   * - `true`: Use the external video source. The SDK prepares to accept the external video frame.
   * - `false`: (Default) Do not use the external video source.
   * @param useTexture Whether to use the external video frame in the Texture format.
   * - `true`: Use the external video frame in the Texture format.
   * - `false`: (Default) Do not use the external video frame in the Texture format.
   * @param sourceType Whether the external video frame is encoded. See `EXTERNAL_VIDEO_SOURCE_TYPE`.
   * @param encodedVideoOption Video encoding options. This parameter needs to be set if `sourceType`
   * is `ENCODED_VIDEO_FRAME`. To set this parameter, contact `technical support`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setExternalVideoSource(
      bool enabled, bool useTexture, EXTERNAL_VIDEO_SOURCE_TYPE sourceType = VIDEO_FRAME,
      rtc::SenderOptions encodedVideoOption = rtc::SenderOptions()) = 0;

#if defined(__ANDROID__)
  /**
   * @brief Sets the EGL context for rendering remote video streams.
   *
   * @details
   * This method can replace the default remote EGL context within the SDK, making it easier to manage
   * the EGL context.
   * When the engine is destroyed, the SDK will automatically release the EGL context.
   * Applicable scenarios: This method is suitable for using a custom video rendering method instead
   * of the default SDK rendering method to render remote video frames in Texture format.
   * Call timing: Call this method before joining a channel.
   *
   * @note This method is for Android only.
   *
   * @param eglContext The EGL context for rendering remote video streams.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setExternalRemoteEglContext(void* eglContext) = 0;
#endif

  /**
   * @brief Sets the external audio source parameters.
   *
   * @deprecated This method is deprecated. Use createCustomAudioTrack(rtc::AUDIO_TRACK_TYPE
   * trackType, const rtc::AudioTrackConfig& config) instead.
   *
   * @details
   * Call timing: Call this method before joining a channel.
   *
   * @param enabled Whether to enable the external audio source:
   * - `true`: Enable the external audio source.
   * - `false`: (Default) Disable the external audio source.
   * @param sampleRate The sample rate (Hz) of the external audio source which can be set as `8000`,
   * `16000`, `32000`, `44100`, or `48000`.
   * @param channels The number of channels of the external audio source, which can be set as `1`
   * (Mono) or `2` (Stereo).
   * @param localPlayback Whether to play the external audio source:
   * - `true`: Play the external audio source.
   * - `false`: (Default) Do not play the external source.
   * @param publish Whether to publish audio to the remote users:
   * - `true`: (Default) Publish audio to the remote users.
   * - `false`: Do not publish audio to the remote users.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setExternalAudioSource(bool enabled, int sampleRate, int channels, bool localPlayback = false, bool publish = true) __deprecated = 0;

  /**
   * @brief Creates a custom audio track.
   *
   * @details
   * To publish a custom audio source, see the following steps:1. Call this method to create a custom
   * audio track and get the audio track ID.
   * 2. Call `joinChannel(const char* token, const char* channelId, uid_t uid, const
   * ChannelMediaOptions& options)` to join the channel. In `ChannelMediaOptions`, set
   * `publishCustomAudioTrackId` to the audio track ID that you want to publish, and set
   * `publishCustomAudioTrack` to `true`.
   * 3. Call `pushAudioFrame` and specify `trackId` as the audio track ID set in step 2. You can then
   * publish the corresponding custom audio source in the channel.
   *
   * @note Call this method before joining a channel.
   *
   * @param trackType The type of the custom audio track. See `AUDIO_TRACK_TYPE`.Attention: If
   * `AUDIO_TRACK_DIRECT` is specified for this parameter, you must set `publishMicrophoneTrack` to
   * `false` in `ChannelMediaOptions` when calling `joinChannel(const char* token, const char*
   * channelId, uid_t uid, const ChannelMediaOptions& options)` to join the channel; otherwise,
   * joining the channel fails and returns the error code -2.
   * @param config The configuration of the custom audio track. See `AudioTrackConfig`.
   *
   * @return
   * - If the method call is successful, the audio track ID is returned as the unique identifier of
   * the audio track.
   * - If the method call fails, 0xffffffff is returned.
   */
  virtual rtc::track_id_t createCustomAudioTrack(rtc::AUDIO_TRACK_TYPE trackType, const rtc::AudioTrackConfig& config) = 0;

  /**
   * @brief Destroys the specified audio track.
   *
   * @param trackId The custom audio track ID returned in `createCustomAudioTrack`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int destroyCustomAudioTrack(rtc::track_id_t trackId) = 0;

  /**
   * @brief Sets the external audio sink.
   *
   * @details
   * After enabling the external audio sink, you can call `pullAudioFrame` to pull remote audio
   * frames. The app can process the remote audio and play it with the audio effects that you want.
   * Applicable scenarios: This method applies to scenarios where you want to use external audio data
   * for playback.
   * Call timing: Call this method before joining a channel.
   *
   * @note Once you enable the external audio sink, the app will not retrieve any audio data from the
   * `onPlaybackAudioFrame` callback.
   *
   * @param enabled Whether to enable or disable the external audio sink:
   * - `true`: Enables the external audio sink.
   * - `false`: (Default) Disables the external audio sink.
   * @param sampleRate The sample rate (Hz) of the external audio sink, which can be set as 16000,
   * 32000, 44100, or 48000.
   * @param channels The number of audio channels of the external audio sink:
   * - 1: Mono.
   * - 2: Stereo.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setExternalAudioSink(bool enabled, int sampleRate, int channels) = 0;

  /**
   * Sets the external audio track.
   *
   * @note
   * Ensure that you call this method before joining the channel.
   *
   * @param trackId The custom audio track id.
   * @param enabled Enable/Disables the local playback of external audio track:
   * - true: Enable local playback
   * - false: Do not enable local playback
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int enableCustomAudioLocalPlayback(rtc::track_id_t trackId, bool enabled) = 0;

  /**
   * @brief Pushes the external raw video frame to the SDK through video tracks.
   *
   * @details
   * To publish a custom video source, see the following steps:1. Call `createCustomVideoTrack` to
   * create a video track and get the video track ID.
   * 2. Call `joinChannel(const char* token, const char* channelId, uid_t uid, const
   * ChannelMediaOptions& options)` to join the channel. In `ChannelMediaOptions`, set
   * `customVideoTrackId` to the video track ID that you want to publish, and set
   * `publishCustomVideoTrack` to `true`.
   * 3. Call this method and specify `videoTrackId` as the video track ID set in step 2. You can then
   * publish the corresponding custom video source in the channel.
   * Applicable scenarios: The SDK supports the ID3D11Texture2D video format since v4.2.3, which is
   * widely used in game scenarios. When you need to push this type of video frame to the SDK, call
   * this method and set the `format` in the `frame` to `VIDEO_TEXTURE_ID3D11TEXTURE2D`, set the
   * `d3d11_texture_2d` and `texture_slice_index` members, and set the format of the video frame to
   * ID3D11Texture2D.
   *
   * @note
   * If you only need to push one custom video source to the channel, you can directly call the
   * `setExternalVideoSource` method and the SDK will automatically create a video track with the
   * `videoTrackId` set to 0.
   * DANGER: After calling this method, even if you stop pushing external video frames to the SDK, the
   * custom video stream will still be counted as the video duration usage and incur charges. Agora
   * recommends that you take appropriate measures based on the actual situation to avoid such video
   * billing.
   * - If you no longer need to capture external video data, you can call `destroyCustomVideoTrack` to
   * destroy the custom video track.
   * - If you only want to use the external video data for local preview and not publish it in the
   * channel, you can call `muteLocalVideoStream` to cancel sending video stream or call
   * `updateChannelMediaOptions` to set `publishCustomVideoTrack` to `false`.
   *
   * @param frame The external raw video frame to be pushed. See `ExternalVideoFrame`.
   * @param videoTrackId The video track ID returned by calling the `createCustomVideoTrack`
   * method.Note: If you only need to push one custom video source, set `videoTrackId` to 0.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int pushVideoFrame(base::ExternalVideoFrame* frame, unsigned int videoTrackId = 0) = 0;
  /**
   * Pushes the encoded video image to the app.
   * @param imageBuffer A pointer to the video image.
   * @param length The data length.
   * @param videoEncodedFrameInfo The reference to the information of the encoded video frame:
   * \ref agora::rtc::EncodedVideoFrameInfo "EncodedVideoFrameInfo".
   * @param videoTrackId The id of the video track.
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int pushEncodedVideoImage(const unsigned char* imageBuffer, size_t length,
                                    const agora::rtc::EncodedVideoFrameInfo& videoEncodedFrameInfo,
                                    unsigned int videoTrackId = 0) = 0;
  /**
   * @hide For internal usage only
   */
  virtual int addVideoFrameRenderer(IVideoFrameObserver *renderer) = 0;

  /**
   * @hide For internal usage only
   */
  virtual int removeVideoFrameRenderer(IVideoFrameObserver *renderer) = 0;

  virtual void release() = 0;

 protected:
  virtual ~IMediaEngine() {}
};

}  // namespace media

}  // namespace agora
