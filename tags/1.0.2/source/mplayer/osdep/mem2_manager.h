#ifndef _MEM2MANAGER_H_
#define _MEM2MANAGER_H_

#ifdef __cplusplus
extern "C" {
#endif

u32 InitMem2Manager ();
void* mem2_malloc(u32 size);
BOOL mem2_free(void *ptr);

#ifdef __cplusplus
}
#endif

#endif
