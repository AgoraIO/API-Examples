/*************************************************************
 * Author:	Lionfore Hao (haolianfu@agora.io)
 * Date	 :	Oct 20th, 2020
 * Module:	Common Posix compatible APIs used in AOSL
 *
 *
 * This is a part of the Advanced Operating System Layer.
 * Copyright (C) 2018 ~ 2020 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#ifndef __AOSL_OS_H__
#define __AOSL_OS_H__

#include <stdio.h>

#include <api/aosl_defs.h>
#include <api/aosl_types.h>
#include <api/aosl_fcntl.h>


#ifdef __cplusplus
extern "C" {
#endif


extern __aosl_api__ int aosl_os_open (const char *file, int oflag, ...);
extern __aosl_api__ int aosl_os_mkdir (const char *pathname, mode_t mode);
extern __aosl_api__ int aosl_os_rmdir (const char *pathname);
extern __aosl_api__ int aosl_os_unlink (const char *pathname);
extern __aosl_api__ int aosl_os_fstat (int fd, struct stat *statbuf);
extern __aosl_api__ int aosl_os_stat (const char *pathname, struct stat *statbuf);
extern __aosl_api__ int aosl_os_rename (const char *oldpath, const char *newpath);


#ifdef __cplusplus
}
#endif


#endif /* __AOSL_OS_H__ */