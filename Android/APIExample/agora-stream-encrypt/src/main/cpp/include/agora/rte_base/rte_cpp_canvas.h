#pragma once

#include "rte_base/c/c_player.h"
#include "rte_base/c/handle.h"
#include "rte_base/c/track/canvas.h"

#include "rte_cpp_error.h"
#include "rte_cpp_rte.h"
#include "rte_cpp_callback_utils.h"


namespace rte {

using VideoRenderMode = ::RteVideoRenderMode;
using VideoMirrorMode = ::RteVideoMirrorMode;
using ViewConfig = ::RteViewConfig;
using View = ::RteView;
using Rect = ::RteRect;

/**
 * The CanvasInitialConfig class is used to initialize the Canvas object.
 * @since v4.4.0
 * @technical preview
 */
class CanvasInitialConfig {
  public:
    CanvasInitialConfig() {RteCanvasInitialConfigInit(&c_canvas_initial_config, nullptr);}
    ~CanvasInitialConfig() {RteCanvasInitialConfigDeinit(&c_canvas_initial_config, nullptr);}
  
  private:
    friend class Canvas;
    ::RteCanvasInitialConfig c_canvas_initial_config;
};

/**
 * @brief This class provides methods for configuring video rendering for the player.
 *
 * @since v4.4.0
 */
class CanvasConfig {
 public:
 CanvasConfig() {RteCanvasConfigInit(&c_canvas_config, nullptr);}
 ~CanvasConfig() {RteCanvasConfigDeinit(&c_canvas_config, nullptr);}

 /**
  * @brief Sets the video render mode.
  *
  * @since v4.4.0
  *
  * @details
  * Call timing: This method must be called before `SetConfigs(CanvasConfig *config, Error *err)`.
  *
  * @param mode Render mode. See `RteVideoRenderMode`. The default render mode is
  * `kRteVideoRenderModeHidden`,
  * which scales the video proportionally and prioritizes filling the view window.
  * @param err Status or error information. See `Error`.
  *
  */
 void SetRenderMode(VideoRenderMode mode, Error *err = nullptr) {
    RteCanvasConfigSetVideoRenderMode(&c_canvas_config, mode, err != nullptr ? err->get_underlying_impl() : nullptr);
 }

  /**
   * @brief Gets the currently set video render mode.
   *
   * @since v4.4.0
   *
   * @details
   * Call timing: This method must be called after `GetConfigs(CanvasConfig *config, Error *err)`.
   *
   * @param err Status or error information. See `Error`.
   *
   * @return
   * The currently set video render mode. See `RteVideoRenderMode`.
   */
  VideoRenderMode GetRenderMode(Error *err = nullptr) {
    VideoRenderMode mode;
    RteCanvasConfigGetVideoRenderMode(&c_canvas_config, &mode, err != nullptr ? err->get_underlying_impl() : nullptr);
    return mode;
 }

  /**
   * @brief Sets the mirror mode for the video.
   *
   * @since v4.4.0
   *
   * @details
   * Call timing: This method must be called before `SetConfigs(CanvasConfig *config, Error *err)`.
   *
   * @param mode Mirror mode. See `RteVideoMirrorMode`. The default is `kRteVideoMirrorModeAuto`,
   * where the SDK
   * determines the mirror mode. By default, the mirror mode for remote users is disabled.
   * @param err Status or error information. See `Error`.
   *
   */
  void SetMirrorMode(VideoMirrorMode mode, Error *err = nullptr) {
    RteCanvasConfigSetVideoMirrorMode(&c_canvas_config, mode, err != nullptr ? err->get_underlying_impl() : nullptr);
  }
  
  /**
   * @brief Gets the currently set mirror mode.
   *
   * @since v4.4.0
   *
   * @details
   * Call timing: This method must be called after `GetConfigs(CanvasConfig *config, Error *err)`.
   *
   * @param err Status or error information. See `Error`.
   *
   * @return
   * The currently set mirror mode. See `RteVideoMirrorMode`.
   */
  VideoMirrorMode GetMirrorMode(Error *err = nullptr) {
    VideoMirrorMode mode;
    RteCanvasConfigGetVideoMirrorMode(&c_canvas_config, &mode, err != nullptr ? err->get_underlying_impl() : nullptr);
    return mode;
  }

  /**
   * Set the Crop Area.
   * @since v4.4.0
   * @param crop_area
   * @param err
   * @return void
   * @technical preview
   */
  void SetCropArea(RteRect &crop_area, Error *err = nullptr) {
    RteCanvasConfigSetCropArea(&c_canvas_config, crop_area, err != nullptr ? err->get_underlying_impl() : nullptr);
  }

  /**
   * Get the Crop Area.
   * @since v4.4.0
   * @param err
   * @return RteRect
   * @technical preview
   */
  RteRect GetCropArea(Error *err = nullptr) {
    RteRect crop_area;
    RteCanvasConfigGetCropArea(&c_canvas_config, &crop_area, err != nullptr ? err->get_underlying_impl() : nullptr);
    return crop_area;
  }

 private:
  friend class Canvas;
  ::RteCanvasConfig c_canvas_config;
};

/**
 * The Canvas class is used to render the video stream.
 * @since v4.4.0
 */
class Canvas {
 public:
  /**
   * @brief Constructs a `Canvas` object.
   *
   * @details
   * Call timing: Call this method after `InitMediaEngine`.
   *
   * @param Rte An `Rte` object.
   * @param initial_config The configuration object for the `Canvas`. Currently, you can pass in null.
   *
   */
  Canvas(Rte *rte, CanvasInitialConfig *initial_config = nullptr) {
    c_canvas = ::RteCanvasCreate(&rte->c_rte, initial_config != nullptr ? &initial_config->c_canvas_initial_config : nullptr, nullptr);
  };
  ~Canvas() { RteCanvasDestroy(&c_canvas, nullptr); };

  Canvas(Canvas&& other) : c_canvas(other.c_canvas) {
    other.c_canvas = {};
  }

  //@{
  Canvas(const Canvas& other) = delete;
  Canvas& operator=(const Canvas& other) = delete;
  Canvas& operator=(Canvas&& other) = delete;
  //@}


  /**
   * @brief Gets the current video rendering configuration of the player.
   *
   * @since v4.4.0
   *
   * @details
   * Call timing: Call this method after `Canvas`.
   *
   * @param config The settings of the `Canvas` object. See `CanvasConfig`.
   * @param err Status or error information. See `Error`.
   *
   * @return
   * Whether the configuration is retrieved successfully:
   * - `true`: Retrieved successfully.
   * - `false`: Failed to retrieve.
   */
  bool GetConfigs(CanvasConfig *config, Error *err = nullptr) {
    return RteCanvasGetConfigs(&c_canvas, &config->c_canvas_config, err != nullptr ? err->get_underlying_impl() : nullptr);
  }

  /**
   * @brief Sets the player video rendering configuration.
   *
   * @since v4.4.0
   *
   * @details
   * Call timing: This method must be called before `OpenWithUrl`.
   *
   * @param config Settings for the `Canvas` object. See `CanvasConfig`.
   * @param err Status or error information. See `Error`.
   *
   * @return
   * Whether the configuration is successful:
   * - `true`: Configuration succeeded.
   * - `false`: Configuration failed.
   */
  bool SetConfigs(CanvasConfig *config, Error *err = nullptr) {
    return RteCanvasSetConfigs(&c_canvas, &config->c_canvas_config, err != nullptr ? err->get_underlying_impl() : nullptr);
  }

  /**
   * @brief Adds a rendering view.
   *
   * @since v4.4.0
   *
   * @details
   * Applicable scenarios: When you need to use the player to play videos, you can call this method to
   * add a view to the video renderer so that the video content can be displayed.
   * Call timing: This method must be called before `SetCanvas`.
   *
   * @note Currently, only one view is supported.
   *
   * @param view The HWND window handle.
   * @param config The settings of the `View` object. Currently, pass in null.
   * @param err Status or error information. See `Error`.
   *
   * @return
   * Whether the view is added successfully:
   * - `true`: The view is added successfully.
   * - `false`: Failed to add the view.
   */
  bool AddView(View *view, ViewConfig *config, rte::Error *err = nullptr) {
    return RteCanvasAddView(&c_canvas, view, config, err != nullptr ? err->get_underlying_impl() : nullptr);
  }

  /**
   * @brief Removes a rendering view.
   *
   * @since v4.4.0
   *
   * @details
   * After you call `AddView` to add a view, you can call this method to remove the rendering view if
   * needed.
   * Call timing: This method must be called after `AddView`.
   *
   * @note Only one view can be removed at a time.
   *
   * @param view The view object to be removed.
   * @param config Settings for the `View` object. Currently, pass null.
   * @param err Status or error information. See `Error`.
   *
   * @return
   * Whether the view is successfully removed:
   * - `true`: View removed successfully.
   * - `false`: Failed to remove view.
   */
  bool RemoveView(View *view, ViewConfig *config, rte::Error *err = nullptr) {
    return RteCanvasRemoveView(&c_canvas, view, config, err != nullptr ? err->get_underlying_impl() : nullptr);
  }

 private:

  friend class Player;

  ::RteCanvas c_canvas;
};

}  // namespace rte