/****************************************************************************
 * WiiMC
 * Tantric 2009
 *
 * filebrowser.h
 *
 * Generic file routines - reading, writing, browsing
 ****************************************************************************/

#ifndef _FILEBROWSER_H_
#define _FILEBROWSER_H_

#include <unistd.h>
#include <gccore.h>

#define MAXJOLIET 255

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
	int isdir; // 0 - file, 1 - directory
	int isplaylist;
	char filename[MAXJOLIET + 1]; // full filename
	char displayname[MAXJOLIET + 1]; // name for browser display
	int icon; // icon to display
} BROWSERENTRY;

typedef struct
{
	char filepath[MAXPATHLEN]; // virtual filepath
	char address[MAXPATHLEN]; // address (URL)
	char displayname[MAXJOLIET + 1]; // name for browser display
} MEDIAENTRY;

enum
{
	ICON_NONE,
	ICON_FOLDER,
	ICON_SD,
	ICON_USB,
	ICON_DVD,
	ICON_SMB,
	ICON_FTP
};

extern BROWSERINFO browser;
extern BROWSERENTRY * browserList;

extern MEDIAENTRY * onlinemediaList;
extern int onlinemediaSize;
extern bool inOnlineMedia;

extern MEDIAENTRY * playlist;
extern int playlistSize;
extern char currentPlaylist[];

bool IsDeviceRoot(char * path);
int FileSortCallback(const void *f1, const void *f2);
void ResetBrowser();
bool AddBrowserEntry();
int BrowserChangeFolder(bool updateDir = true);
int BrowseDevice();

#endif
