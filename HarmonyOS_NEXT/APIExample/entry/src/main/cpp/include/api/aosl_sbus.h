/*************************************************************
 * Author:	Lionfore Hao (haolianfu@agora.io)
 * Date	 :	Feb 7th, 2023
 * Module:	AOSL software bus relative API definition file
 *
 *
 * This is a part of the Advanced Operating System Layer.
 * Copyright (C) 2018 ~ 2023 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#ifndef __AOSL_SBUS_H__
#define __AOSL_SBUS_H__


#include <api/aosl_types.h>
#include <api/aosl_defs.h>
#include <api/aosl_mpq.h>
#include <api/aosl_ref.h>
#include <api/aosl_data.h>


#ifdef __cplusplus
extern "C" {
#endif



/**
 * Create a software bus object.
 * Parameters:
 *         name: the software bus name;
 * Return value:
 *            0: successfully;
 *           <0: failed with aosl_errno set accordingly.
 **/
extern __aosl_api__ int aosl_sbus_create (const char *name);

/**
 * Action callback function for a bus object, which will be invoked when the bus has data to deliver.
 * Parameters:
 *              robj: the ref object specified when registering;
 *              name: the bus object name;
 *              data: the bus data;
 *              argc: the various args count, passed when registering;
 *              argv: the various args vector, passed as various args when registering;
 * Return value:
 *            None.
 * Remarks:
 *        If we issue async operation which uses the data object in this callback function, we must increase
 *        the usage before issuing.
 **/
typedef void (*aosl_sbus_data_t) (aosl_refobj_t robj, const char *name, aosl_data_t data, uintptr_t argc, uintptr_t argv []);


/**
 * Add an action of the bus object specified by name for processing data.
 * Parameters:
 *              name: the bus object name;
 *               qid: the mpq you want the target function to be invoked in, if invalid, arbitrary general pool mpq;
 *               ref: specify a ref object just like the queue operation of an mpq;
 *               max: the max queued data object count, if exceeds then drop the eldest one automatically;
 *            f_name: specify the target function name;
 *                 f: the target function;
 *              dtor: destructor which be invoked when delete the action;
 *              argc: the various args count;
 *               ...: the various args;
 * Return value:
 *       non-NULL: successfully;
 *           NULL: failed with aosl_errno set accordingly.
 *       The return value only used for unregister, no other usage.
 **/
extern __aosl_api__ void *aosl_sbus_action_add (const char *name, aosl_mpq_t qid, aosl_ref_t ref, uintptr_t max, const char *f_name, aosl_sbus_data_t f, aosl_obj_dtor_t dtor, uintptr_t argc, ...);

/**
 * Add an action of the kobject specified by name for processing data.
 * Parameters:
 *              name: the bus object name;
 *               qid: the mpq you want the target function to be invoked in, if invalid, arbitrary general pool mpq;
 *               ref: specify a ref object just like the queue operation of an mpq;
 *               max: the max queued data object count, if exceeds then drop the eldest one automatically;
 *            f_name: specify the target function name;
 *                 f: the target function;
 *              dtor: destructor which be invoked when delete the action;
 *              argc: the various args count;
 *              args: the various args;
 * Return value:
 * Return value:
 *       non-NULL: successfully;
 *           NULL: failed with aosl_errno set accordingly.
 *       The return value only used for unregister, no other usage.
 **/
extern __aosl_api__ void *aosl_sbus_action_add_args (const char *name, aosl_mpq_t qid, aosl_ref_t ref, uintptr_t max, const char *f_name, aosl_sbus_data_t f, aosl_obj_dtor_t dtor, uintptr_t argc, va_list args);

/**
 * Add an action of the bus object specified by name for processing data.
 * Parameters:
 *              name: the bus object name;
 *               qid: the mpq you want the target function to be invoked in, if invalid, arbitrary general pool mpq;
 *               ref: specify a ref object just like the queue operation of an mpq;
 *               max: the max queued data object count, if exceeds then drop the eldest one automatically;
 *            f_name: specify the target function name;
 *                 f: the target function;
 *              dtor: destructor which be invoked when delete the action;
 *              argc: the various args count;
 *              argv: the various args vector;
 * Return value:
 * Return value:
 *       non-NULL: successfully;
 *           NULL: failed with aosl_errno set accordingly.
 *       The return value only used for unregister, no other usage.
 **/
extern __aosl_api__ void *aosl_sbus_action_add_argv (const char *name, aosl_mpq_t qid, aosl_ref_t ref, uintptr_t max, const char *f_name, aosl_sbus_data_t f, aosl_obj_dtor_t dtor, uintptr_t argc, uintptr_t argv []);

/**
 * Delete an action of the bus object specified by name.
 * Parameters:
 *              name: the bus object name;
 *          h_action: the handle returned by the above add functions;
 * Return value:
 *            0: successfully;
 *           <0: failed with aosl_errno set accordingly.
 **/
extern __aosl_api__ int aosl_sbus_action_del (const char *name, void *h_action);

/**
 * Send data to the bus object.
 * Parameters:
 *              name: the bus object name;
 *              data: the data will be sent to bus;
 * Return value:
 *            0: successfully;
 *           <0: failed with aosl_errno set accordingly.
 * Remarks:
 *        The function will increase the usage of data automatically,
 *        so the calling function can decrease the usage just after
 *        this function returns, and no need to worry about the async
 *        operations triggerd by this function.
 **/
extern __aosl_api__ int aosl_sbus_send (const char *name, aosl_data_t data);

/**
 * Delete the bus object specified by name.
 * Parameters:
 *              name: the bus object name;
 * Return value:
 *            0: successfully;
 *           <0: failed with aosl_errno set accordingly.
 **/
extern __aosl_api__ int aosl_sbus_delete (const char *name);


#ifdef __cplusplus
}
#endif



#endif /* __AOSL_SBUS_H__ */