/*************************************************************
 * Author:	Lionfore Hao (haolianfu@agora.io)
 * Date	 :	Jun 23rd, 2023
 * Module:	AOSL poll functionality for C++ definition file
 *
 *
 * This is a part of the Advanced Operating System Layer.
 * Copyright (C) 2018 ~ 2023 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#ifndef __AOSL_POLL_CLASS_H__
#define __AOSL_POLL_CLASS_H__


#include <api/aosl_types.h>
#include <api/aosl_defs.h>
#include <api/aosl_ref.h>
#include <api/aosl_poll.h>
#include <api/cpp/aosl_ares_class.h>

#if (__cplusplus >= 201103) || defined (_MSC_VER)
#include <functional>
#include <memory>
#endif

#include <map>
#include <vector>


class aosl_poll_class {
private:
	std::map<aosl_ref_t, const aosl_ares_class *> poll_refs;
	std::vector<const aosl_ares_class *> signaled_refs;

public:
	void add (const aosl_ares_class &tail)
	{
		poll_refs [tail.ref ()] = &tail;
	}

#if (__cplusplus >= 201103) || defined (_MSC_VER)
	template <class T, class ...Targs>
	void add (const T &head, const Targs&... rest)
	{
		poll_refs [head.ref ()] = &head;
		add (rest...);
	}

	/* constructor with variable args */
	template <class ...Targs>
	aosl_poll_class (Targs&... args)
	{
		add (args...);
	}
#endif /* C++11 */

	aosl_poll_class (const aosl_ares_class * const areses [], size_t count)
	{
		size_t i;
		for (i = 0; i < count; i++)
			add (*areses [i]);
	}

	/* poll the constructed async results */
	int poll (size_t min, intptr_t timeo)
	{
		aosl_ref_t local_refs [32];
		aosl_ref_t *refs = local_refs;
		size_t count = poll_refs.size ();
		std::map<aosl_ref_t, const aosl_ares_class *>::iterator it;
		int i;
		int err;

		if (count > sizeof local_refs / sizeof local_refs [0]) {
			refs = new aosl_ref_t [count];
			if (refs == NULL)
				return -1;
		}

		i = 0;
		for (it = poll_refs.begin (); it != poll_refs.end (); it++)
			refs [i++] = it->first;

		err = aosl_poll (refs, count, min, timeo);
		signaled_refs.clear ();
		for (i = 0; i < err; i++) {
			it = poll_refs.find (refs [i]);
			if (it != poll_refs.end ())
				signaled_refs.push_back (it->second);
		}

		if (refs != local_refs)
			delete [] refs;

		return err;
	}

	/* total async results count */
	size_t total ()
	{
		return poll_refs.size ();
	}

	/* signaled async results count */
	size_t signaled ()
	{
		return signaled_refs.size ();
	}

	/* operator for accessing the signaled async results */
	const aosl_ares_class *operator [] (size_t idx)
	{
		if (idx < signaled_refs.size ())
			return signaled_refs [idx];

		return NULL;
	}

#if (__cplusplus >= 201103) || defined (_MSC_VER)
private:
	aosl_poll_class (const aosl_poll_class &) = delete;
	aosl_poll_class (aosl_poll_class &&) = delete;
	aosl_poll_class &operator = (const aosl_poll_class &) = delete;
	aosl_poll_class &operator = (aosl_poll_class &&) = delete;
#else
private:
	aosl_poll_class (const aosl_poll_class &);
	aosl_poll_class &operator = (const aosl_poll_class &);
#endif
};


#endif /* __AOSL_POLL_CLASS_H__ */