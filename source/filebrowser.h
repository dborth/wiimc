/****************************************************************************
 * WiiMC
 * Tantric 2009-2010
 *
 * filebrowser.h
 *
 * Generic file routines - reading, writing, browsing
 ****************************************************************************/

#ifndef _FILEBROWSER_H_
#define _FILEBROWSER_H_

#include <unistd.h>
#include <gccore.h>

#define MAXJOLIET			255
#define MAX_BROWSER_SIZE	2500

enum
{
	TYPE_FILE,
	TYPE_FOLDER,
	TYPE_PLAYLIST,
	TYPE_SEARCH
};

typedef struct
{
	char * dir; // directory path of browserList
	int numEntries; // # of entries in browserList
	int selIndex; // currently selected index of browserList
	int pageIndex; // starting index of browserList page display
	int size; // # of entries browerList has space allocated to store
} BROWSERINFO;

typedef struct
{
	u64 length; // file length
	time_t mtime; // file modified time
	int type; // TYPE_FILE,	TYPE_FOLDER, TYPE_PLAYLIST,	TYPE_SEARCH
	char filename[MAXPATHLEN + 1]; // full filename
	char displayname[MAXJOLIET + 1]; // name for browser display
	char image[MAXJOLIET + 1];
	int icon; // icon to display
} BROWSERENTRY;

typedef struct
{
	char filepath[MAXPATHLEN + 1]; // virtual filepath
	char address[MAXPATHLEN + 1]; // address (URL)
	char displayname[MAXJOLIET + 1]; // name for browser display
	char image[MAXPATHLEN + 1]; // image to display
} MEDIAENTRY;

enum
{
	ICON_NONE,
	ICON_FOLDER,
	ICON_FOLDER_CHECKED,
	ICON_FILE,
	ICON_FILE_CHECKED,
	ICON_CHECK,
	ICON_PLAY,
	ICON_SD,
	ICON_USB,
	ICON_DVD,
	ICON_SMB,
	ICON_FTP
};

// Browser
extern BROWSERINFO browser;
extern BROWSERENTRY * browserList;

// Video playlist
extern int videoPlaylistIndex;
extern int videoPlaylistSize;
extern char videoPlaylist[20][1024];

// Music playlist
extern MEDIAENTRY * playlist;
extern int playlistSize;
extern int playlistIndex;

// Online media
extern MEDIAENTRY * onlinemediaList;
extern int onlinemediaSize;

int FileSortCallback(const void *f1, const void *f2);
void ResetBrowser();
void BrowserHistoryStore(char *path);
const char * BrowserHistoryRetrieve();
void BrowserHistoryDiscard();
void PopulateVideoPlaylist();
void ClearVideoPlaylist();
bool AddBrowserEntry();
bool AddMediaEntry();
bool AddPlaylistEntry();
int BrowserChangeFolder(bool updateDir = true, bool waitParse = false);
int BrowseDevice();

#endif
