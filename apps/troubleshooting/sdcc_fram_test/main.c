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

//---------------------------------------------------------------------------
//-- change these defines according to the device type / device count     ---
//-- test_value_deviceaddr_only helps to find problems with multiplexing  ---
//---------------------------------------------------------------------------


//address range: 0x7FFF for FM24C512 and FM24C256, 0x1FFF for the FM24C64
#define address_range 0x7FFF 

//device address count. each FM24C512 uses two addresses (two memory blocks)
//16 x FM24C512 (multiplexed) equals 0x20 devices, 4 FM24C512 (not multiplexed)
//equal 0x08 devices.
#define num_devices 0x08

//set this to 1 to have always the device-address written to each byte of
//a device. on rd/cp errors you can see wrong multiplexing. you will only
//notice problems in subsequent tests if multiplexing fails! wr/rd/cp tests
//will read the written data immediately, so errors in multiplexing will not
//cause wrong values read!
#define test_value_deviceaddr_only  0

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------



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
unsigned char error_op; //1: begin write; 2: write; 3 begin read; 4: read/compare
unsigned char error_phase;
unsigned char error_byte_rd,error_byte_cp;
unsigned char fram_buffer[0x100]; //data buffer
unsigned int timecount,timecount_bw,timecount_br;//timecount in mS

unsigned char first_test_phase;

//how much data is written / read in subsequent test (*256)
//this value should not exceed the address_range!
#define subseq_rw_chunks 0x20


//note: phase_value_offset [0] and [1] are never used. [4] and [5] has to be equal,
//also [6] and [7] (else subsequent r/w test(s) will fail on read/compare! 
const unsigned char phase_value_offset[0x08] = {0x00,0x00,0x01,0x02,0x03,0x03,0x04,0x04};


/////////////////////////////////////////////////////////////////////////////
// Application Prototypes
/////////////////////////////////////////////////////////////////////////////

void fram_timecount_start(void) __wparam;
unsigned int fram_timecount_stop(void) __wparam;

unsigned char get_test_byte(unsigned int) __wparam;
unsigned char check_test_byte(unsigned int,unsigned char) __wparam;
void fram_init_buffer(void) __wparam;
unsigned char fram_check_buffer(void) __wparam;

void print_hex4(unsigned int) __wparam;

/////////////////////////////////////////////////////////////////////////////
// Application Functions
/////////////////////////////////////////////////////////////////////////////

//--------timecount functions-------

void fram_timecount_start(void) __wparam{
	timecount = 0;
	MIOS_TIMER_Init(0x01,50000);//10 mS
	}
	
unsigned int fram_timecount_stop(void) __wparam{
	MIOS_TIMER_Stop();
	return timecount;
	}
	
//---------byte test / buffer functions------

//gets a test byte for a byte_address. each device / phase has a different 
//offset for the first byte to test also proper device selection / not to
//inherit devices from the last test phase (if write does not work properly).
//to find problems with multiplexing, enable test_value_deviceaddr_only.
unsigned char get_test_byte(unsigned int byte_addr) __wparam{
	#if test_value_deviceaddr_only==0
	return (address+device_addr+phase_value_offset[phase]);
	#else
	return device_addr;
	#endif
	}

unsigned char check_test_byte(unsigned int byte_addr,unsigned char value) __wparam{
	error_byte_cp = get_test_byte(byte_addr);
	if(error_byte_cp != value){
		error_byte_rd = value;
		address = byte_addr;
		return 0x00;
		}
	return 0x01;
	}


void fram_init_buffer(void) __wparam{
	unsigned int i;
	for(i=0;i<0x100;i++)
		fram_buffer[i] = get_test_byte(address+i);
	}

unsigned char fram_check_buffer(void) __wparam{
	unsigned int i;
	for(i=0;i<0x100;i++){
		if(!check_test_byte(address+i,fram_buffer[i]))
			return 0x00;
		}
	return 0x01;
	}

//----------helper functions-----------

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
	unsigned char b,mode;
	unsigned int i;
	if (phase < 0x02 || phase > 0x09)
		return;
	//FRAM_Begin with read/write mode
	if(phase > 0x03){//for continous read/write test
		mode = (phase & 0x01) ? FRAM_Mode_Read : FRAM_Mode_Write;
		if(!FRAM_Begin(device_addr,address,mode))
			error_op = (mode==FRAM_Mode_Read) ? 0x03 : 0x01;
		}
	if(!error_op)
		switch(phase){
			case 0x02://Test Buffer W/R
				fram_init_buffer();
				if(!FRAM_WriteBuf(device_addr,address,0x00,fram_buffer)){
					error_op = (FRAM_ERROR == 0x10) ? 0x02 : 0x01;
					break;
					}
				fram_init_buffer();
				if(!FRAM_ReadBuf(device_addr,address,0x00,fram_buffer)){
					error_op = 0x03;
					break;
					}
				if(!fram_check_buffer())
					error_op = 0x04;
				else
					address += 0x100;
				break;
			case 0x03://Test Byte W/R
				for(i=0;i<0x100;i++){
					if(!FRAM_WriteByte(device_addr,address,get_test_byte(address+i))){
						error_op = (FRAM_ERROR == 0x10) ? 0x02 : 0x01;
						break;
						}
					b = FRAM_ReadByte(device_addr,address); 
					if(FRAM_ERROR)
						error_op = 0x03;
					else if(!check_test_byte(address+i,b))
						error_op = 0x04;
					else
						address++;
					}
				break;
			case 0x04://Test subsequent Buffer write
				for(i=0;i < subseq_rw_chunks;i++){
					fram_init_buffer();
					if(!FRAM_WriteBuf_Cont(0x00,fram_buffer)){
						error_op = 0x02;
						break;
						}
					address += 0x100;
					}
				break;
			case 0x05://Test subsequent Buffer read
				for(i=0;i < subseq_rw_chunks;i++){
					FRAM_ReadBuf_Cont(0x00,fram_buffer);
					if(!fram_check_buffer()){
						error_op = 0x04;
						break;
						}
					address += 0x100;
					}
				break;
			case 0x06://Test subsequent Byte write
				for(i=0;i < (subseq_rw_chunks*0x100);i++){
					if(!FRAM_WriteByte_Cont(get_test_byte(address))) {
						error_op = 0x02;
						break;
						}
					address ++;
					}
				break;
			case 0x07://Test subsequent Byte read
				for(i=0;i < (subseq_rw_chunks*0x100);i++){
					if(FRAM_ReadByte_Cont() != get_test_byte(address) ){
						error_op = 0x04;
						break;
						}
					address++;
					}
				break;
			case 0x08://Speed check Buffer write
				for(i=0;i < subseq_rw_chunks;i++){
					if(!FRAM_WriteBuf_Cont(0x00,fram_buffer)){
						error_op = 0x02;
						break;
						}
					address += 0x100;
					}
				break;
			case 0x09://Speed check buffer read
				for(i=0;i < subseq_rw_chunks;i++)
					FRAM_ReadBuf_Cont(0x00,fram_buffer);
				address += (subseq_rw_chunks*0x100);
				break;				
			}
	if(phase > 0x03)
		FRAM_End(); //for continous read/write test
	//on error, set error status vars and abort.
	if(error_op){
		error_phase = phase;
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
			error_op = 0;
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
			switch(error_phase){
				case 0x02:MIOS_LCD_PrintCString("Buffer Wr/Rd/Cp");break;
				case 0x03:MIOS_LCD_PrintCString("Byte Wr/Rd/Cp");break;
				case 0x04:MIOS_LCD_PrintCString("Subseq Buffer Wr");break;
				case 0x05:MIOS_LCD_PrintCString("Subseq Buffer Rd/Cp");break;
				case 0x06:MIOS_LCD_PrintCString("Subseq Byte Wr");break;
				case 0x07:MIOS_LCD_PrintCString("Subseq Byte Rd/Cp");break;
				case 0x08:MIOS_LCD_PrintCString("Speed check Wr");break;
				}
			MIOS_LCD_CursorSet(0x40);			
			MIOS_LCD_PrintCString("Error: ");
			switch(error_op){
				case 0x01:MIOS_LCD_PrintCString("Begin Write");break;
				case 0x02:MIOS_LCD_PrintCString("Write");break;
				case 0x03:MIOS_LCD_PrintCString("Begin Read");break;
				case 0x04:MIOS_LCD_PrintCString("Read/Compare");break;
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
			if(error_op == 0x04){//for read/compare error op
				MIOS_LCD_PrintCString("Read Byte       0x");
				MIOS_LCD_PrintHex2(error_byte_rd);
				MIOS_LCD_CursorSet(0x40);
				MIOS_LCD_PrintCString("Does not equal: 0x");
				MIOS_LCD_PrintHex2(error_byte_cp);
				}
			else{//all other error op's
				MIOS_LCD_PrintCString("FRAM_ERROR: 0x");
				MIOS_LCD_PrintHex2(FRAM_ERROR);
				MIOS_LCD_CursorSet(0x40);
				MIOS_LCD_PrintCString("FRAM_REG: 0x");
				MIOS_LCD_PrintHex2(FRAM_REG);
				}
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
