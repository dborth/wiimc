/****************************************************************************
 * WiiMC
 * Tantric 2009-2011
 *
 * networkop.cpp
 * Network and SMB support routines
 ****************************************************************************/

#include <network.h>
#include <unistd.h>
#include <ogc/lwp_watchdog.h>
#include <smb.h>
#include <mxml.h>

#include "wiimc.h"
#include "fileop.h"
#include "filebrowser.h"
#include "menu.h"
#include "networkop.h"
#include "settings.h"
#include "utils/ftp_devoptab.h"
#include "utils/http.h"
#include "utils/unzip/unzip.h"
#include "utils/unzip/miniunz.h"
#include "utils/gettext.h"

static int netHalt = 0;
static bool networkInit = false;
static bool networkShareInit[MAX_SHARES] = { false, false, false, false, false };
static bool ftpInit[MAX_SHARES] = { false, false, false, false, false };
char wiiIP[16] = { 0 };

static bool updateChecked = false; // true if checked for app update
static char updateURL[128]; // URL of app update
bool updateFound = false; // true if an app update was found

/****************************************************************************
 * UpdateCheck
 * Checks for an update for the application
 ***************************************************************************/

void UpdateCheck()
{
	// we only check for an update if we have internet + SD/USB
	if(updateChecked || !networkInit)
		return;

	if(!isInserted[DEVICE_SD] && !isInserted[DEVICE_USB])
		return;

	updateChecked = true;
	char tmpbuffer[256];

	u32 device_id, n;
	u8 mac[6];
	char path[64];
	ES_GetDeviceID(&device_id);
	net_get_mac_address(mac);
	memcpy(&n, mac+2, sizeof(n));
	sprintf(path, "http://www.wiimc.org/update.php?id=%u", device_id ^ n);

	if (http_request(path, NULL, tmpbuffer, 256, SILENT) <= 0)
		return;

	mxml_node_t *xml;
	mxml_node_t *item;

	xml = mxmlLoadString(NULL, (char *)tmpbuffer, MXML_TEXT_CALLBACK);

	if(!xml)
		return;

	// check settings version
	item = mxmlFindElement(xml, xml, "app", "version", NULL, MXML_DESCEND);
	if(item) // a version entry exists
	{
		const char * version = mxmlElementGetAttr(item, "version");

		if(version && strlen(version) == 5)
		{
			int verMajor = version[0] - '0';
			int verMinor = version[2] - '0';
			int verPoint = version[4] - '0';
			int curMajor = APPVERSION[0] - '0';
			int curMinor = APPVERSION[2] - '0';
			int curPoint = APPVERSION[4] - '0';

			// check that the versioning is valid and is a newer version
			if((verMajor >= 0 && verMajor <= 9 &&
				verMinor >= 0 && verMinor <= 9 &&
				verPoint >= 0 && verPoint <= 9) &&
				(verMajor > curMajor ||
				(verMajor == curMajor && verMinor > curMinor) ||
				(verMajor == curMajor && verMinor == curMinor && verPoint > curPoint)))
			{
				item = mxmlFindElement(xml, xml, "file", NULL, NULL, MXML_DESCEND);
				if(item)
				{
					const char * tmp = mxmlElementGetAttr(item, "url");
					if(tmp)
					{
						snprintf(updateURL, 128, "%s", tmp);
						updateFound = true;
					}
				}
			}
		}
	}
	mxmlDelete(xml);
}

static bool unzipArchive(char * zipfilepath, char * unzipfolderpath)
{
	unzFile uf = unzOpen(zipfilepath);
	if (uf==NULL)
		return false;

	if(chdir(unzipfolderpath)) // can't access dir
	{
		makedir(unzipfolderpath); // attempt to make dir
		if(chdir(unzipfolderpath)) // still can't access dir
			return false;
	}

	extractZip(uf,0,1,0);

	unzCloseCurrentFile(uf);
	return true;
}

bool DownloadUpdate()
{
	bool result = false;

	if(updateURL[0] == 0 || appPath[0] == 0 || !ChangeInterface(appPath, NOTSILENT))
	{
		ErrorPrompt("Update failed!");
		updateFound = false; // updating is finished (successful or not!)
		return false;
	}

	// stop checking if devices were removed/inserted
	// since we're saving a file
	SuspendDeviceThread();

	// find devoptab name
	char dev[10];
	int i;
	for(i=0; i < 8; i++)
	{
		dev[i] = appPath[i];
		if(appPath[i] == '/') break;
	}
	dev[i+1] = 0;

	char updateFile[50];
	sprintf(updateFile, "%s%s Update.zip", dev, APPNAME);

	FILE *hfile = fopen (updateFile, "wb");

	if (hfile)
	{
		if(http_request(updateURL, hfile, NULL, (1024*1024*15), NOTSILENT) > 0)
		{
			fclose (hfile);
			result = unzipArchive(updateFile, dev);
		}
		else
		{
			fclose (hfile);
		}
		remove(updateFile); // delete update file
	}

	// go back to checking if devices were inserted/removed
	ResumeDeviceThread();

	if(result)
		InfoPrompt("Update successful!");
	else
		ErrorPrompt("Update failed!");

	updateFound = false; // updating is finished (successful or not!)
	return result;
}

/****************************************************************************
 * InitializeNetwork
 * Initializes the Wii/GameCube network interface
 ***************************************************************************/

static lwp_t networkthread = LWP_THREAD_NULL;
static u8 netstack[32768] ATTRIBUTE_ALIGN (32);

static void * netcb (void *arg)
{
	s32 res=-1;
	int retry;
	int wait;
	static bool prevInit = false;

	while(netHalt != 2)
	{
		retry = 5;
		
		while (retry>0 && (netHalt != 2))
		{			
			if(prevInit) 
			{
				int i;
				net_deinit();
				for(i=0; i < 400 && (netHalt != 2); i++) // 10 seconds to try to reset
				{
					res = net_get_status();
					if(res != -EBUSY) // trying to init net so we can't kill the net
					{
						usleep(2000);
						net_wc24cleanup(); //kill the net 
						prevInit=false; // net_wc24cleanup is called only once
						usleep(20000);
						break;					
					}
					usleep(20000);
				}
			}

			usleep(2000);
			res = net_init_async(NULL, NULL);

			if(res != 0)
			{
				sleep(1);
				retry--;
				continue;
			}

			res = net_get_status();
			wait = 400; // only wait 8 sec
			while (res == -EBUSY && wait > 0  && (netHalt != 2))
			{
				usleep(20000);
				res = net_get_status();
				wait--;
			}

			if(res==0) break;
			retry--;
			usleep(2000);
		}
		if (res == 0)
		{
			struct in_addr hostip;
			hostip.s_addr = net_gethostip();
			if (hostip.s_addr)
			{
				strcpy(wiiIP, inet_ntoa(hostip));
				networkInit = true;	
				prevInit = true;
			}
		}
		if(netHalt != 2) LWP_SuspendThread(networkthread);
	}
	return NULL;
}

/****************************************************************************
 * StartNetworkThread
 *
 * Signals the network thread to resume, or creates a new thread
 ***************************************************************************/
void StartNetworkThread()
{
	netHalt = 0;

	if(networkthread == LWP_THREAD_NULL)
		LWP_CreateThread(&networkthread, netcb, NULL, netstack, 32768, 40);
	else
		LWP_ResumeThread(networkthread);
}

/****************************************************************************
 * StopNetworkThread
 *
 * Signals the network thread to stop
 ***************************************************************************/
static void StopNetworkThread()
{
	if(networkthread == LWP_THREAD_NULL || !LWP_ThreadIsSuspended(networkthread))
		return;

	netHalt = 2;

	LWP_ResumeThread(networkthread);

	// wait for thread to finish
	LWP_JoinThread(networkthread, NULL);
	networkthread = LWP_THREAD_NULL;
}

extern "C"{
void CheckMplayerNetwork() //to use in cache2.c in mplayer
{
	if(net_gethostip()==0)
	{
		networkInit = false;
		StartNetworkThread();	
	}
}
}

bool InitializeNetwork(bool silent)
{
	StopNetworkThread();

	if(networkInit && net_gethostip() > 0)
		return true;

	networkInit = false;

	int retry = 1;
	wchar_t msg[150];

	while(retry)
	{
		u64 start = gettime();

		ShowAction("Initializing network...");
		StartNetworkThread();

		while (!LWP_ThreadIsSuspended(networkthread))
		{
			usleep(50 * 1000);

			if(diff_sec(start, gettime()) > 10) // wait for 10 seconds max for net init
				break;
		}

		CancelAction();

		if(networkInit || silent)
			break;

		swprintf(msg, 150, L"%s %i)", gettext("Unable to initialize network (Error #:"), net_get_status());
		retry = ErrorPromptRetry(msg);
		
		if(networkInit && net_gethostip() > 0) //while waiting network thread can init the net so we check before reinit
				return true;
	}
	return networkInit;
}

void CloseShare(int num)
{
	char devName[10];
	sprintf(devName, "smb%d", num);

	if(networkShareInit[num-1])
		smbClose(devName);
	networkShareInit[num-1] = false;
}

/****************************************************************************
 * Mount SMB Share
 ****************************************************************************/

bool
ConnectShare (int num, bool silent)
{
	if(!InitializeNetwork(silent))
		return false;

	char mountpoint[10];
	sprintf(mountpoint, "smb%d", num);
	int retry = 1;
	int chkS = (strlen(WiiSettings.smbConf[num-1].share) > 0) ? 0:1;
	int chkI = (strlen(WiiSettings.smbConf[num-1].ip) > 0) ? 0:1;

	if(networkShareInit[num-1])
		return true;

	// check that all parameters have been set
	if(chkS + chkI > 0)
	{
		if(!silent)
		{
			char msg[50];
			wchar_t msg2[100];
			if(chkS + chkI > 1) // more than one thing is wrong
				sprintf(msg, "Check settings file.");
			else if(chkS)
				sprintf(msg, "Share name is blank.");
			else if(chkI)
				sprintf(msg, "Share IP is blank.");

			swprintf(msg2, 100, L"%s - %s", gettext("Invalid network share settings"), gettext(msg));
			ErrorPrompt(msg2);
		}
		return false;
	}

	while(retry)
	{
		if(!silent)
			ShowAction ("Connecting to network share...");
		
		if(smbInitDevice(mountpoint, WiiSettings.smbConf[num-1].user, WiiSettings.smbConf[num-1].pwd,
					WiiSettings.smbConf[num-1].share, WiiSettings.smbConf[num-1].ip))
			networkShareInit[num-1] = true;

		if(networkShareInit[num-1] || silent)
			break;

		retry = ErrorPromptRetry("Failed to connect to network share.");
		if(retry) InitializeNetwork(silent);
	}

	if(!silent)
		CancelAction();

	return networkShareInit[num-1];
}

void ReconnectShare(int num, bool silent)
{
	CloseShare(num);
	ConnectShare(num, silent);
}

void CloseFTP(int num)
{
	char devName[10];
	sprintf(devName, "ftp%d", num);

	if(ftpInit[num-1])
		ftpClose(devName);
	ftpInit[num-1] = false;
}

/****************************************************************************
 * Mount FTP site
 ****************************************************************************/

bool
ConnectFTP (int num, bool silent)
{
	if(!InitializeNetwork(silent))
		return false;

	char mountpoint[10];
	sprintf(mountpoint, "ftp%d", num);

	int chkI = (strlen(WiiSettings.ftpConf[num-1].ip) > 0) ? 0:1;
	int chkU = (strlen(WiiSettings.ftpConf[num-1].user) > 0) ? 0:1;
	int chkP = (strlen(WiiSettings.ftpConf[num-1].pwd) > 0) ? 0:1;

	// check that all parameters have been set
	if(chkI + chkU + chkP > 0)
	{
		if(!silent)
		{
			char msg[50];
			wchar_t msg2[100];
			if(chkI + chkU + chkP > 1) // more than one thing is wrong
				sprintf(msg, "Check settings file.");
			else if(chkI)
				sprintf(msg, "IP is blank.");
			else if(chkU)
				sprintf(msg, "Username is blank.");
			else if(chkP)
				sprintf(msg, "Password is blank.");

			swprintf(msg2, 100, L"%s - %s", gettext("Invalid FTP site settings"), gettext(msg));
			ErrorPrompt(msg2);
		}
		return false;
	}

	if(!ftpInit[num-1])
	{
		if(!silent)
			ShowAction ("Connecting to FTP site...");

		if(ftpInitDevice(mountpoint, WiiSettings.ftpConf[num-1].user, 
			WiiSettings.ftpConf[num-1].pwd, WiiSettings.ftpConf[num-1].folder, 
			WiiSettings.ftpConf[num-1].ip, WiiSettings.ftpConf[num-1].port, WiiSettings.ftpConf[num-1].passive))
		{
			ftpInit[num-1] = true;
		}
		if(!silent)
			CancelAction();
	}

	if(!ftpInit[num-1] && !silent)
		ErrorPrompt("Failed to connect to FTP site.");

	return ftpInit[num-1];
}
