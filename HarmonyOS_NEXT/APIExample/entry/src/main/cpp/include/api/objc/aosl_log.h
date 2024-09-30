#ifndef __AOSL_LOG_OBJC_H__
#define __AOSL_LOG_OBJC_H__

#import <Foundation/NSString.h>

#include <api/aosl_types.h>
#include <api/aosl_log.h>


#ifdef __cplusplus
extern "C" {
#endif



typedef void (*aosl_objc_vlog_t) (int level, const NSString *format, va_list args);

extern void aosl_set_vlog_func_objc (aosl_objc_vlog_t vlog);



#ifdef __cplusplus
}
#endif



#endif /* __AOSL_LOG_OBJC_H__ */
