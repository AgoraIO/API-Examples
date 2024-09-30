/*************************************************************
 * Author:	Lionfore Hao (haolianfu@agora.io)
 * Date	 :	Jul 22nd, 2018
 * Module:	Socket helper utils header file
 *
 *
 * This is a part of the Advanced Operating System Layer.
 * Copyright (C) 2018 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#ifndef __AOSL_MPQ_NET_H__
#define __AOSL_MPQ_NET_H__

#include <api/aosl_types.h>
#include <api/aosl_defs.h>
#include <api/aosl_mpq.h>
#include <api/aosl_mpq_fd.h>
#include <api/aosl_socket.h>
#include <api/aosl_data.h>


#ifdef __cplusplus
extern "C" {
#endif

/**
 * The fucking Windows has different definitions for a 
 * 'fd' and a 'sk', so we define a aosl_sk_t type to
 * handle this issue uniformly.
 * Fucking Windows, Fucking Microsoft!
 **/
#ifndef _WIN32
typedef aosl_fd_t aosl_sk_t;
#define AOSL_INVALID_SK AOSL_INVALID_FD
#else
typedef SOCKET aosl_sk_t;
#define AOSL_INVALID_SK ((aosl_sk_t)AOSL_INVALID_FD)
#endif

#if !defined (__linux__) && !defined (__MACH__) && !defined (__kliteos__)
typedef uint16_t sa_family_t;
#endif

#define AOSL_IPHDR_LEN 20

#define AOSL_UDPHDR_LEN 8
#define AOSL_TCPHDR_LEN 20

#define AOSL_IP_UDP_HDR_LEN (AOSL_IPHDR_LEN + AOSL_UDPHDR_LEN)
#define AOSL_IP_TCP_HDR_LEN (AOSL_IPHDR_LEN + AOSL_TCPHDR_LEN)


typedef union {
	struct sockaddr sa;
	struct sockaddr_in in;
	aosl_sockaddr_in6_t in6;
#if defined (__linux__) || defined (__MACH__) || defined (__kliteos2__)
	struct sockaddr_un un;
#endif
	aosl_sockaddr_storage_t __storage;
} aosl_sk_addr_t;

typedef struct {
	aosl_fd_t newsk;
	aosl_sk_addr_t addr;
	socklen_t addrlen;
} aosl_accept_data_t;

typedef struct {
	aosl_sk_addr_t addr;
	socklen_t addrlen;
} aosl_sk_addr_and_len_t;

/**
 * The listen state socket readable callback function type
 * Parameters:
 *    data: the data buffer holding the packet
 *     len: the data length in bytes in the buffer
 *    argc: the args count when adding the fd
 *    argv: the args vector when adding the fd
 *    addr: the socket address data received from
 * Return value:
 *    None.
 **/
typedef void (*aosl_sk_accepted_t) (aosl_accept_data_t *accept_data, size_t len, uintptr_t argc, uintptr_t argv []);

/**
 * The dgram socket received data callback function type
 * Parameters:
 *    data: the data buffer holding the packet
 *     len: the data length in bytes in the buffer
 *    argc: the args count when adding the fd
 *    argv: the args vector when adding the fd
 *    addr: the socket address data received from
 * Return value:
 *    None.
 **/
typedef void (*aosl_dgram_sk_data_t) (void *data, size_t len, uintptr_t argc, uintptr_t argv [], const aosl_sk_addr_t *addr);

extern __aosl_api__ aosl_fd_t aosl_socket (int domain, int type, int protocol);
extern __aosl_api__ int aosl_bind (aosl_fd_t sockfd, const struct sockaddr *addr, socklen_t addrlen);
extern __aosl_api__ int aosl_getsockname (aosl_fd_t sockfd, struct sockaddr *addr, socklen_t *addrlen);
extern __aosl_api__ int aosl_getpeername (aosl_fd_t sockfd, struct sockaddr *addr, socklen_t *addrlen);
extern __aosl_api__ int aosl_getsockopt (aosl_fd_t sockfd, int level, int optname, void *optval, socklen_t *optlen);
extern __aosl_api__ int aosl_setsockopt (aosl_fd_t sockfd, int level, int optname, const void *optval, socklen_t optlen);


extern __aosl_api__ int aosl_mpq_add_dgram_socket (aosl_mpq_t qid, aosl_fd_t fd, size_t max_pkt_size,
							aosl_dgram_sk_data_t data_f, aosl_fd_event_t event_f, uintptr_t argc, ...);

extern __aosl_api__ int aosl_mpq_add_stream_socket (aosl_mpq_t qid, aosl_fd_t fd, int enable, size_t max_pkt_size,
			aosl_check_packet_t chk_pkt_f, aosl_fd_data_t data_f, aosl_fd_event_t event_f, uintptr_t argc, ...);

extern __aosl_api__ int aosl_mpq_add_listen_socket (aosl_mpq_t qid, aosl_fd_t fd, int backlog,
				aosl_sk_accepted_t accepted_f, aosl_fd_event_t event_f, uintptr_t argc, ...);

extern __aosl_api__ int aosl_mpq_connect (aosl_fd_t fd, const struct sockaddr *dest_addr, socklen_t addrlen, int timeo);
extern __aosl_api__ int aosl_mpq_listen (aosl_fd_t fd, int backlog);

extern __aosl_api__ ssize_t aosl_send (aosl_fd_t sockfd, const void *buf, size_t len, int flags);
extern __aosl_api__ ssize_t aosl_sendto (aosl_fd_t sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen);

extern __aosl_api__ int aosl_ip_sk_bind_port_only (aosl_fd_t sk, sa_family_t af, unsigned short port);


typedef struct {
	aosl_fd_t v4;
	aosl_fd_t v6;
} aosl_ip_sk_t;

static __inline__ void aosl_ip_sk_init (aosl_ip_sk_t *sk)
{
	sk->v4 = AOSL_INVALID_FD;
	sk->v6 = AOSL_INVALID_FD;
}

extern __aosl_api__ int aosl_ip_sk_create (aosl_ip_sk_t *sk, int type, int protocol);

typedef struct {
	struct sockaddr_in v4;
	aosl_sockaddr_in6_t v6;
} aosl_ip_addr_t;

extern __aosl_api__ void aosl_ip_addr_init (aosl_ip_addr_t *addr);
extern __aosl_api__ int aosl_ip_sk_bind (const aosl_ip_sk_t *sk, const aosl_ip_addr_t *addr);

extern __aosl_api__ int aosl_mpq_ip_sk_connect (const aosl_ip_sk_t *sk, const struct sockaddr *dest_addr, int timeo);

extern __aosl_api__ ssize_t aosl_ip_sk_sendto (const aosl_ip_sk_t *sk, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr);
extern __aosl_api__ void aosl_ip_sk_close (aosl_ip_sk_t *sk);

extern __aosl_api__ int aosl_ipv6_addr_v4_mapped (const aosl_in6_addr_t *a6);
extern __aosl_api__ int aosl_ipv6_addr_nat64 (const aosl_in6_addr_t *a6);
extern __aosl_api__ int aosl_ipv6_addr_v4_compatible (const aosl_in6_addr_t *a6);

extern __aosl_api__ int aosl_ipv6_sk_addr_from_ipv4 (struct sockaddr *sk_addr_v6, const struct sockaddr *sk_addr_v4);
extern __aosl_api__ int aosl_ipv6_sk_addr_to_ipv4 (struct sockaddr *sk_addr_v6, const struct sockaddr *sk_addr_v4);


extern __aosl_api__ int aosl_ip_sk_addr_init_with_port (aosl_sk_addr_t *sk_addr, sa_family_t af, unsigned short port);

/* Structure for describing a resolved sock address information */
typedef struct {
	sa_family_t sk_af;
	int sk_type;
	int sk_prot;
	aosl_sk_addr_t sk_addr;
	socklen_t sk_addr_len;
} aosl_sk_addrinfo_t;


extern __aosl_api__ int aosl_sk_addr_ip_equal (const struct sockaddr *addr1, const struct sockaddr *addr2);
extern __aosl_api__ socklen_t aosl_sk_addr_len (const struct sockaddr *addr);

extern __aosl_api__ const char *aosl_inet_addr_str (int af, const void *addr, char *addr_buf, size_t buf_len);
extern __aosl_api__ const char *aosl_ip_sk_addr_str (const aosl_sk_addr_t *addr, char *addr_buf, size_t buf_len);
extern __aosl_api__ unsigned short aosl_ip_sk_addr_port (const aosl_sk_addr_t *addr);

extern __aosl_api__ socklen_t aosl_inet_addr_from_string (void *addr, const char *str_addr);
extern __aosl_api__ socklen_t aosl_ip_sk_addr_from_string (aosl_sk_addr_t *sk_addr, const char *str_addr, uint16_t port);

extern __aosl_api__ int aosl_net_get_ipv6_prefix (aosl_in6_addr_t *prefix);
extern __aosl_api__ void aosl_net_set_ipv6_prefix (const aosl_in6_addr_t *a6);


/**
 * Resolve host name asynchronously relative functions, these functions would queue back a function call
 * specified by f to the mpq object specified by q with the specified args.
 **/
extern __aosl_api__ int aosl_resolve_host_async (aosl_ref_t ref, const char *hostname, unsigned short port, aosl_sk_addrinfo_t *addrs, size_t addr_count, aosl_mpq_t q, aosl_mpq_func_argv_t f, uintptr_t argc, ...);
extern __aosl_api__ int aosl_resolve_host_asyncv (aosl_ref_t ref, const char *hostname, unsigned short port, aosl_sk_addrinfo_t *addrs, size_t addr_count, aosl_mpq_t q, aosl_mpq_func_argv_t f, uintptr_t argc, va_list args);
extern __aosl_api__ int aosl_tcp_resolve_host_async (aosl_ref_t ref, const char *hostname, unsigned short port, aosl_sk_addrinfo_t *addrs, size_t addr_count, aosl_mpq_t q, aosl_mpq_func_argv_t f, uintptr_t argc, ...);
extern __aosl_api__ int aosl_tcp_resolve_host_asyncv (aosl_ref_t ref, const char *hostname, unsigned short port, aosl_sk_addrinfo_t *addrs, size_t addr_count, aosl_mpq_t q, aosl_mpq_func_argv_t f, uintptr_t argc, va_list args);
extern __aosl_api__ int aosl_udp_resolve_host_async (aosl_ref_t ref, const char *hostname, unsigned short port, aosl_sk_addrinfo_t *addrs, size_t addr_count, aosl_mpq_t q, aosl_mpq_func_argv_t f, uintptr_t argc, ...);
extern __aosl_api__ int aosl_udp_resolve_host_asyncv (aosl_ref_t ref, const char *hostname, unsigned short port, aosl_sk_addrinfo_t *addrs, size_t addr_count, aosl_mpq_t q, aosl_mpq_func_argv_t f, uintptr_t argc, va_list args);

/**
 * The DNS resolving function which supports coroutine.
 * Parameters:
 *      d_count: the data object for holding the resolved addresses
 *               count, the length must >= sizeof (size_t);
 *   d_hostname: the data object for holding the resolving hostname;
 *      d_addrs: the data object for holding the resolved addresses,
 *               this is an array of aosl_sk_addrinfo_t in fact;
 * Return value:
 *       0: async DNS resolve issued successfully;
 *      <0: failed with aosl_errno set;
 **/
extern __aosl_api__ int aosl_co_resolve_host (aosl_data_t d_count, aosl_data_t d_hostname, aosl_data_t d_addrs);
extern __aosl_api__ int aosl_co_resolve_tcp_host (aosl_data_t d_count, aosl_data_t d_hostname, aosl_data_t d_addrs);
extern __aosl_api__ int aosl_co_resolve_udp_host (aosl_data_t d_count, aosl_data_t d_hostname, aosl_data_t d_addrs);

/**
 * This is just a wrapper implementation of listen syscall for
 * cross platforms supporting.
 **/
extern __aosl_api__ int aosl_listen (aosl_fd_t sk, int backlog);

/**
 * The socket accept function which supports coroutine.
 * Parameters:
 *      fd: the socket fd;
 *   d_ret: the data object for holding return value, it is
 *          the accepted new socket fd, so the length must
 *          >= sizeof (aosl_fd_t);
 *  d_addr: the data object for holding the remote address,
 *          the len must >= sizeof (aosl_sk_addr_and_len_t);
 * Return value:
 *      <0: failed with aosl_errno set;
 *       0: async accept issued successfully;
 **/
extern __aosl_api__ int aosl_co_accept (aosl_fd_t fd, aosl_data_t d_ret, aosl_data_t d_addr);

/**
 * The socket recv function which supports coroutine.
 * Parameters:
 *      fd: the socket fd;
 *   d_ret: the data object for holding return value, the
 *          length must >= sizeof (ssize_t);
 *   d_buf: the buffer for holding the read data;
 * buf_off: the buffer offset for holding the read data;
 *   flags: the flags same as recv syscall;
 * Return value:
 *      <0: failed with aosl_errno set;
 *       0: async read issued successfully;
 **/
extern __aosl_api__ int aosl_co_recv (aosl_fd_t fd, aosl_data_t d_ret, aosl_data_t d_buf, uintptr_t buf_off, int flags);

/**
 * The socket recv function which supports coroutine.
 * Parameters:
 *      fd: the socket fd;
 *   d_ret: the data object for holding return value, the
 *          length must >= sizeof (ssize_t);
 *   d_buf: the buffer for holding the read data;
 * buf_off: the buffer offset for holding the read data;
 *   flags: the flags same as recvfrom syscall;
 *  d_addr: the data object for holding the remote address,
 *          the len must >= sizeof (aosl_sk_addr_and_len_t);
 * Return value:
 *      <0: failed with aosl_errno set;
 *       0: async read issued successfully;
 **/
extern __aosl_api__ int aosl_co_recvfrom (aosl_fd_t fd, aosl_data_t d_ret, aosl_data_t d_buf, uintptr_t buf_off, int flags, aosl_data_t d_addr);

/**
 * The socket connect function which supports coroutine.
 * Parameters:
 *           fd: the socket fd;
 *        d_ret: the data object for holding the async connect
 *               done return value, it can be NULL, but if set
 *               the length must >= sizeof (int);
 *    dest_addr: the target address;
 *      addrlen: the target address size;
 *        timeo: the flags same as send syscall;
 * Return value:
 *      <0: failed with aosl_errno set;
 *       0: async connect issued successfully;
 **/
extern __aosl_api__ int aosl_co_connect (aosl_fd_t fd, aosl_data_t d_ret, const struct sockaddr *dest_addr, socklen_t addrlen, int timeo);

/**
 * The IPv4/IPv6 socket connect function which supports coroutine.
 * Parameters:
 *           sk: the IPv4/IPv6 sockets;
 *        d_ret: the data object for holding the async connect
 *               done return value, it can be NULL, but if set
 *               the length must >= sizeof (int);
 *    dest_addr: the target address;
 *        timeo: the flags same as send syscall;
 * Return value:
 *      <0: failed with aosl_errno set;
 *       0: async connect issued successfully;
 **/
extern __aosl_api__ int aosl_co_ip_sk_connect (const aosl_ip_sk_t *sk, aosl_data_t d_ret, const struct sockaddr *dest_addr, int timeo);

/**
 * The socket send function which supports coroutine.
 * Parameters:
 *      fd: the socket fd;
 *   d_ret: the data object for holding the async write
 *          done return value, it can be NULL, but if
 *          set the length must >= sizeof (ssize_t);
 *   d_buf: the buffer for holding the data for writing;
 *   flags: the flags same as send syscall;
 * Return value:
 *      <0: failed with aosl_errno set;
 *     >=0: successfully;
 **/
extern __aosl_api__ ssize_t aosl_co_send (aosl_fd_t fd, aosl_data_t d_ret, aosl_data_t d_buf, int flags);

/**
 * The socket sendto function which supports coroutine.
 * Parameters:
 *           fd: the socket fd;
 *        d_ret: the data object for holding the async write
 *               done return value, it can be NULL, but if
 *               set the length must >= sizeof (ssize_t);
 *        d_buf: the buffer for holding the data for writing;
 *        flags: the flags same as sendto syscall;
 *    dest_addr: the target address;
 *      addrlen: the target address size;
 * Return value:
 *      <0: failed with aosl_errno set;
 *     >=0: successfully;
 **/
extern __aosl_api__ ssize_t aosl_co_sendto (aosl_fd_t fd, aosl_data_t d_ret, aosl_data_t d_buf, int flags,
													const struct sockaddr *dest_addr, socklen_t addrlen);

/**
 * The socket send function which supports coroutine.
 * Parameters:
 *      fd: the socket fd;
 *   d_ret: the data object for holding the async write
 *          done return value, it can be NULL, but if
 *          set the length must >= sizeof (ssize_t);
 *     buf: the buffer for holding the data for writing;
 *     len: the writing data bytes;
 *   flags: the flags same as send syscall;
 * Return value:
 *      <0: failed with aosl_errno set;
 *     >=0: successfully;
 **/
extern __aosl_api__ ssize_t aosl_co_buf_send (aosl_fd_t fd, aosl_data_t d_ret, const void *buf, size_t len, int flags);

/**
 * The socket sendto function which supports coroutine.
 * Parameters:
 *           fd: the socket fd;
 *        d_ret: the data object for holding the async write
 *               done return value, it can be NULL, but if
 *               set the length must >= sizeof (ssize_t);
 *          buf: the buffer for holding the data for writing;
 *          len: the writing data bytes;
 *        flags: the flags same as sendto syscall;
 *    dest_addr: the target address;
 *      addrlen: the target address size;
 * Return value:
 *      <0: failed with aosl_errno set;
 *     >=0: successfully;
 **/
extern __aosl_api__ ssize_t aosl_co_buf_sendto (aosl_fd_t fd, aosl_data_t d_ret, const void *buf, size_t len,
											int flags, const struct sockaddr *dest_addr, socklen_t addrlen);

/**
 * The IPv4/IPv6 socket sendto function which supports coroutine.
 * Parameters:
 *           sk: the IPv4/IPv6 sockets;
 *        d_ret: the data object for holding the async write
 *               done return value, it can be NULL, but if
 *               set the length must >= sizeof (ssize_t);
 *        d_buf: the buffer for holding the data for writing;
 *        flags: the flags same as sendto syscall;
 *    dest_addr: the target address;
 * Return value:
 *      <0: failed with aosl_errno set;
 *     >=0: successfully;
 **/
__export_in_so__ ssize_t aosl_co_ip_sk_sendto (const aosl_ip_sk_t *sk, aosl_data_t d_ret, aosl_data_t d_buf,
															int flags, const struct sockaddr *dest_addr);

/**
 * The IPv4/IPv6 socket sendto function which supports coroutine.
 * Parameters:
 *           sk: the IPv4/IPv6 sockets;
 *        d_ret: the data object for holding the async write
 *               done return value, it can be NULL, but if
 *               set the length must >= sizeof (ssize_t);
 *          buf: the buffer for holding the data for writing;
 *          len: the writing data bytes;
 *        flags: the flags same as sendto syscall;
 *    dest_addr: the target address;
 * Return value:
 *      <0: failed with aosl_errno set;
 *     >=0: successfully;
 **/
extern __aosl_api__ ssize_t aosl_co_ip_sk_buf_sendto (const aosl_ip_sk_t *sk, aosl_data_t d_ret, const void *buf,
														size_t len, int flags, const struct sockaddr *dest_addr);


#ifdef __cplusplus
}
#endif


#endif /* __AOSL_MPQ_NET_H__ */