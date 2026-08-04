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

#include <map>
#include <vector>

/**
 * Convenience class for polling multiple aosl_ares_class objects.
 * Remarks:
 *      The class stores the watched refs, delegates the wait to aosl_poll(),
 *      and preserves the subset that became signaled for indexed access after
 *      poll() returns.
 **/
class aosl_poll_class {
private:
	std::map<aosl_ref_t, const aosl_ares_class *> poll_refs;
	std::vector<const aosl_ares_class *> signaled_refs;

public:
	/**
	 * Add an async-result object to the polling set.
	 **/
	void add (const aosl_ares_class &tail)
	{
		poll_refs [tail.ref ()] = &tail;
	}

#if (__cplusplus >= 201103) || (defined (_MSC_VER) && _MSC_VER >= 1800)
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

	/**
	 * Poll the constructed async-result set.
	 * Parameters:
	 *        min: minimum number of signaled refs required to return;
	 *      timeo: timeout in milliseconds.
	 * Return value:
	 *         >=0 number of signaled refs reported by aosl_poll();
	 *          <0 failure.
	 **/
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

	/**
	 * Total watched async-result count.
	 **/
	size_t total ()
	{
		return poll_refs.size ();
	}

	/**
	 * Count of refs recorded as signaled by the last poll() call.
	 **/
	size_t signaled ()
	{
		return signaled_refs.size ();
	}

	/**
	 * Indexed access to the refs signaled in the last poll() call.
	 **/
	const aosl_ares_class *operator [] (size_t idx)
	{
		if (idx < signaled_refs.size ())
			return signaled_refs [idx];

		return NULL;
	}

#if (__cplusplus >= 201103) || (defined (_MSC_VER) && _MSC_VER >= 1800)
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
