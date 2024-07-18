/*************************************************************
 * Author:	Lionfore Hao (haolianfu@agora.io)
 * Date	 :	Jul 9th, 2019
 * Module:	AOSL thread object for C++ definition file
 *
 *
 * This is a part of the Advanced Operating System Layer.
 * Copyright (C) 2018 ~ 2019 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#ifndef __AOSL_THREAD_OBJ_CPP_H__
#define __AOSL_THREAD_OBJ_CPP_H__


#include <stdlib.h>

#include <api/aosl_types.h>
#include <api/aosl_thread.h>


class aosl_lock_class {
private:
	aosl_lock_t lk;

public:
	aosl_lock_class (bool recursive = false)
	{
		lk = aosl_lock_create ((int)recursive);
		if (lk == NULL)
			abort ();
	}

	void lock (void)
	{
		aosl_lock_lock (lk);
	}

	bool try_lock (void)
	{
		return (bool)(aosl_lock_trylock (lk) != 0);
	}

	void unlock (void)
	{
		aosl_lock_unlock (lk);
	}

	~aosl_lock_class ()
	{
		aosl_lock_destroy (lk);
	}

public:
	class scoped {
	private:
		aosl_lock_class &the_lock;
		bool locked;

	public:
		scoped (aosl_lock_class &lk): the_lock (lk)
		{
			the_lock.lock ();
			locked = true;
		}

		void release (void)
		{
			if (!locked)
				abort ();

			locked = false;
			the_lock.unlock ();
		}

		void reacquire (void)
		{
			if (locked)
				abort ();

			the_lock.lock ();
			locked = true;
		}

		~scoped ()
		{
			if (locked)
				the_lock.unlock ();
		}

#if (__cplusplus >= 201103) || defined (_MSC_VER)
	private:
		scoped (const scoped &) = delete;
		scoped (scoped &&) = delete;
		scoped &operator = (const scoped &) = delete;
		scoped &operator = (scoped &&) = delete;
#else
	private:
		scoped (const scoped &);
		scoped &operator = (const scoped &);
#endif /* C++11 */
	};

#if (__cplusplus >= 201103) || defined (_MSC_VER)
private:
	aosl_lock_class (const aosl_lock_class &) = delete;
	aosl_lock_class (aosl_lock_class &&) = delete;
	aosl_lock_class &operator = (const aosl_lock_class &) = delete;
	aosl_lock_class &operator = (aosl_lock_class &&) = delete;
#else
private:
	aosl_lock_class (const aosl_lock_class &);
	aosl_lock_class &operator = (const aosl_lock_class &);
#endif /* C++11 */
};

class aosl_rwlock_class {
private:
	aosl_rwlock_t rw;

public:
	aosl_rwlock_class ()
	{
		rw = aosl_rwlock_create ();
		if (rw == NULL)
			abort ();
	}

	void rdlock (void)
	{
		aosl_rwlock_rdlock (rw);
	}

	bool try_rdlock (void)
	{
		return (bool)(aosl_rwlock_tryrdlock (rw) != 0);
	}

	void wrlock (void)
	{
		aosl_rwlock_wrlock (rw);
	}

	bool try_wrlock (void)
	{
		return (bool)(aosl_rwlock_trywrlock (rw) != 0);
	}

	void rdunlock (void)
	{
		aosl_rwlock_rdunlock (rw);
	}

	void wrunlock (void)
	{
		aosl_rwlock_wrunlock (rw);
	}

	~aosl_rwlock_class ()
	{
		aosl_rwlock_destroy (rw);
	}

public:
	class scoped_rdlock {
	private:
		aosl_rwlock_class &the_rwlock;
		bool locked;

	public:
		scoped_rdlock (aosl_rwlock_class &rwlock): the_rwlock (rwlock)
		{
			the_rwlock.rdlock ();
			locked = true;
		}

		void release (void)
		{
			if (!locked)
				abort ();

			locked = false;
			the_rwlock.rdunlock ();
		}

		void reacquire (void)
		{
			if (locked)
				abort ();

			the_rwlock.rdlock ();
			locked = true;
		}

		~scoped_rdlock ()
		{
			if (locked)
				the_rwlock.rdunlock ();
		}

#if (__cplusplus >= 201103) || defined (_MSC_VER)
	private:
		scoped_rdlock (const scoped_rdlock &) = delete;
		scoped_rdlock (scoped_rdlock &&) = delete;
		scoped_rdlock &operator = (const scoped_rdlock &) = delete;
		scoped_rdlock &operator = (scoped_rdlock &&) = delete;
#else
	private:
		scoped_rdlock (const scoped_rdlock &);
		scoped_rdlock &operator = (const scoped_rdlock &);
#endif /* C++11 */
	};

	class scoped_wrlock {
	private:
		aosl_rwlock_class &the_rwlock;
		bool locked;

	public:
		scoped_wrlock (aosl_rwlock_class &rwlock): the_rwlock (rwlock)
		{
			the_rwlock.wrlock ();
			locked = true;
		}

		void release (void)
		{
			if (!locked)
				abort ();

			locked = false;
			the_rwlock.wrunlock ();
		}

		void reacquire (void)
		{
			if (locked)
				abort ();

			the_rwlock.wrlock ();
			locked = true;
		}

		~scoped_wrlock ()
		{
			if (locked)
				the_rwlock.wrunlock ();
		}

#if (__cplusplus >= 201103) || defined (_MSC_VER)
	private:
		scoped_wrlock (const scoped_wrlock &) = delete;
		scoped_wrlock (scoped_wrlock &&) = delete;
		scoped_wrlock &operator = (const scoped_wrlock &) = delete;
		scoped_wrlock &operator = (scoped_wrlock &&) = delete;
#else
	private:
		scoped_wrlock (const scoped_wrlock &);
		scoped_wrlock &operator = (const scoped_wrlock &);
#endif /* C++11 */
	};

#if (__cplusplus >= 201103) || defined (_MSC_VER)
private:
	aosl_rwlock_class (const aosl_rwlock_class &) = delete;
	aosl_rwlock_class (aosl_rwlock_class &&) = delete;
	aosl_rwlock_class &operator = (const aosl_rwlock_class &) = delete;
	aosl_rwlock_class &operator = (aosl_rwlock_class &&) = delete;
#else
private:
	aosl_rwlock_class (const aosl_rwlock_class &);
	aosl_rwlock_class &operator = (const aosl_rwlock_class &);
#endif /* C++11 */
};



#endif /* __AOSL_THREAD_OBJ_CPP_H__ */