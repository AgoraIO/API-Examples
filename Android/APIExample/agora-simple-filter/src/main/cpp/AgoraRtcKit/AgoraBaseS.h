//
//  Agora Engine SDK
//
//  Created by Sting Feng in 2017-11.
//  Copyright (c) 2017 Agora.io. All rights reserved.
//

// This header file is included by both high level and low level APIs,
#pragma once  // NOLINT(build/header_guard)

#include "AgoraCommonBase.h"
#include "AgoraMediaBaseS.h"

namespace agora {
namespace rtc {
/**
 * The definition of the EncodedVideoFrameInfoS struct, which contains the information of the external encoded video frame.
 */
struct EncodedVideoFrameInfoS : public EncodedVideoFrameInfoBase {
  /**
   * User account of the user that pushes the the external encoded video frame..
   */
  const char* userAccount;

  EncodedVideoFrameInfoS(): userAccount(NULL) {}

  EncodedVideoFrameInfoS& operator=(const EncodedVideoFrameInfoS& rhs) {
    if (this == &rhs) {
      return *this;
    }
    userAccount = rhs.userAccount;
    EncodedVideoFrameInfoBase::operator=(rhs);
    return *this;
  }
};

/**
 * The volume information of users.
 */
struct AudioVolumeInfoS : public AudioVolumeInfoBase {
  /**
   * User account of the speaker.
   * - In the local user's callback, `userAccount` is "".
   * - In the remote users' callback, `userAccount` is the user account of a remote user whose instantaneous
   * volume is one of the three highest.
   */
  const char* userAccount;

  AudioVolumeInfoS() : userAccount(OPTIONAL_NULLPTR) {}
};

/**
 * Transcoding configurations of each host.
 */
struct TranscodingUserS : public TranscodingUserBase {
  /**
   * The user account of the host.
   */
  const char* userAccount;
 
  TranscodingUserS() : userAccount(OPTIONAL_NULLPTR) {}
};

/**
 * Transcoding configurations for Media Push.
 */
struct LiveTranscodingS : public LiveTranscodingBase {
  /** The number of users in the live interactive streaming.
   *  The value range is [0, 17].
   */
  unsigned int userCount;
  /** Manages the user layout configuration in the Media Push. Agora supports a maximum of 17 transcoding users in a Media Push channel. See `TranscodingUserS`.
   */
  TranscodingUserS* transcodingUsers;

  LiveTranscodingS() : userCount(0), transcodingUsers(OPTIONAL_NULLPTR) {}
};

/**
 * The video streams for the video mixing on the local client.
 */
struct TranscodingVideoStreamS : public TranscodingVideoStreamBase {
  /**
   * The ID of the remote user.
   * @note Use this parameter only when the source type of the video for the video mixing on the local client is `VIDEO_SOURCE_REMOTE`.
   */
  const char* remoteUserAccount;

  TranscodingVideoStreamS() : remoteUserAccount(OPTIONAL_NULLPTR) {}
};

/**
 * The configuration of the video mixing on the local client.
 */
struct LocalTranscoderConfigurationS : public LocalTranscoderConfigurationBase {
  /**
   * The number of the video streams for the video mixing on the local client.
   */
  unsigned int streamCount;
  /**
   * The video streams for the video mixing on the local client. See TranscodingVideoStreamS.
   */
  TranscodingVideoStreamS* videoInputStreams;

  LocalTranscoderConfigurationS() : streamCount(0), videoInputStreams(OPTIONAL_NULLPTR) {}
};

/**
 * Attributes of video canvasS object.
 */
struct VideoCanvasS : public VideoCanvasBase {
  /**
   * The user id of local video.
   */
  const char* userAccount;
  /**
   * The user id of video stream in transcoded stream.
   */
  const char* subviewUserAccount;

  VideoCanvasS() : userAccount(""),subviewUserAccount("") {}

  VideoCanvasS(view_t v, media::base::RENDER_MODE_TYPE m, VIDEO_MIRROR_MODE_TYPE mt, const char* u)
      : VideoCanvasBase(v, m, mt), userAccount(u),subviewUserAccount("") {}

  VideoCanvasS(view_t v, media::base::RENDER_MODE_TYPE m, VIDEO_MIRROR_MODE_TYPE mt)
      : VideoCanvasBase(v, m, mt), userAccount(""),subviewUserAccount("") {}

  VideoCanvasS(view_t v, media::base::RENDER_MODE_TYPE m, VIDEO_MIRROR_MODE_TYPE mt, const char* u, const char* subAccount)
      : VideoCanvasBase(v, m, mt), userAccount(u),subviewUserAccount(subAccount){}
};

/** The definition of ChannelMediaInfoS.
 */
struct ChannelMediaInfoS : public ChannelMediaInfoBase {
    /** The user account.
     */
  const char* userAccount;

  ChannelMediaInfoS() {}
  ChannelMediaInfoS(const char* c, const char* t, const char* u) : ChannelMediaInfoBase(c, t), userAccount(u) {}
};

/** The definition of ChannelMediaRelayConfigurationS.
 */
struct ChannelMediaRelayConfigurationS : public ChannelMediaRelayConfigurationBase {
  /** The information of the source channel `ChannelMediaInfoS`. It contains the following members:
   * - `channelName`: The name of the source channel. The default value is `NULL`, which means the SDK applies the name
   * of the current channel.
   * - `uid`: The unique ID to identify the relay stream in the source channel. The default value is 0, which means the
   * SDK generates a random UID. You must set it as 0.
   * - `token`: The token for joining the source channel. It is generated with the `channelName` and `uid` you set in
   * `srcInfo`.
   *   - If you have not enabled the App Certificate, set this parameter as the default value `NULL`, which means the
   * SDK applies the App ID.
   *   - If you have enabled the App Certificate, you must use the token generated with the `channelName` and `uid`, and
   * the `uid` must be set as 0.
   */
  ChannelMediaInfoS* srcInfo;
  /** The information of the destination channel `ChannelMediaInfoS`. It contains the following members:
   * - `channelName`: The name of the destination channel.
   * - `uid`: The unique ID to identify the relay stream in the destination channel. The value
   * ranges from 0 to (2^32-1). To avoid UID conflicts, this `UID` must be different from any
   * other `UID` in the destination channel. The default value is 0, which means the SDK generates
   * a random `UID`. Do not set this parameter as the `UID` of the host in the destination channel,
   * and ensure that this `UID` is different from any other `UID` in the channel.
   * - `token`: The token for joining the destination channel. It is generated with the `channelName`
   * and `uid` you set in `destInfos`.
   *   - If you have not enabled the App Certificate, set this parameter as the default value NULL,
   * which means the SDK applies the App ID.
   * If you have enabled the App Certificate, you must use the token generated with the `channelName`
   * and `uid`.
   */
  ChannelMediaInfoS* destInfos;

  ChannelMediaRelayConfigurationS() : srcInfo(OPTIONAL_NULLPTR), destInfos(OPTIONAL_NULLPTR) {}
};

/**
 * The information about recorded media streams.
 */
struct RecorderStreamInfoS : public RecorderStreamInfoBase {
    /**
     * The User account.
     */
    const char* userAccount;
    RecorderStreamInfoS() : userAccount(NULL) {}
    RecorderStreamInfoS(const char* channelId, const char* userAccount) : RecorderStreamInfoBase(channelId), userAccount(userAccount) {}
};

}  // namespace rtc
}  // namespace agora