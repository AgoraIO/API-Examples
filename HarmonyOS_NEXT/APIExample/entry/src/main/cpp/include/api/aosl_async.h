/*************************************************************
 * Author:	Lionfore Hao (haolianfu@agora.io)
 * Date	 :	Aug 9th, 2023
 * Module:	AOSL async and resume definition file
 *
 *
 * This is a part of the Advanced Operating System Layer.
 * Copyright (C) 2018 ~ 2023 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#ifndef __AOSL_ASYNC_H__
#define __AOSL_ASYNC_H__


#include <api/aosl_types.h>
#include <api/aosl_defs.h>
#include <api/aosl_ref.h>


#ifdef __cplusplus
extern "C" {
#endif



typedef const struct _aosl_stack_ *aosl_stack_id_t;
#define AOSL_STACK_INVALID NULL
#define aosl_stack_invalid(id) ((id) == AOSL_STACK_INVALID)
#define aosl_define_stack(id) aosl_stack_id_t id = (aosl_stack_id_t)&id

/**
 * The async prepare mechanism function prototype.
 * Parameter:
 *      free_only: only do free action, no other action allowed
 *           argc: specify the argv array elements count
 *           argv: array for passing variable args
 *                 
 * Return value:
 *             <0: failed, not continue the prepare functions,
 *                 just do free only for the left functions in
 *                 the stack chain;
 *   other values: success, continue executing left functions;
 **/
typedef int (*aosl_prepare_func_t) (int free_only, uintptr_t argc, uintptr_t argv []);

extern __aosl_api__ int aosl_async_prepare (aosl_stack_id_t stack_id, aosl_ref_t ref, const char *f_name, aosl_prepare_func_t f, uintptr_t argc, ...);
extern __aosl_api__ int aosl_async_prepare_args (aosl_stack_id_t stack_id, aosl_ref_t ref, const char *f_name, aosl_prepare_func_t f, uintptr_t argc, va_list args);
extern __aosl_api__ int aosl_async_prepare_argv (aosl_stack_id_t stack_id, aosl_ref_t ref, const char *f_name, aosl_prepare_func_t f, uintptr_t argc, uintptr_t argv []);

/**
 * The async resume mechanism function prototype.
 * Parameter:
 *      free_only: only do free action, no other action allowed
 *           argc: specify the argv array elements count
 *           argv: array for passing variable args
 *                 
 * Return value:
 *       none.
 **/
typedef void (*aosl_resume_func_t) (int free_only, uintptr_t argc, uintptr_t argv []);

extern __aosl_api__ int aosl_async_resume (aosl_stack_id_t stack_id, aosl_ref_t ref, const char *f_name, aosl_resume_func_t f, uintptr_t argc, ...);
extern __aosl_api__ int aosl_async_resume_args (aosl_stack_id_t stack_id, aosl_ref_t ref, const char *f_name, aosl_resume_func_t f, uintptr_t argc, va_list args);
extern __aosl_api__ int aosl_async_resume_argv (aosl_stack_id_t stack_id, aosl_ref_t ref, const char *f_name, aosl_resume_func_t f, uintptr_t argc, uintptr_t argv []);


/**
 * Start a new coroutine block, which acts just like a C/C++ block
 * scope, and the coroutine will act as a whole logic unit in the
 * block.
 **/
extern __aosl_api__ int aosl_async_block (aosl_argv_f f, uintptr_t argc, ...);
extern __aosl_api__ int aosl_async_block_args (aosl_argv_f f, uintptr_t argc, va_list args);
extern __aosl_api__ int aosl_async_block_argv (aosl_argv_f f, uintptr_t argc, uintptr_t argv []);



#ifdef __cplusplus
}
#endif



#endif /* __AOSL_ASYNC_H__ */