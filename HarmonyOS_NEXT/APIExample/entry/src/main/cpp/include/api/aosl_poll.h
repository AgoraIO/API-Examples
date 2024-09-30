/*************************************************************
 * Author:	Lionfore Hao (haolianfu@agora.io)
 * Date	 :	May 30th, 2023
 * Module:	AOSL poll functionality definition header file
 *
 *
 * This is a part of the Advanced Operating System Layer.
 * Copyright (C) 2018 ~ 2023 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#ifndef __AOSL_POLL_H__
#define __AOSL_POLL_H__

#include <api/aosl_types.h>
#include <api/aosl_defs.h>
#include <api/aosl_ref.h>

#ifdef __cplusplus
extern "C" {
#endif



#define AOSL_POLL_ST_NONE 0
#define AOSL_POLL_ST_SIGNALED 1
#define AOSL_POLL_ST_DESTROY 2

/**
 * Poll the objects specified in refs, return their states.
 * Parameters:
 *      refs: the object refs array for input, and the signaled refs array for output;
 *     count: the number of items for input refs array;
 *       min: the minimum number of signaled refs for triggering waken up.
 *            NOTE: if any one of the refs encounters error or destroy,
 *                  then wake up immediately regardless min parameter.
 *     timeo: maximum waiting time in milliseconds;
 * Return value:
 *        <0: error occured, and aosl_errno indicates which error;
 *       >=0: the signaled refs count before timeout;
 **/
extern __aosl_api__ ssize_t aosl_poll (aosl_ref_t refs [], size_t count, size_t min, intptr_t timeo);



#ifdef __cplusplus
}
#endif

#endif /* __AOSL_POLL_H__ */