/*************************************************************
 * Author:	Lionfore Hao (haolianfu@agora.io)
 * Date	 :	Jul 27th, 2020
 * Module:	AOSL POSIX definitions header file
 *
 *
 * This is a part of the Advanced Operating System Layer.
 * Copyright (C) 2020 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#ifndef __AOSL_TYPES_H__
#define __AOSL_TYPES_H__

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <inttypes.h>
#if defined (__linux__) || defined (__MACH__) || defined (__kliteos2__) || defined (_WIN32)
#include <sys/types.h>
#if defined (__linux__) || defined (__MACH__)
#include <sys/uio.h>
#endif
#endif

#include <api/aosl_defs.h>

#ifdef __cplusplus
extern "C" {
#endif


#if !defined (__linux__) && !defined (__MACH__)
/**
 * Worry about some guy would like to define a macro
 * for this type, so confirm that it is not a macro.
 * -- Lionfore Hao Nov 5th, 2018
 **/
#ifndef __ssize_t_defined
typedef intptr_t ssize_t;
#define __ssize_t_defined
#endif
#endif


/* The AOSL timestamp type */
typedef unsigned long long aosl_ts_t;

/* The proto for a general aosl var args function with argc & argv. */
typedef void (*aosl_argv_f) (uintptr_t argc, uintptr_t argv []);

/* The proto for a general aosl object destructor function. */
typedef aosl_argv_f aosl_obj_dtor_t;


#if !defined (_WIN32) && !defined (__kspreadtrum__)
typedef int aosl_fd_t;
#define AOSL_INVALID_FD ((aosl_fd_t)-1)

static __inline__ int aosl_fd_invalid (aosl_fd_t fd)
{
	return (int)(fd < 0);
}
#else
#if defined (_WIN32)
/**
 * We MUST include 'winsock2.h' before any occurrence
 * of including 'windows.h', the Microsoft Windows has
 * the stupid issue that many definitions would be
 * complained redefinition if not so.
 * -- Lionfore Hao Sep 25th, 2018
 **/
#include <winsock2.h>
#include <windows.h>

typedef HANDLE aosl_fd_t;
#define AOSL_INVALID_FD ((aosl_fd_t)INVALID_HANDLE_VALUE)
#elif defined (__kspreadtrum__)
#include <DAPS/export/inc/tcpip6/socket_types.h>
#include <DAPS/export/inc/tcpip6/socket_api.h>

typedef TCPIP_SOCKET_T aosl_fd_t;
#define AOSL_INVALID_FD ((aosl_fd_t)-1)
#endif

static __inline__ int aosl_fd_invalid (aosl_fd_t fd)
{
	return (int)(fd == AOSL_INVALID_FD);
}
#endif


#ifndef _WIN32
typedef aosl_fd_t aosl_sk_t;
#define AOSL_INVALID_SK AOSL_INVALID_FD
#else
typedef SOCKET aosl_sk_t;
#define AOSL_INVALID_SK ((aosl_sk_t)INVALID_SOCKET)
#endif


#if !defined (_WIN32) && !defined (__kspreadtrum__)
typedef struct iovec aosl_miov_t;
#else
typedef struct {
	void *iov_base;
	size_t iov_len;
} aosl_miov_t;
#endif

#ifndef UIO_MAXIOV
#define UIO_MAXIOV 1024
#endif


#ifdef __cplusplus
}
#endif


#endif /* __AOSL_TYPES_H__ */