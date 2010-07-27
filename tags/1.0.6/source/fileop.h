/****************************************************************************
 * WiiMC
 * Tantric 2009-2010
 *
 * fileop.h
 *
 * File operations
 ****************************************************************************/

#ifndef _FILEOP_H_
#define _FILEOP_H_

#include <gccore.h>
#include <stdio.h>
#include <string.h>
#include <ogcsys.h>
#include <unistd.h>

#define MAX_DEVICES 10

typedef struct
{
	char name[100];
	char mount[10];
	int type;
	DISC_INTERFACE* interface;
	sec_t sector;
} DEVICE_STRUCT;

void ResumeDeviceThread();
void SuspendDeviceThread();
void StopDeviceThread();
void ResumeParseThread();
void SuspendParseThread();
void StopParseThread();
void CancelFileOp();
bool IsDeviceRoot(char * path);
bool IsOnlineMediaPath(char *path);
void CleanupPath(char * path);
void GetFullPath(int index, char *path);
void MountAllDevices();
bool ChangeInterface(int device, int devnum, bool silent);
bool ChangeInterface(char * filepath, bool silent);
void CreateLoadPath(char * origpath);
void StripExt(char* string);
char *GetExt(char *file);
bool IsPlaylistExt(char *ext);
bool IsVideoExt(char *ext);
bool IsAudioExt(char *ext);
bool IsImageExt(char *ext);
bool IsAllowedExt(char *ext);
bool IsAllowedProtocol(char *file);
void FindFile();
void FindDirectory();
int ParseDirectory(bool waitParse = false);
int ParsePlaylistFile();
int ParseOnlineMedia();
size_t LoadFile (char * buffer, char *filepath, bool silent);
size_t SaveFile (char * buffer, char *filepath, size_t datasize, bool silent);

extern DEVICE_STRUCT part[2][MAX_DEVICES];

extern bool devicesChanged;
extern bool isInserted[];
extern int currentDevice;
extern int currentDeviceNum;
extern int findLoadedFile;
extern int selectLoadedFile;
extern size_t loadOffset, loadSize;

#endif