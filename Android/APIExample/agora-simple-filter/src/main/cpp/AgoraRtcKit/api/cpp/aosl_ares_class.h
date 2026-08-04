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

/**
 * RAII wrapper for aosl_ares_*.
 * Remarks:
 *      Copies share the same underlying ares object, but only the "owner"
 *      instance whose magic remains invalid destroys the underlying ref in its
 *      destructor.
 **/
class aosl_ares_class
#if (__cplusplus >= 201103) || (defined (_MSC_VER) && _MSC_VER >= 1800)
						final
#endif
{
private:
	aosl_ref_t refid;
	/**
	 * The magic is AOSL_REF_MAGIC_INVALID when define an ares class object,
	 * and set it to the corresponding ref magic when copy construct other
	 * instance. Only the instance having invalid magic will destroy the ares
	 * object in the destructor.
	 **/
	aosl_ref_magic_t magic;

public:
	aosl_ares_class ()
	{
		refid = aosl_ares_create_flags (AOSL_REF_NEWID, this);
		if (aosl_ref_invalid (refid))
			abort ();

		/* Invalid magic indicates needing destroy in the destructor */
		magic = AOSL_REF_MAGIC_INVALID;
	}

	/**
	 * The destructor of this class is very different with
	 * base class and other derivatives, destroy the ref
	 * in the destructor and the destructor is public.
	 **/
	~aosl_ares_class ()
	{
		/**
		 * Only the instance having invalid magic will destroy the ares object,
		 * and all other copies do nothing in the destructor.
		 **/
		if (magic == AOSL_REF_MAGIC_INVALID) {
			if (!aosl_ref_invalid (refid))
				aosl_ref_destroy (refid, 1);
		}
	}

	int set_scope (aosl_ref_t scope_ref)
	{
		return aosl_ref_set_scope (refid, scope_ref);
	}

	/* complete the async result */
	int complete (intptr_t result = 0) const
	{
		return aosl_ares_magic_complete (refid, magic, result);
	}

	/* wait the async result to be completed */
	int wait (intptr_t timeo, intptr_t *result = NULL)
	{
		return aosl_ares_wait (refid, timeo, result);
	}

	/* reset the signaled state */
	int reset (void)
	{
		return aosl_ares_reset (refid);
	}

	aosl_ref_t ref () const
	{
		return refid;
	}

	operator aosl_ref_t () const
	{
		return refid;
	}

	aosl_ares_class (const aosl_ares_class &src)
	{
		refid = src.refid;
		/* Set the magic in the copy constructor */
		if (aosl_ref_magic (refid, &magic) < 0)
			magic = (aosl_ref_magic_t)-1; /* failed to get the magic, set it to -1 */
	}

	aosl_ares_class &operator = (const aosl_ares_class &src)
	{
		this->~aosl_ares_class ();
		refid = src.refid;
		/* Set the magic in the copy assignment operator */
		if (aosl_ref_magic (refid, &magic) < 0)
			magic = (aosl_ref_magic_t)-1; /* failed to get the magic, set it to -1 */
		return *this;
	}

#if (__cplusplus >= 201103) || (defined (_MSC_VER) && _MSC_VER >= 1800)
	aosl_ares_class (aosl_ares_class &&src)
	{
		refid = src.refid;
		magic = src.magic;
		src.refid = AOSL_REF_INVALID;
		src.magic = AOSL_REF_MAGIC_INVALID;
	}
	aosl_ares_class &operator = (aosl_ares_class &&src)
	{
		this->~aosl_ares_class ();
		refid = src.refid;
		magic = src.magic;
		src.refid = AOSL_REF_INVALID;
		src.magic = AOSL_REF_MAGIC_INVALID;
		return *this;
	}
#endif /* C++11 */
};


#endif /* __AOSL_ARES_CLASS_H__ */
