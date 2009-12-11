/****************************************************************************
 * WiiMC
 * Tantric 2009
 *
 * mplayer.cpp
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
#include "video.h"
#include "menu.h"
#include "libwiigui/gui.h"
#include "input.h"
#include "filelist.h"
#include "fileop.h"
#include "filebrowser.h"
#include "wiimc.h"
#include "settings.h"

int ScreenshotRequested = 0;
int ConfigRequested = 0;
int ShutdownRequested = 0;
int ResetRequested = 0;
int ExitRequested = 0;
char appPath[1024] = { 0 };
char loadedFile[1024];
bool playingAudio = false;

#define TSTACK (512*1024)
static lwp_t mthread = LWP_THREAD_NULL;
static u8 mstack[TSTACK] ATTRIBUTE_ALIGN (32);

/****************************************************************************
 * Shutdown / Reboot / Exit
 ***************************************************************************/

void ExitApp()
{
	ShutoffRumble();
	SaveSettings(SILENT);

	// shut down some threads
	HaltDeviceThread();
	CancelAction();
	StopGX();

	if(ShutdownRequested == 1)
		SYS_ResetSystem(SYS_POWEROFF, 0, 0); // Shutdown Wii

	if(CESettings.exitAction == EXIT_AUTO)
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
	else if(CESettings.exitAction == EXIT_WIIMENU)
	{
		SYS_ResetSystem(SYS_RETURNTOMENU, 0, 0);
	}
	else
	{
		SYS_ResetSystem(SYS_POWEROFF, 0, 0);
	}
}

void ShutdownCB()
{
	ConfigRequested = 1;
	ShutdownRequested = 1;
}
void ResetCB()
{
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

void USBGeckoOutput()
{
	gecko = usb_isgeckoalive(1);
	if(!gecko) return;
	LWP_MutexInit(&gecko_mutex, false);
	
	devoptab_list[STD_OUT] = &gecko_out;
	devoptab_list[STD_ERR] = &gecko_out;
}

/****************************************************************************
 * MPlayer interface
 ***************************************************************************/

void FindNextFile()
{
	if(CESettings.playOrder == PLAY_CONTINUOUS)
	{
		browser.selIndex++;
	}
	else if(CESettings.playOrder == PLAY_SHUFFLE)
	{
		int n = rand() % browser.numEntries;
		browser.selIndex = n;
	}
	
	if(browser.selIndex >= browser.numEntries || browser.selIndex == 0)
		browser.selIndex = 1;

	sprintf(loadedFile, "%s%s", browser.dir, browserList[browser.selIndex].filename);

	if(browser.selIndex < browser.pageIndex || browser.selIndex >= browser.pageIndex + FILE_PAGESIZE)
	{
		browser.pageIndex = (ceil(browser.selIndex/FILE_PAGESIZE*1.0)) * FILE_PAGESIZE;
		if(browser.pageIndex + FILE_PAGESIZE > browser.numEntries)
			browser.pageIndex = browser.numEntries - FILE_PAGESIZE;
	}
	selectLoadedFile = 2;
}

static void *
mplayerthread (void *arg)
{
	while(1)
	{
		if(controlledbygui == 2 || CESettings.playOrder == 0 || !playingAudio)
			LWP_SuspendThread(mthread);

		printf("load file: %s\n",loadedFile);
		if(loadedFile[0] != 0)
		{
			controlledbygui = 0;
			mplayer_loadfile(loadedFile);
		}

		if(controlledbygui != 2 && CESettings.playOrder > 0 && playingAudio) // load next file
			FindNextFile();
	}
	return NULL;
}

void LoadMPlayer()
{
	controlledbygui = 0;
	HaltDeviceThread();
	printf("return control to mplayer\n");
	if(LWP_ThreadIsSuspended(mthread))
		LWP_ResumeThread(mthread);
}

void ShutdownMPlayer()
{
	printf("shutting down mplayer\n");
	controlledbygui=2;
	while(!LWP_ThreadIsSuspended(mthread))
		usleep(500);
}

/****************************************************************************
 * Main
 ***************************************************************************/

int
main(int argc, char *argv[])
{
	USBGeckoOutput(); // uncomment to enable USB gecko output
	__exception_setreload(8);

	//try to load ios202
	if(IOS_GetVersion()!=202)
	{
		if(FindIOS(202))
		{
			IOS_ReloadIOS(202);
			WIIDVD_Init(false);  //dvdx not needed
		}
		else WIIDVD_Init(true);
	}
	else WIIDVD_Init(false);

	//load usb2 driver
	if(mload_init() >= 0)
		if(load_ehci_module())
			USB2Enable(true);

	VIDEO_Init();
	InitVideo(); // Initialise video
	SetupPads();
	AUDIO_Init(NULL);

	// Wii Power/Reset buttons
	WPAD_SetPowerButtonCallback((WPADShutdownCallback)ShutdownCB);
	SYS_SetPowerCallback(ShutdownCB);
	SYS_SetResetCallback(ResetCB);

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
	LWP_CreateThread (&mthread, mplayerthread, NULL, mstack, TSTACK, 68);

	while(1)
	{
		AUDIO_RegisterDMACallback(NULL);
		AUDIO_StopDMA();
		ResetVideo_Menu();
		ResumeDeviceThread();
		WiiMenu();
		MPlayerMenu();
	}
}
