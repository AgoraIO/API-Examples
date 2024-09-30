/*************************************************************
 * Author:	Lionfore Hao
 * Date	 :	Jun 30th, 2019
 * Module:	AOSL mpq fd C++ implementation
 *
 *
 * This is a part of the Advanced Operating System Layer.
 * Copyright (C) 2019 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#ifndef __AOSL_MPQ_FD_CLASS_H__
#define __AOSL_MPQ_FD_CLASS_H__

#include <stdlib.h>

#include <api/aosl_types.h>
#include <api/aosl_mpq_fd.h>
#include <api/aosl_ref.h>


class aosl_mpq_fd_class {
public:
	aosl_mpq_fd_class (aosl_fd_t fd): attached_fd (fd)
	{
		ref_id = aosl_ref_create (this, NULL, 1/* MUST BE CALLER-FREE */);
		if (aosl_ref_invalid (ref_id))
			abort ();
	}

	virtual ~aosl_mpq_fd_class ()
	{
		/**
		 * Please make sure this is the first action in the destructor of
		 * each final derived class of this class and its' derivatives.
		 **/
		ref_destroy ();
		if (!aosl_fd_invalid (attached_fd))
			aosl_close (attached_fd);
	}

	aosl_ref_t ref (void)
	{
		return ref_id;
	}

	/**
	 * Please call this function first before freeing any resource relative
	 * with this class and its' derivatives.
	 **/
	void ref_destroy (void)
	{
		if (!aosl_ref_invalid (ref_id)) {
			/**
			 * For caller-free ref object, the destroy will wait other threads
			 * to release the ref usage count, so this will guarantee the safe
			 * accessing of 'this' object.
			 **/
			aosl_ref_destroy (ref_id, 1);
			ref_id = AOSL_REF_INVALID;
		}
	}

	aosl_fd_t get_fd (void)
	{
		return attached_fd;
	}

	aosl_fd_t detach (void)
	{
		if (!aosl_fd_invalid (attached_fd)) {
			aosl_fd_t fd = attached_fd;
			attached_fd = AOSL_INVALID_FD;
			return fd;
		}

		return AOSL_INVALID_FD;
	}

public:
	int co_read (aosl_data_t d_ret, aosl_data_t d_buf, uintptr_t buf_off = 0)
	{
		return aosl_co_read (attached_fd, d_ret, d_buf, buf_off);
	}

	ssize_t co_write (aosl_data_t d_ret, aosl_data_t d_buf)
	{
		return aosl_co_write (attached_fd, d_ret, d_buf);
	}

	ssize_t co_buf_write (aosl_data_t d_ret, const void *buf, size_t count)
	{
		return aosl_co_buf_write (attached_fd, d_ret, buf, count);
	}

private:
	aosl_ref_t ref_id;
	aosl_fd_t attached_fd;

#if (__cplusplus >= 201103) || defined (_MSC_VER)
private:
	aosl_mpq_fd_class () = delete;
	aosl_mpq_fd_class (const aosl_mpq_fd_class &) = delete;
	aosl_mpq_fd_class (aosl_mpq_fd_class &&) = delete;
	aosl_mpq_fd_class &operator = (const aosl_mpq_fd_class &) = delete;
	aosl_mpq_fd_class &operator = (aosl_mpq_fd_class &&) = delete;
#else
private:
	aosl_mpq_fd_class ();
	aosl_mpq_fd_class (const aosl_mpq_fd_class &);
	aosl_mpq_fd_class &operator = (const aosl_mpq_fd_class &);
#endif /* C++11 */
};


#endif /* __AOSL_MPQ_FD_CLASS_H__ */