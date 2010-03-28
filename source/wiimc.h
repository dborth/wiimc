/****************************************************************************
 * WiiMC
 * Tantric 2009-2010
 *
 * wiimc.h
 ***************************************************************************/

#ifndef _WIIMC_H_
#define _WIIMC_H_

#include "utils/FreeTypeGX.h"
#include "network.h"
#include "utils/libdvdiso.h"
#include "utils/mload.h"

#define APPNAME			"WiiMC"
#define APPVERSION		"1.0.0"
#define APPFOLDER		"wiimc"

enum {
	DEVICE_SD,
	DEVICE_USB,
	DEVICE_DVD,
	DEVICE_SMB,
	DEVICE_FTP,
	DEVICE_HTTP
};

#define NOTSILENT 0
#define SILENT 1

void ExitApp();
bool InitMPlayer();
void LoadMPlayer();
void ShutdownMPlayer();
void FindNextAudioFile();
extern int ScreenshotRequested;
extern int ConfigRequested;
extern int ShutdownRequested;
extern int ExitRequested;
extern FreeTypeGX *fontSystem[];
extern char loadedFile[];
extern char appPath[];
extern bool playingAudio;

#ifdef __cplusplus
extern "C" {
#endif

extern int controlledbygui;
int mplayer_loadfile(const char* _file); // in mplayer.c
void DrawMPlayer(); // in gx_supp.c

void wiiLoadRestorePoints(char *buffer, int size);
char * wiiSaveRestorePoints(char *path);
void wiiGotoGui();
void wiiPause();
bool wiiIsPaused();
void wiiMute();
void wiiSeekPos(int sec);
void wiiFastForward();
void wiiRewind();
void wiiSkipForward();
void wiiSkipBackward();
double wiiGetTimeLength();
double wiiGetTimePos();
void wiiGetTimeDisplay(char *buf);
bool wiiAudioOnly();
char * wiiGetMetaTitle();
char * wiiGetMetaArtist();
char * wiiGetMetaAlbum();
char * wiiGetMetaYear();
void wiiDVDNav(int cmd);
void wiiUpdatePointer(int x, int y);
bool wiiInDVDMenu();
void wiiSetCache(int size, int prefill);
void wiiSetAutoResume(int enable);
void wiiSetProperty(int cmd, float val);

#ifdef __cplusplus
}
#endif

#endif
