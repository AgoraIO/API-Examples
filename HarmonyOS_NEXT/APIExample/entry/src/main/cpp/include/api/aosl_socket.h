/*************************************************************
 * Author:	Lionfore Hao (haolianfu@agora.io)
 * Date	 :	Jul 27th, 2020
 * Module:	AOSL BSD socket definitions header file
 *
 *
 * This is a part of the Advanced Operating System Layer.
 * Copyright (C) 2020 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#ifndef __AOSL_SOCKET_H__
#define __AOSL_SOCKET_H__

#include <api/aosl_types.h>
#include <api/aosl_defs.h>

#if defined (__linux__) || defined (__MACH__) || defined (__kliteos2__) || defined (_WIN32)
#if defined (__linux__) || defined (__MACH__) || defined (__kliteos2__)
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <netdb.h>
#elif defined (_WIN32)
/**
 * We MUST include 'winsock2.h' before any occurrence
 * of including 'windows.h', the fucking Windows has
 * the fucking issue that many definitions would be
 * complained redefinition if not so.
 * -- Lionfore Hao Sep 25th, 2018
 **/
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <in6addr.h>
#include <winternl.h>
#include <mswsock.h>
#endif

typedef struct in6_addr aosl_in6_addr_t;
typedef struct sockaddr_in6 aosl_sockaddr_in6_t;
typedef struct sockaddr_storage aosl_sockaddr_storage_t;
#else /* defined (__linux__) || defined (__MACH__) || defined (__kliteos2__) || defined (_WIN32) */
#ifdef __cplusplus
extern "C" {
#endif

#if defined (__kliteos__) || defined (__kaliosthings__)
#include <lwip/sockets.h>
#include <lwip/netdb.h>
#elif defined (__kspreadtrum__)
#include <DAPS/export/inc/tcpip6/socket_types.h>
#include <DAPS/export/inc/tcpip6/socket_api.h>
#endif

typedef struct aosl_in6_addr {
	union {
		uint8_t u6_addr8[16];
		uint16_t u6_addr16[8];
		uint32_t u6_addr32[4];
	} u6_addr;
} aosl_in6_addr_t;

#undef s6_addr8
#undef s6_addr16
#undef s6_addr32
#undef s6_addr

#define	s6_addr8  u6_addr.u6_addr8
#define	s6_addr16 u6_addr.u6_addr16
#define	s6_addr32 u6_addr.u6_addr32
#define	s6_addr  u6_addr.u6_addr8

typedef struct aosl_sockaddr_in6 {
	uint16_t sin6_family;
	uint16_t sin6_port;
	uint32_t sin6_flowinfo;
	aosl_in6_addr_t sin6_addr;
	uint32_t sin6_scope_id;
} aosl_sockaddr_in6_t;

/**
 * Protocol independent socket structure (RFC 2553).
 **/
#define _AOSL_SS_SIZE 128 /* Implementation specific max size */
#define _AOSL_SS_PADSIZE (_AOSL_SS_SIZE - sizeof (uint16_t) - sizeof (int64_t))

typedef struct aosl_sockaddr_storage {
	uint16_t ss_family;
	char __ss_padding [_AOSL_SS_PADSIZE];
	int64_t __ss_align; /* Force desired alignment. */
} aosl_sockaddr_storage_t;

/**
 * PLEASE BE CAREFUL ENOUGH HERE:
 * The macro __kspreadtrum__ is not a compiler toolchain
 * predefined macro, and makercore building system will
 * define this macro when building Spreadtrum system, so
 * you must GUARANTEE define it when not using makercore
 * building system.
 * Use this style name rather than Kbuild CONFIG_* just
 * want to keep consistent with __linux__/__MACH__ etc.
 * -- Lionfore Hao Aug 1st, 2020
 **/
#if defined (__kspreadtrum__)
typedef int socklen_t;

struct sockaddr {
	uint16_t sa_family;
	char sa_data [14];
};

typedef uint32_t in_addr_t;
struct in_addr {
	in_addr_t s_addr;
};

/**
 * Please keep the size of this struct is 16 bytes.
 * -- Lionfore Hao Aug 11th, 2020
 **/
struct sockaddr_in {
	uint16_t sin_family;
	uint16_t sin_port;
	struct in_addr sin_addr;
	unsigned char sin_zero [8];
};

struct hostent {
	char*           h_name;         /* Official name of host. */
	char**          h_aliases;      /* Alias list. */
	int             h_addrtype;     /* Host address type. */
	int             h_length;       /* Length of address. */
#if !defined (CONFIG_UNISOC_8910)
	int     h_cntv4;        /* count of v4 address.  */
	int     h_cntv6;        /* count of v6 address.  */
#endif
	char**          h_addr_list;    /* List of addresses from name server. */
	char  **h_addr6_list;    /* List of addresses from name server.  */
#if !defined (CONFIG_UNISOC_8910)
	int     h_cntsrv;
	char  **h_srv_list;
#endif
};

#ifndef IPPROTO_TCP
#define IPPROTO_TCP TCPIP_IPPROTO_TCP
#endif

#ifndef IPPROTO_UDP
#define IPPROTO_UDP TCPIP_IPPROTO_UDP
#endif

#ifndef INET_ADDRSTRLEN
#define INET_ADDRSTRLEN 16
#endif

#ifndef INET6_ADDRSTRLEN
#define INET6_ADDRSTRLEN 46
#endif
#endif /* __kspreadtrum__ */


#ifdef __cplusplus
}
#endif
#endif /* defined (__linux__) || defined (__MACH__) || defined (__kliteos2__) || defined (_WIN32) */


#endif /* __AOSL_SOCKET_H__ */