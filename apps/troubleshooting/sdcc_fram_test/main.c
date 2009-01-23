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
//16 x FM24C512 (multiplexed) equals 0x20 devices
#define num_devices 0x20
//abort test after count errors.
#define error_count_abort 0x01

//how much data is written / read in subsequent test (*256)
//this value should not exceed the address_range!
#define subseq_rw_chunks 0x20

//0: start check; 1 show start message; 
//2: test buffer write/read; 3: test single byte write/read; 
//4: check subsequent buffer write; 5: check subsequent buffer read; 
//6: check subsequent single byte write; 7: check subsequent single byte read;
//8: speed check write; 9: speed check read;
//10: test finished successfully; 11: test finished on errors; 
//other phases used for alternating screens at the end of the test
unsigned char phase;
unsigned int address; //address counter
unsigned char device_addr; //device address counter
unsigned char error_count; //error counter
unsigned char last_error_op; //1: begin write; 2: write; 3 begin read; 4: read/compare
unsigned char last_error_phase;
unsigned char fram_buffer[0x100]; //data buffer
unsigned int timecount,timecount_bw,timecount_br;//timecount in mS

//note: test values [0] and [1] are never used. [4] and [5] has to be equal,
//also [6] and [7] (else subsequent r/w test(s) will fail on read/compare! 
unsigned char test_value[0x08] = {0x00,0x00,0x99,0x1C,0x91,0x91,0x87,0x87};

unsigned char first_test_phase;

/////////////////////////////////////////////////////////////////////////////
// Application Prototypes
/////////////////////////////////////////////////////////////////////////////

void fram_init_buffer(void) __wparam;
unsigned char fram_check_buffer(void) __wparam;
void print_hex4(unsigned int) __wparam;

void fram_timecount_start(void) __wparam;
unsigned int fram_timecount_stop(void) __wparam;

/////////////////////////////////////////////////////////////////////////////
// Application Functions
/////////////////////////////////////////////////////////////////////////////

void fram_timecount_start(void) __wparam{
	timecount = 0;
	MIOS_TIMER_Init(0x01,50000);//10 mS
	}
	
unsigned int fram_timecount_stop(void) __wparam{
	MIOS_TIMER_Stop();
	return timecount;
	}

void fram_init_buffer(void) __wparam{
	unsigned int i;
	for(i=0;i<0x100;i++)
		fram_buffer[i] = ((test_value[phase]^device_addr) ^ (unsigned char)(address + i));
	}

unsigned char fram_check_buffer(void) __wparam{
	unsigned int i;
	for(i=1;i<0x100;i++)
		if(fram_buffer[i] != ((test_value[phase]^device_addr) ^ (unsigned char)(address + i)))
			return 0x00;
	return 0x01;
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
	first_test_phase = 0x02;
	MIOS_SRIO_NumberSet(1);
	MIOS_SRIO_UpdateFrqSet(10);
	}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS in the mainloop when nothing else is to do
/////////////////////////////////////////////////////////////////////////////
void Tick(void) __wparam{
	unsigned char b,mode,err;
	unsigned int i;
	if (phase < 0x02 || phase > 0x09)
		return;
	err = 0;
	//FRAM_Begin with read/write mode
	if(phase > 0x03){//for continous read/write test
		mode = (phase & 0x01) ? FRAM_Mode_Read : FRAM_Mode_Write;
		if(!FRAM_Begin(device_addr,address,mode))
			err = (mode==FRAM_Mode_Read) ? 0x03 : 0x01;
		}
	if(!err)
		switch(phase){
			case 0x02://Test Buffer W/R
				fram_init_buffer();
				if(!FRAM_WriteBuf(device_addr,address,0x00,fram_buffer)){
					err = (FRAM_ERROR == 0x10) ? 0x02 : 0x01;
					break;
					}
				fram_init_buffer();
				if(!FRAM_ReadBuf(device_addr,address,0x00,fram_buffer)){
					err = 0x03;
					break;
					}
				if(!fram_check_buffer())
					err = 0x04;
				else
					address += 0x100;
				break;
			case 0x03://Test Byte W/R
				for(i=0;i<0x100;i++){
					if(!FRAM_WriteByte(device_addr,address,(test_value[phase]^device_addr) ^ (unsigned char)address)){
						err = (FRAM_ERROR == 0x10) ? 0x02 : 0x01;
						break;
						}
					b = FRAM_ReadByte(device_addr,address); 
					if(FRAM_ERROR)
						err = 0x03;
					else if(b != ((test_value[phase]^device_addr) ^ (unsigned char)address))
						err = 0x04;
					else
						address++;
					}
				break;
			case 0x04://Test subsequent Buffer write
				for(i=0;i < subseq_rw_chunks;i++){
					fram_init_buffer();
					if(!FRAM_WriteBuf_Cont(0x00,fram_buffer)){
						err = 0x02;
						break;
						}
					}
				if(!err) address += (subseq_rw_chunks*0x100);
				break;
			case 0x05://Test subsequent Buffer read
				for(i=0;i < subseq_rw_chunks;i++){
					FRAM_ReadBuf_Cont(0x00,fram_buffer);
					if(!fram_check_buffer()){
						err = 0x04;
						break;
						}
					}
				if(!err) address += (subseq_rw_chunks*0x100);
				break;
			case 0x06://Test subsequent Byte write
				for(i=0;i < (subseq_rw_chunks*0x100);i++)
					if(!FRAM_WriteByte_Cont((test_value[phase]^device_addr) ^ (unsigned char)(address + i))){
						err = 0x02;
						break;
						}
				if(!err)
					address += (subseq_rw_chunks*0x100);
				break;
			case 0x07://Test subsequent Byte read
				for(i=0;i < (subseq_rw_chunks*0x100);i++)
					if(FRAM_ReadByte_Cont() != ((test_value[phase]^device_addr) ^ (unsigned char)(address + i))){
						err = 0x04;
						break;
						}
				if(!err)
					address += (subseq_rw_chunks*0x100);
				break;
			case 0x08://Speed check Buffer write
				for(i=0;i < subseq_rw_chunks;i++){
					if(!FRAM_WriteBuf_Cont(0x00,fram_buffer)){
						err = 0x02;
						break;
						}
					}
				if(!err) address += (subseq_rw_chunks*0x100);
				break;
			case 0x09://Speed check buffer read
				for(i=0;i < subseq_rw_chunks;i++)
					FRAM_ReadBuf_Cont(0x00,fram_buffer);
				address += (subseq_rw_chunks*0x100);
				break;				
			}
	if(phase > 0x03)
		FRAM_End(); //for continous read/write test
	//on error, set error status vars. Abort if error_count_abort is reached
	if(err){
		last_error_op = err;
		last_error_phase = phase;
		//error_count_abort is reached, switch to abort-on-error phase
		if(++error_count == error_count_abort)
			phase = 0x0B;
		}
	//on success, check if address range is reached and switch device_addr / phase accordingly
	else{
		//address range reached, next device
		if(address > address_range){
			address = 0x0000;
			//all devices tested, next test phase
			if(++device_addr == num_devices){
				phase++;
				device_addr = 0x00;
				//timecount start/stop&fetch on phase switch
				if(phase == 0x09)
					timecount_bw = fram_timecount_stop();
				else if (phase == 0x0A)
					timecount_br = fram_timecount_stop();
				fram_timecount_start();
				}
			}
		}
	}

/////////////////////////////////////////////////////////////////////////////
// This function is periodically called by MIOS. The frequency has to be
// initialized with MIOS_Timer_Set
/////////////////////////////////////////////////////////////////////////////
void Timer(void) __wparam{
	timecount += 10;
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
			phase = first_test_phase;
			break;
		case 0x02://buffer w/rc
		case 0x03://single byte w/rc
		case 0x04://subsequent buffer w
		case 0x05://subsequent buffer rc
		case 0x06://subsequent byte w
		case 0x07://subsequent byte rc
		case 0x08://speed check (buffer write)
		case 0x09://speed check (buffer read)
			switch(phase){
				case 0x02:MIOS_LCD_PrintCString("Buffer Wr/Rd/Cp");break;
				case 0x03:MIOS_LCD_PrintCString("Byte Wr/Rd/Cp");break;
				case 0x04:MIOS_LCD_PrintCString("Subseq Buffer Wr");break;
				case 0x05:MIOS_LCD_PrintCString("Subseq Buffer Rd/Cp");break;
				case 0x06:MIOS_LCD_PrintCString("Subseq Byte Wr");break;
				case 0x07:MIOS_LCD_PrintCString("Subseq Byte Rd/Cp");break;
				case 0x08:MIOS_LCD_PrintCString("Speed check Buf Wr");break;
				case 0x09:MIOS_LCD_PrintCString("Speed check Buf Rd");break;
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
		case 0x0A://test successfully finished
			MIOS_LCD_PrintCString("FRAM Test finished");
			MIOS_LCD_CursorSet(0x40);
			MIOS_LCD_PrintCString("successfully!");
			MIOS_LCD_MessageStart(0xFF);
			phase = 0x0E;
			break;
		case 0x0B://abort on error
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
				case 0x08:MIOS_LCD_PrintCString("Speed check Wr:");break;
				}
			switch(last_error_op){
				case 0x01:MIOS_LCD_PrintCString("BW");break;
				case 0x02:MIOS_LCD_PrintCString("W");break;
				case 0x03:MIOS_LCD_PrintCString("BR");break;
				case 0x04:MIOS_LCD_PrintCString("RC");break;
				}
			phase = 0x0C;
			MIOS_LCD_MessageStart(0xFF);
			break;
		case 0x0C://abort on error, screen 2
			MIOS_LCD_PrintCString("Device: 0x");
			MIOS_LCD_PrintHex2(device_addr);
			MIOS_LCD_CursorSet(0x40);
			MIOS_LCD_PrintCString("Address: ");
			print_hex4(address);
			phase = 0x0D;
			MIOS_LCD_MessageStart(0xFF);
			break;
		case 0x0D://abort on error, screen 3
			MIOS_LCD_PrintCString("FRAM_ERROR: 0x");
			MIOS_LCD_PrintHex2(FRAM_ERROR);
			MIOS_LCD_CursorSet(0x40);
			MIOS_LCD_PrintCString("FRAM_REG: 0x");
			MIOS_LCD_PrintHex2(FRAM_REG);
			phase = 0x0B;
			MIOS_LCD_MessageStart(0xFF);
			break;
		case 0x0E://test successfully finished, screen 2
			MIOS_LCD_PrintCString("Device count: 0x");
			MIOS_LCD_PrintHex2(num_devices);
			MIOS_LCD_CursorSet(0x40);
			MIOS_LCD_PrintCString("Addr. Range: ");
			print_hex4(address_range);
			phase = 0x0F;
			MIOS_LCD_MessageStart(0xFF);
			break;
		case 0x0F://test successfully finished, screen 3
			MIOS_LCD_PrintCString("Buf Wr: ");
			MIOS_LCD_PrintBCD5(timecount_bw);
			MIOS_LCD_PrintCString(" mSec");
			MIOS_LCD_CursorSet(0x40);
			MIOS_LCD_PrintCString("Bur Rd: ");
			MIOS_LCD_PrintBCD5(timecount_br);
			MIOS_LCD_PrintCString(" mSec");
			phase = 0x0A;
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
	if(pin_value == 0 && pin < 0x06){
		switch(pin){
			case 0x00:first_test_phase = 0x02;break;
			case 0x01:first_test_phase = 0x03;break;
			case 0x02:first_test_phase = 0x04;break;
			case 0x03:first_test_phase = 0x06;break;
			case 0x04:first_test_phase = 0x08;break;
			case 0x05:first_test_phase = 0x09;break;
			}
		phase = 0x01;
		MIOS_LCD_MessageStop();
		fram_timecount_start();
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
