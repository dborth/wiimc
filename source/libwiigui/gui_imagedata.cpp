/****************************************************************************
 * libwiigui
 *
 * Tantric 2009
 *
 * gui_imagedata.cpp
 *
 * GUI class definitions
 ***************************************************************************/

#include "../utils/pngu.h"
#include "../utils/jmemsrc.h"
#include "../utils/easybmp.h"
#include "gui.h"

/**
 * Constructor for the GuiImageData class.
 */
GuiImageData::GuiImageData(const u8 * i)
{
	data = NULL;
	width = 0;
	height = 0;

	if(i)
		LoadPNG(i);
}

GuiImageData::GuiImageData(const u8 * i, int s)
{
	data = NULL;
	width = 0;
	height = 0;

	if(i)
	{
		if (s < 8)
			return;
		if (i[0] == 0xFF && i[1] == 0xD8)
			LoadJPEG(i, s);
		else if (i[0] == 'B' && i[1] == 'M')
			LoadBMP(i, s);
		else if (i[0] == 0x89 && i[1] == 'P' && i[2] == 'N' && i[3] == 'G')
			LoadPNG(i);
	}
}

/**
 * Destructor for the GuiImageData class.
 */
GuiImageData::~GuiImageData()
{
	if(data)
	{
		free(data);
		data = NULL;
	}
}

void GuiImageData::LoadPNG(const u8 *i)
{
	PNGUPROP imgProp;
	IMGCTX ctx = PNGU_SelectImageFromBuffer(i);

	if(!ctx)
		return;

	int res = PNGU_GetImageProperties(ctx, &imgProp);

	if(res == PNGU_OK)
	{
		int len = imgProp.imgWidth * imgProp.imgHeight * 4;
		if(len%32) len += (32-len%32);
		data = (u8 *)memalign (32, len);

		if(data)
		{
			res = PNGU_DecodeTo4x4RGBA8 (ctx, imgProp.imgWidth, imgProp.imgHeight, data, 255);

			if(res == PNGU_OK)
			{
				width = imgProp.imgWidth;
				height = imgProp.imgHeight;
				DCFlushRange(data, len);
			}
			else
			{
				free(data);
				data = NULL;
			}
		}
	}
	PNGU_ReleaseImageContext (ctx);
}

void GuiImageData::LoadJPEG(const u8 *i, int s)
{
	data = DecodeJPEG(i, s, &width, &height);
}

void GuiImageData::LoadBMP(const u8 *i, int s)
{
	data = DecodeBMP(i, s, &width, &height);
}

u8 * GuiImageData::GetImage()
{
	return data;
}

int GuiImageData::GetWidth()
{
	return width;
}

int GuiImageData::GetHeight()
{
	return height;
}
