; $Id: $
;
; Condition Tables
;
; ==========================================================================
;
;  Copyright 2003-2008 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================


;; ==========================================================================
;; get number of expected bytes depending on event type
MIDI_GetCMax
	movwf	TMP1
	swapf	TMP1, W
	andlw	0x07
	addwf	PCL, F
	retlw	0x02		; 0x8n
	retlw	0x02		; 0x9n
	retlw	0x02		; 0xan
	retlw	0x02		; 0xbn
	retlw	0x01		; 0xcn
	retlw	0x01		; 0xdn
	retlw	0x02		; 0xen
				; 0xfx
	movf	TMP1, W
	andlw	0xfe		; 0xf0 and 0xf1: expected bytes = 1
	xorlw	0xf0
	skpnz
	retlw	0x01

	movf	TMP1, W		; 0xf2:	expected bytes = 2
	xorlw	0xf2
	skpnz
	retlw	0x02

	retlw	0x00		; others: expected bytes = 0
