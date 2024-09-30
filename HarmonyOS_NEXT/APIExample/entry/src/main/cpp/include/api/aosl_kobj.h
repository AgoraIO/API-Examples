/*************************************************************
 * Author:	Lionfore Hao (haolianfu@agora.io)
 * Date	 :	Nov 26th, 2022
 * Module:	AOSL kernel object API definition file
 *
 *
 * This is a part of the Advanced Operating System Layer.
 * Copyright (C) 2018 ~ 2022 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#ifndef __AOSL_KOBJ_H__
#define __AOSL_KOBJ_H__


#include <api/aosl_types.h>
#include <api/aosl_defs.h>
#include <api/aosl_mpq.h>
#include <api/aosl_ref.h>


#ifdef __cplusplus
extern "C" {
#endif


/* The kobject file descriptor type */
typedef struct _internal_kobj_od_ *aosl_kfd_t;

/* The common invalid file descriptor value for initializing an aosl_kfd_t var */
#define AOSL_KFD_INVALID ((aosl_kfd_t)(intptr_t)NULL)

/* Check whether an aosl_kfd_t val is invalid */
#define aosl_kfd_invalid(id) ((int)(intptr_t)(id) <= 0)


typedef enum {
	aosl_kobj_obj, /* object */
	aosl_kobj_dir, /* directory */
	aosl_kobj_lnk, /* symbolic link */
	aosl_kobj_drv, /* driver */
	aosl_kobj_val, /* value */
} aosl_kobj_type_t;

#define AOSL_KOBJ_PATHMAX 256

#define AOSL_KOBJ_R 0x10 /* Read */
#define AOSL_KOBJ_W 0x20 /* Write */
#define AOSL_KOBJ_RW (AOSL_KOBJ_R | AOSL_KOBJ_W)
#define AOSL_KOBJ_S 0x40 /* Start */
#define AOSL_KOBJ_NONBLOCK 0x80 /* Non block */
#define AOSL_KOBJ_OPEN_METHOD_MASK 0xf

/**
 * The kobject open function prototype, which is used to open a kobject.
 * Parameters:
 *              dfd: the opened fd of directory, AOSL_KFD_INVALID for root or pwd:
 *                   1. if the name starts with a '/', then directory is root;
 *                   2. if the name does not start with a '/', then directory is pwd;
 *             name: the opening kobject name;
 *            flags: the opening flags bitmask AOSL_KOBJ_R, AOSL_KOBJ_W
 * Return value:
 *         the opened kobject id, some like an fd.
 *      aosl_kfd_invalid(result): open failed with aosl_errno set;
 *     !aosl_kfd_invalid(result): successfully;
 **/
extern __aosl_api__ aosl_kfd_t aosl_kobj_open (aosl_kfd_t dfd, const char *name, int flags);

/**
 * Close an aosl_kfd_t.
 * Parameters:
 *              fd: the fd to be closed
 * Return value:
 *         0: successfully
 *        <0: failed with aosl_errno set
 **/
extern __aosl_api__ int aosl_kobj_close (aosl_kfd_t fd);

/**
 * Get the full path of the kobject specified by fd.
 * Parameters:
 *                fd: the opened fd;
 *           pathbuf: the buffer for holding the path.
 *                    NOTE: the path was filled from the end, so do NOT
 *                          use the pathbuf directly when accessing the
 *                          path result, using the return value instead.
 *               len: the pathbuf size in bytes.
 *          needed_p: the path length needed when non-NULL, it could be
 *                    NULL if guarantee the pathbuf is big enough.
 * Return value:
 *         the returned path pointer, it is >= &pathbuf [0] and < &pathbuf [len],
 *         NULL for failed, with aosl_errno set correspondingly.
 **/
extern __aosl_api__ char *aosl_kobj_path (aosl_kfd_t fd, char *pathbuf, size_t len, size_t *needed_p);

/**
 * Get the current working directory path.
 * Parameters:
 *           pathbuf: the buffer for holding the path.
 *                    NOTE: the path was filled from the end, so do NOT
 *                          use the pathbuf directly when accessing the
 *                          path result, using the return value instead.
 *               len: the pathbuf size in bytes.
 *          needed_p: the path length needed when non-NULL, it could be
 *                    NULL if guarantee the pathbuf is big enough.
 * Return value:
 *         the returned path pointer, it is >= &pathbuf [0] and < &pathbuf [len],
 *         NULL for failed, with aosl_errno set correspondingly.
 **/
extern __aosl_api__ char *aosl_kobj_pwd (char *pathbuf, size_t len, size_t *needed_p);

/**
 * Read data from the kobject specified by fd.
 * Parameters:
 *                fd: the opened fd;
 *               buf: the buffer for holding the data;
 *               len: the buffer size in bytes;
 * Return value:
 *          >=0: the read data length;
 *           <0: failed with aosl_errno set accordingly.
 **/
extern __aosl_api__ ssize_t aosl_kobj_read (aosl_kfd_t fd, void *buf, size_t len);

/**
 * Write data to the kobject specified by fd.
 * Parameters:
 *                fd: the opened fd;
 *               buf: the buffer for holding the data;
 *               len: the buffer size in bytes;
 * Return value:
 *          >=0: the written data length;
 *           <0: failed with aosl_errno set accordingly.
 **/
extern __aosl_api__ ssize_t aosl_kobj_write (aosl_kfd_t fd, const void *buf, size_t len);

/**
 * IO control the kobject specified by fd.
 * Parameters:
 *                fd: the opened fd;
 *               req: the request code;
 *              argc: various arg count;
 *              argv: the various args vector;
 * Return value:
 *          >=0: the IO control result according to the request;
 *           <0: failed with aosl_errno set accordingly.
 **/
extern __aosl_api__ intptr_t aosl_kobj_ioctl_argv (aosl_kfd_t fd, uintptr_t req, uintptr_t argc, uintptr_t *argv);

/**
 * IO control the kobject specified by fd.
 * Parameters:
 *                fd: the opened fd;
 *               req: the request code;
 *              argc: various arg count;
 *              args: the various args;
 * Return value:
 *          >=0: the IO control result according to the request;
 *           <0: failed with aosl_errno set accordingly.
 **/
extern __aosl_api__ intptr_t aosl_kobj_ioctl_args (aosl_kfd_t fd, uintptr_t req, uintptr_t argc, va_list args);

/**
 * IO control the kobject specified by fd.
 * Parameters:
 *                fd: the opened fd;
 *               req: the request code;
 *              argc: various arg count;
 *               ...: the various args;
 * Return value:
 *          >=0: the IO control result according to the request;
 *           <0: failed with aosl_errno set accordingly.
 **/
extern __aosl_api__ intptr_t aosl_kobj_ioctl (aosl_kfd_t fd, uintptr_t req, uintptr_t argc, ...);


typedef enum {
	aosl_kobj_seek_set,
	aosl_kobj_seek_cur,
	aosl_kobj_seek_end
} aosl_kobj_seek_t;

/**
 * Seek the kobject specified by fd.
 * Parameters:
 *                fd: the opened fd;
 *            offset: the seek offset;
 *            whence: where the seeking from, aosl_kobj_seek_t type;
 * Return value:
 *          >=0: the seek result;
 *           <0: failed with aosl_errno set accordingly.
 **/
extern __aosl_api__ int32_t aosl_kobj_seek (aosl_kfd_t fd, int32_t offset, int whence);

/**
 * Make a directory in the directory specified by dfd.
 * Parameters:
 *               dfd: the opened fd of directory, AOSL_KFD_INVALID for root or pwd:
 *                    1. if the name starts with a '/', then directory is root;
 *                    2. if the name does not start with a '/', then directory is pwd;
 *              name: the directory name;
 * Return value:
 *            0: successfully;
 *           <0: failed with aosl_errno set accordingly.
 **/
extern __aosl_api__ int aosl_kobj_mkdir (aosl_kfd_t dfd, const char *name);

/**
 * Remove a directory from the directory specified by dfd.
 * Parameters:
 *               dfd: the opened fd of directory, AOSL_KFD_INVALID for root or pwd:
 *                    1. if the name starts with a '/', then directory is root;
 *                    2. if the name does not start with a '/', then directory is pwd;
 *              name: the kobject name;
 * Return value:
 *            0: successfully;
 *           <0: failed with aosl_errno set accordingly.
 **/
extern __aosl_api__ int aosl_kobj_rm (aosl_kfd_t dfd, const char *name);

/**
 * Change the current working directory to the directory specified by dfd + name.
 * Parameters:
 *               dfd: the opened fd of directory, AOSL_KFD_INVALID for root or pwd:
 *                    1. if the name starts with a '/', then directory is root;
 *                    2. if the name does not start with a '/', then directory is pwd;
 *              name: the directory name;
 * Return value:
 *            0: successfully;
 *           <0: failed with aosl_errno set accordingly.
 **/
extern __aosl_api__ int aosl_kobj_chdir (aosl_kfd_t dfd, const char *name);

/**
 * Create a symbolic link to the target.
 * Parameters:
 *               dfd: the opened fd of directory, AOSL_KFD_INVALID for root or pwd:
 *                    1. if the name starts with a '/', then directory is root;
 *                    2. if the name does not start with a '/', then directory is pwd;
 *              name: the directory name;
 *              tdfd: the target opened fd of directory, AOSL_KFD_INVALID for root or pwd:
 *                    1. if the name starts with a '/', then directory is root;
 *                    2. if the name does not start with a '/', then directory is pwd;
 *            target: the target name;
 * Return value:
 *            0: successfully;
 *           <0: failed with aosl_errno set accordingly.
 **/
extern __aosl_api__ int aosl_kobj_ln (aosl_kfd_t dfd, const char *name, aosl_kfd_t tdfd, const char *target);

/**
 * Read the full path of a symbolic link specified by dfd + name.
 * Parameters:
 *               dfd: the opened fd of directory, AOSL_KFD_INVALID for root or pwd:
 *                    1. if the name starts with a '/', then directory is root;
 *                    2. if the name does not start with a '/', then directory is pwd;
 *              name: the directory name;
 *               buf: the buffer for holding the path.
 *                    NOTE: the path was filled from the end, so do NOT
 *                          use the pathbuf directly when accessing the
 *                          path result, using the return value instead.
 *             bufsz: the buf size in bytes.
 * Return value:
 *         the returned path pointer, it is >= &buf [0] and < &buf [bufsz],
 *         NULL for failed, with aosl_errno set correspondingly.
 **/
extern __aosl_api__ char *aosl_kobj_readlink (aosl_kfd_t dfd, const char *name, char *buf, size_t bufsz);

/**
 * Move the object specified by sdfd + sname to the destination specified by ddfd + dname.
 * Parameters:
 *              sdfd: the source opened fd of directory, AOSL_KFD_INVALID for root or pwd:
 *                    1. if the name starts with a '/', then directory is root;
 *                    2. if the name does not start with a '/', then directory is pwd;
 *             sname: the source object name;
 *              ddfd: the destination opened fd of directory, AOSL_KFD_INVALID for root or pwd:
 *                    1. if the name starts with a '/', then directory is root;
 *                    2. if the name does not start with a '/', then directory is pwd;
 *             dname: the destination name;
 * Return value:
 *            0: successfully;
 *           <0: failed with aosl_errno set accordingly.
 **/
extern __aosl_api__ int aosl_kobj_mv (aosl_kfd_t sdfd, const char *sname, aosl_kfd_t ddfd, const char *dname);

/**
 * Get the kobject type specified by fd.
 * Parameters:
 *                fd: the opened kobject fd;
 * Return value:
 *          >=0: the kobject type, value is one of aosl_kobj_type_t;
 *           <0: failed with aosl_errno set accordingly.
 **/
extern __aosl_api__ int aosl_kobj_fdtype (aosl_kfd_t fd);

/**
 * Get the kobject type specified by dfd + name.
 * Parameters:
 *               dfd: the opened fd of directory, AOSL_KFD_INVALID for root or pwd:
 *                    1. if the name starts with a '/', then directory is root;
 *                    2. if the name does not start with a '/', then directory is pwd;
 *              name: the directory name;
 * Return value:
 *          >=0: the kobject type, value is one of aosl_kobj_type_t;
 *           <0: failed with aosl_errno set accordingly.
 **/
extern __aosl_api__ int aosl_kobj_type (aosl_kfd_t dfd, const char *name);


typedef struct {
	uint16_t e_len; /* the total dir entry length in bytes. */
	uint16_t e_type; /* the dir entry type, one of aosl_kobj_type_t */
	char e_name [0]; /* the dir entry name */
} aosl_kobj_dent_t;

typedef struct kobj_dir *aosl_kobj_dir_t;

/**
 * Open the dir specified by dfd + name for reading its' content.
 * Parameters:
 *               dfd: the opened fd of directory, AOSL_KFD_INVALID for root or pwd:
 *                    1. if the name starts with a '/', then directory is root;
 *                    2. if the name does not start with a '/', then directory is pwd;
 *              name: the directory name;
 * Return value:
 *     non-NULL: the opened dir;
 *         NULL: failed with aosl_errno set accordingly.
 **/
extern __aosl_api__ aosl_kobj_dir_t aosl_kobj_dir_open (aosl_kfd_t dfd, const char *name);

/**
 * Get the opened dir fd.
 * Parameters:
 *               d: the opened dir.
 * Return value: the fd of the opened dir.
 **/
extern __aosl_api__ aosl_kfd_t aosl_kobj_dir_kfd (aosl_kobj_dir_t d);

/**
 * Read the opened dir specified by d.
 * Parameters:
 *               d: the opened dir.
 * Return value:
 *     non-NULL: one dir entry of the opened dir;
 *         NULL: failed with aosl_errno set accordingly.
 **/
extern __aosl_api__ aosl_kobj_dent_t *aosl_kobj_dir_read (aosl_kobj_dir_t d);

/**
 * Seek the opened dir specified by d to the specified position.
 * Parameters:
 *               d: the opened dir;
 *             pos: the target position.
 * Return value:
 *           <0: failed with aosl_errno set accordingly;
 *        non-0: equal to value of pos.
 **/
extern __aosl_api__ int32_t aosl_kobj_dir_seek (aosl_kobj_dir_t d, uint32_t pos);

/**
 * Tell the current position of the opened dir specified by d.
 * Parameters:
 *               d: the opened dir;
 * Return value:
 *           <0: failed with aosl_errno set accordingly;
 *        non-0: the current position of d.
 **/
extern __aosl_api__ int32_t aosl_kobj_dir_pos (aosl_kobj_dir_t d);

/**
 * Close the the opened dir specified by d.
 * Parameters:
 *               d: the opened dir;
 * Return value:
 *           <0: failed with aosl_errno set accordingly;
 *            0: successfully.
 **/
extern __aosl_api__ int aosl_kobj_dir_close (aosl_kobj_dir_t d);

typedef enum {
	aosl_kobj_dent_add, /* add a dir entry */
	aosl_kobj_dent_del, /* del a dir entry */
} aosl_kobj_dir_op_t;

typedef struct {
	const char *name;
	aosl_kobj_type_t type;
	aosl_kobj_dir_op_t dir_op;
} aosl_kobj_dir_notify_t;

/**
 * A registered callback function for an object, which will be invoked when the object changed.
 * Parameters:
 *              robj: the ref object specified when registering;
 *           dirpath: the fullpath for the object;
 *              name: the object name;
 *        notif_data: the notification data, which may be 'aosl_kobj_dir_notify_t *' or 'aosl_kobj_value_notify_t *';
 *              argc: the various args count, passed when registering;
 *              argv: the various args vector, passed as various args when registering;
 * Return value:
 *            None.
 **/
typedef void (*aosl_kobj_notify_t) (aosl_refobj_t robj, const char *dirpath, const char *name, void *notif_data, uintptr_t argc, uintptr_t argv []);

/**
 * Register a notification of the kobject specified by dfd + name for monitoring changes.
 * Parameters:
 *               dfd: the opened fd of directory, AOSL_KFD_INVALID for root or pwd:
 *                    1. if the name starts with a '/', then directory is root;
 *                    2. if the name does not start with a '/', then directory is pwd;
 *              name: the object name;
 *             touch: 0: notify only when changed; !0: notify anyway;
 *               qid: the mpq you want the target function to be invoked in, must be valid;
 *               ref: specify a ref object just like the queue operation of an mpq;
 *            f_name: specify the target function name;
 *                 f: the target function;
 *              dtor: destructor which be invoked when unregister the notification;
 *              argc: the various args count;
 *               ...: the various args;
 * Return value:
 *       non-NULL: successfully;
 *           NULL: failed with aosl_errno set accordingly.
 *       The return value only used for unregister, no other usage.
 **/
extern __aosl_api__ void *aosl_kobj_notify_register (aosl_kfd_t dfd, const char *name, int touch, aosl_mpq_t qid, aosl_ref_t ref, const char *f_name, aosl_kobj_notify_t f, aosl_obj_dtor_t dtor, uintptr_t argc, ...);

/**
 * Register a notification of the kobject specified by dfd + name for monitoring changes.
 * Parameters:
 *               dfd: the opened fd of directory, AOSL_KFD_INVALID for root or pwd:
 *                    1. if the name starts with a '/', then directory is root;
 *                    2. if the name does not start with a '/', then directory is pwd;
 *              name: the object name;
 *             touch: 0: notify only when changed; !0: notify anyway;
 *               qid: the mpq you want the target function to be invoked in, must be valid;
 *               ref: specify a ref object just like the queue operation of an mpq;
 *            f_name: specify the target function name;
 *                 f: the target function;
 *              dtor: destructor which be invoked when unregister the notification;
 *              argc: the various args count;
 *              args: the various args;
 * Return value:
 * Return value:
 *       non-NULL: successfully;
 *           NULL: failed with aosl_errno set accordingly.
 *       The return value only used for unregister, no other usage.
 **/
extern __aosl_api__ void *aosl_kobj_notify_register_args (aosl_kfd_t dfd, const char *name, int touch, aosl_mpq_t qid, aosl_ref_t ref, const char *f_name, aosl_kobj_notify_t f, aosl_obj_dtor_t dtor, uintptr_t argc, va_list args);

/**
 * Register a notification of the kobject specified by dfd + name for monitoring changes.
 * Parameters:
 *               dfd: the opened fd of directory, AOSL_KFD_INVALID for root or pwd:
 *                    1. if the name starts with a '/', then directory is root;
 *                    2. if the name does not start with a '/', then directory is pwd;
 *              name: the object name;
 *             touch: 0: notify only when changed; !0: notify anyway;
 *               qid: the mpq you want the target function to be invoked in, must be valid;
 *               ref: specify a ref object just like the queue operation of an mpq;
 *            f_name: specify the target function name;
 *                 f: the target function;
 *              dtor: destructor which be invoked when unregister the notification;
 *              argc: the various args count;
 *              argv: the various args vector;
 * Return value:
 * Return value:
 *       non-NULL: successfully;
 *           NULL: failed with aosl_errno set accordingly.
 *       The return value only used for unregister, no other usage.
 **/
extern __aosl_api__ void *aosl_kobj_notify_register_argv (aosl_kfd_t dfd, const char *name, int touch, aosl_mpq_t qid, aosl_ref_t ref, const char *f_name, aosl_kobj_notify_t f, aosl_obj_dtor_t dtor, uintptr_t argc, uintptr_t argv []);

/**
 * Unregister a notification of the kobject specified by dfd + name.
 * Parameters:
 *               dfd: the opened fd of directory, AOSL_KFD_INVALID for root or pwd:
 *                    1. if the name starts with a '/', then directory is root;
 *                    2. if the name does not start with a '/', then directory is pwd;
 *              name: the object name;
 *            handle: the handle returned by above register functions;
 * Return value:
 *            0: successfully;
 *           <0: failed with aosl_errno set accordingly.
 **/
extern __aosl_api__ int aosl_kobj_notify_unregister (aosl_kfd_t dfd, const char *name, void *handle);


#ifdef __cplusplus
}
#endif



#endif /* __AOSL_KOBJ_H__ */