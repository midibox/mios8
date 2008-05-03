// $Id$
/*
 * IIC MIDI Software Loopback Test
 *
 * See the README.txt for details
 *
 * ==========================================================================
 *
 *  Copyright (C) 2006 Thorsten Klose (tk@midibox.org)
 *  Licensed for personal non-commercial use only.
 *  All other rights reserved.
 * 
 * ==========================================================================
 */

#include <cmios.h>
#include <pic18fregs.h>

#include <iic_midi.h>

#include "main.h"

/////////////////////////////////////////////////////////////////////////////
// Global variables
/////////////////////////////////////////////////////////////////////////////

// status of application (see bitfield declaration in main.h)
app_flags_t app_flags;

unsigned char iic_midi_available_slaves_last;

/////////////////////////////////////////////////////////////////////////////
// Local variables
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS after startup to initialize the 
// application
/////////////////////////////////////////////////////////////////////////////
void Init(void) __wparam
{
  // initialize handlers
  IIC_MIDI_Init();
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS in the mainloop when nothing else is to do
/////////////////////////////////////////////////////////////////////////////
void Tick(void) __wparam
{
  unsigned char slave;

  // check if available slaves have been changed (this should update the screen)
  if( iic_midi_available_slaves != iic_midi_available_slaves_last ) {
    iic_midi_available_slaves_last = iic_midi_available_slaves;
    app_flags.DISPLAY_UPDATE_REQ = 1;
  }

  // forward data of Slave x IN -> Slave x Out
  for(slave=0; slave<4; ++slave) {
    if( IIC_MIDI_Receive(slave) ) {
      IIC_MIDI_Transmit(slave, 
			iic_midi_rx_package[0], 
			iic_midi_rx_package[1], 
			iic_midi_rx_package[2], 
			iic_midi_rx_package[3]);
    }
  }
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
  MIOS_LCD_Clear();
  MIOS_LCD_CursorSet(0x00);
  MIOS_LCD_PrintCString("Sl1 Sl2 Sl3 Sl4");
  app_flags.DISPLAY_UPDATE_REQ = 1;
}

/////////////////////////////////////////////////////////////////////////////
//  This function is called in the mainloop when no temporary message is shown
//  on screen. Print the realtime messages here
/////////////////////////////////////////////////////////////////////////////
void DISPLAY_Tick(void) __wparam
{
  // check for update request
  if( !app_flags.DISPLAY_UPDATE_REQ )
    return;
  app_flags.DISPLAY_UPDATE_REQ = 0;

  // update slave status messages
  MIOS_LCD_CursorSet(0x41);
  MIOS_LCD_PrintChar((iic_midi_available_slaves & 0x01) ? '*' : '-');

  MIOS_LCD_CursorSet(0x45);
  MIOS_LCD_PrintChar((iic_midi_available_slaves & 0x02) ? '*' : '-');

  MIOS_LCD_CursorSet(0x49);
  MIOS_LCD_PrintChar((iic_midi_available_slaves & 0x04) ? '*' : '-');

  MIOS_LCD_CursorSet(0x4d);
  MIOS_LCD_PrintChar((iic_midi_available_slaves & 0x08) ? '*' : '-');
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
void DIN_NotifyToggle(unsigned char pin, unsigned char pin_value) __wparam
{
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
