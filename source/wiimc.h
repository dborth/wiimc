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
#define APPVERSION		"1.0.3"
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
void SetSleepTimer();
void CheckSleepTimer();
bool InitMPlayer();
void LoadMPlayerFile();
void ResumeMPlayerFile();
void StopMPlayerFile();
extern int ScreenshotRequested;
extern int ConfigRequested;
extern int ShutdownRequested;
extern int ExitRequested;
extern FreeTypeGX *fontSystem[];
extern char loadedFile[];
extern char loadedFileDisplay[];
extern char appPath[];
extern char loadPath[];

extern "C" {
	
void FindNextFile(bool load);

extern int controlledbygui;
int mplayer_main(); // in mplayer.c

void wiiLoadFile(char *file);
void wiiGotoGui();
void wiiPause();
bool wiiIsPaused();
void wiiMute();
void wiiSetSeekTime(int sec);
void wiiSeekPos(int sec);
void wiiFastForward();
void wiiRewind();
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
bool wiiPlayingDVD();
bool wiiInDVDMenu();
void wiiSetCacheFill(int fill);
void wiiSetAutoResume(int enable);
void wiiSetVolume(int vol);
void wiiSetProperty(int cmd, float val);
void wiiSetLanguage(char *lang);
void wiiSetCodepage(char *cp);
void wiiSetSubtitleColor(char *color);
void wiiSetSubtitleSize(float size);
void wiiLoadRestorePoints(char *buffer, int size);
char * wiiSaveRestorePoints(char *path);

}

#endif
