/*************************************************************
 * Author:	Lionfore Hao (haolianfu@agora.io)
 * Date	 :	Aug 15th, 2023
 * Module:	AOSL async class for C++11 definition file
 *
 *
 * This is a part of the Advanced Operating System Layer.
 * Copyright (C) 2018 ~ 2023 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#if (__cplusplus >= 201103) || defined (_MSC_VER)

#ifndef __AOSL_ASYNC_CPP_H__
#define __AOSL_ASYNC_CPP_H__


#include <api/aosl_types.h>
#include <api/aosl_async.h>

#include <functional>
#include <memory>

class aosl_async_class {
public:
	typedef std::function <void (void)> aosl_async_block_lambda_f;

	static int block (aosl_async_block_lambda_f&& task)
	{
		aosl_async_block_lambda_f block_f (std::move (task));
		return aosl_async_block (____block_f, 1, &block_f);
	}

private:
	static void ____block_f (uintptr_t argc, uintptr_t argv [])
	{
		aosl_async_block_lambda_f *block_f = reinterpret_cast<aosl_async_block_lambda_f *>(argv [0]);
		(*block_f) ();
	}

#if (__cplusplus >= 201103) || defined (_MSC_VER)
private:
	aosl_async_class () = delete;
	aosl_async_class (const aosl_async_class &) = delete;
	aosl_async_class (aosl_async_class &&) = delete;
	aosl_async_class &operator = (const aosl_async_class &) = delete;
	aosl_async_class &operator = (aosl_async_class &&) = delete;
	~aosl_async_class () = delete;
#else
private:
	aosl_async_class ();
	aosl_async_class (const aosl_async_class &);
	aosl_async_class &operator = (const aosl_async_class &);
	~aosl_async_class ();
#endif

};

/**
 * Helper async macro for single line code, example:
 * __aosl_async__ (some_func ());
 **/
#define __aosl_async__(_x_y__) { aosl_async_class::block ([&] { _x_y__; }); }

/**
 * Helper async macros for multiple lines code, example:
 *
 * __aosl_async_begin__
 *	// Add your own multiple lines code here:
 *	aosl_define_stack (stack);
 *
 *	task->async ("<task lambda name>", [=] {
 *		......
 *	});
 *
 *	aosl_ref_class::resume (stack, AOSL_REF_INVALID, "<resume lambda name>", [=](int free_only) {
 *		......
 *	});
 * __aosl_async_end__
 *
 **/
#define __aosl_async_begin__ { aosl_async_class::block ([&] {
#define __aosl_async_end__ }); }

/**
 * Helper async macros for implementing a function with return value, example:
 *
 * __aosl_async_func_def__ (<return type>, <ret var>, func_name (func args))
 * {
 *     <func body>
 * }
 * __aosl_async_func_end__ (<ret var>)
 *
 **/
#define __aosl_async_func_def__(_ret_type_, _ret_var_, _fn_and_args_) \
_ret_type_ _fn_and_args_ \
{ \
	_ret_type_ _ret_var_; \
	aosl_async_class::block ([&]

#define __aosl_async_func_end__(_ret_var_) \
	); \
	return _ret_var_; \
}

/**
 * Helper async macros for implementing a void function, example:
 *
 * __aosl_async_func_def_void__ (func_name (func args))
 * {
 *     <func body>
 * }
 * __aosl_async_func_end_void__ ()
 *
 **/
#define __aosl_async_func_def_void__(_fn_) \
void _fn_ \
{ \
	aosl_async_class::block ([&]

#define __aosl_async_func_end_void__() \
	); \
}


#endif /* __AOSL_ASYNC_CPP_H__ */

#endif /* C++11 */