; $Id$
;
; MIDImon MTC handler
;
; ==========================================================================
;
;  Copyright 1998-2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function is called by MB_MPROC when a MIDI byte has been received
;;  Input:
;;     o received MIDI byte in WREG and MIOS_PARAMETER1
;; --------------------------------------------------------------------------
MON_CLK_NotifyReceivedByte
	;; save MIOS_PARAMETER1 in TMP5 to make this module cascadable with other MIDI handlers
	movff	MIOS_PARAMETER1, TMP5

	;; call MIDI clock handler
	movf	MIOS_PARAMETER1, W
	call	MIDICLK_Handler
	;; returns 0x01 when the display should be updated

	;; if return status != 0, convert the MIDI clock to digits
	bz	MON_CLK_NotifyReceivedByte_End

	;; convert beats (16-bit value, so we have to use MIOS_HLP_Dec2BCD)
	SET_BSR	MIDICLK_BEAT_MSB
	movff	MIDICLK_BEAT_MSB, MIOS_PARAMETER1
	incf	MIDICLK_BEAT_LSB, W, BANKED	; (beat + 1)
	skpnz
	incf	MIOS_PARAMETER1, F	
	call	MIOS_HLP_Dec2BCD
	;; copy result to LEDDIGITS_[7654]

	;; 8th digit
	swapf	MIOS_PARAMETER2, W
	andlw	0x0f
	movwf	LEDDIGITS_7

	;; 7th digit
	movf	MIOS_PARAMETER2, W
	andlw	0x0f
	movwf	LEDDIGITS_6

	;; 6th digit
	swapf	MIOS_PARAMETER1, W
	andlw	0x0f
	movwf	LEDDIGITS_5

	;; 5th digit
	movf	MIOS_PARAMETER1, W
	andlw	0x0f
	iorlw	0x80		; set dot
	movwf	LEDDIGITS_4


	;; blank unused beat digits
	movf	LEDDIGITS_7, W
	bnz	MON_CLK_NotifyReceivedByte_BE
	movlw	0x10
	movwf	LEDDIGITS_7
	
	movf	LEDDIGITS_6, W
	bnz	MON_CLK_NotifyReceivedByte_BE
	movlw	0x10
	movwf	LEDDIGITS_6
	
	movf	LEDDIGITS_5, W
	bnz	MON_CLK_NotifyReceivedByte_BE
	movlw	0x10
	movwf	LEDDIGITS_5
MON_CLK_NotifyReceivedByte_BE

	;; blank 4th digit
	movlw	0x10
	movwf	LEDDIGITS_3

	;; copy quarter note counter + 1 to 3rd digit
	SET_BSR	MIDICLK_QN
	incf	MIDICLK_QN, W, BANKED
	andlw	0x0f
	iorlw	0x80		; set dot
	movwf	LEDDIGITS_2

	;; the last digits: use fast BCD routine
	incf	MIDICLK_CTR, W, BANKED
	rcall	CONVERT_Bin2BCD	; result in MSD and LSD
	movf	MSD, W
	;; blank 2nd digit if 0
	skpnz
	movlw	0x10
	movwf	LEDDIGITS_1
	movff	LSD, LEDDIGITS_0

	;; display MIDI clock at first line
	bsf	MB_STAT, MB_STAT_SHOW_MIDICLK
	bcf	MB_STAT, MB_STAT_SHOW_MTC

	;; request 1st line update
	bsf	MB_STAT, MB_STAT_DISPLAY_UPDATE_1L_REQ

MON_CLK_NotifyReceivedByte_End

	;; restore MIOS_PARAMETER1 from TMP5
	movff	TMP5, MIOS_PARAMETER1

	return
