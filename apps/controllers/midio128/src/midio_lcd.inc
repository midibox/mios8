; $Id$
;
; MIDIO LCD functions, mainly used by midio_display.inc
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
MIDIO_LCD_SetDefaultOffsets
	movlw	DEFAULT_YOFFSET_LINE0
	movwf	MIOS_PARAMETER1
	movlw	DEFAULT_YOFFSET_LINE1
	movwf	MIOS_PARAMETER2
	movlw	DEFAULT_YOFFSET_LINE2
	movwf	MIOS_PARAMETER3
	movlw	DEFAULT_YOFFSET_LINE3
	goto	MIOS_LCD_YAddressSet

;; --------------------------------------------------------------------------
;;  This help function prints the standard string for a DIN value
;;  IN:  DIN number in WREG
;;  OUT: 16 characters
;;  USES: MIDIO_CURRENT_DIN
;; --------------------------------------------------------------------------
MIDIO_LCD_PrintEvntStd_Btn
	;; copy WREG to MIDIO_CURRENT_DIN
	movff	WREG, MIDIO_CURRENT_DIN
	;; prepare MIDI_EVNT[01] and MIDI_EVNT_VALUE
	call	MIDIO_MIDI_PrepareDINEvent
	;; print MIDI event
	call	MIDI_EVNT_Print
	;; print DIN status
	rgoto	MIDIO_LCD_PrnBtnStatus

;; ==========================================================================
;; print the DIN status (* if pressed, o if depressed)
;; IN:   DIN number in MIDIO_CURRENT_DIN
;; OUT:  1 character
;; USES: TMP1
;; ==========================================================================
MIDIO_LCD_PrnBtnStatus
	;; get DIN state
	call	MIDIO_FSR_DINValue
	movff	MIDIO_CURRENT_DIN, WREG
	call	MIOS_HLP_GetBitORMask
	andwf	INDF0, W

	;; print character
	movlw	'*'		; button pressed
	skpz
	movlw	'o'		; button depressed
	goto	MIOS_LCD_PrintChar
