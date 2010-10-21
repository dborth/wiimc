/*
   ao_gekko.c - MPlayer audio driver for Wii

   Copyright (C) 2008 dhewg
   Improved by Tantric & rodries

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
#include "libaf/af_format.h"
#include "audio_out.h"
#include "audio_out_internal.h"
#include "mp_msg.h"
#include "help_mp.h"
#include "libaf/af.h"


#include <ogcsys.h>
#include "osdep/ave-rvl.h"


static ao_info_t info = {
	"gekko audio output",
	"gekko",
	"Team Twiizers",
	""
};

LIBAO_EXTERN(gekko)

#define SFX_BUFFER_SIZE (4*1024)
#define SFX_BUFFERS 64
//#define PREBUFFER 65536
#define PREBUFFER 32768

static u8 buffer[SFX_BUFFERS][SFX_BUFFER_SIZE] ATTRIBUTE_ALIGN(32);
static u8 buffer_fill = 0;
static u8 buffer_play = 0;
static bool playing = false;
static int buffered=0;
static ao_control_vol_t volume = { 0x8E, 0x8E };


static void switch_buffers() {

	if (buffered <= 0) {
			//printf("audio stop no data\n");
			playing = false;
			AUDIO_StopDMA();
			return;
	}
	buffered-=SFX_BUFFER_SIZE;
	buffer_play = (buffer_play + 1) % SFX_BUFFERS;
	AUDIO_InitDMA((u32) buffer[buffer_play], SFX_BUFFER_SIZE);
	AUDIO_StartDMA();

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


void reinit_audio()  // for newgui
{
	AUDIO_SetDSPSampleRate(AI_SAMPLERATE_48KHZ);
	AUDIO_RegisterDMACallback(switch_buffers);
}

static int init(int rate, int channels, int format, int flags) {
	u8 i;
	
	AUDIO_StopDMA();
	AUDIO_SetDSPSampleRate(AI_SAMPLERATE_48KHZ);
	AUDIO_RegisterDMACallback(switch_buffers);

	ao_data.buffersize = SFX_BUFFER_SIZE * SFX_BUFFERS;
	ao_data.outburst = SFX_BUFFER_SIZE;
	ao_data.channels = 2;
	ao_data.samplerate = 48000;
	ao_data.format = AF_FORMAT_S16_BE;
	ao_data.bps = 192000;
	
	for (i = 0; i < SFX_BUFFERS; ++i) {
		memset(buffer[i], 0, SFX_BUFFER_SIZE);
		DCFlushRange(buffer[i], SFX_BUFFER_SIZE);
	}

	playing = false;
	buffer_fill = 0;
	buffer_play = 0;
	buffered=0;
	
	return 1;
}

static void reset(void) {
	u8 i;

	AUDIO_StopDMA();

	for (i = 0; i < SFX_BUFFERS; ++i) {
		memset(buffer[i], 0, SFX_BUFFER_SIZE);
		DCFlushRange(buffer[i], SFX_BUFFER_SIZE);
	}

	AUDIO_RegisterDMACallback(NULL);
	AUDIO_InitDMA((u32) buffer[0], 32);
	AUDIO_StartDMA();
	usleep(100);
	while(AUDIO_GetDMABytesLeft()>0) usleep(100);
	AUDIO_StopDMA();
	AUDIO_RegisterDMACallback(switch_buffers);


	buffer_fill = 0;
	buffer_play = 0;
	buffered=0;
	playing = false;
}

static void uninit(int immed) {
	reset();

	AUDIO_RegisterDMACallback(NULL);
}

static void audio_pause(void) {
	AUDIO_StopDMA();
	playing = false;
}

static void audio_resume(void) {
	playing=true;
	switch_buffers();
}

static int get_space(void) {
	return (SFX_BUFFER_SIZE*(SFX_BUFFERS-1))-buffered;
}

#define SWAP(x) ((x>>16)|(x<<16))
#define SWAP_LEN SFX_BUFFER_SIZE/4
static void copy_swap_channels(u32 *d, u32 *s)
{
	int n;
	for(n=0;n<SWAP_LEN;n++) d[n] = SWAP(s[n]);
}

static int play(void* data, int len, int flags) {
	int ret = 0;
	u8 *s = (u8 *) data;

	while ((len >= SFX_BUFFER_SIZE)	&& (get_space()>=SFX_BUFFER_SIZE)) {
		copy_swap_channels((u32*)buffer[buffer_fill], (u32*)s);
		DCFlushRange(buffer[buffer_play], SFX_BUFFER_SIZE);

		buffer_fill = (buffer_fill + 1) % SFX_BUFFERS;
		len -= SFX_BUFFER_SIZE;
		s += SFX_BUFFER_SIZE;
		ret += SFX_BUFFER_SIZE;
		buffered+=SFX_BUFFER_SIZE;
	}
	if (!playing && buffered>=PREBUFFER)
	{
		playing=true;
		switch_buffers();
	}


	return ret;
}

static float get_delay(void) {
	if(playing)
		return (buffered+AUDIO_GetDMABytesLeft()) / 192000.0f;
	else
		return (buffered) / 192000.0f;
}
