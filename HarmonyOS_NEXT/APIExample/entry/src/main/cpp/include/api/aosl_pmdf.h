/*************************************************************
 * Author:	Lionfore Hao (haolianfu@agora.io)
 * Date	 :	Jun 27th, 2021
 * Module:	AOSL performance monitor dump file format
 *
 *
 * This is a part of the Advanced Operating System Layer.
 * Copyright (C) 2021 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#ifndef	__AOSL_PMDF_H__
#define	__AOSL_PMDF_H__

#include <api/aosl_defs.h>
#include <api/aosl_types.h>


#ifdef __cplusplus
extern "C" {
#endif


#define APMDF_NMAGIC 6

typedef struct apmdf_fhdr {
	char f_magic [APMDF_NMAGIC]; /* AOSLPM */
	uint16_t f_hchksum;
	uint8_t f_type;
	uint8_t f_version;
	uint8_t f_machine;
	uint8_t f_os;
	uint16_t f_cpus;
	uint16_t f_shentsize;
	uint16_t f_shcount;
	uint16_t f_shdatidx;
	uint32_t f_shoff;
	uint32_t f_size;
	uint32_t f_period;
	uint32_t f_totalhits;
	uint32_t f_fchksum;
} aosl_pmdf_fhdr_t;

#define APMDF_TYPE_PERF 0x1

#define APMDF_ARCH_ARM 0x1
#define APMDF_ARCH_X86 0x2
#define APMDF_ARCH_MIPS 0x3
#define APMDF_ARCH_PPC 0x4
#define APMDF_ARCH_RISCV 0x5
#define APMDF_ARCH_OPENRISC 0x6
#define APMDF_ARCH_SPARC 0x7

#define APMDF_ARCH_64BIT 0x10
#define APMDF_EB_BIG 0x20

#define APMDF_OS_LINUX 0x1
#define APMDF_OS_ANDROID 0x2
#define APMDF_OS_MACOSX 0x3
#define APMDF_OS_IOS 0x4
#define APMDF_OS_WINDOWS 0x5
#define APMDF_OS_SPREADTRUM 0x6
#define APMDF_OS_LITEOS 0x7
#define APMDF_OS_LITEOS2 0x8

#define APMDF_ST_THRTAB 0x1
#define APMDF_ST_IMGTAB 0x2
#define APMDF_ST_SYMTAB 0x3
#define APMDF_ST_CNTTAB 0x4
#define APMDF_ST_DATTAB 0x5
#define APMDF_ST_TLVTAB 0x6

typedef struct apmdf_shdr {
	uint16_t s_type;
	uint16_t s_entsize; /* if being a table */
	uint32_t s_size;
	uint32_t s_off;
} aosl_pmdf_shdr_t;

typedef struct apmdf_thr_ent {
	uint32_t thrd_l;
	uint32_t thrd_h; /* just for future's potential OSes */
	int32_t name; /* Only valid if none negative */
	uint16_t native;
	uint16_t reserved;
	uint32_t sys_us; /* sys micro seconds */
	uint32_t usr_us; /* usr micro seconds */
} aosl_pmdf_thr_ent_t;

typedef struct apmdf_img_ent {
	uint32_t start_l;
	uint32_t start_h;
	uint32_t name;
} aosl_pmdf_img_ent_t;

typedef struct apmdf_sym_ent {
	uint32_t sym_val;
	uint32_t sym_name;
} aosl_pmdf_sym_ent_t;

typedef struct apmdf_cnt_ent {
	uint16_t native;
	uint16_t img;
	uint32_t addr_off;
	int32_t sym; /* Only valid if none negative */
	uint32_t hits;
} aosl_pmdf_cnt_ent_t;

typedef struct apmdf_tlv_ent {
	uint16_t v_tag;
	uint16_t v_len;
	uint32_t v_voo; /* value or off */
} aosl_pmdf_tlv_ent_t;


#ifdef __cplusplus
}
#endif


#endif /* __AOSL_PMDF_H__ */
