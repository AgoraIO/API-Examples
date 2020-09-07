//
//  Agora Media SDK
//
//  Copyright (c) 2018 Agora IO. All rights reserved.
//
#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "api2/IAgoraService.h"
#include "api2/NGIAgoraRtcConnection.h"
#include "api2/internal/rtc_connection_i.h"
#include "api2/NGIAgoraVideoTrack.h"
#include "local_user_api.h"
#include "test_base/test_utils.h"

struct ConnectResult {
  bool is_success;
  bool is_reconnect;
  agora::rtc::TConnectionInfo info;
  agora::rtc::CONNECTION_CHANGED_REASON_TYPE reason;
};

struct DisconnectResult {
  agora::rtc::TConnectionInfo info;
};

struct ConnectingResult {
  agora::rtc::TConnectionInfo info;
  agora::rtc::CONNECTION_CHANGED_REASON_TYPE reason;
};

struct LastmileProbeQualityResult {
  agora::rtc::QUALITY_TYPE quality;
};

struct LastmileProbeResult {
  agora::rtc::LastmileProbeResult result = {
      agora::rtc::LASTMILE_PROBE_RESULT_UNAVAILABLE, {0, 0, 0}, {0, 0, 0}, 0};
};

struct TokenPrivilegeWillExpireResult {
  std::string token;
};

struct RemoteUserConnectResult {
  std::string userId;
};

struct RemoteUserDisconnectResult {
  std::string userId;
  agora::rtc::USER_OFFLINE_REASON_TYPE reason;
};

struct TransportStateResult {
  agora::rtc::RtcStats stats;
};

struct LowLevelClientRoleChangedResult {
  agora::rtc::CLIENT_ROLE_TYPE old;
  agora::rtc::CLIENT_ROLE_TYPE _new;
};

class AsyncConnection : public agora::rtc::IRtcConnectionObserver {
 public:
  struct Config {
    bool internal_vos = g_use_internal_vos;
    int client_type = 0;
    bool auto_subscribe_audio = true;
    bool auto_subscribe_video = true;
    bool enableAudioRecordingOrPlayout = true;
    int min_port = 0;
    int max_port = 0;
    agora::rtc::AudioSubscriptionOptions audio_sub_option;
    agora::rtc::CLIENT_ROLE_TYPE clientRoleType = agora::rtc::CLIENT_ROLE_BROADCASTER;
    agora::rtc::RECV_TYPE recvType = agora::rtc::RECV_MEDIA_ONLY;
    agora::rtc::CongestionControlType congestionControlType = agora::rtc::CONGESTION_CONTROLLER_TYPE_AGORA_CC;
    agora::CHANNEL_PROFILE_TYPE channelProfile = agora::CHANNEL_PROFILE_COMMUNICATION;
#if defined(FEATURE_P2P)
    bool is_p2p_switch_enabled = false;
#endif
  };

 public:
  AsyncConnection(agora::base::IAgoraService* service, const Config& config);
  ~AsyncConnection();

  AsyncResult<ConnectResult>& Connect(const char* token, const char* channelId,
                                      agora::user_id_t userId);

  AsyncResult<DisconnectResult>& Disconnect();

  AsyncResult<LastmileProbeQualityResult>& GetQualityResult() { return lastmile_quality_result_; }

  AsyncResult<ConnectingResult>& GetConnectingResult() { return connecting_result_; }

  AsyncResult<TokenPrivilegeWillExpireResult>& GetTokenPrivilegeWillExpireResult() {
    return token_will_expire_result_;
  }

  AsyncResult<RemoteUserConnectResult>& GetRemoteUserConnectResult() {
    return remote_user_connect_result_;
  }

  AsyncResult<RemoteUserDisconnectResult>& GetRemoteUserDisconnectResult() {
    return remote_user_disconnect_result_;
  }

  AsyncResult<TransportStateResult>& GetTransportStateResult() { return transport_state_result_; }

  AsyncLocalUserType GetLocalUser();

  void FakeVideoStream(uint32_t uid, uint32_t ssrc);

  agora::rtc::RtcStats GetStats();

  AsyncResult<LowLevelClientRoleChangedResult>& SetClientRole(agora::rtc::CLIENT_ROLE_TYPE role);

  void SetVos(const char* addr, int port = 4001);

  agora::agora_refptr<agora::rtc::IRtcConnection> RawConnection() { return connection_; }

  uint64_t GetConnStartTime() { return conn_start_time_; }
  uint64_t GetConnDoneTime() { return conn_done_time_; }
  uint64_t GetDisconnStartTime() { return disconn_start_time_; }
  uint64_t GetDisconnDoneTime() { return disconn_done_time_; }

  uint64_t GetStatsSpace();

 private:
  std::unique_ptr<AsyncLocalUser> local_user_;
  agora::agora_refptr<agora::rtc::IRtcConnection> connection_;
  AsyncResult<ConnectResult> connect_result_;
  AsyncResult<DisconnectResult> disconnect_result_;
  AsyncResult<ConnectingResult> connecting_result_;
  AsyncResult<LastmileProbeQualityResult> lastmile_quality_result_;
  AsyncResult<LastmileProbeResult> lastmile_probe_result_;
  AsyncResult<TokenPrivilegeWillExpireResult> token_will_expire_result_;
  AsyncResult<RemoteUserConnectResult> remote_user_connect_result_;
  AsyncResult<RemoteUserDisconnectResult> remote_user_disconnect_result_;
  AsyncResult<TransportStateResult> transport_state_result_;
  AsyncResult<LowLevelClientRoleChangedResult> client_role_changed_result_;

  uint64_t conn_start_time_ = 0;
  uint64_t conn_done_time_ = 0;
  uint64_t disconn_start_time_ = 0;
  uint64_t disconn_done_time_ = 0;

 private:
  void onConnected(const agora::rtc::TConnectionInfo& connectionInfo,
                   agora::rtc::CONNECTION_CHANGED_REASON_TYPE reason) override;

  void onDisconnected(const agora::rtc::TConnectionInfo& connectionInfo,
                      agora::rtc::CONNECTION_CHANGED_REASON_TYPE reason) override;

  void onConnecting(const agora::rtc::TConnectionInfo& connectionInfo,
                    agora::rtc::CONNECTION_CHANGED_REASON_TYPE reason) override;

  void onReconnecting(const agora::rtc::TConnectionInfo& connectionInfo,
                      agora::rtc::CONNECTION_CHANGED_REASON_TYPE reason) override;

  void onReconnected(const agora::rtc::TConnectionInfo& connectionInfo,
                     agora::rtc::CONNECTION_CHANGED_REASON_TYPE reason) override;

  void onLastmileQuality(const agora::rtc::QUALITY_TYPE quality) override;

  void onLastmileProbeResult(const agora::rtc::LastmileProbeResult& result) override;

  void onTokenPrivilegeWillExpire(const char* token) override;

  void onTokenPrivilegeDidExpire() override;

  void onChannelMediaRelayStateChanged(int state, int code) override;

  void onConnectionFailure(const agora::rtc::TConnectionInfo& connectionInfo,
                           agora::rtc::CONNECTION_CHANGED_REASON_TYPE reason) override;

  void onConnectionLost(const agora::rtc::TConnectionInfo& connectionInfo) override;
  void onUserJoined(agora::user_id_t userId) override;

  void onUserLeft(agora::user_id_t userId, agora::rtc::USER_OFFLINE_REASON_TYPE reason) override;

  void onTransportStats(const agora::rtc::RtcStats& stats) override;

  void onChangeRoleSuccess(agora::rtc::CLIENT_ROLE_TYPE oldRole,
                           agora::rtc::CLIENT_ROLE_TYPE newRole) override;

  void onChangeRoleFailure() override;

  void onError(agora::ERROR_CODE_TYPE error, const char* msg) override;

  void onWarning(agora::WARN_CODE_TYPE warning, const char* msg) override;
};

typedef std::shared_ptr<AsyncConnection> AsyncConnectionType;
