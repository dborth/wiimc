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
#include <di/di.h>

#include "utils/FreeTypeGX.h"
#include "utils/gettext.h"
#include "utils/mem2_manager.h"
#include "video.h"
#include "networkop.h"
#include "menu.h"
#include "libwiigui/gui.h"
#include "input.h"
#include "filelist.h"
#include "fileop.h"
#include "filebrowser.h"
#include "musicplaylist.h"
#include "wiimc.h"
#include "settings.h"


//#include "mplayer/config.h"
extern char MPLAYER_DATADIR[512]; 
extern char MPLAYER_CONFDIR[512]; 
extern char MPLAYER_LIBDIR[512]; 
extern char MPLAYER_CSSDIR[512];

#include "mplayer/input/input.h"
#include "mplayer/osdep/gx_supp.h"

extern "C" {
extern void __exception_setreload(int t);
extern u32 __di_check_ahbprot(void);
extern char *network_useragent;
}

static int ShutdownRequested = 0;
static int ResetRequested = 0;

int ScreenshotRequested = 0;
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
	if(WiiSettings.sleepTimer == 0)
		return;

	if(diff_sec(sleepStart, gettime()) > (u32)(WiiSettings.sleepTimer*60))
	{
		ExitRequested = 1;
		ShutdownRequested = 1;
		controlledbygui = 2;
	}
}

void ExitApp()
{
	DisableRumble();

	if(ExitRequested == 1)
	{
		SaveFolder();
		SaveSettings(SILENT);
	}

	// shut down some threads
	SuspendDeviceThread();
	StopGX();
	UnmountAllDevices();

	if(ShutdownRequested || WiiSettings.exitAction == EXIT_POWEROFF)
		SYS_ResetSystem(SYS_POWEROFF, 0, 0);
	else if(WiiSettings.exitAction == EXIT_WIIMENU)
		SYS_ResetSystem(SYS_RETURNTOMENU, 0, 0);
	else
		exit(0); // Exit to HBC
}

static void ShutdownCB()
{
	if(controlledbygui != 1 && menuMode == 0)
		return;

	ExitRequested = 1;
	ShutdownRequested = 1;
}

static void ResetCB()
{
	if(controlledbygui != 1 && menuMode == 0)
		return;

	ExitRequested = 1;
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
 * IOS Check
 ***************************************************************************/
bool SaneIOS()
{
	bool res = false;
	u32 num_titles=0;
	u32 tmd_size;
	u32 ios = IOS_GetVersion();

	if(ios > 200)
		return false;

	if (ES_GetNumTitles(&num_titles) < 0)
		return false;

	if(num_titles < 1) 
		return false;

	u64 *titles = (u64 *)memalign(32, num_titles * sizeof(u64) + 32);
	
	if(!titles)
		return false;

	if (ES_GetTitles(titles, num_titles) < 0)
	{
		free(titles);
		return false;
	}
	
	u32 *tmdbuffer = (u32 *)memalign(32, MAX_SIGNED_TMD_SIZE);

	if(!tmdbuffer)
	{
		free(titles);
		return false;
	}

	for(u32 n=0; n < num_titles; n++)
	{
		if((titles[n] & 0xFFFFFFFF) != ios) 
			continue;

		if (ES_GetStoredTMDSize(titles[n], &tmd_size) < 0)
			break;

		if (tmd_size < 0 || tmd_size > 4096)
			break;

		if(ES_GetStoredTMD(titles[n], (signed_blob *)tmdbuffer, tmd_size) < 0)
			break;

		if (tmdbuffer[1] || tmdbuffer[2])
		{
			res = true;
			break;
		}
	}
	free(tmdbuffer);
    free(titles);
	return res;
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
			playlistIndex = MusicPlaylistGetNextShuffle();
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

void show_mem()
{
	printf("m1(%.4f) m2(%.4f)\n",
								((float)((char*)SYS_GetArena1Hi()-(char*)SYS_GetArena1Lo()))/0x100000,
								 ((float)((char*)SYS_GetArena2Hi()-(char*)SYS_GetArena2Lo()))/0x100000);
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
	sprintf(MPLAYER_CSSDIR,"%s/css",appPath);
	DIR_ITER *dir = diropen(MPLAYER_CSSDIR);

	if(!dir && mkdir(MPLAYER_CSSDIR, 0777) != 0)
		sprintf(MPLAYER_CSSDIR, "off");
	else
		dirclose(dir);

	setenv("HOME", MPLAYER_DATADIR, 1);
	setenv("DVDCSS_CACHE", MPLAYER_CSSDIR, 1);
	setenv("DVDCSS_METHOD", "disc", 1);
	setenv("DVDCSS_VERBOSE", "0", 1);
	setenv("DVDREAD_VERBOSE", "0", 1);
	setenv("DVDCSS_RAW_DEVICE", "/dev/di", 1);

	char agent[15];
	sprintf(agent, "%s/%s", APPNAME, APPVERSION);
	network_useragent = mem2_strdup(agent, OTHER_AREA);

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
	static float aspectRatio=-2;
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

	if(aspectRatio!=WiiSettings.aspectRatio)
	{
		aspectRatio=WiiSettings.aspectRatio;
		wiiSetProperty(MP_CMD_SWITCH_RATIO, WiiSettings.aspectRatio);
	}

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
 
	// only reload IOS if AHBPROT is not enabled
	u32 have_ahbprot = __di_check_ahbprot();
	u32 version = IOS_GetVersion();
	s32 preferred = IOS_GetPreferredVersion();

	if(version != 58 && preferred > 0 && version != (u32)preferred && !have_ahbprot)
		IOS_ReloadIOS(preferred);
	
	if(have_ahbprot)
		DI_Init();

	WPAD_Init();
	InitVideo();
	USBStorage_Initialize(); // to set aside MEM2 area
	
	u32 size = 	(8*1024*1024) + // cache
			(sizeof(BROWSERENTRY)*MAX_BROWSER_SIZE) + // browser memory
			(1024*1024*2)+(1024*512*2) + //textures
			(VIDEO_GetFrameBufferSize(vmode)*2) + //video buffers
			(vmode->fbWidth * vmode->efbHeight * 4) + //videoScreenshot
			(16*1024); // padding
	AddMem2Area (size, VIDEO_AREA);
	AddMem2Area (6*1024*1024, GUI_AREA);
	AddMem2Area (3*1024*1024, OTHER_AREA); // vars + ttf , we have to improve ext_ttf

	InitVideo2();
	SetupPads();
	StartNetworkThread();

	// Wii Power/Reset buttons
	WPAD_SetPowerButtonCallback((WPADShutdownCallback)ShutdownCB);
	SYS_SetPowerCallback(ShutdownCB);
	SYS_SetResetCallback(ResetCB);

	AUDIO_Init(NULL);
	GX_AllocTextureMemory();
	browserList = (BROWSERENTRY *)mem2_malloc(sizeof(BROWSERENTRY)*MAX_BROWSER_SIZE, VIDEO_AREA);
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
