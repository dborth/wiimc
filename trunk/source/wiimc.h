/****************************************************************************
 * WiiMC
 * Tantric 2009
 *
 * mplayer.h
 ***************************************************************************/

#ifndef _MPLAYERGUI_H_
#define _MPLAYERGUI_H_

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
	DEVICE_FTP
};

#define NOTSILENT 0
#define SILENT 1

void ExitApp();
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

void plat_init (int size);
int mplayer_loadfile(const char* _file);
void DrawMPlayer();
bool DVDGekkoMount();
void log_console_init(GXRModeObj *vmode, u16 logsize);
//void log_console_deinit(void);
//void log_console_enable_log(bool enable);
void log_console_enable_video(bool enable);

bool FindIOS(u32 ios);   //in osdep/plat_gekko.c
bool load_ehci_module();  //in osdep/plat_gekko.c
void __exception_setreload(int t); //in osdep/plat_gekko.c
void USB2Enable(bool e); // in osdep/usb2storage.c

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
void wiiGetTimeDisplay(char * buf);
char * wiiGetMetaTitle();
char * wiiGetMetaArtist();
char * wiiGetMetaAlbum();
char * wiiGetMetaYear();
void wiiDVDNav(int cmd, int x, int y);
bool wiiInDVDMenu();

#ifdef __cplusplus
}
#endif

#endif
