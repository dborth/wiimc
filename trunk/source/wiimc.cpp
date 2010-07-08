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
#include "utils/usb2storage.h"
#include "utils/gettext.h"
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
extern char *network_useragent;
}

int ScreenshotRequested = 0;
int ConfigRequested = 0;
int ShutdownRequested = 0;
int ResetRequested = 0;
int ExitRequested = 0;
char appPath[1024] = { 0 };
char loadPath[1024] = { 0 };
char loadedFile[1024] = { 0 };
char loadedFileDisplay[128] = { 0 };
static bool settingsSet = false;

// MPlayer threads
#define STACKSIZE (512*1024)
#define CACHE_STACKSIZE (8*1024)
static lwp_t mthread = LWP_THREAD_NULL;
static lwp_t cthread = LWP_THREAD_NULL;
static u8 mplayerstack[STACKSIZE] ATTRIBUTE_ALIGN (32);
static u8 cachestack[CACHE_STACKSIZE] ATTRIBUTE_ALIGN (32);

/****************************************************************************
 * Shutdown / Reboot / Exit
 ***************************************************************************/

static u64 sleepStart = 0;

void SetSleepTimer()
{
	if(WiiSettings.sleepTimer == 0)
		sleepStart = 0;
	else
		sleepStart = gettime();
}

void CheckSleepTimer()
{
	if(sleepStart == 0)
		return;

	if(diff_sec(sleepStart, gettime()) > (u32)(WiiSettings.sleepTimer*60))
	{
		ShutdownRequested = 1;
		controlledbygui = 2;
	}
}

void ExitApp()
{
	DisableRumble();
	if(ShutdownRequested == 1 || ExitRequested == 1)
	{
		SaveFolder();
		SaveSettings(SILENT);
	}
	DI2_Close();

	// shut down some threads
	SuspendDeviceThread();
	CancelAction();
	StopGX();

	if(ShutdownRequested == 1 || WiiSettings.exitAction == EXIT_POWEROFF)
		SYS_ResetSystem(SYS_POWEROFF, 0, 0);
	else if(WiiSettings.exitAction == EXIT_WIIMENU)
		SYS_ResetSystem(SYS_RETURNTOMENU, 0, 0);
	else
		exit(0); // Exit to HBC
}

static void ShutdownCB()
{
	if((controlledbygui != 1 && menuMode == 0) || inNetworkInit)
		return;

	ConfigRequested = 1;
	ShutdownRequested = 1;
}
static void ResetCB()
{
	if((controlledbygui != 1 && menuMode == 0) || inNetworkInit)
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
	if (!gecko || !ptr || len <= 0)
		return -1;

	u32 level;
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
	gecko = usb_isgeckoalive(1); // uncomment to enable USB Gecko output

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

#define ROUNDDOWN32(v)				(((u32)(v)-0x1f)&~0x1f)

static bool USBLANDetected()
{
	u8 dummy;
	u8 i;
	u16 vid, pid;

	USB_Initialize();
	u8 *buffer = (u8*)ROUNDDOWN32(((u32)SYS_GetArena2Hi() - (32*1024)));
	memset(buffer, 0, 8 << 3);

	if(USB_GetDeviceList("/dev/usb/oh0", buffer, 8, 0, &dummy) < 0)
		return false;

	for(i = 0; i < 8; i++)
	{
		memcpy(&vid, (buffer + (i << 3) + 4), 2);
		memcpy(&pid, (buffer + (i << 3) + 6), 2);
		if( (vid == 0x0b95) && (pid == 0x7720))
			return true;
	}
	return false;
}

/****************************************************************************
 * MPlayer interface
 ***************************************************************************/

extern "C" bool FindNextFile(bool load)
{
	nowPlayingSet = false;

	if(menuMode == 0 && menuCurrent == MENU_BROWSE_MUSIC)
	{
		// clear any play icons
		for(int i=0; i < browser.numEntries; i++)
		{
			if(browserList[i].icon == ICON_PLAY)
			{
				if(MusicPlaylistFind(i))
					browserList[i].icon = ICON_FILE_CHECKED;
				else
					browserList[i].icon = ICON_FILE;
			}
		}
		findLoadedFile = 2; // trigger file browser update
	}

	if(controlledbygui == 2) // file termination requested, do not load another file
		return false;

	if(controlledbygui == 0) // playing a video
	{
		if(!load)
			return false;

		if(!WiiSettings.autoPlayNextVideo || videoPlaylistIndex+1 >= videoPlaylistSize)
		{
			loadedFile[0] = 0;
			loadedFileDisplay[0] = 0;
			ClearVideoPlaylist();
			return false;
		}
		else
		{
			strcpy(loadedFile, videoPlaylist[++videoPlaylistIndex]);

			char *start = strrchr(loadedFile,'/');

			// use part after last / for display name, if it's not already the end of the string
			if(start != NULL && start[1] != 0)
			{
				start++;
				snprintf(loadedFileDisplay, 128, "%s", start);
			}
			else
			{
				snprintf(loadedFileDisplay, 128, "%s", loadedFile);
			}
		}
	}
	else
	{
		if(playlistSize == 0 || (WiiSettings.playOrder == PLAY_SINGLE && playlistIndex != -1))
		{
			playlistIndex = -1;
			return false;
		}
	
		if(WiiSettings.playOrder == PLAY_CONTINUOUS)
		{
			playlistIndex++;
	
			if(playlistIndex >= playlistSize)
				playlistIndex = 0;
		}
		else if(WiiSettings.playOrder == PLAY_SHUFFLE)
		{
			if(playlistSize == 1)
			{
				playlistIndex = 0;
			}
			else
			{
				int n = -1;
	
				// don't select the same song twice
				while(n < 0 || n >= playlistSize || n == playlistIndex)
					n = (int)(((double)rand() / double(RAND_MAX + 1.0)) * playlistSize);
				playlistIndex = n;
			}
		}
		else if(playlistIndex == -1 || playlistIndex >= playlistSize)
		{
			playlistIndex = 0;
		}
		sprintf(loadedFile, "%s", playlist[playlistIndex].filepath);
	}

	if(load)
		wiiLoadFile(loadedFile);

	if(controlledbygui == 1)
		FindFile(); // try to find this file

	return true;
}

static void *
mplayerthread (void *arg)
{
	mplayer_main();
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
bool CacheThreadSuspended()
{
	if(LWP_ThreadIsSuspended(cthread))
		return true;
	return false;
}
}

bool InitMPlayer()
{
	static bool init = false;
	if(init) return true;

	if(appPath[0] == 0)
	{
		InfoPrompt("Unable to Initialize MPlayer", "Unable to find a valid working path");
		return false;
	}

	if(chdir(appPath) != 0)
	{
		wchar_t msg[512];
		swprintf(msg, 512, L"%s %s", gettext("Unable to change path to"), appPath);
		InfoPrompt("Unable to Initialize MPlayer", msg);
		return false;
	}

	sprintf(MPLAYER_DATADIR,"%s",appPath);
	sprintf(MPLAYER_CONFDIR,"%s",appPath);
	sprintf(MPLAYER_LIBDIR,"%s",appPath);

	setenv("HOME", MPLAYER_DATADIR, 1);
	setenv("DVDCSS_CACHE", "off", 1);
	setenv("DVDCSS_METHOD", "disc", 1);
	setenv("DVDCSS_VERBOSE", "0", 1);
	setenv("DVDREAD_VERBOSE", "0", 1);
	setenv("DVDCSS_RAW_DEVICE", "/dev/di", 1);

	char agent[15];
	sprintf(agent, "%s/%s", APPNAME, APPVERSION);
	network_useragent = strdup(agent);

	// create mplayer thread
	LWP_CreateThread (&mthread, mplayerthread, NULL, mplayerstack, STACKSIZE, 68);

	init = true;
	return true;
}

void LoadMPlayerFile()
{
	SuspendDeviceThread();
	SuspendPictureThread();
	SuspendParseThread();
	settingsSet = false;
	nowPlayingSet = false;
	controlledbygui = 2; // signal any previous file to end

	// wait for previous file to end
	while(controlledbygui == 2)
		usleep(100);

	// set new file to load
	wiiLoadFile(loadedFile);

	while(controlledbygui != 0)
		usleep(100);
}

void ResumeMPlayerFile()
{
	DisableRumble();
	SuspendDeviceThread();
	SuspendPictureThread();
	SuspendParseThread();
	settingsSet = false;
	nowPlayingSet = false;
	controlledbygui = 0;
}

void StopMPlayerFile()
{
	controlledbygui = 2; // signal any previous file to end
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
	wiiSetSeekTime(WiiSettings.seekTime);
	wiiSetCacheFill(WiiSettings.cacheFill);

	if(strncmp(loadedFile, "dvd", 3) == 0) // always use framedropping for DVD
		wiiSetProperty(MP_CMD_FRAMEDROPPING, FRAMEDROPPING_AUTO);
	else
		wiiSetProperty(MP_CMD_FRAMEDROPPING, WiiSettings.frameDropping);
	
	
	wiiSetProperty(MP_CMD_SWITCH_RATIO, WiiSettings.aspectRatio);
	wiiSetProperty(MP_CMD_AUDIO_DELAY, WiiSettings.audioDelay);
	wiiSetProperty(MP_CMD_SUB_VISIBILITY, WiiSettings.subtitleVisibility);
	wiiSetProperty(MP_CMD_SUB_DELAY, WiiSettings.subtitleDelay);
	wiiSetCodepage(WiiSettings.subtitleCodepage);

	char audioLang[14] = { 0 };
	char subtitleLang[14] = { 0 };

	if(WiiSettings.audioLanguage[0] != 0)
		sprintf(audioLang, "%s,%s,en,eng", 
			WiiSettings.audioLanguage, languages[GetLangIndex(WiiSettings.audioLanguage)].abbrev2);
	if(WiiSettings.subtitleLanguage[0] != 0)
		sprintf(subtitleLang, "%s,%s,en,eng", 
			WiiSettings.subtitleLanguage, languages[GetLangIndex(WiiSettings.subtitleLanguage)].abbrev2);

	wiiSetAudioLanguage(audioLang);
	wiiSetSubtitleLanguage(subtitleLang);
	wiiSetSubtitleColor(WiiSettings.subtitleColor);
	wiiSetSubtitleSize(WiiSettings.subtitleSize);
}
}

/****************************************************************************
 * Main
 ***************************************************************************/

int main(int argc, char *argv[])
{
	USBGeckoOutput(); // don't disable - we need the stdout/stderr devoptab!
	__exception_setreload(8);

	bool usblan = USBLANDetected();

	// try to load IOS 202
	if(FindIOS(202))
		IOS_ReloadIOS(202);
	else if(IOS_GetVersion() < 61 && FindIOS(61))
		IOS_ReloadIOS(61);

	if(IOS_GetVersion() == 202)
	{
		// enable DVD and USB2
		if(mload_init() >= 0 && load_ehci_module())
		{
			int mode = 3;

			if(usblan)
			{
				int usblanport = GetUSB2LanPort();

				if(usblanport >= 0)
				{
					if(usblanport == 1)
						mode = 1;
					else
						mode = 2;

					USB2Storage_Close();
					mload_close();
					IOS_ReloadIOS(202);
					mload_init();
					load_ehci_module();
				}
			}
			SetUSB2Mode(mode);
			USB2Enable(true);
			WIIDVD_Init(false);
		}
	}

	VIDEO_Init();
	InitVideo();
	SetupPads();
	AUDIO_Init(NULL);

	// Wii Power/Reset buttons
	WPAD_SetPowerButtonCallback((WPADShutdownCallback)ShutdownCB);
	SYS_SetPowerCallback(ShutdownCB);
	SYS_SetResetCallback(ResetCB);
	InitMem2Manager(); // only used for cache_mem
	// we want browser to sit at high MEM1 and NOT low MEM2 (to optimize space)
	browserList = (BROWSERENTRY *)malloc(sizeof(BROWSERENTRY)*MAX_BROWSER_SIZE);
	GX_AllocTextureMemory(); // low MEM2 - texture memory
	MountAllDevices(); // Initialize SD and USB devices

	// store path app was loaded from
	if(argc > 0 && argv[0] != NULL)
		CreateLoadPath(argv[0]);

	DefaultSettings(); // set defaults
	srand (time (0)); // random seed
	InitFreeType((u8*)font_ttf, font_ttf_size); // Initialize font system

	// mplayer cache thread
	LWP_CreateThread(&cthread, mplayercachethread, NULL, cachestack, CACHE_STACKSIZE, 70);

	// create GUI thread
	GuiInit();

	while(1)
	{
		ResetVideo_Menu();
		ResumeDeviceThread();
		ResumeParseThread();
		WiiMenu();
		StopDeviceThread();
		StopParseThread();
		MPlayerMenu();
	}
}
