/*
 * CORE 2 PART FOR THE PROTODECK CONTROLLER
 * main.c
 *
 * Note: hardware settings (e.g. number of pots, muxed/unmuxed mode) are specified in main.h
 *
 * See also http://www.julienbayle.net/diy/protodeck/ for additional informations about protodeck
 * See also http://www.ucapps.de/ for midibox framework informations
 *
 * ==========================================================================
 *
 * Compilation Makefile and Makefile.orig are made to link 18f4620 PIC libraries as the protodeck brains are 18f4620.
 * Be careful if you don't have a 18f4620, you'll have to change things
 *
 * before compiling anything, you have to check it and install your toolchain :
 * http://www.midibox.org/dokuwiki/doku.php?id=windows_toolchain_quickstart
 *
 * ==========================================================================
 *
 *  Copyright 2009 protofuse (aka julien.bayle)
 *	Inspired by noofny/mike code
 *  Licensed for personal non-commercial use only.
 *  All other rights reserved.
 *
 *  BIG credits to the midibox framework creator guru: Thorsten Klose
 *
 * ==========================================================================
 */

/////////////////////////////////////////////////////////////////////////////
// Include files
/////////////////////////////////////////////////////////////////////////////
#include <cmios.h>
#include <pic18fregs.h>
#include "protodeck.h"
#include "main.h"

/////////////////////////////////////////////////////////////////////////////
// Global variables
/////////////////////////////////////////////////////////////////////////////

// status of application (see bitfield declaration in main.h)
app_flags_t app_flags;

/////////////////////////////////////////////////////////////////////////////
// Local variables
/////////////////////////////////////////////////////////////////////////////

//unsigned static int current_sysex_package = -1;

// we create a 2-dimensional array with 64 entries for mapping between potentiometers movements & events associated
// each entry consists of two bytes:
//   o one for the first MIDI byte (MIDI status) => 0xb* means CC message for Channel *
//   o a second for the second MIDI byte (=> CC number)
// The meaning of the bytes can be found in the MIDI spec
// (-> http://www.harmony-central.com/MIDI/Doc/table1.html)
// (-> http://www.harmony-central.com/MIDI/Doc/table2.html)
// (-> http://www.harmony-central.com/MIDI/Doc/table3.html)
const unsigned char pot_event_map[64][2] = {
		//AIN 21
		// SR1 = instrument control 2 for channels 1-8
		{0xb0, 0x21},   {0xb0, 0x22},   {0xb0, 0x23},   {0xb0, 0x24},
		{0xb0, 0x25},   {0xb0, 0x26},   {0xb0, 0x27},   {0xb0, 0x28},
		// SR2 = instrument control 3 for channels 1-8
		{0xb0, 0x29},   {0xb0, 0x2A},   {0xb0, 0x2B},   {0xb0, 0x2C},
		{0xb0, 0x2D},   {0xb0, 0x2E},   {0xb0, 0x2F},   {0xb0, 0x30},
		// SR3 = volume control for channels 1-8
		{0xb0, 0x31},   {0xb0, 0x32},   {0xb0, 0x33},   {0xb0, 0x34},
		{0xb0, 0x35},   {0xb0, 0x36},   {0xb0, 0x37},   {0xb0, 0x38},
		// SR4 = drums controls pots
		{0xb0, 0x39},   {0xb0, 0x3A},   {0xb0, 0x3B},   {0xb0, 0x3C},	{0xb0, 0x3D},   {0xb0, 0x3E},
		{0xb0, 0x7F},   {0xb0, 0x7F}, // unused

		//AIN 22
		// SR1 = EQ
		{0xb0, 0x3F},   {0xb0, 0x40},   {0xb0, 0x41},   {0xb0, 0x42},	{0xb0, 0x43},   {0xb0, 0x44},
		{0xb0, 0x7F},   {0xb0, 0x7F}, // unused
		// SR2 = drums controls pots
		{0xb0, 0x45},   {0xb0, 0x46},   {0xb0, 0x47},   {0xb0, 0x48},	{0xb0, 0x49},   {0xb0, 0x4A},
		{0xb0, 0x7F},   {0xb0, 0x7F}, // unused
		// SR3 = Master FX Rack A controls + Master FX Rack B controls
		{0xb0, 0x4B},   {0xb0, 0x4C},   {0xb0, 0x4F},   {0xb0, 0x50},
		{0xb0, 0x4D},	{0xb0, 0x4E}, 	{0xb0, 0x51},	{0xb0, 0x52},
		// SR4 = Master Volume = channel 16
		{0xb0, 0x53},
		// multifunctionnal purpose pots = channel 16
		{0xb0, 0x54},   {0xb0, 0x55},   {0xb0, 0x56},   {0xb0, 0x57},
		{0xbF, 0x7F},   {0xbF, 0x7F}, 	{0xbF, 0x7F} // unused
};


// we create a 2-dimensional array with 22 entries for mapping between switches  push & events associated
// each entry consists of two bytes:
//   o one for the first MIDI byte (MIDI status) => 0x9* means Note message for Channel *
//   o a second for the second MIDI byte (=> Note number)
// This const is parsed lower by void DIN_NotifyToggle()
// The meaning of the bytes can be found in the MIDI spec
// (-> http://www.harmony-central.com/MIDI/Doc/table1.html)
// (-> http://www.harmony-central.com/MIDI/Doc/table2.html)
// (-> http://www.harmony-central.com/MIDI/Doc/table3.html)
const unsigned char button_event_map[32][2] = {
		//-------- DIN 21
		// SR1 = send A controls + send B controls
		{0x90, 0x41},   {0x90, 0x42},	{0x90, 0x43},   {0x90, 0x44},
		// unused
		{0x90, 0x7F},   {0x90, 0x7F},	{0x90, 0x7F},   {0x90, 0x7F},

		// SR2 = Instruments control
		{0x90, 0x45},   {0x90, 0x46},	{0x90, 0x47},   {0x90, 0x48},
		{0x90, 0x49},   {0x90, 0x4A},	{0x90, 0x4B},   {0x90, 0x4C},

		// SR3 = Drums  mute
		{0x90, 0x4D},   {0x90, 0x4E},	{0x90, 0x4F},   {0x90, 0x50},	{0x90, 0x51},   {0x90, 0x52},
		// unused
		{0x90, 0x7F},   {0x90, 0x7F},

		// SR4 = Master FX Rack A controls = channel 16
		{0x90, 0x53},   {0x90, 0x54},
		// Buttons 15 & 16 = Master FX Rack B controls = channel 16
		{0x90, 0x55},   {0x90, 0x56},

		// Buttons multi-purpose
		{0x90, 0x57},   {0x90, 0x58},	{0x90, 0x59},   {0x90, 0x5A}
};


// we create a 1-dimensional array with 32 entries for leds colors storage
// each entry consists of 1 byte coded like that:
// 0x00 = OFF
// 0x10 = RED
// 0x20 = GREEN
// 0x40 = BLUE
// 0x30 = RED + GREEN =  YELLOW
// 0x60= GREEN + BLUE = CYAN
// 0x50 = RED + BLUE = MAGENTA
// 0x70 = WHITE
// structure is matrix[]=COLOR
static unsigned char matrix[32] ;

// blinking stuff
// thanks TK: http://www.midibox.org/forum/index.php/topic,14129.msg121892.html#msg121892
#if BLINKING_FEATURE
	unsigned char matrixBlinkingState[32] ;
	unsigned char flash_ctr;
#endif

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS after startup to initialize the
// application
/////////////////////////////////////////////////////////////////////////////
void Init(void) __wparam
{
	// configure the core2 as midibox-link END Point
	// see infos about midibox-link here: http://www.ucapps.de/midibox_link.html
	MIOS_MIDI_MergerSet(MIOS_MIDI_MERGER_MBLINK_EP);

	// set shift register update frequency
	MIOS_SRIO_UpdateFrqSet(1); // ms

	// we need to set at least one IO shift register pair
	MIOS_SRIO_NumberSet(NUMBER_OF_SRIO);

	// debouncing value for DINs
	MIOS_SRIO_DebounceSet(DIN_DEBOUNCE_VALUE);

	MIOS_SRIO_TS_SensitivitySet(DIN_TS_SENSITIVITY);

	// initialize the AIN driver
	MIOS_AIN_NumberSet(AIN_NUMBER_INPUTS);
#if AIN_MUXED_MODE
	MIOS_AIN_Muxed();
#else
	MIOS_AIN_UnMuxed();
#endif
	MIOS_AIN_DeadbandSet(AIN_DEADBAND);



	// all pin of row & column driver low
	MIOS_DOUT_SRSet(0, 0);
	MIOS_DOUT_SRSet(1, 0);
	MIOS_DOUT_SRSet(2, 0);
	MIOS_DOUT_SRSet(3, 0);

	// hello sequence
	DoStartupSequence();
	
		// note on hello from core2
	MIOS_MIDI_BeginStream();
	MIOS_MIDI_TxBufferPut(0x90);
	MIOS_MIDI_TxBufferPut(0x7F);
	MIOS_MIDI_TxBufferPut(0x7F);
	MIOS_MIDI_EndStream();
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
	if( (evnt0 & 0xf0) == 0x90)								// being sure that this is a note message
	{
		if(evnt1 >= _NOTE_MATRIX_OFFSET && evnt1 <= 0x7A)	// being sure parsing message for DOUT21 (matrix[] bounds are implicitely verified)
		{
			unsigned char noteIndex = evnt1;
			unsigned char value = evnt2;
			matrix[noteIndex - _NOTE_MATRIX_OFFSET] = value;
		}
		else if(evnt1 == 0x7D)	// clear the matrix with only one midi message
		{
			 ClearMatrix();
		}
	}

	/*
		int channelIndex = evnt0-0x90;
		int noteIndex = evnt1;
		unsigned char value = evnt2;
	#if BLINKING_FEATURE
		if (value != _BLINKING_OFF_VELOCITY || value != _BLINKING_ON_VELOCITY)
			matrix[8*(noteIndex - 1 - _NOTE_MATRIX_OFFSET)+channelIndex] = value; //-1 cause note begin at 1 and row at 0

		else if (value == _BLINKING_ON_VELOCITY) // value = 0x7F means blinking state ON
			matrixBlinkingState[8*(noteIndex - 1 - _NOTE_MATRIX_OFFSET)+channelIndex] = 0x01;

		else if (value == _BLINKING_OFF_VELOCITY) // value = 0x7E means blinking state OFF
			matrixBlinkingState[8*(noteIndex - 1 - _NOTE_MATRIX_OFFSET)+channelIndex] = 0x00;
	#else
		matrix[8*(noteIndex - 1 - _NOTE_MATRIX_OFFSET)+channelIndex] = value;
	#endif
	 */
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

void DisplayLED(unsigned int column, unsigned char color) __wparam
{
	color >>= 4;
	MIOS_DOUT_PinSet(column+8,		(unsigned char)(color & 0x01)); // RED
	color >>= 1;
	MIOS_DOUT_PinSet(column+8+8,	(unsigned char)(color & 0x01)); // GREEN
	color >>= 1;
	MIOS_DOUT_PinSet(column+8+8+8,	(unsigned char)(color & 0x01));	// BLUE
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS before the shift register are loaded
/////////////////////////////////////////////////////////////////////////////
void SR_Service_Prepare(void) __wparam
{
	static unsigned int row;
	static unsigned int lastrow;
	unsigned int x;

	row = ++row & 0x03;						// row cycling
	MIOS_DOUT_PinSet0(lastrow);				// lastrow OFF
	MIOS_DOUT_PinSet1(row);					// current row ON

	for (x = 0; x < 8; x++)
	{
		DisplayLED(x , matrix[x+row*8]);	// displaying the led (x,row)
	}
	lastrow = row;

	/*
		static unsigned int row;
		static unsigned int lastrow;
		unsigned char color;
		unsigned int x;

		row = ++row & 0x07;						// row cycling
		MIOS_DOUT_PinSet0(lastrow);				// lastrow OFF
		MIOS_DOUT_PinSet1(row);					// current row ON

		for (x = 0; x < 8; x++)
		{
	#if BLINKING_FEATURE
			if( flash_ctr >= 200 ) {
				flash_ctr = 0;
				if (matrixBlinkingState[x+row*8] == 0x00)
						DisplayLED(x , matrix[x+row*8]);	// displaying the led (x,row) not blinking
				else 	DisplayLED(x , 0x00);			// OFF this led cause it blinks
			}
	#else
			DisplayLED(x , matrix[x+row*8]);
	#endif
		}
		lastrow = row;
		flash_ctr++;
	 */
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
	// send mapped midi-note with 127 or 0 velocity
	MIOS_MIDI_TxBufferPut((unsigned char) button_event_map[pin][0]); // first byte from table = CHANNEL
	MIOS_MIDI_TxBufferPut((unsigned char) button_event_map[pin][1]); // second byte from table = NOTE
	MIOS_MIDI_TxBufferPut(pin_value ? 0x00 : 0x7f);
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
	// send mapped CC value
	MIOS_MIDI_TxBufferPut((unsigned char)pot_event_map[pin][0]); // first value from table
	MIOS_MIDI_TxBufferPut((unsigned char)pot_event_map[pin][1]); // second value from table
	MIOS_MIDI_TxBufferPut(MIOS_AIN_Pin7bitGet(pin)); // 7bit pot value
}


// ----------------------------------------------------------------------------------------------------------------

void ClearMatrix(void) __wparam
{
	unsigned int index = 0;
	for (index = 0; index < 64; index++)
	{
		matrix[index] = _COLOR_OFF;
	}
}

#if BLINKING_FEATURE
void ClearMatrixBlinking(void) __wparam
{
	unsigned int index = 0;
	for (index = 0; index < 64; index++)
	{
		matrixBlinkingState[index] = _COLOR_OFF;
	}
}
#endif

void DoStartupSequence(void) __wparam
{
	unsigned int index;
	unsigned int waitTime=25;
	for (index = 0; index < 32; index++)
	{
		matrix[index] = _COLOR_GREEN ;
		MIOS_Delay(waitTime);
	}
}
