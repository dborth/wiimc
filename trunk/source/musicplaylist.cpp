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
#include <malloc.h>
#include <sys/dir.h>

#include "filebrowser.h"
#include "menu.h"
#include "settings.h"
#include "fileop.h"

extern "C" {
#include "mplayer/playtree.h"
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
		InfoPrompt("There are no files currently in the playlist.");
		return 0;
	}

	ResetBrowser();

	AddBrowserEntry();
	sprintf(browserList[0].filename, "..");
	sprintf(browserList[0].displayname, "Exit Playlist");
	browserList[0].length = 0;
	browserList[0].mtime = 0;
	browserList[0].isdir = 1;
	browserList[0].icon = ICON_FOLDER;
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
		browserList[i+1].isdir = 0;
		browserList[i+1].isplaylist = 0;
		browserList[i+1].icon = ICON_NONE;
	}
	browser.numEntries += i;
	return browser.numEntries;
}

bool MusicPlaylistFind(int index)
{
	if(index <= 0 || index >= browser.size)
		return false;
	
	char fullpath[MAXPATHLEN];
	sprintf(fullpath, "%s%s", browser.dir, browserList[index].filename);
	
	for(int i=0; i < playlistSize; i++)
	{
		if(strcmp(fullpath, playlist[i].filepath) == 0)
			return true;
	}
	return false;
}

/****************************************************************************
 * EnqueueFile
 * 
 * Adds the specified file (with full path) to the playlist.
 * If a playlist is found, it is parsed and the music files inside are added.
 ***************************************************************************/
static bool EnqueueFile(char * path, char * name)
{
	if(path == NULL || name == NULL || strcmp(name,".") == 0)
		return false;

	char *ext = strrchr(path,'.');

	if(ext == NULL)
		return false; // file does not have an extension - skip it

	ext++;

	// check if this is a playlist
	int i=0;
	char *start;
	do
	{
		if (strcasecmp(ext, validPlaylistExtensions[i++]) == 0)
		{
			// this is a playlist - parse it and add the files inside
			char * playlistEntry;
			char display[MAXJOLIET+1];
			play_tree_t * list = parse_playlist_file(browserPlaylist);
			
			if(!list)
				return false;

			play_tree_iter_t *pt_iter = NULL;

			if((pt_iter = pt_iter_create(&list, NULL)))
			{
				while ((playlistEntry = pt_iter_get_next_file(pt_iter)) != NULL)
				{
					start = strrchr(playlistEntry,'/');
					if(start != NULL) // start up starting part of path
					{
						start++;
						sprintf(display, start);
					}
					else
					{
						strncpy(display, playlistEntry, MAXJOLIET);
						display[MAXJOLIET] = 0;
					}
					EnqueueFile(playlistEntry, display);
				}
				pt_iter_destroy(&pt_iter);
			}
			return true;
		}
	} while (validPlaylistExtensions[i][0] != 0);

	// check if this is a valid audio file
	i=0;
	do
	{
		if (strcasecmp(ext, validAudioExtensions[i]) == 0)
			break;
	} while (validAudioExtensions[++i][0] != 0);
	if (validAudioExtensions[i][0] == 0) // extension not found
		return false;

	if(!AddPlaylistEntry()) // add failed
		return false;

	strncpy(playlist[playlistSize-1].filepath, path, MAXPATHLEN);
	strncpy(playlist[playlistSize-1].displayname, name, MAXJOLIET);
	playlist[playlistSize-1].filepath[MAXPATHLEN] = 0;
	playlist[playlistSize-1].displayname[MAXJOLIET] = 0;

	// hide the file's extension
	if(WiiSettings.hideExtensions)
		StripExt(playlist[playlistSize-1].displayname);

	// strip unwanted stuff from the filename
	if(WiiSettings.cleanFilenames)
		CleanFilename(playlist[playlistSize-1].displayname);

	return true;
}

/****************************************************************************
 * EnqueueFolder
 * 
 * Adds all music files in the specified folder to the playlist
 ***************************************************************************/
static bool EnqueueFolder(char * path)
{
	char filename[MAXJOLIET+1];
	char filepath[MAXPATHLEN];
	struct stat filestat;
	DIR_ITER *dir = diropen(path);

	if(dir == NULL)
	{
		char msg[1024];
		sprintf(msg, "Error opening %s", path);
		ErrorPrompt(msg);
		return false;
	}

	while(dirnext(dir,filename,&filestat) == 0)
	{
		if(filename[0] == '.')
			continue;

		sprintf(filepath, "%s/%s", path, filename);

		if(filestat.st_mode & _IFDIR)
			EnqueueFolder(filepath); // directory recursion doesn't work properly on SMB
		else
			EnqueueFile(filepath, filename);
	}
	return true;
}

bool MusicPlaylistEnqueue(int index)
{
	char fullpath[MAXPATHLEN+1];
	sprintf(fullpath, "%s%s", browser.dir, browserList[index].filename);
	
	if(browserList[index].isdir)
		browserList[index].icon = ICON_FOLDER_CHECKED;
	else
		browserList[index].icon = ICON_FILE_CHECKED;
	
	if(browserList[index].isdir)
		return EnqueueFolder(fullpath);
	else
		return EnqueueFile(fullpath, browserList[index].filename);
}

static void Remove(int index)
{
	if(index < playlistSize-1)
		memmove(&playlist[index], &playlist[index+1], sizeof(MEDIAENTRY)*(playlistSize-index-1));
	playlistSize--;
	playlist = (MEDIAENTRY *)realloc(playlist, playlistSize * sizeof(MEDIAENTRY));
}

void MusicPlaylistDequeue(int index)
{
	bool matchFound = false;
	char fullpath[MAXPATHLEN];
	sprintf(fullpath, "%s%s", browser.dir, browserList[index].filename);
	int len = strlen(fullpath);
	
	if(browserList[index].isdir)
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
