; $Id$
;
; MIDIO Display functions
;
; ==========================================================================
;
;  Copyright 1998-2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  This handler is called from DISPLAY_Tick
;;  IN:  -
;;  OUT: -
;; --------------------------------------------------------------------------
MIDIO_DISPLAY_Handler
	;; don't continue of no display update has been requested
	btfss	MB_STAT, MB_STAT_DISPLAY_UPDATE_REQ
	return
	;; clear flag
	bcf	MB_STAT, MB_STAT_DISPLAY_UPDATE_REQ

	;; first line: print last incoming event
	movlw	0x00
	call	MIOS_LCD_CursorSet

	SET_BSR	MIDIO_BASE
	movf	MIDIO_LAST_RECEIVED_EVNT0, W, BANKED
	bz	MIDIO_DISPLAY_Handler_NoInEvnt
MIDIO_DISPLAY_Handler_InEvnt
	movff	MIDIO_LAST_RECEIVED_EVNT0, MIDI_EVNT0
	movff	MIDIO_LAST_RECEIVED_EVNT1, MIDI_EVNT1
	movff	MIDIO_LAST_RECEIVED_EVNT2, MIDI_EVNT_VALUE
	call	MIDI_EVNT_Print
	rgoto	MIDIO_DISPLAY_Handler_InEvntCont
MIDIO_DISPLAY_Handler_NoInEvnt
MIDIO_DISPLAY_Handler_InEvntCont

	;; second line: print last outgoing event
	movlw	0x40
	call	MIOS_LCD_CursorSet
	movff	MIDIO_LAST_DIN0, WREG
	goto	MIDIO_LCD_PrintEvntStd_Btn

