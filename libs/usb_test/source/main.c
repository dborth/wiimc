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

#include <ogc/lwp_watchdog.h>

#include <fat.h>


#include <ogc/usbstorage.h>

const DISC_INTERFACE* usb = &__io_usbstorage;
const DISC_INTERFACE* sd = &__io_wiisd;


static bool reset_pressed = false;
static bool power_pressed = false;

#define USB_TEST_VERSION "1.4"

static int method=0;
static u64 timer_init=0;
void usb_log(char* format, ...);
void reset_usb_log();




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
/*
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
*/

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
			fprintf(fp,"USB2 device test version: %s\n",USB_TEST_VERSION);
			fprintf(fp,"=============================\n");		
			fwrite(log,1, strlen(log) ,fp);
				
			fclose(fp);
		}
	}
	

}




void test(int _method)
{
	void set_usb_method(int _method);

	if(timer_init>0)
	{
		printf ("\x1b[%d;%dH", 15, 1 );
		
		printf("Wake up test aborted\n");
	}

	timer_init=0;

	method=_method;

	set_usb_method(method);

	usb_log("USB2 device test method: %i\n------------------------------\n",method);
	

	if(usb->isInserted())
	{
		usb_log("USB Device Compatible!!!\n");
	}
	else
	{
		usb_log("USB Device NOT Compatible!!!\n");
	}
}

void InitialScreen()
{
	printf ("\x1b[2J"); //clear screen
	
	printf("\n");
	printf("USB2 device test version: %s\n",USB_TEST_VERSION);
	printf("=============================\n");	
	printf("Press reset or Home to exit\n\n");
	printf("Press A to test usb device with standard method\n");
	printf("Press 1 to test usb device with method 1\n");
	printf("Press 2 to test usb device with method 2\n");
	printf("Press + to test wake up device (wait 20mins, then try access device)\n");
	printf("To test all methods unplug&plug device then test another method.\n");
	printf("On exit application log will be saved to sd:/log_usb.txt\n");

}

void enable_wakeup()
{
	timer_init=gettime();

	InitialScreen();

	printf("\nWake up test in progress\n");
	
}

void check_wakeup()
{
	if(timer_init==0) return;
	if(ticks_to_secs(gettime() - timer_init) > 20*60) //20 mins
	{
		char buf[1024];

		timer_init=0;
		printf("Testing wake up\n");
	
		if(usb->readSectors(1024,1,buf)<0)
			usb_log("Error reading sector. Device wake up fail!!\n");
		else
			usb_log("OK reading sector. Device wake up OK!!\n");
		
	}
	else 
	{		
		printf ("\x1b[%d;%dH", 14, 1 ); //cursor position 
		printf("Wake up counter: %i                           ", (20*60)-(int)ticks_to_secs(gettime() - timer_init));
		fflush(stdout);
	}

}

int main(int argc, char **argv) 
{
	void __exception_setreload(int t);	

	__exception_setreload(1);
	if(IOS_GetVersion()!=58) IOS_ReloadIOS(58);
	usleep(5000);
	
	initialise_video(); 
	
	CON_EnableGecko(1,0);
	SYS_SetResetCallback (reset_cb);
	SYS_SetPowerCallback(power_cb);
	

	if(IOS_GetVersion()!=58) 
	{
		printf("\n\nYou need ios58 installed.");
		sleep(4);
		return 0;
	}

	WPAD_Init();
	
	InitialScreen();

	usleep(1000000);
	reset_usb_log();
	usb->startup();
		
	usleep(1000000);


	while(!reset_pressed)
	{	
		WPAD_ScanPads();
		u32 pressed = WPAD_ButtonsDown(0);

		if ( pressed & WPAD_BUTTON_HOME ) break;
		if ( (pressed & WPAD_BUTTON_A)) test(0);
		else if ( (pressed & WPAD_BUTTON_1)) test(1); 
		else if ( (pressed & WPAD_BUTTON_2)) test(2);

		else if ( (pressed & WPAD_BUTTON_PLUS)) enable_wakeup();

		check_wakeup();

		usleep(5000);		
	}
	writelog();
	fatUnmount("sd:");
	
	usleep(50000);

	return 0;
}
