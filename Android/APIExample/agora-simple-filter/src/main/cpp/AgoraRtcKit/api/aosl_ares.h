/*************************************************************
 * Author:	Lionfore Hao (haolianfu@agora.io)
 * Date	 :	May 30th, 2023
 * Module:	AOSL async result object header file
 *
 *
 * This is a part of the Advanced Operating System Layer.
 * Copyright (C) 2018 ~ 2023 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#ifndef __AOSL_ARES_H__
#define __AOSL_ARES_H__

#include <api/aosl_types.h>
#include <api/aosl_defs.h>
#include <api/aosl_ref.h>
#include <api/aosl_poll.h>

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Create an async result object.
 * Parameter:
 *          arg: the parameter attached with the async result object;
 * Return value:
 *     the async result object ref id just created, AOSL_REF_INVALID when failed.
 * Remarks:
 *     ares is an async-result synchronization primitive:
 *     1. one side completes it with aosl_ares_complete();
 *     2. another side waits or polls for completion;
 *     3. it can be reused through aosl_ares_reset().
 *     Unlike most long-lived refs, ares objects typically do not need
 *     destroy_wait behavior because they are meant to model completion state,
 *     not long business callbacks.
 **/
extern __aosl_api__ aosl_ref_t aosl_ares_create (void *arg);

/**
 * Create an async result object with specified flags.
 * Parameter:
 *        flags: creating flags bitmask, which specified some special attributes of the ares object;
 *          arg: the parameter attached with the async result object;
 * Return value:
 *     the async result object ref id just created, AOSL_REF_INVALID when failed.
 * Remarks:
 *     This is the flagged form of aosl_ares_create(). In practice, ares is
 *     often used as a short-lived completion primitive, so callers typically
 *     care more about the completion/reset lifecycle than about long-lived
 *     ref-style locking behavior.
 **/
extern __aosl_api__ aosl_ref_t aosl_ares_create_flags (int flags, void *arg);

/**
 * Complete the specified async result object.
 * Parameters:
 *      ref: the async result object ref id;
 *   result: a result value which can be retrieved by wait function;
 * Return value:
 *        <0: error occured, and aosl_errno indicates which error;
 *       >=0: successful;
 * Remarks:
 *     Completing an ares wakes current/future waiters and records the result
 *     value that aosl_ares_wait() can later retrieve.
 **/
extern __aosl_api__ int aosl_ares_complete (aosl_ref_t ref, intptr_t result);

/**
 * Complete the async result object specified with ref + magic.
 * Parameters:
 *      ref: the async result object ref id;
 *    magic: the saved magic of the async result object;
 *   result: a result value which can be retrieved by wait function;
 * Return value:
 *        <0: error occured, and aosl_errno indicates which error;
 *       >=0: successful;
 * Remarks:
 *     Use the magic form when the completion signal may race with object reuse
 *     or replacement and the caller needs ABA-style protection.
 **/
extern __aosl_api__ int aosl_ares_magic_complete (aosl_ref_t ref, aosl_ref_magic_t magic, intptr_t result);

/**
 * Wait the specified async result object to complete.
 * Parameters:
 *      ref: the async result object ref id;
 *    timeo: maximum waiting time in milliseconds;
 *   result: variable address for the value which was set by complete function,
 *           NOTE: the *result only will be set when the return value of wait
 *                 function is AOSL_POLL_ST_SIGNALED and result != NULL, if you
 *                 do not care the complete result, just passing NULL to it;
 * Return value:
 *        <0: error occured, and aosl_errno indicates which error;
 *       >=0: AOSL_POLL_ST_* macros value;
 * Remarks:
 *     This API returns a poll-style state rather than a plain boolean:
 *     AOSL_POLL_ST_SIGNALED means completed normally,
 *     AOSL_POLL_ST_DESTROY means the ares object was destroyed,
 *     and AOSL_POLL_ST_NONE means still unsignaled in nonblocking cases.
 *     If timeo is 0, the call behaves like a nonblocking state probe.
 **/
extern __aosl_api__ int aosl_ares_wait (aosl_ref_t ref, intptr_t timeo, intptr_t *result);

/**
 * Reset the specified async result object to non signaled state.
 * Parameters:
 *     ref: the async result object ref id
 * Return value:
 *        <0: error occured, and aosl_errno indicates which error;
 *       >=0: successful;
 * Remarks:
 *     Reset is what makes an ares reusable across multiple async rounds.
 **/
extern __aosl_api__ int aosl_ares_reset (aosl_ref_t ref);



#ifdef __cplusplus
}
#endif

#endif /* __AOSL_ARES_H__ */
