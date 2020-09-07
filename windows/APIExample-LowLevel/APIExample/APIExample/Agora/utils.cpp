//  Agora RTC/MEDIA SDK
//
//  Created by Pengfei Han in 2019-06.
//  Copyright (c) 2019 Agora.io. All rights reserved.
//

#include <stdio.h>
//#include <unistd.h>
#include <string>
#include <vector>
#include <sstream>

#include "utils.h"
#include "wrapper/audio_frame_observer.h"
#include "wrapper/audio_pcm_frame_handler.h"
#include "wrapper/audio_pcm_puller.h"
#include "wrapper/connection_wrapper.h"
#include "wrapper/audio_frame_handler_factory.h"

void dumpByteArray(const char* byteArray, int size) {
  int i = 0;
  fprintf(stdout, "\n0x");
  while (i < size) {
    fprintf(stdout, "%02x ", (0xFF & byteArray[i]));
    ++i;
    if (i % 16 == 0) {
      fprintf(stdout, " \n0x");
    } else if (i % 4 == 0) {
      fprintf(stdout, " ");
    }
  }
  fprintf(stdout, "\n");
  fprintf(stdout, " \n");
}

std::string generateChannelName(int postfix, const char* cname, bool containPidInfo) {
  static const int pid = 0;
  char threadname[256] = {0};

  if (postfix == 0) {
    if (containPidInfo) {
      snprintf(threadname, sizeof(threadname), "%s_%05d", cname, pid);
    } else {
      snprintf(threadname, sizeof(threadname), "%s", cname);
    }
  } else {
    if (containPidInfo) {
      snprintf(threadname, sizeof(threadname), "%s_%05d_%04d", cname, pid, postfix);
    } else {
      snprintf(threadname, sizeof(threadname), "%s_%04d", cname, postfix);
    }
  }
  std::string threadName(threadname);
  return threadName;
}

agora::base::IAgoraService* createAndInitAgoraService(bool enableAudioDevice,
                                                      bool enableAudioProcessor, bool enableVideo) {
  auto service = createAgoraService();
  agora::base::AgoraServiceConfiguration scfg;
  scfg.enableAudioProcessor = enableAudioProcessor;
  scfg.enableAudioDevice = enableAudioDevice;
  scfg.enableVideo = enableVideo;

  int ret = service->initialize(scfg);
  return (ret == agora::ERR_OK) ? service : nullptr;
}

std::shared_ptr<ConnectionWrapper> createRecvConnection(agora::base::IAgoraService* service,
                                                        MediaDataRecvConfig &recv_config,
                                                        const char* channelId) {
  ConnectionConfig config;
  config.minPort = recv_config.minPort;
  config.maxPort = recv_config.maxPort;

  if (recv_config.audio_recv_mode == AudioRecvAudioDataOnly
      && recv_config.audio_data_fetch_mode == AudioDataFetchPcmPull) {
    config.numberOfChannels_ = recv_config.audio_data_pull_param.numberOfChannels;
    config.sampleRateHz_ = recv_config.audio_data_pull_param.sampleRateHz;
  } else if (recv_config.audio_data_fetch_mode == AudioDataFetchMediaPacket) {
    config.recv_type = agora::rtc::RECV_PACKET_ONLY;
  }

  config.clientRoleType = agora::rtc::CLIENT_ROLE_BROADCASTER;
  config.subscribeAllAudio = true;
  config.subscribeAllVideo = true;

  if (recv_config.video_recv_mode == VideoRecvEncodedFrame) {
    config.encodedFrameOnly = true;
  }

  auto connection = ConnectionWrapper::CreateConnection(service, config);
  std::ostringstream oss;
  oss << recv_config.uid;
  if (!connection->Connect(AGORA_APP_ID, channelId, oss.str().c_str())) {
    printf("Connect to channel %s failed, tid %ld\n", channelId, gettid());
    return nullptr;
  }
  printf("Connect to channel %s uid %s successfully!\n", channelId, oss.str().c_str());
  return connection;
}

std::string genPullFileName(int32_t uid) {
  char pullerd_audio_pcm_filename[64] = { 0 };
  if (uid == 0) {
    snprintf(pullerd_audio_pcm_filename, sizeof(pullerd_audio_pcm_filename), "%s.wav", "pulled_audio_pcm");
  } else {
    snprintf(pullerd_audio_pcm_filename, sizeof(pullerd_audio_pcm_filename), "%s_%03d.wav",
        "pulled_audio_pcm", uid);
  }
  return pullerd_audio_pcm_filename;
}

std::shared_ptr<AudioPCMPuller> createAudioPcmPuller(agora::rtc::ILocalUser* audioPuller,
    AudioDataFetchParams &params, uint64_t duration) {
  agora::rtc::AudioPcmDataInfo audioFrameInfo;
  audioFrameInfo.sampleCount = params.sampleRateHz / 100;  // 10 ms data
  size_t bytesPerSample = sizeof(int16_t) * params.numberOfChannels;
  auto puller = std::make_shared<AudioPCMPuller>(audioFrameInfo, params.numberOfChannels, bytesPerSample,
      params.sampleRateHz, duration, audioPuller);
  if (params.save_file) {
    std::unique_ptr<AudioPCMFrameFileHandler> handler(
        new AudioPCMFrameFileHandler(params.file_saved_path, params.numberOfChannels, params.sampleRateHz));
    puller->setAudioPcmFrameHandler(std::move(handler));
  } else {
    std::unique_ptr<AudioPCMFrameCheckerHandler> handler(new AudioPCMFrameCheckerHandler);
    puller->setAudioPcmFrameHandler(std::move(handler));
  }
  return puller;
}

std::unique_ptr<AudioPCMFrameHandler> createAudioFrameHandler(AudioDataFetchParams params) {
  size_t numberOfChannels = params.numberOfChannels;
  uint32_t sampleRateHz = params.sampleRateHz;
  bool save_file = params.save_file;

  std::unique_ptr<AudioPCMFrameHandler> handler;
  if (save_file) {
    handler.reset(new AudioPCMFrameFileHandler(params.file_saved_path, numberOfChannels, sampleRateHz));
  } else {
    handler.reset(new AudioPCMFrameCheckerHandler);
  }
  return std::move(handler);
}

std::shared_ptr<AudioFrameObserver> setupAudioFrameObserver(
    const AudioDataObserverParams &params, agora::rtc::ILocalUser* local_user) {
  if (params.audio_data_fetch_type == AudioDataTypeNone) {
    return nullptr;
  }

  std::unique_ptr<AudioPCMFrameHandler> recording_data_handler;
  std::unique_ptr<AudioPCMFrameHandler> playback_data_handler;
  std::unique_ptr<AudioFrameHandlerFactory> user_data_handler_factory;
  std::unique_ptr<AudioPCMFrameHandler> mixed_data_handler;

  #define AUDIO_HANDLER(name, index) \
      name = std::move(createAudioFrameHandler(params.audio_data_observer_params[index]))

  #define AUDIO_HANDLER_FACTORY(name, index)                           \
      name.reset(new AudioFrameHandlerFactory(                         \
          (params.audio_data_observer_params[index].save_file == true) \
          ?                                                            \
          AudioFrameHandlerFactory::HANDLER_TYPE::FILE_HANDLER         \
          :                                                            \
          AudioFrameHandlerFactory::HANDLER_TYPE::CHECKER_HANDLER,     \
          params.audio_data_observer_params[index].file_saved_path,    \
          params.audio_data_observer_params[index].numberOfChannels,   \
          params.audio_data_observer_params[index].sampleRateHz        \
          ));

  #define INITIALIZE_HANDLER(index, TAG, HANDLER_WRAPPER)                                \
  if (params.audio_data_observer_params[index].numberOfChannels != 0 &&                  \
      params.audio_data_observer_params[index].sampleRateHz != 0) {                      \
    int ret = local_user->TAG(params.audio_data_observer_params[index].numberOfChannels, \
        params.audio_data_observer_params[index].sampleRateHz);                          \
    HANDLER_WRAPPER;                                                                     \
  }

  INITIALIZE_HANDLER(
      0, setRecordingAudioFrameParameters, AUDIO_HANDLER(recording_data_handler, 0))
  INITIALIZE_HANDLER(
      1, setPlaybackAudioFrameParameters, AUDIO_HANDLER(playback_data_handler, 1))
  INITIALIZE_HANDLER(
      2, setPlaybackAudioFrameBeforeMixingParameters, AUDIO_HANDLER_FACTORY(user_data_handler_factory, 2))
  INITIALIZE_HANDLER(
      3, setMixedAudioFrameParameters, AUDIO_HANDLER(mixed_data_handler, 3))

  auto observer = std::make_shared<AudioFrameObserver>(
      std::move(playback_data_handler), std::move(recording_data_handler),
      std::move(mixed_data_handler), std::move(user_data_handler_factory));

  local_user->registerAudioFrameObserver(observer.get());

  return observer;
}

