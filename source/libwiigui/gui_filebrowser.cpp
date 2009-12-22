/****************************************************************************
 * libwiigui
 *
 * Tantric 2009
 *
 * gui_filebrowser.cpp
 *
 * GUI class definitions
 ***************************************************************************/

#include "gui.h"
#include "filebrowser.h"

/**
 * Constructor for the GuiFileBrowser class.
 */
GuiFileBrowser::GuiFileBrowser(int w, int h)
{
	width = w;
	height = h;
	numEntries = 0;
	selectedItem = 0;
	selectable = true;
	listChanged = true; // trigger an initial list update
	focus = 0; // allow focus

	trigA = new GuiTrigger;
	trigA->SetSimpleTrigger(-1, WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A, PAD_BUTTON_A);

	trigHeldA = new GuiTrigger;
	trigHeldA->SetHeldTrigger(-1, WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A, PAD_BUTTON_A);

	bgBrowseEntry = new GuiImageData(bg_browse_entry_png);
	bgBrowseEntryOver = new GuiImageData(bg_browse_entry_over_png);
	iconFolder = new GuiImageData(icon_folder_png);
	iconSD = new GuiImageData(icon_sd_png);
	iconUSB = new GuiImageData(icon_usb_png);
	iconDVD = new GuiImageData(icon_dvd_png);
	iconSMB = new GuiImageData(icon_smb_png);
	iconFTP = new GuiImageData(icon_ftp_png);
	playlistAdd = new GuiImageData(playlist_add_png);

	scrollbar = new GuiImageData(scrollbar_png);
	scrollbarImg = new GuiImage(scrollbar);
	scrollbarImg->SetParent(this);
	scrollbarImg->SetAlignment(ALIGN_RIGHT, ALIGN_TOP);
	scrollbarImg->SetPosition(0, 30);

	arrowDown = new GuiImageData(scrollbar_arrowdown_png);
	arrowDownImg = new GuiImage(arrowDown);
	arrowDownOver = new GuiImageData(scrollbar_arrowdown_over_png);
	arrowDownOverImg = new GuiImage(arrowDownOver);
	arrowUp = new GuiImageData(scrollbar_arrowup_png);
	arrowUpImg = new GuiImage(arrowUp);
	arrowUpOver = new GuiImageData(scrollbar_arrowup_over_png);
	arrowUpOverImg = new GuiImage(arrowUpOver);
	scrollbarBox = new GuiImageData(scrollbar_box_png);
	scrollbarBoxImg = new GuiImage(scrollbarBox);
	scrollbarBoxOver = new GuiImageData(scrollbar_box_over_png);
	scrollbarBoxOverImg = new GuiImage(scrollbarBoxOver);

	arrowUpBtn = new GuiButton(arrowUpImg->GetWidth(), arrowUpImg->GetHeight());
	arrowUpBtn->SetParent(this);
	arrowUpBtn->SetImage(arrowUpImg);
	arrowUpBtn->SetImageOver(arrowUpOverImg);
	arrowUpBtn->SetAlignment(ALIGN_RIGHT, ALIGN_TOP);
	arrowUpBtn->SetSelectable(false);
	arrowUpBtn->SetClickable(false);
	arrowUpBtn->SetHoldable(true);
	arrowUpBtn->SetTrigger(trigHeldA);

	arrowDownBtn = new GuiButton(arrowDownImg->GetWidth(), arrowDownImg->GetHeight());
	arrowDownBtn->SetParent(this);
	arrowDownBtn->SetImage(arrowDownImg);
	arrowDownBtn->SetImageOver(arrowDownOverImg);
	arrowDownBtn->SetAlignment(ALIGN_RIGHT, ALIGN_BOTTOM);
	arrowDownBtn->SetSelectable(false);
	arrowDownBtn->SetClickable(false);
	arrowDownBtn->SetHoldable(true);
	arrowDownBtn->SetTrigger(trigHeldA);

	scrollbarBoxBtn = new GuiButton(scrollbarBoxImg->GetWidth(), scrollbarBoxImg->GetHeight());
	scrollbarBoxBtn->SetParent(this);
	scrollbarBoxBtn->SetImage(scrollbarBoxImg);
	scrollbarBoxBtn->SetImageOver(scrollbarBoxOverImg);
	scrollbarBoxBtn->SetAlignment(ALIGN_RIGHT, ALIGN_TOP);
	scrollbarBoxBtn->SetMinY(0);
	scrollbarBoxBtn->SetMaxY(130);
	scrollbarBoxBtn->SetSelectable(false);
	scrollbarBoxBtn->SetClickable(false);
	scrollbarBoxBtn->SetHoldable(true);
	scrollbarBoxBtn->SetTrigger(trigHeldA);

	for(int i=0; i<FILE_PAGESIZE; i++)
	{
		fileListText[i] = new GuiText(NULL, 18, (GXColor){255, 255, 255, 0xff});
		fileListText[i]->SetAlignment(ALIGN_LEFT, ALIGN_MIDDLE);
		fileListText[i]->SetPosition(30,0);
		fileListText[i]->SetMaxWidth(455);

		fileListBg[i] = new GuiImage(bgBrowseEntryOver);
		fileListIcon[i] = NULL;

		fileList[i] = new GuiButton(460, 26);
		fileList[i]->SetParent(this);
		fileList[i]->SetLabel(fileListText[i]);
		fileList[i]->SetImageOver(fileListBg[i]);
		fileList[i]->SetPosition(0,26*i+3);
		fileList[i]->SetTrigger(trigA);
		
		playlistAddImg[i] = new GuiImage(playlistAdd);
		playlistAddTip[i] = new GuiTooltip("Add to Playlist");
		playlistAddBtn[i] = new GuiButton(playlistAdd->GetWidth(), playlistAdd->GetHeight());
		playlistAddBtn[i]->SetParent(this);
		playlistAddBtn[i]->SetImage(playlistAddImg[i]);
		playlistAddBtn[i]->SetTooltip(playlistAddTip[i]);
		playlistAddBtn[i]->SetAlignment(ALIGN_RIGHT, ALIGN_TOP);
		playlistAddBtn[i]->SetPosition(-40, 2 + i*26);
		playlistAddBtn[i]->SetTrigger(trigA);
		playlistAddBtn[i]->SetEffectGrow();
		playlistAddBtn[i]->SetVisible(false);
		playlistAddBtn[i]->SetState(STATE_DISABLED);
	}
}

/**
 * Destructor for the GuiFileBrowser class.
 */
GuiFileBrowser::~GuiFileBrowser()
{
	delete arrowUpBtn;
	delete arrowDownBtn;
	delete scrollbarBoxBtn;

	delete scrollbarImg;
	delete arrowDownImg;
	delete arrowDownOverImg;
	delete arrowUpImg;
	delete arrowUpOverImg;
	delete scrollbarBoxImg;
	delete scrollbarBoxOverImg;

	delete bgBrowseEntry;
	delete bgBrowseEntryOver;
	delete iconFolder;
	delete iconSD;
	delete iconUSB;
	delete iconDVD;
	delete iconSMB;
	delete iconFTP;
	delete playlistAdd;
	delete scrollbar;
	delete arrowDown;
	delete arrowDownOver;
	delete arrowUp;
	delete arrowUpOver;
	delete scrollbarBox;
	delete scrollbarBoxOver;

	delete trigA;
	delete trigHeldA;

	for(int i=0; i<FILE_PAGESIZE; i++)
	{
		delete fileListText[i];
		delete fileList[i];
		delete fileListBg[i];

		if(fileListIcon[i])
			delete fileListIcon[i];
		
		delete playlistAddImg[i];
		delete playlistAddTip[i];
		delete playlistAddBtn[i];
	}
}

void GuiFileBrowser::SetFocus(int f)
{
	focus = f;

	for(int i=0; i<FILE_PAGESIZE; i++)
		fileList[i]->ResetState();

	if(f == 1)
		fileList[selectedItem]->SetState(STATE_SELECTED);
}

void GuiFileBrowser::ResetState()
{
	state = STATE_DEFAULT;
	stateChan = -1;
	selectedItem = 0;

	for(int i=0; i<FILE_PAGESIZE; i++)
	{
		fileList[i]->ResetState();
		playlistAddBtn[i]->ResetState();
	}
}

void GuiFileBrowser::TriggerUpdate()
{
	selectedItem = browser.selIndex - browser.pageIndex;
	listChanged = true;
}

/**
 * Draw the button on screen
 */
void GuiFileBrowser::Draw()
{
	if(!this->IsVisible())
		return;

	for(int i=0; i<FILE_PAGESIZE; i++)
	{
		fileList[i]->Draw();
		playlistAddBtn[i]->Draw();
	}

	scrollbarImg->Draw();
	arrowUpBtn->Draw();
	arrowDownBtn->Draw();
	scrollbarBoxBtn->Draw();
	this->UpdateEffects();
}

void GuiFileBrowser::DrawTooltip()
{
	for(int i=0; i<FILE_PAGESIZE; i++)
	{
		if(playlistAddTip[i])
			playlistAddTip[i]->DrawTooltip();
	}
}

void GuiFileBrowser::Update(GuiTrigger * t)
{
	if(state == STATE_DISABLED || !t)
		return;

	int position = 0;
	int positionWiimote = 0;

	arrowUpBtn->Update(t);
	arrowDownBtn->Update(t);
	scrollbarBoxBtn->Update(t);

	// move the file listing to respond to wiimote cursor movement
	if(scrollbarBoxBtn->GetState() == STATE_HELD &&
		scrollbarBoxBtn->GetStateChan() == t->chan &&
		t->wpad->ir.valid &&
		browser.numEntries > FILE_PAGESIZE
		)
	{
		scrollbarBoxBtn->SetPosition(0,0);
		positionWiimote = t->wpad->ir.y - 60 - scrollbarBoxBtn->GetTop();

		if(positionWiimote < scrollbarBoxBtn->GetMinY())
			positionWiimote = scrollbarBoxBtn->GetMinY();
		else if(positionWiimote > scrollbarBoxBtn->GetMaxY())
			positionWiimote = scrollbarBoxBtn->GetMaxY();

		browser.pageIndex = (positionWiimote * browser.numEntries)/190.0 - selectedItem;

		if(browser.pageIndex <= 0)
		{
			browser.pageIndex = 0;
		}
		else if(browser.pageIndex+FILE_PAGESIZE >= browser.numEntries)
		{
			browser.pageIndex = browser.numEntries-FILE_PAGESIZE;
		}
		listChanged = true;
		focus = false;
	}

	if(arrowDownBtn->GetState() == STATE_HELD && arrowDownBtn->GetStateChan() == t->chan)
	{
		t->wpad->btns_d |= WPAD_BUTTON_DOWN;
		if(!this->IsFocused())
			((GuiWindow *)this->GetParent())->ChangeFocus(this);
	}
	else if(arrowUpBtn->GetState() == STATE_HELD && arrowUpBtn->GetStateChan() == t->chan)
	{
		t->wpad->btns_d |= WPAD_BUTTON_UP;
		if(!this->IsFocused())
			((GuiWindow *)this->GetParent())->ChangeFocus(this);
	}

	// pad/joystick navigation
	if(!focus)
	{
		goto endNavigation; // skip navigation
		listChanged = false;
	}

	if(t->Right())
	{
		if(browser.pageIndex < browser.numEntries && browser.numEntries > FILE_PAGESIZE)
		{
			browser.pageIndex += FILE_PAGESIZE;
			if(browser.pageIndex+FILE_PAGESIZE >= browser.numEntries)
				browser.pageIndex = browser.numEntries-FILE_PAGESIZE;
			listChanged = true;
		}
	}
	else if(t->Left())
	{
		if(browser.pageIndex > 0)
		{
			browser.pageIndex -= FILE_PAGESIZE;
			if(browser.pageIndex < 0)
				browser.pageIndex = 0;
			listChanged = true;
		}
	}
	else if(t->Down())
	{
		if(browser.pageIndex + selectedItem + 1 < browser.numEntries)
		{
			if(selectedItem == FILE_PAGESIZE-1)
			{
				// move list down by 1
				browser.pageIndex++;
				listChanged = true;
			}
			else if(fileList[selectedItem+1]->IsVisible())
			{
				fileList[selectedItem]->ResetState();
				fileList[++selectedItem]->SetState(STATE_SELECTED, t->chan);
			}
		}
	}
	else if(t->Up())
	{
		if(selectedItem == 0 &&	browser.pageIndex + selectedItem > 0)
		{
			// move list up by 1
			browser.pageIndex--;
			listChanged = true;
		}
		else if(selectedItem > 0)
		{
			fileList[selectedItem]->ResetState();
			fileList[--selectedItem]->SetState(STATE_SELECTED, t->chan);
		}
	}

	endNavigation:

	for(int i=0; i<FILE_PAGESIZE; i++)
	{
		if(listChanged || numEntries != browser.numEntries)
		{
			if(browser.pageIndex+i < browser.numEntries)
			{
				if(fileList[i]->GetState() == STATE_DISABLED)
					fileList[i]->SetState(STATE_DEFAULT);

				fileList[i]->SetVisible(true);

				fileListText[i]->SetText(browserList[browser.pageIndex+i].displayname);

				if(fileListIcon[i])
				{
					delete(fileListIcon[i]);
					fileListIcon[i] = NULL;
				}

				switch(browserList[browser.pageIndex+i].icon)
				{
					case ICON_FOLDER:
						fileListIcon[i] = new GuiImage(iconFolder);
						break;
					case ICON_SD:
						fileListIcon[i] = new GuiImage(iconSD);
						break;
					case ICON_USB:
						fileListIcon[i] = new GuiImage(iconUSB);
						break;
					case ICON_DVD:
						fileListIcon[i] = new GuiImage(iconDVD);
						break;
					case ICON_SMB:
						fileListIcon[i] = new GuiImage(iconSMB);
						break;
					case ICON_FTP:
						fileListIcon[i] = new GuiImage(iconFTP);
						break;
				}
				fileList[i]->SetIcon(fileListIcon[i]);
				
				if(currentMenu == MENU_BROWSE_MUSIC && i > 0)
				{
					if(browser.dir[0] == 0 || inPlaylist) // at root or in playlist
					{
						playlistAddBtn[i]->SetVisible(false);
						playlistAddBtn[i]->SetState(STATE_DISABLED);
					}
					else
					{
						playlistAddBtn[i]->SetVisible(true);
						if(playlistAddBtn[i]->GetState() == STATE_DISABLED)
							playlistAddBtn[i]->SetState(STATE_DEFAULT);
					}
				}
			}
			else
			{
				fileList[i]->SetVisible(false);
				fileList[i]->SetState(STATE_DISABLED);
				
				if(currentMenu == MENU_BROWSE_MUSIC && i > 0)
				{
					playlistAddBtn[i]->SetVisible(false);
					playlistAddBtn[i]->SetState(STATE_DISABLED);
				}
			}
		}

		if(i != selectedItem && fileList[i]->GetState() == STATE_SELECTED)
			fileList[i]->ResetState();
		else if(focus && i == selectedItem && fileList[i]->GetState() == STATE_DEFAULT)
			fileList[selectedItem]->SetState(STATE_SELECTED, t->chan);

		int currChan = t->chan;

		if(t->wpad->ir.valid && !fileList[i]->IsInside(t->wpad->ir.x, t->wpad->ir.y))
			t->chan = -1;

		fileList[i]->Update(t);
		t->chan = currChan;

		if(fileList[i]->GetState() == STATE_SELECTED)
		{
			selectedItem = i;
			browser.selIndex = browser.pageIndex + i;
		}

		if(selectedItem == i)
			fileListText[i]->SetScroll(SCROLL_HORIZONTAL);
		else
			fileListText[i]->SetScroll(SCROLL_NONE);
		
		playlistAddBtn[i]->Update(t);
	}

	// update the location of the scroll box based on the position in the file list
	if(positionWiimote > 0 || listChanged || numEntries != browser.numEntries)
	{
		if(positionWiimote > 0)
		{
			position = positionWiimote; // follow wiimote cursor
		}
		else
		{
			position = 130*(browser.pageIndex + FILE_PAGESIZE/2.0) / (browser.numEntries*1.0);
	
			if(browser.pageIndex/(FILE_PAGESIZE/2.0) < 1)
				position = 0;
			else if((browser.pageIndex+FILE_PAGESIZE)/(FILE_PAGESIZE*1.0) >= (browser.numEntries)/(FILE_PAGESIZE*1.0))
				position = 130;
		}
		scrollbarBoxBtn->SetPosition(0,position+36);
	}

	listChanged = false;
	numEntries = browser.numEntries;

	if(updateCB)
		updateCB(this);
}
