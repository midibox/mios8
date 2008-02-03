; $Id$
;
; MIDIbox LC LED/LEDring/Meter Driver
;
; ==========================================================================
;
;  Copyright 1998-2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function sets all LEDs which are assigned to a specific 
;;  control function ID (defined in lc_io_table.inc)
;;  Input:
;;     o Function ID in MIOS_PARAMETER2
;;     o LED on/off status in MIOS_PARAMETER3:
;;       - 1 if LED should be turned on
;;       - 0 if LED should be turned off
;; --------------------------------------------------------------------------
LC_LED_Set
	;; search in the LC_IO_TABLE (lc_io_table.inc) if a DOUT pin has been
	;; assigned to the function
	TABLE_ADDR LC_IO_TABLE_LAYER0	; init table pointer
	;; if layer1 selected, increment high byte (-> +256)
	btfsc	LC_STAT, LC_STAT_LAYER_SEL
	incf	TBLPTRH, F

	movf	MIOS_PARAMETER2, W	; load compare value to WREG
	clrf	LC_LED_LOOP_CTR
LC_LED_Set_Loop
	tblrd*+				; read (and ignore) button entry
	tblrd*+				; read LED entry
	movf	TABLAT, W
	cpfseq	MIOS_PARAMETER2, ACCESS
	rgoto LC_LED_Set_Loop_Next
	
	;; found ID_*; set the LED
	movf	MIOS_PARAMETER3, W
	bz	LC_LED_Set_Loop_LEDOff
LC_LED_Set_Loop_LEDOn
	movf	LC_LED_LOOP_CTR, W
	call	MIOS_DOUT_PinSet1
	rgoto	LC_LED_Set_Loop_Next

LC_LED_Set_Loop_LEDOff
	movf	LC_LED_LOOP_CTR, W
	call	MIOS_DOUT_PinSet0
	;; 	rgoto	LC_LED_Set_Loop_Next

LC_LED_Set_Loop_Next
	;; increment loop counter
	incf	LC_LED_LOOP_CTR, F
	;; loop until LC_LED_LOOP_CTR == 0x80 (bit #7 set)
	BRA_IFCLR LC_LED_LOOP_CTR, 7, ACCESS, LC_LED_Set_Loop

	return


;; --------------------------------------------------------------------------
;;  This function is used after a layer change to set all LEDs to the
;;  values appr. to the layer
;; --------------------------------------------------------------------------
LC_LED_Refresh
	;; search in the LC_IO_TABLE (lc_io_table.inc) if a DOUT pin has been
	;; assigned to the function
	TABLE_ADDR LC_IO_TABLE_LAYER0	; init table pointer
	;; if layer1 selected, increment high byte (-> +256)
	btfsc	LC_STAT, LC_STAT_LAYER_SEL
	incf	TBLPTRH, F

	clrf	LC_LED_LOOP_CTR
LC_LED_Refresh_Loop
	tblrd*+				; read (and ignore) button entry
	tblrd*+				; read LED entry
	;; skip if bit 7 set (LED assigned to SFB or ignored)
	BRA_IFSET TABLAT, 7, ACCESS, LC_LED_Refresh_Loop_Next

	;; get pointer to LC function
	lfsr	FSR0, LED_STATUS_BEGIN
	movf	TABLAT, W
	andlw	0x7f
	addwf	FSR0L, F

	;; branch depending on saved status
	movf	INDF0, W
	bz	LC_LED_Refresh_Loop_LEDOff
LC_LED_Refresh_Loop_LEDOn
	movf	LC_LED_LOOP_CTR, W
	call	MIOS_DOUT_PinSet1
	rgoto	LC_LED_Refresh_Loop_Next

LC_LED_Refresh_Loop_LEDOff
	movf	LC_LED_LOOP_CTR, W
	call	MIOS_DOUT_PinSet0
	;; 	rgoto	LC_LED_Refresh_Loop_Next

LC_LED_Refresh_Loop_Next
	;; increment loop counter
	incf	LC_LED_LOOP_CTR, F
	;; loop until LC_LED_LOOP_CTR == 0x80 (bit #7 set)
	BRA_IFCLR LC_LED_LOOP_CTR, 7, ACCESS, LC_LED_Refresh_Loop

	return




;; --------------------------------------------------------------------------
;; user timer will be called every 20 ms (see USER_Init)
;; 
;; handles the meter counters. For the best show effect we don't decrement
;; all meter levels at once every 300 ms, but seperately
;; 	
;; every single counter will be reloaded by lc_mproc.inc::LC_MPROC_Received_D0
;; to 15 (== 300 ms / 20 ms), the levels will be decremented when this
;; value reaches zero
;; --------------------------------------------------------------------------
LC_LED_MeterTimer
	lfsr	FSR2, METER_LEVEL_BEGIN
	lfsr	FSR1, METER_CTR_BEGIN

	;; loop 8 times
	clrf	IRQ_TMP1
LC_LED_MeterTimer_Loop
	;; decrement METER_CTR[number], continue if != 0
	decf	INDF1, F
	bnz	LC_LED_MeterTimer_Loop_Next
	;; else reload counter
#if 0
	movlw	15		; according to spec
#else
	movlw	127		; slower (and nicer)
#endif
	movwf	INDF1
	;; and decrement meter level if != 0
	movf	INDF2, W
	andlw	0x0f		; (level in low-nibble)
	bz	LC_LED_MeterTimer_Loop_Next
	decf	INDF2, F
	;; request meter update
	movf	IRQ_TMP1, W
	call	MIOS_HLP_GetBitORMask
	iorwf	LC_METER_UPDATE_REQ, F
	;; if ledrings in ledmeters mode: update ledring pattern
	btfsc	LC_STAT, LC_STAT_LEDMETERS
	iorwf	LC_LEDRING_UPDATE_REQ, F

LC_LED_MeterTimer_Loop_Next
	;; increment pointers
	movf	POSTINC1, W
	movf	POSTINC2, W
	;; increment loop counter - we have to process 8 entries
	incf	IRQ_TMP1, F
	BRA_IFCLR IRQ_TMP1, 3, ACCESS, LC_LED_MeterTimer_Loop

	;; thats all
	return


;; --------------------------------------------------------------------------
;;  This handler services 8 LEDrings and/or 8 meters
;;  every variant has it's own code
;; --------------------------------------------------------------------------
LC_LED_DOUT_Handler

#if LEDRINGS_ENABLED == 0 && METERS_ENABLED == 0
	return
#endif

#if LEDRINGS_ENABLED != 0 && METERS_ENABLED == 0
	;; increment SRIO counter, wrap at 8
	;; (0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, ...)
	incf	SRIO_LED_CTR, W
	andlw	0x07
	movwf	SRIO_LED_CTR

	;; set shift register for the LED cathode, all others should be cleared
	;; this results into following bitpattern
	;;  LED Ring 0 -> b'11111110'
	;;  LED Ring 1 -> b'11111101'
	;;  LED Ring 2 -> b'11111011'
	;;  LED Ring 3 -> b'11110111'
	;;  LED Ring 4 -> b'11101111'
	;;  LED Ring 5 -> b'11011111'
	;;  LED Ring 6 -> b'10111111'
	;;  LED Ring 7 -> b'01111111'
	;; which is delivered by LC_SRIO_CathodeGet
	movf	SRIO_LED_CTR, W
	rcall	LC_SRIO_CathodeGet

	;; save pattern in MIOS_PARAMETER1
	movwf	MIOS_PARAMETER1

	;; move value to DOUT shift register number #8 directly
	movlw	LEDRINGS_SR_CATHODES
	call	MIOS_DOUT_SRSet

	;; now load the anode shift registers to set the ring patterns
	;; the patterns are saved in LEDRING_PATTERN_VPOT0_[LH]
	;; note: since this is an interrupt service routine, we have to use FSR2 instead of FSR0!
	lfsr	FSR2, LEDRING_PATTERN_VPOT0_L		; calc pointer to pattern
	clrc
	rlf	SRIO_LED_CTR, W				; add ringnumber * 2 (== ringnumber << 1) to pointer
	addwf	FSR2L, F

	movff	POSTINC2, MIOS_PARAMETER1		; transfer value of [INDF2] to MIOS_PARAMETER1
							; and increment INDF2 (-> POSTINC2)
	movlw	LEDRINGS_METERS_SR_ANODES_1		; write this pattern to shift register 10
	call	MIOS_DOUT_SRSet

	movff	INDF2, MIOS_PARAMETER1			; transfer value of [INDF2] to MIOS_PARAMETER1
							; don't increment here (not important...)
	movlw	LEDRINGS_METERS_SR_ANODES_2
	call	MIOS_DOUT_SRSet				; write this pattern to shift register 11
#endif


	;; 
	;; now without comments to make the code shorter!
	;; 

#if LEDRINGS_ENABLED == 0 && METERS_ENABLED != 0
	incf	SRIO_LED_CTR, W
	andlw	0x07
	movwf	SRIO_LED_CTR

	movf	SRIO_LED_CTR, W
	rcall	LC_SRIO_CathodeGet
	movwf	MIOS_PARAMETER1

	movlw	METERS_SR_CATHODES
	call	MIOS_DOUT_SRSet

	lfsr	FSR2, METER_PATTERN_VPOT0_L
	clrc
	rlf	SRIO_LED_CTR, W
	addwf	FSR2L, F
	movff	POSTINC2, MIOS_PARAMETER1
	movlw	LEDRINGS_METERS_SR_ANODES_1
	call	MIOS_DOUT_SRSet
	movff	INDF2, MIOS_PARAMETER1
	movlw	LEDRINGS_METERS_SR_ANODES_2
	call	MIOS_DOUT_SRSet
#endif

#if LEDRINGS_ENABLED != 0 && METERS_ENABLED != 0
	incf	SRIO_LED_CTR, W
	andlw	0x0f
	movwf	SRIO_LED_CTR

	movf	SRIO_LED_CTR, W
	rcall	LC_SRIO_CathodeGet
	movwf	MIOS_PARAMETER1

	BRA_IFSET SRIO_LED_CTR, 3, ACCESS, LC_LED_DOUT_Handler_Meters
LC_LED_DOUT_Handler_Rings
	movlw	LEDRINGS_SR_CATHODES
	call	MIOS_DOUT_SRSet
	setf	MIOS_PARAMETER1
	movlw	METERS_SR_CATHODES
	call	MIOS_DOUT_SRSet
	lfsr	FSR2, LEDRING_PATTERN_VPOT0_L
	rgoto	LC_LED_DOUT_Handler_Cont

LC_LED_DOUT_Handler_Meters
	movlw	METERS_SR_CATHODES
	call	MIOS_DOUT_SRSet
	setf	MIOS_PARAMETER1
	movlw	LEDRINGS_SR_CATHODES
	call	MIOS_DOUT_SRSet
	lfsr	FSR2, METER_PATTERN_VPOT0_L
LC_LED_DOUT_Handler_Cont
	
	clrc
	rlf	SRIO_LED_CTR, W
	andlw	0x0e
	addwf	FSR2L, F
	movff	POSTINC2, MIOS_PARAMETER1
	movlw	LEDRINGS_METERS_SR_ANODES_1
	call	MIOS_DOUT_SRSet
	movff	INDF2, MIOS_PARAMETER1
	movlw	LEDRINGS_METERS_SR_ANODES_2
	call	MIOS_DOUT_SRSet
#endif

	;; thats all!
	return


	;; help routine which returns the cathode line which has to be set for the appr. LED ring (0-7)
	;; note: we could also use the MIOS_HLP_GetBitANDMask routine which delivers exactly the same results
LC_SRIO_CathodeGet
	andlw	0x07
	JUMPTABLE_2BYTES_UNSECURE
	retlw	b'11111110'
	retlw	b'11111101'
	retlw	b'11111011'
	retlw	b'11110111'
	retlw	b'11101111'
	retlw	b'11011111'
	retlw	b'10111111'
	retlw	b'01111111'

