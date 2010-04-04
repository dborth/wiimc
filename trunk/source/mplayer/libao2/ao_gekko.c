/*
   ao_gekko.c - MPlayer audio driver for Wii

   Copyright (C) 2008 dhewg
   Improved by Tantric, rodries and Extrems

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

#include "config.h"
#include "libaf/af.h"
#include "libaf/af_format.h"
#include "audio_out.h"
#include "audio_out_internal.h"
#include "mp_msg.h"
#include "help_mp.h"

#include <ogcsys.h>
#include "osdep/ave-rvl.h"


#define BUFFER_SIZE (4 * 1024)
#define BUFFER_COUNT 64
#define PREBUFFER 32768


static ao_info_t info = {
	"gekko audio output",
	"gekko",
	"Team Twiizers",
	""
};

LIBAO_EXTERN(gekko)

static u8 buffers[BUFFER_COUNT][BUFFER_SIZE] ATTRIBUTE_ALIGN(32);
static u8 buffer_fill = 0;
static u8 buffer_play = 0;
static int buffered = 0;

static bool playing = false;

static ao_control_vol_t volume = { 0x8E, 0x8E };


static void switch_buffers()
{
	if (buffered > 0)
	{
		buffered -= BUFFER_SIZE;
		buffer_play = (buffer_play + 1) % BUFFER_COUNT;
		
		AUDIO_InitDMA((u32)buffers[buffer_play], BUFFER_SIZE);
		AUDIO_StartDMA();
	}
	else
	{
		playing = false;
		AUDIO_StopDMA();
	}
}

static int control(int cmd, void *arg)
{
	switch (cmd)
	{
		case AOCONTROL_QUERY_FORMAT:
			return CONTROL_TRUE;
		case AOCONTROL_GET_VOLUME:
		case AOCONTROL_SET_VOLUME:
		{
			ao_control_vol_t *vol = (ao_control_vol_t *)arg;
			
			if (cmd == AOCONTROL_SET_VOLUME)
			{
				volume.left = (vol->left / 100.0) * 0xFF;
				volume.right = (vol->right / 100.0) * 0xFF;
				
				VIWriteI2CRegister8(AVE_AI_VOLUME, clamp(volume.left, 0x00, 0xFF));
				VIWriteI2CRegister8(AVE_AI_VOLUME + 1, clamp(volume.right, 0x00, 0xFF));
			}
			
			vol->left = (volume.left / 0xFF) * 100.0;
			vol->right = (volume.right / 0xFF) * 100.0;
			
			return CONTROL_OK;
		}
		default:
			return CONTROL_UNKNOWN;
	}
}

static u8 quality = AI_SAMPLERATE_48KHZ;

void reinit_audio()  // for newgui
{
	AUDIO_SetDSPSampleRate(quality);
	AUDIO_RegisterDMACallback(switch_buffers);
}

static int init(int rate, int channels, int format, int flags)
{
	quality = AI_SAMPLERATE_32KHZ;
	ao_data.samplerate = 32000;

	if(rate > 32000)
	{
		quality = AI_SAMPLERATE_48KHZ;
		ao_data.samplerate = 48000;
	}

	ao_data.channels = 2;
	ao_data.format = AF_FORMAT_S16_NE;
	ao_data.bps = ao_data.channels * ao_data.samplerate * 2;
	ao_data.buffersize = BUFFER_SIZE * BUFFER_COUNT;
	ao_data.outburst = BUFFER_SIZE;
	
	for (int counter = 0; counter < BUFFER_COUNT; counter++)
	{
		memset(buffers[counter], 0, BUFFER_SIZE);
		DCFlushRange(buffers[counter], BUFFER_SIZE);
	}

	buffer_fill = 0;
	buffer_play = 0;
	buffered = 0;

	playing = false;

	AUDIO_Init(NULL);
	AUDIO_SetDSPSampleRate(quality);
	AUDIO_RegisterDMACallback(switch_buffers);

	return CONTROL_TRUE;
}

static void reset(void)
{
	AUDIO_StopDMA();
	
	for (int counter = 0; counter < BUFFER_COUNT; counter++)
	{
		memset(buffers[counter], 0, BUFFER_SIZE);
		DCFlushRange(buffers[counter], BUFFER_SIZE);
	}
	
	AUDIO_RegisterDMACallback(NULL);
	AUDIO_InitDMA((u32)buffers[0], 32);
	AUDIO_StartDMA();
	
	usleep(100);
	
	while (AUDIO_GetDMABytesLeft() > 0)
		usleep(100);
	
	AUDIO_StopDMA();
	AUDIO_RegisterDMACallback(switch_buffers);
	
	buffer_fill = 0;
	buffer_play = 0;
	buffered = 0;
	
	playing = false;
}

static void uninit(int immed)
{
	reset();
	AUDIO_RegisterDMACallback(NULL);
}

static void audio_pause(void)
{
	AUDIO_StopDMA();
	playing = false;
}

static void audio_resume(void)
{
	playing = true;
	switch_buffers();
}

static int get_space(void)
{
	return (BUFFER_SIZE * (BUFFER_COUNT - 1)) - buffered;
}

#define SWAP(x) ((x >> 16) | (x << 16))
#define SWAP_LEN (BUFFER_SIZE / 4)

static void copy_swap_channels(u32 *destination, u32 *source)
{
	for (int counter = 0; counter < SWAP_LEN; counter++)
		destination[counter] = SWAP(source[counter]);
}

static int play(void *data, int len, int flags)
{
	int result = 0;
	u8 *source = (u8 *)data;
	
	while ((len >= BUFFER_SIZE)	&& (get_space() >= BUFFER_SIZE))
	{
		copy_swap_channels((u32 *)buffers[buffer_fill], (u32 *)source);
		DCFlushRange(buffers[buffer_play], BUFFER_SIZE);

		buffer_fill = (buffer_fill + 1) % BUFFER_COUNT;
		
		result += BUFFER_SIZE;
		source += BUFFER_SIZE;
		buffered += BUFFER_SIZE;
		
		len -= BUFFER_SIZE;
	}
	
	if (!playing && (buffered >= PREBUFFER))
	{
		playing = true;
		switch_buffers();
	}
	
	return result;
}

static float get_delay(void)
{
	if (playing)
		return (float)(buffered + AUDIO_GetDMABytesLeft()) / ao_data.bps;
	else
		return (float)buffered / ao_data.bps;
}
