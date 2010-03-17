/****************************************************************************
 * WiiMC
 * Tantric 2009-2010
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
#include "musicplaylist.h"
#include "filebrowser.h"
#include "utils/gettext.h"
#include "filelist.h"

#define THREAD_SLEEP 100

static GuiImageData * pointer[4] = { NULL, NULL, NULL, NULL };
static GuiImageData throbber(throbber_png);
static GuiImage * videoImg = NULL;
static GuiButton * videosBtn = NULL;
static GuiButton * musicBtn = NULL;
static GuiButton * picturesBtn = NULL;
static GuiButton * dvdBtn = NULL;
static GuiButton * onlineBtn = NULL;
static GuiButton * settingsBtn = NULL;
static GuiButton * exitBtn = NULL;

static GuiButton * logoBtn = NULL;
static GuiWindow * mainWindow = NULL;
static GuiText * settingText = NULL;
static GuiText * settingText2 = NULL;

// actionbar

static GuiText * statusText = NULL;

static GuiWindow * videobar = NULL;
static GuiWindow * audiobar = NULL;
static GuiWindow * picturebar = NULL;
static GuiTrigger * actionbarTrigA = NULL;

static GuiImageData * actionbarProgress = NULL;
static GuiImageData * actionbarProgressLeft = NULL;
static GuiImageData * actionbarProgressMid = NULL;
static GuiImageData * actionbarProgressRight = NULL;
static GuiImageData * actionbarCircle = NULL;
static GuiImageData * actionbarCircleOver = NULL;
static GuiImageData * actionbarSkipBackward = NULL;
static GuiImageData * actionbarRewind = NULL;
static GuiImageData * actionbarPause = NULL;
static GuiImageData * actionbarPlay = NULL;
static GuiImageData * actionbarFastForward = NULL;
static GuiImageData * actionbarSkipForward = NULL;
static GuiImageData * actionbarSingle = NULL;
static GuiImageData * actionbarContinuous = NULL;
static GuiImageData * actionbarShuffle = NULL;
static GuiImageData * actionbarLoop = NULL;
static GuiImageData * actionbarSlideshow = NULL;
static GuiImageData * actionbarClose = NULL;

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
static GuiImage * videobarFastForwardImg = NULL;
static GuiImage * videobarFastForwardOverImg = NULL;
static GuiImage * videobarFastForwardIcon = NULL;
static GuiImage * videobarSkipForwardImg = NULL;
static GuiImage * videobarSkipForwardOverImg = NULL;
static GuiImage * videobarSkipForwardIcon = NULL;

static GuiTooltip * videobarSkipBackwardTip = NULL;
static GuiTooltip * videobarRewindTip = NULL;
static GuiTooltip * videobarPauseTip = NULL;
static GuiTooltip * videobarFastForwardTip = NULL;
static GuiTooltip * videobarSkipForwardTip = NULL;

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
static GuiImage * audiobarSkipForwardImg = NULL;
static GuiImage * audiobarSkipForwardOverImg = NULL;
static GuiImage * audiobarSkipForwardIcon = NULL;
static GuiImage * audiobarModeImg = NULL;
static GuiImage * audiobarModeOverImg = NULL;
static GuiImage * audiobarModeIcon = NULL;

static GuiTooltip * audiobarSkipBackwardTip = NULL;
static GuiTooltip * audiobarPauseTip = NULL;
static GuiTooltip * audiobarSkipForwardTip = NULL;
static GuiTooltip * audiobarModeTip = NULL;

static GuiButton * audiobarProgressBtn = NULL;
static GuiButton * audiobarSkipBackwardBtn = NULL;
static GuiButton * audiobarPauseBtn = NULL;
static GuiButton * audiobarSkipForwardBtn = NULL;
static GuiButton * audiobarModeBtn = NULL;

static GuiText * audiobarNowPlaying[4] = { NULL, NULL, NULL, NULL };
bool nowPlayingSet = false;

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

static GuiTooltip * picturebarPreviousTip = NULL;
static GuiTooltip * picturebarNextTip = NULL;
static GuiTooltip * picturebarSlideshowTip = NULL;
static GuiTooltip * picturebarCloseTip = NULL;

static GuiButton * picturebarPreviousBtn = NULL;
static GuiButton * picturebarNextBtn = NULL;
static GuiButton * picturebarSlideshowBtn = NULL;
static GuiButton * picturebarCloseBtn = NULL;

int menuCurrent = MENU_BROWSE_VIDEOS;
static int menuPrevious = MENU_BROWSE_VIDEOS;
static int menuUndo = MENU_BROWSE_VIDEOS;
bool inPlaylist = false;
static int netEditIndex = 0; // current index of FTP/SMB share being edited

// threads
static lwp_t guithread = LWP_THREAD_NULL;
static lwp_t progressthread = LWP_THREAD_NULL;
static lwp_t creditsthread = LWP_THREAD_NULL;
static lwp_t updatethread = LWP_THREAD_NULL;
static lwp_t picturethread = LWP_THREAD_NULL;

static int progressThreadHalt = 0;
static int creditsThreadHalt = 0;
static int updateThreadHalt = 0;
static int pictureThreadHalt = 0;

static int guiHalt = 0;
static bool guiShutdown = true;
static int showProgress = 0;

static char progressTitle[100];
static char progressMsg[200];
static int progressDone = 0;
static int progressTotal = 0;

int doMPlayerGuiDraw = 0; // draw MPlayer menu
static bool menuMode = 0; // 0 - normal GUI, 1 - GUI for MPlayer

static void ChangeMenu(int menu)
{
	if(menu == menuCurrent)
		return;

	menuUndo = menuPrevious;
	menuPrevious = menuCurrent;
	menuCurrent = menu;
}

static void UndoChangeMenu()
{
	menuCurrent = menuPrevious;
	menuPrevious = menuUndo;
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
 * UpdateThread
 *
 * Prompts for confirmation, and downloads/installs updates
 ***************************************************************************/
static void * UpdateThread (void *arg)
{
	while(1)
	{
		LWP_SuspendThread(updatethread);

		if(updateThreadHalt == 2)
			return NULL;

		bool installUpdate = WindowPrompt(
			"Update Available",
			"An update is available!",
			"Update now",
			"Update later");
		if(installUpdate)
			if(DownloadUpdate())
				ExitRequested = 1;
	}
	return NULL;
}

static void ResumeUpdateThread()
{
	if(updatethread == LWP_THREAD_NULL || guiShutdown)
		return;

	updateThreadHalt = 0;
	LWP_ResumeThread(updatethread);
}

/****************************************************************************
 * GuiThread
 *
 * Primary GUI thread to allow GUI to respond to state changes, and draws GUI
 ***************************************************************************/
static void *GuiThread (void *arg)
{
	int i;

	while(1)
	{
		if(guiHalt == 1)
			LWP_SuspendThread(guithread);

		UpdatePads();

		if(menuMode == 1)
			MPlayerInput();

		if(menuMode == 0 || doMPlayerGuiDraw)
		{
			mainWindow->Draw();

			if (mainWindow->GetState() != STATE_DISABLED)
				mainWindow->DrawTooltip();

			i = 3;
			do
			{
				if(userInput[i].wpad->ir.valid)
					Menu_DrawImg(userInput[i].wpad->ir.x-48, userInput[i].wpad->ir.y-48,
						96, 96, pointer[i]->GetImage(), userInput[i].wpad->ir.angle, 1, 1, 255);
				DoRumble(i);
				--i;
			} while(i>=0);
			doMPlayerGuiDraw = 0;
		}

		mainWindow->Update(&userInput[3]);
		mainWindow->Update(&userInput[2]);
		mainWindow->Update(&userInput[1]);
		mainWindow->Update(&userInput[0]);

		if(menuMode == 0) // normal GUI
		{
			for(i=3; i >= 0; i--)
			{
				if(userInput[i].wpad->btns_d & WPAD_BUTTON_1)
				{
					int newMenu = menuCurrent + 1;
					if(newMenu > MENU_SETTINGS)
						newMenu = MENU_BROWSE_VIDEOS;
					ChangeMenu(newMenu);
				}
				else if(userInput[i].wpad->btns_d & WPAD_BUTTON_2)
				{
					int newMenu = menuCurrent - 1;
					if(newMenu < MENU_BROWSE_VIDEOS)
						newMenu = MENU_SETTINGS;
					ChangeMenu(newMenu);
				}
			}

			Menu_Render();

			if(updateFound)
			{
				updateFound = false;
				ResumeUpdateThread();
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
static void ResumeGui()
{
	if(guithread == LWP_THREAD_NULL || guiShutdown)
		return;

	guiHalt = 0;
	LWP_ResumeThread (guithread);
}

/****************************************************************************
 * SuspendGui
 *
 * Signals the GUI thread to stop, and waits for GUI thread to stop
 * This is necessary whenever removing/inserting new elements into the GUI.
 * This eliminates the possibility that the GUI is in the middle of accessing
 * an element that is being changed.
 ***************************************************************************/
static void SuspendGui()
{
	if(guithread == LWP_THREAD_NULL)
		return;

	guiHalt = 1;

	// wait for thread to finish
	while(!LWP_ThreadIsSuspended(guithread))
		usleep(THREAD_SLEEP);
}

extern "C" {
void ShutdownGui()
{
	if(menuMode == 1) // prevent MPlayer from shutting down OSD
		return;

	guiShutdown = true;
	CancelAction();
	SuspendGui();
}
}

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
	SuspendGui();
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
	SuspendGui();
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
	if(!showProgress || progressThreadHalt > 0)
		return;

	SuspendGui();
	mainWindow->SetState(STATE_DISABLED);
	mainWindow->Append(&promptWindow);
	mainWindow->ChangeFocus(&promptWindow);
	ResumeGui();

	float angle = 0;
	u32 count = 0;

	while(showProgress && progressThreadHalt == 0)
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
				angle+=45.0f;
				if(angle >= 360.0f)
					angle = 0;
				throbberImg.SetAngle(angle);
			}
			++count;
		}
	}

	SuspendGui();
	mainWindow->Remove(&promptWindow);
	mainWindow->SetState(STATE_DEFAULT);
	ResumeGui();
}

static void * ProgressThread (void *arg)
{
	while(1)
	{
		if(progressThreadHalt == 1)
			LWP_SuspendThread (progressthread);
		if(progressThreadHalt == 2)
			return NULL;

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
	progressThreadHalt = 1;
	showProgress = 0;

	if(progressthread == LWP_THREAD_NULL)
		return;

	// wait for thread to finish
	while(!LWP_ThreadIsSuspended(progressthread))
		usleep(THREAD_SLEEP);
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
	if(progressthread == LWP_THREAD_NULL || guiShutdown)
		return;

	if(!mainWindow || ExitRequested || ShutdownRequested)
		return;

	if(total < (256*1024))
		return;
	
	if(done > total) // this shouldn't happen
		done = total;

	if(showProgress != 1)
		CancelAction(); // wait for previous progress window to finish

	strncpy(progressMsg, msg, 200);
	sprintf(progressTitle, "Please Wait");
	progressThreadHalt = 0;
	showProgress = 1;
	progressTotal = total;
	progressDone = done;

	LWP_ResumeThread (progressthread);
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

	if(progressthread == LWP_THREAD_NULL || guiShutdown)
		return;

	if(showProgress != 2)
		CancelAction(); // wait for previous progress window to finish

	strncpy(progressMsg, msg, 200);
	sprintf(progressTitle, "Please Wait");
	progressThreadHalt = 0;
	showProgress = 2;
	progressDone = 0;
	progressTotal = 0;

	LWP_ResumeThread (progressthread);
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

	SuspendGui();
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

	SuspendGui();
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

	SuspendGui();
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
	SuspendGui();
	mainWindow->Remove(&promptWindow);
	mainWindow->Remove(w);
	mainWindow->SetState(STATE_DEFAULT);
	ResumeGui();
	return save;
}

/****************************************************************************
 * CreditsWindow
 * Display credits, legal copyright and licence
 *
 * THIS MUST NOT BE REMOVED OR DISABLED IN ANY DERIVATIVE WORK
 ***************************************************************************/

static void CreditsWindow()
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

	int numEntries = 17;
	GuiText * txt[numEntries];

	txt[i] = new GuiText("Credits", 30, (GXColor){0, 0, 0, 255});
	txt[i]->SetAlignment(ALIGN_CENTRE, ALIGN_TOP); txt[i]->SetPosition(0,y); i++; y+=32;

	txt[i] = new GuiText("Official Site: http://www.wiimc.org/", 20, (GXColor){0, 0, 0, 255});
	txt[i]->SetAlignment(ALIGN_CENTRE, ALIGN_TOP); txt[i]->SetPosition(0,y); i++; y+=40;

	txt[i]->SetPresets(20, (GXColor){0, 0, 0, 255}, 0,
			FTGX_JUSTIFY_LEFT | FTGX_ALIGN_TOP, ALIGN_LEFT, ALIGN_TOP);

	txt[i] = new GuiText("Tantric");
	txt[i]->SetPosition(50,y); i++;
	txt[i] = new GuiText("Coding & menu design");
	txt[i]->SetPosition(320,y); i++; y+=22;
	txt[i] = new GuiText("rodries");
	txt[i]->SetPosition(50,y); i++;
	txt[i] = new GuiText("Coding");
	txt[i]->SetPosition(320,y); i++; y+=22;
	txt[i] = new GuiText("Sam Mularczyk");
	txt[i]->SetPosition(50,y); i++;
	txt[i] = new GuiText("Menu artwork");
	txt[i]->SetPosition(320,y); i++; y+=22;
	txt[i] = new GuiText("Psyche & drmr");
	txt[i]->SetPosition(50,y); i++;
	txt[i] = new GuiText("Logo");
	txt[i]->SetPosition(320,y); i++; y+=44;

	txt[i] = new GuiText("Thanks also to:");
	txt[i]->SetPosition(50,y); i++; y+=36;

	txt[i] = new GuiText("MPlayer Team");
	txt[i]->SetPosition(50,y); i++; y+=22;

	txt[i] = new GuiText("libogc / devkitPPC");
	txt[i]->SetPosition(50,y); i++;
	txt[i] = new GuiText("shagkur & wintermute");
	txt[i]->SetPosition(320,y); i++; y+=36;

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

	SuspendGui();
	mainWindow->SetState(STATE_DISABLED);
	mainWindow->Append(&creditsWindow);
	mainWindow->ChangeFocus(&creditsWindow);
	ResumeGui();
	
	while(!exit && creditsThreadHalt == 0)
	{
		for(i=0; i < 4; i++)
		{
			if(userInput[i].wpad->btns_d || userInput[i].pad.btns_d)
				exit = true;
		}
		usleep(THREAD_SLEEP);
	}

	SuspendGui();
	mainWindow->Remove(&creditsWindow);
	mainWindow->SetState(STATE_DEFAULT);
	ResumeGui();
	
	for(i=0; i < numEntries; i++)
		delete txt[i];
}

static void *CreditsThread(void *arg)
{
	while(1)
	{
		LWP_SuspendThread (creditsthread);

		if(creditsThreadHalt == 2)
			break;

		CreditsWindow();
		usleep(THREAD_SLEEP);
	}
	return NULL;
}

static void ResumeCreditsThread()
{
	if(creditsthread == LWP_THREAD_NULL || guiShutdown)
		return;

	creditsThreadHalt = 0;
	LWP_ResumeThread(creditsthread);
}

static void DisplayCredits(void * ptr)
{
	if(logoBtn->GetState() != STATE_CLICKED)
		return;

	logoBtn->ResetState();
	ResumeCreditsThread();
}

void UpdateVideobarPauseBtn(bool paused)
{
	if(paused)
	{
		videobarPauseIcon->SetImage(actionbarPlay);
		videobarPauseTip->SetText("Play");
	}
	else
	{
		videobarPauseIcon->SetImage(actionbarPause);
		videobarPauseTip->SetText("Pause");
	}
}

void UpdateAudiobarPauseBtn(bool paused)
{
	if(!audiobarPauseIcon || !audiobarPauseTip)
		return;

	if(paused)
	{
		audiobarPauseIcon->SetImage(actionbarPlay);
		audiobarPauseTip->SetText("Play");
	}
	else
	{
		audiobarPauseIcon->SetImage(actionbarPause);
		audiobarPauseTip->SetText("Pause");
	}
}

static void UpdateAudiobarModeBtn()
{
	switch(WiiSettings.playOrder)
	{
		case PLAY_SINGLE:
			audiobarModeIcon->SetImage(actionbarSingle);
			audiobarModeTip->SetText("Single Play");
			break;
		case PLAY_CONTINUOUS:
			audiobarModeIcon->SetImage(actionbarContinuous);
			audiobarModeTip->SetText("Continuous Play");
			break;
		case PLAY_SHUFFLE:
			audiobarModeIcon->SetImage(actionbarShuffle);
			audiobarModeTip->SetText("Shuffle");
			break;
		case PLAY_LOOP:
			audiobarModeIcon->SetImage(actionbarLoop);
			audiobarModeTip->SetText("Loop");
			break;
	}
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
			browser.dir = &WiiSettings.videosFolder[0]; break;
		case MENU_BROWSE_MUSIC:
			browser.dir = &WiiSettings.musicFolder[0]; break;
		case MENU_BROWSE_ONLINEMEDIA:
			browser.dir = &WiiSettings.onlinemediaFolder[0]; break;
		default:
			return;
	}

	GuiTrigger trigA;
	trigA.SetSimpleTrigger(-1, WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A, PAD_BUTTON_A);
	GuiTrigger trigPlus;
	trigPlus.SetButtonOnlyTrigger(-1, WPAD_BUTTON_PLUS | WPAD_CLASSIC_BUTTON_PLUS, PAD_BUTTON_X);

	GuiFileBrowser fileBrowser(552, 240);
	fileBrowser.SetAlignment(ALIGN_LEFT, ALIGN_TOP);
	fileBrowser.SetPosition(44, 100);

	GuiTooltip playlistBtnTip("Playlist");
	GuiImageData playlistImg(nav_playlist_png);
	GuiImage playlistBtnImg(&playlistImg);
	GuiText playlistBtnTxt(NULL, 18, (GXColor){0, 0, 0, 255});
	playlistBtnTxt.SetPosition(0, 9);
	GuiButton playlistBtn(playlistBtnImg.GetWidth(), playlistBtnImg.GetHeight());
	playlistBtn.SetAlignment(ALIGN_LEFT, ALIGN_BOTTOM);
	playlistBtn.SetPosition(48, -30);
	playlistBtn.SetTooltip(&playlistBtnTip);
	playlistBtn.SetImage(&playlistBtnImg);
	playlistBtn.SetLabel(&playlistBtnTxt);
	playlistBtn.SetTrigger(&trigA);
	playlistBtn.SetSelectable(false);
	playlistBtn.SetEffectGrow();
	playlistBtn.SetAlpha(128);

	GuiButton playlistAddBtn(0, 0);
	playlistAddBtn.SetTrigger(&trigPlus);
	playlistAddBtn.SetSelectable(false);

	char txt[10];
	sprintf(txt, "%d", playlistSize);
	playlistBtnTxt.SetText(txt);

	GuiImageData plus(icon_plus_png);
	GuiImage plusImg(&plus);
	GuiText plusTxt("Add to Playlist", 18, (GXColor){0, 0, 0, 255});
	plusImg.SetAlignment(ALIGN_LEFT, ALIGN_BOTTOM);
	plusImg.SetPosition(49, -110);
	plusTxt.SetAlignment(ALIGN_LEFT, ALIGN_BOTTOM);
	plusTxt.SetPosition(71, -110);

	if(menu == MENU_BROWSE_ONLINEMEDIA && onlinemediaSize == 0)
	{
		ErrorPrompt("Online media file not found.");
		UndoChangeMenu();
		goto done;
	}

	mainWindow->Append(&fileBrowser);
	ResumeGui();

	// populate initial directory listing
	while(BrowserChangeFolder(false) <= 0)
	{
		int choice = WindowPrompt(
		"Error",
		"Unable to display files on selected load device.",
		"Retry",
		"Cancel");

		if(choice == 0)
		{
			UndoChangeMenu();
			goto done;
		}
	}

	SuspendGui();

	if(menu == MENU_BROWSE_MUSIC || menu == MENU_BROWSE_ONLINEMEDIA)
		mainWindow->Append(audiobar);

	if(menu == MENU_BROWSE_MUSIC) // add playlist functionality
	{
		mainWindow->Append(&plusImg);
		mainWindow->Append(&plusTxt);
		mainWindow->Append(&playlistAddBtn);
		mainWindow->Append(&playlistBtn);
		UpdateAudiobarModeBtn();
	}
	ResumeGui();

	while(menuCurrent == menu && !guiShutdown)
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
						if(menuCurrent == MENU_BROWSE_ONLINEMEDIA)
							sprintf(browserPlaylist, "%s", browserList[browser.selIndex].filename);
						else
							sprintf(browserPlaylist, "%s%s", browser.dir, browserList[browser.selIndex].filename);
						
						int numItems = BrowserChangeFolder();

						if(numItems <= 1)
						{
							browserPlaylist[0] = 0;
							ErrorPrompt("Error loading playlist!");
							inPlaylist = false;
							BrowserChangeFolder(false);
							continue;
						}
						else if(numItems == 2) // let's load this one file
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
						if(inPlaylist || browserPlaylist[0] != 0 || menuCurrent == MENU_BROWSE_ONLINEMEDIA)
							sprintf(loadedFile, "%s", browserList[browser.selIndex].filename);
						else
							sprintf(loadedFile, "%s%s", browser.dir, browserList[browser.selIndex].filename);
					}

					ShutdownMPlayer();
					ShowAction("Loading...");
					LoadMPlayer(); // signal MPlayer to load

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
						UpdateAudiobarPauseBtn(false);

						// we loaded an audio file - if we already had a video
						// loaded, we should remove the bg
						mainWindow->Remove(videoImg);

						// update the audio bar
						if(wiiGetTimeLength() <= 1) // this is a stream - hide some elements
						{
							audiobar->Remove(audiobarProgressBtn);
							audiobar->Remove(audiobarProgressLeftImg);
							audiobar->Remove(audiobarProgressMidImg);
							audiobar->Remove(audiobarProgressRightImg);
							audiobar->Remove(audiobarSkipBackwardBtn);
							audiobar->Remove(audiobarSkipForwardBtn);
							audiobar->Remove(audiobarModeBtn);
						}
						else
						{
							audiobar->Append(audiobarProgressBtn);
							audiobar->Append(audiobarProgressLeftImg);
							audiobar->Append(audiobarProgressMidImg);
							audiobar->Append(audiobarProgressRightImg);
							audiobar->Append(audiobarSkipBackwardBtn);
							audiobar->Append(audiobarSkipForwardBtn);
							audiobar->Append(audiobarModeBtn);
						}
					}
				}
			}
		}

		// updating audio bar elements is not required for Videos area
		if(menu == MENU_BROWSE_VIDEOS)
			continue;

		if(playlistAddBtn.GetState() == STATE_CLICKED)
		{
			playlistAddBtn.ResetState();
			int addIndex = browser.selIndex;

			if(fileBrowser.IsFocused() && addIndex > 0)
			{
				if(browserList[addIndex].icon == ICON_FILE_CHECKED || 
					browserList[addIndex].icon == ICON_FOLDER_CHECKED)
					MusicPlaylistDequeue(addIndex);
				else
					MusicPlaylistEnqueue(addIndex);
	
				char txt[10];
				sprintf(txt, "%d", playlistSize);
				playlistBtnTxt.SetText(txt);
				fileBrowser.TriggerUpdate();
			}
		}
		if(playlistBtn.GetState() == STATE_CLICKED)
		{
			playlistBtn.ResetState();
			inPlaylist = MusicPlaylistLoad();
		}

		if(playlistSize > 0)
		{
			if(playlistBtn.GetAlpha() == 128)
				playlistBtn.SetAlpha(255);
			if(playlistSize == 1)
			{
				if(audiobarSkipForwardBtn->GetState() != STATE_DISABLED)
				{
					audiobarSkipForwardBtn->SetState(STATE_DISABLED);
					audiobarSkipForwardBtn->SetAlpha(128);
				}
			}
			else if(audiobarSkipForwardBtn->GetState() == STATE_DISABLED && playingAudio)
			{
				audiobarSkipForwardBtn->SetState(STATE_DEFAULT);
				audiobarSkipForwardBtn->SetAlpha(255);
			}
			if(audiobarPauseBtn->GetState() == STATE_DISABLED)
			{
				audiobarPauseBtn->SetState(STATE_DEFAULT);
				audiobarPauseBtn->SetAlpha(255);
			}
		}
		else if(playlistSize == 0)
		{
			if(playlistBtn.GetAlpha() == 255)
				playlistBtn.SetAlpha(128);
			if(audiobarSkipForwardBtn->GetState() != STATE_DISABLED)
			{
				audiobarSkipForwardBtn->SetState(STATE_DISABLED);
				audiobarSkipForwardBtn->SetAlpha(128);
			}
			if(audiobarPauseBtn->GetState() != STATE_DISABLED && !playingAudio)
			{
				audiobarPauseBtn->SetState(STATE_DISABLED);
				audiobarPauseBtn->SetAlpha(128);
			}
		}

		if(playingAudio)
		{
			if(audiobarProgressBtn->GetState() == STATE_DISABLED && wiiGetTimeLength() > 0)
			{
				audiobarProgressBtn->SetState(STATE_DEFAULT);
				audiobarProgressBtn->SetAlpha(255);
				audiobarSkipBackwardBtn->SetState(STATE_DEFAULT);
				audiobarSkipBackwardBtn->SetAlpha(255);

				for(int i=0; i < 4; i++)
					audiobarNowPlaying[i]->SetVisible(true);
			}
			if(audiobarPauseBtn->GetState() == STATE_DISABLED)
			{
				audiobarPauseBtn->SetState(STATE_DEFAULT);
				audiobarPauseBtn->SetAlpha(255);
			}
		}
		else
		{
			if(audiobarProgressBtn->GetState() != STATE_DISABLED)
			{
				audiobarProgressBtn->SetState(STATE_DISABLED);
				audiobarProgressBtn->SetAlpha(128);
				audiobarSkipBackwardBtn->SetState(STATE_DISABLED);
				audiobarSkipBackwardBtn->SetAlpha(128);
				for(int i=0; i < 4; i++)
				{
					audiobarNowPlaying[i]->SetVisible(false);
					if(i > 0) audiobarNowPlaying[i]->SetText(NULL);
				}
			}
		}
	}
done:
	SuspendParseThread(); // halt parsing
	SuspendGui();
	mainWindow->Remove(&fileBrowser);

	if(menu == MENU_BROWSE_MUSIC || menu == MENU_BROWSE_ONLINEMEDIA)
		mainWindow->Remove(audiobar);

	if(menu == MENU_BROWSE_MUSIC) // remove playlist functionality
	{
		mainWindow->Remove(&plusImg);
		mainWindow->Remove(&plusTxt);
		mainWindow->Remove(&playlistAddBtn);
		mainWindow->Remove(&playlistBtn);
	}
}

// Picture Viewer
#define MAX_PICTURE_SIZE (1024*1024*10) // 10 MB
static int loadPictures = 0; // reload pictures

#define NUM_PICTURES 		7 // 1 image with a buffer of +/- 3 on each side
#define PIC_WIDTH			240
#define PIC_HEIGHT			180

static GuiImage *pictureImg = NULL;
static GuiButton *pictureBtn = NULL;
static GuiImageData *picture[NUM_PICTURES] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL};
static int pictureIndex[NUM_PICTURES] = {-1, -1, -1, -1, -1, -1, -1};
static int pictureIndexLoaded = -1;
static int pictureIndexLoading = -1;
static int pictureLoaded = -1;

static int closePictureViewer = 1; // 0 = picture viewer is open
static bool setPicture = false;
static int slideshow = 0; // slideshow mode
static u64 slideprev, slidenow; // slideshow timer

static int FoundPicture(int p)
{
	if(p <= 0)
		return -1;

	for(int i=0; i < NUM_PICTURES; i++)
		if(pictureIndex[i] == p)
			return i;
	return -1;
}

static void SetPicture(int picIndex, int browserIndex)
{
	if(picIndex >= 0)
	{
		pictureLoaded = picIndex;
		pictureIndexLoaded = browserIndex;
		SuspendGui();
		pictureImg->SetImage(picture[picIndex]);
		pictureImg->SetScale(PIC_WIDTH, PIC_HEIGHT);
		pictureImg->SetEffect(EFFECT_ROTATE, 0);
		pictureBtn->SetState(STATE_DEFAULT);
		ResumeGui();
	}
	else
	{
		if(browserList[browser.selIndex].isdir)
		{
			if(pictureImg->GetImage() != NULL)
			{
				SuspendGui();
				pictureImg->SetImage(NULL);
				pictureImg->SetEffect(EFFECT_ROTATE, 0);
				ResumeGui();
			}
		}
		else
		{
			SuspendGui();
			pictureImg->SetScale(1);
			pictureImg->SetImage(&throbber);
			pictureImg->SetEffect(EFFECT_ROTATE, 100);
			ResumeGui();
		}
		pictureBtn->SetState(STATE_DISABLED);
		pictureLoaded = -1;
		pictureIndexLoaded = -1;
	}
}

static void CleanupPictures(int selIndex)
{
	// free any unused picture data
	for(int i=0; i < NUM_PICTURES; i++)
	{
		if(pictureIndex[i] == -1 || i == pictureLoaded)
			continue;

		if(selIndex == -1 || pictureIndex[i] < selIndex-3 || pictureIndex[i] > selIndex+3)
		{
			if(picture[i]) delete picture[i];
			picture[i] = NULL;
			pictureIndex[i] = -1;
		}
	}
}

static void *PictureThread (void *arg)
{
	int selIndex;
	int i,next;
	char filepath[1024];
	u8 * picBuffer = (u8 *)memalign(32, MAX_PICTURE_SIZE);

	if(!picBuffer)
	{
		while(pictureThreadHalt == 0) usleep(THREAD_SLEEP);
		return NULL;
	}

	pictureLoaded = -1;
	pictureIndexLoaded = -1;
	pictureIndexLoading = -1;

	while(1)
	{
restart:
		if(pictureThreadHalt == 1)
		{
			pictureLoaded = -1;
			pictureIndexLoaded = -1;
			pictureIndexLoading = -1;
			LWP_SuspendThread(picturethread);
		}
		if(pictureThreadHalt == 2)
			break;

		if(loadPictures)
		{
			loadPictures = 0;
			selIndex = browser.selIndex;
			CleanupPictures(selIndex);

			// load missing pictures - starting with selected index
			if(selIndex > 0 
				&& !browserList[selIndex].isdir
				&& pictureIndexLoaded != selIndex
				&& browserList[selIndex].length < MAX_PICTURE_SIZE)
			{
				int found = FoundPicture(selIndex);
				if(found < 0)
				{
					sprintf(filepath, "%s%s", browser.dir, browserList[selIndex].filename);
					pictureIndexLoading = selIndex;
					int size = LoadFile((char *)picBuffer, filepath, SILENT);

					if(size == 0)
						goto restart;

					// find first empty slot
					for(i=0; i < NUM_PICTURES; i++)
						if(pictureIndex[i] == -1)
							break;
					
					picture[i] = new GuiImageData(picBuffer, size);
					pictureIndex[i] = selIndex;
					found = i;
				}

				pictureIndexLoading = -1;
				setPicture = true; // trigger picture to be reloaded

				if(found < 0)
					goto restart;
			}

			// fill up image buffer slots
			next = selIndex-3;

			if(next <= 0)
				next = 1;

			for(i=0; i < NUM_PICTURES; i++)
			{
				if(pictureIndex[i] > 0)
					continue;

				while(next < browser.numEntries && 
					(browserList[next].isdir
					|| next == selIndex 
					|| browserList[next].length > MAX_PICTURE_SIZE
					|| FoundPicture(next) >= 0))
					next++;

				if(next >= browser.numEntries || next > selIndex+3)
					break;

				sprintf(filepath, "%s%s", browser.dir, browserList[next].filename);
				pictureIndexLoading = next;
				int size = LoadFile((char *)picBuffer, filepath, SILENT);

				if(size == 0)
					goto restart;

				picture[i] = new GuiImageData(picBuffer, size);
				pictureIndex[i] = next;
				pictureIndexLoading = -1;
				if(browser.selIndex == next)
					setPicture = true; // trigger picture to be reloaded
				next++;
			}
		}
		usleep(THREAD_SLEEP);
	}

	// reset everything
	SuspendGui();

	for(i=0; i < NUM_PICTURES; i++)
	{
		if(picture[i]) delete picture[i];
		picture[i] = NULL;
		pictureIndex[i] = -1;
	}
	ResumeGui();
	free(picBuffer);
	return NULL;
}

/****************************************************************************
 * ResumePictureThread
 *
 * Signals the picture thread to start, and resumes the thread.
 ***************************************************************************/
static void ResumePictureThread()
{
	if(picturethread == LWP_THREAD_NULL || guiShutdown)
		return;

	pictureThreadHalt = 0;
	LWP_ResumeThread(picturethread);
}

/****************************************************************************
 * SuspendPictureThread
 *
 * Signals the picture thread to stop.
 ***************************************************************************/
void SuspendPictureThread()
{
	if(picturethread == LWP_THREAD_NULL)
		return;

	pictureThreadHalt = 1;
	CancelFileOp();

	// wait for thread to finish
	while(!LWP_ThreadIsSuspended(picturethread))
		usleep(THREAD_SLEEP);
}

static void ChangePicture(int dir)
{
	int newIndex = browser.selIndex;

	while(1)
	{
		usleep(THREAD_SLEEP);
		newIndex += dir;

		if(newIndex >= browser.numEntries)
			newIndex = 1;
		else if(newIndex < 1)
			newIndex = browser.numEntries-1;

		if(browserList[newIndex].isdir)
			continue;

		if(newIndex == browser.selIndex)
			return; // we have wrapped around to the same image - do nothing

		if(browserList[newIndex].length <= MAX_PICTURE_SIZE)
			break; // found a picture we can display
	}
	browser.selIndex = newIndex;
	loadPictures = 1;
}

static void ToggleSlideshow()
{
	if(slideshow == 0)
		slideprev = gettime(); // setup timer

	slideshow ^= 1;
}

static void PictureViewer()
{
	int currentIndex = -1;
	closePictureViewer = 0;
	slideshow = 0;
	GuiWindow * oldWindow = mainWindow;
	GuiImage * pictureFullImg = new GuiImage;
	pictureFullImg->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	
	GuiWindow * w = new GuiWindow(screenwidth, screenheight);
	w->Append(pictureFullImg);
	w->Append(picturebar);

	SuspendGui();
	mainWindow = w;
	ResumeGui();

	while(closePictureViewer == 0 && !guiShutdown)
	{
		if(browser.selIndex != currentIndex)
		{
			currentIndex = browser.selIndex;
			loadPictures = 1; // trigger picture thread

			// search through already loaded pictures for this picture
			int found = FoundPicture(browser.selIndex);
			if(found >= 0)
				setPicture = true;
			else if(!browserList[browser.selIndex].isdir
				&& pictureIndexLoading != browser.selIndex)
				CancelFileOp();

			if(!slideshow && !setPicture)
			{
				ShowAction("Loading...");
				while(!setPicture) // wait for picture to load
					usleep(THREAD_SLEEP);

				CancelAction();
			}
		}

		if(setPicture)
		{
			setPicture = false;

			int found = FoundPicture(browser.selIndex);
			if(found >= 0)
			{
				SuspendGui();
				pictureFullImg->SetImage(picture[found]);
				ResumeGui();
			}
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

	SuspendGui();
	mainWindow = oldWindow;
	ResumeGui();
	delete w;
	delete pictureFullImg;
}

static void MenuBrowsePictures()
{
	int currentIndex = -1;
	ShutoffRumble();
	browser.dir = &WiiSettings.picturesFolder[0];

	GuiFileBrowser fileBrowser(300, 240);
	fileBrowser.SetAlignment(ALIGN_LEFT, ALIGN_TOP);
	fileBrowser.SetPosition(44, 100);

	SuspendGui();
	mainWindow->Append(&fileBrowser);
	ResumeGui();

	// populate initial directory listing
	while(BrowserChangeFolder(false, true) <= 0)
	{
		int choice = WindowPrompt(
		"Error",
		"Unable to display files on selected load device.",
		"Retry",
		"Cancel");

		if(choice == 0)
		{
			UndoChangeMenu();
			goto done;
		}
	}

	SetPicture(-1, -1);
	SuspendGui();
	mainWindow->Append(pictureBtn);
	ResumeGui();

	// start picture thread
	loadPictures = 1; // trigger picture thread
	ResumePictureThread();

	while(menuCurrent == MENU_BROWSE_PICTURES && !guiShutdown)
	{
		usleep(THREAD_SLEEP);

		// devices were inserted or removed - update the filebrowser!
		if(devicesChanged)
		{
			devicesChanged = false;

			if(BrowserChangeFolder(true, true))
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

		// update displayed picture
		if(browser.selIndex != currentIndex || setPicture)
		{
			currentIndex = browser.selIndex;
			setPicture = false;

			if(browserList[browser.selIndex].isdir)
			{
				SetPicture(-1, -1); // set picture to blank
			}
			else
			{
				// search through already loaded pictures for this picture
				int found = FoundPicture(browser.selIndex);
				if(found >= 0)
				{
					SetPicture(found, browser.selIndex);
				}
				else
				{
					SetPicture(-1, -1); // set picture to blank
					if(!browserList[browser.selIndex].isdir && 
						pictureIndexLoading != browser.selIndex)
					{
						CancelFileOp();
					}
				}
				loadPictures = 1; // trigger picture thread
			}
		}

		// update file browser based on arrow buttons
		for(int i=0; i<FILE_PAGESIZE; i++)
		{
			if(fileBrowser.fileList[i]->GetState() == STATE_CLICKED)
			{
				fileBrowser.fileList[i]->ResetState();

				if(browserList[browser.selIndex].isdir)
				{
					SuspendPictureThread();
					SetPicture(-1, -1); // set picture to blank
					CleanupPictures(-1);

					if(BrowserChangeFolder())
					{
						fileBrowser.ResetState();
						fileBrowser.fileList[0]->SetState(STATE_SELECTED);
						fileBrowser.TriggerUpdate();
						loadPictures = 1; // trigger picture thread
						ResumePictureThread();
					}
					else
					{
						goto done;
					}
				}
				else
				{
					PictureViewer();
				}
			}
		}
		if(pictureBtn->GetState() == STATE_CLICKED)
		{
			pictureBtn->ResetState();
			PictureViewer();
		}
	}
done:
	SuspendPictureThread(); // halt picture thread
	SuspendParseThread(); // halt parsing
	SuspendGui();
	mainWindow->Remove(pictureBtn);
	mainWindow->Remove(&fileBrowser);
}

static void MenuDVD()
{
	ResumeGui();

	if(!ChangeInterface(DEVICE_DVD, -1, NOTSILENT))
	{
		UndoChangeMenu(); // go back to last menu
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
	UndoChangeMenu(); // go back to last menu
	
	if(!guiShutdown) // load failed
		ErrorPrompt("Unrecognized DVD format!");

	SuspendGui();
}

static void MenuSettingsGlobal()
{
	int ret;
	int i = 0;
	bool firstRun = true;
	OptionList options;
	
	sprintf(options.name[i++], "File Extensions");
	sprintf(options.name[i++], "Language");
	sprintf(options.name[i++], "Volume");
	sprintf(options.name[i++], "Exit Action");
	sprintf(options.name[i++], "Wiimote Rumble");

	options.length = i;
		
	for(i=0; i < options.length; i++)
	{
		options.value[i][0] = 0;
		options.icon[i] = 0;
	}

	GuiText titleTxt("Settings - Global", 26, (GXColor){255, 255, 255, 255});
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

	SuspendGui();
	GuiWindow w(screenwidth, screenheight);
	w.Append(&backBtn);
	mainWindow->Append(&optionBrowser);
	mainWindow->Append(&w);
	mainWindow->Append(&titleTxt);
	ResumeGui();

	while(menuCurrent == MENU_SETTINGS_GLOBAL && !guiShutdown)
	{
		usleep(THREAD_SLEEP);

		ret = optionBrowser.GetClickedOption();

		switch (ret)
		{
			case 0:
				WiiSettings.hideExtensions ^= 1;
				break;
			case 1:
				//WiiSettings.language++;
				//if(WiiSettings.language > LANG_KOREAN)
				//	WiiSettings.language = 0;
				break;
			case 2:
				WiiSettings.volume += 10;
				if(WiiSettings.volume > 100)
					WiiSettings.volume = 0;
				break;
			case 3:
				WiiSettings.exitAction++;
				if(WiiSettings.exitAction > EXIT_LOADER)
					WiiSettings.exitAction = 0;
				break;
			case 4:
				WiiSettings.rumble ^= 1;
				break;
		}

		if(ret >= 0 || firstRun)
		{
			firstRun = false;

			sprintf(options.value[0], "%s", WiiSettings.hideExtensions ? "Hide" : "Show");

			switch(WiiSettings.language)
			{
				case LANG_JAPANESE:		sprintf(options.value[1], "Japanese"); break;
				case LANG_ENGLISH:		sprintf(options.value[1], "English"); break;
				case LANG_GERMAN:		sprintf(options.value[1], "German"); break;
				case LANG_FRENCH:		sprintf(options.value[1], "French"); break;
				case LANG_SPANISH:		sprintf(options.value[1], "Spanish"); break;
				case LANG_ITALIAN:		sprintf(options.value[1], "Italian"); break;
				case LANG_DUTCH:		sprintf(options.value[1], "Dutch"); break;
				case LANG_SIMP_CHINESE:	sprintf(options.value[1], "Chinese (Simplified)"); break;
				case LANG_TRAD_CHINESE:	sprintf(options.value[1], "Chinese (Traditional)"); break;
				case LANG_KOREAN:		sprintf(options.value[1], "Korean"); break;
			}
			
			sprintf (options.value[2], "%d%%", WiiSettings.volume);

			switch(WiiSettings.exitAction)
			{
				case EXIT_AUTO:		sprintf(options.value[3], "Auto"); break;
				case EXIT_WIIMENU:	sprintf(options.value[3], "Return to Wii Menu"); break;
				case EXIT_POWEROFF:	sprintf(options.value[3], "Power Off Wii"); break;
				case EXIT_LOADER:	sprintf(options.value[3], "Return to Loader"); break;
			}

			sprintf(options.value[4], "%s", WiiSettings.rumble ? "On" : "Off");

			optionBrowser.TriggerUpdate();
		}

		if(backBtn.GetState() == STATE_CLICKED)
		{
			menuCurrent = MENU_SETTINGS;
		}
	}
	SuspendGui();
	mainWindow->Remove(&optionBrowser);
	mainWindow->Remove(&w);
	mainWindow->Remove(&titleTxt);
	ResetText();
}

static void ScreenZoomWindowUpdate(void * ptr, float h, float v)
{
	GuiButton * b = (GuiButton *)ptr;
	if(b->GetState() == STATE_CLICKED)
	{
		WiiSettings.videoZoomHor += h;
		WiiSettings.videoZoomVert += v;

		char zoom[10];
		sprintf(zoom, "%.2f%%", WiiSettings.videoZoomHor*100);
		settingText->SetText(zoom);
		sprintf(zoom, "%.2f%%", WiiSettings.videoZoomVert*100);
		settingText2->SetText(zoom);
		b->ResetState();
	}
}

static void ScreenZoomWindowLeftClick(void * ptr) { ScreenZoomWindowUpdate(ptr, -0.01, 0); }
static void ScreenZoomWindowRightClick(void * ptr) { ScreenZoomWindowUpdate(ptr, +0.01, 0); }
static void ScreenZoomWindowUpClick(void * ptr) { ScreenZoomWindowUpdate(ptr, 0, +0.01); }
static void ScreenZoomWindowDownClick(void * ptr) { ScreenZoomWindowUpdate(ptr, 0, -0.01); }

static void ScreenZoomWindow()
{
	GuiWindow * w = new GuiWindow(200,200);
	w->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);

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
	arrowLeftBtn.SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	arrowLeftBtn.SetPosition(50, 0);
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
	arrowRightBtn.SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	arrowRightBtn.SetPosition(164, 0);
	arrowRightBtn.SetTrigger(0, &trigA);
	arrowRightBtn.SetTrigger(1, &trigRight);
	arrowRightBtn.SetSelectable(false);
	arrowRightBtn.SetUpdateCallback(ScreenZoomWindowRightClick);

	GuiImageData arrowUp(button_arrow_up_png);
	GuiImage arrowUpImg(&arrowUp);
	GuiImageData arrowUpOver(button_arrow_up_over_png);
	GuiImage arrowUpOverImg(&arrowUpOver);
	GuiButton arrowUpBtn(arrowUp.GetWidth(), arrowUp.GetHeight());
	arrowUpBtn.SetImage(&arrowUpImg);
	arrowUpBtn.SetImageOver(&arrowUpOverImg);
	arrowUpBtn.SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	arrowUpBtn.SetPosition(-76, -27);
	arrowUpBtn.SetTrigger(0, &trigA);
	arrowUpBtn.SetTrigger(1, &trigUp);
	arrowUpBtn.SetSelectable(false);
	arrowUpBtn.SetUpdateCallback(ScreenZoomWindowUpClick);

	GuiImageData arrowDown(button_arrow_down_png);
	GuiImage arrowDownImg(&arrowDown);
	GuiImageData arrowDownOver(button_arrow_down_over_png);
	GuiImage arrowDownOverImg(&arrowDownOver);
	GuiButton arrowDownBtn(arrowDown.GetWidth(), arrowDown.GetHeight());
	arrowDownBtn.SetImage(&arrowDownImg);
	arrowDownBtn.SetImageOver(&arrowDownOverImg);
	arrowDownBtn.SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	arrowDownBtn.SetPosition(-76, 27);
	arrowDownBtn.SetTrigger(0, &trigA);
	arrowDownBtn.SetTrigger(1, &trigDown);
	arrowDownBtn.SetSelectable(false);
	arrowDownBtn.SetUpdateCallback(ScreenZoomWindowDownClick);

	GuiImageData screenPosition(screen_position_png);
	GuiImage screenPositionImg(&screenPosition);
	screenPositionImg.SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	screenPositionImg.SetPosition(0, 0);

	settingText = new GuiText(NULL, 22, (GXColor){0, 0, 0, 255});
	settingText2 = new GuiText(NULL, 22, (GXColor){0, 0, 0, 255});
	char zoom[10];
	sprintf(zoom, "%.2f%%", WiiSettings.videoZoomHor*100);
	settingText->SetText(zoom);
	settingText->SetPosition(108, 0);
	sprintf(zoom, "%.2f%%", WiiSettings.videoZoomVert*100);
	settingText2->SetText(zoom);
	settingText2->SetPosition(-76, 0);

	float currentZoomHor = WiiSettings.videoZoomHor;
	float currentZoomVert = WiiSettings.videoZoomVert;

	w->Append(&arrowLeftBtn);
	w->Append(&arrowRightBtn);
	w->Append(&arrowUpBtn);
	w->Append(&arrowDownBtn);
	w->Append(&screenPositionImg);
	w->Append(settingText);
	w->Append(settingText2);

	if(!SettingWindow("Screen Zoom",w))
	{
		// undo changes
		WiiSettings.videoZoomHor = currentZoomHor;
		WiiSettings.videoZoomVert = currentZoomVert;
	}

	delete(w);
	delete(settingText);
	delete(settingText2);
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

static void MenuSettingsVideos()
{
	int ret;
	int i = 0;
	bool firstRun = true;
	OptionList options;
	
	sprintf(options.name[i++], "Screen Zoom");
	sprintf(options.name[i++], "Screen Position");
	sprintf(options.name[i++], "Frame Dropping");
	sprintf(options.name[i++], "Aspect Ratio");
	sprintf(options.name[i++], "Cache Size");
	sprintf(options.name[i++], "Cache Prefill");
	sprintf(options.name[i++], "Audio Delay");
	sprintf(options.name[i++], "Auto-Resume");
	sprintf(options.name[i++], "Videos Files Folder");

	options.length = i;

	for(i=0; i < options.length; i++)
	{
		options.value[i][0] = 0;
		options.icon[i] = 0;
	}

	GuiText titleTxt("Settings - Videos", 26, (GXColor){255, 255, 255, 255});
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

	SuspendGui();
	GuiWindow w(screenwidth, screenheight);
	w.Append(&backBtn);
	mainWindow->Append(&optionBrowser);
	mainWindow->Append(&w);
	mainWindow->Append(&titleTxt);
	ResumeGui();

	while(menuCurrent == MENU_SETTINGS_VIDEOS && !guiShutdown)
	{
		usleep(THREAD_SLEEP);

		ret = optionBrowser.GetClickedOption();

		switch (ret)
		{
			case 0:
				ScreenZoomWindow();
				break;
			case 1:
				ScreenPositionWindow();
				break;
			case 2:
				WiiSettings.frameDropping++;
				if (WiiSettings.frameDropping > 2)
					WiiSettings.frameDropping = 0;
				break;
			case 3:
				if(WiiSettings.aspectRatio > 2.34)
					WiiSettings.aspectRatio = -1;
				else if(WiiSettings.aspectRatio > 1.84)
					WiiSettings.aspectRatio = 2.35;
				else if(WiiSettings.aspectRatio > 1.77)
					WiiSettings.aspectRatio = 1.85;
				else if(WiiSettings.aspectRatio > 1.32)
					WiiSettings.aspectRatio = 1.7778;
				else if(WiiSettings.aspectRatio > -1.01)
					WiiSettings.aspectRatio = 1.3333;
				else
					WiiSettings.aspectRatio = -1;
				break;
			case 4:
				WiiSettings.cacheSize += 2048;
				if(WiiSettings.cacheSize > 16384)
					WiiSettings.cacheSize = 2048;
				break;
			case 5:
				WiiSettings.cachePrefill += 10;
				if (WiiSettings.cachePrefill > 100)
					WiiSettings.cachePrefill = 10;
				break;
			case 6:
				WiiSettings.audioDelay += 0.1;
				if (WiiSettings.audioDelay > 2)
					WiiSettings.audioDelay = 0;
				break;
			case 7:
				WiiSettings.autoResume ^= 1;
				break;
			case 8:
				OnScreenKeyboard(WiiSettings.videosFolder, MAXPATHLEN);
				break;
		}

		if(ret >= 0 || firstRun)
		{
			firstRun = false;
			sprintf (options.value[0], "%.2f%%, %.2f%%", WiiSettings.videoZoomHor*100, WiiSettings.videoZoomVert*100);
			sprintf (options.value[1], "%d, %d", WiiSettings.videoXshift, WiiSettings.videoYshift);

			switch(WiiSettings.frameDropping)
			{
				case FRAMEDROPPING_AUTO:
					sprintf (options.value[2], "Auto"); break;
				case FRAMEDROPPING_ALWAYS:
					sprintf (options.value[2], "Always"); break;
				case FRAMEDROPPING_DISABLED:
					sprintf (options.value[2], "Disabled"); break;
			}

			if(WiiSettings.aspectRatio > 2.34)
				sprintf (options.value[3], "2.35:1");
			else if(WiiSettings.aspectRatio > 1.84)
				sprintf (options.value[3], "1.85:1");
			else if(WiiSettings.aspectRatio > 1.77)
				sprintf (options.value[3], "16:9");
			else if(WiiSettings.aspectRatio > 1.32)
				sprintf (options.value[3], "4:3");
			else
				sprintf (options.value[3], "Auto");

			sprintf (options.value[4], "%d MB", WiiSettings.cacheSize/1024);
			sprintf (options.value[5], "%d%%", WiiSettings.cachePrefill);
			sprintf (options.value[6], "%.1f sec", WiiSettings.audioDelay);
			sprintf (options.value[7], "%s", WiiSettings.autoResume ? "On" : "Off");
			snprintf(options.value[8], 20, "%s", WiiSettings.videosFolder);

			optionBrowser.TriggerUpdate();
		}

		if(backBtn.GetState() == STATE_CLICKED)
		{
			menuCurrent = MENU_SETTINGS;
		}
	}
	SuspendGui();
	mainWindow->Remove(&optionBrowser);
	mainWindow->Remove(&w);
	mainWindow->Remove(&titleTxt);
}

static void MenuSettingsMusic()
{
	int ret;
	int i = 0;
	bool firstRun = true;
	OptionList options;

	sprintf(options.name[i++], "Play Order");
	sprintf(options.name[i++], "Music Files Folder");

	options.length = i;
		
	for(i=0; i < options.length; i++)
	{
		options.value[i][0] = 0;
		options.icon[i] = 0;
	}

	GuiText titleTxt("Settings - Music", 26, (GXColor){255, 255, 255, 255});
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

	SuspendGui();
	GuiWindow w(screenwidth, screenheight);
	w.Append(&backBtn);
	mainWindow->Append(&optionBrowser);
	mainWindow->Append(&w);
	mainWindow->Append(&titleTxt);
	ResumeGui();

	while(menuCurrent == MENU_SETTINGS_MUSIC && !guiShutdown)
	{
		usleep(THREAD_SLEEP);

		ret = optionBrowser.GetClickedOption();

		switch (ret)
		{
			case 0:
				WiiSettings.playOrder++;
				if(WiiSettings.playOrder > PLAY_LOOP)
					WiiSettings.playOrder = 0;
				break;
			case 1:
				OnScreenKeyboard(WiiSettings.musicFolder, MAXPATHLEN);
				break;
		}

		if(ret >= 0 || firstRun)
		{
			firstRun = false;
			

			switch(WiiSettings.playOrder)
			{
				case PLAY_SINGLE:		sprintf(options.value[0], "Single"); break;
				case PLAY_CONTINUOUS:	sprintf(options.value[0], "Continuous"); break;
				case PLAY_SHUFFLE:		sprintf(options.value[0], "Shuffle"); break;
				case PLAY_LOOP:			sprintf(options.value[0], "Loop"); break;
			}
			snprintf(options.value[1], 20, "%s", WiiSettings.musicFolder);

			optionBrowser.TriggerUpdate();
		}

		if(backBtn.GetState() == STATE_CLICKED)
		{
			menuCurrent = MENU_SETTINGS;
		}
	}
	SuspendGui();
	mainWindow->Remove(&optionBrowser);
	mainWindow->Remove(&w);
	mainWindow->Remove(&titleTxt);
}

static void MenuSettingsPictures()
{
	int ret;
	int i = 0;
	bool firstRun = true;
	OptionList options;

	sprintf(options.name[i++], "Pictures Files Folder");

	options.length = i;
		
	for(i=0; i < options.length; i++)
	{
		options.value[i][0] = 0;
		options.icon[i] = 0;
	}

	GuiText titleTxt("Settings - Pictures", 26, (GXColor){255, 255, 255, 255});
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

	SuspendGui();
	GuiWindow w(screenwidth, screenheight);
	w.Append(&backBtn);
	mainWindow->Append(&optionBrowser);
	mainWindow->Append(&w);
	mainWindow->Append(&titleTxt);
	ResumeGui();

	while(menuCurrent == MENU_SETTINGS_PICTURES && !guiShutdown)
	{
		usleep(THREAD_SLEEP);

		ret = optionBrowser.GetClickedOption();

		switch (ret)
		{
			case 0:
				OnScreenKeyboard(WiiSettings.picturesFolder, MAXPATHLEN);
				break;
		}

		if(ret >= 0 || firstRun)
		{
			firstRun = false;

			snprintf(options.value[0], 20, "%s", WiiSettings.picturesFolder);
			optionBrowser.TriggerUpdate();
		}

		if(backBtn.GetState() == STATE_CLICKED)
		{
			menuCurrent = MENU_SETTINGS;
		}
	}
	SuspendGui();
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

	SuspendGui();
	GuiWindow w(screenwidth, screenheight);
	w.Append(&backBtn);
	w.Append(&addsmbBtn);
	w.Append(&addftpBtn);
	mainWindow->Append(&optionBrowser);
	mainWindow->Append(&w);
	mainWindow->Append(&titleTxt);
	ResumeGui();

	while(menuCurrent == MENU_SETTINGS_NETWORK && !guiShutdown)
	{
		usleep(THREAD_SLEEP);

		ret = optionBrowser.GetClickedOption();

		if((ret >= 0 && ret < 5) || addsmbBtn.GetState() == STATE_CLICKED)
		{
			netEditIndex = ret;
			menuCurrent = MENU_SETTINGS_NETWORK_SMB;
		}
		else if(ret >= 5 || addftpBtn.GetState() == STATE_CLICKED)
		{
			netEditIndex = ret-5;
			menuCurrent = MENU_SETTINGS_NETWORK_FTP;
		}

		if(backBtn.GetState() == STATE_CLICKED)
		{
			menuCurrent = MENU_SETTINGS;
		}
	}
	SuspendGui();
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

	SuspendGui();
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

	while(menuCurrent == MENU_SETTINGS_NETWORK_SMB && !guiShutdown)
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
			menuCurrent = MENU_SETTINGS_NETWORK;
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
				menuCurrent = MENU_SETTINGS_NETWORK;
			}
		}
	}
	SuspendGui();
	mainWindow->Remove(&optionBrowser);
	mainWindow->Remove(&w);
	mainWindow->Remove(&titleTxt);
	CloseShare(netEditIndex);
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

	SuspendGui();
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

	while(menuCurrent == MENU_SETTINGS_NETWORK_FTP && !guiShutdown)
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
			menuCurrent = MENU_SETTINGS_NETWORK;
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
				menuCurrent = MENU_SETTINGS_NETWORK;
			}
		}
	}
	SuspendGui();
	mainWindow->Remove(&optionBrowser);
	mainWindow->Remove(&w);
	mainWindow->Remove(&titleTxt);
	CloseFTP(netEditIndex);
}

static void MenuSettingsSubtitles()
{
	int ret;
	int i = 0;
	bool firstRun = true;
	OptionList options;

	sprintf(options.name[i++], "Visibility");
	sprintf(options.name[i++], "Position");
	sprintf(options.name[i++], "Size");
	sprintf(options.name[i++], "Delay");

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

	SuspendGui();
	GuiWindow w(screenwidth, screenheight);
	w.Append(&backBtn);
	mainWindow->Append(&optionBrowser);
	mainWindow->Append(&w);
	mainWindow->Append(&titleTxt);
	ResumeGui();

	while(menuCurrent == MENU_SETTINGS_SUBTITLES && !guiShutdown)
	{
		usleep(THREAD_SLEEP);

		ret = optionBrowser.GetClickedOption();

		switch (ret)
		{
			case 0:
				WiiSettings.subtitleVisibility ^= 1;
				break;

			case 1:
				WiiSettings.subtitlePosition++;
				if (WiiSettings.subtitlePosition > 3)
					WiiSettings.subtitlePosition = 0;
				break;

			case 2:
				WiiSettings.subtitleScale += 0.1;
				if (WiiSettings.subtitleScale > 1.5)
					WiiSettings.subtitleScale = 0.5;
				break;

			case 3:
				WiiSettings.subtitleDelay += 0.1;
				if (WiiSettings.subtitleDelay > 2)
					WiiSettings.subtitleDelay = -2;
				break;
		}

		if(ret >= 0 || firstRun)
		{
			firstRun = false;
			sprintf(options.value[0], "%s", WiiSettings.subtitleVisibility ? "On" : "Off");
			
			switch(WiiSettings.subtitlePosition)
			{
				case SUBTITLE_ALIGN_TOP:
					sprintf(options.value[1], "Top"); break;
				case SUBTITLE_ALIGN_CENTER:
					sprintf(options.value[1], "Center"); break;
				case SUBTITLE_ALIGN_BOTTOM:
					sprintf(options.value[1], "Bottom"); break;
			}

			sprintf(options.value[2], "%.2f%%", WiiSettings.subtitleScale*100);
			sprintf(options.value[3], "%.1f sec", WiiSettings.subtitleDelay);

			optionBrowser.TriggerUpdate();
		}

		if(backBtn.GetState() == STATE_CLICKED)
		{
			menuCurrent = MENU_SETTINGS;
		}
	}
	SuspendGui();
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

	MenuItemList items;
	sprintf(items.name[i], "Global");
	items.img[i] = NULL; i++;
	sprintf(items.name[i], "Videos");
	items.img[i] = NULL; i++;
	sprintf(items.name[i], "Music");
	items.img[i] = NULL; i++;
	sprintf(items.name[i], "Pictures");
	items.img[i] = NULL; i++;
	sprintf(items.name[i], "Network");
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

	SuspendGui();
	mainWindow->Append(&itemBrowser);
	mainWindow->Append(&backBtn);
	mainWindow->Append(&titleTxt);
	ResumeGui();

	while(menuCurrent == MENU_SETTINGS && !guiShutdown)
	{
		usleep(THREAD_SLEEP);

		ret = itemBrowser.GetClickedItem();

		switch (ret)
		{
			case 0:
				menuCurrent = MENU_SETTINGS_GLOBAL;
				break;

			case 1:
				menuCurrent = MENU_SETTINGS_VIDEOS;
				break;

			case 2:
				menuCurrent = MENU_SETTINGS_MUSIC;
				break;

			case 3:
				menuCurrent = MENU_SETTINGS_PICTURES;
				break;

			case 4:
				menuCurrent = MENU_SETTINGS_NETWORK;
				break;

			case 5:
				menuCurrent = MENU_SETTINGS_SUBTITLES;
				break;
		}

		if(backBtn.GetState() == STATE_CLICKED)
			ChangeMenu(menuPrevious);
	}

	SuspendGui();
	mainWindow->Remove(&itemBrowser);
	mainWindow->Remove(&backBtn);
	mainWindow->Remove(&titleTxt);
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
		DisableRumble();
		UpdateVideobarPauseBtn(true);
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
		DisableRumble();
		UpdateVideobarPauseBtn(true);
		wiiSkipBackward();
	}
}

static void VideoRewindCallback(void * ptr)
{
	GuiButton * b = (GuiButton *)ptr;
	if(b->GetState() == STATE_CLICKED)
	{
		b->ResetState();
		DisableRumble();
		UpdateVideobarPauseBtn(true);
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
		UpdateVideobarPauseBtn(wiiIsPaused());
	}
}

static void VideoFastForwardCallback(void * ptr)
{
	GuiButton * b = (GuiButton *)ptr;
	if(b->GetState() == STATE_CLICKED)
	{
		b->ResetState();
		DisableRumble();
		UpdateVideobarPauseBtn(true);
		wiiFastForward();
	}
}

static void VideoSkipForwardCallback(void * ptr)
{
	GuiButton * b = (GuiButton *)ptr;
	if(b->GetState() == STATE_CLICKED)
	{
		b->ResetState();
		DisableRumble();
		UpdateVideobarPauseBtn(true);
		wiiSkipForward();
	}
}

static void AudioProgressCallback(void * ptr)
{
	GuiButton * b = (GuiButton *)ptr;

	double total = wiiGetTimeLength();
	int done = wiiGetTimePos();
	double percent = 0;

	if(!playingAudio)
		total = 0; // values are from a loaded video - do not show!

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
	}

	if(b->GetState() == STATE_CLICKED)
	{
		percent = (userInput[b->GetStateChan()].wpad->ir.x - b->GetLeft())/360.0;
		done = total*percent;
		b->ResetState();
		wiiSeekPos(done);
		UpdateAudiobarPauseBtn(false);
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
		wiiSeekPos(0);
		UpdateAudiobarPauseBtn(false);
	}
}

static void AudioPauseCallback(void * ptr)
{
	GuiButton * b = (GuiButton *)ptr;
	if(b->GetState() == STATE_CLICKED)
	{
		b->ResetState();
		
		if(playingAudio)
		{
			wiiPause();
			UpdateAudiobarPauseBtn(wiiIsPaused());
		}
		else if(playlistSize > 0)
		{
			// skip to next song
			playingAudio = true;
			ShutdownMPlayer();
			mainWindow->Remove(videoImg);
			FindNextAudioFile();
			LoadMPlayer();
			UpdateAudiobarPauseBtn(false);
		}
	}
}

static void AudioSkipForwardCallback(void * ptr)
{
	GuiButton * b = (GuiButton *)ptr;
	if(b->GetState() == STATE_CLICKED)
	{
		b->ResetState();

		if(playlistSize > 0)
		{
			// skip to next song
			ShutdownMPlayer();
			FindNextAudioFile();
			LoadMPlayer();
			UpdateAudiobarPauseBtn(false);
		}
	}
}

static void AudioModeCallback(void * ptr)
{
	GuiButton * b = (GuiButton *)ptr;
	if(b->GetState() == STATE_CLICKED)
	{
		b->ResetState();
		WiiSettings.playOrder++;
		if(WiiSettings.playOrder > PLAY_LOOP)
			WiiSettings.playOrder = 0;

		UpdateAudiobarModeBtn();
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
	static int actionbarSetup = 0;
	
	if(actionbarSetup)
		return;

	actionbarTrigA = new GuiTrigger();
	actionbarTrigA->SetSimpleTrigger(-1, WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A, PAD_BUTTON_A);
	
	// images
	
	actionbarProgress = new GuiImageData(actionbar_progress_bg_png);
	actionbarProgressLeft = new GuiImageData(actionbar_progress_left_png);
	actionbarProgressMid = new GuiImageData(actionbar_progress_mid_png);
	actionbarProgressRight = new GuiImageData(actionbar_progress_right_png);
	actionbarCircle = new GuiImageData(actionbar_circle_png);
	actionbarCircleOver = new GuiImageData(actionbar_circle_over_png);
	actionbarSkipBackward = new GuiImageData(actionbar_skipbackward_png);
	actionbarRewind = new GuiImageData(actionbar_rewind_png);
	actionbarPause = new GuiImageData(actionbar_pause_png);
	actionbarPlay = new GuiImageData(actionbar_play_png);
	actionbarFastForward = new GuiImageData(actionbar_fastforward_png);
	actionbarSkipForward = new GuiImageData(actionbar_skipforward_png);
	actionbarSingle = new GuiImageData(actionbar_single_png);
	actionbarContinuous = new GuiImageData(actionbar_continuous_png);
	actionbarShuffle = new GuiImageData(actionbar_shuffle_png);
	actionbarLoop = new GuiImageData(actionbar_loop_png);	
	actionbarClose = new GuiImageData(actionbar_close_png);
	actionbarSlideshow = new GuiImageData(actionbar_slideshow_png);
	
	// video bar

	videobarProgressImg = new GuiImage(actionbarProgress);
	videobarProgressLeftImg = new GuiImage(actionbarProgressLeft);
	videobarProgressLeftImg->SetPosition(3, 0);
	videobarProgressLeftImg->SetVisible(false);
	videobarProgressMidImg = new GuiImage(actionbarProgressMid);
	videobarProgressMidImg->SetPosition(15, 0);
	videobarProgressMidImg->SetTile(0);
	videobarProgressRightImg = new GuiImage(actionbarProgressRight);
	videobarProgressRightImg->SetAlignment(ALIGN_RIGHT, ALIGN_TOP);
	videobarProgressRightImg->SetVisible(false);
	videobarSkipBackwardImg = new GuiImage(actionbarCircle);
	videobarSkipBackwardImg->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	videobarSkipBackwardOverImg = new GuiImage(actionbarCircleOver);
	videobarSkipBackwardOverImg->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	videobarSkipBackwardIcon = new GuiImage(actionbarSkipBackward);
	videobarSkipBackwardIcon->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	videobarRewindImg = new GuiImage(actionbarCircle);
	videobarRewindImg->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	videobarRewindOverImg = new GuiImage(actionbarCircleOver);
	videobarRewindOverImg->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	videobarRewindIcon = new GuiImage(actionbarRewind);
	videobarRewindIcon->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	videobarPauseImg = new GuiImage(actionbarCircle);
	videobarPauseImg->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	videobarPauseOverImg = new GuiImage(actionbarCircleOver);
	videobarPauseOverImg->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	videobarPauseIcon = new GuiImage(actionbarPause);
	videobarPauseIcon->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	videobarFastForwardImg = new GuiImage(actionbarCircle);
	videobarFastForwardImg->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	videobarFastForwardOverImg = new GuiImage(actionbarCircleOver);
	videobarFastForwardOverImg->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	videobarFastForwardIcon = new GuiImage(actionbarFastForward);
	videobarFastForwardIcon->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	videobarSkipForwardImg = new GuiImage(actionbarCircle);
	videobarSkipForwardImg->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	videobarSkipForwardOverImg = new GuiImage(actionbarCircleOver);
	videobarSkipForwardOverImg->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	videobarSkipForwardIcon = new GuiImage(actionbarSkipForward);
	videobarSkipForwardIcon->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	
	videobarSkipBackwardTip = new GuiTooltip("Skip Backward");
	videobarRewindTip = new GuiTooltip("Rewind");
	videobarPauseTip = new GuiTooltip("Pause");
	videobarFastForwardTip = new GuiTooltip("Fast Forward");
	videobarSkipForwardTip = new GuiTooltip("Skip Forward");
	
	videobarProgressBtn = new GuiButton(videobarProgressImg->GetWidth(), videobarProgressImg->GetHeight());
	videobarProgressBtn->SetImage(videobarProgressImg);
	videobarProgressBtn->SetPosition(3, 0);
	videobarProgressBtn->SetTrigger(actionbarTrigA);
	videobarProgressBtn->SetSelectable(false);
	videobarProgressBtn->SetUpdateCallback(VideoProgressCallback);

	videobarSkipBackwardBtn = new GuiButton(50, 50);
	videobarSkipBackwardBtn->SetAlignment(ALIGN_LEFT, ALIGN_BOTTOM);
	videobarSkipBackwardBtn->SetPosition(0, 0);
	videobarSkipBackwardBtn->SetImage(videobarSkipBackwardImg);
	videobarSkipBackwardBtn->SetImageOver(videobarSkipBackwardOverImg);
	videobarSkipBackwardBtn->SetIcon(videobarSkipBackwardIcon);
	videobarSkipBackwardBtn->SetTooltip(videobarSkipBackwardTip);
	videobarSkipBackwardBtn->SetTrigger(actionbarTrigA);
	videobarSkipBackwardBtn->SetSelectable(false);
	videobarSkipBackwardBtn->SetUpdateCallback(VideoSkipBackwardCallback);
	videobarSkipBackwardBtn->SetEffectGrow();
	
	videobarRewindBtn = new GuiButton(50, 50);
	videobarRewindBtn->SetAlignment(ALIGN_LEFT, ALIGN_BOTTOM);
	videobarRewindBtn->SetPosition(80, 0);
	videobarRewindBtn->SetImage(videobarRewindImg);
	videobarRewindBtn->SetImageOver(videobarRewindOverImg);
	videobarRewindBtn->SetIcon(videobarRewindIcon);
	videobarRewindBtn->SetTooltip(videobarRewindTip);
	videobarRewindBtn->SetTrigger(actionbarTrigA);
	videobarRewindBtn->SetSelectable(false);
	videobarRewindBtn->SetUpdateCallback(VideoRewindCallback);
	videobarRewindBtn->SetEffectGrow();
	
	videobarPauseBtn = new GuiButton(50, 50);
	videobarPauseBtn->SetAlignment(ALIGN_LEFT, ALIGN_BOTTOM);
	videobarPauseBtn->SetPosition(160, 0);
	videobarPauseBtn->SetImage(videobarPauseImg);
	videobarPauseBtn->SetImageOver(videobarPauseOverImg);
	videobarPauseBtn->SetIcon(videobarPauseIcon);
	videobarPauseBtn->SetTooltip(videobarPauseTip);
	videobarPauseBtn->SetTrigger(actionbarTrigA);
	videobarPauseBtn->SetSelectable(false);
	videobarPauseBtn->SetUpdateCallback(VideoPauseCallback);
	videobarPauseBtn->SetEffectGrow();
	
	videobarFastForwardBtn = new GuiButton(50, 50);
	videobarFastForwardBtn->SetAlignment(ALIGN_LEFT, ALIGN_BOTTOM);
	videobarFastForwardBtn->SetPosition(240, 0);
	videobarFastForwardBtn->SetImage(videobarFastForwardImg);
	videobarFastForwardBtn->SetImageOver(videobarFastForwardOverImg);
	videobarFastForwardBtn->SetIcon(videobarFastForwardIcon);
	videobarFastForwardBtn->SetTooltip(videobarFastForwardTip);
	videobarFastForwardBtn->SetTrigger(actionbarTrigA);
	videobarFastForwardBtn->SetSelectable(false);
	videobarFastForwardBtn->SetUpdateCallback(VideoFastForwardCallback);
	videobarFastForwardBtn->SetEffectGrow();
	
	videobarSkipForwardBtn = new GuiButton(50, 50);
	videobarSkipForwardBtn->SetAlignment(ALIGN_LEFT, ALIGN_BOTTOM);
	videobarSkipForwardBtn->SetPosition(320, 0);
	videobarSkipForwardBtn->SetImage(videobarSkipForwardImg);
	videobarSkipForwardBtn->SetImageOver(videobarSkipForwardOverImg);
	videobarSkipForwardBtn->SetIcon(videobarSkipForwardIcon);
	videobarSkipForwardBtn->SetTooltip(videobarSkipForwardTip);
	videobarSkipForwardBtn->SetTrigger(actionbarTrigA);
	videobarSkipForwardBtn->SetSelectable(false);
	videobarSkipForwardBtn->SetUpdateCallback(VideoSkipForwardCallback);
	videobarSkipForwardBtn->SetEffectGrow();
	
	videobarTime = new GuiText(NULL, 16, (GXColor){0, 0, 0, 255});
	videobarTime->SetAlignment(ALIGN_RIGHT, ALIGN_TOP);
	videobarTime->SetPosition(0, -18);

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
	
	audiobarProgressImg = new GuiImage(actionbarProgress);
	audiobarProgressLeftImg = new GuiImage(actionbarProgressLeft);
	audiobarProgressLeftImg->SetPosition(3, 0);
	audiobarProgressLeftImg->SetVisible(false);
	audiobarProgressMidImg = new GuiImage(actionbarProgressMid);
	audiobarProgressMidImg->SetPosition(15, 0);
	audiobarProgressMidImg->SetTile(0);
	audiobarProgressRightImg = new GuiImage(actionbarProgressRight);
	audiobarProgressRightImg->SetAlignment(ALIGN_RIGHT, ALIGN_TOP);
	audiobarProgressRightImg->SetVisible(false);
	audiobarSkipBackwardImg = new GuiImage(actionbarCircle);
	audiobarSkipBackwardImg->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	audiobarSkipBackwardOverImg = new GuiImage(actionbarCircleOver);
	audiobarSkipBackwardOverImg->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	audiobarSkipBackwardIcon = new GuiImage(actionbarSkipBackward);
	audiobarSkipBackwardIcon->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	audiobarPauseImg = new GuiImage(actionbarCircle);
	audiobarPauseImg->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	audiobarPauseOverImg = new GuiImage(actionbarCircleOver);
	audiobarPauseOverImg->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	audiobarPauseIcon = new GuiImage(actionbarPlay);
	audiobarPauseIcon->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	audiobarSkipForwardImg = new GuiImage(actionbarCircle);
	audiobarSkipForwardImg->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	audiobarSkipForwardOverImg = new GuiImage(actionbarCircleOver);
	audiobarSkipForwardOverImg->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	audiobarSkipForwardIcon = new GuiImage(actionbarSkipForward);
	audiobarSkipForwardIcon->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	audiobarModeImg = new GuiImage(actionbarCircle);
	audiobarModeImg->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	audiobarModeOverImg = new GuiImage(actionbarCircleOver);
	audiobarModeOverImg->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	audiobarModeIcon = new GuiImage;
	audiobarModeIcon->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);

	audiobarSkipBackwardTip = new GuiTooltip("Restart Song");
	audiobarPauseTip = new GuiTooltip("Play");
	audiobarSkipForwardTip = new GuiTooltip("Next Song");
	audiobarModeTip = new GuiTooltip("Single Play");

	UpdateAudiobarModeBtn();

	audiobarProgressBtn = new GuiButton(audiobarProgressImg->GetWidth(), audiobarProgressImg->GetHeight());
	audiobarProgressBtn->SetImage(audiobarProgressImg);
	audiobarProgressBtn->SetPosition(3, 0);
	audiobarProgressBtn->SetTrigger(actionbarTrigA);
	audiobarProgressBtn->SetSelectable(false);
	audiobarProgressBtn->SetUpdateCallback(AudioProgressCallback);

	audiobarSkipBackwardBtn = new GuiButton(50, 50);
	audiobarSkipBackwardBtn->SetAlignment(ALIGN_LEFT, ALIGN_BOTTOM);
	audiobarSkipBackwardBtn->SetPosition(0, 0);
	audiobarSkipBackwardBtn->SetImage(audiobarSkipBackwardImg);
	audiobarSkipBackwardBtn->SetImageOver(audiobarSkipBackwardOverImg);
	audiobarSkipBackwardBtn->SetIcon(audiobarSkipBackwardIcon);
	audiobarSkipBackwardBtn->SetTooltip(audiobarSkipBackwardTip);
	audiobarSkipBackwardBtn->SetTrigger(actionbarTrigA);
	audiobarSkipBackwardBtn->SetUpdateCallback(AudioSkipBackwardCallback);
	audiobarSkipBackwardBtn->SetEffectGrow();
	
	audiobarPauseBtn = new GuiButton(50, 50);
	audiobarPauseBtn->SetAlignment(ALIGN_LEFT, ALIGN_BOTTOM);
	audiobarPauseBtn->SetPosition(80, 0);
	audiobarPauseBtn->SetImage(audiobarPauseImg);
	audiobarPauseBtn->SetImageOver(audiobarPauseOverImg);
	audiobarPauseBtn->SetIcon(audiobarPauseIcon);
	audiobarPauseBtn->SetTooltip(audiobarPauseTip);
	audiobarPauseBtn->SetTrigger(actionbarTrigA);
	audiobarPauseBtn->SetUpdateCallback(AudioPauseCallback);
	audiobarPauseBtn->SetEffectGrow();
	
	audiobarSkipForwardBtn = new GuiButton(50, 50);
	audiobarSkipForwardBtn->SetAlignment(ALIGN_LEFT, ALIGN_BOTTOM);
	audiobarSkipForwardBtn->SetPosition(160, 0);
	audiobarSkipForwardBtn->SetImage(audiobarSkipForwardImg);
	audiobarSkipForwardBtn->SetImageOver(audiobarSkipForwardOverImg);
	audiobarSkipForwardBtn->SetIcon(audiobarSkipForwardIcon);
	audiobarSkipForwardBtn->SetTooltip(audiobarSkipForwardTip);
	audiobarSkipForwardBtn->SetTrigger(actionbarTrigA);
	audiobarSkipForwardBtn->SetUpdateCallback(AudioSkipForwardCallback);
	audiobarSkipForwardBtn->SetEffectGrow();
	
	audiobarModeBtn = new GuiButton(50, 50);
	audiobarModeBtn->SetAlignment(ALIGN_LEFT, ALIGN_BOTTOM);
	audiobarModeBtn->SetPosition(240, 0);
	audiobarModeBtn->SetImage(audiobarModeImg);
	audiobarModeBtn->SetImageOver(audiobarModeOverImg);
	audiobarModeBtn->SetIcon(audiobarModeIcon);
	audiobarModeBtn->SetTooltip(audiobarModeTip);
	audiobarModeBtn->SetTrigger(actionbarTrigA);
	audiobarModeBtn->SetUpdateCallback(AudioModeCallback);
	audiobarModeBtn->SetEffectGrow();
	
	audiobarNowPlaying[0] = new GuiText("Now Playing:", 16, (GXColor){0, 0, 0, 255});
	audiobarNowPlaying[0]->SetAlignment(ALIGN_RIGHT, ALIGN_TOP);
	audiobarNowPlaying[0]->SetPosition(0, 0);
	audiobarNowPlaying[0]->SetVisible(false);
	
	for(int i=1; i < 4; i++)
	{
		audiobarNowPlaying[i] = new GuiText(NULL, 16, (GXColor){0, 0, 0, 255});
		audiobarNowPlaying[i]->SetAlignment(ALIGN_RIGHT, ALIGN_TOP);
		audiobarNowPlaying[i]->SetPosition(0, 20*i);
		audiobarNowPlaying[i]->SetMaxWidth(250);
		audiobarNowPlaying[i]->SetVisible(false);
	}

	audiobar = new GuiWindow(500, 80);

	audiobar->Append(audiobarProgressBtn);
	audiobar->Append(audiobarProgressLeftImg);
	audiobar->Append(audiobarProgressMidImg);
	audiobar->Append(audiobarProgressRightImg);
	audiobar->Append(audiobarSkipBackwardBtn);
	audiobar->Append(audiobarPauseBtn);
	audiobar->Append(audiobarSkipForwardBtn);
	audiobar->Append(audiobarModeBtn);
	
	for(int i=0; i < 4; i++)
		audiobar->Append(audiobarNowPlaying[i]);

	audiobar->SetAlignment(ALIGN_LEFT, ALIGN_BOTTOM);
	audiobar->SetPosition(100, -30);
	
	// setup picture bar

	picturebarPreviousImg = new GuiImage(actionbarCircle);
	picturebarPreviousImg->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	picturebarPreviousOverImg = new GuiImage(actionbarCircleOver);
	picturebarPreviousOverImg->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	picturebarPreviousIcon = new GuiImage(actionbarRewind);
	picturebarPreviousIcon->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	
	picturebarNextImg = new GuiImage(actionbarCircle);
	picturebarNextImg->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	picturebarNextOverImg = new GuiImage(actionbarCircleOver);
	picturebarNextOverImg->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	picturebarNextIcon = new GuiImage(actionbarFastForward);
	picturebarNextIcon->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	
	picturebarSlideshowImg = new GuiImage(actionbarCircle);
	picturebarSlideshowImg->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	picturebarSlideshowOverImg = new GuiImage(actionbarCircleOver);
	picturebarSlideshowOverImg->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	picturebarSlideshowIcon = new GuiImage(actionbarSlideshow);
	picturebarSlideshowIcon->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);

	picturebarCloseImg = new GuiImage(actionbarCircle);
	picturebarCloseImg->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	picturebarCloseOverImg = new GuiImage(actionbarCircleOver);
	picturebarCloseOverImg->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	picturebarCloseIcon = new GuiImage(actionbarClose);
	picturebarCloseIcon->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	
	picturebarPreviousTip = new GuiTooltip("Previous");
	picturebarNextTip = new GuiTooltip("Next");
	picturebarSlideshowTip = new GuiTooltip("Slideshow");
	picturebarCloseTip = new GuiTooltip("Close");

	picturebarPreviousBtn = new GuiButton(50, 50);
	picturebarPreviousBtn->SetAlignment(ALIGN_LEFT, ALIGN_BOTTOM);
	picturebarPreviousBtn->SetPosition(0, 0);
	picturebarPreviousBtn->SetImage(picturebarPreviousImg);
	picturebarPreviousBtn->SetImageOver(picturebarPreviousOverImg);
	picturebarPreviousBtn->SetIcon(picturebarPreviousIcon);
	picturebarPreviousBtn->SetTooltip(picturebarPreviousTip);
	picturebarPreviousBtn->SetTrigger(actionbarTrigA);
	picturebarPreviousBtn->SetUpdateCallback(PicturePreviousCallback);
	picturebarPreviousBtn->SetEffectGrow();
	
	picturebarNextBtn = new GuiButton(50, 50);
	picturebarNextBtn->SetAlignment(ALIGN_LEFT, ALIGN_BOTTOM);
	picturebarNextBtn->SetPosition(80, 0);
	picturebarNextBtn->SetImage(picturebarNextImg);
	picturebarNextBtn->SetImageOver(picturebarNextOverImg);
	picturebarNextBtn->SetIcon(picturebarNextIcon);
	picturebarNextBtn->SetTooltip(picturebarNextTip);
	picturebarNextBtn->SetTrigger(actionbarTrigA);
	picturebarNextBtn->SetUpdateCallback(PictureNextCallback);
	picturebarNextBtn->SetEffectGrow();

	picturebarSlideshowBtn = new GuiButton(50, 50);
	picturebarSlideshowBtn->SetAlignment(ALIGN_LEFT, ALIGN_BOTTOM);
	picturebarSlideshowBtn->SetPosition(160, 0);
	picturebarSlideshowBtn->SetImage(picturebarSlideshowImg);
	picturebarSlideshowBtn->SetImageOver(picturebarSlideshowOverImg);
	picturebarSlideshowBtn->SetIcon(picturebarSlideshowIcon);
	picturebarSlideshowBtn->SetTooltip(picturebarSlideshowTip);
	picturebarSlideshowBtn->SetTrigger(actionbarTrigA);
	picturebarSlideshowBtn->SetUpdateCallback(PictureSlideshowCallback);
	picturebarSlideshowBtn->SetEffectGrow();
	
	picturebarCloseBtn = new GuiButton(50, 50);
	picturebarCloseBtn->SetAlignment(ALIGN_LEFT, ALIGN_BOTTOM);
	picturebarCloseBtn->SetPosition(240, 0);
	picturebarCloseBtn->SetImage(picturebarCloseImg);
	picturebarCloseBtn->SetImageOver(picturebarCloseOverImg);
	picturebarCloseBtn->SetIcon(picturebarCloseIcon);
	picturebarCloseBtn->SetTooltip(picturebarCloseTip);
	picturebarCloseBtn->SetTrigger(actionbarTrigA);
	picturebarCloseBtn->SetUpdateCallback(PictureCloseCallback);
	picturebarCloseBtn->SetEffectGrow();
	
	picturebar = new GuiWindow(320, 80);
	picturebar->SetAlignment(ALIGN_CENTRE, ALIGN_BOTTOM);
	picturebar->SetPosition(0, -40);

	picturebar->Append(picturebarPreviousBtn);
	picturebar->Append(picturebarNextBtn);
	picturebar->Append(picturebarSlideshowBtn);
	picturebar->Append(picturebarCloseBtn);
	
	actionbarSetup = 1;
}

void GuiInit()
{
	guiHalt = 1;
	LWP_CreateThread (&guithread, GuiThread, NULL, NULL, 0, 66);
}

static void StartGuiThreads()
{
	showProgress = 0;
	progressThreadHalt = 1;
	pictureThreadHalt = 1;
	creditsThreadHalt = 1;
	updateThreadHalt = 1;
	
	LWP_CreateThread (&progressthread, ProgressThread, NULL, NULL, 0, 60);
	LWP_CreateThread (&picturethread, PictureThread, NULL, NULL, 0, 60);
	LWP_CreateThread (&creditsthread, CreditsThread, NULL, NULL, 0, 60);
	LWP_CreateThread (&updatethread, UpdateThread, NULL, NULL, 0, 60);
}

static void StopGuiThreads()
{
	showProgress = 0;
	progressThreadHalt = 2;

	if(progressthread != LWP_THREAD_NULL)
	{
		if(LWP_ThreadIsSuspended(progressthread))
			LWP_ResumeThread (progressthread);
		
		// wait for thread to finish
		LWP_JoinThread(progressthread, NULL);
		progressthread = LWP_THREAD_NULL;
	}

	pictureThreadHalt = 2;

	if(picturethread != LWP_THREAD_NULL)
	{
		if(LWP_ThreadIsSuspended(picturethread))
			LWP_ResumeThread (picturethread);
		
		// wait for thread to finish
		LWP_JoinThread(picturethread, NULL);
		picturethread = LWP_THREAD_NULL;
	}

	creditsThreadHalt = 2;

	if(creditsthread != LWP_THREAD_NULL)
	{
		if(LWP_ThreadIsSuspended(creditsthread))
			LWP_ResumeThread (creditsthread);
		
		// wait for thread to finish
		LWP_JoinThread(creditsthread, NULL);
		creditsthread = LWP_THREAD_NULL;
	}

	updateThreadHalt = 2;

	if(updatethread != LWP_THREAD_NULL)
	{
		if(LWP_ThreadIsSuspended(updatethread))
			LWP_ResumeThread (updatethread);
		
		// wait for thread to finish
		LWP_JoinThread(updatethread, NULL);
		updatethread = LWP_THREAD_NULL;
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

	mainWindow->Append(&bg);

	if(videoScreenshot)
	{
		videoImg = new GuiImage(videoScreenshot, screenwidth, screenheight);
		mainWindow->Append(videoImg);
	}
	
	mainWindow->Append(&bg2);
	mainWindow->Append(&bgRoundedImg);

	GuiImageData logo(logo_png);
	GuiImage logoBtnImg(&logo);
	logoBtn = new GuiButton(logo.GetWidth(), logo.GetHeight());
	logoBtn->SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	logoBtn->SetPosition(70, 40);
	logoBtn->SetImage(&logoBtnImg);
	logoBtn->SetTrigger(&trigA);
	logoBtn->SetSelectable(false);
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
	videosBtn->SetPosition(44, 30);
	videosBtn->SetTooltip(&videosBtnTip);
	videosBtn->SetImage(&videosBtnImg);
	videosBtn->SetTrigger(&trigA);
	videosBtn->SetSelectable(false);
	videosBtn->SetEffectGrow();
	videosBtn->SetUpdateCallback(ChangeMenuVideos);

	GuiTooltip musicBtnTip("Music");
	GuiImage musicBtnImg(&music);
	musicBtn = new GuiButton(musicBtnImg.GetWidth(), musicBtnImg.GetHeight());
	musicBtn->SetAlignment(ALIGN_LEFT, ALIGN_TOP);
	musicBtn->SetPosition(92, 30);
	musicBtn->SetTooltip(&musicBtnTip);
	musicBtn->SetImage(&musicBtnImg);
	musicBtn->SetTrigger(&trigA);
	musicBtn->SetSelectable(false);
	musicBtn->SetEffectGrow();
	musicBtn->SetUpdateCallback(ChangeMenuMusic);

	GuiTooltip picturesBtnTip("Pictures");
	GuiImage picturesBtnImg(&pictures);
	picturesBtn = new GuiButton(picturesBtnImg.GetWidth(), picturesBtnImg.GetHeight());
	picturesBtn->SetAlignment(ALIGN_LEFT, ALIGN_TOP);
	picturesBtn->SetPosition(140, 30);
	picturesBtn->SetTooltip(&picturesBtnTip);
	picturesBtn->SetImage(&picturesBtnImg);
	picturesBtn->SetTrigger(&trigA);
	picturesBtn->SetSelectable(false);
	picturesBtn->SetEffectGrow();
	picturesBtn->SetUpdateCallback(ChangeMenuPictures);

	GuiTooltip dvdBtnTip("DVD");
	GuiImage dvdBtnImg(&dvd);
	dvdBtn = new GuiButton(dvdBtnImg.GetWidth(), dvdBtnImg.GetHeight());
	dvdBtn->SetAlignment(ALIGN_LEFT, ALIGN_TOP);
	dvdBtn->SetPosition(188, 30);
	dvdBtn->SetTooltip(&dvdBtnTip);
	dvdBtn->SetImage(&dvdBtnImg);
	dvdBtn->SetTrigger(&trigA);
	dvdBtn->SetSelectable(false);
	dvdBtn->SetEffectGrow();
	dvdBtn->SetUpdateCallback(ChangeMenuDVD);

	GuiTooltip onlineBtnTip("Online Media");
	GuiImage onlineBtnImg(&online);
	onlineBtn = new GuiButton(onlineBtnImg.GetWidth(), onlineBtnImg.GetHeight());
	onlineBtn->SetAlignment(ALIGN_LEFT, ALIGN_TOP);
	onlineBtn->SetPosition(236, 30);
	onlineBtn->SetTooltip(&onlineBtnTip);
	onlineBtn->SetImage(&onlineBtnImg);
	onlineBtn->SetTrigger(&trigA);
	onlineBtn->SetSelectable(false);
	onlineBtn->SetEffectGrow();
	onlineBtn->SetUpdateCallback(ChangeMenuOnline);

	GuiTooltip settingsBtnTip("Settings");
	GuiImage settingsBtnImg(&settings);
	settingsBtn = new GuiButton(settingsBtnImg.GetWidth(), settingsBtnImg.GetHeight());
	settingsBtn->SetAlignment(ALIGN_RIGHT, ALIGN_TOP);
	settingsBtn->SetPosition(-92, 30);
	settingsBtn->SetImage(&settingsBtnImg);
	settingsBtn->SetTooltip(&settingsBtnTip);
	settingsBtn->SetTrigger(&trigA);
	settingsBtn->SetSelectable(false);
	settingsBtn->SetEffectGrow();
	settingsBtn->SetUpdateCallback(ChangeMenuSettings);
	
	GuiTooltip exitBtnTip("Exit");
	GuiImage exitBtnImg(&exit);
	exitBtn = new GuiButton(exitBtnImg.GetWidth(), exitBtnImg.GetHeight());
	exitBtn->SetAlignment(ALIGN_RIGHT, ALIGN_TOP);
	exitBtn->SetPosition(-44, 30);
	exitBtn->SetImage(&exitBtnImg);
	exitBtn->SetTooltip(&exitBtnTip);
	exitBtn->SetTrigger(&trigA);
	exitBtn->SetSelectable(false);
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

	pictureImg = new GuiImage;
	pictureImg->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	pictureBtn = new GuiButton(PIC_WIDTH, PIC_HEIGHT);
	pictureBtn->SetImage(pictureImg);
	pictureBtn->SetTrigger(&trigA);
	pictureBtn->SetSelectable(false);
	pictureBtn->SetAlignment(ALIGN_RIGHT, ALIGN_TOP);
	pictureBtn->SetPosition(-50, 100);

	StartGuiThreads();
	ResumeGui();

	// Load settings
	if(!LoadSettings())
		SaveSettings(NOTSILENT);

	while(!guiShutdown)
	{
		switch (menuCurrent)
		{
			case MENU_BROWSE_VIDEOS:
			case MENU_BROWSE_MUSIC:
			case MENU_BROWSE_ONLINEMEDIA:
				MenuBrowse(menuCurrent);
				break;
			case MENU_BROWSE_PICTURES:
				MenuBrowsePictures();
				break;
			case MENU_DVD:
				MenuDVD();
				break;
			case MENU_SETTINGS:
				MenuSettings();
				break;
			case MENU_SETTINGS_GLOBAL:
				MenuSettingsGlobal();
				break;
			case MENU_SETTINGS_VIDEOS:
				MenuSettingsVideos();
				break;
			case MENU_SETTINGS_MUSIC:
				MenuSettingsMusic();
				break;
			case MENU_SETTINGS_PICTURES:
				MenuSettingsPictures();
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
	StopGuiThreads();
	SuspendGui();

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
	delete pictureBtn;
	pictureBtn = NULL;
	delete pictureImg;
	pictureImg = NULL;

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
	if(!statusText)
		return false;

	return statusText->IsVisible();
}

void SetStatus(const char * txt)
{
	if(!statusText)
		return;

	if(txt)
	{
		statusText->SetVisible(true);
	}
	else
	{
		statusText->SetVisible(false);
		EnableRumble();
		UpdateVideobarPauseBtn(wiiIsPaused());
	}
	statusText->SetText(txt);
}

/****************************************************************************
 * MPlayer Menu
 ***************************************************************************/
void MPlayerMenu()
{
	menuMode = 1; // switch to MPlayer GUI mode
	guiShutdown = false;

	GuiTrigger trigA;
	trigA.SetSimpleTrigger(-1, WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A, PAD_BUTTON_A);

	mainWindow = new GuiWindow(screenwidth, screenheight);
	GuiImage bgBottom(screenwidth, 152, (GXColor){155, 155, 155, 155});
	bgBottom.SetAlignment(ALIGN_LEFT, ALIGN_BOTTOM);
	
	// status text
	statusText = new GuiText(NULL, 24, (GXColor){255, 255, 255, 255});
	statusText->SetVisible(false);

	mainWindow->Append(&bgBottom);
	mainWindow->Append(videobar);
	mainWindow->Append(statusText);

	ResumeGui();

	while(controlledbygui == 0)
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
			statusText->SetVisible(false);
		}
	}

	ShutoffRumble();
	CancelAction();
	SuspendGui();

	delete statusText;
	delete mainWindow;
	mainWindow = NULL;
}
