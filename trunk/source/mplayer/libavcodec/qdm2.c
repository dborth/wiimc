/*
 * QDM2 compatible decoder
 * Copyright (c) 2003 Ewald Snel
 * Copyright (c) 2005 Benjamin Larsson
 * Copyright (c) 2005 Alex Beregszaszi
 * Copyright (c) 2005 Roberto Togni
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/**
 * @file libavcodec/qdm2.c
 * QDM2 decoder
 * @author Ewald Snel, Benjamin Larsson, Alex Beregszaszi, Roberto Togni
 * The decoder is not perfect yet, there are still some distortions
 * especially on files encoded with 16 or 8 subbands.
 */

#include <math.h>
#include <stddef.h>
#include <stdio.h>

#define ALT_BITSTREAM_READER_LE
#include "avcodec.h"
#include "get_bits.h"
#include "dsputil.h"
#include "fft.h"
#include "mpegaudio.h"

#include "qdm2data.h"
#include "qdm2_tablegen.h"

#undef NDEBUG
#include <assert.h>


#define QDM2_LIST_ADD(list, size, packet) \
do { \
      if (size > 0) { \
    list[size - 1].next = &list[size]; \
      } \
      list[size].packet = packet; \
      list[size].next = NULL; \
      size++; \
} while(0)

// Result is 8, 16 or 30
#define QDM2_SB_USED(sub_sampling) (((sub_sampling) >= 2) ? 30 : 8 << (sub_sampling))

#define FIX_NOISE_IDX(noise_idx) \
  if ((noise_idx) >= 3840) \
    (noise_idx) -= 3840; \

#define SB_DITHERING_NOISE(sb,noise_idx) (noise_table[(noise_idx)++] * sb_noise_attenuation[(sb)])

#define BITS_LEFT(length,gb) ((length) - get_bits_count ((gb)))

#define SAMPLES_NEEDED \
     av_log (NULL,AV_LOG_INFO,"This file triggers some untested code. Please contact the developers.\n");

#define SAMPLES_NEEDED_2(why) \
     av_log (NULL,AV_LOG_INFO,"This file triggers some missing code. Please contact the developers.\nPosition: %s\n",why);


typedef int8_t sb_int8_array[2][30][64];

/**
 * Subpacket
 */
typedef struct {
    int type;            ///< subpacket type
    unsigned int size;   ///< subpacket size
    const uint8_t *data; ///< pointer to subpacket data (points to input data buffer, it's not a private copy)
} QDM2SubPacket;

/**
 * A node in the subpacket list
 */
typedef struct QDM2SubPNode {
    QDM2SubPacket *packet;      ///< packet
    struct QDM2SubPNode *next; ///< pointer to next packet in the list, NULL if leaf node
} QDM2SubPNode;

typedef struct {
    float re;
    float im;
} QDM2Complex;

typedef struct {
    float level;
    QDM2Complex *complex;
    const float *table;
    int   phase;
    int   phase_shift;
    int   duration;
    short time_index;
    short cutoff;
} FFTTone;

typedef struct {
    int16_t sub_packet;
    uint8_t channel;
    int16_t offset;
    int16_t exp;
    uint8_t phase;
} FFTCoefficient;

typedef struct {
    DECLARE_ALIGNED(16, QDM2Complex, complex)[MPA_MAX_CHANNELS][256];
} QDM2FFT;

/**
 * QDM2 decoder context
 */
typedef struct {
    /// Parameters from codec header, do not change during playback
    int nb_channels;         ///< number of channels
    int channels;            ///< number of channels
    int group_size;          ///< size of frame group (16 frames per group)
    int fft_size;            ///< size of FFT, in complex numbers
    int checksum_size;       ///< size of data block, used also for checksum

    /// Parameters built from header parameters, do not change during playback
    int group_order;         ///< order of frame group
    int fft_order;           ///< order of FFT (actually fftorder+1)
    int fft_frame_size;      ///< size of fft frame, in components (1 comples = re + im)
    int frame_size;          ///< size of data frame
    int frequency_range;
    int sub_sampling;        ///< subsampling: 0=25%, 1=50%, 2=100% */
    int coeff_per_sb_select; ///< selector for "num. of coeffs. per subband" tables. Can be 0, 1, 2
    int cm_table_select;     ///< selector for "coding method" tables. Can be 0, 1 (from init: 0-4)

    /// Packets and packet lists
    QDM2SubPacket sub_packets[16];      ///< the packets themselves
    QDM2SubPNode sub_packet_list_A[16]; ///< list of all packets
    QDM2SubPNode sub_packet_list_B[16]; ///< FFT packets B are on list
    int sub_packets_B;                  ///< number of packets on 'B' list
    QDM2SubPNode sub_packet_list_C[16]; ///< packets with errors?
    QDM2SubPNode sub_packet_list_D[16]; ///< DCT packets

    /// FFT and tones
    FFTTone fft_tones[1000];
    int fft_tone_start;
    int fft_tone_end;
    FFTCoefficient fft_coefs[1000];
    int fft_coefs_index;
    int fft_coefs_min_index[5];
    int fft_coefs_max_index[5];
    int fft_level_exp[6];
    RDFTContext rdft_ctx;
    QDM2FFT fft;

    /// I/O data
    const uint8_t *compressed_data;
    int compressed_size;
    float output_buffer[1024];

    /// Synthesis filter
    DECLARE_ALIGNED(16, MPA_INT, synth_buf)[MPA_MAX_CHANNELS][512*2];
    int synth_buf_offset[MPA_MAX_CHANNELS];
    DECLARE_ALIGNED(16, int32_t, sb_samples)[MPA_MAX_CHANNELS][128][SBLIMIT];

    /// Mixed temporary data used in decoding
    float tone_level[MPA_MAX_CHANNELS][30][64];
    int8_t coding_method[MPA_MAX_CHANNELS][30][64];
    int8_t quantized_coeffs[MPA_MAX_CHANNELS][10][8];
    int8_t tone_level_idx_base[MPA_MAX_CHANNELS][30][8];
    int8_t tone_level_idx_hi1[MPA_MAX_CHANNELS][3][8][8];
    int8_t tone_level_idx_mid[MPA_MAX_CHANNELS][26][8];
    int8_t tone_level_idx_hi2[MPA_MAX_CHANNELS][26];
    int8_t tone_level_idx[MPA_MAX_CHANNELS][30][64];
    int8_t tone_level_idx_temp[MPA_MAX_CHANNELS][30][64];

    // Flags
    int has_errors;         ///< packet has errors
    int superblocktype_2_3; ///< select fft tables and some algorithm based on superblock type
    int do_synth_filter;    ///< used to perform or skip synthesis filter

    int sub_packet;
    int noise_idx; ///< index for dithering noise table
} QDM2Context;


static uint8_t empty_buffer[FF_INPUT_BUFFER_PADDING_SIZE];

static VLC vlc_tab_level;
static VLC vlc_tab_diff;
static VLC vlc_tab_run;
static VLC fft_level_exp_alt_vlc;
static VLC fft_level_exp_vlc;
static VLC fft_stereo_exp_vlc;
static VLC fft_stereo_phase_vlc;
static VLC vlc_tab_tone_level_idx_hi1;
static VLC vlc_tab_tone_level_idx_mid;
static VLC vlc_tab_tone_level_idx_hi2;
static VLC vlc_tab_type30;
static VLC vlc_tab_type34;
static VLC vlc_tab_fft_tone_offset[5];

static const uint16_t qdm2_vlc_offs[] = {
    0,260,566,598,894,1166,1230,1294,1678,1950,2214,2278,2310,2570,2834,3124,3448,3838,
};

static av_cold void qdm2_init_vlc(void)
{
    static int vlcs_initialized = 0;
    static VLC_TYPE qdm2_table[3838][2];

    if (!vlcs_initialized) {

        vlc_tab_level.table = &qdm2_table[qdm2_vlc_offs[0]];
        vlc_tab_level.table_allocated = qdm2_vlc_offs[1] - qdm2_vlc_offs[0];
        init_vlc (&vlc_tab_level, 8, 24,
            vlc_tab_level_huffbits, 1, 1,
            vlc_tab_level_huffcodes, 2, 2, INIT_VLC_USE_NEW_STATIC | INIT_VLC_LE);

        vlc_tab_diff.table = &qdm2_table[qdm2_vlc_offs[1]];
        vlc_tab_diff.table_allocated = qdm2_vlc_offs[2] - qdm2_vlc_offs[1];
        init_vlc (&vlc_tab_diff, 8, 37,
            vlc_tab_diff_huffbits, 1, 1,
            vlc_tab_diff_huffcodes, 2, 2, INIT_VLC_USE_NEW_STATIC | INIT_VLC_LE);

        vlc_tab_run.table = &qdm2_table[qdm2_vlc_offs[2]];
        vlc_tab_run.table_allocated = qdm2_vlc_offs[3] - qdm2_vlc_offs[2];
        init_vlc (&vlc_tab_run, 5, 6,
            vlc_tab_run_huffbits, 1, 1,
            vlc_tab_run_huffcodes, 1, 1, INIT_VLC_USE_NEW_STATIC | INIT_VLC_LE);

        fft_level_exp_alt_vlc.table = &qdm2_table[qdm2_vlc_offs[3]];
        fft_level_exp_alt_vlc.table_allocated = qdm2_vlc_offs[4] - qdm2_vlc_offs[3];
        init_vlc (&fft_level_exp_alt_vlc, 8, 28,
            fft_level_exp_alt_huffbits, 1, 1,
            fft_level_exp_alt_huffcodes, 2, 2, INIT_VLC_USE_NEW_STATIC | INIT_VLC_LE);


        fft_level_exp_vlc.table = &qdm2_table[qdm2_vlc_offs[4]];
        fft_level_exp_vlc.table_allocated = qdm2_vlc_offs[5] - qdm2_vlc_offs[4];
        init_vlc (&fft_level_exp_vlc, 8, 20,
            fft_level_exp_huffbits, 1, 1,
            fft_level_exp_huffcodes, 2, 2, INIT_VLC_USE_NEW_STATIC | INIT_VLC_LE);

        fft_stereo_exp_vlc.table = &qdm2_table[qdm2_vlc_offs[5]];
        fft_stereo_exp_vlc.table_allocated = qdm2_vlc_offs[6] - qdm2_vlc_offs[5];
        init_vlc (&fft_stereo_exp_vlc, 6, 7,
            fft_stereo_exp_huffbits, 1, 1,
            fft_stereo_exp_huffcodes, 1, 1, INIT_VLC_USE_NEW_STATIC | INIT_VLC_LE);

        fft_stereo_phase_vlc.table = &qdm2_table[qdm2_vlc_offs[6]];
        fft_stereo_phase_vlc.table_allocated = qdm2_vlc_offs[7] - qdm2_vlc_offs[6];
        init_vlc (&fft_stereo_phase_vlc, 6, 9,
            fft_stereo_phase_huffbits, 1, 1,
            fft_stereo_phase_huffcodes, 1, 1, INIT_VLC_USE_NEW_STATIC | INIT_VLC_LE);

        vlc_tab_tone_level_idx_hi1.table = &qdm2_table[qdm2_vlc_offs[7]];
        vlc_tab_tone_level_idx_hi1.table_allocated = qdm2_vlc_offs[8] - qdm2_vlc_offs[7];
        init_vlc (&vlc_tab_tone_level_idx_hi1, 8, 20,
            vlc_tab_tone_level_idx_hi1_huffbits, 1, 1,
            vlc_tab_tone_level_idx_hi1_huffcodes, 2, 2, INIT_VLC_USE_NEW_STATIC | INIT_VLC_LE);

        vlc_tab_tone_level_idx_mid.table = &qdm2_table[qdm2_vlc_offs[8]];
        vlc_tab_tone_level_idx_mid.table_allocated = qdm2_vlc_offs[9] - qdm2_vlc_offs[8];
        init_vlc (&vlc_tab_tone_level_idx_mid, 8, 24,
            vlc_tab_tone_level_idx_mid_huffbits, 1, 1,
            vlc_tab_tone_level_idx_mid_huffcodes, 2, 2, INIT_VLC_USE_NEW_STATIC | INIT_VLC_LE);

        vlc_tab_tone_level_idx_hi2.table = &qdm2_table[qdm2_vlc_offs[9]];
        vlc_tab_tone_level_idx_hi2.table_allocated = qdm2_vlc_offs[10] - qdm2_vlc_offs[9];
        init_vlc (&vlc_tab_tone_level_idx_hi2, 8, 24,
            vlc_tab_tone_level_idx_hi2_huffbits, 1, 1,
            vlc_tab_tone_level_idx_hi2_huffcodes, 2, 2, INIT_VLC_USE_NEW_STATIC | INIT_VLC_LE);

        vlc_tab_type30.table = &qdm2_table[qdm2_vlc_offs[10]];
        vlc_tab_type30.table_allocated = qdm2_vlc_offs[11] - qdm2_vlc_offs[10];
        init_vlc (&vlc_tab_type30, 6, 9,
            vlc_tab_type30_huffbits, 1, 1,
            vlc_tab_type30_huffcodes, 1, 1, INIT_VLC_USE_NEW_STATIC | INIT_VLC_LE);

        vlc_tab_type34.table = &qdm2_table[qdm2_vlc_offs[11]];
        vlc_tab_type34.table_allocated = qdm2_vlc_offs[12] - qdm2_vlc_offs[11];
        init_vlc (&vlc_tab_type34, 5, 10,
            vlc_tab_type34_huffbits, 1, 1,
            vlc_tab_type34_huffcodes, 1, 1, INIT_VLC_USE_NEW_STATIC | INIT_VLC_LE);

        vlc_tab_fft_tone_offset[0].table = &qdm2_table[qdm2_vlc_offs[12]];
        vlc_tab_fft_tone_offset[0].table_allocated = qdm2_vlc_offs[13] - qdm2_vlc_offs[12];
        init_vlc (&vlc_tab_fft_tone_offset[0], 8, 23,
            vlc_tab_fft_tone_offset_0_huffbits, 1, 1,
            vlc_tab_fft_tone_offset_0_huffcodes, 2, 2, INIT_VLC_USE_NEW_STATIC | INIT_VLC_LE);

        vlc_tab_fft_tone_offset[1].table = &qdm2_table[qdm2_vlc_offs[13]];
        vlc_tab_fft_tone_offset[1].table_allocated = qdm2_vlc_offs[14] - qdm2_vlc_offs[13];
        init_vlc (&vlc_tab_fft_tone_offset[1], 8, 28,
            vlc_tab_fft_tone_offset_1_huffbits, 1, 1,
            vlc_tab_fft_tone_offset_1_huffcodes, 2, 2, INIT_VLC_USE_NEW_STATIC | INIT_VLC_LE);

        vlc_tab_fft_tone_offset[2].table = &qdm2_table[qdm2_vlc_offs[14]];
        vlc_tab_fft_tone_offset[2].table_allocated = qdm2_vlc_offs[15] - qdm2_vlc_offs[14];
        init_vlc (&vlc_tab_fft_tone_offset[2], 8, 32,
            vlc_tab_fft_tone_offset_2_huffbits, 1, 1,
            vlc_tab_fft_tone_offset_2_huffcodes, 2, 2, INIT_VLC_USE_NEW_STATIC | INIT_VLC_LE);

        vlc_tab_fft_tone_offset[3].table = &qdm2_table[qdm2_vlc_offs[15]];
        vlc_tab_fft_tone_offset[3].table_allocated = qdm2_vlc_offs[16] - qdm2_vlc_offs[15];
        init_vlc (&vlc_tab_fft_tone_offset[3], 8, 35,
            vlc_tab_fft_tone_offset_3_huffbits, 1, 1,
            vlc_tab_fft_tone_offset_3_huffcodes, 2, 2, INIT_VLC_USE_NEW_STATIC | INIT_VLC_LE);

        vlc_tab_fft_tone_offset[4].table = &qdm2_table[qdm2_vlc_offs[16]];
        vlc_tab_fft_tone_offset[4].table_allocated = qdm2_vlc_offs[17] - qdm2_vlc_offs[16];
        init_vlc (&vlc_tab_fft_tone_offset[4], 8, 38,
            vlc_tab_fft_tone_offset_4_huffbits, 1, 1,
            vlc_tab_fft_tone_offset_4_huffcodes, 2, 2, INIT_VLC_USE_NEW_STATIC | INIT_VLC_LE);

        vlcs_initialized=1;
    }
}


/* for floating point to fixed point conversion */
static const float f2i_scale = (float) (1 << (FRAC_BITS - 15));


static int qdm2_get_vlc (GetBitContext *gb, VLC *vlc, int flag, int depth)
{
    int value;

    value = get_vlc2(gb, vlc->table, vlc->bits, depth);

    /* stage-2, 3 bits exponent escape sequence */
    if (value-- == 0)
        value = get_bits (gb, get_bits (gb, 3) + 1);

    /* stage-3, optional */
    if (flag) {
        int tmp = vlc_stage3_values[value];

        if ((value & ~3) > 0)
            tmp += get_bits (gb, (value >> 2));
        value = tmp;
    }

    return value;
}


static int qdm2_get_se_vlc (VLC *vlc, GetBitContext *gb, int depth)
{
    int value = qdm2_get_vlc (gb, vlc, 0, depth);

    return (value & 1) ? ((value + 1) >> 1) : -(value >> 1);
}


/**
 * QDM2 checksum
 *
 * @param data      pointer to data to be checksum'ed
 * @param length    data length
 * @param value     checksum value
 *
 * @return          0 if checksum is OK
 */
static uint16_t qdm2_packet_checksum (const uint8_t *data, int length, int value) {
    int i;

    for (i=0; i < length; i++)
        value -= data[i];

    return (uint16_t)(value & 0xffff);
}


/**
 * Fills a QDM2SubPacket structure with packet type, size, and data pointer.
 *
 * @param gb            bitreader context
 * @param sub_packet    packet under analysis
 */
static void qdm2_decode_sub_packet_header (GetBitContext *gb, QDM2SubPacket *sub_packet)
{
    sub_packet->type = get_bits (gb, 8);

    if (sub_packet->type == 0) {
        sub_packet->size = 0;
        sub_packet->data = NULL;
    } else {
        sub_packet->size = get_bits (gb, 8);

      if (sub_packet->type & 0x80) {
          sub_packet->size <<= 8;
          sub_packet->size  |= get_bits (gb, 8);
          sub_packet->type  &= 0x7f;
      }

      if (sub_packet->type == 0x7f)
          sub_packet->type |= (get_bits (gb, 8) << 8);

      sub_packet->data = &gb->buffer[get_bits_count(gb) / 8]; // FIXME: this depends on bitreader internal data
    }

    av_log(NULL,AV_LOG_DEBUG,"Subpacket: type=%d size=%d start_offs=%x\n",
        sub_packet->type, sub_packet->size, get_bits_count(gb) / 8);
}


/**
 * Return node pointer to first packet of requested type in list.
 *
 * @param list    list of subpackets to be scanned
 * @param type    type of searched subpacket
 * @return        node pointer for subpacket if found, else NULL
 */
static QDM2SubPNode* qdm2_search_subpacket_type_in_list (QDM2SubPNode *list, int type)
{
    while (list != NULL && list->packet != NULL) {
        if (list->packet->type == type)
            return list;
        list = list->next;
    }
    return NULL;
}


/**
 * Replaces 8 elements with their average value.
 * Called by qdm2_decode_superblock before starting subblock decoding.
 *
 * @param q       context
 */
static void average_quantized_coeffs (QDM2Context *q)
{
    int i, j, n, ch, sum;

    n = coeff_per_sb_for_avg[q->coeff_per_sb_select][QDM2_SB_USED(q->sub_sampling) - 1] + 1;

    for (ch = 0; ch < q->nb_channels; ch++)
        for (i = 0; i < n; i++) {
            sum = 0;

            for (j = 0; j < 8; j++)
                sum += q->quantized_coeffs[ch][i][j];

            sum /= 8;
            if (sum > 0)
                sum--;

            for (j=0; j < 8; j++)
                q->quantized_coeffs[ch][i][j] = sum;
        }
}


/**
 * Build subband samples with noise weighted by q->tone_level.
 * Called by synthfilt_build_sb_samples.
 *
 * @param q     context
 * @param sb    subband index
 */
static void build_sb_samples_from_noise (QDM2Context *q, int sb)
{
    int ch, j;

    FIX_NOISE_IDX(q->noise_idx);

    if (!q->nb_channels)
        return;

    for (ch = 0; ch < q->nb_channels; ch++)
        for (j = 0; j < 64; j++) {
            q->sb_samples[ch][j * 2][sb] = (int32_t)(f2i_scale * SB_DITHERING_NOISE(sb,q->noise_idx) * q->tone_level[ch][sb][j] + .5);
            q->sb_samples[ch][j * 2 + 1][sb] = (int32_t)(f2i_scale * SB_DITHERING_NOISE(sb,q->noise_idx) * q->tone_level[ch][sb][j] + .5);
        }
}


/**
 * Called while processing data from subpackets 11 and 12.
 * Used after making changes to coding_method array.
 *
 * @param sb               subband index
 * @param channels         number of channels
 * @param coding_method    q->coding_method[0][0][0]
 */
static void fix_coding_method_array (int sb, int channels, sb_int8_array coding_method)
{
    int j,k;
    int ch;
    int run, case_val;
    int switchtable[23] = {0,5,1,5,5,5,5,5,2,5,5,5,5,5,5,5,3,5,5,5,5,5,4};

    for (ch = 0; ch < channels; ch++) {
        for (j = 0; j < 64; ) {
            if((coding_method[ch][sb][j] - 8) > 22) {
                run = 1;
                case_val = 8;
            } else {
                switch (switchtable[coding_method[ch][sb][j]-8]) {
                    case 0: run = 10; case_val = 10; break;
                    case 1: run = 1; case_val = 16; break;
                    case 2: run = 5; case_val = 24; break;
                    case 3: run = 3; case_val = 30; break;
                    case 4: run = 1; case_val = 30; break;
                    case 5: run = 1; case_val = 8; break;
                    default: run = 1; case_val = 8; break;
                }
            }
            for (k = 0; k < run; k++)
                if (j + k < 128)
                    if (coding_method[ch][sb + (j + k) / 64][(j + k) % 64] > coding_method[ch][sb][j])
                        if (k > 0) {
                           SAMPLES_NEEDED
                            //not debugged, almost never used
                            memset(&coding_method[ch][sb][j + k], case_val, k * sizeof(int8_t));
                            memset(&coding_method[ch][sb][j + k], case_val, 3 * sizeof(int8_t));
                        }
            j += run;
        }
    }
}


/**
 * Related to synthesis filter
 * Called by process_subpacket_10
 *
 * @param q       context
 * @param flag    1 if called after getting data from subpacket 10, 0 if no subpacket 10
 */
static void fill_tone_level_array (QDM2Context *q, int flag)
{
    int i, sb, ch, sb_used;
    int tmp, tab;

    // This should never happen
    if (q->nb_channels <= 0)
        return;

    for (ch = 0; ch < q->nb_channels; ch++)
        for (sb = 0; sb < 30; sb++)
            for (i = 0; i < 8; i++) {
                if ((tab=coeff_per_sb_for_dequant[q->coeff_per_sb_select][sb]) < (last_coeff[q->coeff_per_sb_select] - 1))
                    tmp = q->quantized_coeffs[ch][tab + 1][i] * dequant_table[q->coeff_per_sb_select][tab + 1][sb]+
                          q->quantized_coeffs[ch][tab][i] * dequant_table[q->coeff_per_sb_select][tab][sb];
                else
                    tmp = q->quantized_coeffs[ch][tab][i] * dequant_table[q->coeff_per_sb_select][tab][sb];
                if(tmp < 0)
                    tmp += 0xff;
                q->tone_level_idx_base[ch][sb][i] = (tmp / 256) & 0xff;
            }

    sb_used = QDM2_SB_USED(q->sub_sampling);

    if ((q->superblocktype_2_3 != 0) && !flag) {
        for (sb = 0; sb < sb_used; sb++)
            for (ch = 0; ch < q->nb_channels; ch++)
                for (i = 0; i < 64; i++) {
                    q->tone_level_idx[ch][sb][i] = q->tone_level_idx_base[ch][sb][i / 8];
                    if (q->tone_level_idx[ch][sb][i] < 0)
                        q->tone_level[ch][sb][i] = 0;
                    else
                        q->tone_level[ch][sb][i] = fft_tone_level_table[0][q->tone_level_idx[ch][sb][i] & 0x3f];
                }
    } else {
        tab = q->superblocktype_2_3 ? 0 : 1;
        for (sb = 0; sb < sb_used; sb++) {
            if ((sb >= 4) && (sb <= 23)) {
                for (ch = 0; ch < q->nb_channels; ch++)
                    for (i = 0; i < 64; i++) {
                        tmp = q->tone_level_idx_base[ch][sb][i / 8] -
                              q->tone_level_idx_hi1[ch][sb / 8][i / 8][i % 8] -
                              q->tone_level_idx_mid[ch][sb - 4][i / 8] -
                              q->tone_level_idx_hi2[ch][sb - 4];
                        q->tone_level_idx[ch][sb][i] = tmp & 0xff;
                        if ((tmp < 0) || (!q->superblocktype_2_3 && !tmp))
                            q->tone_level[ch][sb][i] = 0;
                        else
                            q->tone_level[ch][sb][i] = fft_tone_level_table[tab][tmp & 0x3f];
                }
            } else {
                if (sb > 4) {
                    for (ch = 0; ch < q->nb_channels; ch++)
                        for (i = 0; i < 64; i++) {
                            tmp = q->tone_level_idx_base[ch][sb][i / 8] -
                                  q->tone_level_idx_hi1[ch][2][i / 8][i % 8] -
                                  q->tone_level_idx_hi2[ch][sb - 4];
                            q->tone_level_idx[ch][sb][i] = tmp & 0xff;
                            if ((tmp < 0) || (!q->superblocktype_2_3 && !tmp))
                                q->tone_level[ch][sb][i] = 0;
                            else
                                q->tone_level[ch][sb][i] = fft_tone_level_table[tab][tmp & 0x3f];
                    }
                } else {
                    for (ch = 0; ch < q->nb_channels; ch++)
                        for (i = 0; i < 64; i++) {
                            tmp = q->tone_level_idx[ch][sb][i] = q->tone_level_idx_base[ch][sb][i / 8];
                            if ((tmp < 0) || (!q->superblocktype_2_3 && !tmp))
                                q->tone_level[ch][sb][i] = 0;
                            else
                                q->tone_level[ch][sb][i] = fft_tone_level_table[tab][tmp & 0x3f];
                        }
                }
            }
        }
    }

    return;
}


/**
 * Related to synthesis filter
 * Called by process_subpacket_11
 * c is built with data from subpacket 11
 * Most of this function is used only if superblock_type_2_3 == 0, never seen it in samples
 *
 * @param tone_level_idx
 * @param tone_level_idx_temp
 * @param coding_method        q->coding_method[0][0][0]
 * @param nb_channels          number of channels
 * @param c                    coming from subpacket 11, passed as 8*c
 * @param superblocktype_2_3   flag based on superblock packet type
 * @param cm_table_select      q->cm_table_select
 */
static void fill_coding_method_array (sb_int8_array tone_level_idx, sb_int8_array tone_level_idx_temp,
                sb_int8_array coding_method, int nb_channels,
                int c, int superblocktype_2_3, int cm_table_select)
{
    int ch, sb, j;
    int tmp, acc, esp_40, comp;
    int add1, add2, add3, add4;
    int64_t multres;

    // This should never happen
    if (nb_channels <= 0)
        return;

    if (!superblocktype_2_3) {
        /* This case is untested, no samples available */
        SAMPLES_NEEDED
        for (ch = 0; ch < nb_channels; ch++)
            for (sb = 0; sb < 30; sb++) {
                for (j = 1; j < 63; j++) {  // The loop only iterates to 63 so the code doesn't overflow the buffer
                    add1 = tone_level_idx[ch][sb][j] - 10;
                    if (add1 < 0)
                        add1 = 0;
                    add2 = add3 = add4 = 0;
                    if (sb > 1) {
                        add2 = tone_level_idx[ch][sb - 2][j] + tone_level_idx_offset_table[sb][0] - 6;
                        if (add2 < 0)
                            add2 = 0;
                    }
                    if (sb > 0) {
                        add3 = tone_level_idx[ch][sb - 1][j] + tone_level_idx_offset_table[sb][1] - 6;
                        if (add3 < 0)
                            add3 = 0;
                    }
                    if (sb < 29) {
                        add4 = tone_level_idx[ch][sb + 1][j] + tone_level_idx_offset_table[sb][3] - 6;
                        if (add4 < 0)
                            add4 = 0;
                    }
                    tmp = tone_level_idx[ch][sb][j + 1] * 2 - add4 - add3 - add2 - add1;
                    if (tmp < 0)
                        tmp = 0;
                    tone_level_idx_temp[ch][sb][j + 1] = tmp & 0xff;
                }
                tone_level_idx_temp[ch][sb][0] = tone_level_idx_temp[ch][sb][1];
            }
            acc = 0;
            for (ch = 0; ch < nb_channels; ch++)
                for (sb = 0; sb < 30; sb++)
                    for (j = 0; j < 64; j++)
                        acc += tone_level_idx_temp[ch][sb][j];

            multres = 0x66666667 * (acc * 10);
            esp_40 = (multres >> 32) / 8 + ((multres & 0xffffffff) >> 31);
            for (ch = 0;  ch < nb_channels; ch++)
                for (sb = 0; sb < 30; sb++)
                    for (j = 0; j < 64; j++) {
                        comp = tone_level_idx_temp[ch][sb][j]* esp_40 * 10;
                        if (comp < 0)
                            comp += 0xff;
                        comp /= 256; // signed shift
                        switch(sb) {
                            case 0:
                                if (comp < 30)
                                    comp = 30;
                                comp += 15;
                                break;
                            case 1:
                                if (comp < 24)
                                    comp = 24;
                                comp += 10;
                                break;
                            case 2:
                            case 3:
                            case 4:
                                if (comp < 16)
                                    comp = 16;
                        }
                        if (comp <= 5)
                            tmp = 0;
                        else if (comp <= 10)
                            tmp = 10;
                        else if (comp <= 16)
                            tmp = 16;
                        else if (comp <= 24)
                            tmp = -1;
                        else
                            tmp = 0;
                        coding_method[ch][sb][j] = ((tmp & 0xfffa) + 30 )& 0xff;
                    }
            for (sb = 0; sb < 30; sb++)
                fix_coding_method_array(sb, nb_channels, coding_method);
            for (ch = 0; ch < nb_channels; ch++)
                for (sb = 0; sb < 30; sb++)
                    for (j = 0; j < 64; j++)
                        if (sb >= 10) {
                            if (coding_method[ch][sb][j] < 10)
                                coding_method[ch][sb][j] = 10;
                        } else {
                            if (sb >= 2) {
                                if (coding_method[ch][sb][j] < 16)
                                    coding_method[ch][sb][j] = 16;
                            } else {
                                if (coding_method[ch][sb][j] < 30)
                                    coding_method[ch][sb][j] = 30;
                            }
                        }
    } else { // superblocktype_2_3 != 0
        for (ch = 0; ch < nb_channels; ch++)
            for (sb = 0; sb < 30; sb++)
                for (j = 0; j < 64; j++)
                    coding_method[ch][sb][j] = coding_method_table[cm_table_select][sb];
    }

    return;
}


/**
 *
 * Called by process_subpacket_11 to process more data from subpacket 11 with sb 0-8
 * Called by process_subpacket_12 to process data from subpacket 12 with sb 8-sb_used
 *
 * @param q         context
 * @param gb        bitreader context
 * @param length    packet length in bits
 * @param sb_min    lower subband processed (sb_min included)
 * @param sb_max    higher subband processed (sb_max excluded)
 */
static void synthfilt_build_sb_samples (QDM2Context *q, GetBitContext *gb, int length, int sb_min, int sb_max)
{
    int sb, j, k, n, ch, run, channels;
    int joined_stereo, zero_encoding, chs;
    int type34_first;
    float type34_div = 0;
    float type34_predictor;
    float samples[10], sign_bits[16];

    if (length == 0) {
        // If no data use noise
        for (sb=sb_min; sb < sb_max; sb++)
            build_sb_samples_from_noise (q, sb);

        return;
    }

    for (sb = sb_min; sb < sb_max; sb++) {
        FIX_NOISE_IDX(q->noise_idx);

        channels = q->nb_channels;

        if (q->nb_channels <= 1 || sb < 12)
            joined_stereo = 0;
        else if (sb >= 24)
            joined_stereo = 1;
        else
            joined_stereo = (BITS_LEFT(length,gb) >= 1) ? get_bits1 (gb) : 0;

        if (joined_stereo) {
            if (BITS_LEFT(length,gb) >= 16)
                for (j = 0; j < 16; j++)
                    sign_bits[j] = get_bits1 (gb);

            for (j = 0; j < 64; j++)
                if (q->coding_method[1][sb][j] > q->coding_method[0][sb][j])
                    q->coding_method[0][sb][j] = q->coding_method[1][sb][j];

            fix_coding_method_array(sb, q->nb_channels, q->coding_method);
            channels = 1;
        }

        for (ch = 0; ch < channels; ch++) {
            zero_encoding = (BITS_LEFT(length,gb) >= 1) ? get_bits1(gb) : 0;
            type34_predictor = 0.0;
            type34_first = 1;

            for (j = 0; j < 128; ) {
                switch (q->coding_method[ch][sb][j / 2]) {
                    case 8:
                        if (BITS_LEFT(length,gb) >= 10) {
                            if (zero_encoding) {
                                for (k = 0; k < 5; k++) {
                                    if ((j + 2 * k) >= 128)
                                        break;
                                    samples[2 * k] = get_bits1(gb) ? dequant_1bit[joined_stereo][2 * get_bits1(gb)] : 0;
                                }
                            } else {
                                n = get_bits(gb, 8);
                                for (k = 0; k < 5; k++)
                                    samples[2 * k] = dequant_1bit[joined_stereo][random_dequant_index[n][k]];
                            }
                            for (k = 0; k < 5; k++)
                                samples[2 * k + 1] = SB_DITHERING_NOISE(sb,q->noise_idx);
                        } else {
                            for (k = 0; k < 10; k++)
                                samples[k] = SB_DITHERING_NOISE(sb,q->noise_idx);
                        }
                        run = 10;
                        break;

                    case 10:
                        if (BITS_LEFT(length,gb) >= 1) {
                            float f = 0.81;

                            if (get_bits1(gb))
                                f = -f;
                            f -= noise_samples[((sb + 1) * (j +5 * ch + 1)) & 127] * 9.0 / 40.0;
                            samples[0] = f;
                        } else {
                            samples[0] = SB_DITHERING_NOISE(sb,q->noise_idx);
                        }
                        run = 1;
                        break;

                    case 16:
                        if (BITS_LEFT(length,gb) >= 10) {
                            if (zero_encoding) {
                                for (k = 0; k < 5; k++) {
                                    if ((j + k) >= 128)
                                        break;
                                    samples[k] = (get_bits1(gb) == 0) ? 0 : dequant_1bit[joined_stereo][2 * get_bits1(gb)];
                                }
                            } else {
                                n = get_bits (gb, 8);
                                for (k = 0; k < 5; k++)
                                    samples[k] = dequant_1bit[joined_stereo][random_dequant_index[n][k]];
                            }
                        } else {
                            for (k = 0; k < 5; k++)
                                samples[k] = SB_DITHERING_NOISE(sb,q->noise_idx);
                        }
                        run = 5;
                        break;

                    case 24:
                        if (BITS_LEFT(length,gb) >= 7) {
                            n = get_bits(gb, 7);
                            for (k = 0; k < 3; k++)
                                samples[k] = (random_dequant_type24[n][k] - 2.0) * 0.5;
                        } else {
                            for (k = 0; k < 3; k++)
                                samples[k] = SB_DITHERING_NOISE(sb,q->noise_idx);
                        }
                        run = 3;
                        break;

                    case 30:
                        if (BITS_LEFT(length,gb) >= 4)
                            samples[0] = type30_dequant[qdm2_get_vlc(gb, &vlc_tab_type30, 0, 1)];
                        else
                            samples[0] = SB_DITHERING_NOISE(sb,q->noise_idx);

                        run = 1;
                        break;

                    case 34:
                        if (BITS_LEFT(length,gb) >= 7) {
                            if (type34_first) {
                                type34_div = (float)(1 << get_bits(gb, 2));
                                samples[0] = ((float)get_bits(gb, 5) - 16.0) / 15.0;
                                type34_predictor = samples[0];
                                type34_first = 0;
                            } else {
                                samples[0] = type34_delta[qdm2_get_vlc(gb, &vlc_tab_type34, 0, 1)] / type34_div + type34_predictor;
                                type34_predictor = samples[0];
                            }
                        } else {
                            samples[0] = SB_DITHERING_NOISE(sb,q->noise_idx);
                        }
                        run = 1;
                        break;

                    default:
                        samples[0] = SB_DITHERING_NOISE(sb,q->noise_idx);
                        run = 1;
                        break;
                }

                if (joined_stereo) {
                    float tmp[10][MPA_MAX_CHANNELS];

                    for (k = 0; k < run; k++) {
                        tmp[k][0] = samples[k];
                        tmp[k][1] = (sign_bits[(j + k) / 8]) ? -samples[k] : samples[k];
                    }
                    for (chs = 0; chs < q->nb_channels; chs++)
                        for (k = 0; k < run; k++)
                            if ((j + k) < 128)
                                q->sb_samples[chs][j + k][sb] = (int32_t)(f2i_scale * q->tone_level[chs][sb][((j + k)/2)] * tmp[k][chs] + .5);
                } else {
                    for (k = 0; k < run; k++)
                        if ((j + k) < 128)
                            q->sb_samples[ch][j + k][sb] = (int32_t)(f2i_scale * q->tone_level[ch][sb][(j + k)/2] * samples[k] + .5);
                }

                j += run;
            } // j loop
        } // channel loop
    } // subband loop
}


/**
 * Init the first element of a channel in quantized_coeffs with data from packet 10 (quantized_coeffs[ch][0]).
 * This is similar to process_subpacket_9, but for a single channel and for element [0]
 * same VLC tables as process_subpacket_9 are used.
 *
 * @param q         context
 * @param quantized_coeffs    pointer to quantized_coeffs[ch][0]
 * @param gb        bitreader context
 * @param length    packet length in bits
 */
static void init_quantized_coeffs_elem0 (int8_t *quantized_coeffs, GetBitContext *gb, int length)
{
    int i, k, run, level, diff;

    if (BITS_LEFT(length,gb) < 16)
        return;
    level = qdm2_get_vlc(gb, &vlc_tab_level, 0, 2);

    quantized_coeffs[0] = level;

    for (i = 0; i < 7; ) {
        if (BITS_LEFT(length,gb) < 16)
            break;
        run = qdm2_get_vlc(gb, &vlc_tab_run, 0, 1) + 1;

        if (BITS_LEFT(length,gb) < 16)
            break;
        diff = qdm2_get_se_vlc(&vlc_tab_diff, gb, 2);

        for (k = 1; k <= run; k++)
            quantized_coeffs[i + k] = (level + ((k * diff) / run));

        level += diff;
        i += run;
    }
}


/**
 * Related to synthesis filter, process data from packet 10
 * Init part of quantized_coeffs via function init_quantized_coeffs_elem0
 * Init tone_level_idx_hi1, tone_level_idx_hi2, tone_level_idx_mid with data from packet 10
 *
 * @param q         context
 * @param gb        bitreader context
 * @param length    packet length in bits
 */
static void init_tone_level_dequantization (QDM2Context *q, GetBitContext *gb, int length)
{
    int sb, j, k, n, ch;

    for (ch = 0; ch < q->nb_channels; ch++) {
        init_quantized_coeffs_elem0(q->quantized_coeffs[ch][0], gb, length);

        if (BITS_LEFT(length,gb) < 16) {
            memset(q->quantized_coeffs[ch][0], 0, 8);
            break;
        }
    }

    n = q->sub_sampling + 1;

    for (sb = 0; sb < n; sb++)
        for (ch = 0; ch < q->nb_channels; ch++)
            for (j = 0; j < 8; j++) {
                if (BITS_LEFT(length,gb) < 1)
                    break;
                if (get_bits1(gb)) {
                    for (k=0; k < 8; k++) {
                        if (BITS_LEFT(length,gb) < 16)
                            break;
                        q->tone_level_idx_hi1[ch][sb][j][k] = qdm2_get_vlc(gb, &vlc_tab_tone_level_idx_hi1, 0, 2);
                    }
                } else {
                    for (k=0; k < 8; k++)
                        q->tone_level_idx_hi1[ch][sb][j][k] = 0;
                }
            }

    n = QDM2_SB_USED(q->sub_sampling) - 4;

    for (sb = 0; sb < n; sb++)
        for (ch = 0; ch < q->nb_channels; ch++) {
            if (BITS_LEFT(length,gb) < 16)
                break;
            q->tone_level_idx_hi2[ch][sb] = qdm2_get_vlc(gb, &vlc_tab_tone_level_idx_hi2, 0, 2);
            if (sb > 19)
                q->tone_level_idx_hi2[ch][sb] -= 16;
            else
                for (j = 0; j < 8; j++)
                    q->tone_level_idx_mid[ch][sb][j] = -16;
        }

    n = QDM2_SB_USED(q->sub_sampling) - 5;

    for (sb = 0; sb < n; sb++)
        for (ch = 0; ch < q->nb_channels; ch++)
            for (j = 0; j < 8; j++) {
                if (BITS_LEFT(length,gb) < 16)
                    break;
                q->tone_level_idx_mid[ch][sb][j] = qdm2_get_vlc(gb, &vlc_tab_tone_level_idx_mid, 0, 2) - 32;
            }
}

/**
 * Process subpacket 9, init quantized_coeffs with data from it
 *
 * @param q       context
 * @param node    pointer to node with packet
 */
static void process_subpacket_9 (QDM2Context *q, QDM2SubPNode *node)
{
    GetBitContext gb;
    int i, j, k, n, ch, run, level, diff;

    init_get_bits(&gb, node->packet->data, node->packet->size*8);

    n = coeff_per_sb_for_avg[q->coeff_per_sb_select][QDM2_SB_USED(q->sub_sampling) - 1] + 1; // same as averagesomething function

    for (i = 1; i < n; i++)
        for (ch=0; ch < q->nb_channels; ch++) {
            level = qdm2_get_vlc(&gb, &vlc_tab_level, 0, 2);
            q->quantized_coeffs[ch][i][0] = level;

            for (j = 0; j < (8 - 1); ) {
                run = qdm2_get_vlc(&gb, &vlc_tab_run, 0, 1) + 1;
                diff = qdm2_get_se_vlc(&vlc_tab_diff, &gb, 2);

                for (k = 1; k <= run; k++)
                    q->quantized_coeffs[ch][i][j + k] = (level + ((k*diff) / run));

                level += diff;
                j += run;
            }
        }

    for (ch = 0; ch < q->nb_channels; ch++)
        for (i = 0; i < 8; i++)
            q->quantized_coeffs[ch][0][i] = 0;
}


/**
 * Process subpacket 10 if not null, else
 *
 * @param q         context
 * @param node      pointer to node with packet
 * @param length    packet length in bits
 */
static void process_subpacket_10 (QDM2Context *q, QDM2SubPNode *node, int length)
{
    GetBitContext gb;

    init_get_bits(&gb, ((node == NULL) ? empty_buffer : node->packet->data), ((node == NULL) ? 0 : node->packet->size*8));

    if (length != 0) {
        init_tone_level_dequantization(q, &gb, length);
        fill_tone_level_array(q, 1);
    } else {
        fill_tone_level_array(q, 0);
    }
}


/**
 * Process subpacket 11
 *
 * @param q         context
 * @param node      pointer to node with packet
 * @param length    packet length in bit
 */
static void process_subpacket_11 (QDM2Context *q, QDM2SubPNode *node, int length)
{
    GetBitContext gb;

    init_get_bits(&gb, ((node == NULL) ? empty_buffer : node->packet->data), ((node == NULL) ? 0 : node->packet->size*8));
    if (length >= 32) {
        int c = get_bits (&gb, 13);

        if (c > 3)
            fill_coding_method_array (q->tone_level_idx, q->tone_level_idx_temp, q->coding_method,
                                      q->nb_channels, 8*c, q->superblocktype_2_3, q->cm_table_select);
    }

    synthfilt_build_sb_samples(q, &gb, length, 0, 8);
}


/**
 * Process subpacket 12
 *
 * @param q         context
 * @param node      pointer to node with packet
 * @param length    packet length in bits
 */
static void process_subpacket_12 (QDM2Context *q, QDM2SubPNode *node, int length)
{
    GetBitContext gb;

    init_get_bits(&gb, ((node == NULL) ? empty_buffer : node->packet->data), ((node == NULL) ? 0 : node->packet->size*8));
    synthfilt_build_sb_samples(q, &gb, length, 8, QDM2_SB_USED(q->sub_sampling));
}

/*
 * Process new subpackets for synthesis filter
 *
 * @param q       context
 * @param list    list with synthesis filter packets (list D)
 */
static void process_synthesis_subpackets (QDM2Context *q, QDM2SubPNode *list)
{
    QDM2SubPNode *nodes[4];

    nodes[0] = qdm2_search_subpacket_type_in_list(list, 9);
    if (nodes[0] != NULL)
        process_subpacket_9(q, nodes[0]);

    nodes[1] = qdm2_search_subpacket_type_in_list(list, 10);
    if (nodes[1] != NULL)
        process_subpacket_10(q, nodes[1], nodes[1]->packet->size << 3);
    else
        process_subpacket_10(q, NULL, 0);

    nodes[2] = qdm2_search_subpacket_type_in_list(list, 11);
    if (nodes[0] != NULL && nodes[1] != NULL && nodes[2] != NULL)
        process_subpacket_11(q, nodes[2], (nodes[2]->packet->size << 3));
    else
        process_subpacket_11(q, NULL, 0);

    nodes[3] = qdm2_search_subpacket_type_in_list(list, 12);
    if (nodes[0] != NULL && nodes[1] != NULL && nodes[3] != NULL)
        process_subpacket_12(q, nodes[3], (nodes[3]->packet->size << 3));
    else
        process_subpacket_12(q, NULL, 0);
}


/*
 * Decode superblock, fill packet lists.
 *
 * @param q    context
 */
static void qdm2_decode_super_block (QDM2Context *q)
{
    GetBitContext gb;
    QDM2SubPacket header, *packet;
    int i, packet_bytes, sub_packet_size, sub_packets_D;
    unsigned int next_index = 0;

    memset(q->tone_level_idx_hi1, 0, sizeof(q->tone_level_idx_hi1));
    memset(q->tone_level_idx_mid, 0, sizeof(q->tone_level_idx_mid));
    memset(q->tone_level_idx_hi2, 0, sizeof(q->tone_level_idx_hi2));

    q->sub_packets_B = 0;
    sub_packets_D = 0;

    average_quantized_coeffs(q); // average elements in quantized_coeffs[max_ch][10][8]

    init_get_bits(&gb, q->compressed_data, q->compressed_size*8);
    qdm2_decode_sub_packet_header(&gb, &header);

    if (header.type < 2 || header.type >= 8) {
        q->has_errors = 1;
        av_log(NULL,AV_LOG_ERROR,"bad superblock type\n");
        return;
    }

    q->superblocktype_2_3 = (header.type == 2 || header.type == 3);
    packet_bytes = (q->compressed_size - get_bits_count(&gb) / 8);

    init_get_bits(&gb, header.data, header.size*8);

    if (header.type == 2 || header.type == 4 || header.type == 5) {
        int csum = 257 * get_bits(&gb, 8) + 2 * get_bits(&gb, 8);

        csum = qdm2_packet_checksum(q->compressed_data, q->checksum_size, csum);

        if (csum != 0) {
            q->has_errors = 1;
            av_log(NULL,AV_LOG_ERROR,"bad packet checksum\n");
            return;
        }
    }

    q->sub_packet_list_B[0].packet = NULL;
    q->sub_packet_list_D[0].packet = NULL;

    for (i = 0; i < 6; i++)
        if (--q->fft_level_exp[i] < 0)
            q->fft_level_exp[i] = 0;

    for (i = 0; packet_bytes > 0; i++) {
        int j;

        q->sub_packet_list_A[i].next = NULL;

        if (i > 0) {
            q->sub_packet_list_A[i - 1].next = &q->sub_packet_list_A[i];

            /* seek to next block */
            init_get_bits(&gb, header.data, header.size*8);
            skip_bits(&gb, next_index*8);

            if (next_index >= header.size)
                break;
        }

        /* decode subpacket */
        packet = &q->sub_packets[i];
        qdm2_decode_sub_packet_header(&gb, packet);
        next_index = packet->size + get_bits_count(&gb) / 8;
        sub_packet_size = ((packet->size > 0xff) ? 1 : 0) + packet->size + 2;

        if (packet->type == 0)
            break;

        if (sub_packet_size > packet_bytes) {
            if (packet->type != 10 && packet->type != 11 && packet->type != 12)
                break;
            packet->size += packet_bytes - sub_packet_size;
        }

        packet_bytes -= sub_packet_size;

        /* add subpacket to 'all subpackets' list */
        q->sub_packet_list_A[i].packet = packet;

        /* add subpacket to related list */
        if (packet->type == 8) {
            SAMPLES_NEEDED_2("packet type 8");
            return;
        } else if (packet->type >= 9 && packet->type <= 12) {
            /* packets for MPEG Audio like Synthesis Filter */
            QDM2_LIST_ADD(q->sub_packet_list_D, sub_packets_D, packet);
        } else if (packet->type == 13) {
            for (j = 0; j < 6; j++)
                q->fft_level_exp[j] = get_bits(&gb, 6);
        } else if (packet->type == 14) {
            for (j = 0; j < 6; j++)
                q->fft_level_exp[j] = qdm2_get_vlc(&gb, &fft_level_exp_vlc, 0, 2);
        } else if (packet->type == 15) {
            SAMPLES_NEEDED_2("packet type 15")
            return;
        } else if (packet->type >= 16 && packet->type < 48 && !fft_subpackets[packet->type - 16]) {
            /* packets for FFT */
            QDM2_LIST_ADD(q->sub_packet_list_B, q->sub_packets_B, packet);
        }
    } // Packet bytes loop

/* **************************************************************** */
    if (q->sub_packet_list_D[0].packet != NULL) {
        process_synthesis_subpackets(q, q->sub_packet_list_D);
        q->do_synth_filter = 1;
    } else if (q->do_synth_filter) {
        process_subpacket_10(q, NULL, 0);
        process_subpacket_11(q, NULL, 0);
        process_subpacket_12(q, NULL, 0);
    }
/* **************************************************************** */
}


static void qdm2_fft_init_coefficient (QDM2Context *q, int sub_packet,
                       int offset, int duration, int channel,
                       int exp, int phase)
{
    if (q->fft_coefs_min_index[duration] < 0)
        q->fft_coefs_min_index[duration] = q->fft_coefs_index;

    q->fft_coefs[q->fft_coefs_index].sub_packet = ((sub_packet >= 16) ? (sub_packet - 16) : sub_packet);
    q->fft_coefs[q->fft_coefs_index].channel = channel;
    q->fft_coefs[q->fft_coefs_index].offset = offset;
    q->fft_coefs[q->fft_coefs_index].exp = exp;
    q->fft_coefs[q->fft_coefs_index].phase = phase;
    q->fft_coefs_index++;
}


static void qdm2_fft_decode_tones (QDM2Context *q, int duration, GetBitContext *gb, int b)
{
    int channel, stereo, phase, exp;
    int local_int_4,  local_int_8,  stereo_phase,  local_int_10;
    int local_int_14, stereo_exp, local_int_20, local_int_28;
    int n, offset;

    local_int_4 = 0;
    local_int_28 = 0;
    local_int_20 = 2;
    local_int_8 = (4 - duration);
    local_int_10 = 1 << (q->group_order - duration - 1);
    offset = 1;

    while (1) {
        if (q->superblocktype_2_3) {
            while ((n = qdm2_get_vlc(gb, &vlc_tab_fft_tone_offset[local_int_8], 1, 2)) < 2) {
                offset = 1;
                if (n == 0) {
                    local_int_4 += local_int_10;
                    local_int_28 += (1 << local_int_8);
                } else {
                    local_int_4 += 8*local_int_10;
                    local_int_28 += (8 << local_int_8);
                }
            }
            offset += (n - 2);
        } else {
            offset += qdm2_get_vlc(gb, &vlc_tab_fft_tone_offset[local_int_8], 1, 2);
            while (offset >= (local_int_10 - 1)) {
                offset += (1 - (local_int_10 - 1));
                local_int_4  += local_int_10;
                local_int_28 += (1 << local_int_8);
            }
        }

        if (local_int_4 >= q->group_size)
            return;

        local_int_14 = (offset >> local_int_8);

        if (q->nb_channels > 1) {
            channel = get_bits1(gb);
            stereo = get_bits1(gb);
        } else {
            channel = 0;
            stereo = 0;
        }

        exp = qdm2_get_vlc(gb, (b ? &fft_level_exp_vlc : &fft_level_exp_alt_vlc), 0, 2);
        exp += q->fft_level_exp[fft_level_index_table[local_int_14]];
        exp = (exp < 0) ? 0 : exp;

        phase = get_bits(gb, 3);
        stereo_exp = 0;
        stereo_phase = 0;

        if (stereo) {
            stereo_exp = (exp - qdm2_get_vlc(gb, &fft_stereo_exp_vlc, 0, 1));
            stereo_phase = (phase - qdm2_get_vlc(gb, &fft_stereo_phase_vlc, 0, 1));
            if (stereo_phase < 0)
                stereo_phase += 8;
        }

        if (q->frequency_range > (local_int_14 + 1)) {
            int sub_packet = (local_int_20 + local_int_28);

            qdm2_fft_init_coefficient(q, sub_packet, offset, duration, channel, exp, phase);
            if (stereo)
                qdm2_fft_init_coefficient(q, sub_packet, offset, duration, (1 - channel), stereo_exp, stereo_phase);
        }

        offset++;
    }
}


static void qdm2_decode_fft_packets (QDM2Context *q)
{
    int i, j, min, max, value, type, unknown_flag;
    GetBitContext gb;

    if (q->sub_packet_list_B[0].packet == NULL)
        return;

    /* reset minimum indexes for FFT coefficients */
    q->fft_coefs_index = 0;
    for (i=0; i < 5; i++)
        q->fft_coefs_min_index[i] = -1;

    /* process subpackets ordered by type, largest type first */
    for (i = 0, max = 256; i < q->sub_packets_B; i++) {
        QDM2SubPacket *packet= NULL;

        /* find subpacket with largest type less than max */
        for (j = 0, min = 0; j < q->sub_packets_B; j++) {
            value = q->sub_packet_list_B[j].packet->type;
            if (value > min && value < max) {
                min = value;
                packet = q->sub_packet_list_B[j].packet;
            }
        }

        max = min;

        /* check for errors (?) */
        if (!packet)
            return;

        if (i == 0 && (packet->type < 16 || packet->type >= 48 || fft_subpackets[packet->type - 16]))
            return;

        /* decode FFT tones */
        init_get_bits (&gb, packet->data, packet->size*8);

        if (packet->type >= 32 && packet->type < 48 && !fft_subpackets[packet->type - 16])
            unknown_flag = 1;
        else
            unknown_flag = 0;

        type = packet->type;

        if ((type >= 17 && type < 24) || (type >= 33 && type < 40)) {
            int duration = q->sub_sampling + 5 - (type & 15);

            if (duration >= 0 && duration < 4)
                qdm2_fft_decode_tones(q, duration, &gb, unknown_flag);
        } else if (type == 31) {
            for (j=0; j < 4; j++)
                qdm2_fft_decode_tones(q, j, &gb, unknown_flag);
        } else if (type == 46) {
            for (j=0; j < 6; j++)
                q->fft_level_exp[j] = get_bits(&gb, 6);
            for (j=0; j < 4; j++)
            qdm2_fft_decode_tones(q, j, &gb, unknown_flag);
        }
    } // Loop on B packets

    /* calculate maximum indexes for FFT coefficients */
    for (i = 0, j = -1; i < 5; i++)
        if (q->fft_coefs_min_index[i] >= 0) {
            if (j >= 0)
                q->fft_coefs_max_index[j] = q->fft_coefs_min_index[i];
            j = i;
        }
    if (j >= 0)
        q->fft_coefs_max_index[j] = q->fft_coefs_index;
}


static void qdm2_fft_generate_tone (QDM2Context *q, FFTTone *tone)
{
   float level, f[6];
   int i;
   QDM2Complex c;
   const double iscale = 2.0*M_PI / 512.0;

    tone->phase += tone->phase_shift;

    /* calculate current level (maximum amplitude) of tone */
    level = fft_tone_envelope_table[tone->duration][tone->time_index] * tone->level;
    c.im = level * sin(tone->phase*iscale);
    c.re = level * cos(tone->phase*iscale);

    /* generate FFT coefficients for tone */
    if (tone->duration >= 3 || tone->cutoff >= 3) {
        tone->complex[0].im += c.im;
        tone->complex[0].re += c.re;
        tone->complex[1].im -= c.im;
        tone->complex[1].re -= c.re;
    } else {
        f[1] = -tone->table[4];
        f[0] =  tone->table[3] - tone->table[0];
        f[2] =  1.0 - tone->table[2] - tone->table[3];
        f[3] =  tone->table[1] + tone->table[4] - 1.0;
        f[4] =  tone->table[0] - tone->table[1];
        f[5] =  tone->table[2];
        for (i = 0; i < 2; i++) {
            tone->complex[fft_cutoff_index_table[tone->cutoff][i]].re += c.re * f[i];
            tone->complex[fft_cutoff_index_table[tone->cutoff][i]].im += c.im *((tone->cutoff <= i) ? -f[i] : f[i]);
        }
        for (i = 0; i < 4; i++) {
            tone->complex[i].re += c.re * f[i+2];
            tone->complex[i].im += c.im * f[i+2];
        }
    }

    /* copy the tone if it has not yet died out */
    if (++tone->time_index < ((1 << (5 - tone->duration)) - 1)) {
      memcpy(&q->fft_tones[q->fft_tone_end], tone, sizeof(FFTTone));
      q->fft_tone_end = (q->fft_tone_end + 1) % 1000;
    }
}


static void qdm2_fft_tone_synthesizer (QDM2Context *q, int sub_packet)
{
    int i, j, ch;
    const double iscale = 0.25 * M_PI;

    for (ch = 0; ch < q->channels; ch++) {
        memset(q->fft.complex[ch], 0, q->fft_size * sizeof(QDM2Complex));
    }


    /* apply FFT tones with duration 4 (1 FFT period) */
    if (q->fft_coefs_min_index[4] >= 0)
        for (i = q->fft_coefs_min_index[4]; i < q->fft_coefs_max_index[4]; i++) {
            float level;
            QDM2Complex c;

            if (q->fft_coefs[i].sub_packet != sub_packet)
                break;

            ch = (q->channels == 1) ? 0 : q->fft_coefs[i].channel;
            level = (q->fft_coefs[i].exp < 0) ? 0.0 : fft_tone_level_table[q->superblocktype_2_3 ? 0 : 1][q->fft_coefs[i].exp & 63];

            c.re = level * cos(q->fft_coefs[i].phase * iscale);
            c.im = level * sin(q->fft_coefs[i].phase * iscale);
            q->fft.complex[ch][q->fft_coefs[i].offset + 0].re += c.re;
            q->fft.complex[ch][q->fft_coefs[i].offset + 0].im += c.im;
            q->fft.complex[ch][q->fft_coefs[i].offset + 1].re -= c.re;
            q->fft.complex[ch][q->fft_coefs[i].offset + 1].im -= c.im;
        }

    /* generate existing FFT tones */
    for (i = q->fft_tone_end; i != q->fft_tone_start; ) {
        qdm2_fft_generate_tone(q, &q->fft_tones[q->fft_tone_start]);
        q->fft_tone_start = (q->fft_tone_start + 1) % 1000;
    }

    /* create and generate new FFT tones with duration 0 (long) to 3 (short) */
    for (i = 0; i < 4; i++)
        if (q->fft_coefs_min_index[i] >= 0) {
            for (j = q->fft_coefs_min_index[i]; j < q->fft_coefs_max_index[i]; j++) {
                int offset, four_i;
                FFTTone tone;

                if (q->fft_coefs[j].sub_packet != sub_packet)
                    break;

                four_i = (4 - i);
                offset = q->fft_coefs[j].offset >> four_i;
                ch = (q->channels == 1) ? 0 : q->fft_coefs[j].channel;

                if (offset < q->frequency_range) {
                    if (offset < 2)
                        tone.cutoff = offset;
                    else
                        tone.cutoff = (offset >= 60) ? 3 : 2;

                    tone.level = (q->fft_coefs[j].exp < 0) ? 0.0 : fft_tone_level_table[q->superblocktype_2_3 ? 0 : 1][q->fft_coefs[j].exp & 63];
                    tone.complex = &q->fft.complex[ch][offset];
                    tone.table = fft_tone_sample_table[i][q->fft_coefs[j].offset - (offset << four_i)];
                    tone.phase = 64 * q->fft_coefs[j].phase - (offset << 8) - 128;
                    tone.phase_shift = (2 * q->fft_coefs[j].offset + 1) << (7 - four_i);
                    tone.duration = i;
                    tone.time_index = 0;

                    qdm2_fft_generate_tone(q, &tone);
                }
            }
            q->fft_coefs_min_index[i] = j;
        }
}


static void qdm2_calculate_fft (QDM2Context *q, int channel, int sub_packet)
{
    const float gain = (q->channels == 1 && q->nb_channels == 2) ? 0.5f : 1.0f;
    int i;
    q->fft.complex[channel][0].re *= 2.0f;
    q->fft.complex[channel][0].im = 0.0f;
    ff_rdft_calc(&q->rdft_ctx, (FFTSample *)q->fft.complex[channel]);
    /* add samples to output buffer */
    for (i = 0; i < ((q->fft_frame_size + 15) & ~15); i++)
        q->output_buffer[q->channels * i + channel] += ((float *) q->fft.complex[channel])[i] * gain;
}


/**
 * @param q        context
 * @param index    subpacket number
 */
static void qdm2_synthesis_filter (QDM2Context *q, int index)
{
    OUT_INT samples[MPA_MAX_CHANNELS * MPA_FRAME_SIZE];
    int i, k, ch, sb_used, sub_sampling, dither_state = 0;

    /* copy sb_samples */
    sb_used = QDM2_SB_USED(q->sub_sampling);

    for (ch = 0; ch < q->channels; ch++)
        for (i = 0; i < 8; i++)
            for (k=sb_used; k < SBLIMIT; k++)
                q->sb_samples[ch][(8 * index) + i][k] = 0;

    for (ch = 0; ch < q->nb_channels; ch++) {
        OUT_INT *samples_ptr = samples + ch;

        for (i = 0; i < 8; i++) {
            ff_mpa_synth_filter(q->synth_buf[ch], &(q->synth_buf_offset[ch]),
                ff_mpa_synth_window, &dither_state,
                samples_ptr, q->nb_channels,
                q->sb_samples[ch][(8 * index) + i]);
            samples_ptr += 32 * q->nb_channels;
        }
    }

    /* add samples to output buffer */
    sub_sampling = (4 >> q->sub_sampling);

    for (ch = 0; ch < q->channels; ch++)
        for (i = 0; i < q->frame_size; i++)
            q->output_buffer[q->channels * i + ch] += (float)(samples[q->nb_channels * sub_sampling * i + ch] >> (sizeof(OUT_INT)*8-16));
}


/**
 * Init static data (does not depend on specific file)
 *
 * @param q    context
 */
static av_cold void qdm2_init(QDM2Context *q) {
    static int initialized = 0;

    if (initialized != 0)
        return;
    initialized = 1;

    qdm2_init_vlc();
    ff_mpa_synth_init(ff_mpa_synth_window);
    softclip_table_init();
    rnd_table_init();
    init_noise_samples();

    av_log(NULL, AV_LOG_DEBUG, "init done\n");
}


#if 0
static void dump_context(QDM2Context *q)
{
    int i;
#define PRINT(a,b) av_log(NULL,AV_LOG_DEBUG," %s = %d\n", a, b);
    PRINT("compressed_data",q->compressed_data);
    PRINT("compressed_size",q->compressed_size);
    PRINT("frame_size",q->frame_size);
    PRINT("checksum_size",q->checksum_size);
    PRINT("channels",q->channels);
    PRINT("nb_channels",q->nb_channels);
    PRINT("fft_frame_size",q->fft_frame_size);
    PRINT("fft_size",q->fft_size);
    PRINT("sub_sampling",q->sub_sampling);
    PRINT("fft_order",q->fft_order);
    PRINT("group_order",q->group_order);
    PRINT("group_size",q->group_size);
    PRINT("sub_packet",q->sub_packet);
    PRINT("frequency_range",q->frequency_range);
    PRINT("has_errors",q->has_errors);
    PRINT("fft_tone_end",q->fft_tone_end);
    PRINT("fft_tone_start",q->fft_tone_start);
    PRINT("fft_coefs_index",q->fft_coefs_index);
    PRINT("coeff_per_sb_select",q->coeff_per_sb_select);
    PRINT("cm_table_select",q->cm_table_select);
    PRINT("noise_idx",q->noise_idx);

    for (i = q->fft_tone_start; i < q->fft_tone_end; i++)
    {
    FFTTone *t = &q->fft_tones[i];

    av_log(NULL,AV_LOG_DEBUG,"Tone (%d) dump:\n", i);
    av_log(NULL,AV_LOG_DEBUG,"  level = %f\n", t->level);
//  PRINT(" level", t->level);
    PRINT(" phase", t->phase);
    PRINT(" phase_shift", t->phase_shift);
    PRINT(" duration", t->duration);
    PRINT(" samples_im", t->samples_im);
    PRINT(" samples_re", t->samples_re);
    PRINT(" table", t->table);
    }

}
#endif


/**
 * Init parameters from codec extradata
 */
static av_cold int qdm2_decode_init(AVCodecContext *avctx)
{
    QDM2Context *s = avctx->priv_data;
    uint8_t *extradata;
    int extradata_size;
    int tmp_val, tmp, size;

    /* extradata parsing

    Structure:
    wave {
        frma (QDM2)
        QDCA
        QDCP
    }

    32  size (including this field)
    32  tag (=frma)
    32  type (=QDM2 or QDMC)

    32  size (including this field, in bytes)
    32  tag (=QDCA) // maybe mandatory parameters
    32  unknown (=1)
    32  channels (=2)
    32  samplerate (=44100)
    32  bitrate (=96000)
    32  block size (=4096)
    32  frame size (=256) (for one channel)
    32  packet size (=1300)

    32  size (including this field, in bytes)
    32  tag (=QDCP) // maybe some tuneable parameters
    32  float1 (=1.0)
    32  zero ?
    32  float2 (=1.0)
    32  float3 (=1.0)
    32  unknown (27)
    32  unknown (8)
    32  zero ?
    */

    if (!avctx->extradata || (avctx->extradata_size < 48)) {
        av_log(avctx, AV_LOG_ERROR, "extradata missing or truncated\n");
        return -1;
    }

    extradata = avctx->extradata;
    extradata_size = avctx->extradata_size;

    while (extradata_size > 7) {
        if (!memcmp(extradata, "frmaQDM", 7))
            break;
        extradata++;
        extradata_size--;
    }

    if (extradata_size < 12) {
        av_log(avctx, AV_LOG_ERROR, "not enough extradata (%i)\n",
               extradata_size);
        return -1;
    }

    if (memcmp(extradata, "frmaQDM", 7)) {
        av_log(avctx, AV_LOG_ERROR, "invalid headers, QDM? not found\n");
        return -1;
    }

    if (extradata[7] == 'C') {
//        s->is_qdmc = 1;
        av_log(avctx, AV_LOG_ERROR, "stream is QDMC version 1, which is not supported\n");
        return -1;
    }

    extradata += 8;
    extradata_size -= 8;

    size = AV_RB32(extradata);

    if(size > extradata_size){
        av_log(avctx, AV_LOG_ERROR, "extradata size too small, %i < %i\n",
               extradata_size, size);
        return -1;
    }

    extradata += 4;
    av_log(avctx, AV_LOG_DEBUG, "size: %d\n", size);
    if (AV_RB32(extradata) != MKBETAG('Q','D','C','A')) {
        av_log(avctx, AV_LOG_ERROR, "invalid extradata, expecting QDCA\n");
        return -1;
    }

    extradata += 8;

    avctx->channels = s->nb_channels = s->channels = AV_RB32(extradata);
    extradata += 4;

    avctx->sample_rate = AV_RB32(extradata);
    extradata += 4;

    avctx->bit_rate = AV_RB32(extradata);
    extradata += 4;

    s->group_size = AV_RB32(extradata);
    extradata += 4;

    s->fft_size = AV_RB32(extradata);
    extradata += 4;

    s->checksum_size = AV_RB32(extradata);

    s->fft_order = av_log2(s->fft_size) + 1;
    s->fft_frame_size = 2 * s->fft_size; // complex has two floats

    // something like max decodable tones
    s->group_order = av_log2(s->group_size) + 1;
    s->frame_size = s->group_size / 16; // 16 iterations per super block

    s->sub_sampling = s->fft_order - 7;
    s->frequency_range = 255 / (1 << (2 - s->sub_sampling));

    switch ((s->sub_sampling * 2 + s->channels - 1)) {
        case 0: tmp = 40; break;
        case 1: tmp = 48; break;
        case 2: tmp = 56; break;
        case 3: tmp = 72; break;
        case 4: tmp = 80; break;
        case 5: tmp = 100;break;
        default: tmp=s->sub_sampling; break;
    }
    tmp_val = 0;
    if ((tmp * 1000) < avctx->bit_rate)  tmp_val = 1;
    if ((tmp * 1440) < avctx->bit_rate)  tmp_val = 2;
    if ((tmp * 1760) < avctx->bit_rate)  tmp_val = 3;
    if ((tmp * 2240) < avctx->bit_rate)  tmp_val = 4;
    s->cm_table_select = tmp_val;

    if (s->sub_sampling == 0)
        tmp = 7999;
    else
        tmp = ((-(s->sub_sampling -1)) & 8000) + 20000;
    /*
    0: 7999 -> 0
    1: 20000 -> 2
    2: 28000 -> 2
    */
    if (tmp < 8000)
        s->coeff_per_sb_select = 0;
    else if (tmp <= 16000)
        s->coeff_per_sb_select = 1;
    else
        s->coeff_per_sb_select = 2;

    // Fail on unknown fft order
    if ((s->fft_order < 7) || (s->fft_order > 9)) {
        av_log(avctx, AV_LOG_ERROR, "Unknown FFT order (%d), contact the developers!\n", s->fft_order);
        return -1;
    }

    ff_rdft_init(&s->rdft_ctx, s->fft_order, IDFT_C2R);

    qdm2_init(s);

    avctx->sample_fmt = SAMPLE_FMT_S16;

//    dump_context(s);
    return 0;
}


static av_cold int qdm2_decode_close(AVCodecContext *avctx)
{
    QDM2Context *s = avctx->priv_data;

    ff_rdft_end(&s->rdft_ctx);

    return 0;
}


static void qdm2_decode (QDM2Context *q, const uint8_t *in, int16_t *out)
{
    int ch, i;
    const int frame_size = (q->frame_size * q->channels);

    /* select input buffer */
    q->compressed_data = in;
    q->compressed_size = q->checksum_size;

//  dump_context(q);

    /* copy old block, clear new block of output samples */
    memmove(q->output_buffer, &q->output_buffer[frame_size], frame_size * sizeof(float));
    memset(&q->output_buffer[frame_size], 0, frame_size * sizeof(float));

    /* decode block of QDM2 compressed data */
    if (q->sub_packet == 0) {
        q->has_errors = 0; // zero it for a new super block
        av_log(NULL,AV_LOG_DEBUG,"Superblock follows\n");
        qdm2_decode_super_block(q);
    }

    /* parse subpackets */
    if (!q->has_errors) {
        if (q->sub_packet == 2)
            qdm2_decode_fft_packets(q);

        qdm2_fft_tone_synthesizer(q, q->sub_packet);
    }

    /* sound synthesis stage 1 (FFT) */
    for (ch = 0; ch < q->channels; ch++) {
        qdm2_calculate_fft(q, ch, q->sub_packet);

        if (!q->has_errors && q->sub_packet_list_C[0].packet != NULL) {
            SAMPLES_NEEDED_2("has errors, and C list is not empty")
            return;
        }
    }

    /* sound synthesis stage 2 (MPEG audio like synthesis filter) */
    if (!q->has_errors && q->do_synth_filter)
        qdm2_synthesis_filter(q, q->sub_packet);

    q->sub_packet = (q->sub_packet + 1) % 16;

    /* clip and convert output float[] to 16bit signed samples */
    for (i = 0; i < frame_size; i++) {
        int value = (int)q->output_buffer[i];

        if (value > SOFTCLIP_THRESHOLD)
            value = (value >  HARDCLIP_THRESHOLD) ?  32767 :  softclip_table[ value - SOFTCLIP_THRESHOLD];
        else if (value < -SOFTCLIP_THRESHOLD)
            value = (value < -HARDCLIP_THRESHOLD) ? -32767 : -softclip_table[-value - SOFTCLIP_THRESHOLD];

        out[i] = value;
    }
}


static int qdm2_decode_frame(AVCodecContext *avctx,
            void *data, int *data_size,
            AVPacket *avpkt)
{
    const uint8_t *buf = avpkt->data;
    int buf_size = avpkt->size;
    QDM2Context *s = avctx->priv_data;

    if(!buf)
        return 0;
    if(buf_size < s->checksum_size)
        return -1;

    *data_size = s->channels * s->frame_size * sizeof(int16_t);

    av_log(avctx, AV_LOG_DEBUG, "decode(%d): %p[%d] -> %p[%d]\n",
       buf_size, buf, s->checksum_size, data, *data_size);

    qdm2_decode(s, buf, data);

    // reading only when next superblock found
    if (s->sub_packet == 0) {
        return s->checksum_size;
    }

    return 0;
}

AVCodec qdm2_decoder =
{
    .name = "qdm2",
    .type = CODEC_TYPE_AUDIO,
    .id = CODEC_ID_QDM2,
    .priv_data_size = sizeof(QDM2Context),
    .init = qdm2_decode_init,
    .close = qdm2_decode_close,
    .decode = qdm2_decode_frame,
    .long_name = NULL_IF_CONFIG_SMALL("QDesign Music Codec 2"),
};
