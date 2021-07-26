//  Agora Media SDK
//
//  Created by Yaqi Li in 2021-03.
//  Copyright (c) 2021 Agora IO. All rights reserved.
//

#include <atomic>
#include <limits>
#include <logutils.h>
#include "external_thread_pool.h"
#include "AgoraRtcKit/AgoraBase.h"

namespace {

int GetConcurrencyLevel(int required_thread_num, bool forced) {
  int default_concurrency_level = std::thread::hardware_concurrency();
  default_concurrency_level += default_concurrency_level >> 1;

  if (default_concurrency_level == 0) {
    // We only keep four threads for external use
    default_concurrency_level = 4;
  }

  if (required_thread_num == 0) {
    required_thread_num = default_concurrency_level;
  }
  if (forced || required_thread_num <= default_concurrency_level) {
    return required_thread_num;
  }

  return default_concurrency_level;
}

}

namespace agora {
namespace rtc {
namespace extensions {

ThreadPool::ThreadPool(int required_thread_num, bool forced)
  : max_thread_num_(GetConcurrencyLevel(required_thread_num, forced)) {
}

ThreadPool::~ThreadPool() {
  {
    std::lock_guard<std::mutex> _(task_mutex_);
    stop_all_ = true;
    task_notifier_.notify_all();
  }
  for (auto& th : threads_) {
    th.join();
  }
}

int ThreadPool::RegisterInvoker(const std::string& worker_name) {
  auto invoker_id = generatorInvokerId();
  std::lock_guard<std::mutex> _(thread_mutex_);
  if (registered_invokers_.find(invoker_id) != registered_invokers_.end()) {
    PRINTF_ERROR("exernal thread pool is full, no new invoker can be registered!");
    return -1;
  }
  initThread(invoker_id, worker_name);
  return invoker_id;
}

int ThreadPool::PostTask(int invoker_id, FuncType&& func) {
  std::thread::id th_id;
  {
    std::lock_guard<std::mutex> thread_lock(thread_mutex_);
    auto err = findThreadId(invoker_id, th_id);
    if (err != ERR_OK) {
      return err;
    }
  }
  std::lock_guard<std::mutex> task_lock(task_mutex_);
  return insertTask(invoker_id, th_id, std::move(func));
}

int ThreadPool::UnregisterInvoker(int invoker_id) {
  std::lock_guard<std::mutex> _(thread_mutex_);
  if (registered_invokers_.find(invoker_id) == registered_invokers_.end()) {
    return -ERR_INVALID_ARGUMENT;
  }
  auto name = registered_invokers_[invoker_id];
  registered_invokers_.erase(invoker_id);
  if (!name.empty() && thread_name_id_map_.find(name) != thread_name_id_map_.end()) {
    auto th_id = thread_name_id_map_[name].first;
    if (--thread_name_id_map_[name].second == 0) {
      // recycle the thread name if the invoker is the last once that references
      thread_name_id_map_.erase(name);
    }
  }
  return ERR_OK;
}

int ThreadPool::generatorInvokerId() {
  static std::atomic_int s_invoker_id = {0};
  return s_invoker_id.fetch_add(1);
}

void ThreadPool::initThread(int invoker_id, const std::string& th_name) {
  // iIf thread allocation is of no interest and thread pool is full, then
  // we do not create new thread and return directly
  if (th_name.empty() && threads_.size() == max_thread_num_) {
    registered_invokers_[invoker_id] = th_name;
    return;
  }

  // If thread allocation is specified and a thread of the same name already exists,
  // then assign the thread to the invoker, increase the thread name reference count and return
  if (!th_name.empty() && thread_name_id_map_.find(th_name) != thread_name_id_map_.end()) {
    ++thread_name_id_map_[th_name].second;
    registered_invokers_[invoker_id] = th_name;
    return;
  }

  // If thread allocation is specified and the thread pool is full,
  // then find a least busy thread, reuse that thread and assign it to the invoker and return
  if (!th_name.empty() && threads_.size() == max_thread_num_) {
    auto min_th_id = findLeastBusyThread();
    thread_name_id_map_[th_name].first = min_th_id;
    thread_name_id_map_[th_name].second = 1;
    registered_invokers_[invoker_id] = th_name;
    return;
  }

  // Thread pool is not full, we create a new thread
  threads_.emplace_back(std::thread([this] {
    while(true) {
      std::list<Task> task_list;
      auto th_id = std::this_thread::get_id();
      {
        std::unique_lock<std::mutex> _(task_mutex_);
        task_notifier_.wait(_, [this,th_id] {
          return !assigned_tasks_[th_id].empty()
                 || !unassigned_tasks_.empty() || stop_all_;
        });
        if (stop_all_) {
          break;
        }
        if (!assigned_tasks_[th_id].empty()) {
          task_list = std::move(assigned_tasks_[th_id]);
        }
        if (task_list.empty() && !unassigned_tasks_.empty()) {
          task_list.push_back(unassigned_tasks_.front());
          unassigned_tasks_.pop_front();
        }
      }
      for (auto& task : task_list) {
        if (!task.task) {
          continue;
        }
        // Note: We don't handle residue tasks after unregister happens.
        // Invokers should handle life control issues by themselves.
        task.task();
      }
    }
  }));
  auto th_id = threads_.back().get_id();
  // If a thread name is specified, register it in the map
  if (!th_name.empty()) {
    thread_name_id_map_[th_name].first = th_id;
    thread_name_id_map_[th_name].second = 1;
  }
  registered_invokers_[invoker_id] = th_name;
}

std::thread::id ThreadPool::findLeastBusyThread() {
  // thread id to the invoker count map
  std::unordered_map<std::thread::id, int> invoker_cnt;
  for (auto& th : threads_) {
    invoker_cnt[th.get_id()] = 0;
  }
  // traverse the invoker map and count the number of invokers that are
  // assigned to the same thread
  for(auto& invoker : registered_invokers_) {
    if (invoker.second.empty()) {
      continue;
    }
    invoker_cnt[thread_name_id_map_[invoker.second].first] += 1;
  }

  // We consider the thread with minimum invokers are the least busy one
  std::thread::id min_th_id;
  int min_num = std::numeric_limits<int>::max();

  for(auto& it : invoker_cnt) {
    if (it.second < min_num) {
      min_num = it.second;
      min_th_id = it.first;
    }
  }
  return min_th_id;
}

int ThreadPool::findThreadId(int invoker_id, std::thread::id& th_id) {
  if (registered_invokers_.find(invoker_id) == registered_invokers_.end()) {
    return -ERR_NOT_READY;
  }
  auto& th_name = registered_invokers_[invoker_id];
  if (!th_name.empty() && thread_name_id_map_.find(th_name) == thread_name_id_map_.end()) {
    return -ERR_INVALID_STATE;
  }
  if (!th_name.empty()) {
    th_id = thread_name_id_map_[th_name].first;
  } else {
    th_id = std::thread::id(); // id of no thread
  }
  return ERR_OK;
}

int ThreadPool::insertTask(int invoker_id, std::thread::id th_id, FuncType&& task) {
  if (th_id == std::thread::id()) { // id of no thread
    unassigned_tasks_.push_back({invoker_id, std::move(task)});
  } else {
    assigned_tasks_[th_id].push_back({invoker_id, std::move(task)});
  }
  task_notifier_.notify_all();
  return ERR_OK;
}

}  // namespace extensions
}  // namespace rtc
}  // namespace agora
