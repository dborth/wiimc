/*
 * This file is part of MPlayer CE.
 *
 * MPlayer CE is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * MPlayer CE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with MPlayer CE; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "libavcodec/h264pred.h"
#include "libavutil/ppc/paired.h"

static void pred4x4_tm_vp8_paired(uint8_t *src, const uint8_t *topright, int stride)
{
	vector float pair[2];
	vector float base, offset;
	
	src -= stride + 1;
	base = psq_l(0,src,1,4);
	
	pair[0] = psq_l(1,src,0,4);
	pair[1] = psq_l(3,src,0,4);
	
	for (int y=0; y<4; y++) {
		offset = psq_lux(src,stride,1,4);
		asm("fsubs %0,%1,%0" : "+f"(offset) : "f"(base));
		
		psq_st(paired_sub(pair[0], offset),1,src,0,4);
		psq_st(paired_sub(pair[1], offset),3,src,0,4);
	}
}

static void pred16x16_tm_vp8_paired(uint8_t *src, int stride)
{
	vector float pair[8];
	vector float base, offset;
	
	src -= stride + 1;
	base = psq_l(0,src,1,4);
	
	pair[0] = psq_l(1,src,0,4);
	pair[1] = psq_l(3,src,0,4);
	pair[2] = psq_l(5,src,0,4);
	pair[3] = psq_l(7,src,0,4);
	pair[4] = psq_l(9,src,0,4);
	pair[5] = psq_l(11,src,0,4);
	pair[6] = psq_l(13,src,0,4);
	pair[7] = psq_l(15,src,0,4);
	
	for (int y=0; y<16; y++) {
		offset = psq_lux(src,stride,1,4);
		asm("fsubs %0,%1,%0" : "+f"(offset) : "f"(base));
		
		psq_st(paired_sub(pair[0], offset),1,src,0,4);
		psq_st(paired_sub(pair[1], offset),3,src,0,4);
		psq_st(paired_sub(pair[2], offset),5,src,0,4);
		psq_st(paired_sub(pair[3], offset),7,src,0,4);
		psq_st(paired_sub(pair[4], offset),9,src,0,4);
		psq_st(paired_sub(pair[5], offset),11,src,0,4);
		psq_st(paired_sub(pair[6], offset),13,src,0,4);
		psq_st(paired_sub(pair[7], offset),15,src,0,4);
	}
}

static void pred8x8_tm_vp8_paired(uint8_t *src, int stride)
{
	vector float pair[4];
	vector float base, offset;
	
	src -= stride + 1;
	base = psq_l(0,src,1,4);
	
	pair[0] = psq_l(1,src,0,4);
	pair[1] = psq_l(3,src,0,4);
	pair[2] = psq_l(5,src,0,4);
	pair[3] = psq_l(7,src,0,4);
	
	for (int y=0; y<8; y++) {
		offset = psq_lux(src,stride,1,4);
		asm("fsubs %0,%1,%0" : "+f"(offset) : "f"(base));
		
		psq_st(paired_sub(pair[0], offset),1,src,0,4);
		psq_st(paired_sub(pair[1], offset),3,src,0,4);
		psq_st(paired_sub(pair[2], offset),5,src,0,4);
		psq_st(paired_sub(pair[3], offset),7,src,0,4);
	}
}

void ff_h264_pred_init_ppc(H264PredContext *h, int codec_id)
{
	if (codec_id == CODEC_ID_VP8) {
		h->pred4x4[TM_VP8_PRED] = pred4x4_tm_vp8_paired;
		h->pred8x8[PLANE_PRED8x8] = pred8x8_tm_vp8_paired;
		h->pred16x16[PLANE_PRED8x8] = pred16x16_tm_vp8_paired;
	}
}
