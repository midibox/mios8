// $Id$
//
//  Optional USB Descriptors for MBHP_USB_GM5 module, which can
//  be stored in an external IIC EEPROM to customize the PID/VID (and more)
//
//  The tables are statically located at 0x7000 to simplify the programming
//  of the EEPROM
//
// ==========================================================================
//
//  Copyright 2008 Thorsten Klose (tk@midibox.org)
//  Licensed for personal non-commercial use only.
//  All other rights reserved.
// 
// ==========================================================================

/////////////////////////////////////////////////////////////////////////////
// Include files
/////////////////////////////////////////////////////////////////////////////

#include <cmios.h>
#include <pic18fregs.h>

#include <iic_eeprom8.h>
#include "eeprom_content.h"


/////////////////////////////////////////////////////////////////////////////
// Local Definitions
/////////////////////////////////////////////////////////////////////////////

// 1 to stay compatible to USB MIDI spec, 0 as workaround for some windows versions...
#define USE_AC_INTERFACE        1

// allowed numbers: 1..5
#define NUMBER_MIDI_INTERFACES  3


// don't touch
#define DSCR_DEVICE	1	// Descriptor type: Device
#define DSCR_CONFIG	2	// Descriptor type: Configuration
#define DSCR_STRING	3	// Descriptor type: String
#define DSCR_INTRFC	4	// Descriptor type: Interface
#define DSCR_ENDPNT	5	// Descriptor type: Endpoint

#define CS_INTERFACE	0x24	// Class-specific type: Interface
#define CS_ENDPOINT	0x25	// Class-specific type: Endpoint


/////////////////////////////////////////////////////////////////////////////
// Local Variables
/////////////////////////////////////////////////////////////////////////////

// for debugging - reads back EEPROM content, so that it can be read into the MIOS Studio Debug Window
unsigned char eeprom_content[0x100];


/////////////////////////////////////////////////////////////////////////////
// String descriptors
/////////////////////////////////////////////////////////////////////////////

const unsigned char __at(0x7010) USBDSC_STR0[] = {
  4,                            // String descriptor length
  DSCR_STRING,			// Descriptor Type
  0x09, 0x04,			// Charset
};

const unsigned char __at(0x7014) USBDSC_STR1[] = {
  sizeof(USBDSC_STR1)*2-2,      // String descriptor length
  DSCR_STRING,			// Descriptor Type
  'w',                          // String (for GM5 in ASCII, not Unicode!)
  'w',
  'w',
  '.',
  'm',
  'i',
  'd',
  'i',
  'b',
  'o',
  'x',
  '.',
  'o',
  'r',
  'g',
};

const unsigned char __at(0x7028) USBDSC_STR2[] = {
  sizeof(USBDSC_STR2)*2-2,      // String descriptor length
  DSCR_STRING,			// Descriptor Type
  'M',                          // String (for GM5 in ASCII, not Unicode!)
  'B',
  'H',
  'P',
  ' ',
  'U',
  'S',
  'B',
  ' ',
  'G',
  'M',
  '5',
};

const unsigned char __at(0x7038) USBDSC_STR3[] = {
  sizeof(USBDSC_STR3)*2-2,      // String descriptor length
  DSCR_STRING,			// Descriptor Type
  'G',                          // String (for GM5 in ASCII, not Unicode!)
  'M',
  '5',
};


/////////////////////////////////////////////////////////////////////////////
// Device descriptor
/////////////////////////////////////////////////////////////////////////////

const unsigned char __at(0x7040) USBDSC_DEVICE[] = {
  // MIDI Adapter Device Descriptor
  sizeof(USBDSC_DEVICE),	// Descriptor length
  DSCR_DEVICE,			// Decriptor type
  0x0200 & 0xff,		// Specification Version (BCD, LSB)
  0x0200 >> 8,			// Specification Version (BCD, MSB)
  0x00,				// Device class
  0x00,				// Device sub-class
  0x00,				// Device sub-sub-class
  0x10,				// Maximum packet size for EP0
  0x16c0 & 0xff,		// Vendor ID 16C0 --- sponsored by voti.nl! see http://www.voti.nl/pids
  0x16c0 >> 8,			// Vendor ID (MSB)
  1022 & 0xff,			// Product ID (LSB)
  1022 >> 8,			// Product ID (MSB)
  0x0000 & 0xff,		// Product version ID (LSB)
  0x0001 >> 8,  		// Product version ID (MSB)
  0x01,				// Manufacturer string index
  0x02,				// Product string index
  0x00,				// Serial number string index
  0x01,				// Number of configurations
};


/////////////////////////////////////////////////////////////////////////////
// Configuration descriptor
/////////////////////////////////////////////////////////////////////////////

const unsigned char __at(0x7052) USBDSC_CFG[] = {
  // Configuration Descriptor
  9,				// Descriptor length
  DSCR_CONFIG,			// Descriptor type
  sizeof(USBDSC_CFG)&0xff,	// Config + End Points length (LSB)
  sizeof(USBDSC_CFG)>>8,	// Config + End Points length (MSB)
#if USE_AC_INTERFACE
  0x02,				// Number of interfaces
#else
  0x01,				// Number of interfaces
#endif
  0x01,				// Interface number
  0x00,				// Configuration string
  0x80,				// Attributes (b7 - buspwr, b6 - selfpwr, b5 - rwu)
  0x10,				// Power requirement (div 2 ma)
#if USE_AC_INTERFACE
  // Standard AC Interface Descriptor
  9,				// Descriptor length
  DSCR_INTRFC,			// Descriptor type
  0x00,				// Zero-based index of this interface
  0x00,				// Alternate setting
  0x00,				// Number of end points 
  0x01,				// Interface class  (AUDIO)
  0x01,				// Interface sub class  (AUDIO_CONTROL)
  0x00,				// Interface sub sub class
  0x00,				// Interface descriptor string index

  // MIDI Adapter Class-specific AC Interface Descriptor
  9,				// Descriptor length
  CS_INTERFACE,			// Descriptor type
  0x01,				// Header subtype
  0x00,				// Revision of class specification - 1.0 (LSB)
  0x01,				// Revision of class specification (MSB)
  9,				// Total size of class-specific descriptors (LSB)
  0,				// Total size of class-specific descriptors (MSB)
  0x01,				// Number of streaming interfaces
  0x01,				// MIDI Streaming Interface 1 belongs to this AudioControl Interface
#endif

  // Standard MS Interface Descriptor
  9,				// Descriptor length
  DSCR_INTRFC,			// Descriptor type
#if USE_AC_INTERFACE
  0x01,				// Zero-based index of this interface
#else
  0x00,				// Zero-based index of this interface
#endif
  0x00,				// Alternate setting
  0x02,				// Number of end points 
  0x01,				// Interface class  (AUDIO)
  0x03,				// Interface sub class  (MIDISTREAMING)
  0x00,				// Interface sub sub class
  0x00,				// Interface descriptor string index

  // Class-specific MS Interface Descriptor
  7,				// Descriptor length
  CS_INTERFACE,			// Descriptor type
#if USE_AC_INTERFACE
  0x01,				// Zero-based index of this interface
#else
  0x00,				// Zero-based index of this interface
#endif
  0x00,				// revision of this class specification (LSB)
  0x01,				// revision of this class specification (MSB)
				// Total size of class-specific descriptors (LSB)
  7+(NUMBER_MIDI_INTERFACES)*(6+6+9+9)+9+(4+NUMBER_MIDI_INTERFACES)+9+(4+NUMBER_MIDI_INTERFACES),
  0,				// Total size of class-specific descriptors (MSB)

  // MIDI IN Jack Descriptor (Embedded)
  6,				// Descriptor length
  CS_INTERFACE,			// Descriptor type (CS_INTERFACE)
  0x02,				// MIDI_IN_JACK subtype
  0x01,				// EMBEDDED
  0x01,				// ID of this jack
  0x00,				// unused

  // MIDI Adapter MIDI IN Jack Descriptor (External)
  6,				// Descriptor length
  CS_INTERFACE,			// Descriptor type (CS_INTERFACE)
  0x02,				// MIDI_IN_JACK subtype
  0x02,				// EXTERNAL
  0x02,				// ID of this jack
  0x00,				// unused

  // MIDI Adapter MIDI OUT Jack Descriptor (Embedded)
  9,				// Descriptor length
  CS_INTERFACE,			// Descriptor type (CS_INTERFACE)
  0x03,				// MIDI_OUT_JACK subtype
  0x01,				// EMBEDDED
  0x03,				// ID of this jack
  0x01,				// number of input pins of this jack
  0x02,				// ID of the entity to which this pin is connected
  0x01,				// Output Pin number of the entity to which this input pin is connected
  0x00,				// unused

  // MIDI Adapter MIDI OUT Jack Descriptor (External)
  9,				// Descriptor length
  CS_INTERFACE,			// Descriptor type (CS_INTERFACE)
  0x03,				// MIDI_OUT_JACK subtype
  0x02,				// EXTERNAL
  0x04,				// ID of this jack
  0x01,				// number of input pins of this jack
  0x01,				// ID of the entity to which this pin is connected
  0x01,				// Output Pin number of the entity to which this input pin is connected
  0x00,				// unused

#if NUMBER_MIDI_INTERFACES >= 2
  // Second MIDI IN Jack Descriptor (Embedded)
  6,				// Descriptor length
  CS_INTERFACE,			// Descriptor type (CS_INTERFACE)
  0x02,				// MIDI_IN_JACK subtype
  0x01,				// EMBEDDED
  0x05,				// ID of this jack
  0x00,				// unused

  // Second MIDI Adapter MIDI IN Jack Descriptor (External)
  6,				// Descriptor length
  CS_INTERFACE,			// Descriptor type (CS_INTERFACE)
  0x02,				// MIDI_IN_JACK subtype
  0x02,				// EXTERNAL
  0x06,				// ID of this jack
  0x00,				// unused

  // Second MIDI Adapter MIDI OUT Jack Descriptor (Embedded)
  9,				// Descriptor length
  CS_INTERFACE,			// Descriptor type (CS_INTERFACE)
  0x03,				// MIDI_OUT_JACK subtype
  0x01,				// EMBEDDED
  0x07,				// ID of this jack
  0x01,				// number of input pins of this jack
  0x06,				// ID of the entity to which this pin is connected
  0x01,				// Output Pin number of the entity to which this input pin is connected
  0x00,				// unused

  // Second MIDI Adapter MIDI OUT Jack Descriptor (External)
  9,				// Descriptor length
  CS_INTERFACE,			// Descriptor type (CS_INTERFACE)
  0x03,				// MIDI_OUT_JACK subtype
  0x02,				// EXTERNAL
  0x08,				// ID of this jack
  0x01,				// number of input pins of this jack
  0x05,				// ID of the entity to which this pin is connected
  0x01,				// Output Pin number of the entity to which this input pin is connected
  0x00,				// unused
#endif


#if NUMBER_MIDI_INTERFACES >= 3
  // Third MIDI IN Jack Descriptor (Embedded)
  6,				// Descriptor length
  CS_INTERFACE,			// Descriptor type (CS_INTERFACE)
  0x02,				// MIDI_IN_JACK subtype
  0x01,				// EMBEDDED
  0x09,				// ID of this jack
  0x00,				// unused

  // Third MIDI Adapter MIDI IN Jack Descriptor (External)
  6,				// Descriptor length
  CS_INTERFACE,			// Descriptor type (CS_INTERFACE)
  0x02,				// MIDI_IN_JACK subtype
  0x02,				// EXTERNAL
  0x0a,				// ID of this jack
  0x00,				// unused

  // Third MIDI Adapter MIDI OUT Jack Descriptor (Embedded)
  9,				// Descriptor length
  CS_INTERFACE,			// Descriptor type (CS_INTERFACE)
  0x03,				// MIDI_OUT_JACK subtype
  0x01,				// EMBEDDED
  0x0b,				// ID of this jack
  0x01,				// number of input pins of this jack
  0x0a,				// ID of the entity to which this pin is connected
  0x01,				// Output Pin number of the entity to which this input pin is connected
  0x00,				// unused

  // Third MIDI Adapter MIDI OUT Jack Descriptor (External)
  9,				// Descriptor length
  CS_INTERFACE,			// Descriptor type (CS_INTERFACE)
  0x03,				// MIDI_OUT_JACK subtype
  0x02,				// EXTERNAL
  0x0c,				// ID of this jack
  0x01,				// number of input pins of this jack
  0x09,				// ID of the entity to which this pin is connected
  0x01,				// Output Pin number of the entity to which this input pin is connected
  0x00,				// unused
#endif


#if NUMBER_MIDI_INTERFACES >= 4
  // Fourth MIDI IN Jack Descriptor (Embedded)
  6,				// Descriptor length
  CS_INTERFACE,			// Descriptor type (CS_INTERFACE)
  0x02,				// MIDI_IN_JACK subtype
  0x01,				// EMBEDDED
  0x0d,				// ID of this jack
  0x00,				// unused

  // Fourth MIDI Adapter MIDI IN Jack Descriptor (External)
  6,				// Descriptor length
  CS_INTERFACE,			// Descriptor type (CS_INTERFACE)
  0x02,				// MIDI_IN_JACK subtype
  0x02,				// EXTERNAL
  0x0e,				// ID of this jack
  0x00,				// unused

  // Fourth MIDI Adapter MIDI OUT Jack Descriptor (Embedded)
  9,				// Descriptor length
  CS_INTERFACE,			// Descriptor type (CS_INTERFACE)
  0x03,				// MIDI_OUT_JACK subtype
  0x01,				// EMBEDDED
  0x0f,				// ID of this jack
  0x01,				// number of input pins of this jack
  0x0e,				// ID of the entity to which this pin is connected
  0x01,				// Output Pin number of the entity to which this input pin is connected
  0x00,				// unused

  // Fourth MIDI Adapter MIDI OUT Jack Descriptor (External)
  9,				// Descriptor length
  CS_INTERFACE,			// Descriptor type (CS_INTERFACE)
  0x03,				// MIDI_OUT_JACK subtype
  0x02,				// EXTERNAL
  0x10,				// ID of this jack
  0x01,				// number of input pins of this jack
  0x0d,				// ID of the entity to which this pin is connected
  0x01,				// Output Pin number of the entity to which this input pin is connected
  0x00,				// unused
#endif


#if NUMBER_MIDI_INTERFACES >= 5
  // Fifth MIDI IN Jack Descriptor (Embedded)
  6,				// Descriptor length
  CS_INTERFACE,			// Descriptor type (CS_INTERFACE)
  0x02,				// MIDI_IN_JACK subtype
  0x01,				// EMBEDDED
  0x11,				// ID of this jack
  0x00,				// unused

  // Fifth MIDI Adapter MIDI IN Jack Descriptor (External)
  6,				// Descriptor length
  CS_INTERFACE,			// Descriptor type (CS_INTERFACE)
  0x02,				// MIDI_IN_JACK subtype
  0x02,				// EXTERNAL
  0x12,				// ID of this jack
  0x00,				// unused

  // Fifth MIDI Adapter MIDI OUT Jack Descriptor (Embedded)
  9,				// Descriptor length
  CS_INTERFACE,			// Descriptor type (CS_INTERFACE)
  0x03,				// MIDI_OUT_JACK subtype
  0x01,				// EMBEDDED
  0x13,				// ID of this jack
  0x01,				// number of input pins of this jack
  0x12,				// ID of the entity to which this pin is connected
  0x01,				// Output Pin number of the entity to which this input pin is connected
  0x00,				// unused

  // Fifth MIDI Adapter MIDI OUT Jack Descriptor (External)
  9,				// Descriptor length
  CS_INTERFACE,			// Descriptor type (CS_INTERFACE)
  0x03,				// MIDI_OUT_JACK subtype
  0x02,				// EXTERNAL
  0x14,				// ID of this jack
  0x01,				// number of input pins of this jack
  0x11,				// ID of the entity to which this pin is connected
  0x01,				// Output Pin number of the entity to which this input pin is connected
  0x00,				// unused
#endif


  // Standard Bulk IN Endpoint Descriptor
  9,				// Descriptor length
  DSCR_ENDPNT,			// Descriptor type
  0x83,				// In Endpoint 3
  0x02,				// Bulk, not shared
  0x08,				// 8 bytes per packet (LSB)
  0x00,				// bytes per packet (MSB)
  0x00,				// ignore for bulk
  0x00,				// unused
  0x00,				// unused

  // Class-specific MS Bulk In Endpoint Descriptor
  4+NUMBER_MIDI_INTERFACES,	// Descriptor length
  CS_ENDPOINT,			// Descriptor type (CS_ENDPOINT)
  0x01,				// MS_GENERAL
  NUMBER_MIDI_INTERFACES,	// number of embedded MIDI Out Jacks
  0x03,				// ID of embedded MIDI Out Jack
#if NUMBER_MIDI_INTERFACES >= 2
  0x07,				// ID of embedded MIDI Out Jack
#endif
#if NUMBER_MIDI_INTERFACES >= 3
  0x0b,				// ID of embedded MIDI Out Jack
#endif
#if NUMBER_MIDI_INTERFACES >= 4
  0x0f,				// ID of embedded MIDI Out Jack
#endif
#if NUMBER_MIDI_INTERFACES >= 5
  0x13,				// ID of embedded MIDI Out Jack
#endif

  // Standard Bulk OUT Endpoint Descriptor
  9,				// Descriptor length
  DSCR_ENDPNT,			// Descriptor type
  0x04,				// Out Endpoint 4
  0x02,				// Bulk, not shared
  0x08,				// 8 bytes per packet (LSB)
  0x00,				// bytes per packet (MSB)
  0x00,				// ignore for bulk
  0x00,				// unused
  0x00,				// unused

  // Class-specific MS Bulk Out Endpoint Descriptor
  4+NUMBER_MIDI_INTERFACES,	// Descriptor length
  CS_ENDPOINT,			// Descriptor type (CS_ENDPOINT)
  0x01,				// MS_GENERAL
  NUMBER_MIDI_INTERFACES,	// number of embedded MIDI IN Jacks
  0x01,				// ID of embedded MIDI In Jack
#if NUMBER_MIDI_INTERFACES >= 2
  0x05,				// ID of embedded MIDI In Jack
#endif
#if NUMBER_MIDI_INTERFACES >= 3
  0x09,				// ID of embedded MIDI In Jack
#endif
#if NUMBER_MIDI_INTERFACES >= 4
  0x0d,				// ID of embedded MIDI In Jack
#endif
#if NUMBER_MIDI_INTERFACES >= 5
  0x11,				// ID of embedded MIDI In Jack
#endif
};


// prevent that more than 256 bytes are allocated
const unsigned char __at(0x7200) OVERRUN_BLOCKER[] = { 0x00 }; 


// ==========================================================================
// Initialize size variables for external access
// ==========================================================================
unsigned char EEPROM_CONTENT_Write(void) __wparam
{
  unsigned char error = 0;
  unsigned char addr;
  unsigned char *ptr;
  unsigned char addr_dscr_interface;
  unsigned char addr_dscr_endpoints;

  // determine pointer to interface descriptor
  addr_dscr_interface = USBDSC_CFG[0];
  if( USBDSC_CFG[addr_dscr_interface+1] != DSCR_INTRFC )
    return 0x40; // interface descriptor at wrong position
  addr_dscr_interface += ((unsigned char)USBDSC_CFG&0xff);

  // determine pointer to endpoint descriptor
  addr_dscr_endpoints=0;
  for(addr=0; addr<0xff;) {
    if( USBDSC_CFG[addr+1] == DSCR_ENDPNT ) {
      addr_dscr_endpoints = addr;
      break;
    }
    addr += USBDSC_CFG[addr+0];
  }
  
  if( !addr_dscr_endpoints ) {
    return 0x41; // endpoint descriptor not found
  }
  addr_dscr_endpoints += ((unsigned char)USBDSC_CFG&0xff);


  // ensure that verify mode is enabled
  MIOS_BOX_CFG1.BS_DIS_VERIFY=0;

  // select CS#0, block0
  MIOS_BOX_CFG1.BS_A=0;

  // program header
  if( error=IIC_EEPROM8_Write(0x00, (unsigned char)USBDSC_DEVICE&0xff) ) return error; // pointer to device descriptor
  if( error=IIC_EEPROM8_Write(0x01, (unsigned char)USBDSC_CFG&0xff) )    return error; // pointer to configuration descriptor
  if( error=IIC_EEPROM8_Write(0x02, 0x05) )                              return error; // pointer to string table
  if( error=IIC_EEPROM8_Write(0x03, addr_dscr_interface) )               return error; // pointer to interface descriptors
  if( error=IIC_EEPROM8_Write(0x04, addr_dscr_endpoints) )               return error; // pointer to entpoint descriptors

  // string header
  if( error=IIC_EEPROM8_Write(0x05, (unsigned char)USBDSC_STR0&0xff) ) return error;
  if( error=IIC_EEPROM8_Write(0x06, (unsigned char)USBDSC_STR1&0xff) ) return error;
  if( error=IIC_EEPROM8_Write(0x07, (unsigned char)USBDSC_STR2&0xff) ) return error;
  if( error=IIC_EEPROM8_Write(0x08, (unsigned char)USBDSC_STR1&0xff) ) return error;
  if( error=IIC_EEPROM8_Write(0x09, (unsigned char)USBDSC_STR1&0xff) ) return error;
  if( error=IIC_EEPROM8_Write(0x0a, (unsigned char)USBDSC_STR1&0xff) ) return error;
  if( error=IIC_EEPROM8_Write(0x0b, (unsigned char)USBDSC_STR3&0xff) ) return error;
  if( error=IIC_EEPROM8_Write(0x0c, (unsigned char)USBDSC_STR1&0xff) ) return error;

  if( error=IIC_EEPROM8_Write(0x0d, 0x00) )                            return error; // fill
  if( error=IIC_EEPROM8_Write(0x0e, 0x00) )                            return error; // fill
  if( error=IIC_EEPROM8_Write(0x0f, 0x00) )                            return error; // fill

  // program descriptors at 0x10..0xff
  addr=0x10;
  ptr=(unsigned char *)0x7010;
  do {
    if( error = IIC_EEPROM8_Write(addr, *ptr) )
      return error;
    ++ptr;
  } while( ++addr );

  // select CS#0, block1
  MIOS_BOX_CFG1.BS_A=1;

  do {
    if( error = IIC_EEPROM8_Write(addr, *ptr) )
      return error;
    ++ptr;
  } while( ++addr );


  // select CS#0, block0
  MIOS_BOX_CFG1.BS_A=0;

  // read back EEPROM content of bank0 for debugging
  addr=0;
  do {
    eeprom_content[addr] = IIC_EEPROM8_Read(addr);
  } while( ++addr );

  return MIOS_BOX_STAT.BS_AVAILABLE ? 0x00 : 0x02;
}

