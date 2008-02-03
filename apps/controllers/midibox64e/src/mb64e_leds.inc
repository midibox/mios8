; $Id$
;
; MIDIbox64E
; LED handler
;
; ==========================================================================
;
;  Copyright 1998-2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================
	
;; --------------------------------------------------------------------------
;;  this function maps the physcial button number before it sets/clears a single LED
;;  IN: physcial button number of the button which controls the LED in MIOS_PARAMETER1
;;      value (0 or 1) in MIOS_PARAMETER2
;; --------------------------------------------------------------------------
MB64E_LED_SetMapped
	call	MB64E_BUTTON_Map
	bz	MB64E_LED_Set			; zero flag set if button mapped
	return					; exit if button not mapped

;; --------------------------------------------------------------------------
;;  this function sets/clears a single LED
;;  IN: number of button which controls the LED in MIOS_PARAMETER1
;;      value (0 or 1) in MIOS_PARAMETER2
;; --------------------------------------------------------------------------
MB64E_LED_Set
	rrf	MIOS_PARAMETER1, W
	rrf	WREG, W
	rrf	WREG, W
	andlw	0x07
	lfsr	FSR0, MB64E_BUTTON_VALUES_SR0
	addwf	FSR0L, F
	;; branch depending on value
	BRA_IFSET MIOS_PARAMETER2, 0, ACCESS, MB64E_LED_Set1
MB64E_LED_Set0
	;; clear LED
	movf	MIOS_PARAMETER1, W
	call	MIOS_HLP_GetBitANDMask
	andwf	INDF0, F
	rgoto	MB64E_LED_Update

MB64E_LED_Set1
	;; set LED
	movf	MIOS_PARAMETER1, W
	call	MIOS_HLP_GetBitORMask
	iorwf	INDF0, F
	rgoto	MB64E_LED_Update


;; --------------------------------------------------------------------------
;;  the DOUT map (values defined in main.asm header)
;; --------------------------------------------------------------------------
MB64E_LED_DOUT_MAP
	db	(DEFAULT_DOUT_SR_PIN_01_08 - 1) & 0xff, (DEFAULT_DOUT_SR_PIN_09_16 - 1) & 0xff
	db	(DEFAULT_DOUT_SR_PIN_17_24 - 1) & 0xff, (DEFAULT_DOUT_SR_PIN_25_32 - 1) & 0xff
	db	(DEFAULT_DOUT_SR_PIN_33_40 - 1) & 0xff, (DEFAULT_DOUT_SR_PIN_41_48 - 1) & 0xff
	db	(DEFAULT_DOUT_SR_PIN_49_56 - 1) & 0xff, (DEFAULT_DOUT_SR_PIN_57_64 - 1) & 0xff

;; --------------------------------------------------------------------------
;;  this function is called by cs_menu.inc on every "display" update
;; --------------------------------------------------------------------------
MB64E_LED_Update
	;; we don't want to overwrite MIOS_PARAMETER1, save it in TMP3
	movff	MIOS_PARAMETER1, TMP3

	;; ------------------------------------------------------------------
	;; copy selected DOUT values to MB64E_DATA_BUFFER[0..7] depending on LED map saved in dump
	;; ------------------------------------------------------------------
	SET_BSR	MB64E_BASE
	clrf	TMP1	; used as loop counter
MB64E_LED_Update_InitLoop
	;; get map setting
	lfsr	FSR0, MB64E_LED_MAP_BEGIN
	movf	TMP1, W
	movf	PLUSW0, W
	JUMPTABLE_2BYTES 24; entries
	rgoto	MB64E_LED_Update_Init_Default	; 0x00: returns MB64E_BUTTON_VALUES_SRx depending on TMP1
	rgoto	MB64E_LED_Update_Init_Button	; 0x01: returns MB64E_BUTTON_VALUES_SR0
	rgoto	MB64E_LED_Update_Init_Button	; 0x02: returns MB64E_BUTTON_VALUES_SR1
	rgoto	MB64E_LED_Update_Init_Button	; 0x03: returns MB64E_BUTTON_VALUES_SR2
	rgoto	MB64E_LED_Update_Init_Button	; 0x04: returns MB64E_BUTTON_VALUES_SR3
	rgoto	MB64E_LED_Update_Init_Button	; 0x05: returns MB64E_BUTTON_VALUES_SR4
	rgoto	MB64E_LED_Update_Init_Button	; 0x06: returns MB64E_BUTTON_VALUES_SR5
	rgoto	MB64E_LED_Update_Init_Button	; 0x07: returns MB64E_BUTTON_VALUES_SR6
	rgoto	MB64E_LED_Update_Init_Button	; 0x08: returns MB64E_BUTTON_VALUES_SR7
	rgoto	MB64E_LED_Update_Init_Group	; 0x09: returns selected group 1-8
	rgoto	MB64E_LED_Update_Init_Bank	; 0x0a: returns selected bank 1-8
	rgoto	MB64E_LED_Update_Init_Reserved	; 0x0b: returns 0x00
	rgoto	MB64E_LED_Update_Init_Reserved	; 0x0c: returns 0x00
	rgoto	MB64E_LED_Update_Init_Reserved	; 0x0d: returns 0x00
	rgoto	MB64E_LED_Update_Init_Reserved	; 0x0e: returns 0x00
	rgoto	MB64E_LED_Update_Init_Reserved	; 0x0f: returns 0x00
	rgoto	MB64E_LED_Update_Init_MButton	; 0x10: returns MB64E_MBUTTON_VALUES_SR0
	rgoto	MB64E_LED_Update_Init_MButton	; 0x11: returns MB64E_MBUTTON_VALUES_SR1
	rgoto	MB64E_LED_Update_Init_MButton	; 0x12: returns MB64E_MBUTTON_VALUES_SR2
	rgoto	MB64E_LED_Update_Init_MButton	; 0x13: returns MB64E_MBUTTON_VALUES_SR3
	rgoto	MB64E_LED_Update_Init_MButton	; 0x14: returns MB64E_MBUTTON_VALUES_SR4
	rgoto	MB64E_LED_Update_Init_MButton	; 0x15: returns MB64E_MBUTTON_VALUES_SR5
	rgoto	MB64E_LED_Update_Init_MButton	; 0x16: returns MB64E_MBUTTON_VALUES_SR6
	rgoto	MB64E_LED_Update_Init_MButton	; 0x17: returns MB64E_MBUTTON_VALUES_SR7

MB64E_LED_Update_Init_Reserved	; returns 0x00
	rgoto	MB64E_LED_Update_InitLoop_Cont

MB64E_LED_Update_Init_Default	; 0x00: returns MB64E_BUTTON_VALUES_SRx depending on TMP1
	lfsr	FSR0, MB64E_BUTTON_VALUES_SR0
	movf	TMP1, W
	comf	PLUSW0, W	; (invert)
	rgoto	MB64E_LED_Update_InitLoop_Cont

MB64E_LED_Update_Init_Button	; 0x01-0x08: returns MB64E_BUTTON_VALUES_SRx
	movf	TMP1, W
	decf	PLUSW0, W
	andlw	0x07
	lfsr	FSR0, MB64E_BUTTON_VALUES_SR0
	comf	PLUSW0, W	; (invert)
	rgoto	MB64E_LED_Update_InitLoop_Cont

MB64E_LED_Update_Init_Group	; 0x09: returns selected group 1-8
	SET_BSR	MB64E_BASE
	swapf	MB64E_GROUP_OFFSET, W, BANKED
	call	MIOS_HLP_GetBitORMask
	rgoto	MB64E_LED_Update_InitLoop_Cont

MB64E_LED_Update_Init_Bank	; 0x09: returns selected bank 1-8
	movf	MB_BANK, W
	call	MIOS_HLP_GetBitORMask
	rgoto	MB64E_LED_Update_InitLoop_Cont

MB64E_LED_Update_Init_MButton	; 0x10-0x17: returns MB64E_MBUTTON_VALUES_SRx
	movf	TMP1, W
	movf	PLUSW0, W
	andlw	0x07
	lfsr	FSR0, MB64E_MBUTTON_VALUES_SR0
	comf	PLUSW0, W	; (invert)
	;; 	rgoto	MB64E_LED_Update_InitLoop_Cont

MB64E_LED_Update_InitLoop_Cont
	movwf	TMP2
	lfsr	FSR0, MB64E_DATA_BUFFER
	movf	TMP1, W
	movff	TMP2, PLUSW0

	incf	TMP1, F
	BRA_IFCLR TMP1, 3, ACCESS, MB64E_LED_Update_InitLoop


	;; ------------------------------------------------------------------
	;; now transfer the LED values to the appr. DOUT registers
	;; ------------------------------------------------------------------
	TABLE_ADDR MB64E_LED_DOUT_MAP
	lfsr	FSR0, MB64E_DATA_BUFFER
	clrf	TMP1		; used as loop counter
MB64E_LED_Update_SRLoop
	;; we are using the MIOS_DOUT_SRSet function
	;; copy value to MIOS_PARAMETER1
	movff	POSTINC0, MIOS_PARAMETER1
	;; copy mapped DOUT shift register to WREG
	tblrd*+
	;; skip if DOUT disabled
	incf	TABLAT, W
	bz	MB64E_LED_Update_SRLoopNext
	movf	TABLAT, W	
	;; execute DOUT SR set function of MIOS
	call	MIOS_DOUT_SRSet
MB64E_LED_Update_SRLoopNext
	;; loop until last SR (#7) reached
	incf	TMP1, F
	BRA_IFCLR TMP1, 3, ACCESS, MB64E_LED_Update_SRLoop

	;; restore MIOS_PARAMETER1 from TMP3
	movff	TMP3, MIOS_PARAMETER1
	
	return
