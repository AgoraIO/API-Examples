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

#include <api/aosl_types.h>
#include <api/aosl_defs.h>
#include <api/aosl_data.h>


class aosl_data_class {
protected:
	aosl_data_t d;

public:
	aosl_data_class ()
	{
		d = NULL;
	}

	aosl_data_class (const aosl_data_class &src)
	{
		d = src.d;
		if (d != NULL)
			aosl_data_get (d);
	}

	aosl_data_class &operator = (const aosl_data_class &src)
	{
		if (d != NULL)
			aosl_data_put (d);

		d = src.d;
		if (d != NULL)
			aosl_data_get (d);

		return *this;
	}

	operator aosl_data_t () const
	{
		return d;
	}

	aosl_data_t detach ()
	{
		aosl_data_t data = d;
		d = NULL;
		return data;
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
		d = src.d;
		src.d = NULL;
	}

	aosl_data_class &operator = (aosl_data_class &&src)
	{
		if (d != NULL)
			aosl_data_put (d);

		d = src.d;
		src.d = NULL;
		return *this;
	}
#endif /* C++11 */
};

class aosl_data_buf: public aosl_data_class {
public:
	aosl_data_buf (size_t sz)
	{
		if (sz > 0) {
			d = aosl_data_create (sz);
			if (d == NULL)
				abort ();
		}
	}

	aosl_data_buf (const void *src, size_t len)
	{
		if (len > 0) {
			d = aosl_data_create (len);
			if (d == NULL)
				abort ();

			memcpy (ptr (), src, len);
		}
	}

	aosl_data_buf (const char *str)
	{
		if (str != NULL) {
			size_t str_l = strlen (str);
			d = aosl_data_create (str_l + 1);
			if (d == NULL)
				abort ();

			memcpy (ptr (), str, str_l + 1);
		}
	}

	aosl_data_buf (aosl_data_t src)
	{
		d = src;
		if (src != NULL)
			aosl_data_get (src);
	}

	aosl_data_buf (const aosl_data_buf &src)
	{
		d = src.d;
		if (d != NULL)
			aosl_data_get (d);
	}

	aosl_data_buf &operator = (const char *str)
	{
		if (d != NULL) {
			aosl_data_put (d);
			d = NULL;
		}

		if (str != NULL) {
			size_t str_l = strlen (str);
			d = aosl_data_create (str_l + 1);
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
		if (d != NULL)
			aosl_data_put (d);

		d = src.d;
		if (d != NULL)
			aosl_data_get (d);

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

#if (__cplusplus >= 201103) || defined (_MSC_VER)
	aosl_data_buf (aosl_data_buf &&src)
	{
		d = src.d;
		src.d = NULL;
	}

	aosl_data_buf &operator = (aosl_data_buf &&src)
	{
		if (d != NULL)
			aosl_data_put (d);

		d = src.d;
		src.d = NULL;
		return *this;
	}
#endif /* C++11 */
};

template<typename T>
class aosl_data_var: public aosl_data_class {
public:
	aosl_data_var ()
	{
		d = aosl_data_create (sizeof (T));
		if (d == NULL)
			abort ();
	}

	aosl_data_var (size_t nelems)
	{
		if (nelems == 0)
			abort ();

		d = aosl_data_create (sizeof (T) * nelems);
		if (d == NULL)
			abort ();
	}

	aosl_data_var (const aosl_data_var &src)
	{
		d = src.d;
		if (d != NULL)
			aosl_data_get (d);
	}

	aosl_data_var &operator = (const aosl_data_var &src)
	{
		if (d != NULL)
			aosl_data_put (d);

		d = src.d;
		if (d != NULL)
			aosl_data_get (d);

		return *this;
	}

	aosl_data_var &operator = (const T &v)
	{
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

#if (__cplusplus >= 201103) || defined (_MSC_VER)
	aosl_data_var (aosl_data_var &&src)
	{
		d = src.d;
		src.d = NULL;
	}

	aosl_data_var &operator = (aosl_data_var &&src)
	{
		if (d != NULL)
			aosl_data_put (d);

		d = src.d;
		src.d = NULL;
		return *this;
	}
#endif /* C++11 */
};


#endif /* __AOSL_DATA_CLASS_H__ */