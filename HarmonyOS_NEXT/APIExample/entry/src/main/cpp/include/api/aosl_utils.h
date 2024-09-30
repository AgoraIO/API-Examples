/*************************************************************
 * Author:	Lionfore Hao (haolianfu@agora.io)
 * Date	 :	Jul 26th, 2018
 * Module:	AOSL utilities definition file.
 *
 *
 * This is a part of the Advanced Operating System Layer.
 * Copyright (C) 2018 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#ifndef __AOSL_UTILS_H__
#define __AOSL_UTILS_H__

#include <api/aosl_types.h>
#include <api/aosl_defs.h>
#include <api/aosl_mm.h>


#ifdef __cplusplus
extern "C" {
#endif


extern __aosl_api__ int aosl_get_uuid (char buf [], size_t buf_sz);
extern __aosl_api__ int aosl_os_version (char buf [], size_t buf_sz);

/* The returned path must be freed via aosl_free when not used any more */
extern __aosl_api__ const char *aosl_get_data_path ();

extern __aosl_api__ int aosl_processors_count (void);

extern __aosl_api__ void *aosl_platform_obj_get (void *o);
extern __aosl_api__ void aosl_platform_obj_put (void *o);

extern __aosl_api__ const char *aosl_addr_syminfo (uintptr_t addr, uintptr_t *sym_addr);


#ifdef __cplusplus
}
#endif


#endif /* __AOSL_UTILS_H__ */