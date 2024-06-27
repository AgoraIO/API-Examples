
// Copyright (c) 2022 Agora.io. All rights reserved

// This program is confidential and proprietary to Agora.io.
// And may not be copied, reproduced, modified, disclosed to others, published
// or used, in whole or in part, without the express prior written permission
// of Agora.io.
#pragma once  // NOLINT(build/header_guard)

#include "AgoraRefPtr.h"
#include "AgoraBase.h"
#include <api/cpp/aosl_ares_class.h>
namespace agora {

/**
 * The definition of the DataChannelConfig struct.
 */
struct DataChannelConfig {
  /**
   * Whether this channel should sync with media stream:
   * - true: sync .
   * - false: not sync.
   */
  bool syncWithMedia;
  /**
   * Whether this channel should ensure oredered message:
   * - true: orderd .
   * - false: not orderd.
   */
  bool ordered;

  /**
   * Whether this channel should compress the data packet:
   * - <= 0: We dont compress the data packet
   * - > 0: Once the packet length exceeds the compressionLength, we compress it.
   */
  int compressionLength;
  // optional
  Optional<int> channelId;  // 0~7

  /**
   * The priority
   */
  int32_t priority;
  DataChannelConfig() :
    syncWithMedia(false),
    ordered(false),
    compressionLength(0),
    priority(-1) {}
};

/**
 * The definition of the DataChannelInfo struct.
 */
struct DataChannelInfo {
  /**
   * The Id of the data channel
   */
  int dataChannelId;
  /**
   * The metaData of the data channel
   */
  util::AString metadata;
};

/**
 * The definition of the DataChannelInfo struct.
 */
struct UserDataChannelInfo {
  /**
   * The user Id of the data channel
   */
  util::AString userId;
  /**
   * The data channel infos 
   */
  const DataChannelInfo* infos;
  /**
   * The info size
   */
  size_t info_size;
};

class ILocalDataChannel : public RefCountInterface {
 public:
  /**
   * Send data packet to this data channel after publishing.
   *
   * @param [in] packet packet buffer pointer.
   * @param [in] length packet buffer length.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int sendDataPacket(const char* packet, size_t length, uint64_t capture_time_ms, aosl_ref_t ares = AOSL_REF_INVALID) = 0;
  /**
   * Send meta data to this data channel before publishing.
   *
   * @param [in] metaData meta data pointer.
   * @param [in] length meta data length.
   * @return····
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setMetaData(const char* metaData, size_t length, aosl_ref_t ares = AOSL_REF_INVALID) = 0;

  /**
   * return configured channel id
   *
   * @return
   */
  virtual Optional<int> configuredChannelId() const = 0;

 protected:
  virtual ~ILocalDataChannel() {}
};

class IRemoteDataChannel: public RefCountInterface {
 public:
  virtual util::AString UserId() const = 0;
  virtual int ChannelId() const = 0;
  virtual util::AString Meta() = 0;

 protected:
  virtual ~IRemoteDataChannel() {}
};

class IDataChannelObserver {
 public:
  /**
   * Occurs when the channe is ready to send the data packet.
   * @param channel the published channel.
   */
  virtual void onLocalDataChannelPublished(agora_refptr<ILocalDataChannel> channel) {}

  /**
   * Occurs when the the channe is added and ready to receive data packet.
   * @param channel the remote channel pointer.
   */
  virtual void onRemoteDataChannelSubscribed(agora_refptr<IRemoteDataChannel> channel) {}

  /**
   * Occurs when the the channe is removed.
   * @param channel the remote channel pointer.
   */
  virtual void onRemoteDataChannelUnsubscribed(agora_refptr<IRemoteDataChannel> channel) {}
  /**
   * Occurs when the packet is received.
   * @param info the channel Info.
   * @param packet the received packet.
   */
  virtual void onRemoteDataPacketReceived(const UserDataChannelInfo& info, util::AString packet) {}

  /**
   * Occurs when the remote data channel info updated.
   * @param modified_infos the modifed channel Infos, add or update.
   * @param modified_infos_size the size of modifed channel Infos .
   * @param deleted_infos the deleted channel Infos.
   * @param deleted_infos_size the size of deleted channel Infos .
   */
  virtual void onRemoteDataChannelInfoUpdated(const UserDataChannelInfo* modified_infos, size_t modified_infos_size,
                                              const UserDataChannelInfo* deleted_infos, size_t deleted_infos_size) {}
  virtual ~IDataChannelObserver() {}
};

}  // namespace agora
