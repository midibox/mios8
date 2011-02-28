; $Id: special_characters.inc 111 2008-02-22 00:41:21Z tk $
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

;; loads a new charset if necessary, charset ID in WREG
MIOS_CUSTOM_CHAR_HANDLER_LoadCharset
	;; check if the charset is already loaded
	cpfseq 	LAST_CHARSET, ACCESS
	rgoto 	MIOS_CUSTOM_CHAR_HANDLER_DoLoad
	;; not loading the same charset again
	return
	
;; loads the charset
MIOS_CUSTOM_CHAR_HANDLER_DoLoad	
	;; save loaded charset ID
	movwf	LAST_CHARSET

	JUMPTABLE_2BYTES 7 ; entries
	rgoto	MIOS_CUSTOM_CHAR_HANDLER_0
	rgoto	MIOS_CUSTOM_CHAR_HANDLER_1
	rgoto	MIOS_CUSTOM_CHAR_HANDLER_2
	rgoto	MIOS_CUSTOM_CHAR_HANDLER_3
	rgoto	MIOS_CUSTOM_CHAR_HANDLER_4
	rgoto	MIOS_CUSTOM_CHAR_HANDLER_5
	rgoto	MIOS_CUSTOM_CHAR_HANDLER_6

	;; pick right table
MIOS_CUSTOM_CHAR_HANDLER_0
	TABLE_ADDR CS_MENU_SPECIAL_CHARS_SET0
	goto	MIOS_CLCD_SpecialCharsInit
MIOS_CUSTOM_CHAR_HANDLER_1	
	TABLE_ADDR CS_MENU_SPECIAL_CHARS_SET1
	goto	MIOS_CLCD_SpecialCharsInit
MIOS_CUSTOM_CHAR_HANDLER_2
	TABLE_ADDR CS_MENU_SPECIAL_CHARS_SET2
	goto	MIOS_CLCD_SpecialCharsInit
MIOS_CUSTOM_CHAR_HANDLER_3
	TABLE_ADDR CS_MENU_SPECIAL_CHARS_SET3
	goto	MIOS_CLCD_SpecialCharsInit
MIOS_CUSTOM_CHAR_HANDLER_4
	TABLE_ADDR CS_MENU_SPECIAL_CHARS_SET4
	goto	MIOS_CLCD_SpecialCharsInit
MIOS_CUSTOM_CHAR_HANDLER_5
	TABLE_ADDR CS_MENU_SPECIAL_CHARS_SET5
	goto	MIOS_CLCD_SpecialCharsInit
MIOS_CUSTOM_CHAR_HANDLER_6
	TABLE_ADDR CS_MENU_SPECIAL_CHARS_SET6
	goto    MIOS_CLCD_SpecialCharsInit
	
;; all charsets have identical chars 0-3 (left, right and both arrows)
;; as they appear on all menu pages	
	
;; Set0	contains the first part of the charset used for the routing
;; display (chars 3-7)
CS_MENU_SPECIAL_CHARS_SET0
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
	;; char #4: CON 1
	db b'00000000', b'00000000'; 1st and 2nd line of special char
	db b'00011100', b'00011111'; 3rd and 4th line of special char
	db b'00011100', b'00000000'; 5th and 6th line of special char
	db b'00000000', b'00000000'; 7th and 8th line of special char	
	;; char #5: CON 2
	db b'00000000', b'00000000'; 1st and 2nd line of special char
	db b'00011100', b'00010111'; 3rd and 4th line of special char
	db b'00011100', b'00000000'; 5th and 6th line of special char
	db b'00000000', b'00000000'; 7th and 8th line of special char	
	;; char #6: CON 3
	db b'00011100', b'00010111'; 1st and 2nd line of special char
	db b'00011100', b'00000000'; 3rd and 4th line of special char
	db b'00011100', b'00011111'; 5th and 6th line of special char
	db b'00011100', b'00000000'; 7th and 8th line of special char
	;; char #7: CON 4
	db b'00000000', b'11111111'; 1st and 2nd line of special char
	db b'00000000', b'00000000'; 3rd and 4th line of special char
	db b'00011100', b'00011111'; 5th and 6th line of special char
	db b'00011100', b'00000000'; 7th and 8th line of special char
	;; char #8: CON 5
	db b'00000000', b'00011000'; 1st and 2nd line of special char
	db b'00000100', b'00000011'; 3rd and 4th line of special char
	db b'00000100', b'00011000'; 5th and 6th line of special char
	db b'00000000', b'00000000'; 7th and 8th line of special char
	
;; Set1	contains the second part of the charset used for the routing
;; display (chars 3-7)
CS_MENU_SPECIAL_CHARS_SET1
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
	;; char #4: CON 6
	db b'00000000', b'00011111'; 1st and 2nd line of special char
	db b'00000000', b'00000000'; 3rd and 4th line of special char
	db b'00011101', b'00011110'; 5th and 6th line of special char
	db b'00011100', b'00000000'; 7th and 8th line of special char	
	;; char #5: CON 7
	db b'00011100', b'00011111'; 1st and 2nd line of special char
	db b'00011100', b'00000000'; 3rd and 4th line of special char
	db b'00011100', b'00011111'; 5th and 6th line of special char
	db b'00011100', b'00000000'; 7th and 8th line of special char	
	;; char #6: CON 8
	db b'00011100', b'00010111'; 1st and 2nd line of special char
	db b'00011100', b'00000000'; 3rd and 4th line of special char
	db b'00011100', b'00011111'; 5th and 6th line of special char
	db b'00011100', b'00000000'; 7th and 8th line of special char
	;; char #7: CON 9
	db b'00000000', b'00011111'; 1st and 2nd line of special char
	db b'00001000', b'00010000'; 3rd and 4th line of special char
	db b'00000111', b'00000111'; 5th and 6th line of special char
	db b'00000111', b'00000000'; 7th and 8th line of special char
	;; char #8: CON 10
	db b'00000000', b'00011000'; 1st and 2nd line of special char
	db b'00000100', b'00000011'; 3rd and 4th line of special char
	db b'00000100', b'00011000'; 5th and 6th line of special char
	db b'00000000', b'00000000'; 7th and 8th line of special char	

;; Set2	is the charset for the main page, containing the arrows
;; (which can be replaced but are kept to have this table align
;; with the standard set) and the inverted characters for the
;; [E]nsemble, [I]nstrument, [P]atch, [A]udio and [C]hannel
CS_MENU_SPECIAL_CHARS_SET2
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
	
;; Set3	contains the 1st set of characters for the waveform display
;; each waveform is 4 characters long, but also uses regular 
;; characters like ^ or -. 
CS_MENU_SPECIAL_CHARS_SET3
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
	;; char #4: WAVE 1
	db b'00000011', b'00000100'; 1st and 2nd line of special char
	db b'00001000', b'00010000'; 3rd and 4th line of special char
	db b'00000000', b'00000000'; 5th and 6th line of special char
	db b'00000000', b'00000000'; 7th and 8th line of special char	
	;; char #5: WAVE 2
	db b'00011000', b'00000100'; 1st and 2nd line of special char
	db b'00000010', b'00000001'; 3rd and 4th line of special char
	db b'00000000', b'00000000'; 5th and 6th line of special char
	db b'00000000', b'00000000'; 7th and 8th line of special char	
	;; char #6: WAVE 3
	db b'00000000', b'00000000'; 1st and 2nd line of special char
	db b'00000000', b'00000000'; 3rd and 4th line of special char
	db b'00010000', b'00001000'; 5th and 6th line of special char
	db b'00000100', b'00000011'; 7th and 8th line of special char
	;; char #7: WAVE 4
	db b'00000000', b'00000000'; 1st and 2nd line of special char
	db b'00000000', b'00000000'; 3rd and 4th line of special char
	db b'00000001', b'00000010'; 5th and 6th line of special char
	db b'00000100', b'00011000'; 7th and 8th line of special char
	;; char #8: WAVE 5
	db b'00000111', b'00001001'; 1st and 2nd line of special char
	db b'00010001', b'00010001'; 3rd and 4th line of special char
	db b'00000000', b'00000000'; 5th and 6th line of special char
	db b'00000000', b'00000000'; 7th and 8th line of special char	
		
;; Set4	contains the 2nd set of characters for the waveform display
CS_MENU_SPECIAL_CHARS_SET4
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
	;; char #4: WAVE 6
	db b'00001110', b'00010001'; 1st and 2nd line of special char
	db b'00010001', b'00010001'; 3rd and 4th line of special char
	db b'00000000', b'00000000'; 5th and 6th line of special char
	db b'00000000', b'00000000'; 7th and 8th line of special char	
	;; char #5: WAVE 7
	db b'00000000', b'00000000'; 1st and 2nd line of special char
	db b'00000000', b'00000000'; 3rd and 4th line of special char
	db b'00010001', b'00010001'; 5th and 6th line of special char
	db b'00010001', b'00001110'; 7th and 8th line of special char	
	;; char #6: WAVE 8
	db b'00011111', b'00010000'; 1st and 2nd line of special char
	db b'00010000', b'00010000'; 3rd and 4th line of special char
	db b'00000000', b'00000000'; 5th and 6th line of special char
	db b'00000000', b'00000000'; 7th and 8th line of special char
	;; char #7: WAVE 9
	db b'00011111', b'00000001'; 1st and 2nd line of special char
	db b'00000001', b'00000001'; 3rd and 4th line of special char
	db b'00000001', b'00000001'; 5th and 6th line of special char
	db b'00000001', b'00000000'; 7th and 8th line of special char
	;; char #8: WAVE 10
	db b'00000000', b'00000000'; 1st and 2nd line of special char
	db b'00000000', b'00000001'; 3rd and 4th line of special char
	db b'00000001', b'00000001'; 5th and 6th line of special char
	db b'00011111', b'00000000'; 7th and 8th line of special char	

;; Set5	contains the 1st set of characters for the LFO waveform 
;; display where each waveform is 3 characters long. Again, they
;; also use regular characters like ^ or -. 
CS_MENU_SPECIAL_CHARS_SET5
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
	;; char #4: LFO 1
	db b'00000111', b'00001000'; 1st and 2nd line of special char
	db b'00010000', b'00000000'; 3rd and 4th line of special char
	db b'00000000', b'00000000'; 5th and 6th line of special char
	db b'00000000', b'00000000'; 7th and 8th line of special char	
	;; char #5: LFO 2
	db b'00000000', b'00010000'; 1st and 2nd line of special char
	db b'00001000', b'00000100'; 3rd and 4th line of special char
	db b'00000010', b'00000001'; 5th and 6th line of special char
	db b'00000000', b'00000000'; 7th and 8th line of special char	
	;; char #6: LFO 3
	db b'00000000', b'00000000'; 1st and 2nd line of special char
	db b'00000000', b'00000000'; 3rd and 4th line of special char
	db b'00000001', b'00000010'; 5th and 6th line of special char
	db b'00011100', b'00000000'; 7th and 8th line of special char
	;; char #7: LFO 4
	db b'00000000', b'00000000'; 1st and 2nd line of special char
	db b'00000000', b'00000000'; 3rd and 4th line of special char
	db b'00000001', b'00000010'; 5th and 6th line of special char
	db b'00000100', b'00001000'; 7th and 8th line of special char
	;; char #8: LFO 5
	db b'00000000', b'00000000'; 1st and 2nd line of special char
	db b'00000000', b'00000000'; 3rd and 4th line of special char
	db b'00010000', b'00001000'; 5th and 6th line of special char
	db b'00000100', b'00000010'; 7th and 8th line of special char	

;; Set6	contains the 2nd set of characters for the LFO waveform 
;; display 
CS_MENU_SPECIAL_CHARS_SET6
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
	;; char #4: LFO 6
	db b'00000000', b'00000000'; 1st and 2nd line of special char
	db b'00000000', b'00000000'; 3rd and 4th line of special char
	db b'00000000', b'00000001'; 5th and 6th line of special char
	db b'00000110', b'00011000'; 7th and 8th line of special char	
	;; char #5: LFO 7
	db b'00000000', b'00000000'; 1st and 2nd line of special char
	db b'00000001', b'00000110'; 3rd and 4th line of special char
	db b'00011000', b'00000000'; 5th and 6th line of special char
	db b'00000000', b'00000000'; 7th and 8th line of special char	
	;; char #6: LFO 8
	db b'00000110', b'00011010'; 1st and 2nd line of special char
	db b'00000010', b'00000010'; 3rd and 4th line of special char
	db b'00000010', b'00000010'; 5th and 6th line of special char
	db b'00000010', b'00000010'; 7th and 8th line of special char
	;; char #7: LFO 9
	db b'00000000', b'00000100'; 1st and 2nd line of special char
	db b'00001010', b'00001001'; 3rd and 4th line of special char
	db b'00001001', b'00010001'; 5th and 6th line of special char
	db b'00010000', b'00000000'; 7th and 8th line of special char
	;; char #8: LFO 10
	db b'00000010', b'00000010'; 1st and 2nd line of special char
	db b'00000010', b'00000101'; 3rd and 4th line of special char
	db b'00001001', b'00001000'; 5th and 6th line of special char
	db b'00011000', b'00001000'; 7th and 8th line of special char	
