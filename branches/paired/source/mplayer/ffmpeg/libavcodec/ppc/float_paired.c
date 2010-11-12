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

#include "libavcodec/dsputil.h"
#include "dsputil_paired.h"
#include "libavutil/ppc/paired.h"

static void vector_fmul_paired(float *dst, const float *src, int len)
{
	vector float pair[2];
	vector float result;
	
	for (int i=0; i<len*4-7; i+=8) {
		pair[0] = paired_lx(i, src);
		pair[1] = paired_lx(i, dst);
		
		result = paired_mul(pair[1], pair[0]);
		paired_stx(result, i, dst);
	}
}

static void vector_fmul_reverse_paired(float *dst, const float *src0, const float *src1, int len)
{
	vector float pair[2];
	vector float result;
	
	src1 += len-2;
	for (int i=0; i<len*4-7; i+=8) {
		pair[0] = paired_lx(i, src0);
		pair[1] = paired_lx(-i, src1);
		pair[1] = paired_merge10(pair[1], pair[1]);
		
		result = paired_mul(pair[0], pair[1]);
		paired_stx(result, i, dst);
	}
}

static void vector_fmul_add_paired(float *dst, const float *src0, const float *src1, const float *src2, int len)
{
	vector float pair[3];
	vector float result;
	
	for (int i=0; i<len*4-7; i+=8) {
		pair[0] = paired_lx(i, src0);
		pair[1] = paired_lx(i, src1);
		pair[2] = paired_lx(i, src2);
		
		result = paired_madd(pair[0], pair[1], pair[2]);
		paired_stx(result, i, dst);
	}
}

static void vector_fmul_window_paired(float *dst, const float *src0, const float *src1, const float *win, float add_bias, int len)
{
	vector float pair[2], window[2];
	vector float bias = {add_bias,add_bias};
	vector float result;
	
	dst += len;
	win += len;
	src0 += len;
	
	int i, j;
	for (i=-len*4, j=len*4-8; i<0; i+=8, j-=8) {
		pair[0] = paired_lx(i, src0);
		pair[1] = paired_lx(j, src1);
		pair[1] = paired_merge10(pair[1], pair[1]);
		
		window[0] = paired_lx(i, win);
		window[1] = paired_lx(j, win);
		window[1] = paired_merge10(window[1], window[1]);
		
		result = paired_mul(pair[1], window[0]);
		result = paired_msub(pair[0], window[1], result);
		result = paired_add(result, bias);
		paired_stx(result, i, dst);
		
		result = paired_madd(pair[1], window[1], bias);
		result = paired_madd(pair[0], window[0], result);
		result = paired_merge10(result, result);
		paired_stx(result, j, dst);
	}
}

static void float_to_int16_paired(int16_t *dst, const float *src, long len)
{
	src -= 2;
	dst -= 2;
	
	for (int i=0; i<len-1; i+=2) {
		vector float pair = psq_lu(8,src,0,0);
		psq_stu(pair,4,dst,0,7);
	}
}

static void float_to_int16_interleave_paired(int16_t *dst, const float **src, long len, int channels)
{
	vector float pair[2];
	vector float result;
	
	int i, c;
	if (channels > 2) {
		for (i=0; i<len-1; i+=2) {
			for (c=0; c<channels-1; c+=2) {
				pair[0] = paired_lx(0, src[c]+i);
				pair[1] = paired_lx(0, src[c+1]+i);
				
				result = paired_merge00(pair[0], pair[1]);
				psq_st(result,0,dst+(channels*i)+c,0,7);
				
				result = paired_merge11(pair[0], pair[1]);
				psq_st(result,0,dst+(channels*i+1)+c,0,7);
			}
		}
	} else {
		if (channels == 2) {
			for (i=0; i<len*4-7; i+=8) {
				pair[0] = paired_lx(i, src[0]);
				pair[1] = paired_lx(i, src[1]);
				
				result = paired_merge00(pair[0], pair[1]);
				psq_stx(result,i,dst,0,7);
				
				result = paired_merge11(pair[0], pair[1]);
				psq_stx(result,i+4,dst,0,7);
			}
		} else
			float_to_int16_paired(dst, src[0], len);
	}
}

static void butterflies_float_paired(float *restrict v1, float *restrict v2, int len)
{
	vector float pair[2];
	vector float result;
	
	for (int i=0; i<len*4-7; i+=8) {
		pair[0] = paired_lx(i, v1);
		pair[1] = paired_lx(i, v2);
		
		result = paired_add(pair[0], pair[1]);
		paired_stx(result, i, v1);
		
		result = paired_sub(pair[0], pair[1]);
		paired_stx(result, i, v2);
	}
}

static float scalarproduct_float_paired(const float *v1, const float *v2, int len)
{
	vector float pair[2];
	vector float result = {0.0,0.0};
	
	for (int i=0; i<len*4-7; i+=8) {
		pair[0] = paired_lx(i, v1);
		pair[1] = paired_lx(i, v2);
		result = paired_madd(pair[0], pair[1], result);
	}
	
	register float scalar;
	asm("ps_sum0	%0,%1,%1,%1" : "=f"(scalar) : "f"(result));
	return scalar;
}

static void vector_fmul_scalar_paired(float *dst, const float *src, float mul, int len)
{
	vector float pair, result;
	
	for (int i=0; i<len*4-7; i+=8) {
		pair = paired_lx(i, src);
		result = ps_muls0(pair, mul);
		paired_stx(result, i, dst);
	}
}

void float_init_paired(DSPContext *c, AVCodecContext *avctx)
{
	register uint32_t gqr;
	asm volatile(
		"li		%0,7\n"
		"oris	%0,%0,7\n"
		"mtspr	919,%0"
		: "=r"(gqr)
	);
	
	c->vector_fmul = vector_fmul_paired;
	c->vector_fmul_reverse = vector_fmul_reverse_paired;
	c->vector_fmul_add = vector_fmul_add_paired;
	c->vector_fmul_window = vector_fmul_window_paired;
	c->float_to_int16 = float_to_int16_paired;
	c->float_to_int16_interleave = float_to_int16_interleave_paired;
	c->butterflies_float = butterflies_float_paired;
	c->scalarproduct_float = scalarproduct_float_paired;
	c->vector_fmul_scalar = vector_fmul_scalar_paired;
}
