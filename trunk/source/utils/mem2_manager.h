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

#ifdef __cplusplus
extern "C" {
#endif


bool AddMem2Area (u32 size, const char *name);
void ClearMem2Area (const char *area);
void* mem2_memalign(u8 align, u32 size, const char *area);
void* mem2_malloc(u32 size, const char *area);
void mem2_free(void *ptr, const char *area);	
void* mem2_calloc(u32 num, u32 size, const char *area);
void* mem2_realloc(void *ptr, u32 newsize, const char *area);


void ShowAreaInfo(const char *area); //if area == NULL print all areas info


/*
u32 InitMem2Manager ();
void* mem2_memalign(u8 align, u32 size);
void* mem2_malloc(u32 size);
void mem2_free(void *ptr);

u32 InitMem2GUIManager ();
void ClearMem2GUIManager ();
void* gui_mem2_memalign(u8 align, u32 size);
void* gui_mem2_malloc(u32 size);
void gui_mem2_free(void *ptr);
void print_gui_mem2_info();
void init_debug_gui_mem2(bool enable);
*/


#ifdef __cplusplus
}
#endif

#endif
