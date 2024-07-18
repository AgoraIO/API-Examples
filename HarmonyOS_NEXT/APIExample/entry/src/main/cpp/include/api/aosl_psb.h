/*************************************************************
 * Author:	Lionfore Hao (haolianfu@agora.io)
 * Date	 :	Jul 21st, 2018
 * Module:	Packet Segment Buffer relatives header file
 *
 *
 * This is a part of the Advanced Operating System Layer.
 * Copyright (C) 2018 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#ifndef __AOSL_PSB_H__
#define __AOSL_PSB_H__


#include <api/aosl_types.h>
#include <api/aosl_defs.h>


#ifdef __cplusplus
extern "C" {
#endif



#define DFLT_MAX_PSB_PKT (4 << 10) /* default max packet size is 4K */

/* AOSL Packet Segment Buffer */
typedef struct _____psb {
	void *data;
	size_t len;
	struct _____psb *next;
} aosl_psb_t;


extern __aosl_api__ aosl_psb_t *aosl_alloc_user_psb (void *buf, size_t bufsz);
extern __aosl_api__ aosl_psb_t *aosl_alloc_psb (size_t size);
extern __aosl_api__ void aosl_psb_attach_buf (aosl_psb_t *psb, void *buf, size_t bufsz);
extern __aosl_api__ void aosl_psb_detach_buf (aosl_psb_t *psb);
extern __aosl_api__ size_t aosl_psb_headroom (const aosl_psb_t *psb);
extern __aosl_api__ size_t aosl_psb_tailroom (const aosl_psb_t *psb);
extern __aosl_api__ int aosl_psb_reserve (aosl_psb_t *psb, size_t len);
extern __aosl_api__ void *aosl_psb_data (const aosl_psb_t *psb);
extern __aosl_api__ size_t aosl_psb_len (const aosl_psb_t *psb);
extern __aosl_api__ size_t aosl_psb_total_len (const aosl_psb_t *psb);
extern __aosl_api__ void *aosl_psb_put (aosl_psb_t *psb, size_t len);
extern __aosl_api__ void *aosl_psb_get (aosl_psb_t *psb, size_t len);
extern __aosl_api__ void *aosl_psb_peek (const aosl_psb_t *psb, size_t len);
extern __aosl_api__ void *aosl_psb_push (aosl_psb_t *psb, size_t len);
extern __aosl_api__ void *aosl_psb_pull (aosl_psb_t *psb, size_t len);
extern __aosl_api__ ssize_t aosl_psb_write (aosl_psb_t *psb, const void *buf, size_t count);
extern __aosl_api__ int aosl_psb_modify (aosl_psb_t *psb, uintptr_t off, const void *buf, size_t count);
extern __aosl_api__ ssize_t aosl_psb_read (aosl_psb_t *psb, void *buf, size_t count);
extern __aosl_api__ int aosl_psb_single (const aosl_psb_t *psb);
extern __aosl_api__ void aosl_psb_reset (aosl_psb_t *psb);
extern __aosl_api__ void aosl_free_psb_list (aosl_psb_t *psb);



#ifdef __cplusplus
}
#endif



#endif /* __AOSL_PSB_H__ */
