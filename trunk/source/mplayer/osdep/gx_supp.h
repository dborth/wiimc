/****************************************************************************
*   gx_supp.h - Generic GX Scaler 
*   softdev 2007
*   dhewg 2008
*   sepp256 2008 - Coded YUV->RGB conversion in TEV.
*
*   This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 2 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License along
*   with this program; if not, write to the Free Software Foundation, Inc.,
*   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*
* NGC GX Video Functions
*
* These are pretty standard functions to setup and use GX scaling.
****************************************************************************/
#ifndef _WII_GX_SUPP_H_
#define _WII_GX_SUPP_H_

#include <gccore.h>

#ifdef __cplusplus
extern "C" {
#endif

extern GXRModeObj *vmode;

void GX_InitVideo();
void GX_SetCamPosZ(float f);
void GX_SetOverscan(bool f);
void StopDrawThread();
void GX_SetScreenPos(int _hor_pos,int _vert_pos, int _stretch);

void GX_StartYUV(u16 width, u16 height, u16 haspect, u16 vaspect);
void GX_RenderTexture();
void GX_UpdatePitch(int width,u16 *pitch);
void GX_ResetTextureYUVPointers();
void GX_FillTextureYUV(u16 height,u8 *buffer[3]);
void GX_ConfigTextureYUV(u16 width, u16 height, u16 *pitch);
void getStrideInfo(int *_w1,int *_df1,int *_Yrowpitch);
u8* GetYtexture();

#ifdef __cplusplus
}
#endif

#endif
