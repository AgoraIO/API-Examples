/*************************************************************
 * Author:	Lionfore Hao (haolianfu@agora.io)
 * Date	 :	Aug 16th, 2020
 * Module:	AOSL regular file async read/write operations
 *          definition header file
 *
 *
 * This is a part of the Advanced Operating System Layer.
 * Copyright (C) 2020 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#ifndef __AOSL_FILE_H__
#define __AOSL_FILE_H__

#include <api/aosl_types.h>
#include <api/aosl_defs.h>
#include <api/aosl_fcntl.h>
#include <api/aosl_data.h>

/**
 * PLEASE BE CAREFUL ENOUGH HERE:
 * The macro __kspreadtrum__ is not a compiler toolchain
 * predefined macro, and makercore building system will
 * define this macro when building Spreadtrum system, so
 * you must GUARANTEE define it when not using makercore
 * building system.
 * Use this style name rather than Kbuild CONFIG_* just
 * want to keep consistent with __linux__/__MACH__ etc.
 * -- Lionfore Hao Aug 1st, 2020
 **/
#if defined (__kspreadtrum__)
#include <MS_Ref/export/inc/sfs.h>
#elif defined (__kliteos__)
#include <stdio.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if defined (__kspreadtrum__)
#undef SEEK_SET
#undef SEEK_CUR
#undef SEEK_END

#define SEEK_SET SFS_SEEK_BEGIN
#define SEEK_CUR SFS_SEEK_CUR
#define SEEK_END SFS_SEEK_END

typedef int64_t off_t;
#endif

/**
 * Cross platform file open function just like the posix open function.
 * Parameters:
 *       file: the file path;
 *      oflag: the open flag like posix open;
 *        ...: the mode when specified O_CREAT in the oflag, or ignored;
 * Return Value:
 *       the opened file 'fd', AOSL_INVALID_FD for failed with aosl_errno
 *       set.
 * Remarks:
 *       For posix platforms, this function acts just like open, but for
 *       Windows platform, this function is implemented via Win32 API.
 **/
extern __aosl_api__ aosl_fd_t aosl_file_open (const char *file, int oflag, ...);

/**
 * Cross platform get file size function.
 * Parameters:
 *         fd: the file fd;
 * Return Value:
 *       the file size, -1 for failed with aosl_errno set.
 **/
extern __aosl_api__ ssize_t aosl_file_size (aosl_fd_t fd);

/**
 * Cross platform file read function just like the posix read function.
 * Parameters:
 *         fd: the file fd;
 *        buf: the buffer for holding the read data;
 *      count: the max bytes to read;
 * Return Value:
 *       the actual read bytes, -1 for failed with aosl_errno set.
 **/
extern __aosl_api__ ssize_t aosl_file_read (aosl_fd_t fd, void *buf, size_t count);

/**
 * Cross platform file write function just like the posix write function.
 * Parameters:
 *         fd: the file fd;
 *        buf: the buffer holding the writing data;
 *      count: the number of bytes to write;
 * Return Value:
 *       the actual written bytes, -1 for failed with aosl_errno set.
 **/
extern __aosl_api__ ssize_t aosl_file_write (aosl_fd_t fd, const void *buf, size_t count);

/**
 * Cross platform file seek function just like the posix lseek function.
 * Parameters:
 *         fd: the file fd;
 *     offset: the argument offset according to the directive whence as follows:
 *             SEEK_SET
 *                    The file offset is set to offset bytes.
 *             SEEK_CUR
 *                    The file offset is set to its current location plus offset bytes.
 *             SEEK_END
 *                    The file offset is set to the size of the file plus offset bytes.
 *     whence: SEEK_SET/SEEK_CUR/SEEK_END
 * Return Value:
 *     Upon successful completion, returns the resulting offset location as measured in bytes
 *     from the beginning of the file;
 *     On error, the value (off_t)-1 is returned with aosl_errno set.
 **/
extern __aosl_api__ off_t aosl_file_lseek (aosl_fd_t fd, off_t offset, int whence);

/**
 * The async file read callback function prototype.
 * Parameters:
 *     free_only: indicates whether this is a 'free only' case, do nothing except freeing
 *                possible resource for 'free only' cases;
 *            fd: the file fd;
 *           ret: the async read return value;
 *           buf: the buffer holding the read data;
 *          argc: arguments count passed by async read function;
 *          argv: the various arguments passed by async read function;
 * Return Value:
 *          None.
 **/
typedef void (*aosl_file_aread_t) (int free_only, aosl_fd_t fd, ssize_t ret, void *buf, uintptr_t argc, uintptr_t argv []);

/**
 * The async file write callback function prototype.
 * Parameters:
 *     free_only: indicates whether this is a 'free only' case, do nothing except freeing
 *                possible resource for 'free only' cases;
 *            fd: the file fd;
 *           ret: the async write return value;
 *          argc: arguments count passed by async write function;
 *          argv: the various arguments passed by async write function;
 * Return Value:
 *          None.
 **/
typedef void (*aosl_file_awrite_t) (int free_only, aosl_fd_t fd, ssize_t ret, uintptr_t argc, uintptr_t argv []);

/**
 * The async file seek callback function prototype.
 * Parameters:
 *     free_only: indicates whether this is a 'free only' case, do nothing except freeing
 *                possible resource for 'free only' cases;
 *            fd: the file fd;
 *           ret: the async seek return value;
 *          argc: arguments count passed by async write function;
 *          argv: the various arguments passed by async write function;
 * Return Value:
 *          None.
 **/
typedef void (*aosl_file_alseek_t) (int free_only, aosl_fd_t fd, off_t ret, uintptr_t argc, uintptr_t argv []);

/**
 * The async file read function for reading from the specified file asynchronously.
 * Parameters:
 *            fd: the file fd;
 *         count: the max read bytes;
 *             f: the async read callback function;
 *          argc: arguments count would be passed to async callback function;
 *           ...: the various arguments would be passed to async callback function;
 * Return Value:
 *             0: successful, and the async callback will be invoked;
 *            <0: failed with aosl_errno set;
 **/
extern __aosl_api__ int aosl_file_aread (aosl_fd_t fd, size_t count, aosl_file_aread_t f, uintptr_t argc, ...);

/**
 * The async file write function for writing to the specified file asynchronously.
 * Parameters:
 *            fd: the file fd;
 *           buf: the buffer holding the writing data;
 *         count: the writing data size in bytes;
 *          done: the async write done callback function;
 *          argc: arguments count would be passed to async callback function;
 *           ...: the various arguments would be passed to async callback function;
 * Return Value:
 *             0: successful, and the async callback will be invoked;
 *            <0: failed with aosl_errno set;
 **/
extern __aosl_api__ int aosl_file_awrite (aosl_fd_t fd, const void *buf, size_t count, aosl_file_awrite_t done, uintptr_t argc, ...);

/**
 * The async file seek function for seeking the specified file asynchronously.
 * Parameters:
 *            fd: the file fd;
 *        offset: the argument offset according to the directive whence as follows:
 *                SEEK_SET
 *                       The file offset is set to offset bytes.
 *                SEEK_CUR
 *                       The file offset is set to its current location plus offset bytes.
 *                SEEK_END
 *                       The file offset is set to the size of the file plus offset bytes.
 *        whence: SEEK_SET/SEEK_CUR/SEEK_END
 *          done: the async seek done callback function;
 *          argc: arguments count would be passed to async callback function;
 *           ...: the various arguments would be passed to async callback function;
 * Return Value:
 *             0: successful, and the async callback will be invoked;
 *            <0: failed with aosl_errno set;
 **/
extern __aosl_api__ int aosl_file_alseek (aosl_fd_t fd, off_t offset, int whence, aosl_file_alseek_t done, uintptr_t argc, ...);

/* va_list versions */
extern __aosl_api__ int aosl_file_aread_args (aosl_fd_t fd, size_t count, aosl_file_aread_t f, uintptr_t argc, va_list args);
extern __aosl_api__ int aosl_file_awrite_args (aosl_fd_t fd, const void *buf, size_t count, aosl_file_awrite_t done, uintptr_t argc, va_list args);
extern __aosl_api__ int aosl_file_alseek_args (aosl_fd_t fd, off_t offset, int whence, aosl_file_alseek_t done, uintptr_t argc, va_list args);

/* args vector versions */
extern __aosl_api__ int aosl_file_aread_argv (aosl_fd_t fd, size_t count, aosl_file_aread_t f, uintptr_t argc, uintptr_t argv []);
extern __aosl_api__ int aosl_file_awrite_argv (aosl_fd_t fd, const void *buf, size_t count, aosl_file_awrite_t done, uintptr_t argc, uintptr_t argv []);
extern __aosl_api__ int aosl_file_alseek_argv (aosl_fd_t fd, off_t offset, int whence, aosl_file_alseek_t done, uintptr_t argc, uintptr_t argv []);

/**
 * The regular file read function which supports coroutine.
 * Parameters:
 *      fd: the opened file fd;
 *   d_ret: the data object for holding return value, the
 *          length must >= sizeof (ssize_t);
 *   d_buf: the buffer for holding the read data;
 * buf_off: the buffer offset for holding the read data;
 * Return value:
 *      <0: failed with aosl_errno set;
 *       0: async read issued successfully;
 **/
extern __aosl_api__ int aosl_co_file_read (aosl_fd_t fd, aosl_data_t d_ret, aosl_data_t d_buf, uintptr_t buf_off);

/**
 * The regular file write function which supports coroutine.
 * Parameters:
 *      fd: the opened file fd;
 *   d_ret: the data object for holding the async write
 *          done return value, it can be NULL, but if
 *          set the length must >= sizeof (ssize_t);
 *   d_buf: the buffer for holding the data for writing;
 *   count: the writing data bytes;
 * Return value:
 *      <0: failed with aosl_errno set;
 *       0: successfully;
 **/
extern __aosl_api__ int aosl_co_file_write (aosl_fd_t fd, aosl_data_t d_ret, aosl_data_t d_buf);

/**
 * Cross platform file seek function just like the posix lseek function.
 * Parameters:
 *         fd: the file fd;
 *      d_ret: the data object for holding return value, the length must >= sizeof (off_t),
 *             upon successful completion, returns the resulting offset location as measured
 *             in bytes from the beginning of the file, on error, the value (off_t)-aosl_errno
 *             is returned;
 *     offset: the argument offset according to the directive whence as follows:
 *             SEEK_SET
 *                    The file offset is set to offset bytes.
 *             SEEK_CUR
 *                    The file offset is set to its current location plus offset bytes.
 *             SEEK_END
 *                    The file offset is set to the size of the file plus offset bytes.
 *     whence: SEEK_SET/SEEK_CUR/SEEK_END
 * Return Value:
 **/
extern __aosl_api__ int aosl_co_file_lseek (aosl_fd_t fd, aosl_data_t d_ret, off_t offset, int whence);

/**
 * Cross platform close file function.
 * Parameters:
 *         fd: the file fd;
 * Return Value:
 *          0: successful;
 *         <0: failed with aosl_errno set;
 **/
extern __aosl_api__ int aosl_file_close (aosl_fd_t fd);



#ifdef __cplusplus
}
#endif


#endif /* __AOSL_FILE_H__ */