/*************************************************************
 * Author:	Lionfore Hao (haolianfu@agora.io)
 * Date	 :	Jun 23rd, 2023
 * Module:	AHPL async result object for C++ definition file
 *
 *
 * This is a part of the Advanced High Performance Library.
 * Copyright (C) 2018 ~ 2023 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#ifndef __AHPL_ARES_CLASS_H__
#define __AHPL_ARES_CLASS_H__


#include <stdlib.h>

#include <api/ahpl_types.h>
#include <api/ahpl_defs.h>
#include <api/ahpl_ref.h>
#include <api/ahpl_ares.h>
#include <api/cpp/ahpl_ref_class.h>


class ahpl_ares_class: public ahpl_ref_class {
public:
	ahpl_ares_class (): ahpl_ref_class (ahpl_ares_create (this))
	{
		if (ahpl_ref_invalid (ref ()))
			abort ();
	}

	/**
	 * The destructor of this class is very different with
	 * base class and other derivatives, destroy the ref
	 * in the destructor and the destructor is public.
	 **/
	virtual ~ahpl_ares_class ()
	{
		ahpl_ref_destroy (ref (), true);
	}

	/* complete the async result */
	int complete (intptr_t result = 0)
	{
		return ahpl_ares_complete (ref (), result);
	}

	/* wait the async result to be completed */
	int wait (intptr_t timeo, intptr_t *result = NULL)
	{
		return ahpl_ares_wait (ref (), timeo, result);
	}

	/* reset the signaled state */
	int reset (void)
	{
		return ahpl_ares_reset (ref ());
	}

	operator ahpl_ref_t () const
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
	ahpl_ares_class (const ahpl_ares_class &) = delete;
	ahpl_ares_class (ahpl_ares_class &&) = delete;
	ahpl_ares_class &operator = (const ahpl_ares_class &) = delete;
	ahpl_ares_class &operator = (ahpl_ares_class &&) = delete;
#endif /* C++11 */
};


#endif /* __AHPL_ARES_CLASS_H__ */