/*************************************************************
 * Author:	Lionfore Hao (haolianfu@agora.io)
 * Date	 :	Jun 6th, 2023
 * Module:	AOSL data queue object header file
 *
 *
 * This is a part of the Advanced Operating System Layer.
 * Copyright (C) 2018 ~ 2023 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#ifndef __AOSL_DQ_H__
#define __AOSL_DQ_H__

#include <api/aosl_types.h>
#include <api/aosl_defs.h>
#include <api/aosl_ref.h>
#include <api/aosl_data.h>
#include <api/aosl_poll.h>

#ifdef __cplusplus
extern "C" {
#endif



/**
 * Create a data queue object.
 * Parameter:
 *          max: the maximum queue length;
 * Return value:
 *          !aosl_ref_invalid(ret): the data queue object ref id just created;
 *           aosl_ref_invalid(ret): create failed with aosl_errno set;
 * AOSL_REF_INVALID when failed.
 **/
extern __aosl_api__ aosl_ref_t aosl_dq_create (size_t max);

/**
 * Complete the specified async result object.
 * Parameters:
 *         dque: the data queue object ref id;
 *         data: the data to be queued to the data queue;
 * Return value:
 *        <0: error occured, and aosl_errno indicates which error;
 *       >=0: queued the data to dq successfully;
 **/
extern __aosl_api__ int aosl_dq_queue (aosl_ref_t dque, aosl_data_t data);

/**
 * Complete the specified async result object.
 * Parameters:
 *         dque: the data queue object ref id;
 *        timeo: the max waiting time before dequeuing a data object in ms;
 * Return value:
 *     non-NULL: the dequeued data pointer;
 *         NULL: failed with aosl_errno set;
 **/
extern __aosl_api__ aosl_data_t aosl_dq_dequeue (aosl_ref_t dque, intptr_t timeo);

/**
 * Complete the specified async result object.
 * Parameters:
 *         dque: the data queue object ref id;
 * Return value:
 *        <0: error occured, and aosl_errno indicates which error;
 *       >=0: queued the data object count in the dq;
 **/
extern __aosl_api__ int aosl_dq_count (aosl_ref_t dque);



#ifdef __cplusplus
}
#endif

#endif /* __AOSL_DQ_H__ */