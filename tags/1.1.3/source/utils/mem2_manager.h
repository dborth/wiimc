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

enum mem2_areas_enum {
	MEM2_VIDEO,
	MEM2_BROWSER,
	MEM2_GUI,
	MEM2_OTHER,
	MEM2_PICTURE,
	MEM2_INDEX,
	MEM2_EXTFONT,
	MEM2_MAX
};
 
#ifdef __cplusplus
extern "C" {
#endif
 
bool AddMem2Area (u32 size, const int index);
bool RemoveMem2Area(const int area);
void ClearMem2Area (const int area);
void* mem2_memalign(u8 align, u32 size, const int area);
void* mem2_malloc(u32 size, const int area);
void mem2_free(void *ptr, const int area);	
void* mem2_calloc(u32 num, u32 size, const int area);
void* mem2_realloc(void *ptr, u32 newsize, const int area);
char *mem2_strdup(const char *s, const int area);
char *mem2_strndup(const char *s, size_t n, const int area);
u32 mem2_size(const int i);

void ShowAreaInfo(const int area); //if area == -1 print all areas info

#ifdef __cplusplus
}
#endif

#endif
