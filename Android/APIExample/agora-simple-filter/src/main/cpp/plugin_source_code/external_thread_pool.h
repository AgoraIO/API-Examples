//  Agora Media SDK
//
//  Created by Yaqi Li in 2021-03.
//  Copyright (c) 2021 Agora IO. All rights reserved.
//
#pragma once

#include <thread>
#include <list>
#include <string>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <unordered_map>
#include <vector>
#include <future>

namespace agora {
namespace rtc {
namespace extensions {

class ThreadPool {
 public:
  using FuncType = std::function<void()>;
  explicit ThreadPool(int required_thread_num, bool forced = false);
  ~ThreadPool();

  // Rgister invoker in thread pool
  int RegisterInvoker(const std::string& worker_name = {});

  int UnregisterInvoker(int invoker_id);

  int PostTask(int invoker_id, FuncType&& func);

  // Note: this function spends extra time on creating packaged tasks.
  // We should avoid using this function for posting small and repeated tasks.
  // Use it only when you want synchronous invocation.
  template <typename F, typename... Args>
  auto PostTaskWithRes(int invoker_id, F&& f, Args&&... args)
      -> std::future<typename std::result_of<F(Args...)>::type> {
    using task_ret_type = typename std::result_of<F(Args...)>::type;
    auto package = std::make_shared<std::packaged_task<task_ret_type()>>(
      std::bind(std::forward<F>(f), std::forward<Args>(args)...));
    auto task = [package] { (*package)();};

    std::thread::id th_id;
    {
      std::lock_guard<std::mutex> thread_lock(thread_mutex_);
      auto err = findThreadId(invoker_id, th_id);
      if (err != 0) {
        return {}; // causing ret future in an invalid state
      }
    }
    auto res = package->get_future();
    std::lock_guard<std::mutex> task_lock(task_mutex_);
    if (insertTask(invoker_id, th_id, std::move(task)) != 0) {
      return {}; // causing ret future in an invalid state
    }
    return res;
  }

 private:
  ThreadPool(const ThreadPool&) = delete;
  ThreadPool(ThreadPool&&) = delete;
  ThreadPool& operator=(const ThreadPool&) = delete;
  ThreadPool& operator=(ThreadPool&&) = delete;

  int generatorInvokerId();
  void initThread(int invoker_id, const std::string& th_name);
  std::thread::id findLeastBusyThread();
  int findThreadId(int invoker_id, std::thread::id& th_id);
  int insertTask(int invoker_id, std::thread::id th_id, FuncType&& task);

 private:
  struct Task {
    int invoker_id = 0;
    FuncType task = nullptr;
  };
  std::mutex thread_mutex_;
  std::vector<std::thread> threads_;
  // thread name to (id, reference_count) map
  std::unordered_map<std::string, std::pair<std::thread::id, int>> thread_name_id_map_;
  // invoker_id to thread_name map
  std::unordered_map<int, std::string> registered_invokers_;
  std::mutex task_mutex_;
  // thread_id to task list map
  std::unordered_map<std::thread::id, std::list<Task>> assigned_tasks_;
  std::list<Task> unassigned_tasks_;
  std::condition_variable task_notifier_;
  int max_thread_num_ = {0};
  bool stop_all_ = false;
};

}  // namespace extensions
}  // namespace rtc
}  // namespace agora
