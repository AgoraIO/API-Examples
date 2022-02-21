**Note**: Please update this file for every Agora API change you do. Simply fill in
your updates in the Working section below.


Agora Low Level APIs (Working)
==================================================

API (yyyy-mm-dd)
==================================================
Purpose of this change

API file name #1
-------------
**Add:**
Short description
- Foo()
- Bar()

**Modified:**
Short description
- Changes Foo() to Foo1()
- Changes Bar() to Bar1()

API file name #2
=======


API (2022-01-26)
==================================================
NGIAgoraAudioTrack.h
-------------
**Add:**
- IAudioTrack::AudioFilterPosition::PcmSource

API (2022-01-26)
==================================================
NGIAgoraLocalUser.h
-------------
**Add:**
- int int enableSpatialAudio(bool enabled)
- int setRemoteUserSpatialAudioParams(user_id_t userId, const agora::SpatialAudioParams& params)

API (2022-01-24)
==================================================
NGIAgoraMediaNode.h
-------------
**Add:**
Extension video filter support rotation apply
- `bool getRotationApplied()`

API (2022-01-10)
==================================================
NGIAgoraVideoTrack.h
-------------
**Modify:**
- Replace tab with spaces.

NGIAgoraAudioTrack.h
-------------
**Add:**
- int int enableSpatialAudio(bool enabled)
- int setRemoteUserSpatialAudioParams(const agora::SpatialAudioParams& params)

API (2021-12-20)
==================================================
IAgoraService.h
**Add:**
Modify AgoraServiceConfiguration for external egl context
- bool useExternalEglContext


API(2021-12-09)
==================================================
NGIAgoraExtensionControl.h
-------------
**Modify:**
- provider_name -> provider
- ext_name -> extension
- event_key -> key
- event_json_str -> value

API (2021-12-07)
==================================================
NGIAgoraVideoTrack.h
-------------
**Modify:**
- initialize `int regulated_frame_rate`
- initialize `int regulated_width`
- initialize `int regulated_height`


API (2021-11-07)
==================================================
NGIAgoraVideoTrack.h
-------------
**Add:**
Add filed in LocalVideoTrackStats
- `int capture_frame_rate`
- `int regulated_capture_frame_rate`
- `int capture_width`
- `int capture_height`
- `int regulated_capture_width`
- `int regulated_capture_height`

API (2021-11-07)
==================================================
NGIAgoraVideoTrack.h
-------------
**Add:**
- QUALITY_ADAPT_INDICATION quality_adapt_indication 

API (2021-09-30)
==================================================
NGIAgoraAudioTrack.h
-------------
**Add:**
- e2e_delay_ms


API (2021-11-27)
remove enum TCcMode

IAgoraService.h
-------------
**Deleted:**
- enum TCcMode

API (2021-10-18)
==================================================
NGIAgoraRtmpConnection.h
-------------
**Add:**
- enum RTMP_CONNECTION_ERROR::STATE_RECONNECTED
- class IRtmpConnectionObserver:virtual void onReconnected(const RtmpConnectionInfo& connectionInfo) = 0;


API (2021-12-12)
==================================================
IAgoraService.h
-------------
**Modify:**
- nothing


API (2021-10-12)
==================================================
 config_engine_i.h
-------------
**Add:**
- CONFIGURABLE_KEY_RTC_ENABLE_SYNC_RENDER_NTP
API (2021-09-24)
==================================================
add Korea Area Code

config_engine_i.h
-------------
**Add:**
add Korea Area Code

API (2021-11-23)
==================================================
IAgoraService.h
-------------
**Add:**
- createDirectCustomAudioTrack(audioSource)

 API (2021-10-14)
==================================================
- add definition CONFIGURABLE_KEY_VIDEO_VP8_ENC_SWITCH
- add definition CONFIGURABLE_KEY_VIDEO_SVC_TEMPORAL_LAYERS
- add definition CONFIGURABLE_KEY_VIDEO_VP8_ENC_SWITCH
- add definition CONFIGURABLE_KEY_VIDEO_SVC_TEMPORAL_LAYERS

channel_capability_i.h
-------------
**Add:**
- add enum Vp8Feature
- add enum Svc
- add enum CapabilityType:kVp8Feature

 video_track_i.h
-------------
**Add:**
- add variable video_codec_capbits
- add variable curr_temporal_layer

 packet_i.h
-------------
**Add:**
- add variable max_temporal_layers
- add variable codec_type

 video_config_i.h
-------------
**Add:**
- add variable svc_num_temporal_layers
- add variable vp8_enc_switch


API (2021-10-09)
==================================================
Modify param for RtmpStreamingVideoConfiguration

NGIAgoraRtmpConnection.h
-------------
**Modify:**
- modify gop_ms to gopInMs in RtmpStreamingVideoConfiguration


API (2021-10-09)
==================================================
Add param for RtmpStreamingVideoConfiguration

NGIAgoraRtmpConnection.h
-------------
**Add:**
- add push_video_frame_cnt、pop_video_frame_cnt in onTransferStatistics

**Add:**
- add gop_ms in RtmpStreamingVideoConfiguration


API (2021-09-23)
==================================================
NGIAgoraRtcConnection.h
-------------
**Add:**
- AudioSubscriptionOptions& operator=(const AudioSubscriptionOptions& rhs)

API (2021-09-22)
==================================================
NGIAgoraAudioDeviceManager.h
-------------
**Modified:**
- fix marco

API (2021-09-17)
==================================================

NGIAgoraMediaNode.h
-------------
**Modified:**
- Update interface for C++98 standard

API (2021-09-08)
==================================================

AgoraExtensionProviderEntry.h
-------------
**Add:**
- add ProviderInterfaceUsed argument for providers to declare provider version.

AgoraExtensionVersion.h
-------------
**Add:**
- Define extension version and concrete interface versions.

NGIAgoraVideoFrame.h
-------------
**Add:**
- add PaddedRawPixelBuffer data structure for padded memory buffer.

API (2021-09-08)
==================================================

IAgoraService.h
-------------
**Add:**
- add a createCustomAudioTrack() for IAgoraService.

API (2021-09-07)
==================================================
IAgoraService.h
-------------
**Add:**
- Add thread priority option

API (2021-09-03)
==================================================
IAgoraMediaRelayService.h
-------------
**Add:**
- Add pauseAllChannelMediaRelay
- Add resumeAllChannelMediaRelay

API (2021-08-30)
==================================================
AgoraExtensionProviderEntry.h
-------------
**Modified:**
- Use extension provider entry for android extensions too.

extension_counter_event_def.h
-------------
**Add:**
- Add enum LocalExtensionCounter and struct VideoBeautyParam.

API (2021-08-12)
==================================================
Gets the NTP time.

NGIAgoraRtcConnection.h
-------------
**Add:**
- agora::base::NtpTime getNtpTime()

API (2021-09-02)
==================================================
NGIAgoraAudioDeviceManager.h
-------------
**Add:**
- void onAudioDeviceStateChanged(const char *deviceId, int deviceType, int deviceState)

API (2021-08-30)
==================================================
agora_service_i.h
-------------
**Add:**
- Add AGtracer switch

API (2021-08-25)
==================================================
NGIAgoraAudioDeviceManager.h
-------------
**Add:**
- onAudioDeviceVolumeChanged(int deviceType, int volume, bool muted)
API (2021-08-27)
==================================================
Update class IVideoSinkBase

NGIAgoraMediaNode.h
-------------
**Add:**
- bool applyMirror()
- bool applyRotation()

API (2021-08-16)
==================================================
NGIAgoraLocalUser.h
-------------
**Modified:**
- Add callback function onUserStateChanged() and enum REMOTE_USER_STATE.

API (2021-08-12)
==================================================
NGIAgoraLocalUser.h
-------------
**Add:**
- onActiveSpeaker(user_id_t userId).

API (2021-09-11)
==================================================

audio_track_i.h
-------------
**Add:**
- Add IRemoteAudioTrackEx GetAudioLevel

API (2021-08-13)
==================================================
NGIAgoraScreenCapturer.h
-------------
**Add:**
- void setScreenOrientation(VIDEO_ORIENTATION orientation)

API (2021-08-11)
==================================================
NGIAgoraRtcConnection.h
-------------
**Modified:**
Deprecate setEncryptionMode and setEncryptionSecret, correct description of enableEncryption
- setEncryptionMode
- setEncryptionSecret
- enableEncryption

**Add:**
- void onContentInspectResult(media::CONTENT_INSPECT_RESULT result)
**Add:**
- void onSnapshotTaken(const char* channel, uid_t uid, const char* filePath, int width, int height, int errCode)

API (2021-08-06)
==================================================
NGIVideoFrame.h
-------------
**Add:**
- MatrixID and TransferID in agora::rtc::ColorSpace

IAgoraService.h
-------------
**Modified:**
- EnableExtension: fix comment and use track_id instead of id

NGIAgoraExtensionProvider.h
-------------
**Add:**
- setProperty: set provider-wide (key, value) property

API (2021-07-29)
==================================================
IAgoraService.h
-------------
**Add:**
- DisableExtension
**Modified:**
- EnableExtension: use id to identify track and mark if the extension is open automatically on track

API (2021-08-16)
==================================================
NGIAgoraLocalUser.h
-------------
**Modified:**
- onFrame(user_id_t uid, rtc::conn_id_t connectionId, const media::base::VideoFrame* frame) to onFrame(const char* channelId, user_id_t remoteUid, const media::base::VideoFrame* frame)

API (2021-08-05)
==================================================
NGIAgoraLocalUser.h
-------------
**Add:**
- onFirstRemoteVideoFrameRendered()

API (2021-09-10)
==================================================
NGIAgoraMediaNode.h
-------------
**Add:**
- IAudioFilterBase::getPreferredChannelNumbers()
- IAudioFilter::getPreferredChannelNumbers()

API (2021-07-29)
==================================================
AgoraExtensionProviderEntry.h
-------------
**Modified:**
- Register entry function while loading windows extension


API (2021-10-19)
==================================================
NGIAgoraLocalUser.h
-------------
**Add:**
- struct AudioVolumeInformation

**Modified:**
- onAudioVolumeIndication(const AudioVolumeInfo* , ...) to onAudioVolumeIndication(const AudioVolumeInformation* , ...)

API (2021-07-29)
==================================================
IAgoraService.h
-------------
**Add:**
- const char* getExtensionId(const char* provider_name, const char* extension_name)
- int loadExtensionProvider(const char* extension_lib_path)

API (2021-07-27)
==================================================
IAgoraService.h
-------------
**Add:**
- enableExtension(const char* provider_name, const char* extension_name, bool enable)

API (2021-07-22)
==================================================
NGIAgoraLocalUser.h
-------------
**Add:**
- void setAudienceLatencyLevel(AUDIENCE_LATENCY_LEVEL_TYPE level)
- AUDIENCE_LATENCY_LEVEL_TYPE getAudienceLatencyLevel()

API (2021-07-15)
==================================================
IAgoraService.h
-------------
**Add:**
- createCustomAudioTrack(agora_refptr<rtc::IAudioPcmDataSender> audioSource, bool enableAec)

NGIAgoraLocalUser.h
-------------
**Modified:**
- Add comments.

API (2021-07-22)
==================================================
NGIAgoraVideoTrack.h
-------------
**Add:**
hasVideoFilter(const char* id, media::base::VIDEO_MODULE_POSITION position = media::base::POSITION_POST_CAPTURER) = 0;

API (2021-06-23)
==================================================
NGIAgoraExtensionControl.h
-------------
**Add:**
- agora::rtc::IExtensionControl* getAgoraExtensionControl()
- int registerProvider(const char* provider_name, agora_refptr<IExtensionProvider> provider)


API (2021-06-21)
==================================================
NGIAgoraVideoFrame.h
-------------
**Add:**
- ColorSpace in VideoFrameInfo


API (2021-06-25)
==================================================
NGIAgoraAudioTrack.h
-------------
**Add:**
- enableAudioFilter(const char* id, bool enable)
- setFilterProperty(const char* id, const char* key, const char* jsonValue)


API (2021-06-09)
==================================================
NGIAgoraCameraCapturer.h
-------------
**Delete:**
- VIDEO_ORIENTATION getDeviceOrientation() = 0


API (2021-07-19)
==================================================
use string uid in ILocalUserObserver callback

NGIAgoraLocalUser.h
-------------
**Modified:**
- Change onAudioSubscribeStateChanged(const char* channel, uid_t uid , ...) to onAudioSubscribeStateChanged(const char* channel, user_id_t userId , ...)
- Change onVideoSubscribeStateChanged(const char* channel, uid_t uid , ...) to onVideoSubscribeStateChanged(const char* channel, user_id_t userId , ...)


API (2021-06-08)
==================================================

NGIAgoraVideoTrack.h
-------------
**Add:**
- Add playout_audio_frames field for ILocalAudioTrack::LocalAudioTrackStats

API (2021-06-10)
==================================================

NGIAgoraAudioTrack.h
-------------
**Add:**
- Add LocalAudioTrackStats.hw_ear_monitor


API (2021-06-06)
==================================================

NGIAgoraVideoFrame.h
-------------
**Modified:**
- Exchange kGlTextureOes & kGlTexture2D in TextureType


API (2021-05-25)
==================================================
1. add function to update minor stream config

NGIAgoraVideoTrack.h
-------------
**Add:**
- virtual int updateSimulcastStreamConfig(const SimulcastStreamConfig& config) = 0;

API (2020-05-24)
==================================================
Support set audio scenario dynamically.

NGIAgoraLocalUser.h
-------------
**Add:**
- int ILocalUser::setAudioScenario(AUDIO_SCENARIO_TYPE scenario)


API (2021-06-02)
==================================================
 config_engine_i.h
-------------
**Delete:**
- INTERNAL_KEY_RTC_ENABLE_CRYPTO_ACCESS


API (2020-05-10)
==================================================
Default value change for RtmpStreamingVideoConfiguration

NGIAgoraRtmpConnection.h
-------------
**Add:**
- IRtmpConnectionObserver::onTransferStatistics

**Modified:**
 - RtmpStreamingVideoConfiguration

API (2021-05-06)
==================================================
1. Add switches for c++11 features.
2. Change displayId type from view_t to uint32_t.

NGIAgoraScreenCapture.h
-------------
**Modified:**
- int initWithDisplayId(uint32_t displayId, const Rectangle& regionRect)

API (2020-05-05)
==================================================
Support R/W recording audio frame observer.

NGIAgoraLocalUser.h
-------------
**Modified:**
 - setPlaybackAudioFrameParameters()
 - setRecordingAudioFrameParameters()
 - setMixedAudioFrameParameters()

API (2021-05-06)
==================================================
delete onRemoteVideoStreamInfoUpdated.

NGIAgoraLocalUser.h
-------------
**Modified:**
- delete onRemoteVideoStreamInfoUpdated

API (2020-05-31)
==================================================
NGIAgoraCameraCapturer.h
-------------
**Add:**
- bool isCameraFaceDetectSupported()
- int enableFaceDetection(bool enabled)

API (2020-04-30)
==================================================
NGIAgoraCameraCapturer.h
-------------
**Add:**
- bool isCameraAutoExposureFaceModeSupported()
- int setCameraAutoExposureFaceModeEnabled(bool enabled)

API (2020-05-20)
==================================================
NGIAgoraMediaNode.h
-------------
**Add:**
- int getPreferredSampleRate()

API (2021-04-28)
==================================================
NGIAgoraAudioTrack.h
-------------
**Add:**
- RemoteAudioTrackStats::mos_value


API (2021-04-28)
==================================================

video_track_i.h
-------------
**Add:**
- enum class InternalVideoSourceType
- IVideoTrackObserver::onEncoderConfigurationChanged()
- ILocalVideoTrackEx::getInternalVideoSourceType()
- RemoteVideoTrackStatsEx::isHardwareCodec

vide_node_i.h
-------------
**Add:**
- enum InternalRendererType

API (2021-08-10)
==================================================

NGIAgoraLocalUser.h
-------------
**Modified:**
- registerAudioFrameObserver(agora::media::IAudioFrameObserver *) to registerAudioFrameObserver(agora::media::IAudioFrameObserverBase *)
- unregisterAudioFrameObserver(agora::media::IAudioFrameObserver *) to registerAudioFrameObserver(agora::media::IAudioFrameObserverBase *)


API (2021-04-27)
==================================================
Ensure get correct filter composite from specified position.
NGIAgoraAudioTrack.h
------------
**Modified:**
add position parameter
- Changes getAudioFilter(const char *name) to getAudioFilter(const char *name, AudioFilterPosition position)

API (2020-04-22)
====================================================
delete some includeing of unnecessary header files,modify it to forward declaration

NGIAgoraVideoTrack.h
-------------
**Delete:**
- #include "AgoraRefPtr.h"
- #include "NGIAgoraMediaNodeFactory.h"
- #include "IAgoraService.h"

**Add:**
- class IVideoEncodedImageReceiver;
- class IMediaPacketReceiver;
- class IVideoSinkBase;

NGIAgoraVideoFrame.h
-------------
**Delete:**
- #include "AgoraRefPtr.h"


NGIAgoraRtcConnection.h
-------------
**Delete:**
- #include "AgoraRefPtr.h"
- #include "NGIAgoraLocalUser.h"

**Add:**
- class ILocalUser;

NGIAgoraMediaNodeFactory.h
-------------
**Delete:**
- #include "AgoraRefPtr.h"
- #include "IAgoraService.h"
- #include "NGIAgoraMediaNode.h"
- #include "IAgoraMediaPlayerSource.h"
- #include "NGIAgoraCameraCapturer.h"
- #include "NGIAgoraRemoteAudioMixerSource.h"
- #include "NGIAgoraScreenCapturer.h"
- #include "NGIAgoraVideoMixerSource.h"

**Add:**
- class IAudioPcmDataSender;
- class IAudioEncodedFrameSender;
- class IRemoteAudioMixerSource;
- class ICameraCapturer;
- class IScreenCapturer;
- class IVideoMixerSource;
- class IVideoFrameTransceiver;
- class IVideoFrameSender;
- class IVideoRenderer;
- class IVideoFilter;
- class IAudioFilter;
- class IVideoSinkBase;
- class IVideoEncodedImageSender;
- class IMediaPlayerSource;
- class IMediaPacketSender;
- class IMediaStreamingSource;

NGIAgoraLocalUser.h
-------------
**Delete:**
- #include "IAgoraService.h"
- #include "NGIAgoraAudioTrack.h"
- #include "NGIAgoraVideoTrack.h"

**Add:**
- class IMediaControlPacketSender;
- class IMediaControlPacketReceiver;
- struct AudioEncoderConfiguration;
- struct VideoEncoderConfiguration;

- class ILocalAudioTrack;
- struct RemoteAudioTrackStats;
- class IRemoteAudioTrack;
- struct LocalVideoTrackStats;
- struct RemoteVideoTrackStats;
- class IMediaPacketReceiver;
- class IVideoSinkBase;

NGIAgoraAudioTrack.h
-------------
**Delete:**
- #include "AgoraRefPtr.h"
- #include "IAgoraService.h"
- #include "NGIAgoraMediaNodeFactory.h"

**Add:**
- class IAudioFilter;
- class IAudioSinkBase;
- class IMediaPacketReceiver;

IAgoraService.h
-------------
**Delete:**
- #include "NGIAgoraExtensionProvider.h"
- #include "NGIAgoraExtensionProvider.h"

**Add:**
- class IMediaExtensionObserver;
- class IExtensionProvider;

NGIAgoraVideoTrack.h
-------------
**Delete:**
#include "AgoraRefPtr.h"
#include "NGIAgoraMediaNodeFactory.h"
#include "IAgoraService.h"
**Add:**
class IVideoEncodedImageReceiver;
class IMediaPacketReceiver;
class IVideoSinkBase;

NGIAgoraVideoFrame.h
-------------
**Delete:**
- #include "AgoraRefPtr.h"

NGIAgoraRtcConnection.h
-------------
**Delete:**
- #include "AgoraRefPtr.h"
- #include "NGIAgoraLocalUser.h"

**Add:**
- class ILocalUser;

NGIAgoraMediaNodeFactory.h
-------------
**Delete:**
- #include "AgoraRefPtr.h"
- #include "IAgoraService.h"
- #include "NGIAgoraMediaNode.h"
- #include "IAgoraMediaPlayerSource.h"
- #include "NGIAgoraCameraCapturer.h"
- #include "NGIAgoraRemoteAudioMixerSource.h"
- #include "NGIAgoraScreenCapturer.h"
- #include "NGIAgoraVideoMixerSource.h"

**Add:**
- class IAudioPcmDataSender;
- class IAudioEncodedFrameSender;
- class IRemoteAudioMixerSource;
- class ICameraCapturer;
- class IScreenCapturer;
- class IVideoMixerSource;
- class IVideoFrameTransceiver;
- class IVideoFrameSender;
- class IVideoRenderer;
- class IVideoFilter;
- class IAudioFilter;
- class IVideoSinkBase;
- class IVideoEncodedImageSender;
- class IMediaPlayerSource;
- class IMediaPacketSender;
- class IMediaStreamingSource;

NGIAgoraLocalUser.h
-------------
**Delete:**
- #include "IAgoraService.h"
- #include "NGIAgoraAudioTrack.h"
- #include "NGIAgoraVideoTrack.h"

**Add:**
- class IMediaControlPacketSender;
- class IMediaControlPacketReceiver;
- struct AudioEncoderConfiguration;
- struct VideoEncoderConfiguration;
- class ILocalAudioTrack;
- struct RemoteAudioTrackStats;
- class IRemoteAudioTrack;
- struct LocalVideoTrackStats;
- struct RemoteVideoTrackStats;
- class IMediaPacketReceiver;
- class IVideoSinkBase;

NGIAgoraAudioTrack.h
-------------
**Delete:**
- #include "AgoraRefPtr.h"
- #include "IAgoraService.h"
- #include "NGIAgoraMediaNodeFactory.h"

**Add:**
- class IAudioFilter;
- class IAudioSinkBase;
- class IMediaPacketReceiver;

IAgoraService.h
-------------
**Delete:**
- #include "NGIAgoraExtensionProvider.h"
- #include "NGIAgoraExtensionProvider.h"

**Add:**
- class IMediaExtensionObserver;
- class IExtensionProvider;

API (2020-07-13)
==================================================
NGIAgoraLocalUser.h
-------------
**Modified:**
- adjustUserPlaybackSignalVolume(uid_t uid, int volume) to adjustUserPlaybackSignalVolume(user_id_t userId, int volume)
- getUserPlaybackSignalVolume(uid_t uid, int* volume) to getUserPlaybackSignalVolume(user_id_t userId, int* volume)

API (2020-04-25)
==================================================
NGIAgoraLocalUser.h
-------------
**Add:**
- int enableSoundPositionIndication(bool enabled)
- int setRemoteVoicePosition(user_id_t userId, double pan, double gain)

NGIAgoraAudioTrack.h
-------------
**Add:**
- int setRemoteVoicePosition(float pan, float gain)

API (2020-04-06)
==================================================
NGIAgoraLocalUser.h
-------------
**Add:**
- Add new interface IVideoFrameObserver2
- registerVideoEncodedFrameObserver()
- unregisterVideoEncodedFrameObserver()
- registerVideoFrameObserver()
- unregisterVideoFrameObserver()

NGIAgoraLocalUser.h
-------------
**Add:**
- Add new interface IVideoFrameObserver2
- registerVideoEncodedFrameObserver()
- unregisterVideoEncodedFrameObserver()
- registerVideoFrameObserver()
- unregisterVideoFrameObserver()

API (2021-03-31)
==================================================
Add media streaming source module

IAgoraService.h
-------------
**Add:**
- IAgoraService::createMediaStreamingVideoTrack
- IAgoraService::createMediaStreamingAudioTrack

NGIAgoraMediaNodeFactory.h
-------------
**Add:**
- IMediaNodeFactory::createMediaStreamingSource

API (2020-04-06)
==================================================
NGIAgoraCameraCapturer.h
-------------
**Add:**
- ICameraCapturer::isCameraTorchSupported()
- ICameraCapturer::setCameraTorchOn(bool isOn)
- ICameraCapturer::isCameraExposurePositionSupported()
- ICameraCapturer::setCameraExposurePosition(float positionXinView, float positionYinView)
- ICameraCaptureObserver::onCameraExposureAreaChanged(int x, int y, int width, int height)

API (2020-03-26)
==================================================
Refine IVideoFrame interfaces

NGIVideoFrame.h
-------------
**Modified:**
- Refine interfaces to support mempixelbuffer, texture and cvpixelbuffer

API (2020-03-26)
==================================================
Move 'areaCode' field from 'AgoraServiceConfigurationEx' to 'AgoraServiceConfiguration'

IAgoraService.h
-------------
**Modified:**
- IAgoraService::AgoraServiceConfiguration


API (2021-04-12)
==================================================
Add parameter key and modify broadcast_packet_t.

config_engine_i.h
-------------
**Add:**
Add parameter key
- INTERNAL_KEY_RTC_JOIN_VOS_TIMEOUT

config_engine_i.h
-------------
**Modified:**
Modify broadcast_packet_t.
- add [bool need_reliable; / bool real_quit;] to broadcast_packet_t.


API (2021-03-23)
==================================================
IAgoraService.h
rtc_engine_i.h
IAgoraRtcEngine.h
IAgoraRtcEngineEx.h
-------------

API (2021-02-23)
==================================================
IAgoraService.h
-------------

AgoraServiceConfiguration
-------------
**Add:**
channelProfile

API (2021-01-18)
==================================================
IAgoraService.h
NGIAgoraExtensionProvider.h
NGIAgoraMediaNode.h
NGIAgoraRtcConnection.h
NGIAgoraVideoTrack.h
-------------

API (2021-02-05)
==================================================
NGIAgoraAudioTrack.h
-------------
**Modify:**
Modify Struct RemoteAudioTrackStats:
- Initial audio_level in constructor

API (2021-03-25)
==================================================
NGIAgoraAudioTrack.h
-------------
**Modified:**
- enableEarMonitor(bool enable, bool includeAudioFilter) to enableEarMonitor(bool enable, int includeAudioFilters)

**Add:**
- add AudioFilterPosition.RecordingLocalPlayback
- add AudioFilterPosition.PostAudioRecordingDevice
- add AudioFilterPosition.PostAudioProcessing

API (2020-03-25)
==================================================
Add "Safe Deleter" in Register*Observer function argument list

**Modified:**
IAgoraMediaRelayService.h
NGIAgoraAudioDeviceManager.h
NGIAgoraLocalUser.h
NGIAgoraRtcConnection.h
NGIAgoraRtmpConnection.h
NGIAgoraRtmpLocalUser.h

API (2021-03-25)
==================================================

NGIAgoraAudioTrack.h
-------------
**Add:**
- RemoteAudioTrackStats::delay_estimate_ms

NGIAgoraLocalUser.h
-------------
**Add:**
- ILocalUserObserver::onStreamMessage()

NGIAgoraRtcConnection.h
-------------
**Modified:**
- IRtcConnection::createDataStream()
- IRtcConnectionObserver::onStreamMessage()

API (2020-03-23)
==================================================
Loopback recording.

IAgoraService.h
-------------
**Modified:**
- Add a parameter for IAgoraService::createRecordingDeviceAudioTrack()

API (2020-03-19)
==================================================
modify to adapt c++ 11

API (2020-03-17)
==================================================
Add audio and video downlink and uplink process time stats field

NGIAgoraLocalUser.h
-------------
**Add:**
- LocalAudioDetailedStats::uplink_process_time_ms

NGIAgoraAudioTrack.h
-------------
**Add:**
- RemoteAudioTrackStats::downlink_process_time_ms

NGIAgoraVideoTrack.h
-------------
**Add:**
- LocalVideoTrackStats::uplink_cost_time_ms
- RemoteVideoTrackStats::downlink_process_time_ms
- RemoteVideoTrackStats::frame_render_delay_ms;


API (2021-03-16)
==================================================
AgoraRefCountedObject.h
-------------
**Modified:**
- Modify include path


API (2020-03-10)
==================================================
Add remote video A/V sync offset

IAgoraRtcEngine.h
-------------
**Modified:**
Add `avSyncTimeMs` field for struct `RemoteVideoStats`


API (2021-03-18)
==================================================
Modify RtcConnectionConfigurationEx.

rtc_connection_i.h
-------------
**Modified:**
- Changes RtcConnectionConfigurationEx::RtcConnectionConfigurationEx implement

video_track_i.h
-------------
**Modified:**
- Changes AttachInfo::recv_type to AttachInfo::recv_media_packet

NGIAgoraRtcConnection.h
-------------
**Modified:**
- Changes RtcConnectionConfiguration::recvType to RtcConnectionConfiguration::audio/videoRecvMediaPacket

NGIAgoraRtcConnection.h
-------------
**Deleted:**
- Deleted enum RECV_TYPE


API (2021-02-22)
==================================================
Support for mixed remote audio track

NGIAgoraRemoteAudioMixerSource.h
-------------
**Add:**
- class IRemoteAudioMixerSource


API (2021-01-12)
==================================================
Change definition of bytes_per_sample without channel

NGIAgoraMediaNode.h
-------------
**Modified:**
- change param type of sendAudioPcmData#bytes_per_sample from size_t to enum BYTES_PER_SAMPLE

NGIAgoraRtcConnection.h
-------------
**Modified:**
- change param type of AudioSubscriptionOptions#bytes_per_sample from size_t to enum BYTES_PER_SAMPLE

--------------
API (2020-10-23)
===================================================
Add/Remove/Modify methods to support extension management,
which includes adding/removing extension,
enabling/disabling extension, setting extension property.

rtc_engine_i.h
-------------
**Add:**
- struct Extension
- RtcEngineContextEx::extensions
- RtcEngineContextEx::numExtension

IAgoraRtcEngine.h
-------------
**Add:**
- IRtcEngine::enableExtension
- IRtcEngine::setExtensionProperty

**Remove:**
- IRtcEngine::enableLocalVideoFilter
- IRtcEngine::setLocalVideoFilterProperty
- IRtcEngine::getLocalVideoFilterProperty
- IRtcEngine::enableRemoteVideoFilter
- IRtcEngine::setRemoteVideoFilterProperty
- IRtcEngine::getRemoteVideoFilterProperty

IAgoraService.h
-------------
**Add:**
- IAgoraService::createCustomAudioTrack

NGIAgoraMediaNodeFactory.h
-------------
**Add:**
- IMediaNodeFactory::createRemoteAudioMixerSource

API (2021-03-24)
==================================================
NGIAgoraAudioTrack.h
-------------
**Add:**
- RemoteAudioTrackStats::frozen_count_80_ms
- RemoteAudioTrackStats::frozen_time_80_ms
- RemoteAudioTrackStats::frozen_count_200_ms
- RemoteAudioTrackStats::frozen_time_200_ms

API (2021-02-08)
==================================================
NGIAgoraMediaNodeFactory.h
NGIAgoraRtcConnection.h
-------------
Fix "warning: unused parameter"

API (2021-02-05)
==================================================
Add two interface file NGIAgoraRtmpConnection.h and NGIAgoraRtmpLocalUser.h.

IAgoraService.h
-------------
**Add:**
- createRtmpConnection()


API (2021-02-18)
==================================================
Add key INTERNAL_KEY_RTC_DESENSITISE_IP.

config_engine_i.h
-------------
**Add:**
- INTERNAL_KEY_RTC_DESENSITISE_IP


API (2021-02-05)
==================================================
NGIAgoraAudioTrack.h
-------------
**Modify:**
Modify Struct RemoteAudioTrackStats:
- Add int32_t audio_level

API (2020-11-27)
==================================================
NGIAgoraLocalUser.h
-------------
**Modified:**
- changed agora::base::Optional to agora::Optional

API (2020-12-2)
==================================================

NGIAgoraRemoteAudioMixerSource.h
-------------
**Deleted:**
- Deleted IRemoteAudioMixerSource::startMixing()
- Deleted IRemoteAudioMixerSource::stopMixing()

API (2021-02-02)
==================================================
Modify INetworkObserver.

NGIAgoraRtcConnection.h
-------------
**Modified:**
Add more callback to INetworkObserver
- Changes add onDownlinkNetworkInfoUpdated


API (2021-01-03)
==================================================
IAgoraService.h
-------------
**Modify:**
Modify Struct AgoraServiceConfiguration:
- Remove const char* logDir;
- Add commons::LogConfig logConfig;

**Deleted:**
Short description
- Deleted Foo()


API (2021-02-05)
==================================================
IAgoraRtmService.h
-------------
**Add:**
- getRawMessageData()
- getRawMessageLength()
- setMessageType()
- setRawMessage()


API (2021-01-05)
==================================================
NGIAgoraLocalUser.h
-------------
**Add:**
- onAudioSubscribeStateChanged()
- onVideoSubscribeStateChanged()
- onAudioPublishStateChanged()
- onVideoPublishStateChanged()


API (2021-01-19)
==================================================
Remove INTERNAL_KEY_RTC_CONNECTION_LOST_PERIOD

config_engine_i.h
-------------
**Deleted:**
- Deleted macro INTERNAL_KEY_RTC_CONNECTION_LOST_PERIOD

NGIAgoraMediaNodeFactory.h
-------------
**Modified:**
- Changes IMediaControlPacketReceiver::onMediaControlPacketReceived(const uint8_t *packet, size_t length) to IMediaControlPacketReceiver::onMediaControlPacketReceived(uid_t uid, const uint8_t *packet, size_t length)


API  (2021-01-04)
==================================================
NGIAgoraCameraCampturer.h
-------------
**Added:**
- ICameraCaptureObserver::onCameraStateChanged
- ICameraCapturer::switchCamera


API (2020-12-24)
==================================================
adjustUserPlaybackSignalVolume

NGIAgoraLocalUser.h
-------------
**Add:**
- adjustUserPlaybackSignalVolume(uid_t uid, int volume)
- getUserPlaybackSignalVolume(uid_t uid, int* volume)


API  (2020-12-17)
==================================================
video_node_i.h

NGIAgoraRtcConnection.h
-------------
**Modified:**
- Modify TConnectionInfo to include int uid


API (2020-12-09)
==================================================
Rename REMOTE_VIDEO_STREAM_TYPE.

rtc_connection_i.h/NGIAgoraLocalUser.h/NGIAgoraVideoTrack.h
-------------
**Modified:**
- Changes Rename all REMOTE_VIDEO_STREAM_TYPE/REMOTE_VIDEO_STREAM_HIGH to VIDEO_STREAM_TYPE/VIDEO_STREAM_HIGH.

video_node_i.h
-------------
**Modified:**
Modify VideoEncodedImageData.
- Changes Add VIDEO_STREAM_TYPE streamType to VideoEncodedImageData

API (2020-11-30)
==================================================
Refine comments.
- IMediaRelayService::unregisterEventHandler

video_node_i.h
-------------
**Add:**
- IVideoFilterEx::attachStatsSpace


API (2020-11-11)
==================================================
AgoraBase.h
-------------
**Add:**
- MAINTAIN_RESOLUTION into DEGRADATION_PREFERENCE


API (2020-11-11)
==================================================

AgoraBase.h
-------------
**Add:**
- MAINTAIN_RESOLUTION into DEGRADATION_PREFERENCE

API (2020-11-09)
==================================================
Update API parameter

NGIAgoraLocalUser.h
-------------
**Modified:**
Remove elapsed
- Changes `void onVideoTrackPublishSuccess(agora_refptr<ILocalVideoTrack> videoTrack, int elapsed)` to `void onVideoTrackPublishSuccess(agora_refptr<ILocalVideoTrack> videoTrack)`

API (2020-11-6)
==================================================

NGIAgoraAudioDeviceManager.h
-------------
Refine a inclusion usage of TARGET_OS_IPHONE


API (2020-10-27)
==================================================
Add callback for encryption error

NGIAgoraRtcConnection.h
-------------
**Add:**
- IRtcConnectionObserver::onEncryptionError

API (2020-09-16)
==================================================
fix ios and mac marco

API (2020-10-12)

API (2020-09-16)
==================================================
Support SM4 Encryption.

NGIAgoraRtcConnection.h
-------------
**Add:**
- IRtcConnection::enableEncryption

API (2020-09-17)
==================================================
Add member fields to RemoteAudioTrackStats

NGIAgoraAudioTrack.h
-------------
**Add:**
- RemoteAudioTrackStats::expanded_speech_samples
- RemoteAudioTrackStats::expanded_noise_samples
- RemoteAudioTrackStats::timestamps_since_last_report
- RemoteAudioTrackStats::min_sequence_number
- RemoteAudioTrackStats::max_sequence_number

API (2020-08-28)
==================================================
Modify VideoEncodedImageData.

video_node_i.h
-------------
**Modify:**
Modify VideoEncodedImageData.
- Remove packetizationMode.


API (2020-09-11)
==================================================

Add v2.7.1 docs for high-level apis. For details, see jira MS-17241.
-------------
Merge docs for low-level apis from v2.7.0.2 to v2.7.1.


NGIAgoraAudioDeviceManager.h
-------------
**Modified:**
- Modify change marco


API (2020-09-24)
==================================================

 NGIAgoraLocalUser.h
-------------
**Modified:**
- Changes ILocalUser::sendIntraRequest(uid_t uid) to
          ILocalUser::sendIntraRequest(user_id_t userId)


API (2020-09-07)
=======
API (2020-09-15)
Add video engine flags and video track method.

packet_i.h
-------------
**Add:**
- kVideoEngineFlagScalableDelta
- kVideoEngineFlagMajorStreamOnly

video_track_i.h
-------------
**Add:**
Add method to ILocalVideoTrackEx
- GetActiveStreamsCount()


API (2020-09-09)
==================================================
Modify enum VIDEO_FRAME_TYPE

packet_i.h
-------------
**Modified:**
- Changes remove DROPPABLE_FRAME from VIDEO_FRAME_TYPE

API (2020-08-30)
==================================================

NGIAgoraVideoTrack.h
-------------
**Modified:**
Add `frameLossRate` field for struct `RemoteVideoTrackStats`

API (2020-08-21)
==================================================

NGIAgoraAudioDeviceManager.h
-------------
**Deleted:**
- Delete INGAudioDeviceManager::startMicrophoneTest and INGAudioDeviceManager::stopMicrophoneTest
- Delete IAudioDeviceManagerObserver::onVolumeIndication


API (2020-09-02)
==================================================
Modify enum VIDEO_FRAME_TYPE

packet_i.h
-------------
**Modified:**
- Changes remove DROPPABLE_FRAME from VIDEO_FRAME_TYPE


API (2020-08-21)
==================================================
agora_service_i.h
-------------
**Modified:**
Remove dup 'appId'.

IAgoraRtmService.h
-------------
**Modified:**
Merge changes from arsenal.

IAgoraRtmService.h
-------------
**Modified:**
Modify macro definition

NGIAgoraAudioDeviceManager.h
NGIAgoraCameraCapturer.h
NGIAgoraScreenCapturer.h
-------------
**Modified:**
Refine macro defition and add comments for #endif.

NGIAgoraLocalUser.h
-------------
**Modified:**
Fix typo.

NGIAgoraMediaNodeFactory.h
-------------
**Modified:**
Fix return value of setProperty() and getProperty().


API (2020-09-04)
==================================================
Add new callback to IRtcConnectionObserver.

NGIAgoraRtcConnection.h
-------------
**Modified:**
- Changes add onNetworkTypeChanged to IRtcConnectionObserver


API (2020-08-13)
==================================================

IAgoraRtmService.h
-------------
**Modified:**
- Changes onSendMessageState(long long messageId, CHANNEL_MESSAGE_STATE state) to
 onSendMessageState(int64_t messageId, CHANNEL_MESSAGE_STATE state)


API (2020-08-21)
==================================================
NGIAgoraVideoTrack.h
-------------
**Modified:**
- Change addVideoFilter(filter) to addVideoFilter(filter, position)

API (2020-08-07)
==================================================
NGIAgoraMediaNodeFactory.h
-------------
**Add:**
- `IVideoFrameTransceiver`class
- createVideoFrameTransceiver function for IMediaNodeFactory

IAgoraService.h
-------------
**Add:**
- createTranscodedVideoTrack function for IAgoraService


API (2020-08-04)
==================================================
API change to support string UID.

IAgoraService.h
-------------
**Modified:**
Modify AgoraServiceConfiguration to include string uid switch
- bool useStringUid
**Add:**
Add Low level api to IAgoraService to support register user account
- IAgoraService::registerLocalUserAccount


API (2020-08-11)
==================================================
Fix return error

NGIAgoraMediaNodeFactory.h
NGIAgoraLocalUser.h
-------------
**Modified:**
- Modify setProperty and getProperty return type
- Fix spell error


API (2020-07-31)
==================================================

NGIAgoraAudioTrack.h
-------------
Add `mean_waiting_time` field for RemoteAudioTrackStats


API (2020-07-29)
==================================================

NGIAgoraAudioDeviceManager.h
-------------
Add `getPlayoutAudioParameters(AudioParameters* params) const` function for INGAudioDeviceManager
Add `getRecordAudioParameters(AudioParameters* params) const` function for INGAudioDeviceManager


API (2020-08-03)
==================================================

IAgoraService.h
**Add:**
- AgoraServiceConfiguration add logDir


API (2020-07-27)
==================================================

NGIAgoraAudioTrack.h
-------------
Remove free_pcm_data_list_size from ILocalAudioTrack::LocalAudioTrackStats


API (2020-07-17)
==================================================
Modify comment.

AgoraRefCountedObject.h
IAgoraRtmpStreamingService.h
IAgoraService.h
NGIAgoraAudioDeviceManager.h
NGIAgoraAudioTrack.h
NGIAgoraCameraCapturer.h
NGIAgoraExtensionControl.h
NGIAgoraLocalUser.h
NGIAgoraMediaNodeFactory.h
NGIAgoraRtcConnection.h
NGIAgoraScreenCapturer.h
NGIAgoraVideoFrame.h
NGIAgoraVideoMixerSource.h
NGIAgoraVideoTrack.h
-------------
**Modify:**
- Remove api2 prefix.


API (2020-07-16)
==================================================
add onConnecting

NGIAgoraRtcConnection.h
-------------
**Modified:**
add onConnecting to IRtcConnectionObserver


API (2020-07-13)
==================================================
NGIAgoraAudioTrack.h
-------------
**Add:**
- add "uint32_t effect_type" member to ILocalAudioTrack::LocalAudioTrackStats


API (2020-07-03)
==================================================
Modify comment.

NGIAgoraLocalUser.h
-------------
**Modify:**
Modify comment.
- Changes changes comment about pcmDataOnly.

NGIAgoraRtcConnection.h
-------------
**Modify:**
Modify AudioSubscriptionOptions and RtcConnectionConfiguration.
- Changes remove pcmDataOnly from AudioSubscriptionOptions.
- Changes add enableAudioRecordingOrPlayout into RtcConnectionConfiguration.


API (2020-07-01)
==================================================
IAgoraService.h
-------------
**Modify:**
Remove media engine type.


API (2020-06-27)
==================================================
Make include path correct


API (2020-06-18)
==================================================
Modify VideoEncodedImageData.

video_node_i.h
-------------
**Modify:**
Modify VideoEncodedImageData.
- Remove packetizationMode.


API (2020-06-18)
==================================================

Move Move IRecordingDeviceSource from NGIAgoraMediaNodeFactory.h
to NGIAgoraAudioDeviceManager.h


API (2020-06-17)
==================================================

agora_service_i.h
-------------
**Deleted:**
Delete unused api
- Deleted createLocalAudioTrack(const rtc::AudioOptions& audioOptions)


API (2020-06-16)
==================================================
IAgoraRtmService.h
-------------
**Modify:**
Fix compile error
- Remove the incorrect type conversion code in class IChannelEventHandler


API (2020-06-15)
==================================================

IAgoraService.h
-------------
**Add:**
- Add createRecordingDeviceAudioTrack() for IAgoraService.

NGIAgoraMediaNodeFactory.h
-------------
**Add:**
- Add createRecordingDeviceSource() for IMediaNodeFactory.
- Add interface IRecordingDeviceSource



API (2020-06-10)
==================================================

NGIAgoraVideoFrame.h
-------------
**Modify:**
Fix compile error
- unnamed class used in typedef name won't compile in C++20


API (2020-06-09)
==================================================
Modify onMediaPacketReceived.

NGIAgoraMediaNodeFactory.h
-------------
**Modified:**
Modify the declaration of onMediaPacketReceived.
- Changes onMediaPacketReceived(const uint8_t *packet, size_t length)
  to      onMediaPacketReceived(const uint8_t *packet, size_t length, const agora::media::base::PacketOptions& options)

API (2020-06-04)
==================================================
Modify the callback onBandwidthEstimationUpdated

NGIAgoraRtcConnection.h
-------------
**Modified:**
Modify the declaration of onBandwidthEstimationUpdated
- Changes "virtual void onBandwidthEstimationUpdated(int target_bitrate_bps)" to "virtual void onBandwidthEstimationUpdated(const NetworkInfo& info)"


API (2020-06-04)
==================================================
Add SenderOptions and modify createCustomVideoTrack

IAgoraService.h
-------------
**Add:**
Add SenderOptions
- struct SenderOptions

**Modified:**
Change the declaration of createCustomVideoTrack
- Changes "virtual agora_refptr<rtc::ILocalVideoTrack> createCustomVideoTrack(
               agora_refptr<rtc::IVideoEncodedImageSender> videoSource, bool syncWithAudioTrack = false, TCcMode ccMode = CC_ENABLED)"
  To "virtual agora_refptr<rtc::ILocalVideoTrack> createCustomVideoTrack(
          agora_refptr<rtc::IVideoEncodedImageSender> videoSource, SenderOptions& options)"

- Remove option syncWithAudioTrack  from all related apis


API (2020-05-29)
==================================================
Updated docs for the following header files:
- NGIAgoraVideoTrack.h
- NGIAgoraRtcConnection.h
- NGIAgoraMediaNodeFactory.h
- NGIAgoraLocalUser.h
- NGIAgoraAudioTrack.h
- IAgoraService.h


API (2020-05-27)
==================================================

NGIAgoraMediaNodeFactory.h
-------------
**Modify:**
- Add a parameter for createMediaPlayerSource()


API (2020-05-26)
==================================================
NGIAgoraAudioTrack.h
-------------
**Modified:**
- Init RemoteAudioTrackStats


API (2020-05-26)
==================================================
Move createObservableVideoSink() from IMediaNodeFactory to IMediaNodeFactoryEx

NGIAgoraMediaNodeFactory.h
-------------
**Deleted:**
- Delete createObservableVideoSink()


API (2020-05-20)
==================================================
NGIAgoraMediaPlayerSource.h
-------------
**Modified:**
- Rename registerPlayerObserver()/unregisterPlayerObserver() to registerPlayerSourceObserver()/unregisterPlayerSourceObserver() in IMediaPlayerSource.
- Remove 'const' from the parameter of onPlayerSourceStateChanged(), onPositionChanged() and onPlayerEvent() in IMediaPlayerSourceObserver since unnecessary.


API (2020-05-20)
==================================================
NGIAgoraVideoTrack.h
-------------
**Modified:**
- Change addRenderer(renderer) to addRenderer(renderer, position)


API (2020-05-19)
==================================================
NGIAgoraMediaNodeFactory.h
-------------
**Add:**
- bool isExternal()

NGIAgoraVideoTrack.h
-------------
**Modified:**
- remove "bool internal" parameter form funciton addVideoFilter


API (2020-05-19)
==================================================
NGIAgoraCameraCapturer.h
-------------
**Deleted:**
- Delete ICameraCaptureObserver interface
- Delete ICameraCaptureObserver::registerCameraCaptureObserver
- Delete ICameraCaptureObserver::unregisterCameraCaptureObserver
- Delete ICameraCaptureObserver::getCaptureState

NGIAgoraLocalUser.h
-------------
**Modified:**
- Annotation of ILocalUserObserver::onLocalVideoTrackStateChanged


API (2020-05-12)
==================================================
Remove illegal header files included and add including header files.

NGIAgoraLocalUser.h
-------------
**Deleted:**
Delete the IAgoraRtcEngine.h included
- Deleted #include "IAgoraRtcEngine.h"

NGIAgoraMediaNodeFactory.h
-------------
**Deleted:**
Delete the IAgoraRtcEngine.h included
- Deleted #include "IAgoraRtcEngine.h"

internal/agora_service_i.h
-------------
**Add:**
Add the IAgoraLog.h including
- #include "base/IAgoraLog.h"


API (2020-05-09)
=========================

IAgoraService.h
--------------------
**Add:**

Add AUDIO_SCENARIO_TYPE audioScenario for AgoraServiceConfiguration


API (2020-05-08)
==================================================
NGIAgoraLocalUser.h
NGIAgoraMediaNodeFactory.h
NGIAgoraMediaPlayerSource.h
internal/media_node_factory_i.h
internal/rtc_connection_i.h
-------------
**Modified:**

- Change namespace


API (2020-05-08)
==================================================
NGIAgoraExtensionControl.h
-------------
**Modified:**
  - Move LOG_LEVEL from AgoraBase.h to IAgoraLog.h and change namespace to commons

NGIAgoraMediaNodeFactory.h
-------------
**Modify:**
- Remove parameter |view| in createVideoRenderer



- Change namespace

API (2020-05-07)
==================================================
AgoraOptional.h
-------------
**Modified:**
- Changes optional.h to AgoraOptional.h

API (2020-04-29)
==================================================
Refine RemoteVideoTrackStats

NGIAgoraVideoTrack.h
-------------
**Modified:**
Rename fields
- streamType to rxStreamType

**Add:**
Add fields
- decoderOutputFrameRate
- rendererOutputFrameRate
- packetLossRate
- totalFrozenTime
- frozenRate

API (2020-04-26)
==================================================
NGIAgoraCameraCapturer.h
-------------
**Deleted:**
- ICameraCapturer::CAPTURE_STATE::CAPTURE_STATE_STOPPING
- ICameraCapturer::CAPTURE_STATE::CAPTURE_STATE_STARTING
- switchCamera()

API (2020-04-20)
==================================================
NGIAgoraMediaNodeFactory.h
-------------
**Add:**
- IVideoSinkBase::onDataStreamWillStart()
- IVideoSinkBase::onDataStreamWillStop()
- IVideoFilter::onDataStreamWillStart()
- IVideoFilter::onDataStreamWillStop()


API (2020-04-17)
==================================================
NGIAgoraExtensionControl.h
-------------
**Add:**
Add IExtensionControl interfaces for agora extensions
- getCapabilities
- registerExtensionProvider
- unregisterExtensionProvider
- createVideoFrame
- copyVideoFrame
- recycleVideoCache
- dumpVideoFrame
- log

NGIAgoraExtension.h
-------------
**Deleted:**
file deleted

NGIAgoraMediaNodeProvider.h
-------------
**Deleted:**
file deleted

NGIAgoraMediaNodeFactory
-------------
**Deleted:**
- registerMediaNodeProvider
- unregisterMediaNodeProvider


API (2020-04-16)
==================================================
NGIAgoraVideoFrame.h
**Add:**
Add IVideoFrame interfaces for external video frames
- type
- format
- width
- height
- size
- rotation
- setRotation
- timestampUs
- setTimestampUs
- data
- mutableData
- resize
- textureId
- fill


API (2020-04-13)
==================================================
NGIAgoraMediaPlayerSource.h
-------------
**Modified:**

- Refine the API order of IMediaPlayerSource().


API (2020-04-12)
Add send intra request api.
==================================================
NGIAgoraLocalUser.h
-------------
**Add:**
Add send intra request api.
- sendIntraRequest()


API (2020-04-11)
==================================================
NGIAgoraVideoTrack.h
-------------
**Modified:**
- IVideoTrack::addVideoFilter()

API (2020-04-10)
==================================================
NGIAgoraRtcConnection.h
-------------
**Added:**
Support sending custom event to argus
- IRtcConnection::sendCustomReportMessage().

IAgoraRtmService.h
-------------
**Added:**
Add parameter 'eventSpace', used during report RTM events
- IAgoraRtmService::initialize(const char *appId, IRtmServiceEventHandler *eventHandler, uint64_t eventSpace).


API (2020-04-10)
==================================================
Add field in struct LocalAudioTrackStats

NGIAgoraAudioTrack.h
-------------
*Deleted:**
delete struct LocalAudioTrackStats, use LocalAudioStats instead
- struct LocalAudioTrackStats


API (2020-04-10)
==================================================
NGIAgoraMediaPlayerSource.h
-------------
**Modified:**
- Change one parameter's type of getStreamCount() and getStreamInfo() from int to int64_t to get aligned with the other APIs.


API (2020-04-09)
==================================================
NGIAgoraVideoMixerSource.h
-------------
**Add:**
- Video Mixer related data structures and interfaces (POC)

IAgoraService.h
-------------
**Add:**
- createMixedVideoTrack (POC)

NGIAgoraMediaNodeFactory.h
-------------
**Add:**
- createVideoMixer (POC)


API (2020-04-03)
==================================================
NGIAgoraCameraCapturer.h
-------------
**Modified:**
Add __APPLE__ macro judge.

API (2020-04-03)
==================================================
Add intra request callback and network observer.

NGIAgoraLocalUser.h
-------------
**Add:**
Add intra request callback.
- ILocalUserObserver::onIntraRequestReceived()

NGIAgoraRtcConnection.h
-------------
**Add:**
Add network observer and its register/unregister methods.
- class INetworkObserver
- IRtcConnection::registerNetworkObserver()
- IRtcConnection::unregisterNetworkObserver()

NGIAgoraCameraCapturer.h
-------------
**Add:**
Add destructor of ICameraCaptureObserver
- ICameraCaptureObserver::~ICameraCaptureObserver()

NGIAgoraMediaNodeFactory.h
-------------
**Modified:**
Modify the implement of BeautyOptions constructor
- BeautyOptions::BeautyOptions()


API (2020-04-2)
==================================================
NGIAgoraLocalUser.h
-------------
*Add:**
 elapsed in onUserVideoTrackStateChanged onVideoTrackPublishSuccess

API (2020-03-31)
==================================================
Move appId from AgoraServiceConfigurationEx to AgoraServiceConfiguration

IAgoraService.h
-------------
*Add:**
struct AgoraServiceConfiguration {
  const char* appId = nullptr;
}


API (2020-03-30)
==================================================
NGIAgoraCameraCapturer.h
-------------
**Modified:**
Refine the API order of IDeviceInfo.


API (2020-03-30)
==================================================
IAgoraService.h
-------------
**Modified:**
Remove enabled parameter from IAgoraService::createLocalAudioTrack()


API (2020-03-27)
==================================================
NGIAgoraRtcConnection.h
-------------
**Modified:**
  - Modified the comment of onLastmileQuality
  - Modified the comment of startLastmileProbeTest

**Deleted:**
  - Deleted enableLastmileTest()
  - Deleted disableLastmileTest()


API (2020-03-16)
==================================================
NGIAgoraAudioTrack.h
-------------
**Add:**
Add isEnabled to ILocalAudioTrack to get the local audio track enabled status
- ILocalAudioTrack::isEnabled()


API (2020-03-16)
==================================================
NGIAgoraRtcConnection.h
-------------
**Add:**
- enum RECV_TYPE


API (2020-03-12)
==================================================
Add 'received_bytes' for 'RemoteAudioTrackStats'

NGIAgoraAudioTrack.h
-------------
**Add:**
- RemoteAudioTrackStats::received_bytes


API (2020-03-06)
==================================================
Add 'virtual' for ~IAgoraService() and ~IAgoraServiceEx()

IAgoraService.h
-------------
**Modified:**
- Add 'virtual' for ~IAgoraService()

agora_service_i.h
-------------
**Modified:**
- Add 'virtual' for ~IAgoraServiceEx()


API (2020-02-27)
==================================================
new cc type

AgoraBase.h
-------------
**Modified:**
- Add new cc type CONGESTION_CONTROLLER_TYPE_AUT_CC;


API (2020-02-25)
==================================================
Changes for AudioSessionConfiguration

IAgoraService.h
-------------
**Modified:**
- Change Optional<int> ioBufferDuration; to Optional<double> ioBufferDuration;


API (2020-02-23)
==================================================
API changes for supporting data channel

NGIAgoraRtcConnection.h
-------------
**Add:**
Add data channel api to IRtcConnection
- createDataStream()
- sendStreamMessage()

Add data channel callback api to IRtcConnectionObserver
- onStreamMessage()
- onStreamMessageError()


API (2020-02-21)
==================================================
Changes for audio video stats and state

NGIAgoraAudioTrack.h
-------------
**Add:**
Add two structs of audio track stats.
- add struct RemoteAudioTrackStats
- add struct LocalAudioTrackStats

**Modified:**
- Changes getStatistics(RemoteAudioStats& stats) to getStatistics(RemoteAudioTrackStats& stats)
- Changes REMOTE_AUDIO_STATE getState() to REMOTE_AUDIO_STATE getState()

NGIAgoraLocalUser.h
-------------
**Add:**
Add callback functions of audio track statistics.
- onLocalAudioTrackStatistics()
- onRemoteAudioTrackStatistics()

**Modified:**
Modify the type of field codec_name
- Changes std::string codec_name to char codec_name[media::kMaxCodecNameLength]
- Changes onUserAudioTrackStateChanged(user_id_t, agora_refptr<rtc::IRemoteAudioTrack>, REMOTE_AUDIO_STREAM_STATE, REMOTE_AUDIO_STREAM_REASON)
to onUserAudioTrackStateChanged(user_id_t, REMOTE_AUDIO_STATE, REMOTE_AUDIO_STATE_REASON, int) = 0;

NGIAgoraAudioDeviceManager.h
-------------
**Deleted:**
- Deleted onMicrophoneEnabled()


API (2020-02-20)
==================================================
Support lastmile probe test

NGIAgoraRtcConnection.h
-------------
**Add:**
Add callback for getting lastmile probe test results and two functions to control lastmile probe test
- void onLastmileProbeResult(const LastmileProbeResult& result)
- int startLastmileProbeTest(const LastmileProbeConfig& config)
- int stopLastmileProbeTest()


API (2020-02-10)
==================================================
API changes for create video track parameter

IAgoraService.h
-------------
**Modified:**

  - createCameraVideoTrack(), createScreenVideoTrack(), createCustomVideoTrack()
  remove `bool enable`


API (2020-02-15)
==================================================
API changes for unifying the definition of connection state in AgoraBase.h

NGIAgoraRtcConnection.h
-------------
**Deleted:**
Delete the definition of TConnectionState.
* Deleted enum TConnectionState


API (2020-02-06)
==================================================
API changes for media packet and media control packet sender and receiver

IAgoraService.h
--------------------------
**Add:**

Add two functions to create media tracks with IMediaPacketSender, which is created from media node factory.
- agora_refptr<rtc::ILocalAudioTrack> createCustomAudioTrack(agora_refptr<rtc::IMediaPacketSender> source)
- agora_refptr<rtc::ILocalVideoTrack> createCustomVideoTrack(agora_refptr<rtc::IMediaPacketSender> source)

NGIAgoraAudioTrack.h
--------------------------
**Add:**
Add two functions to register and unregister media packet receiver.

- int registerMediaPacketReceiver(IMediaPacketReceiver* packetReceiver)
- int unregisterMediaPacketReceiver(IMediaPacketReceiver* packetReceiver)

NGIAgoraLocalUser.h
--------------------------
**Add:**
Add function to get IMediaControlPacketSender.
Add two functions to register and unregister media control packet receiver.

- IMediaControlPacketSender* getMediaControlPacketSender()
- int registerMediaControlPacketReceiver(IMediaControlPacketReceiver* ctrlPacketReceiver)
- int unregisterMediaControlPacketReceiver(IMediaControlPacketReceiver* ctrlPacketReceiver)

NGIAgoraVideoTrack.h
--------------------------
**Add:**
Add two functions to register and unregister media packet receiver.

- int registerMediaPacketReceiver(IMediaPacketReceiver* packetReceiver)
- int unregisterMediaPacketReceiver(IMediaPacketReceiver* packetReceiver)

NGIAgoraMediaNodeFactory.h
--------------------------
**Add:**
Add function to create media packet sender as well as related definitions

- agora_refptr<IMediaPacketSender> createMediaPacketSender()
- IMediaPacketSender
- IMediaControlPacketSender
- IMediaPacketReceiver
- IMediaControlPacketReceiver


API (2020-01-21)
==================================================
API changes for getting audio filter.

Audio Filter (api2/NGIAgoraMediaNodeFactory.h)
--------------------------

**Add:**

Main methods:

- const char * getName() const

Local Audio Track (api2/NGIAgoraAudioTrack.h)
--------------------------

**Add:**

Main methods:

- agora_refptr<IAudioFilter> getAudioFilter(const char *name) const


API (2020-02-10)
==================================================
Deleted:
-------------
* IAgoraService.h
  - class IAgoraService
    -API createSignalingEngine()


API (2020-01-20)
==================================================
Add:
-------------
* NGIAgoraLocalUser.h
  - class ILocalUser
    - API adjustPlaybackSignalVolume()
    - API getPlaybackSignalVolume()


API (2020-01-19)
==================================================
Modified:
-------------
* NGIAgoraRtcConnection.h
  - struct RtcConnectionConfiguration
    add CHANNEL_PROFILE_TYPE  channelProfile


API (2020-01-17)
==================================================
Modified:
-------------
* IAgoraService.h
  - createCameraVideoTrack(), createScreenVideoTrack(), createCustomVideoTrack()
  and createCustomVideoTrack() change argument to `bool enable = false`


API (2020-01-15)
==================================================
Add:
-------------
* NGIAgoraScreenCapturer.h
  - class IScreenCapturer
    - API initWithMediaProjectionPermissionResultData()


API (2020-01-13)
==================================================
Modified:
-------------
* NGIAgoraMediaPlayerSource.h
  - Rename onPlayerStateChanged() of IMediaPlayerSourceObserver to onPlayerSourceStateChanged


API (2020-01-09)
==================================================
Deleted:
-------------
* NGIAgoraMediaPlayerSource.h
  - class IMediaPlayerSource
    - API registerVideoFrameObserver()
    - API unregisterVideoFrameObserver()


API (2020-01-08)
==================================================
Add:
-------------
* NGIAgoraMediaPlayerSource.h
  - Add the following class
    - class IMediaPlayerSource
    - class IMediaPlayerSourceObserver

* NGIAgoraMediaNodeFactory.h
  - class IMediaNodeFactory
    - Add API createMediaPlayerSource()

* IAgoraService.h
  - class IAgoraService
    - Add API createMediaPlayerVideoTrack()
    - Add API createMediaPlayerAudioTrack()


Deleted:
-------------
* NGIAgoraMediaPlayer.h
  - Delete class IMediaPlayer
  - Delete class IMediaPlayerObserver

* IAgoraService.h
  - class IAgoraService
    - Delete API createMediaPlayer()


API (2019-12-24)
==================================================
Macro cleanup

IAgoraService (api2/IAgoraService.h)
 - remove FEATURE_RTMP_STREAMING_SERVICE and FEATURE_RTM_SERVICE


API (2019-12-22)
==================================================
API changes

Add:
-------------

Modified:
-------------
* AgoraBase.h
  - rename RemoteVideoTrackInfo to VideoTrackInfo
  - relevant changes in other interfaces

* IAgoraRtcEngine.h
  - struct VideoCanvas add field isScreenView

* IAgoraService.h
  - createCameraVideoTrack(), createScreenVideoTrack(), createCustomVideoTrack()
  and createCustomVideoTrack() add argument `bool enable = true`

* NGIAgoraLocalUser.h
  - rename RemoteVideoTrackInfo to VideoTrackInfo

* NGIAgoraMediaNodeFactory.h
  - rename RemoteVideoTrackInfo to VideoTrackInfo

* NGIAgoraVideoTrack.h
  - rename RemoteVideoTrackInfo to VideoTrackInfo


API (2019-12-03)
==================================================
API changes for graceful exiting

IAgoraService (api2/IAgoraService.h)
 - add release()


API (2019-12-10)
==================================================
Local Audio Track (api2/NGIAgoraAudioTrack.h)
--------------------------

**Add:**

Main methods:

- int enableEarMonitor(bool enable, bool includeAudioFilter)

API (2019-12-04)
==================================================
API changes for supporting create RTMP streaming serivce.

Add:
-------------
* IAgoraRtmpStreamingService.h
* IAgoraRtmService.h

Modified:
-------------
* IAgoraService.h
  - Add method `agora_refptr<rtc::IRtmpStreamingService> createRtmpStreamingService`
  - Add method `rtm::IRtmService* createRtmService()`


API (2019-11-29)
==================================================
API changes for supporting audio filter.

Audio Filter (api2/NGIAgoraMediaNodeFactory.h)
--------------------------

Base class of custom audio filter, application can implement its own
filter and add it to audio track.

New interface and enum definition:

```
class IAudioFilterBase : public RefCountInterface {
public:
 // Return false if filter decide to drop the frame.
 virtual bool adaptAudioFrame(const AudioPcmFrame& inAudioFrame,
                              AudioPcmFrame& adaptedFrame) = 0;
};

class IAudioFilter : public IAudioFilterBase {
 public:
  virtual void setEnabled(bool enable) = 0;
  virtual bool isEnabled() = 0;
};

enum AUDIO_FILTER_TYPE {
  AUDIO_FILTER_EFFECT,
  AUDIO_FILTER_ANS,
  AUDIO_FILTER_VOICE_CHANGE
};
```


Media Node Factory (api2/NGIAgoraMediaNodeFactory.h)
--------------------------

**Add:**

Main methods:

- agora_refptr<IAudioFilter> createAudioFilter(AUDIO_FILTER_TYPE type)


Local Audio Track (api2/NGIAgoraAudioTrack.h)
--------------------------

**Add:**

Internal enum:

```
  enum AudioFilterPosition {
    Default
  };
```

Main methods:

- bool addAudioFilter(agora_refptr<IAudioFilter> filter, AudioFilterPosition position)
- bool removeAudioFilter(agora_refptr<IAudioFilter> filter, AudioFilterPosition position)


API (2019-11-28)
==================================================
API cleanup

Modified:
-------------
* NGIAgoraMediaNodeFactory
  - modify class IVideoFrameSender, use agora ExternalVideoFrame instead of webrtc VideoFrame
  - modify class IVideoFilterBase
    - use agora VideoFrame instead of webrtc VideoFrame
    - remove onSinkWantsChanged
  - modify class IVideoSinkBase
    - Add a interface to distinguish external or internal video sink
  - move class IVideoFrameAdapter to internal interface
  - move createVideoFrameAdapter() to internal interface

* NGIAgoraAudioTrack.h
  - IAudioTrack
    - virtual int adjustPlayoutVolume(int volume) = 0;
    - virtual int getPlayoutVolume(int* volume) = 0;
  - ILocalAudioTrack
    - virtual int adjustPublishVolume(int volume) = 0;
    - virtual int getPublishVolume(int* volume) = 0;


API (2019-11-14)
==================================================
Refine Media Statistics and event report

Add:
-------------

Modified:
-------------
* NGIAgoraAudioTrack.h
  - Add struct LocalAudioTrackStats
  - Add method GetStats()

* NGIAgoraLocalUser.h
  - rename LocalAudioStats to LocalAudioDetailedStats, since LocalAudioStats belong to high level API

Deleted:
-------------
* AgoraBase.h
  - remove AUDIO_CODEC_AAC in AUDIO_CODEC_TYPE


API (2019-11-25)
==================================================
API changes for extension

 - move `interface/cpp/api2/IAgoraParameter.h` to `interface/cpp/IAgoraParameter.h`


API (2019-11-21)
==================================================
API changes for extension

 - move `interface/cpp/AgoraParameter.h` to `interface/cpp/api2/IAgoraParameter.h`


API (2019-11-19)
==================================================
API changes.

Audio Frame Observer (IAgoraMediaEngine.h)
--------------------------

**Modified:**

Main field(s)

| Old  | New |
|---|---|
|`IAudioFrameObserver::AudioFrame::samples`  |  `IAudioFrameObserver::AudioFrame::samplesPerChannel` |


API (2019-11-12)
==================================================
API changes for extension

Agora Extension (interface/cpp/api2/NGIAgoraExtension.h)
-------------
 - struct `agora_extension`
 - struct `agora_core`

API (2021-06-03)
==================================================
IAudioDeviceManagerObserver (api2/NGIAgoraAudioDeviceManager.h)
--------------------------

**Add:**
-  void onVolumeIndication(int volume)

API (2019-11-11)
==================================================
API changes for supporting audio local playback.

Local Audio Track (api2/NGIAgoraAudioTrack.h)
--------------------------

**Add:**

Main methods:

- void enableLocalPlayback(bool enable)
- int adjustPlayoutVolume(int volume)
- int adjustPublishVolume(int volume)


API (2019-11-08)
==================================================
API changes for default client role is audience.

Rtc Engine (interface/cpp/IAgoraRtcEngine.h)
-------------
 - struct `ChannelMediaOptions` add field `clientRoleType`.

Rtc Connection (interface/cpp/api2/NGIAgoraRtcConnection.h)
-------------
 - struct `RtcConnectionConfiguration` add field `clientRoleType`.

Local User (api2/NGIAgoraLocalUser.h)
-------------

**Add:**

Main methods:

 - CLIENT_ROLE_TYPE getUserRole();


API (2019-11-05)
==================================================
API changes for supporting register/unregister
encoded image receiver.

**Deleted:**

move IVideoEncodedImageReceiver to AgoraBase.h


API (2019-11-04)
==================================================
Media Player (api2/NGIAgoraMediaPlayer.h)
-------------

**Add:**

Main methods:

 - int setLooping(bool looping)


**Deleted:**

Main methods:

 - int close()


API (2019-10-30)
==================================================
API changes for supporting audio simple audio player.

Media Player (api2/NGIAgoraMediaPlayer.h)
-------------

This interface provide access to a media player.

Main methods are:

 - int open(const char* url)
 - int play()
 - int playFromPosition(int position)
 - int stop()
 - int pause()
 - int resume()
 - int getDuration(int& duration)

 - int getCurrentPosition(int& currentPosition)
 - int setCurrentPosition(int newPos)
 - int close()


 - int registerPlayerObserver(IMediaPlayerObserver* observer)
 - int unregisterPlayerObserver(IMediaPlayerObserver* observer)
 - agora_refptr<rtc::ILocalAudioTrack> getPlayerAudioTrack()
 - agora_refptr<rtc::ILocalVideoTrack> getPlayerVideoTrack()

The denifinion of IMediaPlayerObserver:
```
class IMediaPlayerObserver {
public:
  virtual ~IMediaPlayerObserver() = default;
  virtual void onPlayerStateChanged(const IMediaPlayer::PlayerState& state) = 0;
  virtual void onPositionChanged(const int position) = 0;
};
```


API (2019-10-17)
==================================================
API changes for supporting audio tx mixer.

Local User (api2/NGIAgoraLocalUser.h)
--------------------------

**Add:**

Internal struct

- struct ANAStats;
- struct AudioProcessingStats;
- struct LocalAudioStats;

Main methods:

- int setAudioEncoderConfiguration(const rtc::AudioEncoderConfiguration& config)
- bool getLocalAudioStatistics(LocalAudioStats& stats)

**Modified:**

Main methods

| Old  | New |
|---|---|
|`int setPlaybackAudioFrameParameters(size_t bytesPerSample, size_t numberOfChannels, uint32_t sampleRateHz)`  |  `int setPlaybackAudioFrameParameters(size_t numberOfChannels, uint32_t sampleRateHz)` |
|`int setRecordingAudioFrameParameters(size_t bytesPerSample, size_t numberOfChannels, uint32_t sampleRateHz)`  |  `int setPlaybackAudioFrameParameters(size_t numberOfChannels, uint32_t sampleRateHz)` |
|`int setMixedAudioFrameParameters(size_t bytesPerSample, size_t numberOfChannels, uint32_t sampleRateHz)`  |  `int setPlaybackAudioFrameParameters(size_t numberOfChannels, uint32_t sampleRateHz)` |
|`int setPlaybackAudioFrameBeforeMixingParameters(size_t bytesPerSample, size_t numberOfChannels, uint32_t sampleRateHz)`  |  `int setPlaybackAudioFrameParameters(size_t numberOfChannels, uint32_t sampleRateHz)` |


agora_refptr (AgoraRefPtr.h)
--------------------------

**Add:**

Main methods

- bool operator==(const agora_refptr<T>& r)
- bool operator<(const agora_refptr<T>& r) const


Audio PCM Data (api2/internal/audio_node_i.h)
--------------------------

**Modified:**

Main field(s)

| Old  | New |
|---|---|
| number_of_samples  | samples_per_channel |


Local Audio Track (api2/NGIAgoraAudioTrack.h)
--------------

**Deleted:**

Internal struct

- struct ANAStats;
- struct AudioProcessingStats;
- struct LocalAudioStats;

Main methods:

- int setAudioEncoderConfiguration(const rtc::AudioEncoderConfiguration& config)
- bool getStatistics(LocalAudioStats& stats)


API (2019-10-13 recorded frome release/2.6.0)
==================================================
The full details of API descritpion can be seen in https://confluence.agoralab.co/display/MS/API+Introduction


Agora Low Level APIs
============================
NGIAgoraLocalUser.h
-------------
**Modify:**
moidfy enableAudioVolumeIndication support vad


API (2021-09-01)
==================================================

Agora Service
-------------
Agora Communications as a Service (CaaS) provides ensured quality of experience (QoE) for worldwide Internet-based voice and video communications through a virtual global network that is especially optimized for real-time web and mobile-to-mobile applications.

The IAgoraService is the entry point of Agora Low Level APIs. This interface is used by applications to create access points to Agora Modules to enable real-time communication, including RTC Connection, Media Tracks, Audio Device Manager and etc. In addition, in order to customize different user scenarios, the interface allows application to configure service and media sessions on the global level, i.e., AgoraServiceConfiguration or AudioSessionConfiguration.

Main methods are:

- initialize(appId, serviceConfiguration)
- createRtcConnection()
- createLocalAudioTrack()
- createAudioDeviceManager()
- createMediaNodeFactory()
- createLocalVideoTrack()
- setAudioSessionConfiguration()
- getAudioSessionConfiguration()
- setLogFile()
- setLogFilter()
- release()

RTC Connection
-------------
This allows an application to establish a connection to a Agora Channel. In order to establish a connection, application must provide an App ID or Token and the channel identifier you want to join.

Connecting to a Agora Channel is done asynchronous, application can listen connection state by provided RTC Connection Observer. With connected to a channel, application can use [Local User](##Local User) to publish and subscribe media stream/data from Agora Channel.

RTC connection also monitors remote users in the channel. Once a remote user joined or left a channel application will be notified.

Main methods are:

- connect()
- disconnect()
- getLocalUser()
- getConnectionInfo()
- getRemoteUsers()
- getUserInfo()
- release()

- IConnectionObserver::onConnected()
- IConnectionObserver::onDisconnected()
- IConnectionObserver::onConnecting()
- IConnectionObserver::onConnectionFailure()
- IConnectionObserver::onUserJoined()
- IConnectionObserver::onUserleft()

Local User (get from RtcConnection)
-------------
This interface represents a combination of Publisher and Subscriber. Each Rtc Connection has its own Local User, and application can get the Local User by IRtcConnection::getLocalUser()

The Local User has two clients roles, broadcaster(Publisher with or w/o Subscriber) and audience(Subscriber only). The Publisher publish local Audio and Video tracks to a channel, like a host in LIVE BROADCAST profile or a participant in COMMUNICATION profile.
The Subscriber subscribes and receives remote Audio and Video tracks from different users in the channel.

Main methods are:

- publishAudio()
- unpublishAudio()
- publishVideo()
- unpublishVideo()
- subscribeAudio()
- subscribeAllAudio()
- unsubscribeAudio()
- unsubscribeAllAudio()
- subscribeVideo()
- subscribeAllVideo()
- unsubscribeVideo()
- unsubscribeAllVideo()
- registerLocalUserObserver()
- unregisterLocalUserObserver()
- release()

- ILocaUserlObserver::onAudioTrackPublishSuccess()
- ILocaUserlObserver::onAudioTrackPublicationFailure()
- ILocaUserlObserver::onVideoTrackPublishSuccess()
- ILocaUserlObserver::onVideoTrackPublicationFailure()
- ILocaUserlObserver::onUserAudioTrackSubscribed()
- ILocaUserlObserver::onUserAudioTrackUnsubscribed()
- ILocaUserlObserver::onUserAudioTrackStateChanged()
- ILocaUserlObserver::onUserAudioTrackStatistics()
- ILocaUserlObserver::onUserAudioTrackSubscriptionFailure()
- ILocaUserlObserver::onUserVideoTrackSubscribed()
- ILocaUserlObserver::onUserVideoTrackUnsubscribed()
- ILocaUserlObserver::onUserVideoTrackStateChanged()
- ILocaUserlObserver::onUserVideoTrackStatistics()
- ILocaUserlObserver::onUserVideoTrackSubscriptionFailure()

- ILocaUserlObserver::onUserNetworkQuality()
- ILocaUserlObserver::onAudioVolumeIndication()


Local Audio Track
--------------
A presentation of a PCM stream. A Local Audio Track can be created directly by IAgoraService::createLocalTrack(), which originates from default audio recording device, i.e., built-in microphone. Application can also change default audio recording devices by IAudioDeviceManager APIs if multiple recording devices available in system. Local Audio Track can also be created from IMediaPlayer().

After local tracks are created, Application can publish one or multiple local audio tracks through ILocalUser::PublishAudio(). By natual, all published tracks will be mixed first, then encoded into a single audio stream and send to Agora Channel.

Main methods are:
- setEnabled()
- isEnabled()
- adjustSignalingVolume()
- getSignalingVolume()
- release()

Remote Audio Track
---------
A presentation of a remote audio track.

Main methods are:
- setPlaybackEnabled()
- isPlaybackEnabled()
- getState()

Audio Device Manager
-------------
The interface provides access to connected audio recording devices like microphones and audio playout devices like speakers.

Main methods are:

- setMicrophoneVolume()
- getMicrophoneVolume()
- setSpeakerVolume()
- getSpeakerVolume()
- setMicrophoneMute()
- getMicrophoneMute()
- setSpeakerMute()
- getSpeakerMute()

- getNumberOfPlayoutDevices()
- getNumberOfRecordingDevices()
- getPlayoutDeviceInfo()
- getRecordingDeviceInfo()
- setPlayoutDevice()
- setRecordingDevice()

- startMicrophoneTest()
- stopMicrophoneTest()

- registerObserver()
- unregisterObserver()

- onVolumeIndication()
- onDeviceStateChanged()
- onRoutingChanged()

Video Node Factory
----------------
This factory interface is used to create video nodes to build video track.

Main methods are:

- createCameraCapturer()
- createScreenCapturer()
- createCustomVideoSource()
- createVideoFilter()
- createVideoRenderer()

Video Camera Capturer
----------------
This interface is used to control camera as a video source. It also manages camera devices in the windows and MacOS.

Main methods are:

- setCameraSource()
- switchCamera()
- startCapture()
- stopCapture()
- getCaptureFormat()
- getCaptureState()
- registerVideoFrameObserver()
- unregisterVideoFrameObserver()
- registerCameraCaptureObserver()
- unregisterCameraCaptureObserver()

Video Screen Capturer
----------------
This interface is used to control screen sharing as video source.

Main methods are:

- release()

Custom Video Source
----------------
This interface allows application to push external video frame to the Agora SDK.

Main method is:

- pushVideoFrame()

Video Filter & Video Filter Base
----------------
The Video Filter is an intermediate node, which can be added in the video track by IVideoTrack::addVideoFilter(). It reads video frames from the previous node in the track, e.g., a video source or another video filter and then writes video frames back after frame adaption.

Agora provides several built-in video filters, e.g., video beauty filter, which can be created by IMediaNodeFactory::createVideoFilter().

Application can also write its own video filter to modify video frame by implementing IVideoFilterBase::adaptVideoFrame() and add the video filter in the track by IVideoTrack::addVideoFilter().

Main methods are:

- setFilterParameters()
- setEnabled()
- isEnabled()
- registerVideoFrameObserver()
- unregisterVideoFrameObserver()


Video Renderer & Video Sink Base
----------------
The Video Renderer inherits Video Sink Base, which is used to render video frame. Agora provides built-in video renderer by IMediaNodeFactory::createVideoRenderer() and application can add it in the video track by IVideoTrack::addVideoRenderer()
The Video Sink Base is video frame observer, which can reads video frame from built-in video sources, video filters and video track.

Main methods are:

- setRenderMode()
- setMirror()

Local Video Track
----------------
A presentation of local video track that originates from one video source, e.g., camera capturer, screen capturer or a custom video source. Local Video Track can be created by IAgoraService::createVideoTrack()

Agora allows application to customize video track by selecting or adding different video nodes which can be created by IMediaNodeFactory, e.g., Video Source, Video Filter and Video Renderer. You can add multiple nodes and the order of nodes in the pipeline depends on the order of addition by the application.

After video track customized, Application can publish one or multiple local video tracks through ILocalUser::PublishVideo() and each video track has its own video send stream. e.g., you can send camera capturing stream and screen sharing stream simultaneously, and remote users can receive two video streams accordingly.

Main methods are:

- setEnabled()
- isEnabled()
- addVideoFilter()
- removeVideoFilter()
- addVideoRenderer()
- removeVideoRenderer()
- release()

Remote Video Track
----------------
A presentation of a remote video track. Similar with local video track, it also supports customization by adding video filters and renderers.

Main methods are:

- isEnabled()
- addVideoFilter()
- removeVideoFilter()
- addVideoRenderer()
- removeVideoRenderer()
- release()


Terminologies & Basics
==============================
APP ID
-----------
Each “vendor�organization using the Agora SDK (across platforms) has a unique App ID which identifies that organization. All the communication sessions that are created across the Agora Global Network for one App ID are isolated from all other sessions for other App IDs. Therefore, communication sessions will not be connected together across vendors. Statistics, management, and billing are all separated according to the App ID. If an organization has multiple applications that should be kept completely separate, like they are built by completely different teams, then they would use multiple App IDs in this case. If applications need to communicate with each other, then a single App ID should be used.

Dynamical Keys
----------
Using your App ID directly is easy and works well for the initial development of applications, However, if someone illicitly obtains your App ID, then they will be able to perform the same operations in their own client applications. This will allow them to join sessions that belong to you and are billed to you. To prevent this and to secure your applications, Agora recommends to use dynamic keys for large-scale production applications.

Dynamical Keys has constructed within server-side code by APP Cert and other key materials, and APP Cert is never accessible in any client code, which makes Dynamical Key is more secure than static App ID.

Dynamical Key has expiration dates and also contains client permissions as different roles to access Agora Global Network, like host and audience.

Please also review [Agora's Security Keys](https://docs.agora.io/en/2.0.2/product/Interactive%20Broadcast/Product%20Overview/key?platform=All%20Platforms) for more technical details.

Agora Channel
-------------
Think of a channel as a meeting room.  To set up communications between two or more instances of an application, the instances will join the same channel.  Channel names are unique within the domain of an App ID.  The process for app instances to exchange channel names is managed outside the SDK, and is handled by the application developer.

Channels are hosted on the Agora.io Global Network and a channel will automatically be created the first time an app instance tries to join, and destroyed when the last app instance leaved.

User & User Information
--------------
User represents a participant in the channel. Each User has a unique string ID for its identity. Application can get current users-in-channel information and monitor user joining and leaving event by provided RTC Connection Observer after connected to a channel.

There are two kinds of users in channel, Local User and remote users. Local User is application itself which has two roles as Publisher to publish local streams and Subscriber to subscribe remote streams. Remote user represents the remote participant in the channel, who produces and publishes remote stream from remote end.

After connection established, application can get basic user info including User Id and user's media stream info. If Subscriber has subscribed specific remote user, application may also monitor remote user's stream track state.
