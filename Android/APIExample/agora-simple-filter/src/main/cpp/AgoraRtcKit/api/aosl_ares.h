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
 **/
extern __aosl_api__ aosl_ref_t aosl_ares_create (void *arg);

/**
 * Complete the specified async result object.
 * Parameters:
 *      ref: the async result object ref id;
 *   result: a result value which can be retrieved by wait function;
 * Return value:
 *        <0: error occured, and aosl_errno indicates which error;
 *       >=0: successful;
 **/
extern __aosl_api__ int aosl_ares_complete (aosl_ref_t ref, intptr_t result);

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
 **/
extern __aosl_api__ int aosl_ares_wait (aosl_ref_t ref, intptr_t timeo, intptr_t *result);

/**
 * Reset the specified async result object to non signaled state.
 * Parameters:
 *     ref: the async result object ref id
 * Return value:
 *        <0: error occured, and aosl_errno indicates which error;
 *       >=0: successful;
 **/
extern __aosl_api__ int aosl_ares_reset (aosl_ref_t ref);



#ifdef __cplusplus
}
#endif

#endif /* __AOSL_ARES_H__ */