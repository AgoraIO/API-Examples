
// Copyright (c) 2020 Agora.io. All rights reserved

// This program is confidential and proprietary to Agora.io.
// And may not be copied, reproduced, modified, disclosed to others, published
// or used, in whole or in part, without the express prior written permission
// of Agora.io.
#pragma once

#ifndef __AGORA_REF_COUNTED_OBJECT_H__
#define __AGORA_REF_COUNTED_OBJECT_H__
#endif

#if defined(__AGORA_REF_COUNTED_OBJECT_INTERNAL_H__)
#error AgoraRefCountedObject is deprected now, its only purpose is for API compatiable.
#endif

#include "AgoraRefPtr.h"
#include "AgoraAtomicOps.h"

#ifndef OPTIONAL_REFCOUNTRELEASESTATUS_SPECIFIER
#if __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1800)
#define OPTIONAL_REFCOUNTRELEASESTATUS_SPECIFIER agora::RefCountReleaseStatus::
#else
#define OPTIONAL_REFCOUNTRELEASESTATUS_SPECIFIER
#endif
#endif
namespace agora {

class RefCounter {
 public:
  explicit RefCounter(int ref_count) : ref_count_(ref_count) {}

  void IncRef() { AtomicOps::Increment(&ref_count_); }

  /**
   *  Returns true if this was the last reference, and the resource protected by
   * the reference counter can be deleted.
   */
  agora::RefCountReleaseStatus DecRef() {
    return (AtomicOps::Decrement(&ref_count_) == 0
            ? OPTIONAL_REFCOUNTRELEASESTATUS_SPECIFIER kDroppedLastRef
            : OPTIONAL_REFCOUNTRELEASESTATUS_SPECIFIER kOtherRefsRemained);
  }

  /**
   * Return whether the reference count is one. If the reference count is used
   * in the conventional way, a reference count of 1 implies that the current
   * thread owns the reference and no other thread shares it. This call performs
   * the test for a reference count of one, and performs the memory barrier
   * needed for the owning thread to act on the resource protected by the
   * reference counter, knowing that it has exclusive access.
   */
  bool HasOneRef() const { return (AtomicOps::AcquireLoad(&ref_count_) == 1); }

 private:
  RefCounter();

 private:
  volatile int ref_count_;
};

/**
 * Agora sample code for wrapping a class that needs to inherit from RefCountInterface in order
 * to be held by agora::agora_refptr 
 * Usage:
 *  agora::agora_refptr<TypeName> ptr = new RefCountedObject<TypeName>(Arg1, Arg2, ...);
 */

template <class T>
class RefCountedObject : public T {
 public:
  RefCountedObject(): ref_count_(0) {}

  template <class P0>
#if __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1800)
  explicit RefCountedObject(P0&& p0) : T(std::forward<P0>(p0)), ref_count_(0) {}
#else
  explicit RefCountedObject(const P0& p0) : T(p0), ref_count_(0) {}
#endif

#if __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1800)
  template <class P0, class P1, class... Args>
  RefCountedObject(P0&& p0, P1&& p1, Args&&... args)
      : T(std::forward<P0>(p0),
          std::forward<P1>(p1),
          std::forward<Args>(args)...),
        ref_count_(0) {}
#endif

  virtual void AddRef() const { ref_count_.IncRef(); }

  virtual agora::RefCountReleaseStatus Release() const {
    const agora::RefCountReleaseStatus status = ref_count_.DecRef();
    if (status == OPTIONAL_REFCOUNTRELEASESTATUS_SPECIFIER kDroppedLastRef) {
      delete this;
    }
    return status;
  }

  /**
   * Return whether the reference count is one. If the reference count is used
   * in the conventional way, a reference count of 1 implies that the current
   * thread owns the reference and no other thread shares it. This call
   * performs the test for a reference count of one, and performs the memory
   * barrier needed for the owning thread to act on the object, knowing that it
   * has exclusive access to the object.
   */
  virtual bool HasOneRef() const { return ref_count_.HasOneRef(); }

 protected:
  virtual ~RefCountedObject() {}

 private:
  RefCountedObject(const RefCountedObject&);
  RefCountedObject& operator=(const RefCountedObject&);

 protected:
  mutable agora::RefCounter ref_count_;
};

#if __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1800)
template <typename T, typename... types>
inline agora_refptr<T> make_refptr(types&&... args) {
  return agora_refptr<T>(new RefCountedObject<T>(std::forward<types>(args)...));
}
#else
template <typename T>
inline agora_refptr<T> make_refptr() {
  return agora_refptr<T>(new RefCountedObject<T>());
}
template <typename T, typename P0>
inline agora_refptr<T> make_refptr(const P0& p0) {
  return agora_refptr<T>(new RefCountedObject<T>(p0));
}
#endif
}  // namespace agora