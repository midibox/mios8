// $Id: main.c 333 2008-05-10 20:49:56Z tk $
/*
 * MIDI-Box Channel-Mapper
 *
 * ==========================================================================
 *
 *  Copyright (C) 2008  Matthias Mächler (maechler@mm-computing.ch)
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


/////////////////////////////////////////////////////////////////////////////
// Application specific 
/////////////////////////////////////////////////////////////////////////////

//each set bit of inputs/outputs assigns an channel to the bus (lsb = channel 0, msb = channel 16)
struct midi_bus{
	unsigned int inputs;
	unsigned int outputs;	
	};
	
//--constants--

//--1 tick = 52.428ms; 20 ticks = ca. 1 sec--
//time that the user has to hold the preset/bus button down to init all presets/all buses
#define init_request_ticks 60
//time that all the 16 value-leds will flash after a init
#define init_ledflash_ticks 7

const unsigned int int_bit_ormask[16] = {
	0x0001,0x0002,0x0004,0x0008,	
	0x0010,0x0020,0x0040,0x0080,	
	0x0100,0x0200,0x0400,0x0800,	
	0x1000,0x2000,0x4000,0x8000	
	};
	
//--current state--
unsigned char current_screen_num=0;//0:prs;1:bus;2:in;3:out;stored at 0x00 in EEPROM
unsigned char current_preset_num=0;//stored at 0x01 of EEPROM
unsigned char current_bus_num=0;//stored at 0x02 of EEPROM

struct midi_bus current_preset[16];//stored in Bank Stick after change
signed char midi_inout_map[16][16];//map the input-> output relations for faster msg forwarding

//--timer--
unsigned char timer_ticks_count=0;

//------------------------------------------
//----------application functions-----------
//------------------------------------------

void store_preset(unsigned char num) __wparam{
	MIOS_BANKSTICK_WritePage(num*64, @current_preset);
	}
	
void load_preset(unsigned char num) __wparam{
	unsigned char bus,chn_in,chn_out,i;	
	MIOS_BANKSTICK_ReadPage(num*64, @current_preset);	
	//extraxt midi_inout_map
	for(bus=0;bus<16;bus++){
		for(chn_in=0;chn_in<16;chn_in++){
			if(current_preset[bus].inputs & int_bit_ormask[chn_in]){				
				i=0;				
				for(chn_out=0;chn_out<16;chn_out++){
					if(current_preset[bus].outputs & int_bit_ormask[chn_out])
						midi_inout_map[chn_in][i++] = chn_out;
					}
				midi_inout_map[chn_in][i] = -1;//terminate list
				}
			}
		}
	}
		
void screen_init(void) __wparam{
	//set bitmask for screen button
	MIOS_DOUT_SRSet(0x00,MIOS_HLP_GetBitORMask(current_screen_num));
	//clear value screen
	MIOS_DOUT_SRSet(0x01,0x00);
	MIOS_DOUT_SRSet(0x02,0x00);
	switch(current_screen_num){
		case 0://preset screen
			if(current_preset_num < 8)			
				MIOS_DOUT_SRSet(0x01,MIOS_HLP_GetBitORMask(current_screen_num));
			else
				MIOS_DOUT_SRSet(0x02,MIOS_HLP_GetBitORMask(current_screen_num-8));				
			break;
		case 1://bus screen
			if(current_bus_num < 8)			
				MIOS_DOUT_SRSet(0x01,MIOS_HLP_GetBitORMask(current_bus_num));
			else
				MIOS_DOUT_SRSet(0x02,MIOS_HLP_GetBitORMask(current_bus_num-8));							
			break;
		case 2://inputs screen
			MIOS_DOUT_SRSet(0x01,(unsigned char)(current_preset[current_bus_num].inputs & 0x00ff));
			MIOS_DOUT_SRSet(0x02,(unsigned char)(current_preset[current_bus_num].inputs >>8 & 0x00ff));
			break;
		case 3://outputs screen
			MIOS_DOUT_SRSet(0x01,(unsigned char)(current_preset[current_bus_num].outputs & 0x00ff));
			MIOS_DOUT_SRSet(0x02,(unsigned char)(current_preset[current_bus_num].outputs >>8 & 0x00ff));
		}
	}

//inits and stores the current preset. bus0:in_ch,out_ch0;bus1:in_ch1,out_ch1;..
void preset_init(void) __wparam{
	unsigned char i;
	for(i=0;i<16;i++){
		current_preset[i].inputs = current_preset[i].outputs = int_bit_ormask[i];
		midi_inout_map[i][0]=i;
		midi_inout_map[i][0]=-1;//terminate		
		}
	store_preset(current_preset_num);	
	}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS after startup to initialize the 
// application
/////////////////////////////////////////////////////////////////////////////
void Init(void) __wparam{
  	MIOS_SRIO_UpdateFrqSet(1); // ms
  	MIOS_SRIO_NumberSet(3);//need 20 inputs / outputs 
  	MIOS_SRIO_DebounceSet(10);
  	MIOS_BANKSTICK_CtrlSet(0x00);//stick 0, verify enabled
  	//load last application state
  	current_screen_num = MIOS_EEPROM_Read(0x00);
  	current_preset_num = MIOS_EEPROM_Read(0x01);
  	current_bus_num = MIOS_EEPROM_Read(0x02);
  	load_preset(current_preset_num);
  	//init screen
  	screen_init();
  	}

/////////////////////////////////////////////////////////////////////////////
// This function is periodically called by MIOS. The frequency has to be
// initialized with MIOS_Timer_Set
/////////////////////////////////////////////////////////////////////////////
void Timer(void) __wparam{
	unsigned char i;
	if(++timer_ticks_count >= init_request_ticks){
		if(timer_ticks_count == init_request_ticks){
			preset_init();//inits the current preset
			if(!current_preset_num){//page is preset.. 
				for(i=0;i<16;i++)//inits all presets on bankstick(store reset preset)
					store_preset(i);
				}	
			else//page is bank so..
				store_preset(current_preset_num);//store the current (reset) preset
			//flash!
			MIOS_DOUT_SRSet(0x01,0xff);
			MIOS_DOUT_SRSet(0x02,0xff);
			}
		else if(timer_ticks_count >= init_request_ticks+init_ledflash_ticks){
			MIOS_TIMER_Stop();//stops the init request countdown
			screen_init();
			}
		}
	}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS when a MIDI byte has been received
/////////////////////////////////////////////////////////////////////////////

void MPROC_NotifyReceivedByte(unsigned char byte) __wparam{
	//this function forwards all system messages to the output
	static unsigned char fx_status = 0;
	if(byte >= 0xf0){//system status byte
		MIOS_MIDI_TxBufferPut(byte);
	   //determine status
		switch(byte){
			case 0xf1://midi timecode 
			case 0xf3://songselect
				fx_status = 1;
				break; 	
			case 0xf2://songposition pointer
				fx_status = 2;				
				break;
			case 0xf0://sysex, forward until 0xf7
				fx_status = 0xff;
				break;
			default://applies also on 0xf7!
				fx_status = 0;
		 	}   
    	}
  	else if(fx_status){
      MIOS_MIDI_TxBufferPut(byte);
      if(fx_status!=0xff)
      	fx_status--;
   	}
 	}

/////////////////////////////////////////////////////////////////////////////
//  This function is called by MIOS when a complete MIDI event has been received
/////////////////////////////////////////////////////////////////////////////
void MPROC_NotifyReceivedEvnt(unsigned char evnt0, unsigned char evnt1, unsigned char evnt2) __wparam{
	unsigned char i;
	unsigned char evnt_type = evnt0 & 0xf0;
	unsigned char in_chn = evnt0 & 0x0f;
	unsigned char evnt2_send = !(evnt_type == 0xc0 || evnt_type == 0xd0);
	for(i=0;i<16 && midi_inout_map[in_chn][i]!=-1;i++){
		MIOS_MIDI_TxBufferPut(evnt_type+midi_inout_map[in_chn][i]);
		MIOS_MIDI_TxBufferPut(evnt1);
		if(evnt2_send)
			MIOS_MIDI_TxBufferPut(evnt2);
		}
	}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS when an button has been toggled
// pin_value is 1 when button released, and 0 when button pressed
/////////////////////////////////////////////////////////////////////////////
void DIN_NotifyToggle(unsigned char pin, unsigned char pin_value) __wparam{
	unsigned char i,chn_out,chn_in,value;
	MIOS_TIMER_Stop();//stops the init request countdown
	if(pin_value) return;//button up, nothing more to do	
	if(pin > 7){
		value = pin - 8;
		switch(current_screen_num){
			case 0://preset screen
				current_preset_num = value;				
				load_preset(current_preset_num);
				MIOS_EEPROM_Write(0x01,current_preset_num);
				break;			
			case 1://bus screen
				current_bus_num = value;
				MIOS_EEPROM_Write(0x03,current_preset_num);
				break;			
			case 2://input screen
				if (MIOS_DIN_PinGet(0x02)){//set/unset input chanel
					current_preset[current_bus_num].inputs = 
						current_preset[current_bus_num].inputs==int_bit_ormask[value] ? 0x0000 :int_bit_ormask[value];
					}
				else{//add/remove input chanel
					current_preset[current_bus_num].inputs = 
						(current_preset[current_bus_num].inputs & int_bit_ormask[value])?
							(current_preset[current_bus_num].inputs ^ int_bit_ormask[value]):
							(current_preset[current_bus_num].inputs | int_bit_ormask[value]);
					}		
				//assign the bus's output channels to new input channel on the map			
				i=0;	
				for(chn_out=0;chn_out<16;chn_out++){
					if(current_preset[current_bus_num].outputs & int_bit_ormask[chn_out])//this is a bus output
						midi_inout_map[value][i++] = chn_out;
					}
				midi_inout_map[value][i] = -1;
				break;			
			case 3://output screen
				if (MIOS_DIN_PinGet(0x03)){//set/unset output chanel
					current_preset[current_bus_num].outputs = 
						(current_preset[current_bus_num].outputs==int_bit_ormask[value]) ? 
							0x0000 :int_bit_ormask[value];
					}
				else{//add/remove output chanel
					current_preset[current_bus_num].outputs = 
						(current_preset[current_bus_num].outputs & int_bit_ormask[value])?
							(current_preset[current_bus_num].outputs ^ int_bit_ormask[value]):
							(current_preset[current_bus_num].outputs | int_bit_ormask[value]);
					}				
				//assign the outputchannels to the bus's input-channels on the map  			
				for(chn_in=0;chn_in<16;chn_in++){
					if(current_preset[current_bus_num].inputs & int_bit_ormask[chn_in]){//this is a bus input
						i=0;						
						for(chn_out=0;chn_out<16;chn_out++){
							if(current_preset[current_bus_num].outputs & int_bit_ormask[chn_out])//this is a bus output
								midi_inout_map[chn_in][i++] = chn_out;
							}
						midi_inout_map[chn_in][i] = -1;
						}						
					}
				break;
			}
		}
	else if(pin < 4){
		current_screen_num = pin;
		MIOS_EEPROM_Write(0x00, current_screen_num);
		if(pin < 2){//start init request countdown
			timer_ticks_count = 0;			
			MIOS_TIMER_Init(0x03,0xffff);
			}			 			
		}
	screen_init();
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
	}
	
/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS in the mainloop when nothing else is to do
/////////////////////////////////////////////////////////////////////////////
void Tick(void) __wparam{
	}
	
/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS when a MIDI event has been received
// which has been specified in the MIOS_MPROC_EVENT_TABLE
/////////////////////////////////////////////////////////////////////////////
void MPROC_NotifyFoundEvent(unsigned entry, unsigned char evnt0, unsigned char evnt1, unsigned char evnt2) __wparam{
	}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS when a MIDI event has not been completly
// received within 2 seconds
/////////////////////////////////////////////////////////////////////////////
void MPROC_NotifyTimeout(void) __wparam{
	}

	
/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS before the shift register are loaded
/////////////////////////////////////////////////////////////////////////////
void SR_Service_Prepare(void) __wparam{
	}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS after the shift register have been loaded
/////////////////////////////////////////////////////////////////////////////
void SR_Service_Finish(void) __wparam{
	}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS when an encoder has been moved
// incrementer is positive when encoder has been turned clockwise, else
// it is negative
/////////////////////////////////////////////////////////////////////////////
void ENC_NotifyChange(unsigned char encoder, char incrementer) __wparam{
	}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS when a pot has been moved
/////////////////////////////////////////////////////////////////////////////
void AIN_NotifyChange(unsigned char pin, unsigned int pin_value) __wparam{
	}
	
