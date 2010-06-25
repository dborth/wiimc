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
#include <malloc.h>
#include <string>
#include <vector>
using namespace std;

#include "filebrowser.h"
#include "wiimc.h"
#include "menu.h"
#include "fileop.h"
#include "networkop.h"
#include "settings.h"
#include "utils/gettext.h"

BROWSERINFO browser;
BROWSERENTRY * browserList = NULL; // list of files/folders in browser

static std::vector<std::string> browserHistory; // browser.dir history - for nested playlists

// video playlist
int videoPlaylistIndex = 0;
int videoPlaylistSize = 0;
char videoPlaylist[VIDEO_PLAYLIST_SIZE][1024];

// music playlist
MEDIAENTRY * playlist = NULL; // list of files in the current playlist
int playlistSize = 0; // number of playlist files
int playlistIndex = -1; // index of file currently playing in the playlist

// online media
MEDIAENTRY * onlinemediaList = NULL; // list of online media files
int onlinemediaSize = 0; // number of online media files

/****************************************************************************
 * ResetBrowser()
 * Clears the file browser memory, and allocates one initial entry
 ***************************************************************************/
void ResetBrowser()
{
	browser.numEntries = 0;
	browser.selIndex = 0;
	browser.pageIndex = 0;
	browser.size = 0;
}

bool AddBrowserEntry()
{
	if(browser.size >= MAX_BROWSER_SIZE)
		return false; // out of space

	memset(&(browserList[browser.size]), 0, sizeof(BROWSERENTRY)); // clear the new entry
	browser.size++;
	return true;
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

	for(int i=0; i < VIDEO_PLAYLIST_SIZE; i++)
	{
		GetFullPath(browser.selIndex+i, videoPlaylist[i]);
		videoPlaylistSize++;

		if(browser.selIndex+i+1 >= browser.numEntries)
			break;
	}
}

void ClearVideoPlaylist()
{
	videoPlaylistIndex = 0;
	videoPlaylistSize = 0;
}

bool AddPlaylistEntry()
{
	if(playlistSize+1 >= MAX_BROWSER_SIZE)
	{
		ErrorPrompt("Out of memory: too many files!");
		return false; // out of space
	}

	MEDIAENTRY * newList = (MEDIAENTRY *)realloc(playlist, (playlistSize+1) * sizeof(MEDIAENTRY));

	if(!newList) // failed to allocate required memory
	{
		ErrorPrompt("Out of memory: too many files!");
		return false;
	}
	else
	{
		playlist = newList;
	}
	memset(&(playlist[playlistSize]), 0, sizeof(MEDIAENTRY)); // clear the new entry
	playlistSize++;
	return true;
}

bool AddMediaEntry()
{
	if(onlinemediaSize+1 >= MAX_BROWSER_SIZE)
	{
		ErrorPrompt("Out of memory: too many files!");
		return false; // out of space
	}

	MEDIAENTRY * newList = (MEDIAENTRY *)realloc(onlinemediaList, (onlinemediaSize+1) * sizeof(MEDIAENTRY));

	if(!newList) // failed to allocate required memory
	{
		ErrorPrompt("Out of memory: too many files!");
		return false;
	}
	else
	{
		onlinemediaList = newList;
	}
	memset(&(onlinemediaList[onlinemediaSize]), 0, sizeof(MEDIAENTRY)); // clear the new entry
	onlinemediaSize++;
	return true;
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
	if(browser.selIndex == 0 && strlen(browserList[0].filename) == 0)
	{
		browser.dir[0] = 0;
		BrowserHistoryClear();
		return 1;
	}

	// exiting a playlist - return to specified location
	if(browser.selIndex == 0 && browserList[0].filename[0] != '.' && browser.dir[0] != 0)
	{
		strcpy(browser.dir, browserList[0].filename);
		BrowserHistoryDiscard();
		return 1;
	}

	// entering a playlist - this is handled when the playlist is parsed
	if(browserList[browser.selIndex].type == TYPE_PLAYLIST || 
		strncmp(browserList[browser.selIndex].filename, "http:", 5) == 0)
	{
		BrowserHistoryStore(browser.dir);
		GetFullPath(browser.selIndex, browser.dir);
		return 1;
	}

	// current directory doesn't change
	if (strcmp(browserList[browser.selIndex].filename,".") == 0)
	{
		return 0;
	}
	// go up to parent directory
	else if (strcmp(browserList[browser.selIndex].filename,"..") == 0)
	{
		// already at the top level
		if(IsDeviceRoot(browser.dir))
		{
			browser.dir[0] = 0; // remove device - we are going to the device listing screen
		}
		else
		{
			// determine last subdirectory namelength
			int size = strlen(browser.dir);
			browser.dir[--size] = 0;
			char *test = strrchr(browser.dir,'/');

			if(test == NULL)
				return 1;

			*test = 0; // remove last subdirectory name
			strcpy(browser.lastdir, ++test); // record last folder
		}
		return 1;
	}

	if(menuCurrent != MENU_BROWSE_ONLINEMEDIA && browser.dir[0] == 0)
	{
		// try to switch to device
		if(!ChangeInterface(browserList[browser.selIndex].filename, NOTSILENT))
			return -1;
	}

	// Open directory

	// test new directory namelength
	if ((strlen(browser.dir)+1+strlen(browserList[browser.selIndex].filename)) < MAXPATHLEN)
	{
		if(browserList[browser.selIndex].type != TYPE_PLAYLIST)
		{
			// update current directory name
			sprintf(browser.dir, "%s%s/",browser.dir, browserList[browser.selIndex].filename);
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
 * FileSortCallback
 *
 * Quick sort callback to sort file entries with the following order:
 *   .
 *   ..
 *   <dirs>
 *   <files>
 ***************************************************************************/
int FileSortCallback(const void *f1, const void *f2)
{
	/* Special case for implicit directories */
	if(((BROWSERENTRY *)f1)->filename[0] == '.' || ((BROWSERENTRY *)f2)->filename[0] == '.')
	{
		if(strcmp(((BROWSERENTRY *)f1)->filename, ".") == 0) { return -1; }
		if(strcmp(((BROWSERENTRY *)f2)->filename, ".") == 0) { return 1; }
		if(strcmp(((BROWSERENTRY *)f1)->filename, "..") == 0) { return -1; }
		if(strcmp(((BROWSERENTRY *)f2)->filename, "..") == 0) { return 1; }
	}

	/* If one is a file and one is a directory the directory is first. */
	if(((BROWSERENTRY *)f1)->type == TYPE_FOLDER && !(((BROWSERENTRY *)f2)->type == TYPE_FOLDER)) return -1;
	if(!(((BROWSERENTRY *)f1)->type == TYPE_FOLDER) && ((BROWSERENTRY *)f2)->type == TYPE_FOLDER) return 1;

	return stricmp(((BROWSERENTRY *)f1)->filename, ((BROWSERENTRY *)f2)->filename);
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
			isPlaylist = IsPlaylistExt(GetExt(browser.dir));
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
		BrowserHistoryDiscard();
		strcpy(browser.dir, BrowserHistoryRetrieve());

		if(browser.numEntries > 0) // parsing failed, but we held onto the last listing
			return res; // so we can return without any more work required
	}

	ResetBrowser();

	if(menuCurrent == MENU_BROWSE_ONLINEMEDIA)
		return ParseOnlineMedia();

	if(browser.dir[0] != 0)
		ParseDirectory(waitParse);

	if(browser.numEntries > 0)
		return browser.numEntries;

	browser.dir[0] = 0;

	int i;

	for(i=0; i < MAX_DEVICES; i++)
	{
		if(part[DEVICE_SD][i].type > 0)
		{
			AddBrowserEntry();

			sprintf(browserList[browser.numEntries].filename, "%s:", part[DEVICE_SD][i].mount);

			if(strlen(part[DEVICE_SD][i].name) > 0)
				sprintf(browserList[browser.numEntries].displayname, "%s - %s", gettext("SD"), part[DEVICE_SD][i].name);
			else if(i == 0 && part[DEVICE_SD][1].type == 0) // only one SD partition
				sprintf(browserList[browser.numEntries].displayname, "SD Card");
			else
				sprintf(browserList[browser.numEntries].displayname, "%s (%d)", gettext("SD Card"), i+1);

			browserList[browser.numEntries].length = 0;
			browserList[browser.numEntries].mtime = 0;
			browserList[browser.numEntries].type = TYPE_FOLDER; // flag this as a dir
			browserList[browser.numEntries].icon = ICON_SD;
			browser.numEntries++;
		}
	}

	for(i=0; i < MAX_DEVICES; i++)
	{
		if(part[DEVICE_USB][i].type > 0)
		{
			AddBrowserEntry();

			sprintf(browserList[browser.numEntries].filename, "%s:", part[DEVICE_USB][i].mount);

			if(strlen(part[DEVICE_USB][i].name) > 0)
				sprintf(browserList[browser.numEntries].displayname, "%s - %s", gettext("USB"), part[DEVICE_USB][i].name);
			else if(i == 0 && part[DEVICE_USB][1].type == 0) // only one USB partition
				sprintf(browserList[browser.numEntries].displayname, "USB Mass Storage");
			else
				sprintf(browserList[browser.numEntries].displayname, "%s (%d)", gettext("USB Mass Storage"), i+1);

			browserList[browser.numEntries].length = 0;
			browserList[browser.numEntries].mtime = 0;
			browserList[browser.numEntries].type = TYPE_FOLDER; // flag this as a dir
			browserList[browser.numEntries].icon = ICON_USB;
			browser.numEntries++;
		}
	}

	if(!WiiSettings.dvdDisabled)
	{
		AddBrowserEntry();
		sprintf(browserList[browser.numEntries].filename, "dvd:");
		sprintf(browserList[browser.numEntries].displayname, "Data DVD");
		browserList[browser.numEntries].length = 0;
		browserList[browser.numEntries].mtime = 0;
		browserList[browser.numEntries].type = TYPE_FOLDER;
		browserList[browser.numEntries].icon = ICON_DVD;
		browser.numEntries++;
	}

	for(i=0; i < MAX_SHARES; i++)
	{
		if(WiiSettings.smbConf[i].share[0] != 0)
		{
			AddBrowserEntry();

			sprintf(browserList[browser.numEntries].filename, "smb%d:", i+1);
			
			if(WiiSettings.smbConf[i].displayname[0] != 0)
				sprintf(browserList[browser.numEntries].displayname, "%s", WiiSettings.smbConf[i].displayname);
			else
				sprintf(browserList[browser.numEntries].displayname, "%s", WiiSettings.smbConf[i].share);
			browserList[browser.numEntries].length = 0;
			browserList[browser.numEntries].mtime = 0;
			browserList[browser.numEntries].type = TYPE_FOLDER; // flag this as a dir
			browserList[browser.numEntries].icon = ICON_SMB;
			browser.numEntries++;
		}
	}

	for(i=0; i < MAX_SHARES; i++)
	{
		if(WiiSettings.ftpConf[i].ip[0] != 0)
		{
			AddBrowserEntry();

			sprintf(browserList[browser.numEntries].filename, "ftp%d:", i+1);
			if(WiiSettings.ftpConf[i].displayname[0] != 0)
				sprintf(browserList[browser.numEntries].displayname, "%s", WiiSettings.ftpConf[i].displayname);
			else
				sprintf(browserList[browser.numEntries].displayname, "%s@%s/%s", WiiSettings.ftpConf[i].user, WiiSettings.ftpConf[i].ip, WiiSettings.ftpConf[i].folder);
			browserList[browser.numEntries].length = 0;
			browserList[browser.numEntries].mtime = 0;
			browserList[browser.numEntries].type = TYPE_FOLDER; // flag this as a dir
			browserList[browser.numEntries].icon = ICON_FTP;
			browser.numEntries++;
		}
	}
	return browser.numEntries;
}
