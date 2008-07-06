// $Id$
/*
 * This demo application allows to send .syx files which are stored in the
 * root directory of a SD card
 *
 * ==========================================================================
 *
 *  Copyright (C) 2008  Thorsten Klose (tk@midibox.org)
 *  Licensed for personal non-commercial use only.
 *  All other rights reserved.
 * 
 * ==========================================================================
 */


/////////////////////////////////////////////////////////////////////////////
// Include files
/////////////////////////////////////////////////////////////////////////////

#include <cmios.h>
#include <pic18fregs.h>

#include <sdcard.h>
#include <sdcard_file.h>

/////////////////////////////////////////////////////////////////////////////
// DIN assignments
/////////////////////////////////////////////////////////////////////////////
#define DIN_NUMBER_EXEC     7
#define DIN_NUMBER_INC      6
#define DIN_NUMBER_DEC      5
#define DIN_NUMBER_SNAPSHOT 4


/////////////////////////////////////////////////////////////////////////////
// Delay after F7 in mS
/////////////////////////////////////////////////////////////////////////////
#define DELAY_AFTER_F7   750 // ms

/////////////////////////////////////////////////////////////////////////////
// Prototypes
/////////////////////////////////////////////////////////////////////////////
unsigned char ReadDir(unsigned char next_dir_entry, unsigned char search_mode) __wparam;
unsigned char SysEx_Sender(unsigned char *syx_filename) __wparam;
void SysEx_Sender_Delay(unsigned int delay) __wparam;


/////////////////////////////////////////////////////////////////////////////
// Global Variables
/////////////////////////////////////////////////////////////////////////////
unsigned char error_code;
unsigned int dir_entry;
unsigned char filter_active;
unsigned char reset_dir_entry_req;
unsigned char file_selected;
unsigned char filename[8+3];


/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS after startup to initialize the 
// application
/////////////////////////////////////////////////////////////////////////////
void Init(void) __wparam
{
  // initialize SRIO driver
  MIOS_SRIO_UpdateFrqSet(1); // ms
  MIOS_SRIO_NumberSet(1);

  // initialize SD card driver
  SDCARD_Init();

  // read partition
  error_code = SDCARD_FILE_Init();
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS in the mainloop when nothing else is to do
/////////////////////////////////////////////////////////////////////////////
void Tick(void) __wparam
{
}

/////////////////////////////////////////////////////////////////////////////
// This function is periodically called by MIOS. The frequency has to be
// initialized with MIOS_Timer_Set
/////////////////////////////////////////////////////////////////////////////
void Timer(void) __wparam
{
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS when the display content should be 
// initialized. Thats the case during startup and after a temporary message
// has been printed on the screen
/////////////////////////////////////////////////////////////////////////////
void DISPLAY_Init(void) __wparam
{
  MIOS_LCD_Clear();
  MIOS_LCD_CursorSet(0x00);
  MIOS_LCD_PrintCString("Initialisation  ");
  MIOS_LCD_CursorSet(0x40);
  MIOS_LCD_PrintCString("Error Code: ");
  MIOS_LCD_PrintHex2(error_code);
}

/////////////////////////////////////////////////////////////////////////////
//  This function is called in the mainloop when no temporary message is shown
//  on screen. Print the realtime messages here
/////////////////////////////////////////////////////////////////////////////
void DISPLAY_Tick(void) __wparam
{
}

/////////////////////////////////////////////////////////////////////////////
//  This function is called by MIOS when a complete MIDI event has been received
/////////////////////////////////////////////////////////////////////////////
void MPROC_NotifyReceivedEvnt(unsigned char evnt0, unsigned char evnt1, unsigned char evnt2) __wparam
{
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS when a MIDI event has been received
// which has been specified in the MIOS_MPROC_EVENT_TABLE
/////////////////////////////////////////////////////////////////////////////
void MPROC_NotifyFoundEvent(unsigned entry, unsigned char evnt0, unsigned char evnt1, unsigned char evnt2) __wparam
{
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS when a MIDI event has not been completly
// received within 2 seconds
/////////////////////////////////////////////////////////////////////////////
void MPROC_NotifyTimeout(void) __wparam
{
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS when a MIDI byte has been received
/////////////////////////////////////////////////////////////////////////////
void MPROC_NotifyReceivedByte(unsigned char byte) __wparam
{
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS before the shift register are loaded
/////////////////////////////////////////////////////////////////////////////
void SR_Service_Prepare(void) __wparam
{
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS after the shift register have been loaded
/////////////////////////////////////////////////////////////////////////////
void SR_Service_Finish(void) __wparam
{
}


/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS when an button has been toggled
// pin_value is 1 when button released, and 0 when button pressed
/////////////////////////////////////////////////////////////////////////////
void DIN_NotifyToggle(unsigned char pin, unsigned char pin_value) __wparam
{
  // ignore if button has been depressed
  if( pin_value )
    return;

  // call function depending on button number
  switch( pin ) {
    case DIN_NUMBER_EXEC: // Exec button
      // just try to read partition and print error code
      SDCARD_Init();
      if( error_code = SDCARD_FILE_Init() ) {
	// maybe card has been disconnected - try to init card and read partition again
	SDCARD_Init();
	error_code = SDCARD_FILE_Init();
      }
      MIOS_LCD_Clear();
      MIOS_LCD_CursorSet(0x00);
      MIOS_LCD_PrintCString("Partition read  ");
      MIOS_LCD_CursorSet(0x40);
      MIOS_LCD_PrintCString("Error Code: ");
      MIOS_LCD_PrintHex2(error_code);

      // start at first dir_entry with next inc/dec
      reset_dir_entry_req = 1;
      // no file selected anymore
      file_selected = 0;
      break;
      
    case DIN_NUMBER_INC: // Inc button
      filter_active = 1;
       // with forward search
      if( !(error_code = ReadDir(dir_entry+1, (0 << SDCARD_FILE_MODE_BACKWARD_SEARCH))) ) {
	file_selected = 1;
      }
      break;

    case DIN_NUMBER_DEC: // Dec button
      filter_active = 1;
      if( dir_entry ) {
	// with backward search
	if( !(error_code = ReadDir(dir_entry-1, (1 << SDCARD_FILE_MODE_BACKWARD_SEARCH))) ) {
	  file_selected = 1;
	}
      }
      break;

    case DIN_NUMBER_SNAPSHOT: // Snapshot button
      if( !file_selected ) {
	MIOS_LCD_Clear();
	MIOS_LCD_CursorSet(0x00);
	MIOS_LCD_PrintCString("No file selected");
      } else {
	// send file
	SysEx_Sender(filename);
      }

#if 0
      // toggle filter on/off
      filter_active = filter_active ? 0 : 1;
      MIOS_LCD_Clear();
      MIOS_LCD_CursorSet(0x00);
      MIOS_LCD_PrintCString(".syx filter ");
      MIOS_LCD_PrintCString(filter_active ? "on" : "off");
#else
#endif
      break;
  }
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS when an encoder has been moved
// incrementer is positive when encoder has been turned clockwise, else
// it is negative
/////////////////////////////////////////////////////////////////////////////
void ENC_NotifyChange(unsigned char encoder, char incrementer) __wparam
{
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS when a pot has been moved
/////////////////////////////////////////////////////////////////////////////
void AIN_NotifyChange(unsigned char pin, unsigned int pin_value) __wparam
{
}



/////////////////////////////////////////////////////////////////////////////
// This function reads a directory entry and displays the result
// Input: new dir entry from which the search should start
//        search_mode parameter allows to select between forward/backward search
//        (passed to SDCARD_File_Open)
// Global Variables:
//    - dir_entry: number of directory entry from which the search will start
//    - reset_dir_entry_req: if 1, dir_entry will be set to 0
//    - error_code: global variable which stores the error code of last operation
//    - filter_active: if set, only .syx files will be searched
/////////////////////////////////////////////////////////////////////////////
unsigned char ReadDir(unsigned char next_dir_entry, unsigned char search_mode) __wparam
{
  unsigned char i;
  unsigned char prev_dir_entry = dir_entry;
  dir_entry = next_dir_entry;

  // re-start at first directory entry if requested
  if( reset_dir_entry_req ) {
    dir_entry = 0;
    reset_dir_entry_req = 0;
  }

  // open partition
  if( error_code = SDCARD_FILE_Init() ) {
    // maybe card has been disconnected - try to init card and read partition again
    SDCARD_Init();
    error_code = SDCARD_FILE_Init();
  }

  // print error code if this failed
  if( error_code ) {
    MIOS_LCD_CursorSet(0x00);
    MIOS_LCD_PrintCString("Initialisation  ");
    MIOS_LCD_CursorSet(0x40);
    MIOS_LCD_PrintCString("Error Code: ");
    MIOS_LCD_PrintHex2(error_code);
    MIOS_LCD_PrintCString("  ");
    // start with first entry on next try
    reset_dir_entry_req = 0;
  } else {
    // using wildcards for all characters
    for(i=0; i<8+11; ++i) {
      filename[i] = 0xff; // wildcard
    }

    // filter .syx files?
    if( filter_active ) {
      filename[8]  = 's';
      filename[9]  = 'y';
      filename[10] = 'x';
    }

    // search dir entry, if file not found, try previous dir entry
    if( error_code = SDCARD_FILE_Open(search_mode, filename, dir_entry) ) {
      if( error_code == SDCARD_ERROR_FILE_NOT_FOUND ) {
	error_code = SDCARD_FILE_Open(search_mode, filename, prev_dir_entry);
      }
    }

    // print error code if file hasn't been found
    if( error_code ) {
      MIOS_LCD_CursorSet(0x00);
      MIOS_LCD_PrintCString("File not found  ");
      MIOS_LCD_CursorSet(0x40);
      MIOS_LCD_PrintCString("Error Code: ");
      MIOS_LCD_PrintHex2(error_code);
      MIOS_LCD_PrintChar(' ');
      MIOS_LCD_PrintChar(' ');
      // start with first entry on next try
      reset_dir_entry_req = 0;
    } else {
      // set directory entry
      dir_entry = sdcard_file_dir_entry_num;

      // store filename in global variable
      for(i=0; i<8; ++i)
	filename[i] = sdcard_file_dir_entry_ptr->name[i];
      for(i=0; i<3; ++i)
	filename[i+8] = sdcard_file_dir_entry_ptr->extension[i];

      // 1st Line: print entry number and filename
      MIOS_LCD_CursorSet(0x00);
      MIOS_LCD_PrintBCD3((unsigned char)sdcard_file_dir_entry_num);
      MIOS_LCD_PrintChar(':');	
      for(i=0; i<8; ++i)
	MIOS_LCD_PrintChar(filename[i]);
      MIOS_LCD_PrintChar('.');
      for(i=0; i<3; ++i)
	MIOS_LCD_PrintChar(filename[8+i]);

      // 2nd Line: print attribute, cluster and filesize
      MIOS_LCD_CursorSet(0x40);
      // Attribute
      MIOS_LCD_PrintHex2(sdcard_file_dir_entry_ptr->attribute);
      MIOS_LCD_PrintChar(' ');
      MIOS_LCD_PrintHex2((unsigned char)(sdcard_file_dir_entry_ptr->cluster >> 8));
      MIOS_LCD_PrintHex2((unsigned char)sdcard_file_dir_entry_ptr->cluster);
      MIOS_LCD_PrintChar(' ');
      MIOS_LCD_PrintHex2((unsigned char)(sdcard_file_dir_entry_ptr->size >> 24));
      MIOS_LCD_PrintHex2((unsigned char)(sdcard_file_dir_entry_ptr->size >> 16));
      MIOS_LCD_PrintHex2((unsigned char)(sdcard_file_dir_entry_ptr->size >> 8));
      MIOS_LCD_PrintHex2((unsigned char)(sdcard_file_dir_entry_ptr->size >> 0));
    }
  }

  return error_code;
}


/////////////////////////////////////////////////////////////////////////////
// This function sends the content of a given .syx file
// Input: filename (can either be located in RAM or code memory)
/////////////////////////////////////////////////////////////////////////////
unsigned char SysEx_Sender(unsigned char *syx_filename) __wparam
{
  unsigned char i;
  unsigned char c;
  unsigned long filesize;
  unsigned long offset;
  unsigned int send_offset;

  // 1st Line: print filename
  MIOS_LCD_CursorSet(0x00);
  for(i=0; i<8; ++i)
    MIOS_LCD_PrintChar(syx_filename[i]);
  MIOS_LCD_PrintChar('.');
  for(i=0; i<3; ++i)
    MIOS_LCD_PrintChar(syx_filename[8+i]);
  for(i=0; i<4; ++i)
    MIOS_LCD_PrintChar(' ');	

  // open partition
  if( error_code = SDCARD_FILE_Init() ) {
    // maybe card has been disconnected - try to init card and read partition again
    SDCARD_Init();
    error_code = SDCARD_FILE_Init();
  }

  // if no error: open file
  if( !error_code )
    error_code = SDCARD_FILE_Open(0, syx_filename, 0); // search mode, filename, start entry

  // stop function by printing error code if this failed
  if( error_code ) {
    MIOS_LCD_CursorSet(0x40);
    MIOS_LCD_PrintCString("Error Code: ");
    MIOS_LCD_PrintHex2(error_code);
    MIOS_LCD_PrintCString("  ");
    return error_code;
  }

  // check filesize
  if( !(filesize = sdcard_file_dir_entry_ptr->size) ) {
    MIOS_LCD_CursorSet(0x40);
    MIOS_LCD_PrintCString("File is empty.  ");
    return 0xff;
  }

  // send .syx file
  offset = 0;
  while( offset < filesize ) {

    // 2nd Line: print offset
    MIOS_LCD_CursorSet(0x40);
    MIOS_LCD_PrintCString("Offs. 0x");
    MIOS_LCD_PrintHex2((unsigned char)(offset >> 24));
    MIOS_LCD_PrintHex2((unsigned char)(offset >> 16));
    MIOS_LCD_PrintHex2((unsigned char)(offset >> 8));
    MIOS_LCD_PrintHex2((unsigned char)(offset >> 0));

    // search for sector at given offset
    if( error_code = SDCARD_FILE_Seek(offset) ) {
      MIOS_LCD_Clear();
      MIOS_LCD_CursorSet(0x00);
      MIOS_LCD_PrintCString("File Seek failed");
      MIOS_LCD_CursorSet(0x40);
      MIOS_LCD_PrintCString("Error Code: ");
      MIOS_LCD_PrintHex2(error_code);
      return error_code;
    }

    // read sector
    if( error_code = SDCARD_SectorRead(sdcard_file_sector) ) {
      MIOS_LCD_Clear();
      MIOS_LCD_CursorSet(0x00);
      MIOS_LCD_PrintCString("SD access error ");
      MIOS_LCD_CursorSet(0x40);
      MIOS_LCD_PrintCString("Error Code: ");
      MIOS_LCD_PrintHex2(error_code);
      return error_code;
    }

    // feed watchdog
__asm
    clrwdt
__endasm;

    // 512 bytes now in sdcard_buffer_p[01]

    send_offset = 0;
    while( send_offset < 256 && offset < filesize ) {
      MIOS_MIDI_TxBufferPut(c=sdcard_buffer_p0[(unsigned char)send_offset]);
      ++send_offset;
      ++offset;
      if( c == 0xf7 )
	SysEx_Sender_Delay(DELAY_AFTER_F7);
    }

    while( send_offset < 512 && offset < filesize ) {
      MIOS_MIDI_TxBufferPut(c=sdcard_buffer_p1[(unsigned char)send_offset]);
      ++send_offset;
      ++offset;
      if( c == 0xf7 )
	SysEx_Sender_Delay(DELAY_AFTER_F7);
    }
  }

  // final message at second line
  MIOS_LCD_CursorSet(0x40);
  MIOS_LCD_PrintCString("Transfer done.  ");

  return 0;
}

/////////////////////////////////////////////////////////////////////////////
// Delay function for SysEx sender, used to add delays after F7
/////////////////////////////////////////////////////////////////////////////
void SysEx_Sender_Delay(unsigned int delay) __wparam
{
  while( delay-- ) {
    // wait for 1 mS
    MIOS_Delay(1);
    // feed watchdog
__asm
    clrwdt
__endasm;
  }
}
