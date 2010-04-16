/****************************************************************************
 * WiiMC
 * Tantric 2009
 *
 * menu.h
 * Menu flow routines - handles all menu logic
 ***************************************************************************/

#ifndef _MENU_H_
#define _MENU_H_

#include <ogcsys.h>

#define MAX_PICTURE_SIZE (1024*1024*6) // 6 MB

void GuiInit();
void WiiMenu();
void MPlayerMenu();
void ErrorPrompt(const char * msg);
int ErrorPromptRetry(const char * msg);
void InfoPrompt(const char * msg);
void InfoPrompt(const char *title, const char *msg);
void OnScreenKeyboard(char * var, u32 maxlen);
void ShowAction (const char *msg);
void CancelAction();
int WindowPrompt(const char *title, const char *msg, const char *btn1Label, const char *btn2Label);
void SuspendPictureThread();
void ResetText();
bool VolumeLevelBarVisible();
void ShowVolumeLevelBar();
void HideVolumeLevelBar();

extern "C" {
void ShowProgress (const char *msg, int done, int total);
void SetStatus(const char * txt);
bool StatusSet();
}

extern int menuCurrent;
extern bool menuMode;
extern int doMPlayerGuiDraw;
extern bool nowPlayingSet;
extern u8 *picBuffer;

enum
{
	MENU_BROWSE_VIDEOS,
	MENU_BROWSE_MUSIC,
	MENU_BROWSE_PICTURES,
	MENU_DVD,
	MENU_BROWSE_ONLINEMEDIA,
	MENU_SETTINGS,
	MENU_SETTINGS_GLOBAL,
	MENU_SETTINGS_VIDEOS,
	MENU_SETTINGS_MUSIC,
	MENU_SETTINGS_PICTURES,
	MENU_SETTINGS_DVD,
	MENU_SETTINGS_ONLINEMEDIA,
	MENU_SETTINGS_NETWORK,
	MENU_SETTINGS_NETWORK_SMB,
	MENU_SETTINGS_NETWORK_FTP
};

#endif
