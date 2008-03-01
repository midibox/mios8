; $Id$
;
; This file includes helpful functions to handle with common MIDI events
;
;    o MIDI_EVNT_Send:       sends a simple MIDI event
;    o MIDI_EVNT_Print:      prints a simple MIDI event
;    o MIDI_EVNT_PrintShort: prints the hex bytes of the MIDI event
;    o MIDI_EVNT_PrintNote:  prints a Note
;
; See the function headers for further details
;
; Following registers have to be located to free addresses in app_defines.h:
;
;MIDI_EVNT0		EQU	0x017
;MIDI_EVNT1		EQU	0x018
;MIDI_EVNT_VALUE	EQU	0x019
;
; ==========================================================================
; NOTE that this is a modified version for MIDIboxSEQ which forwards
; events to the CV outputs when event status is 0xf.
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================


;; --------------------------------------------------------------------------
;;  FUNCTION: MIDI_EVNT_Send
;;  DESCRIPTION: sends a simple MIDI event
;;  IN: 
;;     o first  MIDI event byte in MIDI_EVNT0
;;     o second MIDI event byte (if applicable) in MIDI_EVNT1
;;     o value in MIDI_EVNT_VALUE
;; --------------------------------------------------------------------------
MIDI_EVNT_Send
	call	MIOS_MIDI_BeginStream

	;; branch depending on MIDI status
	swapf	MIDI_EVNT0, W
	andlw	0x07
	JUMPTABLE_2BYTES_UNSECURE	; 8 entries
	rgoto	MIDI_EVNT_Send_8x	; Note Off
	rgoto	MIDI_EVNT_Send_9x	; Note On
	rgoto	MIDI_EVNT_Send_Ax	; Aftertouch
	rgoto	MIDI_EVNT_Send_Bx	; Controller
	rgoto	MIDI_EVNT_Send_Cx	; Program Change
	rgoto	MIDI_EVNT_Send_Dx	; Channel Pressure
	rgoto	MIDI_EVNT_Send_Ex	; Pitch Bend
	rgoto	MIDI_EVNT_Send_Fx	; AOUT


	;; sending three bytes:	
MIDI_EVNT_Send_8x	; Note Off
MIDI_EVNT_Send_9x	; Note On
MIDI_EVNT_Send_Ax	; Aftertouch
MIDI_EVNT_Send_Bx	; Controller
	movf	MIDI_EVNT0, W
	call	MIOS_MIDI_TxBufferPut
	movf	MIDI_EVNT1, W
	andlw	0x7f
	call	MIOS_MIDI_TxBufferPut
	movf	MIDI_EVNT_VALUE, W
	andlw	0x7f
	call	MIOS_MIDI_TxBufferPut
	rgoto	MIDI_EVNT_Send_End

	;; sending two bytes
MIDI_EVNT_Send_Cx	; Program Change
MIDI_EVNT_Send_Dx	; Channel Pressure
	movf	MIDI_EVNT0, W
	call	MIOS_MIDI_TxBufferPut
	movf	MIDI_EVNT_VALUE, W
	andlw	0x7f
	call	MIOS_MIDI_TxBufferPut
	rgoto	MIDI_EVNT_Send_End

	;; sending three bytes (7bit converted to 14bit)
MIDI_EVNT_Send_Ex	; Pitch Bend
	movf	MIDI_EVNT0, W
	call	MIOS_MIDI_TxBufferPut

	movlw	0x00
	BRA_IFCLR MIDI_EVNT_VALUE, 6, ACCESS, MIDI_EVNT_Send_ExLow
	rlf	MIDI_EVNT_VALUE, W
	andlw	0x7e
MIDI_EVNT_Send_ExLow
	call	MIOS_MIDI_TxBufferPut
	movf	MIDI_EVNT_VALUE, W
	andlw	0x7f
	call	MIOS_MIDI_TxBufferPut
	rgoto	MIDI_EVNT_Send_End

	;; MIDIbox SEQ specific AOUT sending routine
MIDI_EVNT_Send_Fx
	;; channel 1 routed to CV Out #1 and #2 and sets the gate #1 (for Notes)
	;; channel 2 routed to CV Out #3 and #4 and sets the gate #2 (for Notes)
	;; channel 3 routed to CV Out #5 and #6 and sets the gate #3 (for Notes)
	;; channel 4 routed to CV Out #7 and #8 and sets the gate #4 (for Notes)
	;; channel 5 routed to CV Out #1 and #2 and sets the gate #1 (for CCs)
	;; channel 6 routed to CV Out #3 and #4 and sets the gate #2 (for CCs)
	;; channel 7 routed to CV Out #5 and #6 and sets the gate #3 (for CCs)
	;; channel 8 routed to CV Out #7 and #8 and sets the gate #4 (for CCs)
	;; channel 9-16 routed to CV Out #1-8 (only the second MIDI byte is sent to the AOUT)

	;; branch depending on channel
	movf	MIDI_EVNT0, W
	andlw	0x08
	bnz	MIDI_EVNT_Send_Fx_Single
MIDI_EVNT_Send_Fx_Pair
	;; channel 1-8
	;; only set first byte (e.g. note number) if second byte (e.g. velocity) is > 0 
	movf	MIDI_EVNT_VALUE, W
	bz	MIDI_EVNT_Send_Fx_Pair_SkipFirst
MIDI_EVNT_Send_Fx_Pair_First
	movff	MIDI_EVNT1, MIOS_PARAMETER1
	rlf	MIDI_EVNT0, W
	andlw	0x06
	call	AOUT_Pin7bitSet
MIDI_EVNT_Send_Fx_Pair_SkipFirst

	;; if note (channel 1-4) don't set velocity if value == 0
	movff	MIDI_EVNT_VALUE, MIOS_PARAMETER1
	movf	MIDI_EVNT0, W
	andlw	0x0c
	bnz	MIDI_EVNT_Send_Fx_Pair_CC
MIDI_EVNT_Send_Fx_Pair_Note
	movf	MIOS_PARAMETER1, W
	bz	MIDI_EVNT_Send_Fx_Pair_NoVel
MIDI_EVNT_Send_Fx_Pair_CC
	rlf	MIDI_EVNT0, W
	andlw	0x06
	addlw	1
	call	AOUT_Pin7bitSet
MIDI_EVNT_Send_Fx_Pair_NoVel

	;; set/clear gate if channel < 4
	movf	MIDI_EVNT0, W
	andlw	0xfc
	bnz	MIDI_EVNT_Send_Fx_Gate_Skip
MIDI_EVNT_Send_Fx_Gate
	movf	MIDI_EVNT_VALUE, W
	skpz
	movlw	0x01
	movwf	MIOS_PARAMETER1
	rrf	MIDI_EVNT0, W
	andlw	0x03
	rcall	MIDI_EVNT_AOUT_GatePinSet

MIDI_EVNT_Send_Fx_Gate_Skip
	rgoto	MIDI_EVNT_Send_End

MIDI_EVNT_Send_Fx_Single
	movff	MIDI_EVNT_VALUE, MIOS_PARAMETER1
	movf	MIDI_EVNT0, W
	andlw	0x07
	call	AOUT_Pin7bitSet

	movf	MIDI_EVNT_VALUE, W
	skpz
	movlw	0x01
	movwf	MIOS_PARAMETER1
	movf	MIDI_EVNT0, W
	andlw	0x07
	rcall	MIDI_EVNT_AOUT_GatePinSet

	;; 	rgoto	MIDI_EVNT_Send_End

MIDI_EVNT_Send_End
	goto	MIOS_MIDI_EndStream


;; --------------------------------------------------------------------------
;;  FUNCTION: MIDI_EVNT_AOUT_GatePinSet
;;  DESCRIPTION: Help Function which sets a Gate Pin independent from the interface
;;  IN: 
;;     o pin number in WREG
;;     o value in MIOS_PARAMETER1
;; --------------------------------------------------------------------------
MIDI_EVNT_AOUT_GatePinSet
	SET_BSR	GATES
	movf	MIOS_PARAMETER1, F
	bz	MIDI_EVNT_AOUT_PinSet0
MIDI_EVNT_AOUT_PinSet1
	call	MIOS_HLP_GetBitORMask
	iorwf	GATES, F, BANKED
	return

MIDI_EVNT_AOUT_PinSet0
	call	MIOS_HLP_GetBitANDMask
	andwf	GATES, F, BANKED
	return

;; ==========================================================================
;;  FUNCTION: MIDI_EVNT_PrintNote
;;  DESCRIPTION: print a note
;;  IN:  note number in WREG
;;  OUT: 3 characters
;; ==========================================================================
MIDI_EVNT_PrintNote
	;; size-optimized note print routine

	;; determine octave (PRODH) and note number within octave (PRODL)
	movwf	PRODL		; note number
	clrf	PRODH		; octave

MIDI_EVNT_PrintNote_Loop
	movlw	12 - 1		; octave found?
	cpfsgt	PRODL, ACCESS
	rgoto MIDI_EVNT_PrintNote_LoopBreak
	movlw	-12		; if not, decrement 12 from note number and increment octave
	addwf	PRODL, F
	incf	PRODH, F
	rgoto	MIDI_EVNT_PrintNote_Loop

MIDI_EVNT_PrintNote_LoopBreak

	;; get note name from table
	rlf	PRODL, W
	andlw	0xfe
	addlw	MIDI_EVNT_PrintNote_Tab & 0xff
	movwf	TBLPTRL
	clrf	TBLPTRH
	movlw	MIDI_EVNT_PrintNote_Tab >> 8
	addwfc	TBLPTRH, F

	;; get first character
	tblrd*+

	;; capital letter if octave > 2
	movlw	2 - 1
	cpfsgt	PRODH, ACCESS
	rgoto MIDI_EVNT_PrintNote_NoCap
MIDI_EVNT_PrintNote_Cap
	movlw	'A'-'a'
	addwf	TABLAT, F
	movlw	-2
	addwf	PRODH, F
	rgoto	MIDI_EVNT_PrintNote_Cont
MIDI_EVNT_PrintNote_NoCap
	movlw	2
	btfsc	PRODH, 0
	movlw 1
	movwf	PRODH
	;; 	rgoto	MIDI_EVNT_PrintNote_Cont

MIDI_EVNT_PrintNote_Cont
	
	;; print first char
	movf	TABLAT, W
	call	MIOS_LCD_PrintChar

	;; print second char
	tblrd*+
	movf	TABLAT, W
	call	MIOS_LCD_PrintChar

	;; print octave
	movf	PRODH, W
	goto	MIOS_LCD_PrintBCD1
	

MIDI_EVNT_PrintNote_Tab
	db	"c-", "c#", "d-", "d#", "e-", "f-", "f#", "g-", "g#", "a-", "a#", "b-"

