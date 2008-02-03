; $Id$
;
; MIDIbox LC LED Rings Driver
;
; ==========================================================================
;
;  Copyright 1998-2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================


;; --------------------------------------------------------------------------
;;  This function is used to set a LED pattern
;;  expecting LED ring number in WREG and pattern index in MIOS_PARAMETER1
;; --------------------------------------------------------------------------
LC_LEDRING_PatternSet
	lfsr	FSR1, LEDRING_PATTERN_VPOT0_L		; calc pointer to pattern
	clrc
	rlf	WREG, W					; add ringnumber * 2 (== ringnumber << 1) to pointer
	addwf	FSR1L, F				; (note: this function is not called by an interrupt routine,
							; therefore we have to use FSR1)

	;; branch depending on controller mode
	BRA_IFSET LC_STAT, LC_STAT_LEDMETERS, ACCESS, LC_LEDRING_PatternSet_LC
	BRA_IFSET LC_STAT, LC_STAT_GPC_SEL, ACCESS, LC_LEDRING_PatternSet_GPC

	;; Logic Control Mode
LC_LEDRING_PatternSet_LC
	;; calc pointer to table entry: LEDRING_PATTERNS_LC + ((pattern index << 1) & 0x7e)
	TABLE_ADDR LEDRING_PATTERNS_LC
	rlf	MIOS_PARAMETER1, W
	andlw	0x7e
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
	btfsc	MIOS_PARAMETER1, 6; set 12th (center) LED if bit 6 of received CC value is set
	iorlw 0x08	
	movwf	INDF1

	;; thats all
	return


	;; GPC Mode
LC_LEDRING_PatternSet_GPC
	;; calc pointer to table entry: LEDRING_PATTERNS_GPC + ((pattern index >> 1) & 0x7e)
	TABLE_ADDR LEDRING_PATTERNS_GPC
	rrf	MIOS_PARAMETER1, W
	andlw	0x7e
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
	movwf	INDF1

	;; thats all
	return	

;; --------------------------------------------------------------------------
;;  This function is called by USER_Tick (lc_tick.inc) to update the LED rings
;;  if requested.
;; --------------------------------------------------------------------------
LC_LEDRING_Handler
	clrf	TMP1		; loop counter
	movlw	0x01		; a walking 1 used as AND mask for LC_LEDRING_UPDATE_REQ
	movwf	TMP2
LC_LEDRING_Handler_Loop
	movf	TMP2, W
	andwf	LC_LEDRING_UPDATE_REQ, W
	bz	LC_LEDRING_Handler_Loop_Next

	;; clear request
	comf	TMP2, W
	andwf	LC_LEDRING_UPDATE_REQ, F

	;; calc pointer to LEDRING_POS (LC emulation mode) or absolute value (GPC mode)
	lfsr	FSR0, LEDRING_POS_BEGIN
	movlw	0x00
	BRA_IFCLR LC_STAT, LC_STAT_GPC_SEL, ACCESS, LC_LEDRING_Handler_Loop_NoLC
	lfsr	FSR0, GPC_ABS_VALUES_BEGIN
	movf	GPC_OFFSET, W
LC_LEDRING_Handler_Loop_NoLC
	addwf	TMP1, W
	addwf	FSR0L, F

	;; print virtual knob (if enabled)
	call	LC_DISPLAY_Print_Knob

	;; set LEDring pattern by using the LC_LEDRING_PatternSet function
	movff	INDF0, MIOS_PARAMETER1
	BRA_IFCLR LC_STAT, LC_STAT_LEDMETERS, ACCESS, LC_LEDRING_Handler_Loop_NoM
LC_LEDRING_Handler_Loop_M
	;; if ledrings in meter mode: copy meter pattern to ledring pattern
	lfsr	FSR1, METER_PATTERN_VPOT0_L
	rlf	TMP1, W
	andlw	0x0e
	addwf	FSR1L, F
	movff	POSTINC1, TMP3
	movff	POSTDEC1, TMP4
	movlw	(LEDRING_PATTERN_VPOT0_L-METER_PATTERN_VPOT0_L)
	addwf	FSR1L, F
	movff	TMP3, POSTINC1
	movff	TMP4, POSTDEC1
	rgoto	LC_LEDRING_Handler_Loop_Next

LC_LEDRING_Handler_Loop_NoM
	movf	TMP1, W
	rcall	LC_LEDRING_PatternSet

LC_LEDRING_Handler_Loop_Next
	clrc
	rlf	TMP2, F		; shift walking one
	incf	TMP1, F		; increment loop counter
	BRA_IFCLR TMP1, 3, ACCESS, LC_LEDRING_Handler_Loop

	return

;; --------------------------------------------------------------------------
;;  In this table the LED ring patterns are defined
;;  the patterns are defined in the Logic Control MIDI implementation, chapter 13
;;  note that the 12th LED (the center LED below the encoder) is set by
;;  LC_LEDRING_PatternSet seperately
;; --------------------------------------------------------------------------
LEDRING_PATTERNS_LC
	;; 16 entries for LED pattern #1
	dw	b'0000000000000000'
	dw	b'0000000000000001'
	dw	b'0000000000000010'
	dw	b'0000000000000100'
	dw	b'0000000000001000'
	dw	b'0000000000010000'
	dw	b'0000000000100000'
	dw	b'0000000001000000'
	dw	b'0000000010000000'
	dw	b'0000000100000000'
	dw	b'0000001000000000'
	dw	b'0000010000000000'
	dw	b'0000010000000000'
	dw	b'0000010000000000'
	dw	b'0000010000000000'
	dw	b'0000010000000000'

	;; 16 entries for LED pattern #2	
	dw	b'0000000000000000'
	dw	b'0000000000111111'
	dw	b'0000000000111110'
	dw	b'0000000000111100'
	dw	b'0000000000111000'
	dw	b'0000000000110000'
	dw	b'0000000000100000'
	dw	b'0000000001100000'
	dw	b'0000000011100000'
	dw	b'0000000111100000'
	dw	b'0000001111100000'
	dw	b'0000011111100000'
	dw	b'0000011111100000'
	dw	b'0000011111100000'
	dw	b'0000011111100000'
	dw	b'0000011111100000'

	;; 16 entries for LED pattern #3
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

	;; 16 entries for LED pattern #4
	dw	b'0000000000000000'
	dw	b'0000000000100000'
	dw	b'0000000001110000'
	dw	b'0000000011111000'
	dw	b'0000000111111100'
	dw	b'0000001111111110'
	dw	b'0000011111111111'
	dw	b'0000011111111111'
	dw	b'0000011111111111'
	dw	b'0000011111111111'
	dw	b'0000011111111111'
	dw	b'0000011111111111'
	dw	b'0000011111111111'
	dw	b'0000011111111111'
	dw	b'0000011111111111'
	dw	b'0000011111111111'

LEDRING_PATTERNS_GPC
	;; 32 entries for LED pattern of GPC mode
	dw	b'0000000000000001'
	dw	b'0000000000000001'
	dw	b'0000000000000011'
	dw	b'0000000000000011'
	dw	b'0000000000000011'
	dw	b'0000000000000111'
	dw	b'0000000000000111'
	dw	b'0000000000000111'
	dw	b'0000000000001111'
	dw	b'0000000000001111'
	dw	b'0000000000001111'
	dw	b'0000000000011111'
	dw	b'0000000000011111'
	dw	b'0000000000011111'
	dw	b'0000000000111111'
	dw	b'0000000000111111'
	dw	b'0000000000111111'
	dw	b'0000000000111111'
	dw	b'0000000001111111'
	dw	b'0000000001111111'
	dw	b'0000000001111111'
	dw	b'0000000011111111'
	dw	b'0000000011111111'
	dw	b'0000000011111111'
	dw	b'0000000111111111'
	dw	b'0000000111111111'
	dw	b'0000000111111111'
	dw	b'0000001111111111'
	dw	b'0000001111111111'
	dw	b'0000001111111111'
	dw	b'0000011111111111'
	dw	b'0000011111111111'
