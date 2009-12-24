/****************************************************************************
 * WiiMC
 * Tantric 2009
 *
 * menu.cpp
 * Menu flow routines - handles all menu logic
 ***************************************************************************/

#include <gccore.h>
#include <ogcsys.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <wiiuse/wpad.h>
#include <ogc/lwp_watchdog.h>

#include "libwiigui/gui.h"
#include "menu.h"
#include "wiimc.h"
#include "settings.h"
#include "fileop.h"
#include "input.h"
#include "networkop.h"
#include "filebrowser.h"
#include "utils/gettext.h"
#include "filelist.h"

#define THREAD_SLEEP 100

static GuiImageData * pointer[4] = { NULL, NULL, NULL, NULL };
static GuiImage * videoImg = NULL;
static GuiButton * videosBtn = NULL;
static GuiButton * musicBtn = NULL;
static GuiButton * picturesBtn = NULL;
static GuiButton * dvdBtn = NULL;
static GuiButton * onlineBtn = NULL;
static GuiButton * settingsBtn = NULL;
static GuiButton * exitBtn = NULL;

static GuiButton * logoBtn = NULL;
static GuiButton * mplayerBtn = NULL;
static GuiWindow * mainWindow = NULL;
static GuiText * settingText = NULL;

// playbar

static GuiText * statusText = NULL;

static GuiWindow * videobar = NULL;
static GuiWindow * audiobar = NULL;
static GuiWindow * picturebar = NULL;
static GuiTrigger * playbarTrigA = NULL;

static GuiImageData * playbarProgress = NULL;
static GuiImageData * playbarProgressLeft = NULL;
static GuiImageData * playbarProgressMid = NULL;
static GuiImageData * playbarProgressRight = NULL;
static GuiImageData * playbarCircle = NULL;
static GuiImageData * playbarCircleOver = NULL;
static GuiImageData * playbarSkipBackward = NULL;
static GuiImageData * playbarRewind = NULL;
static GuiImageData * playbarPause = NULL;
static GuiImageData * playbarPlay = NULL;
static GuiImageData * playbarFastForward = NULL;
static GuiImageData * playbarSkipForward = NULL;
static GuiImageData * playbarSlideshow = NULL;
static GuiImageData * playbarClose = NULL;

static GuiImage * videobarProgressImg = NULL;
static GuiImage * videobarProgressLeftImg = NULL;
static GuiImage * videobarProgressMidImg = NULL;
static GuiImage * videobarProgressRightImg = NULL;
static GuiImage * videobarSkipBackwardImg = NULL;
static GuiImage * videobarSkipBackwardOverImg = NULL;
static GuiImage * videobarSkipBackwardIcon = NULL;
static GuiImage * videobarRewindImg = NULL;
static GuiImage * videobarRewindOverImg = NULL;
static GuiImage * videobarRewindIcon = NULL;
static GuiImage * videobarPauseImg = NULL;
static GuiImage * videobarPauseOverImg = NULL;
static GuiImage * videobarPauseIcon = NULL;
static GuiImage * videobarPlayIcon = NULL;
static GuiImage * videobarFastForwardImg = NULL;
static GuiImage * videobarFastForwardOverImg = NULL;
static GuiImage * videobarFastForwardIcon = NULL;
static GuiImage * videobarSkipForwardImg = NULL;
static GuiImage * videobarSkipForwardOverImg = NULL;
static GuiImage * videobarSkipForwardIcon = NULL;

static GuiButton * videobarProgressBtn = NULL;
static GuiButton * videobarSkipBackwardBtn = NULL;
static GuiButton * videobarRewindBtn = NULL;
static GuiButton * videobarPauseBtn = NULL;
static GuiButton * videobarFastForwardBtn = NULL;
static GuiButton * videobarSkipForwardBtn = NULL;

static GuiText * videobarTime = NULL;

static GuiImage * audiobarProgressImg = NULL;
static GuiImage * audiobarProgressLeftImg = NULL;
static GuiImage * audiobarProgressMidImg = NULL;
static GuiImage * audiobarProgressRightImg = NULL;
static GuiImage * audiobarSkipBackwardImg = NULL;
static GuiImage * audiobarSkipBackwardOverImg = NULL;
static GuiImage * audiobarSkipBackwardIcon = NULL;
static GuiImage * audiobarPauseImg = NULL;
static GuiImage * audiobarPauseOverImg = NULL;
static GuiImage * audiobarPauseIcon = NULL;
static GuiImage * audiobarPlayIcon = NULL;
static GuiImage * audiobarSkipForwardImg = NULL;
static GuiImage * audiobarSkipForwardOverImg = NULL;
static GuiImage * audiobarSkipForwardIcon = NULL;

static GuiButton * audiobarProgressBtn = NULL;
static GuiButton * audiobarSkipBackwardBtn = NULL;
static GuiButton * audiobarPauseBtn = NULL;
static GuiButton * audiobarSkipForwardBtn = NULL;

static GuiText * audiobarNowPlaying[4] = { NULL, NULL };
static bool nowPlayingSet = false;

static GuiImage * picturebarPreviousImg = NULL;
static GuiImage * picturebarPreviousOverImg = NULL;
static GuiImage * picturebarPreviousIcon = NULL;
static GuiImage * picturebarNextImg = NULL;
static GuiImage * picturebarNextOverImg = NULL;
static GuiImage * picturebarNextIcon = NULL;
static GuiImage * picturebarSlideshowImg = NULL;
static GuiImage * picturebarSlideshowOverImg = NULL;
static GuiImage * picturebarSlideshowIcon = NULL;
static GuiImage * picturebarCloseImg = NULL;
static GuiImage * picturebarCloseOverImg = NULL;
static GuiImage * picturebarCloseIcon = NULL;

static GuiButton * picturebarPreviousBtn = NULL;
static GuiButton * picturebarNextBtn = NULL;
static GuiButton * picturebarSlideshowBtn = NULL;
static GuiButton * picturebarCloseBtn = NULL;

int currentMenu = MENU_BROWSE_VIDEOS;
bool inPlaylist = false;
static int lastMenu = MENU_BROWSE_VIDEOS;
static int netEditIndex = 0; // current index of FTP/SMB share being edited

static lwp_t guithread = LWP_THREAD_NULL;
static lwp_t progressthread = LWP_THREAD_NULL;
static lwp_t creditsthread = LWP_THREAD_NULL;
static lwp_t updatethread = LWP_THREAD_NULL;
static bool guiHalt = true;
static bool guiShutdown = true;
static int showProgress = 0;

static char progressTitle[100];
static char progressMsg[200];
static int progressDone = 0;
static int progressTotal = 0;

static bool creditsOpen = false;

int doMPlayerGuiDraw = 0; // draw MPlayer menu
static bool menuMode = 0; // 0 - normal GUI, 1 - GUI for MPlayer

static void ChangeMenu(int menu)
{
	if(menu == currentMenu)
		return;

	lastMenu = currentMenu;
	currentMenu = menu;
}
static void ChangeMenu(void * ptr, int menu)
{
	GuiButton * b = (GuiButton *)ptr;
	if(b->GetState() == STATE_CLICKED)
	{
		ChangeMenu(menu);
		b->ResetState();
	}
}
static void ChangeMenuVideos(void * ptr) { ChangeMenu(ptr, MENU_BROWSE_VIDEOS); }
static void ChangeMenuMusic(void * ptr) { ChangeMenu(ptr, MENU_BROWSE_MUSIC); }
static void ChangeMenuPictures(void * ptr) { ChangeMenu(ptr, MENU_BROWSE_PICTURES); }
static void ChangeMenuDVD(void * ptr) { ChangeMenu(ptr, MENU_DVD); }
static void ChangeMenuOnline(void * ptr) { ChangeMenu(ptr, MENU_BROWSE_ONLINEMEDIA); }
static void ChangeMenuSettings(void * ptr) { ChangeMenu(ptr, MENU_SETTINGS); }

static void ExitButtonCallback(void * ptr)
{
	GuiButton * b = (GuiButton *)ptr;
	if(b->GetState() == STATE_CLICKED)
	{
		b->ResetState();
		ExitRequested = 1;
	}
}

/****************************************************************************
 * AppUpdate
 *
 * Prompts for confirmation, and downloads/installs updates
 ***************************************************************************/
static void *
AppUpdate (void *arg)
{
	bool installUpdate = WindowPrompt(
		"Update Available",
		"An update is available!",
		"Update now",
		"Update later");
	if(installUpdate)
		if(DownloadUpdate())
			ExitRequested = 1;
	return NULL;
}

/****************************************************************************
 * UpdateGui
 *
 * Primary GUI thread to allow GUI to respond to state changes, and draws GUI
 ***************************************************************************/
static void *
UpdateGui (void *arg)
{
	int i;

	while(1)
	{
		if(guiHalt)
			break;

		UpdatePads();

		if(menuMode == 1)
			MPlayerInput();

		if(menuMode == 0 || doMPlayerGuiDraw)
		{
			mainWindow->Draw();

			if (mainWindow->GetState() != STATE_DISABLED)
				mainWindow->DrawTooltip();

			for(i=3; i >= 0; i--) // so that player 1's cursor appears on top!
			{
				if(userInput[i].wpad->ir.valid)
					Menu_DrawImg(userInput[i].wpad->ir.x-48, userInput[i].wpad->ir.y-48,
						96, 96, pointer[i]->GetImage(), userInput[i].wpad->ir.angle, 1, 1, 255);
				DoRumble(i);
			}
			doMPlayerGuiDraw = 0;
		}

		for(i=3; i >= 0; i--)
		{
			mainWindow->Update(&userInput[i]);
		}

		if(menuMode == 0) // normal GUI
		{
			for(i=3; i >= 0; i--)
			{
				if(userInput[i].wpad->btns_d & WPAD_BUTTON_PLUS)
				{
					int newMenu = currentMenu + 1;
					if(newMenu > MENU_SETTINGS)
						newMenu = MENU_BROWSE_VIDEOS;
					ChangeMenu(newMenu);
				}
				else if(userInput[i].wpad->btns_d & WPAD_BUTTON_MINUS)
				{
					int newMenu = currentMenu - 1;
					if(newMenu < MENU_BROWSE_VIDEOS)
						newMenu = MENU_SETTINGS;
					ChangeMenu(newMenu);
				}
			}

			Menu_Render();

			if(updateFound)
			{
				updateFound = false;
				LWP_CreateThread (&updatethread, AppUpdate, NULL, NULL, 0, 60);
			}
			
			if(!creditsOpen && creditsthread != LWP_THREAD_NULL)
			{
				LWP_JoinThread(creditsthread, NULL);
				creditsthread = LWP_THREAD_NULL;
			}

			if(userInput[0].wpad->btns_d & WPAD_BUTTON_HOME)
			{
				if(videoScreenshot) // a video is loaded
					LoadMPlayer(); // go back to MPlayer
				else
					ExitRequested = 1; // exit program
			}

			if(ExitRequested || ShutdownRequested)
			{
				for(i = 0; i < 255; i += 15)
				{
					mainWindow->Draw();
					Menu_DrawRectangle(0,0,screenwidth,screenheight,(GXColor){0, 0, 0, i},1);
					Menu_Render();
				}
				ExitApp();
			}
		}
		usleep(THREAD_SLEEP);
	}
	return NULL;
}

/****************************************************************************
 * ResumeGui
 *
 * Signals the GUI thread to start, and resumes the thread. This is called
 * after finishing the removal/insertion of new elements, and after initial
 * GUI setup.
 ***************************************************************************/
static void
ResumeGui()
{
	guiHalt = false;

	if(guithread == LWP_THREAD_NULL)
		LWP_CreateThread (&guithread, UpdateGui, NULL, NULL, 0, 66);
}

/****************************************************************************
 * HaltGui
 *
 * Signals the GUI thread to stop, and waits for GUI thread to stop
 * This is necessary whenever removing/inserting new elements into the GUI.
 * This eliminates the possibility that the GUI is in the middle of accessing
 * an element that is being changed.
 ***************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

static void
HaltGui()
{
	guiHalt = true;

	if(guithread == LWP_THREAD_NULL)
		return;

	// wait for thread to finish
	LWP_JoinThread(guithread, NULL);
	guithread = LWP_THREAD_NULL;
}

void ShutdownGui()
{
	CancelAction();
	HaltGui();
	guiShutdown = true;
}

#ifdef __cplusplus
}
#endif

void ResetText()
{
	LoadLanguage();

	if(mainWindow)
		mainWindow->ResetText();
}

/****************************************************************************
 * WindowPrompt
 *
 * Displays a prompt window to user, with information, an error message, or
 * presenting a user with a choice
 ***************************************************************************/
int
WindowPrompt(const char *title, const char *msg, const char *btn1Label, const char *btn2Label)
{
	int choice = -1;

	GuiWindow promptWindow(448,288);
	promptWindow.SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	promptWindow.SetPosition(0, -10);
	GuiImageData btnOutline(button_png);
	GuiImageData btnOutlineOver(button_over_png);
	GuiTrigger trigA;
	trigA.SetSimpleTrigger(-1, WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A, PAD_BUTTON_A);

	GuiImageData dialogBox(dialogue_box_png);
	GuiImage dialogBoxImg(&dialogBox);

	GuiText titleTxt(title, 26, (GXColor){0, 0, 0, 255});
	titleTxt.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	titleTxt.SetPosition(0,40);
	GuiText msgTxt(msg, 22, (GXColor){0, 0, 0, 255});
	msgTxt.SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	msgTxt.SetPosition(0,-20);
	msgTxt.SetWrap(true, 430);

	GuiText btn1Txt(btn1Label, 22, (GXColor){255, 255, 255, 255});
	GuiImage btn1Img(&btnOutline);
	GuiImage btn1ImgOver(&btnOutlineOver);
	GuiButton btn1(btnOutline.GetWidth(), btnOutline.GetHeight());

	if(btn2Label)
	{
		btn1.SetAlignment(ALIGN_LEFT, ALIGN_BOTTOM);
		btn1.SetPosition(20, -40);
	}
	else
	{
		btn1.SetAlignment(ALIGN_CENTRE, ALIGN_BOTTOM);
		btn1.SetPosition(0, -40);
	}

	btn1.SetLabel(&btn1Txt);
	btn1.SetImage(&btn1Img);
	btn1.SetImageOver(&btn1ImgOver);
	btn1.SetTrigger(&trigA);
	btn1.SetState(STATE_SELECTED);
	btn1.SetEffectGrow();

	GuiText btn2Txt(btn2Label, 22, (GXColor){255, 255, 255, 255});
	GuiImage btn2Img(&btnOutline);
	GuiImage btn2ImgOver(&btnOutlineOver);
	GuiButton btn2(btnOutline.GetWidth(), btnOutline.GetHeight());
	btn2.SetAlignment(ALIGN_RIGHT, ALIGN_BOTTOM);
	btn2.SetPosition(-20, -40);
	btn2.SetLabel(&btn2Txt);
	btn2.SetImage(&btn2Img);
	btn2.SetImageOver(&btn2ImgOver);
	btn2.SetTrigger(&trigA);
	btn2.SetEffectGrow();

	promptWindow.Append(&dialogBoxImg);
	promptWindow.Append(&titleTxt);
	promptWindow.Append(&msgTxt);
	promptWindow.Append(&btn1);

	if(btn2Label)
		promptWindow.Append(&btn2);

	promptWindow.SetEffect(EFFECT_SLIDE_TOP | EFFECT_SLIDE_IN, 50);
	CancelAction();
	HaltGui();
	mainWindow->SetState(STATE_DISABLED);
	mainWindow->Append(&promptWindow);
	mainWindow->ChangeFocus(&promptWindow);
	ResumeGui();

	while(choice == -1)
	{
		usleep(THREAD_SLEEP);

		if(btn1.GetState() == STATE_CLICKED)
			choice = 1;
		else if(btn2.GetState() == STATE_CLICKED)
			choice = 0;
	}

	promptWindow.SetEffect(EFFECT_SLIDE_TOP | EFFECT_SLIDE_OUT, 50);
	while(promptWindow.GetEffect() > 0) usleep(THREAD_SLEEP);
	HaltGui();
	mainWindow->Remove(&promptWindow);
	mainWindow->SetState(STATE_DEFAULT);
	ResumeGui();
	return choice;
}

/****************************************************************************
 * ProgressWindow
 *
 * Opens a window, which displays progress to the user. Can either display a
 * progress bar showing % completion, or a throbber that only shows that an
 * action is in progress.
 ***************************************************************************/
static void
ProgressWindow(char *title, char *msg)
{
	GuiWindow promptWindow(448,288);
	promptWindow.SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	promptWindow.SetPosition(0, -10);
	GuiImageData btnOutline(button_png);
	GuiImageData btnOutlineOver(button_over_png);
	GuiTrigger trigA;
	trigA.SetSimpleTrigger(-1, WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A, PAD_BUTTON_A);

	GuiImageData dialogBox(dialogue_box_png);
	GuiImage dialogBoxImg(&dialogBox);

	GuiImageData progressbarOutline(progressbar_outline_png);
	GuiImage progressbarOutlineImg(&progressbarOutline);
	progressbarOutlineImg.SetAlignment(ALIGN_LEFT, ALIGN_MIDDLE);
	progressbarOutlineImg.SetPosition(25, 40);

	GuiImageData progressbarEmpty(progressbar_empty_png);
	GuiImage progressbarEmptyImg(&progressbarEmpty);
	progressbarEmptyImg.SetAlignment(ALIGN_LEFT, ALIGN_MIDDLE);
	progressbarEmptyImg.SetPosition(25, 40);
	progressbarEmptyImg.SetTile(100);

	GuiImageData progressbar(progressbar_png);
	GuiImage progressbarImg(&progressbar);
	progressbarImg.SetAlignment(ALIGN_LEFT, ALIGN_MIDDLE);
	progressbarImg.SetPosition(25, 40);

	GuiImageData throbber(throbber_png);
	GuiImage throbberImg(&throbber);
	throbberImg.SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	throbberImg.SetPosition(0, 40);

	GuiText titleTxt(title, 26, (GXColor){0, 0, 0, 255});
	titleTxt.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	titleTxt.SetPosition(0,40);
	GuiText msgTxt(msg, 26, (GXColor){0, 0, 0, 255});
	msgTxt.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	msgTxt.SetPosition(0,80);

	promptWindow.Append(&dialogBoxImg);
	promptWindow.Append(&titleTxt);
	promptWindow.Append(&msgTxt);

	if(showProgress == 1)
	{
		promptWindow.Append(&progressbarEmptyImg);
		promptWindow.Append(&progressbarImg);
		promptWindow.Append(&progressbarOutlineImg);
	}
	else
	{
		promptWindow.Append(&throbberImg);
	}

	usleep(400000); // wait to see if progress flag changes soon
	if(!showProgress)
		return;

	HaltGui();
	mainWindow->SetState(STATE_DISABLED);
	mainWindow->Append(&promptWindow);
	mainWindow->ChangeFocus(&promptWindow);
	ResumeGui();

	float angle = 0;
	u32 count = 0;

	while(showProgress)
	{
		usleep(20000);

		if(showProgress == 1)
		{
			progressbarImg.SetTile(100*progressDone/progressTotal);
		}
		else if(showProgress == 2)
		{
			if(count % 5 == 0)
			{
				angle+=45;
				if(angle >= 360)
					angle = 0;
				throbberImg.SetAngle(angle);
			}
			count++;
		}
	}

	HaltGui();
	mainWindow->Remove(&promptWindow);
	mainWindow->SetState(STATE_DEFAULT);
	ResumeGui();
}

static void * ProgressThread (void *arg)
{
	while(1)
	{
		if(!showProgress)
			break;

		ProgressWindow(progressTitle, progressMsg);
		usleep(THREAD_SLEEP);
	}
	return NULL;
}

/****************************************************************************
 * CancelAction
 *
 * Signals the GUI progress window thread to halt, and waits for it to
 * finish. Prevents multiple progress window events from interfering /
 * overriding each other.
 ***************************************************************************/
void
CancelAction()
{
	showProgress = 0;

	if(progressthread == LWP_THREAD_NULL)
		return;

	// wait for thread to finish
	LWP_JoinThread(progressthread, NULL);
	progressthread = LWP_THREAD_NULL;
}

/****************************************************************************
 * ShowProgress
 *
 * Updates the variables used by the progress window for drawing a progress
 * bar. Also resumes the progress window thread if it is suspended.
 ***************************************************************************/
void
ShowProgress (const char *msg, int done, int total)
{
	if(!mainWindow || ExitRequested || ShutdownRequested)
		return;

	if(total < (256*1024))
		return;
	else if(done > total) // this shouldn't happen
		done = total;

	if(done/total > 0.99)
		done = total;

	if(showProgress != 1)
		CancelAction(); // wait for previous progress window to finish

	strncpy(progressMsg, msg, 200);
	sprintf(progressTitle, "Please Wait");
	showProgress = 1;
	progressTotal = total;
	progressDone = done;

	if(progressthread == LWP_THREAD_NULL)
		LWP_CreateThread (&progressthread, ProgressThread, NULL, NULL, 0, 60);
}

/****************************************************************************
 * ShowAction
 *
 * Shows that an action is underway. Also resumes the progress window thread
 * if it is suspended.
 ***************************************************************************/
void
ShowAction (const char *msg)
{
	if(!mainWindow || ExitRequested || ShutdownRequested)
		return;

	if(showProgress != 2)
		CancelAction(); // wait for previous progress window to finish

	strncpy(progressMsg, msg, 200);
	sprintf(progressTitle, "Please Wait");
	showProgress = 2;
	progressDone = 0;
	progressTotal = 0;

	if(progressthread == LWP_THREAD_NULL)
		LWP_CreateThread (&progressthread, ProgressThread, NULL, NULL, 0, 60);
}

void ErrorPrompt(const char *msg)
{
	WindowPrompt("Error", msg, "OK", NULL);
}

int ErrorPromptRetry(const char *msg)
{
	return WindowPrompt("Error", msg, "Retry", "Cancel");
}

void InfoPrompt(const char *msg)
{
	WindowPrompt("Information", msg, "OK", NULL);
}

/****************************************************************************
 * OnScreenKeyboard
 *
 * Opens an on-screen keyboard window, with the data entered being stored
 * into the specified variable.
 ***************************************************************************/
static void OnScreenKeyboard(char * var, u16 maxlen)
{
	int save = -1;

	GuiKeyboard keyboard(var, maxlen);

	GuiImageData btnOutline(button_png);
	GuiImageData btnOutlineOver(button_over_png);
	GuiTrigger trigA;
	trigA.SetSimpleTrigger(-1, WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A, PAD_BUTTON_A);

	GuiText okBtnTxt("OK", 22, (GXColor){255, 255, 255, 255});
	GuiImage okBtnImg(&btnOutline);
	GuiImage okBtnImgOver(&btnOutlineOver);
	GuiButton okBtn(btnOutline.GetWidth(), btnOutline.GetHeight());

	okBtn.SetAlignment(ALIGN_LEFT, ALIGN_BOTTOM);
	okBtn.SetPosition(25, -25);

	okBtn.SetLabel(&okBtnTxt);
	okBtn.SetImage(&okBtnImg);
	okBtn.SetImageOver(&okBtnImgOver);
	okBtn.SetTrigger(&trigA);
	okBtn.SetEffectGrow();

	GuiText cancelBtnTxt("Cancel", 22, (GXColor){255, 255, 255, 255});
	GuiImage cancelBtnImg(&btnOutline);
	GuiImage cancelBtnImgOver(&btnOutlineOver);
	GuiButton cancelBtn(btnOutline.GetWidth(), btnOutline.GetHeight());
	cancelBtn.SetAlignment(ALIGN_RIGHT, ALIGN_BOTTOM);
	cancelBtn.SetPosition(-25, -25);
	cancelBtn.SetLabel(&cancelBtnTxt);
	cancelBtn.SetImage(&cancelBtnImg);
	cancelBtn.SetImageOver(&cancelBtnImgOver);
	cancelBtn.SetTrigger(&trigA);
	cancelBtn.SetEffectGrow();

	keyboard.Append(&okBtn);
	keyboard.Append(&cancelBtn);

	HaltGui();
	mainWindow->SetState(STATE_DISABLED);
	mainWindow->Append(&keyboard);
	mainWindow->ChangeFocus(&keyboard);
	ResumeGui();

	while(save == -1)
	{
		usleep(THREAD_SLEEP);

		if(okBtn.GetState() == STATE_CLICKED)
			save = 1;
		else if(cancelBtn.GetState() == STATE_CLICKED)
			save = 0;
	}

	if(save)
	{
		snprintf(var, maxlen, "%s", keyboard.kbtextstr);
	}

	HaltGui();
	mainWindow->Remove(&keyboard);
	mainWindow->SetState(STATE_DEFAULT);
	ResumeGui();
}

/****************************************************************************
 * SettingWindow
 *
 * Opens a new window, with the specified window element appended. Allows
 * for a customizable prompted setting.
 ***************************************************************************/
static int
SettingWindow(const char * title, GuiWindow * w)
{
	int save = -1;

	GuiWindow promptWindow(448,288);
	promptWindow.SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	GuiImageData btnOutline(button_png);
	GuiImageData btnOutlineOver(button_over_png);
	GuiTrigger trigA;
	trigA.SetSimpleTrigger(-1, WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A, PAD_BUTTON_A);

	GuiImageData dialogBox(dialogue_box_png);
	GuiImage dialogBoxImg(&dialogBox);

	GuiText titleTxt(title, 26, (GXColor){70, 70, 10, 255});
	titleTxt.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	titleTxt.SetPosition(0,40);

	GuiText okBtnTxt("OK", 24, (GXColor){255, 255, 255, 255});
	GuiImage okBtnImg(&btnOutline);
	GuiImage okBtnImgOver(&btnOutlineOver);
	GuiButton okBtn(btnOutline.GetWidth(), btnOutline.GetHeight());

	okBtn.SetAlignment(ALIGN_LEFT, ALIGN_BOTTOM);
	okBtn.SetPosition(20, -25);

	okBtn.SetLabel(&okBtnTxt);
	okBtn.SetImage(&okBtnImg);
	okBtn.SetImageOver(&okBtnImgOver);
	okBtn.SetTrigger(&trigA);
	okBtn.SetEffectGrow();

	GuiText cancelBtnTxt("Cancel", 24, (GXColor){255, 255, 255, 255});
	GuiImage cancelBtnImg(&btnOutline);
	GuiImage cancelBtnImgOver(&btnOutlineOver);
	GuiButton cancelBtn(btnOutline.GetWidth(), btnOutline.GetHeight());
	cancelBtn.SetAlignment(ALIGN_RIGHT, ALIGN_BOTTOM);
	cancelBtn.SetPosition(-20, -25);
	cancelBtn.SetLabel(&cancelBtnTxt);
	cancelBtn.SetImage(&cancelBtnImg);
	cancelBtn.SetImageOver(&cancelBtnImgOver);
	cancelBtn.SetTrigger(&trigA);
	cancelBtn.SetEffectGrow();

	promptWindow.Append(&dialogBoxImg);
	promptWindow.Append(&titleTxt);
	promptWindow.Append(&okBtn);
	promptWindow.Append(&cancelBtn);

	HaltGui();
	mainWindow->SetState(STATE_DISABLED);
	mainWindow->Append(&promptWindow);
	mainWindow->Append(w);
	mainWindow->ChangeFocus(w);
	ResumeGui();

	while(save == -1)
	{
		usleep(THREAD_SLEEP);

		if(okBtn.GetState() == STATE_CLICKED)
			save = 1;
		else if(cancelBtn.GetState() == STATE_CLICKED)
			save = 0;
	}
	HaltGui();
	mainWindow->Remove(&promptWindow);
	mainWindow->Remove(w);
	mainWindow->SetState(STATE_DEFAULT);
	ResumeGui();
	return save;
}

// Picture Viewer
static int changePicture = 0; // load a new picture
static int closePictureViewer = 0; // is picture viewer open
static int slideshow = 0; // slideshow mode
static u64 slideprev, slidenow; // slideshow timer

static void ChangePicture(int dir)
{
	browser.selIndex += dir;

	if(browser.selIndex >= browser.numEntries)
		browser.selIndex = 0;
	else if(browser.selIndex < 0)
		browser.selIndex = browser.numEntries-1;

	sprintf(loadedFile, "%s%s", browser.dir, browserList[browser.selIndex].filename);
	printf("now loading: %s\n", loadedFile);
	changePicture = 1;
}

static void ToggleSlideshow()
{
	if(slideshow == 0)
		slideprev = gettime(); // setup timer

	slideshow ^= 1;
}

static void PictureViewer()
{
	closePictureViewer = 0;
	changePicture = 1;
	slideshow = 0;
	u8 * picBuffer = NULL;
	GuiWindow * oldWindow = mainWindow;
	GuiImageData * picture = NULL;
	GuiImage pictureImg;
	pictureImg.SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);

	HaltGui();
	GuiWindow w(screenwidth, screenheight);
	w.Append(&pictureImg);
	w.Append(picturebar);
	mainWindow = &w;
	ResumeGui();

	while(closePictureViewer == 0)
	{
		if(changePicture)
		{
			printf("changing picture!\n");
			printf("selected index: %d, length: %d\n", browser.selIndex, (int)browserList[browser.selIndex].length);
			printf("mem1: %d, mem2: %d\n", SYS_GetArena1Size(), SYS_GetArena2Size());
			picBuffer = (u8 *)malloc(browserList[browser.selIndex].length);
			printf("done allocating!\n");
			if(!picBuffer)
			{
				printf("could not allocate picture buffer!\n");
				break;
			}

			printf("loading file: %s\n", loadedFile);
			int size = LoadFile((char *)picBuffer, loadedFile, NOTSILENT);

			if(!size)
			{
				printf("could not load file!\n");
				break;
			}
			printf("done loading file\n");
			HaltGui();
			if(picture)	delete picture;
			picture = new GuiImageData(picBuffer, size);
			free(picBuffer);

			if(picture->GetImage() == NULL)
			{
				printf("image data empty - could not create image!\n");
				break;
			}

			pictureImg.SetImage(picture);
			ResumeGui();
			changePicture = 0;
		}

		if(slideshow) // slideshow mode - change every 3 seconds
		{
			slidenow = gettime();
			if(diff_usec(slideprev, slidenow) > 1000*1000*3)
			{
				ChangePicture(1); // change to next picture
				slideprev = slidenow; // reset timer
			}
		}
		usleep(THREAD_SLEEP);
	}
	printf("done!\n");
	HaltGui();
	mainWindow = oldWindow;
	ResumeGui();
	if(picBuffer) free(picBuffer);
	if(picture)	delete picture;
}


/****************************************************************************
 * WindowCredits
 * Display credits, legal copyright and licence
 *
 * THIS MUST NOT BE REMOVED OR DISABLED IN ANY DERIVATIVE WORK
 ***************************************************************************/
static void * WindowCredits(void *arg)
{
	bool exit = false;
	int i = 0;
	int y = 20;

	GuiWindow creditsWindow(580,448);
	creditsWindow.SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);

	GuiImageData creditsBox(credits_box_png);
	GuiImage creditsBoxImg(&creditsBox);
	creditsBoxImg.SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	creditsWindow.Append(&creditsBoxImg);

	int numEntries = 23;
	GuiText * txt[numEntries];

	txt[i] = new GuiText("Credits", 30, (GXColor){0, 0, 0, 255});
	txt[i]->SetAlignment(ALIGN_CENTRE, ALIGN_TOP); txt[i]->SetPosition(0,y); i++; y+=32;

	txt[i] = new GuiText("Official Site: http://code.google.com/p/mplayer-ce/", 20, (GXColor){0, 0, 0, 255});
	txt[i]->SetAlignment(ALIGN_CENTRE, ALIGN_TOP); txt[i]->SetPosition(0,y); i++; y+=40;

	txt[i]->SetPresets(20, (GXColor){0, 0, 0, 255}, 0,
			FTGX_JUSTIFY_LEFT | FTGX_ALIGN_TOP, ALIGN_LEFT, ALIGN_TOP);

	txt[i] = new GuiText("rodries");
	txt[i]->SetPosition(50,y); i++;
	txt[i] = new GuiText("Coding");
	txt[i]->SetPosition(320,y); i++; y+=22;
	txt[i] = new GuiText("Tantric");
	txt[i]->SetPosition(50,y); i++;
	txt[i] = new GuiText("Coding & menu design");
	txt[i]->SetPosition(320,y); i++; y+=22;
	txt[i] = new GuiText("drmr");
	txt[i]->SetPosition(50,y); i++;
	txt[i] = new GuiText("Menu artwork");
	txt[i]->SetPosition(320,y); i++; y+=22;
	txt[i] = new GuiText("scip");
	txt[i]->SetPosition(50,y); i++;
	txt[i] = new GuiText("Original project author");
	txt[i]->SetPosition(320,y); i++; y+=22;
	txt[i] = new GuiText("AgentX");
	txt[i]->SetPosition(50,y); i++;
	txt[i] = new GuiText("Coding & testing");
	txt[i]->SetPosition(320,y); i++; y+=22;
	txt[i] = new GuiText("DJDynamite123");
	txt[i]->SetPosition(50,y); i++;
	txt[i] = new GuiText("Testing");
	txt[i]->SetPosition(320,y); i++; y+=44;

	txt[i] = new GuiText("Thanks also to:");
	txt[i]->SetPosition(50,y); i++; y+=36;

	txt[i] = new GuiText("MPlayer Team");
	txt[i]->SetPosition(50,y); i++; y+=22;

	txt[i] = new GuiText("libogc / devkitPPC");
	txt[i]->SetPosition(50,y); i++;
	txt[i] = new GuiText("shagkur & wintermute");
	txt[i]->SetPosition(320,y); i++; y+=36;

	txt[i] = new GuiText("Team Twiizers, Armin Tamzarian, Daca, dargllun,");
	txt[i]->SetPosition(50,y); i++; y+=22;
	txt[i] = new GuiText("Extrems, GeeXboX Authors, hax, Shareese, tipolosko");
	txt[i]->SetPosition(50,y); i++; y+=22;

	txt[i]->SetPresets(18, (GXColor){0, 0, 0, 255}, 0,
		FTGX_JUSTIFY_CENTER | FTGX_ALIGN_TOP, ALIGN_CENTRE, ALIGN_TOP);

	txt[i] = new GuiText("This software is open source and may be copied,");
	txt[i]->SetPosition(0,y); i++; y+=20;
	txt[i] = new GuiText("distributed, or modified under the terms of the");
	txt[i]->SetPosition(0,y); i++; y+=20;
	txt[i] = new GuiText("GNU General Public License (GPL) Version 2.");
	txt[i]->SetPosition(0,y); i++; y+=20;

	for(i=0; i < numEntries; i++)
		creditsWindow.Append(txt[i]);

	HaltGui();
	mainWindow->SetState(STATE_DISABLED);
	mainWindow->Append(&creditsWindow);
	mainWindow->ChangeFocus(&creditsWindow);
	ResumeGui();
	
	while(!exit)
	{
		for(i=0; i < 4; i++)
		{
			if(userInput[i].wpad->btns_d || userInput[i].pad.btns_d)
				exit = true;
		}
		usleep(THREAD_SLEEP);
	}

	HaltGui();
	mainWindow->Remove(&creditsWindow);
	mainWindow->SetState(STATE_DEFAULT);
	ResumeGui();
	
	for(i=0; i < numEntries; i++)
		delete txt[i];
	creditsOpen = false;
	return NULL;
}

static void DisplayCredits(void * ptr)
{
	if(logoBtn->GetState() != STATE_CLICKED)
		return;

	logoBtn->ResetState();
	
	// spawn a new thread to handle the Credits
	creditsOpen = true;
	if(creditsthread == LWP_THREAD_NULL)
		LWP_CreateThread (&creditsthread, WindowCredits, NULL, NULL, 0, 60);
}

/****************************************************************************
 * MenuBrowse
 ***************************************************************************/

static void MenuBrowse(int menu)
{
	ShutoffRumble();
	browserPlaylist[0] = 0;
	inPlaylist = false;
	
	switch(menu)
	{
		case MENU_BROWSE_VIDEOS:
			browser.dir = &WiiSettings.videoFolder[0]; break;
		case MENU_BROWSE_MUSIC:
			browser.dir = &WiiSettings.musicFolder[0]; break;
		case MENU_BROWSE_PICTURES:
			browser.dir = &WiiSettings.pictureFolder[0]; break;
		case MENU_BROWSE_ONLINEMEDIA:
			browser.dir = &WiiSettings.onlinemediaFolder[0]; break;
		default:
			return;
	}

	GuiTrigger trigA;
	trigA.SetSimpleTrigger(-1, WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A, PAD_BUTTON_A);

	GuiFileBrowser fileBrowser(530, 240);
	fileBrowser.SetAlignment(ALIGN_LEFT, ALIGN_TOP);
	fileBrowser.SetPosition(58, 100);

	GuiTooltip playlistBtnTip("Playlist");
	GuiImageData playlistImg(nav_playlist_png);
	GuiImage playlistBtnImg(&playlistImg);
	GuiText playlistBtnTxt(NULL, 18, (GXColor){0, 0, 0, 255});
	playlistBtnTxt.SetAlignment(ALIGN_LEFT, ALIGN_MIDDLE);
	playlistBtnTxt.SetPosition(-20, 0);
	GuiButton playlistBtn(playlistBtnImg.GetWidth(), playlistBtnImg.GetHeight());
	playlistBtn.SetAlignment(ALIGN_LEFT, ALIGN_BOTTOM);
	playlistBtn.SetPosition(48, -30);
	playlistBtn.SetTooltip(&playlistBtnTip);
	playlistBtn.SetImage(&playlistBtnImg);
	playlistBtn.SetLabel(&playlistBtnTxt);
	playlistBtn.SetTrigger(&trigA);
	playlistBtn.SetEffectGrow();
	playlistBtn.SetAlpha(128);

	HaltGui();
	mainWindow->Append(&fileBrowser);

	if(menu == MENU_BROWSE_MUSIC) // add playlist functionality
	{
		char txt[10];
		sprintf(txt, "%d", playlistSize);
		playlistBtnTxt.SetText(txt);

		mainWindow->Append(&playlistBtn);
	}

	ResumeGui();

	// populate initial directory listing
	while(BrowserChangeFolder(false) <= 0)
	{
		int choice = WindowPrompt(
		"Error",
		"Unable to display files on selected load device.",
		"Retry",
		"Check Settings");

		if(choice == 0)
		{
			ChangeMenu(MENU_SETTINGS);
			goto done;
		}
	}

	while(currentMenu == menu && !guiShutdown)
	{
		usleep(THREAD_SLEEP);
		
		if(selectLoadedFile == 2)
		{
			selectLoadedFile = 0;
			mainWindow->ChangeFocus(&fileBrowser);
			fileBrowser.TriggerUpdate();
		}

		// devices were inserted or removed - update the filebrowser!
		if(devicesChanged)
		{
			devicesChanged = false;

			if(BrowserChangeFolder(false))
			{
				fileBrowser.ResetState();
				fileBrowser.fileList[0]->SetState(STATE_SELECTED);
				fileBrowser.TriggerUpdate();
			}
			else
			{
				goto done;
			}
		}
		
		if(inPlaylist && fileBrowser.fileList[0]->GetState() == STATE_CLICKED)
		{
			fileBrowser.fileList[0]->ResetState();
			inPlaylist = false;
			BrowserChangeFolder(false);
		}

		// update file browser based on arrow buttons
		// request guiShutdown if A button pressed on a file
		for(int i=0; i<FILE_PAGESIZE; i++)
		{
			if(fileBrowser.fileList[i]->GetState() == STATE_CLICKED)
			{
				fileBrowser.fileList[i]->ResetState();

				// check corresponding browser entry
				if(browserList[browser.selIndex].isdir)
				{
					if(BrowserChangeFolder())
					{
						fileBrowser.ResetState();
						fileBrowser.fileList[0]->SetState(STATE_SELECTED);
						fileBrowser.TriggerUpdate();
					}
					else
					{
						goto done;
					}
				}
				else
				{
					if(browserList[browser.selIndex].isplaylist)
					{
						// parse list
						if(currentMenu == MENU_BROWSE_ONLINEMEDIA)
							sprintf(browserPlaylist, "%s", browserList[browser.selIndex].filename);
						else
							sprintf(browserPlaylist, "%s%s", browser.dir, browserList[browser.selIndex].filename);
						
						int numItems = BrowserChangeFolder();

						if(numItems == 0)
						{
							browserPlaylist[0] = 0;
							ErrorPrompt("Error loading playlist!");
							continue;
						}
						else if(numItems == 1) // let's load this one file
						{
							sprintf(loadedFile, browserList[1].filename);
							// go up one level
							browser.selIndex = 0;
							BrowserChangeFolder();
						}
						else
						{
							fileBrowser.ResetState();
							fileBrowser.fileList[0]->SetState(STATE_SELECTED);
							fileBrowser.TriggerUpdate();
							continue;
						}
					}
					else
					{
						if(inPlaylist || browserPlaylist[0] != 0 || currentMenu == MENU_BROWSE_ONLINEMEDIA)
							sprintf(loadedFile, "%s", browserList[browser.selIndex].filename);
						else
							sprintf(loadedFile, "%s%s", browser.dir, browserList[browser.selIndex].filename);
					}

					if(currentMenu == MENU_BROWSE_PICTURES)
					{
						// load picture viewer
						PictureViewer();
						continue;
					}

					ShutdownMPlayer();

					ShowAction("Loading...");

					// signal MPlayer to load
					LoadMPlayer();

					// wait until MPlayer is ready to take control (or return control)
					while(!guiShutdown && controlledbygui != 1)
						usleep(THREAD_SLEEP);

					CancelAction();

					if(guiShutdown)
					{
						playingAudio = false;
						goto done;
					}
					else
					{
						playingAudio = true;
						ResumeDeviceThread();

						// we loaded an audio file - if we already had a video
						// loaded, we should remove the bg and MPlayer button
						mainWindow->Remove(videoImg);
						//mainWindow->Remove(mplayerBtn);
						
						// add the audio bar
						mainWindow->Append(audiobar);
						nowPlayingSet = false;
					}
				}
			}
			
			// music playlist add buttons - skip first entry (..)
			if(currentMenu != MENU_BROWSE_MUSIC || i == 0)
				continue;

			if(fileBrowser.playlistAddBtn[i]->GetState() == STATE_CLICKED)
			{
				fileBrowser.playlistAddBtn[i]->ResetState();
				char fullpath[MAXPATHLEN+1];
				int addIndex = browser.pageIndex + i;
				sprintf(fullpath, "%s%s", browser.dir, browserList[addIndex].filename);

				if(browserList[addIndex].isdir)
					EnqueueFolder(fullpath);
				else
					EnqueueFile(fullpath, browserList[addIndex].filename);

				char txt[10];
				sprintf(txt, "%d", playlistSize);
				playlistBtnTxt.SetText(txt);
			}
		}
		
		if(playlistBtn.GetState() == STATE_CLICKED)
		{
			playlistBtn.ResetState();
			inPlaylist = ParsePlaylist(); // load the current playlist into the filebrowser
		}
		
		if(playlist && playlistBtn.GetAlpha() == 128)
		{
			playlistBtn.SetAlpha(255);
			playlistBtn.ResetState();
		}
		else if(!playlist && playlistBtn.GetAlpha() == 255)
		{
			playlistBtn.SetAlpha(128);
		}
	}
done:
	HaltParseThread(); // halt parsing
	HaltGui();
	mainWindow->Remove(&fileBrowser);
	
	if(menu == MENU_BROWSE_MUSIC) // remove playlist functionality
		mainWindow->Remove(&playlistBtn);
}

static void MenuDVD()
{
	ResumeGui();

	if(!ChangeInterface(DEVICE_DVD, -1, NOTSILENT))
	{
		ChangeMenu(lastMenu); // go back to last menu
		return;
	}

	sprintf(loadedFile, "dvdnav://");
	ShutdownMPlayer();
	ShowAction("Loading...");
	LoadMPlayer(); // signal MPlayer to load

	// wait until MPlayer is ready to take or return control
	while(!guiShutdown && controlledbygui != 1)
		usleep(THREAD_SLEEP);

	CancelAction();
	ChangeMenu(lastMenu); // go back to last menu
	
	if(!guiShutdown) // load failed
		ErrorPrompt("Invalid DVD!");

	HaltGui();
}

static void MenuSettingsGeneral()
{
	int ret;
	int i = 0;
	bool firstRun = true;
	OptionList options;

	sprintf(options.name[i++], "Auto-Resume");
	sprintf(options.name[i++], "Play Order");
	sprintf(options.name[i++], "Clean Filenames");
	sprintf(options.name[i++], "File Extensions");
	sprintf(options.name[i++], "Unsupported Files");
	sprintf(options.name[i++], "Language");
	sprintf(options.name[i++], "Video Files Folder");
	sprintf(options.name[i++], "Music Files Folder");
	sprintf(options.name[i++], "Exit Action");
	sprintf(options.name[i++], "Wiimote Rumble");

	options.length = i;
		
	for(i=0; i < options.length; i++)
	{
		options.value[i][0] = 0;
		options.icon[i] = 0;
	}

	GuiText titleTxt("Settings - General", 26, (GXColor){255, 255, 255, 255});
	titleTxt.SetAlignment(ALIGN_LEFT, ALIGN_TOP);
	titleTxt.SetPosition(70, 90);

	GuiImageData btnOutline(button_png);
	GuiImageData btnOutlineOver(button_over_png);

	GuiTrigger trigA;
	trigA.SetSimpleTrigger(-1, WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A, PAD_BUTTON_A);

	GuiText backBtnTxt("Go Back", 24, (GXColor){255, 255, 255, 255});
	GuiImage backBtnImg(&btnOutline);
	GuiImage backBtnImgOver(&btnOutlineOver);
	GuiButton backBtn(btnOutline.GetWidth(), btnOutline.GetHeight());
	backBtn.SetAlignment(ALIGN_LEFT, ALIGN_BOTTOM);
	backBtn.SetPosition(30, -35);
	backBtn.SetLabel(&backBtnTxt);
	backBtn.SetImage(&backBtnImg);
	backBtn.SetImageOver(&backBtnImgOver);
	backBtn.SetTrigger(&trigA);
	backBtn.SetEffectGrow();

	GuiOptionBrowser optionBrowser(460, 220, &options);
	optionBrowser.SetPosition(70, 120);
	optionBrowser.SetCol2Position(200);
	optionBrowser.SetAlignment(ALIGN_LEFT, ALIGN_TOP);

	HaltGui();
	GuiWindow w(screenwidth, screenheight);
	w.Append(&backBtn);
	mainWindow->Append(&optionBrowser);
	mainWindow->Append(&w);
	mainWindow->Append(&titleTxt);
	ResumeGui();

	while(currentMenu == MENU_SETTINGS_GENERAL && !guiShutdown)
	{
		usleep(THREAD_SLEEP);

		ret = optionBrowser.GetClickedOption();

		switch (ret)
		{
			case 0:
				WiiSettings.autoResume ^= 1;
				break;
			case 1:
				WiiSettings.playOrder++;
				if(WiiSettings.playOrder > PLAY_LOOP)
					WiiSettings.playOrder = 0;
				break;
			case 2:
				WiiSettings.cleanFilenames ^= 1;
				break;
			case 3:
				WiiSettings.hideExtensions ^= 1;
				break;
			case 4:
				WiiSettings.filterFiles ^= 1;
				break;
			case 5:
				WiiSettings.language++;
				if(WiiSettings.language > LANG_KOREAN)
					WiiSettings.language = 0;
				break;
			case 6:
				OnScreenKeyboard(WiiSettings.videoFolder, MAXPATHLEN);
				break;
			case 7:
				OnScreenKeyboard(WiiSettings.musicFolder, MAXPATHLEN);
				break;
			case 8:
				WiiSettings.exitAction++;
				if(WiiSettings.exitAction > EXIT_LOADER)
					WiiSettings.exitAction = 0;
				break;
			case 9:
				WiiSettings.rumble ^= 1;
				break;
		}

		if(ret >= 0 || firstRun)
		{
			firstRun = false;

			sprintf(options.value[0], "%s", WiiSettings.autoResume ? "On" : "Off");

			switch(WiiSettings.playOrder)
			{
				case PLAY_SINGLE:		sprintf(options.value[1], "Single"); break;
				case PLAY_CONTINUOUS:	sprintf(options.value[1], "Continuous"); break;
				case PLAY_SHUFFLE:		sprintf(options.value[1], "Shuffle"); break;
				case PLAY_LOOP:			sprintf(options.value[1], "Loop"); break;
			}

			sprintf(options.value[2], "%s", WiiSettings.cleanFilenames ? "On" : "Off");
			sprintf(options.value[3], "%s", WiiSettings.hideExtensions ? "Hide" : "Show");
			sprintf(options.value[4], "%s", WiiSettings.filterFiles ? "Hide" : "Show");

			switch(WiiSettings.language)
			{
				case LANG_JAPANESE:		sprintf(options.value[5], "Japanese"); break;
				case LANG_ENGLISH:		sprintf(options.value[5], "English"); break;
				case LANG_GERMAN:		sprintf(options.value[5], "German"); break;
				case LANG_FRENCH:		sprintf(options.value[5], "French"); break;
				case LANG_SPANISH:		sprintf(options.value[5], "Spanish"); break;
				case LANG_ITALIAN:		sprintf(options.value[5], "Italian"); break;
				case LANG_DUTCH:		sprintf(options.value[5], "Dutch"); break;
				case LANG_SIMP_CHINESE:	sprintf(options.value[5], "Chinese (Simplified)"); break;
				case LANG_TRAD_CHINESE:	sprintf(options.value[5], "Chinese (Traditional)"); break;
				case LANG_KOREAN:		sprintf(options.value[5], "Korean"); break;
			}

			snprintf(options.value[6], 20, "%s", WiiSettings.videoFolder);
			snprintf(options.value[7], 20, "%s", WiiSettings.musicFolder);

			switch(WiiSettings.exitAction)
			{
				case EXIT_AUTO:		sprintf(options.value[8], "Auto"); break;
				case EXIT_WIIMENU:	sprintf(options.value[8], "Return to Wii Menu"); break;
				case EXIT_POWEROFF:	sprintf(options.value[8], "Power Off Wii"); break;
				case EXIT_LOADER:	sprintf(options.value[8], "Return to Loader"); break;
			}

			sprintf(options.value[9], "%s", WiiSettings.rumble ? "On" : "Off");

			optionBrowser.TriggerUpdate();
		}

		if(backBtn.GetState() == STATE_CLICKED)
		{
			currentMenu = MENU_SETTINGS;
		}
	}
	HaltGui();
	mainWindow->Remove(&optionBrowser);
	mainWindow->Remove(&w);
	mainWindow->Remove(&titleTxt);
	ResetText();
}

static void MenuSettingsCache()
{
	int ret;
	int i = 0;
	bool firstRun = true;
	OptionList options;

	sprintf(options.name[i++], "Size");
	sprintf(options.name[i++], "Prefill");
	sprintf(options.name[i++], "Refill");

	options.length = i;

	for(i=0; i < options.length; i++)
	{
		options.value[i][0] = 0;
		options.icon[i] = 0;
	}

	GuiText titleTxt("Settings - Cache", 26, (GXColor){255, 255, 255, 255});
	titleTxt.SetAlignment(ALIGN_LEFT, ALIGN_TOP);
	titleTxt.SetPosition(70, 90);

	GuiImageData btnOutline(button_png);
	GuiImageData btnOutlineOver(button_over_png);

	GuiTrigger trigA;
	trigA.SetSimpleTrigger(-1, WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A, PAD_BUTTON_A);

	GuiText backBtnTxt("Go Back", 24, (GXColor){255, 255, 255, 255});
	GuiImage backBtnImg(&btnOutline);
	GuiImage backBtnImgOver(&btnOutlineOver);
	GuiButton backBtn(btnOutline.GetWidth(), btnOutline.GetHeight());
	backBtn.SetAlignment(ALIGN_LEFT, ALIGN_BOTTOM);
	backBtn.SetPosition(30, -35);
	backBtn.SetLabel(&backBtnTxt);
	backBtn.SetImage(&backBtnImg);
	backBtn.SetImageOver(&backBtnImgOver);
	backBtn.SetTrigger(&trigA);
	backBtn.SetEffectGrow();

	GuiOptionBrowser optionBrowser(460, 220, &options);
	optionBrowser.SetPosition(70, 120);
	optionBrowser.SetCol2Position(220);
	optionBrowser.SetAlignment(ALIGN_LEFT, ALIGN_TOP);

	HaltGui();
	GuiWindow w(screenwidth, screenheight);
	w.Append(&backBtn);
	mainWindow->Append(&optionBrowser);
	mainWindow->Append(&w);
	mainWindow->Append(&titleTxt);
	ResumeGui();

	while(currentMenu == MENU_SETTINGS_CACHE && !guiShutdown)
	{
		usleep(THREAD_SLEEP);

		ret = optionBrowser.GetClickedOption();

		switch (ret)
		{
			case 0:
				WiiSettings.cacheSize += 2048;
				if(WiiSettings.cacheSize > 16384)
					WiiSettings.cacheSize = 0;
				break;

			case 1:
				WiiSettings.cacheFillStart += 10;
				if (WiiSettings.cacheFillStart > 100)
					WiiSettings.cacheFillStart = 0;
				break;

			case 2:
				WiiSettings.cacheFillRestart += 10;
				if (WiiSettings.cacheFillRestart > 100)
					WiiSettings.cacheFillRestart = 0;
				break;
		}

		if(ret >= 0 || firstRun)
		{
			firstRun = false;
			sprintf (options.value[0], "%d", WiiSettings.cacheSize);
			sprintf (options.value[1], "%d%%", WiiSettings.cacheFillStart);
			sprintf (options.value[2], "%d%%", WiiSettings.cacheFillRestart);

			optionBrowser.TriggerUpdate();
		}

		if(backBtn.GetState() == STATE_CLICKED)
		{
			currentMenu = MENU_SETTINGS;
		}
	}
	HaltGui();
	mainWindow->Remove(&optionBrowser);
	mainWindow->Remove(&w);
	mainWindow->Remove(&titleTxt);
}

static void MenuSettingsNetwork()
{
	int ret;
	int i = 0;
	OptionList options;
	
	// find all currently set SMB/FTP entries
	
	for(int j=0; j < 5; j++)
	{
		options.name[i][0] = 0;
		options.icon[i] = ICON_SMB;
		options.value[i][0] = 0;

		if(WiiSettings.smbConf[j].share[0] != 0)
		{
			if(WiiSettings.smbConf[j].displayname[0] != 0)
				sprintf(options.name[i], "%s", WiiSettings.smbConf[j].displayname);
			else
				sprintf(options.name[i], "%s", WiiSettings.smbConf[j].share);
		}
		i++;
	}
	for(int j=0; j < 5; j++)
	{
		options.name[i][0] = 0;
		options.icon[i] = ICON_FTP;
		options.value[i][0] = 0;

		if(WiiSettings.ftpConf[j].ip[0] != 0)
		{
			if(WiiSettings.ftpConf[j].displayname[0] != 0)
				sprintf(options.name[i], "%s", WiiSettings.ftpConf[j].displayname);
			else
				sprintf(options.name[i], "%s@%s/%s", WiiSettings.ftpConf[j].user, WiiSettings.ftpConf[j].ip, WiiSettings.ftpConf[j].folder);
		}
		i++;
	}

	options.length = i;

	GuiText titleTxt("Settings - Network", 26, (GXColor){255, 255, 255, 255});
	titleTxt.SetAlignment(ALIGN_LEFT, ALIGN_TOP);
	titleTxt.SetPosition(70, 90);

	GuiImageData btnOutline(button_png);
	GuiImageData btnOutlineOver(button_over_png);
	GuiImageData iconSMB(icon_smb_png);
	GuiImageData iconFTP(icon_ftp_png);

	GuiTrigger trigA;
	trigA.SetSimpleTrigger(-1, WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A, PAD_BUTTON_A);

	GuiText backBtnTxt("Go Back", 24, (GXColor){255, 255, 255, 255});
	GuiImage backBtnImg(&btnOutline);
	GuiImage backBtnImgOver(&btnOutlineOver);
	GuiButton backBtn(btnOutline.GetWidth(), btnOutline.GetHeight());
	backBtn.SetAlignment(ALIGN_LEFT, ALIGN_BOTTOM);
	backBtn.SetPosition(30, -35);
	backBtn.SetLabel(&backBtnTxt);
	backBtn.SetImage(&backBtnImg);
	backBtn.SetImageOver(&backBtnImgOver);
	backBtn.SetTrigger(&trigA);
	backBtn.SetEffectGrow();
	
	GuiText addsmbBtnTxt("Add", 24, (GXColor){255, 255, 255, 255});
	GuiImage addsmbBtnImg(&iconSMB);
	addsmbBtnImg.SetAlignment(ALIGN_LEFT, ALIGN_MIDDLE);
	addsmbBtnTxt.SetAlignment(ALIGN_RIGHT, ALIGN_MIDDLE);
	GuiButton addsmbBtn(75, btnOutline.GetHeight());
	addsmbBtn.SetAlignment(ALIGN_LEFT, ALIGN_BOTTOM);
	addsmbBtn.SetPosition(250, -35);
	addsmbBtn.SetLabel(&addsmbBtnTxt);
	addsmbBtn.SetImage(&addsmbBtnImg);
	addsmbBtn.SetTrigger(&trigA);
	addsmbBtn.SetEffectGrow();
	
	GuiText addftpBtnTxt("Add", 24, (GXColor){255, 255, 255, 255});
	GuiImage addftpBtnImg(&iconFTP);
	addftpBtnImg.SetAlignment(ALIGN_LEFT, ALIGN_MIDDLE);
	addftpBtnTxt.SetAlignment(ALIGN_RIGHT, ALIGN_MIDDLE);
	GuiButton addftpBtn(75, btnOutline.GetHeight());
	addftpBtn.SetAlignment(ALIGN_LEFT, ALIGN_BOTTOM);
	addftpBtn.SetPosition(335, -35);
	addftpBtn.SetLabel(&addftpBtnTxt);
	addftpBtn.SetImage(&addftpBtnImg);
	addftpBtn.SetTrigger(&trigA);
	addftpBtn.SetEffectGrow();

	GuiOptionBrowser optionBrowser(460, 220, &options);
	optionBrowser.SetPosition(70, 120);
	optionBrowser.SetCol1Position(30);
	optionBrowser.SetAlignment(ALIGN_LEFT, ALIGN_TOP);

	HaltGui();
	GuiWindow w(screenwidth, screenheight);
	w.Append(&backBtn);
	w.Append(&addsmbBtn);
	w.Append(&addftpBtn);
	mainWindow->Append(&optionBrowser);
	mainWindow->Append(&w);
	mainWindow->Append(&titleTxt);
	ResumeGui();

	while(currentMenu == MENU_SETTINGS_NETWORK && !guiShutdown)
	{
		usleep(THREAD_SLEEP);

		ret = optionBrowser.GetClickedOption();

		if((ret >= 0 && ret < 5) || addsmbBtn.GetState() == STATE_CLICKED)
		{
			netEditIndex = ret;
			currentMenu = MENU_SETTINGS_NETWORK_SMB;
		}
		else if(ret >= 5 || addftpBtn.GetState() == STATE_CLICKED)
		{
			netEditIndex = ret-5;
			currentMenu = MENU_SETTINGS_NETWORK_FTP;
		}

		if(backBtn.GetState() == STATE_CLICKED)
		{
			currentMenu = MENU_SETTINGS;
		}
	}
	HaltGui();
	mainWindow->Remove(&optionBrowser);
	mainWindow->Remove(&w);
	mainWindow->Remove(&titleTxt);
}

static void MenuSettingsNetworkSMB()
{
	int ret;
	int i = 0;
	bool firstRun = true;
	OptionList options;
	char titleStr[100];
	char shareName[100];

	sprintf(options.name[i++], "Display Name");
	sprintf(options.name[i++], "Share IP");
	sprintf(options.name[i++], "Share Name");
	sprintf(options.name[i++], "Username");
	sprintf(options.name[i++], "Password");

	options.length = i;
	
	for(i=0; i < options.length; i++)
	{
		options.value[i][0] = 0;
		options.icon[i] = 0;
	}
	
	if(netEditIndex < 0)
		sprintf(shareName, "New Share");
	else if(WiiSettings.smbConf[netEditIndex].displayname[0] != 0)
		sprintf(shareName, "%s", WiiSettings.smbConf[netEditIndex].displayname);
	else
		sprintf(shareName, "%s", WiiSettings.smbConf[netEditIndex].share);

	sprintf(titleStr, "Settings - Network - %s", shareName);

	GuiText titleTxt(titleStr, 26, (GXColor){255, 255, 255, 255});
	titleTxt.SetAlignment(ALIGN_LEFT, ALIGN_TOP);
	titleTxt.SetPosition(70, 90);

	GuiImageData btnOutline(button_png);
	GuiImageData btnOutlineOver(button_over_png);

	GuiTrigger trigA;
	trigA.SetSimpleTrigger(-1, WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A, PAD_BUTTON_A);

	GuiText backBtnTxt("Go Back", 24, (GXColor){255, 255, 255, 255});
	GuiImage backBtnImg(&btnOutline);
	GuiImage backBtnImgOver(&btnOutlineOver);
	GuiButton backBtn(btnOutline.GetWidth(), btnOutline.GetHeight());
	backBtn.SetAlignment(ALIGN_LEFT, ALIGN_BOTTOM);
	backBtn.SetPosition(30, -35);
	backBtn.SetLabel(&backBtnTxt);
	backBtn.SetImage(&backBtnImg);
	backBtn.SetImageOver(&backBtnImgOver);
	backBtn.SetTrigger(&trigA);
	backBtn.SetEffectGrow();
	
	GuiText deleteBtnTxt("Delete", 24, (GXColor){255, 255, 255, 255});
	GuiImage deleteBtnImg(&btnOutline);
	GuiImage deleteBtnImgOver(&btnOutlineOver);
	GuiButton deleteBtn(btnOutline.GetWidth(), btnOutline.GetHeight());
	deleteBtn.SetAlignment(ALIGN_LEFT, ALIGN_BOTTOM);
	deleteBtn.SetPosition(245, -35);
	deleteBtn.SetLabel(&deleteBtnTxt);
	deleteBtn.SetImage(&deleteBtnImg);
	deleteBtn.SetImageOver(&deleteBtnImgOver);
	deleteBtn.SetTrigger(&trigA);
	deleteBtn.SetEffectGrow();

	GuiOptionBrowser optionBrowser(460, 220, &options);
	optionBrowser.SetPosition(70, 120);
	optionBrowser.SetCol2Position(220);
	optionBrowser.SetAlignment(ALIGN_LEFT, ALIGN_TOP);

	HaltGui();
	GuiWindow w(screenwidth, screenheight);
	w.Append(&backBtn);
	
	if(netEditIndex < 0)
	{
		// find a share to put the data into
		for(i=0; i < 5; i++)
		{
			if(WiiSettings.smbConf[i].share[0] == 0)
			{
				netEditIndex = i;
				break;
			}
		}
	}
	else
	{
		w.Append(&deleteBtn);
	}
	
	mainWindow->Append(&optionBrowser);
	mainWindow->Append(&w);
	mainWindow->Append(&titleTxt);
	ResumeGui();

	while(currentMenu == MENU_SETTINGS_NETWORK_SMB && !guiShutdown)
	{
		usleep(THREAD_SLEEP);

		ret = optionBrowser.GetClickedOption();

		switch (ret)
		{
			case 0:
				OnScreenKeyboard(WiiSettings.smbConf[netEditIndex].displayname, 80);
				break;
			
			case 1:
				OnScreenKeyboard(WiiSettings.smbConf[netEditIndex].ip, 80);
				break;

			case 2:
				OnScreenKeyboard(WiiSettings.smbConf[netEditIndex].share, 80);
				break;

			case 3:
				OnScreenKeyboard(WiiSettings.smbConf[netEditIndex].user, 20);
				break;

			case 4:
				OnScreenKeyboard(WiiSettings.smbConf[netEditIndex].pwd, 14);
				break;
		}

		if(ret >= 0 || firstRun)
		{
			firstRun = false;
			strncpy (options.value[0], WiiSettings.smbConf[netEditIndex].displayname, 80);
			strncpy (options.value[1], WiiSettings.smbConf[netEditIndex].ip, 80);
			strncpy (options.value[2], WiiSettings.smbConf[netEditIndex].share, 80);
			strncpy (options.value[3], WiiSettings.smbConf[netEditIndex].user, 20);
			strncpy (options.value[4], WiiSettings.smbConf[netEditIndex].pwd, 14);
			optionBrowser.TriggerUpdate();
		}

		if(backBtn.GetState() == STATE_CLICKED)
		{
			currentMenu = MENU_SETTINGS_NETWORK;
		}
		if(deleteBtn.GetState() == STATE_CLICKED)
		{
			deleteBtn.ResetState();
			if (WindowPrompt("Delete Share", "Are you sure that you want to delete this share?", "OK", "Cancel"))
			{
				WiiSettings.smbConf[netEditIndex].displayname[0] = 0;
				WiiSettings.smbConf[netEditIndex].ip[0] = 0;
				WiiSettings.smbConf[netEditIndex].share[0] = 0;
				WiiSettings.smbConf[netEditIndex].user[0] = 0;
				WiiSettings.smbConf[netEditIndex].pwd[0] = 0;
				currentMenu = MENU_SETTINGS_NETWORK;
			}
		}
	}
	HaltGui();
	mainWindow->Remove(&optionBrowser);
	mainWindow->Remove(&w);
	mainWindow->Remove(&titleTxt);
}

static void MenuSettingsNetworkFTP()
{
	int ret;
	int i = 0;
	bool firstRun = true;
	OptionList options;
	char titleStr[100];
	char siteName[100];

	sprintf(options.name[i++], "Display Name");
	sprintf(options.name[i++], "IP");
	sprintf(options.name[i++], "Folder");
	sprintf(options.name[i++], "Username");
	sprintf(options.name[i++], "Password");
	sprintf(options.name[i++], "Mode");

	options.length = i;
		
	for(i=0; i < options.length; i++)
	{
		options.value[i][0] = 0;
		options.icon[i] = 0;
	}
	
	if(netEditIndex < 0)
		sprintf(siteName, "New Site");
	else if(WiiSettings.ftpConf[netEditIndex].displayname[0] != 0)
		sprintf(siteName, "%s", WiiSettings.ftpConf[netEditIndex].displayname);
	else
		sprintf(options.name[i], "%s@%s/%s", 
		WiiSettings.ftpConf[netEditIndex].user, 
		WiiSettings.ftpConf[netEditIndex].ip, 
		WiiSettings.ftpConf[netEditIndex].folder);

	sprintf(titleStr, "Settings - Network - %s", siteName);

	GuiText titleTxt(titleStr, 26, (GXColor){255, 255, 255, 255});
	titleTxt.SetAlignment(ALIGN_LEFT, ALIGN_TOP);
	titleTxt.SetPosition(70, 90);

	GuiImageData btnOutline(button_png);
	GuiImageData btnOutlineOver(button_over_png);

	GuiTrigger trigA;
	trigA.SetSimpleTrigger(-1, WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A, PAD_BUTTON_A);

	GuiText backBtnTxt("Go Back", 24, (GXColor){255, 255, 255, 255});
	GuiImage backBtnImg(&btnOutline);
	GuiImage backBtnImgOver(&btnOutlineOver);
	GuiButton backBtn(btnOutline.GetWidth(), btnOutline.GetHeight());
	backBtn.SetAlignment(ALIGN_LEFT, ALIGN_BOTTOM);
	backBtn.SetPosition(30, -35);
	backBtn.SetLabel(&backBtnTxt);
	backBtn.SetImage(&backBtnImg);
	backBtn.SetImageOver(&backBtnImgOver);
	backBtn.SetTrigger(&trigA);
	backBtn.SetEffectGrow();
	
	GuiText deleteBtnTxt("Delete", 24, (GXColor){255, 255, 255, 255});
	GuiImage deleteBtnImg(&btnOutline);
	GuiImage deleteBtnImgOver(&btnOutlineOver);
	GuiButton deleteBtn(btnOutline.GetWidth(), btnOutline.GetHeight());
	deleteBtn.SetAlignment(ALIGN_LEFT, ALIGN_BOTTOM);
	deleteBtn.SetPosition(245, -35);
	deleteBtn.SetLabel(&deleteBtnTxt);
	deleteBtn.SetImage(&deleteBtnImg);
	deleteBtn.SetImageOver(&deleteBtnImgOver);
	deleteBtn.SetTrigger(&trigA);
	deleteBtn.SetEffectGrow();

	GuiOptionBrowser optionBrowser(460, 220, &options);
	optionBrowser.SetPosition(70, 120);
	optionBrowser.SetCol2Position(220);
	optionBrowser.SetAlignment(ALIGN_LEFT, ALIGN_TOP);

	HaltGui();
	GuiWindow w(screenwidth, screenheight);
	w.Append(&backBtn);
	
	if(netEditIndex < 0)
	{
		// find a site to put the data into
		for(i=0; i < 5; i++)
		{
			if(WiiSettings.ftpConf[i].ip[0] == 0)
			{
				netEditIndex = i;
				break;
			}
		}
	}
	else
	{
		w.Append(&deleteBtn);
	}
	mainWindow->Append(&optionBrowser);
	mainWindow->Append(&w);
	mainWindow->Append(&titleTxt);
	ResumeGui();

	while(currentMenu == MENU_SETTINGS_NETWORK_FTP && !guiShutdown)
	{
		usleep(THREAD_SLEEP);

		ret = optionBrowser.GetClickedOption();

		switch (ret)
		{
			case 0:
				OnScreenKeyboard(WiiSettings.ftpConf[netEditIndex].displayname, 80);
				break;
			
			case 1:
				OnScreenKeyboard(WiiSettings.ftpConf[netEditIndex].ip, 80);
				break;

			case 2:
				OnScreenKeyboard(WiiSettings.ftpConf[netEditIndex].folder, 80);
				break;

			case 3:
				OnScreenKeyboard(WiiSettings.ftpConf[netEditIndex].user, 20);
				break;

			case 4:
				OnScreenKeyboard(WiiSettings.ftpConf[netEditIndex].pwd, 14);
				break;
				
			case 5:
				WiiSettings.ftpConf[netEditIndex].passive ^= 1;
				break;
		}

		if(ret >= 0 || firstRun)
		{
			firstRun = false;
			strncpy (options.value[0], WiiSettings.ftpConf[netEditIndex].displayname, 80);
			strncpy (options.value[1], WiiSettings.ftpConf[netEditIndex].ip, 80);
			strncpy (options.value[2], WiiSettings.ftpConf[netEditIndex].folder, 80);
			strncpy (options.value[3], WiiSettings.ftpConf[netEditIndex].user, 20);
			strncpy (options.value[4], WiiSettings.ftpConf[netEditIndex].pwd, 14);
			sprintf(options.value[5], "%s", WiiSettings.ftpConf[netEditIndex].passive ? "Passive" : "Active");
			optionBrowser.TriggerUpdate();
		}

		if(backBtn.GetState() == STATE_CLICKED)
		{
			currentMenu = MENU_SETTINGS_NETWORK;
		}
		if(deleteBtn.GetState() == STATE_CLICKED)
		{
			deleteBtn.ResetState();
			if (WindowPrompt("Delete Site", "Are you sure that you want to delete this site?", "OK", "Cancel"))
			{
				WiiSettings.ftpConf[netEditIndex].displayname[0] = 0;
				WiiSettings.ftpConf[netEditIndex].ip[0] = 0;
				WiiSettings.ftpConf[netEditIndex].folder[0] = 0;
				WiiSettings.ftpConf[netEditIndex].user[0] = 0;
				WiiSettings.ftpConf[netEditIndex].pwd[0] = 0;
				WiiSettings.ftpConf[netEditIndex].passive = 0;
				currentMenu = MENU_SETTINGS_NETWORK;
			}
		}
	}
	HaltGui();
	mainWindow->Remove(&optionBrowser);
	mainWindow->Remove(&w);
	mainWindow->Remove(&titleTxt);
}

static void ScreenZoomWindowUpdate(void * ptr, float amount)
{
	GuiButton * b = (GuiButton *)ptr;
	if(b->GetState() == STATE_CLICKED)
	{
		WiiSettings.videoZoom += amount;

		char zoom[10];
		sprintf(zoom, "%.2f%%", WiiSettings.videoZoom*100);
		settingText->SetText(zoom);
		b->ResetState();
	}
}

static void ScreenZoomWindowLeftClick(void * ptr) { ScreenZoomWindowUpdate(ptr, -0.01); }
static void ScreenZoomWindowRightClick(void * ptr) { ScreenZoomWindowUpdate(ptr, +0.01); }

static void ScreenZoomWindow()
{
	GuiWindow * w = new GuiWindow(250,250);
	w->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);

	GuiTrigger trigA;
	trigA.SetSimpleTrigger(-1, WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A, PAD_BUTTON_A);

	GuiTrigger trigLeft;
	trigLeft.SetButtonOnlyInFocusTrigger(-1, WPAD_BUTTON_LEFT | WPAD_CLASSIC_BUTTON_LEFT, PAD_BUTTON_LEFT);

	GuiTrigger trigRight;
	trigRight.SetButtonOnlyInFocusTrigger(-1, WPAD_BUTTON_RIGHT | WPAD_CLASSIC_BUTTON_RIGHT, PAD_BUTTON_RIGHT);

	GuiImageData arrowLeft(button_arrow_left_png);
	GuiImage arrowLeftImg(&arrowLeft);
	GuiImageData arrowLeftOver(button_arrow_left_over_png);
	GuiImage arrowLeftOverImg(&arrowLeftOver);
	GuiButton arrowLeftBtn(arrowLeft.GetWidth(), arrowLeft.GetHeight());
	arrowLeftBtn.SetImage(&arrowLeftImg);
	arrowLeftBtn.SetImageOver(&arrowLeftOverImg);
	arrowLeftBtn.SetAlignment(ALIGN_LEFT, ALIGN_MIDDLE);
	arrowLeftBtn.SetTrigger(0, &trigA);
	arrowLeftBtn.SetTrigger(1, &trigLeft);
	arrowLeftBtn.SetSelectable(false);
	arrowLeftBtn.SetUpdateCallback(ScreenZoomWindowLeftClick);

	GuiImageData arrowRight(button_arrow_right_png);
	GuiImage arrowRightImg(&arrowRight);
	GuiImageData arrowRightOver(button_arrow_right_over_png);
	GuiImage arrowRightOverImg(&arrowRightOver);
	GuiButton arrowRightBtn(arrowRight.GetWidth(), arrowRight.GetHeight());
	arrowRightBtn.SetImage(&arrowRightImg);
	arrowRightBtn.SetImageOver(&arrowRightOverImg);
	arrowRightBtn.SetAlignment(ALIGN_RIGHT, ALIGN_MIDDLE);
	arrowRightBtn.SetTrigger(0, &trigA);
	arrowRightBtn.SetTrigger(1, &trigRight);
	arrowRightBtn.SetSelectable(false);
	arrowRightBtn.SetUpdateCallback(ScreenZoomWindowRightClick);

	settingText = new GuiText(NULL, 22, (GXColor){0, 0, 0, 255});
	char zoom[10];
	sprintf(zoom, "%.2f%%", WiiSettings.videoZoom*100);
	settingText->SetText(zoom);

	float currentZoom = WiiSettings.videoZoom;

	w->Append(&arrowLeftBtn);
	w->Append(&arrowRightBtn);
	w->Append(settingText);

	if(!SettingWindow("Screen Zoom",w))
		WiiSettings.videoZoom = currentZoom; // undo changes

	delete(w);
	delete(settingText);
}

static void ScreenPositionWindowUpdate(void * ptr, int x, int y)
{
	GuiButton * b = (GuiButton *)ptr;
	if(b->GetState() == STATE_CLICKED)
	{
		WiiSettings.videoXshift += x;
		WiiSettings.videoYshift += y;

		char shift[10];
		sprintf(shift, "%i, %i", WiiSettings.videoXshift, WiiSettings.videoYshift);
		settingText->SetText(shift);
		b->ResetState();
	}
}

static void ScreenPositionWindowLeftClick(void * ptr) { ScreenPositionWindowUpdate(ptr, -1, 0); }
static void ScreenPositionWindowRightClick(void * ptr) { ScreenPositionWindowUpdate(ptr, +1, 0); }
static void ScreenPositionWindowUpClick(void * ptr) { ScreenPositionWindowUpdate(ptr, 0, -1); }
static void ScreenPositionWindowDownClick(void * ptr) { ScreenPositionWindowUpdate(ptr, 0, +1); }

static void ScreenPositionWindow()
{
	GuiWindow * w = new GuiWindow(150,150);
	w->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	w->SetPosition(0, -10);

	GuiTrigger trigA;
	trigA.SetSimpleTrigger(-1, WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A, PAD_BUTTON_A);

	GuiTrigger trigLeft;
	trigLeft.SetButtonOnlyInFocusTrigger(-1, WPAD_BUTTON_LEFT | WPAD_CLASSIC_BUTTON_LEFT, PAD_BUTTON_LEFT);

	GuiTrigger trigRight;
	trigRight.SetButtonOnlyInFocusTrigger(-1, WPAD_BUTTON_RIGHT | WPAD_CLASSIC_BUTTON_RIGHT, PAD_BUTTON_RIGHT);

	GuiTrigger trigUp;
	trigUp.SetButtonOnlyInFocusTrigger(-1, WPAD_BUTTON_UP | WPAD_CLASSIC_BUTTON_UP, PAD_BUTTON_UP);

	GuiTrigger trigDown;
	trigDown.SetButtonOnlyInFocusTrigger(-1, WPAD_BUTTON_DOWN | WPAD_CLASSIC_BUTTON_DOWN, PAD_BUTTON_DOWN);

	GuiImageData arrowLeft(button_arrow_left_png);
	GuiImage arrowLeftImg(&arrowLeft);
	GuiImageData arrowLeftOver(button_arrow_left_over_png);
	GuiImage arrowLeftOverImg(&arrowLeftOver);
	GuiButton arrowLeftBtn(arrowLeft.GetWidth(), arrowLeft.GetHeight());
	arrowLeftBtn.SetImage(&arrowLeftImg);
	arrowLeftBtn.SetImageOver(&arrowLeftOverImg);
	arrowLeftBtn.SetAlignment(ALIGN_LEFT, ALIGN_MIDDLE);
	arrowLeftBtn.SetTrigger(0, &trigA);
	arrowLeftBtn.SetTrigger(1, &trigLeft);
	arrowLeftBtn.SetSelectable(false);
	arrowLeftBtn.SetUpdateCallback(ScreenPositionWindowLeftClick);

	GuiImageData arrowRight(button_arrow_right_png);
	GuiImage arrowRightImg(&arrowRight);
	GuiImageData arrowRightOver(button_arrow_right_over_png);
	GuiImage arrowRightOverImg(&arrowRightOver);
	GuiButton arrowRightBtn(arrowRight.GetWidth(), arrowRight.GetHeight());
	arrowRightBtn.SetImage(&arrowRightImg);
	arrowRightBtn.SetImageOver(&arrowRightOverImg);
	arrowRightBtn.SetAlignment(ALIGN_RIGHT, ALIGN_MIDDLE);
	arrowRightBtn.SetTrigger(0, &trigA);
	arrowRightBtn.SetTrigger(1, &trigRight);
	arrowRightBtn.SetSelectable(false);
	arrowRightBtn.SetUpdateCallback(ScreenPositionWindowRightClick);

	GuiImageData arrowUp(button_arrow_up_png);
	GuiImage arrowUpImg(&arrowUp);
	GuiImageData arrowUpOver(button_arrow_up_over_png);
	GuiImage arrowUpOverImg(&arrowUpOver);
	GuiButton arrowUpBtn(arrowUp.GetWidth(), arrowUp.GetHeight());
	arrowUpBtn.SetImage(&arrowUpImg);
	arrowUpBtn.SetImageOver(&arrowUpOverImg);
	arrowUpBtn.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	arrowUpBtn.SetTrigger(0, &trigA);
	arrowUpBtn.SetTrigger(1, &trigUp);
	arrowUpBtn.SetSelectable(false);
	arrowUpBtn.SetUpdateCallback(ScreenPositionWindowUpClick);

	GuiImageData arrowDown(button_arrow_down_png);
	GuiImage arrowDownImg(&arrowDown);
	GuiImageData arrowDownOver(button_arrow_down_over_png);
	GuiImage arrowDownOverImg(&arrowDownOver);
	GuiButton arrowDownBtn(arrowDown.GetWidth(), arrowDown.GetHeight());
	arrowDownBtn.SetImage(&arrowDownImg);
	arrowDownBtn.SetImageOver(&arrowDownOverImg);
	arrowDownBtn.SetAlignment(ALIGN_CENTRE, ALIGN_BOTTOM);
	arrowDownBtn.SetTrigger(0, &trigA);
	arrowDownBtn.SetTrigger(1, &trigDown);
	arrowDownBtn.SetSelectable(false);
	arrowDownBtn.SetUpdateCallback(ScreenPositionWindowDownClick);

	GuiImageData screenPosition(screen_position_png);
	GuiImage screenPositionImg(&screenPosition);
	screenPositionImg.SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);

	settingText = new GuiText(NULL, 22, (GXColor){0, 0, 0, 255});
	char shift[10];
	sprintf(shift, "%i, %i", WiiSettings.videoXshift, WiiSettings.videoYshift);
	settingText->SetText(shift);

	int currentX = WiiSettings.videoXshift;
	int currentY = WiiSettings.videoYshift;

	w->Append(&arrowLeftBtn);
	w->Append(&arrowRightBtn);
	w->Append(&arrowUpBtn);
	w->Append(&arrowDownBtn);
	w->Append(&screenPositionImg);
	w->Append(settingText);

	if(!SettingWindow("Screen Position",w))
	{
		WiiSettings.videoXshift = currentX; // undo changes
		WiiSettings.videoYshift = currentY;
	}

	delete(w);
	delete(settingText);
}

static void MenuSettingsVideo()
{
	int ret;
	int i = 0;
	bool firstRun = true;
	OptionList options;

	sprintf(options.name[i++], "Frame Dropping");
	sprintf(options.name[i++], "Aspect Ratio");
	sprintf(options.name[i++], "Zoom");
	sprintf(options.name[i++], "Position");

	options.length = i;
		
	for(i=0; i < options.length; i++)
	{
		options.value[i][0] = 0;
		options.icon[i] = 0;
	}

	GuiText titleTxt("Settings - Video", 26, (GXColor){255, 255, 255, 255});
	titleTxt.SetAlignment(ALIGN_LEFT, ALIGN_TOP);
	titleTxt.SetPosition(70, 90);

	GuiImageData btnOutline(button_png);
	GuiImageData btnOutlineOver(button_over_png);

	GuiTrigger trigA;
	trigA.SetSimpleTrigger(-1, WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A, PAD_BUTTON_A);

	GuiText backBtnTxt("Go Back", 24, (GXColor){255, 255, 255, 255});
	GuiImage backBtnImg(&btnOutline);
	GuiImage backBtnImgOver(&btnOutlineOver);
	GuiButton backBtn(btnOutline.GetWidth(), btnOutline.GetHeight());
	backBtn.SetAlignment(ALIGN_LEFT, ALIGN_BOTTOM);
	backBtn.SetPosition(30, -35);
	backBtn.SetLabel(&backBtnTxt);
	backBtn.SetImage(&backBtnImg);
	backBtn.SetImageOver(&backBtnImgOver);
	backBtn.SetTrigger(&trigA);
	backBtn.SetEffectGrow();

	GuiOptionBrowser optionBrowser(460, 220, &options);
	optionBrowser.SetPosition(70, 120);
	optionBrowser.SetCol2Position(220);
	optionBrowser.SetAlignment(ALIGN_LEFT, ALIGN_TOP);

	HaltGui();
	GuiWindow w(screenwidth, screenheight);
	w.Append(&backBtn);
	mainWindow->Append(&optionBrowser);
	mainWindow->Append(&w);
	mainWindow->Append(&titleTxt);
	ResumeGui();

	while(currentMenu == MENU_SETTINGS_VIDEO && !guiShutdown)
	{
		usleep(THREAD_SLEEP);

		ret = optionBrowser.GetClickedOption();

		switch (ret)
		{
			case 0:
				WiiSettings.frameDropping++;
				if (WiiSettings.frameDropping > 2)
					WiiSettings.frameDropping = 0;
				break;

			case 1:
				WiiSettings.aspectRatio++;
				if (WiiSettings.aspectRatio > 3)
					WiiSettings.aspectRatio = 0;
				break;

			case 2:
				ScreenZoomWindow();
				break;

			case 3:
				ScreenPositionWindow();
				break;
		}

		if(ret >= 0 || firstRun)
		{
			firstRun = false;
			switch(WiiSettings.frameDropping)
			{
				case FRAMEDROPPING_AUTO:
					sprintf (options.value[0], "Auto"); break;
				case FRAMEDROPPING_ALWAYS:
					sprintf (options.value[0], "Always"); break;
				case FRAMEDROPPING_DISABLED:
					sprintf (options.value[0], "Disabled"); break;
			}

			switch(WiiSettings.aspectRatio)
			{
				case ASPECT_AUTO:
					sprintf (options.value[1], "Auto"); break;
				case ASPECT_16_9:
					sprintf (options.value[1], "16:9"); break;
				case ASPECT_4_3:
					sprintf (options.value[1], "4:3"); break;
				case ASPECT_235_1:
					sprintf (options.value[1], "2.35:1"); break;
			}

			sprintf (options.value[2], "%.2f%%", WiiSettings.videoZoom*100);
			sprintf (options.value[3], "%d, %d", WiiSettings.videoXshift, WiiSettings.videoYshift);

			optionBrowser.TriggerUpdate();
		}

		if(backBtn.GetState() == STATE_CLICKED)
		{
			currentMenu = MENU_SETTINGS;
		}
	}
	HaltGui();
	mainWindow->Remove(&optionBrowser);
	mainWindow->Remove(&w);
	mainWindow->Remove(&titleTxt);
}

static void MenuSettingsAudio()
{
	int ret;
	int i = 0;
	bool firstRun = true;
	OptionList options;

	sprintf(options.name[i++], "Volume");
	sprintf(options.name[i++], "Delay (ms)");

	options.length = i;
		
	for(i=0; i < options.length; i++)
	{
		options.value[i][0] = 0;
		options.icon[i] = 0;
	}

	GuiText titleTxt("Settings - Audio", 26, (GXColor){255, 255, 255, 255});
	titleTxt.SetAlignment(ALIGN_LEFT, ALIGN_TOP);
	titleTxt.SetPosition(70, 90);

	GuiImageData btnOutline(button_png);
	GuiImageData btnOutlineOver(button_over_png);

	GuiTrigger trigA;
	trigA.SetSimpleTrigger(-1, WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A, PAD_BUTTON_A);

	GuiText backBtnTxt("Go Back", 24, (GXColor){255, 255, 255, 255});
	GuiImage backBtnImg(&btnOutline);
	GuiImage backBtnImgOver(&btnOutlineOver);
	GuiButton backBtn(btnOutline.GetWidth(), btnOutline.GetHeight());
	backBtn.SetAlignment(ALIGN_LEFT, ALIGN_BOTTOM);
	backBtn.SetPosition(30, -35);
	backBtn.SetLabel(&backBtnTxt);
	backBtn.SetImage(&backBtnImg);
	backBtn.SetImageOver(&backBtnImgOver);
	backBtn.SetTrigger(&trigA);
	backBtn.SetEffectGrow();

	GuiOptionBrowser optionBrowser(460, 220, &options);
	optionBrowser.SetPosition(70, 120);
	optionBrowser.SetCol2Position(220);
	optionBrowser.SetAlignment(ALIGN_LEFT, ALIGN_TOP);

	HaltGui();
	GuiWindow w(screenwidth, screenheight);
	w.Append(&backBtn);
	mainWindow->Append(&optionBrowser);
	mainWindow->Append(&w);
	mainWindow->Append(&titleTxt);
	ResumeGui();

	while(currentMenu == MENU_SETTINGS_AUDIO && !guiShutdown)
	{
		usleep(THREAD_SLEEP);

		ret = optionBrowser.GetClickedOption();

		switch (ret)
		{
			case 0:
				WiiSettings.volume += 10;
				if(WiiSettings.volume > 100)
					WiiSettings.volume = 0;
				break;

			case 1:
				WiiSettings.audioDelay += 100;
				if (WiiSettings.audioDelay > 1000)
					WiiSettings.audioDelay = 0;
				break;
		}

		if(ret >= 0 || firstRun)
		{
			firstRun = false;
			sprintf (options.value[0], "%d%%", WiiSettings.volume);
			sprintf (options.value[1], "%d ms", WiiSettings.audioDelay);

			optionBrowser.TriggerUpdate();
		}

		if(backBtn.GetState() == STATE_CLICKED)
		{
			currentMenu = MENU_SETTINGS;
		}
	}
	HaltGui();
	mainWindow->Remove(&optionBrowser);
	mainWindow->Remove(&w);
	mainWindow->Remove(&titleTxt);
}

static void MenuSettingsSubtitles()
{
	int ret;
	int i = 0;
	bool firstRun = true;
	OptionList options;

	sprintf(options.name[i++], "Delay");
	sprintf(options.name[i++], "Position");
	sprintf(options.name[i++], "Size");
	sprintf(options.name[i++], "Transparency");
	sprintf(options.name[i++], "Color");

	options.length = i;
		
	for(i=0; i < options.length; i++)
	{
		options.value[i][0] = 0;
		options.icon[i] = 0;
	}

	GuiText titleTxt("Settings - Subtitles", 26, (GXColor){255, 255, 255, 255});
	titleTxt.SetAlignment(ALIGN_LEFT, ALIGN_TOP);
	titleTxt.SetPosition(70, 90);

	GuiImageData btnOutline(button_png);
	GuiImageData btnOutlineOver(button_over_png);

	GuiTrigger trigA;
	trigA.SetSimpleTrigger(-1, WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A, PAD_BUTTON_A);

	GuiText backBtnTxt("Go Back", 24, (GXColor){255, 255, 255, 255});
	GuiImage backBtnImg(&btnOutline);
	GuiImage backBtnImgOver(&btnOutlineOver);
	GuiButton backBtn(btnOutline.GetWidth(), btnOutline.GetHeight());
	backBtn.SetAlignment(ALIGN_LEFT, ALIGN_BOTTOM);
	backBtn.SetPosition(30, -35);
	backBtn.SetLabel(&backBtnTxt);
	backBtn.SetImage(&backBtnImg);
	backBtn.SetImageOver(&backBtnImgOver);
	backBtn.SetTrigger(&trigA);
	backBtn.SetEffectGrow();

	GuiOptionBrowser optionBrowser(460, 220, &options);
	optionBrowser.SetPosition(70, 120);
	optionBrowser.SetCol2Position(220);
	optionBrowser.SetAlignment(ALIGN_LEFT, ALIGN_TOP);

	HaltGui();
	GuiWindow w(screenwidth, screenheight);
	w.Append(&backBtn);
	mainWindow->Append(&optionBrowser);
	mainWindow->Append(&w);
	mainWindow->Append(&titleTxt);
	ResumeGui();

	while(currentMenu == MENU_SETTINGS_SUBTITLES && !guiShutdown)
	{
		usleep(THREAD_SLEEP);

		ret = optionBrowser.GetClickedOption();

		switch (ret)
		{
			case 0:
				WiiSettings.subtitleDelay += 100;
				if (WiiSettings.subtitleDelay > 1000)
					WiiSettings.subtitleDelay = 0;
				break;

			case 1:
				WiiSettings.subtitlePosition += 10;
				if (WiiSettings.subtitlePosition > 100)
					WiiSettings.subtitlePosition = 100;
				break;

			case 2:
				WiiSettings.subtitleSize += 4;
				if (WiiSettings.subtitleSize > 80)
					WiiSettings.subtitleSize = 10;
				break;

			case 3:
				WiiSettings.subtitleAlpha += 20;
				if(WiiSettings.subtitleAlpha > 255)
					WiiSettings.subtitleAlpha = 0;
				break;

			case 4:
				WiiSettings.subtitleColor++;
				if(WiiSettings.subtitleColor > FONTCOLOR_GRAY)
					WiiSettings.subtitleColor = 0;
				break;
		}

		if(ret >= 0 || firstRun)
		{
			firstRun = false;
			sprintf(options.value[0], "%d ms", WiiSettings.subtitleDelay);
			sprintf(options.value[1], "%d", WiiSettings.subtitlePosition);
			sprintf(options.value[2], "%d", WiiSettings.subtitleSize);
			sprintf(options.value[3], "%.2f%%", WiiSettings.subtitleAlpha/255.0*100);
			
			switch(WiiSettings.subtitleColor)
			{
				case FONTCOLOR_WHITE:
					sprintf(options.value[4], "White"); break;
				case FONTCOLOR_BLACK:
					sprintf(options.value[4], "Black"); break;
				case FONTCOLOR_GRAY:
					sprintf(options.value[4], "Gray"); break;
			}

			optionBrowser.TriggerUpdate();
		}

		if(backBtn.GetState() == STATE_CLICKED)
		{
			currentMenu = MENU_SETTINGS;
		}
	}
	HaltGui();
	mainWindow->Remove(&optionBrowser);
	mainWindow->Remove(&w);
	mainWindow->Remove(&titleTxt);
}

/****************************************************************************
 * MenuSettings
 ***************************************************************************/
static void MenuSettings()
{
	int ret;
	int i = 0;
	int selected = -1;

	MenuItemList items;
	sprintf(items.name[i], "General");
	items.img[i] = NULL; i++;
	sprintf(items.name[i], "Cache");
	items.img[i] = NULL; i++;
	sprintf(items.name[i], "Network");
	items.img[i] = NULL; i++;
	sprintf(items.name[i], "Video");
	items.img[i] = NULL; i++;
	sprintf(items.name[i], "Audio");
	items.img[i] = NULL; i++;
	sprintf(items.name[i], "Subtitles");
	items.img[i] = NULL; i++;
	items.length = i;

	GuiText titleTxt("Settings", 26, (GXColor){255, 255, 255, 255});
	titleTxt.SetAlignment(ALIGN_LEFT, ALIGN_TOP);
	titleTxt.SetPosition(70, 90);

	GuiTrigger trigA;
	trigA.SetSimpleTrigger(-1, WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A, PAD_BUTTON_A);

	GuiImageData btnOutline(button_png);
	GuiImageData btnOutlineOver(button_over_png);
	GuiText backBtnTxt("Go Back", 24, (GXColor){255, 255, 255, 255});
	GuiImage backBtnImg(&btnOutline);
	GuiImage backBtnImgOver(&btnOutlineOver);
	GuiButton backBtn(btnOutline.GetWidth(), btnOutline.GetHeight());
	backBtn.SetAlignment(ALIGN_LEFT, ALIGN_BOTTOM);
	backBtn.SetPosition(30, -35);
	backBtn.SetLabel(&backBtnTxt);
	backBtn.SetImage(&backBtnImg);
	backBtn.SetImageOver(&backBtnImgOver);
	backBtn.SetTrigger(&trigA);
	backBtn.SetEffectGrow();

	GuiMenuBrowser itemBrowser(300, 400, &items);
	itemBrowser.SetPosition(70, 120);
	itemBrowser.SetAlignment(ALIGN_LEFT, ALIGN_TOP);

	HaltGui();
	mainWindow->Append(&itemBrowser);
	mainWindow->Append(&backBtn);
	mainWindow->Append(&titleTxt);
	ResumeGui();

	while(currentMenu == MENU_SETTINGS && !guiShutdown)
	{
		usleep(THREAD_SLEEP);

		if(selected != itemBrowser.GetSelectedItem())
		{
			selected = itemBrowser.GetSelectedItem();
		}

		ret = itemBrowser.GetClickedItem();

		switch (ret)
		{
			case 0:
				currentMenu = MENU_SETTINGS_GENERAL;
				break;

			case 1:
				currentMenu = MENU_SETTINGS_CACHE;
				break;

			case 2:
				currentMenu = MENU_SETTINGS_NETWORK;
				break;

			case 3:
				currentMenu = MENU_SETTINGS_VIDEO;
				break;

			case 4:
				currentMenu = MENU_SETTINGS_AUDIO;
				break;

			case 5:
				currentMenu = MENU_SETTINGS_SUBTITLES;
				break;
		}

		if(backBtn.GetState() == STATE_CLICKED)
			ChangeMenu(lastMenu);
	}

	HaltGui();
	mainWindow->Remove(&itemBrowser);
	mainWindow->Remove(&titleTxt);
}

static void BackToMplayerCallback(void * ptr)
{
	GuiButton * b = (GuiButton *)ptr;
	if(b->GetState() == STATE_CLICKED)
	{
		b->ResetState();
		LoadMPlayer(); // signal MPlayer to resume
	}
}

static void VideoProgressCallback(void * ptr)
{
	GuiButton * b = (GuiButton *)ptr;

	float total = wiiGetTimeLength();
	int done = wiiGetTimePos();
	float percent = 0;
	
	if(total > 0)
		percent = done/total;
	
	if(b->GetState() == STATE_CLICKED)
	{
		percent = (userInput[b->GetStateChan()].wpad->ir.x - b->GetLeft())/360.0;
		done = total*percent;
		b->ResetState();
		wiiSeekPos(done);
	}
	
	if(percent <= 0.01)
	{
		videobarProgressLeftImg->SetVisible(false);
		videobarProgressMidImg->SetTile(0);
		videobarProgressRightImg->SetVisible(false);
	}
	else if(percent >= 0.99)
	{
		videobarProgressLeftImg->SetVisible(true);
		videobarProgressMidImg->SetTile(84);
		videobarProgressRightImg->SetVisible(true);
	}
	else
	{
		videobarProgressLeftImg->SetVisible(true);
		videobarProgressMidImg->SetTile((int)(84*percent));
		videobarProgressRightImg->SetVisible(false);
	}
	char time[50];
	wiiGetTimeDisplay(time);
	videobarTime->SetText(time);
}

static void VideoSkipBackwardCallback(void * ptr)
{
	GuiButton * b = (GuiButton *)ptr;
	if(b->GetState() == STATE_CLICKED)
	{
		b->ResetState();
		wiiSkipBackward();
	}
}

static void VideoRewindCallback(void * ptr)
{
	GuiButton * b = (GuiButton *)ptr;
	if(b->GetState() == STATE_CLICKED)
	{
		b->ResetState();
		wiiRewind();
	}
}

static void VideoPauseCallback(void * ptr)
{
	GuiButton * b = (GuiButton *)ptr;
	if(b->GetState() == STATE_CLICKED)
	{
		b->ResetState();
		wiiPause();
	}
}

static void VideoFastForwardCallback(void * ptr)
{
	GuiButton * b = (GuiButton *)ptr;
	if(b->GetState() == STATE_CLICKED)
	{
		b->ResetState();
		wiiFastForward();
	}
}

static void VideoSkipForwardCallback(void * ptr)
{
	GuiButton * b = (GuiButton *)ptr;
	if(b->GetState() == STATE_CLICKED)
	{
		b->ResetState();
		wiiSkipForward();
	}
}

static void AudioProgressCallback(void * ptr)
{
	GuiButton * b = (GuiButton *)ptr;

	double total = wiiGetTimeLength();
	int done = wiiGetTimePos();
	double percent = 0;

	if(total > 0)
		percent = done/total;
	
	if(!nowPlayingSet && total > 0)
	{
		nowPlayingSet = true;
		char *title = NULL;
		char *artist = NULL;
		char *album = NULL;
		char *year = NULL;
		int i;
		audiobarNowPlaying[0]->SetVisible(true);

		if(wiiGetMetaTitle() != NULL)
		{
			title = strdup(wiiGetMetaTitle());
			for(i=strlen(title)-1; i >= 0; i--)	if(title[i] == ' ') title[i] = '\0'; else break;
			
			if(strlen(title) > 0)
				audiobarNowPlaying[1]->SetText(title);
		}
		else
		{
			char tmp[MAXJOLIET+1];
			char *start = strrchr(loadedFile,'/');
			if(start != NULL) // start up starting part of path
				strcpy(tmp, &start[1]);
			else
				strcpy(tmp, loadedFile);
			StripExt(tmp);
			audiobarNowPlaying[1]->SetText(tmp);
		}

		if(wiiGetMetaArtist() != NULL)
		{
			artist = strdup(wiiGetMetaArtist());
			for(i=strlen(artist)-1; i >= 0; i--) if(artist[i] == ' ') artist[i] = '\0'; else break;
			
			if(strlen(artist) > 0)
				audiobarNowPlaying[2]->SetText(artist);
		}

		if(wiiGetMetaAlbum() != NULL)
		{
			album = strdup(wiiGetMetaAlbum());
			for(i=strlen(album)-1; i >= 0; i--) if(album[i] == ' ') album[i] = '\0'; else break;
		}

		if(wiiGetMetaYear() != NULL)
		{
			year = strdup(wiiGetMetaYear());
			for(i=strlen(year)-1; i >= 0; i--) if(year[i] == ' ') year[i] = '\0'; else break;
		}

		char txt[1024];

		if(album && strlen(album) > 0 && year && strlen(year) > 0)
		{
			sprintf(txt, "%s (%s)", album, year);
			audiobarNowPlaying[3]->SetText(txt);
		}
		else if(album && strlen(album) > 0)
		{
			audiobarNowPlaying[3]->SetText(album);
		}
		else if(year && strlen(year) > 0)
		{
			audiobarNowPlaying[3]->SetText(year);
		}

		if(title) free(title);
		if(artist) free(artist);
		if(album) free(album);
		if(year) free(year);
		
		for(i=1; i < 4; i++)
			audiobarNowPlaying[i]->SetVisible(true);
	}

	if(b->GetState() == STATE_CLICKED)
	{
		percent = (userInput[b->GetStateChan()].wpad->ir.x - b->GetLeft())/360.0;
		done = total*percent;
		b->ResetState();
		wiiSeekPos(done);
	}

	if(percent <= 0.01)
	{
		audiobarProgressLeftImg->SetVisible(false);
		audiobarProgressMidImg->SetTile(0);
		audiobarProgressRightImg->SetVisible(false);
	}
	else if(percent >= 0.99)
	{
		audiobarProgressLeftImg->SetVisible(true);
		audiobarProgressMidImg->SetTile(84);
		audiobarProgressRightImg->SetVisible(true);
	}
	else
	{
		audiobarProgressLeftImg->SetVisible(true);
		audiobarProgressMidImg->SetTile((int)(84*percent));
		audiobarProgressRightImg->SetVisible(false);
	}
}

static void AudioSkipBackwardCallback(void * ptr)
{
	GuiButton * b = (GuiButton *)ptr;
	if(b->GetState() == STATE_CLICKED)
	{
		b->ResetState();
		// do something
	}
}

static void AudioPauseCallback(void * ptr)
{
	GuiButton * b = (GuiButton *)ptr;
	if(b->GetState() == STATE_CLICKED)
	{
		b->ResetState();
		wiiPause();
	}
}

static void AudioSkipForwardCallback(void * ptr)
{
	GuiButton * b = (GuiButton *)ptr;
	if(b->GetState() == STATE_CLICKED)
	{
		b->ResetState();

		// skip to next song
		ShutdownMPlayer();
		FindNextAudioFile();
		LoadMPlayer();
	}
}

static void PicturePreviousCallback(void * ptr)
{
	GuiButton * b = (GuiButton *)ptr;
	if(b->GetState() == STATE_CLICKED)
	{
		b->ResetState();
		ChangePicture(-1);
	}
}

static void PictureNextCallback(void * ptr)
{
	GuiButton * b = (GuiButton *)ptr;
	if(b->GetState() == STATE_CLICKED)
	{
		b->ResetState();
		ChangePicture(1);
	}
}

static void PictureSlideshowCallback(void * ptr)
{
	GuiButton * b = (GuiButton *)ptr;
	if(b->GetState() == STATE_CLICKED)
	{
		b->ResetState();
		ToggleSlideshow();
	}
}

static void PictureCloseCallback(void * ptr)
{
	GuiButton * b = (GuiButton *)ptr;
	if(b->GetState() == STATE_CLICKED)
	{
		b->ResetState();
		closePictureViewer = 1;
	}
}

static void SetupPlaybar()
{	
	static int playbarSetup = 0;
	
	if(playbarSetup)
		return;

	playbarTrigA = new GuiTrigger();
	playbarTrigA->SetSimpleTrigger(-1, WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A, PAD_BUTTON_A);
	
	// images
	
	playbarProgress = new GuiImageData(playbar_progress_bg_png);
	playbarProgressLeft = new GuiImageData(playbar_progress_left_png);
	playbarProgressMid = new GuiImageData(playbar_progress_mid_png);
	playbarProgressRight = new GuiImageData(playbar_progress_right_png);
	playbarCircle = new GuiImageData(playbar_circle_png);
	playbarCircleOver = new GuiImageData(playbar_circle_over_png);
	playbarSkipBackward = new GuiImageData(playbar_skipbackward_png);
	playbarRewind = new GuiImageData(playbar_rewind_png);
	playbarPause = new GuiImageData(playbar_pause_png);
	playbarPlay = new GuiImageData(playbar_play_png);
	playbarFastForward = new GuiImageData(playbar_fastforward_png);
	playbarSkipForward = new GuiImageData(playbar_skipforward_png);
	playbarClose = new GuiImageData(playbar_close_png);
	playbarSlideshow = new GuiImageData(playbar_slideshow_png);
	
	// video bar

	videobarProgressImg = new GuiImage(playbarProgress);
	videobarProgressLeftImg = new GuiImage(playbarProgressLeft);
	videobarProgressLeftImg->SetPosition(3, 0);
	videobarProgressLeftImg->SetVisible(false);
	videobarProgressMidImg = new GuiImage(playbarProgressMid);
	videobarProgressMidImg->SetPosition(15, 0);
	videobarProgressMidImg->SetTile(0);
	videobarProgressRightImg = new GuiImage(playbarProgressRight);
	videobarProgressRightImg->SetAlignment(ALIGN_RIGHT, ALIGN_TOP);
	videobarProgressRightImg->SetVisible(false);
	videobarSkipBackwardImg = new GuiImage(playbarCircle);
	videobarSkipBackwardImg->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	videobarSkipBackwardOverImg = new GuiImage(playbarCircleOver);
	videobarSkipBackwardOverImg->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	videobarSkipBackwardIcon = new GuiImage(playbarSkipBackward);
	videobarSkipBackwardIcon->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	videobarRewindImg = new GuiImage(playbarCircle);
	videobarRewindImg->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	videobarRewindOverImg = new GuiImage(playbarCircleOver);
	videobarRewindOverImg->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	videobarRewindIcon = new GuiImage(playbarRewind);
	videobarRewindIcon->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	videobarPauseImg = new GuiImage(playbarCircle);
	videobarPauseImg->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	videobarPauseOverImg = new GuiImage(playbarCircleOver);
	videobarPauseOverImg->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	videobarPauseIcon = new GuiImage(playbarPause);
	videobarPauseIcon->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	videobarPlayIcon = new GuiImage(playbarPlay);
	videobarPlayIcon->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	videobarFastForwardImg = new GuiImage(playbarCircle);
	videobarFastForwardImg->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	videobarFastForwardOverImg = new GuiImage(playbarCircleOver);
	videobarFastForwardOverImg->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	videobarFastForwardIcon = new GuiImage(playbarFastForward);
	videobarFastForwardIcon->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	videobarSkipForwardImg = new GuiImage(playbarCircle);
	videobarSkipForwardImg->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	videobarSkipForwardOverImg = new GuiImage(playbarCircleOver);
	videobarSkipForwardOverImg->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	videobarSkipForwardIcon = new GuiImage(playbarSkipForward);
	videobarSkipForwardIcon->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	
	videobarProgressBtn = new GuiButton(videobarProgressImg->GetWidth(), videobarProgressImg->GetHeight());
	videobarProgressBtn->SetImage(videobarProgressImg);
	videobarProgressBtn->SetPosition(3, 0);
	videobarProgressBtn->SetTrigger(playbarTrigA);
	videobarProgressBtn->SetSelectable(false);
	videobarProgressBtn->SetUpdateCallback(VideoProgressCallback);

	videobarSkipBackwardBtn = new GuiButton(50, 50);
	videobarSkipBackwardBtn->SetAlignment(ALIGN_LEFT, ALIGN_BOTTOM);
	videobarSkipBackwardBtn->SetPosition(0, 0);
	videobarSkipBackwardBtn->SetImage(videobarSkipBackwardImg);
	videobarSkipBackwardBtn->SetImageOver(videobarSkipBackwardOverImg);
	videobarSkipBackwardBtn->SetIcon(videobarSkipBackwardIcon);
	videobarSkipBackwardBtn->SetTrigger(playbarTrigA);
	videobarSkipBackwardBtn->SetUpdateCallback(VideoSkipBackwardCallback);
	videobarSkipBackwardBtn->SetEffectGrow();
	
	videobarRewindBtn = new GuiButton(50, 50);
	videobarRewindBtn->SetAlignment(ALIGN_LEFT, ALIGN_BOTTOM);
	videobarRewindBtn->SetPosition(80, 0);
	videobarRewindBtn->SetImage(videobarRewindImg);
	videobarRewindBtn->SetImageOver(videobarRewindOverImg);
	videobarRewindBtn->SetIcon(videobarRewindIcon);
	videobarRewindBtn->SetTrigger(playbarTrigA);
	videobarRewindBtn->SetUpdateCallback(VideoRewindCallback);
	videobarRewindBtn->SetEffectGrow();
	
	videobarPauseBtn = new GuiButton(50, 50);
	videobarPauseBtn->SetAlignment(ALIGN_LEFT, ALIGN_BOTTOM);
	videobarPauseBtn->SetPosition(160, 0);
	videobarPauseBtn->SetImage(videobarPauseImg);
	videobarPauseBtn->SetImageOver(videobarPauseOverImg);
	videobarPauseBtn->SetIcon(videobarPauseIcon);
	videobarPauseBtn->SetTrigger(playbarTrigA);
	videobarPauseBtn->SetUpdateCallback(VideoPauseCallback);
	videobarPauseBtn->SetEffectGrow();
	
	videobarFastForwardBtn = new GuiButton(50, 50);
	videobarFastForwardBtn->SetAlignment(ALIGN_LEFT, ALIGN_BOTTOM);
	videobarFastForwardBtn->SetPosition(240, 0);
	videobarFastForwardBtn->SetImage(videobarFastForwardImg);
	videobarFastForwardBtn->SetImageOver(videobarFastForwardOverImg);
	videobarFastForwardBtn->SetIcon(videobarFastForwardIcon);
	videobarFastForwardBtn->SetTrigger(playbarTrigA);
	videobarFastForwardBtn->SetUpdateCallback(VideoFastForwardCallback);
	videobarFastForwardBtn->SetEffectGrow();
	
	videobarSkipForwardBtn = new GuiButton(50, 50);
	videobarSkipForwardBtn->SetAlignment(ALIGN_LEFT, ALIGN_BOTTOM);
	videobarSkipForwardBtn->SetPosition(320, 0);
	videobarSkipForwardBtn->SetImage(videobarSkipForwardImg);
	videobarSkipForwardBtn->SetImageOver(videobarSkipForwardOverImg);
	videobarSkipForwardBtn->SetIcon(videobarSkipForwardIcon);
	videobarSkipForwardBtn->SetTrigger(playbarTrigA);
	videobarSkipForwardBtn->SetUpdateCallback(VideoSkipForwardCallback);
	videobarSkipForwardBtn->SetEffectGrow();
	
	videobarTime = new GuiText(NULL, 16, (GXColor){0, 0, 0, 255});
	videobarTime->SetAlignment(ALIGN_RIGHT, ALIGN_TOP);
	videobarTime->SetPosition(0, -13);

	videobar = new GuiWindow(360, 80);

	videobar->Append(videobarProgressBtn);
	videobar->Append(videobarProgressLeftImg);
	videobar->Append(videobarProgressMidImg);
	videobar->Append(videobarProgressRightImg);
	videobar->Append(videobarSkipBackwardBtn);
	videobar->Append(videobarRewindBtn);
	videobar->Append(videobarPauseBtn);
	videobar->Append(videobarFastForwardBtn);
	videobar->Append(videobarSkipForwardBtn);
	videobar->Append(videobarTime);
	
	videobar->SetAlignment(ALIGN_CENTRE, ALIGN_BOTTOM);
	videobar->SetPosition(0, -40);
	
	// audio bar
	
	audiobarProgressImg = new GuiImage(playbarProgress);
	audiobarProgressLeftImg = new GuiImage(playbarProgressLeft);
	audiobarProgressLeftImg->SetPosition(3, 0);
	audiobarProgressLeftImg->SetVisible(false);
	audiobarProgressMidImg = new GuiImage(playbarProgressMid);
	audiobarProgressMidImg->SetPosition(15, 0);
	audiobarProgressMidImg->SetTile(0);
	audiobarProgressRightImg = new GuiImage(playbarProgressRight);
	audiobarProgressRightImg->SetAlignment(ALIGN_RIGHT, ALIGN_TOP);
	audiobarProgressRightImg->SetVisible(false);
	audiobarSkipBackwardImg = new GuiImage(playbarCircle);
	audiobarSkipBackwardImg->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	audiobarSkipBackwardOverImg = new GuiImage(playbarCircleOver);
	audiobarSkipBackwardOverImg->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	audiobarSkipBackwardIcon = new GuiImage(playbarSkipBackward);
	audiobarSkipBackwardIcon->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	audiobarPauseImg = new GuiImage(playbarCircle);
	audiobarPauseImg->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	audiobarPauseOverImg = new GuiImage(playbarCircleOver);
	audiobarPauseOverImg->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	audiobarPauseIcon = new GuiImage(playbarPause);
	audiobarPauseIcon->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	audiobarPlayIcon = new GuiImage(playbarPlay);
	audiobarPlayIcon->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	audiobarSkipForwardImg = new GuiImage(playbarCircle);
	audiobarSkipForwardImg->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	audiobarSkipForwardOverImg = new GuiImage(playbarCircleOver);
	audiobarSkipForwardOverImg->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	audiobarSkipForwardIcon = new GuiImage(playbarSkipForward);
	audiobarSkipForwardIcon->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	
	audiobarProgressBtn = new GuiButton(audiobarProgressImg->GetWidth(), audiobarProgressImg->GetHeight());
	audiobarProgressBtn->SetImage(audiobarProgressImg);
	audiobarProgressBtn->SetPosition(3, 0);
	audiobarProgressBtn->SetTrigger(playbarTrigA);
	audiobarProgressBtn->SetSelectable(false);
	audiobarProgressBtn->SetUpdateCallback(AudioProgressCallback);

	audiobarSkipBackwardBtn = new GuiButton(50, 50);
	audiobarSkipBackwardBtn->SetAlignment(ALIGN_LEFT, ALIGN_BOTTOM);
	audiobarSkipBackwardBtn->SetPosition(0, 0);
	audiobarSkipBackwardBtn->SetImage(audiobarSkipBackwardImg);
	audiobarSkipBackwardBtn->SetImageOver(audiobarSkipBackwardOverImg);
	audiobarSkipBackwardBtn->SetIcon(audiobarSkipBackwardIcon);
	audiobarSkipBackwardBtn->SetTrigger(playbarTrigA);
	audiobarSkipBackwardBtn->SetUpdateCallback(AudioSkipBackwardCallback);
	audiobarSkipBackwardBtn->SetEffectGrow();
	
	audiobarPauseBtn = new GuiButton(50, 50);
	audiobarPauseBtn->SetAlignment(ALIGN_LEFT, ALIGN_BOTTOM);
	audiobarPauseBtn->SetPosition(80, 0);
	audiobarPauseBtn->SetImage(audiobarPauseImg);
	audiobarPauseBtn->SetImageOver(audiobarPauseOverImg);
	audiobarPauseBtn->SetIcon(audiobarPauseIcon);
	audiobarPauseBtn->SetTrigger(playbarTrigA);
	audiobarPauseBtn->SetUpdateCallback(AudioPauseCallback);
	audiobarPauseBtn->SetEffectGrow();
	
	audiobarSkipForwardBtn = new GuiButton(50, 50);
	audiobarSkipForwardBtn->SetAlignment(ALIGN_LEFT, ALIGN_BOTTOM);
	audiobarSkipForwardBtn->SetPosition(160, 0);
	audiobarSkipForwardBtn->SetImage(audiobarSkipForwardImg);
	audiobarSkipForwardBtn->SetImageOver(audiobarSkipForwardOverImg);
	audiobarSkipForwardBtn->SetIcon(audiobarSkipForwardIcon);
	audiobarSkipForwardBtn->SetTrigger(playbarTrigA);
	audiobarSkipForwardBtn->SetUpdateCallback(AudioSkipForwardCallback);
	audiobarSkipForwardBtn->SetEffectGrow();
	
	audiobarNowPlaying[0] = new GuiText("Now Playing:", 16, (GXColor){0, 0, 0, 255});
	audiobarNowPlaying[0]->SetAlignment(ALIGN_RIGHT, ALIGN_TOP);
	audiobarNowPlaying[0]->SetPosition(0, 0);
	
	for(int i=1; i < 4; i++)
	{
		audiobarNowPlaying[i] = new GuiText(NULL, 16, (GXColor){0, 0, 0, 255});
		audiobarNowPlaying[i]->SetAlignment(ALIGN_RIGHT, ALIGN_TOP);
		audiobarNowPlaying[i]->SetPosition(0, 20*i);
		audiobarNowPlaying[i]->SetMaxWidth(250);
	}

	audiobar = new GuiWindow(500, 80);

	audiobar->Append(audiobarProgressBtn);
	audiobar->Append(audiobarProgressLeftImg);
	audiobar->Append(audiobarProgressMidImg);
	audiobar->Append(audiobarProgressRightImg);
	audiobar->Append(audiobarSkipBackwardBtn);
	audiobar->Append(audiobarPauseBtn);
	audiobar->Append(audiobarSkipForwardBtn);
	
	for(int i=0; i < 4; i++)
		audiobar->Append(audiobarNowPlaying[i]);

	audiobar->SetAlignment(ALIGN_LEFT, ALIGN_BOTTOM);
	audiobar->SetPosition(100, -30);
	
	// setup picture bar

	picturebarPreviousImg = new GuiImage(playbarCircle);
	picturebarPreviousImg->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	picturebarPreviousOverImg = new GuiImage(playbarCircleOver);
	picturebarPreviousOverImg->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	picturebarPreviousIcon = new GuiImage(playbarRewind);
	picturebarPreviousIcon->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	
	picturebarNextImg = new GuiImage(playbarCircle);
	picturebarNextImg->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	picturebarNextOverImg = new GuiImage(playbarCircleOver);
	picturebarNextOverImg->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	picturebarNextIcon = new GuiImage(playbarFastForward);
	picturebarNextIcon->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	
	picturebarSlideshowImg = new GuiImage(playbarCircle);
	picturebarSlideshowImg->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	picturebarSlideshowOverImg = new GuiImage(playbarCircleOver);
	picturebarSlideshowOverImg->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	picturebarSlideshowIcon = new GuiImage(playbarSlideshow);
	picturebarSlideshowIcon->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);

	picturebarCloseImg = new GuiImage(playbarCircle);
	picturebarCloseImg->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	picturebarCloseOverImg = new GuiImage(playbarCircleOver);
	picturebarCloseOverImg->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	picturebarCloseIcon = new GuiImage(playbarClose);
	picturebarCloseIcon->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);

	picturebarPreviousBtn = new GuiButton(50, 50);
	picturebarPreviousBtn->SetAlignment(ALIGN_LEFT, ALIGN_BOTTOM);
	picturebarPreviousBtn->SetPosition(0, 0);
	picturebarPreviousBtn->SetImage(picturebarPreviousImg);
	picturebarPreviousBtn->SetImageOver(picturebarPreviousOverImg);
	picturebarPreviousBtn->SetIcon(picturebarPreviousIcon);
	picturebarPreviousBtn->SetTrigger(playbarTrigA);
	picturebarPreviousBtn->SetUpdateCallback(PicturePreviousCallback);
	picturebarPreviousBtn->SetEffectGrow();
	
	picturebarNextBtn = new GuiButton(50, 50);
	picturebarNextBtn->SetAlignment(ALIGN_LEFT, ALIGN_BOTTOM);
	picturebarNextBtn->SetPosition(80, 0);
	picturebarNextBtn->SetImage(picturebarNextImg);
	picturebarNextBtn->SetImageOver(picturebarNextOverImg);
	picturebarNextBtn->SetIcon(picturebarNextIcon);
	picturebarNextBtn->SetTrigger(playbarTrigA);
	picturebarNextBtn->SetUpdateCallback(PictureNextCallback);
	picturebarNextBtn->SetEffectGrow();
	
	picturebarSlideshowBtn = new GuiButton(50, 50);
	picturebarSlideshowBtn->SetAlignment(ALIGN_LEFT, ALIGN_BOTTOM);
	picturebarSlideshowBtn->SetPosition(160, 0);
	picturebarSlideshowBtn->SetImage(picturebarSlideshowImg);
	picturebarSlideshowBtn->SetImageOver(picturebarSlideshowOverImg);
	picturebarSlideshowBtn->SetIcon(picturebarSlideshowIcon);
	picturebarSlideshowBtn->SetTrigger(playbarTrigA);
	picturebarSlideshowBtn->SetUpdateCallback(PictureSlideshowCallback);
	picturebarSlideshowBtn->SetEffectGrow();
	
	picturebarCloseBtn = new GuiButton(50, 50);
	picturebarCloseBtn->SetAlignment(ALIGN_LEFT, ALIGN_BOTTOM);
	picturebarCloseBtn->SetPosition(240, 0);
	picturebarCloseBtn->SetImage(picturebarCloseImg);
	picturebarCloseBtn->SetImageOver(picturebarCloseOverImg);
	picturebarCloseBtn->SetIcon(picturebarCloseIcon);
	picturebarCloseBtn->SetTrigger(playbarTrigA);
	picturebarCloseBtn->SetUpdateCallback(PictureCloseCallback);
	picturebarCloseBtn->SetEffectGrow();
	
	picturebar = new GuiWindow(360, 80);

	picturebar->Append(picturebarPreviousBtn);
	picturebar->Append(picturebarNextBtn);
	picturebar->Append(picturebarSlideshowBtn);
	picturebar->Append(picturebarCloseBtn);
	
	playbarSetup = 1;
}

void HideNowPlaying()
{
	for(int i=0; i < 4; i++)
	{
		audiobarNowPlaying[i]->SetVisible(false);
		if(i > 0) audiobarNowPlaying[i]->SetText(NULL);
	}
}

/****************************************************************************
 * Menu
 ***************************************************************************/
void WiiMenu()
{
	menuMode = 0; // switch to normal GUI mode
	guiShutdown = false;
	selectLoadedFile = 1;

	if(pointer[0] == NULL)
	{
		pointer[0] = new GuiImageData(player1_point_png);
		pointer[1] = new GuiImageData(player2_point_png);
		pointer[2] = new GuiImageData(player3_point_png);
		pointer[3] = new GuiImageData(player4_point_png);
	}

	GuiTrigger trigA;
	trigA.SetSimpleTrigger(-1, WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A, PAD_BUTTON_A);

	mainWindow = new GuiWindow(screenwidth, screenheight);
	
	GuiImage bg(screenwidth, screenheight, (GXColor){155, 155, 155, 255});
	bg.SetAlignment(ALIGN_LEFT, ALIGN_TOP);
	GuiImage bg2(560, 332, (GXColor){0, 0, 0, 168});
	bg2.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	
	GuiImageData bgRounded(bg_rounded_png);
	GuiImage bgRoundedImg(&bgRounded);
	bgRoundedImg.SetPosition(0, 332);

	/*GuiText mplayerBtnTxt("MPlayer", 18, (GXColor){255, 255, 255, 255});
	mplayerBtn = new GuiButton(100, 100);
	mplayerBtn->SetAlignment(ALIGN_RIGHT, ALIGN_TOP);
	mplayerBtn->SetPosition(-30, 20);
	mplayerBtn->SetLabel(&mplayerBtnTxt);
	mplayerBtn->SetTrigger(&trigA);
	mplayerBtn->SetEffectGrow();
	mplayerBtn->SetUpdateCallback(BackToMplayerCallback);*/
	
	mainWindow->Append(&bg);

	if(videoScreenshot)
	{
		videoImg = new GuiImage(videoScreenshot, screenwidth, screenheight);
		mainWindow->Append(videoImg);
		//mainWindow->Append(mplayerBtn);
	}
	
	mainWindow->Append(&bg2);
	mainWindow->Append(&bgRoundedImg);

	GuiImageData logo(logo_png);
	GuiImage logoBtnImg(&logo);
	logoBtn = new GuiButton(logo.GetWidth(), logo.GetHeight());
	logoBtn->SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	logoBtn->SetPosition(70, 30);
	logoBtn->SetImage(&logoBtnImg);
	logoBtn->SetTrigger(&trigA);
	logoBtn->SetUpdateCallback(DisplayCredits);
	mainWindow->Append(logoBtn);

	GuiImageData videos(nav_videos_png);
	GuiImageData music(nav_music_png);
	GuiImageData pictures(nav_pictures_png);
	GuiImageData dvd(nav_dvd_png);
	GuiImageData online(nav_onlinemedia_png);
	GuiImageData settings(nav_settings_png);
	GuiImageData exit(nav_exit_png);

	GuiTooltip videosBtnTip("Videos");
	GuiImage videosBtnImg(&videos);
	videosBtn = new GuiButton(videosBtnImg.GetWidth(), videosBtnImg.GetHeight());
	videosBtn->SetAlignment(ALIGN_LEFT, ALIGN_TOP);
	videosBtn->SetPosition(58, 30);
	videosBtn->SetTooltip(&videosBtnTip);
	videosBtn->SetImage(&videosBtnImg);
	videosBtn->SetTrigger(&trigA);
	videosBtn->SetEffectGrow();
	videosBtn->SetUpdateCallback(ChangeMenuVideos);

	GuiTooltip musicBtnTip("Music");
	GuiImage musicBtnImg(&music);
	musicBtn = new GuiButton(musicBtnImg.GetWidth(), musicBtnImg.GetHeight());
	musicBtn->SetAlignment(ALIGN_LEFT, ALIGN_TOP);
	musicBtn->SetPosition(106, 30);
	musicBtn->SetTooltip(&musicBtnTip);
	musicBtn->SetImage(&musicBtnImg);
	musicBtn->SetTrigger(&trigA);
	musicBtn->SetEffectGrow();
	musicBtn->SetUpdateCallback(ChangeMenuMusic);

	GuiTooltip picturesBtnTip("Pictures");
	GuiImage picturesBtnImg(&pictures);
	picturesBtn = new GuiButton(picturesBtnImg.GetWidth(), picturesBtnImg.GetHeight());
	picturesBtn->SetAlignment(ALIGN_LEFT, ALIGN_TOP);
	picturesBtn->SetPosition(154, 30);
	picturesBtn->SetTooltip(&picturesBtnTip);
	picturesBtn->SetImage(&picturesBtnImg);
	picturesBtn->SetTrigger(&trigA);
	picturesBtn->SetEffectGrow();
	picturesBtn->SetUpdateCallback(ChangeMenuPictures);

	GuiTooltip dvdBtnTip("DVD");
	GuiImage dvdBtnImg(&dvd);
	dvdBtn = new GuiButton(dvdBtnImg.GetWidth(), dvdBtnImg.GetHeight());
	dvdBtn->SetAlignment(ALIGN_LEFT, ALIGN_TOP);
	dvdBtn->SetPosition(202, 30);
	dvdBtn->SetTooltip(&dvdBtnTip);
	dvdBtn->SetImage(&dvdBtnImg);
	dvdBtn->SetTrigger(&trigA);
	dvdBtn->SetEffectGrow();
	dvdBtn->SetUpdateCallback(ChangeMenuDVD);

	GuiTooltip onlineBtnTip("Online Media");
	GuiImage onlineBtnImg(&online);
	onlineBtn = new GuiButton(onlineBtnImg.GetWidth(), onlineBtnImg.GetHeight());
	onlineBtn->SetAlignment(ALIGN_LEFT, ALIGN_TOP);
	onlineBtn->SetPosition(250, 30);
	onlineBtn->SetTooltip(&onlineBtnTip);
	onlineBtn->SetImage(&onlineBtnImg);
	onlineBtn->SetTrigger(&trigA);
	onlineBtn->SetEffectGrow();
	onlineBtn->SetUpdateCallback(ChangeMenuOnline);

	GuiTooltip settingsBtnTip("Settings");
	GuiImage settingsBtnImg(&settings);
	settingsBtn = new GuiButton(settingsBtnImg.GetWidth(), settingsBtnImg.GetHeight());
	settingsBtn->SetAlignment(ALIGN_RIGHT, ALIGN_TOP);
	settingsBtn->SetPosition(-106, 30);
	settingsBtn->SetImage(&settingsBtnImg);
	settingsBtn->SetTooltip(&settingsBtnTip);
	settingsBtn->SetTrigger(&trigA);
	settingsBtn->SetEffectGrow();
	settingsBtn->SetUpdateCallback(ChangeMenuSettings);
	
	GuiTooltip exitBtnTip("Exit");
	GuiImage exitBtnImg(&exit);
	exitBtn = new GuiButton(exitBtnImg.GetWidth(), exitBtnImg.GetHeight());
	exitBtn->SetAlignment(ALIGN_RIGHT, ALIGN_TOP);
	exitBtn->SetPosition(-58, 30);
	exitBtn->SetImage(&exitBtnImg);
	exitBtn->SetTooltip(&exitBtnTip);
	exitBtn->SetTrigger(&trigA);
	exitBtn->SetEffectGrow();
	exitBtn->SetUpdateCallback(ExitButtonCallback);

	mainWindow->Append(videosBtn);
	mainWindow->Append(musicBtn);
	mainWindow->Append(picturesBtn);
	mainWindow->Append(dvdBtn);
	mainWindow->Append(onlineBtn);
	mainWindow->Append(settingsBtn);
	mainWindow->Append(exitBtn);

	// play bar
	SetupPlaybar();

	ResumeGui();

	// Load settings
	if(!LoadSettings())
		SaveSettings(SILENT);

	while(!guiShutdown)
	{
		switch (currentMenu)
		{
			case MENU_BROWSE_VIDEOS:
			case MENU_BROWSE_MUSIC:
			case MENU_BROWSE_PICTURES:
			case MENU_BROWSE_ONLINEMEDIA:
				MenuBrowse(currentMenu);
				break;
			case MENU_DVD:
				MenuDVD();
				break;
			case MENU_SETTINGS:
				MenuSettings();
				break;
			case MENU_SETTINGS_GENERAL:
				MenuSettingsGeneral();
				break;
			case MENU_SETTINGS_CACHE:
				MenuSettingsCache();
				break;
			case MENU_SETTINGS_NETWORK:
				MenuSettingsNetwork();
				break;
			case MENU_SETTINGS_NETWORK_SMB:
				MenuSettingsNetworkSMB();
				break;
			case MENU_SETTINGS_NETWORK_FTP:
				MenuSettingsNetworkFTP();
				break;
			case MENU_SETTINGS_VIDEO:
				MenuSettingsVideo();
				break;
			case MENU_SETTINGS_AUDIO:
				MenuSettingsAudio();
				break;
			case MENU_SETTINGS_SUBTITLES:
				MenuSettingsSubtitles();
				break;
			default: // unrecognized menu
				MenuBrowse(MENU_BROWSE_VIDEOS);
				break;
		}
		
		usleep(THREAD_SLEEP);
	}

	ShutoffRumble();
	CancelAction();
	HaltGui();

	delete mainWindow;
	mainWindow = NULL;

	delete videosBtn;
	videosBtn = NULL;
	delete musicBtn;
	musicBtn = NULL;
	delete picturesBtn;
	picturesBtn = NULL;
	delete dvdBtn;
	dvdBtn = NULL;
	delete onlineBtn;
	onlineBtn = NULL;
	delete settingsBtn;
	settingsBtn = NULL;
	delete exitBtn;
	exitBtn = NULL;
	delete logoBtn;
	logoBtn = NULL;
	//delete mplayerBtn;
	//mplayerBtn = NULL;

	if(videoImg)
	{
		delete videoImg;
		videoImg = NULL;
	}

	if(videoScreenshot)
	{
		free(videoScreenshot);
		videoScreenshot = NULL;
	}
}

bool StatusSet()
{
	return statusText->IsVisible();
}

void SetStatus(const char * txt)
{
	if(txt)
		statusText->SetVisible(true);
	else
		statusText->SetVisible(false);

	statusText->SetText(txt);
}

/****************************************************************************
 * MPlayer Menu
 ***************************************************************************/
void MPlayerMenu()
{
	menuMode = 1; // switch to MPlayer GUI mode
	
	GuiTrigger trigA;
	trigA.SetSimpleTrigger(-1, WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A, PAD_BUTTON_A);

	mainWindow = new GuiWindow(screenwidth, screenheight);

	GuiImage bgBottom(screenwidth, 140, (GXColor){155, 155, 155, 155});
	bgBottom.SetAlignment(ALIGN_LEFT, ALIGN_BOTTOM);
	
	// status text
	statusText = new GuiText(NULL, 24, (GXColor){255, 255, 255, 255});
	statusText->SetVisible(false);

	mainWindow->Append(&bgBottom);
	mainWindow->Append(videobar);
	mainWindow->Append(statusText);

	ResumeGui();

	while(!controlledbygui)
	{
		usleep(THREAD_SLEEP);

		if(mainWindow->IsVisible() && wiiInDVDMenu())
		{
			mainWindow->SetVisible(false);
			mainWindow->SetState(STATE_DISABLED);
		}
		else if(!mainWindow->IsVisible() && !wiiInDVDMenu())
		{
			mainWindow->SetVisible(true);
			mainWindow->SetState(STATE_DEFAULT);
		}
	}

	ShutoffRumble();
	CancelAction();
	HaltGui();

	delete statusText;
	delete mainWindow;
	mainWindow = NULL;
}
