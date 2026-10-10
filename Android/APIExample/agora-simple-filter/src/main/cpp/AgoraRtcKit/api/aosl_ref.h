/*************************************************************
 * Author:	Lionfore Hao (haolianfu@agora.io)
 * Date	 :	Nov 19th, 2018
 * Module:	AOSL reference object definition file
 *
 *
 * This is a part of the Advanced Operating System Layer.
 * Copyright (C) 2018 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#ifndef __AOSL_REF_H__
#define __AOSL_REF_H__


#include <api/aosl_types.h>
#include <api/aosl_defs.h>


#ifdef __cplusplus
extern "C" {
#endif



typedef struct _internal_ref_od_ *aosl_ref_t;

#define AOSL_REF_INVALID ((aosl_ref_t)(intptr_t)NULL)

#define aosl_ref_invalid(ref) ((aosl_ref_t)(ref) == AOSL_REF_INVALID)


/**
 * The reference object destructor function prototype, which invoked when application
 * calling aosl_ref_destroy functions to release resources.
 * Parameter:
 *       arg: the parameter passed in when creating the reference object;
 * Return value:
 *       none.
 **/
typedef void (*aosl_ref_dtor_t) (void *arg);

/**
 * The reference object creating function prototype, which is used to create a ref object.
 * Parameters:
 *              arg: the parameter attached with the reference object;
 *             dtor: the ref object destructor function, which will be invoked when
 *                   the ref object is deleted;
 *     destroy_wait:
 *            none-0 the destroy caller will wait other threads to release the ref object;
 *                 0 the destroy caller will not wait other threads;
 * Return value:
 *         the ref object id, please use aosl_ref_invalid macro to check whether failed.
 * Remarks:
 *     destroy_wait is one of the most important lifetime policy switches:
 *     1. if non-zero, aosl_ref_destroy() waits for in-flight normal ref ops
 *        such as hold/read/write to finish before the destroy completes;
 *     2. if 0, destroy only marks the object destroyed and does not wait for
 *        those in-flight users.
 *     AOSL ares objects are a typical case that usually do not need waiting.
 **/
extern __aosl_api__ aosl_ref_t aosl_ref_create (void *arg, aosl_ref_dtor_t dtor, int destroy_wait);

#define AOSL_REF_NEWID 0x00000001

/**
 * The reference object creating with flags function prototype, which is used to create a ref object.
 * Parameters:
 *            flags: creating flags bitmask, which specified some special attributes of the ref object;
 *              arg: the parameter attached with the reference object;
 *             dtor: the ref object destructor function, which will be invoked when
 *                   the ref object is deleted;
 *     destroy_wait:
 *            none-0 the destroy caller will wait other threads to release the ref object;
 *                 0 the destroy caller will not wait other threads;
 * Return value:
 *         the ref object id, please use aosl_ref_invalid macro to check whether failed.
 **/
extern __aosl_api__ aosl_ref_t aosl_ref_create_flags (int flags, void *arg, aosl_ref_dtor_t dtor, int destroy_wait);

/**
 * Returns the total ref objects count.
 **/
extern __aosl_api__ int aosl_ref_count (void);


typedef uintptr_t aosl_ref_magic_t;
#define AOSL_REF_MAGIC_INVALID ((aosl_ref_magic_t)0)

/**
 * Retrieve the reference object magic function prototype.
 * Parameters:
 *              ref: the ref object id;
 *            magic: the variable address for storing the magic;
 * Return value:
 *            0: success
 *           <0: failure with aosl_errno set
 **/
extern __aosl_api__ int aosl_ref_magic (aosl_ref_t ref, aosl_ref_magic_t *magic);

/**
 * The ref object callback function prototype.
 * Parameters:
 *            arg: the ref object argument which was passed in when creating;
 *           argc: specify the argv array elements count, the same as the argc
 *                    when invoking aosl_ref_[get|read|write] functions;
 *           argv: array for passing variable args, the same as the args
 *                    when invoking aosl_task_exec_* functions;
 * Return value:
 *       none.
 **/
typedef void (*aosl_ref_func_t) (void *arg, uintptr_t argc, uintptr_t argv []);

/**
 * Hold the ref object, and invoke the specified callback function.
 * Parameters:
 *            ref: the ref object id;
 *              f: the callback function;
 *           argc: the args count
 *            ...: variable args
 * Return value:
 *            0: success
 *           <0: failure with aosl_errno set
 * Remarks:
 *     hold protects the object lifetime by usage count, but does not take the
 *     internal read/write lock. For destroy_wait refs, an already-running hold
 *     callback still delays aosl_ref_destroy() until the callback returns.
 **/
extern __aosl_api__ int aosl_ref_hold (aosl_ref_t ref, aosl_ref_func_t f, uintptr_t argc, ...);
extern __aosl_api__ int aosl_ref_hold_args (aosl_ref_t ref, aosl_ref_func_t f, uintptr_t argc, va_list args);
extern __aosl_api__ int aosl_ref_hold_argv (aosl_ref_t ref, aosl_ref_func_t f, uintptr_t argc, uintptr_t argv []);

/**
 * Read lock the ref object and read lock it, then invoke the specified callback function.
 * Parameters:
 *            ref: the ref object id;
 *              f: the callback function;
 *           argc: the args count
 *            ...: variable args
 * Return value:
 *            0: success
 *           <0: failure with aosl_errno set
 * Remarks:
 *     Multiple read callbacks may run concurrently. For destroy_wait refs,
 *     aosl_ref_destroy() waits until in-flight read callbacks return.
 **/
extern __aosl_api__ int aosl_ref_read (aosl_ref_t ref, aosl_ref_func_t f, uintptr_t argc, ...);
extern __aosl_api__ int aosl_ref_read_args (aosl_ref_t ref, aosl_ref_func_t f, uintptr_t argc, va_list args);
extern __aosl_api__ int aosl_ref_read_argv (aosl_ref_t ref, aosl_ref_func_t f, uintptr_t argc, uintptr_t argv []);

/**
 * Write lock the ref object and write lock it, then invoke the specified callback function.
 * Parameters:
 *            ref: the ref object id;
 *              f: the callback function;
 *           argc: the args count
 *            ...: variable args
 * Return value:
 *            0: success
 *           <0: failure with aosl_errno set
 * Remarks:
 *     write is exclusive against other read/write callbacks. For destroy_wait
 *     refs, aosl_ref_destroy() waits until the in-flight write callback
 *     returns.
 **/
extern __aosl_api__ int aosl_ref_write (aosl_ref_t ref, aosl_ref_func_t f, uintptr_t argc, ...);
extern __aosl_api__ int aosl_ref_write_args (aosl_ref_t ref, aosl_ref_func_t f, uintptr_t argc, va_list args);
extern __aosl_api__ int aosl_ref_write_argv (aosl_ref_t ref, aosl_ref_func_t f, uintptr_t argc, uintptr_t argv []);

/**
 * Hold the ref object with the saved magic, and invoke the specified callback function.
 * Parameters:
 *            ref: the ref object id;
 *          magic: the saved magic of the ref object;
 *              f: the callback function;
 *           argc: the args count
 *            ...: variable args
 * Return value:
 *            0: success
 *           <0: failure with aosl_errno set
 **/
extern __aosl_api__ int aosl_ref_magic_hold (aosl_ref_t ref, aosl_ref_magic_t magic, aosl_ref_func_t f, uintptr_t argc, ...);
extern __aosl_api__ int aosl_ref_magic_hold_args (aosl_ref_t ref, aosl_ref_magic_t magic, aosl_ref_func_t f, uintptr_t argc, va_list args);
extern __aosl_api__ int aosl_ref_magic_hold_argv (aosl_ref_t ref, aosl_ref_magic_t magic, aosl_ref_func_t f, uintptr_t argc, uintptr_t argv []);

/**
 * Read lock the ref object with the saved magic, and invoke the specified callback function.
 * Parameters:
 *            ref: the ref object id;
 *          magic: the saved magic of the ref object;
 *              f: the callback function;
 *           argc: the args count
 *            ...: variable args
 * Return value:
 *            0: success
 *           <0: failure with aosl_errno set
 **/
extern __aosl_api__ int aosl_ref_magic_read (aosl_ref_t ref, aosl_ref_magic_t magic, aosl_ref_func_t f, uintptr_t argc, ...);
extern __aosl_api__ int aosl_ref_magic_read_args (aosl_ref_t ref, aosl_ref_magic_t magic, aosl_ref_func_t f, uintptr_t argc, va_list args);
extern __aosl_api__ int aosl_ref_magic_read_argv (aosl_ref_t ref, aosl_ref_magic_t magic, aosl_ref_func_t f, uintptr_t argc, uintptr_t argv []);

/**
 * Write lock the ref object with the saved magic, and invoke the specified callback function.
 * Parameters:
 *            ref: the ref object id;
 *          magic: the saved magic of the ref object;
 *              f: the callback function;
 *           argc: the args count
 *            ...: variable args
 * Return value:
 *            0: success
 *           <0: failure with aosl_errno set
 **/
extern __aosl_api__ int aosl_ref_magic_write (aosl_ref_t ref, aosl_ref_magic_t magic, aosl_ref_func_t f, uintptr_t argc, ...);
extern __aosl_api__ int aosl_ref_magic_write_args (aosl_ref_t ref, aosl_ref_magic_t magic, aosl_ref_func_t f, uintptr_t argc, va_list args);
extern __aosl_api__ int aosl_ref_magic_write_argv (aosl_ref_t ref, aosl_ref_magic_t magic, aosl_ref_func_t f, uintptr_t argc, uintptr_t argv []);

/**
 * Potential read unlock the ref object and set it unsafe, then invoke the specified callback function.
 * Parameters:
 *            ref: the ref object id;
 *              f: the callback function;
 *           argc: the args count
 *            ...: variable args
 * Return value:
 *            0: success
 *           <0: failure with aosl_errno set
 * Remarks:
 *     unsafe is intentionally different from hold/read/write:
 *     1. it is allowed to run even after the ref has already been marked
 *        destroyed internally, as long as the internal object is still
 *        reachable;
 *     2. destroy_wait destroyers do not wait for unsafe/maystall sections in
 *        the same way they wait normal hold/read/write users;
 *     3. after the callback returns, unsafe reports -EPERM if the ref has
 *        already become destroyed.
 *     This mode is mainly for final cleanup paths and advanced lifecycle
 *     control, not for normal business access.
 **/
extern __aosl_api__ int aosl_ref_unsafe (aosl_ref_t ref, aosl_ref_func_t f, uintptr_t argc, ...);
extern __aosl_api__ int aosl_ref_unsafe_args (aosl_ref_t ref, aosl_ref_func_t f, uintptr_t argc, va_list args);
extern __aosl_api__ int aosl_ref_unsafe_argv (aosl_ref_t ref, aosl_ref_func_t f, uintptr_t argc, uintptr_t argv []);

/**
 * Potential read unlock the ref object and set it maystall, then invoke the specified callback function.
 * Parameters:
 *            ref: the ref object id;
 *              f: the callback function;
 *           argc: the args count
 *            ...: variable args
 * Return value:
 *            0: success
 *           <0: failure with aosl_errno set
 * Remarks:
 *     maystall currently shares the same exported implementation as unsafe.
 *     Use it when the callback may block for a long time and therefore should
 *     not force destroy_wait callers to wait for it like a normal read/write
 *     user.
 **/
extern __aosl_api__ int aosl_ref_maystall (aosl_ref_t ref, aosl_ref_func_t f, uintptr_t argc, ...);
extern __aosl_api__ int aosl_ref_maystall_args (aosl_ref_t ref, aosl_ref_func_t f, uintptr_t argc, va_list args);
extern __aosl_api__ int aosl_ref_maystall_argv (aosl_ref_t ref, aosl_ref_func_t f, uintptr_t argc, uintptr_t argv []);


typedef void *aosl_refobj_t;

#define AOSL_FREE_ONLY_OBJ ((aosl_refobj_t)(uintptr_t)1)
#define aosl_is_free_only(robj) ((int)((aosl_refobj_t)(robj) == AOSL_FREE_ONLY_OBJ))

/**
 * Retrieve the ref object arg.
 * Parameter:
 *     robj: the reference object;
 * Return value:
 *     the ref object arg;
 **/
extern __aosl_api__ void *aosl_refobj_arg (aosl_refobj_t robj);

/**
 * Get the ref id of the specified ref object.
 * Parameter:
 *      robj: the reference object;
 * Return value:
 *      the ref id.
 **/
extern __aosl_api__ aosl_ref_t aosl_refobj_id (aosl_refobj_t robj);

/**
 * Get the ref magic of the specified ref object.
 * Parameters:
 *      robj: the reference object;
 * Return value:
 *      the ref magic.
 **/
extern __aosl_api__ aosl_ref_magic_t aosl_refobj_magic (aosl_refobj_t robj);

/**
 * Make sure read lock the ref object specified by robj, then invoke the specified callback function.
 * Parameters:
 *           robj: the ref object itself;
 *              f: the callback function;
 *           argc: the args count
 *            ...: variable args
 * Return value:
 *            0: success
 *           <0: failure with aosl_errno set
 **/
extern __aosl_api__ int aosl_refobj_read (aosl_refobj_t robj, aosl_ref_func_t f, uintptr_t argc, ...);
extern __aosl_api__ int aosl_refobj_read_args (aosl_refobj_t robj, aosl_ref_func_t f, uintptr_t argc, va_list args);
extern __aosl_api__ int aosl_refobj_read_argv (aosl_refobj_t robj, aosl_ref_func_t f, uintptr_t argc, uintptr_t argv []);

/**
 * Make sure set the ref object specified by robj unsafe, then invoke the specified callback function.
 * Parameters:
 *           robj: the ref object itself;
 *              f: the callback function;
 *           argc: the args count
 *            ...: variable args
 * Return value:
 *            0: success
 *           <0: failure with aosl_errno set
 **/
extern __aosl_api__ int aosl_refobj_unsafe (aosl_refobj_t robj, aosl_ref_func_t f, uintptr_t argc, ...);
extern __aosl_api__ int aosl_refobj_unsafe_args (aosl_refobj_t robj, aosl_ref_func_t f, uintptr_t argc, va_list args);
extern __aosl_api__ int aosl_refobj_unsafe_argv (aosl_refobj_t robj, aosl_ref_func_t f, uintptr_t argc, uintptr_t argv []);

/**
 * Make sure set the ref object specified by robj maystall, then invoke the specified callback function.
 * Parameters:
 *           robj: the ref object itself;
 *              f: the callback function;
 *           argc: the args count
 *            ...: variable args
 * Return value:
 *            0: success
 *           <0: failure with aosl_errno set
 **/
extern __aosl_api__ int aosl_refobj_maystall (aosl_refobj_t robj, aosl_ref_func_t f, uintptr_t argc, ...);
extern __aosl_api__ int aosl_refobj_maystall_args (aosl_refobj_t robj, aosl_ref_func_t f, uintptr_t argc, va_list args);
extern __aosl_api__ int aosl_refobj_maystall_argv (aosl_refobj_t robj, aosl_ref_func_t f, uintptr_t argc, uintptr_t argv []);


/**
 * Detect whether the reference object specified by ref is read locked
 * by the calling thread.
 * Parameter:
 *         ref: the reference object id
 * Return value:
 *           0: not read locked
 *   none zero: read locked by calling thread
 **/
extern __aosl_api__ int aosl_ref_rdlocked (aosl_ref_t ref);

/**
 * Set the living scope ref object of the specified ref object.
 * Parameters:
 *          ref: the ref object ref id;
 *    scope_ref: the living scope ref, the ref object will be destroyed
 *               when the object specified by scope_ref was destroyed;
 * Return value:
 *        <0: error occured, and aosl_errno indicates which error;
 *       >=0: successful;
 **/
extern __aosl_api__ int aosl_ref_set_scope (aosl_ref_t ref, aosl_ref_t scope_ref);

/**
 * Destroy the reference object specified by ref.
 * Parameters:
 *         ref: the reference object id
 *   do_delete: 0 for just marking it destroyed
 *              non-0 value for deleting it
 * Return value:
 *        0: success
 *       <0: failed, and aosl_errno indicates what error occurs
 * Remarks:
 *     Destroy has two logically separate effects:
 *     1. mark the ref object as destroying/destroyed so that later normal
 *        hold/read/write attempts fail;
 *     2. optionally uninstall and free the ref object when do_delete != 0.
 *     For destroy_wait refs, destroy waits for in-flight normal hold/read/
 *     write users to leave before it reports completion. It does not turn an
 *     already-running direct hold/read/write callback into free_only; those
 *     callbacks run their normal path and only later callers observe the
 *     destroyed state.
 **/
extern __aosl_api__ int aosl_ref_destroy (aosl_ref_t ref, int do_delete);

/**
 * The proto for a ref destroy async exec callback function.
 * Parameters:
 *        err: 0 for destroy ref object successfully, <0 for error code;
 *       argc: the args count passed by exec series functions;
 *       argv: args vector passed by exec series functions;
 * Return value:
 *      none.
 **/
typedef void (*aosl_ref_destroy_exec_f) (int err, uintptr_t argc, uintptr_t argv []);

/**
 * Execute the specified function asynchronously in thread pool before destroying
 * the reference object specified by ref, this function supports coroutine.
 * Parameters:
 *         ref: the reference object id;
 *        ares: ares object if you want to wait the execution of function f,
 *              specify AOSL_REF_INVALID when you do not want to wait;
 *           f: the target function which will be executed in thread pool
 *              after destroyed the ref object;
 *        argc: the args count;
 *         ...: variable args;
 * Return value:
 *        0: success
 *       <0: failed, and aosl_errno indicates what error occurs
 * Remarks:
 *   If ares is AOSL_REF_INVALID and the invoking thread is an mpq thread,
 *   then this function will support coroutine resume mechanism.
 **/
extern __aosl_api__ int aosl_ref_destroy_exec (aosl_ref_t ref, aosl_ref_t ares, aosl_ref_destroy_exec_f f, uintptr_t argc, ...);

/**
 * Execute the specified function asynchronously in thread pool before destroying
 * the reference object specified by ref, this function supports coroutine.
 * Parameters:
 *         ref: the reference object id;
 *        ares: ares object if you want to wait the execution of function f,
 *              specify AOSL_REF_INVALID when you do not want to wait;
 *           f: the target function which will be executed in thread pool
 *              after destroyed the ref object;
 *        argc: the args count;
 *        args: variable args;
 * Return value:
 *        0: success
 *       <0: failed, and aosl_errno indicates what error occurs
 * Remarks:
 *   If ares is AOSL_REF_INVALID and the invoking thread is an mpq thread,
 *   then this function will support coroutine resume mechanism.
 **/
extern __aosl_api__ int aosl_ref_destroy_exec_args (aosl_ref_t ref, aosl_ref_t ares, aosl_ref_destroy_exec_f f, uintptr_t argc, va_list args);

/**
 * Execute the specified function asynchronously in thread pool before destroying
 * the reference object specified by ref, this function supports coroutine.
 * Parameters:
 *         ref: the reference object id;
 *        ares: ares object if you want to wait the execution of function f,
 *              specify AOSL_REF_INVALID when you do not want to wait;
 *           f: the target function which will be executed in thread pool
 *              after destroyed the ref object;
 *        argc: the args count;
 *        argv: variable args vector;
 * Return value:
 *        0: success
 *       <0: failed, and aosl_errno indicates what error occurs
 * Remarks:
 *   If ares is AOSL_REF_INVALID and the invoking thread is an mpq thread,
 *   then this function will support coroutine resume mechanism.
 **/
extern __aosl_api__ int aosl_ref_destroy_exec_argv (aosl_ref_t ref, aosl_ref_t ares, aosl_ref_destroy_exec_f f, uintptr_t argc, uintptr_t argv []);

/**
 * Get the top ref object id of the ref stack.
 * Parameter:
 *      none.
 * Return value:
 *      the top ref object id, AOSL_REF_INVALID if the ref stack is empty.
 **/
extern __aosl_api__ aosl_ref_t aosl_ref_stack_top (void);

/**
 * Get the top ref object of the ref stack.
 * Parameter:
 *      none.
 * Return value:
 *      the top ref object, NULL if the ref stack is empty.
 **/
extern __aosl_api__ aosl_refobj_t aosl_ref_stack_top_obj (void);


#ifdef __cplusplus
}
#endif



#endif /* __AOSL_REF_H__ */
