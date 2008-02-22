; $Id$
;
; MIDImon LCD functions, mainly used by mon_display.inc
;
; ==========================================================================
;
;  Copyright 1998-2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  This functions sets the default Y offsets
;;  IN:  -
;;  OUT: -
;; --------------------------------------------------------------------------
MON_LCD_SetDefaultOffsets
	movlw	DEFAULT_YOFFSET_LINE0
	movwf	MIOS_PARAMETER1
	movlw	DEFAULT_YOFFSET_LINE1
	movwf	MIOS_PARAMETER2
	movlw	DEFAULT_YOFFSET_LINE2
	movwf	MIOS_PARAMETER3
	movlw	DEFAULT_YOFFSET_LINE3
	goto	MIOS_LCD_YAddressSet


;; --------------------------------------------------------------------------
;;  This functions prints an entry stored in trace memory
;;  IN:  entry number in MON_CURRENT_ENTRY_PTR
;;  OUT: 20 characters
;; --------------------------------------------------------------------------
MON_LCD_PrintEntry
	;; get FSR0 to entry
	call	MON_FSR_TraceEntry
	;; copy entry to MIDI_EVNT*
	movff	POSTINC0, MIDI_EVNT0
	movff	POSTINC0, MIDI_EVNT1
	movff	POSTINC0, MIDI_EVNT_VALUE

	;; if event0 is zero: print "<empty>"
	movf	MIDI_EVNT0, W
	bz	MON_LCD_PrintEntryZero
	rgoto	MON_LCD_PrintEntryNotZero

MON_LCD_PrintEntryZeroSTR	;<------------------>
	db			" -| <no event>      "
MON_LCD_PrintEntryZero
	TABLE_ADDR MON_LCD_PrintEntryZeroSTR
	movlw	20
	goto	MIOS_LCD_PrintPreconfString

MON_LCD_PrintEntryNotZero
	;; if event0 is a CC, print CC name if requested
	BRA_IFCLR MB_STAT, MB_STAT_PRINT_CC_NAMES, ACCESS, MON_LCD_PrintEntry_NoCC
	movf	MIDI_EVNT0, W
	andlw	0xf0
	xorlw	0xb0
	bnz	MON_LCD_PrintEntry_NoCC
	;; print channel
	movf	MIDI_EVNT0, W
	andlw	0x0f
	addlw	1
	call	MIOS_LCD_PrintBCD2
	;; print |
	movlw	'|'
	call	MIOS_LCD_PrintChar
	;; print CC name
	movf	MIDI_EVNT1, W
	rcall	MON_PrintCCName
	;; a space
	movlw	' '
	call	MIOS_LCD_PrintChar	
	;; and the value
	movf	MIDI_EVNT_VALUE, W
	call	MIOS_LCD_PrintBCD3
	;; continue with h-meter
	rgoto	MON_LCD_PrintEntry_Cont
	
MON_LCD_PrintEntry_NoCC

	;; if event >= 0xf0, print special text
	movlw	0xf0-1
	cpfsgt	MIDI_EVNT0, ACCESS
	rgoto MON_LCD_PrintEntry_NoFx
MON_LCD_PrintEntry_Fx
	;; branch depending on event
	movf	MIDI_EVNT0, W
	andlw	0x0f
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	MON_LCD_PrintEntry_F0
	rgoto	MON_LCD_PrintEntry_F1
	rgoto	MON_LCD_PrintEntry_F2
	rgoto	MON_LCD_PrintEntry_F3
	rgoto	MON_LCD_PrintEntry_F4
	rgoto	MON_LCD_PrintEntry_F5
	rgoto	MON_LCD_PrintEntry_F6
	rgoto	MON_LCD_PrintEntry_F7
	rgoto	MON_LCD_PrintEntry_F8
	rgoto	MON_LCD_PrintEntry_F9
	rgoto	MON_LCD_PrintEntry_FA
	rgoto	MON_LCD_PrintEntry_FB
	rgoto	MON_LCD_PrintEntry_FC
	rgoto	MON_LCD_PrintEntry_FD
	rgoto	MON_LCD_PrintEntry_FE
	rgoto	MON_LCD_PrintEntry_FF

MON_LCD_PrintEntry_F0STR
			;<------------------>
	db		"F0| internal error  "
MON_LCD_PrintEntry_F0
	TABLE_ADDR MON_LCD_PrintEntry_F0STR
	movlw	20
	goto	MIOS_LCD_PrintPreconfString
	
MON_LCD_PrintEntry_F1STR;<------------------>
	db		"F1| MTC "
MON_LCD_PrintEntry_F1
	TABLE_ADDR MON_LCD_PrintEntry_F1STR
	movlw	8
	call	MIOS_LCD_PrintPreconfString
	movf	MIDI_EVNT1, W
	call	MIOS_LCD_PrintHex2
	movlw	20-8-2
	rgoto	MON_LCD_PrintSpaces

MON_LCD_PrintEntry_F2STR;<------------------>
	db		"F2| Song Pos "
MON_LCD_PrintEntry_F2
	TABLE_ADDR MON_LCD_PrintEntry_F2STR
	movlw	13
	call	MIOS_LCD_PrintPreconfString
	movf	MIDI_EVNT_VALUE, W
	call	MIOS_LCD_PrintHex2
	movlw	' '
	call	MIOS_LCD_PrintChar
	movf	MIDI_EVNT1, W
	call	MIOS_LCD_PrintHex2
	movlw	20-13-2-1-2
	rgoto	MON_LCD_PrintSpaces

MON_LCD_PrintEntry_F3STR;<------------------>
	db		"F3| Song Sel "
MON_LCD_PrintEntry_F3
	TABLE_ADDR MON_LCD_PrintEntry_F3STR
	movlw	13
	call	MIOS_LCD_PrintPreconfString
	movf	MIDI_EVNT1, W
	call	MIOS_LCD_PrintBCD3
	movlw	20-13-3
	rgoto	MON_LCD_PrintSpaces

MON_LCD_PrintEntry_F4STR;<------------------>
	db		"F4| undefined       "
MON_LCD_PrintEntry_F4
	TABLE_ADDR MON_LCD_PrintEntry_F4STR
	movlw	20
	goto	MIOS_LCD_PrintPreconfString

MON_LCD_PrintEntry_F5STR;<------------------>
	db		"F5| undefined       "
MON_LCD_PrintEntry_F5
	TABLE_ADDR MON_LCD_PrintEntry_F5STR
	movlw	20
	goto	MIOS_LCD_PrintPreconfString

MON_LCD_PrintEntry_F6STR;<------------------>
	db		"F6| Tune Request    "
MON_LCD_PrintEntry_F6
	TABLE_ADDR MON_LCD_PrintEntry_F6STR
	movlw	20
	goto	MIOS_LCD_PrintPreconfString

MON_LCD_PrintEntry_F7STR;<------------------>
	db		"Sx|"
	db		   " <SysEx expired> "
MON_LCD_PrintEntry_F7
	TABLE_ADDR MON_LCD_PrintEntry_F7STR
	movlw	3
	call	MIOS_LCD_PrintPreconfString
	;; uncommented voodoo magic
	movf	MON_CURRENT_ENTRY_PTR, W
	xorwf	MON_TRACE_PTR, W
	bz	MON_LCD_PrintEntry_F7Full
MON_LCD_PrintEntry_F7Trc
	movlw	20-3
	goto	MIOS_LCD_PrintPreconfString

MON_LCD_PrintEntry_F7Full
	movlw	0x01
	movwf	TMP1
	lfsr	FSR0, MON_SYSEX_BUFFER_BEGIN
	decf	MON_SYSEX_PTR, W
	andlw	0x0f
	movwf	TMP2
MON_LCD_PrintEntry_F7SLoop
	incf	TMP1, F
	movf	TMP2, W
	movf	PLUSW0, W
	xorlw	0xf0
	bz	MON_LCD_PrintEntry_F7SLoopBrk
	decf	TMP2, W
	andlw	0x0f
	movwf	TMP2
	BRA_IFCLR TMP1, 4, ACCESS, MON_LCD_PrintEntry_F7SLoop
MON_LCD_PrintEntry_F7SLoopBrk
	movlw	0x09
	cpfslt	TMP1, ACCESS
	rgoto MON_LCD_PrintEntry_F7_GT8
	clrf	TMP3
MON_LCD_PrintEntry_F7_LE8
	movf	TMP2, W
	movf	PLUSW0, W
	call	MIOS_LCD_PrintHex2
	incf	TMP2, W
	andlw	0x0f
	movwf	TMP2
	incf	TMP3, F
	incf	TMP3, F
	decfsz	TMP1, F
	rgoto	MON_LCD_PrintEntry_F7_LE8
MON_LCD_PrintEntry_F7_LE8Loop	
	movlw	' '
	call	MIOS_LCD_PrintChar
	incf	TMP3, F
	movlw	20-3-1
	cpfsgt	TMP3, ACCESS
	rgoto MON_LCD_PrintEntry_F7_LE8Loop
	return

MON_LCD_PrintEntry_F7_GT8
	movf	MON_SYSEX_PTR, W
	addlw	-8
	andlw	0x0f
	movwf	TMP2
	movlw	8
	movwf	TMP1
MON_LCD_PrintEntry_F7_GT8Loop
	movf	TMP2, W
	movf	PLUSW0, W
	call	MIOS_LCD_PrintHex2
	incf	TMP2, W
	andlw	0x0f
	movwf	TMP2
	decfsz	TMP1, F
	rgoto	MON_LCD_PrintEntry_F7_GT8Loop
	movlw	'.'
	goto	MIOS_LCD_PrintChar
	

MON_LCD_PrintEntry_F8STR;<------------------>
	db		"F8| MIDI Clock      "
MON_LCD_PrintEntry_F8
	TABLE_ADDR MON_LCD_PrintEntry_F8STR
	movlw	20
	goto	MIOS_LCD_PrintPreconfString

MON_LCD_PrintEntry_F9STR;<------------------>
	db		"F9| MIDI Tick       "
MON_LCD_PrintEntry_F9
	TABLE_ADDR MON_LCD_PrintEntry_F9STR
	movlw	20
	goto	MIOS_LCD_PrintPreconfString

MON_LCD_PrintEntry_FASTR;<------------------>
	db		"FA| MIDI Start      "
MON_LCD_PrintEntry_FA
	TABLE_ADDR MON_LCD_PrintEntry_FASTR
	movlw	20
	goto	MIOS_LCD_PrintPreconfString

MON_LCD_PrintEntry_FBSTR;<------------------>
	db		"FB| MIDI Continue   "
MON_LCD_PrintEntry_FB
	TABLE_ADDR MON_LCD_PrintEntry_FBSTR
	movlw	20
	goto	MIOS_LCD_PrintPreconfString

MON_LCD_PrintEntry_FCSTR;<------------------>
	db		"FC| MIDI Stop       "
MON_LCD_PrintEntry_FC
	TABLE_ADDR MON_LCD_PrintEntry_FCSTR
	movlw	20
	goto	MIOS_LCD_PrintPreconfString

MON_LCD_PrintEntry_FDSTR;<------------------>
	db		"FD| undefined       "
MON_LCD_PrintEntry_FD
	TABLE_ADDR MON_LCD_PrintEntry_FDSTR
	movlw	20
	goto	MIOS_LCD_PrintPreconfString

MON_LCD_PrintEntry_FESTR;<------------------>
	db		"FE| Active Sense    "
MON_LCD_PrintEntry_FE
	TABLE_ADDR MON_LCD_PrintEntry_FESTR
	movlw	20
	goto	MIOS_LCD_PrintPreconfString

MON_LCD_PrintEntry_FFSTR;<------------------>
	db		"FF| MIDI Reset      "
MON_LCD_PrintEntry_FF
	TABLE_ADDR MON_LCD_PrintEntry_FFSTR
	movlw	20
	goto	MIOS_LCD_PrintPreconfString
	

MON_LCD_PrintEntry_NoFx
	;; else print event with standard output function
	call	MIDI_EVNT_Print

MON_LCD_PrintEntry_Cont
	;; print meter and exit
	movf	MIDI_EVNT_VALUE, W
	rgoto	MON_LCD_Print_Meter_H


;; --------------------------------------------------------------------------
;;  This functions prints the CC controller name (table can be found in
;;  mon_cc_names_table.inc) based on the GM standard
;;  IN:  CC number in WREG
;;  OUT: 8 characters
;; --------------------------------------------------------------------------
MON_PrintCCName
	mullw	8
	TABLE_ADDR MON_CC_NAMES_TABLE
	movf	PRODL, W
	addwf	TBLPTRL, F
	movf	PRODH, W
	addwfc	TBLPTRH, F

	;; print preconfigured string
	movlw	8		; (8 chars)
	goto	MIOS_LCD_PrintPreconfString


;; --------------------------------------------------------------------------
;;  This functions prints the MIDI clock which is traced by midiclk.inc
;;  IN:  -
;;  OUT: 20 characters
;;  USES: -
;; --------------------------------------------------------------------------
MON_LCD_PrintClk
	movf	LEDDIGITS_7, W
	call	MON_LCD_PrintClk_Hlp
	movf	LEDDIGITS_6, W
	call	MON_LCD_PrintClk_Hlp
	movf	LEDDIGITS_5, W
	call	MON_LCD_PrintClk_Hlp
	movf	LEDDIGITS_4, W
	call	MON_LCD_PrintClk_Hlp_BCD
	movlw	'.'
	call	MON_LCD_PrintClk_Hlp_Chr
	movf	LEDDIGITS_3, W
	call	MON_LCD_PrintClk_Hlp
	movf	LEDDIGITS_2, W
	call	MON_LCD_PrintClk_Hlp_BCD
	movlw	'.'
	call	MON_LCD_PrintClk_Hlp_Chr
	movf	LEDDIGITS_1, W
	call	MON_LCD_PrintClk_Hlp
	movf	LEDDIGITS_0, W
	call	MON_LCD_PrintClk_Hlp_BCD

	movlw	3
	rcall	MON_LCD_PrintSpaces

	call	BPM_Get
	call	MIOS_LCD_PrintBCD3

	rcall	MON_LCD_PrintClk_Hlp_Spc

	movlw	'B'
	rcall	MON_LCD_PrintClk_Hlp_Chr
	movlw	'P'
	rcall	MON_LCD_PrintClk_Hlp_Chr
	movlw	'M'
	rgoto	MON_LCD_PrintClk_Hlp_Chr

;; this help function prints a space if bit 4 set, else the BCD
MON_LCD_PrintClk_Hlp
	BRA_IFSET WREG, 4, ACCESS, MON_LCD_PrintClk_Hlp_Spc
MON_LCD_PrintClk_Hlp_BCD
	goto	MIOS_LCD_PrintBCD1
MON_LCD_PrintClk_Hlp_Spc
	movlw	' '
MON_LCD_PrintClk_Hlp_Chr
	goto	MIOS_LCD_PrintChar

;; --------------------------------------------------------------------------
;;  This functions prints the MTC which is traced by mtc.inc
;;  IN:  -
;;  OUT: 20 characters
;;  USES: -
;; --------------------------------------------------------------------------
MON_LCD_PrintMTC_STR
	db	"MTC: "
MON_LCD_PrintMTC
	TABLE_ADDR MON_LCD_PrintMTC_STR
	movlw	5
	call	MIOS_LCD_PrintPreconfString

	movf	LEDDIGITS_7, W
	rcall	MON_LCD_PrintMTC_Hlp_BCD
	movf	LEDDIGITS_6, W
	rcall	MON_LCD_PrintMTC_Hlp_BCD
	movlw	':'
	rcall	MON_LCD_PrintMTC_Hlp_Chr
	movf	LEDDIGITS_5, W
	rcall	MON_LCD_PrintMTC_Hlp_BCD
	movf	LEDDIGITS_4, W
	rcall	MON_LCD_PrintMTC_Hlp_BCD
	movlw	':'
	rcall	MON_LCD_PrintMTC_Hlp_Chr
	movf	LEDDIGITS_3, W
	rcall	MON_LCD_PrintMTC_Hlp_BCD
	movf	LEDDIGITS_2, W
	rcall	MON_LCD_PrintMTC_Hlp_BCD
	movlw	':'
	rcall	MON_LCD_PrintMTC_Hlp_Chr
	movf	LEDDIGITS_1, W
	rcall	MON_LCD_PrintMTC_Hlp_BCD
	movf	LEDDIGITS_0, W
	rcall	MON_LCD_PrintMTC_Hlp_BCD

	movlw	4
	rgoto	MON_LCD_PrintSpaces

;; this help function prints a space if bit 4 set, else the BCD
MON_LCD_PrintMTC_Hlp
	BRA_IFSET WREG, 4, ACCESS, MON_LCD_PrintClk_Hlp_Spc
MON_LCD_PrintMTC_Hlp_BCD
	goto	MIOS_LCD_PrintBCD1
MON_LCD_PrintMTC_Hlp_Spc
	movlw	' '
MON_LCD_PrintMTC_Hlp_Chr
	goto	MIOS_LCD_PrintChar

;; --------------------------------------------------------------------------
;;  This functions prints x spaces
;;  IN:  number of spaces in WREG
;;  OUT: x spaces
;;  USES: TMP5
;; --------------------------------------------------------------------------
MON_LCD_PrintSpaces
	movwf	TMP5
MON_LCD_PrintSpacesLoop
	movlw	' '
	call	MIOS_LCD_PrintChar
	decfsz	TMP5, F
	rgoto	MON_LCD_PrintSpacesLoop
	return


;; --------------------------------------------------------------------------
;;  This functions prints a horizontal meter
;;  IN:  7-bit value in WREG
;;  OUT: 5 characters
;;  USES: TMP1 (when graphical LCD connected)
;; --------------------------------------------------------------------------
MON_LCD_Print_Meter_H
	;; branch if GLCD connected
	BRA_IFSET MB_STAT, MB_STAT_DISPLAY_GLCD, ACCESS, MON_LCD_Print_Meter_H_GLCD

	;; calc pointer to table entry: offset = (value/8) * 6 = (value*32/256) * 6
	mullw	32		; *32
	movf	PRODH, W	; (/256)
	mullw	6		; *6
	;; add to offset
	TABLE_ADDR MON_LCD_Print_Meter_H_Table
	movf	PRODL, W
	addwf	TBLPTRL, F
	movf	PRODH, W
	addwfc	TBLPTRH, F

	;; print preconfigured string
	movlw	5		; (5 chars)
	goto	MIOS_LCD_PrintPreconfString

MON_LCD_Print_Meter_H_Table
	;; special chars which should be print depending on meter value (16 entries, only 14 used)
	;; only 5 are used, the 6th has to be inserted due to a MPASM imperfection
	db	1, 0, 0, 0, 0, 0
	db	2, 0, 0, 0, 0, 0
	db	3, 0, 0, 0, 0, 0
	db	3, 1, 0, 0, 0, 0
	db	3, 2, 0, 0, 0, 0
	db	3, 3, 0, 0, 0, 0
	db	3, 3, 1, 0, 0, 0
	db	3, 3, 2, 0, 0, 0
	db	3, 3, 3, 0, 0, 0
	db	3, 3, 3, 1, 0, 0
	db	3, 3, 3, 2, 0, 0
	db	3, 3, 3, 3, 1, 0
	db	3, 3, 3, 3, 2, 0
	db	3, 3, 3, 3, 3, 0
	db	3, 3, 3, 3, 3, 1
	db	3, 3, 3, 3, 3, 2


;; --------------------------------------------------------------------------
;;  The same for graphical LCDs
;;  IN:  7-bit value in WREG
;;  OUT: 28 pixels
;;  USES: TMP1
;; --------------------------------------------------------------------------
MON_LCD_Print_Meter_H_GLCD
	movwf	TMP1

	;; switch to meter icons
	TABLE_ADDR GLCD_FONT_METER_ICONS_H
	call	MIOS_GLCD_FontInit

	;; 16 icons are available
	;; icon = value * 8 = (value * 32) / 256
	movf	TMP1, W
	mullw	32
	movf	PRODH, W
	call	MIOS_LCD_PrintChar

	;; switch back to default font
	TABLE_ADDR MIOS_GLCD_FONT
	goto	MIOS_GLCD_FontInit
		

;; --------------------------------------------------------------------------
;;  Table of special characters
;; --------------------------------------------------------------------------
MON_LCD_InitHBars
	;; exit if GLCD connected
	btfsc	MB_STAT, MB_STAT_DISPLAY_GLCD
	return

	;; init special chars and exit
	TABLE_ADDR MON_LCD_H_BARS
	goto	MIOS_CLCD_SpecialCharsInit

	;; for horizontal meters
MON_LCD_H_BARS
	;; due to an imperfection in the MPASM we have
	;; to write two bytes in every line :-(
	;; char #1: bar1
	db b'00000000', b'00000000'; 1st and 2nd line of special char
	db b'00000000', b'00000000'; 3rd and 4th line of special char
	db b'00000000', b'00000000'; 5th and 6th line of special char
	db b'00000000', b'00000000'; 7th and 8th line of special char
	;; char #2: bar2
	db b'00000000', b'00010000'; 1st and 2nd line of special char
	db b'00010000', b'00010000'; 3rd and 4th line of special char
	db b'00010000', b'00010000'; 5th and 6th line of special char
	db b'00010000', b'00000000'; 7th and 8th line of special char
	;; char #3: bar3
	db b'00000000', b'00010100'; 1st and 2nd line of special char
	db b'00010100', b'00010100'; 3rd and 4th line of special char
	db b'00010100', b'00010100'; 5th and 6th line of special char
	db b'00010100', b'00000000'; 7th and 8th line of special char
	;; char #4: bar4
	db b'00000000', b'00010101'; 1st and 2nd line of special char
	db b'00010101', b'00010101'; 3rd and 4th line of special char
	db b'00010101', b'00010101'; 5th and 6th line of special char
	db b'00010101', b'00000000'; 7th and 8th line of special char
	;; char #5: bar5
	db b'00000000', b'00000000'; 1st and 2nd line of special char
	db b'00000100', b'00000110'; 3rd and 4th line of special char
	db b'00000110', b'00000100'; 5th and 6th line of special char
	db b'00000000', b'00000000'; 7th and 8th line of special char
	;; char #6: bar6
	db b'00000000', b'00000000'; 1st and 2nd line of special char
	db b'00000000', b'00000000'; 3rd and 4th line of special char
	db b'00000000', b'00000000'; 5th and 6th line of special char
	db b'00000000', b'00000000'; 7th and 8th line of special char
	;; char #7: bar7
	db b'00000000', b'00000000'; 1st and 2nd line of special char
	db b'00000000', b'00000000'; 3rd and 4th line of special char
	db b'00000000', b'00000000'; 5th and 6th line of special char
	db b'00000000', b'00000000'; 7th and 8th line of special char
	;; char #8: bar8
	db b'00000000', b'00000000'; 1st and 2nd line of special char
	db b'00000000', b'00000000'; 3rd and 4th line of special char
	db b'00000000', b'00000000'; 5th and 6th line of special char
	db b'00000000', b'00000000'; 7th and 8th line of special char

