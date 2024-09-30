/*************************************************************
 * Author:	Lionfore Hao (haolianfu@agora.io)
 * Date	 :	Jul 17th, 2018
 * Module:	Multiplex queue iofd header file
 *
 *
 * This is a part of the Advanced Operating System Layer.
 * Copyright (C) 2018 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#ifndef __AOSL_MPQ_FD_H__
#define __AOSL_MPQ_FD_H__


#include <api/aosl_types.h>
#include <api/aosl_defs.h>
#include <api/aosl_mpq.h>
#include <api/aosl_data.h>

#ifdef __cplusplus
extern "C" {
#endif


/**
 * The user provided read/write callback function, just use
 * AOSL_DEFAULT_READ_FN/AOSL_DEFAULT_WRITE_FN respectively
 * for read/write for most cases, except the read/write ops
 * are special, such as an 'ioctl' supplied read/write.
 * Please make sure that you can only do the actual I/O ops
 * in these callback functions, and return '-error' when
 * encounter errors, MUST NOT close the io fd in these
 * callback functions.
 * -- Lionfore Hao Nov 9th, 2018
 **/
#ifndef _WIN32
typedef ssize_t (*aosl_fd_read_t) (aosl_fd_t fd, void *buf, size_t len, size_t extra_size, uintptr_t argc, uintptr_t argv []);
typedef ssize_t (*aosl_fd_write_t) (aosl_fd_t fd, const void *buf, size_t len, size_t extra_size, uintptr_t argc, uintptr_t argv []);
#else
typedef ssize_t (*aosl_fd_read_t) (aosl_fd_t fd, void *buf, size_t len, size_t extra_size, OVERLAPPED *ovlp, uintptr_t argc, uintptr_t argv []);
typedef ssize_t (*aosl_fd_write_t) (aosl_fd_t fd, const void *buf, size_t len, size_t extra_size, OVERLAPPED *ovlp, uintptr_t argc, uintptr_t argv []);
#endif

#define AOSL_DEFAULT_READ_FN ((aosl_fd_read_t)1)
#define AOSL_DEFAULT_WRITE_FN ((aosl_fd_write_t)1)

/**
 * The data packet check callback function is provided by the user for determining
 * whether the data has contained a complet user level packet, as following:
 * Parameters:
 *    data: the data buffer holding the packet
 *     len: the data length in bytes in the buffer
 *    argc: the args count when adding the fd
 *    argv: the args vector when adding the fd
 * Return value:
 *      <0: indicate some error occurs;
 *       0: no error, but the whole packet is not ready so far;
 *      >0: the whole packet length
 **/
typedef ssize_t (*aosl_check_packet_t) (const void *data, size_t len, uintptr_t argc, uintptr_t argv []);

/**
 * The iofd received data callback function type
 * Parameters:
 *    data: the data buffer holding the packet
 *     len: the data length in bytes in the buffer
 *    argc: the args count when adding the fd
 *    argv: the args vector when adding the fd
 * Return value:
 *    None.
 **/
typedef void (*aosl_fd_data_t) (void *data, size_t len, uintptr_t argc, uintptr_t argv []);

/**
 * The iofd event callback function type
 * Parameters:
 *      fd: the fd
 *   event: if <0, then indicates an error occurs, and the error number is '-event';
 *          if >=0, then indicates an event needs to be reported to application;
 *    argc: the args count when adding the fd
 *    argv: the args vector when adding the fd
 * Return value:
 *    None.
 **/
/* event values when no error */
#define AOSL_IOFD_READY_FOR_WRITING 1
#define AOSL_IOFD_HUP -19999
#define AOSL_IOFD_ERR -20000

/**
 * The fd event callback function.
 * Parameters:
 *      fd: the fd itself
 *   event: the reporting event, values are as following:
 *         <0: an error occurs:
 *             [-19999 ~ -1]: OS error and '-event' indicates the OS error number;
 *                 <= -20000: SDK defined errors such as HUP etc;
 *    argc: the args count passed in when adding the fd
 *    argv: the args vector passed in when adding the fd
 * Return value:
 *    None.
 **/
typedef void (*aosl_fd_event_t) (aosl_fd_t fd, int event, uintptr_t argc, uintptr_t argv []);

extern __aosl_api__ int aosl_mpq_add_fd (aosl_mpq_t qid, aosl_fd_t fd, int enable, size_t max_pkt_size,
						aosl_fd_read_t read_f, aosl_fd_write_t write_f, aosl_check_packet_t chk_pkt_f,
									aosl_fd_data_t data_f, aosl_fd_event_t event_f, uintptr_t argc, ...);

extern __aosl_api__ int aosl_mpq_enable_fd (aosl_fd_t fd);

extern __aosl_api__ ssize_t aosl_write (aosl_fd_t fd, const void *buf, size_t len);

#define AOSL_FD_EVM_SET 0 /* fd event mask set */
#define AOSL_FD_EVM_ADD 1 /* fd event mask add */
#define AOSL_FD_EVM_DEL 2 /* fd event mask del */

#define AOSL_FD_EV_I 0x1 /* fd event input */
#define AOSL_FD_EV_O 0x2 /* fd event output */

/**
 * Modify the fd listening event mask.
 * Parameters:
 *           fd: the fd you want to modify event mask
 *         ctrl: one of AOSL_FD_EVM_SET, AOSL_FD_EVM_ADD or AOSL_FD_EVM_DEL
 *               for setting, adding or deleting event mask respectively
 *    eventmask: bitmask of AOSL_FD_EV_I and AOSL_FD_EV_O
 * Return value:
 *    <0: error occured, and aosl_errno indicates which error;
 *     0: call successful, and '*arg' is value of the N-th argument;
 * Remarks:
 *    This function is used for flow control normally, if we do not
 *    want to receive data from fd now, then we can remove the bit
 *    AOSL_FD_EV_I from the event mask, and add it again later
 *    when needed.
 **/
extern __aosl_api__ int aosl_fd_evm_ctrl (aosl_fd_t fd, int ctrl, int eventmask);

/**
 * Clear the fd error.
 * Parameters:
 *    fd: the fd you want to clear error
 * Return value:
 *    <0: error occured, and aosl_errno indicates which error;
 *     0: call successful, and '*arg' is value of the N-th argument;
 * Remarks:
 *    The fd errors are also edge triggered, so if some error is not real
 *    error in fact, you must clear the error manually, otherwise the fd
 *    would be in an incorrect state and could not work normally.
 **/
extern __aosl_api__ int aosl_fd_clear_err (aosl_fd_t fd);

/**
 * Get the N-th argument of the mpq attached fd.
 * Parameters:
 *    fd: the fd you want to retrieve the arg
 *     n: which argument you want to get, the first arg is 0;
 *   arg: the argument variable address to save the argument value;
 * Return value:
 *    <0: error occured, and aosl_errno indicates which error;
 *     0: call successful, and '*arg' is value of the N-th argument;
 **/
extern __aosl_api__ int aosl_mpq_fd_arg (aosl_fd_t fd, uintptr_t n, uintptr_t *arg);

extern __aosl_api__ int aosl_mpq_disable_fd (aosl_fd_t fd);
extern __aosl_api__ int aosl_mpq_del_fd (aosl_fd_t fd);
extern __aosl_api__ int aosl_close (aosl_fd_t fd);


/**
 * The I/O device read function which supports coroutine.
 * Parameters:
 *      fd: the opened device fd;
 *   d_ret: the data object for holding return value, the
 *          length must >= sizeof (ssize_t);
 *   d_buf: the buffer for holding the read data;
 * buf_off: the buffer offset for holding the read data;
 * Return value:
 *      <0: failed with aosl_errno set;
 *       0: async read issued successfully;
 **/
extern __aosl_api__ int aosl_co_read (aosl_fd_t fd, aosl_data_t d_ret, aosl_data_t d_buf, uintptr_t buf_off);

/**
 * The I/O device write function which supports coroutine.
 * Parameters:
 *      fd: the opened device fd;
 *   d_ret: the data object for holding the async write
 *          done return value, it can be NULL, but if
 *          set the length must >= sizeof (ssize_t);
 *   d_buf: the buffer for holding the data for writing;
 * Return value:
 *      <0: failed with aosl_errno set;
 *     >=0: successfully;
 **/
extern __aosl_api__ ssize_t aosl_co_write (aosl_fd_t fd, aosl_data_t d_ret, aosl_data_t d_buf);

/**
 * The I/O device write function which supports coroutine.
 * Parameters:
 *      fd: the opened device fd;
 *   d_ret: the data object for holding the async write
 *          done return value, it can be NULL, but if
 *          set the length must >= sizeof (ssize_t);
 *     buf: the buffer for holding the data for writing;
 *   count: the writing data bytes;
 * Return value:
 *      <0: failed with aosl_errno set;
 *     >=0: successfully;
 **/
extern __aosl_api__ ssize_t aosl_co_buf_write (aosl_fd_t fd, aosl_data_t d_ret, const void *buf, size_t count);


#ifdef __cplusplus
}
#endif


#endif /* __AOSL_MPQ_FD_H__ */