/*************************************************************
 * Author		:		Lionfore Hao (haolianfu@agora.io)
 * Date			:		Jul 31st, 2020
 * Module		:		AOSL byteswap header file
 *
 *
 * This is a part of the Advanced Operating System Layer.
 * Copyright (C) 2020 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#ifndef __AOSL_BYTESWAP_H__
#define __AOSL_BYTESWAP_H__

#if defined (__linux__)
#include <byteswap.h>
#elif defined (__MACH__)
#include <libkern/OSByteOrder.h>
#define bswap_32(x) OSSwapInt32(x)
#define bswap_64(x) OSSwapInt64(x)
#elif defined (_WIN32)
#include <stdlib.h>
#define bswap_32(x) _byteswap_ulong(x)
#define bswap_64(x) _byteswap_uint64(x)
#else
#include <api/aosl_types.h>
#include <api/aosl_defs.h>

extern __aosl_api__ uint32_t aosl_bswap_32 (uint32_t v);
extern __aosl_api__ uint64_t aosl_bswap_64 (uint64_t v);

#define bswap_32(x) aosl_bswap_32(x)
#define bswap_64(x) aosl_bswap_64(x)
#endif


#endif /* __AOSL_BYTESWAP_H__ */