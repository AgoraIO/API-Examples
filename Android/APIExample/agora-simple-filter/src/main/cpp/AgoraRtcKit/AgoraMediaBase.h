//
//  Agora Engine SDK
//
//  Created by Sting Feng in 2017-11.
//  Copyright (c) 2017 Agora.io. All rights reserved.

#pragma once  // NOLINT(build/header_guard)

#include "AgoraMediaCommonBase.h"

namespace agora {
namespace rtc {
struct EncodedVideoFrameInfo;
}
namespace media {
struct UserAudioSpectrumInfo : public UserAudioSpectrumInfoBase  {
  /**
   * User ID of the speaker.
   */
  agora::rtc::uid_t uid;

  UserAudioSpectrumInfo() : uid(0) {}
  UserAudioSpectrumInfo(agora::rtc::uid_t uid, const float* data, int length) 
      : UserAudioSpectrumInfoBase(data, length), uid(uid) {}
};

class IAudioSpectrumObserver : virtual public IAudioSpectrumObserverBase {
 public:
  /**
   * Reports the audio spectrum of remote user.
   *
   * This callback reports the IDs and audio spectrum data of the loudest speakers at the moment
   * in the channel.
   *
   * You can set the time interval of this callback using \ref ILocalUser::enableAudioSpectrumMonitor "enableAudioSpectrumMonitor".
   *
   * @param spectrums The pointer to \ref agora::media::UserAudioSpectrumInfo "UserAudioSpectrumInfo", which is an array containing
   * the user ID and audio spectrum data for each speaker.
   * - This array contains the following members:
   *   - `uid`, which is the UID of each remote speaker
   *   - `spectrumData`, which reports the audio spectrum of each remote speaker.
   * @param spectrumNumber The array length of the spectrums.
   * - true: Processed.
   * - false: Not processed.
   */
  virtual bool onRemoteAudioSpectrum(const UserAudioSpectrumInfo* spectrums, unsigned int spectrumNumber) = 0;
};

class IAudioFrameObserver : public IAudioFrameObserverBase {
 public:
  using IAudioFrameObserverBase::onPlaybackAudioFrameBeforeMixing;
  /**
   * Occurs when the before-mixing playback audio frame is received.
   * @param channelId The channel name
   * @param uid ID of the remote user.
   * @param audioFrame The reference to the audio frame: AudioFrame.
   * @return
   * - true: The before-mixing playback audio frame is valid and is encoded and sent.
   * - false: The before-mixing playback audio frame is invalid and is not encoded or sent.
   */
  virtual bool onPlaybackAudioFrameBeforeMixing(const char* channelId, rtc::uid_t uid, AudioFrame& audioFrame) = 0;
};

/**
 * The IVideoFrameObserver class.
 */
class IVideoFrameObserver : virtual public IVideoFrameObserverBase {
 public:
  virtual ~IVideoFrameObserver() {}

  /**
   * Occurs each time the SDK receives a video frame sent by the remote user.
   *
   * After you successfully register the video frame observer, the SDK triggers this callback each time a
   * video frame is received. In this callback, you can get the video data sent by the remote user. You
   * can then post-process the data according to your scenarios.
   *
   * After post-processing, you can send the processed data back to the SDK by setting the `videoFrame`
   * parameter in this callback.
   * 
   * @note This callback does not support sending processed RGBA video data back to the SDK.
   *
   * @param channelId The channel name
   * @param remoteUid ID of the remote user who sends the current video frame.
   * @param videoFrame A pointer to the video frame: VideoFrame
   * @return Determines whether to ignore the current video frame if the post-processing fails:
   * - true: Do not ignore.
   * - false: Ignore, in which case this method does not sent the current video frame to the SDK.
   */
  virtual bool onRenderVideoFrame(const char* channelId, rtc::uid_t remoteUid, VideoFrame& videoFrame) = 0;
};

/**
 * The IVideoEncodedFrameObserver class.
 */
class IVideoEncodedFrameObserver {
 public:
  /**
   * Occurs each time the SDK receives an encoded video image.
   * @param uid The user id of remote user.
   * @param imageBuffer The pointer to the video image buffer.
   * @param length The data length of the video image.
   * @param videoEncodedFrameInfo The information of the encoded video frame: EncodedVideoFrameInfo.
   * @return Determines whether to accept encoded video image.
   * - true: Accept.
   * - false: Do not accept.
   */
  virtual bool onEncodedVideoFrameReceived(rtc::uid_t uid, const uint8_t* imageBuffer, size_t length,
                                           const rtc::EncodedVideoFrameInfo& videoEncodedFrameInfo) = 0;

  virtual ~IVideoEncodedFrameObserver() {}
};

class IMediaRecorderObserver {
 public:
  /**
   * Occurs when the recording state changes.
   *
   * @since v4.0.0
   *
   * When the local audio and video recording state changes, the SDK triggers this callback to report the current
   * recording state and the reason for the change.
   *
   * @param channelId The channel name.
   * @param uid ID of the user.
   * @param state The current recording state. See \ref agora::media::RecorderState "RecorderState".
   * @param error The reason for the state change. See \ref agora::media::RecorderErrorCode "RecorderErrorCode".
   */
  virtual void onRecorderStateChanged(const char* channelId, rtc::uid_t uid, RecorderState state, RecorderErrorCode error) = 0;
  /**
   * Occurs when the recording information is updated.
   *
   * @since v4.0.0
   *
   * After you successfully register this callback and enable the local audio and video recording, the SDK periodically triggers
   * the `onRecorderInfoUpdated` callback based on the set value of `recorderInfoUpdateInterval`. This callback reports the
   * filename, duration, and size of the current recording file.
   *
   * @param channelId The channel name.
   * @param uid ID of the user.
   * @param info Information about the recording file. See \ref agora::media::RecorderInfo "RecorderInfo".
   *
   */
  virtual void onRecorderInfoUpdated(const char* channelId, rtc::uid_t uid, const RecorderInfo& info) = 0;

  virtual ~IMediaRecorderObserver() {}
};

}  // namespace media
}  // namespace agora
