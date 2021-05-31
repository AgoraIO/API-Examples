//
//  AudioFrameCriticalSection.h
//  Agora-Screen-Sharing-iOS-Broadcast
//
//  Created by GongYuhua on 16/11/19.
//  Copyright © 2016年 Agora. All rights reserved.
//

#ifndef AGDCAudioFrameCriticalSection_h
#define AGDCAudioFrameCriticalSection_h

#include <pthread.h>

class CriticalSectionWrapper {
public:
  // Factory method, constructor disabled
  static CriticalSectionWrapper* CreateCriticalSection();
  
  virtual ~CriticalSectionWrapper() {}
  
  // Tries to grab lock, beginning of a critical section. Will wait for the
  // lock to become available if the grab failed.
  virtual void Enter() = 0;
  
  // Returns a grabbed lock, end of critical section.
  virtual void Leave() = 0;
};

// RAII extension of the critical section. Prevents Enter/Leave mismatches and
// provides more compact critical section syntax.
class CriticalSectionScoped {
public:
  explicit CriticalSectionScoped(CriticalSectionWrapper* critsec)
  : ptr_crit_sec_(critsec) {
    ptr_crit_sec_->Enter();
  }
  
  ~CriticalSectionScoped() {
    if (ptr_crit_sec_) {
      Leave();
    }
  }
  
private:
  void Leave() {
    ptr_crit_sec_->Leave();
    ptr_crit_sec_ = 0;
  }
  
  CriticalSectionWrapper* ptr_crit_sec_;
};

class CriticalSectionPosix : public CriticalSectionWrapper {
public:
  //  CriticalSectionPosix();
  //
  //  virtual ~CriticalSectionPosix();
  //
  //  virtual void Enter() override;
  //  virtual void Leave() override;
  
  CriticalSectionPosix() {
    pthread_mutexattr_t attr;
    (void) pthread_mutexattr_init(&attr);
    (void) pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    (void) pthread_mutex_init(&mutex_, &attr);
  }
  
  ~CriticalSectionPosix() {
    (void) pthread_mutex_destroy(&mutex_);
  }
  
  void
  Enter() {
    (void) pthread_mutex_lock(&mutex_);
  }
  
  void
  Leave() {
    (void) pthread_mutex_unlock(&mutex_);
  }
  
private:
  pthread_mutex_t mutex_;
  friend class ConditionVariablePosix;
};


CriticalSectionWrapper* CriticalSectionWrapper::CreateCriticalSection() {
  return new CriticalSectionPosix();
}


#endif /* AGDCAudioFrame_hpp */
