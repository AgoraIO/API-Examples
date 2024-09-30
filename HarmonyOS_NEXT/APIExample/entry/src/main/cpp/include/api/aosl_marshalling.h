/*************************************************************
 * Author		:		Lionfore Hao (haolianfu@agora.io)
 * Date			:		Jul 21st, 2018
 * Module		:		Data Marshalling
 *
 *
 * This is a part of the Advanced Operating System Layer.
 * Copyright (C) 2018 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#ifndef __AOSL_MARSHALLING_H__
#define __AOSL_MARSHALLING_H__

#include <api/aosl_types.h>
#include <api/aosl_defs.h>
#include <api/aosl_psb.h>


#ifdef __cplusplus
extern "C" {
#endif


typedef enum {
	AOSL_TYPE_VOID,
	AOSL_TYPE_INT8,
	AOSL_TYPE_INT16,
	AOSL_TYPE_INT32,
	AOSL_TYPE_INT64,
#if !defined (_MSC_VER) && defined (__LP64__)
	AOSL_TYPE_INT128,
#endif
	AOSL_TYPE_FLOAT,
	AOSL_TYPE_DOUBLE,
	AOSL_TYPE_V4_IPADDR,
	AOSL_TYPE_V6_IPADDR,
	AOSL_TYPE_POINTER,
	AOSL_TYPE_REFERENCE,
	AOSL_TYPE_STRING,
	AOSL_TYPE_STRUCT,
	AOSL_TYPE_FIXED_BYTES,
	AOSL_TYPE_VAR_BYTES,
	AOSL_TYPE_BYTES_WITH_NIL,
	AOSL_TYPE_DYNAMIC_BYTES,
	AOSL_TYPE_DYNAMIC_STRING,
	AOSL_TYPE_FIXED_ARRAY,
	AOSL_TYPE_VAR_ARRAY,
	AOSL_TYPE_DYNAMIC_ARRAY,
} aosl_type_id;

typedef struct {
	/**
	 * For almost all cases, the count of an array will not
	 * exceed a 16 bit unsigned integer, so we just employ
	 * a 16 bit unsigned in the marshalling packet is OK.
	 * For those cases that the size of array exceed a 16
	 * bit integer, I think the system designer should split
	 * the packet into severals to resolve the problem.
	 * -- Lionfore Hao Jul 19th, 2018
	 **/
	uint16_t count;
	uint16_t allocated;
	void *values;
} aosl_dynamic_array_t;

extern __aosl_api__ void aosl_dynamic_array_init (aosl_dynamic_array_t *arr);
extern __aosl_api__ void aosl_dynamic_array_init_with (aosl_dynamic_array_t *arr, void *data, size_t len);
extern __aosl_api__ int aosl_dynamic_array_add_elems (aosl_dynamic_array_t *arr, const void *elems, size_t elem_size, size_t nelems);
extern __aosl_api__ void aosl_dynamic_array_take (aosl_dynamic_array_t *dst, aosl_dynamic_array_t *src);
extern __aosl_api__ int aosl_dynamic_array_is_empty (const aosl_dynamic_array_t *arr);

typedef aosl_dynamic_array_t aosl_dynamic_bytes_t;
typedef aosl_dynamic_bytes_t aosl_dynamic_string_t;

#define aosl_dynamic_bytes_init(v) aosl_dynamic_array_init (v)
#define aosl_dynamic_bytes_init_with(v, buf, len) aosl_dynamic_array_init_with (v, buf, len)
#define aosl_dynamic_string_init(s) aosl_dynamic_array_init (s)
#define aosl_dynamic_string_init_with(v, buf, len) aosl_dynamic_array_init_with (v, buf, len)

#define aosl_dynamic_array_data(arr, T) ((T *)(arr).values)
#define aosl_dynamic_bytes_data(arr) aosl_dynamic_array_data (arr, uint8_t)
#define aosl_dynamic_string_c_str(dyn_str) aosl_dynamic_array_data (dyn_str, char)

extern __aosl_api__ int aosl_dynamic_bytes_add_data (aosl_dynamic_bytes_t *dyn_bytes, const void *data, size_t len);
extern __aosl_api__ int aosl_dynamic_bytes_copy_data (aosl_dynamic_bytes_t *dyn_bytes, const void *data, size_t len);
extern __aosl_api__ int aosl_dynamic_string_strcat (aosl_dynamic_string_t *dyn_str, const char *str);
extern __aosl_api__ int aosl_dynamic_string_strcpy (aosl_dynamic_string_t *dyn_str, const char *str);
extern __aosl_api__ const char *aosl_dynamic_string_strcpy_out (char *buf, size_t buf_sz, const aosl_dynamic_string_t *dyn_str);

#define aosl_dynamic_bytes_take(dst, src) aosl_dynamic_array_take (dst, src)
#define aosl_dynamic_string_take(dst, src) aosl_dynamic_array_take (dst, src)

#define aosl_bytes_is_empty(v) aosl_dynamic_array_is_empty (v)
#define aosl_dynamic_string_is_empty(v) aosl_dynamic_array_is_empty (v)

extern __aosl_api__ int aosl_dynamic_bytes_copy (aosl_dynamic_bytes_t *dst, const aosl_dynamic_bytes_t *src);
#define aosl_dynamic_string_copy(dst, src) aosl_dynamic_bytes_copy (dst, src)

extern __aosl_api__ int aosl_dynamic_bytes_compare (const aosl_dynamic_bytes_t *b1, const aosl_dynamic_bytes_t *b2);
#define aosl_dynamic_string_compare(s1, s2) aosl_dynamic_bytes_compare (s1, s2)


typedef struct _____type_info {
	aosl_type_id type_id;
	uint32_t obj_size;
	uint32_t array_size;
	uint16_t *count_var_addr;
	void *obj_addr;
	const struct _____type_info *child;
} aosl_type_info_t;

#define AOSL_TYPE_STRUCT_END { .type_id = AOSL_TYPE_VOID }



extern __aosl_api__ void aosl_init_typed_obj (const aosl_type_info_t *type, void *typed_obj_p);
extern __aosl_api__ ssize_t aosl_marshal (const aosl_type_info_t *type, const void *typed_obj_p, aosl_psb_t *psb);
extern __aosl_api__ ssize_t aosl_unmarshal (const aosl_type_info_t *type, void *typed_obj_p, const aosl_psb_t *psb);
extern __aosl_api__ ssize_t aosl_marshal_c_str (const char *str, uint32_t nmax, aosl_psb_t *psb);
extern __aosl_api__ ssize_t aosl_unmarshal_c_str (char *str, uint32_t nmax, const aosl_psb_t *psb);
extern __aosl_api__ void aosl_fini_typed_obj (const aosl_type_info_t *type, const void *typed_obj_p);
extern __aosl_api__ ssize_t aosl_marshal_size (const aosl_type_info_t *type, const void *typed_obj_p);


extern __aosl_api__ uint16_t aosl_encode_int16 (uint16_t v);
extern __aosl_api__ uint32_t aosl_encode_int32 (uint32_t v);
extern __aosl_api__ uint64_t aosl_encode_int64 (uint64_t v);

extern __aosl_api__ uint16_t aosl_decode_int16 (uint16_t v);
extern __aosl_api__ uint32_t aosl_decode_int32 (uint32_t v);
extern __aosl_api__ uint64_t aosl_decode_int64 (uint64_t v);


extern const aosl_type_info_t aosl_void_type;
extern const aosl_type_info_t aosl_int16_type;
extern const aosl_type_info_t aosl_int32_type;
extern const aosl_type_info_t aosl_int64_type;
#if !defined (_MSC_VER) && defined (__LP64__)
extern const aosl_type_info_t aosl_int128_type;
#endif
extern const aosl_type_info_t aosl_float_type;
extern const aosl_type_info_t aosl_double_type;
extern const aosl_type_info_t aosl_v4_ipaddr_type;
extern const aosl_type_info_t aosl_v6_ipaddr_type;
extern const aosl_type_info_t aosl_dynamic_bytes_type;
extern const aosl_type_info_t aosl_dynamic_string_type;
extern const aosl_type_info_t aosl_dynamic_int16_array_type;
extern const aosl_type_info_t aosl_dynamic_int32_array_type;
extern const aosl_type_info_t aosl_dynamic_int64_array_type;
#if !defined (_MSC_VER) && defined (__LP64__)
extern const aosl_type_info_t aosl_dynamic_int128_array_type;
#endif
extern const aosl_type_info_t aosl_dynamic_float_array_type;
extern const aosl_type_info_t aosl_dynamic_double_array_type;
extern const aosl_type_info_t aosl_dynamic_v4_ipaddr_array_type;
extern const aosl_type_info_t aosl_dynamic_v6_ipaddr_array_type;

#define aosl_dynamic_bytes_fini(bytes) aosl_fini_typed_obj (&aosl_dynamic_bytes_type, bytes)
#define aosl_dynamic_string_fini(dyn_str) aosl_fini_typed_obj (&aosl_dynamic_string_type, dyn_str)



#ifdef __cplusplus
}
#endif


#endif /* __AOSL_MARSHALLING_H__ */