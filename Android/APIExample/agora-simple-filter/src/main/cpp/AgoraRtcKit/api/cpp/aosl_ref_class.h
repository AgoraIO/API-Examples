/*************************************************************
 * Author:	Lionfore Hao (haolianfu@agora.io)
 * Date	 :	Nov 19th, 2018
 * Module:	AOSL reference object for C++ definition file
 *
 *
 * This is a part of the Advanced Operating System Layer.
 * Copyright (C) 2018 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#ifndef __AOSL_REF_OBJ_CPP_H__
#define __AOSL_REF_OBJ_CPP_H__


#include <stdlib.h>

#include <api/aosl_types.h>
#include <api/aosl_defs.h>
#include <api/aosl_ref.h>

#ifdef COMPILING_WITH_MPQ_H
#include <api/aosl_mpq.h>
#ifdef COMPILING_WITH_MPQP_H
#include <api/aosl_mpqp.h>
#endif
#endif

#ifdef COMPILING_WITH_ASYNC_H
#include <api/aosl_async.h>
#endif

#if (__cplusplus >= 201103) || defined (_MSC_VER)
#include <functional>
#include <memory>
typedef std::function <void (void *arg)> aosl_ref_lambda_f;
typedef std::function <void (const aosl_ts_t &queued_ts, aosl_refobj_t robj)> aosl_ref_mpq_lambda_f;
typedef std::function <void (void)> aosl_ref_mpq_lambda_0arg_f;
typedef std::function <int (int free_only)> aosl_async_prepare_lambda_f;
typedef std::function <void (int free_only)> aosl_async_resume_lambda_f;
#endif

class aosl_ref_class {
public:
	class aosl_ref_t_oop {
	private:
		/**
		 * We do not allow create any object of this class directly,
		 * only the pointer of this class is significant.
		 **/
		aosl_ref_t_oop ();

	public:
		static aosl_ref_t_oop *create (void *arg = NULL, aosl_ref_dtor_t dtor = NULL, bool caller_free = true)
		{
			return (aosl_ref_t_oop *)aosl_ref_create (arg, dtor, (int)caller_free);
		}

		static aosl_ref_t_oop *from_aosl_ref_t (aosl_ref_t ref)
		{
			return (aosl_ref_t_oop *)ref;
		}

		static void operator delete (void *ptr)
		{
			((aosl_ref_t_oop *)ptr)->destroy (true);
		}

		aosl_ref_t ref () const
		{
			return (aosl_ref_t)this;
		}

		int hold (aosl_ref_func_t f, uintptr_t argc, ...)
		{
			va_list args;
			int err;

			va_start (args, argc);
			err = aosl_ref_hold_args (ref (), f, argc, args);
			va_end (args);

			return err;
		}

		int hold_args (aosl_ref_func_t f, uintptr_t argc, va_list args)
		{
			return aosl_ref_hold_args (ref (), f, argc, args);
		}

		int hold_argv (aosl_ref_func_t f, uintptr_t argc, uintptr_t argv [])
		{
			return aosl_ref_hold_argv (ref (), f, argc, argv);
		}

		int read (aosl_ref_func_t f, uintptr_t argc, ...)
		{
			va_list args;
			int err;

			va_start (args, argc);
			err = aosl_ref_read_args (ref (), f, argc, args);
			va_end (args);

			return err;
		}

		int read_args (aosl_ref_func_t f, uintptr_t argc, va_list args)
		{
			return aosl_ref_read_args (ref (), f, argc, args);
		}

		int read_argv (aosl_ref_func_t f, uintptr_t argc, uintptr_t argv [])
		{
			return aosl_ref_read_argv (ref (), f, argc, argv);
		}

		int write (aosl_ref_func_t f, uintptr_t argc, ...)
		{
			va_list args;
			int err;

			va_start (args, argc);
			err = aosl_ref_write_args (ref (), f, argc, args);
			va_end (args);

			return err;
		}

		int write_args (aosl_ref_func_t f, uintptr_t argc, va_list args)
		{
			return aosl_ref_write_args (ref (), f, argc, args);
		}

		int write_argv (aosl_ref_func_t f, uintptr_t argc, uintptr_t argv [])
		{
			return aosl_ref_write_argv (ref (), f, argc, argv);
		}

		int unsafe (aosl_ref_func_t f, uintptr_t argc, ...)
		{
			va_list args;
			int err;

			va_start (args, argc);
			err = aosl_ref_unsafe_args (ref (), f, argc, args);
			va_end (args);

			return err;
		}

		int unsafe_args (aosl_ref_func_t f, uintptr_t argc, va_list args)
		{
			return aosl_ref_unsafe_args (ref (), f, argc, args);
		}

		int unsafe_argv (aosl_ref_func_t f, uintptr_t argc, uintptr_t argv [])
		{
			return aosl_ref_unsafe_argv (ref (), f, argc, argv);
		}

		/* The static version of member functions */
		static int hold (aosl_ref_t ref, aosl_ref_func_t f, uintptr_t argc, ...)
		{
			va_list args;
			int err;

			va_start (args, argc);
			err = aosl_ref_hold_args (ref, f, argc, args);
			va_end (args);

			return err;
		}

		static int hold_args (aosl_ref_t ref, aosl_ref_func_t f, uintptr_t argc, va_list args)
		{
			return aosl_ref_hold_args (ref, f, argc, args);
		}

		static int hold_argv (aosl_ref_t ref, aosl_ref_func_t f, uintptr_t argc, uintptr_t argv [])
		{
			return aosl_ref_hold_argv (ref, f, argc, argv);
		}

		static int read (aosl_ref_t ref, aosl_ref_func_t f, uintptr_t argc, ...)
		{
			va_list args;
			int err;

			va_start (args, argc);
			err = aosl_ref_read_args (ref, f, argc, args);
			va_end (args);

			return err;
		}

		static int read_args (aosl_ref_t ref, aosl_ref_func_t f, uintptr_t argc, va_list args)
		{
			return aosl_ref_read_args (ref, f, argc, args);
		}

		static int read_argv (aosl_ref_t ref, aosl_ref_func_t f, uintptr_t argc, uintptr_t argv [])
		{
			return aosl_ref_read_argv (ref, f, argc, argv);
		}

		static int write (aosl_ref_t ref, aosl_ref_func_t f, uintptr_t argc, ...)
		{
			va_list args;
			int err;

			va_start (args, argc);
			err = aosl_ref_write_args (ref, f, argc, args);
			va_end (args);

			return err;
		}

		static int write_args (aosl_ref_t ref, aosl_ref_func_t f, uintptr_t argc, va_list args)
		{
			return aosl_ref_write_args (ref, f, argc, args);
		}

		static int write_argv (aosl_ref_t ref, aosl_ref_func_t f, uintptr_t argc, uintptr_t argv [])
		{
			return aosl_ref_write_argv (ref, f, argc, argv);
		}

		static int unsafe (aosl_ref_t ref, aosl_ref_func_t f, uintptr_t argc, ...)
		{
			va_list args;
			int err;

			va_start (args, argc);
			err = aosl_ref_unsafe_args (ref, f, argc, args);
			va_end (args);

			return err;
		}

		static int unsafe_args (aosl_ref_t ref, aosl_ref_func_t f, uintptr_t argc, va_list args)
		{
			return aosl_ref_unsafe_args (ref, f, argc, args);
		}

		static int unsafe_argv (aosl_ref_t ref, aosl_ref_func_t f, uintptr_t argc, uintptr_t argv [])
		{
			return aosl_ref_unsafe_argv (ref, f, argc, argv);
		}

		static int read (aosl_refobj_t robj, aosl_ref_func_t f, uintptr_t argc, ...)
		{
			va_list args;
			int err;

			va_start (args, argc);
			err = aosl_refobj_read_args (robj, f, argc, args);
			va_end (args);

			return err;
		}

		static int read_args (aosl_refobj_t robj, aosl_ref_func_t f, uintptr_t argc, va_list args)
		{
			return aosl_refobj_read_args (robj, f, argc, args);
		}

		static int read_argv (aosl_refobj_t robj, aosl_ref_func_t f, uintptr_t argc, uintptr_t argv [])
		{
			return aosl_refobj_read_argv (robj, f, argc, argv);
		}

		static int unsafe (aosl_refobj_t robj, aosl_ref_func_t f, uintptr_t argc, ...)
		{
			va_list args;
			int err;

			va_start (args, argc);
			err = aosl_refobj_unsafe_args (robj, f, argc, args);
			va_end (args);

			return err;
		}

		static int unsafe_args (aosl_refobj_t robj, aosl_ref_func_t f, uintptr_t argc, va_list args)
		{
			return aosl_refobj_unsafe_args (robj, f, argc, args);
		}

		static int unsafe_argv (aosl_refobj_t robj, aosl_ref_func_t f, uintptr_t argc, uintptr_t argv [])
		{
			return aosl_refobj_unsafe_argv (robj, f, argc, argv);
		}

		/* set the living scope ref object of this ref object */
		int set_scope (aosl_ref_t scope_ref)
		{
			return aosl_ref_set_scope (ref (), scope_ref);
		}

		int destroy (bool do_delete = true)
		{
			if (!aosl_ref_invalid (ref ()))
				return aosl_ref_destroy (ref (), (int)do_delete);

			return -1;
		}

	#ifdef __AOSL_MPQ_H__
		/* MPQ relative encapsulations */
	public:
		int queue (aosl_mpq_t tq, aosl_mpq_t dq, const char *f_name, aosl_mpq_func_argv_t f, uintptr_t argc, ...)
		{
			va_list args;
			int err;

			va_start (args, argc);
			err = aosl_mpq_queue_args (tq, dq, ref (), f_name, f, argc, args);
			va_end (args);

			return err;
		}

		int queue_args (aosl_mpq_t tq, aosl_mpq_t dq, const char *f_name, aosl_mpq_func_argv_t f, uintptr_t argc, va_list args)
		{
			return aosl_mpq_queue_args (tq, dq, ref (), f_name, f, argc, args);
		}

		int queue_argv (aosl_mpq_t tq, aosl_mpq_t dq, const char *f_name, aosl_mpq_func_argv_t f, uintptr_t argc, uintptr_t *argv)
		{
			return aosl_mpq_queue_argv (tq, dq, ref (), f_name, f, argc, argv);
		}

		int queue_data (aosl_mpq_t tq, aosl_mpq_t dq, const char *f_name, aosl_mpq_func_data_t f, size_t len, void *data)
		{
			return aosl_mpq_queue_data (tq, dq, ref (), f_name, f, len, data);
		}

		int call (aosl_mpq_t q, const char *f_name, aosl_mpq_func_argv_t f, uintptr_t argc, ...)
		{
			va_list args;
			int err;

			va_start (args, argc);
			err = aosl_mpq_call_args (q, ref (), f_name, f, argc, args);
			va_end (args);

			return err;
		}

		int call_args (aosl_mpq_t q, const char *f_name, aosl_mpq_func_argv_t f, uintptr_t argc, va_list args)
		{
			return aosl_mpq_call_args (q, ref (), f_name, f, argc, args);
		}

		int call_argv (aosl_mpq_t q, const char *f_name, aosl_mpq_func_argv_t f, uintptr_t argc, uintptr_t *argv)
		{
			return aosl_mpq_call_argv (q, ref (), f_name, f, argc, argv);
		}

		int call_data (aosl_mpq_t q, const char *f_name, aosl_mpq_func_data_t f, size_t len, void *data)
		{
			return aosl_mpq_call_data (q, ref (), f_name, f, len, data);
		}

		int run (aosl_mpq_t q, aosl_mpq_t dq, const char *f_name, aosl_mpq_func_argv_t f, uintptr_t argc, ...)
		{
			va_list args;
			int err;

			va_start (args, argc);
			err = aosl_mpq_run_args (q, dq, ref (), f_name, f, argc, args);
			va_end (args);

			return err;
		}

		int run_args (aosl_mpq_t q, aosl_mpq_t dq, const char *f_name, aosl_mpq_func_argv_t f, uintptr_t argc, va_list args)
		{
			return aosl_mpq_run_args (q, dq, ref (), f_name, f, argc, args);
		}

		int run_argv (aosl_mpq_t q, aosl_mpq_t dq, const char *f_name, aosl_mpq_func_argv_t f, uintptr_t argc, uintptr_t *argv)
		{
			return aosl_mpq_run_argv (q, dq, ref (), f_name, f, argc, argv);
		}

		int run_data (aosl_mpq_t q, aosl_mpq_t dq, const char *f_name, aosl_mpq_func_data_t f, size_t len, void *data)
		{
			return aosl_mpq_run_data (q, dq, ref (), f_name, f, len, data);
		}

	#ifdef __AOSL_MPQP_H__
		/* MPQP relative encapsulations */
		aosl_mpq_t queue (aosl_mpqp_t qp, aosl_mpq_t dq, const char *f_name, aosl_mpq_func_argv_t f, uintptr_t argc, ...)
		{
			va_list args;
			aosl_mpq_t qid;

			va_start (args, argc);
			qid = aosl_mpqp_queue_args (qp, dq, ref (), f_name, f, argc, args);
			va_end (args);

			return qid;
		}

		aosl_mpq_t queue_args (aosl_mpqp_t qp, aosl_mpq_t dq, const char *f_name, aosl_mpq_func_argv_t f, uintptr_t argc, va_list args)
		{
			return aosl_mpqp_queue_args (qp, dq, ref (), f_name, f, argc, args);
		}

		aosl_mpq_t queue_argv (aosl_mpqp_t qp, aosl_mpq_t dq, const char *f_name, aosl_mpq_func_argv_t f, uintptr_t argc, uintptr_t *argv)
		{
			return aosl_mpqp_queue_argv (qp, dq, ref (), f_name, f, argc, argv);
		}

		aosl_mpq_t queue_data (aosl_mpqp_t qp, aosl_mpq_t dq, const char *f_name, aosl_mpq_func_data_t f, size_t len, void *data)
		{
			return aosl_mpqp_queue_data (qp, dq, ref (), f_name, f, len, data);
		}

		aosl_mpq_t call (aosl_mpqp_t qp, const char *f_name, aosl_mpq_func_argv_t f, uintptr_t argc, ...)
		{
			va_list args;
			aosl_mpq_t qid;

			va_start (args, argc);
			qid = aosl_mpqp_call_args (qp, ref (), f_name, f, argc, args);
			va_end (args);

			return qid;
		}

		aosl_mpq_t call_args (aosl_mpqp_t qp, const char *f_name, aosl_mpq_func_argv_t f, uintptr_t argc, va_list args)
		{
			return aosl_mpqp_call_args (qp, ref (), f_name, f, argc, args);
		}

		aosl_mpq_t call_argv (aosl_mpqp_t qp, const char *f_name, aosl_mpq_func_argv_t f, uintptr_t argc, uintptr_t *argv)
		{
			return aosl_mpqp_call_argv (qp, ref (), f_name, f, argc, argv);
		}

		aosl_mpq_t call_data (aosl_mpqp_t qp, const char *f_name, aosl_mpq_func_data_t f, size_t len, void *data)
		{
			return aosl_mpqp_call_data (qp, ref (), f_name, f, len, data);
		}

		aosl_mpq_t run (aosl_mpqp_t qp, aosl_mpq_t dq, const char *f_name, aosl_mpq_func_argv_t f, uintptr_t argc, ...)
		{
			va_list args;
			aosl_mpq_t qid;

			va_start (args, argc);
			qid = aosl_mpqp_run_args (qp, dq, ref (), f_name, f, argc, args);
			va_end (args);

			return qid;
		}

		aosl_mpq_t run_args (aosl_mpqp_t qp, aosl_mpq_t dq, const char *f_name, aosl_mpq_func_argv_t f, uintptr_t argc, va_list args)
		{
			return aosl_mpqp_run_args (qp, dq, ref (), f_name, f, argc, args);
		}

		aosl_mpq_t run_argv (aosl_mpqp_t qp, aosl_mpq_t dq, const char *f_name, aosl_mpq_func_argv_t f, uintptr_t argc, uintptr_t *argv)
		{
			return aosl_mpqp_run_argv (qp, dq, ref (), f_name, f, argc, argv);
		}

		aosl_mpq_t run_data (aosl_mpqp_t qp, aosl_mpq_t dq, const char *f_name, aosl_mpq_func_data_t f, size_t len, void *data)
		{
			return aosl_mpqp_run_data (qp, dq, ref (), f_name, f, len, data);
		}

		int pool_tail_queue (aosl_mpqp_t qp, aosl_mpq_t dq, const char *f_name, aosl_mpq_func_argv_t f, uintptr_t argc, ...)
		{
			va_list args;
			int err;

			va_start (args, argc);
			err = aosl_mpqp_pool_tail_queue_args (qp, dq, ref (), f_name, f, argc, args);
			va_end (args);

			return err;
		}

		int pool_tail_queue_args (aosl_mpqp_t qp, aosl_mpq_t dq, const char *f_name, aosl_mpq_func_argv_t f, uintptr_t argc, va_list args)
		{
			return aosl_mpqp_pool_tail_queue_args (qp, dq, ref (), f_name, f, argc, args);
		}

		int pool_tail_queue_argv (aosl_mpqp_t qp, aosl_mpq_t dq, const char *f_name, aosl_mpq_func_argv_t f, uintptr_t argc, uintptr_t *argv)
		{
			return aosl_mpqp_pool_tail_queue_argv (qp, dq, ref (), f_name, f, argc, argv);
		}
	#endif /* __AOSL_MPQP_H__ */
	#endif /* __AOSL_MPQ_H__ */

		/* C++11 lambda encapsulations */
	#if (__cplusplus >= 201103) || defined (_MSC_VER)
	public:
		int hold (aosl_ref_lambda_f &&lambda_f)
		{
			aosl_ref_lambda_f lambda_obj (std::move (lambda_f));
			return aosl_ref_t_oop::hold (____ref_f, 1, &lambda_obj);
		}

		int read (aosl_ref_lambda_f &&lambda_f)
		{
			aosl_ref_lambda_f lambda_obj (std::move (lambda_f));
			return aosl_ref_t_oop::read (____ref_f, 1, &lambda_obj);
		}

		int write (aosl_ref_lambda_f &&lambda_f)
		{
			aosl_ref_lambda_f lambda_obj (std::move (lambda_f));
			return aosl_ref_t_oop::write (____ref_f, 1, &lambda_obj);
		}

		int unsafe (aosl_ref_lambda_f &&lambda_f)
		{
			aosl_ref_lambda_f lambda_obj (std::move (lambda_f));
			return aosl_ref_t_oop::unsafe (____ref_f, 1, &lambda_obj);
		}

		static int hold (aosl_ref_t ref, aosl_ref_lambda_f &&lambda_f)
		{
			aosl_ref_lambda_f lambda_obj (std::move (lambda_f));
			return aosl_ref_t_oop::hold (ref, ____ref_f, 1, &lambda_obj);
		}

		static int read (aosl_ref_t ref, aosl_ref_lambda_f &&lambda_f)
		{
			aosl_ref_lambda_f lambda_obj (std::move (lambda_f));
			return aosl_ref_t_oop::read (ref, ____ref_f, 1, &lambda_obj);
		}

		static int write (aosl_ref_t ref, aosl_ref_lambda_f &&lambda_f)
		{
			aosl_ref_lambda_f lambda_obj (std::move (lambda_f));
			return aosl_ref_t_oop::write (ref, ____ref_f, 1, &lambda_obj);
		}

		static int unsafe (aosl_ref_t ref, aosl_ref_lambda_f &&lambda_f)
		{
			aosl_ref_lambda_f lambda_obj (std::move (lambda_f));
			return aosl_ref_t_oop::unsafe (ref, ____ref_f, 1, &lambda_obj);
		}

		static int read (aosl_refobj_t robj, aosl_ref_lambda_f &&lambda_f)
		{
			aosl_ref_lambda_f lambda_obj (std::move (lambda_f));
			return aosl_ref_t_oop::read (robj, ____ref_f, 1, &lambda_obj);
		}

		static int unsafe (aosl_refobj_t robj, aosl_ref_lambda_f &&lambda_f)
		{
			aosl_ref_lambda_f lambda_obj (std::move (lambda_f));
			return aosl_ref_t_oop::unsafe (robj, ____ref_f, 1, &lambda_obj);
		}

	private:
		static void ____ref_f (void *arg, uintptr_t argc, uintptr_t argv [])
		{
			aosl_ref_lambda_f *lambda_obj = reinterpret_cast<aosl_ref_lambda_f *>(argv [0]);
			(*lambda_obj) (arg);
		}

	#ifdef __AOSL_MPQ_H__
	public:
		/* MPQ encapsulations */
		int queue (aosl_mpq_t tq, aosl_mpq_t dq, const char *f_name, aosl_ref_mpq_lambda_f&& task)
		{
			aosl_ref_mpq_lambda_f *task_obj = new aosl_ref_mpq_lambda_f (std::move (task));
			int err = aosl_ref_t_oop::queue (tq, dq, f_name, ____mpq_f, 1, task_obj);
			if (err < 0)
				delete task_obj;

			return err;
		}

		int call (aosl_mpq_t q, const char *f_name, aosl_ref_mpq_lambda_f&& task, void *task_result = NULL)
		{
			aosl_ref_mpq_lambda_f *task_obj = new aosl_ref_mpq_lambda_f (std::move (task));
			int err = aosl_ref_t_oop::call (q, f_name, ____mpq_f, 2, task_obj, task_result);
			if (err < 0)
				delete task_obj;

			return err;
		}

		int run (aosl_mpq_t q, const char *f_name, aosl_ref_mpq_lambda_f&& task)
		{
			aosl_ref_mpq_lambda_f *task_obj = new aosl_ref_mpq_lambda_f (std::move (task));
			int err = aosl_ref_t_oop::run (q, AOSL_MPQ_INVALID, f_name, ____mpq_f, 1, task_obj);
			if (err < 0)
				delete task_obj;

			return err;
		}

		int queue (aosl_mpq_t tq, const char *f_name, aosl_ref_mpq_lambda_0arg_f&& task)
		{
			aosl_ref_mpq_lambda_0arg_f *task_obj = new aosl_ref_mpq_lambda_0arg_f (std::move (task));
			int err = aosl_ref_t_oop::queue (tq, AOSL_MPQ_INVALID, f_name, ____mpq_0arg_f, 1, task_obj);
			if (err < 0)
				delete task_obj;

			return err;
		}

		int call (aosl_mpq_t q, const char *f_name, aosl_ref_mpq_lambda_0arg_f&& task, void *task_result = NULL)
		{
			aosl_ref_mpq_lambda_0arg_f *task_obj = new aosl_ref_mpq_lambda_0arg_f (std::move (task));
			int err = aosl_ref_t_oop::call (q, f_name, ____mpq_0arg_f, 2, task_obj, task_result);
			if (err < 0)
				delete task_obj;

			return err;
		}

		int run (aosl_mpq_t q, const char *f_name, aosl_ref_mpq_lambda_0arg_f&& task)
		{
			aosl_ref_mpq_lambda_0arg_f *task_obj = new aosl_ref_mpq_lambda_0arg_f (std::move (task));
			int err = aosl_ref_t_oop::run (q, AOSL_MPQ_INVALID, f_name, ____mpq_0arg_f, 1, task_obj);
			if (err < 0)
				delete task_obj;

			return err;
		}

	#ifdef __AOSL_MPQP_H__
		/* MPQP encapsulations */
		aosl_mpq_t queue (aosl_mpqp_t qp, aosl_mpq_t dq, const char *f_name, aosl_ref_mpq_lambda_f&& task)
		{
			aosl_ref_mpq_lambda_f *task_obj = new aosl_ref_mpq_lambda_f (std::move (task));
			aosl_mpq_t qid = aosl_ref_t_oop::queue (qp, dq, f_name, ____mpq_f, 1, task_obj);
			if (aosl_mpq_invalid (qid))
				delete task_obj;

			return qid;
		}

		aosl_mpq_t call (aosl_mpqp_t qp, const char *f_name, aosl_ref_mpq_lambda_f&& task, void *task_result = NULL)
		{
			aosl_ref_mpq_lambda_f *task_obj = new aosl_ref_mpq_lambda_f (std::move (task));
			aosl_mpq_t qid = aosl_ref_t_oop::call (qp, f_name, ____mpq_f, 2, task_obj, task_result);
			if (aosl_mpq_invalid (qid))
				delete task_obj;

			return qid;
		}

		aosl_mpq_t run (aosl_mpqp_t qp, const char *f_name, aosl_ref_mpq_lambda_f&& task)
		{
			aosl_ref_mpq_lambda_f *task_obj = new aosl_ref_mpq_lambda_f (std::move (task));
			aosl_mpq_t qid = aosl_ref_t_oop::run (qp, AOSL_MPQ_INVALID, f_name, ____mpq_f, 1, task_obj);
			if (aosl_mpq_invalid (qid))
				delete task_obj;

			return qid;
		}

		int pool_tail_queue (aosl_mpqp_t qp, aosl_mpq_t dq, const char *f_name, aosl_ref_mpq_lambda_f&& task)
		{
			aosl_ref_mpq_lambda_f *task_obj = new aosl_ref_mpq_lambda_f (std::move (task));
			int err = aosl_ref_t_oop::pool_tail_queue (qp, dq, f_name, ____mpq_f, 1, task_obj);
			if (err < 0)
				delete task_obj;

			return err;
		}

		aosl_mpq_t queue (aosl_mpqp_t qp, const char *f_name, aosl_ref_mpq_lambda_0arg_f&& task)
		{
			aosl_ref_mpq_lambda_0arg_f *task_obj = new aosl_ref_mpq_lambda_0arg_f (std::move (task));
			aosl_mpq_t qid = aosl_ref_t_oop::queue (qp, AOSL_MPQ_INVALID, f_name, ____mpq_0arg_f, 1, task_obj);
			if (aosl_mpq_invalid (qid))
				delete task_obj;

			return qid;
		}

		aosl_mpq_t call (aosl_mpqp_t qp, const char *f_name, aosl_ref_mpq_lambda_0arg_f&& task, void *task_result = NULL)
		{
			aosl_ref_mpq_lambda_0arg_f *task_obj = new aosl_ref_mpq_lambda_0arg_f (std::move (task));
			aosl_mpq_t qid = aosl_ref_t_oop::call (qp, f_name, ____mpq_0arg_f, 2, task_obj, task_result);
			if (aosl_mpq_invalid (qid))
				delete task_obj;

			return qid;
		}

		aosl_mpq_t run (aosl_mpqp_t qp, const char *f_name, aosl_ref_mpq_lambda_0arg_f&& task)
		{
			aosl_ref_mpq_lambda_0arg_f *task_obj = new aosl_ref_mpq_lambda_0arg_f (std::move (task));
			aosl_mpq_t qid = aosl_ref_t_oop::run (qp, AOSL_MPQ_INVALID, f_name, ____mpq_0arg_f, 1, task_obj);
			if (aosl_mpq_invalid (qid))
				delete task_obj;

			return qid;
		}

		int pool_tail_queue (aosl_mpqp_t qp, const char *f_name, aosl_ref_mpq_lambda_0arg_f&& task)
		{
			aosl_ref_mpq_lambda_0arg_f *task_obj = new aosl_ref_mpq_lambda_0arg_f (std::move (task));
			int err = aosl_ref_t_oop::pool_tail_queue (qp, AOSL_MPQ_INVALID, f_name, ____mpq_0arg_f, 1, task_obj);
			if (err < 0)
				delete task_obj;

			return err;
		}
	#endif /* __AOSL_MPQP_H__ */

		/* MPQ with specified ref encapsulations */
		static int queue (aosl_mpq_t tq, aosl_mpq_t dq, aosl_ref_t ref, const char *f_name, aosl_ref_mpq_lambda_f&& task)
		{
			aosl_ref_mpq_lambda_f *task_obj = new aosl_ref_mpq_lambda_f (std::move (task));
			int err = aosl_mpq_queue (tq, dq, ref, f_name, ____mpq_f, 1, task_obj);
			if (err < 0)
				delete task_obj;

			return err;
		}

		static int call (aosl_mpq_t q, aosl_ref_t ref, const char *f_name, aosl_ref_mpq_lambda_f&& task, void *task_result = NULL)
		{
			aosl_ref_mpq_lambda_f *task_obj = new aosl_ref_mpq_lambda_f (std::move (task));
			int err = aosl_mpq_call (q, ref, f_name, ____mpq_f, 2, task_obj, task_result);
			if (err < 0)
				delete task_obj;

			return err;
		}

		static int run (aosl_mpq_t q, aosl_ref_t ref, const char *f_name, aosl_ref_mpq_lambda_f&& task)
		{
			aosl_ref_mpq_lambda_f *task_obj = new aosl_ref_mpq_lambda_f (std::move (task));
			int err = aosl_mpq_run (q, AOSL_MPQ_INVALID, ref, f_name, ____mpq_f, 1, task_obj);
			if (err < 0)
				delete task_obj;

			return err;
		}

		static int queue (aosl_mpq_t tq, aosl_ref_t ref, const char *f_name, aosl_ref_mpq_lambda_0arg_f&& task)
		{
			aosl_ref_mpq_lambda_0arg_f *task_obj = new aosl_ref_mpq_lambda_0arg_f (std::move (task));
			int err = aosl_mpq_queue (tq, AOSL_MPQ_INVALID, ref, f_name, ____mpq_0arg_f, 1, task_obj);
			if (err < 0)
				delete task_obj;

			return err;
		}

		static int call (aosl_mpq_t q, aosl_ref_t ref, const char *f_name, aosl_ref_mpq_lambda_0arg_f&& task, void *task_result = NULL)
		{
			aosl_ref_mpq_lambda_0arg_f *task_obj = new aosl_ref_mpq_lambda_0arg_f (std::move (task));
			int err = aosl_mpq_call (q, ref, f_name, ____mpq_0arg_f, 2, task_obj, task_result);
			if (err < 0)
				delete task_obj;

			return err;
		}

		static int run (aosl_mpq_t q, aosl_ref_t ref, const char *f_name, aosl_ref_mpq_lambda_0arg_f&& task)
		{
			aosl_ref_mpq_lambda_0arg_f *task_obj = new aosl_ref_mpq_lambda_0arg_f (std::move (task));
			int err = aosl_mpq_run (q, AOSL_MPQ_INVALID, ref, f_name, ____mpq_0arg_f, 1, task_obj);
			if (err < 0)
				delete task_obj;

			return err;
		}

	#ifdef __AOSL_MPQP_H__
		/* MPQP with specified ref encapsulations */
		static aosl_mpq_t queue (aosl_mpqp_t qp, aosl_mpq_t dq, aosl_ref_t ref, const char *f_name, aosl_ref_mpq_lambda_f&& task)
		{
			aosl_ref_mpq_lambda_f *task_obj = new aosl_ref_mpq_lambda_f (std::move (task));
			aosl_mpq_t qid = aosl_mpqp_queue (qp, dq, ref, f_name, ____mpq_f, 1, task_obj);
			if (aosl_mpq_invalid (qid))
				delete task_obj;

			return qid;
		}

		static aosl_mpq_t call (aosl_mpqp_t qp, aosl_ref_t ref, const char *f_name, aosl_ref_mpq_lambda_f&& task, void *task_result = NULL)
		{
			aosl_ref_mpq_lambda_f *task_obj = new aosl_ref_mpq_lambda_f (std::move (task));
			aosl_mpq_t qid = aosl_mpqp_call (qp, ref, f_name, ____mpq_f, 2, task_obj, task_result);
			if (aosl_mpq_invalid (qid))
				delete task_obj;

			return qid;
		}

		static aosl_mpq_t run (aosl_mpqp_t qp, aosl_ref_t ref, const char *f_name, aosl_ref_mpq_lambda_f&& task)
		{
			aosl_ref_mpq_lambda_f *task_obj = new aosl_ref_mpq_lambda_f (std::move (task));
			aosl_mpq_t qid = aosl_mpqp_run (qp, AOSL_MPQ_INVALID, ref, f_name, ____mpq_f, 1, task_obj);
			if (aosl_mpq_invalid (qid))
				delete task_obj;

			return qid;
		}

		static int pool_tail_queue (aosl_mpqp_t qp, aosl_mpq_t dq, aosl_ref_t ref, const char *f_name, aosl_ref_mpq_lambda_f&& task)
		{
			aosl_ref_mpq_lambda_f *task_obj = new aosl_ref_mpq_lambda_f (std::move (task));
			int err = aosl_mpqp_pool_tail_queue (qp, dq, ref, f_name, ____mpq_f, 1, task_obj);
			if (err < 0)
				delete task_obj;

			return err;
		}

		static aosl_mpq_t queue (aosl_mpqp_t qp, aosl_ref_t ref, const char *f_name, aosl_ref_mpq_lambda_0arg_f&& task)
		{
			aosl_ref_mpq_lambda_0arg_f *task_obj = new aosl_ref_mpq_lambda_0arg_f (std::move (task));
			aosl_mpq_t qid = aosl_mpqp_queue (qp, AOSL_MPQ_INVALID, ref, f_name, ____mpq_0arg_f, 1, task_obj);
			if (aosl_mpq_invalid (qid))
				delete task_obj;

			return qid;
		}

		static aosl_mpq_t call (aosl_mpqp_t qp, aosl_ref_t ref, const char *f_name, aosl_ref_mpq_lambda_0arg_f&& task, void *task_result = NULL)
		{
			aosl_ref_mpq_lambda_0arg_f *task_obj = new aosl_ref_mpq_lambda_0arg_f (std::move (task));
			aosl_mpq_t qid = aosl_mpqp_call (qp, ref, f_name, ____mpq_0arg_f, 2, task_obj, task_result);
			if (aosl_mpq_invalid (qid))
				delete task_obj;

			return qid;
		}

		static aosl_mpq_t run (aosl_mpqp_t qp, aosl_ref_t ref, const char *f_name, aosl_ref_mpq_lambda_0arg_f&& task)
		{
			aosl_ref_mpq_lambda_0arg_f *task_obj = new aosl_ref_mpq_lambda_0arg_f (std::move (task));
			aosl_mpq_t qid = aosl_mpqp_run (qp, AOSL_MPQ_INVALID, ref, f_name, ____mpq_0arg_f, 1, task_obj);
			if (aosl_mpq_invalid (qid))
				delete task_obj;

			return qid;
		}

		static int pool_tail_queue (aosl_mpqp_t qp, aosl_ref_t ref, const char *f_name, aosl_ref_mpq_lambda_0arg_f&& task)
		{
			aosl_ref_mpq_lambda_0arg_f *task_obj = new aosl_ref_mpq_lambda_0arg_f (std::move (task));
			int err = aosl_mpqp_pool_tail_queue (qp, AOSL_MPQ_INVALID, ref, f_name, ____mpq_0arg_f, 1, task_obj);
			if (err < 0)
				delete task_obj;

			return err;
		}
	#endif /* __AOSL_MPQP_H__ */

		static void *call_result_var_addr (void)
		{
			void *var_addr;

			if (aosl_mpq_run_func_arg (1, (uintptr_t *)&var_addr) < 0)
				return NULL;

			return var_addr;
		}

	private:
		static void ____mpq_f (const aosl_ts_t *queued_ts_p, aosl_refobj_t robj, uintptr_t argc, uintptr_t argv [])
		{
			aosl_ref_mpq_lambda_f *task_obj = reinterpret_cast<aosl_ref_mpq_lambda_f *>(argv [0]);
			aosl_mpq_t done_qid = aosl_mpq_run_func_done_qid ();
			(*task_obj) (*queued_ts_p, robj);
			if (aosl_mpq_invalid (done_qid) || aosl_is_free_only (robj)) {
				/**
				 * We only free the task object when the running function has no
				 * done mpq id, due to the task object would be still in use if
				 * the function has a done mpq id when queuing back to the done
				 * mpq.
				 * -- Lionfore Hao Nov 19th, 2018
				 **/
				delete task_obj;
			}
		}

		static void ____mpq_0arg_f (const aosl_ts_t *queued_ts_p, aosl_refobj_t robj, uintptr_t argc, uintptr_t argv [])
		{
			aosl_ref_mpq_lambda_0arg_f *task_obj = reinterpret_cast<aosl_ref_mpq_lambda_0arg_f *>(argv [0]);
			aosl_mpq_t done_qid = aosl_mpq_run_func_done_qid ();
			if (!aosl_is_free_only (robj))
				(*task_obj) ();
			if (aosl_mpq_invalid (done_qid) || aosl_is_free_only (robj)) {
				/**
				 * We only free the task object when the running function has no
				 * done mpq id, due to the task object would be still in use if
				 * the function has a done mpq id when queuing back to the done
				 * mpq.
				 * -- Lionfore Hao Nov 19th, 2018
				 **/
				delete task_obj;
			}
		}
	#endif /* __AOSL_MPQ_H__ */

	#ifdef __AOSL_ASYNC_H__
		/**
		 * The stackless coroutine like implementation in AOSL. We could not
		 * support the real stackless coroutine except in the language level,
		 * so we just provide similar equivalent functionals here.
		 **/
	public:
		int prepare (aosl_stack_id_t stack_id, const char *f_name, aosl_async_prepare_lambda_f&& task)
		{
			aosl_async_prepare_lambda_f *prepare_f = new aosl_async_prepare_lambda_f (std::move (task));
			int err = aosl_async_prepare (stack_id, ref (), f_name, ____async_prepare_f, 1, prepare_f);
			if (err < 0)
				delete prepare_f;

			return err;
		}

		static int prepare (aosl_stack_id_t stack_id, aosl_ref_t ref, const char *f_name, aosl_async_prepare_lambda_f&& task)
		{
			aosl_async_prepare_lambda_f *prepare_f = new aosl_async_prepare_lambda_f (std::move (task));
			int err = aosl_async_prepare (stack_id, ref, f_name, ____async_prepare_f, 1, prepare_f);
			if (err < 0)
				delete prepare_f;

			return err;
		}

	private:
		static int ____async_prepare_f (int free_only, uintptr_t argc, uintptr_t argv [])
		{
			aosl_async_prepare_lambda_f *prepare_f = reinterpret_cast<aosl_async_prepare_lambda_f *>(argv [0]);
			int err;
			err = (*prepare_f) (free_only);
			delete prepare_f;
			return err;
		}

	public:
		int resume (aosl_stack_id_t stack_id, const char *f_name, aosl_async_resume_lambda_f&& task)
		{
			aosl_async_resume_lambda_f *resume_f = new aosl_async_resume_lambda_f (std::move (task));
			int err = aosl_async_resume (stack_id, ref (), f_name, ____async_resume_f, 1, resume_f);
			if (err < 0)
				delete resume_f;

			return err;
		}

		static int resume (aosl_stack_id_t stack_id, aosl_ref_t ref, const char *f_name, aosl_async_resume_lambda_f&& task)
		{
			aosl_async_resume_lambda_f *resume_f = new aosl_async_resume_lambda_f (std::move (task));
			int err = aosl_async_resume (stack_id, ref, f_name, ____async_resume_f, 1, resume_f);
			if (err < 0)
				delete resume_f;

			return err;
		}

	private:
		static void ____async_resume_f (int free_only, uintptr_t argc, uintptr_t argv [])
		{
			aosl_async_resume_lambda_f *resume_f = reinterpret_cast<aosl_async_resume_lambda_f *>(argv [0]);
			(*resume_f) (free_only);
			delete resume_f;
		}
	#endif /* __AOSL_ASYNC_H__ */
	#endif /* C++11 */

	#if (__cplusplus >= 201103) || defined (_MSC_VER)
	private:
		aosl_ref_t_oop (const aosl_ref_t_oop &) = delete;
		aosl_ref_t_oop (aosl_ref_t_oop &&) = delete;
		aosl_ref_t_oop &operator = (const aosl_ref_t_oop &) = delete;
		aosl_ref_t_oop &operator = (aosl_ref_t_oop &&) = delete;
	#else
	private:
		aosl_ref_t_oop (const aosl_ref_t_oop &);
		aosl_ref_t_oop &operator = (const aosl_ref_t_oop &);
	#endif /* C++11 */
	};

private:
	aosl_ref_t_oop *refoop;

public:
	aosl_ref_class (bool caller_free = true)
	{
		refoop = aosl_ref_t_oop::create (this, __dtor, caller_free);
		if (aosl_ref_invalid (refoop))
			abort ();
	}

	aosl_ref_class (aosl_ref_t_oop *obj)
	{
		refoop = obj;
	}

	aosl_ref_class (aosl_ref_t ref)
	{
		refoop = aosl_ref_t_oop::from_aosl_ref_t (ref);
	}

	aosl_ref_t_oop *ref_oop () const
	{
		return refoop;
	}

	aosl_ref_t ref () const
	{
		return refoop->ref ();
	}

	int hold (aosl_ref_func_t f, uintptr_t argc, ...)
	{
		va_list args;
		int err;

		va_start (args, argc);
		err = refoop->hold_args (f, argc, args);
		va_end (args);

		return err;
	}

	int hold_args (aosl_ref_func_t f, uintptr_t argc, va_list args)
	{
		return refoop->hold_args (f, argc, args);
	}

	int hold_argv (aosl_ref_func_t f, uintptr_t argc, uintptr_t argv [])
	{
		return refoop->hold_argv (f, argc, argv);
	}

	int read (aosl_ref_func_t f, uintptr_t argc, ...)
	{
		va_list args;
		int err;

		va_start (args, argc);
		err = refoop->read_args (f, argc, args);
		va_end (args);

		return err;
	}

	int read_args (aosl_ref_func_t f, uintptr_t argc, va_list args)
	{
		return refoop->read_args (f, argc, args);
	}

	int read_argv (aosl_ref_func_t f, uintptr_t argc, uintptr_t argv [])
	{
		return refoop->read_argv (f, argc, argv);
	}

	int write (aosl_ref_func_t f, uintptr_t argc, ...)
	{
		va_list args;
		int err;

		va_start (args, argc);
		err = refoop->write_args (f, argc, args);
		va_end (args);

		return err;
	}

	int write_args (aosl_ref_func_t f, uintptr_t argc, va_list args)
	{
		return refoop->write_args (f, argc, args);
	}

	int write_argv (aosl_ref_func_t f, uintptr_t argc, uintptr_t argv [])
	{
		return refoop->write_argv (f, argc, argv);
	}

	int unsafe (aosl_ref_func_t f, uintptr_t argc, ...)
	{
		va_list args;
		int err;

		va_start (args, argc);
		err = refoop->unsafe_args (f, argc, args);
		va_end (args);

		return err;
	}

	int unsafe_args (aosl_ref_func_t f, uintptr_t argc, va_list args)
	{
		return refoop->unsafe_args (f, argc, args);
	}

	int unsafe_argv (aosl_ref_func_t f, uintptr_t argc, uintptr_t argv [])
	{
		return refoop->unsafe_argv (f, argc, argv);
	}

	int maystall (aosl_ref_func_t f, uintptr_t argc, ...)
	{
		va_list args;
		int err;

		va_start (args, argc);
		err = refoop->unsafe_args (f, argc, args);
		va_end (args);

		return err;
	}

	int maystall_args (aosl_ref_func_t f, uintptr_t argc, va_list args)
	{
		return refoop->unsafe_args (f, argc, args);
	}

	int maystall_argv (aosl_ref_func_t f, uintptr_t argc, uintptr_t argv [])
	{
		return refoop->unsafe_argv (f, argc, argv);
	}

	/* The static version of member functions */
	static int hold (aosl_ref_t ref, aosl_ref_func_t f, uintptr_t argc, ...)
	{
		va_list args;
		int err;

		va_start (args, argc);
		err = aosl_ref_t_oop::hold_args (ref, f, argc, args);
		va_end (args);

		return err;
	}

	static int hold_args (aosl_ref_t ref, aosl_ref_func_t f, uintptr_t argc, va_list args)
	{
		return aosl_ref_t_oop::hold_args (ref, f, argc, args);
	}

	static int hold_argv (aosl_ref_t ref, aosl_ref_func_t f, uintptr_t argc, uintptr_t argv [])
	{
		return aosl_ref_t_oop::hold_argv (ref, f, argc, argv);
	}

	static int read (aosl_ref_t ref, aosl_ref_func_t f, uintptr_t argc, ...)
	{
		va_list args;
		int err;

		va_start (args, argc);
		err = aosl_ref_t_oop::read_args (ref, f, argc, args);
		va_end (args);

		return err;
	}

	static int read_args (aosl_ref_t ref, aosl_ref_func_t f, uintptr_t argc, va_list args)
	{
		return aosl_ref_t_oop::read_args (ref, f, argc, args);
	}

	static int read_argv (aosl_ref_t ref, aosl_ref_func_t f, uintptr_t argc, uintptr_t argv [])
	{
		return aosl_ref_t_oop::read_argv (ref, f, argc, argv);
	}

	static int write (aosl_ref_t ref, aosl_ref_func_t f, uintptr_t argc, ...)
	{
		va_list args;
		int err;

		va_start (args, argc);
		err = aosl_ref_t_oop::write_args (ref, f, argc, args);
		va_end (args);

		return err;
	}

	static int write_args (aosl_ref_t ref, aosl_ref_func_t f, uintptr_t argc, va_list args)
	{
		return aosl_ref_t_oop::write_args (ref, f, argc, args);
	}

	static int write_argv (aosl_ref_t ref, aosl_ref_func_t f, uintptr_t argc, uintptr_t argv [])
	{
		return aosl_ref_t_oop::write_argv (ref, f, argc, argv);
	}

	static int unsafe (aosl_ref_t ref, aosl_ref_func_t f, uintptr_t argc, ...)
	{
		va_list args;
		int err;

		va_start (args, argc);
		err = aosl_ref_t_oop::unsafe_args (ref, f, argc, args);
		va_end (args);

		return err;
	}

	static int unsafe_args (aosl_ref_t ref, aosl_ref_func_t f, uintptr_t argc, va_list args)
	{
		return aosl_ref_t_oop::unsafe_args (ref, f, argc, args);
	}

	static int unsafe_argv (aosl_ref_t ref, aosl_ref_func_t f, uintptr_t argc, uintptr_t argv [])
	{
		return aosl_ref_t_oop::unsafe_argv (ref, f, argc, argv);
	}

	static int maystall (aosl_ref_t ref, aosl_ref_func_t f, uintptr_t argc, ...)
	{
		va_list args;
		int err;

		va_start (args, argc);
		err = aosl_ref_t_oop::unsafe_args (ref, f, argc, args);
		va_end (args);

		return err;
	}

	static int maystall_args (aosl_ref_t ref, aosl_ref_func_t f, uintptr_t argc, va_list args)
	{
		return aosl_ref_t_oop::unsafe_args (ref, f, argc, args);
	}

	static int maystall_argv (aosl_ref_t ref, aosl_ref_func_t f, uintptr_t argc, uintptr_t argv [])
	{
		return aosl_ref_t_oop::unsafe_argv (ref, f, argc, argv);
	}

	static int read (aosl_refobj_t robj, aosl_ref_func_t f, uintptr_t argc, ...)
	{
		va_list args;
		int err;

		va_start (args, argc);
		err = aosl_ref_t_oop::read_args (robj, f, argc, args);
		va_end (args);

		return err;
	}

	static int read_args (aosl_refobj_t robj, aosl_ref_func_t f, uintptr_t argc, va_list args)
	{
		return aosl_ref_t_oop::read_args (robj, f, argc, args);
	}

	static int read_argv (aosl_refobj_t robj, aosl_ref_func_t f, uintptr_t argc, uintptr_t argv [])
	{
		return aosl_ref_t_oop::read_argv (robj, f, argc, argv);
	}

	static int unsafe (aosl_refobj_t robj, aosl_ref_func_t f, uintptr_t argc, ...)
	{
		va_list args;
		int err;

		va_start (args, argc);
		err = aosl_ref_t_oop::unsafe_args (robj, f, argc, args);
		va_end (args);

		return err;
	}

	static int unsafe_args (aosl_refobj_t robj, aosl_ref_func_t f, uintptr_t argc, va_list args)
	{
		return aosl_ref_t_oop::unsafe_args (robj, f, argc, args);
	}

	static int unsafe_argv (aosl_refobj_t robj, aosl_ref_func_t f, uintptr_t argc, uintptr_t argv [])
	{
		return aosl_ref_t_oop::unsafe_argv (robj, f, argc, argv);
	}

	static int maystall (aosl_refobj_t robj, aosl_ref_func_t f, uintptr_t argc, ...)
	{
		va_list args;
		int err;

		va_start (args, argc);
		err = aosl_ref_t_oop::unsafe_args (robj, f, argc, args);
		va_end (args);

		return err;
	}

	static int maystall_args (aosl_refobj_t robj, aosl_ref_func_t f, uintptr_t argc, va_list args)
	{
		return aosl_ref_t_oop::unsafe_args (robj, f, argc, args);
	}

	static int maystall_argv (aosl_refobj_t robj, aosl_ref_func_t f, uintptr_t argc, uintptr_t argv [])
	{
		return aosl_ref_t_oop::unsafe_argv (robj, f, argc, argv);
	}

	static aosl_ref_class *from_refobj (aosl_refobj_t robj)
	{
		return (aosl_ref_class *)aosl_refobj_arg (robj);
	}

	/* set the living scope ref object of this ref object */
	int set_scope (aosl_ref_t scope_ref)
	{
		return refoop->set_scope (scope_ref);
	}

	int destroy (bool do_delete = true)
	{
		int err = refoop->destroy (do_delete);
		if (err < 0 && do_delete) {
			::delete this;
			return 0;
		}

		return err;
	}

public:
	class deleter {
	public:
		void operator () (aosl_ref_class *obj_ptr) const
		{
			if (obj_ptr != NULL)
				obj_ptr->destroy ();
		}
	};

protected:
	/* We do not allow delete this object directly. */
	virtual ~aosl_ref_class ()
	{
	}

private:
	static void __dtor (void *arg)
	{
		aosl_ref_class *__this = (aosl_ref_class *)arg;
		::delete __this;
	}

#ifdef __AOSL_MPQ_H__
	/* MPQ relative encapsulations */
public:
	int queue (aosl_mpq_t tq, aosl_mpq_t dq, const char *f_name, aosl_mpq_func_argv_t f, uintptr_t argc, ...)
	{
		va_list args;
		int err;

		va_start (args, argc);
		err = refoop->queue_args (tq, dq, f_name, f, argc, args);
		va_end (args);

		return err;
	}

	int queue_args (aosl_mpq_t tq, aosl_mpq_t dq, const char *f_name, aosl_mpq_func_argv_t f, uintptr_t argc, va_list args)
	{
		return refoop->queue_args (tq, dq, f_name, f, argc, args);
	}

	int queue_argv (aosl_mpq_t tq, aosl_mpq_t dq, const char *f_name, aosl_mpq_func_argv_t f, uintptr_t argc, uintptr_t *argv)
	{
		return refoop->queue_argv (tq, dq, f_name, f, argc, argv);
	}

	int queue_data (aosl_mpq_t tq, aosl_mpq_t dq, const char *f_name, aosl_mpq_func_data_t f, size_t len, void *data)
	{
		return refoop->queue_data (tq, dq, f_name, f, len, data);
	}

	int call (aosl_mpq_t q, const char *f_name, aosl_mpq_func_argv_t f, uintptr_t argc, ...)
	{
		va_list args;
		int err;

		va_start (args, argc);
		err = refoop->call_args (q, f_name, f, argc, args);
		va_end (args);

		return err;
	}

	int call_args (aosl_mpq_t q, const char *f_name, aosl_mpq_func_argv_t f, uintptr_t argc, va_list args)
	{
		return refoop->call_args (q, f_name, f, argc, args);
	}

	int call_argv (aosl_mpq_t q, const char *f_name, aosl_mpq_func_argv_t f, uintptr_t argc, uintptr_t *argv)
	{
		return refoop->call_argv (q, f_name, f, argc, argv);
	}

	int call_data (aosl_mpq_t q, const char *f_name, aosl_mpq_func_data_t f, size_t len, void *data)
	{
		return refoop->call_data (q, f_name, f, len, data);
	}

	int run (aosl_mpq_t q, aosl_mpq_t dq, const char *f_name, aosl_mpq_func_argv_t f, uintptr_t argc, ...)
	{
		va_list args;
		int err;

		va_start (args, argc);
		err = refoop->run_args (q, dq, f_name, f, argc, args);
		va_end (args);

		return err;
	}

	int run_args (aosl_mpq_t q, aosl_mpq_t dq, const char *f_name, aosl_mpq_func_argv_t f, uintptr_t argc, va_list args)
	{
		return refoop->run_args (q, dq, f_name, f, argc, args);
	}

	int run_argv (aosl_mpq_t q, aosl_mpq_t dq, const char *f_name, aosl_mpq_func_argv_t f, uintptr_t argc, uintptr_t *argv)
	{
		return refoop->run_argv (q, dq, f_name, f, argc, argv);
	}

	int run_data (aosl_mpq_t q, aosl_mpq_t dq, const char *f_name, aosl_mpq_func_data_t f, size_t len, void *data)
	{
		return refoop->run_data (q, dq, f_name, f, len, data);
	}

#ifdef __AOSL_MPQP_H__
	/* MPQP relative encapsulations */
	aosl_mpq_t queue (aosl_mpqp_t qp, aosl_mpq_t dq, const char *f_name, aosl_mpq_func_argv_t f, uintptr_t argc, ...)
	{
		va_list args;
		aosl_mpq_t qid;

		va_start (args, argc);
		qid = refoop->queue_args (qp, dq, f_name, f, argc, args);
		va_end (args);

		return qid;
	}

	aosl_mpq_t queue_args (aosl_mpqp_t qp, aosl_mpq_t dq, const char *f_name, aosl_mpq_func_argv_t f, uintptr_t argc, va_list args)
	{
		return refoop->queue_args (qp, dq, f_name, f, argc, args);
	}

	aosl_mpq_t queue_argv (aosl_mpqp_t qp, aosl_mpq_t dq, const char *f_name, aosl_mpq_func_argv_t f, uintptr_t argc, uintptr_t *argv)
	{
		return refoop->queue_argv (qp, dq, f_name, f, argc, argv);
	}

	aosl_mpq_t queue_data (aosl_mpqp_t qp, aosl_mpq_t dq, const char *f_name, aosl_mpq_func_data_t f, size_t len, void *data)
	{
		return refoop->queue_data (qp, dq, f_name, f, len, data);
	}

	aosl_mpq_t call (aosl_mpqp_t qp, const char *f_name, aosl_mpq_func_argv_t f, uintptr_t argc, ...)
	{
		va_list args;
		aosl_mpq_t qid;

		va_start (args, argc);
		qid = refoop->call_args (qp, f_name, f, argc, args);
		va_end (args);

		return qid;
	}

	aosl_mpq_t call_args (aosl_mpqp_t qp, const char *f_name, aosl_mpq_func_argv_t f, uintptr_t argc, va_list args)
	{
		return refoop->call_args (qp, f_name, f, argc, args);
	}

	aosl_mpq_t call_argv (aosl_mpqp_t qp, const char *f_name, aosl_mpq_func_argv_t f, uintptr_t argc, uintptr_t *argv)
	{
		return refoop->call_argv (qp, f_name, f, argc, argv);
	}

	aosl_mpq_t call_data (aosl_mpqp_t qp, const char *f_name, aosl_mpq_func_data_t f, size_t len, void *data)
	{
		return refoop->call_data (qp, f_name, f, len, data);
	}

	aosl_mpq_t run (aosl_mpqp_t qp, aosl_mpq_t dq, const char *f_name, aosl_mpq_func_argv_t f, uintptr_t argc, ...)
	{
		va_list args;
		aosl_mpq_t qid;

		va_start (args, argc);
		qid = refoop->run_args (qp, dq, f_name, f, argc, args);
		va_end (args);

		return qid;
	}

	aosl_mpq_t run_args (aosl_mpqp_t qp, aosl_mpq_t dq, const char *f_name, aosl_mpq_func_argv_t f, uintptr_t argc, va_list args)
	{
		return refoop->run_args (qp, dq, f_name, f, argc, args);
	}

	aosl_mpq_t run_argv (aosl_mpqp_t qp, aosl_mpq_t dq, const char *f_name, aosl_mpq_func_argv_t f, uintptr_t argc, uintptr_t *argv)
	{
		return refoop->run_argv (qp, dq, f_name, f, argc, argv);
	}

	aosl_mpq_t run_data (aosl_mpqp_t qp, aosl_mpq_t dq, const char *f_name, aosl_mpq_func_data_t f, size_t len, void *data)
	{
		return refoop->run_data (qp, dq, f_name, f, len, data);
	}

	int pool_tail_queue (aosl_mpqp_t qp, aosl_mpq_t dq, const char *f_name, aosl_mpq_func_argv_t f, uintptr_t argc, ...)
	{
		va_list args;
		int err;

		va_start (args, argc);
		err = refoop->pool_tail_queue_args (qp, dq, f_name, f, argc, args);
		va_end (args);

		return err;
	}

	int pool_tail_queue_args (aosl_mpqp_t qp, aosl_mpq_t dq, const char *f_name, aosl_mpq_func_argv_t f, uintptr_t argc, va_list args)
	{
		return refoop->pool_tail_queue_args (qp, dq, f_name, f, argc, args);
	}

	int pool_tail_queue_argv (aosl_mpqp_t qp, aosl_mpq_t dq, const char *f_name, aosl_mpq_func_argv_t f, uintptr_t argc, uintptr_t *argv)
	{
		return refoop->pool_tail_queue_argv (qp, dq, f_name, f, argc, argv);
	}
#endif /* __AOSL_MPQP_H__ */
#endif /* __AOSL_MPQ_H__ */

	/* C++11 lambda encapsulations */
#if (__cplusplus >= 201103) || defined (_MSC_VER)
public:
	int hold (aosl_ref_lambda_f &&lambda_f)
	{
		return refoop->hold (std::move (lambda_f));
	}

	int read (aosl_ref_lambda_f &&lambda_f)
	{
		return refoop->read (std::move (lambda_f));
	}

	int write (aosl_ref_lambda_f &&lambda_f)
	{
		return refoop->write (std::move (lambda_f));
	}

	int unsafe (aosl_ref_lambda_f &&lambda_f)
	{
		return refoop->unsafe (std::move (lambda_f));
	}

	int maystall (aosl_ref_lambda_f &&lambda_f)
	{
		return refoop->unsafe (std::move (lambda_f));
	}

	static int hold (aosl_ref_t ref, aosl_ref_lambda_f &&lambda_f)
	{
		return aosl_ref_t_oop::hold (ref, std::move (lambda_f));
	}

	static int read (aosl_ref_t ref, aosl_ref_lambda_f &&lambda_f)
	{
		return aosl_ref_t_oop::read (ref, std::move (lambda_f));
	}

	static int write (aosl_ref_t ref, aosl_ref_lambda_f &&lambda_f)
	{
		return aosl_ref_t_oop::write (ref, std::move (lambda_f));
	}

	static int unsafe (aosl_ref_t ref, aosl_ref_lambda_f &&lambda_f)
	{
		return aosl_ref_t_oop::unsafe (ref, std::move (lambda_f));
	}

	static int maystall (aosl_ref_t ref, aosl_ref_lambda_f &&lambda_f)
	{
		return aosl_ref_t_oop::unsafe (ref, std::move (lambda_f));
	}

	static int read (aosl_refobj_t robj, aosl_ref_lambda_f &&lambda_f)
	{
		return aosl_ref_t_oop::read (robj, std::move (lambda_f));
	}

	static int unsafe (aosl_refobj_t robj, aosl_ref_lambda_f &&lambda_f)
	{
		return aosl_ref_t_oop::unsafe (robj, std::move (lambda_f));
	}

	static int maystall (aosl_refobj_t robj, aosl_ref_lambda_f &&lambda_f)
	{
		return aosl_ref_t_oop::unsafe (robj, std::move (lambda_f));
	}

#ifdef __AOSL_MPQ_H__
public:
	typedef std::function <void (const aosl_ts_t &queued_ts, aosl_refobj_t robj)> aosl_ref_mpq_lambda_f;
	typedef std::function <void (void)> aosl_ref_mpq_lambda_0arg_f;

	/* MPQ encapsulations */
	int queue (aosl_mpq_t tq, aosl_mpq_t dq, const char *f_name, aosl_ref_mpq_lambda_f&& task)
	{
		return refoop->queue (tq, dq, f_name, std::move (task));
	}

	int call (aosl_mpq_t q, const char *f_name, aosl_ref_mpq_lambda_f&& task, void *task_result = NULL)
	{
		return refoop->call (q, f_name, std::move (task), task_result);
	}

	int run (aosl_mpq_t q, const char *f_name, aosl_ref_mpq_lambda_f&& task)
	{
		return refoop->run (q, f_name, std::move (task));
	}

	int queue (aosl_mpq_t tq, const char *f_name, aosl_ref_mpq_lambda_0arg_f&& task)
	{
		return refoop->queue (tq, f_name, std::move (task));
	}

	int call (aosl_mpq_t q, const char *f_name, aosl_ref_mpq_lambda_0arg_f&& task, void *task_result = NULL)
	{
		return refoop->call (q, f_name, std::move (task), task_result);
	}

	int run (aosl_mpq_t q, const char *f_name, aosl_ref_mpq_lambda_0arg_f&& task)
	{
		return refoop->run (q, f_name, std::move (task));
	}

#ifdef __AOSL_MPQP_H__
	/* MPQP encapsulations */
	aosl_mpq_t queue (aosl_mpqp_t qp, aosl_mpq_t dq, const char *f_name, aosl_ref_mpq_lambda_f&& task)
	{
		return refoop->queue (qp, dq, f_name, std::move (task));
	}

	aosl_mpq_t call (aosl_mpqp_t qp, const char *f_name, aosl_ref_mpq_lambda_f&& task, void *task_result = NULL)
	{
		return refoop->call (qp, f_name, std::move (task), task_result);
	}

	aosl_mpq_t run (aosl_mpqp_t qp, const char *f_name, aosl_ref_mpq_lambda_f&& task)
	{
		return refoop->run (qp, f_name, std::move (task));
	}

	int pool_tail_queue (aosl_mpqp_t qp, aosl_mpq_t dq, const char *f_name, aosl_ref_mpq_lambda_f&& task)
	{
		return refoop->pool_tail_queue (qp, dq, f_name, std::move (task));
	}

	aosl_mpq_t queue (aosl_mpqp_t qp, const char *f_name, aosl_ref_mpq_lambda_0arg_f&& task)
	{
		return refoop->queue (qp, f_name, std::move (task));
	}

	aosl_mpq_t call (aosl_mpqp_t qp, const char *f_name, aosl_ref_mpq_lambda_0arg_f&& task, void *task_result = NULL)
	{
		return refoop->call (qp, f_name, std::move (task), task_result);
	}

	aosl_mpq_t run (aosl_mpqp_t qp, const char *f_name, aosl_ref_mpq_lambda_0arg_f&& task)
	{
		return refoop->run (qp, f_name, std::move (task));
	}

	int pool_tail_queue (aosl_mpqp_t qp, const char *f_name, aosl_ref_mpq_lambda_0arg_f&& task)
	{
		return refoop->pool_tail_queue (qp, f_name, std::move (task));
	}
#endif /* __AOSL_MPQP_H__ */

	/* MPQ with specified ref encapsulations */
	static int queue (aosl_mpq_t tq, aosl_mpq_t dq, aosl_ref_t ref, const char *f_name, aosl_ref_mpq_lambda_f&& task)
	{
		return aosl_ref_t_oop::queue (tq, dq, ref, f_name, std::move (task));
	}

	static int call (aosl_mpq_t q, aosl_ref_t ref, const char *f_name, aosl_ref_mpq_lambda_f&& task, void *task_result = NULL)
	{
		return aosl_ref_t_oop::call (q, ref, f_name, std::move (task), task_result);
	}

	static int run (aosl_mpq_t q, aosl_ref_t ref, const char *f_name, aosl_ref_mpq_lambda_f&& task)
	{
		return aosl_ref_t_oop::run (q, ref, f_name, std::move (task));
	}

	static int queue (aosl_mpq_t tq, aosl_ref_t ref, const char *f_name, aosl_ref_mpq_lambda_0arg_f&& task)
	{
		return aosl_ref_t_oop::queue (tq, ref, f_name, std::move (task));
	}

	static int call (aosl_mpq_t q, aosl_ref_t ref, const char *f_name, aosl_ref_mpq_lambda_0arg_f&& task, void *task_result = NULL)
	{
		return aosl_ref_t_oop::call (q, ref, f_name, std::move (task), task_result);
	}

	static int run (aosl_mpq_t q, aosl_ref_t ref, const char *f_name, aosl_ref_mpq_lambda_0arg_f&& task)
	{
		return aosl_ref_t_oop::run (q, ref, f_name, std::move (task));
	}

#ifdef __AOSL_MPQP_H__
	/* MPQP with specified ref encapsulations */
	static aosl_mpq_t queue (aosl_mpqp_t qp, aosl_mpq_t dq, aosl_ref_t ref, const char *f_name, aosl_ref_mpq_lambda_f&& task)
	{
		return aosl_ref_t_oop::queue (qp, dq, ref, f_name, std::move (task));
	}

	static aosl_mpq_t call (aosl_mpqp_t qp, aosl_ref_t ref, const char *f_name, aosl_ref_mpq_lambda_f&& task, void *task_result = NULL)
	{
		return aosl_ref_t_oop::call (qp, ref, f_name, std::move (task), task_result);
	}

	static aosl_mpq_t run (aosl_mpqp_t qp, aosl_ref_t ref, const char *f_name, aosl_ref_mpq_lambda_f&& task)
	{
		return aosl_ref_t_oop::run (qp, ref, f_name, std::move (task));
	}

	static int pool_tail_queue (aosl_mpqp_t qp, aosl_mpq_t dq, aosl_ref_t ref, const char *f_name, aosl_ref_mpq_lambda_f&& task)
	{
		return aosl_ref_t_oop::pool_tail_queue (qp, dq, ref, f_name, std::move (task));
	}

	static aosl_mpq_t queue (aosl_mpqp_t qp, aosl_ref_t ref, const char *f_name, aosl_ref_mpq_lambda_0arg_f&& task)
	{
		return aosl_ref_t_oop::queue (qp, ref, f_name, std::move (task));
	}

	static aosl_mpq_t call (aosl_mpqp_t qp, aosl_ref_t ref, const char *f_name, aosl_ref_mpq_lambda_0arg_f&& task, void *task_result = NULL)
	{
		return aosl_ref_t_oop::call (qp, ref, f_name, std::move (task), task_result);
	}

	static aosl_mpq_t run (aosl_mpqp_t qp, aosl_ref_t ref, const char *f_name, aosl_ref_mpq_lambda_0arg_f&& task)
	{
		return aosl_ref_t_oop::run (qp, ref, f_name, std::move (task));
	}

	static int pool_tail_queue (aosl_mpqp_t qp, aosl_ref_t ref, const char *f_name, aosl_ref_mpq_lambda_0arg_f&& task)
	{
		return aosl_ref_t_oop::pool_tail_queue (qp, ref, f_name, std::move (task));
	}
#endif /* __AOSL_MPQP_H__ */
#endif /* __AOSL_MPQ_H__ */

#ifdef __AOSL_ASYNC_H__
	/**
	 * The stackless coroutine like implementation in AOSL. We could not
	 * support the real stackless coroutine except in the language level,
	 * so we just provide similar equivalent functionals here.
	 **/
public:
	int prepare (aosl_stack_id_t stack_id, const char *f_name, aosl_async_prepare_lambda_f&& task)
	{
		return refoop->prepare (stack_id, f_name, std::move (task));
	}

	static int prepare (aosl_stack_id_t stack_id, aosl_ref_t ref, const char *f_name, aosl_async_prepare_lambda_f&& task)
	{
		return aosl_ref_t_oop::prepare (stack_id, ref, f_name, std::move (task));
	}

public:
	typedef std::function <void (int free_only)> aosl_async_resume_lambda_f;

	int resume (aosl_stack_id_t stack_id, const char *f_name, aosl_async_resume_lambda_f&& task)
	{
		return refoop->resume (stack_id, f_name, std::move (task));
	}

	static int resume (aosl_stack_id_t stack_id, aosl_ref_t ref, const char *f_name, aosl_async_resume_lambda_f&& task)
	{
		return aosl_ref_t_oop::resume (stack_id, ref, f_name, std::move (task));
	}
#endif /* __AOSL_ASYNC_H__ */
#endif /* C++11 */

#if (__cplusplus >= 201103) || defined (_MSC_VER)
private:
	aosl_ref_class (const aosl_ref_class &) = delete;
	aosl_ref_class (aosl_ref_class &&) = delete;
	aosl_ref_class &operator = (const aosl_ref_class &) = delete;
	aosl_ref_class &operator = (aosl_ref_class &&) = delete;
#else
private:
	aosl_ref_class (const aosl_ref_class &);
	aosl_ref_class &operator = (const aosl_ref_class &);
#endif /* C++11 */
};


/**
 * The T_ref_cls argument of this template must be
 * aosl_ref_class or its derivatives.
 **/
template<typename T_ref_cls>
class aosl_ref_unique_ptr {
private:
	T_ref_cls *_ptr;

public:
	aosl_ref_unique_ptr (): _ptr (NULL) {}
	aosl_ref_unique_ptr (T_ref_cls *p): _ptr (p) {}

	aosl_ref_unique_ptr &operator = (T_ref_cls *p)
	{
		reset ();
		_ptr = p;
		return *this;
	}

	T_ref_cls *operator -> () const
	{
		return _ptr;
	}

	T_ref_cls *get () const
	{
		return _ptr;
	}

	operator bool () const
	{
		return _ptr != NULL;
	}

	T_ref_cls *release ()
	{
		T_ref_cls *p = _ptr;
		_ptr = NULL;
		return p;
	}

	void reset (T_ref_cls *p = NULL)
	{
		T_ref_cls *old = _ptr;

		/**
		 * We do the destroy and not delete the object
		 * before we set the pointer to the new value,
		 * this is very important to make sure that no
		 * any async operation is executing.
		 **/
		if (old != NULL)
			old->destroy (false/* not delete */);

		_ptr = p;

		/**
		 * The destroy with delete operation must be
		 * the last action, and don't touch any member
		 * of this object anymore after it.
		 **/
		if (old != NULL)
			old->destroy (true/* do delete */);
	}

	~aosl_ref_unique_ptr ()
	{
		reset ();
	}

#if (__cplusplus >= 201103) || defined (_MSC_VER)
private:
	aosl_ref_unique_ptr (const aosl_ref_unique_ptr &) = delete;
	aosl_ref_unique_ptr &operator = (const aosl_ref_unique_ptr &) = delete;

public:
	aosl_ref_unique_ptr (aosl_ref_unique_ptr &&src): _ptr (src.release ()) {}
	aosl_ref_unique_ptr &operator = (aosl_ref_unique_ptr &&ptr)
	{
		reset (ptr.release ());
		return *this;
	}
#else
private:
	aosl_ref_unique_ptr (const aosl_ref_unique_ptr &);
	aosl_ref_unique_ptr &operator = (const aosl_ref_unique_ptr &);
#endif /* C++11 */
};


template<typename T_ref_cls>
inline bool operator == (const aosl_ref_unique_ptr<T_ref_cls> &ptr, intptr_t _null)
{
	return ptr.get () == (T_ref_cls *)_null;
}

template<typename T_ref_cls>
inline bool operator != (const aosl_ref_unique_ptr<T_ref_cls> &ptr, intptr_t _null)
{
	return ptr.get () != (T_ref_cls *)_null;
}

template<typename T_ref_cls>
inline bool operator == (intptr_t _null, const aosl_ref_unique_ptr<T_ref_cls> &ptr)
{
	return (T_ref_cls *)_null == ptr.get ();
}

template<typename T_ref_cls>
inline bool operator != (intptr_t _null, const aosl_ref_unique_ptr<T_ref_cls> &ptr)
{
	return  (T_ref_cls *)_null != ptr.get ();
}

#if (__cplusplus >= 201103) || defined (_MSC_VER)
template<typename T_ref_cls>
inline bool operator == (const aosl_ref_unique_ptr<T_ref_cls> &ptr, std::nullptr_t)
{
	return !ptr;
}

template<typename T_ref_cls>
inline bool operator != (const aosl_ref_unique_ptr<T_ref_cls> &ptr, std::nullptr_t)
{
	return ptr;
}

template<typename T_ref_cls>
inline bool operator == (std::nullptr_t, const aosl_ref_unique_ptr<T_ref_cls> &ptr)
{
	return !ptr;
}

template<typename T_ref_cls>
inline bool operator != (std::nullptr_t, const aosl_ref_unique_ptr<T_ref_cls> &ptr)
{
	return ptr;
}
#endif /* C++11 */


typedef aosl_ref_unique_ptr<aosl_ref_class> aosl_ref_class_unique_ptr;


#endif /* __AOSL_REF_OBJ_CPP_H__ */