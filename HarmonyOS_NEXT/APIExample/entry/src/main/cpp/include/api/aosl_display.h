/*************************************************************
 * Author:	Lionfore Hao (haolianfu@agora.io)
 * Date	 :	Feb 19th, 2023
 * Module:	AOSL display relative API definition file
 *
 *
 * This is a part of the Advanced Operating System Layer.
 * Copyright (C) 2018 ~ 2023 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#ifndef __AOSL_DISPLAY_H__
#define __AOSL_DISPLAY_H__


#include <api/aosl_types.h>
#include <api/aosl_defs.h>


#ifdef __cplusplus
extern "C" {
#endif



/**
 * Set the global display.
 * Parameters:
 *          display: the display;
 * Return value:
 *           <0: failed with aosl_errno set
 *            0: successful
 **/
extern __aosl_api__ int aosl_display_set (void *display);

/**
 * Get the global display.
 * Parameters:
 *          None.
 * Return value:
 *          the display.
 **/
extern __aosl_api__ void *aosl_display_get (void);



#ifdef __cplusplus
}
#endif



#endif /* __AOSL_DISPLAY_H__ */