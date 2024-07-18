/*************************************************************
 * Author:	Lionfore Hao (haolianfu@agora.io)
 * Date	 :	Dec 23rd, 2018
 * Module:	AOSL HTTP API definition file
 *
 *
 * This is a part of the Advanced Operating System Layer.
 * Copyright (C) 2018 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#ifndef __AOSL_HTTP_API_H__
#define __AOSL_HTTP_API_H__


#include <api/aosl_types.h>
#include <api/aosl_defs.h>


#ifdef __cplusplus
extern "C" {
#endif


#if 0 /* Will be implemented soon */
typedef struct http_server *aosl_http_server_t;
typedef struct http_client *aosl_http_client_t;

typedef enum {
	aosl_http_req_type_get,
	aosl_http_req_type_options,
	aosl_http_req_type_head,
	aosl_http_req_type_post,
	aosl_http_req_type_put,
	aosl_http_req_type_delete,
	aosl_http_req_type_trace,
	aosl_http_req_type_connect,
	aosl_http_req_type_propfind,
	aosl_http_req_type_proppatch,
	aosl_http_req_type_mkcol,
	aosl_http_req_type_copy,
	aosl_http_req_type_move,
	aosl_http_req_type_lock,
	aosl_http_req_type_unlock
} aosl_http_req_type_t;


/**
 * The http object creating function prototype, which is used to create an http object.
 * Parameters:
 *            arg: the parameter attached with the http object;
 *           dtor: the http object destructor function, which will be invoked when
 *                 the http object is deleted;
 **/
extern __aosl_api__ aosl_http_conn_t aosl_http_conn_create (const char *host, uint16_t port);

/**
 * The http object creating function prototype, which is used to create an http object.
 * Parameters:
 *            arg: the parameter attached with the http object;
 *           dtor: the http object destructor function, which will be invoked when
 *                 the http object is deleted;
 **/
extern __aosl_api__ aosl_http_t aosl_http_create ();

/**
 * Add header to http object function prototype, which is used to add a header to an http object.
 * Parameters:
 *           http: the http object;
 *            key: the header key;
 *            val: the header value;
 * Return value:
 *     0: success
 *    <0: failed, and errno indicates what error occurs
 **/
extern __aosl_api__ int aosl_http_add_header (aosl_http_t http, const char *key, const char *val);

/**
 * Del header from http object function prototype, which is used to add a header to an http object.
 * Parameters:
 *           http: the http object;
 *            key: the header key;
 * Return value:
 *     0: success
 *    <0: failed, and errno indicates what error occurs
 **/
extern __aosl_api__ int aosl_http_del_header (aosl_http_t http, const char *key);

/**
 * Destroy the http object specified by http.
 * Parameter:
 *      http: the http object
 * Return value:
 *     0: success
 *    <0: failed, and errno indicates what error occurs
 **/
extern __aosl_api__ int aosl_http_destroy (aosl_http_t http);
#endif

/**
 * Encode a URI string, escape all characters which are not
 * allowed to appear in a URI string.
 * Parameters:
 *      buf: the destination buffer pointer to hold the escaped URI
 *      src: the source URI string
 *  buf_len: the destination buffer size
 * Return value:
 *     <0: the specified destination buffer too small
 *    >=0: the escaped URI string length, not including the termination '\0'
 **/
extern __aosl_api__ int aosl_uri_encode (char *buf, const char *src, size_t buf_len);

typedef void *aosl_http_parser_t;

#define AOSL_HTTP_PARSER_INVALID ((aosl_http_parser_t)NULL)

#define aosl_http_parser_invalid(p) ((aosl_http_parser_t)(p) == NULL)

typedef enum {
	aosl_http_ev_tcp_connected = 0x1000,
	aosl_http_ev_message_begin,
	aosl_http_ev_request_url,
	aosl_http_ev_response_status,
	aosl_http_ev_header_field,
	aosl_http_ev_header_value,
	aosl_http_ev_headers_complete,
	aosl_http_ev_body,
	aosl_http_ev_message_complete,
	aosl_http_ev_chunk_header,
	aosl_http_ev_chunk_complete,
} aosl_http_event_t;

/**
 * HTTP parser event callback function.
 * Parameters:
 *     parser: the parser object
 *      event: indicate what happened, values are as following:
 *            <0: negative value for OS level error, equal to -errno
 *             0: OK, no error
 *    >0&<0x1000: HTTP parser errno
 *      >=0x1000: events defined in aosl_http_event_t
 *        ...:
 *        aosl_http_ev_message_begin:
 *                 N/A
 *        aosl_http_ev_request_url:
 *                 1. va_arg (args, const char *): url, terminated with '\0'
 *        aosl_http_ev_response_status:
 *                 1. va_arg (args, const char *): status string, terminated with '\0'
 *        aosl_http_ev_header_field:
 *                 1. va_arg (args, const char *): header field name, terminated with '\0'
 *        aosl_http_ev_header_field:
 *                 1. va_arg (args, const char *): header field value, terminated with '\0'
 *        aosl_http_ev_headers_complete:
 *                 N/A
 *        aosl_http_ev_body:
 *                 1. va_arg (args, const char *): body content(NOTE: it is not terminated with '\0')
 *                 2. va_arg (args, size_t): body content length
 *        aosl_http_ev_message_complete:
 *                 N/A
 *        aosl_http_ev_chunk_header:
 *                 N/A
 *        aosl_http_ev_chunk_complete:
 *                 N/A
 * Return value:
 *     none.
 **/
typedef void (*aosl_http_event_cb_t) (aosl_http_parser_t parser, int event, ...);

/**
 * Create an HTTP parser.
 * Parameters:
 *    event_f: HTTP parser event callback function
 *        arg: application passed arg which will be passed in event_f
 * Return value:
 *   NULL: failed to create the HTTP parser.
 *  Other: the created HTTP parser object.
 **/
extern __aosl_api__ aosl_http_parser_t aosl_http_parser_create (aosl_http_event_cb_t event_f, void *arg);

/**
 * Retrieve the argument passed in when creating this parser.
 * Parameters:
 *   parser_obj: the parser object
 * Return value:
 *     the arg.
 **/
extern __aosl_api__ void *aosl_http_parser_arg (aosl_http_parser_t parser_obj);

/**
 * Reset the HTTP parser object.
 * Parameters:
 *   parser_obj: the parser object
 * Return value:
 *     none.
 **/
extern __aosl_api__ void aosl_http_parser_reset (aosl_http_parser_t parser_obj);

/**
 * HTTP parser execute the parsing.
 * Parameters:
 *   parser_obj: the parser object
 *         data: the buffer contains the HTTP data
 *          len: the buffer length
 * Return value:
 *     Returns number of parsed bytes, negative value for error.
 **/
extern __aosl_api__ ssize_t aosl_http_parser_execute (aosl_http_parser_t parser_obj, const char *data, size_t len);

/**
 * Get the HTTP major from parser.
 * Parameters:
 *   parser_obj: the parser object
 * Return value:
 *        the HTTP major number, negative value for error.
 **/
extern __aosl_api__ int aosl_http_parser_major (aosl_http_parser_t parser_obj);

/**
 * Get the HTTP minor from parser.
 * Parameters:
 *   parser_obj: the parser object
 * Return value:
 *        the HTTP minor number, negative value for error.
 **/
extern __aosl_api__ int aosl_http_parser_minor (aosl_http_parser_t parser_obj);

/**
 * Get the HTTP status code from parser.
 * Parameters:
 *   parser_obj: the parser object
 * Return value:
 *        the HTTP status code, negative value for error.
 **/
extern __aosl_api__ int aosl_http_parser_status (aosl_http_parser_t parser_obj);

/**
 * Get the HTTP status code string from parser.
 * Parameters:
 *   parser_obj: the parser object
 * Return value:
 *        the HTTP status code string.
 **/
extern __aosl_api__ const char *aosl_http_parser_status_str (int status);

/**
 * Get the HTTP errno from parser.
 * Parameters:
 *   parser_obj: the parser object
 * Return value:
 *        the HTTP errno.
 **/
extern __aosl_api__ int aosl_http_parser_errno (aosl_http_parser_t parser_obj);

/**
 * Get the HTTP errno name string from parser.
 * Parameters:
 *   parser_obj: the parser object
 * Return value:
 *        the HTTP errno name string.
 **/
extern __aosl_api__ const char *aosl_http_parser_errno_name (int http_err);

/**
 * Get the HTTP errno description string from parser.
 * Parameters:
 *   parser_obj: the parser object
 * Return value:
 *        the HTTP errno description string.
 **/
extern __aosl_api__ const char *aosl_http_parser_errno_descr (int http_err);

/**
 * Destroy the specified parser object.
 * Parameters:
 *   parser_obj: the parser object
 * Return value:
 *        none.
 **/
extern __aosl_api__ void aosl_http_parser_destroy (aosl_http_parser_t parser);


#ifdef __cplusplus
}
#endif



#endif /* __AOSL_HTTP_API_H__ */