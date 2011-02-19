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
#include "utils/mem2_manager.h"
#include "utils/gettext.h"

extern "C" {
#include "mplayer/playtree.h"
}

static int shuffleList[2000];
static int shuffleIndex = -1;

int MusicPlaylistGetNextShuffle()
{
	if(shuffleIndex == -1 || shuffleIndex >= browserinfoMusic.size)
	{
		// populate new list
		int i, n, t;

		for(i=0; i < browserinfoMusic.size; i++)
			shuffleList[i] = i;

		// shuffle the list
		for(i = 0; i < browserinfoMusic.size-1; i++)
		{
			n = rand() / (RAND_MAX/(browserinfoMusic.size-i) + 1);

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
 * ParsePlaylist
 * 
 * Loads the files inside the current playlist into the filebrowser
 ***************************************************************************/

int MusicPlaylistLoad()
{
	if(browserinfoMusic.size == 0)
	{
		InfoPrompt("Playlist is Empty", "Hover over items and push the + button to add them to your playlist.");
		return 0;
	}

	BrowserHistoryStore(browser.dir);
	ResetFiles();

	AddEntryFiles();
	browserFiles[0].file = mem2_strdup(BrowserHistoryRetrieve(), MEM2_BROWSER);
	browserFiles[0].display = mem2_strdup("Exit Playlist", MEM2_BROWSER);
	browserFiles[0].length = 0;
	browserFiles[0].icon = ICON_FOLDER;

	char ext[7];
	GetExt(browser.dir, ext);

	if(IsPlaylistExt(ext) || strncmp(browser.dir, "http:", 5) == 0)
		browserFiles[0].type = TYPE_PLAYLIST;
	else
		browserFiles[0].type = TYPE_FOLDER;

	browser.numEntries++;

	int i;

	for(i=0; i < browserinfoMusic.size; i++)
	{
		if(!AddEntryFiles())
			break;

		browserFiles[i+1].file = mem2_strdup(browserMusic[i].file, MEM2_BROWSER);
		browserFiles[i+1].display = mem2_strdup(browserMusic[i].display, MEM2_BROWSER);
		browserFiles[i+1].length = 0;
		browserFiles[i+1].type = TYPE_FILE;
		browserFiles[i+1].icon = ICON_FILE_CHECKED;
	}
	browser.numEntries += i;
	return browser.numEntries;
}

int MusicPlaylistFindIndex(char * fullpath)
{
	for(int i=0; i < browserinfoMusic.size; i++)
	{
		if(strcmp(fullpath, browserMusic[i].file) == 0)
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

			if(!AddEntryMusic()) // add failed
			{
				pt_iter_destroy(&pt_iter);
				play_tree_free(list, 1);
				return -1;
			}

			browserMusic[browserinfoMusic.size-1].file = mem2_strdup(file, MEM2_BROWSER);

			// use parameter pt_prettyformat_title for display if it exists
			if(i->params) 
			{
				for (int n = 0; i->params[n].name != NULL; n++) 
				{
					if(strcasecmp(i->params[n].name,PLAY_TREE_PARAM_PRETTYFORMAT_TITLE) != 0)
						continue;
					if(i->params[n].value == NULL)
						break;
					browserMusic[browserinfoMusic.size-1].display = mem2_strdup(i->params[n].value, MEM2_BROWSER);
					break;
				}
			}

			if(!browserMusic[browserinfoMusic.size-1].display)
			{
				char *start = strrchr(i->files[0],'/');

				// use part after last / for display name, if it's not already the end of the string
				if(start != NULL && start[1] != 0)
				{
					start++;
					browserMusic[browserinfoMusic.size-1].display = mem2_strdup(start, MEM2_BROWSER);
				}
				else
				{
					browserMusic[browserinfoMusic.size-1].display = mem2_strdup(i->files[0], MEM2_BROWSER);
				}

				// hide the file's extension
				if(WiiSettings.hideExtensions)
					StripExt(browserMusic[browserinfoMusic.size-1].display);
			}
		}

		pt_iter_destroy(&pt_iter);
		play_tree_free(list, 1);
	}
	else
	{
		if(!AddEntryMusic()) // add failed
			return -1;

		browserMusic[browserinfoMusic.size-1].file = mem2_strdup(path, MEM2_BROWSER);

		char *start = strrchr(path,'/');

		// use part after last / for display name, if it's not already the end of the string
		if(start != NULL && start[1] != 0)
		{
			start++;
			browserMusic[browserinfoMusic.size-1].display = mem2_strdup(start, MEM2_BROWSER);
		}
		else
		{
			browserMusic[browserinfoMusic.size-1].display = mem2_strdup(path, MEM2_BROWSER);
		}

		// hide the file's extension
		if(WiiSettings.hideExtensions)
			StripExt(browserMusic[browserinfoMusic.size-1].display);
	}

	shuffleIndex = -1; // reset shuffle
	return 1;
}

static int MusicSortCallback(const void *f1, const void *f2)
{
	return stricmp(((BROWSERENTRY *)f1)->display, ((BROWSERENTRY *)f2)->display);
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
	
	int start = browserinfoMusic.size;

	while ((entry=readdir(dir))!=NULL)
	{
		if(entry->d_name[0] == '.')
			continue;

		sprintf(filepath, "%s/%s", path, entry->d_name);

#ifdef _DIRENT_HAVE_D_TYPE
		if(entry->d_type==DT_DIR)
			filestat.st_mode = S_IFDIR;
		else
			filestat.st_mode = S_IFREG;
#else
		if(stat(filepath, &filestat) < 0)
			continue;
#endif
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

	if(browserinfoMusic.size-start > 1)
		qsort(&browserMusic[start], browserinfoMusic.size-start, sizeof(BROWSERENTRY), MusicSortCallback);

	return true;
}

bool MusicPlaylistEnqueue(int index)
{
	char fullpath[MAXPATHLEN+1];
	GetFullPath(index, fullpath);
	
	if(browserFiles[index].type == TYPE_FOLDER)
		browserFiles[index].icon = ICON_FOLDER_CHECKED;
	else
		browserFiles[index].icon = ICON_FILE_CHECKED;
	
	if(browserFiles[index].type == TYPE_FOLDER)
		return EnqueueFolder(fullpath, NOTSILENT);

	if(EnqueueFile(fullpath) == 1)
		return true;

	return false;
}

static void Remove(int i)
{
	mem2_free(browserMusic[i].file, MEM2_BROWSER);
	mem2_free(browserMusic[i].url, MEM2_BROWSER);
	mem2_free(browserMusic[i].display, MEM2_BROWSER);
	mem2_free(browserMusic[i].image, MEM2_BROWSER);

	// shift entries down by one
	for(int e=i; e < browserinfoMusic.size-1; e++)
		memcpy(&browserMusic[e], &browserMusic[e+1], sizeof(BROWSERENTRY));

	browserinfoMusic.size--;
	memset(&browserMusic[browserinfoMusic.size], 0, sizeof(BROWSERENTRY));

	if(browserinfoMusic.selIndex >= browserinfoMusic.size)
		browserinfoMusic.selIndex = browserinfoMusic.size-1;

	shuffleIndex = -1; // reset shuffle
}

void MusicPlaylistDequeue(int index)
{
	bool matchFound = false;
	char fullpath[MAXPATHLEN];
	GetFullPath(index, fullpath);

	int len = strlen(fullpath);

	if(browserFiles[index].type == TYPE_FOLDER)
		browserFiles[index].icon = ICON_FOLDER;
	else
		browserFiles[index].icon = ICON_FILE;

	if(browserFiles[index].type == TYPE_PLAYLIST)
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

			for(int i=0; i < browserinfoMusic.size; i++)
			{
				if(strcmp(file, browserMusic[i].file) == 0)
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
		for(int i=0; i < browserinfoMusic.size; i++)
		{
			if(strncmp(fullpath, browserMusic[i].file, len) == 0)
			{
				Remove(i);
				matchFound = true;
				break;
			}
		}
	} while(matchFound);
}
