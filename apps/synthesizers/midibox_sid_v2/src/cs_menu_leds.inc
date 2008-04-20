; $Id$
;
; SID Control Surface LED Handler
;
; ==========================================================================
;
;  Copyright 1998-2007 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function is called by the menu handler in cs_menu.inc
;;  when all LEDs should be updated
;;  The DOUT pin numbers are defined in cs_menu_io_tables.inc
;; --------------------------------------------------------------------------
CS_MENU_LED_Update
	;; get current CS_MENU_SID and base address
	call	CS_MENU_MS_GetSIDNumber

	;; copy the modulation matrix to CS_MENU_MATRIX
	;; this update is only required when CS_MENU_MODE_MATRIX_METER_DISP flag is not set
	;; (checked within function)
	rcall	CS_MENU_LED_Update_ModMatrix

	;; init temporary registers which hold some LED flags which are not saved anywhere else
	;; (see also CS_MENU_DOUT_TABLE)
	;; this quick & dirty method saves some memory...

	;; in bassline mode: selected OSC flag reflects L/R channel
	;; (it isn't used at all in this mode)
	movff	SID_EDIT_BUFFER + SID_Ix_ENGINE, WREG
	andlw	0x03
	xorlw	0x01
	bz	CS_MENU_LED_Update_OSC_B
	rgoto	CS_MENU_LED_Update_OSC_LDM
CS_MENU_LED_Update_OSC_B
	movf	CS_MENU_SELECTED_SID_LR, W
	movwf	CS_MENU_SELECTED_OSC_FLAGS
CS_MENU_LED_Update_OSC_LDM

	;; TMP1[2..0]: selected OSC layer
	movf	CS_MENU_SELECTED_OSC_CTRL, W
	call	MIOS_HLP_GetBitORMask
	andlw	0x07
	movwf	TMP1

	;; skip waveform/sync/ring in drum mode
	movff	SID_EDIT_BUFFER + SID_Ix_ENGINE, WREG
	andlw	0x03
	xorlw	0x02
	bz	CS_MENU_LED_Update_WSR_Skip
CS_MENU_LED_Update_WSR
	;; TMP1[7..4]: waveforms
	movlw	SID_Ix_Vx_WAVEFORM
	movwf	CS_MENU_PARAMETER_IX
	call	CS_MENU_FSR0_Voice
	swapf	INDF0, W
	andlw	0xf0
	btfsc	INDF0, 4; (voice off)
	movlw 0x00
	iorwf	TMP1, F

	;; TMP2[1..0]: sync/ring	
	swapf	INDF0, W
	rrf	WREG, W
	andlw	0x03
CS_MENU_LED_Update_WSR_Skip

	;; TMP2[3..2]: matrix/meter mode
	btfss	CS_MENU_MODE, CS_MENU_MODE_MATRIX_METER
	iorlw 0x08
	btfsc	CS_MENU_MODE, CS_MENU_MODE_MATRIX_METER
	iorlw 0x04
	movwf	TMP2

	;; TMP2[4..5]: selected envelope
	;; in bassline/drum mode: L/R channel
	movff	SID_EDIT_BUFFER + SID_Ix_ENGINE, WREG
	andlw	0x03
	bz	CS_MENU_LED_Update_ENV_L
	xorlw	0x03
	bz	CS_MENU_LED_Update_ENV_M
CS_MENU_LED_Update_ENV_B
CS_MENU_LED_Update_ENV_D
	swapf	CS_MENU_SELECTED_SID_LR, W
	rgoto	CS_MENU_LED_Update_ENV_BD_Cont
CS_MENU_LED_Update_ENV_L
CS_MENU_LED_Update_ENV_M
	movf	CS_MENU_SELECTED_ENV, W
	call	MIOS_HLP_GetBitORMask
	swapf	WREG, W
CS_MENU_LED_Update_ENV_BD_Cont
	andlw	0x30
	iorwf	TMP2, F

	;; TMP2[6..7]: selected ENV Ctrl
	movf	CS_MENU_SELECTED_ENV_CTRL, W
	call	MIOS_HLP_GetBitORMask
	swapf	WREG, F
	rlf	WREG, F
	rlf	WREG, W
	andlw	0xc0
	iorwf	TMP2, F
	
	;; TMP3[7..0]: filter O[123E] and filter LP/BP/HP/3O
	movlw	SID_Ix_L_Fx_CHN_MODE
	movwf	CS_MENU_PARAMETER_IX
	call	CS_MENU_FSR0_Filter
	movff	INDF0, TMP3

	;; TMP4[5..0]: selected LFO
	movf	CS_MENU_SELECTED_LFO, W
	call	MIOS_HLP_GetBitORMask
	andlw	0x3f
	movwf	TMP4

	;; TMP4[7..6]: selected SID L/R (TMP4[7]=R, TMP4[6]=L)
	movf	CS_MENU_SELECTED_SID_LR, W
	swapf	WREG, F
	rlf	WREG, F
	rlf	WREG, W
	andlw	0xc0
	iorwf	TMP4, F

	;; TMP5[5..0]: LFO waveform
	movff	SID_EDIT_BUFFER + SID_Ix_ENGINE, WREG
	BRA_IFSET WREG, 1, ACCESS, CS_MENU_LED_Update_LFO_DM
CS_MENU_LED_Update_LFO_LB
	BRA_IFSET WREG, 0, ACCESS, CS_MENU_LED_Update_LFO_B
CS_MENU_LED_Update_LFO_L
	movlw	SID_Ix_LFOx_MODE
	rgoto	CS_MENU_LED_Update_LFO_L_Cont
CS_MENU_LED_Update_LFO_B
	movlw	SID_Ix_B_Vx_LFO1_MODE
	rgoto	CS_MENU_LED_Update_LFO_B_Cont
CS_MENU_LED_Update_LFO_DM
	BRA_IFSET WREG, 0, ACCESS, CS_MENU_LED_Update_LFO_M
CS_MENU_LED_Update_LFO_D
	movlw	0x00		; not used
	rgoto	CS_MENU_LED_Update_LFO_Cont
CS_MENU_LED_Update_LFO_M
	movlw	SID_Ix_M_Vx_LFO1_MODE
CS_MENU_LED_Update_LFO_L_Cont
CS_MENU_LED_Update_LFO_B_Cont
	movwf	CS_MENU_PARAMETER_IX
	call	CS_MENU_FSR0_LFO
	swapf	INDF0, W
	andlw	0x0f
	rcall	CS_MENU_LED_LFO_Wav_Pattern
	btfss	INDF0, 0
	movlw 0x00
	;; 	rgoto	CS_MENU_LED_Update_LFO_Cont
CS_MENU_LED_Update_LFO_Cont
	movwf	TMP5


	;; TMP5[6]: SID play state
	movf	TMP5, W
	andlw	0x3f
	btfsc	CS_STAT2, CS_STAT2_PLAY_SID1
	iorlw 0x40
	btfsc	CS_STAT2, CS_STAT2_PLAY_SID2
	iorlw 0x40
	btfsc	CS_STAT2, CS_STAT2_PLAY_SID3
	iorlw 0x40
	btfsc	CS_STAT2, CS_STAT2_PLAY_SID4
	iorlw 0x40
	movwf	TMP5

	;; TMP5[7]: Env Misc LED
	btfsc	CS_MENU_SELECTED_ENV_CTRL, 1
	bsf	TMP5, 7

	;; go through the DOUT map
	TABLE_ADDR CS_MENU_DOUT_TABLE
CS_MENU_LED_Update_Loop
	tblrd*+				; get low-byte of register address
	movf	TABLAT, W		; load it in WREG
	movwf	FSR0L			; and store it in FSR0L
	tblrd*+				; get high byte
	movff	TABLAT, FSR0H		; store it in FSR0H without touching WREG
	iorwf	TABLAT, W		; or the byte with WREG --- if both bytes are zero, the result will be zero
	bz	CS_MENU_LED_Update_End	; in this case jump to the end

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
#if DEFAULT_SRM_LEDS_DOUT
	CALL_IFSET TABLAT, 7, ACCESS, CS_MENU_MATRIX_PinSet	; or pin of LED matrix if bit #7 set
#else
	CALL_IFSET TABLAT, 7, ACCESS, J5_IO_PinSet		; or pin of J5 if bit #7 set (SR #0 in setup_*.asm)
#endif
	rgoto	CS_MENU_LED_Update_Loop	; next entry

CS_MENU_LED_Update_End
	return


;; --------------------------------------------------------------------------
;; this function updates the modulation matrix when not in METER_DISP mode
;; --------------------------------------------------------------------------
CS_MENU_LED_Update_ModMatrix
	;; not in SID player mode (special handling in )
	btfsc	SID_STAT, SID_STAT_SIDPLAYER_CS_DISABLE
	return

	lfsr	FSR1, CS_MENU_MATRIX_BEGIN

	;; set LEDs of modulation matrix depending on selected engine
	movff	SID_EDIT_BUFFER + SID_Ix_ENGINE, WREG
	BRA_IFSET WREG, 1, ACCESS, CS_MENU_LED_Update_ModMatrix_DM
CS_MENU_LED_Update_ModMatrix_LB
	BRA_IFSET WREG, 0, ACCESS, CS_MENU_LED_Update_ModMatrix_B
CS_MENU_LED_Update_ModMatrix_L
	;; don't use level meters
	bcf	CS_STAT2, CS_STAT2_LEVEL_METERS

	;; exit if in MOD meter mode
	btfsc	CS_MENU_MODE, CS_MENU_MODE_MATRIX_METER_DISP
	return

	BRA_IFSET CS_MENU_SELECTED_SID_LR, 0, ACCESS, CS_MENU_LED_Update_ModMatrix_L_L

CS_MENU_LED_Update_ModMatrix_L_R
	movff	SID_EDIT_BUFFER + SID_Ix_L_MOD1_BASE + SID_Ix_MODx_TARG_R, POSTINC1
	movff	SID_EDIT_BUFFER + SID_Ix_L_MOD2_BASE + SID_Ix_MODx_TARG_R, POSTINC1
	movff	SID_EDIT_BUFFER + SID_Ix_L_MOD3_BASE + SID_Ix_MODx_TARG_R, POSTINC1
	movff	SID_EDIT_BUFFER + SID_Ix_L_MOD4_BASE + SID_Ix_MODx_TARG_R, POSTINC1
	movff	SID_EDIT_BUFFER + SID_Ix_L_MOD5_BASE + SID_Ix_MODx_TARG_R, POSTINC1
	movff	SID_EDIT_BUFFER + SID_Ix_L_MOD6_BASE + SID_Ix_MODx_TARG_R, POSTINC1
	movff	SID_EDIT_BUFFER + SID_Ix_L_MOD7_BASE + SID_Ix_MODx_TARG_R, POSTINC1
	movff	SID_EDIT_BUFFER + SID_Ix_L_MOD8_BASE + SID_Ix_MODx_TARG_R, POSTINC1
	return
	
CS_MENU_LED_Update_ModMatrix_L_L
	movff	SID_EDIT_BUFFER + SID_Ix_L_MOD1_BASE + SID_Ix_MODx_TARG_L, POSTINC1
	movff	SID_EDIT_BUFFER + SID_Ix_L_MOD2_BASE + SID_Ix_MODx_TARG_L, POSTINC1
	movff	SID_EDIT_BUFFER + SID_Ix_L_MOD3_BASE + SID_Ix_MODx_TARG_L, POSTINC1
	movff	SID_EDIT_BUFFER + SID_Ix_L_MOD4_BASE + SID_Ix_MODx_TARG_L, POSTINC1
	movff	SID_EDIT_BUFFER + SID_Ix_L_MOD5_BASE + SID_Ix_MODx_TARG_L, POSTINC1
	movff	SID_EDIT_BUFFER + SID_Ix_L_MOD6_BASE + SID_Ix_MODx_TARG_L, POSTINC1
	movff	SID_EDIT_BUFFER + SID_Ix_L_MOD7_BASE + SID_Ix_MODx_TARG_L, POSTINC1
	movff	SID_EDIT_BUFFER + SID_Ix_L_MOD8_BASE + SID_Ix_MODx_TARG_L, POSTINC1
	return

CS_MENU_LED_Update_ModMatrix_B
	;; exit if master is selected - realtime update done in SIDSE_B_CS_LM
	btfsc	CS_MENU_SELECTED_SID_FLAGS, 0
	return

	;; don't use level meters
	bcf	CS_STAT2, CS_STAT2_LEVEL_METERS

	;; clear matrix and meter values
	rcall	CS_MENU_LED_Update_Hlp_Clr

#if 0
	;; exit if meter node selected
	btfsc	CS_MENU_MODE, CS_MENU_MODE_MATRIX_METER_DISP
	return
#endif

	;; set sequencer position of left channel in horizontal line if sequencer enabled and valid pos
	SET_BSR	SID_EDIT_BUFFER
	BRA_IFCLR SID_EDIT_BUFFER + SID_Ix_B_S1V1_BASE + SID_Ix_B_Vx_FLAGS2, SID_I_B_V_FLAGS2_WT_ONLY, BANKED, CS_MENU_LED_Update_ModMatrix_BLD
	movf	SID_EDIT_BUFFER + SID_Ix_B_S1V1_BASE + SID_Ix_B_Vx_SEQ_NUM, W, BANKED
	andlw	0xf8
	bnz	CS_MENU_LED_Update_ModMatrix_BLD
CS_MENU_LED_Update_ModMatrix_BL
	movf	SID_EDIT_BUFFER + SID_Ix_B_S1V1_BASE + SID_Ix_B_Vx_SEQ_NUM, W, BANKED
	call	MIOS_HLP_GetBitORMask
	movwf	TMP2

	;; static bar
	lfsr	FSR1, CS_MENU_MATRIX_BEGIN
	movlw	8
	movwf	TMP1
CS_MENU_LED_Update_ModMatrix_BLL
	movff	TMP2, POSTINC1
	decfsz	TMP1, F
	rgoto	CS_MENU_LED_Update_ModMatrix_BLL
CS_MENU_LED_Update_ModMatrix_BLD

	;; set sequencer position of right channel in vertical line
	BRA_IFCLR SID_EDIT_BUFFER + SID_Ix_B_S2V1_BASE + SID_Ix_B_Vx_FLAGS2, SID_I_B_V_FLAGS2_WT_ONLY, BANKED, CS_MENU_LED_Update_ModMatrix_BRD
	movf	SID_EDIT_BUFFER + SID_Ix_B_S2V1_BASE + SID_Ix_B_Vx_SEQ_NUM, W, BANKED
	andlw	0xf8
	bnz	CS_MENU_LED_Update_ModMatrix_BRD
CS_MENU_LED_Update_ModMatrix_BR
	lfsr	FSR1, CS_MENU_MATRIX_BEGIN
	movf	SID_EDIT_BUFFER + SID_Ix_B_S2V1_BASE + SID_Ix_B_Vx_SEQ_NUM, W, BANKED
	andlw	0x07
	addwf	FSR1L, F

	;; static bar
	setf	INDF1
CS_MENU_LED_Update_ModMatrix_BRD

	return



CS_MENU_LED_Update_ModMatrix_DM
	BRA_IFSET WREG, 0, ACCESS, CS_MENU_LED_Update_ModMatrix_M
CS_MENU_LED_Update_ModMatrix_D
	;; exit if master is selected - realtime update done in SIDSE_D_CS_LM
	btfsc	CS_MENU_SELECTED_SID_FLAGS, 0
	return

	;; don't use level meters
	bcf	CS_STAT2, CS_STAT2_LEVEL_METERS

	;; clear matrix and meter values
	rcall	CS_MENU_LED_Update_Hlp_Clr

#if 0
	;; exit if meter node selected
	btfsc	CS_MENU_MODE, CS_MENU_MODE_MATRIX_METER_DISP
	return
#endif

	;; set sequencer position of left channel in horizontal line if sequencer enabled and valid pos
	SET_BSR	SID_EDIT_BUFFER
	BRA_IFCLR SID_EDIT_BUFFER + SID_Ix_D_SEQ_SPEED, SID_I_V_SEQ_ON, BANKED, CS_MENU_LED_Update_ModMatrix_DOf
	movf	SID_EDIT_BUFFER + SID_Ix_D_SEQ_NUM, W, BANKED
	andlw	0xf8
	bnz	CS_MENU_LED_Update_ModMatrix_DOf
CS_MENU_LED_Update_ModMatrix_DOn
	movf	SID_EDIT_BUFFER + SID_Ix_D_SEQ_NUM, W, BANKED
	call	MIOS_HLP_GetBitORMask
	movwf	TMP2

	;; static bar
	lfsr	FSR1, CS_MENU_MATRIX_BEGIN
	movlw	8
	movwf	TMP1
CS_MENU_LED_Update_ModMatrix_DL
	movff	TMP2, POSTINC1
	decfsz	TMP1, F
	rgoto	CS_MENU_LED_Update_ModMatrix_DL
CS_MENU_LED_Update_ModMatrix_DOf

	return


CS_MENU_LED_Update_ModMatrix_M
	;; exit if master is selected and we are in meter mote - realtime update done in SIDSE_M_CS_LM
	BRA_IFCLR CS_MENU_SELECTED_SID_FLAGS, 0, ACCESS, CS_MENU_LED_Update_ModMatrix_MOk

	;; clear matrix and meter values
	rcall	CS_MENU_LED_Update_Hlp_Clr

	btfsc	CS_MENU_MODE, CS_MENU_MODE_MATRIX_METER_DISP
	return

CS_MENU_LED_Update_ModMatrix_MOk
	;; don't use level meters
	bcf	CS_STAT2, CS_STAT2_LEVEL_METERS

	movf	CS_MENU_SELECTED_INS, W
	TABLE_ADDR_MUL_W CS_MENU_LED_MODMATRIX_NUMBERS, 8
	
	lfsr	FSR1, CS_MENU_MATRIX_BEGIN
	movlw	8		; copy 8 bytes
	movwf	PRODL
CS_MENU_LED_Update_ModMatrix_M_L
	tblrd*+
	movff	TABLAT, POSTINC1
	decfsz	PRODL, F
	rgoto	CS_MENU_LED_Update_ModMatrix_M_L
	return


	;; number 1-6 patterns
	;; (must be rotated by 90d)
	;; yes, this is based on the original C64 charset! :-)
CS_MENU_LED_MODMATRIX_NUMBERS
	;; ...**...
	;; ...**...
	;; ..***...
	;; ...**...
	;; ...**...
	;; ...**...
	;; .******.
	;; ........
	db	b'00000000', b'01000000', b'01000100', b'01111111', b'01111111', b'01000000', b'01000000', b'00000000'

	;; ..****..
	;; .**..**.
	;; .....**.
	;; ....**..
	;; ..**....
	;; .**.....
	;; .******.
	;; ........
	db	b'00000000', b'01100010', b'01110011', b'01010001', b'01001001', b'01001111', b'01000110', b'00000000'

	;; ..****..
	;; .**..**.
	;; .....**.
	;; ...***..
	;; .....**.
	;; .**..**.
	;; ..****..
	;; ........
	db	b'00000000', b'00100010', b'01100011', b'01001001', b'01001001', b'01111111', b'00110110', b'00000000'

	;; .....**.
	;; ....***.
	;; ...****.
	;; .**..**.
	;; .*******
	;; .....**.
	;; .....**.
	;; ........
	db	b'00000000', b'00011000', b'00011000', b'00010100', b'00010110', b'01111111', b'01111111', b'00010000'

	;; .******.
	;; .**.....
	;; .*****..
	;; .....**.
	;; .....**.
	;; .**..**.
	;; ..****..
	;; ........
	db	b'00000000', b'00100111', b'01100111', b'01000101', b'01000101', b'01111101', b'00111001', b'00000000'

	;; ..****..
	;; .**..**.
	;; .**.....
	;; .*****..
	;; .**..**.
	;; .**..**.
	;; ..****..
	;; ........
	db	b'00000000', b'00111110', b'01111111', b'01001001', b'01001001', b'01110011', b'00110010', b'00000000'


;; --------------------------------------------------------------------------
;; help function which clears the matrix/meter array
;; --------------------------------------------------------------------------
CS_MENU_LED_Update_Hlp_Clr
	lfsr	FSR1, CS_MENU_MATRIX_BEGIN
	lfsr	FSR2, METER_VALUES0
	clrf	PRODL
CS_MENU_LED_Update_Hlp_Clr_Loop
	clrf	POSTINC1
	clrf	POSTINC2
	incf	PRODL, F
	BRA_IFCLR PRODL, 3, ACCESS, CS_MENU_LED_Update_Hlp_Clr_Loop
	return

;; --------------------------------------------------------------------------
;; we have 5 LFO LEDs
;; this function maps the LFO waveform number to these LEDs
;; --------------------------------------------------------------------------
CS_MENU_LED_LFO_Wav_Pattern
	andlw	0x0f		; 16 entries
	JUMPTABLE_2BYTES_UNSECURE
	retlw	b'00000001'	; Sine
	retlw	b'00000010'	; Triangle
	retlw	b'00000100'	; Saw
	retlw	b'00001000'	; Pulse
	retlw	b'00010000'	; Random
#if DEFAULT_MB6582_CS
	retlw	b'00100001'	; Positive Sine
	retlw	b'00100010'	; Positive Triangle
	retlw	b'00100100'	; Positive Saw
	retlw	b'00101000'	; Positive Pulse
#else
	retlw	b'00010001'	; Positive Sine
	retlw	b'00010010'	; Positive Triangle
	retlw	b'00010100'	; Positive Saw
	retlw	b'00011000'	; Positive Pulse
#endif
	retlw	b'00011111'	; reserved
	retlw	b'00011111'	; reserved
	retlw	b'00011111'	; reserved
	retlw	b'00011111'	; reserved
	retlw	b'00011111'	; reserved
	retlw	b'00011111'	; reserved
	retlw	b'00011111'	; reserved
