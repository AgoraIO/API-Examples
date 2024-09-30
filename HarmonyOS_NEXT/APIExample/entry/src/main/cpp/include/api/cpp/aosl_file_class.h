/*************************************************************
 * Author:	Lionfore Hao (haolianfu@agora.io)
 * Date	 :	Aug 19th, 2020
 * Module:	AOSL file operations for C++ definition file
 *
 *
 * This is a part of the Advanced Operating System Layer.
 * Copyright (C) 2020 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#ifndef __AOSL_FILE_CPP_H__
#define __AOSL_FILE_CPP_H__


#include <stdlib.h>

#include <api/aosl_types.h>
#include <api/aosl_defs.h>
#include <api/aosl_file.h>
#include <api/aosl_errno.h>

#if (__cplusplus >= 201103) || defined (_MSC_VER)
#include <functional>
#include <memory>
#endif

class aosl_file_class {
private:
	aosl_fd_t fd;

public:
	aosl_file_class ()
	{
		fd = AOSL_INVALID_FD;
	}

	aosl_fd_t detach ()
	{
		if (!aosl_fd_invalid (fd)) {
			aosl_fd_t _fd = fd;
			fd = AOSL_INVALID_FD;
			return _fd;
		}

		return AOSL_INVALID_FD;
	}

	int open (const char *file, int oflag, ...)
	{
		int mode = 0;

		if (!aosl_fd_invalid (fd)) {
			aosl_file_close (fd);
			fd = AOSL_INVALID_FD;
		}

		if ((oflag & O_CREAT) != 0) {
			va_list arg;
			va_start (arg, oflag);
			mode = va_arg (arg, int);
			va_end (arg);
		}

		fd = aosl_file_open (file, oflag, mode);
		if (aosl_fd_invalid (fd))
			return -1;

		return 0;
	}

	ssize_t size ()
	{
		return aosl_file_size (fd);
	}

	ssize_t read (void *buf, size_t count)
	{
		return aosl_file_read (fd, buf, count);
	}

	ssize_t write (const void *buf, size_t count)
	{
		return aosl_file_write (fd, buf, count);
	}

	off_t lseek (off_t offset, int whence)
	{
		return aosl_file_lseek (fd, offset, whence);
	}

	int aread (size_t count, aosl_file_aread_t f, uintptr_t argc, ...)
	{
		va_list args;
		int err;

		va_start (args, argc);
		err = aosl_file_aread_args (fd, count, f, argc, args);
		va_end (args);

		return err;
	}

	int aread_args (size_t count, aosl_file_aread_t f, uintptr_t argc, va_list args)
	{
		return aosl_file_aread_args (fd, count, f, argc, args);
	}

	int aread_argv (size_t count, aosl_file_aread_t f, uintptr_t argc, uintptr_t argv [])
	{
		return aosl_file_aread_argv (fd, count, f, argc, argv);
	}

	int awrite (const void *buf, size_t count, aosl_file_awrite_t f, uintptr_t argc, ...)
	{
		va_list args;
		int err;

		va_start (args, argc);
		err = aosl_file_awrite_args (fd, buf, count, f, argc, args);
		va_end (args);

		return err;
	}

	int awrite_args (const void *buf, size_t count, aosl_file_awrite_t f, uintptr_t argc, va_list args)
	{
		return aosl_file_awrite_args (fd, buf, count, f, argc, args);
	}

	int awrite_argv (const void *buf, size_t count, aosl_file_awrite_t f, uintptr_t argc, uintptr_t argv [])
	{
		return aosl_file_awrite_argv (fd, buf, count, f, argc, argv);
	}

	int alseek (off_t offset, int whence, aosl_file_alseek_t f, uintptr_t argc, ...)
	{
		va_list args;
		int err;

		va_start (args, argc);
		err = aosl_file_alseek_args (fd, offset, whence, f, argc, args);
		va_end (args);

		return err;
	}

	int alseek_args (off_t offset, int whence, aosl_file_alseek_t f, uintptr_t argc, va_list args)
	{
		return aosl_file_alseek_args (fd, offset, whence, f, argc, args);
	}

	int alseek_argv (off_t offset, int whence, aosl_file_alseek_t f, uintptr_t argc, uintptr_t argv [])
	{
		return aosl_file_alseek_argv (fd, offset, whence, f, argc, argv);
	}

	/* C++11 lambda encapsulations */
#if (__cplusplus >= 201103) || defined (_MSC_VER)
public:
	typedef std::function <void (bool free_only, aosl_fd_t fd, ssize_t ret, void *buf)> aosl_file_aread_lambda_f;
	typedef std::function <void (bool free_only, aosl_fd_t fd, ssize_t ret)> aosl_file_awrite_lambda_f;
	typedef std::function <void (bool free_only, aosl_fd_t fd, off_t ret)> aosl_file_alseek_lambda_f;

	int aread (size_t count, aosl_file_aread_lambda_f &&lambda)
	{
		if (lambda != nullptr) {
			aosl_file_aread_lambda_f *lambda_p = new aosl_file_aread_lambda_f (std::move (lambda));
			int err = aread (count, ____aread_f, 1, lambda_p);
			if (err < 0)
				delete lambda_p;

			return err;
		}

		aosl_errno = EINVAL;
		return -1;
	}

	int awrite (const void *buf, size_t count, aosl_file_awrite_lambda_f &&lambda = nullptr)
	{
		int err;

		if (lambda == nullptr) {
			err = awrite (buf, count, NULL, 0);
		} else {
			aosl_file_awrite_lambda_f *lambda_p = new aosl_file_awrite_lambda_f (std::move (lambda));
			err = awrite (buf, count, ____awrite_f, 1, lambda_p);
			if (err < 0)
				delete lambda_p;
		}

		return err;
	}

	int alseek (off_t offset, int whence, aosl_file_alseek_lambda_f &&lambda = nullptr)
	{
		int err;

		if (lambda == nullptr) {
			err = alseek (offset, whence, NULL, 0);
		} else {
			aosl_file_alseek_lambda_f *lambda_p = new aosl_file_alseek_lambda_f (std::move (lambda));
			err = alseek (offset, whence, ____alseek_f, 1, lambda_p);
			if (err < 0)
				delete lambda_p;
		}

		return err;
	}

private:
	static void ____aread_f (int free_only, aosl_fd_t fd, ssize_t ret, void *buf, uintptr_t argc, uintptr_t argv [])
	{
		aosl_file_aread_lambda_f *lambda_f = (aosl_file_aread_lambda_f *)argv [0];
		(*lambda_f) ((bool)(free_only != 0), fd, ret, buf);
		delete lambda_f;
	}

	static void ____awrite_f (int free_only, aosl_fd_t fd, ssize_t ret, uintptr_t argc, uintptr_t argv [])
	{
		aosl_file_awrite_lambda_f *lambda_f = (aosl_file_awrite_lambda_f *)argv [0];
		(*lambda_f) ((bool)(free_only != 0), fd, ret);
		delete lambda_f;
	}

	static void ____alseek_f (int free_only, aosl_fd_t fd, off_t ret, uintptr_t argc, uintptr_t argv [])
	{
		aosl_file_alseek_lambda_f *lambda_f = (aosl_file_alseek_lambda_f *)argv [0];
		(*lambda_f) ((bool)(free_only != 0), fd, ret);
		delete lambda_f;
	}
#endif /* C++11 */

public:
	int co_read (aosl_data_t d_ret, aosl_data_t d_buf, uintptr_t buf_off = 0)
	{
		return aosl_co_file_read (fd, d_ret, d_buf, buf_off);
	}

	int co_write (aosl_data_t d_ret, aosl_data_t d_buf)
	{
		return aosl_co_file_write (fd, d_ret, d_buf);
	}

	int co_file_lseek (aosl_data_t d_ret, off_t offset, int whence)
	{
		return aosl_co_file_lseek (fd, d_ret, offset, whence);
	}

	int close ()
	{
		if (!aosl_fd_invalid (fd)) {
			int err;
			err = aosl_file_close (fd);
			fd = AOSL_INVALID_FD;
			return err;
		}

		return -1;
	}

	virtual ~aosl_file_class ()
	{
		if (!aosl_fd_invalid (fd))
			aosl_file_close (fd);
	}

#if (__cplusplus >= 201103) || defined (_MSC_VER)
private:
	aosl_file_class (const aosl_file_class &) = delete;
	aosl_file_class (aosl_file_class &&) = delete;
	aosl_file_class &operator = (const aosl_file_class &) = delete;
	aosl_file_class &operator = (aosl_file_class &&) = delete;
#else
private:
	aosl_file_class (const aosl_file_class &);
	aosl_file_class &operator = (const aosl_file_class &);
#endif /* C++11 */
};


#endif /* __AOSL_FILE_CPP_H__ */