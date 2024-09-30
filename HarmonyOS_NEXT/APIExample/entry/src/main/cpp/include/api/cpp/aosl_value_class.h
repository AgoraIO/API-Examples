/*************************************************************
 * Author:	Lionfore Hao (haolianfu@agora.io)
 * Date	 :	Dec 8th, 2022
 * Module:	AOSL value object for C++ definition file
 *
 *
 * This is a part of the Advanced Operating System Layer.
 * Copyright (C) 2018 ~ 2022 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#ifndef __AOSL_VALUE_OBJ_CPP_H__
#define __AOSL_VALUE_OBJ_CPP_H__


#include <stdlib.h>

#include <api/aosl_types.h>
#include <api/aosl_value.h>
#include <api/cpp/aosl_kobj_class.h>

#if (__cplusplus >= 201103) || defined (_MSC_VER)
#include <functional>
#include <memory>
#endif


class aosl_value_base: public aosl_kobj_class {
private:
#if (__cplusplus >= 201103) || defined (_MSC_VER)
	aosl_value_base () = delete;
	aosl_value_base (const aosl_value_base &) = delete;
	aosl_value_base (aosl_value_base &&) = delete;
	aosl_value_base &operator = (const aosl_value_base &) = delete;
	aosl_value_base &operator = (aosl_value_base &&) = delete;
	~aosl_value_base () = delete;
#else
	aosl_value_base ();
	aosl_value_base (const aosl_value_base &);
	aosl_value_base &operator = (const aosl_value_base &);
	~aosl_value_base ();
#endif

public:
	static ssize_t get (const char *key, aosl_value_t &val, aosl_kfd_t dfd = AOSL_KFD_INVALID)
	{
		aosl_value_init (&val);
		return aosl_value_get (dfd, key, &val);
	}

	static ssize_t set (const char *key, const aosl_value_t &val, aosl_kfd_t dfd = AOSL_KFD_INVALID)
	{
		return aosl_value_set (dfd, key, &val);
	}

public:
	static void *verify_register (aosl_kfd_t dfd, const char *name, aosl_value_verify_t f, aosl_value_verify_dtor_t dtor, uintptr_t argc, ...)
	{
		va_list args;
		void *err;

		va_start (args, argc);
		err = aosl_value_verify_register_args (dfd, name, f, dtor, argc, args);
		va_end (args);
		return err;
	}

	static void *verify_register_args (aosl_kfd_t dfd, const char *name, aosl_value_verify_t f, aosl_value_verify_dtor_t dtor, uintptr_t argc, va_list args)
	{
		return aosl_value_verify_register_args (dfd, name, f, dtor, argc, args);
	}

	static void *verify_register_argv (aosl_kfd_t dfd, const char *name, aosl_value_verify_t f, aosl_value_verify_dtor_t dtor, uintptr_t argc, uintptr_t argv [])
	{
		return aosl_value_verify_register_argv (dfd, name, f, dtor, argc, argv);
	}

	static int verify_unregister (aosl_kfd_t dfd, const char *name, void *handle)
	{
		return aosl_value_verify_unregister (dfd, name, handle);
	}

	/* C++11 lambda encapsulations */
#if (__cplusplus >= 201103) || defined (_MSC_VER)
public:
	typedef std::function <int (const aosl_value_t *val)> aosl_value_verify_lambda_f;
	static void *verify_register (aosl_kfd_t dfd, const char *name, aosl_value_verify_lambda_f&& task)
	{
		aosl_value_verify_lambda_f *task_obj = new aosl_value_verify_lambda_f (std::move (task));
		void *err = verify_register (dfd, name, ____verify_f, ____verify_dtor, 1, task_obj);
		if (err == NULL)
			delete task_obj;

		return err;
	}

private:
	static int ____verify_f (const aosl_value_t *val, uintptr_t argc, uintptr_t argv [])
	{
		aosl_value_verify_lambda_f *task_obj = reinterpret_cast<aosl_value_verify_lambda_f *>(argv [0]);
		return (*task_obj) (val);
	}

	static void ____verify_dtor (uintptr_t argc, uintptr_t argv [])
	{
		aosl_value_verify_lambda_f *task_obj = reinterpret_cast<aosl_value_verify_lambda_f *>(argv [0]);
		delete task_obj;
	}
#endif /* C++11 */
};

class aosl_value_bool: public aosl_value_base {
private:
#if (__cplusplus >= 201103) || defined (_MSC_VER)
	aosl_value_bool () = delete;
	aosl_value_bool (const aosl_value_bool &) = delete;
	aosl_value_bool (aosl_value_bool &&) = delete;
	aosl_value_bool &operator = (const aosl_value_bool &) = delete;
	aosl_value_bool &operator = (aosl_value_bool &&) = delete;
	~aosl_value_bool () = delete;
#else
	aosl_value_bool ();
	aosl_value_bool (const aosl_value_bool &);
	aosl_value_bool &operator = (const aosl_value_bool &);
	~aosl_value_bool ();
#endif

public:
	static bool get (const char *key, aosl_kfd_t dfd = AOSL_KFD_INVALID, bool error_val = false)
	{
		int8_t v;
		if (aosl_value_get_int8 (dfd, key, &v) != sizeof v)
			return error_val;

		return (bool)(v != 0);
	}

	static ssize_t set (const char *key, bool v, aosl_kfd_t dfd = AOSL_KFD_INVALID, uint32_t weight = 0)
	{
		return aosl_value_set_int8 (dfd, key, weight, (int8_t)v);
	}
};

class aosl_value_int8: public aosl_value_base {
private:
#if (__cplusplus >= 201103) || defined (_MSC_VER)
	aosl_value_int8 () = delete;
	aosl_value_int8 (const aosl_value_int8 &) = delete;
	aosl_value_int8 (aosl_value_int8 &&) = delete;
	aosl_value_int8 &operator = (const aosl_value_int8 &) = delete;
	aosl_value_int8 &operator = (aosl_value_int8 &&) = delete;
	~aosl_value_int8 () = delete;
#else
	aosl_value_int8 ();
	aosl_value_int8 (const aosl_value_int8 &);
	aosl_value_int8 &operator = (const aosl_value_int8 &);
	~aosl_value_int8 ();
#endif

public:
	static int8_t get (const char *key, aosl_kfd_t dfd = AOSL_KFD_INVALID, int8_t error_val = 0)
	{
		int8_t v;
		if (aosl_value_get_int8 (dfd, key, &v) != sizeof v)
			return error_val;

		return v;
	}

	static ssize_t set (const char *key, int8_t v, aosl_kfd_t dfd = AOSL_KFD_INVALID, uint32_t weight = 0)
	{
		return aosl_value_set_int8 (dfd, key, weight, v);
	}
};

class aosl_value_int16: public aosl_value_base {
private:
#if (__cplusplus >= 201103) || defined (_MSC_VER)
	aosl_value_int16 () = delete;
	aosl_value_int16 (const aosl_value_int16 &) = delete;
	aosl_value_int16 (aosl_value_int16 &&) = delete;
	aosl_value_int16 &operator = (const aosl_value_int16 &) = delete;
	aosl_value_int16 &operator = (aosl_value_int16 &&) = delete;
	~aosl_value_int16 () = delete;
#else
	aosl_value_int16 ();
	aosl_value_int16 (const aosl_value_int16 &);
	aosl_value_int16 &operator = (const aosl_value_int16 &);
	~aosl_value_int16 ();
#endif

public:
	static int16_t get (const char *key, aosl_kfd_t dfd = AOSL_KFD_INVALID, int16_t error_val = 0)
	{
		int16_t v;
		if (aosl_value_get_int16 (dfd, key, &v) != sizeof v)
			return error_val;

		return v;
	}

	static ssize_t set (const char *key, int16_t v, aosl_kfd_t dfd = AOSL_KFD_INVALID, uint32_t weight = 0)
	{
		return aosl_value_set_int16 (dfd, key, weight, v);
	}
};

class aosl_value_int32: public aosl_value_base {
private:
#if (__cplusplus >= 201103) || defined (_MSC_VER)
	aosl_value_int32 () = delete;
	aosl_value_int32 (const aosl_value_int32 &) = delete;
	aosl_value_int32 (aosl_value_int32 &&) = delete;
	aosl_value_int32 &operator = (const aosl_value_int32 &) = delete;
	aosl_value_int32 &operator = (aosl_value_int32 &&) = delete;
	~aosl_value_int32 () = delete;
#else
	aosl_value_int32 ();
	aosl_value_int32 (const aosl_value_int32 &);
	aosl_value_int32 &operator = (const aosl_value_int32 &);
	~aosl_value_int32 ();
#endif

public:
	static int32_t get (const char *key, aosl_kfd_t dfd = AOSL_KFD_INVALID, int32_t error_val = 0)
	{
		int32_t v;
		if (aosl_value_get_int32 (dfd, key, &v) != sizeof v)
			return error_val;

		return v;
	}

	static ssize_t set (const char *key, int32_t v, aosl_kfd_t dfd = AOSL_KFD_INVALID, uint32_t weight = 0)
	{
		return aosl_value_set_int32 (dfd, key, weight, v);
	}
};

class aosl_value_int64: public aosl_value_base {
private:
#if (__cplusplus >= 201103) || defined (_MSC_VER)
	aosl_value_int64 () = delete;
	aosl_value_int64 (const aosl_value_int64 &) = delete;
	aosl_value_int64 (aosl_value_int64 &&) = delete;
	aosl_value_int64 &operator = (const aosl_value_int64 &) = delete;
	aosl_value_int64 &operator = (aosl_value_int64 &&) = delete;
	~aosl_value_int64 () = delete;
#else
	aosl_value_int64 ();
	aosl_value_int64 (const aosl_value_int64 &);
	aosl_value_int64 &operator = (const aosl_value_int64 &);
	~aosl_value_int64 ();
#endif

public:
	static int64_t get (const char *key, aosl_kfd_t dfd = AOSL_KFD_INVALID, int64_t error_val = 0)
	{
		int64_t v;
		if (aosl_value_get_int64 (dfd, key, &v) != sizeof v)
			return error_val;

		return v;
	}

	static ssize_t set (const char *key, int64_t v, aosl_kfd_t dfd = AOSL_KFD_INVALID, uint32_t weight = 0)
	{
		return aosl_value_set_int64 (dfd, key, weight, v);
	}
};

class aosl_value_intptr: public aosl_value_base {
private:
#if (__cplusplus >= 201103) || defined (_MSC_VER)
	aosl_value_intptr () = delete;
	aosl_value_intptr (const aosl_value_intptr &) = delete;
	aosl_value_intptr (aosl_value_intptr &&) = delete;
	aosl_value_intptr &operator = (const aosl_value_intptr &) = delete;
	aosl_value_intptr &operator = (aosl_value_intptr &&) = delete;
	~aosl_value_intptr () = delete;
#else
	aosl_value_intptr ();
	aosl_value_intptr (const aosl_value_intptr &);
	aosl_value_intptr &operator = (const aosl_value_intptr &);
	~aosl_value_intptr ();
#endif

public:
	static intptr_t get (const char *key, aosl_kfd_t dfd = AOSL_KFD_INVALID, intptr_t error_val = 0)
	{
		intptr_t v;
		if (aosl_value_get_intptr (dfd, key, &v) != sizeof v)
			return error_val;

		return v;
	}

	static ssize_t set (const char *key, intptr_t v, aosl_kfd_t dfd = AOSL_KFD_INVALID, uint32_t weight = 0)
	{
		return aosl_value_set_intptr (dfd, key, weight, v);
	}
};

class aosl_value_pointer: public aosl_value_base {
private:
#if (__cplusplus >= 201103) || defined (_MSC_VER)
	aosl_value_pointer () = delete;
	aosl_value_pointer (const aosl_value_pointer &) = delete;
	aosl_value_pointer (aosl_value_pointer &&) = delete;
	aosl_value_pointer &operator = (const aosl_value_pointer &) = delete;
	aosl_value_pointer &operator = (aosl_value_pointer &&) = delete;
	~aosl_value_pointer () = delete;
#else
	aosl_value_pointer ();
	aosl_value_pointer (const aosl_value_pointer &);
	aosl_value_pointer &operator = (const aosl_value_pointer &);
	~aosl_value_pointer ();
#endif

public:
	static void *get (const char *key, aosl_kfd_t dfd = AOSL_KFD_INVALID, void *error_val = NULL)
	{
		void *v;
		if (aosl_value_get_pointer (dfd, key, &v) != sizeof v)
			return error_val;

		return v;
	}

	static ssize_t set (const char *key, void *v, aosl_kfd_t dfd = AOSL_KFD_INVALID, uint32_t weight = 0)
	{
		return aosl_value_set_pointer (dfd, key, weight, v);
	}
};

class aosl_value_float: public aosl_value_base {
private:
#if (__cplusplus >= 201103) || defined (_MSC_VER)
	aosl_value_float () = delete;
	aosl_value_float (const aosl_value_float &) = delete;
	aosl_value_float (aosl_value_float &&) = delete;
	aosl_value_float &operator = (const aosl_value_float &) = delete;
	aosl_value_float &operator = (aosl_value_float &&) = delete;
	~aosl_value_float () = delete;
#else
	aosl_value_float ();
	aosl_value_float (const aosl_value_float &);
	aosl_value_float &operator = (const aosl_value_float &);
	~aosl_value_float ();
#endif

public:
	static float get (const char *key, aosl_kfd_t dfd = AOSL_KFD_INVALID, float error_val = 1.0)
	{
		float v;
		if (aosl_value_get_float (dfd, key, &v) != sizeof v)
			return error_val;

		return v;
	}

	static ssize_t set (const char *key, float v, aosl_kfd_t dfd = AOSL_KFD_INVALID, uint32_t weight = 0)
	{
		return aosl_value_set_float (dfd, key, weight, v);
	}
};

class aosl_value_double: public aosl_value_base {
private:
#if (__cplusplus >= 201103) || defined (_MSC_VER)
	aosl_value_double () = delete;
	aosl_value_double (const aosl_value_double &) = delete;
	aosl_value_double (aosl_value_double &&) = delete;
	aosl_value_double &operator = (const aosl_value_double &) = delete;
	aosl_value_double &operator = (aosl_value_double &&) = delete;
	~aosl_value_double () = delete;
#else
	aosl_value_double ();
	aosl_value_double (const aosl_value_double &);
	aosl_value_double &operator = (const aosl_value_double &);
	~aosl_value_double ();
#endif

public:
	static double get (const char *key, aosl_kfd_t dfd = AOSL_KFD_INVALID, double error_val = 1.0)
	{
		double v;
		if (aosl_value_get_double (dfd, key, &v) != sizeof v)
			return error_val;

		return v;
	}

	static ssize_t set (const char *key, double v, aosl_kfd_t dfd = AOSL_KFD_INVALID, uint32_t weight = 0)
	{
		return aosl_value_set_double (dfd, key, weight, v);
	}
};

class aosl_value_string: public aosl_value_base {
private:
#if (__cplusplus >= 201103) || defined (_MSC_VER)
	aosl_value_string () = delete;
	aosl_value_string (const aosl_value_string &) = delete;
	aosl_value_string (aosl_value_string &&) = delete;
	aosl_value_string &operator = (const aosl_value_string &) = delete;
	aosl_value_string &operator = (aosl_value_string &&) = delete;
	~aosl_value_string () = delete;
#else
	aosl_value_string ();
	aosl_value_string (const aosl_value_string &);
	aosl_value_string &operator = (const aosl_value_string &);
	~aosl_value_string ();
#endif

public:
	static ssize_t get (const char *key, char *str, size_t len, aosl_kfd_t dfd = AOSL_KFD_INVALID)
	{
		return aosl_value_get_string (dfd, key, str, len);
	}

	static ssize_t set (const char *key, const char *str, aosl_kfd_t dfd = AOSL_KFD_INVALID, uint32_t weight = 0)
	{
		return aosl_value_set_string (dfd, key, weight, str);
	}
};

class aosl_value_data: public aosl_value_base {
private:
#if (__cplusplus >= 201103) || defined (_MSC_VER)
	aosl_value_data () = delete;
	aosl_value_data (const aosl_value_data &) = delete;
	aosl_value_data (aosl_value_data &&) = delete;
	aosl_value_data &operator = (const aosl_value_data &) = delete;
	aosl_value_data &operator = (aosl_value_data &&) = delete;
	~aosl_value_data () = delete;
#else
	aosl_value_data ();
	aosl_value_data (const aosl_value_data &);
	aosl_value_data &operator = (const aosl_value_data &);
	~aosl_value_data ();
#endif

public:
	static ssize_t get (const char *key, void *buf, size_t len, aosl_kfd_t dfd = AOSL_KFD_INVALID)
	{
		return aosl_value_get_data (dfd, key, buf, len);
	}

	static ssize_t set (const char *key, const void *buf, size_t len, aosl_kfd_t dfd = AOSL_KFD_INVALID, uint32_t weight = 0)
	{
		return aosl_value_set_data (dfd, key, weight, buf, len);
	}
};

class aosl_value_class: public aosl_value_t {
public:
	aosl_value_class ()
	{
		aosl_value_init (this);
	}

	~aosl_value_class ()
	{
		aosl_value_free (this);
	}
};


#endif /* __AOSL_VALUE_OBJ_CPP_H__ */