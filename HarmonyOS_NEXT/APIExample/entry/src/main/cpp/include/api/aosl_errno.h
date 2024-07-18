/*************************************************************
 * Author		:		Lionfore Hao (haolianfu@agora.io)
 * Date			:		Jul 30th, 2020
 * Module		:		AOSL errno definition header file
 *
 *
 * This is a part of the Advanced Operating System Layer.
 * Copyright (C) 2020 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#ifndef __AOSL_ERRNO_H__
#define __AOSL_ERRNO_H__

#include <api/aosl_types.h>
#include <api/aosl_defs.h>

#ifdef __cplusplus
extern "C" {
#endif


#if defined (__linux__) || defined (__MACH__) || defined (_WIN32) || defined (__kliteos__) || defined (__kliteos2__)
#define _AOSL_HAVING_OS_ERRNO
#endif

#if defined (__linux__) || defined (__MACH__) || defined (__kliteos__) || defined (__kliteos2__)
#define _AOSL_USING_OS_ERRNO
#endif

#ifdef _AOSL_USING_OS_ERRNO
#define aosl_errno_ptr() (&errno)
#else
extern __aosl_api__ int *aosl_errno_ptr (void);
#endif

#ifdef _AOSL_HAVING_OS_ERRNO
#include <errno.h>
#define aosl_strerror(err) strerror (err)
#else
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
#include <DAPS/export/inc/tcpip6/socket_types.h>
#endif

#define errno (*aosl_errno_ptr ())

extern __aosl_api__ char *aosl_strerror (int errnum);
#endif


#define aosl_errno (*aosl_errno_ptr ())


/**
 * Check whether a value returned by aosl api is an error code.
 * Parameters:
 *      err: the value returned by one of aosl api;
 * Return Value:
 *        1: this value is an error code;
 *        0: this value is normal, not an error;
*/
extern __aosl_api__ int aosl_is_err (intptr_t err);

/**
 * Convert an error code to aosl_errno value.
 * Parameters:
 *      err: the error code value returned by one of aosl api;
 * Return Value:
 *        the corresponding aosl_errno value;
*/
extern __aosl_api__ int aosl_err_to_errno (intptr_t err);


#ifdef __cplusplus
}
#endif


#endif /* __AOSL_ERRNO_H__ */