/*************************************************************
 * Author:	Lionfore Hao (haolianfu@agora.io)
 * Date	 :	Jul 29th, 2018
 * Module:	Network route relative functionals header file
 *
 *
 * This is a part of the Advanced Operating System Layer.
 * Copyright (C) 2018 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#ifndef __AOSL_NET_API_H__
#define __AOSL_NET_API_H__

#include <api/aosl_types.h>
#include <api/aosl_defs.h>
#include <api/aosl_mpq_net.h>


#ifdef __cplusplus
extern "C" {
#endif


typedef enum {
	AOSL_NET_EV_NONE = 0,
	AOSL_NET_EV_NET_DOWN,
	AOSL_NET_EV_NET_UP,
	AOSL_NET_EV_NET_UP_CHANGED,
	AOSL_NET_EV_NET_SWITCH,
} aosl_net_ev_t;

typedef struct {
	int if_index;
	int if_type;
	char if_name [64];
} aosl_netif_t;

typedef struct {
	aosl_netif_t netif;
	int if_cellnet;
	aosl_sk_addr_t gw;
} aosl_rt_t;

typedef struct {
	aosl_rt_t IPv4;
	aosl_rt_t IPv6;
} aosl_def_rt_t;

extern __aosl_api__ void aosl_init_def_rt (aosl_def_rt_t *def_rt);

extern __aosl_api__ void aosl_invalidate_rt (aosl_rt_t *rt);
extern __aosl_api__ void aosl_invalidate_def_rt (aosl_def_rt_t *def_rt);

extern __aosl_api__ int aosl_rt_valid (const aosl_rt_t *rt);
extern __aosl_api__ int aosl_def_rt_valid (const aosl_def_rt_t *def_rt);

extern __aosl_api__ int aosl_network_is_down ();

extern __aosl_api__ int aosl_same_rt (const aosl_rt_t *rt1, const aosl_rt_t *rt2);
extern __aosl_api__ int aosl_same_def_rt (const aosl_def_rt_t *def_rt1, const aosl_def_rt_t *def_rt2);

extern __aosl_api__ const char *aosl_rt_str (const aosl_rt_t *rt, char buf [], size_t buf_size);
extern __aosl_api__ const char *aosl_def_rt_str (const aosl_def_rt_t *def_rt, char buf [], size_t buf_size);


typedef void (*aosl_net_ev_func_t) (aosl_net_ev_t ev, void *arg, ...);

/**
 * Subscribe/Unsubscribe the low level network events.
 * If f is not NULL, then subscribe the low level network events, and the callback
 * function specified by f will be invoked in the calling thread context. Please
 * note that only one subscriber allowed in the whole process.
 * If f is NULL, then unsubscribe the low level network events.
 * Parameters:
 *       f: the callback function
 *     arg: the parameter which will be passed when invoking the f callback
 * Return values:
 *     <0: failed with aosl_errno set
 *      0: successful
 **/
extern __aosl_api__ int aosl_subscribe_net_events (aosl_net_ev_func_t f, void *arg);

/**
 * Return value is the default route count both for IPv4 and IPv6 we retrieved.
 *      0: no default route
 *      1: only one of IPv4 or IPv6 has default route
 *      2: both IPv4 and IPv6 have default route
 **/
extern __aosl_api__ int aosl_get_default_rt (aosl_def_rt_t *def_rt);


extern __aosl_api__ int aosl_is_mobile_net (sa_family_t af);


enum {
	AOSL_ROUTE_T_NONE = -1, /* No network available now */
	AOSL_ROUTE_T_MOBILE = 0, /* Mobile network */
	AOSL_ROUTE_T_LAN = 1, /* Lan network */
};

/**
 * Notify the network has changed, such as Mobile -> Lan
 * or Vice Versa.
 * Parameters:
 *        net_type: Active network type
 *                     AOSL_ROUTE_T_NONE: No active network
 *                   AOSL_ROUTE_T_MOBILE: Mobile network active
 *                      AOSL_ROUTE_T_LAN: Lan network active
 *             arg: OS specific parameter;
 * Return values:
 *     None.
 **/
extern __aosl_api__ void aosl_net_route_changed (int net_type, uintptr_t arg);


#ifdef __cplusplus
}
#endif


#endif /* __AOSL_NET_API_H__ */
