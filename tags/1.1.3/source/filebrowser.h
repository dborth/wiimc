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

enum
{
	TYPE_FILE,
	TYPE_FOLDER,
	TYPE_PLAYLIST,
	TYPE_SEARCH
};

typedef struct
{
	char dir[MAXPATHLEN]; // directory path
	int menu; // current menu area
	char lastdir[MAXPATHLEN]; // last browsed directory
	int numEntries; // notifies the GUI that the # of entries changed
	int selIndex; // currently selected index
	int pageIndex; // starting index of page display
	int size; // total # of entries allocated
	int maxSize; // maximum # of entries
} BROWSERINFO;

typedef struct
{
	u64 length; // file length
	int type; // TYPE_FILE,	TYPE_FOLDER, TYPE_PLAYLIST, TYPE_SEARCH
	char *file; // full filename
	char *url; // address
	char *display; // name for browser display
	char *image;
	int icon; // icon to display
} BROWSERENTRY;

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

extern BROWSERINFO browser;
extern BROWSERENTRY *browserFiles;

extern BROWSERINFO browserinfoSubs;
extern BROWSERENTRY *browserSubs;

extern BROWSERINFO browserinfoVideos;
extern BROWSERENTRY *browserVideos;

extern BROWSERINFO browserinfoMusic;
extern BROWSERENTRY *browserMusic;

extern BROWSERINFO browserinfoOnlineMedia;
extern BROWSERENTRY *browserOnlineMedia;

void ResetFiles();
void ResetVideos();
void ResetMusic();
void ResetOnlineMedia();
bool AddEntryFiles();
bool AddEntrySubs();
bool AddEntryVideos();
bool AddEntryMusic();
bool AddEntryOnlineMedia();
void BrowserHistoryStore(char *path);
const char * BrowserHistoryRetrieve();
void BrowserHistoryDiscard();
void PopulateVideoPlaylist();
char *GetParentDir();
int BrowserChangeFolder(bool updateDir = true, bool waitParse = false);
int BrowseDevice();

#endif
