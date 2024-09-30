/*************************************************************
 * Author:	Lionfore Hao (haolianfu@agora.io)
 * Date	 :	Jun 23rd, 2023
 * Module:	AOSL async result object for C++ definition file
 *
 *
 * This is a part of the Advanced Operating System Layer.
 * Copyright (C) 2018 ~ 2023 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#ifndef __AOSL_ARES_CLASS_H__
#define __AOSL_ARES_CLASS_H__


#include <stdlib.h>

#include <api/aosl_types.h>
#include <api/aosl_defs.h>
#include <api/aosl_ref.h>
#include <api/aosl_ares.h>
#include <api/cpp/aosl_ref_class.h>


class aosl_ares_class: public aosl_ref_class {
public:
	aosl_ares_class (): aosl_ref_class (aosl_ares_create (this))
	{
		if (aosl_ref_invalid (ref ()))
			abort ();
	}

	/**
	 * The destructor of this class is very different with
	 * base class and other derivatives, destroy the ref
	 * in the destructor and the destructor is public.
	 **/
	virtual ~aosl_ares_class ()
	{
		aosl_ref_t refid = ref ();
		if (!aosl_ref_invalid (refid))
			aosl_ref_destroy (refid, true);
	}

	/* complete the async result */
	int complete (intptr_t result = 0)
	{
		return aosl_ares_complete (ref (), result);
	}

	/* wait the async result to be completed */
	int wait (intptr_t timeo, intptr_t *result = NULL)
	{
		return aosl_ares_wait (ref (), timeo, result);
	}

	/* reset the signaled state */
	int reset (void)
	{
		return aosl_ares_reset (ref ());
	}

	operator aosl_ref_t () const
	{
		return ref ();
	}

private:
	/* we do not allow invoke the destroy function of base class */
	int destroy (bool do_delete = true)
	{
		abort ();
		return 0;
	}

#if (__cplusplus >= 201103) || defined (_MSC_VER)
private:
	aosl_ares_class (const aosl_ares_class &) = delete;
	aosl_ares_class (aosl_ares_class &&) = delete;
	aosl_ares_class &operator = (const aosl_ares_class &) = delete;
	aosl_ares_class &operator = (aosl_ares_class &&) = delete;
#else
private:
	aosl_ares_class (const aosl_ares_class &);
	aosl_ares_class &operator = (const aosl_ares_class &);
#endif /* C++11 */
};


#endif /* __AOSL_ARES_CLASS_H__ */