; $Id$
;
; This file contains all special characters used by the application
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  Table of special characters
;; --------------------------------------------------------------------------
#ifndef USE_CUSTOM_CHAR_HANDLER

CS_MENU_SPECIAL_CHARS
	;; due to an imperfection in the MPASM we have
	;; to write two bytes in every line :-(
	;; char #1: left-arrow
	db b'00000001', b'00000011'; 1st and 2nd line of special char
	db b'00000111', b'00000011'; 3rd and 4th line of special char
	db b'00000001', b'00000000'; 5th and 6th line of special char
	db b'00000000', b'00000000'; 7th and 8th line of special char
	;; char #2: right-arrow
	db b'00000000', b'00000000'; 1st and 2nd line of special char
	db b'00000000', b'00010000'; 3rd and 4th line of special char
	db b'00011000', b'00011100'; 5th and 6th line of special char
	db b'00011000', b'01010000'; 7th and 8th line of special char
	;; char #3: both arrows
	db b'00000001', b'00000011'; 1st and 2nd line of special char
	db b'00000111', b'00010011'; 3rd and 4th line of special char
	db b'00011001', b'00011100'; 5th and 6th line of special char
	db b'00011000', b'00010000'; 7th and 8th line of special char
	;; char #4: Inverted A
	db b'00011111', b'00010001'; 1st and 2nd line of special char
	db b'00010101', b'00010001'; 3rd and 4th line of special char
	db b'00010101', b'00010101'; 5th and 6th line of special char
	db b'00011111', b'00000000'; 7th and 8th line of special char
	;; char #5: inverted C
	db b'00011111', b'00010001'; 1st and 2nd line of special char
	db b'00010111', b'00010111'; 3rd and 4th line of special char
	db b'00010111', b'00010001'; 5th and 6th line of special char
	db b'00011111', b'00000000'; 7th and 8th line of special char
	;; char #6: Inverted E
	db b'00011111', b'00010001'; 1st and 2nd line of special char
	db b'00010111', b'00010001'; 3rd and 4th line of special char
	db b'00010111', b'00010001'; 5th and 6th line of special char
	db b'00011111', b'00000000'; 7th and 8th line of special char
	;; char #7: Inverted I
	db b'00011111', b'00010001'; 1st and 2nd line of special char
	db b'00011011', b'00011011'; 3rd and 4th line of special char
	db b'00011011', b'00010001'; 5th and 6th line of special char
	db b'00011111', b'00000000'; 7th and 8th line of special char
	;; char #8: Inverted P
	db b'00011111', b'00010001'; 1st and 2nd line of special char
	db b'00010101', b'00010001'; 3rd and 4th line of special char
	db b'00010111', b'00010111'; 5th and 6th line of special char
	db b'00011111', b'00000000'; 7th and 8th line of special char

#endif