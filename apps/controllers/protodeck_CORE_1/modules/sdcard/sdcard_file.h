// $Id: sdcard_file.h 378 2008-07-06 20:38:32Z tk $
/*
 * Header file for SDCARD File Access Routines
 *
 * ==========================================================================
 *
 *  Copyright 2008 Thorsten Klose (tk@midibox.org)
 *  Licensed for personal non-commercial use only.
 *  All other rights reserved.
 *
 * ==========================================================================
 */

#ifndef _SDCARD_FILE_H
#define _SDCARD_FILE_H

/////////////////////////////////////////////////////////////////////////////
// Global definitions
/////////////////////////////////////////////////////////////////////////////

// Note: keep codes in-sync with definitions in sdcard_file.inc!
#define SDCARD_ERROR_NONE                 0x00

#define SDCARD_ERROR_SDC_ACCESS           0x01

#define SDCARD_ERROR_MBR_MISSING_MARKER   0x10
#define SDCARD_ERROR_MBR_WRONG_PARTI_TYPE 0x11

#define SDCARD_ERROR_BR_MISSING_MARKER    0x20
#define SDCARD_ERROR_BR_UNSUPP_SEC_SIZE   0x21
#define SDCARD_ERROR_BR_UNSUPP_CLUS_SIZE  0x22

#define SDCARD_ERROR_FILE_NOT_FOUND       0x30

#define SDCARD_ERROR_SEEK_INVALID_CLUS    0x40
#define SDCARD_ERROR_SEEK_INVALID_FAT     0x41
#define SDCARD_ERROR_SEEK_END_REACHED     0x42


#define SDCARD_FILE_MODE_CASE_SENSITIVE   0	// mode flag 0 controls case sensitivity
#define SDCARD_FILE_MODE_BACKWARD_SEARCH  1	// mode flag 1 controls forward/backward search
	

/////////////////////////////////////////////////////////////////////////////
// Global Types
/////////////////////////////////////////////////////////////////////////////

// reference: http://home.teleport.com/~brainy/lfn.htm
struct sdcard_file_dir_entry {
  unsigned char name[8];
  unsigned char extension[3];
  unsigned char attribute;
  unsigned char reserved[10]; // only used by long filenames
  unsigned int time;
  unsigned int date;
  unsigned int cluster;
  unsigned long size;
};

/////////////////////////////////////////////////////////////////////////////
// Prototypes
/////////////////////////////////////////////////////////////////////////////
extern unsigned char SDCARD_FILE_Init(void) __wparam;
extern unsigned char SDCARD_FILE_Open(unsigned char mode, unsigned char *filename, unsigned int from_dir_entry) __wparam;
extern unsigned char SDCARD_FILE_Seek(unsigned long offset) __wparam;


/////////////////////////////////////////////////////////////////////////////
// Export global variables
/////////////////////////////////////////////////////////////////////////////
extern unsigned int sdcard_file_mode;
extern unsigned int sdcard_file_cluster;
extern unsigned long sdcard_file_sector;
extern unsigned int sdcard_file_dir_entry_num;
extern struct sdcard_file_dir_entry *sdcard_file_dir_entry_ptr;

extern unsigned int sdcard_file_sector0;
extern unsigned int sdcard_file_sector_fat;
extern unsigned int sdcard_file_sector_root;
extern unsigned char sdcard_file_root_sectors;
extern unsigned int sdcard_file_sector_cl2;
extern unsigned char sdcard_file_cluster_size;

extern unsigned long sdcard_file_seek_offset;
extern unsigned long sdcard_file_seek_cluster;

#endif /* _SDCARD_FILE_H */
