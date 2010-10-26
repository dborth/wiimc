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

void ShutdownGui();
void SetMPlayerSettings();

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

u32 gx_width, gx_height;

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
	vo_draw_text(image_width, image_height, vo_draw_alpha_gekko);
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
	ShutdownGui(); // tell GUI to shut down, MPlayer is ready to take over
	SetMPlayerSettings(); // pass settings from WiiMC into MPlayer

	GX_StartYUV(image_width, image_height, gx_width / 2, gx_height / 2 ); 
	GX_ConfigTextureYUV(image_width, image_height, pitch);	
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
	
	vo_screenheight = image_height;
	vo_screenwidth = image_width;
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

	return 0;
}

static int control(uint32_t request, void *data, ...)
{
	switch (request)
	{
		case VOCTRL_QUERY_FORMAT:
			return query_format(*((uint32_t *)data));
		case VOCTRL_UPDATE_SCREENINFO:
            vo_screenwidth = image_width;
            vo_screenheight = image_height;
            aspect_save_screenres(vo_screenwidth, vo_screenheight);
            return VO_TRUE;
		default:
			return VO_NOTIMPL;
	}
}
