/****************************************************************************
 * WiiMC
 * Tantric 2009-2010
 *
 * filebrowser.cpp
 *
 * Generic file routines - reading, writing, browsing
 ***************************************************************************/

#include <gccore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wiiuse/wpad.h>
#include <sys/dir.h>
#include <string>
#include <vector>
using namespace std;

#include "filebrowser.h"
#include "wiimc.h"
#include "menu.h"
#include "fileop.h"
#include "networkop.h"
#include "settings.h"
#include "utils/mem2_manager.h"
#include "utils/gettext.h"

BROWSERINFO browser;
BROWSERENTRY *browserFiles = NULL;

BROWSERINFO browserinfoSubs;
BROWSERENTRY *browserSubs = NULL;

BROWSERINFO browserinfoVideos;
BROWSERENTRY *browserVideos = NULL;

BROWSERINFO browserinfoMusic;
BROWSERENTRY *browserMusic = NULL;

BROWSERINFO browserinfoOnlineMedia;
BROWSERENTRY *browserOnlineMedia = NULL;

static std::vector<std::string> browserHistory; // browser.dir history - for nested playlists

/****************************************************************************
 * Reset()
 * Clears the browser memory
 ***************************************************************************/
static void Reset(BROWSERINFO *info, BROWSERENTRY *list)
{
	for(int i=0; i < info->size; i++)
	{
		mem2_free(list[i].file, MEM2_BROWSER);
		mem2_free(list[i].url, MEM2_BROWSER);
		mem2_free(list[i].display, MEM2_BROWSER);
		mem2_free(list[i].image, MEM2_BROWSER);
	}
	memset(list, 0, sizeof(BROWSERENTRY)*info->maxSize);
	info->numEntries = 0;
	info->selIndex = 0;
	info->pageIndex = 0;
	info->size = 0;
}

static void ResetSubs() { Reset(&browserinfoSubs, browserSubs); }
void ResetFiles() {	ResetSubs(); Reset(&browser, browserFiles); }
void ResetVideos() { Reset(&browserinfoVideos, browserVideos); }
void ResetMusic() { Reset(&browserinfoMusic, browserMusic); }
void ResetOnlineMedia() { Reset(&browserinfoOnlineMedia, browserOnlineMedia); }

static bool AddEntry(BROWSERINFO *info, BROWSERENTRY *list)
{
	if(info->size == info->maxSize || mem2_size(MEM2_BROWSER) < 8192)
		return false;

	info->size++;
	return true;
}

bool AddEntryFiles() { return AddEntry(&browser, browserFiles); }
bool AddEntrySubs() { return AddEntry(&browserinfoSubs, browserSubs); }
bool AddEntryVideos() { return AddEntry(&browserinfoVideos, browserVideos); }
bool AddEntryMusic() { return AddEntry(&browserinfoMusic, browserMusic); }
bool AddEntryOnlineMedia() { return AddEntry(&browserinfoOnlineMedia, browserOnlineMedia); }

extern "C" {

int BrowserGetSubSize()
{
	return browserinfoSubs.size;
}

char *BrowserGetSub(int i)
{
	return browserSubs[i].file;
}

int BrowserFindSub(char *path)
{
	if(!path || path[0] == 0)
		return 0;

	for(int i=0; i < browserinfoSubs.size; i++)
	{
		if (strcasecmp(browserSubs[i].file, path) == 0)
			return 1;
	}
	return 0;
}

}

void BrowserHistoryStore(char *path)
{
	string newStr(path);
	browserHistory.push_back(newStr);
}

const char * BrowserHistoryRetrieve()
{
	if(browserHistory.empty())
		return "";

	return browserHistory.back().c_str();
}

void BrowserHistoryDiscard()
{
	if(browserHistory.empty())
		return;

	browserHistory.pop_back();
}

void BrowserHistoryClear()
{
	if(browserHistory.empty())
		return;

	browserHistory.clear();
}

void PopulateVideoPlaylist()
{
	if(!WiiSettings.autoPlayNextVideo)
		return;

	char tmp[MAXPATHLEN];

	for(int i=0; i < browser.numEntries; i++)
	{
		if(!browserFiles[browser.selIndex+i].file || strncmp(browserFiles[browser.selIndex+i].file, "http://www.youtube.com", 22) == 0)
			continue;

		if(browserFiles[browser.selIndex+i].type != TYPE_FILE)
			continue;

		if(!AddEntryVideos())
			break;

		GetFullPath(browser.selIndex+i, tmp);
		browserVideos[browserinfoVideos.size-1].file = mem2_strdup(tmp, MEM2_BROWSER);
	}
}

/****************************************************************************
 * UpdateDirName()
 * Update curent directory name for file browser
 ***************************************************************************/
static int UpdateDirName()
{
	if(browser.numEntries == 0)
		return 1;

	// we are in a playlist, and have nowhere to go back to but the device listing
	if(browser.selIndex == 0 && (!browserFiles[0].file || strlen(browserFiles[0].file) == 0))
	{
		browser.dir[0] = 0;
		BrowserHistoryClear();
		return 1;
	}

	// exiting a playlist - return to specified location
	if(browser.selIndex == 0 && browserFiles[0].file[0] != '.' && browser.dir[0] != 0)
	{
		strcpy(browser.lastdir, browser.dir);
		strcpy(browser.dir, browserFiles[0].file);
		BrowserHistoryDiscard();
		return 1;
	}

	// entering a playlist - this is handled when the playlist is parsed
	if(browserFiles[browser.selIndex].type == TYPE_PLAYLIST || 
		strncmp(browserFiles[browser.selIndex].file, "http:", 5) == 0)
	{
		BrowserHistoryStore(browser.dir);
		GetFullPath(browser.selIndex, browser.dir);
		return 1;
	}

	// current directory doesn't change
	if (strcmp(browserFiles[browser.selIndex].file,".") == 0)
	{
		return 0;
	}
	// go up to parent directory
	else if (strcmp(browserFiles[browser.selIndex].file,"..") == 0)
	{
		// already at the top level
		if(IsDeviceRoot(browser.dir))
		{
			strcpy(browser.lastdir, browser.dir);
			browser.lastdir[strlen(browser.lastdir)-1] = 0; // strip trailing slash
			browser.dir[0] = 0; // remove device - we are going to the device listing screen
		}
		else
		{
			// determine last subdirectory namelength
			int size = strlen(browser.dir);
			browser.dir[--size] = 0; // remove trailing slash
			char *test = strrchr(browser.dir,'/');

			if(test == NULL)
			{
				strcpy(browser.lastdir, browser.dir);
				browser.dir[0] = 0;
			}
			else
			{
				strcpy(browser.lastdir, ++test); // record last folder
				*test = 0; // remove last subdirectory name
			}
		}
		return 1;
	}

	if(menuCurrent != MENU_BROWSE_ONLINEMEDIA && browser.dir[0] == 0)
	{
		// try to switch to device
		if(!ChangeInterface(browserFiles[browser.selIndex].file, NOTSILENT))
			return -1;
	}

	// Open directory

	// test new directory namelength
	if ((strlen(browser.dir)+1+strlen(browserFiles[browser.selIndex].file)) < MAXPATHLEN)
	{
		if(browserFiles[browser.selIndex].type != TYPE_PLAYLIST)
		{
			// update current directory name
			sprintf(browser.dir, "%s%s/",browser.dir, browserFiles[browser.selIndex].file);
		}
		return 1;
	}
	else
	{
		ErrorPrompt("Directory name is too long!");
		return -1;
	}
}

static char parentdir[1024];

char *GetParentDir()
{
	int size = strlen(browser.dir);
	strcpy(parentdir, browser.dir);
	parentdir[--size] = 0;
	char *test = strrchr(parentdir,'/');

	if(test == NULL)
		return (char *)"";

	*test = 0; // remove last subdirectory name
	return parentdir;
}

/****************************************************************************
 * BrowserChangeFolder
 *
 * Update current directory and set new entry list if directory has changed
 ***************************************************************************/
int BrowserChangeFolder(bool updateDir, bool waitParse)
{
	if(updateDir && !UpdateDirName())
		return -1;

	if(!(menuCurrent == MENU_BROWSE_ONLINEMEDIA && IsOnlineMediaPath(browser.dir)))
		CleanupPath(browser.dir);

	SuspendParseThread(); // halt parsing

	bool isPlaylist = false;

	if(browser.dir[0] != 0)
	{
		bool mounted = ChangeInterface(browser.dir, NOTSILENT);
		if(mounted)
		{
			char ext[7];
			GetExt(browser.dir, ext);
			isPlaylist = IsPlaylistExt(ext);
		}
		else if(menuCurrent != MENU_BROWSE_ONLINEMEDIA)
		{
			browser.dir[0] = 0;
		}
	}

	if(isPlaylist || (strlen(browser.dir) > 10 && strncmp(browser.dir,"http:", 5) == 0))
	{
		int res = ParsePlaylistFile();

		if(res > 0)
			return browser.numEntries;

		// parsing failed - setup last browser dir
		strcpy(browser.dir, BrowserHistoryRetrieve());
		BrowserHistoryDiscard();

		if(browser.numEntries > 0) // parsing failed, but we held onto the last listing
			return res; // so we can return without any more work required
	}

	ResetFiles();

	if(menuCurrent == MENU_BROWSE_ONLINEMEDIA)
		return ParseOnlineMedia();

	if(browser.dir[0] != 0)
		ParseDirectory(waitParse);

	if(browser.numEntries > 0)
		return browser.numEntries;

	browser.dir[0] = 0;

	int i;
	char tmp[200];

	if(isInserted[DEVICE_SD])
	{
		for(i=0; i < MAX_DEVICES; i++)
		{
			if(part[DEVICE_SD][i].type > 0)
			{
				AddEntryFiles();

				sprintf(tmp, "%s:", part[DEVICE_SD][i].mount);
				browserFiles[browser.numEntries].file = mem2_strdup(tmp, MEM2_BROWSER);

				if(strlen(part[DEVICE_SD][i].name) > 0)
					sprintf(tmp, "%s - %s", gettext("SD"), part[DEVICE_SD][i].name);
				else if(i == 0 && part[DEVICE_SD][1].type == 0) // only one SD partition
					sprintf(tmp, "SD Card");
				else
					sprintf(tmp, "%s (%d)", gettext("SD Card"), i+1);

				browserFiles[browser.numEntries].display = mem2_strdup(tmp, MEM2_BROWSER);
				browserFiles[browser.numEntries].length = 0;
				browserFiles[browser.numEntries].type = TYPE_FOLDER; // flag this as a dir
				browserFiles[browser.numEntries].icon = ICON_SD;
				browser.numEntries++;
			}
		}
	}

	if(isInserted[DEVICE_USB])
	{
		for(i=0; i < MAX_DEVICES; i++)
		{
			if(part[DEVICE_USB][i].type > 0)
			{
				AddEntryFiles();

				sprintf(tmp, "%s:", part[DEVICE_USB][i].mount);
				browserFiles[browser.numEntries].file = mem2_strdup(tmp, MEM2_BROWSER);

				if(strlen(part[DEVICE_USB][i].name) > 0)
					sprintf(tmp, "%s - %s", gettext("USB"), part[DEVICE_USB][i].name);
				else if(i == 0 && part[DEVICE_USB][1].type == 0) // only one USB partition
					sprintf(tmp, "USB Mass Storage");
				else
					sprintf(tmp, "%s (%d)", gettext("USB Mass Storage"), i+1);

				browserFiles[browser.numEntries].display = mem2_strdup(tmp, MEM2_BROWSER);
				browserFiles[browser.numEntries].length = 0;
				browserFiles[browser.numEntries].type = TYPE_FOLDER; // flag this as a dir
				browserFiles[browser.numEntries].icon = ICON_USB;
				browser.numEntries++;
			}
		}
	}

	if(!WiiSettings.dvdDisabled && isInserted[DEVICE_DVD])
	{
		AddEntryFiles();
		browserFiles[browser.numEntries].file = mem2_strdup("dvd:", MEM2_BROWSER);
		browserFiles[browser.numEntries].display = mem2_strdup("Data DVD", MEM2_BROWSER);
		browserFiles[browser.numEntries].length = 0;
		browserFiles[browser.numEntries].type = TYPE_FOLDER;
		browserFiles[browser.numEntries].icon = ICON_DVD;
		browser.numEntries++;
	}

	for(i=0; i < MAX_SHARES; i++)
	{
		if(WiiSettings.smbConf[i].share[0] != 0)
		{
			AddEntryFiles();

			sprintf(tmp, "smb%d:", i+1);
			browserFiles[browser.numEntries].file = mem2_strdup(tmp, MEM2_BROWSER);
			
			if(WiiSettings.smbConf[i].displayname[0] != 0)
				sprintf(tmp, "%s", WiiSettings.smbConf[i].displayname);
			else
				sprintf(tmp, "%s", WiiSettings.smbConf[i].share);
			
			browserFiles[browser.numEntries].display = mem2_strdup(tmp, MEM2_BROWSER);
			browserFiles[browser.numEntries].length = 0;
			browserFiles[browser.numEntries].type = TYPE_FOLDER; // flag this as a dir
			browserFiles[browser.numEntries].icon = ICON_SMB;
			browser.numEntries++;
		}
	}

	for(i=0; i < MAX_SHARES; i++)
	{
		if(WiiSettings.ftpConf[i].ip[0] != 0)
		{
			AddEntryFiles();

			sprintf(tmp, "ftp%d:", i+1);
			browserFiles[browser.numEntries].file = mem2_strdup(tmp, MEM2_BROWSER);
			
			if(WiiSettings.ftpConf[i].displayname[0] != 0)
				sprintf(tmp, "%s", WiiSettings.ftpConf[i].displayname);
			else
				sprintf(tmp, "%s@%s/%s", WiiSettings.ftpConf[i].user, WiiSettings.ftpConf[i].ip, WiiSettings.ftpConf[i].folder);
			
			browserFiles[browser.numEntries].display = mem2_strdup(tmp, MEM2_BROWSER);
			browserFiles[browser.numEntries].length = 0;
			browserFiles[browser.numEntries].type = TYPE_FOLDER; // flag this as a dir
			browserFiles[browser.numEntries].icon = ICON_FTP;
			browser.numEntries++;
		}
	}
	if(browser.lastdir[0] != 0)
		FindDirectory(); // try to find and select the last directory
	return browser.numEntries;
}
