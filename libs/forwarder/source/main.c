/****************************************************************************
 * WiiMC
 * Tantric 2009-2010
 ***************************************************************************/

#include <malloc.h>
#include <stdlib.h>
#include <ogc/machine/processor.h>
#include <sys/iosupport.h>

#include "pngu.h"
#include "fileop.h"

extern void __exception_closeall();
typedef void (*entrypoint) (void);
u32 load_dol_image (void *dolstart, struct __argv *argv);

extern const u8		background_png[];
extern const u32	background_png_size;
extern const u8		background_wide_png[];
extern const u32	background_wide_png_size;

void InitVideo();
void StopGX();
void Menu_Render();
void Menu_DrawImg(f32 xpos, f32 ypos, u16 width, u16 height, u8 data[], f32 degrees, f32 scaleX, f32 scaleY, u8 alphaF );

static ssize_t __out_write(struct _reent *r, int fd, const char *ptr, size_t len)
{
	return -1;
}

const devoptab_t phony_out = 
{ "stdout",0,NULL,NULL,__out_write,
  NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL, NULL,0,NULL,NULL,NULL,NULL,NULL };

#define HW_REG_BASE   0xcd800000
#define HW_ARMIRQMASK (HW_REG_BASE + 0x03c)
#define HW_ARMIRQFLAG (HW_REG_BASE + 0x038)

int have_hw_access()
{
	if((*(volatile unsigned int*)HW_ARMIRQMASK)&&(*(volatile unsigned int*)HW_ARMIRQFLAG))
		return 1;
  return 0;
}

int main(int argc, char **argv)
{
	void *buffer = (void *)0x92000000;
	devoptab_list[STD_OUT] = &phony_out; // to keep libntfs happy
	devoptab_list[STD_ERR] = &phony_out; // to keep libntfs happy

	// only reload IOS if AHBPROT is not enabled
//	u32 version = IOS_GetVersion();
//	s32 preferred = IOS_GetPreferredVersion();

//	if(version != 58 && preferred > 0 && version != (u32)preferred && have_hw_access() != 1)
//		IOS_ReloadIOS(preferred);

	InitVideo();

	u8 *bg;
	int bgWidth, bgHeight;
	int a;//i,j;

	if (CONF_GetAspectRatio() == CONF_ASPECT_16_9)
		bg = DecodePNG(background_wide_png, &bgWidth, &bgHeight);
	else
		bg = DecodePNG(background_png, &bgWidth, &bgHeight);

	for(a = 0; a <= 255; a+=15)
	{
		Menu_DrawImg(0, 0, bgWidth, bgHeight, bg, 0, 1, 1, a);
		Menu_Render();
	}

	char filepath[1024] = { 0 };
	FILE *fp = NULL;

	// mount devices and look for app path
	FindAppPath(filepath);
	if(filepath[0]=='\0')
	{
		StopGX();
		SYS_ResetSystem(SYS_RETURNTOMENU, 0, 0);
		exit(0);
	}
	strcat(filepath,"/boot.dol");

	fp = fopen(filepath, "rb");
	if(!fp)
	{
		StopGX();
		SYS_ResetSystem(SYS_RETURNTOMENU, 0, 0);
		exit(0);
	}
	
	fseek (fp, 0, SEEK_END);
	int len = ftell(fp);
	fseek (fp, 0, SEEK_SET);
	fread(buffer, 1, len, fp);
	fclose (fp);
	UnmountAllDevices();
	USB_Deinitialize();

	// load entry point
	struct __argv args;
	bzero(&args, sizeof(args));
	args.argvMagic = ARGV_MAGIC;
	args.length = strlen(filepath) + 2;
	args.commandLine = (char*)malloc(args.length);
	strcpy(args.commandLine, filepath);
	args.commandLine[args.length - 1] = '\0';
	args.argc = 1;
	args.argv = &args.commandLine;
	args.endARGV = args.argv + 1;

	u32 exeEntryPointAddress = load_dol_image(buffer, &args);

	entrypoint exeEntryPoint = (entrypoint) exeEntryPointAddress;

	for(a = 255; a >= 0; a-=15)
	{
		Menu_DrawImg(0, 0, bgWidth, bgHeight, bg, 0, 1, 1, a);
		Menu_Render();
	}

	StopGX();
	VIDEO_WaitVSync();
	VIDEO_WaitVSync();

	u32 level;
	SYS_ResetSystem(SYS_SHUTDOWN, 0, 0);
	_CPU_ISR_Disable(level);
	__exception_closeall();
	exeEntryPoint();
	_CPU_ISR_Restore(level);
	return 0;
}
