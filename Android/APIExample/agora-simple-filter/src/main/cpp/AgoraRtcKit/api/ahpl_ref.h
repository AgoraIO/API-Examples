/*************************************************************
 * Author:	Lionfore Hao (haolianfu@agora.io)
 * Date	 :	Nov 19th, 2018
 * Module:	AHPL reference object definition file
 *
 *
 * This is a part of the Advanced High Performance Library.
 * Copyright (C) 2018 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#ifndef __AHPL_REF_H__
#define __AHPL_REF_H__


#include <api/ahpl_types.h>
#include <api/ahpl_defs.h>


#ifdef __cplusplus
extern "C" {
#endif



typedef int ahpl_ref_t;

#define AHPL_REF_INVALID ((ahpl_ref_t)-1)

#define ahpl_ref_invalid(ref) (((int)(ref)) < 0)


/**
 * The reference object destructor function prototype, which invoked when application
 * calling ahpl_ref_destroy functions to release resources.
 * Parameter:
 *       arg: the parameter passed in when creating the reference object;
 * Return value:
 *       none.
 **/
typedef void (*ahpl_ref_dtor_t) (void *arg);

/**
 * The reference object creating function prototype, which is used to create a ref object.
 * Parameters:
 *              arg: the parameter attached with the reference object;
 *             dtor: the ref object destructor function, which will be invoked when
 *                   the ref object is deleted;
 *      caller_free:
 *            none-0 guarantee the ref object relatives must be freed in the caller thread
 *                 0 the ref object relatives could be freed in any thread
 * Return value:
 *         the ref object id, please use ahpl_ref_invalid macro to check whether failed.
 **/
extern __ahpl_api__ ahpl_ref_t ahpl_ref_create (void *arg, ahpl_ref_dtor_t dtor, int caller_free);


/**
 * The ref object callback function prototype.
 * Parameter:
 *            arg: the ref object argument which was passed in when creating;
 *           argc: specify the argv array elements count, the same as the argc
 *                    when invoking ahpl_ref_[get|read|write] functions;
 *           argv: array for passing variable args, the same as the args
 *                    when invoking ahpl_task_exec_* functions;
 * Return value:
 *       none.
 **/
typedef void (*ahpl_ref_func_t) (void *arg, uintptr_t argc, uintptr_t argv []);

/**
 * Hold the ref object, and invoke the specified callback function.
 * Parameter:
 *            ref: the ref object id;
 *              f: the callback function;
 *           argc: the args count
 *            ...: variable args
 * Return value:
 *            0: success
 *           <0: failure with ahpl_errno set
 **/
extern __ahpl_api__ int ahpl_ref_hold (ahpl_ref_t ref, ahpl_ref_func_t f, uintptr_t argc, ...);
extern __ahpl_api__ int ahpl_ref_hold_args (ahpl_ref_t ref, ahpl_ref_func_t f, uintptr_t argc, va_list args);
extern __ahpl_api__ int ahpl_ref_hold_argv (ahpl_ref_t ref, ahpl_ref_func_t f, uintptr_t argc, uintptr_t argv []);

/**
 * Hold the ref object and read lock it, then invoke the specified callback function.
 * Parameter:
 *            ref: the ref object id;
 *              f: the callback function;
 *           argc: the args count
 *            ...: variable args
 * Return value:
 *            0: success
 *           <0: failure with ahpl_errno set
 **/
extern __ahpl_api__ int ahpl_ref_read (ahpl_ref_t ref, ahpl_ref_func_t f, uintptr_t argc, ...);
extern __ahpl_api__ int ahpl_ref_read_args (ahpl_ref_t ref, ahpl_ref_func_t f, uintptr_t argc, va_list args);
extern __ahpl_api__ int ahpl_ref_read_argv (ahpl_ref_t ref, ahpl_ref_func_t f, uintptr_t argc, uintptr_t argv []);

/**
 * Hold the ref object and write lock it, then invoke the specified callback function.
 * Parameter:
 *            ref: the ref object id;
 *              f: the callback function;
 *           argc: the args count
 *            ...: variable args
 * Return value:
 *            0: success
 *           <0: failure with ahpl_errno set
 **/
extern __ahpl_api__ int ahpl_ref_write (ahpl_ref_t ref, ahpl_ref_func_t f, uintptr_t argc, ...);
extern __ahpl_api__ int ahpl_ref_write_args (ahpl_ref_t ref, ahpl_ref_func_t f, uintptr_t argc, va_list args);
extern __ahpl_api__ int ahpl_ref_write_argv (ahpl_ref_t ref, ahpl_ref_func_t f, uintptr_t argc, uintptr_t argv []);

/**
 * Hold the ref object and set it maystall, then invoke the specified callback function.
 * Parameter:
 *            ref: the ref object id;
 *              f: the callback function;
 *           argc: the args count
 *            ...: variable args
 * Return value:
 *            0: success
 *           <0: failure with ahpl_errno set
 **/
extern __ahpl_api__ int ahpl_ref_maystall (ahpl_ref_t ref, ahpl_ref_func_t f, uintptr_t argc, ...);
extern __ahpl_api__ int ahpl_ref_maystall_args (ahpl_ref_t ref, ahpl_ref_func_t f, uintptr_t argc, va_list args);
extern __ahpl_api__ int ahpl_ref_maystall_argv (ahpl_ref_t ref, ahpl_ref_func_t f, uintptr_t argc, uintptr_t argv []);


typedef void *ahpl_refobj_t;

#define AHPL_FREE_ONLY_OBJ ((ahpl_refobj_t)(uintptr_t)1)
#define ahpl_is_free_only(robj) ((int)((ahpl_refobj_t)(robj) == AHPL_FREE_ONLY_OBJ))

/**
 * Retrieve the ref object arg.
 * Parameters:
 *     robj: the reference object;
 * Return value:
 *     the ref object arg;
 **/
extern __ahpl_api__ void *ahpl_refobj_arg (ahpl_refobj_t robj);

/**
 * Get the ref id of the specified ref object.
 * Parameters:
 *      robj: the reference object;
 * Return value:
 *      the ref id.
 **/
extern __ahpl_api__ ahpl_ref_t ahpl_refobj_id (ahpl_refobj_t robj);

/**
 * Make sure read lock the ref object specified by robj, then invoke the specified callback function.
 * Parameter:
 *           robj: the ref object itself;
 *              f: the callback function;
 *           argc: the args count
 *            ...: variable args
 * Return value:
 *            0: success
 *           <0: failure with ahpl_errno set
 **/
extern __ahpl_api__ int ahpl_refobj_read (ahpl_refobj_t robj, ahpl_ref_func_t f, uintptr_t argc, ...);
extern __ahpl_api__ int ahpl_refobj_read_args (ahpl_refobj_t robj, ahpl_ref_func_t f, uintptr_t argc, va_list args);
extern __ahpl_api__ int ahpl_refobj_read_argv (ahpl_refobj_t robj, ahpl_ref_func_t f, uintptr_t argc, uintptr_t argv []);

/**
 * Make sure set the ref object specified by robj maystall, then invoke the specified callback function.
 * Parameter:
 *           robj: the ref object itself;
 *              f: the callback function;
 *           argc: the args count
 *            ...: variable args
 * Return value:
 *            0: success
 *           <0: failure with ahpl_errno set
 **/
extern __ahpl_api__ int ahpl_refobj_maystall (ahpl_refobj_t robj, ahpl_ref_func_t f, uintptr_t argc, ...);
extern __ahpl_api__ int ahpl_refobj_maystall_args (ahpl_refobj_t robj, ahpl_ref_func_t f, uintptr_t argc, va_list args);
extern __ahpl_api__ int ahpl_refobj_maystall_argv (ahpl_refobj_t robj, ahpl_ref_func_t f, uintptr_t argc, uintptr_t argv []);


/**
 * Detect whether the reference object specified by ref is read locked
 * by the calling thread.
 * Parameter:
 *         ref: the reference object id
 * Return value:
 *           0: not read locked
 *   none zero: read locked by calling thread
 **/
extern __ahpl_api__ int ahpl_ref_locked (ahpl_ref_t ref);

/**
 * Set the living scope ref object of the specified ref object.
 * Parameters:
 *          ref: the ref object ref id;
 *    scope_ref: the living scope ref, the ref object will be destroyed
 *               when the object specified by scope_ref was destroyed;
 * Return value:
 *        <0: error occured, and ahpl_errno indicates which error;
 *       >=0: successful;
 **/
extern __ahpl_api__ int ahpl_ref_set_scope (ahpl_ref_t ref, ahpl_ref_t scope_ref);

/**
 * Destroy the reference object specified by ref.
 * Parameter:
 *         ref: the reference object id
 *   do_delete: 0 for just marking it destroyed
 *              non-0 value for deleting it
 * Return value:
 *        0: success
 *       <0: failed, and ahpl_errno indicates what error occurs
 **/
extern __ahpl_api__ int ahpl_ref_destroy (ahpl_ref_t ref, int do_delete);



#ifdef __cplusplus
}
#endif



#endif /* __AHPL_REF_H__ */