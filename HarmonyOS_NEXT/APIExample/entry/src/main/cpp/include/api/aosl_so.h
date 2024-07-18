/*************************************************************
 * Author:	Lionfore Hao (haolianfu@agora.io)
 * Date	 :	Dec 13th, 2020
 * Module:	AOSL so module interface definition header file
 *
 *
 * This is a part of the Advanced Operating System Layer.
 * Copyright (C) 2020 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#ifndef __AOSL_SO_H__
#define __AOSL_SO_H__

#include <api/aosl_defs.h>
#include <api/aosl_log.h>

#ifdef __cplusplus
extern "C" {
#endif


extern __aosl_api__ int aosl_so_register (const char *name, const void *mod);

#if defined (__GNUC__)
#define AOSL_SO_REGISTER(name) \
	static void __attribute__  ((constructor, used)) _##name##_so_reg_ctor (void) \
	{ \
		if (aosl_so_register (#name, (const void *)_##name##_so_reg_ctor) < 0) \
			aosl_printf ("SO register failed, so could not monitor it."); \
	}
#elif defined (_MSC_VER)
#pragma section (".CRT$XIG", long, read)

#define AOSL_SO_REGISTER(name) \
	static int _##name##_so_reg_ctor (void) \
	{ \
		if (aosl_so_register (#name, (const void *)_##name##_so_reg_ctor) < 0) \
			aosl_printf ("SO register failed, so could not monitor it."); \
		return 0; \
	} \
	__declspec(allocate(".CRT$XIG")) int (*_##name##_so_module_ctor_f) (void) = _##name##_so_reg_ctor;
#else
#error Unsupported Toolchain!
#endif


#ifdef __cplusplus
}
#endif

#endif /* __AOSL_SO_H__ */