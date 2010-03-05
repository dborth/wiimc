/****************************************************************************
 * WiiMC
 * Tantric 2009
 *
 * settings.cpp
 * Settings save/load to XML file
 ***************************************************************************/

#include <gccore.h>
#include <stdio.h>
#include <string.h>
#include <sys/dir.h>
#include <ogcsys.h>
#include <mxml.h>
#include <ogc/conf.h>

#include "wiimc.h"
#include "menu.h"
#include "fileop.h"
#include "filebrowser.h"
#include "input.h"
#include "button_mapping.h"
#include "settings.h"

#define SAVEBUFFERSIZE (64*1024)

struct SWiiSettings WiiSettings;
static char * savebuffer = NULL;

/****************************************************************************
 * Prepare settings data
 *
 * This sets up the save buffer for saving.
 ***************************************************************************/
static mxml_node_t *xml = NULL;
static mxml_node_t *data = NULL;
static mxml_node_t *section = NULL;
static mxml_node_t *item = NULL;
static mxml_node_t *elem = NULL;

static char temp[20];

static const char * toStr(int i)
{
	sprintf(temp, "%d", i);
	return temp;
}
static const char * FtoStr(float i)
{
	sprintf(temp, "%.2f", i);
	return temp;
}

static void createXMLSection(const char * name, const char * description)
{
	section = mxmlNewElement(data, "section");
	mxmlElementSetAttr(section, "name", name);
	mxmlElementSetAttr(section, "description", description);
}

static void createXMLSetting(const char * name, const char * description, const char * value)
{
	item = mxmlNewElement(section, "setting");
	mxmlElementSetAttr(item, "name", name);
	mxmlElementSetAttr(item, "value", value);
	mxmlElementSetAttr(item, "description", description);
}

static void createXMLVariable(const char * name, const char * description, const char * value)
{
	elem = mxmlNewElement(item, "variable");
	mxmlElementSetAttr(elem, "name", name);
	mxmlElementSetAttr(elem, "value", value);
	mxmlElementSetAttr(elem, "description", description);
}

static void createXMLSMBShare(int i)
{
	item = mxmlNewElement(section, "smbshare");
	mxmlElementSetAttr(item, "name", toStr(i));

	// create variables
	createXMLVariable("ip", "SMB Share IP", WiiSettings.smbConf[i].ip);
	createXMLVariable("share", "SMB Share Name", WiiSettings.smbConf[i].share);
	createXMLVariable("user", "SMB Share Username", WiiSettings.smbConf[i].user);
	createXMLVariable("pwd", "SMB Share Password", WiiSettings.smbConf[i].pwd);
	createXMLVariable("displayname", "SMB Display Name", WiiSettings.smbConf[i].displayname);
}

static void createXMLFTPSite(int i)
{
	item = mxmlNewElement(section, "ftpsite");
	mxmlElementSetAttr(item, "name", toStr(i));

	// create variables
	createXMLVariable("ip", "FTP IP", WiiSettings.ftpConf[i].ip);
	createXMLVariable("folder", "FTP Name", WiiSettings.ftpConf[i].folder);
	createXMLVariable("user", "FTP Username", WiiSettings.ftpConf[i].user);
	createXMLVariable("pwd", "FTP Password", WiiSettings.ftpConf[i].pwd);
	createXMLVariable("passive", "FTP Passive Mode", toStr(WiiSettings.ftpConf[i].passive));
	createXMLVariable("displayname", "FTP Display Name", WiiSettings.ftpConf[i].displayname);
}

static const char * XMLSaveCallback(mxml_node_t *node, int where)
{
	const char *name;

	name = node->value.element.name;

	if(where == MXML_WS_BEFORE_CLOSE)
	{
		if(!strcmp(name, "file") || !strcmp(name, "section"))
			return ("\n");
		else if(!strcmp(name, "smbshare") || !strcmp(name, "ftpsite"))
			return ("\n\t");
	}
	if (where == MXML_WS_BEFORE_OPEN)
	{
		if(!strcmp(name, "file"))
			return ("\n");
		else if(!strcmp(name, "section"))
			return ("\n\n");
		else if(!strcmp(name, "setting") || !strcmp(name, "smbshare") || !strcmp(name, "ftpsite"))
			return ("\n\t");
		else if(!strcmp(name, "variable"))
			return ("\n\t\t");
	}
	return (NULL);
}


static int
prepareSettingsData ()
{
	xml = mxmlNewXML("1.0");
	mxmlSetWrapMargin(0); // disable line wrapping

	data = mxmlNewElement(xml, "file");
	mxmlElementSetAttr(data, "app", APPNAME);
	mxmlElementSetAttr(data, "version", APPVERSION);

	// General
	createXMLSection("General", "General Settings");
	createXMLSetting("autoResume", "Auto-resume", toStr(WiiSettings.autoResume));
	createXMLSetting("playOrder", "Play order", toStr(WiiSettings.playOrder));
	createXMLSetting("cleanFilenames", "Clean filenames", toStr(WiiSettings.cleanFilenames));
	createXMLSetting("hideExtensions", "Hide filename extensions", toStr(WiiSettings.hideExtensions));
	createXMLSetting("filterFiles", "Hide invalid file types", toStr(WiiSettings.filterFiles));
	createXMLSetting("language", "Language", toStr(WiiSettings.language));
	createXMLSetting("videosFolder", "Videos folder", WiiSettings.videosFolder);
	createXMLSetting("musicFolder", "Music folder", WiiSettings.musicFolder);
	createXMLSetting("picturesFolder", "Pictures folder", WiiSettings.picturesFolder);
	createXMLSetting("onlinemediaFolder", "Online media folder", WiiSettings.onlinemediaFolder);
	createXMLSetting("exitAction", "Exit action", toStr(WiiSettings.exitAction));
	createXMLSetting("rumble", "Wiimote rumble", toStr(WiiSettings.rumble));

	// Cache
	createXMLSection("Cache", "Cache Settings");
	createXMLSetting("cacheSize", "Cache size", toStr(WiiSettings.cacheSize));
	createXMLSetting("cachePrefill", "Cache prefill %", toStr(WiiSettings.cachePrefill));

	// Network
	createXMLSection("Network", "Network Settings");
	for(int i=0; i<5; i++)
		createXMLSMBShare(i);
	for(int i=0; i<5; i++)
		createXMLFTPSite(i);

	// Video
	createXMLSection("Video", "Video Settings");
	createXMLSetting("frameDropping", "Frame dropping compensation", toStr(WiiSettings.frameDropping));
	createXMLSetting("aspectRatio", "Aspect ratio", toStr(WiiSettings.aspectRatio));
	createXMLSetting("videoZoomHor", "Horizontal video zoom", FtoStr(WiiSettings.videoZoomHor));
	createXMLSetting("videoZoomVert", "Vertical video zoom", FtoStr(WiiSettings.videoZoomVert));
	createXMLSetting("videoXshift", "Video X offset", toStr(WiiSettings.videoXshift));
	createXMLSetting("videoYshift", "Video Y offset", toStr(WiiSettings.videoYshift));

	// Audio
	createXMLSection("Audio", "Audio Settings");
	createXMLSetting("volume", "Volume", toStr(WiiSettings.volume));
	createXMLSetting("audioDelay", "Audio delay (ms)", toStr(WiiSettings.audioDelay));

	// Subtitles
	createXMLSection("Subtitle", "Subtitle Settings");
	createXMLSetting("subtitleVisibility", "Subtitle visibility", toStr(WiiSettings.subtitleVisibility));
	createXMLSetting("subtitlePosition", "Subtitle position", toStr(WiiSettings.subtitlePosition));
	createXMLSetting("subtitleScale", "Subtitle size", toStr(WiiSettings.subtitleScale));
	createXMLSetting("subtitleDelay", "Subtitle delay", toStr(WiiSettings.subtitleDelay));

	int datasize = mxmlSaveString(xml, (char *)savebuffer, SAVEBUFFERSIZE, XMLSaveCallback);

	mxmlDelete(xml);
	return datasize;
}

/****************************************************************************
 * loadXMLSetting
 *
 * Load XML elements into variables for an individual variable
 ***************************************************************************/

static void loadXMLSetting(char * var, const char * name, int maxsize)
{
	item = mxmlFindElement(xml, xml, "setting", "name", name, MXML_DESCEND);
	if(item)
	{
		const char * tmp = mxmlElementGetAttr(item, "value");
		if(tmp)
			snprintf(var, maxsize, "%s", tmp);
	}
}
static void loadXMLSetting(int * var, const char * name)
{
	item = mxmlFindElement(xml, xml, "setting", "name", name, MXML_DESCEND);
	if(item)
	{
		const char * tmp = mxmlElementGetAttr(item, "value");
		if(tmp)
			*var = atoi(tmp);
	}
}
static void loadXMLSetting(float * var, const char * name)
{
	item = mxmlFindElement(xml, xml, "setting", "name", name, MXML_DESCEND);
	if(item)
	{
		const char * tmp = mxmlElementGetAttr(item, "value");
		if(tmp)
			*var = atof(tmp);
	}
}

static void loadXMLVariable(char * var, const char * name, int maxsize)
{
	elem = mxmlFindElement(item, xml, "variable", "name", name, MXML_DESCEND);
	if(elem)
	{
		const char * tmp = mxmlElementGetAttr(elem, "value");
		if(tmp)
			snprintf(var, maxsize, "%s", tmp);
	}
}
static void loadXMLVariable(int * var, const char * name)
{
	elem = mxmlFindElement(item, xml, "variable", "name", name, MXML_DESCEND);
	if(elem)
	{
		const char * tmp = mxmlElementGetAttr(elem, "value");
		if(tmp)
			*var = atoi(tmp);
	}
}

static void loadXMLSMBShare(int i)
{
	item = mxmlFindElement(xml, xml, "smbshare", "name", toStr(i), MXML_DESCEND);

	if(item)
	{
		// find variables
		loadXMLVariable(WiiSettings.smbConf[i].ip, "ip", sizeof(WiiSettings.smbConf[i].ip));
		loadXMLVariable(WiiSettings.smbConf[i].share, "share", sizeof(WiiSettings.smbConf[i].share));
		loadXMLVariable(WiiSettings.smbConf[i].user, "user", sizeof(WiiSettings.smbConf[i].user));
		loadXMLVariable(WiiSettings.smbConf[i].pwd, "pwd", sizeof(WiiSettings.smbConf[i].pwd));
		loadXMLVariable(WiiSettings.smbConf[i].displayname, "displayname", sizeof(WiiSettings.smbConf[i].displayname));
	}
}

static void loadXMLFTPSite(int i)
{
	item = mxmlFindElement(xml, xml, "ftpsite", "name", toStr(i), MXML_DESCEND);

	if(item)
	{
		// find variables
		loadXMLVariable(WiiSettings.ftpConf[i].ip, "ip", sizeof(WiiSettings.ftpConf[i].ip));
		loadXMLVariable(WiiSettings.ftpConf[i].folder, "folder", sizeof(WiiSettings.ftpConf[i].folder));
		loadXMLVariable(WiiSettings.ftpConf[i].user, "user", sizeof(WiiSettings.ftpConf[i].user));
		loadXMLVariable(WiiSettings.ftpConf[i].pwd, "pwd", sizeof(WiiSettings.ftpConf[i].pwd));
		loadXMLVariable(&WiiSettings.ftpConf[i].passive, "passive");
		loadXMLVariable(WiiSettings.ftpConf[i].displayname, "displayname", sizeof(WiiSettings.ftpConf[i].displayname));
	}
}

static void RecurseOnlineMedia(mxml_node_t * top, char * path)
{
	mxml_node_t * next;
	
	next = mxmlFindElement(top, top, "link", NULL, NULL, MXML_DESCEND_FIRST);

	while(next != NULL)
	{
		const char * name = mxmlElementGetAttr(next, "name");
		const char * addr = mxmlElementGetAttr(next, "addr");
		
		if(name && addr) // this is a link
		{
			if(!AddMediaEntry())
				break;

			strncpy(onlinemediaList[onlinemediaSize-1].filepath, path, MAXPATHLEN);
			strncpy(onlinemediaList[onlinemediaSize-1].address, addr, MAXPATHLEN);
			strncpy(onlinemediaList[onlinemediaSize-1].displayname, name, MAXJOLIET);
			onlinemediaList[onlinemediaSize-1].filepath[MAXPATHLEN] = 0;
			onlinemediaList[onlinemediaSize-1].address[MAXPATHLEN] = 0;
			onlinemediaList[onlinemediaSize-1].displayname[MAXJOLIET] = 0;
		}
		next = mxmlFindElement(next, top, "link", NULL, NULL, MXML_NO_DESCEND);
	}
	
	next = mxmlFindElement(top, top, "folder", NULL, NULL, MXML_DESCEND_FIRST);
	
	while(next != NULL)
	{
		const char * name = mxmlElementGetAttr(next, "name");
		
		if(name) // this is a folder
		{
			char newpath[MAXPATHLEN];
			sprintf(newpath, "%s%s/", path, name);
			RecurseOnlineMedia(next, newpath);
		}
		next = mxmlFindElement(next, top, "folder", NULL, NULL, MXML_NO_DESCEND);
	}
}

/****************************************************************************
 * Load online media entries from specified file
 ***************************************************************************/
static void LoadOnlineMediaFile(char * filepath)
{
	int offset = 0;

	savebuffer = (char *)malloc(SAVEBUFFERSIZE);
	memset(savebuffer, 0, SAVEBUFFERSIZE);
	offset = LoadFile(savebuffer, filepath, SILENT);

	if (offset > 0)
	{
		xml = mxmlLoadString(NULL, savebuffer, MXML_TEXT_CALLBACK);

		if(xml)
		{
			data = mxmlFindElement(xml, xml, "file", NULL, NULL, MXML_DESCEND);
			if(data) RecurseOnlineMedia(data, (char *)"");
			mxmlDelete(xml);
		}
	}
	free(savebuffer);
}

/****************************************************************************
 * DefaultSettings
 *
 * Sets all the defaults!
 ***************************************************************************/
void DefaultSettings ()
{
	memset(&WiiSettings, 0, sizeof(SWiiSettings));

	// General
	WiiSettings.autoResume = 1;
	WiiSettings.playOrder = PLAY_SINGLE;
	WiiSettings.cleanFilenames = 1;
	WiiSettings.hideExtensions = 1;
	WiiSettings.filterFiles = 1;
	WiiSettings.language = CONF_GetLanguage();
	WiiSettings.videosFolder[0] = 0;
	WiiSettings.musicFolder[0] = 0;
	WiiSettings.picturesFolder[0] = 0;
	WiiSettings.onlinemediaFolder[0] = 0;
	WiiSettings.exitAction = EXIT_AUTO;
	WiiSettings.rumble = 1;

	// Cache
	WiiSettings.cacheSize = 8192;
	WiiSettings.cachePrefill = 50;

	// Network

	// Video
	WiiSettings.frameDropping = FRAMEDROPPING_AUTO;
	WiiSettings.aspectRatio = -1;
	WiiSettings.videoZoomHor = 1;
	WiiSettings.videoZoomVert = 1;
	WiiSettings.videoXshift = 0;
	WiiSettings.videoYshift = 0;

	// Audio
	WiiSettings.volume = 100;
	WiiSettings.audioDelay = 0;

	// Subtitles
	WiiSettings.subtitleVisibility = 1;
	WiiSettings.subtitlePosition = SUBTITLE_ALIGN_BOTTOM;
	WiiSettings.subtitleScale = 1;
	WiiSettings.subtitleDelay = 0;
}

/****************************************************************************
 * FixInvalidSettings
 *
 * Resets invalid settings back to defaults
 ***************************************************************************/
static void FixInvalidSettings()
{
	// General
	if(WiiSettings.autoResume != 1 && WiiSettings.autoResume != 0)
		WiiSettings.autoResume = 1;
	if(WiiSettings.playOrder < 0 || WiiSettings.playOrder > PLAY_LOOP)
		WiiSettings.playOrder = PLAY_SINGLE;
	if(WiiSettings.cleanFilenames != 1 && WiiSettings.cleanFilenames != 0)
		WiiSettings.cleanFilenames = 1;
	if(WiiSettings.hideExtensions != 1 && WiiSettings.hideExtensions != 0)
		WiiSettings.hideExtensions = 1;
	if(WiiSettings.filterFiles != 1 && WiiSettings.filterFiles != 0)
		WiiSettings.filterFiles = 1;
	if(WiiSettings.language < 0 || WiiSettings.language > LANG_KOREAN)
		WiiSettings.language = LANG_ENGLISH;
	if(WiiSettings.exitAction < 0 || WiiSettings.exitAction > EXIT_LOADER)
		WiiSettings.exitAction = EXIT_AUTO;
	if(WiiSettings.rumble != 1 && WiiSettings.rumble != 0)
		WiiSettings.rumble = 1;

	// Cache
	if(WiiSettings.cacheSize < 2048 || WiiSettings.cacheSize > 16384)
		WiiSettings.cacheSize = 8192;
	if(WiiSettings.cachePrefill < 10 || WiiSettings.cachePrefill > 100)
		WiiSettings.cachePrefill = 50;

	// Network

	// Video
	if(WiiSettings.frameDropping < 0 || WiiSettings.frameDropping > FRAMEDROPPING_ALWAYS)
		WiiSettings.frameDropping = FRAMEDROPPING_AUTO;
	if(WiiSettings.aspectRatio <= 0 || WiiSettings.aspectRatio > 2.36)
		WiiSettings.aspectRatio = -1;
	if(WiiSettings.videoZoomHor < 0.5 || WiiSettings.videoZoomHor > 1.5)
		WiiSettings.videoZoomHor = 1;
	if(WiiSettings.videoZoomVert < 0.5 || WiiSettings.videoZoomVert > 1.5)
		WiiSettings.videoZoomVert = 1;
	if(WiiSettings.videoXshift < -50 || WiiSettings.videoXshift > 50)
		WiiSettings.videoXshift = 0;
	if(WiiSettings.videoYshift < -50 || WiiSettings.videoYshift > 50)
		WiiSettings.videoYshift = 0;

	// Audio
	if(WiiSettings.volume < 0 || WiiSettings.volume > 100)
		WiiSettings.volume = 100;
	if(WiiSettings.audioDelay < 0 || WiiSettings.audioDelay > 1000)
		WiiSettings.audioDelay = 0;

	// Subtitles
	if(WiiSettings.subtitleVisibility < 0 || WiiSettings.subtitleVisibility > 1)
		WiiSettings.subtitleVisibility = 1;
	if(WiiSettings.subtitlePosition < 0 || WiiSettings.subtitlePosition > 2)
		WiiSettings.subtitlePosition = 0;
	if(WiiSettings.subtitleScale < 0.5 || WiiSettings.subtitleScale > 1.5)
		WiiSettings.subtitleScale = 1;
	if(WiiSettings.subtitleDelay < -1000 || WiiSettings.subtitleDelay > 1000)
		WiiSettings.subtitleDelay = 0;
}

/****************************************************************************
 * Save settings
 ***************************************************************************/
bool
SaveSettings (bool silent)
{
	char filepath[1024] = { 0 };
	int datasize;
	int offset = 0;

	// We'll save using the first available method (probably SD) since this
	// is the method settings will be loaded from by default

	if(strlen(appPath) > 0)
	{
		sprintf(filepath, "%s/settings.xml", appPath);
	}
	else
	{
		if(ChangeInterface(DEVICE_SD, 1, SILENT))
			sprintf(filepath, "sd1:");
		else if(ChangeInterface(DEVICE_USB, 1, SILENT))
			sprintf(filepath, "usb1:");
		else if(ChangeInterface(DEVICE_USB, 2, SILENT))
			sprintf(filepath, "usb2:");
		else if(ChangeInterface(DEVICE_USB, 3, SILENT))
			sprintf(filepath, "usb3:");

		// could not mount any devices
		if(strlen(filepath) == 0)
		{
			if(!silent)
			{
				ErrorPrompt("Could not find a valid SD or USB device - one is required for normal operation.");
				if(!InitMPlayer())
					ExitRequested = 1;
			}
			return false;
		}

		// ensure the necessary folders exists for saving
		strcat(filepath, "/apps");
		if (!diropen(filepath))
			mkdir(filepath, 0777);
		
		strcat(filepath, "/");
		strcat(filepath, APPFOLDER);
		if (!diropen(filepath))
			mkdir(filepath, 0777);
		
		strcat(filepath, "/settings.xml");
	}

	if (!silent)
		ShowAction ("Saving settings...");

	FixInvalidSettings();
	savebuffer = (char *)malloc(SAVEBUFFERSIZE);
	memset(savebuffer, 0, SAVEBUFFERSIZE);
	datasize = prepareSettingsData ();

	offset = SaveFile(savebuffer, filepath, datasize, silent);

	free(savebuffer);
	CancelAction();

	if (offset > 0)
	{
		strcpy(appPath, filepath); // save successful path
		char * end = strrchr(appPath, '/');
		end[0] = 0; // strip filename

		// save restore points
		sprintf(filepath,"%s/%s",appPath,"restore_points");
		char * buff = wiiSaveRestorePoints(filepath);
		SaveFile(buff, filepath, strlen(buff), SILENT);
		free(buff);

		if (!silent)
			InfoPrompt("Settings saved");
		return true;
	}
	else if(!silent)
	{
		char msg[512];
		sprintf(msg, "Could not save settings to %s", filepath);
		ErrorPrompt(msg);

		if(!InitMPlayer())
			ExitRequested = 1;
	}
	return false;
}

/****************************************************************************
 * Load settings from specified file
 ***************************************************************************/
static bool LoadSettingsFile(char * filepath)
{
	bool result = false;
	int offset = 0;

	savebuffer = (char *)malloc(SAVEBUFFERSIZE);
	memset(savebuffer, 0, SAVEBUFFERSIZE);
	offset = LoadFile(savebuffer, filepath, SILENT);

	if (offset > 0)
	{
		xml = mxmlLoadString(NULL, savebuffer, MXML_TEXT_CALLBACK);

		if(xml)
		{
			// check settings version
			// we don't do anything with the version #, but we'll store it anyway
			item = mxmlFindElement(xml, xml, "file", "version", NULL, MXML_DESCEND);
			if(item) // a version entry exists
			{
				const char * version = mxmlElementGetAttr(item, "version");

				if(version && strlen(version) == 5)
				{
					// this code assumes version in format X.X.X
					// XX.X.X, X.XX.X, or X.X.XX will NOT work
					int verMajor = version[0] - '0';
					int verMinor = version[2] - '0';
					int verPoint = version[4] - '0';
					int curMajor = APPVERSION[0] - '0';
					int curMinor = APPVERSION[2] - '0';
					int curPoint = APPVERSION[4] - '0';

					// first we'll check that the versioning is valid
					if(!(verMajor >= 0 && verMajor <= 9 &&
						verMinor >= 0 && verMinor <= 9 &&
						verPoint >= 0 && verPoint <= 9))
						result = false;
					else if(verMajor < 1) // less than version 1.0.0
						result = false; // reset settings
					else if(verMajor > curMajor || verMinor > curMinor || verPoint > curPoint) // some future version
						result = false; // reset settings
					else
						result = true;
				}
			}

			if(result)
			{
				// General
				loadXMLSetting(&WiiSettings.autoResume, "autoResume");
				loadXMLSetting(&WiiSettings.playOrder, "playOrder");
				loadXMLSetting(&WiiSettings.cleanFilenames, "cleanFilenames");
				loadXMLSetting(&WiiSettings.hideExtensions, "hideExtensions");
				loadXMLSetting(&WiiSettings.filterFiles, "filterFiles");
				loadXMLSetting(&WiiSettings.language, "language");
				loadXMLSetting(WiiSettings.videosFolder, "videosFolder", sizeof(WiiSettings.videosFolder));
				loadXMLSetting(WiiSettings.musicFolder, "musicFolder", sizeof(WiiSettings.musicFolder));
				loadXMLSetting(WiiSettings.picturesFolder, "picturesFolder", sizeof(WiiSettings.picturesFolder));
				loadXMLSetting(WiiSettings.onlinemediaFolder, "onlinemediaFolder", sizeof(WiiSettings.onlinemediaFolder));
				loadXMLSetting(&WiiSettings.exitAction, "exitAction");
				loadXMLSetting(&WiiSettings.rumble, "rumble");

				// Cache
				loadXMLSetting(&WiiSettings.cacheSize, "cacheSize");
				loadXMLSetting(&WiiSettings.cachePrefill, "cachePrefill");

				// Network
				for(int i=0; i<5; i++)
				{
					loadXMLSMBShare(i);
					loadXMLFTPSite(i);
				}

				// Video
				loadXMLSetting(&WiiSettings.frameDropping, "frameDropping");
				loadXMLSetting(&WiiSettings.aspectRatio, "aspectRatio");
				loadXMLSetting(&WiiSettings.videoZoomHor, "videoZoomHor");
				loadXMLSetting(&WiiSettings.videoZoomVert, "videoZoomVert");
				loadXMLSetting(&WiiSettings.videoXshift, "videoXshift");
				loadXMLSetting(&WiiSettings.videoYshift, "videoYshift");

				// Audio
				loadXMLSetting(&WiiSettings.volume, "volume");
				loadXMLSetting(&WiiSettings.audioDelay, "audioDelay");

				// Subtitles
				loadXMLSetting(&WiiSettings.subtitleVisibility, "subtitleVisibility");
				loadXMLSetting(&WiiSettings.subtitlePosition, "subtitlePosition");
				loadXMLSetting(&WiiSettings.subtitleScale, "subtitleScale");
				loadXMLSetting(&WiiSettings.subtitleDelay, "subtitleDelay");
			}
			mxmlDelete(xml);
		}
	}

	free(savebuffer);
	return result;
}

/****************************************************************************
 * Load settings
 * Checks sources consecutively until we find a settings file
 ***************************************************************************/
static bool settingsLoaded = false;

bool LoadSettings()
{
	if(settingsLoaded) // already attempted loading
		return true;

	bool settingsFound = false;
	char path[5][MAXPATHLEN];
	char filepath[MAXPATHLEN];

	sprintf(path[0], "%s", appPath);
	sprintf(path[1], "sd1:/apps/%s", APPFOLDER);
	sprintf(path[2], "usb1:/apps/%s", APPFOLDER);
	sprintf(path[3], "usb2:/apps/%s", APPFOLDER);
	sprintf(path[4], "usb3:/apps/%s", APPFOLDER);

	for(int i=0; i<5; i++)
	{
		sprintf(filepath, "%s/settings.xml", path[i]);
		settingsFound = LoadSettingsFile(filepath);
		sprintf(filepath, "%s/onlinemedia.xml", path[i]);
		LoadOnlineMediaFile(filepath);

		if(settingsFound)
		{
			strcpy(appPath, path[i]);
			break;
		}
	}

	settingsLoaded = true; // attempted to load settings

	if(settingsFound)
		FixInvalidSettings();

	ResetText();

	if(settingsFound)
	{
		wiiLoadRestorePoints(appPath);
		InitMPlayer();
	}
	return settingsFound;
}
