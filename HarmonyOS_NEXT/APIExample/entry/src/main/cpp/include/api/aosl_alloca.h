/*************************************************************
 * Author		:		Lionfore Hao (haolianfu@agora.io)
 * Date			:		Jul 31st, 2020
 * Module		:		AOSL alloca definition header file
 *
 *
 * This is a part of the Advanced Operating System Layer.
 * Copyright (C) 2020 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#ifndef __AOSL_ALLOCA_H__
#define __AOSL_ALLOCA_H__

#if defined (__GNUC__)
#include <alloca.h>
#elif defined (_MSC_VER)
#include <malloc.h>
#endif

#endif /* __AOSL_ALLOCA_H__ */