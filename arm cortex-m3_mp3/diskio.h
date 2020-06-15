#ifndef _DISKIO
#define _READONLY			0	/* 1: Read-only mode */
#define _USE_IOCTL			1

typedef unsigned char DSTATUS;

/* Results of Disk Functions */
typedef enum {
      RES_OK = 0,				/* 0: Successful */
      RES_ERROR,				/* 1: R/W Error */
      RES_WRPRT,				/* 2: Write Protected */
      RES_NOTRDY,				/* 3: Not Ready */
      RES_PARERR				/* 4: Invalid Parameter */
} DRESULT;


DSTATUS disk_initialize (unsigned char);
DSTATUS disk_status (unsigned char);
DRESULT disk_read (unsigned char, unsigned char*, unsigned int, unsigned char);
#if _READONLY == 0
DRESULT disk_write (unsigned char, const unsigned char*, unsigned int, unsigned char);
#endif
DRESULT disk_ioctl (unsigned char, unsigned char, void*);
void disk_timerproc (void);

struct tm get_calendar_fat_time (unsigned int date,unsigned int time);

/* Disk Status Bits (DSTATUS) */
#define STA_NOINIT			0x01	/* Drive not initialized */
#define STA_NODISK			0x02	/* No medium in the drive */
#define STA_PROTECT			0x04	/* Write protected */

/* Command code for disk_ioctrl() */
/* Generic command */
#define CTRL_SYNC			0	/* Mandatory for read/write configuration */
#define GET_SECTOR_COUNT		1	/* Mandatory for only f_mkfs() */
#define GET_SECTOR_SIZE			2
#define GET_BLOCK_SIZE			3	/* Mandatory for only f_mkfs() */
#define CTRL_POWER			4
#define CTRL_LOCK			5
#define CTRL_EJECT			6
/* MMC/SDC command */
#define MMC_GET_TYPE			10
#define MMC_GET_CSD			11
#define MMC_GET_CID			12
#define MMC_GET_OCR			13
#define MMC_GET_SDSTAT			14
/* ATA/CF command */
#define ATA_GET_REV			20
#define ATA_GET_MODEL			21
#define ATA_GET_SN			22


#define _DISKIO
#endif
