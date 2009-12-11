/****************************************************************************
*	gx_supp.c - Generic GX Support for Emulators
*	softdev 2007
*	dhewg 2008
*	sepp256 2008 - Coded YUV->RGB conversion in TEV.
*	Tantric 2009 - rewritten using threads, with GUI overlaid
*
*	This program is free software; you can redistribute it and/or modify
*	it under the terms of the GNU General Public License as published by
*	the Free Software Foundation; either version 2 of the License, or
*	(at your option) any later version.
*
*	This program is distributed in the hope that it will be useful,
*	but WITHOUT ANY WARRANTY; without even the implied warranty of
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*	GNU General Public License for more details.
*
*	You should have received a copy of the GNU General Public License along
*	with this program; if not, write to the Free Software Foundation, Inc.,
*	51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*
* NGC GX Video Functions
*
* These are pretty standard functions to setup and use GX scaling.
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <unistd.h>
#include <math.h>
#include <ogc/mutex.h>
#include <ogc/lwp.h>
#include "gx_supp.h"

#ifdef WIILIB
#include <wiiuse/wpad.h>
#endif

#ifdef WIILIB
u64 frameCounter = 0;
#endif
#define DEFAULT_FIFO_SIZE (256 * 1024)

#define HASPECT 320
#define VASPECT 240

static u16 currentWidth = 0;
static u16 * currentPitch = NULL;
static int drawMode = 0;

#ifdef WIILIB
void StartDrawThread();
void PauseAndGotoGUI();
void ShutdownGui();
void TakeScreenshot();
int DrawMPlayerGui();
int copyScreen = 0;
extern int pause_gui;
#endif

/*** 2D ***/
#ifdef WIILIB
extern u32 whichfb;
extern u32 *xfb[2];

extern int screenwidth;
extern int screenheight;
#else
static u32 whichfb;
static u32 *xfb[2];
GXRModeObj *vmode = NULL;

int screenwidth = 640;
int screenheight = 480;
#endif
static u32 whichtex=0;

//static bool component_fix=false;
static int hor_pos=0, vert_pos=0, stretch=0;

/*** 3D GX ***/
#ifndef WIILIB
static u8 *gp_fifo;
#endif

/*** Texture memory ***/
static u8 *Ytexture[2] = {NULL,NULL};
static u8 *Utexture[2] = {NULL,NULL};
static u8 *Vtexture[2] = {NULL,NULL};

static u32 Ytexsize,UVtexsize;

static GXTexObj YtexObj[2],UtexObj[2],VtexObj[2];
static Mtx view;
static u16 vwidth, vheight, oldvwidth, oldvheight, oldpitch;
static u16 Ywidth, Yheight, UVwidth, UVheight;

/* New texture based scaler */
typedef struct tagcamera {
	guVector pos;
	guVector up;
	guVector view;
} camera;

static s16 square[] ATTRIBUTE_ALIGN(32) = {
	-HASPECT, VASPECT, 0,
	HASPECT, VASPECT, 0,
	HASPECT, -VASPECT, 0,
	-HASPECT, -VASPECT, 0,
};

static GXColor colors[] ATTRIBUTE_ALIGN(32) = {
	{0,255,0,255}		//G
};

static u8 texcoords[] ATTRIBUTE_ALIGN(32) = {
	0x00, 0x00,
	0x01, 0x00,
	0x01, 0x01,
	0x00, 0x01,
};

static camera cam = {
	{ 0.0f, 0.0f, 352.0f },
	{ 0.0f, 0.5f, 0.0f },
	{ 0.0f, 0.0f, -0.5f }
};
#ifndef WIILIB
int video_mode=0;
void ChangeVideoMode(int video_mode)
{
	switch(video_mode)
	{
		case 1: // NTSC (480i)
			vmode = &TVNtsc480IntDf;
				break;
		case 2: // Progressive (480p)
			vmode = &TVNtsc480Prog;
				break;
		case 3: // PAL (50Hz)
			vmode = &TVPal574IntDfScale;
				break;
		case 4: // PAL (60Hz)
			vmode = &TVEurgb60Hz480IntDf;
				break;
		default:
			return;
	}
	
	vmode->viHeight *= 0.95; vmode->viHeight += fmod(vmode->viHeight, 2);
	screenheight = vmode->efbHeight;
	
	if (CONF_GetAspectRatio() == CONF_ASPECT_16_9)
	{
        vmode->viWidth = VI_MAX_WIDTH_NTSC * 0.95;
		screenwidth = (int)(((float)screenheight / 9) * 16);
	}
	else
	{
        vmode->viWidth = VI_MAX_WIDTH_NTSC * 0.93;
		screenwidth = (int)(((float)screenheight / 3) * 4);
	}
	
	screenwidth -= fmod(screenwidth, 4);
	
	vmode->xfbHeight *= 0.95; vmode->xfbHeight += fmod(vmode->xfbHeight, 2);
	vmode->efbHeight *= 0.95; vmode->efbHeight += fmod(vmode->efbHeight, 2);
	
	vmode->viXOrigin = (VI_MAX_WIDTH_NTSC - vmode->viWidth) / 2;
	vmode->viYOrigin = (screenheight - vmode->viHeight) / 2;

	VIDEO_Configure(vmode);
	VIDEO_Flush();

	free(MEM_K1_TO_K0(xfb[0]));
	free(MEM_K1_TO_K0(xfb[1]));

	xfb[0] = (u32 *) MEM_K0_TO_K1 (SYS_AllocateFramebuffer(vmode));
	xfb[1] = (u32 *) MEM_K0_TO_K1 (SYS_AllocateFramebuffer(vmode));

	VIDEO_ClearFrameBuffer(vmode, xfb[0], COLOR_BLACK);
	VIDEO_ClearFrameBuffer(vmode, xfb[1], COLOR_BLACK);

	VIDEO_SetNextFramebuffer(xfb[whichfb]);
	VIDEO_SetBlack(FALSE);
	VIDEO_Flush();
	VIDEO_WaitVSync();

	if (vmode->viTVMode & VI_NON_INTERLACE)
		VIDEO_WaitVSync();
	else
	    while (VIDEO_GetNextField())
	    	VIDEO_WaitVSync();


}
void GX_InitVideo()
{
	vmode = VIDEO_GetPreferredMode(NULL);
	
	vmode->viHeight *= 0.95; vmode->viHeight += fmod(vmode->viHeight, 2);
	screenheight = vmode->efbHeight;
	
	if (CONF_GetAspectRatio() == CONF_ASPECT_16_9)
	{
        vmode->viWidth = VI_MAX_WIDTH_NTSC * 0.95;
		screenwidth = (int)(((float)screenheight / 9) * 16);
	}
	else
	{
        vmode->viWidth = VI_MAX_WIDTH_NTSC * 0.93;
		screenwidth = (int)(((float)screenheight / 3) * 4);
	}
	
	screenwidth -= fmod(screenwidth, 4);
	
	vmode->xfbHeight *= 0.95; vmode->xfbHeight += fmod(vmode->xfbHeight, 2);
	vmode->efbHeight *= 0.95; vmode->efbHeight += fmod(vmode->efbHeight, 2);
	
	vmode->viXOrigin = (VI_MAX_WIDTH_NTSC - vmode->viWidth) / 2;
	vmode->viYOrigin = (screenheight - vmode->viHeight) / 2;

	VIDEO_Configure(vmode);

	xfb[0] = (u32 *) MEM_K0_TO_K1 (SYS_AllocateFramebuffer(vmode));
	xfb[1] = (u32 *) MEM_K0_TO_K1 (SYS_AllocateFramebuffer(vmode));
	gp_fifo = (u8 *) memalign(32, DEFAULT_FIFO_SIZE);

	VIDEO_ClearFrameBuffer(vmode, xfb[0], COLOR_BLACK);
	VIDEO_ClearFrameBuffer(vmode, xfb[1], COLOR_BLACK);

	whichfb = 0;
	VIDEO_SetNextFramebuffer(xfb[whichfb]);
	VIDEO_SetBlack(FALSE);
	VIDEO_Flush();
	VIDEO_WaitVSync();

	if (vmode->viTVMode & VI_NON_INTERLACE)
		VIDEO_WaitVSync();
	else
	    while (VIDEO_GetNextField())
	    	VIDEO_WaitVSync();

	//make texture memory fixed (max texture 900*700, gx can't manage more) and in mem1 (is faster)
	if (!Ytexture[0])
		Ytexture[0] = (u8 *) memalign(32,900*700);
	if (!Utexture[0])
		Utexture[0] = (u8 *) memalign(32,900*700/4);
	if (!Vtexture[0])
		Vtexture[0] = (u8 *) memalign(32,900*700/4);
}
#endif
void GX_SetScreenPos(int _hor_pos,int _vert_pos, int _stretch)
{
	hor_pos = _hor_pos;
	vert_pos = _vert_pos;
	stretch = _stretch;
}

void GX_SetCamPosZ(float f)
{
	cam.pos.z = f;
}

/****************************************************************************
 * draw_initYUV - Internal function to setup TEV for YUV->RGB conversion.
 ****************************************************************************/
static void draw_initYUV(void)
{
	//Setup TEV
	GX_SetNumChans (1);
	GX_SetNumTexGens (3);
	GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);
	GX_SetTexCoordGen(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX1, GX_IDENTITY);

	//Y'UV->RGB formulation 2
	GX_SetNumTevStages(12);
	GX_SetTevKColor(GX_KCOLOR0, (GXColor) {255,   0,   0,  19});	//R {1, 0, 0, 16*1.164}
	GX_SetTevKColor(GX_KCOLOR1, (GXColor) {  0,   0, 255,  42});	//B {0, 0, 1, 0.164}
	GX_SetTevKColor(GX_KCOLOR2, (GXColor) {204,  104,   0, 255});	// {1.598/2, 0.813/2, 0}
	GX_SetTevKColor(GX_KCOLOR3, (GXColor) {  0,  25, 129, 255});	// {0, 0.391/4, 2.016/4}
	//Stage 0: TEVREG0 <- { 0, 2Um, 2Up }; TEVREG0A <- {16*1.164}
	GX_SetTevKColorSel(GX_TEVSTAGE0,GX_TEV_KCSEL_K1);
	GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD1, GX_TEXMAP1,GX_COLOR0A0);
	GX_SetTevColorIn (GX_TEVSTAGE0, GX_CC_RASC, GX_CC_KONST, GX_CC_TEXC, GX_CC_ZERO);
	GX_SetTevColorOp (GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_SUBHALF, GX_CS_SCALE_2, GX_ENABLE, GX_TEVREG0);
	GX_SetTevKAlphaSel(GX_TEVSTAGE0,GX_TEV_KASEL_K0_A);
	GX_SetTevAlphaIn (GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_RASA, GX_CA_KONST, GX_CA_ZERO);
	GX_SetTevAlphaOp (GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVREG0);
	//Stage 1: TEVREG1 <- { 0, 2Up, 2Um };
	GX_SetTevKColorSel(GX_TEVSTAGE1,GX_TEV_KCSEL_K1);
	GX_SetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP1,GX_COLOR0A0);
	GX_SetTevColorIn (GX_TEVSTAGE1, GX_CC_KONST, GX_CC_RASC, GX_CC_TEXC, GX_CC_ZERO);
	GX_SetTevColorOp (GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_SUBHALF, GX_CS_SCALE_2, GX_ENABLE, GX_TEVREG1);
	GX_SetTevAlphaIn (GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
	GX_SetTevAlphaOp (GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);
	//Stage 2: TEVREG2 <- { Vp, Vm, 0 }
	GX_SetTevKColorSel(GX_TEVSTAGE2,GX_TEV_KCSEL_K0);
	GX_SetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD1, GX_TEXMAP2,GX_COLOR0A0);
	GX_SetTevColorIn (GX_TEVSTAGE2, GX_CC_RASC, GX_CC_KONST, GX_CC_TEXC, GX_CC_ZERO);
	GX_SetTevColorOp (GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_SUBHALF, GX_CS_SCALE_1, GX_ENABLE, GX_TEVREG2);
	GX_SetTevAlphaIn (GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
	GX_SetTevAlphaOp (GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);
	//Stage 3: TEVPREV <- { (Vm), (Vp), 0 }
	GX_SetTevKColorSel(GX_TEVSTAGE3,GX_TEV_KCSEL_K0);
	GX_SetTevOrder(GX_TEVSTAGE3, GX_TEXCOORD1, GX_TEXMAP2,GX_COLOR0A0);
	GX_SetTevColorIn (GX_TEVSTAGE3, GX_CC_KONST, GX_CC_RASC, GX_CC_TEXC, GX_CC_ZERO);
	GX_SetTevColorOp (GX_TEVSTAGE3, GX_TEV_ADD, GX_TB_SUBHALF, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);
	GX_SetTevAlphaIn (GX_TEVSTAGE3, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
	GX_SetTevAlphaOp (GX_TEVSTAGE3, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);
	//Stage 4: TEVPREV <- { (-1.598Vm), (-0.813Vp), 0 }; TEVPREVA <- {Y' - 16*1.164}
	GX_SetTevKColorSel(GX_TEVSTAGE4,GX_TEV_KCSEL_K2);
	GX_SetTevOrder(GX_TEVSTAGE4, GX_TEXCOORD0, GX_TEXMAP0,GX_COLORNULL);
	GX_SetTevColorIn (GX_TEVSTAGE4, GX_CC_ZERO, GX_CC_KONST, GX_CC_CPREV, GX_CC_ZERO);
	GX_SetTevColorOp (GX_TEVSTAGE4, GX_TEV_SUB, GX_TB_ZERO, GX_CS_SCALE_2, GX_DISABLE, GX_TEVPREV);
	GX_SetTevKAlphaSel(GX_TEVSTAGE4,GX_TEV_KASEL_1);
	GX_SetTevAlphaIn (GX_TEVSTAGE4, GX_CA_ZERO, GX_CA_KONST, GX_CA_A0, GX_CA_TEXA);
	GX_SetTevAlphaOp (GX_TEVSTAGE4, GX_TEV_SUB, GX_TB_ZERO, GX_CS_SCALE_1, GX_DISABLE, GX_TEVPREV);
	//Stage 5: TEVPREV <- { -1.598Vm (+1.139/2Vp), -0.813Vp +0.813/2Vm), 0 }; TEVREG1A <- {Y' -16*1.164 - Y'*0.164} = {(Y'-16)*1.164}
	GX_SetTevKColorSel(GX_TEVSTAGE5,GX_TEV_KCSEL_K2);
	GX_SetTevOrder(GX_TEVSTAGE5, GX_TEXCOORD0, GX_TEXMAP0,GX_COLORNULL);
	GX_SetTevColorIn (GX_TEVSTAGE5, GX_CC_ZERO, GX_CC_KONST, GX_CC_C2, GX_CC_CPREV);
	GX_SetTevColorOp (GX_TEVSTAGE5, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_DISABLE, GX_TEVPREV);
	GX_SetTevKAlphaSel(GX_TEVSTAGE5,GX_TEV_KASEL_K1_A);
	GX_SetTevAlphaIn (GX_TEVSTAGE5, GX_CA_ZERO, GX_CA_KONST, GX_CA_TEXA, GX_CA_APREV);
	GX_SetTevAlphaOp (GX_TEVSTAGE5, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVREG1);
	//Stage 6: TEVPREV <- {	-1.598Vm (+1.598Vp), -0.813Vp (+0.813Vm), 0 } = {	(+1.598V), (-0.813V), 0 }
	GX_SetTevKColorSel(GX_TEVSTAGE6,GX_TEV_KCSEL_K2);
	GX_SetTevOrder(GX_TEVSTAGE6, GX_TEXCOORDNULL, GX_TEXMAP_NULL,GX_COLORNULL);
	GX_SetTevColorIn (GX_TEVSTAGE6, GX_CC_ZERO, GX_CC_KONST, GX_CC_C2, GX_CC_CPREV);
	GX_SetTevColorOp (GX_TEVSTAGE6, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_DISABLE, GX_TEVPREV);
	GX_SetTevAlphaIn (GX_TEVSTAGE6, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
	GX_SetTevAlphaOp (GX_TEVSTAGE6, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);
	//Stage 7: TEVPREV <- {	((Y'-16)*1.164) +1.598V, ((Y'-16)*1.164) -0.813V, ((Y'-16)*1.164) }
	GX_SetTevKColorSel(GX_TEVSTAGE7,GX_TEV_KCSEL_1);
	GX_SetTevOrder(GX_TEVSTAGE7, GX_TEXCOORDNULL, GX_TEXMAP_NULL,GX_COLORNULL);
	GX_SetTevColorIn (GX_TEVSTAGE7, GX_CC_ZERO, GX_CC_ONE, GX_CC_A1, GX_CC_CPREV);
	GX_SetTevColorOp (GX_TEVSTAGE7, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_DISABLE, GX_TEVPREV);
	GX_SetTevAlphaIn (GX_TEVSTAGE7, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
	GX_SetTevAlphaOp (GX_TEVSTAGE7, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);
	//Stage 8: TEVPREV <- {	(Y'-16)*1.164 +1.598V, (Y'-16)*1.164 -0.813V (-.394/2Up), (Y'-16)*1.164 (-2.032/2Um)}
	GX_SetTevKColorSel(GX_TEVSTAGE8,GX_TEV_KCSEL_K3);
	GX_SetTevOrder(GX_TEVSTAGE8, GX_TEXCOORDNULL, GX_TEXMAP_NULL,GX_COLORNULL);
	GX_SetTevColorIn (GX_TEVSTAGE8, GX_CC_ZERO, GX_CC_KONST, GX_CC_C1, GX_CC_CPREV);
	GX_SetTevColorOp (GX_TEVSTAGE8, GX_TEV_SUB, GX_TB_ZERO, GX_CS_SCALE_1, GX_DISABLE, GX_TEVPREV);
	GX_SetTevAlphaIn (GX_TEVSTAGE8, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
	GX_SetTevAlphaOp (GX_TEVSTAGE8, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);
	//Stage 9: TEVPREV <- { (Y'-16)*1.164 +1.598V, (Y'-16)*1.164 -0.813V (-.394Up), (Y'-16)*1.164 (-2.032Um)}
	GX_SetTevKColorSel(GX_TEVSTAGE9,GX_TEV_KCSEL_K3);
	GX_SetTevOrder(GX_TEVSTAGE9, GX_TEXCOORDNULL, GX_TEXMAP_NULL,GX_COLORNULL);
	GX_SetTevColorIn (GX_TEVSTAGE9, GX_CC_ZERO, GX_CC_KONST, GX_CC_C1, GX_CC_CPREV);
	GX_SetTevColorOp (GX_TEVSTAGE9, GX_TEV_SUB, GX_TB_ZERO, GX_CS_SCALE_1, GX_DISABLE, GX_TEVPREV);
	GX_SetTevAlphaIn (GX_TEVSTAGE9, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
	GX_SetTevAlphaOp (GX_TEVSTAGE9, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);
	//Stage 10: TEVPREV <- { (Y'-16)*1.164 +1.598V, (Y'-16)*1.164 -0.813V -.394Up (+.394/2Um), (Y'-16)*1.164 -2.032Um (+2.032/2Up)}
	GX_SetTevKColorSel(GX_TEVSTAGE10,GX_TEV_KCSEL_K3);
	GX_SetTevOrder(GX_TEVSTAGE10, GX_TEXCOORDNULL, GX_TEXMAP_NULL,GX_COLORNULL);
	GX_SetTevColorIn (GX_TEVSTAGE10, GX_CC_ZERO, GX_CC_KONST, GX_CC_C0, GX_CC_CPREV);
	GX_SetTevColorOp (GX_TEVSTAGE10, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_DISABLE, GX_TEVPREV);
	GX_SetTevAlphaIn (GX_TEVSTAGE10, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
	GX_SetTevAlphaOp (GX_TEVSTAGE10, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);
	//Stage 11: TEVPREV <- { (Y'-16)*1.164 +1.598V, (Y'-16)*1.164 -0.813V -.394Up (+.394Um), (Y'-16)*1.164 -2.032Um (+2.032Up)} = { (Y'-16)*1.164 +1.139V, (Y'-16)*1.164 -0.58V -.394U, (Y'-16)*1.164 +2.032U}
	GX_SetTevKColorSel(GX_TEVSTAGE11,GX_TEV_KCSEL_K3);
	GX_SetTevOrder(GX_TEVSTAGE11, GX_TEXCOORDNULL, GX_TEXMAP_NULL,GX_COLORNULL);
	GX_SetTevColorIn (GX_TEVSTAGE11, GX_CC_ZERO, GX_CC_KONST, GX_CC_C0, GX_CC_CPREV);
	GX_SetTevColorOp (GX_TEVSTAGE11, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);
	GX_SetTevKAlphaSel(GX_TEVSTAGE11,GX_TEV_KASEL_1);
	GX_SetTevAlphaIn (GX_TEVSTAGE11, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_KONST);
	GX_SetTevAlphaOp (GX_TEVSTAGE11, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);

	//Setup blending
	GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR); //Fix src alpha
	GX_SetColorUpdate(GX_ENABLE);
	GX_SetAlphaUpdate(GX_ENABLE);

	//Setup vertex description/format
	GX_ClearVtxDesc();
	GX_SetVtxDesc(GX_VA_POS, GX_INDEX8);
	GX_SetVtxDesc(GX_VA_CLR0, GX_INDEX8);
	GX_SetVtxDesc(GX_VA_TEX0, GX_INDEX8);
	GX_SetVtxDesc(GX_VA_TEX1, GX_INDEX8);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_S16, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_U8, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX1, GX_TEX_ST, GX_U8, 0);

	GX_SetArray(GX_VA_POS, square, 3 * sizeof(s16));
	GX_SetArray(GX_VA_CLR0, colors, sizeof(GXColor));
	GX_SetArray(GX_VA_TEX0, texcoords, 2 * sizeof(u8));
	GX_SetArray(GX_VA_TEX1, texcoords, 2 * sizeof(u8));

	//init YUV texture objects
	GX_InitTexObj(&YtexObj[0], Ytexture[0], (u16) Ywidth, (u16) Yheight, GX_TF_I8, GX_CLAMP, GX_CLAMP, GX_FALSE);
	GX_InitTexObj(&UtexObj[0], Utexture[0], (u16) UVwidth, (u16) UVheight, GX_TF_I8, GX_CLAMP, GX_CLAMP, GX_FALSE);
	GX_InitTexObj(&VtexObj[0], Vtexture[0], (u16) UVwidth, (u16) UVheight, GX_TF_I8, GX_CLAMP, GX_CLAMP, GX_FALSE);

#ifdef WIILIB
	GX_InitTexObj(&YtexObj[1], Ytexture[1], (u16) Ywidth, (u16) Yheight, GX_TF_I8, GX_CLAMP, GX_CLAMP, GX_FALSE);
	GX_InitTexObj(&UtexObj[1], Utexture[1], (u16) UVwidth, (u16) UVheight, GX_TF_I8, GX_CLAMP, GX_CLAMP, GX_FALSE);
	GX_InitTexObj(&VtexObj[1], Vtexture[1], (u16) UVwidth, (u16) UVheight, GX_TF_I8, GX_CLAMP, GX_CLAMP, GX_FALSE);
#endif	
}

//------- rodries change: to avoid image_buffer intermediate ------
static int w1,w2,h1,h2,df1,df2,old_h1_2=-1;
static int p01,p02,p03,p11,p12,p13;
static u16 Yrowpitch;
static u16 UVrowpitch;
static u64 *Ydst, *Udst, *Vdst;

void getStrideInfo(int *_w1,int *_df1,int *_Yrowpitch)  // for subtitle info
{
	*_w1=w1;
	*_df1=df1;
	*_Yrowpitch=Yrowpitch;
}

static void draw_scaling()
{
	Mtx m, mv;
	
	memset(&view, 0, sizeof(Mtx));
	guLookAt(view, &cam.pos, &cam.up, &cam.view);
	guMtxIdentity(m);
	guMtxTransApply(m, m, 0, 0, -100);
	guMtxConcat(view, m, mv);
	GX_LoadPosMtxImm(mv, GX_PNMTX0);
	GX_SetViewport(0, 0, vmode->fbWidth, vmode->efbHeight, 0, 1);
}

void GX_ConfigTextureYUV(u16 width, u16 height, u16 *pitch)
{
	int wp,ww;
	
	GX_ResetTextureYUVPointers();

	wp=pitch[0];
	ww=width;

	ww= (ww / 16);
	if(ww % 2) ww++;
	ww=ww*16;

	if(wp>ww)wp=ww;
    w1 = wp >> 3 ;
    w2 = wp >> 4 ;

    df1 = ((ww >> 3) - w1)*4;
    df2 = ((ww >> 4) - w2)*4;

    UVrowpitch = pitch[1]/2-w2;
    Yrowpitch = pitch[0]/2-w1;

  	vwidth = width;

	Ywidth = ww;
	UVwidth = ww>>1;

	vheight = height;
	Yheight = vheight;
	UVheight = vheight>>1;

	/** Update scaling **/
	oldvwidth = vwidth;
	oldvheight = vheight;
	draw_initYUV();
	draw_scaling();

	p01= pitch[0];
    p02= pitch[0] * 2;
    p03= pitch[0] * 3;
    p11= pitch[1];
    p12= pitch[1] * 2;
    p13= pitch[1] * 3;
    
    GX_UpdateSquare();
}

void GX_UpdatePitch(int width,u16 *pitch)
{
	//black
    memset(Ytexture[0], 0, Ytexsize);
	memset(Utexture[0], 0x80, UVtexsize);
	memset(Vtexture[0], 0x80, UVtexsize);
#ifdef WIILIB	
    memset(Ytexture[1], 0, Ytexsize);
	memset(Utexture[1], 0x80, UVtexsize);
	memset(Vtexture[1], 0x80, UVtexsize);
#endif
	currentWidth = width;
	currentPitch = pitch;
	GX_ConfigTextureYUV(width, vheight, pitch);
}

void DrawMPlayer()
{
	// render textures
#ifdef WIILIB	
	static u32 last_frame=-1;
	u32 frame=whichtex^1;
#else
	u32 frame=0;	
#endif
	

	GX_InvVtxCache();
	GX_InvalidateTexAll();

#ifdef WIILIB	
	if(last_frame!=frame) //not sure If we get performance here
	{
		last_frame=frame;
#endif
		DCFlushRange(Ytexture[frame], Ytexsize);
		DCFlushRange(Utexture[frame], UVtexsize);
		DCFlushRange(Vtexture[frame], UVtexsize);
#ifdef WIILIB	
	}
#endif

	GX_LoadTexObj(&YtexObj[frame], GX_TEXMAP0);	// MAP0 <- Y
	GX_LoadTexObj(&UtexObj[frame], GX_TEXMAP1);	// MAP1 <- U
	GX_LoadTexObj(&VtexObj[frame], GX_TEXMAP2);	// MAP2 <- V

	GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
		GX_Position1x8(0); GX_Color1x8(0); GX_TexCoord1x8(0); GX_TexCoord1x8(0);
		GX_Position1x8(1); GX_Color1x8(0); GX_TexCoord1x8(1); GX_TexCoord1x8(1);
		GX_Position1x8(2); GX_Color1x8(0); GX_TexCoord1x8(2); GX_TexCoord1x8(2);
		GX_Position1x8(3); GX_Color1x8(0); GX_TexCoord1x8(3); GX_TexCoord1x8(3);
	GX_End();

	GX_SetColorUpdate(GX_TRUE);

	#ifdef WIILIB
	if(copyScreen == 1)
	{
		TakeScreenshot();
		copyScreen = 2;
	}
	else
	{
		drawMode = DrawMPlayerGui();
	}
	#endif

	whichfb ^= 1;
	GX_CopyDisp(xfb[whichfb], GX_TRUE);
	GX_DrawDone();

	VIDEO_SetNextFramebuffer(xfb[whichfb]);
	VIDEO_Flush();

	#ifdef WIILIB
	if(copyScreen == 2)
	{
		copyScreen = 0;
		pause_gui = 1;
	}
	else if(drawMode != 0)
	{
		// reconfigure GX for MPlayer
		Mtx44 p;
		draw_initYUV();
		draw_scaling();
		guOrtho(p, screenheight / 2, -(screenheight / 2), -(screenwidth / 2), screenwidth / 2, 10, 1000);
		GX_LoadProjectionMtx (p, GX_ORTHOGRAPHIC);
		drawMode = 0;
	}
	#endif
}

//nunchuk control
extern float m_screenleft_shift, m_screenright_shift;
extern float m_screentop_shift, m_screenbottom_shift;
static s16 mysquare[12] ATTRIBUTE_ALIGN(32);
void GX_UpdateSquare()
{
	memcpy(mysquare, square, sizeof(square));
	
	mysquare[0] -= m_screenleft_shift*100;
	mysquare[9] -= m_screenleft_shift*100;
	mysquare[3] -= m_screenright_shift*100;
	mysquare[6] -= m_screenright_shift*100;
	mysquare[1] -= m_screentop_shift*100;
	mysquare[4] -= m_screentop_shift*100;
	mysquare[7] -= m_screenbottom_shift*100;
	mysquare[10] -= m_screenbottom_shift*100;
	
	GX_SetArray(GX_VA_POS, mysquare, 3 * sizeof(s16));
//	set_osd_msg(124,1,5000,"fH:%u vH:%i sob:%i st:%i sb:%i",vmode->efbHeight,vmode->viHeight,square[7],mysquare[1],mysquare[7]);
}

/****************************************************************************
 * GX_StartYUV - Initialize GX for given width/height.
 ****************************************************************************/
void GX_StartYUV(u16 width, u16 height, u16 haspect, u16 vaspect)
{
	int w,h;
	Mtx44 p;
	int diffx,diffy;
	
	#ifdef WIILIB
	// tell GUI to shut down, MPlayer is ready to take over
	ShutdownGui();
	#endif

	#ifdef WIILIB
	StartDrawThread();
	#endif

	/*** Set new aspect ***/
	square[0] = square[9] = -haspect;
	square[3] = square[6] = haspect;
	square[1] = square[4] = vaspect;
	square[7] = square[10] = -vaspect;

	/*** Allocate 32byte aligned texture memory ***/

	w = (width / 16);
	if(w % 2) w++;
	w=w*16;
	h = ((int)((height/8.0)))*8;

	//center, to correct difference between pitch and real width
	diffx=width-w;
	diffx+=hor_pos;

	diffy=height-h;

	square[3] -= diffx;
  	square[6] -= diffx;

	square[7] += diffy;
	square[10] += diffy;

	square[1] -= vert_pos;
	square[4] -= vert_pos;
	square[7] -= vert_pos;
	square[10] -= vert_pos;

	square[0] += stretch/2;
  	square[9] += stretch/2;
	square[3] -= stretch/2;
  	square[6] -= stretch/2;

	Ytexsize = (w*h);
	UVtexsize = (w*h)/4;

#ifdef WIILIB
	//make memory fixed (max texture 900*700, gx can't manage more)
	if (!Ytexture[0])
		Ytexture[0] = (u8 *) memalign(32,900*700);
	if (!Utexture[0])
		Utexture[0] = (u8 *) memalign(32,900*700/4);
	if (!Vtexture[0])
		Vtexture[0] = (u8 *) memalign(32,900*700/4);
	if (!Ytexture[1])
		Ytexture[1] = (u8 *) memalign(32,900*700);
	if (!Utexture[1])
		Utexture[1] = (u8 *) memalign(32,900*700/4);
	if (!Vtexture[1])
		Vtexture[1] = (u8 *) memalign(32,900*700/4);
#endif

	memset(Ytexture[0], 0, Ytexsize);
	memset(Utexture[0], 128, UVtexsize);
	memset(Vtexture[0], 128, UVtexsize);
#ifdef WIILIB
	memset(Ytexture[1], 0, Ytexsize);
	memset(Utexture[1], 128, UVtexsize);
	memset(Vtexture[1], 128, UVtexsize);
#endif

	whichtex = 0;

	/*** Setup for first call to scaler ***/
	oldvwidth = oldvheight = oldpitch = -1;

#ifndef WIILIB
	static bool inited = false;
	GXColor gxbackground = { 0, 0, 0, 0xff };

	if (inited)
		return;

	inited = true;

	/*** Clear out FIFO area ***/
	memset(gp_fifo, 0, DEFAULT_FIFO_SIZE);

	/*** Initialise GX ***/
	GX_Init(gp_fifo, DEFAULT_FIFO_SIZE);
	GX_SetCopyClear(gxbackground, 0x00ffffff);
	GX_SetViewport(0, 0, vmode->fbWidth, vmode->efbHeight, 0, 1);
	GX_SetDispCopyYScale((f32) vmode->xfbHeight / (f32) vmode->efbHeight);
	GX_SetScissor(0, 0, vmode->fbWidth, vmode->efbHeight);
	GX_SetDispCopySrc(0, 0, vmode->fbWidth, vmode->efbHeight);
	GX_SetDispCopyDst(vmode->fbWidth, vmode->xfbHeight);
	GX_SetCopyFilter(vmode->aa, vmode->sample_pattern, GX_TRUE, vmode->vfilter);
	GX_SetFieldMode(vmode->field_rendering, ((vmode->viHeight == 2 * vmode->xfbHeight) ? GX_ENABLE : GX_DISABLE));
#endif

	GX_SetPixelFmt(GX_PF_RGB8_Z24, GX_ZC_LINEAR);
	GX_SetCullMode(GX_CULL_NONE);
	GX_CopyDisp(xfb[whichfb ^ 1], GX_TRUE);
	GX_SetDispCopyGamma(GX_GM_1_0);
	guOrtho(p, screenheight / 2, -(screenheight / 2), -(screenwidth / 2), screenwidth / 2, 10, 1000);
	GX_LoadProjectionMtx (p, GX_ORTHOGRAPHIC);

	GX_Flush();
	GX_UpdateSquare();
}

void GX_FillTextureYUV(u16 height,u8 *buffer[3])
{
	int h,w;

	u64 *Ysrc1 = (u64 *) buffer[0];
	u64 *Ysrc2 = (u64 *) (buffer[0] + p01);
	u64 *Ysrc3 = (u64 *) (buffer[0] + p02);
	u64 *Ysrc4 = (u64 *) (buffer[0] + p03);
	u64 *Usrc1 = (u64 *) buffer[1] ;
	u64 *Usrc2 = (u64 *) (buffer[1] + p11);
	u64 *Usrc3 = (u64 *) (buffer[1] + p12);
	u64 *Usrc4 = (u64 *) (buffer[1] + p13);
	u64 *Vsrc1 = (u64 *) buffer[2] ;
	u64 *Vsrc2 = (u64 *) (buffer[2] + p11);
	u64 *Vsrc3 = (u64 *) (buffer[2] + p12);
	u64 *Vsrc4 = (u64 *) (buffer[2] + p13);

	if(height!=old_h1_2)
	{
		old_h1_2 = height;
		h1 = ((height/8)*8) >> 2;
    	h2 = height >> 3 ;
	}

	//Convert YUV frame to GX textures
	//Convert Y plane to texture
	for (h = 0; h < h1; h++)
	{
		for (w = 0; w < w1; w++)
		{
			*Ydst++ = *Ysrc1++;
			*Ydst++ = *Ysrc2++;
			*Ydst++ = *Ysrc3++;
			*Ydst++ = *Ysrc4++;
		}
		Ydst+=df1;
		Ysrc1 += Yrowpitch;
		Ysrc2 += Yrowpitch;
		Ysrc3 += Yrowpitch;
		Ysrc4 += Yrowpitch;
	}

	//Convert U&V planes to textures
	for (h = 0; h < h2; h++)
	{
		for (w = 0; w < w2; w++)
		{
			*Udst++ = *Usrc1++;
			*Udst++ = *Usrc2++;
			*Udst++ = *Usrc3++;
			*Udst++ = *Usrc4++;
			*Vdst++ = *Vsrc1++;
			*Vdst++ = *Vsrc2++;
			*Vdst++ = *Vsrc3++;
			*Vdst++ = *Vsrc4++;
		}
		Udst+=df2;
		Vdst+=df2;
		Usrc1 += UVrowpitch;
		Usrc2 += UVrowpitch;
		Usrc3 += UVrowpitch;
		Usrc4 += UVrowpitch;
		Vsrc1 += UVrowpitch;
		Vsrc2 += UVrowpitch;
		Vsrc3 += UVrowpitch;
		Vsrc4 += UVrowpitch;
	}
}

void GX_RenderTexture()
{
	#ifndef WIILIB
	DrawMPlayer();
	#else
	whichtex ^= 1;
	frameCounter++;
	#endif
}

void GX_ResetTextureYUVPointers()
{
	Ydst = (u64 *) Ytexture[whichtex];
	Udst = (u64 *) Utexture[whichtex];
	Vdst = (u64 *) Vtexture[whichtex];
}

u8* GetYtexture() {return Ytexture[whichtex];}
int GetYrowpitch() {return Yrowpitch;}
int GetYrowpitchDf() {return Yrowpitch+df1;}

/*
void draw_initYUV(void)
{
	//Setup TEV
	GX_SetNumChans (1);
	GX_SetNumTexGens (3);
	GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);
	GX_SetTexCoordGen(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX1, GX_IDENTITY);

	//Y'UV->RGB formulation 1
	GX_SetNumTevStages(12);
	GX_SetTevKColor(GX_KCOLOR0, (GXColor) {255,   0,   0, 255});	//R
	GX_SetTevKColor(GX_KCOLOR1, (GXColor) {  0,   0, 255, 255});	//B
	GX_SetTevKColor(GX_KCOLOR2, (GXColor) {145,  74,   0, 255});	// {1.13982/2, 0.5806/2, 0}
	GX_SetTevKColor(GX_KCOLOR3, (GXColor) {  0,  25, 130, 255});	// {0, 0.39465/4, 2.03211/4}
	//Stage 0: TEVREG0 <- { 0, 2Um, 2Up }
	GX_SetTevKColorSel(GX_TEVSTAGE0,GX_TEV_KCSEL_K1);
	GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD1, GX_TEXMAP1,GX_COLOR0A0);
	GX_SetTevColorIn (GX_TEVSTAGE0, GX_CC_RASC, GX_CC_KONST, GX_CC_TEXC, GX_CC_ZERO);
	GX_SetTevColorOp (GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_SUBHALF, GX_CS_SCALE_2, GX_ENABLE, GX_TEVREG0);
	GX_SetTevAlphaIn (GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
	GX_SetTevAlphaOp (GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);
	//Stage 1: TEVREG1 <- { 0, 2Up, 2Um }
	GX_SetTevKColorSel(GX_TEVSTAGE1,GX_TEV_KCSEL_K1);
	GX_SetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP1,GX_COLOR0A0);
	GX_SetTevColorIn (GX_TEVSTAGE1, GX_CC_KONST, GX_CC_RASC, GX_CC_TEXC, GX_CC_ZERO);
	GX_SetTevColorOp (GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_SUBHALF, GX_CS_SCALE_2, GX_ENABLE, GX_TEVREG1);
	GX_SetTevAlphaIn (GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
	GX_SetTevAlphaOp (GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);
	//Stage 2: TEVREG2 <- { Vp, Vm, 0 }
	GX_SetTevKColorSel(GX_TEVSTAGE2,GX_TEV_KCSEL_K0);
	GX_SetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD1, GX_TEXMAP2,GX_COLOR0A0);
	GX_SetTevColorIn (GX_TEVSTAGE2, GX_CC_RASC, GX_CC_KONST, GX_CC_TEXC, GX_CC_ZERO);
	GX_SetTevColorOp (GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_SUBHALF, GX_CS_SCALE_1, GX_ENABLE, GX_TEVREG2);
	GX_SetTevAlphaIn (GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
	GX_SetTevAlphaOp (GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);
	//Stage 3: TEVPREV <- { (Vm), (Vp), 0 }
	GX_SetTevKColorSel(GX_TEVSTAGE3,GX_TEV_KCSEL_K0);
	GX_SetTevOrder(GX_TEVSTAGE3, GX_TEXCOORD1, GX_TEXMAP2,GX_COLOR0A0);
	GX_SetTevColorIn (GX_TEVSTAGE3, GX_CC_KONST, GX_CC_RASC, GX_CC_TEXC, GX_CC_ZERO);
	GX_SetTevColorOp (GX_TEVSTAGE3, GX_TEV_ADD, GX_TB_SUBHALF, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);
	GX_SetTevAlphaIn (GX_TEVSTAGE3, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
	GX_SetTevAlphaOp (GX_TEVSTAGE3, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);
	//Stage 4: TEVPREV <- { (-1.139Vm), (-0.58Vp), 0 }
	GX_SetTevKColorSel(GX_TEVSTAGE4,GX_TEV_KCSEL_K2);
	GX_SetTevOrder(GX_TEVSTAGE4, GX_TEXCOORDNULL, GX_TEXMAP_NULL,GX_COLORNULL);
	GX_SetTevColorIn (GX_TEVSTAGE4, GX_CC_ZERO, GX_CC_KONST, GX_CC_CPREV, GX_CC_ZERO);
	GX_SetTevColorOp (GX_TEVSTAGE4, GX_TEV_SUB, GX_TB_ZERO, GX_CS_SCALE_2, GX_DISABLE, GX_TEVPREV);
	GX_SetTevAlphaIn (GX_TEVSTAGE4, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
	GX_SetTevAlphaOp (GX_TEVSTAGE4, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);
	//Stage 5: TEVPREV <- { (Y') -1.139Vm, (Y') -0.58Vp, (Y') }
	GX_SetTevKColorSel(GX_TEVSTAGE5,GX_TEV_KCSEL_1);
	GX_SetTevOrder(GX_TEVSTAGE5, GX_TEXCOORD0, GX_TEXMAP0,GX_COLORNULL);
	GX_SetTevColorIn (GX_TEVSTAGE5, GX_CC_ZERO, GX_CC_ONE, GX_CC_TEXC, GX_CC_CPREV);
	GX_SetTevColorOp (GX_TEVSTAGE5, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_DISABLE, GX_TEVPREV);
	GX_SetTevAlphaIn (GX_TEVSTAGE5, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
	GX_SetTevAlphaOp (GX_TEVSTAGE5, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);
	//Stage 6: TEVPREV <- {	Y' -1.139Vm (+1.139/2Vp), Y' -0.58Vp (+0.58/2Vm), Y' }
	GX_SetTevKColorSel(GX_TEVSTAGE6,GX_TEV_KCSEL_K2);
	GX_SetTevOrder(GX_TEVSTAGE6, GX_TEXCOORDNULL, GX_TEXMAP_NULL,GX_COLORNULL);
	GX_SetTevColorIn (GX_TEVSTAGE6, GX_CC_ZERO, GX_CC_KONST, GX_CC_C2, GX_CC_CPREV);
	GX_SetTevColorOp (GX_TEVSTAGE6, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_DISABLE, GX_TEVPREV);
	GX_SetTevAlphaIn (GX_TEVSTAGE6, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
	GX_SetTevAlphaOp (GX_TEVSTAGE6, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);
	//Stage 7: TEVPREV <- {	Y' -1.139Vm (+1.139Vp), Y' -0.58Vp (+0.58Vm), Y' } = {	Y' (+1.139V), Y' (-0.58V), Y' }
	GX_SetTevKColorSel(GX_TEVSTAGE7,GX_TEV_KCSEL_K2);
	GX_SetTevOrder(GX_TEVSTAGE7, GX_TEXCOORDNULL, GX_TEXMAP_NULL,GX_COLORNULL);
	GX_SetTevColorIn (GX_TEVSTAGE7, GX_CC_ZERO, GX_CC_KONST, GX_CC_C2, GX_CC_CPREV);
	GX_SetTevColorOp (GX_TEVSTAGE7, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_DISABLE, GX_TEVPREV);
	GX_SetTevAlphaIn (GX_TEVSTAGE7, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
	GX_SetTevAlphaOp (GX_TEVSTAGE7, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);
	//Stage 8: TEVPREV <- {	Y' +1.139V, Y' -0.58V (-.394/2Up), Y' (-2.032/2Um)}
	GX_SetTevKColorSel(GX_TEVSTAGE8,GX_TEV_KCSEL_K3);
	GX_SetTevOrder(GX_TEVSTAGE8, GX_TEXCOORDNULL, GX_TEXMAP_NULL,GX_COLORNULL);
	GX_SetTevColorIn (GX_TEVSTAGE8, GX_CC_ZERO, GX_CC_KONST, GX_CC_C1, GX_CC_CPREV);
	GX_SetTevColorOp (GX_TEVSTAGE8, GX_TEV_SUB, GX_TB_ZERO, GX_CS_SCALE_1, GX_DISABLE, GX_TEVPREV);
	GX_SetTevAlphaIn (GX_TEVSTAGE8, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
	GX_SetTevAlphaOp (GX_TEVSTAGE8, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);
	//Stage 9: TEVPREV <- { Y' +1.139V, Y' -0.58V (-.394Up), Y' (-2.032Um)}
	GX_SetTevKColorSel(GX_TEVSTAGE9,GX_TEV_KCSEL_K3);
	GX_SetTevOrder(GX_TEVSTAGE9, GX_TEXCOORDNULL, GX_TEXMAP_NULL,GX_COLORNULL);
	GX_SetTevColorIn (GX_TEVSTAGE9, GX_CC_ZERO, GX_CC_KONST, GX_CC_C1, GX_CC_CPREV);
	GX_SetTevColorOp (GX_TEVSTAGE9, GX_TEV_SUB, GX_TB_ZERO, GX_CS_SCALE_1, GX_DISABLE, GX_TEVPREV);
	GX_SetTevAlphaIn (GX_TEVSTAGE9, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
	GX_SetTevAlphaOp (GX_TEVSTAGE9, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);
	//Stage 10: TEVPREV <- { Y' +1.139V, Y' -0.58V -.394Up (+.394/2Um), Y' -2.032Um (+2.032/2Up)}
	GX_SetTevKColorSel(GX_TEVSTAGE10,GX_TEV_KCSEL_K3);
	GX_SetTevOrder(GX_TEVSTAGE10, GX_TEXCOORDNULL, GX_TEXMAP_NULL,GX_COLORNULL);
	GX_SetTevColorIn (GX_TEVSTAGE10, GX_CC_ZERO, GX_CC_KONST, GX_CC_C0, GX_CC_CPREV);
	GX_SetTevColorOp (GX_TEVSTAGE10, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_DISABLE, GX_TEVPREV);
	GX_SetTevAlphaIn (GX_TEVSTAGE10, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
	GX_SetTevAlphaOp (GX_TEVSTAGE10, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);
	//Stage 11: TEVPREV <- { Y' +1.139V, Y' -0.58V -.394Up (+.394Um), Y' -2.032Um (+2.032Up)} = { Y' +1.139V, Y' -0.58V -.394U, Y' +2.032U}
	GX_SetTevKColorSel(GX_TEVSTAGE11,GX_TEV_KCSEL_K3);
	GX_SetTevOrder(GX_TEVSTAGE11, GX_TEXCOORDNULL, GX_TEXMAP_NULL,GX_COLORNULL);
	GX_SetTevColorIn (GX_TEVSTAGE11, GX_CC_ZERO, GX_CC_KONST, GX_CC_C0, GX_CC_CPREV);
	GX_SetTevColorOp (GX_TEVSTAGE11, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);
	GX_SetTevKAlphaSel(GX_TEVSTAGE11,GX_TEV_KASEL_1);
	GX_SetTevAlphaIn (GX_TEVSTAGE11, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_KONST);
	GX_SetTevAlphaOp (GX_TEVSTAGE11, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);

	//Setup blending
	GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR); //Fix src alpha
	GX_SetColorUpdate(GX_ENABLE);
	GX_SetAlphaUpdate(GX_ENABLE);

	//Setup vertex description/format
	GX_ClearVtxDesc();
	GX_SetVtxDesc(GX_VA_POS, GX_INDEX8);
	GX_SetVtxDesc(GX_VA_CLR0, GX_INDEX8);
	GX_SetVtxDesc(GX_VA_TEX0, GX_INDEX8);
	GX_SetVtxDesc(GX_VA_TEX1, GX_INDEX8);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_S16, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_U8, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX1, GX_TEX_ST, GX_U8, 0);

	GX_SetArray(GX_VA_POS, square, 3 * sizeof(s16));
	GX_SetArray(GX_VA_CLR0, colors, sizeof(GXColor));
	GX_SetArray(GX_VA_TEX0, texcoords, 2 * sizeof(u8));
	GX_SetArray(GX_VA_TEX1, texcoords, 2 * sizeof(u8));

	//init YUV texture objects
	GX_InitTexObj(&YtexObj, Ytexture, (u16) Ywidth, (u16) Yheight, GX_TF_I8, GX_CLAMP, GX_CLAMP, GX_FALSE);
	GX_InitTexObj(&UtexObj, Utexture, (u16) UVwidth, (u16) UVheight, GX_TF_I8, GX_CLAMP, GX_CLAMP, GX_FALSE);
	GX_InitTexObj(&VtexObj, Vtexture, (u16) UVwidth, (u16) UVheight, GX_TF_I8, GX_CLAMP, GX_CLAMP, GX_FALSE);
}

static void draw_init(void)
{
	GX_ClearVtxDesc();
	GX_SetVtxDesc(GX_VA_POS, GX_INDEX8);
	GX_SetVtxDesc(GX_VA_CLR0, GX_INDEX8);
	GX_SetVtxDesc(GX_VA_TEX0, GX_DIRECT);

	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_S16, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);

	GX_SetArray(GX_VA_POS, square, 3 * sizeof(s16));

	GX_SetNumTexGens(1);
	GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);

	GX_InvalidateTexAll();

	GX_InitTexObj(&texobj, texturemem, vwidth, vheight, GX_TF_RGB565,
					GX_CLAMP, GX_CLAMP, GX_FALSE);
}

static void draw_vert(u8 pos, u8 c, f32 s, f32 t)
{
	GX_Position1x8(pos);
	GX_Color1x8(c);
	GX_TexCoord2f32(s, t);
}

static void draw_square(Mtx v)
{
	Mtx m;
	Mtx mv;

	guMtxIdentity(m);
	guMtxTransApply(m, m, 0, 0, -100);
	guMtxConcat(v, m, mv);

	GX_LoadPosMtxImm(mv, GX_PNMTX0);
	GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
	draw_vert(0, 0, 0.0, 0.0);
	draw_vert(1, 0, 1.0, 0.0);
	draw_vert(2, 0, 1.0, 1.0);
	draw_vert(3, 0, 0.0, 1.0);
	GX_End();
}

void GX_Start(u16 width, u16 height, s16 haspect, s16 vaspect)
{
	static bool inited = false;

	Mtx44 p;
	GXColor gxbackground = { 0, 0, 0, 0xff };

	// Set new aspect
	square[0] = square[9] = -haspect;
	square[3] = square[6] = haspect;
	square[1] = square[4] = vaspect;
	square[7] = square[10] = -vaspect;



	// Allocate 32byte aligned texture memory
	texturesize = (width * height) * 2;
	if (texturemem)
		free (texturemem);

	texturemem = (u8 *) memalign(32, texturesize);

	memset(texturemem, 0, texturesize);

	// Setup for first call to scaler
	oldvwidth = oldvheight = -1;

	if (inited)
		return;

	inited = true;

	// Clear out FIFO area
	memset(gp_fifo, 0, DEFAULT_FIFO_SIZE);

	// Initialise GX
	GX_Init(gp_fifo, DEFAULT_FIFO_SIZE);
	GX_SetCopyClear(gxbackground, 0x00ffffff);

	GX_SetViewport(0, 0, vmode->fbWidth, vmode->efbHeight, 0, 1);
	GX_SetDispCopyYScale((f32) vmode->xfbHeight / (f32) vmode->efbHeight);
	GX_SetScissor(0, 0, vmode->fbWidth, vmode->efbHeight);
	GX_SetDispCopySrc(0, 0, vmode->fbWidth, vmode->efbHeight);
	GX_SetDispCopyDst(vmode->fbWidth, vmode->xfbHeight);
	GX_SetCopyFilter(vmode->aa, vmode->sample_pattern, GX_TRUE,
						vmode->vfilter);
	GX_SetFieldMode(vmode->field_rendering,
					((vmode->viHeight == 2 * vmode->xfbHeight) ?
					GX_ENABLE : GX_DISABLE));
	GX_SetPixelFmt(GX_PF_RGB8_Z24, GX_ZC_LINEAR);
	GX_SetCullMode(GX_CULL_NONE);
	GX_CopyDisp(xfb[whichfb ^ 1], GX_TRUE);
	GX_SetDispCopyGamma(GX_GM_1_0);

	guPerspective(p, 60, 1.33f, 10.0f, 1000.0f);
	GX_LoadProjectionMtx(p, GX_PERSPECTIVE);

	GX_Flush();
}

void GX_Render(u16 width, u16 height, u8 *buffer, u16 pitch)
{
	u16 h, w;
	u64 *dst = (u64 *) texturemem;
	u64 *src1 = (u64 *) buffer;
	u64 *src2 = (u64 *) (buffer + pitch);
	u64 *src3 = (u64 *) (buffer + (pitch * 2));
	u64 *src4 = (u64 *) (buffer + (pitch * 3));
	u16 rowpitch = (pitch >> 3) * 3 + pitch % 8;

	vwidth = width;
	vheight = height;

	whichfb ^= 1;

	if ((oldvheight != vheight) || (oldvwidth != vwidth))
	{
		// Update scaling
		oldvwidth = vwidth;
		oldvheight = vheight;
		draw_init();
		memset(&view, 0, sizeof(Mtx));
		guLookAt(view, &cam.pos, &cam.up, &cam.view);
		GX_SetViewport(0, 0, vmode->fbWidth, vmode->efbHeight, 0, 1);
	}

	GX_InvVtxCache();
	GX_InvalidateTexAll();
	GX_SetTevOp(GX_TEVSTAGE0, GX_DECAL);
	GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);

	for (h = 0; h < vheight; h += 4)
	{
		for (w = 0; w < (vwidth >> 2); w++)
		{
			*dst++ = *src1++;
			*dst++ = *src2++;
			*dst++ = *src3++;
			*dst++ = *src4++;
		}

		src1 += rowpitch;
		src2 += rowpitch;
		src3 += rowpitch;
		src4 += rowpitch;
	}

	DCFlushRange(texturemem, texturesize);

	GX_SetNumChans(1);
	GX_LoadTexObj(&texobj, GX_TEXMAP0);

	draw_square(view);

	GX_DrawDone();

	GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
	GX_SetColorUpdate(GX_TRUE);
	GX_CopyDisp(xfb[whichfb], GX_TRUE);
	GX_Flush();

	VIDEO_SetNextFramebuffer(xfb[whichfb]);
	VIDEO_Flush();
}

void GX_RenderYUV(u16 width, u16 height, u8 *buffer[3], u16 *pitch)
{
	Mtx m, mv;
	u16 h, w;
	u64 *Ydst = (u64 *) Ytexture;
	u64 *Udst = (u64 *) Utexture;
	u64 *Vdst = (u64 *) Vtexture;
	u64 *Ysrc1 = (u64 *) buffer[0];
	u64 *Ysrc2 = (u64 *) (buffer[0] + pitch[0]);
	u64 *Ysrc3 = (u64 *) (buffer[0] + (pitch[0] * 2));
	u64 *Ysrc4 = (u64 *) (buffer[0] + (pitch[0] * 3));
	u64 *Usrc1 = (u64 *) buffer[1];
	u64 *Usrc2 = (u64 *) (buffer[1] + pitch[1]);
	u64 *Usrc3 = (u64 *) (buffer[1] + (pitch[1] * 2));
	u64 *Usrc4 = (u64 *) (buffer[1] + (pitch[1] * 3));
	u64 *Vsrc1 = (u64 *) buffer[2];
	u64 *Vsrc2 = (u64 *) (buffer[2] + pitch[2]);
	u64 *Vsrc3 = (u64 *) (buffer[2] + (pitch[2] * 2));
	u64 *Vsrc4 = (u64 *) (buffer[2] + (pitch[2] * 3));

	static u16 Yrowpitch;// = (pitch[0] >> 3) * 3 + pitch[0] % 8;
	static u16 UVrowpitch;// = (pitch[1] >> 3) * 3 + pitch[1] % 8;

	static int w1,w2,h1,h2,df1,df2;

	if (oldpitch!=pitch[0])
	{
		oldpitch=pitch[0];
		//Yrowpitch = (pitch[0] >> 3) * 3 + pitch[0] % 8;
		//UVrowpitch = (pitch[1] >> 3) * 3 + pitch[1] % 8;

		vwidth = width;
		Ywidth = vwidth;
		UVwidth = vwidth>>1;

		vheight = height;
		Yheight = vheight;
		UVheight = vheight>>1;

		h1 = height / 4 ;
	    h2 = (vheight >> 1) / 4 ;

	    w1 = pitch[0] >> 3 ;
	    w2 = pitch[0] >> 4 ;
	    df1 = ((vwidth >> 3) - w1)*4;
	    df2 = ((vwidth >> 4) - w2)*4;

	    UVrowpitch = pitch[1]/2-w2;
	    Yrowpitch = pitch[0]/2-w1;

    	//black
    	memset(Ytexture, 0, Ytexsize);
		memset(Utexture, 0x80, UVtexsize);
		memset(Vtexture, 0x80, UVtexsize);

		// Update scaling
		oldvwidth = vwidth;
		oldvheight = vheight;
		draw_initYUV();
		memset(&view, 0, sizeof(Mtx));
		guLookAt(view, &cam.pos, &cam.up, &cam.view);
		guMtxIdentity(m);
		guMtxTransApply(m, m, 0, 0, -100);
		guMtxConcat(view, m, mv);
		GX_LoadPosMtxImm(mv, GX_PNMTX0);
		GX_SetViewport(0, 0, vmode->fbWidth, vmode->efbHeight, 0, 1);
	}


	//Convert YUV frame to GX textures
	//Convert Y plane to texture
	//for (h = 0; h < vheight; h+=4) {
	//	for (w = 0; w < (vwidth >>3); w++) {
	for (h = 0; h < h1; h++)
	{
		for (w = 0; w < w1; w++)
		{
			*Ydst++ = *Ysrc1++;
			*Ydst++ = *Ysrc2++;
			*Ydst++ = *Ysrc3++;
			*Ydst++ = *Ysrc4++;
		}
		Ydst+=df1;
		Ysrc1 += Yrowpitch;
		Ysrc2 += Yrowpitch;
		Ysrc3 += Yrowpitch;
		Ysrc4 += Yrowpitch;
	}


	//Convert U&V planes to textures
	//for (h = 0; h < vheight >> 1; h+=4) {
		//for (w = 0; w < (vwidth >> 4); w++) {
	for (h = 0; h < h2; h++)
	{
		for (w = 0; w < w2; w++)
		{
			*Udst++ = *Usrc1++;
			*Udst++ = *Usrc2++;
			*Udst++ = *Usrc3++;
			*Udst++ = *Usrc4++;
			*Vdst++ = *Vsrc1++;
			*Vdst++ = *Vsrc2++;
			*Vdst++ = *Vsrc3++;
			*Vdst++ = *Vsrc4++;
		}
		Udst+=df2;
		Vdst+=df2;
		Usrc1 += UVrowpitch;
		Usrc2 += UVrowpitch;
		Usrc3 += UVrowpitch;
		Usrc4 += UVrowpitch;
		Vsrc1 += UVrowpitch;
		Vsrc2 += UVrowpitch;
		Vsrc3 += UVrowpitch;
		Vsrc4 += UVrowpitch;
	}

	whichfb ^= 1;

	GX_InvVtxCache();
	GX_InvalidateTexAll();

	DCFlushRange(Ytexture, Ytexsize);
	DCFlushRange(Utexture, UVtexsize);
	DCFlushRange(Vtexture, UVtexsize);

	GX_LoadTexObj(&YtexObj, GX_TEXMAP0);	// MAP0 <- Y
	GX_LoadTexObj(&UtexObj, GX_TEXMAP1);	// MAP1 <- U
	GX_LoadTexObj(&VtexObj, GX_TEXMAP2);	// MAP2 <- V

	//render textures
	GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
		GX_Position1x8(0); GX_Color1x8(0); GX_TexCoord1x8(0); GX_TexCoord1x8(0);
		GX_Position1x8(1); GX_Color1x8(0); GX_TexCoord1x8(1); GX_TexCoord1x8(1);
		GX_Position1x8(2); GX_Color1x8(0); GX_TexCoord1x8(2); GX_TexCoord1x8(2);
		GX_Position1x8(3); GX_Color1x8(0); GX_TexCoord1x8(3); GX_TexCoord1x8(3);
	GX_End();

	GX_SetColorUpdate(GX_TRUE);
	GX_CopyDisp(xfb[whichfb], GX_TRUE);
	GX_DrawDone();


	VIDEO_SetNextFramebuffer(xfb[whichfb]);
	VIDEO_Flush();
}
*/
