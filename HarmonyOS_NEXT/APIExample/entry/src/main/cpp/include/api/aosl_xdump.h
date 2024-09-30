#ifndef __AOSL_XDUMP_H__
#define __AOSL_XDUMP_H__

#include <api/aosl_defs.h>
#include <api/aosl_types.h>

#ifdef __cplusplus
extern "C" {
#endif


/**
 * xdump callback info about crash:
 *   @module_ver: the module version
 *   @module_name: the module name
 *   @log_file: the full path of log_file
 *   @type: the dump type:
 *          0: crash
 *          1: dead loop/dead lock/ANR
 *   @crash_at: virtual address where the crash happen
 *   @ld_begin/ld_end: DSO will be loaded on [ld_begin, ld_end)
 **/
#define AOSL_XDUMP_CRASH 0
#define AOSL_XDUMP_DEAD 1
typedef struct {
    uintptr_t module_ver;
    const char *module_name;
    const char *log_file;
	int type;
    void *crash_at;
    void *ld_begin;
    void *ld_end;
    int q_flags;
} xdump_info_t;
typedef void (*xdump_cb_t)(const xdump_info_t *log_file);

extern __aosl_api__ int aosl_xdump_attach(const char *log_dir, xdump_cb_t cb);
extern __aosl_api__ int aosl_xdump_detach();

#ifdef __cplusplus
}
#endif

#endif /* __AOSL_XDUMP_H__ */
