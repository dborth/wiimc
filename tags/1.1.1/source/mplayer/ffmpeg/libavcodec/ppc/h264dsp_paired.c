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
#include "libavcodec/h264data.h"
#include "libavcodec/h264dsp.h"
#include "dsputil_paired.h"
#include "libavutil/ppc/paired.h"

static void put_h264_chroma_mc4_paired(uint8_t *dst, uint8_t *src, int stride, int h, int x, int y)
{
	const double scale = 0.015625;
	const float half = 0.5;
	
	const float fA = (double)((8-x)*(8-y))*scale;
	const float fB = (double)((  x)*(8-y))*scale;
	const float fC = (double)((8-x)*(  y))*scale;
	const float fD = (double)((  x)*(  y))*scale;
	
	vector float pair[4];
	vector float result;
	
	dst -= stride;
	uint8_t *src1 = src - stride;
	uint8_t *src2 = src;
	
	int i;
	if (fD) {
		for (i=0; i<h; i++) {
			pair[0] = psq_lux(src1,stride,0,4);
			pair[1] = psq_l(2,src1,0,4);
			
			pair[2] = psq_lux(src2,stride,0,4);
			pair[3] = psq_l(2,src2,0,4);
			
			result = ps_madds0(pair[0], fA, half);
			result = ps_madds0(paired_merge10(pair[0], pair[1]), fB, result);
			result = ps_madds0(pair[2], fC, result);
			result = ps_madds0(paired_merge10(pair[2], pair[3]), fD, result);
			psq_stux(result,dst,stride,0,4);
			
			pair[0] = psq_l(4,src1,1,4);
			pair[2] = psq_l(4,src2,1,4);
			
			result = ps_madds0(pair[1], fA, half);
			result = ps_madds0(paired_merge10(pair[1], pair[0]), fB, result);
			result = ps_madds0(pair[3], fC, result);
			result = ps_madds0(paired_merge10(pair[3], pair[2]), fD, result);
			psq_st(result,2,dst,0,4);
		}
	} else {
		const float fE = fB+fC;
		
		if (fC) {
			for (i=0; i<h; i++) {
				pair[0] = psq_lux(src1,stride,0,4);
				pair[2] = psq_lux(src2,stride,0,4);
				
				result = ps_madds0(pair[0], fA, half);
				result = ps_madds0(pair[2], fE, result);
				psq_stux(result,dst,stride,0,4);
				
				pair[0] = psq_l(2,src1,0,4);
				pair[2] = psq_l(2,src2,0,4);
				
				result = ps_madds0(pair[0], fA, half);
				result = ps_madds0(pair[2], fE, result);
				psq_st(result,2,dst,0,4);
			}
		} else {
			for (i=0; i<h; i++) {
				pair[0] = psq_lux(src1,stride,0,4);
				pair[1] = psq_l(2,src1,0,4);
				
				result = ps_madds0(pair[0], fA, half);
				result = ps_madds0(paired_merge10(pair[0], pair[1]), fE, result);
				psq_stux(result,dst,stride,0,4);
				
				pair[0] = psq_l(4,src1,1,4);
				
				result = ps_madds0(pair[1], fA, half);
				result = ps_madds0(paired_merge10(pair[1], pair[0]), fE, result);
				psq_st(result,2,dst,0,4);
			}
		}
	}
}

static void put_h264_chroma_mc8_paired(uint8_t *dst, uint8_t *src, int stride, int h, int x, int y)
{
	const double scale = 0.015625;
	const float half = 0.5;
	
	const float fA = (double)((8-x)*(8-y))*scale;
	const float fB = (double)((  x)*(8-y))*scale;
	const float fC = (double)((8-x)*(  y))*scale;
	const float fD = (double)((  x)*(  y))*scale;
	
	vector float pair[4];
	vector float result;
	
	dst -= stride;
	uint8_t *src1 = src - stride;
	uint8_t *src2 = src;
	
	int i;
	if (fD) {
		for (i=0; i<h; i++) {
			pair[0] = psq_lux(src1,stride,0,4);
			pair[1] = psq_l(2,src1,0,4);
			
			pair[2] = psq_lux(src2,stride,0,4);
			pair[3] = psq_l(2,src2,0,4);
			
			result = ps_madds0(pair[0], fA, half);
			result = ps_madds0(paired_merge10(pair[0], pair[1]), fB, result);
			result = ps_madds0(pair[2], fC, result);
			result = ps_madds0(paired_merge10(pair[2], pair[3]), fD, result);
			psq_stux(result,dst,stride,0,4);
			
			pair[0] = psq_l(4,src1,0,4);
			pair[2] = psq_l(4,src2,0,4);
			
			result = ps_madds0(pair[1], fA, half);
			result = ps_madds0(paired_merge10(pair[1], pair[0]), fB, result);
			result = ps_madds0(pair[3], fC, result);
			result = ps_madds0(paired_merge10(pair[3], pair[2]), fD, result);
			psq_st(result,2,dst,0,4);
			
			pair[1] = psq_l(6,src1,0,4);
			pair[3] = psq_l(6,src2,0,4);
			
			result = ps_madds0(pair[0], fA, half);
			result = ps_madds0(paired_merge10(pair[0], pair[1]), fB, result);
			result = ps_madds0(pair[2], fC, result);
			result = ps_madds0(paired_merge10(pair[2], pair[3]), fD, result);
			psq_st(result,4,dst,0,4);
			
			pair[0] = psq_l(8,src1,1,4);
			pair[2] = psq_l(8,src2,1,4);
			
			result = ps_madds0(pair[1], fA, half);
			result = ps_madds0(paired_merge10(pair[1], pair[0]), fB, result);
			result = ps_madds0(pair[3], fC, result);
			result = ps_madds0(paired_merge10(pair[3], pair[2]), fD, result);
			psq_st(result,6,dst,0,4);
		}
	} else {
		const float fE = fB+fC;
		
		if (fC) {
			for (i=0; i<h; i++) {
				pair[0] = psq_lux(src1,stride,0,4);
				pair[2] = psq_lux(src2,stride,0,4);
				
				result = ps_madds0(pair[0], fA, half);
				result = ps_madds0(pair[2], fE, result);
				psq_stux(result,dst,stride,0,4);
				
				pair[0] = psq_l(2,src1,0,4);
				pair[2] = psq_l(2,src2,0,4);
				
				result = ps_madds0(pair[0], fA, half);
				result = ps_madds0(pair[2], fE, result);
				psq_st(result,2,dst,0,4);
				
				pair[0] = psq_l(4,src1,0,4);
				pair[2] = psq_l(4,src2,0,4);
				
				result = ps_madds0(pair[0], fA, half);
				result = ps_madds0(pair[2], fE, result);
				psq_st(result,4,dst,0,4);
				
				pair[0] = psq_l(6,src1,0,4);
				pair[2] = psq_l(6,src2,0,4);
				
				result = ps_madds0(pair[0], fA, half);
				result = ps_madds0(pair[2], fE, result);
				psq_st(result,6,dst,0,4);
			}
		} else {
			for (i=0; i<h; i++) {
				pair[0] = psq_lux(src1,stride,0,4);
				pair[1] = psq_l(2,src1,0,4);
				
				result = ps_madds0(pair[0], fA, half);
				result = ps_madds0(paired_merge10(pair[0], pair[1]), fE, result);
				psq_stux(result,dst,stride,0,4);
				
				pair[0] = psq_l(4,src1,0,4);
				
				result = ps_madds0(pair[1], fA, half);
				result = ps_madds0(paired_merge10(pair[1], pair[0]), fE, result);
				psq_st(result,2,dst,0,4);
				
				pair[1] = psq_l(6,src1,0,4);
				
				result = ps_madds0(pair[0], fA, half);
				result = ps_madds0(paired_merge10(pair[0], pair[1]), fE, result);
				psq_st(result,4,dst,0,4);
				
				pair[0] = psq_l(8,src1,1,4);
				
				result = ps_madds0(pair[1], fA, half);
				result = ps_madds0(paired_merge10(pair[1], pair[0]), fE, result);
				psq_st(result,6,dst,0,4);
			}
		}
	}
}

static void avg_h264_chroma_mc4_paired(uint8_t *dst, uint8_t *src, int stride, int h, int x, int y)
{
	const double scale = 0.015625;
	const float half = 0.5;
	
	const float fA = (double)((8-x)*(8-y))*scale;
	const float fB = (double)((  x)*(8-y))*scale;
	const float fC = (double)((8-x)*(  y))*scale;
	const float fD = (double)((  x)*(  y))*scale;
	
	vector float pair[4];
	vector float result;
	
	dst -= stride;
	uint8_t *src1 = src - stride;
	uint8_t *src2 = src;
	
	int i;
	if (fD) {
		for (i=0; i<h; i++) {
			pair[0] = psq_lux(src1,stride,0,4);
			pair[1] = psq_l(2,src1,0,4);
			
			pair[2] = psq_lux(src2,stride,0,4);
			pair[3] = psq_l(2,src2,0,4);
			
			result = psq_lux(dst,stride,0,4);
			
			result = ps_madds0(pair[0], fA, result);
			result = ps_madds0(paired_merge10(pair[0], pair[1]), fB, result);
			result = ps_madds0(pair[2], fC, result);
			result = ps_madds0(paired_merge10(pair[2], pair[3]), fD, result);
			psq_st(ps_madd(result, half, half),0,dst,0,4);
			
			pair[0] = psq_l(4,src1,1,4);
			pair[2] = psq_l(4,src2,1,4);
			
			result = psq_l(2,dst,0,4);
			
			result = ps_madds0(pair[1], fA, result);
			result = ps_madds0(paired_merge10(pair[1], pair[0]), fB, result);
			result = ps_madds0(pair[3], fC, result);
			result = ps_madds0(paired_merge10(pair[3], pair[2]), fD, result);
			psq_st(ps_madd(result, half, half),2,dst,0,4);
		}
	} else {
		const float fE = fB+fC;
		
		if (fC) {
			for (i=0; i<h; i++) {
				pair[0] = psq_lux(src1,stride,0,4);
				pair[1] = psq_lux(src2,stride,0,4);
				
				result = psq_lux(dst,stride,0,4);
				
				result = ps_madds0(pair[0], fA, result);
				result = ps_madds0(pair[1], fE, result);
				psq_st(ps_madd(result, half, half),0,dst,0,4);
				
				pair[0] = psq_l(2,src1,0,4);
				pair[1] = psq_l(2,src2,0,4);
				
				result = psq_l(2,dst,0,4);
				
				result = ps_madds0(pair[0], fA, result);
				result = ps_madds0(pair[1], fE, result);
				psq_st(ps_madd(result, half, half),2,dst,0,4);
			}
		} else {
			for (i=0; i<h; i++) {
				pair[0] = psq_lux(src1,stride,0,4);
				pair[1] = psq_l(2,src1,0,4);
				
				result = psq_lux(dst,stride,0,4);
				
				result = ps_madds0(pair[0], fA, result);
				result = ps_madds0(paired_merge10(pair[0], pair[1]), fE, result);
				psq_st(ps_madd(result, half, half),0,dst,0,4);
				
				pair[0] = psq_l(4,src1,1,4);
				result = psq_l(2,dst,0,4);
				
				result = ps_madds0(pair[1], fA, result);
				result = ps_madds0(paired_merge10(pair[1], pair[0]), fE, result);
				psq_st(ps_madd(result, half, half),2,dst,0,4);
			}
		}
	}
}

static void avg_h264_chroma_mc8_paired(uint8_t *dst, uint8_t *src, int stride, int h, int x, int y)
{
	const double scale = 0.015625;
	const float half = 0.5;
	
	const float fA = (double)((8-x)*(8-y))*scale;
	const float fB = (double)((  x)*(8-y))*scale;
	const float fC = (double)((8-x)*(  y))*scale;
	const float fD = (double)((  x)*(  y))*scale;
	
	vector float pair[4];
	vector float result;
	
	dst -= stride;
	uint8_t *src1 = src - stride;
	uint8_t *src2 = src;
	
	int i;
	if (fD) {
		for (i=0; i<h; i++) {
			pair[0] = psq_lux(src1,stride,0,4);
			pair[1] = psq_l(2,src1,0,4);
			
			pair[2] = psq_lux(src2,stride,0,4);
			pair[3] = psq_l(2,src2,0,4);
			
			result = psq_lux(dst,stride,0,4);
			
			result = ps_madds0(pair[0], fA, result);
			result = ps_madds0(paired_merge10(pair[0], pair[1]), fB, result);
			result = ps_madds0(pair[2], fC, result);
			result = ps_madds0(paired_merge10(pair[2], pair[3]), fD, result);
			psq_st(ps_madd(result, half, half),0,dst,0,4);
			
			pair[0] = psq_l(4,src1,0,4);
			pair[2] = psq_l(4,src2,0,4);
			
			result = psq_l(2,dst,0,4);
			
			result = ps_madds0(pair[1], fA, result);
			result = ps_madds0(paired_merge10(pair[1], pair[0]), fB, result);
			result = ps_madds0(pair[3], fC, result);
			result = ps_madds0(paired_merge10(pair[3], pair[2]), fD, result);
			psq_st(ps_madd(result, half, half),2,dst,0,4);
			
			pair[1] = psq_l(6,src1,0,4);
			pair[3] = psq_l(6,src2,0,4);
			
			result = psq_l(4,dst,0,4);
			
			result = ps_madds0(pair[0], fA, result);
			result = ps_madds0(paired_merge10(pair[0], pair[1]), fB, result);
			result = ps_madds0(pair[2], fC, result);
			result = ps_madds0(paired_merge10(pair[2], pair[3]), fD, result);
			psq_st(ps_madd(result, half, half),4,dst,0,4);
			
			pair[0] = psq_l(8,src1,1,4);
			pair[2] = psq_l(8,src2,1,4);
			
			result = psq_l(6,dst,0,4);
			
			result = ps_madds0(pair[1], fA, result);
			result = ps_madds0(paired_merge10(pair[1], pair[0]), fB, result);
			result = ps_madds0(pair[3], fC, result);
			result = ps_madds0(paired_merge10(pair[3], pair[2]), fD, result);
			psq_st(ps_madd(result, half, half),6,dst,0,4);
		}
	} else {
		const float fE = fB+fC;
		
		if (fC) {
			for (i=0; i<h; i++) {
				pair[0] = psq_lux(src1,stride,0,4);
				pair[1] = psq_lux(src2,stride,0,4);
				
				result = psq_lux(dst,stride,0,4);
				
				result = ps_madds0(pair[0], fA, result);
				result = ps_madds0(pair[1], fE, result);
				psq_st(ps_madd(result, half, half),0,dst,0,4);
				
				pair[0] = psq_l(2,src1,0,4);
				pair[1] = psq_l(2,src2,0,4);
				
				result = psq_l(2,dst,0,4);
				
				result = ps_madds0(pair[0], fA, result);
				result = ps_madds0(pair[1], fE, result);
				psq_st(ps_madd(result, half, half),2,dst,0,4);
				
				pair[0] = psq_l(4,src1,0,4);
				pair[1] = psq_l(4,src2,0,4);
				
				result = psq_l(4,dst,0,4);
				
				result = ps_madds0(pair[0], fA, result);
				result = ps_madds0(pair[1], fE, result);
				psq_st(ps_madd(result, half, half),4,dst,0,4);
				
				pair[0] = psq_l(6,src1,0,4);
				pair[1] = psq_l(6,src2,0,4);
				
				result = psq_l(6,dst,0,4);
				
				result = ps_madds0(pair[0], fA, result);
				result = ps_madds0(pair[1], fE, result);
				psq_st(ps_madd(result, half, half),6,dst,0,4);
			}
		} else {
			for (i=0; i<h; i++) {
				pair[0] = psq_lux(src1,stride,0,4);
				pair[1] = psq_l(2,src1,0,4);
				
				result = psq_lux(dst,stride,0,4);
				
				result = ps_madds0(pair[0], fA, result);
				result = ps_madds0(paired_merge10(pair[0], pair[1]), fE, result);
				psq_st(ps_madd(result, half, half),0,dst,0,4);
				
				pair[0] = psq_l(4,src1,0,4);
				result = psq_l(2,dst,0,4);
				
				result = ps_madds0(pair[1], fA, result);
				result = ps_madds0(paired_merge10(pair[1], pair[0]), fE, result);
				psq_st(ps_madd(result, half, half),2,dst,0,4);
				
				pair[1] = psq_l(6,src1,0,4);
				result = psq_l(4,dst,0,4);
				
				result = ps_madds0(pair[0], fA, result);
				result = ps_madds0(paired_merge10(pair[0], pair[1]), fE, result);
				psq_st(ps_madd(result, half, half),4,dst,0,4);
				
				pair[0] = psq_l(8,src1,1,4);
				result = psq_l(6,dst,0,4);
				
				result = ps_madds0(pair[1], fA, result);
				result = ps_madds0(paired_merge10(pair[1], pair[0]), fE, result);
				psq_st(ps_madd(result, half, half),6,dst,0,4);
			}
		}
	}
}

static void put_no_rnd_vc1_chroma_mc8_paired(uint8_t *dst, uint8_t *src, int stride, int h, int x, int y)
{
	const double scale = 0.015625;
	const float offset = 0.4375;
	
	const float fA = (double)((8-x)*(8-y))*scale;
	const float fB = (double)((  x)*(8-y))*scale;
	const float fC = (double)((8-x)*(  y))*scale;
	const float fD = (double)((  x)*(  y))*scale;
	
	vector float pair[4];
	vector float result;
	
	dst -= stride;
	uint8_t *src1 = src - stride;
	uint8_t *src2 = src;
	
	int i;
	if (fD) {
		for (i=0; i<h; i++) {
			pair[0] = psq_lux(src1,stride,0,4);
			pair[1] = psq_l(2,src1,0,4);
			
			pair[2] = psq_lux(src2,stride,0,4);
			pair[3] = psq_l(2,src2,0,4);
			
			result = ps_madds0(pair[0], fA, offset);
			result = ps_madds0(paired_merge10(pair[0], pair[1]), fB, result);
			result = ps_madds0(pair[2], fC, result);
			result = ps_madds0(paired_merge10(pair[2], pair[3]), fD, result);
			psq_stux(result,dst,stride,0,4);
			
			pair[0] = psq_l(4,src1,0,4);
			pair[2] = psq_l(4,src2,0,4);
			
			result = ps_madds0(pair[1], fA, offset);
			result = ps_madds0(paired_merge10(pair[1], pair[0]), fB, result);
			result = ps_madds0(pair[3], fC, result);
			result = ps_madds0(paired_merge10(pair[3], pair[2]), fD, result);
			psq_st(result,2,dst,0,4);
			
			pair[1] = psq_l(6,src1,0,4);
			pair[3] = psq_l(6,src2,0,4);
			
			result = ps_madds0(pair[0], fA, offset);
			result = ps_madds0(paired_merge10(pair[0], pair[1]), fB, result);
			result = ps_madds0(pair[2], fC, result);
			result = ps_madds0(paired_merge10(pair[2], pair[3]), fD, result);
			psq_st(result,4,dst,0,4);
			
			pair[0] = psq_l(8,src1,1,4);
			pair[2] = psq_l(8,src2,1,4);
			
			result = ps_madds0(pair[1], fA, offset);
			result = ps_madds0(paired_merge10(pair[1], pair[0]), fB, result);
			result = ps_madds0(pair[3], fC, result);
			result = ps_madds0(paired_merge10(pair[3], pair[2]), fD, result);
			psq_st(result,6,dst,0,4);
		}
	} else {
		const float fE = fB + fC;
		
		if (fC) {
			for (i=0; i<h; i++) {
				pair[0] = psq_lux(src1,stride,0,4);
				pair[2] = psq_lux(src2,stride,0,4);
				
				result = ps_madds0(pair[0], fA, offset);
				result = ps_madds0(pair[2], fE, result);
				psq_stux(result,dst,stride,0,4);
				
				pair[0] = psq_l(2,src1,0,4);
				pair[2] = psq_l(2,src2,0,4);
				
				result = ps_madds0(pair[0], fA, offset);
				result = ps_madds0(pair[2], fE, result);
				psq_st(result,2,dst,0,4);
				
				pair[0] = psq_l(4,src1,0,4);
				pair[2] = psq_l(4,src2,0,4);
				
				result = ps_madds0(pair[0], fA, offset);
				result = ps_madds0(pair[2], fE, result);
				psq_st(result,4,dst,0,4);
				
				pair[0] = psq_l(6,src1,0,4);
				pair[2] = psq_l(6,src2,0,4);
				
				result = ps_madds0(pair[0], fA, offset);
				result = ps_madds0(pair[2], fE, result);
				psq_st(result,6,dst,0,4);
			}
		} else {
			for (i=0; i<h; i++) {
				pair[0] = psq_lux(src1,stride,0,4);
				pair[1] = psq_l(2,src1,0,4);
				
				result = ps_madds0(pair[0], fA, offset);
				result = ps_madds0(paired_merge10(pair[0], pair[1]), fE, result);
				psq_stux(result,dst,stride,0,4);
				
				pair[0] = psq_l(4,src1,0,4);
				
				result = ps_madds0(pair[1], fA, offset);
				result = ps_madds0(paired_merge10(pair[1], pair[0]), fE, result);
				psq_st(result,2,dst,0,4);
				
				pair[1] = psq_l(6,src1,0,4);
				
				result = ps_madds0(pair[0], fA, offset);
				result = ps_madds0(paired_merge10(pair[0], pair[1]), fE, result);
				psq_st(result,4,dst,0,4);
				
				pair[0] = psq_l(8,src1,1,4);
				
				result = ps_madds0(pair[1], fA, offset);
				result = ps_madds0(paired_merge10(pair[1], pair[0]), fE, result);
				psq_st(result,6,dst,0,4);
			}
		}
	}
}

static void avg_no_rnd_vc1_chroma_mc8_paired(uint8_t *dst, uint8_t *src, int stride, int h, int x, int y)
{
	const double scale = 0.015625;
	const float offset = 0.4375;
	const float half = 0.5;
	
	const float fA = (double)((8-x)*(8-y))*scale;
	const float fB = (double)((  x)*(8-y))*scale;
	const float fC = (double)((8-x)*(  y))*scale;
	const float fD = (double)((  x)*(  y))*scale;
	
	vector float pair[4];
	vector float result;
	
	dst -= stride;
	uint8_t *src1 = src - stride;
	uint8_t *src2 = src;
	
	int i;
	if (fD) {
		for (i=0; i<h; i++) {
			pair[0] = psq_lux(src1,stride,0,4);
			pair[1] = psq_l(2,src1,0,4);
			
			pair[2] = psq_lux(src2,stride,0,4);
			pair[3] = psq_l(2,src2,0,4);
			
			result = psq_lux(dst,stride,0,4);
			
			result = ps_madds0(pair[0], fA, result);
			result = ps_madds0(paired_merge10(pair[0], pair[1]), fB, result);
			result = ps_madds0(pair[2], fC, result);
			result = ps_madds0(paired_merge10(pair[2], pair[3]), fD, result);
			psq_st(ps_madd(result, half, offset),0,dst,0,4);
			
			pair[0] = psq_l(4,src1,0,4);
			pair[2] = psq_l(4,src2,0,4);
			
			result = psq_l(2,dst,0,4);
			
			result = ps_madds0(pair[1], fA, result);
			result = ps_madds0(paired_merge10(pair[1], pair[0]), fB, result);
			result = ps_madds0(pair[3], fC, result);
			result = ps_madds0(paired_merge10(pair[3], pair[2]), fD, result);
			psq_st(ps_madd(result, half, offset),2,dst,0,4);
			
			pair[1] = psq_l(6,src1,0,4);
			pair[3] = psq_l(6,src2,0,4);
			
			result = psq_l(4,dst,0,4);
			
			result = ps_madds0(pair[0], fA, result);
			result = ps_madds0(paired_merge10(pair[0], pair[1]), fB, result);
			result = ps_madds0(pair[2], fC, result);
			result = ps_madds0(paired_merge10(pair[2], pair[3]), fD, result);
			psq_st(ps_madd(result, half, offset),4,dst,0,4);
			
			pair[0] = psq_l(8,src1,1,4);
			pair[2] = psq_l(8,src2,1,4);
			
			result = psq_l(6,dst,0,4);
			
			result = ps_madds0(pair[1], fA, result);
			result = ps_madds0(paired_merge10(pair[1], pair[0]), fB, result);
			result = ps_madds0(pair[3], fC, result);
			result = ps_madds0(paired_merge10(pair[3], pair[2]), fD, result);
			psq_st(ps_madd(result, half, offset),6,dst,0,4);
		}
	} else {
		const float fE = fB + fC;
		
		if (fC) {
			for (i=0; i<h; i++) {
				pair[0] = psq_lux(src1,stride,0,4);
				pair[1] = psq_lux(src2,stride,0,4);
				
				result = psq_lux(dst,stride,0,4);
				
				result = ps_madds0(pair[0], fA, result);
				result = ps_madds0(pair[1], fE, result);
				psq_st(ps_madd(result, half, offset),0,dst,0,4);
				
				pair[0] = psq_l(2,src1,0,4);
				pair[1] = psq_l(2,src2,0,4);
				
				result = psq_l(2,dst,0,4);
				
				result = ps_madds0(pair[0], fA, result);
				result = ps_madds0(pair[1], fE, result);
				psq_st(ps_madd(result, half, offset),2,dst,0,4);
				
				pair[0] = psq_l(4,src1,0,4);
				pair[1] = psq_l(4,src2,0,4);
				
				result = psq_l(4,dst,0,4);
				
				result = ps_madds0(pair[0], fA, result);
				result = ps_madds0(pair[1], fE, result);
				psq_st(ps_madd(result, half, offset),4,dst,0,4);
				
				pair[0] = psq_l(6,src1,0,4);
				pair[1] = psq_l(6,src2,0,4);
				
				result = psq_l(6,dst,0,4);
				
				result = ps_madds0(pair[0], fA, result);
				result = ps_madds0(pair[1], fE, result);
				psq_st(ps_madd(result, half, offset),6,dst,0,4);
			}
		} else {
			for (i=0; i<h; i++) {
				pair[0] = psq_lux(src1,stride,0,4);
				pair[1] = psq_l(2,src1,0,4);
				
				result = psq_lux(dst,stride,0,4);
				
				result = ps_madds0(pair[0], fA, result);
				result = ps_madds0(paired_merge10(pair[0], pair[1]), fE, result);
				psq_st(ps_madd(result, half, offset),0,dst,0,4);
				
				pair[0] = psq_l(4,src1,0,4);
				result = psq_l(2,dst,0,4);
				
				result = ps_madds0(pair[1], fA, result);
				result = ps_madds0(paired_merge10(pair[1], pair[0]), fE, result);
				psq_st(ps_madd(result, half, offset),2,dst,0,4);
				
				pair[1] = psq_l(6,src1,0,4);
				result = psq_l(4,dst,0,4);
				
				result = ps_madds0(pair[0], fA, result);
				result = ps_madds0(paired_merge10(pair[0], pair[1]), fE, result);
				psq_st(ps_madd(result, half, offset),4,dst,0,4);
				
				pair[0] = psq_l(8,src1,1,4);
				result = psq_l(6,dst,0,4);
				
				result = ps_madds0(pair[1], fA, result);
				result = ps_madds0(paired_merge10(pair[1], pair[0]), fE, result);
				psq_st(ps_madd(result, half, offset),6,dst,0,4);
			}
		}
	}
}

static void ff_h264_idct_add_paired(uint8_t *dst, DCTELEM *block, int stride)
{
	const float half = 0.5;
	const float scalar = 0.015625;
	vector float pair[2], sub[2], add[2];
	vector float result, element[8];
	
	block[0] += 32;
	
	uint8_t *dst1 = dst;
	uint8_t *dst2 = dst;
	
	pair[0] = psq_l(0,block,0,7);
	pair[1] = psq_l(16,block,0,7);
	add[0] = paired_add(pair[0], pair[1]);
	sub[0] = paired_sub(pair[0], pair[1]);
	
	pair[0] = psq_l(8,block,0,7);
	pair[1] = psq_l(24,block,0,7);
	sub[1] = ps_msub(pair[0], half, pair[1]);
	add[1] = ps_madd(pair[1], half, pair[0]);
	
	pair[0] = paired_add(add[0], add[1]);
	pair[1] = paired_add(sub[0], sub[1]);
	
	element[0] = paired_merge00(pair[0], pair[1]);
	element[2] = paired_merge11(pair[0], pair[1]);
	
	pair[0] = paired_sub(sub[0], sub[1]);
	pair[1] = paired_sub(add[0], add[1]);
	
	element[1] = paired_merge00(pair[0], pair[1]);
	element[3] = paired_merge11(pair[0], pair[1]);
	
	pair[0] = psq_l(4,block,0,7);
	pair[1] = psq_l(20,block,0,7);
	add[0] = paired_add(pair[0], pair[1]);
	sub[0] = paired_sub(pair[0], pair[1]);
	
	pair[0] = psq_l(12,block,0,7);
	pair[1] = psq_l(28,block,0,7);
	sub[1] = ps_msub(pair[0], half, pair[1]);
	add[1] = ps_madd(pair[1], half, pair[0]);
	
	pair[0] = paired_add(add[0], add[1]);
	pair[1] = paired_add(sub[0], sub[1]);
	
	element[4] = paired_merge00(pair[0], pair[1]);
	element[6] = paired_merge11(pair[0], pair[1]);
	
	pair[0] = paired_sub(sub[0], sub[1]);
	pair[1] = paired_sub(add[0], add[1]);
	
	element[5] = paired_merge00(pair[0], pair[1]);
	element[7] = paired_merge11(pair[0], pair[1]);
	
	add[0] = paired_add(element[0], element[4]);
	sub[0] = paired_sub(element[0], element[4]);
	
	sub[1] = ps_msub(element[2], half, element[6]);
	add[1] = ps_madd(element[6], half, element[2]);
	
	result = psq_l(0,dst1,0,4);
	result = ps_madd(paired_add(add[0], add[1]), scalar, result);
	psq_st(result,0,dst1,0,4);
	
	result = psq_lux(dst1,stride,0,4);
	result = ps_madd(paired_add(sub[0], sub[1]), scalar, result);
	psq_st(result,0,dst1,0,4);
	
	result = psq_lux(dst1,stride,0,4);
	result = ps_madd(paired_sub(sub[0], sub[1]), scalar, result);
	psq_st(result,0,dst1,0,4);
	
	result = psq_lux(dst1,stride,0,4);
	result = ps_madd(paired_sub(add[0], add[1]), scalar, result);
	psq_stx(result,0,dst1,0,4);
	
	add[0] = paired_add(element[1], element[5]);
	sub[0] = paired_sub(element[1], element[5]);
	
	sub[1] = ps_msub(element[3], half, element[7]);
	add[1] = ps_madd(element[7], half, element[3]);
	
	result = psq_lu(2,dst2,0,4);
	result = ps_madd(paired_add(add[0], add[1]), scalar, result);
	psq_st(result,0,dst2,0,4);
	
	result = psq_lux(dst2,stride,0,4);
	result = ps_madd(paired_add(sub[0], sub[1]), scalar, result);
	psq_st(result,0,dst2,0,4);
	
	result = psq_lux(dst2,stride,0,4);
	result = ps_madd(paired_sub(sub[0], sub[1]), scalar, result);
	psq_st(result,0,dst2,0,4);
	
	result = psq_lux(dst2,stride,0,4);
	result = ps_madd(paired_sub(add[0], add[1]), scalar, result);
	psq_st(result,0,dst2,0,4);
}

static void ff_h264_idct_dc_add_paired(uint8_t *dst, DCTELEM *block, int stride)
{
	const float half = 0.5;
	const float scalar = 0.015625;
	vector float pair, offset;
	
	offset = psq_l(0,block,1,7);
	asm("fmadds %0,%0,%1,%2" : "+f"(offset) : "f"(scalar), "f"(half));
	
	dst -= stride;
	
	for (int i=0; i<4; i++) {
		pair = psq_lux(dst,stride,0,4);
		pair = paired_add(pair, offset);
		psq_st(pair,0,dst,0,4);
		
		pair = psq_l(2,dst,0,4);
		pair = paired_add(pair, offset);
		psq_st(pair,2,dst,0,4);
	}
}

static void ff_h264_idct_add16_paired(uint8_t *dst, const int *block_offset, DCTELEM *block, int stride, const uint8_t nnzc[48])
{
	for (int i=0; i<16; i++) {
		int nnz = nnzc[scan8[i]];
		if(nnz) {
			if (nnz==1 && block[i*16])
				ff_h264_idct_dc_add_paired(dst + block_offset[i], block + i*16, stride);
			else ff_h264_idct_add_paired(dst + block_offset[i], block + i*16, stride);
		}
	}
}

static void ff_h264_idct_add16intra_paired(uint8_t *dst, const int *block_offset, DCTELEM *block, int stride, const uint8_t nnzc[48])
{
	for (int i=0; i<16; i++) {
		if (nnzc[scan8[i]])
			ff_h264_idct_add_paired(dst + block_offset[i], block + i*16, stride);
		else if (block[i*16])
			ff_h264_idct_dc_add_paired(dst + block_offset[i], block + i*16, stride);
	}
}

static void ff_h264_idct_add8_paired(uint8_t **dest, const int *block_offset, DCTELEM *block, int stride, const uint8_t nnzc[48])
{
	for (int i=16; i<16+8; i++) {
		if (nnzc[scan8[i]])
			ff_h264_idct_add_paired(dest[(i&4)>>2] + block_offset[i], block + i*16, stride);
		else if (block[i*16])
			ff_h264_idct_dc_add_paired(dest[(i&4)>>2] + block_offset[i], block + i*16, stride);
	}
}

#if 0
// FIXME: Rounding errors.
static void ff_h264_idct8_add_paired(uint8_t *dst, DCTELEM *block, int stride)
{
	const float half = 0.5;
	const float quarter = 0.25;
	const float scalar = 0.015625;
	vector float pair[4], sub[4], add[4];
	vector float result, pre[4];
	
	block[0] += 33;
	block -= 2;
	
	float element[64];
	int16_t *base[2] = {element-16,element-2};
	
	int i;
	for (i=0; i<4; i++) {
		pair[0] = psq_lu(4,block,0,7);
		pair[1] = psq_l(64,block,0,7);
		pre[0] = paired_add(pair[0], pair[1]);
		pre[1] = paired_sub(pair[0], pair[1]);
		
		pair[0] = psq_l(32,block,0,7);
		pair[1] = psq_l(96,block,0,7);
		pre[2] = ps_msub(pair[0], half, pair[1]);
		pre[3] = ps_madd(pair[1], half, pair[0]);
		
		add[0] = paired_add(pre[0], pre[3]);
		add[1] = paired_add(pre[1], pre[2]);
		sub[0] = paired_sub(pre[1], pre[2]);
		sub[1] = paired_sub(pre[0], pre[3]);
		
		pair[0] = psq_l(16,block,0,7);
		pair[1] = psq_l(48,block,0,7);
		pair[2] = psq_l(80,block,0,7);
		pair[3] = psq_l(112,block,0,7);
		
		pre[0] = paired_sub(paired_sub(pair[2], pair[1]), ps_madd(pair[3], half, pair[3]));
		pre[1] = paired_sub(paired_add(pair[0], pair[3]), ps_madd(pair[1], half, pair[1]));
		pre[2] = paired_add(paired_sub(pair[3], pair[0]), ps_madd(pair[2], half, pair[2]));
		pre[3] = paired_add(paired_add(pair[1], pair[2]), ps_madd(pair[0], half, pair[0]));
		
		add[2] = ps_madd(pre[3], quarter, pre[0]);
		add[3] = ps_madd(pre[2], quarter, pre[1]);
		sub[2] = ps_msub(pre[1], quarter, pre[2]);
		sub[3] = paired_sub(pre[3], ps_mul(pre[0], quarter));
		
		pair[0] = paired_add(add[0], sub[3]);
		pair[1] = paired_add(add[1], sub[2]);
		
		psq_stu(paired_merge00(pair[0], pair[1]),64,base[0],0,0);
		psq_st(paired_merge11(pair[0], pair[1]),32,base[0],0,0);
		
		pair[0] = paired_add(sub[0], add[3]);
		pair[1] = paired_add(sub[1], add[2]);
		
		psq_st(paired_merge00(pair[0], pair[1]),8,base[0],0,0);
		psq_st(paired_merge11(pair[0], pair[1]),40,base[0],0,0);
		
		pair[0] = paired_sub(sub[1], add[2]);
		pair[1] = paired_sub(sub[0], add[3]);
		
		psq_st(paired_merge00(pair[0], pair[1]),16,base[0],0,0);
		psq_st(paired_merge11(pair[0], pair[1]),48,base[0],0,0);
		
		pair[0] = paired_sub(add[1], sub[2]);
		pair[1] = paired_sub(add[0], sub[3]);
		
		psq_st(paired_merge00(pair[0], pair[1]),24,base[0],0,0);
		psq_st(paired_merge11(pair[0], pair[1]),56,base[0],0,0);
	}
	
	uint8_t *in_col = dst-2;
	
	for (i=0; i<4; i++) {
		pair[0] = psq_lu(8,base[1],0,0);
		pair[1] = psq_l(128,base[1],0,0);
		pre[0] = paired_add(pair[0], pair[1]);
		pre[1] = paired_sub(pair[0], pair[1]);
		
		pair[0] = psq_l(64,base[1],0,0);
		pair[1] = psq_l(192,base[1],0,0);
		pre[2] = ps_msub(pair[0], half, pair[1]);
		pre[3] = ps_madd(pair[1], half, pair[0]);
		
		add[0] = paired_add(pre[0], pre[3]);
		add[1] = paired_add(pre[1], pre[2]);
		sub[0] = paired_sub(pre[1], pre[2]);
		sub[1] = paired_sub(pre[0], pre[3]);
		
		pair[0] = psq_l(32,base[1],0,0);
		pair[1] = psq_l(96,base[1],0,0);
		pair[2] = psq_l(160,base[1],0,0);
		pair[3] = psq_l(224,base[1],0,0);
		
		pre[0] = paired_sub(paired_sub(pair[2], pair[1]), ps_madd(pair[3], half, pair[3]));
		pre[1] = paired_sub(paired_add(pair[0], pair[3]), ps_madd(pair[1], half, pair[1]));
		pre[2] = paired_add(paired_sub(pair[3], pair[0]), ps_madd(pair[2], half, pair[2]));
		pre[3] = paired_add(paired_add(pair[1], pair[2]), ps_madd(pair[0], half, pair[0]));
		
		add[2] = ps_madd(pre[3], quarter, pre[0]);
		add[3] = ps_madd(pre[2], quarter, pre[1]);
		sub[2] = ps_msub(pre[1], quarter, pre[2]);
		sub[3] = paired_sub(pre[3], ps_mul(pre[0], quarter));
		
		result = psq_lu(2,in_col,0,4);
		result = ps_madd(paired_add(add[0], sub[3]), scalar, result);
		psq_st(result,0,in_col,0,4);
		uint8_t *in_row = in_col;
		
		result = psq_lux(in_row,stride,0,4);
		result = ps_madd(paired_add(add[1], sub[2]), scalar, result);
		psq_st(result,0,in_row,0,4);
		
		result = psq_lux(in_row,stride,0,4);
		result = ps_madd(paired_add(sub[0], add[3]), scalar, result);
		psq_st(result,0,in_row,0,4);
		
		result = psq_lux(in_row,stride,0,4);
		result = ps_madd(paired_add(sub[1], add[2]), scalar, result);
		psq_st(result,0,in_row,0,4);
		
		result = psq_lux(in_row,stride,0,4);
		result = ps_madd(paired_sub(sub[1], add[2]), scalar, result);
		psq_st(result,0,in_row,0,4);
		
		result = psq_lux(in_row,stride,0,4);
		result = ps_madd(paired_sub(sub[0], add[3]), scalar, result);
		psq_st(result,0,in_row,0,4);
		
		result = psq_lux(in_row,stride,0,4);
		result = ps_madd(paired_sub(add[1], sub[2]), scalar, result);
		psq_st(result,0,in_row,0,4);
		
		result = psq_lux(in_row,stride,0,4);
		result = ps_madd(paired_sub(add[0], sub[3]), scalar, result);
		psq_st(result,0,in_row,0,4);
	}
}

static void ff_h264_idct8_dc_add_paired(uint8_t *dst, DCTELEM *block, int stride)
{
	const float half = 0.5;
	const float scalar = 0.015625;
	vector float pair, offset;
	
	offset = psq_l(0,block,1,7);
	asm("fmadds %0,%0,%1,%2" : "+f"(offset) : "f"(scalar), "f"(half));
	
	dst -= stride;
	
	for (int i=0; i<8; i++) {
		pair = psq_lux(dst,stride,0,4);
		pair = paired_add(pair, offset);
		psq_st(pair,0,dst,0,4);
		
		pair = psq_l(2,dst,0,4);
		pair = paired_add(pair, offset);
		psq_st(pair,2,dst,0,4);
		
		pair = psq_l(4,dst,0,4);
		pair = paired_add(pair, offset);
		psq_st(pair,4,dst,0,4);
		
		pair = psq_l(6,dst,0,4);
		pair = paired_add(pair, offset);
		psq_st(pair,6,dst,0,4);
	}
}

static void ff_h264_idct8_add4_paired(uint8_t *dst, const int *block_offset, DCTELEM *block, int stride, const uint8_t nnzc[48])
{
	for (int i=0; i<16; i+=4) {
		int nnz = nnzc[scan8[i]];
		if (nnz) {
			if (nnz==1 && block[i*16])
				ff_h264_idct8_dc_add_paired(dst + block_offset[i], block + i*16, stride);
			else ff_h264_idct8_add_paired(dst + block_offset[i], block + i*16, stride);
		}
	}
}
#endif

#define H264_WEIGHT(W,H) \
static void weight_h264_pixels ## W ## x ## H ## _paired(uint8_t *block, int stride, int log2_denom, int weight, int offset) \
{ \
	int power = 1<<log2_denom; \
	 \
	float weightf = (double)weight/(double)power; \
	vector float offsetf = {offset+0.5,offset+0.5}; \
	vector float pair; \
	 \
	block -= stride; \
	 \
	for (int y=0; y<H; y++) { \
		pair = psq_lux(block,stride,0,4); \
		pair = ps_madds0(pair, weightf, offsetf); \
		psq_st(pair,0,block,0,4); \
		 \
		if (W == 2) continue; \
		 \
		pair = psq_l(2,block,0,4); \
		pair = ps_madds0(pair, weightf, offsetf); \
		psq_st(pair,2,block,0,4); \
		 \
		if (W == 4) continue; \
		 \
		pair = psq_l(4,block,0,4); \
		pair = ps_madds0(pair, weightf, offsetf); \
		psq_st(pair,4,block,0,4); \
		 \
		pair = psq_l(6,block,0,4); \
		pair = ps_madds0(pair, weightf, offsetf); \
		psq_st(pair,6,block,0,4); \
		 \
		if (W == 8) continue; \
		 \
		pair = psq_l(8,block,0,4); \
		pair = ps_madds0(pair, weightf, offsetf); \
		psq_st(pair,8,block,0,4); \
		 \
		pair = psq_l(10,block,0,4); \
		pair = ps_madds0(pair, weightf, offsetf); \
		psq_st(pair,10,block,0,4); \
		 \
		pair = psq_l(12,block,0,4); \
		pair = ps_madds0(pair, weightf, offsetf); \
		psq_st(pair,12,block,0,4); \
		 \
		pair = psq_l(14,block,0,4); \
		pair = ps_madds0(pair, weightf, offsetf); \
		psq_st(pair,14,block,0,4); \
	} \
} \
static void biweight_h264_pixels ## W ## x ## H ## _paired(uint8_t *dst, uint8_t *src, int stride, int log2_denom, int weightd, int weights, int offset) \
{ \
	int power = 1<<log2_denom+1; \
	 \
	float weightdf = (double)weightd/(double)power; \
	float weightsf = (double)weights/(double)power; \
	vector float offsetf = {offset+0.5,offset+0.5}; \
	vector float pair[2]; \
	 \
	dst -= stride; \
	src -= stride; \
	 \
	for (int y=0; y<H; y++) { \
		pair[0] = psq_lux(dst,stride,0,4); \
		pair[1] = psq_lux(src,stride,0,4); \
		pair[0] = ps_madds0(pair[1], weightsf, ps_madds0(pair[0], weightdf, offsetf)); \
		psq_st(pair[0],0,dst,0,4); \
		 \
		if (W == 2) continue; \
		 \
		pair[0] = psq_l(2,dst,0,4); \
		pair[1] = psq_l(2,src,0,4); \
		pair[0] = ps_madds0(pair[1], weightsf, ps_madds0(pair[0], weightdf, offsetf)); \
		psq_st(pair[0],2,dst,0,4); \
		 \
		if (W == 4) continue; \
		 \
		pair[0] = psq_l(4,dst,0,4); \
		pair[1] = psq_l(4,src,0,4); \
		pair[0] = ps_madds0(pair[1], weightsf, ps_madds0(pair[0], weightdf, offsetf)); \
		psq_st(pair[0],4,dst,0,4); \
		 \
		pair[0] = psq_l(6,dst,0,4); \
		pair[1] = psq_l(6,src,0,4); \
		pair[0] = ps_madds0(pair[1], weightsf, ps_madds0(pair[0], weightdf, offsetf)); \
		psq_st(pair[0],6,dst,0,4); \
		 \
		if (W == 8) continue; \
		 \
		pair[0] = psq_l(8,dst,0,4); \
		pair[1] = psq_l(8,src,0,4); \
		pair[0] = ps_madds0(pair[1], weightsf, ps_madds0(pair[0], weightdf, offsetf)); \
		psq_st(pair[0],8,dst,0,4); \
		 \
		pair[0] = psq_l(10,dst,0,4); \
		pair[1] = psq_l(10,src,0,4); \
		pair[0] = ps_madds0(pair[1], weightsf, ps_madds0(pair[0], weightdf, offsetf)); \
		psq_st(pair[0],10,dst,0,4); \
		 \
		pair[0] = psq_l(12,dst,0,4); \
		pair[1] = psq_l(12,src,0,4); \
		pair[0] = ps_madds0(pair[1], weightsf, ps_madds0(pair[0], weightdf, offsetf)); \
		psq_st(pair[0],12,dst,0,4); \
		 \
		pair[0] = psq_l(14,dst,0,4); \
		pair[1] = psq_l(14,src,0,4); \
		pair[0] = ps_madds0(pair[1], weightsf, ps_madds0(pair[0], weightdf, offsetf)); \
		psq_st(pair[0],14,dst,0,4); \
	} \
}

H264_WEIGHT(16,16)
H264_WEIGHT(16,8)
H264_WEIGHT(8,16)
H264_WEIGHT(8,8)
H264_WEIGHT(8,4)
H264_WEIGHT(4,8)
H264_WEIGHT(4,4)
H264_WEIGHT(4,2)
H264_WEIGHT(2,4)
H264_WEIGHT(2,2)

void dsputil_h264_init_ppc(DSPContext *c, AVCodecContext *avctx)
{
	c->put_h264_chroma_pixels_tab[0] = put_h264_chroma_mc8_paired;
	c->put_h264_chroma_pixels_tab[1] = put_h264_chroma_mc4_paired;
	c->avg_h264_chroma_pixels_tab[0] = avg_h264_chroma_mc8_paired;
	c->avg_h264_chroma_pixels_tab[1] = avg_h264_chroma_mc4_paired;
	c->put_no_rnd_vc1_chroma_pixels_tab[0] = put_no_rnd_vc1_chroma_mc8_paired;
	c->avg_no_rnd_vc1_chroma_pixels_tab[0] = avg_no_rnd_vc1_chroma_mc8_paired;
}

void ff_h264dsp_init_ppc(H264DSPContext *c)
{
	c->h264_idct_add = ff_h264_idct_add_paired;
	c->h264_idct_add8 = ff_h264_idct_add8_paired;
	c->h264_idct_add16 = ff_h264_idct_add16_paired;
	c->h264_idct_add16intra = ff_h264_idct_add16intra_paired;
	c->h264_idct_dc_add = ff_h264_idct_dc_add_paired;
	/* c->h264_idct8_dc_add = ff_h264_idct8_dc_add_paired;
	c->h264_idct8_add = ff_h264_idct8_add_paired;
	c->h264_idct8_add4 = ff_h264_idct8_add4_paired; */
	
	c->weight_h264_pixels_tab[0] = weight_h264_pixels16x16_paired;
	c->weight_h264_pixels_tab[1] = weight_h264_pixels16x8_paired;
	c->weight_h264_pixels_tab[2] = weight_h264_pixels8x16_paired;
	c->weight_h264_pixels_tab[3] = weight_h264_pixels8x8_paired;
	c->weight_h264_pixels_tab[4] = weight_h264_pixels8x4_paired;
	c->weight_h264_pixels_tab[5] = weight_h264_pixels4x8_paired;
	c->weight_h264_pixels_tab[6] = weight_h264_pixels4x4_paired;
	c->weight_h264_pixels_tab[7] = weight_h264_pixels4x2_paired;
	c->weight_h264_pixels_tab[8] = weight_h264_pixels2x4_paired;
	c->weight_h264_pixels_tab[9] = weight_h264_pixels2x2_paired;
	c->biweight_h264_pixels_tab[0] = biweight_h264_pixels16x16_paired;
	c->biweight_h264_pixels_tab[1] = biweight_h264_pixels16x8_paired;
	c->biweight_h264_pixels_tab[2] = biweight_h264_pixels8x16_paired;
	c->biweight_h264_pixels_tab[3] = biweight_h264_pixels8x8_paired;
	c->biweight_h264_pixels_tab[4] = biweight_h264_pixels8x4_paired;
	c->biweight_h264_pixels_tab[5] = biweight_h264_pixels4x8_paired;
	c->biweight_h264_pixels_tab[6] = biweight_h264_pixels4x4_paired;
	c->biweight_h264_pixels_tab[7] = biweight_h264_pixels4x2_paired;
	c->biweight_h264_pixels_tab[8] = biweight_h264_pixels2x4_paired;
	c->biweight_h264_pixels_tab[9] = biweight_h264_pixels2x2_paired;
}
