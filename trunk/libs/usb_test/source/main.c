/************************************************************/
/*                   USB ios58 test                         */
/*                                                          */
/*                                                          */
/************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <ogcsys.h>
#include <gccore.h>
#include <sys/dir.h>
#include <unistd.h>
#include <wiiuse/wpad.h>
#include <fcntl.h>
#include <debug.h>

#include <ogc/usbstorage.h>
#include <sdcard/wiisd_io.h>
#include <sdcard/gcsd.h>



#include <fat.h>


#include <ogc/usbstorage.h>

const DISC_INTERFACE* usb = &__io_usbstorage;
const DISC_INTERFACE* sd = &__io_wiisd;


static bool reset_pressed = false;
static bool power_pressed = false;


static int method=0;

static void *xfb = NULL;
static GXRModeObj *rmode = NULL;

static void initialise_video() 
{
    VIDEO_Init();
    rmode = VIDEO_GetPreferredMode(NULL);
    xfb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
    console_init(xfb, 20, 20, rmode->fbWidth, rmode->xfbHeight, rmode->fbWidth * VI_DISPLAY_PIX_SZ);
    VIDEO_Configure(rmode);
    VIDEO_SetNextFramebuffer(xfb);
    VIDEO_SetBlack(FALSE);
    VIDEO_Flush();
    VIDEO_WaitVSync(); 
    if (rmode->viTVMode & VI_NON_INTERLACE) VIDEO_WaitVSync(); 
    printf("\x1b[2;0H");
    VIDEO_WaitVSync();
}

static void showdir(char *path)
{
  char filename[MAXPATHLEN];
  DIR_ITER *dp;
  struct stat fstat;

  dp = diropen(path);

  if(dp==NULL) 
  {
    printf("Error diropen\n");
    return;
  }
  else printf("Ok diropen\n");
  
  int cnt=0;
  while( dirnext( dp, filename, &fstat ) == 0 )
  {
    //if(cnt==20) break;
    cnt++;

    if( fstat.st_mode & S_IFDIR ) printf("/"); 
    printf("%s\n",filename);	  
    	
  }
  dirclose(dp);
  printf("\nEnd show dirrectory (%d).\n",cnt);
}

static void dump_hex(char *ptr, int len)
{
	int i,pos=0;
	char c;
	do
	{
		for(i=0;(i<16)&&(pos<len);i++)
		{
			c=ptr[pos];
			if(c<' ')c='.';
			//printf(" %02X (%c)",ptr[pos],c);
			printf("%c",c);
			pos++;
		}
		printf("\n");
	}
	while(pos<len);
	printf("----------------------------------------------------\n");
}

static void reset_cb (void) {
	reset_pressed = true;
}

static void power_cb (void) {
	power_pressed = true;
}

void writelog()
{
	FILE *fp;
	char *log;
	char * getusblog();
	
	log=getusblog();
	
	if(fatMount("sd",sd,0,2,128) )
	{

		fp=fopen("sd:/log_usb.txt","wb");

		if(fp!=0)
		{
			fprintf(fp,"USB2 device test program. v1.3\n------------------------------\n");
			fprintf(fp,"method: %i\n",method);
			fwrite(log,1, strlen(log) ,fp);
				
			fclose(fp);
		}
	}
	printf("USB2 device test program. v1.3\n------------------------------\n");
	printf("method: %i\n",method);
	printf("%s\n",log);

}

void test()
{
	if(usb->isInserted())
	{
		writelog();
		printf("USB Device Compatible!!!\n");
	}
	else
	{
		writelog();
		printf("USB Device NOT Compatible!!!\n");
	}
}

int main(int argc, char **argv) 
{
	__exception_setreload(1);
	IOS_ReloadIOS(58);
	usleep(5000);
	initialise_video(); 
	CON_EnableGecko(1,0);

	WPAD_Init();

	SYS_SetResetCallback (reset_cb);
	SYS_SetPowerCallback(power_cb);
	VIDEO_WaitVSync();

	void set_usb_method(int _method);

	usleep(1000000);

	if(IOS_GetVersion()!=58) 
	{
		printf("you need ios58 installed.");
		sleep(4);
		return 0;
	}
	
	usb->startup();
	usleep(1000000);
	printf("\nPress reset or Home to exit\n\n");
	printf("\nPress A to test usb device with standard method\n");
	printf("\nPress 1 to test usb device with method 1\n");
	printf("\nPress 2 to test usb device with method 2\n");

  	VIDEO_WaitVSync();
	while(!reset_pressed)
	{	
		WPAD_ScanPads();
		u32 pressed = WPAD_ButtonsDown(0);
		if ( pressed & WPAD_BUTTON_HOME ) break;
		if ( (pressed & WPAD_BUTTON_A)) 
		{
			set_usb_method(0);
			method=0;
			test();
		}
		else if ( (pressed & WPAD_BUTTON_1)) 
		{
			set_usb_method(1);
			method=1;
			test();
		}
		else if ( (pressed & WPAD_BUTTON_2)) 
		{
			set_usb_method(2);
			method=2;
			test();
		}

		usleep(5000);
		
	}
	
	fatUnmount("sd:");
	
	usleep(50000);

	return 0;
}
