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
#include <string.h>
#include <unistd.h>

//1: critical errors     2: detailed info
#define DEBUG_MEM2_LEVEL 1  // to get info about used mem, it's an approximation because of memory fragementation

#ifdef DEBUG_MEM2_LEVEL
#include <stdio.h>
#endif


#define MAX_AREAS 5

typedef struct {
	heap_cntrl heap;
	unsigned char *heap_ptr;
	u32 size;
	char name[50];
#ifdef DEBUG_MEM2_LEVEL
	u32 allocated;
	u32 top_allocated;
#endif
} st_mem2_area;

static bool _initied=false;

static st_mem2_area mem2_areas[MAX_AREAS];

#define ROUNDDOWN32(v)				(((u32)(v)-0x1f)&~0x1f)

static void initMem2Areas()
{
	int i;

	for( i = 0; i < MAX_AREAS ; i++)
	{
		mem2_areas[i].heap_ptr = NULL;
		mem2_areas[i].size = 0;
		mem2_areas[i].name[0] = '\0';
#ifdef DEBUG_MEM2_LEVEL		
		mem2_areas[i].allocated = 0;
		mem2_areas[i].top_allocated = 0;
#endif
	}
	_initied=true;
}

bool AddMem2Area (u32 size, const char *name)  
{
	int i;
	u32 level;

	if(!_initied) initMem2Areas();


#ifdef DEBUG_MEM2_LEVEL
	if(DEBUG_MEM2_LEVEL == 2)
		printf("AddMem2Area: %s\n",name);
#endif

	 
	_CPU_ISR_Disable(level);

	for( i = 0; i < MAX_AREAS ; i++)
		if(mem2_areas[i].size == 0) break;

	if(i == MAX_AREAS) 
	{
		_CPU_ISR_Restore(level);
#ifdef DEBUG_MEM2_LEVEL
		printf("no free area: %s\n",name);
#endif		
		return false; //no free area
	}

	mem2_areas[i].heap_ptr = (void *)ROUNDDOWN32(((u32)SYS_GetArena2Hi() - size));
	
	if((u32)mem2_areas[i].heap_ptr < (u32)SYS_GetArena2Lo())
	{
		_CPU_ISR_Restore(level);
#ifdef DEBUG_MEM2_LEVEL
		printf("not enough mem2: %s\n",name);
#endif		
		return false; // not enough mem2
	}

	SYS_SetArena2Hi(mem2_areas[i].heap_ptr);
	__lwp_heap_init(&mem2_areas[i].heap, mem2_areas[i].heap_ptr, size, 32);
	
	mem2_areas[i].size = size;
	strcpy(mem2_areas[i].name, name);

	_CPU_ISR_Restore(level);
	return true;
}

void ClearMem2Area (const char *area) 
{
	int i;
	for( i = 0; i < MAX_AREAS ; i++)
		if (strcmp(area,mem2_areas[i].name) == 0)  break;

	if(i == MAX_AREAS) return; // area not found
	
	memset (mem2_areas[i].heap_ptr, 0, mem2_areas[i].size);

	__lwp_heap_init(&mem2_areas[i].heap, mem2_areas[i].heap_ptr, mem2_areas[i].size, 32);
}


void* mem2_memalign(u8 align, u32 size, const char *area)
{
	int i;
	void *ptr;
	for( i = 0; i < MAX_AREAS ; i++)
		if (strcmp(area,mem2_areas[i].name) == 0) break;

	if(i == MAX_AREAS) 
	{
#ifdef DEBUG_MEM2_LEVEL
		printf("mem2 malloc error: area %s not found\n",area);
#endif		
		return NULL; // area not found
	}


	ptr = __lwp_heap_allocate(&mem2_areas[i].heap, size);
#ifdef DEBUG_MEM2_LEVEL
	if(ptr == NULL) printf("Error not enough mem2 in malloc, size: %u  area: %s  allocated: %u	top allocated: %u\n",size,area,mem2_areas[i].allocated,mem2_areas[i].top_allocated);
	else
	{
		//mem2_areas[i].allocated += __lwp_heap_block_size(&mem2_areas[i].heap,ptr);
		//if(mem2_areas[i].allocated > mem2_areas[i].top_allocated) mem2_areas[i].top_allocated = mem2_areas[i].allocated;
	
		//if(DEBUG_MEM2_LEVEL == 2)
		//	printf("mem2 malloc: %u  area: %s  allocated: %u  top allocated: %u\n",size,area,mem2_areas[i].allocated,mem2_areas[i].top_allocated);
	}
#endif
	return ptr;
}

void* mem2_malloc(u32 size, const char *area)
{
	return mem2_memalign(32, size, area);
}

void mem2_free(void *ptr, const char *area)
{
	int i;
	for( i = 0; i < MAX_AREAS ; i++)
		if( (mem2_areas[i].size > 0) && (strcmp(area,mem2_areas[i].name) == 0) ) break;

	if(i == MAX_AREAS)
	{
#ifdef DEBUG_MEM2_LEVEL
		printf("mem2 free error: area %s not found\n",area);
#endif		
		return; // area not found
	}
	

#ifdef DEBUG_MEM2_LEVEL
	//u32 size=__lwp_heap_block_size(&mem2_areas[i].heap,ptr); // this function is added in libogc

	//mem2_areas[i].allocated -= size;

	//if(DEBUG_MEM2_LEVEL == 2)
	//	printf("mem2 free: %u	area: %s  allocated: %u  top allocated: %u\n",size,area,mem2_areas[i].allocated,mem2_areas[i].top_allocated);
	
#endif		

	__lwp_heap_free(&mem2_areas[i].heap, ptr);
}

#ifdef DEBUG_MEM2_LEVEL
static void PrintAreaInfo(int index)
{
	heap_iblock info;

	if(mem2_areas[index].size == 0) return;	
	

	printf("Area: %s. Allocated: %u. Top Allocated: %u\n",mem2_areas[index].name,mem2_areas[index].allocated,mem2_areas[index].top_allocated);
	__lwp_heap_getinfo(&mem2_areas[index].heap,&info);
	printf("Area: %s. free blocks: %u  free size: %u  used blocks: %u  used_size: %u\n",mem2_areas[index].name, 
			info.free_blocks,info.free_size,info.used_blocks,info.used_size);
}
#endif

void ShowAreaInfo(const char *area) //if area == NULL print all areas info
{
#ifdef DEBUG_MEM2_LEVEL

	int i=-1;

	if(area != NULL)
	{
		for( i = 0; i < MAX_AREAS ; i++)
			if( (mem2_areas[i].size > 0) && (strcmp(area,mem2_areas[i].name) == 0) ) break;

		if(i == MAX_AREAS) 
		{
			printf("ShowAreaInfo error: area %s not found\n",area);
			return; // area not found
		}
	}

	if(i>=0)
		PrintAreaInfo(i);
	else
		for( i = 0; i < MAX_AREAS ; i++) PrintAreaInfo(i);
#endif
}
