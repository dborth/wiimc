/*
MPlayer Wii port

   Copyright (C) WiiMC Team

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

#include "../config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <malloc.h>

/*
#include <ogcsys.h>
#include <ogc/lwp_watchdog.h>
#include <ogc/lwp.h>
#include <debug.h>
#include <wiiuse/wpad.h>

#include <fat.h>
#include <ntfs.h>
#include <smb.h>

#include <network.h>
#include <errno.h>
#include "mp_osd.h"
#include "timer.h"

#include "log_console.h"
#include "gx_supp.h"
*/
#include "plat_gekko.h"


#include "../m_option.h"
#include "../parser-cfg.h"
#include "../get_path.h"

#include "../../utils/di2.h"
#include "../../utils/libdvdiso.h"
#include "../../utils/ftp_devoptab.h"
#include "../../utils/mload.h"
#include "../../utils/ehcmodule_elf.h"

#undef abort

//#define DEBUG_INIT

#ifdef DEBUG_INIT
#define printf_debug(fmt, args...) \
	printf(fmt, ##args)
#else
#define printf_debug(fmt, args...)
#endif // DEBUG_INIT


extern char appPath[1024];
extern int stream_cache_size;

static off_t get_filesize(char *FileName)
{
    struct stat file;
    if(!stat(FileName,&file))
    {
        return file.st_size;
    }
    return 0;
}

bool load_ehci_module()
{
	data_elf my_data_elf;
	off_t fsize;
	void *external_ehcmodule = NULL;
	FILE *fp;
	char file[100];
	
	sprintf(file,"%s/ehcmodule.elf",MPLAYER_DATADIR);	
	
	fp=fopen(file,"rb");
	if(fp!=NULL)
	{
		fsize=get_filesize(file);
		external_ehcmodule= (void *)memalign(32, fsize);
		if(!external_ehcmodule) 
		{
			fclose(fp);
			free(external_ehcmodule); 
			external_ehcmodule=NULL;
		}
		else
		{
			if(fread(external_ehcmodule,1, fsize ,fp)!=fsize)
			{
				free(external_ehcmodule); 
				external_ehcmodule=NULL;
			}
			else mload_elf((void *) external_ehcmodule, &my_data_elf);
			fclose(fp);
		}
		
	}
	else
		mload_elf((void *) ehcmodule_elf, &my_data_elf);

	if(mload_run_thread(my_data_elf.start, my_data_elf.stack, my_data_elf.size_stack, my_data_elf.prio)<0)
	{
		usleep(1000);
		if(mload_run_thread(my_data_elf.start, my_data_elf.stack, my_data_elf.size_stack, 0x47)<0)
		{
			printf_debug("ehcmodule not loaded\n");
			return false;
		}else printf_debug("ehcmodule loaded with priority: %i\n",0x47);
	}else printf_debug("ehcmodule loaded with priority: %i\n",my_data_elf.prio);
	//usleep(5000);

	return true;
}

bool FindIOS(u32 ios)
{
	//u32 len_buf;
	s32 ret;
	int n;

	u64 *titles = NULL;
	u32 num_titles=0;

	ret = ES_GetNumTitles(&num_titles);
	if (ret < 0)
	{
		printf("error ES_GetNumTitles\n");
		return false;
	}

	if(num_titles<1) 
	{
		printf("error num_titles<1\n");
		return false;
	}

	titles = (u64 *)memalign(32, num_titles * sizeof(u64) + 32);
	if (!titles)
	{
		printf("error memalign\n");
		return false;
	}

	ret = ES_GetTitles(titles, num_titles);
	if (ret < 0)
	{
		free(titles);
		printf("error ES_GetTitles\n");
		return false;	
	}
		
	for(n=0; n<num_titles; n++) {
		//u32 tidl = (titles[n] &  0xFFFFFFFF);
		if((titles[n] &  0xFFFFFFFF)==ios) 
		{
			free(titles); 
			return true;
		}
	}
	
    free(titles); 
	return false;
}

static bool hbc_stub()
{
	char * sig = (char *)0x80001804;
    if(
	   sig[0] == 'S' &&
	   sig[1] == 'T' &&
	   sig[2] == 'U' &&
	   sig[3] == 'B' &&
	   sig[4] == 'H' &&
	   sig[5] == 'A' &&
	   sig[6] == 'X' &&
	   sig[7] == 'X') return true;
    return false;
}

bool reset_pressed = false;
bool power_pressed = false;

#define WATCHDOG_STACKSIZE 8*1024
static u8 watchdog_Stack[WATCHDOG_STACKSIZE] ATTRIBUTE_ALIGN (32);

mutex_t watchdogmutex=LWP_MUTEX_NULL;
int watchdogcounter=-1;
static int enable_watchdog=0;
static bool exit_watchdog_thread=false;

static void * watchdogthreadfunc (void *arg)
{
	while(!exit_watchdog_thread)
	{
		sleep(1);
		if(exit_watchdog_thread)break;
		if(reset_pressed || power_pressed)
		{
			sleep(1);
			if(exit_watchdog_thread)break;
			sleep(1);
			if(exit_watchdog_thread)break;
			sleep(1);
			if(exit_watchdog_thread)break;
			sleep(1);
			if(exit_watchdog_thread)break;
			if(reset_pressed)
			{
				printf("reset\n");
				if (!hbc_stub()) SYS_ResetSystem(SYS_RETURNTOMENU,0,0);
				exit(0);
			}
			if (power_pressed)
			{
				printf("power off\n");
				SYS_ResetSystem(SYS_POWEROFF, 0, 0);
				exit(0);
			}
		}
		if(watchdogcounter>=0)
		{
			if(watchdogcounter==0)
			{
				printf("timeout: return to loader\n");
				if (!*((u32*)0x80001800)) SYS_ResetSystem(SYS_RETURNTOMENU,0,0);
				exit(0);
			}
			if(watchdogmutex!=LWP_MUTEX_NULL)
			{
				LWP_MutexLock(watchdogmutex);
				watchdogcounter--;
				LWP_MutexUnlock(watchdogmutex);
			}
		}
	}
}
extern int cache_size;
void plat_init (int size)
{
	sprintf(MPLAYER_DATADIR,"%s",appPath);
	sprintf(MPLAYER_CONFDIR,"%s",appPath);
	sprintf(MPLAYER_LIBDIR,"%s",appPath);
	chdir(appPath);

	setenv("HOME", MPLAYER_DATADIR, 1);
	setenv("DVDCSS_CACHE", "off", 1);
	setenv("DVDCSS_VERBOSE", "0", 1);
	setenv("DVDREAD_VERBOSE", "0", 1);
	setenv("DVDCSS_RAW_DEVICE", "/dev/di", 1);

	//LWP_MutexInit(watchdogmutex, false);
	cache_size=size;

	// only used for cache_mem
	InitMem2Manager((cache_size*1024)+(8*1024));
}
void plat_deinit (int rc)
{

}
