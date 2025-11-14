//
//  Agora SDK
//
//  Copyright (c) 2021 Agora.io. All rights reserved.
//

#pragma once  // NOLINT(build/header_guard)

#include "AgoraBase.h"
#include "AgoraRefPtr.h"

namespace agora {
namespace base {
class IAgoraService;
}

namespace rtc {
class ILocalAudioTrack;
class IRtcEngineEventHandler;

/**
 * @brief Virtual metronome state.
 */
enum RHYTHM_PLAYER_STATE_TYPE {
  /**
   * 810: The virtual metronome is not enabled or disabled already.
   */
  RHYTHM_PLAYER_STATE_IDLE = 810,
  /**
   * 811: Opening the beat files.
   */
  RHYTHM_PLAYER_STATE_OPENING,
  /**
   * 812: Decoding the beat files.
   */
  RHYTHM_PLAYER_STATE_DECODING,
  /**
   * 813: The beat files are playing.
   */
  RHYTHM_PLAYER_STATE_PLAYING,
  /**
   * 814: Failed to start virtual metronome. You can use the reported `errorCode` to troubleshoot the
   * cause of the error, or you can try to start the virtual metronome again.
   */
  RHYTHM_PLAYER_STATE_FAILED,
};

/**
 * @brief Virtual Metronome error message.
 */
enum RHYTHM_PLAYER_REASON {
  /**
   * 0: The beat files are played normally without errors.
   */
  RHYTHM_PLAYER_REASON_OK = 0,
  /**
   * 1: A general error; no specific reason.
   */
  RHYTHM_PLAYER_REASON_FAILED = 1,
  /**
   * 801: There is an error when opening the beat files.
   */
  RHYTHM_PLAYER_REASON_CAN_NOT_OPEN = 801,
  /**
   * 802: There is an error when playing the beat files.
   */
  RHYTHM_PLAYER_REASON_CAN_NOT_PLAY,
  /**
   * 803: The duration of the beat file exceeds the limit. The maximum duration is 1.2 seconds.
   */
  RHYTHM_PLAYER_REASON_FILE_OVER_DURATION_LIMIT,
};

/**
 * @brief The metronome configuration.
 */
struct AgoraRhythmPlayerConfig {
  /**
   * The number of beats per measure, which ranges from 1 to 9. The default value is 4, which means
   * that each measure contains one downbeat and three upbeats.
   */
  int beatsPerMeasure;
  /**
   * The beat speed (beats/minute), which ranges from 60 to 360. The default value is 60, which means
   * that the metronome plays 60 beats in one minute.
   */
  int beatsPerMinute;

  AgoraRhythmPlayerConfig() : beatsPerMeasure(4), beatsPerMinute(60) {}
};

/**
 * The IRhythmPlayer class provides access to a rhythm player entity.
 * A rhythm player synthesizes beats, plays them locally, and pushes them remotely.
 */
class IRhythmPlayer : public RefCountInterface {
protected:
  virtual ~IRhythmPlayer() {}
  
public:
  static agora_refptr<IRhythmPlayer> Create();
  virtual int initialize(base::IAgoraService* agora_service, IRtcEngineEventHandler* event_handler, bool is_pass_thru_mode) = 0;
  virtual int playRhythm(const char* sound1, const char* sound2, const AgoraRhythmPlayerConfig& config) = 0;
  virtual int stopRhythm() = 0;
  virtual int configRhythmPlayer(const AgoraRhythmPlayerConfig& config) = 0;
  virtual agora_refptr<ILocalAudioTrack> getRhythmPlayerTrack() = 0;
};

} //namespace rtc
} // namespace agora
