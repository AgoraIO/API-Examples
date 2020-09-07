//
//  Agora Media SDK
//
//  Copyright (c) 2018 Agora IO. All rights reserved.
//
#include "pch.h"
#include "sdk_api.h"
#include <memory>

//#include "api2/internal/agora_service_i.h"
//#include "api2/internal/media_player_source_i.h"
//#include "internal/rtc_engine_i.h"
//#include "main/core/agora_service_impl.h"
//#include "main/core/rtc_globals.h"
//#include "utils/refcountedobject.h"


#if defined(HAS_BUILTIN_EXTENSIONS)
#include "agora_extension_provider.h"
#endif

class EmptyRenderer : public agora::rtc::IVideoRenderer {
 public:
  EmptyRenderer() {}
  ~EmptyRenderer() {}

  //int setRenderMode(agora::media::base::RENDER_MODE_TYPE renderMode) override { return 0; }

  int setMirror(bool mirror) override { return 0; }

  int setView(void* view) override { return 0; }

  int unsetView() override { return 0; }

  //int onFrame(const agora::media::base::VideoFrame& videoFrame) override { return 0; }
};

void LowlevelSdk::Init(const Config& sdk_config) {
  service_ = (agora::base::IAgoraService*)createAgoraService(); //agora::base::AgoraService::Create();// sdk_config.io_engine_strategy);
  agora::base::AgoraServiceConfiguration cfgEx;
  //agora::base::AgoraServiceConfigEx cfgEx;
  cfgEx.enableAudioProcessor = sdk_config.enable_audio_processor;
  cfgEx.enableAudioDevice = sdk_config.enable_audio_device;
  cfgEx.enableVideo = sdk_config.has_video;
  //cfgEx.engineType = sdk_config.engine_type;
  //cfgEx.appId = sdk_config.config_token ? sdk_config.config_token : "";
  int ret = service_->initialize(cfgEx);
  if (ret != 0) {
    service_->release();
    service_ = nullptr;
    return;
  }
#if defined(HAS_BUILTIN_EXTENSIONS)
  auto ex_control = service_->getExtensionControl();
  if (!ex_control) {
    service_ = nullptr;
    return;
  }
  auto provider = agora::rtc::ExtensionProviderBuiltin::Create();
  ex_control->registerExtensionProvider(BUILTIN_EXTENSION_PROVIDER, provider);
#endif

  audio_device_manager_ = service_->createAudioDeviceManager();
  auto factory = service_->createMediaNodeFactory();
  factory_ex_ = static_cast<agora::rtc::IMediaNodeFactory*>(factory.get());
  img_sender_ = nullptr;
  frame_sender_ = nullptr;
  if (sdk_config.scenario != agora::rtc::AUDIO_SCENARIO_DEFAULT) {
    service_->setAudioSessionPreset(sdk_config.scenario);
  }
}

LowlevelSdk::LowlevelSdk(const Config& sdk_config) { Init(sdk_config); }

LowlevelSdk::LowlevelSdk() : LowlevelSdk(true, true, true) {}

LowlevelSdk::LowlevelSdk(bool has_audio) : LowlevelSdk(has_audio, has_audio, true) {}

LowlevelSdk::LowlevelSdk(bool has_audio, bool has_video)
    : LowlevelSdk(has_audio, has_audio, has_video) {}

LowlevelSdk::LowlevelSdk(bool enable_audio_device, bool enable_audio_processor, bool has_video)
   // agora::base::TMediaEngineType engine_type) ,
                       //  agora::utils::IoEngineStrategy io_engine_strategy) {
{
  Config config;
  config.enable_audio_device = enable_audio_device;
  config.enable_audio_processor = enable_audio_processor;
  config.has_video = has_video;
  //config.engine_type = engine_type;
 // config.io_engine_strategy = io_engine_strategy;

  Init(config);
}

LowlevelSdk::~LowlevelSdk() {
  camera_ = nullptr;
  screen_ = nullptr;
  //video_mixer_ = nullptr;
  audio_device_manager_.reset();
  factory_ex_ = nullptr;
  img_sender_ = nullptr;
  img_direct_sender_ = nullptr;
  frame_sender_ = nullptr;
  pcm_sender_ = nullptr;
  audio_frame_sender_ = nullptr;
  audio_frame_mixable_sender_ = nullptr;
  
  cameras_.clear();
  // before service release, destruct above objects because service release will destroy all minor
  // worker, but there are some tasks will put into minor worker in above object's destructor, if
  // service release first, some tasks like release resource will lost.
  if (service_) service_->release();
  service_ = nullptr;
}

/*AsyncConnectionType LowlevelSdk::CreateConnection(const AsyncConnection::Config& config) {
  return std::make_shared<AsyncConnection>(static_cast<agora::base::IAgoraService*>(service_),
                                           config);
}

AsyncConnectionType LowlevelSdk::CreateConnection() {
  AsyncConnection::Config config;
  return CreateConnection(config);
}*/

bool LowlevelSdk::InitCameras()
{
  camera_ = factory_ex_->createCameraCapturer();
  if (!camera_) return false;

  auto device_info = camera_->createDeviceInfo();
  if (!device_info) return false;
  if (device_info->NumberOfDevices() == 0) return false;

  auto name = std::make_unique<char[]>(260);
  auto id = std::make_unique<char[]>(260);
  auto uuid = std::make_unique<char[]>(260);

  int count = device_info->NumberOfDevices();
  cameras_.push_back(camera_);
  cameraInfos_.push_back(device_info);
  for (int i = 1; i < count; i++) {
    agora::agora_refptr<agora::rtc::ICameraCapturer> camera;
    camera = factory_ex_->createCameraCapturer();
    cameras_.push_back(camera);
    auto device_info = cameras_[i]->createDeviceInfo();
    cameraInfos_.push_back(device_info);
  }

  for (int i = 0; i < count; i++) {
      device_info->GetDeviceName(i, name.get(), 260, id.get(), 260, uuid.get(), 260);
      cameras_[i]->initWithDeviceId(id.get());
      agora::rtc::VideoFormat videoFormat;
      device_info->GetCapability(id.get(), 0, videoFormat);
      cameras_[i]->setCaptureFormat(videoFormat);
  }
  return true;
}

void LowlevelSdk::DestroyCameraTracks()
{
	for (int i = 0; i < vecCameraTracks_.size(); i++) {
		vecCameraTracks_[i].reset();
	}
	vecCameraTracks_.clear();
}

void LowlevelSdk::ClearCameras()
{
	for (int i = 0; i < cameras_.size(); i++) {
		cameras_[i].reset();
	}

	cameras_.clear();
	cameraInfos_.clear();
}

bool LowlevelSdk::setCameraConfiguration(int cameraIndex, int formatIndex)
{
    if (cameraIndex >= cameraInfos_.size() || cameraIndex < 0)
        return false;

    auto device_info = cameraInfos_[cameraIndex];
    char deviceId[MAX_DEVICE_ID_LEN] = { 0 };
    char deviceName[MAX_DEVICE_ID_LEN] = { 0 };
    device_info->GetDeviceName(cameraIndex, deviceName, MAX_DEVICE_ID_LEN, deviceId, MAX_DEVICE_ID_LEN);
 
    if (formatIndex >= device_info->NumberOfCapabilities(deviceId) || formatIndex < 0)
        return false;
    agora::rtc::VideoFormat videoFormat;
    device_info->GetCapability(deviceId, formatIndex, videoFormat);
    cameras_[cameraIndex]->setCaptureFormat(videoFormat);
    agora::rtc::VideoEncoderConfiguration encoder_config;
    encoder_config.codecType = agora::rtc::VIDEO_CODEC_H264;
    encoder_config.dimensions.width = videoFormat.width;
    encoder_config.dimensions.height = videoFormat.height;
    encoder_config.frameRate = videoFormat.fps;
    vecCameraTracks_[cameraIndex]->setVideoEncoderConfiguration(encoder_config);
   
    return true;
}

bool LowlevelSdk::CreateCameraTracks( bool large_frame,bool simucast, int gop) {
  agora::rtc::VideoFormat format;
  for (int i = 0; i < cameras_.size(); ++i) {
      agora::rtc::VideoFormat videoFormat = cameras_[i]->getCaptureFormat();
      agora::rtc::VideoEncoderConfiguration encoder_config;
      encoder_config.codecType         = agora::rtc::VIDEO_CODEC_H264;
      encoder_config.dimensions.width  = videoFormat.width;
      encoder_config.dimensions.height = videoFormat.height;
      encoder_config.frameRate         = videoFormat.fps;
      agora::agora_refptr<agora::rtc::ILocalVideoTrack> ret = service_->createCameraVideoTrack(cameras_[i]);
      ret->setVideoEncoderConfiguration(encoder_config);
      //ret->setEnabled(true);
      vecCameraTracks_.push_back(ret);
  }
  return true;
}

agora::rtc::VideoFormat LowlevelSdk::GetCameraVideoFormat(int cameraIndex)
{
    return cameras_[cameraIndex]->getCaptureFormat();
}

/*std::vector< agora::rtc::VideoFormat> LowlevelSdk::GetCameraVideoFormats(int cameraIndex)
{
    auto device_info = cameras_[camera]
    agora::rtc::VideoFormat videoFormat;
    device_info->GetCapability(deviceId, formatIndex, videoFormat);
}*/

agora::agora_refptr<agora::rtc::ILocalVideoTrack> LowlevelSdk::CreateCameraTrack(bool large_frame,
                                                                                 bool simucast,
                                                                                 int gop) {
  camera_ = factory_ex_->createCameraCapturer();
  if (!camera_) return nullptr;

  auto device_info = camera_->createDeviceInfo();
  if (!device_info) return nullptr;
  if (device_info->NumberOfDevices() == 0) return nullptr;

  auto name = std::make_unique<char[]>(260);
  auto id = std::make_unique<char[]>(260);
  auto uuid = std::make_unique<char[]>(260);
  device_info->GetDeviceName(0, name.get(), 260, id.get(), 260, uuid.get(), 260);
  device_info->release();
  camera_->initWithDeviceId(id.get());


  agora::rtc::VideoFormat format;
  format.width = 1280;
  format.height = 720;
  format.fps = 30;

  camera_->setCaptureFormat(format);

  agora::agora_refptr<agora::rtc::ILocalVideoTrack> ret = service_->createCameraVideoTrack(camera_);
  agora::rtc::VideoEncoderConfiguration encoder_config;
  if (large_frame) {
    encoder_config.codecType = agora::rtc::VIDEO_CODEC_H264;
    encoder_config.bitrate = 3 * 1000 * 1000;
    encoder_config.minBitrate = 1 * 1000 * 1000;
    encoder_config.dimensions.width = 1280;
    encoder_config.dimensions.height = 720;
    encoder_config.orientationMode = static_cast<agora::rtc::ORIENTATION_MODE>(0);
  } else {
    encoder_config.codecType = agora::rtc::VIDEO_CODEC_H264;
    encoder_config.bitrate = 100 * 1000;
    encoder_config.minBitrate = 100 * 1000;
    encoder_config.dimensions.width = 320;
    encoder_config.dimensions.height = 240;
    encoder_config.orientationMode = static_cast<agora::rtc::ORIENTATION_MODE>(0);
  }
  encoder_config.frameRate = 30;
  ret->setVideoEncoderConfiguration(encoder_config);

  if (simucast) {
    agora::rtc::SimulcastStreamConfig simucast_config;
    simucast_config.dimensions.width = 240;
    simucast_config.dimensions.height = 180;
    simucast_config.bitrate = 256 * 1000;
    ret->enableSimulcastStream(true, simucast_config);
  }

  return ret;
}

agora::agora_refptr<agora::rtc::IScreenCapturer> LowlevelSdk::CreateScreenCapture()
{
	screen_ = factory_ex_->createScreenCapturer();
	return screen_;
}


agora::agora_refptr<agora::rtc::ILocalVideoTrack> LowlevelSdk::CreateScreenTrack(bool large_frame,
                                                                                 bool simucast,
                                                                                 int gop) {
  if (!screen_) return nullptr;

  //agora::rtc::Rectangle rect;

  //screen_->initWithScreenRect(rect, rect);

  agora::agora_refptr<agora::rtc::ILocalVideoTrack> ret = service_->createScreenVideoTrack(screen_);
  agora::rtc::VideoEncoderConfiguration encoder_config;
 /* if (large_frame) {
    encoder_config.codecType = agora::rtc::VIDEO_CODEC_H264;
    encoder_config.bitrate = 1 * 1000 * 1000;
    encoder_config.minBitrate = 500 * 1000;
    encoder_config.dimensions.width = 2560;
    encoder_config.dimensions.height = 1440;
    encoder_config.frameRate = 15;
  } else {
    encoder_config.codecType = agora::rtc::VIDEO_CODEC_H264;
    encoder_config.bitrate = 100 * 1000;
    encoder_config.minBitrate = 10 * 1000;
    encoder_config.dimensions.width = 320;
    encoder_config.dimensions.height = 240;
    encoder_config.frameRate = 15;
  }
  ret->setVideoEncoderConfiguration(encoder_config);

  if (simucast) {
    agora::rtc::SimulcastStreamConfig simucast_config;
    simucast_config.dimensions.width = 320;
    simucast_config.dimensions.height = 180;
    simucast_config.bitrate = 256 * 1000;
    ret->enableSimulcastStream(true, simucast_config);
  }*/

  return ret;
}

agora::agora_refptr<agora::rtc::ILocalVideoTrack> LowlevelSdk::CreateCustomImageTrack(
    bool syncWithAudioTrac, agora::base::TCcMode ccMode) {
  img_sender_ = factory_ex_->createVideoEncodedImageSender();
  if (!img_sender_) return nullptr;
 // agora::agora_refptr<agora::rtc::ILocalVideoTrack> ret =
 //     service_->createCustomVideoTrack(img_sender_, syncWithAudioTrac, ccMode);
  return nullptr;//ret;
}

agora::agora_refptr<agora::rtc::ILocalVideoTrack> LowlevelSdk::CreateCustomDirectImageTrack() {
  img_direct_sender_ = factory_ex_->createVideoEncodedImageSender();
  if (!img_direct_sender_) return nullptr;
  //agora::agora_refptr<agora::rtc::ILocalVideoTrack> ret =
      //service_->createCustomVideoTrack(img_direct_sender_, false, agora::base::CC_DISABLED);
  return nullptr;// ret;
}

/*agora::agora_refptr<agora::rtc::ILocalVideoTrack> LowlevelSdk::CreateMixedVideoTrack() {
  if (!video_mixer_) {
    video_mixer_ = factory_ex_->createVideoMixer();
  }
  agora::agora_refptr<agora::rtc::ILocalVideoTrack> ret =
      service_->createMixedVideoTrack(video_mixer_);
  agora::rtc::VideoEncoderConfiguration encoder_config;
  encoder_config.codecType = agora::rtc::VIDEO_CODEC_H264;
  encoder_config.bitrate = 4 * 1000 * 1000;
  encoder_config.minBitrate = 500 * 1000;
  encoder_config.dimensions.width = 2560;
  encoder_config.dimensions.height = 1440;
  encoder_config.frameRate = 30;
  ret->setVideoEncoderConfiguration(encoder_config);
  return ret;
}*/

agora::agora_refptr<agora::rtc::ILocalVideoTrack> LowlevelSdk::CreateCustomVideoFrameTrack(
    int fps) {
  frame_sender_ = factory_ex_->createVideoFrameSender();
  if (!frame_sender_) return nullptr;
  agora::agora_refptr<agora::rtc::ILocalVideoTrack> ret =
      service_->createCustomVideoTrack(frame_sender_);
  agora::rtc::VideoEncoderConfiguration encoder_config;
  encoder_config.codecType = agora::rtc::VIDEO_CODEC_H264;
  encoder_config.bitrate = 3 * 1000 * 1000;
  encoder_config.minBitrate = 1 * 1000 * 1000;
  encoder_config.dimensions.width = 1280;
  encoder_config.dimensions.height = 720;
  encoder_config.frameRate = 30;
  ret->setVideoEncoderConfiguration(encoder_config);
  return ret;
}

//agora::agora_refptr<agora::rtc::ILocalVideoTrack> LowlevelSdk::CreateMediaPacketSenderVideoTrack(
//    agora::agora_refptr<agora::rtc::IMediaPacketSender> source) {
//  return service_->createCustomVideoTrack(source);
//}

agora::agora_refptr<agora::rtc::ILocalAudioTrack> LowlevelSdk::CreateCustomPcmTrack() {
  pcm_sender_ = factory_ex_->createAudioPcmDataSender();
  if (!pcm_sender_) return nullptr;
  return service_->createCustomAudioTrack(pcm_sender_);
}

agora::agora_refptr<agora::rtc::ILocalAudioTrack> LowlevelSdk::CreateCustomAudioFrameTrack() {
  audio_frame_sender_ = factory_ex_->createAudioEncodedFrameSender();
  if (!audio_frame_sender_) return nullptr;
  return service_->createCustomAudioTrack(audio_frame_sender_, agora::base::MIX_DISABLED);
}

agora::agora_refptr<agora::rtc::ILocalAudioTrack>
LowlevelSdk::CreateCustomMixableAudioFrameTrack() {
  audio_frame_mixable_sender_ = factory_ex_->createAudioEncodedFrameSender();
  if (!audio_frame_mixable_sender_) return nullptr;
  return service_->createCustomAudioTrack(audio_frame_mixable_sender_, agora::base::MIX_ENABLED);
}

/*agora::agora_refptr<agora::rtc::ILocalAudioTrack> LowlevelSdk::CreateRecordingDeviceLocalTrack() {
  if (!audio_device_manager_) {
    return nullptr;
  }
  //recording_device_source_ = audio_device_manager_->createRecordingDeviceSource(nullptr);
  if (!recording_device_source_) return nullptr;

  return service_->createRecordingDeviceAudioTrack(recording_device_source_);
}*/

agora::agora_refptr<agora::rtc::ILocalAudioTrack> LowlevelSdk::CreateAudioTrack(bool bEnabled) {
  auto local_audio_track = service_->createLocalAudioTrack();
  if (local_audio_track) {
    local_audio_track->setEnabled(bEnabled);
  }
  return local_audio_track;
}

agora::agora_refptr<agora::rtc::ILocalAudioTrack> LowlevelSdk::CreateMediaPlayerSourceTrack(
    agora::agora_refptr<agora::rtc::IMediaPlayerSource> source) {
  return service_->createMediaPlayerAudioTrack(source);
}

/*agora::agora_refptr<agora::rtc::ILocalAudioTrack> LowlevelSdk::CreateMediaPacketSenderAudioTrack(
    agora::agora_refptr<agora::rtc::IMediaPacketSender> source) {
  return service_->createCustomAudioTrack(source);
}*/

agora::agora_refptr<agora::rtc::IVideoFilter> LowlevelSdk::CreateBeautyFilter() {
#if defined(HAS_BUILTIN_EXTENSIONS)
  return factory_ex_->createVideoFilter(BUILTIN_VIDEO_FILTER_BEAUTY);
#else
  return nullptr;
#endif
}

/*agora::agora_refptr<agora::rtc::IVideoFrameAdapter> LowlevelSdk::CreateVideoFrameAdapter() {
#if defined(FEATURE_VIDEO)
  return factory_ex_->createVideoFrameAdapter();
#else
  return nullptr;
#endif
}*/

agora::agora_refptr<agora::rtc::IVideoRenderer> LowlevelSdk::CreateWindowedRender(
    std::string name) {
#if defined(FEATURE_VIDEO)
#if !defined(WEBRTC_IOS) && !defined(WEBRTC_ANDROID) && \
    (!defined(WEBRTC_LINUX) || (defined(WEBRTC_LINUX) && defined(FEATURE_ENABLE_X11)))
  agora::agora_refptr<agora::rtc::IVideoRenderer> ret =
      new agora::RefCountedObject<GlRendererWrapper>(name);
  return ret;
#else
  return nullptr;
#endif
#else
  return nullptr;
#endif  // FEATURE_VIDEO
}

agora::agora_refptr<agora::rtc::IVideoRenderer> LowlevelSdk::CreateWindowedRender(agora::view_t view)
{
    agora::agora_refptr<agora::rtc::IVideoRenderer> renderer = factory_ex_->createVideoRenderer();
    renderer->setView(view);
    return renderer;
}

agora::agora_refptr<agora::rtc::IVideoRenderer> LowlevelSdk::CreateEmptyRender() {
    return nullptr;//new agora::RefCountedObject<EmptyRenderer>();
}

agora::agora_refptr<agora::rtc::IMediaPlayerSource> LowlevelSdk::CreateMediaPlayerSource() {
  return factory_ex_->createMediaPlayerSource();
}

agora::agora_refptr<agora::rtc::IVideoFilter> LowlevelSdk::CreateExtVideoFilter(
    const char* name, const char* vendor) {
  return factory_ex_->createVideoFilter(name, vendor);
}

agora::agora_refptr<agora::rtc::IVideoSinkBase> LowlevelSdk::CreateExtVideoSink(
    const char* name, const char* vendor) {
  return factory_ex_->createVideoSink(name, vendor);
}

agora::agora_refptr<agora::rtc::IAudioFilter> LowlevelSdk::CreateExtAudioFilter(
    const char* name, const char* vendor) {
  return factory_ex_->createAudioFilter(name, vendor);
}

/*agora::agora_refptr<agora::rtc::IMediaPacketSender> LowlevelSdk::CreateMediaPacketSender() {
  return factory_ex_->createMediaPacketSender();
}

void LowlevelSdk::setLogWriter(agora::commons::ILogWriter* log_writer) {
  service_->setLogWriter(log_writer);
}

agora::commons::ILogWriter* LowlevelSdk::releaseLogWriter() { return service_->releaseLogWriter(); }

agora::rtc::ConfigService* LowlevelSdk::ConfigService() { return service_->getConfigService(); }*/
