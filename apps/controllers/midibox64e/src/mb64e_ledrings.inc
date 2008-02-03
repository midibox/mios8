; $Id$
;
; MIDIbox64E
; LEDrings handler
;
; ==========================================================================
;
;  Copyright 1998-2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  called by USER_SR_Service_Prepare
;;  Note that this is an interrupt service routine! Use FSR2 instead of FSR0
;;  and IRQ_TMPx instead of TMPx -- and make the routine as fast as possible
;; --------------------------------------------------------------------------
MB64E_LEDRINGS_Handler
	;; increment LED rings counter
	SET_BSR	MB64E_LEDRINGS_CTR
	incf	MB64E_LEDRINGS_CTR, W, BANKED
	andlw	0x0f
	movwf	MB64E_LEDRINGS_CTR, BANKED

	;; CTR2 used to animate patterns
	andlw	0x03
	skpnz
	incf	MB64E_LEDRINGS_CTR2, F, BANKED

	;; get shift register pattern for the LED cathodes
	;; this results into following bitpattern
	;;  LED Ring  0 -> b'1111111111111110'
	;;  LED Ring  1 -> b'1111111111111101'
	;;  LED Ring  2 -> b'1111111111111011'
	;;  LED Ring  3 -> b'1111111111110111'
	;;  LED Ring  4 -> b'1111111111101111'
	;;  LED Ring  5 -> b'1111111111011111'
	;;  LED Ring  6 -> b'1111111110111111'
	;;  LED Ring  7 -> b'1111111101111111'
	;;  LED Ring  8 -> b'1111111011111111'
	;;  LED Ring  9 -> b'1111110111111111'
	;;  LED Ring 10 -> b'1111101111111111'
	;;  LED Ring 11 -> b'1111011111111111'
	;;  LED Ring 12 -> b'1110111111111111'
	;;  LED Ring 13 -> b'1101111111111111'
	;;  LED Ring 14 -> b'1011111111111111'
	;;  LED Ring 15 -> b'0111111111111111'
	;; which is delivered by MB64E_LEDRINGS_CathodeGet_[LH]
	movf	MB64E_LEDRINGS_CTR, W, BANKED
	rcall	MB64E_LEDRINGS_CathodeGet_L
	movwf	IRQ_TMP1			; low-pattern saved in IRQ_TMP1

	movf	MB64E_LEDRINGS_CTR, W, BANKED
	rcall	MB64E_LEDRINGS_CathodeGet_H
	movwf	IRQ_TMP2			; high-pattern saved in IRQ_TMP2

	;; in fact we are serving up to 4 seperate LEDring units with 4 DOUT registers each
	;; in order to reduce the coding effort, we are defining a generic macro
	;; which is used 4 times with different parameters
LEDRINGS_HANDLER_MACRO MACRO enc_offset, LEDRINGS_SR_ENCx_CATHODES_1, LEDRINGS_SR_ENCx_CATHODES_2, LEDRINGS_SR_ENCx_ANODES_1, LEDRINGS_SR_ENCx_ANODES_2

	;; set cathode pattern
	movff	IRQ_TMP1, MIOS_PARAMETER1
	movlw	LEDRINGS_SR_ENCx_CATHODES_1
	call	MIOS_DOUT_SRSet

	movff	IRQ_TMP2, MIOS_PARAMETER1
	movlw	LEDRINGS_SR_ENCx_CATHODES_2
	call	MIOS_DOUT_SRSet

	;; now load the anode shift registers to set the ring patterns
	SET_BSR	MB64E_LEDRINGS_CTR
	movf	MB64E_LEDRINGS_CTR, W, BANKED
	addlw	enc_offset
	rcall	MB64E_LEDRINGS_AnodesGet
	
	movff	IRQ_TMP3, MIOS_PARAMETER1
	movlw	LEDRINGS_SR_ENCx_ANODES_1
	call	MIOS_DOUT_SRSet

	movff	IRQ_TMP4, MIOS_PARAMETER1
	movlw	LEDRINGS_SR_ENCx_ANODES_2
	call	MIOS_DOUT_SRSet
	ENDM

	;; now use the macro 4 times with different parameters
#if LEDRINGS_SR_ENC1_16_CATHODES_1 != 0 && LEDRINGS_SR_ENC1_16_CATHODES_2 != 0 && LEDRINGS_SR_ENC1_16_ANODES_1 != 0 && LEDRINGS_SR_ENC1_16_ANODES_2 != 0
	LEDRINGS_HANDLER_MACRO 0x00, LEDRINGS_SR_ENC1_16_CATHODES_1-1, LEDRINGS_SR_ENC1_16_CATHODES_2-1, LEDRINGS_SR_ENC1_16_ANODES_1-1, LEDRINGS_SR_ENC1_16_ANODES_2-1
#endif

#if LEDRINGS_SR_ENC17_32_CATHODES_1 != 0 && LEDRINGS_SR_ENC17_32_CATHODES_2 != 0 && LEDRINGS_SR_ENC17_32_ANODES_1 != 0 && LEDRINGS_SR_ENC17_32_ANODES_2 != 0
	LEDRINGS_HANDLER_MACRO 0x10, LEDRINGS_SR_ENC17_32_CATHODES_1-1, LEDRINGS_SR_ENC17_32_CATHODES_2-1, LEDRINGS_SR_ENC17_32_ANODES_1-1, LEDRINGS_SR_ENC17_32_ANODES_2-1
#endif

#if LEDRINGS_SR_ENC33_48_CATHODES_1 != 0 && LEDRINGS_SR_ENC33_48_CATHODES_2 != 0 && LEDRINGS_SR_ENC33_48_ANODES_1 != 0 && LEDRINGS_SR_ENC33_48_ANODES_2 != 0
	LEDRINGS_HANDLER_MACRO 0x20, LEDRINGS_SR_ENC33_48_CATHODES_1-1, LEDRINGS_SR_ENC33_48_CATHODES_2-1, LEDRINGS_SR_ENC33_48_ANODES_1-1, LEDRINGS_SR_ENC33_48_ANODES_2-1
#endif

#if LEDRINGS_SR_ENC49_64_CATHODES_1 != 0 && LEDRINGS_SR_ENC49_64_CATHODES_2 != 0 && LEDRINGS_SR_ENC49_64_ANODES_1 != 0 && LEDRINGS_SR_ENC49_64_ANODES_2 != 0
	LEDRINGS_HANDLER_MACRO 0x30, LEDRINGS_SR_ENC49_64_CATHODES_1-1, LEDRINGS_SR_ENC49_64_CATHODES_2-1, LEDRINGS_SR_ENC49_64_ANODES_1-1, LEDRINGS_SR_ENC49_64_ANODES_2-1
#endif
	;; thats all!
	return


	;; help routine which returns the cathode line which has to be set for the appr. LED ring offset+(0-16)
	;; note: we could also use the MIOS_HLP_GetBitANDMask routine which delivers exactly the same results
MB64E_LEDRINGS_CathodeGet_L
	btfsc	WREG, 3
	retlw b'11111111'
MB64E_LEDRINGS_CathodeGet_H_Cont
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

MB64E_LEDRINGS_CathodeGet_H
	btfss	WREG, 3
	retlw b'11111111'
	rgoto	MB64E_LEDRINGS_CathodeGet_H_Cont


MB64E_LEDRINGS_AnodesGet
	movwf	IRQ_TMP5
	BRA_IFSET MB_STAT, MB_STAT_SEL_GROUP_AND_BANK, ACCESS, MB64E_LEDRINGS_AnodesGet_GB
	BRA_IFSET MB_STAT2, MB_STAT2_SHOW_SELECTED, ACCESS, MB64E_LEDRINGS_AnodesGet_Last
	BRA_IFSET MB_STAT2, MB_STAT2_MORPH_MODE, ACCESS, MB64E_LEDRINGS_AnodesGet_Morph

	;; common mode: show ledring pattern
MB64E_LEDRINGS_AnodesGet_Common
	lfsr	FSR2, MB64E_ENC_VALUES_00

	;; special feature: map parameter value
	;; here we should normaly use MB64E_ENC_Map, but in order to save cycles I've duplicated the function
	lfsr	FSR1, MB64E_ENC_MAP_00
	movf	PLUSW1, W	; get entry index
	movwf	IRQ_TMP5

	movf	PLUSW2, W	; get entry value
MB64E_LEDRINGS_AnodesGet_ShowP
	andlw	0x7c
	addlw	MB64E_Presets_OffsetLEDPatterns & 0xff
	movwf	TBLPTRL

	lfsr	FSR2, MB64E_LEDRING_PATTERNS
	rrf	IRQ_TMP5, W
	andlw	0x3f

	btfss	IRQ_TMP5, 0
	movf	PLUSW2, W
	btfsc	IRQ_TMP5, 0
	swapf	PLUSW2, W
	btfsc	WREG, 0
	bsf	TBLPTRL, 7
	
	clrf	TBLPTRH
	btfsc	WREG, 1
	bsf	TBLPTRH, 0
	movlw	MB64E_Presets_OffsetLEDPatterns >> 8
	;; 	addwfc	TBLPTRH, F
	addwf	TBLPTRH, F

	tblrd*+
	movf	TABLAT, W
	tblrd*+
	btfsc	TABLAT, 0
	iorlw 0x80
	movwf	IRQ_TMP4

	tblrd*+
	movf	TABLAT, W
	tblrd*+
	btfsc	TABLAT, 0
	iorlw 0x80
	movwf	IRQ_TMP3
	return


	;; morphing mode: show the 8 morphing values + global value, all other rings off
MB64E_LEDRINGS_AnodesGet_Morph
	movlw	8+1
	cpfslt	IRQ_TMP5, ACCESS
	rgoto MB64E_LEDRINGS_AnodesGet_RingOff
	lfsr	FSR1, MB64E_MORPH_VALUES_G0
	movf	IRQ_TMP5, W
	movf	PLUSW1, W
	btfsc	IRQ_TMP5, 3
	movf	MB64E_MORPH_VALUE, W, BANKED
	rgoto	MB64E_LEDRINGS_AnodesGet_ShowP


	;; select group and bank/patch mode
	;; first ledring: show group offset
	;; second ledring: show bank number
	;; third ledring: show patch number
MB64E_LEDRINGS_AnodesGet_GB
	movlw	2+1
	cpfslt	IRQ_TMP5, ACCESS
	rgoto MB64E_LEDRINGS_AnodesGet_RingOff
	BRA_IFCLR IRQ_TMP5, 1, ACCESS, MB64E_LEDRINGS_AnodesGet_GB_01
MB64E_LEDRINGS_AnodesGet_GB_2
	movf	MB64E_SELECTED_PATCH, W, BANKED
	rgoto	MB64E_LEDRINGS_AnodesGet_ShowP
MB64E_LEDRINGS_AnodesGet_GB_01
#if DEFAULT_PATCHMODE == 0
#if DEFAULT_BANKSTICK_CS >= 0 && DEFAULT_BANKSTICK_CS <= 7
	swapf	MB64E_SELECTED_BANK, W, BANKED
#else
	rlf	MB64E_SELECTED_BANK, W, BANKED
#endif
#else
#if DEFAULT_BANKSTICK_CS >= 0 && DEFAULT_BANKSTICK_CS <= 7
	movlw	0x00
#else
	swapf	MB64E_SELECTED_BANK, W, BANKED
#endif
#endif
	btfss	IRQ_TMP5, 0
	movf	MB64E_GROUP_OFFSET, W, BANKED
	rgoto	MB64E_LEDRINGS_AnodesGet_ShowP


	;; "show last" mode: all rings off, except for the one which relates to MB64_ENC_LAST0
MB64E_LEDRINGS_AnodesGet_Last
	xorwf	MB64E_LAST_ENTRY, W, BANKED
	bnz	MB64E_LEDRINGS_AnodesGet_Last0
MB64E_LEDRINGS_AnodesGet_Last1
	BRA_IFSET MB_STAT2, MB_STAT2_SHOW_SEL_PATTERN, ACCESS, MB64E_LEDRINGS_AnodesGet_Last1P
MB64E_LEDRINGS_AnodesGet_Last1D	; rotate dot
	;; nice effect: rotate one LED
	swapf	MB64E_LEDRINGS_CTR2, W, BANKED
	call	MB64E_LEDRINGS_CathodeGet_H
	xorlw	0xff
	movwf	IRQ_TMP3

	swapf	MB64E_LEDRINGS_CTR2, W, BANKED
	call	MB64E_LEDRINGS_CathodeGet_L
	xorlw	0xff
	movwf	IRQ_TMP4
	return

MB64E_LEDRINGS_AnodesGet_Last1P	; rotate pattern
	;; map parameter
	movf	IRQ_TMP5, W
	lfsr	FSR1, MB64E_ENC_MAP_00
	movf	PLUSW1, W	; get entry index
	movwf	IRQ_TMP5
	
	movf	MB64E_LEDRINGS_CTR2, W, BANKED
	btfsc	MB64E_LEDRINGS_CTR2, 7, BANKED
	xorlw 0xff
	rgoto	MB64E_LEDRINGS_AnodesGet_ShowP
	
MB64E_LEDRINGS_AnodesGet_Last0
MB64E_LEDRINGS_AnodesGet_RingOff
	clrf	IRQ_TMP3
	clrf	IRQ_TMP4
	return
