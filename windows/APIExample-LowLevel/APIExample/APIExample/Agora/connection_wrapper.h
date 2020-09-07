//  Agora RTC/MEDIA SDK
//
//  Created by Pengfei Han in 2020-03.
//  Copyright (c) 2020 Agora.io. All rights reserved.
//

#pragma once

#include "AgoraBase.h"
#include "IAgoraService.h"
#include "NGIAgoraRtcConnection.h"
#include "utils/common_utils.h"
#include "local_user_wrapper.h"

#include <map>

struct ConnectionConfig {
  bool play = false;
  int minPort = 0;
  int maxPort = 0;
  size_t numberOfChannels_ = 2;
  uint32_t sampleRateHz_ = 48000;
  agora::rtc::CLIENT_ROLE_TYPE clientRoleType = agora::rtc::CLIENT_ROLE_BROADCASTER;
  bool subscribeAllAudio = false;
  bool subscribeAllVideo = false;
  agora::rtc::REMOTE_VIDEO_STREAM_TYPE type = agora::rtc::REMOTE_VIDEO_STREAM_HIGH;
  bool encodedFrameOnly = false;
  agora::CHANNEL_PROFILE_TYPE channelProfile = agora::CHANNEL_PROFILE_LIVE_BROADCASTING;
  agora::rtc::RECV_TYPE recv_type = agora::rtc::RECV_MEDIA_ONLY;
};

static const int DefaultConnectWaitTime = 3000;

class ConnectionWrapper : public agora::rtc::IRtcConnectionObserver {
 public:
  ConnectionWrapper(agora::agora_refptr<agora::rtc::IRtcConnection> connection,
                    const ConnectionConfig& config);
  virtual ~ConnectionWrapper();

  bool Connect(const char* appid, const char* channelId, agora::user_id_t userId, int waitMs = DefaultConnectWaitTime);
  bool Disconnect(int waitMs = DefaultConnectWaitTime);
  std::shared_ptr<LocalUserWrapper> GetLocalUser();

  int CreateDataStream(int &streamId, bool reliable, bool ordered);
  int SendStreamMessage(int streamId, const char *data, size_t length);
  int sendIntraRequest(int uid);

  agora::rtc::TConnectionInfo getConnectionInfo();

  bool getDataStreamStats(agora::user_id_t userId, int streamId, DataStreamResult& result);

  void clearDataStreamStats() {
    data_stream_stats_.clear();
  }
 
 public:
  // inherit from agora::rtc::IRtcConnectionObserver
  void onConnected(const agora::rtc::TConnectionInfo& connectionInfo,
                   agora::rtc::CONNECTION_CHANGED_REASON_TYPE reason) override;

  void onDisconnected(const agora::rtc::TConnectionInfo& connectionInfo,
                      agora::rtc::CONNECTION_CHANGED_REASON_TYPE reason) override;

  void onReconnecting(const agora::rtc::TConnectionInfo& connectionInfo,
                      agora::rtc::CONNECTION_CHANGED_REASON_TYPE reason) override {}

  void onReconnected(const agora::rtc::TConnectionInfo& connectionInfo,
                     agora::rtc::CONNECTION_CHANGED_REASON_TYPE reason) override {}

   void onConnecting(const agora::rtc::TConnectionInfo& connectionInfo, agora::rtc::CONNECTION_CHANGED_REASON_TYPE reason){}

  void onConnectionLost(const agora::rtc::TConnectionInfo& connectionInfo) override {}

  void onLastmileQuality(const agora::rtc::QUALITY_TYPE quality) override {}

  void onTokenPrivilegeWillExpire(const char* token) override {}

  void onTokenPrivilegeDidExpire() override {}

  void onConnectionFailure(const agora::rtc::TConnectionInfo& connectionInfo,
                           agora::rtc::CONNECTION_CHANGED_REASON_TYPE reason) override {}

  void onUserJoined(agora::user_id_t userId) override {}

  void onUserLeft(agora::user_id_t userId, agora::rtc::USER_OFFLINE_REASON_TYPE reason) override {}

  void onTransportStats(const agora::rtc::RtcStats& stats) override {}

  void onLastmileProbeResult(const agora::rtc::LastmileProbeResult& result) override {}

  void onChannelMediaRelayStateChanged(int state, int code) override {}

  void onStreamMessage(agora::user_id_t userId, int streamId, const char *data, size_t length) override;

  void onStreamMessageError(agora::user_id_t userId, int streamId, int code, int missed, int cached) override;

 public:
  static std::shared_ptr<ConnectionWrapper> CreateConnection(agora::base::IAgoraService* service,
                                                             const ConnectionConfig& config);
  void SetMsgReceiver(HWND hwnd);
 private:
  agora::agora_refptr<agora::rtc::IRtcConnection> connection_;
  std::shared_ptr<LocalUserWrapper> lcoal_user_wrapper_;
  bool connected_;
  AutoResetEvent connect_ready_;
  AutoResetEvent disconnect_ready_;
  std::map<std::pair<unsigned int, int>, DataStreamResult> data_stream_stats_;

  HWND m_msgReceiver;
};
