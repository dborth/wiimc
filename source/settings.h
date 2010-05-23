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
	LANG_KOREAN,
	LANG_ROMANIAN,
	LANG_ESTONIAN,
	LANG_BRAZILIAN_PORTUGUESE,
	LANG_HUNGARIAN,
	LANG_POLISH,
	LANG_RUSSIAN,
	LANG_LENGTH
};

enum {
	FONT_DEFAULT,
	FONT_GENERIC,
	FONT_SIMP_CHINESE,
	FONT_TRAD_CHINESE,
	FONT_JAPANESE,
	FONT_KOREAN
};

typedef struct _cp {
	const char *cpname;
	const char *language;
} CP;

typedef struct _lang {
	const char *language;
	const char *abbrev;
} LANG;

#define LANGUAGE_SIZE 142
#define CODEPAGE_SIZE 28

extern LANG languages[LANGUAGE_SIZE];
extern CP codepages[CODEPAGE_SIZE];

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
	SUBTITLE_ALIGN_TOP,
	SUBTITLE_ALIGN_CENTER,
	SUBTITLE_ALIGN_BOTTOM
};

struct SWiiSettings {
	// Global
	int 	hideExtensions;
	int 	language;
	int		volume;
	int		exitAction;
	int		rumble;
	// Videos
	float	videoZoomHor; // horizontal zoom amount
	float	videoZoomVert; // vertical zoom amount
	int 	videoXshift;
	int 	videoYshift;
	int 	frameDropping; // FRAMEDROPPING_DISABLED, FRAMEDROPPING_AUTO, FRAMEDROPPING_ALWAYS
	float 	aspectRatio;
	int 	cacheFill;
	float 	audioDelay; // in secs
	int 	autoResume;
	int		seekTime;
	char 	videosFolder[MAXPATHLEN];
	// Music
	int 	playOrder; // PLAY_SINGLE, PLAY_CONTINUOUS, PLAY_SHUFFLE, PLAY_LOOP
	char	musicFolder[MAXPATHLEN];
	// Pictures
	int		slideshowDelay;
	char	picturesFolder[MAXPATHLEN];
	// DVD
	int		dvdMenu;
	// Online Media
	char	onlinemediaFolder[MAXPATHLEN];
	// Network
	SMBSettings smbConf[5];
	FTPSettings ftpConf[5];
	// Subtitles
	int		subtitleVisibility; // 0 - off, 1 - on
	float 	subtitleDelay; // in secs
	char	subtitleLanguage[3];
	char	subtitleCodepage[13];
	char	subtitleColor[9];
	float	subtitleSize;
};

void DefaultSettings ();
bool SaveSettings (bool silent);
bool LoadSettings ();

extern struct SWiiSettings WiiSettings;

const char validVideoExtensions[][7] =
{
	"3gp", "asf", "avi", "bik", "divx", "dpg", "dv", "dvr-ms", "evo", "f4v", 
	"flc", "fli", "flv", "iso", "m1v", "m2t", "m2ts", "m2v", "m4v", "mkv", 
	"mov", "mp4", "mp4v", "mpe", "mpeg", "mpg", "mqv", "nsv", "nuv", "ogm", 
	"qt", "rmvb", "swf", "trp", "ts", "vdr", "vfw", "viv", "vob", "vro", 
	"wmv", "y4m", ""
};

const char validAudioExtensions[][5] = 
{
	"aac", "ac3", "aif", "aiff", "amr", "ape", "apl", "asf", "flac", "m4a", 
	"m4b", "m4p", "mka", "mp2", "mp3", "mpc", "ogg", "ra", "ram", "rm", 
	"shn", "wav", "wma", "wv", ""
};

const char validImageExtensions[][5] = 
{
	"bmp", "gif", "jpeg", "jpg", "png", ""
};

const char validPlaylistExtensions[][5] =
{
	"asx", "m3u", "pls", "plx", "ram", "smil", ""
};
