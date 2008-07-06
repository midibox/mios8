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

	SET_BSR	SEQ_BASE

	;; use temporary registers which hold some LED flags which are not saved anywhere else
	;; (see also SEQ_IO_TABLE_DOUT)
	;; this quick & dirty method saves some memory...

	;; TMP4[3..0] == Selected Group 1-4
	movf	SEQ_SELECTED_GROUP, W, BANKED
	andlw	0x03
	call	MIOS_HLP_GetBitORMask
	movwf	TMP4

	;; TMP4[4..6] == Selected Trigger Layer A-C
	swapf	SEQ_SELECTED_TRG_LAYERS, W, BANKED
	andlw	0x70
	iorwf	TMP4, F

	movlw	0x00
	;; TMP5[0]: Sequencer Running (Play LED)
	btfsc	SEQ_STATE, SEQ_STATE_RUN, BANKED
	iorlw 0x01
	;; TMP5[1]: Sequencer not running (Stop LED)
	btfss	SEQ_STATE, SEQ_STATE_RUN, BANKED
	iorlw 0x02
	;; TMP5[2]: Sequencer paused (Pause LED)
	btfsc	SEQ_STATE, SEQ_STATE_PAUSE, BANKED
	iorlw 0x04
	;; TMP5[3]: Step 1-16 displayed
	btfss	SEQ_SELECTED_STEPS, 0, BANKED
	iorlw 0x08
	;; TMP5[4]: Step 17-32 displayed
	btfsc	SEQ_SELECTED_STEPS, 0, BANKED
	iorlw 0x10
	movwf	TMP5

	;; TMP1[6..0] == 1/2/3/4/A/B/C LEDs
	clrf	TMP1

	;; branch depending on mode
	BRA_IFCLR SEQ_MODE0, SEQ_MODE0_STEP_EDIT, BANKED, SEQ_LED_Update_NoEdit
SEQ_LED_Update_Edit
	;; 1/2/3/4 showing selected tracks
	lfsr	FSR1, SEQ_SELECTED_TRKS_G0
	movf	SEQ_SELECTED_GROUP, W, BANKED
	movf	PLUSW1, W
	andlw	0x0f
	movwf	TMP1
	;; A/B/C showing selected layers
	swapf	SEQ_SELECTED_LAYERS, W, BANKED
	andlw	0x70
	iorwf	TMP1, F
	rgoto	SEQ_LED_Update_Edit_Cont

SEQ_LED_Update_NoEdit
	;; mute mode: only display tracks
	BRA_IFCLR SEQ_MODE0, SEQ_MODE0_MUTE, BANKED, SEQ_LED_Update_NoMute
SEQ_LED_Update_Mute
	;; 1/2/3/4 showing selected tracks
	lfsr	FSR1, SEQ_SELECTED_TRKS_G0
	movf	SEQ_SELECTED_GROUP, W, BANKED
	movf	PLUSW1, W
	andlw	0x0f
	movwf	TMP1
	rgoto	SEQ_LED_Update_Edit_Cont
SEQ_LED_Update_NoMute

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
	;; we are using multiplications within a IRQ service handler - store PROD[LH]
	movff	PRODL, SEQ_IRQ_SAVED_PRODL
	movff	PRODH, SEQ_IRQ_SAVED_PRODH

	;; ---[ turn on the beat indicator LED on every beat ]---
	SET_BSR	SEQ_BASE
	clrf	MIOS_PARAMETER1
	BRA_IFCLR SEQ_STATE, SEQ_STATE_RUN, BANKED, SEQ_LED_UpdateIRQ_NoBeat
	movf	SEQ_CLK_STEP_CTR, W, BANKED
	andlw	0x0f
	addlw	-4		; lit for 4/16 of a quarter note
	bc	SEQ_LED_UpdateIRQ_NoBeat
SEQ_LED_UpdateIRQ_Beat
	setf	MIOS_PARAMETER1
SEQ_LED_UpdateIRQ_NoBeat
	;; w/o PWM dimmed LED
	movlw	DEFAULT_BEAT_INDICATOR_LED
	call	MIOS_DOUT_PinSet

	;; ---[ increment the BLM row counter (used when DEFAULT_SRM_ENABLED or DEFAULT_BPM_DIGITS_ENABLED set ]---
	incf	SEQ_BLM_ROW_CTR, F, BANKED
	bcf	SEQ_BLM_ROW_CTR, 2, BANKED	; wrap at 4

	;; ---[ update the GP LEDs ]---

	;; store selected track in IRQ_TMP5[3:0]
	call	CS_M_HLP_GetSelectedTrk
	movwf	IRQ_TMP5

	;; check if sequencer is running, and the current step position should be marked (either via XOR or second colour of DUO LED)
	;; notified via IRQ_TMP5[4] for GPLEDs
	;; notified via IRQ_TMP5[5] for matrix LEDs
	SET_BSR	SEQ_BASE
	BRA_IFCLR SEQ_STATE, SEQ_STATE_RUN, BANKED, SEQ_LED_UpdateIRQ_SeqOff
SEQ_LED_UpdateIRQ_SeqOn
	;; if pause: flash step LED
	BRA_IFCLR SEQ_STATE, SEQ_STATE_PAUSE, BANKED, SEQ_LED_UpdateIRQ_SeqOn_NoPause
	BRA_IFCLR CS_STAT, CS_STAT_CURSOR_FLASH, ACCESS, SEQ_LED_UpdateIRQ_SeqIgnore
SEQ_LED_UpdateIRQ_SeqOn_NoPause

	;; check for GPLEDs
SEQ_LED_UpdateIRQ_SeqOn_GPLED
	;; get current seq pos, store it in IRQ_TMP3
	lfsr	FSR1, SEQ_TRKVAR0
	movf	IRQ_TMP5, W	; (selected track = IRQ_TMP5[3:0])
	andlw	0x0f
	mullw	SEQ_TRKVARRECORD_LENGTH
	movf	PRODL, W
	addwf	FSR1L, F
	movf	PRODH, W
	addwfc	FSR1H, F
	movlw	SEQ_TRKVARSTEPx
	movff	PLUSW1, IRQ_TMP3

	;; don't print if played step not within view (1-16/17-31)
	BRA_IFSET SEQ_SELECTED_STEPS, 0, BANKED, SEQ_LED_UpdateIRQ_SeqOn_GPLED_S1
SEQ_LED_UpdateIRQ_SeqOn_GPLED_S0
	BRA_IFSET IRQ_TMP3, 4, ACCESS, SEQ_LED_UpdateIRQ_SeqOn_GPLED_NM
	rgoto	SEQ_LED_UpdateIRQ_SeqOn_GPLED_C

SEQ_LED_UpdateIRQ_SeqOn_GPLED_S1
	BRA_IFCLR IRQ_TMP3, 4, ACCESS, SEQ_LED_UpdateIRQ_SeqOn_GPLED_NM
	;; 	rgoto	SEQ_LED_UpdateIRQ_SeqOn_GPLED_C

SEQ_LED_UpdateIRQ_SeqOn_GPLED_C
	;; check if handshake disable set (which means: track not played due to handshaking)
	movlw	SEQ_TRKVARSTATE2x
	BRA_IFSET PLUSW1, SEQ_TRKVARSTATE2_HNDSHK_DIS, ACCESS, SEQ_LED_UpdateIRQ_SeqOn_GPLED_NM

	;; mark position
	bsf	IRQ_TMP5, 4
SEQ_LED_UpdateIRQ_SeqOn_GPLED_NM


#if DEFAULT_SRM_ENABLED
	;; check for matrix LEDs
SEQ_LED_UpdateIRQ_SeqOn_LM
	;; get matrix seq pos, store it in IRQ_TMP4
	lfsr	FSR1, SEQ_TRKVAR0
	movf	IRQ_TMP5, W	; (selected track group = IRQ_TMP5[3:2])
	andlw	0x0c
	addwf	SEQ_BLM_ROW_CTR, W, BANKED
	mullw	SEQ_TRKVARRECORD_LENGTH
	movf	PRODL, W
	addwf	FSR1L, F
	movf	PRODH, W
	addwfc	FSR1H, F
	movlw	SEQ_TRKVARSTEPx
	movff	PLUSW1, IRQ_TMP4

	;; don't print if played step not within view (1-16/17-31)
	BRA_IFSET SEQ_SELECTED_STEPS, 0, BANKED, SEQ_LED_UpdateIRQ_SeqOn_LM_S1
SEQ_LED_UpdateIRQ_SeqOn_LM_S0
	BRA_IFSET IRQ_TMP4, 4, ACCESS, SEQ_LED_UpdateIRQ_SeqOn_LM_NM
	rgoto	SEQ_LED_UpdateIRQ_SeqOn_LM_C

SEQ_LED_UpdateIRQ_SeqOn_LM_S1
	BRA_IFCLR IRQ_TMP4, 4, ACCESS, SEQ_LED_UpdateIRQ_SeqOn_LM_NM
	;; 	rgoto	SEQ_LED_UpdateIRQ_SeqOn_LM_C

SEQ_LED_UpdateIRQ_SeqOn_LM_C
	;; check if handshake disable set (which means: track not played due to handshaking)
	movlw	SEQ_TRKVARSTATE2x
	BRA_IFSET PLUSW1, SEQ_TRKVARSTATE2_HNDSHK_DIS, ACCESS, SEQ_LED_UpdateIRQ_SeqOn_LM_NM

	;; mark position
	bsf	IRQ_TMP5, 5
SEQ_LED_UpdateIRQ_SeqOn_LM_NM
#endif

SEQ_LED_UpdateIRQ_SeqIgnore
SEQ_LED_UpdateIRQ_SeqOff


#if DEFAULT_GP_DOUT_SR_L != DEFAULT_SRM_DOUT_L1
	;; ---[ update primary GP LEDs of the control surface ]---
	;; only used when Shift Registers of the primary GP LEDs are not equal to the multiplexed LED SRs
SEQ_LED_UpdateIRQ_GPLED
	;; transfer current LED pattern to IRQ_TMP[12]
	movff	SEQ_GP_LED_L, IRQ_TMP1
	movff	SEQ_GP_LED_R, IRQ_TMP2

	;; check if a position marker should be XORed
	BRA_IFCLR IRQ_TMP5, 4, ACCESS, SEQ_LED_UpdateIRQ_GPLED_NoMarker
SEQ_LED_UpdateIRQ_GPLED_Marker
	;; get xor mask and operate with IRQ_TMP[12] (pos[3] selects the register)
	movf	IRQ_TMP3, W
	call	MIOS_HLP_GetBitORMask

	btfss	IRQ_TMP3, 3
	xorwf	IRQ_TMP1, F
	btfsc	IRQ_TMP3, 3
	xorwf	IRQ_TMP2, F
SEQ_LED_UpdateIRQ_GPLED_NoMarker

	;; transfer LED value to DOUT registers
#if DEFAULT_GP_DOUT_SR_L
	movff	IRQ_TMP1, MIOS_PARAMETER1
	movlw	DEFAULT_GP_DOUT_SR_L - 1
	call	MIOS_DOUT_SRSet
#endif

#if DEFAULT_GP_DOUT_SR_R
	movff	IRQ_TMP2, MIOS_PARAMETER1
	movlw	DEFAULT_GP_DOUT_SR_R - 1
	call	MIOS_DOUT_SRSet
#endif
#endif



#if DEFAULT_SRM_ENABLED
	;; ---[ update LED matrix ]---
SEQ_LED_UpdateIRQ_LM
	;; transfer current LED pattern to IRQ_TMP[12]
	;; Track LED Row 1-4 - values stored in separate array
	lfsr	FSR1, SEQ_LEDM_TRK0_A_L
	rlf	SEQ_BLM_ROW_CTR, W, BANKED
	andlw	0xfe
	movff	PLUSW1, IRQ_TMP1
	addlw	1
	movff	PLUSW1, IRQ_TMP2

#if DEFAULT_SRM_DOUT_DUOCOLOUR == 0
	;; check if a position marker should be XORed
	BRA_IFCLR IRQ_TMP5, 5, ACCESS, SEQ_LED_UpdateIRQ_LM_NoMarker
SEQ_LED_UpdateIRQ_LM_Marker
	;; get xor mask and operate with IRQ_TMP[12] (pos[3] selects the register)
	movf	IRQ_TMP4, W
	call	MIOS_HLP_GetBitORMask

	btfss	IRQ_TMP4, 3
	xorwf	IRQ_TMP1, F
	btfsc	IRQ_TMP4, 3
	xorwf	IRQ_TMP2, F
SEQ_LED_UpdateIRQ_LM_NoMarker
#endif

	;; transfer LED value to DOUT registers
#if DEFAULT_SRM_DOUT_L1
	movff	IRQ_TMP1, MIOS_PARAMETER1
	movlw	DEFAULT_SRM_DOUT_L1 - 1
	call	MIOS_DOUT_SRSet
#endif
	
#if DEFAULT_SRM_DOUT_L1
	movff	IRQ_TMP2, MIOS_PARAMETER1
	movlw	DEFAULT_SRM_DOUT_R1 - 1
	call	MIOS_DOUT_SRSet
#endif

#if DEFAULT_SRM_DOUT_DUOCOLOUR
	;; drive the second colour LEDs
	lfsr	FSR1, SEQ_LEDM_TRK0_B_L
	rlf	SEQ_BLM_ROW_CTR, W, BANKED
	andlw	0xfe
	movff	PLUSW1, IRQ_TMP1
	addlw	1
	movff	PLUSW1, IRQ_TMP2

	;; if marker is requested (song position), invert the second colour LED
	BRA_IFCLR IRQ_TMP5, 5, ACCESS, SEQ_LED_UpdateIRQ_LM_NoMarker
SEQ_LED_UpdateIRQ_LM_Marker
	movf	IRQ_TMP4, W
	call	MIOS_HLP_GetBitORMask

	btfss	IRQ_TMP4, 3
	xorwf	IRQ_TMP1, F
	btfsc	IRQ_TMP4, 3
	xorwf	IRQ_TMP2, F
SEQ_LED_UpdateIRQ_LM_NoMarker

#if DEFAULT_SRM_DOUT_L2
	movff	IRQ_TMP1, MIOS_PARAMETER1
	movlw	DEFAULT_SRM_DOUT_L2 - 1
	call	MIOS_DOUT_SRSet
#endif

#if DEFAULT_SRM_DOUT_R2
	movff	IRQ_TMP2, MIOS_PARAMETER1
	movlw	DEFAULT_SRM_DOUT_R2 - 1
	call	MIOS_DOUT_SRSet
#endif
#endif

	;; set cathode line of current track
	;; if track active: cathode signal must be 0
	;; if track not active: cathode signal must be 1
	SET_BSR	SEQ_BASE
	movf	SEQ_BLM_ROW_CTR, W, BANKED
	call	MIOS_HLP_GetBitANDMask
	andlw	0x0f
	movwf	MIOS_PARAMETER1

	;; duplicate for button matrix
	swapf	WREG, W
	iorwf	MIOS_PARAMETER1, F

#if DEFAULT_SRM_DOUT_CATHODES1
	movlw	DEFAULT_SRM_DOUT_CATHODES1 - 1
	call	MIOS_DOUT_SRSet
#endif
#if DEFAULT_SRM_DOUT_CATHODES2
	movlw	DEFAULT_SRM_DOUT_CATHODES2 - 1
	call	MIOS_DOUT_SRSet
#endif

#endif


	;; ---[ update the BPM digits ]---
#if DEFAULT_BPM_DIGITS_ENABLED
	;; select common pin
	SET_BSR	SEQ_BASE
	movf	SEQ_BLM_ROW_CTR, W, BANKED
	xorlw	3			; (reverse digit select lines)
#if DEFAULT_BPM_DIGITS_ENABLED == 1
	call	MIOS_HLP_GetBitANDMask	; common cathode
#else
	call	MIOS_HLP_GetBitORMask	; common anode
#endif
	andlw	0x0f
	movwf	MIOS_PARAMETER1

	;; duplicate (just because the remaining pins are not used for any other purpose)
	swapf	WREG, W
	iorwf	MIOS_PARAMETER1, F

#if DEFAULT_BPM_DIGITS_COMMON
	movlw	DEFAULT_BPM_DIGITS_COMMON - 1
	call	MIOS_DOUT_SRSet
#endif

	;; set digit
	lfsr	FSR1, BPM_DIGIT_0
	movf	SEQ_BLM_ROW_CTR, W, BANKED
	movff	PLUSW1, MIOS_PARAMETER1
#if DEFAULT_BPM_DIGITS_ENABLED == 2
	comf	MIOS_PARAMETER1, F	; invert for common anode
#endif

#if DEFAULT_BPM_DIGITS_SEGMENTS
	movlw	DEFAULT_BPM_DIGITS_SEGMENTS - 1
	call	MIOS_DOUT_SRSet
#endif
	
#endif

SEQ_LED_UpdateIRQ_End
	;; restore PROD[LH]
	movff	SEQ_IRQ_SAVED_PRODL, PRODL
	movff	SEQ_IRQ_SAVED_PRODH, PRODH
	return
