//
//  Agora SDK
//
//  Copyright (c) 2022 Agora.io. All rights reserved.
//
#pragma once  // NOLINT(build/header_guard)

#include "AgoraBase.h"
#include "AgoraMediaBase.h"

namespace agora {
namespace rtc {

class IMediaRecorder : public RefCountInterface {
 protected:
  virtual ~IMediaRecorder() {}

 public:
  /**
   * @brief Registers the `IMediaRecorderObserver` observer.
   *
   * @since v4.0.0
   *
   * @details
   * This method sets the callback for audio and video recording, so the app can be notified of
   * recording status and information during the recording process.
   * Before calling this method, make sure that:
   * - The `IRtcEngine` object has been created and initialized.
   * - The media recorder object has been created using `createMediaRecorder`.
   *
   * @param callback Callback for audio and video stream recording. See `IMediaRecorderObserver`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure. See `Error Codes` for details and resolution suggestions.
   */
  virtual int setMediaRecorderObserver(media::IMediaRecorderObserver* callback) = 0;
  /**
   * @brief Starts audio and video stream recording.
   *
   * @since v4.0.0
   *
   * @details
   * This method starts recording audio and video streams. The Agora SDK supports recording both local
   * and remote users' audio and video streams simultaneously.
   * Before starting the recording, make sure that:
   * - You have created the media recorder object using `createMediaRecorder`.
   * - You have registered a recorder observer using `setMediaRecorderObserver` to listen for
   * recording callbacks.
   * - You have joined a channel.
   * This method supports recording the following data:
   * - Audio captured from the microphone in AAC encoding format.
   * - Video captured from the camera in H.264 or H.265 encoding format.
   * After recording starts, if the video resolution changes during recording, the SDK stops the
   * recording. If the audio sample rate or number of channels changes, the SDK continues recording
   * and generates a single MP4 file.
   * A recording file is only successfully generated when a recordable audio or video stream is
   * detected. If there is no recordable stream, or if the stream is interrupted for more than 5
   * seconds during recording, the SDK stops the recording and triggers the
   * `onRecorderStateChanged` (`RECORDER_STATE_ERROR, RECORDER_REASON_NO_STREAM`) callback.
   *
   * @note
   * - If you want to record local audio and video streams, make sure the local user role is set to
   * broadcaster before starting recording.
   * - If you want to record remote audio and video streams, make sure you have subscribed to the
   * remote user's streams before starting recording.
   *
   * @param config Audio and video stream recording configuration. See `MediaRecorderConfiguration`.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure. See `Error Codes` for details and resolution suggestions.
   *   - -2: Invalid parameter. Please ensure that:
   *     - The specified recording file path is correct and writable.
   *     - The specified recording file format is correct.
   *     - The maximum recording duration is set correctly.
   *   - -4: `IRtcEngine` is in a state that does not support this operation. This may be because a
   * recording is already in progress or has stopped due to an error.
   *   - -7: `IRtcEngine` is not initialized when this method is called. Please make sure the
   * `IMediaRecorder` object has been created before calling this method.
   */
  virtual int startRecording(const media::MediaRecorderConfiguration& config) = 0;
  /**
   * @brief Stops audio and video stream recording.
   *
   * @since v4.0.0
   *
   * @note After calling `startRecording`, you must call this method to stop the recording; otherwise,
   * the generated recording file may not play properly.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure:
   *   - -7: `IRtcEngine` is not initialized when this method is called. Please make sure the
   * `IMediaRecorder` object has been created before calling this method.
   */
  virtual int stopRecording() = 0;
};

} //namespace rtc
} // namespace agora
