/****************************************************************************
 * WiiMC
 * Tantric 2009-2010
 *
 * musicplaylist.h
 *
 * Music playlist
 ****************************************************************************/

#ifndef _MUSICPLAYLIST_H_
#define _MUSICPLAYLIST_H_

void MusicPlaylistClear();
int MusicPlaylistLoad();
bool MusicPlaylistFind(char *path);
bool MusicPlaylistFind(int index);
bool MusicPlaylistEnqueue(int index);
void MusicPlaylistDequeue(int index);

#endif
