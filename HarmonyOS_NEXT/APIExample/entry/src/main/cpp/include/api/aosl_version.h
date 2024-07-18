/*************************************************************
 * Author:	Lionfore Hao (haolianfu@agora.io)
 * Date	 :	Jul 26th, 2018
 * Module:	AOSL version definitions.
 *
 *
 * This is a part of the Advanced Operating System Layer.
 * Copyright (C) 2018 Agora IO
 * All rights reserved.
 *
 ************************************************************/

#ifndef __AOSL_VERSION_H__
#define __AOSL_VERSION_H__

#include <api/aosl_defs.h>

#ifdef __cplusplus
extern "C" {
#endif



extern __aosl_api__ const char *aosl_get_git_branch ();
extern __aosl_api__ const char *aosl_get_git_commit ();



#ifdef __cplusplus
}
#endif


#endif /* __AOSL_VERSION_H__ */