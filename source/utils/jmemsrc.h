/****************************************************************************
 * WiiMC
 * Tantric 2009
 *
 * jmemsrc.h
 * Memory based JPEG decoding to 4x4RGBA texture
 ***************************************************************************/

#ifndef _JMEMSRC_H_
#define _JMEMSRC_H_

#include <gctypes.h>

#ifdef __cplusplus
extern "C" {
#endif

u8 * DecodeJPEG(const u8 * src, u32 len, int * width, int * height);

#ifdef __cplusplus
}
#endif

#endif
