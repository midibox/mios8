// $Id$
/*
 * MIDI Router project
 *
 * ==========================================================================
 *
 *  Copyright 2006 Thorsten Klose (tk@midibox.org)
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

#include <midi_rxtx_leds.h>
#include <iic_midi.h>

#include "main.h"
#include "iic_midi_tick.h"
#include "int_midi.h"
#include "router.h"

/////////////////////////////////////////////////////////////////////////////
// Global variables
/////////////////////////////////////////////////////////////////////////////

// status of application (see bitfield declaration in main.h)
app_flags_t app_flags;

// selected menu item
unsigned char menu_enc_item;

// for panic function: armed when any button pressed, dearmed when any button released
unsigned char panic_ctr;

/////////////////////////////////////////////////////////////////////////////
// Local variables
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// Application specific encoder table
// the default (dummy) table has been disabled via 
/////////////////////////////////////////////////////////////////////////////
MIOS_ENC_TABLE {
             // sr pin mode
  MIOS_ENC_ENTRY(1, 0, MIOS_ENC_MODE_DETENTED),
  MIOS_ENC_EOT
};


/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS after startup to initialize the 
// application
/////////////////////////////////////////////////////////////////////////////
void Init(void) __wparam
{
  // initialize handlers
  INT_MIDI_Init();
  IIC_MIDI_Init();
  ROUTER_Init();

  // initialize the shift registers
  MIOS_SRIO_NumberSet(1);
  MIOS_SRIO_UpdateFrqSet(3);
  MIOS_SRIO_DebounceSet(0x20);

  // encoder: always slow speed (adapt this for your personal needs)
  MIOS_ENC_SpeedSet(0, MIOS_ENC_SPEED_SLOW, 2);

  // initialize menu specific values
  menu_enc_item = 0;
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS in the mainloop when nothing else is to do
/////////////////////////////////////////////////////////////////////////////
void Tick(void) __wparam
{
  // check for incoming packages from the IIC slaves
  IIC_MIDI_Tick();

  // check for panic request
  if( app_flags.PANIC_REQ ) {
    unsigned char chn, key;

    app_flags.PANIC_REQ = 0;

    // print tmp. message on screen
    MIOS_LCD_Clear();
    MIOS_LCD_PrintCString("*** PANIC !!! ***");
    MIOS_LCD_MessageStart(0xff);

    // send Note Off to MIDIbox SID, Channel 9-12
    MIOS_LCD_CursorSet(0x40);
    MIOS_LCD_PrintCString("Sending Note Off to MIDIbox SID, Chn#");
    for(chn=0x08; chn<=0x0b; ++chn) {
      MIOS_LCD_CursorSet(0x40 + 37);
      MIOS_LCD_PrintBCD2(chn+1);
__asm
      clrwdt     ; feed watchdog
__endasm;
      for(key=0; key<128; ++key) {
	ROUTER_Tx_IIC0(0x8, 0x80 | chn, key, 0);
      }
    }

    // send Note Off to MIDIbox FM, Channel 13-16
    MIOS_LCD_CursorSet(0x40);
    MIOS_LCD_PrintCString("Sending Note Off to MIDIbox FM, Chn#");
    for(chn=0x0c; chn<=0x0f; ++chn) {
      MIOS_LCD_CursorSet(0x40 + 36);
      MIOS_LCD_PrintBCD2(chn+1);
      MIOS_LCD_PrintChar(' ');
__asm
      clrwdt     ; feed watchdog
__endasm;
      for(key=0; key<128; ++key) {
	ROUTER_Tx_IIC1(0x8, 0x80 | chn, key, 0);
      }
    }

    // send Note Off to MIDIbox SEQ, Channel 1
    MIOS_LCD_CursorSet(0x40);
    MIOS_LCD_PrintCString("Sending Note Off to MIDIbox SEQ, Chn#");
    for(chn=0x00; chn<=0x00; ++chn) {
      MIOS_LCD_CursorSet(0x40 + 37);
      MIOS_LCD_PrintBCD2(chn+1);
__asm
      clrwdt     ; feed watchdog
__endasm;
      for(key=0; key<128; ++key) {
	ROUTER_Tx_IIC2(0x8, 0x80 | chn, key, 0);
      }
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
  //                     <-------------------------------------->
  MIOS_LCD_PrintCString("A:SEQ->SID/FM off   B:K.Chn off  C:Tr +0");
  MIOS_LCD_CursorSet(0x40);
  //                     <-------------------------------------->
  MIOS_LCD_PrintCString("D:K->SID off  E:K->FM off  F:K->SEQ off ");

  // request display update
  app_flags.DISPLAY_UPDATE_REQ = 1;
}

/////////////////////////////////////////////////////////////////////////////
//  This function is called in the mainloop when no temporary message is shown
//  on screen. Print the realtime messages here
/////////////////////////////////////////////////////////////////////////////
void DISPLAY_Tick(void) __wparam
{
  // only continue if display update has been requested
  if( !app_flags.DISPLAY_UPDATE_REQ )
    return;
  app_flags.DISPLAY_UPDATE_REQ = 0;

  // IIC2 forwarding to MBSID/MBFM on/off
  MIOS_LCD_CursorSet(0x00 + 14);
  if( router_flags.IIC2_FWD_CHN8_16 ) {
    MIOS_LCD_PrintCString("on ");
  } else {
    MIOS_LCD_PrintCString("off");
  }

  // print IIC3 forwarding channel
  MIOS_LCD_CursorSet(0x00 + 28);
  if( router_flags.IIC3_FWD_FORCE_CHN ) {
    MIOS_LCD_PrintBCD2(router_iic3_fwd_force_channel+1);
    MIOS_LCD_PrintChar(' ');
  } else {
    MIOS_LCD_PrintCString("off");
  }

  // print IIC3 transpose
  MIOS_LCD_CursorSet(0x00 + 38);
  if( router_iic3_fwd_transpose >= 8 ) {
    MIOS_LCD_PrintChar('+');
    MIOS_LCD_PrintBCD1(router_iic3_fwd_transpose-8);
  } else {
    MIOS_LCD_PrintChar('-');
    MIOS_LCD_PrintBCD1(8-router_iic3_fwd_transpose);
  }

  // IIC3 forwarding to MBSID on/off
  MIOS_LCD_CursorSet(0x40 + 9);
  if( router_flags.IIC3_FWD_MBSID ) {
    MIOS_LCD_PrintCString("on ");
  } else {
    MIOS_LCD_PrintCString("off");
  }

  // IIC3 forwarding to MBFM on/off
  MIOS_LCD_CursorSet(0x40 + 22);
  if( router_flags.IIC3_FWD_MBFM ) {
    MIOS_LCD_PrintCString("on ");
  } else {
    MIOS_LCD_PrintCString("off");
  }

  // IIC3 forwarding to MBSEQ on/off
  MIOS_LCD_CursorSet(0x40 + 36);
  if( router_flags.IIC3_FWD_MBSEQ ) {
    MIOS_LCD_PrintCString("on ");
  } else {
    MIOS_LCD_PrintCString("off");
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
  // check for incoming packages from the IIC slaves
  IIC_MIDI_Tick();
  // forward byte to internal MIDI port parser
  INT_MIDI_Parse(byte);
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS before the shift register are loaded
/////////////////////////////////////////////////////////////////////////////
void SR_Service_Prepare(void) __wparam
{
  // handle Rx/Tx LED
  MIDI_RXTX_LEDS_Handler();

  // decrement panic counter, request panic once we've reached 0
  if( panic_ctr ) {
    --panic_ctr;
    if( !panic_ctr )
      app_flags.PANIC_REQ = 1;
  }

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
  // exit if button has been released
  if( pin_value ) {
    panic_ctr = 0; // de-arm panic function
    return;
  }

  panic_ctr = 0xff; // arm panic function (see Tick())


  // branch depending on button (pin 0/1 allocated by encoder)
  switch( pin ) {
    case 2: // A
      // encoder modifies channel
      menu_enc_item = 0;
      // toggle IIC2_FWD_CHN8_16 flag
      router_flags.IIC2_FWD_CHN8_16 ^= 1; // toggle on/off
      // request display update
      app_flags.DISPLAY_UPDATE_REQ = 1;
      break;

    case 3: // B
      // encoder modifies channel
      menu_enc_item = 0;
      // allow to change the IIC3 forwarding channel
      router_flags.IIC3_FWD_FORCE_CHN ^= 1; // toggle on/off
      // request display update
      app_flags.DISPLAY_UPDATE_REQ = 1;
      break;

    case 4: // C
      // encoder modifies transpose
      menu_enc_item = 1;
      // request display update
      app_flags.DISPLAY_UPDATE_REQ = 1;
      break;

    case 5: // D
      // encoder modifies channel
      menu_enc_item = 0;
      // toggle IIC3_FWD_MBSID flag
      router_flags.IIC3_FWD_MBSID ^= 1; // toggle on/off
      // request display update
      app_flags.DISPLAY_UPDATE_REQ = 1;
      break;

    case 6: // E
      // encoder modifies channel
      menu_enc_item = 0;
      // toggle IIC3_FWD_MBFM flag
      router_flags.IIC3_FWD_MBFM ^= 1; // toggle on/off
      // request display update
      app_flags.DISPLAY_UPDATE_REQ = 1;
      break;

    case 7: // F
      // encoder modifies channel
      menu_enc_item = 0;
      // toggle IIC3_FWD_MBSEQ flag
      router_flags.IIC3_FWD_MBSEQ ^= 1; // toggle on/off
      // request display update
      app_flags.DISPLAY_UPDATE_REQ = 1;
      break;
  };


}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS when an encoder has been moved
// incrementer is positive when encoder has been turned clockwise, else
// it is negative
/////////////////////////////////////////////////////////////////////////////
void ENC_NotifyChange(unsigned char encoder, char incrementer) __wparam
{
  unsigned int value;

  // branch depending on selected item
  switch( menu_enc_item ) {

    case 0:
      // always enable forwarding when encoder rotated
      router_flags.IIC3_FWD_FORCE_CHN = 1;
      // increment/decrement forward channel for IIC3 (0..15)
      value = (unsigned int)router_iic3_fwd_force_channel;
      if( MIOS_HLP_16bitAddSaturate(incrementer, &value, 15) ) {
	router_iic3_fwd_force_channel = (unsigned char)value;
	// request display update
	app_flags.DISPLAY_UPDATE_REQ = 1;
      }
      break;

    case 1:
      // increment/decrement transpose for IIC3 (-8..+7 = 0..15)
      value = (unsigned int)router_iic3_fwd_transpose;
      if( MIOS_HLP_16bitAddSaturate(incrementer, &value, 15) ) {
	router_iic3_fwd_transpose = (unsigned char)value;
	// request display update
	app_flags.DISPLAY_UPDATE_REQ = 1;
      }
      break;

  }
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS when a pot has been moved
/////////////////////////////////////////////////////////////////////////////
void AIN_NotifyChange(unsigned char pin, unsigned int pin_value) __wparam
{
}

