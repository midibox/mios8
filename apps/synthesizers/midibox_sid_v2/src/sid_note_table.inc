; $Id$
;
; MIDIbox SID
; Note Table / Print function
;
; ==========================================================================
;
;  Copyright 1998-2007 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; ==========================================================================
;;  FUNCTION: SID_NOTE_Print
;;  DESCRIPTION: print a note
;;  IN:  note number in WREG
;;  OUT: 3 characters
;; ==========================================================================
SID_NOTE_Print
	;; size-optimized note print routine

	;; determine octave (PRODH) and note number within octave (PRODL)
	movwf	PRODL		; note number
	clrf	PRODH		; octave

SID_NOTE_Print_Loop
	movlw	12 - 1		; octave found?
	cpfsgt	PRODL, ACCESS
	rgoto SID_NOTE_Print_LoopBreak
	movlw	-12		; if not, decrement 12 from note number and increment octave
	addwf	PRODL, F
	incf	PRODH, F
	rgoto	SID_NOTE_Print_Loop

SID_NOTE_Print_LoopBreak

	;; get note name from table
	rlf	PRODL, W
	andlw	0xfe
	addlw	LOW(SID_NOTE_Print_Tab)
	movwf	TBLPTRL
	clrf	TBLPTRH
	movlw	HIGH(SID_NOTE_Print_Tab)
	addwfc	TBLPTRH, F
	movlw	UPPER(SID_NOTE_Print_Tab)
	addwfc	TBLPTRU, F

	;; get first character
	tblrd*+

	;; capital letter if octave > 2
	movlw	2 - 1
	cpfsgt	PRODH, ACCESS
	rgoto SID_NOTE_Print_NoCap
SID_NOTE_Print_Cap
	movlw	'A'-'a'
	addwf	TABLAT, F
	movlw	-2
	addwf	PRODH, F
	rgoto	SID_NOTE_Print_Cont
SID_NOTE_Print_NoCap
	movlw	2
	btfsc	PRODH, 0
	movlw 1
	movwf	PRODH
	;; 	rgoto	SID_NOTE_Print_Cont

SID_NOTE_Print_Cont
	
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
	

SID_NOTE_Print_Tab
	db	"c-", "c#", "d-", "d#", "e-", "f-", "f#", "g-", "g#", "a-", "a#", "b-"
