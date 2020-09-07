//  Agora RTC/MEDIA SDK
//
//  Created by Pengfei Han in 2019-06.
//  Copyright (c) 2019 Agora.io. All rights reserved.
//
#pragma once

#include <chrono>
#include <vector>
#include <string>

#include "IAgoraService.h"
#include "utils/common_utils.h"

#define CONNECTION_TEST_DEFAULT_CNAME "conn_default_rtsa"

#define AGORA_APP_ID "Your Own APPID"

#define DECLARE_SAMPLE_CASES(type) \
class type : public testing::Test { \
 public: \
  void SetUp() override {} \
  void TearDown() override {} \
};

void dumpByteArray(const char* byteArray, int size);

std::string generateChannelName(int postfix, const char* cname, bool containPidInfo = false);

agora::base::IAgoraService* createAndInitAgoraService(bool enableAudioDevice,
                                                      bool enableAudioProcessor, bool enableVideo);

class AudioFrameObserver;
class AudioPCMFrameHandler;
class AudioPCMPuller;
class ConnectionWrapper;

enum AudioRecvMode : uint8_t {
  AudioRecvAudioDataOnly, // Receive PCM data with puller or observer, or just receive media packet
  AudioRecvPlay, // Receive with ADM for playing or observer. Just supported by RTC SDK.
};

enum AudioDataFetchMode : uint8_t {
  AudioDataFetchPcmPull, // Supported only when AudioRecvMode is AudioRecvAudioDataOnly
  AudioDataFetchPcmObserer, // Supported when AudioRecvMode is AudioRecvAudioDataOnly or AudioRecvPlay
  AudioDataFetchMediaPacket, // Supported when AudioRecvMode is AudioRecvAudioDataOnly or AudioRecvPlay
};

enum AudioDataFetchType : uint8_t {
  AudioDataTypeRecording = 1,
  AudioDataTypePlayback = 1 << 1,
  AudioDataTypePlaybackBeforeMixing = 1 << 2,
  AudioDataTypeMixed = 1 << 3,
  AudioDataTypeNone = 1 << 4,
};

struct AudioDataFetchParams {
  uint32_t sampleRateHz = 48000;
  size_t numberOfChannels = 2;
  bool save_file { false };
  std::string file_saved_path;
};

struct AudioDataObserverParams {
  AudioDataFetchType audio_data_fetch_type { AudioDataTypeNone };
  AudioDataFetchParams audio_data_observer_params[4]; // 0 for recording, 1 for playback, 2 for playback before mixing, 3 for mixed
};

enum VideoRecvMode : uint8_t {
  VideoRecvEncodedFrame, // Receive with puller or observer
  VideoRecvDecodedFrame, // Receive with ADM for playing or observer
};

// enum for clockwise rotation.
enum VideoRotation {
  kVideoRotation_0 = 0,
  kVideoRotation_90 = 90,
  kVideoRotation_180 = 180,
  kVideoRotation_270 = 270
};

struct MediaDataRecvConfig {
  AudioRecvMode audio_recv_mode { AudioRecvAudioDataOnly };
  AudioDataFetchMode audio_data_fetch_mode { AudioDataFetchPcmObserer };
  AudioDataObserverParams audio_data_observer_params;  // Just valid when AudioDataFetchMode is AudioDataFetchPcmObserer
  AudioDataFetchParams audio_data_pull_param;  // Just valid when AudioDataFetchMode is AudioDataFetchPcmPull
  VideoRecvMode video_recv_mode { VideoRecvEncodedFrame };

  int32_t uid { 0 };
  uint64_t duration { 0 };
  bool enable_recorder { false };  // Just supported by RTC SDK.
  int minPort = 0;
  int maxPort = 0;
};

std::shared_ptr<ConnectionWrapper> createRecvConnection(
    agora::base::IAgoraService* service, MediaDataRecvConfig &recv_config, const char* channelId);

std::string genPullFileName(int32_t uid);

std::shared_ptr<AudioPCMPuller> createAudioPcmPuller(agora::rtc::ILocalUser* audioPuller,
    AudioDataFetchParams &params, uint64_t duration);

std::unique_ptr<AudioPCMFrameHandler> createAudioFrameHandler(AudioDataFetchParams params);

std::shared_ptr<AudioFrameObserver> setupAudioFrameObserver(const AudioDataObserverParams &params,
    agora::rtc::ILocalUser* local_user);

