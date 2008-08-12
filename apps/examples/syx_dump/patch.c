// $Id$
/*
 * Patch Layer
 * see README.txt for details
 *
 * ==========================================================================
 *
 *  Copyright (C) 2008 Thorsten Klose (tk@midibox.org)
 *  Licensed for personal non-commercial use only.
 *  All other rights reserved.
 * 
 * ==========================================================================
 */

/////////////////////////////////////////////////////////////////////////////
// Include files
/////////////////////////////////////////////////////////////////////////////

#include <cmios.h>

#include "patch.h"


/////////////////////////////////////////////////////////////////////////////
// Global Variables
/////////////////////////////////////////////////////////////////////////////

// the patch structure (could also be located somewhere else, depending on
// where and how you are storing values in RAM)
unsigned char patch_structure[256];


/////////////////////////////////////////////////////////////////////////////
// This function initializes the patch structure
/////////////////////////////////////////////////////////////////////////////
void PATCH_Init(void)
{
  // load first patch
  PATCH_Load(0, 0); // bank, patch
}


/////////////////////////////////////////////////////////////////////////////
// This function returns a byte from patch structure in RAM
/////////////////////////////////////////////////////////////////////////////
unsigned char PATCH_ReadByte(unsigned char addr)
{
  return patch_structure[addr];
}

/////////////////////////////////////////////////////////////////////////////
// This function writes a byte into patch structure in RAM
/////////////////////////////////////////////////////////////////////////////
void PATCH_WriteByte(unsigned char addr, unsigned char byte)
{
  patch_structure[addr] = byte;
}


/////////////////////////////////////////////////////////////////////////////
// This function loads the patch structure from EEPROM/BankStick
// Returns != 0 if Load failed (e.g. BankStick not connected)
/////////////////////////////////////////////////////////////////////////////
unsigned char PATCH_Load(unsigned char bank, unsigned char patch)
{
  unsigned char i;

#if PATCH_USE_BANKSTICK
  // determine offset depending on patch number
  unsigned int offset = patch << 8;
  unsigned char error;

  // select BankStick
  MIOS_BANKSTICK_CtrlSet(bank & 0x07);

  // use 64byte page load functions for faster access
  for(i=0; i<4; ++i)
    if( error = MIOS_BANKSTICK_ReadPage(offset + i*0x40, (char *)(patch_structure + i*0x40)) )
      return error;
#else
  // ignore patch and bank
  // use 64byte page load functions for faster access
  for(i=0; i<4; ++i)
    MIOS_EEPROM_ReadPage(i*0x40, (char *)(patch_structure + i*0x40));
#endif

  return 0;
}


/////////////////////////////////////////////////////////////////////////////
// This function stores the patch structure into EEPROM/BankStick
// Returns != 0 if Store failed (e.g. BankStick not connected)
/////////////////////////////////////////////////////////////////////////////
unsigned char PATCH_Store(unsigned char bank, unsigned char patch)
{
  unsigned char i;

#if PATCH_USE_BANKSTICK
  // determine offset depending on patch number
  unsigned int offset = patch << 8;
  unsigned char error;

  // select BankStick
  MIOS_BANKSTICK_CtrlSet(bank & 0x07);

  // use 64byte page write functions for faster access
  for(i=0; i<4; ++i)
    if( error = MIOS_BANKSTICK_WritePage(offset + i*0x40, (char *)(patch_structure + i*0x40)) )
      return error;
#else
  // ignore patch and bank
  // use 64byte page write functions for faster access
  for(i=0; i<4; ++i)
    MIOS_EEPROM_WritePage(i*0x40, (char *)(patch_structure + i*0x40));
#endif

  return 0;
}
