/*************************************************************
 * Author:	Lionfore Hao (haolianfu@agora.io)
 * Date	 :	May 10th, 2021
 * Module:	AOSL profile system header file
 *
 *
 * This is a part of the Advanced Operating System Layer.
 * Copyright (C) 2021 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#ifndef	__AOSL_PROFILE_H__
#define	__AOSL_PROFILE_H__

#include <api/aosl_types.h>
#include <api/aosl_defs.h>


#ifdef __cplusplus
extern "C" {
#endif


/**
 * Dump the profile data callback function.
 * Parameter:
 *   img_name: the profile hitting image name
 *  img_start: the hitting image start address
 *     pc_off: the hitting address offset in the image
 *     native: 1 for native profile; 0 for extern profile
 *       hits: the hitting count
 *        arg: the parameter passed in dump function
 * Return value:
 *     None.
 **/
typedef void (*aosl_profile_cb_t) (const char *img_name, uintptr_t img_start, uint32_t pc_off, int native, uint32_t hits, void *arg);

#define AOSL_PROFILE_DUMP_NATIVE 0x1
#define AOSL_PROFILE_DUMP_EXTERN 0x2
/**
 * Dump the profile data via specified callback function.
 * Parameter:
 *         cb: the dump callback function of aosl_profile_cb_t type
 *      flags: bit mask for specifying dump which profile data:
 *             AOSL_PROFILE_DUMP_NATIVE: dump native profile data
 *             AOSL_PROFILE_DUMP_EXTERN: dump extern profile data
 *  threshold: specify the dump minimum threshold in 1/1000 units, 0 for any
 *        arg: the parameter passed to dump callback
 * Return value:
 *     None.
 **/
extern __aosl_api__ void aosl_profile_data (aosl_profile_cb_t cb, int flags, uint32_t threshold, void *arg);
extern __aosl_api__ void aosl_profile_dump (const char *filepath, int flags, uint32_t threshold);
extern __aosl_api__ void aosl_profile_reset (void);
extern __aosl_api__ void aosl_profile_free (void);

extern __aosl_api__ void aosl_profile_start (int period);
extern __aosl_api__ void aosl_profile_stop (void);
extern __aosl_api__ int aosl_profile_period (void);

extern __aosl_api__ int aosl_profile_register_this (const char *name);
extern __aosl_api__ void aosl_profile_clear_registered (void);

extern __aosl_api__ void aosl_get_times (uint64_t *system_us, uint64_t *user_us);

#ifdef __cplusplus
}
#endif

#endif /* __AOSL_PROFILE_H__ */
