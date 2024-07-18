/*************************************************************
 * Author:	Lionfore Hao (haolianfu@agora.io)
 * Date	 :	Jul 26th, 2018
 * Module:	Time relative utilities header file
 *
 *
 * This is a part of the Advanced Operating System Layer.
 * Copyright (C) 2018 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#ifndef __AOSL_API_TIME_H__
#define __AOSL_API_TIME_H__

#if defined (__linux__) || defined (__MACH__) || defined (__kliteos2__) || defined (_WIN32)
#include <time.h>

#if defined (__MACH__)
#include <sys/time.h> /* for definition of timeval */
#endif

#ifdef _WIN32
/**
 * For struct timeval definition. We MUST include
 * 'winsock2.h' before any occurrence of including
 * 'windows.h'.
 **/
#include <winsock2.h>
#include <windows.h>
#endif
#endif

#include <api/aosl_types.h>
#include <api/aosl_defs.h>

#ifdef __cplusplus
extern "C" {
#endif


extern __aosl_api__ aosl_ts_t aosl_tick_now ();
extern __aosl_api__ aosl_ts_t aosl_tick_us ();
extern __aosl_api__ aosl_ts_t aosl_tick_ns ();

extern __aosl_api__ aosl_ts_t aosl_time_sec ();
extern __aosl_api__ aosl_ts_t aosl_time_ms ();
extern __aosl_api__ aosl_ts_t aosl_time_us ();

extern __aosl_api__ aosl_ts_t aosl_start_tick_ns ();
extern __aosl_api__ aosl_ts_t aosl_start_tick_us ();
extern __aosl_api__ aosl_ts_t aosl_start_tick_ms ();

extern __aosl_api__ aosl_ts_t aosl_start_time_us ();
extern __aosl_api__ aosl_ts_t aosl_start_time_ms ();
extern __aosl_api__ aosl_ts_t aosl_start_time_sec ();

#if defined (__linux__) || defined (__MACH__) || defined (__kliteos2__) || defined (_WIN32)
extern __aosl_api__ aosl_ts_t aosl_ns_from_tv (const struct timeval *tv);
extern __aosl_api__ aosl_ts_t aosl_us_from_tv (const struct timeval *tv);
extern __aosl_api__ aosl_ts_t aosl_ms_from_tv (const struct timeval *tv);

#ifndef _WIN32
extern __aosl_api__ aosl_ts_t aosl_ns_from_ts (const struct timespec *ts);
extern __aosl_api__ aosl_ts_t aosl_us_from_ts (const struct timespec *ts);
extern __aosl_api__ aosl_ts_t aosl_ms_from_ts (const struct timespec *ts);
#endif
#endif

extern __aosl_api__ void aosl_msleep (uint32_t ms);
extern __aosl_api__ void aosl_usleep (uint32_t us);


#ifdef __cplusplus
}
#endif


#endif /* __AOSL_API_TIME_H__ */
