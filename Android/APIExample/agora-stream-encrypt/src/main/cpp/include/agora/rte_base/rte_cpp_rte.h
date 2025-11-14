/**
 *
 * Agora Real Time Engagement
 * Copyright (c) 2024 Agora IO. All rights reserved.
 *
 */
#pragma once
#include <string>
#include "rte_base/c/c_rte.h"
#include "rte_base/c/bridge.h"

#include "rte_cpp_error.h"
#include "rte_cpp_callback_utils.h"
#include "rte_cpp_string.h"

struct RteObserver;
struct RteInitialConfig;
struct RteConfig;

namespace rte {

class Player;

/**
 * The InitialConfig class is used to initialize the Rte object.
 * @since v4.4.0
 * @technical preview
 */
class InitialConfig {
 public:
  InitialConfig() { RteInitialConfigInit(&c_rte_init_cfg, nullptr); }
  ~InitialConfig() { RteInitialConfigDeinit(&c_rte_init_cfg, nullptr);}

 private:
  friend class Rte;
  ::RteInitialConfig c_rte_init_cfg;
};

/**
 * The Observer class is used to observe the event of Rte object.
 * @since v4.4.0
 * @technical preview
 */
class Observer {
 public:
  Observer(): c_rte_observer(::RteObserverCreate(nullptr)) {
    c_rte_observer->base_observer.me_in_target_lang = this;}
  ~Observer() { RteObserverDestroy(c_rte_observer, nullptr); }

  // @{
  Observer(Observer &other) = delete;
  Observer(Observer &&other) = delete;
  Observer &operator=(const Observer &cmd) = delete;
  Observer &operator=(Observer &&cmd) = delete;
  // @}

 private:
  friend class Rte;

  ::RteObserver *c_rte_observer;
};

/**
 * @brief RTE configuration class used to configure the RTE object.
 *
 * @since v4.4.0
 */
class Config {
 public:
  Config() {RteConfigInit(&c_rte_config, nullptr);}
  ~Config() {RteConfigDeinit(&c_rte_config, nullptr);}

  // @{
  Config(Config &other) = delete;
  Config(Config &&other) = delete;
  Config &operator=(const Config &cmd) = delete;
  Config &operator=(Config &&cmd) = delete;
  // @}

  /**
   * @brief Sets the App ID.
   *
   * @since  v4.4.0
   *
   * @details
   * Call timing: This method must be called before `InitMediaEngine`.
   *
   * @param app_id Your project's App ID, which you can obtain from the Agora Console.
   * @param err Status or error information. See `Error`.
   *
   */
  void SetAppId(const char *app_id, Error *err = nullptr){
    String str(app_id);
    RteConfigSetAppId(&c_rte_config, str.get_underlying_impl(), err != nullptr ? err->get_underlying_impl() : nullptr);
  }

  /**
   * @brief Gets the configured App ID.
   *
   * @since  v4.4.0
   *
   * @param err Status or error information. See `Error`.
   *
   * @return
   * - If the method call succeeds, returns the configured App ID.
   * - If the method call fails, returns an empty string.
   */
  std::string GetAppId(Error *err = nullptr){
    String str;
    RteConfigGetAppId(&c_rte_config, str.get_underlying_impl(), err != nullptr ? err->get_underlying_impl() : nullptr);
    return std::string(str.CStr());
  }


  /**
   * Set the Log Folder Parameter
   * @since v4.4.0
   * @param log_folder 
   * @param err 
   * @technical preview
   */
  void SetLogFolder(const char *log_folder, Error *err = nullptr){
    String str(log_folder);
    RteConfigSetLogFolder(&c_rte_config, str.get_underlying_impl(), err != nullptr ? err->get_underlying_impl() : nullptr);
  }


  /**
   * Get the Log Folder Parameter
   * @since v4.4.0
   * @param err 
   * @return const char* 
   * @technical preview
   */
  std::string GetLogFolder(Error *err = nullptr){
    String str;
    RteConfigGetLogFolder(&c_rte_config, str.get_underlying_impl(), err != nullptr ? err->get_underlying_impl() : nullptr);
    return std::string(str.CStr());
  }

  /**
   * Set the Log File Size Parameter
   * @since v4.4.0
   * @param log_file_size 
   * @param err 
   * @technical preview
   */
  void SetLogFileSize(size_t log_file_size, Error *err = nullptr){
    RteConfigSetLogFileSize(&c_rte_config, log_file_size, err != nullptr ? err->get_underlying_impl() : nullptr);
  }

  /**
   * Get the Log File Size Parameter
   * @since v4.4.0
   * @param err 
   * @return size_t 
   * @technical preview
   */
  size_t GetLogFileSize(Error *err = nullptr){
    size_t log_file_size;
    RteConfigGetLogFileSize(&c_rte_config, &log_file_size, err != nullptr ? err->get_underlying_impl() : nullptr);
    return log_file_size;
  }

  /**
   * Set the Area Code Parameter
   * @since v4.4.0
   * @param area_code 
   * @param err 
   * @technical preview
   */
  void SetAreaCode(int32_t area_code, Error *err = nullptr){
    RteConfigSetAreaCode(&c_rte_config, area_code, err != nullptr ? err->get_underlying_impl() : nullptr);
  }

  /**
   * Get the Area Code Parameter
   * @since v4.4.0
   * @param err 
   * @return int32_t 
   * @technical preview
   */
  int32_t GetAreaCode(Error *err = nullptr){
    int32_t area_code;
    RteConfigGetAreaCode(&c_rte_config, &area_code, err != nullptr ? err->get_underlying_impl() : nullptr);
    return area_code;
  }

  /**
   * Set the Cloud Proxy Parameter
   * @since v4.4.0
   * @param cloud_proxy 
   * @param err 
   * @technical preview
   */
  void SetCloudProxy(const char *cloud_proxy, Error *err = nullptr){
    String str(cloud_proxy);
    RteConfigSetCloudProxy(&c_rte_config, str.get_underlying_impl(), err != nullptr ? err->get_underlying_impl() : nullptr);
  }

  /**
   * Get the Cloud Proxy Parameter
   * @since v4.4.0
   * @param err 
   * @return const char* 
   * @technical preview
   */
  std::string GetCloudProxy(Error *err = nullptr){
    String str;
    RteConfigGetCloudProxy(&c_rte_config, str.get_underlying_impl(), err != nullptr ? err->get_underlying_impl() : nullptr);
    return std::string(str.CStr());
  }

  /**
   * @brief Configures technical preview or custom features provided by the SDK using JSON.
   *
   * @since v4.4.0
   *
   * @details
   * Applicable scenarios: You can call this method when you need to set private parameters or use
   * custom features.
   * Call timing: This method must be called before `SetConfigs(Config *config, Error *err)`.
   *
   * @param json_parameter Parameters in JSON string format.
   * @param err Status or error information. See `Error`.
   *
   */
  void SetJsonParameter(const char *json_parameter, Error *err = nullptr){
    String str(json_parameter);
    RteConfigSetJsonParameter(&c_rte_config, str.get_underlying_impl(), err != nullptr ? err->get_underlying_impl() : nullptr);
  }

  /**
   * @brief Gets the configured SDK JSON configuration information.
   *
   * @since v4.4.0
   *
   * @details
   * After you call `SetJsonParameter` to set the JSON configuration, you can call this method to
   * retrieve the configured information.
   * Call timing: This method must be called after `GetConfigs(Config *config, Error *err)`.
   *
   * @param err Status or error information. See `Error`.
   *
   * @return
   * - If the method call succeeds, returns the configured JSON information.
   * - If the method call fails, returns an empty string.
   */
  std::string GetJsonParameter(Error *err = nullptr){
    String str;
    RteConfigGetJsonParameter(&c_rte_config, str.get_underlying_impl(), err != nullptr ? err->get_underlying_impl() : nullptr);
    return std::string(str.CStr());
  }

 private:
  ::RteConfig* get_underlying_impl() { return &c_rte_config; }

 private:
  friend class Rte;
  ::RteConfig c_rte_config;
};

/**
 * The Rte class, which is the base interface of the Agora Real Time Engagement SDK.
 * @since v4.4.0
 */
class Rte {
 public:

  /**
   * @brief Bridges an RTE object from `IRtcEngine`.
   *
   * @since v4.4.0
   *
   * @details
   * The RTE object created by calling this method does not need to be initialized again using
   * `InitMediaEngine`. If you have not previously created and initialized the RTC engine, you can
   * first call `Rte` to create an RTE object, and then call `InitMediaEngine` to initialize it.
   * Call timing: Before calling this method, make sure you have called `initialize` to initialize the
   * RTC engine.
   *
   * @param err A pointer to an `Error` object used to receive status and error codes.
   *
   * @return
   * An RTE object.
   */
  static Rte GetFromBridge(Error* err = nullptr){
    Rte rte( RteGetFromBridge(err != nullptr ? err->get_underlying_impl() : nullptr));
    return rte;
  }

  /**
   * @brief Constructs an RTE object.
   *
   * @since v4.4.0
   *
   * @details
   * The RTE object is used to organize and manage internal resources within Rte.
   * Call timing: This method must be called before `InitMediaEngine`.
   *
   * @param config Initialization settings. Currently, you can pass null.
   *
   */
  explicit Rte(InitialConfig *config = nullptr): c_rte(::RteCreate(config != nullptr ? &config->c_rte_init_cfg : nullptr, nullptr)) {}
  ~Rte(){Destroy();};

  /**
   * Construct a new Rte object.
   * 
   * @param other 
   */
  Rte(Rte &&other) : c_rte(other.c_rte) {
    other.c_rte = {};
  }

  // @{
  Rte(Rte &other) = delete;
  Rte &operator=(const Rte &other) = delete;
  Rte &operator=(Rte &&other) = delete;
  // @}

  /**
   * Register an RTE observer.
   * @since v4.4.0
   * @param observer The object that observes RTE callback events.
   * @param err Possible return values for ErrorCode:
   *  - kRteOk: Success
   *  - kRteErrorInvalidOperation: The corresponding internal RTE object has been destroyed or is invalid.
   *  - kRteErrorInvalidArgument: The registered observer object is null.
   * @return bool
   *  - true: Registration is successful.
   *  - false: Registration failed.
   * @technical preview
   */
  bool RegisterObserver(Observer *observer, Error *err = nullptr){
    return RteRegisterObserver(&c_rte, observer != nullptr ? observer->c_rte_observer : nullptr, err != nullptr ? err->get_underlying_impl() : nullptr);
  }

  /**
   * Unregister the RTE observer object.
   * @since v4.4.0
   * @param observer The object that observes RTE callback events.
   * @param err Possible return values for ErrorCode:
   *  - kRteOk: Success
   *  - kRteErrorInvalidOperation: The corresponding internal RTE object has been destroyed or is invalid.
   *  - kRteErrorInvalidArgument: The unregistered observer object is null.
   * @return bool
   *  - true: Unregistration is successful.
   *  - false: Unregistration failed.
   * @technical preview
   */
  bool UnregisterObserver(Observer *observer, Error *err = nullptr){
    return RteUnregisterObserver(&c_rte, observer != nullptr ? observer->c_rte_observer : nullptr,
                                err != nullptr ? err->get_underlying_impl() : nullptr);
  }

  /**
   * @brief Initializes the RTE engine.
   *
   * @details
   * This method is asynchronous and returns the initialization result via a callback function.
   * Call timing: This method must be called after creating the RTE object and setting the App ID.
   *
   * @note
   * The return value only indicates whether the initialization operation has been successfully
   * scheduled in the asynchronous task queue, not whether the initialization itself succeeded.
   *
   * @param cb An asynchronous callback function used to return the result of engine initialization.
   * You can get the result or error code of the initialization through the `err` parameter in the
   * callback. See `Error`.
   * @param err Error information during the initialization process. See `Error`. When calling the
   * `InitMediaEngine` method, if immediately detectable errors occur (such as invalid parameters or
   * insufficient resources), the SDK
   * will return the error information synchronously through this parameter.
   *
   * @return
   * Whether the asynchronous operation was successfully added to the queue:
   * - `true`: Successfully added to the queue.
   * - `false`: Failed to be added to the queue.
   * Note: This return value only indicates whether the initialization operation has been successfully
   * scheduled in the asynchronous task queue, not whether the initialization itself succeeded.
   */
  bool InitMediaEngine(std::function<void(rte::Error *err)> cb, Error *err = nullptr){
    auto* ctx = new CallbackContext<Rte>(this, cb);
    return RteInitMediaEngine(&c_rte, &CallbackFunc<::Rte, Rte>, ctx, err != nullptr ? err->get_underlying_impl() : nullptr);
  }

  /**
   * @brief Gets the configuration information of the RTE object.
   *
   * @since v4.4.0
   *
   * @details
   * Call timing: Call this method after `Rte`.
   *
   * @param config The settings of the RTE object. See `Config`.
   * @param err Status or error information. See `Error`.
   *
   * @return
   * Whether the configuration is retrieved successfully:
   * - `true`: Retrieved successfully.
   * - `false`: Failed to retrieve.
   */
  bool GetConfigs(Config *config, Error *err = nullptr){
    return RteGetConfigs(&c_rte, config != nullptr ? config->get_underlying_impl(): nullptr, err != nullptr ? err->get_underlying_impl() : nullptr);
  }

  /**
   * @brief Configures information for the RTE object.
   *
   * @since v4.4.0
   *
   * @details
   * You can call this method to set information such as the App ID.
   * Call timing: This method must be called after `Rte`.
   *
   * @param config Settings for the RTE object. See `Config`.
   * @param err Status or error information. See `Error`.
   *
   * @return
   * Whether the configuration is successful:
   * - `true`: Configuration succeeded.
   * - `false`: Configuration failed.
   */
  bool SetConfigs(Config *config, Error *err = nullptr){
    return RteSetConfigs(&c_rte, config != nullptr ? config->get_underlying_impl(): nullptr, err != nullptr ? err->get_underlying_impl() : nullptr);
  }

  /**
   * @brief Destroys the RTE object.
   *
   * @since v4.4.0
   *
   * @details
   * This method releases all resources used by the RTE object.
   *
   * @param err Status or error information. See `Error`.
   *
   * @return
   * Whether the RTE object is destroyed successfully:
   * - `true`: Destroyed successfully.
   * - `false`: Failed to destroy.
   */
  bool Destroy(Error *err = nullptr){
    return RteDestroy(&c_rte, err != nullptr ? err->get_underlying_impl() : nullptr);
  }

 private:

  explicit Rte(::Rte other) { c_rte = other; }
  
 private:
  friend class Player;
  friend class Canvas;

  ::Rte c_rte;
};

}  // namespace rte
