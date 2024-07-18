/*************************************************************
 * Author:	Lionfore Hao (haolianfu@agora.io)
 * Date	 :	May 4th, 2022
 * Module:	AOSL angel functionalities definition header file
 *
 *
 * This is a part of the Advanced Operating System Layer.
 * Copyright (C) 2020 ~ 2022 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#ifndef __AOSL_ANGEL_H__
#define __AOSL_ANGEL_H__

#include <api/aosl_types.h>
#include <api/aosl_defs.h>
#include <api/aosl_thread.h>

#ifdef __cplusplus
extern "C" {
#endif


#define AOSL_MPQ_STALLED_THRESHOLD_MS 9000

typedef struct {
	aosl_thrdid_t thrd_id;
	char thrd_name [32];
} aosl_stalled_info_t;

typedef void (*aosl_stall_cb_t) (uint32_t stalled_secs, size_t stalled_count, aosl_stalled_info_t *stalled_thrds);

extern __aosl_api__ int aosl_angel_stall_register (aosl_stall_cb_t f);
extern __aosl_api__ int aosl_angel_stall_unregister (aosl_stall_cb_t f);


typedef struct overload_thrd {
	aosl_thrdid_t thrd_id;
	char thrd_name [32];
	float cpu_usage;
} aosl_ovl_thrd_info_t;

typedef void (*aosl_ovl_cb_t) (size_t count, aosl_ovl_thrd_info_t *thrds);
extern __aosl_api__ int aosl_angel_ovl_register (aosl_ovl_cb_t f);
extern __aosl_api__ int aosl_angel_ovl_unregister (aosl_ovl_cb_t f);


#ifdef __cplusplus
}
#endif


#endif /* __AOSL_ANGEL_H__ */