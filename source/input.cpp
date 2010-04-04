/****************************************************************************
 * libwiigui Template
 * Tantric 2009
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
	WPAD_ScanPads();
}

/****************************************************************************
 * SetupPads
 *
 * Sets up userInput triggers for use
 ***************************************************************************/
void SetupPads()
{
	WPAD_Init();
	WPAD_SetIdleTimeout(60);

	// read wiimote accelerometer and IR data
	WPAD_SetDataFormat(WPAD_CHAN_ALL,WPAD_FMT_BTNS_ACC_IR);
	WPAD_SetVRes(WPAD_CHAN_ALL, screenwidth, screenheight);

	for(int i=0; i < 4; i++)
	{
		userInput[i].chan = i;
		userInput[i].wpad = WPAD_Data(i);
	}
}

/****************************************************************************
 * ShutoffRumble
 ***************************************************************************/

static void ShutoffRumble(int i)
{
	prev[i] = now[i] + RUMBLE_COOLOFF;
	WPAD_Rumble(i, 0); // rumble off
	rumbleOn[i] = 0;
}

void ShutoffRumble()
{
	for(int i=0;i<4;i++)
		ShutoffRumble(i);
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
	if(!WiiSettings.rumble || rumbleDisabled || i < 0) return;
	
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
			ShutoffRumble(i);
	}
}

/****************************************************************************
 * MPlayerInput
 ***************************************************************************/

void MPlayerInput()
{
	bool ir = false;
	bool inDVDMenu = wiiInDVDMenu();
	static bool volumeUpdated = false;

	for(int i=0; i<4; i++)
	{
		if(userInput[i].wpad->ir.valid)
			ir = true;

		if(userInput[i].wpad->btns_d & WPAD_BUTTON_1)
		{
			osdLevel ^= 1;
		}

		if(ConfigRequested || userInput[i].wpad->btns_d & WPAD_BUTTON_HOME)
		{
			ConfigRequested = 0;
			wiiGotoGui();
		}

		if(inDVDMenu)
			continue;

		if(userInput[i].wpad->btns_d & WPAD_BUTTON_A)
		{
			// Hack to allow people to unpause while the OSD GUI is visible by
			// pointing above the button bar and pressing A. We also need to be outside
			// the boundaries of the volume bar area, when it is visible
			int x = userInput[i].wpad->ir.x;
			int y = userInput[i].wpad->ir.y;

			if(!drawGui || (y < 360 && 
				(!VolumeLevelBarVisible() || !(x > 20 && x < 100 && y > 180))))
			{
				wiiPause();
			}
		}

		if(userInput[i].wpad->btns_d & WPAD_BUTTON_RIGHT)
		{
			wiiFastForward();
		}
		else if(userInput[i].wpad->btns_d & WPAD_BUTTON_LEFT)
		{
			wiiRewind();
		}
		else if(userInput[i].wpad->btns_h & WPAD_BUTTON_PLUS)
		{
			volnow = gettime();

			if(diff_usec(volprev, volnow) > VOL_DELAY)
			{
				volprev = volnow;
				WiiSettings.volume++;
				if(WiiSettings.volume > 100) WiiSettings.volume = 100;
				wiiSetVolume(WiiSettings.volume);
				volumeUpdated = true;
				ShowVolumeLevelBar();
			}
		}
		else if(userInput[i].wpad->btns_h & WPAD_BUTTON_MINUS)
		{
			volnow = gettime();

			if(diff_usec(volprev, volnow) > VOL_DELAY)
			{
				volprev = volnow;
				WiiSettings.volume--;
				if(WiiSettings.volume < 0) WiiSettings.volume = 0;
				wiiSetVolume(WiiSettings.volume);
				volumeUpdated = true;
				ShowVolumeLevelBar();
			}
		}
	}

	if(inDVDMenu)
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
			wiiDVDNav(MP_CMD_DVDNAV_UP);
		if(userInput[0].wpad->btns_d & WPAD_BUTTON_DOWN)
			wiiDVDNav(MP_CMD_DVDNAV_DOWN);
		if(userInput[0].wpad->btns_d & WPAD_BUTTON_RIGHT)
			wiiDVDNav(MP_CMD_DVDNAV_RIGHT);
		if(userInput[0].wpad->btns_d & WPAD_BUTTON_LEFT)
			wiiDVDNav(MP_CMD_DVDNAV_LEFT);
	}
	else
	{
		if(userInput[0].wpad->btns_d & WPAD_BUTTON_2)
			wiiDVDNav(MP_CMD_DVDNAV_MENU);

		if(userInput[0].wpad->btns_d & WPAD_BUTTON_B)
			wiiSetProperty(MP_CMD_SUB_SELECT, -1);
	}

	if(volumeUpdated)
	{
		volnow = gettime();

		if(volnow > volprev && diff_usec(volprev, volnow) > VOLDISP_MAX)
			volumeUpdated = false;
		else
			ir = true; // trigger display
	}

	if(ir || StatusSet() || osdLevel)
	{
		drawGui = true;
	}
	else if(drawGui)
	{
		drawGui = false;
		HideVolumeLevelBar();
		ShutoffRumble();
	}
}
