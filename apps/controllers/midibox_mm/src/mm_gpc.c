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
#include "mm_gpc.h"
#include "mm_gpc_lables.h"
#include "mm_vpot.h"
#include "mm_lcd.h"

/////////////////////////////////////////////////////////////////////////////
// Local Variables
/////////////////////////////////////////////////////////////////////////////

unsigned char gpc_offset;               // offset for general purpose controller list
unsigned char gpc_abs_value[128];       // the absolute values of Vpots in GPC mode

unsigned char msg_gpc[128];             // the GPC message

/////////////////////////////////////////////////////////////////////////////
// This function initializes the GPC mode
/////////////////////////////////////////////////////////////////////////////
void MM_GPC_Init(void)
{
  // start at 0 offset
  gpc_offset = 0;

  // update GPC message array
  MM_GPC_Msg_Update();
}

/////////////////////////////////////////////////////////////////////////////
// This function returns the absolute value which is assigned to the given vpot
/////////////////////////////////////////////////////////////////////////////
unsigned char MM_GPC_VPotValueGet(unsigned char vpot)
{
  return gpc_abs_value[gpc_offset + vpot];
}

/////////////////////////////////////////////////////////////////////////////
// This function is called from MPROC_NofiyFoundEvent in main.c 
// when a GPC value has been received via MIDI
/////////////////////////////////////////////////////////////////////////////
void MM_GPC_AbsValue_Received(unsigned char entry, unsigned char value)
{
  // store value
  gpc_abs_value[entry & 0x7f] = value;

  // request LEDring update
  MM_VPOT_LEDRingUpdateSet(1 << entry);

  // in GPC mode: update GPC message and request display refresh
  if( mm_flags.GPC_SEL ) {
    MM_GPC_Msg_UpdateValues();
    mm_flags.MSG_UPDATE_REQ = 1;
  }
}

/////////////////////////////////////////////////////////////////////////////
// This function is called from MM_LCD_Msg_Refresh() in mm_lcd.c
// when a GPC message line should be print
/////////////////////////////////////////////////////////////////////////////
void MM_GPC_Msg_Refresh(unsigned char line) {
  unsigned int i;

  if( line == 0 ) {
    for(i=0x00; i<0x00+40; ++i)
      MM_LCD_Msg_PrintChar(msg_gpc[i], 1);
  } else {
    for(i=0x40; i<0x40+40; ++i)
      MM_LCD_Msg_PrintChar(msg_gpc[i], 1);
  }
}

/////////////////////////////////////////////////////////////////////////////
// This function copies lables and values into the GPC message buffer
/////////////////////////////////////////////////////////////////////////////
void MM_GPC_Msg_Update(void) {
  // assembler optimized
  __asm
	movf	_gpc_offset, W
	mullw	5
	movlw	LOW(_mm_gpc_lables)
	addwf	_PRODL, W
	movwf	_TBLPTRL
	movlw	HIGH(_mm_gpc_lables)
	addwfc	_PRODH, W
	movwf	_TBLPTRH

	;; copy 8 lables to the message buffer
	lfsr	1, _msg_gpc     ; FSR1 --- NOTE that we have to use FSR0 register names instead of FSR1 due to MIOS wrapper!
	clrf	_PRODH		; used as loop counter
MM_GPC_Msg_Update_OuterLoop:
	clrf	_PRODL		; used as loop counter
MM_GPC_Msg_Update_InnerLoop:
	tblrd*+
	movff	_TABLAT, _POSTINC0
	incf	_PRODL, F
	movlw	5-1
	cpfsgt	_PRODL
	bra	MM_GPC_Msg_Update_InnerLoop

	incf	_PRODH, F
	btfss	_PRODH, 3
	bra	MM_GPC_Msg_Update_OuterLoop
	
  __endasm;

  // now copy the values to the second line
  MM_GPC_Msg_UpdateValues(); 
}

/////////////////////////////////////////////////////////////////////////////
// This function copies only the values into the GPC message buffer
/////////////////////////////////////////////////////////////////////////////
void MM_GPC_Msg_UpdateValues(void) 
{
  // assembler optimized
  __asm
	movff	_FSR2L, _POSTDEC1	; save FSR2L
	movff	_FSR2H, _POSTDEC1	; save FSR2H

	lfsr	1, _msg_gpc + 0x40      ; FSR1 --- NOTE that we have to use FSR0 register names instead of FSR1 due to MIOS wrapper!
	lfsr	2, _gpc_abs_value	; FSR0 (NOTE - has to be addressed with FSR1 SFRs due to MIOS wrapper!!!)
	movf	_gpc_offset, W
	addwf	_FSR2L, F
	clrf	_PRODL
MM_GPC_Msg_UpdateValues_Loop:
	movlw	' '			; 1 space
	movwf	_POSTINC0
	movf	_POSTINC2, W		; print GPC value
	rcall	MM_GPC_CopyHex2
	movlw	' '			; 1 space
	movwf	_POSTINC0
	movlw	' '			; 1 space
	movwf	_POSTINC0
	incf	_PRODL, F
	btfss	_PRODL, 3
	bra	MM_GPC_Msg_UpdateValues_Loop
	bra	MM_GPC_Msg_UpdateValues_End

MM_GPC_CopyHex2:
	movwf	_PRODH
	swapf	_PRODH, W
	rcall	MM_GPC_CopyHex1
	movf	_PRODH, W
	;; 	rgoto	MM_GPC_CopyHex1
MM_GPC_CopyHex1:
	andlw	0x0f
	addlw	-0x0a
	skpnc
	addlw	0x07
	addlw	0x3a
	movwf	_POSTINC0
	return

MM_GPC_Msg_UpdateValues_End:
	movff	_PREINC1, _FSR2H	; restore FSR2H
	movff	_PREINC1, _FSR2L	; restore FSR2L
  __endasm; 
}


/////////////////////////////////////////////////////////////////////////////
// This function is called from MM_VPOT_SendENCEvent() in mm_vpot.c
// when GPC mode is activated and a V-Pot has been moved
/////////////////////////////////////////////////////////////////////////////
void MM_GPC_SendENCEvent(unsigned char encoder, char incrementer)
{
  unsigned char entry = gpc_offset + encoder;
  unsigned int value;

  // add incrementer to absolute value
  value = (unsigned int)gpc_abs_value[entry];
  if( MIOS_HLP_16bitAddSaturate(incrementer, &value, 0x7f) ) {
    // store new value
    MM_GPC_AbsValue_Received(entry, (unsigned char)value);

    // send value
    MIOS_MIDI_BeginStream();
    MIOS_MIDI_TxBufferPut(mios_mproc_event_table[2*entry+0]);
    MIOS_MIDI_TxBufferPut(mios_mproc_event_table[2*entry+1]);
    MIOS_MIDI_TxBufferPut((unsigned char)value);
    MIOS_MIDI_EndStream();
  }
}

/////////////////////////////////////////////////////////////////////////////
// This function is called from MM_VPOT_SendJogWheelEvent() in mm_vpot.c
// when GPC mode is activated and the jogwheel has been moved
/////////////////////////////////////////////////////////////////////////////
void MM_GPC_SendJogWheelEvent(char incrementer)
{
  unsigned int value;

  // add incrementer to gpc_offset
  value = (unsigned int)gpc_offset;
  if( MIOS_HLP_16bitAddSaturate(incrementer, &value, 127 - 7) ) {
    // store new offset
    gpc_offset = (unsigned char)value;

    // update screen
    MM_GPC_Msg_Update();
    mm_flags.MSG_UPDATE_REQ = 1;

    // request LEDring update
    MM_VPOT_LEDRingUpdateSet(0xff);
  }
}
