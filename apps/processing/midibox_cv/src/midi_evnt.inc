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
;
;  Copyright 1998-2007 Thorsten Klose (tk@midibox.org)
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
	SET_BSR	MIDI_EVNT0
	swapf	MIDI_EVNT0, W, BANKED
	andlw	0x07
	JUMPTABLE_2BYTES_UNSECURE	; 8 entries
	rgoto	MIDI_EVNT_Send_8x	; Note Off
	rgoto	MIDI_EVNT_Send_9x	; Note On
	rgoto	MIDI_EVNT_Send_Ax	; Aftertouch
	rgoto	MIDI_EVNT_Send_Bx	; Controller
	rgoto	MIDI_EVNT_Send_Cx	; Program Change
	rgoto	MIDI_EVNT_Send_Dx	; Channel Pressure
	rgoto	MIDI_EVNT_Send_Ex	; Pitch Bend
	rgoto	MIDI_EVNT_Send_Fx	; not really supported, sends only the first byte for realtime events


	;; sending three bytes:	
MIDI_EVNT_Send_8x	; Note Off
MIDI_EVNT_Send_9x	; Note On
MIDI_EVNT_Send_Ax	; Aftertouch
MIDI_EVNT_Send_Bx	; Controller
	movff	MIDI_EVNT0, WREG
	call	MIOS_MIDI_TxBufferPut
	movff	MIDI_EVNT1, WREG
	andlw	0x7f
	call	MIOS_MIDI_TxBufferPut
	movff	MIDI_EVNT_VALUE, WREG
	andlw	0x7f
	call	MIOS_MIDI_TxBufferPut
	rgoto	MIDI_EVNT_Send_End

	;; sending two bytes
MIDI_EVNT_Send_Cx	; Program Change
MIDI_EVNT_Send_Dx	; Channel Pressure
	movff	MIDI_EVNT0, WREG
	call	MIOS_MIDI_TxBufferPut
	movff	MIDI_EVNT_VALUE, WREG
	andlw	0x7f
	call	MIOS_MIDI_TxBufferPut
	rgoto	MIDI_EVNT_Send_End

	;; sending three bytes (7bit converted to 14bit)
MIDI_EVNT_Send_Ex	; Pitch Bend
	movff	MIDI_EVNT0, WREG
	call	MIOS_MIDI_TxBufferPut

	SET_BSR	MIDI_EVNT0
	movlw	0x00
	BRA_IFCLR MIDI_EVNT_VALUE, 6, BANKED, MIDI_EVNT_Send_ExLow
	rlf	MIDI_EVNT_VALUE, W, BANKED
	andlw	0x7e
MIDI_EVNT_Send_ExLow
	call	MIOS_MIDI_TxBufferPut
	movff	MIDI_EVNT_VALUE, WREG
	andlw	0x7f
	call	MIOS_MIDI_TxBufferPut
	rgoto	MIDI_EVNT_Send_End

	;; System Exclusive messages not really supported, sends only the first byte for realtime events
	;; SysEx streams etc. have to be send by the user routines
MIDI_EVNT_Send_Fx
	movff	MIDI_EVNT0, WREG
	call	MIOS_MIDI_TxBufferPut
	;; 	rgoto	MIDI_EVNT_Send_End

MIDI_EVNT_Send_End
	goto	MIOS_MIDI_EndStream


;; --------------------------------------------------------------------------
;;  FUNCTION: MIDI_EVNT_Print
;;  DESCRIPTION: prints the standard string of an event
;;  IN:  event in MIDI_EVNT0, MIDI_EVNT1 and MIDI_EVNT_VALUE
;;  OUT: 15 characters
;; --------------------------------------------------------------------------
MIDI_EVNT_Print_Tbl
	;; (number of characters + 9-byte string)
	;;          <------------>
	;;          012345678
	db	5, "NOff     "
	db	5, "Note     "
	db	5, "ATch     "
	db	3, "CC#      "
	db	9, "Prg.Chng "
	db	9, "ChnPrsr. "
	db	9, "PitchBnd "
	db	8, "MetaEvt  "

MIDI_EVNT_Print
	;; print channel
	SET_BSR	MIDI_EVNT0
	movf	MIDI_EVNT0, W, BANKED
	andlw	0x0f
	addlw	1
	call	MIOS_LCD_PrintBCD2

	movlw	'|'
	call	MIOS_LCD_PrintChar

	;; print string from table depending on event
	TABLE_ADDR MIDI_EVNT_Print_Tbl
	SET_BSR	MIDI_EVNT0
	swapf	MIDI_EVNT0, W, BANKED
	andlw	0x07
	mullw	10
	movf	PRODL, W
	TABLE_ADD_W
	tblrd*+			; get number of characters
	movf	TABLAT, W	; and call the print string function of MIOS
	call	MIOS_LCD_PrintPreconfString
	
	;; branch depending on MIDI status
	SET_BSR	MIDI_EVNT0
	swapf	MIDI_EVNT0, W, BANKED
	andlw	0x07
	JUMPTABLE_2BYTES_UNSECURE	; 8 entries
	rgoto	MIDI_EVNT_Print_8x	; Note Off
	rgoto	MIDI_EVNT_Print_9x	; Note On
	rgoto	MIDI_EVNT_Print_Ax	; Aftertouch
	rgoto	MIDI_EVNT_Print_Bx	; Controller
	rgoto	MIDI_EVNT_Print_Cx	; Program Change
	rgoto	MIDI_EVNT_Print_Dx	; Channel Pressure
	rgoto	MIDI_EVNT_Print_Ex	; Pitch Bend
	rgoto	MIDI_EVNT_Print_Fx	; prints META EVENT


	;;        0123456789ABCDE
	;; print "xx|NOff X-Y 000"
MIDI_EVNT_Print_8x	; Note Off
	;; print "xx|Note X-Y 000"
MIDI_EVNT_Print_9x	; Note On
	;; print "xx|ATch X-Y 000"
MIDI_EVNT_Print_Ax	; Aftertouch
	movff	MIDI_EVNT1, WREG
	rcall	MIDI_EVNT_PrintNote
	movlw	' '
	call	MIOS_LCD_PrintChar
	movff	MIDI_EVNT_VALUE, WREG
	call	MIOS_LCD_PrintBCD3
	rgoto	MIDI_EVNT_Print_End
	
	;;        0123456789ABCDE
	;; print "xx|CC#xxx [xxx]"
MIDI_EVNT_Print_Bx	; Controller
	movff	MIDI_EVNT1, WREG
	call	MIOS_LCD_PrintBCD3
	movlw	' '
	call	MIOS_LCD_PrintChar
	movlw	'['
	call	MIOS_LCD_PrintChar
	movff	MIDI_EVNT_VALUE, WREG
	call	MIOS_LCD_PrintBCD3
	movlw	']'
	call	MIOS_LCD_PrintChar
	rgoto	MIDI_EVNT_Print_End

	;;        0123456789ABCDE
	;; print "xx|Prg.Chng xxx"
MIDI_EVNT_Print_Cx	; Program Change
	movff	MIDI_EVNT_VALUE, WREG
	addlw	1
	call	MIOS_LCD_PrintBCD3
	rgoto	MIDI_EVNT_Print_End

	;;        0123456789ABCDE
	;; print "xx|ChnPrsr. xxx"
MIDI_EVNT_Print_Dx	; Channel Pressure
	movff	MIDI_EVNT_VALUE, WREG
	call	MIOS_LCD_PrintBCD3
	rgoto	MIDI_EVNT_Print_End

	;;        0123456789ABCDE
	;; print "xx|PitchBnd xxx"
MIDI_EVNT_Print_Ex	; Pitch Bend
	movff	MIDI_EVNT_VALUE, WREG
	call	MIOS_LCD_PrintBCD3
	rgoto	MIDI_EVNT_Print_End

	;;        0123456789ABCDE
	;; print "xx|MetaEvt YYZZ"
MIDI_EVNT_Print_Fx	; prints META EVENT
	movff	MIDI_EVNT1, WREG
	call	MIOS_LCD_PrintHex2
	movff	MIDI_EVNT_VALUE, WREG
	call	MIOS_LCD_PrintHex2
	;; 	rgoto	MIDI_EVNT_Print_End

MIDI_EVNT_Print_End	
	return



;; --------------------------------------------------------------------------
;;  FUNCTION: MIDI_EVNT_PrintShort
;;  DESCRIPTION: prints the standard string of an event in short form
;;  IN:  event in MIDI_EVNT0, MIDI_EVNT1 and MIDI_EVNT_VALUE
;;  OUT: 6 characters
;; --------------------------------------------------------------------------
MIDI_EVNT_PrintShort
	;; just print the hex numbers
	movff	MIDI_EVNT0, WREG
	call	MIOS_LCD_PrintHex2
	movff	MIDI_EVNT1, WREG
	call	MIOS_LCD_PrintHex2
	movff	MIDI_EVNT_VALUE, WREG
	goto	MIOS_LCD_PrintHex2


;; ==========================================================================
;;  FUNCTION: MIDI_EVNT_PrintNote
;;  DESCRIPTION: print a note
;;  IN:  note number in WREG
;;  OUT: 3 characters
;; ==========================================================================
MIDI_EVNT_PrintNote
	mullw	MIDI_EVNT_NOTE_TABLE_ENTRY_LEN

	;; calculate pointer to note entry
	movlw	MIDI_EVNT_NOTE_TABLE & 0xff
	addwf	PRODL, W
	movwf	TBLPTRL
	movlw	MIDI_EVNT_NOTE_TABLE >> 8
	addwfc	PRODH, W
	movwf	TBLPTRH

	;; print 3 chars
	movlw	3
	goto	MIOS_LCD_PrintPreconfString

	;; note: every entry allocates 4 bytes due to assembler imperfection
MIDI_EVNT_NOTE_TABLE
	db	"c-2"
MIDI_EVNT_NOTE_TABLE_1
MIDI_EVNT_NOTE_TABLE_ENTRY_LEN EQU MIDI_EVNT_NOTE_TABLE_1-MIDI_EVNT_NOTE_TABLE
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

