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
;;  NOTE: only two characters are really used by the CS, the others can contain
;;  random bitpatterns, therefore this table only contains 16 bytes
;; --------------------------------------------------------------------------
CS_MENU_SPECIAL_CHARS
	;; due to an imperfection in the MPASM we have
	;; to write two bytes in every line :-(
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
