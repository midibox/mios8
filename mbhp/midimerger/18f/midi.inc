; $Id: $
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
	btfss	TXSTA, TRMT
	bra _MIDI_SendByte
	movwf	TXREG
	return

;; --------------------------------------------------------------------------
;;  MIDI GetCMax: get number of expected MIDI bytes depending on MIDI status
;;  In:	Status byte in W
;; --------------------------------------------------------------------------
MIDI_GetCMax
	movwf	TABLAT
	swapf	TABLAT, W
	andlw	0x07
	bz	MIDI_GetCMax_8n
	addlw	-1
	bz	MIDI_GetCMax_9n
	addlw	-1
	bz	MIDI_GetCMax_An
	addlw	-1
	bz	MIDI_GetCMax_Bn
	addlw	-1
	bz	MIDI_GetCMax_Cn
	addlw	-1
	bz	MIDI_GetCMax_Dn
	addlw	-1
	bz	MIDI_GetCMax_En
MIDI_GetCMax_Fx
	movf	TABLAT, W
	andlw	0xfe		; 0xf0 and 0xf1: expected bytes = 1
	xorlw	0xf0
	skpnz
	retlw	0x01

	movf	TABLAT, W	; 0xf2:	expected bytes = 2
	xorlw	0xf2
	skpnz
	retlw	0x02

	retlw	0x00		; others: expected bytes = 0

	
MIDI_GetCMax_8n
	retlw	0x02
MIDI_GetCMax_9n
	retlw	0x02
MIDI_GetCMax_An
	retlw	0x02
MIDI_GetCMax_Bn
	retlw	0x02
MIDI_GetCMax_Cn
	retlw	0x01
MIDI_GetCMax_Dn
	retlw	0x01
MIDI_GetCMax_En
	retlw	0x02
