/****************************************************************************
 * WiiMC
 * Tantric 2009-2010
 *
 * fileop.cpp
 * File operations
 ***************************************************************************/

#include <gccore.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <ogcsys.h>
#include <sys/dir.h>
#include <sys/stat.h>
#include <errno.h>
#include <ntfs.h>
#include <fat.h>
#include <di/di.h>
#include <iso9660.h>
#include <sdcard/wiisd_io.h>
#include <ogc/usbstorage.h>
#include <ogc/machine/processor.h>

extern "C" {
#include "mplayer/playtree.h"
#include "mplayer/stream/url.h"
}

#include "wiimc.h"
#include "fileop.h"
#include "networkop.h"
#include "menu.h"
#include "filebrowser.h"
#include "settings.h"
#include "musicplaylist.h"
#include "libwiigui/gui.h"
#include "utils/http.h"
#include "utils/gettext.h"

#define PARSESTACK (8*1024)
#define DEVICESTACK (8*1024)

static u8 parsestack[PARSESTACK] ATTRIBUTE_ALIGN (32);
static u8 devicestack[DEVICESTACK] ATTRIBUTE_ALIGN (32);

extern "C" u32 __di_check_ahbprot(void);

#define THREAD_SLEEP 100

int currentDevice = -1;
int currentDeviceNum = -1;
bool isInserted[3] = { false, false, false };
bool devicesChanged = false;
u64 dvdLastUsed = 0;

static char prefix[2][4] = { "sd", "usb" };

static const DISC_INTERFACE* sd = &__io_wiisd;
static const DISC_INTERFACE* usb = &__io_usbstorage;
static const DISC_INTERFACE* dvd = &__io_wiidvd;

// folder parsing thread
static lwp_t parsethread = LWP_THREAD_NULL;
static int parseHalt = 0;
static DIR_ITER * dirIter = NULL;
static bool ParseDirEntries();
int findLoadedFile = 0;
bool selectLoadedFile = false;

// device thread
static lwp_t devicethread = LWP_THREAD_NULL;
static int deviceHalt = 0;

/****************************************************************************
 * devicecallback
 *
 * This checks our devices for changes (SD/USB/DVD)
 ***************************************************************************/
static int devsleep;
static bool MountPartitions(int device, int silent);
static void UnmountPartitions(int device);

static void * devicecallback (void *arg)
{
	while (1)
	{
		if(isInserted[DEVICE_SD])
		{
			if(!sd->isInserted()) // device was removed
			{
				UnmountPartitions(DEVICE_SD);
				sd->shutdown();

				if(strlen(loadedFile) > 2 && strncmp(loadedFile, "sd", 2) == 0)
				{
					loadedFile[0] = 0;
					StopMPlayerFile();
					DisableVideoImg();
				}
				isInserted[DEVICE_SD] = false;
				devicesChanged = true;
			}
		}
		else if(sd->startup() && sd->isInserted()) // device was inserted
		{
			MountPartitions(DEVICE_SD, SILENT);
			isInserted[DEVICE_SD] = true;
			devicesChanged = true;
		}

		if(isInserted[DEVICE_USB])
		{
			if(!usb->isInserted()) // device was removed
			{
				UnmountPartitions(DEVICE_USB);
				usb->shutdown();

				if(strlen(loadedFile) > 3 && strncmp(loadedFile, "usb", 3) == 0)
				{
					loadedFile[0] = 0;
					StopMPlayerFile();
					DisableVideoImg();
				}
				isInserted[DEVICE_USB] = false;
				devicesChanged = true;
			}
		}
		else if(usb->startup() && usb->isInserted()) // device was inserted
		{
			MountPartitions(DEVICE_USB, SILENT);
			isInserted[DEVICE_USB] = true;
			devicesChanged = true;
		}

		if(isInserted[DEVICE_DVD])
		{
			if(!dvd->isInserted())
			{
				dvdLastUsed = 0;
				isInserted[DEVICE_DVD] = false;
				ISO9660_Unmount();

				if(strlen(loadedFile) > 3 && strncmp(loadedFile, "dvd", 3) == 0)
				{
					loadedFile[0] = 0;
					StopMPlayerFile();
					DisableVideoImg();
					if(menuCurrent == MENU_DVD)
						UndoChangeMenu();
				}
				devicesChanged = true;
			}
			else if(dvdLastUsed)
			{
				if(diff_sec(dvdLastUsed, gettime()) > 60)
				{
					DI_StopMotor();
					dvdLastUsed = 0;
				}
			}
		}
		else if(!WiiSettings.dvdDisabled && dvd->isInserted())
		{
			isInserted[DEVICE_DVD] = true;
			devicesChanged = true;
		}

		devsleep = 1000*1000*2; // 2 sec

		while(devsleep > 0)
		{
			if(deviceHalt == 1)
				LWP_SuspendThread(devicethread);

			if(deviceHalt == 2)
				return NULL;

			usleep(THREAD_SLEEP);
			devsleep -= THREAD_SLEEP;
		}
		UpdateCheck();
	}
	return NULL;
}

static void * parsecallback (void *arg)
{
	while (1)
	{
		LWP_SuspendThread(parsethread);

		if(parseHalt == 2)
			break;

		while(ParseDirEntries())
			usleep(THREAD_SLEEP);
	}
	return NULL;
}

/****************************************************************************
 * ResumeDeviceThread
 *
 * Signals the device thread to start, and resumes the thread
 ***************************************************************************/
void ResumeDeviceThread()
{
	deviceHalt = 0;
	
	if(devicethread == LWP_THREAD_NULL)
		LWP_CreateThread (&devicethread, devicecallback, NULL, devicestack, DEVICESTACK, 40);
	else
		LWP_ResumeThread(devicethread);
}

/****************************************************************************
 * SuspendDeviceThread
 *
 * Signals the device thread to stop
 ***************************************************************************/
void SuspendDeviceThread()
{
	deviceHalt = 1;

	if(devicethread == LWP_THREAD_NULL)
		return;

	// wait for thread to finish
	while(!LWP_ThreadIsSuspended(devicethread))
		usleep(THREAD_SLEEP);
}

/****************************************************************************
 * StopDeviceThread
 *
 * Signals the device thread to stop
 ***************************************************************************/
void StopDeviceThread()
{
	deviceHalt = 2;

	if(devicethread == LWP_THREAD_NULL)
		return;

	if(LWP_ThreadIsSuspended(devicethread))
		LWP_ResumeThread(devicethread);

	// wait for thread to finish
	LWP_JoinThread(devicethread, NULL);
	devicethread = LWP_THREAD_NULL;
}

/****************************************************************************
 * ResumeParseThread
 *
 * Signals the parse thread to start
 ***************************************************************************/
void ResumeParseThread()
{
	parseHalt = 0;

	if(parsethread == LWP_THREAD_NULL)
		LWP_CreateThread (&parsethread, parsecallback, NULL, parsestack, PARSESTACK, 40);
	else
		LWP_ResumeThread(parsethread);
}

/****************************************************************************
 * SuspendParseThread
 *
 * Signals the parse thread to stop
 ***************************************************************************/
void SuspendParseThread()
{
	parseHalt = 1;

	if(parsethread == LWP_THREAD_NULL)
		return;

	while(!LWP_ThreadIsSuspended(parsethread))
		usleep(THREAD_SLEEP);
}

/****************************************************************************
 * StopParseThread
 *
 * Signals the parse thread to stop
 ***************************************************************************/
void StopParseThread()
{
	parseHalt = 2;

	if(parsethread == LWP_THREAD_NULL)
		return;

	if(LWP_ThreadIsSuspended(parsethread))
		LWP_ResumeThread(parsethread);

	// wait for thread to finish
	LWP_JoinThread(parsethread, NULL);
	parsethread = LWP_THREAD_NULL;
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

//#define DEBUG_MOUNTALL

#ifdef DEBUG_MOUNTALL
#define debug_printf(fmt, args...) \
	fprintf(stderr, "%s:%d:" fmt, __FUNCTION__, __LINE__, ##args)
#else
#define debug_printf(fmt, args...)
#endif

DEVICE_STRUCT part[2][MAX_DEVICES];

static void AddPartition(sec_t sector, int device, int type, int *devnum)
{
	if (*devnum >= MAX_DEVICES)
		return;

	DISC_INTERFACE *disc = (DISC_INTERFACE *)sd;

	if(device == DEVICE_USB)
		disc = (DISC_INTERFACE *)usb;

	char mount[10];
	sprintf(mount, "%s%i", prefix[device], *devnum+1);

	if(type == T_FAT)
	{
		if(!fatMount(mount, disc, sector, 3, 256))
			return;
		fatGetVolumeLabel(mount, part[device][*devnum].name);
	}
	else
	{
		if(!ntfsMount(mount, disc, sector, 256, 3, NTFS_DEFAULT | NTFS_RECOVER))
			return;

		const char *name = ntfsGetVolumeName(mount);

		if(name)
			strcpy(part[device][*devnum].name, name);
		else
			part[device][*devnum].name[0] = 0;
	}

	strcpy(part[device][*devnum].mount, mount);
	part[device][*devnum].type = type;
	part[device][*devnum].interface = disc;
	part[device][*devnum].sector = sector;
	++*devnum;
}

static int FindPartitions(int device)
{
	int i;
	int devnum = 0;

	// clear list
	for(i=0; i < MAX_DEVICES; i++)
	{
		part[device][i].name[0] = 0;
		part[device][i].mount[0] = 0;
		part[device][i].sector = 0;
		part[device][i].interface = NULL;
		part[device][i].type = 0;
	}

	DISC_INTERFACE *interface;

	if(device == DEVICE_SD)
		interface = (DISC_INTERFACE *)sd;
	else
		interface = (DISC_INTERFACE *)usb;

	MASTER_BOOT_RECORD mbr;
	PARTITION_RECORD *partition = NULL;
	devnum = 0;
	sec_t part_lba = 0;

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
	debug_printf("0x%x\n", sector.mbr.signature);
	if (sector.mbr.signature == MBR_SIGNATURE)
	{
		memcpy(&mbr, &sector, sizeof(MASTER_BOOT_RECORD));
		debug_printf("Valid Master Boot Record found\n");

		// Search the partition table for all partitions (max. 4 primary partitions)
		for (i = 0; i < 4; i++)
		{
			partition = &mbr.partitions[i];
			part_lba = le32_to_cpu(mbr.partitions[i].lba_start);

			debug_printf(
					"Partition %i: %s, sector %lu, type 0x%x\n",
					i + 1,
					partition->status == PARTITION_STATUS_BOOTABLE ? "bootable (active)"
							: "non-bootable", part_lba, partition->type);

			// Figure out what type of partition this is
			switch (partition->type)
			{
				// NTFS partition
				case PARTITION_TYPE_NTFS:
				{
					debug_printf("Partition %i: Claims to be NTFS\n", i + 1);

					// Read and validate the NTFS partition
					if (interface->readSectors(part_lba, 1, &sector))
					{
						debug_printf("sector.boot.oem_id: 0x%x\n", sector.boot.oem_id);
						debug_printf("NTFS_OEM_ID: 0x%x\n", NTFS_OEM_ID);
						if (sector.boot.oem_id == NTFS_OEM_ID)
						{
							debug_printf("Partition %i: Valid NTFS boot sector found\n", i + 1);
							AddPartition(part_lba, device, T_NTFS, &devnum);
						}
						else
						{
							debug_printf("Partition %i: Invalid NTFS boot sector, not actually NTFS\n", i + 1);
						}
					}

					break;
				}
				// DOS 3.3+ or Windows 95 extended partition
				case PARTITION_TYPE_DOS33_EXTENDED:
				case PARTITION_TYPE_WIN95_EXTENDED:
				{
					debug_printf("Partition %i: Claims to be Extended\n", i + 1);

					// Walk the extended partition chain, finding all NTFS partitions within it
					sec_t ebr_lba = part_lba;
					sec_t next_erb_lba = 0;
					do
					{
						// Read and validate the extended boot record
						if (interface->readSectors(ebr_lba + next_erb_lba, 1, &sector))
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
										debug_printf(
												"Logical Partition @ %d: Valid NTFS boot sector found\n",
												part_lba);
										if (sector.ebr.partition.type
												!= PARTITION_TYPE_NTFS)
										{
											debug_printf(
													"Logical Partition @ %d: Is NTFS but type is 0x%x; 0x%x was expected\n",
													part_lba,
													sector.ebr.partition.type,
													PARTITION_TYPE_NTFS);
										}
										AddPartition(part_lba, device, T_NTFS, &devnum);
									}
									else if (!memcmp(sector.buffer
											+ BPB_FAT16_fileSysType, FAT_SIG,
											sizeof(FAT_SIG)) || !memcmp(
											sector.buffer
													+ BPB_FAT32_fileSysType,
											FAT_SIG, sizeof(FAT_SIG)))
									{
										debug_printf("Partition : Valid FAT boot sector found\n");
										AddPartition(part_lba, device, T_FAT, &devnum);
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

				// Ignore empty partitions
				case PARTITION_TYPE_EMPTY:
					debug_printf("Partition %i: Claims to be empty\n", i + 1);
				// Unknown or unsupported partition type
				default:
				{
					// Check if this partition has a valid NTFS boot record anyway,
					// it might be misrepresented due to a lazy partition editor
					if (interface->readSectors(part_lba, 1, &sector))
					{
						if (sector.boot.oem_id == NTFS_OEM_ID)
						{
							debug_printf("Partition %i: Valid NTFS boot sector found\n",i + 1);
							if (partition->type != PARTITION_TYPE_NTFS)
							{
								debug_printf(
										"Partition %i: Is NTFS but type is 0x%x; 0x%x was expected\n",
										i + 1, partition->type,
										PARTITION_TYPE_NTFS);
							}
							AddPartition(part_lba, device, T_NTFS, &devnum);
						}
						else if (!memcmp(sector.buffer + BPB_FAT16_fileSysType,
								FAT_SIG, sizeof(FAT_SIG)) || !memcmp(
								sector.buffer + BPB_FAT32_fileSysType, FAT_SIG,
								sizeof(FAT_SIG)))
						{
							debug_printf("Partition : Valid FAT boot sector found\n");
							AddPartition(part_lba, device, T_FAT, &devnum);
						}
					}
					break;
				}
			}
		}
	}
	if(devnum==0) // it is assumed this device has no master boot record or no partitions found
	{
		debug_printf("No Master Boot Record was found or no partitions found!\n");

		// As a last-ditched effort, search the first 64 sectors of the device for stray NTFS/FAT partitions
		for (i = 0; i < 64; i++)
		{
			if (interface->readSectors(i, 1, &sector))
			{
				if (sector.boot.oem_id == NTFS_OEM_ID)
				{
					debug_printf("Valid NTFS boot sector found at sector %d!\n", i);
					AddPartition(i, device, T_NTFS, &devnum);
					break;
				}
				else if (!memcmp(sector.buffer + BPB_FAT16_fileSysType, FAT_SIG,
						sizeof(FAT_SIG)) || !memcmp(sector.buffer
						+ BPB_FAT32_fileSysType, FAT_SIG, sizeof(FAT_SIG)))
				{
					debug_printf("Partition : Valid FAT boot sector found\n");
					AddPartition(i, device, T_FAT, &devnum);
					break;
				}
			}
		}
	}
	return devnum;
}

static void UnmountPartitions(int device)
{
	char mount[11];

	for(int i=0; i < MAX_DEVICES; i++)
	{
		if(part[device][i].type == T_FAT)
		{
			sprintf(mount, "%s:", part[device][i].mount);
			fatUnmount(mount);
		}
		else if(part[device][i].type == T_NTFS)
		{
			ntfsUnmount(part[device][i].mount, false);
		}

		part[device][i].name[0] = 0;
		part[device][i].mount[0] = 0;
		part[device][i].sector = 0;
		part[device][i].interface = NULL;
		part[device][i].type = 0;
	}
}

void UnmountAllDevices()
{
	UnmountPartitions(DEVICE_SD);
	UnmountPartitions(DEVICE_USB);
}

/****************************************************************************
 * MountPartitions
 * Attempts to mount all partitions on the specified device
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
		if(FindPartitions(device) > 0)
			mounted = true;

		if(mounted || silent)
			break;

		if(device == DEVICE_SD)
			retry = ErrorPromptRetry("SD card not found!");
		else
			retry = ErrorPromptRetry("USB drive not found!");
	}
	return mounted;
}

static bool Remount(int device, int silent)
{
	DISC_INTERFACE *disc = NULL;

	if(device == DEVICE_SD)
		disc = (DISC_INTERFACE *)sd;
	else if(device == DEVICE_USB)
		disc = (DISC_INTERFACE *)usb;
	else
		return false;

	SuspendDeviceThread();

	// unmount
	if(isInserted[device])
	{
		UnmountPartitions(device);
		disc->shutdown();

		if(strlen(loadedFile) > 3 && strncmp(loadedFile, prefix[device], strlen(prefix[device])) == 0)
		{
			loadedFile[0] = 0;
			StopMPlayerFile();
			DisableVideoImg();
		}
	}

	// try to remount
	if(!silent)
		ShowAction("Loading...");

	u64 start = gettime();
	bool mounted = false;

	while (1)
	{
		usleep(250000); // 1/4 sec

		if(disc->startup() && disc->isInserted())
		{
			mounted = true;
			MountPartitions(device, SILENT);
			break;
		}

		if(diff_sec(start, gettime()) > 10) // wait for 10 seconds for device init
			break;
	}

	if(!silent)
		CancelAction();

	isInserted[device] = mounted;
	devicesChanged = true;
	ResumeDeviceThread();
	return mounted;
}

void MountAllDevices()
{
	if(sd->startup() && sd->isInserted())
	{
		isInserted[DEVICE_SD] = true;
		MountPartitions(DEVICE_SD, SILENT);
	}
	if(usb->startup() && usb->isInserted())
	{
		isInserted[DEVICE_USB] = true;
		MountPartitions(DEVICE_USB, SILENT);
	}
	else if(!isInserted[DEVICE_SD])
	{
		Remount(DEVICE_USB, SILENT);
	}
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

	if(dvdLastUsed > 0)
		return true;

	if(__di_check_ahbprot() != 1)
	{
		if(!silent)
			ErrorPrompt("WiiMC does not have DVD access - AHBPROT is not enabled.");
		return false;
	}

	SuspendDeviceThread();

	while(retry)
	{
		if(!silent)
			ShowAction("Loading DVD...");

		if(!dvd->isInserted())
		{
			if(silent)
				break;

			retry = ErrorPromptRetry("No disc inserted!");
		}
		else if(!ISO9660_Mount())
		{
			if(silent)
				break;
			
			retry = ErrorPromptRetry("Invalid DVD.");
		}
		else
		{
			mounted = true;
			dvdLastUsed = gettime();
			break;
		}
	}

	if(!silent)
		CancelAction();

	ResumeDeviceThread();
	return mounted;
}

extern "C" {
void SetLastDVDMotorTime()
{
	if(dvdLastUsed)
		dvdLastUsed = gettime();
}

bool StartDVDMotor()
{
	if(!dvd->isInserted())
		return false;

	if(dvdLastUsed)
		return true;

	if(dvd->startup())
	{
		dvdLastUsed = gettime();
		return true;
	}
	return false;
}
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

	if(IsAllowedProtocol(filepath))
	{
		*device = DEVICE_INTERNET;
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

bool IsDeviceRoot(char * path)
{
	if(path == NULL || path[0] == 0)
		return false;

	int pathlen = strlen(path);

	if((strncmp(path, "sd", 2) == 0 && pathlen == 5) ||
		(strncmp(path, "usb", 3) == 0 && pathlen == 6) ||
		strcmp(path, "dvd:/") == 0 ||
		(strncmp(path, "smb", 3) == 0 && pathlen == 6) ||
		(strncmp(path, "ftp", 3) == 0 && pathlen == 6))
	{
		return true;
	}
	return false;
}

bool IsOnlineMediaPath(char *path)
{
	int dirLen = strlen(path);

	if(dirLen < 2 || path[dirLen-1] != '/')
		return false;

	for(int i=0; i < onlinemediaSize; i++)
	{
		int filepathLen = strlen(onlinemediaList[i].filepath);

		if(filepathLen >= dirLen && strncmp(path, onlinemediaList[i].filepath, dirLen) == 0)
			return true;
	}
	return false;
}

/****************************************************************************
 * CleanupPath()
 * Cleans up the filepath, removing double // and replacing \ with /
 ***************************************************************************/
void CleanupPath(char * path)
{
	if(!path || path[0] == 0)
		return;

	int device = -1;
	int devnum = -1;
	FindDevice(path, &device, &devnum);

	// path is invalid - wipe it
	if(device == -1)
	{
		path[0] = 0;
		return;
	}

	if(strncmp(path, "http:", 5) == 0)
	{
		char *c = strchr(&path[7], '/');
		if(c == NULL) strcat(path, "/"); // should be at least one / in URL
		return;
	}

	int pathlen = strlen(path);
	int j = 0;
	for(int i=0; i < pathlen && i < MAXPATHLEN; i++)
	{
		if(path[i] == '\\')
			path[i] = '/';

		if(j == 0 || !(path[j-1] == '/' && path[i] == '/'))
			path[j++] = path[i];
	}
	
	char ext[6];
	GetExt(path, ext);

	if(ext[0] == 0 && path[j-1] != '/')
		path[j++] = '/'; // add trailing slash

	path[j] = 0;
}

void GetFullPath(int i, char *path)
{
	if(i >= browser.size)
	{
		path[0] = 0;
		return;
	}
	
	int device = -1;
	int devnum = -1;

	if(FindDevice(browserList[i].filename, &device, &devnum))
		sprintf(path, "%s", browserList[i].filename);
	else
		sprintf(path, "%s%s", browser.dir, browserList[i].filename);
}

bool CheckMount(int device, int devnum)
{
	if(devnum < 1 || devnum > 9 || (device != DEVICE_SD && device != DEVICE_USB))
		return false;

	if(isInserted[device] && part[device][devnum-1].type > 0)
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
			mounted = CheckMount(device, devnum);
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
		case DEVICE_INTERNET:
			mounted = InitializeNetwork(silent);
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

void CreateLoadPath(char * origpath)
{
	if(!origpath || origpath[0] == 0)
		return;

	if(strncmp(origpath, "fat", 3) == 0)
		sprintf(loadPath, "sd1:/%s", &origpath[5]);
	else if(strncmp(origpath, "sd", 2) == 0)
		sprintf(loadPath, "sd1:/%s", &origpath[4]);
	else if(strncmp(origpath, "usb", 3) == 0)
		sprintf(loadPath, "usb1:/%s", &origpath[5]);
	else
		return;

	char * loc = strrchr(loadPath,'/');
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

void GetExt(char *file, char *ext)
{
	ext[0] = 0;

	if(!file || file[strlen(file)-1] == '/')
		return;

	char *file_ext = strrchr(file,'.');

	if(file_ext == NULL)
		return;

	file_ext++;
	int i = 0;

	while(i < 5 && file_ext[i] != '?' && file_ext[i] != 0)
	{
		ext[i] = file_ext[i];
		i++;
	}

	// extension is too long
	if(i == 5 && file_ext[i] != '?' && file_ext[i] != 0)
	{
		ext[0] = 0;
		return;
	}

	ext[i] = 0;

	// check if this extension contains valid characters
	for(int n=0; n < i; n++)
	{
		if(!isalnum(ext[n]))
		{
			ext[0] = 0;
			break;
		}
	}
}

bool IsPlaylistExt(char *ext)
{
	if(!ext || ext[0] == 0)
		return false;

	int j=0;
	do
	{
		if (strcasecmp(ext, validPlaylistExtensions[j]) == 0)
			return true;
	} while (validPlaylistExtensions[++j][0] != 0);

	return false;
}

bool IsVideoExt(char *ext)
{
	if(!ext || ext[0] == 0)
		return false;

	int j=0;
	do
	{
		if (strcasecmp(ext, validVideoExtensions[j]) == 0)
			return true;
	} while (validVideoExtensions[++j][0] != 0);

	return false;
}

bool IsAudioExt(char *ext)
{
	if(!ext || ext[0] == 0)
		return false;

	int j=0;
	do
	{
		if (strcasecmp(ext, validAudioExtensions[j]) == 0)
			return true;
	} while (validAudioExtensions[++j][0] != 0);

	return false;
}

bool IsImageExt(char *ext)
{
	if(!ext || ext[0] == 0)
		return false;

	int j=0;
	do
	{
		if (strcasecmp(ext, validImageExtensions[j]) == 0)
			return true;
	} while (validImageExtensions[++j][0] != 0);

	return false;
}

// check that this file's extension is on the list of visible file types
bool IsAllowedExt(char *ext)
{
	if(!ext || ext[0] == 0)
		return false;

	if(menuCurrent == MENU_BROWSE_VIDEOS || menuCurrent == MENU_BROWSE_ONLINEMEDIA)
		if(IsVideoExt(ext)) return true;
	if(menuCurrent == MENU_BROWSE_MUSIC || menuCurrent == MENU_BROWSE_ONLINEMEDIA)
		if(IsAudioExt(ext)) return true;
	if(menuCurrent == MENU_BROWSE_PICTURES)
		if(IsImageExt(ext)) return true;

	return false;
}

bool IsAllowedProtocol(char *file)
{
	if(!file)
		return false;

	char *pos = strchr(file,':');
	if(pos == NULL || pos-file > 5 || pos[1] != '/' || pos[2] != '/')
		return false;

    char protocol[6];
	strncpy(protocol,file,pos-file);
	protocol[(pos-file)]='\0';

	int j=0;
	do
	{
		if (strcasecmp(protocol, validInternetProtocols[j]) == 0) 
			return true;	
	} while (validInternetProtocols[++j][0] != 0);

	return false;
}

void FindDirectory()
{
	int indexFound = -1;

	for(int i=0; i < browser.numEntries; i++)
	{
		if(strcmp(browserList[i].filename, browser.lastdir) == 0)
		{
			indexFound = i;
			break;
		}
	}

	// move to this file
	if(indexFound > 0)
	{
		int pagesize = 11;

		if(menuCurrent == MENU_BROWSE_VIDEOS && VideoImgVisible())
			pagesize = 10;

		if(menuCurrent == MENU_BROWSE_MUSIC || menuCurrent == MENU_BROWSE_ONLINEMEDIA)
			pagesize = 8;

		if(indexFound >= pagesize)
		{
			int newIndex = (floor(indexFound/(float)pagesize)) * pagesize;

			if(newIndex + pagesize > browser.numEntries)
				newIndex = browser.numEntries - pagesize;

			if(newIndex < 0)
				newIndex = 0;
			
			browser.pageIndex = newIndex;
		}
		browser.selIndex = indexFound;
		findLoadedFile = 2;
	}
	browser.lastdir[0] = 0; // only try to select the directory once
}

void FindFile()
{
	if(loadedFile[0] == 0 || browser.dir[0] == 0)
	{
		findLoadedFile = 0;
		return;
	}

	int indexFound = -1;
	char file[MAXPATHLEN];

	for(int i=0; i < browser.numEntries; i++)
	{
		GetFullPath(i, file);
		if(strcmp(file, loadedFile) == 0)
		{
			indexFound = i;
			break;
		}
	}

	// move to this file
	if(indexFound > 0)
	{
		browserList[indexFound].icon = ICON_PLAY;

		if(!selectLoadedFile) // only move to the file when first returning from the video
			return;

		int pagesize = 11;

		if(menuCurrent == MENU_BROWSE_VIDEOS && VideoImgVisible())
			pagesize = 10;

		if(menuCurrent == MENU_BROWSE_MUSIC || menuCurrent == MENU_BROWSE_ONLINEMEDIA)
			pagesize = 8;

		if(indexFound >= pagesize)
		{
			int newIndex = (floor(indexFound/(float)pagesize)) * pagesize;

			if(newIndex + pagesize > browser.numEntries)
				newIndex = browser.numEntries - pagesize;

			if(newIndex < 0)
				newIndex = 0;

			browser.pageIndex = newIndex;
		}
		browser.selIndex = indexFound;
		findLoadedFile = 2;
	}
	selectLoadedFile = false; // only try to select loaded file once
}

static bool ParseDirEntries()
{
	if(!dirIter)
		return false;

	char filename[MAXPATHLEN];
	char ext[6];
	struct stat filestat;

	int i = 0;
	int res;

	while(i < 20)
	{
		res = dirnext(dirIter,filename,&filestat);

		if(res != 0)
			break;

		if(strcmp(filename, "..") == 0)
		{
			if(IsDeviceRoot(browser.dir))
				continue;
		}
		else if(filename[0] == '.' || filename[0] == '$')
		{
			continue;
		}

		GetExt(filename, ext);

		// skip this file if it's not an allowed extension 
		if((filestat.st_mode & _IFDIR) == 0)
		{
			if(!IsAllowedExt(ext) && (!IsPlaylistExt(ext) || menuCurrent != MENU_BROWSE_ONLINEMEDIA))
				continue;
		}

		// add the entry
		if(AddBrowserEntry())
		{
			snprintf(browserList[browser.numEntries+i].filename, MAXJOLIET, "%s", filename);
			browserList[browser.numEntries+i].length = filestat.st_size;
			browserList[browser.numEntries+i].mtime = filestat.st_mtime;

			if(filestat.st_mode & _IFDIR)
			{
				browserList[browser.numEntries+i].type = TYPE_FOLDER;

				if(strcmp(filename, "..") == 0)
					sprintf(browserList[browser.numEntries+i].displayname, "%s (%s)", gettext("Up One Level"), GetParentDir());
				else
					snprintf(browserList[browser.numEntries+i].displayname, MAXJOLIET, "%s", browserList[browser.numEntries+i].filename);

				browserList[browser.numEntries+i].icon = ICON_FOLDER;
			}
			else
			{
				if(IsPlaylistExt(ext))
					browserList[browser.numEntries+i].type = TYPE_PLAYLIST;

				snprintf(browserList[browser.numEntries+i].displayname, MAXJOLIET, "%s", browserList[browser.numEntries+i].filename);
				browserList[browser.numEntries+i].icon = ICON_NONE;

				if(menuCurrent == MENU_BROWSE_VIDEOS)
				{
					// check if this file was watched
					GetFullPath(browser.numEntries+i, filename);
					if(wiiFindRestorePoint(filename))
						browserList[browser.numEntries+i].icon = ICON_CHECK;
				}
				else if(menuCurrent == MENU_BROWSE_MUSIC)
				{
					// check if this file is in the playlist
					if(MusicPlaylistFind(browser.numEntries+i))
						browserList[browser.numEntries+i].icon = ICON_FILE_CHECKED;
					else
						browserList[browser.numEntries+i].icon = ICON_FILE;
				}

				// hide the file's extension
				if(WiiSettings.hideExtensions)
					StripExt(browserList[browser.numEntries+i].displayname);
			}
			i++;
		}
		else
		{
			InfoPrompt("Warning", "This directory contains more entries than the maximum allowed (2000). Not all entries will be visible.");
			res = -1;
			break;
		}
	}

	// Sort the file list
	if(i > 0)
		qsort(browserList, browser.numEntries+i, sizeof(BROWSERENTRY), FileSortCallback);

	browser.numEntries += i;

	if(res != 0 || parseHalt)
	{
		if(parseHalt == 0)
		{
			if(browser.lastdir[0] != 0)
				FindDirectory(); // try to find and select the last directory
	
			if(findLoadedFile == 1)
				FindFile(); // try to find and select the last loaded file
		}

		dirclose(dirIter); // close directory
		dirIter = NULL;
		return false; // no more entries
	}
	return true; // more entries
}

/***************************************************************************
 * ParseDirectory
 * 
 * Browse subdirectories
 **************************************************************************/
int
ParseDirectory(bool waitParse)
{
	wchar_t msg[512];
	int retry = 1;

	SuspendParseThread();
	ResetBrowser(); // reset browser

	// open the directory
	while(dirIter == NULL && retry == 1)
	{
		dirIter = diropen(browser.dir);

		if(dirIter == NULL)
		{
			swprintf(msg, 512, L"%s %s", gettext("Error opening"), browser.dir);
			retry = ErrorPromptRetry(msg);

			if(retry)
			{
				int device = -1;
				int devnum = -1;
				FindDevice(browser.dir, &device, &devnum);

				if(device == DEVICE_SD || device == DEVICE_USB)
					Remount(device, NOTSILENT);
			}
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
		AddBrowserEntry();
		sprintf(browserList[0].filename, "..");
		sprintf(browserList[0].displayname, "Up One Level");
		browserList[0].length = 0;
		browserList[0].mtime = 0;
		browserList[0].type = TYPE_FOLDER; // flag this as a dir
		browserList[0].icon = ICON_FOLDER;
		browser.numEntries++;
	}

	parseHalt = 0;
	findLoadedFile = 1;
	ParseDirEntries(); // index first 20 entries
	ResumeParseThread(); // index remaining entries

	if(waitParse) // wait for complete parsing
	{
		ShowAction("Loading...");
		while(dirIter != NULL) usleep(THREAD_SLEEP);
		CancelAction();

		if(menuCurrent == MENU_BROWSE_PICTURES)
		{
			// check if any pictures were > max size and display a warning
			for(int i=0; i < browser.numEntries; i++)
				if(browserList[i].length > MAX_PICTURE_SIZE)
				{
					InfoPrompt("Warning", "One or more pictures within this folder exceeds the maximum size (6 MB) and will not be viewable.");
					break;
				}
		}
	}

	return browser.numEntries;
}

typedef struct
{
	int type; // 0 - unsupported, 1 - video, 2 - playlist, 3 - search
	char name[MAXJOLIET + 1];
	char url[MAXPATHLEN + 1];
	char thumb[MAXJOLIET + 1];
	char processor[MAXPATHLEN + 1];
} PLXENTRY;

static int ParsePLXPlaylist()
{
	char *buffer = (char*)mem2_malloc(64*1024, "other");

	if(!buffer)
		return 0;

	int size = 0;

	if(strncmp(browser.dir, "http:", 5) == 0)
		size = http_request(browser.dir, NULL, buffer, 64*1024, SILENT);
	else
		size = LoadFile(buffer, browser.dir, SILENT);

	if(size == 0)
	{
		mem2_free(buffer, "other");
		if(browser.numEntries > 0 && browserList[browser.selIndex].type == TYPE_SEARCH)
			return -4;
		return 0;
	}

	// attempt to parse buffer
	bool plxFile = false;
	int numEntries = 0;
	int c, lineptr = 0;
	char line[4096];

	PLXENTRY *list = (PLXENTRY *)malloc(sizeof(PLXENTRY));
	char attribute[1024], value[1024];
	PLXENTRY newEntry;
	memset(&newEntry, 0, sizeof(PLXENTRY));

	while(lineptr < size)
	{	
		// setup next line
		line[0] = 0;
		c = 0;
		while(c < 4096 && lineptr+c < size)
		{
			if(buffer[lineptr+c] == '\n')
			{
				snprintf(line, c+1, "%s", &buffer[lineptr]);
				if(line[c-1] == '\r') line[c-1] = 0;
				break;
			}
			c++;
		}

		// line too long or we've run out of new lines
		if(c == 4096 || lineptr+c > size) 
			break; // discard anything remaining

		lineptr += c+1;

		if(c <= 2)
			continue;

		if(sscanf(line, "%[^=]=%[^\n]", attribute, value) == 2)
		{
			if(strncmp(attribute, "version", 7) == 0)
			{
				plxFile = true;
			}
			else if(strncmp(attribute, "type", 4) == 0)
			{
				// we're on a new entry - add previous entry to list, if complete
				if(newEntry.type > 0 && strlen(newEntry.name) > 0 &&
					strlen(newEntry.url) > 0 && newEntry.processor[0] == 0 && 
					IsAllowedProtocol(newEntry.url))
				{
					memcpy(&list[numEntries], &newEntry, sizeof(PLXENTRY));
					numEntries++;
					PLXENTRY * newList = (PLXENTRY *)realloc(list, (numEntries+1) * sizeof(PLXENTRY));
					if(!newList) // failed to allocate required memory
					{
						free(list);
						mem2_free(buffer, "other");
						return -1; // too many files
					}
					else
					{
						list = newList;
					}
				}

				// blank values
				memset(&newEntry, 0, sizeof(PLXENTRY));

				if(strncmp(value, "video", 5) == 0 || strncmp(value, "audio", 5) == 0)
					newEntry.type = 1;
				else if(strncmp(value, "playlist", 8) == 0)
					newEntry.type = 2;
				else if(strncmp(value, "search", 6) == 0)
					newEntry.type = 3;
			}
			else if(strncmp(attribute, "name", 4) == 0)
			{
				snprintf(newEntry.name, MAXJOLIET, "%s", value);
			}
			else if(strncmp(attribute, "URL", 3) == 0)
			{
				snprintf(newEntry.url, MAXPATHLEN, "%s", value);
			}
			else if(strncmp(attribute, "thumb", 5) == 0)
			{
				snprintf(newEntry.thumb, MAXJOLIET, "%s", value);
			}
			else if(strncmp(attribute, "processor", 9) == 0)
			{
				snprintf(newEntry.processor, MAXPATHLEN, "%s", value);
			}
		}
	}

	// add the final entry
	if(newEntry.type > 0 && strlen(newEntry.name) > 0 && 
		strlen(newEntry.url) > 0 && newEntry.processor[0] == 0 && 
		IsAllowedProtocol(newEntry.url))
	{
		memcpy(&list[numEntries], &newEntry, sizeof(PLXENTRY));
		numEntries++;
	}

	if(numEntries == 0)
	{
		free(list);
		mem2_free(buffer, "other");

		if(plxFile && browserList[browser.selIndex].type == TYPE_SEARCH)
			return -5;
		else
			return -2;
	}

	char *root = (char*)BrowserHistoryRetrieve();
	char ext[6];
	GetExt(root, ext);
	
	ResetBrowser();

	AddBrowserEntry();
	strcpy(browserList[0].filename, BrowserHistoryRetrieve());
	sprintf(browserList[0].displayname, "Up One Level");
	browserList[0].length = 0;
	browserList[0].mtime = 0;
	browserList[0].icon = ICON_FOLDER;

	if(IsPlaylistExt(ext) || strncmp(root, "http:", 5) == 0)
		browserList[0].type = TYPE_PLAYLIST;
	else
		browserList[0].type = TYPE_FOLDER;

	browser.numEntries++;

	for(int i=0; i < numEntries; i++)
	{
		if(!AddBrowserEntry()) // add failed
		{
			free(list);
			mem2_free(buffer, "other");
			return -1;
		}

		strcpy(browserList[browser.numEntries].filename, list[i].url);
		strcpy(browserList[browser.numEntries].displayname, list[i].name);
		strcpy(browserList[browser.numEntries].image, list[i].thumb);
		
		if(list[i].type == 2)
			browserList[browser.numEntries].type = TYPE_PLAYLIST;
		else if(list[i].type == 3)
			browserList[browser.numEntries].type = TYPE_SEARCH;

		browser.numEntries++;
	}
	free(list);
	mem2_free(buffer, "other");

	// try to find and select the last loaded file
	FindFile();

	return browser.numEntries;
}

/****************************************************************************
 * ParsePlaylistFile
 * 
 * Loads a playlist selected from the filebrowser into the filebrowser
 ***************************************************************************/
int ParsePlaylistFile()
{
	int res;
	char ext[6];
	GetExt(browser.dir, ext);

	// if this file has no extension, try parsing it as a plx anyway
	if(ext[0] == 0 || strcmp(ext, "plx") == 0)
	{
		res = ParsePLXPlaylist();

		switch(res)
		{
			case 0:
			case -4:
				if(ext[0] != 0)
					ErrorPrompt("Error loading playlist!");
				break;
			case -1:
				ErrorPrompt("Out of memory: too many files!");
				break;
			case -2:
				if(ext[0] != 0)
					ErrorPrompt("Playlist does not contain any supported entries!");
				break;
			case -3: // blank search query
			case -5: // search query - no results
				break;
		}

		// we parsed the list, or we failed and it was a .plx
		if(res > 0 || res <= -3 || ext[0] != 0)
			return res;
	}

	// allow MPlayer to try parsing the file
	play_tree_t * list = parse_playlist_file(browser.dir);

	if(!list)
	{
		if(ext[0] != 0 && IsPlaylistExt(ext))
			ErrorPrompt("Error loading playlist!");
		return 0;
	}

	// MPlayer thinks it can parse anything. We should check if this list is sane
	char *playlistEntry;
	play_tree_iter_t *pt_iter = pt_iter_create(&list, NULL);

	if(!pt_iter)
	{
		play_tree_free(list, 1);
		ErrorPrompt("Error loading playlist!");
		return 0;
	}

	while ((playlistEntry = pt_iter_get_next_file(pt_iter)) != NULL)
	{
		if(IsAllowedProtocol(playlistEntry))
			break;
	}

	if(playlistEntry == NULL) // we reached the end of the list
	{
		pt_iter_destroy(&pt_iter);
		play_tree_free(list, 1);
		ErrorPrompt("Error loading playlist!");
		return 0;
	}

	char *root = (char*)BrowserHistoryRetrieve();
	GetExt(root, ext);

	ResetBrowser();

	AddBrowserEntry();
	strcpy(browserList[0].filename, root);
	sprintf(browserList[0].displayname, "Up One Level");
	browserList[0].length = 0;
	browserList[0].mtime = 0;
	browserList[0].icon = ICON_FOLDER;

	if(IsPlaylistExt(ext) || strncmp(root, "http:", 5) == 0)
		browserList[0].type = TYPE_PLAYLIST;
	else
		browserList[0].type = TYPE_FOLDER;

	browser.numEntries++;

	char *start;

	pt_iter_goto_head(pt_iter);

	play_tree_t* i;
	for(i = pt_iter->tree; i != NULL; i = i->next)
	{
		if(!i->files || !IsAllowedProtocol(i->files[0]))
			continue;

		if(!AddBrowserEntry()) // add failed
		{
			InfoPrompt("Warning", "This playlist contains more entries than the maximum allowed (2000). Not all entries will be visible.");
			break;
		}

		snprintf(browserList[browser.numEntries].filename, MAXPATHLEN, "%s", i->files[0]);

		// use parameter pt_prettyformat_title for displayname if it exists
		if(i->params) 
		{
			for (int n = 0; i->params[n].name != NULL; n++) 
			{
				if(strcasecmp(i->params[n].name,PLAY_TREE_PARAM_PRETTYFORMAT_TITLE) != 0)
					continue;
				if(i->params[n].value == NULL)
					break;
				snprintf(browserList[browser.numEntries].displayname, MAXJOLIET, "%s", i->params[n].value);
				break;
			}
		}

		if(browserList[browser.numEntries].displayname[0] == 0)
		{
			start = strrchr(i->files[0],'/');

			// use part after last / for display name, if it's not already the end of the string
			if(start != NULL && start[1] != 0)
			{
				start++;
				sprintf(browserList[browser.numEntries].displayname, start);
			}
			else
			{
				snprintf(browserList[browser.numEntries].displayname, MAXJOLIET, "%s", i->files[0]);
			}
		}
		if(IsPlaylistExt(ext))
			browserList[browser.numEntries].type = TYPE_PLAYLIST;

		browser.numEntries++;
	}
	pt_iter_destroy(&pt_iter);
	play_tree_free(list, 1);

	if(browser.numEntries == 1)
	{
		ResetBrowser();
		ErrorPrompt("Playlist does not contain any supported entries!");
		return 0;
	}

	// try to find and select the last loaded file
	FindFile();

	return browser.numEntries;
}

/****************************************************************************
 * ParseOnlineMedia
 * 
 * Loads the current online media list into the filebrowser
 ***************************************************************************/
int ParseOnlineMedia()
{
	if(onlinemediaSize == 0)
		return 0;

	if(browser.dir[0] != 0)
	{
		AddBrowserEntry();
		sprintf(browserList[0].filename, "..");
		sprintf(browserList[0].displayname, "Up One Level");
		browserList[0].length = 0;
		browserList[0].mtime = 0;
		browserList[0].type = TYPE_FOLDER;
		browserList[0].icon = ICON_FOLDER;
		browser.numEntries++;
	}

	char tmpaddress[MAXPATHLEN];
	int dirLen = strlen(browser.dir);

	for(int i=0; i < onlinemediaSize; i++)
	{
		int filepathLen = strlen(onlinemediaList[i].filepath);

		// add file
		if(strcmp(browser.dir, onlinemediaList[i].filepath) == 0)
		{
			// unknown protocol - reject entry
			if(!IsAllowedProtocol(onlinemediaList[i].address) && strstr(onlinemediaList[i].address, "://") != NULL)
				continue;

			AddBrowserEntry();
			if(IsAllowedProtocol(onlinemediaList[i].address))
				snprintf(tmpaddress, MAXPATHLEN, "%s", onlinemediaList[i].address);
			else // protocol not specified - assume http:// and append
				snprintf(tmpaddress, MAXPATHLEN, "http://%s", onlinemediaList[i].address);

			url_unescape_string(browserList[browser.numEntries].filename, tmpaddress);
			snprintf(browserList[browser.numEntries].displayname, MAXJOLIET, "%s", onlinemediaList[i].displayname);
			snprintf(browserList[browser.numEntries].image, MAXJOLIET, "%s", onlinemediaList[i].image);
			browserList[browser.numEntries].length = 0;
			browserList[browser.numEntries].mtime = 0;
			browserList[browser.numEntries].type = onlinemediaList[i].type;
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
				browserList[browser.numEntries].filename[MAXPATHLEN] = 0;
				browserList[browser.numEntries].displayname[MAXJOLIET] = 0;
				browserList[browser.numEntries].length = 0;
				browserList[browser.numEntries].mtime = 0;
				browserList[browser.numEntries].type = TYPE_FOLDER;
				browserList[browser.numEntries].icon = ICON_FOLDER;
				browser.numEntries++;
			}
		}
	}

	// Sort the file list
	qsort(browserList, browser.numEntries, sizeof(BROWSERENTRY), FileSortCallback);

	if(browser.lastdir[0] != 0)
		FindDirectory(); // try to find and select the last directory
	else if(findLoadedFile == 1)
		FindFile(); // try to find and select the last loaded file

	return browser.numEntries;
}

static bool cancelFileLoad = false;

void CancelFileOp()
{
	cancelFileLoad = true;
}

/****************************************************************************
 * LoadFile
 * 
 * Loads the file specified into the provided buffer
 ***************************************************************************/
size_t loadOffset = 0, loadSize = 0;

size_t LoadFile (char * buffer, char *filepath, bool silent)
{
	size_t readsize = 0;
	int retry = 1;
	FILE * file;
	cancelFileLoad = false;


	// stop checking if devices were removed/inserted
	// since we're loading a file
	SuspendDeviceThread();

	// halt parsing
	SuspendParseThread();

	// open the file
	while(retry)
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
		loadSize = ftello(file);
		fseeko(file,0,SEEK_SET);

		while(!feof(file))
		{
			if(!silent)
				ShowProgress ("Loading...", loadOffset, loadSize);
			readsize = fread (buffer + loadOffset, 1, 4096, file); // read in next chunk

			if(readsize <= 0)
				break; // reading finished (or failed)

			loadOffset += readsize;

			if(cancelFileLoad)
			{
				cancelFileLoad = false;
				loadOffset = 0;
				break;
			}
		}
		fclose (file);
		loadSize = loadOffset;
		retry = 0;

		if(!silent)
			CancelAction();
	}

	// go back to checking if devices were inserted/removed
	ResumeDeviceThread();
	if(!silent)
		CancelAction();

	size_t res = loadSize;
	loadSize = 0;
	loadOffset = 0;
	return res;
}

/****************************************************************************
 * SaveFile
 * 
 * Writes the provided buffer to the specified filepath
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
	SuspendDeviceThread();

	// halt parsing
	SuspendParseThread();

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
			retry = ErrorPromptRetry("Error saving file!");
	}

	// go back to checking if devices were inserted/removed
	ResumeDeviceThread();

	CancelAction();
    return written;
}
