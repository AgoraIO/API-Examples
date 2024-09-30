/*************************************************************
 * Author:	Lionfore Hao (haolianfu@agora.io)
 * Date	 :	Jul 19th, 2018
 * Module:	Multiplex queue timer header file
 *
 *
 * This is a part of the Advanced Operating System Layer.
 * Copyright (C) 2018 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#ifndef __AOSL_MPQ_TIMER_H__
#define __AOSL_MPQ_TIMER_H__

#include <api/aosl_types.h>
#include <api/aosl_defs.h>
#include <api/aosl_time.h>
#include <api/aosl_mpq.h>


#ifdef __cplusplus
extern "C" {
#endif


/* The timer object type */
typedef struct _internal_timer_od_ *aosl_timer_t;

#define AOSL_MPQ_TIMER_INVALID ((aosl_timer_t)(intptr_t)NULL)

#define aosl_mpq_timer_invalid(timer_id) ((int)(intptr_t)(timer_id) <= 0)

/**
 * The timer callback function prototype.
 * Parameters:
 *        timer_id: the timer id;
 *           now_p: *now_p is the triggering time tick in ms;
 *            argc: arguments count would be passed to async callback function;
 *            argv: the various arguments would be passed to async callback function;
 * Return Value:
 *           None.
 **/
typedef void (*aosl_timer_func_t) (aosl_timer_t timer_id, const aosl_ts_t *now_p, uintptr_t argc, uintptr_t argv []);


#define AOSL_INVALID_TIMER_INTERVAL ((uintptr_t)(-1))

/**
 * Create but not start a periodic timer.
 * Parameters:
 *             qid: the mpq id in which the created timer will run;
 *        interval: the timer interval in ms;
 *            func: the timer callback function;
 *            dtor: the timer destructor function;
 *            argc: arguments count would be passed to async callback function;
 *             ...: the various arguments would be passed to async callback function;
 * Return Value:
 *           aosl_mpq_timer_invalid($ret): failed with aosl_errno set;
 *           other value: the created timer id;
 **/
extern __aosl_api__ aosl_timer_t aosl_mpq_create_timer (aosl_mpq_t qid, uintptr_t interval, aosl_timer_func_t func, aosl_obj_dtor_t dtor, uintptr_t argc, ...);

/**
 * Create but not start a oneshot timer.
 * Parameters:
 *             qid: the mpq id in which the created timer will run;
 *            func: the timer callback function;
 *            dtor: the timer destructor function;
 *            argc: arguments count would be passed to async callback function;
 *             ...: the various arguments would be passed to async callback function;
 * Return Value:
 *           aosl_mpq_timer_invalid($ret): failed with aosl_errno set;
 *           other value: the created timer id;
 **/
extern __aosl_api__ aosl_timer_t aosl_mpq_create_oneshot_timer (aosl_mpq_t qid, aosl_timer_func_t func, aosl_obj_dtor_t dtor, uintptr_t argc, ...);

/**
 * Create and start a periodic timer.
 * Parameters:
 *             qid: the mpq id in which the created timer will run;
 *        interval: the timer interval in ms;
 *            func: the timer callback function;
 *            dtor: the timer destructor function;
 *            argc: arguments count would be passed to async callback function;
 *             ...: the various arguments would be passed to async callback function;
 * Return Value:
 *           aosl_mpq_timer_invalid($ret): failed with aosl_errno set;
 *           other value: the created timer id;
 **/
extern __aosl_api__ aosl_timer_t aosl_mpq_set_timer (aosl_mpq_t qid, uintptr_t interval, aosl_timer_func_t func, aosl_obj_dtor_t dtor, uintptr_t argc, ...);

/**
 * Create and start a oneshot timer.
 * Parameters:
 *             qid: the mpq id in which the created timer will run;
 *            func: the timer callback function;
 *            dtor: the timer destructor function;
 *            argc: arguments count would be passed to async callback function;
 *             ...: the various arguments would be passed to async callback function;
 * Return Value:
 *           aosl_mpq_timer_invalid($ret): failed with aosl_errno set;
 *           other value: the created timer id;
 **/
extern __aosl_api__ aosl_timer_t aosl_mpq_set_oneshot_timer (aosl_mpq_t qid, aosl_ts_t expire_time, aosl_timer_func_t func, aosl_obj_dtor_t dtor, uintptr_t argc, ...);

/**
 * Get the timer interval.
 * Parameters:
 *        timer_id: the timer id;
 *      interval_p: the variable address to store the interval value;
 * Return Value:
 *           0: successful, and the interval will be stored to *interval_p;
 *          <0: failed with aosl_errno set;
 **/
extern __aosl_api__ int aosl_mpq_timer_interval (aosl_timer_t timer_id, uintptr_t *interval_p);

/**
 * Check whether the timer is active.
 * Parameters:
 *        timer_id: the timer id;
 *        active_p: the variable address to store the active value:
 *                  1: for active;
 *                  0: for inactive;
 * Return Value:
 *           0: successful, and the active status will be stored to *active_p;
 *          <0: failed with aosl_errno set;
 **/
extern __aosl_api__ int aosl_mpq_timer_active (aosl_timer_t timer_id, int *active_p);

/**
 * Reschedule the specified timer.
 * Parameters:
 *        timer_id: the timer id;
 *        interval: AOSL_INVALID_TIMER_INTERVAL for not change interval;
 *                  other values will be set as the new interval;
 * Return Value:
 *           0: successful;
 *          <0: failed with aosl_errno set;
 * Remarks:
 *        We can use this function to reschedule a new created but not start timer.
 **/
extern __aosl_api__ int aosl_mpq_resched_timer (aosl_timer_t timer_id, uintptr_t interval);

/**
 * Reschedule the specified onshot timer.
 * Parameters:
 *        timer_id: the timer id;
 *     expire_time: 0 is the only invalid value for expire time;
 *                  other values will be set as the new expire time;
 * Return Value:
 *           0: successful;
 *          <0: failed with aosl_errno set;
 * Remarks:
 *        We can use this function to reschedule a new created but not start timer.
 **/
extern __aosl_api__ int aosl_mpq_resched_oneshot_timer (aosl_timer_t timer_id, aosl_ts_t expire_time);

/**
 * Cancel the specified timer.
 * Parameters:
 *        timer_id: the timer id;
 * Return Value:
 *           0: successful;
 *          <0: failed with aosl_errno set;
 **/
extern __aosl_api__ int aosl_mpq_cancel_timer (aosl_timer_t timer_id);

/**
 * Get the N-th argument of the timer object specified by timer_id.
 * Parameters:
 *  timer_id: the timer id you want to retrieve the arg
 *         n: which argument you want to get, the first arg is 0;
 *     arg_p: the argument variable address to save the argument value;
 * Return value:
 *        <0: error occured, and aosl_errno indicates which error;
 *         0: call successful, and '*arg' is value of the N-th argument;
 **/
extern __aosl_api__ int aosl_mpq_timer_arg (aosl_timer_t timer_id, uintptr_t n, uintptr_t *arg_p);

/**
 * Cancel and destroy the specified timer.
 * Parameters:
 *        timer_id: the timer id;
 * Return Value:
 *           0: successful;
 *          <0: failed with aosl_errno set;
 **/
extern __aosl_api__ int aosl_mpq_kill_timer (aosl_timer_t timer_id);


/**
 * Coroutine sleep the specified milliseconds.
 * Parameters:
 *      msecs: sleep time in milliseconds;
 * Return Value:
 *        >=0: successful, returns the sleep id, and coroutine resume
 *             chain will be invoked after the specified milliseconds
 *             elapsed or the sleep was canceled by the following API;
 *         <0: failed with aosl_errno set;
 * Remarks:
 *      this function is very different with common sleep, msleep etc
 *      functions, this is stackless coroutine sleep functionality,
 *      so it will suspend the coroutine rather than the running thread,
 *      and the coroutine resume chain will be invoked after the time
 *      specified by msecs passed.
 **/
extern __aosl_api__ intptr_t aosl_co_sleep_ms (uint32_t msecs);

/**
 * Cancel the async sleep specified by the sleep id.
 * Parameters:
 *     sleep_id: sleep id returned by above async sleep API;
 * Return Value:
 *          0: the sleep was canceled successfully;
 *         <0: failed with aosl_errno set;
 * Remarks:
 *      this function is very useful when we want to cancel an async
 *      sleep due to some purpose, just passing the sleep id returned
 *      by async sleep API as the parameter.
 **/
extern __aosl_api__ int aosl_co_sleep_cancel (intptr_t sleep_id);


#ifdef __cplusplus
}
#endif


#endif /* __AOSL_MPQ_TIMER_H__ */
