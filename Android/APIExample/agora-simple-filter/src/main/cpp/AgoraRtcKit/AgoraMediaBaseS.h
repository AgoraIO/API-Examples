//
//  Agora Engine SDK
//
//  Created by Sting Feng in 2017-11.
//  Copyright (c) 2017 Agora.io. All rights reserved.

#pragma once  // NOLINT(build/header_guard)

#include "AgoraMediaCommonBase.h"

namespace agora {
namespace rtc {
struct EncodedVideoFrameInfoS;
}
namespace media {
struct UserAudioSpectrumInfoS : public UserAudioSpectrumInfoBase  {
  /**
   * User account of the speaker.
   */
  const char* userAccount;

  UserAudioSpectrumInfoS() : userAccount(NULL) {}
  UserAudioSpectrumInfoS(const char* account, const float* data, int length)
      : UserAudioSpectrumInfoBase(data, length), userAccount(account) {}
};

/**
 * The IAudioSpectrumObserverS class.
 */
class IAudioSpectrumObserverS : virtual public IAudioSpectrumObserverBase {
public:
  virtual ~IAudioSpectrumObserverS() {}
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
  virtual bool onRemoteAudioSpectrum(const UserAudioSpectrumInfoS* spectrums, unsigned int spectrumNumber) {
    (void)spectrums;
    (void)spectrumNumber;
    return true;
  }
};

/**
 * The IVideoFrameObserverS class.
 */
class IVideoFrameObserverS : virtual public IVideoFrameObserverBase {
 public:
  virtual ~IVideoFrameObserverS() {}
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
   * @param remoteUserId ID of the remote user who sends the current video frame.
   * @param videoFrame A pointer to the video frame: VideoFrame
   * @return Determines whether to ignore the current video frame if the post-processing fails:
   * - true: Do not ignore.
   * - false: Ignore, in which case this method does not sent the current video frame to the SDK.
   */
  virtual bool onRenderVideoFrame(const char* channelId, base::user_id_t remoteUserId, VideoFrame& videoFrame) = 0;/* {
     (void)channelId;
     (void)remoteUserId;
     (void)videoFrame;
     return true;
  }*/
};

/**
 * The IVideoEncodedFrameObserverS class.
 */
class IVideoEncodedFrameObserverS {
 public:
  /**
   * Occurs each time the SDK receives an encoded video image.
   * @param uid The user id of remote user.
   * @param imageBuffer The pointer to the video image buffer.
   * @param length The data length of the video image.
   * @param videoEncodedFrameInfo The information of the encoded video frame: EncodedVideoFrameInfoS.
   * @return Determines whether to accept encoded video image.
   * - true: Accept.
   * - false: Do not accept.
   */
  virtual bool onEncodedVideoFrameReceived(const char* userAccount, const uint8_t* imageBuffer, size_t length,
    const rtc::EncodedVideoFrameInfoS& videoEncodedFrameInfo) = 0;/*{
    (void)userAccount;
    (void)imageBuffer;
    (void)length;
    (void)videoEncodedFrameInfoS;
    return true;
  }*/

  virtual ~IVideoEncodedFrameObserverS() {}
};

class IMediaRecorderObserverS {
 public:
  /**
   * Occurs when the recording state changes.
   *
   * When the local audio and video recording state changes, the SDK triggers this callback to report the current
   * recording state and the reason for the change.
   *
   * @param channelId The channel name.
   * @param userId ID of the user.
   * @param state The current recording state. See \ref agora::media::RecorderState "RecorderState".
   * @param error The reason for the state change. See \ref agora::media::RecorderErrorCode "RecorderErrorCode".
   */
  virtual void onRecorderStateChanged(const char* channelId, base::user_id_t userId, RecorderState state, RecorderErrorCode error) {
    (void)channelId;
    (void)userId;
    (void)state;
    (void)error;
  };
  /**
   * Occurs when the recording information is updated.
   *
   * After you successfully register this callback and enable the local audio and video recording, the SDK periodically triggers
   * the `onRecorderInfoUpdated` callback based on the set value of `recorderInfoUpdateInterval`. This callback reports the
   * filename, duration, and size of the current recording file.
   *
   * @param channelId The channel name.
   * @param uid ID of the userId.
   * @param info Information about the recording file. See \ref agora::media::RecorderInfo "RecorderInfo".
   *
   */
  virtual void onRecorderInfoUpdated(const char* channelId, base::user_id_t userId, const RecorderInfo& info) {
    (void)channelId;
    (void)userId;
    (void)info;
  }

  virtual ~IMediaRecorderObserverS() {}
};

}  // namespace media
}  // namespace agora
