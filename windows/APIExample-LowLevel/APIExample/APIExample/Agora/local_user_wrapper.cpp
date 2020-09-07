//  Agora RTC/MEDIA SDK
//
//  Created by Pengfei Han in 2020-03.
//  Copyright (c) 2020 Agora.io. All rights reserved.
//
#include "pch.h"
#include "local_user_wrapper.h"
#include "utils/log.h"

LocalUserWrapper::LocalUserWrapper(agora::rtc::ILocalUser* local_user) : local_user_(local_user) {
  local_user_->registerLocalUserObserver(this);
}

LocalUserWrapper::~LocalUserWrapper() { local_user_->unregisterLocalUserObserver(this); }

agora::rtc::ILocalUser* LocalUserWrapper::GetLocalUser() { return local_user_; }

void LocalUserWrapper::PublishAudioTrack(
    agora::agora_refptr<agora::rtc::ILocalAudioTrack> audioTrack) {
  local_user_->publishAudio(audioTrack);
}

void LocalUserWrapper::PublishVideoTrack(
    agora::agora_refptr<agora::rtc::ILocalVideoTrack> videoTrack) {
  int ret = local_user_->publishVideo(videoTrack);
}

void LocalUserWrapper::UnpublishAudioTrack(
    agora::agora_refptr<agora::rtc::ILocalAudioTrack> audioTrack) {
  local_user_->unpublishAudio(audioTrack);
}

void LocalUserWrapper::UnpublishVideoTrack(
    agora::agora_refptr<agora::rtc::ILocalVideoTrack> videoTrack) {
  local_user_->unpublishVideo(videoTrack);
}

void LocalUserWrapper::onUserAudioTrackSubscribed(
    agora::user_id_t userId, agora::agora_refptr<agora::rtc::IRemoteAudioTrack> audioTrack) {
  std::lock_guard<std::mutex> _(observer_lock_);
  remote_audio_track_ = audioTrack;
  if (remote_audio_track_ && media_packet_receiver_) {
    remote_audio_track_->registerMediaPacketReceiver(media_packet_receiver_);
  }
  if (remote_audio_track_ && audio_frame_observer_) {
    local_user_->registerAudioFrameObserver(audio_frame_observer_);
  }
}

void LocalUserWrapper::onUserVideoTrackSubscribed(
    agora::user_id_t userId, agora::rtc::VideoTrackInfo trackInfo,
    agora::agora_refptr<agora::rtc::IRemoteVideoTrack> videoTrack) {
  std::lock_guard<std::mutex> _(observer_lock_);
  remote_video_track_ = videoTrack;
  ::PostMessage(m_msgReceiver, MULTI_CAMERA_SUBSCRIBE_VIDEO_TRACKS, 0, (LPARAM)videoTrack.get());
  if (remote_video_track_ && video_encoded_receiver_) {
    remote_video_track_->registerVideoEncodedImageReceiver(video_encoded_receiver_);
  }
  if (remote_video_track_ && media_packet_receiver_) {
    remote_video_track_->registerMediaPacketReceiver(media_packet_receiver_);
  }
  if (remote_video_track_ && video_frame_observer_ && media_node_factory_) {
    /*observable_video_sink_ =
        media_node_factory_->createObservableVideoSink(video_frame_observer_, trackInfo);
    if (observable_video_sink_) {
      AG_LOG(INFO, "Add observable video sink");
      remote_video_track_->addRenderer(observable_video_sink_);
    }*/
  }
}

void LocalUserWrapper::SetMsgReceiver(HWND hwnd)
{
    m_msgReceiver = hwnd;
}