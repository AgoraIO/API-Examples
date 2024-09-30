/*************************************************************
 * Author:	Lionfore Hao (haolianfu@agora.io)
 * Date	 :	Mar 2nd, 2023
 * Module:	AOSL video codec relative API definition file
 *
 *
 * This is a part of the Advanced Operating System Layer.
 * Copyright (C) 2018 ~ 2023 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#ifndef __AOSL_VIDEO_CODEC_H__
#define __AOSL_VIDEO_CODEC_H__


#include <api/aosl_types.h>
#include <api/aosl_defs.h>
#include <api/aosl_time.h>


#ifdef __cplusplus
extern "C" {
#endif


typedef struct {
	int bit_rate_scale;
	int cpb_size_scale;
	int initial_cpb_removal_delay_length;
	int cpb_removal_delay_length;
	int dpb_output_delay_length;
	int bit_rate_value;
	int cpb_size_value;
	int cbr_flag;
} aosl_hrd_params_t;

typedef struct {
	/* --- sample aspect ratio (SAR) --- */
	unsigned char aspect_ratio_info_present_flag;
	unsigned char aspect_ratio_idc;
	unsigned short sar_width; /* sar_width and sar_height are zero if unspecified */
	unsigned short sar_height;

	/* --- overscan --- */
	unsigned char overscan_info_present_flag;
	unsigned char overscan_appropriate_flag;

	/* --- video signal type --- */
	unsigned char video_signal_type_present_flag;
	unsigned char video_format;
	unsigned char video_full_range_flag;
	unsigned char colour_description_present_flag;
	unsigned char colour_primaries;
	unsigned char transfer_characteristics;
	unsigned char matrix_coeffs;

	/* --- chroma / interlaced --- */
	unsigned char chroma_loc_info_present_flag;
	unsigned char chroma_sample_loc_type_top_field;
	unsigned char chroma_sample_loc_type_bottom_field;
	unsigned char neutral_chroma_indication_flag;
	unsigned char field_seq_flag;
	unsigned char frame_field_info_present_flag;

	/* --- default display window --- */
	unsigned char default_display_window_flag;
	unsigned int def_disp_win_left_offset;
	unsigned int def_disp_win_right_offset;
	unsigned int def_disp_win_top_offset;
	unsigned int def_disp_win_bottom_offset;

	/* --- timing --- */
	unsigned char vui_timing_info_present_flag;
	unsigned int vui_num_units_in_tick;
	unsigned int vui_time_scale;

	unsigned char vui_poc_proportional_to_timing_flag;
	unsigned int vui_num_ticks_poc_diff_one;

	/* --- hrd parameters --- */
	unsigned char vui_hrd_parameters_present_flag;
	aosl_hrd_params_t vui_hrd_parameters;

	/* --- bitstream restriction --- */
	unsigned char bitstream_restriction_flag;
	unsigned char tiles_fixed_structure_flag;
	unsigned char motion_vectors_over_pic_boundaries_flag;
	unsigned char restricted_ref_pic_lists_flag;
	unsigned short min_spatial_segmentation_idc;
	unsigned char max_bytes_per_pic_denom;
	unsigned char max_bits_per_mincu_denom;
	unsigned char log2_max_mv_length_horizontal;
	unsigned char log2_max_mv_length_vertical;
} aosl_vui_params_t;

typedef enum {
	EXTC_OK = (0x00000000), // Success codes
	EXTC_FAIL = (0x80000001), // Unspecified error

	EXTC_OUTOFMEMORY = (0x80000002), // Out of memory
	EXTC_POINTER = (0x80000003), // Invalid pointer
	EXTC_NOTSUPPORTED = (0x80000004), // NOT support feature encountered
	EXTC_NO_INSTANCE = (0x80000005),

	EXTC_NEED_MORE_DATA = (0x90000001), // need push more data
} aosl_extc_code_t;

typedef enum {
	VIDEO_CODEC_H264 = 0,
	VIDEO_CODEC_HEVC,
} aosl_vcodec_type_t;

typedef struct {
	aosl_vcodec_type_t codec;
	int isEncoder;
	int isHardware;
	int maxInstances;
	int maxDealyFrames;

	int maxWidth;
	int maxHeight;
	int minWidth;
	int minHeight;
	int alignment;
} aosl_vcodec_cap_t;

typedef struct {
	int size;
	char* name;
	aosl_vcodec_cap_t *caps;
} aosl_vcodec_info_t;

extern aosl_vcodec_info_t *agora_partener_get_codec_info (void);



#ifdef __cplusplus
}
#endif



#endif /* __AOSL_VIDEO_CODEC_H__ */