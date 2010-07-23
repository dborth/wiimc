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
#include <limits.h>

#include "config.h"
#include "libaf/af.h"
#include "libaf/af_format.h"
#include "audio_out.h"
#include "audio_out_internal.h"
#include "mp_msg.h"
#include "help_mp.h"

#include <ogcsys.h>
#include "osdep/ave-rvl.h"

#define BUFFER_SIZE 	(4 * 1024)
#define BUFFER_COUNT 	64
#define PREBUFFER 		32768

#define HW_CHANNELS 	2

#define PAN_CENTER 		0.7071067811865475f		// sqrt(1/2)
#define PAN_SIDE 		0.816496580927726f		// sqrt(2/3)
#define PAN_SIDE_INV 	0.5773502691896258f		// sqrt(1/3)

#define PHASE_SHF 		0.25					// "90 degrees"
#define PHASE_SHF_INV 	0.75

static const ao_info_t info = {
	"gekko audio output",
	"gekko",
	"Team Twiizers",
	""
};

LIBAO_EXTERN(gekko)

static u8 buffers[BUFFER_COUNT][BUFFER_SIZE] ATTRIBUTE_ALIGN(32);
static u8 silence[BUFFER_SIZE] ATTRIBUTE_ALIGN(32);
static u8 buffer_fill = 0;
static u8 buffer_play = 0;
static int buffered = 0;

static float request_mult = 1.0;
static int request_size = BUFFER_SIZE;

static bool playing = false;

static ao_control_vol_t volume = { 0x8E, 0x8E };


static void switch_buffers()
{
	if (playing && buffered > 0)
	{
		AUDIO_InitDMA((u32)buffers[buffer_play], BUFFER_SIZE);
		buffer_play = (buffer_play + 1) % BUFFER_COUNT;
		buffered -= BUFFER_SIZE;
	}
	else
	{
		AUDIO_InitDMA((u32)silence, BUFFER_SIZE);
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

void reinit_audio()
{
	//AUDIO_SetDSPSampleRate(quality);
	//AUDIO_RegisterDMACallback(switch_buffers);
	if(!playing)
	{
		switch_buffers();
		AUDIO_StartDMA();
	}
}

static int init(int rate, int channels, int format, int flags)
{
	ao_data.samplerate = 48000;
	ao_data.channels = clamp(channels, 2, 6);
	ao_data.format = AF_FORMAT_S16_NE;
	ao_data.bps = ao_data.channels * ao_data.samplerate * sizeof(s16);
	request_mult = (float)ao_data.channels / HW_CHANNELS;
	request_size = BUFFER_SIZE * request_mult;
	ao_data.buffersize = request_size * BUFFER_COUNT;
	ao_data.outburst = request_size;
	
	for (int counter = 0; counter < BUFFER_COUNT; counter++)
	{
		memset(buffers[counter], 0, BUFFER_SIZE);
		DCFlushRange(buffers[counter], BUFFER_SIZE);
	}

	memset(silence, 0, BUFFER_SIZE);
	DCFlushRange(silence, BUFFER_SIZE);

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
	//while(playing && (buffered > 0)) usleep(100);
	playing = false;

	AUDIO_StopDMA();

	buffer_fill = 0;
	buffer_play = 0;
	buffered = 0;
	for (int counter = 0; counter < BUFFER_COUNT; counter++)
	{
		memset(buffers[counter], 0, BUFFER_SIZE);
		DCFlushRange(buffers[counter], BUFFER_SIZE);
	}
}

static void uninit(int immed)
{
	//reset();
	//AUDIO_RegisterDMACallback(NULL);
}

static void audio_pause(void)
{
	playing = false;
}

static void audio_resume(void)
{
	playing = true;
}

static int get_space(void)
{
	return ((BUFFER_SIZE * (BUFFER_COUNT - 2)) - buffered) * request_mult;
}

static inline void copy_channels(s16 *dst, s16 *src, int len, int processed, int remaining)
{
	for (int counter = 0; counter < len; ++counter)
	{
		int prs = max((counter - 1), -(processed / (sizeof(s16) * ao_data.channels))) * ao_data.channels;
		int crs = counter * ao_data.channels;	// "I'm surrounded!"
		int nrs = min((counter + 1), (remaining / (sizeof(s16) * ao_data.channels))) * ao_data.channels;
		
		s32 left, right;
		
		if (ao_data.channels > 1)
		{
			left = src[crs];
			right = src[crs + 1];
		}
		else
		{
			left = right = src[crs];
		}
		
		switch (ao_data.channels)
		{
			case 6:
			case 5:
				// Left rear
				left += ((src[crs + 2] * PHASE_SHF_INV) + (src[nrs + 2] * PHASE_SHF)) * PAN_SIDE;
				right += ((src[crs + 2] * PHASE_SHF_INV) + (src[prs + 2] * PHASE_SHF)) * PAN_SIDE_INV;
				
				// Right rear
				left += ((src[crs + 3] * PHASE_SHF_INV) + (src[nrs + 3] * PHASE_SHF)) * PAN_SIDE_INV;
				right += ((src[crs + 3] * PHASE_SHF_INV) + (src[prs + 3] * PHASE_SHF)) * PAN_SIDE;
				
				// Center front
				left += src[crs + 4] * PAN_CENTER;
				right += src[crs + 4] * PAN_CENTER;
				break;
			case 4:
				// Center rear
				left += ((src[crs + 2] * PHASE_SHF_INV) + (src[nrs + 2] * PHASE_SHF)) * PAN_CENTER;
				right += ((src[crs + 2] * PHASE_SHF_INV) + (src[prs + 2] * PHASE_SHF)) * PAN_CENTER;
				
				// Center front
				left += src[crs + 3] * PAN_CENTER;
				right += src[crs + 3] * PAN_CENTER;
				break;
		}
		
		int cws = counter * HW_CHANNELS;
		
		dst[cws] = clamp(right, SHRT_MIN, SHRT_MAX);
		dst[cws + 1] = clamp(left, SHRT_MIN, SHRT_MAX);
	}
}

static int play(void *data, int remaining, int flags)
{
	int processed = 0;
	int samples;
	s16 *source = (s16 *)data;

	while (remaining >= request_size && get_space() >= request_size)
	{
		samples = BUFFER_SIZE / (sizeof(s16) * HW_CHANNELS);
		copy_channels((s16 *)buffers[buffer_fill], source, samples, processed, remaining);
		DCStoreRangeNoSync(buffers[buffer_fill], BUFFER_SIZE);

		buffer_fill = (buffer_fill + 1) % BUFFER_COUNT;

		processed += request_size;
		source += request_size / sizeof(s16);
		buffered += BUFFER_SIZE;

		remaining -= request_size;
	}

	if ((flags & AOPLAY_FINAL_CHUNK) && remaining > 0)
	{
		samples = remaining / (sizeof(s16) * HW_CHANNELS);
		memset(buffers[buffer_fill], 0, BUFFER_SIZE);
		copy_channels((s16 *)buffers[buffer_fill], source, samples, processed, remaining);
		DCStoreRangeNoSync(buffers[buffer_fill], BUFFER_SIZE);
		buffer_fill = (buffer_fill + 1) % BUFFER_COUNT;

		processed += remaining;
		buffered += BUFFER_SIZE;
	}

	if (!playing && buffered > request_size)
	{
		playing = true;
		switch_buffers();
		AUDIO_StartDMA();
	}
	return processed;
}

static float get_delay(void)
{
	if (playing)
		return (float)((buffered + AUDIO_GetDMABytesLeft()) * request_mult) / ao_data.bps;
	else
		return (float)(buffered * request_mult) / ao_data.bps;
}
