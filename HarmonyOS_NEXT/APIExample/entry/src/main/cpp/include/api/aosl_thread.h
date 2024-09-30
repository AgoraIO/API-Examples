/*************************************************************
 * Author:	Lionfore Hao (haolianfu@agora.io)
 * Date	 :	Sep 22nd, 2018
 * Module:	AOSL thread relative definitions
 *
 *
 * This is a part of the Advanced Operating System Layer.
 * Copyright (C) 2018 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#ifndef __AOSL_THREAD_H__
#define __AOSL_THREAD_H__

#include <api/aosl_types.h>
#include <api/aosl_defs.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef uintptr_t aosl_thread_t;
typedef uintptr_t aosl_thrdid_t;
typedef uintptr_t aosl_osthrd_t;

typedef void (*aosl_thread_entry_t) (void *);

#define AOSL_THRD_PRI_DEFAULT 0
#define AOSL_THRD_PRI_LOW 1
#define AOSL_THRD_PRI_NORMAL 2
#define AOSL_THRD_PRI_HIGH 3
#define AOSL_THRD_PRI_HIGHEST 4
#define AOSL_THRD_PRI_RT 5

extern __aosl_api__ aosl_thread_t aosl_thread_self (void);
extern __aosl_api__ aosl_thrdid_t aosl_thread_self_id (void);

/**
 * Get the mpq thread name.
 * Parameters:
 *       thrd: the retrieving thread id
 *             Linux: the thread tid
 *             XNU: the mach thread id, type is mach_port_t
 *             Windows: the Windows thread id, type is DWORD
 *    namebuf: the buf to hold the name
 *     buflen: the name buf length in bytes
 * Return Value:
 *          0: success
 *         <0: failure with aosl_errno set
 **/
extern __aosl_api__ int aosl_thrd_getname (aosl_thrdid_t thrd, char *namebuf, size_t buflen);

/**
 * Get the mpq thread name.
 * Parameters:
 *       thrd: the retrieving thread os kernel identifier
 *             Linux: the thread tid
 *             XNU: the mach thread id, type is mach_port_t
 *             Windows: the Windows kernel thread HANDLE, which must have
 *                      THREAD_QUERY_INFORMATION access right
 *    running: the variable buf of the indicator which tells whether thread
 *             is in running state, specify NULL for not care
 *  system_us: the thread running time in kernel mode, micro seconds,
 *             NULL for not care
 *    user_us: the thread running time in user mode, micro seconds,
 *             NULL for not care
 * Return Value:
 *          0: success
 *         <0: failure with aosl_errno set
 **/
extern __aosl_api__ int aosl_get_thread_times (aosl_osthrd_t thrd, int *running, uint64_t *system_us, uint64_t *user_us);

/**
 * Get the mpq thread flags.
 * Parameters:
 *       thrd: the retrieving thread id
 *             Linux: the thread tid
 *             XNU: the mach thread id, type is mach_port_t
 *             Windows: the Windows thread id, type is DWORD
 *    q_flags: the mpq flags
 * Return Value:
 *          0: success
 *         <0: failure with aosl_errno set
 **/
extern __aosl_api__ int aosl_thrd_getflags (aosl_thrdid_t thrd_id, int *q_flags);

typedef uintptr_t aosl_tls_key_t;

extern __aosl_api__ int aosl_tls_key_create (aosl_tls_key_t *key, void (*dtor) (void *));
extern __aosl_api__ void *aosl_tls_key_get (aosl_tls_key_t key);
extern __aosl_api__ int aosl_tls_key_set (aosl_tls_key_t key, void *value);
extern __aosl_api__ int aosl_tls_key_delete (aosl_tls_key_t key);


typedef void *aosl_lock_t;

extern __aosl_api__ aosl_lock_t aosl_lock_create (int recursive);
extern __aosl_api__ void aosl_lock_lock (aosl_lock_t lock);
extern __aosl_api__ int aosl_lock_trylock (aosl_lock_t lock);
extern __aosl_api__ void aosl_lock_unlock (aosl_lock_t lock);
extern __aosl_api__ void aosl_lock_destroy (aosl_lock_t lock);


typedef void *aosl_rwlock_t;

extern __aosl_api__ aosl_rwlock_t aosl_rwlock_create (void);
extern __aosl_api__ void aosl_rwlock_rdlock (aosl_rwlock_t rwlock);
extern __aosl_api__ int aosl_rwlock_tryrdlock (aosl_rwlock_t rwlock);
extern __aosl_api__ void aosl_rwlock_wrlock (aosl_rwlock_t rwlock);
extern __aosl_api__ int aosl_rwlock_trywrlock (aosl_rwlock_t rwlock);
extern __aosl_api__ void aosl_rwlock_rdunlock (aosl_rwlock_t rwlock);
extern __aosl_api__ void aosl_rwlock_wrunlock (aosl_rwlock_t rwlock);
extern __aosl_api__ void aosl_rwlock_rd2wrlock (aosl_rwlock_t rwlock);
extern __aosl_api__ void aosl_rwlock_wr2rdlock (aosl_rwlock_t rwlock);
extern __aosl_api__ void aosl_rwlock_destroy (aosl_rwlock_t rwlock);


typedef void *aosl_cond_t;

extern __aosl_api__ aosl_cond_t aosl_cond_create (void);
extern __aosl_api__ void aosl_cond_signal (aosl_cond_t cond_var);
extern __aosl_api__ void aosl_cond_broadcast (aosl_cond_t cond_var);
extern __aosl_api__ void aosl_cond_wait (aosl_cond_t cond_var, aosl_lock_t lock);
extern __aosl_api__ int aosl_cond_timedwait (aosl_cond_t cond_var, aosl_lock_t lock, intptr_t timeo);
extern __aosl_api__ void aosl_cond_destroy (aosl_cond_t cond_var);

typedef void *aosl_event_t;

extern __aosl_api__ aosl_event_t aosl_event_create (void);
extern __aosl_api__ void aosl_event_set (aosl_event_t event_var);
extern __aosl_api__ void aosl_event_pulse (aosl_event_t event_var);
extern __aosl_api__ void aosl_event_wait (aosl_event_t event_var);
extern __aosl_api__ int aosl_event_timedwait (aosl_event_t event_var, intptr_t timeo);
extern __aosl_api__ void aosl_event_reset (aosl_event_t event_var);
extern __aosl_api__ void aosl_event_destroy (aosl_event_t event_var);


#ifdef __cplusplus
}
#endif

#endif /* __AOSL_THREAD_H__ */