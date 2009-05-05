// $Id: main.c 378 2008-07-06 20:38:32Z tk $
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

#define max_first_sector 0xFF
#define max_subseq_check_errors  0x10
#define check_step 0xF2
#define initial_sector_inc 0x1000

unsigned char phase = 0;//0x00 init;0x01 determine first rw sector;0x02 determine last rw sector;0x03 deep check;0xff check done;
unsigned long sector;
unsigned long sector_inc;//current sector increment

unsigned long bad_sector_count;
unsigned long first_sector_rw;
unsigned long last_sector_rw;
unsigned char last_error_code;
char last_error_op;
unsigned int subseq_check_errors;


void init_sdcard_buffer(void) __wparam;
unsigned char check_sdcard_buffer(void) __wparam;
void clear_sdcard_buffer(void) __wparam;
unsigned char check_sector_rw(void) __wparam;
void print_long_hex(unsigned long) __wparam;


const unsigned char test_p0[0x100] = {
	0x83,0x78,0x78,0x78,0xfa,0x55,0x83,0x78,0x78,0x78,0xfa,0x55,0x83,0x78,0x78,0x78,
	0x78,0x78,0x78,0xfa,0x55,0x83,0x78,0x78,0x78,0xfa,0x55,0x83,0x78,0x78,0x78,0xfa,
	0x78,0x78,0x78,0xfa,0x55,0x83,0x78,0x78,0x78,0x78,0x78,0x78,0xfa,0xfa,0x55,0x83,
	0x78,0xfa,0x55,0x83,0x78,0x78,0x78,0xfa,0x55,0x83,0x78,0x78,0x78,0x78,0x78,0xfa,
	0x78,0x78,0x78,0xfa,0x55,0x83,0x78,0x78,0x78,0xfa,0x55,0x83,0x78,0x78,0x78,0xfa,
	0x83,0x78,0x78,0x78,0xfa,0x55,0x83,0x78,0x78,0x78,0xfa,0x55,0x83,0x78,0x78,0x78,
	0x78,0x78,0x78,0xfa,0x55,0x83,0x78,0x78,0x78,0x78,0x78,0x78,0xfa,0xfa,0x55,0x83,
	0x78,0xfa,0x55,0x83,0x78,0x78,0x78,0xfa,0x55,0x83,0x78,0x78,0x78,0x78,0x78,0xfa,
	0x83,0x78,0x78,0x78,0xfa,0x55,0x83,0x78,0x78,0x78,0xfa,0x55,0x83,0x7ac,0x78,0x78,
	0x78,0xfa,0x55,0x83,0x78,0x78,0x78,0xfa,0x55,0x83,0x78,0x78,0x78,0x78,0x78,0xfa,
	0x78,0x78,0x78,0xfa,0x55,0x83,0x73,0x78,0x78,0xfa,0x55,0x83,0x78,0x78,0x78,0xfa,
	0x78,0x78,0x78,0xfa,0x55,0x83,0x78,0x78,0x78,0x78,0x78,0x78,0xfa,0xfa,0x55,0x83,
	0x89,0x78,0x78,0x78,0xfa,0x55,0x83,0x78,0x78,0x78,0xfa,0x55,0x83,0x78,0x78,0x78,
	0x78,0x78,0x78,0xfa,0x55,0x83,0x78,0x78,0x78,0xfa,0x55,0x83,0x78,0x78,0x78,0xfa,
	0x78,0x78,0x78,0xfa,0x55,0x83,0x78,0x78,0x78,0x78,0x78,0x78,0xfa,0xfa,0x55,0x70,
	0x78,0xfa,0x55,0x83,0x78,0x78,0x78,0xfa,0x55,0x83,0x78,0x78,0x78,0x78,0x78,0xfa
	};

const unsigned char test_p1[0x100] = {
	0x83,0x78,0x78,0x78,0xfa,0x55,0x83,0x78,0x78,0x78,0xfa,0x55,0x83,0x78,0x78,0x78,
	0x78,0x78,0x78,0xfa,0x55,0x83,0x78,0x78,0x78,0xfa,0x55,0x83,0x78,0x78,0x78,0xfa,
	0x78,0x78,0x78,0xfa,0x55,0x83,0x78,0x78,0x78,0xfa,0x55,0x83,0x78,0x78,0x78,0xfa,
	0x78,0x78,0x78,0xfa,0x55,0x83,0x78,0x78,0x78,0x78,0x78,0x78,0xfa,0xfa,0x55,0x83,
	0x78,0xfa,0x55,0x83,0x78,0x78,0x78,0xfa,0x55,0x83,0x78,0x78,0x78,0x78,0x78,0xfa,
	0x83,0x78,0x78,0x78,0xfa,0x55,0xf9,0x78,0x78,0x78,0xfa,0x55,0x83,0x78,0x78,0x78,
	0x78,0xfa,0x55,0x83,0x78,0x78,0x78,0xfa,0x55,0x83,0x78,0x78,0x78,0x78,0x78,0xfa,
	0x78,0x78,0x78,0xfa,0x55,0x83,0x78,0x78,0x78,0x78,0x78,0x78,0xfa,0xfa,0x55,0x83,
	0x83,0x78,0x78,0x78,0xfa,0x55,0x83,0x78,0x78,0x78,0xfa,0x55,0x83,0x78,0x78,0x78,
	0x55,0x78,0xfa,0x83,0x78,0x78,0x78,0xfa,0x55,0x83,0x78,0x78,0x78,0x78,0x78,0xfa,
	0x99,0x78,0x78,0xfa,0x55,0x83,0xab,0x78,0x78,0xfa,0x55,0x83,0x78,0x78,0x78,0xfa,
	0x78,0x78,0x78,0xfa,0x55,0x83,0x78,0x78,0x78,0x78,0x78,0x78,0xfa,0xfa,0x55,0x83,
	0x83,0x78,0x78,0x78,0xfa,0x55,0x83,0x78,0x78,0x78,0xfa,0xc3,0x83,0x78,0x78,0x78,
	0x78,0x78,0x78,0xfa,0x55,0x83,0x78,0x78,0x78,0xfa,0x55,0x83,0x78,0x78,0x78,0xfa,
	0x78,0x78,0x78,0xfa,0x55,0x83,0x78,0x78,0x78,0x78,0x78,0x78,0xfa,0xfa,0x55,0x83,
	0x78,0xfa,0x55,0x83,0x78,0x78,0x78,0xfa,0x55,0x83,0x78,0x78,0x78,0x78,0x78,0xbb
	};


/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS after startup to initialize the 
// application
/////////////////////////////////////////////////////////////////////////////
void Init(void) __wparam{
	MIOS_SRIO_NumberSet(1);
	MIOS_SRIO_UpdateFrqSet(10);
	}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS in the mainloop when nothing else is to do
/////////////////////////////////////////////////////////////////////////////

void Tick(void) __wparam{
	unsigned char resp;
	switch(phase){
		case 0x02:
			sector_inc = initial_sector_inc;//current sector increment
			first_sector_rw = 0xffffffff;
			last_sector_rw = 0xffffffff;
			sector = 0;
			bad_sector_count = 0;
			last_error_code = 0;
			last_error_op = 0;
			subseq_check_errors = 0;
			MIOS_Delay(250);
			if(resp = SDCARD_Init()){
				last_error_op = 'i';
				last_error_code = resp;
				phase = 0xff;
				}
			else
				phase = 0x03;
			break;		
		case 0x03://determine first rw sector
			if(!check_sector_rw()){
				if(sector < max_first_sector)
					sector++;
				else
					phase = 0xff;
				}
			else{
				first_sector_rw=sector;
				last_sector_rw=sector;
				phase = 0x04;
				while(sector_inc > 0xffffffff - last_sector_rw)
					sector_inc >>= 1;
				sector = last_sector_rw + sector_inc;
				}
			break;
		case 0x04://determine last rw sector
			if(!check_sector_rw()){
				if(sector_inc == 1){
					phase = 0x05;
					sector = first_sector_rw;
					}
				else{
					sector_inc >>= 1;
					sector = last_sector_rw + sector_inc;
					}
				}
			else{
				last_sector_rw = sector;
				while(sector_inc > 0xffffffff - last_sector_rw)
					sector_inc >>= 1;
				sector += sector_inc;
				}
			break;
		case 0x05://deep check all sectors
			if(sector > last_sector_rw){
				phase = 0xff;
				subseq_check_errors = 0;
				}
			else{
				if(!check_sector_rw()){
					bad_sector_count++;
					subseq_check_errors++;
					if(subseq_check_errors > max_subseq_check_errors){
						phase = 0xff;
						}
					}
				else
					subseq_check_errors=0;
				sector += check_step;
				}
			break;
		}
	}

void clear_sdcard_buffer(void) __wparam{
	unsigned int i;
	for(i=0;i<0x100;i++){
		sdcard_buffer_p0[i] = 0x00;
		sdcard_buffer_p1[i] = 0x00;
		}	
	}

void init_sdcard_buffer(void) __wparam{
	unsigned int i;
	for(i=0;i<0x100;i++){
		sdcard_buffer_p0[i] = test_p0[i];
		sdcard_buffer_p1[i] = test_p1[i];
		}
	}
	
unsigned char check_sdcard_buffer(void) __wparam{
	unsigned int i;
	for(i=0;i<0x100;i++){
		if( (sdcard_buffer_p0[i] != test_p0[i]) || (sdcard_buffer_p1[i] != test_p1[i]) )
			return 0x00;
		}
	return 0x01;
	}
	
unsigned char check_sector_rw(void) __wparam{
	unsigned char resp;
	init_sdcard_buffer();
	if(resp = SDCARD_SectorWrite(sector)){
		last_error_op = 'w';
		last_error_code = resp;
		SDCARD_Init();
		return 0;
		}
	clear_sdcard_buffer();
	if(resp = SDCARD_SectorRead(sector)){
		last_error_op = 'r';
		last_error_code = resp;
		SDCARD_Init();
		return 0;
		}
	if(!check_sdcard_buffer()){
		last_error_op = 'r';
		last_error_code = 0x00;
		return 0;
		}	
	return 1;
	}

/////////////////////////////////////////////////////////////////////////////
// This function is periodically called by MIOS. The frequency has to be
// initialized with MIOS_Timer_Set
/////////////////////////////////////////////////////////////////////////////
void Timer(void) __wparam{
	}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS when the display content should be 
// initialized. Thats the case during startup and after a temporary message
// has been printed on the screen
/////////////////////////////////////////////////////////////////////////////
void DISPLAY_Init(void) __wparam{
	}

/////////////////////////////////////////////////////////////////////////////
//  This function is called in the mainloop when no temporary message is shown
//  on screen. Print the realtime messages here
/////////////////////////////////////////////////////////////////////////////

void print_long_hex(unsigned long value) __wparam{
	MIOS_LCD_PrintCString("0x");
	if(value & 0xff000000)
		MIOS_LCD_PrintHex2((unsigned char)(value >> 24));
	if(value & 0xffff0000)
		MIOS_LCD_PrintHex2((unsigned char)(value >> 16));
	if(value & 0xffffff00)
		MIOS_LCD_PrintHex2((unsigned char)(value >> 8));
		MIOS_LCD_PrintHex2((unsigned char)value);
	}

void DISPLAY_Tick(void) __wparam{
	MIOS_LCD_Clear();
	MIOS_LCD_CursorSet(0x00);
	switch(phase){
		case 0x00:
			MIOS_LCD_PrintCString("SD CARD CHECK");
			MIOS_LCD_CursorSet(0x40);
			MIOS_LCD_PrintCString("INITIALIZAION..");
			MIOS_LCD_MessageStart(128);
			phase = 0x01;
			break;
		case 0x01:
			phase = 0x02;
			break;
		case 0x03:
			MIOS_LCD_PrintCString("FS ");
			print_long_hex(sector);
			MIOS_LCD_MessageStart(16);
			break;
		case 0x04:
			MIOS_LCD_PrintCString("LS ");
			print_long_hex(sector);
			MIOS_LCD_MessageStart(16);
			break;
		case 0x05:
			print_long_hex(sector);
			MIOS_LCD_PrintCString(" BAD ");
			print_long_hex(bad_sector_count);
			MIOS_LCD_CursorSet(0x40);
			print_long_hex(last_sector_rw);
			MIOS_LCD_MessageStart(16);
			break;
		case 0xff:
			if(first_sector_rw != 0xffffffff && !subseq_check_errors){
				print_long_hex(first_sector_rw);
				MIOS_LCD_PrintCString(" BAD ");
				print_long_hex(bad_sector_count);
				MIOS_LCD_CursorSet(0x40);
				print_long_hex(last_sector_rw);
				}
			else{
				MIOS_LCD_PrintCString("ABORT ON ");
				MIOS_LCD_PrintCString(last_error_op == 'r' ? " READ " : (last_error_op == 'w' ? " WRITE " : " INIT "));
				MIOS_LCD_CursorSet(0x40);
				MIOS_LCD_PrintCString("ERROR CODE 0x");
				MIOS_LCD_PrintHex2(last_error_code);
				}
			MIOS_LCD_MessageStart(255);
			break;
		}
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
void DIN_NotifyToggle(unsigned char pin, unsigned char pin_value) __wparam{
	if(pin_value == 0 && pin == 0)
		phase = 0x00;
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

