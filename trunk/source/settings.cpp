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

struct SCESettings CESettings;
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
	createXMLVariable("ip", "SMB Share IP", CESettings.smbConf[i].ip);
	createXMLVariable("share", "SMB Share Name", CESettings.smbConf[i].share);
	createXMLVariable("user", "SMB Share Username", CESettings.smbConf[i].user);
	createXMLVariable("pwd", "SMB Share Password", CESettings.smbConf[i].pwd);
	createXMLVariable("displayname", "SMB Display Name", CESettings.smbConf[i].displayname);
}

static void createXMLFTPSite(int i)
{
	item = mxmlNewElement(section, "ftpsite");
	mxmlElementSetAttr(item, "name", toStr(i));

	// create variables
	createXMLVariable("ip", "FTP IP", CESettings.ftpConf[i].ip);
	createXMLVariable("folder", "FTP Name", CESettings.ftpConf[i].folder);
	createXMLVariable("user", "FTP Username", CESettings.ftpConf[i].user);
	createXMLVariable("pwd", "FTP Password", CESettings.ftpConf[i].pwd);
	createXMLVariable("passive", "FTP Passive Mode", toStr(CESettings.ftpConf[i].passive));
	createXMLVariable("displayname", "FTP Display Name", CESettings.ftpConf[i].displayname);
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
	createXMLSetting("autoResume", "Auto-resume", toStr(CESettings.autoResume));
	createXMLSetting("playOrder", "Play order", toStr(CESettings.playOrder));
	createXMLSetting("cleanFilenames", "Clean filenames", toStr(CESettings.cleanFilenames));
	createXMLSetting("hideExtensions", "Hide filename extensions", toStr(CESettings.hideExtensions));
	createXMLSetting("filterFiles", "Hide invalid file types", toStr(CESettings.filterFiles));
	createXMLSetting("language", "Language", toStr(CESettings.language));
	createXMLSetting("videoFolder", "Video files folder", CESettings.videoFolder);
	createXMLSetting("musicFolder", "Music files folder", CESettings.musicFolder);
	createXMLSetting("pictureFolder", "Picture files folder", CESettings.pictureFolder);
	createXMLSetting("onlinemediaFolder", "Online media files folder", CESettings.onlinemediaFolder);
	createXMLSetting("exitAction", "Exit action", toStr(CESettings.exitAction));
	createXMLSetting("rumble", "Wiimote rumble", toStr(CESettings.rumble));

	// Cache
	createXMLSection("Cache", "Cache Settings");
	createXMLSetting("cacheSize", "Cache size", toStr(CESettings.cacheSize));
	createXMLSetting("cacheFillStart", "Cache prefill %", toStr(CESettings.cacheFillStart));
	createXMLSetting("cacheFillRestart", "Cache refill %", toStr(CESettings.cacheFillRestart));

	// Network
	createXMLSection("Network", "Network Settings");
	for(int i=0; i<5; i++)
		createXMLSMBShare(i);
	for(int i=0; i<5; i++)
		createXMLFTPSite(i);

	// Video
	createXMLSection("Video", "Video Settings");
	createXMLSetting("frameDropping", "Frame dropping compensation", toStr(CESettings.frameDropping));
	createXMLSetting("aspectRatio", "Aspect ratio", toStr(CESettings.aspectRatio));
	createXMLSetting("videoZoom", "Video zoom", FtoStr(CESettings.videoZoom));
	createXMLSetting("videoXshift", "Video X offset", toStr(CESettings.videoXshift));
	createXMLSetting("videoYshift", "Video Y offset", toStr(CESettings.videoYshift));

	// Audio
	createXMLSection("Audio", "Audio Settings");
	createXMLSetting("volume", "Volume", toStr(CESettings.volume));
	createXMLSetting("audioDelay", "Audio delay (ms)", toStr(CESettings.audioDelay));

	// Subtitles
	createXMLSection("Subtitle", "Subtitle Settings");
	createXMLSetting("subtitleDelay", "Subtitle delay", toStr(CESettings.subtitleDelay));
	createXMLSetting("subtitlePostion", "Subtitle position", toStr(CESettings.subtitlePosition));
	createXMLSetting("subtitleSize", "Subtitle size", toStr(CESettings.subtitleSize));
	createXMLSetting("subtitleAlpha", "Subtitle transparency", toStr(CESettings.subtitleAlpha));
	createXMLSetting("subtitleColor", "Subtitle color", toStr(CESettings.subtitleColor));

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
		loadXMLVariable(CESettings.smbConf[i].ip, "ip", sizeof(CESettings.smbConf[i].ip));
		loadXMLVariable(CESettings.smbConf[i].share, "share", sizeof(CESettings.smbConf[i].share));
		loadXMLVariable(CESettings.smbConf[i].user, "user", sizeof(CESettings.smbConf[i].user));
		loadXMLVariable(CESettings.smbConf[i].pwd, "pwd", sizeof(CESettings.smbConf[i].pwd));
		loadXMLVariable(CESettings.smbConf[i].displayname, "displayname", sizeof(CESettings.smbConf[i].displayname));
	}
}

static void loadXMLFTPSite(int i)
{
	item = mxmlFindElement(xml, xml, "ftpsite", "name", toStr(i), MXML_DESCEND);

	if(item)
	{
		// find variables
		loadXMLVariable(CESettings.ftpConf[i].ip, "ip", sizeof(CESettings.ftpConf[i].ip));
		loadXMLVariable(CESettings.ftpConf[i].folder, "folder", sizeof(CESettings.ftpConf[i].folder));
		loadXMLVariable(CESettings.ftpConf[i].user, "user", sizeof(CESettings.ftpConf[i].user));
		loadXMLVariable(CESettings.ftpConf[i].pwd, "pwd", sizeof(CESettings.ftpConf[i].pwd));
		loadXMLVariable(&CESettings.ftpConf[i].passive, "passive");
		loadXMLVariable(CESettings.ftpConf[i].displayname, "displayname", sizeof(CESettings.ftpConf[i].displayname));
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
			onlinemediaList = (MEDIAENTRY *)realloc(onlinemediaList, (onlinemediaSize + 1) * sizeof(MEDIAENTRY));
			memset(&(onlinemediaList[onlinemediaSize]), 0, sizeof(MEDIAENTRY)); // clear the new entry
			strncpy(onlinemediaList[onlinemediaSize].filepath, path, MAXPATHLEN);
			strncpy(onlinemediaList[onlinemediaSize].address, addr, MAXPATHLEN);
			strncpy(onlinemediaList[onlinemediaSize].displayname, name, MAXJOLIET);
			onlinemediaSize++;
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
	memset(&CESettings, 0, sizeof(SCESettings));

	// General
	CESettings.autoResume = 1;
	CESettings.playOrder = PLAY_SINGLE;
	CESettings.cleanFilenames = 1;
	CESettings.hideExtensions = 1;
	CESettings.filterFiles = 1;
	CESettings.language = CONF_GetLanguage();
	CESettings.videoFolder[0] = 0;
	CESettings.musicFolder[0] = 0;
	CESettings.pictureFolder[0] = 0;
	CESettings.onlinemediaFolder[0] = 0;
	CESettings.exitAction = EXIT_AUTO;
	CESettings.rumble = 1;

	// Cache
	CESettings.cacheSize = 8192;
	CESettings.cacheFillStart = 30;
	CESettings.cacheFillRestart = 50;

	// Network

	// Video
	CESettings.frameDropping = FRAMEDROPPING_AUTO;
	CESettings.aspectRatio = ASPECT_AUTO;
	CESettings.videoZoom = 1;
	CESettings.videoXshift = 0;
	CESettings.videoYshift = 0;

	// Audio
	CESettings.volume = 100;
	CESettings.audioDelay = 0;

	// Subtitles
	CESettings.subtitleDelay = 0;
	CESettings.subtitlePosition = 75; // 0-100
	CESettings.subtitleSize = 16;
	CESettings.subtitleAlpha = 0; // 0-255
	CESettings.subtitleColor = COLOR_WHITE;
}

/****************************************************************************
 * FixInvalidSettings
 *
 * Resets invalid settings back to defaults
 ***************************************************************************/
static void FixInvalidSettings()
{
	// General
	if(CESettings.autoResume != 1 && CESettings.autoResume != 0)
		CESettings.autoResume = 1;
	if(CESettings.playOrder < 0 || CESettings.playOrder > PLAY_LOOP)
		CESettings.playOrder = PLAY_SINGLE;
	if(CESettings.cleanFilenames != 1 && CESettings.cleanFilenames != 0)
		CESettings.cleanFilenames = 1;
	if(CESettings.hideExtensions != 1 && CESettings.hideExtensions != 0)
		CESettings.hideExtensions = 1;
	if(CESettings.filterFiles != 1 && CESettings.filterFiles != 0)
		CESettings.filterFiles = 1;
	if(CESettings.language < 0 || CESettings.language > LANG_KOREAN)
		CESettings.language = LANG_ENGLISH;
	if(CESettings.exitAction < 0 || CESettings.exitAction > EXIT_LOADER)
		CESettings.exitAction = EXIT_AUTO;
	if(CESettings.rumble != 1 && CESettings.rumble != 0)
		CESettings.rumble = 1;

	// Cache
	if(CESettings.cacheSize < 0 || CESettings.cacheSize > 16384)
		CESettings.cacheSize = 8192;
	if(CESettings.cacheFillStart < 0 || CESettings.cacheFillStart > 100)
		CESettings.cacheFillStart = 30;
	if(CESettings.cacheFillRestart < 0 || CESettings.cacheFillRestart > 100)
		CESettings.cacheFillRestart = 50;

	// Network

	// Video
	if(CESettings.frameDropping < 0 || CESettings.frameDropping > FRAMEDROPPING_ALWAYS)
		CESettings.frameDropping = FRAMEDROPPING_AUTO;
	if(CESettings.aspectRatio < 0 || CESettings.aspectRatio > ASPECT_235_1)
		CESettings.aspectRatio = ASPECT_AUTO;
	if(CESettings.videoZoom < 0.5 || CESettings.videoZoom > 1.5)
		CESettings.videoZoom = 1;
	if(CESettings.videoXshift < -50 || CESettings.videoXshift > 50)
		CESettings.videoXshift = 0;
	if(CESettings.videoYshift < -50 || CESettings.videoYshift > 50)
		CESettings.videoYshift = 0;

	// Audio
	if(CESettings.volume < 0 || CESettings.volume > 100)
		CESettings.volume = 100;
	if(CESettings.audioDelay < 0 || CESettings.audioDelay > 1000)
		CESettings.audioDelay = 0;

	// Subtitles
	if(CESettings.subtitleDelay < 0 || CESettings.subtitleDelay > 1000)
		CESettings.subtitleDelay = 0;
	if(CESettings.subtitlePosition < 0 || CESettings.subtitlePosition > 100)
		CESettings.subtitlePosition = 75; // 0-100
	if(CESettings.subtitleSize < 10 || CESettings.subtitleSize > 80)
		CESettings.subtitleSize = 16;
	if(CESettings.subtitleAlpha < 0 || CESettings.subtitleAlpha > 255)
		CESettings.subtitleAlpha = 0; // 0-255
	if(CESettings.subtitleColor < 0 || CESettings.subtitleColor > FONTCOLOR_GRAY)
		CESettings.subtitleColor = FONTCOLOR_WHITE;
}

/****************************************************************************
 * Save settings
 ***************************************************************************/
bool
SaveSettings (bool silent)
{
	char filepath[1024];
	int datasize;
	int offset = 0;

	// We'll save using the first available method (probably SD) since this
	// is the method settings will be loaded from by default

	if (!silent)
		ShowAction ("Saving settings...");

	FixInvalidSettings();
	savebuffer = (char *)malloc(SAVEBUFFERSIZE);
	memset(savebuffer, 0, SAVEBUFFERSIZE);
	datasize = prepareSettingsData ();

	if(strlen(appPath) > 0)
		sprintf(filepath, "%s/settings.xml", appPath);
	else if(ChangeInterface(DEVICE_SD, 1, SILENT))
		sprintf(filepath, "sd1:/%s/settings.xml", APPFOLDER);
	else if(ChangeInterface(DEVICE_USB, 1, SILENT))
		sprintf(filepath, "usb1:/%s/settings.xml", APPFOLDER);
	else if(ChangeInterface(DEVICE_USB, 2, SILENT))
		sprintf(filepath, "usb2:/%s/settings.xml", APPFOLDER);
	else if(ChangeInterface(DEVICE_USB, 3, SILENT))
		sprintf(filepath, "usb3:/%s/settings.xml", APPFOLDER);

	offset = SaveFile(savebuffer, filepath, datasize, silent);

	free(savebuffer);
	CancelAction();

	if (offset > 0)
	{
		strcpy(appPath, filepath); // save successful path
		char * end = strrchr(appPath, '/');
		end[0] = 0; // strip filename

		if (!silent)
			InfoPrompt("Settings saved");
		return true;
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
				loadXMLSetting(&CESettings.autoResume, "autoResume");
				loadXMLSetting(&CESettings.playOrder, "playOrder");
				loadXMLSetting(&CESettings.cleanFilenames, "cleanFilenames");
				loadXMLSetting(&CESettings.hideExtensions, "hideExtensions");
				loadXMLSetting(&CESettings.filterFiles, "filterFiles");
				loadXMLSetting(&CESettings.language, "language");
				loadXMLSetting(CESettings.videoFolder, "videoFolder", sizeof(CESettings.videoFolder));
				loadXMLSetting(CESettings.musicFolder, "musicFolder", sizeof(CESettings.musicFolder));
				loadXMLSetting(CESettings.pictureFolder, "pictureFolder", sizeof(CESettings.pictureFolder));
				loadXMLSetting(CESettings.onlinemediaFolder, "onlinemediaFolder", sizeof(CESettings.onlinemediaFolder));
				loadXMLSetting(&CESettings.exitAction, "exitAction");
				loadXMLSetting(&CESettings.rumble, "rumble");

				// Cache
				loadXMLSetting(&CESettings.cacheSize, "cacheSize");
				loadXMLSetting(&CESettings.cacheFillStart, "cacheFillStart");
				loadXMLSetting(&CESettings.cacheFillRestart, "cacheFillRestart");

				// Network
				for(int i=0; i<5; i++)
				{
					loadXMLSMBShare(i);
					loadXMLFTPSite(i);
				}

				// Video
				loadXMLSetting(&CESettings.frameDropping, "frameDropping");
				loadXMLSetting(&CESettings.aspectRatio, "aspectRatio");
				loadXMLSetting(&CESettings.videoZoom, "videoZoom");
				loadXMLSetting(&CESettings.videoXshift, "videoXshift");
				loadXMLSetting(&CESettings.videoYshift, "videoYshift");

				// Audio
				loadXMLSetting(&CESettings.volume, "volume");
				loadXMLSetting(&CESettings.audioDelay, "audioDelay");

				// Subtitles
				loadXMLSetting(&CESettings.subtitleDelay, "subtitleDelay");
				loadXMLSetting(&CESettings.subtitlePosition, "subtitlePosition");
				loadXMLSetting(&CESettings.subtitleSize, "subtitleSize");
				loadXMLSetting(&CESettings.subtitleAlpha, "subtitleAlpha");
				loadXMLSetting(&CESettings.subtitleColor, "subtitleColor");
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
	sprintf(path[1], "sd1:/%s", APPFOLDER);
	sprintf(path[2], "usb1:/%s", APPFOLDER);
	sprintf(path[3], "usb2:/%s", APPFOLDER);
	sprintf(path[4], "usb3:/%s", APPFOLDER);

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
	return settingsFound;
}
