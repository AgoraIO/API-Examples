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

/** dual-mono music output mode
 */
enum AUDIO_MIXING_DUAL_MONO_MODE {
  /* 0: Original mode */
  AUDIO_MIXING_DUAL_MONO_AUTO = 0,
  /* 1: Left channel mode */
  AUDIO_MIXING_DUAL_MONO_L = 1,
  /* 2: Right channel mode */
  AUDIO_MIXING_DUAL_MONO_R = 2,
  /* 3: Mixed channel mode */
  AUDIO_MIXING_DUAL_MONO_MIX = 3
};


/**
 * The IMediaEngine class.
 */
class IMediaEngine {
 public:
  /**
   * Registers an audio frame observer object.
   *
   * @note
   * Ensure that you call this method before \ref IRtcEngine::joinChannel "joinChannel".
   *
   * @param observer A pointer to the audio frame observer object: IAudioFrameObserver,
   * nullptr means unregistering observer instead.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int registerAudioFrameObserver(IAudioFrameObserver* observer) = 0;
  /**
   * Registers a video frame observer object.
   *
   * @note
   * - Ensure that you call this method before joining the channel.
   * - If you register an observer for video raw video data, you cannot register an IVideoEncodedFrameObserver
   * object.
   *
   * @param observer A pointer to the video frame observer: IVideoFrameObserver.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int registerVideoFrameObserver(IVideoFrameObserver* observer) = 0;

  /**
   * Registers a receiver object for the encoded video image.
   *
   * @note
   * - Ensure that you call this method before joining the channel.
   *
   * @param observer A pointer to the observer of the encoded video image: \ref IVideoEncodedFrameObserver
   * "IVideoEncodedFrameObserver".
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int registerVideoEncodedFrameObserver(IVideoEncodedFrameObserver* observer) = 0;

  /**
   * Pushes the external audio data to the app.
   *
   * @param type deprecated.
   * @param frame The audio buffer data.
   * @param wrap deprecated.
   * @param sourceId The audio track ID.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int pushAudioFrame(MEDIA_SOURCE_TYPE type, IAudioFrameObserver::AudioFrame* frame,
                             bool wrap = false, int sourceId = 0) = 0;

  virtual int pushCaptureAudioFrame(IAudioFrameObserver::AudioFrame* frame) = 0;

  virtual int pushReverseAudioFrame(IAudioFrameObserver::AudioFrame* frame) = 0;
  /**
   * @brief Directly push audio frame to the rtc channel without mixing with other sources
   *
   * @param frame The audio data buffer
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int pushDirectAudioFrame(IAudioFrameObserver::AudioFrame* frame) = 0;

  /**
   * Pulls the remote audio data.
   *
   * After a successful method call, the app pulls the decoded and mixed audio data for playback.
   *
   * The difference between this method and the \ref onPlaybackAudioFrame "onPlaybackAudioFrame" is as follows:
   * - `onPlaybackAudioFrame`: The SDK sends the audio data to the app once every 10 ms. Any delay in processing
   * the audio frames may result in audio jitter.
   * - `pullAudioFrame`: The app pulls the remote audio data. After setting the audio data parameters, the
   * SDK adjusts the frame buffer and avoids problems caused by jitter in the external audio playback.
   *
   * @param frame The pointer to the audio frame: AudioFrame.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int pullAudioFrame(IAudioFrameObserver::AudioFrame* frame) = 0;

  /**
   * Sets the external video source.
   *
   * Once the external video source is enabled, the SDK prepares to accept the external video frame.
   *
   * @param enabled Determines whether to enable the external video source.
   * - true: Enable the external video source. Once set, the SDK creates the external source and prepares
   * video data from `pushVideoFrame` or `pushEncodedVideoImage`.
   * - false: Disable the external video source.
   * @param useTexture Determines whether to use textured video data.
   * - true: Use texture, which is not supported now.
   * - False: Do not use texture.
   * @param sourceType Determines the type of external video source frame.
   * - ENCODED_VIDEO_FRAME: The external video source is encoded.
   * - VIDEO_FRAME: The external video source is not encoded.
   * @param encodedVideoOption Video encoded track option, which is only used for ENCODED_VIDEO_FRAME.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setExternalVideoSource(
      bool enabled, bool useTexture, EXTERNAL_VIDEO_SOURCE_TYPE sourceType = VIDEO_FRAME,
      rtc::SenderOptions encodedVideoOption = rtc::SenderOptions()) = 0;

  /**
   * Sets the external audio source.
   *
   * @note
   * Ensure that you call this method before joining the channel.
   *
   * @param enabled Determines whether to enable the external audio source:
   * - true: Enable the external audio source.
   * - false: (default) Disable the external audio source.
   * @param sampleRate The Sample rate (Hz) of the external audio source, which can set be as
   * 8000, 16000, 32000, 44100, or 48000.
   * @param channels The number of channels of the external audio source, which can be set as 1 or 2:
   * - 1: Mono.
   * - 2: Stereo.
   * @param sourceNumber The number of the external audio sources, should be greater than 0.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setExternalAudioSource(bool enabled, int sampleRate, int channels, int sourceNumber = 1, bool localPlayback = false, bool publish = true) = 0;

  /**
   * Sets the external audio sink.
   *
   * This method applies to scenarios where you want to use external audio
   * data for playback. After calling the \ref IRtcEngine::initialize "initialize"
   * method and pass value of false in the `enableAudioDevice` member in the RtcEngineContext struct, you can call
   * the \ref agora::media::IMediaEngine::pullAudioFrame "pullAudioFrame" method to pull the remote audio data, process
   * it, and play it with the audio effects that you want.
   *
   * @note
   * Once you call the \ref IRtcEngine::initialize "initialize" method and pass value of false in the `enableAudioDevice`
   * member in the RtcEngineContext struct, the app will not retrieve any audio data from the
   * \ref agora::media::IAudioFrameObserver::onPlaybackAudioFrame "onPlaybackAudioFrame" callback.
   *
   * @param enabled Sets whether or not to the external audio sink
   * - true: Enables the external audio sink.
   * - false: Disables the external audio sink.
   * @param sampleRate Sets the sample rate (Hz) of the external audio sink, which can be set as 16000, 32000, 44100 or 48000.
   * @param channels Sets the number of audio channels of the external
   * audio sink:
   * - 1: Mono.
   * - 2: Stereo.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setExternalAudioSink(bool enabled, int sampleRate, int channels) = 0;

  /**
   * Sets the external audio source.
   *
   * @note
   * Ensure that you call this method before joining the channel.
   *
   * @param sourceId custom audio source id.
   * @param enabled Determines whether to local playback the external audio source:
   * - true: Local playback the external audio source.
   * - false: Local don`t playback the external audio source.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int enableCustomAudioLocalPlayback(int sourceId, bool enabled) = 0;

  /**
   * @brief Enable/Disable the direct external audio source
   *
   * @param enable Determines whether to enable the direct external audio source
   * @param localPlayback Determines whether to enable the local playback of the direct external audio source
   * @return int
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setDirectExternalAudioSource(bool enable, bool localPlayback = false) = 0;

  /**
   * Pushes the external video frame to the app.
   *
   * @param frame The external video frame: ExternalVideoFrame.
   * @param videoTrackId The id of the video track.
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
  virtual int pushEncodedVideoImage(const uint8_t* imageBuffer, size_t length,
                                    const agora::rtc::EncodedVideoFrameInfo& videoEncodedFrameInfo,
                                    unsigned int videoTrackId = 0) = 0;

  virtual void release() = 0;

 protected:
  virtual ~IMediaEngine() {}
};

}  // namespace media

}  // namespace agora
