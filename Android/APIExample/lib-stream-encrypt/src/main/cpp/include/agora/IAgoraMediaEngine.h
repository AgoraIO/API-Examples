//
//  Agora Media SDK
//
//  Copyright (c) 2015 Agora IO. All rights reserved.
//
#pragma once

#include <stdint.h>
#include "IAgoraRtcEngine.h"

namespace agora {
namespace media {
/** 
 * The type of media device.
 */
enum MEDIA_SOURCE_TYPE {
  /** 
   * 0: The audio playback device.
   */
  AUDIO_PLAYOUT_SOURCE = 0,
  /** 
   * 1: Microphone.
   */
  AUDIO_RECORDING_SOURCE = 1,
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
   * - If you register an observer for video raw video data, you cannot register an IVideoEncodedImageReceiver
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
   * - If you register a receiver for encoded video data, you cannot register an IVideoFrameObserver
   * object.
   *
   * @param receiver A pointer to the receiver of the encoded video image: \ref rtc::IVideoEncodedImageReceiver 
   * "IVideoEncodedImageReceiver".
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int registerVideoEncodedImageReceiver(rtc::IVideoEncodedImageReceiver* receiver) = 0;

  /** 
   * Pushes the external audio data to the app.
   *
   * @param type deprecated.
   * @param frame The audio buffer data.
   * @param wrap deprecated.
   * @param sourceId The audio track ID.
   * @param connectionId The channel connection ID.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int pushAudioFrame(MEDIA_SOURCE_TYPE type, IAudioFrameObserver::AudioFrame* frame,
                             bool wrap = false, int sourceId = 0,
                             int connectionId = agora::rtc::DEFAULT_CONNECTION_ID) = 0;

  /**
   * Push the external audio data to the primary audio source.
   *
   * @param frame The audio buffer data.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int pushPrimaryAudioFrame(IAudioFrameObserver::AudioFrame* frame) = 0;

  /**
   * Push the external audio data to the secondary audio source.
   *
   * @param frame The audio buffer data.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int pushSecondaryAudioFrame(IAudioFrameObserver::AudioFrame* frame) = 0;

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
   * @param encodedFrame Determines whether the external video source is encoded.
   * - true: The external video source is encoded.
   * - false: The external video source is not encoded.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setExternalVideoSource(bool enabled, bool useTexture, bool encodedFrame = false) = 0;
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
  virtual int setExternalAudioSource(bool enabled, int sampleRate, int channels, int sourceNumber, bool localPlayback = false, bool publish = true) = 0;

  /** Deprecated by IRtcEngine::setVideoEncoderConfiguration. 
   */
  virtual int setExternalVideoConfigEx(
      const rtc::VideoEncoderConfiguration& config,
      rtc::conn_id_t connectionId = rtc::DEFAULT_CONNECTION_ID) = 0;

  /** 
   * Pushes the external video frame to the app.
   *
   * @param frame The external video frame: ExternalVideoFrame.
   * @param connectionId The connection ID.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int pushVideoFrame(base::ExternalVideoFrame* frame,
                             rtc::conn_id_t connectionId = rtc::DEFAULT_CONNECTION_ID) = 0;

  /** 
   * Pushes the encoded video image to the app.
   * @param imageBuffer A pointer to the video image.
   * @param length The data length.
   * @param videoEncodedFrameInfo The reference to the information of the encoded video frame: 
   * \ref agora::rtc::EncodedVideoFrameInfo "EncodedVideoFrameInfo".
   * @param connectionId The connection ID.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int pushEncodedVideoImage(const uint8_t* imageBuffer, size_t length,
                                    const agora::rtc::EncodedVideoFrameInfo& videoEncodedFrameInfo,
                                    rtc::conn_id_t connectionId = rtc::DEFAULT_CONNECTION_ID) = 0;

  virtual void release() = 0;

 protected:
  virtual ~IMediaEngine() {}
};

}  // namespace media

}  // namespace agora
