/*************************************************************
 * Author:	Lionfore Hao (haolianfu@agora.io)
 * Date	 :	Sep 4th, 2023
 * Module:	AOSL common data for C++ definition file
 *
 *
 * This is a part of the Advanced Operating System Layer.
 * Copyright (C) 2018 ~ 2023 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#ifndef __AOSL_DATA_CLASS_H__
#define __AOSL_DATA_CLASS_H__


#include <stdlib.h>
#include <string.h>

#include <api/aosl_types.h>
#include <api/aosl_defs.h>
#include <api/aosl_data.h>


class aosl_data_class {
protected:
	aosl_data_t d;

public:
	aosl_data_class (void)
	{
		d = NULL;
	}

	aosl_data_class (const aosl_data_class &src)
	{
		if (this != &src) {
			d = src.d;
			if (d != NULL)
				aosl_data_get (d);
		}
	}

	aosl_data_class &operator = (const aosl_data_class &src)
	{
		if (this != &src) {
			if (d != NULL)
				aosl_data_put (d);

			d = src.d;
			if (d != NULL)
				aosl_data_get (d);
		}

		return *this;
	}

	operator aosl_data_t () const
	{
		return d;
	}

	void *ptr () const
	{
		if (d != NULL)
			return aosl_data_ptr_get (d);

		return NULL;
	}

	size_t len () const
	{
		if (d != NULL)
			return aosl_data_len (d);

		return 0;
	}

	~aosl_data_class ()
	{
		if (d != NULL)
			aosl_data_put (d);
	}

#if (__cplusplus >= 201103) || defined (_MSC_VER)
	aosl_data_class (aosl_data_class &&src)
	{
		if (this != &src) {
			d = src.d;
			src.d = NULL;
		}
	}

	aosl_data_class &operator = (aosl_data_class &&src)
	{
		if (this != &src) {
			if (d != NULL)
				aosl_data_put (d);

			d = src.d;
			src.d = NULL;
		}

		return *this;
	}
#endif /* C++11 */
};

class aosl_data_buf: public aosl_data_class {
public:
	aosl_data_buf (void)
	{
	}

	aosl_data_buf (size_t sz)
	{
		d = aosl_data_create (sz, NULL);
		if (d == NULL)
			abort ();
	}

	aosl_data_buf (const void *src, size_t len)
	{
		d = aosl_data_create (len, NULL);
		if (d == NULL)
			abort ();

		if (len > 0)
			memcpy (ptr (), src, len);
	}

	aosl_data_buf (void *ptr, size_t len, aosl_data_user_free_t dtor)
	{
		d = aosl_data_user_create (ptr, len, dtor);
		if (d == NULL)
			abort ();
	}

	aosl_data_buf (const char *str)
	{
		if (str != NULL) {
			size_t str_l = strlen (str);
			d = aosl_data_create (str_l + 1, NULL);
			if (d == NULL)
				abort ();

			memcpy (ptr (), str, str_l + 1);
		}
	}

	aosl_data_buf (char *str, aosl_data_user_free_t dtor)
	{
		size_t len;
		if (str != NULL) {
			len = strlen (str) + 1;
		} else {
			len = 0;
		}

		d = aosl_data_user_create (str, len, dtor);
		if (d == NULL)
			abort ();
	}

	aosl_data_buf (aosl_data_t src)
	{
		d = src;
		if (src != NULL)
			aosl_data_get (src);
	}

	aosl_data_buf (const aosl_data_buf &src)
	{
		if (this != &src) {
			d = src.d;
			if (d != NULL)
				aosl_data_get (d);
		}
	}

	aosl_data_buf &operator = (const char *str)
	{
		if (d != NULL) {
			aosl_data_put (d);
			d = NULL;
		}

		if (str != NULL) {
			size_t str_l = strlen (str);
			d = aosl_data_create (str_l + 1, NULL);
			if (d == NULL)
				abort ();

			memcpy (ptr (), str, str_l + 1);
		}

		return *this;
	}

	aosl_data_buf &operator = (aosl_data_t src)
	{
		if (d != NULL)
			aosl_data_put (d);

		d = src;
		if (src != NULL)
			aosl_data_get (src);

		return *this;
	}

	aosl_data_buf &operator = (const aosl_data_buf &src)
	{
		if (this != &src) {
			if (d != NULL)
				aosl_data_put (d);

			d = src.d;
			if (d != NULL)
				aosl_data_get (d);
		}

		return *this;
	}

	operator void *() const
	{
		if (d != NULL)
			return ptr ();

		return NULL;
	}

	operator char *() const
	{
		return (char *)ptr ();
	}

	operator unsigned char *() const
	{
		return (unsigned char *)ptr ();
	}

	void attach (aosl_data_t src)
	{
		if (d != NULL)
			aosl_data_put (d);

		d = src;
	}

	aosl_data_t detach ()
	{
		aosl_data_t data = d;
		d = NULL;
		return data;
	}

#if (__cplusplus >= 201103) || defined (_MSC_VER)
	aosl_data_buf (aosl_data_buf &&src)
	{
		if (this != &src) {
			d = src.d;
			src.d = NULL;
		}
	}

	aosl_data_buf &operator = (aosl_data_buf &&src)
	{
		if (this != &src) {
			if (d != NULL)
				aosl_data_put (d);

			d = src.d;
			src.d = NULL;
		}

		return *this;
	}
#endif /* C++11 */
};

/**
 * This class is only used in the constructor of aosl_data_var
 * to differentiate constructor with variadic args, do not use
 * it in any other scenario.
 **/
template<typename T>
class aosl_data_arg {
private:
	const T var;

public:
	aosl_data_arg (T arg): var (arg) {}
	operator T ()
	{
		return var;
	}
};

template<typename T>
class aosl_data_var: public aosl_data_class {
public:
	aosl_data_var (void)
	{
		d = aosl_data_create (sizeof (T), __data_var_dtor);
		if (d == NULL)
			abort ();

		new (aosl_data_ptr_get (d)) T;
	}

	/**
	 * We prefer to using this kind of special class arg rather
	 * than naked aosl_data_t just for differentiating from the
	 * constructor with variadic args.
	 **/
	aosl_data_var (aosl_data_arg<aosl_data_t> src_arg)
	{
		aosl_data_t src = src_arg;
		if (src == NULL)
			abort ();

		/* The prerequisite is that the src must be a valid T data */
		if (aosl_data_len (src) < sizeof (T))
			abort ();

		aosl_data_get (src);
		d = src;
	}

	/**
	 * We prefer to using this kind of special class arg rather
	 * than naked size_t.
	 **/
	aosl_data_var (aosl_data_arg<size_t> nelems)
	{
		size_t count = nelems;
		if (count == 0)
			abort ();

		d = aosl_data_create (sizeof (T) * count, __data_var_dtor);
		if (d == NULL)
			abort ();

		new (aosl_data_ptr_get (d)) T [count];
	}

#if (__cplusplus >= 201103) || defined (_MSC_VER)
	template <typename... Targs>
	aosl_data_var (Targs... args)
	{
		d = aosl_data_create (sizeof (T), __data_var_dtor);
		if (d == NULL)
			abort ();

		new (aosl_data_ptr_get (d)) T (args...);
	}
#endif

	aosl_data_var (const aosl_data_var &src)
	{
		if (this != &src) {
			d = src.d;
			if (d != NULL)
				aosl_data_get (d);
		}
	}

	aosl_data_var &operator = (const aosl_data_var &src)
	{
		if (this != &src) {
			if (d != NULL)
				aosl_data_put (d);

			d = src.d;
			if (d != NULL)
				aosl_data_get (d);
		}

		return *this;
	}

	aosl_data_var &operator = (const T &v)
	{
		if (d == NULL)
			abort ();

		*(T *)aosl_data_ptr_get (d) = v;
		return *this;
	}

	operator T () const
	{
		if (d == NULL)
			abort ();

		return *(T *)aosl_data_ptr_get (d);
	}

	operator T& ()
	{
		if (d == NULL)
			abort ();

		return *(T *)aosl_data_ptr_get (d);
	}

	T *operator -> () const
	{
		if (d == NULL)
			abort ();

		return (T *)aosl_data_ptr_get (d);
	}

	T operator [] (size_t i) const
	{
		size_t count;
		T *arr;

		if (d == NULL)
			abort ();

		count = len () / sizeof (T);
		if (i >= count)
			abort ();

		arr = (T *)aosl_data_ptr_get (d);
		return arr [i];
	}

	T& operator [] (size_t i)
	{
		size_t count;
		T *arr;

		if (d == NULL)
			abort ();

		count = len () / sizeof (T);
		if (i >= count)
			abort ();

		arr = (T *)aosl_data_ptr_get (d);
		return arr [i];
	}

private:
	static void __data_var_dtor (void *ptr, size_t len)
	{
		size_t n = len / sizeof (T);
		T *o = (T *)ptr;
		T *e = o + n;
		while (e > o) {
			e--;
			e->~T ();
		}
	}

#if (__cplusplus >= 201103) || defined (_MSC_VER)
	aosl_data_var (aosl_data_var &&src)
	{
		if (this != &src) {
			d = src.d;
			src.d = NULL;
		}
	}

	aosl_data_var &operator = (aosl_data_var &&src)
	{
		if (this != &src) {
			if (d != NULL)
				aosl_data_put (d);

			d = src.d;
			src.d = NULL;
		}

		return *this;
	}
#endif /* C++11 */
};


#endif /* __AOSL_DATA_CLASS_H__ */