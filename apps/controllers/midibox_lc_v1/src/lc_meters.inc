; $Id$
;
; MIDIbox LC Meters Driver
;
; ==========================================================================
;
;  Copyright 1998-2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function is used to set a meter pattern
;;  expecting meter number in WREG and pattern index in MIOS_PARAMETER1
;; --------------------------------------------------------------------------
LC_METERS_PatternSet
	lfsr	FSR1, METER_PATTERN_VPOT0_L		; calc pointer to pattern
	clrc
	rlf	WREG, W					; add ringnumber * 2 (== ringnumber << 1) to pointer
	addwf	FSR1L, F				; (note: this function is not called by an interrupt routine,
							; therefore we have to use FSR1)

	;; calc pointer to table entry: LC_METER_PATTERNS + ((pattern index << 1)&0x1e)
	TABLE_ADDR LC_METER_PATTERNS
	rlf	MIOS_PARAMETER1, W
	andlw	0x1e
	addwf	TBLPTRL, F
	movlw	0x00
	addwfc	TBLPTRH, F

	;; pointer now in TBLPTR[LH], read first entry and increment pointer
	tblrd*+

	;; move table entry (TABLAT) to [INDF0] and increment pointer by using POSTINC0
	movff	TABLAT, POSTINC1

	;; read next table entry
	tblrd*+

	;; move table entry (TABLAT) to [INDF0], don't increment here
	movf	TABLAT, W
	btfsc	MIOS_PARAMETER1, 7; set 12th (overdrive) LED if bit 7 of pattern index is set
	iorlw 0x08	
	movwf	INDF1

	;; thats all
	return


;; --------------------------------------------------------------------------
;;  This function is called by USER_Tick (lc_tick.inc) to update the meters
;;  if requested.
;; --------------------------------------------------------------------------
LC_METERS_Handler
	clrf	TMP1		; loop counter
	movlw	0x01		; a walking 1 used as AND mask for LC_METER_UPDATE_REQ
	movwf	TMP2
LC_METERS_Handler_Loop
	movf	TMP2, W
	IRQ_DISABLE
	andwf	LC_METER_UPDATE_REQ, W
	bz	LC_METERS_Handler_Loop_Next
	;; clear request
	comf	TMP2, W
	andwf	LC_METER_UPDATE_REQ, F
	IRQ_ENABLE
	;; calc   pointer to METER_LEVEL
	lfsr	FSR0, METER_LEVEL_BEGIN
	movf	TMP1, W
	addwf	FSR0L, F

	;; print meter (expecting meter number in FSR0L&7 and meter index in INDF0)
	call	LC_DISPLAY_Print_Meter

	;; convert meter value to pattern
	lfsr	FSR1, METER_LEVEL_BEGIN
	movf	TMP1, W
	addwf	FSR1L, F
	;; get meter level
	movf	INDF1, W
	andlw	0x0f
	;; set center LED if overload flag set
	btfsc	INDF1, 7
	iorlw 0x80
	;; save pattern index in MIOS_PARAMETER1
	movwf	MIOS_PARAMETER1
	movf	TMP1, W
	rcall	LC_METERS_PatternSet

LC_METERS_Handler_Loop_Next
	clrc
	rlf	TMP2, F		; shift walking one
	incf	TMP1, F		; increment loop counter
	BRA_IFCLR TMP1, 3, ACCESS, LC_METERS_Handler_Loop
	IRQ_ENABLE

	return

;; --------------------------------------------------------------------------
;;  In this table the LED ring patterns are defined
;;  the patterns are defined in the Logic Control MIDI implementation, chapter 13
;;  note that the 12th LED (the center LED below the encoder) is set by
;;  LC_LEDRING_PatternSet separately
;; --------------------------------------------------------------------------
LC_METER_PATTERNS
	;; 16 entries for meter pattern (can be changed if more LEDs connected)
	;; note: the 12th LED (overdrive) will be inserted automatically in LC_METERS_PatternSet
	dw	b'0000000000000000'
	dw	b'0000000000000001'
	dw	b'0000000000000011'
	dw	b'0000000000000111'
	dw	b'0000000000001111'
	dw	b'0000000000011111'
	dw	b'0000000000111111'
	dw	b'0000000001111111'
	dw	b'0000000011111111'
	dw	b'0000000111111111'
	dw	b'0000001111111111'
	dw	b'0000011111111111'
	dw	b'0000011111111111'
	dw	b'0000011111111111'
	dw	b'0000011111111111'
	dw	b'0000011111111111'
