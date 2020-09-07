//  Agora RTC/MEDIA SDK
//
//  Created by Pengfei Han in 2019-08.
//  Copyright (c) 2019 Agora.io. All rights reserved.
//

#pragma once
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <functional>
#include <queue>

#if defined(__linux__) && !defined(__ANDROID__)
#include <syscall.h>
#define gettid() syscall(SYS_gettid)
#else
#define gettid() (long)0
#endif

#define AGO_LOG printf

static union { char c[4]; unsigned long mylong; } endian_test = { { 'l', '?', '?', 'b' } };
#define ENDIANNESS ((char)endian_test.mylong)

uint64_t now_ms();

class noncopyable {
 protected:
  noncopyable() {}
  ~noncopyable() {}

 private:
  noncopyable(const noncopyable&) = delete;
  noncopyable& operator=(const noncopyable&) = delete;
};

class AutoResetEvent : public noncopyable {
 public:
  explicit AutoResetEvent(bool init = false) : signal_(init) {}
  ~AutoResetEvent() {}

  void Set();

  int Wait(int wait_ms = -1);

 private:
  std::condition_variable cv_;
  std::mutex lock_;
  std::atomic<bool> signal_;
};

// async_result declaration.
template <typename T>
struct Maybe {
  Maybe() { valid = false; }
  explicit Maybe(const T& v) {
    value = v;
    valid = true;
  }
  explicit Maybe(const T&& v) {
    value = std::move(v);
    valid = true;
  }
  Maybe(const Maybe<T>& v) {
    if (this == &v) return;
    value = v.value;
    valid = v.valid;
  }
  Maybe(const Maybe<T>&& v) {
    if (this == &v) return;
    value = std::move(v.value);
    valid = v.valid;
  }
  Maybe<T>& operator=(const Maybe<T>& v) {
    if (this == &v) return *this;
    value = v.value;
    valid = v.valid;
    return *this;
  }
  T value;
  bool valid;
};

template <typename T>
class AsyncResult {
 public:
  using Compare = std::function<bool(T&)>;
  ~AsyncResult() { Clear(); }
  bool Contains(Compare compare, int timeout = -1) {
    while (true) {
      Maybe<T> v = Wait(timeout);
      if (!v.valid) return false;
      if (compare(v.value)) return true;
    }
  }

  bool Contains(const T& val, int timeout = -1) {
    while (true) {
      Maybe<T> v = Wait(timeout);
      if (!v.valid) return false;
      if (v.valid && v.value == val) return true;
    }
  }

  Maybe<T> Wait(int timeout = -1) {
    bool needs_wait = false;
    do {
      if (needs_wait) {
        int r = result_ready_.Wait(timeout);
        if (r != 0) {
          return Maybe<T>();
        }
      }
      std::lock_guard<std::mutex> _(result_lock_);
      if (results_.size() == 0) {
        needs_wait = true;
        continue;
      } else {
        T result = std::move(results_.front());
        results_.pop();
        return Maybe<T>(result);
      }
    } while (true);
  }

  void AppendResult(const T& val) {
    {
      // Then and wait can not use at the same time
      if (on_value_) {
        Maybe<T> v(val);
        on_value_(v);
        return;
      }
      std::lock_guard<std::mutex> _(result_lock_);
      results_.push(val);
    }
    result_ready_.Set();
  }

  void SetResult(const T& val) {
    {
      // Then and wait can not use at the same time
      if (on_value_) {
        Maybe<T> v(val);
        on_value_(v);
        return;
      }
      std::lock_guard<std::mutex> _(result_lock_);
      if (!results_.empty()) {
        results_.pop();
      }
      results_.push(val);
    }
    result_ready_.Set();
  }

  AsyncResult<T>& Then(std::function<void(Maybe<T>& v)>&& foo) {
    on_value_ = std::move(foo);
    return *this;
  }

  void Clear() {
    std::lock_guard<std::mutex> _(result_lock_);
    std::queue<T> empty;
    results_.swap(empty);
  }

 private:
  AutoResetEvent result_ready_;
  std::queue<T> results_;
  std::mutex result_lock_;
  std::function<void(Maybe<T>& v)> on_value_ = {};
};

struct DataStreamResult {
  bool check_result = true;
  int received_msg_count = 0;
  int received_total_bytes = 0;
};