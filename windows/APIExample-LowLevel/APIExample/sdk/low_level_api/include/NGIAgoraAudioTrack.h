
// Copyright (c) 2018 Agora.io. All rights reserved

// This program is confidential and proprietary to Agora.io.
// And may not be copied, reproduced, modified, disclosed to others, published
// or used, in whole or in part, without the express prior written permission
// of Agora.io.

#pragma once  // NOLINT(build/header_guard)

#include "AgoraBase.h"

// FIXME(Ender): use this class instead of AudioSendStream as local track
namespace agora {
namespace rtc {
class IAudioTrackStateObserver;
class IAudioFilter;
class IAudioSinkBase;
class IMediaPacketReceiver;
/**
 * Properties of audio frames expected by a sink.
 *
 * @note The source determines the audio frame to be sent to the sink based on a variety of factors,
 * such as other sinks or the capability of the source.
 *
 */
struct AudioSinkWants {
  /** The sample rate of the audio frame to be sent to the sink. */
  int samplesPerSec;

  /** The number of audio channels of the audio frame to be sent to the sink. */
  size_t channels;

  AudioSinkWants() : samplesPerSec(0),
                     channels(0) {}
  AudioSinkWants(int sampleRate, size_t chs) : samplesPerSec(sampleRate),
                                               channels(chs) {}
};

/**
 * The IAudioTrack class.
 */
class IAudioTrack : public RefCountInterface {
public:
  /**
   * The position of the audio filter in audio frame.
   */
  enum AudioFilterPosition {
    /**
     * Work on the local playback
     */
    RecordingLocalPlayback,
    /**
     * Work on the post audio recording device.
     */
    PostAudioRecordingDevice,
    /**
     * Work on the post audio processing.
     */
    PostAudioProcessing,
  };

 public:
  /**
   * Adjusts the playback volume.
   * @param volume The playback volume. The value ranges between 0 and 100 (default).
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int adjustPlayoutVolume(int volume) = 0;

  /**
   * Gets the current playback volume.
   * @param volume A pointer to the playback volume.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int getPlayoutVolume(int* volume) = 0;

  /**
   * Adds an audio filter.
   *
   * By adding an audio filter, you can apply various audio effects to the audio, for example, voice change.
   * @param filter A pointer to the audio filter. See \ref agora::rtc::IAudioFilter "IAudioFilter".
   * @param position The position of the audio filter. See \ref agora::rtc::IAudioTrack::AudioFilterPosition "AudioFilterPosition".
   * @return
   * - `true`: Success.
   * - `false`: Failure.
   */
  virtual bool addAudioFilter(agora_refptr<IAudioFilter> filter, AudioFilterPosition position) = 0;
  /**
   * Removes the audio filter added by callling `addAudioFilter`.
   *
   * @param filter The pointer to the audio filter that you want to remove. See \ref agora::rtc::IAudioFilter "IAudioFilter".
   * @param position The position of the audio filter. See #AudioFilterPosition.
   * @return
   * - `true`: Success.
   * - `false`: Failure.
   */
  virtual bool removeAudioFilter(agora_refptr<IAudioFilter> filter, AudioFilterPosition position) = 0;

  /**
   * Gets the audio filter by its name.
   *
   * @param name The name of the audio filter.
   * @param position The position of the audio filter. See #AudioFilterPosition.
   * @return
   * - The pointer to the audio filter: Success.
   * - A null pointer: Failure.
   */
  virtual agora_refptr<IAudioFilter> getAudioFilter(const char *name, AudioFilterPosition position) const = 0;

  /**
   * Adds an audio sink to get PCM data from the audio track.
   *
   * @param sink The pointer to the audio sink. See \ref agora::rtc::IAudioSinkBase "IAudioSinkBase".
   * @param wants The properties an audio frame should have when it is delivered to the sink. See \ref agora::rtc::AudioSinkWants "AudioSinkWants".
   * @return
   * - `true`: Success.
   * - `false`: Failure.
   */
  virtual bool addAudioSink(agora_refptr<IAudioSinkBase> sink, const AudioSinkWants& wants) = 0;

  /**
   * Removes an audio sink.
   *
   * @param sink The pointer to the audio sink to be removed. See \ref agora::rtc::IAudioSinkBase "IAudioSinkBase".
   * @return
   * - `true`: Success.
   * - `false`: Failure.
   */
  virtual bool removeAudioSink(agora_refptr<IAudioSinkBase> sink) = 0;
};

/**
 * `ILocalAudioTrack` is the basic class for local audio tracks, providing main methods of local audio tracks.
 *
 * You can create a local audio track by calling one of the following methods:
 * - `createLocalAudioTrack`
 * - `createCustomAudioTrack`
 * - `createMediaPlayerAudioTrack`
 * @if (!Linux)
 * You can also use the APIs in the \ref agora::rtc::INGAudioDeviceManager "IAudioDeviceManager" class if multiple recording devices are available in the system.
 * @endif
 *
 * After creating local audio tracks, you can publish one or more local audio
 * tracks by calling \ref agora::rtc::ILocalUser::publishAudio "publishAudio".
 */
class ILocalAudioTrack : public IAudioTrack {
 public:
  /**
   * Statistics of a local audio track.
   */
  struct LocalAudioTrackStats {
    /**
     * The source ID of the local audio track.
     */
    uint32_t source_id;
    /**
     * The number of audio frames in the buffer.
     *
     * When sending PCM data, the PCM data is first stored in a buffer area.
     * Then a thread gets audio frames from the buffer and sends PCM data.
     */
    uint32_t buffered_pcm_data_list_size;
    /**
     * The number of audio frames missed by the thread that gets PCM data from the buffer.
     */
    uint32_t missed_audio_frames;
    /**
     * The number of audio frames sent by the thread that gets PCM data from the buffer.
     */
    uint32_t sent_audio_frames;
    /**
     * The number of audio frames sent by the user.
     */
    uint32_t pushed_audio_frames;
    /**
     * The number of dropped audio frames caused by insufficient buffer size.
     */
    uint32_t dropped_audio_frames;
    /**
     * The type of audio effect.
     */
    uint32_t effect_type;
    /**
     * Whether the local audio track is enabled.
     */
    bool enabled;

    LocalAudioTrackStats() : source_id(0),
                             buffered_pcm_data_list_size(0),
                             missed_audio_frames(0),
                             sent_audio_frames(0),
                             pushed_audio_frames(0),
                             dropped_audio_frames(0),
                             effect_type(0),
                             enabled(false) {}
  };

 public:
  /**
   * Enables or disables the local audio track.
   *
   * Once the local audio is enabled, the SDK allows for local audio capturing, processing, and encoding.
   *
   * @param enable Whether to enable the audio track:
   * - `true`: Enable the local audio track.
   * - `false`: Disable the local audio track.
   */
  virtual void setEnabled(bool enable) = 0;

  /**
   * Gets whether the local audio track is enabled.
   * @return Whether the local audio track is enabled:
   * - `true`: The local track is enabled.
   * - `false`: The local track is disabled.
   */
  virtual bool isEnabled() const = 0;

  /**
   * Gets the state of the local audio.
   * @return The state of the local audio: #LOCAL_AUDIO_STREAM_STATE: Success.
   */
  virtual LOCAL_AUDIO_STREAM_STATE getState() = 0;

  /**
   * Gets the statistics of the local audio track: LocalAudioTrackStats.
   * @return The statistics of the local audio: LocalAudioTrackStats: Success.
   */
  virtual LocalAudioTrackStats GetStats() = 0;

  /**
   * Adjusts the audio volume for publishing.
   *
   * @param volume The volume for publishing. The value ranges between 0 and 100 (default).
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int adjustPublishVolume(int volume) = 0;

  /**
   * Gets the current volume for publishing.
   * @param volume A pointer to the publishing volume.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int getPublishVolume(int* volume) = 0;

  /**
   * Enables or disables local playback.
   * @param enable Whether to enable local playback:
   * - `true`: Enable local playback.
   * - `false`: Disable local playback.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int enableLocalPlayback(bool enable) = 0;
  /**
   * Enables in-ear monitoring (for Android and iOS only).
   *
   * @param enabled Determines whether to enable in-ear monitoring.
   * - true: Enable.
   * - false: (Default) Disable.
   * @param includeAudioFilters The type of the ear monitoring: #EAR_MONITORING_FILTER_TYPE
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int enableEarMonitor(bool enable, int includeAudioFilters) = 0;

 protected:
   ~ILocalAudioTrack() {}
};

/**
 * The statistics of a remote audio track.
 */
struct RemoteAudioTrackStats {
  /**
   * The user ID of the remote user who sends the audio track.
   */
  uid_t uid;
  /**
   * The audio quality of the remote audio track: #QUALITY_TYPE.
   */
  int quality;
  /**
   * The network delay (ms) from the sender to the receiver.
   */
  int network_transport_delay;
  /**
   * The delay (ms) from the receiver to the jitter buffer.
   */
  uint32_t jitter_buffer_delay;
  /**
   * The audio frame loss rate in the reported interval.
   */
  int audio_loss_rate;
  /**
   * The number of audio channels.
   */
  int num_channels;
  /**
   * The sample rate (Hz) of the received audio track in the reported interval.
   */
  int received_sample_rate;
  /**
   * The average bitrate (Kbps) of the received audio track in the reported interval.
   * */
  int received_bitrate;
  /**
   * The total freeze time (ms) of the remote audio track after the remote user joins the channel.
   * In a session, audio freeze occurs when the audio frame loss rate reaches 4%.
   * The total audio freeze time = The audio freeze number × 2 seconds.
   */
  int total_frozen_time;
  /**
   * The total audio freeze time as a percentage (%) of the total time when the audio is available.
   * */
  int frozen_rate;
  /**
   * The number of audio bytes received.
   */
  int64_t received_bytes;
  /**
   * The average packet waiting time (ms) in the jitter buffer.
   */
  int mean_waiting_time;
  /**
   * The samples of expanded speech.
   */
  size_t expanded_speech_samples;
  /**
   * The samples of expanded noise.
   */
  size_t expanded_noise_samples;
  /**
   * The timestamps since last report.
   */
  uint32_t timestamps_since_last_report;
  /**
   * The minimum sequence number.
   */
  uint16_t min_sequence_number;
  /**
   * The maximum sequence number.
   */
  uint16_t max_sequence_number;
  /**
   * The audio energy.
   */
  int32_t audio_level;
  /**
   * audio downlink average process time
   */
  uint32_t downlink_process_time_ms;
  /**
   *  The count of 80 ms frozen in 2 seconds
   */
  uint16_t frozen_count_80_ms;
  /**
   *  The time of 80 ms frozen in 2 seconds
   */
  uint16_t frozen_time_80_ms;
  /**
   *  The count of 200 ms frozen in 2 seconds
   */
  uint16_t frozen_count_200_ms;
  /**
   *  The time of 200 ms frozen in 2 seconds
   */
  uint16_t frozen_time_200_ms;
  /**
   *  The estimate delay
   */
  uint32_t delay_estimate_ms;
  /**
   *  The MOS value
   */
  uint32_t mos_value;

  RemoteAudioTrackStats() :
    uid(0),
    quality(0),
    network_transport_delay(0),
    jitter_buffer_delay(0),
    audio_loss_rate(0),
    num_channels(0),
    received_sample_rate(0),
    received_bitrate(0),
    total_frozen_time(0),
    frozen_rate(0),
    received_bytes(0),
    mean_waiting_time(0),
    expanded_speech_samples(0),
    expanded_noise_samples(0),
    timestamps_since_last_report(0),
    min_sequence_number(0xFFFF),
    max_sequence_number(0),
    audio_level(0),
    downlink_process_time_ms(0),
    frozen_count_80_ms(0),
    frozen_time_80_ms(0),
    frozen_count_200_ms(0),
    frozen_time_200_ms(0),
    delay_estimate_ms(0),
    mos_value(0) { }
};

/**
 * The IRemoteAudioTrack class.
 */
class IRemoteAudioTrack : public IAudioTrack {
 public:
  /**
   * Gets the statistics of the remote audio track.
   * @param stats A reference to the statistics of the remote audio track: RemoteAudioTrackStats.
   * @return
   * - `true`: Success.
   * - `false`: Failure.
   */
  virtual bool getStatistics(RemoteAudioTrackStats& stats) = 0;

  /**
   * Gets the state of the remote audio.
   * @return The state of the remote audio: #REMOTE_AUDIO_STATE.
   */
  virtual REMOTE_AUDIO_STATE getState() = 0;

  /**
   * Registers an `IMediaPacketReceiver` object.
   *
   * You need to implement the `IMediaPacketReceiver` class in this method. Once you successfully register
   * the media packet receiver, the SDK triggers the `onMediaPacketReceived` callback when it receives an
   * audio packet.
   *
   * @param packetReceiver The pointer to the `IMediaPacketReceiver` object.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int registerMediaPacketReceiver(IMediaPacketReceiver* packetReceiver) = 0;

  /**
   * Releases the `IMediaPacketReceiver` object.
   * @param packetReceiver The pointer to the `IMediaPacketReceiver` object.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int unregisterMediaPacketReceiver(IMediaPacketReceiver* packetReceiver) = 0;
  
  /** Sets the sound position and gain
   
   @param pan The sound position of the remote user. The value ranges from -1.0 to 1.0:
   - 0.0: the remote sound comes from the front.
   - -1.0: the remote sound comes from the left.
   - 1.0: the remote sound comes from the right.
   @param gain Gain of the remote user. The value ranges from 0.0 to 100.0. The default value is 100.0 (the original gain of the remote user). The smaller the value, the less the gain.
   
   @return
   - 0: Success.
   - < 0: Failure.
   */
  virtual int setRemoteVoicePosition(float pan, float gain) = 0;
};

}  // namespace rtc
}  // namespace agora
