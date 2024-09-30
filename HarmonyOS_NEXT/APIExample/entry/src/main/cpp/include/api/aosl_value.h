/*************************************************************
 * Author:	Lionfore Hao (haolianfu@agora.io)
 * Date	 :	Nov 26th, 2022
 * Module:	AOSL value object API definition file
 *
 *
 * This is a part of the Advanced Operating System Layer.
 * Copyright (C) 2018 ~ 2022 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#ifndef __AOSL_VALUE_H__
#define __AOSL_VALUE_H__


#include <api/aosl_types.h>
#include <api/aosl_defs.h>
#include <api/aosl_kobj.h>


#ifdef __cplusplus
extern "C" {
#endif



typedef enum {
	aosl_value_type_none,
	aosl_value_type_int8,
	aosl_value_type_int16,
	aosl_value_type_int32,
	aosl_value_type_int64,
	aosl_value_type_intptr,
	aosl_value_type_pointer,
	aosl_value_type_float,
	aosl_value_type_double,
	aosl_value_type_string,
	aosl_value_type_data,
} aosl_value_type_t;

typedef struct {
	aosl_value_type_t type;
	uint32_t weight; /* the current weight of the value */
	union {
		int8_t i1_val;
		int16_t i2_val;
		int32_t i4_val;
		int64_t i8_val;
		intptr_t ip_val;
		double f_val;
		void *p_val;
		struct {
			uint32_t m_alloc;
			uint32_t d_len;
			void *p_data;
		};
	};
} aosl_value_t;

typedef struct {
	aosl_value_t value;
	intptr_t copied; /* <0: error; 0: no change; >0: value changed */
} aosl_kobj_value_notify_t;

extern __aosl_api__ void aosl_value_init (aosl_value_t *val);
extern __aosl_api__ int aosl_value_compare (const aosl_value_t *v1, const aosl_value_t *v2);
extern __aosl_api__ ssize_t aosl_value_copy (aosl_value_t *dst, const aosl_value_t *src);
extern __aosl_api__ ssize_t aosl_value_copy_to (void *buf, size_t len, const aosl_value_t *val);
extern __aosl_api__ ssize_t aosl_value_copy_from (aosl_value_t *val, const void *buf, size_t len);
extern __aosl_api__ void aosl_value_free (aosl_value_t *val);

/**
 * Set the value to the value object specified by dfd + name.
 * Parameters:
 *               dfd: the opened fd of directory, AOSL_KFD_INVALID for root or pwd:
 *                    1. if the name starts with a '/', then directory is root;
 *                    2. if the name does not start with a '/', then directory is pwd;
 *              name: the directory name;
 *               val: the value to be set to the value object;
 * Return value:
 *           >0: set the value successfully;
 *           =0: the value not changed due to identical;
 *           <0: failed with aosl_errno set accordingly;
 **/
extern __aosl_api__ ssize_t aosl_value_set (aosl_kfd_t dfd, const char *name, const aosl_value_t *val);

/**
 * Set int8 value to the value object specified by dfd + name.
 * Parameters:
 *               dfd: the opened fd of directory, AOSL_KFD_INVALID for root or pwd:
 *                    1. if the name starts with a '/', then directory is root;
 *                    2. if the name does not start with a '/', then directory is pwd;
 *              name: the directory name;
 *            weight: the setting weight, it must greater than or equal to the weight
 *                    of the value object, otherwise the setting operation would fail;
 *                 v: the value;
 * Return value:
 *           >0: set the value successfully;
 *           =0: the value not changed due to identical;
 *           <0: failed with aosl_errno set accordingly;
 **/
extern __aosl_api__ ssize_t aosl_value_set_int8 (aosl_kfd_t dfd, const char *name, uint32_t weight, int8_t v);

/**
 * Set int16 value to the value object specified by dfd + name.
 * Parameters:
 *               dfd: the opened fd of directory, AOSL_KFD_INVALID for root or pwd:
 *                    1. if the name starts with a '/', then directory is root;
 *                    2. if the name does not start with a '/', then directory is pwd;
 *              name: the directory name;
 *            weight: the setting weight, it must greater than or equal to the weight
 *                    of the value object, otherwise the setting operation would fail;
 *                 v: the value;
 * Return value:
 *           >0: set the value successfully;
 *           =0: the value not changed due to identical;
 *           <0: failed with aosl_errno set accordingly;
 **/
extern __aosl_api__ ssize_t aosl_value_set_int16 (aosl_kfd_t dfd, const char *name, uint32_t weight, int16_t v);

/**
 * Set int32 value to the value object specified by dfd + name.
 * Parameters:
 *               dfd: the opened fd of directory, AOSL_KFD_INVALID for root or pwd:
 *                    1. if the name starts with a '/', then directory is root;
 *                    2. if the name does not start with a '/', then directory is pwd;
 *              name: the directory name;
 *            weight: the setting weight, it must greater than or equal to the weight
 *                    of the value object, otherwise the setting operation would fail;
 *                 v: the value;
 * Return value:
 *           >0: set the value successfully;
 *           =0: the value not changed due to identical;
 *           <0: failed with aosl_errno set accordingly;
 **/
extern __aosl_api__ ssize_t aosl_value_set_int32 (aosl_kfd_t dfd, const char *name, uint32_t weight, int32_t v);

/**
 * Set int64 value to the value object specified by dfd + name.
 * Parameters:
 *               dfd: the opened fd of directory, AOSL_KFD_INVALID for root or pwd:
 *                    1. if the name starts with a '/', then directory is root;
 *                    2. if the name does not start with a '/', then directory is pwd;
 *              name: the directory name;
 *            weight: the setting weight, it must greater than or equal to the weight
 *                    of the value object, otherwise the setting operation would fail;
 *                 v: the value;
 * Return value:
 *           >0: set the value successfully;
 *           =0: the value not changed due to identical;
 *           <0: failed with aosl_errno set accordingly;
 **/
extern __aosl_api__ ssize_t aosl_value_set_int64 (aosl_kfd_t dfd, const char *name, uint32_t weight, int64_t v);

/**
 * Set intptr value to the value object specified by dfd + name.
 * Parameters:
 *               dfd: the opened fd of directory, AOSL_KFD_INVALID for root or pwd:
 *                    1. if the name starts with a '/', then directory is root;
 *                    2. if the name does not start with a '/', then directory is pwd;
 *              name: the directory name;
 *            weight: the setting weight, it must greater than or equal to the weight
 *                    of the value object, otherwise the setting operation would fail;
 *                 v: the value;
 * Return value:
 *           >0: set the value successfully;
 *           =0: the value not changed due to identical;
 *           <0: failed with aosl_errno set accordingly;
 **/
extern __aosl_api__ ssize_t aosl_value_set_intptr (aosl_kfd_t dfd, const char *name, uint32_t weight, intptr_t v);

/**
 * Set pointer value to the value object specified by dfd + name.
 * Parameters:
 *               dfd: the opened fd of directory, AOSL_KFD_INVALID for root or pwd:
 *                    1. if the name starts with a '/', then directory is root;
 *                    2. if the name does not start with a '/', then directory is pwd;
 *              name: the directory name;
 *            weight: the setting weight, it must greater than or equal to the weight
 *                    of the value object, otherwise the setting operation would fail;
 *                 v: the value;
 * Return value:
 *           >0: set the value successfully;
 *           =0: the value not changed due to identical;
 *           <0: failed with aosl_errno set accordingly;
 **/
extern __aosl_api__ ssize_t aosl_value_set_pointer (aosl_kfd_t dfd, const char *name, uint32_t weight, void *v);

/**
 * Set float value to the value object specified by dfd + name.
 * Parameters:
 *               dfd: the opened fd of directory, AOSL_KFD_INVALID for root or pwd:
 *                    1. if the name starts with a '/', then directory is root;
 *                    2. if the name does not start with a '/', then directory is pwd;
 *              name: the directory name;
 *            weight: the setting weight, it must greater than or equal to the weight
 *                    of the value object, otherwise the setting operation would fail;
 *                 v: the value;
 * Return value:
 *           >0: set the value successfully;
 *           =0: the value not changed due to identical;
 *           <0: failed with aosl_errno set accordingly;
 **/
extern __aosl_api__ ssize_t aosl_value_set_float (aosl_kfd_t dfd, const char *name, uint32_t weight, float v);

/**
 * Set double value to the value object specified by dfd + name.
 * Parameters:
 *               dfd: the opened fd of directory, AOSL_KFD_INVALID for root or pwd:
 *                    1. if the name starts with a '/', then directory is root;
 *                    2. if the name does not start with a '/', then directory is pwd;
 *              name: the directory name;
 *            weight: the setting weight, it must greater than or equal to the weight
 *                    of the value object, otherwise the setting operation would fail;
 *                 v: the value;
 * Return value:
 *           >0: set the value successfully;
 *           =0: the value not changed due to identical;
 *           <0: failed with aosl_errno set accordingly;
 **/
extern __aosl_api__ ssize_t aosl_value_set_double (aosl_kfd_t dfd, const char *name, uint32_t weight, double v);

/**
 * Set string value to the value object specified by dfd + name.
 * Parameters:
 *               dfd: the opened fd of directory, AOSL_KFD_INVALID for root or pwd:
 *                    1. if the name starts with a '/', then directory is root;
 *                    2. if the name does not start with a '/', then directory is pwd;
 *              name: the directory name;
 *            weight: the setting weight, it must greater than or equal to the weight
 *                    of the value object, otherwise the setting operation would fail;
 *               str: the string value, must be '\0' terminated;
 * Return value:
 *           >0: set the value successfully;
 *           =0: the value not changed due to identical;
 *           <0: failed with aosl_errno set accordingly;
 **/
extern __aosl_api__ ssize_t aosl_value_set_string (aosl_kfd_t dfd, const char *name, uint32_t weight, const char *str);

/**
 * Set data buffer value to the value object specified by dfd + name.
 * Parameters:
 *               dfd: the opened fd of directory, AOSL_KFD_INVALID for root or pwd:
 *                    1. if the name starts with a '/', then directory is root;
 *                    2. if the name does not start with a '/', then directory is pwd;
 *              name: the directory name;
 *            weight: the setting weight, it must greater than or equal to the weight
 *                    of the value object, otherwise the setting operation would fail;
 *               buf: the value memory;
 *               len: the memory size in bytes;
 * Return value:
 *           >0: set the value successfully;
 *           =0: the value not changed due to identical;
 *           <0: failed with aosl_errno set accordingly;
 **/
extern __aosl_api__ ssize_t aosl_value_set_data (aosl_kfd_t dfd, const char *name, uint32_t weight, const void *buf, size_t len);

/**
 * Get the value from the value object specified by dfd + name.
 * Parameters:
 *               dfd: the opened fd of directory, AOSL_KFD_INVALID for root or pwd:
 *                    1. if the name starts with a '/', then directory is root;
 *                    2. if the name does not start with a '/', then directory is pwd;
 *              name: the directory name;
 *                val: the memory address for holding the value;
 * Return value:
 *           >0: set the value successfully;
 *           =0: the value not changed due to identical;
 *           <0: failed with aosl_errno set accordingly;
 **/
extern __aosl_api__ ssize_t aosl_value_get (aosl_kfd_t dfd, const char *name, aosl_value_t *val);

/**
 * Get int8 value from the value object specified by dfd + name.
 * Parameters:
 *               dfd: the opened fd of directory, AOSL_KFD_INVALID for root or pwd:
 *                    1. if the name starts with a '/', then directory is root;
 *                    2. if the name does not start with a '/', then directory is pwd;
 *              name: the directory name;
 *                 v: the memory address for holding the value;
 * Return value:
 *           >0: set the value successfully;
 *           =0: the value not changed due to identical;
 *           <0: failed with aosl_errno set accordingly;
 **/
extern __aosl_api__ ssize_t aosl_value_get_int8 (aosl_kfd_t dfd, const char *name, int8_t *v);

/**
 * Get int16 value from the value object specified by dfd + name.
 * Parameters:
 *               dfd: the opened fd of directory, AOSL_KFD_INVALID for root or pwd:
 *                    1. if the name starts with a '/', then directory is root;
 *                    2. if the name does not start with a '/', then directory is pwd;
 *              name: the directory name;
 *                 v: the memory address for holding the value;
 * Return value:
 *           >0: set the value successfully;
 *           =0: the value not changed due to identical;
 *           <0: failed with aosl_errno set accordingly;
 **/
extern __aosl_api__ ssize_t aosl_value_get_int16 (aosl_kfd_t dfd, const char *name, int16_t *v);

/**
 * Get int32 value from the value object specified by dfd + name.
 * Parameters:
 *               dfd: the opened fd of directory, AOSL_KFD_INVALID for root or pwd:
 *                    1. if the name starts with a '/', then directory is root;
 *                    2. if the name does not start with a '/', then directory is pwd;
 *              name: the directory name;
 *                 v: the memory address for holding the value;
 * Return value:
 *           >0: set the value successfully;
 *           =0: the value not changed due to identical;
 *           <0: failed with aosl_errno set accordingly;
 **/
extern __aosl_api__ ssize_t aosl_value_get_int32 (aosl_kfd_t dfd, const char *name, int32_t *v);

/**
 * Get int64 value from the value object specified by dfd + name.
 * Parameters:
 *               dfd: the opened fd of directory, AOSL_KFD_INVALID for root or pwd:
 *                    1. if the name starts with a '/', then directory is root;
 *                    2. if the name does not start with a '/', then directory is pwd;
 *              name: the directory name;
 *                 v: the memory address for holding the value;
 * Return value:
 *           >0: set the value successfully;
 *           =0: the value not changed due to identical;
 *           <0: failed with aosl_errno set accordingly;
 **/
extern __aosl_api__ ssize_t aosl_value_get_int64 (aosl_kfd_t dfd, const char *name, int64_t *v);

/**
 * Get intptr value from the value object specified by dfd + name.
 * Parameters:
 *               dfd: the opened fd of directory, AOSL_KFD_INVALID for root or pwd:
 *                    1. if the name starts with a '/', then directory is root;
 *                    2. if the name does not start with a '/', then directory is pwd;
 *              name: the directory name;
 *                 v: the memory address for holding the value;
 * Return value:
 *           >0: set the value successfully;
 *           =0: the value not changed due to identical;
 *           <0: failed with aosl_errno set accordingly;
 **/
extern __aosl_api__ ssize_t aosl_value_get_intptr (aosl_kfd_t dfd, const char *name, intptr_t *v);

/**
 * Get pointer value from the value object specified by dfd + name.
 * Parameters:
 *               dfd: the opened fd of directory, AOSL_KFD_INVALID for root or pwd:
 *                    1. if the name starts with a '/', then directory is root;
 *                    2. if the name does not start with a '/', then directory is pwd;
 *              name: the directory name;
 *                 v: the memory address for holding the value;
 * Return value:
 *           >0: set the value successfully;
 *           =0: the value not changed due to identical;
 *           <0: failed with aosl_errno set accordingly;
 **/
extern __aosl_api__ ssize_t aosl_value_get_pointer (aosl_kfd_t dfd, const char *name, void **v);

/**
 * Get float value from the value object specified by dfd + name.
 * Parameters:
 *               dfd: the opened fd of directory, AOSL_KFD_INVALID for root or pwd:
 *                    1. if the name starts with a '/', then directory is root;
 *                    2. if the name does not start with a '/', then directory is pwd;
 *              name: the directory name;
 *                 v: the memory address for holding the value;
 * Return value:
 *           >0: set the value successfully;
 *           =0: the value not changed due to identical;
 *           <0: failed with aosl_errno set accordingly;
 **/
extern __aosl_api__ ssize_t aosl_value_get_float (aosl_kfd_t dfd, const char *name, float *v);

/**
 * Get double value from the value object specified by dfd + name.
 * Parameters:
 *               dfd: the opened fd of directory, AOSL_KFD_INVALID for root or pwd:
 *                    1. if the name starts with a '/', then directory is root;
 *                    2. if the name does not start with a '/', then directory is pwd;
 *              name: the directory name;
 *                 v: the memory address for holding the value;
 * Return value:
 *           >0: set the value successfully;
 *           =0: the value not changed due to identical;
 *           <0: failed with aosl_errno set accordingly;
 **/
extern __aosl_api__ ssize_t aosl_value_get_double (aosl_kfd_t dfd, const char *name, double *v);

/**
 * Get string value from the value object specified by dfd + name.
 * Parameters:
 *               dfd: the opened fd of directory, AOSL_KFD_INVALID for root or pwd:
 *                    1. if the name starts with a '/', then directory is root;
 *                    2. if the name does not start with a '/', then directory is pwd;
 *              name: the directory name;
 *               str: the memory address for holding the value;
 *               len: the memory size in bytes;
 * Return value:
 *           >0: set the value successfully;
 *           =0: the value not changed due to identical;
 *           <0: failed with aosl_errno set accordingly;
 **/
extern __aosl_api__ ssize_t aosl_value_get_string (aosl_kfd_t dfd, const char *name, char *str, size_t len);

/**
 * Get data buffer value from the value object specified by dfd + name.
 * Parameters:
 *               dfd: the opened fd of directory, AOSL_KFD_INVALID for root or pwd:
 *                    1. if the name starts with a '/', then directory is root;
 *                    2. if the name does not start with a '/', then directory is pwd;
 *              name: the directory name;
 *               buf: the memory address for holding the value;
 *               len: the memory size in bytes;
 * Return value:
 *           >0: set the value successfully;
 *           =0: the value not changed due to identical;
 *           <0: failed with aosl_errno set accordingly;
 **/
extern __aosl_api__ ssize_t aosl_value_get_data (aosl_kfd_t dfd, const char *name, void *buf, size_t len);


/**
 * A registered callback function for verifying a setting value, if returns 0, then the value would
 * NOT be set at all, return non-zero if the value is OK for setting.
 * Parameters:
 *               val: the setting value to be verified;
 *              argc: the various args count, passed when registering;
 *              argv: the various args vector, passed as various args when registering;
 * Return value:
 *        The return value is a 'boolean' really, so
 *            0: the verifying failed, to value would NOT be set at all;
 *     non-zero: the value is OK for setting;
 **/
typedef int (*aosl_value_verify_t) (const aosl_value_t *val, uintptr_t argc, uintptr_t argv []);
typedef void (*aosl_value_verify_dtor_t) (uintptr_t argc, uintptr_t argv []);

/**
 * Register a verification callback for a value specified by dfd + name.
 * Parameters:
 *               dfd: the opened fd of directory, AOSL_KFD_INVALID for root or pwd:
 *                    1. if the name starts with a '/', then directory is root;
 *                    2. if the name does not start with a '/', then directory is pwd;
 *              name: the directory name;
 *                 f: the target callback function;
 *              dtor: destructor which be invoked when unregister the notification;
 *              argc: the various args count;
 *               ...: the various args;
 * Return value:
 *       non-NULL: successfully;
 *           NULL: failed with aosl_errno set accordingly.
 *       The return value only used for unregister, no other usage.
 **/
extern __aosl_api__ void *aosl_value_verify_register (aosl_kfd_t dfd, const char *name, aosl_value_verify_t f, aosl_value_verify_dtor_t dtor, uintptr_t argc, ...);

/**
 * Register a verification callback for a value specified by dfd + name.
 * Parameters:
 *               dfd: the opened fd of directory, AOSL_KFD_INVALID for root or pwd:
 *                    1. if the name starts with a '/', then directory is root;
 *                    2. if the name does not start with a '/', then directory is pwd;
 *              name: the directory name;
 *                 f: the target callback function;
 *              dtor: destructor which be invoked when unregister the notification;
 *              argc: the various args count;
 *              args: the various args;
 * Return value:
 *       non-NULL: successfully;
 *           NULL: failed with aosl_errno set accordingly.
 *       The return value only used for unregister, no other usage.
 **/
extern __aosl_api__ void *aosl_value_verify_register_args (aosl_kfd_t dfd, const char *name, aosl_value_verify_t f, aosl_value_verify_dtor_t dtor, uintptr_t argc, va_list args);

/**
 * Register a verification callback for a value specified by dfd + name.
 * Parameters:
 *               dfd: the opened fd of directory, AOSL_KFD_INVALID for root or pwd:
 *                    1. if the name starts with a '/', then directory is root;
 *                    2. if the name does not start with a '/', then directory is pwd;
 *              name: the directory name;
 *                 f: the target callback function;
 *              dtor: destructor which be invoked when unregister the notification;
 *              argc: the various args count;
 *              argv: the various args vector;
 * Return value:
 *       non-NULL: successfully;
 *           NULL: failed with aosl_errno set accordingly.
 *       The return value only used for unregister, no other usage.
 **/
extern __aosl_api__ void *aosl_value_verify_register_argv (aosl_kfd_t dfd, const char *name, aosl_value_verify_t f, aosl_value_verify_dtor_t dtor, uintptr_t argc, uintptr_t argv []);

/**
 * Unregister a verification callback for a value specified by dfd + name.
 * Parameters:
 *               dfd: the opened fd of directory, AOSL_KFD_INVALID for root or pwd:
 *                    1. if the name starts with a '/', then directory is root;
 *                    2. if the name does not start with a '/', then directory is pwd;
 *              name: the directory name;
 *            handle: the handle returned by above register functions;
 * Return value:
 *            0: successfully;
 *           <0: failed with aosl_errno set accordingly.
 **/
extern __aosl_api__ int aosl_value_verify_unregister (aosl_kfd_t dfd, const char *name, void *handle);



#ifdef __cplusplus
}
#endif



#endif /* __AOSL_VALUE_H__ */