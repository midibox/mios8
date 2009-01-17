// $Id: main.c 539 2008-11-23 23:29:22Z stryd_one $
/*
 * FRAM Module Test application
 *
 * ==========================================================================
 *
 *  Copyright (C) Matthias  Mächler (maechler@mm-computing.ch / thismaechler@gmx.ch)
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
#include <fram.h>

//address range: 0x7FFF for FM24C512 and FM24C256, 0x1FFF for the FM24C64
#define address_range 0x7FFF 
//device address count. each FM24C512 uses two addresses (two memory blocks)
#define num_devices 0x08
//abort test after count errors.
#define error_count_abort 0x01

const unsigned char test_values[0x100] = {
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
	
//0: start check; 1 show start message; 
//2: test buffer write/read; 3: test single byte write/read; 
//4: check subsequent buffer write; 5: check subsequent buffer read; 
//6: check subsequent single byte write; 7: check subsequent single byte read;
//8: test finished successfully; 9: test finished on errors; 10: used for error summary alternate screen
unsigned char phase;
unsigned int address; //address counter
unsigned char device_addr; //device address counter
unsigned char error_count; //error counter
unsigned char last_error_op; //1: begin write; 2: write; 3 begin read; 4: read/compare
unsigned char last_error_phase;
unsigned char fram_buffer[0x100]; //data buffer


/////////////////////////////////////////////////////////////////////////////
// Application Prototypes
/////////////////////////////////////////////////////////////////////////////

void fram_init_buffer(void) __wparam;
unsigned char fram_check_buffer(void) __wparam;
void fram_clear_buffer(void) __wparam;
void print_hex4(unsigned int) __wparam;

/////////////////////////////////////////////////////////////////////////////
// Application Functions
/////////////////////////////////////////////////////////////////////////////

void fram_init_buffer(void) __wparam{
	unsigned int i;
	for(i=0;i<0x100;i++)
		fram_buffer[i] = test_values[i];
	}

unsigned char fram_check_buffer(void) __wparam{
	unsigned int i;
	for(i=0;i<0x100;i++)
		if(fram_buffer[i] != test_values[i])
			return 0x00;
	return 0x01;
	}

void fram_clear_buffer(void) __wparam{
	unsigned int i;
	for(i=0;i<0x100;i++)
		fram_buffer[i] = 0x00;
	}

void print_hex4(unsigned int value) __wparam{
	MIOS_LCD_PrintCString("0x");
	MIOS_LCD_PrintHex2((unsigned char)(value >> 8));
	MIOS_LCD_PrintHex2((unsigned char)value);	
	}



/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS after startup to initialize the 
// application
/////////////////////////////////////////////////////////////////////////////
void Init(void) __wparam{
	phase = 0x00;
	FRAM_REG = 0x00; //indicates that FRAM_Begin has not been called yet
	MIOS_SRIO_NumberSet(1);
	MIOS_SRIO_UpdateFrqSet(10);
	}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS in the mainloop when nothing else is to do
/////////////////////////////////////////////////////////////////////////////
void Tick(void) __wparam{
	unsigned char mode,err=0;
	if (phase < 0x02 || phase > 0x07)
		return;
	//If this is the first FRAM_Begin, or a FRAM_End was called last, do FRAM_Begin
	if(!FRAM_REG){
		mode = (phase & 0x01 && phase > 0x03) ? FRAM_Mode_Read : FRAM_Mode_Write;
		if(!FRAM_Begin(device_addr,address,mode))
			err = (mode==FRAM_Mode_Read) ? 0x03 : 0x01;
		}
	if(!err)
		switch(phase){
			case 0x02://Test Buffer W/R
				fram_init_buffer();
				if(!FRAM_WriteBuf(0x00,&fram_buffer[0])){
					err = 0x02;
					break;
					}
				FRAM_End();
				fram_clear_buffer();
				if(!FRAM_Begin(device_addr,address,FRAM_Mode_Read)){
					err = 0x03;
					break;					
					}
				FRAM_ReadBuf(0x00,&fram_buffer[0]);
				if(!fram_check_buffer())
					err = 0x04;
				break;
			case 0x03://Test Byte W/R
				if(!FRAM_WriteByte(test_values[(unsigned char)address])){
					err = 0x02;
					break;
					}
				FRAM_End();
				if(!FRAM_Begin(device_addr,address,FRAM_Mode_Read)){
					err = 0x03;
					break;
					}
				if(FRAM_ReadByte()!=test_values[(unsigned char)address])
					err = 0x04;
				break;
			case 0x04://Test subsequent Buffer write
				if(!FRAM_WriteBuf(0x00,&fram_buffer[0]))
					err = 0x02;
				break;
			case 0x05://Test subsequent Buffer read
				fram_clear_buffer();
				FRAM_ReadBuf(0x00,&fram_buffer[0]);
				if(!fram_check_buffer())
					err = 0x04;
				break;
			case 0x06://Test subsequent Byte write
				if(!FRAM_WriteByte(test_values[(unsigned char)address]))
					err = 0x02;
				break;
			case 0x07://Test subsequent Byte read
				if(FRAM_ReadByte()!=test_values[(unsigned char)address])
					err = 0x04;
				break;
			}
	//on error, set error status vars and End the R/W session. 
	//Abort if error_count_abort is reached
	if(err){
		last_error_op = err;
		last_error_phase = phase;
		error_count++;
		FRAM_End();
		//error_count_abort is reached, switch to abort-on-error phase
		if(error_count == error_count_abort)
			phase = 0x09;
		}
	//on success, End the session for W/R phases, increment address / device_addr
	//and switch phase accordingly
	else{
		//step to next address
		address += (phase == 0x03 || phase > 0x05) ? 0x01 : 0x100;
		//address range reached, next device
		if(address > address_range){
			FRAM_End();
			address = 0x0000;
			//all devices tested, next test phase
			if(device_addr == num_devices-1){
				phase++;
				device_addr = 0x00;
				if(phase == 0x04)
					fram_init_buffer();
				}
			else
				device_addr++;//switch to next device
			}
		else if(phase == 0x02 || phase == 0x03)
			FRAM_End();
		}
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
	MIOS_LCD_Clear();	
	}

/////////////////////////////////////////////////////////////////////////////
//  This function is called in the mainloop when no temporary message is shown
//  on screen. Print the realtime messages here
/////////////////////////////////////////////////////////////////////////////
void DISPLAY_Tick(void) __wparam{
	MIOS_LCD_Clear();
	MIOS_LCD_CursorSet(0x00);
	switch(phase){
		case 0x00://show initial message
			MIOS_LCD_PrintCString("FRAM Test Start");
			MIOS_LCD_CursorSet(0x40);
			MIOS_LCD_PrintCString("DC: 0x");
			MIOS_LCD_PrintHex2(num_devices);
			MIOS_LCD_PrintCString(" AR: ");
			print_hex4(address_range);
			MIOS_LCD_MessageStart(255);
			phase = 0x01;
			break;
		case 0x01://start check
			address = 0; //address counter
			device_addr = 0; //device address counter
			error_count = 0; //error counter
			phase = 0x02;
			break;
		case 0x02:
		case 0x03:
		case 0x04:
		case 0x05:
		case 0x06:
		case 0x07:
			MIOS_LCD_PrintCString("Test ");
			switch(phase){
				case 0x02:MIOS_LCD_PrintCString("Buffer W/R");break;
				case 0x03:MIOS_LCD_PrintCString("Byte W/R");break;
				case 0x04:MIOS_LCD_PrintCString("Subseq Buffer W");break;
				case 0x05:MIOS_LCD_PrintCString("Subseq Buffer R");break;
				case 0x06:MIOS_LCD_PrintCString("Subseq Byte W");break;
				case 0x07:MIOS_LCD_PrintCString("Subseq Byte R");break;
				}
			MIOS_LCD_CursorSet(0x40);
			MIOS_LCD_PrintCString("0x");
			MIOS_LCD_PrintHex2(device_addr);
			MIOS_LCD_PrintCString("-");
			print_hex4(address);
			MIOS_LCD_PrintCString(" E: 0x");
			MIOS_LCD_PrintHex2(error_count);
			MIOS_LCD_MessageStart(16);
			break;
		case 0x08:
			MIOS_LCD_PrintCString("FRAM Test finished");
			MIOS_LCD_CursorSet(0x40);
			MIOS_LCD_PrintCString("successfully!");
			MIOS_LCD_MessageStart(0xFF);		
			break;
		case 0x09:	
			MIOS_LCD_PrintCString("Abort");
			#if error_count_abort > 0x01
			MIOS_LCD_PrintCString(", 0x");
			MIOS_LCD_PrintHex2(error_count);
			MIOS_LCD_PrintCString(" Errors");
			#else	
			MIOS_LCD_PrintCString(" on Error");
			#endif
			MIOS_LCD_CursorSet(0x40);
			switch(last_error_phase){
				case 0x02:MIOS_LCD_PrintCString("Buffer WR:");break;
				case 0x03:MIOS_LCD_PrintCString("Byte WR:");break;
				case 0x04:MIOS_LCD_PrintCString("Subseq Buffer W:");break;
				case 0x05:MIOS_LCD_PrintCString("Subseq Buffer R:");break;
				case 0x06:MIOS_LCD_PrintCString("Subseq Byte W:");break;
				case 0x07:MIOS_LCD_PrintCString("Subseq Byte R:");break;
				}
			switch(last_error_op){
				case 0x01:MIOS_LCD_PrintCString("BW");break;
				case 0x02:MIOS_LCD_PrintCString("W");break;
				case 0x03:MIOS_LCD_PrintCString("BR");break;
				case 0x04:MIOS_LCD_PrintCString("RC");break;
				}
			phase = 0x0A;
			MIOS_LCD_MessageStart(0xFF);
			break;
		case 0x0A:
			MIOS_LCD_PrintCString("Device: 0x");
			MIOS_LCD_PrintHex2(device_addr);
			MIOS_LCD_CursorSet(0x40);
			MIOS_LCD_PrintCString("Address: ");
			print_hex4(address);
			phase = 0x09;
			MIOS_LCD_MessageStart(0xFF);
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
	if(pin_value == 0 && pin == 0){
		phase = 0x00;
		MIOS_LCD_MessageStop();
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
