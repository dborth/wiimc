/****************************************************************************
 * libwiigui
 *
 * Tantric 2009
 *
 * gui_tooltip.cpp
 *
 * GUI class definitions
 ***************************************************************************/

#include "gui.h"

static GuiImageData tooltipLeft(tooltip_left_png);
static GuiImageData tooltipTile(tooltip_tile_png);
static GuiImageData tooltipRight(tooltip_right_png);


/**
 * Constructor for the GuiTooltip class.
 */
GuiTooltip::GuiTooltip(const char *t)
{
	leftImage.SetImage(&tooltipLeft);
	tileImage.SetImage(&tooltipTile);
	rightImage.SetImage(&tooltipRight);
	
	height = leftImage.GetHeight();
	
	text = new GuiText(t);
	SetText(t);
	
	leftImage.SetParent(this);
	tileImage.SetParent(this);
	rightImage.SetParent(this);
	text->SetParent(this);
	
	time1 = 0;
	time2 = 0;
}

/*
 * Destructor for the GuiTooltip class.
 */
GuiTooltip::~GuiTooltip()
{
	if(text)
		delete text;
}

float GuiTooltip::GetScale()
{
	return (scale * scaleDyn);
}

/* !Sets the text of the GuiTooltip element
 * !\param t Text
 */
void GuiTooltip::SetText(const char * t)
{
	if(text)
	{
		delete text;
		text = NULL;
	}
	
	if(!t)
		return;
	
	text = new GuiText(t, 16, (GXColor){0, 0, 0, 255});
	text->SetParent(this);

	int tile = (strlen(t)*8) / tileImage.GetWidth();

	if(tile < 0)
		tile = 0;

	tileImage.SetPosition(leftImage.GetWidth(), 0);
	tileImage.SetTile(tile);
	rightImage.SetPosition(leftImage.GetWidth() + tile * tileImage.GetWidth(), 0);
	width = leftImage.GetWidth() + tile * tileImage.GetWidth() + rightImage.GetWidth();
}

/*
 * Draws the tooltip
 */
void GuiTooltip::DrawTooltip()
{
	if(!text || !this->IsVisible() || !parentElement)
		return;

	if(parentElement->GetState() == STATE_SELECTED)
	{
		if (time2 == 0)
		{
			time(&time1);
			time2 = time1;
		}
		if(time1 != 0)
			time(&time1);

		if(time1 == 0 || difftime(time1, time2) >= 1.0)
		{
			if(time1 != 0)
				SetEffect(EFFECT_FADE, 20);
			time1 = 0;
			goto draw;
		}
	}
	else
	{
		if(time2 != 0 && time1 == 0)
			SetEffect(EFFECT_FADE, -20);
		time2 = 0;
	}

	if(GetEffect())
	{
		draw:
		leftImage.Draw();
		tileImage.Draw();
		rightImage.Draw();
		text->Draw();
	}

	this->UpdateEffects();
}