; $Id$
;
; MIDIbox FM Control Surface LED Handler
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function is called by the menu handler in cs_menu.inc
;;  when all LEDs should be updated
;;  The DOUT pin numbers are defined in cs_menu_io_tables.inc
;; --------------------------------------------------------------------------
CS_MENU_LED_Update
	;; not used
	;; LEDs are directly controlled from cs_menu_matrix.inc
	return
