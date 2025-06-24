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

#if (__cplusplus >= 201103) || (defined (_MSC_VER) && _MSC_VER >= 1800)
#include <memory>
#include <type_traits>
#include <utility>
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
		static aosl_ref_t_oop *create (void *arg = NULL, aosl_ref_dtor_t dtor = NULL, bool destroy_wait = true)
		{
			return (aosl_ref_t_oop *)aosl_ref_create (arg, dtor, (int)destroy_wait);
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

		aosl_ref_magic_t magic () const
		{
			aosl_ref_magic_t m;
			if (aosl_ref_magic (ref (), &m) < 0)
				return AOSL_REF_MAGIC_INVALID;

			return m;
		}

		int hold (aosl_ref_magic_t magic, aosl_ref_func_t f, uintptr_t argc, ...)
		{
			va_list args;
			int err;

			va_start (args, argc);
			err = aosl_ref_magic_hold_args (ref (), magic, f, argc, args);
			va_end (args);

			return err;
		}

		int hold_args (aosl_ref_magic_t magic, aosl_ref_func_t f, uintptr_t argc, va_list args)
		{
			return aosl_ref_magic_hold_args (ref (), magic, f, argc, args);
		}

		int hold_argv (aosl_ref_magic_t magic, aosl_ref_func_t f, uintptr_t argc, uintptr_t argv [])
		{
			return aosl_ref_magic_hold_argv (ref (), magic, f, argc, argv);
		}

		int read (aosl_ref_magic_t magic, aosl_ref_func_t f, uintptr_t argc, ...)
		{
			va_list args;
			int err;

			va_start (args, argc);
			err = aosl_ref_magic_read_args (ref (), magic, f, argc, args);
			va_end (args);

			return err;
		}

		int read_args (aosl_ref_magic_t magic, aosl_ref_func_t f, uintptr_t argc, va_list args)
		{
			return aosl_ref_magic_read_args (ref (), magic, f, argc, args);
		}

		int read_argv (aosl_ref_magic_t magic, aosl_ref_func_t f, uintptr_t argc, uintptr_t argv [])
		{
			return aosl_ref_magic_read_argv (ref (), magic, f, argc, argv);
		}

		int write (aosl_ref_magic_t magic, aosl_ref_func_t f, uintptr_t argc, ...)
		{
			va_list args;
			int err;

			va_start (args, argc);
			err = aosl_ref_magic_write_args (ref (), magic, f, argc, args);
			va_end (args);

			return err;
		}

		int write_args (aosl_ref_magic_t magic, aosl_ref_func_t f, uintptr_t argc, va_list args)
		{
			return aosl_ref_magic_write_args (ref (), magic, f, argc, args);
		}

		int write_argv (aosl_ref_magic_t magic, aosl_ref_func_t f, uintptr_t argc, uintptr_t argv [])
		{
			return aosl_ref_magic_write_argv (ref (), magic, f, argc, argv);
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
		static int hold (aosl_ref_t ref, aosl_ref_magic_t magic, aosl_ref_func_t f, uintptr_t argc, ...)
		{
			va_list args;
			int err;

			va_start (args, argc);
			err = aosl_ref_magic_hold_args (ref, magic, f, argc, args);
			va_end (args);

			return err;
		}

		static int hold_args (aosl_ref_t ref, aosl_ref_magic_t magic, aosl_ref_func_t f, uintptr_t argc, va_list args)
		{
			return aosl_ref_magic_hold_args (ref, magic, f, argc, args);
		}

		static int hold_argv (aosl_ref_t ref, aosl_ref_magic_t magic, aosl_ref_func_t f, uintptr_t argc, uintptr_t argv [])
		{
			return aosl_ref_magic_hold_argv (ref, magic, f, argc, argv);
		}

		static int read (aosl_ref_t ref, aosl_ref_magic_t magic, aosl_ref_func_t f, uintptr_t argc, ...)
		{
			va_list args;
			int err;

			va_start (args, argc);
			err = aosl_ref_magic_read_args (ref, magic, f, argc, args);
			va_end (args);

			return err;
		}

		static int read_args (aosl_ref_t ref, aosl_ref_magic_t magic, aosl_ref_func_t f, uintptr_t argc, va_list args)
		{
			return aosl_ref_magic_read_args (ref, magic, f, argc, args);
		}

		static int read_argv (aosl_ref_t ref, aosl_ref_magic_t magic, aosl_ref_func_t f, uintptr_t argc, uintptr_t argv [])
		{
			return aosl_ref_magic_read_argv (ref, magic, f, argc, argv);
		}

		static int write (aosl_ref_t ref, aosl_ref_magic_t magic, aosl_ref_func_t f, uintptr_t argc, ...)
		{
			va_list args;
			int err;

			va_start (args, argc);
			err = aosl_ref_magic_write_args (ref, magic, f, argc, args);
			va_end (args);

			return err;
		}

		static int write_args (aosl_ref_t ref, aosl_ref_magic_t magic, aosl_ref_func_t f, uintptr_t argc, va_list args)
		{
			return aosl_ref_magic_write_args (ref, magic, f, argc, args);
		}

		static int write_argv (aosl_ref_t ref, aosl_ref_magic_t magic, aosl_ref_func_t f, uintptr_t argc, uintptr_t argv [])
		{
			return aosl_ref_magic_write_argv (ref, magic, f, argc, argv);
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

	#if (__cplusplus >= 201103) || (defined (_MSC_VER) && _MSC_VER >= 1800)
		/* __ref_destroy_exec_lambda_t: void (int err) */
		template <typename __ref_destroy_exec_lambda_t,
			typename std::enable_if<std::is_void<decltype(std::declval<__ref_destroy_exec_lambda_t>()(std::declval<int>()))>::value, int>::type = 0>
		int destroy_exec (__ref_destroy_exec_lambda_t &&lambda_f, aosl_ref_t ares = AOSL_REF_INVALID)
		{
			__ref_destroy_exec_lambda_t *task_obj = new __ref_destroy_exec_lambda_t (std::move (lambda_f));
			int err = aosl_ref_destroy_exec (ref (), ares, ____ref_destroy_exec_f<typename std::remove_reference<__ref_destroy_exec_lambda_t>::type>, 1, task_obj);
			if (err < 0)
				delete task_obj;

			return err;
		}
	private:
		/* __ref_destroy_exec_lambda_t: void (int err) */
		template <typename __ref_destroy_exec_lambda_t,
			typename std::enable_if<std::is_void<decltype(std::declval<__ref_destroy_exec_lambda_t>()(std::declval<int>()))>::value, int>::type = 0>
		static void ____ref_destroy_exec_f (int err, uintptr_t argc, uintptr_t argv [])
		{
			__ref_destroy_exec_lambda_t *task_obj = reinterpret_cast<__ref_destroy_exec_lambda_t *>(argv [0]);
			(*task_obj) (err);
			delete task_obj;
		}
	#endif

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

		int exec (aosl_mpq_t q, const char *f_name, aosl_mpq_func_argv_t f, uintptr_t argc, ...)
		{
			va_list args;
			int err;

			va_start (args, argc);
			err = aosl_mpq_exec_args (q, ref (), f_name, f, argc, args);
			va_end (args);

			return err;
		}

		int exec_args (aosl_mpq_t q, const char *f_name, aosl_mpq_func_argv_t f, uintptr_t argc, va_list args)
		{
			return aosl_mpq_exec_args (q, ref (), f_name, f, argc, args);
		}

		int exec_argv (aosl_mpq_t q, const char *f_name, aosl_mpq_func_argv_t f, uintptr_t argc, uintptr_t *argv)
		{
			return aosl_mpq_exec_argv (q, ref (), f_name, f, argc, argv);
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
	#endif /* __AOSL_MPQP_H__ */
	#endif /* __AOSL_MPQ_H__ */

		/* C++11 lambda encapsulations */
	#if (__cplusplus >= 201103) || (defined (_MSC_VER) && _MSC_VER >= 1800)
	public:
		/* __local_lambda_t: void (void *arg) */
		template <typename __local_lambda_t,
			typename std::enable_if<std::is_void<decltype(std::declval<__local_lambda_t>()(std::declval<void *>()))>::value, int>::type = 0>
		int hold (__local_lambda_t &&lambda_f, aosl_ref_magic_t magic = AOSL_REF_MAGIC_INVALID)
		{
			return aosl_ref_t_oop::hold (magic, ____ref_f<typename std::remove_reference<__local_lambda_t>::type>, 1, &lambda_f);
		}

		/* __local_lambda_t: void (void *arg) */
		template <typename __local_lambda_t,
			typename std::enable_if<std::is_void<decltype(std::declval<__local_lambda_t>()(std::declval<void *>()))>::value, int>::type = 0>
		int read (__local_lambda_t &&lambda_f, aosl_ref_magic_t magic = AOSL_REF_MAGIC_INVALID)
		{
			return aosl_ref_t_oop::read (magic, ____ref_f<typename std::remove_reference<__local_lambda_t>::type>, 1, &lambda_f);
		}

		/* __local_lambda_t: void (void *arg) */
		template <typename __local_lambda_t,
			typename std::enable_if<std::is_void<decltype(std::declval<__local_lambda_t>()(std::declval<void *>()))>::value, int>::type = 0>
		int write (__local_lambda_t &&lambda_f, aosl_ref_magic_t magic = AOSL_REF_MAGIC_INVALID)
		{
			return aosl_ref_t_oop::write (magic, ____ref_f<typename std::remove_reference<__local_lambda_t>::type>, 1, &lambda_f);
		}

		/* __local_lambda_t: void (void *arg) */
		template <typename __local_lambda_t,
			typename std::enable_if<std::is_void<decltype(std::declval<__local_lambda_t>()(std::declval<void *>()))>::value, int>::type = 0>
		int unsafe (__local_lambda_t &&lambda_f)
		{
			return aosl_ref_t_oop::unsafe (____ref_f<typename std::remove_reference<__local_lambda_t>::type>, 1, &lambda_f);
		}

		/* __local_lambda_t: void (void *arg) */
		template <typename __local_lambda_t,
			typename std::enable_if<std::is_void<decltype(std::declval<__local_lambda_t>()(std::declval<void *>()))>::value, int>::type = 0>
		static int hold (aosl_ref_t ref, __local_lambda_t &&lambda_f, aosl_ref_magic_t magic = AOSL_REF_MAGIC_INVALID)
		{
			return aosl_ref_t_oop::hold (ref, magic, ____ref_f<typename std::remove_reference<__local_lambda_t>::type>, 1, &lambda_f);
		}

		/* __local_lambda_t: void (void *arg) */
		template <typename __local_lambda_t,
			typename std::enable_if<std::is_void<decltype(std::declval<__local_lambda_t>()(std::declval<void *>()))>::value, int>::type = 0>
		static int read (aosl_ref_t ref, __local_lambda_t &&lambda_f, aosl_ref_magic_t magic = AOSL_REF_MAGIC_INVALID)
		{
			return aosl_ref_t_oop::read (ref, magic, ____ref_f<typename std::remove_reference<__local_lambda_t>::type>, 1, &lambda_f);
		}

		/* __local_lambda_t: void (void *arg) */
		template <typename __local_lambda_t,
			typename std::enable_if<std::is_void<decltype(std::declval<__local_lambda_t>()(std::declval<void *>()))>::value, int>::type = 0>
		static int write (aosl_ref_t ref, __local_lambda_t &&lambda_f, aosl_ref_magic_t magic = AOSL_REF_MAGIC_INVALID)
		{
			return aosl_ref_t_oop::write (ref, magic, ____ref_f<typename std::remove_reference<__local_lambda_t>::type>, 1, &lambda_f);
		}

		/* __local_lambda_t: void (void *arg) */
		template <typename __local_lambda_t,
			typename std::enable_if<std::is_void<decltype(std::declval<__local_lambda_t>()(std::declval<void *>()))>::value, int>::type = 0>
		static int unsafe (aosl_ref_t ref, __local_lambda_t &&lambda_f)
		{
			return aosl_ref_t_oop::unsafe (ref, ____ref_f<typename std::remove_reference<__local_lambda_t>::type>, 1, &lambda_f);
		}

		/* __local_lambda_t: void (void *arg) */
		template <typename __local_lambda_t,
			typename std::enable_if<std::is_void<decltype(std::declval<__local_lambda_t>()(std::declval<void *>()))>::value, int>::type = 0>
		static int read (aosl_refobj_t robj, __local_lambda_t &&lambda_f)
		{
			return aosl_ref_t_oop::read (robj, ____ref_f<typename std::remove_reference<__local_lambda_t>::type>, 1, &lambda_f);
		}

		/* __local_lambda_t: void (void *arg) */
		template <typename __local_lambda_t,
			typename std::enable_if<std::is_void<decltype(std::declval<__local_lambda_t>()(std::declval<void *>()))>::value, int>::type = 0>
		static int unsafe (aosl_refobj_t robj, __local_lambda_t &&lambda_f)
		{
			return aosl_ref_t_oop::unsafe (robj, ____ref_f<typename std::remove_reference<__local_lambda_t>::type>, 1, &lambda_f);
		}

	private:
		/* __local_lambda_t: void (void *arg) */
		template <typename __local_lambda_t,
			typename std::enable_if<std::is_void<decltype(std::declval<__local_lambda_t>()(std::declval<void *>()))>::value, int>::type = 0>
		static void ____ref_f (void *arg, uintptr_t argc, uintptr_t argv [])
		{
			(*(__local_lambda_t *)argv [0]) (arg);
		}

	#ifdef __AOSL_MPQ_H__
	public:
		/* MPQ encapsulations */
		/* __mpq_lambda_t: void (const aosl_ts_t &queued_ts, aosl_refobj_t robj) */
		template <typename __mpq_lambda_t,
				typename std::enable_if<std::is_void<decltype(std::declval<__mpq_lambda_t>()(
					std::declval<const aosl_ts_t &>(),
					std::declval<aosl_refobj_t>()
				))>::value, int>::type = 0>
		int queue (aosl_mpq_t tq, aosl_mpq_t dq, const char *f_name, __mpq_lambda_t&& task)
		{
			__mpq_lambda_t *task_obj = new __mpq_lambda_t (std::move (task));
			int err = aosl_ref_t_oop::queue (tq, dq, f_name, ____mpq_f<typename std::remove_reference<__mpq_lambda_t>::type>, 1, task_obj);
			if (err < 0)
				delete task_obj;

			return err;
		}

		/* __mpq_lambda_t: void (const aosl_ts_t &queued_ts, aosl_refobj_t robj) */
		template <typename __mpq_lambda_t,
				typename std::enable_if<std::is_void<decltype(std::declval<__mpq_lambda_t>()(
					std::declval<const aosl_ts_t &>(),
					std::declval<aosl_refobj_t>()
				))>::value, int>::type = 0>
		int call (aosl_mpq_t q, const char *f_name, __mpq_lambda_t&& task, void *task_result = NULL)
		{
			__mpq_lambda_t *task_obj = new __mpq_lambda_t (std::move (task));
			int err = aosl_ref_t_oop::call (q, f_name, ____mpq_f<typename std::remove_reference<__mpq_lambda_t>::type>, 2, task_obj, task_result);
			if (err < 0)
				delete task_obj;

			return err;
		}

		/* __mpq_lambda_t: void (const aosl_ts_t &queued_ts, aosl_refobj_t robj) */
		template <typename __mpq_lambda_t,
				typename std::enable_if<std::is_void<decltype(std::declval<__mpq_lambda_t>()(
					std::declval<const aosl_ts_t &>(),
					std::declval<aosl_refobj_t>()
				))>::value, int>::type = 0>
		int run (aosl_mpq_t q, const char *f_name, __mpq_lambda_t&& task)
		{
			__mpq_lambda_t *task_obj = new __mpq_lambda_t (std::move (task));
			int err = aosl_ref_t_oop::run (q, AOSL_MPQ_INVALID, f_name, ____mpq_f<typename std::remove_reference<__mpq_lambda_t>::type>, 1, task_obj);
			if (err < 0)
				delete task_obj;

			return err;
		}

		/* __mpq_lambda_t: void (const aosl_ts_t &queued_ts, aosl_refobj_t robj) */
		template <typename __mpq_lambda_t,
				typename std::enable_if<std::is_void<decltype(std::declval<__mpq_lambda_t>()(
					std::declval<const aosl_ts_t &>(),
					std::declval<aosl_refobj_t>()
				))>::value, int>::type = 0>
		int exec (aosl_mpq_t q, const char *f_name, __mpq_lambda_t&& task)
		{
			__mpq_lambda_t *task_obj = new __mpq_lambda_t (std::move (task));
			int err = aosl_ref_t_oop::exec (q, f_name, ____mpq_exec_f<typename std::remove_reference<__mpq_lambda_t>::type>, 1, task_obj);
			if (err < 0)
				delete task_obj;

			return err;
		}

		/* __mpq_0arg_lambda_t: void (void) */
		template <typename __mpq_0arg_lambda_t>
		/**
		 * Do not use the template parameter with default value style SFINAE for 0 argument lambda case,
		 * because the buggy MSVC compiler version 14.25.28610 will report:
		 *  - error C2672: XXX: no matching overloaded function found
		 *  - error C2783: XXX(YYY): could not deduce template argument for '__formal'
		 * So, we use the return type style SFINAE here instead.
		 * -- Lionfore Hao Apr 15th, 2025
		 **/
		typename std::enable_if<std::is_void<decltype(std::declval<__mpq_0arg_lambda_t>()())>::value, int>::type
		queue (aosl_mpq_t tq, const char *f_name, __mpq_0arg_lambda_t&& task)
		{
			__mpq_0arg_lambda_t *task_obj = new __mpq_0arg_lambda_t (std::move (task));
			int err = aosl_ref_t_oop::queue (tq, AOSL_MPQ_INVALID, f_name, ____mpq_0arg_f<typename std::remove_reference<__mpq_0arg_lambda_t>::type>, 1, task_obj);
			if (err < 0)
				delete task_obj;

			return err;
		}

		/* __mpq_0arg_lambda_t: void (void) */
		template <typename __mpq_0arg_lambda_t>
		/**
		 * Do not use the template parameter with default value style SFINAE for 0 argument lambda case,
		 * because the buggy MSVC compiler version 14.25.28610 will report:
		 *  - error C2672: XXX: no matching overloaded function found
		 *  - error C2783: XXX(YYY): could not deduce template argument for '__formal'
		 * So, we use the return type style SFINAE here instead.
		 * -- Lionfore Hao Apr 15th, 2025
		 **/
		typename std::enable_if<std::is_void<decltype(std::declval<__mpq_0arg_lambda_t>()())>::value, int>::type
		call (aosl_mpq_t q, const char *f_name, __mpq_0arg_lambda_t&& task, void *task_result = NULL)
		{
			__mpq_0arg_lambda_t *task_obj = new __mpq_0arg_lambda_t (std::move (task));
			int err = aosl_ref_t_oop::call (q, f_name, ____mpq_0arg_f<typename std::remove_reference<__mpq_0arg_lambda_t>::type>, 2, task_obj, task_result);
			if (err < 0)
				delete task_obj;

			return err;
		}

		/* __mpq_0arg_lambda_t: void (void) */
		template <typename __mpq_0arg_lambda_t>
		/**
		 * Do not use the template parameter with default value style SFINAE for 0 argument lambda case,
		 * because the buggy MSVC compiler version 14.25.28610 will report:
		 *  - error C2672: XXX: no matching overloaded function found
		 *  - error C2783: XXX(YYY): could not deduce template argument for '__formal'
		 * So, we use the return type style SFINAE here instead.
		 * -- Lionfore Hao Apr 15th, 2025
		 **/
		typename std::enable_if<std::is_void<decltype(std::declval<__mpq_0arg_lambda_t>()())>::value, int>::type
		run (aosl_mpq_t q, const char *f_name, __mpq_0arg_lambda_t&& task)
		{
			__mpq_0arg_lambda_t *task_obj = new __mpq_0arg_lambda_t (std::move (task));
			int err = aosl_ref_t_oop::run (q, AOSL_MPQ_INVALID, f_name, ____mpq_0arg_f<typename std::remove_reference<__mpq_0arg_lambda_t>::type>, 1, task_obj);
			if (err < 0)
				delete task_obj;

			return err;
		}

		/* __mpq_0arg_lambda_t: void (void) */
		template <typename __mpq_0arg_lambda_t>
		/**
		 * Do not use the template parameter with default value style SFINAE for 0 argument lambda case,
		 * because the buggy MSVC compiler version 14.25.28610 will report:
		 *  - error C2672: XXX: no matching overloaded function found
		 *  - error C2783: XXX(YYY): could not deduce template argument for '__formal'
		 * So, we use the return type style SFINAE here instead.
		 * -- Lionfore Hao Apr 15th, 2025
		 **/
		typename std::enable_if<std::is_void<decltype(std::declval<__mpq_0arg_lambda_t>()())>::value, int>::type
		exec (aosl_mpq_t q, const char *f_name, __mpq_0arg_lambda_t&& task)
		{
			__mpq_0arg_lambda_t *task_obj = new __mpq_0arg_lambda_t (std::move (task));
			int err = aosl_ref_t_oop::exec (q, f_name, ____mpq_exec_0arg_f<typename std::remove_reference<__mpq_0arg_lambda_t>::type>, 1, task_obj);
			if (err < 0)
				delete task_obj;

			return err;
		}

	#ifdef __AOSL_MPQP_H__
		/* MPQP encapsulations */
		/* __mpq_lambda_t: void (const aosl_ts_t &queued_ts, aosl_refobj_t robj) */
		template <typename __mpq_lambda_t,
				typename std::enable_if<std::is_void<decltype(std::declval<__mpq_lambda_t>()(
					std::declval<const aosl_ts_t &>(),
					std::declval<aosl_refobj_t>()
				))>::value, int>::type = 0>
		aosl_mpq_t queue (aosl_mpqp_t qp, aosl_mpq_t dq, const char *f_name, __mpq_lambda_t&& task)
		{
			__mpq_lambda_t *task_obj = new __mpq_lambda_t (std::move (task));
			aosl_mpq_t qid = aosl_ref_t_oop::queue (qp, dq, f_name, ____mpq_f<typename std::remove_reference<__mpq_lambda_t>::type>, 1, task_obj);
			if (aosl_mpq_invalid (qid))
				delete task_obj;

			return qid;
		}

		/* __mpq_lambda_t: void (const aosl_ts_t &queued_ts, aosl_refobj_t robj) */
		template <typename __mpq_lambda_t,
				typename std::enable_if<std::is_void<decltype(std::declval<__mpq_lambda_t>()(
					std::declval<const aosl_ts_t &>(),
					std::declval<aosl_refobj_t>()
				))>::value, int>::type = 0>
		aosl_mpq_t call (aosl_mpqp_t qp, const char *f_name, __mpq_lambda_t&& task, void *task_result = NULL)
		{
			__mpq_lambda_t *task_obj = new __mpq_lambda_t (std::move (task));
			aosl_mpq_t qid = aosl_ref_t_oop::call (qp, f_name, ____mpq_f<typename std::remove_reference<__mpq_lambda_t>::type>, 2, task_obj, task_result);
			if (aosl_mpq_invalid (qid))
				delete task_obj;

			return qid;
		}

		/* __mpq_lambda_t: void (const aosl_ts_t &queued_ts, aosl_refobj_t robj) */
		template <typename __mpq_lambda_t,
				typename std::enable_if<std::is_void<decltype(std::declval<__mpq_lambda_t>()(
					std::declval<const aosl_ts_t &>(),
					std::declval<aosl_refobj_t>()
				))>::value, int>::type = 0>
		aosl_mpq_t run (aosl_mpqp_t qp, const char *f_name, __mpq_lambda_t&& task)
		{
			__mpq_lambda_t *task_obj = new __mpq_lambda_t (std::move (task));
			aosl_mpq_t qid = aosl_ref_t_oop::run (qp, AOSL_MPQ_INVALID, f_name, ____mpq_f<typename std::remove_reference<__mpq_lambda_t>::type>, 1, task_obj);
			if (aosl_mpq_invalid (qid))
				delete task_obj;

			return qid;
		}

		/* __mpq_0arg_lambda_t: void (void) */
		template <typename __mpq_0arg_lambda_t>
		typename std::enable_if<std::is_void<decltype(std::declval<__mpq_0arg_lambda_t>()())>::value, aosl_mpq_t>::type
		queue (aosl_mpqp_t qp, const char *f_name, __mpq_0arg_lambda_t&& task)
		{
			__mpq_0arg_lambda_t *task_obj = new __mpq_0arg_lambda_t (std::move (task));
			aosl_mpq_t qid = aosl_ref_t_oop::queue (qp, AOSL_MPQ_INVALID, f_name, ____mpq_0arg_f<typename std::remove_reference<__mpq_0arg_lambda_t>::type>, 1, task_obj);
			if (aosl_mpq_invalid (qid))
				delete task_obj;

			return qid;
		}

		/* __mpq_0arg_lambda_t: void (void) */
		template <typename __mpq_0arg_lambda_t>
		typename std::enable_if<std::is_void<decltype(std::declval<__mpq_0arg_lambda_t>()())>::value, aosl_mpq_t>::type
		call (aosl_mpqp_t qp, const char *f_name, __mpq_0arg_lambda_t&& task, void *task_result = NULL)
		{
			__mpq_0arg_lambda_t *task_obj = new __mpq_0arg_lambda_t (std::move (task));
			aosl_mpq_t qid = aosl_ref_t_oop::call (qp, f_name, ____mpq_0arg_f<typename std::remove_reference<__mpq_0arg_lambda_t>::type>, 2, task_obj, task_result);
			if (aosl_mpq_invalid (qid))
				delete task_obj;

			return qid;
		}

		/* __mpq_0arg_lambda_t: void (void) */
		template <typename __mpq_0arg_lambda_t>
		typename std::enable_if<std::is_void<decltype(std::declval<__mpq_0arg_lambda_t>()())>::value, aosl_mpq_t>::type
		run (aosl_mpqp_t qp, const char *f_name, __mpq_0arg_lambda_t&& task)
		{
			__mpq_0arg_lambda_t *task_obj = new __mpq_0arg_lambda_t (std::move (task));
			aosl_mpq_t qid = aosl_ref_t_oop::run (qp, AOSL_MPQ_INVALID, f_name, ____mpq_0arg_f<typename std::remove_reference<__mpq_0arg_lambda_t>::type>, 1, task_obj);
			if (aosl_mpq_invalid (qid))
				delete task_obj;

			return qid;
		}
	#endif /* __AOSL_MPQP_H__ */

		/* MPQ with specified ref encapsulations */
		/* __mpq_lambda_t: void (const aosl_ts_t &queued_ts, aosl_refobj_t robj) */
		template <typename __mpq_lambda_t,
				typename std::enable_if<std::is_void<decltype(std::declval<__mpq_lambda_t>()(
					std::declval<const aosl_ts_t &>(),
					std::declval<aosl_refobj_t>()
				))>::value, int>::type = 0>
		static int queue (aosl_mpq_t tq, aosl_mpq_t dq, aosl_ref_t ref, const char *f_name, __mpq_lambda_t&& task)
		{
			__mpq_lambda_t *task_obj = new __mpq_lambda_t (std::move (task));
			int err = aosl_mpq_queue (tq, dq, ref, f_name, ____mpq_f<typename std::remove_reference<__mpq_lambda_t>::type>, 1, task_obj);
			if (err < 0)
				delete task_obj;

			return err;
		}

		/* __mpq_lambda_t: void (const aosl_ts_t &queued_ts, aosl_refobj_t robj) */
		template <typename __mpq_lambda_t,
				typename std::enable_if<std::is_void<decltype(std::declval<__mpq_lambda_t>()(
					std::declval<const aosl_ts_t &>(),
					std::declval<aosl_refobj_t>()
				))>::value, int>::type = 0>
		static int call (aosl_mpq_t q, aosl_ref_t ref, const char *f_name, __mpq_lambda_t&& task, void *task_result = NULL)
		{
			__mpq_lambda_t *task_obj = new __mpq_lambda_t (std::move (task));
			int err = aosl_mpq_call (q, ref, f_name, ____mpq_f<typename std::remove_reference<__mpq_lambda_t>::type>, 2, task_obj, task_result);
			if (err < 0)
				delete task_obj;

			return err;
		}

		/* __mpq_lambda_t: void (const aosl_ts_t &queued_ts, aosl_refobj_t robj) */
		template <typename __mpq_lambda_t,
				typename std::enable_if<std::is_void<decltype(std::declval<__mpq_lambda_t>()(
					std::declval<const aosl_ts_t &>(),
					std::declval<aosl_refobj_t>()
				))>::value, int>::type = 0>
		static int run (aosl_mpq_t q, aosl_ref_t ref, const char *f_name, __mpq_lambda_t&& task)
		{
			__mpq_lambda_t *task_obj = new __mpq_lambda_t (std::move (task));
			int err = aosl_mpq_run (q, AOSL_MPQ_INVALID, ref, f_name, ____mpq_f<typename std::remove_reference<__mpq_lambda_t>::type>, 1, task_obj);
			if (err < 0)
				delete task_obj;

			return err;
		}

		/* __mpq_lambda_t: void (const aosl_ts_t &queued_ts, aosl_refobj_t robj) */
		template <typename __mpq_lambda_t,
				typename std::enable_if<std::is_void<decltype(std::declval<__mpq_lambda_t>()(
					std::declval<const aosl_ts_t &>(),
					std::declval<aosl_refobj_t>()
				))>::value, int>::type = 0>
		static int exec (aosl_mpq_t q, aosl_ref_t ref, const char *f_name, __mpq_lambda_t&& task)
		{
			__mpq_lambda_t *task_obj = new __mpq_lambda_t (std::move (task));
			int err = aosl_mpq_exec (q, ref, f_name, ____mpq_exec_f<typename std::remove_reference<__mpq_lambda_t>::type>, 1, task_obj);
			if (err < 0)
				delete task_obj;

			return err;
		}

		/* __mpq_0arg_lambda_t: void (void) */
		template <typename __mpq_0arg_lambda_t>
		static typename std::enable_if<std::is_void<decltype(std::declval<__mpq_0arg_lambda_t>()())>::value, int>::type
		queue (aosl_mpq_t tq, aosl_ref_t ref, const char *f_name, __mpq_0arg_lambda_t&& task)
		{
			__mpq_0arg_lambda_t *task_obj = new __mpq_0arg_lambda_t (std::move (task));
			int err = aosl_mpq_queue (tq, AOSL_MPQ_INVALID, ref, f_name, ____mpq_0arg_f<typename std::remove_reference<__mpq_0arg_lambda_t>::type>, 1, task_obj);
			if (err < 0)
				delete task_obj;

			return err;
		}

		/* __mpq_0arg_lambda_t: void (void) */
		template <typename __mpq_0arg_lambda_t>
		static typename std::enable_if<std::is_void<decltype(std::declval<__mpq_0arg_lambda_t>()())>::value, int>::type
		call (aosl_mpq_t q, aosl_ref_t ref, const char *f_name, __mpq_0arg_lambda_t&& task, void *task_result = NULL)
		{
			__mpq_0arg_lambda_t *task_obj = new __mpq_0arg_lambda_t (std::move (task));
			int err = aosl_mpq_call (q, ref, f_name, ____mpq_0arg_f<typename std::remove_reference<__mpq_0arg_lambda_t>::type>, 2, task_obj, task_result);
			if (err < 0)
				delete task_obj;

			return err;
		}

		/* __mpq_0arg_lambda_t: void (void) */
		template <typename __mpq_0arg_lambda_t>
		static typename std::enable_if<std::is_void<decltype(std::declval<__mpq_0arg_lambda_t>()())>::value, int>::type
		run (aosl_mpq_t q, aosl_ref_t ref, const char *f_name, __mpq_0arg_lambda_t&& task)
		{
			__mpq_0arg_lambda_t *task_obj = new __mpq_0arg_lambda_t (std::move (task));
			int err = aosl_mpq_run (q, AOSL_MPQ_INVALID, ref, f_name, ____mpq_0arg_f<typename std::remove_reference<__mpq_0arg_lambda_t>::type>, 1, task_obj);
			if (err < 0)
				delete task_obj;

			return err;
		}

		/* __mpq_0arg_lambda_t: void (void) */
		template <typename __mpq_0arg_lambda_t>
		static typename std::enable_if<std::is_void<decltype(std::declval<__mpq_0arg_lambda_t>()())>::value, int>::type
		exec (aosl_mpq_t q, aosl_ref_t ref, const char *f_name, __mpq_0arg_lambda_t&& task)
		{
			__mpq_0arg_lambda_t *task_obj = new __mpq_0arg_lambda_t (std::move (task));
			int err = aosl_mpq_exec (q, ref, f_name, ____mpq_exec_0arg_f<typename std::remove_reference<__mpq_0arg_lambda_t>::type>, 1, task_obj);
			if (err < 0)
				delete task_obj;

			return err;
		}

	#ifdef __AOSL_MPQP_H__
		/* MPQP with specified ref encapsulations */
		/* __mpq_lambda_t: void (const aosl_ts_t &queued_ts, aosl_refobj_t robj) */
		template <typename __mpq_lambda_t,
				typename std::enable_if<std::is_void<decltype(std::declval<__mpq_lambda_t>()(
					std::declval<const aosl_ts_t &>(),
					std::declval<aosl_refobj_t>()
				))>::value, int>::type = 0>
		static aosl_mpq_t queue (aosl_mpqp_t qp, aosl_mpq_t dq, aosl_ref_t ref, const char *f_name, __mpq_lambda_t&& task)
		{
			__mpq_lambda_t *task_obj = new __mpq_lambda_t (std::move (task));
			aosl_mpq_t qid = aosl_mpqp_queue (qp, dq, ref, f_name, ____mpq_f<typename std::remove_reference<__mpq_lambda_t>::type>, 1, task_obj);
			if (aosl_mpq_invalid (qid))
				delete task_obj;

			return qid;
		}

		/* __mpq_lambda_t: void (const aosl_ts_t &queued_ts, aosl_refobj_t robj) */
		template <typename __mpq_lambda_t,
				typename std::enable_if<std::is_void<decltype(std::declval<__mpq_lambda_t>()(
					std::declval<const aosl_ts_t &>(),
					std::declval<aosl_refobj_t>()
				))>::value, int>::type = 0>
		static aosl_mpq_t call (aosl_mpqp_t qp, aosl_ref_t ref, const char *f_name, __mpq_lambda_t&& task, void *task_result = NULL)
		{
			__mpq_lambda_t *task_obj = new __mpq_lambda_t (std::move (task));
			aosl_mpq_t qid = aosl_mpqp_call (qp, ref, f_name, ____mpq_f<typename std::remove_reference<__mpq_lambda_t>::type>, 2, task_obj, task_result);
			if (aosl_mpq_invalid (qid))
				delete task_obj;

			return qid;
		}

		/* __mpq_lambda_t: void (const aosl_ts_t &queued_ts, aosl_refobj_t robj) */
		template <typename __mpq_lambda_t,
				typename std::enable_if<std::is_void<decltype(std::declval<__mpq_lambda_t>()(
					std::declval<const aosl_ts_t &>(),
					std::declval<aosl_refobj_t>()
				))>::value, int>::type = 0>
		static aosl_mpq_t run (aosl_mpqp_t qp, aosl_ref_t ref, const char *f_name, __mpq_lambda_t&& task)
		{
			__mpq_lambda_t *task_obj = new __mpq_lambda_t (std::move (task));
			aosl_mpq_t qid = aosl_mpqp_run (qp, AOSL_MPQ_INVALID, ref, f_name, ____mpq_f<typename std::remove_reference<__mpq_lambda_t>::type>, 1, task_obj);
			if (aosl_mpq_invalid (qid))
				delete task_obj;

			return qid;
		}

		/* __mpq_0arg_lambda_t: void (void) */
		template <typename __mpq_0arg_lambda_t>
		static typename std::enable_if<std::is_void<decltype(std::declval<__mpq_0arg_lambda_t>()())>::value, aosl_mpq_t>::type
		queue (aosl_mpqp_t qp, aosl_ref_t ref, const char *f_name, __mpq_0arg_lambda_t&& task)
		{
			__mpq_0arg_lambda_t *task_obj = new __mpq_0arg_lambda_t (std::move (task));
			aosl_mpq_t qid = aosl_mpqp_queue (qp, AOSL_MPQ_INVALID, ref, f_name, ____mpq_0arg_f<typename std::remove_reference<__mpq_0arg_lambda_t>::type>, 1, task_obj);
			if (aosl_mpq_invalid (qid))
				delete task_obj;

			return qid;
		}

		/* __mpq_0arg_lambda_t: void (void) */
		template <typename __mpq_0arg_lambda_t>
		static typename std::enable_if<std::is_void<decltype(std::declval<__mpq_0arg_lambda_t>()())>::value, aosl_mpq_t>::type
		call (aosl_mpqp_t qp, aosl_ref_t ref, const char *f_name, __mpq_0arg_lambda_t&& task, void *task_result = NULL)
		{
			__mpq_0arg_lambda_t *task_obj = new __mpq_0arg_lambda_t (std::move (task));
			aosl_mpq_t qid = aosl_mpqp_call (qp, ref, f_name, ____mpq_0arg_f<typename std::remove_reference<__mpq_0arg_lambda_t>::type>, 2, task_obj, task_result);
			if (aosl_mpq_invalid (qid))
				delete task_obj;

			return qid;
		}

		/* __mpq_0arg_lambda_t: void (void) */
		template <typename __mpq_0arg_lambda_t>
		static typename std::enable_if<std::is_void<decltype(std::declval<__mpq_0arg_lambda_t>()())>::value, aosl_mpq_t>::type
		run (aosl_mpqp_t qp, aosl_ref_t ref, const char *f_name, __mpq_0arg_lambda_t&& task)
		{
			__mpq_0arg_lambda_t *task_obj = new __mpq_0arg_lambda_t (std::move (task));
			aosl_mpq_t qid = aosl_mpqp_run (qp, AOSL_MPQ_INVALID, ref, f_name, ____mpq_0arg_f<typename std::remove_reference<__mpq_0arg_lambda_t>::type>, 1, task_obj);
			if (aosl_mpq_invalid (qid))
				delete task_obj;

			return qid;
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
		/* __mpq_lambda_t: void (const aosl_ts_t &queued_ts, aosl_refobj_t robj) */
		template <typename __mpq_lambda_t,
			typename std::enable_if<std::is_void<decltype(std::declval<__mpq_lambda_t>()(
				std::declval<const aosl_ts_t &>(),
				std::declval<aosl_refobj_t>()))>::value, int>::type = 0>
		static void ____mpq_f (const aosl_ts_t *queued_ts_p, aosl_refobj_t robj, uintptr_t argc, uintptr_t argv [])
		{
			__mpq_lambda_t *task_obj = reinterpret_cast<__mpq_lambda_t *>(argv [0]);
			(*task_obj) (*queued_ts_p, robj);
			if (aosl_is_free_only (robj) || aosl_mpq_invalid (aosl_mpq_run_func_done_qid ())) {
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

		/* __mpq_0arg_lambda_t: void (void) */
		template <typename __mpq_0arg_lambda_t,
			typename std::enable_if<std::is_void<decltype(std::declval<__mpq_0arg_lambda_t>()())>::value, int>::type = 0>
		static void ____mpq_0arg_f (const aosl_ts_t *queued_ts_p, aosl_refobj_t robj, uintptr_t argc, uintptr_t argv [])
		{
			__mpq_0arg_lambda_t *task_obj = reinterpret_cast<__mpq_0arg_lambda_t *>(argv [0]);
			if (!aosl_is_free_only (robj))
				(*task_obj) ();
			if (aosl_is_free_only (robj) || aosl_mpq_invalid (aosl_mpq_run_func_done_qid ())) {
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

		/* __mpq_lambda_t: void (const aosl_ts_t &queued_ts, aosl_refobj_t robj) */
		template <typename __mpq_lambda_t,
			typename std::enable_if<std::is_void<decltype(std::declval<__mpq_lambda_t>()(
				std::declval<const aosl_ts_t &>(),
				std::declval<aosl_refobj_t>()))>::value, int>::type = 0>
		static void ____mpq_exec_f (const aosl_ts_t *queued_ts_p, aosl_refobj_t robj, uintptr_t argc, uintptr_t argv [])
		{
			__mpq_lambda_t *task_obj = reinterpret_cast<__mpq_lambda_t *>(argv [0]);
			(*task_obj) (*queued_ts_p, robj);
			delete task_obj;
		}

		/* __mpq_0arg_lambda_t: void (void) */
		template <typename __mpq_0arg_lambda_t,
			typename std::enable_if<std::is_void<decltype(std::declval<__mpq_0arg_lambda_t>()())>::value, int>::type = 0>
		static void ____mpq_exec_0arg_f (const aosl_ts_t *queued_ts_p, aosl_refobj_t robj, uintptr_t argc, uintptr_t argv [])
		{
			__mpq_0arg_lambda_t *task_obj = reinterpret_cast<__mpq_0arg_lambda_t *>(argv [0]);
			if (!aosl_is_free_only (robj))
				(*task_obj) ();
			delete task_obj;
		}
	#endif /* __AOSL_MPQ_H__ */

	#ifdef __AOSL_ASYNC_H__
		/**
		 * The stackless coroutine like implementation in AOSL. We could not
		 * support the real stackless coroutine except in the language level,
		 * so we just provide similar equivalent functionals here.
		 **/
	public:
		/* __async_prepare_lambda_t: int (int free_only) */
		template <typename __async_prepare_lambda_t,
			typename std::enable_if<std::is_same<decltype(std::declval<__async_prepare_lambda_t>()(std::declval<int>())), int>::value, int>::type = 0>
		int prepare (aosl_stack_id_t stack_id, const char *f_name, __async_prepare_lambda_t&& task)
		{
			__async_prepare_lambda_t *prepare_f = new __async_prepare_lambda_t (std::move (task));
			int err = aosl_async_prepare (stack_id, ref (), f_name, ____async_prepare_f<typename std::remove_reference<__async_prepare_lambda_t>::type>, 1, prepare_f);
			if (err < 0)
				delete prepare_f;

			return err;
		}

		/* __async_prepare_lambda_t: int (int free_only) */
		template <typename __async_prepare_lambda_t,
			typename std::enable_if<std::is_same<decltype(std::declval<__async_prepare_lambda_t>()(std::declval<int>())), int>::value, int>::type = 0>
		static int prepare (aosl_stack_id_t stack_id, aosl_ref_t ref, const char *f_name, __async_prepare_lambda_t&& task)
		{
			__async_prepare_lambda_t *prepare_f = new __async_prepare_lambda_t (std::move (task));
			int err = aosl_async_prepare (stack_id, ref, f_name, ____async_prepare_f<typename std::remove_reference<__async_prepare_lambda_t>::type>, 1, prepare_f);
			if (err < 0)
				delete prepare_f;

			return err;
		}

	private:
		/* __async_prepare_lambda_t: int (int free_only) */
		template <typename __async_prepare_lambda_t,
			typename std::enable_if<std::is_same<decltype(std::declval<__async_prepare_lambda_t>()(std::declval<int>())), int>::value, int>::type = 0>
		static int ____async_prepare_f (int free_only, uintptr_t argc, uintptr_t argv [])
		{
			__async_prepare_lambda_t *prepare_f = reinterpret_cast<__async_prepare_lambda_t *>(argv [0]);
			int err;
			err = (*prepare_f) (free_only);
			delete prepare_f;
			return err;
		}

	public:
		/* __async_resume_lambda_t: void (int free_only) */
		template <typename __async_resume_lambda_t>
		/**
		 * Do not know why this function needs to be changed to the return type style SFINAE, the lambda has one argument, but
		 * the buggy MSVC compiler version 14.25.28610 also reports the error C2672: XXX: no matching overloaded function found
		 * really, so change it anyway for now.
		 * -- Lionfore Hao Apr 15th, 2025
		 **/
		typename std::enable_if<std::is_void<decltype(std::declval<__async_resume_lambda_t>()(std::declval<int>()))>::value, int>::type
		resume (aosl_stack_id_t stack_id, const char *f_name, __async_resume_lambda_t&& task)
		{
			__async_resume_lambda_t *resume_f = new __async_resume_lambda_t (std::move (task));
			int err = aosl_async_resume (stack_id, ref (), f_name, ____async_resume_f<typename std::remove_reference<__async_resume_lambda_t>::type>, 1, resume_f);
			if (err < 0)
				delete resume_f;

			return err;
		}

		/* __async_resume_lambda_t: void (int free_only) */
		template <typename __async_resume_lambda_t>
		/**
		 * Do not know why this function needs to be changed to the return type style SFINAE, the lambda has one argument, but
		 * the buggy MSVC compiler version 14.25.28610 also reports the error C2672: XXX: no matching overloaded function found
		 * really, so change it anyway for now.
		 * -- Lionfore Hao Apr 15th, 2025
		 **/
		static typename std::enable_if<std::is_void<decltype(std::declval<__async_resume_lambda_t>()(std::declval<int>()))>::value, int>::type
		resume (aosl_stack_id_t stack_id, aosl_ref_t ref, const char *f_name, __async_resume_lambda_t&& task)
		{
			__async_resume_lambda_t *resume_f = new __async_resume_lambda_t (std::move (task));
			int err = aosl_async_resume (stack_id, ref, f_name, ____async_resume_f<typename std::remove_reference<__async_resume_lambda_t>::type>, 1, resume_f);
			if (err < 0)
				delete resume_f;

			return err;
		}

	private:
		/* __async_resume_lambda_t: void (int free_only) */
		template <typename __async_resume_lambda_t>
		/**
		 * Do not know why this function needs to be changed to the return type style SFINAE, the lambda has one argument, but
		 * the buggy MSVC compiler version 14.25.28610 also reports the error C2672: XXX: no matching overloaded function found
		 * really, so change it anyway for now.
		 * -- Lionfore Hao Apr 15th, 2025
		 **/
		static typename std::enable_if<std::is_void<decltype(std::declval<__async_resume_lambda_t>()(std::declval<int>()))>::value, void>::type
		____async_resume_f (int free_only, uintptr_t argc, uintptr_t argv [])
		{
			__async_resume_lambda_t *resume_f = reinterpret_cast<__async_resume_lambda_t *>(argv [0]);
			(*resume_f) (free_only);
			delete resume_f;
		}
	#endif /* __AOSL_ASYNC_H__ */
	#endif /* C++11 */

	#if (__cplusplus >= 201103) || (defined (_MSC_VER) && _MSC_VER >= 1800)
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
	aosl_ref_magic_t refmagic;

public:
	aosl_ref_class (bool destroy_wait = true)
	{
		refoop = aosl_ref_t_oop::create (this, __dtor, destroy_wait);
		if (aosl_ref_invalid (refoop))
			abort ();

		if (aosl_ref_magic (refoop->ref (), &refmagic) < 0)
			refmagic = AOSL_REF_MAGIC_INVALID;
	}

	aosl_ref_class (aosl_ref_t_oop *obj)
	{
		refoop = obj;
		if (aosl_ref_magic (obj->ref (), &refmagic) < 0)
			refmagic = AOSL_REF_MAGIC_INVALID;
	}

	aosl_ref_class (aosl_ref_t ref)
	{
		refoop = aosl_ref_t_oop::from_aosl_ref_t (ref);
		if (aosl_ref_magic (ref, &refmagic) < 0)
			refmagic = AOSL_REF_MAGIC_INVALID;
	}

	aosl_ref_t_oop *ref_oop () const
	{
		return refoop;
	}

	aosl_ref_t ref () const
	{
		return refoop->ref ();
	}

	aosl_ref_magic_t magic () const
	{
		return refoop->magic ();
	}

	int hold (aosl_ref_func_t f, uintptr_t argc, ...)
	{
		va_list args;
		int err;

		va_start (args, argc);
		err = refoop->hold_args (refmagic, f, argc, args);
		va_end (args);

		return err;
	}

	int hold_args (aosl_ref_func_t f, uintptr_t argc, va_list args)
	{
		return refoop->hold_args (refmagic, f, argc, args);
	}

	int hold_argv (aosl_ref_func_t f, uintptr_t argc, uintptr_t argv [])
	{
		return refoop->hold_argv (refmagic, f, argc, argv);
	}

	int read (aosl_ref_func_t f, uintptr_t argc, ...)
	{
		va_list args;
		int err;

		va_start (args, argc);
		err = refoop->read_args (refmagic, f, argc, args);
		va_end (args);

		return err;
	}

	int read_args (aosl_ref_func_t f, uintptr_t argc, va_list args)
	{
		return refoop->read_args (refmagic, f, argc, args);
	}

	int read_argv (aosl_ref_func_t f, uintptr_t argc, uintptr_t argv [])
	{
		return refoop->read_argv (refmagic, f, argc, argv);
	}

	int write (aosl_ref_func_t f, uintptr_t argc, ...)
	{
		va_list args;
		int err;

		va_start (args, argc);
		err = refoop->write_args (refmagic, f, argc, args);
		va_end (args);

		return err;
	}

	int write_args (aosl_ref_func_t f, uintptr_t argc, va_list args)
	{
		return refoop->write_args (refmagic, f, argc, args);
	}

	int write_argv (aosl_ref_func_t f, uintptr_t argc, uintptr_t argv [])
	{
		return refoop->write_argv (refmagic, f, argc, argv);
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
	static int hold (aosl_ref_t ref, aosl_ref_magic_t magic, aosl_ref_func_t f, uintptr_t argc, ...)
	{
		va_list args;
		int err;

		va_start (args, argc);
		err = aosl_ref_t_oop::hold_args (ref, magic, f, argc, args);
		va_end (args);

		return err;
	}

	static int hold_args (aosl_ref_t ref, aosl_ref_magic_t magic, aosl_ref_func_t f, uintptr_t argc, va_list args)
	{
		return aosl_ref_t_oop::hold_args (ref, magic, f, argc, args);
	}

	static int hold_argv (aosl_ref_t ref, aosl_ref_magic_t magic, aosl_ref_func_t f, uintptr_t argc, uintptr_t argv [])
	{
		return aosl_ref_t_oop::hold_argv (ref, magic, f, argc, argv);
	}

	static int read (aosl_ref_t ref, aosl_ref_magic_t magic, aosl_ref_func_t f, uintptr_t argc, ...)
	{
		va_list args;
		int err;

		va_start (args, argc);
		err = aosl_ref_t_oop::read_args (ref, magic, f, argc, args);
		va_end (args);

		return err;
	}

	static int read_args (aosl_ref_t ref, aosl_ref_magic_t magic, aosl_ref_func_t f, uintptr_t argc, va_list args)
	{
		return aosl_ref_t_oop::read_args (ref, magic, f, argc, args);
	}

	static int read_argv (aosl_ref_t ref, aosl_ref_magic_t magic, aosl_ref_func_t f, uintptr_t argc, uintptr_t argv [])
	{
		return aosl_ref_t_oop::read_argv (ref, magic, f, argc, argv);
	}

	static int write (aosl_ref_t ref, aosl_ref_magic_t magic, aosl_ref_func_t f, uintptr_t argc, ...)
	{
		va_list args;
		int err;

		va_start (args, argc);
		err = aosl_ref_t_oop::write_args (ref, magic, f, argc, args);
		va_end (args);

		return err;
	}

	static int write_args (aosl_ref_t ref, aosl_ref_magic_t magic, aosl_ref_func_t f, uintptr_t argc, va_list args)
	{
		return aosl_ref_t_oop::write_args (ref, magic, f, argc, args);
	}

	static int write_argv (aosl_ref_t ref, aosl_ref_magic_t magic, aosl_ref_func_t f, uintptr_t argc, uintptr_t argv [])
	{
		return aosl_ref_t_oop::write_argv (ref, magic, f, argc, argv);
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
		if (!aosl_ref_invalid (refoop->ref ())) {
			/**
			 * if the ref is valid, then just call the destroy
			 * function and do not delete this object directly
			 * even the return value indicates failure.
			 **/
			return refoop->destroy (do_delete);
		}

		if (do_delete) {
			/**
			 * delete this object directly only when the ref
			 * is invalid and the do_delete argument is true.
			 **/
			::delete this;
		}

		return 0;
	}

#if (__cplusplus >= 201103) || (defined (_MSC_VER) && _MSC_VER >= 1800)
	/* __ref_destroy_exec_lambda_t: void (int err) */
	template <typename __ref_destroy_exec_lambda_t,
		typename std::enable_if<std::is_void<decltype(std::declval<__ref_destroy_exec_lambda_t>()(std::declval<int>()))>::value, int>::type = 0>
	int destroy_exec (__ref_destroy_exec_lambda_t &&lambda_f, aosl_ref_t ares = AOSL_REF_INVALID)
	{
		return refoop->destroy_exec (std::move (lambda_f), ares);
	}
#endif

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

	int exec (aosl_mpq_t q, const char *f_name, aosl_mpq_func_argv_t f, uintptr_t argc, ...)
	{
		va_list args;
		int err;

		va_start (args, argc);
		err = refoop->exec_args (q, f_name, f, argc, args);
		va_end (args);

		return err;
	}

	int exec_args (aosl_mpq_t q, const char *f_name, aosl_mpq_func_argv_t f, uintptr_t argc, va_list args)
	{
		return refoop->exec_args (q, f_name, f, argc, args);
	}

	int exec_argv (aosl_mpq_t q, const char *f_name, aosl_mpq_func_argv_t f, uintptr_t argc, uintptr_t *argv)
	{
		return refoop->exec_argv (q, f_name, f, argc, argv);
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
#endif /* __AOSL_MPQP_H__ */
#endif /* __AOSL_MPQ_H__ */

	/* C++11 lambda encapsulations */
#if (__cplusplus >= 201103) || (defined (_MSC_VER) && _MSC_VER >= 1800)
public:
	/* __local_lambda_t: void (void *arg) */
	template <typename __local_lambda_t,
		typename std::enable_if<std::is_void<decltype(std::declval<__local_lambda_t>()(std::declval<void *>()))>::value, int>::type = 0>
	int hold (__local_lambda_t &&lambda_f)
	{
		return refoop->hold (std::move (lambda_f), refmagic);
	}

	/* __local_lambda_t: void (void *arg) */
	template <typename __local_lambda_t,
		typename std::enable_if<std::is_void<decltype(std::declval<__local_lambda_t>()(std::declval<void *>()))>::value, int>::type = 0>
	int read (__local_lambda_t &&lambda_f)
	{
		return refoop->read (std::move (lambda_f), refmagic);
	}

	/* __local_lambda_t: void (void *arg) */
	template <typename __local_lambda_t,
		typename std::enable_if<std::is_void<decltype(std::declval<__local_lambda_t>()(std::declval<void *>()))>::value, int>::type = 0>
	int write (__local_lambda_t &&lambda_f)
	{
		return refoop->write (std::move (lambda_f), refmagic);
	}

	/* __local_lambda_t: void (void *arg) */
	template <typename __local_lambda_t,
		typename std::enable_if<std::is_void<decltype(std::declval<__local_lambda_t>()(std::declval<void *>()))>::value, int>::type = 0>
	int unsafe (__local_lambda_t &&lambda_f)
	{
		return refoop->unsafe (std::move (lambda_f));
	}

	/* __local_lambda_t: void (void *arg) */
	template <typename __local_lambda_t,
		typename std::enable_if<std::is_void<decltype(std::declval<__local_lambda_t>()(std::declval<void *>()))>::value, int>::type = 0>
	int maystall (__local_lambda_t &&lambda_f)
	{
		return refoop->unsafe (std::move (lambda_f));
	}

	/* __local_lambda_t: void (void *arg) */
	template <typename __local_lambda_t,
		typename std::enable_if<std::is_void<decltype(std::declval<__local_lambda_t>()(std::declval<void *>()))>::value, int>::type = 0>
	static int hold (aosl_ref_t ref, __local_lambda_t &&lambda_f, aosl_ref_magic_t magic = AOSL_REF_MAGIC_INVALID)
	{
		return aosl_ref_t_oop::hold (ref, std::move (lambda_f), magic);
	}

	/* __local_lambda_t: void (void *arg) */
	template <typename __local_lambda_t,
		typename std::enable_if<std::is_void<decltype(std::declval<__local_lambda_t>()(std::declval<void *>()))>::value, int>::type = 0>
	static int read (aosl_ref_t ref, __local_lambda_t &&lambda_f, aosl_ref_magic_t magic = AOSL_REF_MAGIC_INVALID)
	{
		return aosl_ref_t_oop::read (ref, std::move (lambda_f), magic);
	}

	/* __local_lambda_t: void (void *arg) */
	template <typename __local_lambda_t,
		typename std::enable_if<std::is_void<decltype(std::declval<__local_lambda_t>()(std::declval<void *>()))>::value, int>::type = 0>
	static int write (aosl_ref_t ref, __local_lambda_t &&lambda_f, aosl_ref_magic_t magic = AOSL_REF_MAGIC_INVALID)
	{
		return aosl_ref_t_oop::write (ref, std::move (lambda_f), magic);
	}

	/* __local_lambda_t: void (void *arg) */
	template <typename __local_lambda_t,
		typename std::enable_if<std::is_void<decltype(std::declval<__local_lambda_t>()(std::declval<void *>()))>::value, int>::type = 0>
	static int unsafe (aosl_ref_t ref, __local_lambda_t &&lambda_f)
	{
		return aosl_ref_t_oop::unsafe (ref, std::move (lambda_f));
	}

	/* __local_lambda_t: void (void *arg) */
	template <typename __local_lambda_t,
		typename std::enable_if<std::is_void<decltype(std::declval<__local_lambda_t>()(std::declval<void *>()))>::value, int>::type = 0>
	static int maystall (aosl_ref_t ref, __local_lambda_t &&lambda_f)
	{
		return aosl_ref_t_oop::unsafe (ref, std::move (lambda_f));
	}

	/* __local_lambda_t: void (void *arg) */
	template <typename __local_lambda_t,
		typename std::enable_if<std::is_void<decltype(std::declval<__local_lambda_t>()(std::declval<void *>()))>::value, int>::type = 0>
	static int read (aosl_refobj_t robj, __local_lambda_t &&lambda_f)
	{
		return aosl_ref_t_oop::read (robj, std::move (lambda_f));
	}

	/* __local_lambda_t: void (void *arg) */
	template <typename __local_lambda_t,
		typename std::enable_if<std::is_void<decltype(std::declval<__local_lambda_t>()(std::declval<void *>()))>::value, int>::type = 0>
	static int unsafe (aosl_refobj_t robj, __local_lambda_t &&lambda_f)
	{
		return aosl_ref_t_oop::unsafe (robj, std::move (lambda_f));
	}

	/* __local_lambda_t: void (void *arg) */
	template <typename __local_lambda_t,
		typename std::enable_if<std::is_void<decltype(std::declval<__local_lambda_t>()(std::declval<void *>()))>::value, int>::type = 0>
	static int maystall (aosl_refobj_t robj, __local_lambda_t &&lambda_f)
	{
		return aosl_ref_t_oop::unsafe (robj, std::move (lambda_f));
	}

#ifdef __AOSL_MPQ_H__
public:
	/* MPQ encapsulations */
	/* __mpq_lambda_t: void (const aosl_ts_t &queued_ts, aosl_refobj_t robj) */
	template <typename __mpq_lambda_t,
			typename std::enable_if<std::is_void<decltype(std::declval<__mpq_lambda_t>()(
				std::declval<const aosl_ts_t &>(),
				std::declval<aosl_refobj_t>()
			))>::value, int>::type = 0>
	int queue (aosl_mpq_t tq, aosl_mpq_t dq, const char *f_name, __mpq_lambda_t&& task)
	{
		return refoop->queue (tq, dq, f_name, std::move (task));
	}

	/* __mpq_lambda_t: void (const aosl_ts_t &queued_ts, aosl_refobj_t robj) */
	template <typename __mpq_lambda_t,
			typename std::enable_if<std::is_void<decltype(std::declval<__mpq_lambda_t>()(
				std::declval<const aosl_ts_t &>(),
				std::declval<aosl_refobj_t>()
			))>::value, int>::type = 0>
	int call (aosl_mpq_t q, const char *f_name, __mpq_lambda_t&& task, void *task_result = NULL)
	{
		return refoop->call (q, f_name, std::move (task), task_result);
	}

	/* __mpq_lambda_t: void (const aosl_ts_t &queued_ts, aosl_refobj_t robj) */
	template <typename __mpq_lambda_t,
			typename std::enable_if<std::is_void<decltype(std::declval<__mpq_lambda_t>()(
				std::declval<const aosl_ts_t &>(),
				std::declval<aosl_refobj_t>()
			))>::value, int>::type = 0>
	int run (aosl_mpq_t q, const char *f_name, __mpq_lambda_t&& task)
	{
		return refoop->run (q, f_name, std::move (task));
	}

	/* __mpq_lambda_t: void (const aosl_ts_t &queued_ts, aosl_refobj_t robj) */
	template <typename __mpq_lambda_t,
			typename std::enable_if<std::is_void<decltype(std::declval<__mpq_lambda_t>()(
				std::declval<const aosl_ts_t &>(),
				std::declval<aosl_refobj_t>()
			))>::value, int>::type = 0>
	int exec (aosl_mpq_t q, const char *f_name, __mpq_lambda_t&& task)
	{
		return refoop->exec (q, f_name, std::move (task));
	}

	/* __mpq_0arg_lambda_t: void (void) */
	template <typename __mpq_0arg_lambda_t>
	typename std::enable_if<std::is_void<decltype(std::declval<__mpq_0arg_lambda_t>()())>::value, int>::type
	queue (aosl_mpq_t tq, const char *f_name, __mpq_0arg_lambda_t&& task)
	{
		return refoop->queue (tq, f_name, std::move (task));
	}

	/* __mpq_0arg_lambda_t: void (void) */
	template <typename __mpq_0arg_lambda_t>
	typename std::enable_if<std::is_void<decltype(std::declval<__mpq_0arg_lambda_t>()())>::value, int>::type
	call (aosl_mpq_t q, const char *f_name, __mpq_0arg_lambda_t&& task, void *task_result = NULL)
	{
		return refoop->call (q, f_name, std::move (task), task_result);
	}

	/* __mpq_0arg_lambda_t: void (void) */
	template <typename __mpq_0arg_lambda_t>
	typename std::enable_if<std::is_void<decltype(std::declval<__mpq_0arg_lambda_t>()())>::value, int>::type
	run (aosl_mpq_t q, const char *f_name, __mpq_0arg_lambda_t&& task)
	{
		return refoop->run (q, f_name, std::move (task));
	}

	/* __mpq_0arg_lambda_t: void (void) */
	template <typename __mpq_0arg_lambda_t>
	typename std::enable_if<std::is_void<decltype(std::declval<__mpq_0arg_lambda_t>()())>::value, int>::type
	exec (aosl_mpq_t q, const char *f_name, __mpq_0arg_lambda_t&& task)
	{
		return refoop->exec (q, f_name, std::move (task));
	}

#ifdef __AOSL_MPQP_H__
	/* MPQP encapsulations */
	/* __mpq_lambda_t: void (const aosl_ts_t &queued_ts, aosl_refobj_t robj) */
	template <typename __mpq_lambda_t,
			typename std::enable_if<std::is_void<decltype(std::declval<__mpq_lambda_t>()(
				std::declval<const aosl_ts_t &>(),
				std::declval<aosl_refobj_t>()
			))>::value, int>::type = 0>
	aosl_mpq_t queue (aosl_mpqp_t qp, aosl_mpq_t dq, const char *f_name, __mpq_lambda_t&& task)
	{
		return refoop->queue (qp, dq, f_name, std::move (task));
	}

	/* __mpq_lambda_t: void (const aosl_ts_t &queued_ts, aosl_refobj_t robj) */
	template <typename __mpq_lambda_t,
			typename std::enable_if<std::is_void<decltype(std::declval<__mpq_lambda_t>()(
				std::declval<const aosl_ts_t &>(),
				std::declval<aosl_refobj_t>()
			))>::value, int>::type = 0>
	aosl_mpq_t call (aosl_mpqp_t qp, const char *f_name, __mpq_lambda_t&& task, void *task_result = NULL)
	{
		return refoop->call (qp, f_name, std::move (task), task_result);
	}

	/* __mpq_lambda_t: void (const aosl_ts_t &queued_ts, aosl_refobj_t robj) */
	template <typename __mpq_lambda_t,
			typename std::enable_if<std::is_void<decltype(std::declval<__mpq_lambda_t>()(
				std::declval<const aosl_ts_t &>(),
				std::declval<aosl_refobj_t>()
			))>::value, int>::type = 0>
	aosl_mpq_t run (aosl_mpqp_t qp, const char *f_name, __mpq_lambda_t&& task)
	{
		return refoop->run (qp, f_name, std::move (task));
	}

	/* __mpq_0arg_lambda_t: void (void) */
	template <typename __mpq_0arg_lambda_t>
	typename std::enable_if<std::is_void<decltype(std::declval<__mpq_0arg_lambda_t>()())>::value, aosl_mpq_t>::type
	queue (aosl_mpqp_t qp, const char *f_name, __mpq_0arg_lambda_t&& task)
	{
		return refoop->queue (qp, f_name, std::move (task));
	}

	/* __mpq_0arg_lambda_t: void (void) */
	template <typename __mpq_0arg_lambda_t>
	typename std::enable_if<std::is_void<decltype(std::declval<__mpq_0arg_lambda_t>()())>::value, aosl_mpq_t>::type
	call (aosl_mpqp_t qp, const char *f_name, __mpq_0arg_lambda_t&& task, void *task_result = NULL)
	{
		return refoop->call (qp, f_name, std::move (task), task_result);
	}

	/* __mpq_0arg_lambda_t: void (void) */
	template <typename __mpq_0arg_lambda_t>
	typename std::enable_if<std::is_void<decltype(std::declval<__mpq_0arg_lambda_t>()())>::value, aosl_mpq_t>::type
	run (aosl_mpqp_t qp, const char *f_name, __mpq_0arg_lambda_t&& task)
	{
		return refoop->run (qp, f_name, std::move (task));
	}
#endif /* __AOSL_MPQP_H__ */

	/* MPQ with specified ref encapsulations */
	/* __mpq_lambda_t: void (const aosl_ts_t &queued_ts, aosl_refobj_t robj) */
	template <typename __mpq_lambda_t,
			typename std::enable_if<std::is_void<decltype(std::declval<__mpq_lambda_t>()(
				std::declval<const aosl_ts_t &>(),
				std::declval<aosl_refobj_t>()
			))>::value, int>::type = 0>
	static int queue (aosl_mpq_t tq, aosl_mpq_t dq, aosl_ref_t ref, const char *f_name, __mpq_lambda_t&& task)
	{
		return aosl_ref_t_oop::queue (tq, dq, ref, f_name, std::move (task));
	}

	/* __mpq_lambda_t: void (const aosl_ts_t &queued_ts, aosl_refobj_t robj) */
	template <typename __mpq_lambda_t,
			typename std::enable_if<std::is_void<decltype(std::declval<__mpq_lambda_t>()(
				std::declval<const aosl_ts_t &>(),
				std::declval<aosl_refobj_t>()
			))>::value, int>::type = 0>
	static int call (aosl_mpq_t q, aosl_ref_t ref, const char *f_name, __mpq_lambda_t&& task, void *task_result = NULL)
	{
		return aosl_ref_t_oop::call (q, ref, f_name, std::move (task), task_result);
	}

	/* __mpq_lambda_t: void (const aosl_ts_t &queued_ts, aosl_refobj_t robj) */
	template <typename __mpq_lambda_t,
			typename std::enable_if<std::is_void<decltype(std::declval<__mpq_lambda_t>()(
				std::declval<const aosl_ts_t &>(),
				std::declval<aosl_refobj_t>()
			))>::value, int>::type = 0>
	static int run (aosl_mpq_t q, aosl_ref_t ref, const char *f_name, __mpq_lambda_t&& task)
	{
		return aosl_ref_t_oop::run (q, ref, f_name, std::move (task));
	}

	/* __mpq_lambda_t: void (const aosl_ts_t &queued_ts, aosl_refobj_t robj) */
	template <typename __mpq_lambda_t,
			typename std::enable_if<std::is_void<decltype(std::declval<__mpq_lambda_t>()(
				std::declval<const aosl_ts_t &>(),
				std::declval<aosl_refobj_t>()
			))>::value, int>::type = 0>
	static int exec (aosl_mpq_t q, aosl_ref_t ref, const char *f_name, __mpq_lambda_t&& task)
	{
		return aosl_ref_t_oop::exec (q, ref, f_name, std::move (task));
	}

	/* __mpq_0arg_lambda_t: void (void) */
	template <typename __mpq_0arg_lambda_t>
	static typename std::enable_if<std::is_void<decltype(std::declval<__mpq_0arg_lambda_t>()())>::value, int>::type
	queue (aosl_mpq_t tq, aosl_ref_t ref, const char *f_name, __mpq_0arg_lambda_t&& task)
	{
		return aosl_ref_t_oop::queue (tq, ref, f_name, std::move (task));
	}

	/* __mpq_0arg_lambda_t: void (void) */
	template <typename __mpq_0arg_lambda_t>
	static typename std::enable_if<std::is_void<decltype(std::declval<__mpq_0arg_lambda_t>()())>::value, int>::type
	call (aosl_mpq_t q, aosl_ref_t ref, const char *f_name, __mpq_0arg_lambda_t&& task, void *task_result = NULL)
	{
		return aosl_ref_t_oop::call (q, ref, f_name, std::move (task), task_result);
	}

	/* __mpq_0arg_lambda_t: void (void) */
	template <typename __mpq_0arg_lambda_t>
	static typename std::enable_if<std::is_void<decltype(std::declval<__mpq_0arg_lambda_t>()())>::value, int>::type
	run (aosl_mpq_t q, aosl_ref_t ref, const char *f_name, __mpq_0arg_lambda_t&& task)
	{
		return aosl_ref_t_oop::run (q, ref, f_name, std::move (task));
	}

	/* __mpq_0arg_lambda_t: void (void) */
	template <typename __mpq_0arg_lambda_t>
	static typename std::enable_if<std::is_void<decltype(std::declval<__mpq_0arg_lambda_t>()())>::value, int>::type
	exec (aosl_mpq_t q, aosl_ref_t ref, const char *f_name, __mpq_0arg_lambda_t&& task)
	{
		return aosl_ref_t_oop::exec (q, ref, f_name, std::move (task));
	}

#ifdef __AOSL_MPQP_H__
	/* MPQP with specified ref encapsulations */
	/* __mpq_lambda_t: void (const aosl_ts_t &queued_ts, aosl_refobj_t robj) */
	template <typename __mpq_lambda_t,
			typename std::enable_if<std::is_void<decltype(std::declval<__mpq_lambda_t>()(
				std::declval<const aosl_ts_t &>(),
				std::declval<aosl_refobj_t>()
			))>::value, int>::type = 0>
	static aosl_mpq_t queue (aosl_mpqp_t qp, aosl_mpq_t dq, aosl_ref_t ref, const char *f_name, __mpq_lambda_t&& task)
	{
		return aosl_ref_t_oop::queue (qp, dq, ref, f_name, std::move (task));
	}

	/* __mpq_lambda_t: void (const aosl_ts_t &queued_ts, aosl_refobj_t robj) */
	template <typename __mpq_lambda_t,
			typename std::enable_if<std::is_void<decltype(std::declval<__mpq_lambda_t>()(
				std::declval<const aosl_ts_t &>(),
				std::declval<aosl_refobj_t>()
			))>::value, int>::type = 0>
	static aosl_mpq_t call (aosl_mpqp_t qp, aosl_ref_t ref, const char *f_name, __mpq_lambda_t&& task, void *task_result = NULL)
	{
		return aosl_ref_t_oop::call (qp, ref, f_name, std::move (task), task_result);
	}

	/* __mpq_lambda_t: void (const aosl_ts_t &queued_ts, aosl_refobj_t robj) */
	template <typename __mpq_lambda_t,
			typename std::enable_if<std::is_void<decltype(std::declval<__mpq_lambda_t>()(
				std::declval<const aosl_ts_t &>(),
				std::declval<aosl_refobj_t>()
			))>::value, int>::type = 0>
	static aosl_mpq_t run (aosl_mpqp_t qp, aosl_ref_t ref, const char *f_name, __mpq_lambda_t&& task)
	{
		return aosl_ref_t_oop::run (qp, ref, f_name, std::move (task));
	}

	/* __mpq_0arg_lambda_t: void (void) */
	template <typename __mpq_0arg_lambda_t>
	static typename std::enable_if<std::is_void<decltype(std::declval<__mpq_0arg_lambda_t>()())>::value, aosl_mpq_t>::type
	queue (aosl_mpqp_t qp, aosl_ref_t ref, const char *f_name, __mpq_0arg_lambda_t&& task)
	{
		return aosl_ref_t_oop::queue (qp, ref, f_name, std::move (task));
	}

	/* __mpq_0arg_lambda_t: void (void) */
	template <typename __mpq_0arg_lambda_t>
	static typename std::enable_if<std::is_void<decltype(std::declval<__mpq_0arg_lambda_t>()())>::value, aosl_mpq_t>::type
	call (aosl_mpqp_t qp, aosl_ref_t ref, const char *f_name, __mpq_0arg_lambda_t&& task, void *task_result = NULL)
	{
		return aosl_ref_t_oop::call (qp, ref, f_name, std::move (task), task_result);
	}

	/* __mpq_0arg_lambda_t: void (void) */
	template <typename __mpq_0arg_lambda_t>
	static typename std::enable_if<std::is_void<decltype(std::declval<__mpq_0arg_lambda_t>()())>::value, aosl_mpq_t>::type
	run (aosl_mpqp_t qp, aosl_ref_t ref, const char *f_name, __mpq_0arg_lambda_t&& task)
	{
		return aosl_ref_t_oop::run (qp, ref, f_name, std::move (task));
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
	/* __async_prepare_lambda_t: int (int free_only) */
	template <typename __async_prepare_lambda_t,
		typename std::enable_if<std::is_same<decltype(std::declval<__async_prepare_lambda_t>()(std::declval<int>())), int>::value, int>::type = 0>
	int prepare (aosl_stack_id_t stack_id, const char *f_name, __async_prepare_lambda_t&& task)
	{
		return refoop->prepare (stack_id, f_name, std::move (task));
	}

	/* __async_prepare_lambda_t: int (int free_only) */
	template <typename __async_prepare_lambda_t,
		typename std::enable_if<std::is_same<decltype(std::declval<__async_prepare_lambda_t>()(std::declval<int>())), int>::value, int>::type = 0>
	static int prepare (aosl_stack_id_t stack_id, aosl_ref_t ref, const char *f_name, __async_prepare_lambda_t&& task)
	{
		return aosl_ref_t_oop::prepare (stack_id, ref, f_name, std::move (task));
	}

public:
	/* __async_resume_lambda_t: void (int free_only) */
	template <typename __async_resume_lambda_t>
	/**
	 * Do not know why this function needs to be changed to the return type style SFINAE, the lambda has one argument, but
	 * the buggy MSVC compiler version 14.25.28610 also reports the error C2672: XXX: no matching overloaded function found
	 * really, so change it anyway for now.
	 * -- Lionfore Hao Apr 15th, 2025
	 **/
	typename std::enable_if<std::is_void<decltype(std::declval<__async_resume_lambda_t>()(std::declval<int>()))>::value, int>::type
	resume (aosl_stack_id_t stack_id, const char *f_name, __async_resume_lambda_t&& task)
	{
		return refoop->resume (stack_id, f_name, std::move (task));
	}

	/* __async_resume_lambda_t: void (int free_only) */
	template <typename __async_resume_lambda_t>
	/**
	 * Do not know why this function needs to be changed to the return type style SFINAE, the lambda has one argument, but
	 * the buggy MSVC compiler version 14.25.28610 also reports the error C2672: XXX: no matching overloaded function found
	 * really, so change it anyway for now.
	 * -- Lionfore Hao Apr 15th, 2025
	 **/
	static typename std::enable_if<std::is_void<decltype(std::declval<__async_resume_lambda_t>()(std::declval<int>()))>::value, int>::type
	resume (aosl_stack_id_t stack_id, aosl_ref_t ref, const char *f_name, __async_resume_lambda_t&& task)
	{
		return aosl_ref_t_oop::resume (stack_id, ref, f_name, std::move (task));
	}
#endif /* __AOSL_ASYNC_H__ */
#endif /* C++11 */

#if (__cplusplus >= 201103) || (defined (_MSC_VER) && _MSC_VER >= 1800)
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
		if (_ptr != p) {
			if (_ptr != NULL) {
	/* C++11 lambda encapsulations */
#if (__cplusplus >= 201103) || (defined (_MSC_VER) && _MSC_VER >= 1800)
				/**
				 * We employ unsafe function to make sure we can
				 * still access this unique ptr object after the
				 * destroy, because unsafe function holds a ref
				 * of the object, so the memory of the object is
				 * still accessible.
				 * Please be careful that only the unsafe action
				 * is allowed after the ref object has already
				 * been destroyed internally, so do not use hold
				 * here, otherwise would lead to the destroy not
				 * being executed.
				 **/
				_ptr->unsafe ([&] (void *arg) {
					_ptr->destroy (true/* do delete */);
					_ptr = p;
				});
#else
				_ptr->unsafe (____ref_reset_f, 2, this, p);
#endif
			} else {
				/**
				 * If the unique ptr pointer is empty, then we
				 * just set it to the new pointer directly.
				 **/
				_ptr = p;
			}
		}
	}

	~aosl_ref_unique_ptr ()
	{
		reset ();
	}

#if (__cplusplus >= 201103) || (defined (_MSC_VER) && _MSC_VER >= 1800)
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

	static void ____ref_reset_f (void *arg, uintptr_t argc, uintptr_t argv [])
	{
		aosl_ref_unique_ptr *__this = (aosl_ref_unique_ptr *)argv [0];
		T_ref_cls *p = (T_ref_cls *)argv [1];
		__this->_ptr->destroy (true/* do delete */);
		__this->_ptr = p;
	}
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

#if (__cplusplus >= 201103) || (defined (_MSC_VER) && _MSC_VER >= 1800)
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