/*************************************************************
 * Author:	Lionfore Hao (haolianfu@agora.io)
 * Date	 :	May 6th, 2024
 * Module:	AOSL software bus relative API C++ definition file
 *
 *
 * This is a part of the Advanced Operating System Layer.
 * Copyright (C) 2018 ~ 2024 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#ifndef __AOSL_SBUS_CPP_H__
#define __AOSL_SBUS_CPP_H__


/* C++11 lambda encapsulations */
#if (__cplusplus >= 201103) || defined (_MSC_VER)
#include <stdlib.h>

#include <api/aosl_types.h>
#include <api/aosl_mpq.h>
#include <api/aosl_ref.h>
#include <api/aosl_data.h>
#include <api/aosl_sbus.h>
#include <api/cpp/aosl_data_class.h>

#include <functional>
#include <memory>


class aosl_sbus_class_base {
private:
	aosl_sbus_class_base () = delete;
	aosl_sbus_class_base (const aosl_sbus_class_base &) = delete;
	aosl_sbus_class_base (aosl_sbus_class_base &&) = delete;
	aosl_sbus_class_base &operator = (const aosl_sbus_class_base &) = delete;
	aosl_sbus_class_base &operator = (aosl_sbus_class_base &&) = delete;
	~aosl_sbus_class_base () = delete;

public:
	typedef std::function <void (aosl_refobj_t robj, const char *name, aosl_data_t data)> aosl_sbus_data_lambda_f;
	/* simple version without robj arg, so it means no "free only" for this callback */
	typedef std::function <void (const char *name, aosl_data_t data)> aosl_sbus_data_lambda_2args_f;

	static void *action_add (const char *name, aosl_mpq_t qid, aosl_ref_t ref, uintptr_t max, const char *f_name, aosl_sbus_data_lambda_f&& task)
	{
		aosl_sbus_data_lambda_f *task_obj = new aosl_sbus_data_lambda_f (std::move (task));
		void *err = aosl_sbus_action_add (name, qid, ref, max, f_name, ____sbus_data, ____action_dtor, 1, task_obj);
		if (err == NULL)
			delete task_obj;

		return err;
	}

	static void *action_add (const char *name, aosl_mpq_t qid, aosl_ref_t ref, uintptr_t max, const char *f_name, aosl_sbus_data_lambda_2args_f&& task)
	{
		aosl_sbus_data_lambda_2args_f *task_obj = new aosl_sbus_data_lambda_2args_f (std::move (task));
		void *err = aosl_sbus_action_add (name, qid, ref, max, f_name, ____sbus_data_2args, ____action_dtor_2args, 1, task_obj);
		if (err == NULL)
			delete task_obj;

		return err;
	}

	static int action_del (const char *name, void *h_action)
	{
		return aosl_sbus_action_del (name, h_action);
	}

	static int send (const char *name, aosl_data_class &data)
	{
		return aosl_sbus_send (name, data);
	}

private:
	static void ____sbus_data (aosl_refobj_t robj, const char *name, aosl_data_t data, uintptr_t argc, uintptr_t argv [])
	{
		aosl_sbus_data_lambda_f *task_obj = reinterpret_cast<aosl_sbus_data_lambda_f *>(argv [0]);
		(*task_obj) (robj, name, data);
	}

	static void ____action_dtor (uintptr_t argc, uintptr_t argv [])
	{
		aosl_sbus_data_lambda_f *task_obj = reinterpret_cast<aosl_sbus_data_lambda_f *>(argv [0]);
		delete task_obj;
	}

	static void ____sbus_data_2args (aosl_refobj_t robj, const char *name, aosl_data_t data, uintptr_t argc, uintptr_t argv [])
	{
		if (!aosl_is_free_only (robj)) {
			aosl_sbus_data_lambda_2args_f *task_obj = reinterpret_cast<aosl_sbus_data_lambda_2args_f *>(argv [0]);
			(*task_obj) (name, data);
		}
	}

	static void ____action_dtor_2args (uintptr_t argc, uintptr_t argv [])
	{
		aosl_sbus_data_lambda_2args_f *task_obj = reinterpret_cast<aosl_sbus_data_lambda_2args_f *>(argv [0]);
		delete task_obj;
	}
};

/**
 * Template class for accessing a software bus with the data var format
 * specified by type T.
 **/
template<typename T>
class aosl_sbus_class: public aosl_sbus_class_base {
private:
	aosl_sbus_class () = delete;
	aosl_sbus_class (const aosl_sbus_class &) = delete;
	aosl_sbus_class (aosl_sbus_class &&) = delete;
	aosl_sbus_class &operator = (const aosl_sbus_class &) = delete;
	aosl_sbus_class &operator = (aosl_sbus_class &&) = delete;
	~aosl_sbus_class () = delete;

public:
	typedef std::function <void (aosl_refobj_t robj, const char *name, aosl_data_var<T> &var)> aosl_sbus_var_lambda_f;
	/* simple version without robj arg, so it means no "free only" for this callback */
	typedef std::function <void (const char *name, aosl_data_var<T> &var)> aosl_sbus_var_lambda_2args_f;

	static void *action_add (const char *name, aosl_mpq_t qid, aosl_ref_t ref, uintptr_t max, const char *f_name, aosl_sbus_var_lambda_f&& task)
	{
		aosl_sbus_var_lambda_f *task_obj = new aosl_sbus_var_lambda_f (std::move (task));
		void *err = aosl_sbus_action_add (name, qid, ref, max, f_name, ____sbus_var, ____action_var_dtor, 1, task_obj);
		if (err == NULL)
			delete task_obj;

		return err;
	}

	static void *action_add (const char *name, aosl_mpq_t qid, aosl_ref_t ref, uintptr_t max, const char *f_name, aosl_sbus_var_lambda_2args_f&& task)
	{
		aosl_sbus_var_lambda_2args_f *task_obj = new aosl_sbus_var_lambda_2args_f (std::move (task));
		void *err = aosl_sbus_action_add (name, qid, ref, max, f_name, ____sbus_var_2args, ____action_var_dtor_2args, 1, task_obj);
		if (err == NULL)
			delete task_obj;

		return err;
	}

	static int send (const char *name, aosl_data_var<T> &var)
	{
		return aosl_sbus_send (name, var);
	}

private:
	static void ____sbus_var (aosl_refobj_t robj, const char *name, aosl_data_t data, uintptr_t argc, uintptr_t argv [])
	{
		aosl_sbus_var_lambda_f *task_obj = reinterpret_cast<aosl_sbus_var_lambda_f *>(argv [0]);
		aosl_data_var<T> var ((aosl_data_arg<aosl_data_t> (data)));
		(*task_obj) (robj, name, var);
	}

	static void ____action_var_dtor (uintptr_t argc, uintptr_t argv [])
	{
		aosl_sbus_var_lambda_f *task_obj = reinterpret_cast<aosl_sbus_var_lambda_f *>(argv [0]);
		delete task_obj;
	}

	static void ____sbus_var_2args (aosl_refobj_t robj, const char *name, aosl_data_t data, uintptr_t argc, uintptr_t argv [])
	{
		if (!aosl_is_free_only (robj)) {
			aosl_sbus_var_lambda_2args_f *task_obj = reinterpret_cast<aosl_sbus_var_lambda_2args_f *>(argv [0]);
			aosl_data_var<T> var ((aosl_data_arg<aosl_data_t> (data)));
			(*task_obj) (name, var);
		}
	}

	static void ____action_var_dtor_2args (uintptr_t argc, uintptr_t argv [])
	{
		aosl_sbus_var_lambda_2args_f *task_obj = reinterpret_cast<aosl_sbus_var_lambda_2args_f *>(argv [0]);
		delete task_obj;
	}
};
#endif /* C++11 */


#endif /* __AOSL_SBUS_CPP_H__ */