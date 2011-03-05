/****************************************************************************
 * WiiMC
 * Tantric 2009-2011
 *
 * input.cpp
 * Wii/GameCube controller management
 ***************************************************************************/

#include <gccore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ogcsys.h>
#include <unistd.h>
#include <wiiuse/wpad.h>
#include <ogc/lwp_watchdog.h>

#include "wiimc.h"
#include "menu.h"
#include "video.h"
#include "input.h"
#include "settings.h"
#include "libwiigui/gui.h"
#include "mplayer/input/input.h"
#include "mplayer/osdep/gx_supp.h"

#define RUMBLE_MAX 		60000
#define RUMBLE_COOLOFF 	10000000
#define VOL_DELAY		30000
#define VOLDISP_MAX 	500000

static bool rumbleDisabled = false;
static int rumbleOn[4] = {0,0,0,0};
static u64 prev[4];
static u64 now[4];
static int osdLevel = 0;
static int volprev = 0, volnow = 0;

GuiTrigger userInput[4];

/****************************************************************************
 * UpdatePads
 *
 * Scans wpad
 ***************************************************************************/
void UpdatePads()
{
	WPAD_ReadPending(0, NULL); // only wiimote 1
}

/****************************************************************************
 * SetupPads
 *
 * Sets up userInput triggers for use
 ***************************************************************************/
void SetupPads()
{
	WPAD_SetIdleTimeout(60);

	// read wiimote accelerometer and IR data
	WPAD_SetDataFormat(WPAD_CHAN_0, WPAD_FMT_BTNS_ACC_IR);
	WPAD_SetVRes(WPAD_CHAN_0, screenwidth, screenheight);

	userInput[0].chan = 0;
	userInput[0].wpad = WPAD_Data(0);
}

/****************************************************************************
 * ShutoffRumble
 ***************************************************************************/

static void ShutoffRumble(int i, int cooloff)
{
	if(CONF_GetPadMotorMode() == 0)
		return;

	prev[i] = gettime() + cooloff;
	WPAD_Rumble(i, 0); // rumble off
	rumbleOn[i] = 0;
}

void ShutoffRumble()
{
	ShutoffRumble(0, RUMBLE_COOLOFF*3);
}

void DisableRumble()
{
	rumbleDisabled = true;
	ShutoffRumble();
}

void EnableRumble()
{
	rumbleDisabled = false;
}

void RequestRumble(int i)
{
	if(CONF_GetPadMotorMode() == 0 || !WiiSettings.rumble || rumbleDisabled || i < 0)
		return;

	now[i] = gettime();

	if(prev[i] > now[i])
		return;

	if(diff_usec(prev[i], now[i]) > RUMBLE_MAX)
	{
		rumbleOn[i] = 1;
		WPAD_Rumble(i, 1); // rumble on
		prev[i] = now[i];
	}
}

/****************************************************************************
 * DoRumble
 ***************************************************************************/

void DoRumble(int i)
{
	if(rumbleOn[i])
	{
		now[i] = gettime();

		if(diff_usec(prev[i], now[i]) > RUMBLE_MAX)
			ShutoffRumble(i, RUMBLE_COOLOFF);
	}
}

/****************************************************************************
 * MPlayerInput
 ***************************************************************************/

void MPlayerResize(float fZoomHorIncr, float fZoomVertIncr)
{
	if(wiiIsPaused())
		return;

	WiiSettings.videoZoomHor += fZoomHorIncr;
	WiiSettings.videoZoomVert += fZoomVertIncr;
	GX_SetScreenPos(WiiSettings.videoXshift, WiiSettings.videoYshift, 
		WiiSettings.videoZoomHor, WiiSettings.videoZoomVert);
}

void MPlayerInput()
{
	bool ir = false;
	bool inDVDMenu = wiiInDVDMenu();
	static bool volumeUpdated = false;

	if(userInput[0].wpad->ir.valid)
		ir = true;

	if(userInput[0].wpad->btns_d & WPAD_BUTTON_1)
		osdLevel ^= 1;
	else if(ExitRequested || userInput[0].wpad->btns_d & WPAD_BUTTON_HOME)
		wiiGotoGui();

	if(!inDVDMenu)
	{
		if(userInput[0].wpad->btns_d & WPAD_BUTTON_A)
		{
			// Hack to allow people to unpause while the OSD GUI is visible by
			// pointing above the button bar and pressing A. We also need to be outside
			// the boundaries of the volume bar area, when it is visible
			int x = userInput[0].wpad->ir.x;
			int y = userInput[0].wpad->ir.y;

			int xoffset = 20;

			if(screenwidth == 768)
				xoffset = 80;

			if(!drawGui || (y < 360 && 
				(!VideoVolumeLevelBarVisible() || !(x > xoffset && x < xoffset+100 && y > 180))))
			{
				wiiPause();
			}
		}
		else if(userInput[0].wpad->btns_d & WPAD_BUTTON_RIGHT)
		{
			if(userInput[0].wpad->btns_h & WPAD_BUTTON_B)
				MPlayerResize(+0.01F, 0.00F);
			else
				wiiFastForward();
		}
		else if(userInput[0].wpad->btns_d & WPAD_BUTTON_LEFT)
		{
			if(userInput[0].wpad->btns_h & WPAD_BUTTON_B)
				MPlayerResize(-0.01F, 0.00F);
			else
				wiiRewind();
		}
		else if(userInput[0].wpad->btns_d & WPAD_BUTTON_UP)
		{
			if(userInput[0].wpad->btns_h & WPAD_BUTTON_B)
				MPlayerResize(0.00F, +0.01F);
			else if(!wiiIsPaused())
				wiiSetProperty(MP_CMD_SUB_SELECT, 0);
		}
		else if(userInput[0].wpad->btns_d & WPAD_BUTTON_DOWN)
		{
			if(userInput[0].wpad->btns_h & WPAD_BUTTON_B)
				MPlayerResize(0.00F, -0.01F);
			else if(!wiiIsPaused())
				wiiSetProperty(MP_CMD_SWITCH_AUDIO, 0);
		}
		else if(userInput[0].wpad->btns_h & WPAD_BUTTON_PLUS)
		{
			volnow = gettime();
	
			if(diff_usec(volprev, volnow) > VOL_DELAY)
			{
				volprev = volnow;
				WiiSettings.volume++;
				if(WiiSettings.volume > 100) WiiSettings.volume = 100;
				wiiSetVolume(WiiSettings.volume);
				volumeUpdated = true;
				ShowVideoVolumeLevelBar();
			}
		}
		else if(userInput[0].wpad->btns_h & WPAD_BUTTON_MINUS)
		{
			volnow = gettime();
	
			if(diff_usec(volprev, volnow) > VOL_DELAY)
			{
				volprev = volnow;
				WiiSettings.volume--;
				if(WiiSettings.volume < 0) WiiSettings.volume = 0;
				wiiSetVolume(WiiSettings.volume);
				volumeUpdated = true;
				ShowVideoVolumeLevelBar();
			}
		}

		if(userInput[0].wpad->btns_d & WPAD_BUTTON_2)
			wiiDVDNav(MP_CMD_DVDNAV_MENU);
	}
	else
	{
		if(userInput[0].wpad->ir.valid)
			wiiUpdatePointer((int)userInput[0].wpad->ir.x, (int)userInput[0].wpad->ir.y);

		if(userInput[0].wpad->btns_d & WPAD_BUTTON_A)
		{
			if(userInput[0].wpad->ir.valid)
				wiiDVDNav(MP_CMD_DVDNAV_MOUSECLICK);
			else
				wiiDVDNav(MP_CMD_DVDNAV_SELECT);
		}

		if(userInput[0].wpad->btns_d & WPAD_BUTTON_UP)
		{
			if(userInput[0].wpad->btns_h & WPAD_BUTTON_B)
				MPlayerResize(0.00F, +0.01F);
			else
				wiiDVDNav(MP_CMD_DVDNAV_UP);
		}
		else if(userInput[0].wpad->btns_d & WPAD_BUTTON_DOWN)
		{
			if(userInput[0].wpad->btns_h & WPAD_BUTTON_B)
				MPlayerResize(0.00F, -0.01F);
			else
				wiiDVDNav(MP_CMD_DVDNAV_DOWN);
		}
		else if(userInput[0].wpad->btns_d & WPAD_BUTTON_RIGHT)
		{
			if(userInput[0].wpad->btns_h & WPAD_BUTTON_B)
				MPlayerResize(+0.01F, 0.00F);
			else
				wiiDVDNav(MP_CMD_DVDNAV_RIGHT);
		}
		else if(userInput[0].wpad->btns_d & WPAD_BUTTON_LEFT)
		{
			if(userInput[0].wpad->btns_h & WPAD_BUTTON_B)
				MPlayerResize(-0.01F, 0.00F);
			else
				wiiDVDNav(MP_CMD_DVDNAV_LEFT);
		}
	}

	if(volumeUpdated)
	{
		volnow = gettime();

		if(volnow > volprev && diff_usec(volprev, volnow) > VOLDISP_MAX)
			volumeUpdated = false;
		else
			ir = true; // trigger display
	}

	if(ir || BufferingStatusSet() || osdLevel)
	{
		drawGui = true;
	}
	else if(drawGui)
	{
		drawGui = false;
		HideVideoVolumeLevelBar();
		ShutoffRumble();
	}
}
