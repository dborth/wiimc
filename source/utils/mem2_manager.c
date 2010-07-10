/****************************************************************************
 * WiiMC
 * Tantric 2009-2010
 *
 * mem2_manager.c
 *
 * MEM2 allocator
 ****************************************************************************/

#include <ogc/machine/asm.h>
#include <ogc/lwp_heap.h>
#include <ogc/system.h>
#include <ogc/machine/processor.h>

static heap_cntrl mem2_heap;
static bool mem2_inited=false;
static u32 mem2_size=0;

u32 InitMem2Manager () 
{
	int size = 	(12*1024*1024) + // cache
				(3*1024*1024) + // browser memory
				(8*1024); // padding
	u32 level;
	unsigned char *mem2_heap_ptr;
	if(mem2_inited) return mem2_size;
	_CPU_ISR_Disable(level);
	size &= ~0x1f; // round down, because otherwise we may exceed the area
	mem2_heap_ptr = (unsigned char *)((u32)SYS_GetArena2Hi()-size);
	SYS_SetArena2Hi(mem2_heap_ptr);
	_CPU_ISR_Restore(level);
	size = __lwp_heap_init(&mem2_heap, mem2_heap_ptr, size, 32);
	mem2_inited=true;
	mem2_size=size;
	return size;
}

void* mem2_memalign(u8 align, u32 size)
{
	void *ptr = __lwp_heap_allocate(&mem2_heap, size+align);
	long diff;

	if (!ptr)
		return NULL;

	diff = ((-(long)ptr - 1) & (align - 1)) + 1;
	ptr = (char *)ptr + diff;
	((char *)ptr)[-1] = diff;
	return ptr;
}

void* mem2_malloc(u32 size)
{
	return mem2_memalign(32, size);
}

void mem2_free(void *ptr)
{
	__lwp_heap_free(&mem2_heap, (char*)ptr - ((char*)ptr)[-1]);
}
