; $Id$
;
; This file contains all special characters used by the application
;
; ==========================================================================
;
;  Copyright 1998-2007 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  Table of special characters
;;  NOTE: only seven characters are really used by the CS, the others can contain
;;  random bitpatterns, therefore this table only contains 16 bytes
;; --------------------------------------------------------------------------
CS_MENU_SPECIAL_CHARS
	;; due to an imperfection in the MPASM we have
	;; to write two bytes in every line :-(
	;; char #0: left-arrow
	db b'00000011', b'00000111'; 1st and 2nd line of special char
	db b'00001111', b'00011111'; 3rd and 4th line of special char
	db b'00011111', b'00001111'; 5th and 6th line of special char
	db b'00000111', b'00000011'; 7th and 8th line of special char
	;; char #1: right-arrow
	db b'00011000', b'00011100'; 1st and 2nd line of special char
	db b'00011110', b'00011111'; 3rd and 4th line of special char
	db b'00011111', b'00011110'; 5th and 6th line of special char
	db b'00011100', b'00011000'; 7th and 8th line of special char
	;; char #2: inverted 1
	db b'00011011', b'00010011'; 1st and 2nd line of special char
	db b'00011011', b'00011011'; 3rd and 4th line of special char
	db b'00011011', b'00011011'; 5th and 6th line of special char
	db b'00010001', b'00011111'; 7th and 8th line of special char
	;; char #3: inverted 2
	db b'00010001', b'00001110'; 1st and 2nd line of special char
	db b'00011110', b'00011101'; 3rd and 4th line of special char
	db b'00011011', b'00010111'; 5th and 6th line of special char
	db b'00000000', b'00011111'; 7th and 8th line of special char
	;; char #4: inverted 3
	db b'00000000', b'00011101'; 1st and 2nd line of special char
	db b'00011011', b'00011101'; 3rd and 4th line of special char
	db b'00011110', b'00001110'; 5th and 6th line of special char
	db b'00010001', b'00011111'; 7th and 8th line of special char
	;; char #5: inverted 4
	db b'00011101', b'00011001'; 1st and 2nd line of special char
	db b'00010101', b'00001101'; 3rd and 4th line of special char
	db b'00000000', b'00011101'; 5th and 6th line of special char
	db b'00011101', b'00011111'; 7th and 8th line of special char
#if 0
	;; char #6: inverted '-'
	db b'00011111', b'00011111'; 1st and 2nd line of special char
	db b'00011111', b'00000000'; 3rd and 4th line of special char
	db b'00011111', b'00011111'; 5th and 6th line of special char
	db b'00011111', b'00011111'; 7th and 8th line of special char
#else
	;; char #6: "-1" in one character
	db b'00000010', b'00000110'; line 1 / 2
	db b'00000010', b'00011010'; line 3 / 4
	db b'00000010', b'00000010'; line 5 / 6
	db b'00000111', b'00000000'; line 7 / 8
#endif
	;; char #7: left/right arrow
	db b'00000100', b'00000100'; line 1 / 2
	db b'00001110', b'00011111'; line 3 / 4
	db b'00011111', b'00001110'; line 5 / 6
	db b'00000100', b'00000100'; line 7 / 8
