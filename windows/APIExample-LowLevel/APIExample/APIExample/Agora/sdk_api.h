//
//  Agora RTC/MEDIA SDK
//
//  Created by Ender Zheng in 2019-03.
//  Copyright (c) 2018 Agora.io. All rights reserved.
//
#pragma once

#include "IAgoraMediaPlayerSource.h"
#include "NGIAgoraMediaNodeFactory.h"
#include "IAgoraService.h"
#include "NGIAgoraVideoTrack.h"
#include "NGIAgoraAudioTrack.h"
#include "NGIAgoraAudioDeviceManager.h"
#include "NGIAgoraRtcConnection.h"
#include "NGIAgoraCameraCapturer.h"
#include "NGIAgoraLocalUser.h"
#include "NGIAgoraScreenCapturer.h"
#include "AgoraMediaBase.h"
#include "AgoraBase.h"
#include "AgoraRefPtr.h"

#include <vector>
#define FORCE_VOS_SERVER1 "FIXME_PUT_YOUR_ADDRESS1"
#define FORCE_VOS_SERVER2 "FIXME_PUT_YOUR_ADDRESS2"
#define MAX_DEVICE_ID_LEN 512
namespace agora {
namespace rtc {
class ConfigService;
}  // namespace rtc
}  // namespace agora

class LowlevelSdk {
 public:
  struct Config {
    bool enable_audio_device = true;
    bool enable_audio_processor = true;
    bool has_video = true;
   // agora::base::TMediaEngineType engine_type = agora::base::MEDIA_ENGINE_WEBRTC;
   // agora::utils::IoEngineStrategy io_engine_strategy = agora::utils::IoEngineStrategy::EventFirst;
    agora::rtc::AUDIO_SCENARIO_TYPE scenario = agora::rtc::AUDIO_SCENARIO_DEFAULT;
    const char* config_token = nullptr;
  };

 public:
  LowlevelSdk(const Config& config);
  LowlevelSdk();
  explicit LowlevelSdk(bool has_audio);
  LowlevelSdk(bool has_audio, bool has_video);
  LowlevelSdk(bool enable_audio_device, bool enable_audio_processor, bool has_video);// ,
     // agora::base::TMediaEngineType engine_type = agora::base::MEDIA_ENGINE_WEBRTC ,
              //agora::utils::IoEngineStrategy io_engine_strategy = agora::utils::IoEngineStrategy::EventFirst);

  ~LowlevelSdk();

  agora::base::IAgoraService* AgoraService() const { return service_; }
  agora::agora_refptr<agora::rtc::IMediaNodeFactory> MediaNodeFactory() const { return factory_ex_; }

  //agora::rtc::ConfigService* ConfigService();
  //AsyncConnectionType CreateConnection(const AsyncConnection::Config& config);
  //AsyncConnectionType CreateConnection();

  bool InitCameras();
  void ClearCameras();
  bool CreateCameraTracks(bool large_frame = true,bool simucast = false, int gop = 2000);
  void DestroyCameraTracks();
  agora::agora_refptr<agora::rtc::ILocalVideoTrack> CreateCameraTrack(bool large_frame = true,
                                                                                   bool simucast =  false,
                                                                                   int gop =  2000);

  agora::agora_refptr<agora::rtc::IScreenCapturer> CreateScreenCapture();
  agora::agora_refptr<agora::rtc::ILocalVideoTrack> CreateScreenTrack(bool large_frame = true,
                                                                      bool simucast = false,
                                                                      int gop = 2000);
 
  agora::agora_refptr<agora::rtc::ILocalVideoTrack> CreateCustomImageTrack(bool syncWithAudioTrack = false,
      agora::base::TCcMode ccMode = agora::base::CC_ENABLED);
  //agora::agora_refptr<agora::rtc::ILocalVideoTrack> CreateMixedVideoTrack();
  agora::agora_refptr<agora::rtc::ILocalVideoTrack> CreateCustomDirectImageTrack();
  agora::agora_refptr<agora::rtc::ILocalVideoTrack> CreateCustomVideoFrameTrack(int fps = 30);
 // agora::agora_refptr<agora::rtc::ILocalVideoTrack> CreateMediaPacketSenderVideoTrack(
 //     agora::agora_refptr<agora::rtc::IMediaPacketSender> source);

  //agora::agora_refptr<agora::rtc::ILocalAudioTrack> CreateRecordingDeviceLocalTrack();

  agora::agora_refptr<agora::rtc::ILocalAudioTrack> CreateCustomPcmTrack();
  agora::agora_refptr<agora::rtc::ILocalAudioTrack> CreateCustomAudioFrameTrack();
  agora::agora_refptr<agora::rtc::ILocalAudioTrack> CreateCustomMixableAudioFrameTrack();
  // TODO(tomiao): here the default enabled is true is to align with
  // AgoraService::createLocalAudioTrack(),
  //               may change later
  //agora::agora_refptr<agora::rtc::ILocalAudioTrack> CreateRecordingDeviceLocalTrack();
  agora::agora_refptr<agora::rtc::ILocalAudioTrack> CreateAudioTrack(bool bEnabled = true);

  // For creating local audio track with media source.
  agora::agora_refptr<agora::rtc::ILocalAudioTrack> CreateMediaPlayerSourceTrack(
      agora::agora_refptr<agora::rtc::IMediaPlayerSource> source);
  //agora::agora_refptr<agora::rtc::ILocalAudioTrack> CreateMediaPacketSenderAudioTrack(
  //    agora::agora_refptr<agora::rtc::IMediaPacketSender> source);

  // For creating media node.
  agora::agora_refptr<agora::rtc::IVideoFilter> CreateBeautyFilter();
 // agora::agora_refptr<agora::rtc::IVideoFrameAdapter> CreateVideoFrameAdapter();
  agora::agora_refptr<agora::rtc::IVideoRenderer> CreateWindowedRender(std::string name);
  agora::agora_refptr<agora::rtc::IVideoRenderer> CreateWindowedRender(agora::view_t view);
  agora::agora_refptr<agora::rtc::IVideoRenderer> CreateEmptyRender();
  agora::agora_refptr<agora::rtc::IMediaPlayerSource> CreateMediaPlayerSource();
  agora::agora_refptr<agora::rtc::IVideoFilter> CreateExtVideoFilter(const char* name,
                                                                     const char* vendor);
  agora::agora_refptr<agora::rtc::IVideoSinkBase> CreateExtVideoSink(const char* name,
                                                                     const char* vendor);
  agora::agora_refptr<agora::rtc::IAudioFilter> CreateExtAudioFilter(const char* name,
                                                                     const char* vendor);
 // agora::agora_refptr<agora::rtc::IMediaPacketSender> CreateMediaPacketSender();

//  void setLogWriter(agora::commons::ILogWriter* log_writer);
 // agora::commons::ILogWriter* releaseLogWriter();
  agora::agora_refptr<agora::rtc::IVideoEncodedImageSender> ImageSender() { return img_sender_; }
  agora::agora_refptr<agora::rtc::IVideoEncodedImageSender> ImageDirectSender() {
    return img_direct_sender_;
  }
  agora::agora_refptr<agora::rtc::IVideoFrameSender> VideoFrameSender() { return frame_sender_; }
  agora::agora_refptr<agora::rtc::IAudioPcmDataSender> PcmSender() { return pcm_sender_; }
  agora::agora_refptr<agora::rtc::IAudioEncodedFrameSender> AudioMixableFrameSender() {
    return audio_frame_mixable_sender_;
  }
  agora::agora_refptr<agora::rtc::IAudioEncodedFrameSender> AudioFrameSender() {
    return audio_frame_sender_;
  }

  agora::agora_refptr<agora::rtc::ICameraCapturer> CameraCapture() { return camera_; }

  std::vector<agora::agora_refptr<agora::rtc::ICameraCapturer>> CamerasCapture() { return cameras_; }
  std::vector <agora::rtc::ICameraCapturer::IDeviceInfo*> GetCameraInfos(){ return cameraInfos_; }
  std::vector<agora::agora_refptr<agora::rtc::ILocalVideoTrack>> GetCameraTracks() { return vecCameraTracks_; }
  bool setCameraConfiguration(int cameraIndex, int formatIndex);
  agora::agora_refptr<agora::rtc::IScreenCapturer> ScreenCapture() { return screen_; }
  agora::rtc::VideoFormat GetCameraVideoFormat(int cameraIndex);
  //agora::agora_refptr<agora::rtc::IVideoMixerSource> VideoMixer() { return video_mixer_; }

  //agora::agora_refptr<agora::rtc::IRecordingDeviceSource> RecordingDeviceSource() {
  //  return recording_device_source_;
 // }

 private:
  void Init(const Config& config);

 private:
  agora::base::IAgoraService* service_;
  agora::agora_refptr<agora::rtc::ICameraCapturer> camera_;
  std::vector<agora::agora_refptr<agora::rtc::ICameraCapturer>> cameras_;
  std::vector < agora::rtc::ICameraCapturer::IDeviceInfo*> cameraInfos_;
  std::vector<agora::agora_refptr<agora::rtc::ILocalVideoTrack>> vecCameraTracks_;
  agora::agora_refptr<agora::rtc::IScreenCapturer> screen_;
  //agora::agora_refptr<agora::rtc::IVideoMixerSource> video_mixer_;
  agora::agora_refptr<agora::rtc::IMediaNodeFactory> factory_ex_;//createVideoFrameAdapter
  agora::agora_refptr<agora::rtc::INGAudioDeviceManager> audio_device_manager_;
  agora::agora_refptr<agora::rtc::IVideoEncodedImageSender> img_sender_;
  agora::agora_refptr<agora::rtc::IVideoEncodedImageSender> img_direct_sender_;
  agora::agora_refptr<agora::rtc::IVideoFrameSender> frame_sender_;
  agora::agora_refptr<agora::rtc::IAudioPcmDataSender> pcm_sender_;
  agora::agora_refptr<agora::rtc::IAudioEncodedFrameSender> audio_frame_sender_;
  agora::agora_refptr<agora::rtc::IAudioEncodedFrameSender> audio_frame_mixable_sender_;
  //agora::agora_refptr<agora::rtc::IRecordingDeviceSource> recording_device_source_;
};
