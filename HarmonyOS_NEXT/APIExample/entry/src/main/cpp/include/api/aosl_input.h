/*************************************************************
 * Author:	Lionfore Hao (haolianfu@agora.io)
 * Date	 :	May 21st, 2019
 * Module:	AOSL input object definition file
 *
 *
 * This is a part of the Advanced Operating System Layer.
 * Copyright (C) 2018 ~ 2019 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#ifndef __AOSL_INPUT_H__
#define __AOSL_INPUT_H__


#include <api/aosl_types.h>
#include <api/aosl_defs.h>
#include <api/aosl_mpq.h>
#include <api/aosl_ref.h>


#ifdef __cplusplus
extern "C" {
#endif



/**
 * The input object function prototype.
 * Parameter:
 *           iobj: the input object itself;
 *      free_only: only do free action, no other action allowed
 *           argc: specify the argv array elements count
 *           argv: array for passing variable args
 *                 
 * Return value:
 *       none.
 **/
typedef void (*aosl_input_func_t) (aosl_refobj_t iobj, int free_only, uintptr_t argc, uintptr_t argv []);

/**
 * The input object creating function prototype, which is used to create an input object.
 * Parameters:
 *            arg: the parameter attached with the input object;
 *           dtor: the input object destructor function, which will be invoked when
 *                 the input object is deleted;
 *   waitings_max: input works max waitings:
 *                 0: for sync mode
 *                >0: async mode, specified the max waitings
 *        input_q: input only to the specified MPQ, if specify AOSL_MPQ_INVALID, then the
 *                 input would be sent to one of valid mpq of the general mpq pool;
 **/
extern __aosl_api__ aosl_ref_t aosl_input_create (void *arg, aosl_ref_dtor_t dtor, uintptr_t waitings_max, aosl_mpq_t input_q);

/**
 * Execute the input action, and the callback function will be invoked.
 * Parameters:
 *          input: the input object;
 *         f_name: the functionality name for the callback function;
 *              f: the input action callback function;
 *           argc: arguments count would be passed to the callback function;
 *            ...: the various arguments would be passed to the callback function;
 * Return Value:
 *              0: successful, and the async callback will be invoked;
 *             <0: failed with aosl_errno set;
 * Remarks:
 *         The input action callback function will be invoked according to the
 *         input object creating parameters:
 *         1. if specified a valid mpq when creating, then the input action f
 *            will be invoked in the mpq thread context;
 *         2. if specified AOSL_MPQ_INVALID when creating, then the input action
 *            f will be invoked in any one of mpq thread of general mpq pool;
 *         3. the callback function will be invoked orderly;
 **/
extern __aosl_api__ int aosl_input_exec (aosl_ref_t input, const char *f_name, aosl_input_func_t f, uintptr_t argc, ...);

/* va_list version */
extern __aosl_api__ int aosl_input_exec_args (aosl_ref_t input, const char *f_name, aosl_input_func_t f, uintptr_t argc, va_list args);

/* args vector versions */
extern __aosl_api__ int aosl_input_exec_argv (aosl_ref_t input, const char *f_name, aosl_input_func_t f, uintptr_t argc, uintptr_t argv []);

/**
 * Get the input waitings count.
 * Parameters:
 *          input: the input object;
 * Return Value:
 *            >=0: successful, and the input waitings count;
 *             <0: failed with aosl_errno set;
 **/
extern __aosl_api__ int aosl_input_waitings_count (aosl_ref_t input);

/**
 * Remove one waiting input from the input waitings head, i.e. the eldest input data.
 * Parameters:
 *          input: the input object;
 * Return Value:
 *            >=0: successful, and the input waitings count;
 *             <0: failed with aosl_errno set;
 * Remarks:
 *        The removed data will be passed to the async callback function with free_only
 *        set to true to do the possible resource releasing.
 **/
extern __aosl_api__ int aosl_input_remove_waitings_head (aosl_ref_t input);



#ifdef __cplusplus
}
#endif



#endif /* __AOSL_INPUT_H__ */