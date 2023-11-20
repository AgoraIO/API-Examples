//
//  AgoraRtcEngine SDK
//
//  Copyright (c) 2019 Agora.io. All rights reserved.
//

#pragma once

#include "IAgoraSpatialAudioBase.h"
#include "AgoraRefPtr.h"

namespace agora {
namespace rtc {

class IRtcEngineS;
struct RtcConnectionS;

/** The definition of LocalSpatialAudioConfigS
 */
struct LocalSpatialAudioConfigS {
  /*The reference to \ref IRtcEngine, which is the base interface class of the Agora RTC SDK and provides
   * the real-time audio and video communication functionality.
   */
  IRtcEngineS* rtcEngine;

  LocalSpatialAudioConfigS() : rtcEngine(NULL) {}
};

class ILocalSpatialAudioEngineS : virtual public ISpatialAudioEngineBase {
protected:
  virtual ~ILocalSpatialAudioEngineS() {}
  
public:
  /**
   * Initializes the ILocalSpatialAudioEngine object and allocates the internal resources.
   *
   * @note Ensure that you call IRtcEngine::queryInterface and initialize before calling any other ILocalSpatialAudioEngine APIs.
   *
   * @param config The pointer to the LocalSpatialAudioConfig. See #LocalSpatialAudioConfig.
   *
   * @return
   * - 0: Success.
   * - <0: Failure.
   */
  virtual int initialize(const LocalSpatialAudioConfigS& config) = 0;
  /**
   * Updates the position information of remote user. You should call it when remote user whose role is broadcaster moves.
   *
   * @param userAccount The remote user ID. It should be the same as RTC channel remote user id.
   * @param posInfo The position information of remote user. See #RemoteVoicePositionInfo.
   * @return
   * - 0: Success.
   * - <0: Failure.
   */
  virtual int updateRemotePosition(const char* userAccount, const RemoteVoicePositionInfo& posInfo) = 0;
  /**
   * Updates the position of remote user. It's supposed to use with IRtcEngineEx::joinChannelEx.
   *
   * @param userAccount The remote user ID. It should be the same as RTC channel remote user id.
   * @param posInfo The position information of remote user. See #RemoteVoicePositionInfo.
   * @param connection The RTC connection whose spatial audio effect you want to update. See #RtcConnectionS.
   * @return
   * - 0: Success.
   * - <0: Failure.
   */
  virtual int updateRemotePositionEx(const char* userAccount, const RemoteVoicePositionInfo& posInfo, const RtcConnectionS& connection) = 0;
  /**
   * Remove the position information of remote user. You should call it when remote user called IRtcEngine::leaveChannel.
   *
   * @param userAccount The remote user ID. It should be the same as RTC channel remote user id.
   * @return
   * - 0: Success.
   * - <0: Failure.
   */
  virtual int removeRemotePosition(const char* userAccount) = 0;
  /**
   * Remove the position information of remote user. It's supposed to use with IRtcEngineEx::joinChannelEx.
   *
   * @param userAccount The remote user ID. It should be the same as RTC channel remote user id.
   * @param connection The RTC connection whose spatial audio effect you want to update. See #RtcConnectionS.
   * @return
   * - 0: Success.
   * - <0: Failure.
   */
  virtual int removeRemotePositionEx(const char* userAccount, const RtcConnectionS& connection) = 0;
  /**
   * Clear the position informations of remote users. It's supposed to use with IRtcEngineEx::joinChannelEx.
   *
   * @return
   * - 0: Success.
   * - <0: Failure.
   */
  virtual int clearRemotePositionsEx(const RtcConnectionS& connection) = 0;
  /**
   * Updates the position of local user. This method is used in scene with multi RtcConnection.
   *
   * @note
   * - This method is only effective in ILocalSpatialAudioEngine currently.
   *
   * @param position The sound position of the user. The coordinate order is forward, right, and up.
   * @param axisForward The vector in the direction of the forward axis in the coordinate system.
   * @param axisRight The vector in the direction of the right axis in the coordinate system.
   * @param axisUp The vector in the direction of the up axis in the coordinate system.
   * @param connection The RTC connection whose spatial audio effect you want to update.
   * @return
   * - 0: Success.
   * - <0: Failure.
   */
  virtual int updateSelfPositionEx(const float position[3], const float axisForward[3], const float axisRight[3], const float axisUp[3], const RtcConnectionS& connection) = 0;
  /**
   * Mute or unmute remote user audio stream.
   *
   * @param userAccount  The ID of the remote user.
   * @param mute When it's false, SDK will receive remote user audio streams, otherwise SDK will not receive remote user audio streams.
   * @return
   * - 0: Success.
   * - <0: Failure.
   */
  virtual int muteRemoteAudioStream(const char* userAccount, bool mute) = 0;
  
  virtual int setRemoteAudioAttenuation(const char* userAccount, double attenuation, bool forceSet) = 0;
};

}  // namespace rtc
}  // namespace agora
