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
#define MAX_BROWSER_SIZE	2000
#define VIDEO_PLAYLIST_SIZE 50
#define MAX_SUBS_SIZE	300


enum
{
	TYPE_FILE,
	TYPE_FOLDER,
	TYPE_PLAYLIST,
	TYPE_SEARCH
};

typedef struct
{
	char dir[MAXPATHLEN]; // directory path of browserList
	int menu; // current menu area of the browser
	char lastdir[MAXPATHLEN]; // last browsed directory
	int numEntries; // # of entries in browserList
	int selIndex; // currently selected index of browserList
	int pageIndex; // starting index of browserList page display
	int size; // # of entries browerList has space allocated to store
} BROWSERINFO;

typedef struct
{
	u64 length; // file length
	int type; // TYPE_FILE,	TYPE_FOLDER, TYPE_PLAYLIST, TYPE_SEARCH
	char filename[MAXPATHLEN + 1]; // full filename
	char displayname[MAXJOLIET + 1]; // name for browser display
	char image[MAXJOLIET + 1];
	int icon; // icon to display
} BROWSERENTRY;

typedef struct
{
	char filepath[MAXPATHLEN + 1]; // virtual filepath
	char address[MAXPATHLEN + 1]; // address (URL)
	int type; // TYPE_FILE,	TYPE_PLAYLIST, TYPE_SEARCH
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
extern char videoPlaylist[VIDEO_PLAYLIST_SIZE][1024];

// Music playlist
extern MEDIAENTRY * playlist;
extern int playlistSize;
extern int playlistIndex;

// Online media
extern MEDIAENTRY * onlinemediaList;
extern int onlinemediaSize;

#ifdef __cplusplus
char *GetParentDir();
void ResetBrowser();
void BrowserHistoryStore(char *path);
const char * BrowserHistoryRetrieve();
void BrowserHistoryDiscard();
void PopulateVideoPlaylist();
void ClearVideoPlaylist();
bool AddBrowserEntry();
void AddSubEntry(char *path);
bool AddMediaEntry();
bool AddPlaylistEntry();
int BrowserChangeFolder(bool updateDir = true, bool waitParse = false);
int BrowseDevice();

extern "C" {
#endif
	
extern char *subsList[MAX_SUBS_SIZE];
extern int subs_size;

#ifdef __cplusplus
}
#endif
	


#endif
