/*************************************************************
 * Author:	Lionfore Hao (haolianfu@agora.io)
 * Date	 :	Jul 18th, 2018
 * Module:	Multiplex queue header file
 *
 *
 * This is a part of the Advanced Operating System Layer.
 * Copyright (C) 2018 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#ifndef __AOSL_MPQ_H__
#define __AOSL_MPQ_H__

#include <api/aosl_types.h>
#include <api/aosl_defs.h>
#include <api/aosl_thread.h>
#include <api/aosl_ref.h>

#ifdef __cplusplus
extern "C" {
#endif



/**
 * AOSL performance callback function proto type.
 * Parameters:
 *     f_name: the functionality name, which was specified when
 *             queue/call something, it might be NULL if not
 *             specified a name
 *  free_only: 0 for normal exec, and other values for free only
 *    wait_us: the wait time in queue before executing(microseconds)
 *    exec_us: the executing time(microseconds)
 * Return Value:
 *    none.
 **/
typedef void (*aosl_perf_f_t) (const char *f_name, int free_only, uint32_t wait_us, uint32_t exec_us);

/**
 * Set/Unset the AOSL performance callback function.
 * Note: this callback function MUST BE set/unset before
 *       creating any mpq or after all mpq have exited.
 * Parameters:
 *     perf_f: the performance callback function, NULL for unset
 * Return Value:
 *          0: success
 *         <0: failure with aosl_errno set
 * Remarks:
 *    If invoking this function when any mpq is active,
 *    then it will return -1 with aosl_errno set to EPERM.
 **/
extern __aosl_api__ int aosl_perf_set_callback (aosl_perf_f_t perf_f);

typedef struct _internal_mpq_od_ *aosl_mpq_t;

#define AOSL_MPQ_INVALID ((aosl_mpq_t)(intptr_t)NULL)

#define aosl_mpq_invalid(mpq) ((int)(intptr_t)(mpq) <= 0)

/**
 * The initialize callback function of the mp queue create function.
 * Parameters:
 *        arg: the input parameter
 * Return Value:
 *         <0: error occurs
 *           0: ok, no error
 **/
typedef int (*aosl_mpq_init_t) (void *arg);

/**
 * The finalize callback function of the mp queue destroy function.
 * Parameters:
 *        arg: the input parameter
 **/
typedef void (*aosl_mpq_fini_t) (void *arg);


/* I think this is big enough */
#define MPQ_MAX_SIZE 1000000

/**
 * Create an multiplex queue.
 * Parameter:
 *      pri: the priority of the mpq, 0 for system default
 *           #define AOSL_THRD_PRI_DEFAULT 0
 *           #define AOSL_THRD_PRI_LOW 1
 *           #define AOSL_THRD_PRI_NORMAL 2
 *           #define AOSL_THRD_PRI_HIGH 3
 *           #define AOSL_THRD_PRI_HIGHEST 4
 *           #define AOSL_THRD_PRI_RT 5
 *      max: specify the max queue size, must > 0 and <= MPQ_MAX_SIZE
 *     name: the queue name
 *     init: the initialize callback function
 *     fini: the finalize callback function
 *     arg: the parameter passed to init callback
 * Return value:
 *     the queue object just created, <0 when failed.
 **/
extern __aosl_api__ aosl_mpq_t aosl_mpq_create (int pri, int max, const char *name, aosl_mpq_init_t init, aosl_mpq_fini_t fini, void *arg);

#define AOSL_MPQ_NONBLOCK 1
#define AOSL_MPQ_MAYSTALL 2
#define AOSL_MPQ_IOMP 4
#define AOSL_MPQ_USER 8

/**
 * Create an multiplex queue with specified flags.
 * Parameter:
 *    flags: specify the queue flags
 *      pri: the priority of the mpq, 0 for system default
 *           #define AOSL_THRD_PRI_DEFAULT 0
 *           #define AOSL_THRD_PRI_LOW 1
 *           #define AOSL_THRD_PRI_NORMAL 2
 *           #define AOSL_THRD_PRI_HIGH 3
 *           #define AOSL_THRD_PRI_HIGHEST 4
 *           #define AOSL_THRD_PRI_RT 5
 *      max: specify the max queue size, must > 0 and <= MPQ_MAX_SIZE
 *     name: the queue name
 *     init: the initialize callback function
 *     fini: the finalize callback function
 *      arg: the parameter passed to init callback
 * Return value:
 *     the queue object just created, <0 when failed.
 **/
extern __aosl_api__ aosl_mpq_t aosl_mpq_create_flags (int flags, int pri, int max, const char *name, aosl_mpq_init_t init, aosl_mpq_fini_t fini, void *arg);

#define BITOP_OR 0
#define BITOP_AND 1
#define BITOP_XOR 2

extern __aosl_api__ int aosl_mpq_change_flags (aosl_mpq_t qid, int bit_op, int bits);

extern __aosl_api__ int aosl_mpq_get_flags (aosl_mpq_t qid);

/* Get this multiplex queue id, if not exists, then just return -1 */
extern __aosl_api__ aosl_mpq_t aosl_mpq_this (void);

/**
 * Create an mpq with default queue size on the current calling thread if not exists yet.
 * Generally, this function is only used in the non-mpq thread, such as the main
 * thread.
 **/
extern __aosl_api__ aosl_mpq_t aosl_mpq_create_current (void);

/**
 * Get the N-th argument of the current running queued function (argv version).
 * Parameters:
 *     n: which argument you want to get, the first arg is 0;
 *   arg: the argument variable address to save the argument value;
 * Return value:
 *    <0: error occured, and aosl_errno indicates which error;
 *     0: call successful, and '*arg' is value of the N-th argument;
 **/
extern __aosl_api__ int aosl_mpq_run_func_arg (uintptr_t n, uintptr_t *arg);

/**
 * Get the data and length of the current running queued function (data version).
 * Parameters:
 *    len_p: return the queued *_data function data length if not NULL;
 *   data_p: return the queued *_data function data pointer if not NULL;
 * Return value:
 *    <0: error occured, and aosl_errno indicates which error;
 *     0: call successful;
 **/
extern __aosl_api__ int aosl_mpq_run_func_data (size_t *len_p, void **data_p);

/**
 * Get the done mpq id of the current running queued function.
 * Parameters:
 *     None.
 * Return value:
 *    <0: error occured or the current running function has no done mpq id;
 *   >=0: running function's done mpq id;
 **/
extern __aosl_api__ aosl_mpq_t aosl_mpq_run_func_done_qid (void);

/**
 * Checking whether the refobj specified by robj is just running on the
 * running mpq.
 * Parameters:
 *       robj: the ref object
 * Return value:
 *          0: Not running on this mpq now
 *     None-0: Just running on this mpq now
 **/
extern __aosl_api__ int aosl_mpq_running_refobj (aosl_refobj_t robj);

/**
 * Get the arg of the running mpq object which was specified when creating.
 * Parameters:
 *      None.
 * Return value:
 *    The arg.
 **/
extern __aosl_api__ void *aosl_mpq_get_q_arg (void);

/**
 * Set the arg of the running mpq object.
 * Parameters:
 *      arg: the arg
 * Return value:
 *    <0: error occured, and aosl_errno indicates which error;
 *     0: call successful;
 **/
extern __aosl_api__ int aosl_mpq_set_q_arg (void *arg);


/**
 * The queue function prototype, something like the standard 'main' function.
 * Parameter:
 *    queued_ts_p: pointer of milliseconds timestamp of the func being queued
 *                 We employ a 'pointer' instead of the timestamp value here 
 *                       just for only passing one value on the 32bit system
 *           robj: when this parameter is AOSL_FREE_ONLY_OBJ, you can only
 *                 free the relative resources, MUST NOT DO ANYTHING ELSE;
 *           argc: specify the argv array elements count
 *           argv: array for passing variable args
 * Return value:
 *       none.
 **/
typedef void (*aosl_mpq_func_argv_t) (const aosl_ts_t *queued_ts_p, aosl_refobj_t robj, uintptr_t argc, uintptr_t argv []);

/**
 * Another queue function prototype for passing a chunk of data.
 * Parameter:
 *    queued_ts_p: pointer of milliseconds timestamp of the func being queued
 *                 We employ a 'pointer' instead of the timestamp value here 
 *                       just for only passing one value on the 32bit system.
 *           robj: when this parameter is AOSL_FREE_ONLY_OBJ, you can only
 *                 free the relative resources, MUST NOT DO ANYTHING ELSE;
 *            len: the passed data length
 *           data: the data itself
 * Return value:
 *     none.
 **/
typedef void (*aosl_mpq_func_data_t) (const aosl_ts_t *queued_ts_p, aosl_refobj_t robj, size_t len, void *data);

/**
 * Queue a function with args for invoking by the target thread which monitoring
 * the queue.
 * Parameter:
 *        tq: the target queue id
 *        dq: the done queue id, -1 for no done notification
 *            If dq is a valid mpq id, then the queued function f will be
 *            queued to the mpq specified by dq, and then got executed again.
 *            It is the responsibility of the function itself to differentiate the
 *            reentrance, such as, identify the reentrance according to one of arg.
 *       ref: the mpq object id for indicating whether the relative operation
 *            should be aborted and only free the relative resources, if the
 *            mpq object specified by ref has been destroyed, then target
 *            function specified by 'f' MUST NOT DO ANYTING ELSE except free
 *            the relative resources.
 *         f: the function
 *      argc: the args count
 *       ...: variable args
 * Return value:
 *        <0: indicates error, check aosl_errno for detail
 *         0: successful.
 **/
extern __aosl_api__ int aosl_mpq_queue (aosl_mpq_t tq, aosl_mpq_t dq, aosl_ref_t ref, const char *f_name, aosl_mpq_func_argv_t f, uintptr_t argc, ...);

/* The synchronous version, the target f must have been invoked when this function returns */
extern __aosl_api__ int aosl_mpq_call (aosl_mpq_t q, aosl_ref_t ref, const char *f_name, aosl_mpq_func_argv_t f, uintptr_t argc, ...);

/**
 * Run the specified function on the specified mpq in async or sync mode:
 * 1. identical to aosl_mpq_queue if the target q is not the same as the running q;
 * 2. identical to aosl_mpq_call if the target q is just the running q;
 **/
extern __aosl_api__ int aosl_mpq_run (aosl_mpq_t tq, aosl_mpq_t dq, aosl_ref_t ref, const char *f_name, aosl_mpq_func_argv_t f, uintptr_t argc, ...);


/**
 * The same as 'aosl_mpq_queue' except this function takes a 'va_list' arg for the
 * variable args rather than '...'.
 **/
extern __aosl_api__ int aosl_mpq_queue_args (aosl_mpq_t tq, aosl_mpq_t dq, aosl_ref_t ref, const char *f_name, aosl_mpq_func_argv_t f, uintptr_t argc, va_list args);

/* The synchronous version, the target f must have been invoked when this function returns */
extern __aosl_api__ int aosl_mpq_call_args (aosl_mpq_t q, aosl_ref_t ref, const char *f_name, aosl_mpq_func_argv_t f, uintptr_t argc, va_list args);

/**
 * Run the specified function on the specified mpq in async or sync mode:
 * 1. identical to aosl_mpq_queue_args if the target q is not the same as the running q;
 * 2. identical to aosl_mpq_call_args if the target q is just the running q;
 **/
extern __aosl_api__ int aosl_mpq_run_args (aosl_mpq_t tq, aosl_mpq_t dq, aosl_ref_t ref, const char *f_name, aosl_mpq_func_argv_t f, uintptr_t argc, va_list args);

/**
 * The same as 'aosl_mpq_queue_args' except this function takes a 'uintptr_t *' arg for the argv
 * rather than 'va_list args'.
 **/
extern __aosl_api__ int aosl_mpq_queue_argv (aosl_mpq_t tq, aosl_mpq_t dq, aosl_ref_t ref, const char *f_name, aosl_mpq_func_argv_t f, uintptr_t argc, uintptr_t *argv);

/* The synchronous version, the target f must have been invoked when this function returns */
extern __aosl_api__ int aosl_mpq_call_argv (aosl_mpq_t q, aosl_ref_t ref, const char *f_name, aosl_mpq_func_argv_t f, uintptr_t argc, uintptr_t *argv);

/**
 * Run the specified function on the specified mpq in async or sync mode:
 * 1. identical to aosl_mpq_queue_argv if the target q is not the same as the running q;
 * 2. identical to aosl_mpq_call_argv if the target q is just the running q;
 **/
extern __aosl_api__ int aosl_mpq_run_argv (aosl_mpq_t tq, aosl_mpq_t dq, aosl_ref_t ref, const char *f_name, aosl_mpq_func_argv_t f, uintptr_t argc, uintptr_t *argv);


/**
 * Queue a function with a chunk of data for invoking by the target thread which monitoring
 * the queue.
 * Parameter:
 *        tq: the target queue id
 *        dq: the done queue id, -1 for no done notification
 *            If dq is a valid mpq id, then the queued function f will be
 *            queued to the mpq specified by dq, and then got executed again.
 *            It is the responsibility of the function itself to differentiate the
 *            reentrance, such as, identify the reentrance according to one of arg.
 *       ref: the mpq object id for indicating whether the relative operation
 *            should be aborted and only free the relative resources, if the
 *            mpq object specified by ref has been destroyed, then target
 *            function specified by 'f' MUST NOT DO ANYTING ELSE except free
 *            the relative resources.
 *         f: the function
 *       len: the data length in bytes
 *      data: the data pointer which contains the data(would be copied in)
 * Return value:
 *        <0: indicates error, check aosl_errno for detail
 *         0: successful.
 **/
extern __aosl_api__ int aosl_mpq_queue_data (aosl_mpq_t tq, aosl_mpq_t dq, aosl_ref_t ref, const char *f_name, aosl_mpq_func_data_t f, size_t len, void *data);

/* The synchronous version, the target f must have been invoked when this function returns */
extern __aosl_api__ int aosl_mpq_call_data (aosl_mpq_t q, aosl_ref_t ref, const char *f_name, aosl_mpq_func_data_t f, size_t len, void *data);

/**
 * Run the specified function on the specified mpq in async or sync mode:
 * 1. identical to aosl_mpq_queue_data if the target q is not the same as the running q;
 * 2. identical to aosl_mpq_call_data if the target q is just the running q;
 **/
extern __aosl_api__ int aosl_mpq_run_data (aosl_mpq_t tq, aosl_mpq_t dq, aosl_ref_t ref, const char *f_name, aosl_mpq_func_data_t f, size_t len, void *data);

/**
 * Start aosl main mpq, only a single main mpq allowed.
 **/
extern __aosl_api__ int aosl_main_start (int pri, aosl_mpq_init_t init, aosl_mpq_fini_t fini, void *arg);

/**
 * Increase and return the main mpq usage count, start the main mpq if not start yet.
 * Parameter:
 *          None.
 * Return value:
 *        <0: indicates error, check aosl_errno for detail;
 *        >0: the main mpq current usage count after get;
 **/
extern __aosl_api__ int aosl_main_get (void);

/**
 * Decrease and return the main mpq usage count, destroy the main mpq if the usage is 0.
 * Parameter:
 *          None.
 * Return value:
 *        <0: indicates error, check aosl_errno for detail;
 *       >=0: the main mpq current usage count after put;
 **/
extern __aosl_api__ int aosl_main_put (void);


/**
 * Get the aosl main mpq id, and return -1 for not exist.
 **/
extern __aosl_api__ aosl_mpq_t aosl_mpq_main (void);

/**
 * Check whether the aosl main mpq started.
 **/
#define aosl_main_started() (!aosl_mpq_invalid (aosl_mpq_main ()))


/**
 * Get the queued function invocations count.
 * Parameters:
 *      q: the queue object id
 * Return value:
 *     <0: indicates error, check aosl_errno for detail
 *    >=0: the queued function invocations count
 */
extern __aosl_api__ int aosl_mpq_queued_count (aosl_mpq_t q);

/**
 * Get the last load/idle costs in micro seconds of this mpq
 * Parameters:
 *   load_p: the variable for saving the last load costs(us), NULL for not care
 *   idle_p: the variable for saving the last idle costs(us), NULL for not care
 * Return value:
 *     <0: indicates error, check aosl_errno for detail
 *      0: the last load/idle costs was/were saved to *load_p, *idle_p
 */
extern __aosl_api__ int aosl_mpq_last_costs (aosl_ts_t *load_p, aosl_ts_t *idle_p);

/**
 * Get the current running counters of this mpq
 * Parameters:
 *   funcs_count_p: the variable for saving the current running funcs count, NULL for not care
 *  timers_count_p: the variable for saving the current running timers count, NULL for not care
 *     fds_count_p: the variable for saving the current running fds count, NULL for not care
 * Return value:
 *     <0: indicates error, check aosl_errno for detail
 *      0: the running counters was/were saved to *funcs_count_p, *timers_count_p, *fds_count_p
 */
extern __aosl_api__ int aosl_mpq_exec_counters (uint64_t *funcs_count_p, uint64_t *timers_count_p, uint64_t *fds_count_p);


typedef struct {
    uint32_t fw_avg; /* function average wait time */
    uint32_t fw_worst; /* function worst wait time */
    uint32_t fe_avg; /* function average exec time */
    uint32_t fe_worst; /* function worst exec time */
    uint32_t f_count; /* executed functions count */
} aosl_ftimes_t;

/**
 * Get the average & worst function wait & execution times of this mpq
 * Parameters:
 *          qid: the mpq id
 *       ftimes: the variable address for saving the function times
 *        reset: whether reset the counters, 0 for not clear, other values clear
 * Return value:
 *       <0: indicates error, check aosl_errno for detail
 *        0: the average function execution time was saved to *fe_*_p
 */
extern __aosl_api__ int aosl_mpq_ftimes (aosl_mpq_t qid, aosl_ftimes_t *ftimes, int reset);

/**
 * Get the average & worst function wait time of this mpq
 * Parameters:
 *          qid: the mpq id
 *     fw_avg_p: the variable address for saving the average function wait time
 *   fw_worst_p: the variable address for saving the worst function wait time
 *        reset: whether reset the counters, 0 for not clear, other values clear
 * Return value:
 *       <0: indicates error, check aosl_errno for detail
 *        0: the average function wait time was saved to *fw_*_p
 */
extern __aosl_api__ int aosl_mpq_fwtime (aosl_mpq_t qid, uint32_t *fw_avg_p, uint32_t *fw_worst_p, int reset);

/**
 * Get the name of the current running mpq
 * Parameters:
 *         None.
 * Return value:
 *     non-NULL: the running mpq name;
 *         NULL: the running thread is not an mpq thread;
 */
extern __aosl_api__ const char *aosl_mpq_this_name (void);

/**
 * Invoking this function will enter the infinite run loop of current thread's multiplex queue.
 * Generally, this function is only used in the non-mpq thread, such as the main thread.
 **/
extern __aosl_api__ void aosl_mpq_loop (void);

/**
 * Invoking this function when you need to ack the aosl ITC logic.
 * There's no need to use this function for most cases due to the default aosl mpq behaviors,
 * and only need to invoke this function explicitly when the aosl mpq thread does not run the
 * default mpq loop.
 * Return value:
 *             <0: indicates error, check aosl_errno for detail:
 *                   ESRCH: this mpq has been destroyed;
 *                   EPERM: the running thread is not an aosl mpq thread;
 *   other values: processed ITC calls count
 **/
extern __aosl_api__ int aosl_mpq_itc_ack (void);

/**
 * Check whether the running mpq is the aosl main mpq.
 * Return value:
 *       non-zero: the running mpq is the main mpq;
 *              0: the running mpq is not the main mpq;
 **/
extern __aosl_api__ int aosl_mpq_is_main (void);

/**
 * Destroy an multiplex queue object.
 * Parameter:
 *        mpq_id: the queue id to be destroyed
 **/
extern __aosl_api__ int aosl_mpq_destroy (aosl_mpq_t q);


/**
 * Destroy an multiplex queue object and wait until done.
 * Parameter:
 *        mpq_id: the queue id to be destroyed
 **/
extern __aosl_api__ int aosl_mpq_destroy_wait (aosl_mpq_t mpq_id);

/**
 * Wait an multiplex queue object to be destroyed. Note
 * that this function will not do the destroy action, and
 * just wait for the queue to be destroyed.
 * Parameter:
 *        mpq_id: the queue id to wait
 **/
extern __aosl_api__ int aosl_mpq_wait (aosl_mpq_t mpq_id);

/**
 * Check whether the thread of specified mpq still exists.
 * Parameter:
 *        qid: the mpq id
 * Return value:
 *  none zero: the thread still exists
 *          0: the thread exited
 **/
extern __aosl_api__ int aosl_mpq_thrd_exist (aosl_mpq_t qid);

/**
 * Let the aosl main multiplex queue thread exit, but not wait.
 **/
extern __aosl_api__ int aosl_main_exit (void);

/**
 * Let the aosl main multiplex queue thread exit, and wait done.
 **/
extern __aosl_api__ int aosl_main_exit_wait (void);

/**
 * Wait the aosl main multiplex queue thread to exit.
 * Note that this function will not let the aosl main thread exit,
 * just wait for the queue thread to exit.
 * Parameter:
 *        mpq_id: the queue id to wait
 **/
extern __aosl_api__ int aosl_main_wait (void);


/**
 * Shrink the resources allocated dynamically by AOSL.
 * Only need to do this when you do not use AOSL again but
 * the program does not exit now.
 **/
extern __aosl_api__ void aosl_shrink_resources ();


#ifdef __cplusplus
}
#endif

#endif /* __AOSL_MPQ_H__ */