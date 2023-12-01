/*************************************************************
 * Author:	Lionfore Hao (haolianfu@agora.io)
 * Date	 :	May 30th, 2023
 * Module:	AHPL async result object header file
 *
 *
 * This is a part of the Advanced High Performance Library.
 * Copyright (C) 2018 ~ 2023 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#ifndef __AHPL_ARES_H__
#define __AHPL_ARES_H__

#include <api/ahpl_types.h>
#include <api/ahpl_defs.h>
#include <api/ahpl_ref.h>
#include <api/ahpl_poll.h>

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Create an async result object.
 * Parameter:
 *          arg: the parameter attached with the async result object;
 * Return value:
 *     the async result object ref id just created, AHPL_REF_INVALID when failed.
 **/
extern __ahpl_api__ ahpl_ref_t ahpl_ares_create (void *arg);

/**
 * Complete the specified async result object.
 * Parameters:
 *      ref: the async result object ref id;
 *   result: a result value which can be retrieved by wait function;
 * Return value:
 *        <0: error occured, and ahpl_errno indicates which error;
 *       >=0: successful;
 **/
extern __ahpl_api__ int ahpl_ares_complete (ahpl_ref_t ref, intptr_t result);

/**
 * Wait the specified async result object to complete.
 * Parameters:
 *      ref: the async result object ref id;
 *    timeo: maximum waiting time in milliseconds;
 *   result: variable address for the value which was set by complete function,
 *           NOTE: the *result only will be set when the return value of wait
 *                 function is AHPL_POLL_ST_SIGNALED and result != NULL, if you
 *                 do not care the complete result, just passing NULL to it;
 * Return value:
 *        <0: error occured, and ahpl_errno indicates which error;
 *       >=0: AHPL_POLL_ST_* macros value;
 **/
extern __ahpl_api__ int ahpl_ares_wait (ahpl_ref_t ref, intptr_t timeo, intptr_t *result);

/**
 * Reset the specified async result object to non signaled state.
 * Parameters:
 *     ref: the async result object ref id
 * Return value:
 *        <0: error occured, and ahpl_errno indicates which error;
 *       >=0: successful;
 **/
extern __ahpl_api__ int ahpl_ares_reset (ahpl_ref_t ref);



#ifdef __cplusplus
}
#endif

#endif /* __AHPL_ARES_H__ */