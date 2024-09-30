#ifndef __AOSL_LOG_H__
#define __AOSL_LOG_H__

#include <api/aosl_types.h>
#include <api/aosl_defs.h>

#if defined (__linux__) || defined (__MACH__) || defined (__kliteos2__)
#include <syslog.h>

#define	AOSL_LOG_EMERG LOG_EMERG /* system is unusable */
#define	AOSL_LOG_ALERT LOG_ALERT /* action must be taken immediately */
#define	AOSL_LOG_CRIT LOG_CRIT /* critical conditions */
#define	AOSL_LOG_ERR LOG_ERR /* error conditions */
#define	AOSL_LOG_WARNING LOG_WARNING /* warning conditions */
#define	AOSL_LOG_NOTICE LOG_NOTICE /* normal but significant condition */
#define	AOSL_LOG_INFO LOG_INFO /* informational */
#define	AOSL_LOG_DEBUG LOG_DEBUG /* debug-level messages */

#define	AOSL_LOG_PRIMASK LOG_PRIMASK	/* mask to extract priority part (internal) */
/* extract priority */
#define	AOSL_LOG_PRI(p)	LOG_PRI(p)
#define	AOSL_LOG_MAKEPRI(fac, pri) LOG_MAKEPRI (fac, pri)
#else
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
/* For SCI_Trace/SCI_TraceHigh/SCI_TraceLow declarations. */
#include <RTOS/export/inc/os_api.h>
#endif

/*
 * priorities/facilities are encoded into a single 32-bit quantity, where the
 * bottom 3 bits are the priority (0-7) and the top 28 bits are the facility
 * (0-big number).  Both the priorities and the facilities map roughly
 * one-to-one to strings in the syslogd(8) source code.  This mapping is
 * included in this file.
 *
 * priorities (these are ordered)
 */
#define	AOSL_LOG_EMERG		0	/* system is unusable */
#define	AOSL_LOG_ALERT		1	/* action must be taken immediately */
#define	AOSL_LOG_CRIT		2	/* critical conditions */
#define	AOSL_LOG_ERR		3	/* error conditions */
#define	AOSL_LOG_WARNING	4	/* warning conditions */
#define	AOSL_LOG_NOTICE		5	/* normal but significant condition */
#define	AOSL_LOG_INFO		6	/* informational */
#define	AOSL_LOG_DEBUG		7	/* debug-level messages */

#define	AOSL_LOG_PRIMASK	0x07	/* mask to extract priority part (internal) */
/* extract priority */
#define	AOSL_LOG_PRI(p)	((p) & AOSL_LOG_PRIMASK)
#define	AOSL_LOG_MAKEPRI(fac, pri)	((fac) | (pri))
#endif

#ifdef __cplusplus
extern "C" {
#endif


#ifndef _MSC_VER
#define aosl_printf_fmt(a, b) __attribute__ ((format (printf, a, b)))
#define aosl_scanf_fmt(a, b) __attribute__ ((format (scanf, a, b)))
#else
#define aosl_printf_fmt(a, b)
#define aosl_scanf_fmt(a, b)
#endif

#define aosl_printf_fmt12 aosl_printf_fmt (1, 2)
#define aosl_printf_fmt23 aosl_printf_fmt (2, 3)



/**
 * The log callback function.
 * The log level uses the same values as syslog, these values are used by AOSL library and
 * its' descendents, so please translate the log levels to your own log levels respectively.
 **/
typedef void (*aosl_vlog_t) (int level, const char *fmt, va_list args);

/**
 * The default AOSL log function is syslog, but you can provide your own log function.
 * Pay enough attention please, your own log function MUST conform the same semantics
 * with syslog, and all log levels must have the same meaning with the ones of syslog.
 **/
extern __aosl_api__ void aosl_set_vlog_func (aosl_vlog_t vlog);

/**
 * Get the aosl log level, AOSL_LOG_ERR is the default value.
 * Parameters:
 *     none.
 * Return value:
 *     the current aosl log level.
 **/
extern __aosl_api__ int aosl_get_log_level (void);

/**
 * Set the aosl log level, AOSL_LOG_ERR is the default value.
 * Parameters:
 *     level: the log level you want to set
 * Return value:
 *     none.
 **/
extern __aosl_api__ void aosl_set_log_level (int level);

extern __aosl_api__ void aosl_printf_fmt23 aosl_log (int level, const char *fmt, ...);
extern __aosl_api__ void aosl_vlog (int level, const char *fmt, va_list args);

#if !defined (__linux__) && !defined (__MACH__) && !defined (__kliteos2__)
extern __aosl_api__ void aosl_printf_fmt23 syslog (int level, const char *fmt, ...);
extern __aosl_api__ void vsyslog (int level, const char *fmt, va_list args);
#endif

extern __aosl_api__ void aosl_printf_fmt12 aosl_printf (const char *fmt, ...);
extern __aosl_api__ void aosl_vprintf (const char *fmt, va_list args);


extern __aosl_api__ void aosl_printf_fmt12 aosl_panic (const char *fmt, ...);


#ifdef __cplusplus
}
#endif



#endif /* __AOSL_LOG_H__ */