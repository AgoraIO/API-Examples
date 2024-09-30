/*************************************************************
 * Author:	Lionfore Hao
 * Date	 :	Nov 6th, 2018
 * Module:	AOSL mpq socket C++ 11 lambda implementation
 *
 *
 * This is a part of the Advanced Operating System Layer.
 * Copyright (C) 2018 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#ifndef __AOSL_MPQ_SK_LAMBDA_H__
#define __AOSL_MPQ_SK_LAMBDA_H__

#include <stdlib.h>

#include <api/aosl_types.h>
#include <api/aosl_data.h>
#include <api/aosl_mpq_fd.h>
#include <api/aosl_mpq_net.h>
#include <api/cpp/aosl_mpq_fd_class.h>
#include <api/cpp/aosl_ref_class.h>

#if (__cplusplus >= 201103) || defined (_MSC_VER)
#include <functional>

typedef std::function <ssize_t (const void *data, size_t len)> aosl_fd_chk_pkt_lambda_t;
typedef std::function <void (void *data, size_t len)> aosl_fd_data_lambda_t;
typedef std::function <void (void *data, size_t len, const aosl_sk_addr_t *addr)> aosl_dgram_sk_data_lambda_t;
typedef std::function <void (aosl_data_t d_data, void *data, size_t len)> aosl_fd_d_data_lambda_t; /* lambda with aosl_data_t */
typedef std::function <void (aosl_data_t d_data, void *data, size_t len, const aosl_sk_addr_t *addr)> aosl_dgram_sk_d_data_lambda_t; /* lambda with aosl_data_t */
typedef std::function <void (aosl_fd_t sk, int event)> aosl_fd_event_lambda_t;
typedef std::function <void (const aosl_accept_data_t *accept_data)> aosl_listen_sk_accept_lambda_t;


class aosl_mpq_dgram_sk: public aosl_mpq_fd_class {
public:
	aosl_mpq_dgram_sk (aosl_fd_t sk, size_t max_pkt_size,
						aosl_dgram_sk_data_lambda_t&& data_cb,
							aosl_fd_event_lambda_t&& event_cb,
							aosl_mpq_t qid = aosl_mpq_this ())
						: aosl_mpq_fd_class (sk),
						  on_data (std::move (data_cb)),
						  on_event (std::move (event_cb))
	{
		if (aosl_mpq_add_dgram_socket (qid, sk, max_pkt_size, &__on_data, &__on_event, 1, ref ()) < 0)
			abort ();
	}

	aosl_mpq_dgram_sk (aosl_fd_t sk, size_t max_pkt_size,
						aosl_dgram_sk_d_data_lambda_t&& data_cb,
							aosl_fd_event_lambda_t&& event_cb,
							aosl_mpq_t qid = aosl_mpq_this ())
						: aosl_mpq_fd_class (sk),
						  on_d_data (std::move (data_cb)),
						  on_event (std::move (event_cb))
	{
		if (aosl_mpq_add_dgram_socket (qid, sk, max_pkt_size, &__on_d_data, &__on_event, 1, ref ()) < 0)
			abort ();
	}

	virtual ~aosl_mpq_dgram_sk ()
	{
		/**
		 * Please make sure this is the first action in the destructor of
		 * each final derived class of this class and its' derivatives.
		 **/
		ref_destroy ();
	}

private:
	static void __on_data (void *data, size_t len, uintptr_t argc, uintptr_t argv [], const aosl_sk_addr_t *addr)
	{
		aosl_ref_class::hold ((aosl_ref_t)argv [0], [&] (void *arg) {
			aosl_mpq_dgram_sk *__this = (aosl_mpq_dgram_sk *)(aosl_mpq_fd_class *)arg;
			__this->on_data (data, len, addr);
		});
	}

	static void __on_d_data (void *data, size_t len, uintptr_t argc, uintptr_t argv [], const aosl_sk_addr_t *addr)
	{
		aosl_ref_class::hold ((aosl_ref_t)argv [0], [&] (void *arg) {
			aosl_mpq_dgram_sk *__this = (aosl_mpq_dgram_sk *)(aosl_mpq_fd_class *)arg;
			__this->on_d_data ((aosl_data_t)argv [argc], data, len, addr);
		});
	}

	static void __on_event (aosl_fd_t sk, int event, uintptr_t argc, uintptr_t argv [])
	{
		aosl_ref_class::hold ((aosl_ref_t)argv [0], [&] (void *arg) {
			aosl_mpq_dgram_sk *__this = (aosl_mpq_dgram_sk *)(aosl_mpq_fd_class *)arg;
			__this->on_event (sk, event);
		});
	}

	const aosl_dgram_sk_data_lambda_t on_data;
	const aosl_dgram_sk_d_data_lambda_t on_d_data;
	const aosl_fd_event_lambda_t on_event;

private:
	aosl_mpq_dgram_sk () = delete;
	aosl_mpq_dgram_sk (const aosl_mpq_dgram_sk &) = delete;
	aosl_mpq_dgram_sk (aosl_mpq_dgram_sk &&) = delete;
	aosl_mpq_dgram_sk &operator = (const aosl_mpq_dgram_sk &) = delete;
	aosl_mpq_dgram_sk &operator = (aosl_mpq_dgram_sk &&) = delete;
};

class aosl_mpq_stream_sk: public aosl_mpq_fd_class {
public:
	aosl_mpq_stream_sk (aosl_fd_t sk, int enable, size_t max_pkt_size,
								aosl_fd_chk_pkt_lambda_t&& chk_pkt_cb,
									aosl_fd_data_lambda_t&& data_cb,
									aosl_fd_event_lambda_t&& event_cb,
									aosl_mpq_t qid = aosl_mpq_this ())
					: aosl_mpq_fd_class (sk),
					  on_chk_pkt (std::move (chk_pkt_cb)),
					  on_data (std::move (data_cb)),
					  on_event (std::move (event_cb))
	{
		if (aosl_mpq_add_stream_socket (qid, sk, enable, max_pkt_size, &__on_chk_pkt, &__on_data, &__on_event, 1, ref ()) < 0)
			abort ();
	}

	aosl_mpq_stream_sk (aosl_fd_t sk, int enable, size_t max_pkt_size,
								aosl_fd_chk_pkt_lambda_t&& chk_pkt_cb,
									aosl_fd_d_data_lambda_t&& data_cb,
									aosl_fd_event_lambda_t&& event_cb,
									aosl_mpq_t qid = aosl_mpq_this ())
					: aosl_mpq_fd_class (sk),
					  on_chk_pkt (std::move (chk_pkt_cb)),
					  on_d_data (std::move (data_cb)),
					  on_event (std::move (event_cb))
	{
		if (aosl_mpq_add_stream_socket (qid, sk, enable, max_pkt_size, &__on_chk_pkt, &__on_d_data, &__on_event, 1, ref ()) < 0)
			abort ();
	}

	virtual ~aosl_mpq_stream_sk ()
	{
		/**
		 * Please make sure this is the first action in the destructor of
		 * each final derived class of this class and its' derivatives.
		 **/
		ref_destroy ();
	}

private:
	static ssize_t __on_chk_pkt (const void *data, size_t len, uintptr_t argc, uintptr_t argv [])
	{
		ssize_t ret = -1;

		aosl_ref_class::hold ((aosl_ref_t)argv [0], [&] (void *arg) {
			aosl_mpq_stream_sk *__this = (aosl_mpq_stream_sk *)(aosl_mpq_fd_class *)arg;
			ret = __this->on_chk_pkt (data, len);
		});

		return ret;
	}

	static void __on_data (void *data, size_t len, uintptr_t argc, uintptr_t argv [])
	{
		aosl_ref_class::hold ((aosl_ref_t)argv [0], [&] (void *arg) {
			aosl_mpq_stream_sk *__this = (aosl_mpq_stream_sk *)(aosl_mpq_fd_class *)arg;
			__this->on_data (data, len);
		});
	}

	static void __on_d_data (void *data, size_t len, uintptr_t argc, uintptr_t argv [])
	{
		aosl_ref_class::hold ((aosl_ref_t)argv [0], [&] (void *arg) {
			aosl_mpq_stream_sk *__this = (aosl_mpq_stream_sk *)(aosl_mpq_fd_class *)arg;
			__this->on_d_data ((aosl_data_t)argv [argc], data, len);
		});
	}

	static void __on_event (aosl_fd_t sk, int event, uintptr_t argc, uintptr_t argv [])
	{
		aosl_ref_class::hold ((aosl_ref_t)argv [0], [&] (void *arg) {
			aosl_mpq_stream_sk *__this = (aosl_mpq_stream_sk *)(aosl_mpq_fd_class *)arg;
			__this->on_event (sk, event);
		});
	}

	const aosl_fd_chk_pkt_lambda_t on_chk_pkt;
	const aosl_fd_data_lambda_t on_data;
	const aosl_fd_d_data_lambda_t on_d_data;
	const aosl_fd_event_lambda_t on_event;

private:
	aosl_mpq_stream_sk () = delete;
	aosl_mpq_stream_sk (const aosl_mpq_stream_sk &) = delete;
	aosl_mpq_stream_sk (aosl_mpq_stream_sk &&) = delete;
	aosl_mpq_stream_sk &operator = (const aosl_mpq_stream_sk &) = delete;
	aosl_mpq_stream_sk &operator = (aosl_mpq_stream_sk &&) = delete;
};

class aosl_mpq_connect_stream_sk: public aosl_mpq_stream_sk {
public:
	aosl_mpq_connect_stream_sk (aosl_fd_t sk, size_t max_pkt_size,
							aosl_fd_chk_pkt_lambda_t&& chk_pkt_cb,
								aosl_fd_data_lambda_t&& data_cb,
								aosl_fd_event_lambda_t&& event_cb,
								aosl_mpq_t qid = aosl_mpq_this ())
								: aosl_mpq_stream_sk (sk, 0/* !enable */,
								  max_pkt_size,
								  std::move (chk_pkt_cb),
								  std::move (data_cb),
								  std::move (event_cb), qid) {}

	aosl_mpq_connect_stream_sk (aosl_fd_t sk, size_t max_pkt_size,
							aosl_fd_chk_pkt_lambda_t&& chk_pkt_cb,
								aosl_fd_d_data_lambda_t&& data_cb,
								aosl_fd_event_lambda_t&& event_cb,
								aosl_mpq_t qid = aosl_mpq_this ())
								: aosl_mpq_stream_sk (sk, 0/* !enable */,
								  max_pkt_size,
								  std::move (chk_pkt_cb),
								  std::move (data_cb),
								  std::move (event_cb), qid) {}

	int connect (const struct sockaddr *dest_addr, socklen_t addrlen, int timeo)
	{
		return aosl_mpq_connect (get_fd (), dest_addr, addrlen, timeo);
	}

private:
	aosl_mpq_connect_stream_sk () = delete;
	aosl_mpq_connect_stream_sk (const aosl_mpq_connect_stream_sk &) = delete;
	aosl_mpq_connect_stream_sk (aosl_mpq_connect_stream_sk &&) = delete;
	aosl_mpq_connect_stream_sk &operator = (const aosl_mpq_connect_stream_sk &) = delete;
	aosl_mpq_connect_stream_sk &operator = (aosl_mpq_connect_stream_sk &&) = delete;
};

class aosl_mpq_accepted_stream_sk: public aosl_mpq_stream_sk {
public:
	aosl_mpq_accepted_stream_sk (aosl_fd_t sk, size_t max_pkt_size,
							aosl_fd_chk_pkt_lambda_t&& chk_pkt_cb,
								aosl_fd_data_lambda_t&& data_cb,
								aosl_fd_event_lambda_t&& event_cb,
								aosl_mpq_t qid = aosl_mpq_this ())
								: aosl_mpq_stream_sk (sk, 1/*enable*/,
								  max_pkt_size,
								  std::move (chk_pkt_cb),
								  std::move (data_cb),
								  std::move (event_cb), qid) {}

	aosl_mpq_accepted_stream_sk (aosl_fd_t sk, size_t max_pkt_size,
							aosl_fd_chk_pkt_lambda_t&& chk_pkt_cb,
								aosl_fd_d_data_lambda_t&& data_cb,
								aosl_fd_event_lambda_t&& event_cb,
								aosl_mpq_t qid = aosl_mpq_this ())
								: aosl_mpq_stream_sk (sk, 1/*enable*/,
								  max_pkt_size,
								  std::move (chk_pkt_cb),
								  std::move (data_cb),
								  std::move (event_cb), qid) {}

private:
	aosl_mpq_accepted_stream_sk () = delete;
	aosl_mpq_accepted_stream_sk (const aosl_mpq_accepted_stream_sk &) = delete;
	aosl_mpq_accepted_stream_sk (aosl_mpq_accepted_stream_sk &&) = delete;
	aosl_mpq_accepted_stream_sk &operator = (const aosl_mpq_accepted_stream_sk &) = delete;
	aosl_mpq_accepted_stream_sk &operator = (aosl_mpq_accepted_stream_sk &&) = delete;
};

class aosl_mpq_listen_sk: public aosl_mpq_fd_class {
public:
	aosl_mpq_listen_sk (aosl_fd_t sk, int backlog,
		aosl_listen_sk_accept_lambda_t&& accept_cb,
				aosl_fd_event_lambda_t&& event_cb,
				aosl_mpq_t qid = aosl_mpq_this ())
					: aosl_mpq_fd_class (sk),
					  on_accept (std::move (accept_cb)),
					  on_event (std::move (event_cb))
	{
		if (aosl_mpq_add_listen_socket (qid, sk, backlog, __on_accept, __on_event, 1, ref ()) < 0)
			abort ();
	}

	virtual ~aosl_mpq_listen_sk ()
	{
		/**
		 * Please make sure this is the first action in the destructor of
		 * each final derived class of this class and its' derivatives.
		 **/
		ref_destroy ();
	}

private:
	static void __on_accept (aosl_accept_data_t *accept_data, size_t len, uintptr_t argc, uintptr_t argv [])
	{
		aosl_ref_class::hold ((aosl_ref_t)argv [0], [&] (void *arg) {
			aosl_mpq_listen_sk *__this = (aosl_mpq_listen_sk *)(aosl_mpq_fd_class *)arg;
			__this->on_accept (accept_data);
		});
	}

	static void __on_event (aosl_fd_t sk, int event, uintptr_t argc, uintptr_t argv [])
	{
		aosl_ref_class::hold ((aosl_ref_t)argv [0], [&] (void *arg) {
			aosl_mpq_listen_sk *__this = (aosl_mpq_listen_sk *)(aosl_mpq_fd_class *)arg;
			__this->on_event (sk, event);
		});
	}

	const aosl_listen_sk_accept_lambda_t on_accept;
	const aosl_fd_event_lambda_t on_event;

private:
	aosl_mpq_listen_sk () = delete;
	aosl_mpq_listen_sk (const aosl_mpq_listen_sk &) = delete;
	aosl_mpq_listen_sk (aosl_mpq_listen_sk &&) = delete;
	aosl_mpq_listen_sk &operator = (const aosl_mpq_listen_sk &) = delete;
	aosl_mpq_listen_sk &operator = (aosl_mpq_listen_sk &&) = delete;
};
#endif /* C++11 */

class aosl_co_sk: public aosl_mpq_fd_class {
public:
	aosl_co_sk (int domain, int type, int protocol): aosl_mpq_fd_class (aosl_socket (domain, type, protocol))
	{
	}

	aosl_co_sk (aosl_fd_t sk): aosl_mpq_fd_class (sk)
	{
	}

public:
	int listen (int backlog)
	{
		return aosl_listen (get_fd (), backlog);
	}

	int co_accept (aosl_data_t d_ret, aosl_data_t d_addr)
	{
		return aosl_co_accept (get_fd (), d_ret, d_addr);
	}

	int co_recv (aosl_data_t d_ret, aosl_data_t d_buf, uintptr_t buf_off = 0, int flags = 0)
	{
		return aosl_co_recv (get_fd (), d_ret, d_buf, buf_off, flags);
	}

	int co_recvfrom (aosl_data_t d_ret, aosl_data_t d_buf, aosl_data_t d_addr, uintptr_t buf_off = 0, int flags = 0)
	{
		return aosl_co_recvfrom (get_fd (), d_ret, d_buf, buf_off, flags, d_addr);
	}

	int co_connect (aosl_data_t d_ret, const struct sockaddr *dest_addr, socklen_t addrlen, int timeo)
	{
		return aosl_co_connect (get_fd (), d_ret, dest_addr, addrlen, timeo);
	}

	ssize_t co_send (aosl_data_t d_ret, aosl_data_t d_buf, int flags)
	{
		return aosl_co_send (get_fd (), d_ret, d_buf, flags);
	}

	ssize_t co_sendto (aosl_data_t d_ret, aosl_data_t d_buf, int flags,
					const struct sockaddr *dest_addr, socklen_t addrlen)
	{
		return aosl_co_sendto (get_fd (), d_ret, d_buf, flags, dest_addr, addrlen);
	}

	ssize_t co_buf_send (aosl_data_t d_ret, const void *buf, size_t len, int flags)
	{
		return aosl_co_buf_send (get_fd (), d_ret, buf, len, flags);
	}

	ssize_t co_buf_sendto (aosl_data_t d_ret, const void *buf, size_t len, int flags,
							const struct sockaddr *dest_addr, socklen_t addrlen)
	{
		return aosl_co_buf_sendto (get_fd (), d_ret, buf, len, flags, dest_addr, addrlen);
	}

#if (__cplusplus >= 201103) || defined (_MSC_VER)
private:
	aosl_co_sk () = delete;
	aosl_co_sk (const aosl_co_sk &) = delete;
	aosl_co_sk (aosl_co_sk &&) = delete;
	aosl_co_sk &operator = (const aosl_co_sk &) = delete;
	aosl_co_sk &operator = (aosl_co_sk &&) = delete;
#else
private:
	aosl_co_sk ();
	aosl_co_sk (const aosl_co_sk &);
	aosl_co_sk &operator = (const aosl_co_sk &);
#endif /* C++11 */
};


#endif /* __AOSL_MPQ_SK_LAMBDA_H__ */