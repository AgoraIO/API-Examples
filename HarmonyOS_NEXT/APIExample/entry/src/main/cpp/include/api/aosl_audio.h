/*************************************************************
 * Author:	Lionfore Hao (haolianfu@agora.io)
 * Date	 :	Jan 26th, 2023
 * Module:	AOSL audio relative API definition file
 *
 *
 * This is a part of the Advanced Operating System Layer.
 * Copyright (C) 2018 ~ 2023 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#ifndef __AOSL_AUDIO_H__
#define __AOSL_AUDIO_H__


#include <api/aosl_types.h>
#include <api/aosl_defs.h>
#include <api/aosl_time.h>
#include <api/aosl_data.h>


#ifdef __cplusplus
extern "C" {
#endif


#define AOSL_PCM_FMT_UNSIGNED 0x80 /* Set if unsigned data for integer format */
#define AOSL_PCM_FMT_BE 0x40 /* Set if bigendian */
#define AOSL_PCM_FMT_NONINTERLEAVED 0x20 /* Set if non interleaved data for channels */

/* Integer formats */
#define AOSL_PCM_FMT_I8 0x0
#define AOSL_PCM_FMT_I16 0x1
#define AOSL_PCM_FMT_I24 0x2
#define AOSL_PCM_FMT_I32 0x3
/* Float formats */
#define AOSL_PCM_FMT_F32 0x4
#define AOSL_PCM_FMT_F64 0x5

/* format type mask */
#define AOSL_PCM_FMT_TYPE_MASK 0xf


/* Integer, Signed, Interleaved */
#define AOSL_PCM_FMT_S8 AOSL_PCM_FMT_I8

/* Integer, Signed, Little Endian, Interleaved */
#define AOSL_PCM_FMT_S16_LE AOSL_PCM_FMT_I16
#define AOSL_PCM_FMT_S24_LE AOSL_PCM_FMT_I24
#define AOSL_PCM_FMT_S32_LE AOSL_PCM_FMT_I32

/* Float: Little Endian, Interleaved */
#define AOSL_PCM_FMT_FLOAT_LE AOSL_PCM_FMT_F32
#define AOSL_PCM_FMT_FLOAT64_LE AOSL_PCM_FMT_F64

/* Integer: Signed, Big Endian, Interleaved */
#define AOSL_PCM_FMT_S16_BE (AOSL_PCM_FMT_I16 | AOSL_PCM_FMT_BE)
#define AOSL_PCM_FMT_S24_BE (AOSL_PCM_FMT_I24 | AOSL_PCM_FMT_BE)
#define AOSL_PCM_FMT_S32_BE (AOSL_PCM_FMT_I32 | AOSL_PCM_FMT_BE)

/* Float: Big Endian, Interleaved */
#define AOSL_PCM_FMT_FLOAT_BE (AOSL_PCM_FMT_F32 | AOSL_PCM_FMT_BE)
#define AOSL_PCM_FMT_FLOAT64_BE (AOSL_PCM_FMT_F64 | AOSL_PCM_FMT_BE)

/* Integer, Unsigned, Interleaved */
#define AOSL_PCM_FMT_U8 (AOSL_PCM_FMT_I8 | AOSL_PCM_FMT_UNSIGNED)

/* Integer, Unsigned, Little Endian, Interleaved */
#define AOSL_PCM_FMT_U16_LE (AOSL_PCM_FMT_I16 | AOSL_PCM_FMT_UNSIGNED)
#define AOSL_PCM_FMT_U24_LE (AOSL_PCM_FMT_I24 | AOSL_PCM_FMT_UNSIGNED)
#define AOSL_PCM_FMT_U32_LE (AOSL_PCM_FMT_I32 | AOSL_PCM_FMT_UNSIGNED)

/* Integer, Unigned, Big Endian, Interleaved */
#define AOSL_PCM_FMT_U16_BE (AOSL_PCM_FMT_I16 | AOSL_PCM_FMT_UNSIGNED | AOSL_PCM_FMT_BE)
#define AOSL_PCM_FMT_U24_BE (AOSL_PCM_FMT_I24 | AOSL_PCM_FMT_UNSIGNED | AOSL_PCM_FMT_BE)
#define AOSL_PCM_FMT_U32_BE (AOSL_PCM_FMT_I32 | AOSL_PCM_FMT_UNSIGNED | AOSL_PCM_FMT_BE)

/* Integer, Signed, Noninterleaved */
#define AOSL_PCM_FMT_S8_N (AOSL_PCM_FMT_I8 | AOSL_PCM_FMT_NONINTERLEAVED)

/* Integer, Signed, Little Endian, Noninterleaved */
#define AOSL_PCM_FMT_S16_LE_N (AOSL_PCM_FMT_I16 | AOSL_PCM_FMT_NONINTERLEAVED)
#define AOSL_PCM_FMT_S24_LE_N (AOSL_PCM_FMT_I24 | AOSL_PCM_FMT_NONINTERLEAVED)
#define AOSL_PCM_FMT_S32_LE_N (AOSL_PCM_FMT_I32 | AOSL_PCM_FMT_NONINTERLEAVED)

/* Float: Little Endian, Noninterleaved */
#define AOSL_PCM_FMT_FLOAT_LE_N (AOSL_PCM_FMT_F32 | AOSL_PCM_FMT_NONINTERLEAVED)
#define AOSL_PCM_FMT_FLOAT64_LE_N (AOSL_PCM_FMT_F64 | AOSL_PCM_FMT_NONINTERLEAVED)

/* Integer, Signed, Big Endian, Noninterleaved */
#define AOSL_PCM_FMT_S16_BE_N (AOSL_PCM_FMT_I16 | AOSL_PCM_FMT_BE | AOSL_PCM_FMT_NONINTERLEAVED)
#define AOSL_PCM_FMT_S24_BE_N (AOSL_PCM_FMT_I24 | AOSL_PCM_FMT_BE | AOSL_PCM_FMT_NONINTERLEAVED)
#define AOSL_PCM_FMT_S32_BE_N (AOSL_PCM_FMT_I32 | AOSL_PCM_FMT_BE | AOSL_PCM_FMT_NONINTERLEAVED)

/* Float: Big Endian, Noninterleaved */
#define AOSL_PCM_FMT_FLOAT_BE_N (AOSL_PCM_FMT_F32 | AOSL_PCM_FMT_BE | AOSL_PCM_FMT_NONINTERLEAVED)
#define AOSL_PCM_FMT_FLOAT64_BE_N (AOSL_PCM_FMT_F64 | AOSL_PCM_FMT_BE | AOSL_PCM_FMT_NONINTERLEAVED)

/* Integer, Unsigned, Noninterleaved */
#define AOSL_PCM_FMT_U8_N (AOSL_PCM_FMT_I8 | AOSL_PCM_FMT_UNSIGNED | AOSL_PCM_FMT_NONINTERLEAVED)

/* Integer, Unsigned, Little Endian, Noninterleaved */
#define AOSL_PCM_FMT_U16_LE_N (AOSL_PCM_FMT_I16 | AOSL_PCM_FMT_UNSIGNED | AOSL_PCM_FMT_NONINTERLEAVED)
#define AOSL_PCM_FMT_U24_LE_N (AOSL_PCM_FMT_I24 | AOSL_PCM_FMT_UNSIGNED | AOSL_PCM_FMT_NONINTERLEAVED)
#define AOSL_PCM_FMT_U32_LE_N (AOSL_PCM_FMT_I32 | AOSL_PCM_FMT_UNSIGNED | AOSL_PCM_FMT_NONINTERLEAVED)

/* Integer, Unsigned, Big Endian, Noninterleaved */
#define AOSL_PCM_FMT_U16_BE_N (AOSL_PCM_FMT_I16 | AOSL_PCM_FMT_UNSIGNED | AOSL_PCM_FMT_BE | AOSL_PCM_FMT_NONINTERLEAVED)
#define AOSL_PCM_FMT_U24_BE_N (AOSL_PCM_FMT_I24 | AOSL_PCM_FMT_UNSIGNED | AOSL_PCM_FMT_BE | AOSL_PCM_FMT_NONINTERLEAVED)
#define AOSL_PCM_FMT_U32_BE_N (AOSL_PCM_FMT_I32 | AOSL_PCM_FMT_UNSIGNED | AOSL_PCM_FMT_BE | AOSL_PCM_FMT_NONINTERLEAVED)

typedef struct {
	uint32_t sample_rate;
	uint8_t format;
	uint8_t channels;
} aosl_pcm_fmt_t;


#define AOSL_IOCS_AUDIO_PARAMS 1
#define AOSL_IOCG_AUDIO_PARAMS 2

/* aosl_kobj_ioctl (vfd, AOSL_IOCS_AUDIO_CAP_START, 4, <mpq id>, <aosl_abuf_cb_t>, <aosl_abuf_ev_t>, <user arg>); */
#define AOSL_IOCS_AUDIO_CAP_START 3

/* aosl_kobj_ioctl (vfd, AOSL_IOCS_AUDIO_CAP_STOP, 0); */
#define AOSL_IOCS_AUDIO_CAP_STOP 4


typedef aosl_data_t aosl_abuf_t;

/**
 * Get the sample size for the specified PCM format.
 * Parameters:
 *              fmt: the PCM format;
 * Return value:
 *           >0: the sample size (in bytes) for the format;
 *           <0: failed with aosl_errno set;
 **/
extern __aosl_api__ int aosl_pcm_sample_size (uint8_t fmt);

/**
 * Create an audio buffer with the specified parameters.
 * Parameters:
 *           sample_rate: the PCM sample rate;
 *                   fmt: the PCM format;
 *              channels: the PCM channels, 1 for mono 2 for stereo etc;
 *                   nms: how many milliseconds audio data the buffer holding;
 * Return value:
 *     non-NULL: the audio buffer created;
 *         NULL: failed with aosl_errno set;
 **/
extern __aosl_api__ aosl_abuf_t aosl_abuf_create (uint32_t sample_rate, uint8_t fmt, uint8_t channels, uint32_t nms);

/**
 * Get the audio data timestamp in microseconds of a audio buffer object.
 * Parameters:
 *              abuf: the audio buffer object;
 * Return value:
 *           the audio data timestamp in microseconds;
 **/
extern __aosl_api__ aosl_ts_t aosl_abuf_us (aosl_abuf_t abuf);

/**
 * Get the audio data PCM format of a audio buffer object.
 * Parameters:
 *              abuf: the audio buffer object;
 *               fmt: the buffer for holding the format;
 * Return value:
 *                 0: successful with fmt filled;
 *                <0: failed with aosl_errno set;
 **/
extern __aosl_api__ int aosl_abuf_fmt (aosl_abuf_t abuf, aosl_pcm_fmt_t *fmt);


#ifdef __cplusplus
}
#endif



#endif /* __AOSL_AUDIO_H__ */