// $Id: main.c 333 2008-05-10 20:49:56Z tk $
/*
 * MIOS SDCC Wrapper
 *
 * ==========================================================================
 *
 *  Copyright (C) 2008  Matthias Mächler (maechler@mm-computing.ch / thismaechler@gmx.ch)
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



//num DOUTs to test
#define num_douts 32
//if 1, DIN switches according DOUT
#define din_to_dout 1
//if 1, a testrun with SR
#define testrun_srset 1
//test interval on startup in 10ms steps
#define interval 30


unsigned int count,run,led;


/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS after startup to initialize the 
// application
/////////////////////////////////////////////////////////////////////////////
void Init(void) __wparam{
  	MIOS_SRIO_UpdateFrqSet(3); // ms
  	MIOS_SRIO_NumberSet(8);
  	MIOS_SRIO_DebounceSet(20);
  	led = 0;
  	count = 0;
  	run=0;
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
//show start message for 2 sec
if(run==0){
	if(count++>100){
		run = 1;
		count = 0;
		MIOS_LCD_Clear();	
		MIOS_LCD_CursorSet(0x00);
		MIOS_LCD_PrintCString("DOUT PinSet ");
		}
	return;
	}
//--walk DOUT's
if(count++>interval){
	count=0;
	if(run==2){
		if (led)
			MIOS_DOUT_SRSet((led-1) >> 3 & 0x0F, 0x00);
		else
			MIOS_DOUT_PinSet(num_douts-1,0);
		MIOS_DOUT_SRSet(led >> 3 & 0x0F, MIOS_HLP_GetBitORMask(led & 0x07));
		}
	else{
		if (led)
			MIOS_DOUT_PinSet(led-1,0);
		MIOS_DOUT_PinSet(led,1);
		}
	MIOS_LCD_CursorSet(0x40);
	MIOS_LCD_PrintCString("Pin ");
	MIOS_LCD_PrintBCD2(led++);
	if((led == num_douts)&&(!testrun_srset || (run==2))){
		MIOS_TIMER_Stop();
		run=0;
		}
	else if(led == num_douts){
		run=2;
		led=0;
		MIOS_LCD_Clear();	
		MIOS_LCD_CursorSet(0x00);
		MIOS_LCD_PrintCString("DOUT SRSet BitOrMask");
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS when the display content should be 
// initialized. Thats the case during startup and after a temporary message
// has been printed on the screen
/////////////////////////////////////////////////////////////////////////////
void DISPLAY_Init(void) __wparam{
 	MIOS_LCD_Clear();	
	MIOS_LCD_CursorSet(0x00);
	MIOS_LCD_PrintCString("Start DOUT Test..");
  	MIOS_TIMER_Init(0x02,50000);//10ms
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
	if(pin_value || run) 
		return; 
	if (din_to_dout && pin < num_douts)
		MIOS_DOUT_PinSet(pin,!MIOS_DOUT_PinGet(pin));
	MIOS_LCD_Clear();
	MIOS_LCD_CursorSet(0x00);
	MIOS_LCD_PrintCString("Pin ");
	MIOS_LCD_PrintBCD2(pin);
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
