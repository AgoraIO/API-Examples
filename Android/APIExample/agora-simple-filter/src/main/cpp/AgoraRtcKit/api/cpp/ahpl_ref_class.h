/*************************************************************
 * Author:	Lionfore Hao (haolianfu@agora.io)
 * Date	 :	Nov 19th, 2018
 * Module:	AHPL reference object for C++ definition file
 *
 *
 * This is a part of the Advanced High Performance Library.
 * Copyright (C) 2018 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#ifndef __AHPL_REF_OBJ_CPP_H__
#define __AHPL_REF_OBJ_CPP_H__


#include <stdlib.h>

#include <api/ahpl_types.h>
#include <api/ahpl_defs.h>
#include <api/ahpl_ref.h>

#ifdef COMPILING_WITH_MPQ_H
#include <api/ahpl_mpq.h>
#ifdef COMPILING_WITH_MPQP_H
#include <api/ahpl_mpqp.h>
#endif
#endif

#ifdef COMPILING_WITH_ASYNC_H
#include <api/ahpl_async.h>
#endif

#if (__cplusplus >= 201103) || defined (_MSC_VER)
#include <functional>
#include <memory>
#endif

class ahpl_ref_class {
private:
	ahpl_ref_t ref_id;

public:
	ahpl_ref_class (bool caller_free = true)
	{
		ref_id = ahpl_ref_create (this, __dtor, (int)caller_free);
		if (ahpl_ref_invalid (ref_id))
			abort ();
	}

	ahpl_ref_class (ahpl_ref_t ref)
	{
		ref_id = ref;
	}

	ahpl_ref_t ref () const
	{
		return ref_id;
	}

	int hold (ahpl_ref_func_t f, uintptr_t argc, ...)
	{
		va_list args;
		int err;

		va_start (args, argc);
		err = ahpl_ref_hold_args (ref (), f, argc, args);
		va_end (args);

		return err;
	}

	int hold_args (ahpl_ref_func_t f, uintptr_t argc, va_list args)
	{
		return ahpl_ref_hold_args (ref (), f, argc, args);
	}

	int hold_argv (ahpl_ref_func_t f, uintptr_t argc, uintptr_t argv [])
	{
		return ahpl_ref_hold_argv (ref (), f, argc, argv);
	}

	int read (ahpl_ref_func_t f, uintptr_t argc, ...)
	{
		va_list args;
		int err;

		va_start (args, argc);
		err = ahpl_ref_read_args (ref (), f, argc, args);
		va_end (args);

		return err;
	}

	int read_args (ahpl_ref_func_t f, uintptr_t argc, va_list args)
	{
		return ahpl_ref_read_args (ref (), f, argc, args);
	}

	int read_argv (ahpl_ref_func_t f, uintptr_t argc, uintptr_t argv [])
	{
		return ahpl_ref_read_argv (ref (), f, argc, argv);
	}

	int write (ahpl_ref_func_t f, uintptr_t argc, ...)
	{
		va_list args;
		int err;

		va_start (args, argc);
		err = ahpl_ref_write_args (ref (), f, argc, args);
		va_end (args);

		return err;
	}

	int write_args (ahpl_ref_func_t f, uintptr_t argc, va_list args)
	{
		return ahpl_ref_write_args (ref (), f, argc, args);
	}

	int write_argv (ahpl_ref_func_t f, uintptr_t argc, uintptr_t argv [])
	{
		return ahpl_ref_write_argv (ref (), f, argc, argv);
	}

	int maystall (ahpl_ref_func_t f, uintptr_t argc, ...)
	{
		va_list args;
		int err;

		va_start (args, argc);
		err = ahpl_ref_maystall_args (ref (), f, argc, args);
		va_end (args);

		return err;
	}

	int maystall_args (ahpl_ref_func_t f, uintptr_t argc, va_list args)
	{
		return ahpl_ref_maystall_args (ref (), f, argc, args);
	}

	int maystall_argv (ahpl_ref_func_t f, uintptr_t argc, uintptr_t argv [])
	{
		return ahpl_ref_maystall_argv (ref (), f, argc, argv);
	}

	/* The static version of member functions */
	static int hold (ahpl_ref_t ref, ahpl_ref_func_t f, uintptr_t argc, ...)
	{
		va_list args;
		int err;

		va_start (args, argc);
		err = ahpl_ref_hold_args (ref, f, argc, args);
		va_end (args);

		return err;
	}

	static int hold_args (ahpl_ref_t ref, ahpl_ref_func_t f, uintptr_t argc, va_list args)
	{
		return ahpl_ref_hold_args (ref, f, argc, args);
	}

	static int hold_argv (ahpl_ref_t ref, ahpl_ref_func_t f, uintptr_t argc, uintptr_t argv [])
	{
		return ahpl_ref_hold_argv (ref, f, argc, argv);
	}

	static int read (ahpl_ref_t ref, ahpl_ref_func_t f, uintptr_t argc, ...)
	{
		va_list args;
		int err;

		va_start (args, argc);
		err = ahpl_ref_read_args (ref, f, argc, args);
		va_end (args);

		return err;
	}

	static int read_args (ahpl_ref_t ref, ahpl_ref_func_t f, uintptr_t argc, va_list args)
	{
		return ahpl_ref_read_args (ref, f, argc, args);
	}

	static int read_argv (ahpl_ref_t ref, ahpl_ref_func_t f, uintptr_t argc, uintptr_t argv [])
	{
		return ahpl_ref_read_argv (ref, f, argc, argv);
	}

	static int write (ahpl_ref_t ref, ahpl_ref_func_t f, uintptr_t argc, ...)
	{
		va_list args;
		int err;

		va_start (args, argc);
		err = ahpl_ref_write_args (ref, f, argc, args);
		va_end (args);

		return err;
	}

	static int write_args (ahpl_ref_t ref, ahpl_ref_func_t f, uintptr_t argc, va_list args)
	{
		return ahpl_ref_write_args (ref, f, argc, args);
	}

	static int write_argv (ahpl_ref_t ref, ahpl_ref_func_t f, uintptr_t argc, uintptr_t argv [])
	{
		return ahpl_ref_write_argv (ref, f, argc, argv);
	}

	static int maystall (ahpl_ref_t ref, ahpl_ref_func_t f, uintptr_t argc, ...)
	{
		va_list args;
		int err;

		va_start (args, argc);
		err = ahpl_ref_maystall_args (ref, f, argc, args);
		va_end (args);

		return err;
	}

	static int maystall_args (ahpl_ref_t ref, ahpl_ref_func_t f, uintptr_t argc, va_list args)
	{
		return ahpl_ref_maystall_args (ref, f, argc, args);
	}

	static int maystall_argv (ahpl_ref_t ref, ahpl_ref_func_t f, uintptr_t argc, uintptr_t argv [])
	{
		return ahpl_ref_maystall_argv (ref, f, argc, argv);
	}

	static int read (ahpl_refobj_t robj, ahpl_ref_func_t f, uintptr_t argc, ...)
	{
		va_list args;
		int err;

		va_start (args, argc);
		err = ahpl_refobj_read_args (robj, f, argc, args);
		va_end (args);

		return err;
	}

	static int read_args (ahpl_refobj_t robj, ahpl_ref_func_t f, uintptr_t argc, va_list args)
	{
		return ahpl_refobj_read_args (robj, f, argc, args);
	}

	static int read_argv (ahpl_refobj_t robj, ahpl_ref_func_t f, uintptr_t argc, uintptr_t argv [])
	{
		return ahpl_refobj_read_argv (robj, f, argc, argv);
	}

	static int maystall (ahpl_refobj_t robj, ahpl_ref_func_t f, uintptr_t argc, ...)
	{
		va_list args;
		int err;

		va_start (args, argc);
		err = ahpl_refobj_maystall_args (robj, f, argc, args);
		va_end (args);

		return err;
	}

	static int maystall_args (ahpl_refobj_t robj, ahpl_ref_func_t f, uintptr_t argc, va_list args)
	{
		return ahpl_refobj_maystall_args (robj, f, argc, args);
	}

	static int maystall_argv (ahpl_refobj_t robj, ahpl_ref_func_t f, uintptr_t argc, uintptr_t argv [])
	{
		return ahpl_refobj_maystall_argv (robj, f, argc, argv);
	}

	static ahpl_ref_class *from_refobj (ahpl_refobj_t robj)
	{
		return (ahpl_ref_class *)ahpl_refobj_arg (robj);
	}

	/* set the living scope ref object of this ref object */
	int set_scope (ahpl_ref_t scope_ref)
	{
		return ahpl_ref_set_scope (ref (), scope_ref);
	}

	int destroy (bool do_delete = true)
	{
		if (!ahpl_ref_invalid (ref_id))
			return ahpl_ref_destroy (ref_id, (int)do_delete);

		if (do_delete)
			delete this;

		return 0;
	}

public:
	class deleter {
	public:
		void operator () (ahpl_ref_class *obj_ptr) const
		{
			if (obj_ptr != NULL)
				obj_ptr->destroy ();
		}
	};

protected:
	/* We do not allow delete this object directly. */
	virtual ~ahpl_ref_class ()
	{
	}

private:
	static void __dtor (void *arg)
	{
		ahpl_ref_class *__this = (ahpl_ref_class *)arg;
		::delete __this;
	}

#ifdef __AHPL_MPQ_H__
	/* MPQ relative encapsulations */
public:
	int queue (ahpl_mpq_t tq, ahpl_mpq_t dq, const char *f_name, ahpl_mpq_func_argv_t f, uintptr_t argc, ...)
	{
		va_list args;
		int err;

		va_start (args, argc);
		err = ahpl_mpq_queue_args (tq, dq, ref (), f_name, f, argc, args); 
		va_end (args);

		return err;
	}

	int queue_args (ahpl_mpq_t tq, ahpl_mpq_t dq, const char *f_name, ahpl_mpq_func_argv_t f, uintptr_t argc, va_list args)
	{
		return ahpl_mpq_queue_args (tq, dq, ref (), f_name, f, argc, args); 
	}

	int queue_argv (ahpl_mpq_t tq, ahpl_mpq_t dq, const char *f_name, ahpl_mpq_func_argv_t f, uintptr_t argc, uintptr_t *argv)
	{
		return ahpl_mpq_queue_argv (tq, dq, ref (), f_name, f, argc, argv); 
	}

	int queue_data (ahpl_mpq_t tq, ahpl_mpq_t dq, const char *f_name, ahpl_mpq_func_data_t f, size_t len, void *data)
	{
		return ahpl_mpq_queue_data (tq, dq, ref (), f_name, f, len, data);
	}

	int call (ahpl_mpq_t q, const char *f_name, ahpl_mpq_func_argv_t f, uintptr_t argc, ...)
	{
		va_list args;
		int err;

		va_start (args, argc);
		err = ahpl_mpq_call_args (q, ref (), f_name, f, argc, args); 
		va_end (args);

		return err;
	}

	int call_args (ahpl_mpq_t q, const char *f_name, ahpl_mpq_func_argv_t f, uintptr_t argc, va_list args)
	{
		return ahpl_mpq_call_args (q, ref (), f_name, f, argc, args); 
	}

	int call_argv (ahpl_mpq_t q, const char *f_name, ahpl_mpq_func_argv_t f, uintptr_t argc, uintptr_t *argv)
	{
		return ahpl_mpq_call_argv (q, ref (), f_name, f, argc, argv); 
	}

	int call_data (ahpl_mpq_t q, const char *f_name, ahpl_mpq_func_data_t f, size_t len, void *data)
	{
		return ahpl_mpq_call_data (q, ref (), f_name, f, len, data);
	}

	int run (ahpl_mpq_t q, ahpl_mpq_t dq, const char *f_name, ahpl_mpq_func_argv_t f, uintptr_t argc, ...)
	{
		va_list args;
		int err;

		va_start (args, argc);
		err = ahpl_mpq_run_args (q, dq, ref (), f_name, f, argc, args); 
		va_end (args);

		return err;
	}

	int run_args (ahpl_mpq_t q, ahpl_mpq_t dq, const char *f_name, ahpl_mpq_func_argv_t f, uintptr_t argc, va_list args)
	{
		return ahpl_mpq_run_args (q, dq, ref (), f_name, f, argc, args); 
	}

	int run_argv (ahpl_mpq_t q, ahpl_mpq_t dq, const char *f_name, ahpl_mpq_func_argv_t f, uintptr_t argc, uintptr_t *argv)
	{
		return ahpl_mpq_run_argv (q, dq, ref (), f_name, f, argc, argv); 
	}

	int run_data (ahpl_mpq_t q, ahpl_mpq_t dq, const char *f_name, ahpl_mpq_func_data_t f, size_t len, void *data)
	{
		return ahpl_mpq_run_data (q, dq, ref (), f_name, f, len, data);
	}

#ifdef __AHPL_MPQP_H__
	/* MPQP relative encapsulations */
	ahpl_mpq_t queue (ahpl_mpqp_t qp, ahpl_mpq_t dq, const char *f_name, ahpl_mpq_func_argv_t f, uintptr_t argc, ...)
	{
		va_list args;
		ahpl_mpq_t qid;

		va_start (args, argc);
		qid = ahpl_mpqp_queue_args (qp, dq, ref (), f_name, f, argc, args); 
		va_end (args);

		return qid;
	}

	ahpl_mpq_t queue_args (ahpl_mpqp_t qp, ahpl_mpq_t dq, const char *f_name, ahpl_mpq_func_argv_t f, uintptr_t argc, va_list args)
	{
		return ahpl_mpqp_queue_args (qp, dq, ref (), f_name, f, argc, args); 
	}

	ahpl_mpq_t queue_argv (ahpl_mpqp_t qp, ahpl_mpq_t dq, const char *f_name, ahpl_mpq_func_argv_t f, uintptr_t argc, uintptr_t *argv)
	{
		return ahpl_mpqp_queue_argv (qp, dq, ref (), f_name, f, argc, argv); 
	}

	ahpl_mpq_t queue_data (ahpl_mpqp_t qp, ahpl_mpq_t dq, const char *f_name, ahpl_mpq_func_data_t f, size_t len, void *data)
	{
		return ahpl_mpqp_queue_data (qp, dq, ref (), f_name, f, len, data);
	}

	ahpl_mpq_t call (ahpl_mpqp_t qp, const char *f_name, ahpl_mpq_func_argv_t f, uintptr_t argc, ...)
	{
		va_list args;
		ahpl_mpq_t qid;

		va_start (args, argc);
		qid = ahpl_mpqp_call_args (qp, ref (), f_name, f, argc, args); 
		va_end (args);

		return qid;
	}

	ahpl_mpq_t call_args (ahpl_mpqp_t qp, const char *f_name, ahpl_mpq_func_argv_t f, uintptr_t argc, va_list args)
	{
		return ahpl_mpqp_call_args (qp, ref (), f_name, f, argc, args); 
	}

	ahpl_mpq_t call_argv (ahpl_mpqp_t qp, const char *f_name, ahpl_mpq_func_argv_t f, uintptr_t argc, uintptr_t *argv)
	{
		return ahpl_mpqp_call_argv (qp, ref (), f_name, f, argc, argv); 
	}

	ahpl_mpq_t call_data (ahpl_mpqp_t qp, const char *f_name, ahpl_mpq_func_data_t f, size_t len, void *data)
	{
		return ahpl_mpqp_call_data (qp, ref (), f_name, f, len, data);
	}

	ahpl_mpq_t run (ahpl_mpqp_t qp, ahpl_mpq_t dq, const char *f_name, ahpl_mpq_func_argv_t f, uintptr_t argc, ...)
	{
		va_list args;
		ahpl_mpq_t qid;

		va_start (args, argc);
		qid = ahpl_mpqp_run_args (qp, dq, ref (), f_name, f, argc, args); 
		va_end (args);

		return qid;
	}

	ahpl_mpq_t run_args (ahpl_mpqp_t qp, ahpl_mpq_t dq, const char *f_name, ahpl_mpq_func_argv_t f, uintptr_t argc, va_list args)
	{
		return ahpl_mpqp_run_args (qp, dq, ref (), f_name, f, argc, args); 
	}

	ahpl_mpq_t run_argv (ahpl_mpqp_t qp, ahpl_mpq_t dq, const char *f_name, ahpl_mpq_func_argv_t f, uintptr_t argc, uintptr_t *argv)
	{
		return ahpl_mpqp_run_argv (qp, dq, ref (), f_name, f, argc, argv); 
	}

	ahpl_mpq_t run_data (ahpl_mpqp_t qp, ahpl_mpq_t dq, const char *f_name, ahpl_mpq_func_data_t f, size_t len, void *data)
	{
		return ahpl_mpqp_run_data (qp, dq, ref (), f_name, f, len, data);
	}

	int pool_tail_queue (ahpl_mpqp_t qp, ahpl_mpq_t dq, const char *f_name, ahpl_mpq_func_argv_t f, uintptr_t argc, ...)
	{
		va_list args;
		int err;

		va_start (args, argc);
		err = ahpl_mpqp_pool_tail_queue_args (qp, dq, ref (), f_name, f, argc, args); 
		va_end (args);

		return err;
	}

	int pool_tail_queue_args (ahpl_mpqp_t qp, ahpl_mpq_t dq, const char *f_name, ahpl_mpq_func_argv_t f, uintptr_t argc, va_list args)
	{
		return ahpl_mpqp_pool_tail_queue_args (qp, dq, ref (), f_name, f, argc, args); 
	}

	int pool_tail_queue_argv (ahpl_mpqp_t qp, ahpl_mpq_t dq, const char *f_name, ahpl_mpq_func_argv_t f, uintptr_t argc, uintptr_t *argv)
	{
		return ahpl_mpqp_pool_tail_queue_argv (qp, dq, ref (), f_name, f, argc, argv); 
	}
#endif /* __AHPL_MPQP_H__ */
#endif /* __AHPL_MPQ_H__ */

	/* C++11 lambda encapsulations */
#if (__cplusplus >= 201103) || defined (_MSC_VER)
public:
	typedef std::function <void (ahpl_ref_class *obj)> ahpl_ref_lambda_f;

	int hold (ahpl_ref_lambda_f &&lambda_f)
	{
		ahpl_ref_lambda_f lambda_obj (std::move (lambda_f));
		return ahpl_ref_class::hold (____ref_f, 1, &lambda_obj);
	}

	int read (ahpl_ref_lambda_f &&lambda_f)
	{
		ahpl_ref_lambda_f lambda_obj (std::move (lambda_f));
		return ahpl_ref_class::read (____ref_f, 1, &lambda_obj);
	}

	int write (ahpl_ref_lambda_f &&lambda_f)
	{
		ahpl_ref_lambda_f lambda_obj (std::move (lambda_f));
		return ahpl_ref_class::write (____ref_f, 1, &lambda_obj);
	}

	int maystall (ahpl_ref_lambda_f &&lambda_f)
	{
		ahpl_ref_lambda_f lambda_obj (std::move (lambda_f));
		return ahpl_ref_class::maystall (____ref_f, 1, &lambda_obj);
	}

	static int hold (ahpl_ref_t ref, ahpl_ref_lambda_f &&lambda_f)
	{
		ahpl_ref_lambda_f lambda_obj (std::move (lambda_f));
		return ahpl_ref_class::hold (ref, ____ref_f, 1, &lambda_obj);
	}

	static int read (ahpl_ref_t ref, ahpl_ref_lambda_f &&lambda_f)
	{
		ahpl_ref_lambda_f lambda_obj (std::move (lambda_f));
		return ahpl_ref_class::read (ref, ____ref_f, 1, &lambda_obj);
	}

	static int write (ahpl_ref_t ref, ahpl_ref_lambda_f &&lambda_f)
	{
		ahpl_ref_lambda_f lambda_obj (std::move (lambda_f));
		return ahpl_ref_class::write (ref, ____ref_f, 1, &lambda_obj);
	}

	static int maystall (ahpl_ref_t ref, ahpl_ref_lambda_f &&lambda_f)
	{
		ahpl_ref_lambda_f lambda_obj (std::move (lambda_f));
		return ahpl_ref_class::maystall (ref, ____ref_f, 1, &lambda_obj);
	}

	static int read (ahpl_refobj_t robj, ahpl_ref_lambda_f &&lambda_f)
	{
		ahpl_ref_lambda_f lambda_obj (std::move (lambda_f));
		return ahpl_ref_class::read (robj, ____ref_f, 1, &lambda_obj);
	}

	static int maystall (ahpl_refobj_t robj, ahpl_ref_lambda_f &&lambda_f)
	{
		ahpl_ref_lambda_f lambda_obj (std::move (lambda_f));
		return ahpl_ref_class::maystall (robj, ____ref_f, 1, &lambda_obj);
	}

private:
	static void ____ref_f (void *arg, uintptr_t argc, uintptr_t argv [])
	{
		ahpl_ref_lambda_f *lambda_obj = reinterpret_cast<ahpl_ref_lambda_f *>(argv [0]);
		(*lambda_obj) ((ahpl_ref_class *)arg);
	}

#ifdef __AHPL_MPQ_H__
public:
	typedef std::function <void (const ahpl_ts_t &queued_ts, ahpl_refobj_t robj)> ahpl_ref_mpq_lambda_f;

	/* MPQ encapsulations */
	int queue (ahpl_mpq_t tq, ahpl_mpq_t dq, const char *f_name, ahpl_ref_mpq_lambda_f&& task)
	{
		ahpl_ref_mpq_lambda_f *task_obj = new ahpl_ref_mpq_lambda_f (std::move (task));
		int err = ahpl_ref_class::queue (tq, dq, f_name, ____mpq_f, 1, task_obj);
		if (err < 0)
			delete task_obj;

		return err;
	}

	int call (ahpl_mpq_t q, const char *f_name, ahpl_ref_mpq_lambda_f&& task, void *task_result = NULL)
	{
		ahpl_ref_mpq_lambda_f *task_obj = new ahpl_ref_mpq_lambda_f (std::move (task));
		int err = ahpl_ref_class::call (q, f_name, ____mpq_f, 2, task_obj, task_result);
		if (err < 0)
			delete task_obj;

		return err;
	}

	int run (ahpl_mpq_t q, const char *f_name, ahpl_ref_mpq_lambda_f&& task)
	{
		ahpl_ref_mpq_lambda_f *task_obj = new ahpl_ref_mpq_lambda_f (std::move (task));
		int err = ahpl_ref_class::run (q, AHPL_MPQ_INVALID, f_name, ____mpq_f, 1, task_obj);
		if (err < 0)
			delete task_obj;

		return err;
	}

#ifdef __AHPL_MPQP_H__
	/* MPQP encapsulations */
	ahpl_mpq_t queue (ahpl_mpqp_t qp, ahpl_mpq_t dq, const char *f_name, ahpl_ref_mpq_lambda_f&& task)
	{
		ahpl_ref_mpq_lambda_f *task_obj = new ahpl_ref_mpq_lambda_f (std::move (task));
		ahpl_mpq_t qid = ahpl_ref_class::queue (qp, dq, f_name, ____mpq_f, 1, task_obj);
		if (ahpl_mpq_invalid (qid))
			delete task_obj;

		return qid;
	}

	ahpl_mpq_t call (ahpl_mpqp_t qp, const char *f_name, ahpl_ref_mpq_lambda_f&& task, void *task_result = NULL)
	{
		ahpl_ref_mpq_lambda_f *task_obj = new ahpl_ref_mpq_lambda_f (std::move (task));
		ahpl_mpq_t qid = ahpl_ref_class::call (qp, f_name, ____mpq_f, 2, task_obj, task_result);
		if (ahpl_mpq_invalid (qid))
			delete task_obj;

		return qid;
	}

	ahpl_mpq_t run (ahpl_mpqp_t qp, const char *f_name, ahpl_ref_mpq_lambda_f&& task)
	{
		ahpl_ref_mpq_lambda_f *task_obj = new ahpl_ref_mpq_lambda_f (std::move (task));
		ahpl_mpq_t qid = ahpl_ref_class::run (qp, AHPL_MPQ_INVALID, f_name, ____mpq_f, 1, task_obj);
		if (ahpl_mpq_invalid (qid))
			delete task_obj;

		return qid;
	}

	int pool_tail_queue (ahpl_mpqp_t qp, ahpl_mpq_t dq, const char *f_name, ahpl_ref_mpq_lambda_f&& task)
	{
		ahpl_ref_mpq_lambda_f *task_obj = new ahpl_ref_mpq_lambda_f (std::move (task));
		int err = ahpl_ref_class::pool_tail_queue (qp, dq, f_name, ____mpq_f, 1, task_obj);
		if (err < 0)
			delete task_obj;

		return err;
	}
#endif /* __AHPL_MPQP_H__ */

	/* MPQ with specified ref encapsulations */
	static int queue (ahpl_mpq_t tq, ahpl_mpq_t dq, ahpl_ref_t ref, const char *f_name, ahpl_ref_mpq_lambda_f&& task)
	{
		ahpl_ref_mpq_lambda_f *task_obj = new ahpl_ref_mpq_lambda_f (std::move (task));
		int err = ahpl_mpq_queue (tq, dq, ref, f_name, ____mpq_f, 1, task_obj);
		if (err < 0)
			delete task_obj;

		return err;
	}

	static int call (ahpl_mpq_t q, ahpl_ref_t ref, const char *f_name, ahpl_ref_mpq_lambda_f&& task, void *task_result = NULL)
	{
		ahpl_ref_mpq_lambda_f *task_obj = new ahpl_ref_mpq_lambda_f (std::move (task));
		int err = ahpl_mpq_call (q, ref, f_name, ____mpq_f, 2, task_obj, task_result);
		if (err < 0)
			delete task_obj;

		return err;
	}

	static int run (ahpl_mpq_t q, ahpl_ref_t ref, const char *f_name, ahpl_ref_mpq_lambda_f&& task)
	{
		ahpl_ref_mpq_lambda_f *task_obj = new ahpl_ref_mpq_lambda_f (std::move (task));
		int err = ahpl_mpq_run (q, AHPL_MPQ_INVALID, ref, f_name, ____mpq_f, 1, task_obj);
		if (err < 0)
			delete task_obj;

		return err;
	}

#ifdef __AHPL_MPQP_H__
	/* MPQP with specified ref encapsulations */
	static ahpl_mpq_t queue (ahpl_mpqp_t qp, ahpl_mpq_t dq, ahpl_ref_t ref, const char *f_name, ahpl_ref_mpq_lambda_f&& task)
	{
		ahpl_ref_mpq_lambda_f *task_obj = new ahpl_ref_mpq_lambda_f (std::move (task));
		ahpl_mpq_t qid = ahpl_mpqp_queue (qp, dq, ref, f_name, ____mpq_f, 1, task_obj);
		if (ahpl_mpq_invalid (qid))
			delete task_obj;

		return qid;
	}

	static ahpl_mpq_t call (ahpl_mpqp_t qp, ahpl_ref_t ref, const char *f_name, ahpl_ref_mpq_lambda_f&& task, void *task_result = NULL)
	{
		ahpl_ref_mpq_lambda_f *task_obj = new ahpl_ref_mpq_lambda_f (std::move (task));
		ahpl_mpq_t qid = ahpl_mpqp_call (qp, ref, f_name, ____mpq_f, 2, task_obj, task_result);
		if (ahpl_mpq_invalid (qid))
			delete task_obj;

		return qid;
	}

	static ahpl_mpq_t run (ahpl_mpqp_t qp, ahpl_ref_t ref, const char *f_name, ahpl_ref_mpq_lambda_f&& task)
	{
		ahpl_ref_mpq_lambda_f *task_obj = new ahpl_ref_mpq_lambda_f (std::move (task));
		ahpl_mpq_t qid = ahpl_mpqp_run (qp, AHPL_MPQ_INVALID, ref, f_name, ____mpq_f, 1, task_obj);
		if (ahpl_mpq_invalid (qid))
			delete task_obj;

		return qid;
	}

	static int pool_tail_queue (ahpl_mpqp_t qp, ahpl_mpq_t dq, ahpl_ref_t ref, const char *f_name, ahpl_ref_mpq_lambda_f&& task)
	{
		ahpl_ref_mpq_lambda_f *task_obj = new ahpl_ref_mpq_lambda_f (std::move (task));
		int err = ahpl_mpqp_pool_tail_queue (qp, dq, ref, f_name, ____mpq_f, 1, task_obj);
		if (err < 0)
			delete task_obj;

		return err;
	}
#endif /* __AHPL_MPQP_H__ */

	static void *call_result_var_addr (void)
	{
		void *var_addr;

		if (ahpl_mpq_run_func_arg (1, (uintptr_t *)&var_addr) < 0)
			return NULL;

		return var_addr;
	}

private:
	static void ____mpq_f (const ahpl_ts_t *queued_ts_p, ahpl_refobj_t robj, uintptr_t argc, uintptr_t argv [])
	{
		ahpl_ref_mpq_lambda_f *task_obj = reinterpret_cast<ahpl_ref_mpq_lambda_f *>(argv [0]);
		ahpl_mpq_t done_qid = ahpl_mpq_run_func_done_qid ();
		(*task_obj) (*queued_ts_p, robj);
		if (ahpl_mpq_invalid (done_qid) || ahpl_is_free_only (robj)) {
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
#endif /* __AHPL_MPQ_H__ */

#ifdef __AHPL_ASYNC_H__
	/**
	 * The stackless coroutine like implementation in AHPL. We could not
	 * support the real stackless coroutine except in the language level,
	 * so we just provide similar equivalent functionals here.
	 **/
public:
	typedef std::function <int (int free_only)> ahpl_async_prepare_lambda_f;

	int prepare (ahpl_stack_id_t stack_id, const char *f_name, ahpl_async_prepare_lambda_f&& task)
	{
		ahpl_async_prepare_lambda_f *prepare_f = new ahpl_async_prepare_lambda_f (std::move (task));
		int err = ahpl_async_prepare (stack_id, ref (), f_name, ____async_prepare_f, 1, prepare_f);
		if (err < 0)
			delete prepare_f;

		return err;
	}

	static int prepare (ahpl_stack_id_t stack_id, ahpl_ref_t ref, const char *f_name, ahpl_async_prepare_lambda_f&& task)
	{
		ahpl_async_prepare_lambda_f *prepare_f = new ahpl_async_prepare_lambda_f (std::move (task));
		int err = ahpl_async_prepare (stack_id, ref, f_name, ____async_prepare_f, 1, prepare_f);
		if (err < 0)
			delete prepare_f;

		return err;
	}

private:
	static int ____async_prepare_f (int free_only, uintptr_t argc, uintptr_t argv [])
	{
		ahpl_async_prepare_lambda_f *prepare_f = reinterpret_cast<ahpl_async_prepare_lambda_f *>(argv [0]);
		int err;
		err = (*prepare_f) (free_only);
		delete prepare_f;
		return err;
	}

public:
	typedef std::function <void (int free_only)> ahpl_async_resume_lambda_f;

	int resume (ahpl_stack_id_t stack_id, const char *f_name, ahpl_async_resume_lambda_f&& task)
	{
		ahpl_async_resume_lambda_f *resume_f = new ahpl_async_resume_lambda_f (std::move (task));
		int err = ahpl_async_resume (stack_id, ref (), f_name, ____async_resume_f, 1, resume_f);
		if (err < 0)
			delete resume_f;

		return err;
	}

	static int resume (ahpl_stack_id_t stack_id, ahpl_ref_t ref, const char *f_name, ahpl_async_resume_lambda_f&& task)
	{
		ahpl_async_resume_lambda_f *resume_f = new ahpl_async_resume_lambda_f (std::move (task));
		int err = ahpl_async_resume (stack_id, ref, f_name, ____async_resume_f, 1, resume_f);
		if (err < 0)
			delete resume_f;

		return err;
	}

private:
	static void ____async_resume_f (int free_only, uintptr_t argc, uintptr_t argv [])
	{
		ahpl_async_resume_lambda_f *resume_f = reinterpret_cast<ahpl_async_resume_lambda_f *>(argv [0]);
		(*resume_f) (free_only);
		delete resume_f;
	}
#endif /* __AHPL_ASYNC_H__ */
#endif /* C++11 */
};


/**
 * The T_ref_cls argument of this template must be
 * ahpl_ref_class or its derivatives.
 **/
template<typename T_ref_cls>
class ahpl_ref_unique_ptr {
private:
	T_ref_cls *_ptr;

public:
	ahpl_ref_unique_ptr (): _ptr (NULL) {}
	ahpl_ref_unique_ptr (T_ref_cls *p): _ptr (p) {}

	ahpl_ref_unique_ptr &operator = (T_ref_cls *p)
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

	~ahpl_ref_unique_ptr ()
	{
		reset ();
	}

#if (__cplusplus >= 201103) || defined (_MSC_VER)
private:
	ahpl_ref_unique_ptr (const ahpl_ref_unique_ptr &) = delete;
	ahpl_ref_unique_ptr &operator = (const ahpl_ref_unique_ptr &) = delete;

public:
	ahpl_ref_unique_ptr (ahpl_ref_unique_ptr &&src): _ptr (src.release ()) {}
	ahpl_ref_unique_ptr &operator = (ahpl_ref_unique_ptr &&ptr)
	{
		reset (ptr.release ());
		return *this;
	}
#else
private:
	ahpl_ref_unique_ptr (const ahpl_ref_unique_ptr &);
	ahpl_ref_unique_ptr &operator = (const ahpl_ref_unique_ptr &);
#endif /* C++11 */
};


template<typename T_ref_cls>
inline bool operator == (const ahpl_ref_unique_ptr<T_ref_cls> &ptr, intptr_t _null)
{
	return ptr.get () == (T_ref_cls *)_null;
}

template<typename T_ref_cls>
inline bool operator != (const ahpl_ref_unique_ptr<T_ref_cls> &ptr, intptr_t _null)
{
	return ptr.get () != (T_ref_cls *)_null;
}

template<typename T_ref_cls>
inline bool operator == (intptr_t _null, const ahpl_ref_unique_ptr<T_ref_cls> &ptr)
{
	return (T_ref_cls *)_null == ptr.get ();
}

template<typename T_ref_cls>
inline bool operator != (intptr_t _null, const ahpl_ref_unique_ptr<T_ref_cls> &ptr)
{
	return  (T_ref_cls *)_null != ptr.get ();
}

#if (__cplusplus >= 201103) || defined (_MSC_VER)
template<typename T_ref_cls>
inline bool operator == (const ahpl_ref_unique_ptr<T_ref_cls> &ptr, nullptr_t)
{
	return !ptr;
}

template<typename T_ref_cls>
inline bool operator != (const ahpl_ref_unique_ptr<T_ref_cls> &ptr, nullptr_t)
{
	return ptr;
}

template<typename T_ref_cls>
inline bool operator == (nullptr_t, const ahpl_ref_unique_ptr<T_ref_cls> &ptr)
{
	return !ptr;
}

template<typename T_ref_cls>
inline bool operator != (nullptr_t, const ahpl_ref_unique_ptr<T_ref_cls> &ptr)
{
	return ptr;
}
#endif /* C++11 */


typedef ahpl_ref_unique_ptr<ahpl_ref_class> ahpl_ref_class_unique_ptr;


#endif /* __AHPL_REF_OBJ_CPP_H__ */