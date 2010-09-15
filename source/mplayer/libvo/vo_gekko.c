/*
   vo_gekko.c - MPlayer video driver for Wii

   Copyright (C) 2008 dhewg
   Copyright (C) 2008 sepp256 - YUV frame rendering functions

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the
   Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301 USA.
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <malloc.h>

#include "config.h"
#include "mp_msg.h"
#include "subopt-helper.h"
#include "help_mp.h"
#include "video_out.h"
#include "video_out_internal.h"
#include "aspect.h"
#include "mp_fifo.h"
#include "osd.h"
#include "sub.h"
#include "osdep/keycodes.h"
#include "osdep/gx_supp.h"
#include "fastmemcpy.h"

#include <gccore.h>

extern int mplayerwidth;
extern int mplayerheight;

static const vo_info_t info = {
	"gekko video output",
	"gekko",
	"Team Twiizers",
	""
};

const LIBVO_EXTERN (gekko)

static u16 pitch[3];
static u32 image_width = 0, image_height = 0;

static u32 gx_width, gx_height;

void vo_draw_alpha_gekko(int w, int h, unsigned char* src, unsigned char *srca,
		int srcstride, unsigned char* dstbase, int dststride, int x0)
		{
	// can be optimized
	int x,y;
	unsigned char* buf, *bufa, *tmp, *tmpa;
	int buf_st;
	int h1, w1, Yrowpitch, df1;

	u8 *dst, *srca1, *src1, *srca2, *src2, *srca3, *src3, *srca4, *src4;

	getStrideInfo(&w1, &df1, &Yrowpitch);
	Yrowpitch = Yrowpitch * 8;
	df1 = df1 * 8;

	h1 = ((h / 8.0) + 0.5) * 8;
	buf = malloc(dststride * h1);
	bufa = malloc(dststride * h1);

	memset(buf, 0, dststride * h1);
	memset(bufa, 0, dststride * h1);

	//	buf_st=(dststride-srcstride)/2; //center
	//	buf_st=0; //align to left
	buf_st = x0; // original pos
	tmp = buf + buf_st;
	tmpa = bufa + buf_st;

	for (y = 0; y < h; y++)
	{
		memcpy(tmp, src, w);
		memcpy(tmpa, srca, w);
		src += srcstride;
		srca += srcstride;
		tmp += dststride;
		tmpa += dststride;
	}

	src = buf;
	srca = bufa;
	h1 = h / 4;

	dst = dstbase;
	srca1 = srca;
	src1 = src;
	srca2 = srca + dststride;
	src2 = src + dststride;
	srca3 = srca + dststride * 2;
	src3 = src + dststride * 2;
	srca4 = srca + dststride * 3;
	src4 = src + dststride * 3;
	for (y = 0; y < h1; y++)
	{
		for (w = 0; w < w1; w++)
		{
			for (x = 0; x < 8; x++)
			{
				if (*srca1)
					*dst = (((*dst) * (*srca1)) >> 8) + (*src1);
				dst++;
				srca1++;
				src1++;
			}
			for (x = 0; x < 8; x++)
			{
				if (*srca2)
					*dst = (((*dst) * (*srca2)) >> 8) + (*src2);
				dst++;
				srca2++;
				src2++;
			}
			for (x = 0; x < 8; x++)
			{
				if (*srca3)
					*dst = (((*dst) * (*srca3)) >> 8) + (*src3);
				dst++;
				srca3++;
				src3++;
			}
			for (x = 0; x < 8; x++)
			{
				if (*srca4)
					*dst = (((*dst) * (*srca4)) >> 8) + (*src4);
				dst++;
				srca4++;
				src4++;
			}
		}
		dst += df1;
		srca1 += Yrowpitch;
		src1 += Yrowpitch;
		srca2 += Yrowpitch;
		src2 += Yrowpitch;
		srca3 += Yrowpitch;
		src3 += Yrowpitch;
		srca4 += Yrowpitch;
		src4 += Yrowpitch;
	}
	free(buf);
	free(bufa);
}

static void draw_alpha(int x0, int y0, int w, int h, unsigned char *src, unsigned char *srca, int stride)
{
	int p = image_width / 16;
	if (p % 2) p++;
	p = p * 16;
	y0 = ((int) (y0 / 8.0)) * 8;
	y0-=8;
	vo_draw_alpha_gekko(w, h, src, srca, stride, GetYtexture() + (y0 * p), pitch[0], x0);
}

static int draw_slice(uint8_t *image[], int stride[], int w, int h, int x, int y)
{
	if (y == 0)
	{
		GX_ResetTextureYUVPointers();
		
		if (stride[0] != pitch[0])
		{
			pitch[0] = stride[0];
			pitch[1] = stride[1];
			pitch[2] = stride[2];
			
			GX_UpdatePitch(pitch);
		}
	}
	
	GX_FillTextureYUV(h, image);
	return 0;
}

static void draw_osd(void)
{
	vo_draw_text(image_width, image_height, draw_alpha);
}

static void flip_page(void)
{
	GX_RenderTexture();
}

static int draw_frame(uint8_t *src[])
{
	return VO_ERROR;
}

static int inline query_format(uint32_t format)
{
	switch (format)
	{
		case IMGFMT_YV12:
			return VFCAP_CSP_SUPPORTED | VFCAP_CSP_SUPPORTED_BY_HW
					| VFCAP_HWSCALE_UP | VFCAP_HWSCALE_DOWN
					| VFCAP_ACCEPT_STRIDE;
		default:
			return VO_FALSE;
	}
}

void reinit_video()
{
	GX_StartYUV(image_width, image_height, gx_width / 2, gx_height / 2 ); 
	GX_ConfigTextureYUV(image_width, image_height, pitch);	
	Set_vsync(vo_vsync);
}

static int config(uint32_t width, uint32_t height, uint32_t d_width,
          uint32_t d_height, uint32_t flags, char *title,
          uint32_t format)
{
	image_width = width;
	image_height = height;

	pitch[0] = 0;
	pitch[1] = 0;
	pitch[2] = 0;
	
	float screen_aspect = (float)mplayerwidth / (float)mplayerheight;
	float image_aspect = (float)d_width / (float)d_height;

	if (image_aspect > screen_aspect)
	{
		gx_width = mplayerwidth;
		gx_height = (f32)d_height * ((f32)mplayerwidth / (f32)d_width);
	}
	else
	{
		gx_width = (f32)d_width * ((f32)mplayerheight / (f32)d_height);
		gx_height = mplayerheight;
	}
	reinit_video();
	return 0;
}

static void uninit(void)
{
	image_width = 0;
	image_height = 0;
}

static void check_events(void)
{
}

static int preinit(const char *arg)
{
	if (CONF_GetAspectRatio() == CONF_ASPECT_16_9)
		mplayerwidth = 854; // 480 * (16/9)

	vo_screenheight = mplayerheight;
	vo_screenwidth = mplayerwidth;
	vo_fs=1;

	ShutdownGui(); // tell GUI to shut down, MPlayer is ready to take over
	SetMPlayerSettings(); // pass settings from WiiMC into MPlayer
	
	return 0;
}

static int control(uint32_t request, void *data, ...)
{
	switch (request)
	{
		case VOCTRL_QUERY_FORMAT:
			return query_format(*((uint32_t *)data));
		case VOCTRL_UPDATE_SCREENINFO:
            vo_screenwidth = mplayerwidth;
            vo_screenheight = mplayerheight;
            aspect_save_screenres(vo_screenwidth, vo_screenheight);
            return VO_TRUE;
		default:
			return VO_NOTIMPL;
	}
}
