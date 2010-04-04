/****************************************************************************
 * WiiMC
 * Tantric 2009-2010
 *
 * wiimc.cpp
 ***************************************************************************/

#include <gccore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ogcsys.h>
#include <unistd.h>
#include <wiiuse/wpad.h>
#include <sys/iosupport.h>

#include "utils/FreeTypeGX.h"
#include "utils/mload.h"
#include "utils/di2.h"
#include "video.h"
#include "menu.h"
#include "libwiigui/gui.h"
#include "input.h"
#include "filelist.h"
#include "fileop.h"
#include "filebrowser.h"
#include "musicplaylist.h"
#include "wiimc.h"
#include "settings.h"

#include "mplayer/config.h"
#include "mplayer/input/input.h"
#include "mplayer/osdep/mem2_manager.h"
#include "mplayer/osdep/gx_supp.h"

extern "C" {
extern void __exception_setreload(int t);
extern void USB2Enable(bool e); // in usb2storage.c
}

int ScreenshotRequested = 0;
int ConfigRequested = 0;
int ShutdownRequested = 0;
int ResetRequested = 0;
int ExitRequested = 0;
char appPath[1024] = { 0 };
char loadedFile[1024];
bool playingAudio = false;
static bool settingsSet = false;

// MPlayer threads
#define STACKSIZE (512*1024)
static lwp_t mthread = LWP_THREAD_NULL;
static lwp_t cthread = LWP_THREAD_NULL;
static u8 mplayerstack[STACKSIZE] ATTRIBUTE_ALIGN (32);
static u8 cachestack[STACKSIZE] ATTRIBUTE_ALIGN (32);

/****************************************************************************
 * Shutdown / Reboot / Exit
 ***************************************************************************/

void ExitApp()
{
	DisableRumble();
	SaveSettings(SILENT);
	DI2_Close();

	// shut down some threads
	ShutdownMPlayer();
	SuspendDeviceThread();
	CancelAction();
	StopGX();

	if(ShutdownRequested == 1)
		SYS_ResetSystem(SYS_POWEROFF, 0, 0); // Shutdown Wii

	if(WiiSettings.exitAction == EXIT_AUTO)
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
			sig[7] == 'X')
			exit(0); // Exit to HBC
		else
			SYS_ResetSystem(SYS_RETURNTOMENU, 0, 0); // HBC not found
	}
	else if(WiiSettings.exitAction == EXIT_WIIMENU)
	{
		SYS_ResetSystem(SYS_RETURNTOMENU, 0, 0);
	}
	else
	{
		SYS_ResetSystem(SYS_POWEROFF, 0, 0);
	}
}

static void ShutdownCB()
{
	if(controlledbygui != 1 && menuMode == 0)
		return;

	ConfigRequested = 1;
	ShutdownRequested = 1;
}
static void ResetCB()
{
	if(controlledbygui != 1 && menuMode == 0)
		return;

	ResetRequested = 1;
}

/****************************************************************************
 * USB Gecko Debugging
 ***************************************************************************/

static bool gecko = false;
static mutex_t gecko_mutex = 0;

static ssize_t __out_write(struct _reent *r, int fd, const char *ptr, size_t len)
{
	u32 level;

	if (!ptr || len <= 0 || !gecko)
		return -1;

	LWP_MutexLock(gecko_mutex);
	level = IRQ_Disable();
	usb_sendbuffer(1, ptr, len);
	IRQ_Restore(level);
	LWP_MutexUnlock(gecko_mutex);
	return len;
}

const devoptab_t gecko_out = {
	"stdout",	// device name
	0,			// size of file structure
	NULL,		// device open
	NULL,		// device close
	__out_write,// device write
	NULL,		// device read
	NULL,		// device seek
	NULL,		// device fstat
	NULL,		// device stat
	NULL,		// device link
	NULL,		// device unlink
	NULL,		// device chdir
	NULL,		// device rename
	NULL,		// device mkdir
	0,			// dirStateSize
	NULL,		// device diropen_r
	NULL,		// device dirreset_r
	NULL,		// device dirnext_r
	NULL,		// device dirclose_r
	NULL		// device statvfs_r
};

static void USBGeckoOutput()
{
	gecko = usb_isgeckoalive(1);
	if(!gecko) return;
	LWP_MutexInit(&gecko_mutex, false);
	
	devoptab_list[STD_OUT] = &gecko_out;
	devoptab_list[STD_ERR] = &gecko_out;
}

/****************************************************************************
 * IOS 202
 ***************************************************************************/

static bool FindIOS(u32 ios)
{
	s32 ret;
	u32 n;

	u64 *titles = NULL;
	u32 num_titles=0;

	ret = ES_GetNumTitles(&num_titles);
	if (ret < 0)
		return false;

	if(num_titles < 1) 
		return false;

	titles = (u64 *)memalign(32, num_titles * sizeof(u64) + 32);
	if (!titles)
		return false;

	ret = ES_GetTitles(titles, num_titles);
	if (ret < 0)
	{
		free(titles);
		return false;
	}
		
	for(n=0; n < num_titles; n++)
	{
		if((titles[n] & 0xFFFFFFFF)==ios) 
		{
			free(titles); 
			return true;
		}
	}
    free(titles); 
	return false;
}

/****************************************************************************
 * MPlayer interface
 ***************************************************************************/

void FindNextAudioFile()
{
	if(playlistSize == 0)
		return;

	if(WiiSettings.playOrder == PLAY_CONTINUOUS)
	{
		playlistIndex++;

		if(playlistIndex >= playlistSize)
			playlistIndex = 0;
	}
	else if(WiiSettings.playOrder == PLAY_SHUFFLE)
	{
		int n = rand() % playlistSize;
		playlistIndex = n;
	}
	sprintf(loadedFile, "%s", playlist[playlistIndex].filepath);
	// clear any play icons
	for(int i=0; i < browser.numEntries; i++)
	{
		if(browserList[i].icon == ICON_PLAY)
		{
			if(menuCurrent == MENU_BROWSE_MUSIC)
			{
				if(MusicPlaylistFind(i))
					browserList[i].icon = ICON_FILE_CHECKED;
				else
					browserList[i].icon = ICON_FILE;
			}
			else
			{
				browserList[i].icon = ICON_NONE;
			}
		}
	}
	FindFile(); // try to find this file
}

static void *
mplayerthread (void *arg)
{
	while(1)
	{
		if(controlledbygui == 2 || WiiSettings.playOrder == 0 || !playingAudio)
			LWP_SuspendThread(mthread);

		nowPlayingSet = false;

		if(loadedFile[0] != 0)
		{
			controlledbygui = 0;
			wiiSetCache(WiiSettings.cacheSize, WiiSettings.cachePrefill);
			mplayer_loadfile(loadedFile);
		}

		if(controlledbygui != 2 && WiiSettings.playOrder > 0 && playingAudio) // load next file
			FindNextAudioFile();
	}
	return NULL;
}

extern "C" {
extern void *mplayercachethread(void *arg);

void SuspendCacheThread()
{
	LWP_SuspendThread(cthread);
}
void ResumeCacheThread()
{
	LWP_ResumeThread(cthread);
}
}

bool InitMPlayer()
{
	static bool init = false;
	if(init) return true;
	if(appPath[0] == 0) return false;

	sprintf(MPLAYER_DATADIR,"%s",appPath);
	sprintf(MPLAYER_CONFDIR,"%s",appPath);
	sprintf(MPLAYER_LIBDIR,"%s",appPath);
	chdir(appPath);

	setenv("HOME", MPLAYER_DATADIR, 1);
	setenv("DVDCSS_CACHE", "off", 1);
	setenv("DVDCSS_VERBOSE", "0", 1);
	setenv("DVDREAD_VERBOSE", "0", 1);
	setenv("DVDCSS_RAW_DEVICE", "/dev/di", 1);

	init = true;
	return true;
}

void LoadMPlayer()
{
	SuspendDeviceThread();
	SuspendPictureThread();
	SuspendParseThread();
	settingsSet = false;
	controlledbygui = 0;

	if(LWP_ThreadIsSuspended(mthread))
		LWP_ResumeThread(mthread);
}

void ShutdownMPlayer()
{
	controlledbygui=2;
	while(!LWP_ThreadIsSuspended(mthread))
		usleep(500);
}

extern "C" {
void SetMPlayerSettings()
{
	if(settingsSet)
		return;

	settingsSet = true;

	GX_SetScreenPos(WiiSettings.videoXshift, WiiSettings.videoYshift, 
					WiiSettings.videoZoomHor, WiiSettings.videoZoomVert);
	wiiSetAutoResume(WiiSettings.autoResume);
	wiiSetVolume(WiiSettings.volume);
	wiiSetProperty(MP_CMD_FRAMEDROPPING, WiiSettings.frameDropping);
	wiiSetProperty(MP_CMD_SWITCH_RATIO, WiiSettings.aspectRatio);
	wiiSetProperty(MP_CMD_AUDIO_DELAY, WiiSettings.audioDelay);
	wiiSetProperty(MP_CMD_SUB_VISIBILITY, WiiSettings.subtitleVisibility);
	wiiSetProperty(MP_CMD_SUB_DELAY, WiiSettings.subtitleDelay);
}
}

/****************************************************************************
 * Main
 ***************************************************************************/

int
main(int argc, char *argv[])
{
	USBGeckoOutput(); // uncomment to enable USB gecko output
	__exception_setreload(8);

	// try to load IOS 202
	if(IOS_GetVersion() != 202 && FindIOS(202))
		IOS_ReloadIOS(202);

	if(IOS_GetVersion() == 202)
	{
		WIIDVD_Init(false);

		// load usb2 driver
		if(mload_init() >= 0 && load_ehci_module())
			USB2Enable(true);
	}

	VIDEO_Init();
	InitVideo(); // Initialise video
	SetupPads();
	AUDIO_Init(NULL);

	// Wii Power/Reset buttons
	WPAD_SetPowerButtonCallback((WPADShutdownCallback)ShutdownCB);
	SYS_SetPowerCallback(ShutdownCB);
	SYS_SetResetCallback(ResetCB);

	InitMem2Manager(); // only used for cache_mem
	picBuffer = (u8 *)memalign(32, 1024*1024*10); // memory for picture viewer

	MountAllDevices(); // Initialize SD and USB devices

	// store path app was loaded from
	if(argc > 0 && argv[0] != NULL)
		CreateAppPath(argv[0]);

	// Set defaults
	DefaultSettings();

	loadedFile[0] = 0;
	srand (time (0)); // random seed

	// Initialize font system
	InitFreeType((u8*)font_ttf, font_ttf_size);

	// create mplayer thread
	LWP_CreateThread (&mthread, mplayerthread, NULL, mplayerstack, STACKSIZE, 68);

	// mplayer cache thread
	LWP_CreateThread(&cthread, mplayercachethread, NULL, cachestack, STACKSIZE, 70);
	
	// create GUI thread
	GuiInit();

	while(1)
	{
		AUDIO_RegisterDMACallback(NULL);
		AUDIO_StopDMA();
		ResetVideo_Menu();
		ResumeDeviceThread();
		ResumeParseThread();
		WiiMenu();
		StopDeviceThread();
		StopParseThread();
		MPlayerMenu();
	}
}
