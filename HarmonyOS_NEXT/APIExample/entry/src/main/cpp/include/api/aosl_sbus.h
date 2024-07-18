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


#ifdef __cplusplus
extern "C" {
#endif



/**
 * Create a software bus.
 * Parameters:
 *              name: the software bus name;
 * Return value:
 *           the current reference count after get;
 **/
extern __aosl_api__ int aosl_sbus_create (const char *name);




#ifdef __cplusplus
}
#endif



#endif /* __AOSL_SBUS_H__ */