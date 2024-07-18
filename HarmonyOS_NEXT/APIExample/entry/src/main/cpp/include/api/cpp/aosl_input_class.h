/*************************************************************
 * Author:	Lionfore Hao (haolianfu@agora.io)
 * Date	 :	May 21st, 2019
 * Module:	AOSL input object for C++ definition file
 *
 *
 * This is a part of the Advanced Operating System Layer.
 * Copyright (C) 2018 ~ 2019 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#ifndef __AOSL_INPUT_CLASS_H__
#define __AOSL_INPUT_CLASS_H__


#include <stdlib.h>

#include <api/aosl_types.h>
#include <api/aosl_defs.h>
#include <api/aosl_mpq.h>
#include <api/aosl_ref.h>
#include <api/aosl_input.h>
#include <api/cpp/aosl_ref_class.h>

#if (__cplusplus >= 201103) || defined (_MSC_VER)
#include <functional>
#include <memory>
typedef std::function <void (aosl_refobj_t iobj, bool free_only)> aosl_input_lambda_f;
#endif


class aosl_input_class: public aosl_ref_class {
public:
	class aosl_ref_t_oop: public aosl_ref_class::aosl_ref_t_oop {
	private:
		/**
		 * We do not allow create any object of this class directly,
		 * only the pointer of this class is significant.
		 **/
		aosl_ref_t_oop ();

	public:
		static aosl_ref_t_oop *create (uintptr_t waitings_max, aosl_mpq_t input_q = aosl_mpq_main (), void *arg = NULL, aosl_ref_dtor_t dtor = NULL)
		{
			return (aosl_ref_t_oop *)aosl_input_create (arg, dtor, waitings_max, input_q);
		}

		static aosl_ref_t_oop *from_aosl_ref_t (aosl_ref_t ref)
		{
			return (aosl_ref_t_oop *)ref;
		}

		int exec (const char *f_name, aosl_input_func_t f, uintptr_t argc, ...)
		{
			va_list args;
			int err;

			va_start (args, argc);
			err = aosl_input_exec_args (ref (), f_name, f, argc, args);
			va_end (args);

			return err;
		}

		int exec_args (const char *f_name, aosl_input_func_t f, uintptr_t argc, va_list args)
		{
			return aosl_input_exec_args (ref (), f_name, f, argc, args);
		}

		int exec_argv (const char *f_name, aosl_input_func_t f, uintptr_t argc, uintptr_t argv [])
		{
			return aosl_input_exec_argv (ref (), f_name, f, argc, argv);
		}

		int waitings_count ()
		{
			return aosl_input_waitings_count (ref ());
		}

		int remove_waitings_head ()
		{
			return aosl_input_remove_waitings_head (ref ());
		}

		/* C++11 lambda encapsulations */
	#if (__cplusplus >= 201103) || defined (_MSC_VER)
	public:
		int exec (const char *f_name, aosl_input_lambda_f&& input)
		{
			aosl_input_lambda_f *input_obj = new aosl_input_lambda_f (std::move (input));
			int err = aosl_ref_t_oop::exec (f_name, ____input_f, 1, input_obj);
			if (err < 0)
				delete input_obj;

			return err;
		}

	private:
		static void ____input_f (aosl_refobj_t iobj, int free_only, uintptr_t argc, uintptr_t argv [])
		{
			aosl_input_lambda_f *input_obj = reinterpret_cast<aosl_input_lambda_f *>(argv [0]);
			(*input_obj) (iobj, (bool)(free_only != 0));
			delete input_obj;
		}
	#endif /* C++11 */
	};

public:
	aosl_input_class (uintptr_t waitings_max, aosl_mpq_t input_q = aosl_mpq_main ())
				: aosl_ref_class (aosl_ref_t_oop::create (waitings_max, input_q, this, __dtor))
	{
		if (aosl_ref_invalid (ref_oop ()))
			abort ();
	}

	aosl_ref_t_oop *ref_oop () const
	{
		return (aosl_ref_t_oop *)aosl_ref_class::ref_oop ();
	}

	int exec (const char *f_name, aosl_input_func_t f, uintptr_t argc, ...)
	{
		va_list args;
		int err;

		va_start (args, argc);
		err = ref_oop ()->exec_args (f_name, f, argc, args);
		va_end (args);

		return err;
	}

	int exec_args (const char *f_name, aosl_input_func_t f, uintptr_t argc, va_list args)
	{
		return ref_oop ()->exec_args (f_name, f, argc, args);
	}

	int exec_argv (const char *f_name, aosl_input_func_t f, uintptr_t argc, uintptr_t argv [])
	{
		return ref_oop ()->exec_argv (f_name, f, argc, argv);
	}

	int waitings_count ()
	{
		return ref_oop ()->waitings_count ();
	}

	int remove_waitings_head ()
	{
		return ref_oop ()->remove_waitings_head ();
	}

protected:
	/* We do not allow delete this object directly. */
	virtual ~aosl_input_class ()
	{
	}

private:
	static void __dtor (void *arg)
	{
		aosl_input_class *__this = (aosl_input_class *)arg;
		::delete __this;
	}

	/* C++11 lambda encapsulations */
#if (__cplusplus >= 201103) || defined (_MSC_VER)
public:
	int exec (const char *f_name, aosl_input_lambda_f&& input)
	{
		return ref_oop ()->exec (f_name, std::move (input));
	}
#endif /* C++11 */

#if (__cplusplus >= 201103) || defined (_MSC_VER)
private:
	aosl_input_class (const aosl_input_class &) = delete;
	aosl_input_class (aosl_input_class &&) = delete;
	aosl_input_class &operator = (const aosl_input_class &) = delete;
	aosl_input_class &operator = (aosl_input_class &&) = delete;
#else
private:
	aosl_input_class (const aosl_input_class &);
	aosl_input_class &operator = (const aosl_input_class &);
#endif /* C++11 */
};


typedef aosl_ref_unique_ptr<aosl_input_class> aosl_input_class_unique_ptr;


#endif /* __AOSL_INPUT_CLASS_H__ */