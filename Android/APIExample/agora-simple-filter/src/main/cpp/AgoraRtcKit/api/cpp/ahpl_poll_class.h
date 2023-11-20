/*************************************************************
 * Author:	Lionfore Hao (haolianfu@agora.io)
 * Date	 :	Jun 23rd, 2023
 * Module:	AHPL poll functionality for C++ definition file
 *
 *
 * This is a part of the Advanced High Performance Library.
 * Copyright (C) 2018 ~ 2023 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#ifndef __AHPL_POLL_CLASS_H__
#define __AHPL_POLL_CLASS_H__


#include <api/ahpl_types.h>
#include <api/ahpl_defs.h>
#include <api/ahpl_ref.h>
#include <api/ahpl_poll.h>
#include <api/cpp/ahpl_ares_class.h>

#if (__cplusplus >= 201103) || defined (_MSC_VER)
#include <functional>
#include <memory>
#endif

#include <map>
#include <vector>


class ahpl_poll_class {
private:
	std::map<ahpl_ref_t, const ahpl_ares_class *> poll_refs;
	std::vector<const ahpl_ares_class *> signaled_refs;

public:
	int add (const ahpl_ares_class &tail)
	{
		poll_refs [tail.ref ()] = &tail;
		return 0;
	}

#if (__cplusplus >= 201103) || defined (_MSC_VER)
	template <class T, class ...Targs>
	int add (const T &head, const Targs&... rest)
	{
		poll_refs [head.ref ()] = &head;
		return add (rest...);
	}

	/* constructor with variable args */
	template <class ...Targs>
	ahpl_poll_class (Targs&... args)
	{
		add (args...);
	}
#endif /* C++11 */

	/* poll the constructed async results */
	int poll (size_t min, intptr_t timeo)
	{
		ahpl_ref_t local_refs [32];
		ahpl_ref_t *refs = local_refs;
		size_t count = poll_refs.size ();
		std::map<ahpl_ref_t, const ahpl_ares_class *>::iterator it;
		int i;
		int err;

		if (count > sizeof local_refs / sizeof local_refs [0]) {
			refs = new ahpl_ref_t [count];
			if (refs == NULL)
				return -1;
		}

		i = 0;
		for (it = poll_refs.begin (); it != poll_refs.end (); it++)
			refs [i++] = it->first;

		err = ahpl_poll (refs, count, min, timeo);
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
	const ahpl_ares_class *operator [] (size_t idx)
	{
		if (idx < signaled_refs.size ())
			return signaled_refs [idx];

		return NULL;
	}

#if (__cplusplus >= 201103) || defined (_MSC_VER)
private:
	ahpl_poll_class (const ahpl_poll_class &) = delete;
	ahpl_poll_class (ahpl_poll_class &&) = delete;
	ahpl_poll_class &operator = (const ahpl_poll_class &) = delete;
	ahpl_poll_class &operator = (ahpl_poll_class &&) = delete;
#endif
};


#endif /* __AHPL_POLL_CLASS_H__ */