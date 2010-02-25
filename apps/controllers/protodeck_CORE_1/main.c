/*
 * CORE 1 PART FOR THE PROTODECK CONTROLLER
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
#include <protodeck.h>
#include "main.h"

/////////////////////////////////////////////////////////////////////////////
// Global variables
/////////////////////////////////////////////////////////////////////////////

// status of application (see bitfield declaration in main.h)
app_flags_t app_flags;

/////////////////////////////////////////////////////////////////////////////
// Local variables
/////////////////////////////////////////////////////////////////////////////

// we create a 2-dimensional array with 23 entries for mapping between potentiometers movements & events associated
// each entry consists of two bytes:
//   o one for the first MIDI byte (MIDI status) => 0xb* means CC message for Channel *
//   o a second for the second MIDI byte (=> CC number)
// The meaning of the bytes can be found in the MIDI spec
// (-> http://www.harmony-central.com/MIDI/Doc/table1.html)
// (-> http://www.harmony-central.com/MIDI/Doc/table2.html)
// (-> http://www.harmony-central.com/MIDI/Doc/table3.html)
const unsigned char pot_event_map[32][2] = {
		//-------- AIN 11
		// SR1 = instrument control 1 for channels 1-8
		{0xb0, 0x11},   {0xb0, 0x12},   {0xb0, 0x13},   {0xb0, 0x14},
		{0xb0, 0x15},   {0xb0, 0x16},   {0xb0, 0x17},   {0xb0, 0x18},
		// SR2 = send B rate for channels 1-8
		{0xb0, 0x09},   {0xb0, 0x0A},   {0xb0, 0x0B},   {0xb0, 0x0C},
		{0xb0, 0x0D},   {0xb0, 0x0E},   {0xb0, 0x0F},   {0xb0, 0x10},
		// SR3 = send A rate for channels 1-8
		{0xb0, 0x01},   {0xb0, 0x02},   {0xb0, 0x03},   {0xb0, 0x04},
		{0xb0, 0x05},   {0xb0, 0x06},   {0xb0, 0x07},   {0xb0, 0x08},
		// SR4 = send A controls + send B controls
		{0xb0, 0x19},   {0xb0, 0x1A},   {0xb0, 0x1B},   {0xb0, 0x1C},
		{0xb0, 0x1D},   {0xb0, 0x1E},   {0xb0, 0x1F},   {0xb0, 0x20}
};


// we create a 2-dimensional array with 64 entries for mapping between switches  push & events associated
// each entry consists of two bytes:
//   o one for the first MIDI byte (MIDI status) => 0x9* means Note message for Channel *
//   o a second for the second MIDI byte (=> Note number)
// This const is parsed lower by void DIN_NotifyToggle()
// The meaning of the bytes can be found in the MIDI spec
// (-> http://www.harmony-central.com/MIDI/Doc/table1.html)
// (-> http://www.harmony-central.com/MIDI/Doc/table2.html)
// (-> http://www.harmony-central.com/MIDI/Doc/table3.html)
const unsigned char button_event_map[64][2] = { //------- clip matrix control
		//-------- DIN11
		// SR1 = tracks mute
		{0x90, 0x01},   {0x90, 0x02},   {0x90, 0x03},   {0x90, 0x04},
		{0x90, 0x05},   {0x90, 0x06},   {0x90, 0x07},   {0x90, 0x08},
		// SR2 = Buttons scenes control
		// Buttons offset control
		{0x90, 0x10},   {0x90, 0x0F},
		// Scenes control
		{0x90, 0x09},   {0x90, 0x0A}, {0x90, 0x0B},   {0x90, 0x0C},
		{0x90, 0x0D},   {0x90, 0x0E},
		// SR3 = Buttons scenes 1
		{0x90, 0x11},   {0x90, 0x12},   {0x90, 0x13},   {0x90, 0x14},
		{0x90, 0x15},   {0x90, 0x16},   {0x90, 0x17},   {0x90, 0x18},
		// SR4 = Buttons scenes 2
		{0x90, 0x19},   {0x90, 0x1A},   {0x90, 0x1B},   {0x90, 0x1C},
		{0x90, 0x1D},   {0x90, 0x1E},   {0x90, 0x1F},   {0x90, 0x20},
		//-------- DIN12
		// SR1 = Buttons scenes 3
		{0x90, 0x21},   {0x90, 0x22},   {0x90, 0x23},   {0x90, 0x24},
		{0x90, 0x25},   {0x90, 0x26},   {0x90, 0x27},   {0x90, 0x28},
		// SR2 = Buttons scenes 4
		{0x90, 0x29},   {0x90, 0x2A},   {0x90, 0x2B},   {0x90, 0x2C},
		{0x90, 0x2D},   {0x90, 0x2E},   {0x90, 0x2F},   {0x90, 0x30},
		// SR3 = Buttons scenes 5
		{0x90, 0x31},   {0x90, 0x32},   {0x90, 0x33},   {0x90, 0x34},
		{0x90, 0x35},   {0x90, 0x36},   {0x90, 0x37},   {0x90, 0x38},
		// SR4 = Buttons scenes 6
		{0x90, 0x39},   {0x90, 0x3A},   {0x90, 0x3B},   {0x90, 0x3C},
		{0x90, 0x3D},   {0x90, 0x3E},   {0x90, 0x3F},   {0x90, 0x40}
};

// we create a 1-dimensional array with 64 entries for leds colors storage
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
static unsigned char matrix[64] ;

const unsigned char patternInit[64] = {
		0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,
		0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,
		0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,
		0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,
		0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,
		0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,
		0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,
		0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40
};

// blinking stuff
// thanks TK: http://www.midibox.org/forum/index.php/topic,14129.msg121892.html#msg121892
#if BLINKING_FEATURE
	unsigned char matrixBlinkingState[64] ;
	unsigned char flash_ctr;
#endif


/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS after startup to initialize the
// application
/////////////////////////////////////////////////////////////////////////////
void Init(void) __wparam
{
	// configure the core1 as midibox-link FORWARD Point
	// see infos about midibox-link here: http://www.ucapps.de/midibox_link.html
	MIOS_MIDI_MergerSet(MIOS_MIDI_MERGER_MBLINK_FP);

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

	// arbitrarely delay to wait for core2 (race condition for which core1 which has to win!)
	MIOS_Delay(250);
	MIOS_Delay(250);
	MIOS_Delay(250);
	MIOS_Delay(250);
	MIOS_Delay(250);


	// all pin of row & column driver low
	MIOS_DOUT_SRSet(0, 0);
	MIOS_DOUT_SRSet(1, 0);
	MIOS_DOUT_SRSet(2, 0);
	MIOS_DOUT_SRSet(3, 0);

	// hello sequence
	DoStartupSequence();
	
	
	// note on hello from core1
	MIOS_MIDI_BeginStream();
	MIOS_MIDI_TxBufferPut(0x90); // midi note type
	MIOS_MIDI_TxBufferPut(0x7E); // note 126
	MIOS_MIDI_TxBufferPut(0x7F); // velo 127
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

//////////////////////////////////////////////////////////////////////////////
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
// This function is called by MIOS when a MIDI byte has been received
/////////////////////////////////////////////////////////////////////////////
void MPROC_NotifyReceivedByte(unsigned char byte) __wparam
{
}

/////////////////////////////////////////////////////////////////////////////
//  This function is called by MIOS when a complete MIDI event has been received
/////////////////////////////////////////////////////////////////////////////
void MPROC_NotifyReceivedEvnt(unsigned char evnt0, unsigned char evnt1, unsigned char evnt2) __wparam
{
	if( (evnt0 & 0xf0) == 0x90)				// being sure that this is a note message
	{
		if(evnt1 >= 0x01 && evnt1 <= 0x40)	// being sure parsing a message for DOUT11 (matrix[] bounds are implicitely verified)
		{
			unsigned char noteIndex = evnt1;
			unsigned char value = evnt2;
			matrix[noteIndex - 0x01] = value;
		}
		else if(evnt1 == 0x7C)	// clear the matrix with only one midi message
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
// This function is called by MIOS when a MIDI event has not been completely
// received within 2 seconds
/////////////////////////////////////////////////////////////////////////////
void MPROC_NotifyTimeout(void) __wparam
{
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by SR_Service_Prepare, decompose color component and UP the right rgb pins
/////////////////////////////////////////////////////////////////////////////
void DisplayLED(unsigned int column, unsigned char color) __wparam
{
	color >>= 4;
	MIOS_DOUT_PinSet(column+8,		(unsigned char)(color & 0x01));  // RED
	color >>= 1;
	MIOS_DOUT_PinSet(column+8+8,	(unsigned char)(color & 0x01)); // BLUE
	color >>= 1;
	MIOS_DOUT_PinSet(column+8+8+8,	(unsigned char)(color & 0x01));	// GREEN
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS before the shift register are loaded
/////////////////////////////////////////////////////////////////////////////
void SR_Service_Prepare(void) __wparam
{
	static unsigned int row;
	static unsigned int lastrow;
	unsigned int x;

	row = ++row & 0x07;						// row cycling
	MIOS_DOUT_PinSet0(lastrow);				// lastrow OFF
	MIOS_DOUT_PinSet1(row);					// current row ON

	for (x = 0; x < 8; x++)
	{
		DisplayLED(x , matrix[x+row*8]);
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
	MIOS_MIDI_BeginStream(); // midilink encapsulation header
	MIOS_MIDI_TxBufferPut((unsigned char) button_event_map[pin][0]); // first byte from table = CHANNEL
	MIOS_MIDI_TxBufferPut((unsigned char) button_event_map[pin][1]); // second byte from table = NOTE
	MIOS_MIDI_TxBufferPut(pin_value ? 0x00 : 0x7f);
	MIOS_MIDI_EndStream();	// midilink encapsulation tail
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
	MIOS_MIDI_BeginStream(); // midilink encapsulation header
	MIOS_MIDI_TxBufferPut((unsigned char)pot_event_map[pin][0]); // first value from table
	MIOS_MIDI_TxBufferPut((unsigned char)pot_event_map[pin][1]); // second value from table
	MIOS_MIDI_TxBufferPut(MIOS_AIN_Pin7bitGet(pin)); // 7bit pot value
	MIOS_MIDI_EndStream();	// midilink encapsulation tail
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
	unsigned int waitTime;
	waitTime = 10;

	for (index = 0; index < 64; index++)
	{
		matrix[index] = _COLOR_RED ;
		MIOS_Delay(waitTime);
	}
}
