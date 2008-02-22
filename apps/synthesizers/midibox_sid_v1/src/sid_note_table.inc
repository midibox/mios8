; $Id$
;
; MIDIbox SID
; Note Table / Print function
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
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
#if 1
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
	addlw	SID_NOTE_Print_Tab & 0xff
	movwf	TBLPTRL
	clrf	TBLPTRH
	movlw	SID_NOTE_Print_Tab >> 8
	addwfc	TBLPTRH, F

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

#else

	;; speed optimized note print routine
	mullw	SID_NOTE_TABLE_ENTRY_LEN

	;; calculate pointer to note entry
	movlw	SID_NOTE_TABLE & 0xff
	addwf	PRODL, W
	movwf	TBLPTRL
	movlw	SID_NOTE_TABLE >> 8
	addwfc	PRODH, W
	movwf	TBLPTRH

	;; print 3 chars
	movlw	3
	goto	MIOS_LCD_PrintPreconfString

	;; note: every entry allocates 4 bytes due to assembler imperfection
SID_NOTE_TABLE
	db	"c-2"
SID_NOTE_TABLE_1
SID_NOTE_TABLE_ENTRY_LEN EQU SID_NOTE_TABLE_1-SID_NOTE_TABLE
	db	"c#2"
	db	"d-2"
	db	"d#2"
	db	"e-2"
	db	"f-2"
	db	"f#2"
	db	"g-2"
	db	"g#2"
	db	"a-2"
	db	"a#2"
	db	"b-2"

	db	"c-1"
	db	"c#1"
	db	"d-1"
	db	"d#1"
	db	"e-1"
	db	"f-1"
	db	"f#1"
	db	"g-1"
	db	"g#1"
	db	"a-1"
	db	"a#1"
	db	"b-1"

	db	"C_0"
	db	"C#0"
	db	"D_0"
	db	"D#0"
	db	"E_0"
	db	"F_0"
	db	"F#0"
	db	"G_0"
	db	"G#0"
	db	"A_0"
	db	"A#0"
	db	"B_0"

	db	"C_1"
	db	"C#1"
	db	"D_1"
	db	"D#1"
	db	"E_1"
	db	"F_1"
	db	"F#1"
	db	"G_1"
	db	"G#1"
	db	"A_1"
	db	"A#1"
	db	"B_1"

	db	"C_2"
	db	"C#2"
	db	"D_2"
	db	"D#2"
	db	"E_2"
	db	"F_2"
	db	"F#2"
	db	"G_2"
	db	"G#2"
	db	"A_2"
	db	"A#2"
	db	"B_2"
	
	db	"C_3"
	db	"C#3"
	db	"D_3"
	db	"D#3"
	db	"E_3"
	db	"F_3"
	db	"F#3"
	db	"G_3"
	db	"G#3"
	db	"A_3"
	db	"A#3"
	db	"B_3"
	
	db	"C_4"
	db	"C#4"
	db	"D_4"
	db	"D#4"
	db	"E_4"
	db	"F_4"
	db	"F#4"
	db	"G_4"
	db	"G#4"
	db	"A_4"
	db	"A#4"
	db	"B_4"
	
	db	"C_5"
	db	"C#5"
	db	"D_5"
	db	"D#5"
	db	"E_5"
	db	"F_5"
	db	"F#5"
	db	"G_5"
	db	"G#5"
	db	"A_5"
	db	"A#5"
	db	"B_5"
	
	db	"C_6"
	db	"C#6"
	db	"D_6"
	db	"D#6"
	db	"E_6"
	db	"F_6"
	db	"F#6"
	db	"G_6"
	db	"G#6"
	db	"A_6"
	db	"A#6"
	db	"B_6"
	
	db	"C_7"
	db	"C#7"
	db	"D_7"
	db	"D#7"
	db	"E_7"
	db	"F_7"
	db	"F#7"
	db	"G_7"
	db	"G#7"
	db	"A_7"
	db	"A#7"
	db	"B_7"
	
	db	"C_8"
	db	"C#8"
	db	"D_8"
	db	"D#8"
	db	"E_8"
	db	"F_8"
	db	"F#8"
	db	"G_8"

#endif
