/*
 * This simple MIOS C Wrapper example demonstrates
 * how to drive 64 pots, 128 buttons, 128 LEDs
 *
 * Note: hardware settings (e.g. number of pots, muxed/unmuxed mode)
 *       are specified in main.h
 * 
 * See also the README.txt for additional informations
 *
 * ==========================================================================
 *
 * Copyright (C) 2005  Thorsten Klose (tk@midibox.org)
 * 
 * ==========================================================================
 * 
 * This file is part of a MIOS application
 *
 * This application is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This application is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with This application; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * ==========================================================================
 */

/////////////////////////////////////////////////////////////////////////////
// Include files
/////////////////////////////////////////////////////////////////////////////

#include "cmios.h"
#include "pic18f452.h"

#include "main.h"

/////////////////////////////////////////////////////////////////////////////
// Global variables
/////////////////////////////////////////////////////////////////////////////

// status of application (see bitfield declaration in main.h)
app_flags_t app_flags;

/////////////////////////////////////////////////////////////////////////////
// Local variables
/////////////////////////////////////////////////////////////////////////////

// last ain/din/dout
unsigned char last_ain_pin;
unsigned char last_din_pin;
unsigned char last_dout_pin;

//last enc vals
unsigned int enc1=1;
unsigned int enc2=1;
unsigned int enc3=1;
unsigned int enc0=1;

/////////////////////////////////////////////////////////////////////////////
// Vars for Loop Flash Function
/////////////////////////////////////////////////////////////////////////////

//channel clip trigger BUTTONS
unsigned int channelOneClipButtons[] = {17,18,101,103,56};
unsigned int channelTwoButtons[] = {120,122,125,127,124};
unsigned int channelThreeButtons[] = {106,104,109,111,108};
unsigned int channelFourButtons[] = {114,112,117,119,116};

//channel clip trigger LEDs
unsigned int channelOneLeds[] = {39,37,35,33,32};
unsigned int channelTwoLeds[] = {61,63,59,57,56};
unsigned int channelThreeLeds[] = {47,45,43,41,40};
unsigned int channelFourLeds[] = {55,53,51,49,48};

//channel - which clip playing
unsigned int channelOneClipPlaying = 0;
unsigned int channelTwoClipPlaying = 0;
unsigned int channelThreeClipPlaying = 0;
unsigned int channelFourClipPlaying = 0;

//current selected channel
unsigned int channelSelected = 1;

//prev selected clips
unsigned char prevChannelOne;
unsigned char prevChannelTwo;
unsigned char prevChannelThree;
unsigned char prevChannelFour;



//which LEDs to flash
unsigned char ledsToFlash[] = {0,0,0,0};



/////////////////////////////////////////////////////////////////////////////
// These functions turns off all the clip triggers 
/////////////////////////////////////////////////////////////////////////////
void killTriggersOne(unsigned int i) __wparam {
	for(i=0;i<6;i++){
		MIOS_DOUT_PinSet(channelOneLeds[i],0);
	}
}

/////////////////////////////////////////////////////////////////////////////
// Kill all the clip trigger lights
/////////////////////////////////////////////////////////////////////////////
unsigned char prevClipOne;
unsigned char prevClipTwo;
unsigned char prevClipThree;
unsigned char prevClipFour;

void setClipOne(unsigned char whichClip){
	channelOneClipPlaying = whichClip;
	if (whichClip != prevClipOne){
		ledsToFlash[0]=0;
	}
	prevClipOne = whichClip;	
}
void setClipTwo(unsigned int whichClip){
	channelTwoClipPlaying = whichClip;
	if (whichClip != prevClipTwo){
		ledsToFlash[1]=0;
	}
	prevClipTwo = whichClip;		
}
void setClipThree(unsigned int whichClip){
	channelThreeClipPlaying = whichClip;
	if (whichClip != prevClipThree){
		ledsToFlash[2]=0;
	}
	prevClipThree = whichClip;	
}
void setClipFour(unsigned int whichClip){
	channelFourClipPlaying = whichClip;
	if (whichClip != prevClipFour){
		ledsToFlash[3]=0;
	}
	prevClipFour = whichClip;	
}


////////////////////////////////////////////////////////////////////////////
// Set the Loop Button depending on the loop state of the channel
////////////////////////////////////////////////////////////////////////////
void setLoopButton1() {
				if (ledsToFlash[0] == 0){
					MIOS_DOUT_PinSet(77, 1);	
				}
				else {
					MIOS_DOUT_PinSet(77, 0);					
				}
				MIOS_DOUT_PinSet(77, 1);	
}

void setLoopButton2() {
				if (ledsToFlash[1] == 0){
					MIOS_DOUT_PinSet(77, 0);	
				}
				else {
					MIOS_DOUT_PinSet(77, 1);					
				}
}
void setLoopButton3() {
				if (ledsToFlash[2] != 0){
					MIOS_DOUT_PinSet(77, 1);	
				}
				else {
					MIOS_DOUT_PinSet(77, 0);					
				}
}
void setLoopButton4() {
				if (ledsToFlash[3] != 0){
					MIOS_DOUT_PinSet(77, 1);	
				}
				else {
					MIOS_DOUT_PinSet(77, 0);					
				}

}



/////////////////////////////////////////////////////////////////////////////
// This function is called when the loop button is pushed
// application
/////////////////////////////////////////////////////////////////////////////


void loopButton(unsigned char loopButtonState, unsigned int i) __wparam
{
	switch( channelSelected )
     {
        case 0 : //none selected;
                   break;

        case 1 : 	
				//if the loopButtonSTate is 0 the loop is off so turn on the LED if not flash it
				 if (loopButtonState == '0'){
				 	//set the led to solid lite
					MIOS_DOUT_PinSet(channelOneLeds[channelOneClipPlaying],1);
					ledsToFlash[0] = 0;
				 }
				 else{
				 	//flash the LED
				 	ledsToFlash[0] = channelOneLeds[channelOneClipPlaying];
				 }
	             break;

        case 2 : 	
				//if the loopButtonSTate is 0 the loop is off so turn on the LED if not flash it
				 if (loopButtonState == '0'){
				 	//set the led to solid lite
					MIOS_DOUT_PinSet(channelTwoLeds[channelTwoClipPlaying],1);
					ledsToFlash[1] = 0;
				 }
				 else{
				 	//flash the LED
				 	ledsToFlash[1] = channelTwoLeds[channelTwoClipPlaying];
				 }
	             break;	
				 
        case 3 : 	
				//if the loopButtonSTate is 0 the loop is off so turn on the LED if not flash it
				 if (loopButtonState == '0'){
				 	//set the led to solid lite
					MIOS_DOUT_PinSet(channelThreeLeds[channelThreeClipPlaying],1);
					ledsToFlash[2] = 0;
				 }
				 else{
				 	//flash the LED
				 	ledsToFlash[2] = channelThreeLeds[channelThreeClipPlaying];
				 }
	             break;		
				 
        case 4 : 	
				//if the loopButtonSTate is 0 the loop is off so turn on the LED if not flash it
				 if (loopButtonState == '0'){
				 	//set the led to solid lite
					MIOS_DOUT_PinSet(channelFourLeds[channelFourClipPlaying],1);
					ledsToFlash[3] = 0;
				 }
				 else{
				 	//flash the LED
				 	ledsToFlash[3] = channelFourLeds[channelFourClipPlaying];
				 }
	             break;						 			 	
		
		default  : break;
	}
}


/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS after startup to initialize the 
// application
/////////////////////////////////////////////////////////////////////////////
void Init(void) __wparam
{
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
  
  
  //LOAD VARS FOR FLASHING FUNCT
  ledsToFlash[0] = 0;
  ledsToFlash[1] = 0;
  ledsToFlash[2] = 0;
  ledsToFlash[3] = 0;
  
  //channel one Leds
  channelOneLeds[0] = 39;
  channelOneLeds[1] = 37;
  channelOneLeds[2] = 35;
  channelOneLeds[3] = 33;
  channelOneLeds[4] = 32;
  
  //channel two leds
  channelTwoLeds[0] = 61;
  channelTwoLeds[1] = 63;
  channelTwoLeds[2] = 59;
  channelTwoLeds[3] = 57;
  channelTwoLeds[4] = 56;  
  
  //channel three leds
	channelThreeLeds[0] = 47;
	channelThreeLeds[1] = 45;
	channelThreeLeds[2] = 43;
	channelThreeLeds[3] = 41;
	channelThreeLeds[4] = 40;

	//channel four leds
	channelFourLeds[0] = 55;
	channelFourLeds[1] = 53;
	channelFourLeds[2] = 51;
	channelFourLeds[3] = 49;
	channelFourLeds[4] = 48;  
  
  
  
  //channel selected
  channelSelected = 1;

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
  // clear screen
  MIOS_LCD_Clear();

  // print static messages
  MIOS_LCD_CursorSet(0x00); // first line
  MIOS_LCD_PrintCString(" AIN   DIN  DOUT");
  MIOS_LCD_CursorSet(0x40); // second line
  MIOS_LCD_PrintCString("xx:xxx xxxx xxxx");

  // request display update
  app_flags.DISPLAY_UPDATE_REQ = 1;
}

/////////////////////////////////////////////////////////////////////////////
//  This function is called in the mainloop when no temporary message is shown
//  on screen. Print the realtime messages here
/////////////////////////////////////////////////////////////////////////////
void DISPLAY_Tick(void) __wparam
{
  // do nothing if no update has been requested
  if( !app_flags.DISPLAY_UPDATE_REQ )
    return;

  // clear request
  app_flags.DISPLAY_UPDATE_REQ = 0;

  // print status of AIN
  //MIOS_LCD_CursorSet(0x40 + 0);
  //MIOS_LCD_PrintBCD2(last_ain_pin + 1);
  //MIOS_LCD_PrintChar(':');
  //MIOS_LCD_PrintBCD3(MIOS_AIN_Pin7bitGet(last_ain_pin));

  // print status of DIN
  //MIOS_LCD_CursorSet(0x40 + 7);
  //MIOS_LCD_PrintBCD3(last_din_pin + 1);
  //MIOS_LCD_PrintChar(MIOS_DIN_PinGet(last_din_pin) ? 'o' : '*');

  // print status of DOUT
 // MIOS_LCD_CursorSet(0x40 + 12);
 // MIOS_LCD_PrintBCD3(last_dout_pin + 1);
 // MIOS_LCD_PrintChar(MIOS_DOUT_PinGet(last_dout_pin) ? '*' : 'o');
}

/////////////////////////////////////////////////////////////////////////////
//  This function is called by MIOS when a complete MIDI event has been received
/////////////////////////////////////////////////////////////////////////////
void MPROC_NotifyReceivedEvnt(unsigned char evnt0, unsigned char evnt1, unsigned char evnt2, unsigned int i, unsigned int whichPin,unsigned int trueEvent) __wparam
{
  // check if note on or off event at channel 1 has been received
  if( evnt0 == 0x80 || evnt0 == 0x90 ) {
    // if note off event: force evnt2 to 0 for easier handling of 'LED off'
    if( evnt0 == 0x80 ) {
      //evnt2 = 0;
	}
	if (evnt0 == 0x90 ) {
	  //evnt2 = 1;
	}	 
	  
trueEvent=0;
		
    //if it is an ON event turn off all the other LEDs
     
		
    //if it is an ON event turn off all the other LEDs
        
	
	//***** CHANNEL 1 *********		
	//channel 1 Clip Trigger Button - 1 
	
	//was - evnt == 97
	if (evnt1 == 17) {
		trueEvent=1;
whichPin = 39;
		if (evnt2 != 0){
			setClipOne(0);
		} 

		
	}
	//channel 1 Clip Trigger Button - 2
	//was evnt1 == 99
	if (evnt1 == 18) {
		trueEvent=1;
whichPin = 37;
		if (evnt2 != 0){
			setClipOne(1);
		} 		
	}
	//channel 1 Clip Trigger Button - 3
	if (evnt1 == 101) {
		trueEvent=1;
whichPin = 35;
		if (evnt2 != 0){
			setClipOne(2);
		} 
	}
	//channel 1 Clip Trigger Button - 4
	if (evnt1 == 103) {
		trueEvent=1;
whichPin = 33;
		if (evnt2 != 0){
			setClipOne(3);
		}		
	} 
	//channel 1 Clip Trigger Button - 5
	//was - evnt == 100
	if (evnt1 == 56) {
		trueEvent=1;
whichPin = 32;
		if (evnt2 != 0){
			setClipOne(4);
		} 	
	}			
	
	
	
	//channel 1 SELECT CHANNEL
	if (evnt1 == 24) {
		//Channel 1 - Cue LED - Pin 2
		trueEvent=1;
whichPin = 38;

		if (evnt2 == 0){
			//channelSelected = 0;
		}
		else{
			channelSelected = 1;
			setLoopButton1();
		}
		

	}
	//channel 1 CUE
	if (evnt1 == 8) {
		trueEvent=1;
whichPin = 34;
	}
	//channel 1 BASS
	if (evnt1 == 32) {
		trueEvent=1;
whichPin = 4;
	}	
	//channel 1 MID
	if (evnt1 == 33) {
		trueEvent=1;
whichPin = 2;
	}
	//channel 1 TOP
	if (evnt1 == 34) {
		trueEvent=1;
whichPin = 0;
	}	
	/*	
	//channel 1 SELECT
	if (evnt1 == 24) {
		trueEvent=1;
whichPin = 38;
	}	
	*/	
	//channel 1 FX1
	if (evnt1 == 36) {
		trueEvent=1;
whichPin = 22;
	}		
	//channel 1 FX2
	if (evnt1 == 37) {
		trueEvent=1;
whichPin = 20;
	}		
	//channel 1 FX3
	if (evnt1 == 78) {
		trueEvent=1;
whichPin = 23;
	}	
	//channel 1 FX4
	if (evnt1 == 38) {
		trueEvent=1;
whichPin = 21;
	}	
	
	
	//***** channel 2 *********		
	//channel 2 Clip Trigger Button - 1 
	if (evnt1 == 120) {
		trueEvent=1;
whichPin = 61;
		if (evnt2 != 0){
			setClipTwo(0);
		} 	


	}
	//channel 2 Clip Trigger Button - 2
	if (evnt1 == 122) {
		trueEvent=1;
whichPin = 63;
		if (evnt2 != 0){
			setClipTwo(1);
		} 	
	}
	//channel 2 Clip Trigger Button - 3
	if (evnt1 == 125) {
		trueEvent=1;
whichPin = 59;
		if (evnt2 != 0){
			setClipTwo(2);
		} 	
	}
	//channel 2 Clip Trigger Button - 4
	if (evnt1 == 127) {
		trueEvent=1;
whichPin = 57;
		if (evnt2 != 0){
			setClipTwo(3);
		} 	
	}
	//channel 2 Clip Trigger Button - 5
	if (evnt1 == 124) {
		trueEvent=1;
whichPin = 56;
		if (evnt2 != 0){
			setClipTwo(4);
		} 	
	}
	
	
	
	//channel 2 SELECT CHANNEL
	if (evnt1 == 25) {
		//channel 2 - Cue LED - Pin 2
		trueEvent=1;
whichPin = 62;
		//TURN OFF ALL THE OTHER CHANNEL SELECT BUTTONS ** TO DO!!!
		
		if (evnt2 == 0){
			//channelSelected = 0;
		}
		else{
			channelSelected = 2;
			setLoopButton2();
		}		
		
	}
	//channel 2 CUE
	if (evnt1 == 9) {
		trueEvent=1;
whichPin = 58;
	}
	//channel 2 BASS
	if (evnt1 == 55) {
		trueEvent=1;
whichPin = 5;
	}	
	//channel 2 MID
	if (evnt1 == 54) {
		trueEvent=1;
whichPin = 3;
	}
	//channel 2 TOP
	if (evnt1 == 53) {
		trueEvent=1;
whichPin = 1;
	}		
	//channel 2 FX1
	if (evnt1 == 52) {
		trueEvent=1;
whichPin = 16;
	}		
	//channel 2 FX2
	if (evnt1 == 49) {
		trueEvent=1;
whichPin = 18;
	}		
	//channel 2 FX3
	if (evnt1 == 50) {
		trueEvent=1;
whichPin = 17;
	}	
	//channel 2 FX4
	//was 51
	if (evnt1 == 76) {
		trueEvent=1;
whichPin = 19;
	}	
	
	//***** channel 3  *********		
	
	//channel 3 Clip Trigger Button - 1 
	if (evnt1 == 106) {
		trueEvent=1;
whichPin = 47;
		if (evnt2 != 0){
			setClipThree(0);
		} 	
	}
	//channel 3 Clip Trigger Button - 2
	if (evnt1 == 104) {
		trueEvent=1;
whichPin = 45;
		if (evnt2 != 0){
			setClipThree(1);
		} 	
	}
	//channel 3 Clip Trigger Button - 3
	if (evnt1 == 109) {
		trueEvent=1;
whichPin = 43;
		if (evnt2 != 0){
			setClipThree(2);
		} 	
	}
	//channel 3 Clip Trigger Button - 4
	if (evnt1 == 111) {
		trueEvent=1;
whichPin = 41;
		if (evnt2 != 0){
			setClipThree(3);
		} 	
	}
	//channel 3 Clip Trigger Button - 5
	if (evnt1 == 108) {
		trueEvent=1;
whichPin = 40;
		if (evnt2 != 0){
			setClipThree(4);
		} 	
	}	
	
	//channel 3  SELECT CHANNEL
	if (evnt1 == 26) {
		//channel 3  - Cue LED - Pin 2
		trueEvent=1;
whichPin = 46;

		if (evnt2 == 0){
			//channelSelected = 0;
		}
		else{
			channelSelected = 3;
			setLoopButton3();
		} 
		

	}
	//channel 3  CUE ****
	if (evnt1 == 10) {
		trueEvent=1;
whichPin = 42;
	}
	//channel 3  BASS
	if (evnt1 == 59) {
		trueEvent=1;
whichPin = 12;
	}	
	//channel 3  MID
	if (evnt1 == 58) {
		trueEvent=1;
whichPin = 10;
	}
	//channel 3  TOP
	if (evnt1 == 57) {
		trueEvent=1;
whichPin = 8;
	}		
	/*
	//channel 3  SELECT ****
	if (evnt1 == 26) {
		trueEvent=1;
whichPin = 46;
	}	
	*/	
	//channel 3  FX1
	if (evnt1 == 64) {
		trueEvent=1;
whichPin = 31;
	}		
	//channel 3  FX2
	if (evnt1 == 61) {
		trueEvent=1;
whichPin = 29;
	}		
	//channel 3  FX3
	if (evnt1 == 63) {
		trueEvent=1;
whichPin = 30;
	}	
	//channel 3  FX4
	if (evnt1 == 62) {
		trueEvent=1;
whichPin = 28;
	}		
	
	
		//***** channel 4   *********		
		
	//channel 4 Clip Trigger Button - 1 
	if (evnt1 == 114) {
		trueEvent=1;
whichPin = 55;
		if (evnt2 != 0){
			setClipFour(0);
		} 	
	}
	//channel 4 Clip Trigger Button - 2
	if (evnt1 == 112) {
		trueEvent=1;
whichPin = 53;
		if (evnt2 != 0){
			setClipFour(1);
		} 	
	}
	//channel 4 Clip Trigger Button - 3
	if (evnt1 == 117) {
		trueEvent=1;
whichPin = 51;
		if (evnt2 != 0){
			setClipFour(2);
		} 	
	}
	//channel 4 Clip Trigger Button - 4
	if (evnt1 == 119) {
		trueEvent=1;
whichPin = 49;
		if (evnt2 != 0){
			setClipFour(3);
		} 	
	}
	//channel 4 Clip Trigger Button - 5
	if (evnt1 == 116) {
		trueEvent=1;
whichPin = 48;
		if (evnt2 != 0){
			setClipFour(4);
		} 	
	}		
	
	//channel 4   SELECT CHANNEL
	if (evnt1 == 27) {
		//channel 4   - Cue LED - Pin 2
		trueEvent=1;
whichPin = 54;


		if (evnt2 == 0){
			//channelSelected = 0;
		}
		else{
			channelSelected = 4;
			setLoopButton4();
		}
		

	}
	//channel 4   CUE
	if (evnt1 == 11) {
		trueEvent=1;
whichPin = 50;
	}
	//channel 4   BASS
	if (evnt1 == 43) {
		trueEvent=1;
whichPin = 13;
	}	
	//channel 4   MID
	if (evnt1 == 42) {
		trueEvent=1;
whichPin = 11;
	}
	//channel 4   TOP
	if (evnt1 == 41) {
		trueEvent=1;
whichPin = 9;
	}		
	//channel 4   FX1
	if (evnt1 == 44) {
		trueEvent=1;
whichPin = 26;
	}		
	//channel 4   FX2
	if (evnt1 == 45) {
		trueEvent=1;
whichPin = 24;
	}		
	//channel 4   FX3
	if (evnt1 == 47) {
		trueEvent=1;
whichPin = 27;
	}	
	//channel 4   FX4
	if (evnt1 == 46) {
		trueEvent=1;
whichPin = 25;
	}	
	
	
	// **** MASTER SECTION *****
	//PLAY
	if (evnt1 == 84) {
		//turn on play button
		trueEvent=1;
whichPin = 73;
		
		//turn on STOP if the msg is to turn off the play
		if(evnt2 == 0){
			MIOS_DOUT_PinSet(72, 1);
		}
		else{
			MIOS_DOUT_PinSet(72, 0);
		}
	}
	//STOP 
	if (evnt1 == 93) {
		//turn on play button
		trueEvent=1;
whichPin = 72;
		//MIOS_DOUT_PinSet(72, 1);
		
	}


//MIOS_DOUT_PinSet(72, 1);
	
	
	//REC
	if (evnt1 == 95) {
		//turn on rec button
		trueEvent=1;
whichPin = 76;
	}	
	//SCN - waveform follow
	if (evnt1 == 83) {
		//turn on SCN button
		trueEvent=1;
whichPin = 74;
	}
	//LOOP
	if (evnt1 == 35) {
		//turn on LOOP button
		trueEvent=1;
whichPin = 77;
	}
	
	
	
	//FX1
	if (evnt1 == 12) {
		//turn on play button
		trueEvent=1;
whichPin = 68;
	}
	//FX2
	if (evnt1 == 65) {
		//turn on play button
		trueEvent=1;
whichPin = 69;
	}	
	
	//FX3
	if (evnt1 == 13) {
		//turn on play button
		trueEvent=1;
whichPin = 70;
	}
	//FX4
	if (evnt1 == 67) {
		//turn on play button
		trueEvent=1;
whichPin = 71;
	}
	//FX5
	if (evnt1 == 14) {
		//turn on play button
		trueEvent=1;
whichPin = 66;
	}		
	//FX6
	if (evnt1 == 69) {
		//turn on play button
		trueEvent=1;
whichPin = 67;
	}			
	
	//FX7
	if (evnt1 == 15) {
		//turn on play button
		trueEvent=1;
whichPin = 64;
	}
	//FX8
	if (evnt1 == 72) {
		//turn on play button
		trueEvent=1;
whichPin = 65;
	}			
	
	// **** CATCH ANY RANDOM STUFF WE DONT WANT TO EFFECT LEDS- *****	
	if (evnt1 == 75 || trueEvent == 0){
		//36 is spare on channel 1 channel strip PCB
		whichPin = 36;
	}
		
	
		
	
	
		//set the LED
		MIOS_DOUT_PinSet(whichPin, evnt2);

	
  
	  
	  
	  
	 //sets the LED to the midi event number 	
    // number of DOUT pin in evnt1, value in evnt2
    //MIOS_DOUT_PinSet(evnt1, evnt2);

	//set the pins how we want
	//MIOS_DOUT_PinSet('24',1 ? 0x01 : 0x00);	
	

	

	
	

    // notify display handler in DISPLAY_Tick() that DOUT value has changed
    //last_dout_pin = evnt1;
	last_dout_pin = evnt1;
	
    app_flags.DISPLAY_UPDATE_REQ = 1;
  }
  //testing stuff from here down
  
  
  //OUTPUT EVENTS TO SCREEN

		
  // clear screen
  MIOS_LCD_Clear();

  // print static messages
  MIOS_LCD_CursorSet(0x00); // first line
  MIOS_LCD_PrintCString(" midi in ");
  MIOS_LCD_CursorSet(0x40); // second line
   MIOS_LCD_CursorSet(0x40 + 0);
MIOS_LCD_PrintBCD3(evnt0); // CC at channel 2
 MIOS_LCD_CursorSet(0x40 + 7);
  		MIOS_LCD_PrintBCD3(evnt1);  // pin number corresponds to CC number
 MIOS_LCD_CursorSet(0x40 + 12);
  		MIOS_LCD_PrintBCD3(evnt2); // don't send 10bit pin_value, but 7bit value
 

	
		
			//aardvark
//turn on all the DOUT 1, J3 LEDS
		for (i=0;i<150;i++) {
			//MIOS_DOUT_PinSet(i, 1);
		}	


  
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

unsigned int midiClockCounter=0;
unsigned int ledBlinkState=1;

void MPROC_NotifyReceivedByte(unsigned char byte, unsigned int i) __wparam
{
	if (byte == 248){
		midiClockCounter++;
	}
	
	//update LEDs
	//96 ticks per whole note
	//48 is 1/2s
	//24 is 1/4s etc
	
	
	if (midiClockCounter >=  12){
		midiClockCounter = 0;
		if (ledBlinkState == 0){
			ledBlinkState = 1;
			//turn on the SCN button
			MIOS_DOUT_PinSet(74, 1);	
			
			/*
			//led flash fun
			for (i=0;i<128;i++){
				MIOS_DOUT_PinSet(i, 1);	
			}
			*/
				
		}
		else{
			ledBlinkState = 0;
			//turn off the SCN button
			MIOS_DOUT_PinSet(74, 0);
			
			
			/*
			//led flash fun
			for (i=0;i<128;i++){
				MIOS_DOUT_PinSet(i, 0);	
			}
			*/
							
		}
		midiClockCounter = 0;
		
		//channelOneFlash = 39;
		
		
		for (i=0;i<4;i++){
			if(ledsToFlash[i] != 0){
				MIOS_DOUT_PinSet(ledsToFlash[i], ledBlinkState);	
			}
		}
/*		
		
		MIOS_DOUT_PinSet(channelOneFlash, ledBlinkState);	
		MIOS_DOUT_PinSet(channelTwoFlash, ledBlinkState);	
		MIOS_DOUT_PinSet(channelThreeFlash, ledBlinkState);	
		MIOS_DOUT_PinSet(channelFourFlash, ledBlinkState);							
*/		
		
		
/*
		MIOS_DOUT_PinSet(39, ledBlinkState);	
		MIOS_DOUT_PinSet(37, ledBlinkState);	
		MIOS_DOUT_PinSet(35, ledBlinkState);	
		MIOS_DOUT_PinSet(33, ledBlinkState);							
*/		
	}
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
unsigned char toggleLoop =0;
unsigned char toggleLoopFollow =0;

void DIN_NotifyToggle(unsigned char pin, unsigned char pin_value) __wparam
{

    //set the loop for Mackie Control which is note D6 = 86 
	
  if (pin == 6){
	//pin = 86;  
	pin = 35;
	
	//led pin =77
	toggleLoop = MIOS_DOUT_PinGet(77);
	if (toggleLoop == 1 && pin_value == 0){
		MIOS_DOUT_PinSet(77, 0);
		//toggleLoop = 0;	
		loopButton('0',0);
	}
	else{
		if (pin_value == 0){
			MIOS_DOUT_PinSet(77, 1);	
			//toggleLoop = 1;
			loopButton('1',0);
		}
	}
	
  }
  
  // *** CHANNEL SELECT **** MAKE ME A SWITCH AND A LOT OF PROBLEMS WILL DISAPEAR//
  //select channel 1 - note C1 = 24
  else if (pin == 98){
   pin = 24;
  }
  //select channel 2
  else if (pin == 121){
   pin = 25;
  }
  //select channel 3
  else if (pin == 105){
   pin = 26;
  }    
  //select channel 4
  else if (pin == 113){
   pin = 27;
  }  
  
  // *** SOLO / CUE SELECT **** //
  //SOLO/CUE channel 1 - note G# 1 = 40
  else if (pin == 102){
   pin = 8;
   //pin = 30;
  }
  //SOLO/CUE channel 2
  else if (pin == 126){
   pin = 9;
  }
  //SOLO/CUE channel 3
  else if (pin == 110){
   pin = 10;
  }
  //SOLO/CUE channel 4
  else if (pin == 118){
   pin = 11;
  }
  
  // ***** MASTER SECTION CONTROLS *********
  //STOP - A 6 = 
  else if (pin == 19){
   pin = 93;
  } 
  //PLAY - A# 6 = 
  else if (pin == 17){
   pin = 94;
  }   
  //REC - B 6 = 
  else if (pin == 18){
   pin = 95;
  }      
  
  
  //cursor UP - C 7
  else if (pin == 22){
   pin = 96;
  }  
  //cursor DOWN - C# 7
  else if (pin == 3){
   pin = 97;
  }    
  //cursor LEFT - E 7
  else if (pin == 0){
   pin = 98;
  }
  //MIDDLE BUTTON-ZOOM - D# 7
  else if (pin == 23){
   pin = 100;
  }  
  //cursor RIGHT - E 7
  else if (pin == 1){
   pin = 99;
  }  
  
  
  //View Clip/FX - D# 5 
  else if (pin == 24){
   pin = 75;
  }  
  
  //Clip Waveform Follow - labeled on controller as SCN
  else if (pin == 16){
   pin = 83;
   
   //hack the LED stuff
	if (toggleLoopFollow == 1 && pin_value == 0){
		MIOS_DOUT_PinSet(74, 0);
		toggleLoopFollow = 0;	
	}
	else{
		if (pin_value == 0){
			MIOS_DOUT_PinSet(74, 1);	
			toggleLoopFollow = 1;
		}
	}   
   
  }    
  
  
  //** misc hacks *** /
  //CHANNEL ONE clip Trigger 5
  else if (pin == 100){
   pin = 56;
  }  
  //CHANNEL ONE clip Trigger 1
  else if (pin == 97){
   pin = 17;
  }    
  //CHANNEL ONE clip Trigger 2
  else if (pin == 99){
   pin = 18;
  } 
  //CHANNEL ONE STOP
  else if (pin == 96){
   pin = 91;
  } 
  //CHANNEL TWO - FX4 
  else if (pin == 51){
  	pin = 76;
  }     
  //CHANNEL ONE - FX3
  else if (pin == 39){
  	pin = 78;
  }
  //Channel Three - FX1
  else if (pin == 60){
  	pin = 64;
  }
  
  //master section POTS
  //fx1	   
  else if (pin == 67){
   pin = 12;
  }
  //fx2	   
  else if (pin == 66){
   pin = 65;
  }  
  //fx3	   
  else if (pin == 64){
   pin = 13;
  }  
  //fx4	   
  else if (pin == 65){
   pin = 67;
  }
  //fx5
  else if (pin == 69){
   pin = 14;
  }
  //fx6
  else if (pin == 68){
   pin = 69;
  }
  //fx7
  else if (pin == 70){
   pin = 15;
  }
  //fx8
  else if (pin == 71){
   pin = 72;
  }
  
  //push and pull bpm controlls
  
  //Tap Tempo
  else if(pin == 9){
  	pin = 79;
  }
  //Nudge <
  else if(pin == 13){
  	pin = 80;
  }
  //Nudge >
  else if(pin == 8){
  	pin = 81;
  }
  //BPM +
  else if(pin == 11){
  	pin = 82;
  }
  //BPM -
  else if(pin == 10){
  	pin = 85;
  }
  
  

  // a button has been pressed, send Note at channel 1
  MIOS_MIDI_BeginStream();
  MIOS_MIDI_TxBufferPut(0x90); // Note at channel 1
  MIOS_MIDI_TxBufferPut(pin);  // pin number corresponds to note number
  MIOS_MIDI_TxBufferPut(pin_value ? 0x00 : 0x7f); // buttons are high-active
  MIOS_MIDI_EndStream();
	
  // notify display handler in DISPLAY_Tick() that DIN value has changed
  last_din_pin = pin;
  app_flags.DISPLAY_UPDATE_REQ = 1;  
  
  
  
  
  
  
  
  
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS when an encoder has been moved
// incrementer is positive when encoder has been turned clockwise, else
// it is negative
/////////////////////////////////////////////////////////////////////////////
void ENC_NotifyChange(unsigned char encoder, char incrementer,unsigned int encValue) __wparam
{
	//Quant ENCODER
	if (encoder == 0) {
		if (incrementer == 1) {
			enc1 = enc1 + 12;
		}
		else {
			enc1 = enc1 - 12;
		}
		if (enc1 < 4) {
			enc1=123;
		}	
		if (enc1 > 127) {
			enc1=1;
		}
	
		encValue = enc1;
		
		
		MIOS_MIDI_BeginStream();
		MIOS_MIDI_TxBufferPut(0xb0); // CC at channel 1
  		MIOS_MIDI_TxBufferPut(encoder);  // pin number corresponds to CC number
  		MIOS_MIDI_TxBufferPut(encValue); // don't send 10bit pin_value, but 7bit value
  		MIOS_MIDI_EndStream();
		
	}
	//Length ENCODER
	if (encoder == 1) {
		if (incrementer == 1) {
			enc1=1;
		}
		else {
			enc1=65;;
		}
		encValue = enc1;
		
		MIOS_MIDI_BeginStream();
		MIOS_MIDI_TxBufferPut(0xb1); // CC at channel 2
  		MIOS_MIDI_TxBufferPut(encoder);  // pin number corresponds to CC number
  		MIOS_MIDI_TxBufferPut(encValue); // don't send 10bit pin_value, but 7bit value
  		MIOS_MIDI_EndStream();
	}	
	//JOG WHEEL
	if (encoder == 2) {
		if (incrementer == 1) {
			enc2 = enc2 + 1;
		}
		else {
			enc2 = enc2 - 1;
		}
		if (enc2 > 127) {
			enc2=1;
		}
		if (enc2 < 1) {
			enc2=127;
		}
		encValue = enc2;
		
		MIOS_MIDI_BeginStream();
		
		//controller 60 at channel 1
		MIOS_MIDI_TxBufferPut(0xb0); // CC at channel 1
  		MIOS_MIDI_TxBufferPut(60);  // pin number corresponds to CC number
  		MIOS_MIDI_TxBufferPut(encValue); // don't send 10bit pin_value, but 7bit value
  		MIOS_MIDI_EndStream();
		
		
		
	}


	

	
	
	
	// clear screen
	  MIOS_LCD_Clear();
	//TELL US THE ENCODER HAS MOVED
		MIOS_LCD_PrintCString("encoder moved");
		  MIOS_LCD_CursorSet(0x40); // second line
  		MIOS_LCD_PrintBCD1(encValue);
		MIOS_LCD_PrintBCD2(encoder);
		MIOS_LCD_PrintBCD3(incrementer);
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS when a pot has been moved
/////////////////////////////////////////////////////////////////////////////
void AIN_NotifyChange(unsigned char pin, unsigned int pin_value) __wparam
{

//fader channel ONE
if (pin == 0){
  MIOS_MIDI_BeginStream();
  MIOS_MIDI_TxBufferPut(0xE0); // PitchBend at channel 1
  MIOS_MIDI_TxBufferPut(pin);  // pin number corresponds to CC number
  MIOS_MIDI_TxBufferPut(MIOS_AIN_Pin7bitGet(pin)); // don't send 10bit pin_value, but 7bit value
  MIOS_MIDI_EndStream();
}
//fader channel TWO
else if(pin == 16){
  MIOS_MIDI_BeginStream();
  MIOS_MIDI_TxBufferPut(0xE1); // PitchBend at channel 2
  MIOS_MIDI_TxBufferPut(pin);  // pin number corresponds to CC number
  MIOS_MIDI_TxBufferPut(MIOS_AIN_Pin7bitGet(pin)); // don't send 10bit pin_value, but 7bit value
  MIOS_MIDI_EndStream();

}
//fader channel THREE
else if(pin == 24){
  MIOS_MIDI_BeginStream();
  MIOS_MIDI_TxBufferPut(0xE2); // PitchBend at channel 2
  MIOS_MIDI_TxBufferPut(pin);  // pin number corresponds to CC number
  MIOS_MIDI_TxBufferPut(MIOS_AIN_Pin7bitGet(pin)); // don't send 10bit pin_value, but 7bit value
  MIOS_MIDI_EndStream();

}
//fader channel FOUR
else if(pin == 8){
  MIOS_MIDI_BeginStream();
  MIOS_MIDI_TxBufferPut(0xE3); // PitchBend at channel 2
  MIOS_MIDI_TxBufferPut(pin);  // pin number corresponds to CC number
  MIOS_MIDI_TxBufferPut(MIOS_AIN_Pin7bitGet(pin)); // don't send 10bit pin_value, but 7bit value
  MIOS_MIDI_EndStream();

}
//channel TWO - FX4
else if(pin == 17){
  // a pot has been moved, send CC#<pin-number> at channel 2
  MIOS_MIDI_BeginStream();
  MIOS_MIDI_TxBufferPut(0xb1); // CC at channel 1
  MIOS_MIDI_TxBufferPut(pin);  // pin number corresponds to CC number
  MIOS_MIDI_TxBufferPut(MIOS_AIN_Pin7bitGet(pin)); // don't send 10bit pin_value, but 7bit value
  MIOS_MIDI_EndStream();

}
else{
  // a pot has been moved, send CC#<pin-number> at channel 1
  MIOS_MIDI_BeginStream();
  MIOS_MIDI_TxBufferPut(0xb0); // CC at channel 1
  MIOS_MIDI_TxBufferPut(pin);  // pin number corresponds to CC number
  MIOS_MIDI_TxBufferPut(MIOS_AIN_Pin7bitGet(pin)); // don't send 10bit pin_value, but 7bit value
  MIOS_MIDI_EndStream();
 }
 
  // notify display handler in DISPLAY_Tick() that AIN value has changed
  last_ain_pin = pin;
  app_flags.DISPLAY_UPDATE_REQ = 1;
}
