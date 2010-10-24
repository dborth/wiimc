/****************************************************************************
 * WiiMC
 * Tantric 2009-2010
 *
 * musicplaylist.cpp
 * Music playlist
 ***************************************************************************/

#include <gccore.h>
#include <stdlib.h>
#include <string.h>
#include <sys/dir.h>

#include "wiimc.h"
#include "filebrowser.h"
#include "menu.h"
#include "settings.h"
#include "fileop.h"
#include "utils/gettext.h"

extern "C" {
#include "mplayer/playtree.h"
}

static int shuffleList[MAX_BROWSER_SIZE];
static int shuffleIndex = -1;

int MusicPlaylistGetNextShuffle()
{
	if(shuffleIndex == -1 || shuffleIndex >= playlistSize)
	{
		// populate new list
		int i, n, t;

		for(i=0; i < playlistSize; i++)
			shuffleList[i] = i;

		// shuffle the list
		for(i = 0; i < playlistSize-1; i++)
		{
			n = rand() / (RAND_MAX/(playlistSize-i) + 1);

			// swap
			t = shuffleList[i];
			shuffleList[i] = shuffleList[i+n];
			shuffleList[i+n] = t;
		}
		shuffleIndex = 0;
	}
	return shuffleList[shuffleIndex++];
}

/****************************************************************************
 * ResetsPlaylist
 * 
 * Resets the current playlist
 ***************************************************************************/
void MusicPlaylistClear()
{
	if(playlist)
	{
		free(playlist);
		playlist = NULL;
	}
	playlistSize = 0;
	shuffleIndex = -1; // reset shuffle
}

/****************************************************************************
 * ParsePlaylist
 * 
 * Loads the files inside the current playlist into the filebrowser
 ***************************************************************************/

int MusicPlaylistLoad()
{
	if(playlistSize == 0)
	{
		InfoPrompt("Playlist is Empty", "Hover over items and push the + button to add them to your playlist.");
		return 0;
	}

	BrowserHistoryStore(browser.dir);
	ResetBrowser();

	AddBrowserEntry();
	strcpy(browserList[0].filename, BrowserHistoryRetrieve());
	sprintf(browserList[0].displayname, "Exit Playlist");
	browserList[0].length = 0;
	browserList[0].mtime = 0;
	browserList[0].icon = ICON_FOLDER;

	char ext[6];
	GetExt(browser.dir, ext);

	if(IsPlaylistExt(ext) || strncmp(browser.dir, "http:", 5) == 0)
		browserList[0].type = TYPE_PLAYLIST;
	else
		browserList[0].type = TYPE_FOLDER;

	browser.numEntries++;

	int i;

	for(i=0; i < playlistSize; i++)
	{
		if(!AddBrowserEntry())
			break;

		sprintf(browserList[i+1].filename, playlist[i].filepath);
		sprintf(browserList[i+1].displayname, playlist[i].displayname);
		browserList[i+1].length = 0;
		browserList[i+1].mtime = 0;
		browserList[i+1].type = TYPE_FILE;
		browserList[i+1].icon = ICON_FILE_CHECKED;
	}
	browser.numEntries += i;
	return browser.numEntries;
}

int MusicPlaylistFindIndex(char * fullpath)
{
	for(int i=0; i < playlistSize; i++)
	{
		if(strcmp(fullpath, playlist[i].filepath) == 0)
			return i;
	}
	return -1;
}

bool MusicPlaylistFind(int index)
{
	if(index <= 0 || index >= browser.size)
		return false;

	char fullpath[MAXPATHLEN];
	GetFullPath(index, fullpath);

	if(MusicPlaylistFindIndex(fullpath) >= 0)
		return true;

	return false;
}

/****************************************************************************
 * EnqueueFile
 * 
 * Adds the specified file (with full path) to the playlist
 *  0 - not enqueued
 *  1 - enqueued
 * -1 - error
 ***************************************************************************/
static int EnqueueFile(char * path, char * name)
{
	if(path == NULL || name == NULL || strcmp(name,".") == 0 || MusicPlaylistFindIndex(path) >= 0)
		return 0;

	char ext[6];
	GetExt(path, ext);

	// check if this is a valid audio file
	if(!IsAudioExt(ext))
		return 0;

	if(!AddPlaylistEntry()) // add failed
		return -1;

	strncpy(playlist[playlistSize-1].filepath, path, MAXPATHLEN);
	strncpy(playlist[playlistSize-1].displayname, name, MAXJOLIET);
	playlist[playlistSize-1].filepath[MAXPATHLEN] = 0;
	playlist[playlistSize-1].displayname[MAXJOLIET] = 0;

	// hide the file's extension
	if(WiiSettings.hideExtensions)
		StripExt(playlist[playlistSize-1].displayname);

	shuffleIndex = -1; // reset shuffle
	return 1;
}

static int MusicSortCallback(const void *f1, const void *f2)
{
	return stricmp(((MEDIAENTRY *)f1)->displayname, ((MEDIAENTRY *)f2)->displayname);
}

/****************************************************************************
 * EnqueueFolder
 * 
 * Adds all music files in the specified folder to the playlist
 ***************************************************************************/
static bool EnqueueFolder(char * path, int silent)
{
	char filename[MAXPATHLEN];
	char filepath[MAXPATHLEN];
	struct stat filestat;
	DIR_ITER *dir = diropen(path);

	if(dir == NULL)
	{
		if(!silent)
		{
			wchar_t msg[512];
			swprintf(msg, 512, L"%s %s", gettext("Error opening"), path);
			ErrorPrompt(msg);
		}
		return false;
	}
	
	int start = playlistSize;

	while(dirnext(dir,filename,&filestat) == 0)
	{
		if(filename[0] == '.')
			continue;

		sprintf(filepath, "%s/%s", path, filename);

		if(filestat.st_mode & _IFDIR)
		{
			if(!EnqueueFolder(filepath, SILENT))
				break;
		}
		else
		{
			if(EnqueueFile(filepath, filename) < 0)
				break;
		}
	}

	if(playlistSize-start > 1)
		qsort(&playlist[start], playlistSize-start, sizeof(MEDIAENTRY), MusicSortCallback);

	return true;
}

bool MusicPlaylistEnqueue(int index)
{
	char fullpath[MAXPATHLEN+1];
	GetFullPath(index, fullpath);
	
	if(browserList[index].type == TYPE_FOLDER)
		browserList[index].icon = ICON_FOLDER_CHECKED;
	else
		browserList[index].icon = ICON_FILE_CHECKED;
	
	if(browserList[index].type == TYPE_FOLDER)
		return EnqueueFolder(fullpath, NOTSILENT);

	if(EnqueueFile(fullpath, browserList[index].filename) == 1)
		return true;

	return false;
}

static void Remove(int index)
{
	if(index < playlistSize-1)
		memmove(&playlist[index], &playlist[index+1], sizeof(MEDIAENTRY)*(playlistSize-index-1));
	playlistSize--;
	playlist = (MEDIAENTRY *)realloc(playlist, playlistSize * sizeof(MEDIAENTRY));

	if(playlistIndex >= playlistSize)
		playlistIndex = playlistSize-1;

	shuffleIndex = -1; // reset shuffle
}

void MusicPlaylistDequeue(int index)
{
	bool matchFound = false;
	char fullpath[MAXPATHLEN];
	GetFullPath(index, fullpath);

	int len = strlen(fullpath);

	if(browserList[index].type == TYPE_FOLDER)
		browserList[index].icon = ICON_FOLDER;
	else
		browserList[index].icon = ICON_FILE;

	do
	{
		matchFound = false;
		for(int i=0; i < playlistSize; i++)
		{
			if(strncmp(fullpath, playlist[i].filepath, len) == 0)
			{
				Remove(i);
				matchFound = true;
				break;
			}
		}
	} while(matchFound);
}
