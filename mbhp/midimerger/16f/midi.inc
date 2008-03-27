; $Id$
;
; MIDI Handling
;
; ==========================================================================
;
;  Copyright 2003-2008 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  _MIDI Send Byte: sends byte over MIDI interface
;;  In:	Byte to Send in W
;; --------------------------------------------------------------------------
_MIDI_SendByte
	SWITCHBANK_0_1
_MIDI_SendBytePoll
	GOTO_IFCLR TXSTA, TRMT, _MIDI_SendBytePoll
	SWITCHBANK_1_0
	movwf	TXREG
	return
