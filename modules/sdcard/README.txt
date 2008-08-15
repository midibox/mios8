$Id$

Driver for SD/MMC Cards
=======================


Oversight
~~~~~~~~~

This driver package contains two modules: 
   sdcard: low level driver to access a SD or MMC card
   sdcard_file: support for FAT16 filesystem

To simplify the documentation, only SD cards are mentioned below. 
The interface of MMC cards is identical, accordingly the driver
can handle them as well.


sdcard can run stand-alone if the application should read/write
sectors directly without taking the filesystem into account
(cheap BankStick replacement)

The usage of sdcard_file is recommented, if files should be shared
between a MIDIbox and a computer. This driver allows to search for
files in the root directory, and it provides a "seek" function to
select a specific sector of a file by taking the FAT entries into
account.

Theoretically it's also possible to read subdirectories by changing
the "sdcard_file_sector_root" pointer, and adapting the 
"sdcard_file_root_sectors" variable, but this hasn't been evaluated yet.

The driver doesn't allow to create new directory entries, and it isn't
possible to change the size of an existing file (although it is possible
to write into existing files), this would require some additional
FAT manipulation code, which would increase the complexity (and code size)
of the program.


C usage examples
~~~~~~~~~~~~~~~~

  -> $MIOS_PATH/apps/examples/sdcard/


Functions
~~~~~~~~~

All functions return an error code, which is documented in the next chapter.

SDCARD_Init(void)
  Initializes the SD card

SDCARD_SectorRead(unsigned long sector)
  Reads a 512 byte sector from SD card. The content will be written
  into the sdcard_buffer_p0 and sdcard_buffer_p1 arrays (each page
  contains 256 bytes)

SDCARD_SectorWrite(unsigned long sector)
  Writes 512 byte into the given sector. 
  Data is taken from sdcard_buffer_p0 and sdcard_buffer_p1 arrays


SDCARD_FILE_Init(void)
  Initializes the filesystem: reads the master boot record (MBR) to
  determine the location of the first parititon. Thereafter reads the
  boot record (BR) of this partition to determine the location of
  the file allocation table (FAT) and root directory

SDCARD_FILE_Open(unsigned char mode, unsigned char *filename, unsigned int from_dir_entry)
  Searches for a file in the root directory and opens it
  mode: if bit 0 is set, filename will be searched with case sensitivity
        if bit 1 is set, filename will be searched backwards
  filename: 11 characters (8 for the actual filename, 3 for the extension)
        Wildcards are possible by replacing characters by 0xff, e.g. if
        all .syx files should be searched, fill the first 8 characters with
        0xff, and set the last 3 characters to "syx".
	Note that the filename can either be located in RAM or code memory
  from_dir_entry:
        Normaly set to 0 to search a file from the first directory entry
        Can be set to a different value if a wildcarded filemask is used
        to search for the next (or previous) file. Bit 1 of <mode> specifies,
        if the function should search forward or backward

Most important output variables of this functions are:
  sdcard_file_sector: starting sector of the file (can be used as input
    for SDCARD_SectorRead/Write if only the first sector should be
    accessed)
  sdcard_file_cluster: important in conjunction with SDCARD_FILE_Seek
  sdcard_file_dir_entry_ptr: access to the directory entry. The structure
  is part of sdcard_file.h
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



SDCARD_FILE_Seek(unsigned long offset)
  Returns the sector of a file depending on the given offset.
  Should be used whenever a certain sector of the file should be accessed.
  The sector is determined based on the cluster number and FAT entries.

  After the execution, the corresponding sector which belongs to the
  offset is located in sdcard_file_sector

  WARNING: this function overwrites the 512 byte buffer sdcard_buffer_p0/1 
  once the FAT has to be read out to locate the sector.
  When using the seek function in conjunction with SDCARD_SectorWrite(), it
  is required to fill the buffer for the write operation *after* 
  SDCARD_FILE_Seek() has located the sector!


Error Codes
~~~~~~~~~~~

A list of supported error codes. The corresponding names are defined 
in sdcard_file.h

0x00  No Error

0x01  Error during SD Card Access (SDCARD_SectorRead or SDCARD_SectorWrite)

0x10  Missing marker in Master Boot Record
0x11  Wrong partition type (expecting FAT16)

0x20  Missing marker in Partition Boot Record
0x21  Unsupported Sector Size
0x22  Unsupported Cluster Size

0x30  File not found

0x40  Seek found invalid cluster in FAT
0x41  Seek found invalid FAT entry
0x42  Seek reached the end of file (offset too big)


Files
~~~~~

  sdcard*.inc: can be included into "asm-only" programs (non-relocatable code)
  sdcard*.asm: relocatable wrapper for C based applications
  sdcard*.h:   C headers
  sdcard*.mk:  include this into Makefile to integrate the module


Defines
~~~~~~~

  C programs (relocatable code): can be overruled with -D<define>=<value> from command line 
  Assembly programs: add #define statements within program before "#include <sdcard.inc>"

SDCARD_LAT_CS=LATC	; The chip select pin
SDCARD_TRIS_CS=TRISC	; is connected to Port C.0
SDCARD_PIN_CS=0		; (CANNOT be shared with other outputs!)

SDCARD_LAT_SCLK=LATC	; The shift clock input pin SCLK
SDCARD_TRIS_SCLK=TRISC	; is connected to Port C.1
SDCARD_PIN_SCLK=1	; (can be shared with other outputs)

SDCARD_LAT_DI=LATC	; The DI pin (data input of SD card)
SDCARD_TRIS_DI=TRISC	; is connected to Port C.2
SDCARD_PIN_DI=2		; (can be shared with other inputs)

SDCARD_PORT_DO=PORTC	; The DO pin (data output of SD card)
SDCARD_TRIS_DO=TRISC	; is connected to Port C.3
SDCARD_PIN_DO=3		; (can be shared with other outputs)



Integration Hints
~~~~~~~~~~~~~~~~~

C based Applications
~~~~~~~~~~~~~~~~~~~~

  1) Makefile: add "sdcard.o" and "sdcard_file.o" to OBJS variable

  2) Makefile: include sdcard.mk, and enhance SDCARD_DEFINES if required:
---
# include SD Card Driver
# use following pin assignments:
SDCARD_DEFINES += -DSDCARD_LAT_CS=LATC -DSDCARD_TRIS_CS=TRISC -DSDCARD_PIN_CS=0
SDCARD_DEFINES += -DSDCARD_LAT_SCLK=LATC -DSDCARD_TRIS_SCLK=TRISC -DSDCARD_PIN_SCLK=1
SDCARD_DEFINES += -DSDCARD_LAT_DI=LATC -DSDCARD_TRIS_DI=TRISC -DSDCARD_PIN_DI=2
SDCARD_DEFINES += -DSDCARD_PORT_DO=PORTC -DSDCARD_TRIS_DO=TRISC -DSDCARD_PIN_DO=3
include $(MIOS_PATH)/modules/sdcard/sdcard.mk
---

  3) main.c: #include <sdcard.h>
             #include <sdcard_file.h>

  4) main.c: define global variable:
             unsigned char error_code;

  5) main.c: add following code to Init() hook
---
  // initialize SD card driver
  SDCARD_Init();

  // read partition
  error_code = SDCARD_FILE_Init();
---

  6) for searching and accessing file, follow the code examples under
     $MIOS_PATH/apps/examples/sdcard/


"Assembler-Only" Applications
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

integration is possible, but not documented yet

