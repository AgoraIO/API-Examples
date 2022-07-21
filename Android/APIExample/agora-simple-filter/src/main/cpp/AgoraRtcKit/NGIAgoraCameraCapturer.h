//
//  Agora SDK
//
//  Copyright (c) 2019 Agora.io. All rights reserved.
//

#pragma once  // NOLINT(build/header_guard)

#include "AgoraBase.h"
#include "AgoraRefPtr.h"

namespace agora {
namespace rtc {
class IVideoSinkBase;
class ICameraCaptureObserver;
/**
 * The ICameraCapturer class, which provides access to a camera capturer.
 */
class ICameraCapturer : public RefCountInterface {

 public:
  /**
   * The camera source.
   */
  enum CAMERA_SOURCE {
    /**
     * The camera source is the rear camera.
     */
    CAMERA_BACK,
    /**
     * The camera source is the front camera.
     */
    CAMERA_FRONT,
  };

  /**
   * The camera state.
   */
  enum CAMERA_STATE {
    /**
     * The camera source is started.
     */
    CAMERA_STARTED,
    /**
     * The camera source is stopped.
     */
    CAMERA_STOPPED,
  };

  // Interface for receiving information about available camera devices.
  /**
   * The IDeviceInfo class, which manages the information of available cameras.
   */
  class IDeviceInfo {
   public:
    virtual ~IDeviceInfo() {}

    /**
     * Releases the device.
     */
    virtual void release() = 0;

    /**
     * Gets the number of all available cameras.
     * @return The number of all available cameras.
     */
    virtual uint32_t NumberOfDevices() = 0;

    /**
     * Gets the name of a specified camera.
     * @param deviceNumber The index number of the device.
     * @param deviceNameUTF8 The name of the device.
     * @param deviceNameLength The length of the device name.
     * @param deviceUniqueIdUTF8 The unique ID of the device.
     * @param deviceUniqueIdLength The length of the device ID.
     * @param productUniqueIdUTF8 The unique ID of the product.
     * @param productUniqueIdLength The length of the product ID.
     * @return
     * The name of the device in the UTF8 format: Success.
     */
    virtual int32_t GetDeviceName(uint32_t deviceNumber, char* deviceNameUTF8,
                                  uint32_t deviceNameLength, char* deviceUniqueIdUTF8,
                                  uint32_t deviceUniqueIdLength, char* productUniqueIdUTF8 = 0,
                                  uint32_t productUniqueIdLength = 0) = 0;

    /**
     * Sets the capability number for a specified device.
     * @param deviceUniqueIdUTF8 The pointer to the ID of the device in the UTF8 format.
     * @return
     * The capability number of the device.
     */
    virtual int32_t NumberOfCapabilities(const char* deviceUniqueIdUTF8) = 0;

    /**
     * Gets the capability of a specified device.
     * @param deviceUniqueIdUTF8 The pointer to the ID of the device in the UTF8 format.
     * @param deviceCapabilityNumber The capability number of the device.
     * @param capability The reference to the video capability. See {@link VideoFormat}.
     * @return
     * The capability number of the device.
     */
    virtual int32_t GetCapability(const char* deviceUniqueIdUTF8,
                                  const uint32_t deviceCapabilityNumber,
                                  VideoFormat& capability) = 0;
  };

 public:
#if defined(__ANDROID__) || (defined(__APPLE__) && TARGET_OS_IPHONE)
  /**
   * Sets the camera source.
   *
   * @note
   * This method applies to Android and iOS only.
   *
   * @param source The camera source that you want to capture. See #CAMERA_SOURCE.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setCameraSource(CAMERA_SOURCE source) = 0;
  /**
   * Gets the camera source.
   *
   * @note
   * This method applies to Android and iOS only.
   *
   * @return The camera source. See #CAMERA_SOURCE.
   */
  virtual CAMERA_SOURCE getCameraSource() = 0;
  /**
   * Switch the camera source
   * 
   * @note
   * This method applies to Android and iOS only.
   */
  virtual void switchCamera() = 0;
  /**
   * Returns whether zooming is supported by the current device.
   * @note
   * This method applies to Android and iOS only.
   * This interface returns valid values only after the device is initialized.
   *
   * @return
   * - true: zooming is supported.
   * - false: zooming is not supported or device is not initialized.
   */
  virtual bool isZoomSupported() = 0;
  /**
   * Sets the zooming factor of the device.
   *
   * @note
   * This method applies to Android and iOS only.
   *
   * @param zoomValue The zooming factor of the device.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int32_t setCameraZoom(float zoomValue) = 0;
  /**
   * Gets the max zooming factor of the device.
   *
   * @note
   * This method applies to Android and iOS only.
   *
   * @return
   * - The max zooming factor of the device
   */
  virtual float getCameraMaxZoom() = 0;
  /**
   * Returns whether auto-focus is supported by the current device.
   * @note
   * This method applies to Android and iOS only.
   * This interface returns valid values only after device is initialized.
   *
   * @return
   * - true: auto-focus is supported.
   * - false: auto-focus is not supported or device is not initialized.
   */
  virtual bool isFocusSupported() = 0;
  /**
   * Sets the focus area of the current device.
   * @note
   * This method applies to Android and iOS only.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int32_t setCameraFocus(float x, float y) = 0;
  /**
   * Returns whether auto face focus is supported by the current device.
   * @note
   * This method applies to Android and iOS only.
   * This interface returns valid values only after device is initialized.
   *
   * @return
   * - true: auto-face-focus is supported.
   * - false: auto-face-focus is not supported or device is not initialized.
   */
  virtual bool isAutoFaceFocusSupported() = 0;
  /**
   * Enables or disables auto face focus.
   * @note
   * This method applies to Android and iOS only.
   * This interface returns valid values only after device is initialized.
   *
   * @return
    * - 0: Success.
   * - < 0: Failure.
   */
  virtual int32_t setCameraAutoFaceFocus(bool enable) = 0;
  /**
   * Enables or disables auto face detection.
   * @note
   * This method applies to Android and iOS only.
   * This interface returns valid values only after device is initialized.
   *
   * @return
    * - 0: Success.
   * - < 0: Failure.
   */
  virtual int32_t enableFaceDetection(bool enable) = 0;

  /**
   * Checks whether the camera face detect is supported.
   *
   * @return
   * - true: The camera face detect is supported.
   * - false: The camera face detect is not supported.
   */
  virtual bool isCameraFaceDetectSupported() = 0;

  /**
   * Checks whether the camera flash function is supported.
   *
   * The SDK uses the front camera by default, so if you call `isCameraTorchSupported` directly,
   * you can find out from the return value whether the device supports enabling the flash
   * when using the front camera. If you want to check whether the device supports enabling the
   * flash when using the rear camera, call \ref IRtcEngine::switchCamera "switchCamera"
   * to switch the camera used by the SDK to the rear camera, and then call `isCameraTorchSupported`.
   *
   * @note
   * - Call this method after the camera is started.
   * - This method is for Android and iOS only.
   * - On iPads with system version 15, even if `isCameraTorchSupported` returns true, you might
   * fail to successfully enable the flash by calling \ref IRtcEngine::setCameraTorchOn "setCameraTorchOn"
   * due to system issues.
   *
   * @return
   * - true: The device supports enabling the flash.
   * - false: The device does not support enabling the flash.
   */
  virtual bool isCameraTorchSupported() = 0;

  /**
   * @note
   * - Call this method after the camera is started.
   * - This method is for Android and iOS only.
   * - On iPads with system version 15, even if \ref IRtcEngine::isCameraTorchSupported "isCameraTorchSupported"
   * returns true, you might fail to successfully enable the flash by calling `setCameraTorchOn` due to
   * system issues.
   *
   * @param isOn Determines whether to enable the flash:
   * - true: Enable the flash.
   * - false: Disable the flash.
   *
   * @return
   * - 0: Success
   * - < 0: Failure
   */
  virtual int setCameraTorchOn(bool isOn) = 0;

  /** Checks whether the camera exposure function is supported.
   *
   * Ensure that you call this method after the camera starts, for example, by calling `startPreview` or `joinChannel`.
   *
   * @since v2.3.2.
   * @return
   * <ul>
   *     <li>true: The device supports the camera exposure function.</li>
   *     <li>false: The device does not support the camera exposure function.</li>
   * </ul>
   */
  virtual bool isCameraExposurePositionSupported() = 0;

  /** Sets the camera exposure position.
   *
   * Ensure that you call this method after the camera starts, for example, by calling `startPreview` or `joinChannel`.
   *
   * A successful setCameraExposurePosition method call triggers the {@link IRtcEngineEventHandler#onCameraExposureAreaChanged onCameraExposureAreaChanged} callback on the local client.
   * @since v2.3.2.
   * @param positionXinView The horizontal coordinate of the touch point in the view.
   * @param positionYinView The vertical coordinate of the touch point in the view.
   *
   * @return
   * <ul>
   *     <li>0: Success.</li>
   *     <li>< 0: Failure.</li>
   * </ul>
   */
  virtual int setCameraExposurePosition(float positionXinView, float positionYinView) = 0;
  
#if (defined(__APPLE__) && TARGET_OS_IOS)
  /**
   * Checks whether the camera auto exposure function is supported.
   *
   * @return
   * - true: The camera auto exposure function is supported.
   * - false: The camera auto exposure function is not supported.
   */
  virtual bool isCameraAutoExposureFaceModeSupported() = 0;


  /**
   * Enables the camera auto exposure face function.
   *
   * @param enabled Determines whether to enable the camera auto exposure face mode.
   * - true: Enable the auto exposure face function.
   * - false: Do not enable the auto exposure face function.
   *
   * @return
   * <ul>
   *     <li>0: Success.</li>
   *     <li>< 0: Failure.</li>
   * </ul>
   */
  virtual int setCameraAutoExposureFaceModeEnabled(bool enabled) = 0;
#endif
  
#elif defined(_WIN32) || (defined(__linux__) && !defined(__ANDROID__)) || \
    (defined(__APPLE__) && TARGET_OS_MAC && !TARGET_OS_IPHONE)

  /**
   * Creates a DeviceInfo object.
   *
   * @note
   * This method applies to Windows, macOS, and Linux only.
   * @return
   * - The pointer to \ref agora::rtc::ICameraCapturer::IDeviceInfo "IDeviceInfo": Success.
   * - An empty pointer NULL: Failure.
   */
  virtual IDeviceInfo* createDeviceInfo() = 0;
  /**
   * Initializes the device with the device ID.
   *
   * @note
   * This method applies to Windows, macOS, and Linux only.
   *
   * @param deviceId The pointer to the device ID.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int initWithDeviceId(const char* deviceId) = 0;
  /**
   * Initializes the device with the device name.
   *
   * @note
   * This method applies to Windows, macOS, and Linux only.
   *
   * @param deviceName The pointer to the device name.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int initWithDeviceName(const char* deviceName) = 0;
#endif

  /**
   * Set the device orientation of the capture device
   * @param VIDEO_ORIENTATION orientaion of the device 0(by default), 90, 180, 270
   */
  virtual void setDeviceOrientation(VIDEO_ORIENTATION orientation) = 0;

  /**
   * Sets the format of the video captured by the camera.
   *
   * If you do not set the video capturing format, the SDK automatically chooses a proper format according to the video encoder configuration of the video track.
   *
   * @param capture_format The reference to the video format: VideoFormat.
   */
  virtual void setCaptureFormat(const VideoFormat& capture_format) = 0;
  /**
   * Gets the format of the video captured by the camera.
   * @return
   * VideoFormat.
   */
  virtual VideoFormat getCaptureFormat() = 0;
  /**
   * Register a camera observer.
   *
   * @param observer Instance of the capture observer.
   */
  virtual int registerCameraObserver(ICameraCaptureObserver* observer) = 0;
  /**
   * Unregisters the camera observer.
   *
   * @param observer Instance of the capture observer.
   */
  virtual int unregisterCameraObserver(ICameraCaptureObserver* observer) = 0;

 protected:
  ~ICameraCapturer() {}
};
/**
 * The ICameraCaptureObserver class, which handles camera capture events.
 */
class ICameraCaptureObserver {
 public:
 /**
  * Occurs when the camera focus area changes.
  *
  * @note The SDK triggers this callback when the local user changes the camera focus position by
  * calling the \ref agora::rtc::ICameraCapturer::setCameraFocus "setCameraFocus" method.
  *
  * @param imageWidth Width of the changed camera focus area.
  * @param imageHeight Height of the changed camera focus area.
  * @param x x coordinate of the changed camera focus area.
  * @param y y coordinate of the changed camera focus area.
  */
  virtual void onCameraFocusAreaChanged(int imageWidth, int imageHeight, int x, int y) {
    (void) imageWidth;
    (void) imageHeight;
    (void) x;
    (void) y;

  }
  /**
   * Reports the face detection result of the local user. Applies to Android and iOS only.
   *
   * Once you enable face detection by calling \ref agora::rtc::ICameraCapturer::setCameraFaceDetection "setCameraFaceDetection" (true),
   * you can get the following information on the local user in real-time:
   * - The width and height of the local video.
   * - The position of the human face in the local video.
   * - The distance between the human face and the device screen.
   * This value is based on the fitting calculation of the local video size and the position of the human face.
   *
   * @note
   * - If the SDK does not detect a face, it reduces the frequency of this callback to reduce power consumption on the local device.
   * - The SDK stops triggering this callback when a human face is in close proximity to the screen.
   * - On Android, the distance value reported in this callback may be slightly different from the actual distance.
   * Therefore, Agora does not recommend using it for accurate calculation.
   *
   * @param imageWidth The width (px) of the local video.
   * @param imageHeight The height (px) of the local video.
   * @param vecRectangle The position and size of the human face on the local video:
   * - `x`: The x coordinate (px) of the human face in the local video. Taking the top left corner of the captured video as the origin,
   * the x coordinate represents the relative lateral displacement of the top left corner of the human face to the origin.
   * - `y`: The y coordinate (px) of the human face in the local video. Taking the top left corner of the captured video as the origin,
   * the y coordinate represents the relative longitudinal displacement of the top left corner of the human face to the origin.
   * - `width`: The width (px) of the human face in the captured video.
   * - `height`: The height (px) of the human face in the captured video.
   * @param vecDistance The distance (cm) between the human face and the screen.
   * @param numFaces The number of faces detected. If the value is 0, it means that no human face is detected.
   */
  virtual void onFacePositionChanged(
      int imageWidth, int imageHeight, const Rectangle* vecRectangle, const int* vecDistance, int numFaces) {
    (void) imageWidth;
    (void) imageHeight;
    (void) vecRectangle;
    (void) vecDistance;
    (void) numFaces;
  }

  /** Occurs when the camera exposure area changes.
  *
  * The SDK triggers this callback when the local user changes the camera exposure position by calling the setCameraExposurePosition method.
  *
  * @note This callback is for Android and iOS only.
  *
  * @param x x coordinate of the changed camera exposure area.
  * @param y y coordinate of the changed camera exposure area.
  * @param width Width of the changed camera exposure area.
  * @param height Height of the changed camera exposure area.
  */
  virtual void onCameraExposureAreaChanged(int x, int y, int width, int height) {
    (void)x;
    (void)y;
    (void)width;
    (void)height;
  }

  virtual void onCameraStateChanged(ICameraCapturer::CAMERA_STATE state, ICameraCapturer::CAMERA_SOURCE source) {
    (void) state;
    (void) source;
  }

 protected:
  virtual ~ICameraCaptureObserver() {}
};

}  // namespace rtc
}  // namespace agora
