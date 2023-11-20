//
//  Agora Engine SDK
//
//  Created by Sting Feng in 2017-11.
//  Copyright (c) 2017 Agora.io. All rights reserved.
//

// This header file is included by both high level and low level APIs,
#pragma once  // NOLINT(build/header_guard)

#include "AgoraCommonBase.h"
#include "AgoraMediaBase.h"

namespace agora {
namespace rtc {
/**
 * The definition of the EncodedVideoFrameInfo struct, which contains the information of the external encoded video frame.
 */
struct EncodedVideoFrameInfo : public EncodedVideoFrameInfoBase {
  /**
   * ID of the user that pushes the the external encoded video frame..
   */
  uid_t uid;

  EncodedVideoFrameInfo() : uid(0) {}

  EncodedVideoFrameInfo& operator=(const EncodedVideoFrameInfo& rhs) {
    if (this == &rhs) {
      return *this;
    }
    EncodedVideoFrameInfoBase::operator=(rhs);
    uid = rhs.uid;
    return *this;
  }
};

/**
 * The volume information of users.
 */
struct AudioVolumeInfo : public AudioVolumeInfoBase {
  /**
   * User ID of the speaker.
   * - In the local user's callback, `uid` = 0.
   * - In the remote users' callback, `uid` is the user ID of a remote user whose instantaneous
   * volume is one of the three highest.
   */
  uid_t uid;

  AudioVolumeInfo() : uid(0) {}
};

/**
 * Transcoding configurations of each host.
 */
struct TranscodingUser : public TranscodingUserBase {
  /**
   * The user ID of the host.
   */
  uid_t uid;
 
  TranscodingUser() : uid(0) {}
};

/**
 * Transcoding configurations for Media Push.
 */
struct LiveTranscoding : public LiveTranscodingBase {
  /** The number of users in the live interactive streaming.
   *  The value range is [0, 17].
   */
  unsigned int userCount;
  /** Manages the user layout configuration in the Media Push. Agora supports a maximum of 17 transcoding users in a Media Push channel. See `TranscodingUser`.
   */
  TranscodingUser* transcodingUsers;

  LiveTranscoding() : userCount(0), transcodingUsers(OPTIONAL_NULLPTR) {}
};

/**
 * The video streams for the video mixing on the local client.
 */
struct TranscodingVideoStream : public TranscodingVideoStreamBase {
  /**
   * The ID of the remote user.
   * @note Use this parameter only when the source type of the video for the video mixing on the local client is `VIDEO_SOURCE_REMOTE`.
   */
  uid_t remoteUserUid;

  TranscodingVideoStream() : remoteUserUid(0) {}
};

/**
 * The configuration of the video mixing on the local client.
 */
struct LocalTranscoderConfiguration : public LocalTranscoderConfigurationBase {
  /**
   * The number of the video streams for the video mixing on the local client.
   */
  unsigned int streamCount;
  /**
   * The video streams for the video mixing on the local client. See TranscodingVideoStream.
   */
  TranscodingVideoStream* videoInputStreams;

  LocalTranscoderConfiguration() : streamCount(0), videoInputStreams(OPTIONAL_NULLPTR) {}
};

/**
 * Attributes of video canvas object.
 */
struct VideoCanvas : public VideoCanvasBase {
  /**
   * The user id of local video.
   */
  uid_t uid;

  /**
  * The uid of video stream composing the video stream from transcoder which will be drawn on this video canvas. 
  */
  uid_t subviewUid;

  VideoCanvas() : uid(0),subviewUid(0) {}

  VideoCanvas(view_t v, media::base::RENDER_MODE_TYPE m, VIDEO_MIRROR_MODE_TYPE mt, uid_t u)
      : VideoCanvasBase(v, m, mt), uid(u),subviewUid(0) {}

  VideoCanvas(view_t v, media::base::RENDER_MODE_TYPE m, VIDEO_MIRROR_MODE_TYPE mt)
      : VideoCanvasBase(v, m, mt), uid(0),subviewUid(0) {}

  VideoCanvas(view_t v, media::base::RENDER_MODE_TYPE m, VIDEO_MIRROR_MODE_TYPE mt, uid_t u, uid_t subu)
    : VideoCanvasBase(v, m, mt), uid(u),subviewUid(subu) {}
};

/** The definition of ChannelMediaInfo.
 */
struct ChannelMediaInfo : public ChannelMediaInfoBase {
    /** The user ID.
     */
  uid_t uid;

  ChannelMediaInfo() {}
  ChannelMediaInfo(const char* c, const char* t, uid_t u) : ChannelMediaInfoBase(c, t), uid(u) {}
};

/** The definition of ChannelMediaRelayConfiguration.
 */
struct ChannelMediaRelayConfiguration : public ChannelMediaRelayConfigurationBase {
  /** The information of the source channel `ChannelMediaInfo`. It contains the following members:
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
  ChannelMediaInfo* srcInfo;
  /** The information of the destination channel `ChannelMediaInfo`. It contains the following members:
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
  ChannelMediaInfo* destInfos;

  ChannelMediaRelayConfiguration() : srcInfo(OPTIONAL_NULLPTR), destInfos(OPTIONAL_NULLPTR) {}
};

/**
 * The information about recorded media streams.
 */
struct RecorderStreamInfo : public RecorderStreamInfoBase {
    /**
     * The user ID.
     */
    uid_t uid;
    RecorderStreamInfo() : uid(0) {}
    RecorderStreamInfo(const char* channelId, uid_t uid) : RecorderStreamInfoBase(channelId), uid(uid) {}
};


}  // namespace rtc
}  // namespace agora
