/****************************************************************************
 * WiiMC
 * Tantric 2009
 *
 * fileop.cpp
 * File operations
 ***************************************************************************/

#include <gccore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ogcsys.h>
#include <sys/dir.h>
#include <sys/stat.h>
#include <malloc.h>
#include <errno.h>
#include <ntfs.h>
#include <fat.h>
#include <sdcard/wiisd_io.h>
#include <ogc/usbstorage.h>
#include <ogc/machine/processor.h>

extern "C" {
#include "mplayer/playtree.h"
}

#include "wiimc.h"
#include "fileop.h"
#include "networkop.h"
#include "menu.h"
#include "filebrowser.h"
#include "settings.h"
#include "libwiigui/gui.h"

#define THREAD_SLEEP 100

int currentDevice = -1;
int currentDeviceNum = -1;
bool isInserted[3] = { false, false, false };
bool devicesChanged = false;

static char prefix[2][4] = { "sd", "usb" };

static const DISC_INTERFACE* sd = &__io_wiisd;
static const DISC_INTERFACE* usb = &__io_usbstorage;

// folder parsing thread
static lwp_t parsethread = LWP_THREAD_NULL;
static DIR_ITER * dirIter = NULL;
static bool parseHalt = true;
static bool ParseDirEntries();
int selectLoadedFile = 0;

// device thread
lwp_t devicethread = LWP_THREAD_NULL;
static bool deviceHalt = true;

/****************************************************************************
 * devicecallback
 *
 * This checks our devices for changes (SD/USB removed) and
 * initializes the network in the background
 ***************************************************************************/
static int devsleep = 1*1000*1000;
static bool MountPartitions(int device, int silent);
static void UnmountPartitions(int device);

static void *
devicecallback (void *arg)
{
	while(devsleep > 0)
	{
		if(deviceHalt)
			return NULL;
		usleep(THREAD_SLEEP);
		devsleep -= THREAD_SLEEP;
	}

	while (1)
	{
		if(isInserted[DEVICE_SD])
		{
			if(!sd->isInserted()) // device was removed
			{
				isInserted[DEVICE_SD] = false;
				UnmountPartitions(DEVICE_SD);
				devicesChanged = true;
			}
		}
		else if(sd->startup() && sd->isInserted()) // device was inserted
		{
			isInserted[DEVICE_SD] = true;
			MountPartitions(DEVICE_SD, SILENT);
			devicesChanged = true;
		}

		if(isInserted[DEVICE_USB])
		{
			if(!usb->isInserted()) // device was removed
			{
				isInserted[DEVICE_USB] = false;
				UnmountPartitions(DEVICE_USB);
				devicesChanged = true;
			}
		}
		else if(usb->startup() && usb->isInserted()) // device was inserted
		{
			isInserted[DEVICE_USB] = true;
			MountPartitions(DEVICE_USB, SILENT);
			devicesChanged = true;
		}

		if(isInserted[DEVICE_DVD])
		{
			if(!WIIDVD_DiscPresent())
			{
				isInserted[DEVICE_DVD] = false;
				WIIDVD_Unmount();
			}
		}

		UpdateCheck();
		InitializeNetwork(SILENT);
		devsleep = 1000*1000; // 1 sec

		while(devsleep > 0)
		{
			if(deviceHalt)
				return NULL;
			usleep(THREAD_SLEEP);
			devsleep -= THREAD_SLEEP;
		}
	}
	return NULL;
}

static void *
parsecallback (void *arg)
{
	while(ParseDirEntries()) usleep(THREAD_SLEEP);
	return NULL;
}

/****************************************************************************
 * ResumeDeviceThread
 *
 * Signals the device thread to start, and resumes the thread.
 ***************************************************************************/
void
ResumeDeviceThread()
{
	deviceHalt = false;
	if(devicethread == LWP_THREAD_NULL)
		LWP_CreateThread (&devicethread, devicecallback, NULL, NULL, 0, 40);
}

/****************************************************************************
 * HaltGui
 *
 * Signals the device thread to stop.
 ***************************************************************************/
void
HaltDeviceThread()
{
	deviceHalt = true;

	if(devicethread != LWP_THREAD_NULL)
	{
		// wait for thread to finish
		LWP_JoinThread(devicethread, NULL);
		devicethread = LWP_THREAD_NULL;
	}
}

/****************************************************************************
 * ResumeParseThread
 *
 * Signals the parse thread to start, and resumes the thread.
 ***************************************************************************/
void
ResumeParseThread()
{
	parseHalt = false;
	if(parsethread == LWP_THREAD_NULL)
		LWP_CreateThread (&parsethread, parsecallback, NULL, NULL, 0, 40);
}

/****************************************************************************
 * HaltParseThread
 *
 * Signals the parse thread to stop.
 ***************************************************************************/
void
HaltParseThread()
{
	parseHalt = true;

	if(parsethread != LWP_THREAD_NULL)
	{
		// wait for thread to finish
		LWP_JoinThread(parsethread, NULL);
		parsethread = LWP_THREAD_NULL;
	}
}

/****************************************************************************
 * FindPartitions
 *
 * Finds all partitions on the specified device
 ***************************************************************************/

#define le32_to_cpu(x) bswap32(x)

#define BYTES_PER_SECTOR 512
#define NTFS_OEM_ID                         (0x4e54465320202020ULL)

#define PARTITION_TYPE_EMPTY                0x00 /* Empty */
#define PARTITION_TYPE_DOS33_EXTENDED       0x05 /* DOS 3.3+ extended partition */
#define PARTITION_TYPE_NTFS                 0x07 /* Windows NT NTFS */
#define PARTITION_TYPE_WIN95_EXTENDED       0x0F /* Windows 95 extended partition */

#define PARTITION_STATUS_NONBOOTABLE        0x00 /* Non-bootable */
#define PARTITION_STATUS_BOOTABLE           0x80 /* Bootable (active) */

#define MBR_SIGNATURE                       (0x55AA)
#define EBR_SIGNATURE                       (0x55AA)

#define BPB_FAT16_fileSysType  0x36
#define BPB_FAT32_fileSysType  0x52

#define T_FAT  1
#define T_NTFS 2

static const char FAT_SIG[3] = {'F', 'A', 'T'};

/**
 * PRIMARY_PARTITION - Block device partition record
 */
typedef struct _PARTITION_RECORD {
    u8 status;                              /* Partition status; see above */
    u8 chs_start[3];                        /* Cylinder-head-sector address to first block of partition */
    u8 type;                                /* Partition type; see above */
    u8 chs_end[3];                          /* Cylinder-head-sector address to last block of partition */
    u32 lba_start;                          /* Local block address to first sector of partition */
    u32 block_count;                        /* Number of blocks in partition */
} __attribute__((__packed__)) PARTITION_RECORD;

/**
 * MASTER_BOOT_RECORD - Block device master boot record
 */
typedef struct _MASTER_BOOT_RECORD {
    u8 code_area[446];                      /* Code area; normally empty */
    PARTITION_RECORD partitions[4];         /* 4 primary partitions */
    u16 signature;                          /* MBR signature; 0xAA55 */
} __attribute__((__packed__)) MASTER_BOOT_RECORD;

/**
 * struct BIOS_PARAMETER_BLOCK - BIOS parameter block (bpb) structure.
 */
typedef struct {
	u16 bytes_per_sector;					/* Size of a sector in bytes. */
	u8  sectors_per_cluster;				/* Size of a cluster in sectors. */
	u16 reserved_sectors;					/* zero */
	u8  fats;								/* zero */
	u16 root_entries;						/* zero */
	u16 sectors;							/* zero */
	u8  media_type;							/* 0xf8 = hard disk */
	u16 sectors_per_fat;					/* zero */
	u16 sectors_per_track;					/* Required to boot Windows. */
	u16 heads;								/* Required to boot Windows. */
	u32 hidden_sectors;						/* Offset to the start of the partition */
	u32 large_sectors;						/* zero */
} __attribute__((__packed__)) BIOS_PARAMETER_BLOCK;

/**
 * struct NTFS_BOOT_SECTOR - NTFS boot sector structure.
 */
typedef struct {
	u8  jump[3];							/* Irrelevant (jump to boot up code).*/
	u64 oem_id;								/* Magic "NTFS    ". */
	BIOS_PARAMETER_BLOCK bpb;				/* See BIOS_PARAMETER_BLOCK. */
	u8 physical_drive;						/* 0x00 floppy, 0x80 hard disk */
	u8 current_head;						/* zero */
	u8 extended_boot_signature; 			/* 0x80 */
	u8 reserved2;							/* zero */
	s64 number_of_sectors;					/* Number of sectors in volume. */
	s64 mft_lcn;							/* Cluster location of mft data. */
	s64 mftmirr_lcn;						/* Cluster location of copy of mft. */
	s8  clusters_per_mft_record;			/* Mft record size in clusters. */
	u8  reserved0[3];						/* zero */
	s8  clusters_per_index_record;			/* Index block size in clusters. */
	u8  reserved1[3];						/* zero */
	u64 volume_serial_number;				/* Irrelevant (serial number). */
	u32 checksum;							/* Boot sector checksum. */
	u8  bootstrap[426];						/* Irrelevant (boot up code). */
	u16 end_of_sector_marker;				/* End of bootsector magic. */
} __attribute__((__packed__)) NTFS_BOOT_SECTOR;

/**
 * EXTENDED_PARTITION - Block device extended boot record
 */
typedef struct _EXTENDED_BOOT_RECORD {
    u8 code_area[446];                      /* Code area; normally empty */
    PARTITION_RECORD partition;             /* Primary partition */
    PARTITION_RECORD next_ebr;              /* Next extended boot record in the chain */
    u8 reserved[32];                        /* Normally empty */
    u16 signature;                          /* EBR signature; 0xAA55 */
} __attribute__((__packed__)) EXTENDED_BOOT_RECORD;

#ifdef DEBUG_MOUNTALL
#define debug_printf(fmt, args...) \
	fprintf(stderr, "%s:%d:" fmt, __FUNCTION__, __LINE__, ##args)
#else
#define debug_printf(fmt, args...)
#endif

DEVICE_STRUCT part[2][MAX_DEVICES];
static int devnum = 0;

static void AddPartition(sec_t sector, int device, int type)
{
	if (devnum >= MAX_DEVICES)
		return;

	part[device][devnum].sector = sector;

	if(device == DEVICE_SD)
		part[device][devnum].interface = (DISC_INTERFACE *)sd;
	else
		part[device][devnum].interface = (DISC_INTERFACE *)usb;

	part[device][devnum].type = type;
	devnum++;
}

static int FindPartitions(int device)
{
	DISC_INTERFACE *interface;

	if(device == DEVICE_SD)
		interface = (DISC_INTERFACE *)sd;
	else
		interface = (DISC_INTERFACE *)usb;

	MASTER_BOOT_RECORD mbr;
	PARTITION_RECORD *partition = NULL;
	devnum = 0;

	sec_t part_lba = 0;
	int i;

	union
	{
		u8 buffer[BYTES_PER_SECTOR];
		MASTER_BOOT_RECORD mbr;
		EXTENDED_BOOT_RECORD ebr;
		NTFS_BOOT_SECTOR boot;
	} sector;

	// Read the first sector on the device
	if (!interface->readSectors(0, 1, &sector.buffer))
	{
		errno = EIO;
		return -1;
	}

	// If this is the devices master boot record
	printf("0x%x\n", sector.mbr.signature);
	if (sector.mbr.signature == MBR_SIGNATURE)
	{
		memcpy(&mbr, &sector, sizeof(MASTER_BOOT_RECORD));
		printf("Valid Master Boot Record found\n");

		// Search the partition table for all partitions (max. 4 primary partitions)
		for (i = 0; i < 4; i++)
		{
			partition = &mbr.partitions[i];
			part_lba = le32_to_cpu(mbr.partitions[i].lba_start);

			printf(
					"Partition %i: %s, sector %d, type 0x%x\n",
					i + 1,
					partition->status == PARTITION_STATUS_BOOTABLE ? "bootable (active)"
							: "non-bootable", part_lba, partition->type);

			// Figure out what type of partition this is
			switch (partition->type)
			{
				// Ignore empty partitions
				case PARTITION_TYPE_EMPTY:
					continue;

				// NTFS partition
				case PARTITION_TYPE_NTFS:
				{
					printf("Partition %i: Claims to be NTFS\n", i + 1);

					// Read and validate the NTFS partition
					if (interface->readSectors(part_lba, 1, &sector))
					{
						debug_printf("sector.boot.oem_id: 0x%x\n", sector.boot.oem_id);
						debug_printf("NTFS_OEM_ID: 0x%x\n", NTFS_OEM_ID);
						if (sector.boot.oem_id == NTFS_OEM_ID)
						{
							printf("Partition %i: Valid NTFS boot sector found\n", i + 1);
							AddPartition(part_lba, device, T_NTFS);
						}
						else
						{
							printf("Partition %i: Invalid NTFS boot sector, not actually NTFS\n", i + 1);
						}
					}

					break;
				}
				// DOS 3.3+ or Windows 95 extended partition
				case PARTITION_TYPE_DOS33_EXTENDED:
				case PARTITION_TYPE_WIN95_EXTENDED:
				{
					printf("Partition %i: Claims to be Extended\n", i + 1);

					// Walk the extended partition chain, finding all NTFS partitions within it
					sec_t ebr_lba = part_lba;
					sec_t next_erb_lba = 0;
					do
					{
						// Read and validate the extended boot record
						if (interface->readSectors(ebr_lba + next_erb_lba, 1,
								&sector))
						{
							if (sector.ebr.signature == EBR_SIGNATURE)
							{
								debug_printf(
										"Logical Partition @ %d: type 0x%x\n",
										ebr_lba + next_erb_lba,
										sector.ebr.partition.status
												== PARTITION_STATUS_BOOTABLE ? "bootable (active)"
												: "non-bootable",
										sector.ebr.partition.type);

								// Get the start sector of the current partition
								// and the next extended boot record in the chain
								part_lba = ebr_lba + next_erb_lba
										+ le32_to_cpu(
												sector.ebr.partition.lba_start);
								next_erb_lba = le32_to_cpu(
										sector.ebr.next_ebr.lba_start);

								// Check if this partition has a valid NTFS boot record
								if (interface->readSectors(part_lba, 1, &sector))
								{
									if (sector.boot.oem_id == NTFS_OEM_ID)
									{
										printf(
												"Logical Partition @ %d: Valid NTFS boot sector found\n",
												part_lba);
										if (sector.ebr.partition.type
												!= PARTITION_TYPE_NTFS)
										{
											printf(
													"Logical Partition @ %d: Is NTFS but type is 0x%x; 0x%x was expected\n",
													part_lba,
													sector.ebr.partition.type,
													PARTITION_TYPE_NTFS);
										}
										AddPartition(part_lba, device, T_NTFS);
									}
									else if (!memcmp(sector.buffer
											+ BPB_FAT16_fileSysType, FAT_SIG,
											sizeof(FAT_SIG)) || !memcmp(
											sector.buffer
													+ BPB_FAT32_fileSysType,
											FAT_SIG, sizeof(FAT_SIG)))
									{
										printf("Partition : Valid FAT boot sector found\n");
										AddPartition(part_lba, device, T_FAT);
									}
								}
							}
							else
							{
								next_erb_lba = 0;
							}
						}
					} while (next_erb_lba);
					break;
				}

				// Unknown or unsupported partition type
				default:
				{
					// Check if this partition has a valid NTFS boot record anyway,
					// it might be misrepresented due to a lazy partition editor
					if (interface->readSectors(part_lba, 1, &sector))
					{
						if (sector.boot.oem_id == NTFS_OEM_ID)
						{
							printf("Partition %i: Valid NTFS boot sector found\n",i + 1);
							if (partition->type != PARTITION_TYPE_NTFS)
							{
								printf(
										"Partition %i: Is NTFS but type is 0x%x; 0x%x was expected\n",
										i + 1, partition->type,
										PARTITION_TYPE_NTFS);
							}
							AddPartition(part_lba, device, T_NTFS);
						}
						else if (!memcmp(sector.buffer + BPB_FAT16_fileSysType,
								FAT_SIG, sizeof(FAT_SIG)) || !memcmp(
								sector.buffer + BPB_FAT32_fileSysType, FAT_SIG,
								sizeof(FAT_SIG)))
						{
							printf("Partition : Valid FAT boot sector found\n");
							AddPartition(part_lba, device, T_FAT);
						}
					}
					break;
				}
			}
		}
	}
	else // it is assumed this device has no master boot record
	{
		printf("No Master Boot Record was found!\n");

		// As a last-ditched effort, search the first 64 sectors of the device for stray NTFS partitions
		for (i = 0; i < 64; i++)
		{
			if (interface->readSectors(i, 1, &sector))
			{
				if (sector.boot.oem_id == NTFS_OEM_ID)
				{
					printf("Valid NTFS boot sector found at sector %d!\n", i);
					AddPartition(i, device, T_NTFS);
				}
			}
			else if (!memcmp(sector.buffer + BPB_FAT16_fileSysType, FAT_SIG,
					sizeof(FAT_SIG)) || !memcmp(sector.buffer
					+ BPB_FAT32_fileSysType, FAT_SIG, sizeof(FAT_SIG)))
			{
				printf("Partition : Valid FAT boot sector found\n");
				AddPartition(i, device, T_FAT);
			}
		}
	}
	return devnum;
}

static void UnmountPartitions(int device)
{
	for(int i=0; i < MAX_DEVICES; i++)
	{
		if(part[device][i].type == T_FAT)
			fatUnmount(part[device][i].mount);
		else if(part[device][i].type == T_NTFS)
			ntfsUnmount(part[device][i].mount, false);
		
		part[device][i].name[0] = 0;
		part[device][i].mount[0] = 0;
		part[device][i].sector = 0;
		part[device][i].interface = NULL;
		part[device][i].type = 0;
	}
}

/****************************************************************************
 * MountPartitions
 * 
 * Shuts down the device
 * Attempts to startup the device specified and mounts all partitions
 ***************************************************************************/

static bool MountPartitions(int device, int silent)
{
	bool mounted = false;
	int retry = 1;
	const DISC_INTERFACE* disc = NULL;

	switch(device)
	{
		case DEVICE_SD:
			disc = sd;
			break;
		case DEVICE_USB:
			disc = usb;
			break;
		default:
			return false; // unknown device
	}

	while(retry)
	{
		disc->shutdown();

		if(disc->startup() && disc->isInserted())
		{
			int numFound = FindPartitions(device);

			for(int i=0; i < numFound; i++)
			{
				sprintf(part[device][i].mount, "%s%i", prefix[device], i+1);

				if(part[device][i].type == T_FAT)
				{
					fatMount(part[device][i].mount, disc, part[device][i].sector, 3, 256);
					fatGetVolumeLabel(part[device][i].mount, part[device][i].name);
				}
				else
				{
					ntfsMount(part[device][i].mount, disc, part[device][i].sector, 256, 3, NTFS_DEFAULT | NTFS_RECOVER);
					strcpy(part[device][i].name, ntfsGetVolumeName(part[device][i].mount));
				}
			}
			mounted = true;
		}

		if(mounted || silent)
			break;

		if(device == DEVICE_SD)
			retry = ErrorPromptRetry("SD card not found!");
		else
			retry = ErrorPromptRetry("USB drive not found!");
	}
	isInserted[device] = mounted;
	return mounted;
}

void MountAllDevices()
{
	MountPartitions(DEVICE_SD, SILENT);
	MountPartitions(DEVICE_USB, SILENT);
}

/****************************************************************************
 * MountDVD()
 *
 * Tests if a ISO9660 DVD is inserted and available, and mounts it
 ***************************************************************************/
bool MountDVD(bool silent)
{
	bool mounted = false;
	int retry = 1;

	if(isInserted[DEVICE_DVD])
		return true;

	while(retry)
	{
		ShowAction("Loading DVD...");

		if(!WIIDVD_DiscPresent())
		{
			if(silent)
				break;

			retry = ErrorPromptRetry("No disc inserted!");
		}
		else if(WIIDVD_Mount() < 0)
		{
			if(silent)
				break;
			
			retry = ErrorPromptRetry("Invalid DVD.");
		}
		else
		{
			mounted = true;
			break;
		}
	}
	CancelAction();
	isInserted[DEVICE_DVD] = mounted;
	return mounted;
}

static bool FindDevice(char * filepath, int * device, int * devnum)
{
	if(!filepath || filepath[0] == 0)
		return false;

	int tmp = -1;

	if(strncmp(filepath, "dvd:", 4) == 0)
	{
		*device = DEVICE_DVD;
		return true;
	}

	if(strncmp(filepath, "sd", 2) == 0)
	{
		tmp = atoi(&filepath[2]);

		if(tmp > 0 && tmp < 10)
		{
			*devnum = tmp;
			*device = DEVICE_SD;
			return true;
		}
		else
		{
			return false;
		}
	}

	tmp = atoi(&filepath[3]);

	if(tmp > 0 && tmp < 10)
	{
		*devnum = tmp;

		if(strncmp(filepath, "usb", 3) == 0)
		{
			*device = DEVICE_USB;
			return true;
		}
		else if(strncmp(filepath, "smb", 3) == 0)
		{
			*device = DEVICE_SMB;
			return true;
		}
		else if(strncmp(filepath, "ftp", 3) == 0)
		{
			*device = DEVICE_FTP;
			return true;
		}
	}
	return false;
}

bool CheckMount(int device, int devnum, bool silent)
{
	if(devnum < 1 || devnum > 9 || (device != DEVICE_SD && device != DEVICE_USB))
		return false;
	
	if(isInserted[device] && part[device][devnum-1].type > 0)
		return true;

	if(MountPartitions(device, silent) >= devnum)
		return true;

	return false;
}

/****************************************************************************
 * ChangeInterface
 * Attempts to mount/configure the device specified
 ***************************************************************************/
bool ChangeInterface(int device, int devnum, bool silent)
{
	bool mounted = false;
	
	switch(device)
	{
		case DEVICE_SD:
		case DEVICE_USB:
			mounted = CheckMount(device, devnum, silent);
			break;
		case DEVICE_DVD:
			mounted = MountDVD(silent);
			break;
		case DEVICE_SMB:
			mounted = ConnectShare(devnum, silent);
			break;
		case DEVICE_FTP:
			mounted = ConnectFTP(devnum, silent);
			break;
	}

	if(mounted)
	{
		currentDevice = device;
		currentDeviceNum = devnum;
	}

	return mounted;
}

bool ChangeInterface(char * filepath, bool silent)
{
	if(!filepath || filepath[0] == 0)
		return false;

	int device = -1;
	int devnum = -1;

	if(!FindDevice(filepath, &device, &devnum))
		return false;

	return ChangeInterface(device, devnum, silent);
}

void CreateAppPath(char * origpath)
{
	if(!origpath || origpath[0] == 0)
		return;

	if(strncmp(origpath, "fat", 3) == 0)
		sprintf(appPath, "sd1:/%s", &origpath[5]);
	else if(strncmp(origpath, "sd", 2) == 0)
		sprintf(appPath, "sd1:/%s", &origpath[4]);
	else if(strncmp(origpath, "usb", 2) == 0)
		sprintf(appPath, "usb1:/%s", &origpath[5]);
	else
		return;

	char * loc = strrchr(appPath,'/');
	if (loc != NULL)
		*loc = 0; // strip file name
}

/****************************************************************************
 * StripExt
 *
 * Strips an extension from a filename
 ***************************************************************************/
void StripExt(char* string)
{
	char* loc_dot;

	if(string == NULL || strlen(string) < 4)
		return;

	loc_dot = strrchr(string,'.');
	if (loc_dot != NULL)
		*loc_dot = 0; // strip file extension
}

/****************************************************************************
 * CleanFilename
 *
 * Strips out all of the useless nonsense from a filename
 ***************************************************************************/
void CleanFilename(char* string)
{
	if(string == NULL || strlen(string) < 4)
		return;
}

static bool ParseDirEntries()
{
	if(!dirIter)
		return false;

	char filename[MAXPATHLEN];
	char *ext;
	struct stat filestat;
	bool isPlaylist;

	int i = 0;
	int j, res;

	while(i < 20)
	{
		isPlaylist = false; // assume this file is not a playlist
		res = dirnext(dirIter,filename,&filestat);

		if(res != 0)
			break;

		if(strcmp(filename,".") == 0)
			continue;
		
		ext = strrchr(filename,'.');
		
		if(ext != NULL)
		{		
			ext++;
			
			// check if this is a playlist
			j=0;
			do
			{
				if (!strcasecmp(ext, validPlaylistExtensions[j++]))
				{
					isPlaylist = true;
					break;
				}
			} while (validPlaylistExtensions[j][0] != 0);
		}

		// check that this file's extension is on the list of visible file types
		if(CESettings.filterFiles && (filestat.st_mode & _IFDIR) == 0 && !isPlaylist)
		{
			if(ext == NULL)
				continue; // file does not have an extension - skip it

			j=0;
			
			if(currentMenu == MENU_BROWSE_VIDEOS)
			{
				do
				{
					if (strcasecmp(ext, validVideoExtensions[j]) == 0)
						break;
				} while (validVideoExtensions[++j][0] != 0);
				if (validVideoExtensions[j][0] == 0) // extension not found
					continue;
			}
			else if(currentMenu == MENU_BROWSE_MUSIC)
			{
				do
				{
					if (strcasecmp(ext, validAudioExtensions[j]) == 0)
						break;
				} while (validAudioExtensions[++j][0] != 0);
				if (validAudioExtensions[j][0] == 0) // extension not found
					continue;
			}
			else if(currentMenu == MENU_BROWSE_PICTURES)
			{
				do
				{
					if (strcasecmp(ext, validImageExtensions[j]) == 0)
						break;
				} while (validImageExtensions[++j][0] != 0);
				if (validImageExtensions[j][0] == 0) // extension not found
					continue;
			}
		}

		// add the entry
		if(AddBrowserEntry())
		{
			strncpy(browserList[browser.numEntries+i].filename, filename, MAXJOLIET);
			browserList[browser.numEntries+i].length = filestat.st_size;
			browserList[browser.numEntries+i].mtime = filestat.st_mtime;
			browserList[browser.numEntries+i].isdir = (filestat.st_mode & _IFDIR) == 0 ? 0 : 1; // flag this as a dir
			
			if(browserList[browser.numEntries+i].isdir)
			{
				if(strcmp(filename, "..") == 0)
					sprintf(browserList[browser.numEntries+i].displayname, "Up One Level");
				else
					strncpy(browserList[browser.numEntries+i].displayname, browserList[browser.numEntries+i].filename, MAXJOLIET);
				browserList[browser.numEntries+i].icon = ICON_FOLDER;
			}
			else
			{
				if(isPlaylist)
					browserList[browser.numEntries+i].isplaylist = 1;
				
				strncpy(browserList[browser.numEntries+i].displayname, browserList[browser.numEntries+i].filename, MAXJOLIET);
				browserList[browser.numEntries+i].icon = ICON_NONE;

				// hide the file's extension
				if(CESettings.hideExtensions)
					StripExt(browserList[browser.numEntries+i].displayname);

				// strip unwanted stuff from the filename
				if(CESettings.cleanFilenames)
					CleanFilename(browserList[browser.numEntries+i].displayname);
			}
			i++;
		}
		else
		{
			parseHalt = true;
		}
	}

	// Sort the file list
	if(i > 0)
	{
		qsort(browserList, browser.numEntries+i, sizeof(BROWSERENTRY), FileSortCallback);
	}

	// try to find and select the last loaded file
	if(selectLoadedFile == 1 && res != 0 && loadedFile[0] != 0 && browser.dir[0] != 0)
	{
		int indexFound = -1;
		int dirLen = strlen(browser.dir);
		int fileLen = strlen(loadedFile);
		char file[MAXPATHLEN];
		
		if(fileLen > dirLen && strncmp(loadedFile, browser.dir, dirLen) == 0)
			strcpy(file, &loadedFile[dirLen]);
		else
			strcpy(file, loadedFile);
		
		for(j=1; j < browser.numEntries + i; j++)
		{
			if(strcmp(browserList[j].filename, file) == 0)
			{
				indexFound = j;
				break;
			}
		}

		// move to this file
		if(indexFound > 0)
		{
			if(indexFound > FILE_PAGESIZE)
			{			
				browser.pageIndex = (ceil(indexFound/FILE_PAGESIZE*1.0)) * FILE_PAGESIZE;
				
				if(browser.pageIndex + FILE_PAGESIZE > browser.numEntries + i)
				{
					browser.pageIndex = browser.numEntries + i - FILE_PAGESIZE;
				}
			}
			browser.selIndex = indexFound;
		}
		selectLoadedFile = 2; // selecting done
	}
	
	browser.numEntries += i;

	if(res != 0 || parseHalt)
	{
		dirclose(dirIter); // close directory
		dirIter = NULL;
		return false; // no more entries
	}
	return true; // more entries
}

/***************************************************************************
 * Browse subdirectories
 **************************************************************************/
int
ParseDirectory(bool waitParse)
{
	char msg[128];
	int retry = 1;
	bool mounted = false;

	ResetBrowser(); // reset browser

	// open the directory
	while(dirIter == NULL && retry == 1)
	{
		mounted = ChangeInterface(browser.dir, NOTSILENT);

		if(mounted)
			dirIter = diropen(browser.dir);
		else
			return -1;

		if(dirIter == NULL)
		{
			sprintf(msg, "Error opening %s", browser.dir);
			retry = ErrorPromptRetry(msg);
		}
	}

	// if we can't open the dir, try higher levels
	if (dirIter == NULL)
	{
		char * devEnd = strrchr(browser.dir, '/');

		while(!IsDeviceRoot(browser.dir))
		{
			devEnd[0] = 0; // strip slash
			devEnd = strrchr(browser.dir, '/');

			if(devEnd == NULL)
				break;

			devEnd[1] = 0; // strip remaining file listing
			dirIter = diropen(browser.dir);
			if (dirIter)
				break;
		}
	}
	
	if(dirIter == NULL)
		return -1;

	if(IsDeviceRoot(browser.dir))
	{
		browser.numEntries = 1;
		sprintf(browserList[0].filename, "..");
		sprintf(browserList[0].displayname, "Up One Level");
		browserList[0].length = 0;
		browserList[0].mtime = 0;
		browserList[0].isdir = 1; // flag this as a dir
		browserList[0].icon = ICON_FOLDER;
	}

	parseHalt = false;
	ParseDirEntries(); // index first 20 entries
	ResumeParseThread(); // index remaining entries

	if(waitParse) // wait for complete parsing
	{
		ShowAction("Loading...");

		if(parsethread != LWP_THREAD_NULL)
		{
			LWP_JoinThread(parsethread, NULL);
			parsethread = LWP_THREAD_NULL;
		}

		CancelAction();
	}

	return browser.numEntries;
}

int LoadPlaylist()
{
	char * playlistEntry;
	
	play_tree_t * list = parse_playlist_file(currentPlaylist);
	
	if(!list)
		return 0;
	
	if(playlist)
	{
		free(playlist);
		playlist = NULL;
	}

	playlistSize = 0;
	
	play_tree_iter_t *pt_iter = NULL;

	if((pt_iter = pt_iter_create(&list, NULL)))
	{
		while ((playlistEntry = pt_iter_get_next_file(pt_iter)) != NULL)
		{
			playlist = (MEDIAENTRY *)realloc(playlist, (playlistSize + 1) * sizeof(MEDIAENTRY));
			memset(&(playlist[playlistSize]), 0, sizeof(MEDIAENTRY)); // clear the new entry
			strncpy(playlist[playlistSize].address, playlistEntry, MAXPATHLEN);
			playlistSize++;
		}
		pt_iter_destroy(&pt_iter);
	}
	return playlistSize;
}

int ParsePlaylist()
{
	AddBrowserEntry();
	sprintf(browserList[0].filename, "..");
	sprintf(browserList[0].displayname, "Up One Level");
	browserList[0].length = 0;
	browserList[0].mtime = 0;
	browserList[0].isdir = 1;
	browserList[0].icon = ICON_FOLDER;
	browser.numEntries++;
	
	int i;
	char * start;
	
	for(i=0; i < playlistSize; i++)
	{
		AddBrowserEntry();
		sprintf(browserList[i+1].filename, playlist[i].address);
		start = strrchr(playlist[i].address,'/');
		if(start != NULL) // start up starting part of path
		{
			start++;
			sprintf(browserList[i+1].displayname, start);
		}
		else
		{
			sprintf(browserList[i+1].displayname, playlist[i].address);
		}
		browserList[i+1].length = 0;
		browserList[i+1].mtime = 0;
		browserList[i+1].isdir = 0;
		browserList[i+1].isplaylist = 0;
		browserList[i+1].icon = ICON_NONE;
	}
	browser.numEntries += i;
	return browser.numEntries;
}

int ParseOnlineMedia()
{
	if(browser.dir[0] != 0)
	{
		AddBrowserEntry();
		sprintf(browserList[0].filename, "..");
		sprintf(browserList[0].displayname, "Up One Level");
		browserList[0].length = 0;
		browserList[0].mtime = 0;
		browserList[0].isdir = 1;
		browserList[0].icon = ICON_FOLDER;
		browser.numEntries++;
	}
	
	for(int i=0; i < onlinemediaSize; i++)
	{
		int filepathLen = strlen(onlinemediaList[i].filepath);
		int dirLen = strlen(browser.dir);
		
		// add file
		if(strcmp(browser.dir, onlinemediaList[i].filepath) == 0)
		{
			AddBrowserEntry();
			strncpy(browserList[browser.numEntries].filename, onlinemediaList[i].address, MAXPATHLEN);
			strncpy(browserList[browser.numEntries].displayname, onlinemediaList[i].displayname, MAXJOLIET);
			browserList[browser.numEntries].length = 0;
			browserList[browser.numEntries].mtime = 0;
			browserList[browser.numEntries].isdir = 0;
			browserList[browser.numEntries].isplaylist = 1;
			browserList[browser.numEntries].icon = ICON_NONE;
			browser.numEntries++;
		}
		else if(filepathLen > dirLen && 
			strncmp(browser.dir, onlinemediaList[i].filepath, dirLen) == 0)
		{
			char folder[MAXPATHLEN];
			strncpy(folder, &onlinemediaList[i].filepath[dirLen], MAXPATHLEN);
			char * end = strchr(folder, '/');
			if(end) *end = 0; // strip end
			
			// check if this folder was already added
			bool matchFound = false;

			for(int j=0; j < browser.numEntries; j++)
			{
				if(strcmp(browserList[j].filename, folder) == 0)
				{
					matchFound = true;
					break;
				}
			}
			
			if(!matchFound)
			{			
				// add the folder
				AddBrowserEntry();
				strncpy(browserList[browser.numEntries].filename, folder, MAXPATHLEN);
				strncpy(browserList[browser.numEntries].displayname, folder, MAXJOLIET);
				browserList[browser.numEntries].length = 0;
				browserList[browser.numEntries].mtime = 0;
				browserList[browser.numEntries].isdir = 1;
				browserList[browser.numEntries].icon = ICON_FOLDER;
				browser.numEntries++;
			}
		}
	}
	
	// Sort the file list
	qsort(browserList, browser.numEntries, sizeof(BROWSERENTRY), FileSortCallback);
	return browser.numEntries;
}

/****************************************************************************
 * LoadFile
 ***************************************************************************/
size_t LoadFile (char * buffer, char *filepath, bool silent)
{
	size_t size = 0, offset = 0, readsize = 0;
	int retry = 1;
	FILE * file;

	// stop checking if devices were removed/inserted
	// since we're loading a file
	HaltDeviceThread();

	// halt parsing
	HaltParseThread();

	// open the file
	while(!size && retry)
	{
		if(!ChangeInterface(filepath, silent))
			break;

		file = fopen (filepath, "rb");

		if(!file)
		{
			if(silent)
				break;

			retry = ErrorPromptRetry("Error opening file!");
			continue;
		}

		fseeko(file,0,SEEK_END);
		size = ftello(file);
		fseeko(file,0,SEEK_SET);

		while(!feof(file))
		{
			ShowProgress ("Loading...", offset, size);
			readsize = fread (buffer + offset, 1, 4096, file); // read in next chunk

			if(readsize <= 0)
				break; // reading finished (or failed)

			offset += readsize;
		}
		fclose (file);
		size = offset;
		CancelAction();
	}

	// go back to checking if devices were inserted/removed
	ResumeDeviceThread();
	CancelAction();
	return size;
}

/****************************************************************************
 * SaveFile
 * Write buffer to file
 ***************************************************************************/
size_t SaveFile (char * buffer, char *filepath, size_t datasize, bool silent)
{
	size_t written = 0;
	size_t writesize, nextwrite;
	int retry = 1;
	FILE * file;

	if(datasize == 0)
		return 0;

	ShowAction("Saving...");

	// stop checking if devices were removed/inserted
	// since we're saving a file
	HaltDeviceThread();

	// halt parsing
	HaltParseThread();

	while(!written && retry)
	{
		if(!ChangeInterface(filepath, silent))
			break;

		file = fopen (filepath, "wb");

		if(!file)
		{
			if(silent)
				break;

			retry = ErrorPromptRetry("Error creating file!");
			continue;
		}

		while(written < datasize)
		{
			if(datasize - written > 4096) nextwrite=4096;
			else nextwrite = datasize-written;
			writesize = fwrite (buffer+written, 1, nextwrite, file);
			if(writesize != nextwrite) break; // write failure
			written += writesize;
		}
		fclose (file);

		if(written != datasize) written = 0;

		if(!written)
		{
			retry = ErrorPromptRetry("Error saving file!");
		}
	}

	// go back to checking if devices were inserted/removed
	ResumeDeviceThread();

	CancelAction();
    return written;
}
