/*
 *  ACSensorizer.c
 *  ACSensorizer
 *
 */


#include "ACSensorizer.h"
#include "ACMidi.h"

#include <cmios.h>
#include <pic18fregs.h>
#include <iic_eeprom8.h>


#if SENSORIZER_HARM_ENA
	#include "ACHarmonizer.h"
#endif

#if SENSORIZER_SYNC_ENA
	#include "ACSyncronizer.h"
#endif

#ifdef _DEBUG_C
	#include "ACSim_console.h"	// for DEBUG_SYSREALTIME_VERBOSE
#endif


// ***** GLOBALS *****

// sensor config
volatile sensorizer_t		sensorizer;				// sensorizer configuration
volatile sensor_t			sensor[SENSOR_NUM];		// sensor configurations

// temp storages
volatile unsigned int		currentAIN_value[SENSOR_NUM];		// holds current AIN_value (for synced mode)
unsigned char				lastAIN_value[SENSOR_NUM];			// last sensed  7-bit value
unsigned int				lastAIN_10bitValue[SENSOR_NUM];		// last sensed 10-bit value

// midi prefs
#if SENSORIZER_CC_ENA
volatile unsigned char		sCH[SENSOR_NUM];		// send on Channel (0 to 15)
volatile unsigned char		sCC[SENSOR_NUM];		// send CC#
#endif
// sense slowdown
volatile unsigned char		slowdown[SENSOR_NUM];	// sense only every nth event (0 to 255)
// sense range
volatile unsigned int		sMin[SENSOR_NUM];		// 10-bit minimum sense (ignore below; => 7bit:   0)
volatile unsigned int		sMax[SENSOR_NUM];		// 10-bit maximum sense (ignore above; => 7bit: 127)
// expand signal
volatile unsigned char		sFactor[SENSOR_NUM];	// calibrated sensor factor: (sMax - sMin) / 127
													//   if somehow possible use 1, 2, 4, 8 or 16
													//   to use fastest bit-shifting algorithm
													//   otherwise a complex multi-cycle division occurs
volatile unsigned char		sFrom[SENSOR_NUM];		// scale to target min value (e.g. 0)
volatile unsigned char		sTo[SENSOR_NUM];		// scale to target max value (e.g. 127)


#if SENSORIZER_BANKSTICK_ENA
// bankstick patch
volatile unsigned char		bank;					// current bank (0..7)
volatile unsigned char		patch;					// current patch (0..127)
volatile unsigned char		patchName[LCD_PATCHNAME_CHAR_NUM] = { "Default" };
//volatile unsigned char	patchName[LCD_PATCHNAME_CHAR_NUM] = { "0701010" };
#endif /* SENSORIZER_BANKSTICK_ENA */

#if SENSORIZER_SYNC_ENA
// syncronizer module
volatile unsigned char		sync_q[SENSOR_NUM];		// quantize starting point
const unsigned char			sync_q_values[SYNC_Q_MAX] = { 0, 1, 2, 3, 4, 6, 8,12,16,24,32,48 };
#endif



// ***** LOCALS *****

unsigned char		slowdown_counter[SENSOR_NUM];
signed int			inverted_value;

#if SENSORIZER_SYNC_ENA
//					sync_q_values[SYNC_Q_MAX]	   = { 0, 1, 2, 3, 4, 6, 8,12,16,24,32,48 };
const unsigned char	sync_q_tickvalues[SYNC_Q_MAX]  = { 0,95,47,31,23,15,11, 7, 5, 3, 2, 1 };
unsigned char		sync_q_decrementer[SENSOR_NUM];	// countdown for clock-ticks
unsigned char		sync_q_lastNote[SENSOR_NUM];	// send note off before sending new note on
#endif




/////////////////////////////////////////////////////////////////////////////
//	This function initializes the sensors
/////////////////////////////////////////////////////////////////////////////

void ACSensorizer_Init(void) __wparam
{
	unsigned char c;
	// init AIN Core Settings
	MIOS_AIN_NumberSet(AIN_NUM);
#if AIN_MUX
	MIOS_AIN_Muxed();
#else
	MIOS_AIN_UnMuxed();
#endif
	MIOS_AIN_DeadbandSet(7);
	
	// init & setup variables
	sensorizer.ALL				= 0x0;	// all bits off
	sensorizer.pedalUp			= TRUE;
	for(c=0; c<SENSOR_NUM; c++) {
		// setup globals
		sensor[c].ALL			= 0x00;	// init all bits to 0
#if (SENSOR_NUM > 4)
		if(c<4) {
			sensor[c].enabled	= TRUE;	 // enable  sensors 1-4 by default
#if SENSORIZER_INTERFACE_DOUT
			MIOS_DOUT_PinSet(c,1);
#endif /* SENSORIZER_INTERFACE_DOUT */
			sensor[c].releaseDetect = TRUE;
		}
#else
		sensor[c].enabled = TRUE;
		sensor[c].releaseDetect = TRUE;
#endif /* (SENSOR_NUM > 4) */
		
#if SENSORIZER_CC_ENA
		sCH[c]					= 0;
		sCC[c]					= 20 + c;
#endif
		slowdown[c]				= 0;	// also see slowdown_counter
		sMin[c]					= 64;	//   8:	changed by autoSense
		sMax[c]					= 880;	// 110:	changed by autoSense
		sFactor[c]				= 4;	//   4: changed by autoSense
		sFrom[c]				= 0;
		sTo[c]					= 127;
		// setup locals
		lastAIN_10bitValue[c]	= 0;
		slowdown_counter[c]		= 0;
#if SENSORIZER_SYNC_ENA
		// quantize setup
		sync_q[c]				= 8;	// defaults to 1/16
#endif
	}
#if SENSORIZER_BANKSTICK_ENA
	// bankstick patch
	bank = 0;
	patch = 0;
#endif /* SENSORIZER_BANKSTICK_ENA */	
}




#if SENSORIZER_BANKSTICK_ENA
/////////////////////////////////////////////////////////////////////////////
//	This function writes a patch to Bankstick
//	set bank (0-7) and patch (0..31) before calling!
//	see readme.txt for detailed explanation & address tables
/////////////////////////////////////////////////////////////////////////////

unsigned char ACSensorizer_Patch_Write(void) 
{
	unsigned char c;
	unsigned char page;
	unsigned int  page_addr;
	unsigned char buffer[64];
	
#if SENSOR_NUM > 8
	// security dropout to prevent accessing wrong bankstick range
	MIOS_LCD_Clear();
	MIOS_LCD_CursorSet(0x00);
	MIOS_LCD_PrintCString("TOO MUCH SENSORS");
	MIOS_LCD_CursorSet(0x40);
	MIOS_LCD_PrintCString("8 SENSORS MAX !!");
	MIOS_LCD_MessageStart(255);
	return;
#endif
	
	// check bank#
	if(bank > 14) { return 1; }
	MIOS_BANKSTICK_CtrlSet(bank >> 1);					// select BS
	
	// ++ page 1 ++
	page = 0;
	page_addr = ((unsigned int)patch << 7) + ((unsigned int)page << 6);
	// add second bank to page_addr
	switch(bank) {
		case 1: case 3: case 5: case 7: case 9: case 11: case 13: case 15:
			page_addr = page_addr + 16256;
			break;
	}
	// version
	buffer[0] = (unsigned char)SENSORIZER_BS_VERSION;	// version
	//	buffer[1..9] = "patchname";						// patchName
	for(c=1;c<10;c++) {
		buffer[c] = patchName[(c-1)];
	}
	//	buffer[10-15] = #reserved;						// reserved
#if SENSORIZER_SYNC_ENA
	buffer[16] = bpm;									// bpm
#endif
//	buffer[17-21] = #reserved;							// reserved
#if SENSORIZER_HARM_ENA
	buffer[22] = harmonizer_base;						// baseNote
	buffer[23] = harmonizer_scale;						// scale
#endif
	for(c=0;c<SENSOR_NUM;c++) {
		buffer[24+c] = sensor[c].ALL;					// sensor
#if SENSORIZER_CC_ENA
		buffer[32+c] = sCH[c];							// sCH
		buffer[40+c] = sCC[c];							// sCC
#endif
#if SENSORIZER_SYNC_ENA
		buffer[48+c] = sync_q[c];						// sync_q
//		buffer[56+c] = sync_length[c];					// <not used>
#endif
	}
	// write page1 of 64 bytes to BankStick memory
	if( MIOS_BANKSTICK_WritePage(page_addr, buffer) ) { return 1; }	// write error!
		
	
	// ++ page 2 ++
	page = 1;
	page_addr = ((unsigned int)patch << 7) + ((unsigned int)page << 6);
	// add second bank to page_addr
	switch(bank) {
		case 1: case 3: case 5: case 7: case 9: case 11: case 13:
			page_addr = page_addr + 16256;
			break;
	}
	for(c=0;c<SENSOR_NUM;c++) {
		buffer[c] = slowdown[c];						// slowdown
		buffer[8+c] = sFactor[c];						// sFactor
		buffer[16+c] = (unsigned char)(sMin[c] >> 8);	// sMin.MSB
		buffer[24+c] = (unsigned char)(sMin[c] & 0xFF);	// sMin.LSB
		buffer[32+c] = (unsigned char)(sMax[c] >> 8);	// sMax.MSB
		buffer[40+c] = (unsigned char)(sMax[c] & 0xFF); // sMax.LSB
		buffer[48+c] = sFrom[c];						// sFrom
		buffer[56+c] = sTo[c];							// sTo
	}
	// write page2 of 64 bytes to BankStick memory
	if( MIOS_BANKSTICK_WritePage(page_addr, buffer) ) { return 1; }	// write error!

	// save last bank/patch to EEPROM
	MIOS_EEPROM_Write(0x00,bank);
	MIOS_EEPROM_Write(0x01,patch);
	
	return 0;	// success!
}


/////////////////////////////////////////////////////////////////////////////
//	This function reads a patch from Bankstick
//	set bank (0-7) and patch (0..31) before calling!
//	see readme.txt for detailed explanation & address tables
/////////////////////////////////////////////////////////////////////////////


unsigned char ACSensorizer_Patch_Read(void)
{
	unsigned char c;
	unsigned char page;
	unsigned int  page_addr;
	unsigned char buffer[64];
	
#if SENSOR_NUM > 8
	// security dropout to prevent accessing wrong bankstick range
	MIOS_LCD_Clear();
	MIOS_LCD_CursorSet(0x00);
	MIOS_LCD_PrintCString("TOO MUCH SENSORS");
	MIOS_LCD_CursorSet(0x40);
	MIOS_LCD_PrintCString("8 SENSORS MAX !!");
	MIOS_LCD_MessageStart(255);
	return;
#endif
	
	// check bank#
	if(bank > 14) { return 1; }
	MIOS_BANKSTICK_CtrlSet(bank >> 1);					// select BS
	
	// ++ page 1 ++
	page = 0;
	page_addr = ((unsigned int)patch << 7) + ((unsigned int)page << 6);
	// add second bank to page_addr
		switch(bank) {
			case 1: case 3: case 5: case 7: case 9: case 11: case 13:
				page_addr = page_addr + 16256;
				break;
		}
	// read page1 of 64 bytes from BankStick memory
	MIOS_BANKSTICK_ReadPage(page_addr, buffer);
	if( ! MIOS_BOX_STAT.BS_AVAILABLE ) { return 1; }	// error!
	// version check (I can read all versions from 0x02 to now)
	if((buffer[0] < 0x02) || (buffer[0] > SENSORIZER_BS_VERSION)) { return 1; }	// error!
	//	buffer[1..9] = "patchname";						// patchName
	for(c=1;c<10;c++) {
		if(buffer[c] == 0) { buffer[c] = LCD_PATCHNAME_CHAR_START; }
		patchName[(c-1)] = buffer[c];
	}
	//	buffer[10..15] = #reserved;						// patchName
#if SENSORIZER_SYNC_ENA
	bpm = buffer[16];									// BPM
	if(bpm < 48) { bpm = 120; }							// maintain compatibility to older patches
#endif
	//	buffer[17-21] = #reserved;						// reserved
#if SENSORIZER_HARM_ENA
	harmonizer_base = buffer[22];						// base note
	harmonizer_scale = buffer[23];						// scale
#endif
	for(c=0;c<SENSOR_NUM;c++) {
		sensor[c].ALL	= buffer[24+c];					// sensor
#if SENSORIZER_INTERFACE_DOUT
		if(sensor[c].enabled) {
			MIOS_DOUT_PinSet(c,1);
		} else {
			MIOS_DOUT_PinSet(c,0);
		}
#endif /* SENSORIZER_INTERFACE_DOUT */
#if SENSORIZER_CC_ENA
		sCH[c]			= buffer[32+c];					// sCH
		sCC[c]			= buffer[40+c];					// sCC
#endif
#if SENSORIZER_SYNC_ENA
		if(buffer[48+c] < SYNC_Q_MAX) {
			sync_q[c]	= buffer[48+c];					// sync_q
		}
	//	sync_length[c]	= buffer[56+c];					// <not used>
#endif
	}
	
	// ++ page 2 ++
	page = 1;
	page_addr = ((unsigned int)patch << 7) + ((unsigned int)page << 6);
	// add second bank to page_addr
	switch(bank) {
		case 1: case 3: case 5: case 7: case 9: case 11: case 13:
			page_addr = page_addr + 16256;
			break;
	}
	// read page2 of 64 bytes from BankStick memory
	MIOS_BANKSTICK_ReadPage(page_addr, buffer);
	if( ! MIOS_BOX_STAT.BS_AVAILABLE ) {	return 1;	}	// error!
	for(c=0;c<SENSOR_NUM;c++) {
		slowdown[c]		= buffer[   c];					// slowdown
		sFactor[c]		= buffer[ 8+c];					// sFactor
		sMin[c]			= (unsigned int)(buffer[16+c] << 8) + buffer[24+c];	// sMin
		sMax[c]			= (unsigned int)(buffer[32+c] << 8) + buffer[40+c];	// sMax
		sFrom[c]		= buffer[48+c];					// sFrom
		sTo[c]			= buffer[56+c];					// sTo
	}

#if SENSORIZER_SYNC_ENA
	// set new tempo
	ACSYNC_BPMSet(bpm);
	// reset meter and send START signal
	ACSYNC_DoRun();
#endif
	
	// save last bank/patch to EEPROM
	MIOS_EEPROM_Write(0x00,bank);
	MIOS_EEPROM_Write(0x01,patch);
	
	return 0;	// success!
}
#endif /* SENSORIZER_BANKSTICK_ENA */





/////////////////////////////////////////////////////////////////////////////
//	This function is called to calibrate the connected sensors
//	AUTOSENSE_MIN:	sMin[pin] stores the current value
//	AUTOSENSE_MAX:	sMax[pin] stores the current value
//  if SENSORIZER_AUTOMODE_PEDAL is 1, autosense is only possible while
//	the pedal (Button or rear external Pedal) is down!
/////////////////////////////////////////////////////////////////////////////

#if SENSORIZER_AUTOSENSE_ENA
void ACSensorizer_AutoSense(unsigned char pin, unsigned int pin_value) __wparam
{
	unsigned int range;
#if SENSORIZER_AUTOMODE_PEDAL
	// no autosense if pedal not down
	if(sensorizer.pedalUp) { return; }
#endif /* SENSORIZER_AUTOMODE_PEDAL */
	// reset factor
	sFactor[pin] = 1;
	// autosense
	switch(sensor[pin].autoSense) {
		case AUTOSENSE_OFF:	// 0x0:	no autosense
			// proceed calculating new factor...
			break;
		case AUTOSENSE_MIN:	// 0x1:	min
			sMin[pin] = pin_value;
			break;
		case AUTOSENSE_MAX:	// 0x2:	max
			sMax[pin] = pin_value;
			break;
	} 
	// get new factor
	if(sMin[pin] > sMax[pin]) { // buffer protection
		sMin[pin] = sMax[pin]; 
		sFactor[pin] = 1; 
		return;
	}
	range = sMax[pin] - sMin[pin];
	// optimize for bitshifting divisions
#if SENSORIZER_SENSEMODE
	// senseMode is "exact" (uses division)
	sFactor[pin] = (unsigned char)(ACMath_Divide(range,(unsigned int)127));
	if(sFactor[pin] == 0) { sFactor[pin] = 1; }
#else
	// senseMode is "efficient" (uses bitshiftings only)
	if(range < 128) {
		sFactor[pin] = 1;
	} else if(range < 256) {
		sFactor[pin] = 2;
	} else if(range < 512) {
		sFactor[pin] = 4;
	} else if(range < 1024) {
		sFactor[pin] = 8;
	} else {
		sFactor[pin] = 16;
	}
#endif /* SENSORIZER_SENSEMODE */
}
#endif /* SENSORIZER_AUTOSENSE_ENA */





/////////////////////////////////////////////////////////////////////////////
//	This function reads the 10-bit value from <pin> and sends a 7-bit inter-
//	polated value. The sensors must be calibrated (with autoSensing) to 
//	obtain good sensor measurements.
/////////////////////////////////////////////////////////////////////////////

unsigned char ACSensorizer_Sensorize(unsigned char pin, unsigned int pin_value) __wparam
{
	// temp values
	unsigned char	value_7bit = 0;
	unsigned int	value_10bit = 0;
	
	// abort if OFF (disabled)
	if(! sensor[pin].enabled) { return SRETURN_ABORT; }
	
	// abort if pedal enabled & pedal is not down (=1)
	if((sensor[pin].pedalMode >= PEDAL_FILTER) && (sensorizer.pedalUp)) { return SRETURN_ABORT; }
	
#ifndef _DEBUG_C
	// abort if slowdown active
	if(slowdown[pin]) {
		slowdown_counter[pin]++;
		if(slowdown_counter[pin] >= slowdown[pin]) { 
			slowdown_counter[pin] = 0;	// protect against overflow
		} else {
			return SRETURN_ABORT;  	// abort if slowdown does not match
		}
	}
#endif /* _DEBUG_C */
	
#if SENSORIZER_AUTOSENSE_ENA
	// check for autosense
	if(sensor[pin].autoSense > 0) {
		ACSensorizer_AutoSense(pin, pin_value);
		acapp.displayNeedsUpdate = TRUE;				// 0xFF won't be updated
		return SRETURN_AUTOSENSE;
	}
#endif /* SENSORIZER_AUTOSENSE_ENA */
	
	// abort if below sMin or above sMax
	if(pin_value < sMin[pin]) { 
		if((sensor[pin].releaseDetect) && (lastAIN_value[pin] != 0)) {
			lastAIN_value[pin] = 0;
			return 0;									// send 0 to mute sound on release
		} else {
			return SRETURN_RELEASEDETECT; 
		}
	}
	if(pin_value > sMax[pin]) { return SRETURN_ABORT; }
	
	// invert
	if(sensor[pin].invert) {
		inverted_value = pin_value - pin_value - pin_value;
		pin_value = inverted_value + 1023;
	}
	
	// expand
	value_10bit = pin_value - sMin[pin];
	switch(sFactor[pin]) {
		case  1:	value_7bit = (unsigned char) value_10bit;		break;
		case  2:	value_7bit = (unsigned char)(value_10bit >> 1);	break;
		case  4:	value_7bit = (unsigned char)(value_10bit >> 2);	break;
		case  8:	value_7bit = (unsigned char)(value_10bit >> 3);	break;
		case 16:	value_7bit = (unsigned char)(value_10bit >> 4);	break;
		default:
			value_7bit = (unsigned char)ACMath_Divide(value_10bit, (unsigned int)sFactor[pin]);
			break;
	}
	
	// scale
	value_7bit = ACMath_Scale(value_7bit, sFrom[pin], sTo[pin]);
	
	// restrict to 7 bit MIDI-values
	if(value_7bit < sFrom[pin]) { return SRETURN_ABORT; }
	if(value_7bit > sTo[pin])	{ return SRETURN_ABORT; }

	// don't send unchanged values
	if(value_7bit == lastAIN_value[pin]) { return SRETURN_ABORT; }
	// store last 10bit value
	lastAIN_value[pin] = value_7bit;
	lastAIN_10bitValue[pin] = value_10bit;
	// return success
	return value_7bit;
}



/////////////////////////////////////////////////////////////////////////////
//	This function is called from ACSyncronizer
//	This is an alternate method for the the AIN_Notify Code
/////////////////////////////////////////////////////////////////////////////

void ACSensorizer_ReceiveClock(void) {

#if DEBUG_SYSREALTIME_VERBOSE
	printf("\n**ACSYNC_Sensor_Decr: \t#0:%i \t#1:%i \t#2:%i \t#3:%i", sync_q_decrementer[0], sync_q_decrementer[1], sync_q_decrementer[2], sync_q_decrementer[3]);
#endif
	
#if SENSORIZER_SYNC_ENA
	unsigned char value;
	unsigned char pin;
	unsigned int  pin_value;
	
	for(pin=0;pin<SENSOR_NUM;pin++) {
		// reset decrementers if needed
		if((sensorizer.resetMeter) && (mclock_ctr_96 == 0)) {
			sync_q_decrementer[pin] = 0;
		}
		// check if it's time to fire action
		if( sync_q_decrementer[pin]-- == 0) {
			sync_q_decrementer[pin] = sync_q_tickvalues[(sync_q[pin])];	// reset decr
			// read sensor values
			pin_value = currentAIN_value[pin];
			// sensorize
			value = ACSensorizer_Sensorize(pin, pin_value);
			if(value < 0x80) {	// value contains 7bit_value
#if SENSORIZER_HARM_ENA
				if(sCC[pin]) {
					// send CC
					ACMidi_SendCC(sCH[pin], sCC[pin], value);
				} else {
					// send note off
					if(sync_q_lastNote[pin]) {
						ACMidi_SendEvent((MIDI_NOTE_OFF + sCH[pin]),sync_q_lastNote[pin],0);
						sync_q_lastNote[pin] = 0;
					}
					if(value) {
						// get harmonized note
						value = ACHarmonize(value);
						// send harmonized note
						ACMidi_SendNote(sCH[pin],value,SENSORIZED_NOTE_VELOCITY);
						// save last note
						sync_q_lastNote[pin] = value;
					}
				}
#else
				// send CC
				ACMidi_SendCC(sCH[pin], sCC[pin], value);
				// update LCD needed
				acapp.displayNeedsUpdate = TRUE;
#endif /* SENSORIZER_HARM_ENA */
			} else if(value == SRETURN_AUTOSENSE) {
				// show autosense state
#if SENSORIZER_AUTOMODE_PEDAL
				if(sensorizer.pedalUp) { return; }
#endif /* SENSORIZER_AUTOMODE_PEDAL */
				AC_DISPLAY_Message(LCD_MSG_AUTOSENSE);
			}			
		}
		
		// reset decrementers if needed
		if((sensorizer.resetMeter) && (mclock_ctr_96 == 0)) {
			sync_q_decrementer[pin] = sync_q_tickvalues[(sync_q[pin])];
			if(pin == (SENSOR_NUM-1)) {
				sensorizer.resetMeter = FALSE;
			}	
		}
	}

#endif /* SENSORIZER_SYNC_ENA */
}
