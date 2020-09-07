//
//  Agora Media SDK
//
//  Copyright (c) 2018 Agora IO. All rights reserved.
//
#include "connection_api.h"

#include "api2/internal/agora_service_i.h"
#include "api2/internal/rtc_connection_i.h"
#include "call_engine/call_context.h"
#include "engine_adapter/video/video_node_interface.h"
#include "media/base/rtputils.h"
#include "rtc/packet_filter.h"
#include "utils/tools/util.h"

AsyncConnection::AsyncConnection(agora::base::IAgoraService* service, const Config& conn_config) {
  agora::rtc::RtcConnectionConfigurationEx config;
  config.autoSubscribeAudio = conn_config.auto_subscribe_audio;
  config.autoSubscribeVideo = conn_config.auto_subscribe_video;
  config.enableAudioRecordingOrPlayout = conn_config.enableAudioRecordingOrPlayout;
  config.audioSubscriptionOptions = conn_config.audio_sub_option;
  config.minPort = conn_config.min_port;
  config.maxPort = conn_config.max_port;
  config.clientType = conn_config.client_type;
  config.clientRoleType = conn_config.clientRoleType;
  config.recvType = conn_config.recvType;
  config.congestionControlType = conn_config.congestionControlType;
  config.channelProfile = conn_config.channelProfile;
#if defined(FEATURE_P2P)
  config.is_p2p_switch_enabled = conn_config.is_p2p_switch_enabled;
#endif
  agora::base::IAgoraServiceEx* serviceEx =
      reinterpret_cast<agora::base::IAgoraServiceEx*>(service);
  connection_ = serviceEx->createRtcConnectionEx(config);

  connection_->registerObserver(this);
  // make sure local_user exist
  connection_->getLocalUser()->setUserRole(agora::rtc::CLIENT_ROLE_BROADCASTER);
  local_user_ = agora::commons::make_unique<AsyncLocalUser>(connection_->getLocalUser());

  if (conn_config.internal_vos) {
    agora::base::IAgoraParameter* agoraParameter = connection_->getAgoraParameter();
    agoraParameter->setParameters(PRIORITY_VOS_LIST);
  }
}

AsyncConnection::~AsyncConnection() {
  local_user_.reset();
  connection_->unregisterObserver(this);
  connection_ = nullptr;
}

AsyncResult<ConnectResult>& AsyncConnection::Connect(const char* token, const char* channelId,
                                                     agora::user_id_t userId) {
  conn_start_time_ = agora::commons::tick_ms();
  connection_->connect(token, channelId, userId);
  return connect_result_;
}

AsyncResult<DisconnectResult>& AsyncConnection::Disconnect() {
  disconn_start_time_ = agora::commons::tick_ms();
  connection_->disconnect();
  return disconnect_result_;
}

AsyncLocalUserType AsyncConnection::GetLocalUser() {
  if (!connection_) return nullptr;
  return local_user_.get();
}

void AsyncConnection::FakeVideoStream(uint32_t uid, uint32_t ssrc) {
#if defined(FEATURE_VIDEO)
  unsigned char fake_rtp_head[12] = {0};
  fake_rtp_head[0] = 0x80;
  fake_rtp_head[1] = 0xff;

  cricket::SetRtpSsrc(fake_rtp_head, 12, ssrc);
  agora::rtc::VideoStreamProperty ext;
  ext.cid = 0;
  ext.uid = uid;
  ext.stream_id = 0;
  ext.track_id = 0;
  ext.ssrc = ssrc;
  auto data = agora::rtc::PackVideoStreamPropertyData<agora::rtc::VideoStreamProperty>(&ext);
  agora::rtc::IRtcConnectionEx* ex = static_cast<agora::rtc::IRtcConnectionEx*>(connection_.get());
  agora::rtc::video_packet_t pkt;
  pkt.extension.has_extension_ = true;
  pkt.extension.tag_ = 1;
  pkt.extension.content_.resize(data.size() / 4);
  memcpy(pkt.extension.content_.data(), &data[0], data.size());
  pkt.uid = uid;
  pkt.payload.assign(reinterpret_cast<char*>(&fake_rtp_head[0]), 12);
  ex->getIOWorker()->sync_call(LOCATION_HERE, [this, ex, &pkt] {
    ex->getVideoPacketFilter()->onFilterVideoPacket(pkt);
    return 0;
  });
#endif
}

agora::rtc::RtcStats AsyncConnection::GetStats() { return connection_->getTransportStats(); }

AsyncResult<LowLevelClientRoleChangedResult>& AsyncConnection::SetClientRole(
    agora::rtc::CLIENT_ROLE_TYPE role) {
  connection_->getLocalUser()->setUserRole(role);
  return client_role_changed_result_;
}

uint64_t AsyncConnection::GetStatsSpace() {
  agora::rtc::IRtcConnectionEx* connectionEx =
      reinterpret_cast<agora::rtc::IRtcConnectionEx*>(connection_.get());
  return connectionEx->getCallContext()->getSpaceId();
}

void AsyncConnection::SetVos(const char* addr, int port) {
  agora::rtc::IRtcConnectionEx* ex = static_cast<agora::rtc::IRtcConnectionEx*>(connection_.get());
  ex->setVos(addr, port);
}

void AsyncConnection::onConnected(const agora::rtc::TConnectionInfo& connectionInfo,
                                  agora::rtc::CONNECTION_CHANGED_REASON_TYPE reason) {
  conn_done_time_ = agora::commons::tick_ms();
  connect_result_.AppendResult(
      {true, false, connectionInfo, static_cast<agora::rtc::CONNECTION_CHANGED_REASON_TYPE>(1)});
}

void AsyncConnection::onDisconnected(const agora::rtc::TConnectionInfo& connectionInfo,
                                     agora::rtc::CONNECTION_CHANGED_REASON_TYPE reason) {
  disconn_done_time_ = agora::commons::tick_ms();
  disconnect_result_.AppendResult({connectionInfo});
}

void AsyncConnection::onConnecting(const agora::rtc::TConnectionInfo& connectionInfo,
                                   agora::rtc::CONNECTION_CHANGED_REASON_TYPE reason) {
  connecting_result_.AppendResult({connectionInfo, reason});
}

void AsyncConnection::onReconnecting(const agora::rtc::TConnectionInfo& connectionInfo,
                                     agora::rtc::CONNECTION_CHANGED_REASON_TYPE reason) {}

void AsyncConnection::onReconnected(const agora::rtc::TConnectionInfo& connectionInfo,
                                    agora::rtc::CONNECTION_CHANGED_REASON_TYPE reason) {
  conn_done_time_ = agora::commons::tick_ms();
  connect_result_.AppendResult(
      {true, true, connectionInfo, static_cast<agora::rtc::CONNECTION_CHANGED_REASON_TYPE>(1)});
}

void AsyncConnection::onLastmileQuality(const agora::rtc::QUALITY_TYPE quality) {
  lastmile_quality_result_.AppendResult({quality});
}

void AsyncConnection::onLastmileProbeResult(const agora::rtc::LastmileProbeResult& result) {
  lastmile_probe_result_.AppendResult({result});
}

void AsyncConnection::onTokenPrivilegeWillExpire(const char* token) {
  token_will_expire_result_.AppendResult({token});
}

void AsyncConnection::onTokenPrivilegeDidExpire() {}

void AsyncConnection::onConnectionFailure(const agora::rtc::TConnectionInfo& connectionInfo,
                                          agora::rtc::CONNECTION_CHANGED_REASON_TYPE reason) {
  conn_done_time_ = agora::commons::tick_ms();
  connect_result_.AppendResult({false, false, connectionInfo, reason});
}

void AsyncConnection::onChannelMediaRelayStateChanged(int state, int code) {}

void AsyncConnection::onConnectionLost(const agora::rtc::TConnectionInfo& connectionInfo) {}

void AsyncConnection::onUserJoined(agora::user_id_t userId) {
  remote_user_connect_result_.AppendResult({userId});
}

void AsyncConnection::onUserLeft(agora::user_id_t userId,
                                 agora::rtc::USER_OFFLINE_REASON_TYPE reason) {
  remote_user_disconnect_result_.AppendResult({userId, reason});
}

void AsyncConnection::onTransportStats(const agora::rtc::RtcStats& stats) {
  transport_state_result_.SetResult({stats});
}

void AsyncConnection::onChangeRoleSuccess(agora::rtc::CLIENT_ROLE_TYPE oldRole,
                                          agora::rtc::CLIENT_ROLE_TYPE newRole) {
  client_role_changed_result_.AppendResult({oldRole, newRole});
}

void AsyncConnection::onChangeRoleFailure() {}

void AsyncConnection::onError(agora::ERROR_CODE_TYPE error, const char* msg) {}

void AsyncConnection::onWarning(agora::WARN_CODE_TYPE warning, const char* msg) {}
