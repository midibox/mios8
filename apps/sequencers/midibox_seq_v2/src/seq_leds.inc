; $Id$
;
; MIDIboxSEQ
; LED handler
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  this function is called by cs_menu.inc on every "display" update
;; --------------------------------------------------------------------------
SEQ_LED_Update
	;; update GP LEDs
	call	SEQ_GP_LED_Update

	;; use temporary registers which hold some LED flags which are not saved anywhere else
	;; (see also SEQ_IO_TABLE_DOUT)
	;; this quick & dirty method saves some memory...

	;; TMP1[6..0] == 1/2/3/4/A/B/C LEDs
	clrf	TMP1

	;; branch depending on mode
	BRA_IFCLR SEQ_MODE0, SEQ_MODE0_STEP_EDIT, BANKED, SEQ_LED_Update_NoEdit
SEQ_LED_Update_Edit
	;; 1/2/3/4 showing selected tracks
	movf	SEQ_SELECTED_TRKS, W, BANKED
	andlw	0x0f
	movwf	TMP1
	;; A/B/C showing selected layers
	swapf	SEQ_SELECTED_LAYERS, W, BANKED
	andlw	0x70
	iorwf	TMP1, F
	rgoto	SEQ_LED_Update_Edit_Cont

SEQ_LED_Update_NoEdit
	;; all LEDs off in mute mode
	BRA_IFSET SEQ_MODE0, SEQ_MODE0_MUTE, BANKED, SEQ_LED_Update_Edit_Cont

	;; in pattern/song mode: show cursor position
	movf	CS_MENU_CURSOR_POS, W
	btfsc	SEQ_MODE0, SEQ_MODE0_SONG, BANKED
	addlw -3
	call	MIOS_HLP_GetBitORMask
	andlw	0x0f
	btfsc	CS_STAT, CS_STAT_CURSOR_FLASH	; (flash LED - makes the different LED/button assignment more induitive)
	iorwf	TMP1, F

	;; in Song Mode: also overlay layer LEDs
	BRA_IFCLR SEQ_MODE0, SEQ_MODE0_SONG, BANKED, SEQ_LED_Update_Edit_Cont
	movf	CS_MENU_CURSOR_POS, W
	call	MIOS_HLP_GetBitORMask
	swapf	WREG, W
	andlw	0x70
	btfsc	CS_STAT, CS_STAT_CURSOR_FLASH	; (flash LED - makes the different LED/button assignment more induitive)
	iorwf	TMP1, F
SEQ_LED_Update_Edit_Cont

	;; TMP2 holds the SEQ_MODE0
	movff	SEQ_MODE0, TMP2
	;; TMP3 holds the SEQ_MODE1
	movff	SEQ_MODE1, TMP3

	;; extra: the song LED flashes if no song selected, and it is permanent on if song mode active
SEQ_LED_Update_SongLED
	btfsc	SEQ_STATE, SEQ_STATE_SONG, BANKED
	bsf	TMP2, SEQ_MODE0_SONG
	BRA_IFSET SEQ_STATE, SEQ_STATE_SONG, BANKED, SEQ_LED_Update_SongLED_NoFl
	btfss	CS_STAT, CS_STAT_CURSOR_FLASH
	bcf	TMP2, SEQ_MODE0_SONG
SEQ_LED_Update_SongLED_NoFl

	;; ----

	;; go through the DOUT map
	TABLE_ADDR SEQ_IO_TABLE_DOUT
SEQ_LED_Update_Loop
	tblrd*+				; get low-byte of register address
	movf	TABLAT, W		; load it in WREG
	movwf	FSR0L			; and store it in FSR0L
	tblrd*+				; get high byte
	movff	TABLAT, FSR0H		; store it in FSR0H without touching WREG
	iorwf	TABLAT, W		; or the byte with WREG --- if both bytes are zero, the result will be zero
	bz	SEQ_LED_Update_Loop_End	; in this case jump to the end

	tblrd*+				; get flag-number
	movf	TABLAT, W
	call	MIOS_HLP_GetBitORMask	; get appr. OR mask (1 of 8)
	andwf	INDF0, W		; AND this with the register specified in the table
	skpz				; LED value: 0 if result zero, 1 if result != zero
	movlw	0x01
	movwf	MIOS_PARAMETER1		; store result in MIOS_PARAMETER1
	tblrd*+				; read pin number from table
	movf	TABLAT, W		; get pin number
	CALL_IFCLR TABLAT, 7, ACCESS, MIOS_DOUT_PinSet	; and finally set the pin of SR if bit #7 cleared
	;; 	IFSET	TABLAT, 7, call J5_DOUT_PinSet		; or pin of J5 if bit #7 set (SR #0 in cs_menu_io_tables.inc)
	rgoto	SEQ_LED_Update_Loop	; next entry

SEQ_LED_Update_Loop_End
	return



;; --------------------------------------------------------------------------
;;  this function is called by USER_SR_ServicePrepare before the DOUT chains
;;  will be loaded
;; --------------------------------------------------------------------------
SEQ_LED_UpdateIRQ
	;; ---[ turn on the beat indicator LED on every beat ]---
	SET_BSR	SEQ_BASE
	clrf	MIOS_PARAMETER1
	BRA_IFCLR SEQ_STATE, SEQ_STATE_RUN, BANKED, SEQ_LED_UpdateIRQ_NoBeat
	movf	SEQ_CLK_STEP_CTR, W, BANKED
	andlw	0x03
	bnz	SEQ_LED_UpdateIRQ_NoBeat
SEQ_LED_UpdateIRQ_Beat
	setf	MIOS_PARAMETER1
SEQ_LED_UpdateIRQ_NoBeat
	movlw	DEFAULT_BEAT_INDICATOR_LED
	call	MIOS_DOUT_PinSet

	;; ---[ increment the LED row counter (used when DEFAULT_TRACK_LEDS_ENABLED set ]---
	incf	SEQ_STEP_LEDS_CTR, F, BANKED
	bcf	SEQ_STEP_LEDS_CTR, 2, BANKED	; wrap at 4

	;; ---[ update the GP LEDs ]---

	;; store selected track in IRQ_TMP4
	call	CS_M_HLP_GetSelectedTrk
	movwf	IRQ_TMP4

SEQ_LED_UpdateIRQ_GPLED
	;; transfer current LED pattern to IRQ_TMP[12]
	movff	SEQ_GP_LED0, IRQ_TMP1
	movff	SEQ_GP_LED1, IRQ_TMP2
#if DEFAULT_TRACK_LEDS_ENABLED
	movf	SEQ_STEP_LEDS_CTR, W, BANKED
	bz	SEQ_LED_UpdateIRQ_GPLED_0
SEQ_LED_UpdateIRQ_GPLED_Not0
	;; Track LED Row 2-4: show mute status (don't show GP status)
	lfsr	FSR1, SEQ_TRK0 + SEQ_TRKMUTED0x
	swapf	SEQ_STEP_LEDS_CTR, W, BANKED
	andlw	0xf0
	addwf	FSR1L, F
	comf	POSTINC1, W
	movwf	IRQ_TMP1
	comf	POSTINC1, W
	movwf	IRQ_TMP2
SEQ_LED_UpdateIRQ_GPLED_0
#endif

	;; if sequencer is running XOR position to GP pattern
	SET_BSR	SEQ_BASE
	BRA_IFCLR SEQ_STATE, SEQ_STATE_RUN, BANKED, SEQ_LED_UpdateIRQ_GPLED_SeqOff
SEQ_LED_UpdateIRQ_GPLED_SeqOn
	;; if pause: flash step LED
	BRA_IFCLR SEQ_STATE, SEQ_STATE_PAUSE, BANKED, SEQ_LED_UpdateIRQ_GPLED_SeqOnNP
	BRA_IFCLR CS_STAT, CS_STAT_CURSOR_FLASH, ACCESS, SEQ_LED_UpdateIRQ_GPLED_SeqIgn
SEQ_LED_UpdateIRQ_GPLED_SeqOnNP
	;; get current seq pos, store it in IRQ_TMP3
	lfsr	FSR1, SEQ_TRK0 + SEQ_TRKPOSx
#if DEFAULT_TRACK_LEDS_ENABLED
	movf	SEQ_STEP_LEDS_CTR, W, BANKED
#else
	movf	IRQ_TMP4, W	; (selected track)
#endif
	swapf	WREG, W
	andlw	0xf0
	addwf	FSR1L, F
	movf	INDF1, W
	movwf	IRQ_TMP3

	;; get xor mask and operate with IRQ_TMP[12] (pos[3] selects the register)
	call	MIOS_HLP_GetBitORMask

	btfss	IRQ_TMP3, 3
	xorwf	IRQ_TMP1, F
	btfsc	IRQ_TMP3, 3
	xorwf	IRQ_TMP2, F
SEQ_LED_UpdateIRQ_GPLED_SeqOff
SEQ_LED_UpdateIRQ_GPLED_SeqIgn

	;; in record mode, show also the record step
	BRA_IFCLR SEQ_MODE1, SEQ_MODE1_RECORD, BANKED, SEQ_LED_UpdateIRQ_GPLED_NoR
	BRA_IFSET CS_STAT, CS_STAT_CURSOR_FLASH, ACCESS, SEQ_LED_UpdateIRQ_GPLED_NoR
#if DEFAULT_TRACK_LEDS_ENABLED
	movf	IRQ_TMP4, W
	xorwf	SEQ_STEP_LEDS_CTR, W, BANKED
	bnz	SEQ_LED_UpdateIRQ_GPLED_NoR
#endif
	movf	SEQ_RECORD_STEP, W, BANKED
	call	MIOS_HLP_GetBitORMask

	btfss	SEQ_RECORD_STEP, 3, BANKED
	xorwf	IRQ_TMP1, F
	btfsc	SEQ_RECORD_STEP, 3, BANKED
	xorwf	IRQ_TMP2, F
SEQ_LED_UpdateIRQ_GPLED_NoR

	;; transfer LED value to DOUT registers
	movff	IRQ_TMP1, MIOS_PARAMETER1
	movlw	DEFAULT_GP_DOUT_SR0 - 1
	call	MIOS_DOUT_SRSet
	
	movff	IRQ_TMP2, MIOS_PARAMETER1
	movlw	DEFAULT_GP_DOUT_SR1 - 1
	call	MIOS_DOUT_SRSet

#if DEFAULT_TRACK_LEDS_ENABLED
	;; set cathode line of current track
	;; if track active: cathode signal must be 0
	;; if track not active: cathode signal must be 1
	SET_BSR	SEQ_BASE
	movf	SEQ_STEP_LEDS_CTR, W, BANKED
	call	MIOS_HLP_GetBitORMask
	movwf	IRQ_TMP1

	movlw	0x00
	btfss	IRQ_TMP1, 0
	movlw 0x01
	movwf	MIOS_PARAMETER1
	movlw	DEFAULT_TRACK_LEDS_1_CATHODE
	call	MIOS_DOUT_PinSet

	movlw	0x00
	btfss	IRQ_TMP1, 1
	movlw 0x01
	movwf	MIOS_PARAMETER1
	movlw	DEFAULT_TRACK_LEDS_2_CATHODE
	call	MIOS_DOUT_PinSet

	movlw	0x00
	btfss	IRQ_TMP1, 2
	movlw 0x01
	movwf	MIOS_PARAMETER1
	movlw	DEFAULT_TRACK_LEDS_3_CATHODE
	call	MIOS_DOUT_PinSet

	movlw	0x00
	btfss	IRQ_TMP1, 3
	movlw 0x01
	movwf	MIOS_PARAMETER1
	movlw	DEFAULT_TRACK_LEDS_4_CATHODE
	call	MIOS_DOUT_PinSet
#endif

	return


;; --------------------------------------------------------------------------
;;  this function sets/clears a LED which is related to a button
;;  used by the CS (see cs_menu.inc and cs_nenu_tables.inc) and
;;  by the SFB handler (mb64_sfb.inc)
;;  The "mapped" function has to be used if the button number is not
;;  mapped to the DIN chain (-> cs_menu.inc)
;;  IN: number of LED (related to the button) in MIOS_PARAMETER1
;;      value (0 or 1) in MIOS_PARAMETER2
;; --------------------------------------------------------------------------
SEQ_LED_Set
	;; (not used by MBSEQ)
	return


