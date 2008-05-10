// $Id$
/*
 * MIDIbox MM V2 (C version)
 *
 * ==========================================================================
 *
 *  Copyright (C) 2005 Thorsten Klose (tk@midibox.org)
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


#include "main.h"
#include "mm_lcd.h"
#include "mm_sysex.h"
#include "mm_midi.h"
#include "mm_mf.h"
#include "mm_vpot.h"
#include "mm_dio.h"
#include "mm_gpc.h"
#include "mm_leddigits.h"


/////////////////////////////////////////////////////////////////////////////
// Application specific encoder table
// the default (dummy) table has been disabled via -DDONT_INCLUDE_MIOS_ENC_TABLE
/////////////////////////////////////////////////////////////////////////////
MIOS_ENC_TABLE {
             //  sr pin mode
  MIOS_ENC_ENTRY(13, 0, MIOS_ENC_MODE_NON_DETENTED), // V-Pot 1
  MIOS_ENC_ENTRY(13, 2, MIOS_ENC_MODE_NON_DETENTED), // V-Pot 2
  MIOS_ENC_ENTRY(13, 4, MIOS_ENC_MODE_NON_DETENTED), // V-Pot 3
  MIOS_ENC_ENTRY(13, 6, MIOS_ENC_MODE_NON_DETENTED), // V-Pot 4
  MIOS_ENC_ENTRY(14, 0, MIOS_ENC_MODE_NON_DETENTED), // V-Pot 5
  MIOS_ENC_ENTRY(14, 2, MIOS_ENC_MODE_NON_DETENTED), // V-Pot 6
  MIOS_ENC_ENTRY(14, 4, MIOS_ENC_MODE_NON_DETENTED), // V-Pot 7
  MIOS_ENC_ENTRY(14, 6, MIOS_ENC_MODE_NON_DETENTED), // V-Pot 8

  MIOS_ENC_ENTRY(15, 0, MIOS_ENC_MODE_NON_DETENTED), // Jog-Wheel

  MIOS_ENC_EOT
};


/////////////////////////////////////////////////////////////////////////////
// Global variables
/////////////////////////////////////////////////////////////////////////////

// status of MM emulation
mm_flags_t mm_flags;


/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS after startup to initialize the 
// application
/////////////////////////////////////////////////////////////////////////////
void Init(void) __wparam
{
  // initialize application specific variables
  MM_SYSEX_Init();
  MM_VPOT_Init();
  MM_MF_Init();
  MM_GPC_Init();
  MM_LEDDIGITS_Init();
  MM_DIO_SFBLEDUpdate();

  // request display initialization
  mm_flags.DISPLAY_INIT_REQ = 1;

  // initialize the AIN driver
  MIOS_AIN_NumberSet(NUMBER_MOTORFADERS);
  MIOS_AIN_UnMuxed();
  MIOS_AIN_DeadbandSet(AIN_DEADBAND);

#if ENABLE_MOTORDRIVER == 1
  // initialize the MF driver
  MIOS_MF_Enable();
  MIOS_MF_DeadbandSet(MOTORFADERS_DEADBAND);

  MIOS_MF_PWM_DutyCycleUpSet(MOTORFADERS_PWM_DUTY_UP);
  MIOS_MF_PWM_DutyCycleDownSet(MOTORFADERS_PWM_DUTY_DOWN);
  MIOS_MF_PWM_PeriodSet(MOTORFADERS_PWM_PERIOD);
#endif

  // initialize the shift registers
  MIOS_SRIO_NumberSet(NUMBER_SHIFTREGISTERS);
  MIOS_SRIO_UpdateFrqSet(SRIO_UPDATE_FREQUENCY);
  MIOS_SRIO_TS_SensitivitySet(TOUCHSENSOR_SENSITIVITY);
  MIOS_SRIO_DebounceSet(SRIO_DEBOUNCE_CTR);

  // initialize the integrated MIDI merger
  MIOS_MIDI_MergerSet(MIOS_MIDI_MERGER_DISABLED);
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS in the mainloop when nothing else is to do
/////////////////////////////////////////////////////////////////////////////
void Tick(void) __wparam
{
  // handles the update requests for VPOT LEDrings
  MM_VPOT_LEDRing_CheckUpdates();

  // handles the update requests for LEDs
  MM_DIO_LED_CheckUpdate();
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

  // request display update
  mm_flags.MSG_UPDATE_REQ = 1;
}

/////////////////////////////////////////////////////////////////////////////
//  This function is called in the mainloop when no temporary message is shown
//  on screen. Print the realtime messages here
/////////////////////////////////////////////////////////////////////////////
void DISPLAY_Tick(void) __wparam
{
  // check if display initialization has been requested
  if( mm_flags.DISPLAY_INIT_REQ ) {
    mm_flags.DISPLAY_INIT_REQ = 0;
    
    MM_LCD_Init();
  }

  // update screen if requested
  if( mm_flags.MSG_UPDATE_REQ ) {
    mm_flags.MSG_UPDATE_REQ = 0;

    MM_LCD_Msg_Refresh();
  }
}

/////////////////////////////////////////////////////////////////////////////
//  This function is called by MIOS when a complete MIDI event has been received
/////////////////////////////////////////////////////////////////////////////
void MPROC_NotifyReceivedEvnt(unsigned char evnt0, unsigned char evnt1, unsigned char evnt2) __wparam
{
  // forward MIDI event to MIDI handler
  MM_MIDI_Received(evnt0, evnt1, evnt2);
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS when a MIDI event has been received
// which has been specified in the MIOS_MPROC_EVENT_TABLE
/////////////////////////////////////////////////////////////////////////////
void MPROC_NotifyFoundEvent(unsigned entry, unsigned char evnt0, unsigned char evnt1, unsigned char evnt2) __wparam
{
  MM_GPC_AbsValue_Received(entry, evnt2);
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS when a MIDI event has not been completly
// received within 2 seconds
/////////////////////////////////////////////////////////////////////////////
void MPROC_NotifyTimeout(void) __wparam
{
  // finish sysex command on a timeout
  MM_SYSEX_CmdFinished();
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS when a MIDI byte has been received
/////////////////////////////////////////////////////////////////////////////
void MPROC_NotifyReceivedByte(unsigned char byte) __wparam
{
  // forward MIDI byte to SysEx parser
  MM_SYSEX_Parser(byte);
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS before the shift register are loaded
/////////////////////////////////////////////////////////////////////////////
void SR_Service_Prepare(void) __wparam
{
  // sets the LEDring patterns
  MM_VPOT_LEDRing_SRHandler();
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
  // branch to button handler
  MM_DIO_ButtonHandler(pin, pin_value);
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS when an encoder has been moved
// incrementer is positive when encoder has been turned clockwise, else
// it is negative
/////////////////////////////////////////////////////////////////////////////
void ENC_NotifyChange(unsigned char encoder, char incrementer) __wparam
{
  // encoder number 0..7: VPOT
  if( encoder < 8 )
    MM_VPOT_SendENCEvent(encoder, incrementer);
  // encoder number 8: datawheel
  else if( encoder == 8 )
    MM_VPOT_SendJogWheelEvent(incrementer);
  else {
    // here you could handle additional encoders
  }
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS when a pot has been moved
/////////////////////////////////////////////////////////////////////////////
void AIN_NotifyChange(unsigned char pin, unsigned int pin_value) __wparam
{
  MM_MF_FaderEvent(pin, pin_value);
}
