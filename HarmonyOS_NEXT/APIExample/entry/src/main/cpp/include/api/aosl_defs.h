/*************************************************************
 * Author:	Lionfore Hao (haolianfu@agora.io)
 * Date	 :	Jul 21st, 2018
 * Module:	AOSL common definitions header file
 *
 *
 * This is a part of the Advanced Operating System Layer.
 * Copyright (C) 2018 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#ifndef __AOSL_DEFS_H__
#define __AOSL_DEFS_H__


#define aosl_stringify_1(x) #x
#define aosl_stringify(x) aosl_stringify_1(x)


#ifndef __MAKERCORE_ASSEMBLY__

#ifdef __cplusplus
extern "C" {
#endif


#ifdef _MSC_VER
#ifndef __inline__
#define __inline__ __inline
#endif
#endif


#ifndef container_of
#if defined (__GNUC__)
#define container_of(ptr, type, member) ({ \
	const typeof( ((type *)0)->member ) *__mptr = (void *)(ptr); \
	(type *)( (char *)__mptr - offsetof(type,member) );})
#else
#define container_of(ptr, type, member) ((type *)((char *)(ptr) - offsetof(type,member)))
#endif
#endif

#define aosl_rela_addr(type, field) (&((type *)0)->field)
#define aosl_base_addr(ptr, type, field) \
	((type *)((uintptr_t)(ptr) - (uintptr_t)(&((type *)0)->field)))


#define aosl_min(x, y) ((x) < (y) ? (x) : (y))
#define aosl_max(x, y) ((x) > (y) ? (x) : (y))


/* I think 64 args is big enough */
#define AOSL_VAR_ARGS_MAX 64


#ifdef BUILD_TARGET_SHARED
#if defined (__GNUC__)
#define __export_in_so__ __attribute__ ((visibility ("default")))
#elif defined (_MSC_VER)
#define __export_in_so__ __declspec (dllexport)
#endif
#else
#define __export_in_so__
#endif


#ifndef __aosl_api__
#if defined (_MSC_VER) && defined (BUILDING_API_IMPL_SOURCE) && defined (BUILD_TARGET_SHARED)
#define __aosl_api__ __declspec (dllexport)
#elif defined (_MSC_VER) && !defined (BUILDING_API_IMPL_SOURCE)
#define __aosl_api__ __declspec (dllimport)
#else
#define __aosl_api__
#endif
#endif

#if defined (BUILDING_API_IMPL_SOURCE) || defined (STATIC_LINKING_AOSL)

#if defined (__GNUC__)
#define __so_api__ __attribute__ ((visibility ("default")))
#elif defined (_MSC_VER)
#define __so_api__ __declspec (dllexport)
#else
#define __so_api__
#endif

#else

#if defined (_MSC_VER)
#define __so_api__ __declspec (dllimport)
#else
#define __so_api__
#endif

#endif


#ifdef __GNUC__
#ifndef __MACH__
#define AOSL_DEFINE_BIN(v, f) \
__asm__ (".section .rodata\n\t" \
	".globl "#v"_bin_begin\n\t" \
	".hidden "#v"_bin_begin\n\t" \
	".align 4\n\t" \
	#v"_bin_begin:\n\t" \
	".incbin \"" aosl_stringify(MAKERCORE_THIS_FILE_DIR/f)"\"\n\t" \
	".globl "#v"_bin_end\n\t" \
	".hidden "#v"_bin_end\n\t" \
	#v"_bin_end:\n\t" \
	".previous\n\t")
#else
#define AOSL_DEFINE_BIN(v, f) \
__asm__ (".section __TEXT,__const\n\t" \
	".globl _"#v"_bin_begin\n\t" \
	".private_extern _"#v"_bin_begin\n\t" \
	".align 4\n\t" \
	"_"#v"_bin_begin:\n\t" \
	".incbin \"" aosl_stringify (MAKERCORE_THIS_FILE_DIR/f) "\"\n\t" \
	".globl _"#v"_bin_end\n\t" \
	".private_extern _"#v"_bin_end\n\t" \
	"_"#v"_bin_end:\n\t" \
	".previous\n\t")
#endif

#define AOSL_DECLARE_BIN(v) extern unsigned char v##_bin_begin, v##_bin_end
#define AOSL_BIN_ADDR(v) ((void *)&v##_bin_begin)
#define AOSL_BIN_SIZE(v) ((size_t)((unsigned char *)&v##_bin_end - (unsigned char *)&v##_bin_begin))
#endif


#ifdef __cplusplus
}
#endif

#else /* __MAKERCORE_ASSEMBLY__ */

#ifdef __GNUC__
#ifndef __MACH__
.macro AOSL_DEFINE_BIN_S v, f
	.section .rodata
	.globl \v\()_bin_begin
	.hidden \v\()_bin_begin
	.align 4
	\v\()_bin_begin:
	.incbin aosl_stringify (MAKERCORE_THIS_FILE_DIR/\f)
	.globl \v\()_bin_end
	.hidden \v\()_bin_end
	\v\()_bin_end:
	.previous
.endm
#else
.macro AOSL_DEFINE_BIN_S v, f
	.section __TEXT,__const
	.globl _\()\v\()_bin_begin
	.private_extern _\()\v\()_bin_begin
	.align 4
	_\()\v\()_bin_begin:
	.incbin aosl_stringify (MAKERCORE_THIS_FILE_DIR/\f)
	.globl _\()\v\()_bin_end
	.private_extern _\()\v\()_bin_end
	_\()\v\()_bin_end:
	.previous
.endm
#endif
#endif

#endif /* __MAKERCORE_ASSEMBLY__ */

#endif /* __AOSL_DEFS_H__ */