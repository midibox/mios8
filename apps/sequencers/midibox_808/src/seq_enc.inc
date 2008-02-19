; $Id$
;
; MIDIboxSEQ
; Encoder handler
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function is called by USER_ENC_NotifyChange when the tempo encoder
;;  has been moved. Input:
;;     o signed incrementer value in MIOS_PARAMETER2:
;;       - is positive when encoder has been turned clockwise
;;       - is negative when encoder has been turned counter clockwise
;; --------------------------------------------------------------------------
SEQ_ENC_Tempo
	SET_BSR	SEQ_BASE
	
	;; get current BPM and copy into TMP[12]
	call	SEQ_BPM_Get
	addlw	-48
	movwf	TMP1
	clrf	TMP2

	;; inc/dec value by using the 16 bit add w/ saturation routine
	;; set pointer to CS_MENU_PARAMETER_L (is located to an even address, thats important when this function is used!)
	lfsr	FSR1, TMP1

	;; set max value
	movlw	255-48
	movwf	MIOS_PARAMETER1		; low-byte of max value
	movf	MIOS_PARAMETER2, W	; load incrementer before clearing MIOS_PARAMETER2
	clrf	MIOS_PARAMETER2		; high-byte of max value

	;; call routine
	call	MIOS_HLP_16bitAddSaturate
	;; now [FSR1] = INDF1 = CS_MENU_PARAMETER_[LH] contains the result
	;; MIOS_PARAMETER1[0] is set when value has been changed

	;; don't change BPM if parameter not changed
	BRA_IFCLR MIOS_PARAMETER1, 0, ACCESS, SEQ_ENC_Tempo_NoChange
SEQ_ENC_Tempo_Change
	;; set new BPM
	movf	TMP1, W
	addlw	48
	call	SEQ_BPM_Set
SEQ_ENC_Tempo_NoChange

	;; print temporary message if not in BPM menu
	movf	CS_MENU_POS, W
	xorlw	CS_MENU_PAGE_BPM
	bz	SEQ_ENC_Tempo_NoMsg
SEQ_ENC_Tempo_Msg
	TABLE_ADDR SEQ_ENC_TEMPO_STR_L
	call	MIOS_LCD_PrintMessage	; print left half
	call	SEQ_BPM_Get		; print BPM value
	call	MIOS_LCD_PrintBCD3
	TABLE_ADDR SEQ_ENC_TEMPO_STR_R
	call	MIOS_LCD_PrintMessage	; print right half
	movlw	0x80			; reduce message display time
	call	MIOS_LCD_MessageStart
SEQ_ENC_Tempo_NoMsg

	;; force display update
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ
	return

SEQ_ENC_TEMPO_STR_L STRING  7, 0x00 + 0,  "Tempo: "
SEQ_ENC_TEMPO_STR_R STRING 10, 0x00 + 10, " BPM      "




;; --------------------------------------------------------------------------
;;  This function is called by USER_ENC_NotifyChange when the instrument encoder
;;  has been moved. Input:
;;     o signed incrementer value in MIOS_PARAMETER2:
;;       - is positive when encoder has been turned clockwise
;;       - is negative when encoder has been turned counter clockwise
;; --------------------------------------------------------------------------
SEQ_ENC_Inst
	SET_BSR	SEQ_BASE

	;; get current track and copy into TMP[12]
	call	CS_M_HLP_GetSelectedTrk
	movwf	TMP1
	clrf	TMP2

	;; if all tracks selected: set TMP1 to 16
	incf	SEQ_SELECTED_TRKS_0, W, BANKED
	bnz	SEQ_ENC_Inst_NotAll
	incf	SEQ_SELECTED_TRKS_1, W, BANKED
	bnz	SEQ_ENC_Inst_NotAll
SEQ_ENC_Inst_All
	movlw	0x10
	movwf	TMP1
SEQ_ENC_Inst_NotAll	

	;; inc/dec value by using the 16 bit add w/ saturation routine
	;; set pointer to CS_MENU_PARAMETER_L (is located to an even address, thats important when this function is used!)
	lfsr	FSR1, TMP1

	;; set max value
	movlw	15+1
	movwf	MIOS_PARAMETER1		; low-byte of max value
	movf	MIOS_PARAMETER2, W	; load incrementer before clearing MIOS_PARAMETER2
	clrf	MIOS_PARAMETER2		; high-byte of max value

	;; call routine
	call	MIOS_HLP_16bitAddSaturate
	;; now [FSR1] = INDF1 = CS_MENU_PARAMETER_[LH] contains the result
	;; MIOS_PARAMETER1[0] is set when value has been changed

#if 0
	;; don't change instrument if parameter not changed
	BRA_IFCLR MIOS_PARAMETER1, 0, ACCESS, SEQ_ENC_Inst_NoChange
#endif
SEQ_ENC_Inst_Change
	;; set new instrument
	BRA_IFSET TMP1, 4, ACCESS, SEQ_ENC_Inst_ChangeAll
SEQ_ENC_Inst_ChangeNotAll
	movf	TMP1, W
	call	CS_M_HLP_SetSelectedTrk
	rgoto	SEQ_ENC_Inst_Cont

SEQ_ENC_Inst_ChangeAll
	SET_BSR	SEQ_BASE
	setf	SEQ_SELECTED_TRKS_0, BANKED
	setf	SEQ_SELECTED_TRKS_1, BANKED

	clrf	SEQ_SELECTED_TRK, BANKED	; (first track displayed)

	setf	CS_MENU_INST_OVERLAY_DOUT_SR0
	setf	CS_MENU_INST_OVERLAY_DOUT_SR1
	rgoto	SEQ_ENC_Inst_ChangeAll_Cont

SEQ_ENC_Inst_Cont	
SEQ_ENC_Inst_NoChange
SEQ_ENC_GP_Cont			; also used by SEQ_ENC_GP, expects instrument in TMP1
	;; temporary output instrument on GP LEDs
	clrf	CS_MENU_INST_OVERLAY_DOUT_SR0
	clrf	CS_MENU_INST_OVERLAY_DOUT_SR1
	
	movf	TMP1, W
	call	MIOS_HLP_GetBitORMask
	btfss	TMP1, 3
	movwf	CS_MENU_INST_OVERLAY_DOUT_SR0
	btfsc	TMP1, 3
	movwf	CS_MENU_INST_OVERLAY_DOUT_SR1

SEQ_ENC_Inst_ChangeAll_Cont
	movlw	0x80
	movwf	CS_MENU_INST_OVERLAY_CTR

	;; force display update
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ
	return

	return

;; --------------------------------------------------------------------------
;;  This function is called by USER_ENC_NotifyChange when a GP encoder has
;;  been moved (only relevant for MBSEQ V3 hardware). Input:
;;     o Encoder number in WREG and MIOS_PARAMETER1
;;     o signed incrementer value in MIOS_PARAMETER2:
;;       - is positive when encoder has been turned clockwise
;;       - is negative when encoder has been turned counter clockwise
;; --------------------------------------------------------------------------
SEQ_ENC_GP
	SET_BSR SEQ_BASE

	;; select track - do nothing else in MIDIbox 808 hardware
	;; (it isn't possible to change values with the GP encoders)
	movf	MIOS_PARAMETER1, W
	movwf	TMP1
	call	CS_M_HLP_SetSelectedTrk

	;; tempory output selected instrument on GP LEDs
	;; expects new track in TMP1 (already set above)
	rgoto	SEQ_ENC_GP_Cont


;; --------------------------------------------------------------------------
;;  This function sets the encoder speed depending on SEQ_MODE0, SEQ_MODE0_FAST_ENCODERS
;; --------------------------------------------------------------------------
SEQ_ENC_ConfigSpeed
	SET_BSR	SEQ_BASE

	;; tempo encoder speed divider
	movlw	DEFAULT_ENC_TEMPO_SPEED_VALUE
	btfss	SEQ_MODE0, SEQ_MODE0_FAST_ENCODERS, BANKED
	movlw	0x00
	movwf	MIOS_PARAMETER2

	;; speed mode
	movlw	MIOS_ENC_SPEED_FAST
	btfss	SEQ_MODE0, SEQ_MODE0_FAST_ENCODERS, BANKED
	movlw	MIOS_ENC_SPEED_NORMAL
	movwf	MIOS_PARAMETER1

	;; encoder number - set speed
	movlw	0x00
	call	MIOS_ENC_SpeedSet

	;; the remaining rotary encoders
	movlw	0x01
	movwf	TMP1
SEQ_ENC_ConfigSpeed_Loop
	;; speed divider
	movlw	DEFAULT_ENC_SPEED_VALUE		; button pressed
	btfss	SEQ_MODE0, SEQ_MODE0_FAST_ENCODERS, BANKED
	movlw	0x00
	movwf	MIOS_PARAMETER2

	;; speed mode
	movlw	MIOS_ENC_SPEED_FAST
	btfss	SEQ_MODE0, SEQ_MODE0_FAST_ENCODERS, BANKED
	movlw	MIOS_ENC_SPEED_NORMAL
	movwf	MIOS_PARAMETER1

	;; encoder number - set speed
	movf	TMP1, W
	call	MIOS_ENC_SpeedSet
	incf	TMP1, F
	movlw	18
	cpfseq	TMP1, ACCESS
	rgoto	SEQ_ENC_ConfigSpeed_Loop
	return


