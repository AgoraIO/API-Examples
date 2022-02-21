//
//  Agora Engine SDK
//
//  Created by minbo in 2019-10.
//  Copyright (c) 2019 Agora.io. All rights reserved.

/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#pragma once  // NOLINT(build/header_guard)

// external key
/**
 * set the range of ports available for connection
 * @example "{\"rtc.udp_port_range\":[4500, 5000]}"
 */
#define KEY_RTC_UDP_PORT_RANGE                       "rtc.udp_port_range"
/**
 * set the list of ports available for connection
 * @example  "{\"rtc.udp_port_list\":[4501, 4502, 4503, 4504, 4505, 4506]}"
 */
#define KEY_RTC_UDP_PORT_LIST                        "rtc.udp_port_list"

/**
 * get the fd of sending socket available for connection
 * note: set method is not supported.
 */
#define KEY_RTC_UDP_SEND_FD                          "rtc.udp_send_fd"

 /**
  * set the video encoder mode (hardware or software)
  */
#define KEY_RTC_VIDEO_ENABLED_HW_ENCODER             "engine.video.enable_hw_encoder"

 /**
  * set the hardware video encoder provider (nv for nvidia or qsv for intel)
  */
#define KEY_RTC_VIDEO_HW_ENCODER_PROVIDER            "engine.video.hw_encoder_provider"

  /**
  * set the video decoder mode (hardware or software)
  */
#define KEY_RTC_VIDEO_ENABLED_HW_DECODER             "engine.video.enable_hw_decoder"

 /**
  * set the hardware video decoder provider (h264_cuvid(default) or h264_qsv)
  */
#define KEY_RTC_VIDEO_HW_DECODER_PROVIDER            "engine.video.hw_decoder_provider"

 /**
  * override the lua policy
  */
#define KEY_RTC_VIDEO_OVERRIDE_SMALLVIDEO_NOT_USE_HWENC_POLICY  "engine.video.override_smallvideo_not_use_hwenc_policy"

/**
  * enable/disable video packet retransmission, enabled by default
 */
#define KEY_RTC_VIDEO_RESEND                         "rtc.video_resend"

/**
  * enable/disable audio packet retransmission, enabled by default
*/
#define KEY_RTC_AUDIO_RESEND                         "rtc.audio_resend"

/**
  * enable/disable agora aec logic, enabled by default
*/
#define KEY_RTC_AUDIO_ENABLE_AGORA_AEC               "rtc.audio.aec.enable"

/**
* update aec aggressiveness level
*/
#define KEY_RTC_AUDIO_AEC_AGGRESSIVENESS             "rtc.audio.aec.aggressiveness"

#define KEY_RTC_AUDIO_MAX_TARGET_DELAY               "rtc.audio.max_target_delay"

/**
  * enable/disable agora agc logic, enabled by default
*/
#define KEY_RTC_AUDIO_ENABLE_AGORA_AGC               "rtc.audio.agc.enable"

/**
  * enable/disable agora ans logic, enabled by default
*/
#define KEY_RTC_AUDIO_ENABLE_AGORA_ANS               "rtc.audio.ans.enable"

/**
  * enable/disable agora md logic, enabled by default
*/
#define KEY_RTC_AUDIO_ENABLE_AGORA_MD                "rtc.audio.md.enable"

/**
  * set the OpenSL mode for Android audio
*/
#define KEY_RTC_AUDIO_OPENSL_MODE                    "rtc.audio.opensl.mode"

/**
  * enable/disable oboe, disabled by default
  * @example
  * enable oboe    : "{\"rtc.audio.oboe.enable\": true}"
  * disable oboe   : "{\"rtc.audio.oboe.enable\": false}"
*/
#define KEY_RTC_AUDIO_OBOE_ENABLE                    "rtc.audio.oboe.enable"

/**
  * enable/disable hardware ear monitor, enabled by default
  * @example "{\"rtc.audio.enable_hardware_ear_monitor\":true}"
*/
#define KEY_RTC_AUDIO_ENABLE_HARDWARE_EAR_MONITOR    "rtc.audio.enable_hardware_ear_monitor"

/**
  * set the adm input sample rate
*/
#define KEY_RTC_AUDIO_INPUT_SAMPLE_RATE              "rtc.audio.input_sample_rate"

/**
  * force use media volume or not
  * @example
  * force use media volume           : "{\"rtc.audio.force_use_media_volume\": true}"
  * force use communication volume   : "{\"rtc.audio.force_use_media_volume\": false}"
*/
#define KEY_RTC_AUDIO_FORCE_USE_MEDIA_VOLUME         "rtc.audio.force_use_media_volume"

/**
  * set the bluetooth a2dp Protocol for audio, disabled by default
  * @example
  * force use a2dp Protocol         : "{\"rtc.audio.force_bluetooth_a2dp\": true}"
  * force use hfp Protocol          : "{\"rtc.audio.force_bluetooth_a2dp\": false}"
*/
#define KEY_RTC_AUDIO_FORCE_BLUETOOTH_A2DP           "rtc.audio.force_bluetooth_a2dp"

/**
  * set the adm keep audiosession
*/
#define KEY_RTC_AUDIO_KEEP_AUDIOSESSION              "rtc.audio.keep.audiosession"

/**
  * use media volume in bluetooth
  * @example
  * use media volume in bluetooth              : "{\"rtc.audio.use_media_volume_in_bluetooth\": 1}"
  * use communication volume in bluetooth      : "{\"rtc.audio.use_media_volume_in_bluetooth\": 0}"
*/
#define KEY_RTC_AUDIO_USE_MEDIA_VOLUME_IN_BLUETOOTH  "rtc.audio.use_media_volume_in_bluetooth"

/**
  * set the bitrate ratio for video
*/
#define KEY_RTC_VIDEO_BITRATE_ADJUST_RATIO           "rtc.video.bitrate_adjust_ratio"

/**
  * set the minbitrate / bitrate ratio for video
*/
#define KEY_RTC_VIDEO_MINBITRATE_RATIO               "rtc.video.minbitrate_ratio"

/**
  * set the degradation preference
*/
#define KEY_RTC_VIDEO_DEGRADATION_PREFERENCE         "rtc.video.degradation_preference"

/**
  * set the degradation fps down step
*/

#define KEY_RTC_VIDEO_DEGRADATION_FPS_DOWN_STEP      "rtc.video.degradation_fps_down_step"
/**
  * set the degradation fps up step
*/
#define KEY_RTC_VIDEO_DEGRADATION_FPS_UP_STEP        "rtc.video.degradation_fps_up_step"

/**
  * set the duration ms for connection lost callback
*/
#define KEY_RTC_CONNECTION_LOST_PERIOD               "rtc.connection_lost_period"

/**
  * set the local ip
*/
#define KEY_RTC_LOCAL_IP                             "rtc.local.ip"

/**
  * set the network interface
*/
#define KEY_RTC_NETWORK_INTERFACE                    "rtc.network.interface"

/**
 * set the video codec type, such as "H264", "JPEG"
 */
#define KEY_RTC_VIDEO_CODEC_TYPE                      "engine.video.codec_type"

/**
  * only use average QP for quality scaling
*/
#define KEY_RTC_VIDEO_QUALITY_SCALE_ONLY_ON_AVERAGE_QP "engine.video.quality_scale_only_on_average_qp"

/**
  * low bound for quality scaling
*/
#define KEY_RTC_VIDEO_H264_QP_THRESHOLD_LOW         "engine.video.h264_qp_thresholds_low"

/**
  * high bound for quality scaling
*/
#define KEY_RTC_VIDEO_H264_QP_THRESHOLD_HIGH        "engine.video.h264_qp_thresholds_high"


namespace agora {

namespace util {
template <class T>
class CopyableAutoPtr;

class IString;
typedef CopyableAutoPtr<IString> AString;
}  // namespace util

namespace base {

class IAgoraParameter {
public:
  /**
   * release the resource
   */
  virtual void release() = 0;

  /**
   * set bool value of the json
   * @param [in] key
   *        the key name
   * @param [in] value
   *        the value
   * @return return 0 if success or an error code
   */
  virtual int setBool(const char* key, bool value) = 0;

  /**
   * set int value of the json
   * @param [in] key
   *        the key name
   * @param [in] value
   *        the value
   * @return return 0 if success or an error code
   */
  virtual int setInt(const char* key, int value) = 0;

  /**
   * set unsigned int value of the json
   * @param [in] key
   *        the key name
   * @param [in] value
   *        the value
   * @return return 0 if success or an error code
   */
  virtual int setUInt(const char* key, unsigned int value) = 0;

  /**
   * set double value of the json
   * @param [in] key
   *        the key name
   * @param [in] value
   *        the value
   * @return return 0 if success or an error code
   */
  virtual int setNumber(const char* key, double value) = 0;

  /**
   * set string value of the json
   * @param [in] key
   *        the key name
   * @param [in] value
   *        the value
   * @return return 0 if success or an error code
   */
  virtual int setString(const char* key, const char* value) = 0;

  /**
   * set object value of the json
   * @param [in] key
   *        the key name
   * @param [in] value
   *        the value
   * @return return 0 if success or an error code
   */
  virtual int setObject(const char* key, const char* value) = 0;

  /**
   * set array value of the json
   * @param [in] key
   *        the key name
   * @param [in] value
   *        the value
   * @return return 0 if success or an error code
   */
  virtual int setArray(const char* key, const char* value) = 0;
  /**
   * get bool value of the json
   * @param [in] key
   *        the key name
   * @param [in, out] value
   *        the value
   * @return return 0 if success or an error code
   */
  virtual int getBool(const char* key, bool& value) = 0;

  /**
   * get int value of the json
   * @param [in] key
   *        the key name
   * @param [in, out] value
   *        the value
   * @return return 0 if success or an error code
   */
  virtual int getInt(const char* key, int& value) = 0;

  /**
   * get unsigned int value of the json
   * @param [in] key
   *        the key name
   * @param [in, out] value
   *        the value
   * @return return 0 if success or an error code
   */
  virtual int getUInt(const char* key, unsigned int& value) = 0;

  /**
   * get double value of the json
   * @param [in] key
   *        the key name
   * @param [in, out] value
   *        the value
   * @return return 0 if success or an error code
   */
  virtual int getNumber(const char* key, double& value) = 0;

  /**
   * get string value of the json
   * @param [in] key
   *        the key name
   * @param [in, out] value
   *        the value
   * @return return 0 if success or an error code
   */
  virtual int getString(const char* key, agora::util::AString& value) = 0;

  /**
   * get a child object value of the json
   * @param [in] key
   *        the key name
   * @param [in, out] value
   *        the value
   * @return return 0 if success or an error code
   */
  virtual int getObject(const char* key, agora::util::AString& value) = 0;

  /**
   * get array value of the json
   * @param [in] key
   *        the key name
   * @param [in, out] value
   *        the value
   * @return return 0 if success or an error code
   */
  virtual int getArray(const char* key, const char* args, agora::util::AString& value) = 0;

  /**
   * set parameters of the sdk or engine
   * @param [in] parameters
   *        the parameters
   * @return return 0 if success or an error code
   */
  virtual int setParameters(const char* parameters) = 0;

  virtual int convertPath(const char* filePath, agora::util::AString& value) = 0;

  virtual ~IAgoraParameter() {}
};

}  // namespace base
}  // namespace agora
