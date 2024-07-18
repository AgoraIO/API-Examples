/*************************************************************
 * Author:	Lionfore Hao (haolianfu@agora.io)
 * Date	 :	Jul 18th, 2020
 * Module:	AOSL module interface definition header file
 *
 *
 * This is a part of the Advanced Operating System Layer.
 * Copyright (C) 2020 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#ifndef __AOSL_MODULE_H__
#define __AOSL_MODULE_H__

#include <stdlib.h>

#include <api/aosl_types.h>
#include <api/aosl_defs.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef void *aosl_module_t;

typedef int (*aosl_module_call_t) (uintptr_t argc, uintptr_t argv []);

extern __so_api__ int aosl_module_register (const char *name, aosl_module_call_t entry);
extern __so_api__ aosl_module_t aosl_module_get (const char *name);
extern __so_api__ int aosl_module_call (aosl_module_t module, uintptr_t argc, ...);
extern __so_api__ int aosl_module_call_args (aosl_module_t module, uintptr_t argc, va_list args);
extern __so_api__ int aosl_module_call_argv (aosl_module_t module, uintptr_t argc, uintptr_t argv []);
extern __so_api__ void aosl_module_put (aosl_module_t module);
extern __so_api__ int aosl_module_unregister (const char *name);

#if defined (__GNUC__)
#define AOSL_DEFINE_MODULE_REGFN(regfn, name, entry) \
	static void __attribute__  ((constructor, used)) _##name##_module_ctor (void) \
	{ \
		if (regfn##_register (#name, entry) < 0) \
			abort (); \
	} \
\
	static void __attribute__  ((destructor, used)) _##name##_module_dtor (void) \
	{ \
		if (regfn##_unregister (#name) < 0) \
			abort (); \
	}
#elif defined (_MSC_VER)
#pragma section (".CRT$XIG", long, read)

#define AOSL_DEFINE_MODULE_REGFN(regfn, name, entry) \
	static void _##name##_module_dtor (void) \
	{ \
		if (regfn##_unregister (#name) < 0) \
			abort (); \
	} \
\
	static int _##name##_module_ctor (void) \
	{ \
		if (regfn##_register (#name, entry) < 0) \
			abort (); \
		atexit (_##name##_module_dtor); \
		return 0; \
	} \
	__declspec(allocate(".CRT$XIG")) int (*_##name##_module_ctor_f) (void) = _##name##_module_ctor;
#else
#error Unsupported Toolchain!
#endif

#define AOSL_DEFINE_MODULE(name, entry) AOSL_DEFINE_MODULE_REGFN(aosl_module, name, entry)


#ifdef __cplusplus
}
#endif


#endif /* __AOSL_MODULE_H__ */