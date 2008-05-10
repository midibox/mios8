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
#include "mm_dio.h"
#include "mm_dio_table.h"
#include "mm_vpot.h"
#include "mm_gpc.h"

/////////////////////////////////////////////////////////////////////////////
// Local Variables
/////////////////////////////////////////////////////////////////////////////

// the status of the 256 LED functions (each byte contains the status of 8 functions)
unsigned char led_status[256/8];


/////////////////////////////////////////////////////////////////////////////
// This function is called from the DIN_NotifyToggle hook in main.c
// when a button has been pressed/depressed
/////////////////////////////////////////////////////////////////////////////
void MM_DIO_ButtonHandler(unsigned char button, unsigned char pin_value)
{
  unsigned char button_id;
  unsigned char third_byte, fifth_byte;

  // read button ID from table
  button_id = mm_flags.LAYER_SEL ? mm_dio_table_layer1[button << 1] : mm_dio_table_layer0[button << 1];

  // branch to special function handler if id >= 0x80
  if( button_id >= 0x80 ) {
    MM_DIO_SFBHandler(button_id, pin_value);
    return;
  }

#if TOUCH_SENSOR_MODE >= 1
  // if note event matches with any ID_FADER_TOUCH_CHNx, use the MIOS function
  // to suspend the faders
#if    ID_FADER_TOUCH_CHN1 != 0x00 || ID_FADER_TOUCH_CHN2 != 0x01 || ID_FADER_TOUCH_CHN3 != 0x02 || ID_FADER_TOUCH_CHN4 != 0x03
  .error "inconsistency in ID_FADER_TOUCH_CHNx - the TOUCH_SENSOR_MODE won't work properly!";
#elif ID_FADER_TOUCH_CHN5 != 0x04 || ID_FADER_TOUCH_CHN6 != 0x05 || ID_FADER_TOUCH_CHN7 != 0x06 || ID_FADER_TOUCH_CHN8 != 0x07
  .error "inconsistency in ID_FADER_TOUCH_CHNx - the TOUCH_SENSOR_MODE won't work properly!";
#endif

  if( (button_id & 0xf8) == ID_FADER_TOUCH_CHN1 ) {
    if( pin_value )
      MIOS_MF_SuspendDisable(button_id & 0x07);
    else
      MIOS_MF_SuspendEnable(button_id & 0x07);
  }
#endif

  // branch depending on button ID range

  // 0x00..0x57 are defined in mm_dio_table.h as MM events
  if( button_id < 0x58 ) {

    // select third and fith byte depending on button id
    fifth_byte = button_id & 0x07;

    if( button_id <= 0x2f ) {        // Range #1: Fader/Select/Mute/Solo/Multi/RecRdy
      third_byte = button_id & 0x07;
      fifth_byte = (button_id >> 3) & 0x07;
    } else if( button_id <= 0x37 ) { // Range #2: 0x08 group
      third_byte = 0x08;
    } else if( button_id <= 0x3f ) { // Range #2: 0x09 group
      third_byte = 0x09;
    } else if( button_id <= 0x4f ) { // Range #3: 0x0a group
      third_byte = 0x0a;
    } else {                         // Range #3: 0x0b group
      third_byte = 0x0b;
    }

    MIOS_MIDI_BeginStream();
    MIOS_MIDI_TxBufferPut(0xb0 | ((MM_MIDI_CHANNEL-1) & 0x0f));
    MIOS_MIDI_TxBufferPut(0x0f);
    MIOS_MIDI_TxBufferPut(third_byte);
    MIOS_MIDI_TxBufferPut(0x2f);
    MIOS_MIDI_TxBufferPut(fifth_byte | (pin_value ? 0x00 : 0x40));
    MIOS_MIDI_EndStream();

  // 0x58..0x7f could be used for other purposes
  } else if( button_id < 0x80 ) {
    // nothing to do (yet)
  }
}


/////////////////////////////////////////////////////////////////////////////
// This function is called from MM_DIO_ButtonHandler if a special function
// button has been pressed/depressed
/////////////////////////////////////////////////////////////////////////////
void MM_DIO_SFBHandler(unsigned char button_id, unsigned char pin_value)
{
  unsigned char gpc_hold_flag = 0;

  // set various flags depending on the IDs which are defined in mm_dio_tables.h
  switch( button_id ) {
    case ID_MBMM_SWITCH_LAYER0:
      mm_flags.LAYER_SEL = 0;
      break;

    case ID_MBMM_SWITCH_LAYER1:
      mm_flags.LAYER_SEL = 1;
      break;

    case ID_MBMM_TOGGLE_LAYER:
      if( !pin_value ) {
	mm_flags.LAYER_SEL = !mm_flags.LAYER_SEL;
      }
      break;

    case ID_MBMM_HOLD_LAYER:
      mm_flags.LAYER_SEL = pin_value ? 0 : 1;
      break;

    case ID_MBMM_SWITCH_MM:
      mm_flags.GPC_SEL = 0;
      break;

    case ID_MBMM_SWITCH_GPC:
      mm_flags.GPC_SEL = 1;
      break;

    case ID_MBMM_TOGGLE_GPC:
      if( !pin_value ) {
	mm_flags.GPC_SEL = !mm_flags.GPC_SEL;
      }
      break;

    case ID_MBMM_HOLD_GPC:
      mm_flags.GPC_SEL = pin_value ? 0 : 1;
      gpc_hold_flag = 1;
      break;

  }

  // request message message, LED and ledring update
  mm_flags.MSG_UPDATE_REQ = 1;
  mm_flags.LED_UPDATE_REQ = 1;
  MM_VPOT_LEDRingUpdateSet(0xff);

  // optional: send MIDI event which notifies if GPC mode is activated or not
#if GPC_BUTTON_SENDS_MIDI_EVENT
  if( !pin_value || gpc_hold_flag ) {
    MIOS_MIDI_BeginStream();
    MIOS_MIDI_TxBufferPut(mm_flags.GPC_SEL ? (GPC_BUTTON_ON_EVNT0) : (GPC_BUTTON_OFF_EVNT0));
    MIOS_MIDI_TxBufferPut(mm_flags.GPC_SEL ? (GPC_BUTTON_ON_EVNT1) : (GPC_BUTTON_OFF_EVNT1));
    MIOS_MIDI_TxBufferPut(mm_flags.GPC_SEL ? (GPC_BUTTON_ON_EVNT2) : (GPC_BUTTON_OFF_EVNT2));
    MIOS_MIDI_EndStream();
  }
#endif

  // update message if button pressed and GPC selected
  if( !pin_value && mm_flags.GPC_SEL )
    MM_GPC_Msg_Update();

  // update the special function LEDs depending on the new flags:
  MM_DIO_SFBLEDUpdate();
}

/////////////////////////////////////////////////////////////////////////////
// This function updates all LEDs which have been assigned to SFBs
/////////////////////////////////////////////////////////////////////////////
void MM_DIO_SFBLEDUpdate(void)
{
  // add IDs >= 0x80 must be handled here!
  MM_DIO_LEDSet(ID_MBMM_SWITCH_LAYER0, !mm_flags.LAYER_SEL);
  MM_DIO_LEDSet(ID_MBMM_SWITCH_LAYER1,  mm_flags.LAYER_SEL);
  MM_DIO_LEDSet(ID_MBMM_TOGGLE_LAYER,   mm_flags.LAYER_SEL);
  MM_DIO_LEDSet(ID_MBMM_HOLD_LAYER,     mm_flags.LAYER_SEL);

  MM_DIO_LEDSet(ID_MBMM_SWITCH_MM,     !mm_flags.GPC_SEL);
  MM_DIO_LEDSet(ID_MBMM_SWITCH_GPC,     mm_flags.GPC_SEL);
  MM_DIO_LEDSet(ID_MBMM_TOGGLE_GPC,     mm_flags.GPC_SEL);
  MM_DIO_LEDSet(ID_MBMM_HOLD_GPC,       mm_flags.GPC_SEL);
}

/////////////////////////////////////////////////////////////////////////////
// This function is called from the Tick() function in main.c when 
// all LEDs should be updated
/////////////////////////////////////////////////////////////////////////////
static unsigned char led_ctr;     // externally defined to simplify access via assembler
static unsigned char led_id;      // externally defined to simplify access via assembler
void MM_DIO_LED_CheckUpdate(void)
{

  if( mm_flags.LED_UPDATE_REQ ) {
    mm_flags.LED_UPDATE_REQ = 0;

    // assembler optimized code
    if( !mm_flags.LAYER_SEL ) {
__asm
	movlw	LOW(_mm_dio_table_layer0)
	movwf	_TBLPTRL
	movlw	HIGH(_mm_dio_table_layer0)
	movwf	_TBLPTRH
__endasm;
    } else {
__asm
	movlw	LOW(_mm_dio_table_layer1)
	movwf	_TBLPTRL
	movlw	HIGH(_mm_dio_table_layer1)
	movwf	_TBLPTRH
__endasm;
    }

__asm
	extern	_MIOS_DOUT_PinSet0
	extern	_MIOS_DOUT_PinSet1
	
	clrf	_led_ctr
MM_DIO_LED_CheckUpdate_Loop:
	tblrd*+				; read (and ignore) button entry
	tblrd*+				; read LED entry

	;; dont change LED if TABLAT is ID_IGNORE == 0xff
	incf	_TABLAT, W
	bz	MM_DIO_LED_CheckUpdate_Next

	;; get pointer to LC function
	lfsr	1, _led_status          ; FSR1 --- NOTE that we have to use FSR0 register names instead of FSR1 due to MIOS wrapper!
	rrcf	_TABLAT, W
	rrcf	WREG, W
	rrcf	WREG, W
	andlw	0x1f
	addwf	_FSR0L, F

	;; branch depending on saved status

	;; ---
	;; like GetBitORMask, but much faster!
	btfsc	_TABLAT, 2
	bra	MM_DIO_LED_CheckUpdate_OR4567
MM_DIO_LED_CheckUpdate_OR0123:
	btfsc	_TABLAT, 1
	bra	MM_DIO_LED_CheckUpdate_OR23
MM_DIO_LED_CheckUpdate_OR01:
	movlw	(1 << 0)
	btfsc	_TABLAT, 0
	movlw	(1 << 1)
	bra	MM_DIO_LED_CheckUpdate_OR_Cont
MM_DIO_LED_CheckUpdate_OR23:
	movlw	(1 << 2)
	btfsc	_TABLAT, 0
	movlw	(1 << 3)
	bra	MM_DIO_LED_CheckUpdate_OR_Cont
	
MM_DIO_LED_CheckUpdate_OR4567:
	btfsc	_TABLAT, 1
	bra	MM_DIO_LED_CheckUpdate_OR67
MM_DIO_LED_CheckUpdate_OR45:
	movlw	(1 << 4)
	btfsc	_TABLAT, 0
	movlw	(1 << 5)
	bra	MM_DIO_LED_CheckUpdate_OR_Cont
MM_DIO_LED_CheckUpdate_OR67:
	movlw	(1 << 6)
	btfsc	_TABLAT, 0
	movlw	(1 << 7)
	;; 	bra	MM_DIO_LED_CheckUpdate_OR_Cont
MM_DIO_LED_CheckUpdate_OR_Cont:
	;; ---
	
	andwf	_INDF0, W
	bz	MM_DIO_LED_CheckUpdate_LEDOff
MM_DIO_LED_CheckUpdate_LEDOn:
	movf	_led_ctr, W
	call	_MIOS_DOUT_PinSet1
	bra	MM_DIO_LED_CheckUpdate_Next

MM_DIO_LED_CheckUpdate_LEDOff:
	movf	_led_ctr, W
	call	_MIOS_DOUT_PinSet0
	;; 	rgoto	MM_DIO_LED_CheckUpdate_Next

MM_DIO_LED_CheckUpdate_Next:
	;; increment loop counter
	incf	_led_ctr, F
	;; loop until MM_LED_LOOP_CTR reached 0x80
	btfss	_led_ctr, 7
	bra	MM_DIO_LED_CheckUpdate_Loop
__endasm;
  }
}


/////////////////////////////////////////////////////////////////////////////
// This function turns on/off all LEDs which are assigned to mm_id
/////////////////////////////////////////////////////////////////////////////
void MM_DIO_LEDSet(unsigned char mm_id, unsigned char led_on) 
{
  unsigned char ix = mm_id >> 3;

  // note that led_status is a packed array, each byte contains the status of 8 LEDs
  if( led_on ) {
    led_status[ix] |= (1 << (mm_id & 0x07));
  } else {
    led_status[ix] &= ~(1 << (mm_id & 0x07));
  }

  // request update of all LEDs
  mm_flags.LED_UPDATE_REQ = 1;
}


/////////////////////////////////////////////////////////////////////////////
// This function is called from MM_MIDI_Received in mm_midi.c when
// a LED status message has been received
/////////////////////////////////////////////////////////////////////////////
const char mm_id_map_0a[8] = { ID_BANK, ID_GROUP, ID_RECRDY_x_FUNCTA, ID_FUNCTA_LED, ID_WRITE_x_FUNCTB, ID_FUNCTB_LED, ID_OTHER_x_FUNCTC, ID_FUNCTC_LED };
const char mm_id_map_0b[8] = { ID_FX_BYPASS_x_EFFECT1, ID_EFFECT1_LED, ID_SEND_MUTE_x_EFFECT2, ID_EFFECT2_LED, ID_PRE_POST_x_EFFECT3, ID_EFFECT3_LED, ID_SELECT_x_EFFECT4, ID_EFFECT4_LED };

void MM_DIO_LEDHandler(unsigned char led_id_l, unsigned char led_id_h)
{
  unsigned char mm_id;
  unsigned char ix = led_id_l & 0x07;

  // map LSB/MSB to MIDIbox MM ID
  if( led_id_h <= 0x07 ) {
    mm_id = ((led_id_l << 3) & 0x38) | led_id_h;
  } else if( led_id_h == 0x08 ) {
    mm_id = ix | 0x30;
  } else if( led_id_h == 0x09 ) {
    mm_id = ix | 0x38;
  } else if( led_id_h == 0x0a ) {
    mm_id = mm_id_map_0a[ix];
  } else if( led_id_h == 0x0b ) {
    mm_id = mm_id_map_0b[ix];
  }

  // set LED status (bit #6 set: LED on)
  MM_DIO_LEDSet(mm_id, led_id_l & (1 << 6 ));
}
