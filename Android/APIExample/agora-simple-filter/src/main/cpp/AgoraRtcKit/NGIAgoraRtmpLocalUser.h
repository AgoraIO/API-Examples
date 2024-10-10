//
//  Agora RTMP connection
//
//  Created by Haonong Yu in 2020-02.
//  Copyright (c) 2020 Agora.io. All rights reserved.
//

#pragma once  // NOLINT(build/header_guard)

#include "NGIAgoraAudioTrack.h"
#include "NGIAgoraVideoTrack.h"

namespace agora {
namespace rtc {

struct RtmpStreamingAudioConfiguration;
struct RtmpStreamingVideoConfiguration;

/**
 * The error occurs while publising audio stream.
 */
enum PublishAudioError {
  /**
   * 0: No error occurs.
   */
  PUBLISH_AUDIO_ERR_OK = 0,

  /**
   * 1: A general error occurs (no specified reason).
   */
  PUBLISH_AUDIO_ERR_FAILED = 1,
};

/**
 * The error occurs while publising video stream.
 */
enum PublishVideoError {
  /**
   * 0: No error occurs.
   */
  PUBLISH_VIDEO_ERR_OK = 0,

  /**
   * 1: A general error occurs (no specified reason).
   */
  PUBLISH_VIDEO_ERR_FAILED = 1,
};

/**
 * The IRtmpLocalUserObserver class.
 */
class IRtmpLocalUserObserver {
 public:
  virtual ~IRtmpLocalUserObserver() {}

  /**
   * Occurs when the first packet of the local audio track is sent, indicating that the local audio track
   * is successfully published.
   *
   * @param audioTrack The pointer to ILocalAudioTrack.
   */
  virtual void onAudioTrackPublishSuccess(agora_refptr<rtc::ILocalAudioTrack> audioTrack) = 0;

  /**
   * @deprecated This method will not be called back
   */
  virtual void onAudioTrackPublishStart(agora_refptr<rtc::ILocalAudioTrack> audioTrack) = 0;

  /**
   * @deprecated This method will not be called back
   */
  virtual void onAudioTrackUnpublished(agora_refptr<rtc::ILocalAudioTrack> audioTrack) = 0;

  /**
   * Occurs when the local audio track fails to be published.
   *
   * @param audioTrack The pointer to ILocalAudioTrack.
   * @param error The error information: #RtmpAudioStreamError.
   */
  virtual void onAudioTrackPublicationFailure(agora_refptr<rtc::ILocalAudioTrack> audioTrack,
                                              PublishAudioError error) = 0;

  /**
   * Occurs when the first packet of the local video track is sent, indicating that the local video track
   * is successfully published.
   * @param videoTrack The pointer to ILocalVideoTrack.
   */
  virtual void onVideoTrackPublishSuccess(agora_refptr<rtc::ILocalVideoTrack> videoTrack) = 0;

  /**
   * @deprecated This method will not be called back
   */
  virtual void onVideoTrackPublishStart(agora_refptr<rtc::ILocalVideoTrack> videoTrack) = 0;

  /**
   * @deprecated This method will not be called back
   */
  virtual void onVideoTrackUnpublished(agora_refptr<rtc::ILocalVideoTrack> videoTrack) = 0;

  /**
   * Occurs when the local video track fails to be published.
   *
   * @param videoTrack The pointer to ILocalVideoTrack.
   * @param error The error information: #RtmpVideoStreamError.
   */
  virtual void onVideoTrackPublicationFailure(agora_refptr<rtc::ILocalVideoTrack> videoTrack,
                                              PublishVideoError error) = 0;
};

/**
 * The IRtmpLocalUser class defines the behavior and state of the RTMP user.
 *
 * Once connected, a RTMP connection has its own user. Apps can get the
 * user object using \ref agora::rtmp::IRtmpConnection::getUser
 * "IRtmpConnection::getUser".
 */
class IRtmpLocalUser {
 public:

  enum VideoBitrateAdjustType {
    None = 0,
    Increasing = 1,   // The buffer is in good declining state to increase bitrate
    Decreasing = 2,   // The buffer is in bad increasing state to decrease bitrate
  };

  virtual ~IRtmpLocalUser() {}

  /**
   * Set the parameters of the audio encoder when pushing the stream
   * 
   * @param config Audio encoder parameters
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setAudioStreamConfiguration(const RtmpStreamingAudioConfiguration& config, aosl_ref_t ares = AOSL_REF_INVALID) = 0;

  /**
   * Set the parameters of the video encoder when pushing the stream
   * 
   * @param config Video encoder parameters
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setVideoStreamConfiguration(const RtmpStreamingVideoConfiguration& config, aosl_ref_t ares = AOSL_REF_INVALID) = 0;

  /**
   * Adjusts the audio volume for publishing.
   *
   * @param volume The volume for publishing. The value ranges between 0 and 100 (default).
   * 
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int adjustRecordingSignalVolume(int volume, aosl_ref_t ares = AOSL_REF_INVALID) = 0;

  /**
   * Gets the current volume for publishing.
   * @param volume A pointer to the publishing volume.
   * 
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int getRecordingSignalVolume(int32_t* volume) = 0;

  /**
   * Dynamically adjust the bit rate parameters of the video encoder in the push stream
   * 
   * @note: When increasing the bit rate, each call increases by 50kbps; 
   *        When you lower the bit rate, you reduce it by 100kbps per call
   * 
   * @param type The type of adjustment mode for the bit-rate parameter of the video encoder in the push stream:
   * - `Increasing`: Increase the video encoding bitrate.
   * - `Decreasing`: Reduce video encoding bitrate
   * 
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int adjustVideoBitrate(VideoBitrateAdjustType type, aosl_ref_t ares = AOSL_REF_INVALID) = 0;

  /**
   * Set whether to enable local video
   * 
   * @param enabled Whether to enable local video:
   * - `true`: Enable local video.
   * - `false`: Disable local video.
   * 
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setVideoEnabled(bool enabled, aosl_ref_t ares = AOSL_REF_INVALID) = 0;

  /**
   * Publishes a local audio track to the RTMP connection.
   *
   * @param audioTrack The local audio track to be published: ILocalAudioTrack.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int publishAudio(agora_refptr<rtc::ILocalAudioTrack> audioTrack, aosl_ref_t ares = AOSL_REF_INVALID) = 0;

  /**
   * Stops publishing the local audio track to the RTMP connection.
   *
   * @param audioTrack The local audio track that you want to stop publishing: ILocalAudioTrack.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int unpublishAudio(agora_refptr<rtc::ILocalAudioTrack> audioTrack, aosl_ref_t ares = AOSL_REF_INVALID) = 0;

  /**
   * Publishes a local video track to the RTMP connection.
   *
   * @param videoTrack The local video track to be published: ILocalVideoTrack.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int publishVideo(agora_refptr<rtc::ILocalVideoTrack> videoTrack, aosl_ref_t ares = AOSL_REF_INVALID) = 0;

  /**
   * Stops publishing the local video track to the RTMP connection.
   *
   * @param videoTrack The local video track that you want to stop publishing: ILocalVideoTrack.
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int unpublishVideo(agora_refptr<rtc::ILocalVideoTrack> videoTrack, aosl_ref_t ares = AOSL_REF_INVALID) = 0;

  /**
   * Registers a RTMP user observer object.
   *
   * You need to implement the IRtmpLocalUserObserver class in this method. Once registered, the
   * IRtmpLocalUserObserver receives events sent by the IRtmpLocalUser object.
   *
   * @param observer The pointer to the IRtmpLocalUserObserver object.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int registerRtmpUserObserver(IRtmpLocalUserObserver* observer, void(*safeDeleter)(IRtmpLocalUserObserver*) = NULL, aosl_ref_t ares = AOSL_REF_INVALID) = 0;

  /**
   * Releases the IRtmpLocalUserObserver object previously registered using registerRtmpUserObserver().
   *
   * @param observer The pointer to the IRtmpLocalUserObserver object that you want to release.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int unregisterRtmpUserObserver(IRtmpLocalUserObserver* observer) = 0;
    /**
     * Registers an audio frame observer object.
     *
     * @param observer A pointer to the audio frame observer object: IAudioFrameObserver.
     *
     * @return
     * - 0: Success.
     * - < 0: Failure.
     */
    virtual int registerAudioFrameObserver(media::IAudioPcmFrameSink* observer, aosl_ref_t ares = AOSL_REF_INVALID) = 0;

    /**
     * Unregisters an audio frame observer object.
     *
     * @param observer A pointer to the audio frame observer object: IAudioFrameObserver.
     */
    virtual void unregisterAudioFrameObserver(media::IAudioPcmFrameSink* observer) = 0;

    /**
     * Registers a video frame observer object.
     *
     * @param observer A pointer to the video frame observer: IVideoFrameObserver.
     *
     * @return
     * - 0: Success.
     * - < 0: Failure.
     */
    virtual int registerVideoFrameObserver(media::base::IVideoFrameObserver* observer, aosl_ref_t ares = AOSL_REF_INVALID) = 0;

    /**
     * Unregisters a video frame observer object.
     *
     * @param observer A pointer to the video frame observer: IVideoFrameObserver.
     */
    virtual void unregisterVideoFrameObserver(media::base::IVideoFrameObserver* observer) = 0;
};

}  // namespace rtc
}  // namespace agora
