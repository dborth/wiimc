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
#include <dirent.h>

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

	char ext[7];
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
static int EnqueueFile(char * path)
{
	if(path == NULL || MusicPlaylistFindIndex(path) >= 0)
		return 0;

	char ext[7];
	GetExt(path, ext);

	// check if this is a valid audio file
	if((!IsAudioExt(ext) && !IsPlaylistExt(ext)) || strcmp(ext, "plx") == 0)
		return 0;

	if(IsPlaylistExt(ext))
	{
		// use MPlayer to try parsing the file
		play_tree_t * list = parse_playlist_file(path);

		if(!list)
			return 0;

		play_tree_iter_t *pt_iter = pt_iter_create(&list, NULL);

		if(!pt_iter)
		{
			play_tree_free(list, 1);
			return 0;
		}

		char file[MAXPATHLEN];
		play_tree_t* i;

		for(i = pt_iter->tree; i != NULL; i = i->next)
		{
			if(!i->files || strlen(i->files[0]) >= MAXPATHLEN)
				continue;

			if(!FindDevice(i->files[0], NULL, NULL))
				continue;

			GetExt(i->files[0], ext);

			if(!IsAllowedExt(ext))
				continue;

			strcpy(file, i->files[0]);
			CleanupPath(file);

			if(file[0] == 0)
				continue;

			if(MusicPlaylistFindIndex(file) >= 0)
				continue;

			if(!AddPlaylistEntry()) // add failed
			{
				pt_iter_destroy(&pt_iter);
				play_tree_free(list, 1);
				return -1;
			}

			snprintf(playlist[playlistSize-1].filepath, MAXPATHLEN, "%s", file);

			// use parameter pt_prettyformat_title for displayname if it exists
			if(i->params) 
			{
				for (int n = 0; i->params[n].name != NULL; n++) 
				{
					if(strcasecmp(i->params[n].name,PLAY_TREE_PARAM_PRETTYFORMAT_TITLE) != 0)
						continue;
					if(i->params[n].value == NULL)
						break;
					snprintf(playlist[playlistSize-1].displayname, MAXJOLIET, "%s", i->params[n].value);
					break;
				}
			}

			if(playlist[playlistSize-1].displayname[0] == 0)
			{
				char *start = strrchr(i->files[0],'/');

				// use part after last / for display name, if it's not already the end of the string
				if(start != NULL && start[1] != 0)
				{
					start++;
					snprintf(playlist[playlistSize-1].displayname, MAXJOLIET, "%s", start);
				}
				else
				{
					snprintf(playlist[playlistSize-1].displayname, MAXJOLIET, "%s", i->files[0]);
				}

				// hide the file's extension
				if(WiiSettings.hideExtensions)
					StripExt(playlist[playlistSize-1].displayname);
			}
		}

		pt_iter_destroy(&pt_iter);
		play_tree_free(list, 1);
	}
	else
	{
		if(!AddPlaylistEntry()) // add failed
			return -1;

		snprintf(playlist[playlistSize-1].filepath, MAXPATHLEN, "%s", path);

		char *start = strrchr(path,'/');

		// use part after last / for display name, if it's not already the end of the string
		if(start != NULL && start[1] != 0)
		{
			start++;
			snprintf(playlist[playlistSize-1].displayname, MAXJOLIET, "%s", start);
		}
		else
		{
			snprintf(playlist[playlistSize-1].displayname, MAXJOLIET, "%s", path);
		}

		// hide the file's extension
		if(WiiSettings.hideExtensions)
			StripExt(playlist[playlistSize-1].displayname);
	}

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
	char filepath[MAXPATHLEN];
	struct stat filestat;
	struct dirent *entry;
	DIR *dir = opendir(path);

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

	while ((entry=readdir(dir))!=NULL)
	{
		if(entry->d_name[0] == '.')
			continue;

		sprintf(filepath, "%s/%s", path, entry->d_name);
		stat(filepath,&filestat);

		if(S_ISDIR(filestat.st_mode))
		{
			if(!EnqueueFolder(filepath, SILENT))
				break;
		}
		else
		{
			if(EnqueueFile(filepath) < 0)
				break;
		}
	}
	closedir(dir);

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

	if(EnqueueFile(fullpath) == 1)
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

	if(browserList[index].type == TYPE_PLAYLIST)
	{
		// use MPlayer to try parsing the file
		play_tree_t * list = parse_playlist_file(fullpath);

		if(!list)
			return;

		play_tree_iter_t *pt_iter = pt_iter_create(&list, NULL);

		if(!pt_iter)
		{
			play_tree_free(list, 1);
			return;
		}

		char file[MAXPATHLEN];
		char ext[7];
		play_tree_t* i;

		for(i = pt_iter->tree; i != NULL; i = i->next)
		{
			if(!i->files || strlen(i->files[0]) >= MAXPATHLEN)
				continue;

			if(!FindDevice(i->files[0], NULL, NULL))
				continue;

			GetExt(i->files[0], ext);

			if(!IsAllowedExt(ext))
				continue;

			strcpy(file, i->files[0]);
			CleanupPath(file);

			if(file[0] == 0)
				continue;

			for(int i=0; i < playlistSize; i++)
			{
				if(strcmp(file, playlist[i].filepath) == 0)
				{
					Remove(i);
					break;
				}
			}
		}
		pt_iter_destroy(&pt_iter);
		play_tree_free(list, 1);
		return;
	}

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
