/****************************************************************************
 * WiiMC
 * Tantric 2009
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

#include "filebrowser.h"
#include "wiimc.h"
#include "menu.h"
#include "fileop.h"
#include "networkop.h"
#include "settings.h"

BROWSERINFO browser;
BROWSERENTRY * browserList = NULL; // list of files/folders in browser
char browserPlaylist[MAXPATHLEN] = "\0"; // playlist URL, loaded from the file browser

MEDIAENTRY * onlinemediaList = NULL; // list of online media files
int onlinemediaSize = 0; // number of online media files

// audio playlist
MEDIAENTRY * playlist = NULL; // list of files in the current playlist
int playlistSize = 0; // number of playlist files
int playlistIndex = 0; // index of file currently playing in the playlist

/****************************************************************************
 * ResetBrowser()
 * Clears the file browser memory, and allocates one initial entry
 ***************************************************************************/
void ResetBrowser()
{
	browser.numEntries = 0;
	browser.selIndex = 0;
	browser.pageIndex = 0;

	// Clear any existing values
	if(browserList != NULL)
	{
		free(browserList);
		browserList = NULL;
	}
	// set aside space for 1 entry
	browserList = (BROWSERENTRY *)malloc(sizeof(BROWSERENTRY));
	memset(browserList, 0, sizeof(BROWSERENTRY));
	browser.size = 1;
}

bool AddBrowserEntry()
{
	BROWSERENTRY * newBrowserList = (BROWSERENTRY *)realloc(browserList, (browser.size+1) * sizeof(BROWSERENTRY));

	if(!newBrowserList) // failed to allocate required memory
	{
		ErrorPrompt("Out of memory: too many files!");
		return false;
	}
	else
	{
		browserList = newBrowserList;
	}
	memset(&(browserList[browser.size]), 0, sizeof(BROWSERENTRY)); // clear the new entry
	browser.size++;
	return true;
}

bool AddPlaylistEntry()
{
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
 * CleanupPath()
 * Cleans up the filepath, removing double // and replacing \ with /
 ***************************************************************************/
static void CleanupPath(char * path)
{
	if(!path || path[0] == 0)
		return;

	int pathlen = strlen(path);
	int j = 0;
	for(int i=0; i < pathlen && i < MAXPATHLEN; i++)
	{
		if(path[i] == '\\')
			path[i] = '/';

		if(j == 0 || !(path[j-1] == '/' && path[i] == '/'))
			path[j++] = path[i];
	}
	path[j] = 0;
}

bool IsDeviceRoot(char * path)
{
	if(path == NULL || path[0] == 0)
		return false;

	int pathlen = strlen(path);

	if((strncmp(path, "sd", 2) == 0 && pathlen == 5) ||
		(strncmp(path, "usb", 3) == 0 && pathlen == 6) ||
		strcmp(path, "dvd:/") == 0 ||
		(strncmp(path, "smb", 3) == 0 && pathlen == 6) ||
		(strncmp(path, "ftp", 3) == 0 && pathlen == 6))
	{
		return true;
	}
	return false;
}

/****************************************************************************
 * UpdateDirName()
 * Update curent directory name for file browser
 ***************************************************************************/
static int UpdateDirName()
{
	int size=0;
	char * test;
	char temp[1024];

	if(browser.numEntries == 0)
		return 1;

	/* current directory doesn't change */
	if (strcmp(browserList[browser.selIndex].filename,".") == 0)
	{
		return 0;
	}
	/* go up to parent directory */
	else if (strcmp(browserList[browser.selIndex].filename,"..") == 0)
	{
		// already at the top level
		if(IsDeviceRoot(browser.dir))
		{
			browser.dir[0] = 0; // remove device - we are going to the device listing screen
		}
		else
		{
			if(browserPlaylist[0] != 0) // inside a playlist
			{
				browserPlaylist[0] = 0; // leave playlist
			}
			else
			{
				/* determine last subdirectory namelength */
				sprintf(temp,"%s",browser.dir);
				test = strtok(temp,"/");
				while (test != NULL)
				{
					size = strlen(test);
					test = strtok(NULL,"/");
				}
	
				/* remove last subdirectory name */
				size = strlen(browser.dir) - size - 1;
				browser.dir[size] = 0;
			}
		}
		return 1;
	}

	if(menuCurrent != MENU_BROWSE_ONLINEMEDIA && browser.dir[0] == 0)
	{
		// try to switch to device
		if(!ChangeInterface(browserList[browser.selIndex].filename, NOTSILENT))
			return -1;
	}

	/* Open directory */

	/* test new directory namelength */
	if ((strlen(browser.dir)+1+strlen(browserList[browser.selIndex].filename)) < MAXPATHLEN)
	{
		if(!browserList[browser.selIndex].isplaylist)
		{
			/* update current directory name */
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
	if(((BROWSERENTRY *)f1)->isdir && !(((BROWSERENTRY *)f2)->isdir)) return -1;
	if(!(((BROWSERENTRY *)f1)->isdir) && ((BROWSERENTRY *)f2)->isdir) return 1;

	return stricmp(((BROWSERENTRY *)f1)->filename, ((BROWSERENTRY *)f2)->filename);
}

/****************************************************************************
 * BrowserChangeFolder
 *
 * Update current directory and set new entry list if directory has changed
 ***************************************************************************/
int BrowserChangeFolder(bool updateDir)
{
	if(updateDir && !UpdateDirName())
		return -1;

	CleanupPath(browser.dir);
	HaltParseThread(); // halt parsing
	ResetBrowser(); // reset browser

	if(browserPlaylist[0] != 0)
	{
		ParsePlaylistFile();
	}
	else if(menuCurrent == MENU_BROWSE_ONLINEMEDIA)
	{
		ParseOnlineMedia();
	}
	else
	{
		if(browser.dir[0] != 0)
			ParseDirectory();

		if(browser.numEntries == 0)
		{
			browser.dir[0] = 0;
			
			int i;

			for(i=0; i < MAX_DEVICES; i++)
			{
				if(part[DEVICE_SD][i].type > 0)
				{
					if(!AddBrowserEntry())
						break;

					sprintf(browserList[browser.numEntries].filename, "%s:/", part[DEVICE_SD][i].mount);

					if(strlen(part[DEVICE_SD][i].name) > 0)
						sprintf(browserList[browser.numEntries].displayname, "SD - %s", part[DEVICE_SD][i].name);
					else if(i == 0 && part[DEVICE_SD][1].type == 0) // only one SD partition
						sprintf(browserList[browser.numEntries].displayname, "SD Card");
					else
						sprintf(browserList[browser.numEntries].displayname, "SD Card (%d)", i);

					browserList[browser.numEntries].length = 0;
					browserList[browser.numEntries].mtime = 0;
					browserList[browser.numEntries].isdir = 1; // flag this as a dir
					browserList[browser.numEntries].icon = ICON_SD;
					browser.numEntries++;
				}
			}

			for(i=0; i < MAX_DEVICES; i++)
			{
				if(part[DEVICE_USB][i].type > 0)
				{
					if(!AddBrowserEntry())
						break;

					sprintf(browserList[browser.numEntries].filename, "%s:/", part[DEVICE_USB][i].mount);

					if(strlen(part[DEVICE_USB][i].name) > 0)
						sprintf(browserList[browser.numEntries].displayname, "USB - %s", part[DEVICE_USB][i].name);
					else if(i == 0 && part[DEVICE_USB][1].type == 0) // only one USB partition
						sprintf(browserList[browser.numEntries].displayname, "USB Mass Storage");
					else
						sprintf(browserList[browser.numEntries].displayname, "USB Mass Storage (%d)", i);

					browserList[browser.numEntries].length = 0;
					browserList[browser.numEntries].mtime = 0;
					browserList[browser.numEntries].isdir = 1; // flag this as a dir
					browserList[browser.numEntries].icon = ICON_USB;
					browser.numEntries++;
				}
			}

			AddBrowserEntry();
			sprintf(browserList[browser.numEntries].filename, "dvd:/");
			sprintf(browserList[browser.numEntries].displayname, "Data DVD");
			browserList[browser.numEntries].length = 0;
			browserList[browser.numEntries].mtime = 0;
			browserList[browser.numEntries].isdir = 1;
			browserList[browser.numEntries].icon = ICON_DVD;
			browser.numEntries++;

			for(i=0; i < 5; i++)
			{
				if(WiiSettings.smbConf[i].share[0] != 0)
				{
					if(!AddBrowserEntry())
						break;
	
					sprintf(browserList[browser.numEntries].filename, "smb%d:/", i+1);
					
					if(WiiSettings.smbConf[i].displayname[0] != 0)
						sprintf(browserList[browser.numEntries].displayname, "%s", WiiSettings.smbConf[i].displayname);
					else
						sprintf(browserList[browser.numEntries].displayname, "%s", WiiSettings.smbConf[i].share);
					browserList[browser.numEntries].length = 0;
					browserList[browser.numEntries].mtime = 0;
					browserList[browser.numEntries].isdir = 1; // flag this as a dir
					browserList[browser.numEntries].icon = ICON_SMB;
					browser.numEntries++;
				}
			}

			for(i=0; i < 5; i++)
			{
				if(WiiSettings.ftpConf[i].ip[0] != 0)
				{
					if(!AddBrowserEntry())
						break;
	
					sprintf(browserList[browser.numEntries].filename, "ftp%d:/", i+1);
					if(WiiSettings.ftpConf[i].displayname[0] != 0)
						sprintf(browserList[browser.numEntries].displayname, "%s", WiiSettings.ftpConf[i].displayname);
					else
						sprintf(browserList[browser.numEntries].displayname, "%s@%s/%s", WiiSettings.ftpConf[i].user, WiiSettings.ftpConf[i].ip, WiiSettings.ftpConf[i].folder);
					browserList[browser.numEntries].length = 0;
					browserList[browser.numEntries].mtime = 0;
					browserList[browser.numEntries].isdir = 1; // flag this as a dir
					browserList[browser.numEntries].icon = ICON_FTP;
					browser.numEntries++;
				}
			}
		}
	}
	return browser.numEntries;
}
