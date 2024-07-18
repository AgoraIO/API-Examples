/*************************************************************
 * Author:	Lionfore Hao (haolianfu@agora.io)
 * Date	 :	Jun 6th, 2023
 * Module:	AOSL common data memory definition header file
 *
 *
 * This is a part of the Advanced Operating System Layer.
 * Copyright (C) 2018 ~ 2023 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#ifndef __AOSL_DATA_H__
#define __AOSL_DATA_H__

#include <api/aosl_types.h>
#include <api/aosl_defs.h>

#ifdef __cplusplus
extern "C" {
#endif



typedef struct _aosl_data_buf_ *aosl_data_t;

typedef enum {
    aosl_dt_data,
    aosl_dt_abuf,
    aosl_dt_vbuf,
} aosl_dt_t;

/**
 * Create an data object with the specified parameters.
 * Parameters:
 *          len: the data length;
 * Return value:
 *     non-NULL: the data object pointer;
 *         NULL: failed with aosl_errno set;
 **/
extern __aosl_api__ aosl_data_t aosl_data_create (size_t len);

/**
 * Get the data object type.
 * Parameters:
 *         data: the data object;
 * Return value:
 *          >=0: one of aosl_dt_t value;
 *           <0: error with aosl_errno set;
 **/
extern __aosl_api__ int aosl_data_type (aosl_data_t d);

/**
 * Increase the data object usage count.
 * Parameters:
 *         data: the data object;
 * Return value:
 *         the current usage count after get;
 **/
extern __aosl_api__ int aosl_data_get (aosl_data_t d);

/**
 * Decrease the data object usage count.
 * Parameters:
 *         data: the data object;
 * Return value:
 *         the current usage count after put.
 *         The data object would be released
 *         if the usage count is zero.
 **/
extern __aosl_api__ int aosl_data_put (aosl_data_t d);

/**
 * Get the data pointer of a data object.
 * Parameters:
 *         data: the data object;
 * Return value:
 *         the data memory pointer, need to be put after used;
 **/
extern __aosl_api__ void *aosl_data_ptr_get (aosl_data_t d);

/**
 * Put the data pointer of a data object.
 * Parameters:
 *         data: the data object;
 * Return value:
 *         None.
 **/
extern __aosl_api__ void aosl_data_ptr_put (aosl_data_t d);

/**
 * Get the data length of a data object.
 * Parameters:
 *         data: the data object;
 * Return value:
 *         the data length;
 **/
extern __aosl_api__ size_t aosl_data_len (aosl_data_t d);

/**
 * Retrieve the OS data object pointer of the data object.
 * Parameters:
 *              data: the data object;
 * Return value:
 *              The OS data object pointer, NULL for none.
 **/
extern __aosl_api__ void *aosl_data_os_data (aosl_data_t d);



#ifdef __cplusplus
}
#endif

#endif /* __AOSL_DATA_H__ */