/*************************************************************
 * Author:	Lionfore Hao (haolianfu@agora.io)
 * Date	 :	Aug 11th, 2021
 * Module:	AOSL objc relative declarations
 *
 *
 * This is a part of the Advanced Operating System Layer.
 * Copyright (C) 2018 ~ 2021 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#ifndef __AOSL_OBJC_H__
#define __AOSL_OBJC_H__


#ifdef __cplusplus
extern "C" {
#endif


extern void *objc_autoreleasePoolPush (void);
extern void objc_autoreleasePoolPop (void *pool);


#ifdef __cplusplus
}
#endif



#endif /* __AOSL_OBJC_H__ */
