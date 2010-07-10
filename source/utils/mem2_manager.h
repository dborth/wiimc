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

#ifdef __cplusplus
extern "C" {
#endif

u32 InitMem2Manager ();
void* mem2_memalign(u8 align, u32 size);
void* mem2_malloc(u32 size);
void mem2_free(void *ptr);

#ifdef __cplusplus
}
#endif

#endif
