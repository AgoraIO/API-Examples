//  (C) Copyright Greg Colvin and Beman Dawes 1998, 1999.
//  Copyright (c) 2001, 2002 Peter Dimov
//
//  Permission to copy, use, modify, sell and distribute this software
//  is granted provided this copyright notice appears in all copies.
//  This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.
//
//  See http://www.boost.org/libs/smart_ptr/scoped_ptr.htm for documentation.
//

//  scoped_ptr mimics a built-in pointer except that it guarantees deletion
//  of the object pointed to, either on destruction of the scoped_ptr or via
//  an explicit reset(). scoped_ptr is a simple solution for simple needs;
//  use shared_ptr or std::auto_ptr if your needs are more complex.

//  scoped_ptr_malloc added in by Google.  When one of
//  these goes out of scope, instead of doing a delete or delete[], it
//  calls free().  scoped_ptr_malloc<char> is likely to see much more
//  use than any other specializations.

//  release() added in by Google. Use this to conditionally
//  transfer ownership of a heap-allocated object to the caller, usually on
//  method success.
#ifndef WEBRTC_SYSTEM_WRAPPERS_INTERFACE_SCOPED_PTR_H_
#define WEBRTC_SYSTEM_WRAPPERS_INTERFACE_SCOPED_PTR_H_

#include <assert.h>            // for assert
#include <stddef.h>             // for ptrdiff_t
#include <stdlib.h>            // for free() decl
#include "constructor_magic.h"
#include "template_util.h"
#include "typedefs.h"
#include <algorithm>  // for std::swap

#ifdef _WIN32
namespace std { using ::ptrdiff_t; };
#endif // _WIN32

namespace AgoraRTC {

// Function object which deletes its parameter, which must be a pointer.
// If C is an array type, invokes 'delete[]' on the parameter; otherwise,
// invokes 'delete'. The default deleter for scoped_ptr<T>.
template <class T>
struct DefaultDeleter {
  DefaultDeleter() {}
  template <typename U> DefaultDeleter(const DefaultDeleter<U>& other) {
    // IMPLEMENTATION NOTE: C++11 20.7.1.1.2p2 only provides this constructor
    // if U* is implicitly convertible to T* and U is not an array type.
    //
    // Correct implementation should use SFINAE to disable this
    // constructor. However, since there are no other 1-argument constructors,
    // using a static_assert based on is_convertible<> and requiring
    // complete types is simpler and will cause compile failures for equivalent
    // misuses.
    //
    // Note, the is_convertible<U*, T*> check also ensures that U is not an
    // array. T is guaranteed to be a non-array, so any U* where U is an array
    // cannot convert to T*.
    enum { T_must_be_complete = sizeof(T) };
    enum { U_must_be_complete = sizeof(U) };
    static_assert(is_convertible<U*, T*>::value,
                  "U* must implicitly convert to T*");
  }
  inline void operator()(T* ptr) const {
    enum { type_must_be_complete = sizeof(T) };
    delete ptr;
  }
};

// Specialization of DefaultDeleter for array types.
template <class T>
struct DefaultDeleter<T[]> {
  inline void operator()(T* ptr) const {
    enum { type_must_be_complete = sizeof(T) };
    delete[] ptr;
  }

private:
  // Disable this operator for any U != T because it is undefined to execute
  // an array delete when the static type of the array mismatches the dynamic
  // type.
  //
  // References:
  //   C++98 [expr.delete]p3
  //   http://cplusplus.github.com/LWG/lwg-defects.html#938
  template <typename U> void operator()(U* array) const;
};

// Function object which invokes 'free' on its parameter, which must be
// a pointer. Can be used to store malloc-allocated pointers in scoped_ptr:
//
// scoped_ptr<int, rtc::FreeDeleter> foo_ptr(
//     static_cast<int*>(malloc(sizeof(int))));
struct FreeDeleter {
    inline void operator()(void* ptr) const {
        free(ptr);
    }
};

namespace internal {

    template <typename T>
    struct ShouldAbortOnSelfReset {
        template <typename U>
        static internal::NoType Test(const typename U::AllowSelfReset*);

        template <typename U>
        static internal::YesType Test(...);

        static const bool value =
        sizeof(Test<T>(0)) == sizeof(internal::YesType);
    };

    // Minimal implementation of the core logic of scoped_ptr, suitable for
    // reuse in both scoped_ptr and its specializations.
    template <class T, class D>
    class scoped_ptr_impl {
    public:
        explicit scoped_ptr_impl(T* p) : data_(p) {}

        // Initializer for deleters that have data parameters.
        scoped_ptr_impl(T* p, const D& d) : data_(p, d) {}

        // Templated constructor that destructively takes the value from another
        // scoped_ptr_impl.
        template <typename U, typename V>
        scoped_ptr_impl(scoped_ptr_impl<U, V>* other)
        : data_(other->release(), other->get_deleter()) {
            // We do not support move-only deleters.  We could modify our move
            // emulation to have rtc::subtle::move() and rtc::subtle::forward()
            // functions that are imperfect emulations of their C++11 equivalents,
            // but until there's a requirement, just assume deleters are copyable.
        }

        template <typename U, typename V>
        void TakeState(scoped_ptr_impl<U, V>* other) {
            // See comment in templated constructor above regarding lack of support
            // for move-only deleters.
            reset(other->release());
            get_deleter() = other->get_deleter();
        }

        ~scoped_ptr_impl() {
            if (data_.ptr != NULL) {
                // Not using get_deleter() saves one function call in non-optimized
                // builds.
                static_cast<D&>(data_)(data_.ptr);
            }
        }

        void reset(T* p) {
            // This is a self-reset, which is no longer allowed for default deleters:
            // https://crbug.com/162971
            assert(!ShouldAbortOnSelfReset<D>::value || p == NULL || p != data_.ptr);

            // Note that running data_.ptr = p can lead to undefined behavior if
            // get_deleter()(get()) deletes this. In order to prevent this, reset()
            // should update the stored pointer before deleting its old value.
            //
            // However, changing reset() to use that behavior may cause current code to
            // break in unexpected ways. If the destruction of the owned object
            // dereferences the scoped_ptr when it is destroyed by a call to reset(),
            // then it will incorrectly dispatch calls to |p| rather than the original
            // value of |data_.ptr|.
            //
            // During the transition period, set the stored pointer to NULL while
            // deleting the object. Eventually, this safety check will be removed to
            // prevent the scenario initially described from occurring and
            // http://crbug.com/176091 can be closed.
            T* old = data_.ptr;
            data_.ptr = NULL;
            if (old != NULL)
                static_cast<D&>(data_)(old);
            data_.ptr = p;
        }

        T* get() const { return data_.ptr; }

        D& get_deleter() { return data_; }
        const D& get_deleter() const { return data_; }

        void swap(scoped_ptr_impl& p2) {
            // Standard swap idiom: 'using std::swap' ensures that std::swap is
            // present in the overload set, but we call swap unqualified so that
            // any more-specific overloads can be used, if available.
            using std::swap;
            swap(static_cast<D&>(data_), static_cast<D&>(p2.data_));
            swap(data_.ptr, p2.data_.ptr);
        }

        T* release() {
            T* old_ptr = data_.ptr;
            data_.ptr = NULL;
            return old_ptr;
        }

        T** accept() {
            reset(NULL);
            return &(data_.ptr);
        }

        T** use() {
            return &(data_.ptr);
        }

    private:
        // Needed to allow type-converting constructor.
        template <typename U, typename V> friend class scoped_ptr_impl;

        // Use the empty base class optimization to allow us to have a D
        // member, while avoiding any space overhead for it when D is an
        // empty class.  See e.g. http://www.cantrip.org/emptyopt.html for a good
        // discussion of this technique.
        struct Data : public D {
            explicit Data(T* ptr_in) : ptr(ptr_in) {}
            Data(T* ptr_in, const D& other) : D(other), ptr(ptr_in) {}
            T* ptr;
        };

        Data data_;

        DISALLOW_COPY_AND_ASSIGN(scoped_ptr_impl);
    };

}  // namespace internal

template <typename T>
class scoped_ptr {
 private:

  T* ptr;

  scoped_ptr(scoped_ptr const &);
  scoped_ptr & operator=(scoped_ptr const &);

 public:

  typedef T element_type;

  explicit scoped_ptr(T* p = NULL): ptr(p) {}
  scoped_ptr(scoped_ptr &&rhs) {
    ptr = rhs.ptr;
    rhs.ptr = NULL;
  }
  
  scoped_ptr& operator=(scoped_ptr &&rhs) {
    if (this != &rhs) {
      ptr = rhs.ptr;
      rhs.ptr = NULL;
    }
    
    return *this;
  }
  
  ~scoped_ptr() {
    typedef char type_must_be_complete[sizeof(T)];
    delete ptr;
  }

  void reset(T* p = NULL) {
    typedef char type_must_be_complete[sizeof(T)];

    if (ptr != p) {
      T* obj = ptr;
      ptr = p;
      // Delete last, in case obj destructor indirectly results in ~scoped_ptr
      delete obj;
    }
  }

  T& operator*() const {
    assert(ptr != NULL);
    return *ptr;
  }

  T* operator->() const  {
    assert(ptr != NULL);
    return ptr;
  }

  T* get() const  {
    return ptr;
  }

  void swap(scoped_ptr & b) {
    T* tmp = b.ptr;
    b.ptr = ptr;
    ptr = tmp;
  }

  T* release() {
    T* tmp = ptr;
    ptr = NULL;
    return tmp;
  }

  T** accept() {
    if (ptr) {
      delete ptr;
      ptr = NULL;
    }
    return &ptr;
  }

  T** use() {
    return &ptr;
  }
};

template<typename T> inline
void swap(scoped_ptr<T>& a, scoped_ptr<T>& b) {
  a.swap(b);
}




//  scoped_array extends scoped_ptr to arrays. Deletion of the array pointed to
//  is guaranteed, either on destruction of the scoped_array or via an explicit
//  reset(). Use shared_array or std::vector if your needs are more complex.

template<typename T>
class scoped_array {
 private:

  T* ptr;

  scoped_array(scoped_array const &);
  scoped_array & operator=(scoped_array const &);

 public:

  typedef T element_type;

  explicit scoped_array(T* p = NULL) : ptr(p) {}

  ~scoped_array() {
    typedef char type_must_be_complete[sizeof(T)];
    delete[] ptr;
  }

  void reset(T* p = NULL) {
    typedef char type_must_be_complete[sizeof(T)];

    if (ptr != p) {
      T* arr = ptr;
      ptr = p;
      // Delete last, in case arr destructor indirectly results in ~scoped_array
      delete [] arr;
    }
  }

  T& operator[](ptrdiff_t i) const {
    assert(ptr != NULL);
    assert(i >= 0);
    return ptr[i];
  }

  T* get() const {
    return ptr;
  }

  void swap(scoped_array & b) {
    T* tmp = b.ptr;
    b.ptr = ptr;
    ptr = tmp;
  }

  T* release() {
    T* tmp = ptr;
    ptr = NULL;
    return tmp;
  }

  T** accept() {
    if (ptr) {
      delete [] ptr;
      ptr = NULL;
    }
    return &ptr;
  }
};

template<class T> inline
void swap(scoped_array<T>& a, scoped_array<T>& b) {
  a.swap(b);
}

// scoped_ptr_malloc<> is similar to scoped_ptr<>, but it accepts a
// second template argument, the function used to free the object.

template<typename T, void (*FF)(void*) = free> class scoped_ptr_malloc {
 private:

  T* ptr;

  scoped_ptr_malloc(scoped_ptr_malloc const &);
  scoped_ptr_malloc & operator=(scoped_ptr_malloc const &);

 public:

  typedef T element_type;

  explicit scoped_ptr_malloc(T* p = 0): ptr(p) {}

  ~scoped_ptr_malloc() {
    FF(static_cast<void*>(ptr));
  }

  void reset(T* p = 0) {
    if (ptr != p) {
      FF(static_cast<void*>(ptr));
      ptr = p;
    }
  }

  T& operator*() const {
    assert(ptr != 0);
    return *ptr;
  }

  T* operator->() const {
    assert(ptr != 0);
    return ptr;
  }

  T* get() const {
    return ptr;
  }

  void swap(scoped_ptr_malloc & b) {
    T* tmp = b.ptr;
    b.ptr = ptr;
    ptr = tmp;
  }

  T* release() {
    T* tmp = ptr;
    ptr = 0;
    return tmp;
  }

  T** accept() {
    if (ptr) {
      FF(static_cast<void*>(ptr));
      ptr = 0;
    }
    return &ptr;
  }
};

template<typename T, void (*FF)(void*)> inline
void swap(scoped_ptr_malloc<T,FF>& a, scoped_ptr_malloc<T,FF>& b) {
  a.swap(b);
}

}  // namespace AgoraRTC

namespace AgoraAPM {
  template <class T, class D = AgoraRTC::DefaultDeleter<T> >
  class scoped_ptr {

    // TODO(ajm): If we ever import RefCountedBase, this check needs to be
    // enabled.
    //static_assert(rtc::internal::IsNotRefCounted<T>::value,
    //              "T is refcounted type and needs scoped refptr");

  public:
    // The element and deleter types.
    typedef T element_type;
    typedef D deleter_type;

    // Constructor.  Takes ownership of p.
    explicit scoped_ptr(element_type* p=NULL) : impl_(p) {}

    // Constructor.  Allows initialization of a stateful deleter.
    scoped_ptr(element_type* p, const D& d) : impl_(p, d) {}

    // Constructor.  Allows construction from a scoped_ptr rvalue for a
    // convertible type and deleter.
    //
    // IMPLEMENTATION NOTE: C++11 unique_ptr<> keeps this constructor distinct
    // from the normal move constructor. By C++11 20.7.1.2.1.21, this constructor
    // has different post-conditions if D is a reference type. Since this
    // implementation does not support deleters with reference type,
    // we do not need a separate move constructor allowing us to avoid one
    // use of SFINAE. You only need to care about this if you modify the
    // implementation of scoped_ptr.
//        template <typename U, typename V>
//        scoped_ptr(scoped_ptr<U, V>&& other)
//        : impl_(&other.impl_) {
//            // static_assert(!AgoraRTC::is_array<U>::value, "U cannot be an array");
//        }
//
//        // operator=.  Allows assignment from a scoped_ptr rvalue for a convertible
//        // type and deleter.
//        //
//        // IMPLEMENTATION NOTE: C++11 unique_ptr<> keeps this operator= distinct from
//        // the normal move assignment operator. By C++11 20.7.1.2.3.4, this templated
//        // form has different requirements on for move-only Deleters. Since this
//        // implementation does not support move-only Deleters, we do not need a
//        // separate move assignment operator allowing us to avoid one use of SFINAE.
//        // You only need to care about this if you modify the implementation of
//        // scoped_ptr.
//        template <typename U, typename V>
//        scoped_ptr& operator=(scoped_ptr<U, V>&& rhs) {
//            // static_assert(!AgoraRTC::is_array<U>::value, "U cannot be an array");
//            impl_.TakeState(&rhs.impl_);
//            return *this;
//        }

      // Deleted copy constructor and copy assignment, to make the type move-only.
  private:
    scoped_ptr(const scoped_ptr& other);
    scoped_ptr& operator=(const scoped_ptr& other);
  public:
    // Reset.  Deletes the currently owned object, if any.
    // Then takes ownership of a new object, if given.
    void reset(element_type* p = NULL) { impl_.reset(p); }

    // Accessors to get the owned object.
    // operator* and operator-> will assert() if there is no current object.
    element_type& operator*() const {
        assert(impl_.get() != NULL);
        return *impl_.get();
    }
    element_type* operator->() const  {
        assert(impl_.get() != NULL);
        return impl_.get();
    }
    element_type* get() const { return impl_.get(); }

    // Access to the deleter.
    deleter_type& get_deleter() { return impl_.get_deleter(); }
    const deleter_type& get_deleter() const { return impl_.get_deleter(); }

    // Allow scoped_ptr<element_type> to be used in boolean expressions, but not
    // implicitly convertible to a real bool (which is dangerous).
    //
    // Note that this trick is only safe when the == and != operators
    // are declared explicitly, as otherwise "scoped_ptr1 ==
    // scoped_ptr2" will compile but do the wrong thing (i.e., convert
    // to Testable and then do the comparison).
  private:
    typedef AgoraRTC::internal::scoped_ptr_impl<element_type, deleter_type>
    scoped_ptr::*Testable;

  public:
    operator Testable() const {
        return impl_.get() ? &scoped_ptr::impl_ : NULL;
    }

    // Comparison operators.
    // These return whether two scoped_ptr refer to the same object, not just to
    // two different but equal objects.
    bool operator==(const element_type* p) const { return impl_.get() == p; }
    bool operator!=(const element_type* p) const { return impl_.get() != p; }

    // Swap two scoped pointers.
    void swap(scoped_ptr& p2) {
        impl_.swap(p2.impl_);
    }

    // Release a pointer.
    // The return value is the current pointer held by this object. If this object
    // holds a NULL, the return value is NULL. After this operation, this
    // object will hold a NULL, and will not own the object any more.
    element_type* release() WARN_UNUSED_RESULT {
        return impl_.release();
    }

    // Delete the currently held pointer and return a pointer
    // to allow overwriting of the current pointer address.
    element_type** accept() WARN_UNUSED_RESULT {
        return impl_.accept();
    }

    // Return a pointer to the current pointer address.
    element_type** use() WARN_UNUSED_RESULT {
        return impl_.use();
    }

  private:
    // Needed to reach into |impl_| in the constructor.
    template <typename U, typename V> friend class scoped_ptr;
    AgoraRTC::internal::scoped_ptr_impl<element_type, deleter_type> impl_;

    // Forbidden for API compatibility with std::unique_ptr.
    explicit scoped_ptr(int disallow_construction_from_null);

    // Forbid comparison of scoped_ptr types.  If U != T, it totally
    // doesn't make sense, and if U == T, it still doesn't make sense
    // because you should never have the same object owned by two different
    // scoped_ptrs.
    template <class U> bool operator==(scoped_ptr<U> const& p2) const;
    template <class U> bool operator!=(scoped_ptr<U> const& p2) const;
  };

  template <class T, class D>
  class scoped_ptr<T[], D> {
  public:
    // The element and deleter types.
    typedef T element_type;
    typedef D deleter_type;

    // Constructor. Stores the given array. Note that the argument's type
    // must exactly match T*. In particular:
    // - it cannot be a pointer to a type derived from T, because it is
    //   inherently unsafe in the general case to access an array through a
    //   pointer whose dynamic type does not match its static type (eg., if
    //   T and the derived types had different sizes access would be
    //   incorrectly calculated). Deletion is also always undefined
    //   (C++98 [expr.delete]p3). If you're doing this, fix your code.
    // - it cannot be const-qualified differently from T per unique_ptr spec
    //   (http://cplusplus.github.com/LWG/lwg-active.html#2118). Users wanting
    //   to work around this may use implicit_cast<const T*>().
    //   However, because of the first bullet in this comment, users MUST
    //   NOT use implicit_cast<Base*>() to upcast the static type of the array.
    explicit scoped_ptr(element_type* array=NULL) : impl_(array) {}

    // operator=.  Allows assignment from a NULL. Deletes the currently owned
    // array, if any.
    scoped_ptr& operator=(element_type *t) {
        reset(t);
        return *this;
    }
  private:
    // Deleted copy constructor and copy assignment, to make the type move-only.
    scoped_ptr(const scoped_ptr& other);
    scoped_ptr& operator=(const scoped_ptr& other);
  public:
    // Reset.  Deletes the currently owned array, if any.
    // Then takes ownership of a new object, if given.
    void reset(element_type* array = NULL) { impl_.reset(array); }

    // Accessors to get the owned array.
    element_type& operator[](size_t i) const {
        assert(impl_.get() != NULL);
        return impl_.get()[i];
    }
    element_type* get() const { return impl_.get(); }

    // Access to the deleter.
    deleter_type& get_deleter() { return impl_.get_deleter(); }
    const deleter_type& get_deleter() const { return impl_.get_deleter(); }

    // Allow scoped_ptr<element_type> to be used in boolean expressions, but not
    // implicitly convertible to a real bool (which is dangerous).
  private:
    typedef AgoraRTC::internal::scoped_ptr_impl<element_type, deleter_type>
    scoped_ptr::*Testable;

  public:
    operator Testable() const {
        return impl_.get() ? &scoped_ptr::impl_ : NULL;
    }

    // Comparison operators.
    // These return whether two scoped_ptr refer to the same object, not just to
    // two different but equal objects.
    bool operator==(element_type* array) const { return impl_.get() == array; }
    bool operator!=(element_type* array) const { return impl_.get() != array; }

    // Swap two scoped pointers.
    void swap(scoped_ptr& p2) {
        impl_.swap(p2.impl_);
    }

    // Release a pointer.
    // The return value is the current pointer held by this object. If this object
    // holds a NULL, the return value is NULL. After this operation, this
    // object will hold a NULL, and will not own the object any more.
    element_type* release() WARN_UNUSED_RESULT {
    return impl_.release();
  }

  // Delete the currently held pointer and return a pointer
  // to allow overwriting of the current pointer address.
  element_type** accept() WARN_UNUSED_RESULT {
  return impl_.accept();
}

// Return a pointer to the current pointer address.
element_type** use() WARN_UNUSED_RESULT {
  return impl_.use();
}

private:
  // Force element_type to be a complete type.
  enum { type_must_be_complete = sizeof(element_type) };

  // Actually hold the data.
  AgoraRTC::internal::scoped_ptr_impl<element_type, deleter_type> impl_;

  // Disable initialization from any type other than element_type*, by
  // providing a constructor that matches such an initialization, but is
  // private and has no definition. This is disabled because it is not safe to
  // call delete[] on an array whose static type does not match its dynamic
  // type.
  template <typename U> explicit scoped_ptr(U* array);
  explicit scoped_ptr(int disallow_construction_from_null);

  // Disable reset() from any type other than element_type*, for the same
  // reasons as the constructor above.
  template <typename U> void reset(U* array);
  void reset(int disallow_reset_from_null);

  // Forbid comparison of scoped_ptr types.  If U != T, it totally
  // doesn't make sense, and if U == T, it still doesn't make sense
  // because you should never have the same object owned by two different
  // scoped_ptrs.
  template <class U> bool operator==(scoped_ptr<U> const& p2) const;
  template <class U> bool operator!=(scoped_ptr<U> const& p2) const;
};
}

#endif  // #ifndef WEBRTC_SYSTEM_WRAPPERS_INTERFACE_SCOPED_PTR_H_
