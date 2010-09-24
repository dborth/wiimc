/****************************************************************************
 * WiiMC
 * Tantric 2009-2010
 *
 * mem2_manager.h
 *
 * MEM2 allocator
 ****************************************************************************/

#ifndef _MEM2MANAGER_H_
#define _MEM2MANAGER_H_
#include <ogc/system.h>

#define MAX_MEM2_AREAS 5

enum mem2_areas_enum {GUI_AREA,VIDEO_AREA,OTHER_AREA};

#ifdef __cplusplus
extern "C" {
#endif


bool AddMem2Area (u32 size, const int index);
void ClearMem2Area (const int area);
void* mem2_memalign(u8 align, u32 size, const int area);
void* mem2_malloc(u32 size, const int area);
void mem2_free(void *ptr, const int area);	
void* mem2_calloc(u32 num, u32 size, const int area);
void* mem2_realloc(void *ptr, u32 newsize, const int area);


void ShowAreaInfo(const int area); //if area == -1 print all areas info

#ifdef __cplusplus
}
#endif

#endif
