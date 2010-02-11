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

#include "wiimc.h"
#include "menu.h"
#include "video.h"
#include "input.h"
#include "settings.h"
#include "libwiigui/gui.h"
#include "mplayer/input/input.h"

int rumbleRequest[4] = {0,0,0,0};
GuiTrigger userInput[4];
static int rumbleCount[4] = {0,0,0,0};
static int osdLevel = 0;

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

void ShutoffRumble()
{
	for(int i=0;i<4;i++)
	{
		WPAD_Rumble(i, 0);
		rumbleCount[i] = 0;
		rumbleRequest[i] = 0;
	}
}

/****************************************************************************
 * DoRumble
 ***************************************************************************/

void DoRumble(int i)
{
	if(!WiiSettings.rumble) return;

	if(rumbleRequest[i])
	{
		if(rumbleCount[i] < 3)
		{
			WPAD_Rumble(i, 1); // rumble on
			++rumbleCount[i];
		}
		else
		{
			rumbleCount[i] = 12;
			rumbleRequest[i] = 0;
		}
	}
	else
	{
		if(rumbleCount[i])
			--rumbleCount[i];
		WPAD_Rumble(i, 0); // rumble off
	}
}

/****************************************************************************
 * MPlayerInput
 ***************************************************************************/

void MPlayerInput()
{
	int i;
	bool ir = false;
	bool inDVDMenu = wiiInDVDMenu();

	for(i=0; i<4; i++)
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

		if(!drawGui)
		{			
			if(userInput[i].wpad->btns_d & WPAD_BUTTON_A)
				wiiPause();
			if(userInput[i].wpad->btns_d & WPAD_BUTTON_UP)
				wiiSkipForward();
			if(userInput[i].wpad->btns_d & WPAD_BUTTON_DOWN)
				wiiSkipBackward();
			if(userInput[i].wpad->btns_d & WPAD_BUTTON_RIGHT)
				wiiFastForward();
			if(userInput[i].wpad->btns_d & WPAD_BUTTON_LEFT)
				wiiRewind();
		}
		else
		{
			// Quick hack to allow people to unpause while the OSD GUI is visible by pointing above the 
			// button bar and pressing A. We should look at this behavior. 
			if(userInput[i].wpad->btns_d & WPAD_BUTTON_A && userInput[i].wpad->ir.y < 340)
				wiiPause();
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
	else if(userInput[0].wpad->btns_d & WPAD_BUTTON_2)
	{
		wiiDVDNav(MP_CMD_DVDNAV_MENU);
	}

	if(ir || StatusSet() || osdLevel)
	{
		drawGui = true;
	}
	else if(drawGui)
	{
		drawGui = false;
		ShutoffRumble();
	}
}
