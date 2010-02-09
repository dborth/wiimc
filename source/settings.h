/****************************************************************************
 * WiiMC
 * Tantric 2009
 *
 * settings.h
 * Settings save/load to XML file
 ***************************************************************************/

#include "networkop.h"

enum {
	PLAY_SINGLE,
	PLAY_CONTINUOUS,
	PLAY_SHUFFLE,
	PLAY_LOOP
};

enum {
	LANG_JAPANESE = 0,
	LANG_ENGLISH,
	LANG_GERMAN,
	LANG_FRENCH,
	LANG_SPANISH,
	LANG_ITALIAN,
	LANG_DUTCH,
	LANG_SIMP_CHINESE,
	LANG_TRAD_CHINESE,
	LANG_KOREAN
};

enum {
	EXIT_AUTO,
	EXIT_WIIMENU,
	EXIT_POWEROFF,
	EXIT_LOADER
};

enum {
	FRAMEDROPPING_DISABLED,
	FRAMEDROPPING_AUTO,
	FRAMEDROPPING_ALWAYS
};

enum {
	ASPECT_AUTO,
	ASPECT_16_9,
	ASPECT_4_3,
	ASPECT_235_1
};

enum {
	FONTCOLOR_WHITE,
	FONTCOLOR_BLACK,
	FONTCOLOR_GRAY
};

struct SWiiSettings {
	// Menu
	int 	autoResume;
	int 	playOrder; // PLAY_SINGLE, PLAY_CONTINUOUS, PLAY_SHUFFLE, PLAY_LOOP
	int 	cleanFilenames;
	int 	hideExtensions;
	int 	filterFiles;
	int 	language;
	char 	videosFolder[MAXPATHLEN];
	char	musicFolder[MAXPATHLEN];
	char	picturesFolder[MAXPATHLEN];
	char	onlinemediaFolder[MAXPATHLEN];
	int		exitAction;
	int		rumble;

	// Cache
	int 	cacheSize;
	int 	cacheFillStart;
	int 	cacheFillRestart;

	// Network
	SMBSettings smbConf[5];
	FTPSettings ftpConf[5];

	// Video
	int 	frameDropping; // FRAMEDROPPING_DISABLED, FRAMEDROPPING_AUTO, FRAMEDROPPING_ALWAYS
	int 	aspectRatio; // ASPECT_ORIGINAL, ASPECT_16_9, ASPECT_4_3, ASPECT_235_1
	float 	videoZoom;
	int 	videoXshift;
	int 	videoYshift;

	// Audio
	int		volume;
	int 	audioDelay; // in ms

	// Subtitles
	int 	subtitleDelay;
	int 	subtitlePosition;
	int 	subtitleSize;
	int 	subtitleAlpha;
	int 	subtitleColor;
};

void DefaultSettings ();
bool SaveSettings (bool silent);
bool LoadSettings ();

extern struct SWiiSettings WiiSettings;

const char validVideoExtensions[][7] =
{
	"3gp", "avi", "asf", "divx", "dvr-ms", "evo", "flv", "m1v", "m2v", "mkv", 
	"mov", "mp4", "mp4v", "mpe", "mpeg", "mpg", "nsv", "ogm", "qt", "rmvb", 
	"swf", "ts", "vob", "vro", "wmv", "y4m", ""
};

const char validAudioExtensions[][5] = 
{
	"aac", "ape", "asf", "flac", "m4a", "m4p", "m4v", "mka", "mp2", "mp3", 
	"mpc", "ogg", "ra", "rm", "shn", "m4b", "wav", "wma", ""
};

const char validImageExtensions[][5] = 
{
	"bmp", "jpeg", "jpg", "png", ""
};

const char validPlaylistExtensions[][5] =
{
	"asx", "m3u", "pls", "ram", "smil", ""
};
