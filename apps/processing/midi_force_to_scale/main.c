// $Id$
/*
 * MIDI Force-To-Scale
 *
 * ==========================================================================
 *
 *  Copyright (C) 2011 Thorsten Klose (tk@midibox.org)
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
#include <debug_msg.h>

#include "scale.h"


/////////////////////////////////////////////////////////////////////////////
// Local Defines
/////////////////////////////////////////////////////////////////////////////
// if 0: any MIDI channel will be converted
// if 1..16: a selected MIDI channel will be converted
#define DEFAULT_MIDI_CHANNEL 0


/////////////////////////////////////////////////////////////////////////////
// Local Variables
/////////////////////////////////////////////////////////////////////////////
static unsigned char selected_scale;
static unsigned char selected_root;
static volatile unsigned char display_update;

static unsigned char last_played_note[128];


/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS after startup to initialize the 
// application
/////////////////////////////////////////////////////////////////////////////
void Init(void) __wparam
{
  unsigned char i;

  // set shift register update frequency
  MIOS_SRIO_UpdateFrqSet(1); // ms

  // maximum number of DIN/DOUT shift registers (1..16)
  MIOS_SRIO_NumberSet(16); // for 128 pins

  // init last played note array
  for(i=0; i<128; ++i)
    last_played_note[i] = 0x80;

  // initial scale/root key
  SCALE_Init(0);
  selected_scale = 2; // Harmonic Minor
  selected_root = 0; // C

  // request initial LCD update
  display_update = 1;
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
  MIOS_LCD_Clear();
}

/////////////////////////////////////////////////////////////////////////////
//  This function is called in the mainloop when no temporary message is shown
//  on screen. Print the realtime messages here
/////////////////////////////////////////////////////////////////////////////
void DISPLAY_Tick(void) __wparam
{
  const char root_name[12*2] = "C C#D D#E F F#G G#A A#B ";
  const char *selected_root_name;

  if( display_update ) {
    display_update = 0;

    MIOS_LCD_CursorSet(0x00);
    MIOS_LCD_PrintCString("Root: ");
    selected_root_name = (const char *)&root_name[2*selected_root];
    MIOS_LCD_PrintChar(*selected_root_name++);
    MIOS_LCD_PrintChar(*selected_root_name);

    MIOS_LCD_CursorSet(0x40);
    if( selected_scale == 0 ) {
      MIOS_LCD_PrintCString("No Scale            ");
    } else {
      MIOS_LCD_PrintCString(SCALE_NameGet(selected_scale-1));
    }
  }
}

/////////////////////////////////////////////////////////////////////////////
//  This function is called by MIOS when a complete MIDI event has been received
/////////////////////////////////////////////////////////////////////////////
void MPROC_NotifyReceivedEvnt(unsigned char evnt0, unsigned char evnt1, unsigned char evnt2) __wparam
{
  unsigned char i;
  unsigned char *last_played_note_ptr;
  // using the played note as index for "last played" array
  unsigned char note_ix = evnt1;
  // extract event type (excluding channel)
  unsigned char event = evnt0 & 0xf0;

  // Note On?
  if( event == 0x90 && evnt2 > 0 ) {
#if DEFAULT_MIDI_CHANNEL
    if( (evnt0 & 0x0f) != (DEFAULT_MIDI_CHANNEL-1) )
      return;
#endif

    // get scaled note
    if( selected_scale )
      SCALE_Note(&evnt1, selected_scale-1, selected_root);

    // same note already played from another key? then send note off and play it again
    last_played_note_ptr = (unsigned char *)&last_played_note[0];
    for(i=0; i<128; ++i, ++last_played_note_ptr)
      if( *last_played_note_ptr == evnt1 ) {
	// forward Note Off event to MIDI port
	MIOS_MIDI_BeginStream();
	MIOS_MIDI_TxBufferPut(evnt0);
	MIOS_MIDI_TxBufferPut(evnt1);
	MIOS_MIDI_TxBufferPut(0x00);
	MIOS_MIDI_EndStream();
	*last_played_note_ptr = 0x80;
      }

    // store note
    last_played_note[note_ix] = evnt1;

    // forward Note On event to MIDI ports
    MIOS_MIDI_BeginStream();
    MIOS_MIDI_TxBufferPut(evnt0);
    MIOS_MIDI_TxBufferPut(evnt1);
    MIOS_MIDI_TxBufferPut(evnt2);
    MIOS_MIDI_EndStream();
    return;
  }

  // Note Off?
  if( event == 0x80 ||
      (event == 0x90 && evnt2 == 0) ) {
#if DEFAULT_MIDI_CHANNEL
    if( (evnt0 & 0x0f) != (DEFAULT_MIDI_CHANNEL-1) )
      return;
#endif

    // map to last played note
    if( last_played_note[note_ix] < 0x80 ) {
      // forward to MIDI ports
      MIOS_MIDI_BeginStream();
      MIOS_MIDI_TxBufferPut(evnt0);
      MIOS_MIDI_TxBufferPut(last_played_note[note_ix]);
      MIOS_MIDI_TxBufferPut(evnt2);
      MIOS_MIDI_EndStream();

      last_played_note[note_ix] = 0x80;
    }

    return;    
  }

  // CC?
  if( event == 0xb0 ) {
#if DEFAULT_MIDI_CHANNEL
    if( (evnt0 & 0x0f) != (DEFAULT_MIDI_CHANNEL-1) )
      return;
#endif

    switch( evnt1 ) {
    case 16:
      if( evnt2 <= SCALE_NumGet() ) {
	selected_scale = evnt2;
	display_update = 1;
      }
      break;
    case 17:
      if( evnt2 < 12 ) {
	selected_root = evnt2;
	display_update = 1;
      }
      break;
    }

    return;
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
  // exit if button has been depressed
  if( pin_value )
    return;

  // inc/dec root/scale
  // functions are assigned to two DINs for higher flexibility with existing control surfaces
  switch( pin ) {
  case 0:
  case 4:
    if( selected_scale < (SCALE_NumGet()+1) )
      ++selected_scale;
    else
      selected_scale = 0;
    break;

  case 1:
  case 5:
    if( selected_scale > 0 )
      --selected_scale;
    else
      selected_scale = SCALE_NumGet();
    break;

  case 2:
  case 6:
    if( selected_root < 12 )
      ++selected_root;
    else
      selected_root = 0;
    break;

  case 3:
  case 7:
    if( selected_root > 0 )
      --selected_root;
    else
      selected_root = 11;
    break;
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
