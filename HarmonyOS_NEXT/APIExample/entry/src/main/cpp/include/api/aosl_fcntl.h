/*************************************************************
 * Author		:		Lionfore Hao (haolianfu@agora.io)
 * Date			:		Jul 31st, 2020
 * Module		:		AOSL fcntl.h definition header file
 *
 *
 * This is a part of the Advanced Operating System Layer.
 * Copyright (C) 2020 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#ifndef __AOSL_FCNTL_H__
#define __AOSL_FCNTL_H__

#if defined (__linux__) || defined (__MACH__) || defined (__kliteos2__) || defined (_WIN32)
#include <fcntl.h>
#include <sys/stat.h>

#if defined (__linux__) || defined (__MACH__) || defined (__kliteos2__)
#include <unistd.h>
#elif defined (_WIN32)
#include <io.h>     /* for open */
#include <direct.h> /* for mkdir */

typedef unsigned short mode_t;
#endif
#elif defined (__kliteos__)
#include <fs/sys/fcntl.h>
#include <fs/sys/stat.h>

typedef unsigned int mode_t;
#elif defined (__kaliosthings__)
#include <sys/fcntl.h>
#include <sys/stat.h>

typedef unsigned int mode_t;
#elif defined (__kspreadtrum__)
#include <api/aosl_types.h>

/* open/fcntl.  */
#define O_ACCMODE 0003
#define O_RDONLY 00
#define O_WRONLY 01
#define O_RDWR 02

#ifndef O_CREAT
#define O_CREAT 0100 /* Not fcntl.  */
#endif

#ifndef O_TRUNC
#define O_TRUNC 01000 /* Not fcntl.  */
#endif

#ifndef O_APPEND
#define O_APPEND 02000
#endif

#ifndef FIONBIO
#define FIONBIO 0
#endif

typedef unsigned short mode_t;

struct stat {
//	unsigned long  st_dev;
//	unsigned long  st_ino;
//	unsigned short st_mode;
//	unsigned short st_nlink;
//	unsigned short st_uid;
//	unsigned short st_gid;
//	unsigned long  st_rdev;
	size_t  st_size;
//	unsigned long  st_blksize;
//	unsigned long  st_blocks;
//	unsigned long  st_atime;
//	unsigned long  st_atime_nsec;
//	unsigned long  st_mtime;
//	unsigned long  st_mtime_nsec;
//	unsigned long  st_ctime;
//	unsigned long  st_ctime_nsec;
//	unsigned long  __unused4;
//	unsigned long  __unused5;
};
#endif

#endif /* __AOSL_FCNTL_H__ */