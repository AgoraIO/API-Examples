/*************************************************************
 * Author:	Lionfore Hao (haolianfu@agora.io)
 * Date	 :	Jul 23rd, 2018
 * Module:	Memory Management relative utilities header file
 *
 *
 * This is a part of the Advanced Operating System Layer.
 * Copyright (C) 2018 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#ifndef __AOSL_MM_H__
#define __AOSL_MM_H__

#include <api/aosl_types.h>
#include <api/aosl_defs.h>


#ifdef __cplusplus
extern "C" {
#endif


extern __aosl_api__ void *aosl_malloc (size_t size);
extern __aosl_api__ void aosl_free (void *ptr);
extern __aosl_api__ void *aosl_calloc (size_t nmemb, size_t size);
extern __aosl_api__ void *aosl_realloc (void *ptr, size_t size);

extern __aosl_api__ char *aosl_strdup (const char *s);

extern __aosl_api__ int aosl_page_size (void);

#define AOSL_PAGE_SIZE (aosl_page_size ())
#define PAGE_ALIGNED_ADDR(a) ((void *)((uintptr_t)(a) & (uintptr_t)(intptr_t)~(AOSL_PAGE_SIZE - 1)))
#define PAGE_ALIGNED_SIZE(s) ((size_t)((size_t)(s) + (AOSL_PAGE_SIZE - 1)) & (uintptr_t)(intptr_t)~(AOSL_PAGE_SIZE - 1))


#ifdef __cplusplus
}
#endif



#endif /* __AOSL_MM_H__ */