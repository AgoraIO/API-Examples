/*************************************************************
 * Author:	Lionfore Hao (haolianfu@agora.io)
 * Date	 :	Jul 27th, 2020
 * Module:	AHPL POSIX definitions header file
 *
 *
 * This is a part of the Advanced High Performance Library.
 * Copyright (C) 2020 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#ifndef __AHPL_TYPES_H__
#define __AHPL_TYPES_H__

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <inttypes.h>
#if defined (__linux__) || defined (__MACH__) || defined (__kliteos2__) || defined (_WIN32)
#include <sys/types.h>
#endif

#include <api/ahpl_defs.h>

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


/* The AHPL timestamp type */
typedef unsigned long long ahpl_ts_t;

/* The proto for a general ahpl var args function with argc & argv. */
typedef void (*ahpl_argv_f) (uintptr_t argc, uintptr_t argv []);

/* The proto for a general ahpl object destructor function. */
typedef ahpl_argv_f ahpl_obj_dtor_t;


#if !defined (_WIN32) && !defined (__kspreadtrum__)
typedef int ahpl_fd_t;
#define AHPL_INVALID_FD ((ahpl_fd_t)-1)

static __inline__ int ahpl_fd_invalid (ahpl_fd_t fd)
{
	return (int)(fd < 0);
}
#else
#if defined (_WIN32)
/**
 * We MUST include 'winsock2.h' before any occurrence
 * of including 'windows.h', the fucking Windows has
 * the fucking issue that many definitions would be
 * complained redefinition if not so.
 * -- Lionfore Hao Sep 25th, 2018
 **/
#include <winsock2.h>
#include <windows.h>

typedef HANDLE ahpl_fd_t;
#define AHPL_INVALID_FD ((ahpl_fd_t)INVALID_HANDLE_VALUE)
#elif defined (__kspreadtrum__)
#include <DAPS/export/inc/tcpip6/socket_types.h>
#include <DAPS/export/inc/tcpip6/socket_api.h>

typedef TCPIP_SOCKET_T ahpl_fd_t;
#define AHPL_INVALID_FD ((ahpl_fd_t)-1)
#endif

static __inline__ int ahpl_fd_invalid (ahpl_fd_t fd)
{
	return (int)(fd == AHPL_INVALID_FD);
}
#endif


#ifdef __cplusplus
}
#endif


#endif /* __AHPL_TYPES_H__ */