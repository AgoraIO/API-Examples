/*************************************************************
 * Author:	Lionfore Hao (haolianfu@agora.io)
 * Date	 :	Nov 19th, 2018
 * Module:	AOSL task object definition file
 *
 *
 * This is a part of the Advanced Operating System Layer.
 * Copyright (C) 2018 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#ifndef __AOSL_TASK_H__
#define __AOSL_TASK_H__


#include <api/aosl_types.h>
#include <api/aosl_defs.h>
#include <api/aosl_mpq.h>
#include <api/aosl_ref.h>
#include <api/aosl_async.h>


#ifdef __cplusplus
extern "C" {
#endif



typedef enum {
	aosl_task_type_cpu, /* CPU sensitive, no any OS waiting operations */
	aosl_task_type_gpu, /* GPU sensitive, might be shrinked when no more GPU operations */
	aosl_task_type_gen, /* General type, can use OS waiting operations */
	aosl_task_type_ltw, /* Time sensitive, can use long time waiting operations */
	aosl_task_type_opa, /* Operation itself is asynchronous */
} aosl_task_type_t;


typedef enum {
	aosl_task_act_exec,
	aosl_task_act_done,
	aosl_task_act_free,
} aosl_task_act_t;

/**
 * The task object function prototype.
 * Parameter:
 *           tobj: the task object itself;
 *            act: aosl_task_act_exec for async processing;
 *                 aosl_task_act_done for async process done;
 *                 aosl_task_act_free for freeing relative resources only;
 *         opaque: the async operation opaque data, which is useful for identifying
 *                 the async operation especially for the OPA cases, which should
 *                 be passed when invoking
 *           argc: specify the argv array elements count, the same as the argc
 *                    when invoking aosl_task_exec_* functions;
 *           argv: array for passing variable args, the same as the args
 *                    when invoking aosl_task_exec_* functions;
 * Return value:
 *       none.
 **/
typedef void (*aosl_task_func_t) (aosl_refobj_t tobj, aosl_task_act_t act, uintptr_t opaque, uintptr_t argc, uintptr_t argv []);

/**
 * The task object result waiting function prototype.
 * Parameter:
 *           tobj: the task object itself;
 *         opaque: the async operation opaque data, which is useful for identifying
 *                 the async operation especially for the OPA cases, which should
 *                 be passed when invoking
 *           argc: specify the argv array elements count, the same as the argc
 *                    when invoking aosl_task_exec_* functions;
 *           argv: array for passing variable args, the same as the args
 *                    when invoking aosl_task_exec_* functions;
 * Return value:
 *      none.
 * NOTE: the task object system assumes this callback function must
 *       be a sync res waiting function, and task object system will
 *       start a dedicate thread for waiting the sync result.
 *       Furthermore, this callback function can only be specified
 *       when creating an OPA type task object, please pass NULL for
 *       non OPA task object.
 **/
typedef void (*aosl_task_res_wait_t) (aosl_refobj_t tobj, uintptr_t opaque, uintptr_t argc, uintptr_t argv []);

/**
 * The task object creating function prototype, which is used to create an task object.
 * Parameters:
 *            arg: the parameter attached with the task object;
 *           dtor: the task object destructor function, which will be invoked when
 *                 the task object is deleted;
 *           type: the task object type;
 *         serial: the operations must be serial, and there are 2 different meanings:
 *                 1) for non OPA task object, serial means any operation must be
 *                    issued after the previous operation was done;
 *                 2) for OPA task object, serial means the async operation will be
 *                    done orderly according to the issuing order;
 *        st_op_q: single thread operations MPQ id, AOSL_MPQ_INVALID for non single
 *                 thread operations mode;
 *         resume: None zero for needing resume, 0 for no need for resuming;
 *         wait_f: for OPA type of task object, this is the sync callback function
 *                 for waiting the result;
 *                 for non-OPA type of task object, this parameter must be NULL;
 **/
extern __aosl_api__ aosl_ref_t aosl_task_create (void *arg, aosl_ref_dtor_t dtor,
							aosl_task_type_t type, int serial, aosl_mpq_t st_op_q,
										int resume, aosl_task_res_wait_t wait_f);

/**
 * Get the task type.
 * Parameters:
 *          task: the task object ref;
 * Return Value:
 *            >=0: successful, return the task type;
 *             <0: failed with aosl_errno set;
 **/
extern __aosl_api__ aosl_task_type_t aosl_task_get_type (aosl_ref_t task);

/**
 * Execute the task action, and the callback function will be invoked.
 * Parameters:
 *           task: the task object ref;
 *         f_name: the functionality name for the callback function;
 *              f: the task action callback function;
 *           argc: arguments count would be passed to the callback function;
 *            ...: the various arguments would be passed to the callback function;
 * Return Value:
 *              0: successful, and the async callback will be invoked;
 *             <0: failed with aosl_errno set;
 **/
extern __aosl_api__ int aosl_task_exec (aosl_ref_t task, const char *f_name, aosl_task_func_t f, uintptr_t argc, ...);

/* va_list version */
extern __aosl_api__ int aosl_task_exec_args (aosl_ref_t task, const char *f_name, aosl_task_func_t f, uintptr_t argc, va_list args);

/* args vector versions */
extern __aosl_api__ int aosl_task_exec_argv (aosl_ref_t task, const char *f_name, aosl_task_func_t f, uintptr_t argc, uintptr_t argv []);

/**
 * These '*exclusive_exec*' functions just guarantee the callback
 * function will be executed exclusively, but not guarantee it is
 * executed after the function returns.
 * The callback function will be invoked on the calling thread.
 **/
extern __aosl_api__ int aosl_task_exclusive_exec (aosl_ref_t task, aosl_task_func_t exclusive_f, uintptr_t argc, ...);
extern __aosl_api__ int aosl_task_exclusive_exec_args (aosl_ref_t task, aosl_task_func_t exclusive_f, uintptr_t argc, va_list args);
extern __aosl_api__ int aosl_task_exclusive_exec_argv (aosl_ref_t task, aosl_task_func_t exclusive_f, uintptr_t argc, uintptr_t argv []);

extern __aosl_api__ int aosl_task_waiting_ops_count (aosl_ref_t task);
extern __aosl_api__ int aosl_task_remove_waiting_ops_head (aosl_ref_t task);

/**
 * This is only for aosl_task_type_opa case, the operation itself is
 * asynchronous, so please invoke this function when the asynchronous
 * operation is done.
 **/
extern __aosl_api__ int aosl_task_async_done (aosl_ref_t task);
extern __aosl_api__ int aosl_task_async_done_opaque (aosl_ref_t task, uintptr_t opaque);



#ifdef __cplusplus
}
#endif



#endif /* __AOSL_TASK_H__ */