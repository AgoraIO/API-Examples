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


class aosl_mpq_fd_class {
public:
	aosl_mpq_fd_class (aosl_fd_t fd): attached_fd (fd)
	{
	}

	virtual ~aosl_mpq_fd_class ()
	{
		if (!aosl_fd_invalid (attached_fd))
			aosl_close (attached_fd);
	}

	aosl_fd_t get_fd ()
	{
		return attached_fd;
	}

	aosl_fd_t detach ()
	{
		if (!aosl_fd_invalid (attached_fd)) {
			aosl_fd_t fd = attached_fd;
			attached_fd = AOSL_INVALID_FD;
			return fd;
		}

		return AOSL_INVALID_FD;
	}

public:
	int co_read (aosl_data_t d_ret, aosl_data_t d_buf)
	{
		return aosl_co_read (attached_fd, d_ret, d_buf);
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