; $Id$
;
; Special Character Handler for Control Surface
; (Optimized routines which ensure that the characters will only be 
;  initialized if really necessary - means: when they haven't been
;  uploaded before)
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  Use this function to initialize the special characters
;;  Expecting the charset number in WREG, supported:
;;     CS_CHARSET_ARROWS: for common menu page
;;     CS_CHARSET_HBARS:  horizontal bars
;;     CS_CHARSET_VBARS:  vertical bars
;; --------------------------------------------------------------------------
CS_SPECIAL_CHARS_Init
	;; save charset number in TABLAT (a temporary register which
	;; will be overwritten anyhow)
	movwf	TABLAT	

	;; exit if GLCD connected
	BRA_IFSET MIOS_BOX_CFG0, MIOS_BOX_CFG0_USE_GLCD, ACCESS, CS_SPECIAL_CHARS_Init_End

	;; exit function if charset already uploaded
	xorwf	CS_MENU_CHARSET, W
	bz	CS_SPECIAL_CHARS_Init_End

	;; else save number and upload new charset
	movf	TABLAT, W
	movwf	CS_MENU_CHARSET
	
	andlw	0x03
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	CS_SPECIAL_CHARS_Init0
	rgoto	CS_SPECIAL_CHARS_Init1
	rgoto	CS_SPECIAL_CHARS_Init2
	rgoto	CS_SPECIAL_CHARS_Init3
	
CS_SPECIAL_CHARS_Init0
	;; charset #0 not used
	rgoto	CS_SPECIAL_CHARS_Init_End

CS_SPECIAL_CHARS_Init1
	;; charset #1: arrows
	movlw	CS_SPECIAL_CHARS1 & 0xff
	movwf	TBLPTRL
	movlw	CS_SPECIAL_CHARS1 >> 8
	rgoto	CS_SPECIAL_CHARS_Init_Cont

CS_SPECIAL_CHARS_Init2
	;; charset #2: vertical bars
	movlw	CS_SPECIAL_CHARS2 & 0xff
	movwf	TBLPTRL
	movlw	CS_SPECIAL_CHARS2 >> 8
	rgoto	CS_SPECIAL_CHARS_Init_Cont

CS_SPECIAL_CHARS_Init3
	;; charset #3: horizontal bars
	movlw	CS_SPECIAL_CHARS3 & 0xff
	movwf	TBLPTRL
	movlw	CS_SPECIAL_CHARS3 >> 8
	;; 	rgoto	CS_SPECIAL_CHARS_Init_Cont

CS_SPECIAL_CHARS_Init_Cont
	movwf	TBLPTRH
	call	MIOS_CLCD_SpecialCharsInit

CS_SPECIAL_CHARS_Init_End
	return


;; --------------------------------------------------------------------------
;;  Table of special characters
;;  due to an imperfection in the MPASM we have
;;  to write two bytes in every line :-(
;; --------------------------------------------------------------------------
CS_SPECIAL_CHARS1
	;; char #1: left-arrow
	db b'00000011', b'00000111'; 1st and 2nd line of special char
	db b'00001111', b'00011111'; 3rd and 4th line of special char
	db b'00011111', b'00001111'; 5th and 6th line of special char
	db b'00000111', b'00000011'; 7th and 8th line of special char
	;; char #2: right-arrow
	db b'00011000', b'00011100'; 1st and 2nd line of special char
	db b'00011110', b'00011111'; 3rd and 4th line of special char
	db b'00011111', b'00011110'; 5th and 6th line of special char
	db b'00011100', b'00011000'; 7th and 8th line of special char
	;; others are not used

CS_SPECIAL_CHARS2
	;; char #1: vbar1
	db b'00000000', b'00000000'; 1st and 2nd line of special char
	db b'00000000', b'00000000'; 3rd and 4th line of special char
	db b'00000000', b'00000000'; 5th and 6th line of special char
	db b'00000000', b'00011111'; 7th and 8th line of special char
	;; char #2: vbar2
	db b'00000000', b'00000000'; 1st and 2nd line of special char
	db b'00000000', b'00000000'; 3rd and 4th line of special char
	db b'00000000', b'00000000'; 5th and 6th line of special char
	db b'00011111', b'00011111'; 7th and 8th line of special char
	;; char #3: vbar3
	db b'00000000', b'00000000'; 1st and 2nd line of special char
	db b'00000000', b'00000000'; 3rd and 4th line of special char
	db b'00000000', b'00011111'; 5th and 6th line of special char
	db b'00011111', b'00011111'; 7th and 8th line of special char
	;; char #4: vbar4
	db b'00000000', b'00000000'; 1st and 2nd line of special char
	db b'00000000', b'00000000'; 3rd and 4th line of special char
	db b'00011111', b'00011111'; 5th and 6th line of special char
	db b'00011111', b'00011111'; 7th and 8th line of special char
	;; char #5: vbar5
	db b'00000000', b'00000000'; 1st and 2nd line of special char
	db b'00000000', b'00011111'; 3rd and 4th line of special char
	db b'00011111', b'00011111'; 5th and 6th line of special char
	db b'00011111', b'00011111'; 7th and 8th line of special char
	;; char #6: vbar6
	db b'00000000', b'00000000'; 1st and 2nd line of special char
	db b'00011111', b'00011111'; 3rd and 4th line of special char
	db b'00011111', b'00011111'; 5th and 6th line of special char
	db b'00011111', b'00011111'; 7th and 8th line of special char
	;; char #7: vbar7
	db b'00000000', b'00011111'; 1st and 2nd line of special char
	db b'00011111', b'00011111'; 3rd and 4th line of special char
	db b'00011111', b'00011111'; 5th and 6th line of special char
	db b'00011111', b'00011111'; 7th and 8th line of special char
	;; char #8: vbar8
	db b'00011111', b'00011111'; 1st and 2nd line of special char
	db b'00011111', b'00011111'; 3rd and 4th line of special char
	db b'00011111', b'00011111'; 5th and 6th line of special char
	db b'00011111', b'00011111'; 7th and 8th line of special char


CS_SPECIAL_CHARS3
	;; char #1: hbar1
	db b'00000000', b'00000000'; 1st and 2nd line of special char
	db b'00000000', b'00000000'; 3rd and 4th line of special char
	db b'00000000', b'00000000'; 5th and 6th line of special char
	db b'00000000', b'00000000'; 7th and 8th line of special char
	;; char #2: hbar2
	db b'00000000', b'00010000'; 1st and 2nd line of special char
	db b'00010000', b'00010000'; 3rd and 4th line of special char
	db b'00010000', b'00010000'; 5th and 6th line of special char
	db b'00010000', b'00000000'; 7th and 8th line of special char
	;; char #3: hbar3
	db b'00000000', b'00010100'; 1st and 2nd line of special char
	db b'00010100', b'00010100'; 3rd and 4th line of special char
	db b'00010100', b'00010100'; 5th and 6th line of special char
	db b'00010100', b'00000000'; 7th and 8th line of special char
	;; char #4: hbar4
	db b'00000000', b'00010101'; 1st and 2nd line of special char
	db b'00010101', b'00010101'; 3rd and 4th line of special char
	db b'00010101', b'00010101'; 5th and 6th line of special char
	db b'00010101', b'00000000'; 7th and 8th line of special char
	;; char #5: hbar5
	db b'00000000', b'00000000'; 1st and 2nd line of special char
	db b'00000000', b'00010000'; 3rd and 4th line of special char
	db b'00010000', b'00000000'; 5th and 6th line of special char
	db b'00000000', b'00000000'; 7th and 8th line of special char
	;; char #6: hbar6
	db b'00000000', b'00010000'; 1st and 2nd line of special char
	db b'00010100', b'00010101'; 3rd and 4th line of special char
	db b'00010101', b'00010100'; 5th and 6th line of special char
	db b'00010000', b'00000000'; 7th and 8th line of special char
	;; others are not used
