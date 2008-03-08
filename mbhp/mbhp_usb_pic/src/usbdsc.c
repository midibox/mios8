// $Id$
//
//  USB Descriptors
//
//  They are defined within C arrays in order to avoid patting caused 
//  by "db" commands
//
// ==========================================================================
//
//  Copyright 2006 Thorsten Klose (tk@midibox.org)
//  Licensed for personal non-commercial use only.
//  All other rights reserved.
// 
// ==========================================================================

// 1 to stay compatible to USB MIDI spec, 0 as workaround for some windows versions...
#define USE_AC_INTERFACE	1

// allowed numbers: 1..5 - they are controlled from MAKEFILE.SPEC
// Changes here have no effect (since NUMBER_MIDI_INTERFACES is always defined!)
#ifndef NUMBER_MIDI_INTERFACES
#define NUMBER_MIDI_INTERFACES  1
#endif

// ==========================================================================

#define DSCR_DEVICE	1	// Descriptor type: Device
#define DSCR_CONFIG	2	// Descriptor type: Configuration
#define DSCR_STRING	3	// Descriptor type: String
#define DSCR_INTRFC	4	// Descriptor type: Interface
#define DSCR_ENDPNT	5	// Descriptor type: Endpoint

#define CS_INTERFACE	0x24	// Class-specific type: Interface
#define CS_ENDPOINT	0x25	// Class-specific type: Endpoint

// ==========================================================================

extern const unsigned char USBDSC_DEVICE[];
const unsigned char USBDSC_DEVICE[] = {
  // MIDI Adapter Device Descriptor
  sizeof(USBDSC_DEVICE),	// Descriptor length
  DSCR_DEVICE,			// Decriptor type
  0x0200 & 0xff,		// Specification Version (BCD, LSB)
  0x0200 >> 8,			// Specification Version (BCD, MSB)
  0x00,				// Device class
  0x00,				// Device sub-class
  0x00,				// Device sub-sub-class
  0x40,				// Maximum packet size
  0x16c0 & 0xff,		// Vendor ID 16C0 --- sponsored by voti.nl! see http://www.voti.nl/pids
  0x16c0 >> 8,			// Vendor ID (MSB)
  1021 & 0xff,			// Product ID (LSB)
  1021 >> 8,			// Product ID (MSB)
  0x0000 & 0xff,		// Product version ID (LSB)
  0x0003 >> 8,  		// Product version ID (MSB)
  0x01,				// Manufacturer string index
  0x02,				// Product string index
  0x00,				// Serial number string index
  0x01,				// Number of configurations
};

const unsigned char USBDSC_CFG[] = {
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
  0x03,				// Configuration string
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
  0x02,				// Interface descriptor string index

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
  0x02,				// Interface descriptor string index

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
  7+NUMBER_MIDI_INTERFACES*(6+6+9+9)+9+(4+NUMBER_MIDI_INTERFACES)+9+(4+NUMBER_MIDI_INTERFACES),
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



  // Standard Bulk OUT Endpoint Descriptor
  9,				// Descriptor length
  DSCR_ENDPNT,			// Descriptor type
  0x02,				// Out Endpoint 2
  0x02,				// Bulk, not shared
  0x40,				// 64 bytes per packet (LSB)
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

  // Standard Bulk IN Endpoint Descriptor
  9,				// Descriptor length
  DSCR_ENDPNT,			// Descriptor type
  0x82,				// In Endpoint 2
  0x02,				// Bulk, not shared
  0x40,				// 64 bytes per packet (LSB)
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
};

const unsigned char USBDSC_STR0[] = {
  4,                            // String descriptor length
  DSCR_STRING,			// Descriptor Type
  0x09, 0x04,			// Charset
};

const unsigned char USBDSC_STR1[] = {
  14*2+2,                       // String descriptor length
  DSCR_STRING,			// Descriptor Type
  'T',00,
  'h',00,
  'o',00,
  'r',00,
  's',00,
  't',00,
  'e',00,
  'n',00,
  ' ',00,
  'K',00,
  'l',00,
  'o',00,
  's',00,
  'e',00,
};

const unsigned char USBDSC_STR2[] = {
  12*2+2,                       // String descriptor length
  DSCR_STRING,			// Descriptor Type
  'M',00,
  'B',00,
  'H',00,
  'P',00,
  ' ',00,
  'U',00,
  'S',00,
  'B',00,
  ' ',00,
  'P',00,
  'I',00,
  'C',00,
};

const unsigned char USBDSC_STR3[] = {
  16*2+2,                       // String descriptor length
  DSCR_STRING,			// Descriptor Type
  'T',00,
  '.',00,
  'K',00,
  'l',00,
  'o',00,
  's',00,
  'e',00,
  ' ',00,
  'M',00,
  'B',00,
  'H',00,
  'P',00,
  ' ',00,
  'U',00,
  'S',00,
  'B',00,
};


void dummy(void) {
__asm
	global	_USBDSC_CFG_LIST
_USBDSC_CFG_LIST:
	dw	_USBDSC_CFG

	global	_USBDSC_STR_LIST
_USBDSC_STR_LIST:
	dw	_USBDSC_STR0
	dw	_USBDSC_STR1
	dw	_USBDSC_STR2
	dw	_USBDSC_STR3
__endasm;
}

