/*************************************************************
 * Author:	Lionfore Hao (haolianfu@agora.io)
 * Date	 :	Jan 26th, 2023
 * Module:	AOSL video relative API definition file
 *
 *
 * This is a part of the Advanced Operating System Layer.
 * Copyright (C) 2018 ~ 2023 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#ifndef __AOSL_VIDEO_H__
#define __AOSL_VIDEO_H__


#include <api/aosl_types.h>
#include <api/aosl_defs.h>
#include <api/aosl_time.h>
#include <api/aosl_data.h>


#ifdef __cplusplus
extern "C" {
#endif



typedef aosl_data_t aosl_vbuf_t;

typedef void (*aosl_vbuf_cb_t) (int dev, aosl_vbuf_t vbuf, void *arg);
typedef void (*aosl_vbuf_ev_t) (int dev, int event, void *arg);

/* aosl_kobj_ioctl (vfd, AOSL_IOCS_VIDEO_CAP_START, 4, <mpq id>, <aosl_vbuf_cb_t>, <aosl_vbuf_ev_t>, <user arg>); */
#define AOSL_IOCS_VIDEO_CAP_START 1

/* aosl_kobj_ioctl (vfd, AOSL_IOCS_VIDEO_CAP_STOP, 0); */
#define AOSL_IOCS_VIDEO_CAP_STOP 2


typedef enum {
	AOSL_PIXFMT_UNKNOWN = -1,

	/* one plane interleaved 3 bytes */
	AOSL_PIXFMT_RGB24 = 0, /* 24 RGB-8-8-8 */
	AOSL_PIXFMT_BGR24, /* 24 BGR-8-8-8 */
	AOSL_PIXFMT_RGB32, /* 32 RGB-8-8-8-8 */
	AOSL_PIXFMT_BGR32, /* 32 BGR-8-8-8-8 */

	/* one plane interleaved 4 bytes */
	AOSL_PIXFMT_YUYV,
	AOSL_PIXFMT_YYUV,
	AOSL_PIXFMT_YVYU,
	AOSL_PIXFMT_UYVY,
	AOSL_PIXFMT_VYUY,

	/* two planes: one Y, one Cb+Cr interleaved */
	AOSL_PIXFMT_NV12, /* 12 Y/CbCr 4:2:0  */
	AOSL_PIXFMT_NV21, /* 12 Y/CrCb 4:2:0  */
	AOSL_PIXFMT_NV16, /* 16 Y/CbCr 4:2:2  */
	AOSL_PIXFMT_NV61, /* 16 Y/CrCb 4:2:2  */

	/* three planes: one for each */
	AOSL_PIXFMT_YUV420, /* 12 YUV 4:2:0 */
	AOSL_PIXFMT_YUV422, /* 16 YUV 4:2:2 */

	AOSL_PIXFMT_HWSURF, /* Hardware surface */
} aosl_pixfmt_t;

typedef enum {
	aosl_vbuf_type_dma,
	aosl_vbuf_type_win,
	aosl_vbuf_type_cpu,
} aosl_vbuf_type_t;


/**
 * Get the image width of a video buffer object.
 * Parameters:
 *         vbuf: the video buffer object;
 * Return value:
 *          >=0: one of aosl_vbuf_type_t value;
 *           <0: failed with aosl_errno set;
 **/
extern __aosl_api__ int aosl_vbuf_type (aosl_vbuf_t vbuf);

/**
 * Get the image width of a video buffer object.
 * Parameters:
 *         vbuf: the video buffer object;
 * Return value:
 *         the video image width;
 **/
extern __aosl_api__ int aosl_vbuf_width (aosl_vbuf_t vbuf);

/**
 * Get the image height of a video buffer object.
 * Parameters:
 *         vbuf: the video buffer object;
 * Return value:
 *         the video image height;
 **/
extern __aosl_api__ int aosl_vbuf_height (aosl_vbuf_t vbuf);

/**
 * Get the video data timestamp in microseconds of a video buffer object.
 * Parameters:
 *         vbuf: the video buffer object;
 * Return value:
 *         the video data timestamp in microseconds;
 **/
extern __aosl_api__ aosl_ts_t aosl_vbuf_us (aosl_vbuf_t vbuf);

/**
 * Get the video data format of a video buffer object.
 * Parameters:
 *         vbuf: the video buffer object;
 * Return value:
 *         the video data pixel format;
 **/
extern __aosl_api__ aosl_pixfmt_t aosl_vbuf_pixfmt (aosl_vbuf_t vbuf);

/**
 * normalized rect: range 0.0 ~ 1.0 for each direction
 * (0.0, 0.0) <=> (left, bottom)
 * (1.0, 1.0) <=> (width, height)
 **/
typedef struct {
	double left;
	double bottom;
	double width;
	double height;
} aosl_nmrect_t;

/**
 * Render the video buffer on the specified surface.
 * Parameters:
 *           surface: the surface rendering on
 *                    1.      Unix X11: a Window id;
 *                    2.        MacOSX: an NSWindow pointer;
 *                    3.       Windows: an HWND handle;
 *                    4. Linux Wayland: ?
 *                    5.           iOS: a UIView pointer;
 *                    6.       Android: an ANativeWindow pointer;
 *              vbuf: the video buffer object;
 *              rect: specified the rendering rect on the surface.
 *                    NULL for whole surface area.
 *              clip: bool value: 1 for clip; 0 for not clip;
 * Return value:
 *            0: sucessfully;
 *           <0: failed with aosl_errno set;
 **/
extern __aosl_api__ int aosl_surface_render (void *surface, aosl_vbuf_t vbuf, const aosl_nmrect_t *rect, int clip);

/**
 * Lock the the specified surface for video rendering.
 * Parameters:
 *           surface: the surface rendering on
 *                    1.      Unix X11: a Window id;
 *                    2.        MacOSX: an NSWindow pointer;
 *                    3.       Windows: an HWND handle;
 *                    4. Linux Wayland: ?
 *                    5.           iOS: a UIView pointer;
 *                    6.       Android: an ANativeWindow pointer;
 * Return value:
 *            0: sucessfully;
 *           <0: failed with aosl_errno set;
 **/
extern __aosl_api__ int aosl_surface_lock (void *surface);

/**
 * Unlock the the specified surface for video rendering.
 * Parameters:
 *           surface: the surface rendering on
 *                    1.      Unix X11: a Window id;
 *                    2.        MacOSX: an NSWindow pointer;
 *                    3.       Windows: an HWND handle;
 *                    4. Linux Wayland: ?
 *                    5.           iOS: a UIView pointer;
 *                    6.       Android: an ANativeWindow pointer;
 * Return value:
 *            0: sucessfully;
 *           <0: failed with aosl_errno set;
 **/
extern __aosl_api__ int aosl_surface_unlock (void *surface);

/**
 * Cleanup the rendering resources on the specified surface.
 * Parameters:
 *           surface: the surface rendering on
 *                    1.      Unix X11: a Window id;
 *                    2.        MacOSX: an NSWindow pointer;
 *                    3.       Windows: an HWND handle;
 *                    4. Linux Wayland: ?
 *                    5.           iOS: a UIView pointer;
 *                    6.       Android: an ANativeWindow pointer;
 * Return value:
 *            0: sucessfully;
 *           <0: failed with aosl_errno set;
 **/
extern __aosl_api__ int aosl_surface_cleanup (void *surface);

/**
 * Set the arg for the specified surface.
 * Parameters:
 *           surface: the surface rendering on
 *                    1.      Unix X11: a Window id;
 *                    2.        MacOSX: an NSWindow pointer;
 *                    3.       Windows: an HWND handle;
 *                    4. Linux Wayland: ?
 *                    5.           iOS: a UIView pointer;
 *                    6.       Android: an ANativeWindow pointer;
 *               arg: the arg to be set for the surface;
 * Return value:
 *            0: sucessfully;
 *           <0: failed with aosl_errno set;
 **/
extern __aosl_api__ int aosl_surface_arg_set (void *surface, void *arg);

/**
 * Get the arg for the specified surface.
 * Parameters:
 *           surface: the surface rendering on
 *                    1.      Unix X11: a Window id;
 *                    2.        MacOSX: an NSWindow pointer;
 *                    3.       Windows: an HWND handle;
 *                    4. Linux Wayland: ?
 *                    5.           iOS: a UIView pointer;
 *                    6.       Android: an ANativeWindow pointer;
 *             arg_p: the memory address for saving the arg of the surface;
 * Return value:
 *            0: sucessfully;
 *           <0: failed with aosl_errno set;
 **/
extern __aosl_api__ int aosl_surface_arg_get (void *surface, void **arg_p);



#ifdef __cplusplus
}
#endif



#endif /* __AOSL_VIDEO_H__ */