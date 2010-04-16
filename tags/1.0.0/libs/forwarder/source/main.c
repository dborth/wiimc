/*

Elf/Dol FOrwarder -- loads an elf or dol specified in the code.

by SpaceJump (2008)

Thanks to svpe, the creator of Front SD Loader and TCPLoad for giving me permission for using some of his
functions.

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from
the use of this software.

*/

#include <stdio.h>
#include <stdlib.h>
#include <gccore.h>
#include <wiiuse/wpad.h>
#include <sys/stat.h>
#include <fat.h>
#include <string.h>
#include <gccore.h>

//#include "elf_abi.h"
#include <ogc/machine/processor.h>

//USB2 includes
#include "mload.h"
#include "../build/ehcmodule_elf.h"

u32 load_dol_image (void *dolstart, struct __argv *argv);
void __exception_closeall();

static void *xfb = NULL;
static GXRModeObj *rmode = NULL;

//u32 load_dol_image (void *);

long get_filesize(char *FileName)
{
    struct stat file;
    if(!stat(FileName,&file))
    {
        return file.st_size;
    }
    return 0;
}

void restart()
{
printf("Rebooting Wii...\n");
sleep(3);
SYS_ResetSystem(SYS_RESTART,0,0);
/*
	WPAD_Init();
		printf("Press A to reboot Wii...\n");
		while (1) {
			WPAD_ScanPads();
			u32 pressed = WPAD_ButtonsDown(0);
			if (pressed & WPAD_BUTTON_A) {
				SYS_ResetSystem(SYS_RESTART,0,0);
				//exit(0);
			}
		}
*/		
}

typedef void (*entrypoint) (void);
#include <sdcard/wiisd_io.h>
#include <sdcard/gcsd.h>
#include <ogc/usbstorage.h>

const DISC_INTERFACE* sd = &__io_wiisd;
const DISC_INTERFACE* usb = &__io_usbstorage;


void load_USB2_driver()
{
	//Try to load IOS202 to use usb2
	int ret;
	IOS_ReloadIOS(202);
	ret=-1;
	ret=mload_init();
	data_elf my_data_elf;
	
	if(ret)
	{
		printf("usb2 IOS detectected!\n");
		mload_elf((void *) ehcmodule_elf, &my_data_elf);
		mload_run_thread(my_data_elf.start, my_data_elf.stack, my_data_elf.size_stack, my_data_elf.prio);
		usleep(2000);
		//printf("Running... at 0x%x\n", (u32) my_data_elf.start);
	}
	
}

//---------------------------------------------------------------------------------
#define CHUNCK 64*1024
int main(int argc, char **argv) {
//---------------------------------------------------------------------------------
	
	// Initialise the video system
	VIDEO_Init();
	
	// This function initialises the attached controllers
	//WPAD_Init();
	
	// Obtain the preferred video mode from the system
	// This will correspond to the settings in the Wii menu
	rmode = VIDEO_GetPreferredMode(NULL);

	// Allocate memory for the display in the uncached region
	xfb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
	
	// Initialise the console, required for printf
	console_init(xfb,20,20,rmode->fbWidth,rmode->xfbHeight,rmode->fbWidth*VI_DISPLAY_PIX_SZ);
	
	// Set up the video registers with the chosen mode
	VIDEO_Configure(rmode);
	
	// Tell the video hardware where our display memory is
	VIDEO_SetNextFramebuffer(xfb);
	
	// Make the display visible
	VIDEO_SetBlack(FALSE);

	// Flush the video register changes to the hardware
	VIDEO_Flush();

	// Wait for Video setup to complete
	VIDEO_WaitVSync();
	if(rmode->viTVMode&VI_NON_INTERLACE) VIDEO_WaitVSync();
	
	//------------------------------------------------------------------------------------------------------
	
	printf("\x1b[2;0H");
	
	
	VIDEO_WaitVSync();

	FILE* inputFile=NULL;
	long size=0;

	if(!fatMount("sd",sd,0,3,256)) {
		load_USB2_driver();
		printf("Detecting usb device...\n");
		if(usb->isInserted()) 
		{
			printf("Mounting USB fat\n");
			if (!fatMount("usb",usb,0,3,256)) {
			printf("FAT could not be initialized!\n");
			restart();		
			}
		}
	}
	else 
	{ // try sd
		inputFile = fopen( "sd:/apps/mplayer_ce/boot.dol", "rb");
		if(inputFile == NULL) {
			inputFile = fopen( "sd:/mplayer/boot.dol", "rb");
			if(inputFile != NULL)
				size = get_filesize("sd:/mplayer/boot.dol");
		}else size = get_filesize("sd:/apps/mplayer_ce/boot.dol");	
	}
	
	if(inputFile == NULL)
	{
		if (!fatMount("usb",usb,0,3,256)) {
			printf("FAT could not be initialized!\n");
			restart();		
		}
		inputFile = fopen( "usb:/apps/mplayer_ce/boot.dol", "rb");
		if(inputFile == NULL) {
			inputFile = fopen( "usb:/mplayer/boot.dol", "rb");
			if(inputFile == NULL) {
				printf("boot.dol does not exist in the /apps/mplayer_ce or /mplayer folder on sd/usb !\n");
				restart();		
			}
			size = get_filesize("usb:/mplayer/boot.dol");
		}else size = get_filesize("usb:/apps/mplayer_ce/boot.dol");
	}

	//create a buffer for the elf/dol content
	void* myBuffer=NULL;
	//u8 *myBuffer = (u8 *)0x92000000;
	
	//read elf/dol from given path:
	
	setvbuf(inputFile,NULL,_IONBF,0);
	printf("Found boot.dol on sd or usb!\n");
	
	/*
	int pos = ftell(inputFile);
	fseek(inputFile, 0, SEEK_END);
	int size = ftell(inputFile);
	fseek(inputFile, pos, SEEK_SET); //return to previous position
	*/

  printf("File size: %ld bytes\n",size);
  myBuffer = malloc(size);
  if(myBuffer == NULL) {
    printf("Error allocating memory\n");
    restart();	
  } 
  else
    printf("Memory allocated\n");
      
  printf("Reading file file, please wait\n");
  
  printf("[");
  printf("\x1b[s");  
  printf("                                                 ]");
  printf("\x1b[u");
  
  
  u32 bytes_read,readed=0;
  int aux,old=0;
  while(1)	
	{
	   bytes_read = fread(myBuffer+readed, 1, CHUNCK, inputFile); 
	   if (bytes_read < CHUNCK)
	   {
        s32 result = -!feof(inputFile);
        readed+=bytes_read;
        if (result < 0 || readed!=size) 
        {
          printf("fread error: [%i] %s\n", ferror(inputFile), strerror(ferror(inputFile)));
          fclose(inputFile);
          restart();	
        }
        break;
       }
     
     readed+=bytes_read;
     aux=(readed*50)/size;
     if(old!=aux)
     {
       old=aux;
       printf(".");
       VIDEO_WaitVSync();
     }
  }  
	fclose(inputFile);
    
	//fread (myBuffer, 1, size, inputFile);
	
  printf("\nFile readed successfully\n"); 
	
	//is a dol
	printf("Loading dol...\n");
	VIDEO_WaitVSync();
	
	
    u32 exeEntryPointAddress = 0;
    entrypoint exeEntryPoint;
	
	struct __argv args[10];
	exeEntryPointAddress = load_dol_image(myBuffer,args);		
	printf("dol loaded, executing...\n");
	
  if (exeEntryPointAddress == 0) {
        printf ("Invalid entry point, press any key to quit\n");
        restart();
    }

    exeEntryPoint = (entrypoint) exeEntryPointAddress;	
	//dol_relocstart(myBuffer);
	
	//not need it, we know is a dol
	//Check if valid elf file:
	/*
	s32 res;
	res = valid_elf_image(myBuffer);
    if(res == 1) {
		//elf ok! -> Load entry point of elf file:
		ep = (void(*)())load_elf_image(myBuffer);
	} else {
		//Elf not valid, load dol:
		ep = (void(*)())load_dol_image(myBuffer);		
	}*/
		
	
	//printf("Entry point: 0x%X\n", (unsigned int)ep);
  fatUnmount("usb:");
  //sdio_Deinitialize();
//  printf("1\n");VIDEO_WaitVSync();sleep(3);
	VIDEO_WaitVSync();	
	//sleep(5);
	// code from geckoloader
	u32 level;
	__IOS_ShutdownSubsystems ();
	//printf("IOS_ShutdownSubsystems() done\n");
//  printf("2\n");VIDEO_WaitVSync();sleep(3);
	_CPU_ISR_Disable (level);
	//printf("_CPU_ISR_Disable() done\n");
	__exception_closeall ();
	//printf("__exception_closeall() done. Jumping to ep now...\n");
//  printf("3\n");VIDEO_WaitVSync();sleep(3);
	exeEntryPoint();
	_CPU_ISR_Restore (level);
		
	exit(0);
	return 0;
}
