/****************************************************************************
 * libwiigui
 *
 * Tantric 2009
 *
 * gui_menubrowser.cpp
 *
 * GUI class definitions
 ***************************************************************************/

#include "gui.h"

/**
 * Constructor for the GuiMenuBrowser class.
 */
GuiMenuBrowser::GuiMenuBrowser(int w, int s, MenuItemList * l)
{
	width = w;
	height = s*32;
	size = s;
	items = l;
	selectable = true;
	selectedItem = this->FindMenuItem(-1, 1);
	focus = 0; // allow focus

	trigA = new GuiTrigger;
	trigA->SetSimpleTrigger(-1, WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A, PAD_BUTTON_A);

	bgItem = new GuiImageData(bg_entry_png);
	bgItemOver = new GuiImageData(bg_entry_over_png);

	for(int i=0; i<items->length; i++)
	{
		itemTxt[i] = new GuiText(items->name[i], 18, (GXColor){255, 255, 255, 0xff});
		itemTxt[i]->SetAlignment(ALIGN_LEFT, ALIGN_MIDDLE);
		itemTxt[i]->SetPosition(0,-2);

		itemBg[i] = new GuiImage(bgItem);
		itemBg[i]->SetPosition(-30, 0);
		itemBg[i]->SetTile(w/20);
		itemOverBg[i] = new GuiImage(bgItemOver);
		itemOverBg[i]->SetPosition(-30, -2);
		itemOverBg[i]->SetTile(w/20);

		itemBtn[i] = new GuiButton(w-92, 32);
		itemBtn[i]->SetParent(this);
		itemBtn[i]->SetLabel(itemTxt[i], 0);
		itemBtn[i]->SetImage(itemBg[i]);
		itemBtn[i]->SetImageOver(itemOverBg[i]);
		itemBtn[i]->SetPosition(30,32*i);
		itemBtn[i]->SetTrigger(trigA);

		if(items->name[i][0] == 0)
		{
			itemBtn[i]->SetVisible(false);
			itemBtn[i]->SetState(STATE_DISABLED);
		}
	}
}

/**
 * Destructor for the GuiMenuBrowser class.
 */
GuiMenuBrowser::~GuiMenuBrowser()
{
	for(int i=0; i<items->length; i++)
	{
		delete itemBtn[i];
		delete itemTxt[i];
		delete itemBg[i];
		delete itemOverBg[i];
	}
	delete bgItem;
	delete bgItemOver;
	delete trigA;
}

void GuiMenuBrowser::SetFocus(int f)
{
	focus = f;

	for(int i=0; i<items->length; i++)
		itemBtn[i]->ResetState();

	if(f == 1)
		itemBtn[selectedItem]->SetState(STATE_SELECTED);
}

void GuiMenuBrowser::ResetState()
{
	if(state != STATE_DISABLED)
	{
		state = STATE_DEFAULT;
		stateChan = -1;
	}

	for(int i=0; i<items->length; i++)
	{
		itemBtn[i]->ResetState();
	}
}

int GuiMenuBrowser::GetSelectedItem()
{
	return selectedItem;
}

int GuiMenuBrowser::GetClickedItem()
{
	int found = -1;
	for(int i=0; i<items->length; i++)
	{
		if(itemBtn[i]->GetState() == STATE_CLICKED)
		{
			itemBtn[i]->SetState(STATE_SELECTED);
			found = i;
			break;
		}
	}
	return found;
}

/****************************************************************************
 * FindMenuItem
 *
 * Help function to find the next visible menu item on the list
 ***************************************************************************/

int GuiMenuBrowser::FindMenuItem(int currentItem, int direction)
{
	int nextItem = currentItem + direction;

	if(nextItem < 0)
		nextItem = items->length-1;
	else if(nextItem >= items->length)
		nextItem = 0;

	if(strlen(items->name[nextItem]) > 0)
		return nextItem;
	else
		return FindMenuItem(nextItem, direction);
}

/**
 * Draw the button on screen
 */
void GuiMenuBrowser::Draw()
{
	if(!this->IsVisible())
		return;

	for(int i=0; i<items->length; i++)
	{
		itemBtn[i]->Draw();
	}

	this->UpdateEffects();
}

void GuiMenuBrowser::ResetText()
{
	for(int i=0; i<items->length; i++)
	{
		itemBtn[i]->ResetText();
	}
}

void GuiMenuBrowser::Update(GuiTrigger * t)
{
	if(state == STATE_DISABLED || !t)
		return;

	for(int i=0; i<items->length; i++)
	{
		if(i != selectedItem && itemBtn[i]->GetState() == STATE_SELECTED)
			itemBtn[i]->ResetState();
		else if(focus && i == selectedItem && itemBtn[i]->GetState() == STATE_DEFAULT)
			itemBtn[selectedItem]->SetState(STATE_SELECTED, -1);

		if(!(t->wpad->btns_h & WPAD_BUTTON_DOWN) && !(t->wpad->btns_h & WPAD_BUTTON_UP))		
		{
			int currChan = t->chan;

			if(t->wpad->ir.valid && !itemBtn[i]->IsInside(t->wpad->ir.x, t->wpad->ir.y))
				t->chan = -1;

			itemBtn[i]->Update(t);
			t->chan = currChan;
			
			// override state chan
			if(itemBtn[i]->GetStateChan() != -1)
				itemBtn[i]->SetStateChan(-1);
		}

		if(itemBtn[i]->GetState() == STATE_SELECTED)
			selectedItem = i;
	}

	// pad/joystick navigation
	if(!focus)
		return; // skip navigation

	if(t->Down())
	{
		itemBtn[selectedItem]->ResetState();
		selectedItem = this->FindMenuItem(selectedItem, 1);
		itemBtn[selectedItem]->SetState(STATE_SELECTED, -1);
	}
	else if(t->Up())
	{
		itemBtn[selectedItem]->ResetState();
		selectedItem = this->FindMenuItem(selectedItem, -1);
		itemBtn[selectedItem]->SetState(STATE_SELECTED, -1);
	}

	if(updateCB)
		updateCB(this);
}
